#include "random.h"

//------------------------------------------------------------------------------
/**
	XorShift128 implementation.
*/
unsigned
RandomGen::FastRandom()
{
	// These are predefined to give us the largest
	// possible sequence of random numbers

    t = x ^ (x << 11);
    x = y;
	y = z;
	z = w;
    return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

//------------------------------------------------------------------------------
/**
    Thanks to Nic Werneck (https://xor0110.wordpress.com/2010/09/24/how-to-generate-floating-point-random-numbers-efficiently/)
*/
float
RandomGen::RandomFloat()
{
    union
    {
        unsigned int i;
        float f;
    } r;
    r.i = FastRandom() & 0x007fffff | 0x3f800000;
    return r.f - 1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float
RandomGen::RandomFloatNTP()
{
    union
    {
        unsigned int i;
        float f;
    } r;
    r.i = FastRandom() & 0x007fffff | 0x40000000;
    return r.f - 3.0f;
}

