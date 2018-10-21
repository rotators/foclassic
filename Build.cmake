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
cmake_minimum_required( VERSION 3.12.2 FATAL_ERROR )

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake" )
include(BuildFunctions)
include(FOClassicVersion)

GetProjectVersion()
FOClassicVersion()

if( WIN32 )
	foreach( compiler VS2010 VS2017.v100 VS2017 )
		CreateBuildDirectory( ${compiler} FOClassic.sln )
	endforeach()
endif()

if( NOT NO_FORMAT )
	FormatSource()
endif()

if( NOT NO_BUILD )
	RunAllBuilds()
endif()

if( NOT NO_ZIP )
	ZipAllBuilds()
endif()
