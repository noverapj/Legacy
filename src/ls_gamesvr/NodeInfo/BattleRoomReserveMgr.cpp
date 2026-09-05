#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "BattleRoomManager.h"
#include "BattleRoomReserveMgr.h"
#include "ServerNodeManager.h"
#include "LevelMatchManager.h"

BattleRoomReserveNode::BattleRoomReserveNode()
{
	m_iReserveSelectMode = -1;
}

BattleRoomReserveNode::~BattleRoomReserveNode()
{
	OnDestroy();
}

void BattleRoomReserveNode::OnCreate()
{
}

void BattleRoomReserveNode::OnDestroy()
{	
	m_vList.clear();
}

void BattleRoomReserveNode::EnterUser( User *pUser )
{
	CRASH_GUARD();
	// 예외 처리
	vUser_iter iter,iEnd;
	iEnd = m_vList.end();
	for(iter = m_vList.begin();iter != iEnd;iter++)
	{
		User *pItem = *iter;
		if( !pItem )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "PartyReserveNode::EnterUser NULL Pointer!!!" );
			continue;
		}
		
		if( pItem->GetPublicID().IsEmpty() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "PartyReserveNode::EnterUser NULL PublicID!!!" );
			continue;
		}
		
		if( pItem->GetPublicID() == pUser->GetPublicID() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "PartyReserveNode::EnterUser Already ID(%s)!!!", pUser->GetPublicID().c_str() );
			return;
		}
	}

	if( pUser )
		m_vList.push_back( pUser );
}

bool BattleRoomReserveNode::LeaveUser( User *pUser )
{
	vUser_iter iter,iEnd;
	iEnd = m_vList.end();
	for(iter = m_vList.begin();iter != iEnd;iter++)
	{
		User *pItem = *iter;
		if( pItem->GetUserIndex() == pUser->GetUserIndex() )
		{
			m_vList.erase( iter );
			return true;
		}
	}
	return false;
}

void BattleRoomReserveNode::SetReserveSelectMode( int iSelectMode )
{
	if( m_iReserveSelectMode == -1 )
		m_iReserveSelectMode = iSelectMode; 
	else if( iSelectMode != BMT_ALL_MODE )
		m_iReserveSelectMode = iSelectMode;
} 

int BattleRoomReserveNode::GetUserSize()
{	
	return m_vList.size(); 
}

bool BattleRoomReserveNode::CreateBattleRoom()
{		
	ioINILoader kLoader( "config/sp2_battleroom.ini" );
	kLoader.SetTitle( "reserve_create" );	
	
	ioHashString szPartyName;
	int iMaxName   = kLoader.LoadInt( "MAX_NAME", 0 );
    if( iMaxName == 0 )
		iMaxName = 1;
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	sprintf_s( szKey, "NAME_%d", rand()%iMaxName );
	kLoader.LoadString( szKey, "파티명이 없습니다.", szBuf, MAX_PATH );
	szPartyName = szBuf;
	
	int iMaxPlayer = kLoader.LoadInt( "MAX_PLAYER", 0 );
	int iMaxObserver = kLoader.LoadInt( "MAX_OBSERVER", 4 );

	DWORD dwServerIndex = 0;
	bool bResult = g_ServerNodeManager.GetSelectBattleRoomServer( g_BattleRoomManager.GetNodeSize(), dwServerIndex );
	if( bResult)
	{
		if( 0 == dwServerIndex )  // 현재 서버에 방을 생성한다
		{
			// 파티 생성
			BattleRoomNode *pNewNode = g_BattleRoomManager.CreateNewBattleRoom();
			if( !pNewNode )
				return false;
	
			pNewNode->SetName( szPartyName );
			pNewNode->SetMaxPlayer( iMaxPlayer / 2, iMaxPlayer / 2, iMaxObserver );
			pNewNode->SetDefaultMode( GetReserveSelectMode() );

			// 파티 입장
			vUser_iter iter,iEnd;
			iEnd = m_vList.end();
			for(iter = m_vList.begin();iter != iEnd;iter++)
			{
				User *pItem = *iter;
				if( pItem )
					pItem->ExitRoomToBattleRoomJoin( pNewNode, false, false, 0 );
			}	
			return true;
		}
		else
		{
			ServerNode* pSelectServer = g_ServerNodeManager.GetServerNode( dwServerIndex );
			if( pSelectServer )
			{
				SP2Packet kPacket( SSTPK_RESERVE_CREATE_BATTLEROOM );
				kPacket << false << szPartyName << "" << (iMaxPlayer/2) << (iMaxPlayer/2) << iMaxObserver << GetReserveSelectMode();
				kPacket << (int)m_vList.size();
				
				vUser_iter iter,iEnd;
				iEnd = m_vList.end();
				for(iter = m_vList.begin();iter != iEnd;iter++)
				{
					User *pItem = *iter;
					if( pItem )
					{
						kPacket << pItem->GetUserIndex() << pItem->GetPublicID();
						pItem->ReserveBattleRoom( true );
					}
				}
				pSelectServer->SendMessage( kPacket );
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomReserveNode::CreateBattleRoom Create : %s:%d", pSelectServer->GetServerIP().c_str(), pSelectServer->GetClientPort() );
				return true;
			}
		}
	}
	return false;
}

void BattleRoomReserveNode::JoinBattleRoom( BattleRoomNode *pBattleRoom )
{
	// 파티 입장
	vUser_iter iter,iEnd;
	iEnd = m_vList.end();
	for(iter = m_vList.begin();iter != iEnd;iter++)
	{
		User *pItem = *iter;
		if( pItem )
			pItem->ExitRoomToBattleRoomJoin( pBattleRoom, false, false, 0 );
	}		
}

int BattleRoomReserveNode::GetUserLevel()
{
	int iLevel = 0;
	vUser_iter iter, iEnd;
	iEnd = m_vList.end();
	for(iter=m_vList.begin();iter!=iEnd;++iter)
	{
		User *pUser = *iter;
		if( pUser )
			iLevel += pUser->GetKillDeathLevel();
	}

	return iLevel;
}

int BattleRoomReserveNode::GetAverageLevel()
{
	int iSize = GetUserSize();
	if( iSize == 0 )
		return 0;
	return GetUserLevel() / iSize;
}

//////////////////////////////////////////////////////////////////////////
BattleRoomReserveMgr *BattleRoomReserveMgr::sg_Instance = NULL;
BattleRoomReserveMgr::BattleRoomReserveMgr()
{
	m_iMaxPartyCreateUser = 0;
}

BattleRoomReserveMgr::~BattleRoomReserveMgr()
{
	m_vBRRNode.clear();
}

BattleRoomReserveMgr &BattleRoomReserveMgr::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new BattleRoomReserveMgr;
	return *sg_Instance;
}

void BattleRoomReserveMgr::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void BattleRoomReserveMgr::InitMemoryPool()
{
	for( int i=0; i<MAX_RESERVE_BATTLEROOM; i++ )
	{
		m_MemNode.Push( new BattleRoomReserveNode );
	}
}

void BattleRoomReserveMgr::ReleaseMemoryPool()
{
	vBRRNode_iter iter, iEnd;
	iEnd = m_vBRRNode.end();
	for( iter=m_vBRRNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomReserveNode *pBRRN = *iter;		
		pBRRN->OnDestroy();
		m_MemNode.Push( pBRRN );
	}
	m_vBRRNode.clear();
	m_MemNode.DestroyPool();
}

void BattleRoomReserveMgr::LoadInfo( const char *szFileName )
{
	ioINILoader kLoader( szFileName );
	kLoader.SetTitle( "Info" );

	m_iMaxPartyCreateUser = kLoader.LoadInt( "battleroom_create_max_user", 2 );
}

bool BattleRoomReserveMgr::CreateBattleRoom( BattleRoomReserveNode *pReserveNode )
{
	if( !pReserveNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "void BattleRoomReserveMgr::CreateBattleRoom pReserveNode == NULL" );
		return false;
	}

	if( pReserveNode->GetUserSize() >= m_iMaxPartyCreateUser )
	{
		if( !pReserveNode->CreateBattleRoom() )
		{
			//더이상 파티를 만들수 없다.
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "void BattleRoomReserveMgr::CreateBattleRoom Full PartyNode" );
			return false;
		}			
		DeleteReserveBattleRoom( pReserveNode );
		return true;
	}

	return false;
}

void BattleRoomReserveMgr::JoinBattleRoom( BattleRoomReserveNode *pReserveNode, BattleRoomNode *pBattleRoom )
{
	if( !pReserveNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomReserveMgr::JoinBattleRoom pReserveNode == NULL" );
		return;
	}

	if( !pBattleRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomReserveMgr::JoinBattleRoom pBattleRoom == NULL" );
		return;
	}
	
	pReserveNode->JoinBattleRoom( pBattleRoom );
	DeleteReserveBattleRoom( pReserveNode );
}

void BattleRoomReserveMgr::ReserveBattleRoom( User *pUser, int iSearchTerm )
{
	DeleteReserveBattleRoom( pUser );
	BattleRoomReserveNode* pBRRN = ( BattleRoomReserveNode* )m_MemNode.Remove();
	if( !pBRRN )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"BattleRoomReserveMgr::ReserveParty MemPool Zero!");
		return;
	}
	
	m_vBRRNode.push_back( pBRRN );
	pBRRN->OnCreate();
	pBRRN->EnterUser( pUser );
	pBRRN->SetReserveSelectMode( iSearchTerm );
}

BattleRoomReserveNode *BattleRoomReserveMgr::DeleteReserveBattleRoom( User *pUser )
{
	vBRRNode_iter iter, iEnd;
	iEnd = m_vBRRNode.end();
	for( iter=m_vBRRNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomReserveNode *pBRRN = *iter;
		
		if( pBRRN->LeaveUser( pUser ) )
		{
			if( pBRRN->IsEmpty() )
			{
				pBRRN->OnDestroy();
				m_MemNode.Push( pBRRN );
				m_vBRRNode.erase( iter );
				return NULL;       //예약룸이 없어졌다.
			}
			return pBRRN;
		}
	}
	return NULL;  //예약룸이 없다.
}

void BattleRoomReserveMgr::DeleteReserveBattleRoom( BattleRoomReserveNode *pReserveNode )
{
	vBRRNode_iter iter, iEnd;
	iEnd = m_vBRRNode.end();
	for( iter=m_vBRRNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomReserveNode *pBRRN = *iter;
		if( pBRRN == pReserveNode )
		{
			pBRRN->OnDestroy();
			m_MemNode.Push( pBRRN );
			m_vBRRNode.erase( iter );				
			break;			
		}
	}
}

int BattleRoomReserveMgr::GetReserveAllUser()
{
	int iUserCnt = 0;
	vBRRNode_iter iter, iEnd;
	iEnd = m_vBRRNode.end();
	for( iter=m_vBRRNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomReserveNode *pBRRN = *iter;
		iUserCnt += pBRRN->GetUserSize();
	}
	return iUserCnt;
}

BattleRoomReserveNode *BattleRoomReserveMgr::SearchReserveBattleRoom( int iMyLevel, int iSearchTerm )
{
	if( m_vBRRNode.empty() ) return NULL;

	vBRRNode_iter iter, iEnd;
	iEnd = m_vBRRNode.end();
	for( iter=m_vBRRNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomReserveNode *pBRRN = *iter;
		if( !g_LevelMatchMgr.IsPartyLevelJoin( pBRRN->GetAverageLevel(), iMyLevel, JOIN_CHECK_MINMAX_LEVEL ) )
			continue;
		if( iSearchTerm != BMT_ALL_MODE && pBRRN->GetReserveSelectMode() != iSearchTerm ) continue;

		return pBRRN;
	}

	return NULL;
}