#pragma once

class ioP2PChecker : public Singleton< ioP2PChecker >
{
public:
	enum
	{
		MAX_SEND_HIGH_SIZE = 10,
	};
protected:
	struct PacketData
	{
		DWORD   m_dwID;
		__int64 m_iSendCount;
		__int64 m_iRecvCount;

		PacketData()
		{
			m_dwID = 0;
			m_iSendCount = 0;
			m_iRecvCount = 0;
		}
	};	
	typedef std::vector< PacketData > vPacketData;
	vPacketData m_vPacketList;
	DWORD       m_dwProcessTime;

	class PacketSendSort : public std::binary_function< const PacketData&, const PacketData&, bool >
	{
	public:
		bool operator()( const PacketData &lhs , const PacketData &rhs ) const
		{
			if( lhs.m_iSendCount == 0 )
				return false;
			if( lhs.m_iSendCount > rhs.m_iSendCount )
			{
				return true;
			}				
			else if( lhs.m_iSendCount == rhs.m_iSendCount )
			{
				if( lhs.m_dwID < rhs.m_dwID )
					return true;
			}
			return false;
		}
	};

	class PacketRecvSort : public std::binary_function< const PacketData&, const PacketData&, bool >
	{
	public:
		bool operator()( const PacketData &lhs , const PacketData &rhs ) const
		{
			if( lhs.m_iRecvCount == 0 )
				return false;
			if( lhs.m_iRecvCount > rhs.m_iRecvCount )
			{
				return true;
			}				
			else if( lhs.m_iRecvCount == rhs.m_iRecvCount )
			{
				if( lhs.m_dwID < rhs.m_dwID )
					return true;
			}
			return false;
		}
	};


	//
	struct RecvLog
	{
		ioHashString m_szLog;
		DWORD        m_dwColor;

		RecvLog()
		{
			m_dwColor = 0xFFFFB400;
		}
	};
	typedef std::vector< RecvLog > vRecvLog;
	vRecvLog m_vRecvLog;
	bool     m_bView;

public:
	void Initialize();

public:
	void SwitchRecvLog();
	bool IsRecvLog(){ return m_bView; }
	void RenderLog( int iXPos, int iYPos );
	void SetControlLog( DWORD dwPacket, ioHashString szSendID, int iSendIndex, int iPrevIndex, bool bParsing );

public:
	void AddSendPacket( DWORD dwID );
	void AddRecvPacket( DWORD dwID );
	
public:
	ioHashString SendPacketLog( int iArray );
	ioHashString RecvPacketLog( int iArray );
	DWORD GetProcessTime(){ return FRAMEGETTIME() - m_dwProcessTime; }

public:
	static ioP2PChecker& GetSingleton();

public:
	ioP2PChecker();
	virtual ~ioP2PChecker();
};

#define g_P2PChecker ioP2PChecker::GetSingleton()

