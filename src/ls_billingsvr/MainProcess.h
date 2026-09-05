#pragma once


#define MAX_INT64_TO_STR             10

#define WAIT_TIME_FOR_RESERVE_LOGOUT 30000 // 30sec


class GoodsManager;
class ioServerBind;
class ioMonitoringBind;
class ioChannelingNodeManager;
class ioLocalManager;
class LogicThread;
class ioMgrToolBind;

class ioMainProcess : public CProcessor 
{
 
	ioServerBind		*m_pServerBind;
	ioMonitoringBind	*m_pMonitoringBind;
	LogicThread			*m_pGameLogicThread;
	ioMgrToolBind		*m_MgrToolBind;

	ioHashString m_szPublicIP;
	ioHashString m_szPrivateIP;
	int			 m_iPort;
	int          m_iMSPort;       // monitoring <-> server 포트
	int			 m_iMgrPort;	  // monitoring port

	GoodsManager *m_pGoodsMgr;
	ioChannelingNodeManager *m_pChannelingMgr;
	ioLocalManager          *m_pLocalMgr;
	
	DWORD        m_dwMainSleepMilliseconds;
	DWORD        m_dwLogOutWaitTime;

private:
	bool  m_bWantExit;
	DWORD m_dwCurTime;
	bool  m_bInfoDraw;

private:
	bool  m_bReserveLogOut;
	DWORD m_dwReserveLogOutTime;
	
private:
	DWORD m_dwDrawTimer;
	int  m_iTestMode; // 0보다크면 테스트 모드 ,  2이면 한국 상품 리스트 에러 없음
	ioHashString m_szINI;
	TCHAR m_szLogFolder[256];

	//유럽 테스트모드
	int m_iEUTestMode;
	char m_EUTestPublicIP[MAX_PATH]; 
	
private:
	long m_cashProcessCount;

 

private:	
	static ioMainProcess *sg_Instance;

	bool m_bUseMonitoring;
	
private:      //Window Init

	bool InitNetWork();

private:      //LOOP
	void Loop();
	void Process(uint32& idleTime);
	void Draw();

protected:
  

public:
	static ioMainProcess &GetInstance();
	static void ReleaseInstance();

public:
	void InitData();
	bool Initialize();
	virtual void Destroy();
 
	void ServerBindStart();
	void MonitoringBindStart();
	void LogicThreadStart();
	void MgrToolBindStart();
	bool Startup(const char* scriptName);

public:
	void SetINI( const char* szINI )	{ m_szINI = szINI; }
	BOOL LoadINI();
public: //GET
 	const ioHashString& GetINI() const			{ return m_szINI; }
	const ioHashString& GetPublicIP() const { return m_szPublicIP;}
	int  GetPort() { return m_iPort; }
	int  GetMgrPort() { return m_iMgrPort; }
	bool IsReserveLogOut() const { return m_bReserveLogOut; }
	bool IsWantExit(){ return m_bWantExit; }
 
private:      
	void ProcessTime();

public:
	void SetWantExit();
	void CrashDown();

public:
	void CheckCreateNewLog( bool bStart = false );

	bool IsTestMode();
	int GetTestMode() const { return m_iTestMode; }

	bool  IsEuTestMode();
	char  *GeEUTestPublicIP();
	
public:
	virtual void PrintTimeAndLog(int debuglv, LPSTR fmt );
	virtual void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt );

	
public://get/set
	long GetCashProcessCount() const { return m_cashProcessCount; }
	void IncrementCashProcessCount() { _InterlockedIncrement(&m_cashProcessCount); }
	void DecrementCashProcessCount() { _InterlockedDecrement(&m_cashProcessCount); }
	void SendCloseEvent();  //test operation용 임시 함수

private:	/* Singleton Class */
	ioMainProcess();
	virtual ~ioMainProcess();
};
#define g_App ioMainProcess::GetInstance()

