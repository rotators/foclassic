function( FormatSource filename root var )

	set( ${var} FALSE )
	set( uncrustemp "${root}/FormatSource.tmp" )
	set( uncrustify "${root}/Source/SourceTools/uncrustify" )

	# in case of cancelled FormatSource runs
	if( EXISTS "${uncrustemp}" )
		file( REMOVE "${uncrustemp}" )
	endif()

	execute_process(
		COMMAND "${uncrustify}${CMAKE_EXECUTABLE_SUFFIX}" -c "${uncrustify}.cfg" -l CPP -f "${filename}" -o "${uncrustemp}" -q --if-changed
	)
	if( EXISTS "${uncrustemp}" )
		set( ${var} TRUE )
		file( RENAME "${uncrustemp}" "${filename}" )
	endif()

	set( ${var} ${${var}} PARENT_SCOPE )

endfunction()
