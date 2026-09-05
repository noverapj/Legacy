#pragma once

class ioServerBind;
class ioMgrToolBind;
class SchedulerNode;
class ExtraItemGrowthCatalystMgr;
class TournamentManager;
class SuperGashponLimitManager;
class GuildRoomInfos;

class ioMainProcess : public CProcessor
{
	static ioMainProcess *sg_Instance;

public:
	static ioMainProcess &GetInstance();
	static void ReleaseInstance();

protected:
	DWORD m_dwLogCheckTime;
	TCHAR m_szLogFolder[256];

private:
	ioHashString m_szINI;

	ioServerBind	*m_ServerBind;
	ioMgrToolBind	*m_MgrToolBind;
	LogicThread		*m_logicThread;

	ioHashString m_szPrivateIP;
	ioHashString m_szPublicIP;
	int			 m_iPort;

	int          m_iTotalRegUser;         //전체 가입자수

	// Nagle Time
	uint32			m_nagleTime;

	bool m_bUseClientVersion;
	int  m_iClientVersion;
	bool m_bTestZone;
	int  m_MatchingTime;
		
protected:
	ExtraItemGrowthCatalystMgr *m_pExtraItemGrowthCatalystMgr;
	bool m_bUseManageToolPrivateIP;

protected:
	TournamentManager *m_pTournamentManager;
	SuperGashponLimitManager* m_pSuperGashponLimitManager;
	GuildRoomInfos* m_pGuildRoomManager;

public:	
	void SetINI( const char* szINI )	{ m_szINI = szINI; }

	bool Initialize();
	bool CreateLog();
	bool CreatePool();
	bool LoadINI();
	bool SetLocalIP( int iPrivateIPFirstByte );
	void CheckTestZone( const char *szIP );
	bool ListenNetwork();
	void SetBeforeLoop();
	bool StartModules();

public:
	virtual void Process(uint32& idleTime);

public:	// GET
	bool IsTestZone() const { return m_bTestZone; }

	const TCHAR* GetLogFolder() const			{ return m_szLogFolder; }
	const ioHashString& GetINI() const			{ return m_szINI; }
	const ioHashString& GetPublicIP() const		{ return m_szPublicIP;}
	const ioHashString& GetPrivateIP() const	{ return m_szPrivateIP;}
	int GetPort()								{ return m_iPort; }
	int GetTotalRegUser()						{ return m_iTotalRegUser; }
	void SetExit()								{ m_bWantExit = true; }
	bool IsWantExit()							{ return m_bWantExit; }
	bool IsUseManageToolPrivateIP() const		{ return m_bUseManageToolPrivateIP; }
	const uint32 GetNagleTime() const			{ return m_nagleTime; }
	void DrawModule( MAINSERVERINFO& info );
	void SetMatchingTime( int iMatchingTime )	{ m_MatchingTime = iMatchingTime; }
	int  GetMatchingTime()						{ return m_MatchingTime; }
	

private:
	void LoadClientVersion();

public:
	void SaveClientVersion( bool bUseClientVerions , int iClientVersion );
	bool IsUseClientVersion() const;
	int  GetClientVersion() const;

public:
	void Exit();
	void Save();
	void Shutdown(const int type);

	void SetTotalRegUser( int iTotal ){ m_iTotalRegUser = iTotal; }
	void AllServerExit( DWORD dwExitType, bool bPopup = true );

	// Monitor Tool
	void SetDBAgentExtend( bool bState )			{ m_bGameServerDBAgentExtend = bState; }
	void SetGameServerOption( bool bState )			{ m_bGameServerOption = bState; }
	void SetExtraItemGrowthCatalyst( bool bState )	{ m_bExtraItemGrowthCatalyst = bState; }
	void SetEventShopReload( bool bState )			{ m_bEventShopReload = bState; }

private: /* Singleton */
	ioMainProcess();
	virtual ~ioMainProcess();

	void ProcessTime();
	void GameServerDBAgentExtend();
	void GameServerOption();
	void ExtraItemGrowthCatalyst();
	void EventShopReload();

private:
	bool  m_bWantExit;

	DWORD m_dwCurTime;
	//bool  m_bInfoDraw;
	bool  m_bGameServerDBAgentExtend;
	bool  m_bGameServerOption;
	bool  m_bExtraItemGrowthCatalyst;
	bool  m_bEventShopReload;

public:
	void CheckLogAllSave();
	void CheckCreateNewLog( bool bStart = false );
	void FillGameServerOption( SP2Packet &rkPacket );

	void PrintTimeAndLog(int debuglv, LPSTR fmt );
	void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt );
};

#define g_MainProc  ioMainProcess::GetInstance()