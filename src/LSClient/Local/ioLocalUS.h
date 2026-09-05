#pragma once

#include "ioLocalParent.h"

class ioLocalUS : public ioLocalParent
{
public:
	enum 
	{ 
		OGP_LM_LOGIN_OK      = 10000, 
		OGP_LM_LOGIN_FAIL    = 10001, 
		OGP_LM_LOGIN_CLOSE   = 10010, 
		OGP_LM_BROWSER_ASTRO = 10021,
	};
protected:
	ioHashString m_szBillingUserKey;

	static bool m_bEU;
	
public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetTextListFileName();
	virtual const char *GetMemTextList();

	virtual DWORD GetResourceIDTitleCashBmp();
	virtual DWORD GetResourceIDTitleEntryBmp();
	virtual DWORD GetResourceIDTitleGuildBmp();
	virtual DWORD GetResourceIDTitleGuildMarkBmp();
	virtual DWORD GetResourceIDTitleHomeBmp();
	virtual DWORD GetResourceIDTitleBmp();

	virtual DWORD GetResourceIDErrorBack();
	virtual DWORD GetResourceIDErrorExitC();
	virtual DWORD GetResourceIDErrorExitN();
	virtual DWORD GetResourceIDErrorExitO();
	virtual DWORD GetResourceIDErrorSolutionC();
	virtual DWORD GetResourceIDErrorSolutionN();
	virtual DWORD GetResourceIDErrorSolutionO();

	virtual void SetLoginData( const ioHashString &szBillingUserKey );
	virtual void FillLoginData( OUT SP2Packet &rkPacket );

	virtual void SendWindowMsg( int iMsgType );

	virtual void GetClassTypeForTutorialDropItem( int &riClassType );

	virtual const char *GetGuildMasterPostion();
	virtual const char *GetGuildSecondMasterPosition();
	virtual const char *GetGuildGeneralPosition();
	virtual const char *GetGuildAdminPosition();
	virtual const char *GetErrorSolutionURL();
	virtual const char *GetBillingErrorMent(ioHashString &sServerError);

	virtual int  GetFirstIDMaxSize();
	virtual bool IsShowFirstIDWnd();

	virtual void OpenFillCashPage();

	virtual bool IsRightNewID( const char *szID );

	virtual void SetTimeZone();

	virtual bool IsSendGashaponList();

	virtual void GetChangedString( OUT char *szCheckString );

	virtual bool IsExecuteLuncher();
	virtual bool IsKoreaGameLimit();
	virtual bool IsCheckKorean();
	virtual bool IsMyHomePage();
	virtual bool IsShowExtraGoodsList();
	virtual bool IsShowRegulation();

	virtual bool SetPresentRecvDisableMsg( int iPresentType );

    virtual int GetLimitGradeLevel();

	virtual int GetLicenseDate();
	virtual bool IsCheckValueServer() { return true; }

public:
	static void SetEU( bool bEU ) { m_bEU = bEU; }
	static bool IsEU() { return m_bEU; }

public:
	ioLocalUS(void);
	virtual ~ioLocalUS(void);
};

