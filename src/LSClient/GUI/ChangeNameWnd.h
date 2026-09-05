#pragma once

#include "../io3DEngine/ioButton.h"
#include "../ioComplexStringPrinter.h"

#include "NewShopBuyWnd.h"

class ManualMgr;
class ioUIRenderFrame;

////////////////////////////////////////////////////////////////////////////////////////////////////

class ChangeNameWnd : public ioWnd
{
public: 
	enum 
	{
		ID_ICON			= 1,
		ID_CHANGE_EDIT	= 2,
		ID_CHANGE_BTN	= 3,
	};

protected:
	DWORD m_dwEtcType;
	ioComplexStringPrinter m_Info[2];

	ioWnd *m_pPreEdit;
	ioWnd *m_pChangeEdit;

	ioHashString m_szTitle;
	ioHashString m_szCurName;
	ioHashString m_szNewName;
	ioHashString m_szDefaultNotice;
	ioHashString m_szErrorNotice;

	ioUIRenderFrame *m_pEditFrm;

	ManualMgr* m_pManual;

	bool m_bRenderNotice;

protected:
	void ChangeName();
	void SendNewName();

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void ShowChangeIDWnd( DWORD dwEtcType );
	void OnChangeResult();

	void ErrorEdit( const char *szError );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ChangeNameWnd(void);
	virtual ~ChangeNameWnd(void);
};

//////////////////////////////////////////////////////////////////////////
class ChangeNameResultWnd : public ioWnd
{
public:
	enum 
	{
		ID_GO	= 1,
	};

protected:
	DWORD m_dwEtcType;

	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pBackEffect;

	ioHashString m_szPreName;
	ioHashString m_szNewName;

public:
	virtual void iwm_command( ioWnd *pWnd , int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetInfoAndShow( const ioHashString &szNewName );
	void SetInfoAndShow( const ioHashString &szPreName, const ioHashString &szNewName );

public:
	ChangeNameResultWnd(void);
	virtual ~ChangeNameResultWnd(void);
};

