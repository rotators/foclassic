add_library( il INTERFACE )
target_compile_definitions( il INTERFACE IL_STATIC_LIB )
target_include_directories( il INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries( il
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/IL${CMAKE_STATIC_LIBRARY_SUFFIX}
)
if( UNIX )
	target_link_libraries( il
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/libmng${CMAKE_STATIC_LIBRARY_SUFFIX}
			${CMAKE_CURRENT_SOURCE_DIR}/liblcms${CMAKE_STATIC_LIBRARY_SUFFIX}
			${CMAKE_CURRENT_SOURCE_DIR}/libtiff${CMAKE_STATIC_LIBRARY_SUFFIX}
			${CMAKE_CURRENT_SOURCE_DIR}/libjbig${CMAKE_STATIC_LIBRARY_SUFFIX}
			${CMAKE_CURRENT_SOURCE_DIR}/liblzma${CMAKE_STATIC_LIBRARY_SUFFIX}
	)
endif()
