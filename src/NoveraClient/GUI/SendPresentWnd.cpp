#include "StdAfx.h"

#include "../ioUserSpiritManager.h"
#include "../ioShopManager.h"

#include "sendpresentwnd.h"

SendPresentIdWnd::SendPresentIdWnd()
{
	m_pReturnWnd     = NULL;
	m_iPresentType   = 0;
	m_iPresentValue1 = 0;
	m_iPresentValue2 = 0;
}

SendPresentIdWnd::~SendPresentIdWnd()
{

}

void SendPresentIdWnd::ShowSendPresentIdWnd( NewShopBuyWnd *pReturnWnd, short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	if( !pReturnWnd )
	{
		LOG.PrintTimeAndLog( 0, "%s pReturnWnd == NULL. ", __FUNCTION__ );
		return;
	}

	m_pReturnWnd     = pReturnWnd;
	m_iPresentType   = iPresentType;
	m_iPresentValue1 = iPresentValue1;
	m_iPresentValue2 = iPresentValue2;

	m_szFriend.Clear();
	SetChildWndStyleAdd( ID_NEXT, IWS_EXACTIVE );
	
	ioWnd *pFriendListBtn = FindChildWnd( ID_FRIEND_BTN );
	if( pFriendListBtn )
		pFriendListBtn->SetTitleText( STR(1) );

	ShowWnd();
}

void SendPresentIdWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_FRIEND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ioWnd *pFriendListWnd = FindChildWnd( ID_FRIEND_LIST );
			if( pFriendListWnd )
			{
				if( pFriendListWnd->IsShow() )
					pFriendListWnd->HideWnd();
				else
					pFriendListWnd->ShowWnd();
			}
		}
		break;
	case ID_NEXT:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( ShowConfirmWnd() )
				HideWnd();
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
		}
		break;
	case ID_FRIEND_LIST:
		if( cmd == IOBN_BTNUP )
		{
			SendPresentFriendListWnd *pFriendWnd = dynamic_cast<SendPresentFriendListWnd*>( pWnd );
			if( pFriendWnd )
			{
				m_szFriend = pFriendWnd->GetPushUser().c_str();
				if( !m_szFriend.IsEmpty() )
				{
					SetChildWndStyleRemove( ID_NEXT, IWS_EXACTIVE );

					ioWnd *pFriendListBtn = FindChildWnd( ID_FRIEND_BTN );
					if( pFriendListBtn )
						pFriendListBtn->SetTitleText( m_szFriend.c_str() );
				}
				else
				{
					SetChildWndStyleAdd( ID_NEXT, IWS_EXACTIVE );

					ioWnd *pFriendListBtn = FindChildWnd( ID_FRIEND_BTN );
					if( pFriendListBtn )
						pFriendListBtn->SetTitleText( STR(2) );
				}
			}
		}
		break;
	}
}

bool SendPresentIdWnd::ShowConfirmWnd()
{
	if( m_szFriend.IsEmpty() )
		return false;

	if( !g_App.IsRightID( m_szFriend.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	if( stricmp( g_MyInfo.GetPublicID().c_str(), m_szFriend.c_str() ) == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return false;
	}

	if( g_App.IsAdminID( m_szFriend.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return false;
	}

	SendPresentConfirmWnd *pConfirmWnd = dynamic_cast< SendPresentConfirmWnd* > ( g_GUIMgr.FindWnd( SEND_PRESENT_CONFIRM_WND ) );
	if( !pConfirmWnd )
	{
		LOG.PrintTimeAndLog( 0, "%s pConfirmWnd == NULL. ", __FUNCTION__ );
		return false;
	}

	pConfirmWnd->ShowSendPresentConfirmWnd( m_pReturnWnd, m_iPresentType, m_iPresentValue1, m_iPresentValue2, m_szFriend.c_str() );

	return true;
}

void SendPresentIdWnd::iwm_show()
{
	SetChildWndStyleAdd( ID_NEXT, IWS_EXACTIVE );
}

void SendPresentIdWnd::iwm_hide()
{
}

void SendPresentIdWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, STR(1) );
}
//-------------------------------------------------
SendPresentFriendListWnd::SendPresentFriendListWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;

	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iStartXPos= 0;
	m_iStartYPos= 0;
	m_iHeightGap= 0;
	m_iCutWidth = 0;
	m_iCurPos   = 0;
}

SendPresentFriendListWnd::~SendPresentFriendListWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void SendPresentFriendListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
}

void SendPresentFriendListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void SendPresentFriendListWnd::iwm_show()
{
	m_szOverListUser.Clear();
	m_szPushListUser.Clear();
	m_FriendUserList.clear();
	for(int i = 0;i < g_FriendManager.GetNodeSize();i++)
	{
		FriendNode *pNode = g_FriendManager.GetNode( i );
		if( !pNode ) continue;

		m_FriendUserList.push_back( pNode->GetName() );
	}			

	if( m_FriendUserList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	m_iCurPos = 0;
	if( (int)m_FriendUserList.size() <= MAX_PAGE_LIST )
	{
		HideChildWnd( ID_VERT_SCROLL );
	}
	else
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, m_FriendUserList.size() );
			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}

	// 윈도우 사이즈 결정	
	SetSize( GetWidth(), ( min( MAX_PAGE_LIST, m_FriendUserList.size() ) * m_iHeightGap) + m_iAddHeight );
	SetCheckParentOutSide( true );
}

void SendPresentFriendListWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void SendPresentFriendListWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( !pScroll->IsShow() ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void SendPresentFriendListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void SendPresentFriendListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_szOverListUser.Clear();
	m_szPushListUser.Clear();

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != GetParent()->FindChildWnd( SendPresentIdWnd::ID_FRIEND_BTN ) &&
			pPreClick != FindChildWnd( ID_VERT_SCROLL ) &&
			pPreClick->GetParent() != FindChildWnd( ID_VERT_SCROLL ) )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_FriendUserList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_FriendUserList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( ( GetWidth() - m_iAddWidth ) - iCutWidth );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_szOverListUser  = m_FriendUserList[iListPos];
			if( pMouse->IsLBtnUp() )
			{
				m_szPushListUser = m_FriendUserList[iListPos];
				if( GetParent() )
					GetParent()->iwm_command( this, IOBN_BTNUP, 0 );
				HideWnd();
			}
			break;
		}
	}
}

void SendPresentFriendListWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_FriendUserList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_FriendUserList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		bDarkRender = !bDarkRender;
		if( m_szOverListUser == m_FriendUserList[iListPos] )
		{
			m_pOverLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintTextWidthCut( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, 244.0f - iCutWidth, 
									 m_FriendUserList[iListPos].c_str() );
	}
}
//----------------------------------------------------------------------------------------------------------------------------------
SendPresentConfirmWnd::SendPresentConfirmWnd()
{
	m_pPresentIcon    = NULL;
	m_pSubIcon = NULL;
	m_pCostumeMark = NULL;

	m_pReturnWnd      = NULL;

	m_iPresentType    = 0;
	m_iPresentValue1  = 0;
	m_iPresentValue2  = 0;

	m_iBonusPeso      = 0;
	m_iCash           = 0;

	m_bIsUnableBonus = false;
	m_bIsEnableBonus = false;
}

SendPresentConfirmWnd::~SendPresentConfirmWnd()
{
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pCostumeMark );
}

void SendPresentConfirmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void SendPresentConfirmWnd::ShowSendPresentConfirmWnd( NewShopBuyWnd *pReturnWnd, short iPresentType, int iPresentValue1, int iPresentValue2, const char *szRecvPublicID )
{
	if( !pReturnWnd )
	{
		LOG.PrintTimeAndLog( 0, "%s pReturnWnd == NULL. ", __FUNCTION__ );
		return;
	}

	m_pReturnWnd     = pReturnWnd;
	m_iPresentType   = iPresentType;
	m_iPresentValue1 = iPresentValue1;
	m_iPresentValue2 = iPresentValue2;
	m_szRecvPublicID = szRecvPublicID;

	m_iBonusPeso     = GetBonusPeso( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );
	m_iCash          = GetCash( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );
	m_bIsUnableBonus = g_ShopManager.GetUnablePresentBonus( m_iPresentType, m_iPresentValue1 );
	m_bIsEnableBonus = g_ShopManager.GetEnablePresentBonus( m_iPresentType, m_iPresentValue1 );

	SetPresentIcon();

	ShowWnd();
}

void SendPresentConfirmWnd::iwm_show()
{
	ShowChildWnd( ID_CLAUSE_VIEW_BTN );
	SetSize( GetWidth(), NORMAL_HEIGHT );

	/*
	if( g_MyInfo.GetChannelingType() == CNT_MGAME )
	{
		// 엠게임은 청약철회 규정보기 버튼 활성화
		ShowChildWnd( ID_CLAUSE_VIEW_BTN );

		SetSize( GetWidth(), MGAME_HEIGHT );
	}
	else
	{
		HideChildWnd( ID_CLAUSE_VIEW_BTN );

		SetSize( GetWidth(), NORMAL_HEIGHT );
	}
	*/

	// 하단바
	if( m_pMultiplyFrm )
	{
		m_pMultiplyFrm->SetSize( GetWidth(), 65 );
		m_pMultiplyFrm->SetPosition( 0, GetHeight() - m_pMultiplyFrm->GetHeight() );
	}

	// 하단 버튼
	ioWnd *pOkBtn = FindChildWnd( ID_OK_BTN );
	if( pOkBtn )
	{
		pOkBtn->SetWndPos( pOkBtn->GetXPos(), GetHeight() - 48 );
	}

	ioWnd *pCancelBtn = FindChildWnd( ID_CANCEL_BTN );
	if( pCancelBtn )
	{
		pCancelBtn->SetWndPos( pCancelBtn->GetXPos(), GetHeight() - 48 );
	}
}

void SendPresentConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT_BTN:
	case ID_CANCEL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_OK_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendBuyFromReturnWnd();
			HideWnd();
		}
		break;
	case ID_CLAUSE_VIEW_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.ShowWnd( CLAUSE_VIEW_WND );
		}
		break;
	}
}

void SendPresentConfirmWnd::SendBuyFromReturnWnd()
{
	if( !m_pReturnWnd )
	{
		LOG.PrintTimeAndLog( 0, "%s pReturnWnd == NULL. ", __FUNCTION__ );
		return;
	}

	m_pReturnWnd->SendBuy();
}

void SendPresentConfirmWnd::SendPresentComplete()
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, m_szRecvPublicID.c_str() );
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );

	g_GUIMgr.SetPrevMsgListPinkBoxWithTitle( NULL, MB_OK, NULL, kPrinter, STR(3), STR(4), STR(5), STR(6) );
}

void SendPresentConfirmWnd::SetPresentIcon()
{
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	ioHashString szIconName = g_PresentMgr.GetPresentIconText(m_iPresentType, m_iPresentValue1, m_iPresentValue2);	
	m_pPresentIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ) );
}

int SendPresentConfirmWnd::GetBonusPeso( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	int iBonusPeso = 0;
	if( iPresentType == PRESENT_SOLDIER )
	{
		iBonusPeso = g_ClassPrice.GetBonusPeso( iPresentValue1 );
	}
	else if( iPresentType == PRESENT_DECORATION )
	{
		int iSexType   = ( iPresentValue1 % 100000) / 1000;
		int iDecoType  = iPresentValue1 % 1000;
		int iDecoCode  = iPresentValue2;

		const DecoData *pData = g_DecorationPrice.GetDecoDataPtr( iSexType, iDecoType, iDecoCode );
		if( pData )
			iBonusPeso = pData->m_iBonusPeso;
	}
	else if( iPresentType == PRESENT_ETC_ITEM )
	{
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iPresentValue1 );
		if( pEtcItem )
		{
			int iSize = pEtcItem->GetValueSize();
			for (int i = 0; i < iSize ; i++)
			{
				if( iPresentValue2 == pEtcItem->GetValue( i ) )
				{
					iBonusPeso = pEtcItem->GetBonusPeso( i );
					break;
				}
			}
		}
	}
	return iBonusPeso;
}

int SendPresentConfirmWnd::GetCash( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	int iCash = 0;
	if( iPresentType == PRESENT_SOLDIER )
	{
		if( iPresentValue2 == 0 )
			iCash = g_ClassPrice.GetMortmainCharCash( iPresentValue1 );
		else 
			iCash = g_ClassPrice.GetClassBuyCash( iPresentValue1, iPresentValue2 );
	}
	else if( iPresentType == PRESENT_DECORATION )
	{
		int iSexType   = ( iPresentValue1 % 100000) / 1000;
		int iDecoType  = iPresentValue1 % 1000;
		int iDecoCode  = iPresentValue2;

		const DecoData *pData = g_DecorationPrice.GetDecoDataPtr( iSexType, iDecoType, iDecoCode );
		if( pData )
			iCash = pData->m_iCash;
	}
	else if( iPresentType == PRESENT_ETC_ITEM )
	{
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iPresentValue1 );
		if( pEtcItem )
		{
			int iSize = pEtcItem->GetValueSize();
			for (int i = 0; i < iSize; i++)
			{
				if( iPresentValue2 == pEtcItem->GetValue( i ) )
				{
					iCash = pEtcItem->GetCash( i );
					break;
				}
			}
		}
	}
	else if( iPresentType == PRESENT_EXTRAITEM_BOX )
	{
		RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iPresentValue1 );
		if( pInfo )
			iCash = pInfo->GetNeedCash(); 
	}
	else if( iPresentType == PRESENT_COSTUME_BOX )
	{
		CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( iPresentValue1 );
		if( pInfo )
			iCash = pInfo->GetNeedCash(); 
	}
	return iCash;
}

void SendPresentConfirmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pPresentIcon )
		m_pPresentIcon->Render( iXPos + 125, iYPos + 98 );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + 125, iYPos + 98 );
	// UI코스튬 관련 (코스튬 마크)
	if ( ( m_iPresentType == PRESENT_COSTUME || m_iPresentType == PRESENT_COSTUME_BOX ) && m_pCostumeMark )
		m_pCostumeMark->Render( iXPos + 125, iYPos + 98 );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 125, iYPos + 144, FONT_SIZE_13, 216.0f, g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
	g_FontMgr.PrintTextWidthCut( iXPos + 125, iYPos + 164, FONT_SIZE_13, 216.0f, g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	if( g_EventMgr.IsPresentBonus(EVT_PRESENT_BUY) && g_ShopManager.IsUnablePresentBonus() && m_bIsUnableBonus )
	{
		g_FontMgr.PrintText( iXPos + 125, iYPos + 234, FONT_SIZE_13, STR(11) );
		g_FontMgr.PrintText( iXPos + 125, iYPos + 254, FONT_SIZE_13, STR(12) );
	}
	else if( g_EventMgr.IsPresentBonus(EVT_PRESENT_BUY_2) && g_ShopManager.IsEnablePresentBonus() && !m_bIsEnableBonus )
	{
		g_FontMgr.PrintText( iXPos + 125, iYPos + 234, FONT_SIZE_13, STR(11) );
		g_FontMgr.PrintText( iXPos + 125, iYPos + 254, FONT_SIZE_13, STR(12) );
	}
	else
	{
		g_FontMgr.PrintText( iXPos + 125, iYPos + 234, FONT_SIZE_13, STR(1) );
		g_FontMgr.PrintText( iXPos + 125, iYPos + 254, FONT_SIZE_13, STR(2) );
	}

	g_FontMgr.PrintText( iXPos + 125, iYPos + 284, FONT_SIZE_13, STR(8) );
	g_FontMgr.PrintText( iXPos + 125, iYPos + 304, FONT_SIZE_13, STR(9) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, m_szRecvPublicID.c_str() );
	kPrinter.PrintFullTextWidthCut( iXPos + 125, iYPos + 184, TAT_CENTER, 216.0f );
	kPrinter.ClearList();

	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );

	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_iCash, szConvertNum, sizeof( szConvertNum ) );
	kPrinter.SetTextColor( 141, 87, 4 );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(7), szConvertNum );
	kPrinter.PrintFullTextWidthCut( iXPos + 125, iYPos + 204, TAT_CENTER, 216.0f );
	kPrinter.ClearList();
}