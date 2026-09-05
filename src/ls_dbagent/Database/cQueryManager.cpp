#include "../StdAfx.h"
#include "cQueryManager.h"


extern void Trace(const char* format, ...);

cQueryManager g_queryManager;

//////////////////////////////////////////////////////////////////////
// Query
//////////////////////////////////////////////////////////////////////
void cQueryManager::Init()
{
	m_executers.clear();
	m_databases.clear();
	m_databaseType = DBTYPE_ADO;
}

void cQueryManager::Destroy()
{
	m_executers.clear();
	m_databases.clear();
}

BOOL cQueryManager::AddDatabase(const uint32 databaseId, const TCHAR* serverIP, const uint32 serverPort, const TCHAR* database, const TCHAR* userId, const TCHAR* userPassword)
{
	char plainPW[MAX_PATH] = "";
	if( strcmp( userPassword, "" ) != 0 )
	{
		Decode( userPassword, strlen(userPassword), plainPW, sizeof( plainPW ) );
	}
	else
	{
		strcpy_s(plainPW, sizeof(plainPW), userPassword );
	}

	DBInfo* dbInfo = new DBInfo;
	if(dbInfo)
	{
		dbInfo->databaseId = databaseId;
		dbInfo->serverPort = serverPort;
		CopyMemory(dbInfo->serverIP, serverIP, sizeof(dbInfo->serverIP));
		CopyMemory(dbInfo->database, database, sizeof(dbInfo->database));
		CopyMemory(dbInfo->userId, userId, sizeof(dbInfo->userId));
		CopyMemory(dbInfo->userPassword, plainPW, sizeof(dbInfo->userPassword));

		m_databases.push_back(dbInfo);
		return TRUE;
	}
	return FALSE;
}

BOOL cQueryManager::AddDatabase(const uint32 databaseId, const TCHAR* database)
{
	DBInfo* dbInfo = new DBInfo;
	if(dbInfo)
	{
		dbInfo->databaseId = databaseId;
		CopyMemory(dbInfo->database, database, sizeof(dbInfo->database));

		m_databases.push_back(dbInfo);
		return TRUE;
	}
	return FALSE;
}

BOOL cQueryManager::CreateExecuter(const uint32 threadId)
{
	ThreadSync ts((SuperParent*)this);

	cExecuter* executer = GetExecuter( threadId );
	if(!executer)
	{
		switch(m_databaseType)
		{
		case DBTYPE_ADO:
			executer = new cExecuterADO;
			break;
		//case DBTYPE_ODBC:
		//	executer = new cExecuterODBC;
		//	break;
		//case DBTYPE_SQLITE:
		//	executer = new cExecuterSQLITE;
		//	break;
		default:
			executer = new cExecuterADO;
			break;
		}
	}
	
	if(executer)
	{
		for(DATABASES::iterator it = m_databases.begin(); it != m_databases.end(); ++it)
		{
			DBInfo* info = (*it);
			if(!info) continue;

			if(!executer->OpenDatabase(info->databaseId, info->serverIP, info->serverPort, info->database, info->userId, info->userPassword))
			{
				delete executer;

				LOG.PrintTimeAndLog(0, "[%lu]Open Database failed %s:%lu, [%s/%s]", threadId, info->serverIP, info->serverPort, info->userId, info->userPassword);
				return FALSE;
			}

			LOG.PrintTimeAndLog( 0,"[%lu]Open Database success %s::%lu", threadId, info->serverIP, info->serverPort );
		}

		m_executers[threadId] = executer;
		return TRUE;
	}

	return FALSE;
}

cExecuter* cQueryManager::GetExecuter(const uint32 threadId)
{	
	EXECUTER_TABLE::iterator it = m_executers.find(threadId);
	if(it == m_executers.end())
	{
		return NULL;
	}
	return it->second;
}

BOOL cQueryManager::Generate(CQueryData *queryData, char* buffer, const int length)
{
	QUERY query;
	FACTORS factors;

	int valueSize  = 0;
	int resultSize = 0;

	if(!g_queryManager.GetQuery(queryData->GetQueryID(), query))
	{
		return FALSE;
	}

	if(!g_queryManager.GetFactors(queryData->GetQueryID(), factors))
	{
		return FALSE;
	}

	return cConnection::Generate(queryData, factors, query.c_str(), buffer, length);
}

void cQueryManager::EncryptDecryptData( OUT char *resultData, IN const int resultLen, IN const char *sourceData, IN const int sourceLen )
{
	enum { MAX_KEY = 30, };
	BYTE byKey[MAX_KEY]={ 1,3,5,7,8,9,3,45,255,39,12,34,56,78,12,43,123,124,124,152,176,129,109,123,123,213,212,218,219,234};
	
	for(int i =0; i < sourceLen; i++)
	{
		if( i >= resultLen ) break;
		resultData[i] = sourceData[i] ^ byKey[i%MAX_KEY];
		resultData[i] = resultData[i] ^ byKey[(sourceLen-i)%MAX_KEY];
	}
}

void cQueryManager::Encode( IN const char* plain, IN int plainLen, OUT char *cipher, IN int cipherSize )
{
	EncryptDecryptData( cipher, cipherSize, plain, plainLen );

	// char를 16진수로 변경
	char szTemp[MAX_PATH]="";
	for(int i = 0; i < plainLen; i++)
	{
		char szTempHex[MAX_PATH]="";
		sprintf_s(szTempHex, sizeof( szTempHex ), "%02x", (BYTE)cipher[i]); // BYTE 캐스팅해서 FFFF붙지 않는다.
		strcat_s(szTemp, sizeof( szTemp ),  szTempHex);
	}
	strcpy_s(cipher, cipherSize, szTemp);
}

void cQueryManager::Decode( IN const char *cipher, IN int cipherSize, OUT char* plain, IN int plainLen  )
{
	char charCipher[MAX_PATH]="";
	int  charChipherSize = cipherSize/2;

	// 16진수 -> char
	int pos = 0;
	for(int i = 0; i < charChipherSize; i++)
	{
		char tempOneHex[MAX_PATH]="";
		char *stopString;
		if( pos >= cipherSize )
			break;
		memcpy(tempOneHex, &cipher[pos], 2);
		pos += 2;
		if( i >= MAX_PATH )
			break;
		charCipher[i] = (BYTE)strtol(tempOneHex, &stopString, 16);
	}

	EncryptDecryptData( plain, plainLen, charCipher, charChipherSize );
}