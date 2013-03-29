#include "chuck.h"



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * [Chuck::clear Clear all member data.]
 */
void Chuck::clear()
{
	// Disable pointers
	pi_new = pi_old = nullptr;

	// Clear arrays
	degrees.~valarray();
	neighbors.~valarray();

	array_a.~valarray();
	array_b.~valarray();

	// Scalars
	next_shot = shot_count = n_nodes = n_edges = 0;
}



/**
 * [Chuck::set_forest Introduce Chuck to the forest.]
 * @param  forest [Freshly generated forest.]
 * @return        [Initialization success.]
 */
bool Chuck::set_forest( const Forest& forest )
{
	// Safety check
	if ( !forest ) return false;

	// Set scalar properties first
	next_shot = shot_count = 0;
	n_nodes = forest.size();
	n_edges = forest.get_neighbors().size();

	// Copy degrees and neighbors
	degrees   = forest.get_degrees(); // Note: std requires to resize first, but gcc is a big boy.
	neighbors = std::valarray<unsigned>( forest.get_neighbors().data(), n_edges );

	// Load gun
	reload();

	// Set distributions pointers
	pi_new = &array_a[0];
	pi_old = &array_b[0];

	// Report success
	return true;
}



/**
 * [Chuck::swap_pointers This is a trick to avoid copying each time pi_new to pi_old. 
 * Instead, pointers are swapped just before the update. That way, we virtually replace 
 * the old-old distribution with the old-new (which becomes the new-old), and clear the 
 * old-old to store the new-new. Got it? If yes, frankly cheers. ;)]
 */
void Chuck::swap_pointers()
{
	double *ptr = pi_old;
	pi_old = pi_new;
	pi_new = ptr;
}



/**
 * [Chuck::reload Set tables to uniform probability distributions.]
 */
void Chuck::reload()
{
	array_a.resize(n_nodes, (1.0/n_nodes) );
	array_b.resize(n_nodes, (1.0/n_nodes) );
}



/**
 * [Chuck::shoot Shoot the darn monkey.]
 * @return [The chosen tree (that sounds like Avatar..). If Chuck is stuck, dial -1.]
 */
int Chuck::shoot()
{
	// Safety check
	if ( n_nodes == 0 || n_edges == 0 ) return -1;

	// Remember current shot
	const unsigned tree = next_shot;

	// I really don't know what to say; read.
	double pi_max      = 0.0; 
	unsigned *neighbor = &neighbors[0];
	swap_pointers(); ++shot_count;

	// Compute new probability distribution
	for ( unsigned t = 0; t < n_nodes; ++t )
	{
		// Reset probability
		pi_new[t] = 0.0;

		// Update probability
		for ( unsigned d = 0; d < degrees[t]; ++d, ++neighbor ) if ( *neighbor != tree ) 
			pi_new[t] += pi_old[ *neighbor ] / degrees[ *neighbor ];

		// Select tree with max probability for the next shot
		if ( pi_new[t] > pi_max )
		{ 
			next_shot = t; 
			pi_max    = pi_new[t]; 
		}
	}

	// If max has become too small, scale tables
	if ( pi_max <= CHUCK_EPSILON )
	{
		array_a /= CHUCK_EPSILON;
		array_b /= CHUCK_EPSILON;
	}

	// Return current shot
	return (int) tree;
}



	/********************     **********     ********************/
	/********************     **********     ********************/


