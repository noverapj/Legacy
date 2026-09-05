#include "stdafx.h"

#include "../MainProcess.h"

#include "UserNodeManager.h"

#include "ShuffleRoomReserveMgr.h"
#include "ShuffleRoomManager.h"
#include "ServerNodeManager.h"
#include "LevelMatchManager.h"

ShuffleRoomReserveMgr *ShuffleRoomReserveMgr::sg_Instance = NULL;
ShuffleRoomReserveMgr::ShuffleRoomReserveMgr()
{
}

ShuffleRoomReserveMgr::~ShuffleRoomReserveMgr()
{
}

ShuffleRoomReserveMgr &ShuffleRoomReserveMgr::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ShuffleRoomReserveMgr;

	return *sg_Instance;
}

void ShuffleRoomReserveMgr::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ShuffleRoomReserveMgr::Initialize()
{
}

void ShuffleRoomReserveMgr::AddShuffleQueue( User *pUser, int iGlobalSearchingTryCount )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - pUser == NULL!", __FUNCTION__ );
		return;
	}

	//바로 입장 가능한 방이 있는지 검색
	ShuffleRoomParent *pNode = g_ShuffleRoomManager.GetJoinShuffleRoomNode( pUser->GetKillDeathLevel(), iGlobalSearchingTryCount );
	if( pNode )
	{
		pUser->EnterShuffleRoom( pNode );
		
		LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : %d번 방(%d) 중간참여 : %s (%d)", __FUNCTION__, pNode->GetIndex(), pNode->GetAbilityMatchLevel(), pUser->GetPublicID().c_str(), pUser->GetKillDeathLevel() );
		return;
	}

	GlobalSearchingShuffleUser( pUser, iGlobalSearchingTryCount );
}

void ShuffleRoomReserveMgr::GlobalSearchingShuffleUser( User *pSearchUser, int iGlobalSearchingTryCount )
{
	if( !pSearchUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - pSearchUser == NULL!", __FUNCTION__ );
		return;
	}

	//유저중에 매칭 조건이 맞는 유저를 찾는다(지정된 횟수 만큼 시도)
	if( iGlobalSearchingTryCount < g_ShuffleRoomManager.GetMatchCheckMaxCount( pSearchUser->GetKillDeathLevel() ) )
	{
		DWORDVec vUserGroup;
		g_UserNodeManager.FindShuffleGlobalQueueUser( pSearchUser, iGlobalSearchingTryCount, vUserGroup );

		if( vUserGroup.empty() )
		{
			//조건에 맞는 유저를 못찾았음으로 매칭 대기
			pSearchUser->SetShuffleGlboalSearch( true );
			pSearchUser->SyncUserShuffle();

			LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : %s(킬데스레벨:%d)님 조건에 맞는 유저 매칭 실패 후 매칭 대기(검색 시도 횟수:%d)", __FUNCTION__, pSearchUser->GetPublicID().c_str(), pSearchUser->GetKillDeathLevel(), iGlobalSearchingTryCount );
		}
		else
		{
			//같이 플레이할 유저 그룹생성
			UserGroup kGroup;
			kGroup.Init();
			kGroup.Add( pSearchUser->GetUserIndex(), pSearchUser->GetKillDeathLevel() );

			for( DWORDVec::iterator iter = vUserGroup.begin(); iter != vUserGroup.end(); ++iter )
			{
				UserParent* pUser = g_UserNodeManager.GetGlobalUserNode( *iter );
				if( pUser )
				{
					kGroup.Add( pUser->GetUserIndex(), pUser->GetKillDeathLevel() );
					LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : %s(킬데스레벨:%d)님이 %s님을 유저 그룹에 추가(검색 시도 횟수:%d)", __FUNCTION__, pSearchUser->GetPublicID().c_str(), pSearchUser->GetKillDeathLevel(), pUser->GetPublicID().c_str(), iGlobalSearchingTryCount );
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : %s(킬데스레벨:%d)님 유저 그룹생성시 유저 추가 실패(검색 시도 횟수:%d)", __FUNCTION__, pSearchUser->GetPublicID().c_str(), pSearchUser->GetKillDeathLevel(), iGlobalSearchingTryCount );
				}
			}

			ShuffleRoomNode* pRoomNode = CreateShuffleRoom( kGroup );
			if( pRoomNode )
			{
				//CreateShuffleRoom에서 ModeList를 검증하기떄문에 RETURN값을 쓰지 않음
				pRoomNode->ShuffleRoomReadyGo();
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : %s(킬데스레벨:%d)님 셔플룸 노드 생성 실패(검색 시도 횟수:%d)", __FUNCTION__, pSearchUser->GetPublicID().c_str(), pSearchUser->GetKillDeathLevel(), iGlobalSearchingTryCount );
				pSearchUser->SetShuffleGlboalSearch( false );
				pSearchUser->SyncUserShuffle();
			}
		}
	}
	else
	{
		//매칭 실패 - 대기 상태도 제거
		pSearchUser->SetShuffleGlboalSearch( false );
		pSearchUser->SyncUserShuffle();

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : %s님 (킬데스레벨:%d) 매칭 검색 횟수 초과로 조건에 맞는 유저 매칭 실패(검색 시도 횟수:%d)", __FUNCTION__, pSearchUser->GetPublicID().c_str(), pSearchUser->GetKillDeathLevel(), iGlobalSearchingTryCount );
	}
}

void ShuffleRoomReserveMgr::DeleteShuffleQueue( User *pUser )
{
	if( pUser )
	{
		pUser->SetShuffleGlboalSearch( false );
		pUser->SyncUserShuffle();

		LOG.PrintTimeAndLog( LOG_SHUFFLE, "[info][shuffle]User stop searching : [%d]", pUser->GetUserIndex() );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][shuffle]Invalid user stop searching");
	}
}

ShuffleRoomNode* ShuffleRoomReserveMgr::CreateShuffleRoom( const UserGroup& kGroup )
{
	ShuffleRoomNode *pRoomNode = g_ShuffleRoomManager.CreateNewShuffleRoom();
	if( !pRoomNode )
		return NULL;

	if( !pRoomNode->HasModeList() )
		return NULL;

	CRASH_GUARD();

	pRoomNode->SyncRealTimeCreate();

	LOOP_GUARD();

	int iSize = kGroup.GetUserCnt();
	for( int i = 0; i < iSize; ++i )
	{
		UserParent *pParent = g_UserNodeManager.GetGlobalUserNode( kGroup.m_vUserIndex[i] );
		if( !pParent )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : %d번 방 알 수 없는 유저 노드 발생 : %d", __FUNCTION__, pRoomNode->GetIndex(), kGroup.m_vUserIndex[i] );
			continue;
		}

		if( pParent->IsUserOriginal() )
		{
			User* pUser = static_cast<User*>( pParent );
			pUser->EnterShuffleRoom( pRoomNode );
			LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : %d번 방 신규참여 : %s님 (%d)", __FUNCTION__, pRoomNode->GetIndex(), pUser->GetPublicID().c_str(), pUser->GetKillDeathLevel() );
		}
		else
		{	
			UserCopyNode* pUser = static_cast<UserCopyNode*>( pParent );
			SP2Packet kPacket( SSTPK_SHUFFLEROOM_GLOBAL_CREATE );
			PACKET_GUARD_NULL( kPacket.Write( pUser->GetUserIndex() ) );
			PACKET_GUARD_NULL( kPacket.Write( pRoomNode->GetIndex() ) );
			pUser->SendMessage( kPacket );

			LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : %d번 방 신규참여 : %s님 (%d)", __FUNCTION__, pRoomNode->GetIndex(), pUser->GetPublicID().c_str(), pUser->GetKillDeathLevel() );
		}
	}
	LOOP_GUARD_CLEAR();

	return pRoomNode;
}