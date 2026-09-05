#pragma once
#include "ioWnd.h"

class NewShopItemPurchaseLeedWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_SHOP  = 2,
		ID_CANCEL= 3,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;

protected:
	ioHashStringVec  m_vDescVec;
	DWORD			 m_dwEtcitemType;


public:
	bool SetInfo( DWORD dwEtcitemType, const ioHashString &rsIcon, const ioHashString &rsSubIcon, const ioHashString &rsTitle, const ioHashStringVec &rvDescVec );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	NewShopItemPurchaseLeedWnd();
	virtual ~NewShopItemPurchaseLeedWnd();
};