##
## collect documentation
##

file( GLOB ROOT_MD_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.md" )
file( GLOB_RECURSE DOCS_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${DOCS_DIR}/*" )
add_custom_target( Documentation
	SOURCES
		${CMAKE_CURRENT_LIST_FILE}
		${ROOT_MD_FILES}
		${DOCS_FILES}
)
source_group( TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${DOCS_FILES} )
source_group( "CMake" FILES "${CMAKE_CURRENT_LIST_FILE}" )

set_property( TARGET Documentation PROPERTY FOLDER "${FOCLASSIC_FOLDER_PREFIX}/Documentation" )
