

#include "stdafx.h"

#include "ioSetItemBuyRight.h"

ioSetItemBuyRight::ioSetItemBuyRight()
{
	m_dwSetItemCode = 0;
}

ioSetItemBuyRight::~ioSetItemBuyRight()
{
}

void ioSetItemBuyRight::LoadProperty( ioINILoader &rkLoader )
{
	ioItemRight::LoadProperty( rkLoader );

	m_dwSetItemCode = rkLoader.LoadInt_e( "set_item_code", 0 );
}

void ioSetItemBuyRight::ExecuteFirstFunc( ioWnd *pWnd ) const
{
	g_GUIMgr.SetMsgBox(MB_OK, NULL, STR(1) );
}

void ioSetItemBuyRight::ExecuteSecondFunc( ioWnd *pWnd ) const
{
}

ioItemRight::RightType ioSetItemBuyRight::GetType() const
{
	return RT_SET_ITEM_BUY;
}

const ioSetItemInfo* ioSetItemBuyRight::GetSetItemInfo() const
{
	return g_SetItemInfoMgr.GetSetInfoByCode( m_dwSetItemCode );
}