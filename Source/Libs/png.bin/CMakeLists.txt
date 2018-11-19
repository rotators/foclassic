add_library( png12-bin INTERFACE )

if( UNIX )
	target_include_directories( png12-bin INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
	target_link_libraries( png12-bin
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/libpng12${CMAKE_STATIC_LIBRARY_SUFFIX}
	)
endif()

add_library( png15-bin INTERFACE )
target_link_libraries( png15-bin
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/libpng15${CMAKE_STATIC_LIBRARY_SUFFIX}
)
target_include_directories( png15-bin INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
