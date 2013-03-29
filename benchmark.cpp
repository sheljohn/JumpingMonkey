#include "benchmark.h"



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * [JumpingMonkeyInstance::print Print current contents to stdout.]
 */
void JumpingMonkeyInstance::print() const
{
	forest.print();
	printf("Current tree is: %u\n", current_tree);
}



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
	n_samples = results.size();

	// Initialize members
	average = std = success_ratio = 0.0; 
	max = 0; min = std::numeric_limits<int>::max();

	// Compute average, success ratio, min and max
	for ( auto it = results.begin(); it != results.end(); ++it ) if ( *it > 0 ) 
		{
			// Update average and success ratio 
			average += *it; success_ratio += 1.0;

			// Update min/max
			if ( *it < min ) min = *it;
			if ( *it > max ) max = *it;
		}

	average       /= success_ratio; 
	success_ratio /= n_samples;

	// Evaluate std
	if ( n_samples > 1 ){ 
	for ( auto it = results.begin(); it != results.end(); ++it )
	{
		const double a = *it - average; std += a*a;
	}	std /= (n_samples-1); }
}



/**
 * [ResultsStatistics::print Display contents on stdout.]
 */
void ResultsStatistics::print() const
{
	printf("Results statistics (%u samples):\n", n_samples);
	printf("\t- Min=%d, Max=%d\n", min, max);
	printf("\t- Avg=%.5f, Std=%.5f\n", average, std);
	printf("\t- Success ratio=%.2f%%\n", 100*success_ratio);
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
	n_trees = n_instances = n_trials = 0;

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
 */
void Benchmark::setup( const unsigned& trees, const unsigned& instances, const unsigned& trials )
{
	// Set members
	n_trees = trees; n_instances = instances; n_trials = trials;

	// Allocate counts
	counts_angelo.reserve( n_instances*n_trials );
	counts_jonathan.reserve( n_instances*n_trials );

	// DEBUG
	printf("---------- BENCHMARK ----------\n");
	printf( "Number of trees    : %u\n", n_trees );
	printf( "Number of instances: %u\n", n_instances );
	printf( "Number of trials   : %u\n", n_trials );
	printf( "Counts sizes       : A(%u), J(%u)\n", counts_angelo.size(), counts_jonathan.size() );
	printf("-------------------------------\n");
}



/**
 * [Benchmark::set_hunters Register both hunters.]
 * @param  A [Angelo.]
 * @param  J [Jonathan.]
 */
void Benchmark::set_hunters( ChuckInterface *A, ChuckInterface *J )
{
	angelo   = A; 
	jonathan = J;
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
	if ( !*this ) return false;

	// Clear counts
	counts_angelo.clear();
	counts_jonathan.clear();

	// Iterate on each instance
	for ( unsigned i = 0; i < n_instances; ++i )
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


