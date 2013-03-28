#include "stratejohn.h"



	/********************     **********     ********************/
	/********************     **********     ********************/



void Chuck::clear()
{
	// Disable pointers
	pi_new = pi_old = nullptr;

	// Clear arrays
	degrees.~valarray();
	strides.~valarray();
	neighbors.~valarray();

	lambdas.~valarray();
	array_a.~valarray();
	array_b.~valarray();

	// Scalars
	current_shot = shot_count = n_nodes = n_edges = 0;
}



bool Chuck::set_forest( const Forest& forest )
{
	// Safety check
	if ( !forest ) return false;

	// Set scalar properties first
	current_shot = shot_count = 0;
	n_nodes = forest.size();
	n_edges = forest.get_neighbors().size();

	// Resize arrays
	degrees.resize(n_nodes);
	strides.resize(n_nodes);

	array_a.resize(n_nodes, (1.0/n_nodes) );
	array_b.resize(n_nodes, (1.0/n_nodes) );

	lambdas.resize(n_edges);
	neighbors.resize(n_edges);

	// Copy data
	valarray_bounds<unsigned> u_bounds();
}