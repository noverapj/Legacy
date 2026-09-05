

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioSoundManager.h"

#include "../ChatMessageFunc.h"
#include "../ioApplication.h"
#include "../WndID.h"

#include "../NetworkWrappingFunc.h"
#include "../ioMyInfo.h"
#include "../Setting.h"
#include "../ioCustomSoundMgr.h"
#include "../ioSkill.h"
#include "../ioSkillMaker.h"
#include "../ioItem.h"
#include "../ioItemMaker.h"

#include "ioSP2GUIManager.h"
#include "ioUserInfoMgr.h"
#include "ChatMsgWnd.h"
#include "UserInfoToolTip.h"
#include "../io3DEngine/ioStringManager.h"

CustomSoundBtn::CustomSoundBtn()
{
	m_bEnableImg    = true;
	m_pSound        = NULL;
	m_pSoundDisable = NULL;
}

CustomSoundBtn::~CustomSoundBtn()
{
	SAFEDELETE( m_pSound );
	SAFEDELETE( m_pSoundDisable );
}

void CustomSoundBtn::SetInfo( const ioHashString &rszPublicID , bool bEnableImg )
{
	m_szPubicID  = rszPublicID;
	m_bEnableImg = bEnableImg;
}

void CustomSoundBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Sound" )
	{
		SAFEDELETE( m_pSound );
		m_pSound = pImage;
	}
	else if( szType == "SoundDisable" )
	{
		SAFEDELETE( m_pSoundDisable );
		m_pSoundDisable = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void CustomSoundBtn::SetWindowAlpha( int iAlpha )
{
	ioButton::SetWindowAlpha( iAlpha );
	if( m_pSound )
		m_pSound->SetAlpha( iAlpha );
	if( m_pSoundDisable )
		m_pSoundDisable->SetAlpha( iAlpha );
}

void CustomSoundBtn::OnRender()
{
	ioButton::OnRender();

	if( !m_pSound || !m_pSoundDisable ) return;

	enum { XOFFSET = 2, YOFFSET = 4, };
	int iXPos = GetDerivedPosX() + XOFFSET;
	int iYPos = GetDerivedPosY() + YOFFSET;

	if( m_bEnableImg )
		m_pSound->Render( iXPos, iYPos );
	else
		m_pSoundDisable->Render( iXPos, iYPos );
}
//////////////////////////////////////////////////////////////////////////
bool ChatMsgWnd::sg_bChatRefresh = false;
ChatMsgWnd::ChatMsgWnd()
{
	m_dwMouseOverIndex = 0;
	m_iCurPage = m_iMaxChatMsgCnt = 0;
	m_iStartXPos = m_iStartYPos = m_iLineYOffset = m_iLineHeight = 0;

	m_pNormalOverBack = NULL;
	m_pCustomOverBack = NULL;
	m_pCustomOverLine = NULL;
}

ChatMsgWnd::~ChatMsgWnd()
{	
	m_ChatList.clear();

	SAFEDELETE( m_pNormalOverBack );
	SAFEDELETE( m_pCustomOverBack );
	SAFEDELETE( m_pCustomOverLine );
}

void ChatMsgWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "NormalOverBack" )
	{
		SAFEDELETE( m_pNormalOverBack );
		m_pNormalOverBack = pImage;
	}
	else if( szType == "CustomOverBack" )
	{
		SAFEDELETE( m_pCustomOverBack );
		m_pCustomOverBack = pImage;
	}
	else if( szType == "CustomOverLine" )
	{
		SAFEDELETE( m_pCustomOverLine );
		m_pCustomOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ChatMsgWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iStartXPos = xElement.GetIntAttribute_e( "StartXPos" );
	m_iStartYPos = xElement.GetIntAttribute_e( "StartYPos" );
	m_iLineYOffset = xElement.GetIntAttribute_e( "LineYOffset" );
	m_iLineHeight = xElement.GetIntAttribute_e( "LineHeight" );
}

void ChatMsgWnd::iwm_show()
{
	m_iMaxChatMsgCnt = g_ChatMgr.GetMaxChatMsgByFeature();
	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		m_iCurPage = 0;
		pScroll->SetScrollRange( m_iCurPage, m_iMaxChatMsgCnt );
		pScroll->SetScrollPos( m_iMaxChatMsgCnt );

		if( g_ChatMgr.GetMaxChatViewMsg() != pScroll->GetPageRange() )
		{
			g_ChatMgr.SetMaxViewMsgCnt( pScroll->GetPageRange() );
			InitScrollPos();
		}		
	}
	SetCheckParentOutSide( true );

	for(int i = 0;i < MAX_CHAT_OVER_DUMMY;i++)
	{
		ioButton *pChatBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_CHAT_OVER_DUMMY_1 + i ) );
		if( pChatBtn )
			pChatBtn->SetFocusRectTypeIsParent( true );
	}
}

void ChatMsgWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void ChatMsgWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
		if( pScroll == NULL ) return;
		//if( pScroll->HasWndStyle( IWS_INACTIVE ) ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPage - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPage + 1 );

		g_ChatMgr.AllAlphaTimeInit();
	}
}

void ChatMsgWnd::iwm_vscroll(DWORD dwID, int curPos )
{
	UpdateSetPage( curPos );
}

void ChatMsgWnd::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttondown( mouse );
	g_ChatMgr.AllAlphaTimeInit();
}

void ChatMsgWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	//DWORD dwID = pWnd->GetID();
	
	//if( COMPARE( dwID, ID_CHAT_OVER_DUMMY_1, ID_CHAT_OVER_DUMMY_1 + MAX_CHAT_OVER_DUMMY ) )
	//{
	//	if( cmd == IOBN_BTNDOWN )
	//		//g_ChatMgr.AllAlphaTimeInit();
	//}
}

void ChatMsgWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	
	if( !bCreate )
	{
		UpdateSetPage( m_iCurPage );
	}
}

void ChatMsgWnd::UpdateSetPage( int iCurPage )
{
	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( !pScroll ) return;

	int iPrevCurPage = m_iCurPage;
	m_iCurPage = iCurPage;

	int i = 0;
	m_ChatList.clear();
	for(i = 0;i < MAX_CHAT_OVER_DUMMY;i++)
	{
		ioButton *pChatBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_CHAT_OVER_DUMMY_1 + i ) );
		if( pChatBtn )
		{
			pChatBtn->HideWnd();
		}
	}

	int iChatPos = ( pScroll->GetMaxPos() - m_iCurPage );
	int iMaxChatViewMsg = g_ChatMgr.GetMaxChatViewMsg();

	for(i = 0;i < iMaxChatViewMsg;i++)
	{
		IMsgItem *pMsgItem = g_ChatMgr.GetReverseChatMsgByFeature( iChatPos + i );
		if( !pMsgItem ) continue;

		m_ChatList.push_back( pMsgItem );

		if( pMsgItem->GetChatID() == NULL ) continue;
		if( pMsgItem->GetChatID()->IsEmpty() ) continue;

		ioButton *pChatBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_CHAT_OVER_DUMMY_1 + i ) );
		if( pChatBtn )
		{
			pChatBtn->SetSize( pMsgItem->GetChatWidth() + 36, m_iLineHeight );
			pChatBtn->ShowWnd();
		}
	}

	if( iPrevCurPage != m_iCurPage )
	{
		if( m_iCurPage < pScroll->GetMaxPos() )
		{
			g_ChatMgr.AllAlphaTimeInit();
		}
	}
}

void ChatMsgWnd::InitScrollPos()
{
	UpdateSetPage( max( 0, g_ChatMgr.GetMaxChatMsgByFeature() - g_ChatMgr.GetMaxChatViewMsg() ) );
}

void ChatMsgWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	g_ChatMgr.ProcessChat();
    
	OnProcessScroll( sg_bChatRefresh );
	OnProcessMouseOver();
}

bool ChatMsgWnd::IsMouseOverFailed()
{
	if( !g_App.IsVisibleMouse() )
		return true;
	
	if( !g_App.IsMouseRender() )
		return false;

	if( g_GUIMgr.GetPreOverWnd() )
	{
		ioWnd* pPreWnd = g_GUIMgr.GetPreOverWnd();
		if( g_GUIMgr.GetPreOverWnd() == this || g_GUIMgr.GetPreOverWnd()->GetParent() == this )
			return false;		
		if( pPreWnd && pPreWnd->GetWndName() == "GLOBAL_CHAT_WND" || ( g_GUIMgr.GetPreOverWnd()->GetParent() && g_GUIMgr.GetPreOverWnd()->GetParent()->GetWndName() == "GLOBAL_CHAT_WND" ) )
			return false;
	}

	if( g_GUIMgr.GetPreClickWnd() )
	{
		if( g_GUIMgr.GetPreClickWnd() == this || g_GUIMgr.GetPreClickWnd()->GetParent() == this )
			return false;
	}

	return true;
}

void ChatMsgWnd::OnProcessMouseOver()
{
	m_dwMouseOverIndex = 0;
	
	if( IsMouseOverFailed() )
		return;

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	int iXPos = GetDerivedPosX() + m_iStartXPos;
	int iYPos = GetDerivedPosY() + m_iStartYPos;

	int i = 0;
	int iMaxChatViewMsg = m_ChatList.size();
	for(i = 0;i < iMaxChatViewMsg;i++)
	{
		IMsgItem *pMsgItem = m_ChatList[i];
		if( !pMsgItem ) continue;
		
		ioHashString szReturnID;
		POINT ptMousePos = { pMouse->GetMousePos().x, pMouse->GetMousePos().y };
		POINT ptMousePos2 = { pMouse->GetMousePos().x, pMouse->GetMousePos().y };

		ptMousePos.x -= iXPos;
		ptMousePos.y -= ( ( iYPos - (i*m_iLineYOffset) ) + 1 );
		if( pMsgItem->CheckPtInItemRect( ptMousePos, m_iLineHeight, szReturnID ) )
		{
			m_dwMouseOverIndex = pMsgItem->GetSentenceIndex();
			if( pMouse->IsLBtnUp() )
			{
				if( !szReturnID.IsEmpty() )
					g_App.OpenMannerWnd( szReturnID.c_str(), true, pMsgItem->GetChatType() );
				break;
			}
			else if( pMouse->IsMouseShowing() )
			{
				pMouse->SetToolTipID( szReturnID, UserInfoToolTip::TT_GLOBAL );
				break;
			}
		}
	}	
}

void ChatMsgWnd::OnProcessScroll( bool bRefresh )
{
	ioScroll *pScroll = dynamic_cast<ioScroll*>(FindChildWnd( ID_VERT_SCROLL ));
	if( !pScroll ) return;

	if( bRefresh || m_iMaxChatMsgCnt != g_ChatMgr.GetMaxChatMsgByFeature() )
	{
		m_iMaxChatMsgCnt = g_ChatMgr.GetMaxChatMsgByFeature();
		if( pScroll->GetMaxPos() == m_iCurPage )
			m_iCurPage = m_iMaxChatMsgCnt;
		pScroll->SetScrollRange( 0, m_iMaxChatMsgCnt );
		pScroll->SetScrollPos( m_iCurPage );
		sg_bChatRefresh = false;
	}
}

void ChatMsgWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + m_iStartXPos;
	int iYPos = GetDerivedPosY() + m_iStartYPos;

	// Process와 Render 사이에 채팅 내용이 갱신된다... 채팅 내용이 갱신되었으면 Render하기전에 m_CharList를 갱신해줘야함.
	if( sg_bChatRefresh )
	{
		OnProcessScroll( sg_bChatRefresh );
	}

	int iMaxChatViewMsg = m_ChatList.size();
	for(int i = 0;i < iMaxChatViewMsg;i++)
	{
		IMsgItem *pMsgItem = m_ChatList[i];
		if( !pMsgItem ) continue;
	
		if( m_dwMouseOverIndex == pMsgItem->GetSentenceIndex() )
		{
			OnRenderOverBack( iXPos, iYPos - ( i * m_iLineYOffset ), pMsgItem->GetChatWidth(), pMsgItem->GetAlpha(), pMsgItem->IsFirstLine(), pMsgItem->IsMultiLine() );
		}
	}

	for(int i = 0;i < iMaxChatViewMsg;i++)
	{
		IMsgItem *pMsgItem = m_ChatList[i];
		if( !pMsgItem ) continue;

		if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::NORMAL_COLOR_TYPE )
			pMsgItem->DrawItem( iXPos, iYPos - ( i * m_iLineYOffset ), ( m_dwMouseOverIndex == pMsgItem->GetSentenceIndex() ) );		
		else
			pMsgItem->DrawItem( iXPos, iYPos - ( i * m_iLineYOffset ), false );	 // 마우스 오버시 채팅 컬러를 바꾸지 않는다.
	}
}

void ChatMsgWnd::OnRenderOverBack (int iXPos, int iYPos, float fFullWidth, BYTE bAlphaRate, bool bFirstLine, bool bMultiLine )
{
	if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::NORMAL_COLOR_TYPE )
	{
		if( m_pNormalOverBack )
		{
			m_pNormalOverBack->SetAlpha( bAlphaRate );
			m_pNormalOverBack->Render( iXPos + 1, iYPos );
		}
	}
	else if( bFirstLine )
	{
		if( m_pCustomOverBack )
		{
			if( bMultiLine )
				m_pCustomOverBack->SetSize( fFullWidth + 36, 18 + 19 );
			else
				m_pCustomOverBack->SetSize( fFullWidth + 36, 18 );
			m_pCustomOverBack->SetAlpha( bAlphaRate );
			m_pCustomOverBack->Render( iXPos, iYPos - 1, UI_RENDER_ADD );
		}

		if( m_pCustomOverLine )
		{
			m_pCustomOverLine->SetSize( fFullWidth + 36, 1 );

			// 위 라인
			m_pCustomOverLine->Render( iXPos, iYPos - 2 );

			// 아래 라인
			if( bMultiLine )
				m_pCustomOverLine->Render( iXPos, iYPos + 17 + 19 );
			else
				m_pCustomOverLine->Render( iXPos, iYPos + 17 );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
KOChatMsgWnd::KOChatMsgWnd()
{
	m_pIconBack	= NULL;
	m_pNoneKillerIcon = NULL;
	m_pNoneSkillIcon = NULL;

	m_iCurPage = 0;	
	m_iMaxChatViewCnt = 3;
	m_dwAlphaTime = 0;
	m_dwAlphaStartTime = 0;
	m_bScrollAlphaInit = true;
	m_iStartXPos = 0;
	m_iStartYPos = 0;
	m_iLineYOffset = 0;
	m_iLineHeight = 0;
	m_iDefaultX  = 0;
	m_iDefaultY  = 0;
}

KOChatMsgWnd::~KOChatMsgWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNoneKillerIcon );
	SAFEDELETE( m_pNoneSkillIcon );

	for(int i = 0;i < (int)m_ItemList.size();i++)
	{
		SAFEDELETE( m_ItemList[i].m_pSkillIcon );
	}
	m_ItemList.clear();

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "KOChatMsgWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "KOChatMsgWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void KOChatMsgWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	float fPosGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	m_iDefXPos = iTempDefXPos + fPosGap;
	m_iDefYPos = iTempDefYPos + fPosGap;

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY );
	}
}

void KOChatMsgWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "KOChatMsgWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "KOChatMsgWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();
}

void KOChatMsgWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iStartXPos = xElement.GetIntAttribute_e( "StartXPos" );
	m_iStartYPos = xElement.GetIntAttribute_e( "StartYPos" );
	m_iLineYOffset = xElement.GetIntAttribute_e( "LineYOffset" );
	m_iLineHeight = xElement.GetIntAttribute_e( "LineHeight" );
	m_iMaxChatViewCnt = xElement.GetIntAttribute_e( "MaxChatViewCnt" );
	m_dwAlphaTime = xElement.GetIntAttribute_e( "AlphaTime" );
	m_dwAlphaStartTime = xElement.GetIntAttribute_e( "AlphaStartTime" );
	m_szMsgSnd = xElement.GetStringAttribute_e( "MsgSound" );
}

void KOChatMsgWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "NoneKillerIcon" )
	{
		SAFEDELETE( m_pNoneKillerIcon );
		m_pNoneKillerIcon = pImage;
	}
	else if( szType == "NoneSkillIcon" )
	{
		SAFEDELETE( m_pNoneSkillIcon );
		m_pNoneSkillIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void KOChatMsgWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		bool bScrollShow = false;
		if( g_App.IsVisibleMouse() && !m_ItemList.empty() )
		{	
			if( g_GUIMgr.GetPreOverWnd() )
			{
				if( g_GUIMgr.GetPreOverWnd() == this || g_GUIMgr.GetPreOverWnd()->GetParent() == this || 
					g_GUIMgr.GetPreOverWnd() == pScroll || g_GUIMgr.GetPreOverWnd()->GetParent() == pScroll )
				{
					bScrollShow = true;
				}
			}
			else if( g_GUIMgr.GetPreClickWnd() )
			{
				if( g_GUIMgr.GetPreClickWnd() == this || g_GUIMgr.GetPreClickWnd()->GetParent() == this || 
					g_GUIMgr.GetPreClickWnd() == pScroll || g_GUIMgr.GetPreClickWnd()->GetParent() == pScroll )
				{
					bScrollShow = true;
				}
			}
		}

		if( pScroll->IsShow() )
		{
			if( !bScrollShow )
			{
				pScroll->HideWnd();
			}
		}
		else
		{
			if( bScrollShow )
			{
				pScroll->ShowWnd();
			}
		}		
	}
	ioMovingWnd::OnProcess( fTimePerSec );

	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ) )
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			int iYPos = GetYPos();
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ) )
				m_iDefaultY = iYPos;
		}
	}
}

void KOChatMsgWnd::OnRender()
{
	ioWnd::OnRender();

	if( m_ItemList.empty() )
		return;

	int iXPos = GetDerivedPosX() + m_iStartXPos;
	int iYPos = GetDerivedPosY() + m_iStartYPos + ( m_iLineYOffset * ( m_iMaxChatViewCnt - 1 ) );

	int iMaxSize  = (int)m_ItemList.size();
	int iStartPos = ( max( 0, iMaxSize - m_iMaxChatViewCnt ) ) - m_iCurPage;
	for(int iCount = 0;iCount < m_iMaxChatViewCnt;iStartPos++,iCount++)
	{
		if( !COMPARE( iStartPos, 0, iMaxSize ) ) break;

		KOMsgItem &rkItem = m_ItemList[iStartPos];
		DWORD dwGapTime = FRAMEGETTIME() - rkItem.m_dwCurrentTime;
		
		if( dwGapTime > m_dwAlphaStartTime + m_dwAlphaTime ) continue;

		float fAlphaRate = FLOAT1;
		if( dwGapTime > m_dwAlphaStartTime && m_dwAlphaTime > 0 )
		{
			fAlphaRate = max( 0.0f, FLOAT1 - (float)(dwGapTime - m_dwAlphaStartTime) / m_dwAlphaTime );
		}
		BYTE iAlphaRate = (float)MAX_ALPHA_RATE * fAlphaRate;

		if( rkItem.m_szKillerName.IsEmpty() )
		{
			// 자살~
			if( m_pIconBack && fAlphaRate == FLOAT1 )
			{
				m_pIconBack->SetScale( 0.34f );
				m_pIconBack->Render( iXPos + 11, iYPos + 8, UI_RENDER_MULTIPLY );
				m_pIconBack->Render( iXPos + 11, iYPos + 8, UI_RENDER_MULTIPLY );
			}

			if( m_pNoneKillerIcon )
			{
				m_pNoneKillerIcon->SetScale( 0.33f );
				m_pNoneKillerIcon->SetAlpha( iAlphaRate );
				m_pNoneKillerIcon->Render( iXPos + 11, iYPos + 8 );
			}
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			if( rkItem.m_eDierTeam == TEAM_BLUE )
				g_FontMgr.SetTextColor( 0, 150, 255 );
			else
				g_FontMgr.SetTextColor( 255, 60, 0 );
	
			g_FontMgr.PrintTextAlpha( iXPos + 25, iYPos, FONT_SIZE_12, iAlphaRate, STR(1), rkItem.m_szDierName.c_str(), rkItem.m_szResultText.c_str() );
		}
		else
		{
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			if( rkItem.m_eKillerTeam == TEAM_BLUE )
				g_FontMgr.SetTextColor( 0, 150, 255 );
			else
				g_FontMgr.SetTextColor( 255, 60, 0 );

			g_FontMgr.PrintTextAlpha( iXPos, iYPos, FONT_SIZE_12, iAlphaRate, STR(2), rkItem.m_szKillerName.c_str() );

			int iFirstTextSize = g_FontMgr.GetTextWidth( rkItem.m_szKillerName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_12 );
			if( m_pIconBack && fAlphaRate == FLOAT1 )
			{
				m_pIconBack->SetScale( 0.34f );
				m_pIconBack->Render( iXPos + iFirstTextSize + 14, iYPos + 8, UI_RENDER_MULTIPLY );
				m_pIconBack->Render( iXPos + iFirstTextSize + 14, iYPos + 8, UI_RENDER_MULTIPLY );
			}
			
			if( !rkItem.m_pSkillIcon && m_pNoneSkillIcon )
			{
				m_pNoneSkillIcon->SetScale( 0.33f );
				m_pNoneSkillIcon->SetAlpha( iAlphaRate );
				m_pNoneSkillIcon->Render( iXPos + iFirstTextSize + 14, iYPos + 8 );
			}
			else if( rkItem.m_pSkillIcon )
			{
				rkItem.m_pSkillIcon->SetScale( 0.33f );
				rkItem.m_pSkillIcon->SetAlpha( iAlphaRate );
				rkItem.m_pSkillIcon->Render( iXPos + iFirstTextSize + 14, iYPos + 8 );
			}

			if( rkItem.m_eDierTeam == TEAM_BLUE )
				g_FontMgr.SetTextColor( 0, 150, 255 );
			else
				g_FontMgr.SetTextColor( 255, 60, 0 );

			g_FontMgr.PrintTextAlpha( iXPos + iFirstTextSize + 28, iYPos, FONT_SIZE_12, iAlphaRate, STR(3), rkItem.m_szDierName.c_str(), rkItem.m_szResultText.c_str() );
		}
		iYPos -= m_iLineYOffset;
	}
}

void KOChatMsgWnd::iwm_show()
{

}

void KOChatMsgWnd::iwm_hide()
{

}

void KOChatMsgWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
		if( pScroll == NULL ) return;
		if( pScroll->HasWndStyle( IWS_INACTIVE ) ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPage - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPage + 1 );
	}
}

void KOChatMsgWnd::iwm_vscroll(DWORD dwID, int curPos )
{
	m_iCurPage = curPos;

	if( m_bScrollAlphaInit )
	{
		for(int i = 0;i < (int)m_ItemList.size();i++)
		{
			KOMsgItem &rkItem = m_ItemList[i];
			rkItem.m_dwCurrentTime = FRAMEGETTIME();
		}
	}
}

void KOChatMsgWnd::SetKOMsg( TeamType eKillerTeam, const ioHashString &szKillerName, const ioHashString &szKillerSkillName, DWORD dwKillerItem, 
							 TeamType eDierTeam, const ioHashString &szDierName )
{
	KOMsgItem kData;
	kData.m_iMsgType = KT_KILL;
	kData.m_eKillerTeam = eKillerTeam;
	kData.m_szKillerName = szKillerName;
	kData.m_eDierTeam = eDierTeam;
	kData.m_szDierName = szDierName;
	kData.m_szKillerSkill = szKillerSkillName;
	kData.m_dwKillerWeaponCode = dwKillerItem;
	kData.m_dwCurrentTime = FRAMEGETTIME();
	if( !szKillerSkillName.IsEmpty() )
	{
		const ioSkill *pSkill = g_SkillMaker.GetSkillConst( szKillerSkillName );
		if( pSkill )
		{
			kData.m_pSkillIcon = g_UIImageSetMgr.CreateImageByFullName( pSkill->GetIconName() );
		}
	}
	else if( kData.m_dwKillerWeaponCode != 0 )
	{
		const ioItem *pItem = g_ItemMaker.GetItemConst( kData.m_dwKillerWeaponCode, __FUNCTION__ );
		if( pItem )
		{
			kData.m_pSkillIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetItemLargeIconName() );
		}
	}

	//kData.m_szResultText = "KO";
	m_ItemList.insert( m_ItemList.begin(), kData );
	m_bScrollAlphaInit = false;
	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		m_iCurPage = m_ItemList.size();
		pScroll->SetScrollRange( 0, m_ItemList.size() );
		pScroll->SetScrollPos( m_iCurPage );
	}
	m_bScrollAlphaInit = true;

	if( !m_szMsgSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szMsgSnd );
}

void KOChatMsgWnd::SetPrisonerMsg( TeamType eKillerTeam, const ioHashString &szKillerName, const ioHashString &szKillerSkillName, DWORD dwKillerItem,
								   TeamType eDierTeam, const ioHashString &szDierName )
{
	KOMsgItem kData;
	kData.m_iMsgType = KT_PRISONER;
	kData.m_eKillerTeam = eKillerTeam;
	kData.m_szKillerName = szKillerName;
	kData.m_eDierTeam = eDierTeam;
	kData.m_szDierName = szDierName;
	kData.m_szKillerSkill = szKillerSkillName;
	kData.m_dwKillerWeaponCode = dwKillerItem;
	kData.m_dwCurrentTime = FRAMEGETTIME();
	if( !szKillerSkillName.IsEmpty() )
	{
		const ioSkill *pSkill = g_SkillMaker.GetSkillConst( szKillerSkillName );
		if( pSkill )
		{
			kData.m_pSkillIcon = g_UIImageSetMgr.CreateImageByFullName( pSkill->GetIconName() );
		}
	}
	else if( kData.m_dwKillerWeaponCode != 0 )
	{
		const ioItem *pItem = g_ItemMaker.GetItemConst( kData.m_dwKillerWeaponCode, __FUNCTION__ );
		if( pItem )
		{
			kData.m_pSkillIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetItemLargeIconName() );
		}
	}

	kData.m_szResultText = STR(1);
	m_ItemList.insert( m_ItemList.begin(), kData );
	m_bScrollAlphaInit = false;
	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		m_iCurPage = m_ItemList.size();
		pScroll->SetScrollRange( 0, m_ItemList.size() );
		pScroll->SetScrollPos( m_iCurPage );
	}
	m_bScrollAlphaInit = true;

	if( !m_szMsgSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szMsgSnd );
}

void KOChatMsgWnd::SetEscapeMsg( TeamType eKillerTeam, const ioHashString &szKillerName, const ioHashString &szKillerSkillName, DWORD dwKillerItem,
							     TeamType eDierTeam, const ioHashString &szDierName )
{
	KOMsgItem kData;
	kData.m_iMsgType = KT_ESCAPE;
	kData.m_eKillerTeam = eKillerTeam;
	kData.m_szKillerName = szKillerName;
	kData.m_eDierTeam = eDierTeam;
	kData.m_szDierName = szDierName;
	kData.m_szKillerSkill = szKillerSkillName;
	kData.m_dwKillerWeaponCode = dwKillerItem;
	kData.m_dwCurrentTime = FRAMEGETTIME();
	if( !szKillerSkillName.IsEmpty() )
	{
		const ioSkill *pSkill = g_SkillMaker.GetSkillConst( szKillerSkillName );
		if( pSkill )
		{
			kData.m_pSkillIcon = g_UIImageSetMgr.CreateImageByFullName( pSkill->GetIconName() );
		}
	}
	else if( kData.m_dwKillerWeaponCode != 0 )
	{
		const ioItem *pItem = g_ItemMaker.GetItemConst( kData.m_dwKillerWeaponCode, __FUNCTION__ );
		if( pItem )
		{
			kData.m_pSkillIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetItemLargeIconName() );
		}
	}

	kData.m_szResultText = STR(1);
	m_ItemList.insert( m_ItemList.begin(), kData );
	m_bScrollAlphaInit = false;
	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		m_iCurPage = m_ItemList.size();
		pScroll->SetScrollRange( 0, m_ItemList.size() );
		pScroll->SetScrollPos( m_iCurPage );
	}
	m_bScrollAlphaInit = true;

	if( !m_szMsgSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szMsgSnd );
}

void KOChatMsgWnd::SetInfectionMsg( TeamType eKillerTeam, const ioHashString &szKillerName, const ioHashString &szKillerSkillName, DWORD dwKillerItem, 
									TeamType eDierTeam, const ioHashString &szDierName )
{
	KOMsgItem kData;
	kData.m_iMsgType = KT_INFECTION;
	kData.m_eKillerTeam = eKillerTeam;
	kData.m_szKillerName = szKillerName;
	kData.m_eDierTeam = eDierTeam;
	kData.m_szDierName = szDierName;
	kData.m_szKillerSkill = szKillerSkillName;
	kData.m_dwKillerWeaponCode = dwKillerItem;
	kData.m_dwCurrentTime = FRAMEGETTIME();
	if( !szKillerSkillName.IsEmpty() )
	{
		const ioSkill *pSkill = g_SkillMaker.GetSkillConst( szKillerSkillName );
		if( pSkill )
		{
			kData.m_pSkillIcon = g_UIImageSetMgr.CreateImageByFullName( pSkill->GetIconName() );
		}
	}
	else if( kData.m_dwKillerWeaponCode != 0 )
	{
		const ioItem *pItem = g_ItemMaker.GetItemConst( kData.m_dwKillerWeaponCode, __FUNCTION__ );
		if( pItem )
		{
			kData.m_pSkillIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetItemLargeIconName() );
		}
	}

	kData.m_szResultText = STR(1);
	m_ItemList.insert( m_ItemList.begin(), kData );
	m_bScrollAlphaInit = false;
	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		m_iCurPage = m_ItemList.size();
		pScroll->SetScrollRange( 0, m_ItemList.size() );
		pScroll->SetScrollPos( m_iCurPage );
	}
	m_bScrollAlphaInit = true;

	if( !m_szMsgSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szMsgSnd );
}