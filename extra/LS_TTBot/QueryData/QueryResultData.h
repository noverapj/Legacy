#pragma once

struct QueryResultHeader
{
	QueryResultHeader()
	{
		nMsgType			= -1;
		nQueryResultType	= -1;
		nResultBufferSize	= 0;
		resultCount			= 0;
		nIndex				= 0;
	}
	int nMsgType;
	int nQueryResultType;
	int nResultBufferSize;
	int resultCount;
	unsigned int nIndex;
};

class CQueryResult
{
protected:
	QueryResultHeader m_queryResultHeader;
	char *m_pBuffer;

public:
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
	int GetResultCount()		{ return m_queryResultHeader.resultCount; }
	unsigned int GetIndex()		{ return m_queryResultHeader.nIndex; }

	//DATA
	void GetValue(bool &vlaue,int len);
	void GetValue(int &vlaue,int len);
	void GetValue(long &vlaue,int len);
	void GetValue(DWORD &vlaue,int len);
	void GetValue(char *vlaue,int len);
	void GetValue(__int64 &value,int len);
	void GetValue(short &value,int len);
	void GetValue(ioHashString &value,int len);
	void GetValue( BYTE &value, int len );

	bool IsExist();

	QueryResultHeader *GetHeader(){ return &m_queryResultHeader; }
	char *GetBuffer(){ return m_pBuffer; }
	
public://SET
	void SetResultData(
		unsigned int nIndex,
		int nMsgType,
		int nResultType,
		char *pResultData, 
		int nResultSize,
		int resultCount);
	void SetBuffer(const char *buffer);
	void SetBuffer(uint8 *buffer);
	
public:
	void InitPos(){ m_nValuePos = 0; }

public:
	CQueryResultData();
	virtual ~CQueryResultData();
};
