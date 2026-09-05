
#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "Mode.h"
#include "Room.h"
#include "ModeSelectManager.h"
#include "../EtcHelpFunc.h"


ModeSelectManager::ModeSelectManager( Room *pCreator )
{
	m_pCreator = pCreator;

	m_iModeIndex = -1;
	
	m_CurModeType = MT_NONE;
	m_iCurSubModeType = 0;
	m_iCurMapNum = 0;
	
	m_vModeTypeList.clear();
	m_vModeInfoList.clear();
}

ModeSelectManager::~ModeSelectManager()
{
	m_vModeTypeList.clear();
	m_vModeInfoList.clear();
}

void ModeSelectManager::InitModeInfoList(int iSelectValue)
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( "config/sp2_mode_select.ini" );

	if( m_pCreator->GetRoomStyle() == RSTYLE_BATTLEROOM || m_pCreator->GetRoomStyle() == RSTYLE_SHUFFLEROOM )
	{
		if( m_pCreator->IsSafetyLevelRoom() )
			rkLoader.SetTitle( "SafetyModeList" );
		else if( m_pCreator->IsBroadcastRoom() )
			rkLoader.SetTitle( "BroadcastModeList" );
		else
			rkLoader.SetTitle( "ShamModeList" );
	}
	else if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		rkLoader.SetTitle( "LadderTeamModeList" );
	}
	else if( m_pCreator->GetRoomStyle() == RSTYLE_MATCHROOM )
	{
		rkLoader.SetTitle( "SuccessionModeList" );
	}
	else if( m_pCreator->GetRoomStyle() == RSTYLE_PLAZA )
	{
		rkLoader.SetTitle( "PlazaList" );
	}
	else if( m_pCreator->GetRoomStyle() == RSTYLE_HEADQUARTERS )
	{
		if( MT_HEADQUARTERS == iSelectValue )
			rkLoader.SetTitle( "HeadquartersList" );
		else if( MT_HOUSE == iSelectValue )
			rkLoader.SetTitle( "HomeModeList" );
	}
	else
		return;

	int i;
	int iCnt = rkLoader.LoadInt( "mode_list_size", 0 );
	for(i = 0;i < iCnt;++i)
	{
		char szKey[MAX_PATH] = "";

		wsprintf( szKey, "mode%d_active", i+1 );
		if( rkLoader.LoadBool( szKey, true ) == false )
			continue;
		ZeroMemory( szKey, sizeof( szKey ) );
		wsprintf( szKey, "mode%d_index", i+1 );
		
		int iModeIndex = rkLoader.LoadInt( szKey, 0 );

		ZeroMemory( szKey, sizeof( szKey ) );
		wsprintf( szKey, "mode%d_minuser", i+1 );

		int iModeMinUser = rkLoader.LoadInt( szKey, 0 );

		ModeListInfo kModeListInfo;
		kModeListInfo.m_iModeIndex = iModeIndex;
		kModeListInfo.m_iModeMinUser = iModeMinUser;

		m_vModeTypeList.push_back( kModeListInfo );
	}

	// 랜덤으로 돌리는 모드 리스트
	iCnt = rkLoader.LoadInt( "rand_list_size", 0 );
	for(i = 0;i < iCnt;++i)
	{
		char szKey[MAX_PATH] = "";
		wsprintf( szKey, "rand%d_active", i+1 );
		if( rkLoader.LoadBool( szKey, true ) == false )
			continue;
		ZeroMemory( szKey, sizeof( szKey ) );
		sprintf_s( szKey, "rand%d_index", i+1 );

		int iModeIndex = rkLoader.LoadInt( szKey, 0 );
		for(int j = 0;j < (int)m_vModeTypeList.size();j++)
		{
			if( m_vModeTypeList[j].m_iModeIndex == iModeIndex )
			{
				m_vModeTypeList[j].m_bRandomPossible = true;
				break;
			}
		}		
	}

	// 플레이 모드 리스트
	iCnt = m_vModeTypeList.size();
	for(i = 0;i < iCnt;++i)
	{
		char szKey[MAX_PATH] = "";

		int iModeIndex = m_vModeTypeList[i].m_iModeIndex;
		if( IsExistModeInfo( iModeIndex ) )
			continue;

		if( iModeIndex == -1 ) continue;

		wsprintf( szKey, "mode%d", iModeIndex );
		rkLoader.SetTitle( szKey );

		SelectModeInfo kModeInfo;
		LoadModeInfo( rkLoader, kModeInfo );

		kModeInfo.m_iModeIndex = iModeIndex;
		m_vModeTypeList[i].m_iModeType = kModeInfo.m_iModeType;

		m_vModeInfoList.push_back( kModeInfo );
	}
}

bool ModeSelectManager::IsExistModeInfo( int iModeIndex )
{
	if( m_vModeInfoList.empty() )
		return false;

	int iModeCnt = m_vModeInfoList.size();
	for( int i=0; i < iModeCnt; ++i )
	{
		if( m_vModeInfoList[i].m_iModeIndex == iModeIndex )
			return true;
	}

	return false;
}

void ModeSelectManager::LoadModeInfo( ioINILoader &rkLoader, SelectModeInfo &rkModeInfo )
{
	rkModeInfo.m_iModeType = rkLoader.LoadInt( "mode_type", 0 );

	int i, j;
	int iSubCnt = rkLoader.LoadInt( "sub_cnt", 0 );
	for(i=0; i < iSubCnt; ++i )
	{
		char szKey[MAX_PATH] = "";

		SubSelectModeInfo kSubModeInfo;

		sprintf_s( szKey, "sub%d_active", i+1 );
		kSubModeInfo.m_bActive = rkLoader.LoadBool( szKey, true );
			
		wsprintf( szKey, "sub%d_type", i+1 );
		kSubModeInfo.m_iSubType = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "sub%d_map_cnt", i+1 );
		int iMapCnt = rkLoader.LoadInt( szKey, 0 );
		
		for( j=0; j < iMapCnt; ++j )
		{
			wsprintf( szKey, "sub%d_map%d", i+1, j+1 );
			int iMapNum = rkLoader.LoadInt( szKey, 0 );

			kSubModeInfo.m_vMapList.push_back( iMapNum );
		}

		kSubModeInfo.MapListShuffle();

		rkModeInfo.m_vSubModeList.push_back( kSubModeInfo );
	}

	// inactive sub mode 처리, 기존 값으로 셋팅함 array를 유지하기 위해서
	int iSize = rkModeInfo.m_vSubModeList.size();
	IntVec vActiveArray;
	vActiveArray.reserve( iSize );
	for (i = 0; i < iSize ; i++)
	{
		SubSelectModeInfo &rCurInfo = rkModeInfo.m_vSubModeList[i];
		if( rCurInfo.m_bActive )
			vActiveArray.push_back( i );
	}
	int iActiveArraySize = vActiveArray.size();
	int iActiveCnt = 0;
	for (i = 0; i < iSize ; i++)
	{
		SubSelectModeInfo &rInActivekInfo = rkModeInfo.m_vSubModeList[i];
		if( rInActivekInfo.m_bActive )
			continue;
		if( iActiveArraySize <= 0 )
			break;
		int iArray = vActiveArray[iActiveCnt];
		if( !COMPARE( iArray, 0, iSize ) )
			break;
		SubSelectModeInfo &rActivekInfo = rkModeInfo.m_vSubModeList[iArray];
		rInActivekInfo = rActivekInfo;
		rInActivekInfo.m_bActive = false;
		iActiveCnt++;
		if( iActiveCnt >= iActiveArraySize )
			iActiveCnt = 0;
	}
	vActiveArray.clear();
	//

	rkModeInfo.SubModeListShuffle();
}

void ModeSelectManager::SetPreModeInfo( ModeType eModeType, int iSubMode, int iMapNum )
{
	if( m_vModeTypeList.empty() || m_vModeInfoList.empty() )
		return;

	if( eModeType == MT_NONE ) return;

	GetSelectModeTypeIndex( (int)eModeType );

	int iModeIndex = m_vModeTypeList[m_iModeIndex].m_iModeIndex;
	SelectModeInfo *pModeInfo = GetSelectModeInfo( iModeIndex );
	if( !pModeInfo ) return;

	if( pModeInfo->m_vSubModeList.empty() ) return;

	SubSelectModeInfo *pSubModeInfo = GetSubSelectModeInfo( pModeInfo, iSubMode );
	if( !pSubModeInfo ) return;

	GetSelectMapNum( pSubModeInfo, iMapNum );
}

void ModeSelectManager::SelectGuildRoomNextModeInfo()
{
	m_CurModeType = MT_TRAINING;
	m_iCurSubModeType = 16;
	m_iCurMapNum = 1;
}

void ModeSelectManager::SelectNextModeInfo( ModeType eModeType, int iSubMode, int iModeMapNum )
{
	int iModeCnt = m_vModeTypeList.size();

	if( iModeCnt == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ModeSelectManager::SelectNextModeInfo() - The ModeList is not Exist" );
		return;
	}

	if( eModeType == MT_NONE )   // 랜덤 선택된 모드
	{
		NextRandomMode();
	}
	else						 // 선택된 모드
	{
		NextSelectMode( (int)eModeType, iSubMode, iModeMapNum );
	}
}

void ModeSelectManager::NextRandomModeIndex( int iModeCnt, int iCurLoop )
{
	if( m_iModeIndex == -1 )
	{
		m_iModeIndex = rand() % iModeCnt;
	}
	else
	{
		m_iModeIndex++;
		if( !COMPARE( m_iModeIndex, 0, iModeCnt ) )
			m_iModeIndex = 0;
	}
	
	if( !m_vModeTypeList[m_iModeIndex].m_bRandomPossible )
	{

		if( iCurLoop < iModeCnt )
			NextRandomModeIndex( iModeCnt, iCurLoop + 1 );
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ModeSelectManager::NextRandomModeIndex 재귀 호출 실패!! %d - %d", iCurLoop, iModeCnt );
	}
}

bool ModeSelectManager::NextRandomMode()					// 랜덤 선택된 모드
{
	NextRandomModeIndex( m_vModeTypeList.size() );
	int iModeIndex = m_vModeTypeList[m_iModeIndex].m_iModeIndex;
	SelectModeInfo *pModeInfo = GetSelectModeInfo( iModeIndex );
	if( !pModeInfo ) return false;

	m_CurModeType = (ModeType)pModeInfo->m_iModeType;

	SubSelectModeInfo *pSubModeInfo = GetSubSelectModeInfo( pModeInfo );
	if( !pSubModeInfo ) return false;

	m_iCurSubModeType = pSubModeInfo->m_iSubType;
	m_iCurMapNum = GetSelectMapNum( pSubModeInfo );

	if( m_iCurMapNum == -1 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ModeSelectManager::NextRandomMode() - Map is not Exist" );
		m_iCurMapNum = 0;
	}

	return true;
}

bool ModeSelectManager::NextSelectMode( int iModeType, int iSubType, int iModeMapNum )           // 선택된 모드
{
	GetSelectModeTypeIndex( iModeType );

	bool bSuccess = true;
	if( m_iModeIndex == -1 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ModeSelectManager::NextSelectMode() -Not Find ModeType: %d", iModeType );
		bSuccess = NextRandomMode();
	}
	else
	{
		int iCurModeIndex = m_vModeTypeList[m_iModeIndex].m_iModeIndex;
		SelectModeInfo *pModeInfo = GetSelectModeInfo( iCurModeIndex );
		if( !pModeInfo )
		{
			char szLog[2048] = "";
			sprintf_s( szLog, "GetSelectModeInfo Fail : %d, %d, %d", iModeType, (int)m_iModeIndex, iCurModeIndex );
			SP2Packet kPacket( LUPK_LOG );
			kPacket << "ServerError";
			kPacket << szLog;
			g_UDPNode.SendLog( kPacket );
			return false;
		}

		m_CurModeType = (ModeType)pModeInfo->m_iModeType;

		SubSelectModeInfo *pSubModeInfo = GetSubSelectModeInfo( pModeInfo, iSubType );
		if( !pSubModeInfo )
		{
			return false;
		}

		m_iCurSubModeType = pSubModeInfo->m_iSubType;
		m_iCurMapNum = GetSelectMapNum( pSubModeInfo, iModeMapNum );

		if( m_iCurMapNum == -1 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ModeSelectManager::NextSelectMode() - Map is not Exist" );
			m_iCurMapNum = 0;
		}
	}

	return bSuccess;
}

int ModeSelectManager::GetSelectMapNum( SubSelectModeInfo *pSubModeInfo, int iMapNum )
{
	int iMapCnt = pSubModeInfo->m_vMapList.size();
	if( iMapCnt == 0 ) return -1;

	int iCurMapIndex = 0;
	if( iMapNum == -1 )
	{
		if( pSubModeInfo->m_iMapIndex != -1 )
		{
			iCurMapIndex = pSubModeInfo->m_iMapIndex;
			iCurMapIndex++;

			if( !COMPARE(iCurMapIndex, 0, iMapCnt) )
				iCurMapIndex = 0;
		}
	}
	else
	{
		iCurMapIndex = -1;
		for( int i=0; i < iMapCnt; ++i )
		{
			if( pSubModeInfo->m_vMapList[i] == iMapNum )
			{
				iCurMapIndex = i;
				break;
			}
		}

		if( iCurMapIndex == -1 )
			iCurMapIndex = 0;
	}

	pSubModeInfo->m_iMapIndex = iCurMapIndex;
	return pSubModeInfo->m_vMapList[iCurMapIndex];
}

void ModeSelectManager::MatchingSelectMapNum( int iModeType )
{
	GetSelectModeTypeIndex( iModeType );

	bool bSuccess = true;
	if( m_iModeIndex == -1 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ModeSelectManager::NextSelectMode() -Not Find ModeType: %d", iModeType );
		bSuccess = NextRandomMode();
	}
	else
	{
		int iCurModeIndex = m_vModeTypeList[m_iModeIndex].m_iModeIndex;
		SelectModeInfo *pModeInfo = GetSelectModeInfo( iCurModeIndex );
		if( !pModeInfo )
		{
			char szLog[2048] = "";
			sprintf_s( szLog, "GetSelectModeInfo Fail : %d, %d, %d", iModeType, (int)m_iModeIndex, iCurModeIndex );
			SP2Packet kPacket( LUPK_LOG );
			kPacket << "ServerError";
			kPacket << szLog;
			g_UDPNode.SendLog( kPacket );
			return;
		}

		m_CurModeType = (ModeType)pModeInfo->m_iModeType;

		if( m_iCurMapNum == -1 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ModeSelectManager::NextSelectMode() - Map is not Exist" );
			m_iCurMapNum = 0;
		}
	}

	return;
}

int ModeSelectManager::GetSelectModeTypeIndex( int iModeType )
{
	int iModeCnt = m_vModeTypeList.size();
	if( iModeCnt == 0 ) return -1;

	if( m_iModeIndex == -1 )
		m_iModeIndex = 0;

	int iCurIndex = m_iModeIndex;
	LOOP_GUARD();
	do
	{
		if( m_vModeTypeList[iCurIndex].m_iModeType == iModeType )
		{
			m_iModeIndex = iCurIndex;
			return m_iModeIndex;
		}

		iCurIndex++;
		if( !COMPARE(iCurIndex, 0, iModeCnt) )
			iCurIndex = 0;

	} while( iCurIndex != m_iModeIndex );
	LOOP_GUARD_CLEAR();
	return -1;
}

SelectModeInfo* ModeSelectManager::GetSelectModeInfo( int iModeIndex )
{
	if( iModeIndex > 0 )
	{
		int iCnt = m_vModeInfoList.size();
		for( int i=0; i < iCnt; ++i )
		{
			if( m_vModeInfoList[i].m_iModeIndex == iModeIndex )
				return &m_vModeInfoList[i];
		}
	}

	return NULL;
}

SubSelectModeInfo* ModeSelectManager::GetSubSelectModeInfo( SelectModeInfo *pModeInfo, int iSubModeType )
{
	if( !pModeInfo )
	{
		char szLog[2048] = "";
		sprintf_s( szLog, "GetSubSelectModeInfo Fail 1: %d, %d", iSubModeType, (int)m_iCurSubModeType );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return NULL;
	}

	if( pModeInfo->m_vSubModeList.empty() )
	{
		char szLog[2048] = "";
		sprintf_s( szLog, "GetSubSelectModeInfo Fail 2: %d, %d, %d", pModeInfo->m_iModeType, iSubModeType, (int)m_iCurSubModeType );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return NULL;
	}

	int iSubCnt = pModeInfo->m_vSubModeList.size();
	if( iSubModeType == -1 )
	{
		int iPreIndex, iCurIndex;
		iPreIndex = iCurIndex = pModeInfo->m_iSubModeIndex;
		
		if( iPreIndex == -1 )
		{
			pModeInfo->m_iSubModeIndex = 0;
			pModeInfo->m_vSubModeList[0].m_bUse = true;
			return &pModeInfo->m_vSubModeList[0];
		}
		LOOP_GUARD();
		do
		{
			iCurIndex++;
			if( !COMPARE(iCurIndex, 0, iSubCnt) )
				iCurIndex = 0;

			if( !pModeInfo->m_vSubModeList[iCurIndex].m_bUse )
			{
				pModeInfo->m_iSubModeIndex = iCurIndex;
				pModeInfo->m_vSubModeList[iCurIndex].m_bUse = true;
				return &pModeInfo->m_vSubModeList[iCurIndex];
			}
		} while( iPreIndex != iCurIndex );
		LOOP_GUARD_CLEAR();

		pModeInfo->InitSubModeList();

		iCurIndex++;
		if( !COMPARE(iCurIndex, 0, iSubCnt) )
			iCurIndex = 0;

		pModeInfo->m_iSubModeIndex = iCurIndex;
		pModeInfo->m_vSubModeList[iCurIndex].m_bUse = true;
		return &pModeInfo->m_vSubModeList[iCurIndex];
	}
	else
	{
		for( int i=0; i < iSubCnt; ++i )
		{
			int iType = pModeInfo->m_vSubModeList[i].m_iSubType;
			if( iType == iSubModeType )
			{
				pModeInfo->m_iSubModeIndex = i;
				pModeInfo->m_vSubModeList[i].m_bUse = true;
				return &pModeInfo->m_vSubModeList[i];
			}
		}
	}

	char szLog[2048] = "";
	sprintf_s( szLog, "GetSubSelectModeInfo Fail 3: %d, %d, %d", pModeInfo->m_iModeType, iSubModeType, (int)m_iCurSubModeType );
	SP2Packet kPacket( LUPK_LOG );
	kPacket << "ServerError";
	kPacket << szLog;
	g_UDPNode.SendLog( kPacket );

	return NULL;
}

int ModeSelectManager::GetModeMinUserCnt()
{
	if( COMPARE( m_iModeIndex, 0, (int)m_vModeTypeList.size() ) )
	{
		return m_vModeTypeList[m_iModeIndex].m_iModeMinUser;
	}

	return 0;
}
