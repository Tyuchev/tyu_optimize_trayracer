#pragma once



class RandomGen
{
public:


	/// Produces an xorshift128 pseudo random number.
	unsigned FastRandom();

	/// Produces an xorshift128 psuedo based floating point random number in range 0..1
	/// Note that this is not a truly random random number generator
	float RandomFloat();

	/// Produces an xorshift128 psuedo based floating point random number in range -1..1
	/// Note that this is not a truly random random number generator
	float RandomFloatNTP();


	RandomGen()
		:x(123456789), y(362436069), z(521288629), w(88675123), t(1)
	{
		// empty
	}

private:


	unsigned x;
	unsigned y;
	unsigned z;
	unsigned w;
	unsigned t;
};



