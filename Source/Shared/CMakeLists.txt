##
## Shared
##

add_library( Shared "" )
target_sources( Shared
	PUBLIC
		${ENGINE_GLOBAL_HEADER_FILE}

		${CMAKE_CURRENT_SOURCE_DIR}/AutoPointers.h
		${CMAKE_CURRENT_SOURCE_DIR}/CommandLine.h
		${CMAKE_CURRENT_SOURCE_DIR}/Core.h
		${CMAKE_CURRENT_SOURCE_DIR}/Crypt.h
		${CMAKE_CURRENT_SOURCE_DIR}/Defines.h
		${CMAKE_CURRENT_SOURCE_DIR}/Defines.Public.h
		${CMAKE_CURRENT_SOURCE_DIR}/DynamicLibrary.h
		${CMAKE_CURRENT_SOURCE_DIR}/GameOptions.h
		${CMAKE_CURRENT_SOURCE_DIR}/FileSystem.h
		${CMAKE_CURRENT_SOURCE_DIR}/Mutex.h
		${CMAKE_CURRENT_SOURCE_DIR}/PlatformSpecific.h
		${CMAKE_CURRENT_SOURCE_DIR}/Random.h
		${CMAKE_CURRENT_SOURCE_DIR}/Text.h
		${CMAKE_CURRENT_SOURCE_DIR}/Thread.h
		${CMAKE_CURRENT_SOURCE_DIR}/Timer.h
		${CMAKE_CURRENT_SOURCE_DIR}/Types.h

		${AS_DIR}/scriptany.h
		${AS_DIR}/scriptarray.h
		${AS_DIR}/scriptfile.h
		${AS_DIR}/scriptmath.h
		${AS_DIR}/scriptstring.h
	PRIVATE
		CommandLine.cpp
		Core.cpp
		Crypt.cpp
		FileSystem.cpp
		GameOptions.cpp
		Mutex.cpp
		Random.cpp
		Thread.cpp
		Timer.cpp
		Text.cpp

		${AS_DIR}/scriptany.cpp
		${AS_DIR}/scriptarray.cpp
		${AS_DIR}/scriptdictionary.cpp
		${AS_DIR}/scriptdictionary.h
		${AS_DIR}/scriptfile.cpp
		${AS_DIR}/scriptmath.cpp
		${AS_DIR}/scriptstring.cpp
)
target_compile_definitions( Shared
	PUBLIC
		FOCLASSIC_ENGINE
)
if( WIN32 )
	target_compile_definitions( Shared PRIVATE WINVER=0x0501 ) # Windows XP
	target_compile_definitions( Shared PRIVATE WIN32_LEAN_AND_MEAN )
endif()

target_include_directories( Shared
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${AS_DIR}
	PRIVATE
		${DEFAULT_INCLUDE_DIR}
		${CMAKE_SOURCE_DIR}/Source/Libs/fltk # Text.cpp -> FL/case.h
)
target_link_libraries( Shared
	PUBLIC
		pthread sha2 zlib
	PRIVATE
		winmm
)

source_group( " "     REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "CMake" REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )
source_group( "Misc"
	FILES
		"${CMAKE_SOURCE_DIR}/Resources/VersionInfo.rc"
)
