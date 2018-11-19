add_library( portaudio INTERFACE )
target_include_directories( portaudio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries( portaudio
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/portaudio${CMAKE_STATIC_LIBRARY_SUFFIX}
)
if( UNIX )
	target_link_libraries( portaudio
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/asound${CMAKE_STATIC_LIBRARY_SUFFIX}
			${CMAKE_CURRENT_SOURCE_DIR}/jack${CMAKE_STATIC_LIBRARY_SUFFIX}
	)
endif()
