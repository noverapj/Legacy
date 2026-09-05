#include "../stdafx.h"
#include ".\iolocalmanagerparent.h"
//#include "HelpFunc.h"

ioLocalManagerParent::LocalType ioLocalManagerParent::m_eLocalType = ioLocalManagerParent::LCT_INDONESIA;

ioLocalManagerParent::ioLocalManagerParent(void)
{
}

ioLocalManagerParent::~ioLocalManagerParent(void)
{
}

DWORD ioLocalManagerParent::GetNativeIMEConvMode( LocalType eLocalType )
{
	if( eLocalType == LCT_US ||
		eLocalType == LCT_INDONESIA )
		return IME_CMODE_ALPHANUMERIC;
	else
		return IME_CMODE_NATIVE;
}

void ioLocalManagerParent::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType, IN LocalType eLocalType )
{
	if( eLocalType == LCT_KOREA )
	{
		if( iPasswordType == 0 )
		{
			// password 0: iosuccess#@
			char szEncPassWord[MAX_PASSWORD]={ -105, 112, 108, 127, 62, 66, 9, -43, 53, 4, 64, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
		else if( iPasswordType == 1 )
		{
			// password 1: XrFrI0%3BF%!0Dcx$30-
			char szEncPassWord[MAX_PASSWORD]={ -90, 109, 89, 120, 20, 17, 73, -107, 4, 97, 37, 6, 118, -30, 15, 89, 121, 57, 47, 50 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
	}
	else if( eLocalType == LCT_US )
	{
		if( iPasswordType == 0 )
		{
			// password : eE39DkE!%E0
			char szEncPassWord[MAX_PASSWORD]={ -101, 90, 44, 51, 25, 74, 41, -121, 99, 98, 48, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
		else if( iPasswordType == 1 )
		{
			// password : Eg%^io03UT$Cvf921-!$
			char szEncPassWord[MAX_PASSWORD]={ -69, 120, 58, 84, 52, 78, 92, -107, 19, 115, 36, 100, 48, -64, 85, 19, 108, 39, 62, 59 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
	}
	else if( eLocalType == LCT_TAIWAN )
	{
		if( iPasswordType == 0 )
		{
			// password : iUT38#@49vnFdjf)(4sg
			char szEncPassWord[MAX_PASSWORD]={ -105, 74, 75, 57, 101, 2, 44, -110, 127, 81, 110, 97, 34, -52, 10, 8, 117, 62, 108, 120 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
		else if( iPasswordType == 1 )
		{
			// password : Yi#weT%^903Unv0$2gfj
			char szEncPassWord[MAX_PASSWORD]={ -89, 118, 60, 125, 56, 117, 73, -8, 127, 23, 51, 114, 40, -48, 92, 5, 111, 109, 121, 117 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
	}
	else if( eLocalType == LCT_INDONESIA )
	{
		if( iPasswordType == 0 )
		{
			// password : T*$f40FRjfoe*(fl304d
			char szEncPassWord[MAX_PASSWORD]={ -86, 53, 59, 108, 105, 17, 42, -12, 44, 65, 111, 66, 108, -114, 10, 77, 110, 58, 43, 123 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
		else if( iPasswordType == 1 )
		{
			// password : Mfe$%2049eFeodk*&31Z
			char szEncPassWord[MAX_PASSWORD]={ -77, 121, 122, 46, 120, 19, 92, -110, 127, 66, 70, 66, 41, -62, 7, 11, 123, 57, 46, 69 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
	}
	else if( eLocalType == LCT_JAPAN )
	{
		if( iPasswordType == 0 )
		{
			// password : EDgei%^df930%#fj!_=]
			char szEncPassWord[MAX_PASSWORD]={ -69, 91, 120, 111, 52, 4, 50, -62, 32, 30, 51, 23, 99, -123, 10, 75, 124, 85, 34, 66 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
		else if( iPasswordType == 1 )
		{
			// password : @7$gjTRreie][!323O++
			char szEncPassWord[MAX_PASSWORD]={ -66, 40, 59, 109, 55, 117, 62, -44, 35, 78, 101, 122, 29, -121, 95, 19, 110, 69, 52, 52 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
	}
	else if( eLocalType == LCT_THAILAND )
	{
		if( iPasswordType == 0 )
		{
			// password : K3$dls49YU#$#eoE3054
			char szEncPassWord[MAX_PASSWORD]={ -75, 44, 59, 110, 49, 82, 88, -97, 31, 114, 35, 3, 101, -61, 3, 100, 110, 58, 42, 43 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
		else if( iPasswordType == 1 )
		{
			// password : -_495IUEVJdlsl++32ed
			char szEncPassWord[MAX_PASSWORD]={ -45, 64, 43, 51, 104, 104, 57, -29, 16, 109, 100, 75, 53, -54, 71, 10, 110, 56, 122, 123 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
	}
	else if( eLocalType == LCT_CHINA )
	{
		if( iPasswordType == 0 )
		{
			// password : -)4TRfkl-41$%dgkrm05
			char szEncPassWord[MAX_PASSWORD]={ -45, 54, 43, 94, 15, 71, 7, -54, 107, 19, 49, 3, 99, -62, 11, 74, 47, 103, 47, 42 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
		else if( iPasswordType == 1 )
		{
			// password : |059rtuGReowo@##tkg0
			char szEncPassWord[MAX_PASSWORD]={ -126, 47, 42, 51, 47, 85, 25, -31, 20, 66, 111, 80, 41, -26, 79, 2, 41, 97, 120, 47 };
			EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
		}
	}
}

void ioLocalManagerParent::EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN bool bPassword )
{
	enum { MAX_KEY_TYPE = 2,  MAX_KEY = 30, };
	BYTE byKey[MAX_KEY_TYPE][MAX_KEY]={255,1,2,9,89,32,123,39,34,211,222,244,100,129,23,1,4,3,29,30,1,4,5,7,8,233,89,1,98,67, // password ¿ë
	                                   48,29,96,1,9,48,57,213,178,123,67,90,2,4,254,255,6,8,9,23,90,44,214,199,108,119,3,2,2,};
	int iKeyType = 0;
	if( !bPassword )
		iKeyType = 1;

	for(int i =0; i < iSourceSize; i++)
	{
		if( i >= iResultSize ) break;
		szResultData[i] = szSourceData[i] ^ byKey[iKeyType][i%MAX_KEY];
		szResultData[i] = szResultData[i] ^ byKey[iKeyType][(iSourceSize-i)%MAX_KEY];
	}
}

void ioLocalManagerParent::GetChangedFontScale( OUT float &rfFontScale )
{
	//if( ioLocalManagerParent::GetLocalType() == ioLocalManagerParent::LCT_US )
	//{
	//	rfFontScale -= 0.02;
	//	if( rfFontScale < 0.0f )
	//		rfFontScale = 0.0f;
	//}
}

bool ioLocalManagerParent::IsVPrintf()
{
	if( GetLocalType() == LCT_KOREA )
		return true;

	return false;
}

bool ioLocalManagerParent::IsIME()
{
	if( GetLocalType() == LCT_KOREA     || 
		GetLocalType() == LCT_US        || 
		GetLocalType() == LCT_INDONESIA || 
		GetLocalType() == LCT_THAILAND )
		return true;

	return false;
}