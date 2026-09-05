
#include "StdAfx.h"

#include "../ioGuildMarkMgr.h"
#include "../ioLadderTeamMgr.h"
#include "../ioMissionManager.h"
#include "../ioNoviceGradeBenefitManager.h"
#include "../ioPlayStage.h"

#include "PlazaMainWnd.h"
#include "UserInfoToolTip.h"
#include "TimeGateSearch.h"
#include "MissionWnd.h"
#include "NamedTitleBtn.h"
#include "PartyMenuWnd.h"

#define TWINKLE_TIME  500

PlazaListBtn::PlazaListBtn()
{
	m_pRoomNumBack = NULL;
	m_pLockKey = NULL;
	m_pLockKeyGray = NULL;
	m_pIconEmpty   = NULL;
	m_pIconNpc = NULL;
	m_fWinkTime = 0.f;
	m_bNpc = false;
	m_bTwinkle = false;
}

PlazaListBtn::~PlazaListBtn()
{
	SAFEDELETE( m_pRoomNumBack );
	SAFEDELETE( m_pLockKey );
	SAFEDELETE( m_pLockKeyGray );
	SAFEDELETE( m_pIconEmpty );
	SAFEDELETE( m_pIconNpc );
}

void PlazaListBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "RoomNumBack" )
	{
		SAFEDELETE( m_pRoomNumBack );
		m_pRoomNumBack = pImage;
	}
	else if( szType == "LockKey" )
	{
		SAFEDELETE( m_pLockKey );
		m_pLockKey = pImage;
	}
	else if( szType == "LockKeyGray" )
	{
		SAFEDELETE( m_pLockKeyGray );
		m_pLockKeyGray = pImage;
	}
	else if( szType == "IconEmpty" )
	{
		SAFEDELETE( m_pIconEmpty );
		m_pIconEmpty = pImage;
	}
	else if( szType == "IconNpc")
	{
		SAFEDELETE( m_pIconNpc );
		m_pIconNpc = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void PlazaListBtn::SetPlazaInfo( PlazaInfo &rkPlazaInfo )
{
	m_PlazaInfo = rkPlazaInfo;

	if( rkPlazaInfo.m_iSubState > 0)
		m_bNpc = true;
	else
		m_bNpc = false;

	if( m_bNpc )
	{
		m_fWinkTime = (float)FRAMEGETTIME() + (float)TWINKLE_TIME;
		m_bTwinkle = true;
	}

	if( m_PlazaInfo.m_ePlazaType == PT_COMMUNITY )
		m_szPlazaType = STR(1);
	else if( m_PlazaInfo.m_ePlazaType == PT_GUILD )
		m_szPlazaType = STR(2);
	else
		m_szPlazaType = STR(3);
}

void PlazaListBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if(m_bNpc)
	{
		if( m_fWinkTime < (float)FRAMEGETTIME() )
		{
			m_fWinkTime += (float)TWINKLE_TIME;
			m_bTwinkle = m_bTwinkle ^ true;
		}
	}
}

void PlazaListBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_PlazaInfo.m_iRoomNum == -1 )
	{
		if( m_pIconEmpty )
		{
			m_pIconEmpty->SetScale( 0.60f );
			m_pIconEmpty->Render( iXPos + 31, iYPos + 31, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}

		if( m_pRoomNumBack )
			m_pRoomNumBack->Render( iXPos + 55, iYPos + 34 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
		g_FontMgr.PrintText( iXPos + 57, iYPos + 16, FONT_SIZE_13, STR(1) );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 69, iYPos + 35, FONT_SIZE_10, "-" );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
		g_FontMgr.PrintText( iXPos + 85, iYPos + 34, FONT_SIZE_11, STR(2) );
	}
	else
	{
		// 맵 아이콘
		ioUIRenderImage *pTypeResource = g_ModeSelectInfo.GetListMapUIResource( m_PlazaInfo.m_szSubTitle );
		if( pTypeResource )
		{
			if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE )
				pTypeResource->Render( iXPos + 9, iYPos + 10, UI_RENDER_NORMAL, TFO_BILINEAR );
			else
				pTypeResource->Render( iXPos + 9, iYPos + 10, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );

			if( m_pIconNpc && m_bNpc)
			{
				m_pIconNpc->SetScale( 0.45f );
				m_pIconNpc->Render(iXPos + 30, iYPos + 31, UI_RENDER_NORMAL, TFO_BILINEAR);
				if(m_bTwinkle)
					m_pIconNpc->Render(iXPos + 30, iYPos + 31, UI_RENDER_ADD, TFO_BILINEAR);
			}
		}

		// 광장 제목
		float fRoomNameLimit = 87.0f;
		if( m_PlazaInfo.m_bRoomPW )
			fRoomNameLimit = 74.0f;

		char szRoomName[MAX_PATH] = "";
		Help::StringCutFun( FONT_SIZE_13, fRoomNameLimit, TS_NORMAL, szRoomName, sizeof(szRoomName), m_PlazaInfo.m_szRoomName.c_str() );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 57, iYPos + 16, FONT_SIZE_13, STR(3), szRoomName );

		// 비번
		float fRealXPos = g_FontMgr.GetTextWidth( szRoomName, TS_NORMAL, FONT_SIZE_13 );
		if( m_PlazaInfo.m_bRoomPW )
		{
			if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE && m_pLockKey )
				m_pLockKey->Render( iXPos + 57 + fRealXPos + 2, iYPos + 18 );
			else if( m_pLockKeyGray )
				m_pLockKeyGray->Render( iXPos + 57 + fRealXPos + 2, iYPos + 18 );
			fRealXPos += 13.0f;
		}	

		// 인원
		if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE  )
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 57 + fRealXPos + 5, iYPos + 16, FONT_SIZE_13, STR(6), m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iMaxUserCount );

		// 방번호
		if( m_pRoomNumBack )
			m_pRoomNumBack->Render( iXPos + 55, iYPos + 34 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( iXPos + 69, iYPos + 35, FONT_SIZE_10, STR(4), m_PlazaInfo.m_iRoomIndex );

		// 맵 명
		char szMapName[MAX_PATH] = "";
		sprintf( szMapName, "%s/", m_PlazaInfo.m_szSubTitle.c_str() );
		g_FontMgr.SetAlignType( TAT_LEFT );
		if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 85, iYPos + 34, FONT_SIZE_11, STR(5), szMapName );
		
		if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE )
		{
			if( m_PlazaInfo.m_ePlazaType == PT_COMMUNITY )
				g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			else 
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		}
		float fMapNameSize = g_FontMgr.GetTextWidth( szMapName, TS_NORMAL, FONT_SIZE_11 );
		g_FontMgr.PrintText( iXPos + 85 + fMapNameSize, iYPos + 34, FONT_SIZE_11, STR(5), m_szPlazaType.c_str() );

/*		// 맵명 + 인원
		char szMode[MAX_PATH] = "";
		sprintf( szMode, "%s %d/%d", m_PlazaInfo.m_szSubTitle.c_str(), m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iMaxUserCount );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 16, FONT_SIZE_13, 126.0f, STR(3), szMode );

		// 방번호
		if( m_pRoomNumBack )
			m_pRoomNumBack->Render( iXPos + 56, iYPos + 34 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( iXPos + 69, iYPos + 35, FONT_SIZE_10, STR(4), m_PlazaInfo.m_iRoomIndex );

		// 방제목
		g_FontMgr.SetAlignType( TAT_LEFT );
		if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 85, iYPos + 34, FONT_SIZE_11, 85.0f, STR(5), m_PlazaInfo.m_szRoomName.c_str() );

		// 비밀방
		if( m_PlazaInfo.m_bRoomPW )
		{
			int iRealNameSize = g_FontMgr.GetTextWidthCutSize( m_PlazaInfo.m_szRoomName.c_str(), TS_NORMAL, FONT_SIZE_11, 85.0f );
			if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE && m_pLockKey )
				m_pLockKey->Render( iXPos + 85 + iRealNameSize + 2, iYPos + 35 );
			else if( m_pLockKeyGray )
				m_pLockKeyGray->Render( iXPos + 85 + iRealNameSize + 2, iYPos + 35 );
		}	
*/
	}
}
//////////////////////////////////////////////////////////////////////////
PlazaPasswordWnd::PlazaPasswordWnd()
{
	m_pPreEdit	= NULL;
	m_iRoomIndex= -1;
	m_iRoomLevel= 0;
	m_iCurJoiner= 0;
	m_iCurPlayer= 0;
	m_iMaxPlayer= 0;
	m_ePlazaType = PT_NONE;
	m_bHidePrevWndShow = true;
}

PlazaPasswordWnd::~PlazaPasswordWnd()
{

}

void PlazaPasswordWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PASS_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}	
	m_bHidePrevWndShow = true;
}

void PlazaPasswordWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PASS_EDIT );
	if( pEdit )
		pEdit->KillKeyFocus();
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

void PlazaPasswordWnd::SetPlazaInfo( int iRoomIndex, int iRoomLevel, ioHashString szName, int iCurJoiner, int iCurPlayer, int iMaxPlayer, PlazaType ePlazaType )
{
	m_iRoomIndex= iRoomIndex;
	m_iRoomLevel= iRoomLevel;
	m_szRoomName= szName;
	m_iCurJoiner= iCurJoiner;
	m_iCurPlayer= iCurPlayer;
	m_iMaxPlayer= iMaxPlayer;
	m_ePlazaType= ePlazaType;

	if( m_iRoomIndex != -1 )
		ShowWnd();
}

void PlazaPasswordWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			JoinPlaza();
		}
		break;
	case ID_CANCEL:
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();			
		}
		break;
	case ID_PASS_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				JoinPlaza();
			}						
		}
		break;
	}
}

void PlazaPasswordWnd::JoinPlaza()
{
	if( g_App.IsMouseBusy() )
		return;
	
	if( g_MyInfo.IsTutorialUser() )
	{	
		g_GUIMgr.SetPrevMsgBox( NULL, MB_OK, NULL, STR(1) );
		HideWnd();
		return;
	}

	if( g_MyInfo.GetCharCount() == 0 )
	{
		g_GUIMgr.SetPrevMsgBox( NULL, MB_OK, NULL, STR(2) );
		HideWnd();
		return;
	}

	if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_MyInfo.IsKickOutPlaza( m_iRoomIndex ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}

	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return;
	}

	if( m_ePlazaType == PT_GUILD && !g_GuildInfo.IsGuild() )
	{
		HideWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		return;
	}

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PASS_EDIT );
	if( pEdit )
	{
		SP2Packet kPacket( CTPK_SEARCH_PLAZA_ROOM );
		kPacket << m_iRoomIndex << false << pEdit->GetText();
		TCPNetwork::SendToServer( kPacket );

		m_bHidePrevWndShow = false;
		HideWnd();
		TCPNetwork::MouseBusy( true );
	}			
}

void PlazaPasswordWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
	g_FontMgr.PrintText( iXPos + 125, iYPos + 55, FONT_SIZE_13, STR(1) );
}
//////////////////////////////////////////////////////////////////////////
PlazaListWnd::PlazaListWnd()
{
	m_iPageYPos = 258;
	m_iCurPage = m_iMaxPage = 0;
	m_dwSearchTime			= 0;
	m_dwMaxPlazaList		= LOBBY_MAX_PLAZA_LIST;
}

PlazaListWnd::~PlazaListWnd()
{
	m_vPlazaList.clear();
}

void PlazaListWnd::StartSend( bool bProcess )
{
	if( !g_App.IsMouseRender() ) 
		return;

	SP2Packet kPacket( CTPK_PLAZA_ROOM_LIST );
	kPacket << m_iCurPage << m_dwMaxPlazaList;
	TCPNetwork::SendToServer( kPacket );	
	m_dwSearchTime = FRAMEGETTIME();
}

void PlazaListWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > PLAZA_SEARCH_TIME )
	{
		StartSend( true );
	}
}

void PlazaListWnd::iwm_show()
{	
	m_iCurPage = m_iMaxPage = 0;
	m_vPlazaList.clear();

	StartSend();

	for( int i = ID_LIST_1; i <= ID_LIST_8; i++ )
	{
		PlazaListBtn *pBtn = (PlazaListBtn*)FindChildWnd( i );
		if( pBtn )
		{
			PlazaInfo kInfo;
			pBtn->SetPlazaInfo( kInfo );
			pBtn->AddWndStyle( IWS_EXACTIVE );

			if( i < static_cast<int>( ID_LIST_1 + m_dwMaxPlazaList ) )
				pBtn->ShowWnd();
			else
				pBtn->HideWnd();
		}
	}	
}

void PlazaListWnd::PlazaUIMove()
{
	ioWnd* pLeft = FindChildWnd( ID_PAGE_LEFT );
	if( pLeft )
		pLeft->SetWndPos( pLeft->GetDefaultPosX(), pLeft->GetDefaultPosY() + 65 );

	ioWnd* pRight = FindChildWnd( ID_PAGE_RIGHT );
	if( pRight )
		pRight->SetWndPos( pRight->GetDefaultPosX(), pRight->GetDefaultPosY() + 65 );

	m_iPageYPos = 323;
}

void PlazaListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_PLAZA_CREATE:
		if( cmd == IOBN_BTNUP )
		{	
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else 
			{
				if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return;
				}

				g_GUIMgr.ShowWnd( PLAZA_CREATE_WND );
			}
		}
		break;
	case ID_PAGE_LEFT:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( max( 0, m_iCurPage - 1 ) );
		}
		break;
	case ID_PAGE_RIGHT:
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
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else 
			{
				PlazaListBtn *pBtn = (PlazaListBtn*)pWnd;
				if( pBtn->GetRoomIndex() < 0 )
				{
					iwm_command( FindChildWnd( ID_PLAZA_CREATE ), IOBN_BTNUP, 0 );
				}
				else if( !pBtn->IsMyRoom() )
				{
					PlazaInfoWnd *pPlazaInfoWnd = dynamic_cast<PlazaInfoWnd*>(g_GUIMgr.FindWnd( PLAZA_INFO_WND ));
					if( pPlazaInfoWnd )
					{
						if( pPlazaInfoWnd->IsShow() )
							pPlazaInfoWnd->HideWnd();

						pPlazaInfoWnd->SetPassword( pBtn->IsRoomPW() );
						pPlazaInfoWnd->SetState( pBtn->GetRoomState() );

						SP2Packet kPacket( CTPK_PLAZA_JOIN_INFO );
						kPacket << pBtn->GetRoomIndex();
						TCPNetwork::SendToServer( kPacket );
					}
				}
			}
		}
		break;
	}
}

void PlazaListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;	

	ProcessSearch();	
}

void PlazaListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 201, iYPos + m_iPageYPos, FONT_SIZE_12, STR(1), m_iCurPage + 1 );
}

void PlazaListWnd::OnJoinPlaza( int iRoomIndex )
{
	if( g_App.IsMouseBusy() ) return;

	if( g_MyInfo.IsTutorialUser() )
	{	
		HideWnd();
		g_GUIMgr.SetPrevMsgBox( this, MB_OK, NULL, STR(1) );
		return;
	}

	if( g_MyInfo.GetCharCount() == 0 )
	{
		HideWnd();
		g_GUIMgr.SetPrevMsgBox( this, MB_OK, NULL, STR(2) );
		return;
	}

	if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_MyInfo.IsKickOutPlaza( iRoomIndex ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}

	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return;
	}

	SP2Packet kPacket( CTPK_SEARCH_PLAZA_ROOM );
	kPacket << iRoomIndex << false;
	TCPNetwork::SendToServer( kPacket );
	HideWnd();
	TCPNetwork::MouseBusy( true );
}

void PlazaListWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = iCurPage;

	if( m_iCurPage != iPrevPage )
		StartSend();

	int iListSIze = m_vPlazaList.size();
	int iStartPos = 0;
	for( int i = ID_LIST_1; i < static_cast<int>( ID_LIST_1 + m_dwMaxPlazaList ); i++, iStartPos++)
	{
		PlazaListBtn *pBtn = (PlazaListBtn*)FindChildWnd( i );
		if( pBtn )
		{
			if( iStartPos < iListSIze )
			{
				PlazaInfo &kInfo = m_vPlazaList[iStartPos];
				pBtn->SetPlazaInfo( kInfo );
				pBtn->RemoveWndStyle( IWS_EXACTIVE );
			}
			else
			{
				PlazaInfo kInfo;
				pBtn->SetPlazaInfo( kInfo );
				pBtn->AddWndStyle( IWS_EXACTIVE );
			}
		}
	}	
}

void PlazaListWnd::SetPlazaList( SP2Packet &rkPacket )
{	
	m_vPlazaList.clear();

	PlazaInfoWnd *pPlazaInfoWnd = dynamic_cast<PlazaInfoWnd*>(g_GUIMgr.FindWnd( PLAZA_INFO_WND ));
	if( !pPlazaInfoWnd ) return;

	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> iTotalList >> iCurList;		
	for(i = 0;i < iCurList;i++)
	{
		int iPlazaType, iSubNum;
		DWORD dwGuildIndex, dwGuildMark;
		PlazaInfo kInfo;
		rkPacket >> kInfo.m_iRoomIndex >> kInfo.m_iRoomNum >> kInfo.m_iState >> kInfo.m_iSubState >> kInfo.m_szRoomName >> kInfo.m_szMasterName >> iSubNum >> kInfo.m_bRoomPW >> kInfo.m_iMaxUserCount 
				 >> kInfo.m_iRoomLevel >> kInfo.m_iJoinUserCount >> kInfo.m_iPlayUserCount >> kInfo.m_iMasterLevel >> iPlazaType >> dwGuildIndex >> dwGuildMark;

		kInfo.m_ePlazaType = (PlazaType)iPlazaType;
		kInfo.m_szSubTitle = g_ModeSelectInfo.GetPlazaMapTitle( iPlazaType, iSubNum );
		m_vPlazaList.push_back( kInfo );

		if( !kInfo.m_szMasterName.IsEmpty() )
			g_GuildInfo.SetGuildMark( kInfo.m_szMasterName, dwGuildIndex, dwGuildMark );

		pPlazaInfoWnd->UpdatePlazaState( kInfo.m_iRoomIndex, kInfo.m_iState );
	}    

	m_iMaxPage = max( 0, iTotalList - 1 ) / m_dwMaxPlazaList;
	UpdateSetPage( m_iCurPage );
}
//////////////////////////////////////////////////////////////////////////
PlazaOwnerWnd::PlazaOwnerWnd() : m_pNameEdit( NULL ), m_pPWEdit( NULL ), m_pPreEdit( NULL )
{
}

PlazaOwnerWnd::~PlazaOwnerWnd()
{

}

void PlazaOwnerWnd::SetPlazaInfo( ioHashString szName, ioHashString szPW )
{
	m_szPlazaName = szName;
	m_szPlazaPW   = szPW;
	ShowWnd();
}

void PlazaOwnerWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	//
	m_pPWEdit = FindChildWnd( ID_EDIT_PW );
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( m_szPlazaPW.c_str() );
	}

	m_pNameEdit = FindChildWnd( ID_EDIT_NAME );
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( m_szPlazaName.c_str() );
	}
}

void PlazaOwnerWnd::iwm_hide()
{
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->KillKeyFocus();
	}
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

void PlazaOwnerWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			ChangePlazaInfo();
		}
		break;
	case ID_EDIT_NAME:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pPWEdit )
				{
					ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
					pPWEdit->SetKeyFocus();
				}
			}			
			else if( param == 1 )
			{
				ChangePlazaInfo();
			}
		}
		break;
	case ID_EDIT_PW:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pNameEdit )
				{
					ioEdit *pNameEdit = (ioEdit*)m_pNameEdit;
					pNameEdit->SetKeyFocus();
				}				
			}			
			else if( param == 1 )
			{
				ChangePlazaInfo();
			}
		}
		break;
	}
}

void PlazaOwnerWnd::ChangePlazaInfo()
{
	// 닉네임 변경
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_NAME );
	if( !pEdit ) return;

	ioHashString szName = pEdit->GetText();

	// 비번 변경
	pEdit = (ioEdit*)FindChildWnd( ID_EDIT_PW );
	if( !pEdit ) return;

	ioHashString szPW = pEdit->GetText();

	if( m_szPlazaName == szName && m_szPlazaPW == szPW )
	{
		HideWnd();
		return;
	}
	m_szPlazaName = g_SlangUtil.ConvertString( szName.c_str() );
	m_szPlazaPW   = szPW;

	// 정보 변경
	SP2Packet kPacket( CTPK_PLAZA_COMMAND );
	kPacket << PLAZA_CMD_NAME_PW_CHANGE << m_szPlazaName << m_szPlazaPW;
	TCPNetwork::SendToServer( kPacket );

	PlazaMainWnd *pMainWnd = dynamic_cast<PlazaMainWnd*>(g_GUIMgr.FindWnd( PLAZA_MAIN_WND ));
	if( pMainWnd )
	{
		pMainWnd->SetPlazaName( m_szPlazaName );
		pMainWnd->SetPlazaMasterPW( m_szPlazaPW );
	}
	HideWnd();
}

void PlazaOwnerWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 110,110,110 );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 57, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 80, FONT_SIZE_12, STR(2) );
}

//////////////////////////////////////////////////////////////////////////
PlazaInviteList::PlazaInviteList()
{
	m_bInviteActive = true;
	m_iGradeLevel = 0;
	m_dwGuildIndex = 0;
	m_dwGuildMark  = 0;

	m_pGuildMark	= NULL;
	m_pFriendMark	= NULL;
}

PlazaInviteList::~PlazaInviteList()
{
	SAFEDELETE( m_pGuildMark );
	SAFEDELETE( m_pFriendMark );
}

void PlazaInviteList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "GuildMark" )
	{
		SAFEDELETE( m_pGuildMark );
		m_pGuildMark = pImage;
	}
	else if( szType == "FriendMark" )
	{
		SAFEDELETE( m_pFriendMark );
		m_pFriendMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void PlazaInviteList::InviteInfo( int iGradeLevel, ioHashString szName, DWORD dwGuildIndex, DWORD dwGUildMark, bool bActive )
{
	m_bInviteActive = bActive;
	m_iGradeLevel   = iGradeLevel;
	m_dwGuildIndex  = dwGuildIndex;
	m_dwGuildMark   = dwGUildMark;
	m_szName        = szName;

	HideChildWnd( ID_INVITE_BTN );
	if( !szName.IsEmpty() )
	{
		ShowChildWnd( ID_INVITE_BTN );
		SetActive();
	}
	else
	{
		SetInActive();
	}

	if( !m_bInviteActive )
	{
		SetChildInActive( ID_INVITE_BTN );
	}
	else
	{
		SetChildActive( ID_INVITE_BTN );
	}
}

void PlazaInviteList::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_INVITE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_App.IsAdminID( m_szName.c_str() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}

			if( !m_bInviteActive )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}

			//초대
			SP2Packet kPacket( CTPK_PLAZA_USER_INVITE );
			kPacket << 1 << m_szName;
			TCPNetwork::SendToServer( kPacket );

			if( GetParent() )
			{
				PlazaInviteListWnd *pParentWnd = dynamic_cast<PlazaInviteListWnd*>(GetParent());
				pParentWnd->StartInivteWait( GetLevel(), GetName() );
			}
		}
		break;
	}
}

void PlazaInviteList::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
	}	
}

void PlazaInviteList::OnRender()
{
	ioButton::OnRender();

	if( !m_szName.IsEmpty() )
	{
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );

		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( m_szName, iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( m_szName, iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 133.0f, m_szName.c_str() );
		int iRealNameSize = g_FontMgr.GetTextWidthCutSize( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12, 133.0f );
		g_GuildMarkMgr.RenderSmallMark( m_dwGuildIndex, m_dwGuildMark, iXPos + 23 + iRealNameSize + 3, iYPos + 4 );
		if( m_dwGuildIndex != 0 )
			iRealNameSize += 16;
		// 길드 - > 친구 - > 일반
		if( g_GuildInfo.IsMyGuildUser( m_szName ) && m_pGuildMark )
			m_pGuildMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
		else if( g_FriendManager.IsFriend( m_szName ) && m_pFriendMark )
			m_pFriendMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
	}		
}
//////////////////////////////////////////////////////////////////////////
PlazaInviteListWnd::PlazaInviteListWnd() : m_pPreEdit( NULL )
{
	m_iCurPage = m_iPrevCurPage = m_iMaxPage = 0;
	m_dwSearchTime  = 0;
	m_bInviteActive = true;
	m_iRoomLevel    = 0;
}

PlazaInviteListWnd::~PlazaInviteListWnd()
{
	m_vUserList.clear();
}

void PlazaInviteListWnd::StartSend( bool bProcess )
{
	SP2Packet kPacket( CTPK_PLAZA_INVITE_LIST );
	kPacket << m_iCurPage << MAX_PAGE_LIST;
	TCPNetwork::SendToServer( kPacket );	
	m_dwSearchTime = FRAMEGETTIME();
}

void PlazaInviteListWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > INVITE_SEARCH_TIME )
	{
		StartSend( true );
	}
}

void PlazaInviteListWnd::iwm_show()
{	
	g_GUIMgr.HideWnd( INVITE_PROGRESS_WND );

	m_vUserList.clear();
	m_iCurPage = m_iPrevCurPage;
	m_iPrevCurPage = 0;
	StartSend();

	for(int i = ID_USER_LIST_1;i < ID_USER_LIST_10 + 1;i++)
	{
		PlazaInviteList *pItem = dynamic_cast<PlazaInviteList*> ( FindChildWnd( i ) );
		if( pItem )
		{
			pItem->InviteInfo( 0, "", 0, 0, false );
		}
	}	

	// 아이디 입력
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pNameEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );	
	if( pNameEdit )
	{
		pNameEdit->ClearString();
		pNameEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}
}

void PlazaInviteListWnd::iwm_hide()
{	
	m_vUserList.clear();

	ioEdit *pNameEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );
	if( pNameEdit )
	{
		pNameEdit->ClearString();
		pNameEdit->KillKeyFocus();		
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

void PlazaInviteListWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = min( m_iMaxPage, iCurPage );

	if( m_iCurPage != iPrevPage )
		StartSend();

	bool bActive = false;

	int iListSIze = m_vUserList.size();
	int iStartPos = 0;
	for(int i = ID_USER_LIST_1;i < ID_USER_LIST_10 + 1;i++,iStartPos++)
	{
		PlazaInviteList *pItem = (PlazaInviteList*)FindChildWnd( i );
		if( pItem )
		{
			if( iStartPos < iListSIze )
			{
				UserInviteInfo &kInfo = m_vUserList[iStartPos];
				pItem->InviteInfo( kInfo.m_iLevel, kInfo.m_szName, 
								   g_UserInfoMgr.GetGuildIndex( kInfo.m_szName ), g_UserInfoMgr.GetGuildMark( kInfo.m_szName ), m_bInviteActive );
			}
			else
			{
				pItem->InviteInfo( 0, "", 0, 0, false );
			}
		}
	}	
}

void PlazaInviteListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_ALL_INVITE:
		if( cmd == IOBN_BTNUP )
		{
			AllInvite();
		}
		break;
	case ID_NAME_INVITE:
		if( cmd == IOBN_BTNUP )
		{
			NameInvite();
		}
		break;
	case ID_NAME_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				iwm_command( FindChildWnd( ID_NAME_INVITE ), IOBN_BTNUP, 0 );
			}			
		}
		break;
	case ID_USER_LIST_1:
	case ID_USER_LIST_2:
	case ID_USER_LIST_3:
	case ID_USER_LIST_4:
	case ID_USER_LIST_5:
	case ID_USER_LIST_6:
	case ID_USER_LIST_7:
	case ID_USER_LIST_8:
	case ID_USER_LIST_9:
	case ID_USER_LIST_10:
		if( cmd == IOBN_BTNUP )
		{
			PlazaInviteList *pBtn = dynamic_cast<PlazaInviteList*>(pWnd);
			if( !pBtn->GetName().IsEmpty() )
				g_App.OpenMannerWnd( pBtn->GetName().c_str(), true );				
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
	}
}

void PlazaInviteListWnd::AllInvite()
{
	if( !m_bInviteActive )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	int i;
	vUserInviteInfo vSendList;
	for(i = 0;i < (int)m_vUserList.size();i++)
	{
		UserInviteInfo &kInfo = m_vUserList[i];
		if( kInfo.m_szName.IsEmpty() ) continue;
		if( g_App.IsAdminID( kInfo.m_szName.c_str() ) ) continue;
		
		vSendList.push_back( kInfo );
	}

	if( vSendList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	m_iPrevCurPage = m_iCurPage;
	SP2Packet kPacket( CTPK_PLAZA_USER_INVITE );
	kPacket << (int)vSendList.size();
	for(i = 0;i < (int)vSendList.size();i++)
	{
		UserInviteInfo &kInfo = vSendList[i];
		kPacket << kInfo.m_szName;
	}	
	TCPNetwork::SendToServer( kPacket );

	StartInivteWait( (int)vSendList.size(), "" );
	vSendList.clear();
}

void PlazaInviteListWnd::NameInvite()
{
	if( !m_bInviteActive )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( g_App.IsMouseBusy() )
		return;

	ioEdit *pNameEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );
	if( !pNameEdit )
		return;

	char szName[MAX_PATH] = "";
	sprintf( szName, "%s", pNameEdit->GetText() );

	pNameEdit->ClearString();
	pNameEdit->SetKeyFocus();
	g_InputBox.SetString( "" );

	if( !g_App.IsRightID( szName ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	ioHashString szIsID = szName;
	if( g_MyInfo.GetPublicID() == szIsID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_App.IsAdminID( szName ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}		

	m_iPrevCurPage = m_iCurPage;
	SP2Packet kPacket( CTPK_PLAZA_USER_INVITE );
	kPacket << 1 << szName;
	TCPNetwork::SendToServer( kPacket );
	StartInivteWait( -1, szName );
}

void PlazaInviteListWnd::StartInivteWait( int iValue, const ioHashString &szName )
{
	if( !IsShow() )
		return;

	HideWnd();
	// 초대 현황 UI SHOW
	InviteProgressWnd *pProgressWnd = dynamic_cast<InviteProgressWnd*>( g_GUIMgr.FindWnd( INVITE_PROGRESS_WND ) );
	if( pProgressWnd )
	{
		pProgressWnd->StartInivteWait( this, iValue, szName );
	}
}

void PlazaInviteListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
	ProcessSearch();		
}

void PlazaInviteListWnd::OnRender()
{
	ioWnd::OnRender();
	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 273, TAT_CENTER );

	if( m_vUserList.empty() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 151, iYPos + 120, FONT_SIZE_12, STR(3) );
	}
}

void PlazaInviteListWnd::SetUserList( SP2Packet &rkPacket )
{	
	m_vUserList.clear();

	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> iTotalList >> iCurList;
	for(i = 0;i < iCurList;i++)
	{
		DWORD dwGuildIndex, dwGuildMark;
		UserInviteInfo kList;
		rkPacket >> kList.m_szName >> kList.m_iLevel >> dwGuildIndex >> dwGuildMark;
		m_vUserList.push_back( kList );
		g_GuildInfo.SetGuildMark( kList.m_szName, dwGuildIndex, dwGuildMark );
	}    	

	m_iMaxPage = max( 0, iTotalList - 1 ) / MAX_PAGE_LIST;
	UpdateSetPage( m_iCurPage );
}

void PlazaInviteListWnd::SetPlazaInfo( int iCurUser, int iMaxUser, int iRoomLevel )
{
	m_iRoomLevel = iRoomLevel;
	if( iCurUser < iMaxUser )
		m_bInviteActive = true;
	else
		m_bInviteActive = false;
	UpdateSetPage( m_iCurPage );
}
//////////////////////////////////////////////////////////////////////////
PlazaMainOptionWnd::PlazaMainOptionWnd()
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
	m_iMaxSlot  = 0;
	m_iSlotArray= 0;

	m_bMonsterBugBear = false;
}

PlazaMainOptionWnd::~PlazaMainOptionWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void PlazaMainOptionWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
}

void PlazaMainOptionWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void PlazaMainOptionWnd::CommandOption( DWORD dwCmd )
{
	switch( dwCmd )
	{
	case OPTION_INFO:
		{
			g_App.OpenMannerWnd( m_szName.c_str(), true );
		}
		break;
	case OPTION_KICK:
		{	
			if( m_szMasterName != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );			
				return;
			}

			if( g_MyInfo.GetPublicID() == m_szName )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );			
				return;
			}			

			if( g_App.IsAdminID( m_szName.c_str() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK,  NULL, STR(3) );			
				return;
			}

			if( m_bMonsterBugBear )
			{
				g_GUIMgr.SetMsgBox( MB_OK,  NULL, "몬스터 출현으로 강퇴 기능이#일시적으로 중단됩니다." );			
				return;
			}

			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(4), m_szName.c_str() );	
		}
		break;
	case OPTION_CLOSE:
		{
			if( m_szMasterName != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );			
				return;
			}

			TCPNetwork::MouseBusy( true );
			SP2Packet kPacket( CTPK_PLAZA_COMMAND );
			kPacket << PLAZA_CMD_MAXPLAYER_CHANGE << m_iSlotArray;
			TCPNetwork::SendToServer( kPacket );			
		}
		break;
	case OPTION_OPEN:
		{
			if( m_szMasterName != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );			
				return;
			}

			TCPNetwork::MouseBusy( true );
			SP2Packet kPacket( CTPK_PLAZA_COMMAND );
			kPacket << PLAZA_CMD_MAXPLAYER_CHANGE << m_iSlotArray + 1;
			TCPNetwork::SendToServer( kPacket );			
		}
		break;
	}
}

void PlazaMainOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				//강퇴
				if( g_MyInfo.GetPublicID() == m_szName )
					return;
				if( g_App.IsAdminID( m_szName.c_str() ) )
					return;
				if( m_szMasterName == g_MyInfo.GetPublicID() )
				{
					SP2Packet kPacket( CTPK_PLAZA_COMMAND );
					kPacket << PLAZA_CMD_KICK_OUT << m_szName;
					TCPNetwork::SendToServer( kPacket );
				}
			}
		}
		break;
	}
}

void PlazaMainOptionWnd::SetOptionInfo( const ioHashString &rkName, const ioHashString &rkMasterName, int iSlotArray, int iMaxSlot, bool bMonsterBugBear )
{
	HideWnd();
	/*
	-내가 방장일 경우
	ㄴ 자기클릭 - 자기정보
	ㄴ 사람클릭 - 사람정보 / 강퇴
	ㄴ 빈곳클릭 - 닫기
	ㄴ 엑스클릭 - 열기

	-내가 일반일 경우
	ㄴ 자기클릭 - 자기정보
	ㄴ 사람클릭 - 사람정보

	-명칭
	[누구누구 정보]
	[강제퇴장 시키기]
	[슬롯 닫기]
	[슬롯 열기]

	-옵션 명령이 하나밖에 없으면 즉시 실행한다.
	*/
	m_OptionMap.clear();

	m_szName		= rkName;
	m_szMasterName  = rkMasterName;
	m_iMaxSlot		= iMaxSlot;
	m_iSlotArray	= iSlotArray;
	m_bMonsterBugBear = bMonsterBugBear;

	// 방장
	if( m_szMasterName == g_MyInfo.GetPublicID() )
	{
		if( rkName.IsEmpty() )
		{
			if( m_iSlotArray >= m_iMaxSlot )
				m_OptionMap.insert( OptionMap::value_type( OPTION_OPEN, STR(1) ) );
			else
				m_OptionMap.insert( OptionMap::value_type( OPTION_CLOSE, STR(2) ) );
		}
		else if( rkName == g_MyInfo.GetPublicID() )
		{
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, STR(3) ) );
		}
		else
		{
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(4), m_szName.c_str() );
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, szTitle ) );

			//강퇴
			if( !g_App.IsAdminID( m_szName.c_str() ) )
			{
				m_OptionMap.insert( OptionMap::value_type( OPTION_KICK, STR(5) ) );
			}
		}
	}
	else    //일반 - 즉시 실행되므로 타이틀에 신경쓰지 말자.
	{
		if( rkName.IsEmpty() )
		{	
			m_OptionMap.insert( OptionMap::value_type( OPTION_OPEN, STR(6) ) );
		}
		else
		{
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, STR(7) ) );
		}
	}

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
			int iOffsetX = GetParent()->GetXPos();
			int iOffsetY = GetParent()->GetYPos();
			if( pMouse->GetMousePos().x + GetWidth() > Setting::Width() )
				SetWndPos( ( pMouse->GetMousePos().x - GetWidth() ) - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
			else
				SetWndPos( pMouse->GetMousePos().x - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
		}
		ShowWnd();
	}
}

void PlazaMainOptionWnd::iwm_show()
{
	SetCheckParentOutSide( true );
}

void PlazaMainOptionWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void PlazaMainOptionWnd::OnProcess( float fTimePerSec )
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
				pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
			break;
		}
	}
}

void PlazaMainOptionWnd::OnRender()
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
PlazaMainWnd::PlazaMainWnd() : m_pPlayStage( NULL )
{
	m_bUserInfoUpadte = true;
	m_bMouseLBtnDown  = false;
	m_iModeSubNum     = 0;
	m_MouseOverIndex  = -1;

	m_pBarNormal	 = NULL;
	m_pBarNormalLight= NULL;
	m_pBarOver		 = NULL;
	m_pPeopleOrange  = NULL;
	m_pPeopleGray	 = NULL;
	m_pModeIconBack  = NULL;
}

PlazaMainWnd::~PlazaMainWnd()
{
	SAFEDELETE( m_pBarNormal );
	SAFEDELETE( m_pBarNormalLight );
	SAFEDELETE( m_pBarOver );
	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
	SAFEDELETE( m_pModeIconBack );
}

void PlazaMainWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BarNormal" )
	{
		SAFEDELETE( m_pBarNormal );
		m_pBarNormal = pImage;
	}
	else if( szType == "BarNormalLight" )
	{
		SAFEDELETE( m_pBarNormalLight );
		m_pBarNormalLight = pImage;
	}
	else if( szType == "BarOver" )
	{
		SAFEDELETE( m_pBarOver );
		m_pBarOver = pImage;
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
	else if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PlazaMainWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	if( bCreate )
	{
		ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
		return;
	}

	RECT rcRect;
	::SetRect( &rcRect, 0, 0, 0, 0 );

	if( !GetParent() || m_bWidthParent )
		rcRect.right = iParentWidth;
	else
		rcRect.right = m_iDefWidth;

	if( !GetParent() || m_bHeightParent )
		rcRect.bottom = iParentHeight;
	else
		rcRect.bottom = m_iDefHeight;

	rcRect.left = m_iDefXPos;
	switch( m_XPosAlign )
	{
	case UI_CENTER_ALIGN:
		rcRect.left += ( iParentWidth - rcRect.right ) / 2;
		break;
	case UI_RIGHT_ALIGN:
		rcRect.left += iParentWidth - rcRect.right;
		break;
	}

	rcRect.top = m_iDefYPos;
	switch( m_YPosAlign )
	{
	case UI_CENTER_ALIGN:
		rcRect.top += ( iParentHeight - rcRect.bottom ) / 2;
		break;
	case UI_BOTTOM_ALIGN:
		rcRect.top += iParentHeight - rcRect.bottom;
		break;
	}

	SetWndPos( rcRect.left, rcRect.top );

		POINT NullPoint = { 0 , 0 };
	m_MoveWnd.SetState( WndMove::WMH_NONE, NullPoint );
}

void PlazaMainWnd::iwm_show()
{
	Update();

	SetChildWndStyleRemove( ID_INVITE_BTN, IWS_EXACTIVE );
	if( g_GUIMgr.GetPlayStage() && g_GUIMgr.GetPlayStage()->GetBlockWorld() )
	{
		SetChildWndStyleAdd( ID_INVITE_BTN, IWS_EXACTIVE );
	}
}

void PlazaMainWnd::iwm_hide()
{
}

bool PlazaMainWnd::iwm_esc()
{
	return ioWnd::iwm_esc();
}

void PlazaMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_INVITE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GUIMgr.IsShow( PLAZA_INVITE_LIST_WND ) )
				g_GUIMgr.HideWnd( PLAZA_INVITE_LIST_WND );
			else
				g_GUIMgr.ShowWnd( PLAZA_INVITE_LIST_WND );
		}
		else if( cmd == IOEX_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드 본부에서는#초대 할 수 없습니다." );
		}
		break;
	case ID_CHANGE_INFO:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( m_PlazaInfo.m_szMasterName != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}

			if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}

			if( m_PlazaInfo.m_szMasterName == g_MyInfo.GetPublicID() )
			{
				PlazaOwnerWnd *pOwnerWnd = dynamic_cast<PlazaOwnerWnd*>(g_GUIMgr.FindWnd( PLAZA_OWNER_WND ));
				if( pOwnerWnd )
					pOwnerWnd->SetPlazaInfo( m_PlazaInfo.m_szRoomName, m_PlazaInfo.m_szRoomPW );
			}
		}
		break;
	}
}

bool PlazaMainWnd::IsUserUpdate()
{
	bool bUpdate = m_bUserInfoUpadte;
	m_bUserInfoUpadte = false;
	return bUpdate;
}

void PlazaMainWnd::InitUserInfo()
{
	m_UserInfoList.clear();
}

void PlazaMainWnd::SetUserInfo( RoundInfo &rkInfo )
{
	if( m_PlazaInfo.m_szMasterName == rkInfo.m_szName )
		m_UserInfoList.insert( m_UserInfoList.begin(), rkInfo );
	else
		m_UserInfoList.push_back( rkInfo );
}

void PlazaMainWnd::UserInfoUpdate()
{
	Update();
}

void PlazaMainWnd::Update()
{
	m_bUserInfoUpadte = true; 

	char szTitle[MAX_PATH] = "";
	if( m_PlazaInfo.m_ePlazaType == PT_COMMUNITY )
	{
		m_szPlazaType = STR(1);
		if( m_PlazaInfo.m_bRoomPW )
			SafeSprintf( szTitle, sizeof( szTitle ), STR(2), m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iMaxUserCount );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), STR(3), m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iMaxUserCount );
	}
	else if( m_PlazaInfo.m_ePlazaType == PT_GUILD )
	{
		m_szPlazaType = STR(4);
		if( m_PlazaInfo.m_bRoomPW )
			SafeSprintf( szTitle, sizeof( szTitle ), STR(5), m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iMaxUserCount );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), STR(6), m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iMaxUserCount );
	}
	else
	{
		m_szPlazaType = STR(7);
		if( m_PlazaInfo.m_bRoomPW )
			SafeSprintf( szTitle, sizeof( szTitle ), STR(8), m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iMaxUserCount );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), STR(9), m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iMaxUserCount );
	}
	SetTitleText( szTitle );

	if( m_pPlayStage )
		m_iModeSubNum = m_pPlayStage->GetModeSubNum();

	if( m_PlazaInfo.m_szMasterName == g_MyInfo.GetPublicID() )
		SetChildWndStyleRemove( ID_CHANGE_INFO, IWS_EXACTIVE );
	else
		SetChildWndStyleAdd( ID_CHANGE_INFO, IWS_EXACTIVE );
}

bool PlazaMainWnd::IsBugBearEmerge()
{
	if( m_pPlayStage == NULL ) 
		return false;

	ioTrainingMode *pTrainingMode = ToTrainingMode( m_pPlayStage->GetPlayMode() );
	if( pTrainingMode == NULL )
		return false;
	return pTrainingMode->IsBugBearEmerge();
}

void PlazaMainWnd::UpdatePlazaInfo( PlazaInfo &rkPlazaInfo )
{
	m_PlazaInfo = rkPlazaInfo;
	Update();
}

void PlazaMainWnd::SetPlazaName( ioHashString szName )
{
	m_PlazaInfo.m_szRoomName = szName;
	Update();
}

void PlazaMainWnd::SetPlazaMasterPW( ioHashString szPW )
{
	m_PlazaInfo.m_szRoomPW = szPW;
	SetPlazaPW( !m_PlazaInfo.m_szRoomPW.IsEmpty() );
}

void PlazaMainWnd::SetPlazaPW( bool bPW )
{
	m_PlazaInfo.m_bRoomPW = bPW;
	Update();
}

void PlazaMainWnd::SetPlazaLevel( int iLevel )
{
	m_PlazaInfo.m_iRoomLevel = iLevel;
	Update();
}

void PlazaMainWnd::SetPlazaMaster( ioHashString szMaster )
{
	m_PlazaInfo.m_szMasterName = szMaster;
	Update();
}

void PlazaMainWnd::SetJoinUserCount( int iJoinUserCnt )
{
	m_PlazaInfo.m_iJoinUserCount = iJoinUserCnt;
	Update();	
}

void PlazaMainWnd::SetMaxUserCount( int iMaxUserCnt )
{
	m_PlazaInfo.m_iMaxUserCount = iMaxUserCnt;
	Update();
}

void PlazaMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	ioMovingWnd::OnProcess( fTimePerSec );

	ProcessSiteClick();
}

void PlazaMainWnd::ProcessSiteClick()
{
	bool bPrevMouseDown = m_bMouseLBtnDown;
	ioHashString szPrevOverName = m_MouseOverName;

	m_bMouseLBtnDown = false;
	m_MouseOverName.Clear();
	m_MouseOverIndex = -1;
	if( !g_App.IsVisibleMouse() || g_GUIMgr.GetPreOverWnd() != this )
		return;

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
	int iRealXPos = iXPos + 18;
	int iRealYPos = iYPos + 100;
	ioLSMouse *pMouse = g_App.GetMouse();
	int iMaxLoop  = MAX_HORZ * MAX_VERT;
	for(int i = 0;i < iMaxLoop;i++)
	{
		RECT rcNameRect;
		rcNameRect.left   = iRealXPos;
		rcNameRect.top    = iRealYPos;
		rcNameRect.right  = rcNameRect.left + 85;
		rcNameRect.bottom = rcNameRect.top + 21;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			bool bEmptySlot = false;
			char szOverName[MAX_PATH] = "";
			if( i < (int)m_UserInfoList.size() )
				sprintf( szOverName, "%s", m_UserInfoList[i].m_szName.c_str() );
			else
			{
				bEmptySlot = true;
				sprintf_e( szOverName, "Empty.Slot%d", i );
				
				//
				if( m_PlazaInfo.m_szMasterName != g_MyInfo.GetPublicID() )
					break;
			}

			if( i >= Help::GetPlazaMaxPlayer() )    
				break;
			
			m_MouseOverIndex = i;
			if( szPrevOverName != szOverName )
			{
				g_SoundMgr.PlaySound( "interface/public_mouseover_00.wav" );
			}
			m_MouseOverName = szOverName;
			if( pMouse->IsLBtnDown() )
			{
				if( !bPrevMouseDown )
					g_SoundMgr.PlaySound( "interface/public_mouseclick_00.wav" );
				m_bMouseLBtnDown = true;
			}
			else if( pMouse->IsLBtnUp() )
			{
				PlazaMainOptionWnd *pOptionWnd = dynamic_cast<PlazaMainOptionWnd*>( FindChildWnd( ID_OPTION_WND ) );
				if( pOptionWnd )
				{
					if( bEmptySlot )
					{
						pOptionWnd->SetOptionInfo( "", m_PlazaInfo.m_szMasterName, i, m_PlazaInfo.m_iMaxUserCount, IsBugBearEmerge() );
					}
					else
					{
						pOptionWnd->SetOptionInfo( szOverName, m_PlazaInfo.m_szMasterName, i, m_PlazaInfo.m_iMaxUserCount, IsBugBearEmerge() );
					}
				}
			}
			break;
		}

		if( 0 == ( i + 1 ) % MAX_VERT )
		{
			iRealXPos += 87;
			iRealYPos = iYPos + 100;
		}
		else
		{
			iRealYPos += 21;
		}
	}
}

void PlazaMainWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( 0.60f, 0.60f );
		m_pModeIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		// 모드 아이콘
		ioUIRenderImage *pTypeResource = g_ModeSelectInfo.GetListModeUIResource( m_szPlazaType );
		if( pTypeResource )
		{
			pTypeResource->SetScale( 0.60f, 0.60f );
			pTypeResource->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 300.0f, m_PlazaInfo.m_szRoomName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	if( m_PlazaInfo.m_ePlazaType == PT_GUILD )
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 300.0f, "길드 본부" );
	}
	else
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 300.0f, STR(1), 
			g_ModeSelectInfo.GetPlazaMapTitle( (int)m_PlazaInfo.m_ePlazaType, m_iModeSubNum ).c_str() );
	}

	int iRealXPos = iXPos + 18;
	int iRealYPos = iYPos + 100;
	int iMaxLoop  = MAX_HORZ * MAX_VERT;
	for(int i = 0;i < iMaxLoop;i++)
	{
		if( i < (int)m_UserInfoList.size() )
		{
			OnRenderBar( iRealXPos, iRealYPos, m_PlazaInfo.m_iMaxUserCount, i, m_UserInfoList[i].m_szName, m_UserInfoList[i].m_iLevel );
		}
		else
		{
			OnRenderBar( iRealXPos, iRealYPos, m_PlazaInfo.m_iMaxUserCount, i, "", 0 );
		}

		if( 0 == ( i + 1 ) % MAX_VERT )
		{
			iRealXPos += 87;
			iRealYPos = iYPos + 100;
		}
		else
		{
			iRealYPos += 21;
		}
	}
}

void PlazaMainWnd::OnRenderBar( int iXPos, int iYPos, int iMaxSlot, int iCurSlot, const ioHashString szName, int iGradeLevel )
{
	if( !m_pPeopleOrange || !m_pPeopleGray || !m_pBarOver )
		return;

	// 바탕
	if( iCurSlot % 2 == 0 )
	{
		if( m_pBarNormal )
			m_pBarNormal->Render( iXPos, iYPos );
	}
	else if( m_pBarNormalLight )
	{
		m_pBarNormalLight->Render( iXPos, iYPos );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	// 사람 아이콘
	if( szName.IsEmpty() )
	{
		if( iCurSlot < iMaxSlot )
		{
			if( m_MouseOverIndex == iCurSlot )
			{
				m_pBarOver->Render( iXPos, iYPos );

				m_pPeopleOrange->SetColor( 255, 255, 255 );
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2, UI_RENDER_NORMAL_ADD_COLOR );
				g_FontMgr.SetTextColor( 255, 255, 255 );
			}
			else
			{
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );
			}
			g_FontMgr.PrintText( iXPos + 23, iYPos + 3, FONT_SIZE_12, STR(1) );
		}
		else
		{
			if( m_MouseOverIndex == iCurSlot )
			{
				m_pBarOver->Render( iXPos, iYPos );

				m_pPeopleGray->SetColor( 255, 255, 255 );
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2, UI_RENDER_NORMAL_ADD_COLOR );	
				g_FontMgr.SetTextColor( 255, 255, 255 );
			}
			else
			{
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );	
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );		
			}
			g_FontMgr.PrintText( iXPos + 23, iYPos + 3, FONT_SIZE_12, STR(2) );
		}
	}
	else if( m_MouseOverName == szName ) // 오버 슬롯
	{
		m_pBarOver->Render( iXPos, iYPos );

		g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
		g_FontMgr.SetTextColor( 255, 255, 255 );
	}
	else
	{
		g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, iGradeLevel, TEAM_PRIVATE_1 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	}
	g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 58.0f, szName.c_str() );
}
//////////////////////////////////////////////////////////////////////////
PlazaInvitedBtn::PlazaInvitedBtn()
{
	m_pPeopleOrange = NULL;
	m_pPeopleGray   = NULL;

	m_iSlotArray = m_iMaxSlot = 0;
	m_iGradeLevel = m_iPingStep = 0;
}

PlazaInvitedBtn::~PlazaInvitedBtn()
{
	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
}

void PlazaInvitedBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "PeopleOrange" )
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
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void PlazaInvitedBtn::iwm_create()
{
	m_PingStep.Create();
}

void PlazaInvitedBtn::SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, int iPingStep )
{
	m_iMaxSlot   = iMaxSlot;
	m_iSlotArray = iSlotArray;
	m_szName     = rkName;
	m_iGradeLevel= iGradeLevel;
	m_iPingStep  = iPingStep;

	if( m_szName.IsEmpty() )
	{
		SetInActive();
	}
	else
	{	
		SetActive();
	}
}

void PlazaInvitedBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_szName.IsEmpty() )
	{
		if( m_iSlotArray < m_iMaxSlot )
		{
			if( m_pPeopleOrange )
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
		}
		else
		{
			if( m_pPeopleGray )
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );			
		}
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );	
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 59.0f, m_szName.c_str() );
		
		m_PingStep.RenderPing( iXPos + 85, iYPos + 2, m_iPingStep, false );
	}
}

void PlazaInvitedBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
	}	
}
//////////////////////////////////////////////////////////////////////////
bool PlazaInvitedWnd::sg_bAutoRefuse = false;
PlazaInvitedWnd::PlazaInvitedWnd()
{
	m_iRoomNum = m_iRoomIndex = m_iPlazaType = m_iModeSubNum = m_iMaxPlayer = 0;
	m_dwStartTime = 0;

	m_iCurPage = 0;
	m_iMaxPage = 3;

	m_pModeIconBack = NULL;
}

PlazaInvitedWnd::~PlazaInvitedWnd()
{
	SAFEDELETE( m_pModeIconBack );
}

void PlazaInvitedWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PlazaInvitedWnd::SetInfo( int iRoomNum, SP2Packet &rkPacket )
{
	m_vUserList.clear();

	m_iRoomNum = iRoomNum;
	rkPacket >> m_iRoomIndex;
	rkPacket >> m_szRoomName;
	rkPacket >> m_szPassword;
	rkPacket >> m_iPlazaType;
	rkPacket >> m_iModeSubNum;
	rkPacket >> m_iMaxPlayer;

	bool bFriendInParty = false;
	int iUserSize;
	rkPacket >> iUserSize;
	for( int i = 0; i < min( 32, iUserSize ); i++ )
	{
		PlazaUserInfo kUserInfo;
		rkPacket >> kUserInfo.m_iGradeLevel >> kUserInfo.m_szUserID >> kUserInfo.m_dwGuildIndex >> kUserInfo.m_dwGuildMark >> kUserInfo.m_iServerPingStep;

		m_vUserList.push_back( kUserInfo );
		g_GuildInfo.SetGuildMark( kUserInfo.m_szUserID, kUserInfo.m_dwGuildIndex, kUserInfo.m_dwGuildMark );

		if( !bFriendInParty )
		{
			if( g_FriendManager.IsFriend(kUserInfo.m_szUserID) || g_GuildInfo.IsMyGuildUser( kUserInfo.m_szUserID )  )
			{
				bFriendInParty = true;
				// 친구 정보 갱신
				g_FriendManager.ClientFriendInfoUpdate( kUserInfo.m_szUserID, kUserInfo.m_iGradeLevel, UP_TRAINING );
			}
		}
	}
	if( !bFriendInParty )
	{
		if( sg_bAutoRefuse )
		{
			return;
		}
	}

	g_SoundMgr.PlaySound( "interface/team_invite.wav" );
	m_dwStartTime = FRAMEGETTIME();

	char szTitle[MAX_PATH] = "";
	if( m_iPlazaType == PT_COMMUNITY )
	{
		m_szPlazaType = STR(1);
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2), (int)m_vUserList.size(), m_iMaxPlayer );
	}
	else if( m_iPlazaType == PT_GUILD )
	{
		m_szPlazaType = STR(3);
		SafeSprintf( szTitle, sizeof( szTitle ), STR(4),(int) m_vUserList.size(), m_iMaxPlayer );
	}
	else
	{
		m_szPlazaType = STR(5);
		SafeSprintf( szTitle, sizeof( szTitle ), STR(6), (int)m_vUserList.size(), m_iMaxPlayer );
	}
	SetTitleText( szTitle );

	if( IsShow() )
		HideWnd();
	ShowWnd();
}

void PlazaInvitedWnd::JoinPlaza()
{
	if( m_iPlazaType == PT_GUILD )
	{
		if( !g_GuildInfo.IsGuild() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			HideWnd();
			return;
		}
	}

	SP2Packet kPacket( CTPK_SEARCH_PLAZA_ROOM );
	kPacket << m_iRoomIndex << true << m_szPassword;
	TCPNetwork::SendToServer( kPacket );
	HideWnd();
	TCPNetwork::MouseBusy( true );
}

void PlazaInvitedWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_REFUSE:
		if( cmd == IOBN_BTNUP )
		{			
			HideWnd();
		}
		break;
	case ID_AUTO_REFUSE:
		if( cmd == IOBN_BTNUP )
		{	
			sg_bAutoRefuse = !sg_bAutoRefuse;
			if( sg_bAutoRefuse )
			{
				HideWnd();
			}			
		}
		break;
	case ID_LEFT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdatePage( m_iCurPage - 1 );
		}
		break;
	case ID_RIGHT_PAGE:
		if( cmd == IOBN_BTNUP )
		{	
			UpdatePage( m_iCurPage + 1 );
		}
		break;
	case ID_AGREE:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else if( g_MyInfo.GetCharCount() == 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( g_BattleRoomMgr.IsBattleRoom() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				HideWnd();
				return;
			}
			else if( g_LadderTeamMgr.IsLadderTeam() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				HideWnd();
				return;
			}
			else
			{
				JoinPlaza();
			}
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
		if( cmd == IOBN_BTNUP )
		{
			PlazaInvitedBtn *pUserBtn = dynamic_cast<PlazaInvitedBtn*>(pWnd);
			if( pUserBtn )
				g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), true );
		}
		break;
	}
}

const PlazaUserInfo &PlazaInvitedWnd::GetUserInfo( int iArray )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		if( --iArray < 0 )
			return m_vUserList[i];
	}

	static PlazaUserInfo kReturnInfo;
	return kReturnInfo;
}

void PlazaInvitedWnd::UpdatePage( int iCurPage )
{
	m_iCurPage = max( 0, iCurPage );
	m_iCurPage = min( m_iCurPage, m_iMaxPage );
	const int iStartArray = m_iCurPage * MAX_PAGE_LIST;
	
	int i = 0;
	int iSlotArray = 0;		
	for(i = 0,iSlotArray = iStartArray;i < MAX_PAGE_LIST;i++,iSlotArray++)
	{
		PlazaInvitedBtn *pPlazaUserBtn = dynamic_cast<PlazaInvitedBtn*>(FindChildWnd( ID_LIST_1 + i ));
		if( pPlazaUserBtn )
		{
			const PlazaUserInfo &rkInfo = GetUserInfo( iSlotArray );
			pPlazaUserBtn->SetMemberInfo( m_iMaxPlayer, iSlotArray, rkInfo.m_szUserID, rkInfo.m_iGradeLevel, rkInfo.m_iServerPingStep );
		}
	}
}

void PlazaInvitedWnd::iwm_show()
{
	ioButton *pAgreeBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_AGREE ) );
	if( pAgreeBtn )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), INVITE_DELAY_SEC );
		pAgreeBtn->SetTitleText( szTitle );
	}

	ioCheckButton *pCheckBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_AUTO_REFUSE ) );
	if( pCheckBtn )
		pCheckBtn->SetCheck( sg_bAutoRefuse );

	UpdatePage( 0 );	
}

void PlazaInvitedWnd::iwm_hide()
{
}

void PlazaInvitedWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( 0.60f, 0.60f );
		m_pModeIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		// 모드 아이콘
		ioUIRenderImage *pTypeResource = g_ModeSelectInfo.GetListModeUIResource( m_szPlazaType );
		if( pTypeResource )
		{
			pTypeResource->SetScale( 0.60f, 0.60f );
			pTypeResource->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 175.0f, m_szRoomName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 175.0f, STR(1), g_ModeSelectInfo.GetPlazaMapTitle( m_iPlazaType, m_iModeSubNum ).c_str() );
	

	////
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_iCurPage + 1 );

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), m_iMaxPage + 1 );
		kPrinter.PrintFullText( iXPos + 128, iYPos + 193, TAT_CENTER );
	}
}

void PlazaInvitedWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;	

	ioMovingWnd::OnProcess( fTimePerSec );

	if(m_dwStartTime == 0)
		return;

	if( FRAMEGETTIME() - m_dwStartTime >= INVITE_DELAY_SEC * 1000 )
	{
		HideWnd();	
		m_dwStartTime = 0;
	}
	else
	{
		ioButton *pBtn = (ioButton*)FindChildWnd( ID_AGREE );
		if( pBtn )
		{
			DWORD dwSec = ((float)(FRAMEGETTIME() - m_dwStartTime) / 1000);
			int   iSec  = max( 0, INVITE_DELAY_SEC - dwSec );	
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), iSec );
			pBtn->SetTitleText( szTitle );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
PlazaModeNMapListWnd::PlazaModeNMapListWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;

	m_pOpenBtn  = NULL;
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

PlazaModeNMapListWnd::~PlazaModeNMapListWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void PlazaModeNMapListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
}

void PlazaModeNMapListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void PlazaModeNMapListWnd::SetList( ioWnd *pOpenBtn, int iStartXPos, int iStartYPos )
{
	m_pOpenBtn = pOpenBtn;
	m_ListData.clear();

	SetWndPos( iStartXPos, iStartYPos );
}

void PlazaModeNMapListWnd::AddList( int iIndex, const ioHashString &szTitle, ioUIRenderImage *pMarkRes )
{
	ListData kListData;
	kListData.m_iMagicCode = iIndex;
	kListData.m_szTitle    = szTitle;
	kListData.m_pMarkRes   = pMarkRes;
	
	m_ListData.push_back( kListData );
}

void PlazaModeNMapListWnd::ShowListWnd()
{
	if( IsShow() )
	{
		HideWnd();
	}
	else
	{
		ShowWnd();
	}
}

void PlazaModeNMapListWnd::iwm_show()
{	
	if( m_ListData.empty() )
		HideWnd();
	else
	{
		m_iCurPos = 0;
		if( (int)m_ListData.size() <= MAX_PAGE_LIST )
		{
			HideChildWnd( ID_VERT_SCROLL );
		}
		else
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
			{
				pScroll->SetScrollRange( 0, m_ListData.size() );
				pScroll->SetScrollPos( m_iCurPos );
				pScroll->ShowWnd();
			}
		}

		// 윈도우 사이즈 결정	
		SetSize( GetWidth(), ( min( MAX_PAGE_LIST, m_ListData.size() ) * m_iHeightGap ) + m_iAddHeight );
		SetCheckParentOutSide( true );
	}
}

void PlazaModeNMapListWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void PlazaModeNMapListWnd::iwm_wheel( int zDelta )
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

void PlazaModeNMapListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void PlazaModeNMapListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_OverListItem.m_szTitle.Clear();
	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != m_pOpenBtn &&
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
	if( (int)m_ListData.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_ListData.size();
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
			m_OverListItem  = m_ListData[iListPos];
			if( pMouse->IsLBtnUp() )
			{
				if( GetParent() )
					GetParent()->iwm_command( m_pOpenBtn, PlazaCreateWnd::ID_MODE_MAP_LIST, m_OverListItem.m_iMagicCode );
				HideWnd();
			}
			break;
		}
	}
}

void PlazaModeNMapListWnd::OnRender()
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
	if( (int)m_ListData.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_ListData.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		bDarkRender = !bDarkRender;
		if( m_OverListItem.m_szTitle == m_ListData[iListPos].m_szTitle )
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

		g_FontMgr.PrintTextWidthCut( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, 181.0f - iCutWidth, 
									 m_ListData[iListPos].m_szTitle.c_str() );

		if( m_ListData[iListPos].m_pMarkRes )
		{
			int iTitleSize = g_FontMgr.GetTextWidthCutSize( m_ListData[iListPos].m_szTitle.c_str(), TS_NORMAL, FONT_SIZE_12, 181.0f - iCutWidth );
			m_ListData[iListPos].m_pMarkRes->Render( iXPos + m_iListXPos + 7 + iTitleSize, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 5 );
		}
	}
}

bool PlazaModeNMapListWnd::GetIndexToXYPos( int iSelectIndex, float &rfXPos, float &rfYPos )
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_ListData.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	int iListSize = m_ListData.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		if( m_ListData[iListPos].m_iMagicCode == iSelectIndex )
		{
			rfXPos = iXPos + GetWidth() / 2;
			rfYPos = iYPos + m_iListYPos + iIndex * m_iHeightGap;
			return true;
		}
	}
	
	// 없으면 스크롤
	ioScroll *pScroll = dynamic_cast<ioScroll*>(FindChildWnd( ID_VERT_SCROLL ));
	if( pScroll && pScroll->IsShow() )
	{
		rfXPos = pScroll->GetDerivedPosX() + ( pScroll->GetWidth() / 2 );
		rfYPos = pScroll->GetDerivedPosY();
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
PlazaCreateWnd::PlazaCreateWnd() : m_pNameEdit( NULL ), m_pPWEdit( NULL ), m_pPreEdit( NULL )
{
	m_iMapIndex = -1;
	m_ePlazaType = PT_BATTLE;
	m_iMaxPlayer = MAX_PLAYER;
}

PlazaCreateWnd::~PlazaCreateWnd()
{

}

void PlazaCreateWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	//
	m_pPWEdit = FindChildWnd( ID_EDIT_PW );
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}

	m_pNameEdit = FindChildWnd( ID_EDIT_NAME );
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}

	m_iMapIndex = -1;
	m_ePlazaType = PT_BATTLE;
	ioWnd *pModeBtn = FindChildWnd( ID_CHANGE_MODE );
	if( pModeBtn )
	{
		pModeBtn->SetTitleText( g_ModeSelectInfo.GetPlazaModeTitle( (int)m_ePlazaType ).c_str() );
	}

	ioWnd *pMapBtn = FindChildWnd( ID_CHANGE_MAP );
	if( pMapBtn )
	{
		pMapBtn->SetTitleText( g_ModeSelectInfo.GetPlazaMapTitle( (int)m_ePlazaType, m_iMapIndex ).c_str() );
	}
	SetChildActive( ID_CHANGE_MODE );
	SetChildActive( ID_CHANGE_MAP );
}

void PlazaCreateWnd::iwm_hide()
{
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->KillKeyFocus();
	}
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

bool PlazaCreateWnd::iwm_esc()
{
	ioWnd *pWnd = FindChildWnd( ID_MODE_MAP_LIST );
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->HideWnd();
		return true;
	}
	return ioWnd::iwm_esc();
}

void PlazaCreateWnd::CreatePlaza()
{
	if( g_App.IsMouseBusy() ) return;	
	
	if( m_ePlazaType == PT_NONE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}

	if( m_iMapIndex == -2 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return;
	}

	if( g_MyInfo.IsTutorialUser() )
	{
		HideWnd();
		g_GUIMgr.SetPrevMsgBox( this, MB_OK, NULL, STR(1) );
		return;
	}

	if( g_MyInfo.GetCharCount() == 0 )
	{
		HideWnd();
		g_GUIMgr.SetPrevMsgBox( this, MB_OK, NULL, STR(2) );
		return;
	}

	if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( !COMPARE( m_iMaxPlayer, 1, Help::GetPlazaMaxPlayer() + 1 ) )
	{
		HideWnd();
		g_GUIMgr.SetPrevMsgBox( this, MB_OK, NULL, STR(4) );
		return;
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
		return;
	}

	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
		return;
	}

	if( m_ePlazaType == PT_GUILD )
	{
		if( !g_GuildInfo.IsGuild() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
			return;
		}
	}

	TimeGateSearch *pSearchWnd = dynamic_cast<TimeGateSearch*>(g_GUIMgr.FindWnd( TIMEGATE_SEARCH ));
	if( pSearchWnd )
	{
		pSearchWnd->SetReserve();
		pSearchWnd->SetSearching( STR(8), TimeGateSearch::ST_PLAZA );
	}

	ioHashString szPlazaName;
	ioEdit *pEdit = (ioEdit*)m_pNameEdit;
	if( pEdit )
	{
		szPlazaName = g_SlangUtil.ConvertString( pEdit->GetText() );
	}

    ioHashString szPlazaPW;
	pEdit = (ioEdit*)m_pPWEdit;
	if( pEdit )
	{
		szPlazaPW = pEdit->GetText();
	}

	SP2Packet kPacket( CTPK_CREATE_PLAZA );
	kPacket << szPlazaName << szPlazaPW << m_iMaxPlayer << (int)m_ePlazaType << m_iMapIndex;
	TCPNetwork::SendToServer( kPacket );
	HideWnd();
	TCPNetwork::MouseBusy( true );
}

void PlazaCreateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_CREATE:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			CreatePlaza();
		}
		break;
	case ID_LEFT:
		if( cmd == IOBN_BTNUP )
		{
			m_iMaxPlayer--;
			if( m_iMaxPlayer <= 0 )
				m_iMaxPlayer = Help::GetPlazaMaxPlayer();
		}
		break;
	case ID_RIGHT:
		if( cmd == IOBN_BTNUP )
		{
			m_iMaxPlayer++;
			if( m_iMaxPlayer > Help::GetPlazaMaxPlayer() )
				m_iMaxPlayer = 1;
		}
		break;
	case ID_CHANGE_MODE:
		if( cmd == IOBN_BTNUP )
		{			
			PlazaModeNMapListWnd *pModeList = dynamic_cast<PlazaModeNMapListWnd *>(FindChildWnd( ID_MODE_MAP_LIST ));
			if( pModeList )
			{
				pModeList->SetList( pWnd, pWnd->GetXPos(), pWnd->GetYPos() + 24 );
				g_ModeSelectInfo.GetPlazaModeUIList( pModeList );
				pModeList->ShowListWnd();
			}
		}
		else if( cmd == ID_MODE_MAP_LIST )
		{
			PlazaType eSelectType = (PlazaType)g_ModeSelectInfo.CheckPlazaModeIndex( (int)param );
			if( m_ePlazaType == eSelectType )
				return;

			m_ePlazaType = eSelectType;	
			pWnd->SetTitleText( g_ModeSelectInfo.GetPlazaModeTitle( (int)m_ePlazaType ).c_str() );

			SetChildWndStyleRemove( ID_CHANGE_MAP, IWS_EXACTIVE );
		}
		break;
	case ID_CHANGE_MAP:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( m_ePlazaType == PT_NONE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else
			{
				PlazaModeNMapListWnd *pMapList = dynamic_cast<PlazaModeNMapListWnd *>(FindChildWnd( ID_MODE_MAP_LIST ));
				if( pMapList )
				{
					pMapList->SetList( pWnd, pWnd->GetXPos(), pWnd->GetYPos() + 24 );
					g_ModeSelectInfo.GetPlazaMapUIList( pMapList, (int)m_ePlazaType );
					pMapList->ShowListWnd();
				}
			}			
		}
		else if( cmd == ID_MODE_MAP_LIST )
		{
			int iMapIndex = g_ModeSelectInfo.CheckPlazaMapIndex( (int)m_ePlazaType, (int)param );
			if( iMapIndex == m_iMapIndex )
				return;

			m_iMapIndex = iMapIndex;
			pWnd->SetTitleText( g_ModeSelectInfo.GetPlazaMapTitle( (int)m_ePlazaType, m_iMapIndex ).c_str() );

			SetChildWndStyleRemove( ID_CREATE, IWS_EXACTIVE );
		}
		break;
	case ID_EDIT_NAME:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pPWEdit )
				{
					ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
					pPWEdit->SetKeyFocus();
				}
			}			
			else if( param == 1 )
			{
				CreatePlaza();	
			}
		}
		break;
	case ID_EDIT_PW:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pNameEdit )
				{
					ioEdit *pNameEdit = (ioEdit*)m_pNameEdit;
					pNameEdit->SetKeyFocus();
				}				
			}			
			else if( param == 1 )
			{
				CreatePlaza();
			}
		}
		break;
	}
}

void PlazaCreateWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	g_FontMgr.PrintText( iXPos + 16, iYPos + 57, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 80, FONT_SIZE_12, STR(2) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 109, FONT_SIZE_12, STR(3) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 133, FONT_SIZE_12, STR(4) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 157, FONT_SIZE_12, STR(5) );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 179, iYPos + 157, FONT_SIZE_12, STR(6), m_iMaxPlayer );
}

bool PlazaCreateWnd::QuestGuide( int iModeIndex, int iMapIndex, float &rfXPos, float &rfYPos )
{
	// 모드 체크
	if( iModeIndex != (int)m_ePlazaType )
	{
		ioWnd *pSelectBtn = FindChildWnd( ID_CHANGE_MODE );
		PlazaModeNMapListWnd *pModeList = dynamic_cast<PlazaModeNMapListWnd *>(FindChildWnd( ID_MODE_MAP_LIST ));
		if( pModeList && pModeList->IsShow() )
		{
			if( pModeList->GetOpenBtn() == pSelectBtn )
			{
				if( pModeList->GetIndexToXYPos( iModeIndex, rfXPos, rfYPos ) )
					return true;
			}
		}
		else if( pSelectBtn )
		{
			rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
			rfYPos = pSelectBtn->GetDerivedPosY();
			return true;			
		}
		return false;
	}

	// 맵 체크
	if( iMapIndex != m_iMapIndex )
	{
		ioWnd *pSelectBtn = FindChildWnd( ID_CHANGE_MAP );
		PlazaModeNMapListWnd *pMapList = dynamic_cast<PlazaModeNMapListWnd *>(FindChildWnd( ID_MODE_MAP_LIST ));
		if( pMapList && pMapList->IsShow() )
		{
			if( pMapList->GetOpenBtn() == pSelectBtn )
			{
				if( pMapList->GetIndexToXYPos( iMapIndex, rfXPos, rfYPos ) )
					return true;
			}
		}
		else if( pSelectBtn )
		{
			rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
			rfYPos = pSelectBtn->GetDerivedPosY();
			return true;			
		}
		return false;
	}

	// 생성 버튼
	ioWnd *pCreateBtn = FindChildWnd( ID_CREATE );
	if( pCreateBtn && pCreateBtn->IsShow() )
	{
		rfXPos = pCreateBtn->GetDerivedPosX() + ( pCreateBtn->GetWidth() / 2 );
		rfYPos = pCreateBtn->GetDerivedPosY();
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
PlazaInfoWnd::PlazaInfoWnd()
{
	m_pBarNormal		= NULL;
	m_pBarNormalLight	= NULL;
	m_pBarOver			= NULL;
	m_pPeopleOrange		= NULL;
	m_pPeopleGray		= NULL;
	m_pModeIconBack		= NULL;

	m_bMouseLBtnDown    = false;
	m_bStateBackup		= false;
	m_iModeSubNum       = 0;
}

PlazaInfoWnd::~PlazaInfoWnd()
{
	m_vUserList.clear();

	SAFEDELETE( m_pBarNormal );
	SAFEDELETE( m_pBarNormalLight );
	SAFEDELETE( m_pBarOver );
	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
	SAFEDELETE( m_pModeIconBack );
}

void PlazaInfoWnd::iwm_create()
{
}

void PlazaInfoWnd::SetInfo( SP2Packet &rkPacket )
{
	m_vUserList.clear();

	int iPlazaType, iState;
	rkPacket >> m_PlazaInfo.m_iRoomIndex;
	rkPacket >> m_PlazaInfo.m_szRoomName;
	rkPacket >> m_PlazaInfo.m_szMasterName;
	rkPacket >> m_PlazaInfo.m_iJoinUserCount;
	rkPacket >> m_PlazaInfo.m_iPlayUserCount;
	rkPacket >> m_PlazaInfo.m_iMaxUserCount;
	rkPacket >> m_PlazaInfo.m_szRoomPW;
	rkPacket >> m_PlazaInfo.m_iRoomLevel;
	rkPacket >> iPlazaType;
	rkPacket >> m_iModeSubNum;
	m_PlazaInfo.m_ePlazaType = (PlazaType)iPlazaType;

	rkPacket >> iState;
	if( !m_bStateBackup )
		m_PlazaInfo.m_iState = iState;
	SetPassword( !m_PlazaInfo.m_szRoomPW.IsEmpty() );


	if( g_App.IsMeDeveloper() )
	{
		// 개발자 수준 매칭 해제
		if( m_PlazaInfo.m_iState == PlazaInfo::PRS_NOT_MIN_LEVEL_MATCH || m_PlazaInfo.m_iState == PlazaInfo::PRS_NOT_MAX_LEVEL_MATCH )
			m_PlazaInfo.m_iState = PlazaInfo::PRS_ACTIVE;
	}

	int iSize;
	rkPacket >> iSize;

	if( iSize > 0 )
	{
		for(int i = 0;i < iSize;i++)
		{
			PlazaUserInfo kUserInfo;
			rkPacket >> kUserInfo.m_iGradeLevel >> kUserInfo.m_szUserID >> kUserInfo.m_dwGuildIndex >> kUserInfo.m_dwGuildMark >> kUserInfo.m_iServerPingStep;
			m_vUserList.push_back( kUserInfo );

			g_GuildInfo.SetGuildMark( kUserInfo.m_szUserID, kUserInfo.m_dwGuildIndex, kUserInfo.m_dwGuildMark );
		}
	}	

	// UI TITLE	
	char szTitle[MAX_PATH] = "";
	if( m_PlazaInfo.m_ePlazaType == PT_COMMUNITY )
	{
		m_szPlazaType = STR(1);
		if( m_PlazaInfo.m_bRoomPW )
			SafeSprintf( szTitle, sizeof( szTitle ), STR(2), (int)m_vUserList.size(), m_PlazaInfo.m_iMaxUserCount );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), STR(3), (int)m_vUserList.size(), m_PlazaInfo.m_iMaxUserCount );
	}
	else if( m_PlazaInfo.m_ePlazaType == PT_GUILD )
	{
		m_szPlazaType = STR(4);
		if( m_PlazaInfo.m_bRoomPW )
			SafeSprintf( szTitle, sizeof( szTitle ), STR(5),(int) m_vUserList.size(), m_PlazaInfo.m_iMaxUserCount );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), STR(6),(int) m_vUserList.size(), m_PlazaInfo.m_iMaxUserCount );
	}
	else
	{
		m_szPlazaType = STR(7);
		if( m_PlazaInfo.m_bRoomPW )
			SafeSprintf( szTitle, sizeof( szTitle ), STR(8), (int)m_vUserList.size(), m_PlazaInfo.m_iMaxUserCount );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), STR(9), (int)m_vUserList.size(), m_PlazaInfo.m_iMaxUserCount );
	}
	SetTitleText( szTitle );

	if( !IsShow() )
		ShowWnd();
	else
		iwm_show();
}

void PlazaInfoWnd::SetPassword( bool bPassword )
{
	m_PlazaInfo.m_bRoomPW = bPassword;
}

void PlazaInfoWnd::SetState( int iState )
{
	m_PlazaInfo.m_iState = iState;
	m_bStateBackup       = true;
}

void PlazaInfoWnd::UpdatePlazaState( int iPlazaIndex, int iState )
{
	if( !IsShow() ) return;

	if( m_PlazaInfo.m_iRoomIndex == iPlazaIndex )
	{
		if( m_PlazaInfo.m_iState != iState )
		{
			m_PlazaInfo.m_iState = iState;
		}
	}
}

void PlazaInfoWnd::ProcessSiteClick()
{
	bool bPrevMouseDown = m_bMouseLBtnDown;
	ioHashString szPrevOverName = m_MouseOverName;

	m_bMouseLBtnDown = false;
	m_MouseOverName.Clear();
	if( !g_App.IsVisibleMouse() || g_GUIMgr.GetPreOverWnd() != this )
		return;

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iRealXPos = iXPos + 18;
	int iRealYPos = iYPos + 100;
	ioLSMouse *pMouse = g_App.GetMouse();
	int iUserSize = m_vUserList.size();
	for( int i=0 ; i < iUserSize ;i++)
	{
		PlazaUserInfo &kUserInfo = m_vUserList[i];

		RECT rcNameRect;
		rcNameRect.left   = iRealXPos;
		rcNameRect.top    = iRealYPos;
		rcNameRect.right  = rcNameRect.left + 85;
		rcNameRect.bottom = rcNameRect.top + 21;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			if( szPrevOverName != kUserInfo.m_szUserID )
			{
				g_SoundMgr.PlaySound( "interface/public_mouseover_00.wav" );
			}

			m_MouseOverName = kUserInfo.m_szUserID;
			if( pMouse->IsLBtnDown() )
			{
				if( !bPrevMouseDown )
					g_SoundMgr.PlaySound( "interface/public_mouseclick_00.wav" );
				m_bMouseLBtnDown = true;
			}
			else if( pMouse->IsLBtnUp() )
			{
				g_App.OpenMannerWnd( m_MouseOverName.c_str(), true );
			}
			else if( pMouse->IsMouseShowing() )
				pMouse->SetToolTipID( m_MouseOverName, UserInfoToolTip::TT_GLOBAL );
			break;
		}

		if( 0 == ( i + 1 ) % MAX_VERT )
		{
			iRealXPos += 87;
			iRealYPos = iYPos + 100;
		}
		else
		{
			iRealYPos += 21;
		}
	}
}

void PlazaInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BarNormal" )
	{
		SAFEDELETE( m_pBarNormal );
		m_pBarNormal = pImage;
	}
	else if( szType == "BarNormalLight" )
	{
		SAFEDELETE( m_pBarNormalLight );
		m_pBarNormalLight = pImage;
	}
	else if( szType == "BarOver" )
	{
		SAFEDELETE( m_pBarOver );
		m_pBarOver = pImage;
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
	else if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PlazaInfoWnd::iwm_show()
{
	if( m_PlazaInfo.m_iState == PlazaInfo::PRS_ACTIVE)
		SetChildWndStyleRemove( ID_ENTER_PLAZA, IWS_EXACTIVE );
	else
		SetChildWndStyleAdd( ID_ENTER_PLAZA, IWS_EXACTIVE );
}

void PlazaInfoWnd::iwm_hide()
{
	m_bStateBackup = false;
}

void PlazaInfoWnd::JoinPlazaRoom()
{
	if( g_App.IsMouseBusy() ) return;

	if( g_MyInfo.IsTutorialUser() )
	{	
		HideWnd();
		g_GUIMgr.SetPrevMsgBox( this, MB_OK, NULL, STR(1) );
		return;
	}

	if( g_MyInfo.GetCharCount() == 0 )
	{
		HideWnd();
		g_GUIMgr.SetPrevMsgBox( this, MB_OK, NULL, STR(2) );
		return;
	}

	if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_MyInfo.IsKickOutPlaza( m_PlazaInfo.m_iRoomIndex ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}

	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return;
	}

	if( m_PlazaInfo.m_ePlazaType == PT_GUILD )
	{
		if( !g_GuildInfo.IsGuild() )
		{
			HideWnd();
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
			return;
		}		
	}

	SP2Packet kPacket( CTPK_SEARCH_PLAZA_ROOM );
	kPacket << m_PlazaInfo.m_iRoomIndex << false;
	TCPNetwork::SendToServer( kPacket );
	HideWnd();
	TCPNetwork::MouseBusy( true );
}

void PlazaInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_ENTER_PLAZA:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( m_PlazaInfo.m_ePlazaType == PT_GUILD && !g_GuildInfo.IsGuild() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( m_PlazaInfo.m_iState == PlazaInfo::PRS_FULL_USER )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			else if( m_PlazaInfo.m_iState == PlazaInfo::PRS_NOT_MIN_LEVEL_MATCH && !g_App.IsMeDeveloper(true) )	// 개발자 제외 2020-10-15
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), g_MyInfo.GetPublicID().c_str() );
			}
			else if( m_PlazaInfo.m_iState == PlazaInfo::PRS_NOT_MAX_LEVEL_MATCH && !g_App.IsMeDeveloper(true) )	// 개발자 제외 2020-10-15
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(4), g_MyInfo.GetPublicID().c_str() );
			}
			else if( m_PlazaInfo.m_bRoomPW )
			{
				PlazaPasswordWnd *pPasswordWnd = dynamic_cast<PlazaPasswordWnd*>(g_GUIMgr.FindWnd( PLAZA_PASSWORD_WND ));
				if( pPasswordWnd )
					pPasswordWnd->SetPlazaInfo( m_PlazaInfo.m_iRoomIndex, m_PlazaInfo.m_iRoomLevel, m_PlazaInfo.m_szRoomName,
												m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iPlayUserCount, m_PlazaInfo.m_iMaxUserCount, m_PlazaInfo.m_ePlazaType );
			}
			else
			{
				JoinPlazaRoom();
			}
			HideWnd();
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( m_PlazaInfo.m_iRoomIndex != 0 )
				{
					if( m_PlazaInfo.m_bRoomPW )
					{
						PlazaPasswordWnd *pPasswordWnd = dynamic_cast<PlazaPasswordWnd*>(g_GUIMgr.FindWnd( PLAZA_PASSWORD_WND ));
						if( pPasswordWnd )
							pPasswordWnd->SetPlazaInfo( m_PlazaInfo.m_iRoomIndex, m_PlazaInfo.m_iRoomLevel, m_PlazaInfo.m_szRoomName,
							m_PlazaInfo.m_iJoinUserCount, m_PlazaInfo.m_iPlayUserCount, m_PlazaInfo.m_iMaxUserCount, m_PlazaInfo.m_ePlazaType );
					}
					else
					{
						JoinPlazaRoom();
					}
				}
				HideWnd();
			}
		}
		break;
	}
}

void PlazaInfoWnd::OnRenderBar( int iXPos, int iYPos, int iMaxSlot, int iCurSlot, const ioHashString szName, int iGradeLevel )
{
	// 바탕
	if( iCurSlot % 2 == 0 )
	{
		if( m_pBarNormal )
			m_pBarNormal->Render( iXPos, iYPos );
	}
	else if( m_pBarNormalLight )
	{
		m_pBarNormalLight->Render( iXPos, iYPos );
	}
	
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	// 사람 아이콘
	if( szName.IsEmpty() )
	{
		if( iCurSlot < iMaxSlot )
		{
			if( m_pPeopleOrange )
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
		}
		else
		{
			if( m_pPeopleGray )
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );			
		}
	}
	else if( m_MouseOverName == szName ) // 오버 슬롯
	{
		if( m_pBarOver )
			m_pBarOver->Render( iXPos, iYPos );

		g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
		g_FontMgr.SetTextColor( 255, 255, 255 );
	}
	else
	{
		g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, iGradeLevel, TEAM_PRIVATE_1 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	}
	g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 58.0f, szName.c_str() );
}

void PlazaInfoWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( 0.60f, 0.60f );
		m_pModeIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		// 모드 아이콘
		ioUIRenderImage *pTypeResource = g_ModeSelectInfo.GetListModeUIResource( m_szPlazaType );
		if( pTypeResource )
		{
			pTypeResource->SetScale( 0.60f, 0.60f );
			pTypeResource->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 300.0f, m_PlazaInfo.m_szRoomName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 300.0f, STR(1), 
								 g_ModeSelectInfo.GetPlazaMapTitle( (int)m_PlazaInfo.m_ePlazaType, m_iModeSubNum ).c_str() );

	int iRealXPos = iXPos + 18;
	int iRealYPos = iYPos + 100;
	int iMaxLoop  = MAX_HORZ * MAX_VERT;
	for(int i = 0;i < iMaxLoop;i++)
	{
		if( i < (int)m_vUserList.size() )
		{
			OnRenderBar( iRealXPos, iRealYPos, m_PlazaInfo.m_iMaxUserCount, i, m_vUserList[i].m_szUserID, m_vUserList[i].m_iGradeLevel );
		}
		else
		{
			OnRenderBar( iRealXPos, iRealYPos, m_PlazaInfo.m_iMaxUserCount, i, "", 0 );
		}

		if( 0 == ( i + 1 ) % MAX_VERT )
		{
			iRealXPos += 87;
			iRealYPos = iYPos + 100;
		}
		else
		{
			iRealYPos += 21;
		}
	}
}

void PlazaInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	ioMovingWnd::OnProcess( fTimePerSec );
	
	ProcessSiteClick();
}
//////////////////////////////////////////////////////////////////////////
LobbyPlazaInfoWnd::LobbyPlazaInfoWnd()
{
	m_dwTabID = ID_MISSION_BTN;
	m_bIsNoviceGrade = false;
}

LobbyPlazaInfoWnd::~LobbyPlazaInfoWnd()
{
}

void LobbyPlazaInfoWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/lobbyquestnewalramwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/lobbymainmissionwnd.xml", this );
}

void LobbyPlazaInfoWnd::iwm_show()
{
	ChangeTab( ID_MISSION_BTN );

	HideChildWnd( ID_NOVICE_GRADE_BTN );
	HideChildWnd( ID_NOVICE_GRADE_TOOTIP_BTN );

	if ( g_NoviceGradeBenefitMgr.IsBenefitGrade( g_MyInfo.GetGradeLevel() ) )
	{
		ShowChildWnd( ID_NOVICE_GRADE_BTN );
		ShowChildWnd( ID_NOVICE_GRADE_TOOTIP_BTN );
		m_bIsNoviceGrade = true;
	}
}

void LobbyPlazaInfoWnd::iwm_hide()
{
	HideChildWnd( ID_MISSION_LIST_WND );
	HideChildWnd( ID_QUEST_ALARM_WND );
	HideChildWnd( ID_NOVICE_GRADE_BTN );
	HideChildWnd( ID_NOVICE_GRADE_TOOTIP_BTN );
	m_bIsNoviceGrade = false;
}

void LobbyPlazaInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_MY_INFO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_App.OpenMannerWnd( g_MyInfo.GetPublicID().c_str(), true );
		}
		break;
	case ID_QUEST_BTN:
	case ID_MISSION_BTN:
		{
			if( cmd == IOBN_BTNDOWN )
			{
				ChangeTab( dwID );
			}
		}
		break;
	case ID_NOVICE_GRADE_TOOTIP_BTN:
		{
			if( cmd == IOWN_OVERED )
			{
				ShowChildWnd( ID_NOVICE_GRADE_TOOTIP );
			}
			else if( cmd == IOWN_LEAVED )
			{
				HideChildWnd( ID_NOVICE_GRADE_TOOTIP );
			}
		}
		break;
	case ID_TIME_GATE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			TimeGateBtn *pBtn = dynamic_cast<TimeGateBtn*>( pWnd );
			if( pBtn->GetTimeGateState() != TimeGateBtn::eActive )
				return;

			TimeGateResultWnd* pResultWnd = dynamic_cast<TimeGateResultWnd*>( g_GUIMgr.FindWnd( TIMEGATE_RESULT_WND ) );
			if( pResultWnd )
				pResultWnd->SendPakcet();
		}
		break;
	}
}

void LobbyPlazaInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	if ( g_MissionManager.IsChangeMission() )
		UpdateRadioBtn();

	SetNamedTitleSparkle();
}

void LobbyPlazaInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 12, 66, 111 );
	g_FontMgr.SetTextColor( 255, 255, 255 );

	if ( m_bIsNoviceGrade )
		g_FontMgr.PrintTextWidthCut( iXPos + 59, iYPos + 11, FONT_SIZE_17, 105.0f, g_MyInfo.GetPublicID().c_str() );
	else
	{
		g_LevelMgr.RenderGrade( g_MyInfo.GetPublicID(), iXPos + 14, iYPos + 14, g_MyInfo.GetGradeLevel(), TEAM_PRIVATE_2 );
		g_FontMgr.PrintTextWidthCut( iXPos + 32, iYPos + 11, FONT_SIZE_17, 115.0f, g_MyInfo.GetPublicID().c_str() );
	}
}

void LobbyPlazaInfoWnd::ChangeTab( DWORD dwTabID )
{
	m_dwTabID = dwTabID;
	CheckRadioButton( ID_MISSION_BTN, ID_QUEST_BTN, m_dwTabID );

	HideChildWnd( ID_MISSION_LIST_WND );
	HideChildWnd( ID_QUEST_ALARM_WND );

	switch( m_dwTabID )
	{
	case ID_MISSION_BTN:
		ShowChildWnd( ID_MISSION_LIST_WND );
		break;
	case ID_QUEST_BTN:
		ShowChildWnd( ID_QUEST_ALARM_WND );
		break;
	}

	UpdateRadioBtn();
}

void LobbyPlazaInfoWnd::UpdateRadioBtn()
{
	MissionRadioBtn *MissionBtn = dynamic_cast<MissionRadioBtn*>( FindChildWnd( ID_MISSION_BTN ) );
	if( MissionBtn )
	{
		bool bStartSparkle = false;
		//현재 탭이 퀘스트일 때
		if ( m_dwTabID != ID_MISSION_BTN )
		{
			for ( int i=MDT_DAILY ; i<MDT_MAX ; ++i )
			{
				//보상 받기가 가능한 미션이 있을 경우
				if ( g_MissionManager.GetAttinCountToType( i ) > 0 )
				{
					bStartSparkle = true;
					break;
				}
			}
		}
		//보상 받기가 가능한 미션이 있고
		if ( bStartSparkle )
		{
			//현재 이펙트 상태가 아니라면
			if ( !MissionBtn->IsSparkling() )
				MissionBtn->SetStartSparkle();	//이펙트 스타트
		}
		//보상 받기가 가능한 미션이 없거나 현재 탭이 미션일 경우
		else
		{
			//이펙트 상태이면 End 시켜준다.
			if ( MissionBtn->IsSparkling() )
				MissionBtn->SetEndSparkle();	//이펙트 종료
		}
	}	
}

void LobbyPlazaInfoWnd::SetNamedTitleSparkle()
{
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	NamedTitleBtn *pNamedTitleBtn = dynamic_cast<NamedTitleBtn*>( FindChildWnd( ID_MY_INFO_BTN ) );
	if ( !pNamedTitleBtn )
		return;

	if ( pUserNamedTitle->IsNewNamedTitle() )
	{
		if ( !pNamedTitleBtn->IsEffect() )
			pNamedTitleBtn->SetStartEffect();
	}
	else
	{
		if ( pNamedTitleBtn->IsEffect() )
			pNamedTitleBtn->SetEndEffect();
	}
}
