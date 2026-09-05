#pragma once


template< typename T, typename Pred >
class PriorityQueue
{
public:

	PriorityQueue( Pred pred ) : m_pred(pred) {}

	void push( const T& node )
	{
		m_heap.push_back( node );
		std::push_heap( m_heap.begin(), m_heap.end(), m_pred );
	}

	T& front()						{ return m_heap.front(); }
	const T& front() const			{ return m_heap.front(); }

	bool pop()
	{
		if ( m_heap.empty() )
			return false;

		std::pop_heap( m_heap.begin(), m_heap.end(), m_pred );
		m_heap.pop_back();
		return true;
	}

	bool pop( T& node )
	{
		if ( m_heap.empty() )
			return false;

		node = m_heap.front();
		std::pop_heap( m_heap.begin(), m_heap.end(), m_pred );
		m_heap.pop_back();
		return true;
	}

	void adjust( const T& node )
	{
		std::vector<T>::iterator it;
		for ( it = m_heap.begin(); it != m_heap.end(); it++ )
		{
			if( (*it) == node )
			{
				std::push_heap( m_heap.begin(), it+1, m_pred );
				return;
			}
		}
	}

	bool empty() const				{ return m_heap.empty(); }
	size_t size() const				{ return m_heap.size(); }

	void clear()					{ m_heap.clear(); }

	// iterators

	typedef typename std::vector< T >::iterator		  iterator;
	typedef typename std::vector< T >::const_iterator const_iterator;

	iterator begin()				{ return m_heap.begin(); }
	iterator end()					{ return m_heap.end(); }
	const_iterator begin() const	{ return m_heap.begin(); }
	const_iterator end() const		{ return m_heap.end(); }

private:

	Pred m_pred;

	std::vector< T > m_heap;
};

