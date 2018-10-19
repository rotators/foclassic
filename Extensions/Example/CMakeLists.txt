#####
#
# FOClassic v@FOCLASSIC_VERSION@
# Last update @EXTENSIONS_CMAKELISTS_TIMESTAMP@
#
#####
#
# This file contains example setup for FOClassic extensions.
#
#####
#
# mkdir ExampleExtensions.dir
# cd ExampleExtensions.dir
# cmake -G "Visual Studio 10" FOClassic.dir/Extensions/Example
# cmake --build . --config Release
#
#####

cmake_minimum_required( VERSION 3.12.2 )

message( STATUS "Configuring extensions..." )

project( ExampleExtensions
	DESCRIPTION  "Example extensions for FOClassic"
	HOMEPAGE_URL "https://rotators.fodev.net/foclassic/"
	LANGUAGES CXX
)

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_SKIP_INSTALL_RULES TRUE )

# helper function for fast/easy creation of extension targets
#
# name    - cmake target name
# target  - CLIENT / MAPPER / SERVER
#           case insensitive
# headers - directory with FOClassic.h and FOClassic.fos
#           absolute path is required

function( FOClassicExtension name target headers )

	# create empty target
	if( TARGET ${name} )
		message( FATAL_ERROR "CMake target '${name}' already exists" )
	endif()
	add_library( ${name} SHARED "" )

	# add required defines
	if( NOT target MATCHES "^([Cc][Ll][Ii][Ee][Nn][Tt]|[Mm][Aa][Pp][Pp][Ee][Rr]|[Ss][Ee][Rr][Vv][Ee][Rr])$" )
		message( FATAL_ERROR "Invalid target '${target}'" )
	endif()
	set( target "__${target}" )
	string( TOUPPER ${target} target )
	target_compile_definitions( ${name} PUBLIC FOCLASSIC_EXTENSION ${target} )

	# add headers directory to include list
	if( NOT EXISTS "${headers}" )
		message( FATAL_ERROR "Directory '${headers}' does not exists" )
	endif()
	target_include_directories( ${name} PUBLIC ${headers} )

	# add engine headers to sources list
	foreach( header IN ITEMS FOClassic.h FOClassic.fos )
		if( NOT EXISTS "${headers}/${header}" )
			message( FATAL_ERROR "File '${headers}/${header}' does not exists" )
		endif()
		target_sources( ${name} PUBLIC ${headers}/${header} )
	endforeach()

	# add required compilation options
	if( MSVC )
		set_property( TARGET ${name} APPEND_STRING PROPERTY COMPILE_OPTIONS "/MT" )
	endif()

	# add information for developer
	string( REPLACE "__" "" target "${target}" )
	string( TOLOWER "${target}" target )
	message( STATUS "Configured ${target} extension '${name}'" )

endfunction()

# create extension targets using function above

if( NOT COMMAND FOClassic )
	# setup for games
	FOClassicExtension( ExampleClient CLIENT "${CMAKE_CURRENT_SOURCE_DIR}/../../Headers" )
	FOClassicExtension( ExampleMapper MAPPER "${CMAKE_CURRENT_SOURCE_DIR}/../../Headers" )
	FOClassicExtension( ExampleServer SERVER "${CMAKE_CURRENT_SOURCE_DIR}/../../Headers" )
else()
	# setup for engine
	FOClassicExtension( ExampleClient CLIENT "${RELEASE_HEADERS_DIR}" )
	FOClassicExtension( ExampleMapper MAPPER "${RELEASE_HEADERS_DIR}" )
	FOClassicExtension( ExampleServer SERVER "${RELEASE_HEADERS_DIR}" )
endif()

# iterate over a list of all targets added in current diectory

get_property( FOCLASSIC_EXTENSION_LIST DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS )
foreach( target IN LISTS FOCLASSIC_EXTENSION_LIST )

	# populate sources list for all targets
	# in this example all targets share same source files

	target_sources( ${target}
		PRIVATE
			# CMakeLists.txt needs to be added for source_group("CMake") to work properly
			${CMAKE_CURRENT_LIST_FILE}

			# while extensions must compile without _defines.fos, there's high chance
			# they'll need game-specific values. it's included in this example
			# just to show how to setup .fos headers for both, extensions and scripts
			_defines.fos

			# example contains only includes and empty extension entry point
			extension.cpp
	)

	# current directory should be always in includes list
	target_include_directories( ${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )

endforeach()

# additional configuration, required only when building engine
if( COMMAND FOClassic )
	foreach( target IN LISTS FOCLASSIC_EXTENSION_LIST )

		# enable Bleeding Edge mode
		target_compile_definitions( ${target} PUBLIC FOCLASSIC_BLEEDING_EDGE )

		# additional files
		target_sources( ${target} PRIVATE ${EXTENSIONS_HEADER_FILE} )

	endforeach()
endif()

# Extensions sources are split into following categories:
#
# TARGET
# |- CMake      - all files with "CMake" in name
# |- Engine     - FOClassic.h and FOClassic.fos
# |- Scripts    - all other *.fos files
# `- source.ext - *.c, *.cpp, *.h, *.hpp are extracted from default groups
source_group( " "       REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh]|[Hh][Pp][Pp])$" )
source_group( "Scripts" REGULAR_EXPRESSION "\.fos$" )
source_group( "Engine"  REGULAR_EXPRESSION "FOClassic\.(h|fos)$" )
source_group( "CMake"   REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )

message( STATUS "Configuring extensions... OK" )
