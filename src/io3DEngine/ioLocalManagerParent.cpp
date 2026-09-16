#include "stdafx.h"
#include ".\iolocalmanagerparent.h"
#include "HelpFunc.h"

ioLocalManagerParent::LocalType ioLocalManagerParent::m_eLocalType = ioLocalManagerParent::LCT_KOREA;

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
	//custom password soon
	switch( eLocalType )
	{
		case LCT_KOREA:
		case LCT_BRAZIL:
		case LCT_CHINA:
		case LCT_EU:
		case LCT_FRANCE:
		case LCT_GERMANY:
		case LCT_INDONESIA:
		case LCT_ITALIA:
		case LCT_JAPAN:
		case LCT_LATIN:
		{
			if( iPasswordType == 0 )
			{
				// password 0: iosuccess#@
				char szEncPassWord[MAX_PASSWORD] = { -105, 112, 108, 127, 62, 66, 9, -43, 53, 4, 64, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
				EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
			}
			else if( iPasswordType == 1 )
			{
				// password 1: XrFrI0%3BF%!0Dcx$30-
				char szEncPassWord[MAX_PASSWORD] = { -90, 109, 89, 120, 20, 17, 73, -107, 4, 97, 37, 6, 118, -30, 15, 89, 121, 57, 47, 50 };
				EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
			}
		}
		break;
		default:
		{
			//default fallback korean
			if( iPasswordType == 0 )
			{
				// password 0: iosuccess#@
				char szEncPassWord[MAX_PASSWORD] = { -105, 112, 108, 127, 62, 66, 9, -43, 53, 4, 64, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
				EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
			}
			else if( iPasswordType == 1 )
			{
				// password 1: XrFrI0%3BF%!0Dcx$30-
				char szEncPassWord[MAX_PASSWORD] = { -90, 109, 89, 120, 20, 17, 73, -107, 4, 97, 37, 6, 118, -30, 15, 89, 121, 57, 47, 50 };
				EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
			}
		}
		break;
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