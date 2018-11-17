function( DisableInSourceBuild )

	set( CMAKE_DISABLE_SOURCE_CHANGES ON PARENT_SCOPE )
	set( CMAKE_DISABLE_IN_SOURCE_BUILD ON PARENT_SCOPE )

	get_filename_component( srcdir "${CMAKE_SOURCE_DIR}" REALPATH )
	get_filename_component( bindir "${CMAKE_BINARY_DIR}" REALPATH )

	if( ${srcdir} STREQUAL ${bindir} )
		file( REMOVE ${srcdir}/cmake_install.cmake ) # removing other files won't work
		file( TO_NATIVE_PATH "${srcdir}/SDK*/" recommended )
		file( TO_NATIVE_PATH "${srcdir}/CMakeFiles/" cmakefiles )
		file( TO_NATIVE_PATH "${srcdir}/CMakeCache.txt" cmakecache )
		message( STATUS "In-Source builds are not allowed;
   CMake must be ran from dedicated build directory.

   Recommended
     ${recommended}

   Following directories and files MUST be deleted manually
   before running CMake again, from any location.
     ${cmakefiles}
     ${cmakecache}
" )
		message( FATAL_ERROR "In-Source build" )
	endif()

endfunction()
