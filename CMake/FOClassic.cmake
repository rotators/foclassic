#####
#
# FOClassic v@FOCLASSIC_VERSION@
# Timestamp @EXTENSIONS_CONTENT_TIMESTAMP@
# 
#####

#####
#
# FOClassicExtension()
# Helper function for fast/easy creation of extension targets
#
#####
#
# Usage: FOClassicExtension( name target headers )
#
# name    - CMake target name
# target  - CLIENT / MAPPER / SERVER
#           case insensitive
# headers - directory with FOClassic.h and FOClassic.fos
#           absolute path is required
#
#####
#
# CMake properties set for each created target:
#
#  FOCLASSIC_EXTENSION              -  always TRUE
#  FOCLASSIC_EXTENSION_TARGET       -  copy of 'target' argument (uppercase)
#  FOCLASSIC_EXTENSION_HEADERS_DIR  -  copy of 'headers' argument (CMake path)
#
function( FOClassicExtension name target headers )

	# create empty target
	if( TARGET ${name} )
		message( FATAL_ERROR "CMake target '${name}' already exists" )
	endif()
	add_library( ${name} SHARED "" )
	set_property( TARGET ${name} PROPERTY FOCLASSIC_EXTENSION TRUE )

	# add required defines
	# TaRgEt -> __TARGET
	if( NOT "${target}" MATCHES "^([Cc][Ll][Ii][Ee][Nn][Tt]|[Mm][Aa][Pp][Pp][Ee][Rr]|[Ss][Ee][Rr][Vv][Ee][Rr])$" )
		message( FATAL_ERROR "Invalid extension target '${target}'" )
	endif()
	string( TOUPPER ${target} target )
	set_property( TARGET ${name} PROPERTY FOCLASSIC_EXTENSION_TARGET "${target}" )
	set( target "__${target}" )
	target_compile_definitions( ${name} PUBLIC FOCLASSIC_EXTENSION ${target} )

	# add headers directory to include list
	file( TO_CMAKE_PATH "${headers}" headers )
	file( TO_NATIVE_PATH "${headers}" headers_native )
	if( NOT EXISTS "${headers}" )
		message( FATAL_ERROR "Path '${headers_native}' does not exists" )
	endif()
	if( NOT IS_DIRECTORY "${headers}" )
		message( FATAL_ERROR "Path '${headers_native}' is not a directory" )
	endif()
	if( NOT IS_ABSOLUTE "${headers}" )
		message( FATAL_ERROR "Path '${headers_native}' is not an absolute directory" )
	endif()
	target_include_directories( ${name} PUBLIC ${headers} )
	set_property( TARGET ${name} PROPERTY FOCLASSIC_HEADERS_DIR "${headers}" )

	# add engine headers to sources list
	foreach( header IN ITEMS FOClassic.h FOClassic.fos )
		if( NOT EXISTS "${headers}/${header}" )
			message( FATAL_ERROR "File '${headers_native}/${header}' does not exists" )
		endif()
		target_sources( ${name} PUBLIC "${headers}/${header}" )
	endforeach()

	# add required compilation options
	if( MSVC )
		set_property( TARGET ${name} APPEND_STRING PROPERTY COMPILE_OPTIONS "/MT" )
	endif()

	# add information for developer
	# __TARGET -> target
	string( REPLACE "__" "" target "${target}" )
	string( TOLOWER "${target}" target )
	message( STATUS "Configured ${target} extension '${name}'" )

endfunction()
