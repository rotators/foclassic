add_subdirectory( AngelScript )
add_subdirectory( Libs )

message( STATUS "Configuring engine..." )

add_subdirectory( DatFile )
add_subdirectory( Shared )

set( FOCLASSIC_LIBS AngelScript AngelScriptPreprocessor DatFile Shared )
set( FOCLASSIC_INCLUDES ${CMAKE_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Libs )

if( WIN32 )
	set( FOCLASSIC_LIBS ${FOCLASSIC_LIBS}
		dbghelp # Debugger.cpp Exception.cpp
		psapi # Exception.cpp
	)
endif()

link_directories( "${CMAKE_SOURCE_DIR}/Lib" )

if( WIN32 )
	link_directories( "${CMAKE_SOURCE_DIR}/Lib/Dx9/x86/" )

	# DX needs glew/glu32 due to 3dStuff.cpp VecProject() VecUnproject()
	set( DX_LIBS dx9 glew glu32 )
endif()

set( GL_LIBS fltkgl glew glu32 opengl32 theora )

# predefine libs used by engine applications
# NOTES
# - il requires jpeg, png

set( ASCOMPILER_LIBS ${FOCLASSIC_LIBS} )
set( CLIENT_LIBS     ${FOCLASSIC_LIBS} acm assimp fltk il jpeg ogg png portaudio )
set( MAPPER_LIBS     ${FOCLASSIC_LIBS} assimp fltk il jpeg png )
set( SERVER_LIBS     ${FOCLASSIC_LIBS} distorm fltk png )

if( WIN32 )
	set( CLIENT_LIBS ${CLIENT_LIBS} ws2_32 )
	set( SERVER_LIBS ${SERVER_LIBS} ws2_32 )
endif()

set( CLIENT_DX_LIBS ${CLIENT_LIBS} ${DX_LIBS} )
set( CLIENT_GL_LIBS ${CLIENT_LIBS} ${GL_LIBS} )
set( MAPPER_DX_LIBS ${MAPPER_LIBS} ${DX_LIBS} )
set( MAPPER_GL_LIBS ${MAPPER_LIBS} ${GL_LIBS} )

##
## Mixed sources
##

set( CLIENT_SERVER_SOURCES
	Access.cpp
	Access.h
	BufferManager.cpp
	BufferManager.h
	CraftManager.cpp
	CraftManager.h
	Network.h
)

set( CLIENT_MAPPER_SOURCES
	3dAnimation.h
	3dStuff.cpp
	3dStuff.h
	CritterCl.cpp
	CritterCl.h
	SpriteManager.cpp
	SpriteManager.h
	SpriteManagerFont.cpp
	Window.cpp
	Window.h
)

set( MAPPER_SERVER_SOURCES
	ProtoMap.cpp
	ProtoMap.h
)

set( CLIENT_MAPPER_SERVER_SOURCES
	ConstantsManager.cpp
	ConstantsManager.h
	CritterType.cpp
	CritterType.h
	IniParser.cpp
	IniParser.h
	ScriptFunctions.cpp
	ScriptFunctions.h
)

##
## ASCompiler
##

add_executable( ASCompiler "" )
target_sources( ASCompiler
	PRIVATE
		ASCompiler.cpp
		ASCompiler.h
		Common.cpp
		Common.h
		DataFile.cpp
		DataFile.h
		Exception.cpp
		Exception.h
		DummyData.h
		FileManager.cpp
		FileManager.h
		IniParser.cpp
		IniParser.h
		Log.cpp
		Log.h
		ScriptBind.h
		ScriptPragmas.cpp
		ScriptPragmas.h
)
target_compile_definitions( ASCompiler PRIVATE FOCLASSIC_SCRIPT_COMPILER )
target_include_directories( ASCompiler PRIVATE ${FOCLASSIC_INCLUDES} )
target_link_libraries( ASCompiler ${ASCOMPILER_LIBS} )

##
## Client
##

set( CLIENT_SOURCES
	Client.cpp
	Client.h
	ClientInterface.cpp
	Common.cpp
	Common.h
	DataFile.cpp
	DataFile.h
	DataMask.h
	Defence.h
	Exception.cpp
	Exception.h
	FileManager.cpp
	FileManager.h
	GraphicLoader.cpp
	GraphicLoader.h
	GraphicStructures.cpp
	GraphicStructures.h
	HexManager.cpp
	HexManager.h
	Item.cpp
	Item.h
	ItemHex.cpp
	ItemHex.h
	ItemManager.cpp
	ItemManager.h
	Keyboard.cpp
	Keyboard.h
	LineTracer.h
	Log.cpp
	Log.h
	MainClient.cpp
	MsgFiles.cpp
	MsgFiles.h
	MsgStr.h
	NetProtocol.h
	QuestManager.cpp
	QuestManager.h
	ResourceClient.h
	ResourceManager.cpp
	ResourceManager.h
	Script.cpp
	Script.h
	ScriptBind.h
	ScriptPragmas.cpp
	ScriptPragmas.h
	SoundManager.cpp
	SoundManager.h
	Sprites.cpp
	Sprites.h
	Version.h
)

##
## ClientDX
##

if( WIN32 )
	add_executable( ClientDX WIN32 "" )
	target_sources( ClientDX
		PRIVATE
			${CLIENT_MAPPER_SERVER_SOURCES}
			${CLIENT_MAPPER_SOURCES}
			${CLIENT_SERVER_SOURCES}
			${CLIENT_SOURCES}
	)
	target_compile_definitions( ClientDX PRIVATE FO_D3D FOCLASSIC_CLIENT )
	target_include_directories( ClientDX PRIVATE ${FOCLASSIC_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/Dx9 )
	target_link_libraries( ClientDX  ${CLIENT_DX_LIBS} )
endif()

##
## ClientGL
##

add_executable( ClientGL WIN32 "" )
target_sources( ClientGL
	PRIVATE
		${CLIENT_MAPPER_SERVER_SOURCES}
		${CLIENT_MAPPER_SOURCES}
		${CLIENT_SERVER_SOURCES}
		${CLIENT_SOURCES}
)
target_compile_definitions( ClientGL PRIVATE FOCLASSIC_CLIENT )
target_include_directories( ClientGL PRIVATE ${FOCLASSIC_INCLUDES} )
target_link_libraries( ClientGL ${CLIENT_GL_LIBS} )

##
## Mapper
##

set( MAPPER_SOURCES
	Common.cpp
	Common.h
	CritterData.h
	CritterManager.cpp
	CritterManager.h
	DataFile.cpp
	DataFile.h
	Exception.cpp
	Exception.h
	F2Palette.h
	FileManager.cpp
	FileManager.h
	FlexRect.h
	GraphicStructures.cpp
	GraphicStructures.h
	GraphicLoader.cpp
	GraphicLoader.h
	HexManager.cpp
	HexManager.h
	Item.cpp
	Item.h
	ItemHex.cpp
	ItemHex.h
	ItemManager.cpp
	ItemManager.h
	Keyboard.cpp
	Keyboard.h
	LineTracer.h
	Log.cpp
	Log.h
	MainMapper.cpp
	Mapper.cpp
	Mapper.h
	MsgFiles.cpp
	MsgFiles.h
	MsgStr.h
	ResourceManager.cpp
	ResourceManager.h
	ResourceMapper.h
	Script.cpp
	Script.h
	ScriptBind.h
	ScriptPragmas.cpp
	ScriptPragmas.h
	Sprites.cpp
	Sprites.h
	Version.h
)

##
## MapperDX
##
if( WIN32 )
	add_executable( MapperDX WIN32 "" )
	target_sources( MapperDX
		PRIVATE
			${CLIENT_MAPPER_SERVER_SOURCES}
			${CLIENT_MAPPER_SOURCES}
			${MAPPER_SERVER_SOURCES}
			${MAPPER_SOURCES}
	)
	target_compile_definitions( MapperDX PRIVATE FO_D3D FOCLASSIC_MAPPER )
	target_include_directories( MapperDX PRIVATE ${FOCLASSIC_INCLUDES} ${FO_DIR} ${FO_DIR}/Dx9 )
	target_link_libraries( MapperDX ${MAPPER_DX_LIBS} )
endif()

##
## MapperGL
##

add_executable( MapperGL WIN32 "" )
target_sources( MapperGL
	PRIVATE
		${CLIENT_MAPPER_SERVER_SOURCES}
		${CLIENT_MAPPER_SOURCES}
		${MAPPER_SERVER_SOURCES}
		${MAPPER_SOURCES}
)
target_compile_definitions( MapperGL PRIVATE FOCLASSIC_MAPPER )
target_include_directories( MapperGL PRIVATE ${FOCLASSIC_INCLUDES} )
target_link_libraries( MapperGL ${MAPPER_GL_LIBS} )

##
## Server
##

add_executable( Server WIN32 "" )
target_sources( Server
	PRIVATE
		${CLIENT_MAPPER_SERVER_SOURCES}
		${CLIENT_SERVER_SOURCES}
		${MAPPER_SERVER_SOURCES}
		AI.cpp
		AI.h
		Common.cpp
		Common.h
		Critter.cpp
		Critter.h
		CritterData.h
		CritterManager.cpp
		CritterManager.h
		DataFile.cpp
		DataFile.h
		DataMask.h
		Debugger.cpp
		Debugger.h
		Dialogs.cpp
		Dialogs.h
		DummyData.h
		Exception.cpp
		Exception.h
		FileManager.cpp
		FileManager.h
		FlexRect.h
		Item.cpp
		Item.h
		ItemManager.cpp
		ItemManager.h
		Jobs.cpp
		Jobs.h
		LineTracer.h
		Log.cpp
		Log.h
		MainServer.cpp
		Map.cpp
		Map.h
		MapManager.cpp
		MapManager.h
		MsgFiles.cpp
		MsgFiles.h
		MsgStr.h
		NetProtocol.h
		Script.cpp
		Script.h
		ScriptBind.h
		ScriptPragmas.cpp
		ScriptPragmas.h
		Server.cpp
		Server.h
		ServerClient.cpp
		ServerItem.cpp
		ServerNpc.cpp
		ServerScript.cpp
		ThreadSync.cpp
		ThreadSync.h
		Vars.cpp
		Vars.h
		Version.h
)
target_compile_definitions( Server PRIVATE FOCLASSIC_SERVER SCRIPT_MULTITHREADING )
target_include_directories( Server PRIVATE ${FOCLASSIC_INCLUDES} )
target_link_libraries( Server ${SERVER_LIBS} )
##
## Finalize
##

set( FOCLASSIC_APPS ASCompiler ClientDX ClientGL MapperDX MapperGL Server )

# create global header
configure_file( "${CMAKE_SOURCE_DIR}/CMake/CMake.h.in" "${CMAKE_BINARY_DIR}/include/CMake.h" @ONLY NEWLINE_STYLE LF )
foreach( target ${FOCLASSIC_APPS} )

	# append headers generated by CMake
	target_sources( ${target} PRIVATE "${CMAKE_BINARY_DIR}/include/CMake.h" )

	# setup released .pdb files
	pdb_strip( ${target} )

	# inform sources about engine compilation
	target_compile_definitions( ${target} PRIVATE FOCLASSIC_ENGINE )

	if( WIN32 )
		# n/c
		target_compile_definitions( ${target} PRIVATE WINVER=0x0501 ) # Windows XP
		target_compile_definitions( ${target} PRIVATE WIN32_LEAN_AND_MEAN )

		# prettify windows executables
		target_sources( ${target} PRIVATE ${CMAKE_SOURCE_DIR}/Resources/VersionInfo.rc )
	endif()

	endforeach()

##
## Prettify IDE
##

source_group( " "           REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "AngelScript" REGULAR_EXPRESSION "${FO_DIR}/AngelScript" )
source_group( "Shared"      REGULAR_EXPRESSION "${FO_DIR}/Shared" )
source_group( "Misc"
	FILES
		"${CMAKE_BINARY_DIR}/include/CMake.h"
		"${CMAKE_SOURCE_DIR}/Resources/VersionInfo.rc"
	REGULAR_EXPRESSION
		"CMakeLists"
)

message( STATUS "Configuring engine... OK" )
