#pragma once

class PlazaNPCGaugeWnd : public ioMovingWnd
{
public:
	enum GaugeType
	{
		GAUGE_NORMAL_NPC = 0,
		GAUGE_AWAKENING_NPC,
		MAX_GAUGE,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

protected:
	ioUIRenderImage *m_pIconBack[MAX_GAUGE];
	ioUIRenderImage *m_pGauge[MAX_GAUGE];
	ioUIRenderImage *m_pGaugeBack;
	ioUIRenderImage *m_pGaugeWhite;
	ioUIRenderImage *m_pAttackEffect1;
	ioUIRenderImage *m_pAttackEffect2;
	ioUIRenderImage *m_pWhiteLine;
	ioUIRenderImage *m_pMonsterIcon;
	ioUIRenderImage *m_pNpcImgName;

protected:
	ioUIRenderImage *m_pRaidGauge;
	ioUIRenderImage *m_pRaidGaugeBack;
	ioUIRenderImage *m_pRaidGaugeWhite;

protected:
	int m_iBlindOffSetY;
	int m_iPreBlindOffSetY;
	int m_iDefaultX;
	int m_iDefaultY;
	int m_nVibY;

	POINT m_stEffectPos;

	static bool m_bPosToDefualt;

protected:
	GaugeType m_eGaugeType;

	ioHashString m_szMonsterName;

	DWORD	m_dwTimer;

	float m_fMaxHP;
	float m_fCurHP;
	float m_fAniHP;
	float m_fCurAniTime;

	bool  m_bScreen;
	float m_fScreenTime;
	float m_fBlinkTime;

protected:
	IORandom m_IORandom;

protected:
	ioAnimationState*	m_pAniState;

protected:
	bool m_bIsRaidMode;
	int m_nCurRank;
	int m_nTotalPlayer;

protected:
	void InitShowAniState();

	void UpdateAniState();
	bool UpdateShowAni();
	void UpdateDefaultPos();
	void UpdateGaugeAni();

public:
	void InitAniState();
	void SetBlindOffSet( int iYOffSet );
	void ChangeShowAniState( ioAnimationState::ShowAniType eShowAniType );
	int  GetCurShowAniState();
	int GetMainDefaultXPos() { return m_iDefXPos; }
	int GetMainDefaultYPos() { return m_iDefYPos; }

	const ioHashString &GetMonsterName(){ return m_szMonsterName; }

public:
	void SetBossMonster( int iGaugeType, const ioHashString &rkMonsterName, const ioHashString &rkMonsterIcon, const ioHashString &rkMonsterMent, DWORD dwAliveTime );
	void UpdateBossMonster( float fCurHP, float fMaxHP );

public:
	void SetRaidBossMonster( int iGaugeType, const ioHashString &rkMonsterName, const ioHashString &rkMonsterIcon, const ioHashString &rkMonsterMent );
	void UpdateRaidBossMonster( float fCurHP, float fMaxHP, int nCurRank, int nTotalPlayer );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

protected:
	void RenderBossMonster( int iXPos, int iYPos );
	void RenderRaidBossMonster( int iXPos, int iYPos );

public:	
	virtual void iwm_create();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	static bool GetPosToDefualt() { return m_bPosToDefualt; }
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefualt = PosToDefualt; }

public:
	PlazaNPCGaugeWnd(void);
	virtual ~PlazaNPCGaugeWnd(void);

};

