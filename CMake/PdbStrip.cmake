### Prepare .pdb files for release
function( pdb_strip target )
	if( MSVC )
		set( pdb "${target}.stripped.pdb" )
		set_property( TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS "/PDBSTRIPPED:${pdb} /PDBALTPATH:${target}.pdb" )
		add_custom_command(
			TARGET ${target}
			POST_BUILD
			COMMAND if exist ${CMAKE_BINARY_DIR}/${pdb} (${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/${pdb} ${CMAKE_BINARY_DIR}/$(ConfigurationName)/${pdb})
		)
	endif()
endfunction()
