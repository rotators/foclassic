#include "Core.h"

#include "DataMask.h"

C2BitMask::C2BitMask()
{
    memset( this, 0, sizeof(C2BitMask) );
}

C2BitMask::C2BitMask( uint width_2bit, uint height_2bit, uint8* ptr )
{
    Create( width_2bit, height_2bit, ptr );
}

C2BitMask::~C2BitMask()
{
    if( isAlloc )
        delete[] data;

    data = NULL;
}

void C2BitMask::Set2Bit( uint x, uint y, int val )
{
    if( x >= width || y >= height )
        return;

    uint8& b = data[y * width_b + x / 4];
    int    bit = (x % 4 * 2);

    UNSETFLAG( b, 3 << bit );
    SETFLAG( b, (val & 3) << bit );
}

int C2BitMask::Get2Bit( uint x, uint y )
{
    if( x >= width || y >= height )
        return 0;

    return (data[y * width_b + x / 4] >> (x % 4 * 2) ) & 3;
}

void C2BitMask::Fill( int fill )
{
    memset( data, fill, width_b * height );
}

void C2BitMask::Create( uint width_2bit, uint height_2bit, uint8* ptr )
{
    if( !width_2bit )
        width_2bit = 1;

    if( !height_2bit )
        height_2bit = 1;

    width = width_2bit;
    height = height_2bit;
    width_b = width / 4;

    if( width % 4 )
        width_b++;

    if( ptr )
    {
        isAlloc = false;
        data = ptr;
    }
    else
    {
        isAlloc = true;
        data = new uint8[width_b * height];
        Fill( 0 );
    }
}

uint8* C2BitMask::GetData()
{
    return data;
}

C4BitMask::C4BitMask( uint width_4bit, uint height_4bit, int fill )
{
    if( !width_4bit )
        width_4bit = 1;

    if( !height_4bit )
        height_4bit = 1;

    width = width_4bit;
    height = height_4bit;
    width_b = width / 2;

    if( width & 1 )
        width_b++;

    data = new uint8[width_b * height];
    Fill( fill );
}

C4BitMask::~C4BitMask()
{
    delete[] data;
}

void C4BitMask::Set4Bit( uint x, uint y, uint8 val )
{
    if( x >= width || y >= height )
        return;

    uint8& b = data[y * width_b + x / 2];

    if( x & 1 )
        b = (b & 0xF0) | (val & 0xF);
    else
        b = (b & 0xF) | (val << 4);
}

uint8 C4BitMask::Get4Bit( uint x, uint y )
{
    if( x >= width || y >= height )
        return 0;

    uint8& b = data[y * width_b + x / 2];

    if( x & 1 )
        return b & 0xF;
    else
        return b >> 4;
}

void C4BitMask::Fill( int fill )
{
    memset( data, fill, width_b * height );
}


CByteMask::CByteMask( uint _width, uint _height, int fill )
{
    if( !_width )
        _width = 1;

    if( !_height )
        _height = 1;

    width = _width;
    height = _height;
    data = new uint8[width * height];
    Fill( fill );
}

CByteMask::~CByteMask()
{
    delete[] data;
}

void CByteMask::SetByte( uint x, uint y, uint8 val )
{
    if( x >= width || y >= height )
        return;

    data[y * width + x] = val;
}

uint8 CByteMask::GetByte( uint x, uint y )
{
    if( x >= width || y >= height )
        return 0;

    return data[y * width + x];
}

void CByteMask::Fill( int fill )
{
    memset( data, fill, width * height );
}
