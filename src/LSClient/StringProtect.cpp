#include "stdafx.h"
#include ".\StringProtect.h"
#include <strsafe.h>
#include "Abcdef.h"
#include "ccddaabb.h"

namespace SP
{
	void EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize )
	{
		enum { MAX_KEY = 30, };
		BYTE byKey[MAX_KEY]={ 100,30,50,70,80,90,30,145,255,139,212,134,6,7,121,143,23,24,24,52,16,19,19,131,113,214,222,228,229,235};

		for(int i =0; i < iSourceSize; i++)
		{
			if( i >= iResultSize ) break;
			szResultData[i] = szSourceData[i] ^ byKey[i%MAX_KEY];
			szResultData[i] = szResultData[i] ^ byKey[(iSourceSize-i)%MAX_KEY];
		}
	}

	void Decode( IN const char *szCipher, IN int iCipherSize, OUT char* szPlain, IN int iPlainSize  )
	{
		char szCharCipher[MAX_PATH]="";
		int  iCharChipherSize = iCipherSize/2;
		// 16진수 -> char
		int pos = 0;
		for(int i = 0; i < iCharChipherSize; i++)
		{
			char szTempOneHex[MAX_PATH]="";
			char *stopstring;
			if( pos >= iCipherSize )
				break;
			memcpy(szTempOneHex, &szCipher[pos], 2);
			pos += 2;
			if( i >= MAX_PATH )
				break;
			szCharCipher[i] = (BYTE)strtol(szTempOneHex, &stopstring, 16);
		}

		EncryptDecryptData( szPlain, iPlainSize, szCharCipher, iCharChipherSize );
	}

	const char *GetDecodeStr( const char *szCipher )
	{
		enum { MAX_DATA = 1000, };
		static int iCnt = rand() % MAX_DATA;
		static char szPlain[MAX_DATA][MAX_PATH];

		iCnt++;
		if( iCnt >= MAX_DATA )
			iCnt = 0;

		ZeroMemory( szPlain[iCnt], MAX_PATH );
		
		Decode( szCipher, strlen( szCipher ), szPlain[iCnt], MAX_PATH );
		return szPlain[iCnt];
	}
	
	const char *GetAESDecodeStr( IN const char *szCipher, IN DWORD dwKeyAndSize, OUT ioHashString &rsReturn )
	{
		char szKey[MAX_PATH]="";
		char szArgu[MAX_PATH]={ 37, 99, 37, 99, 37, 117, 37, 99, 37, 99 , 37, 99, 0 };
		StringCbPrintf( szKey, sizeof( szKey ), szArgu, 25, 31, dwKeyAndSize, 48, 125, 75 );

		char szPlain[MAX_PATH];
		ZeroMemory( szPlain, sizeof( szPlain ) );

		Abcdef oRijndael;                                               
		oRijndael.MakeKey( szKey ,Abcdef::sm_chain0, 16, 16);
		oRijndael.Decrypt( szCipher, szPlain, (dwKeyAndSize%1000), (dwKeyAndSize/1000)%3 );

		rsReturn = szPlain;
		return rsReturn.c_str();
	}

	const char *GetRC5DecodeStr( IN const char *szCipher, IN DWORD dwEncryptSize, OUT ioHashString &rsReturn )
	{
		char szPlain[MAX_PATH];
		ZeroMemory( szPlain, sizeof( szPlain ) );

		//인니
		//unsigned char szKey[ccddaabb::KEY_SIZE]={ 0, 0, 0, 0, 45, 217, 137, 199, 137, 199 , 237, 199, 6, 9, 90, 13 };
		//한국
		unsigned char szKey[ccddaabb::KEY_SIZE]={ 0, 0, 0, 0, 34, 26, 100, 2, 233, 234 , 111, 123, 89, 102, 234, 2 };
		memcpy( szKey, &dwEncryptSize, sizeof( dwEncryptSize ) );
		ccddaabb cRC5;
		cRC5.Setup( (const char*)szKey, dwEncryptSize, dwEncryptSize+345783  );
		cRC5.DecryptByte( szCipher, szPlain, (dwEncryptSize%1000) );

		rsReturn = szPlain;
		return rsReturn.c_str();
	}
}