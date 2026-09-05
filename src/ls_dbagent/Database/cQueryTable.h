#pragma once

#include <map>
#include "Database.h"

struct QueryInfo
{
	QueryInfo()
	{
		fields.reserve(10);
	}

	QUERY query;
	FACTORS factors;
	FIELDS fields;
};

typedef std::map<uint32,QueryInfo*> QUERY_TABLE;

class cQueryTable
{
public:
	cQueryTable(void);
	~cQueryTable(void);

	void Init();
	void Destroy();

public:
	BOOL RegisterQuery(const uint32 queryId, const TCHAR* query);
	BOOL AddFactor(const uint32 queryId, const uint32 factor);

	BOOL GetQuery(const uint32 queryId, QUERY& query);
	BOOL GetFactors(const uint32 queryId, FACTORS& factors);

protected:
	QUERY_TABLE m_queryTable;
};

