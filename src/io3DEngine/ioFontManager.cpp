

#include "stdafx.h"

#include "ioTexture.h"
#include "ioUIRenderer.h"

#include "ioFont.h"
#include "ioFontWorkSpace.h"
#include "ioFontManager.h"
#include "ioMemoryPool.h"
#include "HelpFunc.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "safesprintf.h"
#include "iolocalmanagerparent.h"

#include "../ioFreeType/ioFreeType.h"
#include <strsafe.h>

class ioFontQuad : public IUIQuad, public ioPoolObject< ioFontQuad, 1024 >
{
public:
	short int m_iDrawX;
	short int m_iDrawY;

	float m_fDrawScale;
	DWORD m_dwTextAlpha;

	ioTextPiece *m_pPiece;

public:
	virtual void AddInfo( ioQuadRenderArray *pQuadArray );
	virtual int GetQuadCount() const;

public:
	ioFontQuad(){}
	virtual ~ioFontQuad(){}
};

void ioFontQuad::AddInfo( ioQuadRenderArray *pQuadArray )
{
	D3DXVECTOR4 vTexUV;
	D3DXVECTOR2 vDrawPos, vDrawSize;

	WorkTexture *pWorkTex = m_pPiece->m_pTex;

	int iArrangedX = m_iDrawX;
	if( m_pPiece->m_iFirstBearingX < 0 )
	{
		iArrangedX += m_pPiece->m_iFirstBearingX;
	}

	float fArrangedX = PixelAligned( iArrangedX );
	float fArrangedY = PixelAligned( m_iDrawY );
	
	bool bFirst = true;
	ioTextPiece::PieceQuadList::iterator iter;
	for( iter=m_pPiece->m_vRenderQuad.begin() ; iter!=m_pPiece->m_vRenderQuad.end() ; ++iter )
	{
		const ioTextPiece::PieceQuad &rkQuad = *iter;

		// 두페이지에 걸쳐 분포되있는가..?
		if( !bFirst )
		{
			if( rkQuad.m_iStartU == 0 && rkQuad.m_iStartV == 0 )
			{
				pWorkTex = pWorkTex->m_pNext;
			}
		}

		vDrawPos.x = fArrangedX;
		vDrawPos.y = fArrangedY;

		vDrawSize.x = rkQuad.m_iWidth * m_fDrawScale;
		vDrawSize.y = m_pPiece->m_iHeight * m_fDrawScale;

		vTexUV.x = (float)rkQuad.m_iStartU / (float)TEX_WIDTH;
		vTexUV.y = (float)rkQuad.m_iStartV / (float)TEX_HEIGHT;
		vTexUV.z = vTexUV.x + (float)rkQuad.m_iWidth / (float)TEX_WIDTH;
		vTexUV.w = vTexUV.y + (float)m_pPiece->m_iHeight / (float)TEX_HEIGHT;

		IDirect3DTexture9 *pD3DTex = NULL;
		if( pWorkTex->m_pTex )
		{
			pD3DTex = pWorkTex->m_pTex->GetTexture();
		}

		pQuadArray->AddRenderQuadInfo( vDrawPos,
									   vDrawSize,
									   pD3DTex,
									   vTexUV,
									   UI_RENDER_NORMAL,
									   TFO_BILINEAR,
									   m_dwTextAlpha );

		fArrangedX += (float)rkQuad.m_iWidth * m_fDrawScale;
		bFirst = false;
	}
}

int ioFontQuad::GetQuadCount() const
{
	return m_pPiece->m_vRenderQuad.size();
}

//---------------------------------------------------------------------

template<> ioFontManager* Singleton< ioFontManager >::ms_Singleton = 0;

ioFontManager::ioFontManager()
{
	m_DefaultErasePolicy = EP_NO_ERASE;

	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Font" );

	m_pUIRenderer = NULL;
	m_pWorkSpace = new ioFontWorkSpace;
	m_pCurFont = NULL;

	m_pFallbackCJKBuf  = NULL;
	m_dwFallbackCJKSize  = 0;
	m_pFallbackThaiBuf = NULL;
	m_dwFallbackThaiSize = 0;
	m_bFallbackLoaded  = false;

	m_AlignType = TAT_LEFT;
	m_VertAlignType = TVA_TOP;
	m_fScale	  = 1.0f;

	ZeroMemory( m_szBuffer, FONT_BUFFER_SIZE );
}

ioFontManager::~ioFontManager()
{
	SAFEDELETE( m_pWorkSpace );

	DestroyAll();

	ioFTManager::DestroyManager();

	if( m_pFallbackCJKBuf )
	{
		delete [] m_pFallbackCJKBuf;
		m_pFallbackCJKBuf = NULL;
	}

	if( m_pFallbackThaiBuf )
	{
		delete [] m_pFallbackThaiBuf;
		m_pFallbackThaiBuf = NULL;
	}
}

ioResource* ioFontManager::CreateImpl( const ioHashString &name, bool bThread )
{
	ioFont *pFont = new ioFont( name );
	if( LoadFile( name, pFont ) )
		return pFont;

	SAFEDELETE( pFont );
	return NULL;
}

void ioFontManager::ReLoadImpl( const ioHashString &name )
{
	ioFont *pFont = GetFontByName( name );
	if( pFont )
	{
		LoadFile( name, pFont );

		pFont->SetFontSize( pFont->GetFontSize() );
		pFont->SetWhiteSpaceSize( pFont->GetWhiteSpaceSize() );
	}
}

ioFont* ioFontManager::CreateFont( const ioHashString &name )
{
	return dynamic_cast< ioFont* >( Create( name ) );
}

ioFont* ioFontManager::GetFontByName( const ioHashString &name )
{
	return dynamic_cast< ioFont* >( GetByName( name ) );
}

bool ioFontManager::LoadFile( const ioHashString &name, ioFont *pFont )
{
	char szPath[MAX_PATH];
	wsprintf( szPath, "%s/%s", m_szStartDir, name.c_str() );

	ioFTFace *pFace = NULL;
	ioFTManager *pFTMgr = ioFTManager::GetManager();

	ioBinaryStream kStream;
	if( g_ResourceLoader.LoadStream( szPath, &kStream ) )
	{
		m_MemFile.AllocateMemory( kStream.GetSize(), kStream.GetPtr() );

	pFace = pFTMgr->OpenNewFaceFromMemory( name.c_str(),
										   m_MemFile.GetPtr(),
										   m_MemFile.GetMemFileSize(),
										   0 );

	if( pFace )
		pFace->SetCodePage( ioText::GetCodePage() );

		AddMem( name.c_str(), kStream.GetSize() );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioFontManager::LoadFile - %s Load Failed", szPath );
	}

	if( !pFace )
	{
		LOG.PrintTimeAndLog( 0, "ioFontManager::LoadFile - %s Cannot Open", szPath );
		return false;
	}

	pFont->SetFTFace( pFace );

	// cross-script fallback faces (Noto CJK, Noto Thai) for wide name rendering.
	// each font gets its own fallback face instances, size-matched via ioFont::SetFontSize.
	if( LoadFallbackBuffers() )
	{
		char szFBName[MAX_PATH];

		ioFTFace *pFallback1 = NULL;
		ioFTFace *pFallback2 = NULL;

		if( m_pFallbackCJKBuf )
		{
			wsprintf( szFBName, "%s_FB_CJK", name.c_str() );
			pFallback1 = pFTMgr->OpenNewFaceFromMemory( szFBName,
														m_pFallbackCJKBuf,
														m_dwFallbackCJKSize,
														0 );
		}

		if( m_pFallbackThaiBuf )
		{
			wsprintf( szFBName, "%s_FB_THAI", name.c_str() );
			pFallback2 = pFTMgr->OpenNewFaceFromMemory( szFBName,
														m_pFallbackThaiBuf,
														m_dwFallbackThaiSize,
														0 );
		}

		pFont->SetFallbackFaces( pFallback1, pFallback2 );
	}

	return true;
}

bool ioFontManager::LoadFallbackBuffers()
{
	if( m_bFallbackLoaded )
		return ( m_pFallbackCJKBuf != NULL || m_pFallbackThaiBuf != NULL );

	m_bFallbackLoaded = true;

	char szPath[MAX_PATH];

	ioBinaryStream kStream;
	wsprintf( szPath, "%s/%s", m_szStartDir, "lostsaga.ttf" );
	if( g_ResourceLoader.LoadStream( szPath, &kStream ) && kStream.GetSize() > 0 )
	{
		m_dwFallbackCJKSize = kStream.GetSize();
		m_pFallbackCJKBuf = new BYTE[ m_dwFallbackCJKSize ];
		CopyMemory( m_pFallbackCJKBuf, kStream.GetPtr(), m_dwFallbackCJKSize );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioFontManager::LoadFallbackBuffers - lostsaga.ttf Load Failed" );
	}

	ioBinaryStream kStream2;
	wsprintf( szPath, "%s/%s", m_szStartDir, "thailand.ttf" );
	if( g_ResourceLoader.LoadStream( szPath, &kStream2 ) && kStream2.GetSize() > 0 )
	{
		m_dwFallbackThaiSize = kStream2.GetSize();
		m_pFallbackThaiBuf = new BYTE[ m_dwFallbackThaiSize ];
		CopyMemory( m_pFallbackThaiBuf, kStream2.GetPtr(), m_dwFallbackThaiSize );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioFontManager::LoadFallbackBuffers - thailand.ttf Load Failed" );
	}

	return ( m_pFallbackCJKBuf != NULL || m_pFallbackThaiBuf != NULL );
}

ioFontManager& ioFontManager::GetSingleton()
{
	return Singleton< ioFontManager >::GetSingleton();
}

// ------------- 글자 출력을 위한 기능들 ----------------

void ioFontManager::Initialize( ioUIRenderer *pRenderer )
{
	m_pUIRenderer = pRenderer;

	ioFTManager *pFTMgr = ioFTManager::CreateManager();
	if( pFTMgr )
	{
		pFTMgr->InitLibrary();
	}
}

void ioFontManager::Flush()
{
	if( m_pWorkSpace )
	{
		m_pWorkSpace->Flush();
	}
}

void ioFontManager::Reset()
{
	SAFEDELETE( m_pWorkSpace );

	m_pWorkSpace = new ioFontWorkSpace;
}

bool ioFontManager::SetFont( const ioHashString &name )
{
	if( name.IsEmpty() )
		return false;

	m_pCurFont = GetFontByName( name );
	if( !m_pCurFont )
	{
		m_pCurFont = CreateFont( name );
	}

	if( m_pCurFont )
		return true;

	return false;
}

void ioFontManager::SetFontSize( int iSize, int iWhiteSpaceSize )
{
	if( m_pCurFont )
	{
		m_pCurFont->SetFontSize( iSize );
		m_pCurFont->SetWhiteSpaceSize( iWhiteSpaceSize );
	}
}

int ioFontManager::GetFontSize()
{
	if( m_pCurFont )
	{
		return max( 1, m_pCurFont->GetFontSize() );
	}
	return 1;
}

void ioFontManager::SetTextStyle( TextStyle tStyle )
{
	if( m_pWorkSpace )
	{
		m_pWorkSpace->SetTextStyle( tStyle );
	}
}

void ioFontManager::SetTextColor( int iRed, int iGreen, int iBlue )
{
	SetTextColor( D3DCOLOR_XRGB( iRed, iGreen, iBlue ) );
}

void ioFontManager::SetTextColor( DWORD dwColor )
{
	if( m_pWorkSpace )
	{
		m_pWorkSpace->SetTextColor( dwColor );
	}
}

void ioFontManager::SetBkColor( int iRed, int iGreen, int iBlue )
{
	SetBkColor( D3DCOLOR_XRGB( iRed, iGreen, iBlue ) );
}

void ioFontManager::SetBkColor( DWORD dwColor )
{
	if( m_pWorkSpace )
	{
		m_pWorkSpace->SetBkColor( dwColor );
	}
}

void ioFontManager::SetScale( float fScale )
{
	m_fScale = fScale;
	ioLocalManagerParent::GetChangedFontScale( m_fScale );
}

void ioFontManager::SetFontGap( int iGap )
{
	if( m_pWorkSpace )
	{
		m_pWorkSpace->SetFontGap( iGap );
	}
}

void ioFontManager::TextWidthCut( const char *szAdd, float fScale, float fWidth )
{
	if( strlen( szAdd ) > 0 )
	{
		fWidth -= GetTextWidth( szAdd, fScale );
	}
	
	int iLen = strlen( m_szBuffer );
	char szDst[FONT_BUFFER_SIZE];
	char szTemp[FONT_BUFFER_SIZE];
	memset( szDst, 0, FONT_BUFFER_SIZE );
	memset( szTemp, 0, FONT_BUFFER_SIZE );

	int iTemp = 0;
	int iBuf = 0;
	for(;iBuf < iLen;)
	{
		szTemp[iTemp++]   = m_szBuffer[iBuf++];

		if( ioText::IsLeadByte( (BYTE)szTemp[iTemp - 1] ) )

			szTemp[iTemp++] = m_szBuffer[iBuf++];

		if( GetTextWidth( szTemp, fScale ) >= fWidth )
			break;
		sprintf( szDst, "%s", szTemp );
	}
	
	if( strlen( szAdd ) > 0 )
		sprintf( m_szBuffer, "%s%s", szDst, szAdd );
	else
		sprintf( m_szBuffer, "%s", szDst );		
}

void ioFontManager::TextLeftCut( float fScale, float fWidth )
{
	int iLen = strlen( m_szBuffer );

	char szDst[FONT_BUFFER_SIZE];
	memset( szDst, 0, FONT_BUFFER_SIZE );

	int iBuf = 0;
	for(;iBuf < iLen;)
	{
		if( GetTextWidth( &m_szBuffer[iBuf], fScale ) < fWidth )
		{
			sprintf( szDst, "%s", &m_szBuffer[iBuf] );
			break;		
		}

		if( ioText::IsLeadByte( (BYTE)m_szBuffer[iBuf] ) )

			iBuf+=2;
		else
			iBuf+=1;
	}	
	sprintf( m_szBuffer, "%s", szDst );
}

void ioFontManager::SetText( float x, float y, float fScale, BYTE bAlpha )
{
	m_dwTextAlpha = (bAlpha << 24) | 0xffffff;

	SetScale( fScale );
	PrintByAlign( x, y );
}

void ioFontManager::SetTextWidthCut( float x, float y, float fScale, float fWidth, BYTE bAlpha )
{
	m_dwTextAlpha = (bAlpha << 24) | 0xffffff;

	if( GetTextWidth( m_szBuffer, fScale ) < fWidth )
	{
		SetScale( fScale );
		PrintByAlign( x, y );
	}
	else     
	{
		TextWidthCut( "...", fScale, fWidth );

		SetScale( fScale );
		PrintByAlign( x, y );
	}
}


void ioFontManager::SetTextLeftCut( float x, float y, float fScale, float fWidth, BYTE bAlpha )
{
	m_dwTextAlpha = (bAlpha << 24) | 0xffffff;

	if( GetTextWidth( m_szBuffer, fScale ) < fWidth )
	{
		SetScale( fScale );
		PrintByAlign( x, y );
	}
	else     
	{
		TextLeftCut( fScale, fWidth );
		SetScale( fScale );
		PrintByAlign( x, y );
	}
}

// PrintText
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, args );
		va_end( args );
		SetText( x, y, fScale, 255 );
	}
	else
	{
		SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
		SetText( x, y, fScale, 255 );
	}
}
#else
void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
	SetText( x, y, fScale, 255 );
}
#endif

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, DWORD dwArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, dwArg1 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, unsigned int uArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, uArg1 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, float fArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, int iArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, iArg2 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, const char *szArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, szArg2 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, fArg2 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1, int iArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1, iArg2 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, float fArg1, int iArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1, iArg2 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, float fArg1, float fArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1, fArg2 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1, const char *szArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1, szArg2 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, int iArg2, int iArg3 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, iArg2, iArg3 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2, float fArg3 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, fArg2, fArg3 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, const char *szArg2, const char *szArg3 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, szArg2, szArg3 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, float fArg1, float fArg2, float fArg3 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1, fArg2, fArg3 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1, int iArg2, int iArg3 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1, iArg2, iArg3 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, iArg2, iArg3, iArg4 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, fArg2, fArg3, fArg4 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1, fArg2, fArg3, fArg4 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4, float fArg5 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, fArg2, fArg3, fArg4, fArg5 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4, float fArg5 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1, fArg2, fArg3, fArg4, fArg5 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, int iArg4, int iArg5 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1, szArg2, iArg3, iArg4, iArg5 );
	SetText( x, y, fScale, 255 );
}

void ioFontManager::PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4, float fArg5, float fArg6 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, fArg2, fArg3, fArg4, fArg5, fArg6 );
	SetText( x, y, fScale, 255 );
}

// PrintText end

// PrintTextAlpha
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioFontManager::PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, args );
		va_end( args );
		SetText( x, y, fScale, bAlpha );
	}
	else
	{
		SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
		SetText( x, y, fScale, bAlpha );
	}
}
#else
void ioFontManager::PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
	SetText( x, y, fScale, bAlpha );
}
#endif

void ioFontManager::PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, int iArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1 );
	SetText( x, y, fScale, bAlpha );
}

void ioFontManager::PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, float fArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1 );
	SetText( x, y, fScale, bAlpha );
}

void ioFontManager::PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, const char *szArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1 );
	SetText( x, y, fScale, bAlpha );
}

void ioFontManager::PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, const char *szArg1, const char *szArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1, szArg2 );
	SetText( x, y, fScale, bAlpha );
}
// PrintTextAlpha end

// PrintTextWidthCut
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, args );
		va_end( args );
		SetTextWidthCut( x, y, fScale, fWidth, 255 );
	}
	else
	{
		SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
		SetTextWidthCut( x, y, fScale, fWidth, 255 );
	}
}
#else
void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}
#endif

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, float fArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, int iArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, iArg2 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, const char *szArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, szArg2 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1, int iArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1, iArg2 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1, const char *szArg2 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1, szArg2 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, int iArg2, int iArg3 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, iArg2, iArg3 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1, int iArg2, const char *szArg3 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1, iArg2, szArg3 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, iArg2, iArg3, iArg4 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, const char *szArg2, const char *szArg3, const char *szArg4, const char *szArg5 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1, szArg2, szArg3, szArg4, szArg5 );
	SetTextWidthCut( x, y, fScale, fWidth, 255 );
}
// PrintTextWidthCut end

// PrintTextWidthCutAlpha
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioFontManager::PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, args );
		va_end( args );

		SetTextWidthCut( x, y, fScale, fWidth, bAlpha );
	}
	else
	{
		SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
		SetTextWidthCut( x, y, fScale, fWidth, bAlpha );
	}
}
#else
void ioFontManager::PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
	SetTextWidthCut( x, y, fScale, fWidth, bAlpha );
}
#endif

void ioFontManager::PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource, int iArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1 );
	SetTextWidthCut( x, y, fScale, fWidth, bAlpha );
}

void ioFontManager::PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource, float fArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1 );
	SetTextWidthCut( x, y, fScale, fWidth, bAlpha );
}

void ioFontManager::PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource, const char *szArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1 );
	SetTextWidthCut( x, y, fScale, fWidth, bAlpha );
}
// PrintTextWidthCutAlpha end

// PrintTextLeftCut
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioFontManager::PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, args );
		va_end( args );

		SetTextLeftCut( x, y, fScale, fWidth, 255 );
	}
	else
	{
		SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
		SetTextLeftCut( x, y, fScale, fWidth, 255 );
	}
}
#else
void ioFontManager::PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource );
	SetTextLeftCut( x, y, fScale, fWidth, 255 );
}
#endif

void ioFontManager::PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, iArg1 );
	SetTextLeftCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource, float fArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, fArg1 );
	SetTextLeftCut( x, y, fScale, fWidth, 255 );
}

void ioFontManager::PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1 )
{
	SafeSprintf( m_szBuffer, FONT_BUFFER_SIZE, szSource, szArg1 );
	SetTextLeftCut( x, y, fScale, fWidth, 255 );
}
// PrintTextLeftCut end

void ioFontManager::PrintByAlign( float x, float y )
{
	if( !m_pCurFont || !m_pUIRenderer || !m_pWorkSpace )
		return;

	ioTextPiece *pPiece = m_pWorkSpace->GetTextPiece( m_pCurFont, m_szBuffer );
	if( !pPiece )	return;

	switch( m_AlignType )
	{
	case TAT_CENTER:
		x -= pPiece->m_iWidth * ( m_fScale * FLOAT05 );
		break;
	case TAT_RIGHT:
		x -= pPiece->m_iWidth * m_fScale;
		break;
	}

	switch ( m_VertAlignType )
	{
	case TVA_CENTER:
		y -= ( pPiece->m_iHeight * m_fScale * FLOAT05 );
		break;

	case TVA_BOTTOM:
		y -= ( pPiece->m_iHeight * m_fScale );
		break;
	}

	ioFontQuad *pQuad = new ioFontQuad;
	if( pQuad )
	{
		pQuad->m_iDrawX = x;
		pQuad->m_iDrawY = y;
		pQuad->m_fDrawScale = m_fScale;
		pQuad->m_dwTextAlpha = m_dwTextAlpha;
		pQuad->m_pPiece = pPiece;

		m_pUIRenderer->AddUIQuad( pQuad );
	}
}

void ioFontManager::PrintByAlignWide( float x, float y, const wchar_t *szText )
{
	if( !m_pCurFont || !m_pUIRenderer || !m_pWorkSpace )
		return;

	ioTextPiece *pPiece = m_pWorkSpace->GetTextPieceWide( m_pCurFont, szText );
	if( !pPiece )	return;

	switch( m_AlignType )
	{
	case TAT_CENTER:
		x -= pPiece->m_iWidth * ( m_fScale * FLOAT05 );
		break;
	case TAT_RIGHT:
		x -= pPiece->m_iWidth * m_fScale;
		break;
	}

	switch ( m_VertAlignType )
	{
	case TVA_CENTER:
		y -= ( pPiece->m_iHeight * m_fScale * FLOAT05 );
		break;

	case TVA_BOTTOM:
		y -= ( pPiece->m_iHeight * m_fScale );
		break;
	}

	ioFontQuad *pQuad = new ioFontQuad;
	if( pQuad )
	{
		pQuad->m_iDrawX = x;
		pQuad->m_iDrawY = y;
		pQuad->m_fDrawScale = m_fScale;
		pQuad->m_dwTextAlpha = m_dwTextAlpha;
		pQuad->m_pPiece = pPiece;

		m_pUIRenderer->AddUIQuad( pQuad );
	}
}

void ioFontManager::PrintTextWide( float x, float y, float fScale, const wchar_t *szText )
{
	SetTextWide( x, y, fScale, 255, szText );
}

void ioFontManager::SetTextWide( float x, float y, float fScale, BYTE bAlpha, const wchar_t *szText )
{
	m_dwTextAlpha = (bAlpha << 24) | 0xffffff;

	SetScale( fScale );
	PrintByAlignWide( x, y, szText );
}

float ioFontManager::GetTextWidthWide( const wchar_t *szText, float fScale )
{
	if( m_pWorkSpace && m_pCurFont )
	{
		int iWidth = m_pWorkSpace->CalculateTextWidthWide( szText, m_pCurFont );
		return (float)iWidth * fScale;
	}

	return 0.0f;
}

float ioFontManager::GetTextWidthWide( const wchar_t *szText, TextStyle eStyle, float fScale, int iFontGap )
{
	if( m_pWorkSpace && m_pCurFont )
	{
		int iWidth = m_pWorkSpace->CalculateTextWidthWide( szText, m_pCurFont, eStyle, iFontGap );
		return (float)iWidth * fScale;
	}

	return 0.0f;
}

TextStyle ioFontManager::GetTextStyle()
{
	if( m_pWorkSpace )
		return m_pWorkSpace->GetTextStyle();

	return TS_NORMAL;
}

void ioFontManager::CutWideTextForWidth( const wchar_t *szText, TextStyle eStyle, float fScale, float fWidth, wchar_t *szOut, int iOutSize )
{
	if( !szOut || iOutSize <= 0 )
		return;

	szOut[0] = L'\0';

	if( !szText )
		return;

	const wchar_t *szDot = L"...";
	float fDotWidth = GetTextWidthWide( szDot, eStyle, fScale );

	float fCutWidth = fWidth - fDotWidth;
	if( fCutWidth < 0.0f )
		fCutWidth = 0.0f;

	wchar_t szTemp[FONT_BUFFER_SIZE];
	int iTemp = 0;
	int iFitLen = 0;

	for( int i=0 ; szText[i] != L'\0' && iTemp < FONT_BUFFER_SIZE-1 ; i++ )
	{
		szTemp[iTemp++] = szText[i];
		szTemp[iTemp] = L'\0';

		if( GetTextWidthWide( szTemp, eStyle, fScale ) >= fCutWidth )
			break;

		iFitLen = iTemp;
	}

	int iOut = 0;
	for( int i=0 ; i<iFitLen && iOut < iOutSize-1 ; i++ )
		szOut[iOut++] = szText[i];

	for( int i=0 ; szDot[i] != L'\0' && iOut < iOutSize-1 ; i++ )
		szOut[iOut++] = szDot[i];

	szOut[iOut] = L'\0';
}

void ioFontManager::PrintTextWidthCutWide( float x, float y, float fScale, float fWidth, const wchar_t *szText )
{
	if( !szText )
		return;

	if( GetTextWidthWide( szText, fScale ) < fWidth )
	{
		PrintTextWide( x, y, fScale, szText );
		return;
	}

	wchar_t szCut[FONT_BUFFER_SIZE];
	CutWideTextForWidth( szText, GetTextStyle(), fScale, fWidth, szCut, FONT_BUFFER_SIZE );

	PrintTextWide( x, y, fScale, szCut );
}

float ioFontManager::GetTextWidthCutSizeWide( const wchar_t *szText, TextStyle eStyle, float fScale, float fWidth )
{
	float fCutSize = GetTextWidthWide( szText, eStyle, fScale );
	if( fCutSize < fWidth )
		return fCutSize;

	wchar_t szCut[FONT_BUFFER_SIZE];
	CutWideTextForWidth( szText, eStyle, fScale, fWidth, szCut, FONT_BUFFER_SIZE );

	return GetTextWidthWide( szCut, eStyle, fScale );
}

float ioFontManager::GetTextWidth( const char *szText,
								   TextStyle eStyle,
								   float fScale,
								   int iFontGap )
{
	if( m_pWorkSpace && m_pCurFont )
	{
		int iWidth = m_pWorkSpace->CalculateTextWidth( szText, m_pCurFont, eStyle, iFontGap );
		return (float)iWidth * fScale;
	}

	return 0.0f;
}

float ioFontManager::GetTextWidth( const char *szText, float fScale )
{
	if( m_pWorkSpace && m_pCurFont )
	{
		int iWidth = m_pWorkSpace->CalculateTextWidth( szText, m_pCurFont );
		return (float)iWidth * fScale;
	}

	return 0.0f;
}

float ioFontManager::GetRecentlyPrintTextWidth( float fScale )
{
	return GetTextWidth( m_szBuffer, fScale );
}

float ioFontManager::GetTextWidthCutSize( const char *szText, TextStyle eStyle, float fScale, float fWidth )
{
	float fCutSize = GetTextWidth( szText, eStyle, fScale );
	if( fCutSize < fWidth )
		return fCutSize;
	
	sprintf( m_szBuffer, "%s", szText );
	TextWidthCut( "..", fScale, fWidth );
	return GetTextWidth( m_szBuffer, eStyle, fScale );
}

float ioFontManager::GetTextHeight( const char *szText, TextStyle eStyle, float fScale, int iFontGap )
{
	ioTextPiece *pPiece = m_pWorkSpace->GetTextPiece( m_pCurFont, szText );
	if(pPiece)
	{
		return pPiece->m_iHeight * fScale + iFontGap ;
	}
	return 0.0f;
}

void ioFontManager::RenderTextTexture( int iXPos, int iYPos, float fScale )
{
	if( m_pWorkSpace )
	{
		m_pWorkSpace->RenderWorkTexture( iXPos, iYPos, fScale );
	}
}
