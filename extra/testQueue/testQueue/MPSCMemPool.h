#pragma once

#include <deque>
#include <list>
#include "cCriticalSection.h"


template <class Type> 
class MPMC_MemPooler  
{
public:
	MPMC_MemPooler(int numofBlock=0, int maximumBlock=0, BOOL threadSafe=TRUE) : 
	  m_allocCount(32),
		  m_numofBlock(0),
		  m_maximumBlock(maximumBlock),
		  m_threadSafe(threadSafe)
	  {
		  m_blockSize = sizeof(Type);

		  Create(numofBlock);
	  }
	  ~MPMC_MemPooler()	
	  {
		  Destroy();
	  }

public:
	void CreatePool(const int numofBlock, const int maximumBlock=0, BOOL threadSafe=TRUE)
	{
		m_numofBlock	= 0;
		m_maximumBlock	= maximumBlock;
		m_threadSafe	= threadSafe;

		Create(numofBlock);
	}
	void DestroyPool()
	{
		Destroy();
	}

public:
	Type* Pop()
	{
		return Get();
	}

	Type* Remove()
	{
		if(m_memPooler.empty())
			return (NULL);

		return Get();
	}

	void Push(Type* block)
	{
	 
		if(block == NULL)
			return;
		mpmc_queue::node_t * node = CONTAINING_RECORD(block,mpmc_queue::node_t,data);
		m_memPooler.enqueue(node);
	 
	}

	int GetCount()		{	return m_memPooler.size();	}
	int GetTotalCount()	{	return m_numofBlock;		}

protected:
	void Create(const int numofBlock)
	{
		if(numofBlock <= 0) return;

		BYTE *memBlock = new BYTE[m_blockSize * numofBlock];
		if(memBlock)
		{
			Type *block;
			int nSegment = 0;

			for(int i = 0 ; i < numofBlock ; i++)
			{
				block = reinterpret_cast<Type*>(reinterpret_cast<DWORD>(memBlock) + nSegment);
				new (block) Type;
				nSegment += m_blockSize;
				mpmc_queue::node_t* node = new mpmc_queue::node_t;
				node->data = block;
				m_memPooler.enqueue(node);
			}
			m_numofBlock += numofBlock;

			m_memBlocks.push_back(memBlock);
		}
	}

	void Destroy()
	{
		Lock();
		for(std::list<BYTE*>::iterator it = m_memBlocks.begin() ; it != m_memBlocks.end() ; ++it)
		{
			BYTE* memBlock = *it;
			if(memBlock)
			{
				delete []memBlock;
			}
		}
		m_memBlocks.clear();
		Unlock();
	}

	Type* Get()
	{
		Type* block = NULL;
		
		if(!m_memPooler.empty())
		{
			mpmc_queue::node_t* node = NULL;
			node = m_memPooler.dequeue();
			block = node->data;
			
		}
		else
		{
			if(m_numofBlock < m_maximumBlock)
			{
				int allocCount = m_maximumBlock - m_numofBlock;
				(allocCount > m_allocCount) ? Create(m_allocCount) : Create(allocCount);
				mpmc_queue::node_t* node = NULL;
				node = m_memPooler.dequeue();
				block = node->data;
				
			}
		}
		
		return (block);
	}

	void Lock()
	{
		if(m_threadSafe) m_cs.Lock();
	}

	void Unlock()
	{
		if(m_threadSafe) m_cs.Unlock();
	}

protected:
	int		m_allocCount;
	int		m_numofBlock;
	int		m_maximumBlock;
	int		m_blockSize;
	BOOL	m_threadSafe;
	//std::deque<Type*> m_memPooler;
	mpmc_queue m_memPooler;
	std::list<BYTE*> m_memBlocks;

	// For thread-safe.
	cCriticalSection m_cs;					
};

