#pragma once

#include <string>
#include <vector>
#include "cDatabaseADO.h"
#include "../util/cSerialize.h"

class User;


// 쿼리패킷 정보를 저장
class cQuery
{
public:
	cQuery() : m_buffer(NULL), m_length(0), m_count(0)
	{}

public:
	void SetQuery(uint8* buffer, const uint32 length, const uint32 count)
	{
		m_buffer	= buffer;
		m_length	= length;
		m_count		= count;
	}
	
	uint8* GetQueryBuffer()		{ return m_buffer; }
	uint32 GetQueryCount()		{ return m_count; }
	uint32 GetQueryLength()		{ return m_length; }

	cSerialize* GetStorage()	{ return &m_storage; }

protected:
	uint8* m_buffer;
	uint32 m_length;
	uint32 m_count;

	cSerialize m_storage;
};

// 쿼리실행 인터페이스
class cExecuter : public cQuery
{
public:
	cExecuter(void);
	~cExecuter(void);

protected:
	void Init();
	void Destroy();

protected:
	void Debug(const TCHAR *format, ...);

public:
	virtual BOOL OpenDatabase(
		const uint32 databaseId, 
		const TCHAR* serverIP, 
		const uint32 serverPort, 
		const TCHAR* database, 
		const TCHAR* userId, 
		const TCHAR* userPassword) = 0;

public:
	virtual BOOL Execute(User* user, CQueryData *queryData, CQueryResultData& result) = 0;

protected:
	QUERY m_query;
	FACTORS m_factors;

	cSerialize m_serialize;
};

// 쿼리실행 인터페이스 ADO
class cExecuterADO : public cExecuter
{
public:
	cExecuterADO(void);
	~cExecuterADO(void);

public:
	BOOL OpenDatabase(
		const uint32 databaseId, 
		const TCHAR* serverIP, 
		const uint32 serverPort, 
		const TCHAR* database, 
		const TCHAR* userId, 
		const TCHAR* userPassword);

public:
	virtual BOOL Execute(User* user, CQueryData *queryData, CQueryResultData& result);

protected:
	cDatabaseADO m_database;
};

