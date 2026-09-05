
#include "stdafx.h"

#include "ioCampManager.h"

#include "ioLadderTeamMgr.h"
#include "ioDateHelp.h"

#include "GUI/ioSP2GUIManager.h"
#include "GUI/ioMessageBox.h"

#include "ioTournamentManager.h"

template<> ioCampManager* Singleton< ioCampManager >::ms_Singleton = 0;
ioCampManager::ioCampManager()
{
	m_dwCurTimer = m_dwSyncTime = m_iLimitCampPoint = m_dwProcessTime = m_iInfluenceCloseEntry = m_iNextTodayBattleSec = m_iCurTodayBattleSec = m_iCampHelpID = m_iCampBonusHelpID = 0;
	m_fSeasonBonus = m_fCampBonusCorrectionA = m_fCampBonusCorrectionB = m_fCampBonusDefaultA = m_fCampBonusDefaultB = 0.0f;
	m_iCampBonusMinEntry = m_iCampBonusMaxEntry = 0;
	m_bTodayBattleEndAlarm = true;
	m_SeasonState = SS_NONE;

	m_iCloseCheckMin = 60;
	m_bSendSeasonReward = false;
}

ioCampManager::~ioCampManager()
{
	m_CampData.clear();
	m_HeroRankData.clear();
	m_HeroMatchHistory.clear();
	m_EntryMentTitle.m_MentList.clear();

	int i = 0;
	int iMentLine = m_EntryMentContent.size();
	for(i = 0;i < iMentLine;i++)
	{
		EntryMentList &kMentList = m_EntryMentContent[i];
		kMentList.m_MentList.clear();
	}
	m_EntryMentContent.clear();

	iMentLine = m_BlueAlarmMent.size();
	for(i = 0;i < iMentLine;i++)
	{
		AlarmMent &kMentList = m_BlueAlarmMent[i];
		kMentList.m_MentList.clear();
	}
	m_BlueAlarmMent.clear();

	iMentLine = m_RedAlarmMent.size();
	for(i = 0;i < iMentLine;i++)
	{
		AlarmMent &kMentList = m_RedAlarmMent[i];
		kMentList.m_MentList.clear();
	}
	m_RedAlarmMent.clear();
}

ioCampManager& ioCampManager::GetSingleton()
{
	return Singleton< ioCampManager >::GetSingleton();
}

void ioCampManager::LoadINIData()
{
	ioINILoader_e kLoader( "config/sp2_camp.ini" );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	kLoader.SetTitle_e( "Info" );
	m_dwSyncTime = kLoader.LoadInt_e( "SyncTime", 10000 );	
	m_fSeasonBonus = kLoader.LoadFloat_e( "SeasonBonus", 0.0f );
	m_iLimitCampPoint = kLoader.LoadInt_e( "LimitCampPoint", 100 );	
	m_iInfluenceCloseEntry = kLoader.LoadInt_e( "InfluenceCloseEntry", 0 );
	m_iCampHelpID = kLoader.LoadInt_e( "CampHelpID", 0 );
	m_iCampBonusHelpID = kLoader.LoadInt_e( "CampBonusHelpID", 0 );
	m_fCampBonusCorrectionA = kLoader.LoadFloat_e( "CampBonusCorrectionA", FLOAT05 );
	m_fCampBonusCorrectionB = kLoader.LoadFloat_e( "CampBonusCorrectionB", 0.55f );
	m_fCampBonusDefaultA    = kLoader.LoadFloat_e( "CampBonusDefaultA", 20000 );
	m_fCampBonusDefaultB    = kLoader.LoadFloat_e( "CampBonusDefaultB", 2000 );
	m_iCampBonusMinEntry    = kLoader.LoadInt_e( "CampBonusMinEntry", 200 );
	m_iCampBonusMaxEntry    = kLoader.LoadInt_e( "CampBonusMaxEntry", 2000 );

	int i, iMaxLine, iMaxInfluence;
	m_EntryMentTitle.m_MentList.clear();
	kLoader.SetTitle_e( "entry_title" );
	iMaxLine = kLoader.LoadInt_e( "line_count", 0 );
	for(i = 0;i < iMaxLine;i++)
	{
		MentData kData;
		sprintf_e( szKey, "line_color%d", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		char *pTemp = NULL;
		kData.m_dwMentColor = strtoul( szBuf, &pTemp, 16 );

		sprintf_e( szKey, "line_text%d", i + 1 );
		kLoader.LoadString( szKey, "", kData.m_szMentText, MAX_PATH );
		m_EntryMentTitle.m_MentList.push_back( kData );
    }

	kLoader.SetTitle_e( "entry_content" );
	iMaxLine = kLoader.LoadInt_e( "max_line", 0 );
	for(i = 0;i < iMaxLine;i++)
	{	
		EntryMentList kEntryList;

		sprintf_e( szKey, "line%d_offsetx", i + 1 );
		kEntryList.m_iOffsetX = kLoader.LoadInt( szKey, 0 );
		sprintf_e( szKey, "line%d_offsety", i + 1 );
		kEntryList.m_iOffsetY = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "line%d_count", i + 1 );
		int ilineCount = kLoader.LoadInt( szKey, 0 );
		for(int j = 0;j < ilineCount;j++)
		{
			MentData kData;
			sprintf_e( szKey, "line%d_color%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			char *pTemp = NULL;
			kData.m_dwMentColor = strtoul( szBuf, &pTemp, 16 );

			sprintf_e( szKey, "line%d_text%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", kData.m_szMentText, MAX_PATH );
			kEntryList.m_MentList.push_back( kData );
		}
		m_EntryMentContent.push_back( kEntryList );
	}

	kLoader.SetTitle_e( "blue_ment" );
	m_BlueAlarmMent.clear();
	iMaxInfluence = kLoader.LoadInt_e( "max_influence", 0 );
	for(i = 0;i < iMaxInfluence;i++)
	{
		AlarmMent kAlarmMent;
		sprintf_e( szKey, "check%d_influence", i + 1 );
		kAlarmMent.m_iInfluence = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "check%d_count", i + 1 );
		int ilineCount = kLoader.LoadInt( szKey, 0 );
		for(int j = 0;j < ilineCount;j++)
		{
			MentData kData;
			sprintf_e( szKey, "check%d_color%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			char *pTemp = NULL;
			kData.m_dwMentColor = strtoul( szBuf, &pTemp, 16 );

			sprintf_e( szKey, "check%d_text%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", kData.m_szMentText, MAX_PATH );
			kAlarmMent.m_MentList.push_back( kData );
		}
		m_BlueAlarmMent.push_back( kAlarmMent );
	}

	{
		m_BlueRecessMent.clear();
		sprintf_e( szKey, "recess_count" );
		int irecess_count = kLoader.LoadInt( szKey, 0 );
		for(i = 0;i < irecess_count;i++)
		{
			MentData kData;
			sprintf_e( szKey, "recess_color%d", i + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			char *pTemp = NULL;
			kData.m_dwMentColor = strtoul( szBuf, &pTemp, 16 );

			sprintf_e( szKey, "recess__text%d", i + 1 );
			kLoader.LoadString( szKey, "", kData.m_szMentText, MAX_PATH );
			m_BlueRecessMent.push_back( kData );
		}
	}

	m_RedAlarmMent.clear();
	kLoader.SetTitle_e( "red_ment" );
	iMaxInfluence = kLoader.LoadInt_e( "max_influence", 0 );
	for(i = 0;i < iMaxInfluence;i++)
	{
		AlarmMent kAlarmMent;
		sprintf_e( szKey, "check%d_influence", i + 1 );
		kAlarmMent.m_iInfluence = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "check%d_count", i + 1 );
		int ilineCount = kLoader.LoadInt( szKey, 0 );
		for(int j = 0;j < ilineCount;j++)
		{
			MentData kData;
			sprintf_e( szKey, "check%d_color%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			char *pTemp = NULL;
			kData.m_dwMentColor = strtoul( szBuf, &pTemp, 16 );

			sprintf_e( szKey, "check%d_text%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", kData.m_szMentText, MAX_PATH );
			kAlarmMent.m_MentList.push_back( kData );
		}
		m_RedAlarmMent.push_back( kAlarmMent );
	}

	{
		m_RedRecessMent.clear();
		sprintf_e( szKey, "recess_count" );
		int irecess_count = kLoader.LoadInt( szKey, 0 );
		for(i = 0;i < irecess_count;i++)
		{
			MentData kData;
			sprintf_e( szKey, "recess_color%d", i + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			char *pTemp = NULL;
			kData.m_dwMentColor = strtoul( szBuf, &pTemp, 16 );

			sprintf_e( szKey, "recess__text%d", i + 1 );
			kLoader.LoadString( szKey, "", kData.m_szMentText, MAX_PATH );
			m_RedRecessMent.push_back( kData );
		}
	}
}

void ioCampManager::SyncCampData()
{
	if( m_dwCurTimer == 0 || FRAMEGETTIME() - m_dwCurTimer > m_dwSyncTime )
	{
		m_dwCurTimer = FRAMEGETTIME();
		// SEND
		SP2Packet kPacket( CTPK_CAMP_DATA_SYNC );
		TCPNetwork::SendToServer( kPacket );
	}
}

void ioCampManager::ChangeCampData( CampData &rkCampData )
{
	int iSize = m_CampData.size();
	for(int i = 0;i < iSize;i++)
	{
		if( m_CampData[i].m_iCampType == rkCampData.m_iCampType )
		{
			m_CampData[i] = rkCampData;
			return;
		}
	}
	m_CampData.push_back( rkCampData );
}

int ioCampManager::GetCampPoint( CampType eCampType )
{
	int iSize = m_CampData.size();
	for(int i = 0;i < iSize;i++)
	{
		CampData &rkCampData = m_CampData[i];
		if( (CampType)rkCampData.m_iCampType == eCampType )
			return rkCampData.m_iCampPoint;
	}
	return 0;
}

int ioCampManager::GetCampTodayPoint( CampType eCampType )
{
	int iSize = m_CampData.size();
	for(int i = 0;i < iSize;i++)
	{
		CampData &rkCampData = m_CampData[i];
		if( (CampType)rkCampData.m_iCampType == eCampType )
			return rkCampData.m_iCampTodayPoint;
	}
	return 0;
}

int ioCampManager::GetCampBonusPoint( CampType eCampType )
{
	int iSize = m_CampData.size();
	for(int i = 0;i < iSize;i++)
	{
		CampData &rkCampData = m_CampData[i];
		if( (CampType)rkCampData.m_iCampType == eCampType )
			return rkCampData.m_iCampBonusPoint;
	}
	return 0;
}

int ioCampManager::GetCampEntryUserCount( CampType eCampType )
{
	int iSize = m_CampData.size();
	for(int i = 0;i < iSize;i++)
	{
		CampData &rkCampData = m_CampData[i];
		if( (CampType)rkCampData.m_iCampType == eCampType )
			return rkCampData.m_iCampEntryUserCount;
	}
	return 0;
}

int ioCampManager::GetCampSpecialEntryUserCount( CampType eCampType )
{
	int iSize = m_CampData.size();
	for(int i = 0;i < iSize;i++)
	{
		CampData &rkCampData = m_CampData[i];
		if( (CampType)rkCampData.m_iCampType == eCampType )
			return rkCampData.m_iCampSpecialEntryUserCount;
	}
	return 0;
}

float ioCampManager::GetCampInfluencePointF( CampType eCampType )
{
	int iBluePoint = GetCampPoint( CAMP_BLUE ) + GetCampBonusPoint( CAMP_BLUE );
	int iRedPoint = GetCampPoint( CAMP_RED ) + GetCampBonusPoint( CAMP_RED );
	int iTotalGauge = iBluePoint + iRedPoint;
	if( iTotalGauge <= 0 )
		return FLOAT05;
	else if( eCampType == CAMP_BLUE )
		return ( (float)iBluePoint / (float)iTotalGauge ) + 0.00005f ;
	else if( eCampType == CAMP_RED )
		return ( (float)iRedPoint / (float)iTotalGauge ) + 0.00005f;
	return FLOAT05;
}

int ioCampManager::GetCampInfluencePoint( CampType eCampType )
{
	return GetCampInfluencePointF( eCampType ) * 100;
}

CampType ioCampManager::GetDefaultCampType()
{
	float fBlueInfluence = GetCampInfluencePointF( CAMP_BLUE );
	float fRedInfluence = GetCampInfluencePointF( CAMP_RED );
	int iBlueEntryUser = GetCampEntryUserCount( CAMP_BLUE );
	int iRedEntryUser = GetCampEntryUserCount( CAMP_RED );

	if( fBlueInfluence > fRedInfluence )
		return CAMP_RED;
	else if( fBlueInfluence < fRedInfluence )
		return CAMP_BLUE;
	else if( iBlueEntryUser > iRedEntryUser )
		return CAMP_RED;
	else if( iBlueEntryUser < iRedEntryUser )
		return CAMP_BLUE;
	return CAMP_BLUE;
}

bool ioCampManager::IsCloseEntry( CampType eCampType )
{
	if( GetCampInfluencePoint( eCampType ) >= m_iInfluenceCloseEntry )
		return true;

	return false;
}

const ioHashString &ioCampManager::GetStartHourText()
{
	static ioHashString szReturn = "";
	char szHour[MAX_PATH]="";
	if( COMPARE( GetStartSeasonHour(), 0, 12 ) )
		SafeSprintf( szHour, sizeof( szHour ), STR(1), GetStartSeasonHour() );
	else if( GetStartSeasonHour() == 12 )
		SafeSprintf( szHour, sizeof( szHour ), STR(2), GetStartSeasonHour() );
	else
		SafeSprintf( szHour, sizeof( szHour ), STR(3), GetStartSeasonHour() - 12 );
	szReturn = szHour;
	return szReturn;
}

const ioHashString &ioCampManager::GetNextHourText()
{
	static ioHashString szReturn = "";
	char szHour[MAX_PATH]="";
	if( COMPARE( GetNextSeasonHour(), 0, 12 ) )
		SafeSprintf( szHour, sizeof( szHour ), STR(1), GetNextSeasonHour() );
	else if( GetNextSeasonHour() == 12 )
		SafeSprintf( szHour, sizeof( szHour ), STR(2), GetNextSeasonHour() );
	else
		SafeSprintf( szHour, sizeof( szHour ), STR(3), GetNextSeasonHour() - 12 );
	szReturn = szHour;
	return szReturn;
}

DWORD ioCampManager::GetGuildRank( int iGuildRank )
{
	if( COMPARE( iGuildRank, 0, (int)m_GuildRank.size() ) )
		return m_GuildRank[iGuildRank];
	return 0;
}

void ioCampManager::SetCampPoint( CampType eCampType, const int iCampPoint )
{
	int iSize = m_CampData.size();
	for(int i = 0;i < iSize;i++)
	{
		CampData &rkCampData = m_CampData[i];
		if( (CampType)rkCampData.m_iCampType == eCampType )
			rkCampData.m_iCampPoint = iCampPoint;
	}
}

void ioCampManager::InfluenceAlarm()
{
	//채팅 세력비 블럭처리
	return;

	if( g_MyInfo.GetUserCampPos() == CAMP_NONE ) return;

	ioComplexStringPrinter kPrinter;
	g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

	if( m_SeasonState == SS_PLAY_DELAY || m_SeasonState == SS_PLAY_PROCEED )
	{
		switch( g_MyInfo.GetUserCampPos() )
		{
		case CAMP_BLUE:
			{
				int iBlueInfluence = GetCampInfluencePoint( CAMP_BLUE );
				int iMentLine = m_BlueAlarmMent.size();
				for(int i = 0;i < iMentLine;i++)
				{
					AlarmMent &kMentList = m_BlueAlarmMent[i];
					if( iBlueInfluence <= kMentList.m_iInfluence )
					{
						int iMentSize = kMentList.m_MentList.size();
						for(int j = 0;j < iMentSize;j++)
						{
							MentData &rkMent = kMentList.m_MentList[j];
							kPrinter.SetTextColor( rkMent.m_dwMentColor );
							if( j == 0 )
								kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), rkMent.m_szMentText );
							else
								kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), rkMent.m_szMentText );
						}
						break;
					}
				}
			}
			break;
		case CAMP_RED:
			{
				int iRedInfluence = GetCampInfluencePoint( CAMP_RED );
				int iMentLine = m_RedAlarmMent.size();
				for(int i = 0;i < iMentLine;i++)
				{
					AlarmMent &kMentList = m_RedAlarmMent[i];
					if( iRedInfluence <= kMentList.m_iInfluence )
					{
						int iMentSize = kMentList.m_MentList.size();
						for(int j = 0;j < iMentSize;j++)
						{
							MentData &rkMent = kMentList.m_MentList[j];
							kPrinter.SetTextColor( rkMent.m_dwMentColor );
							if( j == 0 )
								kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4), rkMent.m_szMentText );
							else
								kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), rkMent.m_szMentText );
						}
						break;
					}
				}
			}
			break;
		}

		ioHashString szBlueCampName;
		ioHashString szRedCampName;

		szBlueCampName.Clear();
		szRedCampName.Clear();

		TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
		if( pTournament )
		{
			szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
			szRedCampName = pTournament->GetCampName( CAMP_RED );
		}

		//
		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),"(" );
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%s ", szBlueCampName.c_str() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%.2f", GetCampInfluencePointF( CAMP_BLUE ) * 100 );
		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale()," vs " );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%s ", szRedCampName.c_str() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%.2f", GetCampInfluencePointF( CAMP_RED ) * 100 );
		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),")" );
	}
	else
	{   // 휴식중 알림 멘트
		switch( g_MyInfo.GetUserCampPos() )
		{
		case CAMP_BLUE:
			{
				int iMentSize = m_BlueRecessMent.size();
				for(int i = 0;i < iMentSize;i++)
				{
					MentData &rkMent = m_BlueRecessMent[i];
					kPrinter.SetTextColor( rkMent.m_dwMentColor );
					if( i == 0 )
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5), rkMent.m_szMentText );
					else
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), rkMent.m_szMentText );
				}			
			}
			break;
		case CAMP_RED:
			{
				int iMentSize = m_RedRecessMent.size();
				for(int i = 0;i < iMentSize;i++)
				{
					MentData &rkMent = m_RedRecessMent[i];
					kPrinter.SetTextColor( rkMent.m_dwMentColor );
					if( i == 0 )
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(6), rkMent.m_szMentText );
					else
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), rkMent.m_szMentText );
				}			
			}
			break;
		}
	}
	g_ChatMgr.SetChatComplexString( STR(7), kPrinter );
}

void ioCampManager::FillCampDataSync( SP2Packet &rkPacket )
{
	bool bFirstSync = false;
	if( m_SeasonState == SS_NONE )
		bFirstSync = true;

	int iSeasonState;
	DWORD dwLastSeasonDate, dwNextSeasonDate;
	rkPacket >> iSeasonState >> dwLastSeasonDate >> dwNextSeasonDate;
	m_SeasonState = (SeasonState)iSeasonState;

	m_SeasonDate.m_iStartYear   = dwLastSeasonDate / 1000000;
	m_SeasonDate.m_iStartMonth  = ( dwLastSeasonDate % 1000000 ) / 10000;
	m_SeasonDate.m_iStartDay    = ( dwLastSeasonDate % 10000 ) / 100;
	m_SeasonDate.m_iStartHour   = dwLastSeasonDate % 100;
	m_SeasonDate.m_iStartWeekDay= DateHelp::GetWeekDay( m_SeasonDate.m_iStartYear, m_SeasonDate.m_iStartMonth, m_SeasonDate.m_iStartDay );

	m_SeasonDate.m_iNextYear   = dwNextSeasonDate / 1000000;
	m_SeasonDate.m_iNextMonth  = ( dwNextSeasonDate % 1000000 ) / 10000;
	m_SeasonDate.m_iNextDay    = ( dwNextSeasonDate % 10000 ) / 100;
	m_SeasonDate.m_iNextHour   = dwNextSeasonDate % 100;
	m_SeasonDate.m_iNextWeekDay= DateHelp::GetWeekDay( m_SeasonDate.m_iNextYear, m_SeasonDate.m_iNextMonth, m_SeasonDate.m_iNextDay );
	//
	{
		rkPacket >> m_SeasonDate.m_wPlayStartHour >> m_SeasonDate.m_wPlayPreceedTime >> m_iNextTodayBattleSec;
		m_iCurTodayBattleSec = m_iNextTodayBattleSec;
	}
	//
	int i;
	int iSize;
	rkPacket >> iSize;
	for(i = 0;i < iSize;i++)
	{
		CampData kCampData;
		rkPacket >> kCampData.m_iCampType >> kCampData.m_iCampPoint >> kCampData.m_iCampTodayPoint >> kCampData.m_iCampBonusPoint
			     >> kCampData.m_iCampEntryUserCount >> kCampData.m_iCampSpecialEntryUserCount;
		ChangeCampData( kCampData );
	}

	if( bFirstSync )
	{
		InfluenceAlarm();
	}
}

bool ioCampManager::IsCloseCampBattle()
{
	if( m_SeasonState != SS_PLAY_PROCEED ) return true;
	if( m_iNextTodayBattleSec == -1 ) return true;
	if( m_iNextTodayBattleSec / 60 < m_iCloseCheckMin ) return true;

	return false;
}

void ioCampManager::ProcessCamp()
{
	if( REALGETTIME() - m_dwProcessTime < 1000 ) return;

	// 최초 한번은 시즌 보상 요청
	if( !m_bSendSeasonReward )
	{
		ioGuildData *pMyGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !pMyGuildData ) // 길드가 없으면 보상 즉시 요청
			m_bSendSeasonReward = true;
		else if( pMyGuildData && pMyGuildData->IsSync() )   // 길드가 있고 동기화 되었으면 요청
			m_bSendSeasonReward = true;

		if( m_bSendSeasonReward )
		{
			SP2Packet kPacket( CTPK_CAMP_SEASON_BONUS );
			TCPNetwork::SendToServer( kPacket );
		}
	}

	if( m_SeasonState == SS_NONE )
	{
		SyncCampData();
	}
	else if( m_SeasonState == SS_PLAY_PROCEED && m_iNextTodayBattleSec != -1 )
	{
		int iGapSec = ( REALGETTIME() - m_dwProcessTime ) / 1000;
		m_iCurTodayBattleSec = max( m_iCurTodayBattleSec - iGapSec, 0 );
		if( ( m_iNextTodayBattleSec / 60 ) != ( m_iCurTodayBattleSec / 60 ) )
		{
			m_iNextTodayBattleSec = m_iCurTodayBattleSec;
			if( m_iNextTodayBattleSec / 60 < m_iCloseCheckMin ) // 60분전부터 1분마다 종료 알림
			{
				if( g_LadderTeamMgr.IsLadderTeam() )
				{
					g_ChatMgr.SetSystemMsg( STR(1), ( m_iNextTodayBattleSec / 60 ) + 1 );
					TodayBattleEndAlarm( m_bTodayBattleEndAlarm );
					m_bTodayBattleEndAlarm = false;
				}
			}
			else if( m_iNextTodayBattleSec / 60 > m_iCloseCheckMin + 30 ) 
			{
				m_bTodayBattleEndAlarm = true;
			}
		}
	}
	m_dwProcessTime = REALGETTIME();
}

void ioCampManager::RenderEntryMent( int iXPos, int iYPos )
{
	int i,j;
	// 타이틀 0,0 Font_18
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	int iListSize = m_EntryMentTitle.m_MentList.size();
	for(j = 0;j < iListSize;j++)
	{
		MentData &kMent = m_EntryMentTitle.m_MentList[j];
		kPrinter.SetTextColor( kMent.m_dwMentColor );
		kPrinter.AddTextPiece( FONT_SIZE_17, kMent.m_szMentText );					
	}
	kPrinter.PrintFullText( iXPos, iYPos, TAT_LEFT );
	kPrinter.ClearList();
	// 내용 0, 24 Font_13 Line Gap 18
	int iMentLine = m_EntryMentContent.size();
	for(i = 0;i < iMentLine;i++)
	{
		EntryMentList &kMentList = m_EntryMentContent[i];
		int iListSize = kMentList.m_MentList.size();
		for(int j = 0;j < iListSize;j++)
		{
			MentData &kMent = kMentList.m_MentList[j];
			kPrinter.SetTextColor( kMent.m_dwMentColor );
			kPrinter.AddTextPiece( FONT_SIZE_12, kMent.m_szMentText );					
		}
		kPrinter.PrintFullText( iXPos + kMentList.m_iOffsetX, iYPos + kMentList.m_iOffsetY + 24 + ( i * 18 ), TAT_LEFT );
		kPrinter.ClearList();
	}
}

void ioCampManager::TodayBattleEndAlarm( bool bAlarm )
{
	if( !bAlarm ) return;
	if( m_SeasonState != SS_PLAY_PROCEED || m_iNextTodayBattleSec == -1 ) return;
	
	if( m_iNextTodayBattleSec / 60 < m_iCloseCheckMin ) // 60분전부터 1분마다 종료 알림
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노랑
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(2) );

		kPrinter[2].SetTextStyle( TS_NORMAL );
		kPrinter[2].SetBkColor( 0, 0, 0 );	
		kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3), GetPlayPreceedMent().c_str() );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	}
}

const ioHashString &ioCampManager::GetPlayPreceedMent()
{
	static ioHashString szReturn = STR(1);

	int iStartHour = m_SeasonDate.m_wPlayStartHour;
	int iEndHour   = m_SeasonDate.m_wPlayStartHour + ( m_SeasonDate.m_wPlayPreceedTime - 1 );         //1시간 미리 종료한다.
 
	//6시  ~ 12 ~ 18 ~  24
	char szTick[4][MAX_PATH];
	SafeSprintf( szTick[0], sizeof( szTick[0] ), STR(2) );
	SafeSprintf( szTick[1], sizeof( szTick[1] ), STR(3) );
	SafeSprintf( szTick[2], sizeof( szTick[2] ), STR(4) );
	SafeSprintf( szTick[3], sizeof( szTick[3] ), STR(5) );

	int  iStartTick = 3;
	if( COMPARE( iStartHour, 6, 12 ) ) iStartTick = 0;
	else if( COMPARE( iStartHour, 12, 18 ) ) iStartTick = 1;
	else if( COMPARE( iStartHour, 18, 24 + 1 ) ) iStartTick = 2;
	
	char szStart[MAX_PATH] = "";
	if( iStartHour <= 12 )
		SafeSprintf( szStart, sizeof(szStart), STR(6), szTick[iStartTick], iStartHour );
	else
		SafeSprintf( szStart, sizeof(szStart), STR(7), szTick[iStartTick], iStartHour - 12 );
	
	iEndHour = iEndHour%24;
	int  iEndTick = 3;
	if( COMPARE( iEndHour, 6, 12 ) ) iEndTick = 0;
	else if( COMPARE( iEndHour, 12, 18 ) ) iEndTick = 1;
	else if( COMPARE( iEndHour, 18, 24 ) || iEndHour == 0 ) iEndTick = 2;

	char szEnd[MAX_PATH] = "";
	if( iEndHour == 0 )
		SafeSprintf( szEnd, sizeof(szEnd), STR(8), szTick[iEndTick] );
	else if( iEndHour <= 12 )
		SafeSprintf( szEnd, sizeof(szEnd), STR(9), szTick[iEndTick], iEndHour );
	else
		SafeSprintf( szEnd, sizeof(szEnd), STR(10), szTick[iEndTick], iEndHour - 12 );

	char szAll[MAX_PATH];
	sprintf( szAll, "%s ~ %s", szStart, szEnd );
	szReturn = szAll;

	return szReturn;
}

HeroRankData ioCampManager::GetHeroData( int iRank )
{
	HeroRankData kReturn;
	kReturn.m_iRank = iRank;
	for(int i = 0;i < (int)m_HeroRankData.size();i++)
	{
		if( m_HeroRankData[i].m_iRank == kReturn.m_iRank )
		{			
			kReturn = m_HeroRankData[i];
			break;
		}
	}
	return kReturn;
}

void ioCampManager::InsertHeroTop100( HeroRankData &rkData )
{
	int i = 0;
	for(i = 0;i < (int)m_HeroRankData.size();i++)
	{
		if( m_HeroRankData[i].m_iRank == rkData.m_iRank )
		{
			m_HeroRankData[i] = rkData;
			return;
		}
	}
	m_HeroRankData.push_back( rkData );
}

void ioCampManager::ApplyHeroTop100( SP2Packet &rkPacket )
{
	int iTotalList, iCurList;
	rkPacket >> iTotalList >> iCurList;			
	for(int i = 0;i < iCurList;i++)
	{	
		HeroRankData kData;
		rkPacket >> kData.m_iRank >> kData.m_iGradeLevel >> kData.m_szNick >> kData.m_iHeroTitle 
				 >> kData.m_iHeroWin >> kData.m_iHeroLose >> kData.m_iCampPos >> kData.m_bLogIn;
		InsertHeroTop100( kData );
	}    
}

void ioCampManager::SendHeroTop100( int iCurPage, int iSyncSize )
{
	SP2Packet kPacket( CTPK_HERO_TOP100_DATA );
	kPacket << iCurPage << iSyncSize;
	TCPNetwork::SendToServer( kPacket );
}

void ioCampManager::SetHeroMatchInfo( int iHeroRank, int iHeroTitle, int iGradeLevel, int iCampType, int iTeamType, ioHashString &rkNick )
{
	if( rkNick.IsEmpty() ) return;

	HeroMatchHistory kHeroMatch;
	kHeroMatch.m_iHeroRank	  = iHeroRank;
	kHeroMatch.m_iHeroTitle	  = iHeroTitle;
	kHeroMatch.m_iGradeLevel  = iGradeLevel;
	kHeroMatch.m_iCampType	  = iCampType;
	kHeroMatch.m_szNickName	  = rkNick;
	kHeroMatch.m_iBlueScore	  = 0;
	kHeroMatch.m_iRedScore	  = 0;
	kHeroMatch.m_eWinTeam     = WTT_NONE;
	kHeroMatch.m_dwResultTime = FRAMEGETTIME();
	kHeroMatch.m_bResultCheck = false;
	kHeroMatch.m_bLeave       = false;
	kHeroMatch.m_iTeamType	  = iTeamType;

	//
	m_HeroMatchHistory.insert( m_HeroMatchHistory.begin(), kHeroMatch );
}

void ioCampManager::SetHeroMatchResult( WinTeamType eWinTeam, int iBlueScore, int iRedScore )
{
	if( m_HeroMatchHistory.empty() ) return;

	HeroMatchHistory &rkHeroMatch = m_HeroMatchHistory[0];
	rkHeroMatch.m_iBlueScore      = iBlueScore;
	rkHeroMatch.m_iRedScore	      = iRedScore;
	rkHeroMatch.m_eWinTeam        = eWinTeam;
	rkHeroMatch.m_dwResultTime    = FRAMEGETTIME();
	rkHeroMatch.m_bResultCheck    = true;	
}

void ioCampManager::SetHeroMatchLeave()
{
	if( m_HeroMatchHistory.empty() ) return;

	HeroMatchHistory &rkHeroMatch = m_HeroMatchHistory[0];
	rkHeroMatch.m_bLeave = true;
}

bool ioCampManager::GetHeroMatchHistory( int iArray, HeroMatchHistory &rHeroHistory )
{
	if( !COMPARE( iArray, 0, GetMaxHeroMatch() ) )
		return false;

	rHeroHistory = m_HeroMatchHistory[iArray];
	return true;
}