

#include "stdafx.h"

#include "ioIdxBuffer.h"
#include "ioIndexBufferInstance.h"
#include "ioIndexBufferHeap.h"

#include "ioIndexBufferManager.h"
#include "ioCPU.h"
#include "ioRenderSystem.h"
#include "HelpFunc.h"

template<> ioIndexBufferManager* Singleton< ioIndexBufferManager >::ms_Singleton = 0;
BufferUsage ioIndexBufferManager::m_AutoBufferUsage = BU_HARDWARE;

ioIndexBufferManager::ioIndexBufferManager( IDirect3DDevice9 *pDevice )
{
	m_pD3DDevice = pDevice;
	m_pD3DDevice->AddRef();
	
	m_pQuadSharedIdxBuf  = NULL;
	m_pStripSharedIdxBuf = NULL;

	NotifyQuadSharedBuffer( 200 );
	NotifyStripSharedBuffer( 800 );
}

ioIndexBufferManager::~ioIndexBufferManager()
{
	if( m_pQuadSharedIdxBuf )
	{
		DestroyInstance( m_pQuadSharedIdxBuf );
		m_pQuadSharedIdxBuf = NULL;
	}

	if( m_pStripSharedIdxBuf )
	{
		DestroyInstance( m_pStripSharedIdxBuf );
		m_pStripSharedIdxBuf = NULL;
	}

	DestroyAllIndexHeap();	
	SAFERELEASE( m_pD3DDevice );
}

void ioIndexBufferManager::ReleaseDefaultPoolResources()
{
	IndexBufferHeapList::iterator iter;
	for( iter=m_IndexBufferHeapList.begin() ; iter!=m_IndexBufferHeapList.end() ; ++iter )
	{
		(*iter)->ReleaseIfDefaultPool();
	}
}

void ioIndexBufferManager::ReCreateDefaultPoolResources()
{
	IndexBufferHeapList::iterator iter;
	for( iter=m_IndexBufferHeapList.begin() ; iter!=m_IndexBufferHeapList.end() ; ++iter )
	{
		(*iter)->ReCreateIfDefaultPool( m_pD3DDevice );
	}
}

void ioIndexBufferManager::DestroyAllIndexHeap()
{
	IndexBufferHeapList::iterator iter;
	for( iter=m_IndexBufferHeapList.begin() ; iter!=m_IndexBufferHeapList.end() ; ++iter )
	{
		delete *iter;
	}
}

ioIndexBufferInstance* ioIndexBufferManager::CreateInstance( BufferType eType,
														     int iIdxCnt,
															 BufferUsage eUsage )
{
	if( eUsage == BU_AUTO )
		eUsage = m_AutoBufferUsage;

	ioIndexBufferHeap *pHeap = FindAllocEnableHeap( eType, eUsage, iIdxCnt );
	if( pHeap )
		return pHeap->CreateInstance( iIdxCnt );

	return NULL;
}

ioIndexBufferInstance* ioIndexBufferManager::CreateInstance( BufferType eType,
															 int iIdxCnt,
															 const void *pBuf,
															 DWORD dwLockFlag,
															 BufferUsage eUsage )
{
	ioIndexBufferInstance *pInstance = CreateInstance( eType, iIdxCnt, eUsage );
	if( !pInstance )	return NULL;
	
	if( pInstance->Lock( dwLockFlag ) )
	{
		ioCPU::FastMemcpy( pInstance->GetBuffer(), pBuf, sizeof(WORD) * iIdxCnt );

		pInstance->UnLock();
		return pInstance;
	}

	DestroyInstance( pInstance );
	return NULL;
}

ioIndexBufferInstance* ioIndexBufferManager::CreateExclusiveInstance( BufferType eType,
																	  int iIdxCnt,
																	  BufferUsage eUsage )
{
	if( eUsage == BU_AUTO )
		eUsage = m_AutoBufferUsage;

	ioIndexBufferHeap *pHeap = CreateNewHeap( eType, eUsage, iIdxCnt );
	if( pHeap )
		return pHeap->CreateInstance( iIdxCnt );

	return NULL;
}

void ioIndexBufferManager::DestroyInstance( ioIndexBufferInstance *pInstance )
{
	if( !pInstance )	return;

	ioIndexBufferHeap *pHeap = pInstance->GetParentHeap();
	if( pHeap->DestroyInstance( pInstance ) > 0 )
		return;

	if( m_IndexBufferHeapList.size() > 1 )
	{
		m_IndexBufferHeapList.remove( pHeap );
		delete pHeap;
	}
}

ioIndexBufferInstance* ioIndexBufferManager::NotifyQuadSharedBuffer( int iSquareCnt )
{
	int iCurIdxCnt = 0;
	if( m_pQuadSharedIdxBuf )
		iCurIdxCnt = m_pQuadSharedIdxBuf->GetIndexCount();

	if( iCurIdxCnt >= iSquareCnt * 6 )
		return m_pQuadSharedIdxBuf;

	if( m_pQuadSharedIdxBuf )
	{
		DestroyInstance( m_pQuadSharedIdxBuf );
		m_pQuadSharedIdxBuf = NULL;
	}

	m_pQuadSharedIdxBuf = CreateInstance( BT_MANAGED,
										  iSquareCnt * 6,
										  ioRenderSystem::m_NoneAutoBufferUsage );
	if( !m_pQuadSharedIdxBuf )
		return NULL;

	if( !m_pQuadSharedIdxBuf->Lock() )
	{
		DestroyInstance( m_pQuadSharedIdxBuf );
		m_pQuadSharedIdxBuf = NULL;
		return NULL;
	}

	WORD *pIdx = (WORD*)m_pQuadSharedIdxBuf->GetBuffer();

	int iFaceIdx = 0, iIdxOff = 0;
	for( int i=0 ; i<iSquareCnt ; i++ )
	{
		pIdx[iFaceIdx+0] = iIdxOff + 0;
		pIdx[iFaceIdx+1] = iIdxOff + 1;
		pIdx[iFaceIdx+2] = iIdxOff + 3;
		pIdx[iFaceIdx+3] = iIdxOff + 0;
		pIdx[iFaceIdx+4] = iIdxOff + 3;
		pIdx[iFaceIdx+5] = iIdxOff + 2;

		iFaceIdx += 6;
		iIdxOff += 4;
	}

	m_pQuadSharedIdxBuf->UnLock();

	return m_pQuadSharedIdxBuf;
}

ioIndexBufferInstance* ioIndexBufferManager::NotifyStripSharedBuffer( int iStripCnt )
{
	int iCurIdxCnt = 0;
	if( m_pStripSharedIdxBuf )
		iCurIdxCnt = m_pStripSharedIdxBuf->GetIndexCount();

	if( iCurIdxCnt >= iStripCnt )
		return m_pStripSharedIdxBuf;

	if( m_pStripSharedIdxBuf )
	{
		DestroyInstance( m_pStripSharedIdxBuf );
		m_pStripSharedIdxBuf = NULL;
	}

	m_pStripSharedIdxBuf = CreateInstance( BT_MANAGED, iStripCnt );
	if( !m_pStripSharedIdxBuf )
		return NULL;

	if( !m_pStripSharedIdxBuf->Lock() )
	{
		DestroyInstance( m_pStripSharedIdxBuf );
		m_pStripSharedIdxBuf = NULL;
		return NULL;
	}

	WORD *pIdx = (WORD*)m_pStripSharedIdxBuf->GetBuffer();
	for( int i=0 ; i<iStripCnt ; i++ )
	{
		pIdx[i] = i;
	}

	m_pStripSharedIdxBuf->UnLock();

	return m_pStripSharedIdxBuf;
}

ioIndexBufferHeap* ioIndexBufferManager::FindAllocEnableHeap( BufferType eType,
															  BufferUsage eUsage,
															  int iIdxCnt )
{
	ioIndexBufferHeap *pHeap = NULL;

	IndexBufferHeapList::iterator iter;
	for( iter=m_IndexBufferHeapList.begin() ; iter!=m_IndexBufferHeapList.end() ; ++iter )
	{
		pHeap = *iter;
		if( pHeap->IsCanAlloc( eType, eUsage, iIdxCnt ) )
			return pHeap;
	}

	return CreateNewHeap( eType, eUsage, 1024*512 );
}

ioIndexBufferHeap* ioIndexBufferManager::CreateNewHeap( BufferType eType,
														BufferUsage eUsage,
														int iIndexCnt )
{
	ioIdxBuffer *pIdxBuf = new ioIdxBuffer;
	if( !pIdxBuf->CreateIndexBuffer( m_pD3DDevice, eType, eUsage, iIndexCnt ) )
		return NULL;

	ioIndexBufferHeap *pHeap = new ioIndexBufferHeap( pIdxBuf, eType, eUsage );
	m_IndexBufferHeapList.push_back( pHeap );

	return pHeap;
}

int ioIndexBufferManager::GetTotalHeapCount() const
{
	return m_IndexBufferHeapList.size();
}

int ioIndexBufferManager::GetTotalHeapMemory() const
{
	int iTotalMemory = 0;

	IndexBufferHeapList::const_iterator iter;
	for( iter=m_IndexBufferHeapList.begin() ; iter!=m_IndexBufferHeapList.end() ; ++iter )
	{
		iTotalMemory += (*iter)->GetTotalMemoryBytes();
	}

	return iTotalMemory;
}

int ioIndexBufferManager::GetTotalHeapUsedMemory() const
{
	int iUsedMemory = 0;

	IndexBufferHeapList::const_iterator iter;
	for( iter=m_IndexBufferHeapList.begin() ; iter!=m_IndexBufferHeapList.end() ; ++iter )
	{
		iUsedMemory += (*iter)->GetAllocMemoryBytes();
	}

	return iUsedMemory;
}

int ioIndexBufferManager::GetTotalHeapFreeMemory() const
{
	int iFreeMemory = 0;

	IndexBufferHeapList::const_iterator iter;
	for( iter=m_IndexBufferHeapList.begin() ; iter!=m_IndexBufferHeapList.end() ; ++iter )
	{
		iFreeMemory += (*iter)->GetFreeMemoryBytes();
	}

	return iFreeMemory;
}

int ioIndexBufferManager::GetHeapTotalMemory( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetTotalHeapCount() ) )
	{
		IndexBufferHeapList::const_iterator iter = m_IndexBufferHeapList.begin();
		std::advance( iter, iIndex );

		return (*iter)->GetTotalMemoryBytes();
	}

	return 0;
}

int ioIndexBufferManager::GetHeapUsedMemory( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetTotalHeapCount() ) )
	{
		IndexBufferHeapList::const_iterator iter = m_IndexBufferHeapList.begin();
		std::advance( iter, iIndex );

		return (*iter)->GetAllocMemoryBytes();
	}

	return 0;
}

int ioIndexBufferManager::GetHeapFreeMemory( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetTotalHeapCount() ) )
	{
		IndexBufferHeapList::const_iterator iter = m_IndexBufferHeapList.begin();
		std::advance( iter, iIndex );

		return (*iter)->GetFreeMemoryBytes();
	}

	return 0;
}

ioIndexBufferManager& ioIndexBufferManager::GetSingleton()
{
	return Singleton< ioIndexBufferManager >::GetSingleton();
}

void ioIndexBufferManager::SetAutoBufferUsage( BufferUsage eUsage )
{
	m_AutoBufferUsage = eUsage;
}