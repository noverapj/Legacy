

#include "stdafx.h"

#include "ioItemNShopRight.h"

ioItemNShopRight::ioItemNShopRight()
{
}

ioItemNShopRight::~ioItemNShopRight()
{
}

void ioItemNShopRight::ExecuteFirstFunc( ioWnd *pWnd ) const
{
	pWnd->HideWnd();
}

void ioItemNShopRight::ExecuteSecondFunc( ioWnd *pWnd ) const
{
}

ioItemRight::RightType ioItemNShopRight::GetType() const
{
	return RT_ITEM_SHOP;
}