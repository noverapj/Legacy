#include "../stdafx.h"
#include "ioThreadPool.h"
#include "../Channeling/ioChannelingNodeNaver.h"
#include "../Channeling/ioChannelingNodeManager.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../Channeling/ioChannelingNodeWemadeCashLink.h"
#include "../Local/ioLocalParent.h"

extern CLog LOG;


ioData::ioData()
{
	Clear();
}

ioData::~ioData()
{

}

void ioData::Clear()
{
	m_eProcessType = PT_NONE;
	m_bEmpty = true;
	m_iChannelingType = 0;
	m_szBillingGUID.Clear();
	m_dwUserIndex = 0;
	m_szPrivateID.Clear();
	m_szPublicID.Clear();
	m_bSetUserMouse = false;
	m_szUserNo.Clear();
	m_sServerIP.Clear();
	m_iServerPort = 0;
	m_szUserIP.Clear();
	m_iItemPayAmt = 0;
	m_iItemType = 0;
	m_dwGoodsNo = 0;
	m_szGoodsName.Clear();
	m_iVCode = 0;
	m_iIntID = 0;
	m_iReqNum = 0;

	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		m_iItemValueList[i] = 0;

	m_dwReturnMsgType = 0;
	m_szEncodePW.Clear();
	m_iUserLevel = 0;
	m_szUserMacAddress.Clear();
	m_szUserKey.Clear();

	m_szQuestKey.Clear();
	m_szAuthInfo.Clear();
	m_dwSessionID = 0;

	m_iGameServerPort = 0;
	m_dwRecvUserIndex = 0;
	m_szReceivePrivateID.Clear();
	m_szReceivePublicID.Clear();
	m_dwRecvUserIndex = 0;
	m_szNexonUserNo.Clear();
	m_vBonusCashForConsume.clear();
}

bool ioData::IsEmpty() const
{
	return m_bEmpty;
}

void ioData::SetItemValueList( int iItemValueList[MAX_ITEM_VALUE] )
{
	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		m_iItemValueList[i] = iItemValueList[i];
}

int ioData::GetItemValue( int iArray ) const
{
	if( COMPARE( iArray, 0, MAX_ITEM_VALUE ))
		return m_iItemValueList[iArray];
	else
		return 0;
}

void ioData::AddBonusCashInfoForConsume(const int iIndex, const int iValue)
{
	TwoOfINT stInfo;
	stInfo.value1	= iIndex;
	stInfo.value2	= iValue;

	m_vBonusCashForConsume.push_back(stInfo);
}

ioData& ioData::operator=( const ioData &rhs )
{
	// 아래 처럼 명시적으로 복사하지 않으면 narrow copy가 발생하여 크래쉬가 발생할 수 있다.
	m_eProcessType = rhs.m_eProcessType;
	m_bEmpty = rhs.m_bEmpty;
	m_iChannelingType = rhs.m_iChannelingType;
	m_szBillingGUID = rhs.m_szBillingGUID;
	m_dwUserIndex = rhs.m_dwUserIndex;
	m_szPrivateID = rhs.m_szPrivateID;
	m_szPublicID = rhs.m_szPublicID;
	m_bSetUserMouse = rhs.m_bSetUserMouse;
	m_szUserNo = rhs.m_szUserNo;
	m_sServerIP = rhs.m_sServerIP;
	m_iServerPort = rhs.m_iServerPort;
	m_szUserIP = rhs.m_szUserIP;
	m_Country  = rhs.m_Country;
	m_iItemPayAmt = rhs.m_iItemPayAmt;
	m_iItemType = rhs.m_iItemType;
	m_dwGoodsNo = rhs.m_dwGoodsNo;
	m_szGoodsName = rhs.m_szGoodsName;
	m_iVCode = rhs.m_iVCode;
	m_iIntID = rhs.m_iIntID;
	m_iReqNum = rhs.m_iReqNum;
	m_szNexonUserNo = rhs.m_szNexonUserNo;

	m_dwExp = rhs.m_dwExp;
	m_iGameServerPort = rhs.m_iGameServerPort;
	m_szReceivePrivateID = rhs.m_szReceivePrivateID;
	m_szReceivePublicID = rhs.m_szReceivePublicID;
	m_dwRecvUserIndex = rhs.m_dwRecvUserIndex;

	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		m_iItemValueList[i] = rhs.m_iItemValueList[i];

	m_dwReturnMsgType  = rhs.m_dwReturnMsgType;
	m_szEncodePW       = rhs.m_szEncodePW;
	m_iUserLevel       = rhs.m_iUserLevel;
	m_szUserMacAddress = rhs.m_szUserMacAddress;
	m_szUserKey        = rhs.m_szUserKey;
	m_szChargeNo	   = rhs.m_szChargeNo;
	m_wCancelFlag	   = rhs.m_wCancelFlag;
	m_dwIndex		   = rhs.m_dwIndex;
	m_szQuestKey	   = rhs.m_szQuestKey;
	m_szAuthInfo       = rhs.m_szAuthInfo;
	m_dwSessionID      = rhs.m_dwSessionID;
	m_dwGold		   = rhs.m_dwGold;
	
	TwoOfINTVec vInfo;
	rhs.GetBonusCashInfo(vInfo);

	m_vBonusCashForConsume	= vInfo;

	return *this;
}
///////////////////////////////////////////////////////////////////////////

ioBillThread::ioBillThread()
{
	m_bStopThread = false;
	m_bActiveThread = false;
	m_iQueryProcessCount = 0;
	m_eThreadPoolType = TPT_NONE;
}

ioBillThread::~ioBillThread()
{

}

void ioBillThread::OnCreate()
{
	m_hKillEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	if( m_hKillEvent == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioBillThread::OnCreate() m_hKillEvent == NULL" );
		return;
	}

	InitMemoryPool();

	Begin();	
	if( GetThread() == 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioBillThread::OnCreate() m_threadID == 0" );
		return;
	}	

	char szEventName[MAX_PATH] = "";
	sprintf_s( szEventName, "TID:%d", GetThread() );
	m_hWaitEvent = ::CreateEvent( NULL, FALSE, FALSE, szEventName );
	if( m_hWaitEvent == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioBillThread::OnCreate() m_hWaitEvent == 0" );
		return;
	}
}

void ioBillThread::OnDestroy()
{
	m_bStopThread = true;

	SetEvent( m_hKillEvent );
	Sleep( 20 );

	CloseHandle( m_hWaitEvent );
	m_hWaitEvent = NULL;

	CloseHandle( m_hKillEvent );
	m_hKillEvent = NULL;

	ReleaseMemoryPool();
}

void ioBillThread::Run()
{
	__try
	{
		LOG.PrintTimeAndLog(0,"ioBillThread  TID : %d\n",GetCurrentThreadId());
		enum { MAX_EVENT = 2, };
		while(true)
		{
			HANDLE hWaits[MAX_EVENT];
			hWaits[0] = m_hWaitEvent;
			hWaits[1] = m_hKillEvent;			

			m_bActiveThread = false;
			DWORD dwWait = ::WaitForMultipleObjects( MAX_EVENT, hWaits, FALSE, INFINITE );				
			if( dwWait - WAIT_OBJECT_0 == 0 )
			{
				m_bActiveThread = true;
				if( IsThreadStop() )
					break;
				else
				{
					ProcessLoop();
				}
			}			
			else if( dwWait - WAIT_OBJECT_0 == 1 )
				break;        // 종료 발생
		}
	}
	__except( ExceptCallBack( GetExceptionInformation() ) )
	{
		//                                                 
		LOG.PrintTimeAndLog( 0, "ioBillThread Thread Crash : %d", GetThread() );
	}	
	LOG.PrintTimeAndLog( 0, "ioBillThread Thread EXIT : %d - %d", GetThread(), m_iQueryProcessCount );
}

bool ioBillThread::SetData( const ioData &rData )
{
	ThreadSync ts( this );

	ioData *pData = (ioData*) m_MemNode.Pop();
	if( !pData )
		return false;

	*pData = rData;
	m_vData.push_back( pData );
	SetEvent( m_hWaitEvent );
	return true;
}

void ioBillThread::ProcessLoop()
{
	while( true )
	{
		ioData *pData = NULL;	
		ioData  kData;
		{
			ThreadSync ts( this );
			if( m_vData.empty() )
				break;

			pData = *m_vData.begin();
			if( pData )
			{
				kData = *pData;
				m_vData.erase( m_vData.begin() );
				pData->Clear();
				m_MemNode.Push( pData );		
			}
		}

		ProcessWeb( kData );
	}
}

void ioBillThread::ProcessWeb( const ioData &kData )
{

	if( m_eThreadPoolType == TPT_CHANNELING )
	{
		ChannelingType eChannelingType = (ChannelingType) kData.GetChannelingType();
		ioChannelingNodeParent *pNode =  g_ChannelingMgr.GetNode( eChannelingType );
		if( !pNode )
			return;

		if( kData.GetProcessType() == ioData::PT_GET_CASH )
		{
 			__try
			{
				pNode->ThreadGetCash( kData );
			}
			__except(1)
			{
				LOG.PrintTimeAndLog(0,"Error Process ThreadGetCash ChannelingType :%d",kData.GetChannelingType());
			}
		}
		else if( kData.GetProcessType() == ioData::PT_OUTPUT_CASH )
		{
			__try
			{
				pNode->ThreadOutputCash( kData );
			}
			__except(1)
			{
				LOG.PrintTimeAndLog(0,"Error Process ThreadOutputCash ChannelingType :%d",kData.GetChannelingType());
			}
		}
		else if( kData.GetProcessType() == ioData::PT_FILL_CASH_URL )
		{
			__try
			{
				pNode->ThreadFillCashUrl( kData );
			}
			__except(1)
			{
				LOG.PrintTimeAndLog(0,"Error ProcessThreadFillCashUrl ChannelingType :%d",kData.GetChannelingType());
			}
		}
		else if( kData.GetProcessType() == ioData::PT_SUBSCRIPTION_RETRACT )
		{
			__try
			{
				pNode->ThreadSubscriptionRetract( kData );
			}
			__except(1)
			{
				LOG.PrintTimeAndLog(0,"Error Process ThreadSubscriptionRetract ChannelingType :%d",kData.GetChannelingType());
			}
		}
		else if( kData.GetProcessType() == ioData::PT_SUBSCRIPTION_RETRACT_CHECK )
		{
			__try
			{
				pNode->ThreadSubscriptionRetractCheck( kData );
			}
			__except(1)
			{
				LOG.PrintTimeAndLog(0,"Error Process ThreadSubscriptionRetract ChannelingType :%d",kData.GetChannelingType());
			}
		}
		else if( kData.GetProcessType() == ioData::PT_PRESENT_CASH )
		{
			__try
			{
				if( eChannelingType == CNT_WEMADEBUY )
				{
					ioChannelingNodeWemadeCashLink* pWemade	= static_cast<ioChannelingNodeWemadeCashLink*>(pNode);
					pWemade->ThreadPresentCash( kData );
				}
			}
			__except(1)
			{
				LOG.PrintTimeAndLog(0,"Error Process Present cash ChannelingType :%d",kData.GetChannelingType());
			}
		}
	}
	else if( m_eThreadPoolType == TPT_LOCAL )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( !pLocal )
			return;

		if( kData.GetProcessType() == ioData::PT_LOGIN )
		{
			pLocal->ThreadLogin( kData, m_LoginMgr );
		}
		else if( kData.GetProcessType() == ioData::PT_GET_CASH )
		{
			pLocal->ThreadGetCash( kData );
		}
		else if( kData.GetProcessType() == ioData::PT_OUTPUT_CASH )
		{
			pLocal->ThreadOutputCash( kData );
		}
		else if( kData.GetProcessType() == ioData::PT_CANCEL_CASH )
		{
			pLocal->ThreadCancelCash( kData );
		}
// 		else if( kData.GetProcessType() == ioData::PT_SUBSCRIPTION_RETRACT ) //kyg 해외에는 아직 지원 안하는듯 
// 		{
// 			pLocal->ThreadSubscriptionRetract( kData );
// 		}
	}
}

void ioBillThread::InitMemoryPool()
{
	// Login
	m_LoginMgr.InitMemoryPool();

	// data pool
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "MemoryPool" );
	int iMax = kLoader.LoadInt( "DataPool", 20000 );
	
	for(int i = 0;i < iMax * 3; i++) //kyg 맥스값 조정 
	{
		ioData *pData = new ioData;
		if( !pData )
			continue;
		m_MemNode.Push( pData );
	}
}

void ioBillThread::ReleaseMemoryPool()
{
	// data
	ioDataVec::iterator iter, iEnd;	
	iEnd = m_vData.end();
	for(iter = m_vData.begin();iter != iEnd;++iter)
	{
		ioData *pData = *iter;
		if( !pData )
			continue;
		pData->Clear();
		m_MemNode.Push( pData );

	}	
	m_vData.clear();
	m_MemNode.DestroyPool();
}

//////////////////////////////////////////////////////////////////////////
ioThreadPool *ioThreadPool::sg_Instance = NULL;
ioThreadPool::ioThreadPool()
{
	m_eThreadPoolType = TPT_NONE;
}

ioThreadPool::~ioThreadPool()
{
	Clear();
}

ioThreadPool &ioThreadPool::GetInstance()
{
	if(sg_Instance == NULL)
		sg_Instance = new ioThreadPool;
	return *sg_Instance;
}

void ioThreadPool::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ioThreadPool::Clear()
{
	// thread
	ioThreadVec::iterator iter = m_vThread.begin();
	for(;iter != m_vThread.end();iter++)
	{
		ioBillThread *pThread = *iter;
		if( !pThread )
			continue;
		pThread->OnDestroy();
		SAFEDELETE( pThread );
	}
	m_vThread.clear();
}

void ioThreadPool::Initialize()
{
	// thread pool
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "MemoryPool" );
	int iMax = kLoader.LoadInt( "ThreadPool", 16 );
	for(int i = 0;i < iMax;i++)
	{
		ioBillThread *pThread = new ioBillThread;
		if( !pThread )
			continue;
		pThread->OnCreate();
		pThread->SetThreadPoolType( m_eThreadPoolType );
		m_vThread.push_back( pThread );
	}
}

void ioThreadPool::SetData( const ioData &rData )
{
	if( m_vThread.empty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][thread]%s Thread Empty.", __FUNCTION__ );
		return;
	}

	DWORD dwThread = 0;
	bool bSeccess = false;

	int iThreadSize = m_vThread.size();
	int iArray = rData.GetPrivateID().GetHashCode()%iThreadSize;

	if( COMPARE( iArray, 0, iThreadSize ) )
	{
		ioBillThread *pThread = m_vThread[iArray];
		if( pThread )
		{
			bSeccess = pThread->SetData( rData );

			dwThread = pThread->GetThread();
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error", __FUNCTION__ );
	}

	if( !bSeccess )
	{
		// error
		if( rData.GetProcessType() == ioData::PT_LOGIN )
		{
			SP2Packet kPacket( rData.GetReturnMsgType() );
			kPacket << rData.GetPrivateID();
			kPacket <<  rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			kPacket << false;
			      
			if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %s:%s", "ioThreadPool::SetData", rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
				return;
			}

		}
		else if( rData.GetProcessType() == ioData::PT_GET_CASH )
		{
			SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << rData.GetSetUserMouse();
			kPacket << CASH_RESULT_EXCEPT;
			      
			if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %s:%s", "ioThreadPool::SetData", rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
				return;
			}
		}
		else if( rData.GetProcessType() == ioData::PT_OUTPUT_CASH )
		{
			SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << CASH_RESULT_EXCEPT;
			kPacket << rData.GetExp();
			kPacket << rData.GetItemType();
			      
			if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %s:%s", "ioThreadPool::SetData", rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
				return;
			}
		}
		else if( rData.GetProcessType() == ioData::PT_FILL_CASH_URL )
		{
			SP2Packet kPacket( BSTPK_FILL_CASH_URL_RESULT );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << CASH_RESULT_EXCEPT;
			      
			if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %s:%s", "ioThreadPool::SetData", rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
				return;
			}
		}

		LOG.PrintTimeAndLog( 0, "[error][thread]%s MemPool Zero! %s:%d:%s:%s:%d:%d[%d]", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetUserIndex(), rData.GetPublicID().c_str(), rData.GetPrivateID().c_str(), (int) rData.GetProcessType(), rData.GetReturnMsgType(), dwThread );
	}
}


int ioThreadPool::GetActiveThreadCount()
{
	int iActiveCount = 0;
	ioThreadVec::iterator iter = m_vThread.begin();
	for(;iter != m_vThread.end();iter++)
	{
		ioBillThread *pThread = *iter;
		if( pThread == NULL ) continue;
		if( pThread->IsActiveThread() )
			iActiveCount++;
	}
	return iActiveCount;
}

int ioThreadPool::GetNodeSize()
{
	int iSize = 0;
	ioThreadVec::iterator iter = m_vThread.begin();
	for(;iter != m_vThread.end();iter++)
	{
		ioBillThread *pThread = *iter;
		if( pThread == NULL ) continue;
		iSize += pThread->GetNodeSize();
	}
	return iSize;
}

int ioThreadPool::RemainderNode()
{
	int iRemainderNode = 0;
	ioThreadVec::iterator iter = m_vThread.begin();
	for(;iter != m_vThread.end();iter++)
	{
		ioBillThread *pThread = *iter;
		if( pThread == NULL ) continue;
		iRemainderNode += pThread->RemainderNode();
	}
	return iRemainderNode;
}

int ioThreadPool::GetLoginNodeSize()
{
	int iSize = 0;
	ioThreadVec::iterator iter = m_vThread.begin();
	for(;iter != m_vThread.end();iter++)
	{
		ioBillThread *pThread = *iter;
		if( pThread == NULL ) continue;
		iSize += pThread->GetLoginNodeSize();
	}
	return iSize;
}

int ioThreadPool::RemainderLoginNode()
{
	int iRemainderNode = 0;
	ioThreadVec::iterator iter = m_vThread.begin();
	for(;iter != m_vThread.end();iter++)
	{
		ioBillThread *pThread = *iter;
		if( pThread == NULL ) continue;
		iRemainderNode += pThread->RemainderLoginNode();
	}
	return iRemainderNode;
}