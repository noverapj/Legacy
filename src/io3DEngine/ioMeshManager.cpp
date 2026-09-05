S

#include "stdafx.h"

#include "ioResource.h"

#include "ioVertexFormat.h"
#include "ioVertexDeclaration.h"
#include "ioVertexBufferInstance.h"
#include "ioVertexBufferBinder.h"
#include "ioVertexBufferManager.h"

#include "ioIndexBufferInstance.h"
#include "ioIndexBufferManager.h"

#include "ioFileTokenDefine.h"
#include "ioStream.h"
#include "ioResourceLoader.h"

#include "ioSubMesh.h"
#include "ioMesh.h"
#include "ioMeshManager.h"
#include "ioMaterialManager.h"
#include "ioVertexFormat.h"
#include "ioMeshControlPoint.h"

#include "ioCPU.h"
#include "ioProgressiveMesh.h"

template<> ioMeshManager* Singleton< ioMeshManager >::ms_Singleton = 0;

typedef std::map<WORD, WORD> ExistIndexMap;

template< typename T >
void ReArrangeVertexByNewIndex( const ExistIndexMap &rkIndexMap, T *&pSrc )
{
	if( !pSrc || rkIndexMap.empty() )
		return;

	T *pNew = new T[rkIndexMap.size()];

	ExistIndexMap::const_iterator iter;
	for( iter=rkIndexMap.begin() ; iter!=rkIndexMap.end() ; ++iter )
	{
		pNew[iter->second] = pSrc[iter->first];
	}

	SAFEDELETEARRAY( pSrc );
	pSrc = pNew;
	pNew = NULL;
}

template< typename T >
void CopyVertexComponent( BYTE *&pDest, const T *pSrc, int iIndex )
{
	if( pSrc )
	{
		*( (T*)pDest ) = pSrc[iIndex];
		pDest += sizeof(T);
	}
}

ioMeshManager::ioMeshManager()
{
	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Mesh" );

	m_NotifiedRequestList.reserve( 10 );
	InitializeCriticalSection( &m_NotifiedListSection );

	m_fAnimateMeshLODRate = 0.0f;
	m_bDividedStaticVertexStream = false;

	m_pPos		= NULL;
	m_pNormal	= NULL;
	m_pTangent	= NULL;
	m_pBiNormal	= NULL;
	m_pTexUV	= NULL;
	m_pLightTexUV = NULL;
	m_pWeight	= NULL;
	m_pVtxColor = NULL;

	m_pBillCenter = NULL;
	m_pFace		= NULL;

	m_iCurJointWeightCnt = 0;
	m_iCurMaxWeightCnt = 0;	

	SetDefaultErasePolicy( EP_MANUAL );
}

ioMeshManager::~ioMeshManager()
{
	DeleteCriticalSection( &m_NotifiedListSection );
}

ioResource* ioMeshManager::CreateImpl( const ioHashString &name, bool bThread )
{
	ioMesh *pMesh = new ioMesh( name );
	if( !pMesh )	return NULL;

	int iMaterialCnt = g_MaterialMgr.ParseScript( name.c_str() );
	if( iMaterialCnt > 0 )
	{
		char szMaterial[MAX_PATH];
		for( int i=0 ; i<iMaterialCnt ; i++ )
		{
			ioSubMesh *pSubMesh = pMesh->CreateSubMesh();
			if( pSubMesh )
			{
				wsprintf( szMaterial, "%s/Sub%d", name.c_str(), i+1 );
				pSubMesh->SetMaterialName( szMaterial );
			}
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioMeshManager::CreateImpl - %s Mesh Material Not Exist", name.c_str() );

		delete pMesh;
		return NULL;
	}

	if( !LoadFile( pMesh, bThread ) )
	{
		delete pMesh;
		return NULL;
	}

	return pMesh;
}

void ioMeshManager::ReLoadImpl( const ioHashString &name )
{
	ioMesh *pMesh = GetMeshByName( name );
	if( pMesh && pMesh->IsLoaded() )
	{
		LoadFile( pMesh, false );
	}
}

void ioMeshManager::NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded )
{
	NotifiedRequest kRequest;
	kRequest.m_pStream = dynamic_cast<ioBinaryStream*>( pStream );
	kRequest.m_pMesh = static_cast< ioMesh* >( pParam );

	if( bLoaded )
		kRequest.m_dwLoadState = RLS_LOADED;
	else
		kRequest.m_dwLoadState = RLS_FAILED;

	EnterCriticalSection( &m_NotifiedListSection );
	m_NotifiedRequestList.push_back( kRequest );
	LeaveCriticalSection( &m_NotifiedListSection );
}

ioMesh* ioMeshManager::CreateMesh( const ioHashString &name, bool bThread )
{
	return dynamic_cast< ioMesh* >( Create( name, bThread ) );
}

ioMesh* ioMeshManager::GetMeshByName( const ioHashString &name )
{
	return dynamic_cast< ioMesh* >( GetByName( name ) );
}

void ioMeshManager::SetAnimateMeshLODRate( float fRate )
{
	if( m_fAnimateMeshLODRate == fRate )
		return;

	m_fAnimateMeshLODRate = fRate;

	ResourceMap::iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioMesh *pMesh = dynamic_cast<ioMesh*>( iter->second );
		if( pMesh->IsLoaded() && pMesh->GetMeshType() == MT_ANIMATION )
		{
			LoadFile( pMesh, false );
		}
	}
}

void ioMeshManager::UpdateNotifiedResources()
{
	if( m_NotifiedRequestList.empty() )
		return;

	NotifiedRequestList vTempRequestList;
	EnterCriticalSection( &m_NotifiedListSection );
	vTempRequestList = m_NotifiedRequestList;
	m_NotifiedRequestList.erase( m_NotifiedRequestList.begin(), m_NotifiedRequestList.end() );
	LeaveCriticalSection( &m_NotifiedListSection );

	NotifiedRequestList::iterator iter=vTempRequestList.begin();
	for( ; iter!=vTempRequestList.end() ; ++iter )
	{
		NotifiedRequest &rkRequest = *iter;

		if( rkRequest.m_dwLoadState == RLS_LOADED )
		{
			if( LoadFileInternal( rkRequest.m_pMesh, rkRequest.m_pStream ) )
			{
				rkRequest.m_pMesh->SetLoadState( RLS_LOADED );
				
				AddMem( rkRequest.m_pMesh->GetName(), rkRequest.m_pStream->GetSize() );
			}
			else
			{
				rkRequest.m_pMesh->SetLoadState( RLS_FAILED );
			}
		}
		else
		{
			rkRequest.m_pMesh->SetLoadState( RLS_FAILED );
		}

		SAFEDELETE( rkRequest.m_pStream );
	}
}

bool ioMeshManager::IsMeshFile( const char *szToken )
{
	if( !strcmp( szToken, MESH_TOKEN ) )
		return true;

	return false;
}

bool ioMeshManager::LoadFile( ioMesh *pMesh, bool bThread )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "%s/%s.msh", m_szStartDir, pMesh->GetName().c_str() );

	pMesh->SetLoadState( RLS_LOADING );

	bool bLoaded = true;
	if( bThread )
	{
		ioBinaryStream *pStream = new ioBinaryStream;
		g_ResourceLoader.LoadStream( szFileName, pStream, this, pMesh );
	}
	else
	{
		ioBinaryStream kStream;
		if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
		{
			pMesh->SetLoadState( RLS_FAILED );

			LOG.PrintTimeAndLog( 0, "ioMeshManager::LoadFile - %s Load Failed", szFileName );
			return false;
		}

		bLoaded = LoadFileInternal( pMesh, &kStream );
		if( bLoaded )		
			pMesh->SetLoadState( RLS_LOADED );
		else
			pMesh->SetLoadState( RLS_FAILED );

		if( bLoaded )
			AddMem( pMesh->GetName(), kStream.GetSize() );
	}

	return bLoaded;
}

bool ioMeshManager::LoadFileInternal( ioMesh *pMesh, ioBinaryStream *pStream )
{
	char szToken[8];
	ZeroMemory( szToken, 8 );
	pStream->Read( szToken, 4 );
	if( !IsMeshFile( szToken ) )
	{
		LOG.PrintTimeAndLog( 0, "ioMeshManager::LoadFile - %s is Not Mesh File",
							   pMesh->GetName().c_str() );
		return false;
	}

	// Mesh Version
	int iVersion = 0;
	pStream->Read( &iVersion, sizeof(int) );

	// MeshType
	MeshType eMeshType;
	pStream->Read( &eMeshType, sizeof(MeshType) );
	pMesh->SetMeshType( eMeshType );

	// Mesh Vertex Component Mask
	DWORD dwMask = 0;
	pStream->Read( &dwMask, sizeof(DWORD) );

	// Bounding box
	D3DXVECTOR3 vMin, vMax;
	pStream->Read( &vMin, sizeof(D3DXVECTOR3) );
	pStream->Read( &vMax, sizeof(D3DXVECTOR3) );

	ioAxisAlignBox kAABB( vMin, vMax );
	pMesh->SetBoundBox( kAABB );

	// Bound Radius
	float fSphereRadius;
	pStream->Read( &fSphereRadius, sizeof(float) );
	pMesh->SetBoundRadius( fSphereRadius );

	int i;

	// SubMesh
	int iSubMeshCnt;
	pStream->Read( &iSubMeshCnt, sizeof(int) );

	if( pMesh->GetNumSubMesh() != iSubMeshCnt )
	{
		LOG.PrintTimeAndLog( 0, "ioMeshManager::LoadFileInternal - %s Mesh(%d) : Material(%d) Count Not Match",
								pMesh->GetName().c_str(),
								iSubMeshCnt,
								pMesh->GetNumSubMesh() );
	}
	
	for( i=0 ; i<iSubMeshCnt ; i++ )
	{
		ioSubMesh *pSubMesh = pMesh->GetSubMesh( i );
		if( pSubMesh )
		{
			pStream->Read( &pSubMesh->m_iMinIndex, sizeof(int) );
			pStream->Read( &pSubMesh->m_iVertexCount, sizeof(int) );
			pStream->Read( &pSubMesh->m_iIndexStart, sizeof(int) );
			pStream->Read( &pSubMesh->m_iFaceCount, sizeof(int) );
		}
		else
		{
			pStream->SetCurPos( pStream->GetCurPos() + sizeof(int) * 4 );
		}
	}

	// Number of Vertices
	int iVertexCnt = 0;
	pStream->Read( &iVertexCnt, sizeof(int) );

	if( dwMask & IOFVF_POSITION )
		LoadPosition( pStream, iVertexCnt );
	if( dwMask & IOFVF_NORMAL )
		LoadNormal( pStream, iVertexCnt );
	if( dwMask & IOFVF_TANGENT )
		LoadTangentList( pStream, iVertexCnt );
	if( dwMask & IOFVF_BINORMAL )
		LoadBiNormalList( pStream, iVertexCnt );
	if( dwMask & IOFVF_COLOR0 )
		LoadVertexColor( pStream, iVertexCnt );
	if( dwMask & IOFVF_UV0 )
		LoadTexUV( pStream, iVertexCnt );
	if( dwMask & IOFVF_UV1 )
		LoadLightTexUV( pStream, iVertexCnt );

	if( dwMask & IOFVF_WEIGHTS )
	{
		int iBipedIndexCnt = 0;
		pStream->Read( &iBipedIndexCnt, sizeof(int) );

		ioHashStringVec vBipedNameList;
		vBipedNameList.reserve( iBipedIndexCnt );

		char szName[MAX_PATH];
		for( i=0 ; i<iBipedIndexCnt ; i++ )
		{
			if( pStream->ReadString( szName ) > 0 )
			{
				vBipedNameList.push_back( ioHashString(szName) );
			}
		}

		pMesh->SetBipedNameList( vBipedNameList );

		LoadBlendWeight( pStream, iVertexCnt );
	}

	// Billboard Center
	if( dwMask & IOFVF_POSITION2 )
	{
		LoadBillboardCenter( pStream, iVertexCnt );
	}

	// Number of Faces
	int iFaceCnt = 0;
	pStream->Read( &iFaceCnt, sizeof(int) );

	LoadFace( pStream, iFaceCnt );

/*
	if( eMeshType == MT_ANIMATION && m_fAnimateMeshLODRate > 0.0f )
	{
		BuildAnimateLODMesh( pMesh, iVertexCnt, iFaceCnt );
	}
*/
	pMesh->DestroyHardwareBuffers();

	if( !CreateVertexBuffers( BT_MANAGED, iVertexCnt, dwMask, pMesh ) )
	{
		ClearTempBuffers();
		pMesh->DestroyHardwareBuffers();
		return false;
	}

	if( !CreateIndexBufferInstance( BT_MANAGED, iFaceCnt, pMesh ) )
	{
		ClearTempBuffers();
		pMesh->DestroyHardwareBuffers();
		return false;
	}

	ApplySoftBuffers( pMesh, eMeshType, iVertexCnt, iFaceCnt );

	if( iVersion == MESH_CONTROL_POINT_VERSION )
	{
		pMesh->ClearAllControlPoints();

		int iPointCnt;
		pStream->Read( &iPointCnt, sizeof(int) );

		char szBuf[MAX_PATH];
		D3DXVECTOR3 vPoint;
		for( i=0 ; i<iPointCnt ; i++ )
		{
			ioMeshControlPoint *pCPoint = new ioMeshControlPoint;

			if( pStream->ReadString( szBuf ) > 0 )
				pCPoint->SetTypeIndex( szBuf );

			if( pStream->ReadString( szBuf ) > 0 )
				pCPoint->SetLinkedBipedName( szBuf );

			if( pStream->ReadString( szBuf ) > 0 )
				pCPoint->SetExtraInfo( szBuf );

			pStream->Read( &vPoint, sizeof(D3DXVECTOR3) );
			pCPoint->SetPoint( vPoint );
			
			if( !pMesh->AddControlPoint( pCPoint ) )
			{
				delete pCPoint;
			}
		}
	}

	return true;
}

void ioMeshManager::BuildAnimateLODMesh( ioMesh *pMesh, int &iVertexCount, int &iFaceCount )
{
	int iSubMeshCount = pMesh->GetNumSubMesh();

	ioProgressiveMesh::LODUsageList vLODSubMeshList;
	vLODSubMeshList.reserve( iSubMeshCount );

	int i;
	
	int iOrgVertexStart = 0;
	int iTotalNewIndexCount = 0;

	for( i=0 ; i<iSubMeshCount ; i++ )
	{
		ioSubMesh *pSubMesh = pMesh->GetSubMesh( i );

		ioProgressiveMesh kPMesh;

		kPMesh.SetMeshInfo( m_pPos + iOrgVertexStart,
							pSubMesh->m_iVertexCount,
							m_pFace + pSubMesh->m_iIndexStart,
							pSubMesh->m_iFaceCount * 3,
							iOrgVertexStart );

		iOrgVertexStart += pSubMesh->m_iVertexCount;

		ioProgressiveMesh::LODUsage kUsage;
		kPMesh.Build( ioProgressiveMesh::VRM_RATE, m_fAnimateMeshLODRate, kUsage );

		vLODSubMeshList.push_back( kUsage );
		iTotalNewIndexCount += kUsage.iIndexCount;
	}

	// Index 재정렬
	ExistIndexMap vExistIndexMap;
	ExistIndexMap::iterator iExistIndex;
	WORD *pNewFace = new WORD[ iTotalNewIndexCount ];

	WORD wNewIndex = 0;
	int iNewIndexCursor = 0;

	for( i=0 ; i<iSubMeshCount ; i++ )
	{
		ioProgressiveMesh::LODUsage &rkUsage = vLODSubMeshList[i];

		ioSubMesh *pSubMesh = pMesh->GetSubMesh(i);
		pSubMesh->m_iMinIndex   = wNewIndex;
		pSubMesh->m_iIndexStart = iNewIndexCursor;
		pSubMesh->m_iFaceCount  = rkUsage.iIndexCount / 3;

		for( int j=0 ; j<rkUsage.iIndexCount ; j++ )
		{
			iExistIndex = vExistIndexMap.find( rkUsage.pIndex[j] );
			if( iExistIndex == vExistIndexMap.end() )
			{
				pNewFace[iNewIndexCursor] = wNewIndex;
				vExistIndexMap.insert( ExistIndexMap::value_type( rkUsage.pIndex[j], wNewIndex ) );
				wNewIndex++;
			}
			else
			{
				pNewFace[iNewIndexCursor] = iExistIndex->second;
			}

			iNewIndexCursor++;
		}

		pSubMesh->m_iVertexCount = wNewIndex - pSubMesh->m_iMinIndex;

		SAFEDELETEARRAY( rkUsage.pIndex );
	}

	SAFEDELETEARRAY( m_pFace );
	m_pFace = pNewFace;
	iFaceCount = iTotalNewIndexCount / 3;

	// 버텍스 재정렬
	ReArrangeVertexByNewIndex( vExistIndexMap, m_pPos );
	ReArrangeVertexByNewIndex( vExistIndexMap, m_pNormal );
	ReArrangeVertexByNewIndex( vExistIndexMap, m_pTangent );
	ReArrangeVertexByNewIndex( vExistIndexMap, m_pBiNormal );
	ReArrangeVertexByNewIndex( vExistIndexMap, m_pTexUV );
	ReArrangeVertexByNewIndex( vExistIndexMap, m_pLightTexUV );
	ReArrangeVertexByNewIndex( vExistIndexMap, m_pWeight );
	ReArrangeVertexByNewIndex( vExistIndexMap, m_pVtxColor );
	ReArrangeVertexByNewIndex( vExistIndexMap, m_pBillCenter );
	iVertexCount = vExistIndexMap.size();
}

void ioMeshManager::ApplySoftBuffers( ioMesh *pMesh, MeshType eType, int iVtxCnt, int iFaceCnt )
{
	if( m_pPos )
	{
		pMesh->AddSoftVertexBuffer( IOFVF_POSITION, m_pPos );
		m_pPos = NULL;	// pMesh로 권한이 이전되었다.
	}

	if( m_pNormal )
	{
		pMesh->AddSoftVertexBuffer( IOFVF_NORMAL, m_pNormal );
		m_pNormal = NULL;
	}

	if( m_pTexUV )
	{
		pMesh->AddSoftVertexBuffer( IOFVF_UV0, m_pTexUV );
		m_pTexUV = NULL;
	}

	if( m_pFace )
	{
		pMesh->SetSoftIndexBuffer( m_pFace );
		m_pFace  = NULL;
	}

	if( eType == MT_ANIMATION )
	{
		pMesh->BuildSoftSkinningBuffers( iVtxCnt );
		pMesh->SetSoftBlendWeight( m_pWeight, iVtxCnt, m_iCurJointWeightCnt );
		pMesh->SetWeightBipedCnt( m_iCurMaxWeightCnt );
	}

	SAFEDELETEARRAY( m_pTangent );
	SAFEDELETEARRAY( m_pBiNormal );

	SAFEDELETEARRAY( m_pLightTexUV );
	SAFEDELETEARRAY( m_pWeight );
	SAFEDELETEARRAY( m_pVtxColor );
	SAFEDELETEARRAY( m_pBillCenter );
}

void ioMeshManager::ClearTempBuffers()
{
	SAFEDELETEARRAY( m_pPos );
	SAFEDELETEARRAY( m_pNormal );
	SAFEDELETEARRAY( m_pTangent );
	SAFEDELETEARRAY( m_pBiNormal );
	SAFEDELETEARRAY( m_pTexUV );
	SAFEDELETEARRAY( m_pLightTexUV );
	SAFEDELETEARRAY( m_pWeight );
	SAFEDELETEARRAY( m_pVtxColor );
	SAFEDELETEARRAY( m_pBillCenter );
	SAFEDELETEARRAY( m_pFace );
}

ioMeshManager& ioMeshManager::GetSingleton()
{
	return Singleton<ioMeshManager>::GetSingleton();
}

void ioMeshManager::LoadPosition( ioBinaryStream *pStream, int iVtxCnt )
{
	m_pPos = new D3DXVECTOR3[ iVtxCnt ];
	pStream->Read( m_pPos, sizeof(D3DXVECTOR3)*iVtxCnt );
}

void ioMeshManager::LoadNormal( ioBinaryStream *pStream, int iVtxCnt )
{
	m_pNormal = new D3DXVECTOR3[ iVtxCnt ];
	pStream->Read( m_pNormal, sizeof(D3DXVECTOR3)*iVtxCnt );
}

void ioMeshManager::LoadTexUV( ioBinaryStream *pStream, int iVtxCnt )
{
	m_pTexUV = new D3DXVECTOR2[ iVtxCnt ];
	pStream->Read( m_pTexUV, sizeof(D3DXVECTOR2)*iVtxCnt );
}

void ioMeshManager::LoadLightTexUV( ioBinaryStream *pStream, int iVtxCnt )
{
	m_pLightTexUV = new D3DXVECTOR2[ iVtxCnt ];
	pStream->Read( m_pLightTexUV, sizeof(D3DXVECTOR2)*iVtxCnt );
}

void ioMeshManager::LoadBlendWeight( ioBinaryStream *pStream, int iVtxCnt )
{
	m_pWeight = new BlendWeight[ iVtxCnt ];
	pStream->Read( m_pWeight, sizeof(BlendWeight) * iVtxCnt );

	m_iCurJointWeightCnt = 0;
	m_iCurMaxWeightCnt = 0;

	const BlendWeight *pWeight = m_pWeight;
	for( int i=0 ; i<iVtxCnt ; i++, pWeight++ )
	{
		for( int j=0 ; j<MAX_BLEND_BIPED ; j++ )
		{
			if( pWeight->fWeight[j] > 0.0f )
			{
				m_iCurJointWeightCnt++;
			}
			else
			{
				if( m_iCurMaxWeightCnt < j )
				{
					m_iCurMaxWeightCnt = j;
				}
				break;
			}
		}
	}
}

void ioMeshManager::LoadVertexColor( ioBinaryStream *pStream, int iVtxCnt )
{
	m_pVtxColor = new DWORD[ iVtxCnt ];
	pStream->Read( m_pVtxColor, sizeof(DWORD)*iVtxCnt );
}

void ioMeshManager::LoadTangentList( ioBinaryStream *pStream, int iVtxCnt )
{
	m_pTangent = new D3DXVECTOR3[ iVtxCnt ];
	pStream->Read( m_pTangent, sizeof(D3DXVECTOR3)*iVtxCnt );
}

void ioMeshManager::LoadBiNormalList( ioBinaryStream *pStream, int iVtxCnt )
{
	m_pBiNormal = new D3DXVECTOR3[ iVtxCnt ];
	pStream->Read( m_pBiNormal, sizeof(D3DXVECTOR3) * iVtxCnt );
}

void ioMeshManager::LoadBillboardCenter( ioBinaryStream *pStream, int iVtxCnt )
{
	m_pBillCenter = new D3DXVECTOR3[ iVtxCnt ];
	pStream->Read( m_pBillCenter, sizeof(D3DXVECTOR3) * iVtxCnt );
}

void ioMeshManager::LoadFace( ioBinaryStream *pStream, int iFaceCnt )
{
	m_pFace = new WORD[ iFaceCnt * 3 ];
	pStream->Read( m_pFace, sizeof(WORD) * iFaceCnt * 3 );
}

bool ioMeshManager::CreateVertexBuffers( BufferType bType, int iVtxCnt, DWORD dwMask, ioMesh *pMesh )
{
	pMesh->SetVertexCount( iVtxCnt );

	MeshType eMeshType = pMesh->GetMeshType();
	if( eMeshType == MT_ANIMATION )
		return CreateDivideStream( bType, iVtxCnt, dwMask, pMesh );

	if( eMeshType == MT_STATIC && m_bDividedStaticVertexStream )
		return CreateDivideStream( bType, iVtxCnt, dwMask, pMesh );

	return CreateOneStream( bType, iVtxCnt, dwMask, pMesh );
}

bool ioMeshManager::CreateDivideStream( BufferType bType, int iVtxCnt, DWORD dwMask, ioMesh *pMesh )
{
	VertexComponentList kComponentList;
	ioVertexDeclaration::ParseStreamMaskToComponent( dwMask, kComponentList );

	VertexComponentList::iterator iter;
	for( iter=kComponentList.begin() ; iter!=kComponentList.end() ; ++iter )
	{
		ioVertexBufferInstance *pVtxInstance = NULL;

		switch( *iter )
		{
		case IOFVF_POSITION:
			pVtxInstance = CreatePosition( bType, iVtxCnt );
			break;
		case IOFVF_POSITION2:
			pVtxInstance = CreatePosition2( bType, iVtxCnt );
			break;
		case IOFVF_WEIGHTS:
			pVtxInstance = CreateWeights( bType, iVtxCnt );
			break;
		case IOFVF_INDICES:
			pVtxInstance = CreateIndices( bType, iVtxCnt );
			break;
		case IOFVF_NORMAL:
			pVtxInstance = CreateNormal( bType, iVtxCnt );
			break;
		case IOFVF_COLOR0:
			pVtxInstance = CreateColor0( bType, iVtxCnt );
			break;
		case IOFVF_UV0:
			pVtxInstance = CreateUV0( bType, iVtxCnt );
			break;
		case IOFVF_UV1:
			pVtxInstance = CreateUV1( bType, iVtxCnt );
			break;
		case IOFVF_TANGENT:
			pVtxInstance = CreateTangent( bType, iVtxCnt );
			break;
		case IOFVF_BINORMAL:
			pVtxInstance = CreateBinormal( bType, iVtxCnt );
			break;
		}

		if( pVtxInstance )
			pMesh->AddVertexBuffer( pVtxInstance );
		else
			return false;
	}

	return true;
}

bool ioMeshManager::CreateOneStream( BufferType bType, int iVtxCnt, DWORD dwMask, ioMesh *pMesh )
{
	ioVertexBufferInstance *pInstance = g_VtxBufMgr.CreateInstance( bType, dwMask, iVtxCnt );
	if( !pInstance )	return false;
	
	if( !pInstance->Lock() )
		return false;

	BYTE *pBuf = (BYTE*)pInstance->GetBuffer();
	for( int i=0 ; i<iVtxCnt ; i++ )
	{
		CopyVertexComponent( pBuf, m_pPos, i );
		CopyVertexComponent( pBuf, m_pBillCenter, i );

		if( m_pWeight )
		{
			BlendWeight *pCurWeight = m_pWeight + i;

			D3DXVECTOR4 *pWeight = (D3DXVECTOR4*)pBuf;
			pWeight->x = pCurWeight->fWeight[0];
			pWeight->y = pCurWeight->fWeight[1];
			pWeight->z = pCurWeight->fWeight[2];
			pWeight->w = pCurWeight->fWeight[3];
			pBuf += sizeof(D3DXVECTOR4);

			DWORD dwIndices = 0;
			dwIndices  = ( (DWORD)pCurWeight->fBipedID[0] );
			dwIndices |= ( (DWORD)pCurWeight->fBipedID[1] ) << 8;
			dwIndices |= ( (DWORD)pCurWeight->fBipedID[2] ) << 16;
			dwIndices |= ( (DWORD)pCurWeight->fBipedID[3] ) << 24;
			*( (DWORD*)pBuf ) = dwIndices;
			pBuf += sizeof(DWORD);
		}

		CopyVertexComponent( pBuf, m_pNormal, i );
		CopyVertexComponent( pBuf, m_pVtxColor, i );
		CopyVertexComponent( pBuf, m_pTexUV, i );
		CopyVertexComponent( pBuf, m_pLightTexUV, i );
		CopyVertexComponent( pBuf, m_pTangent, i );
		CopyVertexComponent( pBuf, m_pBiNormal, i );
	}

	pInstance->UnLock();
	pMesh->AddVertexBuffer( pInstance );
	return true;
}

bool ioMeshManager::CreateIndexBufferInstance( BufferType bType, int iFaceCnt, ioMesh *pMesh )
{
	int iIndexCount = iFaceCnt * 3;
	pMesh->SetIndexCount( iIndexCount );

	ioIndexBufferInstance *pInstance = g_IdxBufMgr.CreateInstance( bType, iIndexCount, m_pFace );
	if( pInstance )
	{
		pMesh->SetIndexBufferInstance( pInstance );
		return true;
	}

	return false;
}

ioVertexBufferInstance* ioMeshManager::CreatePosition( BufferType bType, int iVtxCnt )
{
	if( m_pPos )
		return g_VtxBufMgr.CreateInstance( bType, IOFVF_POSITION, iVtxCnt, m_pPos );

	return NULL;
}

ioVertexBufferInstance* ioMeshManager::CreatePosition2( BufferType bType, int iVtxCnt )
{
	if( m_pBillCenter )
		return g_VtxBufMgr.CreateInstance( bType, IOFVF_POSITION2, iVtxCnt, m_pBillCenter );

	return NULL;
}

ioVertexBufferInstance* ioMeshManager::CreateWeights( BufferType bType, int iVtxCnt )
{
	if( !m_pWeight )	return NULL;

	ioVertexBufferInstance *pInstance = g_VtxBufMgr.CreateInstance( bType, IOFVF_WEIGHTS, iVtxCnt );
	if( !pInstance )	return NULL;

	if( !pInstance->Lock() )
	{
		g_VtxBufMgr.DestroyInstance( pInstance );
		return NULL;
	}

	D3DXVECTOR4 *pBuf = (D3DXVECTOR4 *)pInstance->GetBuffer();

	BlendWeight *pWeights = m_pWeight;
	for( int i=0 ; i<iVtxCnt ; i++ )
	{
		pBuf->x = pWeights->fWeight[0];
		pBuf->y = pWeights->fWeight[1];
		pBuf->z = pWeights->fWeight[2];
		pBuf->w = pWeights->fWeight[3];

		pBuf++;
		pWeights++;
	}

	pInstance->UnLock();

	return pInstance;
}

ioVertexBufferInstance* ioMeshManager::CreateIndices( BufferType bType, int iVtxCnt )
{
	if( !m_pWeight )	return NULL;

	ioVertexBufferInstance *pInstance = g_VtxBufMgr.CreateInstance( bType, IOFVF_INDICES, iVtxCnt );
	if( !pInstance )	return NULL;

	if( !pInstance->Lock() )
	{
		g_VtxBufMgr.DestroyInstance( pInstance );
		return NULL;
	}

	DWORD dwIndices = 0;
	DWORD *pBuf= (DWORD*)pInstance->GetBuffer();

	BlendWeight *pWeights = m_pWeight;
	for( int i=0 ; i<iVtxCnt ; i++ )
	{
		dwIndices  = ( (DWORD)pWeights->fBipedID[0] );
		dwIndices |= ( (DWORD)pWeights->fBipedID[1] ) << 8;
		dwIndices |= ( (DWORD)pWeights->fBipedID[2] ) << 16;
		dwIndices |= ( (DWORD)pWeights->fBipedID[3] ) << 24;

		pBuf[i] = dwIndices;
		pWeights++;
	}

	pInstance->UnLock();

	return pInstance;
}

ioVertexBufferInstance* ioMeshManager::CreateNormal( BufferType bType, int iVtxCnt )
{
	if( m_pNormal )
		return g_VtxBufMgr.CreateInstance( bType, IOFVF_NORMAL, iVtxCnt, m_pNormal );

	return NULL;
}

ioVertexBufferInstance* ioMeshManager::CreateColor0( BufferType bType, int iVtxCnt )
{
	if( m_pVtxColor )
		return g_VtxBufMgr.CreateInstance( bType, IOFVF_COLOR0, iVtxCnt, m_pVtxColor );

	return NULL;
}

ioVertexBufferInstance* ioMeshManager::CreateUV0( BufferType bType, int iVtxCnt )
{
	if( m_pTexUV )
		return g_VtxBufMgr.CreateInstance( bType, IOFVF_UV0, iVtxCnt, m_pTexUV );

	return NULL;
}

ioVertexBufferInstance* ioMeshManager::CreateUV1( BufferType bType, int iVtxCnt )
{
	if( m_pLightTexUV )
		return g_VtxBufMgr.CreateInstance( bType, IOFVF_UV1, iVtxCnt, m_pLightTexUV );

	return NULL;
}

ioVertexBufferInstance* ioMeshManager::CreateTangent( BufferType bType, int iVtxCnt )
{
	if( m_pTangent )
		return g_VtxBufMgr.CreateInstance( bType, IOFVF_TANGENT, iVtxCnt, m_pTangent );

	return NULL;
}

ioVertexBufferInstance* ioMeshManager::CreateBinormal( BufferType bType, int iVtxCnt )
{
	if( m_pBiNormal )
		return g_VtxBufMgr.CreateInstance( bType, IOFVF_BINORMAL, iVtxCnt, m_pBiNormal );

	return NULL;
}
