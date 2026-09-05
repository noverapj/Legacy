
#include "StdAfx.h"

#include "io3DCommon.h"
#include "ioVertexFormat.h"
#include "HelpFunc.h"
#include "ioCPU.h"
#include "ErrorReport.h"
#include "ioFontManager.h"
#include <strsafe.h>

D3DXVECTOR3 operator*( const D3DXQUATERNION &qt, const D3DXVECTOR3 &vVec )
{
	// nVidia SDK implementation
	D3DXVECTOR3 uv, uuv;
	D3DXVECTOR3 qtVec( qt.x, qt.y, qt.z );
	D3DXVec3Cross( &uv, &qtVec, &vVec );
	D3DXVec3Cross( &uuv, &qtVec, &uv );

	uv *= ( 2.0f * qt.w );
	uuv *= 2.0f;

	return vVec + uv + uuv;
}

D3DXVECTOR3 operator*( const D3DXVECTOR3 &vLeft, const D3DXVECTOR3 &vRight )
{
	D3DXVECTOR3 vProd;

	vProd.x = vLeft.x * vRight.x;
	vProd.y = vLeft.y * vRight.y;
	vProd.z = vLeft.z * vRight.z;

	return vProd;
}

D3DCOLORVALUE operator*( const D3DCOLORVALUE &lhs, const D3DCOLORVALUE &rhs )
{
	D3DCOLORVALUE vColor;
	vColor.r = lhs.r * rhs.r;
	vColor.g = lhs.g * rhs.g;
	vColor.b = lhs.b * rhs.b;
	vColor.a = lhs.a * rhs.a;

	return vColor;
}

D3DCOLORVALUE operator+( const D3DCOLORVALUE &lhs, const D3DCOLORVALUE &rhs )
{
	D3DCOLORVALUE vColor;
	vColor.r = lhs.r + rhs.r;
	vColor.g = lhs.g + rhs.g;
	vColor.b = lhs.b + rhs.b;
	vColor.a = lhs.a + rhs.a;

	return vColor;
}

D3DCOLORVALUE operator*( const D3DCOLORVALUE &lhs, float fValue )
{
	D3DCOLORVALUE vColor;
	vColor.r = lhs.r * fValue;
	vColor.g = lhs.g * fValue;
	vColor.b = lhs.b * fValue;
	vColor.a = lhs.a * fValue;

	return vColor;
}

bool operator==( const D3DCOLORVALUE &lhs, const D3DCOLORVALUE &rhs )
{
	if( lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a )
		return true;

	return false;
}

bool operator!=( const D3DCOLORVALUE &lhs, const D3DCOLORVALUE &rhs )
{
	return !( lhs == rhs );
}

void StringHeadBlankClear( char *szBuf )
{
	int iCursor = 0;
	while( (szBuf[iCursor] == ' ' || szBuf[iCursor] == '\t') && szBuf[iCursor]!=0 )
		iCursor++;

	if( 0 != iCursor )
		memmove(szBuf, szBuf + iCursor, strlen(szBuf + iCursor) );

	char *pTemp = NULL;
	pTemp = strstr(szBuf, "\n");
	if( NULL != pTemp)
		*pTemp = NULL;
}

bool IsComment( const char *szBuf )
{
	if( szBuf[0] ==';' )
		return true;
	
	return false;
}

void FillVtxRectFan( ioVtxTex *pVtx, float fMaxU, float fMaxV )
{
	pVtx[0].vPos.x = 1.0f;
	pVtx[0].vPos.y = -1.0f;
	pVtx[0].vPos.z = 0.1f;
	pVtx[0].u = fMaxU;
	pVtx[0].v = fMaxV;

	pVtx[1].vPos.x = -1.0f;
	pVtx[1].vPos.y = -1.0f;
	pVtx[1].vPos.z = 0.1f;
	pVtx[1].u = 0.0f;
	pVtx[1].v = fMaxV;

	pVtx[2].vPos.x = -1.0f;
	pVtx[2].vPos.y = 1.0f;
	pVtx[2].vPos.z = 0.1f;
	pVtx[2].u = 0.0f;
	pVtx[2].v = 0.0f;

	pVtx[3].vPos.x = 1.0f;
	pVtx[3].vPos.y = 1.0f;
	pVtx[3].vPos.z = 0.1f;
	pVtx[3].u = fMaxU;
	pVtx[3].v = 0.0f;
}

void FillVtxRHWTexRectStrip( ioVtxRHWTex *pVtx, float fXPos, float fYPos,
												float fWidth, float fHeight,
												float fMaxU, float fMaxV )
{
	fXPos = PixelAligned( fXPos );
	fYPos = PixelAligned( fYPos );

	pVtx[0].vPos.x = fXPos;
	pVtx[0].vPos.y = fYPos + fHeight;
	pVtx[0].vPos.z = 0.0f;
	pVtx[0].fRHW = 1.0f;
	pVtx[0].u = 0.0f;
	pVtx[0].v = fMaxV;

	pVtx[1].vPos.x = fXPos;
	pVtx[1].vPos.y = fYPos;
	pVtx[1].vPos.z = 0.0f;
	pVtx[1].fRHW = 1.0f;
	pVtx[1].u = 0.0f;
	pVtx[1].v = 0.0f;

	pVtx[2].vPos.x = fXPos + fWidth;
	pVtx[2].vPos.y = fYPos + fHeight;
	pVtx[2].vPos.z = 0.0f;
	pVtx[2].fRHW = 1.0f;
	pVtx[2].u = fMaxU;
	pVtx[2].v = fMaxV;

	pVtx[3].vPos.x = fXPos + fWidth;
	pVtx[3].vPos.y = fYPos;
	pVtx[3].vPos.z = 0.0f;
	pVtx[3].fRHW = 1.0f;
	pVtx[3].u = fMaxU;
	pVtx[3].v = 0.0f;
}

void FillVtxRHWRectStrip( ioVtxRHW *pVtx, float fXPos, float fYPos, float fWidth, float fHeight )
{
	fXPos = PixelAligned(fXPos);
	fYPos = PixelAligned(fYPos);

	pVtx[0].vPos.x = fXPos;
	pVtx[0].vPos.y = fYPos + fHeight;
	pVtx[0].vPos.z = 0.0f;
	pVtx[0].fRHW = 1.0f;

	pVtx[1].vPos.x = fXPos;
	pVtx[1].vPos.y = fYPos;
	pVtx[1].vPos.z = 0.0f;
	pVtx[1].fRHW = 1.0f;

	pVtx[2].vPos.x = fXPos + fWidth;
	pVtx[2].vPos.y = fYPos + fHeight;
	pVtx[2].vPos.z = 0.0f;
	pVtx[2].fRHW = 1.0f;

	pVtx[3].vPos.x = fXPos + fWidth;
	pVtx[3].vPos.y = fYPos;
	pVtx[3].vPos.z = 0.0f;
	pVtx[3].fRHW = 1.0f;
}

int GetTwoPowAlignSize( int iSize )
{
	int iResult = 0;
	for( int i=0 ; i<13 ; i++ )
	{
		iResult = 1<<i;
		
		if( iResult >= iSize )
			break;
	}

	return iResult;
}

struct FMTINFO
{
    D3DFORMAT fmt;
    int  nBitsPerTexel;
    bool bHasAlpha;
};

const FMTINFO fmtInfoArray[] = 
{
    D3DFMT_A4L4,            8,     true,
    D3DFMT_A8,              8,     true,
    D3DFMT_L8,              8,     false,
    D3DFMT_P8,              8,     false,
    D3DFMT_R3G3B2,          8,     false,
    D3DFMT_A1R5G5B5,        16,    true,
    D3DFMT_A4R4G4B4,        16,    true,
    D3DFMT_A8L8,            16,    true,
    D3DFMT_A8P8,            16,    true,
    D3DFMT_A8R3G3B2,        16,    true,
    D3DFMT_L16,             16,    false,
    D3DFMT_R5G6B5,          16,    false,
    D3DFMT_X1R5G5B5,        16,    false,
    D3DFMT_X4R4G4B4,        16,    false,
    D3DFMT_R8G8B8,          24,    false,
    D3DFMT_A2B10G10R10,     32,    true,
    D3DFMT_A2R10G10B10,     32,    true,
    D3DFMT_A8B8G8R8,        32,    true,
    D3DFMT_A8R8G8B8,        32,    true,
    D3DFMT_G16R16,          32,    false,
    D3DFMT_X8B8G8R8,        32,    false,
    D3DFMT_X8R8G8B8,        32,    false,
    D3DFMT_A16B16G16R16,    64,    true,

    D3DFMT_CxV8U8,          16,    false,
    D3DFMT_V8U8,            16,    false,
    D3DFMT_Q8W8V8U8,        32,    true,
    D3DFMT_V16U16,          32,    false,
    D3DFMT_Q16W16V16U16,    64,    true,

    D3DFMT_L6V5U5,          16,    false,
    D3DFMT_A2W10V10U10,     32,    false,
    D3DFMT_X8L8V8U8,        32,    false,

    D3DFMT_DXT1,            4,     true,
    D3DFMT_DXT2,            8,     false,
    D3DFMT_DXT3,            8,     true,
    D3DFMT_DXT4,            8,     false,
    D3DFMT_DXT5,            8,     true,
    D3DFMT_G8R8_G8B8,       16,    false,
    D3DFMT_R8G8_B8G8,       16,    false,
    D3DFMT_UYVY,            16,    false,
    D3DFMT_YUY2,            16,    false,

    D3DFMT_R16F,            16,    false,
    D3DFMT_G16R16F,         32,    false,
    D3DFMT_A16B16G16R16F,   64,    true,

    D3DFMT_R32F,            32,    false,
    D3DFMT_G32R32F,         64,    false,
    D3DFMT_A32B32G32R32F,   128,   true,
};

const int fmtInfoArraySize = sizeof(fmtInfoArray) / sizeof(fmtInfoArray[0]);

int GetPixelWidth( D3DFORMAT eFormat )
{
	for( int i=0 ; i<fmtInfoArraySize ; i++ )
	{
		if( fmtInfoArray[i].fmt == eFormat )
			return fmtInfoArray[i].nBitsPerTexel;
	}

	return 24;
}

bool IsAlphaFormat( D3DFORMAT eFormat )
{
	for( int i=0 ; i<fmtInfoArraySize ; i++ )
	{
		if( fmtInfoArray[i].fmt == eFormat )
			return (bool)fmtInfoArray[i].bHasAlpha;
	}

	return false;
}

int GetNumberCount( int iNumber )
{
	int iDigit = 0;
	int iAbsNumber = abs( iNumber );
	for( int i=0 ; i<10 ; i++ )
	{
		iAbsNumber = (int)( iAbsNumber / 10 );
		if( iAbsNumber < 1 )
			return i+1;
	}

	return 0;
}

int GetNumberPos( int iNumber, int iDigit )
{
	int iPowValue = 1;
	for( int i=0 ; i<iDigit ; i++ )
	{
		iPowValue *= 10;
	}

	int iResult = (int)( abs(iNumber) / iPowValue );
	return iResult % 10;
}

int CalcVertexStride( DWORD dwMask )
{
	int iVtxWidth = 0;

	if( dwMask & IOFVF_POSITION )  iVtxWidth += 3;
	if( dwMask & IOFVF_POSITIONW ) iVtxWidth += 4;
	if( dwMask & IOFVF_NORMAL )	   iVtxWidth += 3;
	if( dwMask & IOFVF_TANGENT )   iVtxWidth += 3;
	if( dwMask & IOFVF_BINORMAL )  iVtxWidth += 3;
	if( dwMask & IOFVF_COLOR0 )	   iVtxWidth += 1;
	if( dwMask & IOFVF_COLOR1 )	   iVtxWidth += 1;
	if( dwMask & IOFVF_UV0 )	   iVtxWidth += 2;
	if( dwMask & IOFVF_UV1 )	   iVtxWidth += 2;
	if( dwMask & IOFVF_UV2 )	   iVtxWidth += 2;
	if( dwMask & IOFVF_UV3 )	   iVtxWidth += 2;
	if( dwMask & IOFVF_WEIGHTS )   iVtxWidth += 4;
	if( dwMask & IOFVF_INDICES )   iVtxWidth += 1;
	if( dwMask & IOFVF_POSITION2 ) iVtxWidth += 3;

	return iVtxWidth * sizeof(float);
}

HRESULT SaveSurfaceToFile( IDirect3DSurface9 *pSrc,
						   UINT uWidth,
						   UINT uHeight,
						   bool bSource16Bits,
						   const char *szFileName )
{
	HRESULT hr;

	D3DLOCKED_RECT rcSrc;
	hr = pSrc->LockRect( &rcSrc, NULL, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY );
	if( FAILED(hr) )
		return hr;

	ilInit();
	ilEnable( IL_FILE_OVERWRITE );
	ilSetInteger( IL_JPG_QUALITY, 99 );	// 0 ~ 99

	ILuint uHandle = 0;
	ilGenImages( 1, &uHandle );
	ilBindImage( uHandle );

	if( ilTexImage( uWidth, uHeight, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL ) )
	{
		DWORD *pDestBuf = (DWORD*)ilGetData();

		if( bSource16Bits )
		{
			WORD *pSrcBuf = (WORD*)rcSrc.pBits;
			UINT uPitch = rcSrc.Pitch >> 1;

			pDestBuf += uWidth * (uHeight - 1);	// for flip

			WORD wSrcColor = 0;
			for( UINT uY=0 ; uY<uHeight ; uY++ )
			{
				for( UINT uX=0 ; uX<uWidth ; uX++ )
				{
					wSrcColor = pSrcBuf[uX];

					pDestBuf[uX] =  ( ( wSrcColor & 0xf100 ) << 8 ) |
									( ( wSrcColor & 0x07e0 ) << 5 ) |
									( ( wSrcColor & 0x001f ) << 3 );
				}

				pSrcBuf  += uPitch;
				pDestBuf -= uWidth;
			}
		}
		else
		{
			BYTE *pSrcBuf = (BYTE*)rcSrc.pBits;
			UINT uPitch   = rcSrc.Pitch;
			UINT uCopyBytes = uWidth * sizeof(DWORD);

			pDestBuf += uWidth * (uHeight - 1);	// for flip

			for( UINT uY=0; uY<uHeight ; uY++ )
			{
				ioCPU::FastMemcpy( pDestBuf, pSrcBuf, uCopyBytes );

				pSrcBuf  += uPitch;
				pDestBuf -= uWidth;
			}
		}
	}

	pSrc->UnlockRect();

	if( ilSaveImage((ILstring) szFileName) )
		hr = S_OK;
	else
		hr = E_FAIL;

	ilDeleteImages( 1, &uHandle );
	ilShutDown();

	return hr;
}

int StringCutFunc( float fScale, float fWidth, int iTextStyle, OUT char *szDst, int iDstSize, const char *szSrc )
{
	int iLen = strlen( szSrc );
	char szTemp[MAX_PATH * 2];
	memset( szTemp, 0, sizeof( szTemp ) );

	int iBack = 0;
	int iTemp = 0;
	int i = 0;
	for(;i < iLen;)
	{
		if( iTemp >= (MAX_PATH*2) )
			return iBack;

		iBack = i;
		szTemp[iTemp++]   = szSrc[i++];

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szTemp[iTemp - 1] ) )
#else
		if(false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szTemp[iTemp - 1] ) )
#endif

#else
		if( IsDBCSLeadByte( (BYTE)szTemp[iTemp - 1] ) )
#endif

			szTemp[iTemp++] = szSrc[i++];

		if( g_FontMgr.GetTextWidth( szTemp, (TextStyle)iTextStyle, fScale ) >= fWidth )
			return iBack;
		StringCbCopy( szDst, iDstSize, szTemp );
	}

	return iLen;
}
