#include "stdafx.h"
#include <strsafe.h>
#include "Local\ioLocalManager.h"
#include "../Util/cJSON.h"

extern CLog LOG;
namespace Help
{
	void GetParseData( IN const ioHashString &rSourceData, OUT ioHashStringVec &rvParseStr, IN char chToken )
	{
		char szParseTemp[MAX_PATH]="";
		ZeroMemory( szParseTemp, sizeof( szParseTemp ) );
		
		int iTempCnt = 0;
		for (int i = 0; i < rSourceData.Length(); i++)
		{
			if( iTempCnt < MAX_PATH )
			{
				szParseTemp[iTempCnt] = rSourceData.At(i);
				iTempCnt++;
			}

			// Token은 붙어 있는 경우 strtok이 구분을 못하므로 빈값 추가 A||C ---> A| |C
			if( rSourceData.At(i) == chToken && rSourceData.At(i+1) == chToken )
			{
				if( iTempCnt < MAX_PATH )
				{
					szParseTemp[iTempCnt]=' ';
					iTempCnt++;
				}
			}
		}

		// strtok에 사용할 수 있게 변환
		enum { MAX_TOKEN = 2, };
		char szToken[MAX_TOKEN]="";
		ZeroMemory( szToken, sizeof( szToken ) );
		szToken[0] = chToken;

		enum { MAX_LOOP = 100, };
		char *pPos = NULL;
		char *pContext = NULL;
		rvParseStr.clear();
		for (int i = 0; i < MAX_LOOP ; i++)
		{
			if( i == 0 )
				pPos = strtok_s( szParseTemp, szToken, &pContext );
			else
				pPos = strtok_s( NULL, szToken, &pContext );

			if( !pPos )
				break;

			ioHashString sTemp = pPos;
			rvParseStr.push_back( sTemp );
		}
	}

	void DelEndCh( OUT ioHashString &rSourceData )
	{
		// 제일 끝에 값을 제거 한다.
		if( rSourceData.IsEmpty() )
			return;
		
		char szPID[MAX_PATH]="";
		StringCbCopyN( szPID, sizeof( szPID ), rSourceData.c_str(), rSourceData.Length()-1 );
		rSourceData = szPID;
	}


	void EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN const char *szUserKey, IN int iUserKeySize )
	{
		enum { MAX_KEY = 30, MAX_ARRAY = 4, };
		BYTE byKey[MAX_ARRAY][MAX_KEY]={ 1,56,211,49,67,190,123,231,34,6,8,9,56,23,90,8,124,126,137,59,34,23,90,200,201,202,39,98,96,21,
			                             3,56,211,79,67,191,123,232,34,6,1,9,56,23,91,18,129,126,137,200,34,23,90,202,201,202,39,99,96,22, 
										 99,36,111,179,167,91,113,132,39,16,11,19,156,123,191,218,29,26,37,201,32,13,91,222,101,201,139,199,6,21, 
										 100,36,11,19,16,9,113,132,39,16,111,19,156,123,191,218,29,26,137,201,32,13,91,222,101,21,13,19,61,21, };

		int iArray = 0;
		if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_INDONESIA )
			iArray = 0;
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
			iArray = 1;
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_TAIWAN )
			iArray = 2;
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US )
			iArray = 3;

		for(int i =0; i < iSourceSize; i++)
		{
			if( i >= iResultSize ) break;
			szResultData[i] = szSourceData[i] ^ byKey[iArray][i%MAX_KEY];
			szResultData[i] = szResultData[i] ^ szUserKey[i%iUserKeySize];
			szResultData[i] = szResultData[i] ^ byKey[iArray][ ( (MAX_KEY-1)-(i%MAX_KEY) ) ];
			szResultData[i] = szResultData[i] ^ szUserKey[ ( (iUserKeySize-1)-(i%iUserKeySize) ) ];
		}
	}

	void Encode( IN const char* szPlain, IN int iPlainSize, OUT char *szCipher, IN int iCipherSize, IN const char *szUserKey, IN int iUserKey )
	{
		EncryptDecryptData( szCipher, iCipherSize, szPlain, iPlainSize, szUserKey, iUserKey );

		// char를 16진수로 변경
		char szTemp[MAX_PATH*2]="";
		for(int i = 0; i < iPlainSize; i++)
		{
			char szTempHex[MAX_PATH]="";
			StringCbPrintf(szTempHex, sizeof( szTempHex ), "%02x", (BYTE)szCipher[i]); // BYTE 캐스팅해서 FFFF붙지 않는다.
			StringCbCat(szTemp, sizeof( szTemp ),  szTempHex);
		}
		StringCbCopy(szCipher, iCipherSize, szTemp);
	}

	void Decode( IN const char *szCipher, IN int iCipherSize, OUT char* szPlain, IN int iPlainSize, IN const char *szUserKey, IN int iUserKey  )
	{
		char szCharCipher[MAX_PATH*2]="";
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
			if( i >= MAX_PATH*2 )
				break;
			szCharCipher[i] = (BYTE)strtol(szTempOneHex, &stopstring, 16);
		}

		EncryptDecryptData( szPlain, iPlainSize, szCharCipher, iCharChipherSize, szUserKey, iUserKey );
	}


	bool GetHostByName(OUT ioHashStringVec& rvIPList, char* szURL)
	{
		if (!szURL)
			return false;

		struct addrinfo kHints;
		ZeroMemory(&kHints, sizeof(kHints));
		kHints.ai_family = AF_INET;
		kHints.ai_socktype = SOCK_STREAM;

		struct addrinfo* pResult = NULL;
		if (getaddrinfo(szURL, NULL, &kHints, &pResult) != 0 || pResult == NULL)
		{
			LOG.PrintTimeAndLog(0, "%s lpstHostend == NULL.", __FUNCTION__);
			return false;
		}

		for (struct addrinfo* pCur = pResult; pCur != NULL; pCur = pCur->ai_next)
		{
			if (pCur->ai_family != AF_INET)
				continue;

			char szTemp[MAX_PATH] = "";
			if (inet_ntop(AF_INET, &((struct sockaddr_in*)pCur->ai_addr)->sin_addr, szTemp, sizeof(szTemp)) != NULL)
			{
				ioHashString sTemp = szTemp;
				rvIPList.push_back(sTemp);
			}
		}

		freeaddrinfo(pResult);

		if (rvIPList.empty())
		{
			LOG.PrintTimeAndLog(0, "[error][help]%s Local IP empty.", __FUNCTION__);
			return false;
		}

		return true;
	}

	bool GetParseJSon( IN const char * szJsonSource, IN const ioHashStringVec &rvKeyVec, OUT ioHashStringVec &rvValueVec )
	{
		try 
		{
			cJSON *pJson = NULL;
			pJson=cJSON_Parse(szJsonSource);
			if( pJson == NULL )
				return false;
			int iSize      = rvKeyVec.size();
			int iValueSize = rvValueVec.size();
			if( iValueSize < iSize )
				iSize = iValueSize;

			for (int i = 0; i < iSize ; i++)
			{
				const ioHashString &rKey = rvKeyVec[i];
				if( rKey.IsEmpty() )
					continue;

				cJSON *pObject = cJSON_GetObjectItem( pJson, rKey.c_str() );
				if( pObject )
				{
					char szValue[MAX_PATH]="";
					if( pObject->type == cJSON_Number )
					{
						StringCbPrintf( szValue, MAX_PATH, "%d", pObject->valueint );
						rvValueVec[i] = szValue;

					}
					else if( pObject->type == cJSON_String )
					{
						StringCbCopy( szValue, MAX_PATH, pObject->valuestring );
						rvValueVec[i] = szValue;
					}
				}
			}

			cJSON_Delete(pJson);
			return true;
		}
		catch ( ... )
		{
			LOG.PrintTimeAndLog( 0, "%s Json Crash %s", __FUNCTION__, szJsonSource );
			return false;
		}

		return false;
	}

	DWORD GetStringIPToDWORDIP( const char *szIP )
	{
		int  count       = 0;
		int  cut_ip		 = 0;
		char szCut_ip[4][4];
		memset( szCut_ip, 0, sizeof( szCut_ip ) );
		int  len	     = strlen( szIP );
		for(int i = 0;i < len;i++)
		{
			if( szIP[i] == '.')
			{
				count = 0;
				cut_ip++;
			}
			else
				szCut_ip[cut_ip][count++] = szIP[i];
		}
		return (DWORD)(atoi(szCut_ip[0])<<24) | (DWORD)(atoi(szCut_ip[1])<<16) | (DWORD)(atoi(szCut_ip[2])<<8) | atoi(szCut_ip[3]);	
	}


}	// namespace




