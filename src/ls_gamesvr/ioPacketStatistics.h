#pragma once

#include <unordered_map>

class ioPacketStatistics
{
public:
	ioPacketStatistics(void);
	~ioPacketStatistics(void);

public:
	void Init();
	void Destroy();

public:
	void Hit(const int iPacketID);
	void Extract();

	void Statistics(char* szLog, const int iSize);

private:
	typedef std::vector<int> PACKET_IDS;
	typedef std::vector<int> PACKET_COUNTS;
	typedef std::unordered_map<int,int> PACKET_STATISTICS;

	PACKET_IDS m_vPacketIDs;
	PACKET_COUNTS m_vPacketCounts;
	PACKET_STATISTICS m_vPacketStatistics;

	int m_iTopN;
};
