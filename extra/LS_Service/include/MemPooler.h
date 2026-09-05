#pragma once

#include <deque>
#include "cCriticalSection.h"


template <class Type> 
class MemPooler  
{
public:
	MemPooler(int nNumofBlock=0, int nMaximumBlock=0, BOOL bThreadSafe=TRUE) : 
	  m_nAllocCount(32),
	  m_nNumofBlock(0),
	  m_nMaximumBlock(nMaximumBlock),
	  m_bThreadSafe(bThreadSafe)
	{
		m_nBlockSize = sizeof(Type);

		Create(nNumofBlock);
	}
	~MemPooler()	
	{
		Destroy();
	}

public:
	void CreatePool(const int nNumofBlock, const int nMaximumBlock=0, BOOL bThreadSafe=TRUE)
	{
		m_nNumofBlock	= 0;
		m_nMaximumBlock	= nMaximumBlock;
		m_bThreadSafe	= bThreadSafe;

		Create(nNumofBlock);
	}
	void DestroyPool()
	{
		Destroy();
	}

public:
	Type* Pop()
	{
		if((m_MemPool.empty()) && (m_nNumofBlock < m_nMaximumBlock))
		{
			int allocCount = m_nMaximumBlock - m_nNumofBlock;
			(allocCount > m_nAllocCount) ? Create(m_nAllocCount) : Create(allocCount);
		}
		
		return Get();
	}

	Type* Remove()
	{
		if(m_MemPool.empty())
			return (NULL);
	
		return Get();
	}

	void Push(Type* pBlock)
	{
		Lock();
		m_MemPool.push_back(pBlock);
		Unlock();
	}

	void Push_Front(Type* pBlock)
	{
		Lock();
		m_MemPool.push_front(pBlock);
		Unlock();
	}

	int GetCount()		{	return m_MemPool.size();	}
	int GetTotalCount()	{	return m_nNumofBlock;		}

protected:
	void Create(const int nNumofBlock)
	{
		if(nNumofBlock <= 0) return;

		BYTE *memBlock = new BYTE[m_nBlockSize * nNumofBlock];
		if(memBlock)
		{
			Type *pBlock;
			int nSegment = 0;
			for(int i = 0 ; i < nNumofBlock ; i++)
			{
				pBlock = reinterpret_cast<Type*>(reinterpret_cast<DWORD>(memBlock) + nSegment);
				new (pBlock) Type;
				Push(pBlock);

				nSegment += m_nBlockSize;
			}
			m_nNumofBlock += nNumofBlock;
		}
	}

	void Destroy()
	{
		Type* pBlock = NULL;
		while((pBlock = Remove()) != NULL)
		{
			delete pBlock;
		}
	}

	Type* Get()
	{
		Type* pBlock = NULL;
		Lock();
		if(!m_MemPool.empty())
		{
			pBlock = m_MemPool.front();
			if(pBlock)
				m_MemPool.pop_front();
		}
		Unlock();
		return (pBlock);
	}

	void Lock()
	{
		if(m_bThreadSafe) m_cs.Lock();
	}
	
	void Unlock()
	{
		if(m_bThreadSafe) m_cs.Unlock();
	}

protected:
	int		m_nAllocCount;
	int		m_nNumofBlock;
	int		m_nMaximumBlock;
	int		m_nBlockSize;
	BOOL	m_bThreadSafe;
	std::deque<Type*> m_MemPool;

	// For thread-safe.
	cCriticalSection m_cs;					
};

