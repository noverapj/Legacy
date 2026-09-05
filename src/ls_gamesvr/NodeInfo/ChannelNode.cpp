#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "UserNodeManager.h"
#include "ServerNodeManager.h"
#include "ChannelNodeManager.h"
#include "ChannelNode.h"
#include <algorithm>

ChannelNode::ChannelNode( DWORD dwIndex ) : m_dwIndex( dwIndex )
{

}

ChannelNode::~ChannelNode()
{
	m_vUserNode.clear();
}

void ChannelNode::OnCreate()
{
	SetManToManID( "" );

	SP2Packet kPacket( SSTPK_CHANNEL_SYNC );

	PACKET_GUARD_VOID( kPacket.Write(ChannelParent::CHANNEL_CREATE) );
	PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );

	g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void ChannelNode::OnDestroy()
{
	m_vUserNode.clear();	

	SP2Packet kPacket( SSTPK_CHANNEL_SYNC );

	PACKET_GUARD_VOID( kPacket.Write(ChannelParent::CHANNEL_DESTROY) );
	PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );

	g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void ChannelNode::EnterChannel( const DWORD dwUserIndex, const ioHashString &szUserName )
{
	CRASH_GUARD();
	if( GetManToManID() == szUserName )
		SetManToManID( "" );

	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( !pUserParent )
		return;

	ChannelUser kUser;
	kUser.m_dwUserIndex = dwUserIndex;
	kUser.m_szPublicID  = szUserName;
	m_vUserNode.push_back( kUser );

	bool bManToManChat = false;
	if( GetNodeSize() == 2 )          // 1 : 1 대화면 클라이언트에서 채널을 정리한다. 동일한 1 : 1 정리( 서버에서 처리하였지만 빅서버에서 도저히 동기화가 안됨 )
		bManToManChat = true;
	SP2Packet kPacket( STPK_CHANNEL_JOIN );
	kPacket << GetIndex() << GetNodeSize() << szUserName << GetManToManID() << bManToManChat;
	if( bManToManChat )
		kPacket << m_vUserNode[0].m_szPublicID;
	SendPacketTcp( kPacket );	
	
	SP2Packet kPacket2( STPK_CHANNEL_ALL_USER );
	FillUserJoinInfo( kPacket2 );
	pUserParent->RelayPacket( kPacket2 );	
}

void ChannelNode::LeaveChannel( const DWORD dwUserIndex, const ioHashString &szUserName )
{
	vChannelUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter != iEnd;iter++)
	{
		ChannelUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex )
		{
			m_vUserNode.erase( iter );
			if( GetNodeSize() == 1 )
				SetManToManID( szUserName );

			if( IsLiveChannel() )
			{
				SP2Packet kPacket( STPK_CHANNEL_LEAVE );
				kPacket << GetIndex() << GetNodeSize() << szUserName << GetManToManID();				
				SendPacketTcp( kPacket );		
			}	
			return;
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ChannelNode::LeaveChannel - %s Node Not Exist", szUserName.c_str() );
}

bool ChannelNode::IsLiveChannel() 
{
	return ( !m_vUserNode.empty() );
}

bool ChannelNode::IsChannelUser( const ioHashString &szID )
{
	CRASH_GUARD();
	vChannelUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter != iEnd;iter++)
	{
		ChannelUser &kUser = *iter;
		if( kUser.m_szPublicID == szID )
			return true;
	}
	return false;
}

void ChannelNode::FillUserJoinInfo( SP2Packet &rkPacket )
{
	static int max_join_count = 100;     //최대 100명만 전송.
	int iUserSize = m_vUserNode.size();
	
	int join_count = min( max_join_count, iUserSize );
	rkPacket << m_dwIndex << join_count;

	for(int i = 0;i < join_count;i++)
	{
		ChannelUser &kUser = m_vUserNode[i];
		rkPacket << kUser.m_szPublicID;
	}
}

void ChannelNode::SendPacketTcp( SP2Packet &rkPacket, DWORD dwUserIndex /* = 0 */ )
{
	vChannelUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter != iEnd;iter++)
	{
		ChannelUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex ) continue;

		UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
		if( pUser )
			pUser->RelayPacket( rkPacket );
	}
}
