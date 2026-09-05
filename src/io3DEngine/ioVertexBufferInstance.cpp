


#include "stdafx.h"

#include "ioVtxBuffer.h"
#include "ioVertexBufferHeap.h"
#include "ioVertexBufferInstance.h"

ioVertexBufferInstance::ioVertexBufferInstance( ioVertexBufferHeap *pHeap,
											    ioVtxBuffer *pVtxBuf,
												DWORD dwMask,
												int iOffsetBytes,
												int iVtxStride,
												int iVtxCnt )
{
	m_pParentHeap = pHeap;

	m_pVtxBuf       = pVtxBuf;
	m_dwMask		= dwMask;
	m_iOffsetBytes  = iOffsetBytes;
	m_iVertexStride = iVtxStride;
	m_iVertexCount  = iVtxCnt;

	m_iExtraOffset = 0;
}

ioVertexBufferInstance::~ioVertexBufferInstance()
{
}

bool ioVertexBufferInstance::Lock( DWORD dwAddFlags , int iStartBytes, int iLockSizeBytes )
{
	if( iLockSizeBytes == 0 )
	{
		iLockSizeBytes = m_iVertexStride * m_iVertexCount;
	}

	if( m_pVtxBuf )
		return m_pVtxBuf->Lock( dwAddFlags, iStartBytes + m_iOffsetBytes, iLockSizeBytes );

	return false;
}

bool ioVertexBufferInstance::UnLock()
{
	if( m_pVtxBuf )
		return m_pVtxBuf->UnLock();

	return false;
}

void* ioVertexBufferInstance::GetBuffer()
{
	if( m_pVtxBuf )
		return m_pVtxBuf->GetBuffer();

	return NULL;
}

IDirect3DVertexBuffer9* ioVertexBufferInstance::GetD3DBuffer() const
{
	if( m_pVtxBuf )
		return m_pVtxBuf->GetVtxBuffer();

	return NULL;
}

