// RoomNodeManager.cpp: implementation of the RoomNodeManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"

#include "RoomNodeManager.h"
#include "LevelMatchManager.h"
#include "ServerNodeManager.h"
#include "ioExcavationManager.h"
#include "GuildRoomsBlockManager.h"
#include "HouseMode.h"
#include "HomeModeBlockManager.h"

#include <algorithm>


RoomNodeManager *RoomNodeManager::sg_Instance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RoomNodeManager::RoomNodeManager()
{
	m_dwRoomProcessTime		= 0;
	m_iPlazaRoomNumber		= 0;
	m_dwGuildRoomLifeTime	= 0;
}

RoomNodeManager::~RoomNodeManager()
{
	m_vRoomNode.clear();
	m_vPlazaNode.clear();
	m_vHeadquartersNode.clear();

	m_vPlazaNameList.clear();
	
	m_vRoomCopyNode.clear();
	m_vPlazaCopyNode.clear();
	m_vHeadquartersCopyNode.clear();
}

RoomNodeManager& RoomNodeManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new RoomNodeManager;

	return *sg_Instance;
}

void RoomNodeManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void RoomNodeManager::InitPlazaInfo()
{
	ioINILoader kLoader( "config/sp2_plaza.ini" );
	kLoader.SetTitle( "create" );
	int iMaxName = kLoader.LoadInt( "MAX_NAME", 0 );
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	for(int i = 0;i < iMaxName;i++)
	{
		sprintf_s( szKey, "NAME_%d", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) != 0 )
			m_vPlazaNameList.push_back( szBuf );
	}
}

void RoomNodeManager::InitMemoryPool( const DWORD dwServerIndex )
{
	ioINILoader kLoader( "ls_config_game.ini" );
	kLoader.SetTitle( "MemoryPool" );
	int iMaxRoomNodePool = kLoader.LoadInt( "room_pool", 1000 );
	int iStartIndex      = dwServerIndex * iMaxRoomNodePool;
	m_iPlazaRoomNumber   = ( iStartIndex / 10 ) + 1;

	// MemPooler
	m_MemNode.CreatePool( 0, iMaxRoomNodePool, FALSE );
	for(int i = 0;i < iMaxRoomNodePool;i++)
	{
		Room *pRoom = new Room( iStartIndex + i );
		m_MemNode.Push( pRoom );
	}

	InitPlazaInfo();
	INILoadToGuildRoomLifeTime();

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][room]Init memory pool : [%d ~ %d]", iStartIndex, iStartIndex + iMaxRoomNodePool - 1 );
}

void RoomNodeManager::ReleaseMemoryPool()
{
	vRoom_iter iter, iEnd;
	// 대전 룸
	iEnd = m_vRoomNode.end();
	for(iter = m_vRoomNode.begin();iter != iEnd;++iter)
	{
		Room *pRoom = *iter;
		
		pRoom->OnDestroy();
		m_MemNode.Push( pRoom );
	}	

	// 광장 룸
	iEnd = m_vPlazaNode.end();
	for(iter = m_vPlazaNode.begin();iter != iEnd;++iter)
	{
		Room *pRoom = *iter;

		pRoom->OnDestroy();
		m_MemNode.Push( pRoom );
	}

	// 본부 룸
	iEnd = m_vHeadquartersNode.end();
	for(iter = m_vHeadquartersNode.begin();iter != iEnd;++iter)
	{
		Room *pRoom = *iter;

		pRoom->OnDestroy();
		m_MemNode.Push( pRoom );
	}

	m_vRoomNode.clear();
	m_vPlazaNode.clear();
	m_vHeadquartersNode.clear();
	m_MemNode.DestroyPool();
}

bool RoomNodeManager::IsAfford()
{
	// 쾌적하기 위한 조건
	if(g_RoomNodeManager.RemainderNode() == 0) return false;
	return true;
}

ioHashString RoomNodeManager::GetPlazaRandomName()
{
	static ioHashString szName;

	int iMaxName = m_vPlazaNameList.size();
	if( iMaxName == 0 )
		return szName;

	int r = rand()%iMaxName;
	szName = m_vPlazaNameList[r];	
	return szName;
}

Room *RoomNodeManager::GetRoomNode( int iRoomIndex )
{
	vRoom_iter iter, iEnd;
	iEnd = m_vRoomNode.end();
	for( iter=m_vRoomNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pCursor = *iter;

		if( pCursor->GetRoomIndex() == iRoomIndex )
			return pCursor;
	}

	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pCursor = *iter;

		if( pCursor->GetRoomIndex() == iRoomIndex )
			return pCursor;
	}

	iEnd = m_vHeadquartersNode.end();
	for(iter = m_vHeadquartersNode.begin();iter != iEnd;++iter)
	{
		Room *pCursor = *iter;

		if( pCursor->GetRoomIndex() == iRoomIndex )
			return pCursor;
	}
	return NULL;
}

void RoomNodeManager::SendCurLadderRoomList( User *pUser, int iPage, int iMaxCount )
{
	if( pUser == NULL ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 ); // 0.1 초 이상 걸리면로그 남김
	
	static vSortRoom vRoomList;
	vRoomList.clear();

	if( !g_UserNodeManager.IsDeveloper( pUser->GetPublicID().c_str() ) )
		return;
	
	vRoom_iter iter, iEnd = m_vRoomNode.end();
	for( iter=m_vRoomNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pNode = *iter;
		if( NULL == pNode ) continue;
		if( pNode->GetRoomStyle() != RSTYLE_LADDERBATTLE )	continue;

		SortRoom sRoom;
		sRoom.m_pRoomParent = (RoomParent *)pNode;
		sRoom.m_iRoomPoint = GetSortLadderRoomPoint( sRoom );//방 평균 레벨
		vRoomList.push_back( sRoom );
	}
	
	//m_vRoomCopyNode 다른 서버에 룸정보
	vRoomCopyNode_iter iterCopy, iEndCopy = m_vRoomCopyNode.end();
	for( iterCopy = m_vRoomCopyNode.begin() ; iterCopy != iEndCopy ; ++iterCopy )
	{
		RoomCopyNode *pNode = *iterCopy;
		if( pNode == NULL ) continue;
		if( pNode->GetRoomStyle() != RSTYLE_LADDERBATTLE ) continue;

		SortRoom sRoom;
		sRoom.m_pRoomParent = (RoomParent *)pNode;
		sRoom.m_iRoomPoint = GetSortLadderRoomPoint( sRoom ); //방 평균 레벨
		vRoomList.push_back( sRoom );
	}

	int iMaxList = vRoomList.size();
	if( iMaxList == 0 )
	{
		SP2Packet kPacket( STPK_LADDERROOM_LIST );
		kPacket << 0 << 0;
		pUser->SendMessage( kPacket );
		return;
	}

	std::sort( vRoomList.begin(), vRoomList.end(), RoomInfoSort() );

	int iStartPos = iPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );
	SP2Packet kPacket( STPK_LADDERROOM_LIST );
	kPacket << iMaxList << iCurSize;
	for( int i = iStartPos ; i < iStartPos + iCurSize ; i++ )
	{
		SortRoom &kSortNode = vRoomList[i];
		RoomParent *pRoomNode = kSortNode.m_pRoomParent;
		if( pRoomNode )
		{
			kPacket << pRoomNode->GetRoomIndex() << 0 << "Ladder" << 
				pRoomNode->GetJoinUserCnt() << pRoomNode->GetPlayUserCnt()
				<< 1 << 1 << 4
				<< MT_HEROMATCH << false << 2 /*BRS_FULL_USER*/
				<< false << false;
			
		}
		else
			kPacket << 0 << 0 << ""
			<< 0 << 0
			<< 0 << 0 << 0
			<< 0 << false << 3 /*SortBattleRoom::BRS_TIME_CLOSE*/
			<< false;
	}

	pUser->SendMessage( kPacket );
	vRoomList.clear();
}


// 대전 룸
Room* RoomNodeManager::CreateNewRoom()
{
	Room *pRoom = (Room*)m_MemNode.Remove();
	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::CreateNewRoom MemPool Zero!" );
		return NULL;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "RoomNodeManager::CreateNewRoom : %d", pRoom->GetRoomIndex() );

	m_vRoomNode.push_back( pRoom );
	pRoom->OnCreate();

	return pRoom;
}

//광장 룸
Room *RoomNodeManager::CreateNewPlazaRoom( int iSubType, int iModeMapNum )
{
	Room *pRoom = (Room*)m_MemNode.Remove();
	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][plaza]MemPool zero" );
		return NULL;
	}

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][plaza]Create new plaza room : [%d] [%d] [%d]", pRoom->GetRoomIndex(), iSubType, iModeMapNum );

	pRoom->OnCreate();
	pRoom->SetRoomStyle( RSTYLE_PLAZA );
	pRoom->InitModeTypeList();
	pRoom->SelectNextMode( MT_TRAINING, iSubType, iModeMapNum );
	pRoom->SetModeType( MT_TRAINING, -1, -1 );
	pRoom->SetRoomName( GetPlazaRandomName() );
	pRoom->SetPlazaModeType( PT_COMMUNITY );
	pRoom->SetSubState( false );


	// 순서대로 룸넘버를 적용한다.
	bool bInsert = false;
	int iRoomNum = m_iPlazaRoomNumber;

	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for(iter = m_vPlazaNode.begin();iter != iEnd;++iter)
	{
		Room *pPlazaRoom = *iter;
		if( pPlazaRoom->GetRoomNumber() == iRoomNum )
		{
			iRoomNum++;
			continue;
		}
		else
		{
			bInsert = true;
			pRoom->SetRoomNum( iRoomNum );
			m_vPlazaNode.insert( iter, pRoom );
			break;
		}		
	}

	if( !bInsert )
	{
		pRoom->SetRoomNum( iRoomNum );
		m_vPlazaNode.push_back( pRoom );
	}

#ifdef EXCAVATION_EX_BYBCKIM
	// nobody 
#else
	g_ExcavationMgr.CreateExcavation( pRoom->GetRoomIndex() );
#endif
	
	return pRoom;
}

Room *RoomNodeManager::CreateNewHeadquartersRoom( int iSubType /* = -1 */, int iModeMapNum /* = -1  */ )
{
	Room *pRoom = (Room*)m_MemNode.Remove();
	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::CreateNewHeadquartersRoom MemPool Zero!" );
		return NULL;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "RoomNodeManager::CreateNewHeadquartersRoom : %d - %d - %d", pRoom->GetRoomIndex(), iSubType, iModeMapNum );

	m_vHeadquartersNode.push_back( pRoom );
	pRoom->OnCreate();

	pRoom->SetRoomStyle( RSTYLE_HEADQUARTERS );
	pRoom->InitModeTypeList(MT_HEADQUARTERS);
	pRoom->SelectNextMode( MT_HEADQUARTERS, iSubType, iModeMapNum );
	pRoom->SetModeType( MT_HEADQUARTERS, -1, -1 );

	return pRoom;
}

void RoomNodeManager::RemoveRoom( Room *pRoom )
{
	vRoom_iter iter, iEnd;
	iEnd = m_vRoomNode.end();
	for( iter=m_vRoomNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pCursor = *iter;

		if( pCursor == pRoom )
		{
			pCursor->OnDestroy();
			m_MemNode.Push( pCursor );

			m_vRoomNode.erase( iter );
			return;
		}
	}
}

void RoomNodeManager::RemovePlazaRoom( Room *pRoom )
{
	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pCursor = *iter;

		if( pCursor == pRoom )
		{
			pCursor->OnDestroy();
#ifdef EXCAVATION_EX_BYBCKIM
			// nobody 
#else
			g_ExcavationMgr.DeleteExcavation( pCursor->GetRoomIndex() );
#endif
			m_MemNode.Push( pCursor );

			m_vPlazaNode.erase( iter );
			return;
		}
	}
}

void RoomNodeManager::RemoveHeadquartersRoom( Room *pRoom )
{
	vRoom_iter iter, iEnd;
	iEnd = m_vHeadquartersNode.end();
	for( iter=m_vHeadquartersNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pCursor = *iter;

		if( pCursor == pRoom )
		{
			if( pRoom->GetModeType() == MT_HOUSE )
			{
				Mode* pMode	= pRoom->GetModeInfo();
				if( pMode )
				{
					HouseMode* pInfo	= static_cast<HouseMode*>(pMode);
					DWORD dwMasterIndex	= pInfo->GetMasterIndex();
					g_PersonalRoomBlockMgr.DeletePersonalRoom(dwMasterIndex);
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[info][personalroom]Delete personal room : [%d] [%d]", pInfo->GetMasterIndex(), pRoom->GetRoomIndex());
				}
			}

			pCursor->OnDestroy();
			m_MemNode.Push( pCursor );

			m_vHeadquartersNode.erase( iter );
			return;
		}
	}
}

void RoomNodeManager::AddCopyRoom( RoomCopyNode *pRoom )
{
	if( pRoom == NULL ) return;

	if( pRoom->GetModeType() == MT_TRAINING )
	{
		vRoomCopyNode_iter iter,iEnd;
		iEnd = m_vPlazaCopyNode.end();
		for( iter=m_vPlazaCopyNode.begin() ; iter!=iEnd ; ++iter )
		{
			RoomCopyNode *pCursor = *iter;
			if( pCursor == pRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::AddCopyRoom 이미 리스트에 있는 복사본임 : 광장 (%d)", pCursor->GetRoomIndex() );
				return;
			}
		}
		m_vPlazaCopyNode.push_back( pRoom );
	}
	else if( pRoom->GetModeType() == MT_HEADQUARTERS || pRoom->GetModeType() == MT_HOUSE )
	{
		vRoomCopyNode_iter iter,iEnd;
		iEnd = m_vHeadquartersCopyNode.end();
		for( iter=m_vHeadquartersCopyNode.begin() ; iter!=iEnd ; ++iter )
		{
			RoomCopyNode *pCursor = *iter;
			if( pCursor == pRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::AddCopyRoom 이미 리스트에 있는 복사본임 : 본부 (%d)", pCursor->GetRoomIndex() );
				return;
			}
		}
		m_vHeadquartersCopyNode.push_back( pRoom );
	}
	else
		m_vRoomCopyNode.push_back( pRoom );
}

void RoomNodeManager::RemoveCopyRoom( RoomCopyNode *pRoom )
{
	if( pRoom == NULL ) return;

	if( pRoom->GetModeType() == MT_TRAINING )
	{
		vRoomCopyNode_iter iter,iEnd;
		iEnd = m_vPlazaCopyNode.end();
		for( iter=m_vPlazaCopyNode.begin() ; iter!=iEnd ; ++iter )
		{
			RoomCopyNode *pCursor = *iter;

			if( pCursor == pRoom )
			{
				m_vPlazaCopyNode.erase( iter );
				return;
			}
		}
	}
	else if( pRoom->GetModeType() == MT_HEADQUARTERS || pRoom->GetModeType() == MT_HOUSE )
	{
		vRoomCopyNode_iter iter,iEnd;
		iEnd = m_vHeadquartersCopyNode.end();
		for( iter=m_vHeadquartersCopyNode.begin() ; iter!=iEnd ; ++iter )
		{
			RoomCopyNode *pCursor = *iter;

			if( pCursor == pRoom )
			{
				m_vHeadquartersCopyNode.erase( iter );
				return;
			}
		}
	}
	else
	{
		vRoomCopyNode_iter iter,iEnd;
		iEnd = m_vRoomCopyNode.end();
		for( iter=m_vRoomCopyNode.begin() ; iter!=iEnd ; ++iter )
		{
			RoomCopyNode *pCursor = *iter;

			if( pCursor == pRoom )
			{
				m_vRoomCopyNode.erase( iter );
				return;
			}
		}
	}
}

void RoomNodeManager::ConnectServerNodeSync( ServerNode *pServerNode )
{
	if( pServerNode == NULL ) return;

	static vRoom vRoomList;
	vRoomList.clear();
	
	// 룸
	LOOP_GUARD();
	vRoom_iter iter = m_vRoomNode.begin();
	while( iter != m_vRoomNode.end() )
	{
		Room *pRoom = *iter++;
		if( pRoom->GetRoomStyle() == RSTYLE_NONE ) continue;
		if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue;

		vRoomList.push_back( pRoom );
	}
	LOOP_GUARD_CLEAR();

	//광장
	LOOP_GUARD();
	iter = m_vPlazaNode.begin(); 
	while( iter != m_vPlazaNode.end() )
	{
		Room *pRoom = *iter++;
		if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue;

		vRoomList.push_back( pRoom );
	}
	LOOP_GUARD_CLEAR();
	//본부
	LOOP_GUARD();
	iter = m_vHeadquartersNode.begin();
	while( iter != m_vHeadquartersNode.end() )
	{
		Room *pRoom = *iter++;
		if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue;

		vRoomList.push_back( pRoom );
	}
	LOOP_GUARD_CLEAR();

	// 오리지날 룸 정보만 N개씩 끊어서 전송
	LOOP_GUARD();
	while( true )
	{
		int iMaxSize = min( SSTPK_CONNECT_SYNC_ROOM_MAX, (int)vRoomList.size() );
		if( iMaxSize == 0 )
			break;

		SP2Packet kPacket( SSTPK_CONNECT_SYNC );
		kPacket << SSTPK_CONNECT_SYNC_ROOM << iMaxSize;
		for(int i = 0;i < iMaxSize;i++)
		{
			Room *pRoom  = vRoomList[0];
			pRoom->FillSyncCreate( kPacket );
			vRoomList.erase( vRoomList.begin() );
		}
		pServerNode->SendMessage( kPacket );
	}
	LOOP_GUARD_CLEAR();
}

// 광장 
int RoomNodeManager::GetSortPlazaRoomPoint( SortPlazaRoom &rkSortRoom, int iKillDeathLevel )
{
	if( rkSortRoom.m_pNode == NULL )
		return PLAZA_ROOM_SORT_HALF_POINT;

	int iReturnPoint = 0;
	if( rkSortRoom.m_iState != SortPlazaRoom::PRS_ACTIVE )         // 입장 불가능한 방
		iReturnPoint = PLAZA_ROOM_SORT_HALF_POINT;

	// 현재 인원이 가장 많은 방 우선 순위   
	iReturnPoint += ( MAX_PLAYER - rkSortRoom.m_pNode->GetPlayUserCnt() ) * 100000;

	// 방 최대 인원이 많은 방 우선 순위
	iReturnPoint += ( MAX_PLAYER - rkSortRoom.m_pNode->GetMaxPlayer() ) * 100;

	// 실력차이가 적은 방 우선 순위
	iReturnPoint += abs( rkSortRoom.m_pNode->GetAverageLevel() - iKillDeathLevel );

	return iReturnPoint;
}

Room* RoomNodeManager::GetAutoCreatePlazaNode( int iMyLevel )
{
	Room *pReturnRoom = NULL;
	int	  iGapPoint   = 99999999;
	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();

	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue;
		if( pRoom->GetPlazaModeType() != PT_COMMUNITY ) continue;
		if( pRoom->IsRoomPW() ) continue;
		if( pRoom->IsRoomMasterID() ) continue;
		if( pRoom->IsRoomFull() ) continue;
		if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoom->GetAverageLevel(), iMyLevel, JOIN_CHECK_MINMAX_LEVEL ) ) continue;

		int iGapRate = pRoom->GetGapLevel( iMyLevel );				
		if( iGapRate < iGapPoint )     // 가장 가까운 레벨 룸 .
		{
			iGapPoint   = iGapRate;
			pReturnRoom = pRoom;
		}		
	}

	return pReturnRoom;
}

Room* RoomNodeManager::GetExitRoomJoinPlazaNode( int iMyLevel )
{
	Room *pReturnRoom = NULL;
	int	  iGapPoint   = 99999999;
	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue;
		if( pRoom->IsRoomMasterID() ) continue;
		if( pRoom->IsRoomFull() ) continue;
		if( pRoom->GetPlazaModeType() != PT_COMMUNITY ) continue;
		if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoom->GetAverageLevel(), iMyLevel, JOIN_CHECK_MINMAX_LEVEL ) ) continue;

		int iGapRate = pRoom->GetGapLevel( iMyLevel );				
		if( iGapRate < iGapPoint )     // 가장 가까운 레벨 룸 .
		{
			iGapPoint   = iGapRate;
			pReturnRoom = pRoom;
		}		
	}

	// 입장 가능한 룸이 없으면 공개 룸 생성.
	if( !pReturnRoom )     
		pReturnRoom = CreateNewPlazaRoom();
	return pReturnRoom;
}

Room* RoomNodeManager::GetJoinPlazaNode( int iMyLevel, Room *pMyRoom )
{	
	Room *pReturnRoom = NULL;
	int   iGapPoint   = 99999999;

	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( pRoom == pMyRoom ) continue;
		if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue;
		if( pRoom->IsRoomPW() ) continue;
		if( pRoom->IsRoomFull() ) continue;
		if( pRoom->GetPlazaModeType() != PT_COMMUNITY ) continue;
		if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoom->GetAverageLevel(), iMyLevel, JOIN_CHECK_MINMAX_LEVEL ) ) continue;

		int iGapRate = pRoom->GetGapLevel( iMyLevel );				
		if( iGapRate < iGapPoint )     // 가장 가까운 레벨 룸 .
		{
			iGapPoint   = iGapRate;
			pReturnRoom = pRoom;
		}		
	}
	return pReturnRoom;
}

Room* RoomNodeManager::GetJoinPlazaNodeByNum( int iRoomIndex )
{
	// 예외 처리(풀방/비번 등등)는 외부에서 한다.
	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( !pRoom )
			continue;

		if( pRoom->GetPlazaModeType() != PT_GUILD )
			if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue;

		if( pRoom->GetRoomIndex() == iRoomIndex )
		{
			return pRoom;
		}
	}
	return NULL;
}

RoomCopyNode* RoomNodeManager::GetJoinPlazaCopyNode( int iMyLevel )
{
	RoomCopyNode *pReturnRoom = NULL;
	int   iGapPoint   = 99999999;

	vRoomCopyNode_iter iter, iEnd;
	iEnd = m_vPlazaCopyNode.end();
	for( iter=m_vPlazaCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		RoomCopyNode *pRoom = *iter;
		if( pRoom->IsServerUserFull() ) continue;            //한서버 유저 인원에 대한 자동 참여 제한
		if( pRoom->IsRoomPW() ) continue;
		if( pRoom->IsRoomFull() ) continue;
		if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoom->GetAverageLevel(), iMyLevel, JOIN_CHECK_MINMAX_LEVEL ) ) continue;

		int iGapRate = pRoom->GetGapLevel( iMyLevel );				
		if( iGapRate < iGapPoint )     // 가장 가까운 레벨 룸 .
		{
			iGapPoint   = iGapRate;
			pReturnRoom = pRoom;
		}		
	}
	return pReturnRoom;
}

RoomCopyNode* RoomNodeManager::GetJoinPlazaCopyNodeByNum( int iRoomIndex )
{
	// 예외 처리(풀방/비번 등등)는 외부에서 한다.
	vRoomCopyNode_iter iter, iEnd;
	iEnd = m_vPlazaCopyNode.end();
	for( iter=m_vPlazaCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		RoomCopyNode *pRoom = *iter;
		if( pRoom->GetRoomIndex() == iRoomIndex )
		{
			return pRoom;
		}
	}
	return NULL;
}

RoomParent* RoomNodeManager::GetJoinGlobalPlazaNode( int iMyLevel, RoomParent *pMyRoom  )
{
	vRoomParent vPlazaList;
	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue;

		vPlazaList.push_back( (RoomParent*)pRoom );
	}

	vRoomCopyNode_iter iter_copy,iEnd_copy;
	iEnd_copy = m_vPlazaCopyNode.end();
	for( iter_copy=m_vPlazaCopyNode.begin() ; iter_copy!=iEnd_copy ; ++iter_copy )
	{
		RoomCopyNode *pRoom = *iter_copy;
		if( pRoom->IsServerUserFull() ) continue;            //한서버 유저 인원에 대한 자동 참여 제한

		vPlazaList.push_back( (RoomParent*)pRoom );
	}
	//////////////////////////////////////////////////////////////////////////
	RoomParent *pReturnRoom = NULL;
	int iGapPoint           = 99999999;
    
	vRoomParent_iter iterParent, iEndParent;
	iEndParent = vPlazaList.end();
	for( iterParent=vPlazaList.begin() ; iterParent!=iEndParent ; ++iterParent )
	{
		RoomParent *pRoomParent = *iterParent;
		if( pRoomParent == pMyRoom ) continue;
		if( pRoomParent->IsRoomEmpty() ) continue;
		if( pRoomParent->IsRoomPW() ) continue;
		if( pRoomParent->IsRoomFull() ) continue;
		if( pRoomParent->GetPlazaModeType() != PT_COMMUNITY ) continue;
		if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoomParent->GetAverageLevel(), iMyLevel, JOIN_CHECK_MINMAX_LEVEL ) ) continue;

		int iGapRate = pRoomParent->GetGapLevel( iMyLevel );				
		if( iGapRate < iGapPoint )     // 가장 가까운 레벨 룸 .
		{
			iGapPoint   = iGapRate;
			pReturnRoom = pRoomParent;
		}		
	}
	return pReturnRoom;
}

RoomParent* RoomNodeManager::GetJoinGlobalPlazaNodeByNum( int iRoomIndex )
{
	Room *pRoom = GetJoinPlazaNodeByNum( iRoomIndex );
	if( pRoom )
		return (RoomParent*)pRoom;

	RoomCopyNode *pCopyNode = GetJoinPlazaCopyNodeByNum( iRoomIndex );
	if( pCopyNode )
		return (RoomParent*)pCopyNode;
	return NULL;
}

RoomParent* RoomNodeManager::GetPlazaNodeByNum( int iRoomIndex )
{
	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( pRoom->IsRoomEmpty() ) continue;
		if( pRoom->GetRoomIndex() == iRoomIndex )
		{
			return pRoom;
		}
	}

	return NULL;
}

RoomParent* RoomNodeManager::GetGlobalPlazaNodeByNum( int iRoomIndex )
{
	// 오리지널
	RoomParent *pRoom = GetPlazaNodeByNum( iRoomIndex );
	if( pRoom )
	{
		return pRoom;
	}

	// Copy
	vRoomCopyNode_iter iter, iEnd;
	iEnd = m_vPlazaCopyNode.end();
	for( iter=m_vPlazaCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		RoomCopyNode *pCopyRoom = *iter;
		if( pCopyRoom->GetRoomIndex() == iRoomIndex )
		{
			return pCopyRoom;
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::GetGlobalPlazaNodeByNum() - Not Find Room: %d", iRoomIndex );
	return NULL;
}

RoomParent *RoomNodeManager::GetExitRoomJoinGlobalPlazaNode( int iMyLevel, RoomParent *pMyRoom )
{
	vRoomParent vPlazaList;
	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue;

		vPlazaList.push_back( (RoomParent*)pRoom );
	}

	vRoomCopyNode_iter iter_copy,iEnd_copy;
	iEnd_copy = m_vPlazaCopyNode.end();
	for( iter_copy=m_vPlazaCopyNode.begin() ; iter_copy!=iEnd_copy ; ++iter_copy )
	{
		RoomCopyNode *pRoom = *iter_copy;
		if( pRoom->IsServerUserFull() ) continue;            //한서버 유저 인원에 대한 자동 참여 제한

		vPlazaList.push_back( (RoomParent*)pRoom );
	}
	//////////////////////////////////////////////////////////////////////////

	RoomParent *pReturnRoom = NULL;
	int			iGapPoint   = 99999999;

	vRoomParent_iter iterParent, iEndParent;
	iEndParent = vPlazaList.end();
	for( iterParent=vPlazaList.begin() ; iterParent!=iEndParent ; ++iterParent )
	{
		RoomParent *pRoomParent = *iterParent;
		if( pRoomParent == pMyRoom ) continue;
		if( pRoomParent->IsRoomEmpty() ) continue;
		if( pRoomParent->IsRoomMasterID() ) continue;
		if( pRoomParent->IsRoomFull() ) continue;
		if( pRoomParent->GetPlazaModeType() != PT_COMMUNITY ) continue;
		if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoomParent->GetAverageLevel(), iMyLevel, JOIN_CHECK_MINMAX_LEVEL ) ) continue;

		int iGapRate = pRoomParent->GetGapLevel( iMyLevel );				
		if( iGapRate < iGapPoint )     // 가장 가까운 레벨 룸 .
		{
			iGapPoint   = iGapRate;
			pReturnRoom = pRoomParent;
		}		
	}

	// 입장 가능한 룸이 없으면 공개 룸 생성.
	if( !pReturnRoom )     
		pReturnRoom = CreateNewPlazaRoom();
	return pReturnRoom;
}

Room *RoomNodeManager::GetHeadquartersNode( int iRoomIndex )
{
	vRoom_iter iter, iEnd;
	iEnd = m_vHeadquartersNode.end();
	for( iter=m_vHeadquartersNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( pRoom->GetRoomStyle() != RSTYLE_HEADQUARTERS ) continue;
		if( pRoom->GetModeType() == MT_HEADQUARTERS )
		{
			if( pRoom->IsRoomEmpty() ) continue;
		}
		
		if( pRoom->GetRoomIndex() == iRoomIndex ) 
			return pRoom;
	}
	return NULL;
}

RoomParent* RoomNodeManager::GetHeadquartersGlobalNode( int iRoomIndex )
{
	vRoom_iter iter, iEnd;
	iEnd = m_vHeadquartersNode.end();
	for( iter=m_vHeadquartersNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( pRoom->GetRoomStyle() != RSTYLE_HEADQUARTERS ) continue;
		if( pRoom->IsRoomEmpty() ) continue;

		if( pRoom->GetRoomIndex() == iRoomIndex ) 
			return pRoom;
	}
	
	vRoomCopyNode_iter iter_copy,iEnd_copy;
	iEnd_copy = m_vHeadquartersCopyNode.end();
	for( iter_copy=m_vHeadquartersCopyNode.begin() ; iter_copy!=iEnd_copy ; ++iter_copy )
	{
		RoomCopyNode *pRoom = *iter_copy;
		if( pRoom == NULL ) continue;
		if( pRoom->GetRoomStyle() != RSTYLE_HEADQUARTERS ) continue;
		if( pRoom->GetRoomIndex() == iRoomIndex )
			return pRoom;
	}

	return NULL;
}

void RoomNodeManager::SendPlazaRoomJoinInfo( UserParent *pUserParent, DWORD dwRoomIndex )
{
	if( !pUserParent ) return;

	RoomParent *pNode = GetGlobalPlazaNodeByNum( dwRoomIndex );
	if( pNode )
		pNode->OnPlazaRoomInfo( pUserParent );
}

void RoomNodeManager::SendPlazaRoomList( User *pUser, int iCurPage, int iMaxCount )
{
	if( pUser == NULL ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	int iMyLevel = pUser->GetKillDeathLevel();
	static vSortPlazaRoom vPlazaList;
	vPlazaList.clear();

	if(!IsBlocked())
	{
		vRoom_iter iter, iEnd;
		iEnd = m_vPlazaNode.end();
		for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
		{
			Room *pRoom = *iter;
			if( pRoom->IsRoomEmpty() || pRoom->IsRoomEnterUserDelay() ) continue; //kyg 약간 비효율적인 루틴 IsRoomEnterUSerDelay를 두번 호출함 
			if( pRoom->GetPlazaModeType() == PT_GUILD ) continue;

			SortPlazaRoom spr;
			spr.m_pNode = (RoomParent*)pRoom;
			spr.m_iState = SortPlazaRoom::PRS_ACTIVE;
			spr.m_iSubState = pRoom->GetSubState();

			if( !pUser->GetMyRoom() || pUser->GetMyRoom() != pRoom )
			{
				if( pRoom->GetPlayUserCnt() == pRoom->GetMaxPlayer() )
					spr.m_iState = SortPlazaRoom::PRS_FULL_USER;
				else if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoom->GetAverageLevel(), iMyLevel, JOIN_CHECK_MIN_LEVEL ) )
					spr.m_iState = SortPlazaRoom::PRS_NOT_MIN_LEVEL_MATCH;
				else if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoom->GetAverageLevel(), iMyLevel, JOIN_CHECK_MAX_LEVEL ) )
					spr.m_iState = SortPlazaRoom::PRS_NOT_MAX_LEVEL_MATCH;
			}

			spr.m_iPoint += GetSortPlazaRoomPoint( spr, iMyLevel );

			vPlazaList.push_back( spr );
		}
	}
	

	vRoomCopyNode_iter iter_copy,iEnd_copy;
	iEnd_copy = m_vPlazaCopyNode.end();
	for( iter_copy=m_vPlazaCopyNode.begin() ; iter_copy!=iEnd_copy ; ++iter_copy )
	{
		RoomCopyNode *pRoom = *iter_copy;
		if(pRoom->IsBlocked()) continue;
		if( pRoom->GetPlazaModeType() == PT_GUILD ) continue;

		SortPlazaRoom spr;
		spr.m_pNode = (RoomParent*)pRoom;
		spr.m_iState = SortPlazaRoom::PRS_ACTIVE;
		spr.m_iSubState = pRoom->GetSubState();

		if( pRoom->GetPlayUserCnt() == pRoom->GetMaxPlayer() )
			spr.m_iState = SortPlazaRoom::PRS_FULL_USER;
		else if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoom->GetAverageLevel(), iMyLevel, JOIN_CHECK_MIN_LEVEL ) )
			spr.m_iState = SortPlazaRoom::PRS_NOT_MIN_LEVEL_MATCH;
		else if( !g_LevelMatchMgr.IsPlazaLevelJoin( pRoom->GetAverageLevel(), iMyLevel, JOIN_CHECK_MAX_LEVEL ) )
			spr.m_iState = SortPlazaRoom::PRS_NOT_MAX_LEVEL_MATCH;

		spr.m_iPoint += GetSortPlazaRoomPoint( spr, iMyLevel );

		vPlazaList.push_back( spr );
	}

	int iMaxList = vPlazaList.size();
	if( iMaxList == 0 )
	{		
		//광장 없음 전송
		SP2Packet kPacket( STPK_PLAZA_ROOM_LIST );

		PACKET_GUARD_VOID( kPacket.Write(0) );
		PACKET_GUARD_VOID( kPacket.Write(0) );

		pUser->SendMessage( kPacket );
		return;
	}

	std::sort( vPlazaList.begin(), vPlazaList.end(), PlazaRoomSort() );

	int iStartPos = iCurPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

	SP2Packet kPacket( STPK_PLAZA_ROOM_LIST );

	PACKET_GUARD_VOID( kPacket.Write(iMaxList) );
	PACKET_GUARD_VOID( kPacket.Write(iCurSize) );

	for(int i = iStartPos; i < iStartPos + iCurSize;i++)
	{
		SortPlazaRoom &kSortPlazaRoom = vPlazaList[i];
		RoomParent *pRoom = kSortPlazaRoom.m_pNode;	
		if( pRoom )
		{					    
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetRoomIndex()) );
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetRoomNumber()) );
			PACKET_GUARD_VOID( kPacket.Write(kSortPlazaRoom.m_iState) );
			PACKET_GUARD_VOID( kPacket.Write(kSortPlazaRoom.m_iSubState) );
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetRoomName()) );
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetMasterName()) );
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetModeSubNum()) );
			PACKET_GUARD_VOID( kPacket.Write(pRoom->IsRoomPW()) );
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetMaxPlayer()) ); 
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetPlazaRoomLevel()) );
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetJoinUserCnt()) ); 
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetPlayUserCnt()) );
			PACKET_GUARD_VOID( kPacket.Write(pRoom->GetMasterLevel()) );
			PACKET_GUARD_VOID( kPacket.Write((int)pRoom->GetPlazaModeType()) );

			if( pRoom->GetMasterName().IsEmpty() )
			{
				PACKET_GUARD_VOID( kPacket.Write(0) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
			}
			else
			{
				UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( pRoom->GetMasterName() );
				if( pUserParent )
				{
					PACKET_GUARD_VOID( kPacket.Write(pUserParent->GetGuildIndex()) );
					PACKET_GUARD_VOID( kPacket.Write(pUserParent->GetGuildMark()) );
				}
				else
				{
					PACKET_GUARD_VOID( kPacket.Write(0) );
					PACKET_GUARD_VOID( kPacket.Write(0) );
				}
			}
		}		
		else    //예외
		{
			PACKET_GUARD_VOID( kPacket.Write(0) ); 
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write("") );
			PACKET_GUARD_VOID( kPacket.Write("") );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(true) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
		}
	}	
	pUser->SendMessage( kPacket );

	vPlazaList.clear();
}

int RoomNodeManager::GetSafetySurvivalRoomUserCount()
{
	int iCount = 0;
	vRoom_iter iter, iEnd;
	iEnd = m_vRoomNode.end();
	for( iter=m_vRoomNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		if( !pRoom->IsSafetyLevelRoom() ) continue;
		if( pRoom->GetModeType() != MT_TEAM_SURVIVAL ) continue;

		iCount += pRoom->GetJoinUserCnt();
	}
	return iCount;
}

int RoomNodeManager::GetPlazaUserCount()
{
	int iCount = 0;
	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pRoom = *iter;
		iCount += pRoom->GetJoinUserCnt();
	}
	return iCount;
}

int RoomNodeManager::GetHeadquartersUserCount()
{
	int iCount = 0;
	vRoom_iter iter, iEnd;
	iEnd = m_vHeadquartersNode.end();
	for(iter = m_vHeadquartersNode.begin();iter != iEnd;++iter)
	{
		Room *pRoom = *iter;
		iCount += pRoom->GetJoinUserCnt();
	}
	return iCount;
}

void RoomNodeManager::CreateMatchingTable()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김
	g_LevelMatchMgr.InitPlazaLevelMatch();

	static vSortRoom vRoomList;
	vRoomList.clear();

	//Original Room
	vRoom_iter iter, iEnd;
	iEnd = m_vPlazaNode.end();
	for(iter=m_vPlazaNode.begin();iter != iEnd;++iter)
	{
		Room *pRoom = *iter;
		if( pRoom->GetRoomStyle() != RSTYLE_PLAZA ) continue;

		SortRoom kRoom;
		kRoom.m_pRoomParent	= (RoomParent*)pRoom;
		kRoom.m_iRoomPoint  = pRoom->GetAverageLevel();
		vRoomList.push_back( kRoom );
	}

	//Copy Room
	vRoomCopyNode_iter iterCopy, iEndCopy;
	iEndCopy = m_vPlazaCopyNode.end();
	for(iterCopy = m_vPlazaCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		RoomCopyNode *pRoom = *iterCopy;
		if( pRoom->GetRoomStyle() != RSTYLE_PLAZA ) continue;

		SortRoom kRoom;
		kRoom.m_pRoomParent	= (RoomParent*)pRoom;
		kRoom.m_iRoomPoint  = pRoom->GetAverageLevel();
		vRoomList.push_back( kRoom );
	}

	if( vRoomList.empty() )
		return;

	std::sort( vRoomList.begin(), vRoomList.end(), RoomInfoSort() );

	int iMaxList = vRoomList.size();
	int i = 0;
	int iLimitCount = max( 1, (float)iMaxList * g_LevelMatchMgr.GetPlazaEnterLimitCount() );
	for(i = 0;i < g_LevelMatchMgr.GetRoomEnterLevelMax();i++)
	{		
		int rIndex = 0;
		for(;rIndex < iMaxList;rIndex++)
		{
			SortRoom &kSortRoom = vRoomList[rIndex];
			if( kSortRoom.m_iRoomPoint >= i )
				break;
		}

		int iMaxRemain = max( 0, ( rIndex + iLimitCount ) - iMaxList );
		int iMinRemain = max( 0, iLimitCount - rIndex );
		int iLowIndex  = max( rIndex - iLimitCount - iMaxRemain, 0 );
		int iHighIndex = min( iLimitCount + iMinRemain + rIndex, iMaxList );

		//자신있지만 벡터의 사이즈를 넘는 인덱스가 혹시 나오면 뻗을까봐....-_-;
		if( iLowIndex < 0 )
			iLowIndex = 0;
		else if( iLowIndex >= iMaxList )
			iLowIndex = iMaxList - 1;

		if( iHighIndex < 0 )
			iHighIndex = 0;
		else if( iHighIndex > iMaxList )
			iHighIndex = iMaxList;

		SortRoom &kLowRoom = vRoomList[iLowIndex];
		SortRoom &kHighRoom = vRoomList[iHighIndex - 1];
		g_LevelMatchMgr.InsertPlazaLevelMatch( min( i, kLowRoom.m_iRoomPoint ), max( i, kHighRoom.m_iRoomPoint ) );
	}	
}

void RoomNodeManager::NagleOptionChange( bool bNagleAlgorithm, bool bPlazaNagleAlgorithm )
{
	if( Help::IsNagleAlgorithm() == bNagleAlgorithm &&
		Help::IsPlazaNagleAlgorithm() == bPlazaNagleAlgorithm )
	{
		return;    // 옵션이 변경되지 않았음
	}

	// 대전 룸
	if( Help::IsNagleAlgorithm() != bNagleAlgorithm )
	{
		vRoom_iter iter;
		for(iter = m_vRoomNode.begin(); iter != m_vRoomNode.end();++iter)
		{
			(*iter)->SetNagleAlgorithm( !bNagleAlgorithm );
		}
	}

	// 광장 룸
	if( Help::IsPlazaNagleAlgorithm() != bPlazaNagleAlgorithm )
	{
		vRoom_iter iter;
		for(iter = m_vPlazaNode.begin();iter != m_vPlazaNode.end();++iter)
		{
			(*iter)->SetNagleAlgorithm( !bPlazaNagleAlgorithm );
		}
	}

	// 본부 룸
	if( Help::IsPlazaNagleAlgorithm() != bPlazaNagleAlgorithm )
	{
		vRoom_iter iter;
		for(iter = m_vHeadquartersNode.begin();iter != m_vHeadquartersNode.end();++iter)
		{
			(*iter)->SetNagleAlgorithm( !bPlazaNagleAlgorithm );
		}
	}
}

void RoomNodeManager::RoomProcess()
{
	DWORD dwCurTime = TIMEGETTIME();
	if( m_dwRoomProcessTime + 1000 > dwCurTime )
		return;

	m_dwRoomProcessTime = dwCurTime;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	// 대전 룸
	vRoom_iter iter, iEnd;
	static vRoom vRemoveRoomList;
	vRemoveRoomList.clear();

	iEnd = m_vRoomNode.end();
	for( iter=m_vRoomNode.begin() ; iter!=iEnd ; ++iter )
	{
		(*iter)->RoomProcess();
		
		if( (*iter)->IsRoomEmpty() )
			vRemoveRoomList.push_back( *iter );
	}
	// 대전 룸 삭제
	iEnd = vRemoveRoomList.end();
	for( iter=vRemoveRoomList.begin() ; iter!=iEnd ; ++iter )
	{
		RemoveRoom( *iter );
	}
	vRemoveRoomList.clear();

	// 광장 룸
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		(*iter)->RoomProcess();

		if( (*iter)->IsRoomEmpty() && (*iter)->GetPlazaModeType() != PT_GUILD )
			vRemoveRoomList.push_back( *iter );
	}

	// 길드 룸(광장 룸) 관련 삭제 처리.
	DWORD dwGuildIndex	= 0;
	iEnd = m_vPlazaNode.end();
	for( iter=m_vPlazaNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room* pRoom = *iter;
		if( !pRoom )
			continue;

		if( pRoom->GetPlazaModeType() != PT_GUILD )
			continue;

		if( !pRoom->PassCreateGuildRoomDelayTime() )
			continue;

		if( !pRoom->IsRoomEmpty() )
			continue;

		if( g_GuildRoomBlockMgr.IsDeleteGuildRoom(pRoom->GetRoomIndex(), dwGuildIndex) )
		{
			g_GuildRoomBlockMgr.DeleteGuildRoom(dwGuildIndex);
			vRemoveRoomList.push_back(pRoom);
		}
	}

	// 광장 룸 삭제
	iEnd = vRemoveRoomList.end();
	for( iter=vRemoveRoomList.begin() ; iter!=iEnd ; ++iter )
	{
		RemovePlazaRoom( *iter );
	}
	vRemoveRoomList.clear();

	// 본부 룸
	iEnd = m_vHeadquartersNode.end();
	for( iter=m_vHeadquartersNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room* pRoom = *iter;
		if( !pRoom )
			continue;

		pRoom->RoomProcess();

		if( pRoom->IsRoomEmpty() )
		{
			if( pRoom->GetModeType() == MT_HOUSE )
			{
				Mode* pMode	= pRoom->GetModeInfo();
				if( pMode )
				{
					HouseMode* pHouse	= static_cast<HouseMode*>(pMode);
					if( !pHouse->IsDeleteTime() )
						continue;
				}
			}

			vRemoveRoomList.push_back( *iter );
		}
	}

	// 본부 룸 삭제
	iEnd = vRemoveRoomList.end();
	for( iter=vRemoveRoomList.begin() ; iter!=iEnd ; ++iter )
	{
		RemoveHeadquartersRoom( *iter );
	}
	vRemoveRoomList.clear();

}

DWORD RoomNodeManager::GetGuildRoomLifeTime()
{
	return m_dwGuildRoomLifeTime;
}

void RoomNodeManager::INILoadToGuildRoomLifeTime()
{
	ioINILoader kLoader( "config/sp2_plaza.ini" );
	kLoader.SetTitle( "common" );
	m_dwGuildRoomLifeTime = kLoader.LoadInt( "guild_life_time", 0 ) * 1000;	//*1000은 1초가 1000이기 떄문에....
}

Room *RoomNodeManager::CreateNewPersonalHQRoom( int iSubType /* = -1 */, int iModeMapNum /* = -1  */ )
{
	Room *pRoom = (Room*)m_MemNode.Remove();
	if( !pRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::CreateNewPersonalHQRoom MemPool Zero!" );
		return NULL;
	}

	m_vHeadquartersNode.push_back( pRoom );
	pRoom->OnCreate();

	pRoom->SetRoomStyle( RSTYLE_HEADQUARTERS );
	pRoom->InitModeTypeList(MT_HOUSE);
	pRoom->SelectNextMode( MT_HOUSE, iSubType, iModeMapNum );
	pRoom->SetModeType( MT_HOUSE, -1, -1 );

	return pRoom;
}

int RoomNodeManager::GetSortLadderRoomPoint( SortRoom& rkSortRoom )
{
	enum { MAX_ROOM_POINT = 100000000 };
	if( NULL == rkSortRoom.m_pRoomParent )
		return MAX_ROOM_POINT;

	return rkSortRoom.m_pRoomParent->GetAverageLevel();
}

void RoomNodeManager::SendLadderRoomJoinInfo( UserParent *pUserParent, DWORD dwIndex, int iPrevBattleIndex )
{
	if( !pUserParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::SendLadderRoomJoinInfo Null User Pointer!!(%d)", dwIndex );
		return;
	}
	Room *pNode = GetGlobalLadderRoomNode( dwIndex );
	if( pNode )
	{
		SP2Packet kPacket( STPK_LADDERROOM_JOIN_INFO ); //레더룸 정보 채워주자..
		FillLadderRoomInfo( kPacket , *pNode );
		pNode->FillUserList( kPacket , true);

		// 모드 정보
		enum{ BST_START = 0, BST_PLAYING = 1, BST_RESULT = 2, };
		PACKET_GUARD_VOID( kPacket.Write(BMT_SURVIVAL) );
		PACKET_GUARD_VOID( kPacket.Write(MT_HEROMATCH) );
		if( pNode->IsRoomProcess() )
		{
			if( pNode->IsFinalRoundResult() ) //게임 종료 중
			{
				PACKET_GUARD_VOID( kPacket.Write((int)BST_RESULT) );
			}
			else
				PACKET_GUARD_VOID( kPacket.Write((int)BST_PLAYING) );
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::FillLadderRoomInfo - Is not Processing" );
			return; //배틀룸 일 경우 게임시작 전
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::FillLadderRoomInfo - Send Room Info" );
		pUserParent->RelayPacket( kPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::SendLadderRoomJoinInfo Null Ladder Room Pointer!!(%s:%d)", pUserParent->GetPublicID().c_str(), dwIndex );
	}
}

Room* RoomNodeManager::GetGlobalLadderRoomNode( DWORD dwRoomIndex )
{
	vRoom_iter iter, iEnd = m_vRoomNode.end();
	for( iter=m_vRoomNode.begin() ; iter!=iEnd ; ++iter )
	{
		Room *pNode = *iter;
		if( NULL == pNode ) continue;
		if( pNode->GetRoomStyle() != RSTYLE_LADDERBATTLE )	continue;
		if( pNode->GetRoomIndex() == dwRoomIndex )
			return pNode;
	}


	vRoomCopyNode_iter iterCopy, iEndCopy = m_vRoomCopyNode.end();
	for( iterCopy = m_vRoomCopyNode.begin() ; iterCopy != iEndCopy ; ++iterCopy )
	{
		RoomCopyNode *pNode = *iterCopy;
		if( pNode == NULL ) continue;
		if( pNode->GetRoomStyle() != RSTYLE_LADDERBATTLE ) continue;
		if( pNode->GetRoomIndex() == dwRoomIndex )
			return (Room*)pNode;
	}
	return NULL;
}

//BattleRoomNode 참조, 불필요한 패킷 제거
void RoomNodeManager::FillLadderRoomInfo( SP2Packet& rkPacket, Room& rkRoomNode )
{
	PACKET_GUARD_VOID( rkPacket.Write( rkRoomNode.GetRoomIndex()) );
	PACKET_GUARD_VOID( rkPacket.Write( "LadderRoom" ));
	PACKET_GUARD_VOID( rkPacket.Write( "LadderHostName") ); //rkRoomNode.GetHostName() 대만에서 문제가 됨
	PACKET_GUARD_VOID( rkPacket.Write( rkRoomNode.GetJoinUserCnt()) );
	PACKET_GUARD_VOID( rkPacket.Write( rkRoomNode.GetPlayUserCnt()) );
	PACKET_GUARD_VOID( rkPacket.Write( 1 )); //m_iMaxPlayerBlue
	PACKET_GUARD_VOID( rkPacket.Write( 1 )); //m_iMaxPlayerRed
	PACKET_GUARD_VOID( rkPacket.Write( 4 )); //m_iMaxObserver
	PACKET_GUARD_VOID( rkPacket.Write( rkRoomNode.GetRoomPW()) );
	PACKET_GUARD_VOID( rkPacket.Write( rkRoomNode.GetAverageLevel()) ); //GetRoomLevel()
	PACKET_GUARD_VOID( rkPacket.Write( false )); //m_bUseExtraOption
	PACKET_GUARD_VOID( rkPacket.Write( false ));//m_bNoChallenger
	return;
}

void RoomNodeManager::EnterUserToLadderRoom( User* pUser, Room& rkRoomNode, bool bObserver )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[TEST] RoomNodeManager::EnterUserToLadderRoom - %s - No:%d", pUser->GetPublicID().c_str(), rkRoomNode.GetRoomIndex() );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::EnterUserToLadderRoom - User is Empty" );
		return;
	}

	pUser->SetStealth( bObserver );

	if( rkRoomNode.IsRoundEndState() ) //플레이 중인지 체크~
	{
		pUser->ExitRoomToLobby();
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomNodeManager::EnterUserToLadderRoom - IsRoundEndState is true" );
		return;
	}

	// 예외 처리 : 동일한 유저가 두번 입장 가능성이 있으므로, 입장할 때 이미 입장되어있는 아이디는 삭제한다.
	rkRoomNode.RemoveUser( pUser->GetUserIndex(), rkRoomNode.GetRoomIndex() );

	//들어온 유저에게 파티 정보 전송.
	SP2Packet kPacket( STPK_LADDERROOM_INFO );
	FillLadderRoomInfo( kPacket ,rkRoomNode );
	pUser->RelayPacket( kPacket );

	//들어온 유저에게 모든 파티원 데이터(자신 포함) 전송.(유저 정보 전송할 때 파티 레벨도 같이 보낸다.)
	rkRoomNode.SendUserDataTo( pUser );  //STPK_LADDERROOM_USER_INFO

	// 룸 입장.
	SP2Packet kPacket1( STPK_MOVING_ROOM );
	PACKET_GUARD_VOID( kPacket1.Write(rkRoomNode.GetModeType()) );
	PACKET_GUARD_VOID( kPacket1.Write(rkRoomNode.GetModeSubNum()) );
	PACKET_GUARD_VOID( kPacket1.Write(rkRoomNode.GetModeMapNum()) );
	PACKET_GUARD_VOID( kPacket1.Write((int)rkRoomNode.GetPlazaModeType()) );
	pUser->SendMessage( kPacket1 );
	pUser->EnterRoom( &rkRoomNode );

 	SP2Packet kPacket4( STPK_BATTLEROOM_COMMAND );
 	PACKET_GUARD_VOID( kPacket4.Write(BATTLEROOM_MODE_SEL_OK) );
 	PACKET_GUARD_VOID( kPacket4.Write( rkRoomNode.GetModeType()) );
 	PACKET_GUARD_VOID( kPacket4.Write( rkRoomNode.GetModeMapNum()) );
 	PACKET_GUARD_VOID( kPacket4.Write(false) );
 	pUser->RelayPacket( kPacket4 );
 
  //m_NodeSync.Update( BattleRoomSync::BRS_CHANGEINFO );
}

