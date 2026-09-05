#pragma once

struct QueryResultHeader
{
	QueryResultHeader()
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

class CQueryResult
{
protected:
	QueryResultHeader m_queryResultHeader;
	char *m_pBuffer;

public:
	CQueryResult() : m_pBuffer(NULL)
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_queryResultHeader,0,sizeof(QueryResultHeader));
		if(m_pBuffer != NULL)
		{
			delete[] m_pBuffer;
			m_pBuffer = NULL;
		}
	}
};

class CQueryResultData : public CQueryResult
{
	int m_nValuePos;
public:
	//HEADER
	int GetMsgType()			{ return m_queryResultHeader.nMsgType; }
	int GetResultType()			{ return m_queryResultHeader.nQueryResultType; }
	int GetResultBufferSize()	{ return m_queryResultHeader.nResultBufferSize; }
	int GetResultCount()		{ return m_queryResultHeader.nResultCount; }
	unsigned int GetIndex()		{ return m_queryResultHeader.nIndex; }

	//DATA
	BOOL IsValid(const int nLength);
	BOOL GetValue(bool &value,int nLength);
	BOOL GetValue(int &value,int nLength);
	BOOL GetValue(long &value,int nLength);
	BOOL GetValue(DWORD &value,int nLength);
	BOOL GetValue(char *value,int nLength);
	BOOL GetValue(__int64 &value,int nLength);
	BOOL GetValue(short &value,int nLength);
	BOOL GetValue(WORD &value, int nLength);
	BOOL GetValue(ioHashString &value,int nLength);
	BOOL GetValue( BYTE &value, int nLength );

	BOOL IsExist();

	QueryResultHeader *GetHeader(){ return &m_queryResultHeader; }
	char *GetBuffer(){ return m_pBuffer; }
	
public://SET
	void SetResultData(
		unsigned int nIndex,
		int nMsgType,
		int nResultType,
		char *pResultData, 
		int nResultSize,
		int nResultCount);

	void SetBuffer(const char *buffer);

public:
	CQueryResultData();
	virtual ~CQueryResultData();
};
