// QueryResultData.cpp: implementation of the QueryResultData class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "QueryResultData.h"


extern CLog LOG;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryResultData::CQueryResultData()
{
	m_pBuffer = NULL;
	CQueryResult::Clear();
	InitPos();
}
CQueryResultData::~CQueryResultData()
{
	CQueryResult::Clear();
}

bool CQueryResultData::IsExist() 
{
	if( m_nValuePos < m_queryResultHeader.nResultBufferSize ) 
		return true;
	return false;
}

void CQueryResultData::GetValue(bool &value,int len)
{
	if(m_nValuePos + len > m_queryResultHeader.nResultBufferSize)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer OVER!!");
		return;
	}
	if(m_pBuffer == NULL)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer NULL POINTER!!");
		return;
	}
	memcpy(&value,&m_pBuffer[m_nValuePos],len);
	m_nValuePos += len;
}

void CQueryResultData::GetValue(int &value,int len)
{
	if(m_nValuePos + len > m_queryResultHeader.nResultBufferSize)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer OVER!!");
		return;
	}
	if(m_pBuffer == NULL)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer NULL POINTER!!");
		return;
	}
	memcpy(&value,&m_pBuffer[m_nValuePos],len);
	m_nValuePos += len;
}

void CQueryResultData::GetValue(long &value,int len)
{
	if(m_nValuePos + len > m_queryResultHeader.nResultBufferSize)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer OVER!!");
		return;
	}
	if(m_pBuffer == NULL)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer NULL POINTER!!");
		return;
	}
	memcpy(&value,&m_pBuffer[m_nValuePos],len);
	m_nValuePos += len;
}

void CQueryResultData::GetValue(DWORD &value,int len)
{
	if(m_nValuePos + len > m_queryResultHeader.nResultBufferSize)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer OVER!!");
		return;
	}
	if(m_pBuffer == NULL)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer NULL POINTER!!");
		return;
	}
	memcpy(&value,&m_pBuffer[m_nValuePos],len);
	m_nValuePos += len;
}

void CQueryResultData::GetValue(char *value,int len)
{
	if((m_nValuePos + len) > m_queryResultHeader.nResultBufferSize)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer OVER!!");
		return;
	}
	if(m_pBuffer == NULL)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer NULL POINTER!!");
		return;
	}
	memcpy(value,&m_pBuffer[m_nValuePos],len);
	m_nValuePos += len;
}

void CQueryResultData::GetValue(__int64 &value,int len)
{
	if(m_nValuePos + len > m_queryResultHeader.nResultBufferSize)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer OVER!!");
		return;
	}
	if(m_pBuffer == NULL)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer NULL POINTER!!");
		return;
	}
	memcpy(&value,&m_pBuffer[m_nValuePos],len);
	m_nValuePos += len;
}

void CQueryResultData::GetValue(short &value,int len)
{
	if(m_nValuePos + len > m_queryResultHeader.nResultBufferSize)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer OVER!!");
		return;
	}
	if(m_pBuffer == NULL)
	{
		LOG.PrintTimeAndLog(0,"Result Buffer NULL POINTER!!");
		return;
	}
	memcpy(&value,&m_pBuffer[m_nValuePos],len);
	m_nValuePos += len;
}

void CQueryResultData::SetBuffer(const char *buffer)
{
	memcpy(&m_queryResultHeader,buffer,sizeof(QueryResultHeader));
	buffer += sizeof(QueryResultHeader);
	m_pBuffer = new char[m_queryResultHeader.nResultBufferSize];
	memset(m_pBuffer,0,m_queryResultHeader.nResultBufferSize);
	memcpy(m_pBuffer,buffer,m_queryResultHeader.nResultBufferSize);
}

void CQueryResultData::SetResultData(unsigned int socket,int nMsgType, int nResultSize,
									 int nResultType,int SetValueCnt,
									 char *pResultData)
{
	CQueryResult::Clear();
	m_queryResultHeader.csocket          = socket;
	m_queryResultHeader.nMsgType		 = nMsgType;
	m_queryResultHeader.nQueryResultType = nResultType;
	m_queryResultHeader.SetValueCnt      = SetValueCnt;
	m_queryResultHeader.nResultBufferSize= nResultSize;
	
	if(nResultSize != 0)
	{
		m_pBuffer = new char[nResultSize];
		memset(m_pBuffer,0,nResultSize);
		memcpy(m_pBuffer,pResultData,sizeof(char) * nResultSize);
	}
	else 
		m_pBuffer = NULL;
}
