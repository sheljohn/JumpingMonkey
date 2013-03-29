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
	strides.~valarray();
	neighbors.~valarray();

	array_a.~valarray();
	array_b.~valarray();

	// Scalars
	next_shot = shot_count = n_nodes = n_edges = 0;
}



/**
 * [Chuck::set_forest Initialize Chuck with the current forest (that must hurt..).]
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

	// Resize and assign arrays 
	// Note: std requires to resize first, but gcc is a big boy.
	degrees = forest.get_degrees();
	strides = forest.get_strides();

	// Uniform probability distributions at first
	array_a.resize(n_nodes, (1.0/n_nodes) );
	array_b.resize(n_nodes, (1.0/n_nodes) );

	// Copy neighbors
	neighbors = std::valarray<unsigned>( forest.get_neighbors().data(), n_edges );

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
 * old-old to store the new-new. Got it? If yes, frankly cheers. ;) If no, you should
 * watch Cloud Atlas, Tommy will teach you the true-truth.]
 */
void Chuck::swap_pointers()
{
	double *ptr = pi_old;
	pi_old = pi_new;
	pi_new = ptr;
}



/**
 * [Chuck::shoot Shoot the darn monkey.]
 * @return [The chosen tree (now that sounds like Avatar..). If Chuck is stuck, dial -1.]
 */
int Chuck::shoot()
{
	// Remember current shot
	const unsigned tree = next_shot;

	// Reset probability distribution
	swap_pointers();
	for ( unsigned t = 0; t < n_nodes; ++t ) pi_new[t] = 0.0;

	// Compute new probability distribution
	double pi_max = 0.0; unsigned neighbor;
	for ( unsigned t = 0; t < n_nodes; ++t ) if ( t != tree )
	{
		// Update probability of tree t
		for ( unsigned d = 0; d < degrees[t]; ++d )
		{
			neighbor   = neighbors[ strides[t] + d ];
			pi_new[t] += pi_old[ neighbor ] / degrees[ neighbor ];
		}

		// Update next shot
		if ( pi_new[t] > pi_max )
		{ 
			next_shot = t; 
			pi_max    = pi_new[t]; 
		}
	}
	
	// Increment shot count
	++shot_count;

	// Return current shot
	return (int) tree;
}



	/********************     **********     ********************/
	/********************     **********     ********************/


