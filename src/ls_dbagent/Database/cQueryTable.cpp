#include "../StdAfx.h"
#include "cQueryTable.h"


cQueryTable::cQueryTable(void)
{
	Init();
}


cQueryTable::~cQueryTable(void)
{
	Destroy();
}

void cQueryTable::Init()
{
}

void cQueryTable::Destroy()
{
}

BOOL cQueryTable::RegisterQuery(const uint32 queryId, const TCHAR* query)
{
	Trace( "queryId(%lu) - %s\r\n", queryId, query );
	QUERY_TABLE::const_iterator it = m_queryTable.find(queryId);
	if(it != m_queryTable.end())
	{
		return FALSE;
	}

	QueryInfo* queryInfo = new QueryInfo;
	if(queryInfo)
	{
		queryInfo->query = query;
		m_queryTable[queryId] = queryInfo;
		return TRUE;
	}
	return FALSE;
}

BOOL cQueryTable::AddFactor(const uint32 queryId, const uint32 factor)
{
	QUERY_TABLE::const_iterator it = m_queryTable.find(queryId);
	if(it != m_queryTable.end())
	{
		QueryInfo* queryInfo = it->second;
		queryInfo->factors.push_back(factor);
		return TRUE;
	}
	return FALSE;
}

BOOL cQueryTable::GetQuery(const uint32 queryId, QUERY& query)
{
	QUERY_TABLE::const_iterator it = m_queryTable.find(queryId);
	if(it != m_queryTable.end())
	{
		QueryInfo* queryInfo = it->second;
		if(queryInfo)
		{
			query = queryInfo->query;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL cQueryTable::GetFactors(const uint32 queryId, FACTORS& factors)
{
	QUERY_TABLE::const_iterator it = m_queryTable.find(queryId);
	if(it != m_queryTable.end())
	{
		QueryInfo* queryInfo = it->second;
		factors = queryInfo->factors;
		return TRUE;
	}
	return FALSE;
}