#include "benchmark.h"



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * [JumpingMonkeyInstance::restart Generate a new forest and put Bob in it.]
 */
void JumpingMonkeyInstance::setup( const unsigned& n_trees )
{
	// Generate new forest
	forest.generate(n_trees);

	// Create uniform distribution
	std::uniform_int_distribution<unsigned> U( 0, n_trees-1 );

	// Put Bob in a random tree
	current_tree = U( *MersenneTwister::get_engine() );
}



/**
 * [JumpingMonkeyInstance::restart Put Bob in a random tree.]
 */
unsigned JumpingMonkeyInstance::restart()
{
	// Get number of trees
	const unsigned n = forest.size();

	// Create uniform distribution
	std::uniform_int_distribution<unsigned> U( 0, n-1 );

	// Put Bob in a random tree
	return current_tree = U( *MersenneTwister::get_engine() );	
}



/**
 * [JumpingMonkeyInstance::jump Make Bob jump to a neighbor tree.]
 * @return [The current tree in which is Bob.]
 */
unsigned JumpingMonkeyInstance::jump()
{
	return current_tree = forest.random_neighbor( current_tree );
}



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * [ResultsStatistics::process Compute statistics from benchmark results.]
 * @param results [The counts vector stored for each hunter.]
 */
void ResultsStatistics::process( const std::vector<int>& results )
{
	// Set number of samples
	const unsigned N = results.size();

	// Initialize members
	average = std = success_ratio = 0.0; 
	max = 0; min = std::numeric_limits<int>::max();

	// Compute average, success ratio, min and max
	for ( auto it = results.begin(); it != results.end(); ++it ) if ( *it > 0 ) 
		{
			// Update average and success ratio 
			average += *it; success_ratio += 1.0;

			// Update min/max
			if ( *it < min )      min = *it;
			else if ( *it > max ) max = *it;
		}

	average       /= success_ratio; 
	success_ratio /= N;

	// Evaluate std
	for ( auto it = results.begin(); it != results.end(); ++it )
	{
		const double a = *it - average; std += a*a;
	}	std /= (N-1);
}



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * [Benchmark::clear Clear all member data.]
 */
void Benchmark::clear()
{
	// Disable pointers
	angelo = jonathan = nullptr;

	// Reset scalars
	n_trees = n_instances = n_trials = current_instance = 0;

	// Clear vectors
	counts_angelo.clear();
	counts_jonathan.clear();

	// Clear instance
	instance.clear();
}



/**
 * [Benchmark::setup Setup the benchmark.]
 * @param  trees     [Number of trees for the current benchmark.]
 * @param  instances [Number of forests to generate.]
 * @param  trials    [Number of trials for each hunter in the forest.]
 * @return           [Setup success.]
 */
bool Benchmark::setup( const unsigned& trees, const unsigned& instances, const unsigned& trials )
{
	// Safety check
	if ( is_running() ) return false;

	// Set members
	n_trees = trees; n_instances = instances; n_trials = trials;

	// Allocate arrays
	counts_angelo.reserve( instances*trials );
	counts_jonathan.reserve( instances*trials );

	// Report success
	return true;
}



/**
 * [Benchmark::set_hunters Register both hunters.]
 * @param  A [Angelo.]
 * @param  J [Jonathan.]
 * @return   [Registration success.]
 */
bool Benchmark::set_hunters( ChuckInterface *A, ChuckInterface *J )
{
	// Safety check
	if ( !A || !J ) return false;

	// Set pointers
	angelo   = A; 
	jonathan = J;

	// Report success
	return true;
}




/**
 * [Benchmark::run Run the benchmark after setup and registration.]
 * @param  A [Angelo's results.]
 * @param  J [Jonathan's results.]
 * @return   [Benchmark success.]
 */
bool Benchmark::run( result_type& A, result_type& J )
{
	// Safety check
	if ( !is_ready() || is_running() ) return false;

	// Iterate on each instance
	for ( unsigned current_instance = 0; current_instance < n_instances; ++current_instance )
	{
		// Create new instance
		instance.setup(n_trees);

		// Set forests
		angelo->set_forest( instance.get_forest() );
		jonathan->set_forest( instance.get_forest() );

		// Run n_trials times
		run_instance();
	}

	// Compute results
	A.process( counts_angelo );
	J.process( counts_jonathan );

	// Report success
	return true;
}



	/********************     **********     ********************/



/**
 * [Benchmark::run_instance Internal method to run tests on one specific forest.]
 */
void Benchmark::run_instance()
{
	// Prepare local variables
	unsigned monkey;
	bool killed_a, killed_j;

	// Iterate on trials
	for ( unsigned t = 0; t < n_trials; ++t )
	{
		// Initialize counters and flags
		counts_angelo.push_back(0);
		counts_jonathan.push_back(0);
		killed_a = killed_j = false;

		// Put Bob somewhere
		monkey = instance.restart();

		// Start the competition
		while ( !killed_a || !killed_j )
		{
			// Let the hunters shoot
			if ( !killed_a ) run_shooting( angelo, monkey, counts_angelo.back(), killed_a );
			if ( !killed_j ) run_shooting( jonathan, monkey, counts_jonathan.back(), killed_j );

			// Let Bob jump
			monkey = instance.jump();
		}
	}
}



/**
 * [Benchmark::run_shooting Generic (independent of actual player) method to run a single shooting.]
 * @param chuck  [Pointer to Chuck's interface.]
 * @param bob    [Current position of Bob.]
 * @param count  [Current shot_count for the player.]
 * @param killed [Will be set to true if Bob is killed.]
 */
void Benchmark::run_shooting( ChuckInterface *chuck, const unsigned& bob, int& count, bool& killed )
{
	int shot = chuck->shoot(); ++count;

	if ( shot < 0 ) { count = -1; killed = true; }
	else if ( shot == (int) bob ) killed = true;
}



	/********************     **********     ********************/
	/********************     **********     ********************/


