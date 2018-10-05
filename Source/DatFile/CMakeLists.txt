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
if( WIN32 )
	target_compile_definitions( DatFile PRIVATE WINVER=0x0501 ) # Windows XP
	target_compile_definitions( DatFile PRIVATE WIN32_LEAN_AND_MEAN )
endif()
target_include_directories( DatFile
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${FO_DIR}
	PRIVATE
		${CMAKE_BINARY_DIR}/include
)

target_link_libraries( DatFile Shared zlib )
