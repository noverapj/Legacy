#include "stdafx.h"
#include "ioPacketChecker.h"
 
 
ioPacketChecker::ioPacketChecker() : m_currentTime(0), m_dwCheckerPassTime(0), m_iMaxLogCount(0), m_iFreezingPacketSessionCount(0), m_iFreezingPacketQueryCount(0), m_bFreezing(false)
{
}

ioPacketChecker::~ioPacketChecker()
{
	WriteLOG();
}

 
void ioPacketChecker::LoadINI()
{
	m_dwCheckerPassTime = g_Config()->CheckerPassTime();
	m_iMaxLogCount = g_Config()->MaxLogCount();
}

void ioPacketChecker::WriteLOG()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( !m_sessionPacket.empty() )
	{
		std::sort( m_sessionPacket.begin(), m_sessionPacket.end(), PacketDataSort() );
		int iLogCount = min( m_iMaxLogCount, (int)m_sessionPacket.size() );
		for(int i = 0;i < iLogCount;i++)
		{
			PacketData &rkData = m_sessionPacket[i];
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SessionPacket [0x%x] : %I64d", rkData.m_dwID, rkData.m_iPacketCount );
		}
		
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SessionPacket [Freezing] : %I64d", m_iFreezingPacketSessionCount );
		m_sessionPacket.clear();	
		m_iFreezingPacketSessionCount = 0;
	}

	if( !m_QueryPacket.empty() )
	{
		std::sort( m_QueryPacket.begin(), m_QueryPacket.end(), PacketDataSort() );
		int iLogCount = min( m_iMaxLogCount, (int)m_QueryPacket.size() );
		for(int i = 0;i < iLogCount;i++)
		{
			PacketData &rkData = m_QueryPacket[i];
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QueryPacket [0x%x] : %I64d", rkData.m_dwID, rkData.m_iPacketCount );
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QueryPacket [Freezing] : %I64d", m_iFreezingPacketQueryCount );
		m_QueryPacket.clear();
		m_iFreezingPacketQueryCount = 0;
	}

	if( !m_PacketSizeMap.empty() || !m_PacketSizeDBMap.empty() )
	{
		PacketSizeMap::iterator iter = m_PacketSizeMap.begin();
		for(;iter != m_PacketSizeMap.end();iter++)
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Session Packet [0x%x] : %dbyte", (DWORD)iter->first, (int)iter->second );
		}

		for(iter = m_PacketSizeDBMap.begin();iter != m_PacketSizeDBMap.end();iter++)
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DBAgent Packet [0x%x] : %dbyte", (DWORD)iter->first, (int)iter->second );
		}
	}
}

void ioPacketChecker::CheckCollectFreezing()
{
	m_bFreezing = false;
	if( TIMEGETTIME() - m_currentTime >= m_dwCheckerPassTime )
		m_bFreezing = true;
	m_currentTime = TIMEGETTIME();
}

void ioPacketChecker::SessionPacket( DWORD dwPacketID )
{
	if( m_bFreezing )
	{
		m_iFreezingPacketSessionCount++;		
	}
	else
	{
		FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김
		for(vPacketData::iterator iter = m_sessionPacket.begin();iter != m_sessionPacket.end();iter++)
		{
			PacketData &rkData = *iter;
			if( rkData.m_dwID == dwPacketID )
			{
				rkData.m_iPacketCount++;
				return;
			}
		}

		PacketData kData;
		kData.m_dwID = dwPacketID;
		kData.m_iPacketCount = 1;
		m_sessionPacket.push_back( kData );
	}
}

void ioPacketChecker::QueryPacket( DWORD dwQueryID )
{
	if( m_bFreezing )
	{
		m_iFreezingPacketQueryCount++;
	}
	else
	{
		FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김
		for(vPacketData::iterator iter = m_QueryPacket.begin();iter != m_QueryPacket.end();iter++)
		{
			PacketData &rkData = *iter;
			if( rkData.m_dwID == dwQueryID )
			{
				rkData.m_iPacketCount++;
				return;
			}
		}

		PacketData kData;
		kData.m_dwID = dwQueryID;
		kData.m_iPacketCount = 1;
		m_QueryPacket.push_back( kData );
	}
}

void ioPacketChecker::PacketSizeCheck( DWORD dwID, int iSize )
{
	if( iSize <= CHECK_PACKET_SIZE )
		return;

	PacketSizeMap::iterator iter = m_PacketSizeMap.find( dwID );
	if( iter == m_PacketSizeMap.end() )
	{
		m_PacketSizeMap.insert( PacketSizeMap::value_type( dwID, iSize ) );
	}
}

void ioPacketChecker::PacketDBSizeCheck( DWORD dwID, int iSize )
{
	if( iSize <= CHECK_PACKET_SIZE )
		return;

	PacketSizeMap::iterator iter = m_PacketSizeDBMap.find( dwID );
	if( iter == m_PacketSizeDBMap.end() )
	{
		m_PacketSizeDBMap.insert( PacketSizeMap::value_type( dwID, iSize ) );
	}
}