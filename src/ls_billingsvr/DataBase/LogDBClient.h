// DBClient.h: interface for the DBClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGDBCLIENT_H__8BBCE6AF_B8A6_4D7C_A9FA_B2B4B32491CA__INCLUDED_)
#define AFX_LOGDBCLIENT_H__8BBCE6AF_B8A6_4D7C_A9FA_B2B4B32491CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../QueryData/QueryData.h"
//DB AGENT MSG TYPE
// GET : Select , SET : Insert , DEL : Delete , UPD : Update
#define DBAGENT_CONNECT_USER_SET  0x0001
#define DBAGENT_MILEAGE_LOG_SET   0x0002

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

class MileageInfo;
class LogDBClient : public CConnectNode 
{
private:
	static LogDBClient *sg_Instance;
	DWORD m_dwCurrentTime;

public:
	static LogDBClient &GetInstance();
	static void ReleaseInstance();

private:
	ValueType GetValueType(VariableType nType,int len);

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

// insert
public:
	void OnInsertBillingServerError( ChannelingType eChannelingType, int iRet, const char *szDesc );
	enum JapanMileageLogType
	{
		JMLT_ADD = 3,
	};
	void OnInsertJapanMileageLog( DWORD dwUserIndex, const ioHashString sPrivateID, const ioHashString sPublicID, const ioHashString sPublicIP, int iPresentType, int iValue1, int iValue2, bool bPresent, char *szLogNum, int iAddMileage, int iResultValue );

private:			/* Singleton Class */
	LogDBClient( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~LogDBClient();
};
#define g_LogDBClient LogDBClient::GetInstance()
#endif // !defined(AFX_LOGDBCLIENT_H__8BBCE6AF_B8A6_4D7C_A9FA_B2B4B32491CA__INCLUDED_)
