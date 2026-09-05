#pragma once

#include <unordered_map>
#include "NexonDefine.h"


class CConnectNode;
//struct NexonPacketHeader;


typedef std::list< NexonLogin >RELOGINUSERS;
typedef std::list< OtherLogin >OTHERUSERS;

class NexonSessionServer : public CConnectNode
{
protected:
	enum 
	{ 
		UPDATE_TIME = 60000, // 3분 connect() 함수 딜레이 때문
	};

protected:
	static NexonSessionServer *sg_Instance;
	NexonPacketHeader m_packetParse;
	std::vector<OnNexonSyncUser> m_users;
	
	RELOGINUSERS m_nexonReLoginUsers;
	OTHERUSERS	 m_otherReLoginUsers;	


	SP2Packet m_packet;
	DWORD  m_dwCurrentTime;

protected:
	bool   m_bSendAlive;
	bool   m_bFirst;
	bool   m_bIsInit;
	bool   m_bSyncState;
	BYTE   m_iMonitorState;


protected:
	NexonAlive m_onAlive;


protected:
	char m_szServerIP[MAX_PATH];
	int m_iSSPort;
	int m_nexonDomainSn;

public:
	static NexonSessionServer &GetInstance();
	static void ReleaseInstance();

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual void DispatchReceive(CPacket& packet, DWORD bytesTransferred);
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

public:
	virtual void OnCreate();       //초기화
	virtual bool AfterCreate();
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

public:
	bool ConnectTo( bool bStart );
	void SendAlivePacket();

protected:
	void InitData();

public:
	void ProcessTime();

public: // 보내는 부분 
	
public:
	void OnClose( SP2Packet &rkPacket );
	void OnInitialzie( SP2Packet& rkPacket );
	void OnLogin( SP2Packet& rkPacket );

	void SendPCRoomPacket( NexonUserInfo* userInfo, OnNexonLogin &onLogin, DWORD pcRoomNo );

	void OnTerminate( SP2Packet& rkPacket );
	void OnMessage( SP2Packet& rkPacket );
	void OnSync( SP2Packet& rkPacket );
	void OnAlive( SP2Packet& rkPacket );

public:
	bool SendInitPacket(bool bFirst);
	bool SendReLoginPacket();
	bool SendLoginPacket(const char* userID, const char* privateID, const char* publicIP, const char* privateIP, const int iChanType);
	bool SendLogoutPacket(const uint64_t sessionNo);
	bool SendLogoutPacket(const char* chanID, const uint64_t sessionNo );
	bool SendLogoutPacket(const DWORD userIndex);
	bool SendLogoutPacketForPCRoomUser(const DWORD userIndex);
	bool SendLogoutPacketByUserInfo(const NexonUserInfo* userInfo);
	int  SendSyncPacket(BYTE monitorState);
	void SendClosePacket(bool bNow,int reson,NexonUserInfo* userInfo,int errcode =0,DWORD time = 0);
	void SelectShutdownReasonPacket(bool bState, int errCode, DWORD timeCode,NexonUserInfo* userInfo);
	bool OptionCheck(BYTE option, NexonUserInfo* userInfo, int argument, bool bLogin = false);
	void CheckSessionServer();

public:
	void DeleteReLoginUser( const char* szUserId );

public:
	bool GetPropertyResult(SP2Packet& rkPacket,NexonPolicyResult& OnPolicy, BYTE propertyCount = 0);

	bool ParsePolicyPacket( SP2Packet &rkPacket, NexonPolicyResult &OnPolicy, BYTE policyCount );

public://get/set
	bool GetInitState() const { return m_bIsInit; }
	void SetInitState(bool val) { m_bIsInit = val; }
	BYTE GetMonitorState() const { return m_iMonitorState; }
	void SetMonitorState(BYTE val) { m_iMonitorState = val; }

	void parseDiscription(int& errCode, DWORD& time ,std::string parseString);
 
protected:
	NexonSessionServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~NexonSessionServer();
};
#define g_NexonSessionServer NexonSessionServer::GetInstance()
 