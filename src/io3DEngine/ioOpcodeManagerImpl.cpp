

#include "stdafx.h"

#include "ioResourceManager.h"

#include "ioOpcodeShape.h"
#include "ioOpcodeManager.h"

#include "ioOpcodeShapeImpl.h"
#include "ioOpcodeManagerImpl.h"

#include "ioFileTokenDefine.h"
#include "ioStream.h"
#include "ioResourceLoader.h"

template<> ioOpcodeManagerImpl* Singleton< ioOpcodeManagerImpl >::ms_Singleton = 0;

ioOpcodeManagerImpl::ioOpcodeManagerImpl()
{
	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Mesh" );

	m_pVertex = NULL;
	m_pFace = NULL;

	m_iVertexCnt = 0;
	m_iFaceCnt = 0;
}

ioOpcodeManagerImpl::~ioOpcodeManagerImpl()
{
	ClearLoadHelper();
}

ioResource* ioOpcodeManagerImpl::CreateImpl( const ioHashString &name, bool bThread )
{
	ioOpcodeShape *pShape = new ioOpcodeShapeImpl( name );
	if( LoadFile( name, pShape ) )
		return pShape;

	SAFEDELETE( pShape );
	return NULL;
}

void ioOpcodeManagerImpl::ReLoadImpl( const ioHashString &name )
{
	ioOpcodeShape *pShape = GetShapeByName( name );
	if( pShape )
	{
		LoadFile( name, pShape );
	}
}

ioOpcodeShape* ioOpcodeManagerImpl::CreateShape( const ioHashString &name )
{
	return dynamic_cast< ioOpcodeShape* >( Create( name, false ) );
}

ioOpcodeShape* ioOpcodeManagerImpl::GetShapeByName( const ioHashString &name )
{
	return dynamic_cast< ioOpcodeShape* >( GetShapeByName( name ) );
}

bool ioOpcodeManagerImpl::LoadFile( const ioHashString &name, ioOpcodeShape *pShape )
{
	char szPath[MAX_PATH];
	wsprintf( szPath, "%s/%s", m_szStartDir, name.c_str() );

	bool bLoaded = false;

	ioBinaryStream kStream;
	if( g_ResourceLoader.LoadStream( szPath, &kStream ) )
	{
		bLoaded = LoadFileInternal( pShape, &kStream );
		
		if( bLoaded )
		{
			pShape->SetLoadState( RLS_LOADED );
			AddMem( pShape->GetName(), kStream.GetSize() );
		}
		else
		{
			pShape->SetLoadState( RLS_FAILED );
		}
	}
	else
	{
		pShape->SetLoadState( RLS_FAILED );
		LOG.PrintTimeAndLog( 0, "ioOpcodeManagerImpl::LoadFile - %s Load Failed", szPath );
	}

	return bLoaded;
}

bool ioOpcodeManagerImpl::LoadFileInternal( ioOpcodeShape *pShape, ioBinaryStream *pStream )
{
	char szToken[8];
	ZeroMemory( szToken, 8 );
	pStream->Read( szToken, 4 );

	bool bLoadOk = false;
	if( IsOpcodeShapeFile( szToken ) )
	{
		bLoadOk = LoadOpcodeShapeMesh( pStream );
	}
	else if( IsNormalMeshFile( szToken ) )
	{
		bLoadOk = LoadOpcodeShapeNormalMesh( pStream );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioOpcodeManagerImpl::LoadFileInternal - %s is not Mesh File!",
								pShape->GetName().c_str() );
	}

	if( bLoadOk )
	{
		BuildOpcodeShape( pShape );
	}

	ClearLoadHelper();

	return bLoadOk;
}

bool ioOpcodeManagerImpl::IsOpcodeShapeFile( const char *szToken )
{
	if( !strcmp( szToken, COL_MESH_TOKEN ) )
		return true;

	return false;
}

bool ioOpcodeManagerImpl::IsNormalMeshFile( const char *szToken )
{
	if( !strcmp( szToken, MESH_TOKEN ) )
		return true;

	return false;
}

bool ioOpcodeManagerImpl::LoadOpcodeShapeMesh( ioBinaryStream *pStream )
{
	// Mesh Version
	int iVersion;
	pStream->Read( &iVersion, sizeof(int) );
	if( iVersion != COL_MESH_VERSION )
	{		
		LOG.PrintTimeAndLog( 0, "Load Fail - not matched version : %s ", pStream->GetMemFileName() );
		return false;
	}

	// Number of Vertices
	int iVertexCnt = 0;
	pStream->Read( &iVertexCnt, sizeof(int) );

	m_pVertex = new D3DXVECTOR3[ iVertexCnt ];
	pStream->Read( m_pVertex, sizeof(D3DXVECTOR3)*iVertexCnt );

	int iFaceCnt = 0;
	pStream->Read( &iFaceCnt, sizeof(int) );

	m_pFace = new WORD[ iFaceCnt * 3 ];
	pStream->Read( m_pFace, sizeof(WORD) * iFaceCnt * 3 );

	m_iVertexCnt = iVertexCnt;
	m_iFaceCnt = iFaceCnt;

	return true;
}

bool ioOpcodeManagerImpl::LoadOpcodeShapeNormalMesh( ioBinaryStream *pStream )
{
	// Skip MeshVersion, MeshType
	pStream->SetCurPos( pStream->GetCurPos() + sizeof(int) * 2 );

	// Mesh Vertex Component Mask
	DWORD dwMask = 0;
	pStream->Read( &dwMask, sizeof(DWORD) );

	// Bound Box, SphereRadius
	pStream->SetCurPos( pStream->GetCurPos() + sizeof(D3DXVECTOR3) * 2 + sizeof(float) );

	// Sub Mesh
	int iSubMeshCnt;
	pStream->Read( &iSubMeshCnt, sizeof(int) );
	pStream->SetCurPos( pStream->GetCurPos() + sizeof(int) * 4 * iSubMeshCnt );

	int iVertexCnt = 0;
	pStream->Read( &iVertexCnt, sizeof(int) );

	m_pVertex = new D3DXVECTOR3[iVertexCnt];
	pStream->Read( m_pVertex, sizeof(D3DXVECTOR3) * iVertexCnt );

	int iSeekBytes = 0;

	if( dwMask & IOFVF_NORMAL )
		iSeekBytes += sizeof(D3DXVECTOR3);
	if( dwMask & IOFVF_TANGENT )
		iSeekBytes += sizeof(D3DXVECTOR3);
	if( dwMask & IOFVF_BINORMAL )
		iSeekBytes += sizeof(D3DXVECTOR3);
	if( dwMask & IOFVF_COLOR0 )
		iSeekBytes += sizeof(DWORD);
	if( dwMask & IOFVF_UV0 )
		iSeekBytes += sizeof(D3DXVECTOR2);
	if( dwMask & IOFVF_UV1 )
		iSeekBytes += sizeof(D3DXVECTOR2);

	pStream->SetCurPos( pStream->GetCurPos() + iSeekBytes*iVertexCnt );

	if( dwMask & IOFVF_WEIGHTS )
	{
		int iBipedIndexCnt = 0;
		pStream->Read( &iBipedIndexCnt, sizeof(int) );

		char szName[MAX_PATH];
		for( int i=0 ; i<iBipedIndexCnt ; i++ )
		{
			pStream->ReadString( szName );
		}

		pStream->SetCurPos( pStream->GetCurPos() + sizeof(BlendWeight)*iVertexCnt );
	}
	
	// Billboard Center
	if( dwMask & IOFVF_POSITION2 )
		pStream->SetCurPos( pStream->GetCurPos() + sizeof(D3DXVECTOR3)*iVertexCnt );

	int iFaceCnt = 0;
	pStream->Read( &iFaceCnt, sizeof(int) );
	
	m_pFace = new WORD[ iFaceCnt * 3 ];
	pStream->Read( m_pFace, sizeof(WORD) * iFaceCnt * 3 );

	m_iVertexCnt = iVertexCnt;
	m_iFaceCnt = iFaceCnt;

	return true;
}

void ioOpcodeManagerImpl::BuildOpcodeShape( ioOpcodeShape *pShape )
{
	pShape->ClearShape();

	pShape->Begin( m_iVertexCnt, m_iFaceCnt );
	pShape->SetVertexArray( m_pVertex, m_iVertexCnt );
	pShape->SetTriangleArray( m_pFace, m_iFaceCnt );
	pShape->End();
}

void ioOpcodeManagerImpl::ClearLoadHelper()
{
	SAFEDELETEARRAY( m_pVertex );
	SAFEDELETEARRAY( m_pFace );

	m_iVertexCnt = 0;
	m_iFaceCnt = 0;
}

ioOpcodeManagerImpl& ioOpcodeManagerImpl::GetSingleton()
{
	return Singleton< ioOpcodeManagerImpl >::GetSingleton();
}

static ioOpcodeManager *pMgr = NULL;

__EX ioOpcodeManager* CreateOpcodeManager()
{
	if( !pMgr )
	{
		pMgr = new ioOpcodeManagerImpl;
	}

	return pMgr;
}

__EX void DestroyOpcodeManager()
{
	SAFEDELETE( pMgr );
}

__EX ioOpcodeManager& GetOpcodeManager()
{
	return ioOpcodeManagerImpl::GetSingleton();
}
