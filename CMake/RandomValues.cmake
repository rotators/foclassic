function( HashFile filename var )

	if( NOT EXISTS "${filename}" )
		message( FATAL_ERROR "File does not exists: '${filename}'" )
	endif()

	file( SHA512 "${filename}" hash_file )

	set( ${var} "${hash_file}" PARENT_SCOPE )

endfunction()

function( HashText text var )

	string( SHA512 hash_string "${text}" )

	set( ${var} "${hash_string}" PARENT_SCOPE )

endfunction()

function( HashToNumber hash var )

	string( SHA512 hash_number "${hash}" )
	string( REPLACE "a" "1" hash_number "${hash_number}" )
	string( REPLACE "b" "2" hash_number "${hash_number}" )
	string( REPLACE "c" "3" hash_number "${hash_number}" )
	string( REPLACE "d" "4" hash_number "${hash_number}" )
	string( REPLACE "e" "5" hash_number "${hash_number}" )
	string( REPLACE "f" "6" hash_number "${hash_number}" )

	string( SUBSTRING "${hash_number}" 0 9 hash_number )
	math( EXPR hash_number "${hash_number} + 0" )

	RandomValue( SEED ${hash_number} LENGTH_MIN 5 LENGTH_MAX 9 ALPHABET "123456789" VAR result )

	set( ${var} "${result}" PARENT_SCOPE )

endfunction()

function( SeedFile filename var )

	HashFile( "${filename}" seed_file )
	HashToNumber( "${seed_file}" ${var} )

	set( ${var} "${${var}}" PARENT_SCOPE )

endfunction()

function( SeedText text var )

	HashText( "${text}" seed_text )
	HashToNumber( "${seed_text}" ${var} )

	set( ${var} "${${var}}" PARENT_SCOPE )

endfunction()

function( GenerateNetHeader template_in template_out guard protocol_header hash )

	if( NOT EXISTS "${protocol_header}" )
		message( FATAL_ERROR "File does not exists: '${protocol_header}'" )
	endif()
	list( APPEND CMAKE_CONFIGURE_DEPENDS "${protocol_header}" )

	# limits
	set( max 200000 ) # rounded down
	set( uid 45976 35063 71733 58907 89715 7147 112712 77270 116273 5868 131070 65535 144429 121992 )
	string( LENGTH "${max}" len )

	# file top
	string( APPEND content "#ifndef __${guard}__\n" )
	string( APPEND content "#define __${guard}__\n" )
	string( APPEND content "\n" )
	string( APPEND content "// This file (including template) is generated and updated automagically.\n" )
	string( APPEND content "// Any and all hand-made changes will be lost.\n")
	string( APPEND content "\n" )

	file( STRINGS "${protocol_header}" list )
	foreach( msg IN LISTS list )
		if( "${msg}" MATCHES "^[\t\ ]*#[\t\ ]*define[\t\ ]+NETMSG_([A-Z0-9_]+)[\t\ ]+MAKE_NETMSG_HEADER" )
			set( msg "${CMAKE_MATCH_1}" )
			unset( fixer )
			unset( info )
			unset( id )

			RandomValue(
				SEED_FROM "${hash} <3 ${msg}"
				LENGTH_MIN 2
				LENGTH_MAX ${len}
				ALPHABET "981234567"
				VAR id
			)

			RandomValue( SEED ${id} LENGTH 3 ALPHABET "123456789" VAR fixer )

			while( id LESS 10 OR id GREATER max OR id IN_LIST uid )
				# debug only
				# list( APPEND info ${id} )
				if( id GREATER max )
					math( EXPR id "${id} % ${fixer}")
				else()
					math( EXPR id "${id} + ${fixer}" )
				endif()
			endwhile()

			list( APPEND uid ${id} )

			if( info )
				list( REVERSE info )
				string( REPLACE ";" " " info "${info}" )
				string( PREPEND info " // " )
				string( APPEND info " : ${fixer}")
			endif()

			set( NETUID_${msg} ${id} )
			string( APPEND content "#define NETUID_${msg} @NETUID_${msg}@${info}\n" )
		endif()
	endforeach()
	string( APPEND content "\n" )

	# salt
	SeedFile( "${protocol_header}" protocol )
	RandomValue( SEED ${protocol} LENGTH_MIN 2 LENGTH_MAX 5 ALPHABET "0123456789" VAR NETSALT_BIN SIGNED ADD_ZERO )
	RandomValue( SEED_FROM "${uid}" LENGTH_MIN 2 LENGTH_MAX 5 ALPHABET "9876543210" VAR NETSALT_BOUT SIGNED ADD_ZERO )
	RandomValue( SEED_FROM "${NETSALT_BIN}<->${NETSALT_BOUT}" LENGTH_MIN 2 LENGTH_MAX 4 ALPHABET "01234543210" VAR NETSALT_LOGIN SIGNED ADD_ZERO )
	RandomValue( SEED_FROM "${NETSALT_LOGIN}" LENGTH_MIN 2 LENGTH_MAX 4 ALPHABET "09876567890" VAR NETSALT_REGISTER SIGNED ADD_ZERO )
	string( APPEND content "#define NETSALT_BIN      @NETSALT_BIN@\n" )
	string( APPEND content "#define NETSALT_BOUT     @NETSALT_BOUT@\n" )
	string( APPEND content "#define NETSALT_LOGIN    @NETSALT_LOGIN@\n" )
	string( APPEND content "#define NETSALT_REGISTER @NETSALT_REGISTER@\n" )
	string( APPEND content "\n" )

	# file bottom
	string( APPEND content "#endif // __${guard}__ //\n" )

	# save template
	set( CMAKE_DISABLE_SOURCE_CHANGES OFF )
	file( WRITE "${template_in}" "${content}" )
	FormatSource( "${template_in}" "${CMAKE_CURRENT_LIST_DIR}" dummy )
	set( CMAKE_DISABLE_SOURCE_CHANGES ON )

	# generate header
	configure_file( "${template_in}" "${template_out}" @ONLY NEWLINE_STYLE LF )

endfunction()

function( RandomValue )

	set( optional ADD_ZERO SIGNED )
	set( single SEED SEED_FROM LENGTH LENGTH_MIN LENGTH_MAX ALPHABET VAR )
	set( multi )

	cmake_parse_arguments( rnd "${optional}" "${multi}" "${single}" ${ARGN} )

	if( NOT rnd_SEED )
		if( NOT rnd_SEED_FROM )
			message( FATAL_ERROR "!SEED && !SEED_FROM" )
		endif()
		SeedText( "${rnd_SEED_FROM}" rnd_SEED )
	endif()

	if( NOT rnd_LENGTH )
		if( NOT rnd_LENGTH_MIN OR NOT rnd_LENGTH_MAX )
			message( FATAL_ERROR "!LENGTH && !LENGTH_MIN && !LENGTH_MAX" )
		endif()
		foreach( num RANGE ${rnd_LENGTH_MIN} ${rnd_LENGTH_MAX} 1 )
			string( APPEND alphabet "${num}" )
		endforeach()
		string( RANDOM LENGTH 1 ALPHABET ${alphabet} RANDOM_SEED ${rnd_SEED} rnd_LENGTH )
	endif()

	if( NOT rnd_ALPHABET )
		message( FATAL_ERROR "!ALPHABET" )
	endif()

	if( NOT rnd_VAR )
		message( FATAL_ERROR "!VAR")
	endif()

	string( RANDOM LENGTH ${rnd_LENGTH} ALPHABET ${rnd_ALPHABET} RANDOM_SEED ${rnd_SEED} random_value )

	if( rnd_ADD_ZERO )
		math( EXPR random_value "${random_value} + 0" )
	endif()

	if( rnd_SIGNED )
		string( RANDOM LENGTH 1 ALPHABET "-+" RANDOM_SEED ${random_value} sign )
		if( "${sign}" STREQUAL "-" )
			set( random_value "${sign}${random_value}" )
		endif()
	endif()

	set( ${rnd_VAR} "${random_value}" PARENT_SCOPE )

endfunction()

