##
## AngelScript
## TODO (VS2017) ac_callfunc* not handled correctly (LNK4221)
##

function( EnumToList file enum var )

	if( NOT EXISTS "${file}" )
		message( FATAL_ERROR "${file} does not exists" )
	endif()

	set( found 0 )
	file( STRINGS "${file}" content )
	foreach( str IN LISTS content )
		if( found EQUAL 0 )
			if( "${str}" MATCHES "^[\t\ ]*enum[\t\ ]+${enum}" )
				set( found 1 )
			endif()
		elseif( found EQUAL 1 )
			set( found 2 )
		else()
			if( str MATCHES "^[\t\ ]+([A-Za-z0-9_]+)[\t\ ]*=[\t\ ]*([0-9\-]+)")
				list( APPEND ${var} "${CMAKE_MATCH_1}=${CMAKE_MATCH_2}" )
			else()
				break()
			endif()
		endif()
	endforeach()

	if( ${var} )
		set( ${var} ${${var}} PARENT_SCOPE )
	endif()

endfunction()

add_library( angelscript STATIC "" )
target_sources( angelscript
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/angelscript.h
	PRIVATE
		${CMAKE_CURRENT_LIST_FILE}
		as_array.h
		as_atomic.cpp
		as_atomic.h
		as_builder.cpp
		as_builder.h
		as_bytecode.cpp
		as_bytecode.h
		as_callfunc.cpp
		as_callfunc.h
		as_callfunc_x64_gcc.cpp
		as_callfunc_x64_msvc.cpp
		as_callfunc_x86.cpp
		as_compiler.cpp
		as_compiler.h
		as_config.h
		as_configgroup.cpp
		as_configgroup.h
		as_context.cpp
		as_context.h
		as_criticalsection.h
		as_datatype.cpp
		as_datatype.h
		as_debug.h
		as_gc.cpp
		as_gc.h
		as_generic.cpp
		as_generic.h
		as_globalproperty.cpp
		as_map.h
		as_memory.cpp
		as_memory.h
		as_module.cpp
		as_module.h
		as_objecttype.cpp
		as_objecttype.h
		as_outputbuffer.cpp
		as_outputbuffer.h
		as_parser.cpp
		as_parser.h
		as_property.h
		as_restore.cpp
		as_restore.h
		as_scriptcode.cpp
		as_scriptcode.h
		as_scriptengine.cpp
		as_scriptengine.h
		as_scriptfunction.cpp
		as_scriptfunction.h
		as_scriptnode.cpp
		as_scriptnode.h
		as_scriptobject.cpp
		as_scriptobject.h
		as_string.cpp
		as_string.h
		as_string_util.cpp
		as_string_util.h
		as_symboltable.h
		as_texts.h
		as_thread.cpp
		as_thread.h
		as_tokendef.h
		as_tokenizer.cpp
		as_tokenizer.h
		as_typeinfo.cpp
		as_typeinfo.h
		as_variablescope.cpp
		as_variablescope.h
)

# from AngelScript setup
if( MSVC AND CMAKE_CL_64 )
	enable_language( ASM_MASM )
	if( CMAKE_ASM_MASM_COMPILER_WORKS )
		target_sources( angelscript PRIVATE as_callfunc_x64_msvc_asm.asm )
	else()
		message( FATAL_ERROR "MSVC x86_64 target requires a working assembler" )
	endif()
endif()

target_compile_definitions( angelscript PUBLIC AS_NO_THREADS )
target_include_directories( angelscript PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries( angelscript )
set_property( TARGET angelscript PROPERTY FOLDER "${FOCLASSIC_FOLDER_PREFIX}/Libs" )

source_group( " "     REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "CMake" REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )

if( EXISTS "${CMAKE_CURRENT_LIST_DIR}/Tests" )
	add_subdirectory( Tests )
endif()

EnumToList( "${CMAKE_CURRENT_LIST_DIR}/angelscript.h" asERetCodes ASERETCODES )
set( prefix "" )
foreach( retcode IN LISTS ASERETCODES )
	if( retcode MATCHES "(.*)=(.*)" )
		math( EXPR AS_RETURN_CODES_LENGTH "${AS_RETURN_CODES_LENGTH} + 1" )
		string( APPEND AS_RETURN_CODES "${prefix}\"${CMAKE_MATCH_1}\", // ${CMAKE_MATCH_2}\n" )
		if( "${prefix}" STREQUAL "" )
			set( prefix "    ")
		endif()
	endif()
endforeach()

if( "${AS_RETURN_CODES}" MATCHES "(.*), // ([0-9\-]+)\n$")
	set( AS_RETURN_CODES "${CMAKE_MATCH_1}  // ${CMAKE_MATCH_2}" )
endif()

configure_file( "${ENGINE_ANGELSCRIPT_HEADER_TEMPLATE}" "${ENGINE_ANGELSCRIPT_HEADER_FILE}" @ONLY NEWLINE_STYLE LF )
