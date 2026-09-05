
#include "stdafx.h"

#include "ioUserRecord.h"

ioUserRecord::ioUserRecord()
{
	// ¹Ì¸® Å×ÀÌºí »ý¼ºÇÑ´Ù
	RecordData kRecord;
	m_UserRecord.insert( RecordMap::value_type( RECORD_BATTLE, kRecord ) );
	m_UserRecord.insert( RecordMap::value_type( RECORD_LADDER, kRecord ) );
	m_UserRecord.insert( RecordMap::value_type( RECORD_HEROMATCH, kRecord ) );
}

ioUserRecord::~ioUserRecord()
{
	m_UserRecord.clear();
}

void ioUserRecord::Initialize()
{
	RecordMap::iterator iter;
	for( iter=m_UserRecord.begin() ; iter!=m_UserRecord.end() ; ++iter )
	{
		RecordData &rkRecord = iter->second;
		rkRecord.Init();
	}
	m_HeroSeasonRecord.Init();
}

void ioUserRecord::ApplyRecordData( SP2Packet &rkPacket )
{
	RecordMap::iterator iter;
	for( iter=m_UserRecord.begin() ; iter!=m_UserRecord.end() ; ++iter )
	{
		RecordData &rkRecord = iter->second;
		rkPacket >> rkRecord.m_iWin >> rkRecord.m_iLose >> rkRecord.m_iKill >> rkRecord.m_iDeath;
	}
	rkPacket >> m_HeroSeasonRecord.m_iWin >> m_HeroSeasonRecord.m_iLose >> m_HeroSeasonRecord.m_iKill >> m_HeroSeasonRecord.m_iDeath;
}

void ioUserRecord::ApplyHeroSeasonRecord( SP2Packet &rkPacket )
{
	rkPacket >> m_HeroSeasonRecord.m_iWin >> m_HeroSeasonRecord.m_iLose >> m_HeroSeasonRecord.m_iKill >> m_HeroSeasonRecord.m_iDeath;
}

int ioUserRecord::GetTotalKill()
{
	int iReturnKill = 0;
	RecordMap::iterator iter;
	for(iter = m_UserRecord.begin();iter != m_UserRecord.end();++iter)
	{
		RecordData &rkRecord = iter->second;
		iReturnKill += rkRecord.m_iKill;
	}
	return iReturnKill;
}

int ioUserRecord::GetTotalDeath()
{
	int iReturnDeath = 0;
	RecordMap::iterator iter;
	for(iter = m_UserRecord.begin();iter != m_UserRecord.end();++iter)
	{
		RecordData &rkRecord = iter->second;
		iReturnDeath += rkRecord.m_iDeath;
	}
	return iReturnDeath;
}

float ioUserRecord::GetKillDeathPer()
{
	int iKill  = max( 100, GetTotalKill() + 100 );
	int iDeath = max( 100, GetTotalDeath() + 100 );
	return (float)iKill / (float)iDeath;
}

float ioUserRecord::GetWinLoseRate( RecordType eRecordType )
{
	int iWin  = (float)GetRecordWin( eRecordType ) / 30;
	int iLose = (float)GetRecordLose( eRecordType ) / 10;
	return (float)iWin / (float)max( 1, iWin + iLose );
}

float ioUserRecord::GetHeroSeasonWinLoseRate()
{
	int iWin  = (float)GetHeroSeasonRecordWin() / 30;
	int iLose = (float)GetHeroSeasonRecordLose() / 10;
	return (float)iWin / (float)max( 1, iWin + iLose );
}

void ioUserRecord::AddRecordWin( RecordType eRecordType, int iCount )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		rkRecord.m_iWin += iCount;

		// ¿µ¿õÀü ½ÃÁð
		if( eRecordType == RECORD_HEROMATCH )
			m_HeroSeasonRecord.m_iWin += iCount;
	}
	else
		LOG.PrintTimeAndLog( 0, "ioUserRecord::AddRecordWin Not RecordType : %d", (int)eRecordType );
}

void ioUserRecord::AddRecordLose( RecordType eRecordType, int iCount )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		rkRecord.m_iLose += iCount;

		// ¿µ¿õÀü ½ÃÁð
		if( eRecordType == RECORD_HEROMATCH )
			m_HeroSeasonRecord.m_iLose += iCount;
	}
	else
		LOG.PrintTimeAndLog( 0, "ioUserRecord::AddRecordLose Not RecordType : %d", (int)eRecordType );
}

void ioUserRecord::AddRecordKill( RecordType eRecordType, int iCount )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		rkRecord.m_iKill = max( 0, rkRecord.m_iKill + iCount );

		// ¿µ¿õÀü ½ÃÁð
		if( eRecordType == RECORD_HEROMATCH )
			m_HeroSeasonRecord.m_iKill = max( 0, m_HeroSeasonRecord.m_iKill + iCount );
	}
	else
		LOG.PrintTimeAndLog( 0, "ioUserRecord::AddRecordKill Not RecordType : %d", (int)eRecordType );
}

void ioUserRecord::AddRecordDeath( RecordType eRecordType, int iCount )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		rkRecord.m_iDeath += iCount;

		// ¿µ¿õÀü ½ÃÁð
		if( eRecordType == RECORD_HEROMATCH )
			m_HeroSeasonRecord.m_iDeath += iCount;
	}
	else
		LOG.PrintTimeAndLog( 0, "ioUserRecord::AddRecordDeath Not RecordType : %d", (int)eRecordType );
}

__int64 ioUserRecord::GetRecordEXP( RecordType eRecordType )
{
	int iWin, iLose, iKill, iDeath;
	iWin = iLose = iKill = iDeath = 0;

	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		iWin   += rkRecord.m_iWin;
		iLose  += rkRecord.m_iLose;
		iKill  += rkRecord.m_iKill;
		iDeath += rkRecord.m_iDeath;
	}
	__int64 iTotal = (__int64)iWin + (__int64)iLose;
	if( iTotal <= 0 )
		return 0;

	float fWinPer = FLOAT1;
	if( iKill + iDeath > 0 )
		fWinPer = (float)iKill / ( iKill + iDeath );

	return iTotal * fWinPer;
}

void ioUserRecord::SetRecordInfo( RecordType eRecordType, int iWin, int iLose, int iKill, int iDeath )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		rkRecord.m_iWin   = iWin;
		rkRecord.m_iLose  = iLose;
		rkRecord.m_iKill  = iKill;
		rkRecord.m_iDeath = iDeath;
	}
	else
		LOG.PrintTimeAndLog( 0, "ioUserRecord::SetRecordInfo Not RecordType : %d", (int)eRecordType );
}

void ioUserRecord::SetHeroSeasonRecordInfo( int iWin, int iLose, int iKill, int iDeath )
{
	m_HeroSeasonRecord.m_iWin   = iWin;
	m_HeroSeasonRecord.m_iLose  = iLose;
	m_HeroSeasonRecord.m_iKill  = iKill;
	m_HeroSeasonRecord.m_iDeath = iDeath;
}

int ioUserRecord::GetRecordWin( RecordType eRecordType )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		return rkRecord.m_iWin;
	}

	return 0;
}

int ioUserRecord::GetRecordLose( RecordType eRecordType )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		return rkRecord.m_iLose;
	}

	return 0;
}

int ioUserRecord::GetHeroSeasonRecordWin()
{
	return m_HeroSeasonRecord.m_iWin;
}

int ioUserRecord::GetHeroSeasonRecordLose()
{
	return m_HeroSeasonRecord.m_iLose;
}