#include "stdafx.h"
#include "ioEtcLogManager.h"
//#include "Window.h"
#include "MainProcess.h"

//#include "Network/GameServer.h"
#include "Network/ioPacketQueue.h"

ioEtcLogManager *ioEtcLogManager::sg_Instance = NULL;
ioEtcLogManager::ioEtcLogManager()
{
	Initialize();
}

ioEtcLogManager::~ioEtcLogManager()
{

}

ioEtcLogManager &ioEtcLogManager::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioEtcLogManager;
	return *sg_Instance;
}

void ioEtcLogManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ioEtcLogManager::LoadINI()
{
}

void ioEtcLogManager::Initialize()
{
	m_iUDPTransferCount = 0;
	m_iUDPTransferTCPCount = 0;
	m_iUDPTransferTCPSendCount = 0;
	m_iExceptionDisconnectUserCount = 0;	
	m_RoomEnterLoadMap.clear();
	m_UDPPacketID.clear();
}

void ioEtcLogManager::WriteLOG()
{
	int usingCount[4], remainCount[4];
	g_RecvQueue.GetPoolCount( usingCount, remainCount );

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "" );
//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TCP AND UDP Recv Queue : %d,%d,%d,%d - %d", usingCount[0], usingCount[1], usingCount[2], usingCount[3], g_Relay.RemainderNode() );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UDP TRANSFER COUNT : %I64d(%I64d:%I64d) - ExceptionDisconnect : %d", GetUDPTransferCount(), GetUDPTransferTCPCount(), GetUDPTransferTCPSendCount(), GetExceptionDisconnectCount() );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UDP TRANSFER COUNT : %I64d - ExceptionDisconnect : %d", GetUDPTransferCount(), GetExceptionDisconnectCount() );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "2009/04/01 브리핑 시간 3초" );
	LogDataMap::iterator iCreate;
	for( iCreate = m_RoomEnterLoadMap.begin() ; iCreate != m_RoomEnterLoadMap.end() ; ++iCreate )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room Loading : %dSec - %d명", iCreate->first, iCreate->second );
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "" );
	Initialize();
}

void ioEtcLogManager::RoomEnterLoadTime( DWORD dwLoadingTime )
{
	LogDataMap::iterator iter = m_RoomEnterLoadMap.find( dwLoadingTime );
	if( iter != m_RoomEnterLoadMap.end() )
	{
		int &kCount = iter->second;
		kCount++;
	}
	else 
	{
		m_RoomEnterLoadMap.insert( LogDataMap::value_type( dwLoadingTime, 1 ) );
	}
}

void ioEtcLogManager::UDPPAcketRecv( DWORD dwPacketID )
{
	LogDataMap::iterator iter = m_UDPPacketID.find( dwPacketID );
	if( iter != m_UDPPacketID.end() )
	{
		int &kCount = iter->second;
		kCount++;
	}
	else 
	{
		m_UDPPacketID.insert( LogDataMap::value_type( dwPacketID, 1 ) );
	}
}