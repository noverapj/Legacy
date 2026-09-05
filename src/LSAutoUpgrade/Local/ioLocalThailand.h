#ifndef __ioLocalThailand_h__
#define __ioLocalThailand_h__

#include "ioLocalParent.h"
#include "..\StringManager\ioHashString.h"


class HTTPManager;

class ioLocalThailand  : public ioLocalParent
{
public:
	typedef struct DomainInfo
	{
		ioHashString m_sDomainTitle;
		ioHashString m_sDomainToken;

		DomainInfo()
		{
			m_sDomainTitle.Clear();
			m_sDomainToken.Clear();
		}
	};

	typedef std::vector< DomainInfo* > vDomainInfo;
	typedef std::vector< ioHashString > vHashString;

protected:
	char m_szRegKey[MAX_PATH];
	char m_szStartURL[MAX_PATH*2];
	char m_szLoginID[MAX_PATH*2];
	char m_szLoginEncodePW[MAX_PATH];
	char m_szOTPID[MAX_PATH];
	char m_szOTPEncodePW[MAX_PATH*2];

	vDomainInfo  m_vDomainInfo;
	ioHashString m_szLoginTokenKey;


public:
	virtual void SetLoginTokenKey( ioHashString szToken );

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

	virtual DWORD GetResourceIDLoginEnterBtnBmp() { return IDB_LOGIN_ENTER_BTN_TH; }
	virtual DWORD GetResourceIDLoginEntryBtnBmp() { return IDB_LOGIN_ENTRY_BTN_TH; }
	virtual DWORD GetResourceIDLoginFindIDBtnBmp() { return IDB_LOGIN_FINDID_BTN_TH; }
	virtual DWORD GetResourceIDLoginFindPWBtnBmp() { return IDB_LOGIN_FINDPW_BTN_TH; }

	virtual const char *GetRegKey();
	virtual const char *GetFontName();
	virtual const char *GetLoginFontName();
	virtual const char *GetErrorSolutionURL();
	virtual const char *GetLoginID();
	virtual const char *GetOTPID();

	virtual void  CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL );

    virtual bool IsLoginDlg() { return true; }

	virtual bool GetNewCmdLine( IN const char *szCmd, OUT char *szNewCmd, IN int iNewCmdSize );
	virtual bool GetLoginResultPostData( OUT char *szError, IN int iErrorSize, const char *szURL, const char *szID, const char *szPW );
	virtual int  GetLoginKeyLiveTime();
	virtual int  GetOptimizeBtnWidth() { return 444; }
	virtual int  GetOptimizeBtnX() { return 58; }

	virtual void SetLoginData( const char *szLoginID, const char *szLoginEncodePW );
	virtual void SetOTPID( const char *szOTPID );
	virtual void SetOTPEncodePW( const char *szOTPEncodePW );
	
	virtual bool IsUseSocket() { return true; }
	virtual bool IsRightID( const char *szID );
	virtual bool IsRightPW( const char *szPW );
	virtual bool IsRightOTP( const char *szOTP );
	virtual bool IsLoginURLDelete() { return true; }
	virtual bool IsAutoStart() { return false; }
	virtual bool IsLoginURLINI() { return false;}
	virtual bool IsPrivateLowerID() { return true; }
	virtual bool IsPieceFilePermit() { return true; }
	virtual bool IsOTP();
	
	virtual const char *GetConnectFailMent();
	virtual const char *GetLoginWrongIDMent();
	virtual const char *GetLoginWrongPWMent();

	virtual const char *GetEntryURL();
	virtual const char *GetFindIDURL();
	virtual const char *GetFindPWURL();

	virtual int GetMaxIDSize() { return 24; }
	virtual int GetMaxPWSize() { return 24; }
	virtual int GetMaxOPTSize() { return 6; }

	virtual void SetDomainList();
	virtual int  GetMaxDomain();
	virtual void GetDomainTitle( IN int iArray , OUT ioHashString &rsTitle );
	virtual void GetDomainToken( IN int iArray , OUT ioHashString &rsToken );
	bool DownloadStartIni( const CString & strStartINIURL, HTTPManager* pHttpMgr );

public:
	ioLocalThailand(void);
	virtual ~ioLocalThailand(void);
};

#endif // __ioLocalThailand_h__