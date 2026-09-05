	

#include "stdafx.h"

#include "ioPlayMode.h"

#include "ioModeGuideArrow.h"
#include "ioFishingManager.h"
#include "ioTowerDefMode.h"
#include "ioMapEventSceneMgr.h"
#include "ioNpcChar.h"

#include "ioExcavationManager.h"
#include "ioSuperGashaponMgr.h"
#include "ioMultiSuperGashaponMgr.h"
#include "ioShuffleRoomMgr.h"
#include "ioRankBattleManager.h"
#include "ioLadderTeamMgr.h"
#include "ioModeItem.h"

#include "Housing/ioUserBlockStorage.h"
#include "Housing/ioBlockManager.h"

#include "SkillDefine.h"

#include "GUI/GlobalWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/BriefingWnd.h"
#include "GUI/BossMonsterGaugeWnd.h"
#include "GUI/CenterHelpWnd.h"
#include "GUI/ioExpansionBox.h"
#include "GUI/FightClubWnd.h"
#include "GUI/BattleModeWnd.h"
#include "GUI/TowerDefWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/SoldierSelectWnd.h"
#include "GUI/HeroMainWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/ModeRankWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/SkillListWnd.h"
#include "GUI/TrainingFuncMenuWnd.h"
#include "GUI/DebugInfoWnd.h"
#include "GUI/AccessorySkillListWnd.h"
#include "GUI/ioSuccessionModeWnd.h"
#include "GUI/CountUpWnd.h"
#include "GUI/Practice/PracticeInfoWnd.h"
#include "GUI/FishingInventoryWnd.h"

ioHashString ioPlayMode::m_CurMapFileName;
CEncrypt<bool> ioPlayMode::m_bZeroHP = false;
CEncrypt<ModeType> ioPlayMode::m_ModeType = MT_NONE;

ioPlayMode::ioPlayMode( ioPlayStage *pCreator )
{
	m_pCreator = pCreator;
	m_ModeState = MS_READY;

	m_ScoreSound = SS_DEFAULT;
	m_PreScoreSound = SS_DEFAULT;
	
	m_WinTeamType = WTT_NONE;
	m_iCurRoundCount = 1;
	m_iMaxRoundCount = 4;
	
	m_iRedRoundWinCnt  = 0;
	m_iBlueRoundWinCnt = 0;

	m_fRoundVictoryRate = 0.0f;
	m_fRoundWinRate = 0.0f;

	m_dwLoadEventStartTime = 0;
	m_dwLoadEventDuration = 0;
	m_dwMapCameraSceneID = 0;
	m_dwRoundDuration = 0;
	m_dwReadyStartTime = 0;
	m_dwPlayStartTime = 0;
	m_dwResultStartTime = 0;
	m_dwCurResultShowDuration = 0;

	m_dwShowResultWndTime = 0;
	m_dwRoundResultDuration = 0;
	m_dwFinalRoundResultDuration = 0;

	m_fDropZoneDamage = FLOAT100;
	m_fDropZoneMinDamage = 0.0f;
	m_fDropZoneMaxDamage = 200.0f;

	m_iModeMapIndex = 0;

	m_iSelectCharacter = 0;
	m_iBluePosArray = 0;
	m_iRedPosArray = 1;

	m_pGuideArrow = NULL;
	m_pPushCursor = NULL;

	m_bRoundSetEnd = false;
	m_bCharDieClearByRound = false;
	m_bSafetyLevelRoom = false;

	m_dwCurRoundDuration = 0;

	m_dwStartSoldierWndTime = 0;
	m_dwShowSoldierWndTime = 0;

	m_bFirstSelect = true;
	m_dwCurRevivalTime = 0;

	m_dwCheckWarningTime = 0;
	m_fScoreWarningRate = FLOAT05;

	m_dwModeSoundTime = 0;

	m_dwRandomSeed = 0;
	
	m_vRoundHistory.clear();

	m_pBlindUp = NULL;
	m_pBlindDown = NULL;
	m_pRevivalGaugeBack = NULL;
	m_pRevivalGauge = NULL;
	m_pTextRevival = NULL;
	m_pTextRoundDelay = NULL;

	m_fUpBlindHeight = 50.0f;
	m_fBottomBlindHeight = 51.0f;
	m_fCurBlindHeightRate = 0.0f;

	m_bBlindRender       = true;
	m_dwSetBlindModeTime = 0;
	m_dwEndBlindModeStartTime = 0;
	m_dwBlindSlerpTime = 333;

	m_fRandomRange = 200.0f;
	m_bScoreWarning = false;

	m_bZeroHP = false;

	m_vEndLookAt = ioMath::VEC3_ZERO;
	m_fEndHeight = 0.0f;
	m_fEndDistance = 0.0f;
	m_fEndFov = 0.0f;

	m_fKORecoverRate = 0.0f;

	m_bViewRoundState = false;
	m_bReserveMyPositionEvent = false;

	m_iKillDeathPoint = 10;

	m_RoundInfoList.clear();

	m_fMaxHPRate = FLOAT1;

	m_bResultCameraChange = false;

	m_dwCurAutoStartAlarmTime = 0;
	m_dwAutoStartAlarmTime    = 0;
	m_iKillShakeCamera = 0;

	m_bEnterOneCheckQuest = true;

	m_bMonsterVsMonsterCollision = true;

	m_bProtoTypeMode = false;
	m_bTeamAttackMode= false;
	m_bTeamDamageMode= false;
	m_fDropDamageMapRate = FLOAT1;

	m_bModeCamera			= false;
	m_fModeCameraFov		= 0.0f;
	m_fModeCameraHeight		= 0.0f;
	m_fModeCameraDistance	= 0.0f;
	m_fModeCameraLookAtHeightOffset	= 0.0f;
	m_dwModeCameraSlerpTime	= 0.0f;
	m_fModeCameraDistanceLimit = 0.0f;

	m_bShuffleBonusMode = false;

	m_bIsCanCreateDummyType = true;
	m_iRoundEndRemainUserCnt = 0;

	for( int i=0; i < MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; ++i )
		m_vModeGrowthRate[i] = 1.0f;
}

ioPlayMode::~ioPlayMode()
{
	m_bZeroHP = false;
	ClearMode();

	m_ModePathMap.clear();
}

void ioPlayMode::ClearMode()
{
	SAFEDELETE( m_pGuideArrow );

	SAFEDELETE( m_pBlindUp );
	SAFEDELETE( m_pBlindDown );
	SAFEDELETE( m_pRevivalGaugeBack );
	SAFEDELETE( m_pRevivalGauge );
	SAFEDELETE( m_pTextRevival );
	SAFEDELETE( m_pTextRoundDelay );

	SAFEDELETE( m_pPushCursor );

	DamageTableMap::iterator iCreate = m_DamageTableMap.begin();
	for(; iCreate != m_DamageTableMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_DamageTableMap.clear();

	m_vStartPos.clear();
	m_vRoundHistory.clear();
	m_vContributeTable.clear();
	m_vContributeAddPointItemCode.clear();
	m_RoundInfoList.clear();
	m_DisableSkillList.clear();

	ClearGrowthLevel();

	g_SoundMgr.StopStream();
}


void ioPlayMode::SetModeCreateValueSet()
{
	m_bIsCanCreateDummyType = true;
}

void ioPlayMode::LoadModeINIPATH()
{
	m_ModePathMap.clear();

	char szBuf[MAX_PATH]="";
	char szKey[MAX_PATH]="";

	ioINILoader_e kLoader( "config/sp2_mode.ini" );
	kLoader.SetTitle_e( "common_info" );

	int iModePathCnt = kLoader.LoadInt_e( "max_mode_cnt", 0 );
	int iModeType = 0;
	for( int i=0; i < iModePathCnt; ++i )
	{
		wsprintf_e( szKey, "mode%d_type", i+1 );
		iModeType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "mode%d_path", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ModePathMap.insert( ModePathMap::value_type(iModeType, szBuf) );
	}
}

void ioPlayMode::LoadMap()
{
	LoadModeINIPATH();

	const char *pMapINIFile = GetModeMapINIFileName( m_ModeType,
													 m_iModeSubNum,
													 m_iModeMapIndex );

	ioINILoader kLoader( pMapINIFile );

	m_pCreator->LoadMap( kLoader );
	m_vMapCenter = m_pCreator->m_vMapCenter;

	kLoader.SetTitle_e( "drop_damage" );
	m_fDropDamageMapRate = kLoader.LoadFloat_e( "drop_damage_rate", FLOAT1 );

	kLoader.SetTitle_e( "team_start" );

	if( P2PNetwork::IsNetworkPlaying() )
	{
		int iMaxStartPos = kLoader.LoadFloat_e( "max_start_pos", 0 );

		char szStartX[MAX_PATH];
		char szStartZ[MAX_PATH];

		m_vStartPos.clear();
		m_vStartPos.reserve( iMaxStartPos );
		for( int i=0 ; i<iMaxStartPos; i++ )
		{
			wsprintf_e(szStartX, "start%d_x", i+1);
			wsprintf_e(szStartZ, "start%d_z", i+1);

			D3DXVECTOR3 vStartPos;
			vStartPos.x  = kLoader.LoadFloat( szStartX, 0.0f );
			vStartPos.z  = kLoader.LoadFloat( szStartZ, 0.0f );
			m_vStartPos.push_back(vStartPos);
		}
	}
	else
	{
		m_vStartPos.clear();
		m_vStartPos.reserve( 2 );

		D3DXVECTOR3 vStartPos;
		vStartPos.x  = kLoader.LoadFloat_e( "single_x", 0.0f );
		vStartPos.z  = kLoader.LoadFloat_e( "single_z", 0.0f );
		m_vStartPos.push_back(vStartPos);
		m_iRedPosArray = 0;

		vStartPos.x = kLoader.LoadFloat_e( "single_x", 0.0f );
		vStartPos.z = kLoader.LoadFloat_e( "single_z", 0.0f );
		m_vStartPos.push_back(vStartPos);
		m_iBluePosArray = 1;
	}

	kLoader.SetTitle_e( "minimap" );

	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( pMiniMap )
	{
		pMiniMap->InitMapInfo( kLoader, m_pCreator->GetWorldMapBox(), this );
	}

	// PassiveBuff List
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	kLoader.SetTitle_e( "map_passive_buff" );

	m_vMapPassiveBuffList.clear();

	int iBuffCnt = kLoader.LoadInt_e( "passive_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_vMapPassiveBuffList.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "passive_buff%d", i+1 );

			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			MapBuffInfo info;
			info.m_szBuffName = szBuf;
			m_vMapPassiveBuffList.push_back( info );
		}
	}

	kLoader.SetTitle_e( "event_scene" );
	m_dwMapCameraSceneID = kLoader.LoadInt_e( "camera_event_id", 0 );

	LoadMapAreaWeapon( kLoader );
	LoadMapDummyObject( kLoader );

	kLoader.SetTitle_e( "collision" );
	m_bMonsterVsMonsterCollision = kLoader.LoadBool_e( "monster_vs_monster", true );

	kLoader.SetTitle_e( "proto_type" );
	m_bProtoTypeMode = kLoader.LoadBool_e( "proto_mode", false );
	m_bTeamAttackMode= kLoader.LoadBool_e( "team_attack", false );
	m_bTeamDamageMode= kLoader.LoadBool_e( "team_damage", false );
	if( m_bProtoTypeMode )
	{
		LoadNPC();
	}
}

void ioPlayMode::LoadPushStructs()
{
	if( P2PNetwork::IsNetworkPlaying() )
		return;

	g_PushStructListMgr.DestroyStructList();

	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kLoader.SetTitle_e( "interaction_object_list" );

	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	ioINILoader kPushLoader = g_PushStructListMgr.GetINILoader();
	int iPushStructCnt = kLoader.LoadInt_e( "push_struct_cnt", 0 );

	for( int i=0; i<iPushStructCnt; i++ )
	{
		wsprintf_e( szTitle, "push_struct%d_num", i+1 );
		int iNum = kLoader.LoadInt( szTitle, 0 );

		D3DXVECTOR3 vPos;

		wsprintf_e( szTitle, "push_struct%d_pos_x", i+1 );
		vPos.x = kLoader.LoadFloat( szTitle, 0.0f );

		wsprintf_e( szTitle, "push_struct%d_pos_y", i+1 );
		vPos.y = kLoader.LoadFloat( szTitle, 0.0f );

		wsprintf_e( szTitle, "push_struct%d_pos_z", i+1 );
		vPos.z = kLoader.LoadFloat( szTitle, 0.0f );

		wsprintf_e( szTitle, "push_struct%d", iNum );
		kPushLoader.SetTitle( szTitle );

		kPushLoader.LoadString_e( "push_struct", "", szBuf, MAX_PATH );

		ioPushStruct* pPush = g_PushStructListMgr.CreatePushStruct( szBuf, this );
		if( !pPush ) continue;

		pPush->InitResource();
		pPush->LoadProperty( kPushLoader );

		D3DXVECTOR3 vScale;
		vScale.x = kPushLoader.LoadFloat_e( "scale_x", FLOAT05 );
		vScale.y = kPushLoader.LoadFloat_e( "scale_y", FLOAT05 );
		vScale.z = kPushLoader.LoadFloat_e( "scale_z", FLOAT05 );

		if( vPos.y < FLOAT1 )	// 세팅되지 않았다.
		{
			vPos.y = m_pCreator->GetMapHeight( vPos.x, vPos.z, pPush, false );
		}

		DWORD dwSeed = timeGetTime();
		pPush->InitPushStructInfo( iNum, i + 1, 0, dwSeed, 0, vPos, vScale );
		m_pCreator->AddGameEntity( pPush );
	}
}

void ioPlayMode::LoadMachineStruct()
{
	if( P2PNetwork::IsNetworkPlaying() )
		return;

	g_MachineStructMgr.DestroyMachineStructList();

	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kLoader.SetTitle_e( "interaction_object_list" );

	char szTitle[MAX_PATH];

	ioINILoader kStructLoader = g_MachineStructMgr.GetINILoader();
	int iMachineCnt = kLoader.LoadInt_e( "machine_struct_cnt", 0 );

	for( int i=0; i<iMachineCnt; i++ )
	{
		wsprintf_e( szTitle, "machine_struct%d_num", i+1 );
		int iNum = kLoader.LoadInt( szTitle, 0 );

		D3DXVECTOR3 vPos;

		wsprintf_e( szTitle, "machine_struct%d_pos_x", i+1 );
		vPos.x = kLoader.LoadFloat( szTitle, 0.0f );

		wsprintf_e( szTitle, "machine_struct%d_pos_y", i+1 );
		vPos.y = kLoader.LoadFloat( szTitle, 0.0f );

		wsprintf_e( szTitle, "machine_struct%d_pos_z", i+1 );
		vPos.z = kLoader.LoadFloat( szTitle, 0.0f );

		wsprintf_e( szTitle, "machine_struct%d_angle", i+1 );
		float fAngle = kLoader.LoadFloat( szTitle, 0.0f );

		//
		DWORD dwSeed = timeGetTime();

		wsprintf_e( szTitle, "machine_struct%d", iNum );
		kStructLoader.SetTitle( szTitle );

		D3DXVECTOR3 vScale;
		vScale.x = kStructLoader.LoadFloat_e( "scale_x", FLOAT1 );
		vScale.y = kStructLoader.LoadFloat_e( "scale_y", FLOAT1 );
		vScale.z = kStructLoader.LoadFloat_e( "scale_z", FLOAT1 );

		D3DXQUATERNION qtRot;
		fAngle = ioMath::ArrangeHead( fAngle );
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );

		m_pCreator->CreateMachineStruct( iNum, i+1, vPos, vScale, qtRot, 0, dwSeed );
	}
}

void ioPlayMode::LoadObjectItems()
{
	if( P2PNetwork::IsNetworkPlaying() )
		return;

	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );

	char szBuf[MAX_PATH], szTitle[MAX_PATH];
	kLoader.SetTitle_e( "interaction_object_list" );

	int iObjectItemCnt = kLoader.LoadInt_e( "object_item_cnt", 0 );
	for( int i=0; i<iObjectItemCnt; i++ )
	{
		wsprintf_e( szTitle, "object_item%d_name", i+1 );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioHashString szName = szBuf;

		ioItem *pItem = m_pCreator->CreateItem( szName );
		if( !pItem )	continue;

		pItem->ReLoadProperty();

		D3DXVECTOR3 vPos;

		wsprintf_e( szTitle, "object_item%d_pos_x", i+1 );
		vPos.x = kLoader.LoadFloat( szTitle, 0.0f );

		wsprintf_e( szTitle, "object_item%d_pos_z", i+1 );
		vPos.z = kLoader.LoadFloat( szTitle, 0.0f );

		vPos.y = 0.0f;

		ioFieldItem *pField = m_pCreator->CreateFieldItem( pItem );
		if( pField )
		{
			pField->SetWorldPositionByServerSide( vPos );
			pField->SetDropState( pField->GetWorldPosition() );
			pField->CreateItemEffect();
		}
	}
}

void ioPlayMode::LoadFuntionalObjects()
{
	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );

	int iTotalCount = kLoader.LoadInt_e( "function_object_info", "count", 0 );
	if( iTotalCount <= 0 )	return;

	char szTitle[MAX_PATH];
	for( int i=0 ; i<iTotalCount ; i++ )
	{
		wsprintf_e( szTitle, "function_object%d", i+1 );
		kLoader.SetTitle( szTitle );

		ioPlayEntity *pEntity = m_pCreator->CreateWorldEntity( kLoader, i+1 );
		if( !pEntity )	continue;

		D3DXVECTOR3 vPos;
		vPos.x = kLoader.LoadFloat_e( "pos_x", 0.0f );
		vPos.y = kLoader.LoadFloat_e( "pos_y", 0.0f );
		vPos.z = kLoader.LoadFloat_e( "pos_z", 0.0f );
		pEntity->SetWorldPosition( vPos );

		D3DXVECTOR3 vScale;
		vScale.x = kLoader.LoadFloat_e( "scale_x", FLOAT1 );
		vScale.y = kLoader.LoadFloat_e( "scale_y", FLOAT1 );
		vScale.z = kLoader.LoadFloat_e( "scale_z", FLOAT1 );
		pEntity->SetWorldScale( vScale );
	}
}

void ioPlayMode::LoadMapAreaWeapon( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "map_area_weapon" );

	m_MapAreaWeaponList.clear();

	int iCnt = rkLoader.LoadInt_e( "map_area_weapon_cnt", 0 );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	for( int i=0; i < iCnt; ++i )
	{
		MapAreaWeapon kAreaWeapon;
		
		wsprintf_e( szKey, "map_area_weapon%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kAreaWeapon.m_AreaWeaponName = szBuf;

		wsprintf_e( szKey, "map_area_weapon%d_time", i+1 );
		kAreaWeapon.m_dwCreateTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "map_area_weapon%d_angle", i+1 );
		kAreaWeapon.m_fCreateAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "map_area_weapon%d_onetime_use", i+1 );
		kAreaWeapon.m_bOneTimeCreate = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "map_area_weapon%d_pos_x", i+1 );
		kAreaWeapon.m_vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "map_area_weapon%d_pos_y", i+1 );
		kAreaWeapon.m_vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "map_area_weapon%d_pos_z", i+1 );
		kAreaWeapon.m_vPos.z = rkLoader.LoadFloat( szKey, 0.0f );

		m_MapAreaWeaponList.push_back( kAreaWeapon );
	}
}


void ioPlayMode::LoadMapDummyObject( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "map_dummy" );
	m_MapDummyObjectList.clear();

	int nCnt = rkLoader.LoadInt_e( "dummy_cnt", 0 );
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	for( int i=0; i < nCnt; ++i )
	{
		MapDummyObject kMapDummy;
		wsprintf_e( szKey, "dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kMapDummy.m_stName = szBuf;

		wsprintf_e( szKey, "dummy%d_pos_x", i+1 );
		kMapDummy.m_vPos.x = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "dummy%d_pos_y", i+1 );
		kMapDummy.m_vPos.y = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "dummy%d_pos_z", i+1 );
		kMapDummy.m_vPos.z = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "dummy%d_angle", i+1 );
		kMapDummy.m_fAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_MapDummyObjectList.push_back( kMapDummy );
	}

}


void ioPlayMode::LoadModeCamera( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "camera" );
	m_bModeCamera = rkLoader.LoadBool_e( "mode_camera_use", false );
	if( m_bModeCamera )
	{
		m_fModeCameraFov		= rkLoader.LoadFloat( "projection", "fov", 45.0f );
		m_fModeCameraHeight		= rkLoader.LoadFloat_e( "default_height", 400.0f );
		m_fModeCameraDistance	= rkLoader.LoadFloat_e( "default_distance", FLOAT1000 );
		m_fModeCameraLookAtHeightOffset	= rkLoader.LoadFloat_e( "lookat_height_offset", 200.0f );
		m_fModeCameraDistanceLimit = rkLoader.LoadFloat_e( "distance_limit", 0.0f );
		m_dwModeCameraSlerpTime	= rkLoader.LoadInt_e( "camera_slerp_time", 2000 );
	}
}

void ioPlayMode::LoadCharacters()
{
	if( P2PNetwork::IsNetworkPlaying() )
		return;

	const int iMaxCharIdx = 4;
	ioUserKeyInput::UseKeyValue kKeyList[iMaxCharIdx+1];
	kKeyList[0].SetValue(KEY_Z,KEY_C,KEY_S,KEY_X,
						 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                 KEY_W,KEY_Q,KEY_E,
		                 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kKeyList[1].SetValue(KEY_COMMA,KEY_SLASH,KEY_L,KEY_PERIOD,
		                 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                 KEY_O,KEY_I,KEY_P,
		                 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kKeyList[2].SetValue(KEY_LEFTARROW, KEY_RIGHTARROW, KEY_UPARROW, KEY_DOWNARROW,
		                 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                 KEY_END, KEY_DEL, KEY_PGDN,
		                 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kKeyList[3].SetValue(KEY_NUM1, KEY_NUM3, KEY_NUM5, KEY_NUM2, 
		                 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                 KEY_NUM8, KEY_NUM7, KEY_NUM9,
		                 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kKeyList[4].SetValue(KEY_LEFTARROW, KEY_RIGHTARROW, KEY_UPARROW, KEY_DOWNARROW,
		                 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                 KEY_S,KEY_A,KEY_D,
		                 KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);


	ioUserKeyInput::UseKeyValue kSecondKeyList[iMaxCharIdx+1];
	kSecondKeyList[0].SetValue(KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
							   KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
							   KEY_2, KEY_1, KEY_3,
		                       KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kSecondKeyList[1].SetValue(KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                       KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                       KEY_9, KEY_8, KEY_0,
		                       KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kSecondKeyList[2].SetValue(KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                       KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                       KEY_HOME, KEY_INS, KEY_PGUP,
		                       KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kSecondKeyList[3].SetValue(KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                       KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                       KEY_NUMSLASH, KEY_NUMLOCK, KEY_NUMSTAR,
		                       KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kSecondKeyList[4].SetValue(KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                       KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
		                       KEY_W,KEY_Q,KEY_E,
		                       KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);

	ioUserKeyInput::UseKeyValue kJoyKeyList[iMaxCharIdx+1];
	kJoyKeyList[0].SetValue(KEY_LSTICKLEFT, KEY_LSTICKRIGHT, KEY_LSTICKUP, KEY_LSTICKDOWN,
							KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
							KEY_JOYBUTN1, KEY_JOYBUTN3, KEY_JOYBUTN2,
							KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL);
	kJoyKeyList[1].SetValue(KEY_COMMA,KEY_SLASH,KEY_L,KEY_PERIOD,
							KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
							KEY_O,KEY_I,KEY_P,
							KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kJoyKeyList[2].SetValue(KEY_LEFTARROW, KEY_RIGHTARROW, KEY_UPARROW, KEY_DOWNARROW,
							KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
							KEY_END, KEY_DEL, KEY_PGDN,
							KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kJoyKeyList[3].SetValue(KEY_NUM1, KEY_NUM3, KEY_NUM5, KEY_NUM2, 
							KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
							KEY_NUM8, KEY_NUM7, KEY_NUM9,
							KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);
	kJoyKeyList[4].SetValue(KEY_LSTICKLEFT, KEY_LSTICKRIGHT, KEY_LSTICKUP, KEY_LSTICKDOWN,
							KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,
							KEY_NUM8, KEY_NUM7, KEY_NUM9,
							KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL);

	int iMaxLoop = m_pCreator->m_iMaxSinglePlayer;
	
	ioINILoader &rkLoader = Help::GetCharLoader();
	for( int i=0 ; i<iMaxLoop ; i++ )
	{
		int iCharIdx = i % iMaxCharIdx;

		char szTitle[MAX_PATH];
		wsprintf_e( szTitle, "character%d", iCharIdx+1 );

		rkLoader.SetTitle( szTitle );

		char szModelName[MAX_PATH];
		rkLoader.LoadString_e( "model_file", "", szModelName, MAX_PATH );

		ioBaseChar *pChar = m_pCreator->CreateBaseChar( szModelName );
		if( !pChar )
			continue;

		char szCharName[MAX_PATH];
		wsprintf_e( szCharName, "character%d", i+1);

		pChar->SetCharName( szCharName );
		pChar->InitINIValue( rkLoader );
		pChar->SetIndex( iCharIdx+1 );

		if( i == 0 )
			pChar->InitRaceInfo( true );
		else
			pChar->InitRaceInfo( false );

		TeamType eTeam = TEAM_RED;
		if( rkLoader.LoadBool_e( "blue_team", false ) )
			eTeam = TEAM_BLUE;

		D3DXVECTOR3 vPos = GetStartPos( eTeam );
		pChar->SetWorldPosition( vPos );
		pChar->SetTeam( eTeam );

		if( i == 0 )
		{
			pChar->SetIsOwnerChar( true );
		}

		// kaseton : to control char controller
		int iControl = rkLoader.LoadInt_e( "use_control", iCharIdx );

		pChar->SetUpKeyValue( kKeyList[iControl]);
		pChar->SetUpSecondKeyValue( kSecondKeyList[iControl] );
		pChar->SetUpJoyKeyValue( kJoyKeyList[iControl]);

		DWORD dwItemCode[MAX_INVENTORY];
		dwItemCode[0] = rkLoader.LoadInt_e( "default_armor", 0 );
		dwItemCode[1] = rkLoader.LoadInt_e( "default_weapon", 0 );
		dwItemCode[2] = rkLoader.LoadInt_e( "default_helmet", 0 );
		dwItemCode[3] = rkLoader.LoadInt_e( "default_cloak", 0 );

		// VoiceType Set
		int iVoiceType = 0;
		if( dwItemCode[1] > 0 )
		{
			const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( dwItemCode[1] - 1 );
			if( pSetInfo )
				iVoiceType = pSetInfo->GetVoiceType();

			pChar->SetVoiceType( iVoiceType );
		}

		for( int j=0 ; j<MAX_INVENTORY ; j++ )
		{
			if( dwItemCode[j] == 0 )
				continue;

			ioItem *pItem = m_pCreator->CreateItem( dwItemCode[j] );
			if( pItem )
			{
				pItem->ReLoadProperty();
				pItem->SetOwnerName( pChar->GetCharName() );
				pChar->EquipItem( pItem );
				pItem->Initialize( pChar, m_pCreator );
			}
		}

		DWORD dwAccessory[MAX_ACC_INVENTORY];
		dwAccessory[0] = rkLoader.LoadInt_e( "default_ring", 0 );
		dwAccessory[1] = rkLoader.LoadInt_e( "default_necklace", 0 );
		dwAccessory[2] = rkLoader.LoadInt_e( "default_bracelet", 0 );
		for( int j=0 ; j<MAX_ACC_INVENTORY ; j++ )
		{
			if( dwAccessory[j] == 0 )
				continue;

			ioItem *pItem = m_pCreator->CreateItem( dwAccessory[j] );
			if( pItem )
			{
				pItem->ReLoadProperty();
				pItem->SetOwnerName( pChar->GetCharName() );
				pItem->InitializeBeforeEquip( pChar, m_pCreator );
				pChar->EquipItem( pItem );
				pItem->Initialize( pChar, m_pCreator );
			}
		}

		pChar->SetState( CS_DELAY );
		pChar->FillMaxHP();
		AddNewRecord( pChar );
	}
}

void ioPlayMode::LoadNPC()
{
	ioINILoader kMapLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kMapLoader.SetTitle_e( "FieldNpc" );
	int iMaxNpc = kMapLoader.LoadInt_e( "MaxNpc", 0 );
	for(int i = 0;i < iMaxNpc;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "NpcCode_%d", i + 1 );

		ioNpcChar *pChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, kMapLoader.LoadInt( szKey, 0 ), m_pCreator, this );
		if( pChar == NULL ) continue;
		
		char szBuf[MAX_PATH] = "";
		sprintf_e( szKey, "NpcName_%d", i + 1 );
		char szNpcName[MAX_PATH] = "";
		kMapLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sprintf_e( szNpcName, " %s ", szBuf );        // 좌우에 공백을 넣어서 유저 닉네임과 중복되지 않도록 한다.
		pChar->SetCharName( szNpcName );

 		// 기타 세팅
		if( GetModeType() == MT_MYROOM )
		{
			// 본부라서 팀과 포지션이 따로 설정해야한다. 일단 임시로 개발자K와 같이 설정
			pChar->SetTeam( (TeamType)((m_vStartPos.size() - 1) + TEAM_PRIVATE_1) );
			D3DXVECTOR3 vStartPos = GetStartPos( pChar->GetTeam() );
			pChar->SetWorldPosition( vStartPos );
			Help::SetDeveloperKCharName( szNpcName );
		}
		else if( m_bProtoTypeMode )
		{
			sprintf_e( szKey, "NpcTeam_%d", i + 1 );
			int iTeamType = kMapLoader.LoadInt( szKey, TEAM_NONE );
			pChar->SetTeam( (TeamType)iTeamType );

			D3DXVECTOR3 vStartPos = GetStartPos( pChar->GetTeam() );
			pChar->SetWorldPosition( vStartPos );
		}
		else 
		{
			sprintf_e( szKey, "NpcTeam_%d", i + 1 );
			int iTeamType = kMapLoader.LoadInt( szKey, TEAM_NONE );
			pChar->SetTeam( (TeamType)iTeamType );
		}

		pChar->FillMaxHP();

		if( GetModeType() == MT_MYROOM || !P2PNetwork::IsNetworkPlaying() )
		{
			if( GetModeState() == MS_PLAY )
				pChar->SetCreateMotionState();
			else
				pChar->SetReadyState();
		}
		AddNewRecord( pChar );	
	}
}

void ioPlayMode::LoadGUIValues()
{
	ioINILoader_e kLoader( "config/sp2_gui.ini" );

	kLoader.SetTitle_e( "result" );
	m_dwShowResultWndTime = kLoader.LoadInt_e( "show_result_wnd_time", 400 );
	m_dwRoundResultDuration = kLoader.LoadInt_e( "result_total_time", 10000 );
	m_dwFinalRoundResultDuration = kLoader.LoadInt_e( "final_round_total_time", 20000 );

	// For Blind
	kLoader.SetTitle_e( "blind" );
	m_dwBlindSlerpTime = kLoader.LoadInt_e( "blind_speed", 333 );

	// Sound
	kLoader.SetFileName_e( "config/sp2_sound.ini" );
	kLoader.SetTitle_e( "sound_file_name" );

	char szBuf[MAX_PATH], szBuf2[MAX_PATH];
	kLoader.LoadString_e( "score_warning_sound", "", szBuf, MAX_PATH );
	m_ScoreWarningSound = szBuf;
	kLoader.LoadString_e( "firest_score_sound", "", szBuf, MAX_PATH );
	m_FirstScoreSound = szBuf;
	kLoader.LoadString_e( "equal_score_sound", "", szBuf, MAX_PATH );
	m_EqualScoreSound = szBuf;
	kLoader.LoadString_e( "reversal_score_sound", "", szBuf, MAX_PATH );
	m_ReversalScoreSound = szBuf;
	kLoader.LoadString_e( "add_score_sound", "", szBuf, MAX_PATH );
	m_AddScoreSound = szBuf;
	kLoader.LoadString_e( "last_score_sound", "", szBuf, MAX_PATH );
	m_LastScoreSound = szBuf;

	kLoader.LoadString_e( "lose_score_sound", "", szBuf, MAX_PATH );
	m_LoseScoreSound = szBuf;

	kLoader.LoadString_e( "blind_move_sound", "", szBuf, MAX_PATH );
	m_BlindMoveSound = szBuf;

	// 모드별로 사운드 로드
	kLoader.SetTitle_e( "mode_type_common" );
	sprintf_e( szBuf, "mode_type%d", (int)GetModeType() );
	int iModeTypeValue = kLoader.LoadInt( szBuf, 0 );
	sprintf_e( szBuf, "mode_select_%d", iModeTypeValue );
	kLoader.SetTitle( szBuf );
	m_dwStartWarningTime = kLoader.LoadInt_e( "warning_start_time", 10000 );
	int iMaxWarningSound = m_dwStartWarningTime / 1000;
	for(int i = 0;i < iMaxWarningSound;i++)
	{
		sprintf_e( szBuf, "warning_sound_%d", i + 1 );
		kLoader.LoadString( szBuf, "", szBuf2, MAX_PATH );
		m_WarningSound.push_back( szBuf2 );
	}
	kLoader.LoadString_e( "warning_sound_def", "", szBuf, MAX_PATH );
	m_WarningSoundDef = szBuf;
	kLoader.LoadString_e( "timeout_sound", "", szBuf, MAX_PATH );
	m_TimeOutSound = szBuf;

	//
	kLoader.SetTitle_e( "mode_map_sound" );

	m_dwModeSoundDelayTime = kLoader.LoadInt_e( "mode_map_sound_delay", 0 );

	memset( szBuf, 0, MAX_PATH );
	memset( szBuf2, 0, MAX_PATH );

	switch( GetModeType() )
	{
	case MT_TOWER_DEFENSE:
		wsprintf_e( szBuf, "towerdef_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "towerdef_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_DARK_XMAS:
		wsprintf_e( szBuf, "darkxmas_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "darkxmas_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_FIRE_TEMPLE:
		wsprintf_e( szBuf, "firetemple_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "firetemple_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_SYMBOL:
		wsprintf_e( szBuf, "symbol_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "symbol_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_CATCH:
		wsprintf_e( szBuf, "catch_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "catch_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_CATCH_RUNNINGMAN:
		wsprintf_e( szBuf, "catch_runningman_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "catch_runningman_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_KING:
		wsprintf_e( szBuf, "hiddenking_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "hiddenking_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_TRAINING:
		wsprintf_e( szBuf, "training_mode%d_map_sound%d",m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "training_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_SURVIVAL:
		wsprintf_e( szBuf, "survival_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "survival_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_FIGHT_CLUB:
		wsprintf_e( szBuf, "fight_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "fight_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_TEAM_SURVIVAL:
		wsprintf_e( szBuf, "team_survival_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "team_survival_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_TEAM_SURVIVAL_AI:
		wsprintf_e( szBuf, "team_survivalAI_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "team_survivalAI_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_BOSS:
		wsprintf_e( szBuf, "boss_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "boss_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_MONSTER_SURVIVAL:
		wsprintf_e( szBuf, "monster_survival_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "monster_survival_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_FOOTBALL:
		wsprintf_e( szBuf, "football_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "football_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_HEROMATCH:
		wsprintf_e( szBuf, "heromatch_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "heromatch_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_GANGSI:
		wsprintf_e( szBuf, "gangsi_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "gangsi_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_DUNGEON_A:
		wsprintf_e( szBuf, "dungeon_a_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "dungeon_a_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_HEADQUARTERS:
		wsprintf_e( szBuf, "headquarters_mode%d_map_sound%d",m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "headquarters_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_DOUBLE_CROWN:
		wsprintf_e( szBuf, "double_crown_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "double_crown_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_SHUFFLE_BONUS:
		wsprintf_e( szBuf, "today_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "today_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_FACTORY:
		wsprintf_e( szBuf, "factory_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "factory_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_HOUSE:
		wsprintf_e( szBuf, "house_mode%d_map_sound%d",m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "house_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_RAID:
		wsprintf_e( szBuf, "raid_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "raid_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_SUCCESSION:
		wsprintf_e( szBuf, "succession_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "succession_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_PRACTICE:
		wsprintf_e( szBuf, "practice_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "practice_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_FLAG_CAPTURE:
		wsprintf_e( szBuf, "flag_capture_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "flag_capture_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_ARENA:
		wsprintf_e( szBuf, "arena_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "arena_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_BATTLE:
		wsprintf_e( szBuf, "team_survival_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "team_survival_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	case MT_FARMING:
		wsprintf_e( szBuf, "farming_mode%d_map_sound%d", m_iModeSubNum, m_iModeMapIndex );
		wsprintf_e( szBuf2, "farming_mode%d_map_sound%d_type", m_iModeSubNum, m_iModeMapIndex );
		break;
	}

	if( strcmp( szBuf, "" ) != 0 )
	{
		char szSoundName[MAX_PATH];
		kLoader.LoadString( szBuf, "", szSoundName, MAX_PATH );
		m_ModeMapSound = szSoundName;
	}

	m_ModeMapSoundType = 0;
	if( strcmp( szBuf2, "" ) != 0 )
		m_ModeMapSoundType = kLoader.LoadInt( szBuf2, 0 );
}

void ioPlayMode::LoadModeResource( ioINILoader &rkLoader )
{
	// 가장 마지막에 로드하는 XML
	GlobalChatWnd *pGlobalChat = dynamic_cast<GlobalChatWnd*>( g_GUIMgr.AddWnd( "XML/globalchatwnd.xml" ) );
	if( pGlobalChat )
	{
		pGlobalChat->SetPlayStage( m_pCreator );
		pGlobalChat->ShowWnd();
	}
	g_GUIMgr.AddWnd( "XML/chatoptionwnd.xml" );

	g_GUIMgr.AddWnd( "XML/heromatchversusscenewnd.xml" );

	rkLoader.SetTitle_e( "drop_zone" );
	m_fDropZoneDamage = rkLoader.LoadFloat_e( "drop_damage", FLOAT100 );
	m_fDropZoneMinDamage = rkLoader.LoadFloat_e( "drop_min_damage", 0.0f );
	m_fDropZoneMaxDamage = rkLoader.LoadFloat_e( "drop_max_damage", 200.0f );
	if( m_fDropZoneDamage > m_fDropZoneMaxDamage )    //
		m_fDropZoneMaxDamage = 200.0f;

	m_fDropProtectRate = rkLoader.LoadFloat_e( "drop_protect_rate", FLOAT05 );

	rkLoader.SetTitle_e( "fall" );
	m_fFallDamage = rkLoader.LoadFloat_e( "fall_damage", FLOAT100 );
	m_fFallMinDamage = rkLoader.LoadFloat_e( "fall_min_damage", 0.0f );
	m_fFallMaxDamage = rkLoader.LoadFloat_e( "fall_max_damage", 200.0f );

	m_fNpcFallDamage = rkLoader.LoadFloat_e( "npc_fall_damage", m_fFallDamage );   // 세팅안할것을 대비하여 위의 값을 디폴트값으로 한다
	m_fNpcFallMinDamage = rkLoader.LoadFloat_e( "npc_fall_min_damage", m_fFallMinDamage );
	m_fNpcFallMaxDamage = rkLoader.LoadFloat_e( "npc_fall_max_damage", m_fFallMaxDamage );


	rkLoader.SetTitle_e( "round_info" );
	m_iMaxRoundCount = rkLoader.LoadInt_e( "max_round_count", 6 );

	rkLoader.SetTitle_e( "common_info" );
	m_fRoundVictoryRate = rkLoader.LoadFloat_e( "round_victory_rate", 0.0f );
	m_fRoundWinRate = rkLoader.LoadFloat_e( "round_win_rate", 0.0f );
	m_dwLoadEventDuration = rkLoader.LoadInt_e( "mode_event_duration", 10000 );
	m_bCharDieClearByRound = rkLoader.LoadBool_e( "die_clear_by_round", true );
	m_dwResultCameraDuration = rkLoader.LoadInt_e( "result_camera_duration", 0 );

	m_pBlindUp = g_UIImageSetMgr.CreateImage( "BaseImage001", "strip_black100" );
	m_pBlindDown = g_UIImageSetMgr.CreateImage( "BaseImage001", "strip_black100" );
	m_pRevivalGaugeBack = g_GUIMgr.CreateFrame( "revival_gauge_bg" );
	m_pRevivalGauge = g_GUIMgr.CreateFrame( "revival_gauge_gauge" );
	m_pTextRevival = g_UIImageSetMgr.CreateImage( "BaseImage015", "revival_text_1" );
	m_pTextRoundDelay = g_UIImageSetMgr.CreateImage( "BaseImage015", "revival_text_2" );

	m_fRandomRange = rkLoader.LoadFloat_e( "revival_random_range", 200.0f );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "char_wound_extra_effect", "", szBuf, MAX_PATH );
	m_CharWoundExtraEffect = szBuf;

	m_iKillShakeCamera = rkLoader.LoadInt_e( "kill_shake_camera", 0 );

	SAFEDELETE( m_pGuideArrow );
	m_pGuideArrow = new ioModeGuideArrow( this );
	m_pGuideArrow->Initialize( rkLoader );

	m_dwEndSceneCameraEvent	  = rkLoader.LoadInt_e( "end_scene_camera_event", 0 );
	m_dwEndSceneCameraMoveTime= rkLoader.LoadInt_e( "end_scene_camera_move_time", 1 );
	m_dwEndSceneZoomTime      = rkLoader.LoadInt_e( "end_scene_camera_zoom_time", 1 );

	rkLoader.SetTitle_e( "record_info" );
	m_iKillDeathPoint = rkLoader.LoadInt_e( "kill_death_point", 10 );

	rkLoader.SetTitle_e( "char_hp_info" );
	m_fMaxHPRate = rkLoader.LoadFloat_e( "max_hp_rate", FLOAT1 );
	m_fRevivalHPRate = rkLoader.LoadFloat_e( "revival_hp_rate", FLOAT1 );

	m_pCreator->LoadModeCustomBlind( rkLoader );

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		m_vRoundHistory.clear();
		for(int i=0;i<m_iMaxRoundCount;i++)
		{
			RoundHistory rh;
			m_vRoundHistory.push_back( rh );
		}
	}

	BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>( g_GUIMgr.FindWnd( BRIEFING_WND ) );
	if( pBriefingWnd )
		pBriefingWnd->LoadResource( GetModeType(), g_App.GetPlazaModeType() );

	rkLoader.SetTitle_e( "character_change" );
	m_bCharacterChangeImmediately = rkLoader.LoadBool_e( "change_enable", false );
	m_dwChangeCoolTime = rkLoader.LoadInt_e( "change_cool_time", 0 );
}

void ioPlayMode::LoadContribute()
{
	m_vContributeTable.clear();
	m_vContributeAddPointItemCode.clear();
	
	ioINILoader_e kLoader( "config/sp2_contribute.ini" );
	kLoader.SetTitle_e( "main_info" );

	int i = 0;
	char szKey[MAX_PATH];
	int iMaxNum = kLoader.LoadInt_e( "Max_Num", MAX_PLAYER );
	for(i = 0;i < iMaxNum;i++)
	{
		sprintf_e( szKey, "Num_%d", i + 1 );
		m_vContributeTable.push_back( kLoader.LoadInt( szKey, 0 ) );
	}

	if( m_vContributeTable.size() < MAX_PLAYER )
	{
		LOG.PrintTimeAndLog( 0, "ERROR !! ContributeTable %d", m_vContributeTable.size() );
	}

	kLoader.SetTitle_e( "add_point" );
	iMaxNum = kLoader.LoadInt_e( "Max_Num", 0 );
	for(i = 0;i < iMaxNum;i++)
	{
		sprintf_e( szKey, "ItemCode_%d", i + 1 );
		m_vContributeAddPointItemCode.push_back( kLoader.LoadInt( szKey, 0 ) );
	}
}

void ioPlayMode::PlayExtendSound()
{

}

void ioPlayMode::LoadRevivalTime( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "revival_time" );

	m_dwDefaultRevivalTime = rkLoader.LoadInt_e( "default_time", 10000 );
	m_dwIncreaseRevivalTime = rkLoader.LoadInt_e( "increase_time", 5000 );
	m_dwMaxRevivalTime = rkLoader.LoadInt_e( "max_time", 30000 );
}

DWORD ioPlayMode::GetRevivalGapTime( int iRevivalCnt )
{
	DWORD dwRevivalTime;

	dwRevivalTime = m_dwDefaultRevivalTime + (m_dwIncreaseRevivalTime*iRevivalCnt);
	dwRevivalTime = min( m_dwMaxRevivalTime, dwRevivalTime );

	return dwRevivalTime;	
}

void ioPlayMode::StartEventScene( bool bFirst, DWORD dwEventSceneID  )
{
	Vector3Vec vEventScenePosList, vEventSceneOffsetList;
	if( dwEventSceneID == 0 )
		g_MapEventSceneMgr.GetMapEventScene( m_dwMapCameraSceneID, vEventScenePosList, vEventSceneOffsetList, m_dwLoadEventDuration );
	else
		g_MapEventSceneMgr.GetMapEventScene( dwEventSceneID, vEventScenePosList, vEventSceneOffsetList, m_dwLoadEventDuration );

	if( vEventScenePosList.size() != vEventSceneOffsetList.size() )
	{
		LOG.PrintTimeAndLog( 0, "StartEventScene : Pos Size %d != Offset Size %d Error !!", vEventScenePosList.size(), vEventSceneOffsetList.size() );
		return;
	}

	if( vEventScenePosList.empty() || vEventSceneOffsetList.empty() )
	{
		vEventScenePosList.push_back( m_vMapCenter );
		vEventSceneOffsetList.push_back( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
	}

	m_EventTimeMap.clear();
	m_EventTimeMapOffset.clear();

	m_dwLoadEventStartTime = REALGETTIME();
	AddEventSceneTime( m_dwLoadEventStartTime, vEventScenePosList.front(), vEventSceneOffsetList.front() );

	//Show Briefing Wnd
	BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
	if( pBriefingWnd && !pBriefingWnd->IsShow() )
	{
		pBriefingWnd->SetPlayStage( m_pCreator );
		if( !P2PNetwork::IsNetworkPlaying() )
			pBriefingWnd->SetStartBriefing( 1 );
		else
		{
			if( GetModeType() == MT_MONSTER_SURVIVAL || GetModeType() == MT_DUNGEON_A )
			{
				ioMonsterSurvivalMode *pMonsterMode = ToMonsterSurvivalMode( this );
				if( pMonsterMode )
					pBriefingWnd->SetStartBriefing( pMonsterMode->GetDungeonFloor() );

				ioDungeonAMode *pDungeonAMode = ToDungeonAMode( this );
				if( pDungeonAMode )
					pBriefingWnd->SetStartBriefing( pDungeonAMode->GetDungeonFloor() );
			}
			else if( GetModeType() != MT_FIGHT_CLUB )     // 파이트클럽은 브리핑 없음
			{
				pBriefingWnd->SetStartBriefing( m_iCurRoundCount );
			}
		}
	}

	if( GetModeType() == MT_PRACTICE )
	{
		CountUpWnd *pCountUpWnd = static_cast< CountUpWnd* >( g_GUIMgr.FindWnd( COUNT_UP_WND ) );
		

		PracticeInfoWnd *pInfoWnd = static_cast< PracticeInfoWnd * >( g_GUIMgr.FindWnd( PRACTICE_INFO_WND ) );
	}

	BossMonsterGaugeWnd *pBossMonsterGaugeWnd = dynamic_cast< BossMonsterGaugeWnd * >( g_GUIMgr.FindWnd( MONSTER_BOSS_GAUGE_WND ) );
	if( pBossMonsterGaugeWnd )
		pBossMonsterGaugeWnd->ChangeShowAniState( BossMonsterGaugeWnd::SAT_START_ROUND );

	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if(pTimeScoreWnd)
		pTimeScoreWnd->ChangeShowAniState( TimeScoreWnd::SAT_START_ROUND );

	MiniMap *pMiniMap = dynamic_cast<MiniMap *>( g_GUIMgr.FindWnd( MINIMAP_WND ) );
	if(pMiniMap)
	{
		pMiniMap->ChangeShowAniState( MiniMap::SAT_START_ROUND );
		ReserveMyStartPositionEvent( bFirst );
	}

	FightClubTimeGaugeWnd *pFightClubGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pFightClubGauge )
	{
		pFightClubGauge->ChangeShowAniState( FightClubTimeGaugeWnd::SAT_START_ROUND );
	}
//UNDONE : PVE User Gauge
	TowerDefWnd *pUserGauge = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ) );
	if( pUserGauge )
	{
		pUserGauge->ChangeShowAniState( TowerDefWnd::SAT_START_ROUND );
	}

	SkillListWnd *pSkillListWnd = dynamic_cast<SkillListWnd *>( g_GUIMgr.FindWnd( SKILL_LIST_WND ));
	if(pSkillListWnd)
	{
		pSkillListWnd->ChangeShowAniState( SkillListWnd::SAT_START_ROUND );
		pSkillListWnd->SetCurModeType( GetModeType() );
	}

	AccessorySkillListWnd *pAccessorySkillListWnd = dynamic_cast<AccessorySkillListWnd *>( g_GUIMgr.FindWnd( ACCESSORY_SKILL_LIST_WND ));
	if(pAccessorySkillListWnd)
	{
		pAccessorySkillListWnd->ChangeShowAniState( AccessorySkillListWnd::SAT_START_ROUND );
	}

	MiniSoldierSelectWnd *pMiniSolderSelWnd = dynamic_cast<MiniSoldierSelectWnd *>( g_GUIMgr.FindWnd( MINI_SOLDIER_SELECT_WND ));
	if(pMiniSolderSelWnd)
	{
		pMiniSolderSelWnd->ChangeShowAniState( MiniSoldierSelectWnd::SAT_START_ROUND );
		pMiniSolderSelWnd->SetCurModeType( GetModeType() );
	}

	TrainingFuncMenuWnd *pTrainingFuncMenuWnd = dynamic_cast<TrainingFuncMenuWnd *>( g_GUIMgr.FindWnd( TRAINING_FUNC_MENU ));
	if( pTrainingFuncMenuWnd )
		pTrainingFuncMenuWnd->ChangeShowAniState( TrainingFuncMenuWnd::SAT_START_ROUND );

	//Set BliendMode
	SetBlindMode( true, false );

	int iScenePosCnt = vEventScenePosList.size();
	if( iScenePosCnt == 1 )
		return;

	D3DXVECTOR3 vPrePoint  = vEventScenePosList.front();
	D3DXVECTOR3 vPreOffset = vEventSceneOffsetList.front();

	int i = 0;
	float fTotalDist = 0.0f;
	std::vector< float > vDistList;
	vDistList.reserve( iScenePosCnt );
	vDistList.push_back( 0.0f );	// for first
	for(i = 1;i < iScenePosCnt;i++)
	{
		D3DXVECTOR3 vDiff = vEventScenePosList[i] - vPrePoint;
		float fCurDist = D3DXVec3Length( &vDiff );

		fTotalDist += fCurDist;
		vDistList.push_back( fTotalDist );
		vPrePoint = vEventScenePosList[i];
	}

	// 구간별 거리/총거리 => 총시간중 해당 구간을 지나가는 시간
	for(i = 1;i < iScenePosCnt;i++)
	{
		float fCurDist = vDistList[i];
		float fRate = fCurDist / fTotalDist;

		DWORD dwDividedTime = m_dwLoadEventStartTime + m_dwLoadEventDuration * fRate;
		AddEventSceneTime( dwDividedTime, vEventScenePosList[i], vEventSceneOffsetList[i] );
	}
}

void ioPlayMode::ReserveMyStartPositionEvent( bool bFirst )
{
	m_bReserveMyPositionEvent = true;
}

D3DXVECTOR3 ioPlayMode::UpdateEventScenePos( DWORD dwCurTime ) const
{
	if( m_EventTimeMap.empty() )
		return D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	EventTimeMap::const_iterator iterBefore, iterAfter;
	iterAfter = m_EventTimeMap.upper_bound( dwCurTime );
	if( iterAfter == m_EventTimeMap.end() )
	{
		--iterAfter;
		return iterAfter->second;
	}

	if( iterAfter == m_EventTimeMap.begin() )
		return iterAfter->second;

	iterBefore = iterAfter;
	iterBefore--;

	DWORD dwCurPoint = dwCurTime - iterBefore->first;
	DWORD dwEndPoint = iterAfter->first - iterBefore->first;

	float fFactor = (float)dwCurPoint/(float)dwEndPoint;

	D3DXVECTOR3 vCurPoint;
	D3DXVec3Lerp( &vCurPoint, &iterBefore->second, &iterAfter->second, fFactor );
	return vCurPoint;
}

D3DXVECTOR3 ioPlayMode::UpdateEventSceneOffset( DWORD dwCurTime ) const
{
	if( m_EventTimeMapOffset.empty() )
		return D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	EventTimeMap::const_iterator iterBefore, iterAfter;
	iterAfter = m_EventTimeMapOffset.upper_bound( dwCurTime );
	if( iterAfter == m_EventTimeMapOffset.end() )
	{
		--iterAfter;
		return iterAfter->second;
	}

	if( iterAfter == m_EventTimeMapOffset.begin() )
		return iterAfter->second;

	iterBefore = iterAfter;
	iterBefore--;

	DWORD dwCurPoint = dwCurTime - iterBefore->first;
	DWORD dwEndPoint = iterAfter->first - iterBefore->first;

	float fFactor = (float)dwCurPoint/(float)dwEndPoint;

	D3DXVECTOR3 vCurPoint;
	D3DXVec3Lerp( &vCurPoint, &iterBefore->second, &iterAfter->second, fFactor );
	return vCurPoint;
}

void ioPlayMode::AddEventSceneTime( DWORD dwTime, const D3DXVECTOR3 &vEventPos, const D3DXVECTOR3 &vEventOffset )
{
	m_EventTimeMap.insert( EventTimeMap::value_type( dwTime, vEventPos ) );
	m_EventTimeMapOffset.insert( EventTimeMap::value_type( dwTime, vEventOffset ) );
}

bool ioPlayMode::IsEventSceneOver() const
{
	BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
	if( pBriefingWnd && !pBriefingWnd->IsShow() )
		return true;
	else if( !pBriefingWnd )
		return true;

	return false;
}

void ioPlayMode::GetModeSpecialInfo( SP2Packet &rkPacket )
{
}

void ioPlayMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iMaxRoundCount;

	rkPacket >> m_dwRoundDuration;

	rkPacket >> m_iBlueRoundWinCnt;
	rkPacket >> m_iRedRoundWinCnt;

	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;	

	m_vRoundHistory.erase( m_vRoundHistory.begin(), m_vRoundHistory.end() );
	for(int i=0;i<m_iMaxRoundCount;i++)
	{
		RoundHistory rh;
		m_vRoundHistory.push_back( rh );
	}
}

void ioPlayMode::NotifyCharDrop( ioPlayEntity *pEntity, float fDropDamage )
{
	ioBaseChar *pDropChar = ToBaseChar( pEntity );

	if( !pDropChar ) return;
	if( !pDropChar->IsNeedProcess() ) return;

	ioHashString szAttackerName = pDropChar->GetLastAttackerName();
	if( pDropChar->IsForceDropping() && !szAttackerName.IsEmpty() )
	{
		pDropChar->SetLastAttackerName( "", "" );

		ioBaseChar *pAttacker = m_pCreator->GetBaseChar( szAttackerName );
		if( pAttacker && pDropChar->GetTeam() != pAttacker->GetTeam() )
		{
			NotifyOwnerDamaged( pDropChar->GetCharName(), pAttacker->GetCharName(), fDropDamage );
		}
	}
}

DamageTable *ioPlayMode::FindDamageList( const ioHashString &rkName )
{
	DamageTableMap::iterator iter = m_DamageTableMap.find( rkName );
	if( iter != m_DamageTableMap.end() )
		return iter->second;

	// 없으면 새로 추가한다.
	DamageTable *pTable = new DamageTable;
	pTable->ClearTable();
	m_DamageTableMap.insert( DamageTableMap::value_type( rkName, pTable ) );
	return pTable;
}

void ioPlayMode::NotifyOwnerDamaged( const ioHashString &szTarget, const ioHashString &szAttacker, float fDamage )
{
	DamageTable *pDamageTable = FindDamageList( szTarget );
	if( pDamageTable == NULL ) return;

	if( szTarget != szAttacker )
	{
		pDamageTable->AddDamage( szAttacker, fDamage + 0.999999f );   // 최소 1은 준다.
	}
}

void ioPlayMode::AttachOwnerDamageList( const ioHashString &szName )
{
	DamageTableMap::iterator iCreate = m_DamageTableMap.begin();
	for(; iCreate != m_DamageTableMap.end() ; ++iCreate )
	{
		DamageTable *pTable = iCreate->second;
		if( pTable == NULL ) continue;
		
		pTable->AttachAllDamage( szName );
	}
}

bool ioPlayMode::FillAllDamagedList( SP2Packet &rkPacket )
{
	DamageTable *pAllDamageTable = new DamageTable;
	if( !pAllDamageTable )
	{
		rkPacket << 0;
		return false;
	}
	pAllDamageTable->ClearTable();

	DamageTableMap::iterator iCreate = m_DamageTableMap.begin();
	for(; iCreate != m_DamageTableMap.end() ; ++iCreate )
	{
		DamageTable *pTable = iCreate->second;
		if( pTable == NULL ) continue;

		pTable->AttachAllDamage();
		int iDamageCnt = pTable->GetDamageTableCnt();
		for( int i=0 ; i<iDamageCnt ; i++ )
		{
			ioHashString rkAttacker;
			int iTotalDamage = 0;

			pTable->GetAllDamage( i, rkAttacker, iTotalDamage );
			pAllDamageTable->AddDamage( rkAttacker, iTotalDamage );			
		}
		pTable->ClearTable();
	}

	pAllDamageTable->AttachAllDamage();
	int iDamageCnt = pAllDamageTable->GetDamageTableCnt();
	if( iDamageCnt > 0 )
	{
		rkPacket << iDamageCnt;
		for( int i=0 ; i<iDamageCnt ; i++ )
		{
			ioHashString rkAttacker;
			int iTotalDamage = 0;

			pAllDamageTable->GetAllDamage( i, rkAttacker, iTotalDamage );
			rkPacket << rkAttacker;
			rkPacket << iTotalDamage;
		}
		pAllDamageTable->ClearTable();
		SAFEDELETE( pAllDamageTable );
		return true;
	}
	else
	{
		rkPacket << 0;
	}

	SAFEDELETE( pAllDamageTable );
	return false;
}

void ioPlayMode::FillOwnerDamagedListByOtherTeam( const ioHashString &szTarget, TeamType eTeam, SP2Packet &rkPacket, bool bAttach )
{
	DamageTable *pDamageTable = FindDamageList( szTarget );
	if( pDamageTable == NULL )
	{
		rkPacket << 0;
		return;
	}

	ioBaseChar *pChar = m_pCreator->GetBaseChar( szTarget );
	if( ToNpcChar(pChar) )  // NPC 가 죽을 경우는 total damage 를 보낸 후 return 한다.
	{
		if( bAttach )
			pDamageTable->AttachAllDamage();

		int nTable = pDamageTable->GetDamageTableCnt();
		rkPacket << nTable;
		for( int i = 0; i < nTable; ++i )
		{
			ioHashString rkAttacker;
			int iTotalDamage = 0;

			pDamageTable->GetAllDamage( i, rkAttacker, iTotalDamage );
			rkPacket << rkAttacker;
			rkPacket << iTotalDamage;
		}

		return;
	}

	DamageTable *pTempTable = new DamageTable;
	if( !pTempTable )
	{
		rkPacket << 0;
		return;
	}


	pTempTable->ClearTable();

	int iDamageCnt = pDamageTable->GetDamageTableCnt();
	for( int i=0 ; i<iDamageCnt ; i++ )
	{
		ioHashString rkAttacker;
		int iTotalDamage = 0;

		pDamageTable->GetCurDamage( i, rkAttacker, iTotalDamage );
		
		ioBaseChar *pChar = m_pCreator->GetBaseChar( rkAttacker );
		if( pChar && eTeam != pChar->GetTeam() )
		{
			pTempTable->AddDamage( rkAttacker, iTotalDamage );
		}
	}

	if( bAttach )
	{
		pDamageTable->AttachAllDamage();
	}

	int iNewCnt = pTempTable->GetDamageTableCnt();
	rkPacket << iNewCnt;
	for( int j=0; j < iNewCnt; ++j )
	{
		ioHashString rkAttacker;
		int iTotalDamage = 0;

		pTempTable->GetAllDamage( j, rkAttacker, iTotalDamage );
		rkPacket << rkAttacker;
		rkPacket << iTotalDamage;
	}

	pTempTable->ClearTable();
	SAFEDELETE( pTempTable );
}

void ioPlayMode::GetDamagedTopChar( const ioHashString &szTarget, ioHashString &rkReturnName )
{
	DamageTable *pDamageTable = FindDamageList( szTarget );
	if( pDamageTable == NULL ) return;

	int iDamageValue = 0;
	int iDamageCnt = pDamageTable->GetDamageTableCnt();
	for(int i = 0;i < iDamageCnt;i++)
	{
		int iTotalDamage = 0;
		ioHashString rkAttacker;		
		pDamageTable->GetAllDamage( i, rkAttacker, iTotalDamage );
		if( iTotalDamage > iDamageValue )
		{
			iDamageValue = iTotalDamage;
			rkReturnName = rkAttacker;
		}
	}
}

void ioPlayMode::GetDamagedLowestChar( const ioHashString &szTarget, ioHashString &rkReturnName )
{
	DamageTable *pDamageTable = FindDamageList( szTarget );
	if( pDamageTable == NULL ) return;

	int iDamageValue = MAX_INT_VALUE;
	int iDamageCnt = pDamageTable->GetDamageTableCnt();
	for(int i = 0;i < iDamageCnt;i++)
	{
		int iTotalDamage = 0;
		ioHashString rkAttacker;		
		pDamageTable->GetAllDamage( i, rkAttacker, iTotalDamage );
		if( iTotalDamage < iDamageValue )
		{
			iDamageValue = iTotalDamage;
			rkReturnName = rkAttacker;
		}
	}
}

void ioPlayMode::GetModeCondition( const ioHashString &szTarget, ioHashString &rkReturnName )
{
}

int ioPlayMode::GetAllDamaged( const ioHashString &szTarget, const ioHashString &szFindName )
{
	DamageTable *pDamageTable = FindDamageList( szTarget );
	if( pDamageTable == NULL ) return 0;

	return pDamageTable->GetAllDamage( szFindName );
}

void ioPlayMode::ProcessPushEnableCursor()
{
	if( m_pPushCursor )
	{
		m_pPushCursor->Update();
	}
}

void ioPlayMode::ProcessCharDropZoneCollision()
{
	int iCharCount = GetRecordCharCnt();
	for( int i=0 ; i<iCharCount ; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		ErrorReport::SetPosition( 3000, 13 );
		if( !pChar ) continue;
		ErrorReport::SetPosition( 3000, 14 );

		//by 윤태철
		//////////////////////////////////////////////////////////////////////////
		__try
		{
			if( !pChar->IsNeedProcess() )	continue;
		}
		__except (1)
		{
			RemoveRecord( i );
			return;
		}
		//////////////////////////////////////////////////////////////////////////

		
		ErrorReport::SetPosition( 3000, 15 );

		bool bDummyDrop = false;
		CharState eState = pChar->GetState();
		switch( eState )
		{
		case CS_DIE:
		case CS_DROP_ZONE_DOWN:
		case CS_ESCAPE_PRISONER:
		case CS_READY:
		case CS_LOADING:
		case CS_VIEW:
		case CS_OBSERVER:
		case CS_CHANGE_WAIT:
		case CS_STARTMOTION:
		case CS_ENDMOTION:
		case CS_CREATEMOTION:
		case CS_COIN_REBIRTH:
		case CS_CHANGE_GANGSI:
		case CS_GRADEUP_MOTION:
		case CS_LOCKUP_BUFF:
		case CS_CONSTRUCT_HOUSE:			
			continue;
		case CS_CANNON_SPECIAL:
			bDummyDrop = g_DummyCharMgr.IsDropZoneCollision( pChar );
			break;
		default:
			break;
		}

		if( pChar->GetWorldPosition().y >= 0.1f && bDummyDrop != true )
			continue;

		float fDropDamage = 0.0f;
		fDropDamage = m_fDropZoneDamage * m_fDropDamageMapRate;

		if( IsSafetyLevelRoom() )
		{
			fDropDamage *= m_fDropProtectRate;
		}

		// PvE에서는 드랍데미지가 기본값으로만 적용된다.
		if( GetModeType() != MT_MONSTER_SURVIVAL && GetModeType() != MT_DUNGEON_A )
		{
			TeamType eTeam = pChar->GetTeam();
			fDropDamage = m_pCreator->GetMemberBalanceForDropDamage( eTeam, fDropDamage );
		}

		if( fDropDamage > 0.0f )
		{
			fDropDamage = min( fDropDamage, m_fDropZoneMaxDamage );
			fDropDamage = max( m_fDropZoneMinDamage, fDropDamage );
		}
		pChar->ApplyDropDamage( fDropDamage );
	}
}

void ioPlayMode::ProcessSelectCharacter()
{
	//if( !g_GUIMgr.IsShow( SOLDIER_SELECT_WND ) )
	if( m_dwStartSoldierWndTime == 0 )
		return;
	
	if( m_ModeState == MS_RESULT && g_GUIMgr.IsShow( SOLDIER_SELECT_WND ) )
	{
		g_GUIMgr.HideWnd( SOLDIER_SELECT_WND );
		return;
	}	
	
	SoldierSelectWnd *pSelectWnd = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( !pSelectWnd || !pSelectWnd->IsForcedWait() )
		return;
	
	if( !pSelectWnd->IsSelectOk() )
	{
		DWORD dwShowEndTime = m_dwStartSoldierWndTime + m_dwShowSoldierWndTime;
		if( m_dwShowSoldierWndTime <= 0 || FRAMEGETTIME() < dwShowEndTime )
		{
			return;
		}
	}
	
    ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) return;
	
	m_iSelectCharacter = g_MyInfo.GetAutoSelectChar( pSelectWnd->GetSelectCharArray(), m_iSelectCharacter );

	SP2Packet kPacket( CTPK_CHANGE_CHAR );
	if( GetModeType() == MT_ARENA )
		kPacket << m_iSelectCharacter + g_MyInfo.GetOriginalCharacterCount();
	else
		kPacket << m_iSelectCharacter;
	kPacket << false;
	kPacket << pOwner->GetCharChangeUniqueIndex() + 1;
	TCPNetwork::SendToServer( kPacket );
	EndChangeCharacter();

	if( g_MyInfo.GetActiveCharCount() == 0 ||
		g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ||
		g_MyInfo.GetActiveCharCount() == g_MyInfo.GetCharRentalSetCount() )
	{
		if( GetModeType() != MT_MYROOM )
			g_MyInfo.SetRoomOutTimer( FRAMEGETTIME() );
	}

	// 30초뒤부터 나가기 페널티 적용
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		ioBattleRoomExitMsgBox *pExitMsgBox = dynamic_cast<ioBattleRoomExitMsgBox*>(g_GUIMgr.FindWnd( BATTLEROOM_EXIT_MSG_BOX ));
		if( pExitMsgBox )
			pExitMsgBox->SetPlayJoin();
	}
	else
	{
		ioExitMsgBox *pExitMsgBox = dynamic_cast<ioExitMsgBox*>(g_GUIMgr.FindWnd( EXIT_MSG_BOX ));
		if( pExitMsgBox )
			pExitMsgBox->SetPlayJoin();
	}

	// 최초 용병 선택 카메라 이벤트
	m_pCreator->StartSelectCharCameraEvent();

	// 퀘스트 처리
	switch( GetModeType() )
	{
	case MT_MONSTER_SURVIVAL:
		{
			if( g_BattleRoomMgr.IsBattleRoom() )
			{
				g_QuestMgr.QuestOccurTerm( QO_ENTER_BATTLE_PVE );
			}

			if( m_bEnterOneCheckQuest )
				g_QuestMgr.QuestOccurTerm( QO_ENTER_ROOM_PVE );
			m_bEnterOneCheckQuest = false;
		}
		break;
	case MT_SYMBOL:
	case MT_CATCH:
	case MT_KING:
	case MT_SURVIVAL:
	case MT_FIGHT_CLUB:
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_BOSS:
	case MT_FOOTBALL:
	case MT_HEROMATCH:
	case MT_GANGSI:
	case MT_CATCH_RUNNINGMAN:
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_DOUBLE_CROWN:
	case MT_SHUFFLE_BONUS:
	case MT_FACTORY:
	case MT_RAID:
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_FLAG_CAPTURE:
	case MT_ARENA:
	case MT_BATTLE:
	case MT_FARMING:
		{
			if( g_BattleRoomMgr.IsBattleRoom() || g_ShuffleRoomMgr.IsShuffleRoom() || g_RankBattleMgr.IsRankBattlePlaying() )
			{
				g_QuestMgr.QuestOccurTerm( QO_ENTER_BATTLE_PVP, GetModeType() );
			}

			if( m_bEnterOneCheckQuest )
				g_QuestMgr.QuestOccurTerm( QO_ENTER_ROOM_PVP );
			m_bEnterOneCheckQuest = false;
		}
		break;
	}
}

void ioPlayMode::ProcessAutoStartAlarm()
{
	if( m_dwCurAutoStartAlarmTime == 0 ) return;
	if( m_dwAutoStartAlarmTime == 0 ) return;
	if( !g_BattleRoomMgr.IsBattleRoom() ) return;
	if( !g_BattleRoomMgr.IsAutoModeStart() ) return;
	if( GetModeState() != MS_RESULT ) return;
    
	if( FRAMEGETTIME() - m_dwCurAutoStartAlarmTime > m_dwAutoStartAlarmTime )
	{
		m_dwCurAutoStartAlarmTime = 0;
//		g_ChatMgr.SetSystemMsg( "%d초 후 다음 게임 자동시작.", g_BattleRoomMgr.GetAutoStartAlarmCheckTime() / 1000 );
	}
}

void ioPlayMode::UpdateRoundCount()
{
	m_iCurRoundCount++;
	if( m_iCurRoundCount > m_iMaxRoundCount )
	{
		m_iCurRoundCount = 1;
	}
}

void ioPlayMode::ClearRoundEndAllBuff()
{
	int iCharCnt = GetRecordCharCnt();
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( pChar )
		{
			pChar->RemoveRoundEndAllBuff();
		}
	}
}

void ioPlayMode::UpdateRoundStateWnd()
{
	if( m_ModeState != MS_PLAY )
	{
		if( g_GUIMgr.IsShow( ROUND_STATE_MAIN_WND ) )
		{
			g_GUIMgr.HideWnd( ROUND_STATE_MAIN_WND );
			SetViewRoundState( false );
		}
		return;
	}

	if( g_Input.IsKeyDown( KEY_TAB ) )
	{
		RoundStateMainWnd *pRoundStateWnd = dynamic_cast<RoundStateMainWnd*>(g_GUIMgr.FindWnd( ROUND_STATE_MAIN_WND ));
		if( pRoundStateWnd && !pRoundStateWnd->IsShow() && !pRoundStateWnd->IsSearchShow() )	// 2019-07-03
		{	
			ContributeRoundAllPercent( m_dwRoundDuration * ( m_iCurRoundCount + 1 ), m_dwRoundDuration * m_iMaxRoundCount );

			pRoundStateWnd->ClearRoundInfo();
			for( int i = 0 ; i < GetRecordCharCnt(); i++ )
			{
				RoundInfo kInfo;
				if( GetRoundInfo( i, kInfo ) )
					pRoundStateWnd->AddRoundInfo( kInfo );
			}

			pRoundStateWnd->SetTeamScore( m_iBlueRoundWinCnt, m_iRedRoundWinCnt );
			pRoundStateWnd->ShowWnd();
		}
	}
	else
	{
		if( g_GUIMgr.IsShow( ROUND_STATE_MAIN_WND ) )
		{
			g_GUIMgr.HideWnd( ROUND_STATE_MAIN_WND );
		}
	}
}

void ioPlayMode::UpdateObserverWnd()
{
	if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) && !m_pCreator->IsStealthMode() )
		return;

	if( g_ShuffleRoomMgr.IsShuffleRoom() )
		return;
	if( g_RankBattleMgr.IsRankBattlePlaying() )
		return;

	if( m_ModeState != MS_PLAY )
	{
		g_ObserverWndMgr.ObserverWndHide();
		return;
	}

	if( !g_GUIMgr.IsShow(OBSERVER_BLUE_INFO_WND) || !g_GUIMgr.IsShow(OBSERVER_RED_INFO_WND) )
	{
		g_ObserverWndMgr.ObserverWndShow();
	}

	g_ObserverWndMgr.ClearInfo();
	ContributeRoundAllPercent( m_dwRoundDuration * ( m_iCurRoundCount + 1 ), m_dwRoundDuration * m_iMaxRoundCount );

	for( int i = 0 ; i < GetRecordCharCnt(); i++ )
	{
		RoundInfo kInfo;
		if( GetRoundInfo( i, kInfo ) )
		{
			g_ObserverWndMgr.SetRoundInfo( kInfo );
		}
	}

	g_ObserverWndMgr.UpdateInfo( GetModeType() );
}

void ioPlayMode::SetResultScoreWnd()
{
	RoundResultMainWnd *pResult = dynamic_cast<RoundResultMainWnd*>(g_GUIMgr.FindWnd( ROUND_RESULT_MAIN_WND ));
	if( !pResult )	return;

	m_dwCurResultShowDuration = m_dwRoundResultDuration;

	ContributeRoundAllPercent( m_dwRoundDuration * ( m_iCurRoundCount + 1 ), m_dwRoundDuration * m_iMaxRoundCount );

	pResult->ClearRoundInfo();
	for(int i = 0 ; i < GetRecordCharCnt(); i++ )
	{
		RoundInfo kInfo;
		if( GetRoundInfo( i, kInfo ) )
			pResult->AddRoundInfo( kInfo );
	}

	pResult->SetTeamScore( m_iBlueRoundWinCnt, m_iRedRoundWinCnt );
	SetResultBriefingWnd();
}

void ioPlayMode::SetResultBriefingWnd()
{
	// 팀 모드 중간 결과 브리핑
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
		if( pBriefingWnd && !pBriefingWnd->IsShow() )
		{
			int iWinCount, iLoseCount;
			int iResultType = BriefingWnd::RESULT_DRAW;
			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
			{
				ioBaseChar *pTargetChar = m_pCreator->GetBaseChar(m_szEndLookCharName);
				if( pTargetChar && (GetModeType() == MT_SURVIVAL || GetModeType() == MT_TEAM_SURVIVAL || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS || GetModeType() == MT_TEAM_SURVIVAL_AI) )
				{
					TeamType eOwenerTeam = pTargetChar->GetTeam();
					if( eOwenerTeam == TEAM_BLUE )
					{
						if( IsWinTeam( m_WinTeamType, TEAM_BLUE ) )
							iResultType = BriefingWnd::RESULT_WIN;
						else
							iResultType = BriefingWnd::RESULT_LOSE;
					}
					else
					{
						if( IsWinTeam( m_WinTeamType, TEAM_RED ) )
							iResultType = BriefingWnd::RESULT_WIN;
						else
							iResultType = BriefingWnd::RESULT_LOSE;
					}

					pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_RESULT, iResultType, eOwenerTeam, m_iCurRoundCount );

				}
				else
				{
					if( IsWinTeam( m_WinTeamType, TEAM_BLUE ) )
						pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_RESULT, BriefingWnd::RESULT_WIN, TEAM_BLUE, m_iCurRoundCount );
					else
						pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_RESULT, BriefingWnd::RESULT_WIN, TEAM_RED, m_iCurRoundCount );
				}
			}
			else
			{
				if( pOwner->GetTeam() == TEAM_BLUE )
				{
					iWinCount = m_iBlueRoundWinCnt;
					iLoseCount= m_iRedRoundWinCnt;
					if( IsWinTeam( m_WinTeamType, TEAM_BLUE ) )
						iResultType = BriefingWnd::RESULT_WIN;
					else
						iResultType = BriefingWnd::RESULT_LOSE;
				}
				else
				{
					iWinCount = m_iRedRoundWinCnt;
					iLoseCount= m_iBlueRoundWinCnt;
					if( IsWinTeam( m_WinTeamType, TEAM_RED ) )
						iResultType = BriefingWnd::RESULT_WIN;
					else
						iResultType = BriefingWnd::RESULT_LOSE;
				}

				pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_RESULT, iResultType, pOwner->GetTeam(), m_iCurRoundCount );
			}
		}				
	}
	SetBlindMode( true, false );
}

void ioPlayMode::SetTotalResultBriefingWnd()
{
	// 팀 모드 최종 결과 브리핑
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
		if( pBriefingWnd && !pBriefingWnd->IsShow() )
		{
			TeamType eWinTeam = TEAM_NONE;
			if( m_iBlueRoundWinCnt > m_iRedRoundWinCnt )
				eWinTeam = TEAM_BLUE;
			else if( m_iBlueRoundWinCnt < m_iRedRoundWinCnt )
				eWinTeam = TEAM_RED;

			D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
			int iWinCount, iLoseCount;
			int iResultType = BriefingWnd::RESULT_DRAW;
			TeamType eOwenerTeam = pOwner->GetTeam();

			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
			{
				ioBaseChar *pTargetChar = m_pCreator->GetBaseChar(m_szEndLookCharName);
				if( pTargetChar && (GetModeType() == MT_SURVIVAL || GetModeType() == MT_TEAM_SURVIVAL || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS || GetModeType() == MT_TEAM_SURVIVAL_AI) )
				{
					eOwenerTeam = pTargetChar->GetTeam();

					if( eOwenerTeam == TEAM_BLUE )
					{
						if( eWinTeam == TEAM_BLUE )
							iResultType = BriefingWnd::RESULT_WIN;
						else if( eWinTeam == TEAM_RED )
							iResultType = BriefingWnd::RESULT_LOSE;
					}
					else
					{
						if( eWinTeam == TEAM_RED )
							iResultType = BriefingWnd::RESULT_WIN;
						else if( eWinTeam == TEAM_BLUE )
							iResultType = BriefingWnd::RESULT_LOSE;
					}

					if( IsNeedUpdateMapEffect( pTargetChar ) )
						vNewPos = pTargetChar->GetWorldPosition();
				}
				else
				{
					eOwenerTeam = eWinTeam;
					iResultType = BriefingWnd::RESULT_WIN;
				}
			}
			else
			{
				if( pOwner->GetTeam() == TEAM_BLUE )
				{
					iWinCount = m_iBlueRoundWinCnt;
					iLoseCount= m_iRedRoundWinCnt;

					if( eWinTeam == TEAM_BLUE )
						iResultType = BriefingWnd::RESULT_WIN;
					else if( eWinTeam == TEAM_RED )
						iResultType = BriefingWnd::RESULT_LOSE;
				}
				else
				{
					iWinCount = m_iRedRoundWinCnt;
					iLoseCount= m_iBlueRoundWinCnt;

					if( eWinTeam == TEAM_RED )
						iResultType = BriefingWnd::RESULT_WIN;
					else if( eWinTeam == TEAM_BLUE )
						iResultType = BriefingWnd::RESULT_LOSE;
				}

				if( IsNeedUpdateMapEffect( pOwner ) )
					vNewPos = pOwner->GetWorldPosition();
			}

			pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_FINAL_RESULT, iResultType, eOwenerTeam, m_iCurRoundCount );
			
			ioHashString szMapEffect = pBriefingWnd->GetFinalResultMapEffect( eWinTeam );
			// 맵 이펙트 작렬!
			if( !szMapEffect.IsEmpty() )
			{
				if( vNewPos == ioMath::VEC3_ZERO )
					CreateMapEffect( szMapEffect, GetEndSceneLookAt(), ioMath::UNIT_ALL );
				else
					CreateMapEffect( szMapEffect, vNewPos, ioMath::UNIT_ALL );
			}
		}				
	}
	SetBlindMode( true, false );
}

void ioPlayMode::StartChangeCharacter( int now_select_char, DWORD dwSelectTime )
{
	//
	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		if( g_LadderTeamMgr.IsHeroMatchMode() )
		{
			HeroMatchVersusSceneWnd *pVersusScene = dynamic_cast<HeroMatchVersusSceneWnd*>(g_GUIMgr.FindWnd( HERO_MATCH_VERSUS_SCENE_WND ));
			if( pVersusScene && pVersusScene->IsShow() )
			{
				pVersusScene->StartFadeOut( this, now_select_char, dwSelectTime );
				return;   // 페이드 아웃이 끝난 후에 StartChangeCharacter함수를 호출한다.
			}
		}
	}

	SuccessionMatchVersusSceneWnd *pSuccessionVersusScene = dynamic_cast<SuccessionMatchVersusSceneWnd*>(g_GUIMgr.FindWnd( SUCCESSION_BATTLE_VERSUS_SCENE_WND ));
	if( pSuccessionVersusScene && pSuccessionVersusScene->IsShow() )
	{
		pSuccessionVersusScene->StartFadeOut( this, now_select_char, dwSelectTime );
		return;   // 페이드 아웃이 끝난 후에 StartChangeCharacter함수를 호출한다.
	}
	//////////////////////////////////////////////////////////////////////////
	ModeRecord *pDieRecord = FindModeRecordCharName( g_MyInfo.GetPublicID() );
	if( pDieRecord )
	{
		pDieRecord->dwCurDieTime = 0;
	}

	m_iSelectCharacter		= now_select_char;
	m_dwStartSoldierWndTime = FRAMEGETTIME();
	m_dwShowSoldierWndTime	= dwSelectTime;

	SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( pSoldier )
	{
		pSoldier->HideWnd();
		pSoldier->SetSelectCharArray( m_iSelectCharacter );
		pSoldier->ShowWnd();
	}
	g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_VIEW );

	SetBlindMode( true, false );

	if( GetModeType() == MT_FIGHT_CLUB )
	{
		FightClubTimeGaugeWnd *pFightClubGaugeWnd = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
		if( pFightClubGaugeWnd )
		{
			if( g_MyInfo.GetPublicID() == pFightClubGaugeWnd->GetChampionUserName() )
			{
				// 챔피언은 왼쪽
				ioBaseChar *pOwner = GetOwnerChar();
				if( pOwner )
				{
					D3DXVECTOR3 vPos = GetStartPos( 0, false );
					pOwner->SetWorldPosition( vPos );
				}
			}
			else
			{
				// 도전자는 오른쪽
				ioBaseChar *pOwner = GetOwnerChar();
				if( pOwner )
				{
					D3DXVECTOR3 vPos = GetStartPos( 1, false );
					pOwner->SetWorldPosition( vPos );
				}
			}
		}
	}
}

void ioPlayMode::EndChangeCharacter()
{	
	g_GUIMgr.HideWnd( SOLDIER_SELECT_WND );
	m_dwStartSoldierWndTime = 0;
	SetBlindMode( false );

	// 카메라 포즈 상태 해제
	m_pCreator->EndCameraPause();
}

void ioPlayMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] )
{
	TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
	if( !pTotalResult ) return;

	if( g_GUIMgr.IsShow( ROUND_RESULT_MAIN_WND ) )
		g_GUIMgr.HideWnd( ROUND_RESULT_MAIN_WND );

	ContributeRoundAllPercent( m_dwRoundDuration * m_iMaxRoundCount, m_dwRoundDuration * m_iMaxRoundCount );

	int i = 0;
	int iRecordCnt = GetRecordCharCnt();
	for( i=0 ; i<iRecordCnt ; i++ )
	{
		FinalInfo kInfo;
		if( GetFinalInfo( i, kInfo ) )
		{
			pTotalResult->AddFinalInfo( kInfo );
			if( g_BattleRoomMgr.IsBattleRoom() )
				g_BattleRoomMgr.UpdateUserLevel( kInfo.m_szName, kInfo.m_iLevel );
			else if( g_LadderTeamMgr.IsLadderTeam() )
				g_LadderTeamMgr.UpdateUserData( kInfo.m_szName, kInfo.m_iLevel, kInfo.m_iResultLadderPoint );
			else if( g_ShuffleRoomMgr.IsShuffleRoom() )
				g_ShuffleRoomMgr.UpdateUserLevel( kInfo.m_szName, kInfo.m_iLevel );
		}
	}

	pTotalResult->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	pTotalResult->SetFinalRoundPoint( rvClassTypeList, rvClassPointList, fBonusArray );

	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;
	pTotalResult->SetTeamScore( GetOwnerTeam(), m_iBlueRoundWinCnt, m_iRedRoundWinCnt );

	SetTotalResultBriefingWnd();
}

void ioPlayMode::ShowEndScoreWnd( bool bShow )
{
	if( g_GUIMgr.IsShow( BRIEFING_WND ) ) return;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND ) )
			return;
	}
	if( m_bRoundSetEnd ) return;
	if( g_GUIMgr.IsShow( MIDDLE_JOINDELAY_WND ) ) return;

	if( bShow )
	{
		if( !g_GUIMgr.IsShow( ROUND_RESULT_MAIN_WND ) )
		{
			g_GUIMgr.ShowWnd( ROUND_RESULT_MAIN_WND );
		}
	}
	else
	{
		g_GUIMgr.HideWnd( ROUND_RESULT_MAIN_WND );
	}
}

void ioPlayMode::UpdateEndScoreWndTime()
{
	if( m_ModeState != MS_RESULT )	return;

	DWORD dwPastTime = FRAMEGETTIME() - m_dwResultStartTime;

	DWORD dwRemainTime = 0;
	if( m_dwCurResultShowDuration > dwPastTime )
	{
		dwRemainTime = ( m_dwCurResultShowDuration - dwPastTime ) / 1000;
	}
}

void ioPlayMode::UpdateSoldierWndTime()
{
	SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( !pSoldier || !pSoldier->IsShow() || pSoldier->IsSelectWait() ) return;
		
	DWORD dwPastTime = FRAMEGETTIME() - m_dwStartSoldierWndTime;

	DWORD dwRemainTime = 0;
	if( m_dwShowSoldierWndTime > dwPastTime )
	{
		dwRemainTime = ( m_dwShowSoldierWndTime - dwPastTime ) / 1000;
	}

	pSoldier->SetRemainSecond( dwRemainTime );
}

void ioPlayMode::UpdateSoldierRevivalPenalty()
{
	SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( !pSoldier || pSoldier->IsShow() ) return;

	ioBaseChar *pChar = GetOwnerChar();
	if( !pChar ) return;

	if( pChar->GetState() == CS_DIE )
		return;

	pSoldier->UpdateRecoverRevivalPenalty();
}

void ioPlayMode::UpdateMiniMapSpotPoint()
{
	CheckMyStartPositionEventFire();
}

void ioPlayMode::UpdateTimeGrowthTime()
{
	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		pLevel->CheckTimeGrowthEnd();
	}
}

bool ioPlayMode::CheckMyStartPositionEventFire()
{
	if( !m_bReserveMyPositionEvent )
		return false;

	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( pMiniMap )
	{
		if( pMiniMap->GetCurShowAniState() == MiniMap::SAT_END )
		{
			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
			{
				SetMiniMapEvent( pOwner->GetWorldPosition(), MiniMap::EVENT_START_MY_POSITION, pOwner->GetTeam() );
			}

			m_bReserveMyPositionEvent = false;
			return true;
		}
	}

	return false;
}


bool ioPlayMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	ModeRecord *pRecord = FindModeRecord( iIndex );
	if( !pRecord )	return false;
	if( !pRecord->pChar ) return false;
	if( ToNpcChar( pRecord->pChar ) ) return false;

	ioBaseChar *pChar = m_pCreator->GetBaseChar( pRecord->szName );
	if( !pChar ) return false;

	if( GetModeType() != MT_TRAINING && GetModeType() != MT_MYROOM && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) )
			return false;
	}

	if( pChar->GetState() == CS_OBSERVER )
		return false;

	rkInfo.m_szName	= pRecord->pChar->GetCharName();
	rkInfo.m_Team	= pRecord->pChar->GetTeam();
	rkInfo.m_iLevel = pRecord->pChar->GetLevel();

	rkInfo.m_iTotalKill  = pRecord->GetAllTotalKill();
	rkInfo.m_iTotalDeath = pRecord->GetAllTotalDeath();

	rkInfo.m_iUniqueTotalKill  = pRecord->GetUniqueTotalKill();
	rkInfo.m_iUniqueTotalDeath = pRecord->GetUniqueTotalDeath();

    rkInfo.m_iLadderPoint = g_UserInfoMgr.GetLadderPoint( rkInfo.m_szName );
	rkInfo.m_iVictories = g_VictoriesMgr.GetVictories( rkInfo.m_szName );

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		rkInfo.m_iMonsterCoin = g_BattleRoomMgr.GetUserMonsterCoin( rkInfo.m_szName );
		rkInfo.m_iGoldMonsterCoin = g_BattleRoomMgr.GetUserGoldMonsterCoin( rkInfo.m_szName );
		rkInfo.m_iRaidCoin = g_BattleRoomMgr.GetUserRaidCoin( rkInfo.m_szName );
	}
	
	if( rkInfo.m_szName == g_MyInfo.GetPublicID() )
		rkInfo.m_bOwner = true;
	else
		rkInfo.m_bOwner = false;

	g_PlayBonusMgr.GetUserPlayBonus( rkInfo.m_szName, rkInfo.m_bPCRoomBonus, rkInfo.m_bGuildBonus, rkInfo.m_bFriendBonus );
	g_UserInfoMgr.GetGuildInfo( rkInfo.m_szName, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark );
	return true;
}

bool ioPlayMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	ModeRecord *pRecord = FindModeRecord( iIndex );
	if( !pRecord )	return false;
	if( !pRecord->pChar ) return false;
	if( ToNpcChar( pRecord->pChar ) ) return false;

	if( P2PNetwork::IsNetworkPlaying() && !pRecord->bResultShow )
		return false;

	if( pRecord->pChar->GetState() == CS_OBSERVER )
		return false;

	rkInfo.m_szName	 = pRecord->pChar->GetCharName();
	rkInfo.m_Team		 = pRecord->pChar->GetTeam();
	rkInfo.m_iLevel      = pRecord->pChar->GetLevel();

	rkInfo.m_iTotalKill  = pRecord->GetAllTotalKill();
	rkInfo.m_iTotalDeath = pRecord->GetAllTotalDeath();

	rkInfo.m_iUniqueTotalKill  = pRecord->GetUniqueTotalKill();
	rkInfo.m_iUniqueTotalDeath = pRecord->GetUniqueTotalDeath();

	rkInfo.m_iLadderPoint = g_UserInfoMgr.GetLadderPoint( rkInfo.m_szName );
	rkInfo.m_iVictories = g_VictoriesMgr.GetVictories( rkInfo.m_szName );

	rkInfo.m_iRankDiff   = pRecord->iPreRank - pRecord->iCurRank;

	if( rkInfo.m_szName == g_MyInfo.GetPublicID() )
		rkInfo.m_bOwner = true;
	else
		rkInfo.m_bOwner = false;
	
	g_PlayBonusMgr.GetUserPlayBonus( rkInfo.m_szName, rkInfo.m_bPCRoomBonus, rkInfo.m_bGuildBonus, rkInfo.m_bFriendBonus );
	g_UserInfoMgr.GetGuildInfo( rkInfo.m_szName, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark );
	return true;
}

bool ioPlayMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	ModeRecord *pRecord = FindModeRecord( iIndex );
	if( !pRecord )
		return false;

	if( !pRecord->pChar )
		return false;

	if( ToNpcChar( pRecord->pChar ) )
		return false;

	if( GetModeType() != MT_TRAINING && GetModeType() != MT_MYROOM && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) )
			return false;
	}

	if( pRecord->pChar->GetState() == CS_OBSERVER )
		return false;

	if( P2PNetwork::IsNetworkPlaying() && !pRecord->bResultShow )
		return false;

	rkInfo.m_szName				= pRecord->pChar->GetCharName();
	rkInfo.m_Team				= pRecord->pChar->GetTeam();
	rkInfo.m_bLevelUP			= pRecord->bResultLevelUP;
	rkInfo.m_iResultExp			= pRecord->iResultTotalExp;
	rkInfo.m_iResultPeso		= pRecord->iResultTotalPeso;
	rkInfo.m_iResultLadderPoint	= pRecord->iResultTotalLadderPoint;
	rkInfo.m_iTreasureCardCnt	= pRecord->iResultTreasureCardCount;

	if( rkInfo.m_bLevelUP )
		rkInfo.m_iLevel			= pRecord->iResultLevel;
	else
		rkInfo.m_iLevel			= pRecord->pChar->GetLevel();

	rkInfo.m_iRank				= pRecord->iCurRank;

	rkInfo.m_iPvEClassType		= pRecord->iPvEClassType;
	rkInfo.m_dwPvECharIndex		= pRecord->dwPvECharIndex;

	rkInfo.m_iTotalKill			= pRecord->GetAllTotalKill();
	rkInfo.m_iTotalDeath		= pRecord->GetAllTotalDeath();

	rkInfo.m_iUniqueTotalKill	= pRecord->GetUniqueTotalKill();
	rkInfo.m_iUniqueTotalDeath	= pRecord->GetUniqueTotalDeath();

	rkInfo.m_iLadderPoint		= g_UserInfoMgr.GetLadderPoint( rkInfo.m_szName );
	rkInfo.m_iVictories			= g_VictoriesMgr.GetVictories( rkInfo.m_szName );

	if( rkInfo.m_szName == g_MyInfo.GetPublicID() )
		rkInfo.m_bOwner = true;
	else
		rkInfo.m_bOwner = false;

	g_PlayBonusMgr.GetUserPlayBonus( rkInfo.m_szName, rkInfo.m_bPCRoomBonus, rkInfo.m_bGuildBonus, rkInfo.m_bFriendBonus );
	g_UserInfoMgr.GetGuildInfo( rkInfo.m_szName, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark );

	return true;
}

void ioPlayMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	if( pKillChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pAttRecord->pChar->GetTeam() ) * FLOAT05 );
			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, true, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				g_ModeUIController.SetKOMsg( pAttRecord->pChar->GetTeam(), pAttRecord->pChar->GetCharName(),
					                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
											 pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
						                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -fKillPoint, true, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

					g_ModeUIController.SetKOMsg( TEAM_NONE,	"",
						                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				}
			}

			if ( g_ModeUIController.IsCenterKillInfoWnd() )
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetNoDeathKillCount() );
			else
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
				g_ModeUIController.SetCenterKillUI( m_ModeType, pAttRecord->GetNoDeathKillCount(), pOwner,pKillChar->GetTeam(), pKillChar->GetCharName(),
													pDieChar->GetLastAttackerItemCode(), pDieChar->GetTeam(), pDieChar->GetCharName(), pKillChar->IsMale() );
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

			g_ModeUIController.SetKOMsg( TEAM_NONE, "",
				                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
			
			pDieRecord->pChar->SetDeathEmoticon();
		}
	}

	// Best Attacker 처리
	ioBaseChar *pBestAttChar = ToBaseChar( pBestAttacker );
	if( pBestAttChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pBestAttChar->GetCharName() );
		if( pAttRecord && pDieChar->GetTeam() != pBestAttChar->GetTeam() )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pBestAttChar->GetTeam() ) * FLOAT05 );
			pAttRecord->AddKill( GetModeType(), fKillPoint, true, true );
		}
	}
}

void ioPlayMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );

	if( pKillerChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillerChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pAttRecord->pChar->GetTeam() ) * FLOAT05 );
			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, false, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				g_ModeUIController.SetKOMsg( pAttRecord->pChar->GetTeam(), pAttRecord->pChar->GetCharName(),
					                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
											 pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
						                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				} 
				else
				{
					pAttRecord->AddKill( GetModeType(), -fKillPoint, false, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
						                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				}
			}

			if ( g_ModeUIController.IsCenterKillInfoWnd() )
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetNoDeathKillCount() );
			else
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
				g_ModeUIController.SetCenterKillUI( m_ModeType, pAttRecord->GetNoDeathKillCount(), pOwner,pKillerChar->GetTeam(), pKillerChar->GetCharName(),
												pDieChar->GetLastAttackerItemCode(), pDieChar->GetTeam(), pDieChar->GetCharName(), pKillerChar->IsMale() );
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

			g_ModeUIController.SetKOMsg( TEAM_NONE, "",
				                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
			
			pDieRecord->pChar->SetDeathEmoticon();
		}
	}

	// Best Attacker 처리
	ioBaseChar *pBestAttChar = ToBaseChar( pBestAttacker );
	if( pBestAttChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pBestAttChar->GetCharName() );
		if( pAttRecord && pDieChar->GetTeam() != pBestAttChar->GetTeam() )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pBestAttChar->GetTeam() ) * FLOAT05 );
			pAttRecord->AddKill( GetModeType(), fKillPoint, false, true );
		}
	}
}

void ioPlayMode::UpdateRoundWinRecord()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;

	UpdateUserRank();

	if( IsRedWin( m_WinTeamType ) )
		m_iRedRoundWinCnt++;
	else if( IsBlueWin( m_WinTeamType ) )
		m_iBlueRoundWinCnt++;
}

void ioPlayMode::UpdateUserRank()
{
	if( P2PNetwork::IsNetworkPlaying() )
		return;

	int iRecordCnt = GetRecordCharCnt();

	ResultInfoList vResultInfoList;
	vResultInfoList.reserve( iRecordCnt );
	for( int i = 0 ;i < iRecordCnt; i++ )
	{
		ResultInfo kInfo;
		if( GetResultInfo( i, kInfo ) )
		{
			vResultInfoList.push_back( kInfo );
		}
	}

	std::sort( vResultInfoList.begin(), vResultInfoList.end(), ResultInfoSort() );

	int iRank = 1;
	ResultInfoList::iterator iter;
	for( iter=vResultInfoList.begin() ; iter!=vResultInfoList.end() ; ++iter, ++iRank )
	{
		ModeRecord *pRecord = FindModeRecordCharName( iter->m_szName );

		if( pRecord )
		{
			if( pRecord->iPreRank == 0 )
			{
				pRecord->iPreRank = iRank;
			}
			else
			{
				pRecord->iPreRank = pRecord->iCurRank;
			}

			pRecord->iCurRank = iRank;
		}
	}
}

void ioPlayMode::SetCenterHelp( ioComplexStringPrinter &rkPrinter, DWORD dwFirstDelay, DWORD dwShowTime )
{
	CenterHelpWnd *pCenterHelp = dynamic_cast< CenterHelpWnd* >( g_GUIMgr.FindWnd( CENTER_HELP_WND ) );
	if( pCenterHelp )
		pCenterHelp->SetCenterHelp( rkPrinter, dwFirstDelay, dwShowTime );
}

void ioPlayMode::SetModeState( ModeState eState )
{
	ModeState ePreModeState = m_ModeState;
	m_ModeState = eState;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( eState )
	{
	case MS_READY:
		m_pCreator->NotifySpringDampUseSet( false );
		SetWinTeamType( WTT_NONE );
		m_dwReadyStartTime = dwCurTime;
		break;
	case MS_PLAY:
		PlayExtendSound();
		g_App.SetExLoadingType( ioApplication::LOADING_NONE );
		g_App.SetStepByConnectTry( false );
#ifndef _SHIPPING
		SingleSettingWnd::SetAutoShow( true );
#endif
		if( GetModeType() == MT_TRAINING && m_pCreator->GetPlazaModeType() == PT_GUILD && g_App.GetGuildStorage() )
		{
			g_BlockManager.StartUpdateBound();
			g_App.GetGuildStorage()->SendBlockInvenSync();
		}
		else if( GetModeType() == MT_HOUSE && g_App.GetHomeStorage() )
			g_App.GetHomeStorage()->SendBlockInvenSync();
		m_dwPlayStartTime = dwCurTime;
		if( m_bProtoTypeMode && m_pCreator )
		{
			m_pCreator->AddAllNPCSyncFirstUser();
		}
		break;
	case MS_RESULT:		
		m_bResultCameraChange = false;
		m_dwResultStartTime = dwCurTime;

		// Remove MapAreaWeapon
		RemoveMapAreaWeapon();
		break;
	}

	LOG.PrintTimeAndLog(0, "ModeType : %d - Mode State Change: %d", GetModeType(), (int)eState );

	if( ePreModeState == MS_NEW_CHALLENGER && m_ModeState != MS_NEW_CHALLENGER )
	{
		SetBlindMode( false );
	}
}

ioBaseChar* ioPlayMode::GetOwnerChar()
{
	return m_pCreator->GetOwnerChar();
}

const ioHashString& ioPlayMode::GetOwnerName()
{
	return m_pCreator->GetOwnerName();
}

TeamType ioPlayMode::GetOwnerTeam()
{
	return m_pCreator->GetOwnerTeam();
}

ioMapEffect* ioPlayMode::CreateMapEffect( const ioHashString &szFileName, const D3DXVECTOR3 &vScale )
{
	return m_pCreator->CreateMapEffect( szFileName, vScale );
}

ioMapEffect* ioPlayMode::CreateMapEffect( const ioHashString &rkFileName, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale )
{
	return m_pCreator->CreateMapEffect( rkFileName, vPos, vScale );
}

ioMapEffect* ioPlayMode::GetMapEffect( UniqueObjID eObjID )
{
	return m_pCreator->FindMapEffect( eObjID );
}

void ioPlayMode::DestroyMapEffect( UniqueObjID eObjID )
{
	m_pCreator->DestroyMapEffect( eObjID );
}

void ioPlayMode::SetWinTeamType( WinTeamType eWinTeam )
{
	m_WinTeamType = eWinTeam;
}

WinTeamType ioPlayMode::GetWinTeamType() const
{
	return m_WinTeamType;
}

void ioPlayMode::SetResultJoinState()
{
	if( !g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND ) )
		g_GUIMgr.ShowWnd( MIDDLE_JOINDELAY_WND );

	// Remove MapAreaWeapon
	// InitMode()에서 Set 되었을 것이기 때문에 여기서 Remove 체크
	RemoveMapAreaWeapon();

	// 모든 캐릭터를 관전 상태로 만든다.
	int iCharCount = GetRecordCharCnt();
	for(int i = 0;i < iCharCount;i++)
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( pChar == NULL ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) ) continue;

		pChar->SetState( CS_VIEW );
	}
}

bool ioPlayMode::IsCanExpGoToTarget( const ioHashString &szTarget )
{
	if( !szTarget.IsEmpty() )
		return false;

	ioBaseChar *pChar = GetRecordChar( szTarget );
	if( !pChar )	return false;

	if( IsWinTeam( m_WinTeamType, pChar->GetTeam() ) )
		return false;

	return true;
}

float ioPlayMode::GetMapHeight( float fXPos,
							    float fZPos,
								const ioGameEntity *pEntity,
								bool bMidPositionRay ) const
{
	if( m_pCreator )
		return m_pCreator->GetMapHeight( fXPos, fZPos, pEntity, bMidPositionRay );

	return 0.0f;
}

float ioPlayMode::GetMapHeightByRevial( float fXPos, float fZPos, const ioGameEntity *pEntity, bool bMidPositionRay ) const
{
	if( m_pCreator )
		return m_pCreator->GetMapHeightByRevial( fXPos, fZPos, pEntity, bMidPositionRay );

	return 0.0f;
}

void ioPlayMode::RenderPushEnableCursor()
{
	if( !m_pPushCursor )	return;

	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )	return;

	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	TeamType eOwnerTeam = pOwner->GetTeam();
	int iPushCnt = g_PushStructListMgr.GetPushStructCnt();
	for( int i=0 ; i<iPushCnt ; i++ )
	{
		ioPushStruct *pPush = g_PushStructListMgr.GetPushStruct( i );
		if( !pPush ) continue;

		if( pPush->GetState() != ioPushStruct::PS_DELAY )
			continue;

		if( !pOwner->IsItemCanPickUp() || !pPush->IsCanPushed( pOwner ) )
			continue;

		D3DXVECTOR3 vTop;
		vTop = pPush->GetWorldCollisionBox().GetCenter();
		vTop.y += pPush->GetWorldCollisionBox().GetExtents(1);

		int xPos = 0, yPos = 0;
		if( Setting::Check2DInScreen( vTop, xPos, yPos ) )
		{
			m_pPushCursor->Render( xPos, yPos );
		}
	}
}

void ioPlayMode::LoadPushCursor()
{
	SAFEDELETE( m_pPushCursor );
	m_pPushCursor = new ioAnimate2DImageRender;

	ioINILoader_e kLoader( "config/sp2_gui.ini" );

	DWORD dwFrameTime = kLoader.LoadInt_e( "symbol_attack_enable", "frame_time", 100 );
	
	m_pPushCursor->SetFrameTime( dwFrameTime );
	m_pPushCursor->SetAnimateType( ioAnimate2DImageRender::FT_DEFAULT );
	
	m_pPushCursor->AddImage( "BaseImage013", "TargetSword01" );
	m_pPushCursor->AddImage( "BaseImage013", "TargetSword02" );
	m_pPushCursor->AddImage( "BaseImage013", "TargetSword03" );
	m_pPushCursor->SetHorzAlign( "Center" );
	m_pPushCursor->SetVertAlign( "Bottom" );

	m_pPushCursor->ShowRender();
}

void ioPlayMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
{
	g_SoundMgr.StopSound( m_TimeOutSound, 0 );
	g_SoundMgr.StopStream();

	m_bRoundSetEnd = bRoundSetEnd;

	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		if( pOwner->GetState() != CS_DIE )
		{
			g_MyInfo.SetCharDied( pOwner->GetSelectCharArray(), false );
		}
	}

	m_pCreator->ClearCameraModeFixed( CAMERA_STYLE_MOVIE );
	m_pCreator->ClearCameraModeFixed( CAMERA_STYLE_VIEW );
	m_pCreator->ClearCameraModeFixed( CAMERA_STYLE_HOUSING );
	m_pCreator->SetMapCameraFixedObject( true );	

	SetWinTeamType( eTeam );
	SetModeState( MS_RESULT );
	UpdateRoundWinRecord();
	SetRoundEndMotion();
	SetRoundEndVictories();
	ClearRoundEndAllBuff();

	g_ObserverWndMgr.ObserverWndHide();

	m_pCreator->SetModeEndSceneCamera( true );

	g_PushStructListMgr.DestroyStructList();
	g_MachineStructMgr.DestroyMachineStructList();
	g_DummyCharMgr.DestroyDummyCharList();

	if( GetModeType() != MT_FOOTBALL )
		g_BallStructMgr.DestroyBallStructList();

	m_pCreator->DestroyWeaponList();
	m_pCreator->EnableWorldEntityFunctions( false );

	if( !m_bRoundSetEnd )
	{
		if( P2PNetwork::IsNetworkPlaying() )
		{
			SetResultScoreWnd();
		}
	}
	else
	{
		// Network시에는  ioPlayMode::OnFinalRoundResult 에서
		if( !P2PNetwork::IsNetworkPlaying() )
		{
			float fBonusArray[ioPlayMode::BA_MAX];
			for (int i = 0; i < ioPlayMode::BA_MAX; i++)
				fBonusArray[i] = 0.0f;

			IntVec vClassTypeList, vClassPointList;
			SetTotalScoreWnd( vClassTypeList, vClassPointList, FLOAT1, FLOAT1, fBonusArray );
		}
		
		// 전투방 옵저버X.
		if( g_BattleRoomMgr.IsBattleRoom() && !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		{
			if( GetModeType() != MT_MONSTER_SURVIVAL && GetModeType() != MT_DUNGEON_A )
			{
				g_QuestMgr.QuestOccurTerm( QO_BATTLE_PVP_FINAL_RESULT );
			}
			
			g_QuestMgr.QuestOccurTerm( QO_BATTLE_FINAL_RESULT );
		}
		else if( g_ShuffleRoomMgr.IsShuffleRoom() )
		{
			if( GetModeType() != MT_MONSTER_SURVIVAL && GetModeType() != MT_DUNGEON_A )
			{
				g_QuestMgr.QuestOccurTerm( QO_BATTLE_PVP_FINAL_RESULT );
			}

			g_QuestMgr.QuestOccurTerm( QO_BATTLE_FINAL_RESULT );
		}
		else if( g_RankBattleMgr.IsRankBattlePlaying() )
		{
			g_QuestMgr.QuestOccurTerm( QO_BATTLE_PVP_FINAL_RESULT );
			g_QuestMgr.QuestOccurTerm( QO_BATTLE_FINAL_RESULT );
		}
	}

	ClearObjectItem();

	// PC방 유저는 최초로 한번만 띄워 준다. - 창섭씨 요청에 의해서 블럭 2011.07.11 - PC방 기능 개발 참고
//	if( g_MyInfo.GetPCRoomAuthority() && PCRoomAuthorityWnd::IsAutoShowPossible() )
//	{
//		g_GUIMgr.ShowWnd( PCROOM_AUTHORITY_WND );
//	}
}

void ioPlayMode::SetRoundEndMotion()
{
	WinTeamType eWinTeamType = m_WinTeamType;
	RoundEndMotion eEndMotion;	

	// 마지막 라운드의 승패는 전체 게임 승패이다
	if( m_bRoundSetEnd )
	{
		if( m_iBlueRoundWinCnt > m_iRedRoundWinCnt )
			eWinTeamType = WTT_BLUE_TEAM;
		else if( m_iBlueRoundWinCnt < m_iRedRoundWinCnt )
			eWinTeamType = WTT_RED_TEAM;
		else 
			eWinTeamType = WTT_DRAW;
	}

	int iCharCnt = GetRecordCharCnt();	
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) continue;
		if( pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) ) continue;

		if( eWinTeamType == WTT_DRAW )
		{
			eEndMotion = RM_DRAW;
		}
		else
		{
			if( IsWinTeam( eWinTeamType, pChar->GetTeam() ) )
				eEndMotion = RM_WIN;
			else
				eEndMotion = RM_LOSE;
		}

		pChar->SetRoundEndMotion( eEndMotion );
		pChar->StartNoInputDelayTime( 10000 );
	}	
}

void ioPlayMode::SetRoundEndVictories()
{
	if( !m_bRoundSetEnd ) return;
	if( g_LadderTeamMgr.IsLadderTeam() ) return;

	WinTeamType eWinTeamType = WTT_DRAW;	
	if( m_iBlueRoundWinCnt > m_iRedRoundWinCnt )
		eWinTeamType = WTT_BLUE_TEAM;
	else if( m_iBlueRoundWinCnt < m_iRedRoundWinCnt )
		eWinTeamType = WTT_RED_TEAM;
    
	int iCharCnt = GetRecordCharCnt();	
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) continue;
		if( ToNpcChar( pChar ) ) continue;

		g_VictoriesMgr.SetVictories( pChar->GetCharName(), IsWinTeam( eWinTeamType, pChar->GetTeam() ) );
	}	
}

void ioPlayMode::SetAutoStartAlarmTime( DWORD dwTotalResultTime )
{
	if( !g_BattleRoomMgr.IsBattleRoom() ) return;
	if( !g_BattleRoomMgr.IsAutoModeStart() ) return;

	m_dwCurAutoStartAlarmTime = FRAMEGETTIME();
	m_dwAutoStartAlarmTime = dwTotalResultTime - g_BattleRoomMgr.GetAutoStartAlarmCheckTime();
}

bool ioPlayMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1080, 0 );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_USER_REVIVAL:
		ErrorReport::SetPosition( 1080, 1 );
		OnUserRevival( rkPacket );
		ErrorReport::SetPosition( 1080, 2 );
		return true;
	case STPK_ROUND_READY:
		ErrorReport::SetPosition( 1080, 3 );
		OnRoundReady( rkPacket );
		ErrorReport::SetPosition( 1080, 4 );
		g_GlobalTimer.SetWorkPing();
		ErrorReport::SetPosition( 1080, 5 );
		return true;
	case STPK_ROUND_START:
		ErrorReport::SetPosition( 1080, 6 );
		OnRoundStart( rkPacket );
		ErrorReport::SetPosition( 1080, 7 );
		return true;
	case STPK_ROUND_END:
		ErrorReport::SetPosition( 1080, 8 );
		OnRoundEnd( rkPacket );
		ErrorReport::SetPosition( 1080, 9 );
		return true;
	case STPK_PUSHSTRUCT_DIE:
		ErrorReport::SetPosition( 1080, 10 );
		OnPushStructDie( rkPacket );
		ErrorReport::SetPosition( 1080, 11 );
		return true;
	case STPK_PUSHSTRUCT_INFO:
		ErrorReport::SetPosition( 1080, 12 );
		OnPushStructInfo( rkPacket );
		ErrorReport::SetPosition( 1080, 13 );
		return true;
	case STPK_OBJECT_SUPPLY:
		ErrorReport::SetPosition( 1080, 14 );
		OnObjectSupply( rkPacket );
		ErrorReport::SetPosition( 1080, 15 );
		return true;
	case STPK_FINAL_ROUND_RESULT:
		ErrorReport::SetPosition( 1080, 16 );
		OnFinalRoundResult( rkPacket );
		ErrorReport::SetPosition( 1080, 17 );
		return true;
	case STPK_ROUND_JOIN:
		ErrorReport::SetPosition( 1080, 20 );
		OnRoundJoin( rkPacket );
		ErrorReport::SetPosition( 1080, 21 );
		return true;
	case STPK_ROUND_JOIN_VIEW:
		ErrorReport::SetPosition( 1080, 22 );
		OnRoundJoinView( rkPacket );
		ErrorReport::SetPosition( 1080, 23 );
		return true;
	case STPK_FIELD_ITEM_DELETE:
		ErrorReport::SetPosition( 1080, 24 );
		OnDeleteFieldItem( rkPacket );
		ErrorReport::SetPosition( 1080, 25 );
		return true;
	case STPK_MODE_SCORE:
		ErrorReport::SetPosition( 1080, 28 );
		OnScore( rkPacket );
		ErrorReport::SetPosition( 1080, 29 );
		return true;
	case STPK_CATCH_CHAR:
		ErrorReport::SetPosition( 1080, 30 );
		OnCatchChar( rkPacket );
		ErrorReport::SetPosition( 1080, 31 );
		return true;
	case STPK_ESCAPE_CATCH_CHAR:
		ErrorReport::SetPosition( 1080, 32 );
		OnEscapeCatchChar( rkPacket );
		ErrorReport::SetPosition( 1080, 33 );
		return true;
	case STPK_ROUND_TIME_STOP:
		ErrorReport::SetPosition( 1080, 34 );
		OnRoundTimeStop( rkPacket );
		ErrorReport::SetPosition( 1080, 35 );
		return true;
	case STPK_START_SELECT_CHAR:
		ErrorReport::SetPosition( 1080, 36 );
		OnStartSelectChar( rkPacket );
		ErrorReport::SetPosition( 1080, 37 );
		return true;
	case STPK_ZERO_HP:
		ErrorReport::SetPosition( 1080, 38 );
		OnZeroHP( rkPacket );
		ErrorReport::SetPosition( 1080, 39 );
		return true;
	case STPK_PUSHSTRUCT_CREATE:
		ErrorReport::SetPosition( 1080, 40 );
		OnPushStructCreate( rkPacket );
		ErrorReport::SetPosition( 1080, 41 );
		return true;
	case STPK_FINAL_RESULT_USER_INFO:
		ErrorReport::SetPosition( 1080, 42 );
		OnFinalResultUserInfo( rkPacket );
		ErrorReport::SetPosition( 1080, 43 );
		return true;
	case STPK_ROUND_JOIN_OBSERVER:
		ErrorReport::SetPosition( 1080, 44 );
		OnRoundJoinObserver( rkPacket );
		ErrorReport::SetPosition( 1080, 45 );
		return true;
	case STPK_USER_BONUS_SYNC:		
		ErrorReport::SetPosition( 1080, 46 );		
		OnUserBonusList( rkPacket );		
		ErrorReport::SetPosition( 1080, 47 );
		return true;	
	case STPK_TURN_START:
		ErrorReport::SetPosition( 1080, 48 );
		OnTurnStart( rkPacket );
		ErrorReport::SetPosition( 1080, 49 );
		return true;
	case STPK_MONSTER_SYNC_CHANGE:
		ErrorReport::SetPosition( 1080, 50 );
		OnMonsterSyncChange( rkPacket );
		ErrorReport::SetPosition( 1080, 51 );
		return true;
	case STPK_MONSTER_INFO_SYNC:
		ErrorReport::SetPosition( 1080, 52 );
		OnMonsterInfoSync( rkPacket );
		ErrorReport::SetPosition( 1080, 53 );
		return true;
	case STPK_ROUND_CONTINUE:
		ErrorReport::SetPosition( 1080, 54 );
		OnRoundContinue( rkPacket );
		ErrorReport::SetPosition( 1080, 55 );
		return true;
	case STPK_TURN_END:
		ErrorReport::SetPosition( 1080, 56 );
		OnTurnEnd( rkPacket );
		ErrorReport::SetPosition( 1080, 57 );
		return true;
	case STPK_BALLSTRUCT_INFO:
		ErrorReport::SetPosition( 1080, 58 );
		OnBallStructInfo( rkPacket );
		ErrorReport::SetPosition( 1080, 59 );
		return true;
	case STPK_BALLSTRUCT_REPOSITION:
		ErrorReport::SetPosition( 1080, 60 );
		OnBallStructReposition( rkPacket );
		ErrorReport::SetPosition( 1080, 61 );
		return true;
	case STPK_PUSHSTRUCT_OWNER_CLEAR:
		ErrorReport::SetPosition( 1080, 62 );
		OnPushStructDieByOwnerClear( rkPacket );
		ErrorReport::SetPosition( 1080, 63 );
		return true;
	case STPK_PUSHSTRUCT_INFO_ENTER:
		ErrorReport::SetPosition( 1080, 64 );
		OnPushStructInfoEnter( rkPacket );
		ErrorReport::SetPosition( 1080, 65 );
		return true;
	case STPK_TOURNAMENT_SUDDEN_DEATH:
		ErrorReport::SetPosition( 1080, 66 );
		OnSuddenDeath( rkPacket );
		ErrorReport::SetPosition( 1080, 67 );
		return true;
	case STPK_CREATE_MODE_ITEM:
		ErrorReport::SetPosition( 1080, 68 );
		OnCreateModeItem( rkPacket );
		ErrorReport::SetPosition( 1080, 69 );
		return true;
	case STPK_GET_MODE_ITEM:
		ErrorReport::SetPosition( 1080, 70 );
		OnGetModeItem( rkPacket );
		ErrorReport::SetPosition( 1080, 71 );
		return true;
	case STPK_SHUFFLE_REWARD_RESULT:
		ErrorReport::SetPosition( 1080, 72 );
		OnShuffleBonusRewardResult( rkPacket );
		ErrorReport::SetPosition( 1080, 73 );
	}

	return false;
}

void ioPlayMode::OnScore( SP2Packet &rkPacket )
{
}

void ioPlayMode::OnUserRevival( SP2Packet &rkPacket )
{
	DWORD dwSelectTime;
	ioHashString szName;
	rkPacket >> szName >> dwSelectTime >> m_dwRandomSeed;

	ioBaseChar *pChar = GetRecordChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnUserRevival - %s Not Exist User", szName.c_str() );
		return;       
	}

	if( pChar->IsOwnerChar() )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnUserRevival() - CharChange: %d", FRAMEGETTIME() );
		StartChangeCharacter( pChar->GetSelectCharArray(), dwSelectTime );
		SetViewRoundState( false );
	}

	D3DXVECTOR3 vRevivalPos = GetRevivalPos( pChar );
	pChar->SetWorldPosition( vRevivalPos );
	pChar->SetReadyState();

	if( pChar->IsOwnerChar() )
	{
		m_pCreator->ClearCameraModeFixed( CAMERA_STYLE_VIEW );
		m_pCreator->ClearCameraModeFixed( CAMERA_STYLE_MOVIE );
		m_pCreator->ClearCameraModeFixed( CAMERA_STYLE_HOUSING );
	}
}

void ioPlayMode::OnStartSelectChar( SP2Packet &rkPacket )
{
	int iSelectChar = 0;
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		iSelectChar = pOwner->GetSelectCharArray();
		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnStartSelectChar() - %d", FRAMEGETTIME() );
	}

	DWORD dwSelectTime;
	rkPacket >> dwSelectTime;	
	StartChangeCharacter( iSelectChar, dwSelectTime );
}

void ioPlayMode::OnModeSpecialInfo( SP2Packet &rkPacket )
{
}

void ioPlayMode::OnRoundReady( SP2Packet &rkPacket )
{
	ShowEndScoreWnd( false );
	SetBlindMode( false );

	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;
	rkPacket >> m_dwCurRoundDuration;

	if( m_iCurRoundCount == 1 )
	{
		g_MyInfo.ClearCharDiedInfo();
		g_MyInfo.ClearCharDiedPenaltyInfo();
		g_MyInfo.ClearCharJoinedInfo();
	}

	ClearModeData();
	ClearModeItem();
	ReStartMode();

	g_GUIMgr.HideWnd( MIDDLE_JOINDELAY_WND );
}

void ioPlayMode::OnRoundStart( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1080, 80 );

	rkPacket >> m_iCurRoundCount;

	ErrorReport::SetPosition( 1080, 81 );

	SetModeState( MS_PLAY );

	m_bIsCanCreateDummyType = true;

	ErrorReport::SetPosition( 1080, 82 );
}

void ioPlayMode::OnRoundEnd( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1080, 100 );

	int iWinTeam;
	rkPacket >> iWinTeam;
	rkPacket >> m_iRedRoundWinCnt;
	rkPacket >> m_iBlueRoundWinCnt;

#if defined( USE_GA )
	bool		bWin		= false;
	ioBaseChar *pGAOwner	= GetOwnerChar();
	if( pGAOwner )
	{
		if( IsWinTeam( (WinTeamType)iWinTeam, pGAOwner->GetTeam() ) )
			bWin = true;
		else
			bWin = false;
	}

	g_HttpMng.SetWin( bWin );
#endif

	ErrorReport::SetPosition( 1080, 101 );

	int i = 0;
	int iRecordCnt = GetRecordCharCnt();
	for( i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			pRecord->bResultShow = false;
		}
	}

	ErrorReport::SetPosition( 1080, 102 );

	rkPacket >> m_iRoundEndRemainUserCnt;
	for( i=0 ; i<m_iRoundEndRemainUserCnt ; i++ )
	{
		ioHashString szName;
		rkPacket >> szName;

		ModeRecord *pRecord = FindModeRecordCharName( szName );
		if( pRecord )
		{
			int iMyVictories;
			rkPacket >> iMyVictories;

			if( g_LadderTeamMgr.IsLadderTeam() )
				g_VictoriesMgr.SetVictories( szName, iMyVictories );

			int iKillCharCnt, iDeathCharCnt;
			pRecord->iKillInfoMap.clear();
			pRecord->iDeathInfoMap.clear();

			rkPacket >> iKillCharCnt;
			for( int k=0; k < iKillCharCnt; ++k )
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

			rkPacket >> pRecord->iCurRank;
			rkPacket >> pRecord->iPreRank;

			pRecord->bResultShow = true;
		}
		else
		{
			int iMyVictories;
			rkPacket >> iMyVictories;

			int iKillCharCnt, iDeathCharCnt;

			rkPacket >> iKillCharCnt;
			for( int k=0; k < iKillCharCnt; ++k )
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

			int iCurRank, iPreRank;
			rkPacket >> iCurRank >> iPreRank;
		}
	}

	ErrorReport::SetPosition( 1080, 103 );
	
	bool bRoundSetEnd;
	rkPacket >> bRoundSetEnd;

	// 기여도와 시상식을 동기화 시킬 유저
	bool bResultSync = false;
	int iResultSyncUser;
	rkPacket >> iResultSyncUser;
	for(i = 0;i < iResultSyncUser;i++)
	{
		ioHashString szUserName;
		rkPacket >> szUserName;

		if( szUserName == g_MyInfo.GetPublicID() )
			bResultSync = true;
	}

	SendOwnerDamageList( bRoundSetEnd );

	ErrorReport::SetPosition( 1080, 104 );

	EndChangeCharacter();

	ErrorReport::SetPosition( 1080, 105 );

	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		g_MyInfo.SetCharDiedPenalty( pOwner->GetSelectCharArray(), false );
	}

	ErrorReport::SetPosition( 1080, 106 );

	SetRoundEndInfo( (WinTeamType)iWinTeam, bRoundSetEnd );

	m_bIsCanCreateDummyType = false;

	ErrorReport::SetPosition( 1080, 107 );

	if( m_bRoundSetEnd )
	{
		ErrorReport::SetPosition( 1080, 108 );

		if( !m_CurChampName.IsEmpty() )
		{
			ioBaseChar *pChamp = GetRecordChar( m_CurChampName );
			if( pChamp )
			{
				pChamp->SetChampEmoticon( true, false );
			}
		}

		ErrorReport::SetPosition( 1080, 109 );

		if( bResultSync )
		{
			ErrorReport::SetPosition( 1080, 110 );

			// 최종 기여도를 서버로 전송한다.
			ContributeConvert();      //기여도 환산
			ContributeRoundAllPercent();

			ErrorReport::SetPosition( 1080, 111 );

			SP2Packet kPacket( CTPK_LAST_PLAYRECORD_INFO );
			FillLastPlayRecordInfo( kPacket );
			TCPNetwork::SendToServer( kPacket );

			ErrorReport::SetPosition( 1080, 112 );

			// 시상식 주역 4인을 서버로 전송한다.
			g_AwardMgr.SetFinalResult( (WinTeamType)iWinTeam );		
			int iRecordCnt = GetRecordCharCnt();
			for( i=0 ; i<iRecordCnt ; i++ )
			{
				FinalInfo kInfo;
				if( GetFinalInfo( i, kInfo ) )
				{
					g_AwardMgr.AddFinalInfo( kInfo );
				}
			}
			SP2Packet kPacket1( CTPK_AWARDING_RESULT );
			g_AwardMgr.FillAwardResult( kPacket1 );
			TCPNetwork::SendToServer( kPacket1 );

			ErrorReport::SetPosition( 1080, 113 );
		}
	}

	ErrorReport::SetPosition( 1080, 114 );
}

void ioPlayMode::SendOwnerDamageList( bool bLast )
{
	if( GetModeType() != MT_FOOTBALL )
	{
		if( m_DamageTableMap.empty() )
			return;
	}

	SP2Packet kPakcet( CTPK_CURRENT_DAMAGELIST );
	if( FillAllDamagedList( kPakcet ) || bLast )       //라스트때는 무조건 보낸다.
	{
		TCPNetwork::SendToServer( kPakcet );
	}
}

void ioPlayMode::OnPushStructInfo( SP2Packet &rkPacket )
{
	g_PushStructListMgr.DestroyStructList();
	PushStructSupply( rkPacket );
}

void ioPlayMode::OnPushStructCreate( SP2Packet &rkPacket )
{
	// Create Exception Check
	bool bCreate = true;
	rkPacket >> bCreate;

	int iExceptioCode = 0;
	if( !bCreate )
	{
		rkPacket >> iExceptioCode;
		if( iExceptioCode == FISHING_PLACE_DUPLICATE_STRUCT )
		{
			// 구조물 중복 생성 예외
			LOG.PrintTimeAndLog( 0, "%s - Exception Code : %d", __FUNCTION__, iExceptioCode );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		}
		else if( iExceptioCode == FISHING_PLACE_NON_HAVE_ITEM )
		{
			// 구조물 중복 생성 예외
			LOG.PrintTimeAndLog( 0, "%s - Exception Code : %d", __FUNCTION__, iExceptioCode );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		}
		// 낚시 UI에 사용중 체크 해제하자...
		FishingInventoryWnd *pWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
		if( pWnd )
			pWnd->ChangeSelectedUsingState();
		return;
	}

	// Delete
	int iDeleteIndex;
	rkPacket >> iDeleteIndex;

	ioPushStruct *pPush = g_PushStructListMgr.FindPushStruct( iDeleteIndex );
	ioBaseChar* pMyChar = m_pCreator->GetOwnerChar();
	if( pPush )
	{
		bool bUsingFishingGround = false;
		// 낚시터 삭제 시 내가 낚시하던 곳이었으면 낚시 상태 해제
		if( pMyChar && pMyChar->GetState() == CS_FISHING && pMyChar->CheckUsingFishGroundOwner( pPush->GetOwnerName(), iDeleteIndex ) )
		{
			pMyChar->InitUseFishGroundOwner();
			pMyChar->SetFishingEndState();
			bUsingFishingGround = true;
		}

		pPush->StructDie( true, true, bUsingFishingGround );
	}

	// Create
	int iCreateNum, iIndex;
	D3DXVECTOR3 vCreatePos;
	D3DXQUATERNION qtTargetRot;
	ioHashString szOwnerName, szItemName;
	DWORD dwCreateGapTime;
	DWORD dwSeed, dwEtcCode;
	
	rkPacket >> iCreateNum;
	rkPacket >> iIndex;
	rkPacket >> vCreatePos;
	rkPacket >> qtTargetRot;
	rkPacket >> szOwnerName;
	rkPacket >> szItemName;
	rkPacket >> dwCreateGapTime;
	rkPacket >> dwSeed;
	rkPacket >> dwEtcCode;

	if( iCreateNum <= 0 )
		return;

	ioPushStruct *pNewPush = m_pCreator->CreatePushStruct( iCreateNum,
														   iIndex,
														   vCreatePos,
														   qtTargetRot,
														   szOwnerName,
														   dwCreateGapTime,
														   dwSeed,
														   dwEtcCode );


	ioBaseChar *pCreateObjOwner = m_pCreator->GetBaseChar( szOwnerName );

	if( pNewPush )
		pNewPush->SetAlphaRateDirect( MAX_ALPHA_RATE );

	if( pNewPush && !szItemName.IsEmpty() )
	{
		pNewPush->SetCreateItem( szItemName );

		ioItem *pItem = NULL;

		if( pCreateObjOwner )
		{
			pItem = pCreateObjOwner->GetEquipedItem( szItemName );
		}
		
		float fValue = 0.0f;
		if( pItem )
		{
			int iGrowthNum = 0;
			int iGrowthInfoNum = pItem->GetGrowthInfoNumByType( GT_PUSHSTRUT_HP_UP, iGrowthNum );

			int iLevel = 0;
			int iClassType = 0;

			if( pCreateObjOwner )
			{
				iClassType = pCreateObjOwner->GetCharacterInfo().m_class_type;
			}

			ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( szOwnerName );
			if( pLevel )
			{
				int iSlot = pItem->GetType() - 1;
				iLevel = pLevel->GetItemGrowthLevel( iClassType, iSlot, false );
				
				if( pItem )
				{
					int nReinforce = pItem->GetItemReinforce();
					//강화레벨에 따른 추가 스탯
					if ( nReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
						iLevel += nReinforce;
					else
						iLevel += g_GrowthInfoMgr.GetReinforceAddStat( nReinforce );
				}

				pCreateObjOwner->GetMedalItemGrowth( iLevel, false, iSlot );

				//펫 성장
				pCreateObjOwner->GetPetGrowth( iLevel, false, iSlot );

				//각성
				pCreateObjOwner->GetAwakeCharGrowth( iLevel, iSlot );

				ioGrowthUpInfo *pUpInfo = g_GrowthInfoMgr.GetGrowthUpInfo( iGrowthInfoNum, iLevel );
				ioGrowthNormalUpInfo *pNormal = ToNormalUpInfo( pUpInfo );
				if( pNormal )
					fValue = pNormal->GetValue( m_pCreator );
			}
		}

		float fMaxHp = pNewPush->GetHP().m_fMaxValue;
		float fCurHp = pNewPush->GetHP().m_fCurValue;

		fMaxHp += fValue;
		fCurHp += fValue;

		pNewPush->SetHP( fCurHp, fMaxHp );
	}

	if( pNewPush )
	{
		ioBaseChar *pOwner = m_pCreator->GetBaseChar( szOwnerName );
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( dwEtcCode );
		if( pOwner && pOwner->IsNeedProcess() && pEtcItem )
		{
			// 광장/훈련소/길드본부/개인본부 체크
			bool bTraining = false;
			bool bMyRoom = false;
			if( GetModeType() == MT_TRAINING )
				bTraining = true;

			if( GetModeType() == MT_MYROOM && ioMyRoomMode::GetMyRoomType() == MRT_PRACTICE )
				bMyRoom = true;
			else if( GetModeType() == MT_HEADQUARTERS )
				bMyRoom = true;
			else if( GetModeType() == MT_HOUSE )
				bMyRoom = true;

			int iRoomMax;
			if( bTraining )
				iRoomMax = g_PushStructListMgr.GetBlockMaxCnt();
			else
				iRoomMax = g_PushStructListMgr.GetBlockMyRoomMaxCnt();

			int iRoomCur = g_PushStructListMgr.GetCurPushStructCntByType( PSTT_BLOCK );


			int iEtcMax = g_PushStructListMgr.GetBlockEtcMaxCnt();
			int iEtcCur = g_PushStructListMgr.GetCurPushStructCntByEtcCode( pOwner, dwEtcCode );

			char szMsg[MAX_PATH*2]=""; 
			if( pNewPush && pNewPush->GetStructType() == PSTT_FISHING_GROUND )
			{
				SafeSprintf( szMsg, sizeof( szMsg ), STR(2), pEtcItem->GetName().c_str() );
			}
			else
			{
				SafeSprintf( szMsg, sizeof( szMsg ), STR(1), pEtcItem->GetName().c_str(),
					iEtcCur, iEtcMax );
			}
			
			g_ChatMgr.SetSystemMsg( szMsg );
		}
	}
}

void ioPlayMode::OnPushStructDie( SP2Packet &rkPacket )
{
	int iStructCnt, iPushIndex;
	bool bEffect;
	rkPacket >> iStructCnt;

	for( int i=0; i < iStructCnt; ++i )
	{
		rkPacket >> iPushIndex;
		rkPacket >> bEffect;

		// Create
		int iPushCnt;
		rkPacket >> iPushCnt;
		for( int j=0; j < iPushCnt; ++j )
		{
			int iCreateNum, iIndex;
			D3DXVECTOR3 vCreatePos;
			D3DXQUATERNION qtTargetRot;
			ioHashString szOwnerName;
			DWORD dwCreateGapTime;
			DWORD dwSeed;

			rkPacket >> iCreateNum;
			rkPacket >> iIndex;
			rkPacket >> vCreatePos;
			rkPacket >> qtTargetRot;
			rkPacket >> szOwnerName;
			rkPacket >> dwCreateGapTime;
			rkPacket >> dwSeed;

			if( iCreateNum <= 0 )
				continue;

			m_pCreator->CreatePushStruct( iCreateNum,
										  iIndex,
										  vCreatePos,
										  qtTargetRot,
										  szOwnerName,
										  dwCreateGapTime,
										  dwSeed,
										  0 );
		}

		ioPushStruct *pPush = g_PushStructListMgr.FindPushStruct( iPushIndex );
		if( pPush )
		{
			// 낚시터 삭제 시 내가 낚시하던 곳이었으면 낚시 상태 해제
			ioBaseChar* pMyChar = m_pCreator->GetOwnerChar();
			bool bUsingFishingGround = false;
			if( pMyChar && pMyChar->GetState() == CS_FISHING && pMyChar->CheckUsingFishGroundOwner( pPush->GetOwnerName(), iPushIndex ) )
			{
				pMyChar->InitUseFishGroundOwner();
				pMyChar->SetFishingEndState();
				bEffect = true;
				bUsingFishingGround = true;
			}

			FishingInventoryWnd *pWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
			if( pWnd )
				pWnd->InitCheckDieFishingGround( pPush->GetCreateEtcCode() );

			pPush->StructDie( bEffect, true, bUsingFishingGround );
		}
	}
}

void ioPlayMode::OnFinalResultUserInfo( SP2Packet &rkPacket )
{
	int iUserSize;
	rkPacket >> iUserSize;
	for(int i = 0;i < iUserSize;i++)
	{
		ioHashString szName;
		int iContribute, iKillDeathLevel, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		bool bSafetyLevel;
		rkPacket >> szName >> iContribute >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iKillDeathLevel >> bSafetyLevel;
		ModeRecord *pRecord = FindModeRecordCharName( szName );
		if( pRecord )
		{
			ContributeResultSync( szName, iContribute );
			pRecord->iUniqueTotalKill  = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath = iUniqueTotalDeath;
			g_VictoriesMgr.SetVictories( szName, iVictories );
		}		
		if( g_BattleRoomMgr.IsBattleRoom() )
		{
			g_BattleRoomMgr.UpdateUserAbilityLevel( szName, iKillDeathLevel );
			g_BattleRoomMgr.UpdateUserSafetyLevel( szName, bSafetyLevel );
		}
		else if( g_ShuffleRoomMgr.IsShuffleRoom() )
		{
			g_ShuffleRoomMgr.UpdateUserAbilityLevel( szName, iKillDeathLevel );
		}
	}
}

void ioPlayMode::OnUserBonusList( SP2Packet &rkPacket )
{
	int iUserCnt;
	rkPacket >> iUserCnt;
	for(int i = 0;i < iUserCnt;i++)
	{
		ioHashString szName;
		bool bPCRoomBonus, bGuildBonus, bFriendBonus;
		rkPacket >> szName >> bPCRoomBonus >> bGuildBonus >> bFriendBonus;
		g_PlayBonusMgr.SetUserPlayBonus( szName, bPCRoomBonus, bGuildBonus, bFriendBonus );  
	}
}

void ioPlayMode::OnFinalRoundResult( SP2Packet &rkPacket )
{
	// 최종 결과 패킷을 받을 때 시상식 Show함 . 내부적으로 수상자가 없으면 Hide시킨다.
	g_GUIMgr.HideWnd( BRIEFING_WND );

	DWORD dwTotalResultTime;
	rkPacket >> dwTotalResultTime;
	SetAutoStartAlarmTime( dwTotalResultTime );

	float fBlueGuildTeamBonus = FLOAT1;
	float fRedGuildTeamBonus = FLOAT1;
	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		DWORD dwBlueTeamIndex, dwRedTeamIndex;
		int iBluePrevRank, iBlueCurRank, iRedPrevRank, iRedCurRank;
		rkPacket >> dwBlueTeamIndex >> iBluePrevRank >> iBlueCurRank;
		rkPacket >> dwRedTeamIndex >> iRedPrevRank >> iRedCurRank;
		g_LadderTeamMgr.SetTeamRanking( dwBlueTeamIndex, iBlueCurRank, iBluePrevRank - iBlueCurRank );
		g_LadderTeamMgr.SetTeamRanking( dwRedTeamIndex, iRedCurRank, iRedPrevRank - iRedCurRank );

		// 양팀 길드팀 진영 보너스
		rkPacket >> fBlueGuildTeamBonus >> fRedGuildTeamBonus;

		// 래더전 경험치
		int iHeroExpert;
		rkPacket >> iHeroExpert;
		g_MyInfo.SetHeroExpert( iHeroExpert );
	}

	g_MyInfo.ApplyRoundEndInfo( rkPacket );
	
	IntVec vClassTypeList;
	IntVec vClassPointList;
	
	// 클래스 경험치
	int i = 0;
	int iCount;
	rkPacket >> iCount;
	for( i=0;i < iCount;i++ )
	{
		int iClassType, iClassPoint;
		rkPacket >> iClassType >> iClassPoint;
		if( iClassType == 0 )
			continue;

		vClassTypeList.push_back( iClassType );
		vClassPointList.push_back( iClassPoint );

		g_MyInfo.AddClassExp( iClassType, iClassPoint );
		float fGradeExp = g_MyInfo.GetSoldierExpBonusByGradeExp( iClassType, iClassPoint );
		//g_MyInfo.AddGradeExp( fGradeExp );
	}	
	g_MyInfo.GetFinalResultChar( vClassTypeList );

	
	//보너스 페서 (연승전 )
	if( m_ModeType == MT_SUCCESSION )
	{
		int iSuccessionBonusMoney;
		rkPacket >> iSuccessionBonusMoney;
	}
	

	// 내 정보
	__int64 iMyMoney;
	rkPacket >> iMyMoney;
	g_MyInfo.SetMoney( iMyMoney );

	float fBonusArray[ioPlayMode::BA_MAX];
	for (i = 0; i < ioPlayMode::BA_MAX; i++)
		rkPacket >> fBonusArray[i];

	// 플레이 유저 결과 정보
	int iPlayUserCnt;
	rkPacket >> iPlayUserCnt;

	for(i = 0;i < iPlayUserCnt;i++)
	{
		bool bLevelUP;
		ioHashString szName;
		int iTotalExp, iTotalPeso, iGradeLevel, iLadderPoint, iResultLadderPoint;
		rkPacket >> szName >> iTotalExp >> iTotalPeso >> bLevelUP >> iGradeLevel >> iLadderPoint >> iResultLadderPoint;

		ModeRecord *pRecord = FindModeRecordCharName( szName );
		if( pRecord )
		{
			pRecord->bResultLevelUP  = bLevelUP;
			pRecord->iResultLevel    = iGradeLevel;
			pRecord->iResultTotalExp = iTotalExp;
			pRecord->iResultTotalPeso= iTotalPeso;
			pRecord->iResultTotalLadderPoint = iResultLadderPoint;
			g_UserInfoMgr.SetLadderPoint( szName, iLadderPoint );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioPlayMode::OnFinalRoundResult - Not Exist Record: %s", szName.c_str() );
		}

		if( szName == GetOwnerName() )
			g_MyInfo.AddGradeExp( iTotalExp );
	}

	SetTotalScoreWnd( vClassTypeList, vClassPointList, fBlueGuildTeamBonus, fRedGuildTeamBonus, fBonusArray );

	//
	bool bCheckNext;
	rkPacket >> bCheckNext;

	int iNextModeType = 0;
	int iNextModeSubNum = 0;
	int iNextMapIndex = 0;

	if( bCheckNext )
	{
		rkPacket >> iNextModeType;
		rkPacket >> iNextModeSubNum;
		rkPacket >> iNextMapIndex;
	}

	if( m_pCreator )
	{
		m_pCreator->ReserveNextModeType( (ModeType)iNextModeType, iNextModeSubNum, iNextMapIndex );
	}

	int iSpiritCode;
	int iSpiritCount;
	rkPacket >> iSpiritCode >> iSpiritCount;
	TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
	if( pTotalResult )			
		pTotalResult->SetSpiritRewardInfo( iSpiritCode, iSpiritCount );

#if defined( USE_GA )
	char chLabel[32] = {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLabel, "%d", iMyMoney );
	else
		SafeSprintf( chLabel, sizeof(chLabel), "%1", iMyMoney );

	// PESO_GET_BATTLE
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Peso"
		, "End"
		, chLabel
		, 1
		, "%2FPESO_GET_BATTLE" );
#endif
}

void ioPlayMode::OnRoundJoin( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() ) return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar ) return;

	if( pJoinChar->IsOwnerChar() )
	{
		SetModeState( (ModeState)iModeState );

		DWORD dwStateGap;
		rkPacket >> dwStateGap;

		DWORD dwStartTime = FRAMEGETTIME() - dwStateGap;
		switch( m_ModeState )
		{
		case MS_READY:
			m_dwReadyStartTime = dwStartTime;
			break;
		case MS_PLAY:
			m_dwPlayStartTime = dwStartTime;
			break;
		case MS_RESULT:
			m_dwResultStartTime = dwStartTime;
			break;
		}

		if( m_ModeState != MS_RESULT )
		{
			int iSelectChar = 0;
			
			iSelectChar = pJoinChar->GetSelectCharArray();
			pJoinChar->SetReadyState();

			DWORD dwSelectTime;
			rkPacket >> dwSelectTime;

			StartChangeCharacter( iSelectChar, dwSelectTime );

			if( g_BattleRoomMgr.IsBattleRoom() )
			{
				g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_PVP, GetModeType() );
			}
			else if( g_LadderTeamMgr.IsLadderTeam() )
			{
				g_QuestMgr.QuestCompleteTerm( QC_ENTER_CAMP_BATTLE, GetModeType() );
			}
			else if( g_ShuffleRoomMgr.IsShuffleRoom() )
			{
				g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_PVP, GetModeType() );
			}
			else if( g_RankBattleMgr.IsRankBattlePlaying() )
			{
				g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_PVP, GetModeType() );
			}
		}
		else
		{
			SetResultJoinState();
		}

		pJoinChar->SetState( CS_VIEW );

		g_ModeHelpMgr.InitModeHelp();
		g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_VIEW );

		rkPacket >> m_dwCurRoundDuration;
	}
	else
	{
		pJoinChar->SetReadyState();
		if( m_ModeState == MS_PLAY )
		{
			DWORD dwCurTime = FRAMEGETTIME();
			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
			{
				if( dwCurTime > m_dwPlayStartTime && dwCurTime - m_dwPlayStartTime >= 10000 )
					g_ChatMgr.SetJoinUserChatMsg( pJoinChar->GetCharName().c_str(), pJoinChar->GetTeam(), pJoinChar->GetLevel() );
			}
		}
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioPlayMode::OnRoundJoinView( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() ) return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar ) return;

	if( pJoinChar->IsOwnerChar() )
	{
		SetModeState( (ModeState)iModeState );

		DWORD dwStateGap;
		rkPacket >> dwStateGap;

		DWORD dwStartTime = FRAMEGETTIME() - dwStateGap;
		switch( m_ModeState )
		{
		case MS_READY:
			m_dwReadyStartTime = dwStartTime;
			break;
		case MS_PLAY:
			m_dwPlayStartTime = dwStartTime;
			break;
		case MS_RESULT:
			m_dwResultStartTime = dwStartTime;
			break;
		}

		int iSelectChar = 0;

		pJoinChar->SetState( CS_VIEW );

		g_ModeHelpMgr.InitModeHelp();
		g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_VIEW );

		SetBlindMode( true );
		g_GUIMgr.SetSkipUIByBlind( false );		

		g_ChatMgr.SetInfomationMsg( STR(1) );
		g_ChatMgr.SetInfomationMsg( STR(2) );

		rkPacket >> m_dwCurRoundDuration;
	}
	else if( m_ModeState == MS_PLAY )
	{
		ioBaseChar *pOwner = GetOwnerChar();
		if( pOwner )
		{
			g_ChatMgr.SetJoinUserChatMsg( pJoinChar->GetCharName().c_str(), pJoinChar->GetTeam(), pJoinChar->GetLevel() );
		}

		pJoinChar->SetState( CS_VIEW );
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioPlayMode::OnRoundJoinObserver( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() ) return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar ) return;

	if( pJoinChar->IsOwnerChar() )
	{
		SetModeState( (ModeState)iModeState );

		DWORD dwStateGap;
		rkPacket >> dwStateGap;

		DWORD dwStartTime = FRAMEGETTIME() - dwStateGap;
		switch( m_ModeState )
		{
		case MS_READY:
			m_dwReadyStartTime = dwStartTime;
			break;
		case MS_PLAY:
			m_dwPlayStartTime = dwStartTime;
			break;
		case MS_RESULT:
			m_dwResultStartTime = dwStartTime;
			break;
		}

		int iSelectChar = 0;

		pJoinChar->SetState( CS_OBSERVER );

		g_ModeHelpMgr.InitModeHelp();

		g_ChatMgr.SetInfomationMsg( STR(1) );
		g_ChatMgr.SetInfomationMsg( STR(2) );

		rkPacket >> m_dwCurRoundDuration;

		if( g_BattleRoomMgr.IsBattleRoom() )
		{
			g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_OBSERVER, GetModeType() );
		}
	}
	else if( m_ModeState == MS_PLAY )
	{
		pJoinChar->SetState( CS_OBSERVER );
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioPlayMode::OnRoundTimeStop( SP2Packet &rkPacket )
{
}

void ioPlayMode::OnSuddenDeath( SP2Packet &rkPacket )
{
	int iSuddenType;
	rkPacket >> iSuddenType;
	switch( iSuddenType )
	{
	case TOURNAMENT_SUDDEN_DEATH_TIME_END:
		{
			// 최종 기여도를 서버로 전송한다.
			ContributeConvert();      //기여도 환산
			ContributeRoundAllPercent();

			SP2Packet kPacket( CTPK_TOURNAMENT_SUDDEN_DEATH );
			kPacket << TOURNAMENT_SUDDEN_DEATH_CONTRIBUTE;
			FillLastPlayRecordInfo( kPacket );
			TCPNetwork::SendToServer( kPacket );
		}
		break;
	case TOURNAMENT_SUDDEN_DEATH_CONTRIBUTE:
		{
			// 최종 기여도
			float fBlueContribute, fRedContribute;
			rkPacket >> fBlueContribute >> fRedContribute;

			//
			BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
			if( pBriefingWnd )
			{
				pBriefingWnd->SetTournamentContributePoint( fBlueContribute * FLOAT100, fRedContribute * FLOAT100 );
			}

			TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
			if( pTotalResult )			
				pTotalResult->SetTournamentContributeScore( fBlueContribute, fRedContribute );			
		}
		break;
	}
}

bool ioPlayMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1080, 50 );

	switch( rkPacket.GetPacketID() )
	{
	case CUPK_PUSHSTRUCT_DAMAGE:
		ErrorReport::SetPosition( 1080, 51 );
		OnPushStructDamaged( rkPacket );
		ErrorReport::SetPosition( 1080, 52 );
		return true;
	case SUPK_MODE_PING:
		ErrorReport::SetPosition( 1080, 53 );
		OnModePing( rkPacket );
		ErrorReport::SetPosition( 1080, 54 );
		return true;
	case SUPK_CAMP_INFLUENCE_ALARM:
		ErrorReport::SetPosition( 1080, 55 );
		OnCampInfluenceAlarm( rkPacket );
		ErrorReport::SetPosition( 1080, 56 );
		return true;
	case SUPK_SERVER_ALARM_MENT:
		ErrorReport::SetPosition( 1080, 57 );
		OnServerAlarmMent( rkPacket );
		ErrorReport::SetPosition( 1080, 58 );
		return true;
	case CUPK_BALL_INFO:
		ErrorReport::SetPosition( 1080, 59 );
		ApplyBallStructInfo( rkPacket );
		ErrorReport::SetPosition( 1080, 60 );
		return true;
	}

	return false;
}

void ioPlayMode::OnCampInfluenceAlarm( SP2Packet &rkPacket )
{
	//임시 주석 처리
	/*
	int iCommand;
	rkPacket >> iCommand;
	switch( iCommand )
	{
	case CAMP_INFLUENCE_ALARM:
		{
			int iBlueCampPoint, iRedCampPoint;
			rkPacket >> iBlueCampPoint >> iRedCampPoint;
			g_CampMgr.SetCampPoint( CAMP_BLUE, iBlueCampPoint );
			g_CampMgr.SetCampPoint( CAMP_RED, iRedCampPoint );
			g_CampMgr.InfluenceAlarm();
		}
		break;
	case CAMP_TODAY_INFLUENCE_ALARM:
		{
			int iBlueTodayPoint, iRedTodayPoint;
			float fTodayBonus;
			rkPacket >> iBlueTodayPoint >> iRedTodayPoint >> fTodayBonus;
			
			ioHashString szBlueCampName;
			ioHashString szRedCampName;

			szBlueCampName.Clear();
			szRedCampName.Clear();

			TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
			if( pTournament != NULL )
			{
				szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
				szRedCampName = pTournament->GetCampName( CAMP_RED );
			}

			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );
			if( iBlueTodayPoint > iRedTodayPoint )
			{
				if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
				{
					kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szBlueCampName.c_str() );
					kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), fTodayBonus * FLOAT100 );
				}
				else
				{
					kPrinter.SetTextColor( TCT_DEFAULT_RED );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szRedCampName.c_str() );
					kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5) );
				}
			}
			else if( iRedTodayPoint > iBlueTodayPoint )
			{
				if( g_MyInfo.GetUserCampPos() == CAMP_RED )
				{
					kPrinter.SetTextColor( TCT_DEFAULT_RED );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szRedCampName.c_str() );
					kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(7), fTodayBonus * FLOAT100 );
				}
				else
				{
					kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szBlueCampName.c_str() );
					kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(9) );
				}
			}
			else
			{
				kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(10) );
			}
			g_ChatMgr.SetChatComplexString( STR(11), kPrinter );
		}
		break;
	}
	*/
}

void ioPlayMode::OnServerAlarmMent( SP2Packet &rkPacket )
{
	int iMentType;
	rkPacket >> iMentType;
	
	if( iMentType == UDP_SERVER_ALARM_EVENT )
	{
		int iClassType;
		ioHashString szEventID;
		rkPacket >> szEventID >> iClassType;

		if( g_MyInfo.GetPublicID() != szEventID )
		{
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );
			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szEventID.c_str(), g_MyInfo.GetClassName( iClassType ) );
			g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
		}
	}
	else if( iMentType == UDP_SERVER_ALARM_FISHING )
	{
		bool bEventFishing = false;
		bool bSpecial;
		int iItemCode;
		int iEventType, iEventValue1, iEventValue2;
		int iSpecialType, iSpecialValue1, iSpecialValue2;

		ioHashString szFishingID;
		rkPacket >> szFishingID >> iItemCode;
		rkPacket >> bEventFishing >> iEventType >> iEventValue1 >> iEventValue2;
		rkPacket >> bSpecial >> iSpecialType >> iSpecialValue1 >> iSpecialValue2;

		if( GetRecordChar( szFishingID ) == NULL )    //같은 룸에 없는 유저에게만 공지하고 이모티콘 출력시 채팅창에 출력한다.
		{
			ioHashString szItemName = g_FishingMgr.GetFishingItemName( iItemCode );
			if( bSpecial )
				szItemName = g_FishingMgr.GetFishingPresentItemName( iSpecialType, iSpecialValue1, iSpecialValue2 );
		
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(29), szFishingID.c_str() );

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(30) );

			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(31), szItemName.c_str() );

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(32) );

			if( bEventFishing )
			{
				ioHashString szEventPresent = g_FishingMgr.GetFishingPresentItemName( iEventType, iEventValue1, iEventValue2 );
				if( !szEventPresent.IsEmpty() )
				{
					kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

					kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4), szEventPresent.c_str() );

					kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(7) );
				}
			}

			g_ChatMgr.SetChatComplexString( STR(12), kPrinter );
		}
	}
	else if( iMentType == UDP_SERVER_ALARM_FISHING_SELL )
	{
		int iItemType;
		__int64 iSellPeso;
		ioHashString szFishingID, szItem;
		rkPacket >> szFishingID >> iItemType >> iSellPeso;

		szItem = g_FishingMgr.GetFishingItemName( iItemType );

		char szBuf[MAX_PATH] = "";
		Help::ConvertNumToStrComma( (int)iSellPeso, szBuf, sizeof( szBuf ) );

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(34), szFishingID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(35) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "SSS" );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(36) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(37), szItem.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(38) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(39), szBuf );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(40) );

		g_ChatMgr.SetChatComplexString( STR(13), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_GRADE_UP )
	{
		int iGradeLevel;
		ioHashString szGradeUpID;
		rkPacket >> szGradeUpID >> iGradeLevel;

		char szGradeName[MAX_PATH] = "";
		g_LevelMgr.GetGradeName( iGradeLevel, szGradeName, sizeof( szGradeName ), true, true );

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),szGradeUpID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(5) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szGradeName );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(6) );
		g_ChatMgr.SetChatComplexString( STR(17), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_LEVEL_UP )
	{
		int iLevel, iClassType;
		ioHashString szLevelUpID;
		byte iPowerUpCharGrade = 0;
		rkPacket >> szLevelUpID >> iClassType >> iLevel >> iPowerUpCharGrade;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),szLevelUpID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(8) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(9), g_MyInfo.GetClassName( iClassType, iPowerUpCharGrade ) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(10), iLevel );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(11) );
		g_ChatMgr.SetChatComplexString( STR(21), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_GOLD_ITEM_EVENT )
	{
		ioHashString szOwnerID;
		int iPresentType;
		int   iPresentValue1, iPresentValue2;
		rkPacket >> szOwnerID >> iPresentType >> iPresentValue1 >> iPresentValue2;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(25), szOwnerID.c_str(), 
			g_PresentMgr.GetPresentValue1Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str(),
			g_PresentMgr.GetPresentValue2Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() );
		g_ChatMgr.SetChatComplexString( STR(26), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_AWARD_LUCKY_PRESENT )
	{
		ioHashString szLuckyName;
		int iPresentType;
		int   iPresentValue1, iPresentValue2;
		rkPacket >> szLuckyName >> iPresentType >> iPresentValue1 >> iPresentValue2;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),szLuckyName.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(14) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(15), 
			g_PresentMgr.GetPresentValue1Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str(),
			g_PresentMgr.GetPresentValue2Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(16) );
		g_ChatMgr.SetChatComplexString( STR(27), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_LOSA_SEARCH_EVENT )
	{
		int iPresentPeso;
		ioHashString szSendID;
		rkPacket >> szSendID >> iPresentPeso;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),szSendID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(18) );


		char szConvertNum[MAX_PATH] = "";
		Help::ConvertNumToStrComma( iPresentPeso, szConvertNum, sizeof( szConvertNum ) );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(19), szConvertNum );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(20) );
		g_ChatMgr.SetChatComplexString( STR(28), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_COMPOUND_SUCCESS )
	{
		int iTargetReinforce;
		int iTargetItemCode;
		ioHashString szCharID;
		rkPacket >> szCharID >> iTargetItemCode >> iTargetReinforce;

		const ioItem *pItem = g_ItemMaker.GetItemConst( iTargetItemCode, __FUNCTION__ );
		if( !pItem )
			return;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(42), szCharID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(43) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(44), pItem->GetName().c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(45) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(46), iTargetReinforce );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(47) );

		g_ChatMgr.SetChatComplexString( STR(33), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_GROWTH_CATALYST_SUCCESS )
	{
		ioHashString szCharID;
		int iItemCode;
		rkPacket >> szCharID >> iItemCode;
		const ioItem *pItem = g_ItemMaker.GetItemConst( iItemCode, __FUNCTION__ );
		if( !pItem )
			return;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(236), szCharID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(237) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(238), pItem->GetName().c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(239) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(240) );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(241) );
		g_ChatMgr.SetChatComplexString( STR(242), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_ETC_ITEM )
	{
		ioHashString szSendID;
		int iPresentType = 0;
		int iPresentValue1 = 0;
		int iPresentValue2 = 0;
		DWORD dwEtcItemCode;
		rkPacket >> szSendID >> iPresentType >> iPresentValue1 >> iPresentValue2 >> dwEtcItemCode;

		ioEtcItem *pEtcItem = dynamic_cast<ioEtcItem*>( g_EtcItemMgr.FindEtcItem( dwEtcItemCode ) );
		if( pEtcItem == NULL ) 
			return;

		ioHashString sText2 = pEtcItem->GetAlarmMent1();
		ioHashString sText3 = pEtcItem->GetAlarmMent2();
		ioHashString sText4 = pEtcItem->GetAlarmMent3();

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szSendID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), sText2.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		if( iPresentType == PRESENT_PESO )
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), g_PresentMgr.GetPresentValue2Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() ); // GetPresentValue1Text, GetPresentValue2Text 같이 사용하면 페소 100페소 2번 표시되어 예외 처리
		else
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), sText3.c_str(), g_PresentMgr.GetPresentValue1Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str(), g_PresentMgr.GetPresentValue2Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), sText4.c_str() );

		g_ChatMgr.SetChatComplexString( STR(41), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_MONSTER_SURVIVAL_CLEAR )
	{
		ioHashString szLastKiller;
		int iClearFloor;
		rkPacket >> szLastKiller >> iClearFloor;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(49), szLastKiller.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(50) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(51), iClearFloor );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(52) );

		g_ChatMgr.SetChatComplexString( STR(48), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_RANDOM_DECO_PRESENT )
	{
		ioHashString szSendID;
		int iPresentType, iPresentValue1, iPresentValue2;
		rkPacket >> szSendID >> iPresentType >> iPresentValue1 >> iPresentValue2;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),szSendID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(53) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(54), g_PresentMgr.GetPresentValue1Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(55) );

		g_ChatMgr.SetChatComplexString( STR(56), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_EXTRAITEM )
	{
		ioHashString szSendID;
		int iMachineCode = 0;
		int iItemCode    = 0;
		int iPeriodTime  = 0;
		rkPacket >> szSendID >> iMachineCode >> iItemCode >> iPeriodTime;
		
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),szSendID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(57) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), g_PresentMgr.GetPresentValue1Text( PRESENT_EXTRAITEM_BOX, iMachineCode, 0 ).c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(58) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(65), g_PresentMgr.GetPresentValue1Text( PRESENT_EXTRAITEM, iItemCode, 0 ).c_str() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(67), g_PresentMgr.GetPresentValue2Text( PRESENT_EXTRAITEM, 0, iPeriodTime ).c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(59) );

		g_ChatMgr.SetChatComplexString( STR(71), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_MODE_BONUS )
	{
		float fFriendBonus, fPCRoomBonusExp, fPCRoomBonusPeso, fGuildBonus;
		rkPacket >> fFriendBonus >> fPCRoomBonusExp >> fPCRoomBonusPeso >> fGuildBonus;

		bool bComma = false;
		char szBonus[MAX_PATH] = "";
		if( fFriendBonus > 0.00f )
		{
			SafeSprintf( szBonus, sizeof( szBonus ), STR(60), (int)( (fFriendBonus * FLOAT100) + 0.001f ) );
			bComma = true;
		}

		if( fPCRoomBonusExp > 0.00f || fPCRoomBonusPeso > 0.00f )
		{
			char szTemp[MAX_PATH] = "";
			if( bComma )
				SafeSprintf( szTemp, sizeof( szTemp ), STR(243), (int)( (fPCRoomBonusExp * FLOAT100) + 0.001f ), (int)( (fPCRoomBonusPeso * FLOAT100) + 0.001f ) );
			else
				SafeSprintf( szTemp, sizeof( szTemp ), STR(244), (int)( (fPCRoomBonusExp * FLOAT100) + 0.001f ), (int)( (fPCRoomBonusPeso * FLOAT100) + 0.001f ) );
			strcat( szBonus, szTemp );			
			bComma = true;				
		}

		if( fGuildBonus > 0.00f )
		{
			char szTemp[MAX_PATH] = "";
			if( bComma )
				SafeSprintf( szTemp, sizeof( szTemp ), STR(63), (int)( (fGuildBonus * FLOAT100) + 0.001f ) );
			else
				SafeSprintf( szTemp, sizeof( szTemp ), STR(64), (int)( (fGuildBonus * FLOAT100) + 0.001f ) );
			strcat( szBonus, szTemp );			
			bComma = true;				
		}

		if( strlen( szBonus ) > 0 )
		{
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(66), szBonus );

			g_ChatMgr.SetChatComplexString( STR(72), kPrinter );
		}
	}
	else if( iMentType == UDP_SERVER_ALARM_EXCAVATION )
	{
		ioHashString szSendID;
		int iRewardType   = 0;
		int iIndex = 0;
		int iType = 0;
		int iGrade = 0;
		int iPrice = 0;
		int iMultiple = 0;
		rkPacket >> szSendID >> iRewardType >> iIndex >> iType >> iGrade >> iPrice >> iMultiple;

		g_ExcavationMgr.SetExcavatingSuccessAlarm( szSendID, iRewardType, iIndex, iType, iGrade, iPrice, iMultiple, true );
	}
	else if( iMentType == UDP_SERVER_ALARM_QUEST_COMPLETE )
	{
		ioHashString szCompleteID;
		DWORD dwMainIndex, dwSubIndex;
		rkPacket >> szCompleteID >> dwMainIndex >> dwSubIndex;

		QuestParent *pQuestParent = g_QuestMgr.GetQuest( dwMainIndex, dwSubIndex );
		if( !pQuestParent ) return;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szCompleteID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(73) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(74), pQuestParent->GetTitle().c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(75) );

		g_ChatMgr.SetChatComplexString( STR(76), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_TRAD_ENABLE )
	{
		ioHashString szUserName;
		int iItemCode, iReinforce;
		rkPacket >> szUserName >> iItemCode >> iReinforce;

		const ioItem *pItem = g_ItemMaker.GetItemConst( iItemCode, __FUNCTION__ );
		if( pItem )
		{
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szUserName.c_str() );

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(77) );

			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(78) );

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(79) );

			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(80), pItem->GetName().c_str(), iReinforce );

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(81) );

			g_ChatMgr.SetChatComplexString( STR(82), kPrinter );
		}
	}
	else if( iMentType == UDP_SERVER_ALARM_SUPERGASHAPON_PRESENT )
	{
		DWORD dwEtcItemType;
		ioHashString szSendID;		
		int iPresentIndex;
		rkPacket >> szSendID >> dwEtcItemType >> iPresentIndex;

		g_SuperGashaponMgr.PrintSuperGashaponChatAlaram( szSendID, dwEtcItemType, iPresentIndex );
	}
	else if( iMentType == UDP_SERVER_ALARM_NEW_SUPERGASHAPON_PRESENT )
	{
		DWORD dwEtcItemType, dwCategory, dwPackage;
		ioHashString szSendID;		
		rkPacket >> szSendID >> dwEtcItemType >> dwCategory >> dwPackage;

		g_MultiSuperGashaponMgr.PrintSuperGashaponChatAlaram( szSendID, dwEtcItemType, dwCategory, dwPackage );
	}
	else if( iMentType == UDP_SERVER_ALARM_ITEM_RECHARGE_SUCCESS )
	{
		ioHashString szUserName;
		int dwEtcItemIndex;
		int iItemCode;
		rkPacket >> szUserName >> dwEtcItemIndex >> iItemCode;

		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( dwEtcItemIndex );
		const ioItem *pItem = g_ItemMaker.GetItemConst( iItemCode, __FUNCTION__ );
		if( pEtcItem && pItem )
		{
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szUserName.c_str() );

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "님이 " );

			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), pEtcItem->GetName().c_str() );

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "를 사용하여 " );

			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), pItem->GetName().c_str() );

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), " 영구장비 변환에 성공했습니다." );

			g_ChatMgr.SetChatComplexString( "알림", kPrinter );
		}
	}
	else if( iMentType == UDP_SERVER_ALARM_COMPOUND_EXTRA_SUCCESS )
	{
		int iTargetReinforce;
		int iTargetItemCode;
		ioHashString szCharID;
		rkPacket >> szCharID >> iTargetItemCode >> iTargetReinforce;

		const ioItem *pItem = g_ItemMaker.GetItemConst( iTargetItemCode, __FUNCTION__ );
		if( !pItem )
			return;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(245), szCharID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(246) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(247), pItem->GetName().c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(248) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(249), iTargetReinforce );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(250) );

		g_ChatMgr.SetChatComplexString( STR(33), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_COMPOUND_EXTRA_FAIL )
	{
		int iTargetReinforce;
		int iTargetItemCode;
		ioHashString szCharID;
		rkPacket >> szCharID >> iTargetItemCode >> iTargetReinforce;

		const ioItem *pItem = g_ItemMaker.GetItemConst( iTargetItemCode, __FUNCTION__ );
		if( !pItem )
			return;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(251), szCharID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(252) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(253), pItem->GetName().c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(254) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(255), iTargetReinforce+1 );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(256) );

		g_ChatMgr.SetChatComplexString( STR(33), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_COSTUME )
	{
		ioHashString szSendID;
		int nMachineCode   = 0;
		int nCostumeCode   = 0;
		int nPresentValue2 = 0;
		rkPacket >> szSendID >> nMachineCode >> nCostumeCode >> nPresentValue2;
		
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),szSendID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(57) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), g_PresentMgr.GetPresentValue1Text( PRESENT_COSTUME_BOX, nMachineCode, 0 ).c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(58) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(65), g_PresentMgr.GetPresentValue1Text( PRESENT_COSTUME, nCostumeCode, nPresentValue2 ).c_str() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(67), g_PresentMgr.GetPresentValue2Text( PRESENT_COSTUME, 0, nPresentValue2 ).c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(59) );

		g_ChatMgr.SetChatComplexString( STR(71), kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_OAK_PRESENT )
	{
		int nCount	= 0;
		int eType	= 0;
		int Value1	= 0;
		int Value2	= 0;
		ioHashString szSendID;
		rkPacket >> szSendID >> nCount >> eType >> Value1 >> Value2;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%s", szSendID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "님이" );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%d개", nCount );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "의 검으로 " );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "<%s>", g_PresentMgr.GetPresentValue1Text( eType, Value1, Value2 ).c_str() );
		g_ChatMgr.SetChatComplexString( "알림", kPrinter );

		ioComplexStringPrinter kPrinter2;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter2 );
		kPrinter2.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter2.AddTextPiece( g_ChatMgr.GetChatFontScale(), "을(를) 획득했습니다." );
		g_ChatMgr.SetChatComplexString( "", kPrinter2 );
	}
	else if ( iMentType == UDP_SERVER_ALARM_OAK_RESULT)
	{
		int iLevel;
		bool bSuccess;
		ioHashString szSendID;
		rkPacket >> szSendID >> bSuccess >> iLevel;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%s", szSendID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "님이 오크통" );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%d단계", iLevel );

		if ( bSuccess )
		{
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "를 성공하였습니다." );
		}
		else
		{
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "를 실패하였습니다." );
		}

		g_ChatMgr.SetChatComplexString( "알림", kPrinter );
	}
	else if( iMentType == UDP_SERVER_ALARM_TIMEGATE_RESULT )
	{
		ioHashString szSendID;
		int iListType = 0;
		int iPresentType = 0;
		int iValue1 = 0;
		int iValue2 = 0;
		rkPacket >> szSendID >> iListType >> iPresentType >> iValue1 >> iValue2;

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szSendID.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "님이 타임게이트에서 " );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), g_PresentMgr.GetPresentValue1Text( iPresentType, iValue1, iValue2 ).c_str() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "(%s)", g_PresentMgr.GetPresentValue2Text( iPresentType, iValue1, iValue2 ).c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), " 소환에 성공했습니다." );

		g_ChatMgr.SetChatComplexString( "알림", kPrinter );
	}
}

void ioPlayMode::OnModePing( SP2Packet &rkPacket )
{
	if( m_ModeState != MS_PLAY )
		return;

	DWORD dwGap;
	rkPacket >> dwGap;

	m_dwPlayStartTime = FRAMEGETTIME() - dwGap;
}

void ioPlayMode::OnPushStructDamaged( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	int iPushArray;

	rkPacket >> szAttacker >> iPushArray;

	ioBaseChar *pAttacker = GetRecordChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnPushStructDamaged - %s Not Exist",
								szAttacker.c_str() );
		return;
	}

	ioPushStruct *pPush = g_PushStructListMgr.FindPushStruct( iPushArray );
	if( !pPush )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnPushStructDamaged - %d Not Exist PushStruct",
								iPushArray );
		return;
	}

	pPush->OnPushDamaged( rkPacket, pAttacker );
}

void ioPlayMode::OnCatchChar( SP2Packet &rkPacket )
{
	ioHashString szName;
	DWORD dwDuration;
	rkPacket >> szName >> dwDuration;

	ioHashString szTieEffect, szStateEffect, szEscapeEffect, szRope, szCreateChar;
	rkPacket >> szTieEffect >> szStateEffect >> szEscapeEffect >> szRope >> szCreateChar;

	ioBaseChar *pChar = GetRecordChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnCatchChar - %s Not Exist", szName.c_str() );
		return;
	}

	pChar->ApplyCatchState( dwDuration,
							szTieEffect,
							szStateEffect,
							szEscapeEffect,
							szRope,
							szCreateChar );
}

void ioPlayMode::OnEscapeCatchChar( SP2Packet &rkPacket )
{
	ioHashString szName;
	bool bEscapeFail;
	rkPacket >> szName >> bEscapeFail;

	ioBaseChar *pChar = GetRecordChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnEscapeCatchChar - %s Not Exist", szName.c_str() );
		return;
	}

	pChar->ApplyEscapeCatchState( bEscapeFail );
}

D3DXVECTOR3 ioPlayMode::GetStartPos( int iPosArray, bool bRandom ) const
{
	if( m_vStartPos.empty() )
		return m_vMapCenter;

	int iMaxStartPos = m_vStartPos.size();
	if( !COMPARE( iPosArray, 0, iMaxStartPos ) )
		iPosArray = 0;

	D3DXVECTOR3 vCenter = m_vStartPos[iPosArray];
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot,
										ioMath::RangeRandom( 0.0f, D3DX_PI * 2.0f ),
										0.0f,
										0.0f );

	D3DXVECTOR3 vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );
    
	if( bRandom )
		vCenter += vMoveDir * ioMath::RangeRandom( FLOAT10, m_fRandomRange );
	
	vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );
	return vCenter;
}

D3DXVECTOR3 ioPlayMode::GetStartPos( TeamType eTeamType, bool bRandom )
{
	int iPosArray = -1;
	if( eTeamType == TEAM_BLUE  )
		iPosArray = m_iBluePosArray;
	else if(eTeamType == TEAM_RED )
		iPosArray = m_iRedPosArray;
	else if( !m_vStartPos.empty() )
	{
		int iMaxStartPos = m_vStartPos.size();
		iPosArray = rand()%iMaxStartPos;
	}

	return GetStartPos(iPosArray, bRandom );
}

void ioPlayMode::OnObjectSupply( SP2Packet &rkPacket )
{
	PushStructSupply( rkPacket );
	m_pCreator->OnFieldItemSupply( rkPacket );
}

void ioPlayMode::PushStructSupply( SP2Packet &rkPacket )
{
	int iStructCount;
	rkPacket >> iStructCount;

	int iNum, iIndex;
	D3DXVECTOR3 vCreatePos;
	D3DXQUATERNION qtTargetRot;
	ioHashString szOwnerName;
	DWORD dwCreateGapTime;
	DWORD dwSeed;
	DWORD dwEtcCode;

	for( int i=0; i<iStructCount; i++ )
	{
		rkPacket >> iNum;
		rkPacket >> iIndex;
		rkPacket >> vCreatePos;
		rkPacket >> qtTargetRot;
		rkPacket >> szOwnerName;
		rkPacket >> dwCreateGapTime;
		rkPacket >> dwSeed;
		rkPacket >> dwEtcCode;

		m_pCreator->CreatePushStruct( iNum,
									  iIndex,
									  vCreatePos,
									  qtTargetRot,
									  szOwnerName,
									  dwCreateGapTime,
									  dwSeed,
									  dwEtcCode );
	}
}

void ioPlayMode::BallStructSupply( SP2Packet &rkPacket )
{
	int iStructCount;
	rkPacket >> iStructCount;

	int iNum, iIndex;
	D3DXVECTOR3 vCreatePos;
	D3DXQUATERNION qtTargetRot;

	for( int i=0; i<iStructCount; i++ )
	{
		rkPacket >> iNum;
		rkPacket >> iIndex;
		rkPacket >> vCreatePos;
		rkPacket >> qtTargetRot;

		m_pCreator->CreateBallStruct( iNum, iIndex, vCreatePos, qtTargetRot );
	}
}

int ioPlayMode::GetRoundWinCnt( TeamType eTeamType )
{
	switch( eTeamType )
	{
	case TEAM_RED:
		return m_iRedRoundWinCnt;
	case TEAM_BLUE:
		return m_iBlueRoundWinCnt;
	}

	return 0;
}

void ioPlayMode::OnDeleteFieldItem( SP2Packet &rkPacket )
{
	int iDeleteItemCnt;
	rkPacket >> iDeleteItemCnt;

	for(int i = 0; i < iDeleteItemCnt; i++)
	{
		int iCreateIndex;
		rkPacket >> iCreateIndex;
		
		if(m_pCreator)
		{
			m_pCreator->HidingFieldItem( iCreateIndex );
		}
	}
}

bool ioPlayMode::CheckCharExist( const ioHashString &szCharName )
{
	ModeRecord *pRecord = FindModeRecordCharName( szCharName );
	if( pRecord )
		return true;

	return false;
}

void ioPlayMode::SetMiniMapEvent( const D3DXVECTOR3 &vPos, int iEvent, TeamType eTeam )
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( pMiniMap )
	{
		pMiniMap->AddEvent( vPos.x, vPos.z, (MiniMap::EventType)iEvent, eTeam );
	}
}

void ioPlayMode::PointUpEvent( TeamType eTeam )
{
	if( m_PointUpEmoticon.IsEmpty() ) return;

	int iSize = GetRecordCharCnt();
	for( int i=0 ; i<iSize; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;		
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;		
		if( pRecord->pChar->GetTeam() != eTeam ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;
		if( ioBaseChar::IsOnlyMyEmoticon() && !pRecord->pChar->IsOwnerChar() ) continue;

		pRecord->pChar->SetKillPointEmoticon( m_PointUpEmoticon, "", "", 0 );
	}
}

void ioPlayMode::SetOwnerDieUIProcess()
{
	g_GUIMgr.HideWnd( SOLDIER_SELECT_WND );
}

bool ioPlayMode::IsContributeAddPointItemCode( DWORD dwItemCode )
{
	int iSize = m_vContributeAddPointItemCode.size();
	for(int i = 0;i < iSize;i++)
	{
		if( m_vContributeAddPointItemCode[i] == dwItemCode )
			return true;
	}
	return false;
}

void ioPlayMode::ContributeLowerSort( ContributeInfoList &vInfo )
{
	if( vInfo.empty() )
		return;

	int iSize = vInfo.size();
	int i = 0;	
	for(i = 0;i < iSize;i++)
	{
		vInfo[i].m_iRank = i;
		for(int j = i + 1;j < iSize;j++)
		{
			if( vInfo[i].m_iContributePoint > vInfo[j].m_iContributePoint ) break;

			vInfo[i].m_iRank++;
		}		
	}
}

float ioPlayMode::ContributeTeamPer( TeamType eMyTeam, float fLimit )
{
	float fReturn = 0.0f;
	int iMyTeamCount = 0;
	int iOtherTeamCount = 0;

	int iSize = GetRecordCharCnt();
	for( int i=0 ; i<iSize; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;

		if( GetModeType() != MT_TRAINING && GetModeType() != MT_MYROOM && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
		{
			if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) )
				continue;
		}

		if( pRecord->pChar->GetTeam() == eMyTeam )
			iMyTeamCount++;
		else
			iOtherTeamCount++;
	}

	if( iMyTeamCount != 0 && iOtherTeamCount != 0 )
	{
		fReturn = (float)iMyTeamCount / iOtherTeamCount;
	}

	if( fReturn > fLimit )
		fReturn = fLimit;
	return fReturn;
}

float ioPlayMode::ContributeTeamPerA( TeamType eMyTeam, float fMaxLimit, float fMinLimit )
{
	float fReturn       = FLOAT1;
	int iMyTeamCount    = 0;
	int iOtherTeamCount = 0;

	int iSize = GetRecordCharCnt();
	for( int i=0 ; i<iSize; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;		
		if( pRecord->pChar->GetState() == CS_VIEW ) continue;
		if( pRecord->pChar->GetState() == CS_LOADING ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		
		if( pRecord->pChar->GetTeam() == eMyTeam )
			iMyTeamCount++;
		else
			iOtherTeamCount++;
	}

	if( iMyTeamCount != 0 && iOtherTeamCount != 0 )
	{
		fReturn = (float)iMyTeamCount / iOtherTeamCount;
	}

	if( fReturn < fMinLimit )
		fReturn = fMinLimit;
	else if( fReturn > fMaxLimit )
			fReturn = fMaxLimit;
	
	return fReturn;
}

float ioPlayMode::ContributeTeamPerB( TeamType eMyTeam, float fMaxLimit, float fMinLimit )
{
	float fReturn       = FLOAT1;
	int iMyTeamCount    = 0;
	int iOtherTeamCount = 0;

	int iSize = GetRecordCharCnt();
	for( int i=0 ; i<iSize; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;		
		if( pRecord->pChar->GetState() == CS_VIEW ) continue;
		if( pRecord->pChar->GetState() == CS_LOADING ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;

		if( pRecord->pChar->GetTeam() == eMyTeam )
			iMyTeamCount++;
		else
			iOtherTeamCount++;
	}
	iMyTeamCount -= 1;
	iOtherTeamCount -= 1;
	if( iMyTeamCount != 0 )
	{
		fReturn = (float)iOtherTeamCount / iMyTeamCount;
	}

	if( fReturn < fMinLimit )
		fReturn = fMinLimit;
	else if( fReturn > fMaxLimit )
		fReturn = fMaxLimit;

	return fReturn;
}

void ioPlayMode::RecordLOG()
{
	int iSize = GetRecordCharCnt();
	for( int i=0 ; i<iSize; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		
		char szLog[MAX_PATH] = "";
		SafeSprintf( szLog, sizeof( szLog), STR(1), pRecord->szName.c_str(), pRecord->GetUniqueTotalKill(), pRecord->GetUniqueTotalDeath(), g_VictoriesMgr.GetVictories( pRecord->szName ) );
		g_ChatMgr.SetSystemMsg( szLog );
	}
}

void ioPlayMode::ProcessWarningSound( DWORD dwDuration )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = FRAMEGETTIME() - m_dwPlayStartTime;

	int iWarningSoundSec = -9999999;
	if( m_dwCheckWarningTime == 0 )
	{
		if( m_dwStartWarningTime <= 0 ) return;
		if( dwDuration < dwGapTime ) return;

		if( dwDuration - dwGapTime < m_dwStartWarningTime )
		{
			m_dwCheckWarningTime = dwCurTime;
			iWarningSoundSec     = (dwDuration - dwGapTime) / 1000;
		}
	}
	else
	{
		if( dwCurTime - m_dwCheckWarningTime >= 1000 )
		{
			m_dwCheckWarningTime = dwCurTime;
			iWarningSoundSec     = (dwDuration - dwGapTime) / 1000;			
		}
	}
	
	if( iWarningSoundSec == -9999999 ) return;

	ioHashString szWarningSound = m_WarningSoundDef;
	if( COMPARE( iWarningSoundSec, 0, (int)m_WarningSound.size() ) )
	{
		if( !m_WarningSound[iWarningSoundSec].IsEmpty() )
			szWarningSound = m_WarningSound[iWarningSoundSec];
	}

	if( !szWarningSound.IsEmpty() )
		g_SoundMgr.PlaySound( szWarningSound );
}

void ioPlayMode::ProcessScoreWarningSound( bool bWarning )
{
	if( m_bScoreWarning && !bWarning && !m_ScoreWarningSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_ScoreWarningSound, 0 );
		m_bScoreWarning = false;
		return;
	}

	if( !m_bScoreWarning && bWarning && !m_ScoreWarningSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_ScoreWarningSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
		m_bScoreWarning = true;
		return;
	}
}

void ioPlayMode::ProcessScoreSound( TeamType eTeam, ScoreSound eScoreSound )
{
	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
	if( !pOwner ) return;

	if( pOwner->GetTeam() == eTeam )
	{
		m_PreScoreSound = m_ScoreSound;
		m_ScoreSound = eScoreSound;

		switch( m_ScoreSound )
		{
		case SS_FIRST:
			g_SoundMgr.PlaySound( m_FirstScoreSound );
			break;
		case SS_EQUAL:
			g_SoundMgr.PlaySound( m_EqualScoreSound );
			break;
		case SS_REVERSAL:
			g_SoundMgr.PlaySound( m_ReversalScoreSound );
			break;
		case SS_LAST:
			g_SoundMgr.PlaySound( m_LastScoreSound );
			break;
		default:
			g_SoundMgr.PlaySound( m_AddScoreSound );
			break;
		}
	}
	else
	{
		g_SoundMgr.PlaySound( m_LoseScoreSound );
	}
}

int ioPlayMode::GetCurPlayingTeamUserCnt( TeamType eTeam )
{
	int iUserCnt = 0;

	int iSize = GetRecordCharCnt();
	for(int i = 0;i < iSize;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;		
		if( pRecord->pChar->GetTeam() != eTeam )
			continue;

		switch( pRecord->pChar->GetState() )
		{
		case CS_DIE:
		case CS_VIEW:
		case CS_READY:
		case CS_LOADING:
		case CS_OBSERVER:
			break;
		default:
			iUserCnt++;
			break;
		}
	}

	return iUserCnt;
}

const char* ioPlayMode::GetModeINIPATH( ModeType eType )
{
	int iType = eType;
	if( iType == (int)MT_MYROOM || iType == (int)MT_NONE )
	{
		iType = MT_HEADQUARTERS;
	}

	ModePathMap::iterator iter = m_ModePathMap.find( iType );
	if( iter != m_ModePathMap.end() )
	{
		return iter->second.c_str();
	}

	LOG.PrintTimeAndLog( 0, "ioPlayMode::GetModeINIPATH - %d Unknown Mode", eType );

	return "";
}

const char* ioPlayMode::GetCurrentMapINIFileName()
{
	return GetModeMapINIFileName( GetModeType(), m_iModeSubNum, m_iModeMapIndex );
}

const char* ioPlayMode::GetModeINIFileName( ModeType eType )
{
	char szPath[MAX_PATH] = "";

	switch( eType )
	{
	case MT_TOWER_DEFENSE:
		wsprintf_e( szPath, "%ssp2_towerdefense_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_DARK_XMAS:
		wsprintf_e( szPath, "%ssp2_darkxmas_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_FIRE_TEMPLE:
		wsprintf_e( szPath, "%ssp2_firetemple_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_SYMBOL:
		wsprintf_e( szPath, "%ssp2_symbol_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_CATCH:
		wsprintf_e( szPath, "%ssp2_catch_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_CATCH_RUNNINGMAN:
		wsprintf_e( szPath, "%ssp2_catch_runningman_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_UNDERWEAR:
		wsprintf_e( szPath, "%ssp2_underwear_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_CBT:
		wsprintf_e( szPath, "%ssp2_cbt_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_KING:
		wsprintf_e( szPath, "%ssp2_hiddenking_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_TRAINING:
		wsprintf_e( szPath, "%ssp2_training_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_SURVIVAL:
		wsprintf_e( szPath, "%ssp2_survival_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_FIGHT_CLUB:
		wsprintf_e( szPath, "%ssp2_fight_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_TEAM_SURVIVAL:
		wsprintf_e( szPath, "%ssp2_teamsurvival_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_BOSS:
		wsprintf_e( szPath, "%ssp2_boss_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_MYROOM:
		wsprintf_e( szPath, "%ssp2_myroom_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_MONSTER_SURVIVAL:
		wsprintf_e( szPath, "%ssp2_monstersurvival_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_FOOTBALL:
		wsprintf_e( szPath, "%ssp2_football_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_HEROMATCH:
		wsprintf_e( szPath, "%ssp2_heromatch_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_GANGSI:
		wsprintf_e( szPath, "%ssp2_gangsi_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_DUNGEON_A:
		wsprintf_e( szPath, "%ssp2_dungeon_a_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_HEADQUARTERS:
		wsprintf_e( szPath, "%ssp2_headquarters_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_DOUBLE_CROWN:
		wsprintf_e( szPath, "%ssp2_doublecrown_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_SHUFFLE_BONUS:
		wsprintf_e( szPath, "%ssp2_shufflebonus_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_FACTORY:
		wsprintf_e( szPath, "%ssp2_factory_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_TEAM_SURVIVAL_AI:
		wsprintf_e( szPath, "%ssp2_teamsurvivalAI_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_HOUSE:
		wsprintf_e( szPath, "%ssp2_house_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_RAID:
		wsprintf_e( szPath, "%ssp2_raid_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_SUCCESSION:
		wsprintf_e( szPath, "%ssp2_succession_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_PRACTICE:
		wsprintf_e( szPath, "%ssp2_practice_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_FLAG_CAPTURE:
		wsprintf_e( szPath, "%ssp2_flagcapture_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_ARENA:
		wsprintf_e( szPath, "%ssp2_arena_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_BATTLE:
		wsprintf_e( szPath, "%ssp2_battle_mode.ini", GetModeINIPATH( eType ) );
		break;
	case MT_FARMING:
		wsprintf_e( szPath, "%ssp2_farming_mode.ini", GetModeINIPATH( eType ) );
		break;
	default:
		break;
	}

	m_ModePathName = szPath;
	if( !m_ModePathName.IsEmpty() )
		return m_ModePathName.c_str();

	LOG.PrintTimeAndLog( 0, "ioPlayMode::GetModeINIFileName - %d Unknown Mode", eType );

	return "";
}

const char* ioPlayMode::GetModeMapINIFileName( ModeType eType, int iSubNum, int iMapIndex )
{
	m_CurMapFileName.Clear();

	char szBuf[MAX_PATH];

	switch( eType )
	{
	case MT_SYMBOL:
		wsprintf_e( szBuf, "%ssp2_symbol_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_CATCH:
		{
			if( g_RankBattleMgr.IsRankBattlePlaying() )
			{
				wsprintf_e( szBuf, "%ssp2_heromatch_mode%d_map%d.ini", GetModeINIPATH( MT_HEROMATCH ), iSubNum, iMapIndex );
			}
			else
			{
				wsprintf_e( szBuf, "%ssp2_catch_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
			}

			m_CurMapFileName = szBuf;
			return m_CurMapFileName.c_str();
		}
	case MT_CATCH_RUNNINGMAN:
		wsprintf_e( szBuf, "%ssp2_catch_runningman_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_KING:
		wsprintf_e( szBuf, "%ssp2_hiddenking_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_SURVIVAL:
		wsprintf_e( szBuf, "%ssp2_survival_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FIGHT_CLUB:
		wsprintf_e( szBuf, "%ssp2_fight_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_TEAM_SURVIVAL:
		wsprintf_e( szBuf, "%ssp2_teamsurvival_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_TEAM_SURVIVAL_AI:
		wsprintf_e( szBuf, "%ssp2_teamsurvivalAI_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_BOSS:
		wsprintf_e( szBuf, "%ssp2_boss_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_TRAINING:
		wsprintf_e( szBuf, "%ssp2_training_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();	
	case MT_MYROOM:
		wsprintf_e( szBuf, "%ssp2_headquarters_map%d.ini", GetModeINIPATH( eType ), iMapIndex+1 );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_MONSTER_SURVIVAL:
		wsprintf_e( szBuf, "%ssp2_monstersurvival_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FOOTBALL:
		wsprintf_e( szBuf, "%ssp2_football_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_HEROMATCH:
		wsprintf_e( szBuf, "%ssp2_heromatch_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_GANGSI:
		wsprintf_e( szBuf, "%ssp2_gangsi_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_DUNGEON_A:
		wsprintf_e( szBuf, "%ssp2_dungeon_a_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_HEADQUARTERS:
		wsprintf_e( szBuf, "%ssp2_headquarters_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();	
	case MT_TOWER_DEFENSE:
		wsprintf_e( szBuf, "%ssp2_towerdefense_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_DARK_XMAS:
		wsprintf_e( szBuf, "%ssp2_darkxmas_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FIRE_TEMPLE:
		wsprintf_e( szBuf, "%ssp2_firetemple_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_DOUBLE_CROWN:
		wsprintf_e( szBuf, "%ssp2_doublecrown_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_SHUFFLE_BONUS:
		wsprintf_e( szBuf, "%ssp2_shufflebonus_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FACTORY:
		wsprintf_e( szBuf, "%ssp2_factory_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_HOUSE:
		wsprintf_e( szBuf, "%ssp2_house_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_RAID:
		wsprintf_e( szBuf, "%ssp2_raid_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();	
	case MT_SUCCESSION:
		wsprintf_e( szBuf, "%ssp2_succession_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_PRACTICE:
		wsprintf_e( szBuf, "%ssp2_practice_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FLAG_CAPTURE:
		wsprintf_e( szBuf, "%ssp2_flagcapture_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_ARENA:
		wsprintf_e( szBuf, "%ssp2_arena_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_BATTLE:
		wsprintf_e( szBuf, "%ssp2_battle_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FARMING:
		wsprintf_e( szBuf, "%ssp2_farming_mode%d_map%d.ini", GetModeINIPATH( eType ), iSubNum, iMapIndex );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	}

	LOG.PrintTimeAndLog( 0, "ioPlayMode::GetModeMapINIFileName - %d Unknown Mode", eType );

	return "";
}

void ioPlayMode::SetRankUI( int iRankType, bool bShow )
{
	if( g_GUIMgr.IsShow( ROUND_RESULT_MAIN_WND ) ||
		g_GUIMgr.IsShow( TOTAL_RESULT_MAIN_WND ) ||
		g_GUIMgr.IsShow( BRIEFING_WND ) ||
		IsRoundSetEnd() )
	{
		return;
	}
	
	ModeRankWnd *pModeRankWnd = dynamic_cast<ModeRankWnd*>( g_GUIMgr.FindWnd( MODE_RANK_WND ) );
	if( pModeRankWnd )
	{
		if( bShow )
		{
			if( pModeRankWnd->GetRankType() != ModeRankWnd::DEATH_TYPE )
				pModeRankWnd->SetRank( iRankType );
		}
		else if( pModeRankWnd->GetRankType() == iRankType )
			pModeRankWnd->HideWnd();
	}
}

void ioPlayMode::SetPlayRankUI( ioBaseChar *pChar, int iRankType, bool bShow )
{
	if( m_ModeState != MS_PLAY )
		return;

	if( P2PNetwork::IsNetworkPlaying() )
	{
		if( pChar && g_MyInfo.GetPublicID() == pChar->GetCharName() )
			SetRankUI( iRankType, true );
	}
}

// For Blind
void ioPlayMode::SetBlindMode( bool bStart, bool bBlindRender )
{
	if( !bStart || !bBlindRender )        
		SetRankUI( ModeRankWnd::DEATH_TYPE, false );

	if( !bStart && g_GUIMgr.IsNoneUIMode() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( bStart )
	{
		if( m_dwSetBlindModeTime > 0 ) return;
	
		m_dwSetBlindModeTime = dwCurTime;
		
		if( m_dwEndBlindModeStartTime > 0 )
			m_dwCurBlindSlerpTime = dwCurTime - m_dwEndBlindModeStartTime;
		else
			m_dwCurBlindSlerpTime = m_dwBlindSlerpTime;

		m_dwEndBlindModeStartTime = 0;

		g_SoundMgr.PlaySound( m_BlindMoveSound );

		g_GUIMgr.SetSkipUIByBlind( true );		
		// 배틀모드이고 자신의 용병이 죽은 상태가 아닌 경우 UI를 Skip하지 않음 
		ioBaseChar* pMyChar = m_pCreator->GetBaseChar(g_MyInfo.GetPublicID());
		if( GetModeType() == MT_BATTLE && pMyChar->GetState() != CS_DIE )
			g_GUIMgr.SetSkipUIByBlind( false );		
		m_bBlindRender = bBlindRender;

		ioTowerDefMode* pMode = ToTowerDefMode(this);
		if(pMode)
			pMode->SetReviveShortKey();
	}
	else
	{
		if( m_dwEndBlindModeStartTime > 0 || m_dwSetBlindModeTime == 0 ) return;

		if( m_fCurBlindHeightRate == FLOAT1 )
			m_dwCurBlindSlerpTime = m_dwBlindSlerpTime;
		else
			m_dwCurBlindSlerpTime = dwCurTime - m_dwSetBlindModeTime;

		m_dwSetBlindModeTime = 0;
		m_dwEndBlindModeStartTime = FRAMEGETTIME();
		g_SoundMgr.PlaySound( m_BlindMoveSound );
	}	
}

void ioPlayMode::ProcessBlindMode()
{
	if( m_dwBlindSlerpTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fHeightRate = 0.0f;

	if( m_dwSetBlindModeTime > 0 )
	{
		if( m_dwSetBlindModeTime+m_dwCurBlindSlerpTime > dwCurTime )
		{
			DWORD dwGap = m_dwBlindSlerpTime - m_dwCurBlindSlerpTime;
			dwGap += (dwCurTime - m_dwSetBlindModeTime);
			m_fCurBlindHeightRate = (float)dwGap / (float)m_dwBlindSlerpTime;
		}
		else
		{
			m_fCurBlindHeightRate = FLOAT1;
			m_pCreator->SetBlindAniEnd( true );
		}
	}
	else if( m_dwEndBlindModeStartTime > 0 )
	{
		if( m_dwEndBlindModeStartTime+m_dwCurBlindSlerpTime > dwCurTime )
		{
			DWORD dwGap = m_dwBlindSlerpTime - m_dwCurBlindSlerpTime;
			dwGap += (dwCurTime - m_dwEndBlindModeStartTime);
			fHeightRate = (float)dwGap / (float)m_dwBlindSlerpTime;
			m_fCurBlindHeightRate = FLOAT1 - fHeightRate;
		}
		else
		{
			m_dwEndBlindModeStartTime = 0;
			m_fCurBlindHeightRate = 0.0f;

			g_GUIMgr.SetSkipUIByBlind( false );
			m_pCreator->SetBlindAniEnd( true );
		}
	}
	else if( m_dwSetBlindModeTime == 0 && m_dwEndBlindModeStartTime == 0 )
	{
		m_fCurBlindHeightRate = 0.0f;
		g_GUIMgr.SetSkipUIByBlind( false );
		m_pCreator->SetBlindAniEnd( true );
	}
}

void ioPlayMode::RenderBlind()
{
	if( m_dwSetBlindModeTime == 0 && m_dwEndBlindModeStartTime == 0 )
		return;
	if( !m_bBlindRender )
		return;

	float fCurUpHeight = m_fUpBlindHeight * m_fCurBlindHeightRate;
	float fCurBottomHeight = m_fBottomBlindHeight * m_fCurBlindHeightRate;

	float fXPos, fYPos;
	fXPos = (float)Setting::Width() * FLOAT05;

	fYPos = fCurUpHeight * FLOAT05;
	if( m_pBlindUp )
	{
		m_pBlindUp->SetScale( (float)Setting::Width(), fCurUpHeight );
		m_pBlindUp->Render( fXPos, fYPos );
	}

	fYPos = Setting::Height() - fCurBottomHeight * FLOAT05;
	if( m_pBlindDown )
	{
		m_pBlindDown->SetScale( (float)Setting::Width(), fCurBottomHeight );
		m_pBlindDown->Render( fXPos, fYPos );
	}

	// 블라인드 애니메이션이 끝난 다음부터 표시
	if( m_fCurBlindHeightRate == FLOAT1 )
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( g_MyInfo.GetPublicID() );
		if( !pDieRecord ) return;
		if( !pDieRecord->pChar ) return;
		if( !m_pRevivalGaugeBack || !m_pRevivalGauge || !m_pTextRevival || !m_pTextRoundDelay ) return;

		int iGaugeWidth = Setting::Width() - 56;
		int iGaugeHeight= 27;
		if( pDieRecord->pChar->GetState() == CS_DIE && IsBlindWithRevivalMode() && !pDieRecord->pChar->IsGangsi() )
		{
			// 부활 게이지
			float fGaugeRate = 0.0f;
			if( pDieRecord->dwRevivalGap > 0 && pDieRecord->dwCurDieTime > 0 )
			{
				fGaugeRate = min( FLOAT1, (float)(FRAMEGETTIME() - pDieRecord->dwCurDieTime) / pDieRecord->dwRevivalGap );
			}
			
			if( fGaugeRate <= FLOAT1 )
			{
				m_pRevivalGaugeBack->SetSize( iGaugeWidth, iGaugeHeight );			
				m_pRevivalGaugeBack->Render( 28, Setting::Height() - 43 );
				if( fGaugeRate > 0.0f )
				{
					m_pRevivalGauge->SetSize( max( 11, (float)iGaugeWidth * fGaugeRate ), iGaugeHeight );			
					m_pRevivalGauge->Render( 28, Setting::Height() - 43 );
				}
				m_pTextRevival->Render( Setting::Width() / 2, Setting::Height() - 37, UI_RENDER_ADD );
			}
		}
		else if( pDieRecord->pChar->GetState() == CS_VIEW || pDieRecord->pChar->GetState() == CS_DIE )
		{
			//TODO : Tower 중간 참여 & 부활 안되는 모드
			if( GetModeType() != MT_MONSTER_SURVIVAL && GetModeType() != MT_DUNGEON_A && GetModeType() != MT_FIGHT_CLUB )
			{
				m_pRevivalGaugeBack->SetSize( iGaugeWidth, iGaugeHeight );			
				m_pRevivalGaugeBack->Render( 28, Setting::Height() - 43 );

				m_pTextRoundDelay->Render( Setting::Width() / 2, Setting::Height() - 37, UI_RENDER_ADD );
			}
		}
	}
}

bool ioPlayMode::IsBlindWithRevivalMode()
{
	switch( GetModeType() )
	{
	case MT_SYMBOL:
	case MT_KING:
	case MT_TRAINING:
	case MT_SURVIVAL:
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_BOSS:
	case MT_GANGSI:
	case MT_MYROOM:
	case MT_HEADQUARTERS:	
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_DOUBLE_CROWN:
	case MT_SHUFFLE_BONUS:
	case MT_FACTORY:
	case MT_RAID:
	case MT_HOUSE:
	case MT_FLAG_CAPTURE:
		return true;
	}

	return false;
}

void ioPlayMode::RenderPlayMode()
{
	RenderBlind();
}

bool ioPlayMode::GetBlindRect( RECT &rcTop, RECT &rcBottom )
{
	if( m_dwSetBlindModeTime == 0 && m_dwEndBlindModeStartTime == 0 )
		return false;
	if( !m_bBlindRender )
		return false;

	int iCurUpHeight = (int)( m_fUpBlindHeight * m_fCurBlindHeightRate );
	SetRect( &rcTop, 0, 0, Setting::Width(), iCurUpHeight );

	int iCurBottomHeight = (int)( m_fBottomBlindHeight * m_fCurBlindHeightRate );
	iCurBottomHeight = Setting::Height() - iCurBottomHeight / 2;

	SetRect( &rcBottom, 0, iCurBottomHeight, Setting::Width(), Setting::Height() );

	return true;
}

void ioPlayMode::CheckNonePlayTime( ioBaseChar *pChar, bool bPlayerChanged )
{
	if( !pChar )
		return;

	TeamType eOwnerTeam = pChar->GetTeam();
	TeamType eEnemyTeam = TEAM_NONE;

	if( eOwnerTeam == TEAM_BLUE )
		eEnemyTeam = TEAM_RED;
	else if( eOwnerTeam == TEAM_RED )
		eEnemyTeam = TEAM_BLUE;

	int iEnemyCount = GetCurPlayingTeamUserCnt( eEnemyTeam );
	int iTeamCount = GetCurPlayingTeamUserCnt( eOwnerTeam );
	int iTotalCount = iTeamCount + iEnemyCount;

	if( iTotalCount > 2 && iTeamCount == 1 )
	{
		pChar->CheckNonePlayTime( false, bPlayerChanged );
	}
	else
	{
		pChar->CheckNonePlayTime( true, false );
	}
}

void ioPlayMode::CheckOwnerNonePlayTime()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		CheckNonePlayTime( pOwner, true );
	}
}

void ioPlayMode::ClearObjectItem()
{
	int iCharCnt = GetRecordCharCnt();
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if(pChar)	
		{
			ioObjectItem *pObject = pChar->GetObject();
			if( pObject )
			{
				ioItem * pItem = pChar->ReleaseItemExplicit( pObject->GetItemCode(),
															 pObject->GetItemCreateIndex(),
															 ES_OBJECT );

				SAFEDELETE( pItem );
			}
		}
	}
}

bool ioPlayMode::IsCollisionCheck()
{
	if( GetModeState() == ioPlayMode::MS_RESULT )
		return false;

	return true;
}

int ioPlayMode::GetKillPoint( TeamType eMyTeam )
{
	if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS )
		return m_iKillDeathPoint;

	int iMyTeamCount    = 0;
	int iOtherTeamCount = 0;
	int iSize = GetRecordCharCnt();
	for( int i=0 ; i<iSize; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;	
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;

		if( GetModeType() != MT_TRAINING && GetModeType() != MT_MYROOM && GetModeType() != MT_HEADQUARTERS )
		{
			if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) )
				continue;
		}

		if( pRecord->pChar->GetTeam() == eMyTeam )
			iMyTeamCount++;
		else
			iOtherTeamCount++;
	}

	if( iMyTeamCount == 0 || iOtherTeamCount == 0 )
		return m_iKillDeathPoint;

	float fKillPer = (float)iMyTeamCount / iOtherTeamCount;
	if( fKillPer > FLOAT1 )
		return m_iKillDeathPoint;

	float fKillPoint = m_iKillDeathPoint;
	return ( fKillPoint * fKillPer ) + FLOAT05;
}

int ioPlayMode::GetDeathPoint( TeamType eMyTeam )
{
	if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS )
		return m_iKillDeathPoint;

	int iMyTeamCount    = 0;
	int iOtherTeamCount = 0;
	int iSize = GetRecordCharCnt();
	for( int i=0 ; i<iSize; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;				

		if( GetModeType() != MT_TRAINING && GetModeType() != MT_MYROOM && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
		{
			if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) )
				continue;
		}

		if( pRecord->pChar->GetTeam() == eMyTeam )
			iMyTeamCount++;
		else
			iOtherTeamCount++;
	}

	if( iMyTeamCount == 0 || iOtherTeamCount == 0 )
		return m_iKillDeathPoint;

	float fDeathPer = (float)iOtherTeamCount / iMyTeamCount;
	if( fDeathPer > FLOAT1 )
		return m_iKillDeathPoint;

	float fDeathPoint = m_iKillDeathPoint;
	return ( fDeathPoint * fDeathPer ) + FLOAT05;
}

void ioPlayMode::ProcessModeGuide()
{
	if( !m_pGuideArrow )	return;

	if( g_GUIMgr.IsNoneUIMode() )
	{
		m_pGuideArrow->SetVisible( false );
	}
	else
	{
		ioBaseChar *pOwner = GetOwnerChar();
		if( pOwner )
		{
			// NULL 타겟도 의미가 있어서 체크없이 실행
			ioGameEntity* pTarget = GetGuideTarget( pOwner->GetWorldPosition() );
			m_pGuideArrow->Process( pOwner, pTarget );
		}
	}
}

ioGameEntity* ioPlayMode::GetGuideTarget( const D3DXVECTOR3& vArrowPos )
{
	return NULL;
}

void ioPlayMode::CheckEndSceneLookAt()
{
	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
	if( pOwner )
	{
		m_vEndLookAt		= pOwner->GetWorldPosition();
		m_szEndLookCharName = pOwner->GetCharName();
		pOwner->SetEndFocusEffect( GetRoundEndEffect() );
	}
	else
	{
		m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_szEndLookCharName.Clear();
	}
}

void ioPlayMode::GetEndSceneCameraPos( float &fHeight, float &fDistance, float &fFov )
{
	fHeight = m_fEndHeight;
	fDistance = m_fEndDistance;
	fFov = m_fEndFov;
}

D3DXVECTOR3 ioPlayMode::GetEndSceneLookAt() const
{
	return m_vEndLookAt;
}

void ioPlayMode::ClearModeData()
{
	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
}

bool ioPlayMode::IsNeedRevival( ioBaseChar *pChar )
{
	return false;
}

bool ioPlayMode::IsEnableBalanceMsg()
{
	if( m_ModeState != MS_PLAY ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_iCurRoundCount != 1 )
		return true;
	else if( dwCurTime > m_dwPlayStartTime && dwCurTime - m_dwPlayStartTime >= 10000 )
		return true;

	return false;
}

bool ioPlayMode::IsFirstSelect()
{
	if( m_bFirstSelect )
	{
		m_bFirstSelect = false;
		return true;
	}

	return false;
}

void ioPlayMode::CheckTotalChamp( bool bSound )
{
	if( m_RoundInfoList.size() == 0 )
		return;

	const RoundInfo &kRoundInfo = m_RoundInfoList[0];

	if( !m_CurChampName.IsEmpty() )
	{
		if( kRoundInfo.m_szName == m_CurChampName )
			return;

		ioBaseChar *pPreChamp = GetRecordChar( m_CurChampName );
		if( pPreChamp )
		{
			pPreChamp->SetChampEmoticon( true, bSound );
		}
	}

	ioBaseChar *pNewChamp = GetRecordChar( kRoundInfo.m_szName );
	if( pNewChamp )
	{
		pNewChamp->SetChampEmoticon( false, bSound );
		m_CurChampName = kRoundInfo.m_szName;
	}
}

void ioPlayMode::ResetTotalChamp()
{
	if( !m_pCreator )
		return;

	if( m_CurChampName.IsEmpty() )
		return;

	ioBaseChar *pChamp = m_pCreator->GetBaseChar( m_CurChampName );
	if( pChamp )
	{
		pChamp->ResetChampEmoticon();
	}
}

bool ioPlayMode::ShowSoldierSettingUI( ioBaseChar *pOwnerChar, int iArray )
{
	if( !pOwnerChar ) return false;
	if( GetModeState() != MS_PLAY ) return false;
	if( pOwnerChar->GetState() != CS_DIE ) return false;

	SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( !pSoldier ) return false;

	if( pSoldier->IsShow() )
	{
		// 다음 라운드 출전 용병 예약 및 용병 선택
		int iSlotToArray = g_MyInfo.GetCharSlotIndexToArray( iArray );
		pSoldier->SetFunctionKeySelect( iSlotToArray, IsNetworkMode() );
	}
	else
	{
		// 용병 설정 UI 활성화
		pSoldier->SetSettingSoldierShow( iArray, IsNetworkMode() );
	}
	return true;
}

void ioPlayMode::UpdateRevivalTime( const ioHashString &szName, DWORD dwCurRevivalTime )
{
	ModeRecord *pDieRecord = FindModeRecordCharName( szName );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::UpdateRevivalTime() - %s is not exist to the recorde", szName.c_str() );
		return;
	}

	m_dwCurRevivalTime = dwCurRevivalTime;

	pDieRecord->dwCurDieTime = FRAMEGETTIME();
	pDieRecord->dwRevivalGap = m_dwCurRevivalTime;
	pDieRecord->iRevivalCnt++;

	if( pDieRecord->pChar )
	{
		if( pDieRecord->pChar->IsOwnerChar() )
		{
			g_ModeHelpMgr.ShowHelp( ioModeHelpManager::HT_REVIVING,  FRAMEGETTIME() + m_dwCurRevivalTime );
		}
	}
}

float ioPlayMode::GetMaxHPRateByMode()
{
	return m_fMaxHPRate;
}

float ioPlayMode::GetRevivalHPRateByMode()
{
	return m_fRevivalHPRate;
}

void ioPlayMode::UpdateRoundInfoList()
{
	m_RoundInfoList.clear();
	m_RoundInfoList.reserve( MAX_PLAYER );

	for( int i = 0 ; i<GetRecordCharCnt(); i++ )
	{
		RoundInfo kInfo;
		if( GetRoundInfo( i, kInfo ) )
			m_RoundInfoList.push_back( kInfo );
	}

	if( m_RoundInfoList.size() == 0 )
		return;

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoSort() );
}

void ioPlayMode::CheckModeMapSound()
{
	ErrorReport::SetPosition( 1080, 300 );

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwModeSoundTime > 0 && m_dwModeSoundTime < dwCurTime )
	{
		ErrorReport::SetPosition( 1080, 301 );
		if( !m_ModeMapSound.IsEmpty() )
		{
			ErrorReport::SetPosition( 1080, 302 );
			char szName[MAX_PATH];
			wsprintf( szName, "Resource/wave/%s", m_ModeMapSound.c_str() );

			ErrorReport::SetPosition( 1080, 303 );
			try
			{
				g_SoundMgr.OpenOggStream( szName );
				ErrorReport::SetPosition( 1080, 304 );
				g_SoundMgr.PlayStream( true );
				ErrorReport::SetPosition( 1080, 305 );
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
		}

		m_dwModeSoundTime = 0;
	}

	ErrorReport::SetPosition( 1080, 306 );
}

void ioPlayMode::SetModeMapSound( bool bImmediately )
{
	m_dwModeSoundTime = FRAMEGETTIME();

	if( !bImmediately )
		m_dwModeSoundTime += m_dwModeSoundDelayTime;
}

int ioPlayMode::GetCurRank( const ioHashString& szCharName )
{
	return -1;
}

void ioPlayMode::ProcessDeathTimeHelp()
{
	if( m_ModeState == MS_PLAY && IsZeroHP() )
	{
		g_ModeHelpMgr.ShowHelp( ioModeHelpManager::HT_DEATH_TIME );
	}
	else
	{
		g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_DEATH_TIME );
	}
}

float ioPlayMode::GetFallDamage( ioBaseChar *pChar )
{
	float fFallDamage = m_fFallDamage;
	float fExtraRate = FLOAT1;

	TeamType eTeam = pChar->GetTeam();
	if( eTeam == TEAM_RED )
		fExtraRate = m_pCreator->GetMemberBalance( TEAM_BLUE );
	else if( eTeam == TEAM_BLUE )
		fExtraRate = m_pCreator->GetMemberBalance( TEAM_RED );

	if(ToNpcChar(pChar))
	{
		fFallDamage = m_fNpcFallDamage;
		fFallDamage *= fExtraRate;

		if( fFallDamage > 0.0f )
		{
			fFallDamage = min( fFallDamage, m_fNpcFallMaxDamage );
			fFallDamage = max( m_fNpcFallMinDamage, fFallDamage );
		}
	}
	else
	{
		fFallDamage *= fExtraRate;

		if( fFallDamage > 0.0f )
		{
			fFallDamage = min( fFallDamage, m_fFallMaxDamage );
			fFallDamage = max( m_fFallMinDamage, fFallDamage );
		}
	}

	return fFallDamage;
}

void ioPlayMode::ProcessByOwner()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) return;

	SkillListWnd *pSkillWnd = dynamic_cast<SkillListWnd*>(g_GUIMgr.FindWnd( SKILL_LIST_WND ));
	if( pSkillWnd )
	{
		if( pOwner->GetState() != CS_OBSERVER )
			pSkillWnd->UpdateByOwner( pOwner, false );
		else if( m_pCreator )
			pSkillWnd->UpdateByOwner( m_pCreator->GetCameraTargetChar(), true );
	}

	AccessorySkillListWnd *pAccessorySkillListWnd = dynamic_cast<AccessorySkillListWnd *>( g_GUIMgr.FindWnd( ACCESSORY_SKILL_LIST_WND ));
	if( pAccessorySkillListWnd )
	{
		pAccessorySkillListWnd->UpdateByOwner( pOwner );
	}

	MiniSoldierSelectWnd *pMiniSoldierSelectWnd = dynamic_cast<MiniSoldierSelectWnd*>( g_GUIMgr.FindWnd( MINI_SOLDIER_SELECT_WND ));
	if( pMiniSoldierSelectWnd )
		pMiniSoldierSelectWnd->UpdateByOwner( pOwner );
}

int ioPlayMode::GetRevivalCnt( ioBaseChar *pChar )
{
	if( pChar )
	{
		ModeRecord *pRecord = FindModeRecordCharName( pChar->GetCharName() );

		if( pRecord )
			return pRecord->iRevivalCnt;
	}

	return 0;
}

ioBaseChar* ioPlayMode::GetRecordCharTeamArray( TeamType eTeam, int iArray )
{
	int iRecordCnt = GetRecordCharCnt();
	int iCurCnt = 0;
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( iCurCnt > iArray ) break;

		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;

		if( eTeam == TEAM_NONE && pRecord->pChar->GetTeam() != TEAM_NONE )
		{
			if( iCurCnt == iArray )
				return pRecord->pChar;

			iCurCnt++;
		}
		else if( pRecord->pChar->GetTeam() == eTeam )
		{
			if( iCurCnt == iArray )
				return pRecord->pChar;

			iCurCnt++;
		}
	}

	return NULL;
}

bool ioPlayMode::IsNeedUpdateMapEffect( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	bool bNeedUpdate = false;

	if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS )
	{
		if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
		{
			if( pChar->GetCharName() == m_szEndLookCharName )
			{
				bNeedUpdate = true;
			}
		}
		else if( pChar->IsOwnerChar() )
		{

			bNeedUpdate = true;
		}
	}

	return bNeedUpdate;
}

//////////////////////////////////////////////////////////////////////////
//Test
void ioPlayMode::SetMapDummy()
{
	if( !m_pCreator )
		return;
	
	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
	if( !pOwner ) 
		return;
	
	if( m_MapDummyObjectList.empty() )
		return;

	g_DummyCharMgr.ClearMapDummy();

	MapDummyObjectList::iterator iter = m_MapDummyObjectList.begin();
	while( iter != m_MapDummyObjectList.end() )
	{
		const ioHashString& stName = (*iter).m_stName;
		D3DXVECTOR3 vPos = (*iter).m_vPos;
		float fAngle = (*iter).m_fAngle;

		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );

		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		m_pCreator->CreateDummyChar( stName, iCurIndex, pOwner->GetCharName(), vPos, fAngle, 0, false );

		++iter;
	}
}


void ioPlayMode::RemoveMapDummy()
{

}
//////////////////////////////////////////////////////////////////////////

void ioPlayMode::SetMapAreaWeapon()
{
	if( m_MapAreaWeaponList.empty() ) return;

	g_AreaWeaponMgr.ClearMapAreaWeaponList();

	m_dwAreaWeaponMapIndex = 0;

	MapAreaWeaponList::iterator iter = m_MapAreaWeaponList.begin();
	
	while( iter != m_MapAreaWeaponList.end() )
	{
		(*iter).m_dwCheckTime = FRAMEGETTIME();

		if( (*iter).m_bOneTimeCreate )
			(*iter).m_bCreated = false;

		if( (*iter).m_dwCreateTime > 0 )
		{
			++iter;
			continue;
		}

		float fAngle = (*iter).m_fCreateAngle;
		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( "", "", "", (*iter).m_AreaWeaponName, (*iter).m_vPos, qtRot, ioAreaWeapon::CT_MAP );
		if( pAreaWeapon )
		{
			m_dwAreaWeaponMapIndex++;
			pAreaWeapon->SetAreaWeaponMapIndex(m_dwAreaWeaponMapIndex);
		}

		++iter;
	}
}

void ioPlayMode::RemoveMapAreaWeapon()
{
	g_AreaWeaponMgr.ClearMapAreaWeaponList();
}

void ioPlayMode::ProcessMapAreaWeapon()
{
	if( m_MapAreaWeaponList.empty() ) return;
	if( m_ModeState != MS_PLAY ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	MapAreaWeaponList::iterator iter = m_MapAreaWeaponList.begin();

	while( iter != m_MapAreaWeaponList.end() )
	{
		if( (*iter).m_dwCreateTime == 0 )
		{
			++iter;
			continue;
		}

		float fAngle = (*iter).m_fCreateAngle;
		if( (*iter).m_bOneTimeCreate )
		{
			if( !(*iter).m_bCreated && CheckOneTimeCreateMapAreaWeapon( (*iter).m_dwCheckTime, (*iter).m_dwCreateTime, (*iter).m_bFirst ) )
			{
				(*iter).m_dwCheckTime = dwCurTime;
				(*iter).m_bFirst = false;
				(*iter).m_bCreated = true;

				D3DXQUATERNION qtRot;
				D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );

				ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( "", "", "", (*iter).m_AreaWeaponName, (*iter).m_vPos, qtRot, ioAreaWeapon::CT_MAP );
				if( pAreaWeapon )
				{
					m_dwAreaWeaponMapIndex++;
					pAreaWeapon->SetAreaWeaponMapIndex(m_dwAreaWeaponMapIndex);
				}
			}
		}
		else
		{
			if( CheckCreateMapAreaWeapon( (*iter).m_dwCheckTime, (*iter).m_dwCreateTime, (*iter).m_bFirst ) )
			{
				(*iter).m_dwCheckTime = dwCurTime;
				(*iter).m_bFirst = false;

				D3DXQUATERNION qtRot;
				D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );

				g_AreaWeaponMgr.CreateAreaWeapon( "", "", "", (*iter).m_AreaWeaponName, (*iter).m_vPos, qtRot, ioAreaWeapon::CT_MAP );
			}
		}

		++iter;
	}
}

bool ioPlayMode::CheckOneTimeCreateMapAreaWeapon( DWORD dwCheckTime, DWORD dwCreateTime, bool bFirst )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwPlayStartTime;

	if( dwCreateTime < dwGapTime )
		return true;

	return false;
}

bool ioPlayMode::CheckCreateMapAreaWeapon( DWORD dwCheckTime, DWORD dwCreateTime, bool bFirst )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwPlayStartTime;

	DWORD dwPeriodTime = 0;
	while( dwPeriodTime < m_dwCurRoundDuration )
	{
		dwPeriodTime += dwCreateTime;

		DWORD dwTestTime = m_dwPlayStartTime + dwPeriodTime;

		if( !bFirst && dwTestTime < dwCheckTime+(dwCreateTime/2) )
			continue;

		if( COMPARE( dwTestTime, dwCheckTime, dwCurTime ) )
			return true;
	}

	return false;
}

void ioPlayMode::ClearGrowthLevel()
{
	ioGrowthLevel *pLevel = NULL;

	GrowthLevelList::iterator iter = m_vGrowthLevelList.begin();
	while( iter != m_vGrowthLevelList.end() )
	{
		pLevel = (*iter);
		if( pLevel )
		{
			delete pLevel;
		}

		++iter;
	}

	m_vGrowthLevelList.clear();
}

void ioPlayMode::AddGrowthLevel( ioBaseChar *pChar )
{
	if( !pChar ) return;

	AddGrowthLevel( pChar->GetCharName() );
}

void ioPlayMode::AddGrowthLevel( const ioHashString &rkName )
{
	ioGrowthLevel *pLevel = GetGrowthLevel( rkName );
	if( pLevel )
	{
		pLevel->Initialize( rkName );
	}
	else
	{
		pLevel = new ioGrowthLevel;

		if( pLevel )
		{
			pLevel->Initialize( rkName );

			m_vGrowthLevelList.push_back( pLevel );
		}
	}
}

void ioPlayMode::RemoveGrowthLevel( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioGrowthLevel *pLevel = NULL;

	GrowthLevelList::iterator iter = m_vGrowthLevelList.begin();
	while( iter != m_vGrowthLevelList.end() )
	{
		pLevel = (*iter);
		if( pLevel )
		{
			if( pLevel->GetOwnerName() == pChar->GetCharName() )
			{
				delete pLevel;
				m_vGrowthLevelList.erase( iter );
				break;
			}
		}

		++iter;
	}
}

ioGrowthLevel* ioPlayMode::GetGrowthLevel( const ioHashString &rkName )
{
	ioGrowthLevel *pLevel = NULL;

	GrowthLevelList::iterator iter = m_vGrowthLevelList.begin();
	while( iter != m_vGrowthLevelList.end() )
	{
		pLevel = (*iter);

		if( pLevel )
		{
			if( pLevel->GetOwnerName() == rkName )
			{
				return pLevel;
			}
		}

		++iter;
	}

	return NULL;
}

void ioPlayMode::OnBallStructInfo( SP2Packet &rkPacket )
{
	//g_BallStructMgr.DestroyBallStructList();
	BallStructSupply( rkPacket );
}

void ioPlayMode::OnBallStructReposition( SP2Packet &rkPacket )
{
	g_BallStructMgr.OnBallStructReposition( rkPacket );
}

void ioPlayMode::ApplyBallStructInfo( SP2Packet &rkPacket )
{
	int iBallArray;

	rkPacket >> iBallArray;

	ioBall *pBall = g_BallStructMgr.FindBallStruct( iBallArray );
	if( !pBall )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::ApplyBallStructInfo - %d Not Exist BallStruct", iBallArray );
		return;
	}

	pBall->OnStructInfo( rkPacket );
}

void ioPlayMode::SetMapPassiveBuff( ioBaseChar *pChar )
{
	if( !pChar ) return;

	int iBuffCnt = m_vMapPassiveBuffList.size();
	if( iBuffCnt <= 0 ) return;

	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->AddNewBuff( m_vMapPassiveBuffList[i].m_szBuffName, "", "", NULL );
	}
}

void ioPlayMode::SetStartBuffList( ioBaseChar *pChar )
{
	if( !pChar ) return;

	int iBuffCnt = m_vStartBuffList.size();
	if( iBuffCnt <= 0 ) return;

	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->AddNewBuff( m_vStartBuffList[i], "", "", NULL );
	}
}

void ioPlayMode::RemoveStartBuffList()
{
}

void ioPlayMode::SetZeroHP( bool bZero )
{
	m_bZeroHP = bZero;
}

void ioPlayMode::SetRecoveryGauge( ioBaseChar *pChar, float fRate )
{
	if( !pChar ) return;

	for( int iSlot=0; iSlot < 4; ++iSlot )
	{
		ioItem *pItem = pChar->GetEquipedItem( iSlot );
		ioSkill *pSkill = pChar->GetEquipedSkill( iSlot );

		if( !pItem || !pSkill ) continue;
		if( pSkill->CheckUseBuff( pChar ) )
			continue;

		if( g_WeaponMgr.CheckRotationWeaponByItem(pChar->GetCharName(), pItem->GetItemCreateIndex()) )
			continue;

		if( (ToControlDummySkill(pSkill) ) && pChar->IsCurUsingSkill( ToControlDummySkill(pSkill)->GetName() ) )
			continue;
		
		if( (ToControlDummySkill2(pSkill) ) && pChar->IsCurUsingSkill( ToControlDummySkill2(pSkill)->GetName() ) )
			continue;

		if( (ToControlDummySkill3(pSkill) ) && pChar->IsCurUsingSkill( ToControlDummySkill3(pSkill)->GetName() ) )
			continue;

		if( ToTargetMissileSkill(pSkill) && pChar->IsCurUsingSkill( ToTargetMissileSkill(pSkill)->GetName() ) )
			continue;

		if( ToComboAttackSkill(pSkill) && pChar->IsCurUsingSkill( ToComboAttackSkill(pSkill)->GetName() ) )
			continue;

		if( ToMultiAreaSkill(pSkill) &&
			pChar->IsCurUsingSkill( ToMultiAreaSkill(pSkill)->GetName() ) &&
			ToMultiAreaSkill(pSkill)->CheckKOEffect() )
			continue;

		float fMaxGauge = pItem->GetMaxSkillGauge(pChar->GetCharName());
		float fCurGauge = pItem->GetCurSkillGuage();

		float fRecoveryGauge = fMaxGauge * fRate;

		if( fMaxGauge > fCurGauge )
		{
			fCurGauge += fRecoveryGauge;
			if( fCurGauge > fMaxGauge )
				fCurGauge = fMaxGauge;

			pItem->SetCurSkillGauge( fCurGauge );
		}
	}
}

void ioPlayMode::OnMachineStructInfo( SP2Packet &rkPacket )
{
	g_MachineStructMgr.DestroyMachineStructList();
	MachineStructSupply( rkPacket );
}

void ioPlayMode::MachineStructSupply( SP2Packet &rkPacket )
{
}

void ioPlayMode::OnPushStructDieByOwnerClear( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	g_PushStructListMgr.ApplyStructListByOwner( szName );
}

void ioPlayMode::OnPushStructInfoEnter( SP2Packet &rkPacket )
{
	bool bClearInfo;
	rkPacket >> bClearInfo;

	if( bClearInfo )
	{
		g_PushStructListMgr.DestroyStructList();
	}

	PushStructSupply( rkPacket );
}

void ioPlayMode::LoadDisableSkillList()
{
	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	
	m_DisableSkillList.clear();

	kLoader.SetTitle_e( "disable_skill" );
	int iSkillCnt = kLoader.LoadInt_e( "disable_skill_cnt", 0 );

	_ENCSTR("disable_skill", szTmp);
	for( int i=0; i < iSkillCnt; ++i )
	{
		wsprintf_e( szKey, "%s%d", szTmp, i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_DisableSkillList.push_back( szBuf );
	}
}

void ioPlayMode::LoadStartBuffList()
{
	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	// Start Buff List
	m_vStartBuffList.clear();

	kLoader.SetTitle_e( "start_buff" );
	int iBuffCnt = kLoader.LoadInt_e( "start_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_vStartBuffList.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "start_buff%d", i+1 );

			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			ioHashString szTmp = szBuf;
			m_vStartBuffList.push_back( szTmp );
		}
	}
}

bool ioPlayMode::CheckDisableSkill( const ioHashString &szSkillName )
{
	ioHashStringVec::iterator iter = m_DisableSkillList.begin();
	while( iter != m_DisableSkillList.end() )
	{
		if( (*iter) == szSkillName )
		{
			return true;
		}

		++iter;
	}
	return false;
}

bool ioPlayMode::IsEntityUpdateState()
{
	return true;
}

void ioPlayMode::ClearModeItem()
{
	for( vModeItem_iter iter = m_vModeItemList.begin(); iter != m_vModeItemList.end(); ++iter )
	{
		ioModeItem *pItem = *iter;
		iter = m_vModeItemList.erase( iter );
		SAFEDELETE( pItem );
	}
	m_vModeItemList.clear();
}

ioModeItem* ioPlayMode::FindModeItem( DWORD dwModeItemIndex )
{
	vModeItem_iter iter = m_vModeItemList.begin();
	for( ; iter != m_vModeItemList.end(); ++iter )
	{
		ioModeItem *pItem = *iter;
		if( pItem->GetUniqueID() == dwModeItemIndex )
			return pItem;
	}

	return NULL;
}

void ioPlayMode::OnCreateModeItem( SP2Packet &rkPacket )
{
}

void ioPlayMode::OnGetModeItem( SP2Packet &rkPacket )
{
	DWORD dwUserIdx, dwModeItemIndex;
	rkPacket >> dwUserIdx >> dwModeItemIndex;

	ioModeItem* pItem = FindModeItem( dwModeItemIndex );
	if( pItem )
		pItem->SetReserveDestroy();
}

void ioPlayMode::CreateModeItem( D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, float fSpeed, float fPower )
{
}

void ioPlayMode::ProcessModeItem()
{
	if( m_ModeState != MS_PLAY )
		return;

	if( m_vModeItemList.empty() )
		return;

	vModeItem_iter iter = m_vModeItemList.begin();
	while( iter != m_vModeItemList.end() )
	{
		ioModeItem *pItem = *iter;

		if( !pItem )
			++iter;

		if( pItem->IsLive() )
		{
			pItem->Process();
			++iter;
		}
		else
		{
			pItem->OnDestroy();
			iter = m_vModeItemList.erase( iter );
			SAFEDELETE( pItem );
		}
	}
}


void ioPlayMode::ProcessModeItemVSMapCollision( ioOpcodeShape *pShape, ioPlayStage* pStage, const D3DXMATRIX* pShapeWorldMat )
{
	vModeItem_iter iter = m_vModeItemList.begin();
	for( ; iter != m_vModeItemList.end(); ++iter )
	{
		ioModeItem* pItem = *iter;
		if( !pItem )
			continue;

		pItem->TestMapCollision( pShape, pStage, pShapeWorldMat );
	}
}

#ifndef SHIPPING
void ioPlayMode::RenderModeEntity()
{
	D3DXVECTOR3 vVtxArray[8];

	vModeItem_iter iter = m_vModeItemList.begin();
	for( ; iter != m_vModeItemList.end(); ++iter )
	{
		ioModeItem* pItem = *iter;
		if( !pItem )
			continue;

		if( !pItem->IsAttach() )
			continue;

		const ioOrientBox& rkBox = pItem->GetAttackBox();
		rkBox.GetVertices( vVtxArray );

		if( m_pCreator )
			m_pCreator->RenderBox( vVtxArray,  0x00FF0000 );
	}
}
#endif

void ioPlayMode::ProcessModeItemVSCharCollision( const BaseCharList &rkTargetList )
{
	if( rkTargetList.empty() )
		return;

	BaseCharList::const_iterator char_iter;
	for( char_iter = rkTargetList.begin(); char_iter != rkTargetList.end(); ++char_iter )
	{
		ioBaseChar *pChar = (*char_iter);
		if( !pChar )
			continue;

		if( !pChar->IsNeedProcess() )
			continue;

		CharState eState = pChar->GetState();
		if( eState == CS_WOUNDED                 ||
			eState == CS_BLOW_EXTEND_WOUND       ||
			eState == CS_BLOW_WOUNDED            ||
			eState == CS_GRAPPLING_WOUNDED       ||
			eState == CS_GRAPPLING_WOUNDED_ITEM  ||
			eState == CS_GRAPPLING_WOUNDED_BUFF  ||
			eState == CS_GRAPPLING_WOUNDED_SKILL ||
			eState == CS_GRAPPLING_WOUNDED_SWING ||
			eState == CS_JUMP_GRAPPLING_WOUNDED  ||
			eState == CS_GRAPPLING_PUSHED        ||
			eState == CS_GRAPPLING_PUSHED_BUFF )
			continue;

		vModeItem_iter item_iter = m_vModeItemList.begin();
		for( ; item_iter != m_vModeItemList.end(); ++item_iter )
		{
			ioModeItem* pItem = *item_iter;
			if( !pItem )
				continue;

			pItem->IsCollision( pChar );
		}
	}
}

void ioPlayMode::OnShuffleBonusRewardResult( SP2Packet &rkPacket )
{
	int iPlayPoint			= 0;
	int iWinningPoint		= 0;
	int iConsecutivePoint	= 0;
	int iAwardPoint			= 0;

	rkPacket >> iPlayPoint >> iWinningPoint >> iConsecutivePoint >> iAwardPoint;

	TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd *>( g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ) );
	if( pTotalResult )	
		pTotalResult->SetShufflePoint( iPlayPoint, iWinningPoint, iConsecutivePoint, iAwardPoint ); 	
}

void ioPlayMode::LoadModeGrowthRate()
{
	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );
	char szKey[MAX_PATH] = "";

	kLoader.SetTitle_e( "growth_control" );

	for( int i=0; i < MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; ++i )
	{
		wsprintf_e( szKey, "growth_value%d", i+1 );
		m_vModeGrowthRate[i] = kLoader.LoadFloat( szKey, 1.0f );
	}
}

float ioPlayMode::GetModeGrowthRate( int iGrowthArray )
{
	if ( !COMPARE( iGrowthArray, 0, MAX_CHAR_GROWTH+MAX_ITEM_GROWTH ) )
		return 1.0f;
	return m_vModeGrowthRate[iGrowthArray];
}

bool ioPlayMode::IsCharacterChangeImmediately()
{
	if( m_ModeState == MS_PLAY )
		return m_bCharacterChangeImmediately;

	return false;
}
