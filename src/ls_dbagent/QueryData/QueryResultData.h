#pragma once

#include "../util/cSerialize.h"

// 쿼리결과 헤더
struct QueryResultHeader
{
	QueryResultHeader()
	{
		Clear();
	}
	void Clear()
	{
		nMsgType			= -1;
		nQueryResultType	= -1;
		nResultBufferSize	= 0;
		nResultCount		= 0;
		nIndex				= 0;
	}

	int nMsgType;
	int nQueryResultType;
	int nResultBufferSize;
	int nResultCount;
	unsigned int nIndex;
};

class CQueryResultData
{
public:
	CQueryResultData();
	virtual ~CQueryResultData();

	void Clear();
	BOOL Deserialize(const char *buffer);

public:
	int GetMsgType()			{ return m_Header.nMsgType; }
	int GetResultType()			{ return m_Header.nQueryResultType; }
	int GetResultBufferSize()	{ return m_Header.nResultBufferSize; }
	int GetResultCount()		{ return m_Header.nResultCount; }
	unsigned int GetIndex()		{ return m_Header.nIndex; }

	BOOL IsExist();

	BOOL GetValue(bool &value);
	BOOL GetValue(BYTE &value);
	BOOL GetValue(short &value);
	BOOL GetValue(int &value);
	BOOL GetValue(long &value);
	BOOL GetValue(DWORD &value);
	BOOL GetValue(__int64 &value);
	BOOL GetValue(char *value, int nLength);
	BOOL GetValue(ioHashString &value, int nLength);

	QueryResultHeader *GetHeader()	{ return &m_Header; }
	char *GetBuffer()				{ return (char*)m_Buffer->GetBuffer(); }
	
public:
	void SetResultData(
		unsigned int nIndex,
		int nMsgType,
		int nResultType,
		char *pResultData, 
		int nResultSize,
		int nResultCount);

protected:
	QueryResultHeader m_Header;
	cSerialize *m_Buffer;

};
