#pragma once
#include "../io3DEngine/ioWnd.h"

#define PRACTICE_TOOLTIP_WIDTHCUT_SIZE 130.0f

class PracticeToolTip : public ioWnd
{
public:
	enum
	{
		ID_ICON_BTN      = 1,
		MAX_DESC         = 3,

		MAX_ACC_COM_DESC = 3,

		LINE_GAP		= 18,
		ACC_COM_GAP		= 12,
	};
public:
	struct MToolTipItemInfo
	{
		ioHashString m_sName;
		ioHashString m_sNameAdd;
		ioHashString m_sIcon;
		ioHashString m_sSubIcon;
		int m_iLimitLevel;
		int m_iReinForce;
		int m_iManualIndex;
		int m_iCharGrowth[MAX_CHAR_GROWTH];
		int m_iItemGrowth[MAX_ITEM_GROWTH];
		int m_nGradeType;

		void Clear()
		{
			m_sName.Clear();
			m_sNameAdd.Clear();
			m_sIcon.Clear();
			m_sSubIcon.Clear();
			m_iLimitLevel  = 0;
			m_iReinForce   = 0;
			m_iManualIndex = 0;
			for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
				m_iCharGrowth[i] = 0;
			for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
				m_iItemGrowth[i] = 0;
			m_nGradeType = 0;
		}

		MToolTipItemInfo()
		{
			Clear();
		}
	};
protected:
	int m_iMainType;
	int m_iMagicCode;
	int m_iMagicCodeEx;
	int m_iMagicCodeEx2;
	int m_iManualIndex;
	int m_iCharGrowth[MAX_CHAR_GROWTH];
	int m_iItemGrowth[MAX_ITEM_GROWTH];

	ioComplexStringPrinter m_szDescArray[MAX_DESC];

	IoString m_AccDesc;

	int m_iMaxAccComDesc;
	ioComplexStringPrinter m_szAccComDescArray[MAX_ACC_COM_DESC];

	ioUIRenderImage *m_pGrowthLineLeft;
	ioUIRenderImage *m_pGrowthLineCenter;
	ioUIRenderImage *m_pGrowthLineRight;
	ioUIRenderImage *m_pGrowthBackGray;
	ioUIRenderImage *m_pGrowthBackLight;

protected:
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );
	void RenderGrowth( int iXPos, int iYPos );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

	void GetItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo );
	void GetExtraItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo );
	void GetExtraItemInfoByItemCode( IN int iItemCode , IN int iReinforce, OUT MToolTipItemInfo &rkReturnInfo );
	void GetDecoItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo );
	void GetEtcItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo );
	void GetMedalItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo );
	void GetPresentItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo );
	void GetCostumeInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo );
	void GetAccessoryInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo );

	DWORD GetUseInfo( OUT ioHashString &rsInfo );

	void Clear();
	void UpdateWndSize();

public:
	bool SetInfo( InventoryItemInfo &rkInfo );
	void GetPopupItemInfo( int iPresentType, int iMent, int iPresentValue1, int iPresentValue2, OUT MToolTipItemInfo &rkReturnInfo );
public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	PracticeToolTip();
	virtual ~PracticeToolTip();
};