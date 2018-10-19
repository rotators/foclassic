#include "Core.h"

#include <time.h>

#include "Random.h"

// Default randomizer
Randomizer DefaultRandomizer;

int Random( int minimum, int maximum )
{
    return DefaultRandomizer.Random( minimum, maximum );
}

//

Randomizer::Randomizer()
{
    Generate();
}

Randomizer::Randomizer( unsigned int seed )
{
    Generate( seed );
}

void Randomizer::GenerateState()
{
    static unsigned int mag01[2] = { 0x0, 0x9908B0DF };
    unsigned int        num;
    int                 i = 0;
    for( ; i < periodN - periodM; i++ )
    {
        num = (rndNumbers[i] & 0x80000000) | (rndNumbers[i + 1] & 0x7FFFFFFF);
        rndNumbers[i] = rndNumbers[i + periodM] ^ (num >> 1) ^ mag01[num & 0x1];
    }
    for( ; i < periodN - 1; i++ )
    {
        num = (rndNumbers[i] & 0x80000000) | (rndNumbers[i + 1] & 0x7FFFFFFF);
        rndNumbers[i] = rndNumbers[i + (periodM - periodN)] ^ (num >> 1) ^ mag01[num & 0x1];
    }
    num = (rndNumbers[periodN - 1] & 0x80000000) | (rndNumbers[0] & 0x7FFFFFFF);
    rndNumbers[periodN - 1] = rndNumbers[periodM - 1] ^ (num >> 1) ^ mag01[num & 0x1];

    rndIter = 0;
}

void Randomizer::Generate()
{
    Generate( (unsigned int)time( NULL ) );
}

void Randomizer::Generate( unsigned int seed )
{
    rndNumbers[0] = seed;
    for( int i = 1; i < periodN; i++ )
        rndNumbers[i] = (1812433253 * (rndNumbers[i - 1] ^ (rndNumbers[i - 1] >> 30) ) + i);
    GenerateState();
}

int Randomizer::Random( int minimum, int maximum )
{
    if( rndIter >= periodN )
        GenerateState();

    unsigned int num = rndNumbers[rndIter++];
    num ^= (num >> 11);
    num ^= (num << 7) & 0x9D2C5680;
    num ^= (num << 15) & 0xEFC60000;
    num ^= (num >> 18);

    #ifdef FO_X86
    return minimum + (int)( (double)num * (double)(1.0 / 4294967296.0) * (double)(maximum - minimum + 1) );
    #else     // FO_X64
    return minimum + (int)( (int64)num * (int64)(maximum - minimum + 1) / (int64)0x100000000 );
    #endif
}
