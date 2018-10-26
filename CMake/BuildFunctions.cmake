function( GetProjectVersion )

	set( cmakelists "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt" )

	if( NOT EXISTS "${cmakelists}" )
		message( FATAL_ERROR "${cmakelists} not found" )
	endif()

	file( READ "${cmakelists}" content )

	if( "${content}" MATCHES "FOClassic[\\t\\ ]+VERSION[\\t\\ ]+([0-9]+)\\.([0-9]+)" )
		set( FOCLASSIC_STAGE ${CMAKE_MATCH_1} PARENT_SCOPE )
		set( FOCLASSIC_VERSION ${CMAKE_MATCH_2} PARENT_SCOPE )
	else()
		message( FATAL_ERROR "FOClassic version information not found in ${cmakelists}" )
	endif()

endfunction()

# Prepare build 
function( CreateBuildDirectory compiler file )

	if( ${compiler} STREQUAL "VS2010" )
		set( dir SDK.VS2010 )
		set( generator "Visual Studio 10 2010" )
	elseif( ${compiler} STREQUAL "VS2017.v100" )
		set( dir SDK.VS2017.v100 )
		set( generator "Visual Studio 15 2017" )
		set( extras "-Tv100" )
	elseif( ${compiler} STREQUAL "VS2017" )
		set( dir SDK.VS2017 )
		set( generator "Visual Studio 15 2017" )
	else()
		message( FATAL_ERROR "Unknown compiler '${compiler}'" )
	endif()

	# use full path
	set( dir "${CMAKE_CURRENT_LIST_DIR}/${dir}" )

	message( STATUS "Checking ${dir}" )
	if( NOT EXISTS "${dir}" )
		message( STATUS "Creating ${dir}" )
		file( MAKE_DIRECTORY "${dir}" )
	endif()

	if( NOT EXISTS "${dir}/${file}" )
		message( STATUS "Starting generator (${generator})" )
		execute_process(
			COMMAND ${CMAKE_COMMAND} -G "${generator}" ${extras} ${CMAKE_CURRENT_LIST_DIR}
			WORKING_DIRECTORY ${dir}
		)
	endif()

	if( EXISTS ${dir}/${file} )
		list( APPEND BUILD_DIRS "${dir}" )
		set( BUILD_DIRS "${BUILD_DIRS}" PARENT_SCOPE )
	endif()

endfunction()

function( RunAllBuilds )

	foreach( dir ${BUILD_DIRS} )
		message( STATUS "Starting build (${dir})" )
		execute_process( COMMAND ${CMAKE_COMMAND} --build ${dir} --config Release )
	endforeach()

endfunction()

function( ZipAllBuilds )

	if( NOT FOCLASSIC_VERSION )
		GetProjectInfo()
	endif()

	string( TIMESTAMP ts "%Y-%m-%d %H:%M:%S" )

	foreach( dir ${BUILD_DIRS} )
		set( root "FOClassic-v${FOCLASSIC_VERSION}" )
		set( sum  "sha256" )

		set( zip_dir   "${dir}/${root}" )
		set( zip_file  "${zip_dir}.zip" )
		set( inner_sum "${zip_dir}/CHECKSUM.${sum}" )
		set( outer_sum "${zip_dir}.${sum}" )

		if( NOT EXISTS "${zip_dir}" )
			message( STATUS "Skipped ${zip_file}" )
			continue()
		endif()

		message( STATUS "Creating ${zip_file}" )

		file( REMOVE "${zip_file}" )
		file( REMOVE "${inner_sum}" )
		file( REMOVE "${outer_sum}" )

		file( GLOB         core       LIST_DIRECTORIES false RELATIVE ${zip_dir} ${zip_dir}/* )
		file( GLOB         core_md    LIST_DIRECTORIES false RELATIVE ${zip_dir} ${zip_dir}/*.md )
		file( GLOB_RECURSE cmake      LIST_DIRECTORIES false RELATIVE ${zip_dir} ${zip_dir}/CMake/* )
		file( GLOB_RECURSE headers    LIST_DIRECTORIES false RELATIVE ${zip_dir} ${zip_dir}/Headers/* )
		file( GLOB_RECURSE extensions LIST_DIRECTORIES false RELATIVE ${zip_dir} ${zip_dir}/Extensions/* )
		file( GLOB_RECURSE tools      LIST_DIRECTORIES false RELATIVE ${zip_dir} ${zip_dir}/Tools/* )

		list( APPEND core_md "VERSION" )
		foreach( name IN LISTS core_md )
			if( EXISTS "${zip_dir}/${name}" )
				list( REMOVE_ITEM core "${name}" )
			endif()
		endforeach()

		set( files ${core} ${tools} ${cmake} ${headers} ${extensions} ${core_md} )

		execute_process(
			COMMAND ${CMAKE_COMMAND} -E ${sum}sum ${files}
			WORKING_DIRECTORY ${zip_dir}
			OUTPUT_FILE ${inner_sum}
		)

		set( files "${files};CHECKSUM.${sum}" )

		execute_process(
			COMMAND ${CMAKE_COMMAND} -E tar cfv ${zip_file} --format=zip --mtime=${ts} ${files}
			WORKING_DIRECTORY ${zip_dir}
			OUTPUT_QUIET
		)

		execute_process(
			COMMAND ${CMAKE_COMMAND} -E ${sum}sum ${root}.zip
			WORKING_DIRECTORY ${dir}
			OUTPUT_FILE ${outer_sum}
		)

		file( REMOVE "${inner_sum}" )
	endforeach()

endfunction()
