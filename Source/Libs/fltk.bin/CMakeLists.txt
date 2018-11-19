add_library( fltk INTERFACE )
add_library( fltkgl INTERFACE )

target_include_directories( fltk INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
target_include_directories( fltkgl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries( fltk
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/fltk${CMAKE_STATIC_LIBRARY_SUFFIX}
)
target_link_libraries( fltkgl
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/fltkgl${CMAKE_STATIC_LIBRARY_SUFFIX}
)
