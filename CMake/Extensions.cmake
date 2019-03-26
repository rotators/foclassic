#####
#
# FOClassicExtension()
# Helper function for attaching extension to executable
#
#####
#
# Usage: FOClassicExtension( name targets )
#
# name     - CMake target name
# targets  - CLIENT / MAPPER / SERVER
#            more than one target can be specified
#            case insensitive
#
#####

function( FOClassicExtension name targets )
	message( STATUS "Configuring extension '${name}'" )

	set( msg "Extension(${name}):" )

	# check if name is existing cmake target
	if( NOT TARGET ${name} )
		message( FATAL_ERROR "${msg} CMake target does not exists" )
	endif()

	# validate targets names
	foreach( target IN LISTS targets )
		if( NOT "${target}" MATCHES "^([Cc][Ll][Ii][Ee][Nn][Tt]|[Mm][Aa][Pp][Pp][Ee][Rr]|[Ss][Ee][Rr][Vv][Ee][Rr])$" )
			message( FATAL_ERROR "{$msg} Invalid target '${target}'" )
		endif()
	endforeach()

	# add extension to list
	foreach( target IN LISTS targets )
		string( TOUPPER "${target}" target )

		get_property( extensions GLOBAL PROPERTY FOCLASSIC_${target}_EXTENSIONS )
		list( FIND extensions ${name}  idx )
		if( idx GREATER_EQUAL 0 )
			string( TOLOWER "${target}" target )
			message( WARNING "${msg} Already added to ${target} extensions list" )
			string( TOUPPER "${target}" target )
			continue()
		else()
			set_property( GLOBAL APPEND PROPERTY FOCLASSIC_${target}_EXTENSIONS ${name} )
		endif()
	endforeach()
endfunction()
