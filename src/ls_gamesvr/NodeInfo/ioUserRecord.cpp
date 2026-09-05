
#include "stdafx.h"

#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "User.h"

#include "ioUserRecord.h"


ioUserRecord::ioUserRecord()
{
	// ¹Ì¸® Å×ÀÌºí »ý¼ºÇÑ´Ù. ÀüÅõ : ·¡´õ : ¿µ¿õÀü
	RecordData kRecord;
	m_UserRecord.insert( RecordMap::value_type( RECORD_BATTLE, kRecord ) );
	m_UserRecord.insert( RecordMap::value_type( RECORD_LADDER, kRecord ) );
	m_UserRecord.insert( RecordMap::value_type( RECORD_HEROMATCH, kRecord ) );
	m_HeroSeasonRecord.Init();

	Initialize( NULL );
}

ioUserRecord::~ioUserRecord()
{
	m_UserRecord.clear();
}

void ioUserRecord::Initialize( User *pUser )
{
	m_pUser			= pUser;

	RecordMap::iterator iter;
	for( iter=m_UserRecord.begin() ; iter!=m_UserRecord.end() ; ++iter )
	{
		RecordData &rkRecord = iter->second;
		rkRecord.Init();
	}
	m_HeroSeasonRecord.Init();
	m_bChange = false;
}

void ioUserRecord::DBtoRecordData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserRecord::DBtoRecordData() User NULL!!"); 
		return;
	}
	Initialize( m_pUser );	
	
	RecordData kRecord;

	// ÀüÅõ	ÀüÀû
	kRecord.Init();
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iWin, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iLose, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iKill, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iDeath, sizeof( int ) ) );
	AddRecord( RECORD_BATTLE, kRecord );

	// ·¡´õ	ÀüÀû
	kRecord.Init();
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iWin, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iLose, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iKill, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iDeath, sizeof( int ) ) );
	AddRecord( RECORD_LADDER, kRecord );

	// ¿µ¿õÀü ÀüÀû
	kRecord.Init();
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iWin, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iLose, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iKill, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( kRecord.m_iDeath, sizeof( int ) ) );
	AddRecord( RECORD_HEROMATCH, kRecord );

	// ¿µ¿õÀü ½ÃÁð ÀüÀû
	m_HeroSeasonRecord.Init();
	PACKET_GUARD_VOID ( query_data->GetValue( m_HeroSeasonRecord.m_iWin, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( m_HeroSeasonRecord.m_iLose, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( m_HeroSeasonRecord.m_iKill, sizeof( int ) ) );
	PACKET_GUARD_VOID ( query_data->GetValue( m_HeroSeasonRecord.m_iDeath, sizeof( int ) ) );

	// À¯Àú¿¡°Ô Àü¼Û
	SP2Packet kPacket( STPK_USER_RECORD_DATA );
	FillRecordData( kPacket );
	m_pUser->SendMessage( kPacket );
}

void ioUserRecord::SaveRecordData()
{
	if( !m_pUser ) return;
	if( !m_bChange ) return;

	std::vector<int> contents;
	contents.reserve(m_UserRecord.size() + 4);
	
	// ÀüÅõ / Áø¿µ / ¿µ¿õÀü ±â·Ï
	RecordMap::iterator iter;
	for(iter = m_UserRecord.begin();iter != m_UserRecord.end();++iter)
	{
		RecordData &rkRecord = iter->second;
		contents.push_back( rkRecord.m_iWin );
		contents.push_back( rkRecord.m_iLose );
		contents.push_back( rkRecord.m_iKill );
		contents.push_back( rkRecord.m_iDeath );
	}

	// ¿µ¿õÀü ½ÃÁð ±â·Ï
	contents.push_back( m_HeroSeasonRecord.m_iWin );
	contents.push_back( m_HeroSeasonRecord.m_iLose );
	contents.push_back( m_HeroSeasonRecord.m_iKill );
	contents.push_back( m_HeroSeasonRecord.m_iDeath );

	g_DBClient.OnUpdateUserRecord( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), contents );
	m_bChange = false;
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveRecordData(%s)", m_pUser->GetPublicID().c_str() );
}

void ioUserRecord::AddRecord( RecordType eRecordType, const RecordData &rkInRecord )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		rkRecord = rkInRecord;
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserRecord::AddBattleRecord Not RecordType : %d", (int)eRecordType );
}

void ioUserRecord::FillRecordData( SP2Packet &rkPacket )
{
	RecordMap::iterator iter;
	for(iter = m_UserRecord.begin();iter != m_UserRecord.end();++iter)
	{
		RecordData &rkRecord = iter->second;
		PACKET_GUARD_VOID( rkPacket.Write (rkRecord.m_iWin) );
		PACKET_GUARD_VOID( rkPacket.Write (rkRecord.m_iLose) );
		PACKET_GUARD_VOID( rkPacket.Write (rkRecord.m_iKill) );
		PACKET_GUARD_VOID( rkPacket.Write (rkRecord.m_iDeath) );
	}
	FillHeroSeasonData( rkPacket );
}

void ioUserRecord::FillMoveData( SP2Packet &rkPacket )
{
	rkPacket << m_bChange;
	FillRecordData( rkPacket );
}

void ioUserRecord::FillHeroSeasonData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write( m_HeroSeasonRecord.m_iWin) );
	PACKET_GUARD_VOID( rkPacket.Write( m_HeroSeasonRecord.m_iLose) );
	PACKET_GUARD_VOID( rkPacket.Write( m_HeroSeasonRecord.m_iKill) );
	PACKET_GUARD_VOID( rkPacket.Write( m_HeroSeasonRecord.m_iDeath) );
}

void ioUserRecord::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	rkPacket >> m_bChange;

	RecordMap::iterator iter;
	for(iter = m_UserRecord.begin();iter != m_UserRecord.end();++iter)
	{
		RecordData &rkRecord = iter->second;
		rkPacket >> rkRecord.m_iWin >> rkRecord.m_iLose >> rkRecord.m_iKill >> rkRecord.m_iDeath;
	}

	// ¿µ¿õÀü ½ÃÁð ±â·Ï
	rkPacket >> m_HeroSeasonRecord.m_iWin >> m_HeroSeasonRecord.m_iLose >> m_HeroSeasonRecord.m_iKill >> m_HeroSeasonRecord.m_iDeath;
}

void ioUserRecord::GetRecordInfo( RecordType eRecordType, int &iWin, int &iLose, int &iKill, int &iDeath )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		iWin   += rkRecord.m_iWin;
		iLose  += rkRecord.m_iLose;
		iKill  += rkRecord.m_iKill;
		iDeath += rkRecord.m_iDeath;
	}
}

bool ioUserRecord::InitRecordInfo( RecordType eRecordType )
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;

		m_bChange = true;
		rkRecord.Init();
		return true;
	}
	return false;
}

bool ioUserRecord::InitHeroSeasonRecord()
{
	if( m_HeroSeasonRecord.m_iWin   != 0 ||
		m_HeroSeasonRecord.m_iLose  != 0 ||
		m_HeroSeasonRecord.m_iKill  != 0 ||
		m_HeroSeasonRecord.m_iDeath != 0)	
	{
		m_bChange = true;
		m_HeroSeasonRecord.Init();
		return true;
	}
	return false;
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

void ioUserRecord::AddRecordWin( RecordType eRecordType, int iCount )
{
	m_bChange = true;
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserRecord::AddRecordWin Not RecordType : %d", (int)eRecordType );
}

void ioUserRecord::AddRecordLose( RecordType eRecordType, int iCount )
{
	m_bChange = true;
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserRecord::AddRecordLose Not RecordType : %d", (int)eRecordType );
}

void ioUserRecord::AddRecordKill( RecordType eRecordType, int iCount )
{
	m_bChange = true;
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserRecord::AddRecordKill Not RecordType : %d", (int)eRecordType );
}

void ioUserRecord::AddRecordMaxContinuousKill(RecordType eRecordType, int iCount)
{
	RecordMap::iterator iter = m_UserRecord.find( eRecordType );
	if( iter != m_UserRecord.end() )	
	{
		RecordData &rkRecord = iter->second;
		rkRecord.m_iContinuousKill = max( iCount, rkRecord.m_iContinuousKill );

		// ¿µ¿õÀü ½ÃÁð
		if( eRecordType == RECORD_HEROMATCH )
			m_HeroSeasonRecord.m_iContinuousKill = max( iCount, m_HeroSeasonRecord.m_iContinuousKill );
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserRecord::AddRecordKill Not RecordType : %d", (int)eRecordType );
}

void ioUserRecord::AddRecordDeath( RecordType eRecordType, int iCount )
{
	m_bChange = true;
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserRecord::AddRecordDeath Not RecordType : %d", (int)eRecordType );
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

	float fWinPer = 1.0f;
	if( iKill + iDeath > 0 )
		fWinPer = (float)iKill / ( iKill + iDeath );

	return static_cast< __int64 >( iTotal * fWinPer );
}
