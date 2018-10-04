add_library( png INTERFACE )
target_include_directories( png INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

if( WIN32 )
	target_link_libraries( png
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/libpng15.lib
	)
endif()
