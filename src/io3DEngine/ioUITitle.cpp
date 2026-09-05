

#include "stdafx.h"

#include "ioFontManager.h"

#include "ioUITitle.h"
#include "ioStringConverter.h"

ioUITitle::ioUITitle()
{
	m_iXOffset = 0;
	m_iYOffset = 0;

	m_iSize = 24;
	m_iVertGap = 24;
	m_iFontGap = 0;
	m_fWidthCutSize = 0.0f;
	m_iAlphaRate = MAX_ALPHA_RATE;

	m_TextStyle = m_DisableTextStyle = TS_NORMAL;
	m_TextHorzAlign = TAT_LEFT;
	m_TextVertAlign = TVA_TOP;
	m_PrintTextStyle = PTS_NORMAL;

	m_bVisible = true;
}

ioUITitle::~ioUITitle()
{
}

void ioUITitle::SetStyle( const ioHashString &szStyle )
{
	TextStyle eStyle = ioStringConverter::ParseTextStyle( szStyle );
	SetStyle( eStyle );
}

void ioUITitle::SetDisableStyle( const ioHashString &szStyle )
{
	TextStyle eStyle = ioStringConverter::ParseTextStyle( szStyle );
	SetDisableStyle( eStyle );
}

void ioUITitle::SetHorzAlign( const ioHashString &szAlign )
{
	TextAlignType eAlign = ioStringConverter::ParseTextAlignType( szAlign );
	SetHorzAlign( eAlign );
}

void ioUITitle::SetVertAlign( const ioHashString &szAlign )
{
	TextVertAlign eAlign = ioStringConverter::ParseTextVertAlign( szAlign );
	SetVertAlign( eAlign );
}

void ioUITitle::SetWidthCutSize( const ioHashString &szAlign )
{
	SetWidthCutSize( ioStringConverter::ParseFloat( szAlign.c_str() ) );
}

void ioUITitle::SetColor( const ioHashString &szType,
						  DWORD dwTextColor,
						  DWORD dwBkColor )
{
	if( szType == "Normal" )
	{
		m_Normal.dwText = dwTextColor;
		m_Normal.dwBk = dwBkColor;
	}
	else if( szType == "Over" )
	{
		m_Over.dwText = dwTextColor;
		m_Over.dwBk = dwBkColor;
	}
	else if( szType == "Push" )
	{
		m_Push.dwText = dwTextColor;
		m_Push.dwBk = dwBkColor;
	}
	else if( szType == "Disable" )
	{
		m_Disable.dwText = dwTextColor;
		m_Disable.dwBk = dwBkColor;
	}
	else if( szType == "NoneSelected" )
	{
		m_NoneSelected.dwText = dwTextColor;
		m_NoneSelected.dwBk   = dwBkColor;
	}
}

void ioUITitle::SetColor( const ioHashString &szType,
						  const ioHashString &szText,
						  const ioHashString &szBk )
{
	DWORD dwTextColor, dwBkColor;
	dwTextColor = ioStringConverter::ParseColor( szText );
	dwBkColor   = ioStringConverter::ParseColor( szBk );
	SetColor( szType, dwTextColor, dwBkColor );
}

void ioUITitle::SetAllColor( DWORD dwTextColor, DWORD dwBkColor )
{
	SetColor( "Normal", dwTextColor, dwBkColor );
	SetColor( "Over", dwTextColor, dwBkColor );
	SetColor( "Push", dwTextColor, dwBkColor );
	SetColor( "Disable", dwTextColor, dwBkColor );
	SetColor( "NoneSelected", dwTextColor, dwBkColor );
}

void ioUITitle::SetFontGap( int iGap )
{
	m_iFontGap = iGap;
}

void ioUITitle::SetText( const char *szText, const char *szDelims )
{
	m_vTextList.clear();

	if( strcmp( szDelims, "" ) )
	{
		const StringVector &vTextList = ioStringConverter::Split( szText, szDelims );
		
		StringVector::const_iterator iter;
		for( iter=vTextList.begin() ; iter!=vTextList.end() ; ++iter )
		{
			m_vTextList.push_back( ioHashString( iter->c_str() ) );
		}
	}
	else	// delims 없음
	{
		m_vTextList.push_back( ioHashString(szText) );
	}
}

void ioUITitle::PrintNormal( int iXPos, int iYPos )
{
	Print( iXPos, iYPos, m_Normal.dwText, m_Normal.dwBk, m_TextStyle );
}

void ioUITitle::PrintOver( int iXPos, int iYPos )
{
	Print( iXPos, iYPos, m_Over.dwText, m_Over.dwBk, m_TextStyle );
}

void ioUITitle::PrintPush( int iXPos, int iYPos )
{
	Print( iXPos, iYPos, m_Push.dwText, m_Push.dwBk, m_TextStyle );
}

void ioUITitle::PrintDisable( int iXPos, int iYPos )
{
	Print( iXPos, iYPos, m_Disable.dwText, m_Disable.dwBk, m_DisableTextStyle );
}

void ioUITitle::PrintNoneSelected( int iXPos, int iYPos )
{
	Print( iXPos, iYPos, m_NoneSelected.dwText, m_NoneSelected.dwBk, m_TextStyle );
}

void ioUITitle::Print( int iXPos, int iYPos, DWORD dwTextColor, DWORD dwBkColor, TextStyle eTextStyle )
{
	if( !m_bVisible )
		return;

	if( m_vTextList.empty() )	return;

	iXPos += m_iXOffset;
	iYPos += m_iYOffset;

	TextVertAlign PrevVertAlign = g_FontMgr.GetVertAlign();

	g_FontMgr.SetTextStyle( eTextStyle );
	g_FontMgr.SetAlignType( m_TextHorzAlign );
	g_FontMgr.SetVertAlign( m_TextVertAlign );
	g_FontMgr.SetTextColor( dwTextColor );
	g_FontMgr.SetBkColor( dwBkColor );
	g_FontMgr.SetFontGap( m_iFontGap );

	int iStartY = iYPos;
	int iTextCount = m_vTextList.size();

	switch( m_TextVertAlign )
	{
	case TVA_TOP:
		break;
	case TVA_CENTER:
		iStartY -= ( m_iVertGap * iTextCount ) / 2;
		break;
	case TVA_BOTTOM:
		iStartY -= m_iVertGap * iTextCount;
		break;
	}

	float fScale = (float)m_iSize / (float)g_FontMgr.GetFontSize();

	for( int i=0 ; i<iTextCount ; i++ )
	{
		if( m_vTextList[i].IsEmpty() )
			continue;

		if( m_PrintTextStyle == PTS_NORMAL )
		{
			if( m_iAlphaRate == MAX_ALPHA_RATE )
				g_FontMgr.PrintText( iXPos, iStartY + m_iVertGap * i, fScale, m_vTextList[i].c_str() );
			else
				g_FontMgr.PrintTextAlpha( iXPos, iStartY + m_iVertGap * i, fScale, m_iAlphaRate, m_vTextList[i].c_str() );
		}
		else if( m_PrintTextStyle == PTS_WIDTHCUT )
		{
			if( m_iAlphaRate == MAX_ALPHA_RATE )
				g_FontMgr.PrintTextWidthCut( iXPos, iStartY + m_iVertGap * i, fScale, m_fWidthCutSize, m_vTextList[i].c_str() );
			else
				g_FontMgr.PrintTextWidthCutAlpha( iXPos, iStartY + m_iVertGap * i, fScale, m_fWidthCutSize, m_iAlphaRate, m_vTextList[i].c_str() );
		}
	}

	g_FontMgr.SetFontGap( 0 );
	g_FontMgr.SetVertAlign( PrevVertAlign );
}

void ioUITitle::GetFirstText( OUT ioHashString &rszText )
{
	if( m_vTextList.empty() ) return;
	if( m_vTextList[0].IsEmpty() ) return;

	rszText = m_vTextList[0];
}

// 윈도우 기반에서 정렬을 위해 현재 정렬상태에 따른 렌더위치를 알려주는 함수
void	ioUITitle::GetRenderPosForWndEX( int x, int y, int width, int height, int& outx, int& outy )
{
	switch ( GetVertAlign() )
	{
	case TVA_TOP:
		outy = y;		
		break;

	case TVA_CENTER:
		outy = y + height * FLOAT05;
		break;

	case TVA_BOTTOM:
		outy = y + height;
		break;

	default:
		outy = y;
		break;
	}

	switch ( GetHorzAlign() )
	{
	case TAT_LEFT:
		outx = x;		
		break;

	case TAT_CENTER:
		outx = x + width * FLOAT05;
		break;

	case TAT_RIGHT:
		outx = x + width;
		break;

	default:
		outx = x;
		break;
	}
}

IoUIText::IoUIText()
{
	m_iRenderPos = URT_NORMAL_RENDER;
}

IoUIText::~IoUIText()
{
}

void IoUIText::SetName( const char *name )
{
	m_szName = name;
}

void IoUIText::SetParserRenderPos( const ioHashString &szRenderPos )
{
	m_iRenderPos = URT_NORMAL_RENDER;

    if (szRenderPos == "PreRender")
		m_iRenderPos = URT_PRE_RENDER;
    else if (szRenderPos == "NormalRender")
        m_iRenderPos = URT_NORMAL_RENDER;
    else if (szRenderPos == "AfterRender")
        m_iRenderPos = URT_AFTER_RENDER;
}