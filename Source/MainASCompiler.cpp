#include "Core.h"
#include "App.h"

#include <stdio.h>
#include <locale.h>

#ifdef FO_LINUX
# include <unistd.h>
#endif

#include <angelscript.h>
#include <preprocessor.h>
#include <scriptany.h>
#include <scriptdictionary.h>
#include <scriptfile.h>
#include <scriptmath.h>
#include <scriptstring.h>
#include <scriptarray.h>

#include "ASCompiler.h"
#include "CommandLine.h"
#include "Debugger.h"
#include "Exception.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "Ini.h"
#include "ScriptBind.h"
#include "ScriptPragmas.h"
#include "ScriptUtils.h"
#include "Text.h"
#include "Timer.h"

using namespace std;

#ifdef FO_LINUX
# define _stricmp    strcasecmp
#endif

const string     ErrorNoCMS = "One of following options is required: --Client / --Mapper / --Server";

asIScriptEngine* Engine = NULL;
char*            Buf = NULL;
bool             CollectGarbage = false;

const char*      ContextStatesStr[] =
{
    "Finished",
    "Suspended",
    "Aborted",
    "Exception",
    "Prepared",
    "Uninitialized",
    "Active",
    "Error",
};

uint8         AppType = APP_TYPE_UNKNOWN;

Preprocessor* ScriptPreprocessor;

int Exit( int code, string error = string() )
{
    if( !error.empty() )
        printf( "ERROR: %s\n\n", error.c_str() );

    Timer::Finish();

    // Cleanup

    if( CommandLine )
        delete CommandLine;

    if( ScriptPreprocessor )
        delete ScriptPreprocessor;

    if( Engine )
        Engine->Release();

    if( Buf )
        delete Buf;

    return code;
}

int ExitUsage( string error = string() )
{
    if( !error.empty() )
        printf( "ERROR: %s\n\n", error.c_str() );

    printf( "FOClassic AngelScript compiler\n"
            "USAGE:\n"
            " Single Mode: ASCompiler [--Client/--Mapper/--Server] path/to/script.fos [...]\n"
            " Batch Mode:  ASCompiler --ScriptsCfg [--Client/--Mapper/--Server] [...]\n"
            // TODO "              ASCompiler --ScriptsCfg [path/to/scripts.cfg] [--Client/--Mapper/--Server] [...]\n"
            "\n"
            "  --Client (compile client script(s))\n"
            "  --Mapper (compile mapper script(s))\n"
            "  --Server (compile server script(s))\n"
            "\n"
            "SINGLE MODE OPTIONS:\n"
            "    --Output [preprocessor_output.txt]\n"
            "\n"
            // "BATCH MODE OPTIONS:\n"
            // TODO "    --Bind\n"
            // TODO "    --Import\n"
            // TODO "\n"
            "OTHER OPTIONS:\n"
            " *  --Define SOME_DEFINE *\n"
            "    --GC (collect garbage after execution)\n"
            " *  --Run function_name\n"
            "\n"
            " *  accepts multiple arugments separated by comma\n"
            );

    return Exit( error.empty() ? 0 : -1 );
}

void CompilerLog( ScriptString& str )
{
    printf( "%s\n", str.c_str() );
}

void PrintContextCallstack( asIScriptContext* ctx )
{
    int                      line, column;
    const asIScriptFunction* func;
    int                      stack_size = ctx->GetCallstackSize();
    printf( "State<%s>, call stack<%d>:\n", ContextStatesStr[(int)ctx->GetState()], stack_size );

    // Print current function
    if( ctx->GetState() == asEXECUTION_EXCEPTION )
    {
        line = ctx->GetExceptionLineNumber( &column );
        func = ctx->GetExceptionFunction();
    }
    else
    {
        line = ctx->GetLineNumber( 0, &column );
        func = ctx->GetFunction( 0 );
    }
    if( func )
        printf( "  %d) %s : %s : %d, %d.\n", stack_size - 1, func->GetModuleName(), func->GetDeclaration(), line, column );

    // Print call stack
    for( int i = 1; i < stack_size; i++ )
    {
        func = ctx->GetFunction( i );
        line = ctx->GetLineNumber( i, &column );
        if( func )
            printf( "  %d) %s : %s : %d, %d.\n", stack_size - i - 1, func->GetModuleName(), func->GetDeclaration(), line, column );
    }
}

void RunMain( asIScriptModule* module, string func_str )
{
    // Make function declaration
    char func_decl[1024];
    sprintf( func_decl, "void %s()", func_str.c_str() );

    // Run
    printf( "Executing '%s'.\n", func_str.c_str() );
    asIScriptContext*  ctx = Engine->CreateContext();
    asIScriptFunction* func = module->GetFunctionByDecl( func_decl );
    if( !func )
    {
        printf( "Function '%s' not found.\n", func_decl );
        return;
    }
    int result = ctx->Prepare( func );
    if( result < 0 )
    {
        printf( "Context preparation failure, error code<%d>.\n", result );
        return;
    }

    result = ctx->Execute();
    asEContextState state = ctx->GetState();
    if( state != asEXECUTION_FINISHED )
    {
        if( state == asEXECUTION_EXCEPTION )
            printf( "Execution of script stopped due to exception.\n" );
        else if( state == asEXECUTION_SUSPENDED )
            printf( "Execution of script stopped due to timeout.\n" );
        else
            printf( "Execution of script stopped due to %s.\n", ContextStatesStr[(int)state] );
        PrintContextCallstack( ctx );
        ctx->Abort();
        return;
    }

    if( result < 0 )
    {
        printf( "Execution error<%d>, state<%s>.\n", result, ContextStatesStr[(int)state] );
    }
    printf( "Execution finished.\n" );
}

void* GetScriptEngine()
{
    return Engine;
}

const char* GetDllTarget()
{
    return Str::Duplicate( App.TypeToName( AppType ).c_str() );
}

void CallBack( const asSMessageInfo* msg, void* param )
{
    const char* type = "ERROR";
    if( msg->type == asMSGTYPE_WARNING )
        type = "WARNING";
    else if( msg->type == asMSGTYPE_INFORMATION )
        type = "INFO";

    if( msg->type != asMSGTYPE_INFORMATION )
    {
        if( msg->row )
        {
            printf( "%s(%u) : %s : %s.\n", ScriptPreprocessor->ResolveOriginalFile( msg->row ).c_str(), ScriptPreprocessor->ResolveOriginalLine( msg->row ), type, msg->message );
        }
        else
        {
            printf( "%s : %s.\n", type, msg->message );
        }
    }
    else
    {
        printf( "%s(%u) : %s : %s.\n", ScriptPreprocessor->ResolveOriginalFile( msg->row ).c_str(), ScriptPreprocessor->ResolveOriginalLine( msg->row ), type, msg->message );
    }
}

int main( int argc, char** argv )
{
    CatchExceptions( "ASCompiler" );
    CommandLine = new CmdLine( argc, argv );

    // Initialization
    Timer::Init();
    double main_tick = Timer::AccurateTick();

    // Show usage
    if( argc < 2 )
        return ExitUsage();
    else if( !CommandLine->IsOption( "Client" ) && !CommandLine->IsOption( "Mapper" ) && !CommandLine->IsOption( "Server" ) )
        return ExitUsage( ErrorNoCMS );

    string     filename; // path to .fos file or scripts.cfg
    const bool batch_mode = CommandLine->IsOption( "ScriptsCfg" );

    // Batch mode
    if( batch_mode )
    {
        if( CommandLine->IsOptionEmpty( "ScriptsCfg" ) )
            filename = FILENAME_SCRIPTS_CONFIG;
        else
            filename = CommandLine->GetStr( "ScriptsCfg" ); // TODO change working directory

        if( CommandLine->IsOption( "Client" ) )
            AppType = APP_TYPE_CLIENT;
        else if( CommandLine->IsOption( "Mapper" ) )
            AppType = APP_TYPE_MAPPER;
        else if( CommandLine->IsOption( "Server" ) )
            AppType = APP_TYPE_SERVER;
        else
            return ExitUsage( ErrorNoCMS );
    }
    // Single mode (Client)
    else if( CommandLine->IsOption( "Client" ) )
    {
        if( CommandLine->IsOptionEmpty( "Client" ) )
            return ExitUsage( "Option --Client is empty" );

        filename = CommandLine->GetStr( "Client" );
        AppType = APP_TYPE_CLIENT;
    }
    // Single mode (Mapper)
    else if( CommandLine->IsOption( "Mapper" ) )
    {
        if( CommandLine->IsOptionEmpty( "Mapper" ) )
            return ExitUsage( "Option --Mapper is empty" );

        filename = CommandLine->GetStr( "Mapper" );
        AppType = APP_TYPE_MAPPER;
    }
    // Single mode (Server)
    else if( CommandLine->IsOption( "Server" ) )
    {
        if( CommandLine->IsOptionEmpty( "Server" ) )
            return ExitUsage( "Option --Server is empty" );

        filename = CommandLine->GetStr( "Server" );
        AppType = APP_TYPE_SERVER;
    }

    // all modes must have AppType set at this point
    if( AppType == APP_TYPE_UNKNOWN )
        return ExitUsage( ErrorNoCMS );

    vector<string> defines = CommandLine->GetStrVec( "Define", ',' );
    string         output = CommandLine->GetStr( "Output" );
    vector<string> run_func = CommandLine->GetStrVec( "Run", ',' );
    CollectGarbage = CommandLine->IsOption( "GC" );

    // Fix paths
    #if defined (FO_WINDOWS)
    replace( filename.begin(), filename.end(), '/', '\\' );
    replace( output.begin(), output.end(), '/', '\\' );
    #else
    replace( filename.begin(), filename.end(), '\\', '/' );
    replace( output.begin(), output.end(), '\\', '/' );
    #endif

    vector<string> scripts;
    if( batch_mode )
    {
        Ini* scripts_cfg = new Ini();
        scripts_cfg->KeepKeysOrder = true;

        if( !scripts_cfg->LoadFile( filename ) )
        {
            delete scripts_cfg;

            printf( "ERROR: Cannot load scripts configuration<%s>\n", filename.c_str() );
            return Exit( -1 );
        }

        string section_modules[3] =
        {
            SECTION_CLIENT_SCRIPTS_MODULES,
            SECTION_MAPPER_SCRIPTS_MODULES,
            SECTION_SERVER_SCRIPTS_MODULES
        };

        // not used currently, but required by Script::LoadConfigFile()
        string section_binds[3] =
        {
            SECTION_CLIENT_SCRIPTS_BINDS,
            SECTION_MAPPER_SCRIPTS_BINDS,
            SECTION_SERVER_SCRIPTS_BINDS
        };

        Ini* config = new Ini();
        if( !Script::LoadConfigFile( scripts_cfg, config, section_modules[AppType], section_binds[AppType] ) )
        {

            delete scripts_cfg;
            delete config;

            printf( "ERROR: Cannot parse scripts configuration<%s>\n", filename.c_str() );
            return Exit( -1 );
        }

        delete scripts_cfg;

        StrVec keys;
        if( !config->GetSectionKeys( section_modules[AppType], keys, true ) )
        {
            printf( "ERROR: No modules found; scripts configuration<%s>, target<%s>\n", filename.c_str(), App.TypeToName( AppType ) );
            delete config;

            return Exit( -1 );
        }

        delete config;

        for( auto it = keys.begin(), end = keys.end(); it != end; ++it )
        {
            string script_name = *it + ".fos";

            // Fix paths
            #if defined (FO_WINDOWS)
            replace( script_name.begin(), script_name.end(), '/', '\\' );
            #else
            replace( script_name.begin(), script_name.end(), '\\', '/' );
            #endif

            scripts.push_back( script_name );
        }
    }
    else
        scripts.push_back( filename );

    // Engine
    Engine = asCreateScriptEngine( ANGELSCRIPT_VERSION );
    if( !Engine )
        return Exit( -1, "Cannot create script engine" );

    Engine->SetMessageCallback( asFUNCTION( CallBack ), NULL, asCALL_CDECL );

    // Extensions
    RegisterScriptArray( Engine, true );
    RegisterScriptString( Engine );
    RegisterScriptAny( Engine );
    RegisterScriptDictionary( Engine );
    RegisterScriptFile( Engine );
    RegisterScriptMath( Engine );

    // Stuff for run func
    if( !run_func.empty() )
    {
        Engine->SetDefaultNamespace( "Engine" );       // TODO check result

        if( Engine->RegisterGlobalFunction( "void Printf(string& text)", asFUNCTION( CompilerLog ), asCALL_CDECL ) < 0 )
            printf( "Warning: cannot bind masking Log()." );

        Engine->SetDefaultNamespace( "" );
    }

    // Bind
    if( !Script::BindDummy::RegisterAll( Engine, AppType ) )
        return Exit( -1, "Cannot register script interface" );

    // Preprocessor
    ScriptPreprocessor = new Preprocessor();
    ScriptPreprocessor->SetPragmaCallback( new ScriptPragmaCallback( AppType ) );

    #pragma TODO("Script::DefineVersion()")
    ScriptPreprocessor->Define( "__VERSION", FOCLASSIC_VERSION_STRING );
    ScriptPreprocessor->Define( "FOCLASSIC_STAGE", FOCLASSIC_STAGE_STRING );
    ScriptPreprocessor->Define( "FOCLASSIC_VERSION", FOCLASSIC_VERSION_STRING );
    ScriptPreprocessor->Define( "ANGELSCRIPT_VERSION", std::to_string( (long long)(ANGELSCRIPT_VERSION) ) ); // make VS2010 happy

    ScriptPreprocessor->Define( "__ASCOMPILER" );
    ScriptPreprocessor->Define( string( "__" ) + App.TypeToName( AppType ) );

    for( auto it = defines.begin(), end = defines.end(); it != end; ++it )
        ScriptPreprocessor->Define( *it );

    if( !run_func.empty() )
        ScriptPreprocessor->Define( "Log", "Engine::Printf" );

    // Start compilation
    double preprocess_time = 0, build_time = 0, compile_time = 0, run_time = 0;
    for( auto it = scripts.begin(), end = scripts.end(); it != end; ++it )
    {
        const string& fos = *it;

        printf( "Compiling %s ...\n", fos.c_str() );

        double                        build_tick = 0, preprocess_tick = 0;
        double                        compile_tick = Timer::AccurateTick();

        Preprocessor::StringOutStream result, errors;
        int                           res;

        preprocess_tick = Timer::AccurateTick();
        res = ScriptPreprocessor->Preprocess( fos.c_str(), result, &errors, NULL, false );
        preprocess_tick = Timer::AccurateTick() - preprocess_tick;
        preprocess_time += preprocess_tick;

        Buf = Str::Duplicate( errors.String.c_str() );

        if( res )
        {
            printf( "Unable to preprocess. Errors:\n%s\n", Buf );
            return Exit( -1 );
        }
        else
        {
            if( Str::Length( Buf ) > 0 )
                printf( "%s", Buf );
        }

        if( !batch_mode && !output.empty() )
        {
            FILE* f = fopen( output.c_str(), "wt" );
            if( f )
            {
                string result_formatted = result.String;
                FormatPreprocessorOutput( result_formatted );
                fwrite( result_formatted.c_str(), sizeof(char), result_formatted.length(), f );
                fclose( f );
            }
            else
            {
                printf( "Unable to create preprocessed file<%s>.\n", output.c_str() );
            }
        }

        // Break buffer into null-terminated lines
        for( int i = 0; Buf[i] != '\0'; i++ )
            if( Buf[i] == '\n' )
                Buf[i] = '\0';

        // AS compilation
        build_tick = Timer::AccurateTick();
        asIScriptModule* module = Engine->GetModule( fos.c_str(), asGM_ALWAYS_CREATE );
        if( !module )
            return Exit( -1, "Can't create module" );

        if( module->AddScriptSection( NULL, result.String.c_str() ) < 0 )
            return Exit( -1, "Unable to add scipt section" );

        if( module->Build() < 0 )
            return Exit( -1, "Unable to build" );
        build_tick = Timer::AccurateTick() - build_tick;
        build_time += build_tick;


        // Check global not allowed types, only for server
        if( AppType == APP_TYPE_SERVER )
        {
            int bad_typeids[] =
            {
                Engine->GetTypeIdByDecl( "Critter@" ),
                Engine->GetTypeIdByDecl( "Critter@[]" ),
                Engine->GetTypeIdByDecl( "Item@" ),
                Engine->GetTypeIdByDecl( "Item@[]" ),
                Engine->GetTypeIdByDecl( "Map@" ),
                Engine->GetTypeIdByDecl( "Map@[]" ),
                Engine->GetTypeIdByDecl( "Location@" ),
                Engine->GetTypeIdByDecl( "Location@[]" ),
                Engine->GetTypeIdByDecl( "GameVar@" ),
                Engine->GetTypeIdByDecl( "GameVar@[]" ),
                Engine->GetTypeIdByDecl( "CraftItem@" ),
                Engine->GetTypeIdByDecl( "CraftItem@[]" ),
            };
            int bad_typeids_count = sizeof(bad_typeids) / sizeof(int);
            for( int k = 0; k < bad_typeids_count; k++ )
                bad_typeids[k] &= asTYPEID_MASK_SEQNBR;

            vector<int> bad_typeids_class;
            for( int m = 0, n = module->GetObjectTypeCount(); m < n; m++ )
            {
                asIObjectType* ot = module->GetObjectTypeByIndex( m );
                for( int i = 0, j = ot->GetPropertyCount(); i < j; i++ )
                {
                    int type = 0;
                    ot->GetProperty( i, NULL, &type, NULL, NULL );
                    type &= asTYPEID_MASK_SEQNBR;
                    for( int k = 0; k < bad_typeids_count; k++ )
                    {
                        if( type == bad_typeids[k] )
                        {
                            bad_typeids_class.push_back( ot->GetTypeId() & asTYPEID_MASK_SEQNBR );
                            break;
                        }
                    }
                }
            }

            bool g_fail = false;
            bool g_fail_class = false;
            for( int i = 0, j = module->GetGlobalVarCount(); i < j; i++ )
            {
                int type = 0;
                module->GetGlobalVar( i, NULL, NULL, &type, NULL );

                while( type & asTYPEID_TEMPLATE )
                {
                    asIObjectType* obj = (asIObjectType*)Engine->GetObjectTypeById( type );
                    if( !obj )
                        break;
                    type = obj->GetSubTypeId();
                }

                type &= asTYPEID_MASK_SEQNBR;

                for( int k = 0; k < bad_typeids_count; k++ )
                {
                    if( type == bad_typeids[k] )
                    {
                        const char* name = NULL;
                        module->GetGlobalVar( i, &name, NULL, NULL );
                        string      msg = "The global variable '" + string( name ) + "' uses a type that cannot be stored globally";
                        Engine->WriteMessage( "", 0, 0, asMSGTYPE_ERROR, msg.c_str() );
                        g_fail = true;
                        break;
                    }
                }
                if( std::find( bad_typeids_class.begin(), bad_typeids_class.end(), type ) != bad_typeids_class.end() )
                {
                    const char* name = NULL;
                    module->GetGlobalVar( i, &name, NULL, NULL );
                    string      msg = "The global variable '" + string( name ) + "' uses a type in class property that cannot be stored globally";
                    Engine->WriteMessage( "", 0, 0, asMSGTYPE_ERROR, msg.c_str() );
                    g_fail_class = true;
                }
            }

            if( g_fail || g_fail_class )
            {
                if( !g_fail_class )
                    printf( "Erase global variable listed above.\n" );
                else
                    printf( "Erase global variable or class property listed above.\n" );
                printf( "Classes that cannot be stored in global scope: Critter, Item, ProtoItem, Map, Location, GlobalVar.\n" );
                printf( "Hint: store their Ids, instead of pointers.\n" );
                return Exit( -1 );
            }
        }

        compile_tick = Timer::AccurateTick() - compile_tick;
        compile_time += compile_tick;

        // Print compilation time
        printf( "Success\nTime: " );
        if( CommandLine->IsOption( "Time" ) )
            printf( "Preprocess %g ms, Build %g ms, Compile %g ms", preprocess_tick, build_tick, compile_tick );
        else
            printf( "%g ms", compile_tick );
        printf( "\n" );


        // Execute functions
        for( auto it = run_func.begin(), end = run_func.end(); it != end; ++it )
        {
            double run_tick = Timer::AccurateTick();
            RunMain( module, *it );
            run_time += Timer::AccurateTick() - run_tick;
        }
    }

    // Collect garbage
    double gc_time = 0;
    if( CollectGarbage )
    {
        gc_time = Timer::AccurateTick();
        Engine->GarbageCollect( asGC_FULL_CYCLE );
        gc_time = Timer::AccurateTick() - gc_time;
    }

    if( batch_mode && CommandLine->IsOption( "Time" ) )
    {
        printf( "\n" );
        printf( "Preprocess  %g ms\n", preprocess_time );
        printf( "Build       %g ms\n", build_time );
        printf( "Compile     %g ms\n", compile_time );
        printf( "Run         %g ms\n", run_time );
        printf( "GC          %g ms\n", gc_time );
        printf( "main()      %g ms\n\n", Timer::AccurateTick() - main_tick );
    }

    return Exit( 0 );
}
