#ifndef __RANDOM_ENGINE__
#define __RANDOM_ENGINE__

#include <random>



/**
 * Mersenne Twister engine with singleton access pattern.
 */
struct MersenneTwister
{
	static std::mt19937* get_engine()
	{
		static std::random_device rd;
		static std::mt19937 generator(rd());

		return &generator;
	}
};

#endif