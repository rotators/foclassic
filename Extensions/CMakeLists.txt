# directories
set( RELEASE_EXTENSIONS_DIR         "${RELEASE_DIR}/Extensions" )
set( RELEASE_EXTENSIONS_EXAMPLE_DIR "${RELEASE_EXTENSIONS_DIR}/Example" )

# special files
set( EXTENSIONS_HEADER_FILE     "${CMAKE_CURRENT_LIST_DIR}/FOClassic.Template.h" )
set( EXTENSIONS_CMAKELISTS_FILE "${CMAKE_CURRENT_LIST_DIR}/Example/CMakeLists.txt" )

# timestamps
file( TIMESTAMP "${EXTENSIONS_HEADER_FILE}"     EXTENSIONS_HEADER_TIMESTAMP     "%d.%m.%Y" )
file( TIMESTAMP "${EXTENSIONS_CMAKELISTS_FILE}" EXTENSIONS_CMAKELISTS_TIMESTAMP	"%d.%m.%Y" )

# extensions subdirectories
set( EXTENSIONS_SUBDIRS Example )

##
## prepare extensions for release
##

# create extensions header
configure_file( "${EXTENSIONS_HEADER_FILE}" "${RELEASE_HEADERS_DIR}/FOClassic.h" @ONLY NEWLINE_STYLE LF )

# create ExampleExtensions files
foreach( example_file IN ITEMS extension.cpp _defines.fos )
	configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/Example/${example_file}" "${RELEASE_EXTENSIONS_EXAMPLE_DIR}/${example_file}" @ONLY NEWLINE_STYLE LF )
endforeach()
configure_file( "${EXTENSIONS_CMAKELISTS_FILE}" "${RELEASE_EXTENSIONS_EXAMPLE_DIR}/CMakeLists.txt" @ONLY NEWLINE_STYLE LF )

##
## process subdirectories
##

foreach( dir IN LISTS EXTENSIONS_SUBDIRS )
	add_subdirectory( ${dir} )
endforeach()


foreach( dir IN LISTS EXTENSIONS_SUBDIRS )
	# get all targets created in subdirectories
	get_property( targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS )

	foreach( target IN LISTS targets )

		##
		## prettify IDE
		##
		set_property( TARGET ${target} PROPERTY FOLDER "Extensions" )

	endforeach()
endforeach()
