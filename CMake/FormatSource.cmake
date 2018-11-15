function( FormatSource filename root var )

	set( ${var} FALSE )
	set( uncrustemp "${root}/FormatSource.tmp" )
	set( uncrustify "${root}/Source/SourceTools/uncrustify${CMAKE_EXECUTABLE_SUFFIX}" )
	set( uncrustcfg "${root}/Source/SourceTools/uncrustify.cfg}" )

	if( UNCRUSTIFY_EXECUTABLE )
		set( uncrustify "${UNCRUSTIFY_EXECUTABLE}" )
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
