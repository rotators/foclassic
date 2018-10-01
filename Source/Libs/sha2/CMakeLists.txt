add_library( sha2 STATIC "" )
target_sources( sha2
	PRIVATE
		sha2.c
		sha2.h
)
target_include_directories( sha2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
