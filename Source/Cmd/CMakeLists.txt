cmake_minimum_required( VERSION 3.13.4 FATAL_ERROR )
project( FOClassic VERSION 0.1
	DESCRIPTION  "FOnline Community Edition (r412 base)"
	HOMEPAGE_URL "https://rotators.fodev.net/foclassic/FOCmd.md"
	LANGUAGES CXX
)

add_executable( FOCmd "" )
target_sources( FOCmd
	PRIVATE
		${SOURCE_DIR}/App.cpp

		Command.h
		MainCmd.cpp
		ShowWorld.cpp
		ShowWorld.h
		ShowWorld/WorldSave.cpp
		ShowWorld/WorldSave.h
		ShowWorld/WorldSaveDump.cpp
		ShowWorld/WorldSaveDump.h
)
target_include_directories( FOCmd PRIVATE ${FOCLASSIC_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries( FOCmd Platform Shared )

if( MSVC )
	# see https://docs.microsoft.com/en-us/cpp/build/reference/opt-optimizations
	set_property( TARGET FOCmd APPEND_STRING PROPERTY LINK_FLAGS "/OPT:REF " )
endif()

PdbSetup( FOCmd )

set_property( TARGET FOCmd PROPERTY FOLDER "${FOCLASSIC_FOLDER_PREFIX}/Tools" )

source_group( " "           REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh][Pp][Pp]|[Hh])$" )
source_group( "AngelScript" REGULAR_EXPRESSION "${SOURCE_DIR}/AngelScript" )
source_group( "CMake"       REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )
source_group( "Shared"      REGULAR_EXPRESSION "${SOURCE_DIR}/Shared" )
source_group( "Misc"
	FILES
		"${FOCLASSIC_ROOT_DIR}/Resources/VersionInfo.rc"
)

source_group( "ShowWorld"   REGULAR_EXPRESSION "^${CMAKE_CURRENT_LIST_DIR}/ShowWorld/" )

message( STATUS "Configured FOCmd" )
