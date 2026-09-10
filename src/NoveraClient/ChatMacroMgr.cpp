
#include "stdafx.h"

#include "ioLadderTeamMgr.h"

#include "ItemDefine.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioEtcItem.h"
#include "ioDateHelp.h"

#include "ioDecorationPrice.h"
#include "ioMapEventSceneMgr.h"

#include "ioTowerDefMode.h"

#include "ioSuperGashaponMgr.h"
#include "ioPowerUpManager.h"
#include "ioMissionManager.h"

#include "Housing/ioBlock.h"
#include "Housing/ioBlockManager.h"
#include "Housing/ioUserBlockStorage.h"
#include "Housing/ioBlockWorld.h"
#include "Housing/ioBlockEventParameter.h"

#include "GUI/ioFriendManager.h"
#include "GUI/ioUserInfoMgr.h"
#include "GUI/ioSP2GUIManager.h"
#include "GUI/ioBlackListManager.h"
#include "GUI/ChatMacro.h"
#include "GUI/TimeGateSearch.h"
#include "GUI/GuildWnd.h"
#include "GUI/AnnouncementWnd.h"
#include "GUI/DebugInfoWnd.h"
#include "GUI/ioMessageBox.h"
#include "GUI/RollBookWnd.h"
#include "GUI/PopUpStoreWnd.h"

template<> ChatMacroMgr* Singleton< ChatMacroMgr >::ms_Singleton = 0;
ChatMacroMgr::ChatMacroMgr()
{
	m_pPlayStage = NULL;

	m_dwMacroUserInfoTime = 0;
	m_dwMacroGuildInfoTime = 0;
	m_dwMacroChannelMoveTime = 0;
}

ChatMacroMgr::~ChatMacroMgr()
{
}

ChatMacroMgr& ChatMacroMgr::GetSingleton()
{
	return Singleton< ChatMacroMgr >::GetSingleton();
}

void ChatMacroMgr::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

int ChatMacroMgr::GetKeyword( IN char *szMacro , OUT char *szKeyword, int iKeywordSize )
{
	memset( szKeyword, 0, iKeywordSize );

	char cEOF = ' ';
	int  iLen = strlen( szMacro );
	int  i = 0;

	for(i = 0;i < iLen;i++)
	{
		if( i >= iKeywordSize )
			break;

		szKeyword[i] = szMacro[i];
		if( szMacro[i] == cEOF )
		{
			szKeyword[i] = NULL;
			break;
		}
	}

	return i;
}

void ChatMacroMgr::GetEtcItemMacro( char *szMacro, IntVec& MacroItemList )
{
	if( !m_pPlayStage )
		return;

	if( szMacro[0] != '/')
		return;

	char strCommand[MAX_EDIT_TEXT+1];
	memset( strCommand, 0, sizeof( strCommand ) );
	
	int key_word = 0;
	while( *szMacro != NULL )
	{
		strCommand[key_word++] = *szMacro++;
		if( *szMacro == ' ' )
			break;
	}

	g_EtcItemMgr.FindAllEtcItemMacro( strCommand, MacroItemList );
}

int ChatMacroMgr::ReturnEtcItemMacro( int iMacroType, char *szMacro )
{
	if( iMacroType != ChatMacro::NONE_MACRO )
		return 0;

	// 특수 모션 매크로 가져오기
	IntVec MotionItemList;
	GetEtcItemMacro( szMacro, MotionItemList );

	int iNotHasMotionEtcItem = 0;
	for( IntVec::iterator iter = MotionItemList.begin(); iter != MotionItemList.end(); ++iter )
	{
		int iEtcItem = *iter;
		if( iEtcItem != 0 )
		{
			ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioUserEtcItem::ETCITEMSLOT kItemSlot;
				if( P2PNetwork::IsNetworkPlaying() && !pUserEtcItem->GetEtcItem( iEtcItem, kItemSlot ) )
				{
					iNotHasMotionEtcItem = iEtcItem;
				}
				else
				{
					ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
					if( pOwnerChar )
						pOwnerChar->SetEtcItemMotionState( iEtcItem );
					
					return iEtcItem;
				}
			}
		}
	}

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iNotHasMotionEtcItem );
	if( pEtcItem )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(1), pEtcItem->GetName().c_str() );
		g_ChatMgr.SetSystemMsg( szMsg );
	}

	return 0;
}

int ChatMacroMgr::MacroProcess( const char *szChat , bool bSysMsg /*= false*/ )
{
	char szMacro[MAX_EDIT_TEXT+1] = "";
	strcpy( szMacro, szChat );

	ChatMacro cm;
	int key_word   = 0;
	int macro_type = cm.IsMacro( szMacro, key_word );

	if( bSysMsg && COMPARE( macro_type, 0, ChatMacro::MAX_MACRO ) )
	{
		switch( macro_type )
		{
		case ChatMacro::MACRO_MOVIE_MODE:
			if( m_pPlayStage && m_pPlayStage->HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) )
				break;
		case ChatMacro::MACRO_HELP:
		case ChatMacro::MACRO_NONE_UI:
		case ChatMacro::MACRO_USER_INFO:
		case ChatMacro::MACRO_GUILD_INFO:
		case ChatMacro::MACRO_TIME:		
			{
				char szMsg[MAX_PATH*2]="";
				SafeSprintf( szMsg, sizeof(szMsg), STR(1), ChatMacro::GetMacroText( macro_type )+1 );
				g_ChatMgr.SetSystemMsg( szMsg );
			}
			break;
		}		
	}

	int iEtcItem = ReturnEtcItemMacro( macro_type, szMacro );
	if( iEtcItem != 0 ) 
        return iEtcItem;

	if( SetCreateStructMacro( macro_type, szMacro ) )
		return 1;

	if( ExceptMacro( macro_type ) )
		return ChatMacro::NONE_MACRO;

	switch( macro_type )
	{
	case ChatMacro::MACRO_HELP:
		OnMacroHelp();
		break;
	case ChatMacro::MACRO_EXIT_LADDER:
		OnMacroLadderTeamLeave();
		break;
	case ChatMacro::MACRO_CHANNEL_INFO:
		OnMacroChannelInfo( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CONTRIBUTE:
		OnMacroContribute();
		break;
	case ChatMacro::MACRO_NONE_UI:
		OnMacroScreenShotMode();
		break;
	case ChatMacro::MACRO_USER_INFO:
		OnMacroUserInfo( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_AWARD:
		OnMacroAwardList();
		break;
	case ChatMacro::MACRO_INFO:
		OnMacroDeveloperInfo();
		break;
	case ChatMacro::MACRO_ROOM_ENTER:
		OnMacroDeveloperRoomEnter( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_UDP:
		OnMacroDeveloperUDP( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_GUILD_INFO:
		OnMacroGuildInfo( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_TIME:
		OnMacroCurrentTime();
		break;
	case ChatMacro::MACRO_SHOW_ANNOUNCEMENT:
		OnMacroShowAnnouncementWnd();
		break;
	case ChatMacro::MACRO_BLOCK_GULID_MARK:
		OnMacroBlockGuildMark( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_MOVIE_MODE:
		OnMacroMovieMode();
		break;
	case ChatMacro::MACRO_CHAT_MODE:
		OnMacroChatMode();
		break;
	case ChatMacro::MACRO_USER_RECORD:
		OnMacroUserRecord();
		break;
	case ChatMacro::MACRO_TEAM_INFO:
		OnMacroTeamInfo();
		break;
	case ChatMacro::MACRO_CHANNEL_MOVE:
		OnMacroChannelMove( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CAMP_INFO:
		OnMacroCampInfo();
		break;
	case ChatMacro::MACRO_CAMP_USER:
		OnMacroCampUser( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_GUILDNAME_CHANGE:
		OnMacroGuildNameChange( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_EVENT_TEST:
		OnMacroEventTest();
		break;
	case ChatMacro::MACRO_WHOLE_CHAT_OFF:
		OnMacroWholeChatOnOff( true );
		break;
	case ChatMacro::MACRO_WHOLE_CHAT_ON:
		OnMacroWholeChatOnOff( false );
		break;
	case ChatMacro::MACRO_CAMERA_TEST:
		OnMacroMapCameraEvent( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_FONT_TEXTURE:
		OnMacroFontTexture( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SHOW_WND_ID:
		g_GUIMgr.ChildWndShowIDList();
		break;
	case ChatMacro::MACRO_QUEST_GUIDE:
		OnMacroQuestGuide( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_QUEST_OCCUR:
		OnMacroQuestOccur( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_QUEST_ATTAIN:
		OnMacroQuestAttain( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_QUEST_DELETE:
		OnMacroQuestDelete();
		break;
	case ChatMacro::MACRO_PRESENT_DELETE:
		OnMacroPresentDelete();
		break;
	case ChatMacro::MACRO_CHAT_INFO_OFF:
		OnMacroChatInfoOnOff( true );
		break;
	case ChatMacro::MACRO_CHAT_INFO_ON:
		OnMacroChatInfoOnOff( false );
		break;
	case ChatMacro::MACRO_CONTROL_SYNC_EX_ON:
	case ChatMacro::MACRO_CONTROL_SYNC_EX_OFF:
		OnMacroControlSyncExOnOff( macro_type );
		break;
	case ChatMacro::MACRO_EVENT_SHOP_GOODS_LIST:
		OnMacroEventShopGoodsList( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_EVENT_SHOP_GOODS_BUY:
		OnMacroEventShopGoodsBuy( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_EVENT_SHOP_STATE:
		OnMacroEventShopState( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_EVENT_SHOP_BUY_USER_CLEAR:
		OnMacroEventShopBuyUserClear( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CUSTOM_DDS_FROM_JPG:
		OnMacroCustomDDSFromJPG( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CUSTOM_JPG_MIX_DDS_FROM_DDS:
		OnMacroCustomJPGMixDDSFromDDS( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CUSTOM_DDS_FROM_LSC:
		OnMacroCustomDDSFromLSC( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CUSTOM_LSC_FROM_DDS:
		OnMacroCustomLSCFromDDS( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CUSTOM_ENTITY_TEXTURE:
		OnMacroCustomEntityTexture( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SHOW_WND:
		OnMacroShowWnd( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_PRESENT:
		OnMacroPresent( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_BEST_FRIEND_APPLICATION:
		OnMacroBestFriendApplication( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_BEST_FRIEND_AGREE:
		OnMacroBestFriendAgree( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_BEST_FRIEND_REFUSE:
		OnMacroBestFriendRefuse( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_BEST_FRIEND_CLEAR:
		OnMacroBestFriendClear( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_MULTI_MAP_CHANGE:
		OnMacroMultiMapChange( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_MACRO_NOTIFY:
		OnMacroMacroNotify( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_EXTRAITEM_GROWTH_CATALYST:
		OnMacroExtraItemGrowthCatalyst( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_GROWTH_LEVELUP:
		OnMacroGrowthLevelup( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_MONSTER_CLOSE:
		OnMacroMonsterClose( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_MONSTER_SPAWN:
		OnMacroMonsterSpawn( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_BINGO_PRESENT:
		OnMacroBingoPresent( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SUPERGASHAPON_GET_ALL:
		OnMacroSuperGashaponAllPackage( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SUPERGASHAPON_GET:
		OnMacroSuperGashaponPackage( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SUPERGASHAPON_LIMIT_INFO_GET:
		OnMacroSuperGashaponInfo( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SUPERGASHAPON_LIMIT_RESET:
		OnMacroSuperGashaponInfoReset( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SEND_RECIVE_CHECK_ON:
		OnMacroSendReciveCheckOn(  &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SEND_RECIVE_CHECK_OFF:
		OnMacroSendReciveCheckOff(  &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ATTENDANCE_WND_SHOW:
		OnMacroShowAttendanceWnd( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ATTENDANCE_ADD_DAY:
		OnMacroAttendanceAddDay( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ATTENDANCE_ADD_PREV_MONTH:
		OnMacroAttendancePrevMonth( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ATTENDANCE_RESET:
		OnMacroAttendanceReset( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ATTENDANCE_SET_DATE:
		OnMacroAttendanceDateModify( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_AWAKE_SET_DATE:
		OnMacroAwake( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CHAR_POWER_UP_ADD:
		OnMacroCharPowerUpAdd( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ITEM_SLOT_INDEX_FIND:
		OnMacroItemSlotIndex( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ITEM_POWER_UP_ADD:
		OnMacroItemPowerUpAdd( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ALL_LSC_FROM_DDS:
		LSCAllDecypt( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CANNOT_OPEN_DDS_FILE:
		OnMacroCannotOpenDDSFile( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_CHAR_INFO_WND_SHOW:
		OnMacroCharInfoWndShow( &szMacro[key_word] );
		break;	
	case ChatMacro::MACRO_SET_MISSION:
		OnMacroSetDateMission( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SET_DATE_MISSION:
		OnMacroSetNextDateMission( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SET_MISSION_VALUE:
		OnMacroSetMissionValue( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ROLLBOOK_WND_SHOW:
		OnMacroRollBookWndShow( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_ROLLBOOK_PROGRESS:
		OnMacroRollBookProgress( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_FPS_LOWER:
	case ChatMacro::MACRO_FPS_UPPER:
		OnMacroFPS( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_TIMER_CHANGE:
		OnMacroTimerChange( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_FPS_LAG_CHECK:
		OnMacroFpsLagCheck( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_HOUSING_TEST:
		OnMacroHousingTest( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_HOUSING_CMD:
		OnMacroHousingCommand( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_HOUSING_SAVE:
		OnMacroHousingBlockSave( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_COMFORTABLE_LIFE:
		OnMacroComfortableLife( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_TOURNAMENT_NEXT_STATE:
		OnMacroTournamentNextState( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_TOURNAMENT_RESULT:
		OnMacroTournamentResult( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_SHOW_POPUP:
		OnMacroOpenPopup( &szMacro[key_word] );
		break;
	case ChatMacro::MACRO_PRACTICE_COMPLETE:
		OnMacroPracticeCompleted( &szMacro[key_word] );
		break;
#if defined( SRC_OVERSEAS )
	case ChatMacro::MACRO_GASHAPON_GET_ALL:
		OnMacroGashaponAllPackage( &szMacro[key_word] );
		break;
#endif

	default:
		if( szChat[0] == '/')
		{
			g_ChatMgr.SetSystemMsg( STR(2) );
			return ChatMacro::MACRO_HELP;
		}
		else
			return ChatMacro::NONE_MACRO;

	}
	return macro_type;
}

void ChatMacroMgr::OnMacroHelp()
{
	// 리얼 폰트이므로 띄어쓰기를 논리가 아닌 눈으로 보면서 맞춰야함
	g_ChatMgr.SetSystemMsg( STR(1) );
	g_ChatMgr.SetMsg( STR(2) );
	g_ChatMgr.SetMsg( STR(3) );
}

void ChatMacroMgr::OnMacroLadderTeamLeave()
{
	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_ChatMgr.SetSystemMsg( STR(1) );
		return;
	}
	else if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_ChatMgr.SetSystemMsg( STR(2) );
		return;
	}
	else
	{
		SP2Packet kPacket( CTPK_LADDERTEAM_LEAVE );
		TCPNetwork::SendToServer( kPacket );
	}
}

void ChatMacroMgr::OnMacroChatMode()
{
	if( m_pPlayStage == NULL ) 
	{
		g_ChatMgr.SetSystemMsg( STR(1) );
		return;
	}

	if( m_pPlayStage->GetPlazaModeType() != PT_COMMUNITY )
	{
		g_ChatMgr.SetSystemMsg( STR(2) );
		return;
	}

	ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
	if( !pOwner || !pOwner->IsCanChatModeState() )
	{
		g_ChatMgr.SetSystemMsg( STR(3) );
		return;
	}

	if( pOwner->GetExperienceMode() != EMS_NONE )
	{
		g_ChatMgr.SetSystemMsg( STR(5) );
		return;
	}

	if( pOwner->GetChatModeState() == CMS_NONE )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CTPK_CHAT_MODE );
			kPacket << pOwner->GetCharName();
			kPacket << CHAT_MODE_CHANGE_CHAT;
			kPacket << false;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else if( pOwner->GetChatModeState() == CMS_CHAT )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CTPK_CHAT_MODE );
			kPacket << pOwner->GetCharName();
			kPacket << CHAT_MODE_CHANGE_NORMAL;
			kPacket << true;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		g_ChatMgr.SetSystemMsg( STR(4) );
	}
}

void ChatMacroMgr::OnMacroScreenShotMode()
{
	if( m_pPlayStage == NULL ) return;

	if( m_pPlayStage->GetModeType() == MT_MYROOM )
	{
		if( ioMyRoomMode::GetMyRoomType() != MRT_PRACTICE )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}
	}
	else if( m_pPlayStage->GetModeType() == MT_PRACTICE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "수련장에서는 스크린샷 모드를 사용할 수 없습니다." );
		return;
	}

	g_GUIMgr.SetNoneUIMode( true );
}

void ChatMacroMgr::OnMacroJoinPlaza( ioPlayStage *pPlayStage, char *szMacro, bool bInvite )
{
	if( pPlayStage != NULL ) 
	{
		if( pPlayStage->GetModeType() != MT_MYROOM && pPlayStage->GetModeType() != MT_TRAINING && pPlayStage->GetModeType() != MT_HEADQUARTERS && pPlayStage->GetModeType() != MT_HOUSE )
			return;
	}

	if( g_App.IsMouseBusy() ) return;	

	// Get Room Index
	char szName[MAX_PATH];
	int iMacro = GetKeyword( szMacro, szName, sizeof(szName) );
	iMacro++;

	// Get Room Password
	char szPassword[MAX_PATH] = "";
	if( strlen( &szMacro[iMacro] ) > 0 )
	{
		sprintf_e( szPassword, "%s", &szMacro[iMacro] );
	}	

	if( g_MyInfo.IsTutorialUser() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( g_MyInfo.GetCharCount() == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	int iRoomIndex = -2;
	if( strcmp( szName, "" ) != 0 )
		iRoomIndex = atoi( szName );

	if( iRoomIndex == -2 )
	{
		return;
	}

	TimeGateSearch *pSearchWnd = dynamic_cast<TimeGateSearch*>(g_GUIMgr.FindWnd( TIMEGATE_SEARCH ));
	if( pSearchWnd )
	{
		pSearchWnd->SetReserve();
		pSearchWnd->SetSearching( STR(4), TimeGateSearch::ST_PLAZA );
	}

	SP2Packet kPacket( CTPK_SEARCH_PLAZA_ROOM );
	kPacket << iRoomIndex << bInvite << szPassword;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroHeadQuaterPage( char *szMacro )
{
	char szID[MAX_PATH];
	GetKeyword( szMacro, szID, sizeof(szID) );

	g_App.OpenHeadQuaterPage( szID );
}

void ChatMacroMgr::OnMacroContribute()
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	if( m_pPlayStage )
		m_pPlayStage->ContributeLOG();
}

void ChatMacroMgr::OnMacroUIInitialize()
{
	g_GUIMgr.SetUIPostionToDefault();

	if( m_pPlayStage )
		m_pPlayStage->MySoldierDefPos();
}

void ChatMacroMgr::OnMacroAwardList()
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	g_GUIMgr.ShowWnd( AWARD_DEBUG_WND );
}

void ChatMacroMgr::OnMacroDeveloperInfo()
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_ROOM_INFO;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroDeveloperRoomEnter( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) && !g_App.IsMeGameMaster() ) return;
	if( g_App.IsMouseBusy() ) return;

	if( g_MyInfo.GetCharCount() == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	char szNumber[MAX_PATH];
	GetKeyword(szMacro, szNumber, sizeof(szNumber) );

	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

	int iLevel = atoi( szNumber );
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_ROOM_ENTER << iLevel;
	TCPNetwork::SendToServer( kPacket );	
}

void ChatMacroMgr::OnMacroDeveloperUDP( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	g_GUIMgr.ShowWnd( P2P_CHCEK_WND );
}

void ChatMacroMgr::OnMacroUserInfo( char *szMacro )
{
	if( m_dwMacroUserInfoTime != 0 )
	{
		if( FRAMEGETTIME() - m_dwMacroUserInfoTime < 10000 )
		{
			g_ChatMgr.SetSystemMsg( STR(1) );
			return;
		}		
	}

	char szID[MAX_PATH];
	GetKeyword( szMacro, szID, sizeof(szID) );

	if( !g_App.IsRightID( szID ) )
	{
		g_ChatMgr.SetSystemMsg( STR(2) );
		return;
	}

	m_dwMacroUserInfoTime = FRAMEGETTIME();

	ioHashString szHashID = szID;
	if( g_UserInfoMgr.GetUserInfoNodeExist( szHashID ) )
		g_App.OpenMannerWnd( szID, true );
	else
	{
		SP2Packet kPacket( CTPK_USER_INFO_EXIST );
		kPacket << szHashID;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
		g_App.SetReserveCheckExistID(szHashID); // 네트워크 부하를 줄이기 위해서 로컬에 저장.
	}
}

void ChatMacroMgr::OnMacroGuildInfo( char *szMacro )
{
	if( m_dwMacroGuildInfoTime != 0 )
	{
		if( FRAMEGETTIME() - m_dwMacroGuildInfoTime < 10000 )
		{
			g_ChatMgr.SetSystemMsg( STR(1) );
			return;
		}		
	}
	m_dwMacroGuildInfoTime = FRAMEGETTIME();

	char szGuildName[MAX_PATH];
	GetKeyword(szMacro, szGuildName, sizeof(szGuildName) );

	if( !g_App.IsRightID( szGuildName ) )
	{
		g_ChatMgr.SetSystemMsg( STR(2) );
		return;
	}

	ioHashString szHashGuildName = szGuildName;
	ioGuildData *pData = g_GuildInfo.GetGuildDataExist( szHashGuildName );
	if( pData )
	{
		GuildMainWnd *pGuildMainWnd = dynamic_cast<GuildMainWnd *>(g_GUIMgr.FindWnd( GUILD_MAIN_WND ));
		if( pGuildMainWnd )
			pGuildMainWnd->SetGuildIndex( pData->GetGuildIndex() );
	}
	else
	{
		SP2Packet kPacket( CTPK_GUILD_EXIST );
		kPacket << szHashGuildName;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
}

void ChatMacroMgr::OnMacroCurrentTime()
{
	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

	struct tm *pDate = localtime(&kCurServerTime);
	if( !pDate )
	{
		LOG.PrintTimeAndLog( 0, "Macro time pDate == NULL" );
		return;
	}

	int i12Hour = pDate->tm_hour ;
	char szHour[MAX_PATH]="";
	if( COMPARE( pDate->tm_hour, 0, 11+1) )
		SafeSprintf( szHour, sizeof( szHour) , STR(1) );
	else
		SafeSprintf( szHour, sizeof( szHour) , STR(2) );

	// 24H->12H
	if( pDate->tm_hour > 12 )
		i12Hour -= 12;
	else if( pDate->tm_hour == 0 )
		i12Hour = 12;

	g_ChatMgr.SetSystemMsg( STR(3), szHour, i12Hour, pDate->tm_min ); 
}

void ChatMacroMgr::OnMacroShowAnnouncementWnd()
{
	if( g_AnnouncementManualMgr.IsEmpty() )
		return;

	g_GUIMgr.ShowWnd( ANNOUNCEMENT_WND );
}

void ChatMacroMgr::OnMacroBlockGuildMark( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) && !g_App.IsMeGameMaster() ) return;
	if( strlen( szMacro ) == 0 ) return;

	ioHashString szGuildName = szMacro;
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_GUILDMARK_BLOCK << szGuildName;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroMovieMode()
{
	g_App.CheckMouseControlMode( true );
}

void ChatMacroMgr::OnMacroEnterTraining( ioPlayStage *pPlayStage, bool bSafetyRoom )
{

}

void ChatMacroMgr::OnMacroEnterFighting( ioPlayStage *pPlayStage )
{
	if( pPlayStage != NULL ) 
	{
		if( pPlayStage->GetModeType() != MT_MYROOM && pPlayStage->GetModeType() != MT_TRAINING && pPlayStage->GetModeType() != MT_HEADQUARTERS && pPlayStage->GetModeType() != MT_HOUSE )
			return;
	}

	if( g_MyInfo.IsTutorialUser() ) 
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	if( g_MyInfo.GetActiveCharCount() == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
}

void ChatMacroMgr::OnMacroUserRecord()
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	if( m_pPlayStage )
		m_pPlayStage->RecordLOG();
}

void ChatMacroMgr::OnMacroTeamInfo()
{
	if( !g_App.IsMeDeveloper( true ) ) return;

    if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_ChatMgr.SetSystemMsg( STR(1), g_BattleRoomMgr.GetIndex() ); 
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_ChatMgr.SetSystemMsg( STR(2), g_LadderTeamMgr.GetLadderTeamIndex() ); 
	}
	else
		g_ChatMgr.SetSystemMsg( STR(3) ); 
}

void ChatMacroMgr::OnMacroChannelMove( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;
	if( !m_pPlayStage ) return;

	if( g_App.IsMouseBusy() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	else if( m_pPlayStage->GetModeType() != MT_MYROOM )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}
	else if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}
//	else if( m_dwMacroChannelMoveTime != 0 )
//	{
//		if( FRAMEGETTIME() - m_dwMacroChannelMoveTime < 30000 )
//		{
//			g_ChatMgr.SetSystemMsg( "30초에 한번만 사용할 수 있습니다." );
//			return;
//		}		
//	}
	else if( strcmp( szMacro, "" ) == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}

	m_dwMacroChannelMoveTime = FRAMEGETTIME();
	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

	char szNumber[MAX_PATH] = "";
	GetKeyword( szMacro, szNumber, sizeof(szNumber) );
	int iServerNumber = atoi( szNumber );

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_MYROOM_SERVER_CHANGE );
	kPacket << iServerNumber;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroChannelInfo( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;
	if( !m_pPlayStage ) return;

	if( m_pPlayStage->GetModeType() != MT_MYROOM && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
	{
		g_ChatMgr.SetSystemMsg( STR(1) );
		return;
	}
	else if( strcmp( szMacro, "" ) == 0 )
	{
		g_ChatMgr.SetSystemMsg( STR(2) );
		return;
	}

	char szNumber[MAX_PATH] = "";
	GetKeyword( szMacro, szNumber, sizeof(szNumber) );
	int iCurPage = atoi( szNumber );

	if( iCurPage < 0 )
	{
		g_ChatMgr.SetSystemMsg( STR(3) );
		return;
	}

	enum { MAX_INFO_LIST = 10, };

	SP2Packet kPacket( CTPK_SERVER_LOBBY_INFO );
	kPacket << iCurPage << MAX_INFO_LIST;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroCampInfo()
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	g_ChatMgr.SetSystemMsg( STR(1), 
		g_CampMgr.GetCampPoint( CAMP_BLUE ), g_CampMgr.GetCampBonusPoint( CAMP_BLUE ),
		g_CampMgr.GetCampSpecialEntryUserCount( CAMP_BLUE ), 
		g_CampMgr.GetCampPoint( CAMP_RED ), g_CampMgr.GetCampBonusPoint( CAMP_RED ),
		g_CampMgr.GetCampSpecialEntryUserCount( CAMP_RED ) );
}

void ChatMacroMgr::OnMacroCampUser( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	char szUserName[MAX_PATH];
	GetKeyword( szMacro, szUserName, sizeof(szUserName) );

	if( !g_App.IsRightID( szUserName ) )
	{
		g_ChatMgr.SetSystemMsg( STR(1) );
		return;
	}

	UserInfoNode *pUserInfoNode = g_UserInfoMgr.GetUserInfoNode( szUserName );
	if( pUserInfoNode )
	{		
		g_ChatMgr.SetSystemMsg( STR(2), pUserInfoNode->GetUserID().c_str(), pUserInfoNode->GetLadderPoint() );
	}
}	

void ChatMacroMgr::OnMacroGuildNameChange( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;
	if( g_App.IsMouseBusy() ) return;	

	if( !g_GuildInfo.IsGuild() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	else if( !g_GuildInfo.IsGuildMaster() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	char szGuildName[MAX_PATH] = "";
	GetKeyword( szMacro, szGuildName, sizeof( szGuildName ) );	

	ioHashString szNewGuildName = szGuildName;
	if( szNewGuildName.IsEmpty() )
	{
		g_ChatMgr.SetSystemMsg( STR(3) );
		return;
	}

	if( !g_App.IsRightID( szNewGuildName.c_str() ) )
	{
		g_ChatMgr.SetSystemMsg( STR(4) );
		return;
	}

	ioHashString szNotMakeString;
	if( g_App.IsNotMakeID( szNewGuildName.c_str(), szNotMakeString ) )
	{
		g_ChatMgr.SetSystemMsg( STR(5), szNotMakeString.c_str() );
		return;
	}

	char szSlangName[MAX_PATH]="";
	StringCbPrintf( szSlangName, sizeof( szSlangName ), g_SlangUtil.ConvertString( szNewGuildName.c_str() ) );
	if( szNewGuildName != szSlangName )
	{
		g_ChatMgr.SetSystemMsg( STR(6) );
		return;
	}

	if( g_SlangUtil.IsLimitedGuildName( szNewGuildName.c_str() ) ) 
	{
		g_ChatMgr.SetSystemMsg( STR(7) );
		return;
	}

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem || !pUserEtcItem->IsCanUse( ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );		
		return;
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE;
	kPacket << g_GuildInfo.GetGuildIndex() << szNewGuildName;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void ChatMacroMgr::OnMacroEventTest()
{
	if( !g_App.IsMeDeveloper( true ) ) return;
	if( g_App.IsMouseBusy() ) return;	

	// 서버에서 필요한 이벤트를 초기화 시킨다.
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_EVENT_TEST;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void ChatMacroMgr::OnMacroWholeChatOnOff( bool bOff )
{
	Setting::SetWholeChatOff( bOff );

	char szBuf[MAX_PATH] = "";
	if( bOff )
		SafeSprintf( szBuf, sizeof( szBuf ), STR(1) );
	else
		SafeSprintf( szBuf, sizeof( szBuf ), STR(2) );

	ioComplexStringPrinter kPrinter;
	g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

	kPrinter.SetTextColor( TCT_DEFAULT_ORANGE );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szBuf );

	g_ChatMgr.SetChatComplexString( STR(4), kPrinter );
}

void ChatMacroMgr::OnMacroMapCameraEvent( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	g_MapEventSceneMgr.LoadMapEventScene();
	if( m_pPlayStage )
	{
		char szNumber[MAX_PATH] = "";
		GetKeyword( szMacro, szNumber, sizeof( szNumber ) );
		m_pPlayStage->StartModeStartEventCamera( false, false, atoi( szNumber ) );
	}
}

void ChatMacroMgr::OnMacroFontTexture( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	FontTextureTestWnd *pFontTexture = dynamic_cast<FontTextureTestWnd*>(g_GUIMgr.FindWnd( FONT_TEXTURE_WND ));
	if( pFontTexture )
	{
		pFontTexture->HideWnd();
		char szNumber[MAX_PATH] = "";
		GetKeyword( szMacro, szNumber, sizeof( szNumber ) );
		pFontTexture->SetFontSize( atoi( szNumber ) );
		pFontTexture->ShowWnd();
	}
}

void ChatMacroMgr::OnMacroQuestGuide( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	g_QuestGuide.RotateMouseView();
}

void ChatMacroMgr::OnMacroQuestOccur( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 2 )  
	{
		g_ChatMgr.SetSystemMsg( STR(1) );
		return;
	}

	DWORD dwMainIndex = atoi( kString[0].c_str() );
	DWORD dwSubIndex  = atoi( kString[1].c_str() );
	QuestParent *pQuestParent = g_QuestMgr.GetQuest( dwMainIndex, dwSubIndex );
	if( !pQuestParent )
	{
		g_ChatMgr.SetSystemMsg( STR(2), dwMainIndex, dwSubIndex );
		return;
	}

	ioQuest *pQuest = g_MyInfo.GetQuest();
	if( pQuest )
	{
		if( pQuest->IsQuestIndexCheck( dwMainIndex, dwSubIndex ) )
		{
			g_ChatMgr.SetSystemMsg( STR(3), pQuestParent->GetTitle().c_str() );
			return;
		}
	}
	g_QuestMgr.AddQuestOccurList( pQuestParent );
}

void ChatMacroMgr::OnMacroQuestAttain( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 3 )
	{
		g_ChatMgr.SetSystemMsg( STR(1) );
		return;
	}

	DWORD dwMainIndex   = atoi( kString[0].c_str() );
	DWORD dwSubIndex    = atoi( kString[1].c_str() );
	DWORD dwAttainValue = atoi( kString[2].c_str() );
	QuestParent *pQuestParent = g_QuestMgr.GetQuest( dwMainIndex, dwSubIndex );
	if( !pQuestParent )
	{
		g_ChatMgr.SetSystemMsg( STR(2), dwMainIndex, dwSubIndex );
		return;
	}

	ioQuest *pQuest = g_MyInfo.GetQuest();
	if( pQuest )
	{
		if( !pQuest->IsQuestIndexCheck( dwMainIndex, dwSubIndex ) )
		{
			g_ChatMgr.SetSystemMsg( STR(3), pQuestParent->GetTitle().c_str() );
			return;
		}

		QuestData &rkQuestData = pQuest->GetQuestData( dwMainIndex, dwSubIndex );
		if( rkQuestData.GetState() != QS_PROGRESS )
		{
			g_ChatMgr.SetSystemMsg( STR(4), pQuestParent->GetTitle().c_str() );
			return;
		}

		SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
		kPacket << DEVELOPER_QUEST_ATTAIN;
		kPacket << rkQuestData.GetIndexData() << dwAttainValue << rkQuestData.GetMagicData() << rkQuestData.GetDateData();
		TCPNetwork::SendToServer( kPacket );
	}
}

void ChatMacroMgr::OnMacroQuestDelete()
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	ioQuest *pQuest = g_MyInfo.GetQuest();
	if( pQuest )
	{
		pQuest->ClearQuestData();
		//
		SP2Packet kPacket( CTPK_QUEST_ALL_DELETE );
		TCPNetwork::SendToServer( kPacket );
		//
		g_QuestMgr.QuestInfoChange();
	}
}

void ChatMacroMgr::OnMacroPresentDelete()
{
	if( !g_App.IsMeDeveloper( true ) ) return;
	
	// 서버로 전송
	SP2Packet kPacket( CTPK_PRESENT_ALL_DELETE );
	TCPNetwork::SendToServer( kPacket );

	g_PresentMgr.AllDeletePresentData();
}

void ChatMacroMgr::OnMacroChatInfoOnOff( bool bOff )
{
	if( !g_App.IsMeDeveloper( true ) && !g_App.IsMeGameMaster() ) return;

	if( !bOff )
		Setting::SetChatInfoOff( bOff );

	char szBuf[MAX_PATH] = "";
	if( !bOff )
		SafeSprintf( szBuf, sizeof( szBuf ), STR(1) );
	else
		SafeSprintf( szBuf, sizeof( szBuf ), STR(2) );

	ioComplexStringPrinter kPrinter;
	g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

	kPrinter.SetTextColor( TCT_DEFAULT_ORANGE );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szBuf );

	g_ChatMgr.SetChatComplexString( STR(4), kPrinter );

	if( bOff )
		Setting::SetChatInfoOff( bOff );
}

void ChatMacroMgr::OnMacroControlSyncExOnOff( int iMacro )
{
#ifndef SHIPPING

	if( iMacro == ChatMacro::MACRO_CONTROL_SYNC_EX_ON )
		P2PNetwork::SetControlSyncExpand( true );
	else
		P2PNetwork::SetControlSyncExpand( false );

#endif
}

void ChatMacroMgr::OnMacroEventShopGoodsList( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	// 서버로 전송
	SP2Packet kPacket( CTPK_EVENT_SHOP_GOODS_LIST );
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroEventShopGoodsBuy( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	int iGoodsIndex = atoi( szMacro );
	SP2Packet kPacket( CTPK_EVENT_SHOP_GOODS_BUY );
	kPacket << iGoodsIndex;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroEventShopState( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	// 서버로 전송
	int iShopState = atoi( szMacro );
	SP2Packet kPacket( CTPK_EVENT_SHOP_STATE );
	kPacket << iShopState;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroMonsterClose( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	char szNumber[MAX_PATH];
	GetKeyword(szMacro, szNumber, sizeof(szNumber) );
	int nNpc = atoi( szNumber );

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_SPAWN_NPC << nNpc;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroMonsterSpawn( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	char szNumber[MAX_PATH];
	GetKeyword(szMacro, szNumber, sizeof(szNumber) );

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_PLAZA_NPC_SPAWN << szNumber;
	TCPNetwork::SendToServer( kPacket );
}


void ChatMacroMgr::OnMacroBingoPresent( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;
	
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_BINGO_PRESENT;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroEventShopBuyUserClear( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	// 서버로 전송
	SP2Packet kPacket( CTPK_EVENT_SHOP_BUY_USER_CLEAR );
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroSuperGashaponAllPackage( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	char szNumber[MAX_PATH];
	GetKeyword(szMacro, szNumber, sizeof(szNumber) );
	DWORD dwEtcItemCode = atoi( szNumber );	

	ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( dwEtcItemCode );
	if( !pItem )
	{
		g_ChatMgr.SetSystemMsg( "잘못된 특별아이템 코드입니다 : %d", dwEtcItemCode );
		return;
	}

	if( !g_SuperGashaponMgr.IsSuperGashpon( dwEtcItemCode ) )
	{
		g_ChatMgr.SetSystemMsg( "슈퍼가챠가 아닙니다. : %d", dwEtcItemCode );
		return;
	}

	// 서버로 전송
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_SUPERGASHAPON_GET_ALL << dwEtcItemCode;
	
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroSuperGashaponPackage( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 2 )
	{
		g_ChatMgr.SetSystemMsg( "값 2개의 필요(etcitemcode.present_index)" );
		return;
	}

	DWORD dwEtcItemCode = atoi( kString[0].c_str() );
	int   iPackageIndex = atoi( kString[1].c_str() );

	ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( dwEtcItemCode );
	if( !pItem )
	{
		g_ChatMgr.SetSystemMsg( "잘못된 특별아이템 코드입니다 : %d", dwEtcItemCode );
		return;
	}

	if( !g_SuperGashaponMgr.IsSuperGashpon( dwEtcItemCode ) )
	{
		g_ChatMgr.SetSystemMsg( "슈퍼가챠가 아닙니다. : %d", dwEtcItemCode );
		return;
	}
	
	const ioSuperGashaponMgr::SuperGashaponPackage& rkPresent = g_SuperGashaponMgr.GetSuperGashaponPackage( dwEtcItemCode, iPackageIndex );
	if( (int)rkPresent.m_vSuperGashaponElement.size() <= iPackageIndex )
	{
		g_ChatMgr.SetSystemMsg( "package_index가 유효하지 않습니다", rkPresent.m_vSuperGashaponElement.size() );
		return;
	}

	// 서버로 전송
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_SUPERGASHAPON_GET << dwEtcItemCode << iPackageIndex;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroSuperGashaponInfo( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	char szNumber[MAX_PATH];
	GetKeyword(szMacro, szNumber, sizeof(szNumber) );
	DWORD dwEtcItemCode = atoi( szNumber );	

	ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( dwEtcItemCode );
	if( !pItem )
	{
		g_ChatMgr.SetSystemMsg( "잘못된 특별아이템 코드입니다 : %d", dwEtcItemCode );
		return;
	}

	if( !g_SuperGashaponMgr.IsSuperGashpon( dwEtcItemCode ) )
	{
		g_ChatMgr.SetSystemMsg( "슈퍼가챠가 아닙니다. : %d", dwEtcItemCode );
		return;
	}

	// 서버로 전송
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_SUPERGASHAPON_LIMIT_INFO_GET << dwEtcItemCode;

	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroSuperGashaponInfoReset( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	char szNumber[MAX_PATH];
	GetKeyword(szMacro, szNumber, sizeof(szNumber) );
	DWORD dwEtcItemCode = atoi( szNumber );	

	ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( dwEtcItemCode );
	if( !pItem )
	{
		g_ChatMgr.SetSystemMsg( "잘못된 특별아이템 코드입니다 : %d", dwEtcItemCode );
		return;
	}

	if( !g_SuperGashaponMgr.IsSuperGashpon( dwEtcItemCode ) )
	{
		g_ChatMgr.SetSystemMsg( "슈퍼가챠가 아닙니다. : %d", dwEtcItemCode );
		return;
	}

	// 서버로 전송
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_SUPERGASHAPON_LIMIT_INFO_RESET << dwEtcItemCode;

	TCPNetwork::SendToServer( kPacket );
};

void ChatMacroMgr::OnMacroSendReciveCheckOn( char *szMacro )
{
	Help::SetSendReciveCheck( true );
}

void ChatMacroMgr::OnMacroSendReciveCheckOff( char *szMacro )
{
	Help::SetSendReciveCheck( false );
}

void ChatMacroMgr::OnMacroShowAttendanceWnd( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) )
		return;
	
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_ATTENDANCE_WND_SHOW;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroAttendanceAddDay( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) )
		return;

	char szDayCount[MAX_PATH];
	GetKeyword(szMacro, szDayCount, sizeof(szDayCount) );
	int iDayCount = atoi( szDayCount );	

	if( iDayCount <= 0 )
	{
		g_ChatMgr.SetSystemMsg( "값이 0을 넘겨야 합니다.(iDayCount)" );
		return;
	}
			
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_ATTENDANCE_ADD_DAY;
	kPacket << iDayCount;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroAttendancePrevMonth( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) )
		return;

	char szDayCount[MAX_PATH];
	GetKeyword(szMacro, szDayCount, sizeof(szDayCount) );
	int iDayCount = atoi( szDayCount );	
	
	if( iDayCount <= 0 )
	{
		g_ChatMgr.SetSystemMsg( "값이 0을 넘겨야 합니다.(iDayCount)" );
		return;
	}

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_ATTENDANCE_ADD_PREV_MONTH;
	kPacket << iDayCount;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroAttendanceReset( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) )
		return;

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_ATTENDANCE_RESET;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroAttendanceDateModify( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) )
		return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 3 )
	{
		g_ChatMgr.SetSystemMsg( "값 3개의 필요(year.month.day) - ex)13.9.23" );
		return;
	}

	int iYear  = atoi( kString[0].c_str() );
	int iMonth = atoi( kString[1].c_str() );
	int iDay   = atoi( kString[2].c_str() );
	
	if( iYear <= 0 || 99 < iYear )
	{
		g_ChatMgr.SetSystemMsg( "년 값이 잘못됬습니다( 0 ~ 99 )" );
		return;
	}

	if( iMonth < 1 || 12 < iMonth )
	{
		g_ChatMgr.SetSystemMsg( "월 값이 잘못됬습니다" );
		return;
	}

	if( iDay < 1 || DateHelp::GetMonthDayPeriod( iYear, iMonth ) < iDay )
	{
		g_ChatMgr.SetSystemMsg( "일 값이 잘못됬습니다( 최대 : %d, 입력 : %d)", DateHelp::GetMonthDayPeriod( iYear, iMonth ), iDay );
		return;
	}

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_ATTENDANCE_SET_DATE;
	kPacket << iYear << iMonth << iDay;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroAwake( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) )
		return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 2 )
	{
		g_ChatMgr.SetSystemMsg( "값 2개의 필요(용병번호.현재시간 기준으로 종료될 분) - ex)1.10 - 아이언나이트 10분뒤 종료" );
		return;
	}

	int iClassType	= atoi( kString[0].c_str() );
	int iMin		= atoi( kString[1].c_str() );

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_AWAKE_SET_DATE;
	kPacket << iClassType;
	kPacket << iMin;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroCharPowerUpAdd( char *szMacro )
{
	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 1 )
	{
		g_ChatMgr.SetSystemMsg( "값 2개의 필요(용병번호) - ex)1 - 아이언나이트 용병강화" );
		return;
	}

	int iClassType	= atoi( kString[0].c_str() );
	
	ioHashString szError;
	if( g_PowerUpManager.CheckEnableCharPowerUp( iClassType, szError ) )
	{
		//CheckEnableCharPowerUp함수에서 유효한 값인지 체크됨
		int iCharIndex = g_MyInfo.GetCharIndexByClassType( iClassType );
		ioEtcItemSoulStone* pEtcItem = dynamic_cast<ioEtcItemSoulStone*>( g_EtcItemMgr.FindEtcItem( g_PowerUpManager.GetMtrlEtcItemCode() ) );
		if( pEtcItem )
			pEtcItem->OnUseGradeAdjustment( iCharIndex, PUTT_CHAR );
	}
	else
	{
		g_ChatMgr.SetSystemMsg( "%s", szError );
	}
}

void ChatMacroMgr::OnMacroItemSlotIndex( char *szMacro )
{
	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.empty() )
	{
		g_ChatMgr.SetSystemMsg( "값 1개의 필요(아이템코드) - ex)/아이템슬롯 100001" );
		return;
	}

	int iItemCode = atoi( kString[0].c_str() );

	ioUserExtraItem* pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		g_ChatMgr.SetSystemMsg( "아이템 슬롯 에러" );
		return;
	}

	ExtraItemList FindExtraItemList;
	pExtraItem->FindAllItem( iItemCode, FindExtraItemList );

	for( int i = 0; i < (int)FindExtraItemList.size(); ++i )
	{
		g_ChatMgr.SetSystemMsg( "%d, %d", i, FindExtraItemList[i].m_iIndex );
	}
}

void ChatMacroMgr::OnMacroItemPowerUpAdd( char *szMacro )
{
	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 1 )
	{
		g_ChatMgr.SetSystemMsg( "값 1개의 필요(아이템슬롯) - ex)1" );
		return;
	}

	int iSlotIndex	= atoi( kString[0].c_str() );

	ioHashString szError;
	if( g_PowerUpManager.CheckEnableItemPowerUp( iSlotIndex, szError ) )
	{
		//CheckEnableCharPowerUp함수에서 유효한 값인지 체크됨		
		ioEtcItemSoulStone* pEtcItem = dynamic_cast<ioEtcItemSoulStone*>( g_EtcItemMgr.FindEtcItem( g_PowerUpManager.GetMtrlEtcItemCode() ) );
		if( pEtcItem )
			pEtcItem->OnUseGradeAdjustment( iSlotIndex, PUTT_ITEM );
	}
	else
	{
		g_ChatMgr.SetSystemMsg( "%s", szError );
	}
}

void ChatMacroMgr::OnMacroCannotOpenDDSFile( char *szMacro )
{
	enum { _END_PATH = 5 };
	char szProgramPath[_MAX_PATH] = "";			// exe 실행 경로
	char szResourcePath[_MAX_PATH] = "";		//리소스 위치(exe 경로 + /resource/texture/ )
	char szFileSearchPath[_MAX_PATH] = "";		//리소스 경로 (리소스 위치 + *.*)
	char szFileEndName[_END_PATH] = "";			//file의 끝이름 (dds파일인지 찾기 위해)
	char szResourceFullPath[_MAX_PATH] = "";	//리소스의 경로 + 파일 이름
	char szFileName[_MAX_PATH] = "";			// file의 이름( 확장명 제외한 )

	GetCurrentDirectory( _MAX_PATH, szProgramPath);
	SafeSprintf( szResourcePath, sizeof( szResourcePath ), "%s%s", szProgramPath,"/resource/texture/" );
	SafeSprintf( szFileSearchPath, sizeof( szFileSearchPath ), "%s%s", szProgramPath,"/resource/texture/*.*" );

	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile( szFileSearchPath ,&data );
	if( h==INVALID_HANDLE_VALUE )
		return;
	do
	{
		ZeroMemory( szFileName, sizeof(szFileName) );
		ZeroMemory( szFileEndName, sizeof(szFileEndName) );
		ZeroMemory( szResourceFullPath, sizeof(szResourceFullPath) );
		SafeSprintf( szResourceFullPath, sizeof( szResourceFullPath ), "%s%s", szResourcePath,data.cFileName );
		int iEndName = 0;
		bool bEnd = false;
		for( int i = 0; i < lstrlen( data.cFileName ); i++ )
		{
			if ( data.cFileName[i] == '.' )
				bEnd = true;
			if ( bEnd )
			{
				szFileEndName[iEndName] = data.cFileName[i];
				iEndName++;
			}
			else
				szFileName[i] = data.cFileName[i];
		}
		if( _strnicmp( szFileEndName, ".dds", 4 ) == 0 )
		{
			D3DPOOL ePool = D3DPOOL_MANAGED;
			IDirect3DTexture9 *pSrcTex = NULL;
			HRESULT hr = D3DXCreateTextureFromFileEx( g_TextureMgr.GetDevice(), szResourceFullPath, 0, 0, 0, 0, D3DFMT_UNKNOWN, ePool, D3DX_DEFAULT, 0, 0, NULL, NULL, &pSrcTex );
			if( FAILED(hr) )
			{
				SafeSprintf( szResourceFullPath, sizeof( szResourceFullPath ), "%s%s%s", szResourcePath, szFileName, szFileEndName );
				remove(szResourceFullPath);
				LOG.PrintTimeAndLog( 0, "DDS Load Fail & Delete File : %s%s",szFileName, szFileEndName);
				SafeSprintf( szResourceFullPath, sizeof( szResourceFullPath ), "%s%s%s", szResourcePath, szFileName, ".lsc" );
				remove(szResourceFullPath);
				LOG.PrintTimeAndLog( 0, "DDS Load Fail & Delete File : %s%s",szFileName, ".lsc");
			}

			SAFERELEASE( pSrcTex );
			
		}	
			
	} while(FindNextFile(h,&data));
	FindClose(h);
}

void ChatMacroMgr::OnMacroCharInfoWndShow( char *szMacro )
{
#ifndef SHIPPING

	ioPlayStage* pStage = g_GUIMgr.GetPlayStage();
	if( pStage == NULL )
		return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.empty() )
	{
		CharGameInfoListWnd* pWnd = dynamic_cast<CharGameInfoListWnd*>( g_GUIMgr.FindWnd( CHAR_INFO_LIST_WND ) );
		if( pWnd )
			pWnd->HideWnd();
	}
	else
	{
		CharGameInfoListWnd* pWnd = dynamic_cast<CharGameInfoListWnd*>( g_GUIMgr.FindWnd( CHAR_INFO_LIST_WND ) );
		if( pWnd )
		{
			ioBaseChar* pChar = pStage->GetBaseChar(  kString[0].c_str()  );
			pWnd->SetStateInfoChar( pChar );
			pWnd->ShowWnd();
		}
	}
#endif
}

void ChatMacroMgr::OnMacroRollBookWndShow( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) )
		return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 2 )  
	{
		g_ChatMgr.SetSystemMsg( "값 2개 필요. /누적출석부보자 번호.위치" );
		return;
	}

	int iIndex = atoi( kString[0].c_str() );
	int iPosition = atoi( kString[1].c_str() );

	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );

	RollBookWnd* pWnd = dynamic_cast<RollBookWnd*>( g_GUIMgr.FindWnd( ROLLBOOK_WND ) );
	if ( pWnd )
		g_GUIMgr.DeleteWnd( ROLLBOOK_WND );
	RollBookWnd *pRollBookWnd = dynamic_cast<RollBookWnd*>( g_GUIMgr.AddWnd( "XML/rollbookwnd.xml" ) );
	if( pRollBookWnd )
	{
		pRollBookWnd->SetPlayStage( m_pPlayStage );
		pRollBookWnd->SettingRollBookByPacket( ServerTime.GetTime(), ROLLBOOK_ATTEND_OK, iIndex, iPosition );
	}
}

void ChatMacroMgr::OnMacroRollBookProgress( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) )	return;

	char szProgressCount[MAX_PATH];
	GetKeyword( szMacro, szProgressCount, sizeof(szProgressCount) );
	int iProgressCount = atoi( szProgressCount );	

	if( iProgressCount <= 0 && iProgressCount > 7 )
	{
		g_ChatMgr.SetSystemMsg( "값이 1~7사이의 값이여야 합니다." );
		return;
	}

	RollBookWnd* pWnd = dynamic_cast<RollBookWnd*>( g_GUIMgr.FindWnd( ROLLBOOK_WND ) );
	if( pWnd )
		pWnd->ShowWnd();
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_ROLLBOOK_PROGRESS;
	kPacket << iProgressCount;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroTimerChange( char *szMacro )
{
	//if( !g_App.IsMeDeveloper( true ) )	return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() != 1 )
	{
		g_ChatMgr.SetSystemMsg( "값 1개의 필요( 0, 1 )" );
		return;
	}

	bool bWin2x =  (atoi( kString[0].c_str() ) == 1) ? true : false;

	g_FrameTimer.SetWindowsModule( bWin2x );
}

void ChatMacroMgr::OnMacroHousingTest( char *szMacro )
{
#ifdef SHIPPING 
	if( !g_App.IsMeDeveloper( true ) )
		return;
#endif	

	if( !m_pPlayStage || m_pPlayStage->GetPlazaModeType() != PT_GUILD )
	{
		g_ChatMgr.SetSystemMsg( "길드 본부 내에서만 사용 가능합니다." );
	}
		
	LOG.PrintTimeAndLog(0, "----------------------------------------------------------------------------------------------" );
	LOG.PrintTimeAndLog(0, "Server to Client Block Matching Test Start" );

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_HOUSING_TEST;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroHousingCommand( char *szMacro )
{
#ifdef SHIPPING 
	if( !g_App.IsMeDeveloper( true ) )
		return;
#endif

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 1 )
	{
		g_ChatMgr.SetSystemMsg( "타입 필요" );
		return;
	}

	int nType = atoi( kString[0].c_str() );	
	switch( nType )
	{
	case 1:
		{
			if( kString.size() != 2 )
			{
				g_ChatMgr.SetSystemMsg( "2개의 값 필요" );
				return;
			}

			int nValue1	= atoi( kString[1].c_str() );
			switch( nValue1 )
			{
			case 1:
				g_BlockEventAgency.SendEvent( ioBlockEventListener::ET_BLOCK_COLL_AABB_VIEW, NULL );
				break;
			case 2:
				g_BlockEventAgency.SendEvent( ioBlockEventListener::ET_BLOCK_COLL_SPHERE_VIEW, NULL );
				break;
			case 3:
				g_BlockEventAgency.SendEvent( ioBlockEventListener::ET_BLOCK_INTERPLAY_COLL_VIEW, NULL );
				break;
			default:
				g_BlockEventAgency.SendEvent( ioBlockEventListener::ET_BLOCK_COLL_NONE_VIEW, NULL );
			}
		}
		break;
	case 2:
		{
			if( kString.size() != 5 )
			{
				g_ChatMgr.SetSystemMsg( "5개의 값 필요" );
				return;
			}
			
			ioBlockEventCreate* Event = new ioBlockEventCreate;			
			Event->nCode	= atoi( kString[1].c_str() );
			Event->nTileY	= atoi( kString[2].c_str() );
			Event->nTileX	= atoi( kString[3].c_str() );			
			Event->nTileZ	= atoi( kString[4].c_str() );
			g_BlockEventAgency.SendEvent( ioBlockEventListener::ET_BLOCK_CREATE, Event );
		}
		break;
	}
}

void ChatMacroMgr::OnMacroHousingBlockSave( char *szMacro )
{
#ifdef SHIPPING 
	if( !g_App.IsMeDeveloper( true ) )
		return;
#endif

	g_BlockManager.ExportBlockWorld();
}

void ChatMacroMgr::OnMacroComfortableLife( char *szMacro )
{
#ifndef SHIPPING 
	if( g_GUIMgr.IsShow( SINGLE_SETTING_WND ) )
		g_GUIMgr.HideWnd( SINGLE_SETTING_WND );
	else
		g_GUIMgr.ShowWnd( SINGLE_SETTING_WND );
#endif
}

void ChatMacroMgr::OnMacroTournamentNextState( char *szMacro )
{
#ifdef __DEV_QA__
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_TOURNAMENT_MACRO;
	kPacket << TM_NEXT_STATE;
	TCPNetwork::SendToServer( kPacket );
	g_ChatMgr.SetSystemMsg( "[대회 진행 매크로 전송]" );
#endif
}

void ChatMacroMgr::OnMacroTournamentResult( char *szMacro )
{
#ifdef __DEV_QA__
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_TOURNAMENT_MACRO;
	kPacket << TM_RESULT;
	TCPNetwork::SendToServer( kPacket );
	g_ChatMgr.SetSystemMsg( "[대회 보상 처리 매크로 전송]" );
#endif
}

void ChatMacroMgr::OnMacroOpenPopup( char* szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;
	char szNumber[MAX_PATH] = "";
	GetKeyword( szMacro, szNumber, sizeof(szNumber) );
	DWORD dwPopupIndex = atoi( szNumber );
	if( dwPopupIndex == 0 )
		return;

	PopupStoreWnd* pPopupStoreWnd = dynamic_cast<PopupStoreWnd*>( g_GUIMgr.FindWnd( POPUP_STORE_WND ) );
	if( !pPopupStoreWnd )
		pPopupStoreWnd = dynamic_cast<PopupStoreWnd*>( g_GUIMgr.AddWnd( "xml/popupstore.xml" ) );

	if( !pPopupStoreWnd )
	{
		LOG.PrintTimeAndLog( 0, "[err] PopupStore AddWnd Fail." );
		return;
	}

	if( pPopupStoreWnd )
		pPopupStoreWnd->OpenPopup( dwPopupIndex );
}

void ChatMacroMgr::OnMacroFPS( char *szMacro )
{
	if( g_App.IsFPSRender() )
		g_App.SetFPSRender( false );
	else
		g_App.SetFPSRender( true );
}

void ChatMacroMgr::OnMacroFpsLagCheck( char *szMacro )
{
	if( !Help::CheckEnableTimeGapUser(1) )	
		return;
	
	if( g_RequestLagMgr.IsProcess() )
		g_RequestLagMgr.SetProcess( false );
	else
		g_RequestLagMgr.SetProcess( true );
}

void ChatMacroMgr::OnMacroCustomDDSFromJPG( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif

	StringVector kString = ioStringConverter::Split( szMacro, " " );
	if( kString.size() < 1 ) return;

	bool bTexCreate = false;
	ioTexture *pDDSTex = g_TextureMgr.GetTextureByName( kString[0].c_str() );
	if( !pDDSTex )
	{
		bTexCreate = true;
		pDDSTex = g_TextureMgr.CreateTexture( kString[0].c_str(), D3DUSAGE_DYNAMIC, D3DX_DEFAULT, TQ_HIGH );
	}

	if( !pDDSTex )
	{
		g_ChatMgr.SetSystemMsg( STR(1), kString[0].c_str() );
		return;
	}

	char szPath[MAX_PATH] = "";
	if( kString.size() >= 2 )
		sprintf( szPath, "%s", kString[1].c_str() );
	else	
	{	// 경로를 지정하지 않았으면 알아서 저장 폴더에 저장
		char szName[MAX_PATH] = "";
		_splitpath( kString[0].c_str(), NULL, NULL, szName, NULL );
		sprintf( szPath, "SAVE\\%s.jpg", szName );
	}

	if( !Help::CustomTextureToJPG( pDDSTex, szPath ) )
		g_ChatMgr.SetSystemMsg( STR(2), szPath );
	else
		g_ChatMgr.SetSystemMsg( STR(3), szPath );

	if( bTexCreate )
	{
		g_TextureMgr.DestroyByName( kString[0].c_str(), true );
	}
}

void ChatMacroMgr::OnMacroCustomJPGMixDDSFromDDS( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif

	StringVector kString = ioStringConverter::Split( szMacro, " " );
	if( kString.size() < 2 ) return;

	ioHashString kDDS = kString[0].c_str();
	ioHashString kJPG = kString[1].c_str();
	ioTexture *pDDSTex = g_TextureMgr.CreateTexture( kDDS, D3DUSAGE_DYNAMIC, D3DX_DEFAULT, TQ_HIGH, false, false, true );
	if( !pDDSTex )
	{
		g_ChatMgr.SetSystemMsg( STR(1), kDDS.c_str() );
		return;
	}

	ioTexture *pJPGTex = g_TextureMgr.CreateTexture( kJPG, D3DUSAGE_DYNAMIC, D3DX_DEFAULT, TQ_HIGH, false, false, true );
	if( !pJPGTex )
	{
		g_TextureMgr.DestroyByName( kDDS, true );
		g_ChatMgr.SetSystemMsg( STR(2), kJPG.c_str() );
		return;
	}

	char kSavePath[MAX_PATH] = "";
	char szName[MAX_PATH] = "";
	_splitpath( kString[0].c_str(), NULL, NULL, szName, NULL );
	sprintf( kSavePath, "SAVE\\%s_%d.dds", szName, kDDS.GetHashCode() );

	// DXT1은 그냥 저장
	if( pDDSTex->GetFormat() != D3DFMT_DXT5 )
	{
		if( !Help::CustomTextureToDDS( pJPGTex, kSavePath ) )
			g_ChatMgr.SetSystemMsg( STR(3), kSavePath );
		else
			g_ChatMgr.SetSystemMsg( STR(4), kSavePath );
	}
	else
	{
		BYTE *pRGBA = new BYTE[pDDSTex->GetWidth() * pDDSTex->GetHeight() * 4]; 
		if( !g_TextureMgr.ExtractDDSColorValue( pDDSTex, pRGBA ) )
		{
			SAFEDELETEARRAY( pRGBA );
			g_TextureMgr.DestroyByName( kDDS, true );
			g_TextureMgr.DestroyByName( kJPG, true );
			return;
		}

		if( !g_TextureMgr.MixJPGAlphaValue( pJPGTex, pRGBA, pDDSTex->GetWidth(), pDDSTex->GetHeight() ) )
		{
			SAFEDELETEARRAY( pRGBA );
			g_TextureMgr.DestroyByName( kDDS, true );
			g_TextureMgr.DestroyByName( kJPG, true );
			return;
		}

		if( !g_TextureMgr.ColorCompressDDS( pDDSTex, pRGBA ) )
		{
			SAFEDELETEARRAY( pRGBA );
			g_TextureMgr.DestroyByName( kDDS, true );
			g_TextureMgr.DestroyByName( kJPG, true );
			return;
		}

		if( !Help::CustomTextureToDDS( pDDSTex, kSavePath ) )
			g_ChatMgr.SetSystemMsg( STR(5), kSavePath );
		else
			g_ChatMgr.SetSystemMsg( STR(6), kSavePath );

		SAFEDELETEARRAY( pRGBA );
	}

	g_TextureMgr.DestroyByName( kDDS, true );
	g_TextureMgr.DestroyByName( kJPG, true );
}

void ChatMacroMgr::OnMacroCustomDDSFromLSC( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif

	if( Help::CompressLSCFile( szMacro ) )
	{
		g_ChatMgr.SetSystemMsg( STR(1), szMacro );		
	}
	else
	{
		g_ChatMgr.SetSystemMsg( STR(2), szMacro );
	}
}

void ChatMacroMgr::OnMacroCustomLSCFromDDS( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif

	ioHashString kFilePath = szMacro;
	int iNameLen = kFilePath.Length();
	if( iNameLen < 4 )
	{
		g_ChatMgr.SetSystemMsg( STR(1), kFilePath.c_str() );	
		return;
	}

	HANDLE hFile = ::CreateFile( kFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );

	if( INVALID_HANDLE_VALUE == hFile )
	{
		g_ChatMgr.SetSystemMsg( STR(2), kFilePath.c_str() );	
		return;
	}

	DWORD dwFileSize = ::GetFileSize( hFile, NULL );
	if( INVALID_FILE_SIZE == dwFileSize )
	{
		::CloseHandle( hFile );
		g_ChatMgr.SetSystemMsg( STR(3), kFilePath.c_str() );	
		return;
	}

	unsigned char *pFileBuf = new unsigned char[dwFileSize];
	BOOL  bRet;
	DWORD dwReadedSize = 0;
	bRet = ::ReadFile( hFile, pFileBuf, dwFileSize, &dwReadedSize, NULL );
	if( FALSE == bRet )
	{
		::CloseHandle( hFile );
		SAFEDELETEARRAY(pFileBuf);
		g_ChatMgr.SetSystemMsg( STR(4), kFilePath.c_str() );	
		return;
	}
	::CloseHandle( hFile );

	char szSrc[MAX_PATH] = "";
	sprintf( szSrc, "%s", kFilePath.c_str() );
	szSrc[iNameLen - 3] = 'd';
	szSrc[iNameLen - 2] = 'd';
	szSrc[iNameLen - 1] = 's';

	hFile = ::CreateFile( szSrc, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( INVALID_HANDLE_VALUE == hFile )
	{
		SAFEDELETEARRAY(pFileBuf);
		g_ChatMgr.SetSystemMsg( STR(5), kFilePath.c_str() );	
		return;
	}

	if( !Help::DecompressLSCMemory( pFileBuf, dwFileSize ) )
	{
		::CloseHandle( hFile );
		SAFEDELETEARRAY(pFileBuf);
		g_ChatMgr.SetSystemMsg( STR(6), kFilePath.c_str() );	
		return;
	}

	DWORD dwWritedSize = 0;
	bRet = ::WriteFile( hFile, pFileBuf, dwFileSize, &dwWritedSize, NULL );
	if( FALSE == bRet )
	{
		::CloseHandle( hFile );
		SAFEDELETEARRAY(pFileBuf);
		g_ChatMgr.SetSystemMsg( STR(7), kFilePath.c_str() );	
		return;
	}
	::CloseHandle( hFile );
	SAFEDELETEARRAY(pFileBuf);

	g_ChatMgr.SetSystemMsg( STR(8), kFilePath.c_str() );	
}

void ChatMacroMgr::OnMacroCustomEntityTexture( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif
	if( !m_pPlayStage ) return;

	ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
	if( !pOwnerChar ) return;

	ioEntityGroup *pOwnerGrp = pOwnerChar->GetGroup();
	if( !pOwnerGrp ) return;

	CustomTextureList kTextureList;
	ioHashString kEntity = szMacro;
	if( kEntity == STR(1) )
	{
		ioWeaponItem *pItem = pOwnerChar->GetWeapon();
		if( pItem )
		{
			ioEntity *pEntity = pOwnerGrp->GetEntity( pItem->GetEquipMesh(pOwnerChar) );
			if( pEntity )
			{
				Help::CustomEntityTextureNameCnt( pEntity, 0, kTextureList );
			}
		}
	}
	else if( kEntity == STR(2) )
	{
		ioArmorItem *pItem = pOwnerChar->GetArmor();
		if( pItem )
		{
			// 몸
			ioEntity *pEntity = pOwnerGrp->GetEntity( pItem->GetBodyMesh(pOwnerChar) );
			if( pEntity )
			{
				Help::CustomEntityTextureNameCnt( pEntity, 0, kTextureList );
			}
			
			// 손
			pEntity = pOwnerGrp->GetEntity( pItem->GetHandMesh(pOwnerChar) );
			if( pEntity )
			{
				Help::CustomEntityTextureNameCnt( pEntity, 0, kTextureList );
			}
		}		
	}
	else if( kEntity == STR(3) )
	{
		ioHelmetItem *pItem = pOwnerChar->GetHelmet();
		if( pItem )
		{
			ioEntity *pEntity = pOwnerGrp->GetEntity( pItem->GetEquipMesh(pOwnerChar) );
			if( pEntity )
			{
				Help::CustomEntityTextureNameCnt( pEntity, 0, kTextureList );
			}
		}
	}
	else if( kEntity == STR(4) )
	{
		ioCloakItem *pItem = pOwnerChar->GetCloak();
		if( pItem )
		{
			ioEntity *pEntity = pOwnerGrp->GetEntity( pItem->GetEquipMesh(pOwnerChar) );
			if( pEntity )
			{
				Help::CustomEntityTextureNameCnt( pEntity, 0, kTextureList );
			}
		}
	}

	g_ChatMgr.SetSystemMsg( STR(5), (int)kTextureList.size() );
	
	DWORDVec vColorTable;
	vColorTable.push_back( (DWORD)TCT_DEFAULT_RED );
	vColorTable.push_back( (DWORD)TCT_DEFAULT_BLUE );
	vColorTable.push_back( (DWORD)TCT_DEFAULT_DARKGRAY );
	vColorTable.push_back( (DWORD)TCT_DEFAULT_ORANGE );
	vColorTable.push_back( (DWORD)TCT_DEFAULT_GREEN );
	vColorTable.push_back( (DWORD)TCT_DEFAULT_GOLD );
	vColorTable.push_back( (DWORD)TCT_DEFAULT_YELLOW );
	
	int iMaxColor = (int)vColorTable.size();
	for(int i = 0;i < (int)kTextureList.size();i++)
	{
		CustomTexture &rkData = kTextureList[i];
		ioTexture *pTexture = g_TextureMgr.GetTextureByName( rkData.m_szTextureName );
		if( !pTexture ) continue;

		DWORD dwSrcColor= vColorTable[i % iMaxColor];
		DWORD dwRed, dwGreen, dwBlue;
		dwRed	= ( dwSrcColor & 0x00ff0000 ) >> 16;
		dwGreen = ( dwSrcColor & 0x0000ff00 ) >> 8;
		dwBlue  = ( dwSrcColor & 0x000000ff );
		g_ChatMgr.SetSystemMsg( STR(6), rkData.m_szTextureName.c_str(), dwRed, dwGreen, dwBlue );
		
		BYTE *pRGBA = new BYTE[pTexture->GetWidth() * pTexture->GetHeight() * 4]; 
		if( !g_TextureMgr.ExtractDDSColorValue( pTexture, pRGBA ) )
		{
			SAFEDELETEARRAY( pRGBA );
			continue;
		}
        
		// 컬러 
		int iBitCnt = 0;		
		for( int iY=0 ; iY<pTexture->GetHeight() ; iY++ )
		{
			for( int iX=0 ; iX<pTexture->GetWidth() ; iX++ )
			{
				pRGBA[iBitCnt]   = dwRed;
				pRGBA[iBitCnt+1] = dwGreen;
				pRGBA[iBitCnt+2] = dwBlue;
				iBitCnt+=4;
			}
		}

		if( !g_TextureMgr.ColorCompressDDS( pTexture, pRGBA ) )
		{
			SAFEDELETEARRAY( pRGBA );
			continue;
		}

		SAFEDELETEARRAY( pRGBA );
	}
}

void ChatMacroMgr::OnMacroShowWnd( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif
	g_GUIMgr.ShowWnd( atoi( szMacro ), true );
}

void ChatMacroMgr::OnMacroPresent( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 3 )  
	{
		g_ChatMgr.SetSystemMsg( STR(1) );
		return;
	}

	short iPresentType   = atoi( kString[0].c_str() );
	int   iPresentValue1 = atoi( kString[1].c_str() );
	int   iPresentValue2 = atoi( kString[2].c_str() );

	SP2Packet kPacket( CTPK_PRESENT_TEST_SEND );
	kPacket << iPresentType << iPresentValue1 << iPresentValue2;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroBestFriendApplication( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif
}

void ChatMacroMgr::OnMacroBestFriendAgree( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif   
}

void ChatMacroMgr::OnMacroBestFriendRefuse( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif
}

void ChatMacroMgr::OnMacroBestFriendClear( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif    
}

void ChatMacroMgr::OnMacroMultiMapChange( char *szMacro )
{
#ifdef SHIPPING 
	return;
#endif    

	if( m_pPlayStage )
	{
		m_pPlayStage->ChangeMultiMapIndex( atoi( szMacro ) );
	}
}

void ChatMacroMgr::OnMacroMacroNotify( char *szMacro )
{
	char szUserID[MAX_PATH] = "";
	GetKeyword( szMacro, szUserID, sizeof( szUserID ) );	
	ioHashString kUserID = szUserID;

	if( kUserID == g_MyInfo.GetPublicID() ) return;

	if( g_App.IsMeDeveloper( true ) )
	{
		if( m_pPlayStage )
		{
			if( m_pPlayStage->GetBaseChar( kUserID ) == NULL )
			{
				g_ChatMgr.SetSystemMsg( STR(1) );		
				return;
			}
		}
	}
	else
	{
		if( !g_BattleRoomMgr.IsBattleRoom() || !g_BattleRoomMgr.IsHaveUser( kUserID ) )
		{
			g_ChatMgr.SetSystemMsg( STR(2) );		
			return;
		}

		if( !g_BattleRoomMgr.IsBattleModePlaying() )
		{
			g_ChatMgr.SetSystemMsg( STR(3) );		
			return;
		}
	}

	DWORD dwNotifyTime = g_MyInfo.GetMacroNotifyTime();
	if( dwNotifyTime == 0 || FRAMEGETTIME() - dwNotifyTime > 1800000 )
	{
		SP2Packet kPacket( CTPK_MACRO_NOTIFY );
		kPacket << szUserID;
		TCPNetwork::SendToServer( kPacket );

		// 로그 서버에 전송
		char szLog[MAX_PATH] = "";
		sprintf( szLog, "Macro Notify : %s - > %s", g_MyInfo.GetPublicID().c_str(), szUserID );
		SP2Packet kPacket2( LUPK_LOG );
		kPacket2 << "ClientError";  // 로그 파일 타입
		kPacket2 << szLog;
		kPacket2 << 317;  // 오류번호:317
		kPacket2 << false; // write db
		P2PNetwork::SendLog(kPacket2);
		//
		g_MyInfo.SetMacroNotifyTime();

		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, szUserID );
		kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(4) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(5) );

		kPrinter[2].SetTextStyle( TS_NORMAL );
		kPrinter[2].SetBkColor( 0, 0, 0 );
		kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(6) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	}
	else
	{
		g_ChatMgr.SetSystemMsg( STR(7) );	
	}	
}

void ChatMacroMgr::OnMacroGrowthLevelup( char *szMacro )
{
#ifndef SHIPPING
	if(Help::IsMonsterDungeonMode(m_pPlayStage->GetModeType()))
	{
		ioTowerDefMode *pMode = ToTowerDefMode( m_pPlayStage->GetPlayMode() );
		if( pMode)
			pMode->IncreaseLevel_150();
	}
#endif
};

void ChatMacroMgr::OnMacroExtraItemGrowthCatalyst( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	char szNumber[MAX_PATH] = "";
	GetKeyword( szMacro, szNumber, sizeof(szNumber) );
	DWORD dwItemCode = atoi( szNumber );
	if( dwItemCode == 0 )
		return;

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_EXTRAITEM_GROWTH_CATALYST_INFO << dwItemCode;
	TCPNetwork::SendToServer( kPacket );
}

bool ChatMacroMgr::SetCreateStructMacro( int iMacroType, char *szMacro )
{
	if( iMacroType != ChatMacro::NONE_MACRO ) return false;

	ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
	if( !pOwnerChar )
		return false;

	char strCommand[MAX_EDIT_TEXT+1];
	memset( strCommand, 0, sizeof( strCommand ) );

	int key_word = 0;
	while( *szMacro != NULL )
	{
		strCommand[key_word++] = *szMacro++;
		if( *szMacro == ' ' )
			break;
	}

	szMacro++;	// 공백 넘기기

	// command 체크
	ioHashString szCommand = strCommand;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItemBlockMacro( szCommand.c_str() );
	if( !pEtcItem )
		return false;

	ioEtcItemBlock *pEtcBlock = dynamic_cast<ioEtcItemBlock *>(pEtcItem);
	if( !pEtcBlock ) return false;

	DWORD dwItemCode = pEtcBlock->GetType();
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem ) return false;

	// 권한 아이템 보유여부 체크
	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( P2PNetwork::IsNetworkPlaying() && !pUserEtcItem->GetEtcItem( dwItemCode, kItemSlot ) )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(1), pEtcItem->GetName().c_str() );
		g_ChatMgr.SetSystemMsg( szMsg );
		return true;
	}

	if( m_pPlayStage->GetModeType() == MT_TRAINING && m_pPlayStage->GetBlockWorld() != NULL )
	{
		g_ChatMgr.SetSystemMsg( "길드 본부 에서는 사용 할 수 없습니다." );
		return false;
	}

	// 광장/길드본부/훈련소/개인본부 체크
	bool bTraining = false;
	bool bMyRoom = false;
	if( m_pPlayStage->GetModeType() == MT_TRAINING )
		bTraining = true;

	if( m_pPlayStage->GetModeType() == MT_MYROOM && ioMyRoomMode::GetMyRoomType() == MRT_PRACTICE )
		bMyRoom = true;
	else if( m_pPlayStage->GetModeType() == MT_HEADQUARTERS )
		bMyRoom = true;
	else if( m_pPlayStage->GetModeType() == MT_HOUSE )
		bMyRoom = true;

	if( P2PNetwork::IsNetworkPlaying() && !bTraining && !bMyRoom )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(2) );
		g_ChatMgr.SetSystemMsg( szMsg );
		return true;
	}

	// 갯수제한 체크
	if( bTraining && !g_PushStructListMgr.CheckMaxStructCntForRoom() )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(3) );
		g_ChatMgr.SetSystemMsg( szMsg );
		return true;
	}
	else if( bMyRoom && !g_PushStructListMgr.CheckMaxStructCntForMyRoom() )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(4) );
		g_ChatMgr.SetSystemMsg( szMsg );
		return true;
	}

	if( !g_PushStructListMgr.CheckMaxStructCntForEtcCode( pOwnerChar, dwItemCode ) )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(5), pEtcItem->GetName().c_str(),
																				   g_PushStructListMgr.GetBlockEtcMaxCnt() );
		g_ChatMgr.SetSystemMsg( szMsg );
		return true;
	}

	// 기간 체크
	if( kItemSlot.m_iValue1 != 0 && kItemSlot.m_iValue2 != 0 )
	{
		time_t kLimitTime = DateHelp::ConvertSecondTime( kItemSlot.GetYear(), kItemSlot.GetMonth(), kItemSlot.GetDay(), kItemSlot.GetHour(), kItemSlot.GetMinute(), 0 );
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

		int iGapSec = kLimitTime - kCurServerTime;
		if( iGapSec < 3 )
		{
			char szMsg[MAX_PATH*2]=""; 
			SafeSprintf( szMsg, sizeof( szMsg ), STR(6), pEtcItem->GetName().c_str() );
			g_ChatMgr.SetSystemMsg( szMsg );
			return true;
		}
	}

	ioObjectItem *pItem = pOwnerChar->GetObject();
	ioPoweredObjectItem *pPoweredObjectItem = ToPoweredObjectItem(pItem);
	// 캐릭터 상태체크
	if( (pOwnerChar->GetState() != CS_DELAY && pOwnerChar->GetState() != CS_SPECIAL_MOTION) ||
		pOwnerChar->IsCatchMode() || pOwnerChar->IsPrisonerMode() || pOwnerChar->IsObejctEquipState() ||
		pOwnerChar->HasBuff( BT_CHANGE_MESH ) || pOwnerChar->HasBuff( BT_CHANGE_SKIN_COLOR ) ||
		pPoweredObjectItem )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(7) );
		g_ChatMgr.SetSystemMsg( szMsg );
		return true;
	}

	pOwnerChar->SetCreateStructState( pEtcBlock );
	return true;
}

bool ChatMacroMgr::CheckCreateStructMacro( char *szMacro )
{
	char strCommand[MAX_EDIT_TEXT+1];
	memset( strCommand, 0, sizeof( strCommand ) );

	int key_word = 0;
	while( *szMacro != NULL )
	{
		strCommand[key_word++] = *szMacro++;
		if( *szMacro == ' ' )
			break;
	}

	szMacro++;	// 공백 넘기기

	// command 체크
	ioHashString szCommand = strCommand;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItemBlockMacro( szCommand.c_str() );
	if( !pEtcItem ) return false;

	ioEtcItemBlock *pEtcBlock = dynamic_cast<ioEtcItemBlock *>(pEtcItem);
	if( !pEtcBlock ) return false;

	DWORD dwItemCode = pEtcBlock->GetType();
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem ) return false;

	// 권한 아이템 보유여부 체크
	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( P2PNetwork::IsNetworkPlaying() && !pUserEtcItem->GetEtcItem( dwItemCode, kItemSlot ) )
		return false;

	return true;
}

// 일반 가챠 패키지 전부 보내기		JCLEE 140718
#if defined( SRC_OVERSEAS )
void ChatMacroMgr::OnMacroGashaponAllPackage( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) 
		return;

	char szNumber[MAX_PATH] = "";
	GetKeyword( szMacro, szNumber, sizeof(szNumber) );
	DWORD dwItemCode = atoi( szNumber );
	if( dwItemCode == 0 )
		return;

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_GASHAPON_GET_ALL << dwItemCode;
	TCPNetwork::SendToServer( kPacket );
}
#endif

void ChatMacroMgr::GetFilesFromDirectory( const char *Path, ioHashStringVec &szFileList )
{
	char szBuf[MAX_PATH];
	ioHashString szFilePath;
	WIN32_FIND_DATA file_data;

	wsprintf_e( szBuf, "%s\\*.*", Path );

	HANDLE search_handle = FindFirstFile( szBuf, &file_data );
	if( search_handle != INVALID_HANDLE_VALUE )
	{
		do 
		{
			if( file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if( file_data.cFileName[0] != '.' )
				{
					wsprintf_e( szBuf, "%s/%s", Path, file_data.cFileName );
					szFilePath = szBuf;

					szFileList.push_back( szBuf );
					GetFilesFromDirectory( szFilePath.c_str(), szFileList );
				}
			} 
			else 
			{
				wsprintf_e( szBuf, "%s/%s", Path, file_data.cFileName );
				szFilePath = szBuf;

				szFileList.push_back( szBuf );
			}
		}while( FindNextFile( search_handle, &file_data ) );

		FindClose(search_handle);
	}
}

void ChatMacroMgr::LSCAllDecypt( char *szMacro )
{
	if( !g_App.IsMeDeveloper( true ) ) return;

	ioHashStringVec szFileList;
	GetFilesFromDirectory( szMacro, szFileList );

	for ( int i=0; i<(int)szFileList.size(); ++i )
	{
		char szBuf[MAX_PATH];
		wsprintf( szBuf, "%s", szFileList[i].c_str() );
		OnMacroCustomLSCFromDDS( szBuf );
	}
}

void ChatMacroMgr::OnMacroSetDateMission( char *szMacro )
{
	if( !g_TCPSocketMgr.IsTestZone() )	return;

	if( !g_App.IsMeDeveloper( true ) ) return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 3 )
	{
		g_ChatMgr.SetSystemMsg( "미션 값 부족 : /미션 DateType.Day.Request Min" );
		return;
	}

	int		  iDateType		= atoi( kString[0].c_str() );		//iDateType 0(일),1(주),2(월)
	int		  iDate			= atoi( kString[1].c_str() );		//현재 미션 날짜 + iDate(일) 미션 요청
	int		  iChangeTime	= atoi( kString[2].c_str() );		//iChangeTime 후에 다음미션으로 바뀔것인가.
	if ( iChangeTime <= 0 )
	{
		g_ChatMgr.SetSystemMsg( "요청가능한 시간이 아닙니다. : /미션 DateType.Day.Request Min" );
		return;
	}

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_MISSION_SET_DATE;
	kPacket << 2;
	kPacket << iDateType;
	kPacket << iDate;
	kPacket << iChangeTime;
	TCPNetwork::SendToServer( kPacket );
	//g_MissionManager.SetMacroMinute( iChangeTime );
}

void ChatMacroMgr::OnMacroSetNextDateMission( char *szMacro )
{
	if( !g_TCPSocketMgr.IsTestZone() )	return;
	if( !g_App.IsMeDeveloper( true ) ) return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 2 )
	{
		g_ChatMgr.SetSystemMsg( "미션 값 부족 : /다음미션 DateType.Request Min" );
		return;
	}

	int		  iDateType		= atoi( kString[0].c_str() );		//iDateType 0(일),1(주),2(월)
	int		  iChangeTime	= atoi( kString[1].c_str() );		//iChangeTime 후에 다음미션으로 바뀔것인가.
	if ( iChangeTime <= 0 )
	{
		g_ChatMgr.SetSystemMsg( "요청가능한 시간이 아닙니다. : /다음미션 DateType.Request Min" );
		return;
	}

	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_MISSION_SET_DATE;
	kPacket << 1;
	kPacket << iDateType;
	kPacket << iChangeTime;
	TCPNetwork::SendToServer( kPacket );
//	g_MissionManager.SetMacroMinute( iChangeTime );
}

void ChatMacroMgr::OnMacroSetMissionValue( char *szMacro )
{
	if( !g_TCPSocketMgr.IsTestZone() )	return;
	if( !g_App.IsMeDeveloper( true ) ) return;

	StringVector kString = ioStringConverter::Split( szMacro, "." );
	if( kString.size() < 2 )
	{
		g_ChatMgr.SetSystemMsg( "값 부족 : /미션갱신 missionCode.Value" );
		return;
	}

	int iMagicCode	= atoi( kString[0].c_str() );
	int	iValue		= atoi( kString[1].c_str() );

	const ioMission* const pMission = g_MissionManager.GetMissionToCode( iMagicCode );
	if ( !pMission )
	{
		g_ChatMgr.SetSystemMsg( "존재하지 않는 미션 코드입니다." );
		return;
	}

	int CheckValue = min( max(0, iValue ), pMission->GetMaxAchieveValue() );
	SP2Packet kPacket( CTPK_DEVELOPER_MACRO );
	kPacket << DEVELOPER_MISSION_SET_VALUE;
	kPacket << iMagicCode;
	kPacket << CheckValue;
	TCPNetwork::SendToServer( kPacket );
}

void ChatMacroMgr::OnMacroPracticeCompleted( char* szMacro )
{
	if( !g_App.IsMeDeveloper(true) ) return;
	
	DWORD dwResult = atoi( szMacro );
	LOG.PrintTimeAndLog( 0, "result : %d", dwResult);
	if(dwResult > 3)
		return;
	g_App.OnMacroPracticeCompleted(dwResult);
}

bool ChatMacroMgr::ExceptMacro( int iMacroType )
{
	if( !COMPARE( iMacroType, ChatMacro::MACRO_HELP, ChatMacro::MAX_MACRO ) )
		return true;

	switch( iMacroType )
	{
	case ChatMacro::MACRO_HELP:
	case ChatMacro::MACRO_INVITE_CHANNEL:
	case ChatMacro::MACRO_NONE_UI:
	case ChatMacro::MACRO_USER_INFO:
	case ChatMacro::MACRO_CHAT_MODE:
	case ChatMacro::MACRO_GUILD_INFO:
	case ChatMacro::MACRO_TIME:
	case ChatMacro::MACRO_MOVIE_MODE:
	case ChatMacro::MACRO_WHOLE_CHAT_OFF:
	case ChatMacro::MACRO_WHOLE_CHAT_ON:
	case ChatMacro::MACRO_FPS_LOWER:
	case ChatMacro::MACRO_FPS_UPPER:
		return false;
	}

	return true;
}