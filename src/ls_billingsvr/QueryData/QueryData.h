// QueryData.h: interface for the CQueryData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYDATA_H__BC410DEE_2BBD_424A_A266_499332FBEE59__INCLUDED_)
#define AFX_QUERYDATA_H__BC410DEE_2BBD_424A_A266_499332FBEE59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


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

typedef struct tagValueType
{
	VariableType type;			// 어떤 타입의 데이터인가
	int size;					// 사이즈는?
	tagValueType()
	{
		type = vWrong;
		size = 0;
	}
}ValueType;
typedef std::vector<ValueType> vVALUETYPE;

// CQuery의 헤더
typedef struct tagQueryHeader
{
	int nMsgType;				// 메세지 타입,	DB서버에서 사용하기보다 나중에 게임서버가 결과를 받아서 처리할때 필요
	int nQueryType;				// 쿼리의 타입 (insert = 0, delete = 1, select = 2, update = 3)
	int nQuerySize;				// 쿼리 부분 사이즈
	int nValueTypeCnt;			// 결과값의 갯수
	int nReturnValueSize;		// 다시 되돌려 받아야 하는 데이터의 사이즈
	int nSetValue;				// MoveNext : 한테이블에 몇개의 변수?
	int nQueryBufferSize;       // 버퍼의 사이즈 (실제 데이터)
	unsigned int csocket;       // 소켓.
	int nResult_Type;           // 결과 에러시 행동.
	tagQueryHeader()
	{
		nMsgType		= -1;
		nQueryType		= -1;
		nQuerySize		= -1;
		nValueTypeCnt	= -1;
		nReturnValueSize= -1;
		nSetValue       = -1;
		nQueryBufferSize= -1;
		csocket         = 0;
		nResult_Type    = 0;
	}
}QueryHeader;

class CQuery
{
protected:
	QueryHeader	m_queryHeader;					// 쿼리헤더
	char		*m_pBuffer;		// 쿼리내용, 결과데이터타입들, 다시 돌려받아야할 데이터
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
	int GetMsgType(){ return m_queryHeader.nMsgType; }
	int GetQueryType(){ return m_queryHeader.nQueryType; }
	int GetQuerySize(){ return m_queryHeader.nQuerySize; }
	int GetValueCnt(){ return m_queryHeader.nValueTypeCnt; }
	int GetReturnValueCnt() {return m_queryHeader.nReturnValueSize; }
	int GetSetValue() { return m_queryHeader.nSetValue; }
	int GetBufferSize(){ return m_queryHeader.nQueryBufferSize; }
	unsigned int GetSocket(){ return m_queryHeader.csocket; }
	int GetResultType(){ return m_queryHeader.nResult_Type; }
	//DATA
	ValueType GetValueType(int nPos);
	char *GetQuery(int &nLen);
	QueryHeader *GetHeader(){ return &m_queryHeader; }
	char *GetBuffer(){ return m_pBuffer; }
	
public:
	//SET
	void SetData(unsigned int socket,int nResult_Type,int nMsgType, int nQueryType,
				 char *szQuery, int nQuerySize, ValueType* pTypes, int nTypeCount, int nSetValue = 1);
	void SetBuffer(const char *buffer);
	
public:
	//편의를 위해 만든 함수.
	//ResultMessage를 만들때 버퍼의 사이즈 및 NEW를 할당하기 위한 함수.
	char *NewResultBuffer(int &nSize);
	//유저에게 꼭돌려줜야할 데이터의 포인터와 사이즈 반환.
	//뽑은 데이터가 아니니 참고.
	char *GetReturnResultData(int &nSize);

protected:    //돌려받기 위해 저장하는 데이터
	char m_szReturnBuf[MAX_BUFFER];
	int  m_iReturnCount;
public:
	void SetReturnData( const void *pData, int iSize );

	
public:
	CQueryData();
	virtual ~CQueryData();
};


#endif // !defined(AFX_QUERYDATA_H__BC410DEE_2BBD_424A_A266_499332FBEE59__INCLUDED_)
