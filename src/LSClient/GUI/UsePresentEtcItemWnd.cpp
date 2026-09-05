#include "StdAfx.h"

#include "../ioComplexStringPrinter.h"
#include "../ioEtcItem.h"

#include "UsePresentEtcItemWnd.h"

#include <strsafe.h>

UsePresentEtcItemWnd::UsePresentEtcItemWnd()
{
	m_pEmptyIcon = NULL;
	m_pPresentIcon = NULL;
	m_pSubIcon = NULL;
	m_pStripGrayDark = NULL;
	m_vItemIcon.clear();
	m_vSubIcon.clear();

	m_iSelectedPresent = -1;
	m_SelectedFriendName.Clear();
}

UsePresentEtcItemWnd::~UsePresentEtcItemWnd()
{
	int iCnt = m_vItemIcon.size();
	for( int i=0; i<iCnt; ++i )
	{
		SAFEDELETE( m_vItemIcon[i] );
	}
	m_vItemIcon.clear();

	iCnt = m_vSubIcon.size();
	for( int i=0; i<iCnt; ++i )
	{
		SAFEDELETE( m_vSubIcon[i] );
	}
	m_vSubIcon.clear();

	SAFEDELETE( m_pEmptyIcon );
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pStripGrayDark );
}

void UsePresentEtcItemWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "EmptyIcon" )
	{
		SAFEDELETE( m_pEmptyIcon );
		m_pEmptyIcon = pImage;
	}
	else if( szType == "StripGrayDark" )
	{
		SAFEDELETE( m_pStripGrayDark );
		m_pStripGrayDark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void UsePresentEtcItemWnd::SendPresent()
{
	if( g_App.IsMouseBusy() )
		return;

	FriendNode *pNode = g_FriendManager.GetNode( m_SelectedFriendName );
	if( pNode )
	{
		SP2Packet kPacket( CTPK_ETCITEM_USE );
		kPacket << static_cast<int>( m_dwEtcItemIndex );
		kPacket << pNode->GetUserIndex();
		kPacket << pNode->GetName();
		kPacket << m_iSelectedPresent;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );

		return;
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "친구에게만 보낼 수 있습니다." );
		return;
	}

	return;
}

void UsePresentEtcItemWnd::SendPresentComplete()
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, m_SelectedFriendName.c_str() );
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "님께" );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "선물 발송을 완료하였습니다." );

	g_GUIMgr.SetPrevMsgListPinkBoxWithTitle( NULL, MB_OK, NULL, kPrinter, "알림", "확인", "", "" );
}

void UsePresentEtcItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_SEND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetConfirmWnd();
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
	case ID_PRESENT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			PresentListWnd *pPresentListWnd = dynamic_cast<PresentListWnd*>( FindChildWnd( ID_PRESENT_LIST ) );
			if( pPresentListWnd )
			{
				if( pPresentListWnd->IsShow() )
					pPresentListWnd->HideWnd();
				else
					pPresentListWnd->SetPresentList( m_vPresentList );
			}
		}
		break;
	case ID_FRIEND_LIST:
		if( cmd == IOBN_BTNUP )
		{
			FriendListWnd *pFriendWnd = dynamic_cast<FriendListWnd*>( pWnd );
			if( pFriendWnd )
			{
				g_InputBox.SetString( pFriendWnd->GetOverUser().c_str() );
			}
		}
		break;
	}
}

void UsePresentEtcItemWnd::iwm_show()
{
}

void UsePresentEtcItemWnd::iwm_hide()
{
}

bool UsePresentEtcItemWnd::iwm_esc()
{
	ioWnd *pFriendListWnd = FindChildWnd( ID_FRIEND_LIST );
	if( pFriendListWnd && pFriendListWnd->IsShow() )
	{
		pFriendListWnd->HideWnd();
		return true;
	}
	return ioWnd::iwm_esc();
}

void UsePresentEtcItemWnd::SetInfo( DWORD dwEtcItemIdx, const ioEtcItemSendPresent::vPresentData &PresentList )
{
	m_dwEtcItemIndex = dwEtcItemIdx;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( dwEtcItemIdx );
	if( !pItem )
		return;

	if( !IsShow() )
		ShowWnd();

	m_SelectedFriendName.Clear();
	m_iSelectedPresent = -1;

	ioButton *pPresentBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_PRESENT_BTN ) );
	if( pPresentBtn )
		pPresentBtn->SetTitleText( "선물선택" );

	ioButton *pFriendBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_FRIEND_BTN ) );
	if( pFriendBtn )
		pFriendBtn->SetTitleText( "선물받을 친구를 선택하세요." );

	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	m_pPresentIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() );
	m_PresentItemName = pItem->GetName();

	if( !m_vItemIcon.empty() )
	{
		int iCnt = m_vItemIcon.size();
		for( int i=0; i<iCnt; ++i )
		{
			SAFEDELETE( m_vItemIcon[i] );
		}
	}
	if( !m_vSubIcon.empty() )
	{
		int iCnt = m_vSubIcon.size();
		for( int i=0; i<iCnt; ++i )
		{
			SAFEDELETE( m_vSubIcon[i] );
		}
	}
	int iCnt = PresentList.size();

	m_vPresentList.clear();
	m_vPresentList.reserve( iCnt );
	m_vItemIcon.clear();
	m_vItemIcon.reserve( iCnt );
	
	for( int i=0; i<iCnt; ++i )
	{
		m_vPresentList.push_back( PresentList[i] );

		ioHashString szIconName = g_PresentMgr.GetPresentIconText( PresentList[i].m_iPresentType, PresentList[i].m_iPresentValue1, PresentList[i].m_iPresentValue2 );
		ioUIRenderImage *pImage = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		m_vItemIcon.push_back( pImage );

		ioHashString szSubIconName = g_PresentMgr.GetPresentSubIconText( PresentList[i].m_iPresentType, PresentList[i].m_iPresentValue1, PresentList[i].m_iPresentValue2 );
		ioUIRenderImage *pSubImage = g_UIImageSetMgr.CreateImageByFullName( szSubIconName );
		m_vSubIcon.push_back( pSubImage );
	}
}

void UsePresentEtcItemWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "%s", m_PresentItemName.c_str() );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, " 아이템이" );
	kPrinter.PrintFullTextWidthCut( iXPos + 124, iYPos + 51, TAT_CENTER, 218 );
	kPrinter.ClearList();

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, " 아래와 같은 선물로 변경됩니다" );
	kPrinter.PrintFullTextWidthCut( iXPos + 124, iYPos + 69, TAT_CENTER, 218 );
	kPrinter.ClearList();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 124, iYPos + 320, FONT_SIZE_13, "친구를 선택하신 후 다음을 클릭하세요" );

	if( m_pStripGrayDark )
	{
		for( int i=0; i<43; ++i )
		{
			m_pStripGrayDark->SetSize( 3, 2 );
			m_pStripGrayDark->Render( iXPos + 17 + ( i * 5 ), iYPos + 309, UI_RENDER_MULTIPLY );
		}
	}

	if( m_pPresentIcon )
		m_pPresentIcon->Render( iXPos + 124, iYPos + 131 );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + 124, iYPos + 131 );

	if( m_iSelectedPresent == -1 || m_iSelectedPresent >= (int)m_vPresentList.size() )
	{
		if( m_pEmptyIcon )
			m_pEmptyIcon->Render( iXPos + 124, iYPos + 231, UI_RENDER_GRAY );
	}
	else if( m_vItemIcon[m_iSelectedPresent] )
	{
		m_vItemIcon[m_iSelectedPresent]->Render( iXPos + 124, iYPos + 231, UI_RENDER_NORMAL );
		if( m_vSubIcon[m_iSelectedPresent] )
			m_vSubIcon[m_iSelectedPresent]->Render( iXPos + 124, iYPos + 231, UI_RENDER_NORMAL );
	}
}

void UsePresentEtcItemWnd::SetConfirmWnd()
{
	if( m_iSelectedPresent == -1 || m_iSelectedPresent >= (int)m_vPresentList.size() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "먼저 보낼 선물을 선택하세요." );
		return;
	}

	if( m_SelectedFriendName.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "닉네임을 입력하세요." );
		return;
	}

	if( !g_App.IsRightID( m_SelectedFriendName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 닉네임입니다." );
		return;
	}

	if( m_SelectedFriendName == g_MyInfo.GetPublicID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "자신의 닉네임입니다." );
		return;
	}

	if( g_App.IsAdminID( m_SelectedFriendName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "개발자나 마스터에게는 보낼 수 없습니다." );
		return;
	}

	SendPresentEtcItemConfirmWnd *pWnd = dynamic_cast<SendPresentEtcItemConfirmWnd*>( g_GUIMgr.FindWnd( USE_PRESENT_ETC_ITEM_CONFIRM_WND ) );
	if( pWnd )
		pWnd->SetInfo( this, m_SelectedFriendName, m_vPresentList[m_iSelectedPresent] );
}

//////////////////////////////////////////////////////////////////////////
FriendListWnd::FriendListWnd()
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

FriendListWnd::~FriendListWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void FriendListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
}

void FriendListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void FriendListWnd::iwm_show()
{
	m_szOverListUser.Clear();
	m_FriendList.clear();
	for(int i = 0;i < g_FriendManager.GetNodeSize();i++)
	{
		FriendNode *pNode = g_FriendManager.GetNode( i );
		if( !pNode ) continue;
		m_FriendList.push_back( pNode->GetName() );
	}			

	if( m_FriendList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "친구가 없습니다.." );
		HideWnd();
		return;
	}

	m_iCurPos = 0;
	if( (int)m_FriendList.size() <= MAX_PAGE_LIST )
	{
		HideChildWnd( ID_VERT_SCROLL );
	}
	else
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, m_FriendList.size() );
			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}

	// 윈도우 사이즈 결정	
	SetSize( GetWidth(), ( min( MAX_PAGE_LIST, m_FriendList.size() ) * m_iHeightGap) + m_iAddHeight );
	SetCheckParentOutSide( true );
	
	if( GetParent() )
	{
		ioWnd *pBtn = GetParent()->FindChildWnd( UsePresentEtcItemWnd::ID_FRIEND_BTN );
		if( pBtn )
		{
			int iCnt = min( MAX_PAGE_LIST, m_FriendList.size() );
			int iYOffset = m_iHeightGap * ( iCnt - 1 );
			SetWndPos( pBtn->GetXPos(), pBtn->GetYPos()- iYOffset - 37 );
		}
	}
}

void FriendListWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void FriendListWnd::iwm_wheel( int zDelta )
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

void FriendListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void FriendListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_szOverListUser.Clear();

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != GetParent()->FindChildWnd( UsePresentEtcItemWnd::ID_FRIEND_BTN ) &&
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
	if( (int)m_FriendList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_FriendList.size();
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
			m_szOverListUser  = m_FriendList[iListPos];
			if( pMouse->IsLBtnUp() )
			{
				UsePresentEtcItemWnd *pParent = dynamic_cast<UsePresentEtcItemWnd*>( GetParent() );
				if( pParent )
				{
					pParent->SetSelectedFriendName( m_szOverListUser );
					ioButton *pBtn = dynamic_cast<ioButton*>( pParent->FindChildWnd( UsePresentEtcItemWnd::ID_FRIEND_BTN ) );
					if( pBtn )
						pBtn->SetTitleText( m_szOverListUser.c_str() );
				}
				HideWnd();
			}
			break;
		}
	}
}

void FriendListWnd::OnRender()
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
	if( (int)m_FriendList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_FriendList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		bDarkRender = !bDarkRender;
		if( m_szOverListUser == m_FriendList[iListPos] )
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
			m_FriendList[iListPos].c_str() );
	}
}

//////////////////////////////////////////////////////////////////////////
PresentListWnd::PresentListWnd()
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

	m_iSelectedPresent = -1;
}

PresentListWnd::~PresentListWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void PresentListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
}

void PresentListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void PresentListWnd::iwm_show()
{
	m_szOverListItem.Clear();

	m_iCurPos = 0;
	if( (int)m_PresentList.size() <= MAX_PAGE_LIST )
	{
		HideChildWnd( ID_VERT_SCROLL );
	}
	else
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, m_PresentList.size() );
			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}

	// 윈도우 사이즈 결정	
	SetSize( GetWidth(), ( min( MAX_PAGE_LIST, m_PresentList.size() ) * m_iHeightGap) + m_iAddHeight );
	SetCheckParentOutSide( true );

	if( GetParent() )
	{
		ioWnd *pBtn = GetParent()->FindChildWnd( UsePresentEtcItemWnd::ID_PRESENT_BTN );
		if( pBtn )
		{
			int iCnt = min( MAX_PAGE_LIST, m_PresentList.size() );
			int iYOffset = m_iHeightGap * ( iCnt - 1 );
			SetWndPos( pBtn->GetXPos(), pBtn->GetYPos() - iYOffset - 37 );
		}
	}
}

void PresentListWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void PresentListWnd::iwm_wheel( int zDelta )
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

void PresentListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void PresentListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_szOverListItem.Clear();

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != GetParent()->FindChildWnd( UsePresentEtcItemWnd::ID_PRESENT_BTN ) &&
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
	if( (int)m_PresentList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_PresentList.size();
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
			m_szOverListItem  = m_PresentList[iListPos].m_Name;
			if( pMouse->IsLBtnUp() )
			{
				m_iSelectedPresent = iListPos;

				UsePresentEtcItemWnd *pParent = dynamic_cast<UsePresentEtcItemWnd*>( GetParent() );
				if( pParent )
				{
					pParent->SetSelectedPresentArray( iListPos );
					ioButton *pBtn = dynamic_cast<ioButton*>( pParent->FindChildWnd( UsePresentEtcItemWnd::ID_PRESENT_BTN ) );
					if( pBtn )
						pBtn->SetTitleText( m_szOverListItem.c_str() );
				}

				HideWnd();
			}
			break;
		}
	}
}

void PresentListWnd::OnRender()
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
	if( (int)m_PresentList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_PresentList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		bDarkRender = !bDarkRender;
		if( m_szOverListItem == m_PresentList[iListPos].m_Name )
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
			m_PresentList[iListPos].m_Name.c_str() );
	}
}

void PresentListWnd::SetPresentList( const ioEtcItemSendPresent::vPresentData &PresentList )
{
	m_PresentList.clear();
	m_iSelectedPresent = -1;

	char szBuf[MAX_PATH] = "";
	int iCnt = PresentList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioEtcItemSendPresent::PresentData kPresentData = PresentList[i];
		ioHashString szPresentName;
		

		switch( kPresentData.m_iPresentType )
		{
		case PRESENT_ETC_ITEM:
			{
				ioHashString szName = g_PresentMgr.GetPresentValue1Text( kPresentData.m_iPresentType, kPresentData.m_iPresentValue1, kPresentData.m_iPresentValue2 );
				wsprintf_e( szBuf, "%s %d개", szName.c_str(), kPresentData.m_iPresentValue2 );
				szPresentName = szBuf;
			}
			break;
		case PRESENT_PESO:
			{
				wsprintf_e( szBuf, "%d 페소", kPresentData.m_iPresentValue1 );
				szPresentName = szBuf;
			}
			break;
		case PRESENT_EXTRAITEM:
			{
				ioHashString szName = g_PresentMgr.GetPresentValue1Text( kPresentData.m_iPresentType, kPresentData.m_iPresentValue1, kPresentData.m_iPresentValue2 );
				int iGrowth = kPresentData.m_iPresentValue2 / 10000;
				wsprintf_e( szBuf, "%s +%d", szName.c_str(), iGrowth );
				szPresentName = szBuf;
			}
			break;
		case PRESENT_GRADE_EXP:
			{
				wsprintf_e( szBuf, "경험치 +%d", kPresentData.m_iPresentValue1 );
				szPresentName = szBuf;
			}
			break;
		default:
			szPresentName = g_PresentMgr.GetPresentValue1Text( kPresentData.m_iPresentType, kPresentData.m_iPresentValue1, kPresentData.m_iPresentValue2 );
			break;
		}
		
		PresentInfo kInfo( szPresentName, i );
		m_PresentList.push_back( kInfo );
	}

	if( m_PresentList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "보낼 수 있는 아이템이 없습니다." );
		HideWnd();
	}

	if( !IsShow() )
		ShowWnd();
}

//////////////////////////////////////////////////////////////////////////
SendPresentEtcItemConfirmWnd::SendPresentEtcItemConfirmWnd()
{
	m_pItemIcon = NULL;
	m_pSubIcon = NULL;
	m_pReturnWnd = NULL;
}

SendPresentEtcItemConfirmWnd::~SendPresentEtcItemConfirmWnd()
{
	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pSubIcon );
}

void SendPresentEtcItemConfirmWnd::SetInfo( ioWnd *pReturnWnd, const ioHashString &szFriendName, const ioEtcItemSendPresent::PresentData &kPresent )
{
	m_pReturnWnd = pReturnWnd;

	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pSubIcon );

	char szBuf[MAX_PATH] = "";
	ioHashString szItemDesc;
	switch( kPresent.m_iPresentType )
	{
	case PRESENT_ETC_ITEM:
		{
			ioHashString szName = g_PresentMgr.GetPresentValue1Text( kPresent.m_iPresentType, kPresent.m_iPresentValue1, kPresent.m_iPresentValue2 );
			wsprintf_e( szBuf, "%s %d개", szName.c_str(), kPresent.m_iPresentValue2 );
			szItemDesc = szBuf;
		}
		break;
	case PRESENT_PESO:
		{
			wsprintf_e( szBuf, "%d 페소", kPresent.m_iPresentValue1 );
			szItemDesc = szBuf;
		}
		break;
	case PRESENT_EXTRAITEM:
		{
			ioHashString szName = g_PresentMgr.GetPresentValue1Text( kPresent.m_iPresentType, kPresent.m_iPresentValue1, kPresent.m_iPresentValue2 );
			wsprintf_e( szBuf, "%s +%d", szName.c_str(), ( kPresent.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2 );
			szItemDesc = szBuf;
		}
		break;
	case PRESENT_GRADE_EXP:
		{
			wsprintf_e( szBuf, "경험치 +%d", kPresent.m_iPresentValue1 );
			szItemDesc = szBuf;
		}
		break;
	default:
		{
			szItemDesc = g_PresentMgr.GetPresentValue1Text( kPresent.m_iPresentType, kPresent.m_iPresentValue1, kPresent.m_iPresentValue2 );
		}
		break;
	}

	ioHashString szIConName = g_PresentMgr.GetPresentIconText( kPresent.m_iPresentType, kPresent.m_iPresentValue1, kPresent.m_iPresentValue2 );
	ioHashString szSubIConName = g_PresentMgr.GetPresentSubIconText( kPresent.m_iPresentType, kPresent.m_iPresentValue1, kPresent.m_iPresentValue2 );
	m_pItemIcon = g_UIImageSetMgr.CreateImageByFullName( szIConName );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( szSubIConName );

	for( int i=0; i<MAX_MSG_LIST_SIZE; i++ )
		m_PrinterList[i].ClearList();

	m_PrinterList[0].SetTextStyle( TS_NORMAL );
	m_PrinterList[0].SetBkColor( 0, 0, 0 );	
	m_PrinterList[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_PrinterList[0].AddTextPiece( FONT_SIZE_13, "%s", szItemDesc.c_str() );

	m_PrinterList[1].SetTextStyle( TS_NORMAL );
	m_PrinterList[1].SetBkColor( 0, 0, 0 );	
	m_PrinterList[1].SetTextColor( TCT_DEFAULT_GRAY );
	m_PrinterList[1].AddTextPiece( FONT_SIZE_13, "받는이 : " );
	m_PrinterList[1].SetTextColor( TCT_DEFAULT_GREEN );
	m_PrinterList[1].AddTextPiece( FONT_SIZE_13, "%s", szFriendName.c_str() );

	m_PrinterList[2].SetTextStyle( TS_NORMAL );
	m_PrinterList[2].SetBkColor( 0, 0, 0 );	
	m_PrinterList[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_PrinterList[2].AddTextPiece( FONT_SIZE_13, "본 아이템을 선물 하시겠습니까?" );

	/*
	int iMinWidth  = GetWidth();
	int iMinHeight = 0;
	int iMaxLine   = 0;
	for( int i=0; i<MAX_MSG_LIST_SIZE; ++i )
	{
		if( m_PrinterList[i].IsEmpty() )
			break;

		int iCurLineWidth = m_PrinterList[i].GetFullWidth() + 34;
		if( iMinWidth < iCurLineWidth )
			iMinWidth = iCurLineWidth;
		iMaxLine++;
	}
	iMinHeight += ( 20 * iMaxLine );

	SetWndPos( (Setting::Width() - GetWidth() ) / 2, ( (Setting::Height() - GetHeight() ) / 2 ) - 20 );

	// 하단바
	if( m_pMultiplyFrm )
	{
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
	}*/

	ShowWnd();
}

void SendPresentEtcItemConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			SendPresent();
			HideWnd();
		}
		break;
	}
}

void SendPresentEtcItemConfirmWnd::SendPresent()
{
	UsePresentEtcItemWnd *pWnd = dynamic_cast<UsePresentEtcItemWnd*>( g_GUIMgr.FindWnd( USE_PRESENT_ETC_ITEM_WND ) );
	if( pWnd )
	{
		pWnd->SendPresent();
		pWnd->HideWnd();
		HideWnd();
	}
}

void SendPresentEtcItemConfirmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	for( int i=0 ; i<MAX_MSG_LIST_SIZE ; i++ )
	{
		if( m_PrinterList[i].IsEmpty() )
			break;

		m_PrinterList[i].PrintFullTextWidthCut( iXPos + 124, iYPos + 144 + ( i * 18 ) , TAT_CENTER, 218 );
	}

	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pItemIcon )
		m_pItemIcon->Render( iXPos + 123, iYPos + 98 );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + 123, iYPos + 98 );
}