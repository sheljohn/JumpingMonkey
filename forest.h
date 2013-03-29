#ifndef __FOREST__
#define __FOREST__

#include <cstdlib>
#include <vector>
#include <valarray>
#include <algorithm>
#include "random_engine.h"
#include "data_structures.h"



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Generate a random degree sequence (with di >= 1 for all i).
 * d's size must be >= 2.
 */
void generate_degree_sequence( std::valarray<unsigned>& d );



/**
 * Test that a degree sequence is graphic using the caracterisation proposed in:
 * 
 * Erdős, P. and Gallai, T. "Graphs with Prescribed Degrees of Vertices"
 * Mat. Lapok. 11, 264-274, 1960
 * 
 * d's size must be >= 2.
 */
bool graphic_sequence_test( const std::valarray<unsigned>& d );



/**
 * Generate a random graphic degree sequence (with di >= 1 for all i).
 * d's size must be >= 2.
 */
void generate_graphic_sequence( std::valarray<unsigned>& d );



/**
 * Internal class.
 * Generate a connex, undirected, random graph (CUR), using the procedure A proposed in:
 *
 * Bayati, M. and Kim, J.H. and Saberi, A. "A Sequential Algorithm for Generating Random Graphs"
 * Algorithmica 4, vol. 58, 860-910, 2010
 */
class CUR_Graph
{
public:

	// Initialize generator from prescribed degrees
	void initialize( const std::valarray<unsigned>& d );

	// Generate adjacency matrix (symmetric column storage)
	void generate( bool *G );

private:

	// Sequence Importance Sampling selection of edges
	unsigned sis_select() const;

	// Update post edge-selection
	void update( const unsigned& selected_edge, bool *G );

	// Compute the current sum of probabilities
	double sum_probabilities() const;

	// Members
	// 
	std::valarray<double>   probabilities;
	std::valarray<unsigned> degrees;

	unsigned n, remaining_edges;
	mutable double p_sum;
};



/**
 * Call this method to generate a CUR graph.
 */
void generate_cur_graph( const std::valarray<unsigned>& d, bool *G );



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * This class represents a forest, made of an arbitrary number of trees.
 * A forest is assumed to be a connex (degrees >= 1), undirected, random graph.
 *
 * The following interface allows to generate a random forest, and to simulate
 * efficiently a "jump" of the monkey from it's current tree to a neighbor tree.
 * The adjacency storage is designed to be access-efficient, somewhat like a CRS.
 */
class Forest // ... run!!
{
public:

	/********************     **********     ********************/

	typedef std::vector<unsigned>   vector_type;
	typedef std::valarray<unsigned> array_type;

	typedef std::pair<unsigned,unsigned> agl_pair_type;
	typedef std::vector<agl_pair_type>   agl_vector_type;

	/********************     **********     ********************/

	// Is the forest ready?
	inline operator bool() const { return n_trees; }

	// Reset member data.
	void clear();

	// Generate a random forest with n trees.
	void generate( const unsigned& n );

	// Display contents to stdout
	void print() const;

	/********************     **********     ********************/

	// Return a random neighbor of the input tree. This simulates a "jump".
	unsigned random_neighbor( const unsigned& tree ) const;

	/********************     **********     ********************/

	// Return the current number of trees.
	inline const unsigned& size() const { return n_trees; }

	// Get members
	inline const vector_type& get_neighbors() const { return neighbors; }
	inline const array_type&  get_degrees() const { return degrees; }
	inline const array_type&  get_strides() const { return strides; }

	// Export in ACM format for Angelo
	void acm_export( agl_pair_type& cfg, agl_vector_type& cx ) const;

	/********************     **********     ********************/

private:

	// Set member data after generating forest
	void postgen_set( const bool *G );

	// Members
	// 
	array_type  degrees, strides;
	vector_type neighbors;
	unsigned n_trees;

};

#endif