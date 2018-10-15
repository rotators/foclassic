cmake_minimum_required( VERSION 3.12 )

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake" )
include(BuildFunctions)

GetProjectInfo()

message( STATUS "Building v${FOCLASSIC_VERSION}..." )

if( WIN32 )
	foreach( compiler VS2010 VS2017.v100 VS2017 )
		CreateBuildDirectory( ${compiler} FOClassic.sln )
	endforeach()
endif()

FormatSource()

RunAllBuilds()
ZipAllBuilds()
