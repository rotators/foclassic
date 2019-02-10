STATIC_ASSERT( sizeof(bool) == 1 );
STATIC_ASSERT( sizeof(char) == 1 );
STATIC_ASSERT( sizeof(int8) == 1 );
STATIC_ASSERT( sizeof(uint8) == 1 );
STATIC_ASSERT( sizeof(int16) == 2 );
STATIC_ASSERT( sizeof(uint16) == 2 );
STATIC_ASSERT( sizeof(int32) == 4 );
STATIC_ASSERT( sizeof(uint32) == 4 );
STATIC_ASSERT( sizeof(int64) == 8 );
STATIC_ASSERT( sizeof(uint64) == 8 );
STATIC_ASSERT( sizeof(uint) == 4 );

STATIC_ASSERT( sizeof(size_t) == sizeof(void*) );

#if defined (FO_X86)
STATIC_ASSERT( sizeof(size_t) == 4 );
#elif defined (FO_X64)
STATIC_ASSERT( sizeof(size_t) == 8 );
#endif
