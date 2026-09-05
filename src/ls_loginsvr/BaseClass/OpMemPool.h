#pragma once


typedef boost::singleton_pool<MEM32,32,boost::default_user_allocator_new_delete,boost::details::pool::default_mutex,500>     m_block32;
typedef boost::singleton_pool<MEM64,64,boost::default_user_allocator_new_delete,boost::details::pool::default_mutex,500>     m_block64;
typedef boost::singleton_pool<MEM128,128,boost::default_user_allocator_new_delete,boost::details::pool::default_mutex,500>   m_block128;
typedef boost::singleton_pool<MEM256,256,boost::default_user_allocator_new_delete,boost::details::pool::default_mutex,500>   m_block256;
typedef boost::singleton_pool<MEM512,512,boost::default_user_allocator_new_delete,boost::details::pool::default_mutex,500>   m_block512;
typedef boost::singleton_pool<MEM1024,1024,boost::default_user_allocator_new_delete,boost::details::pool::default_mutex,100> m_block1024;
typedef boost::singleton_pool<MEM2048,2048,boost::default_user_allocator_new_delete,boost::details::pool::default_mutex,20> m_block2048;
typedef boost::singleton_pool<MEM4096,4096,boost::default_user_allocator_new_delete,boost::details::pool::default_mutex,10>  m_block4096;
 
class OpMemPool
{
public:
	OpMemPool(void);
	virtual ~OpMemPool(void);

public:
	virtual void Init();
	
public:
	int GetSize()
	{
		return m_block32::next_size+
		m_block64::next_size+
		m_block128::next_size+
		m_block256::next_size+
		m_block512::next_size+
		m_block1024::next_size+
		m_block2048::next_size+
		m_block4096::next_size;	
	}

	template<typename TYPE>
	void Push(TYPE* data)
	{
		IncrementCount();

		int dataSize = sizeof(*data);
		memset((TCHAR*)data,0,dataSize);

		if(dataSize <= 32)
			m_block32::free((void*)data);

		else if(dataSize <= 64)
			m_block64::free((void*)data);

		else if(dataSize <= 128)
			m_block128::free((void*)data);

		else if(dataSize <= 256)
			m_block256::free((void*)data);

		else if(dataSize <= 512)
			m_block512::free((void*)data);

		else if(dataSize <= 1024)
			m_block1024::free((void*)data);

		else if(dataSize <= 2048)
			m_block2048::free((void*)data);

		else if(dataSize <= 4096)
			m_block4096::free((void*)data);
	}

	void* Pop(int dataSize)
	{
		DecrementCount();

		if(dataSize <= 32)
			return m_block32::malloc();
		
		else if(dataSize <= 64)
			return m_block64::malloc();
		
		else if(dataSize <= 128)
			return m_block128::malloc();
		
		else if(dataSize <= 256)
			return m_block256::malloc();
		
		else if(dataSize <= 512)
			return m_block512::malloc();
		
		else if(dataSize <= 1024)
			return m_block1024::malloc();
		
		else if(dataSize <= 2048)
			return m_block2048::malloc();
		
		else if(dataSize <= 4096)
			return m_block4096::malloc();
		
		return NULL;
	}

protected:
	void DecrementCount();
	void IncrementCount();

};

