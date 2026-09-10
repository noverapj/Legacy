

#include "stdafx.h"

#include "ioCommunityRight.h"

ioCommunityRight::ioCommunityRight()
{
}

ioCommunityRight::~ioCommunityRight()
{
}

void ioCommunityRight::ExecuteFirstFunc( ioWnd *pWnd ) const
{
	// goto community...
	g_GUIMgr.SetMsgBox(MB_OK, NULL, STR(1) );
}

void ioCommunityRight::ExecuteSecondFunc( ioWnd *pWnd ) const
{
}

ioItemRight::RightType ioCommunityRight::GetType() const
{
	return RT_COMMUNITY;
}