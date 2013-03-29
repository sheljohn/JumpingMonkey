#ifndef __CHUCK__
#define __CHUCK__

#include <valarray>
#include "forest.h"



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Common interface for benchmarking.
 */
struct ChuckInterface
{
	virtual bool set_forest( const Forest& forest ) =0;
	virtual int shoot() =0;
};



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * John's strategy for Chuck. See notes for details.
 */
class Chuck : public ChuckInterface
{
public:

	// Clear all member data
	void clear();

	// Set from current forest
	bool set_forest( const Forest& forest );

	// Shoot
	int shoot();

	// Get shot count
	inline const unsigned& get_shot_count() const { return shot_count; }

private:

	// Swap pointers to arrays
	void swap_pointers();

	// Members
	// 
	double *pi_new, *pi_old;
	std::valarray<unsigned> degrees, strides, neighbors;
	std::valarray<double> array_a, array_b;

	unsigned next_shot, shot_count, n_nodes, n_edges;

};

#endif