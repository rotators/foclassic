##
## Shared
##

find_package( Threads )

add_library( Shared "" )
target_sources( Shared
	PUBLIC
		${ENGINE_GLOBAL_HEADER_FILE}

		${CMAKE_CURRENT_LIST_DIR}/App.h
		${CMAKE_CURRENT_LIST_DIR}/AutoPointers.h
		${CMAKE_CURRENT_LIST_DIR}/CommandLine.h
		${CMAKE_CURRENT_LIST_DIR}/Core.h
		${CMAKE_CURRENT_LIST_DIR}/Crypt.h
		${CMAKE_CURRENT_LIST_DIR}/DataFile.h
		${CMAKE_CURRENT_LIST_DIR}/Defines.h
		${CMAKE_CURRENT_LIST_DIR}/Defines.Public.h
		${CMAKE_CURRENT_LIST_DIR}/DynamicLibrary.h
		${CMAKE_CURRENT_LIST_DIR}/Exception.h
		${CMAKE_CURRENT_LIST_DIR}/FileManager.h
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

		${AS_ADDONS_DIR}/scriptany.h
		${AS_ADDONS_DIR}/scriptarray.h
		${AS_ADDONS_DIR}/scriptfile.h
		${AS_ADDONS_DIR}/scriptmath.h
		${AS_ADDONS_DIR}/scriptstring.h
	PRIVATE
		CommandLine.cpp
		Core.cpp
		Crypt.cpp
		DataFile.cpp
		Exception.cpp
		FileManager.cpp
		FileSystem.cpp
		GameOptions.cpp
		Ini.cpp
		Mutex.cpp
		Random.cpp
		Thread.cpp
		Timer.cpp
		Text.cpp

		DataFile/Dat.cpp
		DataFile/Dat.h
		DataFile/Zip.cpp
		DataFile/Zip.h

		${AS_ADDONS_DIR}/scriptany.cpp
		${AS_ADDONS_DIR}/scriptarray.cpp
		${AS_ADDONS_DIR}/scriptdictionary.cpp
		${AS_ADDONS_DIR}/scriptdictionary.h
		${AS_ADDONS_DIR}/scriptfile.cpp
		${AS_ADDONS_DIR}/scriptmath.cpp
		${AS_ADDONS_DIR}/scriptstring.cpp
)
target_compile_definitions( Shared
	PUBLIC
		FOCLASSIC_ENGINE
	PRIVATE
		FOCLASSIC_LIB_SHARED
)
if( WIN32 )
	target_compile_definitions( Shared PRIVATE WINVER=0x0501 ) # Windows XP
	target_compile_definitions( Shared PRIVATE WIN32_LEAN_AND_MEAN )
endif()

target_include_directories( Shared
	PUBLIC
		${CMAKE_CURRENT_LIST_DIR}
		${AS_DIR}
		${AS_ADDONS_DIR}
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

source_group( " "        REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "Addons"   REGULAR_EXPRESSION "^${AS_ADDONS_DIR}/" )
source_group( "DataFile" REGULAR_EXPRESSION "^${CMAKE_CURRENT_LIST_DIR}/DataFile/" )
source_group( "CMake"    REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )
set_property( TARGET Shared PROPERTY FOLDER "${FOCLASSIC_FOLDER_PREFIX}/Libs.Internal" )
