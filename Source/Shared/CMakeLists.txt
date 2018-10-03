##
## Shared
##

add_library( Shared "" )
target_sources( Shared
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Core.h
		${CMAKE_CURRENT_SOURCE_DIR}/Crypt.h
		${CMAKE_CURRENT_SOURCE_DIR}/Defines.h
		${CMAKE_CURRENT_SOURCE_DIR}/GameOptions.h
		${CMAKE_CURRENT_SOURCE_DIR}/FileSystem.h
		${CMAKE_CURRENT_SOURCE_DIR}/Mutex.h
		${CMAKE_CURRENT_SOURCE_DIR}/PUBLIC.Defines.h
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
target_include_directories( Shared
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${AS_DIR}
	PRIVATE
		${CMAKE_BINARY_DIR}/include
)
target_link_libraries( Shared
	PUBLIC
		sha2 zlib
	PRIVATE
		pthreadVC2 winmm
)

source_group( " " REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "Misc"
	FILES
		"${CMAKE_BINARY_DIR}/include/CMake.h"
		"${CMAKE_SOURCE_DIR}/Resources/VersionInfo.rc"
	REGULAR_EXPRESSION
		"CMakeLists"
)
