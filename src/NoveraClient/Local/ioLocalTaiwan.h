#pragma once

#include "ioLocalParent.h"

class ioLocalTaiwan  : public ioLocalParent
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

	virtual const char *GetGuildMasterPostion();
	virtual const char *GetGuildSecondMasterPosition();
	virtual const char *GetGuildGeneralPosition();
	virtual const char *GetGuildAdminPosition();
	virtual const char *GetBillingErrorMent(ioHashString &sServerError);

	virtual bool IsShowFirstIDWnd();

	virtual bool IsSendGashaponList();

	virtual void GetChangedString( OUT char *szCheckString );

	virtual bool IsCheckKorean();
	virtual bool IsFirstIDCheckPass() { return true; }
	virtual bool IsMyHomePage();
	virtual bool IsShowRegulation();
	virtual bool IsFirstIDControlAltCtrl() { return false; }
	virtual DWORD GetUserSyncIntervalTime() { return 1000; }

public:
	ioLocalTaiwan(void);
	virtual ~ioLocalTaiwan(void);
};

