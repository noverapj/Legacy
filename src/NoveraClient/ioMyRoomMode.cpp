

#include "stdafx.h"

#include "ioMyRoomMode.h"
#include "ioReservedTask.h"
#include "ioLadderTeamMgr.h"
#include "ioNpcDefine.h"
#include "ioShuffleRoomMgr.h"
#include "ItemDefine.h"

#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/LobbyMainWnd.h"
#include "GUI/EventWnd.h"
#include "GUI/SoldierPracticeWnd.h"

#include "GameStage/Mode/Practice/IoPracticeManager.h"

bool ioMyRoomMode::m_bBlockNotice = false;
MyRoomType ioMyRoomMode::m_eMyRoomType = MRT_LOBBY;

ioMyRoomMode::ioMyRoomMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{	
	m_ModeType = MT_MYROOM;
	m_qtRemoveExerciseRot = ioMath::QUAT_IDENTITY;
	m_vRemoveExercisePos  = -ioMath::UNIT_Z;
	m_bBuyExercise        = false;
	m_bOneMyChar          = false;
	m_iMyLobbyArray     = 0;
	m_dwCurMyLobbyAlarmTime = 0;
	m_dwMyLobbyAlarmTime    = 0;

	m_vCharRecordList.reserve( 100 );
}

ioMyRoomMode::~ioMyRoomMode()
{
	ClearMode();
}

void ioMyRoomMode::LoadCharacterPos()
{
	char fName[MAX_PATH];
	wsprintf_e( fName,"Save\\%s\\window.ini",g_MyInfo.GetPublicID().c_str() );

	m_vDefStartPos.assign( m_vStartPos.begin(), m_vStartPos.end() );
	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "character" );

	int iVersion = kLoader.LoadInt_e( "version", 0 );
	if( iVersion != CHARACTER_POS_VER ) 
		return;

	char szStartX[MAX_PATH], szStartZ[MAX_PATH];

	int iMaxStartPos = m_vStartPos.size();
	for( int i=0 ; i<iMaxStartPos; i++ )
	{
		sprintf_e( szStartX, "start%d_x", i+1 );
		sprintf_e( szStartZ, "start%d_z", i+1 );

		float fXPos = kLoader.LoadFloat( szStartX, -9999.0f );
		float fZPos = kLoader.LoadFloat( szStartZ, -9999.0f );

		if( fXPos != -9999.0f || fZPos != -9999.0f )
		{
			m_vStartPos[i].x = fXPos;
			m_vStartPos[i].z = fZPos;
		}
	}
}

void ioMyRoomMode::SaveCharacterPos()
{
	char fName[MAX_PATH];
	wsprintf_e( fName,"Save\\%s\\window.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName , false );
	kLoader.SetTitle_e( "character" );

	kLoader.SaveInt( "version", CHARACTER_POS_VER );

	char szStartX[MAX_PATH], szStartZ[MAX_PATH];
	
	int iMaxStartPos = m_vStartPos.size();
	for( int i=0 ; i<iMaxStartPos; i++ )
	{
		wsprintf_e(szStartX, "start%d_x", i+1);
		wsprintf_e(szStartZ, "start%d_z", i+1);

		kLoader.SaveFloat( szStartX, m_vStartPos[i].x );
		kLoader.SaveFloat( szStartZ, m_vStartPos[i].z );
	}
}

void ioMyRoomMode::LoadSound()
{
	if( GetMyRoomType() != MRT_PRACTICE )
		return;

	// 로비 사운드 플레이
	ioINILoader_e kSndLoader( "config/sp2_sound.ini" );
	kSndLoader.SetTitle_e( "sound_file_name" );

	char szBuf[MAX_PATH];
	kSndLoader.LoadString_e( "myroom_sound", "", szBuf, MAX_PATH );

	if( strcmp( szBuf, "" ) != 0 )
	{
		char szName[MAX_PATH];
		wsprintf( szName, "Resource/wave/%s", szBuf );

		g_SoundMgr.OpenOggStream( szName );
		g_SoundMgr.PlayStream( true );
	}
}

void ioMyRoomMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;
	
	// Init GUI	
	// Main UI
	switch( GetMyRoomType() )
	{
	case MRT_LOBBY:
		{
			g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );
			LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>(g_GUIMgr.AddWnd( "XML/LobbyMainWnd.xml" ));
			if( pLobbyMainWnd )
			{
				pLobbyMainWnd->SetPlayStage( m_pCreator );
				if(g_App.GetPracticeState())
				{
					LobbyWnd *pLobbyWnd = dynamic_cast<LobbyWnd*>(pLobbyMainWnd->FindChildWnd(1)); // lobbywnd
					if(pLobbyWnd)
						pLobbyWnd->SetTabID( LobbyWnd::ID_PRACTICE_BTN );
					g_App.SetPracticeState(false);

					SP2Packet kPacket( CTPK_PRACTICE_INDEXRANK );
					int iIndex = g_PracticeMgr.GetPreIndex();
					kPacket << g_PracticeMgr.GetPracticeMap(iIndex);
					kPacket << iIndex;
					if( TCPNetwork::CheckSendToServer( kPacket ) )
						TCPNetwork::MouseBusy( true );			
				}
				
			}

			g_GUIMgr.AddWnd( "XML/campenteralarmwnd.xml" );
			g_GUIMgr.AddWnd( "XML/campenterresultwnd.xml" );			
			g_GUIMgr.AddWnd( "XML/userblocktypealarm.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustomselectwnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustommakewnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustommakedownloadwnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustommakeuploadwnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustommakeconfirmwnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustomdetailviewwnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustomchangeconfirmwnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustommakecompletewnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustomdeletewnd.xml" );

			g_GUIMgr.AddWnd( "XML/itemcustomtrainselectwnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustomtrainmakewnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustomwantetcitemwnd.xml" );
			g_GUIMgr.AddWnd( "XML/itemcustomselecttooltip.xml" );

			g_GUIMgr.AddWnd( "XML/lobbylockproblemwnd.xml" );

			g_GUIMgr.AddWnd( "XML/tournamentdetailmainwnd.xml" );
			g_GUIMgr.AddWnd( "XML/tournamentruleagreeconfirmwnd.xml" );
			g_GUIMgr.AddWnd( "XML/tournamentteamcreatewnd.xml" );
			g_GUIMgr.AddWnd( "XML/tournamentteamcreatecomplete.xml" );
			g_GUIMgr.AddWnd( "XML/tournamentetcitemneedpopup.xml" );
			g_GUIMgr.AddWnd( "XML/tournamentetcitemusepopup.xml" );
			g_GUIMgr.AddWnd( "XML/tournamentroomlistwnd.xml" );

			g_GUIMgr.AddWnd( "XML/customtournamentcreatewnd.xml" );
			g_GUIMgr.AddWnd( "XML/customtournamentcalendarwnd.xml" );
			g_GUIMgr.AddWnd( "XML/customtournamentcreateresultwnd.xml" );
			g_GUIMgr.AddWnd( "XML/customtournamentnoticewnd.xml" );
			g_GUIMgr.AddWnd( "XML/customtournamentalarmrewardwnd.xml" );
			g_GUIMgr.AddWnd( "XML/customtournamentallocatewnd.xml" );
			g_GUIMgr.AddWnd( "XML/customtournamentallocateresultwnd.xml" );
			g_GUIMgr.AddWnd( "XML/customtournamentrewardselectwnd.xml" );
			g_GUIMgr.AddWnd( "XML/tournamentcoinneedpopup.xml" );

			g_GUIMgr.AddWnd( "XML/customtournamententrylistwnd.xml" );

			g_GUIMgr.AddWnd( "XML/relativegradebonuswnd.xml" );
			
			// UI코스튬 관련 (코스튬 스킨 xml 추가)
			g_GUIMgr.AddWnd( "XML/costumecustomchangeconfirmwnd.xml" );
			g_GUIMgr.AddWnd( "XML/costumecustomdeletewnd.xml" );
			g_GUIMgr.AddWnd( "XML/costumecustomdetailviewwnd.xml" );
			g_GUIMgr.AddWnd( "XML/costumecustommakecompletewnd.xml" );
			g_GUIMgr.AddWnd( "XML/costumecustommakeconfirmwnd.xml" );
			g_GUIMgr.AddWnd( "XML/costumecustommakewnd.xml" );
			g_GUIMgr.AddWnd( "XML/costumecustomselecttooltip.xml" );
			g_GUIMgr.AddWnd( "XML/costumecustomselectwnd.xml" );
			g_GUIMgr.AddWnd( "XML/practice/practicetooltip.xml" );
			m_pCreator->SetCameraModeFixedBlock( true );

			ShowBlockNotice();
		}
		break;
	case MRT_BATTLE:
		{
			g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );
			if( g_BattleRoomMgr.IsBattleRoom() )
				g_GUIMgr.ShowWnd( PARTY_SHAM_BATTLE_WND );
			m_pCreator->SetCameraModeFixedBlock( true );
		}
		break;
	case MRT_LADDER:
		{
			g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );
			if( g_LadderTeamMgr.IsLadderTeam() )
				g_GUIMgr.ShowWnd( LADDER_BATTLE_WND );
			m_pCreator->SetCameraModeFixedBlock( true );
		}
		break;
	case MRT_SHUFFLE:
		{
			g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );
			m_pCreator->SetCameraModeFixedBlock( true );
		}
		break;
	case MRT_RANKBATTLE:
		{
			g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );
			g_GUIMgr.ShowWnd( RANK_BATTLE_WND );
			m_pCreator->SetCameraModeFixedBlock( true );
		}
		break;
	default:
		{
			m_pCreator->SetCameraModeFixedBlock( false );

			PracticeHelpWnd *pPracticeHelp = dynamic_cast<PracticeHelpWnd *>( g_GUIMgr.AddWnd( "XML/practicehelpwnd.xml" ) );
			if( pPracticeHelp )
				pPracticeHelp->SetPlayStage( m_pCreator );

			SoldierPracticeSelectWnd *pPracticeSelect = dynamic_cast<SoldierPracticeSelectWnd *>(g_GUIMgr.AddWnd( "XML/soldierpracticeselectwnd.xml" ));
			if( pPracticeSelect )
				pPracticeSelect->SetPlayStage( m_pCreator );

			ExerciseSoldierEventWnd *pExerciseSoldierEventWnd = dynamic_cast<ExerciseSoldierEventWnd*> ( g_GUIMgr.AddWnd( "XML/exercisesoldiereventwnd.xml" ) ); 
			if( pExerciseSoldierEventWnd )
				pExerciseSoldierEventWnd->SetPlayStage( m_pCreator );
		}
		break;
	}
	g_GUIMgr.AddWnd( "XML/TutorialWnd.xml" );

	//Party
	g_GUIMgr.AddWnd( "XML/partycreatewnd.xml" );

	g_GUIMgr.AddWnd( "XML/plazaroominfownd.xml" );
	g_GUIMgr.AddWnd( "XML/plazapasswordwnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazainvitedwnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazacreatewnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazaownerwnd.xml" );

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsShowFirstIDWnd() )
		g_GUIMgr.AddWnd( "XML/firstidchangewnd.xml" );
	if( pLocal && pLocal->IsShowFirstSoldierSelectWnd() )
		g_GUIMgr.AddWnd( "XML/firstsoldierselectwnd.xml" );

	g_GUIMgr.HideWnd( MINIMAP_WND );
	// 연습 모드에서만 맵과 캐릭터를 로드한다.
	if( GetMyRoomType() != MRT_PRACTICE )
	{
		m_dwCurEventDuration = 0;
		return;
	}

	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadFuntionalObjects();
	LoadCharacterPos();
	LoadDisableSkillList();
	LoadStartBuffList();
	
	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	kLoader.SetTitle_e( "common_info" );
	m_vGaugeUpPos.x = kLoader.LoadFloat_e( "gauge_up_x", 0.0f );
	m_vGaugeUpPos.z = kLoader.LoadFloat_e( "gauge_up_z", 0.0f );
	m_vGaugeUpPos.y = GetMapHeight( m_vGaugeUpPos.x, m_vGaugeUpPos.z );;
    m_fGaugeUpDistance = kLoader.LoadFloat_e( "gauge_up_distance", FLOAT1000 );
	m_fGaugeUpRate = kLoader.LoadFloat_e( "gauge_up_rate", 2.0f );
	
	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );
	LoadModeCamera( kLoader );

	char szBuf[MAX_PATH];
	kLoader.SetTitle_e( "ko_recover" );
	m_fKORecoverRate = kLoader.LoadFloat_e( "ko_recover_rate", 0.0f );
	kLoader.LoadString_e( "ko_recover_effect", "", szBuf, MAX_PATH );
	m_KORecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_perfect_effect", "", szBuf, MAX_PATH );
	m_PerfectRecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_buff", "", szBuf, MAX_PATH );
	m_KOBuff = szBuf;

	kLoader.SetTitle_e( "channel_info" );
    m_dwMyLobbyAlarmTime = kLoader.LoadInt_e( "channel_alarm_time", 30000 );
	kLoader.LoadString_e( "channel_alarm_ment_start", "", szBuf, MAX_PATH );
	m_szMyLobbyAlarmMent_start = szBuf;
	kLoader.LoadString_e( "channel_alarm_ment_current", "", szBuf, MAX_PATH );
	m_szMyLobbyAlarmMent_current = szBuf;

	kLoader.SetTitle_e( "item_pos" );
	int iMaxItemPos = kLoader.LoadInt_e( "max_item_pos", 0 );
	char szItemX[MAX_PATH], szItemZ[MAX_PATH];
	m_vDropFieldItemPos.clear();
	m_vDropFieldItemPos.reserve( iMaxItemPos );
	for( int i=0 ; i<iMaxItemPos; i++ )
	{
		wsprintf_e(szItemX, "item%d_x", i+1);
		wsprintf_e(szItemZ, "item%d_z", i+1);

		D3DXVECTOR3 vItemPos;
		vItemPos.x  = kLoader.LoadFloat( szItemX, 0.0f );
		vItemPos.z  = kLoader.LoadFloat( szItemZ, 0.0f );
		vItemPos.y  = 0.0f;
		m_vDropFieldItemPos.push_back( vItemPos );
	}

	LoadGUIValues();
	LoadPushCursor();
	LoadContribute();

	bool bFirstCheck = false;
	m_dwCurEventDuration = 0;
	if( g_App.CheckFirstMyRoom() )
	{
		m_dwCurEventDuration = m_dwLoadEventDuration;
		bFirstCheck = true;
	}

	InitSingleLobby();

	if( !g_BattleRoomMgr.IsBattleRoom() )
	{
		if( bFirstCheck )
		{
			m_pCreator->SetModeEndSceneCamera( false );
			m_pCreator->StartModeStartEventCamera( true );
		}
	}

	if( g_MyInfo.GetExerciseCharCount() > 0 )
		m_bBuyExercise = true;

	g_ModeHelpMgr.InitModeHelp();

	g_GUIMgr.CheckShopSound( true );

	g_MyInfo.InitPraise();

	ClearGrowthLevel();
}

void ioMyRoomMode::InitSingleLobby()
{	
	int iCharCnt = GetRecordCharCnt();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( pChar )
		{
			m_pCreator->RemoveUser( pChar->GetCharName(), false );
		}
	}
	m_vCharRecordList.clear();

	LoadCharacters();
	LoadNPC();

	SetModeState( MS_READY );

	g_MyInfo.ClearCharDiedInfo();
	g_MyInfo.ClearCharDiedPenaltyInfo();
	g_MyInfo.ClearCharJoinedInfo();

	if( g_MyInfo.GetUserState() == 0 )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && pLocal->IsShowFirstIDWnd() )
		{
			char szTempPublicID[ID_NUM_PLUS_ONE]="";
			StringCbPrintf( szTempPublicID, sizeof( szTempPublicID ), "%s#", g_MyInfo.GetPrivateID() );

			if( g_MyInfo.GetPublicID() == szTempPublicID  )
				g_GUIMgr.ShowWnd( FIRST_IDCHANGE_WND );
			else
				g_MyInfo.SetFirstSolderBonus();
		}
		else if( pLocal && pLocal->IsShowFirstSoldierSelectWnd() )
		{
			g_GUIMgr.ShowWnd( FIRST_SOLDIER_SELECT_WND );
		}
		else
		{
			// 최초 용병 구입.
			g_MyInfo.SetFirstSolderBonus();
		}
	}
	else
	{		
		SP2Packet kPacket( CTPK_CHAR_LIMIT_CHECK );
		TCPNetwork::SendToServer( kPacket );
		g_App.SetMouseBusy( true );
	}

	SetCurRevivalTime( m_dwDefaultRevivalTime );

	g_GUIMgr.SetSkipUIByBlind( false );
	g_GUIMgr.HideWnd( MINIMAP_WND );
	g_GUIMgr.HideWnd( SKILL_LIST_WND );
	g_GUIMgr.HideWnd( ACCESSORY_SKILL_LIST_WND );
	
	if( g_MyInfo.GetCharCount() != 0 )
	{
		g_GUIMgr.ShowWnd( SKILL_LIST_WND );      //로비 메인 윈도우보다 위에 띄우기 위해서리..
		g_GUIMgr.ShowWnd( ACCESSORY_SKILL_LIST_WND );
	}	

	m_dwCurClassStartTimer = m_dwClassStartTime = 0;
	if( g_MyInfo.GetUserState() != 0 )    //0이면 최초 접속 유저.
	{
		m_dwCurClassStartTimer = FRAMEGETTIME();
		m_dwClassStartTime     = (float)m_dwCurEventDuration / (g_MyInfo.GetCharCount() + 1);
	}	
	g_App.ClearAllNetworkTable( P2P_STYLE_PLAY );
	g_PushStructListMgr.DestroyStructList();

	g_QuestMgr.QuestCompleteTerm( QC_ENTER_HEADQUARTER );
}

void ioMyRoomMode::LoadCharacters()
{
	int iMaxLoop = g_MyInfo.GetCharCount();
	for(int i = 0;i < iMaxLoop;i++)
	{
		AddMySoldier( g_MyInfo.GetCharIndex( i ) );
	}

	// 용병이 없다. 더미 캐릭터 생성
	if( iMaxLoop == 0 )
	{
		LoadDummyCharacter();
	}
}

void ioMyRoomMode::LoadDummyCharacter()
{
	ioINILoader &rkLoader = Help::GetCharLoader();
	rkLoader.SetTitle_e( "character1" );

	char szModelName[MAX_PATH];
	rkLoader.LoadString_e( "model_file", "", szModelName, MAX_PATH );

	ioBaseChar *pChar = m_pCreator->CreateBaseChar( szModelName );
	if( !pChar ) return;

	pChar->SetCharName( g_MyInfo.GetPublicID() );
	pChar->InitINIValue( rkLoader );
	pChar->InitRaceInfo( true );

	ioUserKeyInput::UseKeyValue kKeys;
	kKeys.SetDefaultFirstKey();
	ioUserKeyInput::UseKeyValue kSecondKeys;
	kSecondKeys.SetDefaultSecondKey();
	ioUserKeyInput::UseKeyValue kJoyKeys;
	kJoyKeys.SetDefaultJoyKey();

	g_KeyManager.GetGameKey(kKeys);
	g_KeyManager.GetSecondGameKey(kSecondKeys);
	g_KeyManager.GetJoyGameKey(kJoyKeys);

	pChar->SetUpKeyValue( kKeys );
	pChar->SetUpSecondKeyValue( kSecondKeys );
	pChar->SetUpJoyKeyValue( kJoyKeys );

	pChar->SetIsOwnerChar( true );
	pChar->EnableKeyInputCheck( false );
	pChar->SetTeam( TEAM_PRIVATE_1 );

	pChar->SetIndex( 0 );
	pChar->SetLevel( 0 );
	pChar->SetClassLevel( 0 );

	D3DXVECTOR3 vPos;
	vPos.x = 0.0f;
	vPos.z = 0.0f;
	vPos.y = m_pCreator->GetMapHeight( vPos.x, vPos.z );

	pChar->SetWorldPosition( vPos );
	pChar->FillMaxHP();
	pChar->SetReadyState();
	AddNewRecord( pChar );

	g_GUIMgr.HideWnd( SKILL_LIST_WND );
	g_GUIMgr.HideWnd( ACCESSORY_SKILL_LIST_WND );

	if( !g_MyInfo.IsBankruptcy() ) //파산이 아니면
	{
		g_GUIMgr.HideWnd( MY_INVENTORY_WND );
	}
}

void ioMyRoomMode::LoadNPC()
{
	ModeRecord *pModeRecord = FindModeRecordCharIndex( NPC_INDEX );
	if( pModeRecord && pModeRecord->pChar )
	{
		pModeRecord->pChar->SetCreateMotionState();
		return;
	}

	ioPlayMode::LoadNPC();
	m_pCreator->AddAllNPCSyncFirstUser();
}

void ioMyRoomMode::SetMyCharEquipItem( ioBaseChar *pChar, int iCharArray )
{
	pChar->DestroyAllItem();

	for(int j = 0;j < MAX_INVENTORY;j++)
	{
		const ITEM_DATA &kItem = g_MyInfo.GetCharItem( iCharArray, j );
		if( kItem.m_item_code > 0 )
		{
			m_pCreator->ReleaseFieldItemByIndex( pChar->GetIndex() * 100 + j );

			ioItem *pItem = m_pCreator->CreateItem( kItem.m_item_code );
			if( pItem )
			{
				pItem->ReLoadProperty();
				pItem->SetOwnerName( pChar->GetCharName() );
				pItem->SetItemCreateIndex( pChar->GetIndex() * 100 + j );
				pItem->SetItemReinforce( kItem.m_item_reinforce );
				pItem->SetItemCustom( kItem.m_item_male_custom, kItem.m_item_female_custom );
		
				ioItem *pPreItem = pChar->EquipItem( pItem, true );
				SAFEDELETE( pPreItem );

				// VoiceType Set
				int iVoiceType = 0;
				if( j == 0 )
				{
					const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( kItem.m_item_code - 1 );
					if( pSetInfo )
						iVoiceType = pSetInfo->GetVoiceType();

					pChar->SetVoiceType( iVoiceType );
				}
			}
		}
	}

	pChar->FillMaxHP();
}

void ioMyRoomMode::SetMyCharEquipMedalItem( ioBaseChar *pChar, int iCharArray )
{
	if( !pChar )
		return;
	pChar->ClearMedalItem();

	int iClassType = g_MyInfo.GetClassType( iCharArray );
	if( iClassType == -1 )
		return;

	ioUserMedalItem *pUserMedalItem = g_MyInfo.GetUserMedalItem();
	if( !pUserMedalItem )
		return;

	MedalSyncInfoVec vItemTypeVec;
	g_MyInfo.GetMedalItemTypeVec( vItemTypeVec, iClassType );

	if( vItemTypeVec.empty() )
		return;

	for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
	{
		if( !COMPARE( i, 0, (int) vItemTypeVec.size() ) )
			return;

		pChar->SetCustomMedalItem( vItemTypeVec[i].m_iItemType, vItemTypeVec[i].m_iCustomIndex, vItemTypeVec[i].m_iStat, true );	
	}
}

void ioMyRoomMode::SetChangeExtraItem( int iCharArray, int iSlot )
{
	bool bNeedChange = true;
	if( m_bOneMyChar && iCharArray != g_MyInfo.GetSelectCharArray() )
		bNeedChange = false;

	if( !bNeedChange )
		return;

	int iCharIndex = g_MyInfo.GetCharIndex( iCharArray );
	const ITEM_DATA &rkItemData = g_MyInfo.GetCharItem( iCharArray, iSlot );

	ioBaseChar *pChar = m_pCreator->GetBaseChar( iCharIndex );
	if( pChar )
	{
		if( !pChar->GetEquipedItem( iSlot ) )
		{
			if( pChar->GetState() == CS_EXTRAITEM_CHANGE )
				pChar->SetState( CS_DELAY );

			pChar->CheckDefaultReinforce();
			return;
		}

		ioItem *pItem = m_pCreator->CreateItem( rkItemData.m_item_code );
		if( pItem )
		{
			pItem->ReLoadProperty();
			pItem->SetOwnerName( pChar->GetCharName() );
			pItem->SetItemCreateIndex( pChar->GetIndex() * 100 + iSlot );
			pItem->SetItemReinforce( rkItemData.m_item_reinforce );
			pItem->SetItemCustom( rkItemData.m_item_male_custom, rkItemData.m_item_female_custom );

			float fPreSkillGauge = 0.0f;
			ioItem *pPreItem = pChar->EquipItem( pItem, true );
			if( pPreItem )
				fPreSkillGauge = pPreItem->GetCurSkillGuage();

			pItem->SetCurSkillGauge( fPreSkillGauge );

			// 용병교체시 총알, 레이져 충전량 관련 처리
			if( pPreItem && pItem->GetType() == ioItem::IT_WEAPON )
			{
				int iCurBullet = pPreItem->GetCurBullet();
				pItem->SetCurBullet( iCurBullet );
			}

			SAFEDELETE( pPreItem );

			if( pChar->GetState() == CS_EXTRAITEM_CHANGE )
				pChar->SetState( CS_DELAY );
		}
		else
		{
			// 예외상황
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), (int)rkItemData.m_item_code );
			return;
		}

		pChar->CheckDefaultReinforce();
	}
}

void ioMyRoomMode::TutorialDropFieldItem()
{
	// 드랍 위치 결정
	Vector3Vec vItemPos;
	vItemPos.assign( m_vDropFieldItemPos.begin(), m_vDropFieldItemPos.end() );
	for(int i = 0;i < (int)vItemPos.size();i++)
	{
		int r = rand()%vItemPos.size();
		D3DXVECTOR3 vTempPos = vItemPos[i];
		vItemPos[i] = vItemPos[r];
		vItemPos[r] = vTempPos;
	}

	// 아이템 생성
	IntVec vClassType;
	vClassType.reserve(10);
	int iSize = g_ClassPrice.MaxClassPrice();
	for (int i = 0; i < iSize ; i++)
	{
		if( !g_ClassPrice.IsActiveClass( i ) )
			continue;
		vClassType.push_back( i + 1 );
	}
	int iRand = rand()%vClassType.size();
	int iClassType = 1;
	if( COMPARE( iRand, 0, (int) vClassType.size() ) )
		iClassType = vClassType[iRand];
	vClassType.clear();
	
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetClassTypeForTutorialDropItem( iClassType );

	for( int i = 0;i < MAX_INVENTORY;i++)
	{
		int ItemType = ( i * DEFAULT_BASIC_ITEM_CODE ) + iClassType;
		ioItem *pItem = m_pCreator->CreateItem( ItemType );
		if( pItem )
		{
			pItem->ReLoadProperty();
			pItem->SetItemCreateIndex( g_MyInfo.GetCharIndex( 0 ) * 100 + i + MAX_INVENTORY );
			pItem->SetOwnerName( Help::GetDeveloperKCharName() );

			ioFieldItem *pField = m_pCreator->CreateFieldItem( pItem );
			if( pField )
			{
				D3DXVECTOR3 vCenter = vItemPos[i];
				vCenter.y = GetMapHeight( vCenter.x, vCenter.z );
				pField->SetWorldPositionByServerSide( vCenter );
				pField->SetDropState( pField->GetWorldPosition() );
				pField->CreateItemEffect();
			}
		}
	}	
}

void ioMyRoomMode::ReleaseTutorialDropFieldItem()
{
	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		m_pCreator->ReleaseFieldItemByIndex( g_MyInfo.GetCharIndex( 0 ) * 100 + i + MAX_INVENTORY );
	}
}

void ioMyRoomMode::ReStartMode()
{
	LOG.PrintTimeAndLog( 0, "ioMyRoomMode::ReStartMode Is Call???" );
}

void ioMyRoomMode::ProcessMode()
{
	ErrorReport::SetPosition( 1050, 0 );
	ProcessSelectCharacter();
	ErrorReport::SetPosition( 1050, 1 );
	ProcessGUI();
	ErrorReport::SetPosition( 1050, 2 );
	ProcessClassStartGOGO();
	ErrorReport::SetPosition( 1050, 3 );

	switch( m_ModeState )
	{
	case MS_READY:
		ErrorReport::SetPosition( 1050, 4 );
		ProcessReadyState();
		ErrorReport::SetPosition( 1050, 5 );
		CheckModeMapSound();
		break;
	case MS_PLAY:
		ErrorReport::SetPosition( 1050, 6 );
		ProcessPlayState();
		ErrorReport::SetPosition( 1050, 7 );
		CheckModeMapSound();
		ErrorReport::SetPosition( 1050, 8 );
		g_EventMgr.Process();
		break;
	}

	ErrorReport::SetPosition( 1050, 9 );
}

void ioMyRoomMode::ProcessReadyState()
{
	if( m_dwReadyStartTime + m_dwCurEventDuration > FRAMEGETTIME() )	return;

	SetModeState( MS_PLAY );	
	m_dwOwnerPosSaveTimer = FRAMEGETTIME();
	LoadSound();
}

void ioMyRoomMode::ProcessClassStartGOGO()
{
	if( m_dwCurClassStartTimer == 0 ) return;

	DWORD dwGap = FRAMEGETTIME() - m_dwCurClassStartTimer;

	if( dwGap < m_dwClassStartTime )
		return;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( !pChar ) continue;
		if( pChar->GetIndex() == 0 ) continue;
		if( pChar->GetState() != CS_READY ) continue;

		pChar->SetCreateMotionState();

		m_dwCurClassStartTimer = FRAMEGETTIME();
		return;
	}

	m_dwCurClassStartTimer = m_dwClassStartTime = 0;
}

void ioMyRoomMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	UpdateTimeGrowthTime();

	m_pCreator->NotifySpringDampUseSet( true );

	ProcessGaugeUpCheck();
	ProcessOwnerDelayPosSave();
	ProcessRevival();	

	if( g_TutorialMgr.StartRandomItemDrop() )
		TutorialDropFieldItem();
	else if( g_TutorialMgr.StartCharInitPos() )
		MySoldierDefPos();
	else if( g_TutorialMgr.StartCharDropStepPos() )
		MySoldierDropStepPos();

	ProcessMyLobbyAlarm();
}

void ioMyRoomMode::ProcessGaugeUpCheck()
{
	m_pCreator->ReCalculateDamageRateByMyRoom();
}

void ioMyRoomMode::ProcessOwnerPosSave( DWORD dwIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( !pChar ) continue;

		if( pChar->GetIndex() == dwIndex )
		{			
			D3DXVECTOR3 vOwnerPos = pChar->GetWorldPosition();
			int iPosArray = pChar->GetTeam() - TEAM_PRIVATE_1;
			if( COMPARE( iPosArray, 0, (int)m_vStartPos.size() ) )
			{
				m_vStartPos[iPosArray].x = vOwnerPos.x;
				m_vStartPos[iPosArray].z = vOwnerPos.z;
			}
		}
	}
}

void ioMyRoomMode::ProcessOwnerDelayPosSave()
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( !pChar ) continue;
		if( !pChar->IsOwnerChar() ) continue;
		if( pChar->GetState() != CS_DELAY )
		{
			m_dwOwnerPosSaveTimer = FRAMEGETTIME();
			continue;
		}

		if( FRAMEGETTIME() - m_dwOwnerPosSaveTimer < 2000 ) continue;

		D3DXVECTOR3 vOwnerPos = pChar->GetWorldPosition();
		int iPosArray = pChar->GetTeam() - TEAM_PRIVATE_1;
		if( COMPARE( iPosArray, 0, (int)m_vStartPos.size() ) )
		{
			m_vStartPos[iPosArray].x = vOwnerPos.x;
			m_vStartPos[iPosArray].z = vOwnerPos.z;
		}
	}
}

void ioMyRoomMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateEndScoreWndTime();
	UpdateSoldierWndTime();
	UpdateRoundStateWnd();

	ProcessPushEnableCursor();
	ProcessBlindMode();

	g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_ATTENDANCE );
	g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_ATTENDANCE_REWARD );
	g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_PCROOOM );
	g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_TOURNAEMNT_REWARD );
	g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_GUILD_RANK_REWARD );
	g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_ROLLBOOK );
}

void ioMyRoomMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ModeRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.dwCurDieTime == 0 ) continue;

		if( rkRecord.pChar->IsOwnerChar() )
		{
			int iRevivalTime = (int)rkRecord.dwRevivalGap - ( dwCurTime - rkRecord.dwCurDieTime );
			if( iRevivalTime < 0 ) iRevivalTime = 0;
			
			if( P2PNetwork::IsNetworkPlaying() && IsNetworkMode() )
			{
				if( rkRecord.dwCurDieTime + rkRecord.dwRevivalGap < dwCurTime )
				{
					rkRecord.dwCurDieTime = 0;
				}
			}
		}
		
		if( !P2PNetwork::IsNetworkPlaying() || !IsNetworkMode() )
		{
			if( rkRecord.dwCurDieTime + rkRecord.dwRevivalGap < dwCurTime )
			{
				rkRecord.iRevivalCnt++;
				rkRecord.dwCurDieTime = 0;
				
				ioBaseChar *pChar = rkRecord.pChar;

				D3DXVECTOR3 vNewPos = GetRevivalPos( pChar );
				pChar->SetWorldPosition( vNewPos );
				pChar->Revival();

				if( g_MyInfo.GetCharArray( pChar->GetIndex() ) != -1 )
				{
					SetMyCharEquipItem( pChar, pChar->GetSelectCharArray() );
				}
				g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_VIEW );
				pChar->SetStartMotionState();
				pChar->CheckDefaultReinforce();
				
				if( pChar->IsOwnerChar() )
				{
					SetBlindMode( false );
				}
			}
		}
	}
}

void ioMyRoomMode::ProcessMyLobbyAlarm()
{
	if( g_MyInfo.IsTutorialUser() ) return;
	if( m_dwCurMyLobbyAlarmTime == 0 ) return;
	if( m_dwMyLobbyAlarmTime == 0 ) return;

	if( FRAMEGETTIME() - m_dwCurMyLobbyAlarmTime > m_dwMyLobbyAlarmTime )
	{
		m_dwCurMyLobbyAlarmTime = FRAMEGETTIME();
	}
}

bool ioMyRoomMode::GetPartyStateInfo( int index, PartyStateInfo &rkInfo )
{
	ModeRecord *pRecord = FindModeRecord( index );
	if( !pRecord )	return false;
	if( !pRecord->pChar ) return false;
	
	rkInfo.m_szName = pRecord->pChar->GetCharName();
	rkInfo.m_Team   = pRecord->pChar->GetTeam();
	rkInfo.m_bOwner = pRecord->pChar->IsOwnerChar();
	rkInfo.m_iLevel = pRecord->pChar->GetLevel();
	if( g_BattleRoomMgr.GetOwnerName() == pRecord->pChar->GetCharName() )
		rkInfo.m_bPartyOwner = true;
	else if( g_ShuffleRoomMgr.GetOwnerName() == pRecord->pChar->GetCharName() )
		rkInfo.m_bPartyOwner = true;
	else
		rkInfo.m_bPartyOwner = false;

	return true;
}

void ioMyRoomMode::UpdateRoundStateWnd()
{	
}

void ioMyRoomMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap || !pMiniMap->IsShow() )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();

	pMiniMap->ClearPointList();

	D3DXVECTOR3 vPos;

	// Other Char
	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		if( pChar->IsOwnerChar() || !pChar->IsCanMiniMapVisible() )
			continue;
		
		vPos = pChar->GetWorldPosition();
		pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, TEAM_RED, 0 );
	}

	// My Char
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && pOwner->IsCanMiniMapVisible() )
	{
		vPos = pOwner->GetWorldPosition();
		pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_ME, TEAM_BLUE, 0 );
	}

	// pDummy
	const GameEntityList &rkObjectList = m_pCreator->GetGameEntityList();
	GameEntityList::const_iterator iter_b;
	for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
	{
		ioDummyChar *pDummy = ToDummyChar( *iter_b );
		if( pDummy && pDummy->IsVisibleMiniMap() )
		{
			vPos = pDummy->GetWorldPosition();
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pDummy->GetTeam(), 0 );
		}
	}
}

void ioMyRoomMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	if( pDieChar && pKillChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				pDieRecord->AddDeath( GetModeType(), 1 );
			else
			{
				pAttRecord->AddKill( GetModeType(), 1, true, false );
				pDieRecord->AddDeath( GetModeType(), 1 );
			}

			pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			pDieRecord->AddDeath( GetModeType(), 1 );
			pDieRecord->pChar->SetDeathEmoticon();
		}
	}	
}

void ioMyRoomMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );
	if( pDieChar && pKillerChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillerChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pDieRecord && pAttRecord )
		{
			if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				pDieRecord->AddDeath( GetModeType(), 1 );
			else
			{
				pAttRecord->AddKill( GetModeType(), 1, false, false );
				pDieRecord->AddDeath( GetModeType(), 1 );	
			}

			pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();
		}
	}
	else
	{
		if( pDieChar )
		{
			ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
			if( pDieRecord )
			{
				pDieRecord->AddDeath( GetModeType(), 1 );
				pDieRecord->pChar->SetDeathEmoticon();
			}
		}
	}
}

bool ioMyRoomMode::IsCollisionCheck()
{
	return true;
}

void ioMyRoomMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioMyRoomMode::RenderAfterGUI()
{
}

void ioMyRoomMode::ClearMode()
{
	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
	g_GUIMgr.HideWnd( LOBBY_MAIN_WND );
	SaveCharacterPos();
	m_vDefStartPos.clear();
	m_vBackupDefStarPos.clear();
	ioPlayMode::ClearMode();

	if( g_MyInfo.DeleteExerciseChar( EXERCISE_EVENT ) )
		g_MyInfo.FixSelectChar();
}

void ioMyRoomMode::LoadModeResource( ioINILoader &rkLoader )
{
	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>(g_GUIMgr.AddWnd( "XML/MiniSoldierSelectWnd.xml" ));
	if(pMiniSoldierWnd)
		pMiniSoldierWnd->SetPlayStage( m_pCreator );

	g_GUIMgr.AddWnd( "XML/SkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/AccessorySkillListWnd.xml" );

	g_ModeUIController.SetCenterKillInfoWnd( NULL );
	ioPlayMode::LoadModeResource( rkLoader );
}

D3DXVECTOR3 ioMyRoomMode::GetRevivalPos( ioBaseChar *pChar )
{
	if(!pChar)
	{
		LOG.PrintTimeAndLog( 0, "ioMyRoomMode::GetRevivalPos() - Char is NULL" );
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	int iPosArray = 0;
	if( pChar->GetTeam() == TEAM_BLUE  )
		iPosArray = m_iBluePosArray;
	else if(pChar->GetTeam() == TEAM_RED )
		iPosArray = m_iRedPosArray;
	else if( !m_vDefStartPos.empty() )
		iPosArray =  ( (int)pChar->GetTeam() -  TEAM_PRIVATE_1 ) ;

	int iMaxStartPos = m_vDefStartPos.size();
	if( !COMPARE( iPosArray, 0, iMaxStartPos ) )
		return m_vMapCenter;

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot, ioMath::RangeRandom( 0.0f, D3DX_PI * 2.0f ), 0.0f, 0.0f );

	D3DXVECTOR3 vCenter = m_vDefStartPos[iPosArray];
	D3DXVECTOR3 vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );
	vCenter += vMoveDir * ioMath::RangeRandom( FLOAT10, FLOAT10 );
	vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );

	return vCenter;
}

D3DXVECTOR3 ioMyRoomMode::GetStartPos( TeamType eTeamType, bool bRandom )
{
	int iPosArray = 0;
	if( eTeamType == TEAM_BLUE  )
		iPosArray = m_iBluePosArray;
	else if(eTeamType == TEAM_RED )
		iPosArray = m_iRedPosArray;
	else if( !m_vStartPos.empty() )
		iPosArray =  ( (int)eTeamType -  TEAM_PRIVATE_1 ) ;

	int iMaxStartPos = m_vStartPos.size();
	if( !COMPARE( iPosArray, 0, iMaxStartPos ) )
		return m_vMapCenter;

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot,	ioMath::RangeRandom( 0.0f, D3DX_PI * 2.0f ), 0.0f, 0.0f );

	D3DXVECTOR3 vCenter = m_vStartPos[iPosArray];
	D3DXVECTOR3 vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );

	if( bRandom )
		vCenter += vMoveDir * ioMath::RangeRandom( FLOAT10, FLOAT10 );

	vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );

	return vCenter;
}

void ioMyRoomMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								   float fLastRate, float fBestRate )
{
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );	
}

void ioMyRoomMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									 float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioMyRoomMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
								   float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioMyRoomMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioMyRoomMode::OnNotifyCharDie - %s Not Exit Char",
								pDieChar->GetCharName().c_str() );
		return;
	}

	if( pKillChar && pKillChar->GetTeam() != pDieChar->GetTeam() )
	{
		if( pBestAttacker && pBestAttacker->GetCharName() == pKillChar->GetCharName() )
		{
			pKillChar->SetKOHPRecover( m_fKORecoverRate * 2.0f, m_PerfectRecoverEffect );
			
			if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
			{
				SetRecoveryGauge( pKillChar, fLastRate );
			}
		}
		else
		{
			pKillChar->SetKOHPRecover( m_fKORecoverRate, m_KORecoverEffect );
			
			if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
			{
				SetRecoveryGauge( pKillChar, fLastRate );
			}

			if( pBestAttacker )
			{
				pBestAttacker->SetKOHPRecover( m_fKORecoverRate, m_KORecoverEffect );
				
				if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
				{
					SetRecoveryGauge( pBestAttacker, fBestRate );
				}

				if( pBestAttacker->IsOwnerChar() )
				{
					m_pCreator->StartScreenBlind( SBT_KILL, NULL, true );
				}
			}
		}
	}

	pDieRecord->dwCurDieTime = FRAMEGETTIME();
	pDieRecord->dwRevivalGap = m_dwCurRevivalTime;
}

void ioMyRoomMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ModeRecord *pRecord = FindModeRecordCharName( pChar->GetCharName() );
	if( !pRecord )
	{
		ModeRecord kRecord;
		kRecord.pChar = pChar;
		kRecord.szName= pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );

		UpdateUserRank();
	}
	else
	{
		pRecord->pChar = pChar;
	}
}

void ioMyRoomMode::RemoveRecord( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
		{
			m_vCharRecordList.erase( m_vCharRecordList.begin() + i );
			break;
		}
	}

	UpdateUserRank();
}

void ioMyRoomMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}

void ioMyRoomMode::AddMySoldier( DWORD dwIndex, bool bBuySoldier )
{
	if( GetMyRoomType() != MRT_PRACTICE )
		return;

	const int iArray = g_MyInfo.GetCharArray( dwIndex );
	if( iArray == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMyRoomMode::AddMySoldier( %d ) = %d -_-;", dwIndex, iArray );
		return; 
	}

	bool bExercise = g_MyInfo.IsCharExercise( iArray );
	if( m_bOneMyChar ) 
	{
		if( bExercise )
			m_bBuyExercise = true;

		ChangeOwnerCharByOneChar( dwIndex );
		return;
	}

	RemoveMySoldier( 0 );     //더미 캐릭터 삭제

	// 최초 고용 용병이거나 파산으로 받은 캐릭터.
	if( g_MyInfo.IsTutorialUser() || m_vCharRecordList.size() == 1 ) 
		bBuySoldier = false;
	
	char szTempName[MAX_PATH];
	sprintf_e( szTempName, "%s(%d)", g_MyInfo.GetPublicID().c_str(), iArray+1 );

	ioHashString szCharName = szTempName;

	const CHARACTER &kCharInfo = g_MyInfo.GetCharacter( iArray );
	ioINILoader &rkLoader = Help::GetCharLoader( kCharInfo.m_kindred, kCharInfo.m_sex );

	char szFileName[MAX_PATH];
	rkLoader.LoadString_e( "model_file", "", szFileName, MAX_PATH );

	ioBaseChar *pChar = m_pCreator->CreateBaseChar( szFileName );
	if( !pChar )
		return;

	pChar->SetCharName( szCharName );
	pChar->InitINIValue( rkLoader );
	pChar->InitRaceInfo( kCharInfo );
	pChar->SetSelectCharArray( iArray );

	if( iArray == g_MyInfo.GetSelectCharArray() )
	{
		ioUserKeyInput::UseKeyValue kKeys;
		kKeys.SetDefaultFirstKey();
		ioUserKeyInput::UseKeyValue kSecondKeys;
		kSecondKeys.SetDefaultSecondKey();
		ioUserKeyInput::UseKeyValue kJoyKeys;
		kJoyKeys.SetDefaultJoyKey();
		
		g_KeyManager.GetGameKey(kKeys);
		g_KeyManager.GetSecondGameKey(kSecondKeys);
		g_KeyManager.GetJoyGameKey(kJoyKeys);

		pChar->SetUpKeyValue( kKeys );
		pChar->SetUpSecondKeyValue( kSecondKeys );
		pChar->SetUpJoyKeyValue( kJoyKeys );

		pChar->SetIsOwnerChar( true );		
		pChar->EnableKeyInputCheck( true );
		g_MyInfo.SetChangeChar( g_MyInfo.GetCharIndex( iArray ) );
	}
	else
	{
		pChar->SetIsOwnerChar( false );
		pChar->EnableKeyInputCheck( false );
	}
	pChar->SetTeam( (TeamType)(TEAM_PRIVATE_1 + iArray) );
	pChar->SetIndex( g_MyInfo.GetCharIndex( iArray ) );
	int iClassType = g_MyInfo.GetClassType( iArray );
	int iClassLevel= g_MyInfo.GetClassLevel( iClassType, false );
	pChar->SetLevel( g_MyInfo.GetGradeLevel() );
	pChar->SetClassLevel( iClassLevel );//

	D3DXVECTOR3 vNewPos = GetStartPos( pChar->GetTeam() );
	pChar->SetWorldPosition( vNewPos );
	AddNewRecord( pChar );
	SetMyCharEquipItem( pChar, iArray );
	SetMyCharEquipMedalItem( pChar, iArray );
	pChar->CheckDefaultReinforce();

	if( GetModeState() == MS_PLAY && !bBuySoldier )
	{
		if( g_MyInfo.IsTutorialUser() && bExercise && m_bBuyExercise )
			pChar->SetReadyState();
		else
			pChar->SetCreateMotionState();
	}
	else
	{
		if( g_MyInfo.IsTutorialUser() )
			pChar->SetCreateMotionState();
		else
			pChar->SetReadyState();
	}
	
	g_GUIMgr.ShowWnd( SKILL_LIST_WND );
	g_GUIMgr.ShowWnd( ACCESSORY_SKILL_LIST_WND );

	if( bExercise )
	{
		if( g_MyInfo.IsTutorialUser() || bBuySoldier )
		{
			if( m_bBuyExercise  )
			{
				ChangeOwnerChar( dwIndex, true );
				pChar->SetWorldPosition( m_vRemoveExercisePos );
				pChar->SetTargetRot( m_qtRemoveExerciseRot );
				pChar->SetState( CS_DELAY );
				pChar->SetCharChangeEffect();
				return;
			}
			else
				m_bBuyExercise = true;
		}
	}

	if( g_MyInfo.IsTutorialUser() ) 
	{
		if( g_MyInfo.GetUserState() % 100  == 0 )
		{
			LoadNPC();
		}		
		else
		{
			ChangeOwnerChar( dwIndex, true );
			m_dwCurClassStartTimer = FRAMEGETTIME();
			m_dwClassStartTime     = 500;
		}
	}
	else if( bBuySoldier && GetMyRoomType() == MRT_PRACTICE )
	{
		if( m_bOneMyChar )
			ChangeOwnerCharByOneChar( dwIndex );
		else
			ChangeOwnerChar( dwIndex, true );

		m_dwCurClassStartTimer = FRAMEGETTIME();
		m_dwClassStartTime     = 500;
	}	
}

void ioMyRoomMode::RemoveMySoldier( DWORD dwIndex )
{
	if( m_bOneMyChar ) 
		return;

	int iCharCnt = GetRecordCharCnt();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( pChar == NULL ) continue;

		if( pChar->GetIndex() == dwIndex )
		{
			for(int j = 0;j < MAX_INVENTORY;j++)
				m_pCreator->ReleaseFieldItemByIndex( pChar->GetIndex() * 100 + j );

			m_vRemoveExercisePos  = pChar->GetWorldPosition();
			m_qtRemoveExerciseRot = pChar->GetTargetRot();
			m_pCreator->RemoveUser( pChar->GetCharName(), true );
			break;
		}
	}

	// 용병이 없다. 더미 캐릭터 생성
	if( g_MyInfo.GetCharCount() == 0 )       
	{
		if( dwIndex != 0 )//더미캐릭터를 삭제한게 아니라면..
		{
			LoadDummyCharacter();
		}
	}
	else
	{
		ResetOwnerAndName();
	}
}

void ioMyRoomMode::ReLoadMySoldier( int iCharArray )
{
	int iCharCnt = GetRecordCharCnt();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( pChar == NULL ) continue;
		if( pChar->GetIndex() == 0 ||
			pChar->GetIndex() == NPC_INDEX ) continue;

		if( pChar->GetSelectCharArray() == iCharArray )
		{
			pChar->InitRaceInfo( g_MyInfo.GetCharacter( iCharArray ) );
			
			SetMyCharEquipItem( pChar, iCharArray );
			pChar->CheckDefaultReinforce();
			break;
		}
	}
}

void ioMyRoomMode::ReLoadAllMySoldier()
{
	if( GetMyRoomType() != MRT_PRACTICE )
		return;

	int iCharCnt = GetRecordCharCnt();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( pChar == NULL ) continue;
		if( pChar->GetIndex() == 0 ||
			pChar->GetIndex() == NPC_INDEX ) continue;

		int iCharArray = pChar->GetSelectCharArray();
		pChar->InitRaceInfo( g_MyInfo.GetCharacter( iCharArray ) );
		SetMyCharEquipItem( pChar, iCharArray );
		pChar->CheckDefaultReinforce();
	}
}

void ioMyRoomMode::ResetOwnerAndName()
{
	int iCharCnt = GetRecordCharCnt();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( pChar == NULL ) continue;
		if( pChar->GetIndex() == NPC_INDEX ) continue;     //개발자 K

		int iCharArray = g_MyInfo.GetCharArray( pChar->GetIndex() );
	
		if( iCharArray == -1 )
		{
			LOG.PrintTimeAndLog( 0, "ioMyRoomMode::ResetOwnerAndName %d - %d - %d",
									g_MyInfo.GetSelectCharArray(),
									iCharArray,
									pChar->GetIndex() );
			continue;
		}

		char szTempName[MAX_PATH];
		sprintf_e( szTempName, "%s(%d)", g_MyInfo.GetPublicID().c_str(), iCharArray+1 );

		ioHashString szCharName = szTempName;

		m_vCharRecordList[i].szName = szTempName;
		pChar->SetCharName( szCharName );
		pChar->SetSelectCharArray( iCharArray );

		if( iCharArray == g_MyInfo.GetSelectCharArray() )
		{
			ioUserKeyInput::UseKeyValue kKeys;
			kKeys.SetDefaultFirstKey();
			ioUserKeyInput::UseKeyValue kSecondKeys;
			kSecondKeys.SetDefaultSecondKey();
			ioUserKeyInput::UseKeyValue kJoyKeys;
			kJoyKeys.SetDefaultJoyKey();

			g_KeyManager.GetGameKey(kKeys);
			g_KeyManager.GetSecondGameKey(kSecondKeys);
			g_KeyManager.GetJoyGameKey(kJoyKeys);

			pChar->SetUpKeyValue( kKeys );
			pChar->SetUpSecondKeyValue( kSecondKeys );
			pChar->SetUpJoyKeyValue( kJoyKeys );

			pChar->SetIsOwnerChar( true );		
			pChar->EnableKeyInputCheck( true );
			g_MyInfo.SetChangeChar( pChar->GetIndex() );

			CharState eState = pChar->GetState();
			if( eState == CS_CHANGE_WAIT || eState == CS_DELAY || eState == CS_SPECIAL_MOTION || eState == CS_ETCITEM_MOTION )
			{
				pChar->SetState( CS_DELAY );
			}
		}
		else
		{
			pChar->SetIsOwnerChar( false );		
			pChar->EnableKeyInputCheck( false );
		}

		pChar->SetTeam( (TeamType)(TEAM_PRIVATE_1 + iCharArray) );
	}
}

void ioMyRoomMode::SetCharSlotChange()
{
	ResetOwnerAndName();	
}

ioBaseChar* ioMyRoomMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioMyRoomMode::GetRecordChar( const ioHashString &rkName )
{
	ModeRecord *pRecord = FindModeRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioMyRoomMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioMyRoomMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioMyRoomMode::FindModeRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

ModeRecord* ioMyRoomMode::FindModeRecordCharIndex( const DWORD dwIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].pChar == NULL ) continue;

        if( m_vCharRecordList[i].pChar->GetIndex() == dwIndex )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

void ioMyRoomMode::MySoldierDefPos()
{
	if( GetMyRoomType() != MRT_PRACTICE )
		return;

	m_bOneMyChar = false;

	if( !m_vBackupDefStarPos.empty() )
	{
		m_vDefStartPos.clear();
		m_vStartPos.clear();
		m_vDefStartPos.assign( m_vBackupDefStarPos.begin(), m_vBackupDefStarPos.end() );
		m_vBackupDefStarPos.clear();
	}

	m_vStartPos.assign( m_vDefStartPos.begin(), m_vDefStartPos.end() );

	int iCharCnt = GetRecordCharCnt();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( pChar )
		{
			m_pCreator->RemoveUser( pChar->GetCharName(), false );
		}
	}
	m_vCharRecordList.clear();
	
	LoadCharacters();
	LoadNPC();

	ReleaseTutorialDropFieldItem();
	SetBlindMode( false );
}

void ioMyRoomMode::MySoldierDropStepPos()
{
	m_vBackupDefStarPos.assign( m_vDefStartPos.begin(), m_vDefStartPos.end() );

	ioINILoader_e kLoader( "config/sp2_tutorial.ini" );
	kLoader.SetTitle_e( "drop_step_pos" );

	enum { MAX_POS = 2, };
	m_vStartPos.clear();
	m_vStartPos.reserve( MAX_POS );
	m_vDefStartPos.clear();
	m_vDefStartPos.reserve( MAX_POS );

	D3DXVECTOR3 vStartPos;
	vStartPos.x  = kLoader.LoadFloat_e( "start_my_x", 0.0f );
	vStartPos.z  = kLoader.LoadFloat_e( "start_my_y", 0.0f );
	m_vStartPos.push_back(vStartPos);
	vStartPos.x  = kLoader.LoadFloat_e( "start_devoloperk_x", 0.0f );
	vStartPos.z  = kLoader.LoadFloat_e( "start_devoloperk_y", 0.0f );
	m_vStartPos.push_back(vStartPos);
	
	m_vDefStartPos.assign( m_vStartPos.begin(), m_vStartPos.end() );

	int iCharCnt = GetRecordCharCnt();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( pChar )
		{
			m_pCreator->RemoveUser( pChar->GetCharName(), false );
		}
	}
	m_vCharRecordList.clear();
	
	LoadCharacters();
	LoadNPC();
	SetBlindMode( false );
}

bool ioMyRoomMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1050, 10 );

	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	ErrorReport::SetPosition( 1050, 11 );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_CHANGE_SINGLE_CHAR:
		ErrorReport::SetPosition( 1050, 12 );
		OnChangeChar( rkPacket );
		ErrorReport::SetPosition( 1050, 13 );
		return true;
	case STPK_JOIN_SERVER_LOBBY_INFO:
		ErrorReport::SetPosition( 1050, 14 );
		OnJoinServerLobbyInfo( rkPacket );
		ErrorReport::SetPosition( 1050, 15 );
		break;
	case STPK_SERVER_LOBBY_CHAT:
		ErrorReport::SetPosition( 1050, 16 );
		OnServerLobbyChat( rkPacket );
		ErrorReport::SetPosition( 1050, 17 );
		return true;
	}

	return false;
}

bool ioMyRoomMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioMyRoomMode::ApplyModeInfo( SP2Packet &rkPacket )
{
}

void ioMyRoomMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	ModeRecord *pRecord = FindModeRecordCharName( rkName );
	if( pRecord )
	{
		// HARDCODE: 아래쪽에 유저가 없는 경우도 반드시 동일하게 해줄것
		int iKillCharCnt, iDeathCharCnt;
		pRecord->iKillInfoMap.clear();
		pRecord->iDeathInfoMap.clear();

		rkPacket >> iKillCharCnt;
		for( int i=0; i < iKillCharCnt; ++i )
		{
			int iClassType, iKillCnt;
			rkPacket >> iClassType >> iKillCnt;

			pRecord->iKillInfoMap.insert( KillDeathInfoMap::value_type(iClassType, iKillCnt) );
		}

		rkPacket >> iDeathCharCnt;
		for( int j=0; j < iDeathCharCnt; ++j )
		{
			int iClassType, iDeathCnt;
			rkPacket >> iClassType >> iDeathCnt;

			pRecord->iDeathInfoMap.insert( KillDeathInfoMap::value_type(iClassType, iDeathCnt) );
		}

		bool bTemp;
		if( bDieCheck )
		{
			rkPacket >> bTemp;
		}

		bool bCatch;
		rkPacket >> bCatch;
	}
	else
	{
		int iKillCharCnt, iDeathCharCnt;

		rkPacket >> iKillCharCnt;
		for( int i=0; i < iKillCharCnt; ++i )
		{
			int iClassType, iKillCnt;
			rkPacket >> iClassType >> iKillCnt;
		}

		rkPacket >> iDeathCharCnt;
		for( int j=0; j < iDeathCharCnt; ++j )
		{
			int iClassType, iDeathCnt;
			rkPacket >> iClassType >> iDeathCnt;
		}

		bool bTemp;
		if( bDieCheck )
			rkPacket >> bTemp;

		bool bCatch;
		rkPacket >> bCatch;
	}
}

int ioMyRoomMode::GetRoundWinCnt( TeamType eTeamType )
{
	return 0;
}

void ioMyRoomMode::ContributeConvert()
{
}

void ioMyRoomMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{	
}

void ioMyRoomMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{	
}

void ioMyRoomMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{	
}

void ioMyRoomMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{	
}

void ioMyRoomMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
}

void ioMyRoomMode::ChangeOwnerChar( DWORD dwIndex, bool bBuySoldier )
{
	int iPreCharArray = g_MyInfo.GetSelectCharArray();

	bool bChange = false;
	ioBaseChar *pChangeChar = NULL;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( !pChar ) continue;

		if( pChar->GetIndex() == dwIndex )
		{
			g_MyInfo.SetChangeChar( dwIndex );
			if( !bBuySoldier )
			{
				if( pChar->GetState() == CS_DIE )
				{
					m_vCharRecordList[i].iRevivalCnt++;
					m_vCharRecordList[i].dwCurDieTime = 0;

					D3DXVECTOR3 vNewPos = GetRevivalPos( pChar );
					pChar->SetWorldPosition( vNewPos );
					pChar->Revival();				
					if( g_MyInfo.GetCharArray( pChar->GetIndex() ) != -1 )
					{
						SetMyCharEquipItem( pChar, pChar->GetSelectCharArray() );
					}

					pChar->SetStartMotionState();
					pChar->CheckDefaultReinforce();
					
					if( pChar->IsOwnerChar() )
					{
						SetBlindMode( false );
					}
				}
				else
				{
					CharState eState = pChar->GetState();
					if( eState == CS_CHANGE_WAIT || eState == CS_DELAY || eState == CS_SPECIAL_MOTION || eState == CS_ETCITEM_MOTION )
						pChar->SetState( CS_DELAY );
					else if( eState == CS_RUN )
						pChar->SetState( CS_RUN );

					pChar->SetCharChangeEffect();

					bChange = true;
					pChangeChar = pChar;
				}
			}	
		}
		else if( g_MyInfo.GetCharArray(pChar->GetIndex()) == iPreCharArray )
		{
			CharState eState = pChar->GetState();
			if( eState == CS_CHANGE_WAIT || eState == CS_DELAY || eState == CS_SPECIAL_MOTION || eState == CS_ETCITEM_MOTION )
				pChar->SetState( CS_DELAY );
			else if( eState == CS_RUN )
				pChar->SetState( CS_RUN );
		}
	}

	ResetOwnerAndName();

	if( bChange && pChangeChar )
	{
		for( int i= 0; i < ES_OBJECT; ++i )
		{
			ioSkill* pSkill = pChangeChar->GetEquipedSkill( i );
			if( pSkill && ToPassiveSkill( pSkill ) )
			{
				if( !pChangeChar->CheckDisableSkillByMode( pSkill->GetName() ) )
					pSkill->OnSkillStart( pChangeChar );
			}
		}
	}
}

void ioMyRoomMode::ChangeOwnerCharByOneChar( DWORD dwIndex )
{
	const int iArray = g_MyInfo.GetCharArray( dwIndex );
	int iPreCharArray = g_MyInfo.GetSelectCharArray();

	if( iArray == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMyRoomMode::ChangeOwnerCharByOneChar( %d ) = %d -_-;", dwIndex, iArray );
		return; 
	}

	int iCharCnt = m_vCharRecordList.size();
	ioBaseChar *pChar = NULL;
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( !m_vCharRecordList[i].pChar ) continue;

		if( m_vCharRecordList[i].pChar->IsOwnerChar() )
		{
			pChar = m_vCharRecordList[i].pChar;
			break;
		}
	}

	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioMyRoomMode::ChangeOwnerCharByOneChar() - Not Find Char: %d", dwIndex );
		return;
	}

	// 체력정보
	float fHPRate = FLOAT1;
	StatusValue eHP = pChar->GetHP();
	if( eHP.m_fMaxValue > 0.0f )
		fHPRate = eHP.m_fCurValue / eHP.m_fMaxValue;

	// 스킬정보
	FloatVec vSkill;
	vSkill.reserve(MAX_INVENTORY);

	for( int i=0; i<MAX_INVENTORY; i++ )
	{
		float fSkillGauge = 0.0f;
		ioItem *pItem = pChar->GetEquipedItem( i );
		if( pItem )
			fSkillGauge = pItem->GetCurSkillGuage();

		vSkill.push_back( fSkillGauge );
	}

	g_MyInfo.SetChangeChar( dwIndex );
	g_MyInfo.SetCharJoined( dwIndex, true );

	pChar->DestroyAllItem();

	const CHARACTER &kCharInfo = g_MyInfo.GetCharacter( iArray );
	ioINILoader &rkLoader = Help::GetCharLoader( kCharInfo.m_kindred, kCharInfo.m_sex );

	pChar->InitINIValue( rkLoader );
	pChar->InitRaceInfo( kCharInfo );
	pChar->SetSelectCharArray( iArray );

	pChar->SetIndex( g_MyInfo.GetCharIndex( iArray ) );
	int iClassType = g_MyInfo.GetClassType( iArray );
	int iClassLevel= g_MyInfo.GetClassLevel( iClassType, false );
	pChar->SetLevel( g_MyInfo.GetGradeLevel() );
	pChar->SetClassLevel( iClassLevel );//

	AddNewRecord( pChar );
	pChar->SetCurHPRate( fHPRate );

	SetMyCharEquipItem( pChar, iArray );
	SetMyCharEquipMedalItem( pChar, iArray );
	pChar->CheckDefaultReinforce();

	float fNewHP = pChar->GetHP().m_fCurValue * fHPRate;
	pChar->SetCurHP( fNewHP );

	for( int i=0; i<MAX_INVENTORY; i++ )
	{
		ioItem *pItem = pChar->GetEquipedItem( i );
		if( pItem )
			pItem->SetCurSkillGauge( vSkill[i] );
	}

	pChar->SetCharChangeEffect();

	CharState eState = pChar->GetState();
	if( eState == CS_CHANGE_WAIT || eState == CS_DELAY || eState == CS_SPECIAL_MOTION || eState == CS_ETCITEM_MOTION )
		pChar->SetState( CS_DELAY );
	else if( eState == CS_RUN )
		pChar->SetState( CS_RUN );

	ioItem *pItem = pChar->GetEquipedItem( ES_WEAPON );
	ioFireWeaponItem *pWeaponItem = ToFireWeaponItem( pItem );
	ioBowItem *pBow = ToBowItem( pItem );
	ioLaserItem *pLaser = ToLaserItem( pItem );
	ioAutoShotItem *pAutoShot = ToAutoShotItem( pItem );
	ioUroborusItem *pUroborus = ToUroborusItem( pItem );
	ioBubbleFighterItem *pBubbleFighter = ToBubbleFighterItem( pItem );
	ioVulcanItem *pVulcanItem = ToVulcanItem( pItem );

	ITEM_DATA &rkItem = g_MyInfo.GetCharWeaponItem();
	int iCurCharArray = g_MyInfo.GetSelectCharArray();

	int iCurBullet = rkItem.m_item_bullet;
	if( iCurBullet >= 0 )
	{
		if( pBow )
		{
			if( pBow->IsChargeBulletType() )
			{
				if( iCurCharArray == iPreCharArray )
					pBow->SetCurBullet( iCurBullet );
			}
			else
			{
				pBow->SetCurBullet( iCurBullet );
			}
		}
		else if( pWeaponItem )
			pWeaponItem->SetCurBullet( iCurBullet );
		else if( pAutoShot )
			pAutoShot->SetCurBullet( iCurBullet );
		else if( pLaser && iCurCharArray == iPreCharArray )
			pLaser->SetCurBullet( iCurBullet );
		else if( pUroborus && iCurCharArray == iPreCharArray )
			pUroborus->SetCurBullet( iCurBullet );
		else if ( pBubbleFighter )
			pBubbleFighter->SetCurBullet( iCurBullet );
		else if ( pVulcanItem )
			pVulcanItem->SetCurBullet( iCurBullet );
	}
}

void ioMyRoomMode::OnChangeChar( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	DWORD dwPrevIdx, dwNextIdx;
	rkPacket >> dwPrevIdx >> dwNextIdx;

	ProcessOwnerPosSave( dwPrevIdx );
	
	if( m_bOneMyChar )
		ChangeOwnerCharByOneChar( dwNextIdx );
	else
		ChangeOwnerChar( dwNextIdx, false );	

	g_MyInfo.SetUserInfoMgr();
}

void ioMyRoomMode::SetOneMyChar()
{
	if( IsOneMyChar() ) return;

	ioHashStringVec vCharNameList;
	vCharNameList.reserve( m_vCharRecordList.size() );

	RecordList::iterator iter = m_vCharRecordList.begin();
	for( ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		if( !pChar )	continue;

		if( pChar->GetIndex() != NPC_INDEX )
		{
			for(int j = 0;j < MAX_INVENTORY;j++)
			{
				m_pCreator->ReleaseFieldItemByIndex( pChar->GetIndex() * 100 + j );
			}

			vCharNameList.push_back( pChar->GetCharName() );
		}
	}

	int iCharCount = vCharNameList.size();
	for( int i=0 ; i<iCharCount ; i++ )
	{
		m_pCreator->RemoveUser( vCharNameList[i], true );
	}

	AddMySoldier( g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() ) );
	m_bOneMyChar = true;
}

void ioMyRoomMode::SetAllMyChar()
{
	if( !IsOneMyChar() ) return;

	m_bOneMyChar = false;

	ioHashStringVec vCharNameList;
	vCharNameList.reserve( m_vCharRecordList.size() );

	RecordList::iterator iter = m_vCharRecordList.begin();
	for( ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		if( pChar )
		{
			if( pChar->GetIndex() != NPC_INDEX )
			{
				vCharNameList.push_back( pChar->GetCharName() );
			}
		}
	}

	int iCharCount = vCharNameList.size();
	for( int i=0 ; i<iCharCount ; i++ )
	{
		m_pCreator->RemoveUser( vCharNameList[i], true );
	}

	LoadCharacters();
}

bool ioMyRoomMode::IsOneMyChar()
{
	return m_bOneMyChar;
}

void ioMyRoomMode::SetMyLobbyArray( int iMyChannelArray )
{
	m_iMyLobbyArray = iMyChannelArray;
	m_dwCurMyLobbyAlarmTime = FRAMEGETTIME();
}

void ioMyRoomMode::OnJoinServerLobbyInfo( SP2Packet &rkPacket )
{
	int iMyChannelArray;
	rkPacket >> iMyChannelArray;
	SetMyLobbyArray( iMyChannelArray );

	ErrorReport::SetPosition( 1050, 20 );
	g_QuestMgr.QuestCompleteTerm( QC_ENTER_LOBBY );
	ErrorReport::SetPosition( 1050, 21 );
	g_QuestMgr.QuestOccurTerm( QO_ENTER_LOBBY );
	ErrorReport::SetPosition( 1050, 22 );
	g_QuestMgr.QuestOccurTerm( QO_LOBBYNPLAZA_ENTER );
	ErrorReport::SetPosition( 1050, 23 );
	g_GUIMgr.ShowReserveMsgBox();
	ErrorReport::SetPosition( 1050, 24 );

	if( g_MyInfo.IsReservePCRoomCheck() )
	{
		ErrorReport::SetPosition( 1050, 25 );
		g_MyInfo.PCRoomQuestCheck();
	}

	ErrorReport::SetPosition( 1050, 26 );

	LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>(g_GUIMgr.FindWnd( LOBBY_MAIN_WND ));
	if( pLobbyMainWnd )
	{
		ErrorReport::SetPosition( 1050, 18 );
		try
		{
			pLobbyMainWnd->PlayBackgroundOGG();
		}
		catch(...)
		{
			MEMORYSTATUSEX statex;
			statex.dwLength = sizeof(statex);
			GlobalMemoryStatusEx(&statex);

			LOG.PrintTimeAndLog( 0,"Memory is %d Percent Used", statex.dwMemoryLoad);
			LOG.PrintTimeAndLog( 0,"Total Physical Mem is %d", statex.ullTotalPhys/1048576);
			LOG.PrintTimeAndLog( 0,"Free Mem is %d", statex.ullAvailPhys/1048576);
			LOG.PrintTimeAndLog( 0,"Total Paging Mbytes file is %d", statex.ullTotalPageFile/1048576);
			LOG.PrintConsoleLog( 0,"Total Free Paging Mbytes file is %d", statex.ullAvailPageFile/1048576);
			LOG.PrintConsoleLog( 0,"Total Virtual Memory is %d", statex.ullTotalVirtual/1048576);
			LOG.PrintConsoleLog( 0,"Total Free Virtual Memory is %d", statex.ullAvailVirtual/1048576);
			LOG.PrintConsoleLog( 0,"Free Extend Memm is %d", statex.ullAvailExtendedVirtual/1048576);

			LOG.PrintTimeAndLog(0, "%s %s", __FUNCTION__, m_ModeMapSound.c_str() );
		}
		ErrorReport::SetPosition( 1050, 19 );
		if( g_MyInfo.IsBankruptcy() )
		{
			// 파산
			g_GUIMgr.ShowWnd( BANKRUPTCY_CHAR_WND );
		}
	}
}

void ioMyRoomMode::OnServerLobbyChat( SP2Packet &rkPacket )
{
	int iChatType;
	ioHashString szID, szChat;
	rkPacket >> iChatType >> szID >> szChat;

	g_MannerTrialChatMgr.Add( ioMannerTrialChatManager::TT_SERVER_LOBBY_CHAT, szID.c_str(), szChat.c_str() ); // 욕필터전에 할것

	char szChatBuf[MAX_PATH] = "";
	StringCbCopy( szChatBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( szChat.c_str() ) );
	szChat = szChatBuf;

	SetServerLobbyChatMsg( TEAM_RED, szID.c_str(), szChat.c_str(), true );
}

void ioMyRoomMode::ShowBlockNotice()
{
	if( m_bBlockNotice )
		return;

	m_bBlockNotice = true;
	
	g_GUIMgr.ShowWnd( USER_BLOCK_TYPE_ALARM );
}

void ioMyRoomMode::SetMyRoomType( MyRoomType eMyRoomType )
{
	m_eMyRoomType = eMyRoomType;
}