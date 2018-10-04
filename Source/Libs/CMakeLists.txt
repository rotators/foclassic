message( STATUS "Configuring libraries..." )

set( LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory( acm )
add_subdirectory( glew )
add_subdirectory( ogg )
add_subdirectory( portaudio )
add_subdirectory( sha2 )
add_subdirectory( zlib )

message( STATUS "Configuring libraries... OK" )
