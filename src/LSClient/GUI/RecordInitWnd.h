#pragma once

#include "../io3DEngine/ioButton.h"
#include "../ioComplexStringPrinter.h"

#include "NewShopBuyWnd.h"

class ManualMgr;
class ioUIRenderFrame;

////////////////////////////////////////////////////////////////////////////////////////////////////

class RecordInitWnd : public ioWnd
{
public: 
	enum 
	{
		ID_ICON			= 1,
		ID_INIT_EDIT	= 2,
		ID_INIT_BTN		= 3,
	};

protected:
	DWORD m_dwEtcType;

	ioWnd *m_pPreEdit;
	ioWnd *m_pInitEdit;

	ioHashString m_szTitle;

	ioUIRenderFrame *m_pEditFrm;

	ManualMgr* m_pManual;

protected:
	void SendRecordInit();
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void ShowRecordInitWnd( DWORD dwEtcType );
	
public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	RecordInitWnd(void);
	virtual ~RecordInitWnd(void);
};

//////////////////////////////////////////////////////////////////////////
class RecordInitResultWnd : public ioWnd
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

public:
	virtual void iwm_command( ioWnd *pWnd , int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetInfoAndShow( DWORD dwEtcType );

public:
	RecordInitResultWnd(void);
	virtual ~RecordInitResultWnd(void);
};

