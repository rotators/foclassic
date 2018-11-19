##
## process subdirectories
##

add_subdirectory( AngelScript )
add_subdirectory( Libs )

message( STATUS "Configuring engine..." )

add_subdirectory( DatFile )
add_subdirectory( Shared )

find_package( OpenGL )
foreach( var IN ITEMS OPENGL_FOUND OPENGL_XMESA_FOUND OPENGL_GLU_FOUND OpenGL_OpenGL_FOUND OpenGL_GLX_FOUND OpenGL_EGL_FOUND OPENGL_INCLUDE_DIR OPENGL_EGL_INCLUDE_DIRS OPENGL_LIBRARIES )
	message( STATUS "${var} = '${${var}}'" )
endforeach()

##
## configure libraries
##

# libraries used by all applications
set( FOCLASSIC_LIBS angelscript angelscript-preprocessor DatFile Shared )

set( FOCLASSIC_INCLUDES ${DEFAULT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

if( UNIX )
	list( APPEND FOCLASSIC_LIBS dl rt )
elseif( WIN32 )
	list( APPEND FOCLASSIC_LIBS
		dbghelp # Debugger.cpp Exception.cpp
		psapi # Exception.cpp
		ws2_32 # pthreads?
	)
endif()

# DX needs glew/glu due to 3dStuff.cpp VecProject() VecUnproject()
set( DX_LIBS dx9 glew OpenGL::GLU )
set( GL_LIBS fltkgl glew OpenGL::GL OpenGL::GLU )

# predefine libs used by engine applications
# NOTES
# - il requires jpeg, png

set( ASCOMPILER_LIBS ${FOCLASSIC_LIBS} )
set( CLIENT_LIBS     ${FOCLASSIC_LIBS} acm assimp fltk il jpeg ogg png portaudio )
set( MAPPER_LIBS     ${FOCLASSIC_LIBS} assimp fltk il jpeg png )
set( SERVER_LIBS     ${FOCLASSIC_LIBS} distorm fltk png )

if( UNIX )
	list( APPEND CLIENT_LIBS X11 Xext Xft Xi Xinerama fontconfig )
elseif( WIN32 )
	list( APPEND CLIENT_LIBS Iphlpapi )
endif()

set( CLIENT_DX_LIBS ${CLIENT_LIBS} ${DX_LIBS} )
set( CLIENT_GL_LIBS ${CLIENT_LIBS} ${GL_LIBS} theora )
set( MAPPER_DX_LIBS ${MAPPER_LIBS} ${DX_LIBS} )
set( MAPPER_GL_LIBS ${MAPPER_LIBS} ${GL_LIBS} )

##
## mixed sources
##

set( CLIENT_MAPPER_SOURCES
	3dAnimation.h
	3dStuff.cpp
	3dStuff.h
	CritterCl.cpp
	CritterCl.h
	Keyboard.cpp
	Keyboard.h
	SpriteManager.cpp
	SpriteManager.h
	SpriteManagerFont.cpp
)

set( CLIENT_SERVER_SOURCES
	${ENGINE_NET_HEADER_FILE}
	Access.cpp
	Access.h
	BufferManager.cpp
	BufferManager.h
	CraftManager.cpp
	CraftManager.h
	Network.cpp
	Network.h
	Scores.h
)

set( MAPPER_SERVER_SOURCES
	Deprecated.cpp
	Deprecated.h
	ProtoMap.cpp
	ProtoMap.h
)

set( CLIENT_MAPPER_SERVER_SOURCES
	ConfigFile.cpp
	ConfigFile.h
	ConstantsManager.cpp
	ConstantsManager.h
	CritterType.cpp
	CritterType.h
	IniParser.cpp
	IniParser.h
	Log.cpp
	Log.h
	Script.cpp
	Script.h
	ScriptFunctions.cpp
	ScriptFunctions.h
	ScriptPragmas.cpp
	ScriptPragmas.h
	SinglePlayer.cpp
	SinglePlayer.h
	Utils.cpp
	Utils.h
	Window.cpp
	Window.h
)

##
## Client
##

set( CLIENT_SOURCES
	Client.cpp
	Client.h
	ClientInterface.cpp
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
	LineTracer.h
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
	ScriptBind.h
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
	target_include_directories( ClientDX PRIVATE ${FOCLASSIC_INCLUDES} )
	target_link_libraries( ClientDX ${CLIENT_DX_LIBS} )
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
	LineTracer.h
	MainMapper.cpp
	Mapper.cpp
	Mapper.h
	MsgFiles.cpp
	MsgFiles.h
	MsgStr.h
	ResourceManager.cpp
	ResourceManager.h
	ResourceMapper.h
	ScriptBind.h
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
	target_include_directories( MapperDX PRIVATE ${FOCLASSIC_INCLUDES} )
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
		LogFile.cpp
		LogFile.h
		MainServer.cpp
		Map.cpp
		Map.h
		MapManager.cpp
		MapManager.h
		MsgFiles.cpp
		MsgFiles.h
		MsgStr.h
		NetProtocol.h
		ScriptBind.h
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
if( MSVC )
	# see https://docs.microsoft.com/en-us/cpp/build/reference/largeaddressaware-handle-large-addresses
	set_property( TARGET Server APPEND_STRING PROPERTY LINK_FLAGS "/LARGEADDRESSAWARE " )
endif()
##
## ASCompiler
##

add_executable( ASCompiler "" )
target_sources( ASCompiler
	PRIVATE
		ASCompiler.cpp
		ASCompiler.h
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
set_property( TARGET ASCompiler PROPERTY FOLDER "Tools" )
set_property( TARGET ASCompiler PROPERTY RELEASE_SUBDIRECTORY "Tools" )

get_property( FOCLASSIC_APPS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS )
foreach( target IN LISTS FOCLASSIC_APPS )

	##
	## shared configuration
	##

	# configure .pdb files creation
	PdbSetup( ${target} )

	# released sources/headers should be aware of engine compilation
	target_compile_definitions( ${target} PRIVATE FOCLASSIC_ENGINE )

	# linux-specific configuration
	if( UNIX )
		if( CMAKE_CXX_STANDARD LESS 17 )
			set_property( TARGET ${target} APPEND_STRING PROPERTY COMPILE_FLAGS "-Wno-invalid-offsetof " )
		endif()
	# windows-specific configuration
	elseif( WIN32 )
		# n/c
		target_compile_definitions( ${target} PRIVATE WINVER=0x0501 ) # Windows XP
		target_compile_definitions( ${target} PRIVATE WIN32_LEAN_AND_MEAN )

		if( MSVC )
			# see https://docs.microsoft.com/en-us/cpp/build/reference/opt-optimizations
			set_property( TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS "/OPT:REF " )
		endif()

		# include version informations
		target_sources( ${target} PRIVATE ${CMAKE_SOURCE_DIR}/Resources/VersionInfo.rc )
	endif()

	##
	## prepare for release
	##

	set( dir "${RELEASE_DIR}" )
	get_property( subdir TARGET ${target} PROPERTY RELEASE_SUBDIRECTORY )
	if( subdir )
		set( dir "${RELEASE_DIR}/${subdir}" )
	endif()

	# prepare executable for release after build
	add_custom_command( TARGET ${target}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo "${target} -> $<SHELL_PATH:${dir}/$<TARGET_FILE_NAME:${target}>>"
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${dir}/$<TARGET_FILE_NAME:${target}>
	)

	if( MSVC )
		# delete .pdb file before build
		add_custom_command( TARGET ${target}
			PRE_BUILD
				COMMAND ${CMAKE_COMMAND} -E remove ${dir}/$<TARGET_PDB_FILE_NAME:${target}>
		)
		# prepare .pdb files for release after build
		add_custom_command( TARGET ${target}
			POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E echo "${target} -> $<SHELL_PATH:${dir}/$<TARGET_PDB_FILE_NAME:${target}>>"
				COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE_DIR:${target}>/$<TARGET_PDB_FILE_NAME:${target}>.PUBLIC ${dir}/$<TARGET_PDB_FILE_NAME:${target}>
		)
	endif()

	message( STATUS "Configured ${target}" )

endforeach()

##
## prettify IDE
##

source_group( " "           REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "AngelScript" REGULAR_EXPRESSION "${FO_DIR}/AngelScript" )
source_group( "CMake"       REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )
source_group( "Shared"      REGULAR_EXPRESSION "${FO_DIR}/Shared" )
source_group( "Misc"
	FILES
		"${CMAKE_SOURCE_DIR}/Resources/VersionInfo.rc"
)

message( STATUS "Configuring engine... OK" )
