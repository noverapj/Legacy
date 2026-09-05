#ifndef _ServerCloseManager_h_
#define _ServerCloseManager_h_

class ServerCloseManager : public SuperParent
{
protected:
	static ServerCloseManager *sg_Instance;

public:
	enum
	{
		CLOSE_NONE = 0,
		CLOSE_STEP1= 1,
		CLOSE_STEP2= 2,
		CLOSE_STEP3= 3,
	};

protected:
	DWORD  m_dwMainTimer;
	DWORD  m_dwCloseState;
	CTime  m_cCloseTime;
	ioHashString m_CloseAnnounce;
	bool   m_bServerClose;

public:
	void LoadCloseServerInfo();

public:
	void ProcessCloseCheck();

public:
	bool IsServerClose(){ return m_bServerClose; }

public:
	static ServerCloseManager &GetInstance();
	static void ReleaseInstance();

private:     	/* Singleton Class */
	ServerCloseManager();
	virtual ~ServerCloseManager();
};
#define g_ServerCloseMgr ServerCloseManager::GetInstance()
#endif