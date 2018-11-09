add_library( il INTERFACE )
target_compile_definitions( il INTERFACE IL_STATIC_LIB )
target_include_directories( il INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

if( WIN32 )
	target_link_libraries( il
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/IL.lib
	)
endif()
