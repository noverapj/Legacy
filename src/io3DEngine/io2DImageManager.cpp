

#include "stdafx.h"

#include "ioResource.h"
#include "ioRc2DImage.h"
#include "io2DImage.h"
#include "ioResourceManager.h"
#include "io2DImageManager.h"

#include "ioFileTokenDefine.h"
#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ioCPU.h"


template<> io2DImageManager* Singleton< io2DImageManager >::ms_Singleton = 0;

io2DImageManager::io2DImageManager()
{
	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "2DImage" );
	
	io2DImage::InitColorTable();
}

io2DImageManager::~io2DImageManager()
{
}

ioResource* io2DImageManager::CreateImpl( const ioHashString &name, bool bThread )
{
	ioRc2DImage *pRc2DImage = new ioRc2DImage(name);
	if( LoadFile( name, pRc2DImage ) )
		return pRc2DImage;

	SAFEDELETE( pRc2DImage );
	return NULL;
}

void io2DImageManager::ReLoadImpl( const ioHashString &name )
{
	ioRc2DImage *pRcImg = Get2DImageByName( name );
	if( pRcImg )
	{
		LoadFile( name, pRcImg );
	}
}

ioRc2DImage* io2DImageManager::Create2DImage( const ioHashString &name )
{
	return dynamic_cast< ioRc2DImage* >( Create( name ) );
}

ioRc2DImage* io2DImageManager::Get2DImageByName( const ioHashString &name )
{
	return dynamic_cast< ioRc2DImage* >( GetByName( name ) );
}

bool io2DImageManager::Is2DImageFile( ioBinaryStream &rkStream )
{
	return true;	// 기존 PGF에는 파일에 대한 토큰이 없다.
}

bool io2DImageManager::LoadFile( const ioHashString &name, ioRc2DImage *pRc2DImg )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "%s/%s", m_szStartDir, name.c_str() );

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "io2DImageManager::LoadFile - %s Load Failed", szFileName );
		return false;

	}

	if( !Is2DImageFile( kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "io2DImageManager::LoadFile - %s Is Not *.pgf File", szFileName );
		return false;
	}

	WORD wPicWidth = 0, wPicHeight = 0;
	kStream.Read( &wPicWidth, sizeof(WORD) );
	kStream.Read( &wPicHeight, sizeof(WORD) );

	// palette 768, width 2, height 2
	int iImageSize = kStream.GetSize() - 772;
	BYTE *pImage = new BYTE[ iImageSize ];
	kStream.Read( pImage, iImageSize );

	WORD *pPalette = LoadPalette( kStream );

	WORD *pImage2 = new WORD[ wPicWidth * wPicHeight ];
	DeCompressPGF( wPicWidth, wPicHeight, pImage, pImage2, pPalette );

	SAFEDELETEARRAY( pImage );
	SAFEDELETEARRAY( pPalette );

	WORD *pResultImg = CompressImage( pImage2, wPicWidth, wPicHeight );

	SAFEDELETEARRAY( pImage2 );

	pRc2DImg->SetWidth( wPicWidth );
	pRc2DImg->SetHeight( wPicHeight );
	pRc2DImg->SetImage( pResultImg );

	AddMem( pRc2DImg->GetName(), kStream.GetSize() );

	return true;
}

WORD* io2DImageManager::LoadPalette( ioBinaryStream &rkStream )
{
	struct
	{
		BYTE Red;
		BYTE Green;
		BYTE Blue;
	}rgb[256];

	rkStream.Read( rgb, 256 * 3 );

	WORD *pPalette = new WORD[256];

	WORD wRed, wGreen, wBlue;
	for(int i=0 ; i<256 ; i++ )
	{
		wRed   = (WORD)( rgb[i].Red >> 3) << 10;
		wGreen = (WORD)( rgb[i].Green >> 3 ) << 5;
		wBlue  = (WORD)( rgb[i].Blue >> 3 );

		pPalette[i] = 0x8000 | wRed | wGreen | wBlue;
	}

	return pPalette;
}

void io2DImageManager::DeCompressPGF( int iWidth,
									  int iHeight,
									  const BYTE *pSrc,
									  WORD *pDest,
									  const WORD *pPalette )
{
	bool bBreakFlag = false;
	int iSrcPos = 0;
	int iLineOffset = 0;
	int iYLineCnt = 0;

	WORD *pDestLine = pDest;

	while( !bBreakFlag )
	{
		if( pSrc[iSrcPos] > 0xC0 )
		{
			int iRepeatCnt = pSrc[iSrcPos] - 0xC0;

			iSrcPos++;

			WORD wColor = pPalette[ pSrc[iSrcPos] ];
			for( int iCnt=0 ; iCnt<iRepeatCnt && iLineOffset<iWidth ; iCnt++ )
			{
				pDestLine[iLineOffset++] = wColor;
			}
		}
		else
		{
			pDestLine[iLineOffset++] = pPalette[ pSrc[iSrcPos] ];
		}

		iSrcPos++;

		if( iLineOffset == iWidth )
		{
			iLineOffset = 0;
			iYLineCnt++;
			pDestLine += iWidth;

			if( iYLineCnt == iHeight )
				bBreakFlag = true;
		}
	}
}

WORD* io2DImageManager::CompressImage( const WORD *pSrc, int iWidth, int iHeight )
{
	int iZeroImgSize = iWidth * iHeight * 4;
	WORD *pZeroImg = new WORD[ iZeroImgSize ];
	ZeroMemory( pZeroImg, sizeof(WORD)*iZeroImgSize );
	WORD wColorKey = 0x801f;

	int i,j;
	int iCnt = 0;
	for( i=0 ; i<iHeight ; i++ )
	{
		WORD &wLineNum = pZeroImg[ iCnt ];
		wLineNum = 0;
		iCnt++;

		j=0;

		if( pSrc[j] != wColorKey )
		{
			pZeroImg[iCnt] = 0;
			iCnt++;
			wLineNum++;
		}

		while( j < iWidth )
		{
			if( pSrc[j] == wColorKey )
			{
				WORD &wColorKeyNum = pZeroImg[iCnt];
				do
				{
					wColorKeyNum++;
					j++;					
				}while( j < iWidth && pSrc[j] == wColorKey );

				iCnt++;
				wLineNum++;
			}
			else
			{
				WORD &wNum = pZeroImg[iCnt];
				wNum = 0;
				do
				{
					wNum++;
					iCnt++;
					wLineNum++;
					pZeroImg[iCnt] = pSrc[j];
					j++;
				}while( j<iWidth && pSrc[j] != wColorKey );

				iCnt++;
				wLineNum++;
			}				
		}

		pSrc += iWidth;
	}

	WORD *pResult = new WORD[iCnt];
	ioCPU::FastMemcpy( pResult, pZeroImg, sizeof(WORD) * iCnt );

	SAFEDELETEARRAY( pZeroImg );

	return pResult;
}

io2DImageManager& io2DImageManager::GetSingleton()
{
	return Singleton<io2DImageManager>::GetSingleton();
}