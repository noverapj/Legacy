#pragma once

#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"

#include "../ioComplexStringPrinter.h"
#include "ioMessageBox.h"

class ioUIRenderImage;
class ioXMLElement;

////////////////////////////////////////////////////////////////////////////////
class ioGrowthSelectWnd : public ioWnd
{
public:
	enum
	{
		ID_X				= 1,
		ID_CONFIRM			= 2,
		ID_ICON_BTN			= 3,

		ID_ITEM_UP1			= 11,
		ID_ITEM_UP2			= 12,
		ID_ITEM_UP3			= 13,
		ID_ITEM_UP4			= 14,
		ID_CHAR_UP1			= 15,
		ID_CHAR_UP2			= 16,
		ID_CHAR_UP3			= 17,
		ID_CHAR_UP4			= 18,

		ID_ITEM_DOWN1		= 21,
		ID_ITEM_DOWN2		= 22,
		ID_ITEM_DOWN3		= 23,
		ID_ITEM_DOWN4		= 24,
		ID_CHAR_DOWN1		= 25,
		ID_CHAR_DOWN2		= 26,
		ID_CHAR_DOWN3		= 27,
		ID_CHAR_DOWN4		= 28,
	};

protected:
	int m_iClassType;
	int m_iMaxLevel;
	int m_iCurPoint;
	int m_iUsePoint;
	int m_iNeedPeso;
	ioComplexStringPrinter m_ClassInfo[2];

	int m_CharGrowth[CUR_GROWTH_CNT];
	int m_ItemGrowth[CUR_GROWTH_CNT];
	int m_CharLevel[CUR_GROWTH_CNT];
	int m_ItemLevel[CUR_GROWTH_CNT];
	int m_CharTotalLevel[CUR_GROWTH_CNT];
	int m_ItemTotalLevel[CUR_GROWTH_CNT];
	int m_iCharLevelUp[CUR_GROWTH_CNT];
	int m_iItemLevelUp[CUR_GROWTH_CNT];

	ioHashStringVec m_CharGrowthText;
	ioHashStringVec m_ItemGrowthText;
	ioHashString m_EquipItemGrowthText[CUR_GROWTH_CNT];
	ioHashString m_szBeforeRenderText[10];
	ioHashString m_szPointPesoText[5];
	ioHashString m_szCurGrowthText;
	ioHashString m_szGrowthText;
	ioHashString m_szConfirmText;
	ioHashString m_szNoMoneyText;

	ioUIRenderImage *m_pPointImg;
	ioUIRenderImage *m_pBoxLeft;
	ioUIRenderImage *m_pBoxTop;
	ioUIRenderImage* m_pDarkStrip;
	ioUIRenderImage* m_pLightStrip;
	ioUIRenderImage* m_pGaugeBack;
	ioUIRenderImage* m_pGaugeGray;
	ioUIRenderImage* m_pGaugeGreen;
	ioUIRenderImage* m_pCharIcon[CUR_GROWTH_CNT];
	ioUIRenderImage* m_pSkillIcon[CUR_GROWTH_CNT];

	ioUIRenderFrame *m_pInnerFrm;	// 2020-08-07

private:
	void BeforeRenderDesc();

public:
	void SetGrowthSelectInfo( int iClassType );
	void UpdateInfo();
	void ResetBtns();
	void Confirm();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );	// 2020-08-07
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();

public:
	ioGrowthSelectWnd();
	virtual ~ioGrowthSelectWnd();
};