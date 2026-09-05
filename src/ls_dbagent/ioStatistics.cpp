#include "stdafx.h"
#include "ioStatistics.h"


ioStatistics::ioStatistics(void) : m_iTopN(7)
{
	Init();
}

ioStatistics::~ioStatistics(void)
{
	Destroy();
}

void ioStatistics::Init()
{
}

void ioStatistics::Destroy()
{
	m_vPacketIDs.clear();
	m_vPacketCounts.clear();
	m_vPacketStatistics.clear();
}

void ioStatistics::Hit(const int iPacketID)
{
	PACKET_STATISTICS::iterator it = m_vPacketStatistics.find( iPacketID );
	if( it != m_vPacketStatistics.end() )
	{
		int iCount = it->second;
		m_vPacketStatistics[ iPacketID ] = ++iCount;
	}
	else
	{
		m_vPacketStatistics[ iPacketID ] = 1;
	}
}

void ioStatistics::Extract()
{
	for(int i = 0 ; i < m_iTopN ; i++)
	{
		PACKET_STATISTICS::iterator removeIter;
		int iPacketId = 0, iPacketCount = 0;
		for(PACKET_STATISTICS::iterator it = m_vPacketStatistics.begin(); it != m_vPacketStatistics.end() ; ++it)
		{
			if(it->second > iPacketCount)
			{
				removeIter	 = it;
				iPacketId = it->first;
				iPacketCount = it->second;
			}
		}

		if(iPacketId != 0)
		{
			// 최고 카운트 저장
			m_vPacketIDs.push_back( iPacketId );
			m_vPacketCounts.push_back( iPacketCount );

			// 기록한 멤버는 삭제
			m_vPacketStatistics.erase( removeIter );
		}
	}
	m_vPacketStatistics.clear();
}

void ioStatistics::Statistics(char* szLog, const int iSize)
{
	ZeroMemory( szLog, iSize );

	char szTemp[512] = {0};
	for (size_t i = 0; i < m_vPacketIDs.size(); i++)
	{
		sprintf_s( szTemp, "[QRY:%d-%d]", m_vPacketIDs[i], m_vPacketCounts[i] );
		strcat_s( szLog, iSize, szTemp );
	}

	m_vPacketIDs.clear();
	m_vPacketCounts.clear();
}