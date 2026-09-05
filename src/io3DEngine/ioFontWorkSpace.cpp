

#include "stdafx.h"

#include "ioVertexFormat.h"

#include "ioRenderSystem.h"
#include "ioTexture.h"
#include "ioTextureManager.h"
#include "ioUIRenderer.h"

#include "ioFont.h"
#include "ioFontWorkSpace.h"
#include "ioFrameTimer.h"
#include "ioFontManager.h"

#include "HelpFunc.h"
#include "ioCPU.h"

#include "../ioFreeType/ioFreeType.h"

WorkTexture::WorkTexture()
{
	m_pTex  = NULL;
	m_pNext = NULL;

	m_pImage = new WORD[ TEX_WIDTH * TEX_HEIGHT ];
	ClearImageBuffer();
}

WorkTexture::~WorkTexture()
{
	DESTROY_TEXTURE( m_pTex );
	SAFEDELETEARRAY( m_pImage );
}

void WorkTexture::NeedUpdate( int iTop, int iBottom )
{
	if( m_iTop == -1 )
		m_iTop = iTop;
	else
		m_iTop = min( m_iTop, iTop );

	if( m_iBottom == -1 )
		m_iBottom = iBottom;
	else
		m_iBottom = max( m_iBottom, iBottom );
}

void WorkTexture::UpdateTexture()
{
	if( m_iTop == m_iBottom )	return;
	if( !m_pTex )	return;

	int iPitch = 0;
	WORD *pBits = (WORD*)m_pTex->Lock( &iPitch, 0, 0, NULL );
	if( !pBits )	return;

	iPitch >>= 1;	// for 16bit

	WORD *pSrc, *pDest;
	pSrc  = m_pImage + m_iTop * TEX_WIDTH;
	pDest = pBits + m_iTop * iPitch;

	int iHeight = min( m_iBottom + 1, TEX_HEIGHT ) - m_iTop;
	for( int i=0 ; i<iHeight ; i++ )
	{
		ioCPU::FastMemcpy( pDest, pSrc, TEX_WIDTH * sizeof(WORD) );

		pSrc  += TEX_WIDTH;
		pDest += iPitch;
	}

	m_pTex->UnLock();

	m_iTop = m_iBottom = -1;
}

void WorkTexture::ClearImageBuffer()
{
	if( m_pImage )
	{
		ZeroMemory( m_pImage, TEX_WIDTH * TEX_HEIGHT * sizeof(WORD) );
	}

	m_iTop = m_iBottom = -1;
}

ioTextPiece::ioTextPiece()
{
	m_pFont = NULL;

	m_dwTextColor = 0xffffffff;
	m_dwBkColor   = 0xff000000;
	m_TextStyle   = TS_NORMAL;
	m_iFontGap    = 0;

	m_pTex = NULL;

	m_iWidth  = 0;
	m_iHeight = 0;
	m_iFirstBearingX = 0;

	m_iUpdateCnt = 0;
}

ioTextPiece::ioTextPiece( ioFont *pFont, const char *szText )
 : m_Text( szText )
{
	m_pFont = pFont;

	m_dwTextColor = 0xffffffff;
	m_dwBkColor   = 0xff000000;
	m_TextStyle   = TS_NORMAL;
	m_iFontGap    = 0;

	m_pTex = NULL;

	m_iWidth  = 0;
	m_iHeight = 0;
	m_iFirstBearingX = 0;

	m_iUpdateCnt = 0;
}

ioTextPiece::~ioTextPiece()
{	
}

bool ioTextPiece::operator<( const ioTextPiece &rhs ) const
{
	if( m_Text < rhs.m_Text )
		return true;
	else if( m_Text == rhs.m_Text )
	{
		if( m_dwTextColor < rhs.m_dwTextColor )
			return true;
		else if( m_dwTextColor == rhs.m_dwTextColor )
		{
			if( m_TextStyle < rhs.m_TextStyle )
				return true;
			else if( m_TextStyle == rhs.m_TextStyle )
			{
				if( m_dwBkColor < rhs.m_dwBkColor )
					return true;
				else if( m_dwBkColor == rhs.m_dwBkColor )
				{
					if( m_iFontGap < rhs.m_iFontGap )
						return true;
					else if( m_iFontGap == rhs.m_iFontGap )
					{
						return m_pFont < rhs.m_pFont;
					}
				}
			}			
		}
	}

	return false;
}

//--------------------------------------------------------------------------

int ioFontWorkSpace::m_iTexCreateCnt = 0;

ioFontWorkSpace::ioFontWorkSpace()
{
	m_iStartU = m_iStartV = 0;
	m_iTop = m_iBottom = 0;
	m_iNeedHeight = 0;

	m_pLineBuf = NULL;
	m_iLineBufWidth = 0;
	m_iLineBufHeight = 0;

	CheckLineBuffer( 512, 64 );
	ClearLineBuffer();

	m_pCurTex = m_pPivotTex = CreateNewTexture();
	m_pCurTex->m_pNext = m_pCurTex;

	m_dwTextColor = 0xffffffff;
	m_dwBkColor   = 0xff000000;
	m_TextStyle   = TS_NORMAL;
	m_iFontGap    = 0;

	float fInvAlphaRate = 1.0f;
	for( int i=0 ; i<DRAW_COLOR_TABLE ; i++ )
	{
		for( int j=0 ; j<DRAW_COLOR_TABLE ; j++ )
		{
			fInvAlphaRate = 1.0f - (float)j/(float)(DRAW_COLOR_TABLE-1);
			m_AlphaColorTable[i][j] = (float)i * fInvAlphaRate + FLOAT05;
		}
	}

	memset( m_aDrawColor, 0, sizeof(m_aDrawColor) );
}

ioFontWorkSpace::~ioFontWorkSpace()
{
	SAFEDELETEARRAY( m_pLineBuf );

	WorkTexture *pCursor, *pTemp;
	pCursor = m_pCurTex;

	do
	{
		pTemp = pCursor;
		pCursor = pCursor->m_pNext;

		SAFEDELETE( pTemp );
	}while( pCursor != m_pCurTex );

	ioTextPieceSet::iterator iter;
	for( iter = m_PieceList.begin() ; iter!=m_PieceList.end() ; ++iter )
	{
		delete *iter;
	}
	m_PieceList.clear();
}

ioTextPiece* ioFontWorkSpace::GetTextPiece( ioFont *pFont, const char *szText )
{
	ioTextPiece kFindPiece( pFont, szText );
	kFindPiece.m_dwTextColor = m_dwTextColor;
	kFindPiece.m_dwBkColor   = m_dwBkColor;
	kFindPiece.m_TextStyle   = m_TextStyle;
	kFindPiece.m_iFontGap    = m_iFontGap;

	ioTextPiece *pPiece = NULL;
	ioTextPieceSet::iterator iter = m_PieceList.find( &kFindPiece );
	if( iter != m_PieceList.end() )
	{
		pPiece = *iter;
		pPiece->m_iUpdateCnt = g_FrameTimer.GetFrameCounter();
	}

	if( !pPiece )
	{
		pPiece = new ioTextPiece( pFont, szText );
		pPiece->m_dwTextColor = m_dwTextColor;
		pPiece->m_dwBkColor   = m_dwBkColor;
		pPiece->m_TextStyle   = m_TextStyle;
		pPiece->m_iFontGap    = m_iFontGap;
		pPiece->m_iUpdateCnt  = g_FrameTimer.GetFrameCounter();
		m_PieceList.insert( pPiece );

		WriteTexture( pPiece );	
	}

	return pPiece;
}

POINT ioFontWorkSpace::GetStyleAddPixel( TextStyle eStyle )
{
	POINT ptAdd;
	ptAdd.x = ptAdd.y = 0;

	switch( eStyle )
	{
	case TS_NORMAL:
		// 0, 0
		break;
	case TS_SHADOW:
		ptAdd.x = ptAdd.y = 1;
		break;
	case TS_OUTLINE:
		ptAdd.x = ptAdd.y = 2;
		break;
	case TS_OUTLINE_2X:
		ptAdd.x = ptAdd.y = 4;
		break;
	case TS_OUTLINE_FULL:
		ptAdd.x = ptAdd.y = 2;
		break;
	case TS_OUTLINE_FULL_2X:
		ptAdd.x = ptAdd.y = 4;
		break;
	case TS_OUTLINE_FULL_3X:
		ptAdd.x = ptAdd.y = 6;
		break;
	case TS_OUTLINE_FULL_4X:
		ptAdd.x = ptAdd.y = 8;
		break;
	case TS_BOLD:
		ptAdd.x = 1;
		ptAdd.y = 0;
		break;
	case TS_BOLD_SHADOW:
		ptAdd.x = 2;
		ptAdd.y = 1;
		break;
	case TS_BOLD_OUTLINE:
		ptAdd.x = 3;
		ptAdd.y = 2;
		break;
	case TS_BOLD_OUTLINE_FULL:
		ptAdd.x = 3;
		ptAdd.y = 2;
		break;
	case TS_BOLD_OUTLINE_FULL_2X:
		ptAdd.x = 5;
		ptAdd.y = 4;
		break;
	case TS_BOLD_OUTLINE_FULL_3X:
		ptAdd.x = 7;
		ptAdd.y = 6;
		break;
	case TS_BOLD_OUTLINE_FULL_4X:
		ptAdd.x = 9;
		ptAdd.y = 8;
		break;
	}

	return ptAdd;
}

POINT ioFontWorkSpace::GetStyleAddTailPixel( TextStyle eStyle )
{
	POINT ptAdd;
	ptAdd.x = ptAdd.y = 0;

	switch( eStyle )
	{
	case TS_NORMAL:
		// 0, 0
		break;
	case TS_SHADOW:
		ptAdd.x = ptAdd.y = 1;
		break;
	case TS_OUTLINE:
		ptAdd.x = ptAdd.y = 1;
		break;
	case TS_OUTLINE_2X:
		ptAdd.x = ptAdd.y = 2;
		break;
	case TS_OUTLINE_FULL:
		ptAdd.x = ptAdd.y = 1;
		break;
	case TS_OUTLINE_FULL_2X:
		ptAdd.x = ptAdd.y = 2;
		break;
	case TS_OUTLINE_FULL_3X:
		ptAdd.x = ptAdd.y = 3;
		break;
	case TS_OUTLINE_FULL_4X:
		ptAdd.x = ptAdd.y = 4;
		break;
	case TS_BOLD:
		ptAdd.x = 1;
		ptAdd.y = 0;
		break;
	case TS_BOLD_SHADOW:
		ptAdd.x = 2;
		ptAdd.y = 1;
		break;
	case TS_BOLD_OUTLINE:
		ptAdd.x = 2;
		ptAdd.y = 1;
		break;
	case TS_BOLD_OUTLINE_FULL:
		ptAdd.x = 2;
		ptAdd.y = 1;
		break;
	case TS_BOLD_OUTLINE_FULL_2X:
		ptAdd.x = 3;
		ptAdd.y = 2;
		break;
	case TS_BOLD_OUTLINE_FULL_3X:
		ptAdd.x = 4;
		ptAdd.y = 3;
		break;
	case TS_BOLD_OUTLINE_FULL_4X:
		ptAdd.x = 5;
		ptAdd.y = 4;
		break;
	}

	return ptAdd;
}

int ioFontWorkSpace::CalculateTextWidth( const char *szText,
										 const ioFont *pFont,
										 TextStyle eStyle,
										 int iFontGap )
{
	UINT iCodePage = GetACP();
	if( iCodePage == 1252 ) // 1252 : english
		iCodePage = 949;    // 949  : korean ( 한국어와영어는 폰트 공유 )

	bool bFirstChar = true;
	int iNeedWidth = 0;

	const GlyphImg *pImg = NULL;
	for( int i=0 ; szText[i] != '\0' ; )
	{
		WORD wCode = (BYTE)szText[i++];
		if( IsDBCSLeadByteEx( iCodePage, (BYTE)wCode ) && szText[i] != NULL )
		{
			wCode = wCode << 8 | (BYTE)szText[i++];
		}

		pImg = pFont->GetGlyphImg( wCode );
		if( !pImg ) continue;

		if( pImg->iBearingX < 0 && bFirstChar )
		{
			iNeedWidth += pImg->iAdvance - pImg->iBearingX;
		}
		else
		{
			iNeedWidth += pImg->iAdvance;
		}

		if( !bFirstChar )
		{
			iNeedWidth += iFontGap;
		}

		bFirstChar = false;
	}

	if( pImg )
	{
		// 마지막 글자가 iAdvance 보다 삐져 나오는 경우
		if( pImg->NeedOriginDrawWidth() > pImg->iAdvance )
		{
			iNeedWidth += pImg->NeedOriginDrawWidth() - pImg->iAdvance;
		}
	}

	iNeedWidth += GetStyleAddPixel( eStyle ).x;

	return iNeedWidth;
}

int ioFontWorkSpace::CalculateTextWidth( const char *szText, const ioFont *pFont )
{
	return CalculateTextWidth( szText, pFont, m_TextStyle, m_iFontGap );
}

int ioFontWorkSpace::CalculateTextHeight( const char *szText, const ioFont *pFont, TextStyle eStyle )
{
	UINT iCodePage = GetACP();

#if !defined( SRC_OVERSEAS )
	if( iCodePage == 1252 ) // 1252 : english
		iCodePage = 949;    // 949  : korean ( 한국어와영어는 폰트 공유 )
#endif

	int iNeedHeight = 0;

	const GlyphImg *pImg = NULL;
	for( int i=0 ; szText[i] != '\0' ; )
	{
		WORD wCode = (BYTE)szText[i++];
		if( IsDBCSLeadByteEx( iCodePage, (BYTE)wCode ) && szText[i] != NULL )
		{
			wCode = wCode << 8 | (BYTE)szText[i++];
		}

		pImg = pFont->GetGlyphImg( wCode );
		if( !pImg ) continue;

		iNeedHeight = std::max<int>( iNeedHeight, pImg->iHeight );
	}

	iNeedHeight += GetStyleAddPixel( eStyle ).y;

	return iNeedHeight;
}

void ioFontWorkSpace::Flush()
{
	m_pCurTex->NeedUpdate( m_iTop, m_iBottom );

	WorkTexture *pCursor, *pTemp;
	pCursor = m_pCurTex;
	
	do
	{
		pTemp = pCursor;
		pCursor = pCursor->m_pNext;

		if( !pTemp->m_pTex )
		{
			pTemp->m_pTex = CreateD3DTexture();
			pTemp->NeedUpdate( 0, TEX_HEIGHT );
		}

		pTemp->UpdateTexture();

	}while( pCursor != m_pCurTex );

	m_iTop = m_iBottom = m_iStartV;
	m_pPivotTex = m_pCurTex;
}

WorkTexture* ioFontWorkSpace::CreateNewTexture()
{
	WorkTexture *pWorkTex = new WorkTexture;
	pWorkTex->m_pTex = CreateD3DTexture();

	return pWorkTex;
}

ioTexture* ioFontWorkSpace::CreateD3DTexture()
{
	char szName[MAX_PATH];
	wsprintf( szName, "FontWorkSpaceTex%d", m_iTexCreateCnt );

	ioTexture *pTex = NULL;
	pTex = g_TextureMgr.CreateManual( szName,
									  TEX_WIDTH,
									  TEX_HEIGHT,
									  D3DFMT_A4R4G4B4,
									  D3DUSAGE_DYNAMIC,
									  1 );

	if( !pTex )
	{
		pTex = g_TextureMgr.CreateManual( szName,
										  TEX_WIDTH,
										  TEX_HEIGHT,
										  D3DFMT_A4R4G4B4,
										  0,
										  1 );
	}

	m_iTexCreateCnt++;
	return pTex;
}

void ioFontWorkSpace::WriteTexture( ioTextPiece *pPiece )
{
	ioFont *pFont = pPiece->m_pFont;
	if( !pFont )	return;

	int iNeedWidth = 0;
	int iNeedHeight = pFont->GetMaxHeight();

	bool bFirstChar = true;
	const char* szText = pPiece->m_Text.c_str();

	LineCopyInfo kGlyphInfo;
	const GlyphImg *pImg = NULL;

	UINT iCodePage = GetACP();

#if !defined( SRC_OVERSEAS )
	if( iCodePage == 1252 ) // 1252 : english
		iCodePage = 949;    // 949  : korean ( 한국어와영어는 폰트 공유 )
#endif

	for( int i=0 ; szText[i] != '\0' ; )
	{
		WORD wCode = (BYTE)szText[i++];
		if( IsDBCSLeadByteEx( iCodePage, (BYTE)wCode )  && szText[i] != NULL )
		{
			wCode = wCode << 8 | (BYTE)szText[i++];
		}

		pImg = pFont->GetGlyphImg( wCode );
		if( !pImg ) continue;

		kGlyphInfo.pImg    = pImg;
		kGlyphInfo.iStartX = 0;

		if( pImg->iBearingX < 0 && bFirstChar )
		{
			pPiece->m_iFirstBearingX = pImg->iBearingX;

			kGlyphInfo.iStartX = -pImg->iBearingX;
			iNeedWidth += pImg->iAdvance - pImg->iBearingX;
		}
		else
		{
			kGlyphInfo.iStartX = iNeedWidth + pImg->iBearingX;
			iNeedWidth += pImg->iAdvance;
		}

		if( !bFirstChar )
		{
			iNeedWidth += pPiece->m_iFontGap;
		}

		m_LineCopyList.push_back( kGlyphInfo );
		bFirstChar = false;
	}

	if( pImg )
	{
		// 마지막 글자가 iAdvance 보다 삐져 나오는 경우
		if( pImg->NeedOriginDrawWidth() > pImg->iAdvance )
		{
			iNeedWidth += pImg->NeedOriginDrawWidth() - pImg->iAdvance;
		}
	}

	POINT ptStyleAdd  = GetStyleAddPixel( pPiece->m_TextStyle );
	iNeedWidth  += ptStyleAdd.x;
	iNeedHeight += ptStyleAdd.y;

	CheckLineBuffer( iNeedWidth + 24, iNeedHeight + 24 );
	ClearLineBuffer();

	POINT ptStyleTail = GetStyleAddTailPixel( pPiece->m_TextStyle );
	WriteLineByStyle( ptStyleAdd.x - ptStyleTail.x,
					  ptStyleAdd.y - ptStyleTail.y,
					  pPiece );

	if( !m_LineCopyList.empty() )
		m_LineCopyList.erase( m_LineCopyList.begin(), m_LineCopyList.end() );

	pPiece->m_iWidth  = iNeedWidth;
	pPiece->m_iHeight = iNeedHeight;
	pPiece->m_pTex    = m_pCurTex;

	if( !pPiece->m_vRenderQuad.empty() )
		pPiece->m_vRenderQuad.erase( pPiece->m_vRenderQuad.begin(), pPiece->m_vRenderQuad.end() );

	if( m_iNeedHeight < iNeedHeight )
		m_iNeedHeight = iNeedHeight;

	int iRemainWidth = iNeedWidth;
	while( iRemainWidth > 0 )
	{
		if( IsPageOver( m_iNeedHeight ) )
		{
			// 폰트의 크기가 23 ~ 29를 왔다 갔다 하므로 마지막 라인에서 23폰트로 진행되던 텍스트가 29폰트로 인해 페이지 오버 되면 
			// 중간까지 썼던 텍스트는 m_iStartV만으로는 업데이트 되지 않는다.
			// m_pCurTex->NeedUpdate( m_iTop, m_iStartV );         

			if( m_pCurTex->m_pNext == m_pPivotTex )
			{
				WorkTexture *pNewTex = CreateNewTexture();
				pNewTex->m_pNext = m_pCurTex->m_pNext;
				m_pCurTex->m_pNext = pNewTex;
			}
			else
			{
				// Over체크 및 페이지 관리
				RemovePage( m_pCurTex->m_pNext );
			}

			m_pCurTex = m_pCurTex->m_pNext;
			m_pCurTex->ClearImageBuffer();

			m_iNeedHeight = iNeedHeight;
			m_iStartU = m_iStartV = 0;
			m_iTop = m_iBottom = 0;

			// 초장부터 새 텍스쳐
			if( iRemainWidth == iNeedWidth )
			{
				pPiece->m_pTex = m_pCurTex;
			}
		}

		if( IsLineOver( iRemainWidth ) )
		{
			// 이전 문자열에서 511까지 카피하고 번짐방지로 +1해서 m_iStartU가 512인 경우 
			// iCapCopyWidth == 0 이다. 이경우 iRemainWidth에 변화가 없기때문에 iNeedWidth가 같아져서
			// 위쪽의 '초장부터 새 텍스쳐'로 오인되어 텍스쳐가 교체되버린다.
			// 문제는 FontManager에서 렌더링시 두번째 Quad의 U와 V가 0이면 중간에 텍스쳐가 교체된것으로 생각하고
			// 다음텍스쳐를 사용하게되는데 '초장부터'에서 의도치않게 텍스쳐가 교체되버렸기때문에 두번교체되어
			// 원래 필요한 텍스쳐가 아닌 다른 텍스쳐가 사용된다.
			// 해서 가로가 0인 quad는 '초장부터'에서 교체되게 하고
			// 아예 RenderQuad에 넣지 않게해서 FontManager에서 교체가 안일어나게 하기위해 iCanCopyWidth == 0인것은 제외시킨다.

			int iCanCopyWidth = TEX_WIDTH - m_iStartU;
			if( iCanCopyWidth > 0 )
			{
				WriteLineToImageBuffer( iNeedWidth - iRemainWidth,
										iCanCopyWidth,
										iNeedHeight,
										m_iStartU,
										m_iStartV );

				ioTextPiece::PieceQuad vQuad;
				vQuad.m_iStartU = m_iStartU;
				vQuad.m_iStartV = m_iStartV;
				vQuad.m_iWidth  = iCanCopyWidth;
				pPiece->m_vRenderQuad.push_back(vQuad );

				iRemainWidth -= iCanCopyWidth;
			}

			m_iStartU = 0;
			m_iStartV += m_iNeedHeight + 1;	// 위아래 글자가 번지는것을 방지

			m_iNeedHeight = iNeedHeight;
		}
		else
		{
			WriteLineToImageBuffer( iNeedWidth - iRemainWidth,
									iRemainWidth,
									iNeedHeight,
									m_iStartU,
									m_iStartV );

			ioTextPiece::PieceQuad vQuad;
			vQuad.m_iStartU = m_iStartU;
			vQuad.m_iStartV = m_iStartV;
			vQuad.m_iWidth  = iRemainWidth;
			pPiece->m_vRenderQuad.push_back(vQuad );

			m_iStartU += iRemainWidth;
			iRemainWidth = 0;
		}
	}

	if( m_iBottom < m_iStartV + iNeedHeight )
		m_iBottom = m_iStartV + iNeedHeight;

	m_iStartU++;	// 좌우 글자가 번지는 것을 방지

	ioTextPiece *pRestPiece = NULL;
	while( !m_ReserveList.empty() )
	{
		pRestPiece = m_ReserveList.front();
		m_ReserveList.pop_front();
		m_PieceList.insert( pRestPiece );

		WriteTexture( pRestPiece );
	}
}

void ioFontWorkSpace::RemovePage( WorkTexture *pCurTex )
{
	int iCurFrameCount = g_FrameTimer.GetFrameCounter();

	ioTextPieceSet::iterator iter = m_PieceList.begin();
	while( iter != m_PieceList.end() )
	{
		ioTextPiece *pPiece = *iter;

		if( pPiece->m_pTex == pCurTex )
		{
			if( pPiece->m_iUpdateCnt == iCurFrameCount )
			{
				m_ReserveList.push_back( pPiece );
			}
			else
			{
				SAFEDELETE( pPiece );
			}

			iter = m_PieceList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioFontWorkSpace::WriteLineToImageBuffer( int iSrcX,
											  int iWidth, int iHeight,
											  int iDestX, int iDestY )
{
	WORD *pSrc  = m_pLineBuf + iSrcX;
	WORD *pDest = m_pCurTex->GetImageBuf() + (iDestY * TEX_WIDTH + iDestX);

	for( int iY=0 ; iY<iHeight ; iY++ )
	{
		ioCPU::FastMemcpy( pDest, pSrc, iWidth * sizeof(WORD) );

		pSrc  += m_iLineBufWidth;
		pDest += TEX_WIDTH;
	}
	m_pCurTex->NeedUpdate( m_iTop, m_iStartV + iHeight );   
}

void ioFontWorkSpace::WriteLineByOutLine( int x, int y, ioTextPiece *pPiece, int iLineGap, bool bBold /* = false  */ )
{
	// 아웃라인 x1은 텍스트 주변 8방향에 그렸지만
	// 아웃라인 x2 ~ x4는 방식이 바뀌었다.
	/*         
		x2는 동서남북 4방향만 더 찍어주며 한 면에 3개씩 찍는다.
		x3는 동서남북 4방향만 더 찍어주며 한 면에 5개씩 찍는다.
		x4는 동서남북 4방향만 더 찍어주며 한 면에 7개씩 찍는다.
	*/

	if( bBold )
	{
		int i = 0;
		int iStart = -(iLineGap - 1);
		//동
		for(i = iStart;i < iLineGap;i++)
			WriteLineBuffer( x+iLineGap+1, y+i, pPiece );
		//서
		for(i = iStart;i < iLineGap;i++)
			WriteLineBuffer( x-iLineGap, y+i, pPiece );
		//남
		for(i = iStart;i < iLineGap+1;i++)
			WriteLineBuffer( x+i, y+iLineGap, pPiece );
		//북
		for(i = iStart;i < iLineGap+1;i++)
			WriteLineBuffer( x+i, y-iLineGap, pPiece );
	}
	else
	{
		int i = 0;
		int iStart = -(iLineGap - 1);
		//동
		for(i = iStart;i < iLineGap;i++)
			WriteLineBuffer( x+iLineGap, y+i, pPiece );
		//서
		for(i = iStart;i < iLineGap;i++)
			WriteLineBuffer( x-iLineGap, y+i, pPiece );
		//남
		for(i = iStart;i < iLineGap;i++)
			WriteLineBuffer( x+i, y+iLineGap, pPiece );
		//북
		for(i = iStart;i < iLineGap;i++)
			WriteLineBuffer( x+i, y-iLineGap, pPiece );
	}
}

void ioFontWorkSpace::WriteLineByStyle( int x, int y, ioTextPiece *pPiece )
{
	switch( pPiece->m_TextStyle )
	{
	case TS_NORMAL:
		SetLineDrawColor( pPiece->m_dwTextColor, true );
        WriteLineBuffer( x, y, pPiece );
		break;
	case TS_SHADOW:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineBuffer( x+1, y+1, pPiece );

		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		break;
	case TS_OUTLINE:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineBuffer( x+1, y, pPiece );
		WriteLineBuffer( x-1, y, pPiece );
		WriteLineBuffer( x, y+1, pPiece );
		WriteLineBuffer( x, y-1, pPiece );

		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		break;
	case TS_OUTLINE_2X:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineBuffer( x+2, y, pPiece );
		WriteLineBuffer( x-2, y, pPiece );
		WriteLineBuffer( x+1, y-1, pPiece );
		WriteLineBuffer( x+1, y+1, pPiece );
		WriteLineBuffer( x, y+2, pPiece );
		WriteLineBuffer( x, y-2, pPiece );
		WriteLineBuffer( x-1, y-1, pPiece );
		WriteLineBuffer( x-1, y+1, pPiece );

		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		break;
	case TS_OUTLINE_FULL:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineBuffer( x+1, y, pPiece );
		WriteLineBuffer( x+1, y-1, pPiece );
		WriteLineBuffer( x+1, y+1, pPiece );
		WriteLineBuffer( x-1, y, pPiece );
		WriteLineBuffer( x-1, y-1, pPiece );
		WriteLineBuffer( x-1, y+1, pPiece );
		WriteLineBuffer( x, y-1, pPiece );
		WriteLineBuffer( x, y+1, pPiece );
		
		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		break;
	case TS_OUTLINE_FULL_2X:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineByOutLine( x, y, pPiece, 2 );

		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		break;
	case TS_OUTLINE_FULL_3X:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineByOutLine( x, y, pPiece, 3 );

		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		break;
	case TS_OUTLINE_FULL_4X:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineByOutLine( x, y, pPiece, 4 );

		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		break;
	case TS_BOLD:
		SetLineDrawColor( pPiece->m_dwTextColor, true );
		WriteLineBuffer( x+1, y, pPiece, true );
		WriteLineBuffer( x, y, pPiece, true );
		break;
	case TS_BOLD_SHADOW:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineBuffer( x+2, y+1, pPiece );
		
		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		WriteLineBuffer( x+1, y, pPiece );
		break;
	case TS_BOLD_OUTLINE:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineBuffer( x-1, y, pPiece );
		WriteLineBuffer( x, y+1, pPiece );
		WriteLineBuffer( x, y-1, pPiece );
		WriteLineBuffer( x+1, y+1, pPiece );
		WriteLineBuffer( x+1, y-1, pPiece );
		WriteLineBuffer( x+2, y, pPiece );
		
		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		WriteLineBuffer( x+1, y, pPiece );
		break;
	case TS_BOLD_OUTLINE_FULL:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineBuffer( x-1, y+1, pPiece );
		WriteLineBuffer( x-1, y-1, pPiece );
		WriteLineBuffer( x-1, y, pPiece );
		WriteLineBuffer( x, y+1, pPiece );
		WriteLineBuffer( x, y-1, pPiece );
		WriteLineBuffer( x+1, y+1, pPiece );
		WriteLineBuffer( x+1, y-1, pPiece );
		WriteLineBuffer( x+2, y+1, pPiece );
		WriteLineBuffer( x+2, y-1, pPiece );
		WriteLineBuffer( x+2, y, pPiece );
		
		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		WriteLineBuffer( x+1, y, pPiece );		
		break;
	case TS_BOLD_OUTLINE_FULL_2X:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineByOutLine( x, y, pPiece, 2, true );
		
		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		WriteLineBuffer( x+1, y, pPiece );		
		break;
	case TS_BOLD_OUTLINE_FULL_3X:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineByOutLine( x, y, pPiece, 3, true );
		
		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		WriteLineBuffer( x+1, y, pPiece );		
		break;
	case TS_BOLD_OUTLINE_FULL_4X:
		SetLineDrawColor( pPiece->m_dwBkColor );
		WriteLineByOutLine( x, y, pPiece, 4, true );
		
		SetLineDrawColor( pPiece->m_dwTextColor );
		WriteLineBuffer( x, y, pPiece );
		WriteLineBuffer( x+1, y, pPiece );		
		break;
	}
}

void ioFontWorkSpace::SetLineDrawColor( DWORD dwColor, bool bFullColor /* = false  */ )
{
	float fRed, fGreen, fBlue;
	fRed   = (float)( ( dwColor & 0xf00000 ) >> 20 );
	fGreen = (float)( ( dwColor & 0xf000 ) >> 12 );
	fBlue  = (float)( ( dwColor & 0xf0 ) >> 4 );

	float fRate = 1.0f;
	for( int i=0 ; i<DRAW_COLOR_TABLE ; i++ )
	{
		if( !bFullColor )
			fRate = (float)i/(float)(DRAW_COLOR_TABLE-1);
		m_aDrawColor[i].pixel.b = (BYTE)( fBlue  * fRate );		
		m_aDrawColor[i].pixel.g = (BYTE)( fGreen * fRate );
		m_aDrawColor[i].pixel.r = (BYTE)( fRed   * fRate );
		m_aDrawColor[i].pixel.a = i;
	}
}

void ioFontWorkSpace::WriteLineBuffer( int iX, int iY, ioTextPiece *pPiece, bool bDirect /* = false  */ )
{
	ioFont *pFont = pPiece->m_pFont;
	if( !pFont || !m_pLineBuf )	return;

	WORD *pStart = m_pLineBuf + iX + iY * m_iLineBufWidth;
	int  iFontMaxBearingY = pFont->GetMaxBearingY();

	LineCopyList::iterator iter;
	for( iter = m_LineCopyList.begin() ; iter!=m_LineCopyList.end() ; ++iter )
	{
		const GlyphImg *pGlyph = iter->pImg;
		if( !pGlyph->pBuf )	continue;

		WORD *pDest = pStart + iter->iStartX;

		int iImgPitch = ( pGlyph->iWidth + 1 ) / 2;
		int iEvenImgWidth = pGlyph->iWidth / 2;

		int iStartY = max( iFontMaxBearingY - pGlyph->iBearingY, 0 );
		int iImgHeight = pGlyph->iHeight;

		int iSrcIdx;
		WORDColor kDestColor;
		for( int iYLoop=0 ; iYLoop<iImgHeight ; iYLoop++ )
		{
			BYTE *pSrc = (BYTE*)pGlyph->pBuf + iImgPitch * iYLoop;
			WORDColor *pDestBuf = (WORDColor*)( pDest + ( iStartY + iYLoop ) * m_iLineBufWidth );

			int iDestX = 0;
			for( int iXLoop=0 ; iXLoop<iEvenImgWidth ; iXLoop++ )
			{
				iSrcIdx      = ( pSrc[iXLoop] & 0xf0 ) >> 4;
				//UJ 130903, 최적화: pDestBuf[] 접근을 최소화한다
				//kDestColor = pDestBuf[iDestX];
				if( bDirect )
				{
					//kDestColor = m_aDrawColor[iSrcIdx];
					//pDestBuf[iDestX] = kDestColor;
					pDestBuf[iDestX] = m_aDrawColor[iSrcIdx];
				}
				else 
				{
					//UJ 130903, 최적화: WORDColor가 4bits x 4 = 16bits이므로 Short*로 대체한다 --start
					//if( kDestColor.a != m_aDrawColor[iSrcIdx].a ||
					//	 kDestColor.b != m_aDrawColor[iSrcIdx].b ||
					//	 kDestColor.g != m_aDrawColor[iSrcIdx].g ||
					//	 kDestColor.r != m_aDrawColor[iSrcIdx].r )
					kDestColor = pDestBuf[iDestX];
					if( kDestColor.argb != m_aDrawColor[iSrcIdx].argb )
					//UJ --end
					{
						kDestColor.pixel.b = m_AlphaColorTable[kDestColor.pixel.b][iSrcIdx] + m_aDrawColor[iSrcIdx].pixel.b;
						kDestColor.pixel.g = m_AlphaColorTable[kDestColor.pixel.g][iSrcIdx] + m_aDrawColor[iSrcIdx].pixel.g;
						kDestColor.pixel.r = m_AlphaColorTable[kDestColor.pixel.r][iSrcIdx] + m_aDrawColor[iSrcIdx].pixel.r;
						kDestColor.pixel.a = min( 0xf, kDestColor.pixel.a + m_aDrawColor[iSrcIdx].pixel.a );
						pDestBuf[iDestX] = kDestColor;
					}
				}
				iDestX++;

				iSrcIdx      = pSrc[iXLoop] & 0x0f;
				//UJ 130903, 최적화: pDestBuf[] 접근을 최소화한다
				//kDestColor = pDestBuf[iDestX];
				if( bDirect )
				{
					//kDestColor = m_aDrawColor[iSrcIdx];
					//pDestBuf[iDestX] = kDestColor;
					pDestBuf[iDestX] = m_aDrawColor[iSrcIdx];
				}
				else 
				{
					//UJ 130903, 최적화: WORDColor가 4bits x 4 = 16bits이므로 Short*로 대체한다 --start
					//if( kDestColor.a != m_aDrawColor[iSrcIdx].a ||
					//	 kDestColor.b != m_aDrawColor[iSrcIdx].b ||
					//	 kDestColor.g != m_aDrawColor[iSrcIdx].g ||
					//	 kDestColor.r != m_aDrawColor[iSrcIdx].r )
					kDestColor = pDestBuf[iDestX];
					if( kDestColor.argb != m_aDrawColor[iSrcIdx].argb )
					//UJ --end
					{
						kDestColor.pixel.b = m_AlphaColorTable[kDestColor.pixel.b][iSrcIdx] + m_aDrawColor[iSrcIdx].pixel.b;
						kDestColor.pixel.g = m_AlphaColorTable[kDestColor.pixel.g][iSrcIdx] + m_aDrawColor[iSrcIdx].pixel.g;
						kDestColor.pixel.r = m_AlphaColorTable[kDestColor.pixel.r][iSrcIdx] + m_aDrawColor[iSrcIdx].pixel.r;
						kDestColor.pixel.a = min( 0xf, kDestColor.pixel.a + m_aDrawColor[iSrcIdx].pixel.a );
						pDestBuf[iDestX] = kDestColor;
					}
				}
				iDestX++;
			}

			if( pGlyph->iWidth & 1 )
			{
				iSrcIdx      = ( pSrc[iXLoop] & 0xf0 ) >> 4;
				//UJ 130903, 최적화: pDestBuf[] 접근을 최소화한다, bDirect에서는 바로 쓰도록함
				//kDestColor = pDestBuf[iDestX];
				if( bDirect )
				{
					//kDestColor = m_aDrawColor[iSrcIdx];
					//pDestBuf[iDestX] = kDestColor;
					pDestBuf[iDestX] = m_aDrawColor[iSrcIdx];
				}
				else
				{
					//UJ 130903, 최적화: WORDColor가 4bits x 4 = 16bits이므로 Short*로 대체한다 --start
					//if( kDestColor.a != m_aDrawColor[iSrcIdx].a ||
					//	 kDestColor.b != m_aDrawColor[iSrcIdx].b ||
					//	 kDestColor.g != m_aDrawColor[iSrcIdx].g ||
					//	 kDestColor.r != m_aDrawColor[iSrcIdx].r )
					kDestColor = pDestBuf[iDestX];
					if( kDestColor.argb != m_aDrawColor[iSrcIdx].argb )
					//UJ --end
					{
						kDestColor.pixel.b = m_AlphaColorTable[kDestColor.pixel.b][iSrcIdx] + m_aDrawColor[iSrcIdx].pixel.b;
						kDestColor.pixel.g = m_AlphaColorTable[kDestColor.pixel.g][iSrcIdx] + m_aDrawColor[iSrcIdx].pixel.g;
						kDestColor.pixel.r = m_AlphaColorTable[kDestColor.pixel.r][iSrcIdx] + m_aDrawColor[iSrcIdx].pixel.r;
						kDestColor.pixel.a = min( 0xf, kDestColor.pixel.a + m_aDrawColor[iSrcIdx].pixel.a );
						pDestBuf[iDestX] = kDestColor;
					}
				}
				iDestX++;
			}
		}
	}
}

void ioFontWorkSpace::CheckLineBuffer( int iNeedWidth, int iNeedHeight )
{
	if( m_iLineBufWidth >= iNeedWidth && m_iLineBufHeight >= iNeedHeight )
		return;

	SAFEDELETEARRAY( m_pLineBuf );

	m_iLineBufWidth  = max( iNeedWidth,  m_iLineBufWidth );
	m_iLineBufHeight = max( iNeedHeight, m_iLineBufHeight );
	m_pLineBuf = new WORD[ m_iLineBufWidth * m_iLineBufHeight ];
}

void ioFontWorkSpace::ClearLineBuffer()
{
	if( m_pLineBuf )
	{
		memset( m_pLineBuf, 0, sizeof(WORD) * m_iLineBufWidth * m_iLineBufHeight );
	}
}

void ioFontWorkSpace::RenderWorkTexture( int iXPos, int iYPos, float fScale )
{
	ioRenderSystem &rkSystem = RenderSystem();

	rkSystem.SetFixedFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	rkSystem.DisableShader();
	rkSystem.ClearStreamDesc();

	rkSystem.SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );

	rkSystem.SetTextureFiltering( 0, TFO_NEAREST );
	rkSystem.SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	rkSystem.SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );

	ioVtxRHWTex vVtx[4];
	WorkTexture *pWorkTex = m_pCurTex;
	do
	{
		vVtx[0].vPos.x = iXPos + TEX_WIDTH * fScale;
		vVtx[0].vPos.y = iYPos + TEX_HEIGHT * fScale;
		vVtx[0].vPos.z = 0.0f;
		vVtx[0].fRHW = 1.0f;
		vVtx[0].u = 1.0f;
		vVtx[0].v = 1.0f;

		vVtx[1].vPos.x = iXPos;
		vVtx[1].vPos.y = iYPos + TEX_HEIGHT * fScale;
		vVtx[1].vPos.z = 0.0f;
		vVtx[1].fRHW = 1.0f;
		vVtx[1].u = 0.0f;
		vVtx[1].v = 1.0f;

		vVtx[2].vPos.x = iXPos;
		vVtx[2].vPos.y = iYPos;
		vVtx[2].vPos.z = 0.0f;
		vVtx[2].fRHW = 1.0f;
		vVtx[2].u = 0.0f;
		vVtx[2].v = 0.0f;

		vVtx[3].vPos.x = iXPos + TEX_WIDTH * fScale;
		vVtx[3].vPos.y = iYPos;
		vVtx[3].vPos.z = 0.0f;
		vVtx[3].fRHW = 1.0f;
		vVtx[3].u = 1.0f;
		vVtx[3].v = 0.0f;

		rkSystem.SetioTexture( 0, pWorkTex->m_pTex );
		rkSystem.DisableTextureUnitsFrom( 1 );
		rkSystem.GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vVtx, sizeof(ioVtxRHWTex) );

		pWorkTex = pWorkTex->m_pNext;
		iXPos += TEX_WIDTH * fScale + 2.0f;
	}while( pWorkTex != m_pCurTex );
}
