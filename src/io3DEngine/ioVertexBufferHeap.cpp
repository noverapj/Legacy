

#include "stdafx.h"

#include "ioVtxBuffer.h"
#include "ioVertexBufferInstance.h"

#include "ioVertexBufferHeap.h"

#define MIN_ALLOC_SIZE sizeof(float)

ioVertexBufferHeap::ioVertexBufferHeap( ioVtxBuffer *pBuf,
									    BufferType eType,
										BufferUsage eUsage )
{
	m_pVtxBuf     = pBuf;
	
	m_BufferType  = eType;
	m_BufferUsage = eUsage;

	m_iHeapBufSize = m_pVtxBuf->GetBufSize();
	m_iRefCount    = 0;

	VChunk *pChunk = new VChunk;
	pChunk->iStart = 0;
	pChunk->iEnd   = m_iHeapBufSize;
	pChunk->pPrev = NULL;
	pChunk->pNext = NULL;
	pChunk->bUsed = FALSE;

	m_EmptyChunkList.push_back( pChunk );
}

ioVertexBufferHeap::~ioVertexBufferHeap()
{
	if( !m_UsedChunkList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "ioVertexBufferHeap(%d) - %d Chunk Not Release",
								m_BufferType, m_UsedChunkList.size() );

		UsedVChunkList::iterator iter;
		for( iter=m_UsedChunkList.begin() ; iter!=m_UsedChunkList.end() ; ++iter )
		{
			delete *iter;
		}
		m_UsedChunkList.clear();
	}

	if( m_EmptyChunkList.size() != 1 )
	{
		LOG.PrintTimeAndLog( 0, "ioVertexBufferHeap(%d) - Empty Chunk Error(%d)",
								m_BufferType, m_EmptyChunkList.size() );
	}

	EmptyChunkList::iterator iter;
	for( iter=m_EmptyChunkList.begin() ; iter!=m_EmptyChunkList.end() ; ++iter )
	{
		delete *iter;
	}
	m_EmptyChunkList.clear();

	SAFEDELETE(m_pVtxBuf);
}

ioVertexBufferInstance* ioVertexBufferHeap::CreateInstance( DWORD dwMask, int iVtxStride, int iVtxCnt )
{
	int iStartOffset = 0;
	if( !AllocNewChunk( iVtxStride, iVtxCnt, &iStartOffset ) )
		return NULL;

	ioVertexBufferInstance *pInstance = new ioVertexBufferInstance( this,
																	m_pVtxBuf,
																	dwMask,
																	iStartOffset,
																	iVtxStride,
																	iVtxCnt );
	m_iRefCount++;

	return pInstance;
}

bool ioVertexBufferHeap::AllocNewChunk( int iVtxStride, int iVtxCnt, int *pStartOffset )
{
	VChunk *pBestChunk = FindBestChunk( iVtxStride, iVtxCnt );
	if( !pBestChunk )
	{
		LOG.PrintTimeAndLog( 0, "ioVertexBufferHeap::AllocNewChunk - OverFlow" );
		return false;
	}

	VChunk *pUsedChunk = new VChunk;
	pUsedChunk->iStart = pBestChunk->iStart;
	pUsedChunk->iEnd   = pBestChunk->iStart + iVtxStride * iVtxCnt;
	pUsedChunk->bUsed  = TRUE;

	pBestChunk->iStart += pUsedChunk->AllocSize();
	if( pBestChunk->AllocSize() < MIN_ALLOC_SIZE )	// Delete Best Chunk
	{
		pUsedChunk->pPrev = pBestChunk->pPrev;
		pUsedChunk->pNext = pBestChunk->pNext;

		if( pBestChunk->pNext )
		{
			pBestChunk->pNext->pPrev = pUsedChunk;
		}
		if( pBestChunk->pPrev )
		{
			pBestChunk->pPrev->pNext = pUsedChunk;
		}

		pUsedChunk->iEnd = pBestChunk->iEnd;

		m_EmptyChunkList.remove( pBestChunk );
		delete pBestChunk;
	}
	else	// Insert Userchunk
	{
		pUsedChunk->pPrev = pBestChunk->pPrev;
		if( pBestChunk->pPrev )
		{
			pBestChunk->pPrev->pNext = pUsedChunk;
		}

		pUsedChunk->pNext = pBestChunk;
		pBestChunk->pPrev = pUsedChunk;
	}

	m_UsedChunkList.push_back( pUsedChunk );

	*pStartOffset = pUsedChunk->iStart;

	return true;
}

int ioVertexBufferHeap::DestroyInstance( ioVertexBufferInstance *pInstance )
{
	if( !pInstance )
	{
		LOG.PrintTimeAndLog( 0, "ioVertexBufferHeap::DestroyInstance - pInstance is NULL" );
		return m_iRefCount;
	}

	DeAllocChunk( pInstance->GetOffsetBytes() );
	delete pInstance;

	return --m_iRefCount;
}

void ioVertexBufferHeap::ReleaseIfDefaultPool()
{
	if( m_pVtxBuf )
	{
		m_pVtxBuf->ReleaseIfDefaultPool();
	}
}

void ioVertexBufferHeap::ReCreateIfDefaultPool( IDirect3DDevice9 *pDevice )
{
	if( m_pVtxBuf )
	{
		m_pVtxBuf->ReCreateIfDefaultPool( pDevice );
	}
}

void ioVertexBufferHeap::DeAllocChunk( int iStartOffset )
{
	VChunk *pUsedChunk = FindUsedChunk( iStartOffset );

	VChunk *pPrev = pUsedChunk->pPrev;
	if( pPrev && !pPrev->bUsed )
	{
		pUsedChunk->iStart = pPrev->iStart;
		pUsedChunk->pPrev = pPrev->pPrev;
		if( pPrev->pPrev )
		{
			pPrev->pPrev->pNext = pUsedChunk;
		}

		m_EmptyChunkList.remove( pPrev );
		delete pPrev;
	}

	VChunk *pNext = pUsedChunk->pNext;
	if( pNext && !pNext->bUsed )
	{
		pUsedChunk->iEnd = pNext->iEnd;
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

bool ioVertexBufferHeap::IsCanAlloc( BufferType eType,
									 BufferUsage eUsage,
									 int iVtxStride,
									 int iVtxCnt )
{
	if( m_BufferType != eType || m_BufferUsage != eUsage )
		return false;

	if( !FindBestChunk( iVtxStride, iVtxCnt ) )
		return false;

	return true;
}

ioVertexBufferHeap::VChunk* ioVertexBufferHeap::FindBestChunk( int iVtxStride, int iVtxCnt )
{
	int iNeedSize = iVtxStride * iVtxCnt;

	VChunk *pChunk;
	EmptyChunkList::iterator iter;
	for( iter=m_EmptyChunkList.begin() ; iter!=m_EmptyChunkList.end() ; ++iter )
	{
		pChunk = *iter;

		if( pChunk->AllocSize() >= iNeedSize )
			return pChunk;
	}

	return NULL;
}

ioVertexBufferHeap::VChunk* ioVertexBufferHeap::FindUsedChunk( int iStartOffset )
{
	UsedVChunkList::iterator iter = std::find_if( m_UsedChunkList.begin(),
												  m_UsedChunkList.end(),
												  FindRealChunk(iStartOffset) );

	if( iter != m_UsedChunkList.end() )
		return *iter;

	LOG.PrintTimeAndLog( 0, "ioVertexBufferHeap - Cannot Find Used Chunk(%d)", iStartOffset );

	return NULL;
}

int ioVertexBufferHeap::GetTotalMemoryBytes() const
{
	return m_iHeapBufSize;
}

int ioVertexBufferHeap::GetUsedMemoryBytes() const
{
	int iUsedMemory = 0;

	UsedVChunkList::const_iterator iter;
	for( iter=m_UsedChunkList.begin() ; iter!=m_UsedChunkList.end() ; ++iter )
	{
		iUsedMemory += (*iter)->AllocSize();
	}

	return iUsedMemory;
}

int ioVertexBufferHeap::GetFreeMemoryBytes() const
{
	int iFreeMemory = 0;

	EmptyChunkList::const_iterator iter;
	for( iter=m_EmptyChunkList.begin() ; iter!=m_EmptyChunkList.end() ; ++iter )
	{
		iFreeMemory += (*iter)->AllocSize();
	}

	return iFreeMemory;
}

