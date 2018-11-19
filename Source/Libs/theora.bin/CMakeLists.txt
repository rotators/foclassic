add_library( theora INTERFACE )
target_include_directories( theora INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries( theora
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/libtheora_static${CMAKE_STATIC_LIBRARY_SUFFIX}
)
