add_library( ogg INTERFACE )
target_include_directories( ogg INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
if( UNIX )
	target_link_libraries( ogg
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/libvorbisenc${CMAKE_STATIC_LIBRARY_SUFFIX}
	)
endif()

target_link_libraries( ogg
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/libvorbisfile_static${CMAKE_STATIC_LIBRARY_SUFFIX}
		${CMAKE_CURRENT_SOURCE_DIR}/libvorbis_static${CMAKE_STATIC_LIBRARY_SUFFIX}
		${CMAKE_CURRENT_SOURCE_DIR}/libogg_static${CMAKE_STATIC_LIBRARY_SUFFIX}
)
