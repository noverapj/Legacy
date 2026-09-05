#pragma once

#include "../util/cBuffer.h"

const int G_MAX_QUERY = 1024 * 31;

class cSerialize;

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

// 쿼리헤더
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
		nQueryBufferSize	= -1;
		nIndex				= 0;
		nResultType			= 0;
		nQueryID			= 0;
		nDatabaseID			= 0;
	}

	int nMsgType;				// 메세지 타입,	DB서버에서 사용하기보다 나중에 게임서버가 결과를 받아서 처리할때 필요
	int nQueryType;				// 쿼리의 타입 (insert = 0, delete = 1, select = 2, update = 3)
	int nFieldLength;			// 쿼리 부분 사이즈
	int nResultLength;			// 결과 부분 사이즈
	int nReturnLength;			// 다시 되돌려 받아야 하는 데이터의 사이즈
	int nValueTypeCnt;			// 결과값의 갯수
	int nSetValue;				// 미사용
	int nQueryBufferSize;       // 버퍼의 사이즈 (실제 데이터)
	unsigned int nIndex;        // 쿼리 주인의 고유 인덱스.
	int nResultType;			// 결과 에러시 행동.
	int nQueryID;				// 실행하고자 하는 쿼리번호
	int nDatabaseID;			// 디비구분
};

class CQueryData 
{
public:
	CQueryData();
	virtual ~CQueryData();

public:
	void Clear();
	void Copy( CQueryData &queryData );

	BOOL Deserialize(const char *buffer);

public: 
	int GetMsgType()			{ return m_Header.nMsgType; }
	int GetQueryType()			{ return m_Header.nQueryType; }
	int GetFieldSize()			{ return m_Header.nFieldLength; }
	int GetResultSize()			{ return m_Header.nResultLength; }
	int GetValueCnt()			{ return m_Header.nValueTypeCnt; }
	int GetReturnSize()			{ return m_Header.nReturnLength; }
	int GetBufferSize()			{ return m_Header.nQueryBufferSize; }
	unsigned int GetIndex()		{ return m_Header.nIndex; }
	int GetResultType()			{ return m_Header.nResultType; }
	int GetQueryID()			{ return m_Header.nQueryID; }
	int GetDatabaseID()			{ return m_Header.nDatabaseID; }

	QueryHeader *GetHeader()	{ return &m_Header; }
	char *GetBuffer()			{ return m_Buffer->GetString(); }
	
	void GetFields(cSerialize& fieldTypes);
	void GetResults(vVALUETYPE& valueTypes);
	void GetReturns(char* pBuffer, int &nSize);

public:
	void SetData(
		unsigned int nIndex,
		int nResultType,
		int nMsgType, 
		int nQueryType,
		int nQueryID, 
		cSerialize& fieldTypes,
		vVALUETYPE& valueTypes);
	void SetReturnData( const void *pData, int nSize );

	void ResizeReturnData( uint32 length )	{ m_Return->Resize( length ); }

protected:
	QueryHeader	m_Header;

	cBuffer *m_Buffer;
	cBuffer *m_Return;
};
