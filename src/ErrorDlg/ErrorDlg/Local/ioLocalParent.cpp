#include "../stdafx.h"
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

void ioLocalParent::LoadINI()
{
	/*char szBuf[MAX_PATH]="";
	ioINILoader kLoaderLocal( SP::GetRC5DecodeStr( "\xad\x2c\xe1\x67\x88\xec\x6f\xf7\xd1\xf7\x88\x09\xdd\xda\x85\x31\x17\xfe\x3a\x1e\xf0\xe2\x02\x66", 2086036024, ioHashString() ) );
	kLoaderLocal.SetTitle( SP::GetRC5DecodeStr( "\xd4\xef\xb9\x58\x11\x69\xe9\xe1", 1783604008, ioHashString() ) );

	ZeroMemory( szBuf, sizeof( szBuf ) );
	kLoaderLocal.LoadString( SP::GetRC5DecodeStr( "\x51\x4c\x2c\xb3\xd7\xb0\x12\x96\x83\x5d\x6d\x78\xf6\xbb\xc2\xe1", 2028946016, ioHashString() ), "", szBuf, MAX_PATH );
	m_sGuildMaster = szBuf;

	ZeroMemory( szBuf, sizeof( szBuf ) );
	kLoaderLocal.LoadString( SP::GetRC5DecodeStr( "\x9e\x63\x48\x3d\xa0\x4a\xb9\xf7\x5c\xab\x65\xa5\x87\xd8\x94\x83\x00\xd4\xa0\x88\x30\x6d\xdf\x1d", 2946108024, ioHashString() ), "", szBuf, MAX_PATH );
	m_sGuildSecondMaster = szBuf;

	ZeroMemory( szBuf, sizeof( szBuf ) );
	kLoaderLocal.LoadString( SP::GetRC5DecodeStr( "\x35\xed\xd8\xce\xb4\xc8\x83\x76\x64\x2f\xb4\x39\x76\xf6\xc7\x93", 1178342016, ioHashString() ), "", szBuf, MAX_PATH );
	m_sGuildGeneral = szBuf;*/
}

const char * ioLocalParent::GetGuildMasterPostion()
{
	return m_sGuildMaster.c_str();
}

const char * ioLocalParent::GetGuildSecondMasterPosition()
{
	return m_sGuildSecondMaster.c_str();
}

const char * ioLocalParent::GetGuildGeneralPosition()
{
	return m_sGuildGeneral.c_str();
}
