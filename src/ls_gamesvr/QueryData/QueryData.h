#pragma once

// 변수의 타입과 사이즈
enum VariableType 
{ 
	vChar = 100,
	vWChar,
	vTimeStamp,
	vLONG,
	vINT64,
	vSHORT,
	vWrong
};

struct ValueType
{
	VariableType type;			// 어떤 타입의 데이터인가
	int size;					// 사이즈는?
	ValueType()
	{
		type = vWrong;
		size = 0;
	}
};

typedef vector<ValueType> vVALUETYPE;

class cSerialize;

// CQuery의 헤더
struct QueryHeader
{
	QueryHeader()
	{
		Clear();
	}
	void Clear()
	{
		nMsgType			= -1;
		nQueryType			= -1;
		nFieldLength		= 0;
		nResultLength		= 0;
		nReturnLength		= -1;
		nValueTypeCnt		= -1;
		nSetValue		 = -1;
		nQueryBufferSize	= -1;
		nIndex				= 0;
		nResultType		= 0;
		nQueryId			= 0;
		nDatabaseId			= 0;
	}
	int nMsgType;				// 메세지 타입,	DB서버에서 사용하기보다 나중에 게임서버가 결과를 받아서 처리할때 필요
	int nQueryType;				// 쿼리의 타입 (insert = 0, delete = 1, select = 2, update = 3)
	int nFieldLength;			// 쿼리 부분 사이즈
	int nResultLength;			// 결과 부분 사이즈
	int nReturnLength;			// 다시 되돌려 받아야 하는 데이터의 사이즈
	int nValueTypeCnt;			// 결과값의 갯수
	int nSetValue;				// MoveNext : 한테이블에 몇개의 변수?
	int nQueryBufferSize;       // 버퍼의 사이즈 (실제 데이터)
	unsigned int nIndex;        // 쿼리 주인의 고유 인덱스.
	int nResultType;           // 결과 에러시 행동.
	int nQueryId;				// 실행하고자 하는 쿼리번호
	int nDatabaseId;				// 디비구분
};

class CQuery
{
protected:
	QueryHeader	m_queryHeader;	// 쿼리헤더
	char *m_pBuffer;			// 쿼리내용, 결과데이터타입들, 다시 돌려받아야할 데이터
	//cBuffer *m_Return;
public:
	void Clear()
	{
		memset(&m_queryHeader,0,sizeof(QueryHeader));
		if(m_pBuffer != NULL)
		{
			delete[] m_pBuffer;
			m_pBuffer = NULL;
		}
	}
};

class CQueryData : public CQuery  
{
public://GET 
	//HEADER
	int GetMsgType()		{ return m_queryHeader.nMsgType; }
	int GetQueryType()		{ return m_queryHeader.nQueryType; }
	int GetFieldSize()		{ return m_queryHeader.nFieldLength; }
	int GetResultSize()		{ return m_queryHeader.nResultLength; }
	int GetValueCnt()		{ return m_queryHeader.nValueTypeCnt; }
	int GetReturnSize()		{ return m_queryHeader.nReturnLength; }
	int GetSetValue()		{ return m_queryHeader.nSetValue; }
	int GetBufferSize()		{ return m_queryHeader.nQueryBufferSize; }
	unsigned int GetIndex()	{ return m_queryHeader.nIndex; }
	int GetResultType()		{ return m_queryHeader.nResultType; }
	int GetQueryID()		{ return m_queryHeader.nQueryId; }
	int GetDatabaseID()		{ return m_queryHeader.nDatabaseId; }

	//DATA
	QueryHeader *GetHeader(){ return &m_queryHeader; }
	char *GetBuffer()		{ return m_pBuffer; }
	
	void GetFields(cSerialize& fieldTypes);
	void GetResults(vVALUETYPE& valueTypes);
	void GetReturns(char* buffer, int &size);

public:
	//SET
	void SetData(
		unsigned int nIndex,
		int nResultType,
		int nMsgType, 
		int nQueryType,
		int nQueryId, 
		cSerialize& fieldTypes,
		vVALUETYPE& valueTypes, 
		int nSetValue = 1);
	void SetData(
		unsigned int nIndex,
		int nMsgType, 
		int nQueryID, 
		cSerialize& fieldTypes,
		int DBID = 1);
	void SetBuffer(const char *buffer);

protected:
	//돌려받기 위해 저장하는 데이터
	char m_szReturnBuf[MAX_BUFFER];
	int  m_iReturnLength;

public:
	void SetReturnData( const void *pData, int iSize );
	void Copy( CQueryData &queryData );

public:
	CQueryData();
	virtual ~CQueryData();
};
