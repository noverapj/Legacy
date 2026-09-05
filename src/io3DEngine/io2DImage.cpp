

#include "stdafx.h"

#include "ioResource.h"
#include "ioRc2DImage.h"
#include "io2DImage.h"

#include "ioResourceManager.h"
#include "io2DImageManager.h"
#include "ioCPU.h"

DWORD io2DImage::m_Screen15LeftRed[32][32];
DWORD io2DImage::m_Screen15LeftGreen[32][32];
DWORD io2DImage::m_Screen15LeftBlue[32][32];

DWORD io2DImage::m_Screen15RightRed[32][32];
DWORD io2DImage::m_Screen15RightGreen[32][32];
DWORD io2DImage::m_Screen15RightBlue[32][32];

DWORD io2DImage::m_Gray15LeftRGB[ 32768 ];
DWORD io2DImage::m_Gray15RightRGB[ 32768 ];

#define GRAY_VALUE 110

io2DImage::io2DImage( const ioHashString &name )
{
	m_pRc2DImage = io2DImageManager::GetSingleton().Create2DImage( name );

	RestoreSrcRect();
}

io2DImage::~io2DImage()
{
	if( m_pRc2DImage )
	{
		io2DImageManager::GetSingleton().Destroy( m_pRc2DImage );
		m_pRc2DImage = NULL;
	}
}

void io2DImage::InitColorTable()
{
	int iBk, iSpr;
	DWORD dwTemp;

	//--------- Screen ----------------------------
	for( iSpr=0 ; iSpr<32 ; iSpr++ )
	{
		m_Screen15LeftRed[iSpr][0]   = 0x80008000;
		m_Screen15LeftGreen[iSpr][0] = 0;
		m_Screen15LeftBlue[iSpr][0]  = 0;

		m_Screen15RightRed[iSpr][0]  = 0x80008000;
		m_Screen15RightGreen[iSpr][0]= 0;
		m_Screen15RightBlue[iSpr][0] = 0;
	}

	for( iBk=0 ; iBk<32 ; iBk++ )
	{
		m_Screen15LeftRed[0][iBk]   = 0x80008000;
		m_Screen15LeftGreen[0][iBk] = 0;
		m_Screen15LeftBlue[0][iBk]  = 0;

		m_Screen15RightRed[0][iBk]  = 0x80008000;
		m_Screen15RightGreen[0][iBk]= 0;
		m_Screen15RightBlue[0][iBk] = 0;
	}

	for( iBk=1; iBk<32; iBk++ )
	{
		for( iSpr=1; iSpr<32; iSpr++ )
		{
			dwTemp = (DWORD)( ( 32.0f - max(iBk,iSpr) ) / 32.0f * min(iBk,iSpr) );

			m_Screen15LeftRed[iSpr][iBk] = (DWORD)( max(iBk,iSpr)+dwTemp );
			if ( m_Screen15LeftRed[iSpr][iBk] >= 32 )
				m_Screen15LeftRed[iSpr][iBk] = 31;
			m_Screen15LeftRed[iSpr][iBk] <<= 26;
			m_Screen15LeftRed[iSpr][iBk] |= 0x80000000;

			m_Screen15LeftGreen[iSpr][iBk] = (DWORD)( max(iBk,iSpr)+dwTemp );
			if ( m_Screen15LeftGreen[iSpr][iBk] >= 32 )
				 m_Screen15LeftGreen[iSpr][iBk] = 31;
			m_Screen15LeftGreen[iSpr][iBk] <<= 21;

			m_Screen15LeftBlue[iSpr][iBk] = (DWORD)( max(iBk,iSpr)+dwTemp );
			if ( m_Screen15LeftBlue[iSpr][iBk] >= 32 )
				 m_Screen15LeftBlue[iSpr][iBk] = 31;
			m_Screen15LeftBlue[iSpr][iBk] <<= 16;

			m_Screen15RightRed[iSpr][iBk] = (DWORD)( max(iBk,iSpr)+dwTemp );
			if ( m_Screen15RightRed[iSpr][iBk] >= 32 )
				 m_Screen15RightRed[iSpr][iBk] = 31;
			m_Screen15RightRed[iSpr][iBk] <<= 10;
			m_Screen15RightRed[iSpr][iBk] |= 0x8000;

			m_Screen15RightGreen[iSpr][iBk] = (DWORD)( max(iBk,iSpr)+dwTemp );
			if ( m_Screen15RightGreen[iSpr][iBk] >= 32 )
				m_Screen15RightGreen[iSpr][iBk] = 31;
			m_Screen15RightGreen[iSpr][iBk] <<= 5;

			m_Screen15RightBlue[iSpr][iBk] = (DWORD)( max(iBk,iSpr)+dwTemp );
			if ( m_Screen15RightBlue[iSpr][iBk] >= 32 )
				m_Screen15RightBlue[iSpr][iBk] = 31;
		}
	}

	//-------- Gray --------------------
	for( iSpr=0; iSpr<32768 ; iSpr++)
	{
		dwTemp = ( ( ( ( ( iSpr>>10 ) &0x1F ) * 30) + ( ( ( iSpr>>5 ) &0x1F ) * 59 ) + ( ( ( iSpr ) &0x1F ) * 11 ) )  / GRAY_VALUE );

		m_Gray15LeftRGB[iSpr]  = ( (dwTemp << 10 ) | (dwTemp << 5 ) | dwTemp ) << 16;
		m_Gray15LeftRGB[iSpr] |= 0x80000000;

		m_Gray15RightRGB[iSpr] = ( (dwTemp << 10 ) | (dwTemp << 5 ) | dwTemp );
		m_Gray15RightRGB[iSpr] |= 0x8000;
	}
}

void io2DImage::SetSrcRect( int iStartX, int iStartY, int iWidth, int iHeight )
{
	int iRcWidth, iRcHeight;
	iRcWidth  = GetWidth();
	iRcHeight = GetHeight();
	
	m_SrcRect.left = max( 0, min( iStartX, iRcWidth ) );
	m_SrcRect.top  = max( 0, min( iStartY, iRcHeight ) );

	m_SrcRect.right  = min( m_SrcRect.left + iWidth, iRcWidth );
	m_SrcRect.bottom = min( m_SrcRect.top + iHeight, iRcHeight );
}

void io2DImage::SetSrcRectWidth( int iStartX, int iWidth )
{
	int iRcWidth  = GetWidth();
	m_SrcRect.left  = max( 0, min( iStartX, iRcWidth ) );
	m_SrcRect.right = min( m_SrcRect.left + iWidth, iRcWidth );

	m_SrcRect.top = 0;
	m_SrcRect.bottom = GetHeight();
}

void io2DImage::SetSrcRectHeight( int iStartY, int iHeight )
{
	int iRcHeight = GetHeight();

	m_SrcRect.top    = max( 0, min( iStartY, iRcHeight ) );
	m_SrcRect.bottom = min( m_SrcRect.top + iHeight, iRcHeight );

	m_SrcRect.left  = 0;
	m_SrcRect.right = GetWidth();
}

void io2DImage::RestoreSrcRect()
{
	SetRect( &m_SrcRect, 0, 0, GetWidth(), GetHeight() );
}

const ioHashString& io2DImage::GetName() const
{
	assert( m_pRc2DImage );

	return m_pRc2DImage->GetName();
}

int io2DImage::GetWidth() const
{
	assert( m_pRc2DImage );

	return m_pRc2DImage->GetWidth();
}

int io2DImage::GetHeight() const
{
	assert( m_pRc2DImage );

	return m_pRc2DImage->GetHeight();
}

void io2DImage::DrawDest( WORD *pDest, int xPos, int yPos, int iPitch )
{
	if( !m_pRc2DImage )	return;

	const WORD *pImgPtr = GetImgHPtr( m_SrcRect.top );

	int iWidth  = m_SrcRect.right - m_SrcRect.left;
	int iHeight = m_SrcRect.bottom - m_SrcRect.top;

	bool bColorKey;
	int iX, iY;
	int iNum;

	for( iY=0 ; iY<iHeight ; iY++ )
	{
		iX=0;
		iNum = 0;
		bColorKey = false;

		const WORD *pSrc = GetImgWPtr( pImgPtr, m_SrcRect.left, bColorKey, iNum );

		while( iX < iWidth )
		{
			if( iNum > 0 )
			{
				if( iX + iNum > iWidth )
				{
					iNum = iWidth - iX;
				}

				if( !bColorKey )	// is Real Key ?
				{
					ioCPU::FastMemcpy( &pDest[ (yPos + iY)*iPitch + xPos + iX ], pSrc, sizeof(WORD)*iNum );
					pSrc += iNum;
				}
				iX += iNum;
			}

			if( iX >= iWidth )	break;

			bColorKey = !bColorKey;
			iNum = *pSrc++;
		}

		int iCount = *pImgPtr;
		pImgPtr += iCount + 1;
	}
}

void io2DImage::ScreenDrawDest( WORD *pDest, int xPos, int yPos, int iPitch )
{
	if( !m_pRc2DImage )	return;

	const WORD *pImgPtr = GetImgHPtr( m_SrcRect.top );

	int iWidth  = m_SrcRect.right - m_SrcRect.left;
	int iHeight = m_SrcRect.bottom - m_SrcRect.top;

	bool bColorKey;
	int iX, iY;
	int iNum;

	for( iY=0 ; iY<iHeight ; iY++ )
	{
		iX=0;
		iNum = 0;
		bColorKey = false;

		const WORD *pSrc = GetImgWPtr( pImgPtr, m_SrcRect.left, bColorKey, iNum );

		while( iX < iWidth )
		{
			if( iNum > 0 )
			{
				if( iX + iNum > iWidth )
				{
					iNum = iWidth - iX;
				}

				if( !bColorKey )	// is Real Key ?
				{
					Screen15Copy4Byte( &pDest[ (yPos + iY)*iPitch + xPos + iX ], pSrc, iNum );
					pSrc += iNum;
				}
				iX += iNum;
			}

			if( iX >= iWidth )	break;

			bColorKey = !bColorKey;
			iNum = *pSrc++;
		}

		int iCount = *pImgPtr;
		pImgPtr += iCount + 1;
	}
}

void __fastcall io2DImage::Screen15Copy4Byte( WORD *pDest, const WORD *pSrc, int iPixelNum )
{
	if( iPixelNum & 0x1 )	// odd
	{
		pDest[0] = (WORD)( m_Screen15RightRed[ ((pSrc[0]>>10)&0x1f) ][ ((pDest[0]>>10)&0x1f) ] |
						   m_Screen15RightGreen[ ((pSrc[0]>>5)&0x1f) ][ ((pDest[0]>>5)&0x1f) ] |
						   m_Screen15RightBlue[ (pSrc[0]&0x1f) ][ (pDest[0]&0x1f) ] );

		pSrc++;
		pDest++;
		iPixelNum--;
	}

	DWORD *pDwDest, *pDwSrc;
	pDwDest = (DWORD*)pDest;
	pDwSrc  = (DWORD*)pSrc;
	iPixelNum >>= 1;

	for( int i=0 ; i<iPixelNum ; i++ )
	{
		*pDwDest = m_Screen15LeftRed[ ((*pDwSrc>>26)&0x1f) ][ ((*pDwDest>>26)&0x1f) ] |
				   m_Screen15LeftGreen[ ((*pDwSrc>>21)&0x1f) ][ ((*pDwDest>>21)&0x1f) ] |
				   m_Screen15LeftBlue[ ((*pDwSrc>>16)&0x1f) ][ ((*pDwDest>>16)&0x1f) ] |
				   m_Screen15RightRed[ ((*pDwSrc>>10)&0x1f) ][ ((*pDwDest>>10)&0x1f) ] |
				   m_Screen15RightGreen[ ((*pDwSrc>>5)&0x1f) ][ ((*pDwDest>>5)&0x1f) ] |
				   m_Screen15RightBlue[ (*pDwSrc&0x1f) ][ (*pDwDest&0x1f) ];

		pDwSrc++;
		pDwDest++;
	}
}

void io2DImage::GrayDrawDest( WORD *pDest, int xPos, int yPos, int iPitch )
{
	if( !m_pRc2DImage )	return;

	const WORD *pImgPtr = GetImgHPtr( m_SrcRect.top );

	int iWidth  = m_SrcRect.right - m_SrcRect.left;
	int iHeight = m_SrcRect.bottom - m_SrcRect.top;

	bool bColorKey;
	int iX, iY;
	int iNum;

	for( iY=0 ; iY<iHeight ; iY++ )
	{
		iX=0;
		iNum = 0;
		bColorKey = false;

		const WORD *pSrc = GetImgWPtr( pImgPtr, m_SrcRect.left, bColorKey, iNum );

		while( iX < iWidth )
		{
			if( iNum > 0 )
			{
				if( iX + iNum > iWidth )
				{
					iNum = iWidth - iX;
				}

				if( !bColorKey )	// is Real Key ?
				{
					Gray15Copy4Byte( &pDest[ (yPos + iY)*iPitch + xPos + iX ], pSrc, iNum );
					pSrc += iNum;
				}
				iX += iNum;
			}

			if( iX >= iWidth )	break;

			bColorKey = !bColorKey;
			iNum = *pSrc++;
		}

		int iCount = *pImgPtr;
		pImgPtr += iCount + 1;
	}
}

void __fastcall io2DImage::Gray15Copy4Byte( WORD *pDest, const WORD *pSrc, int iPixelNum )
{
	if( iPixelNum & 0x1 )	// odd
	{
		pDest[0] = (WORD) m_Gray15RightRGB[ (pSrc[0]&0x7FFF) ];

		pSrc++;
		pDest++;
		iPixelNum--;
	}

	DWORD *pDwDest, *pDwSrc;
	pDwDest = (DWORD*)pDest;
	pDwSrc  = (DWORD*)pSrc;
	iPixelNum >>= 1;

	for( int i=0 ; i<iPixelNum ; i++ )
	{
		*pDwDest = m_Gray15LeftRGB[ (*pDwSrc>>16)&0x7FFF ] |
				   m_Gray15RightRGB[ (*pDwSrc&0x7FFF) ];

		pDwSrc++;
		pDwDest++;
	}
}

void io2DImage::AlphaDrawDest( WORD *pDest, int xPos, int yPos, int iPitch, int iOpacity )
{
	if( !m_pRc2DImage )	return;

	iOpacity = max( 0, min( iOpacity, 100 ) );

	const WORD *pImgPtr = GetImgHPtr( m_SrcRect.top );

	int iWidth  = m_SrcRect.right - m_SrcRect.left;
	int iHeight = m_SrcRect.bottom - m_SrcRect.top;	

	WORD wDest, wSrc;
	
	bool bColorKey;
	int iX, iY;
	int iNum;

	for( iY=0 ; iY<iHeight ; iY++ )
	{
		iX=0;
		iNum = 0;
		bColorKey = false;

		const WORD *pSrc = GetImgWPtr( pImgPtr, m_SrcRect.left, bColorKey, iNum );

		while( iX < iWidth )
		{
			if( iNum > 0 )
			{
				if( iX + iNum > iWidth )
				{
					iNum = iWidth - iX;
				}

				if( !bColorKey )	// is Real Key ?
				{
					WORD *pDestTemp = &pDest[ (yPos + iY)*iPitch + xPos + iX ];
					for( int i=0 ; i<iNum ; i++ )
					{
						wDest = *pDestTemp;
						wSrc  = *pSrc;

						wDest = ((((wSrc&0x7c00)>>10)*iOpacity/100 + ((wDest&0x7c00)>>10)*(100-iOpacity)/100)<<10) |
							    ((((wSrc&0x03e0)>>5)*iOpacity/100 + ((wDest&0x03e0)>>5)*(100-iOpacity)/100)<<5) |
								(((wSrc&0x001f)*iOpacity/100 + (wDest&0x001f)*(100-iOpacity)/100));
						*pDestTemp = wDest | 0x8000;
						
						pDestTemp++;
						pSrc++;
					}
				}

				iX += iNum;
			}

			if( iX >= iWidth )	break;

			bColorKey = !bColorKey;
			iNum = *pSrc++;
		}

		int iCount = *pImgPtr;
		pImgPtr += iCount + 1;
	}
}

const WORD* io2DImage::GetImgWPtr( const WORD *pSrc, int xPos, bool &bColorKey, int &iNum )
{
	bColorKey = false;
	iNum = 0;
	pSrc++;

	int iPlusNum;

	for( int iX=0 ; iX<xPos ; )
	{
		iX += *pSrc++;
		bColorKey = true;
		if( iX > xPos )
		{
			iNum = iX - xPos;
			break;
		}

		iPlusNum = *pSrc;
		iX += *pSrc;

		bColorKey = false;

		if( iX > xPos )
		{
			iPlusNum = xPos - ( iX - *pSrc );
			iNum = iX - xPos;
			pSrc += iPlusNum + 1;
			break;
		}
		else
		{
			pSrc += iPlusNum + 1;
		}
	}

	return pSrc;
}

const WORD* io2DImage::GetImgHPtr( int yPos )
{
	const WORD *pImgPtr = m_pRc2DImage->GetImage();

	for( int i=0 ; i<yPos ; i++ )
	{
		pImgPtr += *pImgPtr + 1;
	}

	return pImgPtr;
}