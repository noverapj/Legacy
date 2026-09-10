#pragma once
#include "../io3DEngine/ioWnd.h"

#define NEW_SHOP_TOOLTIP_WIDTHCUT_SIZE 130.0f

class NewShopToolTip : public ioWnd
{
public:
	enum
	{
		ID_ICON_BTN      = 1,
		MAX_DESC         = 3,
	};
protected:
	int m_iItemType;
	int m_iManualIndex;
	ioComplexStringPrinter m_szDescArray[MAX_DESC];

	int m_nGradeType;

protected:
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );
	void UpdateWndSize();

public:
	bool SetInfo( NewShopItemInfo &rkInfo );

public:
	virtual void iwm_show();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	NewShopToolTip();
	virtual ~NewShopToolTip();
};