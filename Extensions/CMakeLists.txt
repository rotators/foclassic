#
# Extension
#
set( EXTENSION_SOURCES
)

foreach( target Client Mapper Server )
	set( lib Extension${target} )
	set( target "__${target}" )
	string( TOUPPER ${target} target )

	add_library( ${lib} SHARED "" )
	target_sources( ${lib}
		PRIVATE
			extension.cpp
			${CMAKE_SOURCE_DIR}/Source/Shared/PUBLIC.Defines.h
			${CMAKE_SOURCE_DIR}/Source/PUBLIC.Header.h
			_defines.fos
			foclassic.fos
	)
	target_compile_definitions( ${lib} PRIVATE ${target} FOCLASSIC_EXTENSION FOCLASSIC_BLEEDING_EDGE )
endforeach()

source_group( " "        REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "Wrappers" REGULAR_EXPRESSION "\.[Ff][Oo][Ss]$" )
