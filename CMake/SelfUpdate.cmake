cmake_minimum_required( VERSION 3.0.0 )

message( STATUS "Checking system information..." )

execute_process(
	COMMAND ${CMAKE_COMMAND} --system-information
	OUTPUT_VARIABLE system_information
)
message( ${system_information} )
if( "${system_information}" MATCHES "CMAKE_HOST_SYSTEM_PROCESSOR \"([A-Za-z0-9_]+)\"" )
	message( STATUS "Processor: '${CMAKE_MATCH_1}'" )
else()
	#message( ${system_information} )
	message( STATUS )
	message( FATAL_ERROR "Cannot find system architecture" )
endif()
