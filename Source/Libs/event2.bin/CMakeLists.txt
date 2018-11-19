add_library( event2 INTERFACE )

if( UNIX )
	target_compile_definitions( event2 INTERFACE USE_LIBEVENT LIBEVENT_TIMEOUTS_WORKAROUND )
	target_include_directories( event2 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
	target_link_libraries( event2
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/libevent_core${CMAKE_STATIC_LIBRARY_SUFFIX}
			${CMAKE_CURRENT_SOURCE_DIR}/libevent_pthreads${CMAKE_STATIC_LIBRARY_SUFFIX}
	)
endif()
