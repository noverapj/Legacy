#ifndef __ioLocalUS_h__
#define __ioLocalUS_h__

#include "ioLocalParent.h"

#define FACEBOOK_DATA_TOKEN "#"


class HTTPManager;
class ioLocalUS : public ioLocalParent
{
protected:
	char m_szRegKey[MAX_PATH];
	char m_szStartURL[MAX_PATH*2];
	char m_szLoginID[MAX_PATH];
	char m_szLoginEncodePW[MAX_PATH*2];
	char m_szServerID[MAX_PATH];
	char m_szUserType[MAX_PATH];

public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetTextListFileName();
	virtual const char *GetMemTextList();
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );
	virtual DWORD GetResourceIDBackBmp();
	virtual DWORD GetResourceIDOptimizeBtnBmp();
	virtual DWORD GetResourceIDStartBtnBmp();
	virtual DWORD GetResourceIDErrorBackBtnBmp();
	virtual DWORD GetResourceIDErrorSolutionBtnBmp();
	virtual DWORD GetResourceIDErrorExitBtnBmp();

	virtual DWORD GetResourceIDLoginBackBmp() { return IDB_LOGIN_US; }
	virtual DWORD GetResourceIDLoginEnterBtnBmp() { return IDB_LOGIN_ENTER_BTN_US; }
	virtual DWORD GetResourceIDLoginEntryBtnBmp() { return IDB_LOGIN_ENTRY_BTN_US; }
	virtual DWORD GetResourceIDLoginFindIDBtnBmp() { return IDB_LOGIN_FINDID_BTN_US; }
	virtual DWORD GetResourceIDLoginFindPWBtnBmp() { return IDB_LOGIN_FINDPW_BTN_US; }

	virtual const char *GetRegKey();
	virtual const char *GetFontName();
	virtual const char *GetErrorSolutionURL();

	virtual void CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL );

#ifdef STEAM_ATTACH
    virtual bool IsLoginDlg() { return false; }
	bool CheckFileInFolder( CString* const strPath, CString* const strFile );
	int UpdateNRunSteamLauncher(HTTPManager* HttpMgr);
	bool IsLoginDataValidate();
	bool DownloadLauncher(const CString & strLauncherURL, HTTPManager* HttpMgr);
	void GetWorkingFolderPath( OUT CString& strPath );
#else
	virtual bool IsLoginDlg() { return false; }
#endif

	virtual bool GetNewCmdLine( IN const char *szCmd, OUT char *szNewCmd, IN int iNewCmdSize );
	virtual bool GetLoginResultPostData( OUT char *szError, IN int iErrorSize, const char *szURL, const char *szID, const char *szPW );
	virtual int  GetLoginKeyLiveTime();
	virtual int  GetOptimizeBtnWidth() { return 444; }
	virtual int  GetOptimizeBtnX() { return 58; }

	virtual void SetLoginData( const char *szLoginID, const char *szLoginEncodePW );
	virtual void SetServerID( const char *szServerID );
	
	virtual bool IsUseSocket() { return true; }
	virtual bool IsRightID( const char *szID );
	virtual bool IsRightPW( const char *szPW );
	virtual bool IsLoginURLDelete() { return true; }
	virtual bool IsAutoStart() { return false; }
	virtual bool IsLoginURLINI() { return false;}
	virtual bool IsPrivateLowerID() { return true; }
	virtual bool IsPieceFilePermit() { return true; }
	virtual bool IsUserType() { return true; }
	virtual bool IsFacebookLogin() { return true; }
	virtual bool IsAddLoginDelimiter() { return true; }
	
	virtual const char *GetConnectFailMent();
	virtual const char *GetLoginWrongIDMent();
	virtual const char *GetLoginWrongPWMent();
	virtual const char *GetServerFullMent();

	virtual const char *GetEntryURL();
	virtual const char *GetFindIDURL();
	virtual const char *GetFindPWURL();
	virtual const char *GetLoginDlgURL();


	virtual const char *GetLoginID() { return m_szLoginID; }
	virtual const char *GetLoginEncodePW() { return m_szLoginEncodePW; }

	virtual int GetMaxIDSize() { return 80; }
	virtual int GetMaxPWSize() { return 500; }

	virtual void SetUserType( const char *szUserType );
	virtual void FillLoginData( OUT SP2Packet &rkPacket );
	virtual void OnFacebookData( const string &rsEncryptData );

public:
	ioLocalUS(void);
	virtual ~ioLocalUS(void);
	enum STEAM_PROC
	{
		STEAM_FILE_CHECK = 0, STEAM_DOWN, STEAM_RUN, STEAM_FAIL
	};
};

#endif // __ioLocalUS_h__