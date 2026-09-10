

#include "StdAfx.h"

#include "../ioGuildMarkMgr.h"
#include "../ioShuffleRoomMgr.h"
#include "../Housing/BlockDefine.h"

#include "GuildWnd.h"
#include "GuildHomeAdminInfoListWnd.h"
#include "HeroMainWnd.h"
#include "UserInfoToolTip.h"
#include "GuildRewardWnd.h"
#include "GuildHQEtcItemNeedPopup.h"
#include "ioHousingBlockInvenWnd.h"

GuildRankingListBtn::GuildRankingListBtn()
{
	m_pBlueNormal = NULL;
	m_pRedNormal  = NULL;
	m_pBlueOver	  = NULL;
	m_pRedOver	  = NULL;
	m_pBlueGradeText = NULL;
	m_pRedGradeText  = NULL;
	m_bMyGuild    = false;
}

GuildRankingListBtn::~GuildRankingListBtn()
{
	SAFEDELETE( m_pBlueNormal );
	SAFEDELETE( m_pRedNormal );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedOver );
	SAFEDELETE( m_pBlueGradeText );
	SAFEDELETE( m_pRedGradeText );
}

void GuildRankingListBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNormal" )
	{
		SAFEDELETE( m_pBlueNormal );
		m_pBlueNormal = pImage;
	}
	else if( szType == "RedNormal" )
	{
		SAFEDELETE( m_pRedNormal );
		m_pRedNormal = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else if( szType == "BlueGradeText" )
	{
		SAFEDELETE( m_pBlueGradeText );
		m_pBlueGradeText = pImage;
	}
	else if( szType == "RedGradeText" )
	{
		SAFEDELETE( m_pRedGradeText );
		m_pRedGradeText = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void GuildRankingListBtn::SetGuildInfo( GuildRankInfo &rkInfo )
{
	m_GuildInfo		= rkInfo;	
	m_bMyGuild      = false;

	if( m_GuildInfo.m_dwGuildIndex == 0 )
		SetInActive();
	else
	{
		SetActive();
		m_bMyGuild = g_GuildInfo.IsMyGuild( m_GuildInfo.m_dwGuildIndex );
	}
}

void GuildRankingListBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void GuildRankingListBtn::OnRenderBack( int iXPos, int iYPos )
{

	if( m_pRedNormal )
		m_pRedNormal->Render( iXPos, iYPos );

	if( m_bOver || m_bClicked || m_bMyGuild )
	{
		if( m_pRedOver )
			m_pRedOver->Render( iXPos, iYPos );
	}
	
}

void GuildRankingListBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderBack( iXPos, iYPos );

	if( m_GuildInfo.m_dwGuildIndex != 0 )
	{
		g_GuildMarkMgr.RenderSmallMark( m_GuildInfo.m_dwGuildIndex, m_GuildInfo.m_dwGuildMark, iXPos + 64, iYPos + 4 );

		ioUIRenderImage *pGuildRank = g_GuildMarkMgr.GetGuildLevelRes( m_GuildInfo.m_dwGuildLevel );
		if( pGuildRank )
		{
			if( m_bOver || m_bClicked || m_bMyGuild )
				pGuildRank->SetColor( 255, 255, 255 );
			else
				pGuildRank->SetColor( 0, 0, 0 );

			pGuildRank->Render( iXPos + 81, iYPos + 4, UI_RENDER_NORMAL_ADD_COLOR );
		}

		if( m_pRedGradeText )
		{
			if( m_bOver || m_bClicked || m_bMyGuild )
				m_pRedGradeText->SetColor( 255, 255, 255 );
			else
				m_pRedGradeText->SetColor( 0, 0, 0 );
			m_pRedGradeText->Render( iXPos + 81, iYPos + 12, UI_RENDER_NORMAL_ADD_COLOR );
		}

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );

		if( m_bOver || m_bClicked || m_bMyGuild )
			g_FontMgr.SetTextColor( 255, 255, 255 ); 
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 

		g_FontMgr.PrintText( iXPos + 6, iYPos + 3, FONT_SIZE_12, STR(2), m_GuildInfo.m_dwGuildRank );
		g_FontMgr.PrintTextWidthCut( iXPos + 115, iYPos + 3, FONT_SIZE_12, 108.0f, m_GuildInfo.m_szGuildName.c_str() );

		g_FontMgr.SetAlignType( TAT_CENTER );
		float fBonus = m_GuildInfo.m_fGuildBonus * FLOAT100;
		g_FontMgr.PrintText( iXPos + 256, iYPos + 3, FONT_SIZE_12, STR(1), (int)fBonus );

		char szConvertText[MAX_PATH] = "";
		Help::ConvertNumToStrComma( m_GuildInfo.m_iGuildPoint, szConvertText, sizeof( szConvertText ) );
		g_FontMgr.PrintText( iXPos + 333, iYPos + 3, FONT_SIZE_12, szConvertText );

		g_FontMgr.SetAlignType( TAT_RIGHT );
		int iLevelToMaxEntry = (int)g_GuildInfo.GetGuildLevelToMaxEntry( m_GuildInfo.m_dwGuildLevel);
		g_FontMgr.PrintText( iXPos + 417, iYPos + 3, FONT_SIZE_12, STR(3), (int)m_GuildInfo.m_dwGuildJoinUser, min( iLevelToMaxEntry, (int)m_GuildInfo.m_dwGuildMaxUser ) );
	}
}
//////////////////////////////////////////////////////////////////////////
GuildRankingListWnd::GuildRankingListWnd()
{
	m_dwTabID   = 0;
	m_iCurPage = m_iMaxPage = 0;
}

GuildRankingListWnd::~GuildRankingListWnd()
{
	m_vGuildList.clear();
}

void GuildRankingListWnd::StartSend( DWORD dwSearchIndex, bool bProcess )
{
	if( !g_App.IsMouseRender() ) 
		return;
		
	SP2Packet kPacket( CTPK_GUILD_RANK_LIST );
	kPacket << dwSearchIndex << m_iCurPage << MAX_PAGE_LIST;
	TCPNetwork::SendToServer( kPacket );	
	m_dwSearchTime = FRAMEGETTIME();
}

void GuildRankingListWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > GUILD_SEARCH_TIME )
	{
		StartSend( 0, true );
	}
}

void GuildRankingListWnd::SetCampRadioBtn( DWORD dwTabID )
{
	m_dwTabID = dwTabID;
	
	m_vGuildList.clear();
	m_iCurPage = m_iMaxPage = 0;
	HideChildWnd( ID_GUILD_CREATE_BTN );
	HideChildWnd( ID_GUILD_CREATE_BTN );

	DWORD dwSearchIndex = 0;
	switch( m_dwTabID )
	{
	case GuildHeroRankingListWnd::ID_GUILD_BTN:
		ShowChildWnd( ID_GUILD_CREATE_BTN );
		dwSearchIndex = g_GuildInfo.GetGuildIndex();
		break;
	}
    StartSend( dwSearchIndex );

	// 버튼 초기화
	for(int i = ID_LIST_1;i < ID_LIST_10 + 1;i++)
	{
		GuildRankingListBtn *pBtn = (GuildRankingListBtn*)FindChildWnd( i );
		if( pBtn )
		{
			GuildRankInfo kInfo;
			pBtn->SetGuildInfo( kInfo );
		}
	}	
}

void GuildRankingListWnd::iwm_show()
{		
}

void GuildRankingListWnd::iwm_hide()
{
	m_dwSearchTime = 0;
}

void GuildRankingListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_GUILD_CREATE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GuildInfo.IsGuild() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else
			{
				GuildEtcItemPopup *pGuildEtcItem = dynamic_cast<GuildEtcItemPopup *>(g_GUIMgr.FindWnd( GUILD_ETC_ITEM_WND ));
				if( pGuildEtcItem )
					pGuildEtcItem->ShowGuildCreate();
			}
		}
		break;
	case ID_LEFT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( max( 0, m_iCurPage - 1 ) );
		}
		break;
	case ID_RIGHT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( min( m_iMaxPage, m_iCurPage + 1 ) );
		}
		break;
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
	case ID_LIST_6:
	case ID_LIST_7:
	case ID_LIST_8:
	case ID_LIST_9:
	case ID_LIST_10:
		if( cmd == IOBN_BTNUP )
		{
			GuildRankingListBtn *pBtn = (GuildRankingListBtn*)pWnd;
			if( pBtn->GetGuildIndex() == 0 ) return;

			GuildMainWnd *pGuildMainWnd = dynamic_cast<GuildMainWnd *>(g_GUIMgr.FindWnd( GUILD_MAIN_WND ));
			if( pGuildMainWnd )
				pGuildMainWnd->SetGuildIndex( pBtn->GetGuildIndex() );			
		}
		break;
	}
}

void GuildRankingListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessSearch();	
}

void GuildRankingListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL);
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 95, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 81, iYPos + 95, FONT_SIZE_12, STR(2) );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.PrintText( iXPos + 273, iYPos + 95, FONT_SIZE_12, STR(3) );
	g_FontMgr.PrintText( iXPos + 350, iYPos + 95, FONT_SIZE_12, STR(4) );

	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.PrintText( iXPos + 434, iYPos + 95, FONT_SIZE_12, STR(5) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(6), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(7), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 229, iYPos + 332, TAT_CENTER );
}

void GuildRankingListWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = iCurPage;

	if( m_iCurPage != iPrevPage )
		StartSend( 0 );

	int iListSIze = m_vGuildList.size();
	int iStartPos = 0;
	for(int i = ID_LIST_1;i < ID_LIST_10 + 1;i++,iStartPos++)
	{
		GuildRankingListBtn *pBtn = (GuildRankingListBtn*)FindChildWnd( i );
		if( pBtn )
		{
			if( iStartPos < iListSIze )
			{
				GuildRankInfo &kInfo = m_vGuildList[iStartPos];
				pBtn->SetGuildInfo( kInfo );
			}
			else
			{
				GuildRankInfo kInfo;
				pBtn->SetGuildInfo( kInfo );
			}
		}
	}	
}

void GuildRankingListWnd::ApplyGuildList( SP2Packet &rkPacket )
{
	m_vGuildList.clear();

	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> m_iCurPage >> iTotalList >> iCurList;			
	for(i = 0;i < iCurList;i++)
	{
		GuildRankInfo kInfo;
		rkPacket >> kInfo.m_dwGuildIndex >> kInfo.m_dwGuildRank >> kInfo.m_dwGuildLevel >> kInfo.m_dwGuildMark 
				 >> kInfo.m_szGuildName >> kInfo.m_dwGuildJoinUser >> kInfo.m_dwGuildMaxUser >> kInfo.m_fGuildBonus >> kInfo.m_iGuildPoint;
		m_vGuildList.push_back( kInfo );

		g_GuildInfo.SetGuildListInfo( kInfo.m_dwGuildIndex, kInfo.m_dwGuildRank, kInfo.m_dwGuildLevel, kInfo.m_dwGuildMark,
									  kInfo.m_szGuildName, kInfo.m_dwGuildJoinUser, kInfo.m_dwGuildMaxUser, kInfo.m_fGuildBonus, kInfo.m_iGuildPoint );		
	}    

	m_iMaxPage = max( 0, iTotalList - 1 ) / MAX_PAGE_LIST;
	UpdateSetPage( m_iCurPage );
}
//////////////////////////////////////////////////////////////////////////
GuildMarkBtn::GuildMarkBtn()
{
	m_pSlotSelectBtn = NULL;
	m_pGuildMarkLine = NULL;
}

GuildMarkBtn::~GuildMarkBtn()
{
	SAFEDELETE( m_pSlotSelectBtn );
	SAFEDELETE( m_pGuildMarkLine );
}

void GuildMarkBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "SlotSelectBtn" )
	{
		SAFEDELETE( m_pSlotSelectBtn );
		m_pSlotSelectBtn = pFrame;
	}
	else if( szType == "GuildMarkLine" )
	{
		SAFEDELETE( m_pGuildMarkLine );
		m_pGuildMarkLine = pFrame;
	}
	else
	{
		ioRadioButton::AddRenderFrame( szType, pFrame );
	}
}

void GuildMarkBtn::OnRender()
{
	ioRadioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	DWORD dwMarkID = GetID() - GuildCreateWnd::ID_MARK_SELECT1;
	g_GuildMarkMgr.RenderMark80x80Default( dwMarkID, iXPos + 5, iYPos + 5 );
	if( m_bOver && !m_bClicked )
	{
		g_GuildMarkMgr.RenderMark80x80Default( dwMarkID, iXPos + 5, iYPos + 5, UI_RENDER_SCREEN );
	}

	if( m_pGuildMarkLine )
		m_pGuildMarkLine->Render( iXPos, iYPos );

	if( m_bClicked )
	{
		if( m_pSlotSelectBtn )
			m_pSlotSelectBtn->Render( iXPos, iYPos );
	}
}
//////////////////////////////////////////////////////////////////////////
GuildCreateWnd::GuildCreateWnd()
{
	m_pPreEdit = NULL;
	m_pNameEdit= NULL;
	m_dwMarkID = ID_MARK_SELECT1;

	m_pSlotWhite = NULL;
}

GuildCreateWnd::~GuildCreateWnd()
{
	SAFEDELETE( m_pSlotWhite );
}

void GuildCreateWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "SlotWhite" )
	{
		SAFEDELETE( m_pSlotWhite );
		m_pSlotWhite = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

bool GuildCreateWnd::IsOnlyEngHanNumText( const char *szText )
{
	int iSize = strlen( szText );
	if( iSize < 1 )	return false;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal ) 
		 return false;

	for(int i = 0;i < iSize;i++)
	{
		if( !COMPARE( szText[i], 'A', 'Z'+1) && !COMPARE( szText[i], 'a', 'z'+1 ) && !COMPARE( szText[i], '0', '9'+1 ) && szText[i] != '-' )
		{
			if( i < iSize - 1 )
			{
				if( pLocal->IsCheckKorean() )
				{
					// 한글 깨진 경우
					if( (byte)szText[i] == 0xa4 && (byte)szText[i+1] >= 0xa1 && (byte)szText[i+1] <= 0xd3 )
					{
						return false;
					}
					if( (byte)szText[i] >= 0xb0 && (byte)szText[i] <= 0xc8 && (byte)szText[i+1] >= 0xa1 && (byte)szText[i+1] <= 0xfe )
					{
						i++;
						continue;
					}
				}
				else
				{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
					if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (byte)szText[i] ) )
#else
					if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (byte)szText[i] ) )
#endif

#else
					if( IsDBCSLeadByte( (byte)szText[i] ) )
#endif

					{
						i++;
						continue;
					}
				}
			}
			return false;
		}
	}
	return true;
}

void GuildCreateWnd::CreateGuild()
{
	if( g_App.IsMouseBusy() ) return;	

/*	if( ( !g_App.IsMeDeveloper() && !g_App.IsMeGameMaster() ) && 
		g_MyInfo.GetGradeLevel() < g_GuildInfo.GetGuildCreateLimitLevel() )
	{
		char szName[MAX_PATH] = "";
		g_LevelMgr.GetGradeName( g_GuildInfo.GetGuildCreateLimitLevel(), szName, sizeof( szName ), false, false );
	//	g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szName );
		return;
	}
*/
	if( g_GuildInfo.IsGuild() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		iwm_command( FindChildWnd( ID_EXIT ), IOBN_BTNUP, 0 );
		return;
	}

	/*   진영에 가입되어있지 않아도 길드 생성 가능. 2012.09.18
	if( g_MyInfo.GetUserCampPos() == CAMP_NONE )
	{
		g_GUIMgr.ShowWnd( CAMP_JOIN_NEED_POPUP );
		return;
	}
	*/
	
	ioHashString szGuildName;
	ioEdit *pEdit = (ioEdit*)m_pNameEdit;
	if( pEdit )
	{
		szGuildName = pEdit->GetText();
	}

	if( szGuildName.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );		
		return;
	}

	if( !g_App.IsRightID( szGuildName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );		
		return;
	}

	ioHashString szNotMakeString;
	if( g_App.IsNotMakeID( szGuildName.c_str(), szNotMakeString ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11), szNotMakeString.c_str() );		
		return;
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsRightNewID( szGuildName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
		return;
	}

	char szSlangName[MAX_PATH] = "";
	sprintf( szSlangName, "%s", g_SlangUtil.ConvertString( szGuildName.c_str() ) );
	if( szGuildName != szSlangName )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );		
		return;
	}

	if( !IsOnlyEngHanNumText( szGuildName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );		
		return;
	}

	if( g_SlangUtil.IsLimitedGuildName( szGuildName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(14) );		
		return;
	}

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem || !pUserEtcItem->IsCanUse( ioEtcItem::EIT_ETC_GUILD_CREATE ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(15) );		
		return;
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)ioEtcItem::EIT_ETC_GUILD_CREATE;
	kPacket << szGuildName << m_dwMarkID - ID_MARK_SELECT1;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	HideWnd();
}

void GuildCreateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_MARK_SELECT1:
	case ID_MARK_SELECT2:
	case ID_MARK_SELECT3:
	case ID_MARK_SELECT4:
	case ID_MARK_SELECT5:
	case ID_MARK_SELECT6:
	case ID_MARK_SELECT7:
	case ID_MARK_SELECT8:
	case ID_MARK_SELECT9:
	case ID_MARK_SELECT10:
		if( cmd == IOBN_BTNDOWN )
		{
			m_dwMarkID = dwID;
			CheckRadioButton( ID_MARK_SELECT1, ID_MARK_SELECT10, m_dwMarkID );
		}
		break;
	case ID_CREATE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			CreateGuild();	
		}
		break;
	case ID_NAME_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				CreateGuild();	
			}
		}
		break;
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
			}				
			else if( param == IDNO )
			{
				iwm_command( FindChildWnd( ID_EXIT ), IOBN_BTNUP, 0 );
			}
		}
		break;
	}
}

void GuildCreateWnd::iwm_show()
{
	m_dwMarkID = ID_MARK_SELECT1;
	CheckRadioButton( ID_MARK_SELECT1, ID_MARK_SELECT10, m_dwMarkID );

	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	m_pNameEdit = FindChildWnd( ID_NAME_EDIT );
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}
}

void GuildCreateWnd::iwm_hide()
{
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

bool GuildCreateWnd::iwm_esc()
{
	if( ioWnd::iwm_esc() )
	{
		return true;
	}
	return false;
}

void GuildCreateWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void GuildCreateWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 229, iYPos + 234, FONT_SIZE_13, STR(1) );

	if( m_pSlotWhite )
	{
		m_pSlotWhite->SetSize( 21, 21 );
		m_pSlotWhite->Render( iXPos + 106, iYPos + 263 );
		
		DWORD dwMarkID = m_dwMarkID - ID_MARK_SELECT1;
		g_GuildMarkMgr.RenderSmallMarkDefault( dwMarkID, iXPos + 110, iYPos + 267 );

		m_pSlotWhite->SetSize( 223, 21 );
		m_pSlotWhite->Render( iXPos + 129, iYPos + 263 );
	}
}
//////////////////////////////////////////////////////////////////////////
int GuildEventPopup::sg_iEventType = 0;
DWORD GuildEventPopup::sg_dwGuildIndex = 0;
DWORD GuildEventPopup::sg_dwGuildMark = 0;
ioHashString GuildEventPopup::sg_szGuildName = "";
GuildEventPopup::GuildEventPopup()
{
	m_pGuildMarkUpper	= NULL;
}

GuildEventPopup::~GuildEventPopup()
{
	SAFEDELETE( m_pGuildMarkUpper );
}

void GuildEventPopup::SetEventType( int iEventType, DWORD dwGuildIndex, const ioHashString &szGuildName, DWORD dwGuildMark )
{
	sg_iEventType	= iEventType;	
	sg_dwGuildIndex = dwGuildIndex;
	sg_dwGuildMark  = dwGuildMark;
	sg_szGuildName	= szGuildName;
}

void GuildEventPopup::iwm_show()
{
	switch( sg_iEventType )
	{
	case ioGuildInfo::GUILD_JOIN:
	case ioGuildInfo::GUILD_LEAVE:
	case ioGuildInfo::GUILD_SECOND_MASTER:
	case ioGuildInfo::GUILD_MASTER:
	case ioGuildInfo::GUILD_SECOND_MASTER_DISMISS:
	case ioGuildInfo::GUILD_DELETE:
	case ioGuildInfo::GUILD_MARK_BLOCK:
	case ioGuildInfo::GUILD_HOME_ADMIN_SET:
	case ioGuildInfo::GUILD_HOME_ADMIN_RELEASE:
		{
			SetWndPos( Setting::Width() - ( GetWidth() + 1 ), 1 );
		}
		break;
	case ioGuildInfo::GUILD_CREATE:
		{
			AddWndStyle( IWS_MODAL );
			RemoveWndStyle( IWS_CANMOVE );
			SetWndPos( ( Setting::Width() / 2 ) - ( GetWidth() / 2 ), ( Setting::Height() / 2 ) - ( GetHeight() / 2) );
			HeadInsertToModalList();       //모달 윈도우 적용
		}
		break;
	}
}

void GuildEventPopup::iwm_hide()
{
	RemoveWndStyle( IWS_MODAL );
	AddWndStyle( IWS_CANMOVE );
}

bool GuildEventPopup::iwm_esc()
{
	if( ioWnd::iwm_esc() )
	{
		OnOkProcess();
		SetEventType( 0, 0, "", 0 );		
		return true;
	}
	return false;
}

void GuildEventPopup::OnOkProcess()
{
	switch( sg_iEventType )
	{
	case ioGuildInfo::GUILD_CREATE:
	case ioGuildInfo::GUILD_JOIN:
		break;
	}
}

void GuildEventPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			OnOkProcess();
			SetEventType( 0, 0, "", 0 );			
		}
		break;
	case ID_GUILD_VIEW:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			GuildMainWnd *pGuildMainWnd = dynamic_cast<GuildMainWnd *>(g_GUIMgr.FindWnd( GUILD_MAIN_WND ));
			if( pGuildMainWnd )
				pGuildMainWnd->SetGuildIndex( sg_dwGuildIndex );

			SetEventType( 0, 0, "", 0 );
		}
		break;
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			OnOkProcess();
			SetEventType( 0, 0, "", 0 );
		}
		break;
	}
}

void GuildEventPopup::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "GuildMarkUpper" )
	{
		SAFEDELETE( m_pGuildMarkUpper );
		m_pGuildMarkUpper = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void GuildEventPopup::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( sg_iEventType != ioGuildInfo::GUILD_EVENT_NONE )
		{
			ShowWnd();
		}
		return;
	}
}

void GuildEventPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	// 마크
	g_GuildMarkMgr.RenderMarkSize( 73, 73, sg_dwGuildIndex, sg_dwGuildMark, iXPos + 67, iYPos + 54 );
	if( m_pGuildMarkUpper )
	{
		m_pGuildMarkUpper->Render( iXPos, iYPos );
	}

	// 내용
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 173.0f, sg_szGuildName.c_str() );

	switch( sg_iEventType )
	{
	case ioGuildInfo::GUILD_JOIN:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, STR(1) );
			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(2) );	
		}
		break;
	case ioGuildInfo::GUILD_LEAVE:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, STR(3) );	
			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(4) );	
		}
		break;
	case ioGuildInfo::GUILD_SECOND_MASTER:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, STR(5) );	
			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(6) );	
		}
		break;
	case ioGuildInfo::GUILD_MASTER:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, STR(7) );		
			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(8) );
		}
		break;
	case ioGuildInfo::GUILD_SECOND_MASTER_DISMISS:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, STR(9) );		
			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(10) );
		}
		break;
	case ioGuildInfo::GUILD_DELETE:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, STR(11) );		
			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(12) );
		}
		break;
	case ioGuildInfo::GUILD_CREATE:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, STR(13) );		
			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(14) );
		}
		break;
	case ioGuildInfo::GUILD_MARK_BLOCK:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, STR(15) );		
			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(16) );
		}
		break;
	case ioGuildInfo::GUILD_HOME_ADMIN_SET:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, "편집권한이 부여 되었습니다." );		

			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, "길드 본부 편집권한" );
		}
		break;
	case ioGuildInfo::GUILD_HOME_ADMIN_RELEASE:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, "편집권한이 해제 되었습니다." );		
			// 타이틀
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 12, 66, 111 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, "길드 본부 편집권한" );
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
GuildInfoUserOptionWnd::GuildInfoUserOptionWnd()
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
	m_iOverListIndex = -1;
	m_pGuildData = NULL;
	m_dwMsgCmd   = 0;
	m_dwSlotArray= 0;
	m_iUserLevel = 0;
}

GuildInfoUserOptionWnd::~GuildInfoUserOptionWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void GuildInfoUserOptionWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
}

void GuildInfoUserOptionWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void GuildInfoUserOptionWnd::CommandOption( DWORD dwCmd )
{
	if( !m_pGuildData )
		return;

	switch( dwCmd )
	{
	case OPTION_INFO:
		{
			g_App.OpenMannerWnd( m_szUserID.c_str(), false );
		}
		break;
	case OPTION_MASTER:
		{
			if( m_iUserLevel < g_GuildInfo.GetGuildCreateLimitLevel() )
			{
				char szName[MAX_PATH] = "";
				g_LevelMgr.GetGradeName( g_GuildInfo.GetGuildCreateLimitLevel(), szName, sizeof( szName ), false );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szName );
				return;
			}
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(2), m_szUserID.c_str() );
			m_dwMsgCmd = dwCmd;
		}
		break;
	case OPTION_SECOND_MASTER_DISMISSAL:
		{
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(3), m_szUserID.c_str() );
			m_dwMsgCmd = dwCmd;
		}
		break;
	case OPTION_SECOND_MASTER:
		{
			if( m_iUserLevel < g_GuildInfo.GetGuildCreateLimitLevel() )
			{
				char szName[MAX_PATH] = "";
				g_LevelMgr.GetGradeName( g_GuildInfo.GetGuildCreateLimitLevel(), szName, sizeof( szName ), false );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), szName );
				return;
			}
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(5), m_szUserID.c_str() );
			m_dwMsgCmd = dwCmd;
		}
		break;
	case OPTION_KICK_OUT:
		{
			if( g_MyInfo.GetMoney() < g_GuildInfo.GetGuildKickOutPeso() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
				return;
			}
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(7), m_szUserID.c_str(), g_GuildInfo.GetGuildKickOutPeso() );
			m_dwMsgCmd = dwCmd;
		}
		break;
	case OPTION_OPEN_SLOT:
		{			
			if( m_pGuildData->GetGuildMasterName() == g_MyInfo.GetPublicID() )
			{
				TCPNetwork::MouseBusy( true );
				SP2Packet kPacket( CTPK_GUILD_JOINER_CHANGE );
				kPacket << m_pGuildData->GetGuildIndex() << m_dwSlotArray;
				TCPNetwork::SendToServer( kPacket );				
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
			}
		}
		break;
	case OPTION_CLOSE_SLOT:
		{
			if( m_pGuildData->GetGuildMasterName() == g_MyInfo.GetPublicID() )
			{
				if( m_pGuildData->GetGuildJoinUser() >= m_dwSlotArray )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
				}
				else
				{

					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_GUILD_JOINER_CHANGE );
					kPacket << m_pGuildData->GetGuildIndex() << m_dwSlotArray - 1;
					TCPNetwork::SendToServer( kPacket );
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			}
		}
		break;
	}
}

void GuildInfoUserOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param != IDYES )
				return;
			if( !m_pGuildData )
				return;

			switch( m_dwMsgCmd )
			{
			case OPTION_MASTER:
				{
					DWORD dwTargetIndex = m_pGuildData->GetUserIndex( m_szUserID );
					if( dwTargetIndex == 0 )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					else
					{
						SP2Packet kPacket( CTPK_GUILD_MASTER_CHANGE );
						kPacket << dwTargetIndex;
						TCPNetwork::SendToServer( kPacket );
						TCPNetwork::MouseBusy( true );
					}
				}
				break;
			case OPTION_SECOND_MASTER_DISMISSAL:
				{
					DWORD dwTargetIndex = m_pGuildData->GetUserIndex( m_szUserID );
					if( dwTargetIndex == 0 )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
					else
					{
						ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
						if( pLocal )
						{
							SP2Packet kPacket( CTPK_GUILD_POSITION_CHANGE );
							kPacket << dwTargetIndex << m_szUserID << pLocal->GetGuildGeneralPosition();
							TCPNetwork::SendToServer( kPacket );
							TCPNetwork::MouseBusy( true );
						}
					}
				}
				break;
			case OPTION_SECOND_MASTER:
				{
					DWORD dwTargetIndex = m_pGuildData->GetUserIndex( m_szUserID );
					if( dwTargetIndex == 0 )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
					else
					{
						ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
						if( pLocal )
						{
							SP2Packet kPacket( CTPK_GUILD_POSITION_CHANGE );
							kPacket << dwTargetIndex << m_szUserID << pLocal->GetGuildSecondMasterPosition();
							TCPNetwork::SendToServer( kPacket );
							TCPNetwork::MouseBusy( true );
						}
					}
				}
				break;
			case OPTION_KICK_OUT:
				{
					DWORD dwTableIndex = m_pGuildData->GetUserTableIndex( m_szUserID );
					DWORD dwUserIndex  = m_pGuildData->GetUserIndex( m_szUserID );
					if( dwTableIndex == 0 )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
					else
					{
						SP2Packet kPacket( CTPK_GUILD_KICK_OUT );
						kPacket << dwTableIndex << dwUserIndex << m_szUserID << g_GuildInfo.GetGuildKickOutPeso();
						TCPNetwork::SendToServer( kPacket );
						TCPNetwork::MouseBusy( true );
					}
				}
				break;
			}
		}
		break;
	}	
}

void GuildInfoUserOptionWnd::SetOptionInfo( ioGuildData *pGuildData, const ioHashString &szUserID, const ioHashString &szUserPos, int iUserLevel, DWORD dwSlotArray )
{
	HideWnd();
	/*	
	-길드장이 자기 클릭했을 때
	[누구누구 정보]
	-길드장이 부길드장 클릭했을 때
	[누구누구 정보]
	[길드장으로 임명]
	[부길드장 해임]
	[탈퇴 시키기]
	-길드장이 길드원 클릭했을 때
	[누구누구 정보]
	[길드장으로 임명]
	[부길드장으로 임명]
	[탈퇴 시키기]
	-길드장이 빈자리 클릭했을 때
	[닫힘]
	-길드장이 닫힘 클릭했을 때
	[빈자리]
	-부길드장이 자기 클릭했을 때
	[누구누구 정보]
	-부길드장이 길드장 클릭했을 때
	[누구누구 정보]
	-부길드장이 길드원 클릭했을 때
	[누구누구 정보]
	[탈퇴 시키기]
	-부길드장이 빈자리 클릭했을 때
	[길드장 고유 권한 알림]
	-부길드장이 닫힘 클릭했을 때
	[길드장 고유 권한 알림]
	-길드원이 길드장/부길드장/길드원 클릭했을 때
	[누구누구 정보]
	-길드원이 빈자리 클릭했을 때
	[길드장 고유 권한 알림]
	-길드원이 닫힘 클릭했을 때
	[길드장 고유 권한 알림]

	-옵션 명령이 하나밖에 없으면 즉시 실행한다.
	*/
	m_OptionMap.clear();

	m_pGuildData = pGuildData;
	m_szUserID   = szUserID;
	m_szUserPos  = szUserPos;
	m_iUserLevel = iUserLevel;
	m_dwSlotArray= dwSlotArray;

	if( !m_pGuildData )
		return;

	// 정보
	if( !m_szUserID.IsEmpty() )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), m_szUserID.c_str() );
		m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, szTitle ) );
	}

	if( m_szUserID != g_MyInfo.GetPublicID() )
	{
		if( m_pGuildData->GetGuildMasterName() == g_MyInfo.GetPublicID() ) // 길드장
		{
			if( !m_szUserID.IsEmpty() && m_pGuildData->GetGuildSecondMasterName() == m_szUserID )    //부길드장
			{
				m_OptionMap.insert( OptionMap::value_type( OPTION_MASTER, STR(2) ) );
				m_OptionMap.insert( OptionMap::value_type( OPTION_SECOND_MASTER_DISMISSAL, STR(3) ) );
				m_OptionMap.insert( OptionMap::value_type( OPTION_KICK_OUT, STR(4) ) );			
			}
			else if( !m_szUserID.IsEmpty() )     // 길드원
			{
				m_OptionMap.insert( OptionMap::value_type( OPTION_MASTER, STR(5) ) );
				if( m_pGuildData->GetGuildSecondMasterName().IsEmpty() )
					m_OptionMap.insert( OptionMap::value_type( OPTION_SECOND_MASTER, STR(6) ) );
				m_OptionMap.insert( OptionMap::value_type( OPTION_KICK_OUT, STR(7) ) );
			}
		}
		else if( !m_szUserID.IsEmpty() && m_pGuildData->GetGuildSecondMasterName() == g_MyInfo.GetPublicID() ) // 부길드장
		{
			m_OptionMap.insert( OptionMap::value_type( OPTION_KICK_OUT, STR(8) ) );
		}

		// 열림 & 닫힘
		if( m_szUserID.IsEmpty() )
		{
			if( m_dwSlotArray > m_pGuildData->GetGuildMaxUser() )
			{
				m_OptionMap.insert( OptionMap::value_type( OPTION_OPEN_SLOT, STR(9) ) );
			}
			else
			{
				m_OptionMap.insert( OptionMap::value_type( OPTION_CLOSE_SLOT, STR(10) ) );
			}
		}	
	}

	//
	if( m_OptionMap.empty() )
		return;

	if( m_OptionMap.size() == 1 )
	{
		//즉시 실행
		OptionMap::iterator iCreate = m_OptionMap.begin();
		if( GetParent() )
		{
			CommandOption( iCreate->first );
		}
	}
	else
	{
		// 윈도우 사이즈 결정
		int iListWidth = 0;
		OptionMap::iterator iter;
		for( iter = m_OptionMap.begin() ; iter != m_OptionMap.end() ; ++iter )
		{
			int iTextWidth = g_FontMgr.GetTextWidth( iter->second.c_str(), TS_NORMAL, FONT_SIZE_12 );
			if( iListWidth < iTextWidth )
				iListWidth = iTextWidth;
		}
		SetSize( iListWidth + m_iAddWidth + 10, (m_OptionMap.size() * m_iHeightGap) + m_iAddHeight );

		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && GetParent() )
		{
			int iOffsetX = GetParent()->GetDerivedPosX();
			int iOffsetY = GetParent()->GetDerivedPosY();
			if( pMouse->GetMousePos().x + GetWidth() > Setting::Width() )
				SetWndPos( ( pMouse->GetMousePos().x - GetWidth() ) - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
			else
				SetWndPos( pMouse->GetMousePos().x - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
		}
		ShowWnd();
	}
}

void GuildInfoUserOptionWnd::iwm_show()
{
	SetCheckParentOutSide( true );
}

void GuildInfoUserOptionWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void GuildInfoUserOptionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex = -1;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != this )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioLSMouse *pMouse = g_App.GetMouse();

	OptionMap::iterator iter = m_OptionMap.begin();
	for(int iIndex = 0 ; iter != m_OptionMap.end() ; ++iter, ++iIndex )
	{
		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( GetWidth() - m_iAddWidth );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_iOverListIndex  = iIndex;
			if( pMouse->IsLBtnUp() )
			{
				CommandOption( iter->first );
				HideWnd();
			}
			else if( pMouse->IsMouseShowing() )
				pMouse->SetToolTipID( m_szUserID, UserInfoToolTip::TT_GLOBAL );
			break;
		}
	}
}

void GuildInfoUserOptionWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	bool bDarkRender = false;
	OptionMap::iterator iter = m_OptionMap.begin();
	for( int iIndex = 0; iter != m_OptionMap.end() ; ++iter, ++iIndex )
	{
		bDarkRender = !bDarkRender;

		if( m_iOverListIndex == iIndex )
		{
			m_pOverLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintText( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, iter->second.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
GuildInfoUserBtn::GuildInfoUserBtn()
{
	m_pMasterMark		= NULL;
	m_pSecondMasterMark = NULL;
	m_pPeopleOrange		= NULL;
	m_pPeopleGray		= NULL;

	m_pGuildData	= NULL;
	m_dwTableIndex  = 0;
	m_dwUserIndex   = 0;
	m_iUserLevel    = 0;	
	m_dwBtnArray	= 0;
}

GuildInfoUserBtn::~GuildInfoUserBtn()
{
	SAFEDELETE( m_pMasterMark );
	SAFEDELETE( m_pSecondMasterMark );
	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
}

void GuildInfoUserBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "MasterMark" )
	{
		SAFEDELETE( m_pMasterMark );
		m_pMasterMark = pImage;
	}
	else if( szType == "SecondMasterMark" )
	{
		SAFEDELETE( m_pSecondMasterMark );
		m_pSecondMasterMark = pImage;
	}
	else if( szType == "PeopleOrange" )
	{
		SAFEDELETE( m_pPeopleOrange );
		m_pPeopleOrange = pImage;
	}
	else if( szType == "PeopleGray" )
	{
		SAFEDELETE( m_pPeopleGray );
		m_pPeopleGray = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void GuildInfoUserBtn::MemberInfo( ioGuildData *pGuildData, int iListArray, DWORD dwTableIndex, DWORD dwUserIndex, int iUserLevel, const ioHashString &szUserID, const ioHashString &szUserPos )
{
	m_pGuildData	= pGuildData;
	m_dwTableIndex	= dwTableIndex;
	m_dwUserIndex	= dwUserIndex;
	m_iUserLevel    = iUserLevel;
	m_szUserID      = szUserID;
	m_szUserPos     = szUserPos;
	
	if( m_pGuildData == NULL )
		return;

	m_dwBtnArray    = iListArray + 1;
	if( m_szUserID.IsEmpty() )
	{
		if( m_dwBtnArray > g_GuildInfo.GetGuildLevelToMaxEntry( m_pGuildData->GetGuildLevel() ) )
		{
			SetInActive();
		}
		else
		{
			SetActive();
		}
	}
	else
	{
		SetActive();
	}
}

void GuildInfoUserBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void GuildInfoUserBtn::OnRender()
{
	ioButton::OnRender();

	if( !m_pGuildData ) return;

	if( !HasWndStyle( IWS_INACTIVE ) )
	{
		int iXPos, iYPos;
		iXPos = GetDerivedPosX();
		iYPos = GetDerivedPosY();

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		if( m_szUserID.IsEmpty() )
		{
			if( m_dwBtnArray > m_pGuildData->GetGuildMaxUser() )
			{
				if( m_pPeopleGray )
				{
					if( m_bOver || m_bClicked )
					{
						g_FontMgr.SetTextColor( 255, 255, 255 );
						m_pPeopleGray->SetColor( 255, 255, 255 );
						m_pPeopleGray->Render( iXPos + 4, iYPos + 2, UI_RENDER_NORMAL_ADD_COLOR );
					}
					else
					{
						g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
						m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );
					}
				}
				g_FontMgr.PrintText( iXPos + 23, iYPos + 3, FONT_SIZE_12, STR(1) );
			}
			else
			{
				if( m_pPeopleOrange )
				{
					if( m_bOver || m_bClicked )
					{
						g_FontMgr.SetTextColor( 255, 255, 255 );
						m_pPeopleOrange->SetColor( 255, 255, 255 );
						m_pPeopleOrange->Render( iXPos + 4, iYPos + 2, UI_RENDER_NORMAL_ADD_COLOR );
					}
					else
					{
						g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );
						m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
					}
				}
				g_FontMgr.PrintText( iXPos + 23, iYPos + 3, FONT_SIZE_12, STR(2) );
			}
		}
		else
		{
			// 계급
			if( m_bOver || m_bClicked )
			{
				g_LevelMgr.RenderGrade( m_szUserID, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR );
				g_FontMgr.SetTextColor( 255, 255, 255 );
			}
			else
			{
				g_LevelMgr.RenderGrade( m_szUserID, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			}

			// 아이디 / 직책
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
			{
				if( m_szUserPos == pLocal->GetGuildMasterPostion() )
				{
					g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 80.0f, m_szUserID.c_str() );
					int iRealNameSize = min( 80.0f, g_FontMgr.GetTextWidthCutSize( m_szUserID.c_str(), TS_NORMAL, FONT_SIZE_12, 80.0f ) );
					if( m_pMasterMark )
						m_pMasterMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
				}
				else if( m_szUserPos == pLocal->GetGuildSecondMasterPosition() )
				{
					g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 71.0f, m_szUserID.c_str() );
					int iRealNameSize = min( 71.0f, g_FontMgr.GetTextWidthCutSize( m_szUserID.c_str(), TS_NORMAL, FONT_SIZE_12, 71.0f ) );
					if( m_pSecondMasterMark )
						m_pSecondMasterMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
				}
				else
				{
					g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 118.0f, m_szUserID.c_str() );
				}
			}

			// 대회포인트
			g_FontMgr.SetAlignType( TAT_RIGHT );			
			if( m_bOver || m_bClicked )
			{
				g_FontMgr.SetTextColor( 255, 255, 255 );
				int iUserLadderPoint = g_UserInfoMgr.GetLadderPoint( m_szUserID );
				if( iUserLadderPoint == 0 )
				{
					g_FontMgr.PrintText( iXPos + 204, iYPos + 3, FONT_SIZE_12, "- RP" );
				}
				else
				{
					char szConvertText[MAX_PATH] = "";
					Help::ConvertNumToStrComma( iUserLadderPoint, szConvertText, sizeof( szConvertText ) );
					g_FontMgr.PrintText( iXPos + 204, iYPos + 3, FONT_SIZE_12, STR(3), szConvertText );
				}
			}
			else
			{
				int iUserLadderPoint = g_UserInfoMgr.GetLadderPoint( m_szUserID );
				if( iUserLadderPoint == 0 )
				{
					g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
					g_FontMgr.PrintText( iXPos + 204, iYPos + 3, FONT_SIZE_12, "- RP" );
				}
				else
				{
					char szConvertText[MAX_PATH] = "";
					Help::ConvertNumToStrComma( iUserLadderPoint, szConvertText, sizeof( szConvertText ) );
					g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
					g_FontMgr.PrintText( iXPos + 204, iYPos + 3, FONT_SIZE_12, STR(4), szConvertText );
				}
			}			
		}
	}
}
//////////////////////////////////////////////////////////////////////////
GuildInfoUserListWnd::GuildInfoUserListWnd() : m_pGuildData( NULL )
{

}

GuildInfoUserListWnd::~GuildInfoUserListWnd()
{

}

void GuildInfoUserListWnd::UpdateUserList( ioGuildData *pGuildData )
{
	m_pGuildData = pGuildData;
	if( m_pGuildData )
	{
		for(int i = ID_USER_LIST_1;i < ID_USER_LIST_32 + 1;i++ )
		{
			GuildInfoUserBtn *pUserBtn = dynamic_cast<GuildInfoUserBtn*>(FindChildWnd( i ));
			if( pUserBtn )
			{				
				DWORD dwTableIndex = 0;
				DWORD dwUserIndex  = 0;
				int	  iUserLevel   = 0;
				ioHashString szUserID, szUserPos;
				m_pGuildData->GetUserInfo( i, dwTableIndex, dwUserIndex, iUserLevel, szUserID, szUserPos );
				pUserBtn->MemberInfo( m_pGuildData, i, dwTableIndex, dwUserIndex, iUserLevel, szUserID, szUserPos );
			}
		}	
	}
}

void GuildInfoUserListWnd::iwm_show()
{
	if( m_pGuildData )
	{
		m_pGuildData->ProcessUserListSync();
	}
}

bool GuildInfoUserListWnd::iwm_esc()
{
	if( !IsShow() )
		return false;

	GuildInfoUserOptionWnd *pOptionWnd = dynamic_cast<GuildInfoUserOptionWnd *>( FindChildWnd( ID_OPTION_WND ) );
	if( pOptionWnd && pOptionWnd->IsShow() )
	{
		return pOptionWnd->iwm_esc();
	}
	return false;
}

void GuildInfoUserListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if( COMPARE( pWnd->GetID(), ID_USER_LIST_1, ID_USER_LIST_32 + 1 ) )
	{
		if( cmd == IOBN_BTNUP )
		{
			GuildInfoUserBtn *pUserBtn = dynamic_cast<GuildInfoUserBtn *>(pWnd);
			if( pUserBtn )
			{
				GuildInfoUserOptionWnd *pOptionWnd = dynamic_cast<GuildInfoUserOptionWnd*>( FindChildWnd( ID_OPTION_WND ) );
				if( pOptionWnd )
				{
					pOptionWnd->SetOptionInfo( m_pGuildData, pUserBtn->GetUserID(), pUserBtn->GetUserPos(), pUserBtn->GetUserLevel(), pUserBtn->GetSlotArray() );
				}
			}
		}
	}
}

void GuildInfoUserListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( !m_pGuildData ) return;

	if( m_pGuildData->IsUserListUpdate() )
		UpdateUserList( m_pGuildData );
}
//////////////////////////////////////////////////////////////////////////
GuildInfoWnd::GuildInfoWnd() : m_pGuildData( NULL ), m_dwGuildHelpID( 0 )
{
	m_pGuildMarkFrame = NULL;
}

GuildInfoWnd::~GuildInfoWnd()
{
	SAFEDELETE( m_pGuildMarkFrame );
}

void GuildInfoWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "GuildMarkFrame" )
	{
		SAFEDELETE( m_pGuildMarkFrame );
		m_pGuildMarkFrame = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void GuildInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwGuildHelpID = xElement.GetIntAttribute_e( "GuildHelpID" );
}

void GuildInfoWnd::UpdateGuildInfo( ioGuildData *pGuildData )
{
	m_pGuildData = pGuildData;

	if( m_pGuildData == NULL ) 
		return;

	if( g_GuildInfo.IsMyGuild( m_pGuildData->GetGuildIndex() ) )
	{
		HideChildWnd( ID_ENTRY_APP );
		HideChildWnd( ID_ENTRY_CANCEL );
		ShowChildWnd( ID_ENTRY_INVITATION );
		ShowChildWnd( ID_GUILD_LEAVE );
		ShowChildWnd( ID_ENTRY_DELAY_USER );
		ShowChildWnd( ID_GUILD_HEADQUATER );

		ioWnd *pMarkChangeBtn = FindChildWnd( ID_MARK_CHANGE );
		if( pMarkChangeBtn )
		{
			if( g_GuildInfo.IsGuildMaster() || g_GuildInfo.IsGuildSecondMaster() )
			{
				pMarkChangeBtn->SetTitleColor( 0xFF3A3A3A, 0 );
			}
			else
			{
				pMarkChangeBtn->SetTitleColor( 0xFFA7A7A7, 0 );
			}
			pMarkChangeBtn->ShowWnd();
		}

		ioWnd *pTitleChangeBtn = FindChildWnd( ID_TITLE_CHANGE );
		if( pTitleChangeBtn )
		{
			if( g_GuildInfo.IsGuildMaster() )
			{
				pTitleChangeBtn->SetTitleColor( 0xFF3A3A3A, 0 );
			}
			else
			{
				pTitleChangeBtn->SetTitleColor( 0xFFA7A7A7, 0 );
			}
			pTitleChangeBtn->ShowWnd();
		}


		//출석 시간 체크
		if ( g_GuildInfo.CheckEnableAttendTime() )
			SetChildWndStyleRemove( ID_ATTEND_CHECK, IWS_EXACTIVE );
		else
			SetChildWndStyleAdd( ID_ATTEND_CHECK, IWS_EXACTIVE );


		//출석 보상 시간 체크
		if ( m_pGuildData->GetYesterdayAttendedCount() > 0 && g_GuildInfo.CheckEnableAttendRewardTime() )
			SetChildWndStyleRemove( ID_ACCEPT_REWARD, IWS_EXACTIVE );
		else
			SetChildWndStyleAdd( ID_ACCEPT_REWARD, IWS_EXACTIVE );

		ShowChildWnd( ID_RANK_REWARD_INFO );
		ShowChildWnd( ID_ATTEND_REWARD_INFO );
		ShowChildWnd( ID_ATTEND_CHECK );
		ShowChildWnd( ID_ACCEPT_REWARD );
	}
	else
	{		
		HideChildWnd( ID_ENTRY_INVITATION );
		HideChildWnd( ID_GUILD_LEAVE );
		HideChildWnd( ID_ENTRY_DELAY_USER );
		//HideChildWnd( ID_GUILD_HELP );
		HideChildWnd( ID_MARK_CHANGE );
		HideChildWnd( ID_TITLE_CHANGE );
		HideChildWnd( ID_GUILD_HEADQUATER );

		if( g_GuildInfo.IsGuildEntryDelay( m_pGuildData->GetGuildIndex() ) )
		{
			HideChildWnd( ID_ENTRY_APP );
			ShowChildWnd( ID_ENTRY_CANCEL );
		}
		else
		{
			ShowChildWnd( ID_ENTRY_APP );
			HideChildWnd( ID_ENTRY_CANCEL );
		}


		HideChildWnd( ID_RANK_REWARD_INFO );
		HideChildWnd( ID_ATTEND_REWARD_INFO );
		HideChildWnd( ID_ATTEND_CHECK );
		HideChildWnd( ID_ACCEPT_REWARD );
	}

	m_pGuildData->ProcessSync( true );
}

void GuildInfoWnd::iwm_show()
{
	if( g_GuildInfo.CheckMemberAttendInfoRequestTime() )
	{
		SP2Packet kPacket( CTPK_GUILD_MEMBER_ATTEND_RENEWAL );
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
		LOG.PrintTimeAndLog( 0, "%s : Request CTPK_GUILD_MEMBER_ATTEND_RENEWAL, Time : %d " ,__FUNCTION__, g_GuildInfo.GetAttendInfoRequestTime() );
	}
}

void GuildInfoWnd::iwm_hide()
{
	m_pGuildData = NULL;
}

void GuildInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_MARK_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pGuildData )
			{
				if( !g_GuildInfo.IsMyGuild( m_pGuildData->GetGuildIndex() ) )
					return;

				if( !g_GuildInfo.IsGuildMaster() && !g_GuildInfo.IsGuildSecondMaster() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
					return;
				}
			}

			GuildEtcItemPopup *pGuildEtcItem = dynamic_cast<GuildEtcItemPopup *>(g_GUIMgr.FindWnd( GUILD_ETC_ITEM_WND ));
			if( pGuildEtcItem )
				pGuildEtcItem->ShowGuildMarkChange();
		}
		break;
	case ID_TITLE_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pGuildData )
			{
				if( g_GuildInfo.IsMyGuild( m_pGuildData->GetGuildIndex() ) && g_GuildInfo.IsGuildMaster() )
				{
					g_GUIMgr.ShowWnd( GUILD_TITLE_CHANGE_WND );
				}
				else
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
		}
		break;
	case ID_ENTRY_APP:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pGuildData )
			{				
				// 가입 신청
				if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
					g_MyInfo.IsExpertEntryTemporary() )
				{
					g_App.OpenFormalityEntryPage();
				}
				else if( g_GuildInfo.IsGuild() )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				else if( g_GuildInfo.IsGuildEntryDelay( m_pGuildData->GetGuildIndex() ) )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				else if( m_pGuildData->GetGuildJoinUser() >= m_pGuildData->GetGuildMaxUser() )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
				else
				{
					HideChildWnd( ID_ENTRY_APP );
					ShowChildWnd( ID_ENTRY_CANCEL );

					//가입 신청 패킷 전송
					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_GUILD_ENTRY_APP );
					kPacket << m_pGuildData->GetGuildIndex() <<  m_pGuildData->GetGuildMaster() << m_pGuildData->GetGuildSecondMaster();
					TCPNetwork::SendToServer( kPacket );
				}
			}			
		}
		break;
	case ID_ENTRY_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pGuildData )
			{
				ShowChildWnd( ID_ENTRY_APP );
				HideChildWnd( ID_ENTRY_CANCEL );

				//가입 취소 패킷 전송
				SP2Packet kPacket( CTPK_GUILD_ENTRY_CANCEL );
				kPacket << m_pGuildData->GetGuildIndex();
				TCPNetwork::SendToServer( kPacket );			

				g_GuildInfo.DeleteEntryDelayGuildList( m_pGuildData->GetGuildIndex() );
			}
		}
		break;
	case ID_ENTRY_INVITATION:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.ShowWnd( GUILD_INVITATION_WND );
		}
		break;
	case ID_GUILD_LEAVE:
		if( cmd == IOBN_BTNUP )
		{
			GuildMainWnd *pGuildMainWnd = dynamic_cast<GuildMainWnd*>( GetParent() );
			if( pGuildMainWnd )
				pGuildMainWnd->iwm_command( this, GuildMainWnd::GUILD_LEAVE, 0 );
		}
		break;
	case ID_ENTRY_DELAY_USER:
		if( cmd == IOBN_BTNUP )
		{
			SP2Packet kPacket( CTPK_GUILD_ENTRY_DELAY_MEMBER );
			TCPNetwork::SendToServer( kPacket );

			GuildEntryDelayUserWnd *pDelayUserWnd = dynamic_cast<GuildEntryDelayUserWnd *>(g_GUIMgr.FindWnd( GUILD_ENTRY_DELAY_USER_WND ) );
			if( pDelayUserWnd )
			{
				pDelayUserWnd->SetAutoShow( false );
				pDelayUserWnd->ShowWnd();
			}
		}
		break;
	case ID_GUILD_HOMEPAGE:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pGuildData )
				g_App.OpenGuildHomePage( m_pGuildData->GetGuildIndex() );
		}
		break;
		/*
	case ID_GUILD_HELP:
		if( cmd == IOBN_BTNUP )
		{
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( m_dwGuildHelpID );
		}
		break;
		*/
		//길드 출석 관련 UI
	case ID_RANK_REWARD_INFO:
		if( cmd == IOBN_BTNUP )
		{
			GuildPossibleRewardListWnd *pRewardList = dynamic_cast<GuildPossibleRewardListWnd *>( g_GUIMgr.FindWnd( GUILD_REWARD_WND ) );
			if ( pRewardList )
			{
				if ( pRewardList && !pRewardList->IsShow() )
					pRewardList->ShowRewardWnd( RANK_REWARD );
			}
		}
		break;
	case ID_ATTEND_REWARD_INFO:
		if( cmd == IOBN_BTNUP )
		{
			GuildPossibleRewardListWnd *pRewardList = dynamic_cast<GuildPossibleRewardListWnd *>( g_GUIMgr.FindWnd( GUILD_REWARD_WND ) );
			if ( pRewardList && !pRewardList->IsShow() )
					pRewardList->ShowRewardWnd( ATTEND_REWARD );
		}
		break;
	case ID_ATTEND_CHECK:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GuildInfo.CheckMemberAttendInfoRequestTime() )
			{
				SP2Packet kPacket( CTPK_GUILD_MEMBER_ATTEND_RENEWAL );
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "출석 정보 갱신 완료 # 출석 버튼을 다시 눌러주시기 바랍니다." );
				LOG.PrintTimeAndLog( 0, "%s : Request CTPK_GUILD_MEMBER_ATTEND_RENEWAL, Time : %d " ,__FUNCTION__, g_GuildInfo.GetAttendInfoRequestTime() );
			}
			else
			{
				SP2Packet kPacket( CTPK_GUILD_ATTEND );
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
				LOG.PrintTimeAndLog( 0, "%s - Attend Btn Down. Enable Attend Time : %d", __FUNCTION__, g_GuildInfo.GetEnableAttendTime() );
			}
		}
		else if( cmd == IOEX_BTNUP )
		{
			if ( g_GuildInfo.CheckGuildCreatedTime() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드 출석은 가입 후#다음날(새벽 5시)부터 가능합니다." );
			else if ( !g_GuildInfo.CheckEnableAttendTime() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 출석 하셨습니다." );
		}
		break;
	case ID_ACCEPT_REWARD:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GuildInfo.CheckMemberAttendInfoRequestTime() )
			{
				SP2Packet kPacket( CTPK_GUILD_MEMBER_ATTEND_RENEWAL );
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "출석 정보 갱신 완료 # 출석 버튼을 다시 눌러주시기 바랍니다." );
				LOG.PrintTimeAndLog( 0, "%s : Request CTPK_GUILD_MEMBER_ATTEND_RENEWAL, Time : %d " ,__FUNCTION__, g_GuildInfo.GetAttendInfoRequestTime() );
			}
			else
			{
				SP2Packet kPacket( CTPK_RECV_GUILDATTEND_REWARD );
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
				LOG.PrintTimeAndLog( 0, "%s - Reward Btn Down. Enable Attend Reward Time : %d", __FUNCTION__, g_GuildInfo.GetEnableAttendRewardTime() );
			}
		}
		//반비활성화
		else if ( cmd == IOEX_BTNUP)
		{
			if ( g_GuildInfo.CheckGuildCreatedTime() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "출석 보상 받기는 가입 후 다음날#(새벽 5시)부터 가능합니다." );
			else if ( m_pGuildData->GetYesterdayAttendedCount() <= 0 )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "어제 출석한 인원이 없습니다." );
			else if ( !g_GuildInfo.CheckEnableAttendRewardTime() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 보상 받으셨습니다." );
		}
		break;
	case ID_GUILD_HEADQUATER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_App.IsMouseBusy() )
				return;	

			if( g_BattleRoomMgr.IsBattleRoom() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투 중에는 입장이 불가능합니다." );
				return;
			}

			if( g_LadderTeamMgr.IsLadderTeam() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영팀 중에는 입장이 불가능합니다" );
				return;
			}

			if( g_ShuffleRoomMgr.IsShuffleRoom() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "오늘의 모드 중에는 입장이 불가능합니다" );
				return;
			}
			if( g_RankBattleMgr.IsRankBattlePlaying() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "랭킹전 중에는 입장이 불가능합니다" );
				return;
			}

			SP2Packet kPacket( CTPK_ENTER_GUILD_ROOM );
			kPacket << 16;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		else if( cmd == IOEX_BTNUP )
		{	
			if( m_pGuildData )
			{
				if( m_pGuildData->GetGuildMasterName() == g_MyInfo.GetPublicID() )
				{
					GuildHQEtcItemNeedPopup* pWnd = dynamic_cast<GuildHQEtcItemNeedPopup*>( g_GUIMgr.FindWnd( GUILD_HQ_NEED_POPUP ) );
					if( pWnd )
					{
						pWnd->SetType( GuildHQEtcItemNeedPopup::PUT_NEED_HQ );
						ioUserEtcItem* pInven = g_MyInfo.GetUserEtcItem();
						if( pInven )
						{
							ioUserEtcItem::ETCITEMSLOT Slot;
							if( pInven->GetEtcItem( ioEtcItem::EIT_ETC_CREATE_GUILD_HQ, Slot ) && Slot.m_iValue1 > 0 )
							{
								pWnd->SetType( GuildHQEtcItemNeedPopup::PUT_USE_HQ );
							}
						}
						pWnd->ShowWnd();
					}
				}
				else
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드장만이 활성화 할 수 있는 기능입니다." );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "가입된 길드가 없습니다." );
			}
		}
		break;
	}
}

void GuildInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	SetChildWndStyleRemove( ID_GUILD_HEADQUATER, IWS_EXACTIVE );
	if( !m_pGuildData || !m_pGuildData->GetGuildRoomOpen() )
	{		
		SetChildWndStyleAdd( ID_GUILD_HEADQUATER, IWS_EXACTIVE );
	}
}

void GuildInfoWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pGuildData ) return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderTitle( iXPos, iYPos );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 118, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 139, FONT_SIZE_12, STR(3) );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 160, FONT_SIZE_12, "현재인원" );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 181, FONT_SIZE_12, "최대인원" );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 202, FONT_SIZE_12, STR(5) );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 223, FONT_SIZE_12, STR(6) );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 244, FONT_SIZE_12, STR(7) );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 265, FONT_SIZE_12, STR(8) );

	// 길드 명
	int iRealGuildName = 0;
	if( g_GuildInfo.IsMyGuild( m_pGuildData->GetGuildIndex() ) )
	{
		char szGuildName[MAX_PATH] = ""; 
		if( m_pGuildData->GetGuildMasterName() == g_MyInfo.GetPublicID() )
			SafeSprintf( szGuildName, sizeof( szGuildName ), STR(9), m_pGuildData->GetGuildName().c_str() );
		else if( m_pGuildData->GetGuildSecondMasterName() == g_MyInfo.GetPublicID() )
			SafeSprintf( szGuildName, sizeof( szGuildName ), STR(10), m_pGuildData->GetGuildName().c_str() );
		else
			SafeSprintf( szGuildName, sizeof( szGuildName ), STR(11), m_pGuildData->GetGuildName().c_str() );

		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.PrintText( iXPos + 434, iYPos + 118, FONT_SIZE_12, szGuildName );

		iRealGuildName = g_FontMgr.GetTextWidth( szGuildName, TS_NORMAL, FONT_SIZE_12 ) + 16;    // 16은 길드 마크와 갭
	}
	else
	{
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.PrintText( iXPos + 434, iYPos + 118, FONT_SIZE_12, m_pGuildData->GetGuildName().c_str() );

		iRealGuildName = g_FontMgr.GetTextWidth( m_pGuildData->GetGuildName().c_str(), TS_NORMAL, FONT_SIZE_12 ) + 16;    // 16은 길드 마크와 갭
	}
	g_GuildMarkMgr.RenderSmallMark( m_pGuildData->GetGuildIndex(), m_pGuildData->GetGuildMark(), iXPos + 434 - iRealGuildName, iYPos + 119 );
		
	// 등급 표시 F - E - D - C - B - A - S
	enum{ MAX_GUILD_LEVEL = 7, MAX_LEVEL = 2, };
	if( COMPARE( m_pGuildData->GetGuildLevel(), 0, MAX_GUILD_LEVEL ) )
	{
		char szLevelText[MAX_PATH] = "";
		char szLevel[MAX_GUILD_LEVEL][MAX_LEVEL] = { "F", "E", "D", "C", "B", "A", "S" };
		SafeSprintf( szLevelText, sizeof( szLevelText ), STR(14), szLevel[m_pGuildData->GetGuildLevel()] );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 434, iYPos + 139, FONT_SIZE_12, szLevelText );
	}

	// 현재인원
	int iJoinEntry = m_pGuildData->GetGuildJoinUser();
	int iMaxEntry  = m_pGuildData->GetGuildMaxUser();
	int iLevelToMaxEntry = g_GuildInfo.GetGuildLevelToMaxEntry( m_pGuildData->GetGuildLevel() );
	if( iJoinEntry > iLevelToMaxEntry )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 434, iYPos + 160, FONT_SIZE_12, "%d 명 (초과)", iJoinEntry );			
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 434, iYPos + 160, FONT_SIZE_12, "%d 명", iJoinEntry );			
	}

	// 최대 인원
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	// 길드장이 길드원의 최대인원을 제한할 수 있고, 길드에 따른 최대 인원이 정해져있기 때문에 min을 사용.
	g_FontMgr.PrintText( iXPos + 434, iYPos + 181, FONT_SIZE_12, "%d 명", min( iMaxEntry, iLevelToMaxEntry ) );

	// 랭킹
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 434, iYPos + 202, FONT_SIZE_12, STR(17), m_pGuildData->GetGuildRank() );

	// 누적 길드 대회포인트
	char szConvertText[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_pGuildData->GetGuildPoint(), szConvertText, sizeof( szConvertText ) );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 434, iYPos + 223, FONT_SIZE_12, STR(18), szConvertText );

	// 시즌 대회 포인트
	Help::ConvertNumToStrComma( m_pGuildData->GetGuildCurPoint(), szConvertText, sizeof( szConvertText ) );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 434, iYPos + 244, FONT_SIZE_12, STR(19), szConvertText );

	// 길드 보너스
	float fBonus = m_pGuildData->GetGuildBonus() * FLOAT100;
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 434, iYPos + 265, FONT_SIZE_12, STR(20), (int)fBonus, (int)fBonus );
}

void GuildInfoWnd::OnRenderTitle( int iXPos, int iYPos )
{
	if( !m_pGuildData ) return;

	g_GuildMarkMgr.RenderMarkSize( 100, 100, m_pGuildData->GetGuildIndex(), m_pGuildData->GetGuildMark(), iXPos + 17, iYPos );
	if( m_pGuildMarkFrame )
	{
		m_pGuildMarkFrame->SetSize( 104, 104 );
		m_pGuildMarkFrame->Render( iXPos +15, iYPos - 2 );
	}

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	char szGuildName[MAX_PATH] = "";
	SafeSprintf( szGuildName, sizeof( szGuildName ), STR(1), m_pGuildData->GetGuildName().c_str() );
	g_FontMgr.PrintTextWidthCut( iXPos + 123, iYPos - 2, FONT_SIZE_16, 218.0f, szGuildName );
	int iRealGuildName = min( 218.0f, g_FontMgr.GetTextWidth( szGuildName, TS_NORMAL, FONT_SIZE_16 ) );
	char szSince[MAX_PATH] = "";
	sprintf( szSince, "Since %d.%d.%d", m_pGuildData->GetGuildRegYear(), m_pGuildData->GetGuildRegMonth(), m_pGuildData->GetGuildRegDay() );
    g_FontMgr.PrintText( iXPos + iRealGuildName + 127, iYPos + 2, FONT_SIZE_12, szSince );

	//길드 소개
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	char szGuildTitle[MAX_PATH] = "";
	sprintf( szGuildTitle, "%s", m_pGuildData->GetGuildTitle().c_str() );	
	int iLen = (int)strlen( szGuildTitle );
	
	int iResult = 0;
	char szDst[MAX_PATH] = "";
	memset( szDst, 0, sizeof( szDst ) );
	// Line 1
	iResult += Help::StringCutFun( FONT_SIZE_12, 318.0f, TS_NORMAL, szDst, sizeof(szDst), &szGuildTitle[iResult] );
	g_FontMgr.PrintText( iXPos + 123, iYPos + 22, FONT_SIZE_12, szDst );
	// Line 2
	if( iResult < iLen )
	{
		iResult += Help::StringCutFun( FONT_SIZE_12, 318.0f, TS_NORMAL, szDst, sizeof(szDst), &szGuildTitle[iResult] );
		g_FontMgr.PrintText( iXPos + 123, iYPos + 42, FONT_SIZE_12, szDst );
	}
    // Line 3
	if( iResult < iLen )
		g_FontMgr.PrintTextWidthCut( iXPos + 123, iYPos + 62, FONT_SIZE_12, 318.0f, &szGuildTitle[iResult] );	
}
//////////////////////////////////////////////////////////////////////////
GuildMainWnd::GuildMainWnd()
{
	m_dwTabID		= ID_GUILD_INFO_BTN;
	m_dwGuildIndex	= 0;

	m_pGuildData	= NULL;
	m_pTabLine		= NULL;

	m_bIsMyGuild	= false;
}

GuildMainWnd::~GuildMainWnd()
{
	SAFEDELETE( m_pTabLine );
}

void GuildMainWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/guildinfownd.xml", this );
	g_GUIMgr.AddWnd( "XML/guildinfouserlistwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/guildattendinfolistwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/guildhomeadmininfolistwnd.xml", this );
}

void GuildMainWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TapLine" )
	{
		SAFEDELETE( m_pTabLine );
		m_pTabLine = pImage;
	}
	else
		ioMovingWnd::AddRenderImage( szType, pImage );
}

void GuildMainWnd::iwm_show()
{
	g_GUIMgr.HideWnd( GUILD_TITLE_CHANGE_WND );
}

void GuildMainWnd::iwm_hide()
{
	m_pGuildData = NULL;
}

bool GuildMainWnd::iwm_esc()
{
	if( !IsShow() )
		return false;

	if( m_dwTabID == ID_GUILD_USER_BTN )
	{
		GuildInfoUserListWnd *pUserWnd = dynamic_cast<GuildInfoUserListWnd *>( FindChildWnd( ID_GUILD_USER_WND ) );
		if( pUserWnd && pUserWnd->IsShow() )
		{
			if( pUserWnd->iwm_esc() )
				return true;
		}
	}

	return ioWnd::iwm_esc();
}

void GuildMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_GUILD_INFO_BTN:
	case ID_GUILD_INFO_BTN2:
	case ID_GUILD_USER_BTN:
	case ID_GUILD_USER_BTN2:
	case ID_GUILD_ATTEND_LIST_BTN:
	case ID_GUILD_HOME_ADMIN_LIST_BTN:
		if( cmd == IOBN_BTNDOWN )
			SetRadioButton( dwID );
		break;
	case ID_GUILD_INFO_WND:
		if( cmd == GUILD_LEAVE )
		{
			GuildLeaveProcess();
		}
		break;
	case ID_GUILD_HOUSING_INVEN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GUIMgr.IsShow( HOUSING_BLOCK_INVEN_WND ) )
			{
				g_GUIMgr.HideWnd( HOUSING_BLOCK_INVEN_WND );
			}
			else
			{
				ioHousingBlockInvenWnd* pWnd = dynamic_cast<ioHousingBlockInvenWnd*>( g_GUIMgr.FindWnd( HOUSING_BLOCK_INVEN_WND ) );
				if( pWnd )
				{
					pWnd->SetBlockModeType( BMT_GUILD );
					pWnd->ShowWnd();
				}

			}
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( g_GuildInfo.IsGuild() )
				{
					ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
					if( !pGuildData ) 
						return;

					if( g_GuildInfo.IsGuildMaster() && pGuildData->GetGuildJoinUser() > 1 )						
						return;

					int iLeavePeso = g_GuildInfo.GetGuildLeavePeso();
					if( g_GuildInfo.IsGuildMaster() )
						iLeavePeso = 0;

					SP2Packet kPacket( CTPK_GUILD_LEAVE );
					kPacket << iLeavePeso;     //차감 페소
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
		}
		break;
	}
}

void GuildMainWnd::SetRadioButton( DWORD dwTabID )
{
	m_dwTabID = dwTabID;
	if ( m_bIsMyGuild )
	{
		CheckRadioButton( ID_GUILD_INFO_BTN2, ID_GUILD_HOME_ADMIN_LIST_BTN, dwTabID );
		HideChildWnd( ID_GUILD_INFO_BTN );
		HideChildWnd( ID_GUILD_USER_BTN );
		ShowChildWnd( ID_GUILD_INFO_BTN2 );
		ShowChildWnd( ID_GUILD_USER_BTN2 );
		ShowChildWnd( ID_GUILD_ATTEND_LIST_BTN );
		ShowChildWnd( ID_GUILD_HOME_ADMIN_LIST_BTN );
		ShowChildWnd( ID_GUILD_HOUSING_INVEN );

		SetChildInActive( ID_GUILD_HOME_ADMIN_LIST_BTN );
		SetChildInActive( ID_GUILD_HOUSING_INVEN );		
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( pGuildData && pGuildData->GetGuildRoomOpen() )		
		{
			SetChildActive( ID_GUILD_HOME_ADMIN_LIST_BTN );
			SetChildActive( ID_GUILD_HOUSING_INVEN );
		}
	}
	else
	{
		CheckRadioButton( ID_GUILD_INFO_BTN, ID_GUILD_HOME_ADMIN_LIST_BTN, dwTabID );
		HideChildWnd( ID_GUILD_INFO_BTN2 );
		HideChildWnd( ID_GUILD_USER_BTN2 );
		HideChildWnd( ID_GUILD_ATTEND_LIST_BTN );
		HideChildWnd( ID_GUILD_HOME_ADMIN_LIST_BTN );
		HideChildWnd( ID_GUILD_HOUSING_INVEN );
		ShowChildWnd( ID_GUILD_INFO_BTN );
		ShowChildWnd( ID_GUILD_USER_BTN );
	}

	HideChildWnd( ID_GUILD_INFO_WND );
	HideChildWnd( ID_GUILD_USER_WND );
	HideChildWnd( ID_GUILD_ATTEND_WND );
	HideChildWnd( ID_GUILD_ADMIN_WND );
	switch( m_dwTabID )
	{
	case ID_GUILD_INFO_BTN:
	case ID_GUILD_INFO_BTN2:
		{
			GuildInfoWnd *pInfoWnd = dynamic_cast<GuildInfoWnd*>( FindChildWnd( ID_GUILD_INFO_WND ) );
			if( pInfoWnd )
			{
				pInfoWnd->UpdateGuildInfo( m_pGuildData );
				pInfoWnd->ShowWnd();
			}
		}
		break;
	case ID_GUILD_USER_BTN:
	case ID_GUILD_USER_BTN2:
		{
			GuildInfoUserListWnd *pUserWnd = dynamic_cast<GuildInfoUserListWnd *>( FindChildWnd( ID_GUILD_USER_WND ) );
			if( pUserWnd )
			{
				pUserWnd->UpdateUserList( m_pGuildData );
				pUserWnd->ShowWnd();
			}
		}
		break;
	case ID_GUILD_ATTEND_LIST_BTN:
		{
			GuildAttendInfoListWnd *pAttendWnd = dynamic_cast<GuildAttendInfoListWnd *>( FindChildWnd( ID_GUILD_ATTEND_WND ) );
			if( pAttendWnd )
			{
				pAttendWnd->UpdateUserList( m_pGuildData );
				pAttendWnd->ShowWnd();
			}
		}
		break;
	case ID_GUILD_HOME_ADMIN_LIST_BTN:
		{
			GuildHomeAdminInfoListWnd *pAdminWnd = dynamic_cast<GuildHomeAdminInfoListWnd *>( FindChildWnd( ID_GUILD_ADMIN_WND ) );
			if( pAdminWnd )
			{
				pAdminWnd->UpdateUserList( m_pGuildData );
				pAdminWnd->ShowWnd();
			}
		}
		break;
	}
}

void GuildMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if ( m_bIsMyGuild )
	{
		SetChildInActive( ID_GUILD_HOME_ADMIN_LIST_BTN );
		SetChildInActive( ID_GUILD_HOUSING_INVEN );

		bool bIsArena = false;
		if( ioPlayMode::GetModeType() == MT_ARENA && g_BattleRoomMgr.IsBattleModePlaying() )
			bIsArena = true;

		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !bIsArena && pGuildData && pGuildData->GetGuildRoomOpen() )		
		{
			SetChildActive( ID_GUILD_HOME_ADMIN_LIST_BTN );
			SetChildActive( ID_GUILD_HOUSING_INVEN );
		}
	}
}

void GuildMainWnd::SetGuildIndex( DWORD dwGuildIndex )
{	
	m_pGuildData = g_GuildInfo.GetGuildData( dwGuildIndex, true );
	if( m_pGuildData == NULL ) 
		return;

	m_dwGuildIndex	= dwGuildIndex;

	DWORD dwTabID = 0;
	if ( g_GuildInfo.IsMyGuild( m_pGuildData->GetGuildIndex() ) )
	{
		dwTabID = ID_GUILD_INFO_BTN2;
		m_bIsMyGuild = true;
	}
	else
	{
		dwTabID = ID_GUILD_INFO_BTN;
		m_bIsMyGuild = false;
	}
	SetRadioButton( dwTabID );
	UpdateGuildInfo();
	ShowWnd();
}

void GuildMainWnd::UpdateGuildInfo()
{
	switch( m_dwTabID )
	{
	case ID_GUILD_INFO_BTN:
	case ID_GUILD_INFO_BTN2:
		{
			GuildInfoWnd *pInfoWnd = dynamic_cast<GuildInfoWnd*>( FindChildWnd( ID_GUILD_INFO_WND ) );
			if( pInfoWnd )
			{
				pInfoWnd->UpdateGuildInfo( m_pGuildData );
			}
		}
		break;
	case ID_GUILD_USER_BTN:
	case ID_GUILD_USER_BTN2:
		{
			GuildInfoUserListWnd *pUserWnd = dynamic_cast<GuildInfoUserListWnd *>( FindChildWnd( ID_GUILD_USER_WND ) );
			if( pUserWnd )
			{
				pUserWnd->UpdateUserList( m_pGuildData );
			}
		}
		break;
	case ID_GUILD_HOME_ADMIN_LIST_BTN:
		{
			GuildHomeAdminInfoListWnd *pAdminWnd = dynamic_cast<GuildHomeAdminInfoListWnd *>( FindChildWnd( ID_GUILD_ADMIN_WND ) );
			if( pAdminWnd )
			{
				pAdminWnd->UpdateUserList( m_pGuildData );
			}
		}
		break;
	}
	UpdateTitle();
}

void GuildMainWnd::UpdateTitle()
{
	if( m_pGuildData == NULL ) 
		return;

	// 타이틀 변경
	char szTitle[MAX_PATH] = "";
	SafeSprintf( szTitle, sizeof( szTitle ), "%s 길드", m_pGuildData->GetGuildName().c_str() );
	SetTitleText( szTitle );


	if( g_GuildInfo.IsMyGuild( m_pGuildData->GetGuildIndex() ) )
	{
		// 길드원 탭 버튼 변경
		ioWnd *pGuildUserBtn = FindChildWnd( ID_GUILD_USER_BTN2 );
		if( pGuildUserBtn )
		{
			int iJoinEntry = m_pGuildData->GetGuildJoinUser();
			int iMaxEntry  = m_pGuildData->GetGuildMaxUser();
			int iLevelToMaxEntry = g_GuildInfo.GetGuildLevelToMaxEntry( m_pGuildData->GetGuildLevel() );
			if( iJoinEntry > iLevelToMaxEntry )
			{		
				SafeSprintf( szTitle, sizeof( szTitle ), "길드원 %d/%d", iJoinEntry, iLevelToMaxEntry );
				pGuildUserBtn->SetTitleColor( 0xFFB91600, 0 );
			}
			else
			{
				SafeSprintf( szTitle, sizeof( szTitle ), "길드원 %d/%d", iJoinEntry, iMaxEntry );
				pGuildUserBtn->SetTitleColor( 0xFF3A3A3A, 0 );	
			}
			pGuildUserBtn->SetTitleText( szTitle );
		}

		ioWnd *pGuildAttendListBtn = FindChildWnd( ID_GUILD_ATTEND_LIST_BTN );
		if( pGuildAttendListBtn )
		{
			int iJoinEntry = m_pGuildData->GetGuildJoinUser();
			int iTodayAttendCount  = m_pGuildData->GetTodayAttendedCount();
			SafeSprintf( szTitle, sizeof( szTitle ), "출석 정보 %d/%d", iTodayAttendCount, iJoinEntry );	
			pGuildAttendListBtn->SetTitleColor( 0xFF3A3A3A, 0 );	
			pGuildAttendListBtn->SetTitleText( szTitle );
		}
	}
	else
	{
		// 길드원 탭 버튼 변경
		ioWnd *pGuildUserBtn = FindChildWnd( ID_GUILD_USER_BTN );
		if( pGuildUserBtn )
		{
			int iJoinEntry = m_pGuildData->GetGuildJoinUser();
			int iMaxEntry  = m_pGuildData->GetGuildMaxUser();
			int iLevelToMaxEntry = g_GuildInfo.GetGuildLevelToMaxEntry( m_pGuildData->GetGuildLevel() );
			if( iJoinEntry > iLevelToMaxEntry )
			{		
				SafeSprintf( szTitle, sizeof( szTitle ), "길드원 %d/%d", iJoinEntry, iLevelToMaxEntry );
				pGuildUserBtn->SetTitleColor( 0xFFB91600, 0 );
			}
			else
			{
				SafeSprintf( szTitle, sizeof( szTitle ), "길드원 %d/%d", iJoinEntry, iMaxEntry );	
				pGuildUserBtn->SetTitleColor( 0xFF3A3A3A, 0 );	
			}
			pGuildUserBtn->SetTitleText( szTitle );
		}
	}
	
}

DWORD GuildMainWnd::GetGuildIndex()
{
	if( !m_pGuildData ) 
		return 0;

	return m_pGuildData->GetGuildIndex();
}

void GuildMainWnd::GuildLeaveProcess()
{
	if( !g_GuildInfo.IsGuild() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else if( g_GuildInfo.IsGuildMaster() )
	{
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( pGuildData && pGuildData->GetGuildJoinUser() == 1 )         //1명일경우 길드 탈퇴 가능
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(2) );					
		else
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(4), g_GuildInfo.GetGuildLeavePeso() );					
	}			
}

void GuildMainWnd::OnRender()
{
	ioMovingWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	//탭라인 Render
	if ( m_pTabLine )
	{
		if ( m_bIsMyGuild )
		{
			m_pTabLine->Render( iXPos + 123, iYPos + 79 );
			m_pTabLine->Render( iXPos + 230, iYPos + 79 );
			m_pTabLine->Render( iXPos + 337, iYPos + 79 );
		}
		else
			m_pTabLine->Render( iXPos + 229, iYPos + 79 );
	}
}
//////////////////////////////////////////////////////////////////////////
GuildEntryDelayUserBtn::GuildEntryDelayUserBtn()
{
	m_dwMsgBoxID     = 0;
	m_dwGuildIndex   = 0;
	m_dwTableIndex   = 0;
	m_dwUserIndex    = 0;
	m_iUserLevel     = 0;
	m_iMaxList		 = 0;
}

GuildEntryDelayUserBtn::~GuildEntryDelayUserBtn()
{
}

void GuildEntryDelayUserBtn::MemberInfo( int iMaxList, DWORD dwGuildIndex, DWORD dwTableIndex, DWORD dwUserIndex, int iUserLevel, const ioHashString &szUserID )
{
	m_iMaxList      = iMaxList;
	m_dwGuildIndex	= dwGuildIndex;
	m_dwTableIndex	= dwTableIndex;
	m_dwUserIndex	= dwUserIndex;
	m_iUserLevel    = iUserLevel;
	m_szUserID      = szUserID;

	if( m_dwGuildIndex == 0 || !g_GuildInfo.IsMyGuild( m_dwGuildIndex ) || m_szUserID.IsEmpty() )
	{
		SetInActive();
		HideChildWnd( ID_AGREE );
		HideChildWnd( ID_REFUSE );
	}
	else 
	{
		SetActive();
		ShowChildWnd( ID_AGREE );
		ShowChildWnd( ID_REFUSE );
		if( !g_GuildInfo.IsGuildMaster() && !g_GuildInfo.IsGuildSecondMaster() )
		{
			SetChildInActive( ID_AGREE );
			SetChildInActive( ID_REFUSE );
		}
		else
		{
			SetChildActive( ID_AGREE );
			SetChildActive( ID_REFUSE );
		}
	}
	m_dwMsgBoxID = 0;
}

void GuildEntryDelayUserBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_AGREE:
		if( cmd == IOBN_BTNUP )
		{
			if( !g_GuildInfo.IsMyGuild( m_dwGuildIndex ) )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			else if( !g_GuildInfo.IsGuildMaster() && !g_GuildInfo.IsGuildSecondMaster() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			else
			{
				m_dwMsgBoxID = ID_AGREE;
				g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(3), m_szUserID.c_str() );
			}
		}
		break;
	case ID_REFUSE:
		if( cmd == IOBN_BTNUP )
		{
			if( !g_GuildInfo.IsMyGuild( m_dwGuildIndex ) )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			else if( !g_GuildInfo.IsGuildMaster() && !g_GuildInfo.IsGuildSecondMaster() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			else
			{
				m_dwMsgBoxID = ID_REFUSE;
				g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(6), m_szUserID.c_str() );		
			}
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( m_dwMsgBoxID == ID_AGREE )
				{
					if( !g_GuildInfo.IsMyGuild( m_dwGuildIndex ) )
						return;
					else if( !g_GuildInfo.IsGuildMaster() && !g_GuildInfo.IsGuildSecondMaster() )
						return;
					else
					{
						// 패킷 처리
						TCPNetwork::MouseBusy( true );
						SP2Packet kPacket( CTPK_GUILD_ENTRY_AGREE );
						kPacket << m_dwGuildIndex << m_dwUserIndex;
						TCPNetwork::SendToServer( kPacket );

						// UI 처리
						GuildEntryDelayUserWnd *pParent = dynamic_cast<GuildEntryDelayUserWnd*>(GetParent());
						if( pParent )
							pParent->DeleteUserList( m_dwTableIndex );
					}
				}
				else if( m_dwMsgBoxID == ID_REFUSE )
				{
					if( !g_GuildInfo.IsMyGuild( m_dwGuildIndex ) )
						return;
					else if( !g_GuildInfo.IsGuildMaster() && !g_GuildInfo.IsGuildSecondMaster() )
						return;
					else
					{
						// 패킷 처리
						SP2Packet kPacket( CTPK_GUILD_ENTRY_REFUSE );
						kPacket << m_dwGuildIndex << m_dwTableIndex << m_dwUserIndex;
						TCPNetwork::SendToServer( kPacket );

						// UI 처리 
						GuildEntryDelayUserWnd *pParent = dynamic_cast<GuildEntryDelayUserWnd*>(GetParent());
						if( pParent )
							pParent->DeleteUserList( m_dwTableIndex );
					}
				}
			}
		}
		break;
	}
}

void GuildEntryDelayUserBtn::OnRender()
{
	ioButton::OnRender();


	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_szUserID.IsEmpty() )
	{
		if( m_iMaxList == 0 && GetID() == GuildEntryDelayUserWnd::ID_LIST_3 )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			g_FontMgr.PrintText( iXPos + 86, iYPos + 3, FONT_SIZE_12, STR(1) );
		}
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );

		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( m_szUserID, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );

			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( m_szUserID, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 79.0f, m_szUserID.c_str() );
	}	
}

void GuildEntryDelayUserBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szUserID.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szUserID, UserInfoToolTip::TT_GLOBAL );
	}	
}
//////////////////////////////////////////////////////////////////////////
bool GuildEntryDelayUserWnd::sg_bAutoShowIgnore = false;
GuildEntryDelayUserWnd::GuildEntryDelayUserWnd()
{
	m_pBottomFrm = NULL;

	m_bAutoShow = true;
	m_dwGuildIndex = 0;
	m_iCurPage = m_iMaxPage = 0;
}

GuildEntryDelayUserWnd::~GuildEntryDelayUserWnd()
{
	SAFEDELETE( m_pBottomFrm );
}

void GuildEntryDelayUserWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void GuildEntryDelayUserWnd::SetEntryDelayUser( SP2Packet &rkPacket )
{
	if( m_bAutoShow && sg_bAutoShowIgnore )
		return;

	m_vUserList.clear();

	int iMaxUser;
	rkPacket >> m_dwGuildIndex >> iMaxUser;

	if( iMaxUser > 0 && !IsShow() )
	{	
		ShowWnd();
	}

	// 대기자 테이블 인덱스, 대기자 인덱스, 대기자 레벨, 대기자 닉네임
	for(int i = 0;i < iMaxUser;i++)
	{
		EntryUser kInfo;
		rkPacket >> kInfo.m_dwTableIndex >> kInfo.m_dwUserIndex >> kInfo.m_iUserLevel >> kInfo.m_szUserID;

		g_UserInfoMgr.SetGrade( kInfo.m_szUserID, kInfo.m_iUserLevel );
		m_vUserList.push_back( kInfo );
	}
	UpdateSetPage( m_iCurPage );
}

void GuildEntryDelayUserWnd::ReSizeWnd()
{
	if( m_bAutoShow )
	{
		ShowChildWnd( ID_AUTO_SHOW_IGNORE );
		SetSize( GetWidth(), AUTO_SHOW_HEIGHT );
		SetWndPos( Setting::Width() - ( GetWidth() + 1 ), 1 );
	}
	else
	{
		HideChildWnd( ID_AUTO_SHOW_IGNORE );
		SetSize( GetWidth(), CUSTOM_SHOW_HEIGHT );
		SetWndPos( ( Setting::Width() / 2 ) - ( GetWidth() / 2 ), ( Setting::Height() / 2 ) - ( ( GetHeight() / 2 ) + 20 ) );
	}
}

void GuildEntryDelayUserWnd::iwm_show()
{
	m_vUserList.clear();
	
	ioCheckButton *pCheckBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_AUTO_SHOW_IGNORE ) );
	if( pCheckBtn )
		pCheckBtn->SetCheck( sg_bAutoShowIgnore );

	ReSizeWnd();
	UpdateSetPage( 0 );
}

void GuildEntryDelayUserWnd::iwm_hide()
{
	m_bAutoShow = true;
}

void GuildEntryDelayUserWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_LEFT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage - 1 );
		}
		break;
	case ID_RIGHT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage + 1 );
		}
		break;
	case ID_AUTO_SHOW_IGNORE:
		if( cmd == IOBN_BTNUP )
		{
			sg_bAutoShowIgnore = !sg_bAutoShowIgnore;
			if( sg_bAutoShowIgnore )
				HideWnd();
		}
		break;
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
		if( cmd == IOBN_BTNUP )
		{
			GuildEntryDelayUserBtn *pUserBtn = dynamic_cast<GuildEntryDelayUserBtn*>( pWnd );
			if( pUserBtn )
			{
				g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), false );
			}
		}
		break;
	}
}

void GuildEntryDelayUserWnd::DeleteUserList( DWORD dwTableIndex )
{
	int iListSIze = m_vUserList.size();
	for(int i = 0;i < iListSIze;i++)
	{
		EntryUser &kInfo = m_vUserList[i];
		if( kInfo.m_dwTableIndex == dwTableIndex )
		{
			m_vUserList.erase( m_vUserList.begin() + i );
			break;
		}
	}
	UpdateSetPage( m_iCurPage );
}

void GuildEntryDelayUserWnd::UpdateSetPage( int iCurPage )
{
	int iUserCount = m_vUserList.size();
	m_iMaxPage = max( 0, iUserCount - 1 ) / MAX_LIST;
	m_iCurPage = max( 0, iCurPage );
	m_iCurPage = min( m_iCurPage, m_iMaxPage );

	int iStartArray = m_iCurPage * MAX_LIST;
	for(int i = 0;i < MAX_LIST;i++,iStartArray++)
	{
		GuildEntryDelayUserBtn *pItem = (GuildEntryDelayUserBtn*)FindChildWnd( ID_LIST_1 + i );
		if( pItem )
		{
			if( iStartArray < iUserCount )
			{
				EntryUser &kInfo = m_vUserList[iStartArray];
				pItem->MemberInfo( iUserCount, m_dwGuildIndex, kInfo.m_dwTableIndex, kInfo.m_dwUserIndex, kInfo.m_iUserLevel, kInfo.m_szUserID );
			}
			else
			{
				pItem->MemberInfo( iUserCount, 0, 0, 0, 0, "" );
			}
		}
	}	
}

void GuildEntryDelayUserWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void GuildEntryDelayUserWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		ReSizeWnd();
	}
}

void GuildEntryDelayUserWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	//
	if( m_bAutoShow )
	{
		if( m_pBottomFrm )
			m_pBottomFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 168, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
GuildInvitationFriendWnd::GuildInvitationFriendWnd()
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

GuildInvitationFriendWnd::~GuildInvitationFriendWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void GuildInvitationFriendWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
}

void GuildInvitationFriendWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void GuildInvitationFriendWnd::iwm_show()
{
	// 온라인 친구 리스트
	m_szOverListUser.Clear();
	m_OnlineUserList.clear();
	for(int i = 0;i < g_FriendManager.GetNodeSize();i++)
	{
		FriendNode *pNode = g_FriendManager.GetNode( i );
		if( !pNode ) continue;
		if( !pNode->IsOnline() ) continue;

		m_OnlineUserList.push_back( pNode->GetName() );
	}			

	if( m_OnlineUserList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		HideWnd();
		return;
	}

	m_iCurPos = 0;
	if( (int)m_OnlineUserList.size() <= MAX_PAGE_LIST )
	{
		HideChildWnd( ID_VERT_SCROLL );
	}
	else
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, m_OnlineUserList.size() );
			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}

	// 윈도우 사이즈 결정	
	SetSize( GetWidth(), ( min( MAX_PAGE_LIST, m_OnlineUserList.size() ) * m_iHeightGap) + m_iAddHeight );
	SetCheckParentOutSide( true );
}

void GuildInvitationFriendWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void GuildInvitationFriendWnd::iwm_wheel( int zDelta )
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

void GuildInvitationFriendWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void GuildInvitationFriendWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_szOverListUser.Clear();

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != GetParent()->FindChildWnd( GuildInvitationWnd::ID_FRIEND_BTN ) &&
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
	if( (int)m_OnlineUserList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_OnlineUserList.size();
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
			m_szOverListUser  = m_OnlineUserList[iListPos];
			if( pMouse->IsLBtnUp() )
			{
				if( GetParent() )
					GetParent()->iwm_command( this, IOBN_BTNUP, 0 );
				HideWnd();
			}
			break;
		}
	}
}

void GuildInvitationFriendWnd::OnRender()
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
	if( (int)m_OnlineUserList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_OnlineUserList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		bDarkRender = !bDarkRender;
		if( m_szOverListUser == m_OnlineUserList[iListPos] )
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
									 m_OnlineUserList[iListPos].c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
GuildInvitationWnd::GuildInvitationWnd()
{
	m_pPreEdit = NULL;
	m_pNameEdit= NULL;
}

GuildInvitationWnd::~GuildInvitationWnd()
{

}

void GuildInvitationWnd::SendInvitation()
{
	if( g_App.IsMouseBusy() ) return;	

	ioHashString szUserName;
	ioEdit *pEdit = (ioEdit*)m_pNameEdit;
	if( pEdit )
	{
		szUserName = pEdit->GetText();
		pEdit->SetKeyFocus();
	}

	if( szUserName.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( !g_App.IsRightID( szUserName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( szUserName == g_MyInfo.GetPublicID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_GuildInfo.IsInvitationUser( szUserName ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	if( g_App.IsAdminID( szUserName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}

	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
	if( pGuildData )
	{
		if( pGuildData->GetGuildJoinUser() >= pGuildData->GetGuildMaxUser() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
			HideWnd();
		}
	}	

	SP2Packet kPacket( CTPK_GUILD_INVITATION );
	kPacket << szUserName;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}


void GuildInvitationWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			SendInvitation();	
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
	case ID_NAME_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				SendInvitation();	
			}
		}
		break;
	case ID_FRIEND_LIST:
		if( cmd == IOBN_BTNUP )
		{
			GuildInvitationFriendWnd *pFriendWnd = dynamic_cast<GuildInvitationFriendWnd*>( pWnd );
			if( pFriendWnd )
			{
				g_InputBox.SetString( pFriendWnd->GetOverUser().c_str() );
			}
		}
		break;
	}
}

void GuildInvitationWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	m_pNameEdit = FindChildWnd( ID_NAME_EDIT );
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}
}

void GuildInvitationWnd::iwm_hide()
{
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

bool GuildInvitationWnd::iwm_esc()
{
	ioWnd *pFriendListWnd = FindChildWnd( ID_FRIEND_LIST );
	if( pFriendListWnd && pFriendListWnd->IsShow() )
	{
		pFriendListWnd->HideWnd();
		return true;
	}
	return ioWnd::iwm_esc();
}

void GuildInvitationWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void GuildInvitationWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, STR(1) );
}
//////////////////////////////////////////////////////////////////////////
DWORD GuildInvitedWnd::m_dwGuildIndex = 0;
DWORD GuildInvitedWnd::m_dwGuildMark = 0;
ioHashString GuildInvitedWnd::m_szGuildName = "";
ioHashString GuildInvitedWnd::m_szUserName = "";
int   GuildInvitedWnd::m_iUserLevel = 0;
bool  GuildInvitedWnd::sg_bInvitedIgnore = false;
GuildInvitedWnd::GuildInvitedWnd()
{
	m_pGuildMarkUpper = NULL;
}

GuildInvitedWnd::~GuildInvitedWnd()
{
	SAFEDELETE( m_pGuildMarkUpper );
}

void GuildInvitedWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "GuildMarkUpper" )
	{
		SAFEDELETE( m_pGuildMarkUpper );
		m_pGuildMarkUpper = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void GuildInvitedWnd::SetInvitedInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_szUserName >> m_iUserLevel >> m_dwGuildIndex >> m_dwGuildMark >> m_szGuildName;

	g_GuildInfo.SetGuildSimpleInfo( m_dwGuildIndex, m_szGuildName, m_dwGuildMark );
	g_UserInfoMgr.SetGuildIndex( m_szUserName, m_dwGuildIndex );
	
	if( sg_bInvitedIgnore )
	{
		m_dwGuildIndex = 0;
	}

	if( m_dwGuildIndex == 0 )
	{
		HideWnd();
	}
	else
	{
		ShowWnd();
	}
}

bool GuildInvitedWnd::iwm_esc()
{
	if( ioWnd::iwm_esc() )
	{
		m_dwGuildIndex = 0;
		return true;
	}
	return false;
}

void GuildInvitedWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			m_dwGuildIndex = 0;
		}
		break;
	case ID_INFO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_dwGuildIndex != 0 )
			{
				GuildMainWnd *pGuildMainWnd = dynamic_cast<GuildMainWnd *>(g_GUIMgr.FindWnd( GUILD_MAIN_WND ));
				if( pGuildMainWnd )
					pGuildMainWnd->SetGuildIndex( m_dwGuildIndex );
			}
			HideWnd();
			m_dwGuildIndex = 0;
		}
		break;
	case ID_REFUSE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			m_dwGuildIndex = 0;
		}
		break;
	case ID_INVITED_IGNORE:
		if( cmd == IOBN_BTNUP )
		{
            sg_bInvitedIgnore = !sg_bInvitedIgnore;
			if( sg_bInvitedIgnore )
			{
				HideWnd();
				m_dwGuildIndex = 0;
			}
		}
		break;
	}
}

void GuildInvitedWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( m_dwGuildIndex != 0 )
			ShowWnd();
		else
			return;
	}

	ioMovingWnd::OnProcess( fTimePerSec );
}

void GuildInvitedWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_GuildMarkMgr.RenderMarkSize( 35, 35, m_dwGuildIndex, m_dwGuildMark, iXPos + 16, iYPos + 52 );
	if( m_pGuildMarkUpper )
		m_pGuildMarkUpper->Render( iXPos, iYPos );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 길드", m_szGuildName.c_str() );
	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 71, FONT_SIZE_13, 133.0f, "의 길드가입 권유 도착" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
    g_FontMgr.PrintText( iXPos + 16, iYPos + 99, FONT_SIZE_12, STR(5) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 117, FONT_SIZE_12, STR(6) );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 63, iYPos + 117, FONT_SIZE_12, 133.0f, m_szUserName.c_str() );
}

void GuildInvitedWnd::iwm_show()
{
	ioCheckButton *pCheckBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_INVITED_IGNORE ) );
	if( pCheckBtn )
		pCheckBtn->SetCheck( sg_bInvitedIgnore );
}
//////////////////////////////////////////////////////////////////////////
GuildTitleChangeWnd::GuildTitleChangeWnd()
{
	m_pPreEdit = NULL;
}

GuildTitleChangeWnd::~GuildTitleChangeWnd()
{
	
}

void GuildTitleChangeWnd::ChangeTitle()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_LINE );
	if( pEdit )
	{
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( pGuildData )
		{
			ioHashString szGuildTitle= pEdit->GetText();		
			char szSlangName[MAX_PATH] = "";
			sprintf( szSlangName, "%s", g_SlangUtil.ConvertString( szGuildTitle.c_str() ) );
			if( szGuildTitle != szSlangName )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );	
				return;
			}
			else
			{
				SP2Packet kPacket( CTPK_GUILD_TITLE_CHANGE );
				kPacket << szGuildTitle;
				TCPNetwork::SendToServer( kPacket );

				pGuildData->SetGuildTitle( szGuildTitle );
			}
		}
	}
	HideWnd();
}

void GuildTitleChangeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			ChangeTitle();
		}
		break;
	case ID_EDIT_LINE:
		if( cmd == IOED_INPUT )
		{
			if( param == 1 )
			{
				ChangeTitle();
			}
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			
		}
		break;
	}
}

void GuildTitleChangeWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	// 길드명을 넣어준다.
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_LINE );
	if( pEdit )
	{
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( pGuildData )
		{
			pEdit->ClearString();
			pEdit->SetKeyFocus();
			g_InputBox.SetString( pGuildData->GetGuildTitle().c_str() );
		}
	}
}

void GuildTitleChangeWnd::iwm_hide()
{
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

bool GuildTitleChangeWnd::iwm_esc()
{
	return ioWnd::iwm_esc();
}

void GuildTitleChangeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}

void GuildTitleChangeWnd::OnRender()
{
	ioWnd::OnRender();
}
//////////////////////////////////////////////////////////////////////////
GuildEtcItemPopup::GuildEtcItemPopup()
{
	m_dwPopupBtnID = 0;
	m_pNeedIconBack = NULL;
	m_pUseIconBack	= NULL;
	m_pGuildCreateIcon		= NULL;
	m_pGuildMarkChangeIcon  = NULL;
}

GuildEtcItemPopup::~GuildEtcItemPopup()
{
	SAFEDELETE( m_pNeedIconBack );
	SAFEDELETE( m_pUseIconBack );
	SAFEDELETE( m_pGuildCreateIcon );
	SAFEDELETE( m_pGuildMarkChangeIcon );
}

void GuildEtcItemPopup::ShowGuildCreate()
{
	HideWnd();
	HideChildWnd( ID_CREATE_ITEM_BUY );
	HideChildWnd( ID_CREATE_ITEM_USE );
	HideChildWnd( ID_MARK_CHANGE_ITEM_BUY );
	HideChildWnd( ID_MARK_CHANGE_ITEM_USE );

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem && pUserEtcItem->IsCanUse( ioEtcItem::EIT_ETC_GUILD_CREATE ) )
	{
		m_dwPopupBtnID = ID_CREATE_ITEM_USE;
		SetTitleText( STR(1) );
	}
	else
	{
		m_dwPopupBtnID = ID_CREATE_ITEM_BUY;
		SetTitleText( STR(2) );
	}	
	ShowChildWnd( m_dwPopupBtnID );
	ShowWnd();
}

void GuildEtcItemPopup::ShowGuildMarkChange()
{
	HideWnd();
	HideChildWnd( ID_CREATE_ITEM_BUY );
	HideChildWnd( ID_CREATE_ITEM_USE );
	HideChildWnd( ID_MARK_CHANGE_ITEM_BUY );
	HideChildWnd( ID_MARK_CHANGE_ITEM_USE );

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem && pUserEtcItem->IsCanUse( ioEtcItem::EIT_ETC_GUILD_MARK_CHANGE ) )
	{
		m_dwPopupBtnID = ID_MARK_CHANGE_ITEM_USE;
		SetTitleText( STR(1) );
	}
	else
	{
		m_dwPopupBtnID = ID_MARK_CHANGE_ITEM_BUY;
		SetTitleText( STR(2) );
	}
	ShowChildWnd( m_dwPopupBtnID );
	ShowWnd();
}

void GuildEtcItemPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_CREATE_ITEM_BUY:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_GUILD_CREATE, 0 );
			}
		}
		break;
	case ID_CREATE_ITEM_USE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem && pUserEtcItem->IsCanUse( ioEtcItem::EIT_ETC_GUILD_CREATE ) )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_GUILD_CREATE );
				if( pEtcItem )
					pEtcItem->OnUseBtnPush( ioEtcItem::EIT_ETC_GUILD_CREATE, pUserEtcItem );
			}
		}
		break;
	case ID_MARK_CHANGE_ITEM_BUY:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_GUILD_MARK_CHANGE, 0 );
			}
		}
		break;
	case ID_MARK_CHANGE_ITEM_USE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem && pUserEtcItem->IsCanUse( ioEtcItem::EIT_ETC_GUILD_MARK_CHANGE ) )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_GUILD_MARK_CHANGE );
				if( pEtcItem )
					pEtcItem->OnUseBtnPush( ioEtcItem::EIT_ETC_GUILD_MARK_CHANGE, pUserEtcItem );
			}
		}
		break;
	}
}

void GuildEtcItemPopup::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "NeedIconBack" )
	{
		SAFEDELETE( m_pNeedIconBack );
		m_pNeedIconBack = pImage;
	}
	else if( szType == "UseIconBack" )
	{
		SAFEDELETE( m_pUseIconBack );
		m_pUseIconBack = pImage;
	}
	else if( szType == "GuildCreateIcon" )
	{
		SAFEDELETE( m_pGuildCreateIcon );
		m_pGuildCreateIcon = pImage;
	}
	else if( szType == "GuildMarkChangeIcon" )
	{
		SAFEDELETE( m_pGuildMarkChangeIcon );
		m_pGuildMarkChangeIcon = pImage;
	}
	else 
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void GuildEtcItemPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	switch( m_dwPopupBtnID )
	{
	case ID_CREATE_ITEM_BUY:
		{
			if( m_pNeedIconBack )
				m_pNeedIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );
			
			if( m_pGuildCreateIcon )
				m_pGuildCreateIcon->Render( iXPos + 151, iYPos + 98 );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(1) );

			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, "진영구분없이 친구들과 함께할 수 있습니다." );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, STR(3) );
		}
		break;
	case ID_CREATE_ITEM_USE:
		{
			if( m_pUseIconBack )
				m_pUseIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

			if( m_pGuildCreateIcon )
				m_pGuildCreateIcon->Render( iXPos + 151, iYPos + 98 );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(4) );

			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, "진영구분없이 친구들과 함께할 수 있습니다." );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, "길드원들과 함께 최고의 길드를 만들어보세요." );
		}
		break;
	case ID_MARK_CHANGE_ITEM_BUY:
		{
			if( m_pNeedIconBack )
				m_pNeedIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

			if( m_pGuildMarkChangeIcon )
				m_pGuildMarkChangeIcon->Render( iXPos + 151, iYPos + 98 );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(7) );

			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, STR(8) );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, STR(9) );
		}
		break;
	case ID_MARK_CHANGE_ITEM_USE:
		{
			if( m_pUseIconBack )
				m_pUseIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

			if( m_pGuildMarkChangeIcon )
				m_pGuildMarkChangeIcon->Render( iXPos + 151, iYPos + 98 );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(10) );

			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, STR(11) );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, STR(12) );
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
GuildNameChangeWnd::GuildNameChangeWnd()
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pBackEffect = NULL;
}

GuildNameChangeWnd::~GuildNameChangeWnd()
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pBackEffect);
}

void GuildNameChangeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void GuildNameChangeWnd::SetInfoAndShow( const ioHashString &szPreName, const ioHashString &szNewName )
{
	m_szPreGuildName = szPreName;
	m_szNewGuildName = szNewName;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE );
	if( pItem )
	{
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() );
	}

	ShowWnd();
}

void GuildNameChangeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void GuildNameChangeWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X	= 125,
		ICON_Y	= 98,

		TITLE_X	= 125,
		TITLE_Y	= 146,

		DESC_X	= 125,
		DESC_Y	= 169,

		LEFT_X	= 27,
		LEFT_Y	= 240,

		RIGHT_X	= 227,
		RIGHT_Y	= 240,
	};

	if( m_pBackEffect )
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, STR(1) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+LEFT_X, iYPos+LEFT_Y, FONT_SIZE_13, STR(2) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos+LEFT_X, iYPos+LEFT_Y+21, FONT_SIZE_13, STR(3) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );
	g_FontMgr.PrintText( iXPos+RIGHT_X, iYPos+RIGHT_Y, FONT_SIZE_13, STR(4), m_szNewGuildName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos+RIGHT_X, iYPos+RIGHT_Y+21, FONT_SIZE_13, STR(5), m_szPreGuildName.c_str() );
}

