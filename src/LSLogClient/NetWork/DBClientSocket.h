#pragma once

#define WM_DBSOCKET (WM_USER + 12)

#define DTPK_QUERY 0x0001

//DB AGENT MSG TYPE
// GET : Select , SET : Insert , DEL : Delete , UPD : Update
#define DBAGENT_LOG_SET          	        0x9001

//작업 방식
#define _INSERTDB       0
#define _DELETEDB       1
#define _SELECTDB       2
#define _UPDATEDB       3   
#define _SELECTEX1DB    4 

//결과 행동
#define _RESULT_CHECK   0
#define _RESULT_NAUGHT  1


#define MAX_DB_LOG_TYPE_MINUS_ONE     31
#define MAX_DB_LOG_CONTENTS_MINUS_ONE 1499
#define MAX_DB_LOG_TITLE 200
#define MAX_DB_LOG_ZONE_NAME_MINUS_ONE 15

class CQueryResultData;

class DBClientSocket : public ioClientTCPSocket
{
public:
	virtual void ConnectProcess();
	virtual void DisconnectProcess();
	virtual void ProcessPacket( CPacket &rkPacket );
	virtual bool CheckNS( CPacket &rkPacket );

public:
	// send
	void OnInsertLog(CString szZoneName, CString szName,CString szContents, int iErrorNumber);
protected:
	// recv
	void OnResultInsertLog(CQueryResultData *query_data);

protected:
	void ChangeForInsertDB( OUT CString &szConvert , int iMaxLength );

public:
	DBClientSocket();
	virtual ~DBClientSocket();
};