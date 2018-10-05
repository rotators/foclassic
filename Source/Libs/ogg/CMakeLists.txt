add_library( ogg INTERFACE )

if( WIN32 )
	target_link_libraries( ogg
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/libogg_static.lib
			${CMAKE_CURRENT_SOURCE_DIR}/libvorbis_static.lib
			${CMAKE_CURRENT_SOURCE_DIR}/libvorbisfile_static.lib
	)
endif()
