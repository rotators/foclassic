##
## configuration
##

# directories
set( RELEASE_EXTENSIONS_DIR "${RELEASE_DIR}/Extensions" )

# special files
set( EXTENSIONS_HEADER_FILE "${CMAKE_CURRENT_LIST_DIR}/FOClassic.h" )

# create cmake functions file
# this must be done before any extension configuration
file( TIMESTAMP "${ROOT_CMAKE_DIR}/FOClassic.cmake" EXTENSIONS_CONTENT_TIMESTAMP "%d.%m.%Y" )
configure_file( "${ROOT_CMAKE_DIR}/FOClassic.cmake" "${RELEASE_CMAKE_DIR}/FOClassic.cmake" @ONLY NEWLINE_STYLE LF )

# create extensions header
# this must be done before any extension configuration
file( TIMESTAMP "${EXTENSIONS_HEADER_FILE}" EXTENSIONS_CONTENT_TIMESTAMP "%d.%m.%Y" )
configure_file( "${EXTENSIONS_HEADER_FILE}" "${RELEASE_HEADERS_DIR}/FOClassic.h" @ONLY NEWLINE_STYLE LF )

# create angelscript headers
# this must be done before any extension configuration
foreach( header IN ITEMS angelscript scriptany scriptarray scriptdictionary scriptfile scriptmath scriptstring )
	configure_file( "${AS_DIR}/${header}.h" "${RELEASE_DIR}/Headers/AngelScript/${header}.h" @ONLY NEWLINE_STYLE LF )
endforeach()

##
## prepare extensions directories
##

# find CMakeLists.txt in subdirectories
file( GLOB found LIST_DIRECTORIES false RELATIVE "${CMAKE_CURRENT_LIST_DIR}" CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*/CMakeLists.txt" )
foreach( file IN LISTS found )
	# extract directory name
	string( REGEX REPLACE "^(.+)/.+$" "\\1" name "${file}" )

	# prepare all files in extension directory for release
	file( GLOB contents LIST_DIRECTORIES true RELATIVE "${CMAKE_CURRENT_LIST_DIR}/${name}" CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/${name}/*" )
	foreach( content IN LISTS contents )
		file( TIMESTAMP "${CMAKE_CURRENT_LIST_DIR}/${name}/${content}" EXTENSIONS_CONTENT_TIMESTAMP "%d.%m.%Y" )
		configure_file( "${CMAKE_CURRENT_LIST_DIR}/${name}/${content}" "${RELEASE_EXTENSIONS_DIR}/${name}/${content}" @ONLY NEWLINE_STYLE LF )

		# prepare sources list for Extensions target
		list( APPEND EXTENSIONS_FILES "${CMAKE_CURRENT_LIST_DIR}/${name}/${content}" )
	endforeach()

	# prepare subdirectory processing
	list( APPEND EXTENSIONS_SUBDIRS "${name}" )

	# prettify IDE
	source_group( "${name}" REGULAR_EXPRESSION "^${CMAKE_CURRENT_LIST_DIR}/${name}/" )
endforeach()

# Extensions target holds content of all discovered extensions and FOClassic.h template, and cmake-related files
add_custom_target( Extensions
	SOURCES
		${CMAKE_CURRENT_LIST_FILE}
		${ROOT_CMAKE_DIR}/FOClassic.cmake

		${EXTENSIONS_FILES}
		${EXTENSIONS_HEADER_FILE}
)
# prettify IDE
set_property( TARGET Extensions PROPERTY FOLDER Configuration )
source_group( "CMake" REGULAR_EXPRESSION "^${ROOT_CMAKE_DIR}/.+" )

##
## process subdirectories
##

foreach( dir IN LISTS EXTENSIONS_SUBDIRS )
	# cmake should be running in release directory
	add_subdirectory( "${RELEASE_EXTENSIONS_DIR}/${dir}" "${CMAKE_CURRENT_BINARY_DIR}/${dir}.extension" )

	# get all targets created in subdirectory
	get_property( targets DIRECTORY "${RELEASE_EXTENSIONS_DIR}/${dir}" PROPERTY BUILDSYSTEM_TARGETS )
	foreach( name IN LISTS targets )

		##
		## additional configuration
		##

		# make sure target is an extension
		get_property( extension TARGET ${name} PROPERTY FOCLASSIC_EXTENSION )
		if( NOT extension )
			continue()
		endif()

		##
		## each extension should depend on its target(s)
		##
		target_compile_definitions( ${name} PRIVATE FOCLASSIC_EXTENSION_NOTES )

		##
		## each extension should depend on its target(s)
		##

		get_property( target TARGET ${name} PROPERTY FOCLASSIC_EXTENSION_TARGET )
		string( SUBSTRING ${target} 0 1 letter )
		string( TOLOWER ${target} target )
		string( REGEX REPLACE "^.(.+)" "${letter}\\1" target "${target}" )
		if( "${target}" STREQUAL "Server" )
			add_dependencies( ${name} ${target} )
		else()
			add_dependencies( ${name} ${target}DX ${target}GL )
		endif()

		##
		## prettify IDE
		##

		set_property( TARGET ${name} PROPERTY FOLDER "Extensions" )

	endforeach()
endforeach()

# FOClassic.h template shouldn't be in 'Header files' group
source_group( " " FILES "${EXTENSIONS_HEADER_FILE}" )
