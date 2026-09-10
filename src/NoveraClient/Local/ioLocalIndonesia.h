#pragma once

#include "ioLocalParent.h"

class ioLocalIndonesia  : public ioLocalParent
{
protected:
	ioHashString m_sLoginEncodePW;

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
	virtual bool IsStartFullScreen(){ return true; }
	virtual bool IsTooltipAutoLine(){ return true; }

	virtual bool SetPresentRecvDisableMsg( int iPresentType );

	virtual int GetLimitGradeLevel();

	virtual int GetLicenseDate();

	virtual DWORD GetCashSendTime() { return 3600000; } // ms
	virtual bool IsCheckValueServer() { return true; }
	virtual bool IsChangeServerHelp() { return true; }
	virtual bool IsSendCollisionPos() { return false; }
	virtual bool IsLimitStartCancel() { return true; }

	virtual int GetCashPageWidth();
	virtual int GetCashPageHeight();
	virtual void OnCashPageHide();

public:
	ioLocalIndonesia(void);
	virtual ~ioLocalIndonesia(void);
};

