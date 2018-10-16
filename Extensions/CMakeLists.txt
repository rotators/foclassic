#
# This file contains example setup for FOClassic extensions
#
# mkdir ExampleExtensions.buildir
# cd ExampleExtensions.buildir
# cmake -G "Visual Studio 10" FOClassic.dir/Extensions/Example
# cmake --build ExampleExtensions.buildir --config Release
#
cmake_minimum_required( VERSION 3.12.2 )

message( STATUS "Configuring extensions..." )

#
# helper function for fast/easy creation of extension targets
#
# name    - cmake target name
# target  - CLIENT / MAPPER / SERVER
#           case insensitive
# headers - directory with foclassic.h and foclassic.fos
#           absolute path is required
#
function( ExtensionTarget name target headers )
	if( TARGET ${name} )
		message( FATAL_ERROR "CMake target '${name}' already exists" )
	else()
		add_library( ${name} SHARED "" )
	endif()
	#
	# add required defines
	#
	if( target MATCHES "^([Cc][Ll][Ii][Ee][Nn][Tt]|[Mm][Aa][Pp][Pp][Ee][Rr]|[Ss][Ee][Rr][Vv][Ee][Rr])$" )
		set( target "__${target}" )
		string( TOUPPER ${target} target )
		target_compile_definitions( ${name} PUBLIC FOCLASSIC_EXTENSION ${target} )
	else()
		message( FATAL_ERROR "Invalid target '${target}'" )
	endif()
	#
	# add headers directory to include list
	#
	if( NOT EXISTS "${headers}" )
		message( FATAL_ERROR "Directory '${headers}' does not exists" )
	else()
		target_include_directories( ${name} PUBLIC ${headers} )
	endif()
	#
	# add headers files to target
	#
	foreach( header IN ITEMS foclassic.h foclassic.fos )
		if( NOT EXISTS "${headers}/${header}" )
			message( FATAL_ERROR "File '${headers}/${header}' does not exists" )
		else()
			target_sources( ${name} PUBLIC ${headers}/${header} )
		endif()
	endforeach()
	#
	# add required compilation options
	#
	if( MSVC )
		set_property( TARGET ${name} PROPERTY COMPILE_OPTIONS "/MT" )
	endif()
	#
	# add information for developer
	#
	string( REPLACE "__" "" target "${target}" )
	string( TOLOWER "${target}" target )
	message( STATUS "Configured ${target} extension '${name}'" )

endfunction()

if( NOT PROJECT_NAME STREQUAL "FOClassic" )
	#
	# setup for servers
	#
	ExtensionTarget( ExtensionClient CLIENT "${CMAKE_CURRENT_SOURCE_DIR}/../../Headers" )
	ExtensionTarget( ExtensionMapper MAPPER "${CMAKE_CURRENT_SOURCE_DIR}/../../Headers" )
	ExtensionTarget( ExtensionServer SERVER "${CMAKE_CURRENT_SOURCE_DIR}/../../Headers" )
else()
	#
	# setup for engine
	#
	ExtensionTarget( ExtensionClient CLIENT "${RELEASE_HEADERS_DIR}" )
	ExtensionTarget( ExtensionMapper MAPPER "${RELEASE_HEADERS_DIR}" )
	ExtensionTarget( ExtensionServer SERVER "${RELEASE_HEADERS_DIR}" )
endif()

foreach( target IN ITEMS ExtensionClient ExtensionMapper ExtensionServer )
	#
	# populate sources list for all targets
	# in this example all targets share same source files
	#
	target_sources( ${target}
		PRIVATE
			#
			# CMakeLists.txt needs to be added for source_group() to work properly
			#
			CMakeLists.txt
			#
			# while extensions must compile without _defines.fos, there's high chance
			# extension will need server-specific values. it's included in this example
			# just to show how to setup .fos headers for both, extensions and scripts
			#
			_defines.fos
			#
			# example contains only includes and empty extension entry point
			#
			extension.cpp
	)

	#
	# current directory should be always in includes list
	#
	target_include_directories( ${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )
endforeach()

#
# additional, engine-only configuration
#
if( "${PROJECT_NAME}" STREQUAL "FOClassic" )

	foreach( target IN ITEMS ExtensionClient ExtensionMapper ExtensionServer )
		#
		# enable Bleeding Edge mode
		#
		target_compile_definitions( ${target} PUBLIC FOCLASSIC_BLEEDING_EDGE )
		#
		# add sources used for testing
		#
		target_sources( ${target} PRIVATE ${EXTENSIONS_EXTRA_SOURCES} )
		#
		# prepare this file for release
		#
		file( COPY "${CMAKE_CURRENT_LIST_FILE}" DESTINATION "${RELEASE_EXTENSIONS_EXAMPLE_DIR}" )
		#
		# move project into fitting solution folder
		# global property USE_FOLDERS must be enabled
		# does not work with VS2010 Express
		#
		set_property( TARGET ${target} PROPERTY FOLDER "Extensions/Example" )
	endforeach()

endif()

#
# Extensions sources are split into following categories:
#
# TARGET
# |- CMake         - all files with "CMake" in name
# |- Engine        - foclassic.h and foclassic.fos
# |- Scripts       - all other *.fos files
# `- extension.cpp - *.c, *.cpp, *.h, *.hpp are extracted from default groups
#
source_group( " "       REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh]|[Hh][Pp][Pp])$" )
source_group( "Scripts" REGULAR_EXPRESSION "\.fos$" )
source_group( "Engine"  REGULAR_EXPRESSION "foclassic\.(h|fos)$" )
source_group( "CMake"   REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )

message( STATUS "Configuring extensions... OK" )
