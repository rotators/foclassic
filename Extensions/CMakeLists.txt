#
# Extension
#
set( EXTENSION_SOURCES
	extension.cpp
	${CMAKE_SOURCE_DIR}/Source/PUBLIC.Defines.h
	${CMAKE_SOURCE_DIR}/Source/PUBLIC.Header.h
)

foreach( target Client Mapper Server )
	set( lib_old Extension${target} )
	set( lib_new ExtensionBleedingEdge${target} )
	set( target "__${target}" )
	string( TOUPPER ${target} target )

	add_library( ${lib_old} SHARED ${EXTENSION_SOURCES} )
	target_compile_definitions( ${lib_old} PRIVATE ${target} FOCLASSIC_EXTENSION )

	add_library( ${lib_new} SHARED ${EXTENSION_SOURCES} )
	target_compile_definitions( ${lib_new} PRIVATE ${target} FOCLASSIC_EXTENSION FOCLASSIC_BLEEDING_EDGE )
endforeach()
