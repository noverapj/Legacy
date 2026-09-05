#pragma once

class ioUIRenderImage;
class ioComplexStringPrinter;

struct MaterialCompoundTargetItemInfo
{
	int m_iSlotIndex;
	int m_iItemCode;
	int m_iReinforce;

	MaterialCompoundTargetItemInfo()
	{
		m_iSlotIndex = 0;
		m_iItemCode = 0;
		m_iReinforce = 0;
	}
};
typedef std::vector< MaterialCompoundTargetItemInfo > InvenItemNewInfoList;

class MaterialCompoundItemInfoSort : public std::binary_function< const MaterialCompoundTargetItemInfo& , const MaterialCompoundTargetItemInfo&, bool >
{
public:
	bool operator() ( const MaterialCompoundTargetItemInfo &lhs, const MaterialCompoundTargetItemInfo &rhs ) const
	{
		if( lhs.m_iItemCode < rhs.m_iItemCode )
			return true;

		if( lhs.m_iItemCode == rhs.m_iItemCode )
		{
			if( lhs.m_iReinforce > rhs.m_iReinforce )
				return true;	
		}
		return false;
	}
};

class MaterialItemSort : public std::binary_function< const AlchemicItemInfo& , const AlchemicItemInfo&, bool >
{
public:
	bool operator() ( const AlchemicItemInfo &lhs, const AlchemicItemInfo &rhs ) const
	{
		if( lhs.m_iCount > rhs.m_iCount )
			return true;

		if( lhs.m_iCount == rhs.m_iCount )
		{
			if( lhs.m_iCode < rhs.m_iCode )
				return true;	
		}
		return false;
	}
};

struct MagicEffectInfo
{
	DWORD m_dwCreateTime;

	int m_iCurAlphaRate;
	float m_fCurScaleRate;


	MagicEffectInfo()
	{
		Init();
	}

	void Init()
	{
		m_dwCreateTime = 0;
		m_fCurScaleRate = FLOAT1;
		m_iCurAlphaRate = MAX_ALPHA_RATE;
	}
};
typedef std::vector< MagicEffectInfo > vMagicEffectInfo;

class ItemReinforceDownWnd : public ioWnd
{
public:
	//weapon btn
	enum 
	{
		ID_ENABLE_REINFORECE_ITEM_SELECT_BTN	 = 100,
		ID_REINFORCE_BTN						 = 101,
	};

protected:
	int		  m_iItemIndex;

	ioUIRenderImage *m_pMagicCircle;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderImage *m_pItemInfoBG;
	ioUIRenderImage *m_pDarkTextBack;
	ioUIRenderImage *m_pLightTextBack;

	IoString m_ItemReinforceInfo;
	IoString m_SoulStoneCountText;

	ioComplexStringPrinter m_ItemSelectPrinter;
	ioHashString m_szSoulStoneText;
	ioHashString m_szNoItemDesc;
	ioHashString m_ItemInfo;

	//아이템 강화 수치
	int m_iItemReinforce;

	// magic effect
	vMagicEffectInfo m_vMagicEffectInfoList;
	DWORD m_dwMagicEffectCreateTime;

	//result effect
	bool m_bShowResultEffect;
	DWORD m_dwResultStartTime;
	DWORD m_dwResultEndTime;
	DWORD m_dwResultEffectEndTime;
	DWORD m_dwResultEffectTime;
	ioUIRenderImage *m_pLightImage;
	float m_fScaleRate;
	float m_fAlphaRate;
	float m_fSlopeRate;
	BYTE m_iAlpha1;
	BYTE m_iAlpha2;
	float m_fCurEffectScale;
	//result sound
	ioHashString m_szRollingSound;
	UIRenderType m_iIconRenderType;
	int m_nGradeType;

public:
	void SetItemInfo( int iIndex );
	void ProcessMagicCircle();
	void ProcessResultEffect();
public:
	virtual void iwm_hide();
	virtual void OnProcess( float fTimePerSec );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	
protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

protected:
	void ClearListAll();
	void CreateMagicCircle();
	void OnRenderMagicCircle();
	void OnRenderResultEffect();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	ItemReinforceDownWnd(void);
	virtual ~ItemReinforceDownWnd(void);
};

class ItemReinforceDoneWnd : public ioWnd
{
public:
	//weapon btn
	enum 
	{
		ID_REINFORECE_AGAIN_BTN					 = 100,
		ID_SEE_ITEM_BTN							 = 101,
	};

protected:
	int		  m_iItemIndex;

	ioUIRenderImage *m_pMagicCircle;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderImage *m_pItemInfoBG;
	ioUIRenderImage *m_pDarkTextBack;
	ioUIRenderImage *m_pLightTextBack;

	IoString m_ItemReinforceInfo;
	IoString m_SoulStoneCountText;

	ioComplexStringPrinter m_ReinforcedPrinter;
	ioHashString m_szSoulStoneText;
	ioHashString m_ItemInfo;

	//아이템 강화 수치
	int m_iItemReinforce;

	// magic effect
	vMagicEffectInfo m_vMagicEffectInfoList;

	DWORD m_dwMagicEffectCreateTime;

	//Success light effect
	ioUIRenderImage *m_pSuccessLightImage;
	ioHashStringVec m_vSuccessLightImage;	// 성공
	DWORD   m_dwFinalResultStartTime;
	DWORD	m_dwSuccessEffectTime;
	int		m_iCurSuccessEffectArray;
	DWORD   m_dwRollingEffectCheckTime;
	DWORD   m_dwRollingEffectTime;
	//Success Effect
	ioHashString m_szSuccessSound;
	int m_nGradeType;

public:
	void SetItemInfo( int iIndex );
	void ProcessMagicCircle();
	void ProcessSuccessLightEffect();

public:
	virtual void OnProcess( float fTimePerSec );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void OnRender();

protected:
	void ClearListAll();
	void CreateMagicCircle();
	void OnRenderMagicCircle();
	void OnRenderSuccessLightEffect();
	
public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	ItemReinforceDoneWnd(void);
	virtual ~ItemReinforceDoneWnd(void);
};