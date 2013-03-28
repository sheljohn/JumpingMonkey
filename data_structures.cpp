#include "data_structures.h"



/**
 * [SMCSIndexer::ind2sub Convert plain index to subindices knowing SCS.]
 * @param  n     [Size of the matrix.]
 * @param  index [Plain column-wise index.]
 * @param  i     [Output row.]
 * @param  j     [Output column.]
 */
void SMCSIndexer::ind2sub( const unsigned& n, const unsigned& index, unsigned& i, unsigned& j )
{
	// Safety check
	// if ( (n == 0) || (n*(n+1) <= 2*index) ) return;

	const double x = n + 0.5;
	j = static_cast<unsigned>( x - sqrt( x*x - (index<<1) ) );
	i = index + (( j*(j-1) )>>1) - (n-1)*j ;
}