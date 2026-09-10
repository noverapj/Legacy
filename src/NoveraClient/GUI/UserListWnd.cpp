

#include "StdAfx.h"

#include "../ioGuildMarkMgr.h"
#include "../ioPlayStage.h"
#include "../ioLadderTeamMgr.h"
#include "../ioRankBattleManager.h"

#include "UserListWnd.h"
#include "MessengerWnd.h"
#include "CloverToolTip.h"
#include "GuildWnd.h"

CloverButton::CloverButton()
{
	m_pInActiveNormal = NULL;
	m_pInActiveOvered = NULL;
	m_pInActiveOveredAdd = NULL;
	m_pInActivePushed = NULL;
	m_bActive = true;
}

CloverButton::~CloverButton()
{
	SAFEDELETE( m_pInActiveNormal );
	SAFEDELETE( m_pInActiveOvered );
	SAFEDELETE( m_pInActiveOveredAdd );
	SAFEDELETE( m_pInActivePushed );
}

void CloverButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "InActiveNormal" )
	{
		SAFEDELETE( m_pInActiveNormal );
		m_pInActiveNormal = pImage;
	}
	else if( szType == "InActiveOver" )
	{
		SAFEDELETE( m_pInActiveOvered );
		m_pInActiveOvered = pImage;
	}
	else if( szType == "InActiveOverAdd" )
	{
		SAFEDELETE( m_pInActiveOveredAdd );
		m_pInActiveOveredAdd = pImage;
	}
	else if( szType == "InActivePush" )
	{
		SAFEDELETE( m_pInActivePushed );
		m_pInActivePushed = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void CloverButton::iwm_create()
{
}

void CloverButton::iwm_show()
{
}

void CloverButton::iwm_mouseover( const ioMouse& mouse )
{
	ioButton::iwm_mouseover( mouse );

	UserInfoListWnd *pUserInfoList = dynamic_cast<UserInfoListWnd*>( GetParent() );
	if( pUserInfoList )
	{
		UserListWnd *pUserList = dynamic_cast<UserListWnd*>( pUserInfoList->GetParent() );
		if( pUserList )
		{
			CloverToolTip *pToolTip = dynamic_cast<CloverToolTip*>(pUserList->FindChildWnd( UserListWnd::ID_TOOLTIP ));
			if( pToolTip )
				pToolTip->SetInfo( pUserInfoList->GetUserName(), GetID() );
		}
	}
}

void CloverButton::iwm_mouseleave( const ioMouse& mouse )
{
	ioButton::iwm_mouseleave( mouse );

	UserInfoListWnd *pUserInfoList = dynamic_cast<UserInfoListWnd*>( GetParent() );
	if( pUserInfoList )
	{
		UserListWnd *pUserList = dynamic_cast<UserListWnd*>( pUserInfoList->GetParent() );
		if( pUserList )
		{
			pUserList->HideChildWnd( UserListWnd::ID_TOOLTIP );
		}
	}
}

void CloverButton::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_bActive )
	{
		ioButton::OnDrawNormal( iXPos, iYPos );
	}
	else
	{
		if( m_pInActiveNormal )
			m_pInActiveNormal->Render( iXPos, iYPos );
	}
}

void CloverButton::OnDrawOvered( int iXPos, int iYPos )
{
	if( m_bActive )
	{
		ioButton::OnDrawOvered( iXPos, iYPos );
	}
	else
	{
		if( HasExWndStyle( IWS_EX_OVER_SCREEN ) )
		{
			if( m_pInActiveOvered )
			{
				m_pInActiveOvered->Render( iXPos, iYPos );
				m_pInActiveOvered->Render( iXPos, iYPos, UI_RENDER_SCREEN );
			}
		}
		else
		{
			if( m_pInActiveOvered )
			{
				m_pInActiveOvered->Render( iXPos, iYPos );
			}
		}

		OnDrawOveredAdd( iXPos, iYPos );
	}
}

void CloverButton::OnDrawOveredAdd( int iXPos, int iYPos )
{
	if( m_bActive )
	{
		ioButton::OnDrawOveredAdd( iXPos, iYPos );
	}
	else
	{
		if( m_pInActiveOveredAdd )
			m_pInActiveOveredAdd->Render( iXPos, iYPos,  UI_RENDER_ADD );
	}
}

void CloverButton::OnDrawPushed( int iXPos, int iYPos )
{
	if( m_bActive )
	{
		ioButton::OnDrawPushed( iXPos, iYPos );
	}
	else
	{
		if( m_pInActivePushed )
			m_pInActivePushed->Render( iXPos, iYPos );
	}
}

void CloverButton::SetActive()
{
	m_bActive = true;
}

void CloverButton::SetInActive()
{
	m_bActive = false;
}

//////////////////////////////////////////////////////////////////////////
UserInfoListWnd::UserInfoListWnd()
{
	m_bOnline      = false;
	m_pPlayStage   = NULL;
	m_iListType    = TYPE_FRIENDLIST;
	m_iUserLevel   = 0;
	m_bMyBestFriend = false;
	m_bOtherBestFriend = false;

	m_pBlackMark   = NULL;
	m_pMyBestFriendBlack    = NULL;
	m_pMyBestFriendGray	    = NULL;
	m_pOtherBestFriendBlack = NULL;
	m_pOtherBestFriendGray	= NULL;
	m_pCoupleBestFriendBlack= NULL;
	m_pCoupleBestFriendGray = NULL;
}

UserInfoListWnd::~UserInfoListWnd()
{
	SAFEDELETE( m_pBlackMark );
	SAFEDELETE( m_pMyBestFriendBlack );
	SAFEDELETE( m_pMyBestFriendGray );
	SAFEDELETE( m_pOtherBestFriendBlack );
	SAFEDELETE( m_pOtherBestFriendGray );
	SAFEDELETE( m_pCoupleBestFriendBlack );
	SAFEDELETE( m_pCoupleBestFriendGray );
}

void UserInfoListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlackMark" )
	{
		SAFEDELETE( m_pBlackMark );
		m_pBlackMark = pImage;
	}
	else if( szType == "MyBestFriendBlack" )
	{
		SAFEDELETE( m_pMyBestFriendBlack );
		m_pMyBestFriendBlack = pImage;
	}
	else if( szType == "MyBestFriendGray" )
	{
		SAFEDELETE( m_pMyBestFriendGray );
		m_pMyBestFriendGray = pImage;
	}
	else if( szType == "OtherBestFriendBlack" )
	{
		SAFEDELETE( m_pOtherBestFriendBlack );
		m_pOtherBestFriendBlack = pImage;
	}
	else if( szType == "OtherBestFriendGray" )
	{
		SAFEDELETE( m_pOtherBestFriendGray );
		m_pOtherBestFriendGray = pImage;
	}
	else if( szType == "CoupleBestFriendBlack" )
	{
		SAFEDELETE( m_pCoupleBestFriendBlack );
		m_pCoupleBestFriendBlack = pImage;
	}
	else if( szType == "CoupleBestFriendGray" )
	{
		SAFEDELETE( m_pCoupleBestFriendGray );
		m_pCoupleBestFriendGray = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void UserInfoListWnd::SetFriendUserInfo( ioHashString szUserName, bool bOnline, int iUserPos, int iUserLevel, bool bMyBestFriend, bool bOtherBestFriend, 
										 ioPlayStage *pPlayStage )
{
	m_szUserName	= szUserName;
	m_bOnline		= bOnline;		
	m_iUserPos		= iUserPos;
	m_iUserLevel	= iUserLevel;
	m_bMyBestFriend = bMyBestFriend;
	m_bOtherBestFriend = bOtherBestFriend;
	m_pPlayStage	= pPlayStage;
	m_iListType     = TYPE_FRIENDLIST;

	if( m_szUserName.IsEmpty() )
	{
		SetInActive();
		HideChildWnd( ID_RECV_CLOVER );
		HideChildWnd( ID_SEND_CLOVER );
	}
	else
	{
		SetActive();
		HideChildWnd( ID_RECV_CLOVER );
		ShowChildWnd( ID_SEND_CLOVER );
	}
}

void UserInfoListWnd::SetGuildUserInfo( ioHashString szUserName, bool bOnline, int iUserPos, int iUserLevel, ioPlayStage *pPlayStage )
{
	m_szUserName	= szUserName;
	m_bOnline		= bOnline;		
	m_iUserPos		= iUserPos;
	m_iUserLevel	= iUserLevel;
	m_pPlayStage	= pPlayStage;
	m_iListType     = TYPE_GUILDUSERLIST;

	if( m_szUserName.IsEmpty() )
		SetInActive();
	else
		SetActive();
}

void UserInfoListWnd::SetBlackListUserInfo( ioHashString szUserName, bool bOnline )
{
	m_szUserName = szUserName;
	m_bOnline    = bOnline;	
	m_pPlayStage = NULL;
	m_iListType  = TYPE_BLACKLIST;

	if( m_szUserName.IsEmpty() )
	{
		SetInActive();
	}
	else
	{
		SetActive();
	}
}

void UserInfoListWnd::iwm_create()
{
}

void UserInfoListWnd::iwm_show()
{
	if( m_iListType != TYPE_FRIENDLIST )
	{
		HideChildWnd( ID_RECV_CLOVER );
		HideChildWnd( ID_SEND_CLOVER );
	}
	else
	{
		HideChildWnd( ID_RECV_CLOVER );
		ShowChildWnd( ID_SEND_CLOVER );
	}
}

void UserInfoListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_RECV_CLOVER:
		if( cmd == IOBN_BTNUP )
		{
			MessengerWnd *pWnd = dynamic_cast<MessengerWnd*>( g_GUIMgr.FindWnd(MESSENGER_WND) );
			if( pWnd )
			{
				ioButton *pBtn = dynamic_cast<ioButton*>( FindChildWnd(ID_RECV_CLOVER) );
				if( pBtn )
				{
					int iIndex = UserListWnd::ID_USER_LIST_1 - GetID();
					pWnd->SetMyCloverAni( iIndex, pBtn->GetDerivedPosX(), pBtn->GetDerivedPosY() );
					RecvClover();
				}
			}
		}
		break;
	case ID_SEND_CLOVER:
		if( cmd == IOBN_BTNUP )
		{
			MessengerWnd *pWnd = dynamic_cast<MessengerWnd*>( g_GUIMgr.FindWnd(MESSENGER_WND) );
			if( pWnd )
			{
				CloverButton *pBtn = dynamic_cast<CloverButton*>( FindChildWnd(ID_SEND_CLOVER) );
				if( pBtn )
				{
					if( SendClover() )
					{
						int iIndex = UserListWnd::ID_USER_LIST_1 - GetID();
						pWnd->SetGiftCloverAni( iIndex, pBtn->GetDerivedPosX(), pBtn->GetDerivedPosY() );
					}
				}
			}
		}
		break;
	}
}

float UserInfoListWnd::GetNameCut()
{
	return 140.0f;
}

void UserInfoListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szUserName.IsEmpty() ) return;

	if( m_iListType != TYPE_FRIENDLIST )
	{
		HideChildWnd( ID_RECV_CLOVER );
		HideChildWnd( ID_SEND_CLOVER );
		return;
	}

	ioClover *pClover = g_MyInfo.GetClover();
	if( pClover )
	{
		// 클로버 받기
		if( pClover->EnableRecvClover( m_szUserName ) )
			ShowChildWnd( ID_RECV_CLOVER );
		else
			HideChildWnd( ID_RECV_CLOVER );

		// 클로버 보내기
		ioButton *pBtn = dynamic_cast<ioButton*>( FindChildWnd(ID_SEND_CLOVER) );
		if( pBtn )
		{
			if( pClover->EnableSendClover( m_szUserName ) )
				pBtn->SetActive();
			else
				pBtn->SetInActive();
		}
	}
}

void UserInfoListWnd::OnRender()
{
	ioButton::OnRender();	

	if( IsDisabled() )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_iListType == TYPE_FRIENDLIST || m_iListType == TYPE_GUILDUSERLIST )
	{
		if( m_bOver || m_bClicked )
		{
			if( m_bOnline || m_iListType == TYPE_GUILDUSERLIST )
				g_LevelMgr.RenderGrade( m_szUserName, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );			
			else
				g_LevelMgr.RenderMessengerOfflineMark( iXPos + 4, iYPos + 2, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			if( m_bOnline ) 
			{
				g_LevelMgr.RenderGrade( m_szUserName, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			}
			else if( m_iListType == TYPE_GUILDUSERLIST )
			{
				g_LevelMgr.RenderGrade( m_szUserName, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			}
			else
			{
				g_LevelMgr.RenderMessengerOfflineMark( iXPos + 4, iYPos + 2 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			}
		}	

		float iNameCut = GetNameCut();
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, iNameCut, m_szUserName.c_str() );
		
		int iRealNameAfterPos = g_FontMgr.GetTextWidthCutSize( m_szUserName.c_str(), TS_NORMAL, FONT_SIZE_12, iNameCut );
		if( m_bOnline )
		{
			DWORD dwGuildIndex, dwGuildMark;
			g_UserInfoMgr.GetGuildInfo( m_szUserName, dwGuildIndex, dwGuildMark );			
			g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 23 + iRealNameAfterPos + 3, iYPos + 4 );
			if( dwGuildIndex != 0 )
			{	
				iRealNameAfterPos += ( g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 3 );
			}
		}

		int iFriendMakAfterPos = 0;
		if( m_iListType == TYPE_FRIENDLIST )
		{
			// 절친 마크
			if( m_bMyBestFriend && m_bOtherBestFriend )
			{
				if( m_bOnline && m_pCoupleBestFriendBlack )
				{
					m_pCoupleBestFriendBlack->Render( iXPos + 23 + iRealNameAfterPos + 3, iYPos + 3 );
					iFriendMakAfterPos += m_pCoupleBestFriendBlack->GetWidth() + iRealNameAfterPos + 26;
				}
				else if( m_pMyBestFriendGray )
				{
					m_pMyBestFriendGray->Render( iXPos + 23 + iRealNameAfterPos + 3, iYPos + 3 );
					iFriendMakAfterPos += m_pMyBestFriendGray->GetWidth() + iRealNameAfterPos + 26;
				}
			}
			else if( m_bMyBestFriend )
			{
				if( m_bOnline && m_pMyBestFriendBlack )
				{
					m_pMyBestFriendBlack->Render( iXPos + 23 + iRealNameAfterPos + 3, iYPos + 3 );
					iFriendMakAfterPos += m_pMyBestFriendBlack->GetWidth() + iRealNameAfterPos + 26;
				}
				else if( m_pMyBestFriendGray )
				{
					m_pMyBestFriendGray->Render( iXPos + 23 + iRealNameAfterPos + 3, iYPos + 3 );
					iFriendMakAfterPos += m_pMyBestFriendGray->GetWidth() + iRealNameAfterPos + 26;
				}
			}
			else if( m_bOtherBestFriend )
			{	
				if( m_bOnline && m_pOtherBestFriendBlack )
				{
					m_pOtherBestFriendBlack->Render( iXPos + 23 + iRealNameAfterPos + 3, iYPos + 3 );
					iFriendMakAfterPos += m_pOtherBestFriendBlack->GetWidth() + iRealNameAfterPos + 26;
				}
			}

			ioHashString szPosition;
			szPosition.Clear();
			switch( m_iUserPos )
			{
			case UP_TRAINING:
				szPosition = "광장";
				break;
			case UP_BATTLE_ROOM:
				szPosition = "전투";
				break;
			case UP_LADDER_TEAM:
				szPosition = "진영전";
				break;
			case UP_LOBBY:
				szPosition = "로비";
				break;
			case UP_HEADQUARTERS:
				szPosition = "훈련소";
				break;
			case UP_TOURNAMENT:
				szPosition = "대회";
				break;
			case UP_SHUFFLE:
				szPosition = "오늘의모드";
				break;
			case UP_GUILD_HQ:
				szPosition = "길드본부";
				break;
			case UP_HOUSE:
				szPosition = "개인본부";
				break;
			}

			if( !szPosition.IsEmpty() && m_bOnline )
			{
				int iXOffset = 0;
				if( iFriendMakAfterPos > 0 )
					iXOffset = iFriendMakAfterPos + 3;
				else
					iXOffset = iRealNameAfterPos + 26 + 3;

				if( g_BattleRoomMgr.IsBattleRoom() || g_LadderTeamMgr.IsLadderTeam() )
				{
					if( m_bOver || m_bClicked )
						g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
					else if( CheckInvite( false ) )
						g_FontMgr.SetTextColor( 206, 78, 0 );
					else
						g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
				}
				else
				{
					if( m_bOver || m_bClicked )
						g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
					else if( CheckMove( false ) )
						g_FontMgr.SetTextColor( 206, 78, 0 );
					else
						g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
				}

				g_FontMgr.PrintText( iXPos + iXOffset, iYPos + 3, FONT_SIZE_12, "[%s]", szPosition.c_str() );
			}
		}
	}
	else if( m_iListType == TYPE_BLACKLIST )
	{
		if( m_bOver || m_bClicked )
		{
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}		

		if( m_pBlackMark )
			m_pBlackMark->Render( iXPos + 4, iYPos + 2 );
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 170.0f, m_szUserName.c_str() );
	}
}


bool UserInfoListWnd::CheckInvite( bool bAlarm )
{
	if( m_iUserPos == UP_BATTLE_ROOM || m_iUserPos == UP_LADDER_TEAM || m_iUserPos == UP_SHUFFLE || m_iUserPos == UP_TOURNAMENT )
	{
		if( bAlarm )
		{
			if( m_iUserPos == UP_BATTLE_ROOM )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s님은 전투 중이라#초대가 불가능합니다.", m_szUserName.c_str() );
			else if( m_iUserPos == UP_LADDER_TEAM )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s님은 진영전 중이라#초대가 불가능합니다.", m_szUserName.c_str() );
			else if( m_iUserPos == UP_SHUFFLE )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s님은 오늘의모드 중이라#초대가 불가능합니다.", m_szUserName.c_str() );
			else if( m_iUserPos == UP_TOURNAMENT )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s님은 대회 중이라#초대가 불가능합니다.", m_szUserName.c_str() );
		}
		return false;
	}
	else if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.IsStartRoomEnterX() && g_BattleRoomMgr.IsBattleModePlaying() )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "시작 후 참여 X가 설정된 #전투방은 초대가 불가능합니다." );

			return false;
		}
		else if( g_BattleRoomMgr.GetCurPlayer() >= g_BattleRoomMgr.GetMaxPlayer() )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투룸 정원을 초과하였습니다." );

			return false;
		}
		else if( g_BattleRoomMgr.IsHaveUser( m_szUserName ) )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "동일한 전투룸에 있는 유저입니다." );

			return false;
		}
		else if( g_BattleRoomMgr.IsSafetyLevelRoom() )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "초보 팀데스매치는 초대가 불가능합니다." );

			return false;
		}
		else if( g_BattleRoomMgr.IsBroadcastModeType() && g_MyInfo.GetUserEventType() != USER_TYPE_BROADCAST_MBC )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "경기 전용 방에서는 방장만 초대가 가능합니다." );

			return false;
		}
		else if( g_BattleRoomMgr.IsTournamentModeType() )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회방에는 초대가 되지 않습니다." );

			return false;
		}
		else if( g_RankBattleMgr.IsRankBattlePlaying() )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "랭킹전 중에는 초대가 되지 않습니다." );

			return false;
		}
			
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		if( g_LadderTeamMgr.IsLadderTeamFull() )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영팀 정원을 초과하였습니다." );

			return false;
		}		
		else if( g_LadderTeamMgr.IsLadderBattleModePlaying() )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영전 중에는 초대가 불가능합니다." );

			return false;
		}
		else if( g_LadderTeamMgr.IsSearching() && !g_LadderTeamMgr.IsLadderBattleModePlaying() )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영전 검색 중에는 초대가 불가능합니다." );

			return false;
		}
		else if( g_UserInfoMgr.GetCampPosition( m_szUserName ) != g_MyInfo.GetUserCampPos() )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영이 다른 유저는 초대가 불가능합니다." );

			return false;
		}
		else if( g_LadderTeamMgr.IsHaveUser( m_szUserName ) )
		{
			if( bAlarm )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "동일한 진영팀에 있는 유저입니다." );

			return false;
		}
	}	

	return true;
}

bool UserInfoListWnd::CheckMove( bool bAlarm )
{
	if( m_iUserPos == UP_LOBBY )
	{
		if( bAlarm )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "로비로 따라가기는 불가능합니다." );
		}
		return false;
	}
	else if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( bAlarm )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투방에 소속되어있으면#사용할 수 없습니다." );
		}
		return false;
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		if( bAlarm )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영팀에 소속되어있으면#사용할 수 없습니다." );
		}
		return false;
	}
	return true;
}

bool UserInfoListWnd::SendClover()
{
	FriendNode *pNode = g_FriendManager.GetNode( m_szUserName );
	if( pNode )
	{
		if( !g_TCPSocketMgr.IsTestZone() )
		{
			if( g_App.IsMeDeveloper() || g_App.IsMeGameMaster() )
				return false;
		}

		char szBuf[MAX_PATH] = "";
		ioClover *pClover = g_MyInfo.GetClover();
		if( !pClover )
			return false;

		if( pClover->GetCurGiftCloverCount() <= 0 )
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "선물용 클로버가 부족합니다." );
			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "선물용 클로버는 " );
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "%d분", pClover->GetChargingTime() );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "에 %d개씩 충전됩니다.", pClover->GetChargingCount() );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		else if( pClover->CheckFriendRegTime( m_szUserName ) )
		{
			ioHashString szRemainTime;
			pClover->GetFriendRegRemainTime( m_szUserName, szRemainTime );

			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "친구가 된지 " );
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			if( pClover->GetAbusingTime() <= 60 )
				kPrinter[0].AddTextPiece( FONT_SIZE_12, "%d분", pClover->GetAbusingTime() );
			else
				kPrinter[0].AddTextPiece( FONT_SIZE_12, "%d시간", pClover->GetAbusingTime()/60 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, " 후 부터" );
			
			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "클로버를 주고 받을 수 있습니다." );
			
			kPrinter[2].SetTextStyle( TS_NORMAL );
			kPrinter[2].SetBkColor( 0, 0, 0 );
			kPrinter[2].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[2].AddTextPiece( FONT_SIZE_13, "%s", szRemainTime.c_str() );
			kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[2].AddTextPiece( FONT_SIZE_13, " 후 부터 보낼 수 있습니다." );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		else if( pClover->CheckSendRemainTime( m_szUserName ) )
		{
			ioHashString szRemainTime;
			pClover->GetSendRemainTime( m_szUserName, szRemainTime );

			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "최근에 클로버를 선물하였습니다." );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s", szRemainTime.c_str() );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, " 후 다시 보낼 수 있습니다." );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		else
		{
			// 임시로 클라이언트의 값을 셋팅해준다. ( 서버로부터 응답을 받으면 서버 기준 값으로 변경된다. )
			int iGiftClover = pClover->GetCurGiftCloverCount();
			iGiftClover--;
			iGiftClover = max( iGiftClover, 0 );
			pClover->SetCurGiftCloverCount( iGiftClover );

			pNode->SetCloverSendSec( pClover->GetSendTimeGap() * 60 );

			// Send Packet..
			SP2Packet kPacket( CTPK_GIFTCLOVER_SEND );
			kPacket << pNode->GetUserIndex();
			TCPNetwork::SendToServer( kPacket );

			TCPNetwork::MouseBusy( true );
			return true;
		}
	}

	return false;
}

void UserInfoListWnd::RecvClover()
{
	FriendNode *pNode = g_FriendManager.GetNode( m_szUserName );
	if( pNode )
	{
		if( !g_TCPSocketMgr.IsTestZone() )
		{
			if( g_App.IsMeDeveloper() || g_App.IsMeGameMaster() )
				return;
		}

		ioClover *pClover = g_MyInfo.GetClover();
		// 임시로 클라이언트의 값을 셋팅해준다. ( 서버로부터 응답을 받으면 서버 기준 값으로 변경된다. )
		if( pClover )
		{
			pNode->SetCloverRecvSec( pClover->GetRecvTimeGap() * 60 );
		}

		// receive Packet..
		SP2Packet kPacket( CTPK_CLOVER_RECEIVE );
		kPacket << pNode->GetUserIndex();
		TCPNetwork::SendToServer( kPacket );

		TCPNetwork::MouseBusy( true );
	}
}
//////////////////////////////////////////////////////////////////////////
bool UserListWnd::sg_bUserListUpdate = false;
UserListWnd::UserListWnd()
{
	m_iTabID   = ID_FRIEND_LIST;
	m_iCurPage = m_iMaxPage = 0;	
	m_dwChangePageTimer = 0;
	m_dwUserPosRefreshTimer = 0;
	m_pPlayStage = NULL;
}

UserListWnd::~UserListWnd()
{
}

void UserListWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/clovertooltip.xml", this );
}

void UserListWnd::iwm_show()
{	
	m_dwChangePageTimer     = FRAMEGETTIME();
	m_dwUserPosRefreshTimer = 0;
	CheckButton( ID_FRIEND_LIST );	

	ioWnd *pFriendBtn = FindChildWnd( ID_FRIEND_LIST );
	if( pFriendBtn )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), g_FriendManager.GetOnlineNodeSize(), g_FriendManager.GetNodeSize() );
		pFriendBtn->SetTitleText( szTitle );
	}

	ioWnd *pGuildBtn = FindChildWnd( ID_GUILD_LIST );
	if( pGuildBtn )
	{
		int iOnlineUserCnt = 0;
		int iMaxGuildUserCnt= 0;
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( pGuildData )
		{
			pGuildBtn->SetActive();
			iMaxGuildUserCnt = (int)pGuildData->GetGuildJoinUser();
			for(int i = 0;i < iMaxGuildUserCnt;i++)
			{
				UserListInfo kMUI;
				pGuildData->GetUserInfo( i, kMUI.m_szUserID, kMUI.m_iGradeLevel, kMUI.m_iUserPos, kMUI.m_bOnline );
				if( !kMUI.m_szUserID.IsEmpty() && g_MyInfo.GetPublicID() != kMUI.m_szUserID )
				{
					if( kMUI.m_bOnline )
						iOnlineUserCnt++;
				}						
			}
		}
		else
		{
			pGuildBtn->SetInActive();
		}
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2), iOnlineUserCnt, max( 0, iMaxGuildUserCnt - 1 ) );
		pGuildBtn->SetTitleText( szTitle );
	}
}

void UserListWnd::iwm_hide()
{	
}

void UserListWnd::UpdateUserList()
{
	m_UserList.clear();
	switch( m_iTabID )
	{
	case ID_FRIEND_LIST:
		{
			// 친구 목록
			int iOnlineUserCnt = 0;
			for(int i = 0;i < g_FriendManager.GetNodeSize();i++)
			{
				FriendNode *pNode = g_FriendManager.GetNode( i );
				if( pNode )
				{
					UserListInfo kMUI;
					kMUI.m_szUserID	   = pNode->GetName();
					kMUI.m_iGradeLevel = pNode->GetGradeLevel();
					kMUI.m_iUserPos    = pNode->GetUserPos();
					kMUI.m_bOnline     = pNode->IsOnline();
					kMUI.m_bOtherBestFriend = pNode->IsOtherBF();
					kMUI.m_iRecvWaitClover = pNode->GetCloverRecvWaitCount();
					kMUI.m_iRecvClover     = pNode->GetCloverRecvCount();
					kMUI.m_iSendClover     = pNode->GetCloverSendCount();
					if( g_FriendManager.GetBestFriendState( kMUI.m_szUserID ) == BFT_SET )
					{   
						kMUI.m_bMyBestFriend = true;
					}
					m_UserList.push_back( kMUI );

					if( kMUI.m_bOnline )
						iOnlineUserCnt++;
				}
			}			
			std::sort( m_UserList.begin(), m_UserList.end(), UserListInfoSort() );	
			m_iMaxPage = max( 0, (int)m_UserList.size() - 1 ) / MAX_LIST_SIZE;
			UpdateSetPage( m_iCurPage );

			ioWnd *pFriendBtn = FindChildWnd( ID_FRIEND_LIST );
			if( pFriendBtn )
			{
				char szTitle[MAX_PATH] = "";
				SafeSprintf( szTitle, sizeof( szTitle ), STR(1), iOnlineUserCnt, (int)m_UserList.size() );
				pFriendBtn->SetTitleText( szTitle );
			}
		}
		break;
	case ID_GUILD_LIST:
		{
			// 길드원 목록
			int iOnlineUserCnt = 0;
			if( g_GuildInfo.IsGuild() )
			{
				ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
				if( pGuildData )
				{
					for(int i = 0;i < (int)pGuildData->GetGuildJoinUser();i++)
					{
						UserListInfo kMUI;
						pGuildData->GetUserInfo( i, kMUI.m_szUserID, kMUI.m_iGradeLevel, kMUI.m_iUserPos, kMUI.m_bOnline );
						if( !kMUI.m_szUserID.IsEmpty() && g_MyInfo.GetPublicID() != kMUI.m_szUserID )
						{
							m_UserList.push_back( kMUI );
							if( kMUI.m_bOnline )
								iOnlineUserCnt++;
						}						
					}
				}
				std::sort( m_UserList.begin(), m_UserList.end(), UserListInfoSort() );	
			}
			m_iMaxPage = max( 0, (int)m_UserList.size() - 1 ) / MAX_LIST_SIZE;
			UpdateSetPage( m_iCurPage );

			ioWnd *pGuildBtn = FindChildWnd( ID_GUILD_LIST );
			if( pGuildBtn )
			{
				char szTitle[MAX_PATH] = "";
				SafeSprintf( szTitle, sizeof( szTitle ), STR(2), iOnlineUserCnt, (int)m_UserList.size() );
				pGuildBtn->SetTitleText( szTitle );
			}
		}
		break;
	case ID_BLACK_LIST:
		{
			for(int i = 0;i < g_BlackListManager.GetNodeSize();i++)
			{		
				ioHashString *pString = g_BlackListManager.GetNode( i );
				if( pString )
				{
					UserListInfo kMUI;
					kMUI.m_szUserID = *pString;
					m_UserList.push_back( kMUI );
				}
			}
			m_iMaxPage = max( 0, (int)m_UserList.size() - 1 ) / MAX_LIST_SIZE;
			UpdateSetPage( m_iCurPage );
		}
		break;
	}
}

void UserListWnd::UpdateSetPage( int iNewPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = iNewPage;
	switch( m_iTabID )
	{
	case ID_FRIEND_LIST:
		{
			int iStartArray = m_iCurPage * MAX_LIST_SIZE;
			for(int i = ID_USER_LIST_1;i < ID_USER_LIST_9+1;i++,iStartArray++)
			{
				UserInfoListWnd *pWnd = dynamic_cast<UserInfoListWnd*>( FindChildWnd( i ) );
				if( pWnd )
				{
					if( iStartArray < (int)m_UserList.size() )
					{
						UserListInfo &kMUI = m_UserList[iStartArray];
						pWnd->ShowWnd();
						pWnd->SetFriendUserInfo( kMUI.m_szUserID, kMUI.m_bOnline, kMUI.m_iUserPos, kMUI.m_iGradeLevel,
												 kMUI.m_bMyBestFriend, kMUI.m_bOtherBestFriend, m_pPlayStage );
					}
					else
						pWnd->HideWnd();
				}
			}			
		}
		break;
	case ID_GUILD_LIST:
		{
			int iStartArray = m_iCurPage * MAX_LIST_SIZE;
			for(int i = ID_USER_LIST_1;i < ID_USER_LIST_9+1;i++,iStartArray++)
			{
				UserInfoListWnd *pWnd = dynamic_cast<UserInfoListWnd*>( FindChildWnd( i ) );
				if( pWnd )
				{
					if( iStartArray < (int)m_UserList.size() )
					{
						UserListInfo &kMUI = m_UserList[iStartArray];
						pWnd->ShowWnd();
						pWnd->SetGuildUserInfo( kMUI.m_szUserID, kMUI.m_bOnline, kMUI.m_iUserPos, kMUI.m_iGradeLevel, m_pPlayStage );
					}
					else
						pWnd->HideWnd();
				}
			}			
		}
		break;
	case ID_BLACK_LIST:
		{
			int iStartArray = m_iCurPage * MAX_LIST_SIZE;
			for(int i = ID_USER_LIST_1;i < ID_USER_LIST_9+1;i++,iStartArray++)
			{
				UserInfoListWnd *pWnd = dynamic_cast<UserInfoListWnd*>( FindChildWnd( i ) );
				if( pWnd )
				{
					if( iStartArray < (int)m_UserList.size() )
					{
						UserListInfo &kMUI = m_UserList[iStartArray];
						pWnd->ShowWnd();
						pWnd->SetBlackListUserInfo( kMUI.m_szUserID, true ); // black list 무조건 online
					}
					else
						pWnd->HideWnd();
				}
			}
		}
		break;
	}

	if( iPrevPage != m_iCurPage )
	{
		m_dwChangePageTimer = FRAMEGETTIME();
		m_dwUserPosRefreshTimer = 0;
	}
}

void UserListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_FRIEND_LIST:
	case ID_GUILD_LIST:
	case ID_BLACK_LIST:	
		if( cmd == IOBN_BTNDOWN )
		{
			CheckButton( dwID );
		}
		break;
	case ID_FRIEND_INSERT:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.ShowWnd( FRIEND_APPLICATION_WND );
		}
		break;
	case ID_GUILD_INFO:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GuildInfo.GetGuildIndex() != 0 )
			{
				GuildMainWnd *pGuildMainWnd = dynamic_cast<GuildMainWnd *>(g_GUIMgr.FindWnd( GUILD_MAIN_WND ));
				if( pGuildMainWnd )
					pGuildMainWnd->SetGuildIndex( g_GuildInfo.GetGuildIndex() );
			}
		}
		break;
	case ID_GUILD_HOME_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GuildInfo.GetGuildIndex() != 0 )
			{
				g_App.OpenGuildHomePage( g_GuildInfo.GetGuildIndex() );
			}
		}
		break;
	case ID_BLACK_INSERT:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.ShowWnd( BLACKLIST_INSERT_WND );
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
	case ID_USER_LIST_1:
	case ID_USER_LIST_2:
	case ID_USER_LIST_3:
	case ID_USER_LIST_4:
	case ID_USER_LIST_5:
	case ID_USER_LIST_6:
	case ID_USER_LIST_7:
	case ID_USER_LIST_8:
	case ID_USER_LIST_9:
		if( cmd == IOBN_BTNUP )
		{
			if( cmd == IOBN_BTNUP )
			{
				UserInfoListWnd *pUserInfoWnd = dynamic_cast<UserInfoListWnd*>(pWnd);
				if( pUserInfoWnd )
				{
					UserInfoListOptionWnd *pOptionWnd = dynamic_cast<UserInfoListOptionWnd*>( FindChildWnd( ID_OPTION_WND ) );
					if( pOptionWnd )
					{
						pOptionWnd->SetOptionInfo( m_pPlayStage, pUserInfoWnd->GetUserID(), pUserInfoWnd->GetUserLevel(), pUserInfoWnd->IsBestFriend(), pUserInfoWnd->GetUserPosition(), pUserInfoWnd->GetListType(), pUserInfoWnd->IsOnline(), pUserInfoWnd->CheckMove( false ), pUserInfoWnd->CheckInvite( false ) );
					}
				}
			}
		}
		break;
	}
}

bool UserListWnd::iwm_esc()
{
	if( !IsShow() )
		return false;

	UserInfoListOptionWnd *pOptionWnd = dynamic_cast<UserInfoListOptionWnd *>( FindChildWnd( ID_OPTION_WND ) );
	if( pOptionWnd && pOptionWnd->IsShow() )
	{
		return pOptionWnd->iwm_esc();
	}
	return false;
}

void UserListWnd::CheckButton( DWORD dwID )
{
	m_iTabID = dwID;
	CheckRadioButton( ID_FRIEND_LIST, ID_BLACK_LIST, m_iTabID );

	HideChildWnd( ID_FRIEND_INSERT );
	HideChildWnd( ID_BESTFRIEND_INSERT );
	HideChildWnd( ID_GUILD_INFO );
	HideChildWnd( ID_GUILD_HOME_PAGE );
	HideChildWnd( ID_BLACK_INSERT );

	m_iCurPage = 0;
	switch( m_iTabID )
	{
	case ID_FRIEND_LIST:
		UpdateUserList();
		ShowChildWnd( ID_FRIEND_INSERT );
		ShowChildWnd( ID_BESTFRIEND_INSERT );
		break;
	case ID_GUILD_LIST:
		UpdateUserList();
		ShowChildWnd( ID_GUILD_INFO );
		ShowChildWnd( ID_GUILD_HOME_PAGE );
		break;
	case ID_BLACK_LIST:
		UpdateUserList();
		ShowChildWnd( ID_BLACK_INSERT );
		break;
	}
}

void UserListWnd::InviteResult( int iResult, ioHashString szInvitedName )
{
	switch( iResult )
	{
	case CHANNEL_INVITE_NOT_USER:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case CHANNEL_INVITE_USER_NOT_CHANNEL:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case CHANNEL_INVITE_USER_ALREADY_CHANNEL:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		break;
	}
}

void UserListWnd::ChannelCreate( int iResult, ioHashString szManToManName )
{
	g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
}

void UserListWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

bool UserListWnd::IsChatBlock()
{
	if( !IsShow() )
		return false;
	return false;
}

void UserListWnd::SendUserPosRefresh()
{
	if( m_iTabID != ID_FRIEND_LIST && m_iTabID != ID_GUILD_LIST ) return;
    if( !g_FriendManager.IsLoadingComplete() ) return;

	int i = 0;
	ioHashStringVec vString;
	for(i = ID_USER_LIST_1;i < ID_USER_LIST_9+1;i++)
	{
		UserInfoListWnd *pUserListWnd = dynamic_cast<UserInfoListWnd*>( FindChildWnd( i ) );
		if( !pUserListWnd ) continue;
		if( !pUserListWnd->IsShow() ) continue;
		if( !pUserListWnd->IsOnline() ) continue;
		if( pUserListWnd->GetUserName().IsEmpty() ) continue;

		vString.push_back( pUserListWnd->GetUserName() );
	}

	int iUserSize = vString.size();
	if( iUserSize != 0 )
	{
		SP2Packet kPacket( CTPK_USER_POS_REFRESH );
		kPacket << iUserSize;
		for(i = 0;i < iUserSize;i++)
			kPacket << vString[i];
		TCPNetwork::SendToServer( kPacket );
		vString.clear();
	}					
}

void UserListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	switch( m_iTabID )
	{
	case ID_FRIEND_LIST:
	case ID_GUILD_LIST:
		if( sg_bUserListUpdate )
		{
			sg_bUserListUpdate = false;
			UpdateUserList();					
		}
		else if( m_dwChangePageTimer != 0 )
		{
			if( FRAMEGETTIME() - m_dwChangePageTimer > 1000 )
			{
				SendUserPosRefresh();
				m_dwChangePageTimer = 0;
				m_dwUserPosRefreshTimer = FRAMEGETTIME();
			}			
		}
		else if( m_dwUserPosRefreshTimer != 0 )
		{
			if( FRAMEGETTIME() - m_dwUserPosRefreshTimer > 15000 )
			{
				SendUserPosRefresh();
				m_dwUserPosRefreshTimer = FRAMEGETTIME();
			}
		}
		break;
	case ID_BLACK_LIST:
		if( sg_bUserListUpdate )
		{
			sg_bUserListUpdate = false;
			UpdateUserList();
		}
		break;
	}
}

void UserListWnd::OnRender()
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
	kPrinter.PrintFullText( iXPos + 161, iYPos + 226, TAT_CENTER );	
}

void UserListWnd::OnInsertBestFriend( const ioHashString & szUserName )
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "친구 목록을 로드중입니다.#잠시 후 다시 시도해주세요." );
		return;
	}

	if( szUserName.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "절친신청을 받을 친구를 선택하세요." );
		return;
	}

	FriendNode *pFriend = g_FriendManager.GetNode( szUserName );
	if( pFriend == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "절친 설정은 친구여야 가능합니다." );
		return;
	}

	if( g_FriendManager.GetBestFriendState( pFriend->GetUserIndex() ) == BFT_SET )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s 님은절친설정 되어있습니다.", pFriend->GetName().c_str() );
		return;
	}

 	SP2Packet kPacket( CTPK_INSERT_BESTFRIEND );
 	kPacket << pFriend->GetUserIndex();
 	TCPNetwork::SendToServer( kPacket );
 	TCPNetwork::MouseBusy( true );
}

bool UserListWnd::CheckDeleteBestFriend( const ioHashString & szUserName )
{
	if( g_App.IsMouseBusy() )
		return false;

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "친구 목록을 로드중입니다.#잠시 후 다시 시도해주세요." );
		return false;
	}

	FriendNode *pFriendNode = g_FriendManager.GetNode( szUserName );
	if( pFriendNode == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s님은 친구가 아닙니다.", szUserName.c_str() );
		return false;
	}

	if( g_FriendManager.GetBestFriendState( pFriendNode->GetUserIndex() ) != BFT_SET )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s님은 절친이 아닙니다.", pFriendNode->GetName().c_str() );
		return false;
	}

	return true;
}

void UserListWnd::_OnDeleteBestFriend( const ioHashString & szUserName )
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "친구 목록을 로드중입니다.#잠시 후 다시 시도해주세요." );
		return;
	}

	FriendNode *pFriendNode = g_FriendManager.GetNode( szUserName );
	if( pFriendNode == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s님은 친구가 아닙니다.", szUserName.c_str() );
		return;
	}

	if( g_FriendManager.GetBestFriendState( pFriendNode->GetUserIndex() ) != BFT_SET )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s님은 절친이 아닙니다.", pFriendNode->GetName().c_str() );
		return;
	}

	SP2Packet kPacket( CTPK_DISMISS_BESTFRIEND );
	kPacket << pFriendNode->GetUserIndex() << pFriendNode->GetName();
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}
//////////////////////////////////////////////////////////////////////////
UserInfoListOptionWnd::UserInfoListOptionWnd()
{
	m_pPlayStage = NULL;
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

	m_dwMsgCmd = 0;
}

UserInfoListOptionWnd::~UserInfoListOptionWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void UserInfoListOptionWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
}

void UserInfoListOptionWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void UserInfoListOptionWnd::CommandOption( DWORD dwCmd )
{
	switch( dwCmd )
	{
	case OPTION_INFO:
		{
			g_App.OpenMannerWnd( m_szUserID.c_str(), m_bOnLine );
		}
		break;
	case OPTION_ADD_BEST_FRIEND:
		{
			UserListWnd *pWnd = dynamic_cast<UserListWnd*>(GetParent());
			if( pWnd )
				pWnd->OnInsertBestFriend( m_szUserID );
		}
		break;
	case OPTION_RELEASE_BEST_FRIEND:
		{
			UserListWnd *pWnd = dynamic_cast<UserListWnd*>(GetParent());
			if( pWnd && pWnd->CheckDeleteBestFriend( m_szUserID ) )
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "절친을 해제하시겠습니까?#절친을 해제하면 절친해제 완료전까지#%s님에게 재설정을 할 수 없습니다.", m_szUserID.c_str() );
				m_dwMsgCmd = OPTION_RELEASE_BEST_FRIEND;
			}
		}
		break;
	case OPTION_BLACK_DELETE:
		{
			g_BlackListManager.RemoveBlackList( m_szUserID );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "목록에서 삭제되었습니다." );
		}
		break;
	case OPTION_FOLLOW:
		if( m_pPlayStage )
		{
			if( m_pPlayStage->GetModeType() != MT_MYROOM && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "개인본부/길드본부/훈련소/광장에서만#사용 가능합니다." );
			}	
			else if( g_MyInfo.GetActiveCharCount() == 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "용병을 먼저 고용해주세요." );
			}
			else if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "체험용병이 아닌 일반용병을 고용해주세요." );
			}
			else if( g_MyInfo.IsTutorialUser() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "튜토리얼을 먼저 완료해주세요." );
			}
			else if( !m_szUserID.IsEmpty() )
			{
				if( m_bEnableMove )
				{
					switch( m_iUserPos )
					{
					case UP_TRAINING:
					case UP_BATTLE_ROOM:  
					case UP_LADDER_TEAM:
					case UP_HEADQUARTERS:
					case UP_TOURNAMENT:
					case UP_SHUFFLE:
					case UP_GUILD_HQ:
					case UP_HOUSE:
						{
							SP2Packet kPacket( CTPK_USER_POS_INDEX );
							kPacket << m_szUserID << m_iUserPos;
							TCPNetwork::SendToServer( kPacket );
						}
						break;
					default:
						{
							TCPNetwork::MouseBusy( true );
							SP2Packet kPacket( CTPK_FOLLOW_USER );
							kPacket << m_szUserID << m_iUserPos;
							TCPNetwork::SendToServer( kPacket );
						}
						break;
					}							
				}
			}
		}
		break;
	case OPTION_INVITE:
		{
			if( m_bEnableInvite )
			{
				// 초대 작렬
				if( g_LadderTeamMgr.IsLadderTeam() )
				{
					//초대
					if( m_iUserLevel < g_LadderTeamMgr.GetLadderTeamLimitGrade() )
					{
						char szGradeName[MAX_PATH] = "";
						g_LevelMgr.GetGradeName( g_LadderTeamMgr.GetLadderTeamLimitGrade(), szGradeName, sizeof( szGradeName ), false );
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영전투 초대는 %s이상 유저만 가능합니다.", szGradeName );
					}
					else
					{
						SP2Packet kPacket( CTPK_LADDERTEAM_INVITE );
						kPacket << 1 << m_szUserID;
						TCPNetwork::SendToServer( kPacket );
					}
				}
				else if( g_BattleRoomMgr.IsBattleRoom() )
				{
					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_BATTLEROOM_INVITE );
					kPacket << 1 << m_szUserID << -1 << 0 << 0;
					TCPNetwork::SendToServer( kPacket );	
				}
			}
		}
		break;
	}
}

void UserInfoListOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param != IDYES )
				return;

			switch( m_dwMsgCmd )
			{
			case OPTION_RELEASE_BEST_FRIEND:
				{
					UserListWnd *pWnd = dynamic_cast<UserListWnd*>(GetParent());
					if( pWnd )
					{
						pWnd->_OnDeleteBestFriend( m_szUserID );
					}
				}
				break;
			}
		}
		break;
	}	
}

void UserInfoListOptionWnd::SetOptionInfo( ioPlayStage *pStage, const ioHashString &szUserID, int iUserLevel, bool bBestFriend, int iUserPos, int iListType, bool bOnLine, bool bEnableMove, bool bEnableInvite )
{
	HideWnd();
	/*	
	-친구 탭
	-길드 탭
	[누구누구 정보]
	[위치 따라가기] / [초대]
	[절친으로 설정] & [절친 해제]
	
	-차단 탭
	[누구누구 정보]
	[차단 해제]

	-옵션 명령이 하나밖에 없으면 즉시 실행한다.
	-사용할 수 없는 옵션은 보여주지 않는다.
	*/
	m_OptionMap.clear();

	m_pPlayStage    = pStage;
	m_szUserID      = szUserID;
	m_iUserLevel    = iUserLevel;
	m_iUserPos      = iUserPos;
	m_bBestFriend   = bBestFriend;
	m_iListType     = iListType;
	m_bOnLine       = bOnLine;
	m_bEnableMove   = bEnableMove;
	m_bEnableInvite = bEnableInvite;

	// 정보
	if( !m_szUserID.IsEmpty() )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), "%s 정보", m_szUserID.c_str() );
		m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, szTitle ) );
	}

	if( m_iListType == UserInfoListWnd::TYPE_FRIENDLIST||
		m_iListType == UserInfoListWnd::TYPE_GUILDUSERLIST )
	{
		char szTitle[MAX_PATH] = "";
		if( m_bOnLine )
		{
			if( m_bEnableInvite )
			{
				if( g_BattleRoomMgr.IsBattleRoom() || g_LadderTeamMgr.IsLadderTeam() )
				{
					SafeSprintf( szTitle, sizeof( szTitle ), "초대" );
					m_OptionMap.insert( OptionMap::value_type( OPTION_INVITE, szTitle ) );
				}
			}

			if( m_bEnableMove )
			{
				switch( m_iUserPos )
				{
				case UP_TRAINING:
					SafeSprintf( szTitle, sizeof( szTitle ), "광장 따라가기" );
					m_OptionMap.insert( OptionMap::value_type( OPTION_FOLLOW, szTitle ) );
					break;
				case UP_BATTLE_ROOM:
					SafeSprintf( szTitle, sizeof( szTitle ), "전투 따라가기" );
					m_OptionMap.insert( OptionMap::value_type( OPTION_FOLLOW, szTitle ) );
					break;
				case UP_LADDER_TEAM:
					SafeSprintf( szTitle, sizeof( szTitle ), "진영전 따라가기" );
					m_OptionMap.insert( OptionMap::value_type( OPTION_FOLLOW, szTitle ) );
					break;
				case UP_LOBBY:
					SafeSprintf( szTitle, sizeof( szTitle ), "로비 따라가기" );
					m_OptionMap.insert( OptionMap::value_type( OPTION_FOLLOW, szTitle ) );
					break;
				case UP_HEADQUARTERS:
					SafeSprintf( szTitle, sizeof( szTitle ), "훈련소 따라가기" );
					m_OptionMap.insert( OptionMap::value_type( OPTION_FOLLOW, szTitle ) );
					break;
				case UP_HOUSE:
					SafeSprintf( szTitle, sizeof( szTitle ), "개인본부 따라가기" );
					m_OptionMap.insert( OptionMap::value_type( OPTION_FOLLOW, szTitle ) );
					break;
				/*
				case UP_GUILD_HQ:
					SafeSprintf( szTitle, sizeof( szTitle ), "본부 따라가기" );
					m_OptionMap.insert( OptionMap::value_type( OPTION_FOLLOW, szTitle ) );
					break;
				*/
				}
			}
		}

		if( m_bBestFriend )
		{
			SafeSprintf( szTitle, sizeof( szTitle ), "절친 해제" );
			m_OptionMap.insert( OptionMap::value_type( OPTION_RELEASE_BEST_FRIEND, szTitle ) );
		}
		else
		{
			SafeSprintf( szTitle, sizeof( szTitle ), "절친으로 등록" );
			m_OptionMap.insert( OptionMap::value_type( OPTION_ADD_BEST_FRIEND, szTitle ) );
		}
	}
	else if( m_iListType == UserInfoListWnd::TYPE_BLACKLIST )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), "차단 해제" );
		m_OptionMap.insert( OptionMap::value_type( OPTION_BLACK_DELETE, szTitle ) );
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

void UserInfoListOptionWnd::iwm_show()
{
	SetCheckParentOutSide( true );
}

void UserInfoListOptionWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void UserInfoListOptionWnd::OnProcess( float fTimePerSec )
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
			break;
		}
	}
}

void UserInfoListOptionWnd::OnRender()
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