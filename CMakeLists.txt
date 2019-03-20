##
## FOClassic
## https://rotators.fodev.net/foclassic/
## https://github.com/rotators/foclassic/
##

# https://cmake.org/cmake/help/v3.13/
cmake_minimum_required( VERSION 3.13.4 FATAL_ERROR )

# special directories
set( FOCLASSIC_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/CMake" )
list( APPEND CMAKE_MODULE_PATH "${FOCLASSIC_CMAKE_DIR}" )

# disable in-source builds
include(DisableInSourceBuild)
DisableInSourceBuild()

# this file must contain unique line with version info
# "FOClassic VERSION [number:STAGE].[number:VERSION]"
project( FOClassic VERSION 3.7
	DESCRIPTION  "FOnline Community Edition (r412 base)"
	HOMEPAGE_URL "https://rotators.fodev.net/foclassic/"
	LANGUAGES C CXX
)

set( FOCLASSIC_STAGE   "${PROJECT_VERSION_MAJOR}" )
set( FOCLASSIC_VERSION "${PROJECT_VERSION_MINOR}" )

set( CMAKE_CONFIGURATION_TYPES    "Release" )
set( CMAKE_CXX_STANDARD           11 )
set( CMAKE_CXX_STANDARD_REQUIRED  YES )
set( CMAKE_SKIP_INSTALL_RULES     TRUE )
if( DEFINED ENV{VERBOSE} )
	set( CMAKE_VERBOSE_MAKEFILE   TRUE )
endif()

# standard includes
include( FOClassicVersion )
FOClassicVersion()
include( FormatSource )
include( PdbSetup )
include( RandomValues )
include( SourceOptions )

##
## set variables
##

set( FOCLASSIC_ROOT_DIR                  "${CMAKE_CURRENT_LIST_DIR}" )
set( FOCLASSIC_BINARY_DIR                "${CMAKE_CURRENT_BINARY_DIR}" )

# main directories
set( SOURCE_DIR                          "${FOCLASSIC_ROOT_DIR}/Source" )
set( EXTENSIONS_DIR                      "${FOCLASSIC_ROOT_DIR}/Extensions.Dynamic" )
set( DOCS_DIR                            "${FOCLASSIC_ROOT_DIR}/Docs" )

# libraries directories
set( SHARED_DIR                          "${SOURCE_DIR}/Shared" )
set( LIBS_DIR                            "${SOURCE_DIR}/Libs" )
set( AS_DIR                              "${SOURCE_DIR}/AngelScript" )
set( AS_ADDONS_DIR                       "${AS_DIR}/Addons" )
set( DAT_DIR                             "${SOURCE_DIR}/DatFile" )

# release directories
set( RELEASE_DIR_NAME                    "FOClassic-v${PROJECT_VERSION_MINOR}" )
set( RELEASE_DIR                         "${FOCLASSIC_BINARY_DIR}/${RELEASE_DIR_NAME}" )
set( RELEASE_CMAKE_DIR                   "${RELEASE_DIR}/CMake" )
set( RELEASE_HEADERS_DIR                 "${RELEASE_DIR}/Headers" )

# other directories
set( TEMPLATES_DIR                       "${FOCLASSIC_CMAKE_DIR}/Templates" )
set( FOCMAKE_DIR                         "${FOCLASSIC_BINARY_DIR}/CMake" )
set( FOCLASSIC_INCLUDE_DIR               "${FOCMAKE_DIR}/include" )

# templates
set( ENGINE_GLOBAL_HEADER_TEMPLATE       "${TEMPLATES_DIR}/CMake.h" )
set( ENGINE_ANGELSCRIPT_HEADER_TEMPLATE  "${TEMPLATES_DIR}/CMake.AngelScript.h" )
set( ENGINE_NET_HEADER_TEMPLATE          "${TEMPLATES_DIR}/CMake.Net.h" )
set( SCRIPTS_HEADER_TEMPLATE             "${SHARED_DIR}/Defines.Public.h" )
set( VERSION_TEMPLATE                    "${TEMPLATES_DIR}/VERSION" )

# special files
set( CHANGELOG_FILE                      "${DOCS_DIR}/changelog/CHANGELOG.md" )
set( ENGINE_GLOBAL_HEADER_FILE           "${FOCLASSIC_INCLUDE_DIR}/CMake.h" )
set( ENGINE_ANGELSCRIPT_HEADER_FILE      "${FOCLASSIC_INCLUDE_DIR}/CMake.AngelScript.h" )
set( ENGINE_NET_HEADER_FILE              "${FOCLASSIC_INCLUDE_DIR}/CMake.Net.h" )

##
## customization
##

set( USER_TOP_FOCLASSIC_FILE             "${CMAKE_SOURCE_DIR}/CMakeFOClassic.txt" )
set( USER_FOCLASSIC_FILE                 "${FOCLASSIC_ROOT_DIR}/CMakeFOClassic.txt" )

message( STATUS "Customization setup..." )

if( "${USER_FOCLASSIC_FILE}" STREQUAL "${USER_TOP_FOCLASSIC_FILE}" )
	# standalone build
	if( EXISTS "${USER_FOCLASSIC_FILE}" )
		file( TO_NATIVE_PATH "${custom_file}" custom_file_native )
		message( STATUS "  ${custom_file_native}" )
		include( "${USER_FOCLASSIC_FILE}" OPTIONAL )
	endif()
else()
	# add_subdirectory() build
	set( FOCLASSIC_SUBPROJECT_BUILD TRUE )

	if( NOT FOCLASSIC_FOLDER_PREFIX )
		set( FOCLASSIC_FOLDER_PREFIX "FOClassic" )
	endif()

	foreach( custom_file IN ITEMS "${USER_TOP_FOCLASSIC_FILE}" "${USER_FOCLASSIC_FILE}" )
		if( EXISTS "${custom_file}" )
			file( TO_NATIVE_PATH "${custom_file}" custom_file_native )
			message( STATUS "  ${custom_file_native}" )
			include( "${custom_file}" )
		endif()
	endforeach()
endif()

if( FOCLASSIC_HASH_STRING )
	HashText( "${FOCLASSIC_HASH_STRING}" HASH_USER_STRING )
	list( APPEND HASH_LIST "${HASH_USER_STRING}" )
endif()

if( FOCLASSIC_HASH_FILES )
	foreach( user_file IN LISTS FOCLASSIC_HASH_FILES )
		unset( HASH_USER_FILE )
		file( TO_CMAKE_PATH "${user_file}" user_file_path )
		file( TO_NATIVE_PATH "${user_file}" user_file_native )
		if( EXISTS "${user_file_path}" )
			HashFile( "${user_file_path}" HASH_USER_FILE )
			list( APPEND HASH_LIST "${HASH_USER_FILE}" )
			list( APPEND CMAKE_DEPENDS "${user_file_native}" )
		endif()
	endforeach()
endif()

if( NOT HASH_LIST )
	message( STATUS "Using default global seed" )
endif()

message( STATUS "Customization setup... OK" )
message( STATUS "" )

# timestamps
file( TIMESTAMP "${SCRIPTS_HEADER_TEMPLATE}" SCRIPTS_HEADER_TIMESTAMP "%d.%m.%Y" )

##
## TODO user-specific configuration
##
function( SourceOption_LEGACY_SAVEFILE )
	target_compile_definitions( Server PRIVATE OPTION_LEGACY_SAVEFILE)
endfunction()
SourceOption( LEGACY_SAVEFILE "Allow loading legacy savefiles" ON )

# TODO move to subdirectories
if( MSVC )
	set( COMPILER_DEFINES "" )
	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_CRT_SECURE_NO_WARNINGS" )
#	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_CRT_SECURE_NO_DEPRECATE" )
#	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_WINSOCK_DEPRECATED_NO_WARNINGS" )

	set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILER_DEFINES} /MT /MP" )
endif()

include( CheckStructHasMember )
check_struct_has_member( "struct timespec" "tv_sec" "time.h" HAVE_STRUCT_TIMESPEC )
# HAVE_STRUCT_TIMESPEC must be defined for windows pthreads on newer MSVC
if( HAVE_STRUCT_TIMESPEC )
	add_definitions( -DHAVE_STRUCT_TIMESPEC=1 )
endif( HAVE_STRUCT_TIMESPEC )

# create engine header
if( NOT FOCLASSIC_VERSION_CUSTOM )
	set( FOCLASSIC_VERSION_CUSTOM 0 )
endif()
configure_file( "${ENGINE_GLOBAL_HEADER_TEMPLATE}" "${ENGINE_GLOBAL_HEADER_FILE}" @ONLY NEWLINE_STYLE LF )

# release changelog
configure_file( "${CHANGELOG_FILE}" "${RELEASE_DIR}/CHANGELOG.md" @ONLY NEWLINE_STYLE LF )

# release version file
configure_file( "${VERSION_TEMPLATE}" "${RELEASE_DIR}/VERSION" @ONLY NEWLINE_STYLE LF )

# release scripts header
# this must be done before any extension configuration
configure_file( "${SCRIPTS_HEADER_TEMPLATE}" "${RELEASE_HEADERS_DIR}/FOClassic.fos" @ONLY NEWLINE_STYLE LF )

##
## process subdirectories
##

add_subdirectory( Docs )
add_subdirectory( Source )
if( WIN32 )
	add_subdirectory( Extensions.Dynamic )
endif()

##
## process per-version tasks
##

message( STATUS "Generating net header..." )

HashFile( "${ENGINE_GLOBAL_HEADER_FILE}" HASH_VERSION )
list( APPEND HASH_LIST "${HASH_VERSION}" )

HashText( "${HASH_LIST}" HASH_SEED )

GenerateNetHeader( "${ENGINE_NET_HEADER_TEMPLATE}" "${ENGINE_NET_HEADER_FILE}" "CMAKE_NET" "${SOURCE_DIR}/NetProtocol.h" "${HASH_SEED}" )
HashFile( "${ENGINE_NET_HEADER_FILE}" HASH_NET )
list( APPEND HASH_LIST "${HASH_NET}" )

##
## process source options
##
SourceOptions()

##
## collect cmake files which are not part of any target
##

file( GLOB ROOT_CMAKE_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.cmake" )
file( GLOB_RECURSE ROOT_CMAKE_CMAKE_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${FOCLASSIC_CMAKE_DIR}/*" )
add_custom_target( Configuration
	SOURCES
		${ROOT_CMAKE_FILES}
		${ROOT_CMAKE_CMAKE_FILES}
		.gitignore
		.appveyor.yml
		.travis.yml
)
source_group( TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${ROOT_CMAKE_FILES} ${ROOT_CMAKE_CMAKE_FILES} )
set_property( TARGET Configuration PROPERTY FOLDER "${FOCLASSIC_FOLDER_PREFIX}/Configuration" )

##
## prettify IDE
##

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set_property( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "${FOCLASSIC_FOLDER_PREFIX}/Configuration/CMake" )

##
## export selected variables to parent scope
##

if( FOCLASSIC_SUBPROJECT_BUILD )
	set( FOCLASSIC_VERSION     "${FOCLASSIC_VERSION}"     PARENT_SCOPE )
	set( FOCLASSIC_CMAKE_DIR   "${RELEASE_CMAKE_DIR}"     PARENT_SCOPE )
	set( FOCLASSIC_HEADERS_DIR "${RELEASE_HEADERS_DIR}"   PARENT_SCOPE )
endif()
