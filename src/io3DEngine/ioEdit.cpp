

#include "stdafx.h"

#include "ioWndType.h"
#include "ioWnd.h"
#include "ioEdit.h"

#include "InputBox.h"
#include "ioFontManager.h"

#include "ioXMLDocument.h"
#include "ioXMLElement.h"

#include "ioStringConverter.h"

ioEdit* ioEdit::m_pKeyFocusEdit	= NULL;

DWORD ioEdit::m_dwPreCaretCtrlTime = 0;
DWORD ioEdit::m_dwCaretTwinkleGap  = 500;
bool  ioEdit::m_bNowCaretShow = true;

ioEdit::ioEdit()
{
	m_iMaxLen	= MAX_EDIT_TEXT;
	m_iMaxLine  = 1;
	m_iLeftCutPos = 0;
	m_iPrevLeftCut = m_iMaxLen;
	memset( m_szText, 0, MAX_EDIT_TEXT );
	m_szMultiText.clear();

	m_dwTextColor = D3DCOLOR_XRGB( 255, 255, 255 );
	m_dwBkColor	  = D3DCOLOR_XRGB( 0, 0, 0 );
	m_TextStyle	= TS_NORMAL;
	m_AlignType = TAT_LEFT;
	m_fScale	= 0.8f;

	m_bKeyFocus = false;
	m_bReadOnly = false;

	m_iCaretXPos = 0;

	m_iSpaceBetweenLine = 20;	
	m_iCurCaretLine = 0;

	m_iCurPage      = 0;
	m_iPrevCaretPos = 0;

	m_iEndLineMoveAmount = 0;
}

ioEdit::~ioEdit()
{
}

void ioEdit::CheckCaretTwinkle( DWORD dwCurTime )
{
	if( m_dwPreCaretCtrlTime > 0 )
	{
		if( m_dwPreCaretCtrlTime + m_dwCaretTwinkleGap < dwCurTime )
		{
			m_bNowCaretShow = !m_bNowCaretShow;
			m_dwPreCaretCtrlTime = dwCurTime;
		}
	}
	else
	{
		m_dwPreCaretCtrlTime = dwCurTime;
		m_bNowCaretShow = true;
	}
}

void ioEdit::ForceCaretTwinkle()
{
	m_dwPreCaretCtrlTime = 0;
	m_bNowCaretShow = true;
}

void ioEdit::SetCaretTwinkleGap( DWORD dwGap )
{
	m_dwCaretTwinkleGap = dwGap;
}

bool ioEdit::IsNowCaretShow()
{
	return m_bNowCaretShow;
}

void ioEdit::SetMaxLength( int iLen )
{
	m_iMaxLen = max( 2, min( iLen, MAX_EDIT_TEXT-2 ) );
}

void ioEdit::SetReadOnly()
{
	if( m_pKeyFocusEdit == this )
	{
		char szTemp[MAX_EDIT_TEXT];
		strcpy( szTemp, m_szText );

		KillKeyFocus();

		strcpy( m_szText, szTemp );
	}

	m_bReadOnly = true;
}

void ioEdit::SetText( const char *szText )
{
	memset( m_szText, 0, MAX_EDIT_TEXT );

	if( m_dwExStyle & IWS_EX_EDIT_NUM )
	{
		int i, j;
		i = j = 0;

		while( szText[i] != '\0' )
		{
			if( szText[i] >= '0' && szText[i] <= '9' )
			{
				m_szText[j] = szText[i];
				++j;

				if( j >= m_iMaxLen )
					break;
			}
			++i;
		}
		g_InputBox.SetString( m_szText );
	}
	else if( m_dwExStyle & IWS_EX_NO_REND_EDIT_NUM )
	{
		int i, j;
		i = j = 0;

		while( szText[i] != '\0' )
		{
			if( szText[i] >= '0' && szText[i] <= '9' )
			{
				if( i != 0 || szText[i] != '0' )
				{
					m_szText[j] = szText[i];
					++j;

					if( j >= m_iMaxLen )
						break;
				}
			}
			++i;
		}
		g_InputBox.SetString( m_szText );
	}
	else
	{
		strncpy( m_szText, szText, MAX_EDIT_TEXT-1 );
	}

	SetTextSplit();
}

const char* ioEdit::GetText() const
{	
	return m_szMultiText.c_str();
}

void ioEdit::SetTextColor( DWORD dwTextColor, DWORD dwTextBkColor )
{
	m_dwTextColor = dwTextColor;
	m_dwBkColor   = dwTextBkColor;
}

void ioEdit::ClearString()
{
	memset( m_szText, 0, MAX_EDIT_TEXT );
	m_iLeftCutPos  = 0;
	m_iPrevLeftCut = m_iMaxLen;
	m_vSplitText.clear();
	m_szMultiText.clear();
	m_vMultiText.clear();
}

void ioEdit::SetKeyFocus()
{
	if( m_bReadOnly )	return;
	if( m_pKeyFocusEdit == this )	return;

	if( m_pKeyFocusEdit )
		m_pKeyFocusEdit->KillKeyFocus();

	g_InputBox.SetLimitText( m_iMaxLen );
	if( m_dwExStyle & IWS_EX_MULTI_EDIT )
	{
		SetCaretViewPage();
	}
	else
	{
		g_InputBox.Clear();
		g_InputBox.SetString( m_szText );
	}

	if( m_dwExStyle & IWS_EX_EDIT_PW )
		g_InputBox.SetEnglishInput();
	else
		g_InputBox.SetNativeInput();

	m_bKeyFocus = true;
	m_pKeyFocusEdit = this;

	ForceCaretTwinkle();

	m_iCaretXPos    = GetDerivedPosX();
}

void ioEdit::KillKeyFocus()
{
	if( m_pKeyFocusEdit != this )	return;

	SetText( g_InputBox.GetString() );

	g_InputBox.Clear();

	m_bKeyFocus = false;
	m_pKeyFocusEdit = NULL;
}

void ioEdit::OnProcess( float fTimePerSec )
{
	if( !m_bKeyFocus )	return;

	if( m_dwExStyle & IWS_EX_MULTI_EDIT )
	{
		if( g_InputBox.IsEnter() )
		{
			MultiLineEnter();
		}
		else if( g_InputBox.IsTab() )
		{
			KillKeyFocus();

			if( m_pParent )
				m_pParent->iwm_command( this, IOED_INPUT, true );
		}
		else if( g_InputBox.IsBackSpace() )
		{
			MultiLineBackSpace();
		}
		else if( g_InputBox.IsDel() )
		{
			MultiLineDel();
		}
		else
		{
			MultiLineProcess();
		}
	}
	else
	{
		SetText( g_InputBox.GetString() );

		if( m_pParent && m_pParent->IsShow() )
		{
			if( g_InputBox.IsEnter() )
			{
				KillKeyFocus();
				m_pParent->iwm_command( this, IOED_INPUT, true );
			}
			else if( g_InputBox.IsTab() )
			{
				KillKeyFocus();
				m_pParent->iwm_command( this, IOED_INPUT, false );
			}
		}
	}
}

void ioEdit::OnRender()
{
	if( m_dwExStyle & IWS_EX_NO_REND_EDIT_NUM )
		return;

	int xPos, yPos;
	xPos = GetDerivedPosX();
	yPos = GetDerivedPosY();

	g_FontMgr.SetFont( m_FontName );
	g_FontMgr.SetAlignType( m_AlignType );
	g_FontMgr.SetTextStyle( m_TextStyle );
	g_FontMgr.SetTextColor( m_dwTextColor );
	g_FontMgr.SetBkColor( m_dwBkColor );

	switch( m_AlignType )
	{
	case TAT_CENTER:
		xPos += ( ( m_rcPos.right - m_rcPos.left ) / 2 );
		break;
	case TAT_RIGHT:
		xPos += ( m_rcPos.right - m_rcPos.left ) - 8;
		break;
	}

	if( m_dwExStyle & IWS_EX_EDIT_PW )
	{
		char szBuffer[MAX_PATH];

		int iLength = strlen(m_szText);
		for( int i=0; i<iLength ; i++ )
		{
			szBuffer[i] = '*';
		}
		szBuffer[i] = '\0';

		g_FontMgr.PrintText( xPos, yPos, m_fScale, szBuffer );
	}
	else
	{
		RenderMultiLine();
	}
}

void ioEdit::RenderMultiLine()
{
	int xPos, yPos;
	xPos = GetDerivedPosX();
	yPos = GetDerivedPosY();

	float fWidth = ioWnd::GetWidth();
	switch( m_AlignType )
	{
	case TAT_CENTER:
		xPos += ( fWidth / 2 );
		break;
	case TAT_RIGHT:
		xPos += ( fWidth - 8 );
		break;
	}

	int iFocusWidth = 0;
	int iFocusXPos  = 0;
	int iFocusYPos  = 0;
	int iCaretPos   = 0;
	
	bool bCaretVisible = false;
	if( m_dwExStyle & IWS_EX_MULTI_EDIT )
	{
		int iSize = (int)m_vMultiText.size();
		int iOffset = 0;
		for( int i = m_iCurPage; i < m_iCurPage + m_iMaxLine; ++i, ++iOffset )
		{
			if( i < iSize )
			{
				std::string kString = m_vMultiText[i];
				if( !kString.empty() )
				{
					g_FontMgr.PrintText( xPos, yPos + iOffset * m_iSpaceBetweenLine, m_fScale, kString.c_str() );
				}

				if( i == m_iCurCaretLine )
				{
					iCaretPos = g_InputBox.GetCaretPos();
					char szBuffer[MAX_EDIT_TEXT] = "";
					memset( szBuffer, 0, MAX_EDIT_TEXT );
					memcpy( szBuffer, kString.c_str(), iCaretPos );
					iFocusXPos  = (int)g_FontMgr.GetTextWidth( szBuffer, m_TextStyle, m_fScale );
					iFocusWidth = (int)g_FontMgr.GetTextWidth( kString.c_str(), m_TextStyle, m_fScale );
					iFocusYPos  = iOffset * m_iSpaceBetweenLine;
					bCaretVisible = true;
				}
			}
		}
	}
	else
	{
		iCaretPos = g_InputBox.GetCaretPos() - m_iLeftCutPos;

		int iPrevLen  = 0;
		for ( int i = 0; i < (int)m_vSplitText.size(); i++ )
		{
			ioHashString &rkSplitText = m_vSplitText[i];

			if( rkSplitText.IsEmpty() )
				continue;

			g_FontMgr.PrintText( xPos, yPos + i * m_iSpaceBetweenLine, m_fScale, rkSplitText.c_str() );		

			// 캐럿 위치
			if( m_bKeyFocus && IsNowCaretShow() )
			{
				if( COMPARE( iCaretPos, iPrevLen, iPrevLen + rkSplitText.Length() + 1 ) )
				{
					char szBuffer[MAX_EDIT_TEXT] = "";
					memset( szBuffer, 0, MAX_EDIT_TEXT );
					memcpy( szBuffer, rkSplitText.c_str(), min( max( 0, iCaretPos - iPrevLen ), rkSplitText.Length() ) );
					iFocusXPos  = (int)g_FontMgr.GetTextWidth( szBuffer, m_TextStyle, m_fScale );
					iFocusWidth = (int)g_FontMgr.GetTextWidth( rkSplitText.c_str(), m_TextStyle, m_fScale );
					iFocusYPos  = i * m_iSpaceBetweenLine;
				}
				iPrevLen += rkSplitText.Length();
			}
		}
	}
	
	if( m_bKeyFocus && IsNowCaretShow() )
	{
		switch( m_AlignType )
		{
		case TAT_LEFT:
			xPos += iFocusXPos;
			break;
		case TAT_CENTER:
			xPos -= iFocusWidth / 2;
			xPos += iFocusXPos;
			break;
		case TAT_RIGHT:
			xPos -= ( iFocusWidth - iFocusXPos );
			break;
		}

		if( bCaretVisible || !(m_dwExStyle & IWS_EX_MULTI_EDIT) )
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.PrintText( xPos, yPos + iFocusYPos, m_fScale, "_" );
			g_FontMgr.SetAlignType( m_AlignType );
		}
		
		m_iCaretXPos = xPos;
	}
}

void ioEdit::iwm_create()
{
	m_dwStyle |= IWS_PROCESS;
}

void ioEdit::iwm_lbuttondown( const ioMouse& mouse )
{
	if( m_bReadOnly )	return;

	if( !m_bKeyFocus )
		SetKeyFocus();

	if( m_pParent )
		m_pParent->iwm_command( this, IOBN_BTNDOWN, 0 );

	ioWnd::iwm_lbuttondown( mouse );
}


DWORD ioEdit::GetWndType() const
{
	return IWT_EDIT;
}

void ioEdit::ParseExtraInfo( ioXMLElement &xElement )
{
	int iMaxLine = xElement.GetIntAttribute( "MaxLine" );
	if( iMaxLine > 0 )
	{
		m_iMaxLine = iMaxLine;
	}

	int iMaxLength = xElement.GetIntAttribute( "MaxLength" );
	if( iMaxLength > 0 )
	{
		SetMaxLength( iMaxLength );
	}
	
	m_iSpaceBetweenLine = xElement.GetIntAttribute( "SpaceBetweenLine" );
	if( m_iSpaceBetweenLine == 0 )
	{
		m_iSpaceBetweenLine = 20;
	}

	DWORD dwTextColor, dwBkColor;
	dwTextColor = ioStringConverter::ParseColor( xElement.GetStringAttribute( "TextColor" ) );
	dwBkColor = ioStringConverter::ParseColor( xElement.GetStringAttribute( "BkColor" ) );
	SetTextColor( dwTextColor, dwBkColor );

	TextAlignType eAlign = ioStringConverter::ParseTextAlignType( xElement.GetStringAttribute( "Align" ) );
	SetTextAlignType( eAlign );

	TextStyle eStyle = ioStringConverter::ParseTextStyle( xElement.GetStringAttribute( "Style" ) );
	SetTextStyle( eStyle );

	int iFontSize = xElement.GetIntAttribute( "Size" );
	if( iFontSize > 0 )
	{
		SetFontScale( (float)iFontSize / (float)g_FontMgr.GetFontSize() );
	}

	m_iEndLineMoveAmount = xElement.GetIntAttribute( "EndLineMoveAmount" );
	if( m_iEndLineMoveAmount == 0 )
		m_iEndLineMoveAmount = 1;
}

void ioEdit::SetTextSplit()
{
	m_vSplitText.clear();
	m_szMultiText.clear();
	m_iLeftCutPos  = 0;

	float fWidth = ioWnd::GetWidth();	

	char szLeftCutText[MAX_EDIT_TEXT*2] = "";
	memset( szLeftCutText, 0, sizeof( szLeftCutText ) );

	char szRightCutText[MAX_EDIT_TEXT*2] = "";
	memset( szRightCutText, 0, sizeof( szRightCutText ) );

	char szFirstCutText[MAX_EDIT_TEXT*2] = "";
	memset( szFirstCutText, 0, sizeof( szFirstCutText ) );

	char szPrintText[MAX_EDIT_TEXT*2] = "";
	memset( szPrintText, 0, sizeof( szPrintText ) );
	GetTextLeftCut( m_szText, szFirstCutText, szLeftCutText, fWidth * m_iMaxLine );
	GetTextRightCut( szFirstCutText, szPrintText, szRightCutText, fWidth * m_iMaxLine );
	
	int i = 0;
	int iCurLen = 0;
	int iTextLen = strlen( szPrintText );
	const char *pSrc = &szPrintText[0];
	for(i = 0;i < m_iMaxLine - 1;i++)
	{
		if( iCurLen >= iTextLen )
			break;

		int iLineLen = 0;
		char szSplitText[MAX_PATH];
		memset( szSplitText, 0, sizeof( szSplitText ) );
		while(true)
		{
			if( iLineLen >= MAX_PATH )
				break;

			szSplitText[iLineLen] = *pSrc;

			float fTextW = g_FontMgr.GetTextWidth( szSplitText, m_TextStyle, m_fScale );
			if( fTextW < fWidth )
			{
				pSrc++;
				iLineLen++;
				iCurLen++;
			}
			else
			{
				szSplitText[iLineLen] = NULL;
				if( CheckHanByte( szSplitText ) )          //한글을 자르면 글자가 깨지므로...
				{
					szSplitText[iLineLen - 1] = NULL;
					pSrc--;
					iCurLen--;
				}
				break;    //다음 라인으로 넘김.
			}

			if(iCurLen >= iTextLen)
				break;                     //입력된 텍스트를 모두 검사했다.
		}

		if( strlen( szSplitText ) > 0 )
			m_vSplitText.push_back( szSplitText );
	}

	//마지막 라인은 전부 표시
	if( strlen( pSrc ) >= 0 )
		m_vSplitText.push_back( pSrc );
	

	m_szMultiText += szLeftCutText;
	for (i = 0; i < (int)m_vSplitText.size() ; i++)
	{
		ioHashString &rkSplitText = m_vSplitText[i];
		if( rkSplitText.IsEmpty() ) continue;

		m_szMultiText += rkSplitText.c_str();
	}
	m_szMultiText += szRightCutText;
}

bool ioEdit::CheckHanByte( char *szText ) const
{
	int len = strlen(szText);
	int hbyte_count = 0;
	for(int i = 0; i < len; i++ )
	{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szText[i] ) )
#else
		if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szText[i] ) )
#endif

#else
		if( IsDBCSLeadByte( (BYTE)szText[i] ) )
#endif

		{
			i++;
			if(len <= i) // 마지막 한글이 깨진 글자다.
				return true;
		}
	}
	
	return false;
}

bool ioEdit::CheckConstHanByte( const char *szText ) const
{
	int len = strlen(szText);
	int hbyte_count = 0;
	for(int i = 0; i < len; i++ )
	{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szText[i] ) )
#else
		if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szText[i] ) )
#endif

#else
		if( IsDBCSLeadByte( (BYTE)szText[i] ) )
#endif

		{
			i++;
			if(len <= i) // 마지막 한글이 깨진 글자다.
				return true;
		}
	}

	return false;
}


void ioEdit::GetTextLeftCut( char *pSrc, char *pDst, char *pCut, float fWidth )
{
	if( pSrc == NULL || pDst == NULL || pCut == NULL ) 
		return;

	if( g_FontMgr.GetTextWidth( pSrc, m_TextStyle, m_fScale ) < fWidth )
	{
		m_iPrevLeftCut = m_iMaxLen;
		m_iLeftCutPos  = 0;
		sprintf( pDst, "%s", pSrc );
		return;
	}
	
	m_iPrevLeftCut = min( m_iPrevLeftCut, g_InputBox.GetCaretPos() );                   // 캐럿 위치 이하로 자르지는 않는다.
	int iLen = strlen( pSrc );
	int iSrc,iCut;
	for(iSrc = 0, iCut = 0;iSrc < iLen;)
	{
		if( iSrc >= m_iPrevLeftCut || g_FontMgr.GetTextWidth( &pSrc[iSrc], m_TextStyle, m_fScale ) < fWidth )
		{
			sprintf( pDst, "%s", &pSrc[iSrc] );
			break;		
		}

		pCut[iCut++]   = pSrc[iSrc++];

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)pCut[iCut - 1] ) )
#else
		if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)pCut[iCut - 1] ) )
#endif

#else
		if( IsDBCSLeadByte( (BYTE)pCut[iCut - 1] ) )
#endif

			pCut[iCut++] = pSrc[iSrc++];
	}	
	m_iLeftCutPos = iCut;
}

void ioEdit::GetTextRightCut( char *pSrc, char *pDst, char *pCut, float fWidth )
{
	if( pSrc == NULL || pDst == NULL || pCut == NULL ) 
		return;

	if( g_FontMgr.GetTextWidth( pSrc, m_TextStyle, m_fScale ) < fWidth )
	{
		sprintf( pDst, "%s", pSrc );
		return;
	}
	
	int iLen = strlen( pSrc );
	int iSrc,iTemp;
	char szTemp[MAX_EDIT_TEXT*2] = "";
	memset( szTemp, 0, sizeof( szTemp ) );
	for(iSrc = 0, iTemp = 0;iSrc < iLen;)
	{
		int iPrevTemp = iTemp;
		szTemp[iTemp++]   = pSrc[iSrc++];

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szTemp[iTemp - 1] ) )
#else
		if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szTemp[iTemp - 1] ) )
#endif

#else
		if( IsDBCSLeadByte( (BYTE)szTemp[iTemp - 1] ) )
#endif

			szTemp[iTemp++] = pSrc[iSrc++];
		
		if( g_FontMgr.GetTextWidth( szTemp, m_TextStyle, m_fScale ) >= fWidth )
		{
			int iDstLen = strlen( pDst );
			int iGapLen = g_InputBox.GetCaretPos() - ( iDstLen + m_iLeftCutPos );
			if( iGapLen >= 0 )
                m_iPrevLeftCut = m_iLeftCutPos + ( iTemp - iPrevTemp );
			return;
		}
		sprintf( pDst, "%s", szTemp );
		sprintf( pCut, "%s", &pSrc[iSrc] );
	}	
}

int ioEdit::GetCurCaretLine()
{
	return m_iCurCaretLine;
}

int ioEdit::GetMaxLine()
{
	return m_iMaxLine;
}

int ioEdit::GetCurPage()
{
	return m_iCurPage;
}

int ioEdit::GetMaxPage()
{
	int iPage = max(0, GetMultiLineCnt()-1) / max(1, m_iMaxLine);
	return iPage * m_iMaxLine;
}

void ioEdit::PrevPage()
{
	m_iCurPage -= m_iEndLineMoveAmount;
	m_iCurPage = max( 0, m_iCurPage );
}

void ioEdit::NextPage()
{
	int iPage = max(0, GetMultiLineCnt()-1) / max(1, m_iMaxLine);
	m_iCurPage += m_iEndLineMoveAmount;
	m_iCurPage = min( m_iCurPage, iPage * m_iMaxLine );
}

void ioEdit::SpliteTextWidth( std::string&szSource, std::string&szLeftDest, std::string&szRightDest )
{
	//Width 체크를 위한 임시 버퍼
	char szBuffer[MAX_EDIT_TEXT];
	memset( szBuffer, 0, sizeof( szBuffer ) );

	std::string::const_iterator iter = szSource.begin();
	float fWdith = 0;
	for( int i = 0; iter != szSource.end(); ++iter, ++i )
	{
		szBuffer[i] = *iter;
		float fWidth = g_FontMgr.GetTextWidth( szBuffer, m_TextStyle, m_fScale );
		if( GetWidth() < fWidth )
		{
			szBuffer[i] = NULL;
			if( CheckHanByte( szBuffer ) )
			{
				szBuffer[--i] = NULL;
			}
			szLeftDest  = szSource.substr( 0, i );
			szRightDest = szSource.substr( i, szSource.length() );
			break;
		}
	}
}

void ioEdit::DevideMultiLineText( int iCurLine )
{	
	if( (int)m_vMultiText.size() <= iCurLine ) return;
	
	std::string szPrev;
	std::string szNext;
	std::string& szCurLine  = GetMultiLineText( iCurLine );
	std::string& szNextLine = GetMultiLineText( iCurLine+1 );

	float fWidth = g_FontMgr.GetTextWidth( szCurLine.c_str(), m_TextStyle, m_fScale );
	if( GetWidth() < fWidth )
	{
		SpliteTextWidth( szCurLine, szPrev, szNext );
		if( !szPrev.empty() )
		{
			szCurLine = szPrev;
		}
		if( !szNext.empty() )
		{
			szNextLine = szNextLine + szNext;
			DevideMultiLineText( iCurLine +1 );
		}
	}
}

void ioEdit::MultiLineEnter()
{
	if( m_iMaxLen - 1 <= GetMultiLineTextLength() )	
	{
		KillKeyFocus();
		m_pParent->iwm_command( this, IOED_INPUT, true );
		return;
	}

	StringVector::iterator iter = m_vMultiText.begin();
	std::advance( iter, m_iCurCaretLine );
	if( iter != m_vMultiText.end() )
	{
		std::string& szBuff = (*iter);
		int iCaretPos = min( (int)szBuff.length(), g_InputBox.GetCaretPos() );
		std::string szPrev = szBuff.substr( 0, iCaretPos );
		std::string szNext = szBuff.substr( iCaretPos, szBuff.length() );

		szBuff = szPrev;
		if( ++iter == m_vMultiText.end() )
		{
			m_vMultiText.push_back( szNext );
			g_InputBox.SetString( szNext.c_str() );
			if( !szNext.empty() )
			{
				g_InputBox.SetCaretPos( 0 );
			}
		}
		else
		{
			m_vMultiText.insert( iter, szNext );
			g_InputBox.SetString( szNext.c_str() );
			if( !szNext.empty() )
			{
				g_InputBox.SetCaretPos( 0 );
			}
		}

		m_iCurCaretLine++;
	}
	else
	{
		int iCaretPos = g_InputBox.GetCaretPos();
		std::string szBuff = g_InputBox.GetString();
		m_vMultiText.push_back( szBuff );
		g_InputBox.Clear();
		m_iCurCaretLine++;
	}

	SetCaretViewPage();

	if( m_pParent )
		m_pParent->iwm_command( this, IOED_EX_NEWLINECHAR, true );	
}

void ioEdit::MultiLineBackSpace()
{
	if( 0 < m_iCurCaretLine && g_InputBox.GetCaretPos() == 0 )
	{
		//프로세스상에서 첫번째 글자가 제거 후 캐럿을 0으로 이동한 후에 현재 로직이 동작하기 때문에 첫번째 글자가 지워질때
		//행이 제거되지 않도록 현재 캐럿과 이전 캐럿의 위치가 0이 였을때 행을 제거 한다.
		if( m_iPrevCaretPos == 0 )
		{
			StringVector::iterator iter = m_vMultiText.begin();
			std::advance( iter, m_iCurCaretLine );
			if( iter == m_vMultiText.end() )
				return;

			std::string& szCurLineText = (*iter);
			if( szCurLineText.empty() )
			{
				m_iCurCaretLine--;
				if( --iter != m_vMultiText.end() )
				{
					std::string& szPrevLineText = (*iter);
					g_InputBox.SetString( szPrevLineText.c_str() );					
					m_vMultiText.erase( iter );

					if( m_pParent )
						m_pParent->iwm_command( this, IOED_EX_NEWLINECHAR, true );
				}
			}
			else
			{
				m_iCurCaretLine--;
				if( --iter != m_vMultiText.end() )
				{
					std::string& szPrevLineText = (*iter);
					if( szPrevLineText.empty() )
					{
						g_InputBox.SetString( szCurLineText.c_str() );
						g_InputBox.SetCaretPos( 0 );
						m_vMultiText.erase( iter );

						if( m_pParent )
							m_pParent->iwm_command( this, IOED_EX_NEWLINECHAR, true );
					}
					else
					{					
						//글자가 한글인지 체크
						char szLast = (*szPrevLineText.rbegin());
						if( (unsigned char)(szLast) > 0x7f )
						{
							//ㄱㄹㅜㅏ...등의 완성되지 않은 한글인지 체크
							std::string szFirstChar = szPrevLineText.substr( max(0, szPrevLineText.length()- 2), szPrevLineText.length() );
							if( CheckConstHanByte( szFirstChar.c_str() ) )
								szPrevLineText = szPrevLineText.substr( 0, max(0, szPrevLineText.length()- 1) );
							else
								szPrevLineText = szPrevLineText.substr( 0, max(0, szPrevLineText.length()- 2) );
						}
						else
						{
							szPrevLineText = szPrevLineText.substr( 0, max(0, szPrevLineText.length()- 1) );
						}

						int iCaretPos = szPrevLineText.length();
						szPrevLineText += szCurLineText;

						//결합 후 Width 체크
						std::string szPrev;
						std::string szNext;
						SpliteTextWidth( szPrevLineText, szPrev, szNext );

						if( szNext.empty() )
						{
							g_InputBox.SetString( szPrevLineText.c_str() );
							g_InputBox.SetCaretPos( iCaretPos );
							m_vMultiText.erase( iter );
						}
						else
						{
							szPrevLineText = szPrev;
							szCurLineText  = szNext;
							g_InputBox.SetString( szPrev.c_str() );
						}

						if( m_pParent )
							m_pParent->iwm_command( this, IOED_EX_NEWLINECHAR, true );
					}
				}
			}
		}
	}

	SetCaretViewPage();
}

void ioEdit::MultiLineDel()
{
	StringVector::iterator iter = m_vMultiText.begin();
	std::advance( iter, m_iCurCaretLine );
	if( iter != m_vMultiText.end() )
	{
		std::string& szCurLineText = (*iter);
		if( szCurLineText.empty() )
		{
			iter = m_vMultiText.erase( iter );
			if( iter != m_vMultiText.end() )
			{
				const std::string& szBuffer = (*iter);
				if( iter != m_vMultiText.end() )
				{
					g_InputBox.SetString( szBuffer.c_str() );
					g_InputBox.SetCaretPos( 0 );
				}
			}

			if( m_pParent )
				m_pParent->iwm_command( this, IOED_EX_NEWLINECHAR, true );
	
		}
		else
		{
			std::string szBuff = szCurLineText.substr( max( (int)szCurLineText.length(), g_InputBox.GetCaretPos()), szCurLineText.length() );
			if( szBuff.empty() )
			{
				if( ++iter != m_vMultiText.end() )
				{					
					int iPos = szCurLineText.length();
					std::string& szNextLineText = (*iter);

					if( szNextLineText.empty() )
					{
						m_vMultiText.erase( iter );
						g_InputBox.SetString( szCurLineText.c_str() );
					}
					else
					{
						//현재 라인과 다음라인을 결합하기전에 다음라인의 첫번째 글자를 제거 한 후 결합
						std::string szFirstChar = szNextLineText.substr( 0, 2 );

						//글자가 한글인지 체크
						char szFirst = (*szNextLineText.begin());
						if( (unsigned char)(szFirst) > 0x7f )
						{
							//
							std::string szFirstChar = szNextLineText.substr( 2, szNextLineText.length() );
							if( CheckConstHanByte( szFirstChar.c_str() ) )							
								szNextLineText = szNextLineText.substr( 1, szNextLineText.length() );							
							else							
								szNextLineText = szNextLineText.substr( 2, szNextLineText.length() );							
						}
						else
						{
							szNextLineText = szNextLineText.substr( 1, szNextLineText.length() );
						}

						//글자 제거 후 비었다면 라인 제거
						if( szNextLineText.empty() )
						{
							m_vMultiText.erase( iter );
							g_InputBox.SetString( szCurLineText.c_str() );
						}
						else
						{
							szCurLineText += szNextLineText;

							//결합 후 Width 체크
							std::string szPrev;
							std::string szNext;
							SpliteTextWidth( szCurLineText, szPrev, szNext );

							if( szNext.empty() )
							{
								g_InputBox.SetString( szCurLineText.c_str() );
								g_InputBox.SetCaretPos( iPos );
								m_vMultiText.erase( iter );
							}
							else
							{
								szCurLineText  = szPrev;
								szNextLineText = szNext;
								g_InputBox.SetString( szCurLineText.c_str() );
								g_InputBox.SetCaretPos( iPos );
							}
						}

						if( m_pParent )
							m_pParent->iwm_command( this, IOED_EX_NEWLINECHAR, true );						
					}
				}
			}
		}
	}

	SetCaretViewPage();
}

void ioEdit::MultiLineLeft()
{
	if( 0 < m_iCurCaretLine && g_InputBox.GetCaretPos() == 0 )
	{
		//프로세스상에서 캐럿이 첫번째 글자 이전 위치로 이동 한 후에 로직이 동작하기 때문에
		//방향키가 한번 더 눌렸는지를 체크
		if( m_iPrevCaretPos	== 0 )
		{
			StringVector::const_iterator iter = m_vMultiText.begin();
			std::advance( iter, m_iCurCaretLine-1 );
			if( iter != m_vMultiText.end() )
			{
				m_iCurCaretLine--;
				const std::string& szBuff = (*iter);
				g_InputBox.SetString( szBuff.c_str() );

				//캐럿이 안보이면 페이지 이동
				SetCaretViewPage();
			}
		}
	}

	m_iPrevCaretPos = g_InputBox.GetCaretPos();
}

void ioEdit::MultiLineRight()
{
	StringVector::const_iterator iter = m_vMultiText.begin();
	std::advance( iter, m_iCurCaretLine );
	if( iter != m_vMultiText.end() )
	{
		if( (int)(*iter).length() <= g_InputBox.GetCaretPos() )
		{
			//프로세스상에서 캐럿이 첫번째 글자 이전 위치로 이동 한 후에 로직이 동작하기 때문에
			//방향키가 한번 더 눌렸는지를 체크
			if( (int)(*iter).length() <= m_iPrevCaretPos )
			{
				if( ++iter != m_vMultiText.end() )
				{
					m_iCurCaretLine++;
					const std::string& szBuff = (*iter);
					g_InputBox.SetString( szBuff.c_str() );
					g_InputBox.SetCaretPos( 0 );

					//캐럿이 안보이면 페이지 이동
					SetCaretViewPage();
				}
			}
		}
	}
	m_iPrevCaretPos = g_InputBox.GetCaretPos();
}

void ioEdit::MultiLineUp()
{
	if( m_iCurCaretLine == 0 )
		return;

	StringVector::const_iterator iter = m_vMultiText.begin();
	std::advance( iter, max(0, m_iCurCaretLine-1) );
	if( iter != m_vMultiText.end() )
	{
		m_iCurCaretLine--;
		const std::string& szBuff = (*iter);
		g_InputBox.SetString( szBuff.c_str() );

		//캐럿이 안보이면 페이지 이동
		SetCaretViewPage();
	}
}

void ioEdit::MultiLineDown()
{
	if( m_iCurCaretLine == m_vMultiText.size()-1 )
		return;

	StringVector::const_iterator iter = m_vMultiText.begin();
	std::advance( iter, m_iCurCaretLine+1 );
	if( iter != m_vMultiText.end() )
	{
		m_iCurCaretLine++;
		const std::string& szBuff = (*iter);
		g_InputBox.SetString( szBuff.c_str() );

		//캐럿이 안보이면 페이지 이동
		SetCaretViewPage();
	}
}

void ioEdit::MultiLineProcess()
{
	StringVector::iterator iter = m_vMultiText.begin();
	std::advance( iter, m_iCurCaretLine );
	if( iter == m_vMultiText.end() )
	{
		if( CheckMaxLength() )
		{
			std::string szBuff = g_InputBox.GetString();
			m_vMultiText.push_back( szBuff );
		}
	}
	else
	{
		if( CheckMaxLength() )
		{
			std::string& szCurLineBuff = (*iter);
			szCurLineBuff = g_InputBox.GetString();

			std::string szPrev;
			std::string szNext;
			SpliteTextWidth( szCurLineBuff, szPrev, szNext );

			if( !szPrev.empty() )
			{
				szCurLineBuff = szPrev;

				int iCaretPos = g_InputBox.GetCaretPos();
				if( iCaretPos < (int)szPrev.length() )
				{
					g_InputBox.SetString( szCurLineBuff.c_str() );
					g_InputBox.SetCaretPos( iCaretPos );
				}
			}

			if( !szNext.empty() )
			{
				iter++;
				if( iter == m_vMultiText.end() )
				{
					m_vMultiText.push_back( szNext );
				}
				else
				{
					int iPos = szNext.length();
					std::string& szLineBuff = (*iter);
					szNext += szLineBuff;
					szLineBuff = szNext;

					DevideMultiLineText( m_iCurCaretLine + 1 );

					int iCaretPos = g_InputBox.GetCaretPos();
					if( (int)szPrev.length() <= iCaretPos )
					{
						m_iCurCaretLine++;
						g_InputBox.SetString( szLineBuff.c_str() );
						g_InputBox.SetCaretPos( iPos );
					}
				}

				if( m_pParent )
					m_pParent->iwm_command( this, IOED_EX_NEWLINECHAR, true );
			}
		}	
		else
		{
			std::string& szCurLineBuff = (*iter);
			g_InputBox.SetString( szCurLineBuff.c_str() );
		}
	}
	
	//캐럿이 안보이면 페이지 이동
	if( g_InputBox.IsKeyDown() )
		SetCaretViewPage();

	//개행 후에 바로 백스페이스나 방향키로 행제거 및 행이동을 할 수 있도록 위치저장
	m_iPrevCaretPos = g_InputBox.GetCaretPos();
}

bool ioEdit::CheckMaxLength()
{
	int iLength = 0;
	std::string szCurLine;
	StringVector::iterator iter = m_vMultiText.begin();
	for( int i = 0; iter != m_vMultiText.end(); ++iter, ++i )
	{
		if( m_iCurCaretLine == i )
		{
			szCurLine = g_InputBox.GetString();
			iLength += szCurLine.length();
		}
		else
		{
			std::string kStr = (*iter);
			if( kStr.length() == 0 )
				iLength += 1;
			else
				iLength += kStr.length();
		}
	}

	if( iLength < m_iMaxLen )
		return true;
	else
		return false;
}

int ioEdit::GetMultiLineTextLength()
{
	int iLength = 0;
	std::string szCurLine;
	StringVector::iterator iter = m_vMultiText.begin();
	for( ; iter != m_vMultiText.end(); ++iter )
	{
		std::string kStr = (*iter);
		if( kStr.length() == 0 )
			iLength += 1;
		else
			iLength += kStr.length();
	}

	return iLength;
}

int ioEdit::GetMultiLineCnt()
{
	return (int)m_vMultiText.size();
}

std::string& ioEdit::GetMultiLineText( int iCurLine )
{
	int iSize = (int)m_vMultiText.size();
	if( iCurLine < iSize )
	{
		return m_vMultiText[iCurLine];
	}
	else
	{
		std::string szBuffer;
		m_vMultiText.push_back( szBuffer );
		return m_vMultiText[iSize];
	}
}

void ioEdit::SetCaretViewPage()
{
	int iSize = (int)m_vMultiText.size();
	if( m_iCurCaretLine < m_iCurPage )
	{
		m_iCurPage -= m_iEndLineMoveAmount;
		m_iCurPage = max( 0, m_iCurPage );
	}
	else if( m_iCurPage + m_iMaxLine <= m_iCurCaretLine )
	{
		m_iCurPage += m_iEndLineMoveAmount;
	}
}

void ioEdit::SetMultiText( StringVector& vMultiText )
{
	m_vMultiText.clear();
	m_vMultiText = vMultiText;

	m_iCurPage = 0;
	SetCaretLine( 0 );

	StringVector::iterator iter = m_vMultiText.begin();
	std::advance( iter, m_iCurCaretLine );

	if( iter == m_vMultiText.end() )
	{
		g_InputBox.Clear();
	}
	else
	{
		g_InputBox.Clear();
		g_InputBox.SetString( iter->c_str() );
	}

	SetCaretViewPage();
}

void ioEdit::SetCaretLine( int iLine )
{
	m_iCurCaretLine = iLine;

	m_iCurCaretLine = max( 0, m_iCurCaretLine );
	m_iCurCaretLine = min( m_iCurCaretLine, max( 0, (int)m_vMultiText.size()-1) );
	
	StringVector::iterator iter = m_vMultiText.begin();
	std::advance( iter, m_iCurCaretLine );

	if( iter != m_vMultiText.end() )
	{
		g_InputBox.Clear();
		g_InputBox.SetString( iter->c_str() );
	}
	SetCaretViewPage();
}

void ioEdit::ClearMultiText()
{
	m_vMultiText.clear();
	m_iCurPage = 0;
	m_iCurCaretLine = 0;
	g_InputBox.Clear();
}

int ioEdit::GetMaxLength()
{
	return m_iMaxLen;
}
