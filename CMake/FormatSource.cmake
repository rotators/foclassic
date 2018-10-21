function( FormatSource )

	set( tmp "${CMAKE_CURRENT_LIST_DIR}/FormatSource.tmp" )
	set( uncrustify "${CMAKE_CURRENT_LIST_DIR}/Source/SourceTools/uncrustify" )

	message( STATUS "Formatting source..." )

	foreach( dir IN ITEMS
		CMake/Templates
		Extensions Extensions/Example
		Source/Shared Source )
		foreach( ext IN ITEMS h cpp fos )
			file( GLOB files LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/${dir}/*.${ext} )
			if( files )
				set( all_files "${all_files};${files}" )
			endif()
		endforeach()
	endforeach()

	foreach( file ${all_files} )
		message( STATUS "Processing ${file}" )
		execute_process(
			COMMAND "${uncrustify}.exe" -c "${uncrustify}.cfg" -l CPP -f "${file}" -o "${tmp}" -q --if-changed
		)
		if( EXISTS "${tmp}" )
			message( STATUS "           FormatSource prevails" )
			file( RENAME "${tmp}" "${file}" )
		endif()
	endforeach()

endfunction()

function( FormatSourceNew )

    set( optional )
    set( single COMMAND CONFIG DIRECTORY LANGUAGE )
    set( multi  FILES )

    cmake_parse_arguments( FORMAT_SOURCE "${optional}" "${multi}" "${single}" ${ARGN} )

	foreach( var IN LISTS optional single multi )
		message( STATUS "FORMAT_SOURCE_${var} = ${FORMAT_SOURCE_${var}}")
	endforeach()

	foreach( var IN ITEMS COMMAND CONFIG LANGUAGE )
		if( NOT FORMAT_SOURCE_${var} )
			message( SEND_ERROR "FormatSource(): ${var} not set" )
			return()
		endif()
	endforeach()

	set( tmp "${FOCMAKE_DIR}/FormatSource.tmp" )

	foreach( file IN LISTS FORMAT_SOURCE_FILES )
		message( STATUS "Processing ${file}")
		execute_process(
			COMMAND "${FORMAT_SOURCE_COMMAND}" -c "${FORMAT_SOURCE_CONFIG}" -l "${FORMAT_SOURCE_LANGUAGE}" -f "${file}" -o "${tmp}" -q --if-changed
		)
		if( EXISTS "${tmp}" )
			message( STATUS "           FormatSource prevails" )
			file( RENAME "${tmp}" "${file}" )
		endif()
	endforeach()

endfunction()
