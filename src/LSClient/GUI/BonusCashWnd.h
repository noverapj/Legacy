#pragma once

#include "../ioBonusCashManager.h"
class ioComplexStringPrinter;
class BonusCashWnd : public ioWnd
{
private:
	enum
	{
		ID_EXIT					= 1,
		ID_PROGRESS_BTN			= 2,
		ID_EXPRIATION_BTN		= 3,
		ID_PROGRESS_INFO_WND	= 4,
		ID_EXPRIATION_INFO_WND	= 5,
	};

	DWORD		m_dwCurButtonID;
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void OnRender();
public:
	BonusCashWnd(void);
	virtual ~BonusCashWnd(void);
};

////////////////////////////////////////////////////////////////////////////
class BonusCashBtn : public ioWnd
{
private:
	enum
	{
		MAX_TEXT_INFO = 3,
	};
private:
	bool				   m_bData;
	ioComplexStringPrinter m_CashInfoList[MAX_TEXT_INFO];
public:
	bool SetInfo( const ioBonusCashManager::BonusCashInfo& rkInfo, bool bProgress );
protected:
	virtual void OnRender();
public:
	BonusCashBtn(void);
	virtual ~BonusCashBtn(void);
};

////////////////////////////////////////////////////////////////////////////
class BonusCashProgressWnd : public ioWnd
{
protected:
	enum
	{
		ID_BONUSCASH_INFO01		= 1,
		ID_BONUSCASH_INFO10		= 10,
		ID_VERT_SCROLL			= 1000,
	};
private:
	int		m_iCurPos;
public:
	virtual void iwm_show();
	virtual void OnRender();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );
public:
	BonusCashProgressWnd(void);
	virtual ~BonusCashProgressWnd(void);
};

////////////////////////////////////////////////////////////////////////////
class BonusCashExpireWnd : public ioWnd
{
protected:
	enum
	{
		ID_BONUSCASH_INFO01		= 1,
		ID_BONUSCASH_INFO10		= 10,
		ID_VERT_SCROLL			= 1000,
	};
private:
	int		m_iCurPos;
public:
	virtual void iwm_show();
	virtual void OnRender();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );
public:
	BonusCashExpireWnd(void);
	virtual ~BonusCashExpireWnd(void);
};