add_library( glew "" )
target_sources( glew
	PRIVATE
		GL/glew.c
		GL/glew.h
		GL/glxew.h
		GL/wglew.h
)
target_include_directories( glew PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" )
