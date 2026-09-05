#pragma once

#define ID_NUMBER	20
#define REQUEST_SYSTEM_INFO	0xf600

class CREQUEST_SYSTEM_INFO :  public CGenericMessage
{
public:
	void Set();
	void Process();
};

#define REQUEST_COPY 0xf601

#define COPY_REQUEST_ALL  1
#define COPY_REQUEST_CS3  2

class CREQUEST_COPY : public CGenericMessage
{
public:
	int m_iType;
	int m_iVersion;
public:
	void Set( int iType, int iVersion )
	{
		SetBasicHeader( sizeof(CREQUEST_COPY), REQUEST_COPY );
		m_iType    = iType;
		m_iVersion = iVersion;
	}
	void Process();
};

#define REQUEST_DELETE 0xf602

#define DELETE_REQUEST_ALL 1
#define DELETE_REQUEST_CS3 2

class CREQUEST_DELETE : public CGenericMessage
{
public:
	int m_iType;
	int m_iVersion;
public:
	void Set( int iType, int iVersion )
	{
		SetBasicHeader( sizeof(CREQUEST_DELETE), REQUEST_DELETE );
		m_iType    = iType;
		m_iVersion = iVersion;
	}
	void Process();
};

#define ANSWER_SYSTEM_INFO	0xf700

class CANSWER_SYSTEM_INFO : public CGenericMessage
{
public:
	int		m_cpu;
	int		m_memory;
	double	m_cmemory;
	int     m_c_hdd;
	int     m_d_hdd;
public:
	void Set(int cpu, int memory, double cmemory, int c_hdd, int d_hdd);
	void Process();
};

#define ANSWER_COPY 0xf701
#define COPY_ANSWER_SUCCESS            1
#define COPY_ANSWER_FAIL               2
#define COPY_ANSWER_NOT_CS3            3
#define COPY_ANSWER_FAIL_WRONG_VERSION 4

class CANSWER_COPY : public CGenericMessage
{
	int m_iRequestType;
	int m_iType;
	int m_iVersion;
public:
	void Set( int iType, int iRequestType, int iVersion )
	{
		SetBasicHeader( sizeof( CANSWER_COPY ), ANSWER_COPY );
		m_iRequestType = iRequestType;
		m_iType        = iType;
		m_iVersion     = iVersion;
	};
	void Process();
};


#define ANSWER_DELETE 0xf702
#define DELETE_ANSWER_SUCCESS            1
#define DELETE_ANSWER_FAIL               2
#define DELETE_ANSWER_NOT_CS3            3
#define DELETE_ANSWER_FAIL_WRONG_VERSION 4

class CANSWER_DELETE : public CGenericMessage
{
	int m_iRequestType;
	int m_iType;
	int m_iVersion;
public:
	void Set( int iType, int iRequestType, int iVersion )
	{
		SetBasicHeader( sizeof( CANSWER_DELETE ), ANSWER_DELETE );
		m_iRequestType = iRequestType;
		m_iType        = iType;
		m_iVersion     = iVersion;
	};
	void Process();
};
