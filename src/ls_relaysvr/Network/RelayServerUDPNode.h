#pragma once


#include <unordered_map>
#include "../iocpSocketDLL/UserTypeDefine.h"

struct UserData;
struct UserInfo;
class SP2Packet;

class RelayServerUDPNode : public UDPNode
{
public:
	RelayServerUDPNode(void);
	virtual ~RelayServerUDPNode(void);

public:
	void Init();
	void InitMemoryPool();
	void Destroy();

public:
	virtual void SessionClose( int index );
	virtual void ReceivePacket( CPacket &packet, int index );
	virtual void PacketParsing( CPacket &packet );

	void SetCurrentIndex( sockaddr_in& addr );

	virtual BOOL SetNetworkSecurity( int i );
	virtual int  GetConnectType();

public:
	void OnSyncTime( SP2Packet & kPacket, char * ipAddr, int port );
	void OnConnect( SP2Packet & kPacket, char * ipAddr, int port );
	void OnDefaultPacket( SP2Packet &kPacket );
	void OnCheckKingPing( SP2Packet &kPacket, const char* ipAddr, const int port );
	void OnReserveRoomJoin( SP2Packet & kPacket, char * ipAddr, int port );

protected:
	void OnCheckPingStep( UserInfo* userInfo, DWORD dwClientTime );
	bool CheckHackCount( UserInfo* userInfo, DWORD dwCurGap );
	void PrintHackLog( UserInfo* userInfo, DWORD dwCurGap );
	void SendTimeMessage( UserInfo * userInfo, int sendTime );

public://타이머 관련
	void GhostCheck();
	void ModePingCheck(); // 모드별 핑을 체크하기 위해서 현재는 왕모드만 씀 

public:
	BOOL SendMessage(const char* ip,int port, CPacket& rkPacket);
	BOOL SendMessage(const char*  ip,int port, const char* buffer, const int size);
	void SendLogic( const UDPIoInfo* recvinfo, CPacket &packet );
	void SendLog(CPacket& packet);
	int MakeIpAddres( char* rcv_ip, UDPIoInfo* recvInfo, int& port );
	int MakeIpAddres( char* rcv_ip, sockaddr_in& addr, int& port );

public:
	void SendChangeIPMessage( UserInfo* userInfo, const char* ipAddr, const int port, char* publicID);
	void SendRelayPacket( DWORD dwUserIndex,SP2Packet& spPacket );

public:
	int GetServerID(char *publicID);
	int GetServerID(int userIndex);
	UserInfo* GetUserInfo(const char* publicID);
	UserInfo* GetUserInfo(DWORD userIndex);
	BOOL InsertUserInfo(const int userIndex, const char* publicID, const int serverIndex, const char* ipAddr, const int port);
	BOOL SetUserRoomInfo(const DWORD userIndex, short roomState);
	BOOL SetUserRoomInfo(const UserData& userData, short roomState, int serverIndex);
	int DelUserInfoByServerID( const int serverIndex);
	void PushUserInfo( UserInfo* uInfo );
	BOOL DelUserInfo(const DWORD userIndex);
	BOOL DelUserInfo(const DWORD userIndex, const int serverIndex);
	int GetUserCount() { return m_userInfoMap.size(); }
	UserInfo* CreateUserInfo();

public: //get/set
	long ProcessCount() {long rtval = m_processCount; InterlockedExchange(&m_processCount,0); return rtval;}
	void IncrementProcessCount() {InterlockedIncrement(&m_processCount); }
	unsigned long Maxsize() const { return m_maxSize; }
	DWORD NodeGhostCheckTime() const { return m_dwNodeGhostCheckTime; }
	void NodeGhostCheckTime(DWORD val) { m_dwNodeGhostCheckTime = val; }

protected:
	DWORD m_dwNodeGhostCheckTime;
	SP2Packet* m_packet;
	unsigned long m_inputCount;
	unsigned long m_processCount;
	unsigned long m_SendLogicCount;
	unsigned long m_maxSize;
	int m_currentIndex;
	typedef std::unordered_map<int,UserInfo*> USERINFO;
	USERINFO m_userInfoMap;
	MemPooler<UserInfo> m_UserInfoPool; //메모리릭 확인 완료 
};
