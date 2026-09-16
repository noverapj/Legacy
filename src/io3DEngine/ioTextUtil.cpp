#include "stdafx.h"
#include "ioTextUtil.h"

namespace ioText
{
	static int g_iCodePage = 949;

	int GetCodePage()
	{
		return g_iCodePage;
	}

	void SetCodePage( int iCodePage )
	{
		g_iCodePage = iCodePage;
	}

	bool IsLeadByte( unsigned char byChar )
	{
		return IsDBCSLeadByteEx( g_iCodePage, byChar ) != FALSE;
	}
}
