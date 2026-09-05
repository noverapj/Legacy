// DBClient.h: interface for the DBClient class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../QueryData/QueryData.h"
#include "../WemadeLog/ioLogSerialize.h"

//DB AGENT MSG TYPE
// GET : Select , SET : Insert , DEL : Delete , UPD : Update

#define DBAGENT_CONNECT_USER_SET	0x0001
#define LOGDBAGENT_SET				0x0002
#define DBAGENT_LOG_PINGPONG		0x1000

//작업 방식
#define _INSERTDB       0
#define _DELETEDB       1
#define _SELECTDB       2
#define _UPDATEDB       3   
#define _SELECTEX1DB    4 

//결과 행동
#define _RESULT_CHECK   0
#define _RESULT_NAUGHT  1
#define _RESULT_DESTROY 2

class CConnectNode;
class LogDBClient : public CConnectNode 
{
private:
	static LogDBClient *sg_Instance;
	DWORD m_dwCurrentTime;

public:
	static LogDBClient &GetInstance();
	static void ReleaseInstance();

protected:
	ioHashString	m_DBAgentIP;
	int				m_iDBAgentPort;

	ioLogSerialize	m_FT;
	vVALUETYPE		m_VT;
	CQueryData		m_Query;
	static int		m_iUserCntSendDelay;

private:
	ValueType GetValueType(VariableType nType,int len);

public:
	inline ioHashString &GetDBAgentIP()	{ return m_DBAgentIP; }
	inline int GetDBAgentPort()			{ return m_iDBAgentPort; }

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

	void OnClose( SP2Packet &packet );

public:
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

public:
	bool ConnectTo();

private:
	void OnPing();

public:
	void ProcessTime();
	void LogDBNode_SendBufferFlush();

public:
	void Reset();
	void OnInsertConnectUser( int iConnectUser, ChannelingType eChannelingType );

	enum TradeSysType
	{
		TST_REG		= 1,
		TST_BUY		= 2,
		TST_CANCEL	= 3,
		TST_TIMEOUT	= 4,
	};
	void OnInsertTrade( DWORD dwUserIndex,
						const ioHashString &rszPublicID,
						DWORD dwTradeIndex,
						DWORD dwMagicCode,
						DWORD dwValue,
						__int64 iItemPrice,
						TradeSysType eType,
						const char *szPublicIP,
						const char *szNote );

	void OnInsertTournamentRewardSet( DWORD dwTourIndex, const ioHashString &rkOwnerID, DWORD dwOwnerIndex, BYTE TourPos, DWORD dwReward1, DWORD dwReward2, DWORD dwReward3, DWORD dwReward4 );
	void SetUserCntSendDelay(int iDelayCnt) { m_iUserCntSendDelay = iDelayCnt; }
	int	GetUserCntSendDelay() { return m_iUserCntSendDelay; }

private:			/* Singleton Class */
	LogDBClient( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~LogDBClient();
};

#define g_LogDBClient LogDBClient::GetInstance()
