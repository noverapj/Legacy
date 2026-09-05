#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "ChannelNode.h"
#include "ChannelNodeManager.h"
#include "ServerNodeManager.h"
#include <algorithm>

ChannelNodeManager* ChannelNodeManager::sg_Instance = NULL;

ChannelNodeManager::ChannelNodeManager()
{
	m_dwCurTime = TIMEGETTIME();
}

ChannelNodeManager::~ChannelNodeManager()
{
}

ChannelNodeManager &ChannelNodeManager::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ChannelNodeManager;
	return *sg_Instance;
}

void ChannelNodeManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ChannelNodeManager::InitMemoryPool( const DWORD dwServerIndex )
{
	ioINILoader kLoader( "ls_config_game.ini" );
	kLoader.SetTitle( "MemoryPool" );
	int iMaxChannelNodePool = kLoader.LoadInt( "channel_pool", 10000 );
	int iStartIndex      = dwServerIndex * iMaxChannelNodePool;

	// MemPooler
	m_MemNode.CreatePool( 0, iMaxChannelNodePool, FALSE );
	for(int i = 0;i < iMaxChannelNodePool;i++ )
	{
		m_MemNode.Push( new ChannelNode( i + iStartIndex ) );
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][channel]Init memory pool : [%d ~ %d]", iStartIndex, iStartIndex + iMaxChannelNodePool - 1 );
}

void ChannelNodeManager::ReleaseMemoryPool()
{
	vChannelNode_iter iter, iEnd;
	iEnd = m_vChannelNode.end();
	
	for( iter=m_vChannelNode.begin() ; iter!=iEnd ; ++iter )
	{
		ChannelNode *pItem = *iter;
		pItem->OnDestroy();
		
		m_MemNode.Push( pItem );
	}
	
	m_vChannelNode.clear();
	m_vChannelCopyNode.clear();
	m_MemNode.DestroyPool();
}

ChannelNode *ChannelNodeManager::CreateNewNode()
{
	ChannelNode *newNode = ( ChannelNode* )m_MemNode.Remove();
	if( !newNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ChannelNodeManager::CreateNewNode MemPool Zero!");
		return NULL;
	}
	
	m_vChannelNode.push_back(newNode);
	newNode->OnCreate();
	return newNode;
}

void ChannelNodeManager::RemoveNode( DWORD dwChannelIndex )
{
	vChannelNode_iter iter, iEnd;
	iEnd = m_vChannelNode.end();
	for( iter=m_vChannelNode.begin() ; iter!=iEnd ; ++iter )
	{
		ChannelNode *pItem = *iter;
		if( pItem->GetIndex() == dwChannelIndex )
		{
			pItem->OnDestroy();
			m_MemNode.Push( pItem );
			m_vChannelNode.erase( iter );
			return;
		}
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ChannelNodeManager::RemoveNode - %d Node Not Exist", dwChannelIndex );
}

ChannelNode* ChannelNodeManager::GetChannelNode( DWORD dwIndex )
{
	vChannelNode_iter iter, iEnd;
	iEnd = m_vChannelNode.end();
	for( iter=m_vChannelNode.begin() ; iter!=iEnd ; ++iter )
	{
		ChannelNode *pItem = *iter;
		if( pItem->GetIndex() == dwIndex )
			return pItem;
	}
	
	return NULL;
}

void ChannelNodeManager::AddCopyChannel( ChannelCopyNode *pChannel )
{
	m_vChannelCopyNode.push_back( pChannel );
}

void ChannelNodeManager::RemoveCopyChannel( DWORD dwIndex )
{
	vChannelCopyNode_iter iter, iEnd;
	iEnd = m_vChannelCopyNode.end();
	for( iter=m_vChannelCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		ChannelCopyNode *pNode = *iter;
		if( pNode->GetIndex() == dwIndex )
		{
			m_vChannelCopyNode.erase( iter );
			return;
		}
	}
}

ChannelParent* ChannelNodeManager::GetGlobalChannelNode( DWORD dwIndex )
{
	ChannelNode *pOriginalChannel = GetChannelNode( dwIndex );
	if( pOriginalChannel )
		return (ChannelParent*)pOriginalChannel;

	vChannelCopyNode_iter iter, iEnd;
	iEnd = m_vChannelCopyNode.end();
	for( iter=m_vChannelCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		ChannelCopyNode *pNode = *iter;
		if( pNode->GetIndex() == dwIndex )
			return (ChannelParent*)pNode;
	}
	return NULL;
}

void ChannelNodeManager::ConnectServerNodeSync( ServerNode *pServerNode )
{
	if( pServerNode == NULL ) return;

	static vChannelNode vChannel;
	vChannel.clear();

	LOOP_GUARD();
	vChannelNode_iter iter = m_vChannelNode.begin();
	while( iter != m_vChannelNode.end() )
	{
		ChannelNode *pNode = *iter++;
		if( !pNode->IsLiveChannel() ) continue;

		vChannel.push_back( pNode );	
	}
	LOOP_GUARD_CLEAR();

	// 오리지날 채널 정보만 N개씩 끊어서 전송
	LOOP_GUARD();
	while( true )
	{
		int iMaxSize = min( SSTPK_CONNECT_SYNC_CHANNEL_MAX, (int)vChannel.size() );
		if( iMaxSize == 0 )
			break;

		SP2Packet kPacket( SSTPK_CONNECT_SYNC );
		kPacket << SSTPK_CONNECT_SYNC_CHANNEL << iMaxSize;
		for(int i = 0;i < iMaxSize;i++)
		{
			ChannelNode *pNode  = vChannel[0];
			kPacket << pNode->GetIndex();
			vChannel.erase( vChannel.begin() );
		}
		pServerNode->SendMessage( kPacket );
	}
	LOOP_GUARD_CLEAR();
}

void ChannelNodeManager::ChannelNode_AllTimeExit()
{
	if(TIMEGETTIME() - m_dwCurTime < 30000) return;      //30초마다 체크
	if( m_vChannelNode.empty() ) return;


	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	LOOP_GUARD();
	vChannelNode_iter iter = m_vChannelNode.begin();
	vChannelNode_iter iter_Prev;	
	while( iter != m_vChannelNode.end() )
	{
		iter_Prev = iter++;
		ChannelNode *pitem = *iter_Prev;
		if( !pitem->IsLiveChannel() )
		{
			// 체널 삭제
			pitem->OnDestroy();
			m_MemNode.Push( pitem );
			iter = m_vChannelNode.erase( iter_Prev );
		}
	}	
	LOOP_GUARD_CLEAR();
	m_dwCurTime = TIMEGETTIME();
}
