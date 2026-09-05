

#include "stdafx.h"

#include "ioUIImage.h"
#include "ioUIImageSet.h"

#include "ioTexture.h"
#include "ioTextureManager.h"

#include "ioCPU.h"
#include "HelpFunc.h"
#include "ErrorReport.h"

ioUIImageSet::ioUIImageSet( const ioHashString &szName,
						    const ioHashString &rkTexName,
							ErasePolicy ePolicy )
: m_Name( szName ),
  m_TextureName( rkTexName )
{
	m_ErasePolicy = ePolicy;
	m_bLoadNow = false;

	m_pUITexture = NULL;
	m_iImageRef = 0;
	m_bDefaultPath = true;
	m_bLoadPiece   = false;
}

ioUIImageSet::~ioUIImageSet()
{
	UnDefineAllImages();
	DESTROY_TEXTURE( m_pUITexture );
}

void ioUIImageSet::ImageLoadOption( bool bDefaultPath, bool bLoadPiece )
{
	m_bDefaultPath = bDefaultPath;
	m_bLoadPiece   = bLoadPiece;
}

ioUIImage* ioUIImageSet::DefineImage( const ioHashString &szName,
									  const RECT &rcTexRect,
									  int iOffX,
									  int iOffY )
{
	UIImageMap::iterator iter = m_UIImageMap.find( szName );
	if( iter != m_UIImageMap.end() )
	{
		LOG.PrintTimeAndLog( 0, "ioUIImageSet::DefineImage - %s Set %s Image Already Define",
								m_Name.c_str(), szName.c_str() );
		return NULL;
	}

	ioUIImage *pNewImag = new ioUIImage( this, szName, rcTexRect );
	if( pNewImag )
	{
		pNewImag->SetOffsetXY( iOffX, iOffY );
		m_UIImageMap.insert( UIImageMap::value_type( szName, pNewImag ) );
	}

	return pNewImag;
}

ioUIImage* ioUIImageSet::DefineImage( const ioHashString &szName,
									  const POINT &ptPos,
									  const POINT &ptSize,
									  int iOffX,
									  int iOffY )
{
	RECT rcTexRect;
	rcTexRect.left   = ptPos.x;
	rcTexRect.top    = ptPos.y;
	rcTexRect.right  = ptPos.x + ptSize.x;
	rcTexRect.bottom = ptPos.y + ptSize.y;

	return DefineImage( szName, rcTexRect, iOffX, iOffY );
}

void ioUIImageSet::UnDefineImage( const ioHashString &szName )
{
	UIImageMap::iterator iter = m_UIImageMap.find( szName );
	if( iter != m_UIImageMap.end() )
	{
		ioUIImage *pImage = iter->second;
		if( pImage->GetRefCount() > 0 )
		{
			LOG.PrintTimeAndLog( 0, "ioUIImageSet::UnDefineImage - %s : %s Image Not Released(%d)",
									m_Name.c_str(),
									pImage->GetName().c_str(),
									pImage->GetRefCount() );
		}

		delete pImage;

		m_UIImageMap.erase( iter );
	}
}

void ioUIImageSet::UnDefineAllImages()
{
	UIImageMap::iterator iter;
	for( iter=m_UIImageMap.begin() ; iter!=m_UIImageMap.end() ; ++iter )
	{
		ioUIImage *pImage = iter->second;
		if( pImage->GetRefCount() > 0 )
		{
			LOG.PrintTimeAndLog( 0, "ioUIImageSet::UnDefineAllImages - %s : %s Image Not Released(%d)",
									m_Name.c_str(),
									pImage->GetName().c_str(),
									pImage->GetRefCount() );
		}

		delete pImage;
	}

	m_UIImageMap.clear();
}

ioUIImage* ioUIImageSet::GetImage( const ioHashString &szName )
{
	UIImageMap::iterator iter = m_UIImageMap.find( szName );
	if( iter!=m_UIImageMap.end() )
		return iter->second;

	return NULL;
}

ioUIImage* ioUIImageSet::GetImage( int iIndex )
{
	if( COMPARE( iIndex, 0, GetImageCount() ) )
	{
		UIImageMap::iterator iter = m_UIImageMap.begin();
		std::advance( iter, iIndex );
		return iter->second;
	}

	return NULL;
}

const char* ioUIImageSet::GetImageName( int index )
{
	if( COMPARE( index, 0, GetImageCount() ) )
	{
		UIImageMap::iterator iter = m_UIImageMap.begin();
		std::advance( iter, index );
		return iter->first.c_str();
	}

	return NULL;
}

bool ioUIImageSet::IsImageDefined( const ioHashString &szName )
{
	if( GetImage( szName ) )
		return true;

	return false;
}

float ioUIImageSet::GetImageWidth( const ioHashString &szName )
{
	ioUIImage *pImg = GetImage( szName );
	if( pImg )
		return pImg->GetWidth();

	return 1.0f;
}

float ioUIImageSet::GetImageHeight( const ioHashString &szName )
{
	ioUIImage *pImg = GetImage( szName );
	if( pImg )
		return pImg->GetHeight();

	return 1.0f;
}

bool ioUIImageSet::IsLoaded() const
{
	if( m_pUITexture && m_pUITexture->IsLoaded() )
		return true;

	return false;
}

void ioUIImageSet::Touch( bool bThread ) const
{
	if( !m_pUITexture )
	{
		m_pUITexture = g_TextureMgr.CreateTexture( m_TextureName, 0, 1, TQ_MIDDLE, bThread, m_bDefaultPath, m_bLoadPiece );
		if( m_pUITexture )
		{
			m_pUITexture->SetErasePolicy( m_ErasePolicy );
		}
	}
}

void ioUIImageSet::ResetTextureFile( const char *szFileName )
{
	if( m_TextureName == szFileName )
		return;

	DESTROY_TEXTURE( m_pUITexture );
	m_TextureName = szFileName;
}

IDirect3DTexture9* ioUIImageSet::GetD3DTexture() const
{
	Touch( true );

	if( m_pUITexture )
		return m_pUITexture->GetTexture();

	return NULL;
}

IDirect3DTexture9* ioUIImageSet::GetGrayTexture() const
{
	Touch( true );

	if( m_pUITexture )
		return m_pUITexture->GetGrayTexture();

	return NULL;
}

IDirect3DTexture9* ioUIImageSet::GetGrayLightTexture() const
{
	Touch( true );

	if( m_pUITexture )
		return m_pUITexture->GetGrayLightTexture();

	return NULL;
}

int ioUIImageSet::GetTextureWidth() const
{
	Touch( true );

	if( m_pUITexture )
		return m_pUITexture->GetWidth();

	return 1;
}

int ioUIImageSet::GetTextureHeight() const
{
	Touch( true );

	if( m_pUITexture )
		return m_pUITexture->GetHeight();

	return 1;
}

void ioUIImageSet::AddRef() const
{
	m_iImageRef++;
}

void ioUIImageSet::ReleaseRef() const
{
	m_iImageRef--;
	if( m_iImageRef > 0 )
		return;

	if( m_ErasePolicy == EP_AUTOMATIC )
	{
		DESTROY_TEXTURE( m_pUITexture );
	}
}

bool ioUIImageSet::FillFileImage( const char *szFileName,
								  const int iFileWidth,
								  const int iFileHeight,
								  const ioUIImage *pDestImg,
								  OUT D3DXIMAGE_INFO *pSrcInfo /*= NULL*/ )
{
	Touch( false );

	if( !m_pUITexture || !m_pUITexture->GetTexture() )
	{
		if( m_pUITexture )
		{
			LOG.PrintTimeAndLog( 0, "ioUIImageSet::FillFileImage - %s Not Loaded",
									m_pUITexture->GetName().c_str() );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioUIImageSet::FillFileImage - Default Texture Not Loaded" );
		}

		return false;
	}

	int iPixelWidth = GetPixelWidth( m_pUITexture->GetFormat() ) / 8;
	if( iPixelWidth != 2 && iPixelWidth != 4 )	// 16bit or 32bit
	{
		LOG.PrintTimeAndLog( 0, "ioUIImageSet::FillFileImage - %s Unknown Format(%x)",
								m_pUITexture->GetName().c_str(),
								m_pUITexture->GetFormat() );
		return false;
	}

	// Load Src Texture
	IDirect3DTexture9 *pSrcTex = NULL;
	HRESULT hr = S_OK;
	try
	{
		hr = D3DXCreateTextureFromFileEx( g_TextureMgr.GetDevice(),
											  szFileName,
											  iFileWidth,
											  iFileHeight,
											  1, 0,
											  m_pUITexture->GetFormat(),
											  D3DPOOL_SYSTEMMEM,
											  D3DX_FILTER_NONE,
											  D3DX_FILTER_NONE,
											  0, pSrcInfo, NULL, &pSrcTex );
	}
	catch(...)
	{
		//UJ 140212, 버그수정: jpg 파일 오류로 crash 발생하는 경우에 대한 조치(길드 커스텀마크 등)
		LOG.PrintTimeAndLog( 0, "ioUIImageSet::FillFileImage - Exception catched: D3DXCreateTextureFromFileEx" );
		hr = E_FAIL;
	}

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "FillFileImage1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioUIImageSet::FillFileImage - Load Failed(%x) : %s", hr, szFileName );
		return false;
	}

	D3DSURFACE_DESC kSrcSurface;
	pSrcTex->GetLevelDesc( 0, &kSrcSurface );
	if( kSrcSurface.Width < pDestImg->GetWidth() ||
		kSrcSurface.Height < pDestImg->GetHeight() )
	{
		SAFERELEASE(pSrcTex);
		LOG.PrintTimeAndLog( 0, "ioUIImageSet::FillFileImage - Size Failed(%s)", szFileName );
		return false;
	}

	int iBufBytes = pDestImg->GetWidth() * pDestImg->GetHeight() * iPixelWidth;
	BYTE *pSystemBuf = new BYTE[ iBufBytes ];
	if( !pSystemBuf )
	{
		SAFERELEASE(pSrcTex);
		LOG.PrintTimeAndLog( 0, "ioUIImageSet::FillFileImage - SrcBuf Alloc Failed" );
		return false;
	}

	D3DLOCKED_RECT  kSrcLock;
	pSrcTex->LockRect( 0, &kSrcLock, NULL, D3DLOCK_READONLY );
	if( kSrcLock.pBits == NULL )
	{
		pSrcTex->UnlockRect(0);
		SAFERELEASE(pSrcTex);

		LOG.PrintTimeAndLog( 0, "ioUIImageSet::FillFileImage - Lock Failed : %s", szFileName );
		return false;
	}

	BYTE *pSrcBuf  = (BYTE*)kSrcLock.pBits;
	BYTE *pDestBuf = pSystemBuf;

	int iSrcPitch = kSrcLock.Pitch;
	int iSystemPitch = pDestImg->GetWidth() * iPixelWidth;
	int iCopyHeight  = pDestImg->GetHeight();
	
	for( int i=0 ; i<iCopyHeight ; i++ )
	{
		ioCPU::FastMemcpy( pDestBuf, pSrcBuf, iSystemPitch );

		pSrcBuf += iSrcPitch;
		pDestBuf += iSystemPitch;
	}

	pSrcTex->UnlockRect(0);
	SAFERELEASE( pSrcTex );

	if( m_pUITexture->IsLoadedFromFile() )
	{
		m_pUITexture->SetLoadedFromFile( false );
	}

	bool bCopyOK = m_pUITexture->FillImage( pDestImg->GetAreaRect(),
											pSystemBuf );

	SAFEDELETEARRAY( pSystemBuf );

	return bCopyOK;
}


