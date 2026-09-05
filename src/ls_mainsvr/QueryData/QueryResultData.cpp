// QueryResultData.cpp: implementation of the QueryResultData class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "QueryResultData.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryResultData::CQueryResultData() : m_Buffer(NULL)
{
	m_Buffer = new cSerialize(64);
}

CQueryResultData::~CQueryResultData()
{
	Clear();

	if(m_Buffer)
	{
		delete m_Buffer;
		m_Buffer = NULL;
	}
}

void CQueryResultData::Clear()
{
	m_Header.Clear();
}

BOOL CQueryResultData::Deserialize(const char *buffer)
{
	memcpy(&m_Header, buffer, sizeof(QueryResultHeader));
	buffer += sizeof(QueryResultHeader);

	if(m_Buffer)
	{
		m_Buffer->Resize(m_Header.nResultBufferSize);
		m_Buffer->Write(buffer, m_Header.nResultBufferSize, FALSE);
		m_Buffer->SetBuffer(m_Buffer->GetBuffer(), m_Buffer->GetLength());
		return TRUE;
	}
	return FALSE;
}

BOOL CQueryResultData::IsExist() 
{
	if( m_Buffer && (m_Buffer->GetOffset() < m_Header.nResultBufferSize) ) 
		return TRUE;
	return FALSE;
}

BOOL CQueryResultData::GetValue(bool &value)
{
	if(!m_Buffer || !m_Buffer->GetBool(value))
	{
		LOG.PrintTimeAndLog( 0,"CQueryResultData::Buffer overflow[bool,0x%x]", GetMsgType());
		return FALSE;
	}
	return TRUE;
}

BOOL CQueryResultData::GetValue(BYTE &value)
{
	if(!m_Buffer || !m_Buffer->GetInt(value))
	{
		LOG.PrintTimeAndLog( 0,"CQueryResultData::Buffer overflow[BYTE,0x%x]", GetMsgType());
		return FALSE;
	}
	return TRUE;
}

BOOL CQueryResultData::GetValue(short &value)
{
	if(!m_Buffer || !m_Buffer->GetInt(value))
	{
		LOG.PrintTimeAndLog( 0,"CQueryResultData::Buffer overflow[short,0x%x]", GetMsgType());
		return FALSE;
	}
	return TRUE;
}

BOOL CQueryResultData::GetValue(int &value)
{
	if(!m_Buffer || !m_Buffer->GetInt(value))
	{
		LOG.PrintTimeAndLog( 0,"CQueryResultData::Buffer overflow[int,0x%x]", GetMsgType());
		return FALSE;
	}
	return TRUE;
}

BOOL CQueryResultData::GetValue(long &value)
{
	if(!m_Buffer || !m_Buffer->GetInt(value))
	{
		LOG.PrintTimeAndLog( 0,"CQueryResultData::Buffer overflow[long,0x%x]", GetMsgType());
		return FALSE;
	}
	return TRUE;
}

BOOL CQueryResultData::GetValue(DWORD &value)
{
	if(!m_Buffer || !m_Buffer->GetInt(value))
	{
		LOG.PrintTimeAndLog( 0,"CQueryResultData::Buffer overflow[DWORD,0x%x]", GetMsgType());
		return FALSE;
	}
	return TRUE;
}

BOOL CQueryResultData::GetValue(__int64 &value)
{
	if(!m_Buffer || !m_Buffer->GetInt(value))
	{
		LOG.PrintTimeAndLog( 0,"CQueryResultData::Buffer overflow[int64,0x%x]", GetMsgType());
		return FALSE;
	}
	return TRUE;
}

BOOL CQueryResultData::GetValue(char *value, int nLength)
{
	if(!m_Buffer || !m_Buffer->GetBufferF(value, nLength))
	{
		LOG.PrintTimeAndLog( 0,"CQueryResultData::Buffer overflow[char,0x%x]", GetMsgType());
		return FALSE;
	}
	return TRUE;
}

BOOL CQueryResultData::GetValue(ioHashString &value, int nLength)
{
	static char szBuf[4096] = "";
	if( (nLength < 4095) && GetValue(szBuf, nLength) )
	{
		szBuf[nLength] = NULL;
		value = szBuf;
		return TRUE;
	}	

	LOG.PrintTimeAndLog( 0,"CQueryResultData::Buffer overflow[ioHashString,0x%x]", GetMsgType());
	return FALSE;
}

void CQueryResultData::SetResultData(
									 unsigned int nIndex,
									 int nMsgType,
									 int nResultType,
									 char *pResultData, 
									 int nResultSize,
									 int nResultCount )
{
	Clear();

	m_Header.nIndex				= nIndex;
	m_Header.nMsgType			= nMsgType;
	m_Header.nQueryResultType	= nResultType;
	m_Header.nResultBufferSize	= nResultSize;
	m_Header.nResultCount		= nResultCount;
	
	if((nResultSize != 0) && m_Buffer)
	{
		m_Buffer->Reset();
		m_Buffer->Resize( nResultSize );
		m_Buffer->Write( pResultData, nResultSize );
	}
}
