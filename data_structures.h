#ifndef __DATA_STRUCTURES__
#define __DATA_STRUCTURES__

#include <cmath>
#include <utility>
#include <valarray>



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * Simple structure to store valarray boundaries.
 */
template <class T>
class valarray_bounds
{
public:

	// Ctor/dtor
	valarray_bounds() { clear(); }
	valarray_bounds( std::valarray<T>& data ) { bind(data); }
	~valarray_bounds() { clear(); }

	// Clear member data.
	void clear() { first = last = nullptr; }

	// Is iterator ready?
	operator bool() const { return first && last; }

	// Bind to an existing valarray.
	bool bind( std::valarray<T>& data ) 
	{ 
		// Input should not be empty
		if ( data.size() == 0 ) 
			{ clear(); return false; }

		// Set boundaries
		first = &data[0]; 
		last  = first + data.size(); 

		// Report success
		return true;
	}

	// Begin/end
	inline T* begin() { return first; }
	inline T* end() { return last; }

private:

	T *first, *last;
};



	/********************     **********     ********************/
	/********************     **********     ********************/



/**
 * NOTE: Implementations below are fast but unsafe.
 * Make sure you know what inputs you're feeding to these methods...
 */



/**
 * Indexer for Symmetric Matrices with Column Storage.
 */
class SMCSIndexer
{
public:

	// Matrix subindices
	typedef std::pair<unsigned,unsigned> sub_type;

	/********************     **********     ********************/

	// Get subindices from plain index
	void ind2sub( const unsigned& n, const unsigned& index, unsigned& i, unsigned& j );
	inline void ind2sub( const unsigned& n, const unsigned& index, sub_type& sub )
		{ ind2sub( n, index, sub.first, sub.second ); }

	/********************     **********     ********************/

	// Get plain index from subindices
	inline unsigned sub2ind( const unsigned& n, unsigned i, unsigned j )
		{ return i >= j ? offset(n,j) + (i-j) : offset(n,i) + (j-i); }
	inline unsigned sub2ind( const unsigned& n, const sub_type& sub )
		{ return sub2ind( n, sub.first, sub.second ); }

	/********************     **********     ********************/

private:

	inline unsigned offset( const unsigned& n, const unsigned& j ) 
		{ return ( j*((n<<1)-j+1) ) >> 1; }

};



#endif