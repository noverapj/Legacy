#pragma once
#include "../io3DEngine/ioButton.h"

#define NEW_SHOP_WIDTHCUT_SIZE 90.0f

class NewShopItemInfoWnd : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN      = 1,
		ID_DETAIL_BTN    = 2,
		MAX_BTN,
		MAX_DESC         = 3,
	};

	enum LimitRender
	{
		LR_NONE,
		LR_DEFULT_LIMIT,
		LR_ETC_LIMIT,
	};

protected:
	ioComplexStringPrinter	m_szDescArray[MAX_DESC];

	ioUIRenderFrame*		m_pSelectFrm;
	ioUIRenderFrame*		m_pEventFrm;

	ioUIRenderImage*		m_pLeft;
	bool					m_bSelect;
	bool					m_bDecoEquip;

	LimitRender				m_eLimitRender;

protected:
	NewShopItemInfo  m_ItemInfo;
	int m_nGradeType;

protected:
	void SetBlank();
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );
	void RenderBottom( int iXPos, int iYPos );

public:
	void SetSelect(bool bSelect) { m_bSelect = bSelect; }

	bool IsDecoEquip() const { return m_bDecoEquip; }
	void SetDecoEquip(bool bDecoEquip) { m_bDecoEquip = bDecoEquip; }

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );	
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	void SetInfo( NewShopItemInfo &rkInfo );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	NewShopItemInfoWnd();
	virtual ~NewShopItemInfoWnd();
};