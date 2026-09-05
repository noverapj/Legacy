#pragma once

#include <map>
#include "cTable.h"
#include "../util/cBuffer.h"
#include "../QueryData/QueryData.h"
#include "Database.h"

//결과 행동
#define _RESULT_CHECK   0
#define _RESULT_NAUGHT  1
#define _RESULT_DESTROY 2

class cConnection
{
public:
	cConnection();
	virtual ~cConnection();

	void Init();
	void Destroy();

public:
	BOOL Create();

	BOOL Open();
	BOOL Open(const char *userName, const char *password, const char *connName);
	BOOL Open(const char *userName, const char *password, const char *dbName, const char *IP, const uint32 port);
	void Close();

	BOOL Execute(CQueryData *queryData, FACTORS& factorList,const char *queryName, cTable* table = NULL);

public:
	static BOOL Generate(CQueryData *queryData, FACTORS& factorList, const char *queryName, char *buffer, const int length);

public:
	void ClearParameters();
	BOOL AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, int8 value);
	BOOL AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, uint8 value);
	BOOL AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, int16 value);
	BOOL AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, int32 value);
	BOOL AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, int64 value);
	BOOL AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, double value);
	BOOL AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, const char * value);
	BOOL AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, _variant_t value, int precision = 0, int scale = 0);

private:
	ADODB::_ConnectionPtr m_connection;
	ADODB::_CommandPtr	  m_command;

	cBuffer* m_connName;
	cBuffer* m_userName;
	cBuffer* m_userPassword;
	uint32	 m_port;
};

class cDatabaseADO
{
public:
	cDatabaseADO();
	virtual ~cDatabaseADO();

	void Init();
	void Destroy();

public:
	BOOL Startup();
	void Cleanup();

	BOOL Create(const uint32 nID);
	void Close(const uint32 nID);

	BOOL Open(const uint32 nID);
	BOOL Open(const uint32 nID,  const char *userName, const char *password, const char *connName);
	BOOL Open(const uint32 nID, const char *userName, const char *password, const char *dbName, const char *IP, const uint32 port);

	BOOL Execute(const uint32 nID, CQueryData *queryData, FACTORS& factorList, const char *queryName, cTable* table = NULL);
	BOOL Generate(const uint32 nID, CQueryData *queryData, FACTORS& factorList, const char *queryName, char *buffer, const int length);

private:
	typedef std::map<uint32,cConnection*> CONNECTION_TABLE;

	CONNECTION_TABLE m_cConnectionTable;
	
};