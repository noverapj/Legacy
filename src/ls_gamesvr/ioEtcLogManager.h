
#ifndef _ioEtcLogManager_h_
#define _ioEtcLogManager_h_

class ioEtcLogManager
{
private:
	static ioEtcLogManager *sg_Instance;

protected:
	// 중계한 패킷 수
	__int64 m_iUDPTransferCount;
	__int64 m_iUDPTransferTCPCount;
	__int64 m_iUDPTransferTCPSendCount;

	// 비정상 종료된 유저 수
	int m_iExceptionDisconnectUserCount;

	// DataType / Count
	typedef std::map< DWORD, int > LogDataMap;

	// 룸 입장 후 로딩된 시간 sec, count	
	LogDataMap  m_RoomEnterLoadMap;

	// UDP 받은 패킷 packetID, count
	LogDataMap  m_UDPPacketID;

public:
	void WriteLOG();
	void PlusUDPTransfer(){ m_iUDPTransferCount++; }
	void PlusUDPTransferTCP(){ m_iUDPTransferTCPCount++; }
	void PlusUDPTransferTCPSend( int iAddCount ){ m_iUDPTransferTCPSendCount += iAddCount; }
	void PlusExceptionDisconnect(){ m_iExceptionDisconnectUserCount++; } 
	void RoomEnterLoadTime( DWORD dwLoadingTime );
	void UDPPAcketRecv( DWORD dwPacketID );

public:
	__int64 GetUDPTransferCount(){ return m_iUDPTransferCount; }
	int GetExceptionDisconnectCount(){ return m_iExceptionDisconnectUserCount; }

public:
	__int64 GetUDPTransferTCPCount(){ return m_iUDPTransferTCPCount; }
	__int64 GetUDPTransferTCPSendCount(){ return m_iUDPTransferTCPSendCount; }

public:
	static ioEtcLogManager &GetInstance();
	static void ReleaseInstance();

public:
	void LoadINI();
	void Initialize();

private:
	ioEtcLogManager();
	virtual ~ioEtcLogManager();
};
#define g_EtcLogMgr ioEtcLogManager::GetInstance()
#endif