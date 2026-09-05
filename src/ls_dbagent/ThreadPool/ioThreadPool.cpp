#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/SP2Packet.h"
#include "../NodeInfo/User.h"
#include "../Database/cExecuter.h"
#include "../Database/cQueryManager.h"
#include "../ioStatistics.h"
#include "ioThreadPool.h"

extern void Trace(const char* format, ...);

#define QUERY_MAX 4000


ioThreadPool::ioThreadPool()
{
	m_bStopThread = false;
	m_popCount = 0;
	m_pushCount = 0;
	m_maxRemainder = 0;
}

ioThreadPool::~ioThreadPool()
{
}

BOOL ioThreadPool::OnCreate()
{
	cIocpQueue::Startup(1);
	m_queryPool.CreatePool( 100, QUERY_MAX, TRUE );

	for(int i = 0;i < MAX_QUERY_POOL;i++)
	{
		PushQueryPool( new QueryTable );
	}

	GetPushCount(); // kyg 초기화 해줘야함 
	if( Begin() == FALSE )
	{
		LOG.PrintTimeAndLog( 0, "ioThreadPool::OnCreate() m_threadID == 0" );
		return FALSE;
	}	

	DWORD threadId = GetThread();
	if( !g_queryManager.CreateExecuter(GetThread()) )
	{
		LOG.PrintTimeAndLog( 0, " >> [%lu] Executer creation failed", GetThread());
		return FALSE;
	}

	char szEventName[MAX_PATH] = "";
	sprintf_s( szEventName, "TID:%d", GetThread() );
	m_hWaitEvent = ::CreateEvent( NULL, FALSE, FALSE, szEventName );
	if( m_hWaitEvent == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioThreadPool::OnCreate() m_hWaitEvent == 0" );
		return FALSE;
	}

	return TRUE;
}

void ioThreadPool::OnDestroy()
{
	m_bStopThread = true;

	CloseHandle( m_hWaitEvent );
	m_hWaitEvent = NULL;
}

void ioThreadPool::PushQueryPool( QueryTable *queryTable )
{
	if( queryTable ) 
	{
		m_queryPool.Push( queryTable );
		InterlockedIncrement( &m_pushCount );
	}
}

void ioThreadPool::RecordQuery( CQueryData &queryData )
{
	static char buffer[1024 * 16];
	ZeroMemory(buffer, sizeof(buffer));

	if(g_queryManager.Generate( &queryData, buffer, sizeof(buffer) ))
	{
		Debug( "Query pool is empty : %s\r\n", buffer );
		LOG.PrintTimeAndLog( 0, "Unexecuted query(packet) : %s", buffer );
	}
}

BOOL ioThreadPool::PushQueryTable( User *pSender, SP2Packet &rkPacket )
{
	QueryTable *queryTable = m_queryPool.Pop();
	if(queryTable)
	{
		if(!rkPacket.Read(queryTable->m_QueryData)) 
		{	
			PushQueryPool( queryTable );
			LOG.PrintTimeAndLog( 0, "Invalid query(attach) : 0x%x", rkPacket.GetPacketID() );
			return FALSE;
		}
		queryTable->SetUser( pSender );

		InterlockedIncrement( &m_popCount );
		SetMaxRemainder( m_popCount, m_pushCount );

		cIocpQueue::Enqueue(reinterpret_cast<DWORD>(queryTable), sizeof(DWORD));
		return TRUE;
	}
	else
	{
		CQueryData queryData;
		if(!rkPacket.Read(queryData)) 
		{
			LOG.PrintTimeAndLog( 0, "Invalid query(read) : 0x%x", rkPacket.GetPacketID() );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "Unable to query : empty memory(queryId : %d)", queryData.GetQueryID() );
			RecordQuery( queryData );
		}
		return FALSE;
	}
}

void ioThreadPool::RecurrenceQuery()
{
	DWORD bytes = 0;
	while( true )
	{
		QueryTable *queryTable = reinterpret_cast<QueryTable*>(cIocpQueue::Dequeue(bytes));
		if(!queryTable) break;

		PocessQuery( queryTable );
		PushQueryPool( queryTable );
	}
}

void ioThreadPool::PocessQuery( QueryTable *queryTable )
{
	if(queryTable == NULL)
	{
		LOG.PrintTimeAndLog( 0, "[error]Invalid query" );
		return;
	}
	
	User* pUser = queryTable->GetUser();
	if(!pUser) return;

	cExecuter* executer = g_queryManager.GetExecuter(GetThread());	
	if(!executer)
	{
		LOG.PrintTimeAndLog( 0, "[error]Invalid executer" );
		return;
	}

	cSerialize* storage = executer->GetStorage();
	if(!storage)
	{
		LOG.PrintTimeAndLog( 0, "[error]Invalid storage" );
		return;
	}

	CQueryResultData result;
	if( executer->Execute( queryTable->GetUser(), &queryTable->m_QueryData, result ) )
	{
		SP2Packet kPacket( DTPK_QUERY );
		if( kPacket.Write(result) )
		{
			pUser->SendPacket( kPacket );
		}
	}
}

void ioThreadPool::Run()
{
	LOG.PrintTimeAndLog( 0, "[info][threadpool] Run query thread : [threadID:%lu count:%d]", GetThread(), m_queryPool.GetCount() );

	while( TRUE)
	{
		__try
		{
			if( IsThreadStop() )
				break;

			RecurrenceQuery();
		}
		__except( UnHandledExceptionFilter( GetExceptionInformation() ) )
		{
			LOG.PrintTimeAndLog( 0, "[critical][threadpool] Query thread CRASH : [threadID:%lu]", GetThread() );
		}	
	}

	LOG.PrintTimeAndLog( 0, "[%lu]Query thread EXIT", GetThread());
}

long ioThreadPool::GetPopCount()
{
	long value = m_popCount;
	InterlockedExchange(&m_popCount, 0);
	return value;
}

long ioThreadPool::GetPushCount()
{
	long value = m_pushCount;
	InterlockedExchange(&m_pushCount, 0);
	return value;
}

long ioThreadPool::GetMaxRemainder()
{
	long value = m_maxRemainder;
	InterlockedExchange(&m_maxRemainder, 0);
	return value;
}

void ioThreadPool::SetMaxRemainder( long popCount, long pushCount )
{
	if(m_maxRemainder < popCount - pushCount)
		m_maxRemainder = popCount - pushCount;
}

//////////////////////////////////////////////////////////////////////////
ioThreadPoolMgr *ioThreadPoolMgr::sg_Instance = NULL;
ioThreadPoolMgr::ioThreadPoolMgr()
{
	m_dwTestCheckTime = 0;
	m_iTestCompleteCnt= 0;
}

ioThreadPoolMgr::~ioThreadPoolMgr()
{
	ClearThread();
}


ioThreadPoolMgr &ioThreadPoolMgr::GetInstance()
{
	if(sg_Instance == NULL)
		sg_Instance = new ioThreadPoolMgr;
	return *sg_Instance;
}

void ioThreadPoolMgr::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ioThreadPoolMgr::ClearThread()
{
	ioThreadPoolVec::iterator iter = m_ThreadPool.begin();
	for(;iter != m_ThreadPool.end();iter++)
	{
		ioThreadPool *pThreadPool = *iter;
		pThreadPool->OnDestroy();
		SAFEDELETE( pThreadPool );
	}
	m_ThreadPool.clear();
}

BOOL ioThreadPoolMgr::Initialize()
{
	for(int i = 0;i < g_MainProc.GetQueryThreadCnt();i++)
	{
		ioThreadPool *pThread = new ioThreadPool;
		if(!pThread->OnCreate())
		{
			return FALSE;
		}

		m_ThreadPool.push_back( pThread );
	}
	
	return (m_ThreadPool.size() > 0) ? TRUE : FALSE;
}

BOOL ioThreadPoolMgr::AttachQuery( User *pSender, SP2Packet &rkPacket )
{
	QueryHeader header;
	if(!rkPacket.Ref( header ))
	{
		LOG.PrintTimeAndLog( 0, "Invalid query(header)");
		return FALSE;
	}

	DWORD index = g_MainProc.GetThreadIndexSeq( header.nIndex );
	index = (index < m_ThreadPool.size()) ? index : 0;
	if(index < m_ThreadPool.size())
	{
		ioThreadPool *pThread = m_ThreadPool[index];
		return pThread->PushQueryTable( pSender, rkPacket );
	}
	return FALSE;
}

void ioThreadPoolMgr::PrintLog()
{
	long popVal = 0;
	long pushVal =0;
	long remainVal = 0;

	ReportLOG.PrintTimeAndLog(0,"");

	for (int i = 0; i < static_cast<int>(m_ThreadPool.size()); ++i)
	{
		int tempPop = m_ThreadPool[i]->GetPopCount();
		int tempPush = m_ThreadPool[i]->GetPushCount();
		int tempRemain = m_ThreadPool[i]->GetMaxRemainder();

		popVal		+= tempPop;
		pushVal		+= tempPush;
		remainVal	+= tempRemain;

		ReportLOG.PrintNoEnterLog(0, "[%02d]::QPOP:%08d QPUSH:%08d QR:%08d ", i, tempPop, tempPush, tempRemain);
		if((i+1)%2 == 0 && i != 0)
		{
			ReportLOG.PrintNoEnterLog(0, "\r\n");
		}
	}
	ReportLOG.PrintTimeAndLog(0, "ToTal: QPOP:%08d QPUSH:%08d QR:%08d", popVal, pushVal, remainVal);
}
