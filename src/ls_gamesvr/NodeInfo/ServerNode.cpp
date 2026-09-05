#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"

#include "../Network/GameServer.h"
#include "../Network/ioPacketQueue.h"
#include "../DataBase/DBClient.h"
#include "../MainServerNode/MainServerNode.h"
#include "../EtcHelpFunc.h"
#include "../NodeInfo/ioPresentHelper.h"
#include "../NodeInfo/TradeInfoManager.h"
#include "../NodeInfo/GuildRoomsBlockManager.h"
#include "../NodeInfo/TradeSyncManger.h"

#include "ioEtcItemManager.h"
#include "Room.h"
#include "RoomParent.h"
#include "ChannelNode.h"
#include "ServerNodeManager.h"
#include "RoomNodeManager.h"
#include "BattleRoomManager.h"
#include "LadderTeamManager.h"
#include "ShuffleRoomManager.h"
#include "ChannelNodeManager.h"
#include "MemoNodeManager.h"
#include "HeroRankManager.h"
#include "ShuffleRoomReserveMgr.h"

#include "ServerNode.h"

#include "ioSaleManager.h"
#include "../Local/ioLocalParent.h"
#include "../Local/ioLocalManager.h"
#include "../BillingRelayServer/BillingRelayServer.h"

#include "../QueryData/QueryResultData.h"

#include <strsafe.h>

extern CLog EventLOG;
extern CLog TradeLOG;
extern CLog WemadeLOG;

ServerNode::ServerNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize ), m_bBlockState(false), m_iPartitionIndex(0)
{
	InitData();
	InitUserMemoryPool();
	InitRoomMemoryPool();
	InitBattleRoomMemoryPool();
	InitChannelMemoryPool();
	InitLadderTeamMemoryPool();
	InitShuffleRoomMemoryPool();
	m_eSessionState = SS_DISCONNECT;
}

ServerNode::~ServerNode()
{	
	ReleaseUserMemoryPool();
	ReleaseRoomMemoryPool();
	ReleaseBattleRoomMemoryPool();
	ReleaseChannelMemoryPool();
	ReleaseLadderTeamMemoryPool();
	ReleaseShuffleRoomMemoryPool();
}

void ServerNode::InitUserMemoryPool()
{
	// Get MaxConnection
	int	MaxConnection	= g_UserNodeManager.GetMaxConnection();

	// MemPooler
	m_UserMemNode.CreatePool( 0, MaxConnection, TRUE );
	for( int i = 0 ; i < MaxConnection ; i++ )
		m_UserMemNode.Push( new UserCopyNode );
}

void ServerNode::ReleaseUserMemoryPool()
{
	ReturnUserMemoryPool();
	m_UserMemNode.DestroyPool();
}

void ServerNode::ReturnUserMemoryPool()
{
	uUserCopyNode_iter iter, iEnd;
	iEnd = m_UserCopyNode.end();
	for(iter = m_UserCopyNode.begin();iter != iEnd; ++iter)
	{
		UserCopyNode *pUserNode = iter->second;
		if( pUserNode )
		{
			//전투방 / 래더팀 퇴장
			g_BattleRoomManager.RemoveUserCopyNode( pUserNode->GetUserIndex(), pUserNode->GetPublicID() );
			g_LadderTeamManager.RemoveUserCopyNode( pUserNode->GetUserIndex(), pUserNode->GetPublicID() );
			g_ShuffleRoomManager.RemoveUserCopyNode( pUserNode->GetUserIndex(), pUserNode->GetPublicID() );
		}

		g_UserNodeManager.RemoveCopyUser( pUserNode->GetUserIndex() );

		pUserNode->OnDestroy();
		m_UserMemNode.Push( pUserNode );
	}	
	m_UserCopyNode.clear();
}

UserCopyNode *ServerNode::CreateNewUser( DWORD dwIndex )
{
	UserCopyNode *NewNode = GetUserNode( dwIndex );
	if( NewNode )
		return NewNode;

	//NewNode = (UserCopyNode*)m_UserMemNode.OutPoolAddr();
	NewNode = (UserCopyNode*)m_UserMemNode.Remove();
	if( !NewNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::CreateNewUser MemPool Zero!");
		return NULL;
	}

	NewNode->OnCreate( this );
	NewNode->SetUserIndex( dwIndex );
	m_UserCopyNode.insert( make_pair(dwIndex, NewNode) );
	return NewNode;
}

UserCopyNode *ServerNode::GetUserNode( DWORD dwIndex )
{
	uUserCopyNode_iter it = m_UserCopyNode.find( dwIndex );
	if(it != m_UserCopyNode.end())
	{
		return it->second;
	}
	return NULL;
}

//UserCopyNode *ServerNode::GetUserNode( const ioHashString &szUserID )
//{
//	uUserCopyNode_iter iter, iEnd;
//	iEnd = m_UserCopyNode.end();
//	for(iter = m_UserCopyNode.begin();iter != iEnd;++iter)
//	{
//		UserCopyNode *pUserNode = *iter;
//		if( pUserNode->GetPublicID() == szUserID )
//			return pUserNode;
//	}	
//	return NULL;
//}

void ServerNode::RemoveUserNode( DWORD dwIndex )
{
	uUserCopyNode_iter it = m_UserCopyNode.find( dwIndex );
	if(it != m_UserCopyNode.end())
	{
		UserCopyNode *pUserNode = it->second;
		if(!pUserNode)
		{
			m_UserCopyNode.erase( it );
			return;
		}

		g_UserNodeManager.RemoveCopyUser( pUserNode->GetUserIndex() );
		m_UserCopyNode.erase( it );
		pUserNode->OnDestroy();
		m_UserMemNode.Push( pUserNode );
	}
}

void ServerNode::RemoveUserNode( const ioHashString &szUserID )
{
	uUserCopyNode_iter iter, iEnd;
	iEnd = m_UserCopyNode.end();
	for(iter = m_UserCopyNode.begin();iter != iEnd;++iter)
	{
		UserCopyNode *pUserNode = iter->second;
		if( pUserNode->GetPublicID() == szUserID )
		{
			g_UserNodeManager.RemoveCopyUser( pUserNode->GetUserIndex() );
			pUserNode->OnDestroy();
			m_UserCopyNode.erase( iter );

			m_UserMemNode.Push( pUserNode );
			break;;
		}		
	}	
}

//int ServerNode::ExceptionRemoveUserNode( DWORD dwIndex )
//{
//	int iReturnCnt = 0;
//	uUserCopyNode_iter iter, iter_Prev;
//	for(iter = m_UserCopyNode.begin();iter != m_UserCopyNode.end();)
//	{
//		iter_Prev = iter;
//		UserCopyNode *pUserCopyNode = *iter_Prev;
//		if( pUserCopyNode->GetUserIndex() == dwIndex )
//		{
//			pUserCopyNode->OnDestroy();
//			iter = m_UserCopyNode.erase( iter_Prev );
//
//			m_UserMemNode.Push( pUserCopyNode );
//			iReturnCnt++;
//		}		
//		else
//		{
//			iter++;
//		}
//	}	
//
//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ExceptionRemoveUserNode(%d) : %d 실행 - %d개 삭제", m_dwServerIndex, dwIndex, iReturnCnt );
//	return iReturnCnt;
//}

void ServerNode::InitRoomMemoryPool()
{
	ioINILoader kLoader( "ls_config_game.ini" );
	int iMaxRoomNodePool = kLoader.LoadInt( "MemoryPool", "room_pool", 1000 );

	// MemPooler
	m_RoomMemNode.CreatePool( 0, iMaxRoomNodePool, TRUE );
	for( int i = 0 ; i < iMaxRoomNodePool ; i++ )
		m_RoomMemNode.Push( new RoomCopyNode );
 
	strcpy_s(m_szPublicIP,g_App.GetPublicIP().c_str());
}

void ServerNode::ReleaseRoomMemoryPool()
{
	ReturnRoomMemoryPool();
	m_RoomMemNode.DestroyPool();
}

void ServerNode::ReturnRoomMemoryPool()
{
	vRoomCopyNode_iter iter, iEnd;
	iEnd = m_RoomCopyNode.end();
	for(iter = m_RoomCopyNode.begin();iter != iEnd;++iter)
	{
		RoomCopyNode *pRoomNode = *iter;
		g_RoomNodeManager.RemoveCopyRoom( pRoomNode );
		pRoomNode->OnDestroy();

		m_RoomMemNode.Push( pRoomNode );
	}	
	m_RoomCopyNode.clear();
}

RoomCopyNode *ServerNode::CreateNewRoom( int iIndex )
{
	RoomCopyNode *NewNode = GetRoomNode( iIndex );
	if( NewNode )
		return NewNode;

	//NewNode = (RoomCopyNode*)m_RoomMemNode.OutPoolAddr();
	NewNode = (RoomCopyNode*)m_RoomMemNode.Remove();
	if( !NewNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::CreateNewRoom MemPool Zero!");
		return NULL;
	}

	NewNode->OnCreate( this );
	NewNode->SetRoomIndex( iIndex );
	m_RoomCopyNode.push_back( NewNode );
	return NewNode;
}

RoomCopyNode *ServerNode::GetRoomNode( int iIndex )
{
	vRoomCopyNode_iter iter, iEnd;
	iEnd = m_RoomCopyNode.end();
	for(iter = m_RoomCopyNode.begin();iter != iEnd;++iter)
	{
		RoomCopyNode *pRoomNode = *iter;
		if( pRoomNode->GetRoomIndex() == iIndex )
			return pRoomNode;
	}	
	return NULL;
}

void ServerNode::RemoveRoomNode( int iIndex )
{
	vRoomCopyNode_iter iter, iEnd;
	iEnd = m_RoomCopyNode.end();
	for(iter = m_RoomCopyNode.begin();iter != iEnd;++iter)
	{
		RoomCopyNode *pRoomNode = *iter;
		if( pRoomNode->GetRoomIndex() == iIndex )
		{
			g_RoomNodeManager.RemoveCopyRoom( pRoomNode );
			pRoomNode->OnDestroy();
			m_RoomCopyNode.erase( iter );

			m_RoomMemNode.Push( pRoomNode );
			return;
		}		
	}	
}

void ServerNode::RemoveRoomNode( RoomCopyNode *pRoomNode )
{
	vRoomCopyNode_iter iter, iEnd;
	iEnd = m_RoomCopyNode.end();
	for(iter = m_RoomCopyNode.begin();iter != iEnd;++iter)
	{
		RoomCopyNode *pRoomCopyNode = *iter;
		if( pRoomCopyNode == pRoomNode )
		{
			g_RoomNodeManager.RemoveCopyRoom( pRoomNode );
			pRoomCopyNode->OnDestroy();
			m_RoomCopyNode.erase( iter );

			m_RoomMemNode.Push( pRoomCopyNode );
			return;
		}		
	}	
}

void ServerNode::RemoveRoomCopyNodeAll()
{
	for(vRoomCopyNode_iter iter = m_RoomCopyNode.begin() ; iter != m_RoomCopyNode.end() ; ++iter)
	{
		RoomCopyNode *pRoomCopyNode = *iter;
		if( pRoomCopyNode )
		{
			g_RoomNodeManager.RemoveCopyRoom( pRoomCopyNode );

			pRoomCopyNode->OnDestroy();
			m_RoomMemNode.Push( pRoomCopyNode );
		}		
	}	
	m_RoomCopyNode.clear();
}

void ServerNode::InitBattleRoomMemoryPool()
{
	ioINILoader kLoader( "ls_config_game.ini" );
	int iMaxBattleRoomSize = kLoader.LoadInt( "MemoryPool", "battleroom_pool", 3000 );

	// MemPooler
	m_BattleRoomMemNode.CreatePool( 0, iMaxBattleRoomSize, TRUE );
	for( int i = 0 ; i < iMaxBattleRoomSize ; i++ )
		m_BattleRoomMemNode.Push( new BattleRoomCopyNode );
}

void ServerNode::ReleaseBattleRoomMemoryPool()
{
	ReturnBattleRoomMemoryPool();
	m_BattleRoomMemNode.DestroyPool();
}

void ServerNode::ReturnBattleRoomMemoryPool()
{
	vBattleRoomCopyNode_iter iter, iEnd;
	iEnd = m_BattleRoomCopyNode.end();
	for(iter = m_BattleRoomCopyNode.begin();iter != iEnd;++iter)
	{
		BattleRoomCopyNode *pBattleRoomNode = *iter;
		if( pBattleRoomNode )
			g_UserNodeManager.RemoveBattleRoomCopyNode( pBattleRoomNode->GetIndex() );

		g_BattleRoomManager.RemoveBattleCopyRoom( pBattleRoomNode );
		pBattleRoomNode->OnDestroy();

		m_BattleRoomMemNode.Push( pBattleRoomNode );
	}	
	m_BattleRoomCopyNode.clear();
}

BattleRoomCopyNode *ServerNode::CreateNewBattleRoom( DWORD dwIndex )
{
	BattleRoomCopyNode *NewNode = GetBattleRoomNode( dwIndex );
	if( NewNode )
		return NewNode;

	//NewNode = (BattleRoomCopyNode*)m_BattleRoomMemNode.OutPoolAddr();
	NewNode = (BattleRoomCopyNode*)m_BattleRoomMemNode.Remove();
	if( !NewNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::CreateNewBattleRoom MemPool Zero!");
		return NULL;
	}

	NewNode->OnCreate( this );
	NewNode->SetIndex( dwIndex );
	m_BattleRoomCopyNode.push_back( NewNode );
	return NewNode;
}

BattleRoomCopyNode *ServerNode::GetBattleRoomNode( DWORD dwIndex )
{
	vBattleRoomCopyNode_iter iter, iEnd;
	iEnd = m_BattleRoomCopyNode.end();
	for(iter = m_BattleRoomCopyNode.begin();iter != iEnd;++iter)
	{
		BattleRoomCopyNode *pBattleRoomNode = *iter;
		if( pBattleRoomNode->GetIndex() == dwIndex )
			return pBattleRoomNode;
	}	
	return NULL;
}

void ServerNode::RemoveBattleRoomNode( DWORD dwIndex )
{
	vBattleRoomCopyNode_iter iter, iEnd;
	iEnd = m_BattleRoomCopyNode.end();
	for(iter = m_BattleRoomCopyNode.begin();iter != iEnd;++iter)
	{
		BattleRoomCopyNode *pBattleRoomNode = *iter;
		if( pBattleRoomNode->GetIndex() == dwIndex )
		{
			g_BattleRoomManager.RemoveBattleCopyRoom( pBattleRoomNode );
			
			pBattleRoomNode->OnDestroy();
			m_BattleRoomMemNode.Push( pBattleRoomNode );

			m_BattleRoomCopyNode.erase( iter );
			return;
		}		
	}	
}

void ServerNode::RemoveBattleRoomNode( BattleRoomCopyNode *pBattleRoomNode )
{
	vBattleRoomCopyNode_iter iter, iEnd;
	iEnd = m_BattleRoomCopyNode.end();
	for(iter = m_BattleRoomCopyNode.begin();iter != iEnd;++iter)
	{
		BattleRoomCopyNode *pBattleRoomCopyNode = *iter;
		if( pBattleRoomCopyNode == pBattleRoomNode )
		{
			g_BattleRoomManager.RemoveBattleCopyRoom( pBattleRoomNode );

			pBattleRoomCopyNode->OnDestroy();
			m_BattleRoomMemNode.Push( pBattleRoomCopyNode );

			m_BattleRoomCopyNode.erase( iter );
			return;
		}		
	}	
}

void ServerNode::RemoveBattleRoomCopyNodeAll()
{
	for(vBattleRoomCopyNode_iter iter = m_BattleRoomCopyNode.begin() ; iter != m_BattleRoomCopyNode.end() ; ++iter)
	{
		BattleRoomCopyNode *pBattleRoomCopyNode = *iter;
		if( pBattleRoomCopyNode )
		{
			g_BattleRoomManager.RemoveBattleCopyRoom( pBattleRoomCopyNode );

			pBattleRoomCopyNode->OnDestroy();
			m_BattleRoomMemNode.Push( pBattleRoomCopyNode );
		}		
	}	

	m_BattleRoomCopyNode.clear();
}

//////////////////////////////////////////////////////////////////////////
void ServerNode::InitShuffleRoomMemoryPool()
{
	ioINILoader kLoader( "ls_config_game.ini" );
	int iMaxShuffleRoomSize = kLoader.LoadInt( "MemoryPool", "shuffleroom_pool", 3000 );

	// MemPooler
	m_ShuffleRoomMemNode.CreatePool( 0, iMaxShuffleRoomSize, TRUE );
	for( int i = 0 ; i < iMaxShuffleRoomSize ; i++ )
		m_ShuffleRoomMemNode.Push( new ShuffleRoomCopyNode );
}

void ServerNode::ReleaseShuffleRoomMemoryPool()
{
	ReturnShuffleRoomMemoryPool();
	m_ShuffleRoomMemNode.DestroyPool();
}

void ServerNode::ReturnShuffleRoomMemoryPool()
{
	vShuffleRoomCopyNode_iter iter, iEnd;
	iEnd = m_ShuffleRoomCopyNode.end();
	for(iter = m_ShuffleRoomCopyNode.begin();iter != iEnd;++iter)
	{
		ShuffleRoomCopyNode *pShuffleRoomNode = *iter;
		if( pShuffleRoomNode )
			g_UserNodeManager.RemoveShuffleRoomCopyNode( pShuffleRoomNode->GetIndex() );

		g_ShuffleRoomManager.RemoveShuffleCopyRoom( pShuffleRoomNode );
		pShuffleRoomNode->OnDestroy();

		m_ShuffleRoomMemNode.Push( pShuffleRoomNode );
	}	
	m_ShuffleRoomCopyNode.clear();
}

ShuffleRoomCopyNode *ServerNode::CreateNewShuffleRoom( DWORD dwIndex )
{
	ShuffleRoomCopyNode *NewNode = GetShuffleRoomNode( dwIndex );
	if( NewNode )
		return NewNode;

	NewNode = (ShuffleRoomCopyNode*)m_ShuffleRoomMemNode.Remove();
	if( !NewNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::CreateNewShuffleRoom MemPool Zero!");
		return NULL;
	}

	NewNode->OnCreate( this );
	NewNode->SetIndex( dwIndex );
	m_ShuffleRoomCopyNode.push_back( NewNode );
	return NewNode;
}

ShuffleRoomCopyNode *ServerNode::GetShuffleRoomNode( DWORD dwIndex )
{
	vShuffleRoomCopyNode_iter iter, iEnd;
	iEnd = m_ShuffleRoomCopyNode.end();
	for( iter = m_ShuffleRoomCopyNode.begin() ; iter != iEnd ; ++iter )
	{
		ShuffleRoomCopyNode *pShuffleRoomNode = *iter;
		if( pShuffleRoomNode->GetIndex() == dwIndex )
			return pShuffleRoomNode;
	}	
	return NULL;
}

void ServerNode::RemoveShuffleRoomNode( DWORD dwIndex )
{
	vShuffleRoomCopyNode_iter iter, iEnd;
	iEnd = m_ShuffleRoomCopyNode.end();
	for( iter = m_ShuffleRoomCopyNode.begin() ; iter != iEnd ; ++iter )
	{
		ShuffleRoomCopyNode *pShuffleRoomNode = *iter;
		if( pShuffleRoomNode->GetIndex() == dwIndex )
		{
			g_ShuffleRoomManager.RemoveShuffleCopyRoom( pShuffleRoomNode );

			pShuffleRoomNode->OnDestroy();
			m_ShuffleRoomMemNode.Push( pShuffleRoomNode );

			m_ShuffleRoomCopyNode.erase( iter );
			return;
		}		
	}	
}

void ServerNode::RemoveShuffleRoomNode( ShuffleRoomCopyNode *pShuffleRoomNode )
{
	vShuffleRoomCopyNode_iter iter, iEnd;
	iEnd = m_ShuffleRoomCopyNode.end();
	for( iter = m_ShuffleRoomCopyNode.begin() ; iter != iEnd ; ++iter )
	{
		ShuffleRoomCopyNode *pShuffleRoomCopyNode = *iter;
		if( pShuffleRoomCopyNode == pShuffleRoomNode )
		{
			g_ShuffleRoomManager.RemoveShuffleCopyRoom( pShuffleRoomNode );

			pShuffleRoomCopyNode->OnDestroy();
			m_ShuffleRoomMemNode.Push( pShuffleRoomCopyNode );

			m_ShuffleRoomCopyNode.erase( iter );
			return;
		}		
	}	
}

void ServerNode::RemoveShuffleRoomCopyNodeAll()
{
	for( vShuffleRoomCopyNode_iter iter = m_ShuffleRoomCopyNode.begin() ; iter != m_ShuffleRoomCopyNode.end() ; ++iter )
	{
		ShuffleRoomCopyNode *pShuffleRoomCopyNode = *iter;
		if( pShuffleRoomCopyNode )
		{
			g_ShuffleRoomManager.RemoveShuffleCopyRoom( pShuffleRoomCopyNode );

			pShuffleRoomCopyNode->OnDestroy();
			m_ShuffleRoomMemNode.Push( pShuffleRoomCopyNode );
		}		
	}	

	m_ShuffleRoomCopyNode.clear();
}

//////////////////////////////////////////////////////////////////////////
void ServerNode::InitChannelMemoryPool()
{
	ioINILoader kLoader( "ls_config_game.ini" );
	int iMaxRoomNodePool = kLoader.LoadInt( "MemoryPool", "channel_pool", 10000 );

	// MemPooler
	m_ChannelMemNode.CreatePool( 0, iMaxRoomNodePool, TRUE );
	for( int i = 0 ; i < iMaxRoomNodePool ; i++ )
		m_ChannelMemNode.Push( new ChannelCopyNode );
}

void ServerNode::ReleaseChannelMemoryPool()
{
	ReturnChannelMemoryPool();
	m_ChannelMemNode.DestroyPool();
}

void ServerNode::ReturnChannelMemoryPool()
{
	vChannelCopyNode_iter iter, iEnd;
	iEnd = m_ChannelCopyNode.end();
	for(iter = m_ChannelCopyNode.begin();iter != iEnd;++iter)
	{
		ChannelCopyNode *pChannelNode = *iter;
		if( pChannelNode )
			g_UserNodeManager.RemoveChannelCopyNode( pChannelNode->GetIndex() );

		g_ChannelNodeManager.RemoveCopyChannel( pChannelNode->GetIndex() );
		pChannelNode->OnDestroy();

		m_ChannelMemNode.Push( pChannelNode );
	}	
	m_ChannelCopyNode.clear();
}

ChannelCopyNode *ServerNode::CreateNewChannel( DWORD dwIndex )
{
	ChannelCopyNode *NewNode = GetChannelNode( dwIndex );
	if( NewNode )
		return NewNode;

	//NewNode = (ChannelCopyNode*)m_ChannelMemNode.OutPoolAddr();
	NewNode = (ChannelCopyNode*)m_ChannelMemNode.Remove();
	if( !NewNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::CreateNewChannel MemPool Zero!");
		return NULL;
	}

	NewNode->OnCreate( this );
	NewNode->SetIndex( dwIndex );
	m_ChannelCopyNode.push_back( NewNode );
	return NewNode;
}

ChannelCopyNode *ServerNode::GetChannelNode( DWORD dwIndex )
{
	vChannelCopyNode_iter iter, iEnd;
	iEnd = m_ChannelCopyNode.end();
	for(iter = m_ChannelCopyNode.begin();iter != iEnd;++iter)
	{
		ChannelCopyNode *pChannelNode = *iter;
		if( pChannelNode->GetIndex() == dwIndex )
			return pChannelNode;
	}	
	return NULL;
}

void ServerNode::RemoveChannelNode( DWORD dwIndex )
{
	vChannelCopyNode_iter iter, iEnd;
	iEnd = m_ChannelCopyNode.end();
	for(iter = m_ChannelCopyNode.begin();iter != iEnd;++iter)
	{
		ChannelCopyNode *pChannelNode = *iter;
		if( pChannelNode->GetIndex() == dwIndex )
		{
			g_ChannelNodeManager.RemoveCopyChannel( pChannelNode->GetIndex() );
			pChannelNode->OnDestroy();
			m_ChannelCopyNode.erase( iter );

			m_ChannelMemNode.Push( pChannelNode );
			return;
		}		
	}	
}

void ServerNode::RemoveChannelNode( ChannelCopyNode *pChannelNode )
{
	vChannelCopyNode_iter iter, iEnd;
	iEnd = m_ChannelCopyNode.end();
	for(iter = m_ChannelCopyNode.begin();iter != iEnd;++iter)
	{
		ChannelCopyNode *pChannelCopyNode = *iter;
		if( pChannelCopyNode == pChannelNode )
		{
			g_ChannelNodeManager.RemoveCopyChannel( pChannelNode->GetIndex() );
			pChannelCopyNode->OnDestroy();
			m_ChannelCopyNode.erase( iter );

			m_ChannelMemNode.Push( pChannelCopyNode );
			return;
		}		
	}	
}

void ServerNode::InitLadderTeamMemoryPool()
{
	ioINILoader kLoader( "ls_config_game.ini" );
	int iMaxLadderTeamSize = kLoader.LoadInt( "MemoryPool", "ladderteam_pool", 3000 );

	// MemPooler
	m_LadderTeamMemNode.CreatePool( 0, iMaxLadderTeamSize, TRUE );
	for( int i = 0 ; i < iMaxLadderTeamSize ; i++ )
		m_LadderTeamMemNode.Push( new LadderTeamCopyNode );
}

void ServerNode::ReleaseLadderTeamMemoryPool()
{
	ReturnLadderTeamMemoryPool();
	m_LadderTeamMemNode.DestroyPool();
}

void ServerNode::ReturnLadderTeamMemoryPool()
{
	vLadderTeamCopyNode_iter iter, iEnd;
	iEnd = m_LadderTeamCopyNode.end();
	for(iter = m_LadderTeamCopyNode.begin();iter != iEnd;++iter)
	{
		LadderTeamCopyNode *pLadderTeamNode = *iter;
		if( pLadderTeamNode )
			g_UserNodeManager.RemoveLadderTeamCopyNode( pLadderTeamNode->GetIndex() );

		g_LadderTeamManager.RemoveLadderTeamCopy( pLadderTeamNode );
		pLadderTeamNode->OnDestroy();

		m_LadderTeamMemNode.Push( pLadderTeamNode );
	}	
	m_LadderTeamCopyNode.clear();
}

LadderTeamCopyNode *ServerNode::CreateNewLadderTeam( DWORD dwIndex )
{
	LadderTeamCopyNode *NewNode = GetLadderTeamNode( dwIndex );
	if( NewNode )
		return NewNode;

	//NewNode = (LadderTeamCopyNode*)m_LadderTeamMemNode.OutPoolAddr();
	NewNode = (LadderTeamCopyNode*)m_LadderTeamMemNode.Remove();
	if( !NewNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::CreateNewLadderTeam MemPool Zero!");
		return NULL;
	}

	NewNode->OnCreate( this );
	NewNode->SetIndex( dwIndex );
	m_LadderTeamCopyNode.push_back( NewNode );
	return NewNode;
}

LadderTeamCopyNode *ServerNode::GetLadderTeamNode( DWORD dwIndex )
{
	vLadderTeamCopyNode_iter iter, iEnd;
	iEnd = m_LadderTeamCopyNode.end();
	for(iter = m_LadderTeamCopyNode.begin();iter != iEnd;++iter)
	{
		LadderTeamCopyNode *pLadderTeamNode = *iter;
		if( pLadderTeamNode->GetIndex() == dwIndex )
			return pLadderTeamNode;
	}	
	return NULL;
}

void ServerNode::RemoveLadderTeamNode( DWORD dwIndex )
{
	vLadderTeamCopyNode_iter iter, iEnd;
	iEnd = m_LadderTeamCopyNode.end();
	for(iter = m_LadderTeamCopyNode.begin();iter != iEnd;++iter)
	{
		LadderTeamCopyNode *pLadderTeamNode = *iter;
		if( pLadderTeamNode->GetIndex() == dwIndex )
		{
			g_LadderTeamManager.RemoveLadderTeamCopy( pLadderTeamNode );
			pLadderTeamNode->OnDestroy();
			m_LadderTeamCopyNode.erase( iter );

			m_LadderTeamMemNode.Push( pLadderTeamNode );
			return;
		}		
	}	
}

void ServerNode::RemoveLadderTeamNode( LadderTeamCopyNode *pLadderTeamNode )
{
	vLadderTeamCopyNode_iter iter, iEnd;
	iEnd = m_LadderTeamCopyNode.end();
	for(iter = m_LadderTeamCopyNode.begin();iter != iEnd;++iter)
	{
		LadderTeamCopyNode *pLadderTeamCopyNode = *iter;
		if( pLadderTeamCopyNode == pLadderTeamNode )
		{
			g_LadderTeamManager.RemoveLadderTeamCopy( pLadderTeamNode );
			pLadderTeamCopyNode->OnDestroy();
			m_LadderTeamCopyNode.erase( iter );

			m_LadderTeamMemNode.Push( pLadderTeamCopyNode );
			return;
		}		
	}	
}

void ServerNode::SetBlockState(const bool bBlockState)
{ 
	m_bBlockState = bBlockState; 

	// 하위 전투방정보에 블럭값을 설정
	for(vBattleRoomCopyNode_iter iter = m_BattleRoomCopyNode.begin() ;iter != m_BattleRoomCopyNode.end() ; ++iter)
	{
		BattleRoomCopyNode *pBattleRoomCopyNode = *iter;
		pBattleRoomCopyNode->SetBlockFlag( bBlockState );
	}	

	// 하위 셔플방정보에 블럭값을 설정
	for(vShuffleRoomCopyNode_iter iter = m_ShuffleRoomCopyNode.begin() ;iter != m_ShuffleRoomCopyNode.end() ; ++iter)
	{
		ShuffleRoomCopyNode *pShuffleRoomCopyNode = *iter;
		pShuffleRoomCopyNode->SetBlockFlag( bBlockState );
	}	

	// 하위 방(광장)정보에 블럭값을 설정
	for(vRoomCopyNode_iter iter = m_RoomCopyNode.begin() ; iter != m_RoomCopyNode.end() ; ++iter)
	{
		RoomCopyNode *pRoomCopyNode = *iter;
		if( pRoomCopyNode->GetRoomStyle() == RSTYLE_PLAZA )
		{
			pRoomCopyNode->SetBlockFlag( bBlockState );
		}	
	}
}

bool ServerNode::IsBusy()
{
	if( !IsConnectWorkComplete() )	return true;
	if( IsBlocked() )				return true;
	if( IsFull() )					return true;
	if( IsRoomCreateClose() )		return true;
	return false;
}

bool ServerNode::IsRoomCreateClose()
{
	if( m_dwRecvPingTime == 0 ) return false;

	DWORD dwPingGap = TIMEGETTIME() - m_dwRecvPingTime;
	if( dwPingGap >= g_ServerNodeManager.GetServerRoomCreateClosePingTime() ) 
		return true;          // 10초마다 핑 체크를 하므로 15초동안 응답이 안온 서버는 방생성을 일시 막음.
	return false;
}

bool ServerNode::IsFull()
{ 
	if( GetUserNodeSize() >= g_UserNodeManager.GetStableConnection() )
	{
		return true;
	}

	return false; 
}

bool ServerNode::IsMyPartition(const int iPartitionIndex)
{
	if( GetPartitionIndex() == iPartitionIndex )
	{
		return true;
	}
	return false;
}

int ServerNode::GetPlazaRoomCount()
{
	int iCount = 0;
	vRoomCopyNode_iter iter, iEnd;
	iEnd = m_RoomCopyNode.end();
	for(iter = m_RoomCopyNode.begin();iter != iEnd;++iter)
	{
		RoomCopyNode *pRoomCopyNode = *iter;
		if( pRoomCopyNode->GetRoomStyle() == RSTYLE_PLAZA )
		{
			if( pRoomCopyNode->GetModeType() == MT_TRAINING ) 
				iCount++;
		}	
	}	
	return iCount;
}

int ServerNode::GetHeadquartersRoomCount()
{
	int iCount = 0;
	vRoomCopyNode_iter iter, iEnd;
	iEnd = m_RoomCopyNode.end();
	for(iter = m_RoomCopyNode.begin();iter != iEnd;++iter)
	{
		RoomCopyNode *pRoomCopyNode = *iter;
		if( pRoomCopyNode->GetRoomStyle() == RSTYLE_HEADQUARTERS )
		{
			if( pRoomCopyNode->GetModeType() == MT_HEADQUARTERS || pRoomCopyNode->GetModeType() == MT_HOUSE ) 
				iCount++;
		}	
	}	
	return iCount;
}

int ServerNode::GetBattleRoomCount()
{
	return (int)m_BattleRoomCopyNode.size();
}

int ServerNode::GetLadderTeamCount( bool bHeroMode )
{
	if( bHeroMode )
		return GetLadderHeroModeCount();

	int iCount = (int)m_LadderTeamCopyNode.size() - GetLadderHeroModeCount();

	return iCount;
}

int ServerNode::GetLadderHeroModeCount()
{
	int iReturnNodeSize = 0;
	vLadderTeamCopyNode_iter iter, iEnd;
	iEnd = m_LadderTeamCopyNode.end();
	for(iter = m_LadderTeamCopyNode.begin();iter != iEnd;++iter)
	{
		LadderTeamCopyNode *pLadderTeamNode = *iter;
		if( pLadderTeamNode == NULL ) continue;
		if( pLadderTeamNode->IsHeroMatchMode() == false ) continue;

		iReturnNodeSize++;
	}	
	return iReturnNodeSize;
}

int ServerNode::GetShuffleRoomCount()
{
	return (int)m_BattleRoomCopyNode.size();
}

void ServerNode::InitData()
{
	m_eNodeType				= NODE_TYPE_NONE;
	m_eNodeRole				= NODE_ROLE_NONE;
	m_bBlockState			= false;
	m_dwServerIndex			= 0;
	m_iServerPort			= 0;
	m_iClientPort			= 0;
	m_bConnectWorkComplete	= false;
	m_dwPingTIme			= 0;
	m_dwRecvPingTime		= 0;
	m_eSessionState			= SS_DISCONNECT;
	m_iUserCount			= 0;
	m_iRelayIndex			= 0;
	m_iPartitionIndex		= 0;

	m_szServerIP.Clear();
	m_szClientMoveIP.Clear();
	m_ConnectWorkingPacket.clear();

	ZeroMemory(&m_RelayInfo,sizeof(m_RelayInfo));
}

void ServerNode::OnCreate()
{
	CConnectNode::OnCreate();
	InitData();
	m_eSessionState = SS_CONNECT;
}

void ServerNode::OnDestroy()
{
	g_CriticalError.CheckGameServerDisconnect( m_szServerIP, m_iServerPort );

	CConnectNode::OnDestroy();
	m_eSessionState = SS_DISCONNECT;
}

void ServerNode::OnSessionDestroy()
{
	ReturnUserMemoryPool();
	ReturnRoomMemoryPool();
	ReturnBattleRoomMemoryPool();
	ReturnChannelMemoryPool();
	ReturnLadderTeamMemoryPool();
	ReturnShuffleRoomMemoryPool();
}

void ServerNode::SessionClose(BOOL safely)
{
	if(!safely)
	{
		g_CriticalError.CheckGameServerExceptionDisconnect( GetServerIP(), GetServerPort(), GetLastError() );
	}

	CPacket packet(SSTPK_CLOSE);
	ReceivePacket( packet );
}

bool ServerNode::SendMessage( CPacket &rkPacket )
{
	g_ProcessChecker.ServerSendMessage( rkPacket.GetPacketID(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( rkPacket );
}

bool ServerNode::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int ServerNode::GetConnectType()
{
	return CONNECT_TYPE_SERVER;
}

void ServerNode::RequestJoin()
{
	// 연결 승인 정보 전송
	SP2Packet kPacket( SSTPK_CONNECT_INFO );
	kPacket << g_ServerNodeManager.GetServerIndex() << g_App.GetPrivateIP() << g_App.GetClientMoveIP() << g_App.GetSSPort() << g_App.GetCSPort();
	SendMessage( kPacket );
}

void ServerNode::RequestSync()
{
	// 오리지날 노드를 동기화 한다. 유저 - > 룸 - > 전투 룸 - > 채널 - > 길드팀 - > 셔플룸 - > 동기화 완료 순으로 전송.
	g_UserNodeManager.ConnectServerNodeSync( this );
	g_RoomNodeManager.ConnectServerNodeSync( this );
	g_BattleRoomManager.ConnectServerNodeSync( this );
	g_ChannelNodeManager.ConnectServerNodeSync( this );
	g_LadderTeamManager.ConnectServerNodeSync( this );
	g_ShuffleRoomManager.ConnectServerNodeSync( this );

	SP2Packet kPacket( SSTPK_CONNECT_SYNC );
	kPacket << SSTPK_CONNECT_SYNC_COMPLETE;
	SendMessage( kPacket );
}

void ServerNode::AfterJoinProcess()
{
	g_ServerNodeManager.CalculateMaxNode();
}

void ServerNode::ProcessPing()
{
	if( !IsActive() ) return;
	if( m_dwPingTIme == 0 ) return;
	
	if( TIMEGETTIME() - m_dwPingTIme > SERVER_PING_CHECK_TIME )       
	{
		m_dwPingTIme = TIMEGETTIME();
		SP2Packet kPacket( SSTPK_PING );
		SendMessage( kPacket );
	}
}

void ServerNode::SetNodeType( ServerNode::NodeTypes eType )
{
	m_eNodeType = eType;
}

void ServerNode::SetNodeRole( ServerNode::NodeRoles eType )
{
	m_eNodeRole = eType;
}

User *ServerNode::GetUserOriginalNode( DWORD dwUserIndex, SP2Packet &rkPacket )
{
	User *pUser = g_UserNodeManager.GetUserNode( dwUserIndex );
	if( pUser )
		return pUser;

	// 유저가 서버를 이동했거나 접속 종료
	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( pUserParent && !pUserParent->IsUserOriginal() )
	{
		UserCopyNode *pCopyNode = (UserCopyNode*)pUserParent;
		pCopyNode->SendMessage( rkPacket );
	}
	
	// 접속 종료는 그냥 NULL
	return NULL;
}

User *ServerNode::GetUserOriginalNode( const ioHashString &szPublicID, SP2Packet &rkPacket )
{
	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
		return pUser;

	// 유저가 서버를 이동했거나 접속 종료
	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( szPublicID );
	if( pUserParent && !pUserParent->IsUserOriginal() )
	{
		UserCopyNode *pCopyNode = (UserCopyNode*)pUserParent;
		pCopyNode->SendMessage( rkPacket );
	}

	// 접속 종료는 그냥 NULL
	return NULL;
}

User * ServerNode::GetUserOriginalNodeByPrivateID( const ioHashString &szPrivateID, SP2Packet &rkPacket )
{
	User *pUser = g_UserNodeManager.GetUserNodeByPrivateID( szPrivateID );
	if( pUser )
		return pUser;

	// 유저가 서버를 이동했거나 접속 종료
	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNodeByPrivateID( szPrivateID );
	if( pUserParent && !pUserParent->IsUserOriginal() )
	{
		UserCopyNode *pCopyNode = (UserCopyNode*)pUserParent;
		pCopyNode->SendMessage( rkPacket );
	}

	// 접속 종료는 그냥 NULL
	return NULL;
}

void ServerNode::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void ServerNode::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	// 패킷 로그 남기기
	/*{
		const char *p;
		GetStringOfSSTPKPacket(packet.GetPacketID(), p);

		std::string str;
		kPacket.GetStringOfStream(str);

		printf( "ShowLogOfPacket : Id: %s, %s, Stream: %s \n", "Game Server", p, str.c_str());
	}*/

	g_ProcessChecker.ServerPacketProcess( kPacket.GetPacketID() );

	FUNCTION_TIME_CHECKER( 500000.0f, kPacket.GetPacketID() );          // 0.5 초 이상 걸리면로그 남김
	
	if( OnConnectWorkingPacket( kPacket ) )
		return;

	if( OnUserTransferTCP( kPacket ) )
		return;

	if( OnWebUDPParse( kPacket ) )
		return;

	if( OnBillingParse( kPacket ) )
		return;

	switch( kPacket.GetPacketID() )
	{

	case SSTPK_CLOSE:
		OnClose( kPacket );
		break;
	case SSTPK_CONNECT_INFO:
		OnConnectInfo( kPacket );
		break;
	case SSTPK_CONNECT_SYNC:
		OnConnectSync( kPacket );
		break;
	case SSTPK_PING:
		OnPing( kPacket );
		break;
	case SSTPK_ROOM_SYNC:
		OnRoomSync( kPacket );
		break;
	case SSTPK_MOVING_ROOM:
		OnMovingRoom( kPacket );
		break;
	case SSTPK_MOVING_ROOM_RESULT:
		OnMovingRoomResult( kPacket );
		break;
	case SSTPK_USER_DATA_MOVE:
		OnUserDataMove( kPacket );
		break;
	case SSTPK_INVENTORY_DATA_MOVE_RESULT:
		OnUserInventoryMoveResult( kPacket );
		break;
	case SSTPK_EXTRAITEM_DATA_MOVE_RESULT:
		OnUserExtraItemMoveResult( kPacket );
		break;
	case SSTPK_QUEST_DATA_MOVE_RESULT:
		OnUserQuestMoveResult( kPacket );
		break;
	case SSTPK_USER_DATA_MOVE_RESULT:
		OnUserDataMoveResult( kPacket );
		break;
	case SSTPK_USER_SYNC:
		OnUserSync( kPacket );
		break;
	case SSTPK_USER_MOVE:
		OnUserMove( kPacket );
		break;
	case SSTPK_USER_FRIEND_MOVE:
		OnUserFriendMove( kPacket );
		break;
	case SSTPK_USER_FRIEND_MOVE_RESULT:
		OnUserFriendMoveResult( kPacket );
		break;
	case SSTPK_FOLLOW_USER:
		OnFollowUser( kPacket );
		break;
	case SSTPK_USER_POS_INDEX:
		OnUserPosIndex( kPacket );
		break;
	case SSTPK_CHANNEL_SYNC:
		OnChannelSync( kPacket );
		break;
	case SSTPK_CHANNEL_INVITE:
		OnChannelInvite( kPacket );
		break;
	case SSTPK_CHANNEL_CHAT:
		OnChannelChat( kPacket );
		break;
	case SSTPK_BATTLEROOM_SYNC:
		OnBattleRoomSync( kPacket );
		break;
	case SSTPK_BATTLEROOM_TRANSFER:
		OnBattleRoomTransfer( kPacket );
		break;
	case SSTPK_BATTLEROOM_JOIN_RESULT:
		OnBattleRoomJoinResult( kPacket );
		break;
	case SSTPK_BATTLEROOM_FOLLOW:
		OnBattleRoomFollow( kPacket );
		break;
	case SSTPK_PLAZAROOM_TRANSFER:
		OnPlazaRoomTransfer( kPacket );
		break;
	case SSTPK_USER_INFO_REFRESH:
		OnUserInfoRefresh( kPacket );
		break;
	case SSTPK_SIMPLE_USER_INFO_REFRESH:
		OnSimpleUserInfoRefresh( kPacket );
		break;
	case SSTPK_USER_CHAR_INFO_REFRESH:
		OnUserCharInfoRefresh( kPacket );
		break;
	case SSTPK_USER_CHAR_SUB_INFO_REFRESH:
		OnUserCharSubInfoRefresh( kPacket );
		break;
	case SSTPK_BATTLEROOM_KICK_OUT:
		OnBattleRoomKickOut( kPacket );
		break;
	case SSTPK_OFFLINE_MEMO:
		OnOfflineMemo( kPacket );
		break;
	case SSTPK_RESERVE_CREATE_ROOM:
		OnReserveCreateRoom( kPacket );
		break;
	case SSTPK_RESERVE_CREATE_BATTLEROOM:
		OnReserveCreateBattleRoom( kPacket );
		break;
	case SSTPK_RESERVE_CREATE_BATTLEROOM_RESULT:
		OnReserveCreateBattleRoomResult( kPacket );
		break;
	case SSTPK_EXCEPTION_BATTLEROOM_LEAVE:
		OnExceptionBattleRoomLeave( kPacket );
		break;
	case SSTPK_CREATE_GUILD_RESULT:
		OnCreateGuildResult( kPacket );
		break;
	case SSTPK_CREATE_GUILD_COMPLETE:
		OnCreateGuildComplete( kPacket );
		break;
	case SSTPK_GUILD_ENTRY_AGREE:
		OnGuildEntryAgree( kPacket );
		break;
	case SSTPK_GUILD_MEMBER_LIST_EX:
		OnGuildMemberListEx( kPacket );
		break;
	case SSTPK_GUILD_INVITATION:
		OnGuildInvitation( kPacket );
		break;
	case SSTPK_GUILD_MASTER_CHANGE:
		OnGuildMasterChange( kPacket );
		break;
	case SSTPK_GUILD_POSITION_CHANGE:
		OnGuildPositionChange( kPacket );
		break;
	case SSTPK_GUILD_KICK_OUT:
		OnGuildKickOut( kPacket );
		break;
	case SSTPK_FRIEND_DELETE:
		OnFriendDelete( kPacket );
		break;
	case SSTPK_GUILD_MARK_CHANGE:
		OnGuildMarkChange( kPacket );
		break;
	case SSTPK_GUILD_USER_DELETE:
		OnGuildUserDelete( kPacket );
		break;
	case SSTPK_LADDERTEAM_SYNC:
		OnLadderTeamSync( kPacket );
		break;
	case SSTPK_LADDERTEAM_TRANSFER:
		OnLadderTeamTransfer( kPacket );
		break;
	case SSTPK_EXCEPTION_LADDERTEAM_LEAVE:
		OnExceptionLadderTeamLeave( kPacket );
		break;
	case SSTPK_RESERVE_CREATE_LADDERTEAM:
		OnReserveCreateLadderTeam( kPacket );
		break;
	case SSTPK_RESERVE_CREATE_LADDERTEAM_RESULT:
		OnReserveCreateLadderTeamResult( kPacket );
		break;
	case SSTPK_LADDERTEAM_JOIN_RESULT:
		OnLadderTeamJoinResult( kPacket );
		break;
	case SSTPK_LADDERTEAM_KICK_OUT:
		OnLadderTeamKickOut( kPacket );
		break;
	case SSTPK_LADDERTEAM_FOLLOW:
		OnLadderTeamFollow( kPacket );
		break;
	case SSTPK_LADDERTEAM_ENTER_ROOM_USER:
		OnLadderTeamEnterRoomUser( kPacket );
		break;
	case SSTPK_CAMP_SEASON_BONUS:
		OnCampSeasonBonus( kPacket );
		break;
	case SSTPK_GUILD_NAME_CHANGE:
		OnGuildNameChange( kPacket );
		break;
	case SSTPK_GUILD_NAME_CHANGE_RESULT:
		OnGuildNameChangeResult( kPacket );
		break;
	case SSTPK_WHOLE_CHAT:
		OnWholeChat( kPacket );
		break;
	case SSTPK_UDP_RECV_TIMEOUT:
		OnUDPRecvTimeOut( kPacket );
		break;
	case SSTPK_SERVER_ALARM_MENT_UDP:
		OnServerAlarmMent( kPacket );
		break;
	case SSTPK_PRESENT_SELECT:
		OnPresentSelect( kPacket );
		break;
	case SSTPK_SUBSCRIPTION_SELECT:
		OnSubscriptionSelect( kPacket );
		break;
	case SSTPK_USER_HERO_DATA:
		OnUserHeroData( kPacket );
		break;
	case SSTPK_HERO_MATCH_OTHER_INFO:
		OnHeroMatchOtherInfo( kPacket );
		break;
	case SSTPK_USER_CHAR_RENTAL_AGREE:
		OnUserCharRentalAgree( kPacket );
		break;

	case SSTPK_TRADE_CREATE:
		OnTradeCreate( kPacket );
		break;
	case SSTPK_TRADE_CREATE_COMPLETE:
		OnTradeCreateComplete( kPacket );
		break;
	case SSTPK_TRADE_CREATE_FAIL:
		OnTradeCreateFail( kPacket );
		break;
	case SSTPK_TRADE_ITEM_COMPLETE:
		OnTradeItemComplete( kPacket );
		break;
	case SSTPK_TRADE_CANCEL:
		OnTradeCancel( kPacket );
		break;
	case SSTPK_TRADE_TIME_OUT:
		OnTradeTimeOut( kPacket );
		break;
	case SSTPK_EVENT_ITEM_INITIALIZE:
		OnEventItemInitialize( kPacket );
		break;

	case SSTPK_JOIN_HEADQUARTERS_USER:
		OnJoinHeadquartersUser( kPacket );
		break;
	case SSTPK_HEADQUARTERS_INFO:
		OnHeadquartersInfo( kPacket );
		break;
	case SSTPK_HEADQUARTERS_ROOM_INFO:
		OnHeadquartersRoomInfo( kPacket );
		break;
	case SSTPK_HEADQUARTERS_JOIN_AGREE:
		OnHeadquartersJoinAgree( kPacket );
		break;
	case SSTPK_LOGOUT_ROOM_ALARM:
		OnLogoutRoomAlarm( kPacket );
		break;
	case SSTPK_DISCONNECT_ALREADY_ID:
		OnDisconnectAlreadyID( kPacket );
		break;
		// 2020-06-05 by bckim, 하드시리얼 유저 제재
	case SSTPK_DISCONNECT_HDD_SERIAL_BLOCK:
		OnDisconnectHddSerialBlock( kPacket );
		break;
		// End. 2020-06-05 by bckim, 하드시리얼 유저 제재
	case SSTPK_ALCHEMIC_DATA_MOVE_RESULT:
		OnUserAlchemicMoveResult( kPacket );
		break;
	case LSPTK_CONNECT_REQUEST: // 로그인서버에서 처음 접속시 보내는 프로토콜
		OnLoginConnect( kPacket );
		break;
	case LSTPK_STATUS_REQUEST: // 로그인 서버가 게임서버 상태를 주기적으로 요청하는 프로토콜
		OnLoginRequestStatus();
		break;
	case LSPTK_BLOCK_REQUEST: // 로그인 서버가 게임서버 블록을 요청하는 프로토콜
		OnLoginBlockRequest( kPacket );
		break;
	case LSPTK_PARTITION_REQUEST: // 로그인 서버가 게임서버의 파티션 처리를 요청하는 프로토콜
		OnLoginPartitionRequest( kPacket );
		break;
	case SSTPK_TOURNAMENT_TEAM_CREATE:
		OnTournamentCreateTeam( kPacket );
		break;
	case SSTPK_TOURNAMENT_TEAM_ENTRY_AGREE_OK:
		OnTournamentTeamAgreeOK( kPacket );
		break;
	case SSTPK_TOURNAMENT_TEAM_JOIN:
		OnTournamentTeamJoin( kPacket );
		break;
	case SSTPK_TOURNAMENT_TEAM_LEAVE:
		OnTournamentTeamLeave( kPacket );
		break;
	case SSTPK_PRESENT_INSERT:
		OnPresentInsert( kPacket );
		break;
	case SSTPK_CLOVER_SEND:	// 친구가 보냄. 받은거
		OnCloverSend( kPacket );
		break;
	case RSPTK_ON_CONNECT:
		OnRelayServerConnect( kPacket);
		break;
		//relay 사용 안함!!
 	case RSTPK_ON_CONTROL:
 		OnRelayControl(kPacket);
 		break;
	case SSTPK_ETC_ITEM_SEND_PRESENT:
		OnPresentInsertByEtcItem( kPacket );
		break;
	case SSTPK_SHUFFLEROOM_TRANSFER:
		OnShuffleRoomTransfer( kPacket );
		break;
	case SSTPK_SHUFFLEROOM_JOIN_RESULT:
		OnShuffleRoomJoinResult( kPacket );
		break;
	case SSTPK_SHUFFLEROOM_SYNC:
		OnShuffleRoomSync( kPacket );
		break;
	case SSTPK_SHUFFLEROOM_KICK_OUT:
		OnShuffleRoomKickOut( kPacket );
		break;
	case SSTPK_EXCEPTION_SHUFFLEROOM_LEAVE:
		OnExceptionShuffleRoomLeave( kPacket );
		break;
	case SSTPK_SHUFFLEROOM_GLOBAL_CREATE:
		OnShuffleRoomGlobalCreate( kPacket );
		break;
	case SSTPK_PET_DATA_MOVE_RESULT:
		OnUserPetMoveResult( kPacket );
		break;
	case SSTPK_CHAR_AWAKE_MOVE_RESULT:
		OnUserCharAwakeMoveResult( kPacket );
		break;
	case SSTPK_NEW_PET_DATA_INFO:
		OnMoveNewPetData( kPacket ); 
		break;
	case SSTPK_SOLDIER_DATA_MOVE_RESULT:
		OnMoveSoldierData( kPacket );
		break;
	case SSTPK_ETCITEM_DATA_MOVE_RESULT:
		OnMoveEtcItemData( kPacket );
		break;
	case SSTPK_MEDALITEM_DATA_MOVE_RESULT:
		OnMoveMedalItemData( kPacket );
		break;
	case SSTPK_RAINBOW_WHOLE_CHAT:
		OnRainbowWholeChat( kPacket );
		break;
	case SSTPK_COSTUME_DATA_MOVE_RESULT:
		OnMoveCostumeData(kPacket);
		break;
	case SSTPK_COSTUME_ADD:
		OnCostumeAdd(kPacket);
		break;
	case SSTPK_ACCESSORY_ADD:
		OnAccessoryAdd(kPacket);
		break;
	case SSTPK_MISSION_DATA_MOVE_RESULT:
		OnMoveMissionData(kPacket);
		break;
	case SSTPK_ROOLBOOK_DATA_MOVE_RESULT:
		OnMoveRollBookData(kPacket);
		break;
	case SSTPK_ACCESSORY_DATA_MOVE_RESULT:
		OnMoveAccessroyData(kPacket);
		break;
	case SSTPK_RESERVE_CREATE_GUILD_ROOM:
		OnReserveCreateGuildRoom(kPacket);
		break;
	case SSTPK_ACTIVE_GUILD_ROOM:
		OnChangeGuildRoomStatus(kPacket);
		break;
	case SSTPK_JOIN_PERSONAL_HQ_USER:
		OnJoinPersonalHQUser( kPacket );
		break;
	case SSTPK_PERSONAL_HQ_INFO:
		OnPersonalHQInfo( kPacket );
		break;
	case SSTPK_PERSONAL_HQ_ROOM_INFO:
		OnPersonalHQRoomInfo( kPacket );
		break;
	case SSTPK_PERSONAL_HQ_JOIN_AGREE:
		OnPersonalHQJoinAgree(kPacket);
		break;
	case SSTPK_PERSONAL_HQ_DATA_MOVE_RESULT:
		OnMovePersonalHQData(kPacket);
		break;
	case SSTPK_PERSONAL_HQ_ADD_BLOCK:
		OnPersonalHQAddBlock(kPacket);
		break;
	case SSTPK_TIMECASH_DATA_MOVE_RESULT:
		OnMoveTimeCashData(kPacket);
		break;
	case SSTPK_UPDATE_TIME_CASH:
		OnUpdateTimeCashInfo(kPacket);
		break;
	case SSTPK_TITLE_MOVE_RESULT:
		OnMoveTitleData(kPacket);
		break;
	case SSTPK_TITLE_UPDATE:
		OnTitleUpdate(kPacket);
		break;
	case SSTPK_BONUS_CASH_MOVE_RESULT:
		OnMoveBonusCashData(kPacket);
		break;
	case SSTPK_BONUS_CAHSH_ADD:
		OnResultBonusCashAdd(kPacket);
		break;
	case SSTPK_BONUS_CASH_UPDATE:
		OnResultBonusCashUpdate(kPacket);
		break;
	case SSTPK_OAK_BARREL_GET_INFO:
		OnResultOakBarrelGetInfo( kPacket );
		break;
	case SSTPK_OAK_BARREL_UPDATE_INFO:
		OnResultOakBarrelUpdateInfo( kPacket );
		break;
	case SSTPK_OAK_BARREL_UPDATE_TIME:
		OnResultOakBarrelUpdateTime( kPacket );
		break;
	case SSTPK_OAK_BARREL_UPDATE_LIMIT_SWORD:
		OnResultOakBarrelUpdateLimitSword( kPacket );
		break;
	case SSTPK_USER_MATCH_UPDATE:
		OnResultUserMatchUpdate( kPacket );
		break;
	case SSTPK_MATCH_LOG:
		OnResultUserMatchLog( kPacket );
		break;
	case SSTPK_LADDER_MATCH_TIME:
		OnSetLadderMatchTime( kPacket );
		break;
		// 2019-02-14 by bckim, 배틀 모드 추가
	case SSTPK_BATTLE_MODE_ORDER:				// 2019-02-14 by bckim, 배틀 모드 추가
		OnSetBattleModeOrder( kPacket );		// 2019-02-14 by bckim, 배틀 모드 추가
		break;			
	case SSTPK_SUCCESSION_OTHER_USER_INFO:
		OnSuccessionUserInfo( kPacket );
		break;
	case SSTPK_CUSTOM_MEDAL_GET_INFO:
		OnMoveCustomMedalData( kPacket );
		break;
	case SSTPK_CUSTOM_MEDAL_ADD:
		OnCustomMedalAdd( kPacket );
		break;
	case SSTPK_SYNC_PRACTICE_ADMISSION:
		OnUserPracticeAdmission( kPacket );
		break;
	case SSTPK_SYNC_PRACTICE_TIME_UPDATE:
		OnUserPracticeTime( kPacket );
		break;
// 2018-08-30 by bckim, 주사위 이벤트 추가
	case SSTPK_DICE_GAME_GET_INFO:
		OnResultDiceGameGetInfo( kPacket );
		break;
// End. 2018-08-30 by bckim, 주사위 이벤트 추가
	default:
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
		break;
	}
}

bool ServerNode::OnConnectWorkingPacket( SP2Packet &rkPacket )
{
	if( m_bConnectWorkComplete )			return false;
	if( IsLoginNode() || IsRelayNode() )	return false;

	//서버 실행 동기화중 받아서 처리해야할 패킷
	switch( rkPacket.GetPacketID() )
	{
	case SSTPK_CONNECT_INFO:
	case SSTPK_CONNECT_SYNC:
	case SSTPK_PING:
	case LSPTK_CONNECT_REQUEST:
	case RSPTK_ON_CONNECT:
		return false;
	}

	//서버 실행 동기화중 받는 패킷은 동기화 종료후 처리
	m_ConnectWorkingPacket.push_back( rkPacket );
	return true;
}

bool ServerNode::OnUserTransferTCP( SP2Packet &rkPacket )
{
	/* 날중계 */
	if( COMPARE( rkPacket.GetPacketID(), STPK_RESERVE_LOGOUT, STPK_ABSTRACT + 1 ) )
	{
		DWORD dwUserIndex;
		rkPacket >> dwUserIndex;
		User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
		if( pUser )
		{
			// 유저 인덱스 삭제
			SP2Packet kPacket( rkPacket.GetPacketID() );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + sizeof( DWORD ), rkPacket.GetDataSize() - sizeof( DWORD ) );
			pUser->SendMessage( kPacket );
		}
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserTransferTCP Not Node PacketID: 0x%x", rkPacket.GetPacketID() );
		return true;
	}	
	else if( COMPARE( rkPacket.GetPacketID(), CUPK_CONNECT, 0x5000 ) )
	{
		DWORD dwUserIndex;
		rkPacket >> dwUserIndex;
		User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
		if( pUser )
		{
			// 유저 인덱스 삭제
			SP2Packet kPacket( rkPacket.GetPacketID() );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + sizeof( DWORD ), rkPacket.GetDataSize() - sizeof( DWORD ) );
			pUser->SendMessage( kPacket );
		}
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserTransferTCP Not Node PacketID: 0x%x", rkPacket.GetPacketID() );
		return true;
	}	
	return false;
}

void ServerNode::OnClose( SP2Packet &rkPacket )
{
	if(IsRelayNode())
	{
		g_Relay.DelRelayServerInfo(this);
		g_UserNodeManager.SendNotUseRelayMessageAll(m_iRelayIndex);
	}
	if( !IsDisconnectState() )
	{
		OnDestroy();
		OnSessionDestroy();
		
		g_ServerNodeManager.RemoveNode( this );
	}
}

void ServerNode::OnConnectInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_dwServerIndex >> m_szServerIP >> m_szClientMoveIP >> m_iServerPort >> m_iClientPort;
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][server]OnConnect :[%d] [%s] [%s] [%d] [%d] [%d]", m_dwServerIndex, m_szServerIP.c_str(), m_szClientMoveIP.c_str(), m_iServerPort, m_iClientPort, GetNodeRole() );
	
	if( GetNodeRole() == NODE_ROLE_CLIENT )
	{
		//Information( "클라이언트 노드 %s:%d\r\n", m_szServerIP.c_str(), m_iServerPort );
		SetNodeType( NODE_TYPE_GAME );
		RequestJoin();
		AfterJoinProcess();
	}	
	else if( GetNodeRole() == NODE_ROLE_SERVER )
	{
		//Information( "서버 노드 %s:%d\r\n", m_szServerIP.c_str(), m_iServerPort );
		SetNodeType( NODE_TYPE_GAME );
		RequestSync();
		AfterJoinProcess();

		m_bConnectWorkComplete = true;
	}
	
	m_dwPingTIme = TIMEGETTIME();	
}

void ServerNode::OnConnectSync( SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;
	switch( iSyncType )
	{
	case SSTPK_CONNECT_SYNC_USER:
		{
			int iSize;
			rkPacket >> iSize;
			for(int i = 0;i < iSize;i++)
			{
				DWORD dwUserIndex;
				rkPacket >> dwUserIndex;
				UserCopyNode *pNewNode = CreateNewUser( dwUserIndex );
				if( pNewNode )
				{
					pNewNode->ApplySyncCreate( rkPacket );
					g_UserNodeManager.AddCopyUser( pNewNode );
				}
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][server]Differ gameServer UserInfo : [%d] [%d]", m_dwServerIndex, iSize );
		}
		break;
	case SSTPK_CONNECT_SYNC_ROOM:
		{
			int iSize;
			rkPacket >> iSize;
			for(int i = 0;i < iSize;i++)
			{
				int iRoomIndex;
				rkPacket >> iRoomIndex;
				RoomCopyNode *pNewNode = CreateNewRoom( iRoomIndex );
				if( pNewNode )
				{
					pNewNode->ApplySyncCreate( rkPacket );
					g_RoomNodeManager.AddCopyRoom( pNewNode );
				}
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][server]Differ gameServer RoomInfo : [%d] [%d]", m_dwServerIndex, iSize );
		}
		break;
	case SSTPK_CONNECT_SYNC_BATTLEROOM:
		{
			int iSize;
			rkPacket >> iSize;
			for(int i = 0;i < iSize;i++)
			{
				int iBattleRoomIndex;
				rkPacket >> iBattleRoomIndex;
				BattleRoomCopyNode *pNewNode = CreateNewBattleRoom( iBattleRoomIndex );
				if( pNewNode )
				{
					pNewNode->ApplySyncCreate( rkPacket );
					g_BattleRoomManager.AddCopyBattleRoom( pNewNode );
				}
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][server]Differ gameServer BattleRoomInfo : [%d] [%d]", m_dwServerIndex, iSize );
		}
		break;
	case SSTPK_CONNECT_SYNC_CHANNEL:
		{
			int iSize;
			rkPacket >> iSize;
			for(int i = 0;i < iSize;i++)
			{
				DWORD dwChannelIndex;
				rkPacket >> dwChannelIndex;
				ChannelCopyNode *pNewNode = CreateNewChannel( dwChannelIndex );
				if( pNewNode )
				{
					g_ChannelNodeManager.AddCopyChannel( pNewNode );
				}
			}			
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][server]Differ GameServer ChannelInfo : [%d] [%d]", m_dwServerIndex, iSize );
		}
		break;
	case SSTPK_CONNECT_SYNC_LADDERTEAM:
		{
			int iSize;
			rkPacket >> iSize;
			for(int i = 0;i < iSize;i++)
			{
				DWORD dwLadderTeamIndex;
				rkPacket >> dwLadderTeamIndex;
				LadderTeamCopyNode *pNewNode = CreateNewLadderTeam( dwLadderTeamIndex );
				if( pNewNode )
				{
					pNewNode->ApplySyncCreate( rkPacket );
					g_LadderTeamManager.AddCopyLadderTeam( pNewNode );
				}
			}
			g_LadderTeamManager.SortLadderTeamRank( true );
			g_LadderTeamManager.SortLadderTeamRank( false );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][server]Differ GameServer LadderTeamInfo : [%d] [%d]", m_dwServerIndex, iSize );
		}
		break;
	case SSTPK_CONNECT_SYNC_SHUFFLEROOM:
		{
			int iSize;
			rkPacket >> iSize;
			for(int i = 0;i < iSize;i++)
			{
				int iShuffleRoomIndex;
				rkPacket >> iShuffleRoomIndex;
				ShuffleRoomCopyNode *pNewNode = CreateNewShuffleRoom( iShuffleRoomIndex );
				if( pNewNode )
				{
					pNewNode->ApplySyncCreate( rkPacket );
					g_ShuffleRoomManager.AddCopyShuffleRoom( pNewNode );
				}
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][server]Differ GameServer ShuffleRoomInfo : [%d] [%d]", m_dwServerIndex, iSize );
		}
		break;
	case SSTPK_CONNECT_SYNC_COMPLETE:
		{
			// 동기화 완료
			m_bConnectWorkComplete = true;
			g_ServerNodeManager.IsConnectWorkComplete();

			// 동기화중 받은 패킷 처리
			if( !m_ConnectWorkingPacket.empty() )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][server]Differ GameServer Connect Sync Complete PacketParsing : [%d] [%d]", m_dwServerIndex, (int)m_ConnectWorkingPacket.size() );
				SP2PacketList::iterator iter;
				for( iter=m_ConnectWorkingPacket.begin() ; iter!=m_ConnectWorkingPacket.end() ; ++iter )
				{
					PacketParsing( *iter );
				}
				m_ConnectWorkingPacket.clear();
			}
		}
		break;
	}
}

void ServerNode::OnPing( SP2Packet &rkPacket )
{
	m_dwRecvPingTime = TIMEGETTIME();
}

void ServerNode::OnRoomSync( SP2Packet &rkPacket )
{
	int   iSyncType, iRoomIndex;
	rkPacket >> iSyncType >> iRoomIndex;

	switch( iSyncType )
	{
	case RoomSync::RS_MODE:
		{
			RoomCopyNode *pCopyNode = GetRoomNode( iRoomIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncMode( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnRoomSync Not Room : %d - %d - %s:%d", (int)RoomSync::RS_MODE, iRoomIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case RoomSync::RS_CURUSER:
		{
			RoomCopyNode *pCopyNode = GetRoomNode( iRoomIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncCurUser( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnRoomSync Not Room : %d - %d - %s:%d", (int)RoomSync::RS_CURUSER, iRoomIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case RoomSync::RS_PLAZAINFO:
		{
			RoomCopyNode *pCopyNode = GetRoomNode( iRoomIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncPlazaInfo( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnRoomSync Not Room : %d - %d - %s:%d", (int)RoomSync::RS_PLAZAINFO, iRoomIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case RoomSync::RS_CREATE:
		{
			RoomCopyNode *pNewNode = CreateNewRoom( iRoomIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncCreate( rkPacket );
				g_RoomNodeManager.AddCopyRoom( pNewNode );
			}
		}
		break;
	case RoomSync::RS_DESTROY:
		RemoveRoomNode( iRoomIndex );
		break; 
	}
}

void ServerNode::OnMovingRoom( SP2Packet &rkPacket )
{
	int iMoveType, iRoomIndex, iUserIndex;
	rkPacket >> iMoveType >> iRoomIndex >> iUserIndex;

	switch( iMoveType )
	{
	case SS_MOVING_ROOM_JOIN_PLAZA:
		{ 
			int iMatchingLevel;
			rkPacket >> iMatchingLevel;
			Room *pRoom = NULL;
			if( iRoomIndex == -1 )
				pRoom = g_RoomNodeManager.GetJoinPlazaNode( iMatchingLevel, NULL );
			else
				pRoom = g_RoomNodeManager.GetJoinPlazaNodeByNum( iRoomIndex );

			if( pRoom )
			{
				if( pRoom->IsRoomFull() )       
				{
					// 인원 제한
					SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
					kPacket << SS_MOVING_ROOM_ERROR << iMoveType << SEARCH_TRAINING_ERROR_3 << iRoomIndex << iUserIndex;
					SendMessage( kPacket );					
				}
				else
				{
					// 임시 입장이 되면 JoinRoom 전송하여 서버 이동을 시작한다.
					pRoom->EnterReserveUser( iUserIndex );
					SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
					kPacket << SS_MOVING_ROOM_JOIN << iMoveType << (int)pRoom->GetModeType() << pRoom->GetRoomIndex() << iUserIndex;	
					kPacket << pRoom->GetModeSubNum() << pRoom->GetModeMapNum() << (int)pRoom->GetPlazaModeType() << pRoom->GetRoomNumber();
					SendMessage( kPacket );
				}
			}
			else
			{
				// 룸 없어졌음
				SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
				kPacket << SS_MOVING_ROOM_ERROR << iMoveType << SEARCH_TRAINING_ERROR_4 << iRoomIndex << iUserIndex;
				SendMessage( kPacket );
			}
		}
		break;
	case SS_MOVING_ROOM_EXIT_PLAZA:
		{
			int iNewResult;
			bool bPenalty;
			rkPacket >> iNewResult >> bPenalty;

			Room *pRoom = g_RoomNodeManager.GetJoinPlazaNodeByNum( iRoomIndex );			
			if( pRoom && !pRoom->IsRoomFull() )
			{
				// 임시 입장이 되면 JoinRoom 전송하여 서버 이동을 시작한다.
				pRoom->EnterReserveUser( iUserIndex );
				SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
				kPacket << SS_MOVING_ROOM_JOIN << iMoveType << iNewResult << bPenalty << (int)pRoom->GetModeType() << pRoom->GetRoomIndex() << iUserIndex;	
				kPacket << pRoom->GetModeSubNum() << pRoom->GetModeMapNum() << (int)pRoom->GetPlazaModeType() << pRoom->GetRoomNumber();
				SendMessage( kPacket );
			}
			else
			{
				// 룸 없어졌음
				SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
				kPacket << SS_MOVING_ROOM_ERROR << iMoveType << iNewResult << bPenalty << iUserIndex;
				SendMessage( kPacket );
			}
		}
		break;
	case SS_MOVING_ROOM_JOIN_HEADQUARTERS:
		{ 
			bool bInvited;
			int iMapIndex;
			DWORD dwOwnerUser;
			rkPacket >> dwOwnerUser >> iMapIndex >> bInvited;
			RoomParent *pRoomParent = g_RoomNodeManager.GetHeadquartersGlobalNode( iRoomIndex );
			if( pRoomParent )
			{				
				if( pRoomParent->IsRoomFull() )       
				{
					// 인원 제한
					UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( iUserIndex );
					if( pUserParent )
					{
						SP2Packet kPacket( STPK_JOIN_HEADQUARTERS );
						kPacket << JOIN_HEADQUARTERS_ROOM_FULL;
						pUserParent->RelayPacket( kPacket );
					}
				}
				else if( pRoomParent->IsRoomOriginal() )
				{
					Room *pRoom = static_cast< Room * >( pRoomParent );

					// 입장
					UserParent *pRequestUser = g_UserNodeManager.GetGlobalUserNode( iUserIndex );
					if( pRequestUser == NULL )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMovingRoom - %d - pRequestUser NULL(%d).", pRoomParent->GetRoomIndex(), iUserIndex );
					}
					else if( pRequestUser->IsUserOriginal() )
					{				
						User *pUser = static_cast< User * >( pRequestUser );

						SP2Packet kPacket( STPK_JOIN_ROOM );   
						kPacket << JOIN_ROOM_OK;
						kPacket << pRoom->GetModeType();
						kPacket << pRoom->GetModeSubNum();
						kPacket << pRoom->GetModeMapNum();
						kPacket << pRoom->GetRoomNumber();
						kPacket << (int)pRoom->GetPlazaModeType();
						pUser->SendMessage( kPacket );
						pUser->EnterRoom( pRoom );
					}
					else
					{
						pRoom->EnterReserveUser( pRequestUser->GetUserIndex() );

						// 해당 유저에게 입장 알림
						UserCopyNode *pUser = static_cast< UserCopyNode * >( pRequestUser );

						SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
						kPacket << SS_MOVING_ROOM_JOIN << SS_MOVING_ROOM_JOIN_HEADQUARTERS << (int)pRoom->GetModeType() << pRoom->GetRoomIndex() << pUser->GetUserIndex();	
						kPacket << pRoom->GetModeSubNum() << pRoom->GetModeMapNum();
						pUser->SendMessage( kPacket );
					}
				}
				else
				{
					// 복사 룸이다 예외 처리
					UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( iUserIndex );
					if( pUserParent )
					{
						SP2Packet kPacket( STPK_JOIN_HEADQUARTERS );
						kPacket << JOIN_HEADQUARTERS_EXCEPTION;
						pUserParent->RelayPacket( kPacket );
					}
				}
			}
			else
			{
				// 룸 없어졌음
				SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
				kPacket << SS_MOVING_ROOM_ERROR << iMoveType << iRoomIndex << iUserIndex << dwOwnerUser << iMapIndex << bInvited;
				SendMessage( kPacket );
			}
		}
		break;
	case SS_MOVING_ROOM_JOIN_HEADQUARTERS_AGREE:
		{ 
			RoomParent *pRoomParent = g_RoomNodeManager.GetHeadquartersGlobalNode( iRoomIndex );
			if( pRoomParent )
			{				
				if( pRoomParent->IsRoomFull() )       
				{
					// 인원 제한
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnMovingRoom - %d - SS_MOVING_ROOM_JOIN_HEADQUARTERS_AGREE Full(%d).", pRoomParent->GetRoomIndex(), iUserIndex );
				}
				else if( pRoomParent->IsRoomOriginal() )
				{
					Room *pRoom = static_cast< Room * >( pRoomParent );

					// 입장
					UserParent *pRequestUser = g_UserNodeManager.GetGlobalUserNode( iUserIndex );
					if( pRequestUser == NULL )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMovingRoom - %d - pRequestUser NULL(%d).", pRoomParent->GetRoomIndex(), iUserIndex );
					}
					else if( pRequestUser->IsUserOriginal() )
					{				
						User *pUser = static_cast< User * >( pRequestUser );

						SP2Packet kPacket( STPK_JOIN_ROOM );   
						kPacket << JOIN_ROOM_OK;
						kPacket << pRoom->GetModeType();
						kPacket << pRoom->GetModeSubNum();
						kPacket << pRoom->GetModeMapNum();
						kPacket << pRoom->GetRoomNumber();
						kPacket << (int)pRoom->GetPlazaModeType();
						pUser->SendMessage( kPacket );
						pUser->EnterRoom( pRoom );
					}
					else
					{
						pRoom->EnterReserveUser( pRequestUser->GetUserIndex() );

						// 해당 유저에게 입장 알림
						UserCopyNode *pUser = static_cast< UserCopyNode * >( pRequestUser );

						SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
						kPacket << SS_MOVING_ROOM_JOIN << SS_MOVING_ROOM_JOIN_HEADQUARTERS << (int)pRoom->GetModeType() << pRoom->GetRoomIndex() << pUser->GetUserIndex();	
						kPacket << pRoom->GetModeSubNum() << pRoom->GetModeMapNum();
						pUser->SendMessage( kPacket );
					}
				}
				else
				{					
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnMovingRoom - %d - SS_MOVING_ROOM_JOIN_HEADQUARTERS_AGREE -6.", iUserIndex );
				}
			}
			else
			{
				// 룸 없어졌음
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnMovingRoom - %d - SS_MOVING_ROOM_JOIN_HEADQUARTERS_AGREE.", iUserIndex );
			}
		}
		break;
	case SS_MOVING_ROOM_JOIN_PERSONAL_HQ:
		{ 
			bool bInvited		= false;
			int iMapIndex		= 0;
			DWORD dwOwnerUser	= 0;

			PACKET_GUARD_VOID( rkPacket.Read(dwOwnerUser) );
			PACKET_GUARD_VOID( rkPacket.Read(iMapIndex) );
			PACKET_GUARD_VOID( rkPacket.Read(bInvited) );

			RoomParent *pRoomParent = g_RoomNodeManager.GetHeadquartersGlobalNode( iRoomIndex );
			if( pRoomParent )
			{				
				if( pRoomParent->IsRoomFull() )       
				{
					// 인원 제한
					UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( iUserIndex );
					if( pUserParent )
					{
						SP2Packet kPacket( STPK_JOIN_PERSONAL_HQ );
						PACKET_GUARD_VOID( kPacket.Write(JOIN_PERSONAL_HQ_ROOM_FULL) );
						pUserParent->RelayPacket( kPacket );
					}
				}
				else if( pRoomParent->IsRoomOriginal() )
				{
					Room *pRoom = static_cast< Room * >( pRoomParent );

					// 입장
					UserParent *pRequestUser = g_UserNodeManager.GetGlobalUserNode( iUserIndex );
					if( pRequestUser == NULL )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMovingRoom - %d - pRequestUser NULL(%d).", pRoomParent->GetRoomIndex(), iUserIndex );
					}
					else if( pRequestUser->IsUserOriginal() )
					{				
						User *pUser = static_cast< User * >( pRequestUser );

						SP2Packet kPacket( STPK_JOIN_ROOM );   
						kPacket << JOIN_ROOM_OK;
						kPacket << pRoom->GetModeType();
						kPacket << pRoom->GetModeSubNum();
						kPacket << pRoom->GetModeMapNum();
						kPacket << pRoom->GetRoomNumber();
						kPacket << (int)pRoom->GetPlazaModeType();
						pUser->SendMessage( kPacket );
						pUser->EnterRoom( pRoom );
					}
					else
					{
						pRoom->EnterReserveUser( pRequestUser->GetUserIndex() );

						// 해당 유저에게 입장 알림
						UserCopyNode *pUser = static_cast< UserCopyNode * >( pRequestUser );

						SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
						kPacket << SS_MOVING_ROOM_JOIN << SS_MOVING_ROOM_JOIN_PERSONAL_HQ << (int)pRoom->GetModeType() << pRoom->GetRoomIndex() << pUser->GetUserIndex();	
						kPacket << pRoom->GetModeSubNum() << pRoom->GetModeMapNum();
						pUser->SendMessage( kPacket );
					}
				}
				else
				{
					// 복사 룸이다 예외 처리
					UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( iUserIndex );
					if( pUserParent )
					{
						SP2Packet kPacket( STPK_JOIN_PERSONAL_HQ );
						kPacket << JOIN_PERSONAL_HQ_EXCEPTION;
						pUserParent->RelayPacket( kPacket );
					}
				}
			}
			else
			{
				// 룸 없어졌음
				SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
				kPacket << SS_MOVING_ROOM_ERROR << iMoveType << iRoomIndex << iUserIndex << dwOwnerUser << iMapIndex << bInvited;
				SendMessage( kPacket );
			}
		}
		break;
	case SS_MOVING_ROOM_JOIN_PERSONAL_HQ_AGREE:
		{ 
			RoomParent *pRoomParent = g_RoomNodeManager.GetHeadquartersGlobalNode( iRoomIndex );
			if( pRoomParent )
			{				
				if( pRoomParent->IsRoomFull() )       
				{
					// 인원 제한
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnMovingRoom - %d - SS_MOVING_ROOM_JOIN_PERSONAL_HQ_AGREE Full(%d).", pRoomParent->GetRoomIndex(), iUserIndex );
				}
				else if( pRoomParent->IsRoomOriginal() )
				{
					Room *pRoom = static_cast< Room * >( pRoomParent );

					// 입장
					UserParent *pRequestUser = g_UserNodeManager.GetGlobalUserNode( iUserIndex );
					if( pRequestUser == NULL )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMovingRoom - %d - pRequestUser NULL(%d).", pRoomParent->GetRoomIndex(), iUserIndex );
					}
					else if( pRequestUser->IsUserOriginal() )
					{				
						User *pUser = static_cast< User * >( pRequestUser );

						SP2Packet kPacket( STPK_JOIN_ROOM );   
						kPacket << JOIN_ROOM_OK;
						kPacket << pRoom->GetModeType();
						kPacket << pRoom->GetModeSubNum();
						kPacket << pRoom->GetModeMapNum();
						kPacket << pRoom->GetRoomNumber();
						kPacket << (int)pRoom->GetPlazaModeType();
						pUser->SendMessage( kPacket );
						pUser->EnterRoom( pRoom );
					}
					else
					{
						pRoom->EnterReserveUser( pRequestUser->GetUserIndex() );

						// 해당 유저에게 입장 알림
						UserCopyNode *pUser = static_cast< UserCopyNode * >( pRequestUser );

						SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
						kPacket << SS_MOVING_ROOM_JOIN << SS_MOVING_ROOM_JOIN_PERSONAL_HQ << (int)pRoom->GetModeType() << pRoom->GetRoomIndex() << pUser->GetUserIndex();	
						kPacket << pRoom->GetModeSubNum() << pRoom->GetModeMapNum();
						pUser->SendMessage( kPacket );
					}
				}
				else
				{					
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnMovingRoom - %d - SS_MOVING_ROOM_JOIN_PERSONAL_HQ_AGREE -6.", iUserIndex );
				}
			}
			else
			{
				// 룸 없어졌음
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnMovingRoom - %d - SS_MOVING_ROOM_JOIN_PERSONAL_HQ_AGREE.", iUserIndex );
			}
		}
		break;
	default:
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMovingRoom 알수없는 요청 : %d", iMoveType );
		}
		break;
	}
}

void ServerNode::_OnMovingRoomOK( int iMoveType, SP2Packet &rkPacket )
{
	switch( iMoveType )
	{
	case SS_MOVING_ROOM_JOIN_PLAZA:
		{
			int iModeType, iRoomIndex, iUserIndex;
			rkPacket >> iModeType >> iRoomIndex >> iUserIndex;
			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );
			if( pUser )
			{
				if( pUser->GetCharCount() == 0 )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "용병이 없어 본부로 이동시킴 SS_MOVING_ROOM_JOIN_PLAZA : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_NOT_CHAR, false );
				}
				else if( pUser->GetEntryType() == ET_TERMINATION )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "임시가입 만료 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_PLAZA : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_ENTRY_NOT_FORMALITY, false );
				}
				else if( pUser->IsNewPublicID() )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "이름변경 예약한 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_PLAZA : %d|%s", pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_RESERVED_CHANGE_ID, false );
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "_OnMovingRoomOK Plaza : %s, %s:%d - > %s:%d", pUser->GetPublicID().c_str(), g_App.GetClientMoveIP().c_str(), g_App.GetCSPort(), 
											GetServerIP().c_str(), GetClientPort() );
					int iModeSubNum, iModeMapNum, iRoomNumber, iPlazaType;
					rkPacket >> iModeSubNum >> iModeMapNum >> iPlazaType >> iRoomNumber;

					int iMovingValue = pUser->SetServerMoving();
					SP2Packet kPacket( STPK_MOVING_SERVER );   
					kPacket << iModeType << iModeSubNum << iModeMapNum << iPlazaType << iRoomNumber << iRoomIndex;
					kPacket << GetClientMoveIP() << GetClientPort();
					kPacket << iMovingValue << g_ServerNodeManager.GetServerIndex();
					pUser->SendMessage( kPacket );			
				}				
			}		
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomOK Not User SS_MOVING_ROOM_JOIN_PLAZA: %d", iUserIndex );
		}
		break;
	case SS_MOVING_ROOM_EXIT_PLAZA:
		{
			bool bPenalty;
			int iNewResult, iModeType, iRoomIndex, iUserIndex;
			rkPacket >> iNewResult >> bPenalty >> iModeType >> iRoomIndex >> iUserIndex;
			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );
			if( pUser )
			{
				if( pUser->GetCharCount() == 0 )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "용병이 없어 본부로 이동시킴 SS_MOVING_ROOM_EXIT_PLAZA : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_NOT_CHAR, false );
				}
				else if( pUser->GetEntryType() == ET_TERMINATION )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "임시가입 만료 유저로 본부로 이동시킴 SS_MOVING_ROOM_EXIT_PLAZA : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_ENTRY_NOT_FORMALITY, false );
				}
				else if( pUser->IsNewPublicID() )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "이름변경 예약한 유저로 본부로 이동시킴 SS_MOVING_ROOM_EXIT_PLAZA : %d|%s", pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_RESERVED_CHANGE_ID, false );
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "_OnMovingRoomOK  Plaza : %s, %s:%d - > %s:%d", pUser->GetPublicID().c_str(), g_App.GetClientMoveIP().c_str(), g_App.GetCSPort(), 
											GetServerIP().c_str(), GetClientPort() );
					int iModeSubNum, iModeMapNum, iRoomNumber, iPlazaType;
					rkPacket >> iModeSubNum >> iModeMapNum >> iPlazaType >> iRoomNumber;

					SP2Packet kPacket1( STPK_EXIT_ROOM );
					kPacket1 << iNewResult << iModeSubNum << bPenalty << iRoomNumber << iModeMapNum << iPlazaType;
					pUser->SendMessage( kPacket1 );

					int iMovingValue = pUser->SetServerMoving();
					SP2Packet kPacket2( STPK_MOVING_SERVER );   
					kPacket2 << iModeType << iModeSubNum << iModeMapNum << iPlazaType << iRoomNumber << iRoomIndex << GetClientMoveIP() << GetClientPort();
					kPacket2 << iMovingValue << g_ServerNodeManager.GetServerIndex();
					pUser->SendMessage( kPacket2 );			
				}				
			}		
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomOK Not User SS_MOVING_ROOM_EXIT_PLAZA: %d", iUserIndex );
		}
		break;
	case SS_MOVING_ROOM_JOIN_BATTLE:
		{
			int iModeType, iRoomIndex, iUserIndex;
			rkPacket >> iModeType >> iRoomIndex >> iUserIndex;
			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );
			if( pUser )
			{
				if( pUser->GetCharCount() == 0 )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "용병이 없어 본부로 이동시킴 SS_MOVING_ROOM_JOIN_BATTLE : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_NOT_CHAR, false );
				}
				else if( pUser->GetEntryType() == ET_TERMINATION )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "임시가입 만료 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_BATTLE : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_ENTRY_NOT_FORMALITY, false );
				}
				else if( pUser->IsNewPublicID() )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "이름변경 예약한 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_BATTLE : %d|%s", pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_RESERVED_CHANGE_ID, false );
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "_OnMovingRoomOK Battle : %s, %s:%d - > %s:%d", pUser->GetPublicID().c_str(), g_App.GetClientMoveIP().c_str(), g_App.GetCSPort(), 
											GetServerIP().c_str(), GetClientPort() );

					int iModeSubNum, iModeMapNum, iRoomNumber, iPlazaType;
					rkPacket >> iModeSubNum >>  iModeMapNum >> iPlazaType >> iRoomNumber;

					{
						// 유저 로딩 상태 돌입
						SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
						kPacket << BATTLEROOM_READY_GO_OK << iModeType << iModeSubNum << iModeMapNum;
						pUser->SendMessage( kPacket );
					}
					// 서버 이동 신호 전송 
					int iMovingValue = pUser->SetServerMoving();
					SP2Packet kMovePacket( STPK_MOVING_SERVER );   
					kMovePacket << iModeType << iModeSubNum << iModeMapNum << iPlazaType << iRoomNumber << iRoomIndex;
					kMovePacket << GetClientMoveIP() << GetClientPort();
					kMovePacket << iMovingValue << g_ServerNodeManager.GetServerIndex();
					pUser->SendMessage( kMovePacket );			
				}
			}		
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomOK Not User SS_MOVING_ROOM_JOIN_BATTLE: %d", iUserIndex );
		}
		break;
	case SS_MOVING_ROOM_JOIN_LADDER:
		{
			int iModeType, iRoomIndex, iUserIndex;
			rkPacket >> iModeType >> iRoomIndex >> iUserIndex;
			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );


			if( pUser )
			{
				if( pUser->GetCharCount() == 0 )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "용병이 없어 본부로 이동시킴 SS_MOVING_ROOM_JOIN_LADDER : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_NOT_CHAR, false );
				}
				else if( pUser->GetEntryType() == ET_TERMINATION )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "임시가입만료 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_LADDER : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_ENTRY_NOT_FORMALITY, false );
				}
				else if( pUser->IsNewPublicID() )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "이름변경 예약한 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_LADDER : %d|%s", pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_RESERVED_CHANGE_ID, false );
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "_OnMovingRoomOK LadderBattle : %s, %s:%d - > %s:%d", pUser->GetPublicID().c_str(), g_App.GetClientMoveIP().c_str(), g_App.GetCSPort(), 
											GetServerIP().c_str(), GetClientPort() );

					//HRYOON 1124
					g_LogDBClient.OnInsertTime(pUser, LogDBClient::TT_LADDER_MATCH );

					bool bReadyGoSend = false;
					int iModeSubNum = 0;
					int iModeMapNum = 0;
					int iRoomNumber = 0;
					int iPlazaType = 0;
					int iCampType = 0;
					int iTeamType = 0;

					PACKET_GUARD_VOID( rkPacket.Read(iModeSubNum) );
					PACKET_GUARD_VOID( rkPacket.Read(iModeMapNum) );
					PACKET_GUARD_VOID( rkPacket.Read(iPlazaType) );
					PACKET_GUARD_VOID( rkPacket.Read(iRoomNumber) );
					PACKET_GUARD_VOID( rkPacket.Read(bReadyGoSend) );
					PACKET_GUARD_VOID( rkPacket.Read(iCampType) );
					PACKET_GUARD_VOID( rkPacket.Read(iTeamType) );
					
					if( bReadyGoSend )
					{
						// 유저 로딩 상태 돌입
						SP2Packet kPacket( STPK_LADDERTEAM_MACRO );
						PACKET_GUARD_VOID( kPacket.Write(LADDERTEAM_MACRO_MODE_READY_GO) );
						PACKET_GUARD_VOID( kPacket.Write(iModeType) );
						PACKET_GUARD_VOID( kPacket.Write(iModeSubNum) );
						PACKET_GUARD_VOID( kPacket.Write(iModeMapNum) );
						PACKET_GUARD_VOID( kPacket.Write(iCampType) );
						PACKET_GUARD_VOID( kPacket.Write(iTeamType) );

						pUser->SendMessage( kPacket );
					}

					// 서버 이동 신호 전송 
					int iMovingValue = pUser->SetServerMoving();
					SP2Packet kMovePacket( STPK_MOVING_SERVER );   
					kMovePacket << iModeType << iModeSubNum << iModeMapNum << iPlazaType << iRoomNumber << iRoomIndex;
					kMovePacket << GetClientMoveIP() << GetClientPort();
					kMovePacket << iMovingValue << g_ServerNodeManager.GetServerIndex();
					pUser->SendMessage( kMovePacket );			
				}
			}		
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomOK Not User SS_MOVING_ROOM_JOIN_LADDER: %d", iUserIndex );
		}
		break;
	case SS_MOVING_ROOM_JOIN_HEADQUARTERS:
		{
			int iModeType, iRoomIndex, iUserIndex, iModeSubNum, iModeMapNum;
			rkPacket >> iModeType >> iRoomIndex >> iUserIndex >> iModeSubNum >> iModeMapNum;
			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );
			if( pUser )
			{
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "_OnMovingRoomOK Headquarters : %s, %s:%d - > %s:%d", pUser->GetPublicID().c_str(), g_App.GetPublicIP().c_str(), g_App.GetCSPort(), 
																					          GetServerIP().c_str(), GetClientPort() );
				int iMovingValue = pUser->SetServerMoving();
				SP2Packet kPacket( STPK_MOVING_SERVER );   
				kPacket << iModeType << iModeSubNum << iModeMapNum << 0 << 0 << iRoomIndex;
				kPacket << GetClientMoveIP() << GetClientPort();
				kPacket << iMovingValue << g_ServerNodeManager.GetServerIndex();
				pUser->SendMessage( kPacket );
			}		
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomOK Not User SS_MOVING_ROOM_JOIN_HEADQUARTERS: %d", iUserIndex );
		}
		break;
	case SS_MOVING_ROOM_JOIN_SHUFFLE:
		{
			int iModeType, iRoomIndex, iUserIndex;
			rkPacket >> iModeType >> iRoomIndex >> iUserIndex;
			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );
			if( pUser )
			{
				if( pUser->GetCharCount() == 0 )
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "용병이 없어 본부로 이동시킴 SS_MOVING_ROOM_JOIN_SHUFFLE : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_NOT_CHAR, false );
				}
				else if( pUser->GetEntryType() == ET_TERMINATION )
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "임시가입 만료 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_SHUFFLE : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_ENTRY_NOT_FORMALITY, false );
				}
				else if( pUser->IsNewPublicID() )
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "이름변경 예약한 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_SHUFFLE : %d|%s", pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_RESERVED_CHANGE_ID, false );
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomOK Shuffle : %s, %s:%d - > %s:%d", pUser->GetPublicID().c_str(), g_App.GetClientMoveIP().c_str(), g_App.GetCSPort(), GetServerIP().c_str(), GetClientPort() );

					int iModeSubNum, iModeMapNum, iRoomNumber, iPlazaType;
					rkPacket >> iModeSubNum >>  iModeMapNum >> iPlazaType >> iRoomNumber;

					{
						// 유저 로딩 상태 돌입
						SP2Packet kPacket( STPK_SHUFFLEROOM_COMMAND );
						kPacket << SHUFFLEROOM_READY_GO_OK << iModeType << iModeSubNum << iModeMapNum;
						pUser->SendMessage( kPacket );
					}
					// 서버 이동 신호 전송 
					int iMovingValue = pUser->SetServerMoving();
					SP2Packet kMovePacket( STPK_MOVING_SERVER );   
					kMovePacket << iModeType << iModeSubNum << iModeMapNum << iPlazaType << iRoomNumber << iRoomIndex;
					kMovePacket << GetClientMoveIP() << GetClientPort();
					kMovePacket << iMovingValue << g_ServerNodeManager.GetServerIndex();
					pUser->SendMessage( kMovePacket );
				}
			}		
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomOK Not User SS_MOVING_ROOM_JOIN_SHUFFLE: %d", iUserIndex );
		}
		break;
	case SS_MOVING_ROOM_JOIN_PERSONAL_HQ:
		{
			int iModeType	= 0, iRoomIndex	= 0, iUserIndex	= 0, iModeSubNum	= 0, iModeMapNum	= 0;
			rkPacket >> iModeType >> iRoomIndex >> iUserIndex >> iModeSubNum >> iModeMapNum;

			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );
			if( pUser )
			{
				int iMovingValue = pUser->SetServerMoving();
				SP2Packet kPacket( STPK_MOVING_SERVER );   
				kPacket << iModeType << iModeSubNum << iModeMapNum << 0 << 0 << iRoomIndex;
				kPacket << GetClientMoveIP() << GetClientPort();
				kPacket << iMovingValue << g_ServerNodeManager.GetServerIndex();
				pUser->SendMessage( kPacket );
			}		
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomOK Not User SS_MOVING_ROOM_JOIN_HEADQUARTERS: %d", iUserIndex );
		}
		break;
	case SS_MOVING_ROOM_JOIN_MATCH:
		{
			int iModeType, iRoomIndex, iUserIndex;
			rkPacket >> iModeType >> iRoomIndex >> iUserIndex;
			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );
			if( pUser )
			{
				if( pUser->GetCharCount() == 0 )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "용병이 없어 본부로 이동시킴 SS_MOVING_ROOM_JOIN_BATTLE : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_NOT_CHAR, false );
				}
				else if( pUser->GetEntryType() == ET_TERMINATION )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "임시가입 만료 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_BATTLE : %s", pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_ENTRY_NOT_FORMALITY, false );
				}
				else if( pUser->IsNewPublicID() )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "이름변경 예약한 유저로 본부로 이동시킴 SS_MOVING_ROOM_JOIN_BATTLE : %d|%s", pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
					pUser->ExitRoomToTraining( EXIT_ROOM_RESERVED_CHANGE_ID, false );
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "_OnMovingRoomOK Match : %s, %s:%d - > %s:%d", pUser->GetPublicID().c_str(), g_App.GetClientMoveIP().c_str(), g_App.GetCSPort(), 
											GetServerIP().c_str(), GetClientPort() );

					int iModeSubNum, iModeMapNum, iRoomNumber, iPlazaType;
					bool bRevenge = false;
					PACKET_GUARD_VOID( rkPacket.Read(iModeSubNum) );
					PACKET_GUARD_VOID( rkPacket.Read(iModeMapNum) );
					PACKET_GUARD_VOID( rkPacket.Read(iPlazaType) );
					PACKET_GUARD_VOID( rkPacket.Read(iRoomNumber) );
					PACKET_GUARD_VOID( rkPacket.Read(bRevenge) );

					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[info][1v1] match enter room and move server[%d] user[%d] ", iRoomIndex, iUserIndex );

					{
						// 유저 로딩 상태 돌입
						SP2Packet kPacket( STPK_SUCCESSION_MATCHING_GAMESTART );
						PACKET_GUARD_VOID( kPacket.Write(iModeType) );
						PACKET_GUARD_VOID( kPacket.Write(iModeSubNum) );
						PACKET_GUARD_VOID( kPacket.Write(iModeMapNum) );
						PACKET_GUARD_VOID( kPacket.Write(bRevenge) );
						pUser->SendMessage( kPacket );

						g_LogDBClient.OnInsertMatchInfo( pUser, pUser->GetStartTier(), pUser->GetEndTier(), pUser->GetUserMatch()->GetMatchDelayTime(), 
							pUser->GetUserMatch()->GetMMR(),
							pUser->GetUserMatch()->GetMaxWinCount(), LogDBClient::MT_SUCCESS, iRoomIndex, (int)bRevenge );
					}

					// 서버 이동 신호 전송 
					int iMovingValue = pUser->SetServerMoving();
					SP2Packet kMovePacket( STPK_MOVING_SERVER );   
					PACKET_GUARD_VOID( kMovePacket.Write(iModeType) );
					PACKET_GUARD_VOID( kMovePacket.Write(iModeSubNum) );
					PACKET_GUARD_VOID( kMovePacket.Write(iModeMapNum) );
					PACKET_GUARD_VOID( kMovePacket.Write(iPlazaType) );
					PACKET_GUARD_VOID( kMovePacket.Write(iRoomNumber) );
					PACKET_GUARD_VOID( kMovePacket.Write(iRoomIndex) );
					PACKET_GUARD_VOID( kMovePacket.Write(GetClientMoveIP()) );
					PACKET_GUARD_VOID( kMovePacket.Write(GetClientPort()) );
					PACKET_GUARD_VOID( kMovePacket.Write(iMovingValue) );
					PACKET_GUARD_VOID( kMovePacket.Write(g_ServerNodeManager.GetServerIndex()) );
					pUser->SendMessage( kMovePacket );			
				}
			}		
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomOK Not User SS_MOVING_ROOM_JOIN_MATCH: %d", iUserIndex );
		}
		break;
	}        
}

void ServerNode::_OnMovingRoomError( int iMoveType, SP2Packet &rkPacket )
{
	switch( iMoveType )
	{
	case SS_MOVING_ROOM_JOIN_PLAZA:
		{
			int iResult, iRoomIndex, iUserIndex;
			rkPacket >> iResult >> iRoomIndex >> iUserIndex;
			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );
			if( pUser )
			{
				SP2Packet kPacket( STPK_SEARCH_PLAZA_ROOM );
				kPacket << iResult;
				pUser->SendMessage( kPacket );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomError SS_MOVING_ROOM_JOIN_PLAZA :%d Not User", iUserIndex );
		}
		break;
	case SS_MOVING_ROOM_EXIT_PLAZA:
		{
			int  iNewResult, iUserIndex;
			bool bPenalty;
			rkPacket >> iNewResult >> bPenalty >> iUserIndex;
			User *pUser = GetUserOriginalNode( iUserIndex, rkPacket );
			if( pUser )
			{
				// 룸 이탈 후 결과(페널티/감점)으로 무조건 넣어줘야한다.
				Room *pRoom = g_RoomNodeManager.GetExitRoomJoinPlazaNode( pUser->GetKillDeathLevel() );
				if( pRoom )
				{
					SP2Packet kPacket( STPK_EXIT_ROOM );
					kPacket << iNewResult;
					kPacket << pRoom->GetModeSubNum();
					kPacket << bPenalty;
					kPacket << pRoom->GetRoomNumber();
					pUser->SendMessage( kPacket );
					pUser->EnterRoom( pRoom );
				}
				else      //본부
				{
					pUser->ExitRoomToTraining( EXIT_ROOM_LOBBY, false );
				}
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMovingRoomError SS_MOVING_ROOM_EXIT_PLAZA :%d Not User", iUserIndex );
		}
		break;
	case SS_MOVING_ROOM_JOIN_HEADQUARTERS:
		{
			bool bInvited;
			DWORD dwRequestUser, dwOwnerUser;
			int iRequestRoomIndex, iMapIndex;
			rkPacket >> iRequestRoomIndex >> dwRequestUser >> dwOwnerUser >> iMapIndex >> bInvited;

			RoomParent *pRoomParent = g_RoomNodeManager.GetHeadquartersGlobalNode( iRequestRoomIndex );
			if( pRoomParent )
			{
				// -_-; 룸이 없다고해서 왔는데 여기는 있단다....예외 처리
				UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
				if( pUserParent )
				{
					SP2Packet kPacket( STPK_JOIN_HEADQUARTERS );
					kPacket << JOIN_HEADQUARTERS_EXCEPTION;
					pUserParent->RelayPacket( kPacket );
				}
			}
			else
			{
				UserParent *pRequestParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
				if( pRequestParent == NULL )
					return;

				UserParent *pOwnerParent = g_UserNodeManager.GetGlobalUserNode( dwOwnerUser );
				if( pOwnerParent == NULL )
				{
					// 오너가 오프라인
					SP2Packet kPacket( STPK_JOIN_HEADQUARTERS );
					kPacket << JOIN_HEADQUARTERS_OWNER_OFFLINE;
					pRequestParent->RelayPacket( kPacket );
				}
				else if( pOwnerParent->IsUserOriginal() )
				{
					User *pUser = static_cast< User * >( pOwnerParent );
					pUser->_OnJoinHeadquarters( pRequestParent, iMapIndex, bInvited );
				}
				else
				{
					UserCopyNode *pUser = static_cast< UserCopyNode * >( pOwnerParent );

					SP2Packet kPacket( SSTPK_JOIN_HEADQUARTERS_USER );
					kPacket << pRequestParent->GetUserIndex() << pUser->GetUserIndex() << iMapIndex << bInvited;
					pUser->SendMessage( kPacket );
				}
			}
		}
		break;
	}
}

void ServerNode::OnMovingRoomResult( SP2Packet &rkPacket )
{
	int iMovingResult, iMoveType;
	rkPacket >> iMovingResult >> iMoveType;
	switch( iMovingResult )
	{
	case SS_MOVING_ROOM_ERROR:
		_OnMovingRoomError( iMoveType, rkPacket );
		break;
	case SS_MOVING_ROOM_JOIN:
		_OnMovingRoomOK( iMoveType, rkPacket );
		break;
	}
}

void ServerNode::SendDecoMoveData( int iMovingValue, User *pUser   )
{
	int iStartIndex = 0;
	int iPageCount = pUser->GetDecoPageCount();

	if( iPageCount > 0 )
	{
		ioInventory* pInventory = pUser->GetInventory();

		if( !pInventory )
			return;

		int iAddIndex = DB_DECO_SELECT_COUNT;

		for( int i = 0; i < iPageCount; i++ )
		{
			SP2Packet kPacket( SSTPK_INVENTORY_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );

			pUser->FillInventoryMoveData( kPacket, iStartIndex );
			SendMessage( kPacket );

			iStartIndex += iAddIndex; 
		}
	}

}

void ServerNode::SendExtraItemMoveData( int iMovingValue, User *pUser )
{
	int iStartRow = 0;
	int iPageCount = pUser->GetExtraItemPageCount();

	if( iPageCount > 0 )
	{
		ioUserExtraItem* pInventory = pUser->GetUserExtraItem();

		if( !pInventory )
			return;

		int iAddRow = DB_EXTRAITEM_SELECT_COUNT;

		for( int i = 0; i < iPageCount; i++ )
		{
			SP2Packet kPacket( SSTPK_EXTRAITEM_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );

			pUser->FillExtraItemMoveData( kPacket, iStartRow );
			SendMessage( kPacket );

			iStartRow += iAddRow; 
		}
	}
	else
	{
		SP2Packet kPacket( SSTPK_EXTRAITEM_DATA_MOVE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
		PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
		pUser->FillExtraItemMoveData( kPacket, iStartRow );
		SendMessage( kPacket );
	}
}

void ServerNode::OnUserDataMove( SP2Packet &rkPacket )
{
	int iUserIndex = 0, iMovingValue = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );

	User *pUser = g_UserNodeManager.GetMoveUserNode( iUserIndex, iMovingValue );
	if(!pUser)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserDataMove NULL %d - %d", iUserIndex, iMovingValue );
		return;
	}

	if( pUser && pUser->GetServerMovingValue() == iMovingValue )
	{
		{	
			// 유저 치장 정보 전송
			SendDecoMoveData( iMovingValue, pUser );	
		}

		{	// 유저 조합 정보 전송
			SP2Packet kPacket( SSTPK_ALCHEMIC_DATA_MOVE_RESULT );
			kPacket << iMovingValue << pUser->GetPublicID();
			pUser->FillAlchemicInvenMoveData( kPacket );
			SendMessage( kPacket );
		}

		{	// 유저 장비 정보 전송
			SendExtraItemMoveData( iMovingValue, pUser );
		}

		{	// 유저 퀘스트 정보 전송
			SP2Packet kPacket( SSTPK_QUEST_DATA_MOVE_RESULT );
			kPacket << iMovingValue << pUser->GetPublicID();
			pUser->FillQuestMoveData( kPacket );
			SendMessage( kPacket );
		}

		{
			// 유저 ETC아이템 정보 전송
			SP2Packet kPacket( SSTPK_ETCITEM_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(iMovingValue) );
			PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
			pUser->FillEtcItemMoveData( kPacket );
			SendMessage( kPacket );
		}

		{
			// 유저 메달 아이템 정보 전송
			SP2Packet kPacket( SSTPK_MEDALITEM_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(iMovingValue) );
			PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
			pUser->FillMedalItemMoveData( kPacket );
			SendMessage( kPacket );
		}

		{
			// 유저 용병정보 전송
			SP2Packet kPacket( SSTPK_SOLDIER_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(iMovingValue) );
			PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
			pUser->FillSoldierMoveData( kPacket );
			SendMessage( kPacket );

		}

		{	// 유저 기본 & 기타 정보 전송
			SP2Packet kPacket( SSTPK_USER_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(iMovingValue) );
			PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
			pUser->FillMoveData( kPacket );
			SendMessage( kPacket );
		}

		{
			//유저 펫 정보 전송
			SP2Packet kPacket( SSTPK_PET_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillPetMoveData( kPacket );
			SendMessage( kPacket );
		}

		{
			//유저 각성중인 정보 전송
			SP2Packet kPacket( SSTPK_CHAR_AWAKE_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillAwakeMoveData( kPacket );
			SendMessage( kPacket );	
		}

		{
			//유저 코스튬 정보 전송
			SP2Packet kPacket( SSTPK_COSTUME_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillCostumeMoveData(kPacket);
			SendMessage(kPacket);
		}

		{
			//유저 미션 정보 전송
			SP2Packet kPacket( SSTPK_MISSION_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillMissionMoveData(kPacket);
			SendMessage(kPacket);
		}

		{
			//유저 출석부 정보 전송
			SP2Packet kPacket( SSTPK_ROOLBOOK_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillRollBookMoveData(kPacket);
			SendMessage(kPacket);
		}

		{
			// 개인본부 인벤 데이터
			SP2Packet kPacket( SSTPK_PERSONAL_HQ_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillPersonalHQData(kPacket);
			SendMessage(kPacket);
		}

		{
			// 보너스 캐쉬.
			SP2Packet kPacket( SSTPK_BONUS_CASH_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillBonusCashData(kPacket);
			SendMessage(kPacket);
		}

		{
			// 기간 캐쉬 박스 데이터.
			SP2Packet kPacket( SSTPK_TIMECASH_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillTimeCashDate(kPacket);
			SendMessage(kPacket);
		}

		{
			// 칭호 정보 
			SP2Packet kPacket( SSTPK_TITLE_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillTitleMoveData(kPacket);
			SendMessage(kPacket);
		}

		{
			// 악세사리 정보
			SP2Packet kPacket( SSTPK_ACCESSORY_DATA_MOVE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( iMovingValue ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
			pUser->FillAccessoryMoveData(kPacket);
			SendMessage(kPacket);
		}

		g_LogDBClient.OnInsertTime( pUser, LogDBClient::TT_MOVE_SERVER );
		pUser->SetStartTimeLog( 0 ); // 초기화

		// 유저 정보를 전송했으므로 지금부터 DB에서 받는 데이터는 무시하기위해 노드를 변경한다.
		g_UserNodeManager.MoveUserNode( pUser );

		// 유저 복사본 생성
		UserCopyNode *pNewNode = CreateNewUser( pUser->GetUserIndex() );
		if( pNewNode )
		{
			pNewNode->ApplyUserNode( (UserParent*)pUser );
			g_UserNodeManager.AddCopyUser( pNewNode );
		}

		// 모든 서버에 유저 이동 전송
		SP2Packet kPacket1( SSTPK_USER_MOVE );
		kPacket1 << GetServerIndex();
		pUser->FillUserLogin( kPacket1 );
		g_ServerNodeManager.SendMessageAllNode( kPacket1, GetServerIndex() );

		pUser->BeforeLogoutProcess();

		//kyg빌링 이때 유저가 이동 함을 알고 
		// 이후부터 유저의 위치는 지금 서버가 아니라 이동한 서버이다.
		if( IsSameBilling( GetServerIndex()) )
		{
			//kyg 나랑 상대방서버가 같은 빌링이냐 아니냐를 판단 
		}
		else
		{
			//나랑 같지 않다면 내가 LogOut 패킷을 보낸다. 
			LOG.PrintTimeAndLog(0,"[info][ServerMove]Send logout packet : [%lu]", pUser->GetUserIndex());
			pUser->SetSendLogOutUserState(true);

			pUser->SendSessionLogout();
		}
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserDataMove Not Node %d - %d", iUserIndex, iMovingValue );

	
	if(pUser)
	{
		
	}
	
}

void ServerNode::OnUserInventoryMoveResult( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;
	rkPacket >> iMovingValue >> szPublicID;

	// 서버 이동하는 데이터이므로 패킷이 도착했을 때 유저가 다른서버에 있다는게 말이 안된다.
	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyInventoryMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserInventoryMoveResult Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnUserExtraItemMoveResult( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;
	rkPacket >> iMovingValue >> szPublicID;

	// 서버 이동하는 데이터이므로 패킷이 도착했을 때 유저가 다른서버에 있다는게 말이 안된다.
	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyExtraItemMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserExtraItemMoveResult Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnUserAlchemicMoveResult( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;
	rkPacket >> iMovingValue >> szPublicID;

	// 서버 이동하는 데이터이므로 패킷이 도착했을 때 유저가 다른서버에 있다는게 말이 안된다.
	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyAlchemicInvenData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserAlchemicMoveResult Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnUserPetMoveResult( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;
	PACKET_GUARD_VOID( rkPacket.Read( iMovingValue ) );
	PACKET_GUARD_VOID( rkPacket.Read( szPublicID ) );
	
	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyPetData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserPetMoveResult Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnUserCharAwakeMoveResult( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;
	PACKET_GUARD_VOID( rkPacket.Read( iMovingValue ) );
	PACKET_GUARD_VOID( rkPacket.Read( szPublicID ) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyAwakeMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserPetMoveResult Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnUserQuestMoveResult( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;
	rkPacket >> iMovingValue >> szPublicID;

	// 서버 이동하는 데이터이므로 패킷이 도착했을 때 유저가 다른서버에 있다는게 말이 안된다.
	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyQuestMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserQuestMoveResult Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnUserDataMoveResult( SP2Packet &rkPacket )
{//kyg 이떄 유저가 완전 이동 됐을을 인식함 .
	int iMovingValue;
	ioHashString szPublicID;
	rkPacket >> iMovingValue >> szPublicID;

	// 서버 이동하는 데이터이므로 패킷이 도착했을 때 유저가 다른서버에 있다는게 말이 안된다.
	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		
		pUser->ApplyMoveData( rkPacket );	

		pUser->AfterLoginProcess( true );
		//여기서 올드 서버 파악 가능 gk

		// 친구 목록 요청.
		SP2Packet kPacket( SSTPK_USER_FRIEND_MOVE );
		kPacket << pUser->GetUserIndex() << iMovingValue << FRIEND_LIST_MOVE_COUNT << 0;
		SendMessage( kPacket );
		
		// 서버 이동되었으니 정보 갱신.
		RemoveUserNode( pUser->GetUserIndex() );
		g_DBClient.OnUpdateUserMoveServerID( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), g_App.GetGameServerID() );		

		pUser->m_bOnPopup = false;
		g_EventMgr.NotifySpecificEventInfoWhenLogin(pUser);
		//pUser->OnConnectPopupProcess();

		// 현재 총 가입자수 전송
		SP2Packet kTotalRegUser( STPK_TOTAL_REG_USER_CNT );
		kTotalRegUser << g_MainServer.GetTotalUserRegCount();
		pUser->SendMessage( kTotalRegUser );

		// 서버 세일 상태 전송
		g_SaleMgr.SendLastActiveDate( pUser );

	}
	else
	{
		RemoveUserNode( szPublicID );

		// 서버 이동중에 유저가 Disconnect되었으면 유저노드에 넣고 바로 Disconnect시켜서 정상적인 로그아웃 프로세스를 작동 시킨다.
		g_UserNodeManager.ServerMovingPassiveLogOut( rkPacket );
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnUserDataMoveResult Exception Node %s = 로그아웃 시킴", szPublicID.c_str() );
	}
}

void ServerNode::OnUserSync( SP2Packet &rkPacket )
{
	DWORD dwSyncType, dwUserIndex;
	rkPacket >> dwSyncType >> dwUserIndex;
	switch( dwSyncType )
	{
	case USER_SYNC_LOGIN:
		{
			UserCopyNode *pNewNode = CreateNewUser( dwUserIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncCreate( rkPacket );
				g_UserNodeManager.AddCopyUser( pNewNode );

				//
				g_HeroRankManager.CheckLogIn( pNewNode->GetUserIndex(), pNewNode->GetPublicID() );
			}
		}
		break;
	case USER_SYNC_POS:
		{
			UserCopyNode *pNewNode = GetUserNode( dwUserIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncPos( rkPacket );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserSync Not User : %d - %d - %s:%d", dwSyncType, dwUserIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case USER_SYNC_UPDATE:
		{
			UserCopyNode *pNewNode = GetUserNode( dwUserIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncUpdate( rkPacket );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserSync Not User : %d - %d - %s:%d", dwSyncType, dwUserIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case USER_SYNC_GUILD:
		{
			UserCopyNode *pNewNode = GetUserNode( dwUserIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncGuild( rkPacket );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserSync Not User : %d - %d - %s:%d", dwSyncType, dwUserIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case USER_SYNC_CAMP:
		{
			UserCopyNode *pNewNode = GetUserNode( dwUserIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncCamp( rkPacket );

				//
				g_HeroRankManager.CheckCamp( pNewNode->GetUserIndex(), pNewNode->GetUserCampPos() );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserSync Not User : %d - %d - %s:%d", dwSyncType, dwUserIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case USER_SYNC_LOGOUT:
		{
			UserCopyNode *pNewNode = GetUserNode( dwUserIndex );
			if( pNewNode )
			{
				RemoveUserNode( dwUserIndex );
			}			
			else
			{
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnUserSync Not User : %d - %d - %s:%d", dwSyncType, dwUserIndex, GetServerIP().c_str(), GetServerPort() );
				
				// 서버 노드에 있는 유저를 로그아웃 시킨다.
				ServerNode *pServerNode = g_ServerNodeManager.GetUserIndexToServerNode( dwUserIndex );
				if( pServerNode )
				{
					pServerNode->RemoveUserNode( dwUserIndex );
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnUserSync Exception LogOut : %d - %s:%d", dwUserIndex, GetServerIP().c_str(), GetServerPort() );
				}
			}

			//
			g_HeroRankManager.CheckLogOut( dwUserIndex );
		}		
		break;
	case USER_SYNC_PUBLICID:
		{
			UserCopyNode *pNewNode = GetUserNode( dwUserIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncPublicID( rkPacket );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserSync Not User : %d - %d - %s:%d", dwSyncType, dwUserIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case USER_SYNC_BESTFRIEND:
		{
			UserCopyNode *pNewNode = GetUserNode( dwUserIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncBestFriend( rkPacket );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserSync Not User : %d - %d - %s:%d", dwSyncType, dwUserIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case USER_SYNC_SHUFFLE:
		{
			UserCopyNode *pNewNode = GetUserNode( dwUserIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncShuffle( rkPacket );
			}			
		}
		break;
	}
}

void ServerNode::_OnMoveUserNode( DWORD dwUserIndex, SP2Packet &rkPacket )
{
	// 이동된 서버로 유저정보 추가.
	UserCopyNode *pNewNode = CreateNewUser( dwUserIndex );
	if( pNewNode )
	{
		pNewNode->ApplySyncCreate( rkPacket );
		g_UserNodeManager.AddCopyUser( pNewNode );
	}
}

void ServerNode::OnUserMove( SP2Packet &rkPacket )
{
	DWORD dwMoveServerIndex, dwUserIndex;
	rkPacket >> dwMoveServerIndex >> dwUserIndex;

	RemoveUserNode( dwUserIndex );
	{
		ServerNode *pServerNode = g_ServerNodeManager.GetServerNode( dwMoveServerIndex );
		if( pServerNode )
			pServerNode->_OnMoveUserNode( dwUserIndex, rkPacket );
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserMove Not Node : %d", dwMoveServerIndex );
	}		
}

void ServerNode::OnUserFriendMove( SP2Packet &rkPacket )
{
	int iUserIndex, iMovingValue;
	rkPacket >> iUserIndex >> iMovingValue;
	User *pUser = g_UserNodeManager.GetMoveUserNode( iUserIndex, iMovingValue );
	if( pUser )
	{
		int iFriendCount, iLastIndex;
		rkPacket >> iFriendCount >> iLastIndex;

		SP2Packet kPacket( SSTPK_USER_FRIEND_MOVE_RESULT );
		pUser->SendFriendServerMove( iLastIndex, iFriendCount, kPacket );
		SendMessage( kPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnUserFriendMove Not Node : %d", iUserIndex );
		SP2Packet kPacket( SSTPK_USER_FRIEND_MOVE_RESULT );
		kPacket << iUserIndex << 0 << iMovingValue;
		SendMessage( kPacket );
	}
}

void ServerNode::OnUserFriendMoveResult( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwFriendCount, dwMovingValue;
	rkPacket >> dwUserIndex >> dwFriendCount >> dwMovingValue;
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{		
		for(int i = 0;i < (int)dwFriendCount;i++)
		{
			int  iFriendIndex = 0;
			DWORD dwFriendUserIndex = 0;
			ioHashString szName;
			int iGradeLevel, iCampPosition;
			DWORD dwRegTime;
			int iSendCount = 0;
			DWORD dwSendDate = 0;
			int iReceiveCount = 0;
			int iBeforeReceiveCount = 0;
			DWORD dwReceiveDate = 0;
			bool bSave = false;

			rkPacket >> iFriendIndex >> dwFriendUserIndex >> szName >> iGradeLevel >> iCampPosition >> dwRegTime
				>> iSendCount >> dwSendDate >> iReceiveCount >> dwReceiveDate >> iBeforeReceiveCount >> bSave;
			pUser->InsertFriend( iFriendIndex, dwFriendUserIndex, szName, iGradeLevel, iCampPosition, dwRegTime
								, iSendCount, dwSendDate, iReceiveCount, dwReceiveDate, iBeforeReceiveCount, bSave );
		}

		if( dwFriendCount != FRIEND_LIST_MOVE_COUNT )
		{
			// 서버 이동 완료 알림
			SP2Packet kPacket( STPK_MOVING_SERVER_COMPLETE );
			pUser->FillExerciseIndex( kPacket );
			pUser->SendMessage( kPacket );
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnUserFriendMoveResult Complete : %s : %d", pUser->GetPublicID().c_str(), pUser->GetFriendSize() );

			//kyg 무빙 서버 완료 현재 유저는 2곳다 붙어있는 상태 
			//위메시지 보내고 난후 클라이언트는 예전 서버와의 접속을 끊음 
			//kyg빌링

			if(IsSameBilling(pUser->GetOldServerIndex()))
			{
				//로그인 안보냄
			}
			else
			{
				//로그인 패킷 전송 
				pUser->SendSessionLogin();
				LOG.PrintTimeAndLog(0,"[info][ServerMove]Send login packet : [%lu]",pUser->GetUserIndex());
				//pUser->AfterLoginProcess( true );
			}
		}
		else 
		{
			// 다음 친구 목록 요청.
			SP2Packet kPacket( SSTPK_USER_FRIEND_MOVE );
			kPacket << dwUserIndex << dwMovingValue << FRIEND_LIST_MOVE_COUNT << pUser->GetFriendLastIndex();
			SendMessage( kPacket );
		}
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserFriendMoveResult Not Node : %d", dwUserIndex );
}

void ServerNode::OnFollowUser( SP2Packet &rkPacket )
{
	ioHashString szUserName;
	DWORD dwUserIndex, dwRequestUserIndex;
	int   iUserPos, iAbilityLevel, iMyRoomIndex, iLeaveRoomIndex, iLeaveBattleRoomIndex;
	bool  bDeveloper;
	rkPacket >> dwUserIndex >> dwRequestUserIndex >> iUserPos >> iMyRoomIndex >> iLeaveRoomIndex >> iLeaveBattleRoomIndex >> szUserName >> bDeveloper >> iAbilityLevel;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		UserParent *pRequestUser = GetUserNode( dwRequestUserIndex );
		if( !pRequestUser )			
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnFollowUser Request User Not Node." );
			return;
		}

		if( pUser->_OnFollowUser( pRequestUser, iUserPos, iMyRoomIndex, iLeaveRoomIndex, iLeaveBattleRoomIndex, bDeveloper, iAbilityLevel ) )
		{
			switch( iUserPos )
			{
			case UP_TRAINING:
				{
					Room *pRoom = pUser->GetMyRoom();
					if( pRoom )
					{
						pRoom->EnterReserveUser( dwRequestUserIndex );
						SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
						kPacket << SS_MOVING_ROOM_JOIN << SS_MOVING_ROOM_JOIN_PLAZA << (int)pRoom->GetModeType() << pRoom->GetRoomIndex() << dwRequestUserIndex;	
						kPacket << pRoom->GetModeSubNum() << pRoom->GetModeMapNum() << (int)pRoom->GetPlazaModeType() << pRoom->GetRoomNumber();
						SendMessage( kPacket );
					}
				}
				break;
			case UP_BATTLE_ROOM:
				{
					BattleRoomParent *pUserBattleRoom = pUser->GetMyBattleRoom();
					if( pUserBattleRoom  )
					{
						// 요청 유저에게 파티 입장 전송
						SP2Packet kPacket( SSTPK_BATTLEROOM_FOLLOW );
						kPacket << pRequestUser->GetUserIndex() << pUserBattleRoom->GetIndex() << pUser->GetUserPos();
						SendMessage( kPacket );
					}
				}
				break;
			case UP_LADDER_TEAM:
				{
					LadderTeamParent *pUserLadderTeam = pUser->GetMyLadderTeam();
					if( pUserLadderTeam )
					{
						SP2Packet kPacket( SSTPK_LADDERTEAM_FOLLOW );
						kPacket << pRequestUser->GetUserIndex() << pUserLadderTeam->GetIndex() << pUser->GetUserPos();
						SendMessage( kPacket );
					}
				}
				break;
			}
		}		
	}
	else if( !g_UserNodeManager.IsConnectUser( szUserName ) )
	{
		UserParent *pRequestUser = GetUserNode( dwRequestUserIndex );
		if( pRequestUser )
		{
			SP2Packet kPacket( STPK_FOLLOW_USER );
			kPacket << FOLLOW_USER_ERROR_3 << szUserName << iUserPos << iUserPos;
			pRequestUser->RelayPacket( kPacket );
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnFollowUser 요청 및 대상 유저 없음." );
		}
	}
}

void ServerNode::OnUserPosIndex( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwRequestUserIndex;
	int   iUserPos, iPrevBattleIndex;
	ioHashString szUserName;
	bool bSafetyLevel;
	rkPacket >> dwUserIndex >> dwRequestUserIndex >> iUserPos >> szUserName >> bSafetyLevel >> iPrevBattleIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		UserParent *pRequestUser = GetUserNode( dwRequestUserIndex );
		if( !pRequestUser )			
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserPosIndex Request User Not Node." );
			return;
		}
		pUser->_OnUserPosIndex( pRequestUser, bSafetyLevel, iUserPos, iPrevBattleIndex );
	}
	else if( !g_UserNodeManager.IsConnectUser( szUserName ) )
	{
		UserParent *pRequestUser = GetUserNode( dwRequestUserIndex );
		if( pRequestUser )
		{
			SP2Packet kPacket( STPK_FOLLOW_USER );
			kPacket << FOLLOW_USER_ERROR_3 << szUserName << iUserPos << iUserPos;
			pRequestUser->RelayPacket( kPacket );
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserPosIndex 요청 및 대상 유저 없음." );
		}
	}
}

void ServerNode::OnChannelSync( SP2Packet &rkPacket )
{
	int iSyncType;
	DWORD dwChannelIndex;
	rkPacket >> iSyncType >> dwChannelIndex;
	switch( iSyncType )
	{
	case ChannelParent::CHANNEL_CREATE:
		{
			ChannelCopyNode *pNewNode = CreateNewChannel( dwChannelIndex );
			if( pNewNode )
			{
				g_ChannelNodeManager.AddCopyChannel( pNewNode );		
			}
		}
		break;
	case ChannelParent::CHANNEL_DESTROY:
		{
			RemoveChannelNode( dwChannelIndex );
		}
		break;
	case ChannelParent::CHANNEL_ENTER:
		{
			ChannelNode *pChannel = g_ChannelNodeManager.GetChannelNode( dwChannelIndex );
			if( pChannel )
			{
				DWORD dwUserIndex;
				ioHashString szEnterUser;
				rkPacket >> dwUserIndex >> szEnterUser;
				pChannel->EnterChannel( dwUserIndex, szEnterUser );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelSync 입장하려는 채널 없음 %d", dwChannelIndex );
		}
		break;
	case ChannelParent::CHANNEL_LEAVE:
		{
			ChannelNode *pChannel = g_ChannelNodeManager.GetChannelNode( dwChannelIndex );
			if( pChannel )
			{
				DWORD dwUserIndex;
				ioHashString szLeaveUser;
				rkPacket >> dwUserIndex >> szLeaveUser;
				pChannel->LeaveChannel( dwUserIndex, szLeaveUser );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelSync 퇴장하려는 채널 없음 %d", dwChannelIndex );
		}
		break;
	case ChannelParent::CHANNEL_TRANSFER:
		{
			ChannelNode *pChannel = g_ChannelNodeManager.GetChannelNode( dwChannelIndex );
			if( pChannel )
			{
				DWORD dwUserIndex, dwPacketID;
				rkPacket >> dwUserIndex >> dwPacketID;				
				int iMoveSize = sizeof(int) + ( sizeof(DWORD) * 3 );
				// 패킷 생성
				SP2Packet kPacket( dwPacketID );
				kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
				pChannel->SendPacketTcp( kPacket, dwUserIndex );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelSync 중계하려는 채널 없음 %d", dwChannelIndex );
		}
		break;
	}
}

void ServerNode::OnChannelInvite( SP2Packet &rkPacket )
{
	int iTransferType;
	DWORD dwChannelIndex;
	rkPacket >> iTransferType >> dwChannelIndex;
	switch( iTransferType )
	{
	case INVITE_CHANNEL_TRANSFER:
		{
			ioHashString szInvitedID;
			DWORD dwInveteUserIndex;
			rkPacket >> szInvitedID >> dwInveteUserIndex;
			ChannelNode *pChannel = g_ChannelNodeManager.GetChannelNode( dwChannelIndex );
			if( pChannel )
			{
				UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( szInvitedID );
				if( !pUserParent )   // 유저 없음
				{
					UserCopyNode *pInviteUser = GetUserNode( dwInveteUserIndex );
					if( !pInviteUser )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelInvite INVITE_CHANNEL_TRANSFER 1 초대 유저 없음 : %d", dwInveteUserIndex );
						return;
					}

					SP2Packet kPacket( STPK_CHANNEL_INVITE );
					kPacket << CHANNEL_INVITE_NOT_USER << dwChannelIndex << szInvitedID;
					pInviteUser->RelayPacket( rkPacket );
				}
				else if( pUserParent->IsUserOriginal() )
				{
					User *pUser = (User*)pUserParent;
					int iResult = pUser->_OnChannelInviteException( dwChannelIndex );
					if( iResult != CHANNEL_INVITE_OK )
					{
						UserCopyNode *pInviteUser = GetUserNode( dwInveteUserIndex );
						if( !pInviteUser )
						{
							LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelInvite INVITE_CHANNEL_TRANSFER 2 초대 유저 없음 : %d", dwInveteUserIndex );
							return;
						}

						SP2Packet kPacket( STPK_CHANNEL_INVITE );
						kPacket << iResult << dwChannelIndex << szInvitedID;
						pInviteUser->RelayPacket( kPacket );

						CRASH_GUARD();
						if( pChannel->GetManToManID() == szInvitedID )
							pChannel->SetManToManID( "" );
					}
					else
					{
						UserCopyNode *pInviteUser = GetUserNode( dwInveteUserIndex );
						if( !pInviteUser )
						{
							LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelInvite INVITE_CHANNEL_TRANSFER 3 초대 유저 없음 : %d", dwInveteUserIndex );
							return;
						}

						User *pUser = (User*)pUserParent;
						pUser->EnterChannel( pChannel );
						SP2Packet kPacket( STPK_CHANNEL_INVITE );
						kPacket << CHANNEL_INVITE_OK << pChannel->GetIndex() << pInviteUser->GetPublicID() << pUser->GetPublicID();
						pChannel->SendPacketTcp( kPacket );			
					}
				}
				else      // 복사본 유저 처리
				{
					UserCopyNode *pCopyUser = (UserCopyNode*)pUserParent;
					SP2Packet kPacket( SSTPK_CHANNEL_INVITE );
					kPacket << INVITE_CHANNEL_USER_TRANSFER << dwChannelIndex << szInvitedID << dwInveteUserIndex;
					pCopyUser->SendMessage( kPacket );
				}
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelInvite INVITE_CHANNEL_TRANSFER 없는 채널에서 초대 : %d", dwInveteUserIndex );
		}
		break;
	case INVITE_CHANNEL_TRANSFER_RESULT:
		break;
	case INVITE_CHANNEL_USER_TRANSFER:
		{
			ioHashString szInvitedID;
			DWORD dwInveteUserIndex;
			rkPacket >> szInvitedID >> dwInveteUserIndex;
			ChannelParent *pChannelParent = g_ChannelNodeManager.GetGlobalChannelNode( dwChannelIndex );
			if( pChannelParent )
			{
				User *pUser = GetUserOriginalNode( szInvitedID, rkPacket );
				if( !pUser )   // 유저 없음
				{
					UserParent *pInviteUser = g_UserNodeManager.GetGlobalUserNode( dwInveteUserIndex );
					if( !pInviteUser )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelInvite INVITE_CHANNEL_USER_TRANSFER 1 초대 유저 없음 : %d", dwInveteUserIndex );
						return;
					}

					SP2Packet kPacket( STPK_CHANNEL_INVITE );
					kPacket << CHANNEL_INVITE_NOT_USER << dwChannelIndex << szInvitedID;
					pInviteUser->RelayPacket( kPacket );
				}
				else 
				{
					int iResult = pUser->_OnChannelInviteException( dwChannelIndex );
					if( iResult != CHANNEL_INVITE_OK )
					{
						UserParent *pInviteUser = g_UserNodeManager.GetGlobalUserNode( dwInveteUserIndex );
						if( !pInviteUser )
						{
							LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelInvite INVITE_CHANNEL_USER_TRANSFER 2 초대 유저 없음 : %d", dwInveteUserIndex );
							return;
						}

						SP2Packet kPacket( STPK_CHANNEL_INVITE );
						kPacket << iResult << dwChannelIndex << szInvitedID;
						pInviteUser->RelayPacket( kPacket );
						
						if( pChannelParent->IsChannelOriginal() )
						{
							ChannelNode *pNode = (ChannelNode*)pChannelParent;
							CRASH_GUARD();
							if( pNode->GetManToManID() == szInvitedID )
								pNode->SetManToManID( "" );
						}
						else
						{
							// 초대 실패 맨투맨아이디 삭제
							ChannelCopyNode *pCopyNode = (ChannelCopyNode*)pChannelParent;
							SP2Packet kPacket( SSTPK_CHANNEL_INVITE );
							kPacket << INVITE_CHANNEL_USER_TRANSFER_RESULT << dwChannelIndex << szInvitedID;							
							pCopyNode->SendMessage( kPacket );							
						}
					}
					else
					{
						UserParent *pInviteUser = g_UserNodeManager.GetGlobalUserNode( dwInveteUserIndex );
						if( !pInviteUser )
						{
							LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelInvite INVITE_CHANNEL_USER_TRANSFER 3 초대 유저 없음 : %d", dwInveteUserIndex );
							return;
						}

                        pUser->EnterChannel( pChannelParent );
						SP2Packet kPacket( STPK_CHANNEL_INVITE );
						kPacket << CHANNEL_INVITE_OK << pChannelParent->GetIndex() << pInviteUser->GetPublicID() << pUser->GetPublicID();
						pChannelParent->SendPacketTcp( kPacket );			
					}
				}
			}				
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelInvite INVITE_CHANNEL_USER_TRANSFER 없는 채널에서 초대 : %d", dwInveteUserIndex );
		}
		break;
	case INVITE_CHANNEL_USER_TRANSFER_RESULT:
		{
			ioHashString szInvitedID;
			rkPacket >> szInvitedID;
			ChannelNode *pChannel = g_ChannelNodeManager.GetChannelNode( dwChannelIndex );
			if( pChannel )
			{
				CRASH_GUARD();
				if( pChannel->GetManToManID() == szInvitedID )
					pChannel->SetManToManID( "" );
			}
		}
		break;
	}
}

void ServerNode::OnChannelChat( SP2Packet &rkPacket )
{
	DWORD dwTransferType, dwChannelIndex;
	rkPacket >> dwTransferType >> dwChannelIndex;
	switch( dwTransferType )
	{
	case CHAT_CHANNEL_TRANSFER:
		{
			ChannelNode *pChannel = g_ChannelNodeManager.GetChannelNode( dwChannelIndex );
			if( pChannel )
			{
				DWORD dwUserIndex;
				ioHashString szSendUser, szChat;	
				rkPacket >> szSendUser >> szChat >> dwUserIndex;
				if( pChannel->GetManToManID().IsEmpty() )
				{
					SP2Packet kPacket( STPK_CHANNEL_CHAT );
					kPacket << dwChannelIndex << szSendUser << szChat;
					pChannel->SendPacketTcp( kPacket, dwUserIndex );	
				}
				else 
				{
					// 맨투맨 유저를 먼저 입장 시킨후 채팅 발사
					UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( pChannel->GetManToManID() );
					if( !pUserParent )     //유저 없다
					{
						UserParent *pSendUser = g_UserNodeManager.GetGlobalUserNode( szSendUser );
						if( pSendUser )
						{
							SP2Packet kPacket( STPK_CHANNEL_INVITE );
							kPacket << CHANNEL_INVITE_NOT_USER << pChannel->GetIndex() << pChannel->GetManToManID();
							if( pSendUser->IsUserOriginal() )
							{
								User *pUser = (User*)pSendUser;
								pUser->RelayPacket( kPacket );
							}
							else
							{
								UserCopyNode *pUser = (UserCopyNode*)pSendUser;
								pUser->RelayPacket( kPacket );
							}

						}						
						pChannel->SetManToManID( "" );
					}
					else if( pUserParent->IsUserOriginal() )
					{
						User *pUser = (User*)pUserParent;
						int iResult = pUser->_OnChannelInviteException( dwChannelIndex );
						if( iResult != CHANNEL_INVITE_OK )
						{
							SP2Packet kPacket( STPK_CHANNEL_INVITE );
							kPacket << iResult << dwChannelIndex << pChannel->GetManToManID();
							SendMessage( kPacket );
							pChannel->SetManToManID( "" );
						}
						else
						{
							pUser->EnterChannel( pChannel );				
						}

						// 채팅 전송
						SP2Packet kPacket( STPK_CHANNEL_CHAT );
						kPacket << dwChannelIndex << szSendUser << szChat;
						pChannel->SendPacketTcp( kPacket, dwUserIndex );	
					}
					else    //원본 유저에게 보낸 후 처리
					{
						UserCopyNode *pCopyNode = (UserCopyNode*)pUserParent;
						SP2Packet kPacket( SSTPK_CHANNEL_CHAT );
						kPacket << CHAT_CHANNEL_MANTOMAN_TRANSFER << dwChannelIndex << pCopyNode->GetUserIndex() << szSendUser << szChat << dwUserIndex;
						pCopyNode->SendMessage( kPacket );			
					}
				}
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelChat CHAT_CHANNEL_TRANSFER Not Node : %d", dwChannelIndex );
		}
		break;
	case CHAT_CHANNEL_MANTOMAN_TRANSFER:
		{
			DWORD dwUserIndex;
			rkPacket >> dwUserIndex;
			User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
			if( pUser )
			{
				DWORD dwSendUserIndex;
				ioHashString szSendUser, szChat;	
				rkPacket >> szSendUser >> szChat >> dwSendUserIndex;

				int iResult = pUser->_OnChannelInviteException( dwChannelIndex );
				if( iResult != CHANNEL_INVITE_OK )
				{
					UserParent *pInvitedUser = g_UserNodeManager.GetGlobalUserNode( dwSendUserIndex );
					if( !pInvitedUser )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelChat CHAT_CHANNEL_MANTOMAN_TRANSFER 1 초대 유저 없음 : %s", szSendUser.c_str() );
						return;
					}

					ChannelParent *pChannelParent = g_ChannelNodeManager.GetGlobalChannelNode( dwChannelIndex );
					if( !pChannelParent )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelChat CHAT_CHANNEL_MANTOMAN_TRANSFER 1 초대 채널 없음 : %s", szSendUser.c_str() );
						return;
					}
					
					if( !pInvitedUser->IsUserOriginal() )
					{
						UserCopyNode *pUserCopyNode = (UserCopyNode*)pInvitedUser;
						SP2Packet kPacket( STPK_CHANNEL_INVITE );
						kPacket << iResult << dwChannelIndex << pUser->GetPublicID();
						pUserCopyNode->RelayPacket( kPacket );
					}

					if( pChannelParent->IsChannelOriginal() )
					{
						ChannelNode *pNode = (ChannelNode*)pChannelParent;
						CRASH_GUARD();
						if( pNode->GetManToManID() == pUser->GetPublicID() )
							pNode->SetManToManID( "" );
					}
					else
					{
						// 초대 실패 맨투맨아이디 삭제
						ChannelCopyNode *pCopyNode = (ChannelCopyNode*)pChannelParent;
						SP2Packet kPacket( SSTPK_CHANNEL_INVITE );
						kPacket << INVITE_CHANNEL_USER_TRANSFER_RESULT << dwChannelIndex << pUser->GetPublicID();							
						pCopyNode->SendMessage( kPacket );		
					}
				}	
				else
				{
					ChannelParent *pChannelParent = g_ChannelNodeManager.GetGlobalChannelNode( dwChannelIndex );
					if( !pChannelParent )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelChat CHAT_CHANNEL_MANTOMAN_TRANSFER 2 초대 채널 없음 : %s", szSendUser.c_str() );
						return;
					}

					pUser->EnterChannel( pChannelParent );

					// 채팅 전송
					SP2Packet kPacket( STPK_CHANNEL_CHAT );
					kPacket << pChannelParent->GetIndex() << szSendUser << szChat;
					pChannelParent->SendPacketTcp( kPacket, dwSendUserIndex );	
				}
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnChannelChat CHAT_CHANNEL_MANTOMAN_TRANSFER Not Node : %d", dwUserIndex );
		}
		break;
	}
}

void ServerNode::OnBattleRoomSync( SP2Packet &rkPacket )
{
	int iSyncType, iBattleRoomIndex;
	rkPacket >> iSyncType >> iBattleRoomIndex;

	switch( iSyncType )
	{
	case BattleRoomSync::BRS_SELECTMODE:
		{
			BattleRoomCopyNode *pCopyNode = GetBattleRoomNode( iBattleRoomIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncSelectMode( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomSync Not Room : %d - %d - %s:%d", iSyncType, iBattleRoomIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case BattleRoomSync::BRS_PLAY:
		{
			BattleRoomCopyNode *pCopyNode = GetBattleRoomNode( iBattleRoomIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncPlay( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomSync Not Room : %d - %d - %s:%d", iSyncType, iBattleRoomIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;	
	case BattleRoomSync::BRS_CHANGEINFO:
		{
			BattleRoomCopyNode *pCopyNode = GetBattleRoomNode( iBattleRoomIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncChangeInfo( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomSync Not Room : %d - %d - %s:%d", iSyncType, iBattleRoomIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case BattleRoomSync::BRS_CREATE:
		{
			BattleRoomCopyNode *pNewNode = CreateNewBattleRoom( iBattleRoomIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncCreate( rkPacket );
				g_BattleRoomManager.AddCopyBattleRoom( pNewNode );
			}
		}
		break;
	case BattleRoomSync::BRS_DESTROY:
		RemoveBattleRoomNode( iBattleRoomIndex );
		break; 
	}
}

void ServerNode::OnBattleRoomTransfer( SP2Packet &rkPacket )
{
	// 모두 오리지날 전투룸이다
	int iTransferType, iBattleRoomIndex;
	rkPacket >> iTransferType >> iBattleRoomIndex;

	BattleRoomNode *pBattleRoom = g_BattleRoomManager.GetBattleRoomNode( iBattleRoomIndex );	
	switch( iTransferType )
	{
	case BattleRoomParent::ENTER_USER:
		{
			DWORD dwUserIndex;
			bool  bSafetyLevel, bObserver;
			int   iGradeLevel, iAbilityLevel, iClientPort, iTransferPort;
			ioHashString szPublicID, szPublicIP, szPrivateIP, szTransferIP;
			rkPacket >> dwUserIndex >> szPublicID >> iGradeLevel >> iAbilityLevel >> bSafetyLevel >> bObserver
					 >> szPublicIP >> szPrivateIP >> szTransferIP >> iClientPort >> iTransferPort;

			if( pBattleRoom == NULL )
			{
				SP2Packet kPacket( SSTPK_BATTLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_BATTLEROOM_JOIN_NOT_NODE << iBattleRoomIndex;
				SendMessage( kPacket );				
			}
			else if( !bObserver && pBattleRoom->IsFull() )
			{
				SP2Packet kPacket( SSTPK_BATTLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_BATTLEROOM_JOIN_FULL_USER << iBattleRoomIndex << pBattleRoom->GetPlayUserCnt()
						<< pBattleRoom->GetMaxPlayerBlue() << pBattleRoom->GetMaxPlayerRed() << pBattleRoom->GetMaxObserver();
				SendMessage( kPacket );						
			}
			else if( !bObserver && pBattleRoom->IsMapLimitPlayerFull() )
			{
				SP2Packet kPacket( SSTPK_BATTLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_BATTLEROOM_JOIN_MAP_LIMIT_PLAYER << iBattleRoomIndex << pBattleRoom->GetPlayUserCnt()
						<< pBattleRoom->GetMaxPlayerBlue() << pBattleRoom->GetMaxPlayerRed() << pBattleRoom->GetMaxObserver();
				SendMessage( kPacket );
			}
			else if( !bObserver && pBattleRoom->IsMapLimitGrade( iGradeLevel ) )
			{
				SP2Packet kPacket( SSTPK_BATTLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_BATTLEROOM_JOIN_MAP_LIMIT_GRADE << iBattleRoomIndex << pBattleRoom->GetPlayUserCnt()
					<< pBattleRoom->GetMaxPlayerBlue() << pBattleRoom->GetMaxPlayerRed() << pBattleRoom->GetMaxObserver();
				SendMessage( kPacket );
				return;
			}
			else if( bObserver && pBattleRoom->IsObserverFull() )
			{
				SP2Packet kPacket( SSTPK_BATTLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_BATTLEROOM_JOIN_FULL_USER << iBattleRoomIndex << pBattleRoom->GetPlayUserCnt()
						<< pBattleRoom->GetMaxPlayerBlue() << pBattleRoom->GetMaxPlayerRed() << pBattleRoom->GetMaxObserver();
				SendMessage( kPacket );
			}	
			else if( pBattleRoom->IsBattleTimeClose() && !pBattleRoom->IsInviteUser( dwUserIndex ) )
			{
				SP2Packet kPacket( SSTPK_BATTLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_BATTLEROOM_JOIN_TIME_CLOSE << iBattleRoomIndex;
				SendMessage( kPacket );								
			}
			else if( !bObserver && pBattleRoom->IsStartRoomEnterX() )
			{
				SP2Packet kPacket( SSTPK_BATTLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_BATTLEROOM_JOIN_START_ROOM_ENTER_X << iBattleRoomIndex;
				SendMessage( kPacket );	
			}
			else if( !bObserver && pBattleRoom->IsNoChallenger() )
			{
				SP2Packet kPacket( SSTPK_BATTLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_BATTLEROOM_JOIN_NO_CHALLENGER << iBattleRoomIndex;
				SendMessage( kPacket );	
			}
			else if( !bObserver && pBattleRoom->GetSelectModeTerm() == BMT_TEAM_SURVIVAL_FIRST && !bSafetyLevel )
			{
				SP2Packet kPacket( SSTPK_BATTLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_BATTLEROOM_JOIN_SAFETY_ROOM << iBattleRoomIndex;
				SendMessage( kPacket );	
			}
			else
			{				
				pBattleRoom->EnterUser( dwUserIndex, szPublicID, iGradeLevel, iAbilityLevel, bSafetyLevel, bObserver, szPublicIP, szPrivateIP, szTransferIP, iClientPort, iTransferPort );
			}			
		}
		break;
	case BattleRoomParent::LEAVE_USER:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer LEAVE_USER 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}

			DWORD dwUserIndex;
			ioHashString szPublicID;
			rkPacket >> dwUserIndex >> szPublicID;
			pBattleRoom->LeaveUser( dwUserIndex, szPublicID );
		}
		break;
	case BattleRoomParent::ROOM_INFO:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer ROOM_INFO 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}

			int iPrevBattleIndex;
			DWORD dwUserIndex;
			rkPacket >> dwUserIndex >> iPrevBattleIndex;
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
			if( pUser == NULL )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomParent::ROOM_INFO 유저 NULL : %d", dwUserIndex );
				return;
			}
			pBattleRoom->OnBattleRoomInfo( pUser, iPrevBattleIndex );
		}
		break;
	case BattleRoomParent::USER_INFO:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer USER_INFO 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}
			
			DWORD dwUserIndex;
			bool  bSafetyLevel;
			int   iGradeLevel, iAbilityLevel, iClientPort, iTransferPort;
			ioHashString szTransferIP;
			rkPacket >> dwUserIndex >> iGradeLevel >> iAbilityLevel >> bSafetyLevel >> iClientPort >> szTransferIP >> iTransferPort;
			pBattleRoom->UserInfoUpdate( dwUserIndex, iGradeLevel, iAbilityLevel, bSafetyLevel, iClientPort, szTransferIP, iTransferPort );
		}
		break;
	case BattleRoomParent::UDP_CHANGE:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer UDP_CHANGE 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}

			DWORD dwUserIndex;
			int   iClientPort, iTransferPort;
			ioHashString szPublicID, szPublicIP, szPrivateIP, szTransferIP;
			rkPacket >> dwUserIndex >> szPublicID >> szPublicIP >> iClientPort >> szPrivateIP >> szTransferIP >> iTransferPort;
			pBattleRoom->UserUDPChange( dwUserIndex, szPublicID, szPublicIP, iClientPort, szPrivateIP, szTransferIP, iTransferPort );
		}
		break;
	case BattleRoomParent::TRANSFER_PACKET:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer TRANSFER_PACKET 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}

			DWORD dwUserIndex, dwPacketID;
			rkPacket >> dwUserIndex >> dwPacketID;				
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 2 );
			// 패킷 생성
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
			pBattleRoom->SendPacketTcp( kPacket, dwUserIndex );			
		}
		break;
	case BattleRoomParent::TRANSFER_PACKET_USER:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer TRANSFER_PACKET_USER 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}

			ioHashString szSenderName;
			DWORD dwPacketID;
			rkPacket >> szSenderName >> dwPacketID;				
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 1 ) + lstrlen( szSenderName.c_str() ) + 1;
			// 패킷 생성
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
			pBattleRoom->SendPacketTcpUser( kPacket, szSenderName );			
		}
		break;
	case BattleRoomParent::USER_PACKET_TRANSFER:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer USER_PACKET_TRANSFER 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}

			DWORD dwUserIndex, dwPacketID;
			rkPacket >> dwUserIndex >> dwPacketID;	
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
			if( pUser == NULL )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomParent::USER_PACKET_TRANSFER 유저 NULL : %d - 0x%x", dwUserIndex, dwPacketID );
				return;
			}
			// 패킷 생성
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 2 );
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize, true );
			pBattleRoom->OnProcessPacket( kPacket, pUser );
		}
		break;
	case BattleRoomParent::P2P_RELAY_INFO:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer P2P_RELAY_INFO 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}
			DWORD dwUserIndex, dwRelayUserIndex;
			bool  bServerRelay;
			rkPacket >> dwUserIndex >> dwRelayUserIndex >> bServerRelay;
			pBattleRoom->UserP2PRelayInfo( dwUserIndex, dwRelayUserIndex, bServerRelay );
		}
		break;
	case BattleRoomParent::TRANSFER_PACKET_UDP:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer TRANSFER_PACKET_UDP 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}

			DWORD dwUserIndex, dwPacketID;
			rkPacket >> dwUserIndex >> dwPacketID;				
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 2 );
			// 패킷 생성
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
			pBattleRoom->SendPacketUdp( kPacket, dwUserIndex );			
		}
		break;
	case BattleRoomParent::TOURNAMENT_INFO:
		{
			if( !pBattleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomTransfer TOURNAMENT_INFO 중계하려는 전투룸 없음 %d", iBattleRoomIndex );
				return;
			}

			DWORD dwUserIndex, dwTeamIndex;
			rkPacket >> dwUserIndex >> dwTeamIndex;
			pBattleRoom->TournamentInfo( dwUserIndex, dwTeamIndex );
		}
		break;
	}
}

void ServerNode::OnPlazaRoomTransfer( SP2Packet &rkPacket )
{
	// 모두 오리지날 전투룸이다
	int iTransferType, iRoomIndex;
	rkPacket >> iTransferType >> iRoomIndex;

	RoomParent *pNode = g_RoomNodeManager.GetPlazaNodeByNum( iRoomIndex );
	switch( iTransferType )
	{
	case RoomParent::ROOM_INFO:
		{
			if( !pNode )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnPlazaRoomTransfer ROOM_INFO 중계하려는 전투룸 없음 %d", iRoomIndex );
				return;
			}

			DWORD dwUserIndex;
			rkPacket >> dwUserIndex;
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
			if( pUser == NULL )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomParent::ROOM_INFO 유저 NULL : %d", dwUserIndex );
				return;
			}
			pNode->OnPlazaRoomInfo( pUser );
		}
		break;
	}
}

void ServerNode::OnBattleRoomJoinResult( SP2Packet &rkPacket )
{
	DWORD dwUserIndex = 0;
	DWORD dwBattleRoomIndex = 0;
	int   iResultType = 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iResultType) );
	PACKET_GUARD_VOID( rkPacket.Read(dwBattleRoomIndex) );

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomJoinResult 유저 NULL : %d", dwUserIndex );
		return;
	}
	pUser->_OnBattleRoomJoinResult( iResultType, false, 0, true );

	// 변경사항 갱신
	BattleRoomCopyNode *pBattleRoom = GetBattleRoomNode( dwBattleRoomIndex );
	if( pBattleRoom )
	{
		switch( iResultType )
		{
		case USER_BATTLEROOM_JOIN_FULL_USER:
			{
				int iPlayUserCnt, iMaxPlayerBlue, iMaxPlayerRed, iMaxObserver;
				rkPacket >> iPlayUserCnt >> iMaxPlayerBlue >> iMaxPlayerRed >> iMaxObserver;
				pBattleRoom->SetJoinUserCnt( iPlayUserCnt );
				pBattleRoom->SetMaxPlayer( iMaxPlayerBlue, iMaxPlayerRed, iMaxObserver );
			}
			break;
		case USER_BATTLEROOM_JOIN_TIME_CLOSE:
			{
				pBattleRoom->SetTimeClose( true );
			}
			break;
		case USER_BATTLEROOM_JOIN_START_ROOM_ENTER_X:
			{
				pBattleRoom->SetStartRoomEnterX( true );
			}
			break;
		case USER_BATTLEROOM_JOIN_NO_CHALLENGER:
			{
				pBattleRoom->SetNoChallenger( true );
			}
			break;
		}
	}
}

void ServerNode::OnBattleRoomFollow( SP2Packet &rkPacket )
{
	int iNextPos;
	DWORD dwUserIndex, dwBattleRoomIndex;
	rkPacket >> dwUserIndex >> dwBattleRoomIndex >> iNextPos;
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomFollow 유저 NULL : %d", dwUserIndex );
		return;
	}
	pUser->_OnBattleRoomFollow( g_BattleRoomManager.GetGlobalBattleRoomNode( dwBattleRoomIndex ), iNextPos );	
}

void ServerNode::OnUserInfoRefresh( SP2Packet &rkPacket )
{
	DWORD dwTargetIndex, dwSendIndex;
	rkPacket >> dwTargetIndex >> dwSendIndex;
	
	User *pTargetUser = GetUserOriginalNode( dwTargetIndex, rkPacket );
	if( pTargetUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserInfoRefresh 타겟 유저 NULL : %d", dwTargetIndex );
		return;
	}

	UserCopyNode *pSendUser = GetUserNode( dwSendIndex );
	if( pSendUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserInfoRefresh 요청 유저 NULL : %d", dwSendIndex );
		return;
	}
	
	// 툴팁 정보
	SP2Packet kPacket( STPK_USER_INFO_REFRESH );
	pTargetUser->FillToolTipInfo( kPacket );
	pSendUser->RelayPacket( kPacket );

	// 길드 정보
	ioUserGuild *pUserGuild = pTargetUser->GetUserGuild();
	if( pUserGuild )			
	{
		// 유저에게 전송
		SP2Packet kPacket( STPK_GUILD_SIMPLE_DATA );
		kPacket << pTargetUser->GetPublicID() << pUserGuild->GetGuildIndex() << pUserGuild->GetGuildName()
				<< pUserGuild->GetGuildMark();
		pSendUser->RelayPacket( kPacket );
	}			
}

void ServerNode::OnSimpleUserInfoRefresh( SP2Packet &rkPacket )
{
	DWORD dwTargetIndex, dwSendIndex;
	rkPacket >> dwTargetIndex >> dwSendIndex;

	User *pTargetUser = GetUserOriginalNode( dwTargetIndex, rkPacket );
	if( pTargetUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnSimpleUserInfoRefresh 타겟 유저 NULL : %d", dwTargetIndex );
		return;
	}

	UserCopyNode *pSendUser = GetUserNode( dwSendIndex );
	if( pSendUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnSimpleUserInfoRefresh 요청 유저 NULL : %d", dwSendIndex );
		return;
	}

	// 툴팁 정보
	SP2Packet kPacket( STPK_SIMPLE_USER_INFO_REFRESH );
	pTargetUser->FillSimpleToolTipInfo( kPacket );
	pSendUser->RelayPacket( kPacket );
}

void ServerNode::OnUserCharInfoRefresh( SP2Packet &rkPacket )
{
	DWORD dwTargetIndex, dwSendIndex;
	rkPacket >> dwTargetIndex >> dwSendIndex;

	User *pTargetUser = GetUserOriginalNode( dwTargetIndex, rkPacket );
	if( pTargetUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserCharInfoRefresh 타겟 유저 NULL : %d", dwTargetIndex );
		return;
	}

	UserCopyNode *pSendUser = GetUserNode( dwSendIndex );
	if( pSendUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserCharInfoRefresh 요청 유저 NULL : %d", dwSendIndex );
		return;
	}

	int iStartArray, iSyncCount;
	rkPacket >> iStartArray >> iSyncCount;

	SP2Packet kPacket( STPK_USER_CHAR_INFO_REFRESH );
	pTargetUser->FillUserCharListInfo( kPacket, iStartArray, iSyncCount );

	pSendUser->RelayPacket( kPacket );
}

void ServerNode::OnUserCharSubInfoRefresh( SP2Packet &rkPacket )
{
	DWORD dwTargetIndex, dwSendIndex;
	rkPacket >> dwTargetIndex >> dwSendIndex;

	User *pTargetUser = GetUserOriginalNode( dwTargetIndex, rkPacket );
	if( pTargetUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserCharSubInfoRefresh 타겟 유저 NULL : %d", dwTargetIndex );
		return;
	}

	UserCopyNode *pSendUser = GetUserNode( dwSendIndex );
	if( pSendUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserCharSubInfoRefresh 요청 유저 NULL : %d", dwSendIndex );
		return;
	}

	int iClassType;
	rkPacket >> iClassType;
	SP2Packet kPacket( STPK_USER_CHAR_SUB_INFO_REFRESH );
	pTargetUser->FillUserCharSubInfo( kPacket, iClassType );
	pSendUser->RelayPacket( kPacket );
}

void ServerNode::OnBattleRoomKickOut( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomKickOut 유저 NULL : %d", dwUserIndex );
		return;
	}
	pUser->BattleRoomKickOut();
}

void ServerNode::OnOfflineMemo( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwSendCount;
	rkPacket >> dwUserIndex >> dwSendCount;

	UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnOfflineMemo 유저 없음 : %d", dwUserIndex );
		return;
	}
	g_MemoNodeManager.SendMemo( pUser, dwSendCount );
}

void ServerNode::OnReserveCreateRoom( SP2Packet &rkPacket )
{
	bool bSafetyRoom;
	int  iModeType, iSubType, iModeMapNum, iUserSize;
	rkPacket >> bSafetyRoom >> iModeType >> iSubType >> iModeMapNum;

	if( iModeType == MT_TRAINING )       //광장
	{
		Room *pRoom = g_RoomNodeManager.CreateNewPlazaRoom( iSubType, iModeMapNum );
		if( pRoom )
		{
			ioHashString szPlazaName, szPlazaPW;
			int iMaxPlayer, iPlazaType;
			rkPacket >> szPlazaName >> szPlazaPW >> iMaxPlayer >> iPlazaType;
			if( !szPlazaName.IsEmpty() )
				pRoom->SetRoomName( szPlazaName );
			if( !szPlazaPW.IsEmpty() )
				pRoom->SetRoomPW( szPlazaPW );
			pRoom->SetMaxPlayer( iMaxPlayer );
			pRoom->SetPlazaModeType( (PlazaType)iPlazaType );
			pRoom->SetSubState( false );

			//유저들 입장
			rkPacket >> iUserSize;
			for(int i = 0;i < iUserSize;i++)
			{
				ioHashString szUserID;
				DWORD dwUserIndex;
				rkPacket >> dwUserIndex >> szUserID;
				if( i == 0 )
					pRoom->SetRoomMasterID( szUserID );

				pRoom->EnterReserveUser( dwUserIndex );
				SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
				kPacket << SS_MOVING_ROOM_JOIN << SS_MOVING_ROOM_JOIN_PLAZA << (int)pRoom->GetModeType() << pRoom->GetRoomIndex() << dwUserIndex;	
				kPacket << pRoom->GetModeSubNum() << pRoom->GetModeMapNum() << (int)pRoom->GetPlazaModeType() << pRoom->GetRoomNumber();

				UserCopyNode *pUser = GetUserNode( dwUserIndex );
				if( pUser )
					pUser->SendMessage( kPacket );
				else
				{
					SendMessage( kPacket );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnReserveCreateRoom Not User Copy Node Plaza : %d:%s", dwUserIndex, szUserID.c_str() );
				}
			}
		}
		else
		{
			//예외 리턴.
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnReserveCreateRoom Not Room Create Plaza");
		}
	}	
}

void ServerNode::OnReserveCreateBattleRoom( SP2Packet &rkPacket )
{
	bool bUserCreate;
	ioHashString szBattleRoomName, szBattleRoomPW;
	int iBlueCount, iRedCount, iObserver, iUserCount, iSelectTerm;
	rkPacket >> bUserCreate >> szBattleRoomName >> szBattleRoomPW >> iBlueCount >> iRedCount >> iObserver >> iSelectTerm >> iUserCount;

	// 파티 생성
	BattleRoomNode *pBattleRoom = g_BattleRoomManager.CreateNewBattleRoom();
	if( !pBattleRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnReserveCreateBattleRoom Not BattleRoom Node" );
		return;
	}

	if( !szBattleRoomName.IsEmpty() )
		pBattleRoom->SetName( szBattleRoomName );
	if( !szBattleRoomPW.IsEmpty() )
		pBattleRoom->SetPW( szBattleRoomPW );
	pBattleRoom->SetMaxPlayer( iBlueCount, iRedCount, iObserver );
	pBattleRoom->SetDefaultMode( iSelectTerm );

	// 예약 전투방은 즉시 동기화 발동
	pBattleRoom->SyncRealTimeCreate();

	for(int i = 0;i < iUserCount;i++)
	{
		DWORD dwUserIndex;
		ioHashString szPublicID;
		rkPacket >> dwUserIndex >> szPublicID;
		
		UserCopyNode *pUser = GetUserNode( dwUserIndex );
		if( pUser )
		{
			SP2Packet kPacket( SSTPK_RESERVE_CREATE_BATTLEROOM_RESULT );
			kPacket << dwUserIndex << pBattleRoom->GetIndex() << bUserCreate;
			SendMessage( kPacket );			
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnReserveCreateBattleRoom Not User Copy Node : %d:%s", dwUserIndex, szPublicID.c_str() );
		}		
	}
}

void ServerNode::OnReserveCreateBattleRoomResult( SP2Packet &rkPacket )
{
	bool bUserCreate;
	DWORD dwUserIndex, dwBattleRoomIndex;
	rkPacket >> dwUserIndex >> dwBattleRoomIndex >> bUserCreate;
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnReserveCreateBattleRoomResult 유저 NULL : %d", dwUserIndex );
		return;
	}

	pUser->ReserveBattleRoom( false );
	if( !pUser->EnterBattleRoom( dwBattleRoomIndex, this ) )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnReserveCreateBattleRoomResult 요청한 유저가 입장 실패: %s : %d", pUser->GetPublicID().c_str(), dwBattleRoomIndex );
		if( bUserCreate )
		{
			SP2Packet kPacket( STPK_CREATE_BATTLEROOM );
			kPacket << CREATE_BATTLEROOM_NOT;
			pUser->SendMessage( kPacket );
		}
	}
	else if( bUserCreate )
	{
		SP2Packet kPacket( STPK_CREATE_BATTLEROOM );
		kPacket << CREATE_BATTLEROOM_OK;
		pUser->SendMessage( kPacket );
	}
}

void ServerNode::OnExceptionBattleRoomLeave( SP2Packet &rkPacket )
{
	int iBattleRoomIndex;
	rkPacket >> iBattleRoomIndex;
	BattleRoomNode *pBattleRoom = g_BattleRoomManager.GetBattleRoomNode( iBattleRoomIndex );	
	if( !pBattleRoom ) return;

	DWORD dwUserIndex;
	ioHashString szPublicID;
	rkPacket >> dwUserIndex >> szPublicID;
	pBattleRoom->LeaveUser( dwUserIndex, szPublicID );

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnExceptionBattleRoomLeave(%d) : %s", iBattleRoomIndex, szPublicID.c_str() );
}

void ServerNode::OnCreateGuildResult( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwItemTableIndex;
	int iResult, iItemFieldNum;
	rkPacket >> dwUserIndex >> iResult >> dwItemTableIndex >> iItemFieldNum;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		if( g_UserNodeManager.GetGlobalUserNode( dwUserIndex ) == NULL )
		{
			// 유저 로그 아웃
			if( iResult == CREATE_GUILD_OK )
			{
				// 길드 생성이 성공했는데 유저가 로그아웃 상태라면 DB 아이템을 직접 삭제한다.
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnCreateGuildResult User Find Not CreateGuild OK: %d", dwUserIndex );
				g_DBClient.OnUpdateGuildEtcItemDelete( 0, 0, dwUserIndex, iItemFieldNum, dwItemTableIndex );
			}
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnCreateGuildResult 유저 NULL : %d", dwUserIndex );
		return;
	}

	if( iResult == CREATE_GUILD_OK )       //성공
	{
		g_DBClient.OnSelectCreateGuildInfo( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dwUserIndex );

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
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnCreateGuildResult CreateGuild Fail: %s - %I64d - E(%d)", pUser->GetPublicID().c_str(), pUser->GetMoney(), iResult );

		SP2Packet kPacket( STPK_CREATE_GUILD );
		kPacket << iResult;
		pUser->SendMessage( kPacket );
	}
}

void ServerNode::OnCreateGuildComplete( SP2Packet &rkPacket )
{
	DWORD dwUserIndex		= 0;
	DWORD dwAttendRcvDate	= 0;
	DWORD dwRankRcvDate		= 0;
	DWORD dwJoinDate		= 0;
	BOOL bActive			= FALSE;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnCreateGuildComplete 유저 NULL : %d", dwUserIndex );
		return;
	}

	DWORD dwGuildIndex	= 0;
	ioHashString szGuildName, szGuildPos;
	int iGuildMark	= 0;
	PACKET_GUARD_VOID( rkPacket.Read(dwGuildIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(szGuildName) );
	PACKET_GUARD_VOID( rkPacket.Read(iGuildMark) );
	PACKET_GUARD_VOID( rkPacket.Read(szGuildPos) );
	
	SP2Packet kPacket( STPK_CREATE_GUILD );
	kPacket << CREATE_GUILD_OK << pUser->GetMoney() << dwGuildIndex << szGuildName << iGuildMark << szGuildPos;
	pUser->SendMessage( kPacket );

	PACKET_GUARD_VOID( rkPacket.Read(dwAttendRcvDate) );
	PACKET_GUARD_VOID( rkPacket.Read(dwRankRcvDate) );
	PACKET_GUARD_VOID( rkPacket.Read(dwJoinDate) );
	PACKET_GUARD_VOID( rkPacket.Read(bActive) );

	ioUserGuild *pUserGuild = pUser->GetUserGuild();
	if( pUserGuild )
	{
		pUserGuild->SetGuildData( dwGuildIndex, szGuildName, szGuildPos, iGuildMark, true );
		pUserGuild->SetRecvAttendRewardDate(dwAttendRcvDate);
		pUserGuild->SetRecvRankRewardDate(dwRankRcvDate);
		pUserGuild->SetJoinDate(dwJoinDate);
		pUserGuild->SetGuildRoomState(bActive);
	}
}

void ServerNode::OnGuildEntryAgree( SP2Packet &rkPacket )
{
	DWORD dwUserIndex	= 0, dwGuildIndex	= 0;
	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwGuildIndex) );

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		if( g_UserNodeManager.GetGlobalUserNode( dwUserIndex ) == NULL )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnGuildEntryAgree 유저(%d) 유저가 접속 종료하여 래더 포인트 요청", dwUserIndex );
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnGuildEntryAgree 유저 NULL : %d", dwUserIndex );
		}
		return;
	}

	ioUserGuild *pUserGuild = pUser->GetUserGuild();
	if( pUserGuild )
	{
		ioHashString szGuildName, szGuildPos;
		int iGuildMark	= 0, iGuildEvent	= 0;
		DWORD dwAttendRcvDate	= 0;
		DWORD dwRankRcvDate		= 0;
		DWORD dwJoinDate		= 0;
		BOOL bActive			= FALSE;

		PACKET_GUARD_VOID( rkPacket.Read(szGuildName) );
		PACKET_GUARD_VOID( rkPacket.Read(szGuildPos) );
		PACKET_GUARD_VOID( rkPacket.Read(iGuildMark) );
		PACKET_GUARD_VOID( rkPacket.Read(iGuildEvent) );
		PACKET_GUARD_VOID( rkPacket.Read(dwAttendRcvDate) );
		PACKET_GUARD_VOID( rkPacket.Read(dwRankRcvDate) );
		PACKET_GUARD_VOID( rkPacket.Read(dwJoinDate) );
		PACKET_GUARD_VOID( rkPacket.Read(bActive) );

		pUserGuild->SetGuildDataEntryAgree( dwGuildIndex, szGuildName, szGuildPos, iGuildMark, true );
		pUserGuild->SetRecvAttendRewardDate(dwAttendRcvDate);
		pUserGuild->SetRecvRankRewardDate(dwRankRcvDate);
		pUserGuild->SetJoinDate(dwJoinDate);
		pUserGuild->SetGuildRoomState(bActive);

		CTime cCurTime = CTime::GetCurrentTime();
		DWORD dwCurTime	= cCurTime.GetTime();

		//유저에게 길드 패킷 전송
		SP2Packet kPacket( STPK_MY_GUILD_INFO );
		PACKET_GUARD_VOID(kPacket.Write(dwGuildIndex));
		PACKET_GUARD_VOID(kPacket.Write(szGuildName));
		PACKET_GUARD_VOID(kPacket.Write(szGuildPos));
		PACKET_GUARD_VOID(kPacket.Write(iGuildMark));
		PACKET_GUARD_VOID(kPacket.Write(iGuildEvent));
		PACKET_GUARD_VOID(kPacket.Write(pUserGuild->GetRecvAttendRewardDate()));
		PACKET_GUARD_VOID(kPacket.Write(pUserGuild->GetGuildJoinDate()));
		PACKET_GUARD_VOID(kPacket.Write(dwCurTime));
		PACKET_GUARD_VOID(kPacket.Write(bActive));
		pUser->SendMessage( kPacket );

		g_DBClient.OnUpdateGuildMemberEvent( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dwUserIndex, dwGuildIndex );
		g_DBClient.OnSelectGuildMemberListEx( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dwUserIndex, dwGuildIndex, 0, false );
	}	
}

void ServerNode::OnGuildMemberListEx( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnGuildMemberListEx 유저 NULL : %d", dwUserIndex );
		return;
	}

	if( !pUser->IsGuild() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnGuildMemberListEx 길드 유저가 아님 : %d", dwUserIndex );
		return;
	}

	ioUserGuild *pUserGuild = pUser->GetUserGuild();
	if( pUserGuild )
	{
		int iCommand;
		rkPacket >> iCommand;
		switch( iCommand )
		{
		case SSTPK_GUILD_MEMBER_LIST_ALL:
			pUserGuild->GuildEntryAgreeSync();
			break;
		case SSTPK_GUILD_MEMBER_LIST_ME:
			g_DBClient.OnSelectGuildMemberListEx( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), pUserGuild->GetGuildIndex(), 0, pUserGuild->IsGuildMaster() );
			break;
		}
	}	
}

void ServerNode::OnGuildInvitation( SP2Packet &rkPacket )
{
	ioHashString szUserID;
	DWORD dwSendIndex, dwRecvIndex;
	rkPacket >> dwSendIndex >> dwRecvIndex >> szUserID;

	User *pRecvUser = GetUserOriginalNode( dwRecvIndex, rkPacket );
	if( pRecvUser == NULL )
	{
		UserParent *pSendUser = g_UserNodeManager.GetGlobalUserNode( dwSendIndex );
		if( pSendUser )
		{
			// 오프 유저임을 알림
			SP2Packet kPacket( STPK_GUILD_INVITATION );
			kPacket << GUILD_INVITATION_OFFLINE << szUserID;
			pSendUser->RelayPacket( kPacket );
		}		
	}
	else
	{
		UserParent *pSendUser = g_UserNodeManager.GetGlobalUserNode( dwSendIndex );
		if( pSendUser )
		{
			int   iGuildMark;
			DWORD dwGuildIndex;
			ioHashString szGuildName;
			rkPacket >> dwGuildIndex >> iGuildMark >> szGuildName;
			pRecvUser->_OnGuildInvitation( pSendUser, dwGuildIndex, iGuildMark, szGuildName );
		}
	}
}

void ServerNode::OnGuildMasterChange( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwTargetIndex, dwGuildIndex;
	rkPacket >> dwUserIndex >> dwTargetIndex >> dwGuildIndex;

	User *pTargetUser = GetUserOriginalNode( dwTargetIndex, rkPacket );
	if( pTargetUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnGuildMasterChange Not User : %d", dwTargetIndex );
		UserParent *pSendUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
		if( pSendUser )
		{
			// 서버 이동중 잠시 후 다시 시도 알림
			SP2Packet kPacket( STPK_GUILD_MASTER_CHANGE );
			kPacket << GUILD_MASTER_CHANGE_DELAY;
			pSendUser->RelayPacket( kPacket );
		}		
		return;
	}
	else
	{
		ioUserGuild *pUserGuild = pTargetUser->GetUserGuild();
		if( pUserGuild )			
		{
			if( pUserGuild->GetGuildIndex() == dwGuildIndex )        //아직 같은 길드면 길드명 변경
			{
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal )
					pUserGuild->SetGuildPosition( ioHashString( pLocal->GetGuildMasterPostion() ) );	
				g_DBClient.OnUpdateGuildMasterChange( pTargetUser->GetUserDBAgentID(), pTargetUser->GetAgentThreadID(), dwUserIndex, dwTargetIndex, dwGuildIndex );
			}		
			else            //이미 탈퇴한 유저 알림
			{
				UserParent *pSendUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
				if( pSendUser )
				{
					SP2Packet kPacket( SSTPK_GUILD_MASTER_CHANGE );
					kPacket << GUILD_MASTER_CHANGE_LEAVEUSER;
					pSendUser->RelayPacket( kPacket );
				}
			}
		}						
	}	
}

void ServerNode::OnGuildPositionChange( SP2Packet &rkPacket )
{
	ioHashString szGuildPos;
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex >> szGuildPos;
	
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnGuildPositionChange Not User : %d : %s", dwUserIndex, szGuildPos.c_str() );
	}
	else
	{
		ioUserGuild *pUserGuild = pUser->GetUserGuild();
		if( pUserGuild )
			pUserGuild->SetGuildPosition( szGuildPos );
	}
}

void ServerNode::OnGuildKickOut( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwSendIndex, dwGuildIndex;
	rkPacket >> dwUserIndex >> dwSendIndex >> dwGuildIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnGuildKickOut Not User : %d", dwUserIndex );
	}
	else
	{
		ioUserGuild *pUserGuild = pUser->GetUserGuild();
		if( pUserGuild )			
		{
			if( pUserGuild->GetGuildIndex() == dwGuildIndex )        //아직 같은 길드면 길드장 변경
			{
				// DB 실행
				g_DBClient.OnDeleteGuildLeaveUser( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), pUserGuild->GetGuildIndex() );

				// 메인 서버에 알림
				SP2Packet kMainPacket( MSTPK_GUILD_LEAVE );
				kMainPacket << pUserGuild->GetGuildIndex();
				g_MainServer.SendMessage( kMainPacket );

				SP2Packet kPacket( STPK_GUILD_KICK_OUT );
				kPacket << GUILD_KICK_OUT_OK << pUserGuild->GetGuildIndex() << pUser->GetPublicID() << false;
				pUser->SendMessage( kPacket );

				// 길드 정보 초기화
				pUserGuild->LeaveGuildInitInfo( true );
			}		
			else            
			{
				UserParent *pSendUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
				if( pSendUser )
				{
					SP2Packet kPacket( STPK_GUILD_KICK_OUT );
					kPacket << GUILD_KICK_OUT_LEAVEUSER;
					pSendUser->RelayPacket( kPacket );
				}
			}
		}						
	}
}

void ServerNode::OnFriendDelete( SP2Packet &rkPacket )
{
	ioHashString szUserID, szFriendID;
	rkPacket >> szUserID >> szFriendID;

	User *pUser = GetUserOriginalNode( szUserID, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnFriendDelete Not User : %s", szUserID.c_str() );
	}
	else
	{
		pUser->_OnBestFriendDismiss( szFriendID, false );
		pUser->DeleteFriend( szFriendID );
		SP2Packet kPacket( STPK_FRIEND_DELETE );
		kPacket << szFriendID;
		pUser->SendMessage( kPacket );
		pUser->CheckRoomBonusTable();
	}
}

void ServerNode::OnGuildMarkChange( SP2Packet &rkPacket )
{
	bool bBlock;
	DWORD dwUserIndex, dwGuildIndex, dwGuildMark;
	rkPacket >> dwUserIndex >> dwGuildIndex >> dwGuildMark >> bBlock;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnGuildMarkChange Not User : %d", dwUserIndex );
	}
	else
	{
		pUser->_OnGuildMarkChange( dwGuildIndex, dwGuildMark, bBlock );
	}
}

void ServerNode::OnGuildUserDelete( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnGuildUserDelete Not User : %d", dwUserIndex );
	}
	else
	{
		DWORD dwGuildIndex;
		ioHashString szUserID;
		rkPacket >> szUserID >> dwGuildIndex;

		ioUserGuild *pUserGuild = pUser->GetUserGuild();
		if( pUserGuild )
		{
			if( pUserGuild->GetGuildIndex() == dwGuildIndex )
			{
				pUserGuild->DeleteGuildUser( szUserID );

				// 유저에게 전송
				DWORD dwRelayPacketID;
				int iResultType;
				rkPacket >> dwRelayPacketID >> iResultType;

				SP2Packet kPacket( dwRelayPacketID );
				kPacket << iResultType << dwGuildIndex << szUserID;
				pUser->SendMessage( kPacket );
			}
		}
	}
}

void ServerNode::OnLadderTeamSync( SP2Packet &rkPacket )
{
	// 원본 - > 카피본에게 정보 동기화
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case LadderTeamSync::LTS_CHANGEINFO:
		{
			DWORD dwLadderTeamIndex;
			rkPacket >> dwLadderTeamIndex;
			LadderTeamCopyNode *pCopyNode = GetLadderTeamNode( dwLadderTeamIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncChangeInfo( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamSync Not Room : %d - %d - %s:%d", iSyncType, dwLadderTeamIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;	
	case LadderTeamSync::LTS_CHANGERECORD:
		{
			DWORD dwLadderTeamIndex;
			rkPacket >> dwLadderTeamIndex;
			LadderTeamCopyNode *pCopyNode = GetLadderTeamNode( dwLadderTeamIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncChangeRecord( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamSync Not Room : %d - %d - %s:%d", iSyncType, dwLadderTeamIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case LadderTeamSync::LTS_CREATE:
		{
			DWORD dwLadderTeamIndex;
			rkPacket >> dwLadderTeamIndex;
			LadderTeamCopyNode *pNewNode = CreateNewLadderTeam( dwLadderTeamIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncCreate( rkPacket );
				g_LadderTeamManager.AddCopyLadderTeam( pNewNode );
			}
		}
		break;
	case LadderTeamSync::LTS_DESTROY:
		{
			DWORD dwLadderTeamIndex;
			rkPacket >> dwLadderTeamIndex;
			RemoveLadderTeamNode( dwLadderTeamIndex );
		}		
		break; 
	}
}

void ServerNode::OnLadderTeamTransfer( SP2Packet &rkPacket )
{
	// 카피본 - > 원본 정보 전송
	int iTransferType	= 0;
	DWORD dwLadderTeamIndex = 0;

	PACKET_GUARD_VOID( rkPacket.Read(iTransferType) );
	PACKET_GUARD_VOID( rkPacket.Read(dwLadderTeamIndex) );

	LadderTeamNode *pLadderTeam = g_LadderTeamManager.GetLadderTeamNode( dwLadderTeamIndex );	
	switch( iTransferType )
	{
	case LadderTeamParent::ENTER_USER:
		{
			DWORD dwUserIndex, dwGuildIndex, dwGuildMark;
			int   iGradeLevel, iAbilityLevel, iHeroMatchPoint, iLadderPoint, iClientPort, iTransferPort;
			ioHashString szPublicID, szPublicIP, szPrivateIP, szTransferIP;
			rkPacket >> dwUserIndex >> szPublicID >> iGradeLevel >> iAbilityLevel >> iHeroMatchPoint >> iLadderPoint >> dwGuildIndex >> dwGuildMark
					 >> szPublicIP >> szPrivateIP >> szTransferIP >> iClientPort >> iTransferPort;
			if( pLadderTeam == NULL )
			{
				SP2Packet kPacket( SSTPK_LADDERTEAM_JOIN_RESULT );
				kPacket << dwUserIndex << LADDERTEAM_JOIN_NOT_LADDERTEAM << dwLadderTeamIndex;
				SendMessage( kPacket );				
			}
			else if( pLadderTeam->IsMatchPlay() )
			{
				SP2Packet kPacket( SSTPK_LADDERTEAM_JOIN_RESULT );
				kPacket << dwUserIndex << LADDERTEAM_JOIN_MATCH_PLAY << dwLadderTeamIndex;
				SendMessage( kPacket );
			}
			else if( pLadderTeam->IsFull() )
			{
				SP2Packet kPacket( SSTPK_LADDERTEAM_JOIN_RESULT );
				kPacket << dwUserIndex << LADDERTEAM_JOIN_FULL << dwLadderTeamIndex << pLadderTeam->GetJoinUserCnt() << pLadderTeam->GetMaxPlayer();
				SendMessage( kPacket );						
			}					
			else
			{				
				pLadderTeam->EnterUser( dwUserIndex, szPublicID, iGradeLevel, iAbilityLevel, iHeroMatchPoint, iLadderPoint, dwGuildIndex, dwGuildMark, szPublicIP, szPrivateIP, szTransferIP, iClientPort, iTransferPort );
			}			
		}
		break;
	case LadderTeamParent::LEAVE_USER:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer LEAVE_USER 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			DWORD dwUserIndex;
			ioHashString szPublicID;
			rkPacket >> dwUserIndex >> szPublicID;
			pLadderTeam->LeaveUser( dwUserIndex, szPublicID );
		}
		break;
	case LadderTeamParent::TEAM_STATE:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer TEAM_STATE 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}
			DWORD dwTeamState;
			rkPacket >> dwTeamState;
			pLadderTeam->SetTeamState( dwTeamState );
		}
		break;
	case LadderTeamParent::MATCH_ROOM_REQUEST:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer TEAM_STATE 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			DWORD dwRequestIndex;
			rkPacket >> dwRequestIndex;
			pLadderTeam->MatchRoomRequest( dwRequestIndex );
		}
		break;
	case LadderTeamParent::MATCH_ROOM_REQUEST_JOIN:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer MATCH_ROOM_REQUEST_JOIN 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}
			
			DWORD dwOtherTeamIndex	= 0;
			int iModeType = 0;
			int iModeSubNum = 0;
			int iModeMapNum = 0; 
			int iCampType = 0;
			int iTeamType = 0;

			PACKET_GUARD_VOID( rkPacket.Read(dwOtherTeamIndex) );
			PACKET_GUARD_VOID( rkPacket.Read(iModeType) );
			PACKET_GUARD_VOID( rkPacket.Read(iModeSubNum) );
			PACKET_GUARD_VOID( rkPacket.Read(iModeMapNum) );
			PACKET_GUARD_VOID( rkPacket.Read(iCampType) );
			PACKET_GUARD_VOID( rkPacket.Read(iTeamType) );

            pLadderTeam->MatchRoomRequestJoin( dwOtherTeamIndex, (ModeType)iModeType, iModeSubNum, iModeMapNum, iCampType, iTeamType );
		}
		break;
	case LadderTeamParent::MATCH_ROOM_RESERVE_CANCEL:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer MATCH_ROOM_RESERVE_CANCEL 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			pLadderTeam->MatchReserveCancel();
		}
		break;
	case LadderTeamParent::MATCH_ROOM_END_SYNC:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer MATCH_ROOM_END_SYNC 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			pLadderTeam->MatchPlayEndSync();
		}
		break;
	case LadderTeamParent::TEAM_INFO:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer TEAM_INFO 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			DWORD dwUserIndex;
			rkPacket >> dwUserIndex;
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
			if( pUser == NULL )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LadderTeamParent::TEAM_INFO 유저 NULL : %d", dwUserIndex );
				return;
			}
			pLadderTeam->OnLadderTeamInfo( pUser );
		}
		break;
	case LadderTeamParent::USER_INFO:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer USER_INFO 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			DWORD dwUserIndex, dwGuildIndex, dwGuildMark;
			int   iGradeLevel, iAbilityLevel, iHeroMatchPoint, iLadderPoint, iClientPort, iTransferPort;
			ioHashString szTransferIP;
			rkPacket >> dwUserIndex >> iGradeLevel >> iAbilityLevel >> iHeroMatchPoint >> iLadderPoint >> dwGuildIndex >> dwGuildMark >> iClientPort >> szTransferIP >> iTransferPort;
			pLadderTeam->UserInfoUpdate( dwUserIndex, iGradeLevel, iAbilityLevel, iHeroMatchPoint, iLadderPoint, dwGuildIndex, dwGuildMark,  iClientPort, szTransferIP, iTransferPort );
		}
		break;
	case LadderTeamParent::UPDATE_RECORD:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer UPDATE_RECORD 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}
			int iWinTeam, iPlayTeam;
			rkPacket >> iWinTeam >> iPlayTeam;
			pLadderTeam->UpdateRecord( (TeamType)iPlayTeam, (TeamType)iWinTeam );
			g_LadderTeamManager.SortLadderTeamRank( pLadderTeam->IsHeroMatchMode() ); //랭킹 정렬
		}
		break;
	case LadderTeamParent::UPDATE_GUILDMARK:
		{
		}
		break;
	case LadderTeamParent::UDP_CHANGE:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer UDP_CHANGE 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			DWORD dwUserIndex;
			int   iClientPort, iTransferPort;
			ioHashString szPublicID, szPublicIP, szPrivateIP, szTransferIP;
			rkPacket >> dwUserIndex >> szPublicID >> szPublicIP >> iClientPort >> szPrivateIP >> szTransferIP >> iTransferPort;
			pLadderTeam->UserUDPChange( dwUserIndex, szPublicID, szPublicIP, iClientPort, szPrivateIP, szTransferIP, iTransferPort );
		}
		break;
	case LadderTeamParent::TRANSFER_PACKET:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer TRANSFER_PACKET 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			DWORD dwUserIndex, dwPacketID;
			rkPacket >> dwUserIndex >> dwPacketID;				
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 2 );
			// 패킷 생성
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
			pLadderTeam->SendPacketTcp( kPacket, dwUserIndex );			
		}
		break;
	case LadderTeamParent::TRANSFER_PACKET_USER:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer TRANSFER_PACKET_USER 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			ioHashString szSenderName;
			DWORD dwPacketID;
			rkPacket >> szSenderName >> dwPacketID;				
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 1 ) + lstrlen( szSenderName.c_str() ) + 1;
			// 패킷 생성
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
			pLadderTeam->SendPacketTcpUser( kPacket, szSenderName );			
		}
		break;
	case LadderTeamParent::USER_PACKET_TRANSFER:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer USER_PACKET_TRANSFER 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			DWORD dwUserIndex, dwPacketID;
			rkPacket >> dwUserIndex >> dwPacketID;	
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
			if( pUser == NULL )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LadderTeamParent::USER_PACKET_TRANSFER 유저 NULL : %d - 0x%x", dwUserIndex, dwPacketID );
				return;
			}
			// 패킷 생성
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 2 );
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize, true );
			pLadderTeam->OnProcessPacket( kPacket, pUser );
		}
		break;
	case LadderTeamParent::TRANSFER_PACKET_UDP:
		{
			if( !pLadderTeam )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamTransfer TRANSFER_PACKET_UDP 중계하려는 길드팀 없음 %d", dwLadderTeamIndex );
				return;
			}

			DWORD dwUserIndex, dwPacketID;
			rkPacket >> dwUserIndex >> dwPacketID;				
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 2 );
			// 패킷 생성
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
			pLadderTeam->SendPacketUdp( kPacket, dwUserIndex );			
		}
		break;
	}
}

void ServerNode::OnExceptionLadderTeamLeave( SP2Packet &rkPacket )
{
	DWORD dwLadderTeamIndex;
	
	PACKET_GUARD_VOID( rkPacket.Read(dwLadderTeamIndex) ) ; 

	LadderTeamNode *pLadderTeam = g_LadderTeamManager.GetLadderTeamNode( dwLadderTeamIndex );	
	if( !pLadderTeam ) return;

	DWORD dwUserIndex;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) ); 
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	pLadderTeam->LeaveUser( dwUserIndex, szPublicID );

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnExceptionLadderTeamLeave(%d) : %s", dwLadderTeamIndex, szPublicID.c_str() );
}

void ServerNode::OnReserveCreateLadderTeam( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	LadderTeamNode *pLadderTeam = g_LadderTeamManager.CreateNewLadderTeam();
	if( !pLadderTeam )
	{
		UserCopyNode *pUser = GetUserNode( dwUserIndex );
		if( pUser )
		{
			SP2Packet kPacket( STPK_CREATE_LADDERTEAM );
			kPacket << LADDERTEAM_CREATE_NOT;
			pUser->RelayPacket( kPacket );			
			return;
		}
	}	

	DWORD dwJoinGuildIndex;
	int iCampType, iModeSelectType, iLadderMaxPlayer;
	ioHashString szTeamName, szPassword;
	bool bHeroMatchMode;
	rkPacket >> iCampType >> iModeSelectType >> szTeamName >> szPassword >> iLadderMaxPlayer >> dwJoinGuildIndex >> bHeroMatchMode;

	pLadderTeam->SetCampType( iCampType );
	pLadderTeam->SetTeamName( szTeamName );
	pLadderTeam->SetTeamPW( szPassword );
	pLadderTeam->SetMaxPlayer( iLadderMaxPlayer );
	pLadderTeam->SetJoinGuildIndex( dwJoinGuildIndex );
	pLadderTeam->SelectMode( iModeSelectType );
	pLadderTeam->SetHeroMatchMode( bHeroMatchMode );
	pLadderTeam->SyncRealTimeCreate();
	UserCopyNode *pUser = GetUserNode( dwUserIndex );
	if( pUser )
	{
		SP2Packet kPacket( SSTPK_RESERVE_CREATE_LADDERTEAM_RESULT );
		kPacket << dwUserIndex << pLadderTeam->GetIndex();
		SendMessage( kPacket );			
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnReserveCreateLadderTeam Not User Copy Node : %d", dwUserIndex );
	}
}

void ServerNode::OnReserveCreateLadderTeamResult( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwLadderTeamIndex;
	rkPacket >> dwUserIndex >> dwLadderTeamIndex;
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnReserveCreateLadderTeamResult 유저 NULL : %d", dwUserIndex );
		return;
	}

	pUser->ReserveLadderTeam( false );
	if( !pUser->EnterLadderTeam( dwLadderTeamIndex, this ) )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnReserveCreateLadderTeamResult 요청한 유저가 입장 실패: %s : %d", pUser->GetPublicID().c_str(), dwLadderTeamIndex );
		
		SP2Packet kPacket( STPK_CREATE_LADDERTEAM );
		kPacket << LADDERTEAM_CREATE_NOT;
		pUser->SendMessage( kPacket );
	}
}

void ServerNode::OnLadderTeamJoinResult( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwLadderTeamIndex;
	int   iResultType;
	rkPacket >> dwUserIndex >> iResultType >> dwLadderTeamIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamJoinResult 유저 NULL : %d", dwUserIndex );
		return;
	}		
	
	switch( iResultType )
	{
	case LADDERTEAM_JOIN_NOT_LADDERTEAM:
	case LADDERTEAM_JOIN_MATCH_PLAY:
		{
			pUser->LeaveLadderTeam();
			SP2Packet kPacket( STPK_JOIN_LADDERTEAM );
			kPacket << iResultType;
			pUser->SendMessage( kPacket );	
		}
		break;
	case LADDERTEAM_JOIN_FULL:
		{
			pUser->LeaveLadderTeam();
			//인원 초과
			SP2Packet kPacket( STPK_JOIN_LADDERTEAM );
			kPacket << iResultType;
			pUser->SendMessage( kPacket );

			LadderTeamCopyNode *pLadderTeam = GetLadderTeamNode( dwLadderTeamIndex );
			if( pLadderTeam )
			{
				int iJoinUserCount, iMaxPlayer;
				rkPacket >> iJoinUserCount >> iMaxPlayer;
				pLadderTeam->SetJoinUserCnt( iJoinUserCount );
				pLadderTeam->SetMaxPlayer( iMaxPlayer );
			}
		}
		break;
	}
}

void ServerNode::OnLadderTeamKickOut( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamKickOut 유저 NULL : %d", dwUserIndex );
		return;
	}
	pUser->LadderTeamKickOut();
}

void ServerNode::OnLadderTeamFollow( SP2Packet &rkPacket )
{
	int iNextPos;
	DWORD dwUserIndex, dwLadderTeamIndex;
	rkPacket >> dwUserIndex >> dwLadderTeamIndex >> iNextPos;
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamFollow 유저 NULL : %d", dwUserIndex );
		return;
	}
	pUser->_OnLadderTeamFollow( g_LadderTeamManager.GetGlobalLadderTeamNode( dwLadderTeamIndex ), iNextPos );	
}

void ServerNode::OnLadderTeamEnterRoomUser( SP2Packet &rkPacket )
{
	DWORD dwLadderTeamIndex;
	rkPacket >> dwLadderTeamIndex;
	LadderTeamNode *pLadderTeam = g_LadderTeamManager.GetLadderTeamNode( dwLadderTeamIndex );
	if( pLadderTeam )
	{
		if( !pLadderTeam->MatchRequstTeamEnterRoom( rkPacket ) )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnLadderTeamEnterRoomUser[%d] 룸 입장 실패!!", dwLadderTeamIndex );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLadderTeamEnterRoomUser pLadderTeam == NULL : %d", dwLadderTeamIndex );
	}
}

void ServerNode::OnCampSeasonBonus( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwBonusIndex;
	int iBlueCampPoint, iBlueCampBonusPoint, iBlueEntry, iRedCampPoint, iRedCampBonusPoint, iRedEntry, iMyCampType, iMyCampPoint, iMyCampRank;

	rkPacket >> dwUserIndex >> dwBonusIndex >> iBlueCampPoint >> iBlueCampBonusPoint >> iBlueEntry 
             >> iRedCampPoint >> iRedCampBonusPoint >> iRedEntry >> iMyCampType >> iMyCampPoint >> iMyCampRank;
	
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		pUser->AddCampSeasonBonus( dwBonusIndex, iBlueCampPoint, iBlueCampBonusPoint, iBlueEntry,
								   iRedCampPoint, iRedCampBonusPoint, iRedEntry, iMyCampType, iMyCampPoint, iMyCampRank );
	}
}

void ServerNode::OnGuildNameChange( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwMasterIndex, dwGuildIndex;
	ioHashString szNewGuildName;
	rkPacket >> dwUserIndex >> dwMasterIndex >> dwGuildIndex >> szNewGuildName;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		ioUserGuild *pUserGuild = pUser->GetUserGuild();
		if( pUserGuild && pUserGuild->GetGuildIndex() == dwGuildIndex )
		{
			pUserGuild->SetGuildName( szNewGuildName );
			SP2Packet kUserPacket( STPK_GUILD_NAME_CHANGE );
			kUserPacket << GUILD_NAME_CHANGE_OK << dwMasterIndex << pUserGuild->GetGuildIndex() << pUserGuild->GetGuildName();
			pUser->SendMessage( kUserPacket );		
		}
	}
}

void ServerNode::OnGuildNameChangeResult( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwGuildIndex, dwItemTableIndex;
	int iResult, iItemFieldNum;	
	char szGuildName[GUILD_NAME_NUM_PLUS_ONE] = "";
	rkPacket >> dwUserIndex >> iResult >> dwGuildIndex >> szGuildName >> dwItemTableIndex >> iItemFieldNum;
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		if( g_UserNodeManager.GetGlobalUserNode( dwUserIndex ) == NULL )
		{
			// 유저 로그 아웃
			if( iResult == GUILD_NAME_CHANGE_OK )
			{
				// 길드명 변경이 성공했는데 유저가 로그아웃 상태라면 DB 아이템을 직접 삭제한다.
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "OnGuildNameChangeResult User Find Not GuildNameChange OK: %d", dwUserIndex );
				g_DBClient.OnUpdateGuildEtcItemDelete( 0, 0, dwUserIndex, iItemFieldNum, dwItemTableIndex );
			}
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnGuildNameChangeResult 유저 NULL : %d", dwUserIndex );
		return;
	}

	if( iResult == GUILD_NAME_CHANGE_OK )       //성공
	{
		ioUserGuild *pUserGuild = pUser->GetUserGuild();
		if( !pUserGuild || !pUser->IsGuild() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "길드없는 유저가 길드명 변경  치명적인 에러 2: %s - %d:%s", pUser->GetPublicID().c_str(), dwGuildIndex, szGuildName );
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
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "GuildNameChange Fail: %s - E(%d)", pUser->GetPublicID().c_str(), iResult );
	}
}

void ServerNode::OnPresentSelect( SP2Packet &rkPacket )
{
	DWORD dwUserIndex  = 0;
	int   iSelectCount = 0;
	rkPacket >> dwUserIndex >> iSelectCount;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnPresentSelect 유저 NULL : %d", dwUserIndex );
		return;
	}

	pUser->_OnSelectPresent( iSelectCount );
}

void ServerNode::OnSubscriptionSelect( SP2Packet &rkPacket )
{
	DWORD dwUserIndex  = 0;
	int   iSelectCount = 0;
	rkPacket >> dwUserIndex >> iSelectCount;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnSubscriptionSelect 유저 NULL : %d", dwUserIndex );
		return;
	}

	pUser->_OnSelectSubscription( iSelectCount );
}

void ServerNode::OnUserHeroData( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) ); //rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUserHeroData 유저 NULL : %d", dwUserIndex );
		return;
	}

	UserHeroData kUserHeroData;
	PACKET_GUARD_VOID( rkPacket.Read(kUserHeroData.m_iHeroTitle) );
	PACKET_GUARD_VOID( rkPacket.Read(kUserHeroData.m_iHeroTodayRank) );
	for(int i = 0;i < HERO_SEASON_RANK_MAX;i++)
	{
		PACKET_GUARD_VOID( rkPacket.Read(kUserHeroData.m_iHeroSeasonRank[i]) );
	}
	pUser->SetUserHeroData( kUserHeroData );
}

void ServerNode::OnHeroMatchOtherInfo( SP2Packet &rkPacket )
{
	DWORD dwTargetIndex, dwSendIndex;
	rkPacket >> dwTargetIndex >> dwSendIndex;

	User *pTargetUser = GetUserOriginalNode( dwTargetIndex, rkPacket );
	if( pTargetUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnHeroMatchOtherInfo 타겟 유저 NULL : %d", dwTargetIndex );
		return;
	}

	UserCopyNode *pSendUser = GetUserNode( dwSendIndex );
	if( pSendUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnHeroMatchOtherInfo 요청 유저 NULL : %d", dwSendIndex );
		return;
	}

	// 툴팁 정보
	SP2Packet kPacket( STPK_HERO_MATCH_OTHER_INFO );
	pTargetUser->FillHeroMatchInfo( kPacket );
	pSendUser->RelayPacket( kPacket );
}

void ServerNode::OnUserCharRentalAgree( SP2Packet &rkPacket )
{
	DWORD dwAgreeDBAgentID;
	ioHashString szTargetID, szAgreeID;
	rkPacket >> szTargetID >> szAgreeID >> dwAgreeDBAgentID;

	User *pTargetUser = GetUserOriginalNode( szTargetID, rkPacket );
	if( pTargetUser == NULL )
	{
		if( g_UserNodeManager.GetGlobalUserNode( szTargetID ) == NULL )
		{
			CHARACTER kCharInfo;
			rkPacket >> kCharInfo;

			RentalData kRentalData;
			kRentalData.ApplyData( rkPacket );

			// Agree 유저 용병 반환 
			UserParent *pAgreeUserParent = g_UserNodeManager.GetGlobalUserNode( szAgreeID );
			if( pAgreeUserParent )
			{
				SP2Packet kPacket( STPK_USER_CHAR_RENTAL_AGREE );
				kPacket << USER_CHAR_RENTAL_AGREE_NONE_USER << kCharInfo.m_class_type;
				pAgreeUserParent->RelayPacket( kPacket );
			}
			g_DBClient.OnUpdateCharRentalTime( dwAgreeDBAgentID, szAgreeID.GetHashCode(), kRentalData.m_dwCharIndex, 0 );
		}
		return;
	}

	CHARACTER kCharInfo;
	rkPacket >> kCharInfo;

	RentalData kRentalData;
	kRentalData.ApplyData( rkPacket );

	pTargetUser->_OnUserCharRentalAgree( g_UserNodeManager.GetGlobalUserNode( szAgreeID ), szAgreeID, dwAgreeDBAgentID, kCharInfo, kRentalData );
}

void ServerNode::OnJoinHeadquartersUser( SP2Packet &rkPacket )
{
	bool bInvited;
	int iMapIndex;
	DWORD dwRequestUser, dwOwnerUser;
	rkPacket >> dwRequestUser >> dwOwnerUser >> iMapIndex >> bInvited;

	User *pUser = GetUserOriginalNode( dwOwnerUser, rkPacket );
	if( pUser == NULL )
	{
		if( g_UserNodeManager.GetGlobalUserNode( dwOwnerUser ) == NULL )
		{
			UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
			if( pUserParent )
			{
				SP2Packet kPacket( STPK_JOIN_HEADQUARTERS );
				kPacket << JOIN_HEADQUARTERS_OWNER_OFFLINE;
				pUserParent->RelayPacket( kPacket );
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnJoinHeadquartersUser User NULL(%d)", dwOwnerUser );
		}
		return;
	}
	pUser->_OnJoinHeadquarters( g_UserNodeManager.GetGlobalUserNode( dwRequestUser ), iMapIndex, bInvited );
}

void ServerNode::OnHeadquartersInfo( SP2Packet &rkPacket )
{
	ioHashString szOwnerName;
	DWORD dwRequestUser, dwOwnerUser;
	rkPacket >> dwRequestUser >> dwOwnerUser >> szOwnerName;

	User *pUser = GetUserOriginalNode( dwOwnerUser, rkPacket );
	if( pUser == NULL )
	{
		if( g_UserNodeManager.GetGlobalUserNode( dwOwnerUser ) == NULL )
		{
			UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
			if( pUserParent )
			{
				SP2Packet kPacket( STPK_HEADQUARTERS_INFO );
				kPacket << szOwnerName << (MAX_PLAYER / 2) << false << 0;       // - 
				pUserParent->RelayPacket( kPacket );
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnHeadquartersInfo User NULL(%d)", dwOwnerUser );
		}
		return;
	}
	pUser->_OnHeadquartersInfo( g_UserNodeManager.GetGlobalUserNode( dwRequestUser ) );
}

void ServerNode::OnHeadquartersRoomInfo( SP2Packet &rkPacket )
{
	int iRoomIndex;
	rkPacket >> iRoomIndex;

	bool bLock;
	DWORD dwRequestUser;
	ioHashString szMasterName;
	rkPacket >> szMasterName >> bLock >> dwRequestUser;
	
	Room *pRoom = g_RoomNodeManager.GetHeadquartersNode( iRoomIndex );
	if( pRoom == NULL )
	{
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_HEADQUARTERS_INFO );
			kPacket << szMasterName << (MAX_PLAYER / 2) << bLock << 0;       // - 
			pUserParent->RelayPacket( kPacket );
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnHeadquartersRoomInfo Room NULL(%d)", iRoomIndex );
	}
	else
	{
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_HEADQUARTERS_INFO );
			pRoom->FillHeadquartersInfo( szMasterName, bLock, kPacket );
			pUserParent->RelayPacket( kPacket );
		}
	}
}

void ServerNode::OnHeadquartersJoinAgree( SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwRoomIndex;
	rkPacket >> dwUserIndex >> dwRoomIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		return;
	}

	pUser->_OnHeadquartersJoinAgree( dwRoomIndex );
}

void ServerNode::OnLogoutRoomAlarm( SP2Packet &rkPacket )
{
	int iRoomIndex;
	ioHashString szMasterName;
	rkPacket >> iRoomIndex >> szMasterName;
	Room *pRoom = g_RoomNodeManager.GetHeadquartersNode( iRoomIndex );
	if( pRoom )
	{
		pRoom->OnModeLogoutAlarm( szMasterName );
	}
}

void ServerNode::OnUDPRecvTimeOut( SP2Packet &rkPacket )
{
	ioHashString szSenderID, szTargetID;
	DWORD dwSenderIndex, dwSenderBattleRoom;
	rkPacket >> szSenderID >> dwSenderIndex >> szTargetID >> dwSenderBattleRoom;

	User *pUser = GetUserOriginalNode( szTargetID, rkPacket );
	if( pUser == NULL )
	{
		if( g_UserNodeManager.GetGlobalUserNode( szTargetID ) == NULL )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUDPRecvTimeOut User NULL(%s)", szTargetID.c_str() );
		}
		return;
	}

	if( pUser->IsRoomLoadingOrServerMoving() )
	{
		// 로딩중이므로 다시 연결 요청
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnUDPRecvTimeOut User Loading... : %s", szTargetID.c_str() );

		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwSenderIndex );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_UDP_RECV_TIMEOUT_RECHECK );
			kPacket << szTargetID;
			pUserParent->RelayPacket( kPacket );
		}
		else
		{

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnUDPRecvTimeOut Sender User NULL(%s)", szSenderID.c_str() );
		}
	}
	else
	{
		SP2Packet kPacket( STPK_UDP_RECV_TIMEOUT );
		kPacket << szSenderID;
		pUser->SendMessage( kPacket );

		// 전투방 카운트 증가
		BattleRoomParent *pBattleParent = g_BattleRoomManager.GetGlobalBattleRoomNode( dwSenderBattleRoom );
		if( pBattleParent )
		{			
			pBattleParent->UserP2PRelayInfo( dwSenderIndex, pUser->GetUserIndex(), true );
		}
	}
}

void ServerNode::OnServerAlarmMent( SP2Packet &rkPacket )
{
	SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	g_UserNodeManager.SendUDPMessageAll( kPacket );
}

bool ServerNode::OnWebUDPParse( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case SSTPK_WEB_EVENT:
	case SSTPK_WEB_REFRESH_BLOCK:
	case SSTPK_WEB_GET_CASH:
	case SSTPK_WEB_REFRESH_USER_ENTRY:
		break; // 아래로
	default:
		return false;
	}
	
	DWORD dwUserIndex = 0;
	rkPacket >> dwUserIndex;
	User *pUser =  GetUserOriginalNode( dwUserIndex, rkPacket );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnWebUDPParse - Not User :%d", dwUserIndex );
		return true;
	}

	switch( rkPacket.GetPacketID() )
	{
	case SSTPK_WEB_EVENT:
		pUser->OnWebEvent( rkPacket );
		return true;
	case SSTPK_WEB_REFRESH_BLOCK:
		pUser->OnWebRefreshBlock( rkPacket );
		return true;
	case SSTPK_WEB_GET_CASH:
		pUser->OnWebGetCash( rkPacket );
		return true;
	case SSTPK_WEB_REFRESH_USER_ENTRY:
		pUser->OnWebRefreshUserEntry( rkPacket );
		return true;
	}

	return false;
}

bool ServerNode::OnBillingParse( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case SSTPK_GET_CASH_RESULT:
	case SSTPK_OUTPUT_CASH_RESULT:
	case SSTPK_BILLING_LOGIN_RESULT:
	case SSTPK_BILLING_REFUND_CASH_RESULT:
	case SSTPK_BILLING_AUTOUPGRADE_LOGIN_RESULT:
	case SSTPK_BILLING_PCROOM_RESULT:
	case SSTPK_BILLING_OTP_RESULT:
	case SSTPK_BILLING_GET_MILEAGE_RESULT:
	case SSTPK_BILLING_ADD_MILEAGE_RESULT:
	case SSTPK_BILLING_IPBONUS_RESULT:
	case SSTPK_BILLING_ADD_CASH_RESULT:
	case SSTPK_BILLING_FILL_CASH_URL_RESULT:
	case SSTPK_BILLING_SUBSCRIPTION_RETRACT_CHECK_RESULT:
	case SSTPK_BILLING_SUBSCRIPTION_RETRACT_RESULT:
	case SSTPK_SESSION_CONTROL_RESULT:
	case SSTPK_TIMEOUT_BILLINGGUID:
	case SSTPK_REQUEST_TIME_CASH_RESULT:
		break; // 아래로
	default:
		return false;
	}

	DWORD dwUserIndex = 0;
	ioHashString szPrivateID;
	User *pUser       = NULL;
	if( rkPacket.GetPacketID() == SSTPK_BILLING_LOGIN_RESULT     || 
		rkPacket.GetPacketID() == SSTPK_BILLING_USER_INFO_RESULT ||
		rkPacket.GetPacketID() == SSTPK_BILLING_AUTOUPGRADE_LOGIN_RESULT || 
		rkPacket.GetPacketID() == SSTPK_BILLING_OTP_RESULT || 
		rkPacket.GetPacketID() == SSTPK_BILLING_IPBONUS_RESULT )
	{
		rkPacket >> szPrivateID;
		pUser =  GetUserOriginalNodeByPrivateID( szPrivateID, rkPacket );
	}
	else
	{
		rkPacket >> dwUserIndex;
		pUser =  GetUserOriginalNode( dwUserIndex, rkPacket );
	}

	if( !pUser )
	{
		ioHashString szBillingGUID;
		rkPacket >> szBillingGUID;
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBillingParse - Not User :%d:%s:0x%x", dwUserIndex, szBillingGUID.c_str(), rkPacket.GetPacketID() );
		return true;
	}

	switch( rkPacket.GetPacketID() )
	{
	case SSTPK_GET_CASH_RESULT:
		pUser->OnBillingGetCash( rkPacket );
		return true;
	case SSTPK_OUTPUT_CASH_RESULT:
		pUser->OnBillingOutputCash( rkPacket );
		return true;
	case SSTPK_BILLING_LOGIN_RESULT:
		pUser->OnBillingLogin( rkPacket );
		return true;
	case SSTPK_BILLING_REFUND_CASH_RESULT:
		pUser->OnBillingRefundCash( rkPacket );
		return true;
	case SSTPK_BILLING_USER_INFO_RESULT:
		pUser->OnBillingUserInfo( rkPacket );
		return true;
	case SSTPK_BILLING_AUTOUPGRADE_LOGIN_RESULT:
		pUser->OnBillingAutoUpgradeLogin( rkPacket );
		return true;
	case SSTPK_BILLING_PCROOM_RESULT:
		pUser->OnBillingPCRoom( rkPacket );
		return true;
	case SSTPK_BILLING_OTP_RESULT:
		pUser->OnBillingAutoUpgradeOTP( rkPacket );
		return true;
	case SSTPK_BILLING_GET_MILEAGE_RESULT:
		pUser->OnBillingGetMileage( rkPacket );
		return true;
	case SSTPK_BILLING_ADD_MILEAGE_RESULT:
		pUser->OnBillingAddMileage( rkPacket );
		return true;
	case SSTPK_BILLING_IPBONUS_RESULT:
		pUser->OnBillingIPBonus( rkPacket );
		return true;
	case SSTPK_BILLING_ADD_CASH_RESULT:
		pUser->OnBillingAddCash( rkPacket );
		return true;
	case SSTPK_BILLING_FILL_CASH_URL_RESULT:
		pUser->OnBillingFillCashUrl( rkPacket );
		return true;
	case SSTPK_BILLING_SUBSCRIPTION_RETRACT_CHECK_RESULT:
		pUser->OnBillingSubscriptionRetractCheck( rkPacket );
		return true;
	case SSTPK_BILLING_SUBSCRIPTION_RETRACT_RESULT:
		pUser->OnBillingSubscriptionRetract( rkPacket );
		return true;
	case SSTPK_SESSION_CONTROL_RESULT:
		pUser->OnSessionControl(rkPacket);
		return true;
	case SSTPK_TIMEOUT_BILLINGGUID:
		pUser->OnBillingTimeoutBillingGUID( rkPacket );
		return true;
	case SSTPK_REQUEST_TIME_CASH_RESULT:
		pUser->OnBillingTimeCashResult(rkPacket);
		return true;
	}

	return false;
}

void ServerNode::OnWholeChat( SP2Packet &rkPacket )
{
	ioHashString szID;
	ioHashString szChat;
	rkPacket >> szID;
	rkPacket >> szChat;

	SP2Packet kPacket( STPK_WHOLE_CHAT );
	kPacket << szID;
	kPacket << szChat;
	g_UserNodeManager.SendMessageAll( kPacket );	
}

void ServerNode::OnTradeCreate( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnTradeCreate 유저 NULL : %d", dwUserIndex );
		return;
	}

	int iExtraValue;
	rkPacket >> iExtraValue;

	ioUserExtraItem *pExtraItem = pUser->GetUserExtraItem();
	if( pExtraItem )
	{
		pExtraItem->DeleteExtraItem( iExtraValue );

		SP2Packet kPacket( STPK_TRADE_CREATE );
		kPacket << TRADE_CREATE_DEL;
		kPacket << iExtraValue;
		pUser->SendMessage( kPacket );
	}
}

void ServerNode::OnTradeCreateComplete( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;

//	rkPacket >> dwUserIndex;
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnTradeCreateComplete 유저 NULL : %d", dwUserIndex );
		return;
	}

	DWORD dwTradeIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;
	DWORD dwRegisterPeriod, dwDate1, dwDate2;

// 	rkPacket >> dwItemType >> dwItemMagicCode >> dwItemValue >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemPrice
// 			 >> dwRegisterPeriod >> dwDate1 >> dwDate2;

	PACKET_GUARD_VOID( rkPacket.Read( dwTradeIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwItemType ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwItemMagicCode ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwItemValue ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwItemMaleCustom ) );

	PACKET_GUARD_VOID( rkPacket.Read( dwItemFemaleCustom ) );
	PACKET_GUARD_VOID( rkPacket.Read( iItemPrice ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwDate1 ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwDate2 ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwRegisterPeriod ) );

	SP2Packet kPacket( STPK_TRADE_CREATE );
// 	kPacket << TRADE_CREATE_OK;
// 	kPacket << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom << iItemPrice;

	PACKET_GUARD_VOID( kPacket.Write( TRADE_CREATE_OK ) );

	PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
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

	pUser->SendMessage( kPacket );
}

void ServerNode::OnTradeCreateFail( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnTradeCreateFail 유저 NULL : %d", dwUserIndex );
		return;
	}

	SP2Packet kPacket( STPK_TRADE_CREATE );
	kPacket << TRADE_CREATE_FAIL;
	pUser->SendMessage( kPacket );
}

void ServerNode::OnTradeItemComplete( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	int iType;
	rkPacket >> dwUserIndex >> iType;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnTradeItemComplete 유저 NULL : %d", dwUserIndex );
		return;
	}

	switch( iType )
	{
	case TRADE_S_GET_INFO_OK:
		OnTradeGetInfoOK( pUser, rkPacket );
		break;
	case TRADE_S_GET_INFO_FAIL:
		OnTradeGetInfoFail( pUser, rkPacket );
		break;
	case TRADE_S_BUY_COMPLETE:
		OnTradeBuyComplete( pUser, rkPacket );
		break;
	case TRADE_S_SELL_COMPLETE:
		OnTradeSellComplete( pUser, rkPacket );
		break;
	}
}

void ServerNode::OnTradeGetInfoOK( User *pUser, SP2Packet &rkPacket )
{
	DWORD dwTradeIndex;
	DWORD dwRegisterUserIndex, dwBuyUserIndex;
	DWORD dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;
	ioHashString szRegisterUserName;

	rkPacket >> dwBuyUserIndex >> dwTradeIndex;
	rkPacket >> dwRegisterUserIndex >> szRegisterUserName;
	rkPacket >> dwItemType >> dwItemMagicCode >> dwItemValue;
	rkPacket >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemPrice;

	// 페소차감
	__int64 iCurPeso = pUser->GetMoney();
	__int64 iResultPeso = iItemPrice + (iItemPrice * g_TradeInfoMgr.GetBuyTexRate());		// 차감할 페소

	if( iCurPeso < iResultPeso )
	{
		// 메인서버에 실패전송
		SP2Packet kPacket( MSTPK_TRADE_ITEM_TRADE );
		PACKET_GUARD_VOID( kPacket.Write( TRADE_ITEM_TRADE_FAIL ) );
		PACKET_GUARD_VOID( kPacket.Write( dwTradeIndex ) );

		g_MainServer.SendMessage( kPacket );

		// 클라이언트에 실패전송
		SP2Packet kSuccess( STPK_TRADE_ITEM_COMPLETE );
		kSuccess << TRADE_BUY_PESO;
		pUser->SendMessage( kSuccess );
		return;
	}
	else
	{
		// 페소 차감
		pUser->RemoveMoney( iResultPeso );
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_CONSUME, pUser, 0, 0, LogDBClient::LET_ITEM, LogDBClient::PT_TRADE, PRESENT_EXTRAITEM, dwItemMagicCode, iResultPeso, NULL);
	}

	// DB에 삭제요청
	g_DBClient.OnTradeItemComplete( pUser->GetUserDBAgentID(),
									pUser->GetAgentThreadID(),
									dwBuyUserIndex,
									dwRegisterUserIndex,
									szRegisterUserName,
									dwTradeIndex,
									dwItemType,
									dwItemMagicCode,
									dwItemValue,
									dwItemMaleCustom,
									dwItemFemaleCustom,
									iItemPrice );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "[%d-1 : %d,%d]", dwTradeIndex, dwItemMaleCustom, dwItemFemaleCustom );

	g_LogDBClient.OnInsertTrade( dwBuyUserIndex, pUser->GetPublicID(),
								 dwTradeIndex,
								 dwItemType, dwItemMagicCode, dwItemValue, iItemPrice, LogDBClient::TST_BUY,
								 pUser->GetPublicIP(), szNote );
}

void ServerNode::OnTradeGetInfoFail( User *pUser, SP2Packet &rkPacket )
{
	int iResult;
	rkPacket >> iResult;

	int iFailType = TRADE_BUY_ERROR;
	switch( iResult )
	{
	case TRADE_ITEM_NO_ITEM:
		iFailType = TRADE_BUY_NO_ITEM;
		break;
	case TRADE_ITEM_PESO:
		iFailType = TRADE_BUY_PESO;
		break;
	case TRADE_ITEM_OWNER:
		iFailType = TRADE_BUY_OWNER;
		break;
	}

	SP2Packet kSuccess( STPK_TRADE_ITEM_COMPLETE );
	kSuccess << iFailType;
	pUser->SendMessage( kSuccess );
}

void ServerNode::OnTradeBuyComplete( User *pUser, SP2Packet &rkPacket )
{
	DWORD dwTradeIndex;
	DWORD dwRegisterUserIndex, dwBuyUserIndex;
	DWORD dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;
	ioHashString szRegisterUserName;

	rkPacket >> dwBuyUserIndex >> dwTradeIndex;
	rkPacket >> dwRegisterUserIndex >> szRegisterUserName;
	rkPacket >> dwItemType >> dwItemMagicCode >> dwItemValue;
	rkPacket >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemPrice;

	g_PresentHelper.SendPresentByTradeItemBuy( pUser->GetUserDBAgentID(), 
											   pUser->GetAgentThreadID(),
											   dwRegisterUserIndex,
											   dwBuyUserIndex,
											   dwItemType,
											   dwItemMagicCode,
											   dwItemValue,
											   dwItemMaleCustom,
											   dwItemFemaleCustom,
											   szRegisterUserName );

	// DB에서 선물 정보 가져오게하고, 정보 전달.
	pUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

	// 메인서버에 삭제 요청
	SP2Packet kPacket( MSTPK_TRADE_ITEM_TRADE );
	PACKET_GUARD_VOID( kPacket.Write( TRADE_ITEM_DEL ) );
	PACKET_GUARD_VOID( kPacket.Write( dwTradeIndex ) );

	g_MainServer.SendMessage( kPacket );

	// 지불한 페소
	__int64 iResultPeso = iItemPrice;
	SP2Packet kSuccess( STPK_TRADE_ITEM_COMPLETE );

// 	kSuccess << TRADE_BUY_OK;
// 	kSuccess << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
// 	kSuccess << iResultPeso;

	PACKET_GUARD_VOID( kSuccess.Write( TRADE_BUY_OK ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwTradeIndex ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemType ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemMagicCode ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemValue ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemMaleCustom ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemFemaleCustom ) );
	PACKET_GUARD_VOID( kSuccess.Write( iResultPeso ) );

	pUser->SendMessage( kSuccess );
}

void ServerNode::OnTradeSellComplete( User *pUser, SP2Packet &rkPacket )
{
	DWORD dwTradeIndex;
	DWORD dwRegisterUserIndex, dwBuyUserIndex;
	DWORD dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;
	ioHashString szRegisterUserName;

	rkPacket >> dwBuyUserIndex >> dwTradeIndex;
	rkPacket >> dwRegisterUserIndex >> szRegisterUserName;
	rkPacket >> dwItemType >> dwItemMagicCode >> dwItemValue >> dwItemMaleCustom >> dwItemFemaleCustom;
	rkPacket >> iItemPrice;

	// 실제지급할 페소 계산
	int iTradePrice = iItemPrice;
	g_PresentHelper.SendPresentByTradeItemSell( pUser->GetUserDBAgentID(),
												pUser->GetAgentThreadID(),
												dwBuyUserIndex,
												dwRegisterUserIndex,
												iTradePrice,
												szRegisterUserName );

	// DB에서 선물 정보 가져오게하고, 클라이언트에 정보 전달.
	pUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

	SP2Packet kSuccess( STPK_TRADE_ITEM_COMPLETE );

// 	kSuccess << TRADE_SELL_OK;
// 	kSuccess << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
// 	kSuccess << iTradePrice;

	PACKET_GUARD_VOID( kSuccess.Write( TRADE_SELL_OK ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwTradeIndex ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemType ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemMagicCode ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemValue ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemMaleCustom ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemFemaleCustom ) );
	PACKET_GUARD_VOID( kSuccess.Write( iTradePrice ) );

	pUser->SendMessage( kSuccess );
}

void ServerNode::OnTradeCancel( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	int iType;
	rkPacket >> dwUserIndex >> iType;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnTradeCancel 유저 NULL : %d", dwUserIndex );
		return;
	}

	switch( iType )
	{
	case TRADE_CANCEL_S_GET_INFO_OK:
		OnTradeCancelInfoOK( pUser, rkPacket );
		break;
	case TRADE_CANCEL_S_GET_INFO_FAIL:
		OnTradeCancelInfoFail( pUser, rkPacket );
		break;
	case TRADE_CANCEL_S_CANCEL_COMPLETE:
		OnTradeCancelComplete( pUser, rkPacket );
		break;
	}
}

void ServerNode::OnTradeCancelInfoOK( User *pUser, SP2Packet &rkPacket )
{
	DWORD dwTradeIndex;
	DWORD dwRegisterUserIndex;
	DWORD dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;
	ioHashString szRegisterUserName;

	rkPacket >> dwTradeIndex;
	rkPacket >> dwRegisterUserIndex >> szRegisterUserName;
	rkPacket >> dwItemType >> dwItemMagicCode >> dwItemValue >> dwItemMaleCustom >> dwItemFemaleCustom;
	rkPacket >> iItemPrice;

	TradeLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "TradeCancel - [TradeDelete] [RequestUser:%d] [RegisterUser:%d] [TradeIndex:%d] [Type:%d] [Code:%d] [Value:%d] [Custom:%d.%d]",
								  pUser->GetUserIndex(), dwRegisterUserIndex, dwTradeIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom );

	// DB에 삭제요청
	g_DBClient.OnTradeItemCancel( pUser->GetUserDBAgentID(),
								  pUser->GetAgentThreadID(),
								  dwRegisterUserIndex,
								  szRegisterUserName,
								  dwTradeIndex,
								  dwItemType,
								  dwItemMagicCode,
								  dwItemValue,
								  dwItemMaleCustom,
								  dwItemFemaleCustom,
								  iItemPrice );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "[%d-1 : %d,%d]", dwTradeIndex, dwItemMaleCustom, dwItemFemaleCustom );

	g_LogDBClient.OnInsertTrade( dwRegisterUserIndex, pUser->GetPublicID(),
								 dwTradeIndex,
								 dwItemType, dwItemMagicCode, dwItemValue, iItemPrice, LogDBClient::TST_CANCEL,
								 pUser->GetPublicIP(), szNote );
}

void ServerNode::OnTradeCancelInfoFail( User *pUser, SP2Packet &rkPacket )
{
	int iResult;
	rkPacket >> iResult;

	int iFailType = TRADE_CANCEL_ERROR;
	switch( iResult )
	{
	case TRADE_ITEM_CANCEL_NO_ITEM:
		iFailType = TRADE_CANCEL_NO_ITEM;
		break;
	case TRADE_ITEM_CANCEL_NOT_OWNER:
		iFailType = TRADE_CANCEL_NOT_OWNER;
		break;
	}

	SP2Packet kSuccess( STPK_TRADE_CANCEL );
	kSuccess << iFailType;
	pUser->SendMessage( kSuccess );
}

void ServerNode::OnTradeCancelComplete( User *pUser, SP2Packet &rkPacket )
{
	DWORD dwTradeIndex;
	DWORD dwRegisterUserIndex;
	DWORD dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;
	ioHashString szRegisterUserName;

	rkPacket >> dwTradeIndex;
	rkPacket >> dwRegisterUserIndex >> szRegisterUserName;
	rkPacket >> dwItemType >> dwItemMagicCode >> dwItemValue >> dwItemMaleCustom >> dwItemFemaleCustom;
	rkPacket >> iItemPrice;

	g_PresentHelper.SendPresentByTradeCancel( pUser->GetUserDBAgentID(),
											  pUser->GetAgentThreadID(),
											  dwRegisterUserIndex,
											  dwRegisterUserIndex,
											  dwItemType,
											  dwItemMagicCode,
											  dwItemValue,
											  dwItemMaleCustom,
											  dwItemFemaleCustom,
											  szRegisterUserName );

	// DB에서 선물 정보 가져오게하고, 클라이언트에 정보 전달.
	pUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

	// 메인서버에 삭제 요청
	SP2Packet kPacket( MSTPK_TRADE_ITEM_CANCEL );
	kPacket << TRADE_ITEM_CANCEL_DEL;
	kPacket << dwTradeIndex;
	g_MainServer.SendMessage( kPacket );

	SP2Packet kSuccess( STPK_TRADE_CANCEL );

// 	kSuccess << TRADE_CANCEL_COMPLETE;
// 	kSuccess << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
// 	kSuccess << iItemPrice;

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

void ServerNode::OnTradeTimeOut( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;

	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnTradeTimeOut 유저 NULL : %d", dwUserIndex );
		return;
	}

	DWORD dwTradeIndex;
	DWORD dwRegisterUserIndex;
	DWORD dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	ioHashString szRegisterUserName;
	__int64 iItemPrice;

	rkPacket >> dwTradeIndex;
	rkPacket >> dwRegisterUserIndex >> szRegisterUserName;
	rkPacket >> dwItemType >> dwItemMagicCode >> dwItemValue >> dwItemMaleCustom >> dwItemFemaleCustom;
	rkPacket >> iItemPrice;

	g_PresentHelper.SendPresentByTradeTimeOut( pUser->GetUserDBAgentID(),
											   pUser->GetAgentThreadID(),
											   dwRegisterUserIndex,
											   dwRegisterUserIndex,
											   dwItemType,
											   dwItemMagicCode,
											   dwItemValue,
											   dwItemMaleCustom,
											   dwItemFemaleCustom,
											   szRegisterUserName );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "[%d-1 : %d,%d]", dwTradeIndex, dwItemMaleCustom, dwItemFemaleCustom );

	g_LogDBClient.OnInsertTrade( dwRegisterUserIndex, pUser->GetPublicID(),
								 dwTradeIndex,
								 dwItemType, dwItemMagicCode, dwItemValue, iItemPrice, LogDBClient::TST_TIMEOUT,
								 pUser->GetPublicIP(), szNote );


	// DB에서 선물 정보 가져오게하고, 클라이언트에 정보 전달.
	pUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

	SP2Packet kSuccess( STPK_TRADE_TIME_OUT );

// 	kSuccess << dwItemType << dwItemMagicCode << dwItemValue << dwItemMaleCustom << dwItemFemaleCustom;
// 	kSuccess << iItemPrice;

	PACKET_GUARD_VOID( kSuccess.Write( dwTradeIndex ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemType ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemMagicCode ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemValue ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemMaleCustom ) );
	PACKET_GUARD_VOID( kSuccess.Write( dwItemFemaleCustom ) );
	PACKET_GUARD_VOID( kSuccess.Write( iItemPrice ) );

	pUser->SendMessage( kSuccess );
}

void ServerNode::OnDisconnectAlreadyID( SP2Packet &rkPacket )
{
	DWORD        dwUserIndex = 0;
	ioHashString szRequestGUID;
	rkPacket >> dwUserIndex;
	rkPacket >> szRequestGUID;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL : %d", __FUNCTION__, dwUserIndex );
		return;
	}

	pUser->CloseConnection();
//	pUser->ExceptionClose(0);
	WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DisconnectAlreadyID[3]-%s-%s", pUser->GetPrivateID().c_str(), szRequestGUID.c_str() );
}

// 2020-06-05 by bckim, 하드시리얼 유저 제재
void ServerNode::OnDisconnectHddSerialBlock( SP2Packet &rkPacket )
{
	DWORD   dwUserIndex = 0;
	int		iResult = 0;

	rkPacket >> dwUserIndex;
	rkPacket >> iResult;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL : %d", __FUNCTION__, dwUserIndex );
		return;
	}

	if ( iResult == 1001 )
	{
		//	pUser->CloseConnection();		클라이언트에서 종료 시키기로 함
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnDisconnectHddSerialBlock[close_2]-%s", pUser->GetPrivateID().c_str());
	}	
}
// End. 2020-06-05 by bckim, 하드시리얼 유저 제재


void ServerNode::OnEventItemInitialize( SP2Packet &rkPacket )
{
	int iType = 0;
	rkPacket >> iType;
	g_UserNodeManager.InitUserEventItem( iType );
}

void ServerNode::OnLoginConnect(SP2Packet &rkPacket)
{
	SetNodeType( NODE_TYPE_LOGIN );
}

void ServerNode::OnLoginRequestStatus() 
{
	int iServerIndex = g_ServerNodeManager.GetServerIndex();
	int iBlockState	= g_ServerNodeManager.IsBlocked() ? 1 : 0;
	int iUserCount	= g_UserNodeManager.GetNodeSize();

	SP2Packet kPacket(LSTPK_STATUS_RESPONSE); //여기서 public ip/userport로 변경 
	kPacket << iServerIndex;
	kPacket << iBlockState;
	kPacket << iUserCount;
	kPacket << m_szPublicIP;
	kPacket << g_App.GetCSPort();

	SendMessage(kPacket);
}

void ServerNode::OnLoginBlockRequest(SP2Packet &rkPacket)
{
	// 로그인서버의 요청으로 해당서버를 블록한다
	int iServerIndex = 0, iBlockState = 0;
	rkPacket >> iServerIndex;
	rkPacket >> iBlockState;

	bool bBlockFlag = (iBlockState == 1) ? true : false;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLoginBlockRequest : %d, %d", iServerIndex, iBlockState );
	if( 0 == iServerIndex ) // 모든서버 블럭
	{
		// 현재서버 블럭
		g_ServerNodeManager.SetBlockState( bBlockFlag );

		// 다른서버 노드의 블럭
		static vServerIndex vServerIndexes;
		vServerIndexes.clear();

		g_ServerNodeManager.GetServerNodes(true, vServerIndexes);

		for(vServerIndex::iterator it = vServerIndexes.begin() ; it != vServerIndexes.end() ; ++it)
		{
			ServerNode *pServerNode = g_ServerNodeManager.GetServerNode( *it );
			if( !pServerNode ) continue;

			pServerNode->SetBlockState( bBlockFlag );
		}
	}
	else
	{
		if(g_ServerNodeManager.GetServerIndex() == iServerIndex)
		{
			g_ServerNodeManager.SetBlockState( bBlockFlag );

			// 모든 유저에게 블록상태를 알림
			g_UserNodeManager.AllUserLeaveServer(bBlockFlag);
		}
		else
		{
			ServerNode *pServerNode = g_ServerNodeManager.GetServerNode( iServerIndex );
			if( pServerNode )
			{
				pServerNode->SetBlockState( bBlockFlag );
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnLoginBlockRequest not exist : %d, %d", iServerIndex, iBlockState );
			}
		}
	}
}

void ServerNode::OnLoginPartitionRequest(SP2Packet &rkPacket)
{
	int iPartitionCount = 0;
	rkPacket >> iPartitionCount;

	// 최소 1개의 파티션이 존재해야 한다
	if(iPartitionCount > 1 )
	{
		// 파티션 재구성 작업
		g_ServerNodeManager.RearrangePartition( iPartitionCount );
	}
}

void ServerNode::OnTournamentCreateTeam( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		SHORT Position;
		ioHashString kTeamName;
		int iCheerPoint, iLadderPoint;
		BYTE MaxPlayer, CampPos, TourPos;
		DWORD dwTourIndex, dwTeamIndex, dwOwnerIndex;
		rkPacket >> dwTourIndex >> dwTeamIndex >> kTeamName >> dwOwnerIndex;
		rkPacket >> Position >> MaxPlayer >> iCheerPoint >> TourPos >> iLadderPoint >> CampPos;

		ioUserTournament *pTournament = pUser->GetUserTournament();
		if( pTournament )
		{
			pTournament->CreateTeamData( dwTourIndex, dwTeamIndex, dwOwnerIndex, kTeamName, Position, TourPos );

			//
			SP2Packet kPacket( STPK_TOURNAMENT_TEAM_CREATE );
			kPacket << MS_TOURNAMENT_TEAM_CREATE_OK << dwTourIndex << dwTeamIndex << kTeamName << dwOwnerIndex 
					<< Position << MaxPlayer << iCheerPoint << TourPos << iLadderPoint << CampPos;
			pUser->SendMessage( kPacket );
		}
	}
}

void ServerNode::OnTournamentTeamAgreeOK( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		ioUserTournament *pUserTournament = pUser->GetUserTournament();
		if( pUserTournament )
		{
			DWORD dwTeamIndex;
			ioUserTournament::TeamUserData kUserData;
			rkPacket >> dwTeamIndex >> kUserData.m_dwTableIndex >> kUserData.m_dwUserIndex >> kUserData.m_szNick;
			rkPacket >> kUserData.m_iGradeLevel >> kUserData.m_iLadderPoint >> kUserData.m_dwGuildIndex; 
			pUserTournament->AddTeamUserData( dwTeamIndex, kUserData );
		}
	}
}

void ServerNode::OnTournamentTeamJoin( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		ioUserTournament *pUserTournament = pUser->GetUserTournament();
		if( pUserTournament )
		{
			ioHashString szTeamName;
			BYTE LeaguePos, TourPos;
			DWORD dwTourIndex, dwTeamIndex, dwOwnerIndex;
			rkPacket >> dwTourIndex >> dwTeamIndex >> szTeamName >> dwOwnerIndex >> LeaguePos >> TourPos;				
			if( pUserTournament->JoinTeamData( dwTourIndex, dwTeamIndex, dwOwnerIndex, szTeamName, LeaguePos, TourPos ) )
			{
				SP2Packet kPacket( STPK_TOURNAMENT_TEAM_JOIN );
				kPacket << dwTourIndex << dwTeamIndex << szTeamName << dwOwnerIndex << LeaguePos << TourPos;					
				pUser->SendMessage( kPacket );
			}
		}
	}
}

void ServerNode::OnTournamentTeamLeave( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		ioUserTournament *pUserTournament = pUser->GetUserTournament();
		if( pUserTournament )
		{
			DWORD dwSenderIndex, dwTeamIndex, dwLeaveIndex;
			rkPacket >> dwSenderIndex >> dwTeamIndex >> dwLeaveIndex;

			pUserTournament->LeaveTeamUser( dwSenderIndex, dwTeamIndex, dwLeaveIndex );
		}
	}
}

void ServerNode::OnPresentInsert( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		ioHashString kSendID, kLogMent;
		SHORT        iPresentType, iPresentMent, iPresentState;
		int          iPresentValue1, iPresentValue2, iPresentDay;
		rkPacket >> kSendID >> iPresentType >> iPresentValue1 >> iPresentValue2 >> iPresentMent >> iPresentDay >> iPresentState >> kLogMent;

		CTimeSpan cPresentGapTime( iPresentDay, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		pUser->AddPresentMemory( kSendID, iPresentType, iPresentValue1, iPresentValue2, 0, 0, iPresentMent, kPresentTime, iPresentState );
		g_LogDBClient.OnInsertPresent( 0, kSendID, g_App.GetPublicIP().c_str(), dwUserIndex, iPresentType, iPresentValue1, iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, kLogMent.c_str() );
		pUser->SendPresentMemory();
	}
}

void ServerNode::OnPresentInsertByEtcItem( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser )
	{
		DWORD dwSendUserIndex;
		ioHashString kSendUserID, kSendUserIP, kLogMent;
		int iPresentType, iPresentMent, iPresentState, iPresentPeriod;
		int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;

		rkPacket >> dwSendUserIndex;
		rkPacket >> kSendUserID;
		rkPacket >> kSendUserIP;
		rkPacket >> iPresentPeriod;
		rkPacket >> iPresentType;
		rkPacket >> iPresentValue1;
		rkPacket >> iPresentValue2;
		rkPacket >> iPresentValue3;
		rkPacket >> iPresentValue4;
		rkPacket >> iPresentMent;
		rkPacket >> iPresentState;
		rkPacket >> kLogMent;

		CTimeSpan cPresentGapTime( iPresentPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

		pUser->AddPresentMemory( kSendUserID, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, iPresentState );

		g_LogDBClient.OnInsertPresent( dwSendUserIndex, kSendUserID, kSendUserIP.c_str(), pUser->GetUserIndex(), iPresentType,
									   iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4,
									   LogDBClient::PST_RECIEVE, kLogMent.c_str() );
		pUser->SendPresentMemory();
	}
}

//! 친구가 다른서버에서 클로버 받음.
void ServerNode::OnCloverSend( SP2Packet& rkPacket )
{
	// Declare) A -> B. 
	DWORD dwSendUserIndex;
	DWORD dwFriendIndex;
	int iReceiveDate;

	// UserIndex : A / B
	rkPacket >> dwSendUserIndex >> dwFriendIndex >> iReceiveDate;

	// Get User : B
	User* pFriendUser = GetUserOriginalNode( dwFriendIndex, rkPacket );
	if( pFriendUser )
	{
		// Friend
		ioFriend* pTargetFriend = pFriendUser->GetFriend();
		pTargetFriend->SetFriendBeforeReceiveCloverData( dwSendUserIndex, Help::GetCloverSendCount(), iReceiveDate );

		// 여기서 처리.
		pFriendUser->CloverFriendInfo( dwSendUserIndex, ioClover::FRIEND_CLOVER_COME_TO_FRIEND );
	}
}

void ServerNode::OnRelayServerConnect(SP2Packet& kPacket)
{
	SetNodeType( NODE_TYPE_RELAY );
	int nSize = 0;
	kPacket >> m_szPublicIP;
	kPacket >> m_iServerPort; // 기존 구 서버 호환용 
	kPacket >> nSize;
	if(nSize == 0)
	{
		LOG.PrintTimeAndLog(0,"OnRelayServerConnect Error nsize Is NULL (%s)",m_szPublicIP);
	//	return;
		m_relayPorts.push_back(m_iServerPort); // 기존 코드와 호환성을 위해 
	}
	
	for(int i=0; i< nSize; ++i)
	{
		int nPortTemp = 0;
		kPacket >> nPortTemp;
		m_relayPorts.push_back(nPortTemp);
		
	}
	
	 
	g_Relay.AddRelayServerInfo(this);
	m_iRelayIndex = g_Relay.GetRelayServerIndex();

	SP2Packet pk(RSPTK_ON_CONNECT);

	pk << Help::IsWholeChatOn();
	pk << m_iRelayIndex;
	pk << g_ServerNodeManager.GetServerIndex();

	CConnectNode::SendMessage(pk,TRUE);

	g_Relay.SetUseRelayServer(TRUE);

	LOG.PrintTimeAndLog(0,"OnRelayServerConnect(%s:%d)",m_szPublicIP,m_iServerPort);
}

void ServerNode::OnRelayControl( SP2Packet & kPacket )
{
	if( !IsRelayNode() ) return;

	int ctype;
	kPacket >> ctype;
	switch(ctype)
	{
	case RS_INFO:
		{
			kPacket >> m_RelayInfo;
		}
		break;
	case RS_ON_ADD_USER:
		{
			RequestRelayServerConnect(kPacket);
		}
		break;
	case RS_CHANGE_ADDR:
		{
			OnChangeAddress(kPacket);
		}
		break;
	case RS_HACK_ANNOUNCE:
		{
			OnHackAnnounce(kPacket);
		}
		break;
	case RS_USER_GHOST:
		{
			OnUserGhost(kPacket);
		}
		break;
	case RS_RESERVER_ROOM_JOIN:
		{
			DWORD userIndex;
			kPacket >> userIndex;
			User* node = g_UserNodeManager.GetUserNode(userIndex);
			if(node)
				node->OnReserveRoomJoin(kPacket);
			else
				LOG.PrintTimeAndLog(0,"Error RS_RESERVER_ROOM_JOIN! UserIdnex NotFound(%d)",userIndex);

		}
		break;
	}
}

void ServerNode::RequestRelayServerConnect( SP2Packet & kPacket )
{
	char publicID[ID_NUM_PLUS_ONE];
	kPacket >> publicID;
//	LOG.PrintTimeAndLog(0,"Relay::%s:%d[%s]\n",m_szPublicIP,m_iServerPort,publicID);
//	Information("Relay::%s:%d[%s]\n",m_szPublicIP,m_iServerPort,publicID);
	RequestRelayServerConnect(publicID);
}

void ServerNode::RequestRelayServerConnect(const char* publicID)
{ //kyg 여기서 일단 한번 바꿈 
	if(m_iServerPort != 0 )
	{
		User* node = g_UserNodeManager.GetUserNodeByPublicID(publicID);
		if(node)
		{
			Room* pRoom  = node->GetMyRoom();
			int nPort  = 0 ;
			if(pRoom)
			{
				Debug("RQ:%d :: \n",node->GetMyRoom()->GetRoomIndex());
				nPort = GetRelayServerPort(pRoom->GetRoomIndex());
			}
			nPort = (nPort != 0) ? nPort : GetRelayServerPort(0);
			SP2Packet pk(STPK_ON_CONTROL);
			int ctype = RC_USE_RELAYSVR;
			pk << ctype;
			pk << m_szPublicIP;
			pk << nPort;
			Debug("RelayServer Port Send : %d\n",nPort);
			node->SetRelayServerID(m_iRelayIndex);
			node->SendMessage(pk);
		}
	}
}

void ServerNode::OnChangeAddress( SP2Packet & kPacket )
{
	char szPublicID[ID_NUM_PLUS_ONE];
	char szIpaddr[STR_IP_MAX];	
	int  iPort;
	DWORD dwUserIndex;
	kPacket >> szPublicID;
	kPacket >> szIpaddr;
	kPacket >> iPort;
	kPacket >> dwUserIndex;
	User* node = g_UserNodeManager.GetUserNode(dwUserIndex);
	if(node)
	{
		char back_iip[16] = "";
		int  back_port = node->GetUDP_port();
		strcpy_s( back_iip, node->GetPublicIP() );
		LOG.PrintTimeAndLog(0,"OnChangeAddress::%s:%d to %s:%d\n",back_iip,back_port,szIpaddr,iPort);
		node->SetClientAddressForRelay(szIpaddr,iPort);
		node->SetClientAddr(back_iip,back_port);
	}
	else
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"Error RS_CAHNGE_ADDR userID: [%s] NotFound Node!",szPublicID);
	}
}

void ServerNode::OnHackAnnounce( SP2Packet & kPacket )
{
	DWORD userIndex = 0;
	int ht_speed = 0;
	int maxAnswer = 0;
	DWORD clientAnswerTime =0;
	int iFirstOperand = 0;
	int iSecondOperand = 0;
	int iOperator = 0; 

	kPacket >> userIndex >> ht_speed >> maxAnswer >> clientAnswerTime >> 
		iFirstOperand >> iSecondOperand >> iOperator;
	Debug("HACK::userindex:%d FirstOperand:%d\n",userIndex,iFirstOperand);
	User* node = g_UserNodeManager.GetUserNode(userIndex);
	if(node)
	{
		SP2Packet sp(STPK_HACK_ANNOUNCE);
		sp << ht_speed;
		sp << maxAnswer;
		sp << clientAnswerTime;
		sp << iFirstOperand;
		sp << iSecondOperand;
		sp << iOperator;
		node->SendMessage(sp);
	}
}

void ServerNode::OnUserGhost( SP2Packet & kPacket )
{
	DWORD userIndex;
	int CheckTime;
	kPacket >> userIndex;
	kPacket >> CheckTime;

	User* node = g_UserNodeManager.GetUserNode(userIndex);
	if(node)
		node->TimeOutClose(CheckTime);
}

int ServerNode::GetRelayServerPort( int num )
{
	int nIndex = 0;
	if(num == 0)
		nIndex = rand() % m_relayPorts.size();
	else
		nIndex = num % m_relayPorts.size();

	return m_relayPorts[nIndex];
}

bool ServerNode::IsSameBilling( DWORD serverIndex )
{
	int myIndex = g_ServerNodeManager.GetServerIndex() % g_BillingRelayServer.GetBillingServerCount();
	int moveIndex = serverIndex % g_BillingRelayServer.GetBillingServerCount();

	if(myIndex == moveIndex) return true;
	else return false;
}

void ServerNode::OnShuffleRoomTransfer( SP2Packet &rkPacket )
{
	int iTransferType, iShuffleRoomIndex;
	rkPacket >> iTransferType >> iShuffleRoomIndex;

	ShuffleRoomNode *pShuffleRoom = g_ShuffleRoomManager.GetShuffleRoomNode( iShuffleRoomIndex );	
	switch( iTransferType )
	{
	case ShuffleRoomParent::ENTER_USER:
		{
			DWORD dwUserIndex;
			int   iGradeLevel, iAbilityLevel, iClientPort, iTransferPort;
			ioHashString szPublicID, szPublicIP, szPrivateIP, szTransferIP;
			rkPacket >> dwUserIndex >> szPublicID >> iGradeLevel >> iAbilityLevel
					 >> szPublicIP >> szPrivateIP >> szTransferIP >> iClientPort >> iTransferPort;

			if( pShuffleRoom == NULL )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - ENTER_USER 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );

				SP2Packet kPacket( SSTPK_SHUFFLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_SHUFFLEROOM_JOIN_NOT_NODE << iShuffleRoomIndex;
				SendMessage( kPacket );
			}
			else if( pShuffleRoom->IsFull() )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - ENTER_USER 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );

				SP2Packet kPacket( SSTPK_SHUFFLEROOM_JOIN_RESULT );
				kPacket << dwUserIndex << USER_SHUFFLEROOM_JOIN_FULL_USER << iShuffleRoomIndex << pShuffleRoom->GetPlayUserCnt()
						<< pShuffleRoom->GetMaxPlayerBlue() << pShuffleRoom->GetMaxPlayerRed();
				SendMessage( kPacket );						
			}
			else
			{
				pShuffleRoom->EnterUser( dwUserIndex, szPublicID, iGradeLevel, iAbilityLevel, szPublicIP, szPrivateIP, szTransferIP, iClientPort, iTransferPort );
			}
		}
		break;
	case ShuffleRoomParent::LEAVE_USER:
		{
			if( !pShuffleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - LEAVE_USER 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );
				return;
			}

			DWORD dwUserIndex;
			ioHashString szPublicID;
			rkPacket >> dwUserIndex >> szPublicID;
			pShuffleRoom->LeaveUser( dwUserIndex, szPublicID );
		}
		break;
	case ShuffleRoomParent::TRANSFER_PACKET:
		{
			if( !pShuffleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - TRANSFER_PACKET 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );
				return;
			}

			DWORD dwUserIndex, dwPacketID;
			rkPacket >> dwUserIndex >> dwPacketID;
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 2 );
			// 패킷 생성
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
			pShuffleRoom->SendPacketTcp( kPacket, dwUserIndex );
		}
		break;
	case ShuffleRoomParent::TRANSFER_PACKET_USER:
		{
			if( !pShuffleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - TRANSFER_PACKET_USER 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );
				return;
			}

			ioHashString szSenderName;
			DWORD dwPacketID;
			rkPacket >> szSenderName >> dwPacketID;
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 1 ) + lstrlen( szSenderName.c_str() ) + 1;
			// 패킷 생성
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
			pShuffleRoom->SendPacketTcpUser( kPacket, szSenderName );			
		}
		break;
	case ShuffleRoomParent::TRANSFER_PACKET_UDP:
		{
			if( !pShuffleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - TRANSFER_PACKET_UDP 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );
				return;
			}

			DWORD dwUserIndex, dwPacketID;
			rkPacket >> dwUserIndex >> dwPacketID;
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 2 );
			// 패킷 생성
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize );
			pShuffleRoom->SendPacketUdp( kPacket, dwUserIndex );
		}
		break;
	case ShuffleRoomParent::USER_INFO:
		{
			if( !pShuffleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - USER_INFO 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );
				return;
			}

			DWORD dwUserIndex;
			int   iGradeLevel, iAbilityLevel, iClientPort, iTransferPort;
			ioHashString szTransferIP;
			rkPacket >> dwUserIndex >> iGradeLevel >> iAbilityLevel >> iClientPort >> szTransferIP >> iTransferPort;
			pShuffleRoom->UserInfoUpdate( dwUserIndex, iGradeLevel, iAbilityLevel, iClientPort, szTransferIP, iTransferPort );
		}
		break;
	case ShuffleRoomParent::UDP_CHANGE:
		{
			if( !pShuffleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - UDP_CHANGE 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );
				return;
			}

			DWORD dwUserIndex;
			int   iClientPort, iTransferPort;
			ioHashString szPublicID, szPublicIP, szPrivateIP, szTransferIP;
			rkPacket >> dwUserIndex >> szPublicID >> szPublicIP >> iClientPort >> szPrivateIP >> szTransferIP >> iTransferPort;
			pShuffleRoom->UserUDPChange( dwUserIndex, szPublicID, szPublicIP, iClientPort, szPrivateIP, szTransferIP, iTransferPort );
		}
		break;
	case ShuffleRoomParent::P2P_RELAY_INFO:
		{
			if( !pShuffleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - P2P_RELAY_INFO 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );
				return;
			}
			DWORD dwUserIndex, dwRelayUserIndex;
			bool  bServerRelay;
			rkPacket >> dwUserIndex >> dwRelayUserIndex >> bServerRelay;
			pShuffleRoom->UserP2PRelayInfo( dwUserIndex, dwRelayUserIndex, bServerRelay );
		}
		break;
	case ShuffleRoomParent::USER_PACKET_TRANSFER:
		{
			if( !pShuffleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - USER_PACKET_TRANSFER 중계하려는 셔플룸 없음 %d", iShuffleRoomIndex );
				return;
			}

			DWORD dwUserIndex, dwPacketID;
			rkPacket >> dwUserIndex >> dwPacketID;	
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
			if( pUser == NULL )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - USER_PACKET_TRANSFER 유저 NULL : %d - 0x%x", dwUserIndex, dwPacketID );
				return;
			}
			// 패킷 생성
			int iMoveSize = ( sizeof(int) * 2 ) + ( sizeof(DWORD) * 2 );
			SP2Packet kPacket( dwPacketID );
			kPacket.SetDataAdd( (char*)rkPacket.GetData() + iMoveSize, rkPacket.GetDataSize() - iMoveSize, true );
			pShuffleRoom->OnProcessPacket( kPacket, pUser );
		}
		break;
	case ShuffleRoomParent::REJOIN_TRY:
		{
			DWORD dwUserIndex;
			ioHashString szPublicID;
			PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
			PACKET_GUARD_VOID( rkPacket.Read( szPublicID ) );

			if( !pShuffleRoom )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomTransfer - REJOIN_TRY, %s, 중계하려는 셔플룸 없음 %d", szPublicID.c_str(), iShuffleRoomIndex );
				return;
			}

			pShuffleRoom->ReJoinTry( dwUserIndex, szPublicID );
		}
		break;
	}
}

void ServerNode::OnShuffleRoomJoinResult( SP2Packet &rkPacket )
{
	DWORD dwUserIndex;
	int   iResultType, iShuffleRoomIndex;
	rkPacket >> dwUserIndex >> iResultType >> iShuffleRoomIndex;

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomJoinResult 유저 NULL : %d", dwUserIndex );
		return;
	}

	pUser->_OnShuffleRoomJoinResult( iResultType );

	// 변경사항 갱신
	ShuffleRoomCopyNode *pShuffleRoom = GetShuffleRoomNode( iShuffleRoomIndex );
	if( pShuffleRoom )
	{
		switch( iResultType )
		{
		case USER_SHUFFLEROOM_JOIN_FULL_USER:
			{
				int iPlayUserCnt, iMaxPlayerBlue, iMaxPlayerRed;
				rkPacket >> iPlayUserCnt >> iMaxPlayerBlue >> iMaxPlayerRed;
				pShuffleRoom->SetJoinUserCnt( iPlayUserCnt );
				pShuffleRoom->SetMaxPlayer( iMaxPlayerBlue, iMaxPlayerRed );
			}
			break;
		}
	}
}

void ServerNode::OnShuffleRoomSync( SP2Packet &rkPacket )
{
	int iSyncType, iShuffleRoomIndex;
	rkPacket >> iSyncType >> iShuffleRoomIndex;

	switch( iSyncType )
	{
	case ShuffleRoomSync::BRS_PLAY:
		{
			ShuffleRoomCopyNode *pCopyNode = GetShuffleRoomNode( iShuffleRoomIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncPlay( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomSync Not Room : %d - %d - %s:%d", iSyncType, iShuffleRoomIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;	
	case ShuffleRoomSync::BRS_CHANGEINFO:
		{
			ShuffleRoomCopyNode *pCopyNode = GetShuffleRoomNode( iShuffleRoomIndex );
			if( pCopyNode )
				pCopyNode->ApplySyncChangeInfo( rkPacket );
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnShuffleRoomSync Not Room : %d - %d - %s:%d", iSyncType, iShuffleRoomIndex, GetServerIP().c_str(), GetServerPort() );
		}
		break;
	case ShuffleRoomSync::BRS_CREATE:
		{
			ShuffleRoomCopyNode *pNewNode = CreateNewShuffleRoom( iShuffleRoomIndex );
			if( pNewNode )
			{
				pNewNode->ApplySyncCreate( rkPacket );
				g_ShuffleRoomManager.AddCopyShuffleRoom( pNewNode );
			}
		}
		break;
	case ShuffleRoomSync::BRS_DESTROY:
		{
			RemoveShuffleRoomNode( iShuffleRoomIndex );
		}
		break; 
	}
}

void ServerNode::OnShuffleRoomKickOut( SP2Packet &rkPacket )
{
	BYTE eKickType = (BYTE)RoomParent::RLT_NORMAL;
	PACKET_GUARD_VOID( rkPacket.Read( eKickType ) );
	
	DWORD dwUserIndex;
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
		
	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnBattleRoomKickOut 유저 NULL : %d", dwUserIndex );
		return;
	}
	pUser->ShuffleRoomKickOut();
	pUser->ShuffleRoomKickOut( eKickType );
}

void ServerNode::OnExceptionShuffleRoomLeave( SP2Packet &rkPacket )
{
	int iShuffleRoomIndex;
	rkPacket >> iShuffleRoomIndex;
	ShuffleRoomNode *pShuffleRoom = g_ShuffleRoomManager.GetShuffleRoomNode( iShuffleRoomIndex );	
	if( !pShuffleRoom ) return;

	DWORD dwUserIndex;
	ioHashString szPublicID;
	rkPacket >> dwUserIndex >> szPublicID;
	pShuffleRoom->LeaveUser( dwUserIndex, szPublicID );

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ServerNode::OnExceptionBattleRoomLeave(%d) : %s", iShuffleRoomIndex, szPublicID.c_str() );
}

void ServerNode::OnShuffleRoomGlobalCreate( SP2Packet &rkPacket )
{
	DWORD dwUserIdx        = 0;
	DWORD dwShuffleRoomIdx = 0;
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIdx ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwShuffleRoomIdx ) );
	
	User* pUser = g_UserNodeManager.GetUserNode( dwUserIdx );
	if( pUser && pUser->IsUserShuffleRoomJoin() )
	{
		ShuffleRoomParent* pRoom = g_ShuffleRoomManager.GetGlobalShuffleRoomNode( dwShuffleRoomIdx );
		if( pRoom )
		{
			pUser->EnterShuffleRoom( pRoom );			
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - pRoom == NULL!!!", __FUNCTION__ );
		}
	}
}

void ServerNode::OnMoveNewPetData( SP2Packet &rkPacket )
{
	DWORD dwUserIdx = 0;
	CQueryResultData query_data;

	PACKET_GUARD_VOID( rkPacket.Read( dwUserIdx ) );

	User* pUser = g_UserNodeManager.GetUserNode( dwUserIdx );
	if( !pUser )
		return;

	PACKET_GUARD_VOID( rkPacket.Read( query_data ) );

	g_UserNodeManager.ProcessInsertPetData( pUser, query_data );

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Before Recv Pet Insert Packet, User Move Server.  : %s", __FUNCTION__, pUser->GetPublicID().c_str() );
}

void ServerNode::OnMoveSoldierData( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplySoldierMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveSoldierData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnMoveEtcItemData( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyEtcItemMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveEtcItemData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnMoveMedalItemData( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyMedalItemMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveMedalItemData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnMoveCostumeData( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyCostumeMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveCostumeData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnMoveAccessroyData(SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyAccessoryMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveCostumeData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnAccessoryAdd(SP2Packet &rkPacket)
{
	DWORD dwUserIndex	= 0;
	int iIndex			= 0;
	DWORD dwCode		= 0;
	BYTE byPeriodType	= 0;
	int iYMD			= 0;
	int iHM				= 0;
	BYTE byInsertType	= 0;
	int iValue1			= 0;
	int iValue2			= 0;
	int iValue3			= 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwCode) );
	PACKET_GUARD_VOID( rkPacket.Read(byPeriodType) );
	PACKET_GUARD_VOID( rkPacket.Read(iYMD) );
	PACKET_GUARD_VOID( rkPacket.Read(iHM) );
	PACKET_GUARD_VOID( rkPacket.Read(byInsertType) );
	PACKET_GUARD_VOID( rkPacket.Read(iValue1) );
	PACKET_GUARD_VOID( rkPacket.Read(iValue2) );
	PACKET_GUARD_VOID( rkPacket.Read(iValue3) );

	User* pUser = GetUserOriginalNode(dwUserIndex, rkPacket);
	if( pUser )
	{
		pUser->AddAccessoryItem(iIndex, dwCode,byPeriodType, iYMD, iHM, iValue3);
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] servernode accessory add : [code:%d value:%d]", dwCode, iValue3);

		if( byInsertType == 1 )
		{
			int iYMD	= 0;
			int iHM		= 0;

			//상품 구매처리
			SP2Packet kPacket( STPK_ACCESSORY_BUY );
			PACKET_GUARD_VOID(kPacket.Write(ITEM_BUY_OK));
			PACKET_GUARD_VOID(kPacket.Write(iIndex));
			PACKET_GUARD_VOID(kPacket.Write(dwCode));
			PACKET_GUARD_VOID(kPacket.Write(byPeriodType));
			PACKET_GUARD_VOID(kPacket.Write(iYMD));
			PACKET_GUARD_VOID(kPacket.Write(iHM));
			PACKET_GUARD_VOID(kPacket.Write(iValue1));
			PACKET_GUARD_VOID(kPacket.Write(iValue2));		//상품 기간값
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetMoney()));
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetCash()));
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetChannelingCash()));
			PACKET_GUARD_VOID(kPacket.Write(iValue3));

			pUser->SendMessage(kPacket);
			int iPeriod = iValue2 * 60;
			g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_ITEM_BUY, pUser, 0, PRESENT_ACCESSORY, 1, iValue1, iValue3, 1, iPeriod, NULL);
		}
		else if( byInsertType == 2 )
		{
			//선물함 recv
			SP2Packet kPacket( STPK_ACCESSORY_PRESENT );
			PACKET_GUARD_VOID(kPacket.Write(dwCode));
			PACKET_GUARD_VOID(kPacket.Write(iIndex));
			PACKET_GUARD_VOID(kPacket.Write(byPeriodType));
			PACKET_GUARD_VOID(kPacket.Write(iYMD));
			PACKET_GUARD_VOID(kPacket.Write(iHM));
			PACKET_GUARD_VOID(kPacket.Write(iValue3));
			pUser->SendMessage(kPacket);
		}
	}
}

void ServerNode::OnRainbowWholeChat( SP2Packet &rkPacket )
{
	ioHashString szID;
	ioHashString szChat;

	PACKET_GUARD_VOID( rkPacket.Read(szID) );
	PACKET_GUARD_VOID( rkPacket.Read(szChat) );

	SP2Packet kPacket( STPK_RAINBOW_WHOLE_CHAT );
	PACKET_GUARD_VOID( kPacket.Write(szID) );
	PACKET_GUARD_VOID( kPacket.Write(szChat) );
	g_UserNodeManager.SendMessageAll( kPacket );	
}

void ServerNode::OnCostumeAdd(SP2Packet &rkPacket)
{
	DWORD dwUserIndex	= 0;
	int iIndex			= 0;
	DWORD dwCode		= 0;
	BYTE byPeriodType	= 0;
	int iYMD			= 0;
	int iHM				= 0;
	BYTE byInsertType	= 0;
	int iValue1			= 0;
	int iValue2			= 0;
	int iValue3			= 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwCode) );
	PACKET_GUARD_VOID( rkPacket.Read(byPeriodType) );
	PACKET_GUARD_VOID( rkPacket.Read(iYMD) );
	PACKET_GUARD_VOID( rkPacket.Read(iHM) );
	PACKET_GUARD_VOID( rkPacket.Read(byInsertType) );
	PACKET_GUARD_VOID( rkPacket.Read(iValue1) );
	PACKET_GUARD_VOID( rkPacket.Read(iValue2) );
	PACKET_GUARD_VOID( rkPacket.Read(iValue3) );

	User* pUser = GetUserOriginalNode(dwUserIndex, rkPacket);
	if( pUser )
	{
		pUser->AddCostumeItem(iIndex, dwCode,byPeriodType, iValue1, iValue2);

		if( byInsertType == 1 )
		{
			int iYMD	= 0;
			int iHM		= 0;

			//상품 구매처리
			SP2Packet kPacket( STPK_COSTUME_BUY );
			PACKET_GUARD_VOID(kPacket.Write(ITEM_BUY_OK));
			PACKET_GUARD_VOID(kPacket.Write(iIndex));
			PACKET_GUARD_VOID(kPacket.Write(dwCode));
			PACKET_GUARD_VOID(kPacket.Write(byPeriodType));
			PACKET_GUARD_VOID(kPacket.Write(iYMD));
			PACKET_GUARD_VOID(kPacket.Write(iHM));
			PACKET_GUARD_VOID(kPacket.Write(0));
			PACKET_GUARD_VOID(kPacket.Write(0));
			PACKET_GUARD_VOID(kPacket.Write(iValue2));		//상품 기간값
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetMoney()));
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetCash()));
			PACKET_GUARD_VOID(kPacket.Write(pUser->GetChannelingCash()));
			PACKET_GUARD_VOID(kPacket.Write(iValue1));		//상품 코드

			pUser->SendMessage(kPacket);
			int iPeriod = iValue2 * 60;
			g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_ITEM_BUY, pUser, 0, PRESENT_COSTUME, 1, iValue1, iValue3, 1, iPeriod, NULL);
		}
		else if( byInsertType == 2 )
		{
			//선물함 recv
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

}

void ServerNode::OnMoveMissionData( SP2Packet &rkPacket )
{
	int iMovingValue;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyMissionMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveMissionData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnMoveRollBookData( SP2Packet &rkPacket )
{
	int iMovingValue	= 0;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyRollBookMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveRollBookData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnReserveCreateGuildRoom(SP2Packet &rkPacket)
{
	int iSubType		= 0;
	DWORD dwGuildIndex	= 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwGuildIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iSubType) );

	Room *pRoom = g_RoomNodeManager.CreateNewPlazaRoom( iSubType, 1 );
	if( pRoom )
	{
		ioHashString szPlazaName, szPlazaPW;
		int iMax	= 0;
		DWORD dwUserIndex	= 0;
		
		PACKET_GUARD_VOID( rkPacket.Read(szPlazaName) );
		PACKET_GUARD_VOID( rkPacket.Read(szPlazaPW) );
		PACKET_GUARD_VOID( rkPacket.Read(iMax) );
		PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );

		if( !szPlazaName.IsEmpty() )
			pRoom->SetRoomName( szPlazaName );
		if( !szPlazaPW.IsEmpty() )
			pRoom->SetRoomPW( szPlazaPW );

		pRoom->SetMaxPlayer( iMax );
		pRoom->SetPlazaModeType( PT_GUILD );
		pRoom->SetSubState( false );

		g_DBClient.OnSelectGuildBlocksInfos(dwUserIndex, dwGuildIndex, pRoom->GetRoomIndex());
	}
}

void ServerNode::OnChangeGuildRoomStatus(SP2Packet &rkPacket)
{
	DWORD dwUserIndex	= 0;
	DWORD dwGuildIndex	= 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwGuildIndex) );

	User* pUser	= g_UserNodeManager.GetUserNode( dwUserIndex );
	if( !pUser )
		return;

	if( !pUser->IsGuild() )
		return;

	if( pUser->GetGuildIndex() != dwGuildIndex )
		return;

	pUser->ActiveUserGuildRoom();
}

void ServerNode::OnJoinPersonalHQUser( SP2Packet &rkPacket )
{
	bool bInvited		= false;
	int iMapIndex		= 0;
	DWORD dwRequestUser	= 0;
	DWORD dwOwnerUser	= 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwRequestUser) );
	PACKET_GUARD_VOID( rkPacket.Read(dwOwnerUser) );
	PACKET_GUARD_VOID( rkPacket.Read(iMapIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(bInvited) );

	User *pUser = GetUserOriginalNode( dwOwnerUser, rkPacket );
	if( pUser == NULL )
	{
		if( g_UserNodeManager.GetGlobalUserNode( dwOwnerUser ) == NULL )
		{
			UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
			if( pUserParent )
			{
				SP2Packet kPacket( STPK_JOIN_PERSONAL_HQ );
				PACKET_GUARD_VOID( kPacket.Write(JOIN_PERSONAL_HQ_OWNER_OFFLINE) );
				pUserParent->RelayPacket( kPacket );
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnJoinPersonalHQUser User NULL(%d)", dwOwnerUser );
		}
		return;
	}

	pUser->_OnJoinPersonalHQ( g_UserNodeManager.GetGlobalUserNode( dwRequestUser ), iMapIndex, bInvited );
}

void ServerNode::OnPersonalHQInfo( SP2Packet &rkPacket )
{
	ioHashString szOwnerName;
	DWORD dwRequestUser	= 0, dwOwnerUser	= 0;
	PACKET_GUARD_VOID( rkPacket.Read(dwRequestUser) );
	PACKET_GUARD_VOID( rkPacket.Read(dwOwnerUser) );
	PACKET_GUARD_VOID( rkPacket.Read(szOwnerName) );

	User *pUser = GetUserOriginalNode( dwOwnerUser, rkPacket );
	if( pUser == NULL )
	{
		if( g_UserNodeManager.GetGlobalUserNode( dwOwnerUser ) == NULL )
		{
			UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
			if( pUserParent )
			{
				SP2Packet kPacket( STPK_PERSONAL_HQ_INFO );
				PACKET_GUARD_VOID( kPacket.Write(szOwnerName) );
				PACKET_GUARD_VOID( kPacket.Write((MAX_PLAYER / 2)) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
				pUserParent->RelayPacket( kPacket );
			}
		}
		return;
	}
	pUser->_OnPersonalHQInfo( g_UserNodeManager.GetGlobalUserNode( dwRequestUser ) );
}

void ServerNode::OnPersonalHQRoomInfo( SP2Packet &rkPacket )
{
	int iRoomIndex	= 0;
	PACKET_GUARD_VOID( rkPacket.Read(iRoomIndex) );

	bool bLock = false;
	DWORD dwRequestUser = 0;
	ioHashString szMasterName;

	PACKET_GUARD_VOID( rkPacket.Read(szMasterName) );
	PACKET_GUARD_VOID( rkPacket.Read(bLock) );
	PACKET_GUARD_VOID( rkPacket.Read(dwRequestUser) );
	
	Room *pRoom = g_RoomNodeManager.GetHeadquartersNode( iRoomIndex );
	if( pRoom == NULL )
	{
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_PERSONAL_HQ_INFO );
			PACKET_GUARD_VOID( kPacket.Write(szMasterName) );
			PACKET_GUARD_VOID( kPacket.Write((MAX_PLAYER / 2)) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
		
			pUserParent->RelayPacket( kPacket );
		}
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnHeadquartersRoomInfo Room NULL(%d)", iRoomIndex );
	}
	else
	{
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwRequestUser );
		if( pUserParent )
		{
			SP2Packet kPacket( STPK_PERSONAL_HQ_INFO );
			pRoom->FillHeadquartersInfo( szMasterName, bLock, kPacket );
			pUserParent->RelayPacket( kPacket );
		}
	}
}

void ServerNode::OnPersonalHQJoinAgree( SP2Packet &rkPacket )
{
	DWORD dwUserIndex	= 0, dwRoomIndex	= 0;
	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwRoomIndex) );

	User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
	if( pUser == NULL )
		return;

	pUser->_OnPersonalHQJoinAgree( dwRoomIndex );
}

void ServerNode::OnMovePersonalHQData(SP2Packet &rkPacket )
{
	int iMovingValue	= 0;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyPersonalHQMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMovePersonalHQData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnPersonalHQAddBlock(SP2Packet &rkPacket )
{
	DWORD dwUserIndex	= 0;
	DWORD dwItemCode	= 0;
	int iCount			= 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwItemCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iCount) );

	UserParent* pUserParent	= g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
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

void ServerNode::OnMoveTimeCashData( SP2Packet &rkPacket )
{
	int iMovingValue	= 0;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyTimeCashDate( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveTimeCashData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnUpdateTimeCashInfo( SP2Packet &rkPacket )
{
	DWORD dwUserIndex	= 0;
	int iResultType		= 0;
	DWORD dwCode		= 0;
	DWORD dwReceiveDate	= 0;
	ioHashString szBillingGUID;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iResultType) );
	PACKET_GUARD_VOID( rkPacket.Read(dwReceiveDate) );
	PACKET_GUARD_VOID( rkPacket.Read(szBillingGUID) );

	UserParent* pUserParent	= g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

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

void ServerNode::OnMoveTitleData( SP2Packet &rkPacket )
{
	int iMovingValue	= 0;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyTitleMoveData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveTitleData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnTitleUpdate(SP2Packet &rkPacket )
{
	DWORD dwUserIndex	= 0;
	DWORD dwCode		= 0;
	__int64 iValue		= 0;
	int iLevel			= 0;
	BYTE byPremium		= 0;
	BYTE byEquip		= 0;
	BYTE byStatus		= 0;
	BYTE byActionType	= 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(byActionType) );
	PACKET_GUARD_VOID( rkPacket.Read(dwCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iValue) );
	PACKET_GUARD_VOID( rkPacket.Read(iLevel) );
	PACKET_GUARD_VOID( rkPacket.Read(byPremium) );
	PACKET_GUARD_VOID( rkPacket.Read(byEquip) );
	PACKET_GUARD_VOID( rkPacket.Read(byStatus) );

	UserParent* pUserParent	= g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);

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

void ServerNode::OnMoveBonusCashData(SP2Packet &rkPacket )
{
	int iMovingValue	= 0;
	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(iMovingValue) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );

	User *pUser = g_UserNodeManager.GetUserNodeByPublicID( szPublicID );
	if( pUser )
	{
		pUser->ApplyBonusCashData( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNode::OnMoveBonusCashData Exception Node %s", szPublicID.c_str() );
	}
}

void ServerNode::OnResultBonusCashAdd(SP2Packet &rkPacket )
{
	DWORD dwUserIndex		= 0;
	int iAmount				= 0;
	DWORD dwExpirationDate	= 0;
	DWORD dwIndex			= 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iAmount) );
	PACKET_GUARD_VOID( rkPacket.Read(dwExpirationDate) );
	PACKET_GUARD_VOID( rkPacket.Read(dwIndex) );

	UserParent* pUserParent	= g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
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

void ServerNode::OnResultBonusCashUpdate(SP2Packet &rkPacket )
{
	DWORD dwUserIndex		= 0;
	BYTE byType				= 0;
	DWORD dwCashIndex		= 0;
	DWORD dwStatus			= 0;
	int iAmount				= 0;
	int iUsedAmount			= 0;
	int iType				= 0;
	int iValue1				= 0;
	int iValue2				= 0;
	ioHashString szGUID;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(byType) );
	PACKET_GUARD_VOID( rkPacket.Read(dwStatus) );
	PACKET_GUARD_VOID( rkPacket.Read(dwCashIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iAmount) );
	PACKET_GUARD_VOID( rkPacket.Read(iUsedAmount) );
	PACKET_GUARD_VOID( rkPacket.Read(iType) );
	PACKET_GUARD_VOID( rkPacket.Read(iValue1) );

	PACKET_GUARD_VOID( rkPacket.Read(iValue2) );	
	PACKET_GUARD_VOID( rkPacket.Read(szGUID) );	

	UserParent* pUserParent	= g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>(pUserParent);


		pUser->UpdateUserBonusCash(static_cast<BonusCashUpdateType>(byType), dwStatus, dwCashIndex, iAmount, iUsedAmount, iType, iValue1, iValue2, szGUID.c_str());
	}
	else
	{
		UserCopyNode *pUser = static_cast<UserCopyNode*>(pUserParent);

		SP2Packet kPacket(SSTPK_BONUS_CASH_UPDATE);
		PACKET_GUARD_VOID( kPacket.Write(dwUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write(byType) );
		PACKET_GUARD_VOID( rkPacket.Read(dwStatus) );
		PACKET_GUARD_VOID( kPacket.Write(dwCashIndex) );
		PACKET_GUARD_VOID( kPacket.Write(iAmount) );
		PACKET_GUARD_VOID( rkPacket.Read(iUsedAmount) );
		PACKET_GUARD_VOID( kPacket.Write(iType) );
		PACKET_GUARD_VOID( kPacket.Write(iValue1) );
		PACKET_GUARD_VOID( kPacket.Write(iValue2) );
		PACKET_GUARD_VOID( rkPacket.Read(szGUID) );	

		pUser->SendMessage(kPacket);
	}
}

void ServerNode::OnResultDiceGameGetInfo(SP2Packet &rkPacket)		// 2018-08-30 by bckim, 주사위 이벤트 추가
{	
	bool	IsExist = false;
	DWORD	dwUserIndex	= 0;
	int		iPosition	 = 0;
	int		iTrace[DICE_GAME_TRACE_DB] = {0,};
	BYTE	byBoradIndex = 0;
	int		iRewardIndex[DICE_GAME_REWARD_DB] = {0,};

	PACKET_GUARD_VOID( rkPacket.Read( IsExist ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( iPosition ) );
	for( int i = 0; i < DICE_GAME_TRACE_DB; ++i )
		PACKET_GUARD_VOID( rkPacket.Read( iTrace[i] ) );
	PACKET_GUARD_VOID( rkPacket.Read( byBoradIndex ) );
	for( int i = 0; i < DICE_GAME_REWARD_DB; ++i )
		PACKET_GUARD_VOID( rkPacket.Read( iRewardIndex[i] ) );


	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	User* pUser	= static_cast<User*>( pUserParent );
	if( pUser )
	{
		if( pUserParent->IsUserOriginal() )
		{
			if(IsExist)
			{
				pUser->SetProgressStateDice(true);
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
			PACKET_GUARD_VOID( kPacket.Write( IsExist ) );
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

// 오크통 개선
void ServerNode::OnResultOakBarrelGetInfo( SP2Packet &rkPacket )
{
	DWORD	dwUserIndex = 0;
	DWORD	dwTime = 0;
	BYTE	byStep = 0;
	BYTE	byHole[OAK_BARREL_HOLE] = {0,};
	bool	bInit = false;
	int		iLimitTime = 0;

	PACKET_GUARD_VOID( rkPacket.Read( bInit ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( byStep ) );
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		PACKET_GUARD_VOID( rkPacket.Read( byHole[i] ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwTime ) );
	PACKET_GUARD_VOID( rkPacket.Read( iLimitTime ) );

	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>( pUserParent );
		if( pUser )
		{
			if( dwTime == 0 )
			{
				CTime curTime = CTime::GetCurrentTime();
				//curTime = Help::GetSafeValueForCTimeConstructor( curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), 0, 0, 0 );
				dwTime = Help::ConvertCTimeToDate( curTime );
			}

			if( bInit == true )
				pUser->SetOakBarrelData( byStep, byHole, dwTime, iLimitTime, true );
			else
				pUser->SetOakBarrelData( byStep, byHole, dwTime, iLimitTime );

			pUser->SendOakBarrelData();
		}
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
		PACKET_GUARD_VOID( kPacket.Write( iLimitTime ) );
		pUserCopyNode->SendMessage( kPacket );
	}
}

void ServerNode::OnResultOakBarrelUpdateInfo( SP2Packet &rkPacket )
{
	DWORD	dwUserIndex = 0;
	DWORD	dwTime = 0;
	BYTE	byUpdateType = 0;
	BYTE	byStep = 0;
	BYTE	byPrevStep = 0;
	BYTE	byHole[OAK_BARREL_HOLE] = {0,};
	int		iLimitSword = 0;

	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( byUpdateType ) );
	PACKET_GUARD_VOID( rkPacket.Read( byStep ) );
	PACKET_GUARD_VOID( rkPacket.Read( byPrevStep ) );
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		PACKET_GUARD_VOID( rkPacket.Read( byHole[i] ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwTime ) );
	PACKET_GUARD_VOID( rkPacket.Read( iLimitSword ) );

	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>( pUserParent );
		if( pUser )
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
					pUser->SetOakBarrelData( byStep, byHole, dwTime, iLimitSword );
					pUser->SendOakBarrelResult( false, max(0, (int)(byPrevStep - 1)) );
				}
				break;

			case OAK_BARREL_REWARD:
				{
					pUser->SendOakBarrelReward( byStep );
					pUser->SetOakBarrelData( byStep, byHole, dwTime, iLimitSword );
				}
				break;
			}
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

void ServerNode::OnResultOakBarrelUpdateTime( SP2Packet &rkPacket )
{
	DWORD	dwUserIndex = 0;
	DWORD	dwTime = 0;

	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwTime ) );

	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>( pUserParent );
		if( pUser )
		{
			pUser->SetOakBarrelTimeData( dwTime );
		}
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


void ServerNode::OnResultOakBarrelUpdateLimitSword( SP2Packet &rkPacket )
{
	DWORD	dwUserIndex = 0;
	int		iLimitSword	= 0;

	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( iLimitSword ) );

	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
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

void ServerNode::OnResultUserMatchUpdate( SP2Packet &rkPacket )
{
	DWORD dwUserIndex = 0;
	int iPlayCount = 0, iMMR = 0, iWinCount = 0, iMaxWinCount = 0, iMaxLoseCount = 0, iRankMMR = 0;

	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( iPlayCount ) );
	PACKET_GUARD_VOID( rkPacket.Read( iMMR ) );
	PACKET_GUARD_VOID( rkPacket.Read( iWinCount ) );
	PACKET_GUARD_VOID( rkPacket.Read( iMaxWinCount ) );
	PACKET_GUARD_VOID( rkPacket.Read( iMaxLoseCount ) );
	PACKET_GUARD_VOID( rkPacket.Read( iRankMMR ) );

	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
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

void ServerNode::OnResultUserMatchLog( SP2Packet &rkPacket )
{
	DWORD dwUserIndex = 0;
	int iType = 0;
	int iStartTier = 0, iEndTier = 0;
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( iType ) );
	PACKET_GUARD_VOID( rkPacket.Read( iStartTier ) );
	PACKET_GUARD_VOID( rkPacket.Read( iEndTier ) );

	UserParent* pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser = static_cast< User* >( pUserParent );
		g_LogDBClient.OnInsertMatchInfo( pUser, iStartTier, iEndTier, pUser->GetUserMatch()->GetMatchDelayTime(), 
			pUser->GetUserMatch()->GetMMR(),
			pUser->GetUserMatch()->GetMaxWinCount(), (LogDBClient::MatchTypes)iType, 0, LogDBClient::MT_MATCHING );
	}
	else
	{
		UserCopyNode *pUser = static_cast< UserCopyNode * >( pUserParent );
		SP2Packet kPacket( SSTPK_MATCH_LOG );
		PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( iType ) );
		PACKET_GUARD_VOID( kPacket.Write( iStartTier ) );
		PACKET_GUARD_VOID( kPacket.Write( iEndTier ) );

		pUser->SendMessage( kPacket );
	}
}

void ServerNode::OnSuccessionUserInfo( SP2Packet &rkPacket )
{
	BYTE send_type;
	PACKET_GUARD_VOID( rkPacket.Read(send_type) );

	if( send_type == SEND_TO_OTHER_USER )
	{
		DWORD dwUserIndex;
		PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );

		int iTeam;
		PACKET_GUARD_VOID( rkPacket.Read(iTeam) );

		int iStartTier, iEndTier = 0;
		PACKET_GUARD_VOID( rkPacket.Read(iStartTier) );
		PACKET_GUARD_VOID( rkPacket.Read(iEndTier) );

		ioHashString szPublicID;
		int iPoint, iWinCount, iWinStreakCount;
		PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );
		PACKET_GUARD_VOID( rkPacket.Read(iPoint) );
		PACKET_GUARD_VOID( rkPacket.Read(iWinCount) );
		PACKET_GUARD_VOID( rkPacket.Read(iWinStreakCount) );

		CHARACTER kInfo;
		int iClassLevel;
		PACKET_GUARD_VOID( rkPacket.Read(kInfo) );
		PACKET_GUARD_VOID( rkPacket.Read(iClassLevel) );

		int iItemcode[MAX_CHAR_DBITEM_SLOT], iReinforce[MAX_CHAR_DBITEM_SLOT], iMaleCustom[MAX_CHAR_DBITEM_SLOT], iFemaleCustom[MAX_CHAR_DBITEM_SLOT];
		for( int i=0; i<MAX_CHAR_DBITEM_SLOT; ++i )
		{
			PACKET_GUARD_VOID( rkPacket.Read(iItemcode[i]) );
			PACKET_GUARD_VOID( rkPacket.Read(iReinforce[i]) );
			PACKET_GUARD_VOID( rkPacket.Read(iMaleCustom[i]) );
			PACKET_GUARD_VOID( rkPacket.Read(iFemaleCustom[i]) );
		}

		User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
		if( pUser )
		{
			pUser->SetMatchTier( iStartTier, iEndTier );

			SP2Packet kSendPacket( STPK_SUCCESSION_OTHER_USER_INFO );
			PACKET_GUARD_VOID( kSendPacket.Write(iTeam) );
			PACKET_GUARD_VOID( kSendPacket.Write(szPublicID) );
			PACKET_GUARD_VOID( kSendPacket.Write(iPoint) );
			PACKET_GUARD_VOID( kSendPacket.Write(iWinCount) );
			PACKET_GUARD_VOID( kSendPacket.Write(iWinStreakCount) );
			PACKET_GUARD_VOID( kSendPacket.Write(kInfo) );
			PACKET_GUARD_VOID( kSendPacket.Write(iClassLevel) );
			for( int i=0; i<MAX_CHAR_DBITEM_SLOT; ++i )
			{
				PACKET_GUARD_VOID( kSendPacket.Write(iItemcode[i]) );
				PACKET_GUARD_VOID( kSendPacket.Write(iReinforce[i]) );
				PACKET_GUARD_VOID( kSendPacket.Write(iMaleCustom[i]) );
				PACKET_GUARD_VOID( kSendPacket.Write(iFemaleCustom[i]) );
			}
			pUser->SendMessage( kSendPacket );
		}
		
	}
	else if( send_type == SEND_TO_SELF )
	{
		DWORD dwUserIndex;
		DWORD dwOtherUserIndex;
		PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
		PACKET_GUARD_VOID( rkPacket.Read(dwOtherUserIndex) );

		int iTeam;
		PACKET_GUARD_VOID( rkPacket.Read(iTeam) );

		int iStartTier, iEndTier = 0;
		PACKET_GUARD_VOID( rkPacket.Read(iStartTier) );
		PACKET_GUARD_VOID( rkPacket.Read(iEndTier) );

		User *pUser = GetUserOriginalNode( dwUserIndex, rkPacket );
		if( pUser )
		{
			pUser->SetMatchTier( iStartTier, iEndTier );
			
			UserParent *pOtherUser = g_UserNodeManager.GetGlobalUserNode( dwOtherUserIndex );
			if( pOtherUser )
			{
				if( pOtherUser->IsUserOriginal() )
				{
					User* pOther = static_cast<User*>( pOtherUser );
					SP2Packet kPacket( STPK_SUCCESSION_OTHER_USER_INFO );
					PACKET_GUARD_VOID( kPacket.Write(iTeam) );
					pUser->FillSuccessionMatchInfo( kPacket );
					pOther->SendMessage( kPacket );

					pOther->SetMatchTier( iStartTier, iEndTier );
				}
				else
				{
					UserCopyNode *pOther = static_cast<UserCopyNode*>( pOtherUser );

					// 블루의 정보를 레드에게 준다.
					SP2Packet kPacket( SSTPK_SUCCESSION_OTHER_USER_INFO );
					PACKET_GUARD_VOID( kPacket.Write((BYTE)SEND_TO_OTHER_USER) );
					PACKET_GUARD_VOID( kPacket.Write(dwOtherUserIndex) );
					PACKET_GUARD_VOID( kPacket.Write(iTeam) );
					PACKET_GUARD_VOID( kPacket.Write(iStartTier) );
					PACKET_GUARD_VOID( kPacket.Write(iEndTier) );
					pUser->FillSuccessionMatchInfo( kPacket );
					pOther->SendMessage( kPacket );
				}
			}
		}
	}
}

void ServerNode::OnSetLadderMatchTime( SP2Packet &rkPacket )
{
	DWORD dwUserIndex = 0;
	DWORD dwLadderMatchTime = 0;

	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwLadderMatchTime ) );

	if( dwLadderMatchTime == 0 )
	{
		dwLadderMatchTime = TIMEGETTIME();
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::OnSetLadderMatchTime Set LadderMatchTime");
	}
	UserParent* pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser = static_cast< User* >( pUserParent );
		pUser->SetLadderMatchTime( dwLadderMatchTime );
	}
	else
	{
		UserCopyNode *pUser = static_cast< UserCopyNode * >( pUserParent );
		SP2Packet kPacket( SSTPK_LADDER_MATCH_TIME );
		PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( dwLadderMatchTime ) );
		pUser->SendMessage( kPacket );
	}

}

// 2019-02-14 by bckim, 배틀 모드 추가
void ServerNode::OnSetBattleModeOrder( SP2Packet &rkPacket )
{
#ifdef BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::OnSetBattleModeOrder Set OnSetBattleModeOrder");

	DWORD dwUserIndex = 0;
	int	  iBattleModeOrder = 0;

	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( iBattleModeOrder ) );

	if( iBattleModeOrder == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::OnSetBattleModeOrder Set iBattleModeOrder == 0  ");
	}

	UserParent* pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser = static_cast< User* >( pUserParent );
		pUser->SetBattleModeOrder( iBattleModeOrder );
	}
	else
	{
		UserCopyNode *pUser = static_cast< UserCopyNode * >( pUserParent );		
		SP2Packet kPacket( SSTPK_BATTLE_MODE_ORDER );		
		PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( iBattleModeOrder ) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ServerNode::OnSetBattleModeOrder >>> SSTPK_BATTLE_MODE_ORDER");
	}

#endif // BATTLE_MODE_BY_BCKIM		
}
// End. 2019-02-14 by bckim, 배틀 모드 추가

void ServerNode::OnMoveCustomMedalData( SP2Packet &rkPacket )
{
	
}

void ServerNode::OnCustomMedalAdd( SP2Packet &rkPacket )
{
	DWORD	dwUserIndex = 0;
	int iItemCode = 0, iPresentIndex = 0, iPresentSlotIndex = 0, iItemIndex = 0;

	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( iItemCode ) );
	PACKET_GUARD_VOID( rkPacket.Read( iPresentIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( iPresentSlotIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( iItemIndex ) );

	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	if( pUserParent->IsUserOriginal() )
	{
		User* pUser	= static_cast<User*>( pUserParent );
		if( pUser )
		{
			pUser->SendCustomMedalDataToClient( dwUserIndex, iItemCode, iPresentIndex, iPresentSlotIndex, iItemIndex );
		}
	}
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
	}
}

void ServerNode::OnUserPracticeAdmission(SP2Packet &rkPacket)
{
	DWORD dwUserIndex = 0, dwPracticeID = 0, dwPracticeGrade = 0, dwPracticeCount = 0, dwPracticeTime = 0, dwPracticeRank = 0;
	int iMoney = 0;
	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeID) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeGrade) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeCount) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeTime) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeRank) );
	PACKET_GUARD_VOID( rkPacket.Read(iMoney) );

	TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice OnUserPracticeAdmission - [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
							dwPracticeID, dwPracticeGrade, dwPracticeCount, dwPracticeTime, dwPracticeRank);

	UserParent* pUserParent = g_UserNodeManager.GetGlobalUserNode(dwUserIndex);
	if( !pUserParent ) return;

	if(pUserParent->IsUserOriginal())
	{
		User* pUser = (User*)pUserParent;
		if( !pUser ) return;

		ioUserPractice* pPractice = pUser->GetUserPractice();
		if( !pPractice ) return;


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

void ServerNode::OnUserPracticeTime(SP2Packet &rkPacket)
{
	DWORD dwUserIndex = 0, dwPracticeID = 0, dwPracticeGrade = 0, dwPracticeCount = 0, dwPracticeTime = 0, dwPracticeRank = 0;
	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeID) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeGrade) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeCount) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeTime) );
	PACKET_GUARD_VOID( rkPacket.Read(dwPracticeRank) );

	UserParent* pUserParent = g_UserNodeManager.GetGlobalUserNode(dwUserIndex);
	if( !pUserParent ) return;

	TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice OnUserPracticeTime - [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
							dwPracticeID, dwPracticeGrade, dwPracticeCount, dwPracticeTime, dwPracticeRank);

	if(pUserParent->IsUserOriginal())
	{
		User* pUser = (User*)pUserParent;
		if( !pUser ) return;

		ioUserPractice* pPractice = pUser->GetUserPractice();
		if( !pPractice ) return;

		pPractice->SetPractice( dwPracticeID, dwPracticeGrade, dwPracticeCount, dwPracticeTime, dwPracticeRank );
	}
	else
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


#if 0
bool ServerNode::IsMyRoom( DWORD roomIndex )
{
	auto resultVal = m_roomSet.find(roomIndex);
	if( resultVal != m_roomSet.end())
	{
		return true;
	}
	else
		return false;

}

void ServerNode::AddRoom( DWORD roomIndex )
{
	m_roomSet.insert(roomIndex);

}

void ServerNode::DelRoom( DWORD roomIndex )
{
	m_roomSet.erase(roomIndex);
}




#endif