#pragma once

#include "../io3DEngine/ioButton.h"
#include "../CharacterInfo.h"

#include "NewShopBuyWnd.h"

class ioUI3DRender;
class ioCreateChar;
class ioUIRenderImage;
class ManualMgr;

//////////////////////////////////////////////////////////////////////////
class PreSetSoldierPackageWnd : public ioWnd
{
public:
	enum
	{
		ID_BUY			= 1,
		ID_SOLDIER_WND  = 1000,
	};

protected:
	CEncrypt<DWORD> m_dwEtcItemType;
	ManualMgr* m_pManual;
	
	CHARACTER m_CharInfo;

protected:		
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	virtual void iwm_create();
	virtual void iwm_hide();

	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void ShowPackage( DWORD dwType );

protected:
	//virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	PreSetSoldierPackageWnd();
	virtual ~PreSetSoldierPackageWnd();
};

//////////////////////////////////////////////////////////////////////////
class  PreSetSoldierPackageResultWnd : public ioWnd
{
public:
	enum
	{
		ID_GO			= 1,
		ID_SOLDIER_WND  = 1000,
	};

protected:
	CHARACTER m_CharInfo;
	int m_iLimitTime;

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetResultSoldierPackage( CHARACTER &rkCharInfo, int iKeepBonusPeso, int iLimitTime );

protected:
	virtual void OnRender();

public:
	PreSetSoldierPackageResultWnd();
	virtual ~PreSetSoldierPackageResultWnd();
};


