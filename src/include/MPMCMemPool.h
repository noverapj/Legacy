#pragma once
#include "MPMCQueue.h"

template <class Type> 
class MPMCMemPooler  
{
	struct PoolElement
	{
		Type data;
		mpmc_queue::node_t node;
	};
public:
	MPMCMemPooler(int numofBlock=0, int maximumBlock=0 ) : 
	  m_allocCount(32),
		  m_numofBlock(numofBlock),
		  m_maximumBlock(maximumBlock)
	  {
		  Create(numofBlock);
	  }
	  ~MPMCMemPooler()	
	  {
		  Destroy();
	  }

public:
	void CreatePool(const int numofBlock, const int maximumBlock = 100 )
	{
		m_numofBlock	= 0;
		m_maximumBlock	= maximumBlock;
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
		PoolElement* tmpdata = reinterpret_cast<PoolElement*>(block);
		m_memPooler.enqueue(&tmpdata->node);
	}

	int GetCount()		{	return m_memPooler.size();	}
	int GetTotalCount()	{	return m_numofBlock;		}

protected:
	void Create(const int numofBlock)
	{
		if(numofBlock <= 0) return;
		for(int i = 0 ; i < numofBlock ; i++)
		{	
			PoolElement *memBlock = new PoolElement;
			if(memBlock)
				m_memPooler.enqueue(&memBlock->node);
		}
		InterlockedExchangeAdd(&m_numofBlock,numofBlock);
	}
	void Destroy()
	{
		mpmc_queue::node_t* pnode = m_memPooler.dequeue();
		PoolElement* tmpdata = CONTAINING_RECORD(pnode,PoolElement,node);
		while(tmpdata)
		{
			delete tmpdata;
			mpmc_queue::node_t* pnode = m_memPooler.dequeue();
			if(pnode == NULL)
				break;
			tmpdata = CONTAINING_RECORD(pnode,PoolElement,node);
		}
	}
	Type* Get()
	{
		Type* block = NULL;
		if(!m_memPooler.empty())
		{
			mpmc_queue::node_t* pnode = m_memPooler.dequeue();
			if(pnode == NULL)
			{
				return block;
			}
			PoolElement* tmpdata = CONTAINING_RECORD(pnode,PoolElement,node);
			block  = reinterpret_cast<Type*>(tmpdata);
		}
		else
		{
			if(m_numofBlock < m_maximumBlock)
			{
				long allocCount = 0;
				InterlockedExchange(&allocCount,m_maximumBlock - m_numofBlock);
				(allocCount > m_allocCount) ? Create(m_allocCount) : Create(allocCount);
				mpmc_queue::node_t* pnode = m_memPooler.dequeue();
				if(pnode == NULL)
				{
					return block;
				}
				PoolElement* tmpdata = CONTAINING_RECORD(pnode,PoolElement,node);
				block  = reinterpret_cast<Type*>(tmpdata);
			}
		}
		return (block);
	}
protected:
	long		m_allocCount;
	long		m_numofBlock;
	long		m_maximumBlock;
	mpmc_queue m_memPooler;
};

