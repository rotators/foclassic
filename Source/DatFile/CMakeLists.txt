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
		${CMAKE_CURRENT_LIST_DIR}
		${SOURCE_DIR}
	PRIVATE
		${FOCLASSIC_INCLUDE_DIR}
)

target_link_libraries( DatFile Shared zlib )
set_property( TARGET DatFile PROPERTY FOLDER "${FOCLASSIC_FOLDER_PREFIX}/Libs" )
