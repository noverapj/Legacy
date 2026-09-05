#pragma once

#include <deque>
#include <list>
#include "mpmc_queue.h"

template <class Type> 
class MPMC_MemPooler  
{
	struct PoolElement
	{
		Type data;
		mpmc_queue::node_t node;
	};
public:
	MPMC_MemPooler(int numofBlock=0, int maximumBlock=0 ) : 
	  m_allocCount(32),
	  m_numofBlock(numofBlock),
	  m_maximumBlock(maximumBlock)
	  {
		  Create(numofBlock);
	  }
	  ~MPMC_MemPooler()	
	  {
		  Destroy();
	  }

public:
	void CreatePool(const int numofBlock, const int maximumBlock=0 )
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
	void CreatePush(Type* block)
	{
		if(m_numofBlock < m_maximumBlock)
		{
			PoolElement* memBlock = new PoolElement;
			if(memBlock)
			{
				memcpy(&memBlock->data,block,sizeof(Type));
				m_memPooler.enqueue(&memBlock->node);
			}
			m_numofBlock++;
		}
		delete block;
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
		m_numofBlock += numofBlock;
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
			PoolElement* tmpdata = CONTAINING_RECORD(pnode,PoolElement,node);
			block  = reinterpret_cast<Type*>(tmpdata);

		}
		else
		{
			if(m_numofBlock < m_maximumBlock)
			{
				int allocCount = m_maximumBlock - m_numofBlock;
				(allocCount > m_allocCount) ? Create(m_allocCount) : Create(allocCount);
				mpmc_queue::node_t* pnode = m_memPooler.dequeue();
				PoolElement* tmpdata = CONTAINING_RECORD(pnode,PoolElement,node);
				block  = reinterpret_cast<Type*>(tmpdata);
			}
		}
		return (block);
	}
protected:
	int		m_allocCount;
	int		m_numofBlock;
	int		m_maximumBlock;
	mpmc_queue m_memPooler;
};

