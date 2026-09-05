// UserNodeManager.cpp: implementation of the UserNodeManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../EtcHelpFunc.h"
#include "../ioEtcLogManager.h"

#include "UserNodeManager.h"
#include "RoomNodeManager.h"
#include "ChannelNodeManager.h"
#include "LadderTeamManager.h"
#include "ioEtcItemManager.h"
#include "ioSetItemInfo.h"
#include "ioSetItemInfoManager.h"
#include "ioItemInfoManager.h"
#include "LevelMatchManager.h"
#include "ioEventUserNode.h"
#include "BattleRoomManager.h"
#include "HeroRankManager.h"
#include "TournamentManager.h"
#include "ServerGuildInvenInfo.h"

#include "../Network/iocpHandler.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../MainServerNode/MainServerNode.h"

#include "ioUserPresent.h"
#include "ioPresentHelper.h"
#include "ServerNodeManager.h"
#include "ioExerciseCharIndexManager.h"
#include "ioMyLevelMgr.h"
#include "Room.h"
#include "NewPublicIDRefresher.h"
#include "ioSaleManager.h"
#include "licensemanager.h"
#include "ioItemInitializeControl.h"
#include "CostumeManager.h"
#include "GuildRewardManager.h"
#include "TradeInfoManager.h"
#include "GuildRoomsBlockManager.h"
#include "GuildRoomInfos.h"
#include "PersonalHomeInfo.h"
#include "HomeModeBlockManager.h"
#include "AccessoryManager.h"
#include "FishingManager.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../Shutdown.h"
#include "../BillingRelayServer/BillingRelayServer.h"
#include "../Local/ioLocalParent.h"
#include "../Local/ioLocalManager.h"
#include <strsafe.h>
#ifdef NPROTECT
#include "../nProtect/ionprotect.h"
#endif
#include "ioShutDownManager.h"
#include "ShuffleRoomManager.h"
#include "TestNodeManager.h"
#include "../NodeInfo/MissionManager.h"
#include "../NodeInfo/TradeSyncManger.h"
#include "ioOakBarrelManager.h"
#include "MatchManager.h"
#include "ioTimeGateManager.h"
#include "DiceGameManager.h"		// 2018-08-30 by bckim, 주사위 이벤트 추가

extern CLog EventLOG;
extern CLog TradeLOG;
extern CLog WemadeLOG;


UserNodeManager *UserNodeManager::sg_Instance = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UserNodeManager::UserNodeManager() : m_dwNodeGhostCheckTime(0), m_dwNodeSaveCheckTime(0), m_dwNodeRefillMonsterCoinTime(0), m_dwNodeTimeGashaponCheckTime(0),
									 m_dwNodeSyncTime(0), m_dwNodeDestoryTime(0), m_dwNodeShutDownCheckTime(0) , m_dwNodeSelectShutDownCheckTime(0),
									 m_dwNodeEventProcessTime(0), m_dwNodeCloverCheckTime(0), m_dwNodeRefillRaidTime(0), m_iMaxConnection(0),
									 m_dwNodeRefillOakBarrelSwordTime(0), 
									 m_iQuickStartPrisoner(0), m_iMakePrisonerRoom(0), m_iListSelectPrisoner(0)
{
	m_vUserNode.reserve(1500);	
	InitChannelingUserCntMap();
#ifdef XTRAP
	m_dwNodeXtrapCheckTime = 0; 
#endif

#ifdef NPROTECT
	m_dwNodeNProtectCheckTime = 0;
#endif 

#ifdef HACKSHIELD
	m_dwNodeHackShieldCheckTime = 0;
#endif

	CTime curTime = CTime::GetCurrentTime();
	m_OakBarrelInitTime = Help::GetSafeValueForCTimeConstructor( curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), 0, 0, 0 );
}

UserNodeManager::~UserNodeManager()
{
	m_vUserNode.clear();
	m_uUserNode.clear();
	m_uUserCopyNode.clear();
	m_uUserCopyNodePublicID.clear();
	m_uUserCopyNodePrivateID.clear();
	m_UserSyncDataVec.clear();

	m_vLikeDeveloperID.clear();
	m_vDeveloperID.clear();

	m_sDeveloperIPSet.clear();
	m_sCloudIPSet.clear();

	m_mChannelingCurUserCnt.clear();
	m_mChannelingMaxUserCnt.clear();
}

UserNodeManager &UserNodeManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new UserNodeManager;

	return *sg_Instance;
}

void UserNodeManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void UserNodeManager::InitMemoryPool()
{
	User::LoadHackCheckValue();
	User::LoadCharSlotValue();
	User::LoadNagleReferenceValue( 0 );	// Nagle Ref Cnt

	ioINILoader kLoader( "ls_config_game.ini" );
	kLoader.SetTitle( "MemoryPool" );
	m_iMaxConnection = kLoader.LoadInt( "user_pool", 1500 );
	m_iStableConnection = m_iMaxConnection * 0.9;

	kLoader.SetTitle( "User Session" );
	int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", MAX_BUFFER );	
	m_MemNode.CreatePool(0, 5000, FALSE); // 2013-01-28 신영욱, 최대 5000명까지 접속을 받게 한다
	for( int i=0 ; i< m_iMaxConnection; i++ )
	{
		m_MemNode.Push( new User( INVALID_SOCKET, iSendBufferSize, MAX_BUFFER+1 ) );
	}		

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][memorypool]User session send buffer : [%d]", iSendBufferSize );
}

void UserNodeManager::ReleaseMemoryPool()
{
	for(vUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter)
	{
		User *pUser = *iter;
		pUser->OnDestroy();
		pUser->OnSessionDestroy();
		m_MemNode.Push( pUser );
	}	
	for(uUser::iterator iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		pUser->OnDestroy();
		pUser->OnSessionDestroy();
		m_MemNode.Push( pUser );
	}	

	m_vUserNode.clear();
	m_uUserNode.clear();
	m_MemNode.DestroyPool();
	m_uUserCopyNode.clear();
	m_uUserCopyNodePublicID.clear();
	m_uUserCopyNodePrivateID.clear();
	m_UserSyncDataVec.clear();
}

User *UserNodeManager::CreateNewNode(SOCKET s)
{
	User *pUser = m_MemNode.Remove();
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"UserNodeManager::CreateNewNode MemPool Zero!" );
		return NULL;
	}
	
	pUser->SetSocket(s);
	pUser->OnCreate();
	return pUser;
}

void UserNodeManager::AddUserNode( User *pUserNode )
{
	m_vUserNode.push_back( pUserNode );
}

void UserNodeManager::ChangeUserNode(User *pUserNode)
{
	if(pUserNode->GetUserIndex() == 0) return;

	for(vUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter)
	{
		User* pUser  = *iter;
		if(!pUser) continue;

		if(pUser->GetEntity() == pUserNode->GetEntity())
		{
			m_vUserNode.erase(iter);
			m_uUserNode.insert( make_pair(pUserNode->GetUserIndex(), pUserNode) );
			break;
		}
	}	
}

void UserNodeManager::RemoveNode( User *pUserNode )
{
	uUser::iterator iter = m_uUserNode.find(pUserNode->GetUserIndex());
	if(iter != m_uUserNode.end())
	{
		m_uUserNode.erase(iter);
		m_MemNode.Push( pUserNode );
		return;
	}

	for(vUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter)
	{
		User* pUser  = *iter;
		if((pUser) && (pUser->GetEntity() == pUserNode->GetEntity()))
		{
			m_vUserNode.erase(iter);
			m_MemNode.Push( pUser );
			break;
		}
	}	
}

void UserNodeManager::MoveUserNode(User *pUserNode)
{
	// 서버 이동이 되었으므로 Disconnect 발생할 때 까지 정보를 갱신하지 않는다.
	// 서버 이동한 유저에게 친구 목록을 전부 전송해줄때까지 블럭 상태.
	pUserNode->SetSessionState( User::SS_SERVERMOVING );    
}

void UserNodeManager::ServerMovingPassiveLogOut( SP2Packet &rkPacket )
{
	// 더미 노드를 생성하여 마지막으로 받은 정보를 처리하고 로그아웃 시킨다.
	User kDummyNode( INVALID_SOCKET, 1, 1 );
	kDummyNode.ApplyMoveData( rkPacket, true );
	kDummyNode.OnSessionDestroy();
}

void UserNodeManager::RemoveBattleRoomCopyNode( DWORD dwIndex )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsBattleRoom() ) continue;
	
		// 전투방 퇴장
		pUser->LeaveBattleRoomException( dwIndex );
	}
}

void UserNodeManager::RemoveLadderTeamCopyNode( DWORD dwIndex )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsLadderTeam() ) continue;

		// 래더팀 퇴장
		pUser->LeaveLadderTeamException( dwIndex );
	}
}

void UserNodeManager::RemoveShuffleRoomCopyNode( DWORD dwIndex )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsShuffleRoom() ) continue;

		// 전투방 퇴장
		pUser->LeaveShuffleRoomException( dwIndex );
	}
}

void UserNodeManager::RemoveChannelCopyNode( DWORD dwIndex )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;

		// 채널 퇴장
		pUser->LeaveChannelException( dwIndex );
	}
}

void UserNodeManager::AddCopyUser( UserCopyNode *pUser )
{
	if( pUser == NULL ) return;

	if( GetUserCopyNode(pUser->GetUserIndex()) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserNodeManager::AddCopyUser duplicated(%d)", pUser->GetUserIndex() );
	}
	else
	{
		std::string szPublicID = pUser->GetPublicID().c_str();
		std::string szPrivateID = pUser->GetPrivateID().c_str();
		m_uUserCopyNode.insert( make_pair(pUser->GetUserIndex(), pUser) );
		m_uUserCopyNodePublicID.insert( make_pair(szPublicID, pUser->GetUserIndex()) );
		m_uUserCopyNodePrivateID.insert( make_pair(szPrivateID, pUser->GetUserIndex()) );
	}
}

void UserNodeManager::RemoveCopyUser( const DWORD dwUserIndex )
{
	uUserCopyNode_iter it = m_uUserCopyNode.find( dwUserIndex );
	if(it != m_uUserCopyNode.end())
	{
		UserCopyNode *pUser = it->second;
		if(pUser)
		{
			RemoveCopyUserTablePrivateID( pUser->GetPrivateID() );
			RemoveCopyUserTablePublicID( pUser->GetPublicID() );
		}

		m_uUserCopyNode.erase( it );
	}
}

void UserNodeManager::RemoveCopyUserTablePrivateID( const ioHashString &szPrivateID )
{
	std::string szID = szPrivateID.c_str();;

	uUserCopyNodeTable_iter iter = m_uUserCopyNodePrivateID.find( szID );
	if(iter != m_uUserCopyNodePrivateID.end())
	{
		m_uUserCopyNodePrivateID.erase( iter );
	}
}

void UserNodeManager::RemoveCopyUserTablePublicID( const ioHashString &szPublicID )
{
	std::string szID = szPublicID.c_str();

	uUserCopyNodeTable_iter iter = m_uUserCopyNodePublicID.find( szID );
	if(iter != m_uUserCopyNodePublicID.end())
	{
		m_uUserCopyNodePublicID.erase( iter );
	}
}

UserCopyNode* UserNodeManager::GetUserCopyNode( const DWORD dwUserIndex )
{
	uUserCopyNode_iter it = m_uUserCopyNode.find( dwUserIndex );
	if(it != m_uUserCopyNode.end())
	{
		return it->second;
	}
	return NULL;
}

UserCopyNode* UserNodeManager::GetUserCopyNodeByPublicID( const ioHashString &szPublicID )
{
	std::string szID = szPublicID.c_str();

	uUserCopyNodeTable_iter it = m_uUserCopyNodePublicID.find( szID );
	if(it != m_uUserCopyNodePublicID.end())
	{
		return GetUserCopyNode(it->second);
	}
	return NULL;
}

UserCopyNode* UserNodeManager::GetUserCopyNodeByPrivateID( const ioHashString &szPrivateID )
{
	std::string szID = szPrivateID.c_str();

	uUserCopyNodeTable_iter it = m_uUserCopyNodePrivateID.find( szID );
	if(it != m_uUserCopyNodePrivateID.end())
	{
		return GetUserCopyNode(it->second);
	}
	return NULL;
}

User* UserNodeManager::GetUserNodeByPrivateID( const ioHashString &szID )
{
	CRASH_GUARD();
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;

		if( pUser->GetPrivateID() == szID )
			return pUser;
	}

	for(vUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter)
	{
		User *pUser = *iter;
		if( pUser == NULL || !pUser->IsConnectState() ) continue;

		if( pUser->GetPrivateID() == szID )
			return pUser;
	}

	return NULL;
}

User* UserNodeManager::GetUserNodeByPublicID( const ioHashString &szID )
{
	CRASH_GUARD();
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;

		if( pUser->GetPublicID() == szID )
			return pUser;
	}
	//유영재 서버 이동시 유저 index정보가 저장전 해당 함수를 호출하는 경우 발생. m_uUserNode에 값이 없을경우 m_vUserNode 검사하도록 함.
	for(vUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter)
	{
		User *pUser = *iter;
		if( !pUser->IsConnectState() ) continue;

		if( pUser->GetPublicID() == szID )
			return pUser;
	}

	return NULL;
}

User* UserNodeManager::GetUserNodeByGUID( const ioHashString &szUserGUID )
{
	CRASH_GUARD();
	//유영재 유저의 정보를 다 가져온 후에도 해당GUID를 통해 검색을 하므로 m_uUserNode검사 루틴 추가. 차후 수정 필요.
	/*for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;

		if( pUser->GetGUID() == szUserGUID )
			return pUser;
	}*/

	for(vUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter)
	{
		User *pUser = *iter;
		if( !pUser->IsConnectState() ) continue;
		
		if( pUser->GetGUID() == szUserGUID )
			return pUser;
	}

	return NULL;
}

User *UserNodeManager::GetUserNode(DWORD dwIndex)
{
	uUser::iterator it = m_uUserNode.find(dwIndex);
	if(it != m_uUserNode.end())
	{
		return it->second;
	}
	/*
	for(vUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter)
	{
		User *pUser = *iter;
		if( !pUser->IsConnectState() ) continue;

		if( pUser->GetUserIndex() == dwIndex )
		{
			if( !pUser->GetPublicID().IsEmpty() )
				return pUser;
		}
	}
	*/
	return NULL;
}

User *UserNodeManager::GetMoveUserNode( DWORD dwIndex, int iMovingValue )
{
	User* pUser = GetUserNode(dwIndex);
	if(!pUser) return NULL;

	if( pUser->GetServerMovingValue() == iMovingValue )
	{
		return pUser;
	}
	return NULL;
}

UserParent *UserNodeManager::GetGlobalUserNode( DWORD dwIndex )
{
	UserParent *pGlobalUser = (UserParent*)GetUserNode( dwIndex );
	if( pGlobalUser )
		return pGlobalUser;

	return GetUserCopyNode( dwIndex );
}

UserParent *UserNodeManager::GetGlobalUserNode( const ioHashString &szPublicID )
{
	UserParent *pGlobalUser = (UserParent*)GetUserNodeByPublicID( szPublicID );
	if( pGlobalUser )
		return pGlobalUser;

	return GetUserCopyNodeByPublicID( szPublicID );
}

UserParent * UserNodeManager::GetGlobalUserNodeByPrivateID( const ioHashString &szPrivateID )
{
	UserParent *pGlobalUser = (UserParent*)GetUserNodeByPrivateID( szPrivateID );
	if( pGlobalUser )
		return pGlobalUser;

	return GetUserCopyNodeByPrivateID( szPrivateID );
}

bool UserNodeManager::IsConnectUser( const ioHashString &szPrivateID )
{
	if( GetUserNodeByPrivateID( szPrivateID ) )
		return true;

	CRASH_GUARD();

	// 타서버에 있는 유저
	return (GetUserCopyNodeByPrivateID( szPrivateID ) != NULL) ? true : false;
}

bool UserNodeManager::IsPublicIDConnectUser( const DWORD dwUserIndex )
{
	UserParent *pGlobalUser = NULL;

	// 오리지날 유저
	pGlobalUser = GetUserNode( dwUserIndex );

	// 타 서버 유저
	if( !pGlobalUser )
	{
		pGlobalUser = GetUserCopyNode( dwUserIndex );
	}
	
	return (!pGlobalUser) ? false : true;
}

void UserNodeManager::InitUserEventItem( const int iType )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;

		g_ItemInitControl.CheckInitUserEtcItemByPlayer( pUser, iType );
	}
}

void UserNodeManager::InitUserLadderPointNRecord()
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;

		pUser->InitUserLadderPointNRecord();
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "UserNodeManager::InitUserLadderPointNRecord() 로그인 중인 유저 래더포인트 초기화" );
}

void UserNodeManager::AllUserUpdateLadderPointNExpert()
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;
		
		pUser->SaveUserLadderData();
		pUser->SaveUserHeroExpert();
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "UserNodeManager::AllUserUpdateLadderPointNExpert() 로그인 중인 유저 래더포인트 업데이트" );
}

void UserNodeManager::AllCampUserCampDataSync()
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;
		if( pUser->GetUserCampPos() == CAMP_NONE ) continue;

		SP2Packet kPacket( MSTPK_CAMP_DATA_SYNC );
		kPacket << pUser->GetUserIndex();
		g_MainServer.SendMessage( kPacket );
	}
}

void UserNodeManager::AllUserTournamentTeamDelete( DWORD dwTourIndex )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;
	
		ioUserTournament *pUserTournament = pUser->GetUserTournament();
		if( pUserTournament )
		{
			if( pUserTournament->TournamentEndDeleteTeam( dwTourIndex ) )
			{
				pUserTournament->TournamentEndDeleteCheerData( dwTourIndex );

				// 유저에게 알림
				SP2Packet kPacket( STPK_TOURNAMENT_END_PROCESS );
				kPacket << dwTourIndex;
				pUser->SendMessage( kPacket );
			}
		}
	}
}

void UserNodeManager::AllUserTournamentTeamPosSync( DWORD dwTeamIndex, SHORT Position, BYTE TourPos, bool bSync )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;

		ioUserTournament *pUserTournament = pUser->GetUserTournament();
		if( pUserTournament )
		{
			pUserTournament->TournamentTeamPosSync( dwTeamIndex, Position, TourPos, bSync );
		}
	}
}

void UserNodeManager::UserTournamentBattleInvite( DWORD dwBattleIndex, DWORD dwBlueIndex, DWORD dwRedIndex )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;

		ioUserTournament *pUserTournament = pUser->GetUserTournament();
		if( pUserTournament )
		{
			if( pUserTournament->IsInviteCheckTeamSend( dwBlueIndex, dwRedIndex ) )
			{
				SP2Packet kPacket( STPK_TOURNAMENT_BATTLE_INVITE );
				kPacket << dwBattleIndex << dwBlueIndex << dwRedIndex << TournamentRoundData::INVITE_DELAY_TIME;
				pUser->SendMessage( kPacket );

				LOG.PrintTimeAndLog( 0, "[대회로그] %s - %s - 대회 초대 발송(배틀룸 : %d), 팀정보 - 블루 : %d, 레드 : %d", __FUNCTION__, pUser->GetPublicID().c_str(),
					dwBattleIndex, dwBlueIndex, dwRedIndex );
			}
		}
	}
}

void UserNodeManager::UserTournamentBattleInvite( DWORD dwBattleIndex, DWORD dwTeamIndex, DWORD dwBlueIndex, DWORD dwRedIndex )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;

		ioUserTournament *pUserTournament = pUser->GetUserTournament();
		if( pUserTournament )
		{
			if( pUserTournament->IsInviteCheckTeamSend( dwTeamIndex ) )
			{
				SP2Packet kPacket( STPK_TOURNAMENT_BATTLE_INVITE );
				kPacket << dwBattleIndex << dwBlueIndex << dwRedIndex << TournamentRoundData::INVITE_DELAY_TIME;
				pUser->SendMessage( kPacket );
			}
		}
	}
}

void UserNodeManager::AllUserLeaveServer(bool bBlock)
{
	static vServerIndex vServerIndexes;
	vServerIndexes.clear();

	g_ServerNodeManager.GetServerNodes(false, vServerIndexes);

	if(bBlock && (vServerIndexes.size() == 0))
	{
		// 이동할 서버가 없으므로 이동시키지 않는다
		return;
	}

	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User* pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;

		if(bBlock)
		{
			int iIndex = rand() % vServerIndexes.size();
			int iServerIndex = vServerIndexes[iIndex];

			pUser->LeaveServer(iServerIndex);
		}
		else
		{
			pUser->LeaveServer(0);
		}
	}	
}

void UserNodeManager::AllUserDisconnect(const int iDBAgentIndex)
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"Close All UserNode, DBAgent Index : %d", iDBAgentIndex);

	// iDBAgent를 사용하는 모든 유저의 접속을 종료시킨다
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User* pUser = iter->second;
		if( (!pUser) || (pUser->IsDisconnectState()) ) continue;

		if( pUser->GetUserDBAgentID() == iDBAgentIndex )
		{
			pUser->CloseConnection();
		}
	}	
}

int UserNodeManager::GetNodeSizeByChannelingType( ChannelingType eChannelingType )
{
	int iCount = 0;
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser->GetChannelingType() != eChannelingType ) 
			continue;
		iCount++;
	}

	return iCount;
}

void UserNodeManager::SendPlazaInviteList( User *pUser, PlazaType ePlazaType, int iCurPage, int iMaxCount, int iRoomLevel )
{
	if( pUser == NULL ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	static vSortUser vSortList;
	vSortList.clear();
	
	// 동일 서버
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pTargetUser = iter->second;
		if( pTargetUser == pUser ) continue;
		if( !pTargetUser->IsConnectState() ) continue;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->GetModeType() != MT_NONE && pTargetUser->GetModeType() != MT_TRAINING && pTargetUser->GetModeType() != MT_HEADQUARTERS ) continue;
		if( pTargetUser->GetMyRoom() && pTargetUser->GetMyRoom() == pUser->GetMyRoom() ) continue;
		if( ePlazaType == PT_GUILD && pTargetUser->GetGuildIndex() != pUser->GetGuildIndex() ) continue;
		if( pTargetUser->GetUserPos() == UP_BATTLE_ROOM || pTargetUser->GetUserPos() == UP_LADDER_TEAM ) continue;

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pTargetUser->GetKillDeathLevel() - iRoomLevel );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
		}        
		
		if( kSortUser.m_iSortPoint == FRIEND_USER_POINT )
		{
			if( !g_LevelMatchMgr.IsPlazaLevelJoin( iRoomLevel, pTargetUser->GetKillDeathLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) 
				continue;
		}

		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	// 타서버
	uUserCopyNode_iter iterCopy,iEndCopy;
	iEndCopy = m_uUserCopyNode.end();
	for(iterCopy=m_uUserCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		UserCopyNode *pTargetUser = iterCopy->second;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->GetModeType() != MT_NONE && pTargetUser->GetModeType() != MT_TRAINING && pTargetUser->GetModeType() != MT_HEADQUARTERS ) continue;
		if( ePlazaType == PT_GUILD && pTargetUser->GetGuildIndex() != pUser->GetGuildIndex() ) continue;
		if( pTargetUser->GetUserPos() == UP_BATTLE_ROOM || pTargetUser->GetUserPos() == UP_LADDER_TEAM ) continue;

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pTargetUser->GetKillDeathLevel() - iRoomLevel );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;	
		}

		if( kSortUser.m_iSortPoint == FRIEND_USER_POINT )
		{
			if( !g_LevelMatchMgr.IsPlazaLevelJoin( iRoomLevel, pTargetUser->GetKillDeathLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) 
				continue;
		}

		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	int iMaxList = vSortList.size();
	if( iMaxList == 0 )
	{
		// 로비에 유저가 없으면 없다고 알려줘야 클라이언트 리스트에 남지 않는다.
		SP2Packet kPacket( STPK_PLAZA_INVITE_LIST );
		kPacket << 0 << 0;
		pUser->SendMessage( kPacket );
		return;
	}
	std::sort( vSortList.begin(), vSortList.end(), UserSort() );

	int iStartPos = iCurPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

	SP2Packet kPacket( STPK_PLAZA_INVITE_LIST );
	kPacket << iMaxList << iCurSize;
	for(int i = iStartPos; i < iStartPos + iCurSize;i++)
	{
		if( iMaxList <= i ) // 최대 갯수가 넘는 경우가 발생한다면
			break;

		if( vSortList[i].m_pUser )
		{
			kPacket << vSortList[i].m_pUser->GetPublicID() << vSortList[i].m_pUser->GetGradeLevel() 
					<< vSortList[i].m_pUser->GetGuildIndex() << vSortList[i].m_pUser->GetGuildMark();
		}		
		else    //예외
		{
			kPacket << "" << 0 << 0 << 0;
		}
	}	
	pUser->SendMessage( kPacket );
	vSortList.clear();
}

void UserNodeManager::SendBattleRoomInviteList( User *pUser, int iCurPage, int iMaxCount, int iBattleMatchLevel, int iSelectModeTerm )
{
	if( pUser == NULL ) return;
	if( !pUser->IsBattleRoom() ) return;
	if( pUser->IsLadderTeam() ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	BattleRoomParent *pBattleParent = pUser->GetMyBattleRoom();
	if( pBattleParent == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserNodeManager::SendBattleRoomInviteList(%s) : 초대리스트를 요청했지만 전투방이 없음", pUser->GetPublicID().c_str() );
		return;
	}

	// 방송 유저를 제외하고는 초대 리스트를 만들지 않음.
	if( pBattleParent->GetBattleEventType() == BET_BROADCAST_AFRICA || pBattleParent->GetBattleEventType() == BET_BROADCAST_MBC )
	{
		if( pUser->GetUserEventType() != USER_TYPE_BROADCAST_AFRICA && 
			pUser->GetUserEventType() != USER_TYPE_BROADCAST_MBC )
		{
			SP2Packet kPacket( STPK_BATTLEROOM_INVITE_LIST );
			kPacket << 0 << 0;
			pUser->SendMessage( kPacket );
			return;
		}
	}
	else if( pBattleParent->GetBattleEventType() == BET_TOURNAMENT_BATTLE )
	{
		SP2Packet kPacket( STPK_BATTLEROOM_INVITE_LIST );
		kPacket << 0 << 0;
		pUser->SendMessage( kPacket );
		return;
	}

	static vSortUser vSortList;
	vSortList.clear();

	// 동일 서버
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pTargetUser = iter->second;
		if( pTargetUser == pUser ) continue;
		if( !pTargetUser->IsConnectState() ) continue;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->IsBattleRoom() ) continue;
		if( pTargetUser->IsLadderTeam() ) continue;
		if( pBattleParent->IsMapLimitGrade( pTargetUser->GetGradeLevel() ) ) continue;
		if( iSelectModeTerm == BMT_TEAM_SURVIVAL_FIRST && !pTargetUser->IsSafetyLevel() ) continue;

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pTargetUser->GetKillDeathLevel() - iBattleMatchLevel );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;		
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
		}
		
		if( kSortUser.m_iSortPoint == FRIEND_USER_POINT )
		{
			if( !pBattleParent->IsLevelMatchIgnore() && !g_LevelMatchMgr.IsPartyLevelJoin( iBattleMatchLevel, pTargetUser->GetKillDeathLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) 
				continue;

			if( !pBattleParent->IsLevelMatchIgnore() && pBattleParent->IsUseExtraOption() && !g_BattleRoomManager.CheckEnableExtraOptionLevel(pTargetUser->GetGradeLevel()) )
				continue;
		}
		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	// 타서버
	uUserCopyNode_iter iterCopy,iEndCopy;
	iEndCopy = m_uUserCopyNode.end();
	for(iterCopy=m_uUserCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		UserCopyNode *pTargetUser = iterCopy->second;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->GetUserPos() == UP_BATTLE_ROOM || pTargetUser->GetUserPos() == UP_LADDER_TEAM ) continue;
		if( iSelectModeTerm == BMT_TEAM_SURVIVAL_FIRST && !pTargetUser->IsSafetyLevel() ) continue;
		if( pBattleParent->IsMapLimitGrade( pTargetUser->GetGradeLevel() ) ) continue;
		
		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pTargetUser->GetKillDeathLevel() - iBattleMatchLevel );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
		}
		
		if( kSortUser.m_iSortPoint == FRIEND_USER_POINT )
		{
			if( !pBattleParent->IsLevelMatchIgnore() && !g_LevelMatchMgr.IsPartyLevelJoin( iBattleMatchLevel, pTargetUser->GetKillDeathLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) 
				continue;

			if( !pBattleParent->IsLevelMatchIgnore() && pBattleParent->IsUseExtraOption() && !g_BattleRoomManager.CheckEnableExtraOptionLevel(pTargetUser->GetGradeLevel()) )
				continue;
		}
		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	int iMaxList = vSortList.size();
	if( iMaxList == 0 )
	{
		// 로비에 유저가 없으면 없다고 알려줘야 클라이언트 리스트에 남지 않는다.
		SP2Packet kPacket( STPK_BATTLEROOM_INVITE_LIST );
		kPacket << 0 << 0;
		pUser->SendMessage( kPacket );
		return;
	}
	std::sort( vSortList.begin(), vSortList.end(), UserSort() );

	int iStartPos = iCurPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

	SP2Packet kPacket( STPK_BATTLEROOM_INVITE_LIST );
	kPacket << iMaxList << iCurSize;
	for(int i = iStartPos; i < iStartPos + iCurSize;i++)
	{
		if( iMaxList <= i ) // 최대 갯수가 넘는 경우가 발생한다면
			break;

		if( vSortList[i].m_pUser )
		{
			kPacket << vSortList[i].m_pUser->GetPublicID() << vSortList[i].m_pUser->GetGradeLevel()
				    << vSortList[i].m_pUser->GetGuildIndex() << vSortList[i].m_pUser->GetGuildMark();
		}		
		else    //예외
		{
			kPacket << "" << 0 << 0 << 0;
		}
	}	
	pUser->SendMessage( kPacket );
	vSortList.clear();
}

void UserNodeManager::SendLadderTeamInviteList( User *pUser, int iCurPage, int iMaxCount )
{
	if( pUser == NULL ) return;	
	if( !pUser->IsLadderTeam() ) return;
	if( pUser->IsBattleRoom() ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	LadderTeamParent *pLadderTeamParent = pUser->GetMyLadderTeam();
	if( pLadderTeamParent == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserNodeManager::SendLadderTeamInviteList(%s) : 초대리스트를 요청했지만 래더팀이 없음", pUser->GetPublicID().c_str() );
		return;
	}

	static vSortUser vSortList;
	vSortList.clear();

	// 동일 서버
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pTargetUser = iter->second;
		if( pTargetUser == pUser ) continue;
		if( !pTargetUser->IsConnectState() ) continue;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->IsBattleRoom() || pTargetUser->IsLadderTeam() ) continue;
		if( pTargetUser->GetGradeLevel() < Help::GetLadderTeamLimitGrade() ) continue;
		if( pLadderTeamParent->GetCampType() != pTargetUser->GetUserCampPos() ) continue;
		if( pLadderTeamParent->GetJoinGuildIndex() != 0 )
		{
			if( pTargetUser->GetGuildIndex() != pLadderTeamParent->GetJoinGuildIndex() ) continue;
		}

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pLadderTeamParent->GetAbilityMatchLevel() - pTargetUser->GetKillDeathLevel() );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;		
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
		}

		if( kSortUser.m_iSortPoint == FRIEND_USER_POINT )
		{
			if( !g_LevelMatchMgr.IsLadderLevelJoin( pLadderTeamParent->GetAbilityMatchLevel(), pTargetUser->GetKillDeathLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) 
				continue;
		}
		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	// 타서버
	uUserCopyNode_iter iterCopy,iEndCopy;
	iEndCopy = m_uUserCopyNode.end();
	for(iterCopy=m_uUserCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		UserCopyNode *pTargetUser = iterCopy->second;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->GetUserPos() == UP_BATTLE_ROOM || pTargetUser->GetUserPos() == UP_LADDER_TEAM ) continue;
		if( pLadderTeamParent->GetCampType() != pTargetUser->GetUserCampPos() ) continue;
		if( pTargetUser->GetGradeLevel() < Help::GetLadderTeamLimitGrade() ) continue;
		if( pLadderTeamParent->GetJoinGuildIndex() != 0 )
		{
			if( pTargetUser->GetGuildIndex() != pLadderTeamParent->GetJoinGuildIndex() ) continue;
		}

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pLadderTeamParent->GetAbilityMatchLevel() - pTargetUser->GetKillDeathLevel() );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
		}

		if( kSortUser.m_iSortPoint == FRIEND_USER_POINT )
		{
			if( !g_LevelMatchMgr.IsLadderLevelJoin( pLadderTeamParent->GetAbilityMatchLevel(), pTargetUser->GetKillDeathLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) 
				continue;
		}
		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	int iMaxList = vSortList.size();
	if( iMaxList == 0 )
	{
		// 로비에 유저가 없으면 없다고 알려줘야 클라이언트 리스트에 남지 않는다.
		SP2Packet kPacket( STPK_LADDERTEAM_INVITE_LIST );
		kPacket << 0 << 0;
		pUser->SendMessage( kPacket );
		return;
	}
	std::sort( vSortList.begin(), vSortList.end(), UserSort() );

	int iStartPos = iCurPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

	SP2Packet kPacket( STPK_LADDERTEAM_INVITE_LIST );
	kPacket << iMaxList << iCurSize;
	for(int i = iStartPos; i < iStartPos + iCurSize;i++)
	{
		if( iMaxList <= i ) // 최대 갯수가 넘는 경우가 발생한다면
			break;

		if( vSortList[i].m_pUser )
		{
			kPacket << vSortList[i].m_pUser->GetPublicID() << vSortList[i].m_pUser->GetGradeLevel()
				    << vSortList[i].m_pUser->GetGuildIndex() << vSortList[i].m_pUser->GetGuildMark();
		}		
		else    //예외
		{
			kPacket << "" << 0 << 0 << 0;
		}
	}	
	pUser->SendMessage( kPacket );
	vSortList.clear();
}

void UserNodeManager::SendServerLobbyUserList( User *pUser, int iCurPage, int iMaxCount )
{
	if( pUser == NULL ) return;	

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	static vSortUser vSortList;
	vSortList.clear();

	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pTargetUser = iter->second;
		if( pTargetUser == NULL ) continue;
		if( !pTargetUser->IsConnectState() ) continue;
//		if( pUser == pTargetUser ) continue;
		if( !pTargetUser->IsServerLobby() ) continue;

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pTargetUser->GetKillDeathLevel() - pUser->GetKillDeathLevel() );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;		
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
		}
		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	int iMaxList = vSortList.size();
	if( iMaxList == 0 )
	{
		// 채널에 유저가 없으면 없다고 알려줘야 클라이언트 리스트에 남지 않는다.
		SP2Packet kPacket( STPK_SERVER_LOBBY_INFO );
		kPacket << 0 << 0;
		pUser->SendMessage( kPacket );
		return;
	}
	std::sort( vSortList.begin(), vSortList.end(), UserSort() );

	int iStartPos = iCurPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );
	SP2Packet kPacket( STPK_SERVER_LOBBY_INFO );
	kPacket << iMaxList << iCurSize;
	for(int i = iStartPos;i < iStartPos + iCurSize;i++)
	{
		if( iMaxList <= i ) // 최대 갯수가 넘는 경우가 발생한다면
			break;

		if( vSortList[i].m_pUser )
		{
			kPacket << vSortList[i].m_pUser->GetPublicID() << vSortList[i].m_pUser->GetGradeLevel()
				    << vSortList[i].m_pUser->GetGuildIndex() << vSortList[i].m_pUser->GetGuildMark();
		}		
		else    //예외
		{
			kPacket << "" << 0 << 0 << 0;
		}
	}	
	pUser->SendMessage( kPacket );
	vSortList.clear();
}

void UserNodeManager::SendHeadquartersInviteList( User *pUser, int iCurPage, int iMaxCount, int iRoomLevel )
{
	if( pUser == NULL ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	static vSortUser vSortList;
	vSortList.clear();

	// 동일 서버
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pTargetUser = iter->second;
		if( pTargetUser == pUser ) continue;
		if( !pTargetUser->IsConnectState() ) continue;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->GetModeType() != MT_NONE && pTargetUser->GetModeType() != MT_TRAINING && pTargetUser->GetModeType() != MT_HEADQUARTERS ) continue;
		if( pTargetUser->GetMyRoom() && pTargetUser->GetMyRoom() == pUser->GetMyRoom() ) continue;
		if( pTargetUser->GetUserPos() == UP_BATTLE_ROOM || pTargetUser->GetUserPos() == UP_LADDER_TEAM ) continue;

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pTargetUser->GetKillDeathLevel() - iRoomLevel );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
		}        

		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	// 타서버
	uUserCopyNode_iter iterCopy,iEndCopy;
	iEndCopy = m_uUserCopyNode.end();
	for(iterCopy=m_uUserCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		UserCopyNode *pTargetUser = iterCopy->second;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->GetModeType() != MT_NONE && pTargetUser->GetModeType() != MT_TRAINING && pTargetUser->GetModeType() != MT_HEADQUARTERS  ) continue;
		if( pTargetUser->GetUserPos() == UP_BATTLE_ROOM || pTargetUser->GetUserPos() == UP_LADDER_TEAM ) continue;

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pTargetUser->GetKillDeathLevel() - iRoomLevel );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;	
		}
		
		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	int iMaxList = vSortList.size();
	if( iMaxList == 0 )
	{
		// 로비에 유저가 없으면 없다고 알려줘야 클라이언트 리스트에 남지 않는다.
		SP2Packet kPacket( STPK_HEADQUARTERS_INVITE_LIST );
		kPacket << 0 << 0;
		pUser->SendMessage( kPacket );
		return;
	}
	std::sort( vSortList.begin(), vSortList.end(), UserSort() );

	int iStartPos = iCurPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

	SP2Packet kPacket( STPK_HEADQUARTERS_INVITE_LIST );
	kPacket << iMaxList << iCurSize;
	for(int i = iStartPos; i < iStartPos + iCurSize;i++)
	{
		if( vSortList[i].m_pUser )
		{
			kPacket << vSortList[i].m_pUser->GetPublicID() << vSortList[i].m_pUser->GetGradeLevel() 
				<< vSortList[i].m_pUser->GetGuildIndex() << vSortList[i].m_pUser->GetGuildMark();
		}		
		else    //예외
		{
			kPacket << "" << 0 << 0 << 0;
		}
	}	
	pUser->SendMessage( kPacket );
	vSortList.clear();
}

void UserNodeManager::ConnectServerNodeSync( ServerNode *pServerNode )
{
	if( pServerNode == NULL ) return;
	
	LOOP_GUARD();
	static vUser vSyncUser;
	vSyncUser.clear();

	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;
		if( pUser->GetPublicID().IsEmpty() ) continue;

		vSyncUser.push_back( pUser );
	}
	LOOP_GUARD_CLEAR();

	// 오리지날 유저 정보만 N개씩 끊어서 전송
	LOOP_GUARD();
	while( true )
	{
		int iMaxSize = min( SSTPK_CONNECT_SYNC_USER_MAX, (int)vSyncUser.size() );
		if( iMaxSize == 0 )
			break;

		SP2Packet kPacket( SSTPK_CONNECT_SYNC );
		kPacket << SSTPK_CONNECT_SYNC_USER << iMaxSize;
		for(int i = 0;i < iMaxSize;i++)
		{
			User *pUser  = vSyncUser[0];
			pUser->FillUserLogin( kPacket );
			vSyncUser.erase( vSyncUser.begin() );
		}
		pServerNode->SendMessage( kPacket );
	}
	LOOP_GUARD_CLEAR();
}

void UserNodeManager::UpdateRelativeGradeAllUser( DWORD dwUniqueCode )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsUserOriginal() ) continue;
		if( !pUser->IsGeneralGrade() ) continue;
		
		pUser->UpdateRelativeGrade( dwUniqueCode );
	}
}

void UserNodeManager::SendPingUser( const ioHashString &szID )
{
	SP2Packet kPacket( STPK_PING );
	User *pNode = GetUserNodeByPrivateID( szID );
	if( pNode )
		pNode->SendMessage( kPacket );
}

void UserNodeManager::ViewUserNode()
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User List Start" );

	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( !pUser->IsConnectState() ) continue;
		if( pUser->GetPublicID().IsEmpty() ) continue;

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "IsActive(%d) - ID(%s) - Socket(%d) - SyncTime(%d)", 
								pUser->IsActive(), pUser->GetPublicID().c_str(),
								pUser->GetSocket(), TIMEGETTIME() - pUser->GetSyncTime() );
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User List End" );
}

bool UserNodeManager::IsDeveloper( const char *szID )
{
	// 개발자, 매니저, 마스터등 앞부분만 동일한 경우
	int iMax = m_vLikeDeveloperID.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( _strnicmp( szID, m_vLikeDeveloperID[i].c_str(), m_vLikeDeveloperID[i].Length() ) == 0 )
			return true;
	}

	// 100% 동일한 경우
	iMax = m_vDeveloperID.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( _stricmp( szID, m_vDeveloperID[i].c_str() ) == 0 )
			return true;
	}

	return false;
}

bool UserNodeManager::IsAfford()
{
	// 쾌적하기 위한 조건
	if( (GetNodeSize() + 32) >= GetMaxConnection() )
	{
		// 최소 32인이 들어갈 수 있는 공간이 남아있어야 한다
		return false;
	}
	return true;
}

void UserNodeManager::UpdateUserSync( User *pUser )
{
	LOOP_GUARD();
	UserSyncDataVec::iterator iter = m_UserSyncDataVec.begin();
	for(;iter != m_UserSyncDataVec.end();++iter)
	{
		UserSyncData &rkUser = *iter;
		if( rkUser.m_pUser == pUser )
		{
			LOOP_GUARD_CLEAR();
			return;
		}
	}
	LOOP_GUARD_CLEAR();
	
	UserSyncData kSyncData;
	kSyncData.m_dwSyncTime = TIMEGETTIME() + ( 1000 + rand() % max( 1, GetNodeSize() * 2 ) );
	kSyncData.m_pUser      = pUser;
	m_UserSyncDataVec.push_back( kSyncData );
}

void UserNodeManager::UserNode_DataSync()
{
	if( TIMEGETTIME() - m_dwNodeSyncTime < 50 ) return;
	
	m_dwNodeSyncTime = TIMEGETTIME();

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	LOOP_GUARD();
	DWORD dwCurTime = m_dwNodeSyncTime;
	UserSyncDataVec::iterator iter = m_UserSyncDataVec.begin();
	while( iter != m_UserSyncDataVec.end() )
	{
		UserSyncData &rkUserSync = *iter;
		if( rkUserSync.m_pUser == NULL ) 
		{
			iter = m_UserSyncDataVec.erase( iter );
		}
		else if( !rkUserSync.m_pUser->IsConnectState() )
		{
			iter = m_UserSyncDataVec.erase( iter );
		}
		else if( dwCurTime > rkUserSync.m_dwSyncTime )
		{
			rkUserSync.m_pUser->SyncUserUpdate();
			iter = m_UserSyncDataVec.erase( iter );
		}
		else
		{
			++iter;
		}
	}
	LOOP_GUARD_CLEAR();
}

void UserNodeManager::UserNode_GhostCheck()
{
#ifdef _ITEST
	return;
#endif
	if( TIMEGETTIME() - m_dwNodeGhostCheckTime < 10 ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	DWORD dwCurrentTime = TIMEGETTIME();

	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/	
	LOOP_GUARD();
	for( vUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter )
	{
		User *item = *iter;
		if( item->IsDisconnectState() ) continue;
		if( item->IsRelayUse() )
		{
			item->m_sync_time = dwCurrentTime; // 릴레이 사용중에는 싱크타임을 주기적으로 초기화 
			continue; // for relay 
		}
		if( item->GetSocket() == INVALID_SOCKET )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "이미 닫힌 소켓을 가진 노드 발견![%s]", item->GetPublicID().c_str() );
			item->m_sync_time = 1;
		}
		if( item->GetSyncTime() == 0 ) continue;

		if( dwCurrentTime - item->GetSyncTime() >= item->GetSyncCheckTime() )
		{
			if( item->IsServerMoving() )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserNode 서버 이동 유저가 응답이 없다. %s ", item->GetPublicID().c_str() );
				item->InitServerMovingValue();
			}

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%d분간 응답이 없어 접속 종료시킴(%s) : %d - %d", item->GetSyncCheckTime() / 60000, item->GetPublicID().c_str(), dwCurrentTime, item->GetSyncTime() );
			g_EtcLogMgr.PlusExceptionDisconnect();
			item->ExceptionClose( 0 );

			break;   // 한명씩 처리
		}
	}

	for( uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter )
	{
		User *item = iter->second;
		if( item->IsDisconnectState() ) continue;
		if( item->IsRelayUse() )
		{
			item->m_sync_time = dwCurrentTime; // 릴레이 사용중에는 싱크타임을 주기적으로 초기화 
			continue; // for relay 
		}
		if( item->GetSocket() == INVALID_SOCKET )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "이미 닫힌 소켓을 가진 노드 발견![%s]", item->GetPublicID().c_str() );
			item->m_sync_time = 1;
		}
		if( item->GetSyncTime() == 0 ) continue;

		if( dwCurrentTime - item->GetSyncTime() >= item->GetSyncCheckTime() )
		{
			if( item->IsServerMoving() )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserNode 서버 이동 유저가 응답이 없다. %s ", item->GetPublicID().c_str() );
				item->InitServerMovingValue();
			}

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%d분간 응답이 없어 접속 종료시킴(%s) : %d - %d", item->GetSyncCheckTime() / 60000, item->GetPublicID().c_str(), dwCurrentTime, item->GetSyncTime() );
			g_EtcLogMgr.PlusExceptionDisconnect();
			item->ExceptionClose( 0 );

			break;   // 한명씩 처리
		}
	}
	LOOP_GUARD_CLEAR();
	m_dwNodeGhostCheckTime = dwCurrentTime;
}

void UserNodeManager::UserNode_SaveCheck()
{
	if( TIMEGETTIME() - m_dwNodeSaveCheckTime < 10 ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/
	LOOP_GUARD();
	for( uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter )
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;

		if( TIMEGETTIME() - item->GetSaveTime() >= item->GetSaveCheckTime() ) 
		{
			item->SaveData();

			break;   // 한명씩 처리
		}

#ifdef OHTG_USER_BLOCK_DISCONNECT
		if( item->GetBlockType() == BlockType::BKT_BLOCK )
		{
			item->ExceptionClose( 0 );
			continue;
		}
#endif //OHTG_USER_BLOCK_DISCONNECT
	}
	LOOP_GUARD_CLEAR();
	m_dwNodeSaveCheckTime = TIMEGETTIME();
}

// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
void UserNodeManager::UserNode_CardMatchingTickProcess()
{
#ifdef CARD_MATCHING_BY_BCKIM	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가		// tick process // UserNode_CardMatchingTickProcess()

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	LOOP_GUARD();
	for( uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter )
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;
		if( !item->IsCardMatingState() ) continue;

		item->CardMatching_TickProcess();
	}
	LOOP_GUARD_CLEAR();
#endif  // CARD_MATCHING_BY_BCKIM
}
// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가

// SendBuffer
void UserNodeManager::ProcessFlush()
{
	for(vUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter)
	{
		User *item = *iter;
		if( !item || !item->IsConnectState() )		continue;
		if( item->GetSocket() == INVALID_SOCKET )	continue;

		item->FlushSendBuffer();
	}

	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( !item || !item->IsConnectState() )		continue;
		if( item->GetSocket() == INVALID_SOCKET )	continue;

		item->FlushSendBuffer();
	}
}

void UserNodeManager::SendMessageTest( SP2Packet &rkPacket, User *pOnwer /*= NULL*/ )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( pUser == pOnwer ) continue;

		pUser->SendMessage( rkPacket );
	}
}

#ifdef XTRAP
void UserNodeManager::UserNode_XtrapCheck()
{
	//if( TIMEGETTIME() - m_dwNodeXtrapCheckTime < 10 ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/	
	LOOP_GUARD();
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;
		if( IP_CLOUD == item->GetUserIPType() ) continue;

		// Xtrap 정보 확인
		if( TIMEGETTIME() - item->GetXtrapCheckTime() >= ioXtrap::CHECK_TIME )
		{
			if( !item->SendXtrapStep1() )
				item->ExceptionClose( 0 );

			//break;   // 한명씩 처리
		}		
	}
	LOOP_GUARD_CLEAR();
	m_dwNodeXtrapCheckTime = TIMEGETTIME();
}
#endif

#ifdef NPROTECT
void UserNodeManager::UserNode_NProtectCheck()
{
	if( TIMEGETTIME() - m_dwNodeNProtectCheckTime < 10 ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	DWORD dwCheckTime = 0;
	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/
	LOOP_GUARD();
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;

		dwCheckTime = ioNProtect::CHECK_TIME;
		if( item->GetSentNProtectCheckCnt() < 3 )
			dwCheckTime = ioNProtect::FIRST_CHECK_TIME;
		
		// 정보 확인
		if( TIMEGETTIME() - item->GetNProtectCheckTime() >= dwCheckTime )
		{
			if( !item->SendNProtectCheck() )
				item->ExceptionClose( 0 );

			break;   // 한명씩 처리
		}		
	}
	LOOP_GUARD_CLEAR();
	m_dwNodeNProtectCheckTime = TIMEGETTIME();
}
#endif

#ifdef HACKSHIELD
void UserNodeManager::UserNode_HackShieldCheck()
{
	if( TIMEGETTIME() - m_dwNodeHackShieldCheckTime < 10 ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	DWORD dwCheckTime = 0;
	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/	
	LOOP_GUARD();
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;

		// 정보 확인
		if( TIMEGETTIME() - item->GetHackShieldCheckTime() >= ioHackShield::CHECK_TIME )
		{
			if( !item->SendHackShieldCheck() )
				item->ExceptionClose( 0 );

			break;   // 한명씩 처리
		}		
	}
	LOOP_GUARD_CLEAR();
	m_dwNodeHackShieldCheckTime = TIMEGETTIME();
}
#endif
void UserNodeManager::UserNode_RefillMonsterCoin()
{
	if( TIMEGETTIME() - m_dwNodeRefillMonsterCoinTime < 30000 ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	DWORD dwSecode = ( TIMEGETTIME() - m_dwNodeRefillMonsterCoinTime ) / 1000;
	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/	
	LOOP_GUARD();
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;

		item->CheckRefillMonsterCoin( dwSecode );
	}
	LOOP_GUARD_CLEAR();
	m_dwNodeRefillMonsterCoinTime = TIMEGETTIME();
}

void UserNodeManager::UserNode_RefillClover()
{
	if( TIMEGETTIME() - m_dwNodeCloverCheckTime < 60000 )
		return;
	
	m_dwNodeCloverCheckTime = TIMEGETTIME();

	// 시간
	SYSTEMTIME st;
	GetLocalTime( &st );

	if( Help::GetCloverRefillTest() == true )
	{
		// 테스트.
		if( st.wMinute % Help::GetCloverRefillCycle() != 0 )
			return;
	}
	else
	{
		if( st.wHour != Help::GetCloverRefill_Hour() || st.wMinute != Help::GetCloverRefill_Min() )
			return;
	}
	
	// Get : 현재 시간.
	CTime CurrentTime = CTime::GetCurrentTime();

	// Declare) user

	LOOP_GUARD();
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User* pUser = iter->second;

		if( pUser->GetSyncTime() == 0 )
			continue;
		if( ! pUser->IsConnectState() )
			continue;

		pUser->CloverRefill( CurrentTime );
	}
	LOOP_GUARD_CLEAR();
}

void UserNodeManager::UserNode_RefillRaidCoin()
{
	if( TIMEGETTIME() - m_dwNodeRefillRaidTime < 30000 ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	DWORD dwSecode = ( TIMEGETTIME() - m_dwNodeRefillRaidTime ) / 1000;
	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/	
	LOOP_GUARD();
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;

		item->CheckRefillRaidTicket( false );
	}
	LOOP_GUARD_CLEAR();
	m_dwNodeRefillRaidTime = TIMEGETTIME();
}

void UserNodeManager::UserNode_TimeGashaponCheck()
{
	if( TIMEGETTIME() - m_dwNodeTimeGashaponCheckTime < 60000 ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/	
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;
		if( !item->IsConnectProcessComplete() ) continue;

		ioUserEtcItem *pUserEtcItem = item->GetUserEtcItem();
		if( pUserEtcItem )
		{
			pUserEtcItem->UpdateGashaponTime( 1 );
		}
	}
	m_dwNodeTimeGashaponCheckTime = TIMEGETTIME();
}

void UserNodeManager::UserNode_ShutDownCheck()
{
	if( !g_ShutDownMgr.IsActive() )
		return;

	if( TIMEGETTIME() - m_dwNodeShutDownCheckTime < 60000 ) return; // 1분

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/	
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;
		if( !item->IsShutDownUser() ) continue;

		SP2Packet kPacket( STPK_RESERVE_LOGOUT );
		kPacket << RESERVE_LOGOUT_SHUT_DOWN;
		item->SendMessage( kPacket );
	}
	m_dwNodeShutDownCheckTime = TIMEGETTIME();
}

void UserNodeManager::UserNode_SelectShutDownCheck()
{
	if( TIMEGETTIME() - m_dwNodeSelectShutDownCheckTime < 120000 ) return; // 2분

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	CTime CurrentTime = CTime::GetCurrentTime();

	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/	
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;

		ioUserSelectShutDown &rUserSelectShutDown = item->GetUserSelectShutDown();
		if( !rUserSelectShutDown.IsShutDown( CurrentTime ) ) continue;

		SP2Packet kPacket( STPK_RESERVE_LOGOUT );
		kPacket << RESERVE_LOGOUT_SELECT_SHUT_DOWN;
		item->SendMessage( kPacket );
	}
	m_dwNodeSelectShutDownCheckTime = TIMEGETTIME();
}

void UserNodeManager::UserNode_EventProcessTime()
{
	if( TIMEGETTIME() - m_dwNodeEventProcessTime < 60000 ) return; // 1분

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/	
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;

		item->EventProcessTime();
	}
	m_dwNodeEventProcessTime = TIMEGETTIME();
}


void UserNodeManager::UserNode_InitOakBarrelLimitSword()
{
	// 하루가 변경 시 일일 한도 수량 초기화
	//CTime curtime = CTime::GetCurrentTime();
	//CTimeSpan kInitTime = curtime - m_OakBarrelInitTime;
	//if( kInitTime.GetTotalHours() > 24 )
	//{
		//if( TIMEGETTIME() - m_dwNodeRefillOakBarrelSwordTime < 30000 ) return;

		FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

		//DWORD dwSecode = ( TIMEGETTIME() - m_dwNodeRefillOakBarrelSwordTime ) / 1000;
		/************************************************************************/
		/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
		/************************************************************************/	
		LOOP_GUARD();
		for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
		{
			User *item = iter->second;
			if( item->GetSyncTime() == 0 ) continue;
			if( !item->IsConnectState() ) continue;

			item->CheckRefillLimitSword();
		}
		LOOP_GUARD_CLEAR();
		//m_dwNodeRefillOakBarrelSwordTime = TIMEGETTIME();
		//m_OakBarrelInitTime = GetCurrentTime();
	//}
}

void UserNodeManager::UserNode_TimeGateCheck()
{
	
	//if( TIMEGETTIME() - m_dwNodeTimeGateCheckTime < g_TimeGateMgr.GetTimeGateCoolTime() ) return;

	//FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	///************************************************************************/
	///* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	///************************************************************************/	
	//for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	//{
	//	User *item = iter->second;
	//	if( item->GetSyncTime() == 0 ) continue;
	//	if( !item->IsConnectState() ) continue;
	//	if( !item->IsConnectProcessComplete() ) continue;

	//	// 시간이 되었으면 클라에 전송
	//	if( item->m_pMyRoom )
	//	{
	//		if( item->m_pMyRoom->GetModeType() == MT_NONE ) 
	//		{
	//			// 클라에 알림
	//		}
	//	}
	//}
	//m_dwNodeTimeGateCheckTime = TIMEGETTIME();
	
}

void UserNodeManager::UserNode_InitPractice()
{
	// 하루가 변경 시 일일 수련장 초기화

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	LOOP_GUARD();
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *item = iter->second;
		if( item->GetSyncTime() == 0 ) continue;
		if( !item->IsConnectState() ) continue;

		item->CheckInitPractice();
	}
	LOOP_GUARD_CLEAR();
}

/*
void UserNodeManager::UserNode_AllTimeExit()
{
	if( TIMEGETTIME() - m_dwNodeAllTime < 30000 ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	uUser_iter iter = m_uUserNode.begin();
	uUser_iter iter_Prev;
	
	while( iter != m_uUserNode.end() )
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if( item->GetSocket() == INVALID_SOCKET )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "이미 닫힌 소켓을 가진 노드 발견![%s]", item->GetPublicID().c_str() );
			item->m_sync_time = 1;
		}
		if( item->GetSyncTime() == 0 ) continue;
		
		if( TIMEGETTIME() - item->GetSyncTime() >= item->GetSyncCheckTime() )
		{
			if( item->IsServerMoving() )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserNode 서버 이동 유저가 응답이 없다. %s ", item->GetPublicID().c_str() );
				item->InitServerMovingValue();
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%d분간 응답이 없어 접속 종료시킴(%s) : %d - %d", item->GetSyncCheckTime() / 60000, item->GetPublicID().c_str(), TIMEGETTIME(), item->GetSyncTime() );
			g_EtcLogMgr.PlusExceptionDisconnect();
			item->ExceptionClose( 0 );
		}
		
		if( item->IsConnectState() )
		{
			 //30분마다 유저 정보 저장
			if( TIMEGETTIME() - item->GetSaveTime() >= item->GetSaveCheckTime() ) 
			{
				item->SaveData();
			}

			item->CheckRefillMonsterCoin( ( TIMEGETTIME() - m_dwNodeAllTime ) / 1000 );

			// Xtrap 정보 확인
			if( TIMEGETTIME() - item->GetXtrapCheckTime() >= ioXtrap::CHECK_TIME )
			{
				if( !item->SendXtrapStep1() )
					item->ExceptionClose( 0 );
			}
		}		
	}
	m_dwNodeAllTime = TIMEGETTIME();
}
*/
void UserNodeManager::UserNode_AllSave()
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;
		
		pUser->DeleteExerciseRentalCharAll();
        pUser->LogoutMemoryPresentInsert();         //메모리에 있는 선물 저장
		pUser->SaveData();
	}	
}

void UserNodeManager::UserNode_QuestRemove( DWORD dwMainIndex, DWORD dwSubIndex )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;

		ioQuest *pQuest = pUser->GetQuest();
		if( pQuest )
			pQuest->ClearQuestInfo( dwMainIndex, dwSubIndex );
	}	
}

void UserNodeManager::UserNode_QuestOneDayRemoveAll()
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;

		ioQuest *pQuest = pUser->GetQuest();
		if( pQuest )
			pQuest->ClearOneDayQuestCompleteAll();
	}	
}

void UserNodeManager::UserNode_MissionTargetTypeReset(IntVec& vResetTypeList)
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;

		ioMission *pUserMission = pUser->GetUserMission();
		if( pUserMission )
			pUserMission->InitMissionTypes(vResetTypeList);
	}	
}


void UserNodeManager::UserNode_AllHeroDataSync()
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;

		g_DBClient.OnSelectHeroData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
	}	
}

void UserNodeManager::SendMessageAll( SP2Packet &rkPacket, User *pOnwer /*= NULL */ )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( pUser == pOnwer ) continue;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsConnectProcessComplete() ) continue;
		
		pUser->SendMessage( rkPacket );		
	}
}

void UserNodeManager::SendCompensationToAllUser(const int iType, const int iCode, const int iValue)
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsConnectProcessComplete() ) continue;
		
		g_PresentHelper.InsertUserPresent(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), pUser->GetPublicID(), pUser->GetPublicIP(), pUser->GetUserIndex(), iType, iCode, iValue, false, false);
	}
}

void UserNodeManager::SendAllServerAlarmMent( SP2Packet &rkPacket )
{
	SendUDPMessageAll( rkPacket );

	SP2Packet kPacket( SSTPK_SERVER_ALARM_MENT_UDP );
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void UserNodeManager::SendUDPMessageAll( SP2Packet &rkPacket )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsConnectProcessComplete() ) continue;

		g_UDPNode.SendMessage( pUser->GetPublicIP(), pUser->GetUDP_port(), rkPacket );
	}
}

void UserNodeManager::SendUDPMessageCampUserAll( SP2Packet &rkPacket )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsConnectProcessComplete() ) continue;
		if( pUser->GetUserCampPos() == CAMP_NONE ) continue;

		g_UDPNode.SendMessage( pUser->GetPublicIP(), pUser->GetUDP_port(), rkPacket );
	}
}

void UserNodeManager::SendLobbyMessageAll( SP2Packet &rkPacket, User *pOwner /* = NULL  */ )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsConnectProcessComplete() ) continue;
		if( pUser == pOwner ) continue;
		if( !pUser->IsServerLobby() ) continue;

		pUser->SendMessage( rkPacket );		
	}
}

void UserNodeManager::SendLobbyUDPMessageAll( SP2Packet &rkPacket, User *pOwner /* = NULL  */ )
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;
		if( !pUser->IsConnectProcessComplete() ) continue;
		if( pUser == pOwner ) continue;
		if( !pUser->IsServerLobby() ) continue;

		g_UDPNode.SendMessage( pUser->GetPublicIP(), pUser->GetUDP_port(), rkPacket );
	}
}

void UserNodeManager::SendMessage( ioHashString &szID, SP2Packet &rkPacket )
{
	User *pUser = GetUserNodeByPublicID( szID );
	if( pUser )
	{
		pUser->SendMessage( rkPacket );
	}
}

void UserNodeManager::DisconnectNode( ioHashString &szID )
{
	User *pUser = GetUserNodeByPublicID( szID );
	if( !pUser ) //for User Index
	{
		pUser = GetUserNodeByPrivateID( szID );
	}
	if( !pUser ) //for User Index
	{
		pUser = GetUserNode(atoi(szID.c_str()));
	}
	if( pUser)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"Monitoring tool User Check(%s)", szID.c_str());
		if( !pUser->IsDisconnectState() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"Monitoring tool Close Socket (%s)", szID.c_str());
			pUser->CloseConnection();
		}
	}
	else
	{
		UserCopyNode *pUserCopy = GetUserCopyNodeByPublicID( szID );
		if( !pUserCopy ) //for User Index
		{
			pUserCopy = GetUserCopyNodeByPrivateID( szID );
		}
		if( !pUserCopy ) //for User Index
		{
			pUserCopy = GetUserCopyNode(atoi(szID.c_str()));
		}
		if( pUserCopy)
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"Monitoring tool UserCopy Check %s(%d)", szID.c_str(), pUserCopy->GetUserIndex());
			RemoveCopyUser(pUserCopy->GetUserIndex());
		}
	}
}

// DB Recv
bool UserNodeManager::GlobalQueryParse(SP2Packet &packet)
{
	CQueryResultData query_data;
	packet>>query_data;

	g_PacketChecker.QueryPacket( query_data.GetMsgType() );
	g_PacketChecker.PacketDBSizeCheck( query_data.GetMsgType(), packet.GetBufferSize() );

	switch(query_data.GetMsgType())
	{
	case DBAGENT_GAME_PINGPONG:
		OnResultPingPong( &query_data );
		return true;
	case DBAGENT_USER_COUNT_UPD:
		OnResultUpdateUserCount(&query_data);
		return true;
	case DBAGENT_USER_MOVE_SERVER_ID_UPD:
		OnResultUpdateUserMoveServerID(&query_data);
		return true;
	case DBAGENT_SERVER_ON_UPD:
		OnResultUpdateServerOn(&query_data);
		return true;
	case DBAGENT_USER_LOGIN_INFO_GET_UPD:
		OnResultSelectUpdateUserLoginInfo(&query_data);
		return true;
	case DBAGENT_USER_DATA_GET:
		OnResultSelectUserData(&query_data);
		return true;
	case DBAGENT_USER_DATA_UPD:
		OnResultUpdateUserData(&query_data);
		return true;
	case DBAGENT_USER_LADDER_POINT_UPD:
		OnResultUpdateUserLadderPoint(&query_data);
		return true;
	case DBAGENT_HERO_EXPERT_UPD:
		OnResultUpdateUserHeroExpert(&query_data);
		return true;
	case DBAGENT_HERO_EXPERT_INIT:
		OnResultInitUserHeroExpert(&query_data);
		return true;

	case DBAGENT_CONTROL_KEYS_GET:
		OnResultLoginSelectControlKeys( &query_data );
		return true;
	case DBAGENT_AWARD_DATA_GET:
		OnResultLoginSelectAllAwardData(&query_data);
		return true;
	case DBAGENT_AWARD_EXPERT_GET:
		OnResultLoginSelectAwardExpert(&query_data);		
		return true;
	case DBAGENT_CLASS_EXPERT_DATA_GET:
		OnResultLoginSelectAllClassExpert(&query_data);
		return true;
	case DBAGENT_USER_RECORD_GET:
		OnResultLoginSelectUserRecord(&query_data);
		return true;
	case DBAGENT_ETC_ITEM_DATA_GET:
		OnResultLoginSelectAllEtcItemData(&query_data);
		return true;
	case DBAGENT_EXTRAITEM_DATA_GET:
		OnResultLoginSelectAllExtraItemData(&query_data);
		return true;
	case DBAGENT_QUEST_COMPLETE_DATA_GET:
		OnResultLoginSelectAllQuestCompleteData(&query_data);
		return true;
	case DBAGENT_QUEST_DATA_GET:
		OnResultLoginSelectAllQuestData(&query_data);
		return true;
	case DBAGENT_CHAR_DATA_ALL_GET:
		OnResultLoginSelectAllCharData(&query_data);
		return true;
	case DBAGENT_INVEN_DATA_GET:
		OnResultLoginSelectAllInvenData(&query_data);
		return true;
	case DBAGENT_GROWTH_DATA_GET:
		OnResultLoginSelectAllGrowth(&query_data);
		return true;
	case DBAGENT_MEDALITEM_DATA_GET:
		OnResultLoginSelectAllMedalItemData(&query_data);
		return true;
	case DBAGENT_EXPAND_MEDAL_SLOT_DATA_GET:
		OnResultLoginSelectAllExMedalSlotData(&query_data);
		return true;
	case DBAGENT_EVENT_DATA_ALL_GET:
		OnResultLoginSelectAllEventData(&query_data);
		return true;
	case DBAGENT_ALCHEMIC_DATA_GET:
		OnResultLoginSelectAllAlchemicData(&query_data);
		return true;
	case DBAGENT_CHAR_DATA_SET:
		OnResultInsertCharData(&query_data);
		return true;
	case DBAGENT_CHAR_INDEX_GET:
		OnResultSelectCharIndex(&query_data);
		return true;
	case DBAGENT_CHAR_DATA_GET:
		OnResultSelectCharData(&query_data);
		return true;
	case DBAGENT_CHAR_DATA_UPD:
		OnResultUpdateCharData(&query_data);
		return true;
	case DBAGENT_CHAR_DATA_DEL:
		OnResultDeleteCharData(&query_data);
		return true;
	case DBAGENT_CHAR_DATE_LIMIT_DATE_DEL:
		OnResultDeleteCharLimitDate(&query_data);
		return true;
	case DBAGENT_CHAR_RENTAL_HISTORY_GET:
		OnResultSelectCharRentalHistory(&query_data);
		return true;
	case DBAGENT_CLASS_EXPERT_DATA_SET:
		OnResultInsertClassExpert(&query_data);		
		return true;
	case DBAGENT_CLASS_EXPERT_DATA_NEW_INDEX:
		OnResultSelectClassExpertIndex(&query_data);
		return true;
	case DBAGENT_CLASS_EXPERT_DATA_UPD:
		OnResultUpdateClassExpert(&query_data);
		return true;
	case DBAGENT_INVEN_DATA_SET:
		OnResultInsertInvenData(&query_data);
		return true;
	case DBAGENT_INVEN_DATA_CREATE_INDEX:
		OnResultSelectInvenIndex(&query_data);
		return true;
	case DBAGENT_INVEN_DATA_UPD:
		OnResultUpdateInvenData(&query_data);
		return true;
	case DBAGENT_AWARD_EXPERT_UPD:
		OnResultUpdateAwardExpert(&query_data);		
		return true;
	case DBAGENT_AWARD_DATA_SET:
		OnResultInsertAwardData(&query_data);
		return true;
	case DBAGENT_AWARD_DATA_CREATE_INDEX:
		OnResultSelectAwardIndex(&query_data);
		return true;
	case DBAGENT_AWARD_DATA_UPD:
		OnResultUpdateAwardData(&query_data);
		return true;
	case DBAGENT_GROWTH_DATA_SET:
		OnResultInsertGrowth(&query_data);		
		return true;
	case DBAGENT_GROWTH_DATA_NEW_INDEX:
		OnResultSelectGrowthIndex(&query_data);
		return true;
	case DBAGENT_GROWTH_DATA_UPD:
		OnResultUpdateGrowth(&query_data);
		return true;
	case DBAGENT_FISH_DATA_SET:
		OnResultInsertFishData(&query_data);		
		return true;
	case DBAGENT_FISH_DATA_NEW_INDEX:
		OnResultSelectFishDataIndex(&query_data);
		return true;
	case DBAGENT_FISH_DATA_GET:
		OnResultSelectAllFishData(&query_data);
		return true;
	case DBAGENT_FISH_DATA_UPD:
		OnResultUpdateFishData(&query_data);
		return true;
	case DBAGENT_EXTRAITEM_DATA_SET:
		OnResultInsertExtraItemData(&query_data);
		return true;
	case DBAGENT_EXTRAITEM_DATA_CREATE_INDEX:
		OnResultSelectExtraItemIndex(&query_data);
		return true;
	case DBAGENT_EXTRAITEM_DATA_UPD:
		OnResultUpdateExtraItemData(&query_data);
		return true;
	case DBAGENT_USER_LOGOUT_UPD:
		OnResultUpdateUserLogout(&query_data);
		return true;
	case DBAGENT_USER_ALL_LOGOUT_UPD:
		OnResultUpdateALLUserLogout(&query_data);
		return true;
	case DBAGENT_USER_LOGIN_INFO_GET:
		OnResultSelectUserLoginInfo(&query_data);
		return true;
	case DBAGENT_SERVER_INFO_DEL:
		OnResultDeleteGameServerInfo(&query_data);
		return true;
	case DBAGENT_SERVER_SECOND_KEY_GET:
		OnResultSelectSecondEncryptKey(&query_data);
		return true;
	case DBAGENT_ITEM_BUYCNT_SET:
		OnResultSelectItemBuyCnt(&query_data);
		return true;
	case DBAGENT_FRIEND_LIST_GET:
		OnResultSelectFriendList(&query_data);
		return true;
	case DBAGENT_FRIEND_REQUEST_LIST_GET:
		OnResultSelectFriendRequestList(&query_data);
		return true;
	case DBAGENT_FRIEND_APPLICATION_GET:
		OnResultSelectFriendApplication(&query_data);
		return true;
	case DBAGENT_FRIEND_INSERT_GET:
		OnResultSelectInsertFriend(&query_data);
		return true;
	case DBAGENT_FRIEND_USERID_CHECK_GET:
		OnResultSelectUserIDCheck(&query_data);		
		return true;
	case DBAGENT_FRIEND_DELETE_DEL:
		OnResultDeleteFriend(&query_data);
		return true;
	case DBAGENT_FRIEND_DEVELOPER_INSERT_GET:
		OnResultSelectFriendDeveloperInsert(&query_data);
		return true;
	case DBAGENT_BEST_FRIEND_LIST_GET:
		OnResultSelectBestFriendList(&query_data);
		return true;
	case DBAGENT_BEST_FRIEND_ADD_RESULT_GET:
		OnResultSelectBestFriendAdd(&query_data);
		return true;
	case DBAGENT_EVENT_DATA_UPD:
		OnResultUpdateEventData(&query_data);
		return true;
	case DBAGENT_USER_RECORD_UPD:
		OnResultUpdateUserRecord(&query_data);
		return true;

	case DBAGENT_QUEST_DATA_SET:
   		OnResultInsertQuestData(&query_data);
		return true;
	case DBAGENT_QUEST_DATA_CREATE_INDEX:
		OnResultSelectQuestIndex(&query_data);
		return true;
	case DBAGENT_QUEST_DATA_UPD:
		OnResultUpdateQuestData(&query_data);
		return true;

	case DBAGENT_QUEST_COMPLETE_DATA_SET:
		OnResultInsertQuestCompleteData(&query_data);
		return true;
	case DBAGENT_QUEST_COMPLETE_DATA_CREATE_INDEX:
		OnResultSelectQuestCompleteIndex(&query_data);
		return true;
	case DBAGENT_QUEST_COMPLETE_DATA_UPD:
		OnResultUpdateQuestCompleteData(&query_data);
		return true;

	case DBAGENT_CREATE_GUILD_GET:
		OnResultSelectCreateGuild(&query_data);
		return true;
	case DBAGENT_CREATE_GUILD_REG_GET:
		OnResultSelectCreateGuildReg(&query_data);
		return true;
	case DBAGENT_CREATE_GUILD_INFO_GET:
		OnResultSelectCreateGuildInfo(&query_data);
		return true;
	case DBAGENT_USER_GUILD_INFO_GET:
		OnResultSelectUserGuildInfo(&query_data);
		return true;
	case DBAGENT_ENTRY_DELAY_GUILD_LIST_GET:
		OnResultSelectEntryDelayGuildList(&query_data);
		return true;
	case DBAGENT_GUILD_ENTRY_DELAY_MEMBER_GET:
		OnResultSelectGuildEntryDelayMember(&query_data);
		return true;
	case DBAGENT_GUILD_MEMBER_LIST_GET:
		OnResultSelectGuildMemberList(&query_data);
		return true;
	case DBAGENT_GUILD_MEMBER_LIST_EX_GET:
		OnResultSelectGuildMemberListEx(&query_data);
		return true;
	case DBAGENT_GUILD_MARK_BLOCK_INFO:
		OnResultSelectGuildMarkBlockInfo(&query_data);
		return true;
	case DBAGENT_GUILD_NAME_CHANGE_GET:
		OnResultSelectGuildNameChange(&query_data);
		return true;
	case DBAGENT_GUILD_ENTRY_APP_GET:
		OnResultSelectGuildEntryApp(&query_data);
		return true;
	case DBAGENT_GUILD_ENTRY_APP_MASTER_GET:
		OnResultSelectGuildEntryAppMasterGet(&query_data);
		return true;
	case DBAGENT_GUILD_ENTRY_AGREE_GET:
		OnResultSelectGuildEntryAgree(&query_data);
		return true;
	case DBAGENT_GUILD_ENTRY_AGREE_USER_INFO_GET:
		OnResultSelectGuildEntryAgreeUserGuildInfo(&query_data);
		return true;
	case DBAGENT_GUILD_MASTER_CHANGE_UPD:
		OnResultUpdateGuildMasterChange(&query_data);
		return true;
	case DBAGENT_GUILD_POSITION_CHANGE_UPD:
		OnResultUpdateGuildPositionChange(&query_data);
		return true;
	case DBAGENT_GUILD_SIMPLE_DATA_GET:
		OnResultSelectGuildSimpleData(&query_data);
		return true;
	case DBAGENT_GUILD_MARK_CHANGE_KEY_VALUE_GET:
		OnResultSelectGuildMarkChangeKeyValue(&query_data);
		return true;
	case DBAGENT_GUILD_USER_LADDER_POINT_ADD_GET:
		OnResultSelectGuildUserLadderPointADD(&query_data);
		return true;
	case DBAGENT_CAMP_SEASON_BONUS_GET:
		OnResultSelectCampSeasonBonus(&query_data);
		return true;
	case DBAGENT_USER_ENTRY_GET:
		OnResultSelectUserEntry(&query_data);
		return true;
	case DBAGENT_USER_EXIST_GET:
		OnResultSelectUserExist(&query_data);
		return true;	
	case DBAGENT_CHAR_REG_DATE_UPD:
		OnResultUpdateCharRegDate(&query_data);
		return true;
	case DBAGENT_ETC_ITEM_DATA_SET:
		OnResultInsertEtcItemData(&query_data);
		return true;
	case DBAGENT_ETC_ITEM_DATA_CREATE_INDEX:
		OnResultSelectEtcItemIndex(&query_data);
		return true;
	case DBAGENT_ETC_ITEM_DATA_UPD:
		OnResultUpdateEtcItemData(&query_data);
		return true;
	case DBAGENT_PRESENT_DATA_GET:
		OnResultSelectPresentData(&query_data);
		return true;
	case DBAGENT_EVENT_INDEX_GET:
		OnResultSelectEventIndex(&query_data);
		return true;
	case DBAGENT_PUBLIC_ID_EXIST_GET:
		OnResultSelectPublicIDExist(&query_data);
		return true;
	case DBAGENT_CHANGED_PUBLIC_ID_GET:
		OnResultSelectChangedPublicID(&query_data);
		return true;
	case DBAGENT_BLOCK_TYPE_GET:
		OnResultSelectBlockType(&query_data);
		return true;
	case DBAGENT_TRIAL_SET:
		OnResultInsertTrial(&query_data);
		return true;
	case DBAGENT_MEMBER_COUNT_GET:
		OnResultSelectMemberCount( &query_data );
		return true;
	case DBAGENT_MEMBER_SET:
		OnResultInsertMember( &query_data );
		return true;
	case DBAGENT_FIRST_PUBLIC_ID_EXIST_GET:
		OnResultSelectFirstPublicIDExist(&query_data);
		return true;
	case DBAGENT_FIRST_PUBLIC_ID_CHANGED_ID_GET:
		OnResultSelectChangedFirstPublicID(&query_data);
		return true;
	case DBAGENT_USER_INDEX_AND_PRESENT_CNT_GET:
		OnResultSelectUserIndexAndPresentCnt(&query_data);
		return true;
	case DBAGENT_MEDALITEM_DATA_SET:
		OnResultInsertMedalItemData(&query_data);
		return true;
	case DBAGENT_MEDALITEM_DATA_CREATE_INDEX:
		OnResultSelectMedalItemIndex(&query_data);
		return true;
	case DBAGENT_MEDALITEM_DATA_UPD:
		OnResultUpdateMedalItemData(&query_data);
		return true;
	case DBAGENT_EXPAND_MEDAL_SLOT_DATA_SET:
		OnResultInsertExMedalSlotData(&query_data);
		return true;
	case DBAGENT_EXPAND_MEDAL_SLOT_DATA_CREATE_INDEX:
		OnResultSelectExMedalSlotIndex(&query_data);
		return true;
	case DBAGENT_EXPAND_MEDAL_SLOT_DATA_UPD:
		OnResultUpdateExMedalSlotData(&query_data);
		return true;
	case DBAGENT_HERO_DATA_GET:
		OnResultSelectHeroData(&query_data);
		return true;
	case DBAGENT_HERO_TOP100_GET:
		OnResultSelectHeroTop100Data(&query_data);
		return true;
	case DBAGENT_TRADE_CREATE:
		OnResultSelectCreateTrade(&query_data);
		return true;
	case DBAGENT_TRADE_CREATE_INDEX:
		OnResultSelectCreateTradeIndex(&query_data);
		return true;
	case DBAGENT_TRADE_COMPLETE:
		OnResultTradeItemComplete(&query_data);
		return true;
	case DBAGENT_TRADE_CANCEL:
		OnResultTradeItemCancel(&query_data);
		return true;
	case DBAGENT_ITEM_CUSTOM_UNIQUE_INDEX:
		OnResultSelectItemCustomUniqueIndex(&query_data);
		return true;
	case DBAGENT_TEST_SERVER_DELAY:
		OnResultDBServerTestLastQuery(&query_data);
		return true;
	case DBAGENT_HEADQUARTERS_DATA_COUNT:
		OnResultSelectHeadquartersDataCount(&query_data);
		return true;
	case DBAGENT_HEADQUARTERS_DATA_GET:
		OnResultSelectHeadquartersData(&query_data);
		return true;
	case DBAGENT_USER_BIRTH_DATE_GET:
		OnResultSelectUserBirthDate(&query_data);
		return true;
	case DBAGNET_FRIEND_RECOMMEND_GET:
		OnResultSelectFriendRecommendData(&query_data);
		return true;
	case DBAGENT_DISCONNECT_CHECK_GET:
		OnResultSelectDisconnectCheck(&query_data);
		return true;

		// 2020-06-05 by bckim, 하드시리얼 유저 제재
	case DBAGENT_DISCONNECT_HDD_SERIAL_CHECK_GET:		
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB DBAGENT_DISCONNECT_HDD_SERIAL_CHECK_GET");
		OnResultSelectDisconnectHddSerialCheck(&query_data);
		return true;
		// End. 2020-06-05 by bckim, 하드시리얼 유저 제재

	case DBAGENT_USER_SELECT_SHUT_DOWN_GET:
		OnResultSelectUserSelectShutDown(&query_data);
		return true;
	case DBAGENT_ALCHEMIC_DATA_SET:
		OnResultInsertAlchemicData(&query_data);
		return true;
	case DBAGENT_ALCHEMIC_DATA_CREATE_INDEX:
		OnResultSelectAlchemicIndex(&query_data);
		return true;
	case DBAGENT_ALCHEMIC_DATA_UPD:
		OnResultUpdateAlchemicData(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_CREATE_GET:
		OnResultInsertTournamentTeamCreate(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_INDEX_GET:
		OnResultSelectTournamentTeamIndex(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_LIST_GET:
		OnResultSelectTournamentTeamList(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_CREATE_TEAM_DATA_GET:
		OnResultSelectTournamentCreateTeamData(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_MEMBER_GET:
		OnResultSelectTournamentTeamMember(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_APP_LIST_GET:
		OnResultSelectTournamentTeamAppList(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_APP_ADD_SET:
		OnResultInsertTournamentTeamAppAdd(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_APP_REG_UPD:
		OnResultUpdateTournamentTeamAppReg(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_APP_AGREE_MEMBER:
		OnResultSelectTournamentTeamAppAgreeMember(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_APP_AGREE_TEAM:
		OnResultSelectTournamentAppAgreeTeam(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_TEAM_MEMBER_DEL:
		OnResultDeleteTournamentTeamMember(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_HISTORY_LIST:
		OnResultSelectTournamentHistoryList(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_HISTORY_USER_LIST:
		OnResultSelectTournamentHistoryUserList(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_REWARD_DATA:
		OnResultSelectTournamentReward(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_CUSTOM_DATA_ADD:
		OnResultInsertTournamentCustomAdd(&query_data);
		return true;
	case DBAGENT_TOURNAMENT_CUSTOM_REWARD_GET:
		OnResultSelectTournamentCustomReward(&query_data);
		return true;

	case DBAGENT_CLOVER_INFO_GET:
		OnResultSelectCloverInfoRequest( &query_data );
		return true;
	case DBAGENT_FRIEND_RECEIVE_CLOVER_SET:
		OnResultUpdateFriendReceiveCloverInfo( &query_data );
		return true;

	case DBAGENT_BINGO_NUMBER_GET:
		OnResultSelectBingoNumber( &query_data );
		return true;
	case DBAGENT_BINGO_PRESENT_GET:
		OnResultSelectBingoPresent( &query_data );
		return true;

	case DBAGENT_PRESENT_DATA_LOG_SET:
		OnResultInsertPresent( &query_data );
		return true;

	case DBAGENT_RELATIVE_GRADE_INFO_GET:
		OnResultSelectRelativeGradeInfo( &query_data );
		return true;

	case DBAGENT_TOURNAMENT_CHEER_DECISION:
		OnResultInsertTournamentCheerDecision( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_CHEER_LIST_GET:
		OnResultSelectTournamentCheerList( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_CHEER_REWARD_DATA:
		OnResultSelectTournamentCheerReward( &query_data );
		return true;
	case DBAGENT_SUBSCRIPTION_DATA_GET:
		OnResultSelectSubscriptionData(&query_data);
		return true;
	case DBAGENT_ATTENDANCE_LIST_GET:
		OnResultSelectAttendanceRecord(&query_data);
		return true;
	case DBAGENT_PET_DATA_GET:
		OnResultLoginSelectAllPetItemData( &query_data );
		return true;
	case DBAGENT_PET_DATA_SET:
		OnResultInsertPetData( query_data );
		return true;
	case DBAGENT_PET_DATA_UPD:
		OnResultUpdatePetData( &query_data );
		return true;
	case DBAGENT_COSTUME_DATA_GET:
		OnResultLoginSelectCostumeData(&query_data);
		return true;
	case DBAGENT_COSTUME_DATA_INSERT:
		OnResultInsertCostumeData(&query_data);
		return true;
	case DBAGENT_COSTUME_DATA_UPDATE:
		OnResultUpdateCostumeData(&query_data);
		return true;
	case DBAGENT_COSTUME_DATA_DELETE:
		OnResultDeleteCostumeData(&query_data);
		return true;
	case DBAGENT_ACCESSORY_DATA_UPDATE:
		OnResultDeleteAccessoryData(&query_data);
		return true;
	case DBAGENT_ACCESSORY_DATA_INSERT:
		OnResultInsertAccessoryData(&query_data);
		return true;
	case DBAGENT_MISSION_DATA_GET:
		OnResultGetMissionData(&query_data);
		return true;
	case DBAGENT_MISSION_DATA_UPDATE:
		OnResultUpdateMissionData(&query_data);
		return true;
	case DBAGENT_MISSION_DATA_INIT:
		OnResultInitMissionData(&query_data);
		return true;

	case DBAGENT_ROLLBOOK_DATA_GET:
		OnResultGetRollBookData(&query_data);
		return true;
	case DBAGENT_ROLLBOOK_DATA_UPDATE:
		OnResultUpdateRollBookData(&query_data);
		return true;
	case DBAGNET_PRESENT_DEL:
		OnResultDeletePresent(&query_data);
		return true;
	case DBAGENT_GUILD_ATTENDANCE_MEMBER_GET:
		OnResultGetGuildAttendaceMemberGet(&query_data);
		return true;
	case DBAGENT_GUILD_USER_ATTENDANCE_INFO_INSERT:
		OnResultInsertUserGuildAttendanceInfo(&query_data);
		return true;
	case DBAGENT_PRESENT_ADD_BY_PRIVATE:
		OnresultInsertPresentByPrivateID(&query_data);
		return true;
	case DBAGENT_GAME_SPENT_MONEY_GET:
		OnResultSelectGetSpentMoney( &query_data);
		return true;
	case DBAGENT_GAME_POPUP_INDEX_GET:
		OnResultSelectGetPopupIndex( &query_data);
		return true;
	case DBAGENT_USER_CHANNELING_KEY_VALUE_GET:
		OnResultSelectUserChannelingKeyValue( &query_data );
		return true;
	case DBAGENT_GUILD_BLOCK_INFOS_GET:
		OnResultSelectGuildBlockInfos( &query_data );
		return true;
	case DBAGENT_GUILD_BLOCK_RETRIEVE_DELETE:
		OnResultGuildBlockRetrieveORDelete( &query_data );
		return true;
	case DBAGENT_GUILD_BLOCK_CONSTRUCT_MOVE:
		OnResultGuildBlockConstructORMove( &query_data );
		return true;
	case DBAGENT_GUILD_INVEN_VERSION_GET:
		OnResultSelectGuildInvenVersion( &query_data );
		return true;
	case DBAGENT_GUILD_INVEN_INFO_GET:
		OnResultSelectGuildInvenInfo( &query_data );
		return true;
	case DBAGENT_GUILD_BLOCK_ADD:
		OnResultAddGuildBlockItem( &query_data );
		return true;
	case DBAGENT_GUILD_BLOCK_DEFAULT_CONSTRUCT:
		OnResultDefaultConstructGuildBlock( &query_data );
		return true;
	case DBAGENT_PERSONAL_HQ_CONSTRUCT:
		OnResultPersonalHQConstruct( &query_data );
		return true;
	case DBAGENT_PERSONAL_HQ_RETRIEVE:
		OnResultPersonalHQRetrieve( &query_data );
		return true;
	case DBAGENT_PERSONAL_HQ_IS_EXIST:
		OnResultPersonalHQIsExist( &query_data );
		return true;
	case DBAGENT_PERSONAL_HQ_INVEN_INFO_GET:
		OnResultLoginSelectPersonalHQInvenData( &query_data );
		return true;
	case DBAGENT_PERSONAL_HQ_BLOCKS_INFO_GET:
		OnResultPersonalHQBlocksInfo( &query_data );
		return true;
	case DBAGENT_PERSONAL_HQ_BLOCK_ADD:
		OnResultPersonalHQBlockAdd( &query_data );
		return true;
	case DBAGENT_TIME_CASH_TABLE_GET:
		OnResultSelectTimeCashTable( &query_data );
		return true;
	case DBAGENT_TIME_CASH_TABLE_INSERT:
		OnResultInsertTimeCashTable( &query_data );
		return true;
	case DBAGENT_TIME_CASH_TABLE_UPDATE:
		OnResultUpdateTimeCashTable( &query_data );
		return true;
	case DBAGENT_TITLE_UPDATE_STATUS:
		OnResultUpdateStatus( &query_data );
		return true;
	case DBAGENT_TITLE_INSERT_OR_UPDATE:
		OnResultInsertOrUpdate( &query_data );
		return true;
	case DBAGENT_TITLE_SELECT_TITLE:
		OnResultSelectTitle( &query_data ); 
		return true;

	case DBAGENT_PIRATEROULETTE_NUMBER_GET:
		OnResultSelectPirateRouletteNumber( &query_data );
		return true;
	case DBAGENT_PIRATEROULETTE_PRESENT_GET:
		OnResultSelectPirateRoulettePresent( &query_data );
		return true;
	case DBAGENT_ACCESSORY_DATA_GET:
		OnResultLoginSelectAccessoryData(&query_data);
		return true;

	case DBAGENT_BONUS_CASH_INSERT:
		OnResultInsertBonusCash(&query_data);
		return true;
	case DBAGENT_BONUS_CASH_UPDATE:
		OnResultUpdateBonusCash(&query_data);
		return true;
	case DBAGENT_BONUS_CASH_SELECT:
		OnResultSelectBonusCash(&query_data);
		return true;
	case DBAGENT_EXPIRED_BONUS_CASH_SELECT:
		OnResultSelectExpiredBonusCash(&query_data);
		return true;

	case DBAGENT_USERCOIN_SELECT:
		OnResultLoginSelectUserCoin(&query_data);
		return true;
	case DBAGENT_USERCOIN_INSERT:
		OnResultInsertUserCoin(&query_data);
		return true;
	case DBAGENT_USERCOIN_UPDATE:
		OnResultUpdateUserCoin(&query_data);
		return true;

#ifdef CARD_MATCHING_BY_BCKIM	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가			// DBAGENT_CARD_MATCHING
	case DBAGENT_CARD_MATCHING_DATA_SELECT:
		OnResultSelectCardMatching( &query_data );
		return true;		
#endif  // CARD_MATCHING_BY_BCKIM
#ifdef DICE_GAME_BY_BCKIM			// 2018-08-30 by bckim, 주사위 이벤트 추가
	case DBAGENT_DICE_GAME_DATA_SELECT:
		OnResultSelectDiceGame( &query_data );
		return true;
#endif // DICE_GAME_BY_BCKIM			// 2018-08-30 by bckim, 주사위 이벤트 추가

	// 로그인 시 받아오는 오크통 정보
	case DBAGENT_OAK_BARREL_DATA_SELECT:
		OnResultSelectOakBarrel( &query_data );
		return true;
	// 업데이트 되어 반환된 오크통 정보
	case DBAGENT_OAK_BARREL_DATA_UPDATE:
		OnResultUpdateOakBarrel( &query_data );
		return true;
	// 오크통 시간값 갱신
	case DBAGENT_OAK_BARREL_TIME_UPDATE:
		OnResultTimeUpdateOakBarrel( &query_data );
		return true;
	// 오크통 칼 일일 사용 한도 수량 초기화 업데이트
	case DBAGENT_OAK_BARREL_LIMIT_SWORD_INIT:
		OnResultLimitSwordUpdateOakBarrel( &query_data );
		return true;
	case DBAGENT_USER_MATCH_DATA_SELECT:
		OnResultSelectUserMatch( &query_data );
		return true;
	case DBAGENT_USER_MATCH_DATA_UPDATE:
		OnResultUpdateUserMatch( &query_data );
		return true;
	case DBAGENT_USER_SPIRIT_SELECT:
		OnResultSelectUserSpiritData( &query_data );
		return true;
	case DBAGENT_USER_SPIRIT_INSERT:
		OnResultInsertUserSpiritData( &query_data );
		return true;
	case DBAGENT_USER_SPIRIT_UPDATE:
		OnResultUpdateUserSpiritData( &query_data );
		return true;
	case DBAGENT_USER_CUSTOM_MEDAL_SELECT:
		OnResultSelectUserCustomMedalData( &query_data );
		return true;
	case DBAGENT_USER_CUSTOM_MEDAL_INSERT:
		OnResultInsertUserCustomMedalData( &query_data );
		return true;
	case DBAGENT_USER_CUSTOM_MEDAL_DELETE:
		return true;

	case DBAGENT_USER_CUSTOM_MEDAL_STATUS_UPDATE:

		return true;
	case DBAGENT_USER_TIME_GATE_SELECT:
		OnResultSelectUserTimeGateData( &query_data );
		return true;
	case DBAGENT_USER_TIME_GATE_UPDATE:
		OnResultUpdateUserTimeGateData( &query_data );
		return true;

	// 수련장
	case DBAGENT_PRACTICE_LIST_GET:
		OnResultSelectPracticeList( &query_data );
		return true;
	case DBAGENT_PRACTICE_SET_ADMISSION:
		OnResultUpdatePracticeAdmission( &query_data );
		return true;
	case DBAGENT_PRACTICE_SET_TIME:
		OnResultUpdatePracticeTime( &query_data );
		return true;
	case DBAGENT_PRACTICE_INIT_DATA:
		OnResultInitPracticeData( &query_data );
		return true;
	case DBAGENT_PRACTICE_RANK_LIST_GET:
		OnResultPracticeRankList( &query_data );
		return true;

	case DBAGENT_PCBANG_PLAY_TIME:
		OnResultPCBangPlayTime( &query_data );
		return true;
	}

	return false;
}

void UserNodeManager::OnResultPingPong( CQueryResultData *query_data )
{
	DWORD dwLastPing;
	query_data->GetValue( dwLastPing, sizeof(DWORD) );

	DWORD dwElapse = (TIMEGETTIME() - dwLastPing) / 2;

	g_App.SetDBQueryTime( dwElapse );
}

void UserNodeManager::OnResultUpdateUserCount( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserCount Result FAILED! :%d",query_data->GetResultType());
		g_App.Shutdown(0, 21);
		return;
	}
	g_App.SetDBQueryTime( TIMEGETTIME() - g_App.GetDBQuerySendTime() );
}

void UserNodeManager::OnResultUpdateUserMoveServerID( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserMoveServerID Result FAILED! :%d",query_data->GetResultType());
		g_App.Shutdown(0, 22);
		return;
	}
}

void UserNodeManager::OnResultUpdateServerOn( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateServerOn Result FAILED! :%d",query_data->GetResultType());
		g_App.Shutdown(0, 23);
		return;
	}
}

void UserNodeManager::OnResultDeleteGameServerInfo(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultDeleteGameServerInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	// exit program.
	g_App.Exit();
}

void UserNodeManager::OnResultSelectSecondEncryptKey( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectSecondEncryptKey Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	//SELECT
	char szDbSecondKey[LOGIN_KEY_PLUS_ONE] = "";
	query_data->GetValue(szDbSecondKey,LOGIN_KEY_PLUS_ONE);
	ioHashString szHashDbSecondKey = szDbSecondKey;

	if(szHashDbSecondKey.IsEmpty())
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"Error - OnResultSelectSecondEncryptKey - second key is empty");
		return;
	}

	if(szHashDbSecondKey.Length() >= SEED_USER_KEY_LEN)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"Error - OnResultSelectSecondEncryptKey - second key is NULL");
		return;
	}
	g_App.SetSecondKey(szHashDbSecondKey);
}


void UserNodeManager::OnResultSelectUserLoginInfo( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserLoginInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	//보낸 유저의 아이디.
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szUserID[ID_NUM_PLUS_ONE] = "";

	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	PACKET_GUARD_VOID( query_data->GetValue(szUserID,ID_NUM_PLUS_ONE) );

	User *pUser = GetUserNodeByGUID( szUserGUID );
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserLoginInfo USER FIND NOT! :%s : %s",szUserID, szUserGUID);
		return;
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	//SELECT
	char szDbID[ID_NUM_PLUS_ONE] = "";
	char szDbLoginKey[LOGIN_KEY_PLUS_ONE] = "";
	__int64  iDbServerID = 0;
	DBTIMESTAMP dts;
#ifdef __OHTG_LOGIN_IP_CHECK__
	char szConnectIP[LOGIN_KEY_PLUS_ONE] = "";
#endif //__OHTG_LOGIN_IP_CHECK__

	//PACKET_GUARD_VOID( query_data->GetValue(szDbID,ID_NUM_PLUS_ONE) );
	if( !query_data->GetValue(szDbID,ID_NUM_PLUS_ONE) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"OnResultSelectUserLoginInfo UserPrivateID size OVER Error %s %s", szUserID, szUserGUID );
		return;
	}
	PACKET_GUARD_VOID( query_data->GetValue(szDbLoginKey,LOGIN_KEY_PLUS_ONE) );
	PACKET_GUARD_VOID( query_data->GetValue(iDbServerID,sizeof(__int64)) );
	PACKET_GUARD_VOID( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );
#ifdef __OHTG_LOGIN_IP_CHECK__
	PACKET_GUARD_VOID( query_data->GetValue(szConnectIP,LOGIN_KEY_PLUS_ONE) );
#endif //__OHTG_LOGIN_IP_CHECK__

	// calculate time
	CTime keyCreateTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
	CTime current_time = CTime::GetCurrentTime();
	CTimeSpan gaptime;
	gaptime = current_time - keyCreateTime;

	CRASH_GUARD();
	//check error
	char szPlain[DATA_LEN]="";
	int result = CONNECT_OK;
	
	char szUserIp[STR_IP_MAX];
	int iUserPort = 0;
	
	pUser->GetPeerIP(szUserIp,STR_IP_MAX,iUserPort);
	char szTestIP[LOGIN_KEY_PLUS_ONE] = "";

	StringCbPrintf( szTestIP, sizeof(szTestIP), "%s", pUser->GetPrivateIP() );

	if( pUser->GetPrivateID() != szDbID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserLoginInfo USER ID NOT :%s-%s (%s:%d)",pUser->GetPrivateID().c_str(), szDbID, szUserIp,iUserPort);
		result = CONNECT_ID_NOT;
	}
#ifdef __OHTG_LOGIN_IP_CHECK__
	else if( strcmp( szConnectIP, szTestIP) != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserLoginInfo USER IP NOT1 :%s-%s (%s:%s)",pUser->GetPrivateID().c_str(), szDbID, szConnectIP,szTestIP);
		result = CONNECT_EXCEPT;
	}
	else if( strcmp( szUserIp, szTestIP ) != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserLoginInfo USER IP NOT2 :%s-%s (%s:%s)",pUser->GetPrivateID().c_str(), szDbID, szUserIp,szTestIP);
	}
#endif //__OHTG_LOGIN_IP_CHECK__
	else if( iDbServerID != 0)
	{
		result = CONNECT_ID_ALREADY;
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Error ID ALREADY :%s (%s:%d)",__FUNCTION__, pUser->GetPrivateID().c_str(), szUserIp,iUserPort);
	}
	else if( pUser->GetEncLoginKey() == NULL)
	{
		result = CONNECT_EXCEPT;
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : NULL :%s (%s:%d)",__FUNCTION__,pUser->GetPrivateID().c_str(), szUserIp,iUserPort);
	}
	else if( pLocal == NULL )
	{
		result = CONNECT_EXCEPT;
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : pLocal == NULL :%s (%s:%d)",__FUNCTION__,pUser->GetPrivateID().c_str(), szUserIp,iUserPort);
	}
#ifndef _ITEST
	if( !g_App.IsLocalTest() )
	{
		if( !pLocal->IsRightTimeLoginKey( gaptime.GetTotalMinutes() ) ) // MOLPHIN
  		{
  			result = CONNECT_EXPIRE_KEY;
  			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Error Expire Key :%s(%s:%d)-%d",__FUNCTION__, pUser->GetPrivateID().c_str(), szUserIp,iUserPort  ,gaptime.GetTotalMinutes());
  		}
  		else if( !pLocal->DecryptLoginKey( pUser->GetEncLoginKey()->c_str(), szDbLoginKey, szPlain, sizeof( szPlain ) ) )
  		{
  			result = CONNECT_EXCEPT;
  			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Error Decode :%s-%s-%s (%s:%d)",__FUNCTION__, pUser->GetPrivateID().c_str()
  								  ,pUser->GetEncLoginKey()->c_str(), szDbLoginKey, szUserIp,iUserPort);
  		}
  		else if( !pLocal->IsRightLoginKey( szDbLoginKey, szPlain ) )
  		{
  			result = CONNECT_PW_NOT;
  			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, ":%s : Error PW :%s-%s-%s (%s:%d)",__FUNCTION__, pUser->GetPrivateID().c_str()
  								  ,szDbLoginKey, szPlain, szUserIp,iUserPort);
  		}
	}
#endif
	pUser->ClearEncLoginKey();

	// send error
	if(result != CONNECT_OK)
	{
		SP2Packet kPacket( STPK_CONNECT );

		PACKET_GUARD_VOID (kPacket.Write(result) );
		PACKET_GUARD_VOID (kPacket.Write(pUser->GetPrivateID()) );

		pUser->SendMessage( kPacket );

		pUser->SetPrivateID( "" );
		return;
	}
	g_DBClient.OnSelectUpdateUserLoginInfo( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPrivateID() ,g_App.GetGameServerID() ); 
}

void UserNodeManager::OnResultSelectUpdateUserLoginInfo( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUpdateUserLoginInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	//보낸 유저의 아이디.
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szUserID[ID_NUM_PLUS_ONE] = "";
	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	PACKET_GUARD_VOID( query_data->GetValue(szUserID,ID_NUM_PLUS_ONE) );
	User *pUser = GetUserNodeByGUID( szUserGUID );
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUpdateUserLoginInfo USER FIND NOT! :%s :%s",szUserID, szUserGUID);
		return;
	}

	//SELECT
	char szDbID[ID_NUM_PLUS_ONE] = "";
	__int64  iDbServerID = 0;
	PACKET_GUARD_VOID( query_data->GetValue(szDbID,ID_NUM_PLUS_ONE) );
	PACKET_GUARD_VOID( query_data->GetValue(iDbServerID,sizeof(__int64)) );


	//check error
	int result = CONNECT_OK;
	if( pUser->GetPrivateID() != szDbID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUpdateUserLoginInfo USER ID NOT :%s-%s"
			                ,pUser->GetPrivateID().c_str(), szDbID );
		result = CONNECT_ID_NOT;
	}
	else if( iDbServerID != 0)
	{
		result = CONNECT_ID_ALREADY;
	}

	// send error
	if(result != CONNECT_OK)
	{		
		SP2Packet kPacket( STPK_CONNECT );

		PACKET_GUARD_VOID( kPacket.Write(result) );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetPrivateID()) );

		pUser->SendMessage( kPacket );
		pUser->SetPrivateID("");
		return;
	}

	ioLocalParent *pLocal =  g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsRunUserShutDown() )
	{
		g_DBClient.OnSelectUserSelectShutDown( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPrivateID() );
	}
	else
	{
		g_DBClient.OnSelectUserData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPrivateID() );      
	}	
}

void UserNodeManager::OnResultSelectItemBuyCnt(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectItemBuyCnt Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	static boost::unordered_map<int, int> mClassPrice;
	mClassPrice.clear();

	/*int iClassSetPeso[ioItemPriceManager::DB_LOAD_COUNT];
	memset( iClassSetPeso, 0, sizeof(iClassSetPeso) );

	for(int i = 0;i < ioItemPriceManager::DB_LOAD_COUNT;i++)
	{
		query_data->GetValue(iClassSetPeso[i],sizeof(int));
	}*/

	LOOP_GUARD();
	while( query_data->IsExist() )
	{
		int iClass	= 0;
		int iPrice	= 0;

		query_data->GetValue(iClass,sizeof(int));
		query_data->GetValue(iPrice,sizeof(int));

		mClassPrice.insert( make_pair(iClass, iPrice) );
	}

	g_ItemPriceMgr.SetWeekBuyCntAndPeso( mClassPrice );	
}

void UserNodeManager::OnResultSelectUserData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szUserID[ID_NUM_PLUS_ONE] = "";
	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	PACKET_GUARD_VOID( query_data->GetValue(szUserID,ID_NUM_PLUS_ONE) );       //보낸 유저의 아이디.
	User *pUser = GetUserNodeByGUID( szUserGUID );
	if(pUser == NULL)
	{
		UserBot* pNode = static_cast<UserBot*>( g_TestNodeMgr.GetNode( szUserGUID ) );
		if( pNode )
		{
			pNode->ApplyDBUserData( query_data );
			return;
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserData USER FIND NOT! :%s :%s",szUserID, szUserGUID);
		return;
	}

	//SELECT
	int  user_idx = 0;
	char szDBID[ID_NUM_PLUS_ONE] = "";
	char nick_name[ID_NUM_PLUS_ONE] = "";
	PACKET_GUARD_VOID( query_data->GetValue(user_idx,sizeof(int)) );
	PACKET_GUARD_VOID( query_data->GetValue(szDBID,ID_NUM_PLUS_ONE) );
	PACKET_GUARD_VOID( query_data->GetValue(nick_name,ID_NUM_PLUS_ONE) );

	int result = CONNECT_OK;
	if( pUser->GetPrivateID() != szDBID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserData USER ID NOT :%s - %s",
								pUser->GetPrivateID().c_str(), szDBID );
		result = CONNECT_ID_NOT;
	}

	if( result == CONNECT_OK && user_idx <= 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserData USER INDEX EXCPET :%d",user_idx);
		result = CONNECT_EXCEPT;
	}

	if( result == CONNECT_OK && GetUserNode( user_idx ) != NULL )
	{
		result = CONNECT_ID_ALREADY;	//같은 INDEX를 가진 유저 존재
	}

	if( result == CONNECT_OK )
	{
		//데이터 추출.
		USERDATA user_data;
		user_data.m_user_idx    = user_idx;
		user_data.m_private_id  = szDBID;
		user_data.m_public_id   = nick_name;
		
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_user_state,sizeof(int)) );
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_money,sizeof(__int64)) );
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_connect_count,sizeof(int)) );
		
		//최종 접속 시간
		DBTIMESTAMP dts;
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );
		if( user_data.m_connect_count == 0 )
		{
			// 첫 접속 유저는 현재 시간 
			user_data.m_connect_time = CTime::GetCurrentTime();
		}
		else
		{
			CTime connect_time(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
			user_data.m_connect_time = connect_time;
		}

		//계급
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_grade_level,sizeof(int)) );

		//계급 경험치
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_grade_exp,sizeof(int)) );
		
		//유저 이벤트 타입
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_user_event_type,sizeof(int)) );

		//유저 랭킹
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_user_rank,sizeof(int)) );

		// 첫 접속
		bool bFirstLoginUser = false;
		if( user_data.m_connect_count == 0 )
		{
			user_data.m_connect_count = 1;
			user_data.m_money = Help::GetFirstConnectBonusPeso();		
			bFirstLoginUser   = true;
		}
		else
		{
			//금일 처음 접속 확인.
			CTime curTime = CTime::GetCurrentTime();
			DWORD dwCurTime		= (curTime.GetYear()*10000) + (curTime.GetMonth() * 100) + curTime.GetDay();
			DWORD dwConnectTIme = (user_data.m_connect_time.GetYear()*10000) + (user_data.m_connect_time.GetMonth() * 100) + user_data.m_connect_time.GetDay();
			if( dwCurTime != dwConnectTIme )
			{
				user_data.m_connect_count++;
			}
		}

		// 가입타입
		short iEntryType=0;
		PACKET_GUARD_VOID( query_data->GetValue(iEntryType,sizeof(short)) ); // db smallint
		user_data.m_eEntryType = (EntryType) iEntryType;
		
		// 유저가입일
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );
		CTime entry_time(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));

		// 진영 타입
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_camp_position,sizeof(int)) );

		//누적 래더 포인트
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_accumulation_ladder_point,sizeof(int)) );

		//시즌 래더 포인트
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_ladder_point,sizeof(int)) );
		
		//진영 랭킹
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_camp_rank,sizeof(int)) );

		//채널링
		short iChannelingType = CNT_NONE;
		PACKET_GUARD_VOID( query_data->GetValue( iChannelingType, sizeof( short ) ) ); // db smallint
		user_data.m_eChannelingType = (ChannelingType) iChannelingType;

#ifdef WEMADE_CHANNELING_BLOCK_USER_BY_YHSEO
		if ( user_data.m_eChannelingType == CNT_WEMADEBUY )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][WEMADE_CHANNELING_BLOCK_USER_BY_YHSEO]: [%d] [%s] [%s] [%d]", 
				user_data.m_user_idx, user_data.m_private_id.c_str(), user_data.m_public_id.c_str(), (int)iChannelingType );

			SP2Packet kPacket( STPK_CONNECT );
			PACKET_GUARD_VOID( kPacket.Write( CONNECT_EXCEPT ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPrivateID() ) );
			pUser->SendMessage( kPacket );
			pUser->SetPrivateID("");
			return;
		}
#endif
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && pLocal->IsChannelingID() )
		{
			char szChannelingUserID[CHANNELING_USER_ID_NUM_PLUS_ONE]="";
			PACKET_GUARD_VOID( query_data->GetValue( szChannelingUserID, CHANNELING_USER_ID_NUM_PLUS_ONE ) );
			user_data.m_szChannelingUserID = szChannelingUserID;
			char szChannelingUserNo[CHANNELING_USER_NO_NUM_PLUS_ONE]="";
			PACKET_GUARD_VOID( query_data->GetValue( szChannelingUserNo, CHANNELING_USER_NO_NUM_PLUS_ONE ) );
			user_data.m_szChannelingUserNo = szChannelingUserNo;
		}

		//차단
		short nBlockType = BKT_NONE;
		PACKET_GUARD_VOID( query_data->GetValue( nBlockType, sizeof( short ) ) ); // db smallint
		user_data.m_eBlockType = (BlockType) nBlockType;

		PACKET_GUARD_VOID( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );
		CTime kBlockTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
		user_data.m_kBlockTime = kBlockTime;

		// 차단날짜종료확인
		if( user_data.m_eBlockType != BKT_NORMAL )
		{
			CTime current_time = CTime::GetCurrentTime();
			CTimeSpan gaptime;
			gaptime = kBlockTime - current_time;
			if( gaptime.GetTotalMinutes() <= 0 )
				user_data.m_eBlockType = BKT_NORMAL;
		}

		//낚시 레벨
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_fishing_level,sizeof(int)) );

		//낚시 경험치
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_fishing_exp,sizeof(int)) );

		//(리필받기까지 남은 초
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_refill_data,sizeof(int)) );

		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_iExcavationLevel,sizeof(int)) );
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_iExcavationExp,sizeof(int)) );

		// 영웅전
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_iAccrueHeroExpert,sizeof(int)) );
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_iHeroExpert,sizeof(int)) );
		PACKET_GUARD_VOID( query_data->GetValue(user_data.m_sPractice,sizeof(short)) );


#ifdef _DEBUG
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][usernode]Connect Block Info : DB:%d %d-%d-%d %d:%d SERVER:%d", nBlockType, user_data.m_kBlockTime.GetYear(), user_data.m_kBlockTime.GetMonth(), user_data.m_kBlockTime.GetDay(), user_data.m_kBlockTime.GetHour(), user_data.m_kBlockTime.GetMinute(), user_data.m_eBlockType );
#endif 
		
		// 가입만료확인
		if( user_data.m_eEntryType != ET_FORMALITY  &&
			user_data.m_eEntryType != ET_FORMALITY_CASH )
		{
			CTime current_time = CTime::GetCurrentTime();
			CTimeSpan gaptime;
			gaptime = current_time - entry_time;
			int iLimtDay	= g_LevelMgr.GetTempUserLimitDay();
			//enum { MAX_DAY_TEMPORARY_ENTRY = 60, };
			if( gaptime.GetDays() > iLimtDay )
			{
				user_data.m_eEntryType = ET_TERMINATION;
			}
		}

		pUser->SetUserData( user_data, bFirstLoginUser );
		g_UserNodeManager.ChangeUserNode( pUser );


#ifdef OHTG_USER_BLOCK_DISCONNECT
		if( user_data.m_eBlockType == BKT_BLOCK )
		{
			SP2Packet kPacket( STPK_CONNECT );
			PACKET_GUARD_VOID( kPacket.Write( CONNECT_BLOCK_USER) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPrivateID()) );
			pUser->SendMessage( kPacket );
			return ;
		}
#endif //OHTG_USER_BLOCK_DISCONNECT

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "CONNECT OK : %s : %s :%s :%s :%d :%d :(%d위) - %d : %s : %s : %d - AgentID:%d", 
								pUser->GetGUID().c_str(), pUser->GetPrivateID().c_str(), pUser->GetPublicID().c_str(),
                                pUser->GetPublicIP(), (int) user_data.m_eEntryType, pUser->GetLadderPoint(),
								pUser->GetUserRanking(), (int) user_data.m_eChannelingType, user_data.m_szChannelingUserID.c_str(), user_data.m_szChannelingUserNo.c_str(),
								(int) user_data.m_eBlockType, pUser->GetUserDBAgentID() );

#ifndef SRC_OVERSEAS // @32495 튜토리얼 관련 롤백
		// 튜토리얼 완료 안한 유저 강제로 용병 지급! 15.06 유영재 튜토리얼
		if( pUser->GetUserState() != US_TUTORIAL_CLEAR )
		{
			pUser->CreateTutorialChar();
		}
#endif

		// 유저 출석부 셋팅
		int iRollBookType = g_RollBookMgr.JudgmentUserRollBookType(entry_time, user_data.m_connect_time);
		if( COMPARE(iRollBookType, RBT_NEWBIE, RBT_DEFAULT + 1 ) )
			pUser->SetUserRollBookType(iRollBookType);
		else
		{
			pUser->SetUserRollBookType(RBT_DEFAULT);
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][rollbook] user rollbook type is error :[%s : %d]", pUser->GetPublicID().c_str(), iRollBookType );
		}

		// 휴면유저 ComeBack 보너스 : DB에 저장하는 값이 없으므로 바로 체크한다.
		EventUserManager &rEventUserMgr = pUser->GetEventUserMgr();
		DormancyUserEvent *pEventNode = static_cast<DormancyUserEvent*> ( rEventUserMgr.GetEventUserNode( EVT_DORMANCY_USER ) );
		if( pEventNode )
		{
			pEventNode->CheckDormancyDateToPresent( pUser, entry_time, user_data.m_connect_time );
		}

		GradePresentEventUserNode *pGradePresentEventUserNode = static_cast< GradePresentEventUserNode* > ( rEventUserMgr.GetEventUserNode( EVT_GRADEUP_PRESENT ) );
		if( pGradePresentEventUserNode )
			pGradePresentEventUserNode->SetCanReceiveGift( entry_time );

		EntryEventUserNode *pEntryEventUserNode = static_cast< EntryEventUserNode* > ( rEventUserMgr.GetEventUserNode( EVT_ENTRY ) );
		if( pEntryEventUserNode )
			pEntryEventUserNode->SetCanReceiveGift( entry_time );

		EntryAfterEventUserNode *pEntryAfterEventUserNode = static_cast< EntryAfterEventUserNode* > ( rEventUserMgr.GetEventUserNode( EVT_ENTRY_AFTER ) );
		if( pEntryAfterEventUserNode )
			pEntryAfterEventUserNode->SetCanReceiveGift( entry_time, user_data.m_user_state );

		// 용병 기간이 종료되고 24시간 지난 용병은 삭제한다. ( regDate을 확인 - OnUpdateCharRegDate - )
		g_DBClient.OnDeleteCharLimitDate( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );

		// 조작키 정보를 시작으로 유저의 모든 정보를 순차적으로 가져온다.
		g_DBClient.OnLoginSelectControlKeys( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

		g_DBClient.OnSelectRelativeGradeInfo( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );

		//유저 로그아웃 시간을 기준으로 유저 미션 정보 갱신
		DWORD dwLogOutTime = pUser->GetLastLogOutTime().GetTime();
		static IntVec vPassedType;
		vPassedType.clear();

		g_MissionMgr.GetResetMissionType(dwLogOutTime, vPassedType);
		for( int i = 0; i < (int)vPassedType.size(); i++ )
		{
			if( false == vPassedType[i] )
				continue;

			//DB에 해당 타입 미션들 초기화 요청. 유영재 추가 필요.
			g_DBClient.OnInitMissionData(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), i);
			g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_INIT, pUser, 0, 0, i+1, 0, 0, 0, 0, NULL);
		}

		// billing서버에서 캐쉬 확인 / 인터페이스를 맞추기 위해서 사용하지 않는 패킷 선언함.
		if( pLocal && !pLocal->IsGetCashAfterPublicIP() )
		{
			SP2Packet kBillingPacket;
			pUser->OnWebGetCash( kBillingPacket );
		}

		if( pLocal && pLocal->IsMileage())
		{
			// kBillingPacket 사용되지 않지만 인터페이스를 맞추기 위해서 
			SP2Packet kBillingPacket;
			pUser->OnGetMileage( kBillingPacket );
		}

		g_SaleMgr.SendLastActiveDate( pUser );

	}
	else     //유저에게 예외 알림.
	{
		SP2Packet kPacket( STPK_CONNECT );
		PACKET_GUARD_VOID( kPacket.Write( result) );
		PACKET_GUARD_VOID( kPacket.Write( pUser->GetPrivateID()) );
		pUser->SendMessage( kPacket );
		pUser->SetPrivateID("");
	}
}

void UserNodeManager::OnResultUpdateUserData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultUpdateUserLadderPoint(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserLadderPoint Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultUpdateUserHeroExpert(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserHeroExpert Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultInitUserHeroExpert(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInitUserHeroExpert Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultLoginSelectControlKeys(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectControlKeys Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );    //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectControlKeys USER FIND NOT! :%d",dwUserIdx);
		return;
	}
	if(pUser->GetUserIndex() != (DWORD)dwUserIdx)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectControlKeys USER INDEX NOT! :%d - %d",pUser->GetUserIndex(),dwUserIdx);
		return;
	}

	ControlKeys kControlKeys;
	if( query_data->GetResultCount() > 0 )
	{
		PACKET_GUARD_VOID( query_data->GetValue( kControlKeys.m_szControlKeys, MAX_CONTROL_KEYS_PLUS_ONE ) );        // 받은 정보.
	}

	if( kControlKeys.IsRight() )
	{
		SP2Packet kReturn( STPK_CONTROL_KEYS );
		PACKET_GUARD_VOID( kReturn.Write(kControlKeys) );
		pUser->SendMessage( kReturn );
	}

	//유저의 시상 내역을 모두 가져 온다.
	g_DBClient.OnLoginSelectAllAwardData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
}

void UserNodeManager::OnResultLoginSelectAllAwardData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllAwardData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );       //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx, sizeof(int) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllAwardData USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllAwardData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioAward *pAward = pUser->GetAward();
	if( pAward )
		pAward->DBtoData( query_data );

	// 시상 레벨과 경험치
	g_DBClient.OnLoginSelectAwardExpert( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
}

void UserNodeManager::OnResultLoginSelectAwardExpert(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAwardExpert Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );    //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAwardExpert USER FIND NOT! :%d", dwUserIdx );
		return;
	}

	ioAward *pAward = pUser->GetAward();
	if( pAward )
	{
		int iAwardLevel = 0, iAwardExp = 0;
		if( query_data->GetResultCount() > 0 )
		{
			PACKET_GUARD_VOID( query_data->GetValue( iAwardLevel, sizeof(int) ) );
			PACKET_GUARD_VOID( query_data->GetValue( iAwardExp, sizeof(int) ) );
		}
		pAward->SetAwardExpert( iAwardLevel, iAwardExp );
	}
	//유저의 클래스 숙련도를 가져온다
	g_DBClient.OnLoginSelectAllClassExpert( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
}

void UserNodeManager::OnResultLoginSelectAllClassExpert(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllClassExpert Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllClassExpert USER FIND NOT! :%d",dwUserIdx);
		return;
	}
	if(pUser->GetUserIndex() != dwUserIdx)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllClassExpert USER INDEX NOT! :%d - %d",pUser->GetUserIndex(),dwUserIdx);
		return;
	}

	ioClassExpert *pClassSet = pUser->GetClassExpert();
	if( pClassSet )
		pClassSet->DBtoData( query_data );

	//유저의 전적을 가져 온다
	g_DBClient.OnLoginSelectUserRecord( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
}

void UserNodeManager::OnResultLoginSelectUserRecord(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectUserRecord Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID ( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID ( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID ( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectUserRecord USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectUserRecord USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioUserRecord *pUserRecord = pUser->GetUserRecord();
	if( pUserRecord )
		pUserRecord->DBtoRecordData( query_data );

	//유저가 소유한 특별 아이템을 가져온다. 소유한 용병보다 먼저 가져올것
	g_DBClient.OnLoginSelectAllEtcItemData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
}

void UserNodeManager::OnResultLoginSelectAllEtcItemData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllEtcItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllEtcItemData USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllEtcItemData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioUserEtcItem *pInventory = pUser->GetUserEtcItem();
	if( pInventory )
	{
		pInventory->DBtoData( query_data );

		// After Item Init Control...
		ioUserEtcItem::ETCITEMSLOT kEtcItem;
		if( pInventory->GetEtcItem( ioEtcItem::EIT_ETC_BINGO_ITEM, kEtcItem ) == true )
		{
			// Bingo info select
			g_DBClient.OnSelectBingoNumber( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
			g_DBClient.OnSelectBingoPresent( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
		}

		if( pInventory->GetEtcItem( ioEtcItem::EIT_ETC_TIME_CASH1, kEtcItem ) == true || 
			pInventory->GetEtcItem( ioEtcItem::EIT_ETC_TIME_CASH2, kEtcItem ) == true)
		{
			//기간 캐쉬 박스 가져 오자!!
			g_DBClient.OnSelectTimeCashTable(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());
		}

		if( pInventory->GetEtcItem( ioEtcItem::EIT_ETC_OAK_DRUM_ITEM, kEtcItem ) == true )
		{
			g_DBClient.OnSelectPirateRouletteNumber( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
			g_DBClient.OnSelectPirateRoulettePresent( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
		}
	}

	pUser->CheckCurMaxCharSlot();

	//유저의 조합아이템 정보를 모두 가져 온다.
	g_DBClient.OnLoginSelectAllAlchemicData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

	//유저의 추가장비를 모두 가져 온다.  소유한 용병보다 먼저 가져올것
	ioUserExtraItem *pExtraItem = pUser->GetUserExtraItem();
	if( pExtraItem )
	{
		pExtraItem->Initialize(pUser);
	}

	//g_DBClient.OnLoginSelectAllExtraItemData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
	g_DBClient.OnLoginSelectExtraItemData( 0, pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
	
	// 수련장
	g_DBClient.OnLoginSelectPracticeData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
	g_DBClient.OnPracticeRankList( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
	
	//유저의 펫을 모두 가져 온다.
	g_DBClient.OnLoginSelectAllPetData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() ); 
	
	//유저의 개인 본부 인벤 정보를 가지고 온다.
	g_DBClient.OnSelectPersonalInvenInfo(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());

	// 유저의 오크통 관련 데이터를 받아온다
	if( pUser->GetOakBarrelOpen() == true )
		g_DBClient.OnSelectOakBarrelData(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());

#ifdef CARD_MATCHING_BY_BCKIM	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가	// 유저 로그인시 기본 데이터 load DB
	g_DBClient.OnSelectCardMatchingData(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());
#endif  // CARD_MATCHING_BY_BCKIM

#ifdef DICE_GAME_BY_BCKIM			// 2018-08-30 by bckim, 주사위 이벤트 추가
	g_DBClient.OnSelectDiceGameData(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());
#endif	// DICE_GAME_BY_BCKIM

	g_DBClient.OnSelectUserMatch(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());
}	

void UserNodeManager::OnResultLoginSelectAllExtraItemData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllExtraItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	int iDBSelectCount = query_data->GetResultCount();

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllExtraItemData USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioUserExtraItem *pItem = pUser->GetUserExtraItem();
	int iLastIndex = 0;

	if( pItem )
		pItem->DBtoData( query_data, iLastIndex );

	if( iDBSelectCount < DB_EXTRAITEM_SELECT_COUNT )
	{
		//유저 장비 아이템 전송.
		pItem->SendAllExtraItemInfo();

		//유저의 영웅전 정보
		g_DBClient.OnSelectHeroData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

		//유저의 길드 정보 가져온다 : 순서 주의 - 아래 퀘스트 정보에서 길드 가입 여부를 판단하므로 순서 유지해야함
		g_DBClient.OnSelectUserGuildInfo( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

		// 로그인 순서 변경. 16.04.08
		//유저 코인 
		g_DBClient.OnLoginSelectUserCoin( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
		////유저의 퀘스트 완료 목록
		//g_DBClient.OnLoginSelectAllQuestCompleteData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

	}
	else
	{
		// 나머지 장비 가지고 오게 프로시져 호출
		g_DBClient.OnLoginSelectExtraItemData( iLastIndex, pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
	}
}

void UserNodeManager::OnResultLoginSelectAllQuestCompleteData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllQuestCompleteData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectAllQuestCompleteData USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectAllQuestCompleteData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioQuest *pQuest = pUser->GetQuest();
	if( pQuest )
		pQuest->DBtoCompleteData( query_data );

	//유저의 퀘스트 진행중 목록
	g_DBClient.OnLoginSelectAllQuestData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
}

void UserNodeManager::OnResultLoginSelectAllQuestData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllQuestData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllQuestData USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllQuestData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioQuest *pQuest = pUser->GetQuest();
	if( pQuest )
		pQuest->DBtoData( query_data );

	//본부 정보
	g_DBClient.OnSelectHeadquartersDataCount( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
	
	// 대회 팀 
	g_DBClient.OnSelectTournamentTeamList( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetUserIndex(), MAX_INT_VALUE, TOURNAMENT_TEAM_MAX_LOAD );

	//유저가 소유하고있는 캐릭터를 모두 가져온다. 
	g_DBClient.OnLoginSelectAllCharData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

	// 유저의 절친 리스트 가져온다.
	g_DBClient.OnSelectBestFriendList( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );

	//유저의 코스튬 정보를 가져온다.
	g_DBClient.OnLoginSelectCostumeData(0, pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex());

	//유저의 슬롯 아이템을 모두 가져 온다.
	g_DBClient.OnLoginSelectAllInvenData( 0, pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

	//유저의 용병 성장정보를 가져온다. 소유한 슬롯 이후에 가져올것
	g_DBClient.OnLoginSelectAllGrowth( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

	// 유저의 확장 메달슬롯 정보를 가져온다.
	g_DBClient.OnLoginSelectAllExMedalSlotData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

	// 유저의 메달아이템 가져온다 ( 캐릭터에 장착하는 메달 ) 
	g_DBClient.OnLoginSelectAllMedalItemData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

	// 유저의 미션 정보를 가져 온다.
	g_DBClient.OnLoginSelectMissionData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

	// 유저의 칭호 정보를 가져 온다.
	g_DBClient.OnSelectTitlesInfo(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());

	// 보너스 캐쉬
	g_DBClient.OnDeleteExpirationBonusCash(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());// 기간 만료된 보너스 캐쉬 만료상태로 변경
	g_DBClient.OnSelectBonusCash(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), GET_BONUS_CASH_PERIOD);			// 사용 가능 보너스 캐쉬 ( 메모리저장 & 클라이언트전송 )
	g_DBClient.OnSelectExpiredBonusCash(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());	// 만료된 보너스 캐쉬 ( GoldStaus = 0 , 메모리저장 & 클라이언트전송)

	// 유저 커스텀 메달
	g_DBClient.OnSelectCustomMedal(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );

	// 타임게이트 (테스트)
	g_DBClient.OnSelectTimeGate(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );

	// 친구 추천 이벤트
	ioLocalParent *pLocal =  g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsFriendRecommend() )
	{
		g_DBClient.OnSelectFriendRecommendData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() ); 
	}

	// 클로버 정보
	g_DBClient.OnSelectCloverInfoRequest( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), pUser->GetGUID() );

	//대회응원 정보
	g_DBClient.OnSelectTournamentCheerList( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetUserIndex(), TOURNAMENT_CHEER_MAX_LOAD, MAX_INT_VALUE );

	//유저의 악세사리 정보를 가져온다.
	g_DBClient.OnLoginSelectAccessoryData(0, pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex());

	//출석이벤트 정보

	//2015년 1월 1일 이면 새로운 출석부가 불리도록 해야함.
	CTime cCurTime = CTime::GetCurrentTime();
	if( cCurTime.GetYear() >= 2015 )
	{
		if( !pUser->IsEntryFormality() )
			return;

		if( cCurTime.GetMonth() == 1 && cCurTime.GetDay() == 1 )
		{
			if( cCurTime.GetHour() < g_RollBookMgr.GetRenewalHour() )
				return;
		}
		//Roll book 호출
 		g_DBClient.OnLoginSelectRollBookData(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex());
	}
	else if( cCurTime.GetYear() < 2015 )
	{
		ioUserAttendance* pAttendance = pUser->GetUserAttendance();
		if( pAttendance )
		{
			pAttendance->OnSelectAttendanceRecord();
		}
	}
}

void UserNodeManager::OnResultLoginSelectAllCharData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllCharData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	int iUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue(iUserIndex, sizeof(int)) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(iUserIndex);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllCharData USER FIND NOT! :%d", iUserIndex);
		return;
	}

	if( !query_data->IsExist() && pUser->GetUserState() != US_TUTORIAL_CLEAR )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][charFill] first login user don't have hero : %d", iUserIndex );
	}

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		ioCharacter *pCharacter = pUser->AddCharDataToPointer();
		int iCharIndex = 0;
		PACKET_GUARD_BREAK( query_data->GetValue(iCharIndex,sizeof(int)) );	//캐릭터 인덱스
		//char data
		CHARACTER char_data;
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_class_type,sizeof(int)) );	//클래스타입
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_kindred,sizeof(int)) );		//종족
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_sex,sizeof(int)) );			//성별
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_beard,sizeof(int)) );		//수염
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_face,sizeof(int)) );			//얼굴
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_hair,sizeof(int)) );			//머리
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_skin_color,sizeof(int)) );	//피부색
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_hair_color,sizeof(int)) );	//머리색
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_accessories,sizeof(int)) );  //장신구
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_underwear,sizeof(int)) );    //속옷
		//Item
		for( int i = 0; i < MAX_CHAR_DBITEM_SLOT;i++ )
			PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_extra_item[i],sizeof(int)) );

		//용병 슬롯 위치
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_iSlotIndex,sizeof(int)) );

		//용병 기간
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_iLimitSecond,sizeof(int)) );

		//용병 기간 타입
		int iPeriodType = 0;
		PACKET_GUARD_BREAK( query_data->GetValue( iPeriodType, sizeof( int ) ) );
		char_data.m_ePeriodType = (CharPeriodType) iPeriodType;

		//용병 대표 타입
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_sLeaderType,sizeof(short)) );

		//용병 대여 타입
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_sRentalType,sizeof(short)) );

		//용병 대여 시간
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_dwRentalMinute,sizeof(int)) );
		if( char_data.m_dwRentalMinute != 0 )
		{
			char_data.m_dwRentalMinute = Help::CheckDiffTimeToTime( char_data.m_dwRentalMinute, CTime::GetCurrentTime(), pUser->GetLastLogOutTime() );
		}

		//용병 각성
		PACKET_GUARD_BREAK( query_data->GetValue( char_data.m_chAwakeType,sizeof(char) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( char_data.m_iAwakeLimitTime,sizeof(int) ) );

		//용병 강화도 ioClassExpert 에 저장된 정보를 가져옴.
		ioClassExpert *pClassExpert = pUser->GetClassExpert();
		if( pClassExpert )
		{
			char_data.m_byReinforceGrade = pClassExpert->GetClassReinfoce(char_data.m_class_type);
		}

		// 시간 지난 각성 정보 
		/*if( pUser->CheckAwakePassedDate( char_data.m_iAwakeLimitTime ) )
		{
			// 현재 시간으로 변환.
			int iCurDate = pUser->GetINTtypeCurDate();
			char_data.m_iAwakeLimitTime = iCurDate;
		}*/

		if( char_data.m_iAwakeLimitTime > 0 )
			pUser->InsertAwakeDataMap( iCharIndex, char_data.m_iAwakeLimitTime );

		pCharacter->SetCharInfo( iCharIndex, char_data, pUser );
		pCharacter->CheckExtraItemEquip(pUser);

		pCharacter->BackUp();

		if( !pCharacter->IsMortmain() )
		{
			if( pCharacter->GetCharLimitDate() <= 0 ) //기간 지났음
				pCharacter->SetActive( false );				
		}
	}	
	pUser->IntegrityCharSlot();
	LOOP_GUARD_CLEAR();

/*	// 용병 1개당 120byte : 8 * Char = 960byte - 8개씩 나눠 보냄
	int iCharCount = pUser->GetCharCount();
	SP2Packet kPacket( STPK_CHAR_ALL_LOAD );
	kPacket << iCharCount;	
	for(int i = 0;i < iCharCount;i++)
	{
		ioCharacter *pCharacter = pUser->GetCharacter( i );
		if( pCharacter == NULL )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllCharData Character NULL(%s - %d)", pUser->GetPublicID().c_str(), i );
			return ;
		}

		kPacket << i;
		kPacket << pCharacter->GetCharIndex();
		kPacket << (CHARACTER)pCharacter->GetCharInfo();
	}
	pUser->SendMessage( kPacket );		
*/
	// 로그인 완료
	pUser->SendAllCharInfo();

#ifdef ARENA_MODE_BY_BCKIM		// 2018-07-25 by bckim, 아레나 모드 추가
	std::vector<ArenaModeManager::stArenaSoldierInfo> vec_temp_info = g_ArenaModeManager.GetSoldierInfo();
	SP2Packet kPacket( STPK_ARENA_MODE_CHAR_ALL_LOAD );
	int soldier_cnt = vec_temp_info.size();
	kPacket << soldier_cnt;		
	for( std::vector<ArenaModeManager::stArenaSoldierInfo>::iterator it = vec_temp_info.begin() ; it != vec_temp_info.end() ; ++it )
	{
		ArenaModeManager::stArenaSoldierInfo temp = *it;

		kPacket << temp.iSoldier_Number;
		kPacket << temp.iSoldier_PowerUp_On;
		kPacket << temp.iSoldier_PowerUp_Code;
		kPacket << temp.iface;
		kPacket << temp.ihair;
		kPacket << temp.iskin_color;
		kPacket << temp.ihair_color;
		kPacket << temp.iunderwear;
	}
	pUser->SendMessage( kPacket );		

#endif	//	ARENA_MODE_BY_BCKIM		// End. 2018-07-25 by bckim, 아레나 모드 추가

	// 용병 각성 종료 알림 패킷
	//pUser->SendCharAwakeEndPacket( mDeleteAwakeData, true );

	g_DBClient.OnSelectUserSpiritData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), 0, pUser->GetUserIndex(), DB_SPIRIT_SELECT_COUNT );

	pUser->ConnectProcessComplete();

	//------------------- 아래의 정보는 로비에 입장 후 도착한다.	
	g_DBClient.OnLoginSelectAllEventData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );    
}

void UserNodeManager::OnResultLoginSelectAllInvenData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllInvenData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	int iDBSelectCount	= query_data->GetResultCount();
	int iLastIndex		= 0;

	User *pUser = GetUserNode(dwUserIdx);

	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllInvenData USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllInvenData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioInventory *pInventory = pUser->GetInventory();
	if( !pInventory )
		return;

	//TEST 유영재
	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][deco] Select DB deco size : [%s] [%d]", pUser->GetPublicID().c_str(), iDBSelectCount );

	pInventory->DBtoData( query_data, iLastIndex );

	if( iDBSelectCount < DB_DECO_SELECT_COUNT )
	{
		//유저에게 치장 정보 전송.
		int iCount	= pInventory->GetCount();
		for( int iStartArray = 0; iStartArray < iCount; )
		{
			//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][deco] deco send size [start index %d] : [%s]", iStartArray, pUser->GetPublicID().c_str() );
			iStartArray = pInventory->SendInventory( iStartArray, DB_DECO_SELECT_COUNT ); 
			//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][deco] deco send size [end index %d] : [%s]", iStartArray-1, pUser->GetPublicID().c_str() );
		}
	}
	else
	{
		g_DBClient.OnLoginSelectAllInvenData(iLastIndex, pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex());
	}
}

void UserNodeManager::OnResultLoginSelectAllGrowth(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllGrowth Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	int  iUserIndex = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue(szUserID,ID_NUM_PLUS_ONE) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue(iUserIndex, sizeof(int)) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(iUserIndex);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllGrowth USER FIND NOT! :%d", iUserIndex);
		return;
	}
	if(pUser->GetUserIndex() != (DWORD)iUserIndex)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllGrowth USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), iUserIndex);
		return;
	}

	ioUserGrowthLevel *pGrowthLevel = pUser->GetUserGrowthLevel();
	if( pGrowthLevel )
	{
		pGrowthLevel->DBtoRecordData( query_data );
	}
}

void UserNodeManager::OnResultLoginSelectAllMedalItemData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllMedalItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );    //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllMedalItemData USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllMedalItemData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioUserMedalItem *pItem = pUser->GetUserMedalItem();
	if( pItem )
		pItem->DBtoData( query_data );
}

void UserNodeManager::OnResultLoginSelectAllExMedalSlotData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllExMedalSlotData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );    //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllExMedalSlotData USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllExMedalSlotData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioUserExpandMedalSlot *pExMedalSlot = pUser->GetUserExpandMedalSlot();
	if( pExMedalSlot )
		pExMedalSlot->DBtoData( query_data );
}

void UserNodeManager::OnResultLoginSelectAllEventData( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllEventData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	int  iUserIndex = 0;
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char szUserID[ID_NUM_PLUS_ONE] = "";
	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue(szUserID,ID_NUM_PLUS_ONE) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue(iUserIndex,sizeof(int)) );		 //유저의 인덱스

	User *pUser = GetUserNode(iUserIndex);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllEventData USER FIND NOT! :%d",iUserIndex);
		return;
	}
	if(pUser->GetUserIndex() != (DWORD)iUserIndex)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllEventData USER INDEX NOT! :%d - %d",pUser->GetUserIndex(),iUserIndex);
		return;
	}

	EventUserManager &rEventUserMgr = pUser->GetEventUserMgr();

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		int iIndex = 0;
		int iEventType = 0;
		PACKET_GUARD_BREAK( query_data->GetValue(iIndex,sizeof(int)) );     // index
		PACKET_GUARD_BREAK( query_data->GetValue(iEventType,sizeof(int)) ); // event type

		rEventUserMgr.SetIndex( (EventType) iEventType, iIndex );

		for (int i = 0; i < g_EventMgr.GetMaxDBValue() ; i++)
		{
			int iValue = 0;
			PACKET_GUARD_BREAK( query_data->GetValue(iValue,sizeof(int)) );	  // value 	
			rEventUserMgr.SetValue( (EventType)iEventType, i, iValue );
		}
	}
	LOOP_GUARD_CLEAR();
	// Backup -> insertDB -> UpdateFirst
	rEventUserMgr.Backup();

	int iSize = rEventUserMgr.GetSize();
	if( !iSize ) return;

	rEventUserMgr.InsertDB( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), szUserGUID, iUserIndex );
	rEventUserMgr.UpdateFirst( pUser );

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[ID:%s][MaxDB:%d]", pUser->GetPublicID().c_str(), g_EventMgr.GetMaxDBValue() );
	SP2Packet kPacket( STPK_EVENT_DATA );
	PACKET_GUARD_VOID( kPacket.Write(iSize) );
	for(int i = 0;i < iSize;i++)
	{
		// server
		int iEventType = rEventUserMgr.GetType( i );
		int iValueSize = g_EventMgr.GetValueSize( (EventType) iEventType );
		bool bAlive    = g_EventMgr.IsAlive( (EventType) iEventType, pUser->GetChannelingType() );
		int iMaxDBValue= g_EventMgr.GetMaxDBValue();
		int iUseChannelingTypeSize = g_EventMgr.GetUseChannelingTypeSize( (EventType) iEventType ); 
		PACKET_GUARD_VOID( kPacket.Write(iEventType) );
		PACKET_GUARD_VOID( kPacket.Write(iValueSize) );
		PACKET_GUARD_VOID( kPacket.Write(bAlive) );
		PACKET_GUARD_VOID( kPacket.Write(iMaxDBValue) );
		PACKET_GUARD_VOID( kPacket.Write(iUseChannelingTypeSize) );
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SERVER:[Type:%d][Size:%d][Alive:%d][ChannelingSize:%d]", iEventType, iValueSize, bAlive, iUseChannelingTypeSize );
		for (int j = 0; j < iValueSize; j++)
		{
			int iValue = g_EventMgr.GetValue((EventType) iEventType , j);
			PACKET_GUARD_VOID( kPacket.Write(iValue) );
			EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, " [Value%d:%d]", j+1, iValue );
		}

		for (int k = 0; k < iUseChannelingTypeSize ; k++)
		{
			ChannelingType eType = g_EventMgr.GetUseChannelingType( (EventType) iEventType, k );
			PACKET_GUARD_VOID( kPacket.Write((int) eType) );
			EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, " [ChannelingType%d:%d]", k+1, (int) eType );
		}
	}

	for(int i = 0;i < iSize;i++)
	{
		// user
		EventType eType = g_EventMgr.GetType( i );
		int iNodeSize = rEventUserMgr.GetNodeSize( eType );
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "USER:[Type:%d][Size:%d]", (int)eType, iNodeSize );

		PACKET_GUARD_VOID( kPacket.Write(iNodeSize) );
		for (int k = 0; k < iNodeSize ; k++)
		{
			int iValue = rEventUserMgr.GetValue( eType, k );
			PACKET_GUARD_VOID( kPacket.Write(iValue) );
			EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, " [NodeValue%d:%d]", k+1, iValue );
		}
	}
	pUser->SendMessage( kPacket );		

	// 본부 접속 완료 권한 아이템 시간 설정.
	pUser->StartEtcItemTime( __FUNCTION__ );

	// 매일매일 골드 아이템~♬ : 튜토리얼을 클리어하지 못한 유저는 클리어할 때 지급한다.
	if( pUser->GetUserState() == US_TUTORIAL_CLEAR )
	{
		OneDayGoldItemEvent *pEventNode = static_cast<OneDayGoldItemEvent*> ( rEventUserMgr.GetEventUserNode( EVT_ONE_DAY_GOLD_ITEM ) );
		if( pEventNode )
		{
			pEventNode->CheckGoldItemDate( pUser );
		}
	}

	EntryEventUserNode *pEntryEventUserNode = static_cast< EntryEventUserNode* > ( rEventUserMgr.GetEventUserNode( EVT_ENTRY ) );
	if( pEntryEventUserNode )
		pEntryEventUserNode->SetGift( pUser );

	EntryAfterEventUserNode *pEntryAfterEventUserNode = static_cast< EntryAfterEventUserNode* > ( rEventUserMgr.GetEventUserNode( EVT_ENTRY_AFTER ) );
	if( pEntryAfterEventUserNode )
		pEntryAfterEventUserNode->SetGift( pUser );
}

void UserNodeManager::OnResultInsertCharData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertCharData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectCharIndex(CQueryResultData *query_data)
{
	if(FAILED( query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCharIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szUserID[ID_NUM_PLUS_ONE] = "";
	int  iResult = 0;
	int  iLogType  = -1;
	int  iBuyPrice = 0;
	DWORD dwUserIdx = 0;

	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof( int ) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof( int ) ) );           //생성 결과
	PACKET_GUARD_VOID( query_data->GetValue( iLogType, sizeof( int ) ) );          
	PACKET_GUARD_VOID( query_data->GetValue( iBuyPrice, sizeof( int ) ) );          

	User *pUser = GetUserNode(dwUserIdx);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCharIndex USER FIND NOT! :%d",dwUserIdx);
		return;
	}

	LOOP_GUARD();
	while( query_data->IsExist() )
	{	
		int char_index  = 0;
		query_data->GetValue( char_index, sizeof(int) );

		if( !pUser->IsCharIndex( char_index ) )
		{
			g_DBClient.OnSelectCharData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), szUserGUID,  char_index, iResult, iLogType, iBuyPrice );
			iLogType  = -1;
			iBuyPrice = 0; 
		}
		else
			pUser->SetCreateCharCount( max( 0, pUser->GetCreateCharCount() - 1 ) );       //이미 받은 용병 
	}
	LOOP_GUARD_CLEAR();
}

void UserNodeManager::OnResultSelectCharData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCharData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	int  char_index = 0;
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szUserID[ID_NUM_PLUS_ONE] = "";
	int  iResult = 0;
	int  iLogType  = -1;
	int  iBuyPrice = 0;

	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue(szUserID,ID_NUM_PLUS_ONE) );       //보낸 유저의 아이디.
	
	PACKET_GUARD_VOID( query_data->GetValue(iResult,sizeof(int)) );           //생성 결과
	PACKET_GUARD_VOID( query_data->GetValue(char_index,sizeof(int)) );		 //캐릭터 인덱스
	PACKET_GUARD_VOID( query_data->GetValue(iLogType,sizeof(int)) );
	PACKET_GUARD_VOID( query_data->GetValue(iBuyPrice,sizeof(int)) );
	
	// DB data..
	int iUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( iUserIndex, sizeof(int) ) );        //캐릭터 주인의 인덱스

	User *pUser = GetUserNode(iUserIndex);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCharData USER FIND NOT! :%d",iUserIndex);
		return;
	}
	if(pUser->GetUserIndex() != (DWORD)iUserIndex)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCharData USER INDEX NOT! :%d - %d",pUser->GetUserIndex(),iUserIndex);
		return;
	}

	//char data
	CHARACTER char_data;
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_class_type,sizeof(int)) );	//클래스타입
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_kindred,sizeof(int)) );		//종족
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_sex,sizeof(int)) );			//성별
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_beard,sizeof(int)) );		//수염
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_face,sizeof(int)) );			//얼굴
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_hair,sizeof(int)) );			//머리
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_skin_color,sizeof(int)) );	//피부색
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_hair_color,sizeof(int)) );	//머리색
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_accessories,sizeof(int)) );  //장신구
	PACKET_GUARD_VOID( query_data->GetValue(char_data.m_underwear,sizeof(int)) );    //속옷
	//Item
	for( int i = 0; i < MAX_CHAR_DBITEM_SLOT;i++ ) //kyg 위험 코드 
	{//kyg return or break 고민 
		PACKET_GUARD_BREAK( query_data->GetValue(char_data.m_extra_item[i],sizeof(int)) );
	}
	
	//용병 슬롯 위치
	PACKET_GUARD_VOID( query_data->GetValue( char_data.m_iSlotIndex, sizeof(int) ) );

	//용병 기간
	PACKET_GUARD_VOID( query_data->GetValue( char_data.m_iLimitSecond, sizeof(int) ) );

	//용병 기간 타입
	int iPeriodType = 0;
	PACKET_GUARD_VOID( query_data->GetValue( iPeriodType, sizeof( int ) ) );
	char_data.m_ePeriodType = (CharPeriodType) iPeriodType;

	//용병 대표 타입
	PACKET_GUARD_VOID( query_data->GetValue( char_data.m_sLeaderType, sizeof(short) ) );
    
	//용병 대여 타입
	PACKET_GUARD_VOID( query_data->GetValue( char_data.m_sRentalType, sizeof(short) ) );

	//용병 대여 시간
	PACKET_GUARD_VOID( query_data->GetValue( char_data.m_dwRentalMinute, sizeof(int) ) );
	if( char_data.m_dwRentalMinute != 0 )
	{
		char_data.m_dwRentalMinute = Help::CheckDiffTimeToTime( char_data.m_dwRentalMinute, CTime::GetCurrentTime(), pUser->GetLastLogOutTime() );
	}

	//용병 각성 정보
	PACKET_GUARD_VOID( query_data->GetValue( char_data.m_chAwakeType, sizeof(char) ) );
	PACKET_GUARD_VOID( query_data->GetValue( char_data.m_iAwakeLimitTime, sizeof(int) ) );

	// 구매한 용병과 보유한 연습용병이 같다면
	ioCharacter *pCharacter = NULL;
	int iCharArray          = 0;
	int iExerciseCharArray  = pUser->GetExerciseCharArrayByClass( char_data.m_class_type );
	if( iExerciseCharArray != -1 )
	{
		pCharacter = pUser->GetCharacter( iExerciseCharArray );		
		iCharArray = iExerciseCharArray;
		if( pCharacter )
		{
			if( pCharacter->HasExerciseStyle( EXERCISE_RENTAL ) )
			{
				// 주인에게 반납
				pUser->BuyRentalCharacterProcess( pCharacter );				
			}
			else
			{
				g_ExerciseCharIndexMgr.Add( pCharacter->GetCharIndex() );
			}
		}
	}
	else
	{
		pCharacter = pUser->AddCharDataToPointer();
		iCharArray = pUser->GetCharCount() - 1;
	}

	if( !pCharacter )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCharData Char NULL :%s - %d - %d", pUser->GetPublicID().c_str(), iExerciseCharArray, iResult);
		SP2Packet kReturn( STPK_CHAR_CREATE );

		PACKET_GUARD_VOID( kReturn.Write(CREATE_CHAR_EXCEPTION) );

		pUser->SendMessage( kReturn );
		return;
	}

	pCharacter->SetCharInfo( char_index, char_data, pUser );
	pCharacter->BackUp();
	pUser->CheckCharSlot( pCharacter );
	pUser->SetCreateCharCount( max( 0, pUser->GetCreateCharCount() - 1 ) );
	pUser->SetSelectCharDB( char_data.m_class_type );


	const CHARACTER &rkCharInfo = pCharacter->GetCharInfo();
	SP2Packet kPacket( STPK_CHAR_CREATE );

	PACKET_GUARD_VOID( kPacket.Write(iResult) );
	PACKET_GUARD_VOID( kPacket.Write(pUser->GetMoney()) );
	PACKET_GUARD_VOID( kPacket.Write(pUser->GetCash()) );
	PACKET_GUARD_VOID( kPacket.Write(pUser->GetChannelingCash()) );
	PACKET_GUARD_VOID( kPacket.Write(pUser->GetUserState()) );
	PACKET_GUARD_VOID( kPacket.Write(iCharArray) );
	PACKET_GUARD_VOID( kPacket.Write(char_index) );
	PACKET_GUARD_VOID( kPacket.Write((CHARACTER)rkCharInfo) );

	if( iLogType == LogDBClient::CT_CASH )
	{
		bool bMortmain = false;
		if( rkCharInfo.m_ePeriodType == CPT_MORTMAIN )
			bMortmain = true;
		PACKET_GUARD_VOID( kPacket.Write(g_ItemPriceMgr.GetBonusPeso( rkCharInfo.m_class_type, bMortmain )) );
		PACKET_GUARD_VOID( kPacket.Write(true) ); // bCash
	}
	else
	{
		PACKET_GUARD_VOID( kPacket.Write(0) );
		PACKET_GUARD_VOID( kPacket.Write(false) );// bCash
	}
	pUser->SendMessage( kPacket );

	pUser->StartCharLimitDate( pCharacter );

	if( iResult == CREATE_CHAR_OK )
	{
		pUser->CreateSelectCharData( pCharacter );
	}

	Room *pMyRoom = pUser->GetMyRoom();
	int iSoldierType	= pUser->GetSpecialSoldierType(pCharacter->GetClassType());

	if( pMyRoom )
	{
		pMyRoom->OnModeCharInsert( pUser, pCharacter );   // 용병 교체보다 먼저 발생해야함
		
		if( iSoldierType != SST_END )
		{
			if( pCharacter->HasExerciseStyle(EXERCISE_NONE) && pCharacter->IsMortmain() )
			{
				if( SST_RSOLDIER == iSoldierType )
				{
					//해당 방 유저에게 R용병 구매 했다고 통지.
					SP2Packet kPacket(STPK_RSOLDIER_STATUS);
					PACKET_GUARD_VOID( kPacket.Write(RSOLDIER_BUY) );
					PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
					pMyRoom->RoomSendPacketTcp(kPacket);
				}
				else
				{
					SP2Packet kPacket(STPK_SOLDIER_SET_STATUS);
					PACKET_GUARD_VOID( kPacket.Write(RSOLDIER_BUY) );
					PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
					PACKET_GUARD_VOID( kPacket.Write(pCharacter->GetClassType()) );
					pMyRoom->RoomSendPacketTcp(kPacket);
				}
			}
		}

		if( pMyRoom->GetModeType() == MT_HEADQUARTERS )
		{
			bool bWait = true;
			if( pUser->GetCharCount() == 1 )
				bWait = false;

			pUser->SetChangeChar(iCharArray, bWait, pUser->GetSelectChar(), MAX_INT_VALUE );
		}
		else if( pUser->GetSelectChar() == iExerciseCharArray )
			pUser->SetChangeChar(iCharArray, true, -1, MAX_INT_VALUE );
	}
	else
	{
		if( iSoldierType != SST_END )
		{
			if( pCharacter->HasExerciseStyle(EXERCISE_NONE) && pCharacter->IsMortmain() )
			{
				if( SST_RSOLDIER == iSoldierType )
				{
					//해당 방 유저에게 R용병 구매 했다고 통지.
					SP2Packet kPacket(STPK_RSOLDIER_STATUS);
					PACKET_GUARD_VOID( kPacket.Write(RSOLDIER_BUY) );
					PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
					pUser->SendMessage(kPacket);
				}
				else
				{
					SP2Packet kPacket(STPK_SOLDIER_SET_STATUS);
					PACKET_GUARD_VOID( kPacket.Write(RSOLDIER_BUY) );
					PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
					PACKET_GUARD_VOID( kPacket.Write(pCharacter->GetClassType()) );
					pUser->SendMessage(kPacket);
				}
			}
		}
	}

	//판매 기록
	char szItemIndex[MAX_PATH]="";
	StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u", char_index );
	if( iLogType == LogDBClient::CT_CASH )
	{
		if( iBuyPrice != 0 )
			g_LogDBClient.OnInsertCashItem( pUser, char_data.m_class_type, char_data.m_iLimitSecond, iBuyPrice, szItemIndex, LogDBClient::CIT_CHAR, szUserGUID );
	}
	else
		g_LogDBClient.OnInsertChar( pUser, char_data.m_class_type, char_data.m_iLimitSecond, iBuyPrice, szItemIndex, (LogDBClient::CharType) iLogType );
	//

	pUser->SetDefaultDecoItem( char_data );
	if( pUser->IsSpecialSoldier(pCharacter, SST_GFRIEND, FALSE) )
		pUser->GiveWomanGender(char_data.m_class_type);
}

void UserNodeManager::OnResultUpdateCharData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateCharData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultDeleteCharData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultDeleteCharData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultDeleteCharLimitDate(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultDeleteCharLimitDate Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultUpdateCharRegDate( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateCharRegDate Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectCharRentalHistory(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCharRentalHistory Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	int iClassType = 0;
	DWORD dwUserIndex = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szTargetID[ID_NUM_PLUS_ONE] = "";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	query_data->GetValue( dwUserIndex, sizeof(int) );
	query_data->GetValue( szTargetID, ID_NUM_PLUS_ONE );    
	query_data->GetValue( iClassType, sizeof(int) );

	User *pUser = GetUserNode(dwUserIndex);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCharRentalHistory USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	if( pUser->GetUserIndex() != (DWORD)dwUserIndex )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllEventData USER INDEX NOT! :%d - %d",pUser->GetUserIndex(), dwUserIndex );
		return;
	}

	LOOP_GUARD();
	CharRentalList kCharRentalList;
	while( query_data->IsExist() )
	{		
        DBTIMESTAMP dts;
		CharRentalData kRentalData;
		query_data->GetValue( kRentalData.m_dwUserIndex, sizeof(DWORD) ); 
		if( kRentalData.m_dwUserIndex == 0 ) 
			break;

		query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) );
		kRentalData.m_RentalTime = CTime( Help::GetSafeValueForCTimeConstructor( dts.year, dts.month, dts.day, dts.hour, dts.minute, dts.second ) );
		
		kCharRentalList.push_back( kRentalData );
	}
	LOOP_GUARD_CLEAR();

	int iRentalSize = (int)kCharRentalList.size();
	if( iRentalSize >= Help::GetCharRentalCount() )
	{
		SP2Packet kPacket( STPK_USER_CHAR_RENTAL_REQUEST );
		kPacket << USER_CHAR_RENTAL_REQUEST_ONE_DAY_LIMIT << szTargetID;
		pUser->SendMessage( kPacket );    
	}
	else
	{
		UserParent *pUserParent = GetGlobalUserNode( szTargetID );
		if( pUserParent == NULL )
		{
			SP2Packet kPacket( STPK_USER_CHAR_RENTAL_REQUEST );
			kPacket << USER_CHAR_RENTAL_REQUEST_OFFLINE << szTargetID;
			pUser->SendMessage( kPacket );    
		}
		else
		{
			for(int i = 0;i < iRentalSize;i++)
			{
				CharRentalData &rkRentalData = kCharRentalList[i];
				if( rkRentalData.m_dwUserIndex == pUserParent->GetUserIndex() )
				{
					SP2Packet kPacket( STPK_USER_CHAR_RENTAL_REQUEST );
					kPacket << USER_CHAR_RENTAL_REQUEST_ALREADY_LIMIT << szTargetID << rkRentalData.m_RentalTime.GetMonth() 
						    << rkRentalData.m_RentalTime.GetDay() << rkRentalData.m_RentalTime.GetHour() << rkRentalData.m_RentalTime.GetMinute();
					pUser->SendMessage( kPacket );    
					return;
				}
			}

			if( pUserParent->IsBestFriend( pUser->GetUserIndex() ) == false )
			{
				SP2Packet kPacket( STPK_USER_CHAR_RENTAL_REQUEST );
				kPacket << USER_CHAR_RENTAL_REQUEST_NONE_BF << szTargetID;
				pUser->SendMessage( kPacket );    
				return;
			}

			if( pUser->IsCharClassType( iClassType ) )
			{
				SP2Packet kPacket( STPK_USER_CHAR_RENTAL_REQUEST );
				kPacket << USER_CHAR_RENTAL_REQUEST_CHAR_SAME << szTargetID << iClassType;
				pUser->SendMessage( kPacket );    
				return;
			}

			if( pUser->GetExerciseCharCount( EXERCISE_RENTAL ) >= Help::GetCharRentalCount() )
			{
				SP2Packet kPacket( STPK_USER_CHAR_RENTAL_REQUEST );
				kPacket << USER_CHAR_RENTAL_REQUEST_ONE_DAY_LIMIT << szTargetID;
				pUser->SendMessage( kPacket );    
				return;
			}

			// 결과
			SP2Packet kPacket( STPK_USER_CHAR_RENTAL_REQUEST );
			kPacket << USER_CHAR_RENTAL_REQUEST_OK << szTargetID << iClassType;
			pUser->SendMessage( kPacket );

			// 상대방에게 대여 요청
			SP2Packet kPacket2( STPK_USER_CHAR_RENTAL_REQUEST );
			kPacket2 << USER_CHAR_RENTAL_AGREE_REQUEST << pUser->GetPublicID() << iClassType;
			pUserParent->RelayPacket( kPacket2 );
		}
	}
}

void UserNodeManager::OnResultInsertClassExpert(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertClassExpert Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectClassExpertIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectClassExpertIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwClassInfoIdx = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	DWORD dwUserIdx = 0;
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );
	query_data->GetValue( dwUserIdx, sizeof(int) );        
	query_data->GetValue( dwClassInfoIdx, sizeof(int) );	 

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectClassExpertIndex USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioClassExpert *pClassExpert = pUser->GetClassExpert();
	if( pClassExpert )
	{
		if( !pClassExpert->DBtoNewIndex( dwClassInfoIdx ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectClassExpertIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwClassInfoIdx );
	}
}

void UserNodeManager::OnResultUpdateClassExpert(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateClassExpert Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultInsertInvenData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertInvenData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectInvenIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectInvenIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwInvenIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	DWORD dwUserIdx = 0;
	bool   bBuyCash  = false;
	int    iBuyPrice = 0;
	int    iLogType  = 0;

	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );        //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx,  sizeof( int ) );
	query_data->GetValue( bBuyCash, sizeof( bool ) );      
	query_data->GetValue( iBuyPrice, sizeof( int ) );     
	query_data->GetValue( iLogType, sizeof( int ) );

	query_data->GetValue( dwInvenIdx, sizeof(int) );		 //새로운 인벤토리 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectInvenIndex USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioInventory *pInventory = pUser->GetInventory();
	if( !pInventory )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectInvenIndex : (%d - %d) pInventory == NULL.", dwUserIdx, dwInvenIdx );
		return;
	}

	if( !pInventory->DBtoNewIndex( dwInvenIdx ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectInvenIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwInvenIdx );
		return;
	}
	
	ITEMSLOT kItemSlot[ ioInventory::MAX_SLOT ];
	if( !pInventory->GetItemInfo( dwInvenIdx, kItemSlot ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectInvenIndex : (%d - %d) Index not found.", dwUserIdx, dwInvenIdx );
		return;
	}

	for (int i = 0; i < ioInventory::MAX_SLOT ; i++)
	{
		if( kItemSlot[i].m_item_type == 0 && kItemSlot[i].m_item_code == 0 )
			continue;

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwInvenIdx, i+1 );

		if( bBuyCash )
		{
			if( iBuyPrice != 0 )
			{
				g_LogDBClient.OnInsertCashItem( pUser, kItemSlot[i].m_item_type, kItemSlot[i].m_item_code, iBuyPrice, szItemIndex, LogDBClient::CIT_DECO, pUser->GetBillingGUID().c_str() );
				
			}
			// 여자치장을 구매하면 DB insert를 하기전에 default 치장이 메모리에 셋팅된다.
			// bBuyCash와 iBuyPrice은 1개의 아이템만을 셋팅하는 정보이므로 나머지 아이템은 0페소로 강제 셋팅한다.
			// 이부분을 수정하기 위해서 index 값이 설정안되어 있는 모든 치장에 대한 정보를 가지고 있다가 셋팅해야한다.
			// 차후에 개선 고려해 볼것.
			bBuyCash  = false;
			iBuyPrice = 0;
		}
		else
		{
			g_LogDBClient.OnInsertDeco( pUser, kItemSlot[i].m_item_type, kItemSlot[i].m_item_code, iBuyPrice, szItemIndex, (LogDBClient::DecoType)iLogType );
			iBuyPrice = 0;
		}	
	}
}

void UserNodeManager::OnResultUpdateInvenData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateInvenData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}


void UserNodeManager::OnResultInsertEtcItemData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertEtcItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectEtcItemIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectEtcItemIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwIndex = 0;
	DWORD  dwUserIdx	= 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	bool   bBuyCash  = false;
	int    iBuyPrice = 0;
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );    //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx, sizeof( int ) );    
	query_data->GetValue( bBuyCash, sizeof( bool ) );      
	query_data->GetValue( iBuyPrice, sizeof( int ) );    
	query_data->GetValue( dwIndex, sizeof(int) );		 //새로운 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectEtcItemIndex USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioUserEtcItem *pEtcItem = pUser->GetUserEtcItem();
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectEtcItemIndex : (%d - %d) Etcitem is NULL.", dwUserIdx, dwIndex );
		return;
	}
	
	if( !pEtcItem->DBtoNewIndex( dwIndex ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectEtcItemIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwIndex );
		return;
	}
	
	ioUserEtcItem::ETCITEMSLOT kItemSlot[ ioUserEtcItem::MAX_SLOT ];
	if( !pEtcItem->GetRowEtcItem( dwIndex, kItemSlot ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectEtcItemIndex : (%d - %d) Index not found.", dwUserIdx, dwIndex );
		return;
	}

	for (int i = 0; i < ioUserEtcItem::MAX_SLOT ; i++)
	{
		if( kItemSlot[i].m_iType   == 0 && 
			kItemSlot[i].m_iValue1 == 0 &&
			kItemSlot[i].m_iValue2 == 0 )
			continue;

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwIndex, i+1 );

		if( bBuyCash )
		{
			if( iBuyPrice != 0 )
			{
				g_LogDBClient.OnInsertCashItem( pUser, kItemSlot[i].m_iType ,kItemSlot[i].m_iValue1, iBuyPrice, szItemIndex, LogDBClient::CIT_ETC, pUser->GetBillingGUID().c_str() );
			}
		}
		else
			g_LogDBClient.OnInsertEtc( pUser, kItemSlot[i].m_iType, kItemSlot[i].m_iValue1, iBuyPrice, szItemIndex, LogDBClient::ET_BUY );

		pUser->StartEtcItemTime(  __FUNCTION__ , kItemSlot[i].m_iType ); 
	}
}

void UserNodeManager::OnResultUpdateEtcItemData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateEtcItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectPresentData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectPresentData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex;
	query_data->GetValue( dwUserIndex, sizeof(LONG) );

	User *pUser = GetUserNode( dwUserIndex );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectPresentData USER FIND NOT! :%d", dwUserIndex );
		return;
	}	

	ioUserPresent *pUserPresent = pUser->GetUserPresent();
	if( pUserPresent )
		pUserPresent->DBtoPresentData( query_data );
}

void UserNodeManager::OnResultSelectUserIndexAndPresentCnt( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnRetultSelectUserIndexAndPresentCnt Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// 보낸 정보
	char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	DWORD  dwUserIndex = 0;
	short iPresentType = 0;
	int   iBuyValue1   = 0;
	int   iBuyValue2   = 0;
	char  szRecvPublicID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( iPresentType, sizeof(short) );
	query_data->GetValue( iBuyValue1, sizeof(int) );
	query_data->GetValue( iBuyValue2, sizeof(int) );
	query_data->GetValue( szRecvPublicID, ID_NUM_PLUS_ONE );
	
	// 받은 정보
	DWORD dwRecvUserIndex = 0;
	int   iRecvPresentCnt = 0;
	char  szRecvPrivateID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( dwRecvUserIndex, sizeof(DWORD) );
	query_data->GetValue( iRecvPresentCnt, sizeof(int) );
	query_data->GetValue( szRecvPrivateID, ID_NUM_PLUS_ONE ); 

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnRetultSelectUserIndexAndPresentCnt USER FIND NOT! %d", dwUserIndex);
		return;
	}

	if( pUser->GetUserIndex() != dwUserIndex )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnRetultSelectUserIndexAndPresentCnt USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIndex );
		return;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s Step 2 %d[%d:%d:%s/%d:%d:%d]", __FUNCTION__, dwUserIndex, dwRecvUserIndex, iRecvPresentCnt, szRecvPrivateID, iPresentType, iBuyValue1, iBuyValue2  );

	pUser->_OnDBPresentBuy( dwRecvUserIndex, iRecvPresentCnt, szRecvPrivateID, szRecvPublicID, iPresentType, iBuyValue1, iBuyValue2 );
}

void UserNodeManager::OnResultUpdateAwardExpert(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateAwardExpert Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultInsertAwardData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertAwardData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectAwardIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectAwardIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwInvenIdx = 0;
	DWORD dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );        //보낸 유저의 아이디.

	query_data->GetValue( dwUserIdx, sizeof(int) );
	query_data->GetValue( dwInvenIdx, sizeof(int) );		 //새로운 인벤토리 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectAwardIndex USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioAward *pAward = pUser->GetAward();
	if( pAward )
	{
		if( !pAward->DBtoNewIndex( dwInvenIdx ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectAwardIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwInvenIdx );
	}
}

void UserNodeManager::OnResultUpdateAwardData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateAwardData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}


void UserNodeManager::OnResultInsertGrowth(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertGrowth Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectGrowthIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGrowthIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwClassInfoIdx = 0;
	DWORD dwUserIdx = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char szUserID[ID_NUM_PLUS_ONE] = "";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );
	query_data->GetValue( dwUserIdx, sizeof(int) );
	query_data->GetValue( dwClassInfoIdx, sizeof(int) );

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectClassGrowth USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioUserGrowthLevel *pGrowthLevel = pUser->GetUserGrowthLevel();
	if( pGrowthLevel )
	{
		if( !pGrowthLevel->DBtoNewIndex( dwClassInfoIdx ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectGrowthIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwClassInfoIdx );
	}
}

void UserNodeManager::OnResultUpdateGrowth(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateGrowth Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultInsertFishData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertFishData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectFishDataIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectFishDataIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwClassInfoIdx = 0;
	DWORD dwUserIdx = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char szUserID[ID_NUM_PLUS_ONE] = "";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );        
	query_data->GetValue( dwUserIdx, sizeof(int) );
	query_data->GetValue( dwClassInfoIdx, sizeof(int) );

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectFishDataIndex USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioUserFishingItem *pFishItem = pUser->GetUserFishingItem();
	if( pFishItem )
	{
		if( !pFishItem->DBtoNewIndex( dwClassInfoIdx ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectFishDataIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwClassInfoIdx );
	}
}

void UserNodeManager::OnResultSelectAllFishData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectAllFishData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	int  iUserIndex = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char szUserID[ID_NUM_PLUS_ONE] = "";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue(szUserID,ID_NUM_PLUS_ONE);       //보낸 유저의 아이디.
	query_data->GetValue(iUserIndex,sizeof(int));		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(iUserIndex);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectAllFishData USER FIND NOT! :%d",iUserIndex);
		return;
	}
	if(pUser->GetUserIndex() != (DWORD)iUserIndex)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectAllFishData USER INDEX NOT! :%d - %d",pUser->GetUserIndex(),iUserIndex);
		return;
	}

	ioUserFishingItem *pFishItem = pUser->GetUserFishingItem();
	if( pFishItem )
	{
		pFishItem->DBtoData( query_data );
	}
}

void UserNodeManager::OnResultUpdateFishData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateFishData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultInsertExtraItemData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertExtraItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectExtraItemIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectExtraItemIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwInvenIdx = 0;
	DWORD  dwUserIdx  = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	bool   bBuyCash  = false;
	int    iBuyPrice = 0;
	int    iLogType = 0;
	int    iMachineCode = 0;
	int    iPeriodTime  = 0;
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );        //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx, sizeof( int ) );       
	query_data->GetValue( bBuyCash, sizeof( bool ) );      
	query_data->GetValue( iBuyPrice, sizeof( int ) );       
	query_data->GetValue( iLogType, sizeof( int ) );    
	query_data->GetValue( iMachineCode, sizeof( int ) );      
	query_data->GetValue( iPeriodTime,  sizeof( int ) );      

	query_data->GetValue( dwInvenIdx, sizeof(int) );		 //새로운 인벤토리 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectExtraItemIndex USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioUserExtraItem *pItem = pUser->GetUserExtraItem();
	if( !pItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectExtraItemIndex : (%d - %d) pItem == NULL.", dwUserIdx, dwInvenIdx );
		return;
	}

	if( !pItem->DBtoNewIndex( dwInvenIdx ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectExtraItemIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwInvenIdx );
		return;
	}

	
	ioUserExtraItem::EXTRAITEMSLOT kItemSlot[ ioUserExtraItem::MAX_SLOT ];
	if( !pItem->GetRowExtraItem( dwInvenIdx, kItemSlot ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectExtraItemIndex : (%d - %d) Index not found.", dwUserIdx, dwInvenIdx );
		return;
	}

	for (int i = 0; i < ioUserExtraItem::MAX_SLOT ; i++)
	{
		if( kItemSlot[i].m_iItemCode == 0 && kItemSlot[i].m_iIndex == 0 )
			continue;

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwInvenIdx, i+1 );

		if( bBuyCash )
		{
			if( iBuyPrice != 0 )
			{
				//g_LogDBClient.OnInsertCashItem( pUser, kItemSlot[i].m_iItemCode, ( kItemSlot[i].m_iReinforce*10000 ) + iPeriodTime, iBuyPrice, szItemIndex, (LogDBClient::CashItemType)iLogType, pUser->GetBillingGUID().c_str() ); // ( 장비 성장값 * 10000 ) + 장비기간. ex)장비기간값이 0이면 무제한 - 선물하기와 같은 구조  
			}
			bBuyCash  = false;
			iBuyPrice = 0;
		}
		else
		{
			g_LogDBClient.OnInsertExtraItem( pUser, kItemSlot[i].m_iItemCode, kItemSlot[i].m_iReinforce, iMachineCode, iPeriodTime, iBuyPrice, kItemSlot[i].m_PeriodType, kItemSlot[i].m_dwMaleCustom, kItemSlot[i].m_dwFemaleCustom, szItemIndex, (LogDBClient::ExtraType)iLogType );
			iBuyPrice = 0;
		}	
	}
	
}

void UserNodeManager::OnResultUpdateExtraItemData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateExtraItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultUpdateUserLogout(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserLogout Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultUpdateALLUserLogout( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateALLUserLogout Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectFriendList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectFriendList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	char szID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szID, ID_NUM_PLUS_ONE );       // 보낸 유저의 아이디.

	User *pUser = GetUserNodeByPublicID( szID );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectFriendList USER FIND NOT! :%s", szID );
		return;
	}

	LOOP_GUARD();

	// Check : 마지막 로그아웃 시간이 init_control 시간 전이면 삭제.
	bool bInit = g_ItemInitControl.CheckInitUserBeforeReceiveCloverByLogin( pUser );
		
	int iLastIndex = pUser->GetFriendLastIndex();
	while( query_data->IsExist() )
	{
		int  iIndex = 0;
		DWORD dwUserIndex = 0;
		char szName[ID_NUM_PLUS_ONE] = "";
		DBTIMESTAMP dts;

		int iSendCount = 0;
		int iSendDate = 0;
		int iReceiveCount = 0;
		int iBeforeReceiveDate = 0;
		int iBeforeReceiveCount = 0;
		bool bSave = false;

		query_data->GetValue( iIndex, sizeof(int) );			 //테이블 인덱스
		query_data->GetValue( dwUserIndex, sizeof(DWORD) );      //친구 인덱스
		query_data->GetValue( szName, ID_NUM_PLUS_ONE );		 //친구 닉네임	
		query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) );//친구 수락한 시간		                                    
		CTime RegTime( Help::GetSafeValueForCTimeConstructor( dts.year, dts.month, dts.day, dts.hour, dts.minute, dts.second ) );

		query_data->GetValue( iSendCount, sizeof( int ) );
		query_data->GetValue( iSendDate, sizeof( int ) );
		query_data->GetValue( iReceiveCount, sizeof( int ) );
		query_data->GetValue( iBeforeReceiveDate, sizeof( int ) );
		query_data->GetValue( iBeforeReceiveCount, sizeof( int ) );

		// Check : 마지막 로그아웃 시간이 init_control 시간 전이면 삭제.
		if( bInit )
		{
			if( iBeforeReceiveCount > 0 )
			{
				iBeforeReceiveDate = 0;
				iBeforeReceiveCount = 0;
				bSave = true;
			}
		}

		pUser->InsertFriend( iIndex, dwUserIndex, szName, -1, -1, Help::ConvertCTimeToDate( RegTime )
			, iSendCount, iSendDate, iReceiveCount, iBeforeReceiveDate, iBeforeReceiveCount, bSave );
	}
	LOOP_GUARD_CLEAR();

	pUser->SendFriendPacket( iLastIndex, true );	
}

void UserNodeManager::OnResultSelectFriendRequestList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectFriendRequestList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectFriendRequestList USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	DWORDVec vTableIndex;
	DWORDVec vUserIndex;
	ioHashStringVec vUserName;

	LOOP_GUARD();
	while( query_data->IsExist() )
	{
		DWORD dwTableIndex, dwUserIndex;
		char szUserID[ID_NUM_PLUS_ONE] = "";
		query_data->GetValue( dwTableIndex, sizeof(DWORD) );		//테이블 인덱스
		query_data->GetValue( dwUserIndex, sizeof(DWORD) );			//친구 인덱스
		query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );			//친구 닉네임	
		
		vTableIndex.push_back( dwTableIndex );
		vUserIndex.push_back( dwUserIndex );
		vUserName.push_back( szUserID );
	}
	LOOP_GUARD_CLEAR();

	int iMaxSize = min( min( vTableIndex.size(), vUserIndex.size() ), vUserName.size() );
	//유저에게 전송
	SP2Packet kPacket( STPK_FRIEND_REQUEST_LIST );
	kPacket << iMaxSize;
	for(int i = 0;i < iMaxSize;i++)
	{
		kPacket << vTableIndex[i] << vUserIndex[i] << vUserName[i];
	}
	pUserParent->RelayPacket( kPacket );

	vTableIndex.clear();
	vUserIndex.clear();
	vUserName.clear();
}

void UserNodeManager::OnResultSelectFriendApplication(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectFriendApplication Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex;
	char  szFriendID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( szFriendID, ID_NUM_PLUS_ONE );

	int iResult;
	query_data->GetValue( iResult, sizeof(int) );

	if( iResult == FRIEND_APPLICATION_OK )
	{
		UserParent *pFriend = GetGlobalUserNode( szFriendID );
		if( pFriend )
		{
			g_DBClient.OnSelectFriendRequestList( pFriend->GetUserDBAgentID(), pFriend->GetAgentThreadID(), MAX_FRIEND_IDX_VALUE, pFriend->GetUserIndex(), SEND_FRIEND_REQUEST_LIST );
		}
	}

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent )
	{
		SP2Packet kPacket( STPK_FRIEND_APPLICATION );
		kPacket << iResult << szFriendID;
		pUserParent->RelayPacket( kPacket );
	}
}

void UserNodeManager::OnResultSelectInsertFriend(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectInsertFriend Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex, dwTableIndex, dwFriendIndex;
	char szUserID[ID_NUM_PLUS_ONE] = "";
	char szFriendID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );
	query_data->GetValue( dwTableIndex, sizeof(DWORD) );
	query_data->GetValue( dwFriendIndex, sizeof(DWORD) );
	query_data->GetValue( szFriendID, ID_NUM_PLUS_ONE );

	int iResult;
	query_data->GetValue( iResult, sizeof(int) );

	if( iResult == FRIEND_INSERT_OK )
	{
		// 양쪽 모두에게 알려줘서 친구리스트 가져온다.
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_FRIEND_COMMAND );
			kPacket << FRIEND_COMMAND_AGREE << FRIEND_INSERT_OK << szFriendID;
			pUserParent->RelayPacket( kPacket );
		}

		UserParent *pFriendParent = GetGlobalUserNode( dwFriendIndex );
		if( pFriendParent )
		{
			SP2Packet kPacket( STPK_FRIEND_COMMAND );
			kPacket << FRIEND_COMMAND_AGREE << FRIEND_INSERT_OTHER_OK;
			pFriendParent->RelayPacket( kPacket );
		}
	}
	else
	{
		// 요청한 유저에게만 알린다.
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_FRIEND_COMMAND );
			kPacket << FRIEND_COMMAND_AGREE << iResult << szFriendID;
			pUserParent->RelayPacket( kPacket );
		}
	}
}

void UserNodeManager::OnResultDeleteFriend(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultDeleteFriend Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	char  szFriendID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szFriendID, ID_NUM_PLUS_ONE );

	char szUserID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szUserID, ID_NUM_PLUS_ONE);

	UserParent *pUserParent = GetGlobalUserNode( szUserID );
	if( pUserParent )
	{
		if( pUserParent->IsUserOriginal() )
		{
			User *pUser = (User*)pUserParent;

			pUser->_OnBestFriendDismiss( szFriendID, false );
			pUser->DeleteFriend( szFriendID );

			SP2Packet kPacket( STPK_FRIEND_DELETE );
			kPacket << szFriendID;
			pUser->SendMessage( kPacket );
			pUser->CheckRoomBonusTable();
		}
		else
		{
			UserCopyNode *pUser = (UserCopyNode*)pUserParent;
			SP2Packet kPacket( SSTPK_FRIEND_DELETE );
			kPacket << szUserID << szFriendID;
			pUser->SendMessage( kPacket );
		}
	}
}

void UserNodeManager::OnResultSelectFriendDeveloperInsert(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectFriendDeveloperInsert Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex;
	char  szFriendID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( szFriendID, ID_NUM_PLUS_ONE );

	int iResult;
	query_data->GetValue( iResult, sizeof(int) );

	if( iResult == FRIEND_INSERT_OK )
	{
		// 양쪽 모두에게 알려줘서 친구리스트 가져온다.
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_FRIEND_COMMAND );
			kPacket << FRIEND_COMMAND_AGREE << FRIEND_INSERT_OK << szFriendID;
			pUserParent->RelayPacket( kPacket );
		}

		UserParent *pFriendParent = GetGlobalUserNode( szFriendID );
		if( pFriendParent )
		{
			SP2Packet kPacket( STPK_FRIEND_COMMAND );
			kPacket << FRIEND_COMMAND_AGREE << FRIEND_INSERT_OTHER_OK;
			pFriendParent->RelayPacket( kPacket );
		}
	}
	else
	{
		// 요청한 유저에게만 알린다.
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_FRIEND_COMMAND );
			kPacket << FRIEND_COMMAND_AGREE << iResult << szFriendID;
			pUserParent->RelayPacket( kPacket );
		}
	}
}

void UserNodeManager::OnResultSelectBestFriendList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBestFriendList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );

	User *pUser = GetUserNode( dwUserIndex );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBestFriendList USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	LOOP_GUARD();
	while( query_data->IsExist() )
	{
		DWORD dwTableIndex, dwFriendUserIndex, dwState, dwMagicDate;
		PACKET_GUARD_VOID( query_data->GetValue( dwTableIndex, sizeof(DWORD) ) );		//절친 테이블 인덱스
		PACKET_GUARD_VOID( query_data->GetValue( dwFriendUserIndex, sizeof(DWORD) ) );	//절친 유저 인덱스
		PACKET_GUARD_VOID( query_data->GetValue( dwState, sizeof(DWORD) ) );				//절친 상태
		PACKET_GUARD_VOID( query_data->GetValue( dwMagicDate, sizeof(DWORD) ) );			//절친 시간값

		pUser->InsertBestFriend( dwTableIndex, dwFriendUserIndex, dwState, dwMagicDate );
	}
	LOOP_GUARD_CLEAR();

	// 유저에게 전송
	pUser->SendBestFriendPacket( 0 );
	pUser->SyncUserBestFriend();
}

void UserNodeManager::OnResultSelectBestFriendAdd(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBestFriendAdd Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex, dwFriendIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwFriendIndex, sizeof(DWORD) );

	User *pUser = GetUserNode( dwUserIndex );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBestFriendAdd USER FIND NOT! :%d", dwUserIndex );
		return;
	}
	
	DWORD dwTableIndex;
	query_data->GetValue( dwTableIndex, sizeof(DWORD) );
	if( dwTableIndex == 0 )
	{
		SP2Packet kPacket( STPK_INSERT_BESTFRIEND );
		kPacket << BESTFRIEND_INSERT_MEMBER_ERROR << dwFriendIndex;
		pUser->SendMessage( kPacket );
	}
	else
	{
		pUser->InsertBestFriend( dwTableIndex, dwFriendIndex, BFT_SET, 0 );
		pUser->SyncUserBestFriend();

		SP2Packet kPacket( STPK_INSERT_BESTFRIEND );
		kPacket << BESTFRIEND_INSERT_OK << dwTableIndex << dwFriendIndex;
		pUser->SendMessage( kPacket );

		// 대상에게 알림
		UserParent *pUserParent = GetGlobalUserNode( dwFriendIndex );
		if( pUserParent )
		{
			SP2Packet kOtherPacket( STPK_INSERT_BESTFRIEND );
			kOtherPacket << BESTFRIEND_INSERT_OTHER_OK << pUser->GetUserIndex();
			pUserParent->RelayPacket( kOtherPacket );
		}
	}
}

void UserNodeManager::OnResultSelectUserIDCheck(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserIDCheck Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	char szID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szID, ID_NUM_PLUS_ONE );       //보낸 유저의 아이디.

	User *pUser = GetUserNodeByPublicID( szID );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserIDCheck USER FIND NOT! :%s", szID );
		return;
	}

	char szPrivateID[ID_NUM_PLUS_ONE] = "";
	char szPublicID[ID_NUM_PLUS_ONE]  = "";
	query_data->GetValue( szPrivateID, ID_NUM_PLUS_ONE );
	query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE );

	int iAccountIndex = -1;
	query_data->GetValue( iAccountIndex, sizeof(iAccountIndex) );

	if( strcmp( szPrivateID, "" ) == 0 || strcmp( szPublicID, "" ) == 0 )
	{
		iAccountIndex = -1;	// 없는 유저
	}

	SP2Packet kPacket( STPK_REGISTERED_USER );
	kPacket << iAccountIndex;
	pUser->SendMessage( kPacket );
}

void UserNodeManager::OnResultSelectEventIndex( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectEventIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	char  szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	DWORD iUserIndex = 0;
	int   iEventType = 0;
	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	query_data->GetValue( iUserIndex,sizeof(DWORD) );     //유저인덱스
	query_data->GetValue( iEventType,sizeof(int) );       //보낸 event type.
	// select data
	DWORD dwEventIndex = 0;
	query_data->GetValue( dwEventIndex, sizeof(int) );

	User *pUser = GetUserNode(iUserIndex);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectEventIndex USER FIND NOT! :%d :%s",iUserIndex,szUserGUID);
		return;
	}
	if(pUser->GetUserIndex() != iUserIndex)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectEventIndex USER INDEX NOT! :%d : %d",pUser->GetUserIndex(),iUserIndex);
		return;
	}
	if( dwEventIndex == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectEventIndex Event INDEX Zero! :%d : %d",pUser->GetUserIndex(), iEventType );
		return;
	}

	EventUserManager &rEventUserMgr = pUser->GetEventUserMgr();
	rEventUserMgr.SetIndex( (EventType) iEventType, dwEventIndex );
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "New Event Index :%s:%d:%d", pUser->GetPublicID().c_str(), dwEventIndex, iEventType );
}

void UserNodeManager::OnResultUpdateEventData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateEventData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultUpdateUserRecord(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserRecord Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}


void UserNodeManager::OnResultInsertQuestData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertQuestData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectQuestIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectQuestIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwQuestIdx = 0;
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );        //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx, sizeof(int) );

	query_data->GetValue( dwQuestIdx, sizeof(int) );

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectQuestIndex USER FIND NOT! :%d", dwUserIdx );
		return;
	}

	ioQuest *pQuest = pUser->GetQuest();
	if( !pQuest )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectQuestIndex : (%d - %d) pQuest == NULL.", dwUserIdx, dwQuestIdx );
		return;
	}

	if( !pQuest->DBtoNewIndex( dwQuestIdx ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectQuestIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwQuestIdx );
		return;
	}
}

void UserNodeManager::OnResultUpdateQuestData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateQuestData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultInsertQuestCompleteData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertQuestCompleteData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectQuestCompleteIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectQuestCompleteIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwQuestIdx = 0;
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	//query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );        //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx, sizeof(int) );
	query_data->GetValue( dwQuestIdx, sizeof(int) );

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectQuestCompleteIndex USER FIND NOT! :%d", dwUserIdx );
		return;
	}

	ioQuest *pQuest = pUser->GetQuest();
	if( !pQuest )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectQuestCompleteIndex : (%d - %d) pQuest == NULL.", dwUserIdx, dwQuestIdx );
		return;
	}

	if( !pQuest->DBtoNewCompleteIndex( dwQuestIdx ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectQuestCompleteIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwQuestIdx );
		return;
	}
}

void UserNodeManager::OnResultUpdateQuestCompleteData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateQuestCompleteData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectCreateGuild(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCreateGuild Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	
	// Return Data
	DWORD dwUserIndex, dwItemTableIndex;
	int iResult, iItemFieldNum;	
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwItemTableIndex, sizeof(DWORD) );
	query_data->GetValue( iItemFieldNum, sizeof(int) );
	query_data->GetValue( iResult, sizeof(int) );

	DWORD dwAgentServerID = 0;
	DWORD dwAgentThreadID = 0;
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent )
	{
		dwAgentServerID = pUserParent->GetUserDBAgentID();
		dwAgentThreadID = pUserParent->GetAgentThreadID();
	}

	if( iResult == CREATE_GUILD_OK )
	{
		// 해당 길드의 인덱스를 가져와서 메인서버에 등록한다.
		g_DBClient.OnSelectCreateGuildReg( dwAgentServerID, dwAgentThreadID, dwUserIndex );
	}
	
	if( !pUserParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectCreateGuild USER FIND NOT! :%d", dwUserIndex );
		if( iResult == CREATE_GUILD_OK )
		{
			// 길드 생성이 성공했는데 유저가 로그아웃 상태라면 DB 아이템을 직접 삭제한다.
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "User Find Not CreateGuild OK: %d", dwUserIndex );
			g_DBClient.OnUpdateGuildEtcItemDelete( dwAgentServerID, dwAgentThreadID, dwUserIndex, iItemFieldNum, dwItemTableIndex );
		}
		return;
	}

	if( pUserParent->IsUserOriginal() )
	{
		User *pUser = (User*)pUserParent;
		if( iResult == CREATE_GUILD_OK )       //성공
		{
			g_DBClient.OnSelectCreateGuildInfo( dwAgentServerID, dwAgentThreadID, dwUserIndex );

			//길드 생성 권한 아이템 삭제
			ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
			if( pUserEtcItem )
			{
				pUserEtcItem->DeleteEtcItem( ioEtcItem::EIT_ETC_GUILD_CREATE, LogDBClient::ET_DEL );
			}
			SP2Packet kSuccess( STPK_ETCITEM_USE );
			kSuccess << ETCITEM_USE_OK;
			kSuccess << (int)ioEtcItem::EIT_ETC_GUILD_CREATE;
			pUser->SendMessage( kSuccess );
		}
		else
		{
			// 에러 알림
			SP2Packet kPacket( STPK_CREATE_GUILD );
			kPacket << iResult;
			pUser->SendMessage( kPacket );
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "CreateGuild Fail: %s - E(%d)", pUser->GetPublicID().c_str(), iResult );
		}
	}	
	else
	{
		// 해당 서버로 길드 생성 결과 알림
		UserCopyNode *pUser = (UserCopyNode*)pUserParent;
		SP2Packet kPacket( SSTPK_CREATE_GUILD_RESULT );
		kPacket << pUser->GetUserIndex() << iResult << dwItemTableIndex << iItemFieldNum;
		pUser->SendMessage( kPacket );
	}
}

void UserNodeManager::OnResultSelectCreateGuildReg(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectCreateGuildReg Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	DWORD dwGuildIndex;
	int   iGuildMark;
	char  szGuildName[GUILD_NAME_NUM_PLUS_ONE] = "";
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	query_data->GetValue( szGuildName, GUILD_NAME_NUM_PLUS_ONE );
	query_data->GetValue( iGuildMark, sizeof(int) );

	if( dwGuildIndex == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectCreateGuildReg Result FAILED! : 길드 인덱스 Zero!!!");
		return;
	}

	// 디폴트 세팅
	DWORD dwGuildRegDate = 0;
	char  szGuildTitle[GUILD_TITLE_NUMBER_PLUS_ONE] = "";

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetGuildTitle( szGuildTitle, sizeof( szGuildTitle ), szGuildName );
	CTime curTime = CTime::GetCurrentTime();
	dwGuildRegDate = (curTime.GetYear() * 10000) + (curTime.GetMonth() * 100) + curTime.GetDay();
	
	//메인 서버에 길드 등록	
	SP2Packet kPacket( MSTPK_GUILD_CREATE_REG );
	kPacket << dwGuildIndex << szGuildName << szGuildTitle << iGuildMark << GUILD_MAX_ENTRY_USER << dwGuildRegDate;
	g_MainServer.SendMessage( kPacket );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "GUILD REG : %d - %s - %d - %d", dwGuildIndex, szGuildName, iGuildMark, dwGuildRegDate );
}

void UserNodeManager::OnResultSelectCreateGuildInfo(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCreateGuildInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex = 0, dwGuildIndex = 0;
	int   iGuildMark = 0, iGuildEvent = 0;
	BYTE  byGuildRoom	= 0;
	BOOL  bActive		= FALSE;
	char  szGuildName[GUILD_NAME_NUM_PLUS_ONE] = "";
	char  szGuildPos[GUILD_POS_NUM_PLUS_ONE] = "";
	DBTIMESTAMP AttendRewardRecvDate;
	DBTIMESTAMP RankRewardRecvDate;
	DBTIMESTAMP JoinDate;

	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	query_data->GetValue( szGuildName, GUILD_NAME_NUM_PLUS_ONE );
	query_data->GetValue( iGuildMark, sizeof(int) );
	query_data->GetValue( szGuildPos, GUILD_POS_NUM_PLUS_ONE );
	query_data->GetValue( iGuildEvent, sizeof(int) );

	PACKET_GUARD_VOID( query_data->GetValue( (char*)&AttendRewardRecvDate, sizeof(DBTIMESTAMP) ) );
	PACKET_GUARD_VOID( query_data->GetValue( (char*)&RankRewardRecvDate, sizeof(DBTIMESTAMP) ) );
	PACKET_GUARD_VOID( query_data->GetValue( (char*)&JoinDate, sizeof(DBTIMESTAMP) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byGuildRoom, sizeof(BYTE) ) );

	if( 1 == byGuildRoom )
		bActive	= TRUE;

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectCreateGuildInfo USER FIND NOT! :%d", dwUserIndex );		
		return;
	}

	if( dwGuildIndex == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectCreateGuildInfo USER GUILD INDEX NOT! :%d", dwUserIndex );		
		return;
	}

	if( pUserParent->IsUserOriginal() )
	{
		User *pUser = (User*)pUserParent;
		SP2Packet kPacket( STPK_CREATE_GUILD );
		PACKET_GUARD_VOID( kPacket.Write(CREATE_GUILD_OK) );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetMoney()) );
		PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
		PACKET_GUARD_VOID( kPacket.Write(szGuildName) );
		PACKET_GUARD_VOID( kPacket.Write(iGuildMark) );
		PACKET_GUARD_VOID( kPacket.Write(szGuildPos) );
		PACKET_GUARD_VOID( kPacket.Write(bActive) );

		pUser->SendMessage( kPacket );

		ioUserGuild *pUserGuild = pUser->GetUserGuild();
		if( pUserGuild )
		{
			pUserGuild->SetGuildData( dwGuildIndex, szGuildName, szGuildPos, iGuildMark, true );
			pUserGuild->SetRecvRewardDate(AttendRewardRecvDate, RankRewardRecvDate);
			pUserGuild->SetJoinDate(JoinDate);
			pUserGuild->SetGuildRoomState(bActive);
		}
	}	
	else
	{
		// 해당 서버로 길드 정보 전송
		DWORD dwAttendRcvDate	= 0;
		DWORD dwRankRcvDate		= 0;
		DWORD dwJoinDate		= 0;

		dwAttendRcvDate  = g_GuildRewardMgr.RecvDateConvertDBTIMESTAMPToDWORD(AttendRewardRecvDate);
		dwRankRcvDate = g_GuildRewardMgr.RecvDateConvertDBTIMESTAMPToDWORD(RankRewardRecvDate);
		dwJoinDate	  = g_GuildRewardMgr.RecvDateConvertDBTIMESTAMPToDWORD(JoinDate);

		UserCopyNode *pUser = (UserCopyNode*)pUserParent;
		SP2Packet kPacket( SSTPK_CREATE_GUILD_COMPLETE );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetUserIndex()) );
		PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
		PACKET_GUARD_VOID( kPacket.Write(szGuildName) );
		PACKET_GUARD_VOID( kPacket.Write(iGuildMark) );
		PACKET_GUARD_VOID( kPacket.Write(szGuildPos) );
		PACKET_GUARD_VOID( kPacket.Write(dwAttendRcvDate) );
		PACKET_GUARD_VOID( kPacket.Write(dwRankRcvDate) );
		PACKET_GUARD_VOID( kPacket.Write(dwJoinDate) );
		PACKET_GUARD_VOID( kPacket.Write(bActive) );
		pUser->SendMessage( kPacket );
	}
}

void UserNodeManager::OnResultSelectUserGuildInfo(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserGuildInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD  dwUserIdx = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

    DWORD dwGuildIndex = 0;
	int   iGuildMark = 0, iGuildEvent = 0;
	BYTE  byGuildRoom	= 0;
	BOOL bActive	= FALSE;
	char  szGuildName[GUILD_NAME_NUM_PLUS_ONE] = "";
	char  szGuildPos[GUILD_POS_NUM_PLUS_ONE] = "";
	DBTIMESTAMP AttendRewardRecvDate;
	DBTIMESTAMP RankRewardRecvDate;
	DBTIMESTAMP JoinDate;

	if( query_data->GetResultCount() > 0 )
	{
		PACKET_GUARD_VOID( query_data->GetValue( dwGuildIndex, sizeof(DWORD) ) );
		PACKET_GUARD_VOID( query_data->GetValue( szGuildName, GUILD_NAME_NUM_PLUS_ONE ) );
		PACKET_GUARD_VOID( query_data->GetValue( iGuildMark, sizeof(int) ) );
		PACKET_GUARD_VOID( query_data->GetValue( szGuildPos, GUILD_POS_NUM_PLUS_ONE ) );
		PACKET_GUARD_VOID( query_data->GetValue( iGuildEvent, sizeof(int) ) );
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&AttendRewardRecvDate, sizeof(DBTIMESTAMP) ) );
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&RankRewardRecvDate, sizeof(DBTIMESTAMP) ) );
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&JoinDate, sizeof(DBTIMESTAMP) ) );
		PACKET_GUARD_VOID( query_data->GetValue( byGuildRoom, sizeof(BYTE) ) );
	}

	if( 1 == byGuildRoom )
		bActive	= TRUE;

	User *pUser = GetUserNode( dwUserIdx );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserGuildInfo USER FIND NOT! :%d", dwUserIdx );	
		return;
	}

	DWORD dwAgentServerID = pUser->GetUserDBAgentID();
	DWORD dwAgentThreadID = pUser->GetAgentThreadID();
	if( dwGuildIndex == 0 )        //길드 없음
	{
		// 길드 가입 신청한 리스트 가져옴
		g_DBClient.OnSelectEntryDelayGuildList( dwAgentServerID, dwAgentThreadID, dwUserIdx );
		return;
	}
	
	ioUserGuild *pUserGuild = pUser->GetUserGuild();
	if( pUserGuild )
	{
		pUserGuild->SetGuildData( dwGuildIndex, szGuildName, szGuildPos, iGuildMark, false );
		pUserGuild->SetRecvRewardDate(AttendRewardRecvDate, RankRewardRecvDate);
		pUserGuild->SetJoinDate(JoinDate);
		pUserGuild->SetGuildRoomState(bActive);

		//유저에게 길드 패킷 전송
		CTime cCurTime = CTime::GetCurrentTime();
		DWORD dwCurTime	= cCurTime.GetTime();

		SP2Packet kPacket( STPK_MY_GUILD_INFO );
		PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
		PACKET_GUARD_VOID( kPacket.Write(szGuildName) );
		PACKET_GUARD_VOID( kPacket.Write(szGuildPos) );
		PACKET_GUARD_VOID( kPacket.Write(iGuildMark) );
		PACKET_GUARD_VOID( kPacket.Write(iGuildEvent) );
		PACKET_GUARD_VOID( kPacket.Write(pUserGuild->GetRecvAttendRewardDate()) );
		PACKET_GUARD_VOID( kPacket.Write(pUserGuild->GetGuildJoinDate()) );
		PACKET_GUARD_VOID( kPacket.Write(dwCurTime) );
		PACKET_GUARD_VOID( kPacket.Write(bActive) );

		pUser->SendMessage( kPacket );

		switch( iGuildEvent )
		{
		case ioUserGuild::GUILD_DELETE:
		case ioUserGuild::GUILD_LEAVE:
			{
				// 길드에서 탈퇴되었거나 길드가 없어졌음.
				g_DBClient.OnDeleteGuildMemberDelete( dwAgentServerID, dwAgentThreadID, dwUserIdx, dwGuildIndex );
				pUserGuild->InitMyGuildData( true );
			}
			return;
		}		
		
		g_DBClient.OnUpdateGuildMemberEvent( dwAgentServerID, dwAgentThreadID, dwUserIdx, dwGuildIndex );

		if( pUserGuild->IsGuildMaster() || pUserGuild->IsGuildSecondMaster() )
		{
			// 우리 길드의 가입 대기자 리스트 가져옴
			g_DBClient.OnSelectGuildEntryDelayMember( dwAgentServerID, dwAgentThreadID, dwUserIdx, dwGuildIndex );			
		}

		// 메인 서버에 길드 상세 정보 요청
		bool bLoginInfo = true;
		SP2Packet kPacket2( MSTPK_GUILD_INFO );
		PACKET_GUARD_VOID( kPacket2.Write(bLoginInfo) );
		PACKET_GUARD_VOID( kPacket2.Write(dwUserIdx) );
		PACKET_GUARD_VOID( kPacket2.Write(dwGuildIndex) );

		g_MainServer.SendMessage( kPacket2 );

		// 유저리스트를 불러온다.
		g_DBClient.OnSelectGuildMemberListEx( dwAgentServerID, dwAgentThreadID, dwUserIdx, dwGuildIndex, 0, true );

		// 로그인 완료된 다음부터 타서버에 동기화
		if( pUser->IsConnectProcessComplete() )
			pUser->SyncUserGuild();
	}	
}

void UserNodeManager::OnResultSelectEntryDelayGuildList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectEntryDelayGuildList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );

	User *pUser = GetUserNode( dwUserIndex );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectEntryDelayGuildList USER FIND NOT! :%d", dwUserIndex );	
		return;
	}

	LOOP_GUARD();
	DWORDVec vGuildList;
	while( query_data->IsExist() )
	{		
		DWORD dwGuildIndex = 0;
		query_data->GetValue( dwGuildIndex, sizeof(int) ); //길드 인덱스
		vGuildList.push_back( dwGuildIndex );
	}	
	LOOP_GUARD_CLEAR();

	if( vGuildList.empty() ) return;

	int iSize = vGuildList.size();
	SP2Packet kPacket( STPK_ENTRY_DELAY_GUILD_LIST );
	kPacket << iSize;
	for(int i = 0;i < iSize;i++)
	{
		kPacket << vGuildList[i];
	}
	pUser->SendMessage( kPacket );
	vGuildList.clear();
}

void UserNodeManager::OnResultSelectGuildEntryDelayMember(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildEntryDelayMember Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex, dwGuildIndex;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwGuildIndex, sizeof(DWORD) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectGuildEntryDelayMember USER FIND NOT! :%d", dwUserIndex );	
		return;
	}

	int iCount = 0;
	DWORD dwTableIndex[GUILD_MAX_ENTRY_DELAY_USER];
	DWORD dwMemberIndex[GUILD_MAX_ENTRY_DELAY_USER];
	int   iMemberLevel[GUILD_MAX_ENTRY_DELAY_USER];
	char  szMemberID[GUILD_MAX_ENTRY_DELAY_USER][ID_NUM_PLUS_ONE];	
	memset( dwTableIndex, 0, sizeof( dwTableIndex ) );
	memset( dwMemberIndex, 0, sizeof( dwMemberIndex ) );
	memset( iMemberLevel, 0, sizeof( iMemberLevel ) );
	memset( szMemberID, 0, sizeof( szMemberID ) );
	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		PACKET_GUARD_BREAK( query_data->GetValue( dwTableIndex[iCount], sizeof(int) ) );	//테이블 인덱스
		PACKET_GUARD_BREAK( query_data->GetValue( dwMemberIndex[iCount], sizeof(int) ) );	//유저 인덱스
		PACKET_GUARD_BREAK( query_data->GetValue( iMemberLevel[iCount], sizeof(int) ) );	//유저 레벨
		PACKET_GUARD_BREAK( query_data->GetValue( szMemberID[iCount], ID_NUM_PLUS_ONE ) );//유저 닉네임
		iCount++;
		if( iCount >= GUILD_MAX_ENTRY_DELAY_USER )
			break;
	}
	LOOP_GUARD_CLEAR();

	if( iCount == 0 ) return;

	SP2Packet kPacket( STPK_GUILD_ENTRY_DELAY_MEMBER );
	PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
	PACKET_GUARD_VOID( kPacket.Write(iCount) );		 

	for(int i = 0;i < iCount;i++)
	{
		PACKET_GUARD_VOID( kPacket.Write(dwTableIndex[i]) );
		PACKET_GUARD_VOID( kPacket.Write( dwMemberIndex[i]) );
		PACKET_GUARD_VOID( kPacket.Write( iMemberLevel[i]) ); 
		PACKET_GUARD_VOID( kPacket.Write( szMemberID[i]) );
	}
	pUserParent->RelayPacket( kPacket );
}

void UserNodeManager::OnResultSelectGuildMemberList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildMemberList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex, dwGuildIndex, dwGuildJoinUser;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildJoinUser, sizeof(DWORD) );

	User *pUser = GetUserNode( dwUserIndex );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectGuildMemberList USER FIND NOT! :%d", dwUserIndex );	
		return;
	}

	LOOP_GUARD();
	static vTempUserData vTempUserData;
	vTempUserData.clear();

	while( query_data->IsExist() )
	{		
		TempUserData kTempUserData;
		query_data->GetValue( kTempUserData.m_dwTableIndex, sizeof(int) );			//테이블 인덱스
		query_data->GetValue( kTempUserData.m_dwUserIndex, sizeof(int) );			//유저 인덱스
		query_data->GetValue( kTempUserData.m_iGradeLevel, sizeof(int) );			//유저 레벨
		query_data->GetValue( kTempUserData.m_szUserID, ID_NUM_PLUS_ONE );			//유저 닉네임
		query_data->GetValue( kTempUserData.m_szUserPos, GUILD_POS_NUM_PLUS_ONE );	//유저 직책
		query_data->GetValue( kTempUserData.m_iLadderPoint, sizeof(int) );			//유저 진영포인트
		
		if( kTempUserData.m_szUserID.IsEmpty() ) continue;

		vTempUserData.push_back( kTempUserData );
	}
	LOOP_GUARD_CLEAR();

	if( vTempUserData.empty() ) return;

	SP2Packet kPacket( STPK_GUILD_MEMBER_LIST );
	kPacket << dwGuildIndex << (int)vTempUserData.size();
	for(int i = 0;i < (int)vTempUserData.size();i++)
	{
		TempUserData &kTempUserData = vTempUserData[i];
		
		UserParent *pUserParent = GetGlobalUserNode( kTempUserData.m_dwUserIndex );
		if( pUserParent )
			kTempUserData.m_iLadderPoint = pUserParent->GetLadderPoint();

		kPacket << kTempUserData.m_dwTableIndex << kTempUserData.m_dwUserIndex << kTempUserData.m_iGradeLevel
				<< kTempUserData.m_szUserID << kTempUserData.m_szUserPos << kTempUserData.m_iLadderPoint;
	}
	pUser->SendMessage( kPacket );

	if( (int)vTempUserData.size() != (int)dwGuildJoinUser )
	{
		SP2Packet kMainPacket( MSTPK_GUILD_JOIN_USER );
		kMainPacket << dwGuildIndex << (int)vTempUserData.size();
		g_MainServer.SendMessage( kMainPacket );
	}
	vTempUserData.clear();
}

void UserNodeManager::OnResultSelectGuildMemberListEx(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildMemberListEx Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex, dwGuildIndex, dwGuildJoinUser;
	bool bMemberUpdateCheck;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwGuildIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwGuildJoinUser, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( bMemberUpdateCheck, sizeof(bool) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guild]Guild member info does not exist : [%d]", dwUserIndex );	
		return;
	}

    if( pUserParent->IsUserOriginal() )
	{
		// 자신의 길드원 정보 세팅
		User *pUser = (User*)pUserParent;
		ioUserGuild *pUserGuild = pUser->GetUserGuild();
		if( pUserGuild )
		{
			if( pUserGuild->GetGuildIndex() == dwGuildIndex) 
			{
				pUserGuild->SetGuildUserData( query_data );

				if( bMemberUpdateCheck )
				{
					if( pUserGuild->GetGuildMemberCount() != (int)dwGuildJoinUser )
					{
						SP2Packet kMainPacket( MSTPK_GUILD_JOIN_USER );
						
						PACKET_GUARD_VOID( kMainPacket.Write(dwGuildIndex) );
						PACKET_GUARD_VOID( kMainPacket.Write(pUserGuild->GetGuildMemberCount()) );

						g_MainServer.SendMessage( kMainPacket );
					}
				}				
			}
		}	
	}
	else
	{
		// 유저가 있는 서버에 전송하여 길드원 정보를 요청한다.
		UserCopyNode *pUser = (UserCopyNode*)pUserParent;
		SP2Packet kPacket( SSTPK_GUILD_MEMBER_LIST_EX );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetUserIndex()) );
		PACKET_GUARD_VOID( kPacket.Write(SSTPK_GUILD_MEMBER_LIST_ME) );
		pUser->SendMessage( kPacket );
	}
}

void UserNodeManager::OnResultSelectGuildMarkBlockInfo(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildMarkBlockInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	char szDeveloperID[ID_NUM_PLUS_ONE] = "";
	DWORD dwGuildIndex, dwGuildMark;
	query_data->GetValue( szDeveloperID, ID_NUM_PLUS_ONE );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildMark, sizeof(DWORD) );

	DWORD dwAgentServerID = 0;
	DWORD dwAgentThreadID = 0;
	UserParent *pDeveloperUser = GetGlobalUserNode( szDeveloperID );
	if( pDeveloperUser )
	{
		dwAgentServerID = pDeveloperUser->GetUserDBAgentID();
		dwAgentThreadID = pDeveloperUser->GetAgentThreadID();
	}
	DWORD dwChangeMark = max( 100, dwGuildMark ) + 1;	//예약된 길드마크 100
	// 웹 URL 호출
	char szCreateURL[MAX_PATH] = "";
	sprintf_s( szCreateURL, Help::GetGuildMarkBlockURL().c_str(), dwGuildIndex, dwChangeMark );
	if( !Help::CallURL( szCreateURL ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s님께서 (%d)길드의 길드마크 블럭 하려다 웹페이지 호출 실패함", szDeveloperID, dwGuildIndex );
		if( pDeveloperUser )
		{
			SP2Packet kPacket( STPK_GUILD_MARK_CHANGE );
			kPacket << GUILD_MARK_CHANGE_NOT_DEVELOPER;
			pDeveloperUser->RelayPacket( kPacket );
		}
		return;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s님께서 (%d)길드의 길드마크 블럭 : %d - > %d", szDeveloperID, dwGuildIndex, dwGuildMark, dwChangeMark );
	// DB Update
	g_DBClient.OnUpdateGuildMarkChange( dwAgentServerID, dwAgentThreadID, dwGuildIndex, dwChangeMark );

	// 메인 서버에 전송
	SP2Packet kMainPacket( MSTPK_GUILD_MARK_CHANGE );
	kMainPacket << dwGuildIndex << dwChangeMark;
	g_MainServer.SendMessage( kMainPacket );

	// 길드원 인덱스.
	LOOP_GUARD();
	while( query_data->IsExist() )
	{
		DWORD dwUserIndex = 0;
		query_data->GetValue( dwUserIndex, sizeof(DWORD) );		//유저 인덱스

		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUser = (User*)pUserParent;
				pUser->_OnGuildMarkChange( dwGuildIndex, dwChangeMark, true );
			}
			else
			{
				UserCopyNode *pUser = (UserCopyNode*)pUserParent;
				SP2Packet kServerPacket( SSTPK_GUILD_MARK_CHANGE );
				kServerPacket << pUser->GetUserIndex() << dwGuildIndex << dwChangeMark << true;
				pUser->SendMessage( kServerPacket );
			}
		}
	}
	LOOP_GUARD_CLEAR();

	// 개발자에게 성공 알림
	if( pDeveloperUser )
	{
		SP2Packet kPacket( STPK_GUILD_MARK_CHANGE );
		kPacket << GUILD_MARK_CHANGE_OK_DEVELOPER << dwGuildIndex << dwChangeMark;
		pDeveloperUser->RelayPacket( kPacket );
	}
}


void UserNodeManager::OnResultSelectGuildNameChange(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildNameChange Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex, dwGuildIndex, dwItemTableIndex;
	int iResult, iItemFieldNum;	
	char szGuildName[GUILD_NAME_NUM_PLUS_ONE] = "";
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	query_data->GetValue( szGuildName, GUILD_NAME_NUM_PLUS_ONE );	
	query_data->GetValue( dwItemTableIndex, sizeof(DWORD) );
	query_data->GetValue( iItemFieldNum, sizeof(int) );
	query_data->GetValue( iResult, sizeof(int) );

	if( iResult == GUILD_NAME_CHANGE_OK )
	{
		// 메인 서버에 길드명 변경 알림
		SP2Packet kMainPacket( MSTPK_GUILD_NAME_CHANGE );
		kMainPacket << dwGuildIndex << szGuildName;
		g_MainServer.SendMessage( kMainPacket );	
	}
	
	DWORD dwAgentServerID = 0;
	DWORD dwAgentThreadID = 0;
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent )
	{
		dwAgentServerID = pUserParent->GetUserDBAgentID();
		dwAgentThreadID = pUserParent->GetAgentThreadID();
	}

	if( !pUserParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectGuildNameChange USER FIND NOT! :%d", dwUserIndex );
		if( iResult == GUILD_NAME_CHANGE_OK )
		{
			// 길드명 변경이 성공했는데 유저가 로그아웃 상태라면 DB 아이템을 직접 삭제한다.
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User Find Not GuildNameChange OK: %d", dwUserIndex );
			g_DBClient.OnUpdateGuildEtcItemDelete( dwAgentServerID, dwAgentThreadID, dwUserIndex, iItemFieldNum, dwItemTableIndex );
		}
		return;
	}

	if( pUserParent->IsUserOriginal() )
	{
		User *pUser = (User*)pUserParent;
		if( iResult == GUILD_NAME_CHANGE_OK )       //성공
		{
			ioUserGuild *pUserGuild = pUser->GetUserGuild();
			if( !pUserGuild || !pUser->IsGuild() )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "길드없는 유저가 길드명 변경  치명적인 에러 1: %s - %d:%s", pUser->GetPublicID().c_str(), dwGuildIndex, szGuildName );
				return;
			}

			//길드명 변경 권한 아이템 삭제
			ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
			if( pUserEtcItem )
			{
				pUserEtcItem->DeleteEtcItem( ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE, LogDBClient::ET_DEL );
			}
			// 권한 아이템 삭제
			SP2Packet kSuccess( STPK_ETCITEM_USE );
			kSuccess << ETCITEM_USE_OK;
			kSuccess << ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE;
			pUser->SendMessage( kSuccess );

			// 길드원들에게 길드명 동기화
			pUserGuild->SetGuildName( szGuildName );
			SP2Packet kUserPacket( STPK_GUILD_NAME_CHANGE );
			kUserPacket << GUILD_NAME_CHANGE_OK << pUser->GetUserIndex() << pUserGuild->GetGuildIndex() << pUserGuild->GetGuildName();
			pUser->SendMessage( kUserPacket );
			pUserGuild->GuildNameChangeSync();
		}
		else
		{
			// 에러 알림
			SP2Packet kPacket( STPK_GUILD_NAME_CHANGE );
			kPacket << iResult << pUser->GetUserIndex();
			pUser->SendMessage( kPacket );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GuildNameChange Fail: %s - E(%d)", pUser->GetPublicID().c_str(), iResult );
		}
	}	
	else
	{
		// 해당 서버로 길드명 변경 알림 
		UserCopyNode *pUser = (UserCopyNode*)pUserParent;
		SP2Packet kPacket( SSTPK_GUILD_NAME_CHANGE_RESULT );
		kPacket << pUser->GetUserIndex() << iResult << dwGuildIndex << szGuildName << dwItemTableIndex << iItemFieldNum;
		pUser->SendMessage( kPacket );
	}
}

void UserNodeManager::OnResultSelectGuildEntryApp(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildEntryApp Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex, dwGuildIndex, dwMasterIndex, dwSecondMasterIndex;
	int iResult = ENTRY_GUILD_APP_NONE_FORMALITY;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	query_data->GetValue( dwMasterIndex, sizeof(DWORD) );
	query_data->GetValue( dwSecondMasterIndex, sizeof(DWORD) );
	if(query_data->GetResultCount() > 0)
	{
		query_data->GetValue( iResult, sizeof(int) );
	}

	DWORD dwAgentServerID = 0;
	DWORD dwAgentThreadID = 0;
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent )         //신청한 유저에게 결과 전송
	{
		SP2Packet kPacket( STPK_GUILD_ENTRY_APP );
		kPacket << iResult << dwGuildIndex;
		pUserParent->RelayPacket( kPacket );

		dwAgentServerID = pUserParent->GetUserDBAgentID();
		dwAgentThreadID = pUserParent->GetAgentThreadID();
	}

	if( iResult == ENTRY_GUILD_APP_OK )
	{
		//신청에 성공했으면 길드장과 부길드장에게 대기자 명단 전송.
		if( dwMasterIndex != 0 && GetGlobalUserNode( dwMasterIndex ) )
			g_DBClient.OnSelectGuildEntryDelayMember( dwAgentServerID, dwAgentThreadID, dwMasterIndex, dwGuildIndex );
		if( dwSecondMasterIndex != 0 && GetGlobalUserNode( dwSecondMasterIndex ) )
			g_DBClient.OnSelectGuildEntryDelayMember( dwAgentServerID, dwAgentThreadID, dwSecondMasterIndex, dwGuildIndex );
	}
}

void UserNodeManager::OnResultSelectGuildEntryAppMasterGet(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildEntryAppMasterGet Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex, dwGuildIndex, dwMasterIndex, dwSecondMasterIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );

	LOOP_GUARD();
	dwMasterIndex = dwSecondMasterIndex = 0;
	while( query_data->IsExist() )
	{		
		DWORD dwUserIndex = 0;
		query_data->GetValue(dwUserIndex,sizeof(int));			//유저 인덱스
		if( dwMasterIndex == 0 )
			dwMasterIndex = dwUserIndex;
		else if( dwSecondMasterIndex == 0 )
			dwSecondMasterIndex = dwUserIndex;
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildEntryAppMasterGet Result while Faile" );
			break;
		}
	}
	LOOP_GUARD_CLEAR();

	DWORD dwAgentServerID = 0;
	DWORD dwAgentThreadID = 0;
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent )
	{
		dwAgentServerID = pUserParent->GetUserDBAgentID();
		dwAgentThreadID = pUserParent->GetAgentThreadID();
	}

	g_DBClient.OnSelectGuildEntryApp( dwAgentServerID, dwAgentThreadID, dwUserIndex, dwGuildIndex, dwMasterIndex, dwSecondMasterIndex );
}

void UserNodeManager::OnResultSelectGuildEntryAgree(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildEntryAgree Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex	= 0, dwGuildIndex	= 0, dwEntryUserIndex	= 0;
	int   iResult	= 0;

	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	query_data->GetValue( dwEntryUserIndex, sizeof(DWORD) );
	query_data->GetValue( iResult, sizeof(int) );

	// 승인한 유저
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( iResult == ENTRY_GUILD_AGREE_OK )
	{
		if( pUserParent && pUserParent->IsUserOriginal() )
		{
			User *pUser = (User*)pUserParent;
			ioUserGuild *pUserGuild = pUser->GetUserGuild();
			if( pUserGuild )
			{
				pUserGuild->GuildEntryAgreeSync();      // 접속중인 길드원들 전부 길드원 리스트 갱신.
			}	
		}		
		else if( pUserParent )
		{
			UserCopyNode *pUser = (UserCopyNode*)pUserParent;
			SP2Packet kPacket( SSTPK_GUILD_MEMBER_LIST_EX );
			kPacket << pUser->GetUserIndex() << SSTPK_GUILD_MEMBER_LIST_ALL;
			pUser->SendMessage( kPacket );
		}

		// 메인 서버에 가입 전송
// 		SP2Packet kMainPacket( MSTPK_GUILD_ENTRY_AGREE );
// 		kMainPacket << dwGuildIndex;
// 		g_MainServer.SendMessage( kMainPacket );
		
		// 길드 가입된 유저에게 전송
		UserParent *pEntryUser = GetGlobalUserNode( dwEntryUserIndex );
		if( pEntryUser )
		{		
			g_DBClient.OnSelectGuildEntryAgreeUserGuildInfo( pEntryUser->GetUserDBAgentID(), pEntryUser->GetAgentThreadID(), dwEntryUserIndex );
		}
	}   
	else
	{
		SP2Packet kPacket(MSTPK_GUILD_ENTRY_FAIL);
		PACKET_GUARD_VOID(kPacket.Write(dwGuildIndex));
		g_MainServer.SendMessage(kPacket);
	}

	if( pUserParent )
	{
		SP2Packet kPacket( STPK_GUILD_ENTRY_AGREE );
		kPacket << iResult;
        pUserParent->RelayPacket( kPacket );
	}	
}

void UserNodeManager::OnResultSelectGuildEntryAgreeUserGuildInfo(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildEntryAgreeUserGuildInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex	= 0, dwGuildIndex	= 0;
	int   iGuildMark	= 0, iGuildEvent	= 0;
	char  szGuildName[GUILD_NAME_NUM_PLUS_ONE] = "";
	char  szGuildPos[GUILD_POS_NUM_PLUS_ONE] = "";
	BYTE byGuildRoom	= 0;
	BOOL bActive		= FALSE;	
	DBTIMESTAMP AttendRewardRecvDate;
	DBTIMESTAMP RankRewardRecvDate;
	DBTIMESTAMP JoinDate;

	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	query_data->GetValue( szGuildName, GUILD_NAME_NUM_PLUS_ONE );
	query_data->GetValue( iGuildMark, sizeof(int) );
	query_data->GetValue( szGuildPos, GUILD_POS_NUM_PLUS_ONE );
	query_data->GetValue( iGuildEvent, sizeof(int) );
	PACKET_GUARD_VOID( query_data->GetValue( (char*)&AttendRewardRecvDate, sizeof(DBTIMESTAMP) ) );
	PACKET_GUARD_VOID( query_data->GetValue( (char*)&RankRewardRecvDate, sizeof(DBTIMESTAMP) ) );
	PACKET_GUARD_VOID( query_data->GetValue( (char*)&JoinDate, sizeof(DBTIMESTAMP) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byGuildRoom, sizeof(BYTE) ) );

	if( 1 == byGuildRoom )
		bActive	= TRUE;

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectGuildEntryAgreeUserGuildInfo USER FIND NOT! :%d", dwUserIndex );	
		return;
	}

	if( dwGuildIndex == 0 )        //길드 없음
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectGuildEntryAgreeUserGuildInfo Not Guild : %d", dwUserIndex );	
		return;
	}

	if( pUserParent->IsUserOriginal() )
	{
		User *pUser = (User*)pUserParent;
		ioUserGuild *pUserGuild = pUser->GetUserGuild();
		if( pUserGuild )
		{
			pUserGuild->SetGuildDataEntryAgree( dwGuildIndex, szGuildName, szGuildPos, iGuildMark, true );
			pUserGuild->SetRecvRewardDate(AttendRewardRecvDate, RankRewardRecvDate);
			pUserGuild->SetJoinDate(JoinDate);
			pUserGuild->SetGuildRoomState(bActive);
			CTime cCurTime = CTime::GetCurrentTime();
			DWORD dwCurTime	= cCurTime.GetTime();

			//유저에게 길드 패킷 전송
			SP2Packet kPacket( STPK_MY_GUILD_INFO );
			PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
			PACKET_GUARD_VOID( kPacket.Write(szGuildName) );
			PACKET_GUARD_VOID( kPacket.Write(szGuildPos) );
			PACKET_GUARD_VOID( kPacket.Write(iGuildMark) );
			PACKET_GUARD_VOID( kPacket.Write(iGuildEvent) );
			PACKET_GUARD_VOID( kPacket.Write(pUserGuild->GetRecvAttendRewardDate()) );
			PACKET_GUARD_VOID( kPacket.Write(pUserGuild->GetGuildJoinDate()) );
			PACKET_GUARD_VOID( kPacket.Write(dwCurTime) );
			PACKET_GUARD_VOID( kPacket.Write(bActive) );
			pUser->SendMessage( kPacket );

			g_DBClient.OnUpdateGuildMemberEvent( pUser->GetUserDBAgentID(), pUser->GetUserDBAgentID(), dwUserIndex, dwGuildIndex );
			g_DBClient.OnSelectGuildMemberListEx( pUser->GetUserDBAgentID(), pUser->GetUserDBAgentID(), dwUserIndex, dwGuildIndex, 0, false );
		}	
	}	
	else
	{
		DWORD dwAttendRcvDate	= 0;
		DWORD dwRankRcvDate		= 0;
		DWORD dwJoinDate		= 0;

		dwAttendRcvDate  = g_GuildRewardMgr.RecvDateConvertDBTIMESTAMPToDWORD(AttendRewardRecvDate);
		dwRankRcvDate = g_GuildRewardMgr.RecvDateConvertDBTIMESTAMPToDWORD(RankRewardRecvDate);
		dwJoinDate	  = g_GuildRewardMgr.RecvDateConvertDBTIMESTAMPToDWORD(JoinDate);

		UserCopyNode *pUser = (UserCopyNode*)pUserParent;
		//해당 서버로 정보 전송
		SP2Packet kPacket( SSTPK_GUILD_ENTRY_AGREE );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetUserIndex()) );
		PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
		PACKET_GUARD_VOID( kPacket.Write(szGuildName) );
		PACKET_GUARD_VOID( kPacket.Write(szGuildPos) );
		PACKET_GUARD_VOID( kPacket.Write(iGuildMark) );
		PACKET_GUARD_VOID( kPacket.Write(iGuildEvent) );
		PACKET_GUARD_VOID( kPacket.Write(dwAttendRcvDate) );
		PACKET_GUARD_VOID( kPacket.Write(dwRankRcvDate) );
		PACKET_GUARD_VOID( kPacket.Write(dwJoinDate) );
		PACKET_GUARD_VOID( kPacket.Write(bActive) );

		pUser->SendMessage( kPacket );
	}
}

void UserNodeManager::OnResultUpdateGuildMasterChange(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateGuildMasterChange Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex, dwTargetIndex, dwGuildIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTargetIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );

	{
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
			{
				SP2Packet kPacket( STPK_GUILD_MASTER_CHANGE );
				kPacket << GUILD_MASTER_CHANGE_OK << dwTargetIndex << ioHashString( pLocal->GetGuildGeneralPosition() );
				pUserParent->RelayPacket( kPacket );

				if( pUserParent->IsUserOriginal() )
				{
					User *pUser = (User*)pUserParent;
					ioUserGuild *pUserGuild = pUser->GetUserGuild();
					if( pUserGuild )
						pUserGuild->SetGuildPosition( ioHashString( pLocal->GetGuildGeneralPosition() ) );
				}
				else
				{
					UserCopyNode *pUser = (UserCopyNode*)pUserParent;
					SP2Packet kServerPacket( SSTPK_GUILD_POSITION_CHANGE );
					kServerPacket << pUser->GetUserIndex() << ioHashString( pLocal->GetGuildGeneralPosition() );
					pUser->SendMessage( kServerPacket );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		UserParent *pTargetParent = GetGlobalUserNode( dwTargetIndex );
		if( pTargetParent )
		{
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
			{
				SP2Packet kPacket( STPK_GUILD_MASTER_CHANGE );
				kPacket << GUILD_MASTER_CHANGE_OK << dwTargetIndex << ioHashString( pLocal->GetGuildMasterPostion() );
				pTargetParent->RelayPacket( kPacket );

				if( pTargetParent->IsUserOriginal() )
				{
					User *pUser = (User*)pTargetParent;
					ioUserGuild *pUserGuild = pUser->GetUserGuild();
					if( pUserGuild )
						pUserGuild->SetGuildPosition( ioHashString( pLocal->GetGuildMasterPostion() ) );
				}
				else
				{
					UserCopyNode *pUser = (UserCopyNode*)pTargetParent;
					SP2Packet kServerPacket( SSTPK_GUILD_POSITION_CHANGE );
					kServerPacket << pUser->GetUserIndex() << ioHashString( pLocal->GetGuildMasterPostion() );
					pUser->SendMessage( kServerPacket );
				}
			}

			//이벤트 초기화
			g_DBClient.OnUpdateGuildMemberEvent( pTargetParent->GetUserDBAgentID(), pTargetParent->GetAgentThreadID(), dwTargetIndex, dwGuildIndex );
		}
	}	
}

void UserNodeManager::OnResultUpdateGuildPositionChange(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateGuildPositionChange Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data
	DWORD dwUserIndex, dwTargetIndex, dwGuildIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTargetIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	char szTargetID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szTargetID, ID_NUM_PLUS_ONE );	

	char  szGuildPos[GUILD_POS_NUM_PLUS_ONE] = "";
	query_data->GetValue( szGuildPos, GUILD_POS_NUM_PLUS_ONE );	

	BOOL bNotify	= FALSE;
	ioHashString szMasterID = "";

	{
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			szMasterID = pUserParent->GetPublicID();

			SP2Packet kPacket( STPK_GUILD_POSITION_CHANGE );
			PACKET_GUARD_VOID( kPacket.Write(GUILD_POSITION_CHANGE_OK) );
			PACKET_GUARD_VOID( kPacket.Write(szMasterID) );
			PACKET_GUARD_VOID( kPacket.Write(dwTargetIndex) );
			PACKET_GUARD_VOID( kPacket.Write(szTargetID) );
			PACKET_GUARD_VOID( kPacket.Write(szGuildPos) );

			//kPacket << GUILD_POSITION_CHANGE_OK << dwTargetIndex << szTargetID << szGuildPos;
			pUserParent->RelayPacket( kPacket );

			if( pUserParent->IsUserOriginal() )
			{
				User *pUser = (User*)pUserParent;

				ioUserGuild* pGuild	= pUser->GetUserGuild();
				if( pGuild )
				{
					pGuild->SendRelayPacketTcp(kPacket);
					bNotify = TRUE;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	{
		UserParent *pTargetParent = GetGlobalUserNode( dwTargetIndex );
		if( pTargetParent )
		{
			//kPacket << GUILD_POSITION_CHANGE_OK << dwTargetIndex << szTargetID << szGuildPos;
			//pTargetParent->RelayPacket( kPacket );
			SP2Packet kPacket( STPK_GUILD_POSITION_CHANGE );
			PACKET_GUARD_VOID( kPacket.Write(GUILD_POSITION_CHANGE_OK) );
			PACKET_GUARD_VOID( kPacket.Write(szMasterID) );
			PACKET_GUARD_VOID( kPacket.Write(dwTargetIndex) );
			PACKET_GUARD_VOID( kPacket.Write(szTargetID) );
			PACKET_GUARD_VOID( kPacket.Write(szGuildPos) );

			if( pTargetParent->IsUserOriginal() )
			{
				User *pUser = (User*)pTargetParent;
				ioUserGuild *pUserGuild = pUser->GetUserGuild();
				if( pUserGuild )
				{
					pUserGuild->SetGuildPosition( szGuildPos );
					if( !bNotify )
					{
						SP2Packet kPacket( STPK_GUILD_POSITION_CHANGE );
						PACKET_GUARD_VOID( kPacket.Write(GUILD_POSITION_CHANGE_OK) );
						PACKET_GUARD_VOID( kPacket.Write(szMasterID) );
						PACKET_GUARD_VOID( kPacket.Write(dwTargetIndex) );
						PACKET_GUARD_VOID( kPacket.Write(szTargetID) );
						PACKET_GUARD_VOID( kPacket.Write(szGuildPos) );

						pUserGuild->SendRelayPacketTcp(kPacket);
					}
				}
			}
			else
			{
				if( !bNotify )
					pTargetParent->RelayPacket( kPacket );

				UserCopyNode *pUser = (UserCopyNode*)pTargetParent;
				SP2Packet kServerPacket( SSTPK_GUILD_POSITION_CHANGE );
				kServerPacket << pUser->GetUserIndex() << szGuildPos;
				pUser->SendMessage( kServerPacket );
			}

			//이벤트 초기화
			g_DBClient.OnUpdateGuildMemberEvent( pTargetParent->GetUserDBAgentID(), pTargetParent->GetAgentThreadID(), dwTargetIndex, dwGuildIndex );
		}
	}	
}

void UserNodeManager::OnResultSelectGuildSimpleData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSeleteGuildSimpleData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

    DWORD dwUserIndex = 0, dwGuildIndex = 0;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	char szGuildUserID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szGuildUserID, ID_NUM_PLUS_ONE );	
	if(query_data->GetResultCount() > 0)
	{
		query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	}

	// 메인 서버에 길드 정보 요청
	if( dwGuildIndex != 0 )
	{
		SP2Packet kPacket( MSTPK_GUILD_SIMPLE_INFO );
		kPacket << dwUserIndex << szGuildUserID << dwGuildIndex;
		g_MainServer.SendMessage( kPacket );
	}
	else
	{
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_GUILD_SIMPLE_DATA );
			kPacket << szGuildUserID << 0 << "" << 0;               //길드 없음도 알려줘야함.
			pUserParent->RelayPacket( kPacket );
		}
	}
}

void UserNodeManager::OnResultSelectGuildMarkChangeKeyValue(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildMarkChangeKeyValue Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex, dwKeyIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwKeyIndex, sizeof(DWORD) );

	User *pUser = GetUserNode( dwUserIndex );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildMarkChangeKeyValue USER FIND NOT! :%d", dwUserIndex );
		g_DBClient.OnUpdateGuildMarkChangeKeyValue( 0, 0, dwKeyIndex );
		return;
	}
	
	pUser->SetGuildMarkChangeKeyValue( dwKeyIndex );
	
	// 유저에게 키값 전송
	SP2Packet kPacket( STPK_GUILD_MARK_KEY_VALUE );
	kPacket << dwKeyIndex;
	pUser->SendMessage( kPacket );
}

void UserNodeManager::OnResultSelectGuildUserLadderPointADD(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildUserLadderPointADD Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"OnResultSelectGuildUserLadderPointADD 사용하지 않는 쿼리 실행됨" );
/*	DWORD dwUserIndex, dwGuildIndex;
	bool bPlusLadderPoint;
	int iLadderPoint;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwGuildIndex, sizeof(DWORD) );
	query_data->GetValue( bPlusLadderPoint, sizeof(bool) );
	query_data->GetValue( iLadderPoint, sizeof(int) );	

	SP2Packet kPacket( MSTPK_ADD_LADDER_POINT );
	kPacket << dwGuildIndex;
	if( bPlusLadderPoint )
		kPacket << iLadderPoint;
	else
		kPacket << -(iLadderPoint);
	g_MainServer.SendMessage( kPacket );
*/
}

void UserNodeManager::OnResultSelectCampSeasonBonus(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCampSeasonBonus Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0, dwBonusIndex = 0;
	int iBlueCampPoint = 0, iBlueCampBonusPoint = 0, iBlueEntry = 0, iRedCampPoint = 0, iRedCampBonusPoint = 0, iRedEntry = 0, iMyCampType = 0, iMyCampPoint = 0, iMyCampRank = 0;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );

	if( query_data->GetResultCount() > 0 )
	{
		query_data->GetValue( dwBonusIndex, sizeof(DWORD) );	
		query_data->GetValue( iBlueCampPoint, sizeof(int) );
		query_data->GetValue( iBlueCampBonusPoint, sizeof(int) );
		query_data->GetValue( iBlueEntry, sizeof(int) );
		query_data->GetValue( iRedCampPoint, sizeof(int) );
		query_data->GetValue( iRedCampBonusPoint, sizeof(int) );
		query_data->GetValue( iRedEntry, sizeof(int) );
		query_data->GetValue( iMyCampType, sizeof(int) );
		query_data->GetValue( iMyCampPoint, sizeof(int) );
		query_data->GetValue( iMyCampRank, sizeof(int) );
	}

	if( dwBonusIndex == 0 ) return;         //받은 보상이 없다

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent )
	{
		if( pUserParent->IsUserOriginal() )
		{
			User *pUser = (User*)pUserParent;
			pUser->AddCampSeasonBonus( dwBonusIndex, iBlueCampPoint, iBlueCampBonusPoint, iBlueEntry, 
									   iRedCampPoint, iRedCampBonusPoint, iRedEntry, iMyCampType, iMyCampPoint, iMyCampRank );
		}
		else
		{
			UserCopyNode *pUser = (UserCopyNode*)pUserParent;
			SP2Packet kPacket( SSTPK_CAMP_SEASON_BONUS );
			kPacket << pUser->GetUserIndex() << dwBonusIndex << iBlueCampPoint << iBlueCampBonusPoint << iBlueEntry 
				    << iRedCampPoint << iRedCampBonusPoint << iRedEntry << iMyCampType << iMyCampPoint << iMyCampRank;
			pUser->SendMessage( kPacket );
		}
	}
}

void UserNodeManager::OnResultSelectUserEntry(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserEntry Result FAILED! :%d",query_data->GetResultType());
		return;
	}

// 	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
// 	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	char szPublicID[ID_NUM_PLUS_ONE] = "";
	DWORD dwUserIdx = 0;

	query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE );       //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx, sizeof(int) );

	User *pUser = GetUserNode(dwUserIdx);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserEntry USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	
	if( pUser->GetPublicID() != szPublicID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserEntry USER WRONG ID! :%s:%d", szPublicID ,dwUserIdx);
		return;
	}

	short iEntryType=0;
	query_data->GetValue( iEntryType, sizeof(short) ); // db smallint

	if( iEntryType != ET_FORMALITY && 
		iEntryType != ET_FORMALITY_CASH ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserEntry Wrong Entry Type :%d:%d", dwUserIdx, iEntryType);
		return;
	}

	pUser->SetEntryType( (EntryType) iEntryType );
	SP2Packet kPacket( STPK_USER_ENTRY_REFRESH );
	kPacket << (int)iEntryType;
	pUser->SendMessage( kPacket );
}

void UserNodeManager::OnResultSelectUserExist( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserExist Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex   = 0; 
	DWORD dwFindUserCnt = 0;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );        //보낸 유저 인덱스
	query_data->GetValue( dwFindUserCnt, sizeof(DWORD) );

	int iReturnType = USER_INFO_EXIST_OFFLINE;
	if( dwFindUserCnt == 0 )
	{
		iReturnType = USER_INFO_EXIST_NO;
	}
	
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent )
	{
		SP2Packet kPacket( STPK_USER_INFO_EXIST );
		kPacket << iReturnType;               
		pUserParent->RelayPacket( kPacket );
	}
}

void UserNodeManager::OnResultSelectPublicIDExist( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectPublicIDExist Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ); // 보낸 정보. 

	DWORD dwUserIdx = 0;
	char szNewPublicID[ID_NUM_PLUS_ONE]="";

	query_data->GetValue( dwUserIdx, sizeof(int) );
	query_data->GetValue( szNewPublicID, ID_NUM_PLUS_ONE );
	
	DWORD dwExistCnt = 0;
	query_data->GetValue( dwExistCnt, sizeof(DWORD) );          // 받은 정보.

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserNodeManager::OnResultSelectPublicIDExist User not found. :%d", dwUserIdx );
		return;
	}

	if( dwExistCnt > 0 ) // 이미 아이디가 존재한다.
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		kReturn << ETCITEM_USE_EXIST_ID;
		pUser->SendMessage( kReturn );
		pUser->SetNewPublicID( "" );
		return;
	}

	pUser->SetNewPublicID( szNewPublicID );

	// 메인 서버에 길드명 변경 알림
	SP2Packet kMainPacket( MSTPK_NICKNAME_CHANGE );
	kMainPacket << dwUserIdx << szNewPublicID;
	g_MainServer.SendMessage( kMainPacket );	

	SP2Packet kSuccess( STPK_ETCITEM_USE );
	kSuccess << ETCITEM_USE_OK;
	kSuccess << (int) ioEtcItem::EIT_ETC_CHANGE_ID;
	kSuccess << szNewPublicID;
	pUser->SendMessage( kSuccess );

	//kyg 릴레이서버에 전송안함
#if 0
	ServerNode* node = NULL;
	node = g_Relay.GetRelayServer(pUser->RelayServerID());
	if(node)
	{
		SP2Packet pk (RSTPK_ON_CONTROL);
		int ctype = RS_CHANGE_NICKNAME;
		pk << ctype;
		pk << pUser->GetUserIndex();
		pk << szNewPublicID;
		node->SendMessage(pk);
	}
#endif
}

void UserNodeManager::OnResultSelectChangedPublicID( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB %s Result FAILED! :%d", __FUNCTION__, query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	char  szNewPublicID[ID_NUM_PLUS_ONE]="";
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );         // 보낸 정보. 
	query_data->GetValue( szNewPublicID, ID_NUM_PLUS_ONE );

	char  szPublicID[ID_NUM_PLUS_ONE]="";
	query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE );        // 받은 정보.

	if( strcmp( szPublicID, szNewPublicID ) != 0 || strcmp( szPublicID, "") == 0 )
	{
		g_NewPublicIDRefresher.DeleteInfo( dwUserIndex );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail NewPublicID :%d:%s:%s", __FUNCTION__, dwUserIndex, szPublicID, szNewPublicID );
		return;
	}

	g_NewPublicIDRefresher.SendFriendsAndDelete( dwUserIndex );	
}

void UserNodeManager::OnResultSelectBlockType( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBlockType Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	DWORD dwUserIndex = 0;
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );

	User *pUser = GetUserNode(dwUserIndex);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBlockType USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	short nBlockType = BKT_NONE;
	query_data->GetValue( nBlockType, sizeof( short ) ); // db smallint

	DBTIMESTAMP dts;
	query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) );
	CTime kBlockTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
	pUser->SetBlockTime( kBlockTime );

	// 차단날짜종료확인
	if( nBlockType != BKT_NORMAL )
	{
		CTime current_time = CTime::GetCurrentTime();
		CTimeSpan gaptime;
		gaptime = kBlockTime - current_time;
		if( gaptime.GetTotalMinutes() <= 0 )
			nBlockType = BKT_NORMAL;
	}

	if( nBlockType == BKT_BLOCK ) // 차단된 유저는 로그를 남긴다.
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%d Block(%d)%s %d-%d-%d %d:%d", nBlockType , dwUserIndex, pUser->GetPublicID().c_str(), kBlockTime.GetYear(), kBlockTime.GetMonth(), kBlockTime.GetDay(), kBlockTime.GetHour(), kBlockTime.GetMinute() );
	else
	{
#ifdef _DEBUG
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%d Block(%d)%s %d-%d-%d %d:%d", nBlockType , dwUserIndex, pUser->GetPublicID().c_str(), kBlockTime.GetYear(), kBlockTime.GetMonth(), kBlockTime.GetDay(), kBlockTime.GetHour(), kBlockTime.GetMinute() );
#endif
	}

	
	pUser->SetBlockType( (BlockType) nBlockType );

	SP2Packet kPacket( STPK_BLOCK_TYPE );
	kPacket << (int) pUser->GetBlockType();
	pUser->SendMessage( kPacket );
}

void UserNodeManager::OnResultInsertTrial(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertTrial Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectMemberCount( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectMemberCount Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	//보낸 유저의 아이디.
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szPrivateID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	query_data->GetValue(szPrivateID,ID_NUM_PLUS_ONE);       
	User *pUser = GetUserNodeByGUID( szUserGUID );
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectMemberCount USER FIND NOT! :%s : %s",szPrivateID, szUserGUID);
		return;
	}

	if( pUser->GetPrivateID() != szPrivateID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectMemberCount USER WRONG ID! :%s:%s", szPrivateID ,szUserGUID);
		return;
	}

	//SELECT
	DWORD dwExistCnt = 0;
	query_data->GetValue( dwExistCnt, sizeof(DWORD) );          // 받은 정보.

	if( dwExistCnt > 0 )
		g_DBClient.OnSelectUserLoginInfo( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPrivateID());
	else
	{
		//현재는 첫 DB 데이터 insert 이전에 OGP API 서버와 통신이 필요 없다.
		//ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		//if( pLocal )
		//	pLocal->SendUserInfo( pUser );

		// 임시 publid id는 private id + #
		char szTempPublicID[ID_NUM_PLUS_ONE]="";
		StringCbPrintf( szTempPublicID, sizeof( szTempPublicID ), "%s#", pUser->GetPrivateID().c_str() );
		g_DBClient.OnInsertMember( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetPrivateID(), ioHashString( szTempPublicID ) );
		g_DBClient.OnSelectUserLoginInfo(  pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPrivateID());
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s Insert Member : %s", __FUNCTION__, pUser->GetPrivateID().c_str() );
	}
}

void UserNodeManager::OnResultInsertMember(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertMember Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectFirstPublicIDExist( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectFirstPublicIDExist Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	char szPublicID[ID_NUM_PLUS_ONE]="";
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szNewPublicID[ID_NUM_PLUS_ONE]="";
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );         // 보낸 정보. 
	query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE );
	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ); 
	query_data->GetValue( szNewPublicID, ID_NUM_PLUS_ONE );

	DWORD dwExistCnt = 0;
	query_data->GetValue( dwExistCnt, sizeof(DWORD) );          // 받은 정보.

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserNodeManager::OnResultSelectFirstPublicIDExist User not found. :%s(%d:%s)", szUserGUID , dwUserIndex, szPublicID );
		return;
	}

	if( dwExistCnt > 0 ) // 이미 아이디가 존재한다.
	{
		SP2Packet kReturn( STPK_FIRST_CHANGE_ID );
		kReturn << FIRST_CHANGE_ID_EXIST_ID;
		pUser->SetFirstChangeID( false );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Exist ID %s:(%d:%s:%s)", __FUNCTION__, szNewPublicID, dwUserIndex, szPublicID, szUserGUID );
		return;
	}

	char szTempPublicID[ID_NUM_PLUS_ONE]="";
	StringCbPrintf( szTempPublicID , sizeof( szTempPublicID ), "%s#", pUser->GetPrivateID().c_str() );
	if( pUser->GetPublicID() != szTempPublicID )
	{
		SP2Packet kReturn( STPK_FIRST_CHANGE_ID );
		kReturn << FIRST_CHANGE_ID_EXCEPTION;
		pUser->SetFirstChangeID( false );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error [%d:%s:%s] not temp publid id",  __FUNCTION__, dwUserIndex, szPublicID, szUserGUID );
		return;
	}

	if( pUser->GetUserState() != 0 )
	{
		SP2Packet kReturn( STPK_FIRST_CHANGE_ID );
		kReturn << FIRST_CHANGE_ID_EXCEPTION;
		pUser->SetFirstChangeID( false );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error [%d:%s:%s] wrong user state :%d",  __FUNCTION__, dwUserIndex, szPublicID, szUserGUID, pUser->GetUserState() );
		return;
	}

	g_DBClient.OnUpdateFirstPublicID( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dwUserIndex, ioHashString( szNewPublicID ) );
	g_DBClient.OnSelectChangedFirstPublicID( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dwUserIndex, pUser->GetPublicID(), pUser->GetGUID(), ioHashString( szNewPublicID ) );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s First Public ID Success - 2 [%d:%s]", __FUNCTION__ , dwUserIndex, szPublicID );
}

void UserNodeManager::OnResultSelectChangedFirstPublicID( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnRetultSelectChangedFirstPublicID Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	char szPublicID[ID_NUM_PLUS_ONE]="";
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szNewPublicID[ID_NUM_PLUS_ONE]="";
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );         // 보낸 정보. 
	query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE );
	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ); 
	query_data->GetValue( szNewPublicID, ID_NUM_PLUS_ONE );

	char  szReturnPublicID[ID_NUM_PLUS_ONE]="";
	query_data->GetValue( szReturnPublicID, ID_NUM_PLUS_ONE );        // 받은 정보.

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserNodeManager::OnResultSelectFirstPublicIDExist User not found. :%s(%d:%s)", szUserGUID, dwUserIndex, szPublicID );
		return;
	}

	if( strcmp( szReturnPublicID, szNewPublicID ) != 0 || strcmp( szReturnPublicID, "") == 0 )
	{
		SP2Packet kReturn( STPK_FIRST_CHANGE_ID );
		kReturn << FIRST_CHANGE_ID_EXCEPTION;
		pUser->SetFirstChangeID( false );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail update query :(%d:%s:%s) %s", __FUNCTION__, dwUserIndex, szPublicID, szUserGUID, szReturnPublicID );
		return;
	}

	char szTempPublicID[ID_NUM_PLUS_ONE]="";
	StringCbPrintf( szTempPublicID , sizeof( szTempPublicID ), "%s#", pUser->GetPrivateID().c_str() );
	if( pUser->GetPublicID() != szTempPublicID )
	{
		SP2Packet kReturn( STPK_FIRST_CHANGE_ID );
		kReturn << FIRST_CHANGE_ID_EXCEPTION;
		pUser->SetFirstChangeID( false );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error [%d:%s:%s] not temp public id",  __FUNCTION__, dwUserIndex, szPublicID, szUserGUID );
		return;
	}

	if( pUser->GetUserState() != 0 )
	{
		SP2Packet kReturn( STPK_FIRST_CHANGE_ID );
		kReturn << FIRST_CHANGE_ID_EXCEPTION;
		pUser->SetFirstChangeID( false );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error [%d:%s:%s] wrong user state :%d",  __FUNCTION__, dwUserIndex, szPublicID, szUserGUID, pUser->GetUserState() );
		return;
	}

	pUser->SetFirstChangeID( false );
	pUser->SetPublicID( ioHashString( szNewPublicID ) );

	SP2Packet kSuccess( STPK_FIRST_CHANGE_ID );
	kSuccess << FIRST_CHANGE_ID_SUCCESS;
	kSuccess << szNewPublicID;
	pUser->SendMessage( kSuccess );
	pUser->SyncUserPublicID();
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s First Public ID Success - complete [%d:%s->%s]", __FUNCTION__ , dwUserIndex, szPublicID, szNewPublicID );
}

void UserNodeManager::OnResultInsertMedalItemData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertMedalItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectMedalItemIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertMedalItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwInvenIdx = 0;
// 	char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
// 	char   szUserID[ID_NUM_PLUS_ONE] = "";
	int    iLogType   = 0;
	int    iLimitTime = 0;
	DWORD dwUserIdx = 0;
// 	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
// 	query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );        //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx, sizeof(int) );
	query_data->GetValue( iLogType, sizeof( int ) );    
	query_data->GetValue( iLimitTime, sizeof( int ) );      

	query_data->GetValue( dwInvenIdx, sizeof(int) );		 //새로운 인벤토리 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultInsertMedalItemData USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioUserMedalItem *pItem = pUser->GetUserMedalItem();
	if( !pItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultInsertMedalItemData : (%d - %d) pItem == NULL.", dwUserIdx, dwInvenIdx );
		return;
	}

	if( !pItem->DBtoNewIndex( dwInvenIdx ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultInsertMedalItemData : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwInvenIdx );
		return;
	}


	ioUserMedalItem::MEDALITEMSLOT kItemSlot[ ioUserMedalItem::MAX_SLOT ];
	if( !pItem->GetRowMedalItem( dwInvenIdx, kItemSlot ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultInsertMedalItemData : (%d - %d) Index not found.", dwUserIdx, dwInvenIdx );
		return;
	}

	for (int i = 0; i < ioUserMedalItem::MAX_SLOT ; i++)
	{
		if( kItemSlot[i].m_iItemType == 0 )
			continue;

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwInvenIdx, i+1 );
		g_LogDBClient.OnInsertMedalItem( pUser, kItemSlot[i].m_iItemType, kItemSlot[i].m_iPeriodType, szItemIndex, (LogDBClient::MedalType)iLogType );
	}

}

void UserNodeManager::OnResultUpdateMedalItemData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateMedalItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultInsertExMedalSlotData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertExMedalSlotData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectExMedalSlotIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectExMedalSlotIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwInvenIdx = 0;
// 	char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
// 	char   szUserID[ID_NUM_PLUS_ONE] = "";
	int    iLogType   = 0;
	DWORD dwUserIdx = 0;

// 	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
// 	query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );        //보낸 유저의 아이디. 
	query_data->GetValue( dwUserIdx, sizeof(int) );
	query_data->GetValue( iLogType, sizeof( int ) );    
	query_data->GetValue( dwInvenIdx, sizeof(int) );		 //새로운 인벤토리 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectExMedalSlotIndex USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioUserExpandMedalSlot *pExMedalSlot = pUser->GetUserExpandMedalSlot();
	if( !pExMedalSlot )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectExMedalSlotIndex : (%d - %d) pItem == NULL.", dwUserIdx, dwInvenIdx );
		return;
	}

	if( !pExMedalSlot->DBtoNewIndex( dwInvenIdx ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectExMedalSlotIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwInvenIdx );
		return;
	}


	ioUserExpandMedalSlot::ExpandMedalSlot kExMedalSlot[ ioUserExpandMedalSlot::MAX_SLOT ];
	if( !pExMedalSlot->GetRowExMedalSlot( dwInvenIdx, kExMedalSlot ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectExMedalSlotIndex : (%d - %d) Index not found.", dwUserIdx, dwInvenIdx );
		return;
	}

	for( int i=0; i<ioUserExpandMedalSlot::MAX_SLOT; ++i )
	{
		if( kExMedalSlot[i].m_iClassType == 0 )
		{
			continue;
		}

		g_LogDBClient.OnInsertExMedalSlot( pUser, kExMedalSlot[i].m_iClassType, kExMedalSlot[i].m_iSlotNumber, kExMedalSlot[i].m_dwLimitTime, (LogDBClient::ExMedalType)iLogType );
	}
}

void UserNodeManager::OnResultUpdateExMedalSlotData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateExMedalSlotData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectHeroData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectHeroData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char   szUserID[ID_NUM_PLUS_ONE] = "";
	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );    //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //캐릭터 주인 인덱스

	UserParent *pUserParent = GetGlobalUserNode( dwUserIdx );
	if( pUserParent == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectHeroData USER FIND NOT! :%s:%s", szUserID ,szUserGUID );
		return;
	}

	int i = 0;
	UserHeroData kUserHeroData;
	if( query_data->GetResultCount() > 0 )
	{
		PACKET_GUARD_VOID( query_data->GetValue( kUserHeroData.m_iHeroTitle, sizeof(int) ) );			//유저 칭호
		PACKET_GUARD_VOID( query_data->GetValue( kUserHeroData.m_iHeroTodayRank, sizeof(int) ) );		//유저 랭킹
		for(i = 0;i < HERO_SEASON_RANK_MAX;i++)
		{
			PACKET_GUARD_BREAK( query_data->GetValue( kUserHeroData.m_iHeroSeasonRank[i], sizeof(int) ) );//시즌 랭킹
		}
	}

	if( pUserParent->IsUserOriginal() )
	{
		User *pUser = (User*)pUserParent;

		pUser->SetUserHeroData( kUserHeroData );
	}
	else
	{
		UserCopyNode *pUser = (UserCopyNode*)pUserParent;

		SP2Packet kPacket( SSTPK_USER_HERO_DATA );
		 
		PACKET_GUARD_VOID( kPacket.Write( pUser->GetUserIndex()) );
		PACKET_GUARD_VOID( kPacket.Write( kUserHeroData.m_iHeroTitle) );
		PACKET_GUARD_VOID( kPacket.Write( kUserHeroData.m_iHeroTodayRank ) );

		for(i = 0;i < HERO_SEASON_RANK_MAX;i++)
		{
			PACKET_GUARD_VOID(kPacket.Write( kUserHeroData.m_iHeroSeasonRank[i]) );
		}

		pUser->SendMessage( kPacket );
	}
}

void UserNodeManager::OnResultSelectHeroTop100Data(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectHeroTop100Data Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	g_HeroRankManager.DBtoData( query_data );
}

void UserNodeManager::OnResultSelectItemCustomUniqueIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectItemCustomUniqueIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char   szUserID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );    //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx, sizeof(int) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode( dwUserIdx );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectItemCustomUniqueIndex USER FIND NOT! :%s:%s", szUserID ,szUserGUID );
		return;
	}
	DWORD dwUniqueIndex;
	query_data->GetValue( dwUniqueIndex, sizeof(int) );		

	SP2Packet kPacket( STPK_CUSTOM_ITEM_SKIN_UNIQUE_INDEX );
	kPacket << dwUniqueIndex;
	pUser->SendMessage( kPacket );
}

void UserNodeManager::LoadSpecialIP()
{

	m_sDeveloperIPSet.clear();
	m_sCloudIPSet.clear();

	ioINILoader kLoader( "config/sp2_IP.ini" );
	char szValue[ MAX_PATH ]="";
	char szKey[MAX_PATH]="";
	int iMaxCount = 50;
	//클라우드IP
	kLoader.SetTitle( "cloud" ); 

	for( int i=0; i<iMaxCount; i++ )
	{
		StringCbPrintf( szKey, sizeof( szKey ), "%d", i+1 );
		kLoader.LoadString( szKey, "", szValue, MAX_PATH );

		if( !szValue[0] )
			break;
		
		ioHashString szIP = szValue;
		m_sCloudIPSet.insert( szIP );
	}

	//개발자IP
	kLoader.SetTitle( "admin" );

	for( int i=0; i<iMaxCount; i++ )
	{
		StringCbPrintf( szKey, sizeof( szKey ), "%d", i+1 );
		kLoader.LoadString( szKey, "", szValue, MAX_PATH );

		if( !szValue[0] )
			break;
		
		ioHashString szIP = szValue;
		m_sDeveloperIPSet.insert( szIP );
	}
}

bool UserNodeManager::IsCloudIP( const char *szClientIP )
{
	std::set< ioHashString >::iterator iter = m_sCloudIPSet.find( szClientIP );

	if( iter != m_sCloudIPSet.end() )
		return true;
	
	return false;
}

bool UserNodeManager::IsAdminIP( const char *szClientIP )
{
	std::set< ioHashString >::iterator iter = m_sDeveloperIPSet.find( szClientIP );

	if( iter != m_sDeveloperIPSet.end() )
		return true;
	
	return false;
}

int UserNodeManager::GetUserAccountType( const char *szClientIP )
{
	if( IsCloudIP( szClientIP ) )
	{
		return IP_CLOUD;
	}
	else if( IsAdminIP( szClientIP ) )
	{
		return IP_ADMIN;
	}
	
	return IP_DEFAULT;
}

void UserNodeManager::LoadDeveloperID()
{
	m_vLikeDeveloperID.clear();
	m_vDeveloperID.clear();

	ioINILoader kLoader( "config/sp2_developerID.ini" );
	kLoader.SetTitle( "ID_LIKE" );
	int iMax = kLoader.LoadInt( "max", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "id_%d", i+1 );
		char szLikeDevelperID[MAX_PATH]="";
		kLoader.LoadString( szKeyName, "", szLikeDevelperID, MAX_PATH );
		if( strcmp( szLikeDevelperID, "" ) == 0 )
			continue;

		m_vLikeDeveloperID.push_back( ioHashString( szLikeDevelperID ) );
	}

	kLoader.SetTitle( "ID" );
	iMax = kLoader.LoadInt( "max", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "id_%d", i+1 );
		char szDevelperID[MAX_PATH]="";
		kLoader.LoadString( szKeyName, "", szDevelperID, MAX_PATH );
		if( strcmp( szDevelperID, "" ) == 0 )
			continue;

		m_vDeveloperID.push_back( ioHashString( szDevelperID ) );
	}
}

void UserNodeManager::OnResultSelectCreateTrade(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCreateTrade Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectCreateTradeIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCreateTradeIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwTradeIndex, dwRegisterUserIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;
	DWORD dwRegisterPeriod;
	ioHashString szRegisterUserNick, szRegisterIP;

	query_data->GetValue( dwRegisterUserIndex, sizeof(DWORD) );
	query_data->GetValue( szRegisterUserNick, ID_NUM_PLUS_ONE );

	query_data->GetValue( dwItemType, sizeof(DWORD) );
	query_data->GetValue( dwItemMagicCode, sizeof(DWORD) );
	query_data->GetValue( dwItemValue, sizeof(DWORD) );
	query_data->GetValue( dwItemMaleCustom, sizeof(DWORD) );
	query_data->GetValue( dwItemFemaleCustom, sizeof(DWORD) );
	query_data->GetValue( iItemPrice, sizeof(__int64) );
	query_data->GetValue( dwRegisterPeriod, sizeof(DWORD) );

	DBTIMESTAMP dts;
	query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) );		// 등록 날짜

	query_data->GetValue( szRegisterIP, IP_NUM_PLUS_ONE );

	// DB에서 받는 값
	query_data->GetValue( dwTradeIndex, sizeof(DWORD) );

	CTime kRegisterDate(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));

	DWORD dwDate1, dwDate2;
	dwDate1 = ( kRegisterDate.GetYear() * 10000 ) + ( kRegisterDate.GetMonth() * 100 ) + kRegisterDate.GetDay();
	dwDate2 = ( kRegisterDate.GetHour() * 10000 ) + ( kRegisterDate.GetMinute() * 100 ) + kRegisterDate.GetSecond();


	UserParent *pUserParent = GetGlobalUserNode( dwRegisterUserIndex );
	if( dwTradeIndex == 0 )
	{
		TradeLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "TradeCreate(%d:%s) - [IndexFail] : [Type:%d] [Code:%d] [Value:%d] [Custom:%d-%d] [Price:%I64d] [Period:%d] [IP:%s]",
									 dwRegisterUserIndex, szRegisterUserNick.c_str(),
                                     dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom, 
									 iItemPrice, dwRegisterPeriod,
									 szRegisterIP.c_str() );

		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUser = (User*)pUserParent;
				SP2Packet kPacket( STPK_TRADE_CREATE );
				kPacket << TRADE_CREATE_FAIL;
				pUser->SendMessage( kPacket );
			}	
			else
			{
				// 해당 서버로 정보 전송
				UserCopyNode *pUser = (UserCopyNode*)pUserParent;
				SP2Packet kPacket( SSTPK_TRADE_CREATE_FAIL );
				kPacket << pUser->GetUserIndex();
				pUser->SendMessage( kPacket );
			}
		}
		return;
	}

	TradeLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "TradeCreate(%d:%s) - [IndexSuccess] : [Index:%d] [Type:%d] [Code:%d] [Value:%d] [Custom:%d-%d] [Price:%I64d] [Period:%d] [IP:%s]",
								 dwRegisterUserIndex, szRegisterUserNick.c_str(),
								 dwTradeIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom, iItemPrice, dwRegisterPeriod,
								 szRegisterIP.c_str() );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "[%d-1 : %d,%d]", dwTradeIndex, dwItemMaleCustom, dwItemFemaleCustom );

	g_LogDBClient.OnInsertTrade( dwRegisterUserIndex, szRegisterUserNick,
								 dwTradeIndex,
								 dwItemType, dwItemMagicCode, dwItemValue, iItemPrice, LogDBClient::TST_REG,
								 szRegisterIP.c_str(), szNote );


	//메인 서버에 등록	
	SP2Packet kPacket( MSTPK_TRADE_CREATE_REG );
	kPacket << dwTradeIndex << dwRegisterUserIndex << szRegisterUserNick;
	kPacket << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom << iItemPrice;
	kPacket << dwRegisterPeriod	<< dwDate1 << dwDate2;
	g_MainServer.SendMessage( kPacket );


	// 클라이언트에 정보 전달
	if( pUserParent )
	{
		if( pUserParent->IsUserOriginal() )
		{
			User *pUser = (User*)pUserParent;

			SP2Packet kPacket( STPK_TRADE_CREATE );

// 			kPacket << TRADE_CREATE_OK;
// 			kPacket << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom << iItemPrice;

			PACKET_GUARD_VOID( kPacket.Write( TRADE_CREATE_OK ) );
			
			PACKET_GUARD_VOID( kPacket.Write( dwRegisterUserIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( dwTradeIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( dwItemType ) );
			PACKET_GUARD_VOID( kPacket.Write( dwItemMagicCode ) );
			PACKET_GUARD_VOID( kPacket.Write( dwItemValue ) );

			PACKET_GUARD_VOID( kPacket.Write( dwItemMaleCustom ) );
			PACKET_GUARD_VOID( kPacket.Write( dwItemFemaleCustom ) );
			PACKET_GUARD_VOID( kPacket.Write( iItemPrice ) );
			PACKET_GUARD_VOID( kPacket.Write( dwDate1 ) );
			PACKET_GUARD_VOID( kPacket.Write( dwDate2 ) );

			PACKET_GUARD_VOID( kPacket.Write( dwRegisterPeriod ) );
			PACKET_GUARD_VOID( kPacket.Write( szRegisterUserNick ) );


			pUser->SendMessage( kPacket );
		}	
		else
		{
			// 해당 서버로 정보 전송
			UserCopyNode *pUser = (UserCopyNode*)pUserParent;
			SP2Packet kPacket( SSTPK_TRADE_CREATE_COMPLETE );

// 			kPacket << pUser->GetUserIndex();
// 			kPacket << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom << iItemPrice;
// 			kPacket << dwRegisterPeriod << dwDate1 << dwDate2;

			PACKET_GUARD_VOID( kPacket.Write( dwRegisterUserIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( dwTradeIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( dwItemType ) );
			PACKET_GUARD_VOID( kPacket.Write( dwItemMagicCode ) );
			PACKET_GUARD_VOID( kPacket.Write( dwItemValue ) );

			PACKET_GUARD_VOID( kPacket.Write( dwItemMaleCustom ) );
			PACKET_GUARD_VOID( kPacket.Write( dwItemFemaleCustom ) );
			PACKET_GUARD_VOID( kPacket.Write( iItemPrice ) );
			PACKET_GUARD_VOID( kPacket.Write( dwDate1 ) );
			PACKET_GUARD_VOID( kPacket.Write( dwDate2 ) );

			PACKET_GUARD_VOID( kPacket.Write( dwRegisterPeriod ) );
			PACKET_GUARD_VOID( kPacket.Write( szRegisterUserNick ) );
			pUser->SendMessage( kPacket );
		}
	}
}

void UserNodeManager::OnResultTradeItemComplete(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultTradeItemComplete Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwBuyUserIndex, dwRegisterUserIndex;
	DWORD dwTradeIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;
	ioHashString szRegisterUserNick;

	query_data->GetValue( dwBuyUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTradeIndex, sizeof(DWORD) );
	query_data->GetValue( dwRegisterUserIndex, sizeof(DWORD) );
	query_data->GetValue( szRegisterUserNick, ID_NUM_PLUS_ONE );

	query_data->GetValue( dwItemType, sizeof(DWORD) );
	query_data->GetValue( dwItemMagicCode, sizeof(DWORD) );
	query_data->GetValue( dwItemValue, sizeof(DWORD) );
	query_data->GetValue( dwItemMaleCustom, sizeof(DWORD) );
	query_data->GetValue( dwItemFemaleCustom, sizeof(DWORD) );
	query_data->GetValue( iItemPrice, sizeof(__int64) );

	TradeLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "TradeItem - [Complete] [Index:%d] [RegUser:%d] [Type:%d] [Code:%d] [Value:%d] [Custom:%d-%d] [Price:%I64d] [BuyUser:%d]",
								 dwTradeIndex, dwRegisterUserIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom, iItemPrice,
								 dwBuyUserIndex );

	UserParent *pBuyUserParent = GetGlobalUserNode( dwBuyUserIndex );
	if( pBuyUserParent )
	{
		if( pBuyUserParent->IsUserOriginal() )
		{
			User *pBuyUser = (User*)pBuyUserParent;

			// 구매자 처리
			g_PresentHelper.SendPresentByTradeItemBuy( pBuyUser->GetUserDBAgentID(),
													   pBuyUser->GetAgentThreadID(),
													   dwRegisterUserIndex,
													   dwBuyUserIndex,
													   dwItemType,
													   dwItemMagicCode,
													   dwItemValue,
													   dwItemMaleCustom,
													   dwItemFemaleCustom,
													   szRegisterUserNick );

			// DB에서 선물 정보 가져오게하고, 정보 전달.
			pBuyUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

			// 메인서버에 삭제 요청
			SP2Packet kPacket( MSTPK_TRADE_ITEM_TRADE );
			PACKET_GUARD_VOID( kPacket.Write( TRADE_ITEM_DEL ) );
			PACKET_GUARD_VOID( kPacket.Write( dwTradeIndex ) );
		
			g_MainServer.SendMessage( kPacket );

			// 지불한 페소
			__int64 iResultPeso = 0;
			if( pBuyUser->IsPCRoomAuthority() )
				iResultPeso	= iItemPrice + (iItemPrice * g_TradeInfoMgr.GetPCRoomBuyTexRate());
			else
				iResultPeso	= iItemPrice + (iItemPrice * g_TradeInfoMgr.GetBuyTexRate());

			SP2Packet kSuccess( STPK_TRADE_ITEM_COMPLETE );

// 			kSuccess << TRADE_BUY_OK;
// 			kSuccess << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
// 			kSuccess << iResultPeso;
// 			kSuccess << pBuyUser->GetMoney();

			PACKET_GUARD_VOID( kSuccess.Write( TRADE_BUY_OK ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwTradeIndex ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemType ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemMagicCode ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemValue ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemMaleCustom ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemFemaleCustom ) );
			PACKET_GUARD_VOID( kSuccess.Write( iResultPeso ) );
			PACKET_GUARD_VOID( kSuccess.Write( pBuyUser->GetMoney() ) );

			pBuyUser->SendMessage( kSuccess );
		}
		else
		{
			UserCopyNode *pUser = (UserCopyNode*)pBuyUserParent;
			SP2Packet kPacket( SSTPK_TRADE_ITEM_COMPLETE );
			kPacket << pUser->GetUserIndex();
			kPacket << TRADE_S_BUY_COMPLETE;
			kPacket << dwBuyUserIndex << dwTradeIndex;
			kPacket << dwRegisterUserIndex << szRegisterUserNick;
			kPacket << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
			kPacket << iItemPrice;
			pUser->SendMessage( kPacket );
		}
	}
	else
	{
		g_PresentHelper.SendPresentByTradeItemBuy( 0, 0,
												   dwRegisterUserIndex,
												   dwBuyUserIndex,
												   dwItemType,
												   dwItemMagicCode,
												   dwItemValue,
												   dwItemMaleCustom,
												   dwItemFemaleCustom,
												   szRegisterUserNick );
	}

	UserParent *pRegisterUserParent = GetGlobalUserNode( dwRegisterUserIndex );
	if( pRegisterUserParent )
	{
		if( pRegisterUserParent->IsUserOriginal() )
		{
			User *pSellUser = (User*)pRegisterUserParent;

			// 실제지급할 페소 계산
			int iTradePrice = iItemPrice;
			g_PresentHelper.SendPresentByTradeItemSell( pSellUser->GetUserDBAgentID(),
														pSellUser->GetAgentThreadID(),
														dwBuyUserIndex,
														dwRegisterUserIndex,
														iTradePrice,
														szRegisterUserNick );

			// DB에서 선물 정보 가져오게하고, 클라이언트에 정보 전달.
			pSellUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

			SP2Packet kSuccess( STPK_TRADE_ITEM_COMPLETE );
// 			kSuccess << TRADE_SELL_OK;
// 			kSuccess << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
// 			kSuccess << iTradePrice;

			PACKET_GUARD_VOID( kSuccess.Write( TRADE_SELL_OK ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwTradeIndex ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemType ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemMagicCode ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemValue ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemMaleCustom ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemFemaleCustom ) );
			PACKET_GUARD_VOID( kSuccess.Write( iTradePrice ) );

			pSellUser->SendMessage( kSuccess );
		}
		else
		{
			UserCopyNode *pUser = (UserCopyNode*)pRegisterUserParent;
			SP2Packet kPacket( SSTPK_TRADE_ITEM_COMPLETE );
			kPacket << pUser->GetUserIndex();
			kPacket << TRADE_S_SELL_COMPLETE;
			kPacket << dwBuyUserIndex << dwTradeIndex;
			kPacket << dwRegisterUserIndex << szRegisterUserNick;
			kPacket << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
			kPacket << iItemPrice;
			pUser->SendMessage( kPacket );
		}
	}
	else
	{
		// 실제지급할 페소 계산
		int iTradePrice = iItemPrice;
		g_PresentHelper.SendPresentByTradeItemSell( 0, 0, dwBuyUserIndex, dwRegisterUserIndex, iTradePrice, szRegisterUserNick );
	}
}

void UserNodeManager::OnResultTradeItemCancel(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultTradeItemCancel Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwRegisterUserIndex;
	DWORD dwTradeIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;
	ioHashString szRegisterUserNick;

	query_data->GetValue( dwTradeIndex, sizeof(DWORD) );
	query_data->GetValue( dwRegisterUserIndex, sizeof(DWORD) );
	query_data->GetValue( szRegisterUserNick, ID_NUM_PLUS_ONE );

	query_data->GetValue( dwItemType, sizeof(DWORD) );
	query_data->GetValue( dwItemMagicCode, sizeof(DWORD) );
	query_data->GetValue( dwItemValue, sizeof(DWORD) );
	query_data->GetValue( dwItemMaleCustom, sizeof(DWORD) );
	query_data->GetValue( dwItemFemaleCustom, sizeof(DWORD) );
	query_data->GetValue( iItemPrice, sizeof(__int64) );

	TradeLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "TradeCancel - [Complete] [Index:%d] [RegUser:%d] [Type:%d] [Code:%d] [Value:%d] [Custom:%d-%d] [Price:%I64d]",
								 dwTradeIndex, dwRegisterUserIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom, iItemPrice );

	UserParent *pRegisterUserParent = GetGlobalUserNode( dwRegisterUserIndex );
	if( pRegisterUserParent )
	{
		if( pRegisterUserParent->IsUserOriginal() )
		{
			User *pUser = (User*)pRegisterUserParent;

			g_PresentHelper.SendPresentByTradeCancel( pUser->GetUserDBAgentID(),
													  pUser->GetAgentThreadID(),
													  dwRegisterUserIndex,
													  dwRegisterUserIndex,
													  dwItemType,
													  dwItemMagicCode,
													  dwItemValue,
													  dwItemMaleCustom,
													  dwItemFemaleCustom,
													  szRegisterUserNick );

			// DB에서 선물 정보 가져오게하고, 클라이언트에 정보 전달.
			pUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

			// 메인서버에 삭제 요청
			SP2Packet kPacket( MSTPK_TRADE_ITEM_CANCEL );
			kPacket << TRADE_ITEM_CANCEL_DEL;
			kPacket << dwTradeIndex;
			g_MainServer.SendMessage( kPacket );

			SP2Packet kSuccess( STPK_TRADE_CANCEL );

// 			kSuccess << TRADE_CANCEL_COMPLETE;
// 			kSuccess << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
// 			kSuccess << iItemPrice;

			PACKET_GUARD_VOID( kSuccess.Write( TRADE_CANCEL_COMPLETE ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwTradeIndex ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemType ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemMagicCode ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemValue ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemMaleCustom ) );
			PACKET_GUARD_VOID( kSuccess.Write( dwItemFemaleCustom ) );
			PACKET_GUARD_VOID( kSuccess.Write( iItemPrice ) );

			pUser->SendMessage( kSuccess );
		}
		else
		{
			UserCopyNode *pUser = (UserCopyNode*)pRegisterUserParent;
			SP2Packet kPacket( SSTPK_TRADE_CANCEL );
			kPacket << pUser->GetUserIndex();
			kPacket << TRADE_CANCEL_S_CANCEL_COMPLETE;
			kPacket << dwTradeIndex;
			kPacket << dwRegisterUserIndex << szRegisterUserNick;
			kPacket << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
			kPacket << iItemPrice;
			pUser->SendMessage( kPacket );
		}
	}
	else
	{
		g_PresentHelper.SendPresentByTradeCancel( 0, 0,
												  dwRegisterUserIndex,
												  dwRegisterUserIndex,
												  dwItemType,
												  dwItemMagicCode,
												  dwItemValue,
												  dwItemMaleCustom,
												  dwItemFemaleCustom,
												  szRegisterUserNick );
	}
}

void UserNodeManager::OnResultDBServerTestLastQuery( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultDBServerTestLastQuery Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwAgentServerID, dwCurrentTime;
	query_data->GetValue( dwAgentServerID, sizeof( DWORD ) );
	query_data->GetValue( dwCurrentTime, sizeof( DWORD ) );

	DWORD dwFindUserCnt = 0;
	query_data->GetValue( dwFindUserCnt, sizeof(DWORD) );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Test Complete : %d - %d - %d", dwAgentServerID, TIMEGETTIME() - dwCurrentTime, dwFindUserCnt );
}

void UserNodeManager::OnResultSelectHeadquartersDataCount(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectHeadquartersDataCount Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	//char szUserID[ID_NUM_PLUS_ONE] = "";
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(LONG) ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectHeadquartersDataCount USER FIND NOT! :%d", dwUserIndex );
		return;
	}
	/*
	if( pUser->GetPublicID() != szUserID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectHeadquartersDataCount USER FIND NOT! :%s : %s", szUserID, szUserGUID );
		return;
	}
	*/
	int iCount;
	PACKET_GUARD_VOID( query_data->GetValue( iCount, sizeof(LONG) ) );

	if( iCount == 0 )
	{
		g_DBClient.OnInsertHeadquartersData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex() );
		
		// Insert해도 어차피 디폴트값이니 select 필요없음
		UserHeadquartersOption kOutOption;
		pUser->SetUserHeadquartersData( kOutOption );
	}
	else
	{
		// 테이블 있으면 insert
		g_DBClient.OnSelectHeadquartersData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
	}
}

void UserNodeManager::OnResultSelectHeadquartersData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectHeadquartersData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	//char szUserID[ID_NUM_PLUS_ONE] = "";
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(LONG) ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectHeadquartersData USER FIND NOT! :%d", dwUserIndex );
		return;
	}
	/*
	if( pUser->GetPublicID() != szUserID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectHeadquartersData USER FIND NOT! :%s : %s", szUserID, szUserGUID );
		return;
	}
	*/
	UserHeadquartersOption kOutOption;
	for(int i = 0;i < MAX_DISPLAY_CNT;i++)
	{
		PACKET_GUARD_BREAK( query_data->GetValue( kOutOption.m_dwCharacterIndex[i], sizeof(LONG) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( kOutOption.m_iCharacterXPos[i], sizeof(LONG) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( kOutOption.m_iCharacterZPos[i], sizeof(LONG) ) );
	}
	PACKET_GUARD_VOID( query_data->GetValue( kOutOption.m_sLock, sizeof(short) ) );

	pUser->SetUserHeadquartersData( kOutOption );
}

void UserNodeManager::OnResultSelectUserBirthDate( CQueryResultData* query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserBirthDate Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	char szPrivateID[ID_NUM_PLUS_ONE] = "";
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	PACKET_GUARD_VOID( query_data->GetValue( szPrivateID, ID_NUM_PLUS_ONE ) );

	User *pUser = GetUserNodeByGUID( szUserGUID );
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserBirthDate USER FIND NOT! :%s : %s", szPrivateID, szUserGUID );
		return;
	}

	if( pUser->GetPrivateID() != szPrivateID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserBirthDate USER FIND NOT! :%s : %s", szPrivateID, szUserGUID );
		return;
	}

	char szBirthDate[USER_BIRTH_DATE_PLUS_ONE]=""; // 주민번호 앞자리 
	BYTE byYearType = 0;                           // 주민번호 뒷자리 첫번째 숫자
	short sChannelingType	= 0;					   // 채널링 타입

	if( query_data->GetResultCount() > 0 )
	{
		PACKET_GUARD_VOID( query_data->GetValue( szBirthDate, USER_BIRTH_DATE_PLUS_ONE) );
		PACKET_GUARD_VOID( query_data->GetValue( byYearType, sizeof(BYTE) ) );
		PACKET_GUARD_VOID( query_data->GetValue( sChannelingType, sizeof(short) ) );
	}

	//Daum 유저중에 2015년 가입 유저 같은 경우는 생년월일을 저장 하지 않게됨. 별도 체크 필요
	if( 0 == byYearType && CNT_DAUM == sChannelingType )
	{
		//Daum 자체 ID값 Get
		g_DBClient.OnSelectUserChannelingKeyValue(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetPrivateID(), pUser->GetGUID(), sChannelingType);
		return;
	}

	bool bShutDownUser = g_ShutDownMgr.CheckShutDownUser( szBirthDate, byYearType, true );
	pUser->SetShutDownUser( bShutDownUser );

	bool bExitUser = false;
	if( g_ShutDownMgr.IsActive() && bShutDownUser )
		bExitUser = true;

	SP2Packet kPacket( STPK_SHUT_DOWN_USER );

	PACKET_GUARD_VOID( kPacket.Write(bShutDownUser) );
	PACKET_GUARD_VOID( kPacket.Write(bExitUser ) );

	pUser->SendMessage( kPacket );

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s %s:%s [%d]", __FUNCTION__, szUserGUID, szPrivateID, (int) bShutDownUser );

	if( !bExitUser )
	{
		g_DBClient.OnSelectUserData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPrivateID() );      
	}
}

void UserNodeManager::OnResultSelectUserSelectShutDown( CQueryResultData* query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectUserSelectShutDown Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	char szPrivateID[ID_NUM_PLUS_ONE] = "";
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	PACKET_GUARD_VOID( query_data->GetValue( szPrivateID, ID_NUM_PLUS_ONE ) );

	User *pUser = GetUserNodeByGUID( szUserGUID );
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserSelectShutDown USER FIND NOT! :%s : %s", szPrivateID, szUserGUID );
		return;
	}

	if( pUser->GetPrivateID() != szPrivateID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserSelectShutDown USER FIND NOT! :%s : %s", szPrivateID, szUserGUID );
		return;
	}

	BYTE byShutDownUser = 0;
	DBTIMESTAMP dts = { 0, 0, 0, 0, 0, 0, 0, };

	if( query_data->GetResultCount() > 0 )
	{
		PACKET_GUARD_VOID( query_data->GetValue( byShutDownUser, sizeof(BYTE) ) );
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );
	}

	ioUserSelectShutDown &rUserSelectShutDown = pUser->GetUserSelectShutDown();
	rUserSelectShutDown.SetSelectShutDown( (int)byShutDownUser, dts );

	CTime CurrentTime = CTime::GetCurrentTime();
	bool bNowShutDown = rUserSelectShutDown.IsShutDown( CurrentTime );
	CTime ShutDownTime = rUserSelectShutDown.GetShutDownTime();

	if( byShutDownUser == 1 )
	{
		SP2Packet kPacket( STPK_SELECT_SHUT_DOWN_USER );

		PACKET_GUARD_VOID( kPacket.Write(bNowShutDown) );
		PACKET_GUARD_VOID( kPacket.Write(ShutDownTime.GetYear()) );
		PACKET_GUARD_VOID( kPacket.Write(ShutDownTime.GetMonth()) );
		PACKET_GUARD_VOID( kPacket.Write(ShutDownTime.GetDay()) );
		PACKET_GUARD_VOID( kPacket.Write(ShutDownTime.GetHour()) );
		PACKET_GUARD_VOID( kPacket.Write(ShutDownTime.GetMinute()) );
	
		pUser->SendMessage( kPacket );
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s %s:%s [%d:%d:%d-%d-%d %d:%d]", __FUNCTION__, szUserGUID, szPrivateID, 
		                (int) bNowShutDown, (int)byShutDownUser, ShutDownTime.GetYear(), ShutDownTime.GetMonth(), ShutDownTime.GetDay(), ShutDownTime.GetHour(),  ShutDownTime.GetMinute() );

	if( !bNowShutDown )
	{
		// 고정 셧다운
		g_DBClient.OnSelectUserBirthDate( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPrivateID() );
	}
}

void UserNodeManager::OnResultSelectFriendRecommendData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectFriendRecommendData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	//char szUserID[ID_NUM_PLUS_ONE] = "";
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(LONG) ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectFriendRecommendData USER FIND NOT! :%d", dwUserIndex );
		return;
	}
	/*
	if( pUser->GetPublicID() != szUserID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectFriendRecommendData USER FIND NOT! :%s : %s", szUserID, szUserGUID );
		return;
	}
	*/
	DWORD dwTableIndex = 0, dwFriendIndex = 0;
	if( query_data->GetResultCount() > 0 )
	{
		PACKET_GUARD_VOID( query_data->GetValue( dwTableIndex, sizeof(LONG) ) );
		PACKET_GUARD_VOID( query_data->GetValue( dwFriendIndex, sizeof(LONG) ) );
	}

	if( dwTableIndex != 0 )
	{
		pUser->SetFriendRecommendData( dwTableIndex, dwFriendIndex );
	}
}

void UserNodeManager::OnResultSelectDisconnectCheck( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectDisconnectCheck Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	//보낸 유저의 아이디.
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szUserID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	query_data->GetValue( szUserID,ID_NUM_PLUS_ONE );       
	User *pUser = GetUserNodeByGUID( szUserGUID );
	if(pUser == NULL)
	{
		WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectDisconnectCheck USER FIND NOT! :%s : %s", szUserID , szUserGUID);
		return;
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	//SELECT
	char szDbID[ID_NUM_PLUS_ONE] = "";
	char szDbLoginKey[LOGIN_KEY_PLUS_ONE] = "";
	__int64  iDbServerID = 0;
	DBTIMESTAMP dts;

	query_data->GetValue( szDbID,ID_NUM_PLUS_ONE );
	query_data->GetValue( szDbLoginKey,LOGIN_KEY_PLUS_ONE );
	query_data->GetValue( iDbServerID,sizeof(__int64) );
	query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) );

	char szPlain[DATA_LEN]="";
	int result = DISCONNECT_ALREADY_ID_OK;
	if( iDbServerID == 0 )
	{
		result = DISCONNECT_ALREADY_ID_EXCEPT;
		WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : ServerID == 0 :%s",__FUNCTION__, szUserID );
	}
	else if( pUser->GetEncLoginKey() == NULL )
	{
		result = DISCONNECT_ALREADY_ID_EXCEPT;
		WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : LoginKey == NULL :%s",__FUNCTION__, szUserID );
	}
	else if( pLocal == NULL )
	{
		result = DISCONNECT_ALREADY_ID_EXCEPT;
		WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : pLocal == NULL :%s",__FUNCTION__, szUserID );
	}
	else if( !pLocal->DecryptLoginKey( pUser->GetEncLoginKey()->c_str(), szDbLoginKey, szPlain, sizeof( szPlain ) ) )
	{
		result = DISCONNECT_ALREADY_ID_EXCEPT;
		WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Error Decode :%s-%s-%s",__FUNCTION__, szUserID, pUser->GetEncLoginKey()->c_str(), szDbLoginKey );
	}
	else if( !pLocal->IsRightLoginKey( szDbLoginKey, szPlain ) )
	{
		result = DISCONNECT_ALREADY_ID_EXCEPT;
		WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : RightLogin :%s-%s-%s",__FUNCTION__, szUserID, pUser->GetEncLoginKey()->c_str(), szDbLoginKey );
	}

	if( result == DISCONNECT_ALREADY_ID_OK )
	{
		UserParent *pTempUser = GetGlobalUserNodeByPrivateID( szUserID );
		if( pTempUser )
		{
			if( pTempUser->IsUserOriginal() )
			{		
				User *pUserOriginal = (User*)pTempUser;
				if( pUserOriginal )
				{
					pUserOriginal->CloseConnection();
//					pUserOriginal->ExceptionClose(0);
					WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DisconnectAlreadyID[2]-%s-%s", szUserID, szUserGUID );
				}
			}
			else
			{
				//해당 유저 서버로 전송
				UserCopyNode *pUserCopy = (UserCopyNode*)pTempUser;
				SP2Packet kPacket( SSTPK_DISCONNECT_ALREADY_ID );
				kPacket << szUserGUID;
				pUserCopy->RelayPacket( kPacket );
				WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DisconnectAlreadyID[2]-%s-%s-%s-%d", szUserID, szUserGUID, pUserCopy->GetServerIP().c_str(), pUserCopy->GetClientPort() );
			}
		}
		else
		{
			WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DisconnectAlreadyID[3]-Not Found %s-%s", szUserID, szUserGUID);
		}
	}

	pUser->ClearEncLoginKey();

	SP2Packet kPacket( STPK_DISCONNECT_ALREADY_ID );
	kPacket << result;
	pUser->SendMessage( kPacket );
	pUser->SetPrivateID( "" );
}



// 2020-06-05 by bckim, 하드시리얼 유저 제재
void UserNodeManager::OnResultSelectDisconnectHddSerialCheck(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectDisconnectHddSerialCheck Result FAILED! :%d",query_data->GetResultType());
		return;
	}


	DWORD dwAccoutidx = 0;
	int ustrlen = 0;

	query_data->GetValue( dwAccoutidx, sizeof(DWORD) );
	query_data->GetValue( ustrlen, sizeof(int) );		 

	char szHdd[100] = "";
	query_data->GetValue( szHdd, ustrlen );		 

	User *pUser = GetUserNode( dwAccoutidx );
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectDisconnectHddSerialCheck USER FIND NOT! :%lu", dwAccoutidx);
		return;
	}

	int result = 0;
	query_data->GetValue( result, sizeof(int) );		 


	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB DBAGENT_DISCONNECT_HDD_SERIAL_CHECK_GET_RESULT!!!!!!");

	if( result == 1001 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DISCONNECT_HDD_SERIAL>>>>>>>>%s-%lu",pUser->GetPublicID().c_str(),  dwAccoutidx );

		/*
		UserParent *pTempUser = GetGlobalUserNode( dwAccoutidx );
		if( pTempUser )
		{			
			if( pTempUser->IsUserOriginal() )			
			{		
				User *pUserOriginal = (User*)pTempUser;
				if( pUserOriginal )
				{
					// pUserOriginal->CloseConnection();			클라이언트에서 종료 시키기로 함
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnDisconnectHddSerialBlock[original_close]-%s-%lu",pUserOriginal->GetPublicID().c_str(),  dwAccoutidx );
				}
			}
			else
			{
				//해당 유저 서버로 전송
				UserCopyNode *pUserCopy = (UserCopyNode*)pTempUser;
				SP2Packet kPacket( SSTPK_DISCONNECT_HDD_SERIAL_BLOCK );
				kPacket << dwAccoutidx;
				kPacket << result;
				pUserCopy->RelayPacket( kPacket );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectDisconnectHddSerialCheck[original>>otherserver]-%s-%lu",pUserCopy->GetPublicID().c_str(),  dwAccoutidx );
			}			
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectDisconnectHddSerialCheck[3]-Not Found");
		}	
		*/
	}

	
	SP2Packet kPacket( STPK_DISCONNECT_HDD_SERIAL_BLOCK );
	kPacket << result;
	pUser->SendMessage( kPacket );
}
// End. 2020-06-05 by bckim, 하드시리얼 유저 제재

// alchemic
void UserNodeManager::OnResultInsertAlchemicData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertAlchemicData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultSelectAlchemicIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectAlchemicIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwInvenIdx = 0;
	DWORD dwUserIdx = 0;
// 	char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
// 	char   szUserID[ID_NUM_PLUS_ONE] = "";
// 
// 	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
// 	query_data->GetValue( szUserID, ID_NUM_PLUS_ONE );        //보낸 유저의 아이디.
	query_data->GetValue( dwUserIdx, sizeof(int) );
	query_data->GetValue( dwInvenIdx, sizeof(LONG) );		 //새로운 인벤토리 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectAlchemicIndex USER FIND NOT! :%d", dwUserIdx);
		return;
	}

	ioAlchemicInventory *pInven = pUser->GetAlchemicInventory();
	if( !pInven )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectAlchemicIndex : (%d - %d) pItem == NULL.", dwUserIdx, dwInvenIdx );
		return;
	}

	if( !pInven->DBtoNewIndex( dwInvenIdx ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectAlchemicIndex : (%d - %d) 새로운 인덱스를 넣을 수 없습니다.", dwUserIdx, dwInvenIdx );
		return;
	}
}

void UserNodeManager::OnResultUpdateAlchemicData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateAlchemicData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultLoginSelectAllAlchemicData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllAlchemicData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";

	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(LONG) ) );		 //캐릭터 주인 인덱스

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllAlchemicData USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllAlchemicData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioAlchemicInventory *pInven = pUser->GetAlchemicInventory();
	if( pInven )
		pInven->DBtoData( query_data );
}

void UserNodeManager::OnResultInsertTournamentTeamCreate(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertTournamentTeamCreate Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	
	DWORD dwDBAgentID, dwAgentThread, dwUserIndex, dwTourIndex;
	query_data->GetValue( dwDBAgentID, sizeof(DWORD) );
	query_data->GetValue( dwAgentThread, sizeof(DWORD) );
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTourIndex, sizeof(DWORD) );

	int iResult;
	query_data->GetValue( iResult, sizeof(int) );

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL,"DB OnResultInsertTournamentTeamCreate %d - %d - %d", dwUserIndex, dwTourIndex, iResult );
	if( iResult == TOURNAMENT_TEAM_CREATE_OK )
	{
		g_DBClient.OnSelectTournamentTeamIndex( dwDBAgentID, dwAgentThread, dwUserIndex, dwTourIndex );
	}
	else
	{
		// 실패 전송
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_TOURNAMENT_TEAM_CREATE );
			kPacket << iResult;
			pUserParent->RelayPacket( kPacket );
		}
	}
}

void UserNodeManager::OnResultSelectTournamentTeamIndex(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentTeamIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwDBAgentID, dwAgentThread, dwUserIndex, dwTourIndex;
	query_data->GetValue( dwDBAgentID, sizeof(DWORD) );
	query_data->GetValue( dwAgentThread, sizeof(DWORD) );
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTourIndex, sizeof(DWORD) );

	DWORD dwTeamIndex;
	query_data->GetValue( dwTeamIndex, sizeof(DWORD) );

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL,"DB OnResultSelectTournamentTeamIndex %d - %d - %d", dwUserIndex, dwTourIndex, dwTeamIndex );
	if( dwTeamIndex == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "game_league_team_self_index failed!!!!" );
	}
	else
	{
		g_DBClient.OnSelectTournamentCreateTeamData( dwDBAgentID, dwAgentThread, dwUserIndex, dwTeamIndex );
	}
}

void UserNodeManager::OnResultSelectTournamentTeamList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentTeamList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL ) 
	{
		TournamentBot* pBot = TournamentBot::GetTournamentBot( dwUserIndex );
		if( pBot )
		{
			pBot->ApplyUserTeamIndex( query_data );
			return;
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentTeamList USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	if( pUser->GetUserIndex() != dwUserIndex )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentTeamList USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	ioUserTournament *pTournament = pUser->GetUserTournament();
	if( pTournament )
	{
		pTournament->DBtoData( query_data );
	}
}

void UserNodeManager::OnResultSelectTournamentCreateTeamData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentCreateTeamData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	DWORD dwUserIndex, dwTeamIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTeamIndex, sizeof(DWORD) );

	//- 리그인덱스, 팀이름, 팀장인덱스, 리그포지션, 팀맥스카운트, 응원포인트, 토너먼트위치, 진영포인트, 진영타입
	SHORT LeaguePos;
	BYTE TourPos, CampPos, MaxPlayer;
	int iCheerPoint, iLadderPoint;
	DWORD dwTourIndex, dwOwnerIndex;
	char szTeamName[TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE] = "";

	query_data->GetValue( dwTourIndex, sizeof(DWORD) );
	query_data->GetValue( szTeamName, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE );
	query_data->GetValue( dwOwnerIndex, sizeof(DWORD) );
	query_data->GetValue( LeaguePos, sizeof(SHORT) );
	query_data->GetValue( MaxPlayer, sizeof(char) );	
	query_data->GetValue( iCheerPoint, sizeof(int) );
	query_data->GetValue( TourPos, sizeof(char) );
	query_data->GetValue( iLadderPoint, sizeof(int) );
	query_data->GetValue( CampPos, sizeof(char) );

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL,"DB OnResultSelectTournamentCreateTeamData %d - %d - %d - %s - %d - %d - %d", dwUserIndex, dwTeamIndex, dwTourIndex, szTeamName, dwOwnerIndex, iLadderPoint, (int)CampPos );

	// 메인 서버로 보낸다.
	SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_CREATE );
	kPacket << dwUserIndex << dwTourIndex << dwTeamIndex << szTeamName << dwOwnerIndex << iLadderPoint << CampPos;
	g_MainServer.SendMessage( kPacket );
}

void UserNodeManager::OnResultSelectTournamentTeamMember(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentTeamMember Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	DWORD dwUserIndex, dwTeamIndex;
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTeamIndex, sizeof(DWORD) );
	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentTeamMember USER FIND NOT! :%d : %s", dwUserIndex, szUserGUID );
		return;
	}

	if( pUser->GetUserIndex() != dwUserIndex )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentTeamMember USER FIND NOT! :%d : %s", dwUserIndex, szUserGUID );
		return;
	}

	// - 테이블인덱스, 유저인덱스, 닉네임, 레벨, 진영포인트, 길드인덱스
	ioUserTournament *pTournament = pUser->GetUserTournament();
	if( pTournament )
	{
		pTournament->DBtoUserData( dwTeamIndex, query_data );
	}
}

void UserNodeManager::OnResultSelectTournamentTeamAppList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentTeamAppList Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	DWORD dwUserIndex, dwTourIndex, dwTeamIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTourIndex, sizeof(DWORD) );
	query_data->GetValue( dwTeamIndex, sizeof(DWORD) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentTeamAppList USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	// - 테이블인덱스, 유저인덱스, 닉네임, 레벨, 길드인덱스
	ioUserTournament::TeamUserVec kTeamUser;
	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		ioUserTournament::TeamUserData kUserData;

		query_data->GetValue( kUserData.m_dwTableIndex, sizeof(DWORD) );
		query_data->GetValue( kUserData.m_dwUserIndex, sizeof(DWORD) );

		char szUserNick[ID_NUM_PLUS_ONE] = "";
		query_data->GetValue( szUserNick, ID_NUM_PLUS_ONE );
		kUserData.m_szNick = szUserNick;

		query_data->GetValue( kUserData.m_iGradeLevel, sizeof(int) );
		query_data->GetValue( kUserData.m_dwGuildIndex, sizeof(int) );

		query_data->GetValue( kUserData.m_CampPos, sizeof(BYTE) );

		kTeamUser.push_back( kUserData );
	}	
	LOOP_GUARD_CLEAR();

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnSelectTournamentTeamAppList Result : %d", (int)kTeamUser.size() );
	if( kTeamUser.empty() ) return;         // 대기자 없음

	int iSize = (int)kTeamUser.size();
	SP2Packet kPacket( STPK_TOURNAMENT_TEAM_ENTRY_MEMBER );
	kPacket << dwTourIndex << dwTeamIndex << iSize;
	for(int i = 0;i < iSize;i++)
	{
		ioUserTournament::TeamUserData &rkUserData = kTeamUser[i];

		UserParent *pGlobalUser = GetGlobalUserNode( rkUserData.m_dwUserIndex );
		if( pGlobalUser )
		{
			rkUserData.m_iGradeLevel = pGlobalUser->GetGradeLevel();
			rkUserData.m_iLadderPoint= pGlobalUser->GetLadderPoint();
			rkUserData.m_CampPos	 = pGlobalUser->GetUserCampPos();
		}

		kPacket << rkUserData.m_dwTableIndex << rkUserData.m_dwUserIndex << rkUserData.m_iGradeLevel << rkUserData.m_szNick << rkUserData.m_CampPos;
	}
	pUserParent->RelayPacket( kPacket );
}

void UserNodeManager::OnResultInsertTournamentTeamAppAdd(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertTournamentTeamAppAdd Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex, dwTourIndex, dwTeamIndex, dwMasterIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTourIndex, sizeof(DWORD) );
	query_data->GetValue( dwTeamIndex, sizeof(DWORD) );
	query_data->GetValue( dwMasterIndex, sizeof(DWORD) );

	int iResult;
	query_data->GetValue( iResult, sizeof(int) );

	if( iResult == TOURNAMENT_TEAM_ENTRY_APP_OK )
	{
		// 마스터에게 알림
		UserParent *pMasterParent = GetGlobalUserNode( dwMasterIndex );
		if( pMasterParent )
		{			
			g_DBClient.OnSelectTournamentTeamAppList( pMasterParent->GetUserDBAgentID(), pMasterParent->GetAgentThreadID(), dwMasterIndex, dwTourIndex, dwTeamIndex );
		}
	}

	// 신청자에게 전송
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertTournamentTeamAppAdd USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	SP2Packet kPacket( STPK_TOURNAMENT_TEAM_ENTRY_APP );
	kPacket << iResult << dwTourIndex << dwTeamIndex;
	pUserParent->RelayPacket( kPacket );
}

void UserNodeManager::OnResultUpdateTournamentTeamAppReg(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateTournamentTeamAppReg Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex, dwTourIndex, dwTeamIndex, dwRegUserIndex, dwTableIndex, dwAgentID, dwThreadID;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTourIndex, sizeof(DWORD) );
	query_data->GetValue( dwTeamIndex, sizeof(DWORD) );
	query_data->GetValue( dwRegUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTableIndex, sizeof(DWORD) );
	query_data->GetValue( dwAgentID, sizeof(DWORD) );
	query_data->GetValue( dwThreadID, sizeof(DWORD) );

	int iResult;
	query_data->GetValue( iResult, sizeof(int) );
	
	if( iResult == TOURNAMENT_TEAM_ENTRY_AGREE_OK )
	{
		//
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			// 유저 가입 후 리스트 가져오는 쿼리
			g_DBClient.OnSelectTournamentTeamAppAgreeMember( pUserParent->GetUserDBAgentID(), pUserParent->GetAgentThreadID(), pUserParent->GetUserIndex(), dwTourIndex, dwTeamIndex, dwRegUserIndex );
		}

		UserParent *pRegUser = GetGlobalUserNode( dwRegUserIndex );
		if( pRegUser )
		{
			// 팀에 가입되어 리스트 가져오는 쿼리
			g_DBClient.OnSelectTournamentAppAgreeTeam( pRegUser->GetUserDBAgentID(), pRegUser->GetAgentThreadID(), pRegUser->GetUserIndex(), dwTeamIndex );
		}

		// 승인된 유저 대기자 리스트
		g_DBClient.OnDeleteTournamentTeamAppList( dwAgentID, dwThreadID, dwRegUserIndex, dwTourIndex );
	}
	else
	{
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_TOURNAMENT_TEAM_ENTRY_AGREE );
			kPacket << iResult;
			pUserParent->RelayPacket( kPacket );
		}

		// 대기자 삭제
		g_DBClient.OnDeleteTournamentTeamAppDel( dwAgentID, dwThreadID, dwTableIndex );
	}
}

void UserNodeManager::OnResultSelectTournamentTeamAppAgreeMember(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentTeamAppAgreeMember Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex, dwTourIndex, dwTeamIndex, dwAppUserIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTourIndex, sizeof(DWORD) );
	query_data->GetValue( dwTeamIndex, sizeof(DWORD) );
	query_data->GetValue( dwAppUserIndex, sizeof(DWORD) );

	ioUserTournament::TeamUserData kAppUserData;
	while( query_data->IsExist() )
	{		
		// - 테이블인덱스, 유저인덱스, 닉네임, 레벨, 진영포인트, 길드인덱스
		ioUserTournament::TeamUserData kUserData;

		query_data->GetValue( kUserData.m_dwTableIndex, sizeof(DWORD) );
		query_data->GetValue( kUserData.m_dwUserIndex, sizeof(DWORD) );

		char szUserNick[ID_NUM_PLUS_ONE] = "";
		query_data->GetValue( szUserNick, ID_NUM_PLUS_ONE );
		kUserData.m_szNick = szUserNick;

		query_data->GetValue( kUserData.m_iGradeLevel, sizeof(int) );
		query_data->GetValue( kUserData.m_iLadderPoint, sizeof(int) );
		query_data->GetValue( kUserData.m_dwGuildIndex, sizeof(int) );

		if( kUserData.m_dwUserIndex == dwAppUserIndex )
		{
			kAppUserData = kUserData;
			break;
		}
	}	

	// 가입한 유저가 플레이중이면 데이터 갱신
	UserParent *pAppUserParent = GetGlobalUserNode( kAppUserData.m_dwUserIndex );
	if( pAppUserParent )
	{
		kAppUserData.m_iGradeLevel = pAppUserParent->GetGradeLevel();
		kAppUserData.m_iLadderPoint= pAppUserParent->GetLadderPoint();
		kAppUserData.m_dwGuildIndex= pAppUserParent->GetGuildIndex();
	}

	// 메인 서버에 진영 포인트 증가
	if( kAppUserData.m_iLadderPoint > 0 )
	{
		SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_LADDER_POINT_ADD );
		kPacket << dwTourIndex << dwTeamIndex << kAppUserData.m_iLadderPoint;
		g_MainServer.SendMessage( kPacket );
	}

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent )
	{
		if( kAppUserData.m_dwUserIndex != dwAppUserIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultSelectTournamentTeamAppAgreeMember Result Error :%d - %d", dwTeamIndex, dwAppUserIndex );
		}
		else
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUser = static_cast< User * >( pUserParent );
				ioUserTournament *pUserTournament = pUser->GetUserTournament();
				if( pUserTournament )
				{
					pUserTournament->AddTeamUserData( dwTeamIndex, kAppUserData );
				}
			}
			else
			{
				UserCopyNode *pUser = static_cast< UserCopyNode * >( pUserParent );
				SP2Packet kPacket( SSTPK_TOURNAMENT_TEAM_ENTRY_AGREE_OK );
				kPacket << pUser->GetUserIndex() << dwTeamIndex << kAppUserData.m_dwTableIndex << kAppUserData.m_dwUserIndex << kAppUserData.m_szNick;
				kPacket << kAppUserData.m_iGradeLevel << kAppUserData.m_iLadderPoint << kAppUserData.m_dwGuildIndex; 
				pUser->SendMessage( kPacket );
			}

			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnResultSelectTournamentTeamAppAgreeMember Result OK :%d - %d", dwTeamIndex, kAppUserData.m_dwUserIndex );
		}
	}
}

void UserNodeManager::OnResultSelectTournamentAppAgreeTeam(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentAppAgreeTeam Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex, dwTeamIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTeamIndex, sizeof(DWORD) );

	//- 리그인덱스, 팀이름, 팀장인덱스, 리그포지션, 팀맥스카운트, 응원포인트, 토너먼트위치, 진영포인트, 진영타입
	SHORT LeaguePos;
	BYTE TourPos, CampPos, MaxPlayer;
	int iCheerPoint, iLadderPoint;
	DWORD dwTourIndex, dwOwnerIndex;
	char szTeamName[TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE] = "";

	query_data->GetValue( dwTourIndex, sizeof(DWORD) );
	query_data->GetValue( szTeamName, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE );
	query_data->GetValue( dwOwnerIndex, sizeof(DWORD) );
	query_data->GetValue( LeaguePos, sizeof(SHORT) );
	query_data->GetValue( MaxPlayer, sizeof(char) );	
	query_data->GetValue( iCheerPoint, sizeof(int) );
	query_data->GetValue( TourPos, sizeof(char) );
	query_data->GetValue( iLadderPoint, sizeof(int) );
	query_data->GetValue( CampPos, sizeof(char) );

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL,"DB OnResultSelectTournamentAppAgreeTeam %d - %d - %d - %s - %d - %d - %d", dwUserIndex, dwTeamIndex, dwTourIndex, szTeamName, dwOwnerIndex, iLadderPoint, (int)CampPos );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent )
	{
		//
		if( pUserParent->IsUserOriginal() )
		{
			User *pUser = static_cast< User * >( pUserParent );
			ioUserTournament *pUserTournament = pUser->GetUserTournament();
			if( pUserTournament )
			{
				if( pUserTournament->JoinTeamData( dwTourIndex, dwTeamIndex, dwOwnerIndex, szTeamName, LeaguePos, TourPos ) )
				{
					SP2Packet kPacket( STPK_TOURNAMENT_TEAM_JOIN );
					kPacket << dwTourIndex << dwTeamIndex << szTeamName << dwOwnerIndex << LeaguePos << TourPos;					
					pUser->SendMessage( kPacket );
				}
			}
		}
		else
		{
			UserCopyNode *pUser = static_cast< UserCopyNode * >( pUserParent );
			SP2Packet kPacket( SSTPK_TOURNAMENT_TEAM_JOIN );
			kPacket << pUser->GetUserIndex() << dwTourIndex << dwTeamIndex << szTeamName << dwOwnerIndex << LeaguePos << TourPos;				
			pUser->SendMessage( kPacket );
		}
	}
}

void UserNodeManager::OnResultDeleteTournamentTeamMember(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentAppAgreeTeam Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex, dwLeaveUserIndex, dwTourIndex, dwTeamIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwLeaveUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwTourIndex, sizeof(DWORD) );
	query_data->GetValue( dwTeamIndex, sizeof(DWORD) );

	int iResult;
	query_data->GetValue( iResult, sizeof(int) );		  // 결과 : 음수값일 경우 에러 !!! 양수일 때 진영 포인트

	if( iResult >= 0 )
	{
		// 탈퇴 가능
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUser = static_cast< User * >( pUserParent );
				ioUserTournament *pUserTournament = pUser->GetUserTournament();
				if( pUserTournament )
				{
					pUserTournament->LeaveTeamUser( dwUserIndex, dwTeamIndex, dwLeaveUserIndex );
				}
			}
			else
			{
				UserCopyNode *pUser = static_cast< UserCopyNode * >( pUserParent );
				SP2Packet kPacket( SSTPK_TOURNAMENT_TEAM_LEAVE );
				kPacket << pUser->GetUserIndex() << dwUserIndex << dwTeamIndex << dwLeaveUserIndex;
				pUser->SendMessage( kPacket );
			}
		}

		// 메인 서버에 진영 포인트 감소
		if( iResult > 0 )
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_LADDER_POINT_ADD );
			kPacket << dwTourIndex << dwTeamIndex << -iResult;
			g_MainServer.SendMessage( kPacket );
		}
	}
	else
	{
		// 실패 전송
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_TOURNAMENT_TEAM_LEAVE );
			kPacket << TOURNAMENT_TEAM_LEAVE_FAILED_NON_MEMBER;
			pUserParent->RelayPacket( kPacket );
		}
	}
}

void UserNodeManager::OnResultSelectTournamentHistoryList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentHistoryList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

}

void UserNodeManager::OnResultSelectTournamentHistoryUserList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentHistoryUserList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

}

void UserNodeManager::OnResultSelectTournamentReward(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentReward Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentReward USER FIND NOT! :%d", dwUserIndex  );
		return;
	}

	if( pUser->GetUserIndex() != dwUserIndex )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentReward USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	ioUserTournament* pTournament = pUser->GetUserTournament();
	if( !pTournament )
	{
		LOG.PrintTimeAndLog(0 , " %s - not find tournament", __FUNCTION__ );
		return;
	}

	if( query_data->IsExist() )
	{
		BYTE TourPos;
		DWORD dwTableIndex, dwStartDate;
		int iMyCampPos, iWinCampPos, iLadderBonusPeso, iLadderRank, iLadderPoint;

		PACKET_GUARD_VOID( query_data->GetValue( dwTableIndex, sizeof(DWORD) ) );
		PACKET_GUARD_VOID( query_data->GetValue( dwStartDate, sizeof(DWORD) ) );
		PACKET_GUARD_VOID( query_data->GetValue( TourPos, sizeof(BYTE) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iMyCampPos, sizeof(int) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iWinCampPos, sizeof(int) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iLadderBonusPeso, sizeof(int) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iLadderRank, sizeof(int) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iLadderPoint, sizeof(int) ) );

		pTournament->SetRegularRewardData( dwTableIndex, dwStartDate, TourPos, iMyCampPos, iWinCampPos, iLadderBonusPeso, iLadderRank, iLadderPoint );
		g_TournamentManager.InsertRegularTournamentReward( pUser, dwTableIndex, dwStartDate, TourPos, iMyCampPos, iWinCampPos, iLadderBonusPeso, iLadderRank, iLadderPoint );
	}
	else
	{
		pTournament->SetRegularRewardData( 0, 0, CAMP_NONE, CAMP_NONE, 0, 0, 0, 0 );
	}
	
	//진영전, 대회참여 상관없이 응원보상 가져오기
	g_DBClient.OnSelectTournamentCheerReward( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetUserIndex(), CRT_CHAMP );
}

void UserNodeManager::OnResultInsertTournamentCustomAdd(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertTournamentCustomAdd Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex, dwUseEtcItem;
	char szPublicID[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue( dwUserIndex, sizeof( DWORD ) );
	query_data->GetValue( dwUseEtcItem, sizeof( DWORD ) );
	query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE );

	// 생성된 토너먼트 인덱스
	DWORD dwTourIndex;
	query_data->GetValue( dwTourIndex, sizeof( DWORD ) );
	
	if( dwTourIndex > 0 )
	{
		// 생성 완료!!
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultInsertTournamentCustomAdd - %s : %d", szPublicID, dwTourIndex );
		
		// 메인 서버에 전달하여 대회 로드
		SP2Packet kMainPacket( MSTPK_TOURNAMENT_CUSTOM_CREATE );
		kMainPacket << dwUserIndex << dwTourIndex;
		g_MainServer.SendMessage( kMainPacket );
	}
	else
	{
		// 대회 생성이 DB에서 Insert되지 못했으로 선물로 특별아이템 지급
		ioHashString kSendID		= g_MainServer.GetSendID();
		SHORT        iPresentType	= PRESENT_ETC_ITEM;
		int          iPresentValue2	= 1;
		short        iPresentMent   = 3;      // 운영팀이 지급하는 선물 기본 멘트
		short        iPresentState  = ioUserPresent::PRESENT_STATE_NEW;
		int          iPresentDay    = 30;
		ioHashString kLogMent       = "restore";

		//유효기간
		CTimeSpan cPresentGapTime( iPresentDay, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		//
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			// 로그인 유저
			SP2Packet kPacket( STPK_TOURNAMENT_CUSTOM_CREATE );
			kPacket << TOURNAMENT_CUSTOM_CREATE_EXCEPTION;
			pUserParent->RelayPacket( kPacket );

			// 
			if( pUserParent->IsUserOriginal() )
			{
				User *pUser = static_cast< User * >( pUserParent );

				pUser->AddPresentMemory( kSendID, iPresentType, dwUseEtcItem, iPresentValue2, 0, 0, iPresentMent, kPresentTime, iPresentState );
				g_LogDBClient.OnInsertPresent( 0, kSendID, g_App.GetPublicIP().c_str(), dwUserIndex, iPresentType, dwUseEtcItem, iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, kLogMent.c_str() );
				pUser->SendPresentMemory();
			}
			else
			{
				// 유저가 서버를 이동했으면....
				UserCopyNode *pUser = static_cast< UserCopyNode * >( pUserParent );

				SP2Packet kPacket( SSTPK_PRESENT_INSERT );
				kPacket << dwUserIndex << kSendID << iPresentType << dwUseEtcItem << iPresentValue2 << iPresentMent << iPresentDay << iPresentState << kLogMent; 
				pUser->SendMessage( kPacket );
			}
		}
		else 
		{
			// 로그아웃 유저
			g_DBClient.OnInsertPresentData( 0, dwUserIndex, kSendID, szPublicID, iPresentType, dwUseEtcItem, iPresentValue2, 0, 0, iPresentMent, kPresentTime, iPresentState );
			g_LogDBClient.OnInsertPresent( 0, kSendID, g_App.GetPublicIP().c_str(), dwUserIndex, iPresentType, dwUseEtcItem, iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, kLogMent.c_str() );
		}
	}
}

void UserNodeManager::OnResultSelectTournamentCustomReward(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentCustomReward Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentCustomReward USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	if( pUser->GetUserIndex() != dwUserIndex )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentCustomReward USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	while( query_data->IsExist() )
	{		
		// - 테이블인덱스, 리그명, 라운드, 최대라운드, 보상1, 보상2, 보상3, 보상4
		DWORD dwTableIndex;
		char  szNickName[ID_NUM_PLUS_ONE] = "";
		char  szTourName[TOURNAMENT_TITLE_NUM_PLUS_ONE] = "";
		int   iCurrentRound;
		short MaxRound;
		DWORD dwReward1, dwReward2, dwReward3, dwReward4;

		PACKET_GUARD_BREAK( query_data->GetValue( dwTableIndex, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( szNickName, ID_NUM_PLUS_ONE ) );
		PACKET_GUARD_BREAK( query_data->GetValue( szTourName, TOURNAMENT_TITLE_NUM_PLUS_ONE ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iCurrentRound, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( MaxRound, sizeof(short) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( dwReward1, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( dwReward2, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( dwReward3, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( dwReward4, sizeof(DWORD) ) );

		g_TournamentManager.InsertCustomTournamentReward( pUser, dwTableIndex, szNickName, szTourName, iCurrentRound, MaxRound, dwReward1, dwReward2, dwReward3, dwReward4 );
	}	
}

void UserNodeManager::OnResultSelectCloverInfoRequest( CQueryResultData* query_data )
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCloverInfo Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// Declare) user
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	DWORD dwUserIdx = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(DWORD) ) );

	// Get User
	User* pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectCloverInfo USER FIND NOT! : %d", dwUserIdx );
		return;
	}

	// Get Clover
	ioClover* pClover = pUser->GetClover();
	if( pClover )
	{
		pClover->DBtoData( query_data );
	}
}

void UserNodeManager::OnResultUpdateFriendReceiveCloverInfo( CQueryResultData* query_data )
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateFriendReceiveCloverInfo Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	int iUserIndex = 0;
	query_data->GetValue( iUserIndex, sizeof( int ) );

	int iTableIndex, iFriendAccountIdx, iReceiveCloverDate, iReceiveBCloverCnt;
	query_data->GetValue( iTableIndex, sizeof( int ) );
	query_data->GetValue( iFriendAccountIdx, sizeof( int ) );
	query_data->GetValue( iReceiveCloverDate, sizeof( int ) );
	query_data->GetValue( iReceiveBCloverCnt, sizeof( int ) );

	// 유저 로그인 했는지 체크.
	UserParent *pUserParent = GetGlobalUserNode( iUserIndex );
	if( pUserParent )
	{
		// 접속중
		if( pUserParent->IsUserOriginal() )
		{
			// 같은서버.
			User* pUser = dynamic_cast< User* >( pUserParent );

			// Friend
			ioFriend* pTargetFriend = pUser->GetFriend();
			pTargetFriend->SetFriendBeforeReceiveCloverData( iFriendAccountIdx, Help::GetCloverSendCount(), iReceiveCloverDate );

			// 메모리 넣고 알려줌.
			pUser->CloverFriendInfo( iFriendAccountIdx, ioClover::FRIEND_CLOVER_COME_TO_FRIEND );
		}
		else
		{
			// 다른서버.
			UserCopyNode* pUser = (UserCopyNode*)pUserParent;
			
			// 다른서버로 전송.
			SP2Packet kPacket( SSTPK_CLOVER_SEND );
			kPacket << iFriendAccountIdx << static_cast< DWORD >( iUserIndex ) << iReceiveCloverDate;
			pUser->SendMessage( kPacket );
		}
	}
	else
	{
		// 처리 안해도 된다.
	}
}

void UserNodeManager::OnResultSelectBingoNumber( CQueryResultData* query_data )
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBingoNumber Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	int iUserIndex = 0;
	query_data->GetValue( iUserIndex, sizeof( int ) );

	// 유저 로그인 했는지 체크.
	UserParent *pUserParent = GetGlobalUserNode( iUserIndex );
	if( pUserParent == NULL )
		return;
	
	// 접속중
	if( pUserParent->IsUserOriginal() )
	{
		// 같은서버.
		User* pUser = dynamic_cast< User* >( pUserParent );

		// Get Bingo
		ioBingo* pBingo = pUser->GetBingo();

		// Set
		pBingo->DBtoData_Number( query_data );
	}
}

void UserNodeManager::OnResultSelectBingoPresent( CQueryResultData* query_data )
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBingoPresent Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	int iUserIndex = 0;
	query_data->GetValue( iUserIndex, sizeof( int ) );

	// 유저 로그인 했는지 체크.
	UserParent *pUserParent = GetGlobalUserNode( iUserIndex );
	if( pUserParent == NULL )
		return;
	
	// 접속중
	if( pUserParent->IsUserOriginal() )
	{
		// 같은서버.
		User* pUser = dynamic_cast< User* >( pUserParent );

		// Get Bingo
		ioBingo* pBingo = pUser->GetBingo();

		// Set
		pBingo->DBtoData_Present( query_data );
	}
}

void UserNodeManager::OnResultSelectPirateRouletteNumber(CQueryResultData* query_data)
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectPirateRouletteNumber Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	int iUserIndex = 0;
	int iHP = 0;
	query_data->GetValue( iUserIndex, sizeof( int ) );

	// 유저 로그인 했는지 체크.
	UserParent *pUserParent = GetGlobalUserNode( iUserIndex );
	if( pUserParent == NULL )
		return;

	// 접속중
	if( pUserParent->IsUserOriginal() )
	{
		// 같은서버.
		User* pUser = dynamic_cast< User* >( pUserParent );

		// Get Bingo
		ioPirateRoulette* pRoulette = pUser->GetPirateRoulette();

		// Set
		pRoulette->DBtoData_RouletteBoard(query_data);
	}
}

void UserNodeManager::OnResultSelectPirateRoulettePresent(CQueryResultData* query_data)
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectPirateRoulettePresent Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	int iUserIndex = 0;
	int iHP = 0;
	query_data->GetValue( iUserIndex, sizeof( int ) );

	// 유저 로그인 했는지 체크.
	UserParent *pUserParent = GetGlobalUserNode( iUserIndex );
	if( pUserParent == NULL )
		return;

	// 접속중
	if( pUserParent->IsUserOriginal() )
	{
		// 같은서버.
		User* pUser = dynamic_cast< User* >( pUserParent );

		// Get Bingo
		ioPirateRoulette* pRoulette = pUser->GetPirateRoulette();

		// Set
		pRoulette->DBtoData_Present(query_data);
	}
}

void UserNodeManager::OnResultInsertPresent( CQueryResultData* query_data )
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertPresent Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	short iPresentType;
	int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
	int iReceiveIndex;
	query_data->GetValue( iPresentType, sizeof( short ) );
	query_data->GetValue( iPresentValue1, sizeof( int ) );
	query_data->GetValue( iPresentValue2, sizeof( int ) );
	query_data->GetValue( iPresentValue3, sizeof( int ) );
	query_data->GetValue( iPresentValue4, sizeof( int ) );
	query_data->GetValue( iReceiveIndex, sizeof( int ) );
	
	g_LogDBClient.OnInsertPresent( 
		0, 
			g_MainServer.GetSendID(),
		g_App.GetPublicIP().c_str(), 
		iReceiveIndex, 
		(short)iPresentType, 
		iPresentValue1, 
		iPresentValue2, 
		iPresentValue3, 
		iPresentValue4, 
		LogDBClient::PST_RECIEVE, 
		"Monitor" );	
}


void UserNodeManager::OnresultInsertPresentByPrivateID( CQueryResultData* query_data )
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnresultInsertPresentByPrivateID Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	short iPresentType = 0;
	int iPresentValue1 = 0, iPresentValue2 = 0, iPresentValue3 = 0, iPresentValue4 = 0;
	int iReceiveIndex = 0;
	query_data->GetValue( iPresentType, sizeof( short ) );
	query_data->GetValue( iPresentValue1, sizeof( int ) );
	query_data->GetValue( iPresentValue2, sizeof( int ) );
	query_data->GetValue( iPresentValue3, sizeof( int ) );
	query_data->GetValue( iPresentValue4, sizeof( int ) );
	query_data->GetValue( iReceiveIndex, sizeof( int ) );

	g_LogDBClient.OnInsertPresent( 
		0, 
			g_MainServer.GetSendID(),
		g_App.GetPublicIP().c_str(), 
		iReceiveIndex, 
		(short)iPresentType, 
		iPresentValue1, 
		iPresentValue2, 
		iPresentValue3, 
		iPresentValue4, 
		LogDBClient::PST_RECIEVE, 
		"Monitor" );	
}


void UserNodeManager::OnResultSelectRelativeGradeInfo( CQueryResultData* query_data )
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectRelativeGradeInfo Result FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	int iUserIndex = 0;
	int iBackupExp;
	int iInitTime;
	bool bReward = false;
	query_data->GetValue( iUserIndex, sizeof( int ) );
	query_data->GetValue( iBackupExp, sizeof( int ) ); //backup_exp
	query_data->GetValue( iInitTime, sizeof( int ) );
	query_data->GetValue( bReward, sizeof( char ) );

	UserParent *pUserParent = GetGlobalUserNode( iUserIndex );
	if( pUserParent == NULL )
		return;

	// 접속중
	if( pUserParent->IsUserOriginal() )
	{
		// 같은서버.
		User* pUser = dynamic_cast< User* >( pUserParent );
		if( pUser )
			pUser->SetUserRelativeGradeData( iInitTime, bReward, iBackupExp );
	}
}

void UserNodeManager::OnResultInsertTournamentCheerDecision( CQueryResultData* query_data )
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertTournamentCheerDecision FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	DWORD dwUserIndex;
	DWORD dwTourIndex;
	DWORD dwTeamIndex;
	int iResult;
	
	query_data->GetValue( dwUserIndex, sizeof( DWORD ) );
	query_data->GetValue( dwTourIndex, sizeof( DWORD ) );
	query_data->GetValue( dwTeamIndex, sizeof( DWORD ) );
	query_data->GetValue( iResult, sizeof( int ) );
	
	if( iResult == TOURNAMENT_CHEER_DECISION_OK )
	{
		SP2Packet kPacket( MSTPK_TOURNAMENT_CHEER_DECISION );
		kPacket << dwUserIndex;
		kPacket << dwTourIndex;
		kPacket << dwTeamIndex;
		g_MainServer.SendMessage( kPacket );
	}
	else
	{
		// 실패 전송
		UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
		if( pUserParent )
		{
			LOG.PrintTimeAndLog(0, "Tournament cheer decision fail : %s, %d", pUserParent->GetPublicID().c_str(), dwTeamIndex );

			SP2Packet kPacket( STPK_TOURNAMENT_CHEER_DECISION );
			kPacket << TOURNAMENT_CHEER_DECISION_ALREADY;
			pUserParent->RelayPacket( kPacket );
		}
	}
}

void UserNodeManager::OnResultSelectTournamentCheerList( CQueryResultData* query_data )
{
	if( FAILED( query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentCheerList FAILED! :%d", query_data->GetResultType() );
		return;
	}

	// User
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );
	DWORD dwUserIdx = 0;

	query_data->GetValue( dwUserIdx, sizeof( DWORD ) );

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s USER FIND NOT! : %d", __FUNCTION__, dwUserIdx );
		return;
	}

	ioUserTournament *pTournament = pUser->GetUserTournament();
	if( pTournament )
	{
		pTournament->DBtoCheerData( query_data );
	}
}

void UserNodeManager::OnResultSelectTournamentCheerReward( CQueryResultData* query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTournamentCheerReward Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex;
	DWORD dwCheerType;
	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );
	query_data->GetValue( dwCheerType, sizeof(DWORD) );
	query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentCheerReward USER FIND NOT! :%d : %s", dwUserIndex, szUserGUID );
		return;
	}

	if( pUser->GetUserIndex() != dwUserIndex )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectTournamentCheerReward USER FIND NOT! :%d : %s", dwUserIndex, szUserGUID );
		return;
	}

	ioUserTournament* pUserTournament = pUser->GetUserTournament();
	if( !pUserTournament )
	{
		LOG.PrintTimeAndLog(0, "%s - pUserTournament == NULL", __FUNCTION__ );
		return;
	}

	DWORD dwCheerTableIndex = 0;
	DWORD dwCheerPeso = 0;
		
	if( query_data->IsExist() )
	{
		query_data->GetValue( dwCheerTableIndex, sizeof(DWORD) );
		query_data->GetValue( dwCheerPeso, sizeof(DWORD) );
	}	

	switch( dwCheerType )	
	{
	case CRT_CHAMP:
		{
			const ioUserTournament::RegularRewardDBData& rkData = pUserTournament->GetRegularRewardData();
			g_TournamentManager.InsertRegularTournamentCheerReward( pUser,
				rkData.m_dwTableIndex,
				dwCheerTableIndex,
				rkData.m_dwStartDate,
				rkData.m_TourPos,
				rkData.m_iMyCampPos,
				rkData.m_iWinCampPos,
				rkData.m_iLadderBonusPeso,
				rkData.m_iLadderRank,
				rkData.m_iLadderPoint,
				dwCheerPeso );
		}
		break;
	/*case CRT_PREDICT:
		{

		}
		break;*/
	default:
		{
			LOG.PrintTimeAndLog(0, "%s - unknown cheer reward type", __FUNCTION__ );
		}		
	}
}

void UserNodeManager::OnResultSelectAttendanceRecord( CQueryResultData* query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectAttendanceRecord Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex;
	query_data->GetValue( dwUserIndex, sizeof(DWORD) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent == NULL )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser = dynamic_cast<User*>( pUserParent );
		if( pUser )
		{
			ioUserAttendance* pAttendance = pUser->GetUserAttendance();
			if( pAttendance )
			{
				pAttendance->DBtoData( query_data );
			}
			else
			{
				LOG.PrintTimeAndLog(0, "%s - pAttendance == NULL", __FUNCTION__ );
			}
		}
	}
}

void UserNodeManager::OnResultLoginSelectAllPetItemData( CQueryResultData* query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllPetItemData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int iUserIndex = 0;
	//char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char szUserID[ID_NUM_PLUS_ONE] = "";

	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue(szUserID,ID_NUM_PLUS_ONE) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue(iUserIndex,sizeof(int)) );		 //펫 주인 인덱스

	User *pUser = GetUserNode(iUserIndex);
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllPetItemData USER FIND NOT! :%d",iUserIndex);
		return;
	}
	if(pUser->GetUserIndex() != (DWORD)iUserIndex)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAllPetItemData USER INDEX NOT! :%d - %d",pUser->GetUserIndex(),iUserIndex);
		return;
	}

	ioUserPet *pPet = pUser->GetUserPetItem();
	if( pPet )
		pPet->DBtoData( query_data );
}

void UserNodeManager::SendNotUseRelayMessageAll( int relayServerID )
{
	for( int i = 0; i < static_cast<int>( m_uUserNode.size()  ); ++i )
	{
		if( m_uUserNode[i]->RelayServerID() == relayServerID )
		{
			SP2Packet pk(STPK_ON_CONTROL);
			int ctype = RC_NOTUSE_RELAYSVR;
			pk << ctype;
			m_uUserNode[i]->SetRelayServerID(0);
			m_uUserNode[i]->SendMessage(pk);
			m_uUserNode[i]->m_ping_total_send_index = 0; //핑이 기존걸로 돌아가면서 문제 생기지 않게 하기위해 
			if(m_uUserNode[i]->m_pMyRoom != NULL)
			{
				SP2Packet kPacket( STPK_EXIT_ROOM );
				kPacket << EXIT_ROOM_RELAYSVR_DISCONNECTED ;
				kPacket << -1;
				kPacket << false;
				m_uUserNode[i]->SendMessage( kPacket );
				m_uUserNode[i]->LeaveProcess();
			}
		}
	}
}

void UserNodeManager::OnResultSelectSubscriptionData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectSubscriptionData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex;
	query_data->GetValue( dwUserIndex, sizeof(LONG) );

	User *pUser = GetUserNode( dwUserIndex );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectPresentData USER FIND NOT! :%d", dwUserIndex );
		return;
	}	

	ioUserSubscription *pUserSubscription = pUser->GetUserSubscription();
	if( pUserSubscription )
		pUserSubscription->DBtoSubscriptionData( query_data );
}

User* UserNodeManager::GetUserInfo( DWORD index )
{
	 if(m_uUserNode.size() > index)
	 {
		 User* userInfo = m_uUserNode.at(index); //널체크 추가 

		 if(userInfo)
		 {
			 return  userInfo;
		 }
	 }
	 return NULL;
}

BOOL UserNodeManager::GetUserInfos( vUser& vUserInfos, int iStart, int iEnd)
{
	uUser_iter pos = m_uUserNode.begin();

	vUserInfos.clear();

	for(int i=0; i< iStart; i++)
	{
		if(pos != m_uUserNode.end())
			pos++;
		else
			return FALSE;
	}

	for(int i=iStart; i<iEnd; i++)
	{
		User* pUser = (*pos).second;

		if(pUser)
			vUserInfos.push_back(pUser);

		pos++;
	}

	return TRUE;
}

void UserNodeManager::FindShuffleGlobalQueueUser( User *pUser, int iGlbalSearchingTryCount, DWORDVec& vUserGroup )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : pUser == NULL", __FUNCTION__ );
		return;
	}
	
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );

	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pShuffleUser = iter->second;
		if( pShuffleUser == pUser )
			continue;

		if( !pShuffleUser->IsConnectState() )
			continue;

		if( pShuffleUser->GetPublicID().IsEmpty() )
			continue;

		if( !pShuffleUser->IsShuffleGlboalSearch() )
			continue;

		if( !g_ShuffleRoomManager.CheckKillDeathLevel( pUser->GetKillDeathLevel(), pShuffleUser->GetKillDeathLevel(), iGlbalSearchingTryCount ) )
		{
			LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : %s님(%d)과 %s님(%d) 매칭조건이 맞지않아 매칭실패(같은서버)", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetKillDeathLevel(),
									pShuffleUser->GetPublicID().c_str(), pShuffleUser->GetKillDeathLevel() );
			continue;
		}

		vUserGroup.push_back( pShuffleUser->GetUserIndex() );
	}
		
	for( uUserCopyNode_iter iter = m_uUserCopyNode.begin(); iter != m_uUserCopyNode.end(); ++iter )
	{
		UserCopyNode *pShuffleUser = iter->second;
		if( pShuffleUser->GetPublicID().IsEmpty() )
			continue;

		if( !pShuffleUser->IsShuffleGlboalSearch() )
			continue;

		if( !g_ShuffleRoomManager.CheckKillDeathLevel( pUser->GetKillDeathLevel(), pShuffleUser->GetKillDeathLevel(), iGlbalSearchingTryCount ) )
		{
			LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : %s님(%d)과 %s님(%d) 매칭조건이 맞지않아 매칭실패(타서버)", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetKillDeathLevel(),
									pShuffleUser->GetPublicID().c_str(), pShuffleUser->GetKillDeathLevel() );
			continue;
		}

		vUserGroup.push_back( pShuffleUser->GetUserIndex() );
	}
}

//pet
void SendPetHatchPacket( CQueryResultData *query_data, User *pUser, ioUserPet *pPet )
{
	DWORD  dwPetIdx = 0;
	int iPetRank = 0;
	int iPetCode = 0;
	int iPetLevel = 0;
	int iEggCode = 0;

	query_data->GetValue( iEggCode, sizeof( int ) ); //사용한 알 Code
	query_data->GetValue( dwPetIdx, sizeof(int) ); //새로운 펫 index
	query_data->GetValue( iPetCode, sizeof(int) ); //새로운 펫 Code
	query_data->GetValue( iPetRank, sizeof(char) ); //새로운 펫 Rank
	query_data->GetValue( iPetLevel, sizeof(int) );	//새로운 펫 레벨

	SP2Packet kPacket( STPK_PET_ADD  );
	PACKET_GUARD_VOID( kPacket.Write( PET_HATCH_OK ) );
	PACKET_GUARD_VOID( kPacket.Write( dwPetIdx ) );
	PACKET_GUARD_VOID( kPacket.Write( iPetCode ) );
	PACKET_GUARD_VOID( kPacket.Write( iPetRank ) );
	PACKET_GUARD_VOID( kPacket.Write( iPetLevel ) );
	pUser->SendMessage( kPacket );

	pPet->AddPet( dwPetIdx, iPetCode, iPetRank, iPetLevel );
	g_LogDBClient.OnInsertPetLog( pUser, dwPetIdx, iPetCode, iPetRank, 0, 0, iEggCode, LogDBClient::PDT_CREATE );
}

void SendPetCompoundPacket( CQueryResultData *query_data, User *pUser, ioUserPet *pPet )
{
	DWORD  dwPetIdx = 0;
	int iPetRank = 0;
	int iPetCode = 0;
	int iPetLevel = 0;

	DWORD dwTargetIdx = 0;
	DWORD dwVictimIndx = 0;

	query_data->GetValue( dwTargetIdx, sizeof( DWORD ) );
	query_data->GetValue( dwVictimIndx, sizeof( DWORD ) );
	query_data->GetValue( dwPetIdx, sizeof(int) ); //새로운 펫 index
	query_data->GetValue( iPetCode, sizeof(int) ); //새로운 펫 Code
	query_data->GetValue( iPetRank, sizeof(char) ); //새로운 펫 Rank
	query_data->GetValue( iPetLevel, sizeof(int) ); //새로운 펫 Level

	SP2Packet kPacket( STPK_PET_COMPOUND );
	PACKET_GUARD_VOID( kPacket.Write( PET_COMPOUND_OK ) );
	PACKET_GUARD_VOID( kPacket.Write( dwPetIdx ) );
	PACKET_GUARD_VOID( kPacket.Write( iPetCode ) );
	PACKET_GUARD_VOID( kPacket.Write( iPetRank ) );
	PACKET_GUARD_VOID( kPacket.Write( iPetLevel ) );
	PACKET_GUARD_VOID( kPacket.Write( dwTargetIdx ) );
	PACKET_GUARD_VOID( kPacket.Write( dwVictimIndx ) );
	pUser->SendMessage( kPacket );

	pPet->AddPet( dwPetIdx, iPetCode, iPetRank, iPetLevel );
	g_LogDBClient.OnInsertPetLog( pUser, dwPetIdx, iPetCode, iPetRank, 0, 0, 0, LogDBClient::PDT_CREATE );
}

void SendPetPresentRecvPacket( CQueryResultData *query_data, User *pUser, ioUserPet *pPet )
{
	DWORD  dwPetIdx = 0;
	int iPetRank = 0;
	int iPetCode = 0;
	int iPetLevel = 0;

	//선물함에서 삭제를 위해
	DWORD dwIndex = 0;
	DWORD dwSlotIndex = 0;

	query_data->GetValue( dwIndex, sizeof( DWORD ) );
	query_data->GetValue( dwSlotIndex, sizeof( DWORD ) );
	query_data->GetValue( dwPetIdx, sizeof(int) ); //새로운 펫 index
	query_data->GetValue( iPetCode, sizeof(int) ); //새로운 펫 Code
	query_data->GetValue( iPetRank, sizeof(char) ); //새로운 펫 Rank
	query_data->GetValue( iPetLevel, sizeof(int) ); //새로운 펫 Level

	//선물함 삭제
	SP2Packet kPacket( STPK_PRESENT_RECV );
	PACKET_GUARD_VOID( kPacket.Write( PRESENT_RECV_OK ) );
	PACKET_GUARD_VOID( kPacket.Write( dwIndex ) );
	PACKET_GUARD_VOID( kPacket.Write( dwSlotIndex ) );
	pUser->SendMessage( kPacket );


	//팻 추가 패킷
	SP2Packet kPacket2( STPK_PET_ADD );
	PACKET_GUARD_VOID( kPacket2.Write( PET_PRESENT_OK ) );
	PACKET_GUARD_VOID( kPacket2.Write( dwPetIdx ) );
	PACKET_GUARD_VOID( kPacket2.Write( iPetCode ) );
	PACKET_GUARD_VOID( kPacket2.Write( iPetRank ) );
	PACKET_GUARD_VOID( kPacket2.Write( iPetLevel ) );
	pUser->SendMessage( kPacket2 );

	//팻 추가
	pPet->AddPet( dwPetIdx, iPetCode, iPetRank, iPetLevel );
	g_LogDBClient.OnInsertPetLog( pUser, dwPetIdx, iPetCode, iPetRank, 0, 0, 0, LogDBClient::PDT_CREATE );

	//선물함 삭제
	pUser->GetUserPresent()->DeletePresentData( dwIndex, dwSlotIndex, ioUserPresent::DPT_RECV );
}

void UserNodeManager::InsertRespondPacketParsing( DWORD dwPacketID, CQueryResultData *query_data, User* pUser, ioUserPet *pPet )
{
	switch( dwPacketID )
	{
	case STPK_PET_ADD:
		{
			DWORD dwPacketType = 0;
			query_data->GetValue( dwPacketType, sizeof( DWORD ) );

			switch( dwPacketType )
			{
			case PET_HATCH_OK:
				SendPetHatchPacket( query_data, pUser, pPet );
				break;
			case PET_PRESENT_OK:
				SendPetPresentRecvPacket( query_data, pUser, pPet );
				break;
			}
		}
		break;
	case STPK_PET_COMPOUND:
		//펫 조합시 나온 새로운 팻 패킷 함수
		SendPetCompoundPacket( query_data, pUser, pPet );
		break;
	}
}

void UserNodeManager::ProcessInsertPetData( User *pUser, CQueryResultData &query_data )
{
	DWORD dwPacketID = 0;
	ioUserPet *pPet = pUser->GetUserPetItem();
		
	if( !pPet )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnResultInsertPetData : %s  pItem == NULL.", pUser->GetPublicID().c_str() );
		return;
	}

	//query_data.GetValue( szUserID, ID_NUM_PLUS_ONE );        //보낸 유저의 아이디. 
	query_data.GetValue( dwPacketID, sizeof( DWORD ) );

	//결과 응답패킷 파싱
	InsertRespondPacketParsing( dwPacketID, &query_data, pUser, pPet );
}

void UserNodeManager::SendDiffServerNewPetData( UserParent *pUserParent, DWORD dwUserIndex, CQueryResultData &query_data )
{
	UserCopyNode *pUser = static_cast<UserCopyNode*> ( pUserParent );

	SP2Packet kPacket( SSTPK_NEW_PET_DATA_INFO );
	PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );

	pUser->SendMessage( kPacket );
}

void UserNodeManager::OnResultInsertPetData( CQueryResultData &query_data )
{
	if( FAILED(query_data.GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertPetData Result FAILED! :%d",query_data.GetResultType());
		return;
	}

	//생성후 가져온 펫 인덱스를 찾아서 저장.
	DWORD dwUserIndex = 0;
	query_data.GetValue( dwUserIndex, sizeof(DWORD) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultInsertPetData USER FIND NOT!" );
		return;
	}

	//서버 이동
	if( !pUserParent->IsUserOriginal() ) 
	{
		//이동 처리 함수
		SendDiffServerNewPetData( pUserParent, dwUserIndex, query_data );
	}

	User *pUser = static_cast< User* > ( pUserParent );
	
	ProcessInsertPetData( pUser, query_data );
}

void UserNodeManager::OnResultUpdatePetData( CQueryResultData *query_data )
{
	if( FAILED(query_data->GetResultType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdatePetData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}


//유저 수 맵 초기화.
void UserNodeManager::InitChannelingUserCntMap()
{
	m_mChannelingCurUserCnt.clear();
	m_mChannelingMaxUserCnt.clear();

	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_WEMADEBUY , 0 ) );
	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_MGAME , 0 ) );
	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_DAUM , 0 ) );
	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_NAVER , 0 ) );
	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_TOONILAND , 0 ) );
	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_NEXON , 0 ) );
	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_STEAM , 0 ) );
	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_HANGAME , 0 ) );
	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_VALOFE , 0 ) );
#ifdef __OHTG_BILLING_TAIWAN_HAPPYTUK__
	m_mChannelingCurUserCnt.insert( std::make_pair( CNT_HAPPYTUK , 0 ) );
#endif //__OHTG_BILLING_TAIWAN_HAPPYTUK__

	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_WEMADEBUY , 0 ) );
	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_MGAME , 0 ) );
	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_DAUM , 0 ) );
	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_NAVER , 0 ) );
	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_TOONILAND , 0 ) );
	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_NEXON , 0 ) );
	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_STEAM , 0 ) );
	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_HANGAME , 0 ) );
	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_VALOFE , 0 ) );
#ifdef  __OHTG_BILLING_TAIWAN_HAPPYTUK__
	m_mChannelingMaxUserCnt.insert( std::make_pair( CNT_HAPPYTUK , 0 ) );
#endif //__OHTG_BILLING_TAIWAN_HAPPYTUK__
}

bool UserNodeManager::IsExistChannelingType( std::map< ChannelingType, int > &mMap, ChannelingType eChannelingType )
{
	std::map< ChannelingType, int >::iterator iter = mMap.find( eChannelingType );

	if( iter == mMap.end() )
		return false;

	return true;
}

//최대 유저수를 현재 유저수로 초기화.
void UserNodeManager::SetMaxUserCntToCurCnt()
{
	SetChannelingMaxUserCnt( CNT_WEMADEBUY, m_mChannelingCurUserCnt[ CNT_WEMADEBUY ] );
	SetChannelingMaxUserCnt( CNT_MGAME, m_mChannelingCurUserCnt[ CNT_MGAME ] );
	SetChannelingMaxUserCnt( CNT_DAUM, m_mChannelingCurUserCnt[ CNT_DAUM ] );
	SetChannelingMaxUserCnt( CNT_NAVER, m_mChannelingCurUserCnt[ CNT_NAVER ] );
	SetChannelingMaxUserCnt( CNT_TOONILAND, m_mChannelingCurUserCnt[ CNT_TOONILAND ] );
	SetChannelingMaxUserCnt( CNT_NEXON, m_mChannelingCurUserCnt[ CNT_NEXON ] );
	SetChannelingMaxUserCnt( CNT_STEAM, m_mChannelingCurUserCnt[ CNT_STEAM ] );
	SetChannelingMaxUserCnt( CNT_HANGAME, m_mChannelingCurUserCnt[ CNT_HANGAME ] );
	SetChannelingMaxUserCnt( CNT_VALOFE, m_mChannelingCurUserCnt[ CNT_VALOFE ] );
#ifdef  __OHTG_BILLING_TAIWAN_HAPPYTUK__
	SetChannelingMaxUserCnt( CNT_HAPPYTUK, m_mChannelingCurUserCnt[ CNT_HAPPYTUK ] );
#endif //__OHTG_BILLING_TAIWAN_HAPPYTUK__
}

void UserNodeManager::SetChannelingMaxUserCnt( ChannelingType eChannelingType, int iUserNum )
{
	if( !IsExistChannelingType( m_mChannelingMaxUserCnt, eChannelingType ) )
		return;

	m_mChannelingMaxUserCnt[ eChannelingType ] = iUserNum;
}

void UserNodeManager::CheckMaxUserCnt( ChannelingType eChannelingType )
{
	if( !IsExistChannelingType( m_mChannelingMaxUserCnt, eChannelingType ) || !IsExistChannelingType( m_mChannelingCurUserCnt, eChannelingType ) )
		return;

	if( m_mChannelingCurUserCnt[ eChannelingType ] > m_mChannelingMaxUserCnt[ eChannelingType ] )
	{
		SetChannelingMaxUserCnt( eChannelingType, m_mChannelingCurUserCnt[ eChannelingType ] );
	}
}

int UserNodeManager::GetChennelingMaxUserCnt( ChannelingType eChannelingType )
{
	if( !IsExistChannelingType( m_mChannelingMaxUserCnt, eChannelingType ) )
		return 0;

	return m_mChannelingMaxUserCnt[ eChannelingType ];
}

//현재 동접 수 
void UserNodeManager::SetChannelingCurUserCnt( ChannelingType eChannelingType, int iUserNum )
{
	if( !IsExistChannelingType( m_mChannelingCurUserCnt, eChannelingType ) )
		return;

	m_mChannelingCurUserCnt[ eChannelingType ] = iUserNum;
}


void UserNodeManager::IncreaseChannelingUserCnt( ChannelingType eChannelingType )
{
	if( !IsExistChannelingType( m_mChannelingCurUserCnt, eChannelingType ) )
		return;

	m_mChannelingCurUserCnt[ eChannelingType ]++;
}

void UserNodeManager::DecreaseChannelingUserCnt( ChannelingType eChannelingType )
{
	if( !IsExistChannelingType( m_mChannelingCurUserCnt, eChannelingType ) )
		return;

	m_mChannelingCurUserCnt[ eChannelingType ]--;
}

//코스튬
void UserNodeManager::OnResultLoginSelectCostumeData(CQueryResultData* query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectCostumeData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //주인 인덱스

	int iDBSelectCount = query_data->GetResultCount();

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectCostumeData USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllExtraItemData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioUserCostume* pCostumeInven = pUser->GetUserCostume();
	int iLastIndex = 0;
	if( pCostumeInven )
		pCostumeInven->DBtoData(query_data, iLastIndex);

	if( iDBSelectCount < DB_COSTUME_SELECT_COUNT )
	{
		//코스튬 정보 다 전송.
		pCostumeInven->SendAllCostumeInfo();
	}
	else
	{
		//더 남아 있으므로 호출
		g_DBClient.OnLoginSelectCostumeData(iLastIndex, pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
	}
}

void UserNodeManager::OnResultInsertCostumeData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertCostumeData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIdx		= 0;
	DWORD dwCode		= 0;
	BYTE byPeriodType	= 0;
	int iClassType		= 0; 
	SYSTEMTIME sysTime;
	int iIndex			= 0;
	BYTE byInsertType	= 0;
	int iValue1			= 0;
	int iValue2			= 0;
	int iValue3			= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwCode, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byPeriodType, sizeof(BYTE) ) );
	PACKET_GUARD_VOID( query_data->GetValue( (char*)&sysTime, sizeof(SYSTEMTIME) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byInsertType, sizeof(BYTE) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue1, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue2, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue3, sizeof(int) ) );

	PACKET_GUARD_VOID( query_data->GetValue( iIndex, sizeof(int) ) );

	UserParent *pGlobalUser = GetGlobalUserNode(dwUserIdx);
	if( !pGlobalUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][user] user is none" );
		return;
	}

	if( pGlobalUser->IsUserOriginal() )
	{
		User* pUser = (User*)pGlobalUser;
		int iYMD	= 0;
		int iHM		= 0;
		g_CostumeMgr.CalcLimitDateValue(sysTime, iYMD, iHM);

		//insert 처리
		pUser->AddCostumeItem(iIndex, dwCode, byPeriodType, sysTime);
		if( byInsertType == 1 )
		{
			//상품 구매처리
			SP2Packet kPacket( STPK_COSTUME_BUY );
			PACKET_GUARD_VOID(kPacket.Write(ITEM_BUY_OK));
			PACKET_GUARD_VOID(kPacket.Write(dwCode));
			PACKET_GUARD_VOID(kPacket.Write(iIndex));
			PACKET_GUARD_VOID(kPacket.Write(byPeriodType));
			PACKET_GUARD_VOID(kPacket.Write(iYMD));
			PACKET_GUARD_VOID(kPacket.Write(iHM));
			PACKET_GUARD_VOID(kPacket.Write(0));
			PACKET_GUARD_VOID(kPacket.Write(0));
			PACKET_GUARD_VOID(kPacket.Write(iValue1));			//상품 코드
			PACKET_GUARD_VOID(kPacket.Write(iValue2));			//상품 기간값
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetMoney()));
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetCash()));
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetChannelingCash()));

			pUser->SendMessage(kPacket);

			//코스튬 기간값은 시간.
			int iPeriod = iValue2 * 60;
			g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_ITEM_BUY, pUser, 0, PRESENT_COSTUME, 1, iValue1, iValue3, 1, iPeriod, NULL);
		}
		else if( byInsertType == 2 )
		{
			//선물함 recv 결과값 전송.

			SP2Packet kPacket( STPK_COSTUME_PRESENT );
			PACKET_GUARD_VOID(kPacket.Write(dwCode));
			PACKET_GUARD_VOID(kPacket.Write(iIndex));
			PACKET_GUARD_VOID(kPacket.Write(byPeriodType));
			PACKET_GUARD_VOID(kPacket.Write(iYMD));
			PACKET_GUARD_VOID(kPacket.Write(iHM));
			PACKET_GUARD_VOID(kPacket.Write(0));			//남성 치장
			PACKET_GUARD_VOID(kPacket.Write(0));			//여성
			pUser->SendMessage(kPacket);

			//g_LogDBClient.OnInsertCostumeInfo(pUser, dwCode, 1, LogDBClient::COT_PRESENT);
		}
	}
	else
	{
		//해당 서버로 코스튬 인벤토리 insert및 성공 패킷 전송.
		UserCopyNode* pCopyNodeUser = (UserCopyNode*)pGlobalUser;

		int iYMD = ( sysTime.wYear * 10000 ) + ( sysTime.wMonth * 100 ) + sysTime.wDay;
		int iHM = ( sysTime.wHour * 100 ) + sysTime.wMinute;

		SP2Packet kPacket( SSTPK_COSTUME_ADD );
		PACKET_GUARD_VOID(kPacket.Write(dwUserIdx));
		PACKET_GUARD_VOID(kPacket.Write(iIndex));
		PACKET_GUARD_VOID(kPacket.Write(dwCode));
		PACKET_GUARD_VOID(kPacket.Write(byPeriodType));
		PACKET_GUARD_VOID(kPacket.Write(iYMD));
		PACKET_GUARD_VOID(kPacket.Write(iHM));
		PACKET_GUARD_VOID(kPacket.Write(byInsertType));
		PACKET_GUARD_VOID(kPacket.Write(iValue1));
		PACKET_GUARD_VOID(kPacket.Write(iValue2));
		PACKET_GUARD_VOID(kPacket.Write(iValue3));
		pCopyNodeUser->SendMessage(kPacket);
	}
}

void UserNodeManager::OnResultUpdateCostumeData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateCostumeData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultDeleteCostumeData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultDeleteCostumeData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultLoginSelectAccessoryData(CQueryResultData* query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectAccessoryData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //주인 인덱스

	int iDBSelectCount = query_data->GetResultCount();

	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAccessoryData USER FIND NOT! :%d", dwUserIdx);
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAccessoryData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioUserAccessory* pAccessoryInven = pUser->GetUserAccessory();
	int iLastIndex = 0;
	if( pAccessoryInven )
		pAccessoryInven->DBtoData(query_data, iLastIndex);

	if( iDBSelectCount < DB_ACCESSORY_SELECT_COUNT )
	{
		//코스튬 정보 다 전송.
		pAccessoryInven->SendAllAccessoryInfo();
	}
	else
	{
		//더 남아 있으므로 호출
		g_DBClient.OnLoginSelectAccessoryData(iLastIndex, pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );
	}
}

void UserNodeManager::OnResultDeleteAccessoryData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultDeleteAccessoryData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultInsertAccessoryData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertAccessoryData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIdx		= 0;
	DWORD dwCode		= 0;
	BYTE byPeriodType	= 0;
	int iClassType		= 0; 
	SYSTEMTIME sysTime;
	int iIndex			= 0;
	BYTE byInsertType	= 0;
	int iValue1			= 0;
	int iValue2			= 0;
	int iValue3			= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwCode, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byPeriodType, sizeof(BYTE) ) );
	PACKET_GUARD_VOID( query_data->GetValue( (char*)&sysTime, sizeof(SYSTEMTIME) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byInsertType, sizeof(BYTE) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue1, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue2, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue3, sizeof(int) ) );

	PACKET_GUARD_VOID( query_data->GetValue( iIndex, sizeof(int) ) );

	UserParent *pGlobalUser = GetGlobalUserNode(dwUserIdx);
	if( !pGlobalUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][user] user is none" );
		return;
	}

	if( pGlobalUser->IsUserOriginal() )
	{
		User* pUser = (User*)pGlobalUser;
		int iYMD	= 0;
		int iHM		= 0;
		g_AccessoryMgr.CalcLimitDateValue(sysTime, iYMD, iHM);

		//insert 처리
		//pUser->AddAccessoryItem(iIndex, dwCode, byPeriodType, sysTime);
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] db result accessory add : [code:%d value:%d]", dwCode, iValue3);
		pUser->AddAccessoryItem(iIndex, dwCode, byPeriodType, iYMD, iHM, iValue3);
		if( byInsertType == 1 )
		{
			//상품 구매처리
			SP2Packet kPacket( STPK_ACCESSORY_BUY );
			PACKET_GUARD_VOID(kPacket.Write(ITEM_BUY_OK));
			PACKET_GUARD_VOID(kPacket.Write(dwCode));
			PACKET_GUARD_VOID(kPacket.Write(iIndex));
			PACKET_GUARD_VOID(kPacket.Write(byPeriodType));
			PACKET_GUARD_VOID(kPacket.Write(iYMD));
			PACKET_GUARD_VOID(kPacket.Write(iHM));
			PACKET_GUARD_VOID(kPacket.Write(iValue1));			//상품 코드
			PACKET_GUARD_VOID(kPacket.Write(iValue2));			//상품 기간값
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetMoney()));
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetCash()));
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetChannelingCash()));
			PACKET_GUARD_VOID(kPacket.Write(iValue3));

			pUser->SendMessage(kPacket);

			//코스튬 기간값은 시간.
			int iPeriod = iValue2 * 60;
			g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_ITEM_BUY, pUser, 0, PRESENT_ACCESSORY, 1, iValue1, iValue3, 1, iPeriod, NULL);
		}
		else if( byInsertType == 2 )
		{
			//선물함 recv 결과값 전송.

			SP2Packet kPacket( STPK_ACCESSORY_PRESENT );
			PACKET_GUARD_VOID(kPacket.Write(dwCode));
			PACKET_GUARD_VOID(kPacket.Write(iIndex));
			PACKET_GUARD_VOID(kPacket.Write(byPeriodType));
			PACKET_GUARD_VOID(kPacket.Write(iYMD));
			PACKET_GUARD_VOID(kPacket.Write(iHM));
			PACKET_GUARD_VOID(kPacket.Write(iValue3));
			pUser->SendMessage(kPacket);

			g_LogDBClient.OnInsertAccessoryInfo(pUser, dwCode, 1, LogDBClient::COT_PRESENT);
		}
	}
	else
	{
		//해당 서버로 코스튬 인벤토리 insert및 성공 패킷 전송.
		UserCopyNode* pCopyNodeUser = (UserCopyNode*)pGlobalUser;

		int iYMD = ( sysTime.wYear * 10000 ) + ( sysTime.wMonth * 100 ) + sysTime.wDay;
		int iHM = ( sysTime.wHour * 100 ) + sysTime.wMinute;

		SP2Packet kPacket( SSTPK_ACCESSORY_ADD );
		PACKET_GUARD_VOID(kPacket.Write(dwUserIdx));
		PACKET_GUARD_VOID(kPacket.Write(iIndex));
		PACKET_GUARD_VOID(kPacket.Write(dwCode));
		PACKET_GUARD_VOID(kPacket.Write(byPeriodType));
		PACKET_GUARD_VOID(kPacket.Write(iYMD));
		PACKET_GUARD_VOID(kPacket.Write(iHM));
		PACKET_GUARD_VOID(kPacket.Write(byInsertType));
		PACKET_GUARD_VOID(kPacket.Write(iValue1));
		PACKET_GUARD_VOID(kPacket.Write(iValue2));
		PACKET_GUARD_VOID(kPacket.Write(iValue3));
		pCopyNodeUser->SendMessage(kPacket);
	}
}

void UserNodeManager::OnResultGetMissionData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultGetMissionData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	// Return Data..
	DWORD  dwUserIdx = 0;
	//char   szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	//char   szUserID[ID_NUM_PLUS_ONE] = "";
	//PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	//PACKET_GUARD_VOID( query_data->GetValue( szUserID, ID_NUM_PLUS_ONE ) );       //보낸 유저의 아이디.
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIdx, sizeof(int) ) );		 //주인 인덱스

	//User *pUser = GetUserNodeByGUID( szUserGUID );
	User *pUser = GetUserNode(dwUserIdx);
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectCostumeData USER FIND NOT!");
		return;
	}
	if( pUser->GetUserIndex() != dwUserIdx )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultLoginSelectAllExtraItemData USER INDEX NOT! :%d - %d", pUser->GetUserIndex(), dwUserIdx );
		return;
	}

	ioMission* pUserMission = pUser->GetUserMission();
	if( pUserMission )
		pUserMission->DBtoData(query_data);

}

void UserNodeManager::OnResultUpdateMissionData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateMissionData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int iResult = 0;
	int iUserIndex = 0;

	PACKET_GUARD_VOID( query_data->GetValue( iUserIndex, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );

	if( iResult != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][usermission] user mission update failed : [%d:%d]",  iUserIndex, iResult);
	}
}

void UserNodeManager::OnResultInitMissionData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateMissionData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int iResult = 0;
	int iUserIndex = 0;

	PACKET_GUARD_VOID( query_data->GetValue( iUserIndex, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );

	if( iResult != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][usermission] user mission update failed : [%d:%d]",  iUserIndex, iResult);
	}
}

void UserNodeManager::OnResultGetRollBookData(CQueryResultData* query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultGetMissionData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(int) ) );		 //주인 인덱스

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent == NULL )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser = dynamic_cast<User*>( pUserParent );
		if( pUser )
		{
			ioUserRollBook* pUserRollBook = pUser->GetUserRollBook();
			if( pUserRollBook )
				pUserRollBook->DBToData(query_data);
		}
	}
}

void UserNodeManager::OnResultUpdateRollBookData(CQueryResultData* query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][rollbook] user roll book update fail : [%d]",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(int) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( pUserParent == NULL )
	{
		int iRollBookIndex	= 0;
		int iCount			= 0;
		int iResult			= 0;

		PACKET_GUARD_VOID( query_data->GetValue( iRollBookIndex, sizeof(int) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iCount, sizeof(int) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );
// 
// 		if( iResult != 0 )
// 			return;
// 
// 		if( 0 == iRollBookIndex )
// 		{
// 			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][rollbook] user does not exist and has invalid data : [%d]",dwUserIndex);
// 			return;
// 		}
// 
// 		RollBookManager::RewardInfo stReward;
// 		g_RollBookMgr.GetRewardInfo(iRollBookIndex, iCount, stReward);
// 		CTimeSpan cPresentGapTime( stReward.iPresentPeriod, 0, 0, 0 );
// 		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
// 
// 		g_DBClient.OnInsertPresentDataByUserIndex( 0, 0, dwUserIndex, dwUserIndex, stReward.iType, stReward.iValue1, stReward.iValue2, 0, 0, stReward.iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
// 		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[info][rollbook] user does not exist than send reward to present : [%d][%d][%d][%d]",dwUserIndex, stReward.iType, stReward.iValue1, stReward.iValue2 );
		
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][rollbook]user does not exist than don't send reward : [%d][%d][%d][%d]",dwUserIndex, iResult, iRollBookIndex, iCount);
		return;
	}

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser = dynamic_cast<User*>( pUserParent );
		if( pUser )
		{
			ioUserRollBook* pUserRollBook = pUser->GetUserRollBook();
			if( pUserRollBook )
				pUserRollBook->ResultSQLUpdateRollBook(query_data);
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][rollbook] user was server move than don't send reward : [%d]",dwUserIndex);
	}
}

void UserNodeManager::OnResultDeletePresent( CQueryResultData* query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][present] delete present fail : [%d]",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultGetGuildAttendaceMemberGet(CQueryResultData* query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][guildreward]Guild attendance data get fail : [%d]",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(int) ) );
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser = dynamic_cast<User*>( pUserParent );
		if( pUser )
		{
			ioUserGuild* pUserGuild = pUser->GetUserGuild();
			if( pUserGuild )
			{
				pUserGuild->SetGuildAttendanceData(query_data);
			}
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][guildreward] user was server move than don't send attendance info : [%d]",dwUserIndex);
	}
}

void UserNodeManager::OnResultInsertUserGuildAttendanceInfo(CQueryResultData* query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][guildreward]Guild attendance data get fail : [%d]",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;
	int iResult			= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );

	if( iResult != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][guildreward]User guild attendance fail : [%d] [%d]", dwUserIndex, iResult);
	}
}

void UserNodeManager::OnResultSelectGetSpentMoney( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGetSpentMoney Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue(dwUserIndex,sizeof(DWORD)) );
	User* pUser = GetUserNode( dwUserIndex );
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGetSpentMoney USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	DWORD dwReturn = 0;
	DWORD dwTotalCash = 0;
	DWORD dwMonthCash = 0;
	DWORD dwTotalPlayTime = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwReturn, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwTotalCash, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwMonthCash, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwTotalPlayTime, sizeof(DWORD) ) );

	pUser->SetUserSpentMoney( dwTotalCash, dwMonthCash, dwTotalPlayTime );
}

void UserNodeManager::OnResultSelectGetPopupIndex( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGetPopupIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue(dwUserIndex,sizeof(DWORD)) );
	User* pUser = GetUserNode( dwUserIndex );
	if(pUser == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGetPopupIndex USER FIND NOT! :%d", dwUserIndex );
		return;
	}

	static std::vector<int> m_vecPopupIndex;
	m_vecPopupIndex.clear();

	LOOP_GUARD();
	while( query_data->IsExist() )
	{
		int iPopupIndex = 0;
		PACKET_GUARD_BREAK( query_data->GetValue( iPopupIndex, sizeof(int) ) );
		m_vecPopupIndex.push_back( iPopupIndex );
	}
	LOOP_GUARD_CLEAR();

	pUser->SetUserPopupIndex( m_vecPopupIndex );
}

void UserNodeManager::OnResultSelectUserChannelingKeyValue(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGetPopupIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}


	char szUserGUID[USER_GUID_NUM_PLUS_ONE]="";
	char szPrivateID[ID_NUM_PLUS_ONE] = "";
	char szUserChannelingKey[CHANNELING_KEY_VALUE_PLUS_ONE] = "";
	int iChannelingType = 0;

	PACKET_GUARD_VOID( query_data->GetValue( szUserGUID, USER_GUID_NUM_PLUS_ONE ) );
	PACKET_GUARD_VOID( query_data->GetValue( szPrivateID,ID_NUM_PLUS_ONE) );
	PACKET_GUARD_VOID( query_data->GetValue( iChannelingType, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( szUserChannelingKey,CHANNELING_KEY_VALUE_PLUS_ONE) );

	User *pUser = GetUserNodeByGUID( szUserGUID );
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][usernode]user information is not find by guid : [%s] [%s]", szUserGUID, szPrivateID );
		return;
	}

	if( pUser->GetPrivateID() != szPrivateID )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][usernode]user information is not math. privateID : [%s] [%s]", szUserGUID, szPrivateID );
		return;
	}

	//채널링 별 필요 처리 ... 
	if( CNT_DAUM == iChannelingType )
	{
		SP2Packet kPacket(BSTPK_DAUM_SHUTDOWN_CHECK);
		PACKET_GUARD_VOID( kPacket.Write(szUserGUID) );
		PACKET_GUARD_VOID( kPacket.Write(szUserChannelingKey) );
		g_BillingRelayServer.SendMessage( kPacket );
	}
}

void UserNodeManager::TestEnterGuildRoom(DWORD dwUserIndex, DWORD dwGuildIndex, DWORD dwRoomIndex)
{
	Room* pRoom	= g_RoomNodeManager.GetRoomNode(dwRoomIndex);
	if( !pRoom )
		return;

	if( pRoom->GetPlazaModeType() != PT_GUILD )
		return;

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	SP2Packet kPacket(MSTPK_UPDATE_GUILD_ROOM_INDEX);
	PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
	PACKET_GUARD_VOID( kPacket.Write(dwRoomIndex) );
	g_MainServer.SendMessage(kPacket);

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser = dynamic_cast<User*>( pUserParent );
		if( pUser )
		{
			if( pUser->GetGuildIndex() != dwGuildIndex )
				return;

			/*SP2Packet kBlock(STPK_GUILD_BLOCK_INFOS);
			if( g_GuildRoomBlockMgr.FillBlockInfos(dwGuildIndex, kBlock) )
				pUser->SendMessage( kBlock );*/

			//유저 입장 처리.
			SP2Packet kPacket( STPK_JOIN_ROOM );   
			PACKET_GUARD_VOID( kPacket.Write( JOIN_ROOM_OK ) );
			PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeType() ) );
			PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeSubNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeMapNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( pRoom->GetRoomNumber() ) );
			PACKET_GUARD_VOID( kPacket.Write( (int)pRoom->GetPlazaModeType() ) );
	
			pUser->SendMessage( kPacket );
			pUser->EnterRoom( pRoom );
		}
	}
	else
	{
		UserCopyNode *pUser = (UserCopyNode*)pUserParent;

		pRoom->EnterReserveUser( pUser->GetUserIndex() );

		//길드 블럭 정보들 전송.
		/*SP2Packet kBlock(STPK_GUILD_BLOCK_INFOS);
		g_GuildRoomBlockMgr.FillBlockInfos(dwGuildIndex, kBlock);
		pUser->SendMessage( kBlock );*/

		//서버 이동
		SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(SS_MOVING_ROOM_JOIN) );
		PACKET_GUARD_VOID( kPacket.Write( SS_MOVING_ROOM_JOIN_PLAZA ) );
		PACKET_GUARD_VOID( kPacket.Write((int)pRoom->GetModeType()) );
		PACKET_GUARD_VOID( kPacket.Write(pRoom->GetRoomIndex()) );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetUserIndex()) );
		PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeSubNum() ) );
		PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeMapNum() ) );
		PACKET_GUARD_VOID( kPacket.Write( (int)pRoom->GetPlazaModeType() ) );
		PACKET_GUARD_VOID( kPacket.Write( pRoom->GetRoomNumber() ) );

		pUser->SendMessage( kPacket );
	}
}

void UserNodeManager::OnResultSelectGuildBlockInfos(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGetPopupIndex Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;
	DWORD dwGuildIndex	= 0;
	DWORD dwRoomIndex	= 0;
	int iPage			= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwGuildIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwRoomIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iPage, sizeof(int) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	Room* pRoom	= g_RoomNodeManager.GetRoomNode(dwRoomIndex);
	if( !pRoom )
	{
		SP2Packet kPacket( STPK_JOIN_ROOM );   
		PACKET_GUARD_VOID( kPacket.Write( JOIN_ROOM_NOT ) );
		if( pUserParent->IsUserOriginal() )
		{
			User* pUser = dynamic_cast<User*>( pUserParent );
			pUser->SendMessage( kPacket );
		}
		else
		{
			UserCopyNode *pUser = (UserCopyNode*)pUserParent;
			pUser->RelayPacket( kPacket );
		}

		SP2Packet kMPacket(MSTPK_DELETE_GUILD_ROOM_INFO);
		PACKET_GUARD_VOID( kMPacket.Write(dwGuildIndex) );
		PACKET_GUARD_VOID( kMPacket.Write(dwRoomIndex) );
		g_MainServer.SendMessage(kMPacket);
		return;
	}

	if( (pRoom->GetRoomIndex() != dwRoomIndex) || (pRoom->GetPlazaModeType() != PT_GUILD) )
	{
		SP2Packet kPacket( STPK_JOIN_ROOM );   
		PACKET_GUARD_VOID( kPacket.Write( JOIN_ROOM_NOT ) );
		if( pUserParent->IsUserOriginal() )
		{
			User* pUser = dynamic_cast<User*>( pUserParent );
			pUser->SendMessage( kPacket );
		}
		else
		{
			UserCopyNode *pUser = (UserCopyNode*)pUserParent;
			pUser->RelayPacket( kPacket );
		}

		SP2Packet kMPacket(MSTPK_DELETE_GUILD_ROOM_INFO);
		PACKET_GUARD_VOID( kMPacket.Write(dwGuildIndex) );
		PACKET_GUARD_VOID( kMPacket.Write(dwRoomIndex) );
		g_MainServer.SendMessage(kMPacket);
		return;
	}

	//길드 본부 정보 update
	int iSelectCount	= query_data->GetResultCount();
	
	g_GuildRoomBlockMgr.CreateGuildRoomInfos(dwGuildIndex, dwRoomIndex, query_data);
	
	if( iSelectCount == DB_BUILT_BLOCK_ITEM_SELECT_COUNT )
	{
		g_DBClient.OnSelectGuildBlocksInfos(dwUserIndex, dwGuildIndex, dwRoomIndex, iPage+1);
		return;
	}
	
	SP2Packet kPacket(MSTPK_UPDATE_GUILD_ROOM_INDEX);
	PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
	PACKET_GUARD_VOID( kPacket.Write(dwRoomIndex) );
	g_MainServer.SendMessage(kPacket);

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[info][guildroom]Create guild room : [%d] [%d]", dwGuildIndex, dwRoomIndex);

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser = dynamic_cast<User*>( pUserParent );
		if( pUser )
		{
			if( pUser->GetGuildIndex() != dwGuildIndex )
				return;

			/*SP2Packet kBlock(STPK_GUILD_BLOCK_INFOS);
			if( g_GuildRoomBlockMgr.FillBlockInfos(dwGuildIndex, kBlock) )
				pUser->SendMessage( kBlock );*/

			//유저 입장 처리.
			SP2Packet kPacket( STPK_JOIN_ROOM );   
			PACKET_GUARD_VOID( kPacket.Write( JOIN_ROOM_OK ) );
			PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeType() ) );
			PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeSubNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeMapNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( pRoom->GetRoomNumber() ) );
			PACKET_GUARD_VOID( kPacket.Write( (int)pRoom->GetPlazaModeType() ) );
	
			pUser->SendMessage( kPacket );
			pUser->EnterRoom( pRoom );
		}
	}
	else
	{
		UserCopyNode *pUser = (UserCopyNode*)pUserParent;

		pRoom->EnterReserveUser( pUser->GetUserIndex() );

		//길드 블럭 정보들 전송.
		/*SP2Packet kBlock(STPK_GUILD_BLOCK_INFOS);
		g_GuildRoomBlockMgr.FillBlockInfos(dwGuildIndex, kBlock);
		pUser->SendMessage( kBlock );*/

		//서버 이동
		SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(SS_MOVING_ROOM_JOIN) );
		PACKET_GUARD_VOID( kPacket.Write( SS_MOVING_ROOM_JOIN_PLAZA ) );
		PACKET_GUARD_VOID( kPacket.Write((int)pRoom->GetModeType()) );
		PACKET_GUARD_VOID( kPacket.Write(pRoom->GetRoomIndex()) );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetUserIndex()) );
		PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeSubNum() ) );
		PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeMapNum() ) );
		PACKET_GUARD_VOID( kPacket.Write( (int)pRoom->GetPlazaModeType() ) );
		PACKET_GUARD_VOID( kPacket.Write( pRoom->GetRoomNumber() ) );

		pUser->SendMessage( kPacket );
	}
}

void UserNodeManager::OnResultGuildBlockRetrieveORDelete(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultGuildBlockRetrieveORDelete Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwGuildIndex		= 0;
	DWORD dwRoomIndex		= 0;
	__int64	i64ItemIndex	= 0;
	BYTE byState			= 0;
	char szPublicID[ID_NUM_PLUS_ONE] = "";

	PACKET_GUARD_VOID( query_data->GetValue( dwGuildIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwRoomIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( i64ItemIndex, sizeof(__int64) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byState, sizeof(BYTE) ) );
	PACKET_GUARD_VOID( query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE ) );

	GuildRoomInfos* pInfo = g_GuildRoomBlockMgr.GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return;

	if( dwRoomIndex != pInfo->GetGuildRoomIndex() )
		return;

	Room* pRoom	= g_RoomNodeManager.GetRoomNode(dwRoomIndex);
	if( !pRoom )
		return;

	switch( byState )
	{
	case GBT_RETRIEVE:
		{
			g_GuildRoomBlockMgr.DeleteGuildItem(dwGuildIndex, i64ItemIndex);

			SP2Packet kPacket( STPK_RETRIEVE_BLOCK );
			PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_GUILD) );
			PACKET_GUARD_VOID( kPacket.Write((BYTE)RETRIEVE_BLOCK_SUCCESS) );
			PACKET_GUARD_VOID( kPacket.Write(szPublicID) );
			PACKET_GUARD_VOID( kPacket.Write(i64ItemIndex) );

			pRoom->RoomSendPacketTcp(kPacket);
		}
		break;

	case GBT_DELETE:

		break;

	default:
		g_GuildRoomBlockMgr.DeleteGuildItem(dwGuildIndex, i64ItemIndex);
	}
}

void UserNodeManager::OnResultGuildBlockConstructORMove(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultGuildBlockConstructORMove Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwGuildIndex		= 0;
	DWORD dwRoomIndex		= 0;
	BYTE byState			= 0;
	__int64	iItemIndex		= 0;
	DWORD dwItemCode		= 0;
	int iXZIndex			= 0;
	int iY					= 0;
	BYTE byDirection		= 0;
	char szPublicID[ID_NUM_PLUS_ONE] = "";

	PACKET_GUARD_VOID( query_data->GetValue( dwGuildIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwRoomIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byState, sizeof(BYTE) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iItemIndex, sizeof(__int64) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwItemCode, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iXZIndex, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iY, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byDirection, sizeof(BYTE) ) );
	PACKET_GUARD_VOID( query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE ) );

	GuildRoomInfos* pInfo = g_GuildRoomBlockMgr.GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return;

	if( dwRoomIndex != pInfo->GetGuildRoomIndex() )
		return;

	Room* pRoom	= g_RoomNodeManager.GetRoomNode(dwRoomIndex);
	if( !pRoom )
		return;

	switch( byState )
	{
	case GBT_CONSTRUCT:
		{
			PACKET_GUARD_VOID( query_data->GetValue( iItemIndex, sizeof(__int64) ) );

			if( 0 == iItemIndex )
			{
				User* pUser = GetUserNodeByPublicID(szPublicID);
				if( !pUser )
					return;

				SP2Packet kPacket( STPK_CONSTRUCT_BLOCK );
				PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_GUILD) );
				PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_BLOCK_SHORTAGE) );
				pUser->SendMessage( kPacket );
				return;
			}

			if( !g_GuildRoomBlockMgr.AddGuildItem(dwGuildIndex, iItemIndex, dwItemCode, iXZIndex, iY, byDirection) )
			{
				User* pUser = GetUserNodeByPublicID(szPublicID);
				if( !pUser )
					return;

				SP2Packet kPacket(STPK_CONSTRUCT_BLOCK);
				PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_GUILD) );
				PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_BLOCK_EXCEPTION) );
				pUser->SendMessage( kPacket );
				return;
			}

			SP2Packet kPacket(STPK_CONSTRUCT_BLOCK);
			PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_GUILD) );
			PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_BLOCK_SUCCESS) );
			PACKET_GUARD_VOID( kPacket.Write(szPublicID) );
			PACKET_GUARD_VOID( kPacket.Write(iItemIndex) );
			PACKET_GUARD_VOID( kPacket.Write(dwItemCode) );
			PACKET_GUARD_VOID( kPacket.Write(iXZIndex) );
			PACKET_GUARD_VOID( kPacket.Write(iY) );
			PACKET_GUARD_VOID( kPacket.Write(byDirection) );
			pRoom->RoomSendPacketTcp(kPacket);
		}
		break;

	case GBT_MOVE:
		{
			
		}
		break;
	}
}

void UserNodeManager::SendGuildInvenInfoWithSQLResult(const DWORD dwUserIndex, const DWORD dwGuildIndex, const int iRequestType)
{
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User *pUser = static_cast< User* >( pUserParent );

		//인벤 정보 전송.
		if( pUser->GetGuildIndex() == dwGuildIndex )
		{
			//정보 send
			SP2Packet kPacket(STPK_GUILD_INVEN_INFO );
			PACKET_GUARD_VOID( kPacket.Write((BYTE)REQUEST_INVEN_SUCCESS) );
			g_ServerGuildInvenMgr.FillGuildInvenData(dwGuildIndex, kPacket);
			pUser->SendMessage(kPacket);

			if( CONSTRUCT_MODE == iRequestType )
			{
				//방에 대한 검증
				Room* pRoom	= pUser->GetMyRoom();
				GuildRoomInfos* pInfo	= g_GuildRoomBlockMgr.GetGuildRoomInfos(dwGuildIndex);
				if( !pRoom || !pInfo)
				{
					SP2Packet kPacket(STPK_CONSTRUCT_MODE);
					PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_GUILD) );
					PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_MODE_EXCEPTION) );
					pUser->SendMessage(kPacket);
					return;
				}

				if( pRoom->GetRoomIndex() != pInfo->GetGuildRoomIndex() )
				{
					SP2Packet kPacket(STPK_CONSTRUCT_MODE);
					PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_GUILD) );
					PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_MODE_EXCEPTION) );
					pUser->SendMessage(kPacket);
					return;
				}

				//건설 모드 OK 패킷 전송.
				SP2Packet kPacket(STPK_CONSTRUCT_MODE);
				PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_GUILD) );
				PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_MODE_CHANGE_SUCCESS) );
				PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_ON) );
				PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
				pRoom->RoomSendPacketTcp(kPacket);
			}
		}
		else
		{
			SP2Packet kPacket(STPK_GUILD_INVEN_INFO);
			PACKET_GUARD_VOID( kPacket.Write((BYTE)REQUEST_INVEN_EXCEPTION) );
			pUser->SendMessage(kPacket);
			return;
		}
	}
}

void UserNodeManager::OnResultSelectGuildInvenVersion(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildInvenVersion Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;
	DWORD dwGuildIndex	= 0;
	int iRequestType	= 0;
	__int64 i64Ver		= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwGuildIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iRequestType, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( i64Ver, sizeof(__int64) ) );

	if( g_ServerGuildInvenMgr.IsPrevData(dwGuildIndex, i64Ver) )
	{
		//인벤 정보 요청
		g_DBClient.OnSelectGuildInvenInfo(dwUserIndex, dwGuildIndex, iRequestType, i64Ver);
	}
	else
	{
		SendGuildInvenInfoWithSQLResult(dwUserIndex, dwGuildIndex, iRequestType);
	}
}

void UserNodeManager::OnResultSelectGuildInvenInfo(CQueryResultData *query_data)
{
	//인벤 정보 처리. 
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectGuildInvenInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;
	DWORD dwGuildIndex	= 0;
	int iType			= 0;
	__int64 iRequestVer	= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwGuildIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iType, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iRequestVer, sizeof(__int64) ) );

	if( g_ServerGuildInvenMgr.IsPrevData(dwGuildIndex, iRequestVer) )
	{
		g_ServerGuildInvenMgr.InsertInvenData(dwGuildIndex, query_data);
		g_ServerGuildInvenMgr.UpdateInvenVer(dwGuildIndex, iRequestVer);
	}

	SendGuildInvenInfoWithSQLResult(dwUserIndex, dwGuildIndex, iType);
}

void UserNodeManager::OnResultAddGuildBlockItem(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultAddGuildBlockItem Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	//DWORD dwGuildIndex		= 0;
	//DWORD dwItemCode		= 0;
	//int iAddType			= 0;

	//PACKET_GUARD_VOID( query_data->GetValue( dwGuildIndex, sizeof(DWORD) ) );
	//PACKET_GUARD_VOID( query_data->GetValue( dwItemCode, sizeof(DWORD) ) );
	//PACKET_GUARD_VOID( query_data->GetValue( iAddType, sizeof(int) ) );

	//if( GBA_GUILD_CREATE == iAddType )
	//{
	//	//기본 default setting으루 설치
	//	int iXZ	= 0;
	//	int iY	= 0;
	//	int iDirection	= 0;

	//	if( g_GuildRoomBlockMgr.IsValidDefaultItem(dwItemCode) )
	//	{
	//		g_GuildRoomBlockMgr.GetDefaultItemCoordinate(dwItemCode, iXZ, iY, iDirection);

	//		//위치 검증.
	//		/*if( !g_GuildRoomBlockMgr.IsRightPosition(dwGuildIndex, dwItemCode, iXZ, iY, iDirection) )
	//			return;*/

	//		//DB에 상태값 변경 요청.
	//		g_DBClient.OnConstructOrMoveBlock(dwGuildIndex, 0, "", 0, dwItemCode, GBT_DEFAULT_CONSTRUCT, iXZ, iY, iDirection);
	//	}
	//}
}

void UserNodeManager::OnResultDefaultConstructGuildBlock(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultDefaultConstructGuildBlock Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::SendPersonalHQInviteList( User *pUser, int iCurPage, int iMaxCount, int iRoomLevel )
{
	if( pUser == NULL ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	static vSortUser vSortList;
	vSortList.clear();

	// 동일 서버
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pTargetUser = iter->second;
		if( pTargetUser == pUser ) continue;
		if( !pTargetUser->IsConnectState() ) continue;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->GetModeType() != MT_NONE && pTargetUser->GetModeType() != MT_TRAINING && pTargetUser->GetModeType() != MT_HEADQUARTERS && pTargetUser->GetModeType() != MT_HOUSE ) continue;
		if( pTargetUser->GetMyRoom() && pTargetUser->GetMyRoom() == pUser->GetMyRoom() ) continue;
		if( pTargetUser->GetUserPos() == UP_BATTLE_ROOM || pTargetUser->GetUserPos() == UP_LADDER_TEAM ) continue;

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pTargetUser->GetKillDeathLevel() - iRoomLevel );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
		}        

		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	// 타서버
	uUserCopyNode_iter iterCopy,iEndCopy;
	iEndCopy = m_uUserCopyNode.end();
	for(iterCopy=m_uUserCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		UserCopyNode *pTargetUser = iterCopy->second;
		if( pTargetUser->GetPublicID().IsEmpty() ) continue;
		if( pTargetUser->GetModeType() != MT_NONE && pTargetUser->GetModeType() != MT_TRAINING && pTargetUser->GetModeType() != MT_HEADQUARTERS && pTargetUser->GetModeType() != MT_HOUSE ) continue;
		if( pTargetUser->GetUserPos() == UP_BATTLE_ROOM || pTargetUser->GetUserPos() == UP_LADDER_TEAM ) continue;

		SortUser kSortUser;
		kSortUser.m_pUser		= (UserParent*)pTargetUser;
		int iLevelGap			= abs( pTargetUser->GetKillDeathLevel() - iRoomLevel );	
		kSortUser.m_iSortPoint	= FRIEND_USER_POINT;
		if( IsDeveloper( pUser->GetPublicID().c_str() ) || !IsDeveloper( pTargetUser->GetPublicID().c_str() ) )
		{
			if( pUser->IsFriend( pTargetUser->GetPublicID() ) )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;
			else if( pUser->IsGuild() && pUser->GetGuildIndex() == pTargetUser->GetGuildIndex() )
				kSortUser.m_iSortPoint -= FRIEND_USER_POINT;	
		}
		
		kSortUser.m_iSortPoint += iLevelGap;	
		vSortList.push_back( kSortUser );
	}

	int iMaxList = vSortList.size();
	if( iMaxList == 0 )
	{
		// 로비에 유저가 없으면 없다고 알려줘야 클라이언트 리스트에 남지 않는다.
		SP2Packet kPacket( STPK_PERSONAL_HQ_INVITE_LIST );
		kPacket << 0 << 0;
		pUser->SendMessage( kPacket );
		return;
	}
	std::sort( vSortList.begin(), vSortList.end(), UserSort() );

	int iStartPos = iCurPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

	SP2Packet kPacket( STPK_PERSONAL_HQ_INVITE_LIST );
	kPacket << iMaxList << iCurSize;
	for(int i = iStartPos; i < iStartPos + iCurSize;i++)
	{
		if( vSortList[i].m_pUser )
		{
			kPacket << vSortList[i].m_pUser->GetPublicID() << vSortList[i].m_pUser->GetGradeLevel() 
				<< vSortList[i].m_pUser->GetGuildIndex() << vSortList[i].m_pUser->GetGuildMark();
		}		
		else    //예외
		{
			kPacket << "" << 0 << 0 << 0;
		}
	}	
	pUser->SendMessage( kPacket );
	vSortList.clear();
}

void UserNodeManager::OnResultPersonalHQConstruct(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultGuildBlockConstructORMove Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex		= 0;
	DWORD dwRoomIndex		= 0;
	DWORD dwItemCode		= 0;
	__int64	iItemIndex		= 0;
	int iXZIndex			= 0;
	int iY					= 0;
	BYTE byDirection		= 0;
	char szPublicID[ID_NUM_PLUS_ONE] = "";

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwRoomIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwItemCode, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iXZIndex, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iY, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byDirection, sizeof(BYTE) ) );
	PACKET_GUARD_VOID( query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE ) );

	PersonalHomeInfo* pInfo = g_PersonalRoomBlockMgr.GetPersonalRoomInfo(dwUserIndex);
	if( !pInfo )
		return;

	if( dwRoomIndex != pInfo->GetRoomIndex() )
		return;

	Room* pRoom	= g_RoomNodeManager.GetRoomNode(dwRoomIndex);
	User* pUser	= GetUserNode(dwUserIndex);
	if( !pRoom || !pUser )
		return;

	PACKET_GUARD_VOID( query_data->GetValue( iItemIndex, sizeof(__int64) ) );

	if( 0 == iItemIndex )
	{
		User* pUser = GetUserNodeByPublicID(szPublicID);
		if( !pUser )
			return;

		SP2Packet kPacket( STPK_CONSTRUCT_BLOCK );
		PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_PERSONAL) );
		PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_BLOCK_SHORTAGE) );
		pUser->SendMessage( kPacket );
		return;
	}

	if( !g_PersonalRoomBlockMgr.AddBlockItem(dwUserIndex, iItemIndex, dwItemCode, iXZIndex, iY, byDirection) )
	{
		User* pUser = GetUserNodeByPublicID(szPublicID);
		if( !pUser )
			return;

		SP2Packet kPacket(STPK_CONSTRUCT_BLOCK);
		PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_PERSONAL) );
		PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_BLOCK_EXCEPTION) );
		pUser->SendMessage( kPacket );
		return;
	}

	SP2Packet kPacket(STPK_CONSTRUCT_BLOCK);
	PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_PERSONAL) );
	PACKET_GUARD_VOID( kPacket.Write((BYTE)CONSTRUCT_BLOCK_SUCCESS) );
	PACKET_GUARD_VOID( kPacket.Write(szPublicID) );
	PACKET_GUARD_VOID( kPacket.Write(iItemIndex) );
	PACKET_GUARD_VOID( kPacket.Write(dwItemCode) );
	PACKET_GUARD_VOID( kPacket.Write(iXZIndex) );
	PACKET_GUARD_VOID( kPacket.Write(iY) );
	PACKET_GUARD_VOID( kPacket.Write(byDirection) );
	pRoom->RoomSendPacketTcp(kPacket);

	pUser->DecreasePersonalInvenItem(dwItemCode, 1);
}

void UserNodeManager::OnResultPersonalHQRetrieve(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultPersonalHQRetrieve Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex		= 0;
	DWORD dwRoomIndex		= 0;
	__int64	i64ItemIndex	= 0;
	DWORD dwItemCode		= 0;
	BYTE byState			= 0;
	char szPublicID[ID_NUM_PLUS_ONE] = "";

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwRoomIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( i64ItemIndex, sizeof(__int64) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwItemCode, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byState, sizeof(BYTE) ) );
	PACKET_GUARD_VOID( query_data->GetValue( szPublicID, ID_NUM_PLUS_ONE ) );

	PersonalHomeInfo* pInfo = g_PersonalRoomBlockMgr.GetPersonalRoomInfo(dwUserIndex);
	if( !pInfo )
		return;

	if( dwRoomIndex != pInfo->GetRoomIndex() )
		return;

	Room* pRoom	= g_RoomNodeManager.GetRoomNode(dwRoomIndex);
	User* pUser	= GetUserNode(dwUserIndex);
	if( !pRoom || !pUser )
		return;

	switch( byState )
	{
	case GBT_RETRIEVE:
		{
			g_PersonalRoomBlockMgr.DeleteBlockItem(dwUserIndex, i64ItemIndex);

			SP2Packet kPacket( STPK_RETRIEVE_BLOCK );
			PACKET_GUARD_VOID( kPacket.Write((BYTE)BMT_PERSONAL) );
			PACKET_GUARD_VOID( kPacket.Write((BYTE)RETRIEVE_BLOCK_SUCCESS) );
			PACKET_GUARD_VOID( kPacket.Write(szPublicID) );
			PACKET_GUARD_VOID( kPacket.Write(i64ItemIndex) );

			pRoom->RoomSendPacketTcp(kPacket);

			//인벤토리 갯수 +;
			pUser->AddPersonalInvenItem(dwItemCode, 1);
		}
		break;

	default:
		{
			g_PersonalRoomBlockMgr.DeleteBlockItem(dwUserIndex, i64ItemIndex);

			//인벤토리 갯수 +;
			pUser->AddPersonalInvenItem(dwItemCode, 1);
		}
	}
}

void UserNodeManager::OnResultPersonalHQIsExist(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultPersonalHQIsExist Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;
	DWORD dwAgentID		= 0;
	DWORD dwThreadID	= 0;

	int iType	= 0, iValue1	= 0, iValue2	= 0;
	int iExist	= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwAgentID, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwThreadID, sizeof(DWORD) ) );

	PACKET_GUARD_VOID( query_data->GetValue( iType, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue1, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue2, sizeof(int) ) );

	PACKET_GUARD_VOID( query_data->GetValue( iExist, sizeof(int) ) );

	if( 0 == iExist )
	{
		//default 설치.
		g_PersonalRoomBlockMgr.ConstructDefaultBlock(dwUserIndex, dwAgentID, dwThreadID);
	}
	
	UserParent* pUserParent	= GetGlobalUserNode(dwUserIndex);

	if( pUserParent )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_VOID( kReturn.Write( ETCITEM_USE_OK ) );
		PACKET_GUARD_VOID( kReturn.Write( iType ) );
		PACKET_GUARD_VOID( kReturn.Write( iValue1 ) );
		PACKET_GUARD_VOID( kReturn.Write( iValue2 ) );

		pUserParent->RelayPacket( kReturn );
	}
	
}

void UserNodeManager::OnResultLoginSelectPersonalHQInvenData(CQueryResultData *query_data)
{
	//인벤 정보 처리. 
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLoginSelectPersonalHQInvenData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( !pUser )
		return;

	PersonalHQInven *pInfo = pUser->GetUserPersonalHQInven();
	if( pInfo )
		pInfo->DBtoData( query_data );	
}

void UserNodeManager::OnResultPersonalHQBlocksInfo(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultPersonalHQBlocksInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;
	DWORD dwRoomIndex	= 0;
	int iPage			= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iPage, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwRoomIndex, sizeof(int) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( !pUserParent->IsUserOriginal() )
	{
		SP2Packet kPacket( STPK_JOIN_ROOM );   
		PACKET_GUARD_VOID( kPacket.Write( JOIN_ROOM_NOT ) );
		pUserParent->RelayPacket( kPacket );
		return;
	}

	User* pUser	= static_cast<User*>(pUserParent);
	Room* pRoom	= g_RoomNodeManager.GetRoomNode(dwRoomIndex);

	if( !pRoom )
	{
		SP2Packet kPacket( STPK_JOIN_ROOM );   
		PACKET_GUARD_VOID( kPacket.Write( JOIN_ROOM_NOT ) );
		pUser->SendMessage( kPacket );
		return;
	}
	
	////길드 본부 정보 update
	int iSelectCount	= query_data->GetResultCount();
	
	g_PersonalRoomBlockMgr.CreatePersonalRoomInfo(pRoom->GetRoomIndex(), dwUserIndex, query_data);
	
	if( iSelectCount == DB_BUILT_BLOCK_ITEM_SELECT_COUNT )
	{
		g_DBClient.OnSelectPersonalBlocksInfo(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dwRoomIndex, pUser->GetUserIndex(), iPage+1);
		return;
	}
	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[info][personalroom]Create personal room : [%d] [%d]", pUser->GetUserIndex(), pRoom->GetRoomIndex());

	//유저 입장 처리.
	SP2Packet kPacket( STPK_JOIN_ROOM );   
	PACKET_GUARD_VOID( kPacket.Write( JOIN_ROOM_OK ) );
	PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeType() ) );
	PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeSubNum() ) );
	PACKET_GUARD_VOID( kPacket.Write( pRoom->GetModeMapNum() ) );
	PACKET_GUARD_VOID( kPacket.Write( pRoom->GetRoomNumber() ) );
	PACKET_GUARD_VOID( kPacket.Write( (int)pRoom->GetPlazaModeType() ) );
	
	pUser->SendMessage( kPacket );
	pUser->EnterRoom( pRoom );
}

void UserNodeManager::OnResultPersonalHQBlockAdd(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultPersonalHQBlockAdd Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;
	DWORD dwItemCode	= 0;
	int	  iCount		= 0;
	BOOL bEnd			= FALSE;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwItemCode, sizeof(dwItemCode) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iCount, sizeof(iCount) ) );
	PACKET_GUARD_VOID( query_data->GetValue( bEnd, sizeof(bEnd) ) );

	if( !bEnd )
		return;

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);

		pUser->AddPersonalInvenItem(dwItemCode, iCount);
	}
	else
	{
		UserCopyNode *pUser = static_cast<UserCopyNode*>(pUserParent);

		SP2Packet kPacket(SSTPK_PERSONAL_HQ_ADD_BLOCK);
		PACKET_GUARD_VOID( kPacket.Write(dwUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write(dwItemCode) );
		PACKET_GUARD_VOID( kPacket.Write(iCount) );
		pUser->SendMessage(kPacket);
	}
}

void UserNodeManager::OnResultSelectTimeCashTable(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTimeCashTable Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);

		pUser->LoginSelectCashTable(query_data);
	}
}

void UserNodeManager::OnResultUpdateTimeCashTable(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateTimeCashTable Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	//Result
	int iResultType	= 0;
	DWORD dwCode	= 0;
	DBTIMESTAMP UpdateDate;

	char szGUID[USER_GUID_NUM_PLUS_ONE]="";

	PACKET_GUARD_VOID( query_data->GetValue( szGUID, USER_GUID_NUM_PLUS_ONE ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwCode, sizeof(dwCode) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResultType, sizeof(iResultType) ) );
	PACKET_GUARD_VOID( query_data->GetValue( (char*)&UpdateDate, sizeof(DBTIMESTAMP) ) );

	DWORD dwReceiveDate	= Help::ConvertDBTIMESTAMPToDWORD(UpdateDate);
	ioHashString szBillingGUID	= szGUID;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);

		pUser->UpdateCashTable(dwCode, iResultType, dwReceiveDate, szBillingGUID);
	}
	else
	{
		UserCopyNode *pUser = static_cast<UserCopyNode*>(pUserParent);

		SP2Packet kPacket(SSTPK_UPDATE_TIME_CASH);
		PACKET_GUARD_VOID( kPacket.Write(dwUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write(dwCode) );
		PACKET_GUARD_VOID( kPacket.Write(iResultType) );
		PACKET_GUARD_VOID( kPacket.Write(dwReceiveDate) );
		PACKET_GUARD_VOID( kPacket.Write(szBillingGUID) );
		pUser->SendMessage(kPacket);
	}
}

void UserNodeManager::OnResultInsertTimeCashTable(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertTimeCashTable Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);

		pUser->InsertCashTable(query_data);
	}
}

void UserNodeManager::OnResultUpdateStatus(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertOrUpdate Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);

		//Update Or Insert
		pUser->UpdateUserTitleStatus();
	}
}

void UserNodeManager::OnResultInsertOrUpdate(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertOrUpdate Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;
	DWORD dwCode		= 0;
	__int64 iValue		= 0;
	int iLevel			= 0;
	BYTE byPremium		= 0;
	BYTE byEquip		= 0;
	BYTE byStatus		= 0;
	BYTE byActionType	= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byActionType, sizeof(byActionType) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwCode, sizeof(dwCode) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue, sizeof(iValue) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iLevel, sizeof(iLevel) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byPremium, sizeof(byPremium) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byEquip, sizeof(byEquip) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byStatus, sizeof(byStatus) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);

		//Update Or Insert
		pUser->UpdateUserTitle(dwCode, iValue, iLevel, byPremium, byEquip, byStatus, byActionType);
	}
	else
	{
		UserCopyNode *pUser = static_cast<UserCopyNode*>(pUserParent);

		SP2Packet kPacket(SSTPK_TITLE_UPDATE);
		PACKET_GUARD_VOID( kPacket.Write(dwUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write(byActionType) );
		PACKET_GUARD_VOID( kPacket.Write(dwCode) );
		PACKET_GUARD_VOID( kPacket.Write(iValue) );
		PACKET_GUARD_VOID( kPacket.Write(iLevel) );
		PACKET_GUARD_VOID( kPacket.Write(byPremium) );
		PACKET_GUARD_VOID( kPacket.Write(byEquip) );
		PACKET_GUARD_VOID( kPacket.Write(byStatus) );

		pUser->SendMessage(kPacket);
	}
}

void UserNodeManager::OnResultSelectTitle(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectTitle Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex	= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);

		//User title
		pUser->SelectUserTitle(query_data);
	}
}

void UserNodeManager::OnResultInsertBonusCash(CQueryResultData* query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertBonusCash Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex		= 0;
	int iAmount				= 0;
	DWORD dwExpirationDate	= 0;
	DWORD dwIndex			= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iAmount, sizeof(iAmount) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwExpirationDate, sizeof(dwExpirationDate) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwIndex, sizeof(dwIndex) ) );
	
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);

		pUser->InsertUserBonusCash(dwIndex, iAmount, dwExpirationDate);
	}
	else
	{
		UserCopyNode *pUser = static_cast<UserCopyNode*>(pUserParent);

		SP2Packet kPacket(SSTPK_BONUS_CAHSH_ADD);
		PACKET_GUARD_VOID( kPacket.Write(dwUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write(iAmount) );
		PACKET_GUARD_VOID( kPacket.Write(dwExpirationDate) );
		PACKET_GUARD_VOID( kPacket.Write(dwIndex) );

		pUser->SendMessage(kPacket);
	}
}

void UserNodeManager::OnResultUpdateBonusCash(CQueryResultData* query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateBonusCash Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex		= 0;
	BYTE byType				= 0;
	DWORD dwCashIndex		= 0;
	DWORD dwStatus			= 0;
	int iAmount				= 0;
	int iUsedAmount			= 0;
	int iType				= 0;
	int iValue1				= 0;
	int iValue2				= 0;
	char szGUID[USER_GUID_NUM_PLUS_ONE]="";

	DWORD dwTableIndex	= 0;
	int iTotal			= 0;
	int iRemaining		= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byType, sizeof(byType) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwCashIndex, sizeof(dwCashIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iAmount, sizeof(iAmount) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iType, sizeof(iType) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue1, sizeof(iValue1) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iValue2, sizeof(iValue2) ) );
		PACKET_GUARD_VOID( query_data->GetValue( szGUID, USER_GUID_NUM_PLUS_ONE ) );

	if (query_data->IsExist())
	{
		query_data->GetValue( dwStatus, sizeof(dwStatus) );				//성공여부
		query_data->GetValue( dwCashIndex, sizeof(dwCashIndex) );		//캐쉬인덱스
		query_data->GetValue( iAmount, sizeof(iAmount) ) ;				//잔액
		query_data->GetValue( iUsedAmount, sizeof(iUsedAmount) ) ;		//사용금액
	}
//	PACKET_GUARD_BREAK( query_data->GetValue( dwTableIndex, sizeof(dwTableIndex) ) );			// 테이블 인덱스
//	PACKET_GUARD_BREAK( query_data->GetValue( iRemaining, sizeof(iRemaining) ) );				// 잔액
	
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);
		pUser->UpdateUserBonusCash(static_cast<BonusCashUpdateType>(byType), dwStatus, dwCashIndex, iAmount, iUsedAmount, iType, iValue1, iValue2, szGUID);

	}
	else
	{
		UserCopyNode *pUser = static_cast<UserCopyNode*>(pUserParent);

		SP2Packet kPacket(SSTPK_BONUS_CASH_UPDATE);
		PACKET_GUARD_VOID( kPacket.Write(dwUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write(byType) );
		PACKET_GUARD_VOID( kPacket.Write(dwStatus) );
		PACKET_GUARD_VOID( kPacket.Write(dwCashIndex) );
		PACKET_GUARD_VOID( kPacket.Write(iAmount) );
		PACKET_GUARD_VOID( kPacket.Write(iUsedAmount) );
		PACKET_GUARD_VOID( kPacket.Write(iType) );
		PACKET_GUARD_VOID( kPacket.Write(iValue1) );
		PACKET_GUARD_VOID( kPacket.Write(iValue2) );
		PACKET_GUARD_VOID( kPacket.Write(szGUID) );

		pUser->SendMessage(kPacket);
	}
}

void UserNodeManager::OnResultSelectBonusCash(CQueryResultData* query_data)
{                     
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBonusCash Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex		= 0;
	DWORD dwSelectType		= 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwSelectType, sizeof(dwSelectType) ) );

	User *pUser = GetUserNode(dwUserIndex);

	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectBonusCash USER FIND NOT! :%d", dwUserIndex);
		return;
	}
#ifdef HRYOON_TEST
	pUser->SetBonusCashStatus(true);
#endif

	UserBonusCash *pUserBonusCash = pUser->GetUserBonusCash();
	if( !pUserBonusCash )
		return;

	pUserBonusCash->DBToData(query_data, dwSelectType);			// 보너스 캐쉬 메모리 저장 & 클라이언트 전송
}

void UserNodeManager::OnResultSelectExpiredBonusCash(CQueryResultData* query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectBonusCash Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex		= 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	User *pUser = GetUserNode(dwUserIndex);

	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectBonusCash USER FIND NOT! :%d", dwUserIndex);
		return;
	}

	UserBonusCash *pUserBonusCash = pUser->GetUserBonusCash();
	if( !pUserBonusCash )
		return;

	pUserBonusCash->DBToExpiredData(query_data);
}

void UserNodeManager::OnResultLoginSelectUserCoin( CQueryResultData* query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserCoin Result FAILED! :%d",query_data->GetResultType());
		return;
	}


	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	User *pUser = GetUserNode(dwUserIndex);

	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserCoin USER FIND NOT! :%d", dwUserIndex);
		return;
	}

	// UserCoin 메모리에 적용.
	ioUserCoin * pCoin = pUser->GetUserCoin();

	if(!pCoin)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultSelectUserCoin Coin FIND NOT! :%d", dwUserIndex);
		return;
	}

	pCoin->DBtoData(query_data);

	//유저의 퀘스트 완료 목록
	g_DBClient.OnLoginSelectAllQuestCompleteData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetGUID(), pUser->GetPublicID(), pUser->GetUserIndex() );

}

void UserNodeManager::OnResultInsertUserCoin( CQueryResultData* query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertUserCoin Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	BYTE byType = 0;
	PACKET_GUARD_VOID( query_data->GetValue( byType, sizeof(byType) ) );

	User *pUser = GetUserNode(dwUserIndex);

	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultInsertUserCoin USER FIND NOT! :%d", dwUserIndex);
		return;
	}
	// UserCoin 메모리에 적용.
	ioUserCoin * pCoin = pUser->GetUserCoin();

	if(!pCoin)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnResultInsertUserCoin Coin FIND NOT! :%d", dwUserIndex);
		return;
	}

	if( query_data->IsExist() )
	{
		DBTIMESTAMP updateTime;
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&updateTime, sizeof(DBTIMESTAMP) ) );

		CTime lastUpdateTime(Help::GetSafeValueForCTimeConstructor(updateTime.year, updateTime.month, updateTime.day, 
			updateTime.hour, updateTime.minute, updateTime.second));
		pCoin->UpdateCoinTime((USER_COIN_TYPE)byType, lastUpdateTime);
	}
}

void UserNodeManager::OnResultUpdateUserCoin( CQueryResultData* query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnUpdateUserCoin Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	BYTE byType = 0;
	PACKET_GUARD_VOID( query_data->GetValue( byType, sizeof(byType) ) );

	User *pUser = GetUserNode(dwUserIndex);

	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateUserCoin USER FIND NOT! :%d", dwUserIndex);
		return;
	}
	// UserCoin 메모리에 적용.
	ioUserCoin * pCoin = pUser->GetUserCoin();

	if(!pCoin)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB OnUpdateUserCoin Coin FIND NOT! :%d", dwUserIndex);
		return;
	}

	if( query_data->IsExist() )
	{
		DBTIMESTAMP updateTime;
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&updateTime, sizeof(DBTIMESTAMP) ) );

		CTime lastUpdateTime(Help::GetSafeValueForCTimeConstructor(updateTime.year, updateTime.month, updateTime.day, 
			updateTime.hour, updateTime.minute, updateTime.second));
		pCoin->UpdateCoinTime((USER_COIN_TYPE)byType, lastUpdateTime);
	}

}

// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
// 로그인 시 받아오는 카드매칭 유저 정보 
void UserNodeManager::OnResultSelectCardMatching(CQueryResultData *query_data)
{	//		SetCardMatchingData
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectCardMatching Result FAILED! :%d",query_data->GetResultType());
		return;
	}

#ifdef CARD_MATCHING_BY_BCKIM

	DWORD	dwUserIndex	= 0;	
	BYTE	MissionType = 0;
	BYTE	MissionMark1 = 0;
	BYTE	MissionMark2 = 0;
	BYTE	CardMark1 = 0;
	BYTE	CardMark2 = 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	bool bDetermine = false;
	User* pUser	= static_cast<User*>( pUserParent );
	CardMatching* pTempCardMatching = pUser->GetUserCardMatching();
	if( pUser && pTempCardMatching )
	{
		if( query_data->IsExist() )
		{ 	// 카드매칭 정보가 있으면, 저장 후 전송 
			PACKET_GUARD_VOID( query_data->GetValue( MissionType, sizeof(MissionType) ) );
			PACKET_GUARD_VOID( query_data->GetValue( MissionMark1, sizeof(MissionMark1) ) );
			PACKET_GUARD_VOID( query_data->GetValue( MissionMark2, sizeof(MissionMark2) ) );
			PACKET_GUARD_VOID( query_data->GetValue( CardMark1, sizeof(CardMark1) ) );
			PACKET_GUARD_VOID( query_data->GetValue( CardMark2, sizeof(CardMark2) ) );

			pTempCardMatching->SetCardMatchingMissionData( MissionType, MissionMark1, MissionMark2);
			pTempCardMatching->SetCardMatchingMarkData(CardMark1, CardMark2);
		}
		else
		{	//// 카드매칭 정보가 없다고, 새로운 미션 정보 설정 및 저장 
			pTempCardMatching->DetermineTheMissionData();		// 새로운 미션 셋팅  // 랜덤 셋팅하면서 바로 메모리 저장합니다. 
			pTempCardMatching->SetCardMatchingMarkData( 0,0);						// 새로운 미션 시작임으로 초기화 다시 확인 사살~
			pTempCardMatching->SaveData(CARD_MATCHING_NEW_MISSION);		// DB 저장
		}
	}

#endif 

}
// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가

// 2018-08-30 by bckim, 주사위 이벤트 추가
void UserNodeManager::OnResultSelectDiceGame(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectDiceGame Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	
	DWORD	dwUserIndex	= 0;
	int		iPosition	 = 0;
	int		iTrace[DICE_GAME_TRACE_DB] = {0,};
	BYTE	byBoradIndex = 0;
	int		iRewardIndex[DICE_GAME_REWARD_DB] = {0,};

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	User* pUser	= static_cast<User*>( pUserParent );
	if( pUser )
	{
		if( pUserParent->IsUserOriginal() )
		{
			if( query_data->IsExist() )
			{
				PACKET_GUARD_VOID( query_data->GetValue( iPosition, sizeof(int) ) );
				for( int i = 0; i < DICE_GAME_TRACE_DB; ++i )
					PACKET_GUARD_VOID( query_data->GetValue( iTrace[i], sizeof(int)) );
				PACKET_GUARD_VOID( query_data->GetValue( byBoradIndex, sizeof(BYTE) ) );
				for( int i = 0; i < DICE_GAME_REWARD_DB; ++i )
					PACKET_GUARD_VOID( query_data->GetValue( iRewardIndex[i], sizeof(int)) );

				pUser->SetProgressStateDice(COMPARE( iPosition,0,DICE_GAME_SLOT_REAL_COUNT ) );
				pUser->SetDiceGameData( iPosition, iTrace, byBoradIndex, iRewardIndex );				
			}
			else
			{
				pUser->SetProgressStateDice(false);
			}
		}
		else
		{
			UserCopyNode* pUserCopyNode = (UserCopyNode*)pUserParent;			
			SP2Packet kPacket( SSTPK_DICE_GAME_GET_INFO );			
			PACKET_GUARD_VOID( kPacket.Write( query_data->IsExist() ) );
			PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( iPosition ) );
			for( int i = 0; i < DICE_GAME_TRACE_DB; ++i )
				PACKET_GUARD_VOID( kPacket.Write( iTrace[i] ) );
			PACKET_GUARD_VOID( kPacket.Write( byBoradIndex ) );
			for( int i = 0; i < DICE_GAME_REWARD_DB; ++i )
				PACKET_GUARD_VOID( kPacket.Write( iRewardIndex[i] ) );
			pUserCopyNode->SendMessage( kPacket );
		}
	}
}
// End. 2018-08-30 by bckim, 주사위 이벤트 추가

// 로그인 시 받아오는 오크통 데이터
void UserNodeManager::OnResultSelectOakBarrel(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectOakBarrel Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD		dwUserIndex	= 0;
	DWORD		dwTime = 0;
	BYTE		byStep = 0;
	BYTE		byHole[OAK_BARREL_HOLE] = {0,};
	DBTIMESTAMP initTime;
	int			iLimitSword = 0;
	bool		bInit = false;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	User* pUser	= static_cast<User*>( pUserParent );
	if( pUser )
	{
		if( pUserParent->IsUserOriginal() )
		{
			if( query_data->IsExist() )
			{
				PACKET_GUARD_VOID( query_data->GetValue( byStep, sizeof(byStep) ) );
				for( int i = 0; i < OAK_BARREL_HOLE; ++i )
					PACKET_GUARD_VOID( query_data->GetValue( byHole[i], sizeof(byHole[i]) ) );
				PACKET_GUARD_VOID( query_data->GetValue( (char*)&initTime, sizeof(DBTIMESTAMP) ) );
				CTime cInitTime( Help::GetSafeValueForCTimeConstructor( initTime.year, initTime.month, initTime.day, initTime.hour, initTime.minute, initTime.second ) );
				dwTime = Help::ConvertCTimeToDate( cInitTime );
				PACKET_GUARD_VOID( query_data->GetValue( iLimitSword , sizeof(iLimitSword) ) );

				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][oak] login exist [%d] iLimitSword[%d]", dwUserIndex, iLimitSword );
				pUser->SetOakBarrelData( byStep, byHole, dwTime, iLimitSword );
			}
			else
			{
				CTime curTime = CTime::GetCurrentTime();
				//curTime = Help::GetSafeValueForCTimeConstructor( curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), 0, 0, 0 );
				dwTime = Help::ConvertCTimeToDate( curTime );
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][oak] login not exist [%d] iLimitSword[%d] max[%d]", dwUserIndex, iLimitSword, g_OakBarrelMgr.GetLimitSwordMax() );
				pUser->SetOakBarrelData( byStep, byHole, dwTime, g_OakBarrelMgr.GetLimitSwordMax(), true );
				pUser->SaveOakBarrel();
			}

			pUser->SendOakBarrelData();
		}
		else
		{
			UserCopyNode* pUserCopyNode = (UserCopyNode*)pUserParent;			
			SP2Packet kPacket( SSTPK_OAK_BARREL_GET_INFO );
			PACKET_GUARD_VOID( kPacket.Write( bInit ) );
			PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( byStep ) );
			for( int i = 0; i < OAK_BARREL_HOLE; ++i )
				PACKET_GUARD_VOID( kPacket.Write( byHole[i] ) );
			PACKET_GUARD_VOID( kPacket.Write( dwTime ) );
			PACKET_GUARD_VOID( kPacket.Write( iLimitSword ) );
			pUserCopyNode->SendMessage( kPacket );
		}
	}
}

// 오크통 데이터 업데이트
void UserNodeManager::OnResultUpdateOakBarrel(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateOakBarrel Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD		dwUserIndex = 0;
	DWORD		dwTime = 0;
	BYTE		byUpdateType = 0;
	BYTE		byStep = 0;
	BYTE		byPrevStep = 0;
	BYTE		byHole[OAK_BARREL_HOLE] = {0,};
	DBTIMESTAMP updateTime;
	int			iLimitSword = 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( byUpdateType, sizeof(byUpdateType) ) );

	if( query_data->IsExist() )
	{
		PACKET_GUARD_VOID( query_data->GetValue( byStep, sizeof(byStep) ) );
		for( int i = 0; i < OAK_BARREL_HOLE; ++i )
			PACKET_GUARD_VOID( query_data->GetValue( byHole[i], sizeof(byHole[i]) ) );
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&updateTime, sizeof(DBTIMESTAMP) ) );
		CTime cUpdateTime( Help::GetSafeValueForCTimeConstructor( updateTime.year, updateTime.month, updateTime.day, updateTime.hour, updateTime.minute, updateTime.second ) );
		dwTime = Help::ConvertCTimeToDate( cUpdateTime );
		PACKET_GUARD_VOID( query_data->GetValue( iLimitSword, sizeof(iLimitSword) ) );
	}

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	User* pUser	= static_cast<User*>( pUserParent );
	if( pUser )
	{
		// 초기화 되기 전 현재 오크통 단계
		byPrevStep = pUser->GetOakBarrelStep();

		if( pUserParent->IsUserOriginal() )
		{
			switch( byUpdateType )
			{
			case OAK_BARREL_SUCCESS:
				{
					pUser->SetOakBarrelData( byStep, byHole, dwTime, iLimitSword );
					pUser->SendOakBarrelResult( true, max(0, (int)(byPrevStep - 1)) );
				}
				break;

			case OAK_BARREL_FAIL:
				{
					//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][oak] 1. [%d]  byStep[%d] byPreStep[%d]", dwUserIndex, byStep, byPrevStep );
					pUser->SetOakBarrelData( byStep, byHole, dwTime, iLimitSword, true );
					pUser->SendOakBarrelResult( false, max(0, (int)(byPrevStep - 1)) );
				}
				break;

			case OAK_BARREL_REWARD:
				{
					pUser->SendOakBarrelReward( pUser->GetOakBarrelStep() );
					pUser->SetOakBarrelData( byStep, byHole, dwTime, iLimitSword );
				}
				break;
			}
		}		
	
		else
		{
			UserCopyNode* pUserCopyNode = (UserCopyNode*)pUserParent;			
			SP2Packet kPacket( SSTPK_OAK_BARREL_UPDATE_INFO );
			PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( byUpdateType ) );
			PACKET_GUARD_VOID( kPacket.Write( byStep ) );
			PACKET_GUARD_VOID( kPacket.Write( byPrevStep ) );
			for( int i = 0; i < OAK_BARREL_HOLE; ++i )
				PACKET_GUARD_VOID( kPacket.Write( byHole[i] ) );
			PACKET_GUARD_VOID( kPacket.Write( dwTime ) );
			PACKET_GUARD_VOID( kPacket.Write( iLimitSword ) );
			pUserCopyNode->SendMessage( kPacket );	
		}
	}
}


// 오크통 칼 사용 일일한도 수량 초기화 시각 업데이트
void UserNodeManager::OnResultTimeUpdateOakBarrel(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultTimeUpdateOakBarrel Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD		dwUserIndex, dwTime	= 0;
	int			iResult = 0;
	DBTIMESTAMP initTime;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	
	if( query_data->IsExist() )
	{
		PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&initTime, sizeof(DBTIMESTAMP) ) );
		CTime cInitTime( Help::GetSafeValueForCTimeConstructor( initTime.year, initTime.month, initTime.day, initTime.hour, initTime.minute, initTime.second ) );
		dwTime = Help::ConvertCTimeToDate( cInitTime );
	}

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>( pUserParent );
		if( pUser )
			pUser->SetOakBarrelTimeData( dwTime );
	}
	else
	{
		UserCopyNode* pUserCopyNode = (UserCopyNode*)pUserParent;			
		SP2Packet kPacket( SSTPK_OAK_BARREL_UPDATE_TIME );
		PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( dwTime ) );
		pUserCopyNode->SendMessage( kPacket );	
	}
}


// 오크통 칼 일일 사용 한도 수량 초기화 업데이트
void UserNodeManager::OnResultLimitSwordUpdateOakBarrel(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultLimitSwordUpdateOakBarrel Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD	dwUserIndex = 0;
	int		iLimitSword = 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iLimitSword, sizeof(iLimitSword) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>( pUserParent );
		if( pUser )
		{
			pUser->SetOakBarrelLimitSword( iLimitSword );
		}
	}
	else
	{
		UserCopyNode* pUserCopyNode = (UserCopyNode*)pUserParent;			
		SP2Packet kPacket( SSTPK_OAK_BARREL_UPDATE_LIMIT_SWORD );
		PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( iLimitSword ) );
		pUserCopyNode->SendMessage( kPacket );	
	}
}

void UserNodeManager::OnResultSelectUserMatch(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserMatch Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex(0);
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	
	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	User* pUser	= static_cast<User*>( pUserParent );
	if( pUser )
	{
		if( pUserParent->IsUserOriginal() )
		{
			if( query_data->IsExist() )
			{
				int iResult(0), iPlayCount(0), iMMR(0), iWinCount(0), iMaxWinCount(0), iMaxLoseCount(0), iRankMMR(0);

				PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );
				PACKET_GUARD_VOID( query_data->GetValue( iPlayCount, sizeof(int) ) );
				PACKET_GUARD_VOID( query_data->GetValue( iMMR, sizeof(int) ) );
				PACKET_GUARD_VOID( query_data->GetValue( iWinCount, sizeof(int) ) );
				PACKET_GUARD_VOID( query_data->GetValue( iMaxWinCount, sizeof(int) ) );
				PACKET_GUARD_VOID( query_data->GetValue( iMaxLoseCount, sizeof(int) ) );
				PACKET_GUARD_VOID( query_data->GetValue( iRankMMR, sizeof(int) ) );

				if( iResult == 0 )
				{
					// DB에 일대일 정보가 없는 유저다.
					int iMMRPoint = pUser->GetKillDeathLevel() - g_LevelMatchMgr.GetAddGradeLevel();
					iMMRPoint = 1000 + ( iMMRPoint * 10 );
					iMMRPoint = max( iMMRPoint, g_MatchManager.GetDefaultMMR() );

					iMMR = iMMRPoint;
					iRankMMR = 1000;	// 기본 1000점
				}
				else
				{
					//초기화 된 경우. 실계포인트로 세팅
					if( iMMR == 0 && iWinCount == 0 && iMaxLoseCount == 0 && iPlayCount == 0 )
					{
						int iMMRPoint = pUser->GetKillDeathLevel() - g_LevelMatchMgr.GetAddGradeLevel();
						iMMRPoint = 1000 + ( iMMRPoint * 10 );
						iMMRPoint = max( iMMRPoint, g_MatchManager.GetDefaultMMR() );

						iMMR = iMMRPoint;
						iRankMMR = 1000;
					}

				    else if( g_MatchManager.CheckMinimumMMR() && iMMR < g_MatchManager.GetMinimumMMR() )
					{
						iMMR = g_MatchManager.GetMinimumMMR();
					}
				}

				pUser->SetUserMatch( iPlayCount, iMMR, iWinCount, iMaxWinCount, iMaxLoseCount, iRankMMR );
				pUser->SendSuccessionData();
			}
		}
	}
}

void UserNodeManager::OnResultUpdateUserMatch(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserMatch Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	int iPlayCount = 0, iMMR = 0, iWinCount = 0, iMaxWinCount = 0, iMaxLoseCount = 0, iRankMMR = 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iPlayCount, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iMMR, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iWinCount, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iMaxWinCount, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iMaxLoseCount, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iRankMMR, sizeof(int) ) );

	int iResult = 0;
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(iResult) ) );

	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	User* pUser	= static_cast<User*>( pUserParent );
	if( pUser )
	{
		if( pUserParent->IsUserOriginal() )
		{
			pUser->SetUserMatch( iPlayCount, iMMR, iWinCount, iMaxWinCount, iMaxLoseCount, iRankMMR );
		}
		else
		{
			UserCopyNode* pUserCopyNode = (UserCopyNode*)pUserParent;
			SP2Packet kPacket( SSTPK_USER_MATCH_UPDATE );
			PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( iPlayCount ) );
			PACKET_GUARD_VOID( kPacket.Write( iMMR ) );
			PACKET_GUARD_VOID( kPacket.Write( iWinCount ) );
			PACKET_GUARD_VOID( kPacket.Write( iMaxWinCount ) );
			PACKET_GUARD_VOID( kPacket.Write( iMaxLoseCount ) );
			PACKET_GUARD_VOID( kPacket.Write( iRankMMR ) );
			pUserCopyNode->SendMessage( kPacket );	
		}
	}
}

void UserNodeManager::OnResultSelectUserSpiritData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserSpiritData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser )
	{
		ioUserSpirit *pUserSpirit = pUser->GetUserSpirit();
		if( pUserSpirit )
		{
			pUserSpirit->DBtoData( query_data );
		}
	}
}

void UserNodeManager::OnResultInsertUserSpiritData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertUserSpiritData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}

void UserNodeManager::OnResultUpdateUserSpiritData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserSpiritData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
}




// 커스텀 메달
void UserNodeManager::OnResultSelectUserCustomMedalData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserCustomMedalData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser )
	{
		ioCustomMedal *pCustomMedal = pUser->GetUserCustomMedal();
		if( pCustomMedal )
		{
			pCustomMedal->DBtoData( query_data );
		}
	}
}

void UserNodeManager::OnResultInsertUserCustomMedalData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertUserCustomMedalData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	if( !query_data )
		return;


	DWORD dwUserIndex	= 0;
	int iItemCode = 0, iPresentIndex = 0, iPresentSlotIndex = 0, iItemIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iItemCode , sizeof(iItemCode ) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iPresentIndex, sizeof(iPresentIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iPresentSlotIndex, sizeof(iPresentSlotIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iItemIndex, sizeof(iItemIndex) ) );


	UserParent *pUserParent = GetGlobalUserNode( dwUserIndex );

	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);
		//pUser->SendCustomMedalData(query_data);
		pUser->SendCustomMedalDataToClient( dwUserIndex, iItemCode, iPresentIndex, iPresentSlotIndex, iItemIndex );
	}
	// 서버 이동
	else
	{
		UserCopyNode* pUserCopyNode = (UserCopyNode*)pUserParent;
		SP2Packet kPacket( SSTPK_CUSTOM_MEDAL_ADD );
		PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( iItemCode ) );
		PACKET_GUARD_VOID( kPacket.Write( iPresentIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( iPresentSlotIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( iItemIndex ) );
		pUserCopyNode->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInsertUserCustomMedalData Result Sever Move! :%d",dwUserIndex);
	}
}

void UserNodeManager::OnResultUpdateUserCustomMedalStatus(CQueryResultData *query_data)
{

}

// 타임게이트
void UserNodeManager::OnResultSelectUserTimeGateData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectUserTimeGateData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser )
	{
		DWORD dwTableIndex = 0, dwFriendIndex = 0;
		if( query_data->GetResultCount() > 0 )
		{
			DBTIMESTAMP LastOpenTime;

			PACKET_GUARD_VOID( query_data->GetValue( (char*)&LastOpenTime, sizeof(DBTIMESTAMP) ) );	// 만료기간

			CTime cLastOpenTime(LastOpenTime.year, LastOpenTime.month, LastOpenTime.day, LastOpenTime.hour, LastOpenTime.minute, 0);		

			pUser->SetUserTimeGate( cLastOpenTime.GetTime() ); 	// 마지막 오픈시간

			SP2Packet kPacket( STPK_TIME_GATE_INFO );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetUserTimeGate() ) );
			pUser->SendMessage( kPacket );

		}
		else
		{
			SP2Packet kPacket( STPK_TIME_GATE_INFO );
			PACKET_GUARD_VOID( kPacket.Write( 0 ) );
			pUser->SendMessage( kPacket );

			pUser->SetUserTimeGate( 0 ); 	// 마지막 오픈시간
		}
	}
}

void UserNodeManager::OnResultUpdateUserTimeGateData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateUserTimeGateData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	User *pUser = GetUserNode(dwUserIndex);
	if( pUser )
	{
		DWORD dwTableIndex = 0, dwFriendIndex = 0;
		if( query_data->GetResultCount() > 0 )
		{
			DBTIMESTAMP LastOpenTime;

			PACKET_GUARD_VOID( query_data->GetValue( (char*)&LastOpenTime, sizeof(DBTIMESTAMP) ) );	// 만료기간

			CTime cLastOpenTime(LastOpenTime.year, LastOpenTime.month, LastOpenTime.day, LastOpenTime.hour, LastOpenTime.minute, 0);		

			pUser->SetUserTimeGate( cLastOpenTime.GetTime() ); 	// 마지막 오픈시간

			printf("cLastOpenTime.GetTime()%d\n", pUser->GetUserTimeGate());

			int iItemList = 0, iPresentType = 0, iValue1 = 0, iValue2 = 0;
			bool bAlarm = false;
			int nReturn = g_TimeGateMgr.GetTimeGateReward( iItemList, iPresentType, iValue1, iValue2, bAlarm );

			if( nReturn == -1) 
			{
				SP2Packet kPacket( STPK_TIME_GATE_RESPONSE );
				PACKET_GUARD_VOID( kPacket.Write( TG_CLOSE ) );
				pUser->SendMessage( kPacket );
			}
			else
			{
				SP2Packet kPacket( STPK_TIME_GATE_RESPONSE );
				PACKET_GUARD_VOID( kPacket.Write( TG_OPEN ) );
				PACKET_GUARD_VOID( kPacket.Write( iItemList ) );
				PACKET_GUARD_VOID( kPacket.Write( iPresentType ) );
				PACKET_GUARD_VOID( kPacket.Write( iValue1 ) );
				PACKET_GUARD_VOID( kPacket.Write( iValue2 ) );
				PACKET_GUARD_VOID( kPacket.Write( pUser->GetUserTimeGate() ) );
				PACKET_GUARD_VOID( kPacket.Write( bAlarm ) );
				pUser->SendMessage( kPacket );
				
#ifdef __OHTG_SOLDIER_TAKE_CHANGE__

				bool bSend = true;
				if( iPresentType == IT_SOLDIER )
				{
					if(true == pUser->SendSoldierTake(iValue1, iValue2))
					{
						bSend = false;
					}
				}
				if(bSend)
				{
					g_TimeGateMgr.SendTimeGatePresent( iPresentType, iValue1, iValue2 , pUser );
				}
#else //__OHTG_SOLDIER_TAKE_CHANGE__
				g_TimeGateMgr.SendTimeGatePresent( iPresentType, iValue1, iValue2 , pUser );
#endif //__OHTG_SOLDIER_TAKE_CHANGE__

				// 전체 서버 알림
				AlarmEventOfTimeGateToAllUsers(pUser, bAlarm, iItemList, iPresentType, iValue1, iValue2);
			}
		}
		else
		{
			// 타임 게이트 DB 실패로 적용 안됨 
			SP2Packet kPacket( STPK_TIME_GATE_RESPONSE );
			PACKET_GUARD_VOID( kPacket.Write( TG_CLOSE ) );
			pUser->SendMessage( kPacket );
		}
	}
}


void UserNodeManager::OnResultSelectPracticeList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectPracticeList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	User* pUser = GetUserNode( dwUserIndex );
	if( !pUser ) return;

	ioUserPractice* pPractice = pUser->GetUserPractice();
	if( !pPractice ) return;

	LOOP_GUARD();
	while( query_data->IsExist() )
	{
		SPractice kPractice;
		PACKET_GUARD_VOID( query_data->GetValue( kPractice.m_dwID, sizeof(kPractice.m_dwID) ) );
		PACKET_GUARD_VOID( query_data->GetValue( kPractice.m_dwGrade, sizeof(kPractice.m_dwGrade) ) );
		PACKET_GUARD_VOID( query_data->GetValue( kPractice.m_dwCount, sizeof(kPractice.m_dwCount) ) );
		PACKET_GUARD_VOID( query_data->GetValue( kPractice.m_dwTime, sizeof(kPractice.m_dwTime) ) );
		PACKET_GUARD_VOID( query_data->GetValue( kPractice.m_dwRank, sizeof(kPractice.m_dwRank) ) );

		pPractice->AddPractice( kPractice );
	}

	pPractice->SetLastTime(pUser->GetLastLogOutTime());
	pPractice->SetSendInfo(true);
	LOOP_GUARD_CLEAR();
}

void UserNodeManager::OnResultUpdatePracticeAdmission(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdateMissionData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int iResult = 0, iMoney =0;
	DWORD dwUserIndex = 0, dwPracticeID = 0, dwPracticeGrade = 0, dwPracticeCount = 0, dwPracticeTime = 0, dwPracticeRank = 0 ;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeID, sizeof(dwPracticeID) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeGrade, sizeof(dwPracticeGrade) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeCount, sizeof(dwPracticeCount) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeTime, sizeof(dwPracticeTime) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeRank, sizeof(dwPracticeRank) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iMoney, sizeof(iMoney) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );

	UserParent* pUserParent = GetGlobalUserNode(dwUserIndex);
	if( !pUserParent ) return;

	TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice OnUserPracticeAdmission - [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
							dwPracticeID, dwPracticeGrade, dwPracticeCount, dwPracticeTime, dwPracticeRank);

	ioHashString	szNickname = pUserParent->GetPublicID();
	if(pUserParent->IsUserOriginal())
	{
		User* pUser = (User*)pUserParent;
		if( !pUser ) return;

		ioUserPractice* pPractice = pUser->GetUserPractice();
		if( !pPractice ) return;

		if( iResult == 0 )
			pPractice->SetPractice( dwPracticeID, dwPracticeGrade, dwPracticeCount, dwPracticeTime, dwPracticeRank );

		SP2Packet kePacket( STPK_PRACTICE_ENTER );
		PACKET_GUARD_VOID( kePacket.Write(PRACTICE_ENTER_SUCCESS) );
		PACKET_GUARD_VOID( kePacket.Write(dwPracticeID) );
		PACKET_GUARD_VOID( kePacket.Write(dwPracticeCount) );
		PACKET_GUARD_VOID( kePacket.Write(iMoney) );
		pUser->SendMessage( kePacket );
	}
	else
	{
		if( iResult == 0 )
		{
			UserCopyNode* pUser = (UserCopyNode*)pUserParent;
			if( !pUser ) return;

			SP2Packet kPacket( SSTPK_SYNC_PRACTICE_ADMISSION );
			PACKET_GUARD_VOID( kPacket.Write(dwUserIndex) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeID) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeGrade) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeCount) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeTime) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeRank) );
			PACKET_GUARD_VOID( kPacket.Write(iMoney) );
			pUser->SendMessage( kPacket );

		}
	}

	return;
}

void UserNodeManager::OnResultUpdatePracticeTime(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultUpdatePracticeTime Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int iResult = 0;
	DWORD dwUserIndex = 0, dwPracticeID = 0, dwPracticeGrade = 0, dwCurrentGrade = 0, dwPracticeCount = 0, dwPracticeTime = 0, dwPracticeRank = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeID, sizeof(dwPracticeID) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeGrade, sizeof(dwPracticeGrade) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeCount, sizeof(dwPracticeCount) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeTime, sizeof(dwPracticeTime) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPracticeRank, sizeof(dwPracticeRank) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwCurrentGrade, sizeof(dwCurrentGrade) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );

	UserParent* pUserParent = GetGlobalUserNode(dwUserIndex);
	if( !pUserParent ) return;

	TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice OnUserPracticeTime - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
					dwUserIndex, dwPracticeID, dwPracticeGrade, dwPracticeCount, dwPracticeTime, dwPracticeRank);

	ioHashString	szNickname = pUserParent->GetPublicID();
	if(pUserParent->IsUserOriginal())
	{
		User* pUser = (User*)pUserParent;
		if( !pUser ) return;

		ioUserPractice* pPractice = pUser->GetUserPractice();
		if( !pPractice ) return;

		if( iResult == 0 )
		{
			pPractice->SetPractice( dwPracticeID, dwPracticeGrade, dwPracticeCount, dwPracticeTime, dwPracticeRank );
			}
	}
	else
	{
		if( iResult == 0 )
		{
			UserCopyNode* pUser = (UserCopyNode*)pUserParent;
			if( !pUser ) return;

			SP2Packet kPacket( SSTPK_SYNC_PRACTICE_TIME_UPDATE );
			PACKET_GUARD_VOID( kPacket.Write(dwUserIndex) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeID) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeGrade) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeCount) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeTime) );
			PACKET_GUARD_VOID( kPacket.Write(dwPracticeRank) );
			pUser->SendMessage( kPacket );
		}
	}

	SP2Packet kPacket( MSTPC_PRACTICE_USERUPDATE );
	kPacket << dwUserIndex;
	kPacket << szNickname;
	kPacket << dwPracticeID;
	kPacket << dwPracticeTime;
	kPacket << dwCurrentGrade;
	kPacket << dwPracticeCount;
	g_MainServer.SendMessage( kPacket );
	return;
}

void UserNodeManager::OnResultInitPracticeData(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultInitPracticeData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int iResult = 0;
	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );
}

void UserNodeManager::OnResultPracticeRankList(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultSelectPracticeList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );

	User* pUser = GetUserNode( dwUserIndex );
	if( !pUser ) return;

	ioUserPractice* pPractice = pUser->GetUserPractice();
	if( !pPractice ) return;

	LOOP_GUARD();
	while( query_data->IsExist() )
	{
		SPracticeRank kPracticeRank;
		char szStart[TIMESTRING] = "", szEnd[TIMESTRING] = "";
		query_data->GetValue(szStart,TIMESTRING);
		query_data->GetValue(szEnd,TIMESTRING);
		kPracticeRank.m_strStartDate = szStart;
		kPracticeRank.m_strEndDate = szEnd;
		PACKET_GUARD_VOID( query_data->GetValue( kPracticeRank.m_dwID, sizeof(kPracticeRank.m_dwID) ) );
		PACKET_GUARD_VOID( query_data->GetValue( kPracticeRank.m_dwRank, sizeof(kPracticeRank.m_dwRank) ) );
		kPracticeRank.m_vSPracticePresent.clear();

		for(int i = 0; i < PracticePresentCOUNT; i++)
		{
			SPracticePresent kPracticePresent;
			PACKET_GUARD_VOID( query_data->GetValue( kPracticePresent.m_dwPresentType, sizeof(kPracticePresent.m_dwPresentType) ) );
			PACKET_GUARD_VOID( query_data->GetValue( kPracticePresent.m_dwCode, sizeof(kPracticePresent.m_dwCode) ) );
			PACKET_GUARD_VOID( query_data->GetValue( kPracticePresent.m_dwValue, sizeof(kPracticePresent.m_dwValue) ) );
			kPracticeRank.m_vSPracticePresent.push_back(kPracticePresent);
		}

		pPractice->AddPracticeRankDate( kPracticeRank );
	}

	pPractice->SetSendRank(true);
	LOOP_GUARD_CLEAR();
}

void UserNodeManager::AllUserPracticeInitData()
{
	for(uUser_iter iter = m_uUserNode.begin() ; iter != m_uUserNode.end() ; ++iter)
	{
		User *pUser = iter->second;
		if( pUser == NULL ) continue;
		if( !pUser->IsConnectState() ) continue;

		ioUserPractice* pPractice = pUser->GetUserPractice();
		if( !pPractice ) return;

		pPractice->InitData();

	}

	SP2Packet kPacket( STPK_PRACTICE_INIT_DATA );
	SendMessageAll( kPacket );
}


void UserNodeManager::OnResultPCBangPlayTime(CQueryResultData *query_data)
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"DB OnResultPCBangPlayTime Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	int iResult = 0;
	DWORD dwUserIndex = 0, dwPCBangNumber = 0;
	PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(dwUserIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwPCBangNumber, sizeof(dwPCBangNumber) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );

	if(-1 == iResult)
	{
		EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "PCBang LOG Result [UserIndex %d] [PCBang Number %d] [Result %d]" , dwUserIndex, dwPCBangNumber, iResult);
	}
}

// 서버 사이드 알람 테스트
void UserNodeManager::TestAlarmToAllUsers(User * pUser)
{
	AlarmEventOfTimeGateToAllUsers(pUser, true, 1, (int)IT_SOLDIER, 17, 0);
	AlarmEventOfPresentToAllUsers(pUser, (int)PRESENT_LOSA_SEARCH_MENT, (int)ioUserPresent::PRESENT_STATE_NEW, (int)PRESENT_PESO, 20000, 10000);

	// 클라이언트 수정요
	{
		int iItemType = g_FishingMgr.GetFishingItemNum( true, true, true );
		int iPresentNum = g_FishingMgr.GetEventFishingPresentNum( iItemType, true, true );

		bool bEventFishing = false;
		if( iPresentNum > 0 )
			bEventFishing = true;

		SP2Packet kReturn( STPK_FISHING );

		int iEventFishingPresentType = 0, iEventFishingPresentValue1 = 0, iEventFishingPresentValue2 = 0;
		if( bEventFishing )
			g_PresentHelper.SendEventFishingPresent( pUser, iPresentNum, kReturn, iEventFishingPresentType, iEventFishingPresentValue1, iEventFishingPresentValue2 );

		bool bSpecial = true;
		int iPresentType = 0, iPresentValue1 = 0, iPresentValue2 = 0;
		if( bSpecial )
		{
			g_PresentHelper.SendSpecialFishingPresent( pUser, kReturn, iPresentType, iPresentValue1, iPresentValue2 );
		}


		AlarmEventOfFishingToAllUsers(pUser, true, iItemType, 
										bEventFishing, iEventFishingPresentType, iEventFishingPresentValue1, iEventFishingPresentValue2,
										bSpecial, iPresentType, iPresentValue1, iPresentValue2);
	}

	AlarmEventInBuyExtraItemBuyToAllUsers(pUser, (int)EXTRAITEM_BUY_OK, (int)ioUserExtraItem::EET_ENABLE, true, 10023, 2, 10024, 10000);

	AlarmEventInSuperGashaponPresentToAllUsers(pUser, true, (int)SUPER_GASHPON_MAIN, 1002232, 1);

	// 클라이언트 수정요 - szItemName
	{
		ioUserExtraItem *pExtraItem = pUser->GetUserExtraItem();
		ioUserExtraItem::EXTRAITEMSLOT rkExtraItem;
		if( pExtraItem->GetExtraItem( 33, rkExtraItem ) )
		{
			AlarmEventOfItemRechargeProcessToAllUsers(pUser, (int)ioUserExtraItem::EPT_MORTMAIN, 1002232, rkExtraItem.m_iItemCode);
		}
	}

	AlarmEventInGradeUpToAllUsers(pUser);

	// 클라이언트 체크요
	{
		ioHashString szSendID;
		bool  bAlarm = false;
		short iPresentType = 0;
		int   iPresentValue1 = 0;
		int   iPresentValue2 = 0;
		int   iPresentValue3 = 0;
		int   iPresentValue4 = 0;
		if( g_PresentHelper.GetOneDayEventPresent( szSendID, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bAlarm ) != -1 )
		{
			AlarmEventInGoldItemEventToAllUsers(pUser, true, iPresentType, iPresentValue1, iPresentValue2);
		}
	}
	
	// sp2_fishing_info.ini - item%d_num(iItemType)
	AlarmEventInFishingSellToAllUsers(pUser, true, 13, 1000);

	// 클라이언트 수정요
	{
		ioUserExtraItem *pExtraItem = pUser->GetUserExtraItem();
		ioUserExtraItem::EXTRAITEMSLOT rkExtraItem;
		if( pExtraItem->GetExtraItem( 33, rkExtraItem ) )
		{
			AlarmEventInGrowthCatalystSuccessToAllUsers(pUser, (int)ioUserExtraItem::EPT_MORTMAIN, rkExtraItem.m_iItemCode);
		}
	}

	// 클라이언트 수정요
	{
		short iPresentType   = 0;
		int   iPresentValue1 = 0;
		int   iPresentValue2 = 0;
		int   iPresentValue3 = 0;
		int   iPresentValue4 = 0;
		bool  bWholeAlarm    = false;
		int   iPresentPeso   = 0;
		if( g_PresentHelper.SendRandomDecoPresent( pUser, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlarm, iPresentPeso, true ) )
		{
			AlarmEventInRandomDecoPresentToAllUsers(pUser, true, (int)ioEtcItem::EIT_ETC_RANDOM_DECO_M, iPresentType, iPresentValue1, iPresentValue2);
		}
	}
	
	// 클라이언트 수정요
	{
		SP2Packet kReturnPacket( STPK_AWARDING_RESULT );
		int iPresentType = 0, iPresentValue1 = 0, iPresentValue2 = 0;
		bool bAlarm = false;
		g_PresentHelper.SendSpacialAwardPresent( pUser, kReturnPacket, iPresentType, iPresentValue1, iPresentValue2, bAlarm );
		AlarmEventInAwardLuckyPresentToAllUsers(pUser, true, iPresentType, iPresentValue1, iPresentValue2 );
	}

	AlarmEventInExcavationToAllUsers(pUser, true, 3, 4, 20000, 1.1);

	{
		short iPresentType   = 0;
		int   iPresentValue1 = 0;
		int   iPresentValue2 = 0;
		int   iPresentValue3 = 0;
		int   iPresentValue4 = 0;
		bool  bWholeAlarm    = false;
		int   iPresentPeso   = 0;
		if( g_PresentHelper.SendGashaponPresent( pUser, (int)ioEtcItem::EIT_ETC_GASHAPON, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlarm, iPresentPeso ) )
		{
			AlarmEventInGashaponPresentToAllUsers(pUser, true, (int)ioEtcItem::EIT_ETC_GASHAPON, iPresentType, iPresentValue1, iPresentValue2);
		}
	}
}

// 장비의 25 레벨 강화성공인 경우 전서버에 알린다.
void UserNodeManager::AlarmEventInReinforceOfEtcItemToAllUsers(User * pUser, int iMentType, int iItemCode, int iResultReinforce)
{
	if (NEEDED_EXTRAITEM_REINFORCE_LEVEL_TO_BE_ALARMED > iResultReinforce)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << iMentType;
	kPacket << pUser->GetPublicID();
	kPacket << iItemCode;
	kPacket << iResultReinforce;

	SendAllServerAlarmMent( kPacket );
}

// 서버사이드 알람을 사용할지 여부
#define USE_ALARM_BY_SERVER_SIDE

// 타임 게이트 관련 전서버 알림
void UserNodeManager::AlarmEventOfTimeGateToAllUsers(User * pUser, bool bAllAlarm, int iListType, int iPresentType, int iValue1, int iValue2)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAllAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_TIMEGATE_RESULT << pUser->GetPublicID() << iListType << iPresentType << iValue1 << iValue2;

	SendAllServerAlarmMent( kPacket );
}

// 선물관련 전서버 알림
void UserNodeManager::AlarmEventOfPresentToAllUsers(User * pUser, int iPresentMent, int iPresentState, int iPresentType, int iPresentValue1, DWORD dwLimitDate)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	//삭제된 선물
	if( 0 == dwLimitDate )
		return;

	// 검색 이벤트시 10,000페소 획득 유저 전체 서버에 공지
	if( iPresentMent == PRESENT_LOSA_SEARCH_MENT && iPresentState == ioUserPresent::PRESENT_STATE_NEW )
	{
		if( iPresentType == PRESENT_PESO && iPresentValue1 >= 10000 )
		{
			SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
			kPacket << UDP_SERVER_ALARM_LOSA_SEARCH_EVENT << pUser->GetPublicID() << iPresentValue1;

			SendAllServerAlarmMent( kPacket );
		}
	}
}

// 낚시 관련 전서버 알림
void UserNodeManager::AlarmEventOfFishingToAllUsers(User * pUser, bool bAllAlarm, int iItemType, 
													bool bEventFishing, int iEventFishingPresentType, int iEventFishingPresentValue1, int iEventFishingPresentValue2,
													bool bSpecial, int iPresentType, int iPresentValue1, int iPresentValue2)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAllAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_FISHING << pUser->GetPublicID() << iItemType;
	kPacket << bEventFishing << iEventFishingPresentType << iEventFishingPresentValue1 << iEventFishingPresentValue2;
	kPacket << bSpecial << iPresentType << iPresentValue1 << iPresentValue2;

	SendAllServerAlarmMent( kPacket );
}

// extra item 구매 관련 전서버 알림
void UserNodeManager::AlarmEventInBuyExtraItemBuyToAllUsers(User * pUser, int iResult, int iTradeType, bool bAlarm, int iItemCode, int iReinforce, int iMachineCode, int iPeriodTime)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if( iResult == EXTRAITEM_BUY_OK )
	{
		if( iTradeType == ioUserExtraItem::EET_ENABLE )
		{
			SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
			kPacket << UDP_SERVER_ALARM_TRAD_ENABLE << pUser->GetPublicID() << iItemCode << iReinforce;

			SendAllServerAlarmMent( kPacket );
		}
	}

	if( bAlarm )
	{
		SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
		kPacket << UDP_SERVER_ALARM_EXTRAITEM << pUser->GetPublicID() << iMachineCode << iItemCode << iPeriodTime;

		SendAllServerAlarmMent( kPacket );
	}
}

// sp2_super_gashapon_present.ini에 셋팅
// SUPER_GASHPON_MAIN 류에만 셋팅됨
void UserNodeManager::AlarmEventInSuperGashaponPresentToAllUsers(User * pUser, bool bAlarm, int iProductType, DWORD dwEtcItemCode, DWORD dwPresentIndex)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_SUPERGASHAPON_PRESENT << pUser->GetPublicID() << dwEtcItemCode << dwPresentIndex;
	
	SendAllServerAlarmMent( kPacket );
}


#ifdef OHTG_NEW_RANROM_BOX_20201109
// sp2_super_gashapon_present.ini에 셋팅
// SUPER_GASHPON_MAIN 류에만 셋팅됨
void UserNodeManager::AlarmEventInNewSuperGashaponPresentToAllUsers(User * pUser, bool bAlarm, int iProductType, DWORD dwEtcItemCode, DWORD dwCategoryIndex, DWORD dwPackageIndex)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_NEW_SUPERGASHAPON_PRESENT << pUser->GetPublicID() << dwEtcItemCode << dwCategoryIndex << dwPackageIndex;
	
	SendAllServerAlarmMent( kPacket );
}
#endif //OHTG_NEW_RANROM_BOX_20201109

// etc item, accessary recharge process 관련 전서버 알림
void UserNodeManager::AlarmEventOfItemRechargeProcessToAllUsers(User * pUser, int iPeriodType, DWORD dwUseEtcItemIndex, int iItemCode)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (ioUserExtraItem::EPT_MORTMAIN != iPeriodType)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_ITEM_RECHARGE_SUCCESS << pUser->GetPublicID() << dwUseEtcItemIndex << iItemCode;

	SendAllServerAlarmMent( kPacket );
}

// 유저의 특정 레벨업을 전서버 알림
// 클라이언트 config/sp2_alarm_info.ini - [GradeUpInfo] 참고
// MaxLevel = 4
// Level_1	=	50
// Level_2	=	80
// Level_3	=	90
// Level_4	=	100
void UserNodeManager::AlarmEventInGradeUpToAllUsers(User * pUser)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (pUser->GetGradeLevel() != 50 && pUser->GetGradeLevel() != 80 && pUser->GetGradeLevel() != 90 && pUser->GetGradeLevel() != 100)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_GRADE_UP << pUser->GetPublicID() << pUser->GetGradeLevel();

	SendAllServerAlarmMent( kPacket );
}

void UserNodeManager::AlarmEventInGoldItemEventToAllUsers(User * pUser, bool bAlarm, int iPresentType, int iPresentValue1, int iPresentValue2)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_GOLD_ITEM_EVENT << pUser->GetPublicID() << iPresentType << iPresentValue1 << iPresentValue2;

	SendAllServerAlarmMent( kPacket );
}

void UserNodeManager::AlarmEventInFishingSellToAllUsers(User * pUser, bool bAlarm, int iItemType, int iResultPeso)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_FISHING_SELL << pUser->GetPublicID() << iItemType << iResultPeso;

	SendAllServerAlarmMent( kPacket );
}

void UserNodeManager::AlarmEventInGrowthCatalystSuccessToAllUsers(User * pUser, int iPeriodType, int iItemCode)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (ioUserExtraItem::EPT_MORTMAIN != iPeriodType)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_GROWTH_CATALYST_SUCCESS;
	kPacket << pUser->GetPublicID();
	kPacket << iItemCode;

	SendAllServerAlarmMent( kPacket );
}

void UserNodeManager::AlarmEventInRandomDecoPresentToAllUsers(User * pUser, bool bAlarm, DWORD dwItemType, int iPresentType, int iPresentValue1, int iPresentValue2)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAlarm)
		return;

	if( ioEtcItem::EIT_ETC_RANDOM_DECO_M != dwItemType && ioEtcItem::EIT_ETC_RANDOM_DECO_W != dwItemType )
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_RANDOM_DECO_PRESENT << pUser->GetPublicID() << iPresentType << iPresentValue1 << iPresentValue2;

	SendAllServerAlarmMent( kPacket );
}

void UserNodeManager::AlarmEventInAwardLuckyPresentToAllUsers(User * pUser, bool bAlarm, int iPresentType, int iPresentValue1, int iPresentValue2)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_AWARD_LUCKY_PRESENT << pUser->GetPublicID() << iPresentType << iPresentValue1 << iPresentValue2;

	SendAllServerAlarmMent( kPacket );
}

void UserNodeManager::AlarmEventInExcavationToAllUsers(User * pUser, bool bAlarm, int iArtifactType, int iArtifactValue1, int iArtifactValue2, float fMapRate)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_EXCAVATION << pUser->GetPublicID() << iArtifactType << iArtifactValue1 << iArtifactValue2 << fMapRate;

	SendAllServerAlarmMent( kPacket );
}

// 2018-01-15 by bckim, 탐사 확장
void UserNodeManager::AlarmEventInExcavation_EX_ToAllUsers(User * pUser, BOOL bRoomAlarm, BOOL bWholeAlarm, int iRewardType, int iIndex, int iType, int iGrade, int iPrice, int iMultiple)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bWholeAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_EXCAVATION << pUser->GetPublicID() << iRewardType << iIndex << iType << iGrade << iPrice << iMultiple;
	
	SendAllServerAlarmMent( kPacket );
}
// End. 2018-01-15 by bckim, 탐사 확장

void UserNodeManager::AlarmEventInGashaponPresentToAllUsers(User * pUser, bool bAlarm, DWORD dwEtcItemType, int iPresentType, int iPresentValue1, int iPresentValue2)
{
#if !defined( USE_ALARM_BY_SERVER_SIDE )
	return;
#endif

	if (!bAlarm)
		return;

	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket << UDP_SERVER_ALARM_ETC_ITEM << pUser->GetPublicID() << iPresentType << iPresentValue1 << iPresentValue2 << dwEtcItemType;
	
	SendAllServerAlarmMent( kPacket );
}






