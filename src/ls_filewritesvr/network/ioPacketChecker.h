
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
	class PacketDataSort : public std::binary_function< const PacketData&, const PacketData&, bool >
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

protected:
	enum
	{
		CHECK_PACKET_SIZE = 1024,
	};

	typedef std::map< DWORD, int > PacketSizeMap;          
	PacketSizeMap m_PacketSizeMap;				// 1024byte용 데이터
	PacketSizeMap m_PacketSizeDBMap;			// 1024byte용 데이터

public:
	void LoadINI();
	void WriteLOG();

public:
	void CheckCollectFreezing();
	void SessionPacket( DWORD dwPacketID );
	void QueryPacket( DWORD dwQueryID );

public:
	void PacketSizeCheck( DWORD dwID, int iSize );
	void PacketDBSizeCheck( DWORD dwID, int iSize );

public:
	static ioPacketChecker &GetInstance();
	static void ReleaseInstance();

private:
	ioPacketChecker();
	virtual ~ioPacketChecker();
};
#define g_PacketChecker ioPacketChecker::GetInstance()
#endif