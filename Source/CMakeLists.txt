add_subdirectory( AngelScript )
add_subdirectory( Libs )

add_subdirectory( DatFile )
add_subdirectory( Shared )

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
		Debugger.cpp
		Debugger.h
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
target_include_directories( ASCompiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )
target_compile_definitions( ASCompiler PRIVATE FOCLASSIC_SCRIPT_COMPILER )
target_link_libraries( ASCompiler Shared AngelScript AngelScriptPreprocessor DatFile sha2 zlib )

##
## Client
##
set( CLIENT_SOURCES
	3dAnimation.h
	3dStuff.cpp
	3dStuff.h
	Access.cpp
	Access.h
	BufferManager.cpp
	BufferManager.h
	Client.cpp
	Client.h
	ClientInterface.cpp
	Common.cpp
	Common.h
	ConstantsManager.cpp
	ConstantsManager.h
	CraftManager.cpp
	CraftManager.h
	CritterCl.cpp
	CritterCl.h
	CritterType.cpp
	CritterType.h
	DataFile.cpp
	DataFile.h
	DataMask.h
	Debugger.cpp
	Debugger.h
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
	IniParser.cpp
	IniParser.h
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
	SpriteManager.cpp
	SpriteManager.h
	SpriteManagerFont.cpp
	Sprites.cpp
	Sprites.h
	Version.h
)
##
## ClientDX
##
set( CLIENT_DX_SOURCES
	${CLIENT_SOURCES}
)
add_executable( ClientDX WIN32 "" )
target_sources( ClientDX PRIVATE ${CLIENT_DX_SOURCES} )
target_compile_definitions( ClientDX PRIVATE FO_D3D FOCLASSIC_CLIENT )
target_include_directories( ClientDX PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Dx9 )
target_link_libraries( ClientDX Shared AngelScript AngelScriptPreprocessor DatFile acm glew sha2 zlib ${DX_LIBS} )

##
## ClientGL
##
set( CLIENT_GL_SOURCES
	${CLIENT_SOURCES}
)
add_executable( ClientGL WIN32 "" )
target_sources( ClientGL PRIVATE ${CLIENT_GL_SOURCES} )
target_compile_definitions( ClientGL PRIVATE FOCLASSIC_CLIENT )
target_include_directories( ClientGL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries( ClientGL Shared AngelScript AngelScriptPreprocessor DatFile acm glew sha2 zlib ${GL_LIBS} )

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
	${FO_DIR}/DataFile.cpp
	${FO_DIR}/DataFile.h
	${FO_DIR}/Debugger.cpp
	${FO_DIR}/Debugger.h
	${FO_DIR}/Exception.cpp
	${FO_DIR}/Exception.h
	${FO_DIR}/F2Palette.h
	${FO_DIR}/FileManager.cpp
	${FO_DIR}/FileManager.h
	${FO_DIR}/FlexRect.h
	${FO_DIR}/GraphicStructures.cpp
	${FO_DIR}/GraphicStructures.h
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
	${FO_DIR}/ProtoMap.cpp
	${FO_DIR}/ProtoMap.h
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
	${FO_DIR}/Version.h
)
##
## MapperDX
##
set( MAPPER_DX_SOURCES
	${MAPPER_SOURCES}
)
add_executable( MapperDX WIN32 "" )
target_sources( MapperDX PRIVATE ${MAPPER_DX_SOURCES} )
target_compile_definitions( MapperDX PRIVATE FO_D3D FOCLASSIC_MAPPER )
target_include_directories( MapperDX PRIVATE ${FO_DIR} ${FO_DIR}/Dx9 )
target_link_libraries( MapperDX Shared AngelScript AngelScriptPreprocessor DatFile glew sha2 zlib ${DX_LIBS} )

##
## MapperGL
##
set( MAPPER_GL_SOURCES
	${MAPPER_SOURCES}
)
add_executable( MapperGL WIN32 ${MAPPER_GL_SOURCES} )
target_compile_definitions( MapperGL PRIVATE FOCLASSIC_MAPPER )
target_include_directories( MapperGL PRIVATE ${FO_DIR} )
target_link_libraries( MapperGL Shared AngelScript AngelScriptPreprocessor DatFile glew sha2 zlib ${GL_LIBS} )

##
## Server
##
add_executable( Server WIN32 "" )
target_sources( Server
	PRIVATE
		Access.cpp
		Access.h
		AI.cpp
		AI.h
		BufferManager.cpp
		BufferManager.h
		Common.cpp
		Common.h
		ConstantsManager.cpp
		ConstantsManager.h
		CraftManager.cpp
		CraftManager.h
		Critter.cpp
		Critter.h
		CritterData.h
		CritterManager.cpp
		CritterManager.h
		CritterType.cpp
		CritterType.h
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
		IniParser.cpp
		IniParser.h
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
		ProtoMap.cpp
		ProtoMap.h
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
target_include_directories( Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries( Server Shared AngelScript AngelScriptPreprocessor DatFile sha2 zlib libpng15 )

##
## Finalize
##
set( FOCLASSIC_APPS ASCompiler ClientDX ClientGL MapperDX MapperGL Server )

configure_file( "${CMAKE_SOURCE_DIR}/CMake/CMake.h.in" "${CMAKE_BINARY_DIR}/include/CMake.h" @ONLY NEWLINE_STYLE LF )
foreach( target ${FOCLASSIC_APPS} )
	# append headers generated by CMake
	target_sources( ${target} PRIVATE "${CMAKE_BINARY_DIR}/include/CMake.h" )
	# setup released .pdb files
	pdb_strip( ${target} )
	# inform sources about engine compilation
	target_compile_definitions( ${target} PRIVATE FOCLASSIC_ENGINE )
endforeach()

##
## Prettify executables
##
if( MSVC )
	foreach( target ${FOCLASSIC_APPS} )
		target_sources( ${target} PRIVATE ${CMAKE_SOURCE_DIR}/Resources/VersionInfo.rc )
	endforeach()
endif( MSVC )

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
