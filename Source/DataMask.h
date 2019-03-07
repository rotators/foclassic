#ifndef __DATA_MASK__
#define __DATA_MASK__

#include "Types.h"

class C2BitMask
{
public:
    C2BitMask();
    C2BitMask( uint width_2bit, uint height_2bit, uint8* ptr );
    ~C2BitMask();

    void   Set2Bit( uint x, uint y, int val );
    int    Get2Bit( uint x, uint y );
    void   Fill( int fill );
    void   Create( uint width_2bit, uint height_2bit, uint8* ptr );
    uint8* GetData();

private:
    bool   isAlloc;
    uint8* data;
    uint   width;
    uint   height;
    uint   width_b;
};

class C4BitMask
{
public:
    C4BitMask( uint width_4bit, uint height_4bit, int fill );
    ~C4BitMask();

    void  Set4Bit( uint x, uint y, uint8 val );
    uint8 Get4Bit( uint x, uint y );
    void  Fill( int fill );

private:
    uint8* data;
    uint   width;
    uint   height;
    uint   width_b;
};

class CByteMask
{
public:
    CByteMask( uint _width, uint _height, int fill );
    ~CByteMask();

    void  SetByte( uint x, uint y, uint8 val );
    uint8 GetByte( uint x, uint y );
    void  Fill( int fill );

private:
    uint8* data;
    uint   width;
    uint   height;
};

/*class CBitMask
   {
   public:
        void SetBit(uint x, uint y)
        {
                if(x>=width || y>=height) return;
                data[y*width_dw+x/32]|=1<<(x%32);
        }

        int GetBit(uint x, uint y)
        {
                if(x>=width || y>=height) return 0;
                return (data[y*width_dw+x/32]>>(x%32))&1;
        }

        void Fill(int fill)
        {
                memset(data,fill,width_dw*height*sizeof(uint));
        }

        CBitMask(uint width_bit, uint height_bit, uint* ptr, int fill)
        {
                if(!width_bit) width_bit=1;
                if(!height_bit) height_bit=1;
                width=width_bit;
                height=height_bit;
                width_dw=width/32;
                if(width%32) width_dw++;
                if(ptr)
                {
                        isAlloc=false;
                        data=ptr;
                }
                else
                {
                        isAlloc=true;
                        data=new uint[width_dw*height];
                }
                Fill(fill);
        }

        ~CBitMask()
        {
                if(isAlloc) delete[] data;
                data=NULL;
        }

   private:
        bool isAlloc;
        uint* data;
        uint width;
        uint height;
        uint width_dw;
   };*/

/*template<int Size>
   class CBitMask_
   {
   public:
        void SetBit(uint x, uint y, int val)
        {
                if(x>=width || y>=height) return;
                data[y*width_dw+x/32]|=1<<(x%32);
        }

        int GetBit(uint x, uint y)
        {
                if(x>=width || y>=height) return 0;
                return (data[y*width_dw+x/32]>>(x%32))&1;
        }

        void Fill(int fill)
        {
                memset(data,fill,width_dw*height*sizeof(uint));
        }

        CBitMask(uint width_bit, uint height_bit, int fill)
        {
                width=width_bit*Size;
                height=height_bit*Size;

                width_dw=width/32;
                if(width%32) width_dw++;

                data=new uint[width_dw*height];

                Fill(fill);
        }

        ~CBitMask()
        {
                delete[] data;
        }

   private:
        uint* data;
        uint width;
        uint height;
        uint width_dw;
   };*/

#endif // __DATA_MASK__
