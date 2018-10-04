add_library( theora INTERFACE )
target_include_directories( theora INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

if( WIN32 )
	target_link_libraries( theora
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/libtheora_static.lib
	)
endif()
