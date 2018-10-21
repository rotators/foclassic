#####
#
# FOClassic v@FOCLASSIC_VERSION@
# Timestamp @EXTENSIONS_CONTENT_TIMESTAMP@
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
#
# For fully automating building process, additional command can be used:
# cmake --build . --config Release
#
# Currenly only Visal Studio 2010 is supported.
# Note that -G parameters is required if there is multiple Visual Studio versions installed.
#
#####
#
# For interested developers, it's possible to create Visual Studio 2017 solution,
# which allows extensions to be developed using modern IDE, but compiled with VS2010 tools.
# cmake -G "Visual Studio 15" -T v100 FOClassic.dir/Extensions/Example
#
# Without using CMake:
# Project properties -> General -> Platform Toolset -> Visual Studio 10 (2010)
#
# In both scenarios VS2010 must be installed.
#
#####

cmake_minimum_required( VERSION @CMAKE_MINIMUM_REQUIRED_VERSION@ FATAL_ERROR )

project( ExampleExtensions
	DESCRIPTION  "FOClassic extensions example"
	HOMEPAGE_URL "@CMAKE_PROJECT_HOMEPAGE_URL@"
	LANGUAGES CXX
)

set( CMAKE_CXX_STANDARD @CMAKE_CXX_STANDARD@ )
set( CMAKE_SKIP_INSTALL_RULES TRUE )

get_filename_component( FOCLASSIC_RELEASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" REALPATH )

# set required paths
# in real setup, it should point to wherever released files are copied,
# and get_filename_component() line removed
set( FOCLASSIC_CMAKE_DIR   "${FOCLASSIC_RELEASE_DIR}/CMake" )
set( FOCLASSIC_HEADERS_DIR "${FOCLASSIC_RELEASE_DIR}/Headers" )

# include FOClassic.cmake
list( APPEND CMAKE_MODULE_PATH "${FOCLASSIC_CMAKE_DIR}" )
include(FOClassic)

# create extension targets using provided function
# in this example, we create dummy extension for each target

FOClassicExtension( ExampleClient    CLIENT "${FOCLASSIC_HEADERS_DIR}" )
FOClassicExtension( ExampleMapper    MAPPER "${FOCLASSIC_HEADERS_DIR}" )
FOClassicExtension( ExampleServer    SERVER "${FOCLASSIC_HEADERS_DIR}" )

# iterate over a list of all targets added in current diectory

get_property( FOCLASSIC_EXTENSION_LIST DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY BUILDSYSTEM_TARGETS )
foreach( target IN LISTS FOCLASSIC_EXTENSION_LIST )

	# populate sources list for all targets

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

# Extensions sources are split into following categories:
#
# TARGET
# |- CMake      - all files with "CMake" in name
# |- Engine     - FOClassic.h and FOClassic.fos
# |- Scripts    - all other *.fos files
# `- source.ext - *.c, *.cpp, *.h, *.hpp are extracted from default groups
source_group( " "       REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh]|[Hh][Pp][Pp])$" )
source_group( "Scripts" REGULAR_EXPRESSION "\.[Ff][Oo][Ss]$" )
source_group( "Engine"  REGULAR_EXPRESSION "FOClassic\.(h|fos)$" )
source_group( "CMake"   REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" ) 
