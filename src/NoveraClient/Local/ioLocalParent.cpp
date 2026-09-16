#include "stdafx.h"
#include ".\iolocalparent.h"
#include "../ioMyInfo.h"
#include "../ioApplication.h"

ioLocalParent::ioLocalParent(void)
{
}

ioLocalParent::~ioLocalParent(void)
{
}

void ioLocalParent::SetTimeZone()
{
	_putenv("TZ=UTC0");
	_tzset();
}

void ioLocalParent::OpenFillCashPage()
{
	if( g_MyInfo.IsEntryFormality() )
		g_App.OpenFillCashPage();
	else
		g_App.OpenCashEntryPage();
}

bool ioLocalParent::IsRightNewID( const char *szID )
{
	int size = strlen(szID);
	for (int i=0; i<size; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) &&
			szID[i] != '-' && szID[i] != '_' )
		{
			return false;
		}
	}
	return true;
}

const char * ioLocalParent::GetErrorSolutionURL()
{
	return "https://github.com/noverapj/Legacy/issues";
}

bool ioLocalParent::IsRightLicense()
{
	SYSTEMTIME st;
	GetLocalTime( &st );
	int iDate = (st.wYear * 10000) + (st.wMonth * 100) + st.wDay;

	if( iDate >= GetLicenseDate() )
		return false;

	return true;
}
