#
# Extension example
#

foreach( target Client Mapper Server )

	set( extension Extension${target} )

	# create target define
	set( define "__${target}" )
	string( TOUPPER ${define} define )

	add_library( ${extension} SHARED "" )

	target_sources( ${extension}
		PRIVATE
			CMakeLists.txt

			${RELEASE_HEADERS_DIR}/foclassic.h
			${RELEASE_HEADERS_DIR}/foclassic.fos
			${FO_DIR}/PUBLIC.Header.h

			_defines.fos
			extension.cpp
	)

	target_compile_definitions( ${extension}
		PRIVATE
			# required
			FOCLASSIC_EXTENSION
			${define}

			# optional
			FOCLASSIC_BLEEDING_EDGE
	)

	target_include_directories( ${extension} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RELEASE_HEADERS_DIR} )

endforeach()

source_group( " "       REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "Scripts" REGULAR_EXPRESSION "\.fos$" )
source_group( "Engine"  REGULAR_EXPRESSION "foclassic\.(h|fos)$" )
source_group( "CMake"   REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )
