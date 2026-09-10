

#include "stdafx.h"

#include "../io3DEngine/ioAxisAlignBox.h"

#include "ioComplexStringPrinter.h"

#include "ioCharChatBubble.h"
#include "io3DCharEmoticon.h"

#include "ioBaseChar.h"

ioHashString ioCharChatBubble::m_szChatBubble;
ioHashString ioCharChatBubble::m_szChatBubbleTail;
ioHashString ioCharChatBubble::m_szEmoticonBack;
DWORD ioCharChatBubble::m_dwStartTime = 100;
DWORD ioCharChatBubble::m_dwKeepTime = 5000;
int   ioCharChatBubble::m_iDefatulW = 56;
int   ioCharChatBubble::m_iDefatulH = 52;
int   ioCharChatBubble::m_iMaxStringSize = 75;
int   ioCharChatBubble::m_iMaxStringLine = 3;
int   ioCharChatBubble::m_iStartStringX  = 50;
int   ioCharChatBubble::m_iStartStringY  = 26;
int   ioCharChatBubble::m_iStringLineStartGap = 7;
int   ioCharChatBubble::m_iStringLineGap = 14;
int   ioCharChatBubble::m_iEmoticonX     = 26;
int   ioCharChatBubble::m_iEmoticonY     = 26;
float ioCharChatBubble::m_fEmoticonScale = 0.60f; 

ioCharChatBubble::ioCharChatBubble()
{
	m_pChatBubble	  = NULL;
	m_pChatBubbleTail = NULL;
	m_pEmoticon       = NULL;
	m_pEmoticonBack   = NULL;

	m_p3DChatEmoticon = NULL;

	m_iBubbleWidth  = 0;
	m_iBubbleHeight = 0;
	m_dwCurrentTime = 0;
	m_dwStartAniTime= 0;
	m_bWholeChat    = false;

	m_dwCurKeepTime = 0;
}

ioCharChatBubble::~ioCharChatBubble()
{
	SAFEDELETE( m_pChatBubble );
	SAFEDELETE( m_pChatBubbleTail );
	SAFEDELETE( m_pEmoticon );
	SAFEDELETE( m_pEmoticonBack );

	SAFEDELETE( m_p3DChatEmoticon );
}

void ioCharChatBubble::LoadStaticValues()
{
	ioINILoader_e kLoader( "config/sp2_gui.ini" );

	kLoader.SetTitle_e( "chat_bubble" );

	char szBuf[MAX_PATH] = "";
	kLoader.LoadString_e( "chat_bubble_frame", "", szBuf, MAX_PATH );
	m_szChatBubble = szBuf;

	kLoader.LoadString_e( "chat_bubble_tail", "", szBuf, MAX_PATH );
	m_szChatBubbleTail = szBuf;

	kLoader.LoadString_e( "emoticon_back", "", szBuf, MAX_PATH );
	m_szEmoticonBack = szBuf;

	m_iDefatulW = kLoader.LoadInt_e( "default_bubble_width", 56 );
	m_iDefatulH = kLoader.LoadInt_e( "default_bubble_height", 52 );
	m_iMaxStringSize = kLoader.LoadInt_e( "max_string_size", 75 );
	m_iMaxStringLine = kLoader.LoadInt_e( "max_string_line", 3 );
	m_iStartStringX  = kLoader.LoadInt_e( "start_string_x", 50 );
	m_iStartStringY  = kLoader.LoadInt_e( "start_string_y", 26 );
	m_iStringLineStartGap = kLoader.LoadInt_e( "string_line_start_gap", 7 );
	m_iStringLineGap = kLoader.LoadInt_e( "string_line_gap", 14 );
	m_iEmoticonX     = kLoader.LoadInt_e( "emoticon_x", 26 );
	m_iEmoticonY     = kLoader.LoadInt_e( "emoticon_y", 26 );
	m_fEmoticonScale = kLoader.LoadFloat_e( "emoticon_scale", 0.60f ); 
}

bool ioCharChatBubble::IsBubbleChat() const
{
	return !m_BubbleList.empty();
}

void ioCharChatBubble::SetChatBubble( const char *szID, const char *szChat, bool bWholeChat )
{
	if( !m_pChatBubble )
	{
		m_pChatBubble = g_GUIMgr.CreateFrame( m_szChatBubble );
	}

	if( !m_pChatBubbleTail )
	{
		m_pChatBubbleTail = g_UIImageSetMgr.CreateImageByFullName( m_szChatBubbleTail );
	}

	if( !m_pEmoticonBack )
	{
		m_pEmoticonBack = g_UIImageSetMgr.CreateImageByFullName( m_szEmoticonBack );
	}

	if( szChat == NULL ) return;
	if( strlen( szChat ) == 0 ) return;

	m_bWholeChat   = bWholeChat;
	m_szBubbleChat = szChat;

	m_BubbleList.clear();
	DWORD dwTextColor = 0xFF3A3A3A;
	if( bWholeChat )
	{	
		dwTextColor = 0xFF005A53;
	}

	int iWidth = 0;
	int iLen = (int)strlen( szChat );
	int iResult = 0;
	for(int i = 0;i < m_iMaxStringLine;i++)
	{
		ChatBubble kBubble;
		kBubble.m_dwTextColor = dwTextColor;

		char szDst[MAX_PATH] = "";
		memset( szDst, 0, sizeof( szDst ) );
		if( i == m_iMaxStringLine - 1 )
		{
			sprintf( szDst, "%s", &szChat[iResult] ); // 마지막 채팅은 전부 넣는다.			
			iResult = iLen;
		}
		else
		{
			iResult += Help::StringCutFun( FONT_SIZE_12, m_iMaxStringSize, TS_NORMAL, szDst, sizeof( szDst ), &szChat[iResult] );
		}
		kBubble.m_szText = szDst;
		m_BubbleList.push_back( kBubble );

		int iTextWidth = min( m_iMaxStringSize, g_FontMgr.GetTextWidth( szDst, TS_NORMAL, FONT_SIZE_12 ) );
		if( iWidth < iTextWidth )
			iWidth = iTextWidth;

		if( iResult >= iLen ) break;
	}

	m_szBubbleName  = szID;
	m_dwStartAniTime = FRAMEGETTIME();
	m_dwCurrentTime  = FRAMEGETTIME();

	m_iBubbleWidth  = m_iDefatulW + iWidth;
	m_iBubbleHeight = m_iDefatulH;

	m_dwCurKeepTime = m_dwKeepTime;
}

void ioCharChatBubble::SetChatBubbleEmoticon( const ioHashString &rkEmoticonName )
{
	SAFEDELETE( m_pEmoticon );
	m_pEmoticon = g_UIImageSetMgr.CreateImageByFullName( rkEmoticonName );
}

void ioCharChatBubble::Set3DChatBubbleEmoticon( const CHARACTER &rkInfo, int iChatLen, bool bOwnerChar, bool bNoSound )
{
	SAFEDELETE( m_pEmoticon );

	if( !m_p3DChatEmoticon )
	{
		m_p3DChatEmoticon = new io3DCharEmoticon;
		m_p3DChatEmoticon->Initialize( "chat_bubble_cam" );
	}

	m_p3DChatEmoticon->SetAvata( rkInfo, iChatLen, bNoSound, SPA_NONE, VIEW_WIDTH, VIEW_HEIGHT );
}

bool ioCharChatBubble::IsScreenInRect( int iXPos, int iYPos )
{
	iXPos -= ( m_iBubbleWidth / 2 );

	if( iXPos < 0 ) return false;
	if( iXPos + m_iBubbleWidth > Setting::Width() ) return false;
	if( iYPos < 0 ) return false;
	if( iYPos + m_iBubbleHeight > Setting::Height() - 43 ) return false;

	return true;
}

bool ioCharChatBubble::SetScreenInPos( int &rXPos, int &rYPos )
{
	bool bScreenInMove = false;
	int iXPos = rXPos;
	int iYPos = rYPos;
	if( iXPos - (m_iBubbleWidth/2) < 1 ) 
	{
		iXPos = 1 + (m_iBubbleWidth/2);
		bScreenInMove = true;
	}
	else if( iXPos + (m_iBubbleWidth/2) > Setting::Width() - 1 ) 
	{
		iXPos = (Setting::Width()-1) - (m_iBubbleWidth/2);
		bScreenInMove = true;
	}

	if( iYPos - m_iBubbleHeight < 1 ) 
	{
		iYPos = 1;
		bScreenInMove = true;
	}
	else if( iYPos > Setting::Height() - 43 )
	{
		iYPos = (Setting::Height()-43);
		bScreenInMove = true;
	}

	rXPos = iXPos;
	rYPos = iYPos;
	return bScreenInMove;
}

bool ioCharChatBubble::SetVisiblePos( int &rXPos, int &rYPos )
{
	// 중앙을 기준으로 화면 밖으로 보낸다.
	float fAngle = 360+90-(int)RADtoDEG( atan2( (double)rXPos - Setting::HalfWidth(), Setting::HalfHeight() - rYPos ) );
	rXPos += Setting::HalfWidth() * cos(DEGtoRAD(fAngle));
	rYPos -= Setting::HalfHeight() * sin(DEGtoRAD(fAngle));
	return SetScreenInPos( rXPos, rYPos );
}

void ioCharChatBubble::CheckOutScreen( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCamTargetPos, int &iXPos, int &iYPos, bool bVisible )
{
	int iCenterX = Setting::Width() / 2;
	int iCenterY = Setting::Height() / 2;

	int iWidth = m_iBubbleWidth / 2;
	int iHeight = m_iBubbleHeight / 2;

	int iCurXPos, iCurYPos;
	iCurXPos = iXPos;
	iCurYPos = iYPos;

	int iXGap = (int)(vCamTargetPos.x - vPos.x);
	int iZGap = (int)(vPos.z - vCamTargetPos.z);

	iCurXPos = iCenterX - iXGap;
	iCurYPos = iCenterY - iZGap;

	iXPos = iCurXPos;
	iYPos = iCurYPos;

	bool bAxisX = false;
	if( !bVisible )
	{
		D3DXVECTOR3 v2DPos = Setting::Convert3DTo2D( vPos );
		if( v2DPos.z > 1 )
		{
			v2DPos.x = -v2DPos.x;
			v2DPos.y = fabs( v2DPos.y );

			if( COMPARE( v2DPos.x, 0.0f, Setting::Width() ) )
				v2DPos.x += Setting::Width();

			if( COMPARE( v2DPos.y, 0.0f, Setting::Height() ) )
				v2DPos.y += Setting::Height();
		}

		float fXRate = fabs( (float)(iCenterX - v2DPos.x) ) / (float)Setting::Width();
		float fYRate = fabs( (float)(iCenterY - v2DPos.y) ) / (float)Setting::Height();

		if( fXRate >= fYRate )
			bAxisX = true;

		if( bAxisX )
		{
			if( v2DPos.x < iCenterX )
				iXPos = iWidth;
			else
				iXPos = Setting::Width() - iWidth;
		}
		else
		{
			if( v2DPos.y < iCenterY )
			{
				iYPos = iHeight;
			}
			else
			{
				iYPos = Setting::Height() - iHeight;
				iYPos -= 43;
			}
		}
		return;
	}

	bool bRePosition = true;
	if( COMPARE( iCurXPos, 0, Setting::Width() ) &&
		COMPARE( iCurYPos, 0, Setting::Height() ) )
	{
		bRePosition = false;
		float fXRate = fabs( (float)(iCenterX - iCurXPos) ) / (float)Setting::Width();
		float fYRate = fabs( (float)(iCenterY - iCurYPos) ) / (float)Setting::Height();

		if( fXRate >= fYRate )
			bAxisX = true;
	}

	if( iCurXPos < 0 )
	{
		iXPos = iWidth;
	}
	else if( iCurXPos > Setting::Width() )
	{
		iXPos = Setting::Width() - iWidth;
	}

	if( iCurYPos < 0 )
	{
		iYPos = iHeight;
	}
	else if( iCurYPos > Setting::Height() )
	{
		iYPos = Setting::Height() - iHeight;
		iYPos -= 43;
	}

	if( !bRePosition )
	{
		if( bAxisX )
		{
			if( iCenterX < iCenterX )
				iXPos = iWidth;
			else
				iXPos = Setting::Width() - iWidth;
		}
		else
		{
			if( iCenterY < iCenterY )
			{
				iYPos = iHeight;
			}
			else
			{
				iYPos = Setting::Height() - iHeight;
				iYPos -= 43;
			}
		}
	}
}

void ioCharChatBubble::UpdateChatBubble( int iXPos, int iYPos )
{
	if( !IsBubbleChat() ) return;

	if( FRAMEGETTIME() - m_dwCurrentTime > m_dwCurKeepTime )
	{
		m_BubbleList.clear();
	}	
	else if( FRAMEGETTIME() - m_dwStartAniTime > m_dwStartTime )
	{
		if( !Setting::ChatBubbleAlarm() )
		{
			m_BubbleList.clear();
		}
		else
		{
			if( m_pEmoticon == NULL )
			{
				if( m_p3DChatEmoticon )
				{
					iXPos -= ( m_iBubbleWidth / 2 );
					m_p3DChatEmoticon->Update( iXPos + m_iEmoticonX, iYPos + m_iEmoticonY );
				}
			}
		}
	}
}

void ioCharChatBubble::RenderChatBubble( TeamType eTeamType, int iXPos, int iYPos, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCamTargetPos, bool bVisible )
{
	if( !IsBubbleChat() ) return;
	if( !m_pChatBubble ) return;
//	if( !IsScreenInRect( iXPos, iYPos ) ) return;

	bool bScreenInMove = false;
	if( !IsScreenInRect( iXPos, iYPos ) || !bVisible )
	{
		int iChageXPos = iXPos;
		int iChageYPos = iYPos + ( m_iBubbleHeight /  2 );
		CheckOutScreen( vPos, vCamTargetPos, iChageXPos, iChageYPos, bVisible );

		iXPos = max( 1, min( Setting::Width() - (1+( m_iBubbleWidth / 2 )), iChageXPos ) );
		iYPos = max( 1, min( Setting::Height() - (43+( m_iBubbleHeight /  2 )), iChageYPos - ( m_iBubbleHeight /  2 ) ) );
		bScreenInMove = true;
	}

	if( FRAMEGETTIME() - m_dwStartAniTime < m_dwStartTime )
	{
		// 확대 & 축소를 20% ~ 100% 사이만 진행 시킨다.
		DWORD dwGapTime = FRAMEGETTIME() - m_dwStartAniTime;
		float fTimeRate = ( (float)dwGapTime / m_dwStartTime ) * FLOAT08;
		fTimeRate = min( FLOAT08, fTimeRate ) + 0.2f;

		int iWidth = m_iBubbleWidth * fTimeRate;
		int iHeight= m_iBubbleHeight * fTimeRate;
		m_pChatBubble->SetSize( iWidth, iHeight );
		m_pChatBubble->Render( iXPos - ( iWidth / 2 ), iYPos );
	}
	else
	{
		int iAddWidth = 0;
		float fNameSize = 69.0f;
		ioHashString kName = m_szBubbleName;
		if( bScreenInMove )  // 화면 외곽에 그린다면 아이디를 표시해줘야해서 아이디 길이에 따라 사이즈가 변경된다.
		{
			if( g_FontMgr.GetTextWidth( m_szBubbleName.c_str(), TS_NORMAL, FONT_SIZE_12 ) > fNameSize )
				Help::TextCut( m_szBubbleName.c_str(), fNameSize, TS_NORMAL, FONT_SIZE_12, kName );
			char szTempName[MAX_PATH] = "";
			sprintf( szTempName, "[%s]", kName.c_str() );
			int iNewWidth = m_iDefatulW + g_FontMgr.GetTextWidth( szTempName, TS_NORMAL, FONT_SIZE_12 );
			if( m_iBubbleWidth < iNewWidth )
			{
				iAddWidth = iNewWidth - m_iBubbleWidth;
			}
			iXPos -= ( ( m_iBubbleWidth + iAddWidth ) / 2 );
			if( iXPos < 1 )
				iXPos = 1;
			else if( iXPos + (m_iBubbleWidth + iAddWidth) > Setting::Width() - 1 )
				iXPos = (Setting::Width() - 1) - (m_iBubbleWidth + iAddWidth);
		}	
		else
		{
			iXPos -= ( m_iBubbleWidth / 2 );
		}		
		m_pChatBubble->SetSize( (m_iBubbleWidth+iAddWidth), m_iBubbleHeight );
		m_pChatBubble->Render( iXPos, iYPos );

		int iNoneEmoticonGap = 0;
		// 이모티콘 or 얼굴
		if( m_pEmoticonBack && m_pEmoticon )
		{
			m_pEmoticonBack->SetScale( m_fEmoticonScale );
			m_pEmoticonBack->Render( iXPos + m_iEmoticonX, iYPos + m_iEmoticonY, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			m_pEmoticon->SetScale( m_fEmoticonScale );
			m_pEmoticon->Render( iXPos + m_iEmoticonX, iYPos + m_iEmoticonY, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else if( m_p3DChatEmoticon )
		{
			m_p3DChatEmoticon->RenderViewPort( (iXPos + m_iEmoticonX) - (VIEW_WIDTH/2), (iYPos + m_iEmoticonY) - (VIEW_HEIGHT/2), VIEW_WIDTH, VIEW_HEIGHT );
		}
		else
		{
			iNoneEmoticonGap = 26;
		}

		int iListSize = m_BubbleList.size();
		int iMaxLine = min( m_iMaxStringLine, iListSize + (int)bScreenInMove );
		int iTextYPos = m_iStartStringY - ( iMaxLine * m_iStringLineStartGap );
		if( bScreenInMove )
		{
			// 화면 외곽에 그린다면 아이디를 표시
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			if( eTeamType == TEAM_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else if( eTeamType == TEAM_RED )
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED  );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );			
			g_FontMgr.PrintText( iXPos + m_iStartStringX - iNoneEmoticonGap, iYPos + iTextYPos, FONT_SIZE_12, STR(1), kName.c_str() );
		}
		// 기본 풍선 가로 크기 56 + ( 텍스트 길이(최대75) )
		// 기본 풍선 세로 크기 52 고정
		// 텍스트 시작 : 50x(26 - (줄수 x 7)) : 최대 3줄
		// 아이디가 표시된다면 채팅은 2줄만 표시 - 그에 따라 좌표도 변경
		iMaxLine = min( m_iMaxStringLine - (int)bScreenInMove, iListSize );
		for(int i = 0;i < iMaxLine;i++)
		{
			ChatBubble &rkBubble = m_BubbleList[i];

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( rkBubble.m_dwTextColor );

			if( bScreenInMove )
				g_FontMgr.PrintTextWidthCut( iXPos + m_iStartStringX - iNoneEmoticonGap, iYPos + iTextYPos + ((i+1) * m_iStringLineGap), FONT_SIZE_12, m_iMaxStringSize + iNoneEmoticonGap, rkBubble.m_szText.c_str() );
			else
				g_FontMgr.PrintTextWidthCut( iXPos + m_iStartStringX - iNoneEmoticonGap, iYPos + iTextYPos + (i * m_iStringLineGap), FONT_SIZE_12, m_iMaxStringSize + iNoneEmoticonGap, rkBubble.m_szText.c_str() );
		}

		if( !bScreenInMove && m_pChatBubbleTail )
		{
			m_pChatBubbleTail->Render( iXPos + ( m_iBubbleWidth / 2 ) - 5, iYPos + 50 );
		}
	}
}