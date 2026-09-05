#include "stdafx.h"
#include "ioFriend.h"

#include "UserNodeManager.h"
#include "ioFriend.h"

#include ".\newpublicidrefresher.h"


template<> NewPublicIDRefresher* Singleton< NewPublicIDRefresher >::ms_Singleton = 0;

NewPublicIDRefresher::NewPublicIDRefresher(void)
{
	m_vNewPublicIdInfoVector.reserve( 10 );
}

NewPublicIDRefresher::~NewPublicIDRefresher(void)
{
	m_vNewPublicIdInfoVector.clear();
}

void NewPublicIDRefresher::AddInfo( DWORD dwUserIndex, const ioHashString &rszPublicID, const ioHashString &rszNewPublicID, ioFriend *pFriend )
{
	int iSize = m_vNewPublicIdInfoVector.size();
	if( iSize >= MAX_INFO )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Over Max Data %d :%s : %s ", __FUNCTION__, dwUserIndex, rszPublicID.c_str(), rszNewPublicID.c_str() );
		return;
	}

	if( !pFriend )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pFriend == NULL %d :%s : %s", __FUNCTION__, dwUserIndex, rszPublicID.c_str(), rszNewPublicID.c_str() );
		return;
	}

	for(vNewPublicIDInfoVector::iterator iter = m_vNewPublicIdInfoVector.begin(); iter != m_vNewPublicIdInfoVector.end(); ++iter)
	{
	    NewPublicIDInfo &rkInfo = *iter;
		if( rkInfo.m_dwUserIndex == dwUserIndex )
		{
			rkInfo.m_szPublicID    = rszPublicID;
			rkInfo.m_szNewPublicID = rszNewPublicID;
			rkInfo.m_vFriendNameVector.clear();

			for(vFRIEND_iter iterCh = pFriend->FriendIter( 0 ); iterCh != pFriend->FriendEnd();iterCh++)
			{
				FRIEND &rkFriend = *iterCh;		
				rkInfo.m_vFriendNameVector.push_back( rkFriend.m_szName );
			}
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s(change) : %d : %s : %s", __FUNCTION__, dwUserIndex, rkInfo.m_szPublicID.c_str(), rkInfo.m_szNewPublicID.c_str() );
			return;
		}
	}

	NewPublicIDInfo kInfo;
	kInfo.m_dwUserIndex   = dwUserIndex;
	kInfo.m_szPublicID    = rszPublicID;
	kInfo.m_szNewPublicID = rszNewPublicID;
	
	for(vFRIEND_iter iterAdd = pFriend->FriendIter( 0 ); iterAdd != pFriend->FriendEnd(); iterAdd++)
	{
		FRIEND &rkFriend = *iterAdd;		
		kInfo.m_vFriendNameVector.push_back( rkFriend.m_szName );
	}
	m_vNewPublicIdInfoVector.push_back( kInfo );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s : %d : %s : %s", __FUNCTION__, dwUserIndex, kInfo.m_szPublicID.c_str(), kInfo.m_szNewPublicID.c_str() );
}

void NewPublicIDRefresher::DeleteInfo( DWORD dwUserIndex )
{
	for(vNewPublicIDInfoVector::iterator iter = m_vNewPublicIdInfoVector.begin(); iter != m_vNewPublicIdInfoVector.end(); ++iter)
	{
		NewPublicIDInfo &rkInfo = *iter;
		if( rkInfo.m_dwUserIndex == dwUserIndex )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s : %d : %s : %s", __FUNCTION__, dwUserIndex, rkInfo.m_szPublicID.c_str(), rkInfo.m_szNewPublicID.c_str() );
			m_vNewPublicIdInfoVector.erase( iter );
			return;
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail : %d", __FUNCTION__, dwUserIndex );
}

void NewPublicIDRefresher::SendFriendsAndDelete( DWORD dwUserIndex )
{
	for(vNewPublicIDInfoVector::iterator iter = m_vNewPublicIdInfoVector.begin(); iter != m_vNewPublicIdInfoVector.end(); ++iter)
	{
		NewPublicIDInfo &rkInfo = *iter;
		if( rkInfo.m_dwUserIndex != dwUserIndex )
			continue;
		
		for(ioHashStringVec::iterator iterName = rkInfo.m_vFriendNameVector.begin(); iterName != rkInfo.m_vFriendNameVector.end(); ++iterName )
		{
		    ioHashString &rszName = *iterName;
			UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( rszName );
			if( !pUserParent )
				continue;
			
			SP2Packet kPacket( STPK_CHANGE_USER_NAME );

			PACKET_GUARD_VOID( kPacket.Write(rkInfo.m_szPublicID) );
			PACKET_GUARD_VOID( kPacket.Write(rkInfo.m_szNewPublicID) );

			pUserParent->RelayPacket( kPacket );
		}
		
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s : %d : %s : %s", __FUNCTION__, dwUserIndex, rkInfo.m_szPublicID.c_str(), rkInfo.m_szNewPublicID.c_str() );
		m_vNewPublicIdInfoVector.erase( iter );
		return;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail : %d", __FUNCTION__, dwUserIndex );
}

NewPublicIDRefresher& NewPublicIDRefresher::GetSingleton()
{
	return Singleton<NewPublicIDRefresher>::GetSingleton();
}