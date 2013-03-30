#include "forest.h"



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Generate a random degree sequence (with di >= 1 for all i).
 * d's size must be >= 2.
 *
 * Each element of d will be sampled from the uniform integer distribution
 * on {1, 2, .., n-1}.
 */
void generate_degree_sequence( std::valarray<unsigned>& d )
{
	// Safety check
	const unsigned n = d.size();
	if ( n < 2 ) return;

	// Create uniform distribution
	std::uniform_int_distribution<unsigned> U(1, n-1);
	std::mt19937 *engine = MersenneTwister::get_engine();

	// Create iterator
	valarray_bounds<unsigned> bounds(d);

	// Generate n random integers between 1 and (n-1)
	if (bounds) for ( auto it = bounds.begin(); it != bounds.end(); ) *it++ = U( *engine );
}



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Test that a degree sequence is graphic using the caracterisation proposed in:
 * 
 * Erdős, P. and Gallai, T. "Graphs with Prescribed Degrees of Vertices"
 * Mat. Lapok. 11, 264-274, 1960
 * 
 * d's size must be >= 2.
 */
bool graphic_sequence_test( const std::valarray<unsigned>& d )
{
	// Safety check
	const unsigned n = d.size();
	if ( n < 2 ) return false;

	// Storage for cumulative sum of d
	static std::valarray<unsigned> cs;

	// Resize cumulative sum and set first element
	cs.resize(n-1); cs[0] = d[0];

	// NOTE: cs[0] is necessarily less than (n-1) because of the generation process

	// Compute cumulative sum
	for ( unsigned k = 1; k < (n-1); ++k ) cs[k] = cs[k-1] + d[k];

	// Verify that the sum is even
	if ( (cs[n-2] + d[n-1]) % 2u == 1u ) return false;

	// Check all inequalities
	for ( unsigned r = 2; r < n; ++r )
	{
		// Compute partial sum
		unsigned sum = 0;
		for ( unsigned k = r; k < n; ++k ) sum += std::min(r, d[k]);

		// Verify current inequality
		if ( cs[r-1] > r*(r-1) + sum ) return false;
	}

	// All inequalities were verified
	return true;
}



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Generate a random graphic degree sequence (with di >= 1 for all i).
 * d's size must be >= 2.
 *
 * Each element of d will be sampled from the uniform integer distribution
 * on {1, 2, .., n-1}.
 */
void generate_graphic_sequence( std::valarray<unsigned>& d )
{
	// Safety check
	if ( d.size() < 2 ) return;

	// Recursively generate sequences until one is graphic
	bool success = false;

	while ( !success )
	{
		generate_degree_sequence(d);
		success = graphic_sequence_test(d);
	}
}



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * [CUR_Graph::initialize Initialize generation variables.]
 * @param d [Prescribed degrees for the generated graph.]
 */
void CUR_Graph::initialize( const std::valarray<unsigned>& d )
{
	// Initialize variables
	n = d.size(); p_sum = 0.0;

	// Resize both valarrays
	probabilities.resize( n*(n+1) >> 1, 0.0 );
	degrees.resize(n); degrees = d;

	// Compute number of edges
	remaining_edges = degrees.sum() >> 1;
	const double m  = static_cast<double>(remaining_edges << 2);

	// Compute initial probabilities
	double *p = &probabilities[1];

	for ( unsigned j = 0; j < n-1; ++j, ++p )
	for ( unsigned i = j+1; i < n; ++i )
	{
		*p     = 1.0 - (d[i] * d[j]) / m;
		p_sum += (d[i] * d[j]) * *p++;
	}
}



/**
 * [CUR_Graph::sum_probabilities Compute the current sum of probabilities.]
 * @return [The sum.]
 */
double CUR_Graph::sum_probabilities() const
{
	// Get boundaries
	const unsigned* d = &degrees[0];
	const double* p   = &probabilities[1];

	// Compute sum
	p_sum = 0.0;
	for ( unsigned j = 0; j < n-1; ++j, ++p )
	for ( unsigned i = j+1; i < n; ++i )
		p_sum += (d[i]*d[j]) * *p++;

	return p_sum;
}



/**
 * [CUR_Graph::sis_select Random selection of an edge using Sequence Importance Sampling.]
 * @return [Plain index in the probability matrix, equiv. to an edge index.]
 */
unsigned CUR_Graph::sis_select() const
{
	// Static uniform distribution in [0,1)
	static std::uniform_real_distribution<double> U(0.0, 1.0);

	// Sample random number and match level on corresponding CDF
	const double level = p_sum * U( *MersenneTwister::get_engine() );
	unsigned edge = 0, i = 1, j = 0;

	// NOTE: probabilities[0] is 0.0 by construction
	for ( double cdf = 0.0; 
		level >= (cdf += degrees[i]*degrees[j] * probabilities[++edge]); )
		if ( ++i % n == 0 ) if ( (i = ++j) == n ) throw; // Irrecoverable loss of precision

	// Return selected edge
	return edge;
}



/**
 * [CUR_Graph::update Update probabilities, degrees and remaining edges after selection.]
 * @param selected_edge [Output of sis_select().]
 * @param G             [Input of generate().]
 */
void CUR_Graph::update( const unsigned& selected_edge, bool *G )
{
	// Static indexer
	static SMCSIndexer indexer;

	// Get subindices
	unsigned i,j; // NOTE: i and j could be taken after the for-loop in sis_select()...
	indexer.ind2sub( n, selected_edge, i, j );

	// Decrement corresponding degrees
	--degrees[i]; --degrees[j];

	// Decrement remaining edges or break recursion
	remaining_edges = sum_probabilities() ? remaining_edges-1 : 0;

	// Set edge in adjacency matrix
	G[selected_edge] = true;
}



/**
 * [CUR_Graph::generate Generate a random graph and output corresponding binary adjacency matrix 
 * with symmetric column storage convention.]
 * @param G [Must be bool[ n(n+1)/2 ] initialized to false.]
 */
void CUR_Graph::generate( bool *G )
{
	// Iterate selection and update until no more edge can be selected
	while ( remaining_edges ) update(sis_select(),G);
}



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Generate a connex, undirected, random graph (CUR), using the procedure A proposed in:
 *
 * Bayati, M. and Kim, J.H. and Saberi, A. "A Sequential Algorithm for Generating Random Graphs"
 * Algorithmica 4, vol. 58, 860-910, 2010
 *
 * d's size must be >= 2., G must be a bool[ n(n+1)/2 ] initialized to false.
 */
void generate_cur_graph( const std::valarray<unsigned>& d, bool *G )
{
	// Static CUR_Graph instance
	static CUR_Graph graph;

	// Safety checks
	if ( (d.size() < 2) || !G ) return;

	// Initialize graph generator
	graph.initialize(d);

	// Generate new graph
	graph.generate(G);
}



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * [Forest::clear Clear all members.]
 */
void Forest::clear()
{
	// Destruct valarrays
	degrees.~valarray();
	strides.~valarray();
	neighbors.clear();

	// Reset number of trees
	n_trees = 0;
}



/**
 * [Forest::print Display contents in stdout.]
 */
void Forest::print() const
{
	printf("**************************************\n");

	// Show degrees, neighbors and strides
	printf(" degrees(%u) = [", degrees.size() );
	for ( unsigned k = 0; k < degrees.size(); ++k ) printf( " %u ", degrees[k] );
	printf("]\n");

	printf(" strides(%u) = [", strides.size() );
	for ( unsigned k = 0; k < strides.size(); ++k ) printf( " %u ", strides[k] );
	printf("]\n");

	printf(" neighbors(%u) = [", neighbors.size() );
	for ( unsigned k = 0; k < neighbors.size(); ++k ) printf( " %u ", neighbors[k] );
	printf("]\n");

	printf("------------------\n");
	for ( unsigned t = 0; t < n_trees; ++t )
	for ( unsigned d = 0; d < degrees[t]; ++d )
		printf("%u %u\n", t, neighbors[ strides[t] + d ]);
	printf("======================================\n");	
}



/**
 * [Forest::generate Generate a new forest with n trees.]
 * @param n [Number of trees (>=2).]
 */
void Forest::generate( const unsigned& n )
{
	// Safety check
	if ( n < 2 ) return;

	// Set number of trees
	n_trees = n;

	// Resize degrees and strides
	degrees.resize(n,1);
	strides.resize(n+1,0);

	// Allocate boolean adjacency matrix
	const unsigned gsize = n*(n+1) >> 1;
	bool *graph          = (bool*) malloc( gsize*sizeof(bool) );
	bool * const gend    = graph + gsize;

	// Generation process can take a few cycles
	bool success = false;

	while ( !success )
	{
		// Reinitialize graph
		for ( bool *g = graph; g != gend; ) *g++ = false;

		// Generate random graphical sequence of degrees
		generate_graphic_sequence(degrees);

		// Make a reasonnable assumption about the number of edges
		neighbors.clear();
		neighbors.reserve( degrees.sum() >> 1 );

		// Generate random graph
		generate_cur_graph(degrees,graph);

		// Set neighbors and strides
		success = postgen_set(graph);
	}

	// Free adjacency matrix
	free(graph);
}



/**
 * [Forest::set_neighborhoods Private method to populate the neighbors array 
 * from a generated graph.]
 * @param  G [Output of generate_cur_graph().]
 * @return   [Rerun generation process if false.]
 */
bool Forest::postgen_set( const bool *G )
{
	// Static indexer
	static SMCSIndexer indexer;

	// Fill neighbors
	for ( unsigned tree = 0; tree < n_trees; ++tree )
	{
		// Corresponding column of G
		for ( unsigned i = tree+1; i < n_trees; ++i )
		if ( G[ indexer.sub2ind(n_trees,i,tree) ] ) neighbors.push_back(i);

		// Corresponding row of G
		for ( unsigned j = 0; j < tree; ++j )
		if ( G[ indexer.sub2ind(n_trees,tree,j) ] ) neighbors.push_back(j);

		// Set stride for next tree
		strides[tree+1] = neighbors.size();

		// Set final degree and detect if null
		if ( (degrees[tree] = strides[tree+1] - strides[tree]) == 0) return false;
	}

	// Report success
	return true;
}



/**
 * [Forest::random_neighbor Simulate a random jump from the input tree using the adjacency 
 * structure of the forest.]
 * @param  tree [The origin of the jump.]
 * @return      [The new tree after the jump.]
 */
unsigned Forest::random_neighbor( const unsigned& tree ) const
{
	// Create uniform distribution
	std::uniform_int_distribution<unsigned> U( 0, degrees[tree]-1 );

	// Return random neighbor
	return neighbors.at( strides[tree] + U( *MersenneTwister::get_engine() ) );
}



/**
 * [Forest::acm_export Export forest data for Angelo.]
 * @param cfg [Pair of unsigned; (n_trees,n_links).]
 * @param cx  [Vector of pairs of unsigned (tree_a,tree_b).]
 */
void Forest::acm_export( agl_pair_type& cfg, agl_vector_type& cx ) const
{
	// Set configuration first
	cfg.first  = n_trees;
	cfg.second = neighbors.size() >> 1;

	// Set adjacency information
	cx.resize( cfg.second ); unsigned i = 0;
	for ( unsigned tree = 0; tree < n_trees; ++tree )
	for ( unsigned d = 0; d < degrees[tree]; ++d )
	{
		// Current neighbor of 'tree'
		const unsigned n = neighbors[ strides[tree] + d ];

		// Discard doublons
		if ( n < tree ) continue;

		// Write current edge
		cx[i  ].first  = tree;
		cx[i++].second = n;
	}
}



	/********************     **********     ********************/
	/********************     **********     ********************/


