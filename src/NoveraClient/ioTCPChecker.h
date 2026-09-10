#pragma once


class ioTCPChecker : public Singleton< ioTCPChecker >
{
public:
	enum
	{
		MAX_RECV_HIGH_SIZE = 10,
	};

	enum
	{
		CheckType_Packet   = 1,
		CheckType_PlayMode = 2,
	};

protected:
	struct CheckData
	{
		DWORD m_dwType;
		DWORD m_dwDataValue;
		DWORD m_dwTime;

		CheckData()
		{
			m_dwType	 = 0;			
			m_dwDataValue= 0;
			m_dwTime     = 0;
		}
	};
	typedef std::vector< CheckData > vCheckData;
	vCheckData   m_vCheckData;
	char m_szLOG[MAX_PATH];

public:
	void AddRecvPacket( DWORD dwID );
	void AddPlayModeChange( DWORD dwType = 0 );
	char *GetLOG();


public:
	static ioTCPChecker& GetSingleton();

public:
	ioTCPChecker();
	virtual ~ioTCPChecker();
};

#define g_TCPChecker ioTCPChecker::GetSingleton()

