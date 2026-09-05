#include "../stdafx.h"
#include "../NodeInfo/ServerNode.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "BillInfoManager.h"
#include "../database/logdbclient.h"

extern CLog LOG;

BillInfoManager::BillInfoManager()
{
	InitMemoryPool();
}

BillInfoManager::~BillInfoManager(void)
{
	ReleaseMemoryPool();
}

void BillInfoManager::InitMemoryPool()
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "MemoryPool" );
	int maxPool = kLoader.LoadInt( "BillingInfoPool", 5000 );

	m_billInfoPool.CreatePool(200, maxPool, true);
}

void BillInfoManager::ReleaseMemoryPool()
{
	for(BillInfoList::iterator iter = m_billInfos.begin(); iter != m_billInfos.end(); ++iter)
	{
		BillingInfo *pInfo = *iter;
		if( !pInfo )
			 continue;
		m_billInfoPool.Push( pInfo );	
	}
	m_billInfos.clear();
	m_billInfoPool.DestroyPool();	
}

BillInfoManager::BillingInfo * BillInfoManager::Get( const ioHashString &rszKey )
{
	BillInfoList::iterator iter = m_billInfos.begin();
	while ( iter != m_billInfos.end() )
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
			LOG.PrintTimeAndLog( 0, "%s Delete past alive time : %d:%d:%s:%s","BillInfoManager::Get", pInfo->m_iChannelingType, userIndex, szBillingGUID.c_str(), pInfo->m_szKey.c_str() );

			iter = m_billInfos.erase( iter );	

			pInfo->InitData();
			m_billInfoPool.Push( pInfo );	

			continue;
		}

		return pInfo;
	}

	return NULL;
}

bool BillInfoManager::Add( BillingInfo *pBillingInfo )
{
	//// over
	int iSize = m_billInfos.size();
	
	
	
	m_billInfos.push_back( pBillingInfo );
	

	if( ( iSize % 1000 ) == 0 && iSize != 0 )
		LOG.PrintTimeAndLog( 0, "%s Billing Info is %d.", __FUNCTION__, iSize );
	
	return true;
}




void BillInfoManager::Delete( const ioHashString &rszKey )
{
	for(BillInfoList::iterator iter = m_billInfos.begin(); iter != m_billInfos.end(); ++iter)
	{
		BillingInfo *pInfo = *iter;
		if( !pInfo )
			continue;

		if( pInfo->m_szKey != rszKey )
			continue;

		
		m_billInfos.erase( iter );		
		pInfo->InitData();
		m_billInfoPool.Push( pInfo );	
		
		return;
	}
}

///////////////////////////////////////////////////////
//EU용 별도
BillInfoManager::BillingInfo * BillInfoManager::GetEUInfo( const DWORD dwKey )
{
	BillInfoList::iterator iter = m_billInfos.begin();
	while ( iter != m_billInfos.end() )
	{
		BillingInfo *pInfo = *iter;

		if( !pInfo )
		{
			++iter;
			continue;
		}
		else if( pInfo->m_dwKey != dwKey )
		{
			++iter;
			continue;
		}
		else if( TIMEGETTIME() - pInfo->m_dwCreateTime >= MAX_ALIVE_TIME)
		{
			DWORD userIndex = 0;
			ioHashString szBillingGUID;
			pInfo->m_kPacket >> userIndex >> szBillingGUID;
			LOG.PrintTimeAndLog( 0, "%s Delete past alive time : %d:%d:%s","BillInfoManager::Get", pInfo->m_iChannelingType, userIndex, szBillingGUID.c_str() );

			iter = m_billInfos.erase( iter );	

			pInfo->InitData();
			m_billInfoPool.Push( pInfo );	

			continue;
		}

		return pInfo;
	}

	return NULL;
}

	
void BillInfoManager::DeleteEUInfo( const DWORD dwKey )
{
	for(BillInfoList::iterator iter = m_billInfos.begin(); iter != m_billInfos.end(); ++iter)
	{
		BillingInfo *pInfo = *iter;
		if( !pInfo )
			continue;

		if( pInfo->m_dwKey != dwKey )
			continue;
		
		m_billInfos.erase( iter );		
		pInfo->InitData();
		m_billInfoPool.Push( pInfo );	
		
		return;
	}
}


void BillInfoManager::DeleteMaxAliveTimeoutInfo( int maxAliveTime )
{
	BillInfoList::iterator iter = m_billInfos.begin();
	while ( iter != m_billInfos.end() )
	{
		BillingInfo *pInfo = *iter;

		if( !pInfo )
		{
			++iter;
			continue;
		}
	    if( TIMEGETTIME() - pInfo->m_dwCreateTime >= (DWORD)maxAliveTime )		
		{
			//로그
			if( pInfo->m_eType == BillInfoManager::AT_GET )
			{
				DWORD        dwUserIndex   = 0;
				ioHashString szBillingGUID;
				bool         bSetUserMouse = false;
				ioHashString szServerIP;
				int          iClientPort   = 0;
				pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> bSetUserMouse >> szServerIP >> iClientPort;	
				
				LOG.PrintTimeAndLog( 0, "[warning][timeout]BillInfoManager::DeleteMaxAliveTimeoutInfo Billing Info:AT_GET:%d:%s:%d:%s", 
										pInfo->m_iChannelingType, 
										pInfo->m_szKey.c_str(), 
										dwUserIndex, 										
										szBillingGUID.c_str());

				SP2Packet kPacket( BSTPK_TIMEOUT_BILLINGGUID );
				kPacket << dwUserIndex;
				kPacket << szBillingGUID;
				kPacket << pInfo->m_iChannelingType;
				kPacket << pInfo->m_eType;

				if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
				{

					LOG.PrintTimeAndLog( 0, "[warning][timeout]BillInfoManager::DeleteMaxAliveTimeoutInfo Billing Info:AT_GET GameServer Send Fail: %d:%d:%s:%s", 
											pInfo->m_iChannelingType, 
											dwUserIndex, 
											szBillingGUID.c_str(), 
											pInfo->m_szKey.c_str() );
				}
			}

			else if( pInfo->m_eType == BillInfoManager::AT_OUTPUT )
			{
				DWORD        dwUserIndex   = 0;
				ioHashString szBillingGUID;
				int          iType         = 0;
				int          iPayAmt       = 0;
				int          iChannelingType = 0;
				ioHashString szServerIP;
				ioHashString szUserIP = "";
				int          iClientPort   = 0;
				ioHashString szChargeNo = "";
				ioHashString szPrivateID = "";
				//위메이드
				//pInfo->m_kPacket >> dwUserIndex>> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szUserIP >> szServerIP >> iClientPort;

				//투니랜드
				//pInfo->m_kPacket << dwUserIndex << szBillingGUID << iType << iPayAmt << iChannelingType << szPrivateID;
				//pInfo->m_kPacket << pServerNode->GetIP() << pServerNode->GetClientPort();		

				//엠게임
				//pInfo->m_kPacket << dwUserIndex << szBillingGUID << iType << iPayAmt << iChannelingType;
				//pInfo->m_kPacket << pServerNode->GetIP() << pServerNode->GetClientPort();
	
				if( pInfo->m_iChannelingType == CNT_MGAME )
				{
					pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szServerIP >> iClientPort;
				}
				else if ( pInfo->m_iChannelingType == CNT_WEMADEBUY )
				{
					pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szUserIP >> szServerIP >> iClientPort;
				}
				else if ( pInfo->m_iChannelingType == CNT_TOONILAND )
				{
					pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szPrivateID >> szServerIP >> iClientPort;
				}
				

				LOG.PrintTimeAndLog( 0, "[warning][timeout]BillInfoMgr::DeleteMaxAliveTimeoutInfo Billing Info:AT_OUTPUT:%d:%s:%s:%d:%d:ServerIP:%s:%d", 
										pInfo->m_iChannelingType, 
										pInfo->m_szKey.c_str(),
										szBillingGUID.c_str(),
										iType, 
										iPayAmt,
										szServerIP.c_str(),
										iClientPort);

				
				SP2Packet kPacket( BSTPK_TIMEOUT_BILLINGGUID );
				kPacket << dwUserIndex;
				kPacket << szBillingGUID;
				kPacket << pInfo->m_iChannelingType;
				kPacket << pInfo->m_eType;
				
				if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
				{
					LOG.PrintTimeAndLog( 0, "[warning][timeout]BillInfoMgr::DeleteMaxAliveTimeoutInfo Billing Info:AT_OUTPUT GameServer Send Fail: %s:%s:%d:%d:%s:%d", 
											pInfo->m_szKey.c_str(),
											szBillingGUID.c_str(),
											iType, 
											iPayAmt,
											szServerIP.c_str(),
											iClientPort);
				}
			}
			else if( pInfo->m_eType == BillInfoManager::AT_SUBSCRIPTIONRETRACT )
			{
				DWORD		 dwUserIndex = 0;
				ioHashString szBillingGUID;
				int          iChannelingType = 0;
				ioHashString szServerIP;
				int          iClientPort   = 0;
				DWORD		 dwIndex = 0;
				ioHashString szChargeNo;

				pInfo->m_kPacket >> dwUserIndex >>  szBillingGUID >> szServerIP >> iClientPort >> dwIndex >> szChargeNo;
				
				LOG.PrintTimeAndLog( 0, "[warning][timeout]BillInfoManager::DeleteMaxAliveTimeoutInfo Billing Info::AT_SUBSCRIPTIONRETRACT:%d:%s:%d:%s:%s:%d:%d:%s",
										pInfo->m_iChannelingType,
										pInfo->m_szKey.c_str(),
										dwUserIndex,
										szBillingGUID.c_str(),
										szServerIP.c_str(),
										iClientPort,
										dwIndex,
										szChargeNo.c_str());

				SP2Packet kPacket( BSTPK_TIMEOUT_BILLINGGUID );
				kPacket << dwUserIndex;
				kPacket << szBillingGUID;
				kPacket << pInfo->m_iChannelingType;
				kPacket << pInfo->m_eType;
				
				if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
				{
					LOG.PrintTimeAndLog( 0, "[warning][timeout]BillInfoManager::DeleteMaxAliveTimeoutInfo Billing Info::AT_SUBSCRIPTIONRETRACT GameServer Send Fail: %d:%d:%s:%s:%s:%d:%s", 
											pInfo->m_iChannelingType,
											dwUserIndex, 
											pInfo->m_szKey.c_str(),
											szBillingGUID.c_str(),
											szServerIP.c_str(),
											iClientPort,
											szChargeNo.c_str());
				}
									
			}

			iter = m_billInfos.erase( iter );
			pInfo->InitData();
			m_billInfoPool.Push( pInfo );	
		}
		else
		{
			++iter;
		}
	}
}


BillInfoManager::BillingInfo * BillInfoManager::PopBillInfo()
{
	BillingInfo * pInfo = (BillingInfo*)m_billInfoPool.Pop();
	
	if( pInfo )
	{
		pInfo->InitData();
	}
	else
	{
		LOG.PrintTimeAndLog(0, "[error][billinfo]BillInfoManager::PopBillInfo memory empty" );
	}
	return pInfo;
}

void BillInfoManager::RemainBillInfoCount()
{
	LOG.PrintTimeAndLog(0, "BillInfoManager::RemainBillInfoCount :%d", GetCount());
}