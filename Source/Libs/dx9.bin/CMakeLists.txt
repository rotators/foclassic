#  needs changes when/if we support 64bit in future...

add_library( dx9 INTERFACE )
target_include_directories( dx9 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

if( WIN32 )
	target_link_libraries( dx9
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/x86/d3d9.lib
			${CMAKE_CURRENT_SOURCE_DIR}/x86/d3dx9.lib
			${CMAKE_CURRENT_SOURCE_DIR}/x86/d3dxof.lib
			${CMAKE_CURRENT_SOURCE_DIR}/x86/dxerr.lib
			${CMAKE_CURRENT_SOURCE_DIR}/x86/dxguid.lib
	)
endif()
