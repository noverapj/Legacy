#include "stdafx.h"
#include "ioPacketStatistics.h"


ioPacketStatistics::ioPacketStatistics(void) : m_iTopN(5)
{
	Init();
}

ioPacketStatistics::~ioPacketStatistics(void)
{
	Destroy();
}

void ioPacketStatistics::Init()
{
}

void ioPacketStatistics::Destroy()
{
	m_vPacketIDs.clear();
	m_vPacketCounts.clear();
	m_vPacketStatistics.clear();
}

void ioPacketStatistics::Hit(const int iPacketID)
{
#ifdef _PK_STAT
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
#endif // _PK_STAT
}

void ioPacketStatistics::Extract()
{
#ifdef _PK_STAT
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

#endif // _PK_STAT
}

void ioPacketStatistics::Statistics(char* szLog, const int iSize)
{
#ifdef _PK_STAT
	ZeroMemory( szLog, iSize );

	char szTemp[512] = {0};
	for(int i = 0 ; i < m_vPacketIDs.size() ; i++)
	{
		sprintf_s( szTemp, "[PKT:0x%06x-%d]", m_vPacketIDs[i], m_vPacketCounts[i] );
		strcat_s( szLog, iSize, szTemp );
	}

	m_vPacketIDs.clear();
	m_vPacketCounts.clear();

#endif // _PK_STAT
}