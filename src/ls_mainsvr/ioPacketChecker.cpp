#include "stdafx.h"
#include "ioPacketChecker.h"

ioPacketChecker *ioPacketChecker::sg_Instance = NULL;
ioPacketChecker::ioPacketChecker()
{
	m_dwCurrentTime = m_dwCheckerPassTime = 0;
	m_iMaxLogCount = 0;
	m_iFreezingPacketSessionCount = m_iFreezingPacketQueryCount = 0;
	m_bFreezing = false;
}

ioPacketChecker::~ioPacketChecker()
{
	WriteLOG();
}

ioPacketChecker &ioPacketChecker::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioPacketChecker;
	return *sg_Instance;
}

void ioPacketChecker::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ioPacketChecker::LoadINI()
{
	ioINILoader kLoader( "config/sp2_process.ini" );
	kLoader.SetTitle( "PacketChecker" );
	m_dwCheckerPassTime = kLoader.LoadInt( "CheckerPassTime", 0 );
	m_iMaxLogCount = kLoader.LoadInt( "MaxLogCount", 10 );
}

void ioPacketChecker::WriteLOG()
{
	if( !m_SessionPacket.empty() )
	{
		std::sort( m_SessionPacket.begin(), m_SessionPacket.end(), PacketDataSort() );
		int iLogCount = min( m_iMaxLogCount, (int)m_SessionPacket.size() );
		for(int i = 0;i < iLogCount;i++)
		{
			PacketData &rkData = m_SessionPacket[i];
			LOG.PrintTimeAndLog( 0, "SessionPacket [0x%x] : %I64d", rkData.m_dwID, rkData.m_iPacketCount );
		}
		LOG.PrintTimeAndLog( 0, "SessionPacket [Freezing] : %I64d", m_iFreezingPacketSessionCount );
		m_SessionPacket.clear();	
		m_iFreezingPacketSessionCount = 0;
	}

	if( !m_QueryPacket.empty() )
	{
		std::sort( m_QueryPacket.begin(), m_QueryPacket.end(), PacketDataSort() );
		int iLogCount = min( m_iMaxLogCount, (int)m_QueryPacket.size() );
		for(int i = 0;i < iLogCount;i++)
		{
			PacketData &rkData = m_QueryPacket[i];
			LOG.PrintTimeAndLog( 0, "QueryPacket [0x%x] : %I64d", rkData.m_dwID, rkData.m_iPacketCount );
		}
		LOG.PrintTimeAndLog( 0, "QueryPacket [Freezing] : %I64d", m_iFreezingPacketQueryCount );
		m_QueryPacket.clear();
		m_iFreezingPacketQueryCount = 0;
	}
}

void ioPacketChecker::CheckCollectFreezing()
{
	m_bFreezing = false;
	if( TIMEGETTIME() - m_dwCurrentTime >= m_dwCheckerPassTime )
		m_bFreezing = true;
	m_dwCurrentTime = TIMEGETTIME();
}

void ioPacketChecker::SessionPacket( DWORD dwPacketID )
{
	if( m_bFreezing )
	{
		m_iFreezingPacketSessionCount++;		
	}
	else
	{
		for(vPacketData::iterator iter = m_SessionPacket.begin();iter != m_SessionPacket.end();iter++)
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
		m_SessionPacket.push_back( kData );
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