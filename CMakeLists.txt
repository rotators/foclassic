##
## FOClassic
## https://rotators.fodev.net/foclassic/
## https://github.com/rotators/foclassic/
##

# https://cmake.org/cmake/help/v3.12/
cmake_minimum_required( VERSION 3.12.4 FATAL_ERROR )

# cmake includes
# special directories
set( ROOT_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/CMake" )
list( APPEND CMAKE_MODULE_PATH "${ROOT_CMAKE_DIR}" )

# disable in-source builds
include(DisableInSourceBuild)
DisableInSourceBuild()

# this file must contain unique line with version info
# "FOClassic VERSION [number:STAGE].[number:VERSION]"
project( FOClassic VERSION 2.3
	DESCRIPTION  "FOnline Community Edition (r412 base)"
	HOMEPAGE_URL "https://rotators.fodev.net/foclassic/"
	LANGUAGES C CXX
)

set( FOCLASSIC_STAGE   "${PROJECT_VERSION_MAJOR}" )
set( FOCLASSIC_VERSION "${PROJECT_VERSION_MINOR}" )

set( CMAKE_CONFIGURATION_TYPES "Release" )
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_SKIP_INSTALL_RULES TRUE )
#set( CMAKE_VERBOSE_MAKEFILE TRUE )

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

# sources directories
set( FO_DIR                         "${CMAKE_SOURCE_DIR}/Source" )
set( SHARED_DIR                     "${FO_DIR}/Shared" )
set( LIBS_DIR                       "${FO_DIR}/Libs" )

# libraries directories
set( AS_DIR                         "${FO_DIR}/AngelScript" )
set( DAT_DIR                        "${FO_DIR}/DatFile" )
set( EXTENSIONS_DIR                 "${CMAKE_SOURCE_DIR}/Extensions" )

# release directories
set( RELEASE_DIR_NAME               "FOClassic-v${PROJECT_VERSION_MINOR}" )
set( RELEASE_DIR                    "${CMAKE_BINARY_DIR}/${RELEASE_DIR_NAME}" )
set( RELEASE_CMAKE_DIR              "${RELEASE_DIR}/CMake" )
set( RELEASE_HEADERS_DIR            "${RELEASE_DIR}/Headers" )

# other directories
set( DOCS_DIR                        "${CMAKE_SOURCE_DIR}/Docs" )
set( TEMPLATES_DIR                   "${ROOT_CMAKE_DIR}/Templates" )
set( FOCMAKE_DIR                     "${CMAKE_BINARY_DIR}/CMake" )
set( DEFAULT_INCLUDE_DIR             "${FOCMAKE_DIR}/include" )

# templates
set( ENGINE_GLOBAL_HEADER_TEMPLATE   "${TEMPLATES_DIR}/CMake.h" )
set( ENGINE_NET_HEADER_TEMPLATE      "${TEMPLATES_DIR}/CMake.Net.h" )
set( SCRIPTS_HEADER_TEMPLATE         "${SHARED_DIR}/Defines.Public.h" )
set( VERSION_TEMPLATE                "${TEMPLATES_DIR}/VERSION" )

# special files
set( CHANGELOG_FILE                  "${DOCS_DIR}/changelog/CHANGELOG.md" )
set( ENGINE_GLOBAL_HEADER_FILE       "${DEFAULT_INCLUDE_DIR}/CMake.h" )
set( ENGINE_NET_HEADER_FILE          "${DEFAULT_INCLUDE_DIR}/CMake.Net.h" )
set( USER_FUNCTIONS_FILE             "${ROOT_CMAKE_DIR}/UserFunctions.cmake" )

##
## customization
##

if( EXISTS "${USER_FUNCTIONS_FILE}" )
	include( UserFunctions )
	HashFile( "${USER_FUNCTIONS_FILE}" HASH_USER )
	list( APPEND HASH_LIST "${HASH_USER}" )
endif()

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
message( STATUS "OLD LX  :: ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}" )
message( STATUS "OLD WIN :: ${CMAKE_C_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_RELEASE}" )
if( UNIX )
	set( CMAKE_C_FLAGS "-m32" )
	set( CMAKE_CCX_FLAGS "-m32" )
elseif( MSVC )
	set( COMPILER_DEFINES "" )
	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_CRT_SECURE_NO_WARNINGS" )
#	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_CRT_SECURE_NO_DEPRECATE" )
#	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_WINSOCK_DEPRECATED_NO_WARNINGS" )

	set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILER_DEFINES} /MT /MP" )
endif()

# find threads
set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )
include( CheckStructHasMember )
check_struct_has_member( "struct timespec" "tv_sec" "time.h" HAVE_STRUCT_TIMESPEC )
# HAVE_STRUCT_TIMESPEC must be defined for windows pthreads on newer MSVC
if( HAVE_STRUCT_TIMESPEC )
	add_definitions( -DHAVE_STRUCT_TIMESPEC=1 )
endif( HAVE_STRUCT_TIMESPEC )

# create engine header
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
add_subdirectory( Extensions )

##
## process per-version tasks
##

message( STATUS "Generating net header..." )

HashFile( "${ENGINE_GLOBAL_HEADER_FILE}" HASH_VERSION )
list( APPEND HASH_LIST "${HASH_VERSION}" )

GenerateNetHeader( "${ENGINE_NET_HEADER_TEMPLATE}" "${ENGINE_NET_HEADER_FILE}" "CMAKE_NET" "${FO_DIR}/NetProtocol.h" "${HASH_LIST}" )
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
file( GLOB_RECURSE ROOT_CMAKE_CMAKE_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${ROOT_CMAKE_DIR}/*" )
add_custom_target( Configuration
	SOURCES
		${ROOT_CMAKE_FILES}
		${ROOT_CMAKE_CMAKE_FILES}
		.gitignore
		.appveyor.yml
		.travis.yml
)
source_group( TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${ROOT_CMAKE_FILES} ${ROOT_CMAKE_CMAKE_FILES} )
set_property( TARGET Configuration PROPERTY FOLDER Configuration )

##
## prettify IDE
##

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set_property( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "Configuration/CMake" )
