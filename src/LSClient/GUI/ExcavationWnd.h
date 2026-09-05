#pragma once

#include "FSMStateMgr.h"

class ExcavationWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_RECHARGE = 2,
		ID_STOP = 3
	};

	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  // 가속
		SAT_DOWN         = 4,  // 감속
		SAT_UP_STOP      = 5,  // 가속
		SAT_STOP         = 6,
		SAT_END          = 7,

		Y_OFFSET = 205,
		EFFECT_TIME = 1000,
	};

	enum
	{
		TEXT_LEVEL = 0,
		TEXT_EXCAVATION_RECHARGE,
		TEXT_EXCAVATION_READY,
		TEXT_EXCAVATION_COUNT,
		TEXT_MAX,
	};
private:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconGray;
	ioUIRenderImage *m_pHotKeyPushed;
	ioUIRenderImage *m_pHotKeyReleased;
	ioUIRenderImage *m_pGauge;
	ioUIRenderImage *m_pShovelEnable;
	ioUIRenderImage *m_pShovelDisable;
	ioUIRenderImage *m_IconBack;
	//ioUIRenderImage *m_pShovelEffect;

	ioHashString m_szText[TEXT_MAX];

	ioHashString m_szRechargeSound;

	FSMStateMgr m_ShowAniState;
	int m_iDefaultX;
	int m_iDefaultY;

	int m_iCurShovel;

	DWORD m_dwScaleStartTime;
	float m_fCurScaleRate;
	float m_fGaugeRate;
	DWORD m_dwRemainTime;

	bool m_bProcessDelay;
	bool m_bLevelUpDelay;

	//bool m_bShowEffect;
	//DWORD m_dwEffectTime;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false */ );

	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();

private:
	void InitShowAniState();
	bool UpdateShowAni();

public:
	//void SetShovelEffect();
	void ChangeUseIcon();

	void SetProcessDelay( bool bDelay );
	bool IsProcessDelay()					{ return m_bProcessDelay; }
	void SetLevelUpDelay( bool bDelay );
	bool IsLevelUpDelay()					{ return m_bLevelUpDelay; }

public:
	ExcavationWnd();
	virtual ~ExcavationWnd();
};
