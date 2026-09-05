#include "stdafx.h"

#include "..\DataBase\LogDBClient.h"

#include "User.h"
#include "RoomNodeManager.h"
#include "ioEtcItem.h"
#include "ioExerciseCharIndexManager.h"
#include ".\ioexcavationmanager.h"
#include "ioExtraItemInfoManager.h"
#include "ioMyLevelMgr.h"
#include "MissionManager.h"
#include <strsafe.h>

extern CLog EventLOG;

template<> ioExcavationManager* Singleton< ioExcavationManager >::ms_Singleton = 0;

ioExcavationManager::ioExcavationManager(void)
{
	m_RandomTime.Randomize();
	m_RandomItem.Randomize();
	m_RandomGrade.Randomize();
	m_RandomSuccess.Randomize();

	m_dwCurrentTime           = 0;
	m_dwRandomItemSeed        = 0;
	m_dwRandomItemPCRoomSeed  = 0;
	m_dwRandomGradeSeed		  = 0;
	m_dwRandomGradePCRoomSeed = 0;

	m_iMaxArtifact      = 0;
	m_iCreateTimeOffset = 0;
}

ioExcavationManager::~ioExcavationManager(void)
{
	for(vExcavationInfoVector::iterator iter = m_vExcavationInfoVector.begin(); iter != m_vExcavationInfoVector.end(); ++iter)
	{
		ExcavationInfo &Info = *iter;
		Info.m_vArtfactInfoVector.clear();
	}
	m_vExcavationInfoVector.clear();
	m_vItemInfoVector.clear();
	m_vMapInfoVector.clear();
	m_vGradeInfoVector.clear();
}

ioExcavationManager& ioExcavationManager::GetSingleton()
{
	return Singleton< ioExcavationManager >::GetSingleton();
}

void ioExcavationManager::CreateExcavation( int iRoomIndex )
{
	Room *pRoom = g_RoomNodeManager.GetRoomNode( iRoomIndex );
	
	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pRoom == NULL.", __FUNCTION__ );
		return;
	}

	if( pRoom->GetModeType() != MT_TRAINING )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Room is not plaza. (%d)", __FUNCTION__ , pRoom->GetModeType() );
		return;
	}

	ExcavationInfo kInfo;
	kInfo.m_iRoomIndex   = pRoom->GetRoomIndex();
	kInfo.m_iModeSubType = pRoom->GetModeSubNum();

	for (int i = 0; i < m_iMaxArtifact ; i++)
	{
		ArtifactInfo kArtifact;
		kArtifact.m_dwIndex      = 0; // 초기화
		kArtifact.m_dwDeleteTime = 0; // 초기화
		kArtifact.m_iX           = 0; // 초기화
		kArtifact.m_iY           = 0; // 초기화
		kArtifact.m_iZ           = 0; // 초기화
		kInfo.m_vArtfactInfoVector.push_back( kArtifact );	
	}

	m_vExcavationInfoVector.push_back( kInfo );
}


void ioExcavationManager::DeleteExcavation( int iRoomIndex )
{
	for(vExcavationInfoVector::iterator iter = m_vExcavationInfoVector.begin(); iter != m_vExcavationInfoVector.end(); ++iter)
	{
		ExcavationInfo &rkInfo = (*iter);
		if( rkInfo.m_iRoomIndex == iRoomIndex )
		{
			int iSize = rkInfo.m_vArtfactInfoVector.size();
			for (int i = 0; i < iSize ; i++)
			{
				ArtifactInfo &rkArtifactInfo = rkInfo.m_vArtfactInfoVector[i];
			}
			rkInfo.m_vArtfactInfoVector.clear();
			m_vExcavationInfoVector.erase( iter );
			return;
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Excavation is not exist.(%d)", __FUNCTION__, iRoomIndex );
}

void ioExcavationManager::ProcessExcavation()
{
	if(TIMEGETTIME() - m_dwCurrentTime < 60000) return; // 60초 확인
	m_dwCurrentTime = TIMEGETTIME();

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	for(vExcavationInfoVector::iterator iter = m_vExcavationInfoVector.begin(); iter != m_vExcavationInfoVector.end(); ++iter)
	{
	    ExcavationInfo &rkInfo = (*iter);
		int iSize = rkInfo.m_vArtfactInfoVector.size();
		for (int i = 0; i < iSize ; i++)
		{
			ArtifactInfo &rkArtifactInfo = rkInfo.m_vArtfactInfoVector[i];
			if( rkArtifactInfo.m_dwDeleteTime == 0 )
			{
				if( SendCreateArtifact( rkInfo.m_iRoomIndex, rkArtifactInfo.m_dwIndex, true ) )
				{
					// 현재는 로그 없음
				}
				/*else
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s  Fail Artifact Create(%d:%d)", __FUNCTION__, rkInfo.m_iRoomIndex, rkArtifactInfo.m_dwIndex );*/
			}
			else if( rkArtifactInfo.m_dwDeleteTime != 0 && m_dwCurrentTime >= rkArtifactInfo.m_dwDeleteTime )
			{
				SendDeleteArtifact( rkInfo.m_iRoomIndex, rkArtifactInfo.m_dwIndex );

				rkArtifactInfo.m_dwDeleteTime = 0; // 초기화
				rkArtifactInfo.m_iX           = 0; // 초기화
				rkArtifactInfo.m_iY           = 0; // 초기화
				rkArtifactInfo.m_iZ           = 0; // 초기화
				rkArtifactInfo.m_dwIndex = TIMEGETTIME()+ rkInfo.m_iRoomIndex + i; // 서버상에 유니크한 값을 가지도록 

				if( SendCreateArtifact( rkInfo.m_iRoomIndex, rkArtifactInfo.m_dwIndex, false ) )
				{
					// 현재는 로그 없음
				}
				//else
					//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s  Fail Create Artifact For Delete (%d:%d)", __FUNCTION__, rkInfo.m_iRoomIndex, rkArtifactInfo.m_dwIndex );
			}
		}
	}
}

void ioExcavationManager::EnterUser( User *pUser, int iRoomIndex )
{
	if( !m_bSendArtifactEnterUser )
		return;

	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL." , __FUNCTION__ );
		return;
	}

	ExcavationInfo *pExcavationInfo = GetExcavationInfo( iRoomIndex );
	if( !pExcavationInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Excavation is not exist. (%d)", __FUNCTION__ , iRoomIndex );
		return;
	}
	
	int iSize = pExcavationInfo->m_vArtfactInfoVector.size();
	if( iSize == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Artifact is not exist. (%d)", __FUNCTION__ , iRoomIndex );
		return;
	}

	bool bSend = false;
	SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
	kPacket << EXCAVATION_ENTER_USER;
	kPacket << iSize;
	for (int i = 0; i < iSize ; i++)
	{
		ArtifactInfo &rkArtifactInfo = pExcavationInfo->m_vArtfactInfoVector[i];
		if( rkArtifactInfo.m_iX == 0 && rkArtifactInfo.m_iY == 0 && rkArtifactInfo.m_iZ == 0 )
			continue;

		bSend = true;
		kPacket << rkArtifactInfo.m_dwIndex;
		kPacket << rkArtifactInfo.m_iX;
		kPacket << rkArtifactInfo.m_iY;
		kPacket << rkArtifactInfo.m_iZ;
	}	

	if( bSend )
		pUser->SendMessage( kPacket );
}

void ioExcavationManager::OnExcavationPacket( User *pUser, SP2Packet &rkPacket )
{
	int   iCommand = 0;
	rkPacket >> iCommand;

	if( iCommand == EXCAVATION_ATRTIFACT_POSITION )
		_OnArtifactPosition( pUser, rkPacket);
	else if( iCommand == EXCAVATION_REQUEST_JUDGEMENT )
		_OnReseltJugement( pUser, rkPacket );
}

void ioExcavationManager::CheckNeedReload()
{
	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_excavation_info.ini" );
	if( kLoader.ReadBool( "Common", "Change", false ) )
	{
		LoadExcavation();
	}
}

void ioExcavationManager::LoadExcavation()
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadExcavation INI Start!!!!!!!!!!" );

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_excavation_info.ini" );
	
	m_vItemInfoVector.clear();
	m_vMapInfoVector.clear();
	m_vGradeInfoVector.clear();

	m_dwRandomItemSeed		 = 0;
	m_dwRandomItemPCRoomSeed = 0;
	m_dwRandomGradeSeed		 = 0;
	m_dwRandomGradePCRoomSeed= 0;

	m_iMaxArtifact      = 0;
	m_iCreateTimeOffset = 0;

	m_iRealExcavatingUserTime = 0;
	m_iSuccessRatePerOneUser  = 0;
	m_iMaxSuccessRate         = 0;
	m_iExcavatingWaitUserTime = 0;

	kLoader.SetTitle( "Common" );
	//kLoader.SaveBool( "Change", false );	

	m_iMaxArtifact      = kLoader.LoadInt( "Max_Artifact", 0 );
	m_iCreateTimeOffset = kLoader.LoadInt( "Create_Time_Offset", 1 );

	m_iRealExcavatingUserTime = kLoader.LoadInt( "Real_Excavating_User_Time", 0 );
	m_iSuccessRatePerOneUser  = kLoader.LoadInt( "Success_Rate_Per_One_User", 0 );
	m_iMaxSuccessRate         = kLoader.LoadInt( "Max_Success_Rate", 0 );
	m_iExcavatingWaitUserTime = kLoader.LoadInt( "Excavating_Wait_User_Time", 0 );
	m_bSendArtifactEnterUser  = kLoader.LoadBool( "Send_Artifact_Enter_User", false );

	// LOG
	vLogInfoVector vLogInfo;

	kLoader.SetTitle( "ArtifactItem" );
	int iMax = kLoader.LoadInt( "Max_Item", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		ItemInfo kItemInfo;
		char szKey[MAX_PATH]="";
		StringCbPrintf( szKey, sizeof( szKey ), "Item%d_Rand", i+1 );
		kItemInfo.m_dwRand = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey, sizeof( szKey ), "Item%d_PCRand", i+1 );
		kItemInfo.m_dwPCRand = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey, sizeof( szKey ), "Item%d_Alarm", i+1 );
		kItemInfo.m_bAlarm = kLoader.LoadBool( szKey, false );
		StringCbPrintf( szKey, sizeof( szKey ), "Item%d_Type", i+1 );
		kItemInfo.m_iType = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey,  sizeof( szKey ), "Item%d_Value1", i+1 );
		kItemInfo.m_iValue1 = kLoader.LoadInt( szKey, 0 );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Item:%02d] [%d:%d] : %d [%d:%d]", i+1, kItemInfo.m_dwRand, kItemInfo.m_dwPCRand, kItemInfo.m_bAlarm, kItemInfo.m_iType, kItemInfo.m_iValue1 );
		m_dwRandomItemSeed += kItemInfo.m_dwRand;
		m_dwRandomItemPCRoomSeed += kItemInfo.m_dwPCRand;
		m_vItemInfoVector.push_back( kItemInfo );

		// log
		LogInfo kLog;
		kLog.m_iType = kItemInfo.m_iType;
		vLogInfo.push_back( kLog );
	} 

	
	// log
	for (int i = 0; i < 10000; i++)
	{
		DWORD dwRand     = m_RandomItem.Random( m_dwRandomItemSeed );
		DWORD dwCurValue = 0;
		for( vItemInfoVector::iterator iter = m_vItemInfoVector.begin(); iter != m_vItemInfoVector.end(); iter++ )
		{
			ItemInfo &rkInfo = *iter;
			if( COMPARE( dwRand, dwCurValue, dwCurValue + rkInfo.m_dwRand ) ) 
			{
				for(vLogInfoVector::iterator iter = vLogInfo.begin(); iter != vLogInfo.end(); ++iter)
				{
				    LogInfo &rkLog = *iter;
					if( rkLog.m_iType != rkInfo.m_iType)
						continue;
					rkLog.m_iGift++;
				}
				break;
			}
			dwCurValue += rkInfo.m_dwRand;
		}	
	}

	for(vLogInfoVector::iterator iter = vLogInfo.begin(); iter != vLogInfo.end(); ++iter)
	{
		LogInfo &rkInfo = *iter;
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Excavation Artifact Random | Type:%d | %d", rkInfo.m_iType, rkInfo.m_iGift );
	}
	//

	kLoader.SetTitle( "ArtifactGrade" );
	iMax = kLoader.LoadInt( "Max_Grade", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		GradeInfo kGradeInfo;
		char szKey[MAX_PATH]="";
		StringCbPrintf( szKey, sizeof( szKey ), "Grade%d_Type", i+1 );
		kGradeInfo.m_iType = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey, sizeof( szKey ), "Grade%d_Rand", i+1 );
		kGradeInfo.m_dwRand = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey, sizeof( szKey ), "Grade%d_PCRand", i+1 );
		kGradeInfo.m_dwPCRand = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey, sizeof( szKey ), "Grade%d_Rate", i+1 );
		kGradeInfo.m_fRate = kLoader.LoadFloat( szKey, 0.0f );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Grade:[%02d]%d:[%d:%d]:%f", i+1, kGradeInfo.m_iType, kGradeInfo.m_dwRand, kGradeInfo.m_dwPCRand, kGradeInfo.m_fRate );
		m_dwRandomGradeSeed += kGradeInfo.m_dwRand;
		m_dwRandomGradePCRoomSeed += kGradeInfo.m_dwPCRand;
		m_vGradeInfoVector.push_back( kGradeInfo );
	}

	kLoader.SetTitle( "ExcavationMap" );
	iMax = kLoader.LoadInt( "Max_Map", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		MapInfo kMapInfo;
		char szKey[MAX_PATH]="";
		StringCbPrintf( szKey, sizeof( szKey ), "Map%d_Mode_Sub_Type", i+1 );
		kMapInfo.m_iModeSubType = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey, sizeof( szKey ), "Map%d_Live_Time_Min", i+1 );
		kMapInfo.m_dwLiveDelayTimeMin = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey, sizeof( szKey ), "Map%d_Live_Time_Max", i+1 );
		kMapInfo.m_dwLiveDelayTimeMax = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey, sizeof( szKey ), "Map%d_Collision_Length", i+1 );
		kMapInfo.m_iCollisionLength = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf( szKey, sizeof( szKey ), "Map%d_Rate", i+1 );
		kMapInfo.m_fRate = kLoader.LoadFloat( szKey, 1.0f );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Map:%02d] %d[%d~%d]%d(%f)", i+1, kMapInfo.m_iModeSubType, kMapInfo.m_dwLiveDelayTimeMin, kMapInfo.m_dwLiveDelayTimeMax, kMapInfo.m_iCollisionLength, kMapInfo.m_fRate );
		m_vMapInfoVector.push_back( kMapInfo );
	}

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "MaxArtifact:%d | CreateTime:%d | %d | %d | %d | %d | %d {%d:%d}", m_iMaxArtifact, m_iCreateTimeOffset, m_iRealExcavatingUserTime, m_iSuccessRatePerOneUser, m_iMaxSuccessRate, m_iExcavatingWaitUserTime, m_bSendArtifactEnterUser, m_dwRandomItemSeed, m_dwRandomGradeSeed );
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadExcavation INI End" );	
}

DWORD ioExcavationManager::GetReservedTime( int iModeSubType )
{
	// process를 1분마다 체크하므로 기준 값이 분이다.
	DWORD dwGapMinutes = 10; // 기본값 10분
	DWORD dwMinMinutes = 0;
	for(vMapInfoVector::iterator iter = m_vMapInfoVector.begin(); iter != m_vMapInfoVector.end(); ++iter)
	{
	    MapInfo &rkInfo = (*iter);
		if( rkInfo.m_iModeSubType == iModeSubType )
		{
			dwGapMinutes = rkInfo.m_dwLiveDelayTimeMax - rkInfo.m_dwLiveDelayTimeMin;
			dwMinMinutes = rkInfo.m_dwLiveDelayTimeMin;
			break;
		}
	}

	DWORD dwRandomMinutes = m_RandomTime.Random( dwGapMinutes );
	dwRandomMinutes += dwMinMinutes; // 최소시간 더한다.

	return TIMEGETTIME() + ( dwRandomMinutes*60000 ); // *60000 ms로 변경
}

bool ioExcavationManager::SendCreateArtifact( int iRoomIndex, DWORD dwArtifactIndex, bool bWait )
{
	Room *pRoom = g_RoomNodeManager.GetRoomNode( iRoomIndex );

	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pRoom == NULL.", __FUNCTION__ );
		return false;
	}

	if( pRoom->GetPlazaModeType() == PT_GUILD )
		return false;

	if( pRoom->IsRoomEmpty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Room is empty.", __FUNCTION__ );
		return false;
	}

	int iUserCnt = pRoom->GetJoinUserCnt();
	int iArray   = 0;
	if( iUserCnt != 0 )
		iArray   = rand()%iUserCnt;
	User *pUser = pRoom->GetUserNodeByArray( iArray );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
	kPacket << EXCAVATION_CREATE_ARTIFACT;
	kPacket << iRoomIndex;
	kPacket << dwArtifactIndex;
	kPacket << bWait;
	pUser->SendMessage( kPacket );

	return true;
}

void ioExcavationManager::SendDeleteArtifact( int iRoomIndex, DWORD dwArtifactIndex )
{
	Room *pRoom = g_RoomNodeManager.GetRoomNode( iRoomIndex );

	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pRoom == NULL.", __FUNCTION__ );
		return;
	}

	if( pRoom->IsRoomEmpty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Room is empty.", __FUNCTION__ );
		return;
	}

	SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
	kPacket << EXCAVATION_DELETE_ARTIFACT;
	kPacket << dwArtifactIndex;
	pRoom->RoomSendPacketTcp( kPacket );
}

ioExcavationManager::ExcavationInfo *ioExcavationManager::GetExcavationInfo( int iRoomIndex )
{
	for(vExcavationInfoVector::iterator iter = m_vExcavationInfoVector.begin(); iter != m_vExcavationInfoVector.end(); ++iter)
	{
		ExcavationInfo &rkInfo = (*iter);
		if( rkInfo.m_iRoomIndex == iRoomIndex )
		{
			return &rkInfo;
		}
	}

	return NULL;
}

ioExcavationManager::MapInfo *ioExcavationManager::GetMapInfo( int iModeSubType )
{
	for(vMapInfoVector::iterator iter = m_vMapInfoVector.begin(); iter != m_vMapInfoVector.end(); ++iter)
	{
		MapInfo &rkInfo = (*iter);
		if( rkInfo.m_iModeSubType == iModeSubType )
		{
			return &rkInfo;
		}
	}

	return NULL;
}

ioExcavationManager::ItemInfo *ioExcavationManager::GetItemInfo( int iItemType )
{
	for(vItemInfoVector::iterator iter = m_vItemInfoVector.begin(); iter != m_vItemInfoVector.end(); ++iter)
	{
		ItemInfo &rkInfo = (*iter);
		if( rkInfo.m_iType == iItemType )
		{
			return &rkInfo;
		}
	}

	return NULL;
}

void ioExcavationManager::SendGift( User *pUser, Room *pRoom, ExcavationInfo *pExcavationInfo, DWORD dwArtifactIndex )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__);
		return;
	}

	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pRoom == NULL. (%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	if( !pExcavationInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pExcavationInfo == NULL. (%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	MapInfo *pMapInfo = GetMapInfo( pExcavationInfo->m_iModeSubType );
	if( !pMapInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pMapInfo == NULL. (%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	bool  bGift      = false;
	bool  bPCRoom    = pUser->IsPCRoomAuthority();
	bool  bSuccess   = IsRandomSuccess( pUser, pRoom );
	DWORD dwRandValue= GetItemRandomValue( bPCRoom );
	DWORD dwCurValue = 0;
	for( vItemInfoVector::iterator iter = m_vItemInfoVector.begin(); iter != m_vItemInfoVector.end(); iter++ )
	{
		ItemInfo &rkInfo = *iter;
		
		DWORD dwItemRand = rkInfo.GetRandValue( bPCRoom );
		if( bSuccess )
		{
			if( !COMPARE( rkInfo.m_iType , FAIL_ARTIFACT_START_TYPE, EXTRAITEM_START_TYPE ) && 
				COMPARE( dwRandValue, dwCurValue, dwCurValue + dwItemRand ) &&
				dwItemRand > 0 )
			{
				bGift = true;
			}
		}
		else
		{
			if( COMPARE( rkInfo.m_iType , FAIL_ARTIFACT_START_TYPE, EXTRAITEM_START_TYPE ) )
				bGift = true;
		}

		if( bGift )
		{
			// 경험치 처리
			int iAddExp   = g_LevelMgr.GetExcavationFailExp();
			if( bSuccess )
				iAddExp = g_LevelMgr.GetExcavationSuccessExp();
			pUser->AddExcavationExp( iAddExp );

			// 용병 경험치 처리
			bool bGradeLevelUp = false;
			int iAddSoldierExp = 0;
			int iClassType = pUser->GetSelectClassType();
			if( iClassType > 0 )
			{
				if( bSuccess )
					iAddSoldierExp = g_LevelMgr.GetExcavationSuccessSoldierExp();
				else
					iAddSoldierExp = g_LevelMgr.GetExcavationFailSoldierExp();

				if( pUser->IsClassTypeExerciseStyle( iClassType, EXERCISE_RENTAL ) == false )
					pUser->AddClassExp( iClassType, iAddSoldierExp );
				pUser->AddGradeExp( iAddSoldierExp );
				pUser->GradeNClassUPBonus();
			}

			// peso
			if( rkInfo.m_iType < EXTRAITEM_START_TYPE )
			{
				int iGradeType    = GetRandomItemGradeType( bPCRoom );

				if( COMPARE( rkInfo.m_iType , FAIL_ARTIFACT_START_TYPE, EXTRAITEM_START_TYPE ) )
					iGradeType = 6; // F등급 강제

				float fGrade      = GetItemGradeRate( iGradeType );
				int iArtifactPeso = ( rkInfo.m_iValue1 * fGrade ) * pMapInfo->m_fRate;
				pUser->AddMoney( iArtifactPeso );
				g_LogDBClient.OnInsertPeso( pUser, iArtifactPeso, LogDBClient::PT_EXCAVATION_ITEM );
				g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pUser, 0, 0, LogDBClient::LET_ITEM, LogDBClient::PT_EXCAVATION_ITEM, 0, rkInfo.m_iType, iArtifactPeso, NULL);

				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s <%d>(%d:%s)[Peso:%d]", __FUNCTION__, dwArtifactIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iArtifactPeso );

				SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
				kPacket << EXCAVATION_RESULT_JUDGEMENT_SUCCESS;
				kPacket << dwArtifactIndex;
				kPacket << pUser->GetPublicID();
				kPacket << rkInfo.m_iType;
				kPacket << pUser->GetMoney();
				kPacket << rkInfo.m_iValue1; // item peso
				kPacket << iGradeType;
				kPacket << pMapInfo->m_fRate;
				kPacket << rkInfo.m_bAlarm;
				kPacket << pUser->GetExcavationLevel();
				kPacket << pUser->GetExcavationExp();
				kPacket << iAddExp;
				kPacket << iClassType;
				kPacket << iAddSoldierExp;
				kPacket << pUser->GetGradeLevel();
				kPacket << pUser->GetClassLevel( pUser->GetSelectChar(), true );
				pRoom->RoomSendPacketTcp( kPacket );

				// 발굴(excavation)에 관한 전서버 알림
				g_UserNodeManager.AlarmEventInExcavationToAllUsers(pUser, rkInfo.m_bAlarm, rkInfo.m_iType, rkInfo.m_iValue1, iGradeType, pMapInfo->m_fRate);
			}
			else // extraitem
			{
				SendExtraItem( pUser, pRoom, dwArtifactIndex, rkInfo.m_iType, iAddExp, iAddSoldierExp );
			}

			break;
		}
		dwCurValue += dwItemRand;
	}

	if( bGift )
	{
		ArtifactInfo *pArtifactInfo = pExcavationInfo->GetArtifactInfo( dwArtifactIndex );
		if( !pArtifactInfo )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pArtifactInfo == NULL. (%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
			return;
		}

		// 초기화
		pArtifactInfo->m_iX = 0;
		pArtifactInfo->m_iY = 0;
		pArtifactInfo->m_iZ = 0;
		pArtifactInfo->m_dwDeleteTime = 0;
		pArtifactInfo->m_dwIndex = TIMEGETTIME() + pExcavationInfo->m_iRoomIndex;
		if( SendCreateArtifact( pExcavationInfo->m_iRoomIndex, pArtifactInfo->m_dwIndex, true ) )
		{
			// 현재는 로그 없음
		}
		//else
			//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s  Fail Create Artifact(%d:%d)", __FUNCTION__, pExcavationInfo->m_iRoomIndex, pArtifactInfo->m_dwIndex );
	}

	if( !bGift )
	{
		SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
		kPacket << EXCAVATION_RESULT_JUDGEMENT_FAIL;
		kPacket << pUser->GetPublicID();
		pRoom->RoomSendPacketTcp( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail gift <%d>(%d:%s)(%d)", __FUNCTION__, dwArtifactIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwRandValue );
	}
}

int ioExcavationManager::GetRandomItemGradeType( bool bPCRoom )
{
	DWORD dwCurValue  = 0;
	DWORD dwRandValue = GetGradeRandomValue( bPCRoom );
	for( vGradeInfoVector::iterator iter = m_vGradeInfoVector.begin(); iter != m_vGradeInfoVector.end(); iter++ )
	{
		GradeInfo &rkInfo = *iter;
		DWORD dwGradeRand = rkInfo.GetRandValue( bPCRoom );
		if( COMPARE( dwRandValue, dwCurValue, dwCurValue + dwGradeRand ) )
		{
			return rkInfo.m_iType;
		}
		dwCurValue += dwGradeRand;
	}
	return 0;
}

float ioExcavationManager::GetItemGradeRate( int iType )
{
	for( vGradeInfoVector::iterator iter = m_vGradeInfoVector.begin(); iter != m_vGradeInfoVector.end(); iter++ )
	{
		GradeInfo &rkInfo = *iter;
		if( rkInfo.m_iType == iType )
			return rkInfo.m_fRate;
	}

	return 0.0f;
}

void ioExcavationManager::SendExtraItem( User *pUser, Room *pRoom, DWORD dwArtifactIndex, int iItemType, int iAddExp, int iAddSoldierExp )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__);
		return;
	}

	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pRoom == NULL. (%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	ItemInfo *pItemInfo = GetItemInfo( iItemType );
	if( !pItemInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pItemInfo == NULL. (%d:%s)%d", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iItemType );
		return;
	}

	int iMachineCode = pItemInfo->m_iValue1;
	int iTradeTypeList = 0;
	int iItemCode   = g_ExtraItemInfoMgr.GetRandomItemCode( iMachineCode, iTradeTypeList );
	int	iPeriodTime = g_ExtraItemInfoMgr.GetRandomPeriodTime( iMachineCode );
	bool bAlarm     = g_ExtraItemInfoMgr.IsAlarm( iMachineCode, iPeriodTime );
	int iReinforce  = g_ExtraItemInfoMgr.GetRandomReinforce( iMachineCode, false );

	if( !pItemInfo->m_bAlarm ) // excavation과 extraitem 모두 bAlarm이 true 여야한다.
		bAlarm = false;

	ioUserExtraItem::EXTRAITEMSLOT kExtraItem;
	kExtraItem.m_iItemCode  = iItemCode;
	kExtraItem.m_iReinforce = iReinforce;
	kExtraItem.m_PeriodType = ioUserExtraItem::EPT_TIME;

	CTime kLimiteTime = CTime::GetCurrentTime();
	CTimeSpan kAddTime( 0, iPeriodTime, 0, 0 );
	kLimiteTime += kAddTime;

	kExtraItem.SetDate( kLimiteTime.GetYear(), kLimiteTime.GetMonth(), kLimiteTime.GetDay(), kLimiteTime.GetHour(), kLimiteTime.GetMinute() );

	if( iPeriodTime == 0 ) // 무제한
		kExtraItem.m_PeriodType = ioUserExtraItem::EPT_MORTMAIN;
	
	ioUserExtraItem *pItem = pUser->GetUserExtraItem();
	if( !pItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pItem == NULL. (%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );

		SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
		kPacket << EXCAVATION_RESULT_JUDGEMENT_FAIL;
		kPacket << pUser->GetPublicID();
		pRoom->RoomSendPacketTcp( kPacket );
		return;
	}
	
	DWORD dwIndex   = 0;
	int iArrayIndex = 0;
	int iSlotIndex  = pItem->AddExtraItem( kExtraItem, false, 0, LogDBClient::ERT_EXCAVATION, iMachineCode, iPeriodTime, dwIndex, iArrayIndex );

	if( iSlotIndex == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s iSlotIndex == 0. (%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );

		SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
		kPacket << EXCAVATION_RESULT_JUDGEMENT_FAIL;
		kPacket << pUser->GetPublicID();
		pRoom->RoomSendPacketTcp( kPacket );
		return;
	}

	char szItemIndex[MAX_PATH]="";
	if( dwIndex != 0 )
	{
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwIndex, iArrayIndex+1 ); // db field는 1부터 이므로 +1
		g_LogDBClient.OnInsertExtraItem( pUser, kExtraItem.m_iItemCode, kExtraItem.m_iReinforce, iMachineCode, iPeriodTime, 0, kExtraItem.m_PeriodType, kExtraItem.m_dwMaleCustom, kExtraItem.m_dwFemaleCustom, szItemIndex, LogDBClient::ERT_EXCAVATION );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s : <%d> Add ExtraItem(%s:%d) : %d-%d", __FUNCTION__, dwArtifactIndex, pUser->GetPublicID().c_str(), pUser->GetUserIndex(), iMachineCode, kExtraItem.m_iItemCode );

	SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
	kPacket << EXCAVATION_RESULT_JUDGEMENT_SUCCESS;
	kPacket << dwArtifactIndex;
	kPacket << pUser->GetPublicID();
	kPacket << pItemInfo->m_iType;
	kPacket << kExtraItem.m_iItemCode;
	kPacket << kExtraItem.m_iReinforce;
	kPacket << iSlotIndex;
	kPacket << kExtraItem.m_PeriodType;
	kPacket << kExtraItem.m_iValue1;
	kPacket << kExtraItem.m_iValue2;
	kPacket << iPeriodTime;
	kPacket << iMachineCode;
	kPacket << bAlarm;
	kPacket << pUser->GetExcavationLevel();
	kPacket << pUser->GetExcavationExp();
	kPacket << iAddExp;
	kPacket << pUser->GetSelectClassType();
	kPacket << iAddSoldierExp;
	kPacket << pUser->GetGradeLevel();
	kPacket << pUser->GetClassLevel( pUser->GetSelectChar(), true );
	pRoom->RoomSendPacketTcp( kPacket );

	pUser->SaveExtraItem();

	// 발굴(excavation)에 관한 전서버 알림
	g_UserNodeManager.AlarmEventInExcavationToAllUsers(pUser, bAlarm, pItemInfo->m_iType, kExtraItem.m_iValue1, kExtraItem.m_iValue2, 0);
}

void ioExcavationManager::_OnReseltJugement( User *pUser, SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	int iRoomIndex = 0;
	int iX = 0;
	int iY = 0;
	int iZ = 0;
	rkPacket >> iRoomIndex;
	rkPacket >> iX;
	rkPacket >> iY;
	rkPacket >> iZ;

	Room *pRoom = g_RoomNodeManager.GetRoomNode( iRoomIndex );
	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pRoom == NULL. [%d](%d:%s)", __FUNCTION__, iRoomIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	ExcavationInfo *pExcavationInfo = GetExcavationInfo( iRoomIndex );
	if( !pExcavationInfo )
	{
		SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
		kPacket << EXCAVATION_RESULT_JUDGEMENT_FAIL;
		kPacket << pUser->GetPublicID();
		pRoom->RoomSendPacketTcp( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Excavation is not exist.(%d)(%d:%s)", __FUNCTION__, iRoomIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	MapInfo *pMapInfo = GetMapInfo( pExcavationInfo->m_iModeSubType );
	if( !pMapInfo )
	{
		SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
		kPacket << EXCAVATION_RESULT_JUDGEMENT_FAIL;
		kPacket << pUser->GetPublicID();
		pRoom->RoomSendPacketTcp( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s MapInfo is not exist.(%d)(%d:%s)", __FUNCTION__, pExcavationInfo->m_iModeSubType, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kEtcSlot;
	ioUserEtcItem::ETCITEMSLOT kGoldEtcSlot;

	pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_EXCAVATING_KIT, kEtcSlot ); 
	pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_GOLD_EXCAVATING_KIT, kGoldEtcSlot );
	// no item
	if( kEtcSlot.m_iType == 0 && kGoldEtcSlot.m_iType == 0 )
	{
		SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
		kPacket << EXCAVATION_RESULT_JUDGEMENT_FAIL;
		kPacket << pUser->GetPublicID();
		pRoom->RoomSendPacketTcp( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s have not kit.[%d](%d:%s)", __FUNCTION__, iRoomIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	// no use
	if( !kEtcSlot.IsUse() && !kGoldEtcSlot.IsUse() )
	{
		SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
		kPacket << EXCAVATION_RESULT_JUDGEMENT_FAIL;
		kPacket << pUser->GetPublicID();
		pRoom->RoomSendPacketTcp( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s not use kit.[%d](%d:%s)", __FUNCTION__, iRoomIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	DWORD dwWaiteTime = TIMEGETTIME() - pUser->GetTryExcavatedTime();
	pUser->SetTryExcavatedTime( TIMEGETTIME() );

	if( (int)dwWaiteTime < m_iExcavatingWaitUserTime )
	{
		SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
		kPacket << EXCAVATION_RESULT_JUDGEMENT_FAIL;
		kPacket << pUser->GetPublicID();
		pRoom->RoomSendPacketTcp( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Wait Time Error.[%d](%d:%s)", __FUNCTION__, iRoomIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	bool  bSuccess        = false;
	DWORD dwArtifactIndex = 0;
	int iSize = pExcavationInfo->GetArtifactInfoSize();
	for (int i = 0; i < iSize ; i++)
	{
		ArtifactInfo *pArtifactInfo = pExcavationInfo->GetArtifactInfoByArray( i );
		if( !pArtifactInfo )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Artifact is not exist.(%d:%d)(%d:%s)", __FUNCTION__, iRoomIndex, i, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
			continue;
		}

		if( pArtifactInfo->m_iX == 0 && 
			pArtifactInfo->m_iY == 0 && 
			pArtifactInfo->m_iZ == 0 )
			continue;

 		if( !COMPARE( iX, pArtifactInfo->m_iX, pArtifactInfo->m_iX+pMapInfo->m_iCollisionLength ) &&
 			!COMPARE( iX, pArtifactInfo->m_iX-pMapInfo->m_iCollisionLength, pArtifactInfo->m_iX ) )
 			continue;
 
 		if( !COMPARE( iY, pArtifactInfo->m_iY, pArtifactInfo->m_iY+pMapInfo->m_iCollisionLength ) &&
 			!COMPARE( iY, pArtifactInfo->m_iY-pMapInfo->m_iCollisionLength, pArtifactInfo->m_iY ) )
 			continue;
 
 		if( !COMPARE( iZ, pArtifactInfo->m_iZ, pArtifactInfo->m_iZ+pMapInfo->m_iCollisionLength ) &&
 			!COMPARE( iZ, pArtifactInfo->m_iZ-pMapInfo->m_iCollisionLength, pArtifactInfo->m_iZ ) )
 			continue;

		bSuccess = true;
		dwArtifactIndex = pArtifactInfo->m_dwIndex;
		break;
	}
	
	//미션 판단.
	static DWORDVec vValues;
	vValues.clear();
	if( bSuccess )
		vValues.push_back(1);
	else
		vValues.push_back(2);

	g_MissionMgr.DoTrigger(MISSION_CLASS_EXCAVATION, pUser, vValues);

	if( !bSuccess )
	{
		SP2Packet kPacket( STPK_EXCAVATION_COMMAND );
		kPacket << EXCAVATION_RESULT_JUDGEMENT_FAIL;
		kPacket << pUser->GetPublicID();
		pRoom->RoomSendPacketTcp( kPacket );
		return;
	}

	SendGift( pUser, pRoom, pExcavationInfo, dwArtifactIndex );
}

void ioExcavationManager::_OnArtifactPosition( User *pUser, SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	int   iRoomIndex     = 0;
	DWORD dwArtifactIndex = 0;
	rkPacket >> iRoomIndex;
	rkPacket >> dwArtifactIndex;

	Room *pRoom = g_RoomNodeManager.GetRoomNode( iRoomIndex );
	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pRoom == NULL. [%d](%d:%s)", __FUNCTION__, iRoomIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	ExcavationInfo *pExcavationInfo = GetExcavationInfo( iRoomIndex );
	if( !pExcavationInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Excavation is not exist.(%d:%d)(%d:%s)", __FUNCTION__, iRoomIndex, dwArtifactIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	ArtifactInfo *pArtifactInfo = pExcavationInfo->GetArtifactInfo( dwArtifactIndex );
	if( !pArtifactInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Artifact is not exist.(%d:%d)(%d:%s)", __FUNCTION__, iRoomIndex, dwArtifactIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	if( pArtifactInfo->m_dwDeleteTime != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Artifact error delete time.(%d:%d)(%d:%s)", __FUNCTION__, iRoomIndex, dwArtifactIndex, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	if( pArtifactInfo->m_iX != 0 && 
		pArtifactInfo->m_iY != 0 && 
		pArtifactInfo->m_iZ != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s position is error.[%d](%d:%d:%d)(%d:%s)", __FUNCTION__, iRoomIndex, pArtifactInfo->m_iX, pArtifactInfo->m_iY, pArtifactInfo->m_iZ, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	rkPacket >> pArtifactInfo->m_iX;
	rkPacket >> pArtifactInfo->m_iY;
	rkPacket >> pArtifactInfo->m_iZ;

	SP2Packet kResult( STPK_EXCAVATION_COMMAND );
	kResult << EXCAVATION_RESULT_POSITION;
	kResult << dwArtifactIndex;
	kResult << pArtifactInfo->m_iX;
	kResult << pArtifactInfo->m_iY;
	kResult << pArtifactInfo->m_iZ;
	pRoom->RoomSendPacketTcp( kResult );

	pArtifactInfo->m_dwDeleteTime = GetReservedTime( pExcavationInfo->m_iModeSubType );
}

bool ioExcavationManager::IsRandomSuccess( User *pUser, Room *pRoom )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pRoom == NULL. (%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return false;
	}

	int iExcavatingUserCnt = pRoom->GetExcavatingUserCnt();

	int iPercent = pRoom->GetExcavatingUserCnt() * m_iSuccessRatePerOneUser;
	if( iPercent > m_iMaxSuccessRate )
		iPercent = m_iMaxSuccessRate;

	enum { MAX_PERCENT = 100, };
	int iRandom = m_RandomSuccess.Random( MAX_PERCENT );
	
	if( iRandom < iPercent )
		return true;

	return false;
}

DWORD ioExcavationManager::GetItemRandomValue( bool bPCRoom )
{
	if( bPCRoom )
		return m_RandomItem.Random( m_dwRandomItemPCRoomSeed );
	return m_RandomItem.Random( m_dwRandomItemSeed );
}

DWORD ioExcavationManager::GetGradeRandomValue( bool bPCRoom )
{
	if( bPCRoom )
		return m_RandomGrade.Random( m_dwRandomGradePCRoomSeed );
	return m_RandomGrade.Random( m_dwRandomGradeSeed );
}

void ioExcavationManager::CheckSendCreateArtifact( int iRoomIndex )
{
	for(vExcavationInfoVector::iterator iter = m_vExcavationInfoVector.begin(); iter != m_vExcavationInfoVector.end(); ++iter)
	{
		ExcavationInfo &rkInfo = (*iter);
		if( rkInfo.m_iRoomIndex != iRoomIndex )
			continue;
		int iSize = rkInfo.m_vArtfactInfoVector.size();
		for (int i = 0; i < iSize ; i++)
		{
			ArtifactInfo &rkArtifactInfo = rkInfo.m_vArtfactInfoVector[i];
			if( rkArtifactInfo.m_dwIndex == 0 )
			{
				rkArtifactInfo.m_dwIndex = TIMEGETTIME() + rkInfo.m_iRoomIndex + i; // 서버상에 유니크한 값을 가지도록 
				if( SendCreateArtifact( rkInfo.m_iRoomIndex, rkArtifactInfo.m_dwIndex, false ) )
				{
					// 현재는 로그 없음
				}
				//else
					//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s  Fail Create Artifact(%d:%d)", __FUNCTION__, rkInfo.m_iRoomIndex, rkArtifactInfo.m_dwIndex );
			}
		}
	}
}