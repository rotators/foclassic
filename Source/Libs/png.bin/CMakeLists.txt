add_library( png-bin INTERFACE )
target_include_directories( png-bin INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

if( UNIX )
	target_link_libraries( png-bin
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/libpng12${CMAKE_STATIC_LIBRARY_SUFFIX}
	)
elseif( WIN32 )
	target_link_libraries( png-bin
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/libpng15${CMAKE_STATIC_LIBRARY_SUFFIX}
	)
endif()
