#ifndef __BENCHMARK__
#define __BENCHMARK__

#include <vector>
#include <limits>
#include "random_engine.h"
#include "chuck.h"



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Bob jumps from tree to tree in a randomly generated forest.
 */
class JumpingMonkeyInstance
{
public:

	// Clear member data
	inline void clear() { forest.clear(); }

	// Is the instance ready?
	inline operator bool() const { return forest; }

	// Generate a new forest and put Bob somewhere
	void setup( const unsigned& n_trees );

	// Put Bob in a random tree
	unsigned restart();

	// Make Bob jump
	unsigned jump();

	// Getters
	inline const Forest& get_forest() const { return forest; }
	inline const unsigned& where_is_bob() const { return current_tree; }

private:

	Forest forest;
	unsigned current_tree;
};



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Compute results statistics from benchmark.
 */
struct ResultsStatistics 
{ 
	double average, std, success_ratio; 
	unsigned min, max;

	void process( const std::vector<int>& results );
};



	/********************     **********     ********************/



/**
 * This is where the two strategies compete to kill Bob.
 * A benchmark is setup with a fixed number of trees in the forest.
 * At each of the n_instances instance, a new random forest is generated, 
 * in which Bob is placed at a random location n_trial times, for each of
 * which both hunters compete to kill him.
 */
class Benchmark
{
public:

	typedef ResultsStatistics result_type;

	// Clear all members
	void clear();

	// Setup new benchmark
	bool setup( const unsigned& trees, const unsigned& instances, const unsigned& trials );

	// Set players
	bool set_players( ChuckInterface *A, ChuckInterface *J );

	// Progress monitors
	inline bool is_ready() const 
		{ return angelo && jonathan && n_trees && (current_instance < n_instances); }
	inline bool is_running() const 
		{ return is_ready() && current_instance; }
	inline bool is_finished() const 
		{ return angelo && jonathan && n_trees && (current_instance == n_instances); }

	// Start a new instance of the benchmark
	bool start( result_type& A, result_type& J );

private:

	// Internal method to run one instance
	void run_instance();

	// Internal method to run one shooting
	void run_shooting( ChuckInterface *chuck, const unsigned& bob, int& count, bool& killed );

	// Members
	// 
	unsigned n_trees, n_instances, n_trials, current_instance;
	std::vector<int> counts_angelo, counts_jonathan;
	ChuckInterface *angelo, *jonathan;
	JumpingMonkeyInstance instance;
};


#endif