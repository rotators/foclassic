add_library( jpeg INTERFACE )

if( WIN32 )
	target_link_libraries( jpeg
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/jpeg.lib
	)
endif()
