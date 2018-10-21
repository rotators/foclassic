#
# Global version validation, sets FOCLASSIC_CODENAME
# 
function( FOClassicVersion )

	foreach( var IN ITEMS FOCLASSIC_STAGE FOCLASSIC_VERSION )
		if( NOT ${var} )
			message( FATAL_ERROR "Variable ${var} not set" )
		elseif( NOT "${${var}}" MATCHES "^[0-9]+$" )
			message( FATAL_ERROR "Variable ${var} ('${${var}}') not a number" )
		endif()
	endforeach()

	set( names
		"Wasteland"

		"2077"
		"V13"
		"V15"
		"Shady Sands"
		"Junktown"
		"Hub"
	)

	list( LENGTH names length )
	if( ${FOCLASSIC_STAGE} GREATER 0 AND ${FOCLASSIC_STAGE} LESS ${length} )
		list( GET names ${FOCLASSIC_STAGE} codename )
	else()
		list( GET names 0 codename )
	endif()

	if( "${codename}" STREQUAL "" )
		message( FATAL_ERROR "Codename is empty" )
	endif()

	message( STATUS )
	message( STATUS "FOClassic v${FOCLASSIC_VERSION} (${codename})" )
	if( PROJECT_DESCRIPTION )
		message( STATUS "${PROJECT_DESCRIPTION}" )
	endif()
	if( PROJECT_HOMEPAGE_URL )
		message( STATUS "${PROJECT_HOMEPAGE_URL}" )
	endif()
	message( STATUS )

	set( FOCLASSIC_CODENAME "${codename}" PARENT_SCOPE )

endfunction()
