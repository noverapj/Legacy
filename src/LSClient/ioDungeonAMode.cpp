

#include "stdafx.h"

#include "ioDungeonAMode.h"
#include "ioPlayMode.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"
#include "ioNpcMgr.h"
#include "ioMapEventSceneMgr.h"

#include "ioComplexStringPrinter.h"
#include "ioLadderTeamMgr.h"
#include "ioModeGuideArrow.h"

#include "GUI/MiniMap.h"
#include "GUI/SoldierSelectWnd.h"
#include "GUI/ioSP2GUIManager.h"

#include "GUI/TimeScoreWnd.h"
#include "GUI/BossMonsterGaugeWnd.h"
#include "GUI/ioObserverWndManager.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/BriefingWnd.h"
#include "GUI/MonsterCoinWnd.h"
#include "GUI/CenterHelpWnd.h"
#include "GUI/GlobalWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"

ioDungeonAMode::ioDungeonAMode( ioPlayStage *pCreator ) : ioMonsterSurvivalMode( pCreator )
{
	m_ModeType = MT_DUNGEON_A;
}

ioDungeonAMode::~ioDungeonAMode()
{

}

void ioDungeonAMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadObjectItems();
	LoadFuntionalObjects();
	LoadCharacters();
	LoadTurnHelp();
	LoadHighTurn();
	LoadDisableSkillList();
	LoadStartBuffList();
	LoadModeGrowthRate();

	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	char szBuf[MAX_PATH];
	kLoader.SetTitle_e( "common_info" );
	m_dwRoundDuration = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );
	m_fRevivalRecoveryHP = kLoader.LoadFloat_e( "revival_recovery_hp_rate", FLOAT05 );

	m_dwStartCoinTime = kLoader.LoadInt_e( "start_coin_use_time", 0 );
	kLoader.LoadString_e( "coin_revival_buff", "", szBuf, MAX_PATH );
	m_szCoinRevivalBuff = szBuf;

	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );
	LoadModeCamera( kLoader );

	kLoader.SetTitle_e( "ko_recover" );
	m_fKORecoverRate = kLoader.LoadFloat_e( "ko_recover_rate", 0.0f );
	kLoader.LoadString_e( "ko_recover_effect", "", szBuf, MAX_PATH );
	m_KORecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_perfect_effect", "", szBuf, MAX_PATH );
	m_PerfectRecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_buff", "", szBuf, MAX_PATH );
	m_KOBuff = szBuf;

	kLoader.SetTitle_e( "end_scene_camera" );
	m_fEndHeight = kLoader.LoadFloat_e( "end_camera_height", 0.0f );
	m_fEndDistance = kLoader.LoadFloat_e( "end_camera_distance", 0.0f );
	m_fEndFov = kLoader.LoadFloat_e( "end_camera_fov", 0.0f );
	kLoader.LoadString_e( "round_end_focus_effect", "", szBuf, MAX_PATH );
	m_RoundEndEffect = szBuf;

	LoadGUIValues();
	LoadPushCursor();
	LoadContribute();

	kLoader.SetTitle_e( "ui_info" );
	kLoader.LoadString_e( "1point_up_emoticon", "", szBuf, MAX_PATH );
	m_PointUpEmoticon = szBuf;

	kLoader.LoadString_e( "kill_point_emoticon_red", "", szBuf, MAX_PATH );
	m_KillPointEmoticonRed = szBuf;
	kLoader.LoadString_e( "kill_point_emoticon_blue", "", szBuf, MAX_PATH );
	m_KillPointEmoticonBlue = szBuf;
	kLoader.LoadString_e( "kill_point_num", "", szBuf, MAX_PATH );
	m_KillPointNum = szBuf;
	kLoader.LoadString_e( "kill_point_per", "", szBuf, MAX_PATH );
	m_KillPointPer = szBuf;
	kLoader.LoadString_e( "kill_point_sound", "", szBuf, MAX_PATH );
	m_KillPointSound = szBuf;

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		SetModeState( MS_READY );
		LoadObjectItems();
	}

	g_MyInfo.ClearCharDiedInfo();
	g_MyInfo.ClearCharDiedPenaltyInfo();
	g_MyInfo.ClearCharJoinedInfo();

	g_GUIMgr.CheckShopSound( true );

	g_MyInfo.InitPraise();

	ClearGrowthLevel();

	// LoadMap() 이후 사용해야함
	SetMapAreaWeapon();

	SetMapDummy();

	if( g_BattleRoomMgr.IsBattleRoom() )
		g_BattleRoomMgr.ModeStartPlayerCount();
}

void ioDungeonAMode::TurnStartEvent( DWORD dwPrevHighTurn, DWORD dwPrevLowTurn )
{
	if( m_dwCurrentHighTurn == dwPrevHighTurn && m_dwCurrentLowTurn == dwPrevLowTurn )
		return;

	// 로우턴 변경
	int iObjectIndex = -1;
	ioHashString kEffectName;
	ioHashString kEffectLoopName;
	D3DXVECTOR3  kEffectLoopPos;
	if( m_dwCurrentHighTurn == dwPrevHighTurn )
	{
		if( COMPARE( m_dwCurrentHighTurn, 0, (int)m_HighTurnList.size() ) )
		{
			HighTurnData &rkHighTurn = m_HighTurnList[m_dwCurrentHighTurn];
			if( COMPARE( m_dwCurrentLowTurn - 1, 0, (int)rkHighTurn.m_TurnData.size() ) )
			{
				TurnData &rkTurn = rkHighTurn.m_TurnData[m_dwCurrentLowTurn-1];
				iObjectIndex = rkTurn.m_iTurnEndHideObject;
				kEffectName  = rkTurn.m_szTurnEndEffect;

				kEffectLoopName  = rkTurn.m_szTurnEndLoopEffect;
				kEffectLoopPos.x = rkTurn.m_fTurnEndLoopEffectX;
				kEffectLoopPos.y = rkTurn.m_fTurnEndLoopEffectY;
				kEffectLoopPos.z = rkTurn.m_fTurnEndLoopEffectZ;
			}
		}
	}
	else if( COMPARE( m_dwCurrentHighTurn - 1, 0, (int)m_HighTurnList.size() ) )
	{
		HighTurnData &rkHighTurn = m_HighTurnList[m_dwCurrentHighTurn - 1];
		int iEndLowTurn = (int)rkHighTurn.m_TurnData.size() - 1;
		if( COMPARE( iEndLowTurn, 0, (int)rkHighTurn.m_TurnData.size() ) )
		{
			TurnData &rkTurn = rkHighTurn.m_TurnData[iEndLowTurn];
			iObjectIndex = rkTurn.m_iTurnEndHideObject;
			kEffectName  = rkTurn.m_szTurnEndEffect;

			kEffectLoopName  = rkTurn.m_szTurnEndLoopEffect;
			kEffectLoopPos.x = rkTurn.m_fTurnEndLoopEffectX;
			kEffectLoopPos.y = rkTurn.m_fTurnEndLoopEffectY;
			kEffectLoopPos.z = rkTurn.m_fTurnEndLoopEffectZ;
		}
	}

	//
	ioWallZone *pWallZoneEntity = ToWallZone( m_pCreator->GetWorldEntity( ioPlayEntity::PST_WALL_ZONE, iObjectIndex ) );
	if( pWallZoneEntity )
	{
		if( !kEffectName.IsEmpty() )
		{
			if( pWallZoneEntity->GetCreateIndex() == iObjectIndex )
			{
				m_pCreator->CreateMapEffect( kEffectName, pWallZoneEntity->GetWorldPosition(), pWallZoneEntity->GetWorldScale() );			
			}
		}
		
		m_pCreator->RemoveWorldEntity( pWallZoneEntity );
	}

	if( !kEffectLoopName.IsEmpty() )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		m_pCreator->CreateMapEffect( kEffectLoopName, kEffectLoopPos, vScale );
	}
}

void ioDungeonAMode::OnTelepotation( ioBaseChar *pOwnerChar )
{
	if( !pOwnerChar ) return;

	// 맵 교체
	m_pCreator->ChangeMultiMapIndex( m_dwCurrentHighTurn );
}

void ioDungeonAMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
									   IntVec &rvClassPointList,
									   float fBlueGuildTeamBonus,
									   float fRedGuildTeamBonus,
									   float fBonusArray[BA_MAX] )
{
	TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
	if( !pTotalResult ) return;

	if( g_GUIMgr.IsShow( ROUND_RESULT_MAIN_WND ) )
		g_GUIMgr.HideWnd( ROUND_RESULT_MAIN_WND );

	ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

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
		}
	}
    
	pTotalResult->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	pTotalResult->SetFinalRoundPoint( rvClassTypeList, rvClassPointList, fBonusArray );
	pTotalResult->SetRaidScore( IsBlueWin( m_WinTeamType ) );
	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;

	SetTotalResultBriefingWnd();
}

void ioDungeonAMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	ioHashString szDieName, szAttName;
	TeamType eDieTeam = TEAM_NONE;
	TeamType eAttTeam = TEAM_NONE;
	bool bKOChat = true;
	//
	ioNpcChar *pKillNpc = ToNpcChar( pKillChar );
	if( pKillNpc )
	{
		szAttName = pKillNpc->GetViewName();
		eAttTeam  = pKillNpc->GetTeam();		
	}
	else if( pKillChar )
	{
		szAttName = pKillChar->GetCharName();
		eAttTeam  = pKillChar->GetTeam();
	}

	//
	ioNpcChar *pDieNpc = ToNpcChar( pDieChar );
	if( pDieNpc )
	{
		szDieName = pDieNpc->GetViewName();
		eDieTeam  = pDieNpc->GetTeam();
		AddEraseNpc( pDieNpc->GetCharName() );

		if( !pDieNpc->IsMonsterHalfBoss() && !pDieNpc->IsMonsterLastBoss() )
			bKOChat = false;
	}
	else if( pDieChar )
	{
		szDieName = pDieChar->GetCharName();
		eDieTeam  = pDieChar->GetTeam();
	}	

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
			}
			else
			{
				szAttName.Clear();
				eAttTeam = TEAM_NONE;
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -fKillPoint, true, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
				}
			}

			pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			if( bKOChat )
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
			pDieRecord->pChar->SetDeathEmoticon();

			if( bKOChat )
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
		}
	}

	if( pDieChar && pDieChar->IsOwnerChar() )
	{
		m_pCreator->StartScreenBlind( SBT_FIXED_BLIND, NULL, 0 );
	}

	// Best Attacker 처리 안함
}

void ioDungeonAMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );

	ioHashString szDieName, szAttName;
	TeamType eDieTeam = TEAM_NONE;
	TeamType eAttTeam = TEAM_NONE;
	bool bKOChat = true;
	//
	ioNpcChar *pKillNpc = ToNpcChar( pKillerChar );
	if( pKillNpc )
	{
		szAttName = pKillNpc->GetViewName();
		eAttTeam  = pKillNpc->GetTeam();
	}
	else if( pKillerChar )
	{
		szAttName = pKillerChar->GetCharName();
		eAttTeam  = pKillerChar->GetTeam();
	}

	//
	ioNpcChar *pDieNpc = ToNpcChar( pDieChar );
	if( pDieNpc )
	{
		szDieName = pDieNpc->GetViewName();
		eDieTeam  = pDieNpc->GetTeam();
		AddEraseNpc( pDieNpc->GetCharName() );

		if( !pDieNpc->IsMonsterHalfBoss() && !pDieNpc->IsMonsterLastBoss() )
			bKOChat = false;
	}
	else if( pDieChar )
	{
		szDieName = pDieChar->GetCharName();
		eDieTeam  = pDieChar->GetTeam();
	}	

	if( pKillerChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillerChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), GetKillPoint( pAttRecord->pChar->GetTeam() ), false, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
			}
			else
			{
				szAttName.Clear();
				eAttTeam = TEAM_NONE;
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -GetKillPoint( pAttRecord->pChar->GetTeam() ), false, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
				}
			}

			pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			if( bKOChat )
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
			pDieRecord->pChar->SetDeathEmoticon();

			if( bKOChat )
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
		}
	}

	// Best Attacker 처리 안함
}

void ioDungeonAMode::ContributeConvert()
{
	if( m_vContributeTable.size() < MAX_PLAYER )
		return;

	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );

	int i = 0;
	int iCharCnt = kCharReocrdNameList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iKill;
		vInfo.push_back( Contribute );
	}
	iInfoCnt = vInfo.size();

	std::sort( vInfo.begin(), vInfo.end(), ContributeSort() );
	ContributeLowerSort( vInfo );

	for(i = 0;i < iInfoCnt;i++)
	{
		int iArray = vInfo[i].m_iRecordArray;
		if( COMPARE( iArray, 0, iCharCnt ) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iDamage;
		vInfo.push_back( Contribute );
	}
	iInfoCnt = vInfo.size();

	std::sort( vInfo.begin(), vInfo.end(), ContributeSort() );
	ContributeLowerSort( vInfo );

	for(i = 0;i < iInfoCnt;i++)
	{
		int iArray = vInfo[i].m_iRecordArray;
		if( COMPARE( iArray, 0, iCharCnt ) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iAttackCnt;
		vInfo.push_back( Contribute );
	}
	iInfoCnt = vInfo.size();

	std::sort( vInfo.begin(), vInfo.end(), ContributeSort() );
	ContributeLowerSort( vInfo );

	for(i = 0;i < iInfoCnt;i++)
	{
		int iArray = vInfo[i].m_iRecordArray;
		if( COMPARE( iArray, 0, iCharCnt ) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = 100000 - pRecord->Contribute.m_iDeath;          
		vInfo.push_back( Contribute );
	}
	iInfoCnt = vInfo.size();

	std::sort( vInfo.begin(), vInfo.end(), ContributeSort() );
	ContributeLowerSort( vInfo );

	for(i = 0;i < iInfoCnt;i++)
	{
		int iArray = vInfo[i].m_iRecordArray;
		if( COMPARE( iArray, 0, iCharCnt ) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	// 피격 처리
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iWounded;
		vInfo.push_back( Contribute );
	}
	iInfoCnt = vInfo.size();

	std::sort( vInfo.begin(), vInfo.end(), ContributeSort() );
	ContributeLowerSort( vInfo );

	for(i = 0;i < iInfoCnt;i++)
	{
		int iArray = vInfo[i].m_iRecordArray;
		if( COMPARE( iArray, 0, iCharCnt ) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iWoundedRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
	kCharReocrdNameList.clear();
}

int ioDungeonAMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
{
	rRankValue1 = rRankValue2 = rRankValue3 = rRankValue4 = -1;

	bool bObserver = false;
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && ( g_BattleRoomMgr.IsObserverUser(pOwner->GetCharName()) || m_pCreator->IsStealthMode() ) )
		bObserver = true;

	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );

	int i = 0;
	int iCharCnt = kCharReocrdNameList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iKill;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue1 = 0;rRankValue1 < iInfoCnt;rRankValue1++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue1];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iAttackCnt;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue2 = 0;rRankValue2 < iInfoCnt;rRankValue2++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue2];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iDamage;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue3 = 0;rRankValue3 < iInfoCnt;rRankValue3++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue3];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	

	//피격
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iWounded;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue4 = 0;rRankValue4 < iInfoCnt;rRankValue4++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue4];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	
	kCharReocrdNameList.clear();
	return iInfoCnt;
}

int ioDungeonAMode::ContributeRank( const ioHashString &rkName, int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
{
	rRankValue1 = rRankValue2 = rRankValue3 = rRankValue4 = -1;

	bool bObserver = false;
	ioBaseChar *pOwner = m_pCreator->GetBaseChar( rkName );
	if( pOwner && ( g_BattleRoomMgr.IsObserverUser( pOwner->GetCharName() ) || m_pCreator->IsStealthMode() ) )
		bObserver = true;

	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );

	int i = 0;
	int iCharCnt = kCharReocrdNameList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iKill;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue1 = 0;rRankValue1 < iInfoCnt;rRankValue1++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue1];
			if( kContribute.m_szName == rkName )
				break;
		}
	}

	vInfo.clear();	

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iAttackCnt;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue2 = 0;rRankValue2 < iInfoCnt;rRankValue2++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue2];
			if( kContribute.m_szName == rkName )
				break;
		}
	}

	vInfo.clear();	

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iDamage;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue3 = 0;rRankValue3 < iInfoCnt;rRankValue3++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue3];
			if( kContribute.m_szName == rkName )
				break;
		}
	}

	vInfo.clear();	

	//피격
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iWounded;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue4 = 0;rRankValue4 < iInfoCnt;rRankValue4++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue4];
			if( kContribute.m_szName == rkName )
				break;
		}
	}

	vInfo.clear();	
	kCharReocrdNameList.clear();
	return iInfoCnt;
}

void ioDungeonAMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	MonsterSurvivalRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	MonsterSurvivalRecord *pTargetRecord = FindRecordCharName( szTarget );

	ioBaseChar *pAttack = m_pCreator->GetBaseChar( szAttacker );
	ioBaseChar *pTarget = m_pCreator->GetBaseChar( szTarget );
	if( !pAttack || !pTarget )
		return;

	if( pAttack != pTarget )
		CheckNonePlayTime( pAttack );

	//팀 타격 제외 해서 팀버프가 안되었음-_-;
	//임시 수정
	if( iAttackValue != 50 )
	{
		if( pAttack->GetTeam() == pTarget->GetTeam() )
			return;
	}	

	float fAddAttackPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddAttackPoint = 1.1f;

	if( pAttackerRecord && pTargetRecord )
	{
		if( pAttackerRecord->pChar && pTargetRecord->pChar )
		{
			pAttackerRecord->Contribute.m_iAttackCnt += (float)iAttackValue * fAddAttackPoint;
			pTargetRecord->Contribute.m_iWounded += (float)iAttackValue * fAddAttackPoint;
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioDungeonAMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iContribute;		
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );

#if defined( USE_GA )
		if( g_MyInfo.GetPublicID() == pRecord->pChar->GetCharName() )
		{
			g_HttpMng.SetKill( pRecord->GetUniqueTotalKill() );
			g_HttpMng.SetDeath( pRecord->GetUniqueTotalDeath() );
		}
#endif
	}
	kCharReocrdNameList.clear();
}

void ioDungeonAMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iAttackCnt;
		rkPacket << pRecord->Contribute.m_iKill;
		rkPacket << pRecord->Contribute.m_iDeath;
		rkPacket << pRecord->Contribute.m_iWounded;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
	}
	kCharReocrdNameList.clear();
}

void ioDungeonAMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iWounded, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iWounded >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories;

		MonsterSurvivalRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage			= iDamage;
			pRecord->Contribute.m_iAttackCnt		= iAttackCnt;
			pRecord->Contribute.m_iKill             = iKill;
			pRecord->Contribute.m_iDeath            = iDeath;
			pRecord->Contribute.m_iWounded			= iWounded;
			pRecord->Contribute.m_iPrevContributePer= iPrevContributePer;
			pRecord->iUniqueTotalKill = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath= iUniqueTotalDeath;

			if( !g_LadderTeamMgr.IsLadderTeam() )
				g_VictoriesMgr.SetVictories( szName, iVictories );
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioDungeonAMode::OnTurnStart( SP2Packet &rkPacket )
{
	g_GUIMgr.HideWnd( MONSTER_BOSS_GAUGE_WND );

	DWORD dwPrevHighTurn = m_dwCurrentHighTurn;
	DWORD dwPrevLowTurn  = m_dwCurrentLowTurn;

	if( m_bBossTurn )
	{
		// 보스 턴 다음 턴은 HP 풀로 회복
		StartTurnCharHPRecovery();
	}

	rkPacket >> m_bBossTurn >> m_dwAllCurTurn >> m_dwAllMaxTurn >> m_dwCurrentTurnHelpID >> m_dwCurRoundDuration 
			 >> m_dwCurrentHighTurn >> m_dwCurrentLowTurn >> m_dwReduceNpcCreateTime;

	m_StartPosRandom.SetRandomSeed( g_BattleRoomMgr.GetIndex() + m_dwAllCurTurn );
	m_RevivalPosRandom.SetRandomSeed( g_BattleRoomMgr.GetIndex() + m_dwAllCurTurn );

	m_dwPlayStartTime = FRAMEGETTIME();
	m_dwRoundDuration = m_dwCurRoundDuration;
	m_dwCheckWarningTime = 0;
	
	TurnStartEvent( dwPrevHighTurn, dwPrevLowTurn );

	int i = 0;
	// 데스몬 죽음!!
	int iMaxNPC;
	rkPacket >> iMaxNPC;
	for(i = 0;i < iMaxNPC;i++)
	{
		ioHashString kNpcName;
		rkPacket >> kNpcName;
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord == NULL ) continue;

		ioNpcChar *pNpcChar = ToNpcChar( pRecord->pChar );
		if( pNpcChar )
		{
			pNpcChar->StopNPC();
			AddEraseNpc( pNpcChar->GetCharName() );
		}
	}

	// 턴 몬스터
	ioHashString szFirstMonsterName;
	StartMonsterData( rkPacket, StartTurnCameraEventTime(), szFirstMonsterName );	

	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( pTimeScoreWnd )
	{
		pTimeScoreWnd->InitAniState();
		pTimeScoreWnd->UpdateTime( m_dwCurRoundDuration, m_dwRoundDuration );

		// 사운드
		if( COMPARE( m_dwCurrentHighTurn, 0, (int)m_HighTurnList.size() ) )
		{
			HighTurnData &rkTurnData = m_HighTurnList[m_dwCurrentHighTurn];
			if( dwPrevHighTurn != m_dwCurrentHighTurn )
			{				
				if( !rkTurnData.m_szHighTurnStartSnd.IsEmpty() )
					g_SoundMgr.PlaySound( rkTurnData.m_szHighTurnStartSnd );
			}
			else if( !rkTurnData.m_szLowTurnStartSnd.IsEmpty() )
			{
				g_SoundMgr.PlaySound( rkTurnData.m_szLowTurnStartSnd );
			}
		}
	}

	// 첫 턴
	if( m_dwAllCurTurn == 0 )
	{
		if( m_iUseMonsterCoinCnt > 0 )        // 코인 사용하는 난이도
		{
			m_dwCurStartCoinTime = FRAMEGETTIME();
			m_dwCurStartCoinSec  = 0xFFFFFFFF;
		}
		MonsterEventMent( szFirstMonsterName );

		// 첫턴 시작
		m_dwStartTurnTime = FRAMEGETTIME();

	}	// 두번째 턴부터는 이벤트 작동
	else if( COMPARE( m_dwCurrentTurnHelpID, 0, (DWORD)m_TurnHelpList.size() ) )
	{
		StartTurnViewCharStart();
	}		
}

void ioDungeonAMode::OnTurnEnd( SP2Packet &rkPacket )
{
	bool bTurnMonsterAllDie;
	rkPacket >> bTurnMonsterAllDie;

	int i = 0;
	// 턴 종료 처리

	// 데스몬 출격!!
	int iMaxNPC;
	rkPacket >> iMaxNPC;
	for(i = 0;i < iMaxNPC;i++)
	{
		DWORD dwNpcCode, dwStartTime;
		ioHashString kNpcName, kSyncName;
		float fStartXPos, fStartZPos;
		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> dwStartTime >> fStartXPos >> fStartZPos;

		ioNpcChar *pNpcChar = NULL;
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord && ToNpcChar( pRecord->pChar ) )
		{
			pNpcChar = ToNpcChar( pRecord->pChar );
			if( !pNpcChar ) continue;

			pNpcChar->Revival();
			pNpcChar->SetState( CS_LOADING );
			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );		
			pNpcChar->SetTeam( TEAM_RED );
			pNpcChar->StartAI( dwStartTime, fStartXPos, fStartZPos );
			//			pNpcChar->FixedOutLine();
			pNpcChar->SetDeathMonster( true );
		}
		else
		{
			pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
			if( !pNpcChar ) continue;	

			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );		
			pNpcChar->SetTeam( TEAM_RED );
			pNpcChar->StartAI( dwStartTime, fStartXPos, fStartZPos );
			//			pNpcChar->FixedOutLine();
			pNpcChar->SetDeathMonster( true );
			AddNewRecord( pNpcChar );	
		}		
	}

	// 데스몬 출격 헬프~
	if( iMaxNPC > 0 ) 
	{
		if( !m_TurnDeathHelp.m_HelpData.empty() )
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
			kPrinter.SetBkColor( 0, 0, 0 );
			for(i = 0;i < (int)m_TurnDeathHelp.m_HelpData.size();i++)
			{
				TurnHelp &rkData = m_TurnDeathHelp.m_HelpData[i];
				kPrinter.SetTextColor( rkData.m_dwColor );	
				kPrinter.AddTextPiece( rkData.m_fFontScale, rkData.m_szText.c_str() );
			}
			SetCenterHelp( kPrinter, 0, m_TurnDeathHelp.m_dwShowTime );		
		}

		if( !m_TurnDeathHelp.m_szPlaySound.IsEmpty() )
			g_SoundMgr.PlaySound( m_TurnDeathHelp.m_szPlaySound );
	}	

	if( bTurnMonsterAllDie )
	{
		int iRevivalUser;
		rkPacket >> iRevivalUser;
		for(i = 0;i < iRevivalUser;i++)
		{
			ioHashString szRevivalName;
			rkPacket >> szRevivalName;

			MonsterSurvivalRecord *pRecord = FindRecordCharName( szRevivalName );
			if( !pRecord ) return;

			ioBaseChar *pChar = pRecord->pChar;
			if( !pChar ) continue;
		}
	}
}

void ioDungeonAMode::OnRoundEnd( SP2Packet &rkPacket )
{
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

	int iInfoCnt = 0;
	rkPacket >> iInfoCnt;
	for( i=0 ; i<iInfoCnt ; i++ )
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
			for( int k=0;k < iKillCharCnt; ++k )
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

	EndChangeCharacter();

	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		g_MyInfo.SetCharDiedPenalty( pOwner->GetSelectCharArray(), false );
	}

	SetRoundEndInfo( (WinTeamType)iWinTeam, bRoundSetEnd );

	if( m_bRoundSetEnd )
	{
		if( !m_CurChampName.IsEmpty() )
		{
			ioBaseChar *pChamp = GetRecordChar( m_CurChampName );
			if( pChamp )
			{
				pChamp->SetChampEmoticon( true, false );
			}
		}

		if( bResultSync )
		{
			// 최종 기여도를 서버로 전송한다.
			ContributeConvert();      //기여도 환산
			ContributeRoundAllPercent();

			SP2Packet kPacket( CTPK_LAST_PLAYRECORD_INFO );
			FillLastPlayRecordInfo( kPacket );
			TCPNetwork::SendToServer( kPacket );

			// 시상식 없음
		}
	}
}

void ioDungeonAMode::OnFinalResultUserInfo( SP2Packet &rkPacket )
{
	int iUserSize;
	rkPacket >> iUserSize;
	for(int i = 0;i < iUserSize;i++)
	{
		ioHashString szName;
		int iContribute, iRank, iKillDeathLevel, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		bool bSafetyLevel;
		rkPacket >> szName >> iRank >> iContribute >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iKillDeathLevel >> bSafetyLevel;

		// 가장 긴 시간 플레이한 용병 정보
		int iClassType;
		DWORD dwCharIndex;
		rkPacket >> iClassType >> dwCharIndex;

		int iTreasureClearCount;
		rkPacket >> iTreasureClearCount;

		ModeRecord *pRecord = FindModeRecordCharName( szName );
		if( pRecord )
		{
			ContributeResultSync( szName, iContribute );
			pRecord->iUniqueTotalKill  = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath = iUniqueTotalDeath;
			pRecord->iCurRank          = iRank;
			pRecord->iPvEClassType     = iClassType;
			pRecord->dwPvECharIndex    = dwCharIndex;
			pRecord->iResultTreasureCardCount = iTreasureClearCount;
			g_VictoriesMgr.SetVictories( szName, iVictories );
		}		

		if( g_BattleRoomMgr.IsBattleRoom() )
		{
			g_BattleRoomMgr.UpdateUserAbilityLevel( szName, iKillDeathLevel );
			g_BattleRoomMgr.UpdateUserSafetyLevel( szName, bSafetyLevel );
		}
	}
}