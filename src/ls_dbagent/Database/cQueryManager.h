#pragma once

#include <map>
#include <vector>
#include "cExecuter.h"
#include "cQueryTable.h"

// SQL서버 정보를 기록
struct DBInfo
{
	DBInfo()
	{
		databaseId = 0;
		serverPort = 0;
		ZeroMemory(serverIP, sizeof(serverIP));
		ZeroMemory(database, sizeof(database));
		ZeroMemory(userId, sizeof(userId));
		ZeroMemory(userPassword, sizeof(userPassword));
	}
	uint32 databaseId;
	uint32 serverPort;
	TCHAR serverIP[50];
	TCHAR database[50];
	TCHAR userId[50];
	TCHAR userPassword[50];
};


// 데이타베이스와 쿼리 전체를 담당하는 클래스
class cQueryManager : public SuperParent, public cQueryTable
{
public:
	cQueryManager()  { Init(); }
	~cQueryManager() { Destroy(); }

protected:
	void Init();
	void Destroy();

public:
	enum DATABASETYPE
	{
		DBTYPE_NONE = 0,
		DBTYPE_ADO,
		//DBTYPE_ODBC,
		//DBTYPE_SQLITE,
	};

	BOOL AddDatabase(const uint32 databaseId, const TCHAR* serverIP, const uint32 serverPort, const TCHAR* database, const TCHAR* userId, const TCHAR* userPassword);
	BOOL AddDatabase(const uint32 databaseId, const TCHAR* database);

public:
	BOOL CreateExecuter(const uint32 threadId);
	cExecuter* GetExecuter(const uint32 threadId);
	
	BOOL Generate(CQueryData *queryData, char* buffer, const int length);

	uint32 GetExecuterCount() { return m_executers.size(); }
	
public:
	void EncryptDecryptData( OUT char *resultData, IN const int resultLen, IN const char *sourceData, IN const int sourceLen );
	void Encode( IN const char* plain, IN int plainLen, OUT char *cipher, IN int cipherSize );
	void Decode( IN const char *cipher, IN int cipherSize, OUT char* plain, IN int plainLen );

protected: 
	typedef std::map<uint32,cExecuter*>	EXECUTER_TABLE;
	typedef std::vector<DBInfo*>		DATABASES;

	QUERY_TABLE			m_queryTable;
	EXECUTER_TABLE		m_executers;
	DATABASES			m_databases;
	DATABASETYPE        m_databaseType;
};

extern cQueryManager g_queryManager;