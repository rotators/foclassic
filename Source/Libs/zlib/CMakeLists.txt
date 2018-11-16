add_library( zlib STATIC "" )
target_sources( zlib
	PRIVATE
		adler32.c
		compress.c
		crc32.c
		crc32.h
		deflate.c
		deflate.h
#		gzclose.c
#		gzguts.h
#		gzlib.c
#		gzread.c
#		gzwrite.c
		infback.c
		inffast.c
		inffast.h
		inffixed.h
		inflate.c
		inflate.h
		inftrees.c
		inftrees.h
		ioapi.c
		ioapi.h
		trees.c
		trees.h
		uncompr.c
		unzip.c
		unzip.h
		zconf.h
		zlib.h
		zutil.c
		zutil.h
)
target_include_directories( zlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
