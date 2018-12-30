#include "Core.h"

#include <stdio.h>
#include <locale.h>

#include <angelscript.h>
#include <preprocessor.h>
#include <scriptany.h>
#include <scriptdictionary.h>
#include <scriptfile.h>
#include <scriptmath.h>
#include <scriptstring.h>
#include <scriptarray.h>

#include "ASCompiler.h"
#include "Debugger.h"
#include "Exception.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "ScriptBind.hpp"
#include "ScriptPragmas.h"
#include "Text.h"
#include "Timer.h"

#ifdef FO_LINUX
# include <unistd.h>
# define _stricmp    strcasecmp
#endif

asIScriptEngine* Engine = NULL;
bool             IsClient = false;
bool             IsMapper = false;
bool             IsServer = true;
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

uchar  ScriptTarget = SCRIPT_BIND_SERVER; // TODO no default
string ScriptTargetName[4] =
{
    "UNKNOWN",
    "CLIENT",
    "MAPPER",
    "SERVER"
};

Preprocessor* ScriptPreprocessor;

int Exit( int code )
{
    // Clean up
    if( ScriptPreprocessor )
        delete ScriptPreprocessor;

    if( Engine )
        Engine->Release();

    if( Buf )
        delete Buf;

    return code;
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

void RunMain( asIScriptModule* module, const char* func_str )
{
    // Make function declaration
    char func_decl[1024];
    sprintf( func_decl, "void %s()", func_str );

    // Run
    printf( "Executing '%s'.\n", func_str );
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
    return ScriptTargetName[ScriptTarget].c_str();
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

int main( int argc, char* argv[] )
{
    CatchExceptions( "ASCompiler" );

    // Initialization
    Timer::Init();

    // Show usage
    if( argc < 2 )
    {
        printf( "FOClassic AngelScript compiler. Usage:\n"
                "ASCompiler script_name.fos\n"
                " [-client] (compile client scripts)\n"
                " [-mapper] (compile mapper scripts)\n"
                " [-p preprocessor_output.txt]\n"
                " [-d SOME_DEFINE]*\n"
                " [-run func_name]*\n"
                " [-gc] (collect garbage after execution)\n"
                "*can be used multiple times" );
        return 0;
    }

    // Parse args
    char*         str_fname = argv[1];
    char*         str_prep = NULL;
    vector<char*> defines;
    vector<char*> run_func;
    for( int i = 2; i < argc; i++ )
    {
        // Client / Mapper / Server
        if( !_stricmp( argv[i], "-client" ) )
            ScriptTarget = SCRIPT_BIND_CLIENT;
        else if( !_stricmp( argv[i], "-mapper" ) )
            ScriptTarget = SCRIPT_BIND_MAPPER;
        // Preprocessor output
        else if( !_stricmp( argv[i], "-p" ) && i + 1 < argc )
            str_prep = argv[++i];
        // Define
        else if( !_stricmp( argv[i], "-d" ) && i + 1 < argc )
            defines.push_back( argv[++i] );
        // Run function
        else if( !_stricmp( argv[i], "-run" ) && i + 1 < argc )
            run_func.push_back( argv[++i] );
        // Collect garbage
        else if( !_stricmp( argv[i], "-gc" ) )
            CollectGarbage = true;
    }

    // Fix path
    FixPathSlashes( str_fname );
    if( str_prep )
        FixPathSlashes( str_prep );

	// Engine
    Engine = asCreateScriptEngine( ANGELSCRIPT_VERSION );
    if( !Engine )
    {
        printf( "Register failed.\n" );
        return Exit( -1 );
    }
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
        if( Engine->RegisterGlobalFunction( "void __CompilerLog(string& text)", asFUNCTION( CompilerLog ), asCALL_CDECL ) < 0 )
            printf( "Warning: cannot bind masking Log()." );
    }

    // Bind
    if( !Script::BindDummy::RegisterAll( Engine, ScriptTarget ) )
    {
        printf( "Bind error.\n" );
        Exit( -1 );
    }

    // Start compilation
    printf( "Compiling %s ...\n", str_fname );
    double tick = Timer::AccurateTick();

    // Preprocessor
    ScriptPreprocessor = new Preprocessor();
    ScriptPreprocessor->SetPragmaCallback( new ScriptPragmaCallback( ScriptTarget ) );

    #pragma TODO("Script::DefineVersion()")
    ScriptPreprocessor->Define( "__VERSION " + FOCLASSIC_VERSION );
    ScriptPreprocessor->Define( "FOCLASSIC_STAGE " + FOCLASSIC_STAGE );
    ScriptPreprocessor->Define( "FOCLASSIC_VERSION " + FOCLASSIC_VERSION );
    ScriptPreprocessor->Define( "ANGELSCRIPT_VERSION " + ANGELSCRIPT_VERSION );

    ScriptPreprocessor->Define( "__ASCOMPILER" );
    ScriptPreprocessor->Define( "__" + ScriptTargetName[ScriptTarget] );

    for( size_t i = 0; i < defines.size(); i++ )
        ScriptPreprocessor->Define( string( defines[i] ) );
    if( !run_func.empty() )
        ScriptPreprocessor->Define( string( "Log __CompilerLog" ) );

    Preprocessor::StringOutStream result, errors;
    int                           res;
    res = ScriptPreprocessor->Preprocess( str_fname, result, &errors, NULL, false );

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

    if( str_prep )
    {
        FILE* f = fopen( str_prep, "wt" );
        if( f )
        {
            string result_formatted = result.String;
            FormatPreprocessorOutput( result_formatted );
            fwrite( result_formatted.c_str(), sizeof(char), result_formatted.length(), f );
            fclose( f );
        }
        else
        {
            printf( "Unable to create preprocessed file<%s>.\n", str_prep );
        }
    }

    // Break buffer into null-terminated lines
    for( int i = 0; Buf[i] != '\0'; i++ )
        if( Buf[i] == '\n' )
            Buf[i] = '\0';

    // AS compilation
    asIScriptModule* module = Engine->GetModule( 0, asGM_ALWAYS_CREATE );
    if( !module )
    {
        printf( "Can't create module.\n" );
        return Exit( -1 );
    }

    if( module->AddScriptSection( NULL, result.String.c_str() ) < 0 )
    {
        printf( "Unable to add section.\n" );
        return Exit( -1 );
    }

    if( module->Build() < 0 )
    {
        printf( "Unable to build.\n" );
        return Exit( -1 );
    }

    // Check global not allowed types, only for server
    if( ScriptTarget == SCRIPT_BIND_SERVER )
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

    // Print compilation time
    printf( "Success.\nTime: %g ms.\n", Timer::AccurateTick() - tick );

    // Execute functions
    for( size_t i = 0; i < run_func.size(); i++ )
        RunMain( module, run_func[i] );

    // Collect garbage
    if( CollectGarbage )
        Engine->GarbageCollect( asGC_FULL_CYCLE );

    return Exit( 0 );
}
