#pragma once

#include "../io3DEngine/ioMovingWnd.h"

#include "FSMStateMgr.h"

class ioUIRenderImage;

class BossMonsterGaugeWnd : public ioMovingWnd
{
public:
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
	};

public:
	enum GaugeType
	{
		GAUGE_HALF_BOSS = 1,
		GAUGE_LAST_BOSS = 2,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};
	
protected:
	ioUIRenderImage *m_pIconBackOrange;
	ioUIRenderImage *m_pIconBackRed;
	ioUIRenderImage *m_pGaugeBack;
	ioUIRenderImage *m_pGaugeOrange;
	ioUIRenderImage *m_pGaugeRed;
	ioUIRenderImage *m_pMonsterIcon;

protected:
	FSMStateMgr m_ShowAniState;
	int m_iBlindOffSetY;
	int m_iPreBlindOffSetY;
	int m_iDefaultX;
	int m_iDefaultY;
	
	static bool m_bPosToDefualt;

protected:
	GaugeType m_eGaugeType;
	
	ioHashString m_szMonsterName;
	ioHashString m_szMonsterMent;
	float m_fMaxHP;
	float m_fCurHP;
	float m_fAniHP;
	float m_fCurAniTime;

	bool  m_bScreen;
	float m_fScreenTime;

protected:
	void InitShowAniState();

	void UpdateAniState();
	bool UpdateShowAni();
	void UpdateDefaultPos();
	void UpdateGaugeAni();
	
public:
	void InitAniState();
	void SetBlindOffSet( int iYOffSet );
	void ChangeShowAniState( ShowAniType eShowAniType );
	int  GetCurShowAniState();
	int GetMainDefaultXPos() { return m_iDefXPos; }
	int GetMainDefaultYPos() { return m_iDefYPos; }
	
	const ioHashString &GetMonsterName(){ return m_szMonsterName; }

public:
	void SetBossMonster( int iGaugeType, const ioHashString &rkMonsterName, const ioHashString &rkMonsterIcon, ioHashString &rkMonsterMent );
	void UpdateBossMonster( float fCurHP, float fMaxHP );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:	
	virtual void iwm_create();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	static bool GetPosToDefualt() { return m_bPosToDefualt; }
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefualt = PosToDefualt; }

public:
	BossMonsterGaugeWnd();
	virtual ~BossMonsterGaugeWnd();
};

