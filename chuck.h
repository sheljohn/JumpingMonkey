#ifndef __CHUCK__
#define __CHUCK__

#include <cstring>
#include <vector>
#include <bitset>
#include <list>

#include <valarray>
#include "forest.h"

#define CHUCK_EPSILON 1e-10
#define MAXSIZE (1<<21)



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Chuck is the hunter (Bob is the monkey).
 * This is the common interface for benchmarking.
 */
struct ChuckInterface
{
	/**
	 * This should initialize Chuck by providing information about the forest
	 * (number of trees, adjacency). The Forest object implements a method
	 * acm_export, which exports these information in a format resembling 
	 * the input of the original ACM problem.
	 */
	virtual bool set_forest( const Forest& forest ) =0;

	/**
	 * This method is called before the beginning of each hunt.
	 * Once Chuck has been initialized using set_forest() above,
	 * he should prepare for the hunt by resetting all relevant 
	 * valriables to their initial state. The forest remains 
	 * unchanged and the shoot method will not be called before 
	 * restart.
	 */
	virtual void restart() =0;

	/**
	 * This method is called at each shooting turn. It should return
	 * the index of the tree currently being shot (between 0 and n-1
	 * where n is the number of trees). If Chuck is stuck, or the 
	 * algorithm fails to provide a new choice, the method should 
	 * return -1, which will declare that Chuck gives up hunting the
	 * monkey and will be counted as a failure in the results statistics.
	 */
	virtual int shoot() =0;
};



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * John's strategy for Chuck. See notes for details.
 */
class Jonathan : public ChuckInterface
{
public:

	// Clear all member data
	void clear();

	// Set from current forest
	bool set_forest( const Forest& forest );

	// Reset probability tables to uniform distribution
	void restart();

	// Shoot
	int shoot();

private:

	// Swap pointers to arrays
	void swap_pointers();

	// Members
	// 
	double *pi_new, *pi_old;
	std::valarray<unsigned> degrees, neighbors;
	std::valarray<double> array_a, array_b;

	unsigned next_shot, n_nodes, n_edges;

};



/**
 * Angelo's strategy for Chuck.
 */
class Angelo : public ChuckInterface
{
public:

	// Clear member data
	void clear();

	// Set hunter from forest information
	bool set_forest( const Forest& forest );

	// Reset hunter for new hunt
	void restart();

	// Shoot
	int shoot();

private:

	// Angelo's code
	bool bfs();

	// Members
	//
	int n_trees, adjacency[22]; bool impossible;
	

	std::vector<int> shot_sequence;
	std::vector<int>::const_reverse_iterator current_shot;

};

#endif