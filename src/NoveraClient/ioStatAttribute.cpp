
#include "stdafx.h"

#include "ioStatAttribute.h"

#include "ioBaseChar.h"

template<> ioStatAttribute* Singleton< ioStatAttribute >::ms_Singleton = 0;

ioStatAttribute::ioStatAttribute()
{
}

ioStatAttribute::~ioStatAttribute()
{
	ClearAttributeList();
}

void ioStatAttribute::ClearAttributeList()
{
	StatAttributeList::iterator iter;
	for( iter=m_StatAttributeList.begin() ; iter!=m_StatAttributeList.end() ; ++iter )
	{
		delete *iter;
	}

	m_StatAttributeList.clear();
}

void ioStatAttribute::InitINIValue()
{
	ClearAttributeList();

	char szKey[MAX_PATH], szBuf[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_stat_attribute.ini" );

	memset( m_MaxStatAttribute, 0, sizeof(m_MaxStatAttribute) );

	kLoader.SetTitle_e( "race_max_attribute" );
	for( int i=0 ; i<RT_MAX_RACE_TYPE ; i++ )
	{
		wsprintf_e( szKey, "line%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		const StringVector &vSplit = ioStringConverter::Split( szBuf, "\t " );

		int iSplitCnt = vSplit.size();
		if( iSplitCnt <= 4 )
		{
			for( int j=0 ; j<iSplitCnt ; j++ )
			{
				m_MaxStatAttribute[i][j] = atof( vSplit[j].c_str() );
			}
		}

		if( iSplitCnt != 4 )
		{
			LOG.PrintTimeAndLog( 0, "ioStatAttribute::InitINIValue - %d line rate count wrong(%d/%d)",
									i+1, iSplitCnt, MAX_ARMOR_TYPE );
		}
	}

	kLoader.SetTitle_e( "common" );

	int iAttributeCnt = kLoader.LoadInt_e( "statattribute_cnt", 0 );
	for( i=0; i < iAttributeCnt; i++ )
	{
		StatAttribute *tAttribute = new StatAttribute;

		wsprintf_e( szBuf, "attribute%d", i+1 );
		kLoader.SetTitle( szBuf );

		tAttribute->iAttributeType = kLoader.LoadInt_e( "group", 0 );
		
		kLoader.LoadString_e( "attribute_id", "", szBuf, MAX_PATH );
		tAttribute->iAttributeID = GenerateAttributeID( szBuf );

		tAttribute->fChangeWidth = kLoader.LoadFloat_e( "change_width", FLOAT05 );

		kLoader.LoadString_e( "title", "", szBuf, MAX_PATH );
		tAttribute->szTitle = szBuf;

		kLoader.LoadString_e( "desc", "", szBuf, MAX_PATH );
		tAttribute->szDescription = szBuf;
		
		m_StatAttributeList.push_back( tAttribute );
	}
}
	
float ioStatAttribute::GetCalcStatAttribute( float fTarget, int iStatAttributeID, ioBaseChar *pChar )
{
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioStatAttribute::GetCalcStatAttribute - Char is NULL" );
		return 0.0f;
	}

	StatAttribute *pAttribute = FindAttribute( iStatAttributeID );
	if( !pAttribute )
	{
		LOG.PrintTimeAndLog( 0, "ioStatAttribute::GetCalcStatAttribute - %d Not Exist", iStatAttributeID );
		return 0.0f;
	}

	float fAttributeLev = GetStatAttributeLev( iStatAttributeID, pChar );

	return g_MathManager.GetStatAttribute( fTarget, fAttributeLev, pAttribute->fChangeWidth );
}

float ioStatAttribute::GetStatAttributeLev( int iStatAttributeID, ioBaseChar *pChar )
{
	StatAttribute *pAttribute = FindAttribute( iStatAttributeID );
	if( !pAttribute )
	{
		LOG.PrintTimeAndLog( 0, "ioStatAttribute::GetStatAttributeLev - %d Not Exist", iStatAttributeID );
		return 0.0f;
	}

	float fCurLev = 0.0f;
	int iRaceType = (int)pChar->GetRaceType();
	const Stat& tStat = pChar->GetCurrentStat();

	if( iRaceType >= RT_MAX_RACE_TYPE || iRaceType < 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioStatAttribute::GetStatAttributeLev - %d Attribute Type(%d) Error",
			iStatAttributeID, (int) pAttribute->iAttributeType );

		return 0.0f;
	}

	switch( pAttribute->iAttributeType )
	{
	case SAT_STR:
		fCurLev = tStat.m_fStrength;
		fCurLev = min( fCurLev, m_MaxStatAttribute[iRaceType][0] );
		fCurLev = max( fCurLev, 0.0f );
		return fCurLev;
	case SAT_DEX:
		fCurLev = tStat.m_fDexterity;
		fCurLev = min( fCurLev, m_MaxStatAttribute[iRaceType][1] );
		fCurLev = max( fCurLev, 0.0f );
		return fCurLev;
	case SAT_INT:
		fCurLev = tStat.m_fIntelligence;
		fCurLev = min( fCurLev, m_MaxStatAttribute[iRaceType][2] );
		fCurLev = max( fCurLev, 0.0f );
		return fCurLev;
	case SAT_VIT:
		fCurLev = tStat.m_fVitality;
		fCurLev = min( fCurLev, m_MaxStatAttribute[iRaceType][3] );
		fCurLev = max( fCurLev, 0.0f );
		return fCurLev;
	}

	LOG.PrintTimeAndLog( 0, "ioStatAttribute::GetStatAttributeLev - %d Attribute Type(%d) Error",
							iStatAttributeID, (int) pAttribute->iAttributeType );

	return 0.0f;
}

StatAttribute* ioStatAttribute::FindAttribute( int iAttributeID )
{
	StatAttributeList::iterator iter;
	for( iter=m_StatAttributeList.begin() ; iter!=m_StatAttributeList.end() ; ++iter )
	{
		if( (*iter)->iAttributeID == iAttributeID )
			return *iter;
	}

	// 로그 아웃
	if( P2PNetwork::IsNetworkPlaying() )
	{
		ErrorReport::SetPosition( 9000, 2 );
		ErrorReport::SetFormat( "" );
		// 로그 서버 전송
		char szErrorPos[MAX_PATH] = "";
		/*SafeSprintf( szErrorPos, sizeof( szErrorPos ), STR(1), g_MyInfo.GetMyIP().c_str(),
																									g_MyInfo.GetPrivateID().c_str(),
																									g_MyInfo.GetPublicID().c_str(),
																									ErrorReport::GetMajorPosition(),
																									ErrorReport::GetMinorPosition(),
																									ErrorReport::GetEngineMajorPos(),
																									ErrorReport::GetEngineMinorPos(),
																									ErrorReport::GetString(),
																									iAttributeID );*/

		sprintf_s( szErrorPos, sizeof( szErrorPos ), STR(1), g_MyInfo.GetMyIP().c_str(),
			g_MyInfo.GetPrivateID().c_str(),
			g_MyInfo.GetPublicID().c_str(),
			ErrorReport::GetMajorPosition(),
			ErrorReport::GetMinorPosition(),
			ErrorReport::GetEngineMajorPos(),
			ErrorReport::GetEngineMinorPos(),
			ErrorReport::GetString(),
			iAttributeID );


		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ClientError";  // 로그 파일 타입
		kPacket << szErrorPos;
		kPacket << 317;  // 오류번호:317
		kPacket << false; // write db
		P2PNetwork::SendLog(kPacket);
	}

	LOG.PrintTimeAndLog( 0, "ExitProgram - 11" );

#if defined( USE_GA )
	if( g_App.GetGAStart() == true )
	{
		char chLabel[32] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			sprintf_e( chLabel, "%d", 11 );
		else
			SafeSprintf( chLabel, sizeof(chLabel), "%1", 11 );

		// GAME_END_ERR_POS
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Game"
			, "Error"
			, chLabel
			, 1
			, "%2FGAME%2FOVER%2FERR"
			, 1 );

		g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
	}
#endif

	g_App.SetExitProgram();
	LOG.PrintTimeAndLog( 0, "ioStatAttribute::FindAttribute - %d Attribute Not Exist", iAttributeID );
	return NULL;
}

ioStatAttribute& ioStatAttribute::GetSingleton()
{
	return Singleton< ioStatAttribute >::GetSingleton();
}

int ioStatAttribute::GenerateAttributeID( const ioHashString &szID )
{
	if( szID == "STR_DAMAGE_UP" )
		return SA_STR_DAMAGE_UP;
	else if( szID == "PUSH_AMT_UP" )
		return SA_STR_PUSH_AMT_UP;
	else if( szID == "BLOW_AMT_UP" )
		return SA_STR_BLOW_AMT_UP;
	else if( szID == "FORCE_AMT_UP" )
		return SA_STR_FORCE_AMT_UP;
	else if( szID == "WEIGHT_DOWN" )
		return SA_STR_WEIGHT_DOWN;
	else if( szID == "STR_GAUGE_FILL_UP" )
		return SA_STR_GAUGE_FILL_UP;
	else if( szID == "DEX_DAMAGE_UP" )
		return SA_DEX_DAMAGE_UP;
	else if( szID == "MOVE_SPEED_UP" )
		return SA_DEX_MOVE_SPEED_UP;
	else if( szID == "FASTGETUP_UP" )
		return SA_DEX_FASTGETUP_SPEED_UP;
	else if( szID == "DASH_COOLTIME_DOWN" )
		return SA_DEX_DASH_COOLTIME_DOWN;
	else if( szID == "DEX_GAUGE_FILL_UP" )
		return SA_DEX_GAUGE_FILL_UP;
	else if( szID == "INT_DAMAGE_UP" )
		return 	SA_INT_DAMAGE_UP;
	else if( szID == "SKILL_READYTIME_DOWN" )
		return 	SA_INT_SKILL_READYTIME_DOWN;
	else if( szID == "ABNORMAL_TIME_DOWN" )
		return 	SA_INT_ABNORMAL_TIME_DOWN;
	else if( szID == "INT_DAMAGE_DOWN" )
		return 	SA_INT_INT_DAMAGE_DOWN;
	else if( szID == "INT_GAUGE_FILL_UP" )
		return SA_INT_GAUGE_FILL_UP;
	else if( szID == "MAX_HP_UP" )
		return SA_VIT_MAX_HP_UP;
	else if( szID == "FALL_DAMAGE_DOWN" )
		return SA_VIT_FALL_DAMAGE_DOWN;
	else if( szID == "AC_UP" )
		return SA_VIT_AC_UP;
	else if( szID == "SCAIL_UP" )
		return SA_VIT_SCAIL_UP;
	else if( szID == "VIT_GAUGE_FILL_UP" )
		return SA_VIT_GAUGE_FILL_UP;

	return -1;
}

void ioStatAttribute::GetStatAttributeTitle( int iStatAttributeID, ioHashString &szTitle )
{
	StatAttributeList::iterator iter;
	for( iter=m_StatAttributeList.begin(); iter != m_StatAttributeList.end(); ++iter )
	{
		StatAttribute *pAttribute = *iter;
		if( pAttribute->iAttributeID == iStatAttributeID )
		{
			szTitle = pAttribute->szTitle;
			break;
		}
	}
}

void ioStatAttribute::GetStatAttributeDesc( int iStatAttributeID, ioHashString &szDesc )
{
	StatAttributeList::iterator iter;
	for( iter=m_StatAttributeList.begin(); iter != m_StatAttributeList.end(); ++iter )
	{
		StatAttribute *pAttribute = *iter;
		if( pAttribute->iAttributeID == iStatAttributeID )
		{
			szDesc = pAttribute->szDescription;
			break;
		}
	}
}

int ioStatAttribute::GetAttributeCount()
{
	return m_StatAttributeList.size();
}

bool ioStatAttribute::CheckStatType( int iType, int iOffSet )
{
	if( COMPARE( iOffSet, 0, GetAttributeCount() ) )
	{
		if( m_StatAttributeList[iOffSet]->iAttributeType == iType )
			return true;
	}

	return false;
}

float ioStatAttribute::GetAverageMoveSpeed( float fDefaultSpeed )
{
	StatAttribute *pAttribute = FindAttribute( SA_DEX_MOVE_SPEED_UP );
	if( !pAttribute )
	{
		LOG.PrintTimeAndLog( 0, "ioStatAttribute::GetAverageMoveSpeed - %d Not Exist",
								SA_DEX_MOVE_SPEED_UP );
		return 0.0f;
	}

	float fMaxMoveSpeed = fDefaultSpeed * ( FLOAT1 + ( pAttribute->fChangeWidth ) );
	float fAverage = ( fMaxMoveSpeed + fDefaultSpeed ) * FLOAT05;
	
	return fAverage;
}
