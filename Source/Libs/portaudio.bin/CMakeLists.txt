add_library( portaudio INTERFACE )
target_include_directories( portaudio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

if( WIN32 )
	target_link_libraries( portaudio
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/portaudio.lib
	)
endif()
