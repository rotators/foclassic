##
## https://cmake.org/cmake/help/v3.12/
##
cmake_minimum_required( VERSION 3.12.2 )

project( FOClassic
	DESCRIPTION  "FOnline Classic"
#	HOMEPAGE_URL ""
)

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_SKIP_INSTALL_RULES TRUE )
set( CMAKE_VERBOSE_MAKEFILE TRUE )

# directories
set( FO_DIR     "Source" )
set( ACM_DIR    "${FO_DIR}/Acm" )
set( AS_DIR     "${FO_DIR}/AngelScript" )
set( ASSIMP_DIR "${FO_DIR}/Assimp" )
set( DAT_DIR    "${FO_DIR}/DatFile" )
set( GLEW_DIR   "${FO_DIR}/GL" )
set( SHA2_DIR   "${FO_DIR}/SHA2" )
set( ZLIB_DIR   "${FO_DIR}/zlib" )

option( OPTION_USE_STLPORT            "Use StlPort" ON )
option( OPTION_USE_VANILLA_CLIENTSAVE "Use vanilla .client saving" ON )

##
## Parse options
##
message( STATUS "Checking options..." )
if( OPTION_USE_STLPORT )
	message( STATUS " StlPort .................. ${OPTION_USE_STLPORT}" )
	include_directories( BEFORE "Source/StlPort" )
	add_definitions( -DUSE_STLPORT=1 )
endif( OPTION_USE_STLPORT )

unset( OPTION_USE_STLPORT CACHE )
if( OPTION_USE_VANILLA_CLIENTSAVE )
	message( STATUS " Vanilla .client saving ... ${OPTION_USE_VANILLA_CLIENTSAVE}" )
	add_definitions( -DUSE_VANILLA_CLIENTSAVE=1 )
endif( OPTION_USE_VANILLA_CLIENTSAVE )
unset( OPTION_USE_VANILLA_CLIENTSAVE CACHE )

##
## Find threads
##
set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )
include( CheckStructHasMember )
check_struct_has_member( "struct timespec" "tv_sec" "time.h" HAVE_STRUCT_TIMESPEC )
if( HAVE_STRUCT_TIMESPEC )
	add_definitions( -DHAVE_STRUCT_TIMESPEC )
endif( HAVE_STRUCT_TIMESPEC )

if( MSVC )
	set( COMPILER_DEFINES "" )
	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_CRT_SECURE_NO_WARNINGS" )
#	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_CRT_SECURE_NO_DEPRECATE" )
#	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_WINSOCK_DEPRECATED_NO_WARNINGS" )
	set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COMPILER_DEFINES} /MTd /MP" )
	set( CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILER_DEFINES} /MTd /MP" )
	set( CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /INCREMENTAL:NO" )
endif()

link_directories( "Lib" )
link_directories( "Lib/Dx9/x86/" )

##
## Acm
##
set( ACM_SOURCES
	${ACM_DIR}/acmstrm.cpp
	${ACM_DIR}/acmstrm.h
)
add_library( Acm STATIC ${ACM_SOURCES} )
target_include_directories( Acm PRIVATE ${ACM_DIR} )
##
## AngelScript
## TODO (VS2017) ac_callfunc* not handled correctly (LNK4221)
##
set( ANGELSCRIPT_SOURCES
	${AS_DIR}/angelscript.h
	${AS_DIR}/as_array.h
	${AS_DIR}/as_atomic.cpp
	${AS_DIR}/as_atomic.h
	${AS_DIR}/as_builder.cpp
	${AS_DIR}/as_builder.h
	${AS_DIR}/as_bytecode.cpp
	${AS_DIR}/as_bytecode.h
	${AS_DIR}/as_callfunc.cpp
	${AS_DIR}/as_callfunc.h
	${AS_DIR}/as_callfunc_x64_gcc.cpp
	${AS_DIR}/as_callfunc_x64_msvc.asm
	${AS_DIR}/as_callfunc_x64_msvc.cpp
	${AS_DIR}/as_callfunc_x86.cpp
	${AS_DIR}/as_compiler.cpp
	${AS_DIR}/as_compiler.h
	${AS_DIR}/as_config.h
	${AS_DIR}/as_configgroup.cpp
	${AS_DIR}/as_configgroup.h
	${AS_DIR}/as_context.cpp
	${AS_DIR}/as_context.h
	${AS_DIR}/as_criticalsection.h
	${AS_DIR}/as_datatype.cpp
	${AS_DIR}/as_datatype.h
	${AS_DIR}/as_debug.h
	${AS_DIR}/as_gc.cpp
	${AS_DIR}/as_gc.h
	${AS_DIR}/as_generic.cpp
	${AS_DIR}/as_generic.h
	${AS_DIR}/as_globalproperty.cpp
	${AS_DIR}/as_map.h
	${AS_DIR}/as_memory.cpp
	${AS_DIR}/as_memory.h
	${AS_DIR}/as_module.cpp
	${AS_DIR}/as_module.h
	${AS_DIR}/as_objecttype.cpp
	${AS_DIR}/as_objecttype.h
	${AS_DIR}/as_outputbuffer.cpp
	${AS_DIR}/as_outputbuffer.h
	${AS_DIR}/as_parser.cpp
	${AS_DIR}/as_parser.h
	${AS_DIR}/as_property.h
	${AS_DIR}/as_restore.cpp
	${AS_DIR}/as_restore.h
	${AS_DIR}/as_scriptcode.cpp
	${AS_DIR}/as_scriptcode.h
	${AS_DIR}/as_scriptengine.cpp
	${AS_DIR}/as_scriptengine.h
	${AS_DIR}/as_scriptfunction.cpp
	${AS_DIR}/as_scriptfunction.h
	${AS_DIR}/as_scriptnode.cpp
	${AS_DIR}/as_scriptnode.h
	${AS_DIR}/as_scriptobject.cpp
	${AS_DIR}/as_scriptobject.h
	${AS_DIR}/as_string.cpp
	${AS_DIR}/as_string.h
	${AS_DIR}/as_string_util.cpp
	${AS_DIR}/as_string_util.h
	${AS_DIR}/as_symboltable.h
	${AS_DIR}/as_texts.h
	${AS_DIR}/as_thread.cpp
	${AS_DIR}/as_thread.h
	${AS_DIR}/as_tokendef.h
	${AS_DIR}/as_tokenizer.cpp
	${AS_DIR}/as_tokenizer.h
	${AS_DIR}/as_typeinfo.cpp
	${AS_DIR}/as_typeinfo.h
	${AS_DIR}/as_variablescope.cpp
	${AS_DIR}/as_variablescope.h
)
add_library( AngelScript STATIC ${ANGELSCRIPT_SOURCES} )
target_compile_definitions( AngelScript PUBLIC AS_NO_THREADS )
target_include_directories( AngelScript PRIVATE ${AS_DIR} )
target_link_libraries( AngelScript Threads::Threads )
##
## AngelScriptAddons
##
set( ANGELSCRIPT_ADDONS_SOURCES
	${AS_DIR}/scriptany.cpp
	${AS_DIR}/scriptany.h
	${AS_DIR}/scriptarray.cpp
	${AS_DIR}/scriptarray.h
	${AS_DIR}/scriptdictionary.cpp
	${AS_DIR}/scriptdictionary.h
	${AS_DIR}/scriptfile.cpp
	${AS_DIR}/scriptfile.h
	${AS_DIR}/scriptmath.cpp
	${AS_DIR}/scriptmath.h
	${AS_DIR}/scriptstring.cpp
	${AS_DIR}/scriptstring.h	
)
add_library( AngelScriptAddons STATIC ${ANGELSCRIPT_ADDONS_SOURCES} )
target_include_directories( AngelScriptAddons PRIVATE ${AS_DIR} ${FO_DIR} )
target_link_libraries( AngelScript Threads::Threads )
##
## AngelScriptPreprocessor
##
set( ANGELSCRIPT_PREPROCESSOR_SOURCES
	${AS_DIR}/preprocessor.cpp
	${AS_DIR}/preprocessor.h
)
add_library( AngelScriptPreprocessor STATIC ${ANGELSCRIPT_PREPROCESSOR_SOURCES} )
target_include_directories( AngelScriptPreprocessor PRIVATE ${AS_DIR} )
target_link_libraries( AngelScript Threads::Threads )
set( ANGELSCRIPT_LIBS AngelScript AngelScriptAddons )
set( ANGELSCRIPT_ALL_LIBS ${ANGELSCRIPT_LIBS} AngelScriptPreprocessor )
##
## DatFile
##
set( DATFILE_SOURCES
	${DAT_DIR}/cfile.cpp
	${DAT_DIR}/cfile.h
	${DAT_DIR}/unlzss.cpp
	${DAT_DIR}/unlzss.h
)
add_library( DatFile ${DATFILE_SOURCES} )
target_include_directories( DatFile PUBLIC ${DAT_DIR} ${FO_DIR} )
##
## GLEW
##
set( GLEW_SOURCES
	${GLEW_DIR}/glew.c
	${GLEW_DIR}/glew.h
	${GLEW_DIR}/glxew.h
	${GLEW_DIR}/wglew.h
)
add_library( GLEW ${GLEW_SOURCES} )
target_include_directories( GLEW PUBLIC ${FO_DIR} )
##
## SHA2
##
set( SHA2_SOURCES
	${SHA2_DIR}/sha2.c
	${SHA2_DIR}/sha2.h
)
add_library( SHA2 ${SHA2_SOURCES} )
target_include_directories( SHA2 PUBLIC ${SHA2_DIR} )
##
## Zlib
##
set( ZLIB_SOURCES
	${ZLIB_DIR}/adler32.c
	${ZLIB_DIR}/compress.c
	${ZLIB_DIR}/crc32.c
	${ZLIB_DIR}/crc32.h
	${ZLIB_DIR}/deflate.c
	${ZLIB_DIR}/deflate.h
	${ZLIB_DIR}/gzclose.c
	${ZLIB_DIR}/gzguts.h
	${ZLIB_DIR}/gzlib.c
	${ZLIB_DIR}/gzread.c
	${ZLIB_DIR}/gzwrite.c
	${ZLIB_DIR}/infback.c
	${ZLIB_DIR}/inffast.c
	${ZLIB_DIR}/inffast.h
	${ZLIB_DIR}/inffixed.h
	${ZLIB_DIR}/inflate.c
	${ZLIB_DIR}/inflate.h
	${ZLIB_DIR}/inftrees.c
	${ZLIB_DIR}/inftrees.h
	${ZLIB_DIR}/ioapi.c
	${ZLIB_DIR}/ioapi.h
	${ZLIB_DIR}/trees.c
	${ZLIB_DIR}/trees.h
	${ZLIB_DIR}/uncompr.c
	${ZLIB_DIR}/unzip.c
	${ZLIB_DIR}/unzip.h
	${ZLIB_DIR}/zconf.h
	${ZLIB_DIR}/zlib.h
	${ZLIB_DIR}/zutil.c
	${ZLIB_DIR}/zutil.h
)
add_library( Zlib STATIC ${ZLIB_SOURCES} )
target_include_directories( Zlib PUBLIC ${ZLIB_DIR} )
##
## Client
##
set( CLIENT_SOURCES
	${FO_DIR}/3dAnimation.h
	${FO_DIR}/3dStuff.cpp
	${FO_DIR}/3dStuff.h
	${FO_DIR}/Access.h
	${FO_DIR}/BufferManager.cpp
	${FO_DIR}/BufferManager.h
	${FO_DIR}/Client.cpp
	${FO_DIR}/Client.h
	${FO_DIR}/ClientInterface.cpp
	${FO_DIR}/Common.cpp
	${FO_DIR}/Common.h
	${FO_DIR}/ConstantsManager.cpp
	${FO_DIR}/ConstantsManager.h
	${FO_DIR}/CraftManager.cpp
	${FO_DIR}/CraftManager.h
	${FO_DIR}/CritterCl.cpp
	${FO_DIR}/CritterCl.h
	${FO_DIR}/CritterType.cpp
	${FO_DIR}/CritterType.h
	${FO_DIR}/Crypt.cpp
	${FO_DIR}/Crypt.h
	${FO_DIR}/DataFile.cpp
	${FO_DIR}/DataFile.h
	${FO_DIR}/DataMask.h
	${FO_DIR}/Debugger.cpp
	${FO_DIR}/Debugger.h
	${FO_DIR}/Defence.h
	${FO_DIR}/Defines.h
	${FO_DIR}/Exception.cpp
	${FO_DIR}/Exception.h
	${FO_DIR}/FileManager.cpp
	${FO_DIR}/FileManager.h
	${FO_DIR}/FileSystem.cpp
	${FO_DIR}/FileSystem.h
	${FO_DIR}/GraphicLoader.cpp
	${FO_DIR}/GraphicLoader.h
	${FO_DIR}/GraphicStructures.h
	${FO_DIR}/HexManager.cpp
	${FO_DIR}/HexManager.h
	${FO_DIR}/IniParser.cpp
	${FO_DIR}/IniParser.h
	${FO_DIR}/Item.cpp
	${FO_DIR}/Item.h
	${FO_DIR}/ItemHex.cpp
	${FO_DIR}/ItemHex.h
	${FO_DIR}/ItemManager.cpp
	${FO_DIR}/ItemManager.h
	${FO_DIR}/Keyboard.cpp
	${FO_DIR}/Keyboard.h
	${FO_DIR}/LineTracer.h
	${FO_DIR}/Log.cpp
	${FO_DIR}/Log.h
	${FO_DIR}/MainClient.cpp
	${FO_DIR}/MsgFiles.cpp
	${FO_DIR}/MsgFiles.h
	${FO_DIR}/MsgStr.h
	${FO_DIR}/Mutex.h
	${FO_DIR}/NetProtocol.h
	${FO_DIR}/PlatformSpecific.h
	${FO_DIR}/QuestManager.h
	${FO_DIR}/Randomizer.h
	${FO_DIR}/ResourceClient.h
	${FO_DIR}/ResourceManager.cpp
	${FO_DIR}/ResourceManager.h
	${FO_DIR}/Script.cpp
	${FO_DIR}/Script.h
	${FO_DIR}/ScriptBind.h
	${FO_DIR}/ScriptPragmas.cpp
	${FO_DIR}/ScriptPragmas.h
	${FO_DIR}/SoundManager.cpp
	${FO_DIR}/SoundManager.h
	${FO_DIR}/SpriteManager.cpp
	${FO_DIR}/SpriteManager.h
	${FO_DIR}/SpriteManagerFont.cpp
	${FO_DIR}/Sprites.cpp
	${FO_DIR}/Sprites.h
	${FO_DIR}/Text.cpp
	${FO_DIR}/Text.h
	${FO_DIR}/Timer.cpp
	${FO_DIR}/Timer.h
	${FO_DIR}/Types.h
	${FO_DIR}/Version.h
)
##
## ClientDX
##
set( CLIENT_DX_SOURCES
	${CLIENT_SOURCES}
)
add_executable( ClientDX WIN32 ${CLIENT_DX_SOURCES} )
target_compile_definitions( ClientDX PRIVATE FO_D3D FONLINE_CLIENT )
target_include_directories( ClientDX PRIVATE ${FO_DIR} ${FO_DIR}/Dx9 )
target_link_libraries( ClientDX Acm ${ANGELSCRIPT_ALL_LIBS} DatFile SHA2 Zlib )
##
## ClientGL
##
set( CLIENT_GL_SOURCES
	${CLIENT_SOURCES}
)
add_executable( ClientGL WIN32 ${CLIENT_GL_SOURCES} )
target_compile_definitions( ClientGL PRIVATE FONLINE_CLIENT )
target_include_directories( ClientGL PRIVATE ${FO_DIR} )
target_link_libraries( ClientGL Acm ${ANGELSCRIPT_ALL_LIBS} DatFile GLEW SHA2 Zlib )
##
## Mapper
##
set( MAPPER_SOURCES
	${FO_DIR}/3dAnimation.h
	${FO_DIR}/3dStuff.cpp
	${FO_DIR}/3dStuff.h
	${FO_DIR}/Common.cpp
	${FO_DIR}/Common.h
	${FO_DIR}/ConstantsManager.cpp
	${FO_DIR}/ConstantsManager.h
	${FO_DIR}/CritterCl.cpp
	${FO_DIR}/CritterCl.h
	${FO_DIR}/CritterData.h
	${FO_DIR}/CritterManager.cpp
	${FO_DIR}/CritterManager.h
	${FO_DIR}/CritterType.cpp
	${FO_DIR}/CritterType.h
	${FO_DIR}/Crypt.cpp
	${FO_DIR}/Crypt.h
	${FO_DIR}/DataFile.cpp
	${FO_DIR}/DataFile.h
	${FO_DIR}/Debugger.cpp
	${FO_DIR}/Debugger.h
	${FO_DIR}/Defines.h
	${FO_DIR}/Exception.cpp
	${FO_DIR}/Exception.h
	${FO_DIR}/F2Palette.h
	${FO_DIR}/FileManager.cpp
	${FO_DIR}/FileManager.h
	${FO_DIR}/FileSystem.cpp
	${FO_DIR}/FileSystem.h
	${FO_DIR}/FlexRect.h
	${FO_DIR}/GraphicLoader.cpp
	${FO_DIR}/GraphicLoader.h
	${FO_DIR}/HexManager.cpp
	${FO_DIR}/HexManager.h
	${FO_DIR}/IniParser.cpp
	${FO_DIR}/IniParser.h
	${FO_DIR}/Item.cpp
	${FO_DIR}/Item.h
	${FO_DIR}/ItemHex.cpp
	${FO_DIR}/ItemHex.h
	${FO_DIR}/ItemManager.cpp
	${FO_DIR}/ItemManager.h
	${FO_DIR}/Keyboard.cpp
	${FO_DIR}/Keyboard.h
	${FO_DIR}/LineTracer.h
	${FO_DIR}/Log.cpp
	${FO_DIR}/Log.h
	${FO_DIR}/MainMapper.cpp
	${FO_DIR}/Mapper.cpp
	${FO_DIR}/Mapper.h
	${FO_DIR}/MsgFiles.cpp
	${FO_DIR}/MsgFiles.h
	${FO_DIR}/MsgStr.h
	${FO_DIR}/PlatformSpecific.h
	${FO_DIR}/ProtoMap.cpp
	${FO_DIR}/ProtoMap.h
	${FO_DIR}/Randomizer.h
	${FO_DIR}/ResourceManager.cpp
	${FO_DIR}/ResourceManager.h
	${FO_DIR}/ResourceMapper.h
	${FO_DIR}/Script.cpp
	${FO_DIR}/Script.h
	${FO_DIR}/ScriptBind.h
	${FO_DIR}/ScriptPragmas.cpp
	${FO_DIR}/ScriptPragmas.h
	${FO_DIR}/SpriteManager.cpp
	${FO_DIR}/SpriteManager.h
	${FO_DIR}/SpriteManagerFont.cpp
	${FO_DIR}/Sprites.cpp
	${FO_DIR}/Sprites.h
	${FO_DIR}/Text.cpp
	${FO_DIR}/Text.h
	${FO_DIR}/Timer.cpp
	${FO_DIR}/Timer.h
	${FO_DIR}/Types.h
	${FO_DIR}/Version.h
)
##
## MapperDX
##
set( MAPPER_DX_SOURCES
	${MAPPER_SOURCES}
)
add_executable( MapperDX WIN32 ${MAPPER_DX_SOURCES} )
target_compile_definitions( MapperDX PRIVATE FO_D3D FONLINE_MAPPER )
target_include_directories( MapperDX PRIVATE ${FO_DIR} ${FO_DIR}/Dx9 )
target_link_libraries( MapperDX ${ANGELSCRIPT_ALL_LIBS} DatFile SHA2 Zlib )
##
## MapperGL
##
set( MAPPER_GL_SOURCES
	${MAPPER_SOURCES}
)
add_executable( MapperGL WIN32 ${MAPPER_GL_SOURCES} )
target_compile_definitions( MapperGL PRIVATE FONLINE_MAPPER )
target_include_directories( MapperGL PRIVATE ${FO_DIR} )
target_link_libraries( MapperGL ${ANGELSCRIPT_ALL_LIBS} DatFile GLEW SHA2 Zlib )
##
## Server
##
set( SERVER_SOURCES
	${FO_DIR}/Access.h
	${FO_DIR}/AI.cpp
	${FO_DIR}/AI.h
	${FO_DIR}/BufferManager.cpp
	${FO_DIR}/BufferManager.h
	${FO_DIR}/Common.cpp
	${FO_DIR}/Common.h
	${FO_DIR}/ConstantsManager.cpp
	${FO_DIR}/ConstantsManager.h
	${FO_DIR}/CraftManager.cpp
	${FO_DIR}/CraftManager.h
	${FO_DIR}/Critter.cpp
	${FO_DIR}/Critter.h
	${FO_DIR}/CritterData.h
	${FO_DIR}/CritterManager.cpp
	${FO_DIR}/CritterManager.h
	${FO_DIR}/CritterType.cpp
	${FO_DIR}/CritterType.h
	${FO_DIR}/Crypt.cpp
	${FO_DIR}/Crypt.h
	${FO_DIR}/DataFile.cpp
	${FO_DIR}/DataFile.h
	${FO_DIR}/DataMask.h
	${FO_DIR}/Debugger.cpp
	${FO_DIR}/Debugger.h
	${FO_DIR}/Defines.h
	${FO_DIR}/Dialogs.cpp
	${FO_DIR}/Dialogs.h
	${FO_DIR}/DummyData.h
	${FO_DIR}/Exception.cpp
	${FO_DIR}/Exception.h
	${FO_DIR}/FileManager.cpp
	${FO_DIR}/FileManager.h
	${FO_DIR}/FileSystem.cpp
	${FO_DIR}/FileSystem.h
	${FO_DIR}/FlexRect.h
	${FO_DIR}/IniParser.cpp
	${FO_DIR}/IniParser.h
	${FO_DIR}/Item.cpp
	${FO_DIR}/Item.h
	${FO_DIR}/ItemManager.cpp
	${FO_DIR}/ItemManager.h
	${FO_DIR}/Jobs.cpp
	${FO_DIR}/Jobs.h
	${FO_DIR}/LineTracer.h
	${FO_DIR}/Log.cpp
	${FO_DIR}/Log.h
	${FO_DIR}/MainServer.cpp
	${FO_DIR}/Map.cpp
	${FO_DIR}/Map.h
	${FO_DIR}/MapManager.cpp
	${FO_DIR}/MapManager.h
	${FO_DIR}/MsgFiles.cpp
	${FO_DIR}/MsgFiles.h
	${FO_DIR}/MsgStr.h
	${FO_DIR}/Mutex.h
	${FO_DIR}/NetProtocol.h
	${FO_DIR}/PlatformSpecific.h
	${FO_DIR}/ProtoMap.cpp
	${FO_DIR}/ProtoMap.h
	${FO_DIR}/Randomizer.h
	${FO_DIR}/Script.cpp
	${FO_DIR}/Script.h
	${FO_DIR}/ScriptBind.h
	${FO_DIR}/ScriptPragmas.cpp
	${FO_DIR}/ScriptPragmas.h
	${FO_DIR}/Server.cpp
	${FO_DIR}/Server.h
	${FO_DIR}/ServerClient.cpp
	${FO_DIR}/ServerItem.cpp
	${FO_DIR}/ServerNpc.cpp
	${FO_DIR}/ServerScript.cpp
	${FO_DIR}/Text.cpp
	${FO_DIR}/Text.h
	${FO_DIR}/ThreadSync.cpp
	${FO_DIR}/ThreadSync.h
	${FO_DIR}/Timer.cpp
	${FO_DIR}/Timer.h
	${FO_DIR}/Types.h
	${FO_DIR}/Vars.cpp
	${FO_DIR}/Vars.h
	${FO_DIR}/Version.h
)
add_executable( Server WIN32 ${SERVER_SOURCES} )
target_compile_definitions( Server PRIVATE FONLINE_SERVER )
target_include_directories( Server PRIVATE ${FO_DIR} )
target_link_libraries( Server ${ANGELSCRIPT_ALL_LIBS} DatFile SHA2 Zlib )
##
## ASCompiler
##
set( ASCOMPILER_SOURCES
	ASCompiler/ScriptEngine.h
	${FO_DIR}/ASCompiler.cpp
	${FO_DIR}/Common.cpp
	${FO_DIR}/Common.h
	${FO_DIR}/Crypt.cpp
	${FO_DIR}/Crypt.h
	${FO_DIR}/DataFile.cpp
	${FO_DIR}/DataFile.h
	${FO_DIR}/Debugger.cpp
	${FO_DIR}/Debugger.h
	${FO_DIR}/DummyData.h
	${FO_DIR}/FileManager.cpp
	${FO_DIR}/FileManager.h
	${FO_DIR}/FileSystem.cpp
	${FO_DIR}/FileSystem.h
	${FO_DIR}/IniParser.cpp
	${FO_DIR}/IniParser.h
	${FO_DIR}/Log.cpp
	${FO_DIR}/Log.h
	${FO_DIR}/ScriptBind.h
	${FO_DIR}/ScriptPragmas.cpp
	${FO_DIR}/ScriptPragmas.h
	${FO_DIR}/Text.cpp
	${FO_DIR}/Text.h
	${FO_DIR}/Timer.cpp
	${FO_DIR}/Timer.h
)
add_executable( ASCompiler ${ASCOMPILER_SOURCES} )
target_include_directories( ASCompiler PRIVATE "ASCompiler" ${FO_DIR} )
target_compile_definitions( ASCompiler PRIVATE FONLINE_SCRIPT_COMPILER )
target_link_libraries( ASCompiler ${ANGELSCRIPT_ALL_LIBS} DatFile SHA2 Zlib )
##
## Prettify IDE
##
set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set_property( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake" )
set_target_properties( ${ANGELSCRIPT_ALL_LIBS} PROPERTIES FOLDER "AngelScript" )
set_target_properties( ASCompiler PROPERTIES FOLDER "Tools" )
set_target_properties( Acm DatFile GLEW SHA2 Zlib PROPERTIES FOLDER "Libs" )
