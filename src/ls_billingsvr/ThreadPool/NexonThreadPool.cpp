#include "../stdafx.h"
#include "NexonThreadPool.h"
#include "../Channeling/ioChannelingNodeNaver.h"
#include "../Channeling/ioChannelingNodeManager.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../Local/ioLocalParent.h"
#include "ioThreadPool.h"

extern CLog LOG;

 
//////////////////////////////////////////////////////////////////////////
NexonThreadPool *NexonThreadPool::sg_Instance = NULL;
NexonThreadPool::NexonThreadPool()
{
	m_eThreadPoolType = TPT_NONE;
}

NexonThreadPool::~NexonThreadPool()
{
	Clear();
}

NexonThreadPool &NexonThreadPool::GetInstance()
{
	if(sg_Instance == NULL)
		sg_Instance = new NexonThreadPool;
	return *sg_Instance;
}

void NexonThreadPool::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void NexonThreadPool::Clear()
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

void NexonThreadPool::Initialize()
{
	// thread pool
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "MemoryPool" );
	int iMax = 1; //kyg Nexon 고정 130726 soap 에러로 하나만 써야됨
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


bool NexonThreadPool::SetThreadData( ioData &rData )
{
	ioThreadVec::iterator iter = m_vThread.begin();
	for(;iter != m_vThread.end();iter++)
	{
		ioBillThread *pThread = *iter;
		if( !pThread )
			continue;
		if( pThread->SetData( rData ) )
			return true;
	}

	return false;
}

void NexonThreadPool::SetData( const ioData &rData )
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
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %s:%s", "NexonThreadPool::SetData", rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
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
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %s:%s", "NexonThreadPool::SetData", rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
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
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %s:%s", "NexonThreadPool::SetData", rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
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
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %s:%s", "NexonThreadPool::SetData", rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
				return;
			}
		}

		LOG.PrintTimeAndLog( 0, "[error][thread]%s MemPool Zero! %s:%d:%s:%s:%d:%d[%d]", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetUserIndex(), rData.GetPublicID().c_str(), rData.GetPrivateID().c_str(), (int) rData.GetProcessType(), rData.GetReturnMsgType(), dwThread );
	}
}


int NexonThreadPool::GetActiveThreadCount()
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

int NexonThreadPool::GetNodeSize()
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

int NexonThreadPool::RemainderNode()
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

int NexonThreadPool::GetLoginNodeSize()
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

int NexonThreadPool::RemainderLoginNode()
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