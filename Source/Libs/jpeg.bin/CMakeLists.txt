add_library( jpeg INTERFACE )

target_link_libraries( jpeg
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/jpeg${CMAKE_STATIC_LIBRARY_SUFFIX}
)
