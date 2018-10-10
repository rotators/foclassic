##
## https://cmake.org/cmake/help/v3.12/
##

cmake_minimum_required( VERSION 3.12.2 )

# disable in-source builds
set( CMAKE_DISABLE_SOURCE_CHANGES ON )
set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )
get_filename_component( srcdir "${CMAKE_SOURCE_DIR}" REALPATH )
get_filename_component( bindir "${CMAKE_BINARY_DIR}" REALPATH )
if( ${srcdir} STREQUAL ${bindir} )
	file( REMOVE ${srcdir}/cmake_install.cmake ) # removing other files wont work
	message( STATUS "In-Source build are not allowed,
   cmake must be ran from dedicated build directory.

   Recommended
	${srcdir}/SDK*/

   Following files and directories MUST to be deleted
   before running cmake again, from any location.
	${srcdir}/CMakeCache.txt
	${srcdir}/CMakeFiles/
" )
	message( FATAL_ERROR "In-Source build" )
endif()
unset( srcdir )
unset( bindir )

project( FOClassic
	DESCRIPTION  "FOClassic"
	HOMEPAGE_URL "https://rotators.fodev.net/foclassic/"
	VERSION 1.1
	LANGUAGES C CXX
)

if( PROJECT_VERSION_MAJOR EQUAL 1 )
	set( PROJECT_VERSION_CODENAME "2077" )
elseif( PROJECT_VERSION_MAJOR EQUAL 2 )
	set( PROJECT_VERSION_CODENAME "V13" )
elseif( PROJECT_VERSION_MAJOR EQUAL 3 )
	set( PROJECT_VERSION_CODENAME "V15" )
else()
	set( PROJECT_VERSION_CODENAME "Shady Sands" )
endif()

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_SKIP_INSTALL_RULES TRUE )
set( CMAKE_VERBOSE_MAKEFILE TRUE )

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake" )
include(PdbStrip)
include(SourceOptions)

# globals
unset( FOCLASSIC_APPS )

# directories
set( FO_DIR         "${CMAKE_SOURCE_DIR}/Source" )
set( LIBS_DIR       "${FO_DIR}/Libs" )

set( AS_DIR         "${FO_DIR}/AngelScript" )
set( ASSIMP_DIR     "${FO_DIR}/Assimp" )
set( DAT_DIR        "${FO_DIR}/DatFile" )
set( EXTENSIONS_DIR "${CMAKE_SOURCE_DIR}/Extensions" )

if( MSVC )
	set( COMPILER_DEFINES "" )
	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_CRT_SECURE_NO_WARNINGS" )
#	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_CRT_SECURE_NO_DEPRECATE" )
#	set( COMPILER_DEFINES "${COMPILER_DEFINES} -D_WINSOCK_DEPRECATED_NO_WARNINGS" )
	set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COMPILER_DEFINES} /MTd /MP" )
	set( CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILER_DEFINES} /MTd /MP" )
	set( CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILER_DEFINES} /MT /MP" )
	set( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /INCREMENTAL:NO" )
endif()

##
## Source options
##
source_options()

##
## Find threads
##
set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )
include( CheckStructHasMember )
check_struct_has_member( "struct timespec" "tv_sec" "time.h" HAVE_STRUCT_TIMESPEC )
# must be defined for windows pthreads on newer MSVC
if( HAVE_STRUCT_TIMESPEC )
	add_definitions( -DHAVE_STRUCT_TIMESPEC )
endif( HAVE_STRUCT_TIMESPEC )

add_subdirectory( Source )
add_subdirectory( Extensions )

##
## Prettify IDE
##

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set_property( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake" )

set_target_properties( AngelScript AngelScriptPreprocessor PROPERTIES FOLDER "AngelScript" )
set_target_properties( ASCompiler PROPERTIES FOLDER "Tools" )
set_target_properties( acm DatFile glew sha2 zlib PROPERTIES FOLDER "Libs" )
set_target_properties( ExtensionClient ExtensionMapper ExtensionServer PROPERTIES FOLDER "Extensions" )
