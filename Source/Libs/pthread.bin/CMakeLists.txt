add_library( pthread INTERFACE )
target_include_directories( pthread INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

if( WIN32 )
	target_compile_definitions( pthread INTERFACE PTW32_STATIC_LIB )
	target_link_libraries( pthread
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/pthreadVC2.lib
	)
endif()
