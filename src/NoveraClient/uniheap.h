#pragma once

/*
 *    uniheap.h
 *
 *  
 */


template < int UNIT_SIZE, short BLOCK_SIZE = 128 >
class uni_heap 
{
protected:

	enum {
		OCCUPIED	= -1
	};

	struct TBlock;

public:

	uni_heap() 
	{
		m_blocks.push_back( new TBlock() );
	}

	~uni_heap()
	{
		std::vector< TBlock* >::iterator it;
		for ( it = m_blocks.begin(); it != m_blocks.end(); it++ ) 
			delete *it;
	}

	void clear()
	{
		std::vector< TBlock* >::iterator it;
		for ( it = m_blocks.begin(); it != m_blocks.end(); it++ ) 
			delete *it;

		m_blocks.clear();
		m_blocks.push_back( new TBlock() );
	}

	void* alloc()
	{
		// search for a non-full block.
		std::vector< TBlock* >::iterator it;
		for ( it = m_blocks.begin(); it != m_blocks.end(); it++ ) {
			if ( (*it)->count == BLOCK_SIZE )
				continue;

			return alloc_from_block( *it );
		}

		// all blocks are full, so we must make a new empty block and allocate from it.
		m_blocks.push_back( new TBlock() );

		return alloc_from_block( m_blocks.back() );
	}

	void free( void* ptr )
	{
		std::vector< TBlock* >::iterator it;
		for ( it = m_blocks.begin(); it != m_blocks.end(); it++ ) {
			if ( !is_ptr_from_block( ptr, *it ) ) 
				continue;

			assert( (reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>((*it)->buf)) % UNIT_SIZE == 0 );

			free_from_block( ptr, *it );

			// delete tailing empty blocks 
			if ( it == m_blocks.end() - 1 ) {
				while ( m_blocks.back()->count == 0 && m_blocks.size() > 1 ) {
					delete m_blocks.back();
					m_blocks.pop_back();
				}
			}

			return;
		}

		// error : ptr is not valid!!!
		assert( false );
	}

	unsigned get_id_from_ptr( const void* ptr )
	{
		unsigned id = 1;
		std::vector< TBlock* >::iterator it;
		for ( it = m_blocks.begin(); it != m_blocks.end(); it++, id += BLOCK_SIZE ) {
			if ( is_ptr_from_block( ptr, *it ) ) 
				return id + ( reinterpret_cast<const char*>(ptr) - (*it)->buf ) / UNIT_SIZE;
		}
		
		return 0; // invalid ptr
	}

	void* get_ptr_from_id( unsigned id )
	{
        id--;
		if ( id > m_blocks.size() * BLOCK_SIZE )
			return NULL; // invalid id

		TBlock* block = m_blocks[ id / BLOCK_SIZE ];
		unsigned offset = id % BLOCK_SIZE;

		if ( block->freeLink[ offset ] != OCCUPIED )
			return NULL; // invalid id
				
		return block->buf + offset * UNIT_SIZE;
	}

	unsigned get_unit_count()
	{
		unsigned total_count = 0;

		std::vector< TBlock* >::iterator it;
		for ( it = m_blocks.begin(); it != m_blocks.end(); it++ ) {
			total_count += (*it)->count;
		}

		return total_count;
	}

	void* get_first_unit()
	{
		for ( m_lastBlock = m_blocks.begin(); m_lastBlock != m_blocks.end(); m_lastBlock++ ) {
			if ( (*m_lastBlock)->count == 0 )
				continue;
			m_lastIndex = get_next_occupied_index_from_block( *m_lastBlock, 0 );
			if ( m_lastIndex != -1 ) 
				return (*m_lastBlock)->buf + m_lastIndex * UNIT_SIZE;
		}

		return NULL;
	}

	void* get_next_unit()
	{
		if ( m_lastBlock == m_blocks.end() ) 
			return NULL;

		m_lastIndex = get_next_occupied_index_from_block( *m_lastBlock, m_lastIndex+1 );
		if ( m_lastIndex != -1 ) 
			return (*m_lastBlock)->buf + m_lastIndex * UNIT_SIZE;

		while ( (++m_lastBlock) != m_blocks.end() ) {
			if ( (*m_lastBlock)->count == 0 )
				continue;
			m_lastIndex = get_next_occupied_index_from_block( *m_lastBlock, 0 );
			if ( m_lastIndex != -1 ) 
				return (*m_lastBlock)->buf + m_lastIndex * UNIT_SIZE;
		}

		return NULL;
	}

protected:

	struct TBlock {
		char	buf[ BLOCK_SIZE * UNIT_SIZE ];
		short	freeLink[ BLOCK_SIZE ];
		short	firstFree;
		short	count;

		TBlock() {
			firstFree = 0;
			count = 0;
			for ( int i = 0; i < BLOCK_SIZE; i++ ) 
				freeLink[i] = i+1;
		}
	};

	std::vector< TBlock* >				m_blocks;
	typename std::vector< TBlock* >::iterator	m_lastBlock;
	int									m_lastIndex;

	bool is_ptr_from_block( const void* ptr, TBlock* block ) 
	{
		return (ptr >= block->buf) && (ptr < block->buf + BLOCK_SIZE * UNIT_SIZE);
	}

	void* alloc_from_block( TBlock* block ) 
	{
		short index = block->firstFree;
		
		assert( index >= 0 && index < BLOCK_SIZE );

		block->firstFree = block->freeLink[ index ];
		block->freeLink[ index ] = OCCUPIED;

		void* ptr = reinterpret_cast<char*>(block->buf) + index * UNIT_SIZE;

		// memory marking for debugging
		assert( (memset( ptr, 0xcd, UNIT_SIZE ), block->count < BLOCK_SIZE) );

		block->count++;
		return ptr;
	}

	void free_from_block( void* ptr, TBlock* block )
	{
		int index = (reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(block->buf)) 
					/ UNIT_SIZE;

		assert( block->freeLink[ index ] == OCCUPIED );

		if ( block->count == BLOCK_SIZE ) {
			block->firstFree = index;
			block->freeLink[ index ] = BLOCK_SIZE;
		} else {
			block->freeLink[ index ] = block->firstFree;
			block->firstFree = index;
		}
		block->count--;

		// memory marking for debugging
		assert( (memset( ptr, 0xdd, UNIT_SIZE ), block->count >= 0) );
	}

	int get_next_occupied_index_from_block( TBlock* block, int start_index )
	{
		for (; start_index < BLOCK_SIZE; start_index++ )
			if ( block->freeLink[ start_index ] == OCCUPIED )
				return start_index;
		return -1;
	}
};

// ---------------------------------------------------------------------

template < typename T, short BLOCK_SIZE = 128 >
class obj_heap 
{
public:

	~obj_heap()
	{
		T* pObj = reinterpret_cast< T* >( m_heap.get_first_unit() );
		for (; pObj != NULL; pObj = reinterpret_cast< T* >( m_heap.get_next_unit() ) ) {
			pObj->T::~T();
		}
	}

	void clear()
	{
		T* pObj = reinterpret_cast< T* >( m_heap.get_first_unit() );
		for (; pObj != NULL; pObj = reinterpret_cast< T* >( m_heap.get_next_unit() ) ) {
			pObj->T::~T();
		}

		m_heap.clear();
	}

	T* alloc()
	{
		T* pObj = reinterpret_cast< T* >( m_heap.alloc() );
		new (pObj) T();
		return pObj;
	}

	void free( T* pObj )
	{
		pObj->T::~T();
		m_heap.free( pObj );
	}

	unsigned get_id_from_ptr( const T* pObj )
	{
		return m_heap.get_id_from_ptr( pObj );
	}

	T* get_ptr_from_id( unsigned id )
	{
		return reinterpret_cast< T* >( m_heap.get_ptr_from_id( id ) );
	}

	unsigned get_unit_count()
	{
		return m_heap.get_unit_count();
	}

	T* get_first_unit()
	{
		return reinterpret_cast< T* >( m_heap.get_first_unit() );
	}

	T* get_next_unit()
	{
		return reinterpret_cast< T* >( m_heap.get_next_unit() );
	}

private:

	uni_heap< sizeof(T), BLOCK_SIZE > m_heap;
};

