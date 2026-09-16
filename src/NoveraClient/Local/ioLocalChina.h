#pragma once

#include "ioLocalParent.h"

class ioLocalChina : public ioLocalParent
{
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
	virtual bool IsMyHomePage();
	virtual bool IsShowRegulation();

	virtual const char* GetFontFileName();

public:
	ioLocalChina(void);
	virtual ~ioLocalChina(void);
};

