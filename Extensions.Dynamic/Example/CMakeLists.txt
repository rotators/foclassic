#####
#
# FOClassic v@FOCLASSIC_VERSION@
# Timestamp @EXTENSIONS_CONTENT_TIMESTAMP@
#
#####
#
# This file contains an example CMake setup for FOClassic extensions.
#
#####
#
# mkdir ExampleExtensions.dir
# cd ExampleExtensions.dir
# cmake -G "Visual Studio 10" FOClassic.dir/Extensions/Example
#
# For fully automating building process, additional command can be used:
# cmake --build ExampleExtensions.dir --config Release
#
# Only Visal Studio 2010 is supported.
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

set( CMAKE_CXX_STANDARD           @CMAKE_CXX_STANDARD@ )
set( CMAKE_CXX_STANDARD_REQUIRED  @CMAKE_CXX_STANDARD_REQUIRED@ )
set( CMAKE_SKIP_INSTALL_RULES     TRUE )

get_filename_component( FOCLASSIC_RELEASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" REALPATH )

# set required paths
# in real setup, it should point to wherever released files are copied,
# and get_filename_component() line removed
set( FOCLASSIC_CMAKE_DIR   "${FOCLASSIC_RELEASE_DIR}/CMake" )
set( FOCLASSIC_HEADERS_DIR "${FOCLASSIC_RELEASE_DIR}/Headers" )

# include cmake functions provided by engine
list( APPEND CMAKE_MODULE_PATH "${FOCLASSIC_CMAKE_DIR}" )
include(FOClassic)

# create extension targets using provided function
# in this example, we create dummy extensions for each target
FOClassicExtension( ExampleClient CLIENT "${FOCLASSIC_HEADERS_DIR}" )
FOClassicExtension( ExampleMapper MAPPER "${FOCLASSIC_HEADERS_DIR}" )
FOClassicExtension( ExampleServer SERVER "${FOCLASSIC_HEADERS_DIR}" )

# for testing purposes, we add a copy of each extension;
# previously added will be built with Bleeding Mode enabled
# in real setup, only one scenario should be used
FOClassicExtension( ExampleClientOld CLIENT "${FOCLASSIC_HEADERS_DIR}" )
FOClassicExtension( ExampleMapperOld MAPPER "${FOCLASSIC_HEADERS_DIR}" )
FOClassicExtension( ExampleServerOld SERVER "${FOCLASSIC_HEADERS_DIR}" )

set( FOCLASSIC_HEADERS_FILES
	# AngelScript headers
	${FOCLASSIC_HEADERS_DIR}/AngelScript/angelscript.h
	${FOCLASSIC_HEADERS_DIR}/AngelScript/scriptany.h
	${FOCLASSIC_HEADERS_DIR}/AngelScript/scriptarray.h
	${FOCLASSIC_HEADERS_DIR}/AngelScript/scriptdictionary.h
	${FOCLASSIC_HEADERS_DIR}/AngelScript/scriptfile.h
	${FOCLASSIC_HEADERS_DIR}/AngelScript/scriptmath.h
	${FOCLASSIC_HEADERS_DIR}/AngelScript/scriptstring.h

	# additional headers
	${FOCLASSIC_HEADERS_DIR}/CommandLine.h
	${FOCLASSIC_HEADERS_DIR}/Ini.h
)

# iterate over a list of all targets added in current diectory
get_property( cmake_targets DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY BUILDSYSTEM_TARGETS )
foreach( name IN LISTS cmake_targets )

	# example of filtering extensions out of unknown target list
	# not useful here, as there is no non-extension targets added
	get_property( is_extension TARGET ${name} PROPERTY FOCLASSIC_EXTENSION )
	if( NOT is_extension )
		continue()
	endif()

	# extensions with "Old" suffix depend on those without
	# extensions without "Old" suffix are built with Bleeding Edge mode enabled
	if( "${name}" MATCHES "(.+)Old$" )
		add_dependencies( ${name} ${CMAKE_MATCH_1} )
	else()
		target_compile_definitions( ${name} PUBLIC FOCLASSIC_BLEEDING_EDGE )
	endif()

	# populate sources list for all targets
	target_sources( ${name}
		PRIVATE
			# CMakeLists.txt needs to be added for source_group("CMake") to work properly
			${CMAKE_CURRENT_LIST_FILE}

			# Engine headers
			${FOCLASSIC_HEADERS_FILES}

			# while extensions must compile without _defines.fos, there's high chance
			# they'll need game-specific values. it's included in this example
			# just to show how to setup .fos headers to work with extensions and scripts
			_defines.fos

			# example contains only includes and empty extension entry point
			extension.cpp
	)

	# current directory should be always in includes list
	target_include_directories( ${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )

endforeach()

# extensions sources are split into following categories:
#
# TARGET
# |- CMake          - all files with "CMake" in name
# |- Engine         - all headers provided by FOClassic
# |  `- AngelScript - AngelScript headers
# |- Scripts        - all other *.fos files
# `- source.ext     - *.c, *.cpp, *.h, *.hpp are extracted from default groups
#
# in real setup it can be safely ignored - source_group() commands does not affect build process at all

source_group( " "                             REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh]|[Hh][Pp][Pp])$" )
source_group( "Scripts"                       REGULAR_EXPRESSION "\.[Ff][Oo][Ss]$" )
source_group( TREE "${FOCLASSIC_HEADERS_DIR}" PREFIX "Engine" FILES ${FOCLASSIC_HEADERS_FILES} )
source_group( "Engine"                        REGULAR_EXPRESSION "FOClassic\.(h|fos)$" )
source_group( "CMake"                         REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )
