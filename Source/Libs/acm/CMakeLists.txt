add_library( acm STATIC "" )
target_sources( acm
	PRIVATE
		acmstrm.cpp
		acmstrm.h
)
target_include_directories( acm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
