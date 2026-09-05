#ifndef _GameServer_h_
#define _GameServer_h_

class ServerSocket;
class CCompletionHandler;
class WorkerThread;
class MPSCRecvQueue;

#define CONNECT_TYPE_GAMESERVER					   1
#define CONNECT_TYPE_MGAME_BILLING_SERVER		   2
#define CONNECT_TYPE_BUDDY_BILLING_SERVER		   3
#define CONNECT_TYPE_WEMADE_BILLING_SERVER		   4
#define CONNECT_TYPE_MONITORING					   5
#define CONNECT_TYPE_LOGDB_SERVER				   6
#define CONNECT_TYPE_THAILAND_LOGIN_SERVER		   7
#define CONNECT_TYPE_THAILAND_OTP_SERVER		   8
#define CONNECT_TYPE_THAILAND_IPBONUS_SERVER	   9
#define CONNECT_TYPE_THAILAND_BILLING_GET_SERVER  10
#define CONNECT_TYPE_THAILAND_BILLING_SET_SERVER  11
#define CONNECT_TYPE_TOONILAND_BILLING_SERVER     12
#define CONNECT_TYPE_THAILAND_IPBONUS_OUT_SERVER  13
#define CONNECT_TYPE_WEMADE_BUY_SERVER            14
#define CONNECT_TYPE_US_BILLING_SERVER            15
#define CONNECT_TYPE_NEXON_SESSION_SERVER		  16
#define CONNECT_TYPE_NEXON_BUY_SERVER			  17
#define CONNECT_TYPE_MANAGERTOOL				  18
#define CONNECT_TYPE_NEXON_EU_SERVER			  19
#define CONNECT_TYPE_NEXON_EU_SESSION_SERVER	  20

//////////////////////////////////////////////////////////////////////////
class ioServerBind : public ServerSocket
{
public:
	ioServerBind();
};
//////////////////////////////////////////////////////////////////////////
class ioMonitoringBind : public ServerSocket
{
public:
	ioMonitoringBind();
};

//////////////////////////////////////////////////////////////////////////
class ioMgrToolBind : public ServerSocket
{
public:
	ioMgrToolBind();
};
////////////////////////////////////////////////////////////////////////////

class iocpWork : public WorkerThread
{
	public:
	virtual void Run();
	
	public:
	iocpWork(DWORD dwNumber);
	virtual ~iocpWork();
};

#define MAX_BYPASS_MAGIC_TOKEN           5
class FSM;
class ServerSecurity : public NetworkSecurity
{
	FSM m_SndState;
	FSM m_RcvState;
	
	int m_iRcvCount;
	int m_iMaxRcvCheck;
	DWORD m_dwRcvCurTimer;
	
	SOCKET m_Socket;	
public:
	void InitDoSAttack( int iMaxRcvCount );
	void InitState( SOCKET csocket );
	inline int  GetRcvCount() const { return m_iRcvCount; }	
	// DoS Attack
public:
	virtual bool UpdateReceiveCount();
	
	// Packet CheckSum
protected:	
	void  EncryptMsg( CPacket &rkPacket );
	void  DecryptMsg( CPacket &rkPacket );
public:
	virtual bool IsCheckSum( CPacket &rkPacket );
	
	// Packet Replay
protected:
	int m_iCurMagicNum;
	inline void AddMagicNum() { m_iCurMagicNum++; }
	inline int GetMagicNum() const { return m_iCurMagicNum; }
	inline void ClearMagicNum() { m_iCurMagicNum = 0; }
public:
	virtual int	 GetSndState();
	virtual void UpdateSndState();
	virtual int  GetRcvState();
	virtual void UpdateRcvState();
	virtual bool CheckState( CPacket &rkPacket );
	
	// Send 
public:
	virtual void PrepareMsg( CPacket &rkPacket );
	virtual void CompletionMsg( CPacket &rkPacket );
	
public:
	ServerSecurity();
	virtual ~ServerSecurity();	
};

#endif