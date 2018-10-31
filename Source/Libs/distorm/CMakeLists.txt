add_library( distorm INTERFACE )
target_include_directories( distorm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

if( WIN32 )
	target_link_libraries( distorm
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/distorm.lib
	)
endif()
