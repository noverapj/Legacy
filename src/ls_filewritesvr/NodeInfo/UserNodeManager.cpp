
#include "stdafx.h"
#include "UserNodeManager.h"

//#include "../Network/GameServer.h"

UserNodeManager *UserNodeManager::sg_Instance = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UserNodeManager::UserNodeManager() : m_current_timer(0), m_iMaxConnection(0)
{
	m_vUserNode.reserve(1000);	
}

UserNodeManager::~UserNodeManager()
{
	m_vUserNode.clear();
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

	ioINILoader kLoader( "FileWriteServerInfo.ini" );
	kLoader.SetTitle( "Session Buffer" );
	int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
	int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );

	kLoader.SetTitle( "MemoryPool" );
	m_iMaxConnection    = kLoader.LoadInt( "user_pool", 3000 );

	m_MemNode.CreatePool( 0, m_iMaxConnection, FALSE );
	for(int i = 0;i < m_iMaxConnection;i++)
	{
		m_MemNode.Push( new User( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize ) );
	}
}

void UserNodeManager::ReleaseMemoryPool()
{
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pUser = *iter;
		pUser->OnDestroy();
		pUser->OnSessionDestroy();
		m_MemNode.Push( pUser );
	}	
	m_vUserNode.clear();
	m_MemNode.DestroyPool();
}

User* UserNodeManager::CreateNewNode(SOCKET s)
{
	User* newNode = (User*)m_MemNode.Remove();
	if( !newNode )
	{
		LOG.PrintTimeAndLog(0,"UserNodeManager::CreateNewNode MemPool Zero!");
		return NULL;
	}

	newNode->SetSocket(s);
	newNode->OnCreate();
	return newNode;
}

void UserNodeManager::AddUserNode( User *usernode )
{
	m_vUserNode.push_back( usernode );
}

void UserNodeManager::RemoveNode( User *usernode )
{
	/*
	유저 노드를 검색하면 안됨. 호출이 워커 스레드에서 발생하고 메인 루프에서 Input / Output이됨.
	*/
	
	for( vUser::iterator it = m_vUserNode.begin() ; it != m_vUserNode.end() ; ++it )
	{
		User* node  = *it;

		if( node == usernode )
		{
			m_vUserNode.erase( it );
			m_MemNode.Push( node );
			break;
		}
	}
}

void UserNodeManager::UserNode_AllTimeExit()
{
	if( TIMEGETTIME() - m_current_timer < 30000 ) return;

	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;

	/************************************************************************/
	/* 이 함수에서 작업하길 원한다면 최초 유저 정보를 모두 로드했는지 확인  */
	/************************************************************************/
	
	while( iter != m_vUserNode.end() )
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if( item->GetSocket() == INVALID_SOCKET )
		{
			LOG.PrintTimeAndLog( 0, "이미 닫힌 소켓을 가진 노드 발견![%d:%s]", item->GetUserIndex(), item->GetPublicIP() );
			item->m_sync_time = 1;
		}
		if( item->GetSyncTime() == 0 ) continue;
		
		if( TIMEGETTIME() - item->GetSyncTime() >= item->GetSyncCheckTime() )
		{
			LOG.PrintTimeAndLog( 0, "%d분간 응답이 없어 접속 종료시킴(%d:%s) : %d - %d", item->GetSyncCheckTime() / 60000, item->GetUserIndex(), item->GetPublicIP(), TIMEGETTIME(), item->GetSyncTime() );
			item->ExceptionClose( 0 );
		}
	}
	m_current_timer = TIMEGETTIME();
}

void UserNodeManager::SendMessageAll( SP2Packet &rkPacket, User *pOnwer /*= NULL */ )
{
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pUser = *iter;
		if( pUser == NULL ) continue;
		if( pUser == pOnwer ) continue;
		if( !pUser->IsConnectState() ) continue;
		
		pUser->SendMessage( rkPacket );		
	}
}

// SendBuffer
void UserNodeManager::UserNode_SendBufferFlush()
{
	if( m_vUserNode.empty() == false )
	{
		vector< User* >::iterator	iter	= m_vUserNode.begin();
		vector< User* >::iterator	iterEnd	= m_vUserNode.end();
		vector< User* >::iterator	iterTemp;

		// while
		while( iter != iterEnd )
		{
			iterTemp = iter++;

			User *item = *iterTemp;

			if( ! item->IsConnectState() )
				continue;

			if( item->GetSocket() == INVALID_SOCKET )
				continue;

			item->FlushSendBuffer();
		}
	}
}
