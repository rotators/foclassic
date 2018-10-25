## Prepare .pdb files in all VS configurations, including Release
function( PdbSetup target )
	if( MSVC )
		set( pdb "${target}.pdb" )
		set( pdb_public "${target}.pdb.PUBLIC" )

		set_property( TARGET ${target} APPEND_STRING PROPERTY COMPILE_FLAGS "/Zi " )
		set_property( TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS "/DEBUG /PDBSTRIPPED:$(ConfigurationName)/${pdb_public} /PDBALTPATH:${pdb} " )
	endif()
endfunction()
