

#include "stdafx.h"

#include "ioIdxBuffer.h"
#include "ioIndexBufferInstance.h"

ioIndexBufferInstance::ioIndexBufferInstance( ioIndexBufferHeap *pHeap,
											  ioIdxBuffer *pIdxBuffer,
											  int iOffsetIndex,
											  int iIndexCnt )
{
	m_pParentHeap  = pHeap;

	m_pIdxBuf	   = pIdxBuffer;
	m_iOffsetIndex = iOffsetIndex;
	m_iIndexCount  = iIndexCnt;
}

ioIndexBufferInstance::~ioIndexBufferInstance()
{
}

bool ioIndexBufferInstance::Lock( DWORD dwAddFlags, int iStartBytes, int iLockSizeBytes )
{
	iStartBytes += m_iOffsetIndex * sizeof(WORD);
	if( iLockSizeBytes == 0 )
		iLockSizeBytes = m_iIndexCount * sizeof(WORD);

	if( m_pIdxBuf )
		return m_pIdxBuf->Lock( dwAddFlags, iStartBytes, iLockSizeBytes );

	return false;
}

bool ioIndexBufferInstance::UnLock()
{
	if( m_pIdxBuf )
		return m_pIdxBuf->UnLock();

	return false;
}

void* ioIndexBufferInstance::GetBuffer()
{
	if( m_pIdxBuf )
		return m_pIdxBuf->GetBuffer();

	return NULL;
}

IDirect3DIndexBuffer9* ioIndexBufferInstance::GetD3DBuffer() const
{
	if( m_pIdxBuf )
		return m_pIdxBuf->GetIdxBuffer();

	return NULL;
}
