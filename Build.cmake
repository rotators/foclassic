#
# Automated build
# cmake -P Build.cmake
#
# Options
# cmake [options] -P Build.cmake
#
# -DNO_FORMAT=1
# skips source formatting
#
# -DNO_BUILD=1
# skips source building
#
# -DNO_ZIP=1
# skips creating of release package
#
cmake_minimum_required( VERSION 3.13.4 FATAL_ERROR )

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake" )
include(AutomatedBuild)
include(FOClassicVersion)
include(FormatSource)

set( MAKE_FILE     "Makefile" )
set( SOLUTION_FILE "FOClassic.sln" )

GetProjectVersion()
FOClassicVersion()
DetectCI()

# handles NO_FORMAT
PrepareFiles()

if( NOT NO_BUILD )
	if( NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/.git/modules" )
		find_package( Git QUIET )
		if( Git_FOUND )
			message( STATUS "Initializing git submodules" )
			execute_process( COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive )
		endif()
	endif()

	CleanSourceDirectory()

	if( CI )
		CreateBuildDirectory( "SDK"             "${CI_GENERATOR}"       "${CI_TOOL}" "${CI_FILE}" )
	elseif( UNIX )
		CreateBuildDirectory( "SDK"             "Unix Makefiles"        "Linux32"    "${MAKE_FILE}" )
	elseif( WIN32 )
		CreateBuildDirectory( "SDK.VS2017.v100" "Visual Studio 15 2017" "v100"       "${SOLUTION_FILE}" )
		CreateBuildDirectory( "SDK.VS2017"      "Visual Studio 15 2017" ""           "${SOLUTION_FILE}" )
	endif()

	if( BUILD_FAIL )
		message( FATAL_ERROR "Build error" )
	endif()

	RunAllBuilds()
endif()

if( NOT NO_ZIP )
	ZipAllBuilds()
endif()

if( CI AND BUILD_FAIL )
	message( FATAL_ERROR "Build error" )
endif()
