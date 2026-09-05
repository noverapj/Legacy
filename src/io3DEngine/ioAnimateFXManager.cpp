

#include "stdafx.h"

#include "ioResource.h"
#include "ioRcAniTrack.h"
#include "ioRcAnimateFX.h"

#include "ioAnimateFXManager.h"
#include "ioFileTokenDefine.h"

#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferInstance.h"
#include "ioIndexBufferManager.h"
#include "ioMeshData.h"

#include "ioRenderable.h"
#include "ioMovableObject.h"
#include "ioAnimateFX.h"

#include "ioStream.h"
#include "ioResourceLoader.h"

template<> ioAnimateFXManager* Singleton< ioAnimateFXManager >::ms_Singleton = 0;

ioAnimateFXManager::ioAnimateFXManager()
{
	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Effect/AnimateFX" );

	SetDefaultErasePolicy( EP_MANUAL );
}

ioAnimateFXManager::~ioAnimateFXManager()
{
}

ioResource* ioAnimateFXManager::CreateImpl( const ioHashString &name, bool bThread )
{
	ioRcAnimateFX *pRcFX = new ioRcAnimateFX( name );
	if( LoadFile( name, pRcFX ) )
		return pRcFX;

	SAFEDELETE( pRcFX );
	return NULL;
}

void ioAnimateFXManager::ReLoadImpl( const ioHashString &name )
{
	ioRcAnimateFX *pRcFX = GetAnimateFXByName( name );
	if( pRcFX && pRcFX->IsLoaded() )
	{
		LoadFile( name, pRcFX );
	}
}

ioRcAnimateFX* ioAnimateFXManager::CreateAnimateFX( const ioHashString &name )
{
	return dynamic_cast< ioRcAnimateFX* >( Create( name, false ) );
}

ioRcAnimateFX* ioAnimateFXManager::GetAnimateFXByName( const ioHashString &name )
{
	return dynamic_cast< ioRcAnimateFX* >( GetByName( name ) );
}

bool ioAnimateFXManager::IsAnimateFXFile( ioBinaryStream *pStream )
{
	char szBuf[8];
	ZeroMemory( szBuf, 8 );
	pStream->Read( szBuf, 4 );

	if( !strcmp( szBuf, ANIMATE_EFFECT_TOKEN ) )
		return true;

	return false;
}

bool ioAnimateFXManager::LoadFile( const ioHashString &name, ioRcAnimateFX *pRcFX )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "%s/%s", m_szStartDir, name.c_str() );

	bool bLoaded = false;

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		bLoaded = LoadFileInternal( pRcFX, &kStream );
		if( bLoaded )
			pRcFX->SetLoadState( RLS_LOADED );
		else
			pRcFX->SetLoadState( RLS_FAILED );

		if( bLoaded )
			AddMem( pRcFX->GetName(), kStream.GetSize() );

		LOG.PrintTimeAndLog( 0, "ioAnimateFXManager::LoadFile - %s Load Failed", szFileName );
	}
	else
	{
		pRcFX->SetLoadState( RLS_FAILED );
		LOG.PrintTimeAndLog( 0, "ioAnimateFXManager::LoadFile - %s Load Failed", szFileName );
	}

	return bLoaded;
}

bool ioAnimateFXManager::LoadFileInternal( ioRcAnimateFX *pRcFX, ioBinaryStream *pStream )
{
	if( !IsAnimateFXFile( pStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioAnimateFXManager::LoadFile - %s is Not Animate FX File",
								pRcFX->GetName().c_str() );
		return false;
	}

	int iVersion;
	pStream->Read( &iVersion, sizeof(int) );

	D3DXVECTOR3 vMinPos, vMaxPos;
	pStream->Read( &vMinPos, sizeof(D3DXVECTOR3) );
	pStream->Read( &vMaxPos, sizeof(D3DXVECTOR3) );
	pRcFX->SetBoundBox( vMinPos, vMaxPos );

	float fBoundRadius;
	pStream->Read( &fBoundRadius, sizeof(float) );
	pRcFX->SetBoundRadius( fBoundRadius );

	int iPlaneCnt = 0;
	pStream->Read( &iPlaneCnt, sizeof(int) );

	char szBuf[MAX_PATH];
	D3DXQUATERNION qtBipedSpace;
	D3DXVECTOR3 vBipedSpace;
	D3DXMATRIX matBipedSpaceTM;

	int i, j;
	for( i=0 ; i<iPlaneCnt ; i++ )
	{
		ioRcAnimatePlane *pPlane = pRcFX->CreatePlane();

		int iStartTime;
		pStream->Read( &iStartTime, sizeof(int) );
		pPlane->SetStartTime( iStartTime );

		int iDuration;
		pStream->Read( &iDuration, sizeof(int) );
		pPlane->SetDuration( iDuration );

		// BipedSpace 
		pStream->Read( &qtBipedSpace, sizeof(D3DXQUATERNION) );
		pStream->Read( &vBipedSpace, sizeof(D3DXVECTOR3) );

		D3DXMatrixRotationQuaternion( &matBipedSpaceTM, &qtBipedSpace );
		matBipedSpaceTM._41 = vBipedSpace.x;
		matBipedSpaceTM._42 = vBipedSpace.y;
		matBipedSpaceTM._43 = vBipedSpace.z;
		pPlane->SetWorldTMInverse( matBipedSpaceTM );

		ioMeshData *pMesh = pPlane->GetMeshData();

		//Vertex Data
		int iVtxCnt;
		pStream->Read( &iVtxCnt, sizeof(int) );

		ioVertexBufferInstance *pVtxInstance = g_VtxBufMgr.CreateInstance( BT_MANAGED,
																		   IOFVF_POSITION|IOFVF_UV0,
																		   iVtxCnt );

		if( pVtxInstance && pVtxInstance->Lock() )
		{
			pStream->Read( pVtxInstance->GetBuffer(), sizeof(ioVtxTex) * iVtxCnt );
			pVtxInstance->UnLock();
		}
		else
		{
			g_VtxBufMgr.DestroyInstance( pVtxInstance );
			LOG.PrintTimeAndLog( 0, "ioAnimateFXManager::LoadFile - %s Vtx Load Fail",
									pRcFX->GetName().c_str() );
			return false;
		}

		pMesh->InsertStreamSource( 0, IOFVF_POSITION|IOFVF_UV0 );
		pMesh->SetBinding( 0, pVtxInstance );
		pMesh->m_iVertexCount = iVtxCnt;

		//Index Data
		int iFaceCnt;
		pStream->Read( &iFaceCnt, sizeof(int) );

		int iIndexCnt = iFaceCnt * 3;
		ioIndexBufferInstance *pIdxInstance= g_IdxBufMgr.CreateInstance( BT_MANAGED, iIndexCnt );
		if( pIdxInstance && pIdxInstance->Lock() )
		{
			pStream->Read( pIdxInstance->GetBuffer(), sizeof(WORD) * iIndexCnt );
			pIdxInstance->UnLock();
		}
		else
		{
			g_IdxBufMgr.DestroyInstance( pIdxInstance );
			LOG.PrintTimeAndLog( 0, "ioAnimateFXManager::LoadFile - %s Idx Load Fail",
									pRcFX->GetName().c_str() );
			return false;
		}

		pMesh->m_pIndexBufferInstance = pIdxInstance;
		pMesh->m_iIndexCount = iIndexCnt;

		// Texture
		if( pStream->ReadString( szBuf ) > 0 )
			pPlane->SetTextureName( szBuf );

		// Link Biped
		if( pStream->ReadString( szBuf ) > 0 )
			pPlane->SetLinkBipedName( szBuf );

		// Keyframe Data
		int iKeyFrameCnt = 0;
		pStream->Read( &iKeyFrameCnt, sizeof(int) );
		if( iKeyFrameCnt > 0 )
		{
			ioRcAniTrack *pTrack = pPlane->CreateTrack();
			pTrack->ReserveFrameList( iKeyFrameCnt );

			int iTime;
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vTrans;
			for( j=0 ; j<iKeyFrameCnt ; j++ )
			{
				pStream->Read( &qtRot, sizeof(qtRot) );
				pStream->Read( &vTrans, sizeof(vTrans) );
				pStream->Read( &iTime, sizeof(iTime) );
				pTrack->AddKeyFrame( (float)iTime, vTrans, qtRot );
			}
		}

		// RenderType
		FXRenderType eRenderType;
		pStream->Read( &eRenderType, sizeof(int) );
		pPlane->SetRenderType( eRenderType );

		// TransformType
		FXTransformType eTransformType;
		pStream->Read( &eTransformType, sizeof(int) );
		pPlane->SetTransformType( eTransformType );

		// Alpharate List
		int iAlphaRateCnt = 0;
		pStream->Read( &iAlphaRateCnt, sizeof(int) );
		if( iAlphaRateCnt > 0 )
		{
			D3DXVECTOR2 vAlphaRate;
			for( j=0 ; j<iAlphaRateCnt ; j++ )
			{
				pStream->Read( &vAlphaRate, sizeof(D3DXVECTOR2) );
				pPlane->AddAlphaRate( vAlphaRate.x, vAlphaRate.y );
			}
		}
	}

	return true;
}

ioAnimateFX* ioAnimateFXManager::CreateFX( const ioHashString &rkAnimationName )
{
	std::string szTemp( rkAnimationName.c_str() );
	std::string szFileName = szTemp.substr( 0, szTemp.find_last_of( '.' ) );
	szFileName += ".afx";

	ioHashString kFXName( szFileName.c_str() );

	FXFileSet::iterator iter = m_FileNameSet.find( kFXName );
	if( iter == m_FileNameSet.end() )
		return NULL;

	ioRcAnimateFX *pRcFX = CreateAnimateFX( kFXName );
	if( !pRcFX || pRcFX->GetLoadState() == RLS_FAILED )
		return NULL;

	return new ioAnimateFX( pRcFX );
}

void ioAnimateFXManager::InitializeFileSet()
{
	m_FileNameSet.clear();

	char szPath[MAX_PATH];
	wsprintf( szPath, "%s/*.afx", m_szStartDir );

	HANDLE			hSrch;
	WIN32_FIND_DATA wfd;
	BOOL			bResult = TRUE;

	ZeroMemory( &wfd, sizeof(wfd) );
	hSrch = FindFirstFile( szPath, &wfd );

	while( bResult )
	{
		if( !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			strlwr( wfd.cFileName );
			m_FileNameSet.insert( ioHashString(wfd.cFileName ) );
		}

		bResult = FindNextFile(hSrch, &wfd);
	}

	FindClose(hSrch);
}

bool ioAnimateFXManager::SaveFile( const ioHashString &rkName )
{
	ioRcAnimateFX *pRcFX = GetAnimateFXByName( rkName );
	if( !pRcFX )	return false;

	char szPath[MAX_PATH];
	wsprintf( szPath, "%s/%s", m_szStartDir, rkName.c_str() );

	HANDLE hFile = CreateFile( szPath, 
							   GENERIC_WRITE,
							   FILE_SHARE_READ | FILE_SHARE_WRITE,
							   NULL,
							   OPEN_ALWAYS,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		LOG.PrintTimeAndLog( 0, "ioAnimateFXManager::SaveFile - %s Create Failed", szPath );
		return false;
	}

	DWORD dwWriteBytes = 0;
	WriteFile( hFile, ANIMATE_EFFECT_TOKEN, 4, &dwWriteBytes, NULL );

	int iVersion = ANIMATE_EFFECT_VERSION;
	WriteFile( hFile, &iVersion, sizeof(int), &dwWriteBytes, NULL );

	WriteFile( hFile, &pRcFX->GetBoundBox().GetMinPos(), sizeof(D3DXVECTOR3), &dwWriteBytes, NULL );
	WriteFile( hFile, &pRcFX->GetBoundBox().GetMaxPos(), sizeof(D3DXVECTOR3), &dwWriteBytes, NULL );

	float fBoundRadius = pRcFX->GetBoundRadius();
	WriteFile( hFile, &fBoundRadius, sizeof(float), &dwWriteBytes, NULL );

	int iPlaneCnt = pRcFX->GetPlaneCnt();
	WriteFile( hFile, &iPlaneCnt, sizeof(int), &dwWriteBytes, NULL );

	D3DXMATRIX matInverse;
	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vTrans;

	for( int i=0 ; i<iPlaneCnt ; i++ )
	{
		ioRcAnimatePlane *pPlane = pRcFX->GetPlane( i );

		int iStartTime = (int)pPlane->GetStartTime();
		WriteFile( hFile, &iStartTime, sizeof(int), &dwWriteBytes, NULL );

		int iDuration = (int)pPlane->GetDuration();
		WriteFile( hFile, &iDuration, sizeof(int), &dwWriteBytes, NULL );

		matInverse = pPlane->GetWorldTMInverse();
		D3DXQuaternionRotationMatrix( &qtRot, &matInverse );
		vTrans.x = matInverse._41;
		vTrans.y = matInverse._42;
		vTrans.z = matInverse._43;

		WriteFile( hFile, &qtRot, sizeof(D3DXQUATERNION), &dwWriteBytes, NULL );
		WriteFile( hFile, &vTrans, sizeof(D3DXVECTOR3), &dwWriteBytes, NULL );

		ioMeshData *pMesh = pPlane->GetMeshData();

		int iVtxCnt = pMesh->m_iVertexCount;
		WriteFile( hFile, &iVtxCnt, sizeof(int), &dwWriteBytes, NULL );

		ioVertexBufferInstance *pVtxInstance = pMesh->GetVtxInstance(0);
		if( pVtxInstance->Lock() )
		{
			WriteFile( hFile, pVtxInstance->GetBuffer(),
							  sizeof(ioVtxTex) * iVtxCnt,
							  &dwWriteBytes,
							  NULL );

			pVtxInstance->UnLock();
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioAnimateFXManager::SaveFile - %s Vtx Save Fail", szPath );
			CloseHandle( hFile );
			return false;
		}

		int iFaceCnt = pMesh->m_iIndexCount / 3;
		WriteFile( hFile, &iFaceCnt, sizeof(int), &dwWriteBytes, NULL );

		ioIndexBufferInstance *pIdxInstance = pMesh->m_pIndexBufferInstance;
		if( pIdxInstance->Lock() )
		{
			WriteFile( hFile, pIdxInstance->GetBuffer(),
							  sizeof(WORD) * iFaceCnt * 3,
							  &dwWriteBytes,
							  NULL );

			pIdxInstance->UnLock();
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioAnimateFXManager::SaveFile - %s Idx Save Fail", szPath );
			CloseHandle( hFile );
			return false;
		}

		int iNameLen = pPlane->GetTextureName().Length();
		WriteFile( hFile, &iNameLen, sizeof(int), &dwWriteBytes, NULL );
		if( iNameLen > 0 )
		{
			WriteFile( hFile, pPlane->GetTextureName().c_str(), iNameLen, &dwWriteBytes, NULL );
		}

		iNameLen = pPlane->GetLinkBipedName().Length();
		WriteFile( hFile, &iNameLen, sizeof(int), &dwWriteBytes, NULL );
		if( iNameLen > 0 )
		{
			WriteFile( hFile, pPlane->GetLinkBipedName().c_str(), iNameLen, &dwWriteBytes, NULL );
		}

		int iKeyFrameCnt = 0;
		ioRcAniTrack *pTrack = pPlane->GetTrack();
		if( pTrack )
		{
			iKeyFrameCnt = pTrack->GetFrameCount();
			WriteFile( hFile, &iKeyFrameCnt, sizeof(int), &dwWriteBytes, NULL );

			int iTime;
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vTrans;

			for( int j=0 ; j<iKeyFrameCnt ; j++ )
			{
				iTime  = (int)pTrack->GetKeyTime( j );
				qtRot  = pTrack->GetKeyOrientation( j );
				vTrans = pTrack->GetKeyPosition( j );

				WriteFile( hFile, &qtRot, sizeof(D3DXQUATERNION), &dwWriteBytes, NULL );
				WriteFile( hFile, &vTrans, sizeof(D3DXVECTOR3), &dwWriteBytes, NULL );
				WriteFile( hFile, &iTime, sizeof(int), &dwWriteBytes, NULL );
			}
		}
		else
		{
			WriteFile( hFile, &iKeyFrameCnt, sizeof(int), &dwWriteBytes, NULL );
		}

		FXRenderType eRenderType = pPlane->GetRenderType();
		WriteFile( hFile, &eRenderType, sizeof(int), &dwWriteBytes, NULL );

		FXTransformType eTransformType = pPlane->GetTransformType();
		WriteFile( hFile, &eTransformType, sizeof(int), &dwWriteBytes, NULL );

		int iAlphaRateCnt = pPlane->GetAlphaRateCnt();
		WriteFile( hFile, &iAlphaRateCnt, sizeof(int), &dwWriteBytes, NULL );
		if( iAlphaRateCnt > 0 )
		{
			D3DXVECTOR2 vAlphaRate;
			for( int j=0 ; j<iAlphaRateCnt ; j++ )
			{
				vAlphaRate.x = pPlane->GetTimeRate( j );
				vAlphaRate.y = pPlane->GetAlphaRate( j );
				WriteFile( hFile, &vAlphaRate, sizeof(D3DXVECTOR2), &dwWriteBytes, NULL );
			}
		}
	}

	CloseHandle( hFile );

	return true;
}

ioAnimateFXManager& ioAnimateFXManager::GetSingleton()
{
	return Singleton<ioAnimateFXManager>::GetSingleton();
}
