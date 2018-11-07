##
### On/Off option
### will set "OPTION_{NAME}" variable to 1 if enabled, which can be used in configure_file() templates
##
## SourceOption( USE_THING "Use Thing" ON )
## SourceOptions()
##
### On/Off option which requires own function/macro
### will set "OPTION_{NAME}" variable to 1 if enabled, which can be used in configure_file() templates
### function/macro will be called only if option is enabled
##
## SourceOption( USE_THING "Use Thing" ON )
##
## macro( source_option_USE_THING )
##	add_definitions( "-DUSE_THING=1" )
## endmacro()
##
## source_options()
##

macro( SourceOption name description status )
	list( APPEND SOURCE_OPTIONS ${name} )
	set( SOURCE_OPTION_TEXT_${name} "${description} ..." )
	set( SOURCE_OPTION_VALUE_${name} ${status} )
endmacro()

macro( SourceOptions )
	message( STATUS "Checking source options.." )
	# prepare fancy table with all options
	set( max_len 0 )
	foreach( opt ${SOURCE_OPTIONS} )
		string( LENGTH "${SOURCE_OPTION_TEXT_${opt}}" this_len )
		if( this_len GREATER max_len )
			set( max_len ${this_len} )
		endif()
	endforeach()

	# display fancy table with all options and their status
	foreach( opt ${SOURCE_OPTIONS} )
		set( this_text "${SOURCE_OPTION_TEXT_${opt}}" )
		string( LENGTH "${this_text}" this_len )
		while( this_len LESS ${max_len} )
			set( this_text "${this_text}." )
			string( LENGTH "${this_text}" this_len )
		endwhile()
		message( STATUS " ${this_text} ${SOURCE_OPTION_VALUE_${opt}}" )
	endforeach()

	# cleanup
	unset( max_len )
	unset( this_len )
	unset( this_text )

	# check if there's anything to do
	list( LENGTH SOURCE_OPTIONS llen )
	if( ${llen} )
		# there's some work, add info
		message( STATUS "Applying source options..." )
		foreach( opt ${SOURCE_OPTIONS} )
			# check if option is enabled
			if( SOURCE_OPTION_VALUE_${opt} )
	
				set( OPTION_${opt} "${SOURCE_OPTION_VALUE_${opt}}" )

				# check if option have its own macro/function
				if( COMMAND "SourceOption_${opt}" )
					# it does, we need to call SourceOption_${opt}() now, but cmake doesn't like such syntax
					# let's create and include external file to workaround that
					set( invoke "${CMAKE_CURRENT_BINARY_DIR}/CMake/SourceOptions/${opt}.cmake" )
					if( NOT EXISTS "${invoke}" )
						file( WRITE "${invoke}" "SourceOption_${opt}()\n" )
					endif()
					# call our macro/function
					include( "${invoke}" )
				endif()
			endif()
		endforeach()
		message( STATUS "Applying source options... OK" )
	else()
		message( STATUS "Checking source options... not set" )
	endif()

	# cleanup
	unset( opt )
	unset( llen )
	unset( invoke )
endmacro()
