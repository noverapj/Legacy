#pragma once

#define ICON_WIDTH              54
#define ICON_HEIGHT             54

class AccessorySkillListWnd;
class AccessorySkillListBtn : public ioButton
{
public:
	enum SkillEffectDrawType
	{
		SDT_NONE               = 0,
		SDT_CAN                = 1,
		SDT_WAIT               = 2,
		SDT_USING              = 3,
		SDT_EFFECT_WHITE       = 4,
		SDT_EFFECT_WHITE_ITEM  = 5,
		SDT_EFFECT_NORMAL      = 6,
		SDT_EFFECT_NORMAL_ITEM = 7, 
		SDT_EFFECT_END         = 8,
		SDT_NO_SKILL		   = 9,
	};

	enum KeyEffectType
	{
		KET_NONE         = 0,
		KET_CAN_SKILL    = 1,
		KET_EFFECT_ALPHA = 2,
		KET_EFFECT_NONE  = 3,
	};

	enum DrawActiveCountType
	{
		DACT_ONE_TEXT,
		DACT_TWO_TEXT,
		DACT_CONSECUTION3,
	};

protected:
	struct SlotInfo
	{
		ioUIRenderImage *pBackImg;

		ioUIRenderImage *pItemImg;
		ioUIRenderImage *pSkillImg;
		ioUIRenderImage *pTextImg;

		ioHashString m_ItemIconName;
		ioHashString m_SkillIconName;
		ioHashString m_SkillTextImage;

		ioHashString m_SkillDesc;
		int m_iGrowthLevel;
		int m_iReinforce;
		int m_iMedalLevel;
		int m_nPetLevel;
		int m_nAwakeLevel;
		int m_iRCharLevel;
		int m_iSoldierSetLevel;

		bool bSlotGray;
		bool bDisable;
		bool bUseScale;
		bool bUseBuff;
		bool bUseBuffButStopped;

		int  iSkillType;
		bool bOnOffEnable;
		bool bEnableUpdateGauge;

		float m_fScaleRate;
		DWORD m_dwScaleTime;

		float m_fCurScaleRate;
		DWORD m_dwScaleStartTime;

		int m_iMaxGauge;
		int m_iCurGauge;
		int m_iExtendMaxGauge;
		bool m_bUseGauge;

		SlotInfo()
		{
			pItemImg  = NULL;
			pSkillImg = NULL;
			pBackImg  = NULL;
			pTextImg = NULL;

			bSlotGray    = false;
			bDisable     = false;
			bUseScale    = false;
			bUseBuff     = false;
			bUseBuffButStopped = false;

			iSkillType   = 0;
			bOnOffEnable = false;
			bEnableUpdateGauge = false;

			m_fScaleRate  = 0.2f;
			m_dwScaleTime = 300;

			m_fCurScaleRate = FLOAT1;
			m_dwScaleStartTime = 0;

			m_iMaxGauge = 0;
			m_iCurGauge = 0;
			m_iExtendMaxGauge = 0;
			m_bUseGauge = false;

			m_iGrowthLevel = 0;
			m_iReinforce = 0;
			m_iMedalLevel = 0;
			m_nPetLevel = 0;
			m_nAwakeLevel = 0;
			m_iRCharLevel = 0;
			m_iSoldierSetLevel = 0;
		}

		bool IsNeedKeyRender() const;
		bool IsNeedBuffGaugeProtect() const;
		void ProcessScale();
	};
	SlotInfo m_SlotInfo;
protected:
	ioUIRenderImage *m_pGrowthNum;
	ioUIRenderImage *m_pGrowthPlus;

	ioUIRenderImage *m_pActiveCountBack;
	ioUIRenderImage *m_pActiveCountOver;
	ioUIRenderImage *m_pActiveCountOn;

	//T50메가개틀링건 관련 UI(2102. 06. 20 )
	ioUIRenderImage *m_pActiveCountOnYellow;
	ioUIRenderImage *m_pActiveCountOnRed;

	// 용병교체UI 관련 작업. ( 2012.02.27 )
	ioUIRenderFrame *m_pKeyBackNor;
	ioUIRenderFrame *m_pKeyBackOn;
	ioUIRenderFrame *m_pKeyBack;

	struct SkillKeyInfo
	{
		ioHashString	m_SkillKey;
		DWORD			m_TextColor;
		DWORD			m_BkColor;
		SkillKeyInfo()
		{
			m_SkillKey.Clear();
			m_TextColor = 0x00000000;
			m_BkColor = 0x00000000;
		}
	};

	SkillKeyInfo m_SkillKeyInfoNor;
	SkillKeyInfo m_SkillKeyInfoOn;

	//T50메가 개틀링건 관련
	DrawActiveCountType m_ActiveCountType;
	bool m_bRedUI;

	// Active Count
	int	m_iCurActiveCount;
	int	m_iMaxActiveCount;

	bool m_bAllGrayState;

	FSMStateMgr m_SkillState;
	FSMStateMgr m_KeyState;

	AccessorySkillListWnd *m_pAccessorySkillListWnd;

	WORD m_wSkillKeyLeftValue;
	WORD m_wSkillKeyCenterValue;
	WORD m_wSkillKeyRightValue;

	ioUIRenderImage *m_pEnableEffect1;
	ioUIRenderImage *m_pEnableEffect2;
	DWORD m_dwEffectTime1;
	DWORD m_dwEffectTime2;
	float m_fEffectScale;

	ioHashString m_szEffectSound;

	float m_fCurEffectRate;
	bool m_bEnableEffect;
	DWORD m_dwEffectStartTime;

	ioUIRenderImage *m_pExcaliburRed;
	ioUIRenderImage *m_pExcaliburYellow;

	ioUIRenderImage *m_pExcaliburRedS;
	ioUIRenderImage *m_pExcaliburYellowS;

	ioUIRenderImage* m_pExtraGaugeBack;
	ioUIRenderImage* m_pExtraGaugeRed;
	ioUIRenderImage* m_pExtraGaugeYellow;

protected:
	int GetRenderImageStartHeight( ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax );
	void InitSkillState();
	void InitKeyState();

	void SkillKeySet( AccessorySkillListBtn::SkillKeyInfo &rkSkillKeyInfo, WORD wKeySetLeftValue, WORD wKeySetCenterValue, WORD wKeySetRightValue, DWORD dwTextColor, DWORD dwBkColor );

protected:
	void RenderInputKeyBack( int xPos, int yPos );
	bool RenderUseSkillEffect( int xPos, int yPos );
	void RenderSkillSlot( int xPos, int yPos );
	void RenderInputKey( int xPos, int yPos );
	void RenderGrowthLevel( int xPos, int yPos, int iGrowthLevel );

	void RenderGaugeIcon( int xPos, int yPos, ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax, bool bEnableUpdate = true );
	void RenderGaugeIcon2( int xPos, int yPos,
		ioUIRenderImage *pBaseImage,
		ioUIRenderImage *pOverImage,
		int iSrcHeight, int iCur, int iMax, bool bEnableUpdate = true );

	void RenderActiveCount( int xPos, int yPos );

	void RenderEnableEffect( int iXPos, int iYPos );

public:
	void UpdateGaugeInfo( int iMaxValue, int iCurValue, int iExtendMax, bool bUse, bool bUsePassiveGauge = false );
	void UpdateActiveCount( int iCur, int iMax, int iSlotType );
	void UpdateActiveCountType( DrawActiveCountType iType, bool bRedUI );	
	void ResetActiveCount( int iSlotType );
	void UpdateItemSlotInfo( ioBaseChar *pOwner );
	void UpdateSkillSlotInfo( ioBaseChar *pOwner );
	void UpdateSkillKeySet();

	void SetEnableEffect( bool bEnable );
	void UpdateEnableEffect();

public:
	virtual void iwm_create();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();

public:
	AccessorySkillListBtn();
	virtual ~AccessorySkillListBtn();
};

//////////////////////////////////////////////////////////////////////////
class AccessorySkillListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_RING		= 6, // EquipSlot::ES_RING 값이 동일해야 함.
		ID_NECKLACE	= 7, // EquipSlot::ES_NECKLACE 값이 동일해야 함.
		ID_BRACELET	= 8, // EquipSlot::ES_BRACELET 값이 동일해야 함.
		MAX_SLOT	= 3,
	};

	enum
	{
		ID_TOOL_TIP		= 21,
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
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

	typedef struct tagSkillDesc
	{
		ioHashString m_szDescName;
		ioHashString m_szDescFullName;
		ioHashString m_szDesc;
		ioHashString m_szIconName;
	}SkillDesc;

protected:
	int m_iClassTypes[MAX_SLOT];
	SkillDesc m_kSkillDesc[MAX_SLOT];

	int m_iCurOverWnd;

	FSMStateMgr m_ShowAniState;

	int m_iDefaultX;
	int m_iDefaultY;
	ioHashString m_szAniSound;

	CharState m_eOwnerState;

	// ToolTip
	ioUIRenderImage *m_pToolTipCen;
	ioUIRenderImage *m_pToolTipLMid;
	ioUIRenderImage *m_pToolTipRMid;
	ioUIRenderImage *m_pToolTipLeft;
	ioUIRenderImage *m_pToolTipRight;

	ioUIRenderImage *m_pPointImage;

	// Key Image Set
	typedef std::map< ioHashString, ioUIRenderImage* > KeyImageSetMap;
	KeyImageSetMap m_KeyImageSetMap;
public:
	void UpdateByOwner( ioBaseChar *pOwner );
	int GetCurState() const;
	void ChangeShowAniState( ShowAniType eShowAniType );
	bool SkipRender();
	ioUIRenderImage *GetKeyImageSet( const ioHashString &szImageFullName );

protected:
	void UpdateSkillList( ioBaseChar *pOwner );
	void UpdateGaugeInfo( int iSlot, int iMaxValue, int iCurValue, int iExtendMax, bool bUse, bool bUsePassiveGauge = false );
	void UpdateActiveCount( ioBaseChar *pOwner, int iSlotType );
	void UpdateActiveCountType( int iSlotType, AccessorySkillListBtn::DrawActiveCountType iType, bool bRedUI );
	void ResetActiveCount( int iSlotType );
	void InitShowAniState();
	void UpdateDefaultPos();

	void ProcessMouseOver();

public:
	int GetCurOverID();

public:
	virtual void iwm_create();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void RenderWnd();

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();
	virtual void OnProcess( float fTimePerSec );

public:
	AccessorySkillListWnd();
	virtual ~AccessorySkillListWnd();
};

