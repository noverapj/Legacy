

#include "stdafx.h"

#include "ioVertexDeclaration.h"
#include "ioVtxBuffer.h"
#include "ioVertexBufferInstance.h"
#include "ioVertexBufferHeap.h"
#include "ioVertexBufferManager.h"
#include "ioMeshData.h"

#include "ioCPU.h"
#include "HelpFunc.h"
#include "ErrorReport.h"

#define MAX_VERTEX_ELEMENT 14

template<> ioVertexBufferManager* Singleton< ioVertexBufferManager >::ms_Singleton = 0;
BufferUsage ioVertexBufferManager::m_AutoBufferUsage = BU_HARDWARE;

ioVertexBufferManager::ioVertexBufferManager( IDirect3DDevice9 *pDevice )
{
	m_pD3DDevice = pDevice;
	m_pD3DDevice->AddRef();

	m_pShareVtxTexData  = NULL;
	m_pShareXZPlaneData = NULL;
}

ioVertexBufferManager::~ioVertexBufferManager()
{
	if( m_pShareXZPlaneData )
	{
		m_pShareXZPlaneData->DeleteAllBuffers();
		SAFEDELETE( m_pShareXZPlaneData );
	}

	if( m_pShareVtxTexData )
	{
		m_pShareVtxTexData->DeleteAllBuffers();
		SAFEDELETE(m_pShareVtxTexData);
	}

	DestroyAllVertexDeclaration();
	DestroyAllVertexBufferHeap();

	SAFERELEASE(m_pD3DDevice);
}

void ioVertexBufferManager::ReleaseDefaultPoolResources()
{
	VertexBufferHeapList::iterator iter;
	for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		(*iter)->ReleaseIfDefaultPool();
	}
}

void ioVertexBufferManager::ReCreateDefaultPoolResources()
{
	if( m_pD3DDevice )
	{
		VertexBufferHeapList::iterator iter;
		for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
		{
			(*iter)->ReCreateIfDefaultPool( m_pD3DDevice );
		}
	}
}

void ioVertexBufferManager::DestroyAllVertexBufferHeap()
{
	VertexBufferHeapList::iterator iter;
	for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		delete *iter;
	}
	m_VertexBufferHeapList.clear();
}

ioVertexBufferInstance* ioVertexBufferManager::CreateInstance( BufferType eType,
															   DWORD dwMask,
															   int iVtxCnt,
															   BufferUsage eUsage )
{
	if( eUsage == BU_AUTO )
		eUsage = m_AutoBufferUsage;

	int iVtxStride = CalcVertexStride( dwMask );

	ioVertexBufferHeap *pHeap = FindAllocEnableHeap( eType, eUsage, iVtxStride, iVtxCnt );
	if( !pHeap )
	{
		pHeap = CreateNewHeap( eType, eUsage, 1024 * 1024 );
	}

	if( pHeap )
		return pHeap->CreateInstance( dwMask, iVtxStride, iVtxCnt );

	return NULL;
}

ioVertexBufferInstance* ioVertexBufferManager::CreateInstance( BufferType bType,
															   DWORD dwMask,
															   int iVtxCnt,
															   void *pBuf,
															   DWORD dwLockFlags,
															   BufferUsage eUsage )
{
	ioVertexBufferInstance *pInstance = CreateInstance( bType, dwMask, iVtxCnt, eUsage );
	if( !pInstance )	return NULL;

	if( pInstance->Lock( dwLockFlags ) )
	{
		ioCPU::FastMemcpy( pInstance->GetBuffer(), pBuf, pInstance->GetVertexStride() * iVtxCnt );

		pInstance->UnLock();
		return pInstance;
	}

	DestroyInstance( pInstance );
	return NULL;
}

ioVertexBufferInstance* ioVertexBufferManager::CreateExclusiveInstance( BufferType bType,
																		DWORD dwMask,
																		int iVtxCnt,
																		BufferUsage eUsage )
{
	if( eUsage == BU_AUTO )
		eUsage = m_AutoBufferUsage;

	int iVtxStride = CalcVertexStride( dwMask );

	ioVertexBufferHeap *pHeap = CreateNewHeap( bType, eUsage, iVtxCnt * iVtxStride );
	if( pHeap )
		return pHeap->CreateInstance( dwMask, iVtxStride, iVtxCnt );

	return NULL;
}

ioVertexBufferInstance* ioVertexBufferManager::CreateExclusiveInstance( BufferType bType,
																		DWORD dwMask,
																		int iVtxCnt,
																		void *pBuf,
																		DWORD dwLockFlags,
																		BufferUsage eUsage )
{
	ioVertexBufferInstance *pInstance = CreateExclusiveInstance( bType, dwMask, iVtxCnt, eUsage );
	if( !pInstance )	return NULL;

	int iVtxStride = CalcVertexStride( dwMask );
	if( pInstance->Lock( dwLockFlags ) )
	{
		ioCPU::FastMemcpy( pInstance->GetBuffer(), pBuf, iVtxCnt * iVtxStride );

		pInstance->UnLock();
		return pInstance;
	}

	DestroyInstance( pInstance );
	return NULL;
}

void ioVertexBufferManager::DestroyInstance( ioVertexBufferInstance *pInstance )
{
	if( !pInstance )	return;

	ioVertexBufferHeap *pHeap = pInstance->GetParentHeap();
	if( pHeap->DestroyInstance( pInstance ) > 0 )
		return;

	if( m_VertexBufferHeapList.size() > 1 )
	{
		m_VertexBufferHeapList.remove( pHeap );
		delete pHeap;
	}
}

ioVertexBufferHeap* ioVertexBufferManager::FindAllocEnableHeap( BufferType eType,
															    BufferUsage eUsage,
																int iVtxStride,
																int iVtxCnt )
{
	VertexBufferHeapList::iterator iter=m_VertexBufferHeapList.begin();
	for( ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		ioVertexBufferHeap *pHeap = *iter;
		if( pHeap->IsCanAlloc( eType, eUsage, iVtxStride, iVtxCnt ) )
			return pHeap;
	}

	return NULL;
}

ioVertexBufferHeap* ioVertexBufferManager::CreateNewHeap( BufferType eType,
														  BufferUsage eUsage,
														  int iBufferSize )
{
	ioVtxBuffer *pVtxBuf = new ioVtxBuffer;
	if( !pVtxBuf->CreateVertexBuffer( m_pD3DDevice, eType, eUsage, iBufferSize ) )
		return NULL;

	ioVertexBufferHeap *pHeap = new ioVertexBufferHeap( pVtxBuf, eType, eUsage );
	m_VertexBufferHeapList.push_back( pHeap );

	return pHeap;
}

IDirect3DVertexDeclaration9* ioVertexBufferManager::GetD3DVertexDeclaration( const ioVertexDeclaration &kDecl )
{
	VertexDeclarationList::iterator iter;
	for( iter=m_VertexDeclarationList.begin() ; iter!=m_VertexDeclarationList.end() ; ++iter )
	{
		if( *iter == kDecl )
			return iter->GetD3DDeclaration();
	}

	// Create New Declaration
	ioVertexDeclaration kNewDeclaration( kDecl );

	IDirect3DVertexDeclaration9 *pVertexDecl = CreateNewVertexDeclaration( kNewDeclaration );
	if( pVertexDecl )
	{
		kNewDeclaration.SetD3DDeclaration( pVertexDecl );
		m_VertexDeclarationList.push_back( kNewDeclaration );
	}

	return pVertexDecl;
}

IDirect3DVertexDeclaration9* ioVertexBufferManager::CreateNewVertexDeclaration( const ioVertexDeclaration &kDecl )
{
	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];

	int iCurElement = 0;
	int iMaxStream = kDecl.GetMaxStream();
	for( int iStream=0 ; iStream<=iMaxStream ; iStream++ )
	{
		WORD wCurOffset = 0;
		DWORD dwCurStreamMask = kDecl.GetStreamMask( iStream );
		for( int i=0 ; i<MAX_VERTEX_ELEMENT ; i++ )
		{
			DWORD dwMask = 1<<i;

			if( dwCurStreamMask & dwMask )
			{
				decl[ iCurElement ].Stream = (WORD)iStream;
				decl[ iCurElement ].Offset = wCurOffset;
				decl[ iCurElement ].Method = D3DDECLMETHOD_DEFAULT;

				switch( dwMask )
				{
				case IOFVF_POSITION:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT3;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_POSITION;
					decl[ iCurElement ].UsageIndex	= 0;
					wCurOffset += 3 * sizeof(float);
					break;
				case IOFVF_POSITION2:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT3;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_POSITION;
					decl[ iCurElement ].UsageIndex	= 1;
					wCurOffset += 3 * sizeof(float);
					break;
				case IOFVF_POSITIONW:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT4;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_POSITIONT;
					decl[ iCurElement ].UsageIndex	= 0;
					wCurOffset += 4 * sizeof(float);
					break;
				case IOFVF_WEIGHTS:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT4;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_BLENDWEIGHT;
					decl[ iCurElement ].UsageIndex	= 0;
					wCurOffset += 4 * sizeof(float);
					break;
				case IOFVF_INDICES:
					decl[ iCurElement ].Type		= D3DDECLTYPE_D3DCOLOR;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_BLENDINDICES;
					decl[ iCurElement ].UsageIndex	= 0;
					wCurOffset += sizeof(DWORD);
					break;
				case IOFVF_NORMAL:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT3;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_NORMAL;
					decl[ iCurElement ].UsageIndex	= 0;
					wCurOffset += 3 * sizeof(float);
					break;
				case IOFVF_COLOR0:
					decl[ iCurElement ].Type		= D3DDECLTYPE_D3DCOLOR;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_COLOR;
					decl[ iCurElement ].UsageIndex	= 0;
					wCurOffset += sizeof(DWORD);
					break;
				case IOFVF_COLOR1:
					decl[ iCurElement ].Type		= D3DDECLTYPE_D3DCOLOR;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_COLOR;
					decl[ iCurElement ].UsageIndex	= 1;
					wCurOffset += sizeof(DWORD);
					break;
				case IOFVF_UV0:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT2;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_TEXCOORD;
					decl[ iCurElement ].UsageIndex	= 0;
					wCurOffset += 2 * sizeof(float);
					break;
				case IOFVF_UV1:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT2;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_TEXCOORD;
					decl[ iCurElement ].UsageIndex	= 1;
					wCurOffset += 2 * sizeof(float);
					break;
				case IOFVF_UV2:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT2;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_TEXCOORD;
					decl[ iCurElement ].UsageIndex	= 2;
					wCurOffset += 2 * sizeof(float);
					break;
				case IOFVF_UV3:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT2;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_TEXCOORD;
					decl[ iCurElement ].UsageIndex	= 3;
					wCurOffset += 2 * sizeof(float);
					break;
				case IOFVF_TANGENT:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT3;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_TANGENT;
					decl[ iCurElement ].UsageIndex	= 0;
					wCurOffset += 3 * sizeof(float);
					break;
				case IOFVF_BINORMAL:
					decl[ iCurElement ].Type		= D3DDECLTYPE_FLOAT3;
					decl[ iCurElement ].Usage		= D3DDECLUSAGE_BINORMAL;
					decl[ iCurElement ].UsageIndex	= 0;
					wCurOffset += 3 * sizeof(float);
					break;
				default:
					LOG.PrintTimeAndLog( 0, "ioVertexBufferManager::CreateVertexDeclaration - Unknown Vertex Component" );
					break;
				}
				iCurElement++;
			}
		}
	}

	decl[ iCurElement ].Stream = 0xff;
	decl[ iCurElement ].Offset = 0;
	decl[ iCurElement ].Type   = D3DDECLTYPE_UNUSED;
	decl[ iCurElement ].Method = 0;
	decl[ iCurElement ].Usage  = 0;
	decl[ iCurElement ].UsageIndex = 0;

	IDirect3DVertexDeclaration9 *pVertexDecl = NULL;
	HRESULT hr = m_pD3DDevice->CreateVertexDeclaration( decl, &pVertexDecl );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "CreateNewVertexDeclaration1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioVertexBufferManager::CreateNewVertexDeclaration - FAILED(%x)", hr );
	}

	return pVertexDecl;
}

void ioVertexBufferManager::DestroyAllVertexDeclaration()
{
	VertexDeclarationList::iterator iter;
	for( iter=m_VertexDeclarationList.begin() ; iter!=m_VertexDeclarationList.end() ; ++iter )
	{
		IDirect3DVertexDeclaration9 *pDecl = iter->GetD3DDeclaration();
		SAFERELEASE(pDecl);
	}

	m_VertexDeclarationList.clear();
}

ioMeshData* ioVertexBufferManager::GetShareVtxTexData()
{
	if( m_pShareVtxTexData )
		return m_pShareVtxTexData;

	m_pShareVtxTexData = new ioMeshData;
	m_pShareVtxTexData->InsertStreamSource( 0, IOFVF_POSITION | IOFVF_UV0 );

	ioVtxTex kVtxBuffer[4];
	FillVtxRectFan( kVtxBuffer );

	ioVertexBufferInstance *pInstance = CreateExclusiveInstance( BT_MANAGED,
																 IOFVF_POSITION | IOFVF_UV0,
																 4,
																 kVtxBuffer );

	if( !pInstance )
	{
		m_pShareVtxTexData->DeleteAllBuffers();
		SAFEDELETE(m_pShareVtxTexData);
		return NULL;
	}

	m_pShareVtxTexData->SetBinding( 0, pInstance );
	m_pShareVtxTexData->m_iVertexCount = 4;

	return m_pShareVtxTexData;
}

ioMeshData* ioVertexBufferManager::GetShareXZPlaneData()
{
	if( m_pShareXZPlaneData )
		return m_pShareXZPlaneData;

	m_pShareXZPlaneData = new ioMeshData;
	m_pShareXZPlaneData->InsertStreamSource( 0, IOFVF_POSITION | IOFVF_UV0 );

	
/*
	2-----3
	| \   |
	|  \  |
	|   \ |
	1-----0
*/
	// Fan
	ioVtxTex kVtxBuffer[4];
	kVtxBuffer[0].vPos.x = 1.0f;
	kVtxBuffer[0].vPos.y = 0.0f;
	kVtxBuffer[0].vPos.z = -1.0f;
	kVtxBuffer[0].u = 1.0f;
	kVtxBuffer[0].v = 1.0f;

	kVtxBuffer[1].vPos.x = -1.0f;
	kVtxBuffer[1].vPos.y = 0.0f;
	kVtxBuffer[1].vPos.z = -1.0f;
	kVtxBuffer[1].u = 0.0f;
	kVtxBuffer[1].v = 1.0f;

	kVtxBuffer[2].vPos.x = -1.0f;
	kVtxBuffer[2].vPos.y = 0.0f;
	kVtxBuffer[2].vPos.z = 1.0f;
	kVtxBuffer[2].u = 0.0f;
	kVtxBuffer[2].v = 0.0f;

	kVtxBuffer[3].vPos.x = 1.0f;
	kVtxBuffer[3].vPos.y = 0.0f;
	kVtxBuffer[3].vPos.z = 1.0f;
	kVtxBuffer[3].u = 1.0f;
	kVtxBuffer[3].v = 0.0f;

	ioVertexBufferInstance *pInstance = CreateExclusiveInstance( BT_MANAGED,
																 IOFVF_POSITION | IOFVF_UV0,
																 4,
																 kVtxBuffer );
	if( !pInstance )
	{
		m_pShareXZPlaneData->DeleteAllBuffers();
		SAFEDELETE(m_pShareXZPlaneData);
		return NULL;
	}

	m_pShareXZPlaneData->SetBinding( 0, pInstance );
	m_pShareXZPlaneData->m_iVertexCount = 4;

	return m_pShareXZPlaneData;
}

int ioVertexBufferManager::GetTotalHeapCount() const
{
	return m_VertexBufferHeapList.size();
}

int ioVertexBufferManager::GetTotalHeapMemory() const
{
	int iTotalMemory = 0;

	VertexBufferHeapList::const_iterator iter;
	for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		iTotalMemory += (*iter)->GetTotalMemoryBytes();
	}

	return iTotalMemory;
}

int ioVertexBufferManager::GetTotalHeapUsedMemory() const
{
	int iUsedMemory = 0;

	VertexBufferHeapList::const_iterator iter;
	for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		iUsedMemory += (*iter)->GetUsedMemoryBytes();
	}

	return iUsedMemory;
}

int ioVertexBufferManager::GetTotalHeapFreeMemory() const
{
	int iFreeMemory = 0;

	VertexBufferHeapList::const_iterator iter;
	for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		iFreeMemory += (*iter)->GetFreeMemoryBytes();
	}

	return iFreeMemory;
}

int ioVertexBufferManager::GetTotalHeapCount( BufferType eType ) const
{
	int iTotalCount = 0;

	VertexBufferHeapList::const_iterator iter;
	for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		ioVertexBufferHeap *pHeap = *iter;
		if( pHeap->GetBufferType() == eType )
		{
			iTotalCount++;
		}
	}

	return iTotalCount;
}

int ioVertexBufferManager::GetTotalHeapMemory( BufferType eType ) const
{
	int iTotalMemory = 0;

	VertexBufferHeapList::const_iterator iter;
	for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		ioVertexBufferHeap *pHeap = *iter;
		if( pHeap->GetBufferType() == eType )
		{
			iTotalMemory += pHeap->GetTotalMemoryBytes();
		}
	}

	return iTotalMemory;
}

int ioVertexBufferManager::GetTotalHeapUsedMemory( BufferType eType ) const
{
	int iTotalMemory = 0;

	VertexBufferHeapList::const_iterator iter;
	for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		ioVertexBufferHeap *pHeap = *iter;
		if( pHeap->GetBufferType() == eType )
		{
			iTotalMemory += pHeap->GetUsedMemoryBytes();
		}
	}

	return iTotalMemory;
}

int ioVertexBufferManager::GetTotalHeapFreeMemory( BufferType eType ) const
{
	int iTotalMemory = 0;

	VertexBufferHeapList::const_iterator iter;
	for( iter=m_VertexBufferHeapList.begin() ; iter!=m_VertexBufferHeapList.end() ; ++iter )
	{
		ioVertexBufferHeap *pHeap = *iter;
		if( pHeap->GetBufferType() == eType )
		{
			iTotalMemory += pHeap->GetFreeMemoryBytes();
		}
	}

	return iTotalMemory;
}

int ioVertexBufferManager::GetHeapTotalMemory( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetTotalHeapCount() ) )
	{
		VertexBufferHeapList::const_iterator iter = m_VertexBufferHeapList.begin();
		std::advance( iter, iIndex );

		return (*iter)->GetTotalMemoryBytes();
	}

	return 0;
}

int ioVertexBufferManager::GetHeapUsedMemory( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetTotalHeapCount() ) )
	{
		VertexBufferHeapList::const_iterator iter = m_VertexBufferHeapList.begin();
		std::advance( iter, iIndex );

		return (*iter)->GetUsedMemoryBytes();
	}

	return 0;
}

int ioVertexBufferManager::GetHeapFreeMemory( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetTotalHeapCount() ) )
	{
		VertexBufferHeapList::const_iterator iter = m_VertexBufferHeapList.begin();
		std::advance( iter, iIndex );

		return (*iter)->GetFreeMemoryBytes();
	}

	return 0;
}

ioVertexBufferManager& ioVertexBufferManager::GetSingleton()
{
	return Singleton< ioVertexBufferManager >::GetSingleton();
}

void ioVertexBufferManager::SetAutoBufferUsage( BufferUsage eUsage )
{
	m_AutoBufferUsage = eUsage;
}