    STATIC_ASSERT( sizeof(char) == 1 );
    STATIC_ASSERT( sizeof(short) == 2 );
    STATIC_ASSERT( sizeof(int) == 4 );
    STATIC_ASSERT( sizeof(int64) == 8 );
    STATIC_ASSERT( sizeof(uchar) == 1 );
    STATIC_ASSERT( sizeof(ushort) == 2 );
    STATIC_ASSERT( sizeof(uint) == 4 );
    STATIC_ASSERT( sizeof(uint64) == 8 );
    STATIC_ASSERT( sizeof(bool) == 1 );
    STATIC_ASSERT( sizeof(size_t) == 4 );
    STATIC_ASSERT( sizeof(void*) == 4 );

    #if defined (FO_X86)
    STATIC_ASSERT( sizeof(size_t) == 4 );
    STATIC_ASSERT( sizeof(void*) == 4 );
    #elif defined (FO_X64)
    STATIC_ASSERT( sizeof(size_t) == 8 );
    STATIC_ASSERT( sizeof(void*) == 8 );
    #endif
    STATIC_ASSERT( sizeof(size_t) == sizeof(void*) );
