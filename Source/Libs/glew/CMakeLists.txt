add_library( glew "" )
target_sources( glew
	PRIVATE
		glew.c
		glew.h
		glxew.h
		wglew.h
)
target_include_directories( glew PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
