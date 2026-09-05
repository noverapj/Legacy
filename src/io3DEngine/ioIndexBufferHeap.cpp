

#include "stdafx.h"

#include "ioIdxBuffer.h"
#include "ioIndexBufferInstance.h"

#include "ioIndexBufferHeap.h"

ioIndexBufferHeap::ioIndexBufferHeap( ioIdxBuffer *pBuf, BufferType eType, BufferUsage eUsage )
{
	m_pIdxBuf	 = pBuf;
	m_BufferType  = eType;
	m_BufferUsage = eUsage;

	m_iHeapIdxSize = pBuf->GetIdxCnt();
	m_iRefCount = 0;

	IChunk *pChunk = new IChunk;
	pChunk->iStart = 0;
	pChunk->iEnd   = m_iHeapIdxSize;
	pChunk->pPrev  = NULL;
	pChunk->pNext  = NULL;
	pChunk->bUsed  = FALSE;

	m_EmptyChunkList.push_back( pChunk );
}

ioIndexBufferHeap::~ioIndexBufferHeap()
{
	if( !m_UsedChunkList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "ioIndexBufferHeap - %d Chunk Not Release", m_UsedChunkList.size() );

		UsedIChunkList::iterator iter;
		for( iter=m_UsedChunkList.begin() ; iter!=m_UsedChunkList.end() ; ++iter )
		{
			delete *iter;
		}
		m_UsedChunkList.clear();
	}

	if( m_EmptyChunkList.size() != 1 )
	{
		LOG.PrintTimeAndLog( 0, "ioIndexBufferHeap - Empty Chunk Error(%d)", m_EmptyChunkList.size() );
	}

	EmptyIChunkList::iterator iter;
	for( iter=m_EmptyChunkList.begin() ; iter!=m_EmptyChunkList.end() ; ++iter )
	{
		delete *iter;
	}
	m_EmptyChunkList.clear();

	SAFEDELETE(m_pIdxBuf);
}

void ioIndexBufferHeap::ReleaseIfDefaultPool()
{
	if( m_pIdxBuf )
	{
		m_pIdxBuf->ReleaseIfDefaultPool();
	}
}

void ioIndexBufferHeap::ReCreateIfDefaultPool( IDirect3DDevice9 *pDevice )
{
	if( m_pIdxBuf )
	{
		m_pIdxBuf->ReCreateIfDefaultPool( pDevice );
	}
}

ioIndexBufferInstance* ioIndexBufferHeap::CreateInstance( int iIdxCnt )
{
	int iStart = -1;
	if( !AllocNewChunk( iIdxCnt, &iStart ) )
		return NULL;

	ioIndexBufferInstance *pInstance = new ioIndexBufferInstance( this,
																  m_pIdxBuf,
																  iStart,
																  iIdxCnt );

	m_iRefCount++;

	return pInstance;
}

bool ioIndexBufferHeap::AllocNewChunk( int iIdxCnt, int *pStart )
{
	IChunk *pBestChunk = FindBestChunk( iIdxCnt );
	if( !pBestChunk )
	{
		LOG.PrintTimeAndLog( 0, "ioIndexBufferHeap::AllocNewChunk - OverFlow" );
		return false;
	}

	IChunk *pUsedChunk = NULL;
	if( pBestChunk->AllocSize() == iIdxCnt )
	{
		pUsedChunk = pBestChunk;
		m_EmptyChunkList.remove( pBestChunk );
	}
	else
	{
		pUsedChunk = new IChunk;
		pUsedChunk->iStart = pBestChunk->iStart;
		pUsedChunk->iEnd   = pBestChunk->iStart + iIdxCnt;
		pBestChunk->iStart += pUsedChunk->AllocSize();

		pUsedChunk->pPrev = pBestChunk->pPrev;
		if( pBestChunk->pPrev )
		{
			pBestChunk->pPrev->pNext = pUsedChunk;
		}

		pUsedChunk->pNext = pBestChunk;
		pBestChunk->pPrev = pUsedChunk;
	}

	pUsedChunk->bUsed = TRUE;
	m_UsedChunkList.push_back( pUsedChunk );

	*pStart = pUsedChunk->iStart;

	return true;
}

int ioIndexBufferHeap::DestroyInstance( ioIndexBufferInstance *pInstance )
{
	if( !pInstance )
	{
		LOG.PrintTimeAndLog( 0, "ioIndexBufferHeap::DestroyInstance - pInstance is NULL" );
		return m_iRefCount;
	}

	DeAllocChunk( pInstance->GetOffsetIndex() );
	delete pInstance;

	return --m_iRefCount;
}

void ioIndexBufferHeap::DeAllocChunk( int iStart )
{
	IChunk *pUsedChunk = FindUsedChunk( iStart );

	IChunk *pPrev = pUsedChunk->pPrev;
	if( pPrev && !pPrev->bUsed )
	{
		pUsedChunk->iStart = pPrev->iStart;
		pUsedChunk->pPrev  = pPrev->pPrev;
		if( pPrev->pPrev )
		{
			pPrev->pPrev->pNext = pUsedChunk;
		}

		m_EmptyChunkList.remove( pPrev );
		delete pPrev;
	}

	IChunk *pNext = pUsedChunk->pNext;
	if( pNext && !pNext->bUsed )
	{
		pUsedChunk->iEnd  = pNext->iEnd;
		pUsedChunk->pNext = pNext->pNext;
		if( pNext->pNext )
		{
			pNext->pNext->pPrev = pUsedChunk;
		}

		m_EmptyChunkList.remove( pNext );
		delete pNext;
	}

	m_UsedChunkList.remove( pUsedChunk );

	pUsedChunk->bUsed = FALSE;
	m_EmptyChunkList.push_back( pUsedChunk );
}

bool ioIndexBufferHeap::IsCanAlloc( BufferType eType, BufferUsage eUsage, int iIdxCnt )
{
	if( m_BufferType != eType || m_BufferUsage != eUsage )
		return false;

	if( !FindBestChunk( iIdxCnt ) )
		return false;

	return true;
}

ioIndexBufferHeap::IChunk* ioIndexBufferHeap::FindBestChunk( int iIdxCnt )
{
	IChunk *pChunk = NULL;

	EmptyIChunkList::iterator iter;
	for( iter=m_EmptyChunkList.begin() ; iter!=m_EmptyChunkList.end() ; ++iter )
	{
		pChunk = *iter;

		if( pChunk->AllocSize() >= iIdxCnt )
			return pChunk;
	}

	return NULL;
}

ioIndexBufferHeap::IChunk* ioIndexBufferHeap::FindUsedChunk( int iStart )
{
	UsedIChunkList::iterator iter;
	iter = std::find_if( m_UsedChunkList.begin(), m_UsedChunkList.end(), FindIChunk(iStart) );

	if( iter!=m_UsedChunkList.end() )
		return *iter;

	LOG.PrintTimeAndLog( 0, "ioIndexBufferHeap - Cannot Find Used chunk(%d)", iStart );

	return NULL;
}

int ioIndexBufferHeap::GetTotalMemoryBytes() const
{
	return m_pIdxBuf->GetBufferSizeByBytes();
}

int ioIndexBufferHeap::GetAllocMemoryBytes() const
{
	int iUsedMemory = 0;

	UsedIChunkList::const_iterator iter;
	for( iter=m_UsedChunkList.begin() ; iter!=m_UsedChunkList.end() ; ++iter )
	{
		iUsedMemory += (*iter)->AllocSize();
	}

	return iUsedMemory * sizeof(WORD);
}

int ioIndexBufferHeap::GetFreeMemoryBytes() const
{
	int iFreeMemory = 0;

	EmptyIChunkList::const_iterator iter;
	for( iter=m_EmptyChunkList.begin() ; iter!=m_EmptyChunkList.end() ; ++iter )
	{
		iFreeMemory += (*iter)->AllocSize();
	}

	return iFreeMemory * sizeof(WORD);
}
