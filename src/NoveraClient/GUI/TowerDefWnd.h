#pragma once

class TowerDefUserGaugeWnd;
class ioSlotItem;

class TowerDef3DChar
{
public:
	enum
	{
		VIEW_WIDTH = 35,
		VIEW_HEIGHT= 35,
	};

private:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_pAvata;

protected:
	int m_iOffsetXPos;
	int m_iOffsetYPos;

public:
	void Initialize();
	void SetChar( ioBaseChar *pChar);
	void FaceChange( const char *szTexPostfix, int iDuration );
	void SetHelmChange( ioBaseChar *pChar );

	void Update( float fTimePerSec );
	void Render( int iXPos, int iYPos );

public:
	TowerDef3DChar();
	~TowerDef3DChar();
};


class TowerDefWnd : public ioMovingWnd
{
public:
	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  // 啊加
		SAT_DOWN         = 4,  // 皑加
		SAT_UP_STOP      = 5,  // 啊加
		SAT_STOP         = 6,
		SAT_END          = 7,
	};

	enum eGaugeWndID 
	{
		GAUGE_NONE = 0,
		GAUGE_WND1 = 100,
		GAUGE_WND2 = 101,
		GAUGE_WND3 = 102,
		GAUGE_WND4 = 103,
		MAX_USER = 4,

		NPC_GAUGE_WND = 104,
		NPC_BOSS_WND = 105,
		
		END_GAUGE = MAX_USER + 2,

		SLOT_WND = 106,
	};

	enum eSlot
	{
		SLOT_1 =0,
		SLOT_2,
		SLOT_3,
		SLOT_4,
		MAX_SLOT,
	};

public:
	enum GaugeType
	{
		GAUGE_USER = 0,
		GAUGE_NPC, 
		GAUGE_NPC_BOSS,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

	struct stUserGauge
	{
		eGaugeWndID eID;
		ioHashString szUserName;
		TowerDefUserGaugeWnd* pGaugeWnd;
	};

protected:
	FSMStateMgr m_ShowAniState;
	int m_iDefaultX;
	int m_iDefaultY;
	stUserGauge m_stUserGauge[MAX_USER];
	ioHashString m_szNpcName;

public:
	void GetUserNameList(ioHashStringVec &vecName);
	void SetFaceChange( const ioHashString &rkName, const char *szTexPostfix, int iDuration );
	void SetCharChange( ioBaseChar *pChar );
	void SetHelmChange( ioBaseChar *pChar );
	void ResetAllGauge();
	void ResetNpcGauge();
	void HideWnd();

public:
	void ChangeShowAniState( ShowAniType eShowAniType );
	void SetUser( ioBaseChar *pChar, int nInstantLv = 0 );
	void SetNPC(ioNpcChar* pNpc, int nGradeLv = 0, bool nBoss = false);
	void SetSurrender(const ioHashString &szName, bool bSurrender);
	
	const ioHashString &GetNpcName();

public:
	void UpdateGauge(const ioHashString &rkName, float fCurHP, float fMaxHP, int nInstantLv = 0 );
	void UpdateNpcGauge(const ioHashString &rkName, float fCurHP, float fMaxHP, int nInstantLv);
	void UpdateSlotItem(const int nSlot);
	void InitSlotItem();

	
public:
	TowerDefWnd();
	virtual ~TowerDefWnd();
};


class TowerDefUserGaugeWnd : public ioMovingWnd
{
public:
	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  // 啊加
		SAT_DOWN         = 4,  // 皑加
		SAT_UP_STOP      = 5,  // 啊加
		SAT_STOP         = 6,
		SAT_END          = 7,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

	enum 
	{
		IMG_POTRAIT_BACK = 0,
		IMG_POTRAIT_OVERBACK,
		IMG_POTRAIT_MAX,
	};

protected:
	ioUIRenderImage *m_pPotrait[IMG_POTRAIT_MAX];
	ioUIRenderImage *m_pGauge;
	ioUIRenderImage *m_pNpcFace;
	ioUIRenderImage *m_pSurrender;

	TowerDef3DChar *m_pCharPotrait;

	ioUIRenderFrame *m_pGaugeFrame;
	ioUIRenderFrame *m_pGaugeFrameOver;



protected:
	FSMStateMgr m_ShowAniState;
	int m_iBlindOffSetY;
	int m_iPreBlindOffSetY;
	int m_iDefaultX;
	int m_iDefaultY;

	static bool m_bPosToDefault;
	TowerDefWnd::GaugeType m_eGaugeType;

protected:
	
	ioHashString m_szMonsterMent;

	float m_fMaxHP;
	float m_fCurHP;
	float m_fAniHP;
	float m_fScreenTime;
	float m_fCurAniTime;
	float m_fSurrenderTime;

	bool m_bScreen;
	bool m_bSurrender;
	

protected:
	ioHashString m_szUserName;
	ioHashString m_szShowName;
	int m_nLevel;
		
public:
	void SetFaceChange( const ioHashString &rkName, const char *szTexPostfix, int iDuration );
	void SetCharChange( ioBaseChar *pChar );
	void SetHelmChange( ioBaseChar *pChar );
	void SetSurrender(bool bSurrender) {m_bSurrender = bSurrender; m_fSurrenderTime = (float)FRAMEGETTIME();}

protected:
	void InitShowAniState();

	void UpdateAniState();
	bool UpdateShowAni();
	void UpdateDefaultPos();
	void UpdateGaugeAni();

	void OnRenderUser(int iXPos, int iYPos);
	void OnRenderNPC(int iXPos, int iYPos);
	void OnRenderBoss(int iXPos, int iYPos);

	
public:
	void InitAniState();
	void SetBlindOffSet( int iYOffSet );
	void ChangeShowAniState( ShowAniType eShowAniType );
	void SetUser( ioBaseChar *pChar, int nInstantLv = 0, TowerDefWnd::GaugeType m_eGaugeType = TowerDefWnd::GAUGE_USER );
	int GetCurShowAniState();
	int GetMainDefaultXPos() { return m_iDefXPos; }
	int GetMainDefaultYPos() { return m_iDefYPos; }
	
	const ioHashString &GetName(){ return m_szUserName; }

public:
	void UpdateGauge( float fCurHP, float fMaxHP, int nInstantLv = 0 );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnRender();
//	virtual void OnRenderTitle( int iXPos, int iYPos );
	virtual void OnProcess( float fTimePerSec );

public:	
	virtual void iwm_create();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	static bool GetPosToDefualt() { return m_bPosToDefault; }
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefault = PosToDefualt; }

public:
	TowerDefUserGaugeWnd();
	virtual ~TowerDefUserGaugeWnd();
};

//////////////////////////////////////////////////////////////////////////
class ConsumptionWnd : public ioMovingWnd
{
public:
	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  // 啊加
		SAT_DOWN         = 4,  // 皑加
		SAT_UP_STOP      = 5,  // 啊加
		SAT_STOP         = 6,
		SAT_END          = 7,
	};

	enum eSlot
	{
		SLOT_1 =0,
		SLOT_2,
		SLOT_3,
		SLOT_4,
		MAX_SLOT,
	};


protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};


protected:
	FSMStateMgr m_ShowAniState;
	int m_iBlindOffSetY;
	int m_iPreBlindOffSetY;
	int m_iDefaultX;
	int m_iDefaultY;

	static bool m_bPosToDefault;

protected:
	void InitShowAniState();

	void UpdateAniState();
	bool UpdateShowAni();
	void UpdateDefaultPos();


public:
	void InitAniState();
	void SetBlindOffSet( int iYOffSet );
	void ChangeShowAniState( ShowAniType eShowAniType );
	int GetCurShowAniState();
	int GetMainDefaultXPos() { return m_iDefXPos; }
	int GetMainDefaultYPos() { return m_iDefYPos; }

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:	
	virtual void iwm_create();
	virtual void iwm_command(ioWnd *pWnd, int cmd, DWORD param);
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	static bool GetPosToDefualt() { return m_bPosToDefault; }
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefault = PosToDefualt; }

public:
	void SetSlotWnd();

public:
	ConsumptionWnd();
	virtual ~ConsumptionWnd();
};




//////////////////////////////////////////////////////////////////////////
class ConsumptionSlotBtn : public ioButton
{
protected:
	ioUIRenderImage*	m_pEmptySlotImg;
	ioUIRenderImage*	m_pItemIcon;
	ioUIRenderImage*	m_pItemBackImg;

	ioHashString		m_strKey;

	int					m_nItem;

	DWORD				m_dwCoolTime;

	


public:
//	virtual void iwm_command(ioWnd *pWnd, int cmd, DWORD param);
	virtual void iwm_create();
	void InitItem(ioSlotItem* pItem);
	void UpdateItem(ioSlotItem* pItem);


protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	ConsumptionSlotBtn();
	virtual ~ConsumptionSlotBtn();
};