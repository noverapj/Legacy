#include "StdAfx.h"
#include "ioBonusCashManager.h"
#include "ioDateHelp.h"
template<> ioBonusCashManager* Singleton< ioBonusCashManager >::ms_Singleton = 0;

ioBonusCashManager::ioBonusCashManager(void)
{
}


ioBonusCashManager::~ioBonusCashManager(void)
{
}

ioBonusCashManager& ioBonusCashManager::GetSingeton()
{
	return Singleton< ioBonusCashManager >::GetSingleton();
}

void ioBonusCashManager::LoadBonusCashInfo()
{
}

int ioBonusCashManager::CalcTotalAliveBonusCash()
{
	int iTotalBonusCash = 0;
	BonusCashInfoList::iterator iter;
	for ( iter = m_AliveBonusCashInfoList.begin() ; iter != m_AliveBonusCashInfoList.end() ; ++iter )
	{
		iTotalBonusCash += iter->second.iRemainingCash;
	}

	return iTotalBonusCash;
}

void ioBonusCashManager::AddExpireInfo( int iIndex, BonusCashInfo& rkInfo, const ioHashString& szFuncName )
{
	BonusCashInfoList::iterator iter = m_ExpireBonusCashInfoList.find( iIndex );
	if ( iter != m_ExpireBonusCashInfoList.end() )
	{
		LOG.PrintTimeAndLog( 0, "%s - alreay exist Expire Bonus Cash Index  - %d, Call Func : %s ", __FUNCTION__, iIndex, szFuncName );
		return;
	}
}

void ioBonusCashManager::ApplyBonusCashData( SP2Packet& rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case STPK_BONUS_CASH_ADD:
		ApplyBonusCashAdd( rkPacket );
		break;
	case STPK_BONUS_CASH_UPDATE:
		ApplyBonusCashUpdate( rkPacket );
		break;
	case STPK_BONUS_CAHS_INFO:
		ApplyBonusCashInfo( rkPacket );
		break;
	case STPK_EXPIRED_BONUS_CASH:
		ApplyBonusCashExpired( rkPacket );
		break;
	}
}

void ioBonusCashManager::ApplyBonusCashAdd( SP2Packet& rkPacket )
{
	int iIndex;
	rkPacket>>iIndex;
	BonusCashInfoList::iterator iter = m_AliveBonusCashInfoList.find( iIndex );
	if ( iter != m_AliveBonusCashInfoList.end() )
	{
		LOG.PrintTimeAndLog( 0, "%s - alreay exist Bonus Cash Index  - %d", __FUNCTION__, iIndex );
		return;
	}
	BonusCashInfo kInfo;
	__int64 iTime;
	rkPacket>>kInfo.iAcquisitionCash;
	kInfo.iRemainingCash = kInfo.iAcquisitionCash;
	rkPacket>>iTime;
	kInfo.ExpiredTime = CTime( iTime );
	m_AliveBonusCashInfoList.insert( BonusCashInfoList::value_type( iIndex, kInfo ) );

	//방법 1. 현재 캐시 + 지급된 캐시
	//int iBonusCash = g_MyInfo.GetBonusCash() + kInfo.iRemainingCash;
	//g_MyInfo.SetBonusCash( iBonusCash );

	//방법 2. 캐시 다시 계산
	int iBonusCash = CalcTotalAliveBonusCash();
	g_MyInfo.SetBonusCash( iBonusCash );
}

void ioBonusCashManager::ApplyBonusCashUpdate( SP2Packet& rkPacket )
{
	int iIndex;
	rkPacket>>iIndex;
	BonusCashInfoList::iterator iter = m_AliveBonusCashInfoList.find( iIndex );
	if ( iter == m_AliveBonusCashInfoList.end() )
	{
		LOG.PrintTimeAndLog( 0, "%s - not exist Bonus Cash Index  - %d", __FUNCTION__, iIndex );
		return;
	}
	//소비 후 돈이 남았다.
	rkPacket >> iter->second.iRemainingCash;
	//해당 보너스 캐쉬를 다 소진하였을 경우
	if ( iter->second.iRemainingCash <= 0 )
	{
		m_ExpireBonusCashInfoList.insert( BonusCashInfoList::value_type( iter->first, iter->second ) );
		m_AliveBonusCashInfoList.erase( iter );
	}

	//int iBonusCash = g_MyInfo.GetBonusCash() - iter->second.iRemainingCash;
	//g_MyInfo.SetBonusCash( iBonusCash );
		
	int iBonusCash = CalcTotalAliveBonusCash();
	g_MyInfo.SetBonusCash( iBonusCash );
}

void ioBonusCashManager::ApplyBonusCashInfo( SP2Packet& rkPacket )
{
	int iType;
	rkPacket >> iType;

	if ( iType == BONUS_CASH_ALIVE )
		ApplyBonusCashAliveInfo( rkPacket );
	else if ( iType == BONUS_CASH_EXPIRATION )
		ApplyBonusCashExpireInfo( rkPacket );
	else
		LOG.PrintTimeAndLog( 0, "%s - not exist Type - %d", __FUNCTION__, iType );
}

void ioBonusCashManager::ApplyBonusCashAliveInfo( SP2Packet& rkPacket )
{
	m_AliveBonusCashInfoList.clear();
	int iIndex, iSize;
	rkPacket >> iSize;
	int iBonusCash = 0;
	for ( int i=0; i<iSize ; ++i )
	{
		rkPacket>>iIndex;
		BonusCashInfoList::iterator iter = m_AliveBonusCashInfoList.find( iIndex );
		if ( iter != m_AliveBonusCashInfoList.end() )
		{
			LOG.PrintTimeAndLog( 0, "%s - alreay exist Bonus Cash Index  - %d", __FUNCTION__, iIndex );
			return;
		}

		BonusCashInfo kInfo;
		__int64 iTime;
		rkPacket>>kInfo.iAcquisitionCash;
		rkPacket>>kInfo.iRemainingCash;
		rkPacket>>iTime;
		kInfo.ExpiredTime = CTime( iTime );
		m_AliveBonusCashInfoList.insert( BonusCashInfoList::value_type( iIndex, kInfo ) );
		iBonusCash+=kInfo.iRemainingCash;
	}
	g_MyInfo.SetBonusCash( iBonusCash );
}

void ioBonusCashManager::ApplyBonusCashExpireInfo( SP2Packet& rkPacket )
{
	m_ExpireBonusCashInfoList.clear();
	int iIndex, iSize;
	rkPacket >> iSize;
	int iBonusCash = 0;
	for ( int i=0; i<iSize ; ++i )
	{
		rkPacket>>iIndex;
		BonusCashInfoList::iterator iter = m_ExpireBonusCashInfoList.find( iIndex );
		if ( iter != m_ExpireBonusCashInfoList.end() )
		{
			LOG.PrintTimeAndLog( 0, "%s - alreay exist Bonus Cash Index  - %d", __FUNCTION__, iIndex );
			return;
		}

		BonusCashInfo kInfo;
		__int64 iTime;
		rkPacket>>kInfo.iAcquisitionCash;
		rkPacket>>kInfo.iRemainingCash;
		rkPacket>>iTime;
		kInfo.ExpiredTime = CTime( iTime );
		m_ExpireBonusCashInfoList.insert( BonusCashInfoList::value_type( iIndex, kInfo ) );
	}
}

void ioBonusCashManager::ApplyBonusCashExpired( SP2Packet& rkPacket )
{
	int iSize, iIndex;
	rkPacket >> iSize;
	//int iBonusCash = g_MyInfo.GetBonusCash();
	for ( int i=0; i<iSize ; ++i )
	{
		rkPacket>>iIndex;
		BonusCashInfoList::iterator iter = m_AliveBonusCashInfoList.find( iIndex );
		if ( iter == m_AliveBonusCashInfoList.end() )
		{
			LOG.PrintTimeAndLog( 0, "%s - not exist Bonus Cash Index  - %d", __FUNCTION__, iIndex );
			return;
		}
		//iBonusCash -= iter->second.iRemainingCash;
		m_ExpireBonusCashInfoList.insert( BonusCashInfoList::value_type( iter->first, iter->second ) );
		m_AliveBonusCashInfoList.erase( iter );
	}

	int iBonusCash = CalcTotalAliveBonusCash();
	g_MyInfo.SetBonusCash( iBonusCash );
}

const ioBonusCashManager::BonusCashInfo& ioBonusCashManager::GetConstAliveBonusCashInfoByIndex( int iIndex ) const
{
	static const BonusCashInfo kInfo;
	BonusCashInfoList::const_iterator iter = m_AliveBonusCashInfoList.find( iIndex );
	if ( iter != m_AliveBonusCashInfoList.end() )
	{
		return iter->second;
	}
	return kInfo;
}

bool ioBonusCashManager::GetAliveBonusCashInfoByIndex( int iIndex, BonusCashInfo& rkInfo )
{
	int iSize = m_AliveBonusCashInfoList.size();
	BonusCashInfoList::iterator iter = m_AliveBonusCashInfoList.find( iIndex );
	if ( iter != m_AliveBonusCashInfoList.end() )
		rkInfo = iter->second;
	return false;
}

const ioBonusCashManager::BonusCashInfo& ioBonusCashManager::GetConstAliveBonusCashInfoByArray( int iArray ) const
{
	static const BonusCashInfo kInfo;

	BonusCashInfoList::const_iterator iter;
	for( iter = m_AliveBonusCashInfoList.begin();iter != m_AliveBonusCashInfoList.end(); iter++)
	{
		if ( --iArray < 0 )
			return iter->second;
	}
	return kInfo;
}

const ioBonusCashManager::BonusCashInfo& ioBonusCashManager::GetConstExpireBonusCashInfoByIndex( int iIndex ) const
{
	static const BonusCashInfo kInfo;
	BonusCashInfoList::const_iterator iter = m_ExpireBonusCashInfoList.find( iIndex );
	if ( iter != m_ExpireBonusCashInfoList.end() )
	{
		return iter->second;
	}
	return kInfo;
}

const ioBonusCashManager::BonusCashInfo& ioBonusCashManager::GetConstExpireBonusCashInfoByArray( int iArray ) const
{
	static const BonusCashInfo kInfo;

	BonusCashInfoList::const_iterator iter;
	for( iter = m_ExpireBonusCashInfoList.begin();iter != m_ExpireBonusCashInfoList.end(); iter++)
	{
		if ( --iArray < 0 )
			return iter->second;
	}
	return kInfo;
}

bool ioBonusCashManager::CheckAlarmExpireBonusCash()
{
	if ( m_AliveBonusCashInfoList.empty() )
		return false;

	CTime MinTime;
	BonusCashInfoList::const_iterator iter = m_AliveBonusCashInfoList.begin();
	MinTime = iter->second.ExpiredTime.GetTime();
	for( ;iter != m_AliveBonusCashInfoList.end(); iter++)
	{
		if ( MinTime.GetTime() > iter->second.ExpiredTime.GetTime() )
			MinTime = iter->second.ExpiredTime;
	}
	CTime kCurrTime = DateHelp::GetCurrTime();
	CTimeSpan Gap = MinTime - kCurrTime;

	//하루 이하일 경우
	enum { LIMITE_HOUR = 24, };
	if ( Gap.GetTotalHours() <= 24 )
		return true;

	return false;
}
