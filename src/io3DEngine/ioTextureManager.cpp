
#include "stdafx.h"

#include "ioTexture.h"
#include "ioRenderTexture.h"
#include "ioTextureManager.h"
#include "ioRenderSystem.h"
#include "ioCPU.h"
#include "HelpFunc.h"
#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ErrorReport.h"

template<> ioTextureManager* Singleton< ioTextureManager >::ms_Singleton = 0;

ioTextureManager::ioTextureManager( IDirect3DDevice9 *pDevice )
{
	SetDefaultErasePolicy( EP_MANUAL );

	m_pD3DDevice = pDevice;
	m_pD3DDevice->AddRef();

	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Texture" );

	m_TextureQuality = TQ_HIGH;
	m_iGrayScaleRate = 110;
	m_iGrayLightScaleRate = 90;

	m_bFileUpdateCheck = false;
}

ioTextureManager::~ioTextureManager()
{
	SAFERELEASE( m_pD3DDevice );
}

void ioTextureManager::ReLoadUpdateFiles()
{
	if( !m_bFileUpdateCheck )
		return;

	FILETIME kFileTime;
	char szFileName[MAX_PATH];

	FileTimeMap::iterator iter;
	for( iter=m_FileTimeMap.begin() ; iter!=m_FileTimeMap.end() ; ++iter )
	{
		ZeroMemory( &kFileTime, sizeof(kFileTime ) );

		ioTexture *pTexture = GetTextureByName( iter->first );
		if( !pTexture )	continue;

		if( pTexture->IsDefaultPath() )
		{
			wsprintf( szFileName, "%s/%s", m_szStartDir, iter->first.c_str() );
			GetLastFileWriteTime( szFileName, kFileTime );
		}
		else
		{
			GetLastFileWriteTime( iter->first.c_str(), kFileTime );
		}

		if( iter->second.dwLowDateTime != kFileTime.dwLowDateTime ||
			iter->second.dwHighDateTime != kFileTime.dwHighDateTime )
		{
			ReLoadImpl( iter->first );
			iter->second = kFileTime;
		}
	}
}

void ioTextureManager::GetLastFileWriteTime( const char *szFileName, FILETIME &rkFileTime )
{
	WIN32_FILE_ATTRIBUTE_DATA kFileAttr;
	if( GetFileAttributesEx( szFileName, GetFileExInfoStandard, (LPVOID)&kFileAttr ) )
	{
		rkFileTime = kFileAttr.ftLastWriteTime;
	}
}

ioResource* ioTextureManager::CreateImpl( const ioHashString &name, bool bThread )
{
	return new ioTexture( name );
}

void ioTextureManager::RegisterImpl( ioResource *pRes )
{
	ioResourceManager::RegisterImpl( pRes );

	if( m_bFileUpdateCheck )
	{
		FILETIME kFileTime;
		ZeroMemory( &kFileTime, sizeof(FILETIME) );

		char szFileName[MAX_PATH];
		wsprintf( szFileName, "%s/%s", m_szStartDir, pRes->GetName().c_str() );

		GetLastFileWriteTime( szFileName, kFileTime );

		m_FileTimeMap.insert( FileTimeMap::value_type( pRes->GetName(), kFileTime ) );
	}
}

void ioTextureManager::ReLoadImpl( const ioHashString &name )
{
	ioTexture *pTexture = GetTextureByName( name );
	if( pTexture )
	{
		ReLoadImpl( pTexture );
	}
}

void ioTextureManager::ReLoadImpl( ioTexture *pTexture )
{
	if( pTexture->IsLoaded() )
	{
		char szFullPath[MAX_PATH];
		if( pTexture->IsDefaultPath() )
			wsprintf( szFullPath, "%s/%s", m_szStartDir, pTexture->GetName().c_str() );
		else
			strcpy( szFullPath, pTexture->GetName().c_str() );

		LoadFile( szFullPath, pTexture, false, pTexture->IsLoadPiece() );
	}
}

void ioTextureManager::NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded )
{
	ioTexture *pTexture = static_cast< ioTexture* >( pParam );

	if( bLoaded )
	{
		if( LoadFileInternal( pTexture, pStream ) )
		{
			pTexture->SetLoadState( RLS_LOADED );
			AddMem( pTexture->GetName(), pStream->GetSize() );
		}
		else
		{
			pTexture->SetLoadState( RLS_FAILED );
		}
	}
	else
	{
		pTexture->SetLoadState( RLS_FAILED );
	}

	delete pStream;
	pStream = NULL;
}

void ioTextureManager::SetGrayScaleRate( int iRate )
{
	m_iGrayScaleRate = max( iRate, 1 );
}

void ioTextureManager::SetGrayLightScaleRate( int iRate )
{
	m_iGrayLightScaleRate = max( iRate, 1 );
}

void ioTextureManager::ChangeTextureQuality( TextureQuality eQuality )
{
	if( m_TextureQuality == eQuality )
		return;

	m_TextureQuality = eQuality;

	ResourceMap::iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioTexture *pTexture = dynamic_cast< ioTexture* >( iter->second );

		if( pTexture->GetUsage() == D3DUSAGE_DYNAMIC ||
			pTexture->GetUsage() == D3DUSAGE_RENDERTARGET )
			continue;

		if( !pTexture->IsLoadedFromFile() || !pTexture->IsLoaded() )
			continue;

		TextureQuality eQuality = max( pTexture->GetMinimumQuality(), m_TextureQuality );

		D3DFORMAT eFormat = CheckFileFormatByName( pTexture->GetName() );
		if( eQuality != TQ_HIGH )
		{
			eFormat = ChangeTo16BitForamt( eFormat );
		}

		pTexture->SetFormat( eFormat );
		ReLoadImpl( pTexture );
	}
}

void ioTextureManager::ChangeTextureQuality( ioTexture *pTexture, TextureQuality eQuality )
{
	if( !pTexture ) return;

	TextureQuality ePrevQuality = m_TextureQuality;
	{
		m_TextureQuality = max( pTexture->GetMinimumQuality(), eQuality );
		D3DFORMAT eFormat = CheckFileFormatByName( pTexture->GetName() );
		if( eQuality != TQ_HIGH )
		{
			eFormat = ChangeTo16BitForamt( eFormat );
		}

		pTexture->SetFormat( eFormat );
		ReLoadImpl( pTexture );
	}
	m_TextureQuality = ePrevQuality;
}

bool ioTextureManager::IsCustomTexture( const ioHashString &szFileName )
{
	int iNameLen = szFileName.Length();
	if( iNameLen < 4 )
		return false;

	char szExt[4];
	const char *szSrc = szFileName.c_str();
	szExt[0] = szSrc[iNameLen - 3];
	szExt[1] = szSrc[iNameLen - 2];
	szExt[2] = szSrc[iNameLen - 1];
	szExt[3] = '\0';
	strlwr( szExt );
	if( !strcmp( szExt, "lsc" ) )
		return true;
	return false;
}

ioTexture* ioTextureManager::CreateTexture( const ioHashString &name,
											DWORD dwUsage,
											int iMipMapLevelCnt,
											TextureQuality eMinimumQuality,
											bool bThread,
											bool bDefaultPath,
											bool bLoadPiece )
{
	ioTexture *pTexture = GetTextureByName( name );
	if( pTexture )
	{
		pTexture->AddRef();
		return pTexture;
	}

	// 무조건 낱개 파을을 로드한다.
	if( IsCustomTexture( name ) )
	{
		bLoadPiece   = true;
	}

	char szFullPath[MAX_PATH];
	if( bDefaultPath )
		wsprintf( szFullPath, "%s/%s", m_szStartDir, name.c_str() );
	else
		strcpy( szFullPath, name.c_str() );

	D3DFORMAT eFormat = CheckFileFormatByName( name );

	TextureQuality eCurQuality = max( m_TextureQuality, eMinimumQuality );
	if( eCurQuality != TQ_HIGH )
	{
		eFormat = ChangeTo16BitForamt( eFormat );
	}

	pTexture = dynamic_cast< ioTexture* >( Create( name, bThread ) );
	pTexture->SetFormat( eFormat );
	pTexture->SetUsage( dwUsage );
	pTexture->SetMipMapLevel( iMipMapLevelCnt );
	pTexture->SetDefaultPath( bDefaultPath );
	pTexture->SetLoadPiece( bLoadPiece );
	pTexture->SetLoadedFromFile( true );
	pTexture->SetMinimumQuality( eMinimumQuality );

	LoadFile( szFullPath, pTexture, bThread, bLoadPiece );

	// 화면으로 문제점을 확인해야하니까 실패한것이라도 리턴한다.
	return pTexture;
}

ioTexture* ioTextureManager::CreateManual( const ioHashString &name,
										   int iWidth,
										   int iHeight,
										   D3DFORMAT fmt,
										   DWORD dwUsage,
										   int iMipMapLevelCnt )
{
	ioTexture *pTexture = GetTextureByName( name );
	if( pTexture )
	{
		pTexture->AddRef();
		return pTexture;
	}

	D3DPOOL ePool = D3DPOOL_MANAGED;
	if( dwUsage == D3DUSAGE_DYNAMIC || dwUsage == D3DUSAGE_RENDERTARGET )
		ePool = D3DPOOL_DEFAULT;

	IDirect3DTexture9 *pSrcTex = CreateD3DTexture( iWidth,
												   iHeight,
												   iMipMapLevelCnt,
												   dwUsage,
												   fmt,
												   ePool );

	if( !pSrcTex )
	{
		LOG.PrintTimeAndLog( 0, " %s CreateManual Failed", name.c_str() );
		return NULL;
	}

	pTexture = dynamic_cast< ioTexture* >( Create( name ) );
	pTexture->SetFormat( fmt );
	pTexture->SetUsage( dwUsage );
	pTexture->SetTexture( pSrcTex );
	pTexture->SetSize( iWidth, iHeight );
	pTexture->SetLoadedFromFile( false );
	pTexture->SetLoadState( RLS_LOADED );
	pTexture->SetMinimumQuality( TQ_HIGH );

	return pTexture;
}

ioRenderTexture* ioTextureManager::CreateRTT( const ioHashString &name,
											  int iWidth,
											  int iHeight,
											  D3DFORMAT eFormat,
											  D3DFORMAT eDepthFormat,
											  bool bUseDepthTexture )
{
	ioTexture *pTexture = GetTextureByName( name );
	if( pTexture )
	{
		pTexture->AddRef();
		return dynamic_cast< ioRenderTexture* >( pTexture );
	}

	IDirect3DTexture9 *pRenderTex = CreateD3DTexture( iWidth,
													  iHeight,
													  1,
													  D3DUSAGE_RENDERTARGET,
													  eFormat,
													  D3DPOOL_DEFAULT );

	if( !pRenderTex )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::CreateRTT - %s RenderTex Failed", name.c_str() );
		return NULL;
	}

	ioRenderTexture *pRTT = new ioRenderTexture( name );
	RegisterImpl( pRTT );
	pRTT->AddRef();
	pRTT->SetErasePolicy( EP_NO_ERASE );

	pRTT->SetFormat( eFormat );
	pRTT->SetMipMapLevel( 1 );
	pRTT->SetUsage( D3DUSAGE_RENDERTARGET );
	pRTT->SetTexture( pRenderTex );
	pRTT->SetSize( iWidth, iHeight );
	pRTT->SetLoadedFromFile( false );
	pRTT->SetLoadState( RLS_LOADED );
	pRTT->SetMinimumQuality( TQ_HIGH );

	if( eDepthFormat == D3DFMT_UNKNOWN )	// Depth Surface가 필요없는 경우
		return pRTT;

	pRTT->SetDepthFormat( eDepthFormat );

	if( bUseDepthTexture )
	{
		IDirect3DTexture9 *pDepthTex = CreateD3DTexture( iWidth,
														 iHeight,
														 1,
														 D3DUSAGE_DEPTHSTENCIL,
														 eDepthFormat,
														 D3DPOOL_DEFAULT );
		if( pDepthTex )
		{
			pRTT->SetDepthTexture( pDepthTex );
		}
	}
	else
	{
		IDirect3DSurface9 *pDepthSurf = NULL;
		HRESULT hr = m_pD3DDevice->CreateDepthStencilSurface( iWidth,
															  iHeight,
															  eDepthFormat,
															  D3DMULTISAMPLE_NONE,
															  0,
															  TRUE,
															  &pDepthSurf,
															  NULL );

		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "CreateRTT: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioTextureManager::CreateRTT - %s DepthSurface Failed(%x)",
									name.c_str(), hr );
		}
		else
		{
			pRTT->SetDepthSurface( pDepthSurf );
		}
	}

	return pRTT;
}

bool ioTextureManager::DestroyRTTIfNoRef( ioRenderTexture *pRTT )
{
	if( !pRTT )	return false;
	if( pRTT->Release() > 0 )	return false;

	m_Resource.erase( pRTT->GetName() );
	delete pRTT;

	return true;
}

ioTexture* ioTextureManager::GetTextureByName( const ioHashString &name )
{
	return dynamic_cast< ioTexture* >( GetByName( name ) );
}

void ioTextureManager::DestroyByName( const ioHashString &kName, bool bForceDestroy /* = false  */ )
{
	ioResource *pRes = GetByName( kName );
	if( pRes )
	{
		if( bForceDestroy )
			ForceDestroy( pRes );
		else
            Destroy( pRes );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::DestroyByName - %s Not Exist", kName.c_str() );
	}
}

D3DFORMAT ioTextureManager::ChangeTo16BitForamt( D3DFORMAT eFormat )
{
	switch( eFormat )
	{
	case D3DFMT_R8G8B8:
	case D3DFMT_X8R8G8B8:
		return D3DFMT_R5G6B5;
	case D3DFMT_A8R8G8B8:
		return D3DFMT_A4R4G4B4;
	}

	return eFormat;
}

D3DFORMAT ioTextureManager::CheckFileFormatByName( const ioHashString &kName )
{
	int iNameLen = kName.Length();
	if( iNameLen < 4 )
		return D3DFMT_UNKNOWN;

	char szExt[4];
	const char *szSrc = kName.c_str();
	szExt[0] = szSrc[iNameLen - 3];
	szExt[1] = szSrc[iNameLen - 2];
	szExt[2] = szSrc[iNameLen - 1];
	szExt[3] = '\0';
	strlwr( szExt );

	if( !strcmp( szExt, "tga" ) || !strcmp( szExt, "png" ) )
		return D3DFMT_A8R8G8B8;
	else if( !strcmp( szExt, "bmp" ) || !strcmp( szExt, "jpg" ) || !strcmp( szExt, "gif" ) )
		return D3DFMT_X8R8G8B8;
	return D3DFMT_UNKNOWN;
}

bool ioTextureManager::LoadFile( const char *szFileName, ioTexture *pTexture, bool bThread, bool bLoadPiece )
{
	bool bLoaded = true;

	if( bThread )
	{
		pTexture->SetLoadState( RLS_LOADING );

		ioBinaryStream *pStream = new ioBinaryStream;
		g_ResourceLoader.LoadStream( szFileName, pStream, this, pTexture, bLoadPiece );
	}
	else
	{
		ioBinaryStream kStream;
		if( !g_ResourceLoader.LoadStream( szFileName, &kStream, NULL, NULL, bLoadPiece ) )
		{
			pTexture->SetLoadState( RLS_FAILED );

			LOG.PrintTimeAndLog( 0, "ioTextureManager::LoadFile - %s Load Failed",szFileName);
			return false;
		}

		bLoaded = LoadFileInternal( pTexture, &kStream );
		if( bLoaded )
		{
			pTexture->SetLoadState( RLS_LOADED );
		}
		else
		{
			pTexture->SetLoadState( RLS_FAILED );
		}

		if( bLoaded )
		{
			AddMem( pTexture->GetName(), kStream.GetSize() );
		}
	}

	return bLoaded;
}

bool ioTextureManager::LoadFileInternal( ioTexture *pTexture, ioStream *pStream )
{
	if( IsCustomTexture( pTexture->GetName() ) )
	{
		pStream->DecompressLSCFile();
	}

	D3DPOOL ePool = D3DPOOL_MANAGED;
	if( pTexture->GetUsage() == D3DUSAGE_DYNAMIC )
		ePool = D3DPOOL_DEFAULT;

	IDirect3DTexture9 *pSrcTex = NULL;
	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx( m_pD3DDevice,
													  pStream->GetPtr(),
													  pStream->GetSize(),
													  0,
													  0,
													  pTexture->GetMipMapLevelCnt(),
													  pTexture->GetUsage(),
													  pTexture->GetFormat(),
													  ePool,
													  D3DX_FILTER_NONE,
													  D3DX_DEFAULT,
													  0,
													  NULL,
													  NULL,
													  &pSrcTex );

	if( hr == D3DERR_OUTOFVIDEOMEMORY || hr == E_OUTOFMEMORY )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::LoadFileInternal - Evict and Retry" );

		if( SUCCEEDED( m_pD3DDevice->EvictManagedResources() ) )
		{
			hr = D3DXCreateTextureFromFileInMemoryEx( m_pD3DDevice,
													  pStream->GetPtr(),
													  pStream->GetSize(),
													  0,
													  0,
													  pTexture->GetMipMapLevelCnt(),
													  pTexture->GetUsage(),
													  pTexture->GetFormat(),
													  ePool,
													  D3DX_FILTER_NONE,
													  D3DX_DEFAULT,
													  0,
													  NULL,
													  NULL,
													  &pSrcTex );
		}
	}

	if( SUCCEEDED(hr) )
	{
		D3DSURFACE_DESC kSurface;
		pSrcTex->GetLevelDesc( 0, &kSurface );
		pTexture->SetFormat( kSurface.Format );
		pTexture->SetSize( kSurface.Width, kSurface.Height );
		pTexture->SetTexture( pSrcTex );

		TextureQuality eCurQuality = max( m_TextureQuality, pTexture->GetMinimumQuality() );
		if( eCurQuality == TQ_LOW )
		{
			DownSizingToLowQuality( pTexture, ePool );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::LoadFileInternal - %s Cannot Load(%x)",
							    pTexture->GetName().c_str(), hr );

		hr = m_pD3DDevice->CreateTexture( 2,
										  2,
										  1,
										  pTexture->GetUsage(),
										  D3DFMT_A8R8G8B8,
										  D3DPOOL_MANAGED,
										  &pSrcTex,
										  NULL );

		pTexture->SetSize( 2, 2 );
		pTexture->SetFormat( D3DFMT_A8R8G8B8 );
		pTexture->SetTexture( pSrcTex );

		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "LoadFileInternal: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioTextureManager::LoadFileInternal - Temp Texture Failed(%x)", hr );
			return false;
		}
	}

	if( pTexture->HasGrayTexture() )
	{
		BuildGrayTexture( pTexture );
	}

	if( pTexture->HasGrayLightTexture() )
	{
		BuildGrayLightTexture( pTexture );
	}

	return true;
}

void ioTextureManager::DownSizingToLowQuality( ioTexture *pTexObj, D3DPOOL ePool )
{
	int iSrcWidth  = pTexObj->GetWidth();
	int iSrcHeight = pTexObj->GetHeight();

	if( iSrcWidth <= 4 || iSrcHeight <= 4 )
		return;

	IDirect3DTexture9 *pSrcTex = pTexObj->GetTexture();

	int iSrcMipLevel = (int)pSrcTex->GetLevelCount();
	if( iSrcMipLevel < 2 )
		return;

	int iNewWidth  = iSrcWidth >> 1;
	int iNewHeight = iSrcHeight >> 1;

	int iTargetLv = 1;
/*	for( ; iTargetLv<iSrcMipLevel ; iTargetLv++ )
	{
		if( iNewWidth > 512 || iNewHeight > 512 )
		{
			iNewWidth  >>= 1;
			iNewHeight >>= 1;
		}
		else
		{
			break;
		}
	}
*/
	iNewWidth  = max( 1, iNewWidth );
	iNewHeight = max( 1, iNewHeight );

	HRESULT hr;
	DWORD dwUsage = pTexObj->GetUsage();

	if( ioRenderSystem::m_D3DCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP )
	{
		dwUsage |= D3DUSAGE_AUTOGENMIPMAP;

		IDirect3DTexture9 *pNewTex = CreateD3DTexture( iNewWidth,
													   iNewHeight,
													   1,
													   dwUsage,
													   pTexObj->GetFormat(),
													   ePool );

		if( !pNewTex )
		{
			LOG.PrintTimeAndLog( 0, "ioTextureManager::DownSizingToLowQuality - NewTextureCreate Failed(%s)", pTexObj->GetName().c_str() );
			return;
		}

		IDirect3DSurface9 *pSrcFace = NULL;
		hr = pSrcTex->GetSurfaceLevel( iTargetLv, &pSrcFace );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "DownSizingToLowQuality1: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioTextureManager::DownSizingToLowQuality - GetSrc Failed(%x)", hr );
			SAFERELEASE( pNewTex );
			return;
		}

		IDirect3DSurface9 *pDestFace = NULL;
		hr = pNewTex->GetSurfaceLevel( 0, &pDestFace );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "DownSizingToLowQuality2: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioTextureManager::DownSizingToLowQuality - GetDest Failed(%x)", hr );
			SAFERELEASE( pSrcFace );
			SAFERELEASE( pNewTex );
		}

		hr = D3DXLoadSurfaceFromSurface( pDestFace, NULL, NULL, pSrcFace, NULL, NULL, D3DX_FILTER_NONE, 0 );

		//어짜피 release할거라서 fail체크전에 지워버린다.
		SAFERELEASE( pSrcFace );
		SAFERELEASE( pDestFace );

		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "DownSizingToLowQuality3: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioTextureManager::DownSizingToLowQuality - Copy Failed(%x)", hr );
		}
		else
		{
			pNewTex->SetAutoGenFilterType( D3DTEXF_LINEAR );
			pNewTex->GenerateMipSubLevels();

			// HARDCODE: 내부에서 기존에 가지고있던 pSrcTex가 Release되므로 여기서 pSrcTex를 Release하면 안됨
			pTexObj->SetTexture( pNewTex );
			pTexObj->SetSize( iNewWidth, iNewHeight );
			pNewTex = NULL;
		}
	}
	else
	{
		IDirect3DTexture9 *pNewTex = CreateD3DTexture( iNewWidth,
													   iNewHeight,
													   0,
													   dwUsage,
													   pTexObj->GetFormat(),
													   ePool );

		if( !pNewTex )
		{
			LOG.PrintTimeAndLog( 0, "ioTextureManager::DownSizingToLowQuality - NewTextureCreate Failed" );
			return;
		}

		int iMipLevel = pNewTex->GetLevelCount();
		for( int i=0 ; i<iMipLevel ; i++ )
		{
			IDirect3DSurface9 *pSrcFace;
			hr = pSrcTex->GetSurfaceLevel( iTargetLv + i, &pSrcFace );
			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "DownSizingToLowQuality4: %x", hr );
				SAFERELEASE( pNewTex );
				LOG.PrintTimeAndLog( 0, "ioTextureManager::DownSizingToLowQuality - Src %d GetLevel Failed(%x)", i+1, hr );
				return;
			}

			IDirect3DSurface9 *pDestFace;
			hr = pNewTex->GetSurfaceLevel( i, &pDestFace );
			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "DownSizingToLowQuality5: %x", hr );
				SAFERELEASE( pSrcFace );
				SAFERELEASE( pNewTex );
				LOG.PrintTimeAndLog( 0, "ioTextureManager::DownSizingToLowQuality - Dest %d GetLevel Failed(%x)", i, hr );
				return;
			}

			hr = D3DXLoadSurfaceFromSurface( pDestFace, NULL, NULL, pSrcFace, NULL, NULL, D3DX_FILTER_NONE, 0 );

			//어짜피 release할거라서 fail체크전에 지워버린다.
			SAFERELEASE( pSrcFace );
			SAFERELEASE( pDestFace );

			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "DownSizingToLowQuality5: %x", hr );
				SAFERELEASE( pNewTex );
				LOG.PrintTimeAndLog( 0, "ioTextureManager::DownSizingToLowQuality - %d Level Copy Failed(%x)", i, hr );
				return;
			}
		}

		// HARDCODE: 내부에서 기존에 가지고있던 pSrcTex가 Release되므로 여기서 pSrcTex를 Release하면 안됨
		pTexObj->SetTexture( pNewTex );
		pTexObj->SetSize( iNewWidth, iNewHeight );
		pNewTex = NULL;
	}
}

IDirect3DTexture9* ioTextureManager::CreateD3DTexture( int iWidth,
													   int iHeight,
													   int iMipMap,
													   DWORD dwUsage,
													   D3DFORMAT fmt,
													   D3DPOOL ePool )
{
	if( dwUsage == D3DUSAGE_DYNAMIC )
	{
		if( !( ioRenderSystem::m_D3DCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES ) )
		{
			dwUsage = 0;
			ePool = D3DPOOL_MANAGED;
		}
	}

	IDirect3DTexture9 *pTexture = NULL;
	HRESULT hr = m_pD3DDevice->CreateTexture( iWidth,
											  iHeight,
											  iMipMap,
											  dwUsage,
											  fmt,
											  ePool,
											  &pTexture,
											  NULL );

	if( hr == D3DERR_OUTOFVIDEOMEMORY || hr == E_OUTOFMEMORY )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::CreateD3DTexture - Evict and Retry" );

		if( SUCCEEDED( m_pD3DDevice->EvictManagedResources() ) )
		{
			hr = m_pD3DDevice->CreateTexture( iWidth,
											  iHeight,
											  iMipMap,
											  dwUsage,
											  fmt,
											  ePool,
											  &pTexture,
											  NULL );
		}
	}

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "CreateD3DTexture: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::CreateD3DTexture - Failed(%x)", hr );
	}

	return pTexture;
}

void ioTextureManager::ReleaseDefaultPoolTextures()
{
	ResourceMap::iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioTexture *pTexture = dynamic_cast< ioTexture* >( iter->second );
		if( pTexture )
		{
			pTexture->ReleaseIfDefaultPool();
		}
	}
}

void ioTextureManager::ReCreateDefaultPoolTextures()
{
	ResourceMap::iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioTexture *pTexture = dynamic_cast< ioTexture* >( iter->second );

		if( pTexture->GetUsage() != D3DUSAGE_DYNAMIC &&
			pTexture->GetUsage() != D3DUSAGE_RENDERTARGET )
			continue;

		if( pTexture->IsLoadedFromFile() )
		{
			ReLoadImpl( pTexture );
		}
		else
		{
			IDirect3DTexture9 *pSrcTex = CreateD3DTexture( pTexture->GetWidth(),
														   pTexture->GetHeight(),
														   pTexture->GetMipMapLevelCnt(),
														   pTexture->GetUsage(),
														   pTexture->GetFormat(),
														   D3DPOOL_DEFAULT );
			if( pSrcTex )
			{
				pTexture->SetTexture( pSrcTex );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioTextureManager::ReCreateDefaultPoolTextures - (%s) ReCreate Failed",
										pTexture->GetName().c_str() );
			}

			if( pTexture->GetUsage() == D3DUSAGE_RENDERTARGET )
			{
				ioRenderTexture *pRTT = dynamic_cast< ioRenderTexture* >( pTexture );

				if( pRTT->GetDepthFormat() == D3DFMT_UNKNOWN )
					continue;

				if( pRTT->IsUseDepthTexture() )
				{
					IDirect3DTexture9 *pDepthTex = CreateD3DTexture( pRTT->GetWidth(),
																	 pRTT->GetHeight(),
																	 1,
																	 D3DUSAGE_DEPTHSTENCIL,
																	 pRTT->GetDepthFormat(),
																	 D3DPOOL_DEFAULT );
					if( pDepthTex )
					{
						pRTT->SetDepthTexture( pDepthTex );
					}
				}
				else
				{
					IDirect3DSurface9 *pDepthSurf = NULL;
					HRESULT hr = m_pD3DDevice->CreateDepthStencilSurface( pRTT->GetWidth(),
																		  pRTT->GetHeight(),
																		  pRTT->GetDepthFormat(),
																		  D3DMULTISAMPLE_NONE ,
																		  0,
																		  TRUE,
																		  &pDepthSurf,
																		  NULL );

					if( FAILED(hr) )
					{
						ErrorReport::SetFormat( "ReCreateDefaultPoolTextures: %x", hr );
						LOG.PrintTimeAndLog( 0, "ioTextureManager::ReCreateDefaultPoolTextures - %s DepthSurface Failed(%x)",
												 pRTT->GetName().c_str(), hr );
					}
					else
					{
						pRTT->SetDepthSurface( pDepthSurf );
					}
				}
			}
		}
	}
}

void ioTextureManager::BuildGrayTexture( const ioTexture *pTexture )
{
	if( !pTexture || !pTexture->GetTexture() )
		return;

	IDirect3DTexture9 *pGrayTex = CreateD3DTexture( pTexture->GetWidth(),
												    pTexture->GetHeight(),
												    1,
												    0,
												    pTexture->GetFormat(),
												    D3DPOOL_MANAGED );

	if( !pGrayTex )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::BuildGrayTexture - %s Build Failed",
								pTexture->GetName().c_str() );
		return;
	}

	switch( pTexture->GetFormat() )
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		GrayA8R8G8B8Filter( pTexture, pGrayTex, m_iGrayScaleRate );
		break;
	case D3DFMT_R8G8B8:
		GrayR8G8B8Filter( pTexture, pGrayTex, m_iGrayScaleRate );
		break;
	case D3DFMT_A4R4G4B4:
		GrayA4R4G4B4Filter( pTexture, pGrayTex, m_iGrayScaleRate );
		break;
	case D3DFMT_A1R5G5B5:
		GrayA1R5G5B5Filter( pTexture, pGrayTex, m_iGrayScaleRate );
		break;
	case D3DFMT_R5G6B5:
		GrayR5G6B5Filter( pTexture, pGrayTex, m_iGrayScaleRate );
		break;
	case D3DFMT_DXT1:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		GrayDDSFilter( pTexture, pGrayTex, m_iGrayScaleRate );
		break;
	}

	pTexture->SetGrayTexture( pGrayTex );
}

void ioTextureManager::BuildGrayLightTexture( const ioTexture *pTexture )
{
	if( !pTexture || !pTexture->GetTexture() )
		return;

	IDirect3DTexture9 *pGrayTex = CreateD3DTexture( pTexture->GetWidth(),
		pTexture->GetHeight(),
		1,
		0,
		pTexture->GetFormat(),
		D3DPOOL_MANAGED );

	if( !pGrayTex )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::BuildGrayLightTexture - %s Build Failed",
			pTexture->GetName().c_str() );
		return;
	}

	switch( pTexture->GetFormat() )
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		GrayA8R8G8B8Filter( pTexture, pGrayTex, m_iGrayLightScaleRate );
		break;
	case D3DFMT_R8G8B8:
		GrayR8G8B8Filter( pTexture, pGrayTex, m_iGrayLightScaleRate );
		break;
	case D3DFMT_A4R4G4B4:
		GrayA4R4G4B4Filter( pTexture, pGrayTex, m_iGrayLightScaleRate );
		break;
	case D3DFMT_A1R5G5B5:
		GrayA1R5G5B5Filter( pTexture, pGrayTex, m_iGrayLightScaleRate );
		break;
	case D3DFMT_R5G6B5:
		GrayR5G6B5Filter( pTexture, pGrayTex, m_iGrayLightScaleRate );
		break;
	case D3DFMT_DXT1:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		GrayDDSFilter( pTexture, pGrayTex, m_iGrayLightScaleRate );
		break;
	}

	pTexture->SetGrayLightTexture( pGrayTex );
}

void ioTextureManager::GrayA8R8G8B8Filter( const ioTexture *pRcSrc, IDirect3DTexture9 *pGrayTex, int iScaleRate )
{
	HRESULT hr;
	D3DLOCKED_RECT rcSrcRect;

	IDirect3DTexture9 *pSrcTex = pRcSrc->GetTexture();
	hr = pSrcTex->LockRect( 0, &rcSrcRect, NULL, D3DLOCK_READONLY );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayA8R8G8B8Filter1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayA8R8G8B8Filter - %s Src Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		return;
	}

	D3DLOCKED_RECT rcDestRect;
	hr = pGrayTex->LockRect( 0, &rcDestRect, NULL, D3DLOCK_NOSYSLOCK );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayA8R8G8B8Filter2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayA8R8G8B8Filter - %s Gray Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		pSrcTex->UnlockRect( 0 );
		return;
	}

	DWORD *pSrc  = (DWORD*)rcSrcRect.pBits;
	DWORD *pDest = (DWORD*)rcDestRect.pBits;

	int iSrcPitch  = rcSrcRect.Pitch >> 2;
	int iDestPitch = rcDestRect.Pitch >> 2;

	int iWidth = pRcSrc->GetWidth();
	int iHeight = pRcSrc->GetHeight();

	DWORD dwAlpha, dwRed, dwGreen, dwBlue, dwSrcColor, dwGray;
	for( int iY=0 ; iY<iHeight ; iY++ )
	{
		for( int iX=0 ; iX<iWidth ; iX++ )
		{
			dwSrcColor = pSrc[iX];
			dwAlpha = ( dwSrcColor & 0xff000000 );
			dwRed	= ( dwSrcColor & 0x00ff0000 ) >> 16;
			dwGreen = ( dwSrcColor & 0x0000ff00 ) >> 8;
			dwBlue  = ( dwSrcColor & 0x000000ff );

			dwGray = ( dwRed * 30 + dwGreen * 59 + dwBlue * 11 ) / iScaleRate;

			pDest[iX] = dwAlpha | ( dwGray << 16 ) | ( dwGray << 8 ) | dwGray;
		}

		pSrc += iSrcPitch;
		pDest += iDestPitch;
	}

	pSrcTex->UnlockRect( 0 );
	pGrayTex->UnlockRect( 0 );
}

void ioTextureManager::GrayR8G8B8Filter( const ioTexture *pRcSrc, IDirect3DTexture9 *pGrayTex, int iScaleRate )
{
	HRESULT hr;
	D3DLOCKED_RECT rcSrcRect;

	IDirect3DTexture9 *pSrcTex = pRcSrc->GetTexture();
	hr = pSrcTex->LockRect( 0, &rcSrcRect, NULL, D3DLOCK_READONLY );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayR8G8B8Filter1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayR8G8B8Filter - %s Src Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		return;
	}

	D3DLOCKED_RECT rcDestRect;
	hr = pGrayTex->LockRect( 0, &rcDestRect, NULL, D3DLOCK_NOSYSLOCK );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayR8G8B8Filter2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayR8G8B8Filter - %s Gray Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		pSrcTex->UnlockRect( 0 );
		return;
	}

	BYTE *pSrc  = (BYTE*)rcSrcRect.pBits;
	BYTE *pDest = (BYTE*)rcDestRect.pBits;

	int iWidth = pRcSrc->GetWidth();
	int iHeight = pRcSrc->GetHeight();

	int iRed, iGreen, iBlue, iGray;
	for( int iY=0 ; iY<iHeight ; iY++ )
	{
		int iWBytes = 0;
		for( int iX=0 ; iX<iWidth ; iX++ )
		{
			iRed	= pSrc[ iWBytes+0 ];
			iGreen	= pSrc[ iWBytes+1 ];
			iBlue	= pSrc[ iWBytes+2 ];

			iGray = ( iRed * 30 + iGreen * 59 + iBlue * 11 ) / iScaleRate;

			pDest[ iWBytes+0 ] = iGray;
			pDest[ iWBytes+1 ] = iGray;
			pDest[ iWBytes+2 ] = iGray;

			iWBytes += 3;
		}

		pSrc += rcSrcRect.Pitch;
		pDest += rcDestRect.Pitch;
	}

	pSrcTex->UnlockRect( 0 );
	pGrayTex->UnlockRect( 0 );
}

void ioTextureManager::GrayA4R4G4B4Filter( const ioTexture *pRcSrc, IDirect3DTexture9 *pGrayTex, int iScaleRate )
{
	HRESULT hr;
	D3DLOCKED_RECT rcSrcRect;

	IDirect3DTexture9 *pSrcTex = pRcSrc->GetTexture();
	hr = pSrcTex->LockRect( 0, &rcSrcRect, NULL, D3DLOCK_READONLY );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayA4R4G4B4Filter1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayA4R4G4B4Filter - %s Src Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		return;
	}

	D3DLOCKED_RECT rcDestRect;
	hr = pGrayTex->LockRect( 0, &rcDestRect, NULL, D3DLOCK_NOSYSLOCK );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayA4R4G4B4Filter2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayA4R4G4B4Filter - %s Gray Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		pSrcTex->UnlockRect( 0 );
		return;
	}

	WORD *pSrc  = (WORD*)rcSrcRect.pBits;
	WORD *pDest = (WORD*)rcDestRect.pBits;

	int iSrcPitch  = rcSrcRect.Pitch >> 1;
	int iDestPitch = rcDestRect.Pitch >> 1;
	int iWidth = pRcSrc->GetWidth();
	int iHeight = pRcSrc->GetHeight();

	WORD wAlpha, wRed, wGreen, wBlue, wSrcColor, wGray;
	for( int iY=0 ; iY<iHeight ; iY++ )
	{
		for( int iX=0 ; iX<iWidth ; iX++ )
		{
			wSrcColor = pSrc[iX];
			wAlpha = ( wSrcColor & 0xf000 );
			wRed   = ( wSrcColor & 0x0f00 ) >> 8;
			wGreen = ( wSrcColor & 0x00f0 ) >> 4;
			wBlue  = ( wSrcColor & 0x000f );

			wGray  = ( wRed * 30 + wGreen * 59 + wBlue * 11 ) / iScaleRate;

			pDest[iX] = wAlpha | ( wGray << 8 ) | ( wGray << 4 ) | wGray;
		}

		pSrc += iSrcPitch;
		pDest += iDestPitch;
	}

	pSrcTex->UnlockRect( 0 );
	pGrayTex->UnlockRect( 0 );
}

void ioTextureManager::GrayR5G6B5Filter( const ioTexture *pRcSrc, IDirect3DTexture9 *pGrayTex, int iScaleRate )
{
	HRESULT hr;
	D3DLOCKED_RECT rcSrcRect;

	IDirect3DTexture9 *pSrcTex = pRcSrc->GetTexture();
	hr = pSrcTex->LockRect( 0, &rcSrcRect, NULL, D3DLOCK_READONLY );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayR5G6B5Filter1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayR5G6B5Filter - %s Src Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		return;
	}

	D3DLOCKED_RECT rcDestRect;
	hr = pGrayTex->LockRect( 0, &rcDestRect, NULL, D3DLOCK_NOSYSLOCK );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayR5G6B5Filter2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayR5G6B5Filter - %s Gray Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		pSrcTex->UnlockRect( 0 );
		return;
	}

	WORD *pSrc  = (WORD*)rcSrcRect.pBits;
	WORD *pDest = (WORD*)rcDestRect.pBits;

	int iSrcPitch  = rcSrcRect.Pitch >> 1;
	int iDestPitch = rcDestRect.Pitch >> 1;
	int iWidth = pRcSrc->GetWidth();
	int iHeight = pRcSrc->GetHeight();

	WORD wRed, wGreen, wBlue, wSrcColor, wGray6, wGray5;
	for( int iY=0 ; iY<iHeight ; iY++ )
	{
		for( int iX=0 ; iX<iWidth ; iX++ )
		{
			wSrcColor = pSrc[iX];
			wRed   = ( wSrcColor & 0xf800 ) >> 10;
			wGreen = ( wSrcColor & 0x07e0 ) >> 5;
			wBlue  = ( wSrcColor & 0x001f ) << 1;

			wGray6 = ( wRed * 30 + wGreen * 59 + wBlue * 11 ) / iScaleRate;
			wGray5 = wGray6 >> 1;

			pDest[iX] = ( wGray5 << 11 ) | ( wGray6 << 5 ) | wGray5;
		}

		pSrc  += iSrcPitch;
		pDest += iDestPitch;
	}
	
	pSrcTex->UnlockRect( 0 );
	pGrayTex->UnlockRect( 0 );
}

void ioTextureManager::GrayA1R5G5B5Filter( const ioTexture *pRcSrc, IDirect3DTexture9 *pGrayTex, int iScaleRate )
{
	HRESULT hr;
	D3DLOCKED_RECT rcSrcRect;

	IDirect3DTexture9 *pSrcTex = pRcSrc->GetTexture();
	hr = pSrcTex->LockRect( 0, &rcSrcRect, NULL, D3DLOCK_READONLY );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayA1R5G5B5Filter1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayA1R5G5B5Filter - %s Src Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		return;
	}

	D3DLOCKED_RECT rcDestRect;
	hr = pGrayTex->LockRect( 0, &rcDestRect, NULL, D3DLOCK_NOSYSLOCK );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayA1R5G5B5Filter2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayA1R5G5B5Filter - %s Gray Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		pSrcTex->UnlockRect( 0 );
		return;
	}

	WORD *pSrc  = (WORD*)rcSrcRect.pBits;
	WORD *pDest = (WORD*)rcDestRect.pBits;

	int iSrcPitch  = rcSrcRect.Pitch >> 1;
	int iDestPitch = rcDestRect.Pitch >> 1;
	int iWidth = pRcSrc->GetWidth();
	int iHeight = pRcSrc->GetHeight();

	WORD wRed, wGreen, wBlue, wSrcColor, wGray;
	for( int iY=0 ; iY<iHeight ; iY++ )
	{
		for( int iX=0 ; iX<iWidth ; iX++ )
		{
			wSrcColor = pSrc[iX];
			wRed   = ( wSrcColor & 0x7c00 ) >> 10;
			wGreen = ( wSrcColor & 0x03e0 ) >> 5;
			wBlue  = ( wSrcColor & 0x001f );

			wGray  = ( wRed * 30 + wGreen * 59 + wBlue * 11 ) / iScaleRate;

			pDest[iX] = ( wSrcColor & 8000 ) | ( wGray << 10 ) | ( wGray << 5 ) | wGray;
		}

		pSrc += iSrcPitch;
		pDest += iDestPitch;
	}

	pSrcTex->UnlockRect( 0 );
	pGrayTex->UnlockRect( 0 );
}

void ioTextureManager::GrayDDSFilter( const ioTexture *pRcSrc, IDirect3DTexture9 *pGrayTex, int iScaleRate )
{
	HRESULT hr;

	// Creat Temp Surface
	IDirect3DTexture9 *pTempTex = NULL;
	pTempTex = CreateD3DTexture( pRcSrc->GetWidth(),
								 pRcSrc->GetHeight(),
								 1,
								 0,
								 D3DFMT_A4R4G4B4,
								 D3DPOOL_MANAGED );
	if( !pTempTex )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayDDSFilter - %s Temp Texture Failed",
								pRcSrc->GetName().c_str() );
		return;
	}
	
	IDirect3DSurface9 *pSrcSurf = NULL;
	hr = pRcSrc->GetTexture()->GetSurfaceLevel( 0, &pSrcSurf );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayDDSFilter1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayDDSFilter - %s GetSurfaceFailed(%x)",
								pRcSrc->GetName().c_str(), hr );
		SAFERELEASE( pTempTex );
		return;
	}

	IDirect3DSurface9 *pTempSurf = NULL;
	hr = pTempTex->GetSurfaceLevel( 0, &pTempSurf );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayDDSFilter2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayDDSFilter - %s Temp GetSurfaceFailed(%x)",
								pRcSrc->GetName().c_str(), hr );
		SAFERELEASE( pSrcSurf );
		SAFERELEASE( pTempTex );
		return;
	}

	// DeCompress
	hr = D3DXLoadSurfaceFromSurface( pTempSurf, NULL, NULL,
									 pSrcSurf, NULL, NULL,
									 D3DX_FILTER_NONE, 0 );

	SAFERELEASE( pSrcSurf );

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayDDSFilter3: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayDDSFilter - %s Temp LoadSurfaceFailed(%x)",
								pRcSrc->GetName().c_str(), hr );
		SAFERELEASE( pTempSurf );
		SAFERELEASE( pTempTex );
		return;
	}

	// Gray Scale..
	D3DLOCKED_RECT rcLock;
	hr = pTempSurf->LockRect( &rcLock, NULL, D3DLOCK_NOSYSLOCK );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayDDSFilter4: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayDDSFilter - %s Dest Lock Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
		SAFERELEASE( pTempSurf );
		SAFERELEASE( pTempTex );
		return;
	}

	WORD *pBuf = (WORD*)rcLock.pBits;
	int iPitch = rcLock.Pitch >> 1;

	int iWidth = pRcSrc->GetWidth();
	int iHeight = pRcSrc->GetHeight();

	WORD wAlpha, wRed, wGreen, wBlue, wSrcColor, wGray;
	for( int iY=0 ; iY<iHeight ; iY++ )
	{
		for( int iX=0 ; iX<iWidth ; iX++ )
		{
			wSrcColor = pBuf[iX];
			wAlpha = ( wSrcColor & 0xf000 );
			wRed   = ( wSrcColor & 0x0f00 ) >> 8;
			wGreen = ( wSrcColor & 0x00f0 ) >> 4;
			wBlue  = ( wSrcColor & 0x000f );

			wGray  = ( wRed * 30 + wGreen * 59 + wBlue * 11 ) / iScaleRate;
			pBuf[iX] = wAlpha | ( wGray << 8 ) | ( wGray << 4 ) | wGray;
		}

		pBuf += iPitch;
	}

	pTempSurf->UnlockRect();

	// Compress
	IDirect3DSurface9 *pGraySurf = NULL;
	hr = pGrayTex->GetSurfaceLevel( 0, &pGraySurf );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayDDSFilter5: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayDDSFilter - %s Gray GetSurface Failed(%x)",
								pRcSrc->GetName().c_str(), hr );

		SAFERELEASE( pTempSurf );
		SAFERELEASE( pTempTex );
		return;
	}

	hr = D3DXLoadSurfaceFromSurface( pGraySurf, NULL, NULL,
									 pTempSurf, NULL, NULL,
									 D3DX_FILTER_NONE, 0 );

	SAFERELEASE( pGraySurf );
	SAFERELEASE( pTempSurf );
	SAFERELEASE( pTempTex );

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "GrayDDSFilter6: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::GrayDDSFilter - %s Gray LoadSurface Failed(%x)",
								pRcSrc->GetName().c_str(), hr );
	}
}

void ioTextureManager::UpdateGrayTexture( const ioTexture *pTex,
										  const RECT &rcArea,
										  BYTE *pSrcBuf )
{
	IDirect3DTexture9 *pGrayTex = pTex->GetGrayTexture();
	if( !pGrayTex )	return;

	D3DFORMAT eFormat = pTex->GetFormat();
	if( eFormat != D3DFMT_A8R8G8B8 &&
		eFormat != D3DFMT_X8R8G8B8 &&
		eFormat != D3DFMT_A4R4G4B4 &&
		eFormat != D3DFMT_R5G6B5 )
		return;

	D3DLOCKED_RECT  kDestLock;
	pGrayTex->LockRect( 0, &kDestLock, NULL, 0 );
	if( !kDestLock.pBits )
	{
		pGrayTex->UnlockRect(0);
		return;
	}

	int iCopyWidth  = ( rcArea.right - rcArea.left );
	int iCopyHeight = ( rcArea.bottom - rcArea.top );

	if( eFormat == D3DFMT_A8R8G8B8 || eFormat == D3DFMT_X8R8G8B8 )
	{
		int iDestPitch = kDestLock.Pitch >> 2;

		DWORD *pSrc  = (DWORD*)pSrcBuf;
		DWORD *pDest = (DWORD*)kDestLock.pBits;
		pDest += ( rcArea.top * iDestPitch + rcArea.left );

		DWORD dwAlpha, dwRed, dwGreen, dwBlue, dwGray, dwSrcColor;
		for( int iY=0 ; iY<iCopyHeight ; iY++ )
		{
			for( int iX=0 ; iX<iCopyWidth ; iX++ )
			{
				dwSrcColor = pSrc[iX];

				dwAlpha = ( dwSrcColor & 0xff000000 );
				dwRed	= ( dwSrcColor & 0x00ff0000 ) >> 16;
				dwGreen = ( dwSrcColor & 0x0000ff00 ) >> 8;
				dwBlue  = ( dwSrcColor & 0x000000ff );

				dwGray = ( dwRed * 30 + dwGreen * 59 + dwBlue * 11 ) / m_iGrayScaleRate;
				pDest[iX] = dwAlpha | ( dwGray << 16 ) | ( dwGray << 8 ) | dwGray;
			}

			pSrc  += iCopyWidth;
			pDest += iDestPitch;
		}
	}
	else if( eFormat == D3DFMT_A4R4G4B4 )
	{
		int iDestPitch = kDestLock.Pitch >> 1;

		WORD *pSrc  = (WORD*)pSrcBuf;
		WORD *pDest = (WORD*)kDestLock.pBits;
		pDest += ( rcArea.top * iDestPitch + rcArea.left );

		WORD wAlpha, wRed, wGreen, wBlue, wGray, wSrcColor;
		for( int iY=0 ; iY<iCopyHeight ; iY++ )
		{
			for( int iX=0 ; iX<iCopyWidth ; iX++ )
			{
				wSrcColor = pSrc[iX];

				wAlpha = ( wSrcColor & 0xf000 );
				wRed   = ( wSrcColor & 0x0f00 ) >> 8;
				wGreen = ( wSrcColor & 0x00f0 ) >> 4;
				wBlue  = ( wSrcColor & 0x000f );

				wGray  = ( wRed * 30 + wGreen * 59 + wBlue * 11 ) / m_iGrayScaleRate;
				pDest[iX] = wAlpha | ( wGray << 8 ) | ( wGray << 4 ) | wGray;
			}

			pSrc  += iCopyWidth;
			pDest += iDestPitch;
		}
	}
	else	// eFormat == D3DFMT_R5G6B5
	{
		int iDestPitch = kDestLock.Pitch >> 1;

		WORD *pSrc  = (WORD*)pSrcBuf;
		WORD *pDest = (WORD*)kDestLock.pBits;
		pDest += ( rcArea.top * iDestPitch + rcArea.left );

		WORD wRed, wGreen, wBlue, wGray6, wGray5, wSrcColor;
		for( int iY=0 ; iY<iCopyHeight ; iY++ )
		{
			for( int iX=0 ; iX<iCopyWidth ; iX++ )
			{
				wSrcColor = pSrc[iX];

				wRed   = ( wSrcColor & 0xf800 ) >> 10;
				wGreen = ( wSrcColor & 0x07e0 ) >> 5;
				wBlue  = ( wSrcColor & 0x001f ) << 1;

				wGray6 = ( wRed * 30 + wGreen * 59 + wBlue * 11 ) / m_iGrayScaleRate;
				wGray5 = wGray6 >> 1;

				pDest[iX] = ( wGray5 << 11 ) | ( wGray6 << 5 ) | wGray5;
			}

			pSrc  += iCopyWidth;
			pDest += iDestPitch;
		}
	}

	pGrayTex->UnlockRect(0);
}


void ioTextureManager::UpdateGrayLightTexture( const ioTexture *pTex,
										 const RECT &rcArea,
										 BYTE *pSrcBuf )
{
	IDirect3DTexture9 *pGrayTex = pTex->GetGrayLightTexture();
	if( !pGrayTex )	return;

	D3DFORMAT eFormat = pTex->GetFormat();
	if( eFormat != D3DFMT_A8R8G8B8 &&
		eFormat != D3DFMT_X8R8G8B8 &&
		eFormat != D3DFMT_A4R4G4B4 &&
		eFormat != D3DFMT_R5G6B5 )
		return;

	D3DLOCKED_RECT  kDestLock;
	pGrayTex->LockRect( 0, &kDestLock, NULL, 0 );
	if( !kDestLock.pBits )
	{
		pGrayTex->UnlockRect(0);
		return;
	}

	int iCopyWidth  = ( rcArea.right - rcArea.left );
	int iCopyHeight = ( rcArea.bottom - rcArea.top );

	if( eFormat == D3DFMT_A8R8G8B8 || eFormat == D3DFMT_X8R8G8B8 )
	{
		int iDestPitch = kDestLock.Pitch >> 2;

		DWORD *pSrc  = (DWORD*)pSrcBuf;
		DWORD *pDest = (DWORD*)kDestLock.pBits;
		pDest += ( rcArea.top * iDestPitch + rcArea.left );

		DWORD dwAlpha, dwRed, dwGreen, dwBlue, dwGray, dwSrcColor;
		for( int iY=0 ; iY<iCopyHeight ; iY++ )
		{
			for( int iX=0 ; iX<iCopyWidth ; iX++ )
			{
				dwSrcColor = pSrc[iX];

				dwAlpha = ( dwSrcColor & 0xff000000 );
				dwRed	= ( dwSrcColor & 0x00ff0000 ) >> 16;
				dwGreen = ( dwSrcColor & 0x0000ff00 ) >> 8;
				dwBlue  = ( dwSrcColor & 0x000000ff );

				dwGray = ( dwRed * 30 + dwGreen * 59 + dwBlue * 11 ) / m_iGrayLightScaleRate;
				pDest[iX] = dwAlpha | ( dwGray << 16 ) | ( dwGray << 8 ) | dwGray;
			}

			pSrc  += iCopyWidth;
			pDest += iDestPitch;
		}
	}
	else if( eFormat == D3DFMT_A4R4G4B4 )
	{
		int iDestPitch = kDestLock.Pitch >> 1;

		WORD *pSrc  = (WORD*)pSrcBuf;
		WORD *pDest = (WORD*)kDestLock.pBits;
		pDest += ( rcArea.top * iDestPitch + rcArea.left );

		WORD wAlpha, wRed, wGreen, wBlue, wGray, wSrcColor;
		for( int iY=0 ; iY<iCopyHeight ; iY++ )
		{
			for( int iX=0 ; iX<iCopyWidth ; iX++ )
			{
				wSrcColor = pSrc[iX];

				wAlpha = ( wSrcColor & 0xf000 );
				wRed   = ( wSrcColor & 0x0f00 ) >> 8;
				wGreen = ( wSrcColor & 0x00f0 ) >> 4;
				wBlue  = ( wSrcColor & 0x000f );

				wGray  = ( wRed * 30 + wGreen * 59 + wBlue * 11 ) / m_iGrayLightScaleRate;
				pDest[iX] = wAlpha | ( wGray << 8 ) | ( wGray << 4 ) | wGray;
			}

			pSrc  += iCopyWidth;
			pDest += iDestPitch;
		}
	}
	else	// eFormat == D3DFMT_R5G6B5
	{
		int iDestPitch = kDestLock.Pitch >> 1;

		WORD *pSrc  = (WORD*)pSrcBuf;
		WORD *pDest = (WORD*)kDestLock.pBits;
		pDest += ( rcArea.top * iDestPitch + rcArea.left );

		WORD wRed, wGreen, wBlue, wGray6, wGray5, wSrcColor;
		for( int iY=0 ; iY<iCopyHeight ; iY++ )
		{
			for( int iX=0 ; iX<iCopyWidth ; iX++ )
			{
				wSrcColor = pSrc[iX];

				wRed   = ( wSrcColor & 0xf800 ) >> 10;
				wGreen = ( wSrcColor & 0x07e0 ) >> 5;
				wBlue  = ( wSrcColor & 0x001f ) << 1;

				wGray6 = ( wRed * 30 + wGreen * 59 + wBlue * 11 ) / m_iGrayLightScaleRate;
				wGray5 = wGray6 >> 1;

				pDest[iX] = ( wGray5 << 11 ) | ( wGray6 << 5 ) | wGray5;
			}

			pSrc  += iCopyWidth;
			pDest += iDestPitch;
		}
	}

	pGrayTex->UnlockRect(0);
}

int ioTextureManager::GetTotalTextureCount() const
{
	int iTextureCnt = 0;

	ResourceMap::const_iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		iTextureCnt++;

		ioTexture *pRcTex = dynamic_cast< ioTexture* >( iter->second );
		if( pRcTex->HasGrayTexture() )
			iTextureCnt++;
		if( pRcTex->HasGrayLightTexture() )
			iTextureCnt++;
	}

	return iTextureCnt;
}

int ioTextureManager::GetTextureAboutMemoryBytes() const
{
	int iTexMemory = 0;
	int iTotalMemory = 0;

	ResourceMap::const_iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioTexture *pRcTex = dynamic_cast< ioTexture* >( iter->second );

		iTexMemory = pRcTex->GetWidth() * pRcTex->GetHeight();
		iTexMemory *= (float)( GetPixelWidth( pRcTex->GetFormat() ) / 8 );

		if( pRcTex->HasGrayTexture() && pRcTex->HasGrayLightTexture() )
			iTexMemory *= 3;
		if( pRcTex->HasGrayTexture() || pRcTex->HasGrayLightTexture() )
			iTexMemory *= 2;

		iTotalMemory += iTexMemory;
	}

	return iTotalMemory;
}

bool ioTextureManager::ExtractDDSColorValue( ioTexture *pDDSTex, BYTE *pRGBA )
{
	if( !pDDSTex )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::ExtractDDSColorValue null Pointer" );
		return false;
	}

	D3DLOCKED_RECT rcDDSRect;
	HRESULT hr = pDDSTex->GetTexture()->LockRect( 0, &rcDDSRect, NULL, D3DLOCK_READONLY );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ExtractDDSColorValue1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::ExtractDDSColorValue - %s Lock Failed(%x)", pDDSTex->GetName().c_str(), hr );
		return false;
	}	

	if( !rcDDSRect.pBits )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::ExtractDDSColorValue - %s None Pointer", pDDSTex->GetName().c_str() );
		return false;
	}

	switch( pDDSTex->GetFormat() )
	{
	case D3DFMT_DXT1:
		{
			squish::DecompressImage( (squish::u8 *)pRGBA, pDDSTex->GetWidth(), pDDSTex->GetHeight(), rcDDSRect.pBits, squish::kDxt1 );
		}
		break;
	case D3DFMT_DXT3:
		{
			squish::DecompressImage( (squish::u8 *)pRGBA, pDDSTex->GetWidth(), pDDSTex->GetHeight(), rcDDSRect.pBits, squish::kDxt3 );
		}
		break;
	case D3DFMT_DXT5:
		{
			squish::DecompressImage( (squish::u8 *)pRGBA, pDDSTex->GetWidth(), pDDSTex->GetHeight(), rcDDSRect.pBits, squish::kDxt5 );
		}
		break;
	}
	pDDSTex->GetTexture()->UnlockRect( 0 );
	return true;
}

bool ioTextureManager::MixJPGAlphaValue( ioTexture *pJPGTex, BYTE *pRGBA, int iWidth, int iHeight )
{
	if( !pJPGTex )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::MixJPGAlphaValue null Pointer" );
		return false;
	}

	if( pJPGTex->GetWidth() != iWidth ||
		pJPGTex->GetHeight() != iHeight )
	{
		return false;
	}

	// 컬러 혼합
	D3DLOCKED_RECT rcJPGRect;
	HRESULT hr = pJPGTex->GetTexture()->LockRect( 0, &rcJPGRect, NULL, D3DLOCK_NOSYSLOCK );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "MixJPGAlphaValue1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::MixJPGAlphaValue - Lock Failed(%x)", hr );
		return false;
	}

	DWORD *pdwSrc  = (DWORD*)rcJPGRect.pBits;
	int iSrcPitch  = rcJPGRect.Pitch >> 2;
	// 원본의 알파와 복사본의 컬러 합치기
	{
		int iBitCnt = 0;
		DWORD dwAlpha, dwRed, dwGreen, dwBlue, dwSrcColor;
		for( int iY=0 ; iY<iHeight ; iY++ )
		{
			for( int iX=0 ; iX<iWidth ; iX++ )
			{
				dwSrcColor = pdwSrc[iX];
				dwAlpha = ( dwSrcColor & 0xff000000 ) >> 24;
				dwRed	= ( dwSrcColor & 0x00ff0000 ) >> 16;
				dwGreen = ( dwSrcColor & 0x0000ff00 ) >> 8;
				dwBlue  = ( dwSrcColor & 0x000000ff );

				pRGBA[iBitCnt]   = dwRed;
				pRGBA[iBitCnt+1] = dwGreen;
				pRGBA[iBitCnt+2] = dwBlue;
				iBitCnt+=4;
			}
			pdwSrc += iSrcPitch;
		}
	}
	pJPGTex->GetTexture()->UnlockRect( 0 );
	return true;
}

bool ioTextureManager::ColorCompressDDS( ioTexture *pDDSTex, BYTE *pRGBA )
{
	if( !pDDSTex )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureManager::ColorCompressDDS null Pointer" );
		return false;
	}

	// 합친 파일 압축
	D3DLOCKED_RECT rcDDSRect;
	HRESULT hr = pDDSTex->GetTexture()->LockRect( 0, &rcDDSRect, NULL, D3DLOCK_NOSYSLOCK );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ColorCompressDDS1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::ColorCompressDDS - Lock Failed(%x)", hr );
		return false;
	}	
	switch( pDDSTex->GetFormat() )
	{
	case D3DFMT_DXT1:
		{
			squish::CompressImage( (squish::u8 *)pRGBA, pDDSTex->GetWidth(), pDDSTex->GetHeight(), rcDDSRect.pBits, squish::kDxt1 );
		}
		break;
	case D3DFMT_DXT3:
		{
			squish::CompressImage( (squish::u8 *)pRGBA, pDDSTex->GetWidth(), pDDSTex->GetHeight(), rcDDSRect.pBits, squish::kDxt3 );
		}
		break;
	case D3DFMT_DXT5:
		{
			squish::CompressImage( (squish::u8 *)pRGBA, pDDSTex->GetWidth(), pDDSTex->GetHeight(), rcDDSRect.pBits, squish::kDxt5 );
		}
		break;
	}
	pDDSTex->GetTexture()->UnlockRect( 0 );		

	// 밉맵 적용
	IDirect3DSurface9 *pSrcFace;
	hr = pDDSTex->GetTexture()->GetSurfaceLevel( 0, &pSrcFace );
	if( FAILED( hr ) )
	{
		ErrorReport::SetFormat( "ColorCompressDDS2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTextureManager::ColorCompressDDS - MipMap Failed (%x)", hr );
		return false;
	}

	D3DSURFACE_DESC kSrcDesc;
	pSrcFace->GetDesc( &kSrcDesc );
	RECT kSrcRect = { 0, 0, kSrcDesc.Width, kSrcDesc.Height };

	int iMipLevel = pDDSTex->GetTexture()->GetLevelCount();
	for( int i=1 ; i<iMipLevel ; i++ )
	{
		IDirect3DSurface9 *pDstFace;
		hr = pDDSTex->GetTexture()->GetSurfaceLevel( i, &pDstFace );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ColorCompressDDS3: %x", hr );
			SAFERELEASE( pSrcFace );
			LOG.PrintTimeAndLog( 0, "ioTextureManager::ColorCompressDDS - %d : MipMap Failed1 (%x)", i, hr );
			return false;
		}

		D3DSURFACE_DESC kDstDesc;
		pDstFace->GetDesc( &kDstDesc );
		RECT kDstRect = { 0, 0, kDstDesc.Width, kDstDesc.Height };
		hr = D3DXLoadSurfaceFromSurface( pDstFace, NULL, &kDstRect, pSrcFace, NULL, &kSrcRect, D3DX_FILTER_BOX, 0 );

		SAFERELEASE( pDstFace );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ColorCompressDDS4: %x", hr );
			SAFERELEASE( pSrcFace );
			LOG.PrintTimeAndLog( 0, "ioTextureManager::ColorCompressDDS - %d : MipMap Failed2 (%x)", i, hr );
			return false;
		}
	}
	SAFERELEASE( pSrcFace );
	return true;
}

ioTextureManager& ioTextureManager::GetSingleton()
{
	return Singleton<ioTextureManager>::GetSingleton();
}
