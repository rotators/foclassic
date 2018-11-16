##
## AngelScript
## TODO (VS2017) ac_callfunc* not handled correctly (LNK4221)
##

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
		as_callfunc_x64_msvc.asm
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
target_compile_definitions( angelscript PUBLIC AS_NO_THREADS )
target_include_directories( angelscript PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries( angelscript )
set_property( TARGET angelscript PROPERTY FOLDER "Libs" )

source_group( " "     REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "CMake" REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )
