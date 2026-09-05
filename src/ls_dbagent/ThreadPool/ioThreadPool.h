#pragma once

class User;
class SP2Packet;

const int MAX_QUERY_POOL = 5;

struct QueryTable
{
	User      *m_pSender;
	CQueryData m_QueryData;

	QueryTable()
	{
		m_pSender = NULL;
	}

	void SetUser(User* pUser)	{ m_pSender = pUser; }
	User* GetUser()				{ return m_pSender; }
};

class ioThreadPool : public Thread, public cIocpQueue
{
protected:
	HANDLE m_hWaitEvent;

protected:
	bool   m_bStopThread;
	long   m_popCount;
	long   m_pushCount;
	long   m_maxRemainder;
	MemPooler<QueryTable> m_queryPool;

public:
	BOOL OnCreate();
	void OnDestroy();

public:
	virtual void Run();

public:
	inline bool IsThreadStop(){ return m_bStopThread; }

protected:
	void PushQueryPool( QueryTable *queryTable );
	void RecordQuery( CQueryData &queryData );

public:
	BOOL PushQueryTable( User* pSender, SP2Packet &rkPacket );
	void PocessQuery( QueryTable *queryTable );

public:
	void RecurrenceQuery();

public:
	long GetPopCount();
	long GetPushCount();
	long GetMaxRemainder();
	void SetMaxRemainder( long popCount, long pushCount );

public:
	ioThreadPool();
	virtual ~ioThreadPool();
};
//////////////////////////////////////////////////////////////////////////
class ioThreadPoolMgr : public SuperParent
{
public:
	enum
	{
		MAX_TOTAL_POOL = 32000,
	};

private:
	static ioThreadPoolMgr *sg_Instance;

public:
	static ioThreadPoolMgr &GetInstance();
	static void ReleaseInstance();

protected:
	typedef std::vector< ioThreadPool * > ioThreadPoolVec;
	ioThreadPoolVec m_ThreadPool;

protected:
	DWORD m_dwTestCheckTime;
	int   m_iTestCompleteCnt;

protected:
	void ClearThread();

public:
	BOOL Initialize();
	BOOL AttachQuery( User *pSender, SP2Packet &rkPacket );
	void PrintLog();

private:
	ioThreadPoolMgr();
	virtual ~ioThreadPoolMgr();
};

#define g_ThreadPool ioThreadPoolMgr::GetInstance()
