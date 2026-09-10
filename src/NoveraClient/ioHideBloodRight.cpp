

#include "stdafx.h"

#include "ioHideBloodRight.h"

ioHideBloodRight::ioHideBloodRight()
{
}

ioHideBloodRight::~ioHideBloodRight()
{
}

void ioHideBloodRight::ExecuteFirstFunc( ioWnd *pWnd ) const
{
	g_GUIMgr.SetMsgBox(MB_OK, NULL, STR(1) );
}

void ioHideBloodRight::ExecuteSecondFunc( ioWnd *pWnd ) const
{
}

ioItemRight::RightType ioHideBloodRight::GetType() const
{
	return RT_HIDE_BLOOD;
}
