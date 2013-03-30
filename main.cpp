#include <vector>
#include "benchmark.h"



int main()
{
	// Create both hunters
	Chuck jonathan, angelo;	

	// Create benchmark object
	Benchmark benchmark;

	// Set hunters
	benchmark.set_hunters( &angelo, &jonathan );

	// Create testing data and storage
	
		/**
		 * TEST FROM FOREST SIZE OF 6 TILL 21.
		 * RUN N^2 INSTANCES FOR EACH BENCHMARK WITH N TREES.
		 * RUN N TRIALS FOR EACH RANDOM FOREST WITH N TREES.
		 */
	 
	const unsigned N = 16;
	std::vector<ResultsStatistics> j_results(N), a_results(N);

	// Run benchmark
	unsigned i = 0;
	for ( unsigned n = 6; n < 22; ++n, ++i )
	{
		// Notify
		printf("******************************************\n");
		printf("    STARTING BENCHMARK WITH %u TREES      \n", n);
		printf("******************************************\n");

		// Setup benchmark
		benchmark.setup( n, n*n, n );

		// Run benchmark
		if ( !benchmark.run( a_results[i], j_results[i] ) )
		{
			printf("An error occured during benchmark. Aborting.");
			continue;
		}

		// Print results
		a_results[i].print("Angelo");
		j_results[i].print("Jonathan");
	}
}