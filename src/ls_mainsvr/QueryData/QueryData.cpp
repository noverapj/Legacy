// QueryData.cpp: implementation of the CQueryData class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "../util/cSerialize.h"
#include "QueryData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryData::CQueryData() : m_Buffer(NULL), m_Return(NULL)
{
	m_Buffer = new cBuffer(16);
	m_Return = new cBuffer(8096);
}

CQueryData::~CQueryData()
{
	if(m_Buffer)
	{
		delete m_Buffer;
		m_Buffer = NULL;
	}
	if(m_Return)
	{
		delete m_Return;
		m_Return = NULL;
	}
}

void CQueryData::Clear()
{
	m_Header.Clear();
	m_Buffer->Erase();
	m_Return->Erase();
}

void CQueryData::Copy( CQueryData &queryData )
{
	m_Buffer->Resize(queryData.m_Header.nQueryBufferSize);

	m_Header = queryData.m_Header;
	m_Buffer->Copy((const uint8*)queryData.GetBuffer(), m_Header.nQueryBufferSize );
}

BOOL CQueryData::Deserialize( const char *buffer )
{
	memcpy(&m_Header,buffer,sizeof(QueryHeader));
	if(m_Header.nQueryBufferSize > G_MAX_QUERY)
	{
		return FALSE;
	}

	buffer += sizeof(QueryHeader);

	m_Buffer->Resize(m_Header.nQueryBufferSize);
	m_Buffer->Copy((const uint8*)buffer, m_Header.nQueryBufferSize);
	return TRUE;
}

void CQueryData::GetFields(cSerialize& fieldTypes)
{
	if(GetFieldSize() == 0) 
		return;

	if(m_Buffer->GetLength() > 0)
	{
		uint32 index = sizeof(int);

		fieldTypes.Reset();
		fieldTypes.SetBuffer( m_Buffer->GetBuffer(index), GetFieldSize() );
	}
}

void CQueryData::GetResults(vVALUETYPE& valueTypes)
{
	valueTypes.clear();

	if(GetResultSize() == 0) 
		return;

	if(m_Buffer->GetLength() > 0)
	{
		uint32 index = sizeof(int) + GetFieldSize();
		for(int32 n = 0 ; n < GetResultSize() ; n += sizeof(ValueType) )
		{
			ValueType* type = (ValueType*)(m_Buffer->GetBuffer(index + n));
			valueTypes.push_back( *type );
		}
	}
}

void CQueryData::GetReturns(char* pBuffer, int &nSize)
{
	if(m_Buffer->GetLength() > 0)
	{
		uint32 nOffset = sizeof(int) + GetFieldSize() + GetResultSize();
		nSize = GetReturnSize();

		memcpy( pBuffer, m_Buffer->GetBuffer(nOffset), nSize );
	}
}

void CQueryData::SetData(
						 unsigned int nIndex,
						 int nResultType,
						 int nMsgType, 
						 int nQueryType,
						 int nQueryID,
						 cSerialize& fieldTypes,
						 vVALUETYPE& valueTypes)
{
	//헤더
	m_Header.Clear();
	m_Header.nIndex				= nIndex;
	m_Header.nResultType		= nResultType;
	m_Header.nMsgType			= nMsgType;
	m_Header.nQueryType			= nQueryType;
	m_Header.nFieldLength		= fieldTypes.GetLength();
	m_Header.nResultLength		= sizeof(ValueType) * valueTypes.size();
	m_Header.nValueTypeCnt		= 0;
	m_Header.nReturnLength		= m_Return->GetLength();
	m_Header.nQueryID			= nQueryID;
	m_Header.nDatabaseID		= 1;
	m_Header.nQueryBufferSize	= sizeof(nQueryID) + m_Header.nFieldLength + m_Header.nResultLength + m_Header.nReturnLength;	

	// 버퍼 메모리 할당
	if(m_Header.nQueryBufferSize != 0)
	{
		m_Buffer->Erase();
		m_Buffer->Resize(m_Header.nQueryBufferSize);

		if(0 != nQueryID)
		{
			m_Buffer->Append((const uint8*)(&nQueryID), sizeof(nQueryID));				// 쿼리 내용
		}
		if(m_Header.nFieldLength > 0)
		{
			m_Buffer->Append(fieldTypes.GetBuffer(), m_Header.nFieldLength);				// 필드 타입
		}
		if(m_Header.nResultLength > 0)
		{
			m_Buffer->Append((const uint8*)(&valueTypes[0]), m_Header.nResultLength);	// 결과 타입
		}
		if(m_Header.nReturnLength > 0)
		{
			m_Buffer->Append(m_Return->GetBuffer(), m_Return->GetLength());				// 돌려받을 데이터
		}
	}
}

void CQueryData::SetReturnData( const void *pData, int nSize )
{
	int nLength = m_Return->GetLength() + nSize;
	if(nLength > m_Return->GetMaxLength())
	{
		LOG.PrintTimeAndLog( 0, "CQueryData::SetReturnData overflow : %d", nLength );
		return ;
	}

	m_Return->Append( (const uint8*)pData, nSize );
	if(m_Return->GetLength() >= MAX_BUFFER)
	{
		LOG.PrintTimeAndLog( 0, "CQueryData::SetReturnData Size : %d", m_Return->GetLength() );
	}
}
