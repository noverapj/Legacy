#ifndef __ioLocalParent_h__
#define __ioLocalParent_h__

#include "ioLocalManager.h"
#include <strsafe.h>
#include "..\resource.h"
#include "..\NetWork\SP2Packet.h"
#include "..\StringManager\ioHashString.h"

class ioHashString;

class ioLocalParent
{
public:
	enum 
	{
		LOGIN_KEY_LIVE_TIME = 1800000,
	};

	enum
	{ 
		SUB_TYPE_EN = 0,
		SUB_TYPE_DE,
		SUB_TYPE_FR,
		SUB_TYPE_IT,
		SUB_TYPE_PL,
		SUB_TYPE_TR,
	};

	int m_nSubType;

public:
	virtual ioLocalManager::LocalType GetType() = 0;
	virtual int GetSubType() {return m_nSubType;}
	virtual void SetSubType(int nSubType) {m_nSubType = nSubType;}
	virtual const char *GetTextListFileName() = 0;
	virtual const char *GetMemTextList() = 0;
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 ) = 0;
	
	virtual DWORD GetResourceIDBackBmp() = 0;
	virtual DWORD GetResourceIDOptimizeBtnBmp() = 0;
	virtual DWORD GetResourceIDStartBtnBmp() = 0;
	virtual DWORD GetResourceIDErrorBackBtnBmp() = 0;
	virtual DWORD GetResourceIDErrorSolutionBtnBmp() = 0;
	virtual DWORD GetResourceIDErrorExitBtnBmp() = 0;

	virtual DWORD GetResourceIDLoginBackBmp() { return IDB_LOGIN; }
	virtual DWORD GetResourceIDLoginEnterBtnBmp() { return IDB_LOGIN_ENTER_BTN; }
	virtual DWORD GetResourceIDLoginEntryBtnBmp() { return IDB_LOGIN_ENTRY_BTN; }
	virtual DWORD GetResourceIDLoginFindIDBtnBmp() { return IDB_LOGIN_FINDID_BTN; }
	virtual DWORD GetResourceIDLoginFindPWBtnBmp() { return IDB_LOGIN_FINDPW_BTN; }

	virtual const char *GetRegKey() = 0;
	virtual const char *GetFontName() = 0;
	virtual const char *GetLoginFontName(){ return "verdana"; }
	virtual const char *GetErrorSolutionURL() = 0;
	virtual const char *GetLoginID() { return ""; }
	virtual const char *GetLoginEncodePW() { return ""; }
	virtual const char *GetOTPID() { return ""; }

	virtual void  CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL ) = 0;

	virtual bool IsLoginDlg() { return false; }
	virtual bool IsFine() { return false; }

	virtual bool GetNewCmdLine( IN const char *szCmd, OUT char *szNewCmd, IN int iNewCmdSize ) {  StringCbPrintf( szNewCmd, iNewCmdSize, " %s", szCmd); return true; };
	virtual bool GetLoginResultPostData( OUT char *szError, IN int iErrorSize, const char *szURL, const char *szID, const char *szPW ) { return true;};
	virtual int  GetLoginKeyLiveTime() { return LOGIN_KEY_LIVE_TIME; }
	virtual int  GetOptimizeBtnWidth() { return 271; }
	virtual int  GetOptimizeBtnX() { return 115; }

	virtual void SetLoginData( const char *szLoginID, const char *szLoginEncodePW ) {}
	virtual void SetServerID( const char *szServerID ) {}
	virtual void SetOTPID( const char *szOTPID ) {}
	virtual void SetOTPEncodePW( const char *szOTPEncodePW ) {}
	
	virtual bool IsUseSocket() { return false; }
	virtual bool IsRightID( const char *szID ) { return true; }
	virtual bool IsRightPW( const char *szPW ) { return true; }
	virtual bool IsRightOTP( const char *szOTP ) { return true; }
	virtual bool IsLoginURLDelete() { return false; }
	virtual bool IsAutoStart() { return true; }
	virtual bool IsLoginURLINI() { return false;}
	virtual bool IsPrivateLowerID() { return false; }
	virtual bool IsPieceFilePermit() { return false; }
	virtual bool IsDomain() { return false; }
	virtual bool IsAddLoginDelimiter() { return false; }
	virtual bool IsOTP() { return false; }
	virtual bool IsFacebookLogin() { return false; }

	virtual const char *GetConnectFailMent()  { return ""; }
	virtual const char *GetLoginWrongIDMent() { return ""; }
	virtual const char *GetLoginWrongPWMent() { return ""; }
	virtual const char *GetServerFullMent() { return ""; }

	virtual const char *GetEntryURL()  { return ""; }
	virtual const char *GetFindIDURL() { return ""; }
	virtual const char *GetFindPWURL() { return ""; }
	virtual const char *GetLoginDlgURL() { return ""; }
	virtual int GetMaxIDSize() { return 12; }
	virtual int GetMaxPWSize() { return 12; }
	virtual int GetMaxOPTSize() { return 6; }

	virtual void SetDomainList(){}
	virtual int  GetMaxDomain() { return 0;}
	virtual void GetDomainTitle( IN int iArray , OUT ioHashString &rsTitle ) {}
	virtual void GetDomainToken( IN int iArray , OUT ioHashString &rsToken ) {}

	virtual void SetUserType( const char *szUserType ){}
	virtual void FillLoginData( OUT SP2Packet &rkPacket ){}
	virtual void OnFacebookData( const string &rsEncryptData ){}
	virtual char* GetNexonPasspot() { return ""; }
	virtual void RegisterCallBack(HWND nHwnd){}

	virtual void SetLoginTokenKey( ioHashString szToken ){}
public:
	void EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN bool bPassword );

public:
	ioLocalParent(void);
	virtual ~ioLocalParent(void);
};

#endif // __ioLocalParent_h__
