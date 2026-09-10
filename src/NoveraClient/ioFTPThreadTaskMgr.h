#pragma once
#include "../io3DEngine/ioThreadTaskManager.h"

class ioFTPThreadTask : public IThreadTask
{
friend class ioFTPThreadTaskMgr;

public:
	enum FTPSTATUS
	{
		FS_NONE,
		FS_CREATE,
		FS_UPLOAD,
		FS_UPLOAD_DONE,
		FS_UPLOAD_FAIL,
	};

protected:
	DWORD m_dwCreateID;

protected:
	FTPSTATUS m_CurrentState;

	ioHashString m_szServerAddress;	
	ioHashString m_szID;
	ioHashString m_szPassword;
	ioHashString m_szFileName;

	DWORD m_dwPort;

protected:
	virtual void Process();
	virtual void Done();

	virtual int  GetPriority() const ;

protected:	
	inline void SetState( FTPSTATUS status ){ m_CurrentState = status; }

	virtual bool CreateFile();
	void FileUpLoad( const char* szUploadFileName );
	
public:
	inline DWORD GetID(){ return m_dwCreateID; }

protected:
	ioFTPThreadTask();

public:
	ioFTPThreadTask( ioHashString szServerAddress, DWORD dwPort, ioHashString szID, ioHashString szPassword, ioHashString szFileName, DWORD CreateID );
	~ioFTPThreadTask();
};

class ioCreateChar;
class ioRenderSystem;
class ioRenderTexture;
class ioUI3DRender;

class ioMyCharShotTask : public ioFTPThreadTask
{
protected:
	int m_iWidth;
	int m_iHeight;
		
	ioUI3DRender*		m_pUI3DRender;
	IDirect3DSurface9*	m_pTextureSurf;

	ioCreateChar*	m_pChar;
	D3DXVECTOR3		m_OrgCharPos;

public:
	void CharCreate( CharInfoData &rkInfo, CHARACTER pCharInfo );
	void CharEquipItem( CharInfoData &rkInfo, CHARACTER pCharInfo );

	void SetMyCharShot( CharInfoData &rkInfo, CHARACTER pCharInfo );
	virtual bool CreateFile();

protected:
	ioMyCharShotTask();

public:
	ioMyCharShotTask( ioHashString szServerAddress, DWORD dwPort, ioHashString szID, ioHashString szPassword, ioHashString szFileName, DWORD CreateID );
	~ioMyCharShotTask();
};

class ioFTPThreadTaskMgr : public Singleton< ioFTPThreadTaskMgr >
{
public:
	enum FTPTASKSTATUS
	{
		FTS_NONE,
		FTS_LOADING,
		FTS_LOADED_OK,
		FTS_LOADED_FAIL,
	};

protected:
	DWORD m_dwTaskCreate;

	ioHashString m_szServerAddress;	
	ioHashString m_szID;
	ioHashString m_szPassword;
	ioHashString m_szResourcePath;

	DWORD m_dwPort;

protected:
	typedef std::map< DWORD, FTPTASKSTATUS > TaskStatusMap;
	TaskStatusMap m_TaskStatusMap;

protected:
	typedef std::vector<ioFTPThreadTask*> ReserveTaskVec;
	ReserveTaskVec m_ReserveTaskVec;

public:
	void Process();

public:
	virtual void Initialize( const char* szResourcePath );

	ioFTPThreadTask* CreateFTPTask( ioHashString szFileName );
	ioMyCharShotTask* CreateShotTask( ioHashString szFileName );

	ioFTPThreadTaskMgr::FTPTASKSTATUS GetTaskStatus( DWORD dwID );

	void FileUpLoadDone( DWORD dwID, ioFTPThreadTaskMgr::FTPTASKSTATUS State );
	inline ioHashString GetFullPath(){ return m_szResourcePath; }

public:
	static ioFTPThreadTaskMgr& GetSingleton();
	
public:
	ioFTPThreadTaskMgr();
	virtual ~ioFTPThreadTaskMgr();
};

#define g_FTPMgr ioFTPThreadTaskMgr::GetSingleton()