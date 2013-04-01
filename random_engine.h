#ifndef __RANDOM_ENGINE__
#define __RANDOM_ENGINE__

//=============================================
// @filename     random_engine.h
// @date         April 1st 2013
// @author       Jonathan H. (Sheljohn on Github)
// @contact      ariel .dot hadida [at] gmail
// @license      Creative Commons by-nc-sa 3.0 
//               http://creativecommons.org/licenses/by-nc-sa/3.0/
//=============================================

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