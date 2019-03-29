#####
#
# FOClassicExtensionInit()
# Prepares helper targets for extensions
#
# Creates following targets:
#
# - ExtensionClient, ExtensionMapper, ExtensionServer
#   interface targets adding minimal required settings for extension targets (include directories, definitions, etc.)
#
# - ExtensionListClient, ExtensionListMapper, ExtensionListServer
#   interface targets used for actual linking extensions to engine
#
# This function is called automagically by engine setup; projects which wants to use their own CMakeLists.txt only, can include this file
# before adding any extension:
#
#    include( path/to/engine/root/dir/CMake/Extensions.cmake )
#    FOClassicExtensionInit( ${CMAKE_CURRENT_LIST_DIR}/path/to/engine/root/dir )
#
#####
#
# Usage: FOClassicExtensionInit( engine_root_dir )
#
# engine_root_dir - path to root directory of engine repository (where README.md and LICENSE.md are)
#
#####

function( FOClassicExtensionInit engine_root_dir )

	if( "${engine_root_dir}" MATCHES "(.+)[/]+" )
		set( engine_root_dir "${CMAKE_MATCH_1}" )
	endif()

	foreach( type IN ITEMS Client Mapper Server )

		# add targets only if they wasn't created already

		if( NOT TARGET Extension${type} )
			add_library( Extension${type} INTERFACE )

			target_include_directories( Extension${type} INTERFACE ${engine_root_dir}/Source )
			target_include_directories( Extension${type} INTERFACE ${engine_root_dir}/Source/Shared )
			target_include_directories( Extension${type} INTERFACE ${engine_root_dir}/Source/AngelScript )
			target_include_directories( Extension${type} INTERFACE ${engine_root_dir}/Source/AngelScript/Addons )

			string( TOUPPER ${type} uctype )
			target_compile_definitions( Extension${type} INTERFACE FOCLASSIC_ENGINE )
			target_compile_definitions( Extension${type} INTERFACE FOCLASSIC_${uctype} )
		endif()

		if( NOT TARGET ExtensionList${type} )
			add_library( ExtensionList${type} INTERFACE )
		endif()

	endforeach()

endfunction()

#####
#
# FOClassicExtension()
# Helper function for attaching extension to executable
#
#####
#
# Usage: FOClassicExtension( type target )
#
# type    - CLIENT / MAPPER / SERVER
#           case insensitive
# target  - CMake target name
#
#####

function( FOClassicExtension type target )

	set( type_raw ${type} )
	set( msg "Extension(${target}):" )

	# check for valid extension types
	if( NOT "${type}" MATCHES "^([Cc][Ll][Ii][Ee][Nn][Tt]|[Mm][Aa][Pp][Pp][Ee][Rr]|[Ss][Ee][Rr][Vv][Ee][Rr])$" )
		message( FATAL_ERROR "{$msg} Invalid extension type '${type}'" )
	endif()

	# check if target exists
	if( NOT TARGET ${target} )
		message( FATAL_ERROR "${msg} CMake target does not exists" )
	endif()

	# TyPe -> Type
	string( TOUPPER ${type} type )
	string( SUBSTRING ${type} 0 1 letter )
	string( TOLOWER ${type} type )
	string( REGEX REPLACE "^.(.+)" "${letter}\\1" type "${type}" )

	# check if helper target exists
	if( NOT TARGET Extension${type} )
		message( FATAL_ERROR "${msg} CMake target 'Extension${type}' does not exists; remember to call FOClassicExtensionInit() before adding any extensions" )
	endif()

	# add helper library
	target_link_libraries( ${target} PUBLIC Extension${type} )

	# configure linking
	FOClassicExtensionAdd( ${type_raw} ${target} )

endfunction()

#####
#
# FOClassicExtensionAdd()
# Helper function for attaching extension to executable
#
# Unlike FOClassicExtension(), this function does not alter target configuration, and does a minimum required to "connect" extension
# with engine. It might be more fitting for projects using their own CMakeLists.txt instead of CMakeFOClassic.txt
#
#####
#
# Usage: FOClassicExtensionAdd( type target )
#
# type    - CLIENT / MAPPER / SERVER
#           case insensitive
# target  - CMake target name
#
#####

function( FOClassicExtensionAdd type target )

	set( msg "Extension(${target}):" )

	# check for valid extension types
	if( NOT "${type}" MATCHES "^([Cc][Ll][Ii][Ee][Nn][Tt]|[Mm][Aa][Pp][Pp][Ee][Rr]|[Ss][Ee][Rr][Vv][Ee][Rr])$" )
		message( FATAL_ERROR "{$msg} Invalid extension type '${type}'" )
	endif()

	# check if target exists
	if( NOT TARGET ${target} )
		message( FATAL_ERROR "${msg} CMake target does not exists" )
	endif()

	# TyPe -> Type
	string( TOUPPER ${type} type )
	string( SUBSTRING ${type} 0 1 letter )
	string( TOLOWER ${type} type )
	string( REGEX REPLACE "^.(.+)" "${letter}\\1" type "${type}" )

	# check if helper target exists
	if( NOT TARGET ExtensionList${type} )
		message( FATAL_ERROR "${msg} CMake target 'ExtensionList${type}' does not exists; remember to call FOClassicExtensionInit() before adding any extensions" )
	endif()

	# configure linking
	# minimum required to "connect" extension and executable targets
	cmake_policy( PUSH )
	cmake_policy( SET CMP0079 NEW )
	target_link_libraries( ExtensionList${type} INTERFACE ${target} )
	cmake_policy( POP )

endfunction()
