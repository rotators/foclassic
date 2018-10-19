#ifndef __RANDOM__
#define __RANDOM__

// Mersenne Twister pseudorandom number generator

class Randomizer
{
private:
    // Period parameters
    static const int periodN = 624;
    static const int periodM = 397;

    // Data
    unsigned int rndNumbers[periodN];
    int          rndIter;

public:
    Randomizer();
    Randomizer( unsigned int seed );

private:
    void GenerateState();

public:
    void Generate();
    void Generate( unsigned int seed );
    int  Random( int minimum, int maximum );
};

extern Randomizer DefaultRandomizer;
int Random( int minimum, int maximum );

#endif // __RANDOM__ //
