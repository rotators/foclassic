##
## automagically update cmake to version required by build
## SDK.Download/cmake/ must be in PATH before running this script
##
cmake_minimum_required( VERSION 3.0.0 )

# extract version from cmake_minimum_required()
function( GetRequiredVersion file var )
	file( STRINGS "${file}" ver REGEX "^[\ \t]*cmake_minimum_required" )
	if( ver )
		string( REGEX REPLACE ".*VERSION[ \t]+([0-9]+).*" "\\1"                   ver1 "${ver}" )
		string( REGEX REPLACE ".*VERSION[ \t]+([0-9]+\\.[0-9]+).*" "\\1"          ver2 "${ver}" )
		string( REGEX REPLACE ".*VERSION[ \t]+([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1" ver3 "${ver}" )
	endif()

	if( NOT ver1 OR ver1 MATCHES "cmake" )
		set( ver1 3 )
	endif()

	if( NOT ver2 OR ver2 MATCHES "cmake" )
		set( ver2 "${ver1}.0" )
	endif()

	if( NOT ver3 OR ver3 MATCHES "cmake" )
		set( ver3 "${ver2}.0" )
	endif()

	set( ${var} "${ver3}" PARENT_SCOPE )
endfunction()

GetRequiredVersion( "${CMAKE_CURRENT_LIST_DIR}/../CMakeLists.txt" VERSION_REQUIRED )
GetRequiredVersion( "${CMAKE_CURRENT_LIST_DIR}/../Build.cmake" VERSION_BUILD )

message( STATUS "CMake:           v${CMAKE_VERSION}" )
message( STATUS "CMake required:  v${VERSION_REQUIRED} (CMakeLists.txt)" )
message( STATUS "CMake required:  v${VERSION_BUILD} (Build.cmake)" )

# make sure to use highest required version
if( "${VERSION_BUILD}" VERSION_GREATER "${VERSION_REQUIRED}" )
	set( VERSION_REQUIRED "${VERSION_BUILD}" )
endif()

# set short version
if( "${VERSION_REQUIRED}" MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" )
	set( VERSION_REQ "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}" )
else()
	message( FATAL_ERROR "Invalid verson '${VERSION_REQUIRED}" )
endif()

# version check
if( NOT "${CMAKE_VERSION}" VERSION_LESS "${VERSION_REQUIRED}" )
	return()
endif()

set( DOWNLOAD_DIR "SDK.Download" )
set( DOWNLOAD_NAME "cmake-${VERSION_REQUIRED}-" )

# remove previously downloaded versions, if any
file( REMOVE_RECURSE "${DOWNLOAD_DIR}/cmake" )

# find 32/64 bit...
message( STATUS "Checking system information..." )

execute_process(
	COMMAND ${CMAKE_COMMAND} --system-information
	OUTPUT_VARIABLE system_information
)

if( "${system_information}" MATCHES "CMAKE_HOST_SYSTEM_PROCESSOR \"([A-Za-z0-9_]+)\"" )
	set( HOST_PROCESSOR "${CMAKE_MATCH_1}" )
else()
	message( ${system_information} )
	message( STATUS )
	message( FATAL_ERROR "Cannot find CPU type" )
endif()

if( "${HOST_PROCESSOR}" STREQUAL "AMD64" OR "${HOST_PROCESSOR}" STREQUAL "x86_64" )
	set( HOST_32 FALSE )
	set( HOST_64 TRUE )
else()
	set( HOST_32 TRUE )
	set( HOST_64 FALSE )
endif()

# build name of cmake file/directory
if( UNIX )
	set( DOWNLOAD_NAME "${DOWNLOAD_NAME}Linux-")
	if( HOST_32 )
		set( DOWNLOAD_NAME "${DOWNLOAD_NAME}i386" )
	elseif( HOST_64 )
		set( DOWNLOAD_NAME "${DOWNLOAD_NAME}x86_64" )
	endif()
	set( DOWNLOAD_EXT "tar.gz")
elseif( WIN32 )
	set( DOWNLOAD_NAME "${DOWNLOAD_NAME}win")
	if( HOST_32 )
		set( DOWNLOAD_NAME "${DOWNLOAD_NAME}32-x86")
	elseif( HOST_64 )
		set( DOWNLOAD_NAME "${DOWNLOAD_NAME}64-x64")
	endif()
	set( DOWNLOAD_EXT "zip" )
else()
	message( FATAL_ERROR "Cannot find OS type")
endif()

set( DOWNLOAD_FILE "${DOWNLOAD_NAME}.${DOWNLOAD_EXT}" )
set( DOWNLOAD_URL "http://cmake.org/files/v${VERSION_REQ}/${DOWNLOAD_FILE}" )

# download
message( STATUS "Downloading ${DOWNLOAD_URL}" )
file( MAKE_DIRECTORY "${DOWNLOAD_DIR}" )
file( DOWNLOAD "${DOWNLOAD_URL}" "${DOWNLOAD_DIR}/${DOWNLOAD_FILE}" )

# extract
message( STATUS "Extracting ${DOWNLOAD_FILE}" )
execute_process(
	COMMAND ${CMAKE_COMMAND} -E tar xzf ${DOWNLOAD_FILE}
	WORKING_DIRECTORY ${DOWNLOAD_DIR}
)

# cleanup
message( STATUS "Renaming ${DOWNLOAD_NAME} -> cmake")
file( RENAME "${DOWNLOAD_DIR}/${DOWNLOAD_NAME}" "${DOWNLOAD_DIR}/cmake" )

message( STATUS "Removing ${DOWNLOAD_FILE}" )
file( REMOVE "${DOWNLOAD_DIR}/${DOWNLOAD_FILE}" )
