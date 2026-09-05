

#ifndef _HelpFunc_h_
#define _HelpFunc_h_

__EX D3DXVECTOR3 operator*( const D3DXQUATERNION &qt, const D3DXVECTOR3 &vVec );
__EX D3DXVECTOR3 operator*( const D3DXVECTOR3 &vLeft, const D3DXVECTOR3 &vRight );

__EX D3DCOLORVALUE operator*( const D3DCOLORVALUE &lhs, const D3DCOLORVALUE &rhs );
__EX D3DCOLORVALUE operator+( const D3DCOLORVALUE &lhs, const D3DCOLORVALUE &rhs );
__EX D3DCOLORVALUE operator*( const D3DCOLORVALUE &lhs, float fValue );

__EX bool operator==( const D3DCOLORVALUE &lhs, const D3DCOLORVALUE &rhs );
__EX bool operator!=( const D3DCOLORVALUE &lhs, const D3DCOLORVALUE &rhs );

__EX void StringHeadBlankClear( char *szBuf );
__EX bool IsComment( const char *szBuf );

__EX void FillVtxRectFan( ioVtxTex *pVtx, float fMaxU = 1.0f, float fMaxV = 1.0f );

__EX void FillVtxRHWTexRectStrip( ioVtxRHWTex *pVtx, float fXPos, float fYPos,
													 float fWidth, float fHeight,
													 float fMaxU = 1.0f, float fMaxV = 1.0f );

__EX void FillVtxRHWRectStrip( ioVtxRHW *pVtx, float fXPos, float fYPos, float fWidth, float fHeight );

__EX int GetTwoPowAlignSize( int iSize );	// √÷¥Î 4096
__EX int GetPixelWidth( D3DFORMAT eFormat );
__EX bool IsAlphaFormat( D3DFORMAT eFormat );

__EX int GetNumberCount( int iNumber );
__EX int GetNumberPos( int iNumber, int iDigit );

__EX int CalcVertexStride( DWORD dwMask );

__EX HRESULT SaveSurfaceToFile( IDirect3DSurface9 *pSrc,
								UINT uWidth,
								UINT uHeight,
								bool bSource16Bits,
								const char *szFileName );

__EX int StringCutFunc( float fScale, float fWidth, int iTextStyle, OUT char *szDst, int iDstSize, const char *szSrc );

#define PixelAligned(x)	( (float)(int)(x) - 0.5f )
#define LSC_FILE_CUT_RATE     0.2f

#endif