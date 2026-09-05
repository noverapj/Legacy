#include "../stdafx.h"
#include "MemInfoManager.h"

extern CLog LOG;

#define MAX_POOL 100

BasicInfo::BasicInfo()
{
	Clear();
}

BasicInfo::~BasicInfo()
{

}

void BasicInfo::Clear()
{
	m_szKey.Clear();
	m_dwCreateTime = 0;
	m_dwUserIndex  = 0;
	m_szBillingGUID.Clear();
	m_szServrIP.Clear();
	m_iServerClientPort = 0;
}
//-------------------------------------------------------------------------

GetCashInfo::GetCashInfo()
{
	Clear();
}

GetCashInfo::~GetCashInfo()
{

}

void GetCashInfo::Clear()
{
	BasicInfo::Clear();
	m_bSetUserMouse = false;
}

GetCashInfo& GetCashInfo::operator=( const GetCashInfo &rhs )
{
	m_szKey         = rhs.m_szKey;
	m_dwCreateTime  = rhs.m_dwCreateTime;
	m_dwUserIndex   = rhs.m_dwUserIndex;
	m_szBillingGUID = rhs.m_szBillingGUID;
	m_szServrIP     = rhs.m_szServrIP;
	m_iServerClientPort = rhs.m_iServerClientPort;
	
	m_bSetUserMouse = rhs.m_bSetUserMouse;

	return *this;
}

//-------------------------------------------------------------------------
OutPutCashInfo::OutPutCashInfo()
{
	Clear();
}

OutPutCashInfo::~OutPutCashInfo()
{

}

void OutPutCashInfo::Clear()
{
	BasicInfo::Clear();
	m_iItemType       = 0;
	m_iPayAmt         = 0;
	m_iChannelingType = 0;
	m_szValue.Clear();
	for (int i = 0; i < MAX_ITEM_VALUE; i++)
	{
		m_iItemValueList[i] = 0;
	}
}

OutPutCashInfo& OutPutCashInfo::operator=( const OutPutCashInfo &rhs )
{
	m_szKey         = rhs.m_szKey;
	m_dwCreateTime  = rhs.m_dwCreateTime;
	m_dwUserIndex   = rhs.m_dwUserIndex;
	m_szBillingGUID = rhs.m_szBillingGUID;
	m_szServrIP     = rhs.m_szServrIP;
	m_iServerClientPort = rhs.m_iServerClientPort;

	m_iItemType       = rhs.m_iItemType;
	m_iPayAmt         = rhs.m_iPayAmt;
	m_iChannelingType = rhs.m_iChannelingType;
	m_szValue         = rhs.m_szValue;
	for (int i = 0; i < MAX_ITEM_VALUE; i++)
	{
		m_iItemValueList[i] = rhs.m_iItemValueList[i];
	}

	return *this;
}
//--------------------------------------------------------------------------
BillInfoMgr::BillInfoMgr(void)
{
	InitMemoryPool();
}

BillInfoMgr::~BillInfoMgr(void)
{
	ReleaseMemoryPool();
}

void BillInfoMgr::InitMemoryPool()
{	m_vBillingInfo.reserve( 10 );

	m_billInfoPool.CreatePool(0, MAX_POOL, true);
	
	for(int i = 0; i< MAX_POOL; i++)
	{
		BillInfoMgr::BillingInfo* pInfo = new BillInfoMgr::BillingInfo ;
		if(pInfo)
		{
			m_billInfoPool.Push(pInfo);
		}
	}
}

void BillInfoMgr::ReleaseMemoryPool()
{
	for(vBillingInfo::iterator iter = m_vBillingInfo.begin(); iter != m_vBillingInfo.end(); ++iter)
	{
		BillingInfo *pInfo = *iter;
		if( !pInfo )
			 continue;
		m_billInfoPool.Push( pInfo );	
	}
	m_vBillingInfo.clear();
	m_billInfoPool.DestroyPool();	

}

BillInfoMgr::BillingInfo * BillInfoMgr::Get( const ioHashString &rszKey )
{
	vBillingInfo::iterator iter = m_vBillingInfo.begin();
	while ( iter != m_vBillingInfo.end() )
	{
		BillingInfo *pInfo = *iter;

		if( !pInfo )
		{
			++iter;
			continue;
		}
		else if( pInfo->m_szKey != rszKey )
		{
			++iter;
			continue;
		}
		else if( TIMEGETTIME() - pInfo->m_dwCreateTime >= MAX_ALIVE_TIME)
		{
			DWORD userIndex = 0;
			ioHashString szBillingGUID;
			pInfo->m_kPacket >> userIndex >> szBillingGUID;
			LOG.PrintTimeAndLog( 0, "%s Delete past alive time : %d:%s:%s","BillInfoMgr::Get", userIndex, szBillingGUID.c_str(), pInfo->m_szKey.c_str() );

			iter = m_vBillingInfo.erase( iter );	

			pInfo->InitData();
			m_billInfoPool.Push( pInfo );	

			continue;
		}

		return pInfo;
	}

	return NULL;
}

bool BillInfoMgr::Add( BillingInfo *pBillingInfo )
{
	// over
	int iSize = m_vBillingInfo.size();
	if( iSize >= MAX_INFO )
	{
		BillingInfo *pBeginInfo = m_vBillingInfo[0];
		if( !pBeginInfo )
		{
			LOG.PrintTimeAndLog( 0, "%s vector is over. | pBegin == NULL.", __FUNCTION__ );
			return false;
		}
	}
	m_vBillingInfo.push_back( pBillingInfo );
	
	if( ( iSize % 1000 ) == 0 && iSize != 0 )
		LOG.PrintTimeAndLog( 0, "%s Billing Info is %d.", __FUNCTION__, iSize );
	
	return true;
}

void BillInfoMgr::Delete( const ioHashString &rszKey )
{
	for(vBillingInfo::iterator iter = m_vBillingInfo.begin(); iter != m_vBillingInfo.end(); ++iter)
	{
		BillingInfo *pInfo = *iter;
		if( !pInfo )
			continue;

		if( pInfo->m_szKey != rszKey )
			continue;

		m_vBillingInfo.erase( iter );		
		pInfo->InitData();
		m_billInfoPool.Push( pInfo );	//©öYE?
		
		return;
	}
}

void BillInfoMgr::DeleteMaxAliveTimeoutInfo( int maxAliveTime )
{
	vBillingInfo::iterator iter = m_vBillingInfo.begin();
	while ( iter != m_vBillingInfo.end() )
	{
		BillingInfo *pInfo = *iter;

		if( !pInfo )
		{
			iter++;
			continue;
		}
	    if( TIMEGETTIME() - pInfo->m_dwCreateTime >= (DWORD)maxAliveTime )		
		{
			if( pInfo->m_eType == BillInfoMgr::AT_GET )
			{
				DWORD        dwUserIndex   = 0;
				ioHashString szBillingGUID;
				bool         bSetUserMouse = false;
				ioHashString szServerIP;
				int          iClientPort   = 0;
				pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> bSetUserMouse >> szServerIP >> iClientPort;	
				
				LOG.PrintTimeAndLog( 0, "BillInfoMgr::DeleteMaxAliveTimeoutInfo() Timeout Billing Info:AT_GET:%d:%s:%d:%s", 
										pInfo->m_iChannelingType, 
										pInfo->m_szKey.c_str(), 
										dwUserIndex, 										
										szBillingGUID.c_str());
			}

			else if( pInfo->m_eType == BillInfoMgr::AT_OUTPUT )
			{
				DWORD        dwUserIndex   = 0;
				ioHashString szBillingGUID;
				int          iType         = 0;
				int          iPayAmt       = 0;
				int          iChannelingType = 0;
				ioHashString szServerIP;
				int          iClientPort   = 0;
				ioHashString szChargeNo = "";
				pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szServerIP >> iClientPort;

				LOG.PrintTimeAndLog( 0, "BillInfoMgr::DeleteMaxAliveTimeoutInfo() Timeout Billing Info:AT_OUTPUT:%d:%s:%d:%d:%d:%s:%d", 
										pInfo->m_iChannelingType, 
										pInfo->m_szKey.c_str(),
										dwUserIndex, 
										iType, 
										iPayAmt, 
										szServerIP.c_str(),
										iClientPort
										);
			}
			else if( pInfo->m_eType == BillInfoMgr::AT_SUBSCRIPTIONRETRACT )
			{
				DWORD		 dwUserIndex = 0;
				ioHashString szBillingGUID;
				int          iChannelingType = 0;
				ioHashString szServerIP;
				int          iClientPort   = 0;
				DWORD		 dwIndex = 0;
				ioHashString szChargeNo;

				pInfo->m_kPacket >> dwUserIndex >>  szBillingGUID >> szServerIP >> iClientPort >> dwIndex >> szChargeNo;

				LOG.PrintTimeAndLog( 0, "BillInfoMgr::DeleteMaxAliveTimeoutInfo() Timeout Billing Info::AT_SUBSCRIPTIONRETRACT:%d:%s:%d:%s:%s:%d:%d:%s",
										pInfo->m_iChannelingType,
										pInfo->m_szKey.c_str(),
										dwUserIndex,
										szBillingGUID.c_str(),
										szServerIP.c_str(),
										iClientPort,
										dwIndex,
										szChargeNo.c_str());

									
			}

			iter = m_vBillingInfo.erase( iter );
			pInfo->InitData();
			m_billInfoPool.Push( pInfo );	
			
		}
		else
		{
			iter++;
		}
	}
}


BillInfoMgr::BillingInfo * BillInfoMgr::PopBillInfo()
{
	BillingInfo * pInfo = (BillingInfo*)m_billInfoPool.Pop();
	
	if( pInfo )
	{
		pInfo->InitData();
	}
	else
	{
		LOG.PrintTimeAndLog(0, "[error][billinfo]BillInfoMgr::PopBillInfo() memory empty" );
	}
	return pInfo;
}

