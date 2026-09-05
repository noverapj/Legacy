
#ifndef _ioPacketChecker_h_
#define _ioPacketChecker_h_

class ioPacketChecker
{
private:
	static ioPacketChecker *sg_Instance;

protected:
	struct PacketData
	{
		DWORD   m_dwID;
		__int64 m_iPacketCount;
		PacketData()
		{
			m_dwID = 0;
			m_iPacketCount = 0;
		}
	};
	class PacketDataSort
	{
	public:
		bool operator()( const PacketData &lhs , const PacketData &rhs ) const
		{
			if( lhs.m_iPacketCount > rhs.m_iPacketCount )
				return true;
			return false;
		}
	};
	typedef std::vector< PacketData > vPacketData;
	vPacketData m_SessionPacket;
	vPacketData m_QueryPacket;
	
	__int64 m_iFreezingPacketSessionCount;
	__int64 m_iFreezingPacketQueryCount;

protected:
	DWORD m_dwCurrentTime;
	DWORD m_dwCheckerPassTime;
	int   m_iMaxLogCount;
	bool  m_bFreezing;

public:
	void LoadINI();
	void WriteLOG();

public:
	void CheckCollectFreezing();
	void SessionPacket( DWORD dwPacketID );
	void QueryPacket( DWORD dwQueryID );

public:
	static ioPacketChecker &GetInstance();
	static void ReleaseInstance();

private:
	ioPacketChecker();
	virtual ~ioPacketChecker();
};
#define g_PacketChecker ioPacketChecker::GetInstance()
#endif