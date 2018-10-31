function( DisableInSourceBuild )

	set( CMAKE_DISABLE_SOURCE_CHANGES ON PARENT_SCOPE )
	set( CMAKE_DISABLE_IN_SOURCE_BUILD ON PARENT_SCOPE )

	get_filename_component( srcdir "${CMAKE_SOURCE_DIR}" REALPATH )
	get_filename_component( bindir "${CMAKE_BINARY_DIR}" REALPATH )

	if( ${srcdir} STREQUAL ${bindir} )
		file( REMOVE ${srcdir}/cmake_install.cmake ) # removing other files won't work
		message( STATUS "In-Source builds are not allowed,
	   cmake must be ran from dedicated build directory.

	   Recommended
		${srcdir}/SDK*/

	   Following directories and files MUST be deleted manually
	   before running cmake again, from any location.
		${srcdir}/CMakeFiles/
		${srcdir}/CMakeCache.txt
	" )
		message( FATAL_ERROR "In-Source build" )
	endif()

endfunction()
