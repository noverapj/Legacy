// QueryData.cpp: implementation of the CQueryData class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "../util/cSerialize.h"
#include "QueryData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryData::CQueryData()
{
	memset( m_szReturnBuf, 0, sizeof( m_szReturnBuf ) );
	m_iReturnLength = 0;

	m_pBuffer = NULL;
	CQuery::Clear();
}

CQueryData::~CQueryData()
{
	CQuery::Clear();
}

void CQueryData::GetFields(cSerialize& fieldTypes)
{
	if(GetFieldSize() == 0) 
		return;

	if(m_pBuffer)
	{
		uint32 index = sizeof(int);

		fieldTypes.Reset();
		fieldTypes.SetBuffer( (uint8*)(m_pBuffer + index), GetFieldSize() );
	}
}

void CQueryData::GetResults(vVALUETYPE& valueTypes)
{
	valueTypes.clear();

	if(GetResultSize() == 0) 
		return;

	if(m_pBuffer)
	{
		uint32 index = sizeof(int) + GetFieldSize();
		for(int32 n = 0 ; n < GetResultSize() ; n += sizeof(ValueType) )
		{
			ValueType* type = (ValueType*)(m_pBuffer + index + n);
			valueTypes.push_back( *type );
		}
	}
}

void CQueryData::GetReturns(char* buffer, int &size)
{
	if(m_pBuffer)
	{
		uint32 index = sizeof(int) + GetFieldSize() + GetResultSize();
		size = GetReturnSize();
		memcpy( buffer, m_pBuffer + index, size );
	}
}

void CQueryData::SetReturnData( const void *pData, int iSize )
{
	memcpy( &m_szReturnBuf[m_iReturnLength], pData, iSize );
	m_iReturnLength += iSize;

	if( m_iReturnLength >= MAX_BUFFER )
		LOG.PrintTimeAndLog( 0, "CQueryData::SetReturnData Return Size : %d", m_iReturnLength );
}

void CQueryData::SetBuffer(const char *buffer)
{
	memcpy(&m_queryHeader,buffer,sizeof(QueryHeader));
	buffer+= sizeof(QueryHeader);
	m_pBuffer = new char[m_queryHeader.nQueryBufferSize];
	memcpy(m_pBuffer,buffer,m_queryHeader.nQueryBufferSize);		
}

void CQueryData::SetData(
						 unsigned int nIndex,
						 int nResult_Type,
						 int nMsgType, 
						 int nQueryType,
						 int queryId,
						 cSerialize& fieldTypes,
						 vVALUETYPE& valueTypes, 
						 int nSetValue)
{
	CQuery::Clear();
	//헤더
	m_queryHeader.nIndex            = nIndex;
	m_queryHeader.nResult_Type      = nResult_Type;
	m_queryHeader.nMsgType			= nMsgType;
	m_queryHeader.nQueryType		= nQueryType;
	m_queryHeader.fieldLength		= fieldTypes.GetLength();
	m_queryHeader.resultLength		= sizeof(ValueType) * valueTypes.size();
	m_queryHeader.nValueTypeCnt		= 0;
	m_queryHeader.returnLength		= m_iReturnLength;
	m_queryHeader.nSetValue         = nSetValue;
	m_queryHeader.queryId			= queryId;
	m_queryHeader.databaseId		= 1;
	m_queryHeader.nQueryBufferSize  = sizeof(queryId) + m_queryHeader.fieldLength + m_queryHeader.resultLength + m_queryHeader.returnLength;	

	//버퍼 메모리 할당.
	if(m_queryHeader.nQueryBufferSize != 0)
	{
		m_pBuffer = new char[m_queryHeader.nQueryBufferSize];
		//데이터.
		int nSize = 0;                     //버퍼의 위치
		int nlen = 0;
		if(0 != queryId)
		{
			nlen = sizeof(queryId);        //카피할 데이터 사이즈
			memcpy(&m_pBuffer[nSize], &queryId, nlen);					 //쿼리 내용
		}
		if(fieldTypes.GetLength() > 0)
		{
			nSize += nlen;                      
			nlen = m_queryHeader.fieldLength;
			memcpy(&m_pBuffer[nSize], fieldTypes.GetBuffer(), nlen);      // 필드 타입
		}
		if(valueTypes.size() > 0)
		{
			nSize += nlen;                      
			nlen = m_queryHeader.resultLength;
			memcpy(&m_pBuffer[nSize], &valueTypes[0], nlen);      // 결과 타입
		}
		if(m_iReturnLength > 0)
		{
			nSize += nlen;
			memcpy(&m_pBuffer[nSize], m_szReturnBuf, m_iReturnLength);   //돌려받을 데이터.
		}
	}
}

void CQueryData::Copy( CQueryData &queryData )
{
	if(m_pBuffer)
	{
		delete []m_pBuffer;
	}

	m_pBuffer = new char[queryData.GetBufferSize()];
	if(m_pBuffer)
	{
		m_queryHeader = queryData.m_queryHeader;
		memcpy( m_pBuffer, queryData.m_pBuffer, m_queryHeader.nQueryBufferSize );
	}
}
