

#include "stdafx.h"
#include "ioIdxBuffer.h"
#include "ErrorReport.h"

ioIdxBuffer::ioIdxBuffer()
{
	m_pIdxBuffer = NULL;
	m_pBuffer	 = NULL;

	m_BufType  = BT_MANAGED;
	m_BufUsage = BU_HARDWARE;
	
	m_dwLockFlags = 0;
	m_iIdxCnt	  = 0;
}

ioIdxBuffer::~ioIdxBuffer()
{
	SAFERELEASE( m_pIdxBuffer );
}

bool ioIdxBuffer::Lock( DWORD dwAddFlags, int iStartBytes, int iLockSizeBytes )
{
	if( !m_pIdxBuffer )
		return false;

	if( m_pBuffer )
	{
		LOG.PrintTimeAndLog( 0, "ioIdxBuffer::Lock - Is already Locked" );
		return true;
	}

	HRESULT	hr = m_pIdxBuffer->Lock( iStartBytes,
									 iLockSizeBytes,
									 &m_pBuffer,
									 m_dwLockFlags | dwAddFlags );

	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioIdxBuffer::Lock - FAILED(%x)", hr );
		return false;
	}

	if( !m_pBuffer )
	{
		LOG.PrintTimeAndLog( 0, "ioIdxBuffer::Lock - Success But Ptr is NULL" );
		m_pIdxBuffer->Unlock();
		return false;
	}

	return true;
}

bool ioIdxBuffer::UnLock()
{
	if( !m_pIdxBuffer )
		return true;

	if( !m_pBuffer )
	{
		LOG.PrintTimeAndLog( 0, "ioIdxBuffer::UnLock - Is Not Locked" );
		return true;
	}

	m_pBuffer = NULL;

	HRESULT	hr = m_pIdxBuffer->Unlock();
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioIdxBuffer::UnLock - FAILED(%x)", hr );
		return false;
	}

	return true;
}

void ioIdxBuffer::ReleaseIfDefaultPool()
{
	if( m_BufType == BT_DYNAMIC )
	{
		SAFERELEASE( m_pIdxBuffer );
	}
}

void ioIdxBuffer::ReCreateIfDefaultPool( IDirect3DDevice9 *pDevice )
{
	if( m_BufType == BT_DYNAMIC )
	{
		CreateIndexBuffer( pDevice, m_BufType, m_BufUsage, m_iIdxCnt );
	}
}

bool ioIdxBuffer::CreateIndexBuffer( IDirect3DDevice9 *pDevice,
									 BufferType eType,
									 BufferUsage eUsage,
									 int iIdxCnt )
{
	SAFERELEASE( m_pIdxBuffer );
	
	if( eType == BT_STATIC )
	{
		eType = BT_MANAGED;

		// Device Lost시 STATIC은 다시 생성해야되는데 이때 데이터가 날라가므로 안전한 MANGED로 교체
		LOG.PrintTimeAndLog( 0, "ioIdxBuffer::CreateIndexBuffer - Auto Type Change STATIC->MANAGED" );
	}

	m_BufType  = eType;
	m_BufUsage = eUsage;
	m_iIdxCnt = iIdxCnt;
	m_pBuffer = NULL;

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

	HRESULT hr;
	hr = pDevice->CreateIndexBuffer( m_iIdxCnt * sizeof(WORD),
									 dwUsage,
									 D3DFMT_INDEX16,
									 ePool,
									 &m_pIdxBuffer,
									 NULL );

	if( hr == D3DERR_OUTOFVIDEOMEMORY || hr == E_OUTOFMEMORY )
	{
		LOG.PrintTimeAndLog( 0, "ioIdxBuffer::CreateIndexBuffer - Evict and Retry" );

		if( SUCCEEDED( pDevice->EvictManagedResources() ) )
		{
			hr = pDevice->CreateIndexBuffer( m_iIdxCnt * sizeof(WORD),
											 dwUsage,
											 D3DFMT_INDEX16,
											 ePool,
											 &m_pIdxBuffer,
											 NULL );
		}
	}

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "CreateIndexBuffer: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioIdxBuffer::CreateIndexBuffer - FAILED(%x, %d)", hr, eType );
		return false;
	}

	return true;
}
