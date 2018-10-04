message( STATUS "Configuring libraries..." )

set( LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory( acm )
add_subdirectory( distorm )
add_subdirectory( glew )
add_subdirectory( il )
add_subdirectory( jpeg )
add_subdirectory( ogg )
add_subdirectory( png )
add_subdirectory( portaudio )
add_subdirectory( pthread )
add_subdirectory( sha2 )
add_subdirectory( theora )
add_subdirectory( zlib )

message( STATUS "Configuring libraries... OK" )
