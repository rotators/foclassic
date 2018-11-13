# extract version from CMakeLists
# requires specific usage of project() command
function( GetProjectVersion )

	set( cmakelists "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt" )
	file( TO_NATIVE_PATH "${cmakelists}" cmakelists_native )

	if( NOT EXISTS "${cmakelists}" )
		message( FATAL_ERROR "${cmakelists_native} not found" )
	endif()

	file( READ "${cmakelists}" content )

	if( "${content}" MATCHES "FOClassic[\\t\\ ]+VERSION[\\t\\ ]+([0-9]+)\\.([0-9]+)" )
		set( FOCLASSIC_STAGE ${CMAKE_MATCH_1} PARENT_SCOPE )
		set( FOCLASSIC_VERSION ${CMAKE_MATCH_2} PARENT_SCOPE )
	else()
		message( FATAL_ERROR "FOClassic version information not found in ${cmakelists_native}" )
	endif()

endfunction()

# CI detection
function( DetectCI )

	if( DEFINED ENV{CI} )
		# TODO how to find x32/x64 without generator step?
		#if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
		#	set( CI_ARCH 32 )
		#elseif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		#	set( CI_ARCH 64 )
		#else()
		#	message( FATAL_ERROR "Unknown architecture (CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P})" )
		#endif()

		if( DEFINED ENV{APPVEYOR_BUILD_WORKER_IMAGE} )
			set( CI "AppVeyor" PARENT_SCOPE )
			set( CI_FILE "${SOLUTION_FILE}" PARENT_SCOPE )

			if( "$ENV{APPVEYOR_BUILD_WORKER_IMAGE}" STREQUAL "Visual Studio 2013" )
				set( CI_GENERATOR "Visual Studio 10 2010" PARENT_SCOPE )
			elseif( "$ENV{APPVEYOR_BUILD_WORKER_IMAGE}" STREQUAL "Visual Studio 2017" )
				set( CI_ZIP_SUFFIX "-vs2017" PARENT_SCOPE )
				set( CI_GENERATOR "Visual Studio 15 2017" PARENT_SCOPE )
			else()
				message( FATAL_ERROR "Unknown AppVeyor image ('$ENV{APPVEYOR_BUILD_WORKER_IMAGE}')" )
			endif()
		else()
			message( FATAL_ERROR "Unknown CI" )
		endif()
	endif()

	if( CI )
		message( STATUS "CI: ${CI} (CMake v${CMAKE_VERSION})" )
		message( STATUS )
	endif()

endfunction()

# Prepare repository files
# - format source
# - restore last modification time (CI only)
function( PrepareFiles )

	# get list of repository files
	execute_process(
		COMMAND git ls-files
		OUTPUT_VARIABLE ls
	)
	string( REGEX REPLACE "\n" ";" ls "${ls}" )

	foreach( file IN LISTS ls )
		file( TO_CMAKE_PATH "${file}" file )

		# directories which shouldnt't be processed
		if( "${file}" MATCHES "^[\"]?Legacy" OR "${file}" MATCHES "^Source/Libs" OR "${file}" STREQUAL "" )
			continue()
		endif()

		get_filename_component( ext "${file}" EXT )
		string( REGEX REPLACE "\\." "" ext "${ext}" )
		if( ext STREQUAL "" )
			continue()
		endif()

		set( mtime FALSE )
		set( format FALSE )

		# mark source files for formatting
		# all files must processed
		if( "${ext}" MATCHES "^[Hh]$" OR "${ext}" MATCHES "^[Cc][Pp][Pp]$" OR "${ext}" MATCHES "^[Ff][Oo][Ss]$" )
			if( NOT NO_FORMAT )
				set( format TRUE )
			endif()
		endif()
		# unmark non-addon angelscript sources
		if( "${file}" MATCHES "^Source/AngelScript" AND NOT "${file}" MATCHES "^Source/AngelScript/script" )
			set( format FALSE)
		endif()

		# restore modification time when running on CI
		# required for released files which contain timestamp
		if( CI )
			set( mtime TRUE )
		endif()

		if( mtime OR format )
			file( TO_NATIVE_PATH "${file}" file_native )
			message( STATUS "Processing ${file_native}" )
		endif()

		if( format )
			FormatSource( "${file}" "${CMAKE_CURRENT_LIST_DIR}" file_changed )
			if( file_changed )
				message( STATUS "           FormatSource prevails" )
			endif()
		endif()

		if( mtime )
			execute_process(
				COMMAND git log --pretty=format:%ct -1 "HEAD" -- "${file}"
				OUTPUT_VARIABLE timestamp
			)
			execute_process( COMMAND touch -d @${timestamp} "${file}" )
		endif()
	endforeach()

endfunction()

# Prepare build directory
function( CreateBuildDirectory dir generator toolset file )

	# use full path
	file( TO_CMAKE_PATH "${CMAKE_CURRENT_LIST_DIR}/${dir}" dir )
	file( TO_NATIVE_PATH "${dir}" dir_native )

	message( STATUS "Checking ${dir_native}" )
	if( NOT EXISTS "${dir}" )
		message( STATUS "Creating ${dir_native}" )
		file( MAKE_DIRECTORY "${dir}" )
	endif()

	if( NOT EXISTS "${dir}/${file}" )
		if( toolset )
			set( info ", toolset: ${toolset}" )
			set( toolset -T ${toolset} )
		endif()
		message( STATUS "Starting generator (${generator}${info})" )
		execute_process(
			COMMAND ${CMAKE_COMMAND} -G "${generator}" ${toolset} "${CMAKE_CURRENT_LIST_DIR}"
			WORKING_DIRECTORY "${dir}"
		)
	endif()

	if( EXISTS "${dir}/${file}" )
		list( APPEND BUILD_DIRS "${dir}" )
		set( BUILD_DIRS "${BUILD_DIRS}" PARENT_SCOPE )
	endif()

endfunction()

function( RunAllBuilds )

	foreach( dir IN LISTS BUILD_DIRS )
		file( TO_NATIVE_PATH "${dir}" dir_native )
		message( STATUS "Starting build (${dir_native})" )
		execute_process(
			COMMAND ${CMAKE_COMMAND} --build "${dir}" --config Release
			RESULT_VARIABLE result
		)
		if( NOT result EQUAL 0 )
			list( APPEND BUILD_FAIL "${dir}" )
			set( BUILD_FAIL "${BUILD_FAIL}" PARENT_SCOPE )
		endif()
	endforeach()

endfunction()

function( ZipAllBuilds )

	if( NOT FOCLASSIC_VERSION )
		GetProjectInfo()
	endif()

	# synch mtime inside packages
	string( TIMESTAMP ts "%Y-%m-%d %H:%M:%S" )

	foreach( dir IN LISTS BUILD_DIRS )
		set( root "FOClassic-v${FOCLASSIC_VERSION}" )
		set( sum  "sha256" )

		set( zip_dir   "${dir}/${root}" )
		set( zip_file  "${zip_dir}${CI_ZIP_SUFFIX}.zip" )
		set( inner_sum "${zip_dir}/CHECKSUM.${sum}" )
		set( outer_sum "${zip_dir}${CI_ZIP_SUFFIX}.${sum}" )

		file( TO_NATIVE_PATH "${zip_file}" zip_file_native )

		if( dir IN_LIST BUILD_FAIL )
			message( STATUS "Skipped ${zip_file_native} (build error)" )
			continue()
		endif()

		if( NOT EXISTS "${zip_dir}" )
			message( STATUS "Skipped ${zip_file_native} (release directory not found)" )
			continue()
		endif()

		message( STATUS "Creating ${zip_file_native}" )

		file( REMOVE "${zip_file}" )
		file( REMOVE "${inner_sum}" )
		file( REMOVE "${outer_sum}" )

		# it could be way simpler, but we want checksum to list files in very specific order
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

		# inner sum is a checksum of files inside package
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E ${sum}sum ${files}
			WORKING_DIRECTORY ${zip_dir}
			OUTPUT_FILE ${inner_sum}
		)

		# include inner checksum in release files list
		list( APPEND files "${inner_sum}" )

		# cmake is used for preparing release package
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E tar cfv ${zip_file} --format=zip --mtime=${ts} ${files}
			WORKING_DIRECTORY ${zip_dir}
			OUTPUT_QUIET
		)
		file( REMOVE "${inner_sum}" )

		# outer sum is a checksum of release package
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E ${sum}sum ${root}${CI_ZIP_SUFFIX}.zip
			WORKING_DIRECTORY ${dir}
			OUTPUT_FILE ${outer_sum}
		)

		if( CI )
			# copy package and outer sum to main dir
			# simplifies stripping build dir from artifacts path
			file( TO_NATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}" dir )
			message( STATUS "Moving (pre)release package to main directory (${dir})" )
			file( RENAME "${outer_sum}" "${CMAKE_CURRENT_LIST_DIR}/${root}${CI_ZIP_SUFFIX}.${sum}" )
			file( RENAME "${zip_file}"  "${CMAKE_CURRENT_LIST_DIR}/${root}${CI_ZIP_SUFFIX}.zip" )
		endif()
	endforeach()

endfunction()
