#
# Extension
#

foreach( name Client Mapper Server )
	set( lib Extension${name} )
	# create required define
	set( name "__${name}" )
	string( TOUPPER ${name} name )

	add_library( ${lib} SHARED "" )
	target_sources( ${lib}
		PRIVATE
			extension.cpp
			${CMAKE_SOURCE_DIR}/Source/Shared/PUBLIC.Defines.h
			${CMAKE_SOURCE_DIR}/Source/PUBLIC.Header.h
			_defines.fos
			foclassic.fos
	)
	target_compile_definitions( ${lib} PRIVATE ${name} FOCLASSIC_EXTENSION FOCLASSIC_BLEEDING_EDGE )
endforeach()

source_group( " "        REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "Wrappers" REGULAR_EXPRESSION "\.[Ff][Oo][Ss]$" )
source_group( "Misc"     REGULAR_EXPRESSION "CMakeLists" )
