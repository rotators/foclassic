##
## DatFile
##

add_library( DatFile STATIC "" )
target_sources( DatFile
	PRIVATE
		cfile.cpp
		cfile.h
		unlzss.cpp
		unlzss.h
)
target_compile_definitions( DatFile PRIVATE FOCLASSIC_ENGINE )
target_include_directories( DatFile
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${FO_DIR}
	PRIVATE
		${CMAKE_BINARY_DIR}/include
)
target_link_libraries( DatFile Shared zlib )
