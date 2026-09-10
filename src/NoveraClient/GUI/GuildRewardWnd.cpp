#include "StdAfx.h"
#include "GuildRewardWnd.h"
#include "GuildWnd.h"
#include "NewShopIconBtn.h"

#include "../ioDecorationPrice.h"

RewardItemListIconBtn::RewardItemListIconBtn()
{
	m_ItemNamePrinter.ClearList();
	m_szItemName.Clear();
}

RewardItemListIconBtn::~RewardItemListIconBtn()
{
}

void RewardItemListIconBtn::SetItem( int iPresentType, int iValue1, int iValue2 )
{
	char szBuf[MAX_PATH] = "";
	m_szItemName.Clear();
	m_ItemNamePrinter.ClearList();
	if( iPresentType == 0 && iValue1 == 0 && iValue2 == 0 )
		return;

	ioHashString szItemName = g_PresentMgr.GetPresentIconText( iPresentType, iValue1, iValue2 );
	if ( szItemName.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s - Item Value is not exist - PresentType : %d, Value1 : %d, Value2 : %d", __FUNCTION__, iPresentType, iValue1, iValue2 );
		SAFEDELETE( m_pIcon );
		return;
	}

	ioUIRenderImage* pIcon = g_UIImageSetMgr.CreateImageByFullName( szItemName );
	ioUIRenderImage* pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( iPresentType, iValue1, iValue2 ) );
	if ( pIcon )
		SetIcon( pIcon );
	else
	{
		LOG.PrintTimeAndLog( 0, "%s - Item Icon is not exist - Item Name: %s,", __FUNCTION__, szItemName.c_str() );
		SAFEDELETE( m_pIcon );
	}
	SetSubIcon( pSubIcon );

	sprintf_s_e( szBuf, "%s %s", g_PresentMgr.GetPresentValue1Text( iPresentType, iValue1, iValue2 ).c_str(),
		g_PresentMgr.GetPresentValue2Text( iPresentType, iValue1, iValue2 ).c_str() );
	
	m_szItemName = szBuf;
	m_ItemNamePrinter.ClearList();
	m_ItemNamePrinter.SetTextStyle( TS_NORMAL);
	m_ItemNamePrinter.SetBkColor( 0, 0, 0 );
	m_ItemNamePrinter.SetTextColor( TCT_DEFAULT_BLUE ); 
	m_ItemNamePrinter.AddTextPiece( FONT_SIZE_13, m_szItemName.c_str() );
}

void RewardItemListIconBtn::OnRender()
{
	NewShopIconBtn::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{ 
		X_TEXT_OFFSET = 32, 
		Y_TEXT_OFFSET = 79 
	};
	if ( !m_ItemNamePrinter.IsEmpty() )
		m_ItemNamePrinter.PrintFullTextWidthCut( iXPos+X_TEXT_OFFSET, iYPos+Y_TEXT_OFFSET, TAT_CENTER, 104.0f );
}

/////////////////////////////////////////////////////////////////////////////

GuildPossibleRewardListWnd::GuildPossibleRewardListWnd(void)
{
	m_szAttendTitleName.Clear();
	m_szRankTitleName.Clear();
	ClearData();
}

GuildPossibleRewardListWnd::~GuildPossibleRewardListWnd(void)
{
}

void GuildPossibleRewardListWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	ioWnd::AddRenderFrame( szType, pFrame );
}

void GuildPossibleRewardListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioWnd::AddRenderImage( szType, pImage );
}

void GuildPossibleRewardListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szAttendTitleName = xElement.GetStringAttribute( "AttendTitle");
	m_szRankTitleName = xElement.GetStringAttribute( "RankTitle");
}

void GuildPossibleRewardListWnd::ClearData()
{
	m_iRewardType = 0;
	m_dwCurRadioBtn = 0;
	m_iMaxPage = 0;
	m_iCurPage = 0;
	m_PagePrinter.ClearList();

	for ( int i= 0 ; i<MAX_TEXT ; ++i )
		m_UnderLinePrinter[i].ClearList();
}

void GuildPossibleRewardListWnd::iwm_show()
{
	SetTitleByType();
	ChangeRadioBtn( ID_RADIO_BTN1 );

	GuildEnableRewardToolTip *pToolTip = dynamic_cast<GuildEnableRewardToolTip*>( FindChildWnd( ID_TOOLTIP ) );
	if ( pToolTip && !pToolTip->IsShow() )
	{
		pToolTip->SetSize( 0, 0 );
		pToolTip->ShowWnd();
	}
}

void GuildPossibleRewardListWnd::iwm_hide()
{
	ClearData();
	GuildEnableRewardToolTip *pToolTip = dynamic_cast<GuildEnableRewardToolTip*>( FindChildWnd( ID_TOOLTIP ) );
	if ( pToolTip && pToolTip->IsShow() )
		pToolTip->HideWnd();
}

void GuildPossibleRewardListWnd::ShowRewardWnd( int iType )
{
	ClearData();
	m_iRewardType = iType;
	ShowWnd();
}

void GuildPossibleRewardListWnd::SetTitleByType()
{
	for ( int i= 0 ; i<MAX_TEXT ; ++i )
	{
		m_UnderLinePrinter[i].SetTextStyle( TS_NORMAL);
		m_UnderLinePrinter[i].SetBkColor( 0, 0, 0 );
	}

	if ( m_iRewardType == GuildInfoWnd::ATTEND_REWARD )
	{
		SetTitleText( m_szAttendTitleName.c_str() );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, "★ " );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_RED ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, "어제 길드 출석 인원" );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, "에 따라" );

		m_UnderLinePrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[1].AddTextPiece( FONT_SIZE_13, "보상 1개를 랜덤으로 주는 아이템이 지급됩니다." );
	}
	else if ( m_iRewardType == GuildInfoWnd::RANK_REWARD )
	{
		SetTitleText( m_szRankTitleName.c_str() );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, "★ " );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_RED ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, "대회 시즌 후 달성한 길드 등급 " );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, "에 따라" );

		m_UnderLinePrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[1].AddTextPiece( FONT_SIZE_13, "보상 1개를 랜덤으로 주는 아이템이 지급됩니다." );
	}
	else
		LOG.PrintTimeAndLog( 0, "%s - is not exist RewardType : %d", m_iRewardType );
}

void GuildPossibleRewardListWnd::ChangeRadioBtn( DWORD dwSubBtn )
{
	m_dwCurRadioBtn = dwSubBtn;
	CheckRadioButton( ID_RADIO_BTN1, ID_RADIO_BTN5, m_dwCurRadioBtn );

	m_RewardItemVec.clear();
	if ( m_iRewardType == GuildInfoWnd::ATTEND_REWARD )
	{
		for ( int i=0 ; i<MAX_RADIO_BTN ; ++i )
		{
			ioButton* pBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_RADIO_BTN1 + i ) );
			if ( pBtn )
			{
				const GuildAttendRwardInfo* const pAttendInfo = g_GuildInfo.GetAttendRewardInfo( i );
				if ( pAttendInfo )
				{
					pBtn->SetTitleText( pAttendInfo->m_szName.c_str() );
					if( !pBtn->IsShow() )
						pBtn->ShowWnd();
				}
				else
					pBtn->HideWnd();
			}
		}

		int iIndex = m_dwCurRadioBtn - ID_RADIO_BTN1;
		const GuildAttendRwardInfo* const pAttendInfo = g_GuildInfo.GetAttendRewardInfo( iIndex );
		if ( pAttendInfo )
			m_RewardItemVec = pAttendInfo->m_RewardPossibleItemVec;

	}
	else if ( m_iRewardType == GuildInfoWnd::RANK_REWARD )
	{
		for ( int i=0 ; i<MAX_RADIO_BTN ; ++i )
		{
			ioButton* pBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_RADIO_BTN1 + i ) );
			if( pBtn )
			{
				const GuildRankRwardInfo* const pRankInfo = g_GuildInfo.GetRankRewardInfo( i );
				if ( pRankInfo )
				{
					pBtn->SetTitleText( pRankInfo->m_szName.c_str() );
					if( !pBtn->IsShow() )
						pBtn->ShowWnd();
				}
				else
					pBtn->HideWnd();
			}
		}

		int iIndex = m_dwCurRadioBtn - ID_RADIO_BTN1;
		const GuildRankRwardInfo* const pRankInfo = g_GuildInfo.GetRankRewardInfo( iIndex );
		if ( pRankInfo )
			m_RewardItemVec = pRankInfo->m_RewardPossibleItemVec;
	}

	//std::sort( m_RewardItemVec.begin(), m_RewardItemVec.end(), ShopExtraItemSort() );

	int iTotalCnt = m_RewardItemVec.size();
	m_iMaxPage = iTotalCnt / MAX_ICON_BTN;
	if( iTotalCnt%MAX_ICON_BTN > 0 )
		m_iMaxPage += 1;

	UpdatePage( 0 );
}


void GuildPossibleRewardListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	
	switch( dwID )
	{
	case ID_PRE_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int iNewPage = m_iCurPage - 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int iNewPage = m_iCurPage + 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_X:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
	default:
		{
			if ( COMPARE( dwID, ID_RADIO_BTN1, ID_RADIO_BTN5 + 1 ) )
			{
				if( cmd == IOBN_BTNDOWN )
				{
					ChangeRadioBtn( dwID );
				}
			}
			else if ( COMPARE( dwID, ID_ICON_BTN1, ID_ICON_BTN12 + 1 ) )
			{
				if( cmd == IOWN_OVERED )
				{
					RewardItemListIconBtn *pIcon = dynamic_cast<RewardItemListIconBtn*>( FindChildWnd( dwID ) );
					if ( pIcon )
					{
						if ( !pIcon->GetItemName().IsEmpty() )
						{
							GuildEnableRewardToolTip *pTooltip = dynamic_cast<GuildEnableRewardToolTip*>( FindChildWnd( ID_TOOLTIP ) );
							if( pTooltip )
								pTooltip->SetInfo( pIcon->GetItemName() );
						}
					}
				}
				else if( cmd == IOWN_LEAVED )
				{
					GuildEnableRewardToolTip *pTooltip = dynamic_cast<GuildEnableRewardToolTip*>( FindChildWnd( ID_TOOLTIP ) );
					if( pTooltip )
						pTooltip->SetSize( 0, 0 );
				}
			}
		}
		break;
	}
}

void GuildPossibleRewardListWnd::UpdatePage( int iPage )
{
	m_iCurPage = iPage;

	m_PagePrinter.ClearList();

	m_PagePrinter.SetTextStyle( TS_NORMAL );
	m_PagePrinter.SetBkColor( 0, 0, 0 );
	m_PagePrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_PagePrinter.AddTextPiece( FONT_SIZE_13, "%d", m_iCurPage + 1 );
	m_PagePrinter.SetTextColor( TCT_DEFAULT_GRAY );
	if ( m_iMaxPage == 0 )
		m_PagePrinter.AddTextPiece( FONT_SIZE_13, "/%d", m_iMaxPage + 1 );
	else
		m_PagePrinter.AddTextPiece( FONT_SIZE_13, "/%d", m_iMaxPage );

	SetChildActive( ID_PRE_BTN );
	SetChildActive( ID_NEXT_BTN );

	if( m_iCurPage == 0 )
		SetChildInActive( ID_PRE_BTN );

	if( m_iCurPage == m_iMaxPage-1 )
		SetChildInActive( ID_NEXT_BTN );

	UpdateIcon();
}

void GuildPossibleRewardListWnd::UpdateIcon()
{
	int iStartArray = m_iCurPage * MAX_ICON_BTN;
	for ( int i=0 ; i<MAX_ICON_BTN ; ++i )
	{
		RewardItemListIconBtn* pRewardBtn = dynamic_cast<RewardItemListIconBtn*>( FindChildWnd( ID_ICON_BTN1 + i ) );
		if ( !pRewardBtn )
			continue;

		int iCurArray = iStartArray + i;
		if( COMPARE( iCurArray, 0, (int)m_RewardItemVec.size() ) )
		{
			GashaponPresent RewardItemInfo = m_RewardItemVec[ iCurArray ];
			pRewardBtn->SetItem( (int)RewardItemInfo.m_iType, RewardItemInfo.m_iValue1,RewardItemInfo.m_iValue2 );
			if ( !pRewardBtn->IsShow() )
				pRewardBtn->ShowWnd();
		}
		else
			pRewardBtn->HideWnd();
	}
}

void GuildPossibleRewardListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		DESC_X		= 238,
		DESC_Y		= 470,

		LINE_GAP	= 20,

		PAGE_X		= 238,
		PAGE_Y		= 425,
	};

	if ( !m_PagePrinter.IsEmpty() )
		m_PagePrinter.PrintFullText( iXPos + PAGE_X, iYPos + PAGE_Y, TAT_CENTER );

	for (int i = 0; i < MAX_TEXT ; i++)
	{
		if ( !m_UnderLinePrinter[i].IsEmpty() )
			m_UnderLinePrinter[i].PrintFullText( iXPos + DESC_X, iYPos + DESC_Y + ( i*LINE_GAP), TAT_CENTER );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GuildAttendUserBtn::GuildAttendUserBtn()
{
	ClearData();
	m_pGrayAttendance = NULL;
	m_pRedAttendance = NULL;
	m_pGreenAttendance = NULL;
}

GuildAttendUserBtn::~GuildAttendUserBtn()
{
	SAFEDELETE( m_pGrayAttendance );
	SAFEDELETE( m_pRedAttendance );
	SAFEDELETE( m_pGreenAttendance );
}

void GuildAttendUserBtn::ClearData()
{
	m_dwBtnArray	= 0;
	m_bTodayAttend = false;
	m_bYesterdayAttend = false;
	m_iUserLevel = 0;
}

void GuildAttendUserBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "GrayAttendance" )
	{
		SAFEDELETE( m_pGrayAttendance );
		m_pGrayAttendance = pImage;
	}
	else if( szType == "RedAttendance" )
	{
		SAFEDELETE( m_pRedAttendance );
		m_pRedAttendance = pImage;
	}
	else if( szType == "GreenAttendance" )
	{
		SAFEDELETE( m_pGreenAttendance );
		m_pGreenAttendance = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void GuildAttendUserBtn::MemberInfo( int iListArray, const ioHashString &szUserID, int iUserLevel, bool &bTodayAttend, bool &YesterdaybAttend )
{
	ClearData();
	m_szUserID      = szUserID;
	m_bTodayAttend = bTodayAttend;
	m_bYesterdayAttend = YesterdaybAttend;
	m_iUserLevel	= iUserLevel;
	m_dwBtnArray    = iListArray + 1;
	SetActive();
}

void GuildAttendUserBtn::OnRender()
{
	ioButton::OnRender();

	if( !HasWndStyle( IWS_INACTIVE ) )
	{
		int iXPos, iYPos;
		iXPos = GetDerivedPosX();
		iYPos = GetDerivedPosY();

		enum
		{
			ID_X			= 23,
			YESTERDAY_X		= 134,
			TODAY_X			= 181,
			TEXT_Y			= 3,
			YESTER_ICON_X	= 118,
			TODAY_ICON_X	= 165,

		};

		// 계급
		g_LevelMgr.RenderGrade( m_szUserID, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1 );
		
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		// 아이디 
		g_FontMgr.PrintTextWidthCut( iXPos + ID_X, iYPos + TEXT_Y, FONT_SIZE_12,FLOAT90, m_szUserID.c_str() );
		//Text
		g_FontMgr.PrintText( iXPos + YESTERDAY_X, iYPos + TEXT_Y, FONT_SIZE_12, "어제/");
		g_FontMgr.PrintText( iXPos + TODAY_X, iYPos + TEXT_Y, FONT_SIZE_12, "오늘");

		//아이콘
		//어제 출석을 한경우
		if ( m_bYesterdayAttend )
		{
			if ( m_pGreenAttendance )
				m_pGreenAttendance->Render( iXPos + YESTER_ICON_X, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			if ( m_pRedAttendance )
				m_pRedAttendance->Render( iXPos + YESTER_ICON_X, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		//오늘 출석을 한경우
		if ( m_bTodayAttend )
		{
			if ( m_pGreenAttendance )
				m_pGreenAttendance->Render( iXPos + TODAY_ICON_X, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			if ( m_pGrayAttendance )
				m_pGrayAttendance->Render( iXPos + TODAY_ICON_X, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
					
	}
}
///////////////////////////////////////////////////
GuildAttendInfoListWnd::GuildAttendInfoListWnd( void )
{
}

GuildAttendInfoListWnd::~GuildAttendInfoListWnd( void )
{

}

void GuildAttendInfoListWnd::iwm_show()
{
	if( g_GuildInfo.CheckMemberAttendInfoRequestTime() )
	{
		SP2Packet kPacket( CTPK_GUILD_MEMBER_ATTEND_RENEWAL );
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
		LOG.PrintTimeAndLog( 0, "%s : Request CTPK_GUILD_MEMBER_ATTEND_RENEWAL, Time : %d " ,__FUNCTION__, g_GuildInfo.GetAttendInfoRequestTime() );
	}
}

void GuildAttendInfoListWnd::UpdateUserList( ioGuildData *pGuildData )
{
	if( pGuildData )
	{
		for(int i = ID_USER_LIST_1;i < ID_USER_LIST_32 + 1;i++ )
		{
			GuildAttendUserBtn *pUserBtn = dynamic_cast<GuildAttendUserBtn*>(FindChildWnd( i ));
			if( pUserBtn )
			{
				int	  iUserLevel;
				ioHashString szUserID, szUserPos;
				bool bTodayAttend,bYesterdayAttend;
				pGuildData->GetUserInfo( i, szUserID,iUserLevel, bYesterdayAttend, bTodayAttend );
				if ( szUserID.IsEmpty() )
					pUserBtn->SetInActive();
				else
					pUserBtn->MemberInfo( i, szUserID, iUserLevel, bTodayAttend, bYesterdayAttend  );
			}
		}	
	}
}

//////////////////////////////////////////////////////////////
GuildAttendRewardWnd::GuildAttendRewardWnd()
{
	m_pItemIcon = NULL;
	m_pSubIcon = NULL;
}

GuildAttendRewardWnd::~GuildAttendRewardWnd()
{
	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pSubIcon );
}

void GuildAttendRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_X:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void GuildAttendRewardWnd::SettingRewardInfo( BYTE btYesterdayAttendCount, short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	m_AttendNumberPrinter.ClearList();
	m_ItemNamePrinter.ClearList();
	
	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pSubIcon );
	ioHashString szItemName = g_PresentMgr.GetPresentIconText( iPresentType, iPresentValue1, iPresentValue2 );
	if ( szItemName.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s - Item Value is not exist - PresentType : %d, Value1 : %d, Value2 : %d", __FUNCTION__, iPresentType, iPresentValue1, iPresentValue2 );
		return;
	}

	m_pItemIcon = g_UIImageSetMgr.CreateImageByFullName( szItemName );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( iPresentType, iPresentValue1, iPresentValue2 ) );
	if ( !m_pItemIcon )
		LOG.PrintTimeAndLog( 0, "%s - Item Icon is not exist - Item Name: %s,", __FUNCTION__, szItemName.c_str() );

	m_AttendNumberPrinter.SetTextStyle( TS_NORMAL );
	m_AttendNumberPrinter.SetBkColor( 0, 0, 0 );
	m_AttendNumberPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_AttendNumberPrinter.AddTextPiece( FONT_SIZE_17, "길드 " );
	m_AttendNumberPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	m_AttendNumberPrinter.AddTextPiece( FONT_SIZE_17, "%d명", btYesterdayAttendCount );
	m_AttendNumberPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_AttendNumberPrinter.AddTextPiece( FONT_SIZE_17, "출석!!" );


	char szBuf[MAX_PATH] = "";
	sprintf_s_e( szBuf, "%s 획득!!!", g_PresentMgr.GetPresentValue1Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() );

	m_ItemNamePrinter.SetTextStyle( TS_NORMAL );
	m_ItemNamePrinter.SetBkColor( 0, 0, 0 );
	m_ItemNamePrinter.SetTextColor( TCT_DEFAULT_GRAY );
	m_ItemNamePrinter.AddTextPiece( FONT_SIZE_13, szBuf );

	ShowWnd();
}

void GuildAttendRewardWnd::OnRender()
{
	ioWnd::OnRender();
	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pItemIcon )
		m_pItemIcon->Render( iXPos + 125, iYPos + 98 );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + 125, iYPos + 98 );

	if ( !m_AttendNumberPrinter.IsEmpty() )
		m_AttendNumberPrinter.PrintFullText( iXPos + 126, iYPos + 147, TAT_CENTER );
	if ( !m_ItemNamePrinter.IsEmpty())
		m_ItemNamePrinter.PrintFullText( iXPos + 126, iYPos + 169, TAT_CENTER );
}

//////////////////////////////////////////////////////////////
GuildRankRewardWnd::GuildRankRewardWnd()
{
	m_pItemIcon = NULL;
	m_pSubIcon = NULL;
}

GuildRankRewardWnd::~GuildRankRewardWnd()
{
	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pSubIcon );
}

void GuildRankRewardWnd::SettingRewardInfo( int iGuildLevel, short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	m_GuildRankPrinter.ClearList();
	m_ItemNamePrinter.ClearList();
	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pSubIcon );
	ioHashString szItemName = g_PresentMgr.GetPresentIconText( iPresentType, iPresentValue1, iPresentValue2 );
	if ( szItemName.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s - Item Value is not exist - PresentType : %d, Value1 : %d, Value2 : %d", __FUNCTION__, iPresentType, iPresentValue1, iPresentValue2 );
		return;
	}

	m_pItemIcon = g_UIImageSetMgr.CreateImageByFullName( szItemName );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( iPresentType, iPresentValue1, iPresentValue2 ) );
	if ( !m_pItemIcon )
		LOG.PrintTimeAndLog( 0, "%s - Item Icon is not exist - Item Name: %s,", __FUNCTION__, szItemName.c_str() );

	enum{ MAX_GUILD_LEVEL = 7, MAX_LEVEL = 2, };
	if ( !COMPARE( iGuildLevel, 0, MAX_GUILD_LEVEL + 1 ) )
		return;

	char szLevelText[MAX_PATH] = "";
	char szLevel[MAX_GUILD_LEVEL][MAX_LEVEL] = { "F", "E", "D", "C", "B", "A", "S" };
	sprintf_s_e( szLevelText, "%s등급", szLevel[iGuildLevel] );

	m_GuildRankPrinter.SetTextStyle( TS_NORMAL );
	m_GuildRankPrinter.SetBkColor( 0, 0, 0 );
	m_GuildRankPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_GuildRankPrinter.AddTextPiece( FONT_SIZE_17, "길드 " );
	m_GuildRankPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	m_GuildRankPrinter.AddTextPiece( FONT_SIZE_17, szLevelText );
	m_GuildRankPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_GuildRankPrinter.AddTextPiece( FONT_SIZE_17, "달성!!" );

	char szBuf[MAX_PATH] = "";
	sprintf_s_e( szBuf, "%s 획득!!!", g_PresentMgr.GetPresentValue1Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() );

	m_ItemNamePrinter.SetTextStyle( TS_NORMAL );
	m_ItemNamePrinter.SetBkColor( 0, 0, 0 );
	m_ItemNamePrinter.SetTextColor( TCT_DEFAULT_GRAY );
	m_ItemNamePrinter.AddTextPiece( FONT_SIZE_13, szBuf );

	ShowWnd();
}

void GuildRankRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_X:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void GuildRankRewardWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pItemIcon )
		m_pItemIcon->Render( iXPos + 125, iYPos + 98 );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + 125, iYPos + 98 );

	if( !m_GuildRankPrinter.IsEmpty() )
		m_GuildRankPrinter.PrintFullText( iXPos + 126, iYPos + 147, TAT_CENTER );
	if( !m_ItemNamePrinter.IsEmpty() )
		m_ItemNamePrinter.PrintFullText( iXPos + 126, iYPos + 169, TAT_CENTER );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GuildEnableRewardToolTip::GuildEnableRewardToolTip()
{
	m_szPresentName.ClearList();
	m_szRewardInfoOverSnd.Clear();
}

GuildEnableRewardToolTip::~GuildEnableRewardToolTip()
{
	m_szPresentName.ClearList();
	m_szRewardInfoOverSnd.Clear();
}

void GuildEnableRewardToolTip::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );
	m_szRewardInfoOverSnd = xElement.GetStringAttribute_e( "RewardInfoOver" );
}

void GuildEnableRewardToolTip::OnProcess( float fTimePerSec )
{
	if ( IsShow() )
	{
		if ( GetWidth() != 0 && GetHeight() != 0 )
		{
			ioLSMouse *pMouse = g_App.GetMouse();
			if( !pMouse ) return;
			pMouse->SetToolTipCursor();
			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetScreenWndPos( iXPos, iYPos );
			GoToTop();
		}
	}
}

void GuildEnableRewardToolTip::SetInfo( const ioHashString szItemName )
{
	if ( szItemName.IsEmpty() )
	{
		SetSize( 0, 0 );
		return;
	}
	m_szPresentName.ClearList();
	m_szPresentName.SetTextStyle( TS_NORMAL );
	m_szPresentName.SetBkColor( 0, 0, 0 );	
	m_szPresentName.SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회색
	m_szPresentName.AddTextPiece( FONT_SIZE_12, "%s", szItemName.c_str() );

	int iMaxWidth  = m_szPresentName.GetFullWidth() + SIDE_TEXT_GAP;
	int iMaxHeight = MIN_HEIGHT;
	SetSize( iMaxWidth, iMaxHeight );

	if( !m_szRewardInfoOverSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRewardInfoOverSnd );
}

void GuildEnableRewardToolTip::OnRender()
{
	if ( GetWidth() == 0 && GetHeight() == 0 )
		return;
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;
	if( !m_szPresentName.IsEmpty() )
		m_szPresentName.PrintFullText( iXPos, iYPos, TAT_LEFT );
}
