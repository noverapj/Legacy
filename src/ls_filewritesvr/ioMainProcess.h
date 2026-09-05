
class SchedulerNode;

class ioMainProcess : public CProcessor
{
private:
	ioMainProcess();
	virtual ~ioMainProcess();

	static ioMainProcess *sg_Instance;

public:
	static ioMainProcess &GetInstance();
	static void ReleaseInstance();

private:
	virtual void Process(uint32& idleTime);
	virtual void PrintTimeAndLog(int debuglv, LPSTR fmt );
	virtual	void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt );

	void ProcessTime();

private:
	ioHashString m_szINI;
	void SetINI( const char* szINI ){ m_szINI = szINI; }
	const ioHashString& GetINI() const	{ return m_szINI; }

	TCHAR m_szLogFolder[256];

	ioHashString m_szIP;
	int			 m_iPort;

	ioClientBind*	m_pClientBind;
	SchedulerNode*	m_pScheduler;
	LogicThread*	m_pLogicThread;

	bool  m_bWantExit;
	bool  m_bReserveLogOut;

	DWORD m_dwCurTime;

	// Nagle Time
	uint8 m_NagleTime;

	struct PathInfo 
	{
		ioHashString m_sExt;
		ioHashString m_sFilePath;
		ioHashString m_sTempFilePath;
		ioHashString m_sErrorFilePath;

		PathInfo()
		{
			m_sExt.Clear();
			m_sFilePath.Clear();
			m_sTempFilePath.Clear();
			m_sErrorFilePath.Clear();
		}
	};

	typedef std::vector<PathInfo> PathInfoVec;
	PathInfoVec m_vPathInfoVec;
	PathInfo m_stUserUploadPath;
	BOOL bUserUpload;
public:
	ULONG_PTR m_gdiToken;

public:
	bool Startup( const char* scriptName );
	bool Initialize( SchedulerNode* schedulerPointer );
	BOOL CreatePool();
	BOOL LoadINI();
	void SetCreateDirectoryByFullPath( const char *szDir );
	BOOL ListenNetwork();
	BOOL StartModules();
	bool ClientBindStart();

public:	// GET
	const TCHAR* GetLogFolder() const	{ return m_szLogFolder; }
	const ioHashString& GetPublicIP() const	{ return m_szIP;}
	const uint8 GetNagleTime() const { return m_NagleTime; }
	void CheckLogAllSave();
	void CheckCreateNewLog( bool bStart = false );

public:
	void Exit();
	void Save();
	void Shutdown(const int type=0);
	void GetAllPath( IN const ioHashString &rsFileName, OUT ioHashString &rsPath, OUT ioHashString &rsTempPath, OUT ioHashString &rsErrorPath );

public:
	BOOL SetUserSkinUploadInfo(ioINILoader& kLoader);
	void GetUserUploadPath( OUT ioHashString &rsPath, OUT ioHashString &rsTempPath, OUT ioHashString &rsErrorPath );
	BOOL IsPossibleUserUpload() { return bUserUpload; }

};

#define g_App  ioMainProcess::GetInstance()
