##
## Shared
##

find_package( Threads )

add_library( Shared "" )
target_sources( Shared
	PUBLIC
		${ENGINE_GLOBAL_HEADER_FILE}

		${CMAKE_CURRENT_LIST_DIR}/AutoPointers.h
		${CMAKE_CURRENT_LIST_DIR}/CommandLine.h
		${CMAKE_CURRENT_LIST_DIR}/Core.h
		${CMAKE_CURRENT_LIST_DIR}/Crypt.h
		${CMAKE_CURRENT_LIST_DIR}/Defines.h
		${CMAKE_CURRENT_LIST_DIR}/Defines.Public.h
		${CMAKE_CURRENT_LIST_DIR}/DynamicLibrary.h
		${CMAKE_CURRENT_LIST_DIR}/GameOptions.h
		${CMAKE_CURRENT_LIST_DIR}/FileSystem.h
		${CMAKE_CURRENT_LIST_DIR}/Ini.h
		${CMAKE_CURRENT_LIST_DIR}/Mutex.h
		${CMAKE_CURRENT_LIST_DIR}/PlatformSpecific.h
		${CMAKE_CURRENT_LIST_DIR}/Random.h
		${CMAKE_CURRENT_LIST_DIR}/StaticAssert.h
		${CMAKE_CURRENT_LIST_DIR}/Text.h
		${CMAKE_CURRENT_LIST_DIR}/Thread.h
		${CMAKE_CURRENT_LIST_DIR}/Timer.h
		${CMAKE_CURRENT_LIST_DIR}/Types.h

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
		Ini.cpp
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
		${CMAKE_CURRENT_LIST_DIR}
		${AS_DIR}
	PRIVATE
		${FOCLASSIC_INCLUDE_DIR}
		${LIBS_DIR}/fltk.bin # Text.cpp -> FL/case.h
)

target_link_libraries( Shared PUBLIC sha2 zlib Threads::Threads )
if( WIN32 )
	target_link_libraries( Shared
		PUBLIC
			pthread
		PRIVATE
			winmm
	)
	PdbSetup( Shared )
endif()

source_group( " "     REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "CMake" REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )
