function( FormatSource filename root var )

	set( ${var} FALSE )
	set( uncrustemp "${root}/FormatSource.tmp" )
	set( uncrustify "${root}/Source/SourceTools/uncrustify" )
	set( uncrustcfg "${root}/Source/SourceTools/uncrustify.cfg" )

	if( UNCRUSTIFY_EXECUTABLE )
		set( uncrustify "${UNCRUSTIFY_EXECUTABLE}" )
	endif()

	# CMAKE_EXECUTABLE_SUFFIX is not reliable
	if( WIN32 AND NOT "${uncrustify}" MATCHES "\.[Ee][Xx][Ee]" )
		string( APPEND uncrustify ".exe" )
	endif()

	# in case of cancelled FormatSource runs
	if( EXISTS "${uncrustemp}" )
		file( REMOVE "${uncrustemp}" )
	endif()

	execute_process( COMMAND "${uncrustify}" -c "${uncrustcfg}" -l CPP -f "${filename}" -o "${uncrustemp}" -q --if-changed )

	if( EXISTS "${uncrustemp}" )
		set( ${var} TRUE )
		file( RENAME "${uncrustemp}" "${filename}" )
	endif()

	set( ${var} ${${var}} PARENT_SCOPE )

endfunction()
