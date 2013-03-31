#include "chuck.h"



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * [Jonathan::clear Clear all member data.]
 */
void Jonathan::clear()
{
	// Disable pointers
	pi_new = pi_old = nullptr;

	// Clear arrays
	degrees.~valarray();
	neighbors.~valarray();

	array_a.~valarray();
	array_b.~valarray();

	// Scalars
	next_shot = n_nodes = n_edges = 0;
}



/**
 * [Jonathan::set_forest Introduce Chuck to the forest.]
 * @param  forest [Freshly generated forest.]
 * @return        [Initialization success.]
 */
bool Jonathan::set_forest( const Forest& forest )
{
	// Safety check
	if ( !forest ) return false;

	// Set scalar properties first
	next_shot = 0;
	n_nodes   = forest.size();
	n_edges   = forest.get_neighbors().size();

	// Copy degrees and neighbors
	degrees   = forest.get_degrees(); // Note: std requires to resize first, but gcc is a big boy.
	neighbors = std::valarray<unsigned>( forest.get_neighbors().data(), n_edges );

	// Load gun
	restart();

	// Set distributions pointers
	pi_new = &array_a[0];
	pi_old = &array_b[0];

	// Report success
	return true;
}



/**
 * [Jonathan::swap_pointers This is a trick to avoid copying each time pi_new to pi_old. 
 * Instead, pointers are swapped just before the update. That way, we virtually replace 
 * the old-old distribution with the old-new (which becomes the new-old), and clear the 
 * old-old to store the new-new. Got it? If yes, frankly cheers. ;)]
 */
void Jonathan::swap_pointers()
{
	double *ptr = pi_old;
	pi_old = pi_new;
	pi_new = ptr;
}



/**
 * [Jonathan::reload Reset tables to uniform probability distributions.]
 */
void Jonathan::restart()
{
	array_a.resize(n_nodes, (1.0/n_nodes) );
	array_b.resize(n_nodes, (1.0/n_nodes) );
}



/**
 * [Jonathan::shoot Shoot the darn monkey.]
 * @return [The chosen tree (that sounds like Avatar..). If Chuck is stuck, dial -1.]
 */
int Jonathan::shoot()
{
	// Remember current shot
	const unsigned tree = next_shot;

	// Trick to avoid testing for the tree being shot
	swap_pointers(); pi_old[ tree ] = 0.0;

	// Find new max probability
	register double pi_max = 0.0; 

	// Iterator on neighbors
	const unsigned *neighbor = &neighbors[0];

	// Compute new probability distribution
	for ( unsigned t = 0; t < n_nodes; ++t )
	{
		// Reset probability
		pi_new[t] = 0.0;

		// Update probability
		for ( unsigned d = 0; d++ < degrees[t]; ++neighbor )
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



/**
 * [Angelo::clear Clear all member data.]
 */
void Angelo::clear()
{
	// Clear shot sequence
	shot_sequence.clear();

	// Reset iterator
	current_shot = shot_sequence.rbegin();
}



/**
 * [Angelo::set_forest Set member data from current forest.]
 * @param  forest [A freshly generated forest.]
 * @return        [Whether setting was successful or not.]
 */
bool Angelo::set_forest( const Forest& forest )
{
	// Get forest information
	Forest::agl_pair_type   cfg;
	Forest::agl_vector_type cx;

	forest.acm_export( cfg, cx );

	// Remember the number of trees
	n_trees = cfg.first;

	// Reset adjacency
	memset( adjacency, 0, 22*sizeof(int) );
	for ( auto it = cx.cbegin(); it != cx.cend(); ++it )
	{
		adjacency[ it->first ]  |= 1 << it->second;
		adjacency[ it->second ] |= 1 << it->first;
	}

	// Compute results
	impossible = bfs();

	// Restart iterator
	restart();

	// Report success
	return true;
}



/**
 * [Angelo::restart Reset shot sequence.]
 */
void Angelo::restart()
{
	current_shot = shot_sequence.rbegin();
}



/**
 * [Angelo::bfs Setup shooting strategy from the forest adjacency.]
 * @return [Whether the planning was successful or the algorithm failed.]
 */
bool Angelo::bfs()
{

	// Local variables
	register int exploration_set, complement_adjacency;

	// Begin with the exploration of every node except the first
	const int all_nodes_but_first = (1 << n_trees) - 1;

	// Allocate storage
	std::bitset<MAXSIZE> explored;
	std::vector<int> target_tree( MAXSIZE ), parent_set( MAXSIZE );

	// Allocate unexplored queue
	std::list<int> unexplored; 
	unexplored.push_back(all_nodes_but_first);

	// Explore nodes
	while( !unexplored.empty() )
	{

		// Pull new exploration set from queue
		if( (exploration_set = unexplored.front()) == 0 ) break;
		unexplored.pop_front();

		// Explore each node of the set
		for ( int i = 0, current_node = 1; i < n_trees; ++i, current_node = current_node << 1 ) 
			if( exploration_set & current_node )
		{

			// All nodes of the set, except the current one
			const int node_complement = exploration_set ^ current_node;

			// Create a new set with the adjacency of the complement
			complement_adjacency = 0;

			// Iterate on each node of the complement, adding its adjacency
			for ( int j = 0, other_node = 1; j < n_trees; ++j, other_node = other_node << 1 ) 
				if( node_complement & other_node ) 
					complement_adjacency |= adjacency[j];

			// Remember the current node, the current complement adjacency, and 
			// the link between this new set and the current exploration set.
			if( !explored[complement_adjacency] )
			{
				explored[complement_adjacency]    = true;
				parent_set[complement_adjacency]  = exploration_set;
				target_tree[complement_adjacency] = i;

				unexplored.push_back(complement_adjacency);
			}
		}

	}

	// Impossible
	if( exploration_set != 0 ) return false;

	// Rollback through the parent sets, and stack the corresponding target trees
	while( exploration_set != all_nodes_but_first )
	{
		shot_sequence.push_back(target_tree[exploration_set]);
		exploration_set = parent_set[exploration_set];
	}

	// Report success
	return true;
}



/**
 * [Angelo::shoot Simply pull the current target tree from the shooting sequence.]
 * @return [A tree index or -1 if the shooting sequence is empty or there is no strategy.]
 */
int Angelo::shoot()
{
	return ( impossible || current_shot == shot_sequence.rend() ) ? -1 : *current_shot++;
}


	/********************     **********     ********************/
	/********************     **********     ********************/


