#include "../stdafx.h"
#include "../Util/md5.h"
#include "../ThailandLoginServer/ThailandUser.h"

#include "./iolocalparent.h"

ioLocalParent::ioLocalParent(void)
{
	m_iAgencyNoCnt= 0;
}

ioLocalParent::~ioLocalParent(void)
{
}

void ioLocalParent::GetHexMD5( OUT char *szHexMD5, IN int iHexSize, IN const char *szSource )
{
	enum { MAX_DIGEST = 16, };
	MD5Context md5_ctx;
	BYTE byDigest[MAX_DIGEST];

	MD5Init( &md5_ctx );
	MD5Update( &md5_ctx, (unsigned char const *)szSource, strlen( szSource ) );
	MD5Final( byDigest, &md5_ctx );

	for (int i = 0; i < MAX_DIGEST ; i++)
	{
		char szTempHex[MAX_PATH]="";
		StringCbPrintf(szTempHex, sizeof( szTempHex ), "%02x", byDigest[i]); // BYTE 캐스팅해서 FFFF붙지 않는다.
		StringCbCat( szHexMD5, iHexSize, szTempHex );	
	}
}


void ioLocalParent::GetAgencyNo( OUT char *szAgencyNo, IN int iAgencyNoSize, IN bool bYear )
{
	SYSTEMTIME st;
	GetLocalTime( &st );

	enum { START_YEAR = 2011, MAX_YEAR_TOKEN = 36, };
	char szYearToken[MAX_PATH]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char szTemp[MAX_PATH]="";

	if( bYear ) // 2011
	{
		StringCbPrintf( szTemp, sizeof( szTemp ), "%04d", st.wYear );
	}
	else // 태국 16자 길이 제한으로 년도 대신에 기호값을 넣는다.
	{
		int iArray = st.wYear - START_YEAR;
		if( COMPARE( iArray , 0, MAX_YEAR_TOKEN ) )
			StringCbPrintf( szTemp, sizeof( szTemp ), "%c", szYearToken[iArray] );
	}
	StringCbCat( szAgencyNo, iAgencyNoSize, szTemp );

	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wMonth );
	StringCbCat( szAgencyNo, iAgencyNoSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wDay );
	StringCbCat( szAgencyNo, iAgencyNoSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wHour );
	StringCbCat( szAgencyNo, iAgencyNoSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wMinute );
	StringCbCat( szAgencyNo, iAgencyNoSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wSecond );
	StringCbCat( szAgencyNo, iAgencyNoSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%05d", m_iAgencyNoCnt );
	StringCbCat( szAgencyNo, iAgencyNoSize, szTemp );

	m_iAgencyNoCnt++;
	if( m_iAgencyNoCnt > MAX_AGENCY_NO_CNT )
		m_iAgencyNoCnt = 0;
}