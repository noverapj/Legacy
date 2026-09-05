

#include "stdafx.h"

#include "HelpFunc.h"
#include "ioVtxBuffer.h"
#include "ErrorReport.h"

ioVtxBuffer::ioVtxBuffer()
{
	m_pVtxBuffer  = NULL;
	m_pBuffer	  = NULL;

	m_BufType   = BT_MANAGED;
	m_BufUsage	= BU_HARDWARE;

	m_dwLockFlags = 0;
	m_iBufSize  = 0;
}

ioVtxBuffer::~ioVtxBuffer()
{
	SAFERELEASE( m_pVtxBuffer );
}

bool ioVtxBuffer::Lock( DWORD dwAddFlags, int iStartBytes, int iLockSizeBytes )
{
	if( !m_pVtxBuffer )
		return false;

	if( m_pBuffer )
	{
		LOG.PrintTimeAndLog( 0, "ioVtxBuffer::Lock - Is Already Locked" );
		return true;
	}

	HRESULT	hr = m_pVtxBuffer->Lock( iStartBytes,
									 iLockSizeBytes,
									 &m_pBuffer,
									 m_dwLockFlags | dwAddFlags );

	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioVtxBuffer::Lock - FAILED(%x)", hr );
		return false;
	}

	return true;
}

bool ioVtxBuffer::UnLock()
{
	if( !m_pVtxBuffer )
		return true;

	if( !m_pBuffer )
	{
		LOG.PrintTimeAndLog( 0, "ioVtxBuffer::UnLock - Is Not Locked" );
		return true;
	}

	m_pBuffer = NULL;
	
	HRESULT	hr = m_pVtxBuffer->Unlock();
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioVtxBuffer::UnLock - FAILED(%x)", hr );
		return false;
	}

	return true;
}

void ioVtxBuffer::ReleaseIfDefaultPool()
{
	if( m_BufType == BT_DYNAMIC )
	{
		SAFERELEASE( m_pVtxBuffer );
	}
}

void ioVtxBuffer::ReCreateIfDefaultPool( IDirect3DDevice9 *pDevice )
{
	if( m_BufType == BT_DYNAMIC )
	{
		CreateVertexBuffer( pDevice, m_BufType, m_BufUsage, m_iBufSize );
	}
}

bool ioVtxBuffer::CreateVertexBuffer( IDirect3DDevice9 *pDevice,
									  BufferType eType,
									  BufferUsage eUsage,
									  int iBufferSize )
{
	SAFERELEASE( m_pVtxBuffer );

	if( eType == BT_STATIC )
	{
		eType = BT_MANAGED;

		// Device Lost시 STATIC은 다시 생성해야되는데 이때 데이터가 날라가므로 안전한 MANGED로 교체
		LOG.PrintTimeAndLog( 0, "ioVtxBuffer::CreateVertexBuffer - Auto Type Change STATIC->MANAGED" );
	}

	m_BufType  = eType;
	m_BufUsage = eUsage;
	m_iBufSize = iBufferSize;

	DWORD dwUsage = D3DUSAGE_WRITEONLY;
	D3DPOOL ePool = D3DPOOL_DEFAULT;

	switch( eType )
	{
	case BT_MANAGED:
		ePool = D3DPOOL_MANAGED;
		break;
	case BT_DYNAMIC:
		dwUsage |= D3DUSAGE_DYNAMIC;
		break;
	case BT_SYSTEM:
		ePool = D3DPOOL_SYSTEMMEM;
		break;
	}

	if( eUsage == BU_SOFTWARE )
	{
		dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;
	}

	HRESULT	hr;
	hr = pDevice->CreateVertexBuffer( m_iBufSize, 
									  dwUsage,
									  0,
									  ePool,
									  &m_pVtxBuffer,
									  NULL );

	if( hr == D3DERR_OUTOFVIDEOMEMORY || hr == E_OUTOFMEMORY )
	{
		LOG.PrintTimeAndLog( 0, "ioVtxBuffer::CreateVertexBuffer - Evict and Retry" );

		if( SUCCEEDED( pDevice->EvictManagedResources() ) )
		{
			hr = pDevice->CreateVertexBuffer( m_iBufSize, 
											  dwUsage,
											  0,
											  ePool,
											  &m_pVtxBuffer,
											  NULL );
		}
	}

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "CreateVertexBuffer: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioVtxBuffer::CreateVertexBuffer - FAILED(%x, %d)", hr, eType );
		return false;
	}

	return true;
}
