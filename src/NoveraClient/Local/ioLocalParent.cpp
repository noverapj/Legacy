#include "stdafx.h"
#include ".\iolocalparent.h"

ioLocalParent::ioLocalParent(void)
{
}

ioLocalParent::~ioLocalParent(void)
{
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
