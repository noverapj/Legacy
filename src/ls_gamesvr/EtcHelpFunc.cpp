

#include "stdafx.h"
#include "../ioINILoader/ioINILoader.h"
#include "NodeInfo/ServerNodeManager.h"
#include "NodeInfo/NodeHelpStructDefine.h"
#include "DataBase/DBClient.h"
#include "EtcHelpFunc.h"
#include <WinINet.h>
#include <strsafe.h>

extern CLog CriticalLOG;

namespace Help
{

static int s_iFirstBonus = 10000;


static float s_fPesoExpRate = 100.0f;
static float s_fSellReturnRate = 1.0f;

static int s_iTutoralBonusStep = 9;
static int s_iTutoralBonusPeso = 5000;
static int s_iTutoralBonusEventPeso = 100000;

static int s_iGradeUpBonusPeso = 3000;
static int s_iLevelUpBonusPeso = 3000;

static int s_iCreateGuildPeso = 25000;
static int s_iChangeGuildMarkPeso = 25000;

static int s_iGuildPointCorrection = 5;
static float s_fPCRoomBonusExp = 0.05f;
static float s_fPCRoomBonusPeso = 0.05f;
static float s_fModeConsecutivelyBonus = 0.05f;
static float s_fModeConsecutivelyMaxBonus = 0.50f;
static int s_iFriendBonusBeforeMin = 30;
static float s_fFriendBonus = 0.05f;
static float s_fPcRoomFriendBonus = 0.10f;
static float s_fMaxFriendBonus = 0.20f;
static float s_fPcRoomMaxFriendBonus = 0.40f;
static bool  s_bRoomFriendCnt = true;


static float s_fLadderBonus = 0.30f;
static int   s_iLadderTeamLimitGrade = 0;
static int   s_iLimitCampPoint = 0;
static float s_fCampBonusCorrectionA = 0.5f;
static float s_fCampBonusCorrectionB = 0.55f;
static float s_fCampBonusDefaultA    = 20000;    
static float s_fCampBonusDefaultB    = 2000;
static float s_iCampBonusMinEntry    = 200;
static float s_iCampBonusMaxEntry    = 2000;

static float s_fContributePerRate;
static float s_fKillDeathLevelRate;

static int s_iFirstVictories;
static float s_fFirstVictoriesRate;
static float s_fVictoriesRate;
static float s_fMaxVictoriesRate;

static ioHashString s_szGuildMarkBlockURL;

static vSoldierBonus s_SoldierBonus;

static bool s_bPCRoomBonus = true;

static int s_iFriendDefaultSlotSize;

static int s_iRefillCoinCount = 0;
static int s_iRefillCoinMax = 0;
static int s_iRefillCoinSec = 0;

static bool s_bNagleAlgorithm		= true;
static bool s_bPlazaNagleAlgorithm  = false;
static bool s_bCharChangeToUDP	    = false;
static bool s_bOnlyServerRelay      = false;
static bool s_bWholeChatOn          = true;
static bool s_bServerRelayToTCP     = true;

static int s_iHeroTop100SyncHour = 9;
static int s_iHeroUserDataSyncHour = 9;

static int s_iAwardEtcItemBonus = 1000049;
static int s_iAwardEtcItemBonusMent       = 0;
static int s_iAwardEtcItemBonusAbusePoint = 1;

static int s_iDefaultBestFriendCount	  = 5;
static int s_iBestFriendDismissDelayHour  = 120;

static int s_iCharRentalMinute = 30;
static int s_iBestFriendCharRentalDelayHour = 24;
static int s_iCharRentalCount = 3;
static int s_iCharRentalGrade = 20;
static int s_iMaxCharRentSet  = 2;

static DWORD s_dwQuestAbuseSecond		= 50;
static bool  s_bQuestAbuseDeveloperPass = false;
static bool  s_bQuestAbuseLogOut        = true;

static int s_iMaxCloverCount = 5;			// 최대 5개의 클로버 가질 수 있다.
static int s_iCloverDefaultCount = 5;		// 초기 클로버 갯수 설정.
static int s_iGiveCloverTimeMinute = 30;	// 클로버 선물 가능 걸리는 시간(분)
static int s_iCloverChargeCount	= 1;		// 충전시 충전 갯수.
static int s_iCloverChargeTime = 30 * 60;	// sec
static int s_iCloverSendCount = 1;			// 한번에 클로버를 보낼 수 있는 갯수.
static int s_iCloverReceiveCount = 1;		// 한번에 클로버 받을 수 있는 갯수.
static int s_iCloverSendTime = 1440;		// 24시간. ( 24 * 60 )
static int s_iCloverReceiveTime = 1380;		// 23시간. ( 23 * 60 )
static int s_iCloverRefill_Hour = 4;		// 04시
static int s_iCloverRefill_Min = 0;			// 00분
static bool s_bCloverRefill_Test = false;	// 리필시간 테스트
static int s_iCloverRefill_Cycle = 5;		// 리필싸이클. 5분
static bool s_bSpawnNpc = true;


// 레이드 티켓
static int s_iRefillRaidTicketMax = 0;
static int s_iRefillRaidTicketCount = 0;
static int s_iRefillRaidTicketTime = 0;
static int s_iDefaultRaidTicketCount = 0;

void InitHelpConstant()
{
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Load INI config.ini" );

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/config.ini" );

	kLoader.SetTitle( "first_connect" );
	s_iFirstBonus = kLoader.LoadInt( "peso", 10000 );

	kLoader.SetTitle( "levelup" );
	s_iLevelUpBonusPeso= kLoader.LoadInt( "levelup_bonus_peso", 3000 );

	kLoader.SetTitle( "gradeup" );
	s_iGradeUpBonusPeso= kLoader.LoadInt( "gradeup_bonus_peso", 3000 );

	s_fPesoExpRate	  = kLoader.LoadFloat( "exp_peso", "peso_rate", 1.0f );
	s_fSellReturnRate = kLoader.LoadFloat( "shop", "sell_return_rate", 1.0f );

	kLoader.SetTitle( "tutorial_bonus" );
	s_iTutoralBonusStep = kLoader.LoadInt( "BonusStep", 5 );
	s_iTutoralBonusPeso = kLoader.LoadInt( "BonusPeso", 5000 );
	s_iTutoralBonusEventPeso = kLoader.LoadInt( "BonusEventPeso", 100000 );

	kLoader.SetTitle( "guild" );
	s_iCreateGuildPeso = kLoader.LoadInt( "CreatePeso", 25000 );
	s_iChangeGuildMarkPeso = kLoader.LoadInt( "MarkChangePeso", 25000 );

	s_SoldierBonus.clear();
	kLoader.SetTitle( "CharBonus" );
	int iMaxSize = kLoader.LoadInt( "MaxSize", 0 );
	for(int i = 0;i < iMaxSize;i++)
	{
		SoldierBonus kBonus;
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey,"PossessionCnt_%d", i + 1 );
		kBonus.iPossessionCnt = kLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey,"PossessionBonus_%d", i + 1 );
		kBonus.fBonusPer = kLoader.LoadFloat( szKey, 0.0f );
		s_SoldierBonus.push_back( kBonus );
	}

	kLoader.SetTitle( "result_correction" );
	s_fPCRoomBonusExp = kLoader.LoadFloat( "PCRoomBonusExp", 0.05f );
	s_fPCRoomBonusPeso = kLoader.LoadFloat( "PCRoomBonusPeso", 0.05f );
	s_iGuildPointCorrection = kLoader.LoadInt( "GuildPointCorrection", 5 );
	s_fModeConsecutivelyBonus = kLoader.LoadFloat( "ModeConsecutivelyBonus", 0.05f );
	s_fModeConsecutivelyMaxBonus = kLoader.LoadFloat( "ModeConsecutivelyMaxBonus", 0.50f );

	kLoader.SetTitle( "guild_manage" );
	char szBuf[MAX_PATH] = "";
	kLoader.LoadString( "mark_block_url", "", szBuf, MAX_PATH );
	s_szGuildMarkBlockURL = szBuf;

	kLoader.SetTitle( "NextTeam" );
	s_fContributePerRate = kLoader.LoadFloat( "contirbute_per_rate", 1.0f );
	s_fKillDeathLevelRate = kLoader.LoadFloat( "kill_death_lever_rate", 1.0f );

	kLoader.SetTitle( "pcroom" );
	s_bPCRoomBonus = kLoader.LoadBool( "bonus", true );

	kLoader.SetTitle( "friend_bonus" );
	s_iFriendBonusBeforeMin = kLoader.LoadInt( "before_min", 30 );
	s_fFriendBonus          = kLoader.LoadFloat( "bonus", 0.05f );	
	s_fMaxFriendBonus       = kLoader.LoadFloat( "max_bonus", 0.20f );
	s_fPcRoomFriendBonus    = kLoader.LoadFloat( "pcroom_bonus", 0.05f );
	s_fPcRoomMaxFriendBonus = kLoader.LoadFloat( "pcroom_max_bonus", 0.20f );
	s_bRoomFriendCnt        = kLoader.LoadBool( "is_room_friend_cnt" , true );

	kLoader.SetTitle( "victories_bonus" );
	s_iFirstVictories = kLoader.LoadInt( "first_victories", 2 );
	s_fFirstVictoriesRate = kLoader.LoadFloat( "first_bonus_rate", 0.2f );
	s_fVictoriesRate = kLoader.LoadFloat( "bonus_rate", 0.1f );
	s_fMaxVictoriesRate = kLoader.LoadFloat( "max_bonus_rate", 1.0f );

	kLoader.SetTitle( "ladder" );
	s_fLadderBonus			= kLoader.LoadFloat( "bonus", 0.30f );
	s_iLadderTeamLimitGrade = kLoader.LoadInt( "LadderTeamLimitGrade", 0 );
	s_iLimitCampPoint       = kLoader.LoadInt( "LimitCampPoint", 0 );
	s_fCampBonusCorrectionA = kLoader.LoadFloat( "CampBonusCorrectionA", 0.5f );
	s_fCampBonusCorrectionB = kLoader.LoadFloat( "CampBonusCorrectionB", 0.55f );
	s_fCampBonusDefaultA    = kLoader.LoadFloat( "CampBonusDefaultA", 20000 );
	s_fCampBonusDefaultB    = kLoader.LoadFloat( "CampBonusDefaultB", 2000 );
	s_iCampBonusMinEntry    = kLoader.LoadInt( "CampBonusMinEntry", 200 );
	s_iCampBonusMaxEntry    = kLoader.LoadInt( "CampBonusMaxEntry", 2000 );


	kLoader.SetTitle( "friend_manager" );
	s_iFriendDefaultSlotSize = kLoader.LoadInt( "default_slot_size", 100 );

	kLoader.SetTitle( "refill_coin" );
	s_iRefillCoinMax   = kLoader.LoadInt( "refill_coin_max", 0 );
	s_iRefillCoinCount = kLoader.LoadInt( "refill_coin_cnt", 0 );
	s_iRefillCoinSec   = kLoader.LoadInt( "refill_coin_sec", 0 );

	kLoader.SetTitle( "hero_rank" );
	s_iHeroTop100SyncHour   = kLoader.LoadInt( "top100_sync_hour", 9 );
	s_iHeroUserDataSyncHour = kLoader.LoadInt( "user_data_sync_hour", 9 );

	kLoader.SetTitle( "award_etcitem_bonus" );
	s_iAwardEtcItemBonus = kLoader.LoadInt( "etcitem_type", 1000049 );
	s_iAwardEtcItemBonusMent = kLoader.LoadInt( "present_ment", 0 );
	s_iAwardEtcItemBonusAbusePoint = kLoader.LoadInt( "abuse_point", 1 );

	kLoader.SetTitle( "best_friend" );
	s_iDefaultBestFriendCount = kLoader.LoadInt( "default_best_friend_count", 5 );
	s_iBestFriendDismissDelayHour = kLoader.LoadInt( "best_friend_dismiss_delay_hour", 120 );

	kLoader.SetTitle( "char_rental" );
	s_iCharRentalMinute = kLoader.LoadInt( "char_rental_minute", 30 );
	s_iBestFriendCharRentalDelayHour = kLoader.LoadInt( "best_friend_char_rental_delay_hour", 24 );
	s_iCharRentalCount = kLoader.LoadInt( "char_rental_count", 3 );
	s_iCharRentalGrade = kLoader.LoadInt( "char_rental_grade", 20 );
	s_iMaxCharRentSet  = kLoader.LoadInt( "max_char_rent_set", 2 );

	kLoader.SetTitle( "quest_abuse" );
	s_dwQuestAbuseSecond	   = kLoader.LoadInt( "abuse_second", 50 );
	s_bQuestAbuseDeveloperPass = kLoader.LoadBool( "abuse_developer_pass", false );
	s_bQuestAbuseLogOut        = kLoader.LoadBool( "abuse_logout", true );

	kLoader.SetTitle( "server_relay" );
	s_bServerRelayToTCP = kLoader.LoadBool( "isTcp", false );

	kLoader.SetTitle( "FRIEND_CLOVER" );
	s_iMaxCloverCount		= kLoader.LoadInt( "MaxCloverCount", 9 );
	s_iCloverDefaultCount	= kLoader.LoadInt( "CloverDefaultCount", 5 );
	s_iGiveCloverTimeMinute = kLoader.LoadInt( "GiveCloverTimeMinute", 30 );
	s_iCloverChargeCount	= kLoader.LoadInt( "CloverCharge_Count", 1 );
	s_iCloverChargeTime		= kLoader.LoadInt( "CloverCharge_Time", 30 * 60 );
	s_iCloverSendCount		= kLoader.LoadInt( "CloverSendCount", 1 );
	s_iCloverReceiveCount	= kLoader.LoadInt( "CloverReceiveCount", 1 );
	s_iCloverSendTime		= kLoader.LoadInt( "CloverSendTime", 24 * 60 );
	s_iCloverReceiveTime	= kLoader.LoadInt( "CloverReceiveTime", 23 * 60 );
	s_iCloverRefill_Hour	= kLoader.LoadInt( "CloverRefill_Hour", 4 );		// 04시
	s_iCloverRefill_Min		= kLoader.LoadInt( "CloverRefill_Min", 0 );			// 00분
	s_bCloverRefill_Test	= kLoader.LoadBool( "CloverRefill_Test", false );	// 리필시간 테스트
	s_iCloverRefill_Cycle	= kLoader.LoadInt( "CloverRefill_Cycle", 60 );		// 리필싸이클.

	kLoader.SetTitle( "refill_raid" );
	s_iRefillRaidTicketMax = kLoader.LoadInt( "ticket_max", 99 );
	s_iRefillRaidTicketCount = kLoader.LoadInt( "ticket_cnt", 1 );
	s_iRefillRaidTicketTime = kLoader.LoadInt( "ticket_time", 5 );
	s_iDefaultRaidTicketCount = kLoader.LoadInt( "default_cnt", 1 );

}

int GetFirstConnectBonusPeso()
{
	return s_iFirstBonus;
}

int GetGradeUPBonus()
{
	return s_iGradeUpBonusPeso;
}

int GetLevelUPBonus()
{
	return s_iLevelUpBonusPeso;
}

float GetPesoExpRate()
{
	return s_fPesoExpRate;
}

float GetSellReturnRate()
{
	return s_fSellReturnRate;
}

int GetCreateGuildPeso()
{
	return s_iCreateGuildPeso;
}

int GetChangeGuildMarkPeso()
{
	return s_iChangeGuildMarkPeso;
}

int GetGuildPointCorrection()
{
	return s_iGuildPointCorrection;
}

float GetPCRoomBonusExp()
{
	return s_fPCRoomBonusExp;
}

float GetPCRoomBonusPeso()
{
	return s_fPCRoomBonusPeso;
}

float GetModeConsecutivelyBonus()
{
	return s_fModeConsecutivelyBonus;
}

float GetModeConsecutivelyMaxBonus()
{
	return s_fModeConsecutivelyMaxBonus;
}

int GetHeroTop100SyncHour()
{
	return s_iHeroTop100SyncHour;
}

int GetHeroUserDataSyncHour()
{
	return s_iHeroUserDataSyncHour;
}

DWORD ConvertCTimeToDayCount( const CTime &rkTime )
{
	DWORD dwDay = rkTime.GetYear() * 10000 +
				  rkTime.GetMonth() * 100 +
				  rkTime.GetDay();

	return dwDay;
}

DWORD ConvertCTimeToDate( const CTime &rkTime )
{
	DWORD dwDate = ( rkTime.GetYear() % 100 ) * 100000000 +
		           rkTime.GetMonth() * 1000000 + 
				   rkTime.GetDay() * 10000 +
				   rkTime.GetHour() * 100 +
				   rkTime.GetMinute();
	return dwDate;
}

DWORD ConvertCTimeToYearMonthDay( const CTime &rkTime )
{
	DWORD dwDate =   rkTime.GetYear()  * 10000 +
		             rkTime.GetMonth() * 100   + 
		             rkTime.GetDay(); 
	return dwDate;
}

DWORD ConvertCTimeToHourMinute( const CTime &rkTime )
{
	DWORD dwDate = rkTime.GetHour() * 100 +
		           rkTime.GetMinute();
	return dwDate;
}

DWORD CheckDiffTimeToTime( DWORD dwRemainMinute, CTime &rkCurTime, CTime &rkBeforeTime )
{
	CTimeSpan cGapTime = rkCurTime - rkBeforeTime;
	int iMinute = cGapTime.GetTotalMinutes();
	int iRemainTime = dwRemainMinute;
	return max( 0, iRemainTime - iMinute );
}

DWORD ConvertYYMMDDHHMMToDate( WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute )
{
	// 년도에서 2010년을 빼고 4바이트 날짜를 리턴
	enum { DEFAULT_YEAR = 2010,	DATE_YEAR_VALUE = 100000000, DATE_MONTH_VALUE= 1000000, DATE_DAY_VALUE =  10000, DATE_HOUR_VALUE = 100, };

	DWORD dwReturnDate = ((wYear - DEFAULT_YEAR) * DATE_YEAR_VALUE) +
						  (wMonth * DATE_MONTH_VALUE) + (wDay * DATE_DAY_VALUE) +
						  (wHour * DATE_HOUR_VALUE) + wMinute;
	return dwReturnDate;
}

DWORD ConvertYYMMDDToDate( WORD wYear, WORD wMonth, WORD wDay )
{
	enum { DEFAULT_YEAR = 2000,	DATE_YEAR_VALUE = 10000, DATE_MONTH_VALUE= 100  };

	DWORD dwReturnDate = ((wYear - DEFAULT_YEAR) * DATE_YEAR_VALUE) + (wMonth * DATE_MONTH_VALUE) + wDay;
	return dwReturnDate;
}

DWORD ConvertYYMMToDate( WORD wYear, WORD wMonth )
{
	enum { DEFAULT_YEAR = 2000,	DATE_YEAR_VALUE = 100,  };
	DWORD dwReturnDate = ((wYear - DEFAULT_YEAR) * DATE_YEAR_VALUE) + wMonth;
	return dwReturnDate;
}

CTime ConvertDateToCTime( DWORD dwDate, bool bTenBase /*= true*/ )
{
	enum { DEFAULT_YEAR = 2000,	DEFAULT_YEAR_TEN_BASE = 2010, DATE_YEAR_VALUE = 100000000, DATE_MONTH_VALUE= 1000000, DATE_DAY_VALUE =  10000, DATE_HOUR_VALUE = 100, };

	SYSTEMTIME returnSt={0,0,0,0,0,0,0,0};

	if( bTenBase == true )
		returnSt.wYear = DEFAULT_YEAR_TEN_BASE + ( dwDate / DATE_YEAR_VALUE );
	else
		returnSt.wYear = DEFAULT_YEAR + ( dwDate / DATE_YEAR_VALUE );

	returnSt.wMonth = ( dwDate % DATE_YEAR_VALUE ) / DATE_MONTH_VALUE;
	returnSt.wDay = ( dwDate % DATE_MONTH_VALUE ) / DATE_DAY_VALUE;
	returnSt.wHour = ( dwDate % DATE_DAY_VALUE ) / DATE_HOUR_VALUE;
	returnSt.wMinute = ( dwDate % DATE_HOUR_VALUE );
	
	// CTime 보안 코드 추가
	CTime kReturnTime( GetSafeValueForCTimeConstructor( returnSt.wYear, returnSt.wMonth, returnSt.wDay, returnSt.wHour, returnSt.wMinute, returnSt.wSecond ) );

	return kReturnTime;
}

CTime ConvertNumberToCTime( const int iDateTime )
{
	enum { DEFAULT_YEAR = 2010,	DATE_YEAR_VALUE = 100000000, DATE_MONTH_VALUE= 1000000, DATE_DAY_VALUE =  10000, DATE_HOUR_VALUE = 100, };

	int year	= iDateTime / DATE_YEAR_VALUE;
	int month	= ( iDateTime % DATE_YEAR_VALUE ) / DATE_MONTH_VALUE;
	int day		= ( iDateTime % DATE_MONTH_VALUE ) / DATE_DAY_VALUE;
	int hour	= ( iDateTime % DATE_DAY_VALUE ) / DATE_HOUR_VALUE;
	int min		= iDateTime % DATE_HOUR_VALUE;

	CTime time( GetSafeValueForCTimeConstructor( year + DEFAULT_YEAR, month, day, hour, min, 0 ) );
	return time;
}

int GetEquipSlot( int iItemCode )
{
	if( COMPARE( iItemCode, 1, 100000 ) )
		return EQUIP_WEAPON;
	if( COMPARE( iItemCode, 100001, 200000 ) )
		return EQUIP_ARMOR;
	if( COMPARE( iItemCode, 200001, 300000 ) )
		return EQUIP_HELM;
	if( COMPARE( iItemCode, 300001, 400000 ) )
		return EQUIP_CLOAK;
	if( COMPARE( iItemCode, 400001, 500000 ) )
		return EQUIP_OBJECT;
	if( COMPARE( iItemCode, 500001, 600000 ) )
		return EQUIP_WEAR;

	return EQUIP_UNKNOWN;
}

int GetTutorialBonusStep()
{
	return s_iTutoralBonusStep;
}

int GetTutorialBonusPeso()
{
	return s_iTutoralBonusPeso;
}

int GetTutorialBonusEventPeso()
{
	return s_iTutoralBonusEventPeso;
}

int GetFirstVictories()
{
	return s_iFirstVictories;
}

float GetFirstVictoriesRate()
{
	return s_fFirstVictoriesRate;
}

float GetVictoriesRate()
{
	return s_fVictoriesRate;
}

float GetMaxVictoriesRate()
{
	return s_fMaxVictoriesRate;
}

SYSTEMTIME GetSafeValueForCTimeConstructor( SHORT iYear, SHORT iMonth,  SHORT iDay, SHORT iHour,  SHORT iMinute,  SHORT iSecond )
{
	SYSTEMTIME returnSt={0,0,0,0,0,0,0,0};

	if( !COMPARE(iYear, 1971, 3001) )
		returnSt.wYear = 1971;
	else
		returnSt.wYear = iYear;

	if( !COMPARE(iMonth, 1, 13) )
		returnSt.wMonth = 1;
	else
		returnSt.wMonth = iMonth;

	if( !COMPARE(iDay, 1, 32) )
		returnSt.wDay = 1;
	else
		returnSt.wDay = iDay;

	if( !COMPARE(iHour, 0, 25) )
		returnSt.wHour = 0;
	else
		returnSt.wHour = iHour;

	if( !COMPARE(iMinute, 0, 61) )
		returnSt.wMinute = 0;
	else
		returnSt.wMinute = iMinute;

	if( !COMPARE(iSecond, 0, 61) )
		returnSt.wSecond = 0;
	else
		returnSt.wSecond = iSecond;

	return returnSt;
}

void GetSafeTextWriteDB( OUT std::string &rResultString , const ioHashString &rSourceString )
{
	char ch = NULL;
	bool bFirstLeadByte = false;
	for (int i = 0; i < rSourceString.Length() ; i++)
	{
		ch = rSourceString.At(i);
		if( IsDBCSLeadByte( ch ) )
		{
			if( !bFirstLeadByte )
				bFirstLeadByte = true;
			else
				bFirstLeadByte = false;
		}
		else
			bFirstLeadByte = false;

		if( !bFirstLeadByte )
		{
			if( ch == '\'' )
				rResultString += "''";
			else if( ch == '<' )
				rResultString += "&lt;";
			else if( ch == '>' )
				rResultString += "&gt;";
			else if( ch == '\n' )
				rResultString += "<br>";
			else
				rResultString += ch;
		}
		else
			rResultString += ch;
	}
}

ioHashString GetTeamTypeString( TeamType eTeamType )
{
	if( eTeamType == TEAM_BLUE )
		return ioHashString("Blue team");
	else if( eTeamType == TEAM_RED )
		return ioHashString("Red team");
	else if( eTeamType == TEAM_NONE )
		return ioHashString("NONE");
	else 
		return ioHashString("Each team");

	return ioHashString();
}

int GetRefillCoinCount()
{
	return s_iRefillCoinCount;
}

int GetRefillCoinMax()
{
	return s_iRefillCoinMax;
}

int GetRefillCoinSec()
{
	return s_iRefillCoinSec;
}

void GetGUID(OUT char *szGUID, IN int iSize)
{
	char szLongGUID[MAX_PATH]="";

	GUID guid;
	CoCreateGuid(&guid);
	StringCbPrintf(szLongGUID,sizeof(szLongGUID), "%04X%04X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		HIWORD(guid.Data1), LOWORD(guid.Data1), guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	StringCbCopyN(szGUID, iSize, szLongGUID, iSize-1);
}

DWORD GetStringIPToDWORDIP( const char *szIP )
{
	int  count       = 0;
	int  cut_ip		 = 0;
	char szCut_ip[4][4];
	memset( szCut_ip, 0, sizeof( szCut_ip ) );
	int  len	     = strlen( szIP );
	for(int i = 0;i < len;i++)
	{
		if( szIP[i] == '.')
		{
			count = 0;
			cut_ip++;
		}
		else
			szCut_ip[cut_ip][count++] = szIP[i];
	}
	return (DWORD)(atoi(szCut_ip[0])<<24) | (DWORD)(atoi(szCut_ip[1])<<16) | (DWORD)(atoi(szCut_ip[2])<<8) | atoi(szCut_ip[3]);	
}

bool IsSameZoneIP( DWORD dwIP_A, DWORD dwIP_B )
{
	//동일 IP이면 제외한다. 즉 인접IP만 판단
	if( dwIP_A == dwIP_B ) return false;

	if( COMPARE( dwIP_A, dwIP_B - 20, dwIP_B + 21 ) )
		return true;
	return false;
}

bool IsPCRoomBonus()
{
	return s_bPCRoomBonus;
}

float GetSoldierPossessionBonus( int iPossessionCnt )
{
	int iSize = s_SoldierBonus.size();
	for(int i = 0;i < iSize;i++)
	{
		SoldierBonus &kBonus = s_SoldierBonus[i];
		if( iPossessionCnt >= kBonus.iPossessionCnt )
			return kBonus.fBonusPer;
	}
	return 0.0f;
}

ioHashString GetGuildMarkBlockURL()
{
	return s_szGuildMarkBlockURL;
}

bool CallURL( const char *szCallURL )              //URL만 호출한다.
{
	HINTERNET hSession = ::InternetOpen( "CallURL", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if( !hSession ) return false;

	HINTERNET hOpenUrl = ::InternetOpenUrl( hSession, szCallURL, NULL, -1, INTERNET_FLAG_DONT_CACHE, 0 );
	if( hOpenUrl == NULL )
	{
		InternetCloseHandle( hSession );
		return false;
	}

	InternetCloseHandle( hOpenUrl);
	InternetCloseHandle( hSession );
	return true;
}

float GetContributePerRate()
{
	return s_fContributePerRate;
}

float GetKillDeathLevelRate()
{
	return s_fKillDeathLevelRate;
}

int GetFriendBonusBeforeDay()
{	
	return ( s_iFriendBonusBeforeMin / 60 ) / 24;
}

int GetFriendBonusBeforeHour()
{
	return ( s_iFriendBonusBeforeMin / 60 ) % 24;
}

int GetFriendBonusBeforeMin()
{
	return s_iFriendBonusBeforeMin % 60;
}

float GetFriendBonus()
{
	return s_fFriendBonus;
}

float GetMaxFriendBonus()
{
	return s_fMaxFriendBonus;
}

float GetPcRoomFriendBonus()
{
	return s_fPcRoomFriendBonus;
}

float GetPcRoomMaxFriendBonus()
{
	return s_fPcRoomMaxFriendBonus;
}

bool IsRoomFriendCnt()
{
	return s_bRoomFriendCnt;
}

float GetLadderBonus()
{
	return s_fLadderBonus;
}

int GetLimitCampPoint()
{
	return s_iLimitCampPoint;
}

int GetLadderTeamLimitGrade()
{ 
	return s_iLadderTeamLimitGrade; 
}

float GetCampBonusCorrectionA()
{ 
	return s_fCampBonusCorrectionA; 
}

float GetCampBonusCorrectionB()
{ 
	return s_fCampBonusCorrectionB; 
}

float GetCampBonusDefaultA()
{ 
	return s_fCampBonusDefaultA; 
}

float GetCampBonusDefaultB()
{ 
	return s_fCampBonusDefaultB; 
}

int   GetCampBonusMinEntry()
{ 
	return s_iCampBonusMinEntry;
}

int   GetCampBonusMaxEntry()
{ 
	return s_iCampBonusMaxEntry;
}

int GetFriendDefaultSlotSize()
{
	return s_iFriendDefaultSlotSize;
}

bool IsStringCheck( char *szFullText, char *szFindText, char cSection )
{
	int  iDstCount = 0;
	char szDst[MAX_PATH] = "";
	int iMaxLen = strlen( szFullText );
	for(int i = 0;i < iMaxLen;i++)
	{
		if( szFullText[i] == cSection )
		{
			if( strcmp( szDst, szFindText ) == 0 )
				return true;
			memset( szDst, 0, sizeof( szDst ) );
			iDstCount = 0;
		}
		else
		{
			szDst[iDstCount++] = szFullText[i];
		}
	}
	// cSection이 마지막에는 포함되지 않으므로 마지막은 여기서 체크
	if( strcmp( szDst, szFindText ) == 0 )
		return true;
	return false;
}

bool IsStringCheck( char *szFullText, int iFindNumber, char cSection )
{
	char szFindText[MAX_PATH] = "";
	sprintf_s( szFindText, "%d", iFindNumber );
	return IsStringCheck( szFullText, szFindText, cSection );
}

void SplitString( char *szFullText, IntVec &rkReturn, char cSection )
{
	int  iDstCount = 0;
	char szDst[MAX_PATH] = "";
	int iMaxLen = strlen( szFullText );
	for(int i = 0;i < iMaxLen;i++)
	{
		if( szFullText[i] == cSection )
		{
			rkReturn.push_back( atoi( szDst ) );
			memset( szDst, 0, sizeof( szDst ) );
			iDstCount = 0;
		}
		else
		{
			szDst[iDstCount++] = szFullText[i];
		}
	}

	if( strlen( szDst ) != 0 )
		rkReturn.push_back( atoi( szDst ) );
}

void SetNagleAlgorithm( bool bOn )
{
	s_bNagleAlgorithm = bOn;
}

bool IsNagleAlgorithm()
{
	return s_bNagleAlgorithm;
}

void SetPlazaNagleAlgorithm( bool bOn )
{
	s_bPlazaNagleAlgorithm = bOn;
}

bool IsPlazaNagleAlgorithm()
{
	return s_bPlazaNagleAlgorithm;
}

void SetCharChangeToUDP( bool bUDP )
{
	s_bCharChangeToUDP = bUDP;
}

bool IsCharChangeToUDP()
{
	return s_bCharChangeToUDP;
}

void SetOnlyServerRelay( bool bRelay )
{
	s_bOnlyServerRelay = bRelay;
}

bool IsOnlyServerRelay()
{
	return s_bOnlyServerRelay;
}

void SetWholeChatOn( bool bOn )
{
	s_bWholeChatOn = bOn;
}

void SetSpawnNpc( bool bSpawn )
{
	s_bSpawnNpc = bSpawn;
}

bool IsWholeChatOn()
{
	return s_bWholeChatOn;
}

void SetServerRelayToTCP( bool bTCP )
{
	s_bServerRelayToTCP = bTCP;
}

bool IsServerRelayToTCP()
{
	return s_bServerRelayToTCP;
}

int GetAwardEtcItemBonus()
{
	return s_iAwardEtcItemBonus;
}

int GetAwardEtcItemBonusMent()
{
	return s_iAwardEtcItemBonusMent;
}

int GetAwardEtcItemBonusAbusePoint()
{
	return s_iAwardEtcItemBonusAbusePoint;
}

int GetDefaultBestFriendCount()
{
	return s_iDefaultBestFriendCount;
}

int GetBestFriendDismissDelayHour()
{
	return s_iBestFriendDismissDelayHour;
}

int GetCharRentalMinute()
{
	return s_iCharRentalMinute;
}

int GetBestFriendCharRentalDelayHour()
{
	return s_iBestFriendCharRentalDelayHour;
}

int GetCharRentalCount()
{
	return s_iCharRentalCount;
}

int GetCharRentalGrade()
{
	return s_iCharRentalGrade;
}

int GetMaxCharRentSet()
{
	return s_iMaxCharRentSet;
}

DWORD GetQuestAbuseSecond()
{
	return s_dwQuestAbuseSecond;
}

bool IsQuestAbuseDeveloperPass()
{
	return s_bQuestAbuseDeveloperPass;
}

bool IsQuestAbuseLogOut()
{
	return s_bQuestAbuseLogOut;
}

int GetCloverMaxCount()
{
	return s_iMaxCloverCount;
}

int GetCloverDefaultCount()
{
	return s_iCloverDefaultCount;
}

int GetGiveCloverTimeMinute()
{
	return s_iGiveCloverTimeMinute;
}

int GetCloverChargeCount()
{
	return s_iCloverChargeCount;
}

int GetCloverChargeTimeMinute()
{
	return s_iCloverChargeTime;
}

int GetCloverSendCount()
{
	return s_iCloverSendCount;
}

int GetCloverReceiveCount()
{
	return s_iCloverReceiveCount;
}

int GetCloverSendTimeMinute()
{
	return s_iCloverSendTime;
}

int GetCloverReceiveTimeMinute()
{
	return s_iCloverReceiveTime;
}

int GetCloverRefill_Hour()
{
	return s_iCloverRefill_Hour;
}

int GetCloverRefill_Min()
{
	return s_iCloverRefill_Min;
}

bool GetCloverRefillTest()
{
	return s_bCloverRefill_Test;
}

int GetCloverRefillCycle()
{
	return s_iCloverRefill_Cycle;
}

bool IsSpawnNpc()
{
	return s_bSpawnNpc;
}

DWORD GetUserDBAgentID( const ioHashString &rPrivateID )
{
	int iAgentCount =  g_DBClient.GetNodeSize();
	if( iAgentCount <= 1 ) 
	{
		Debug("UserNode::DBAgent Index - zero\r\n");
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"UserNode::DBAgent Index - zero" );
		return 0;
	}

	DWORD dwUserAgentID = 0;

	// 32개의 게임서버당 1대의 디비에이전트를 할당
	int iBaseIndex = (g_ServerNodeManager.GetMaxServerNodes() / 32 + 1);
	if(iAgentCount > iBaseIndex)
	{
		dwUserAgentID = (rPrivateID.GetHashCode() % (iAgentCount-iBaseIndex)) + iBaseIndex;
	}
	Debug("UserNode::DBAgent Index - %lu\r\n", dwUserAgentID);
	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"UserNode::DBAgent Index - %lu (%d, %d, %d)", dwUserAgentID, iAgentCount, g_ServerNodeManager.GetMaxServerNodes(), iBaseIndex );
	return dwUserAgentID;
}

// WSAStartup() 호출 이후에 호출해야함
bool GetLocalIpAddressList( OUT ioHashStringVec &rvIPList, IN bool bMessageBox )
{
	char szHostName[MAX_PATH];
	ZeroMemory( szHostName, sizeof( szHostName ) );
	gethostname(szHostName, sizeof(szHostName));

	LPHOSTENT lpstHostent = gethostbyname(szHostName);
	if ( !lpstHostent ) 
	{
		if( bMessageBox )
			CriticalLOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "GetLocalIpAddressList lpstHostend == NULL.", "IOEnter", MB_OK  );
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s lpstHostend == NULL.", __FUNCTION__ );
		return false;
	}

	enum { MAX_LOOP = 100, };
	LPIN_ADDR lpstInAddr = NULL;
	if( lpstHostent->h_addrtype == AF_INET )
	{
		for (int i = 0; i < MAX_LOOP ; i++) // 100개까지 NIC 확인
		{
			lpstInAddr = (LPIN_ADDR)* lpstHostent->h_addr_list;

			if( lpstInAddr == NULL )
				break;

			char szTemp[MAX_PATH]="";
			StringCbCopy( szTemp, sizeof( szTemp ), inet_ntoa(*lpstInAddr) );
			ioHashString sTemp = szTemp;
			rvIPList.push_back( sTemp );			

			lpstHostent->h_addr_list++;
		}
	}

	if( rvIPList.empty() )
	{
		if( bMessageBox )
			CriticalLOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "GetLocalIpAddressList Local IP empty.", "IOEnter", MB_OK  );
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s Local IP empty.", __FUNCTION__ );
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// PVE
//////////////////////////////////////////////////////////////////////////

bool IsMonsterDungeonMode(ModeType eMode)
{
	switch (eMode)
	{
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_FACTORY:
		return true;
	}

	return false;
}

int ToGetDay( int iYear, int iMonth )  // 입력한 달이 몇 칠까진지 구한다.
{ 
	int MonthArray[12]={ 31,28,31,30,31,30,31,31,30,31,30,31 };
	int iRetDay = 0;
	if(iMonth  > 0 && iMonth < 13)
	{  
		if(iMonth == 2)
		{
			MonthArray[1] = (iYear%4) ? 28 : (iYear%100) ? 29 : (iYear%400) ? 28 : 29;  // 윤달인지 체크 한다. 
		}
		iRetDay = MonthArray[iMonth-1];
	}
	return  iRetDay;
}
bool IsAvailableDate( int iYear, int iMonth, int iDay )
{
	// 2000 ~ 3000 Year
	if( iYear <= 0 || 99 < iYear )
	{
		return false;
	}

	if( iMonth < 1 || 12 < iMonth )
	{
		return false;
	}

	if( iDay < 1 || Help::ToGetDay( iYear, iMonth ) < iDay )
	{
		return false;
	}

	return true;
}

int GetDatePeriod( int iStartYear,
	int iStartMonth,
	int iStartDay,
	int iStartHour,
	int iStartMin,
	int iEndYear,
	int iEndMonth,
	int iEndDay,
	int iEndHour,
	int iEndMin,
	PeriodType eType )
{

	//difftime 함수를 이용해서 두 날짜의 차이값이 허용 범위를 초과 하게 되면 0이되므로 
	//최소 날짜를 2000, 1, 1일로 설정
	time_t rawtime = time(NULL);

	tm StartTime;
	localtime_s( &StartTime, &rawtime );
	StartTime.tm_year = max( 100, iStartYear - 1900 );
	StartTime.tm_mon  = max( 0, iStartMonth - 1 );
	StartTime.tm_mday = max( 1, iStartDay );
	StartTime.tm_hour = iStartHour;
	StartTime.tm_min  = iStartMin;

	tm EndTime;
	localtime_s( &EndTime, &rawtime );
	EndTime.tm_year = max( 100, iEndYear - 1900 );
	EndTime.tm_mon  = max( 0, iEndMonth - 1 );
	EndTime.tm_mday = max( 1, iEndDay );
	EndTime.tm_hour = iEndHour;
	EndTime.tm_min  = iEndMin;

	time_t tStart = mktime( &StartTime );
	time_t tEnd = mktime( &EndTime );

	if( eType == PT_DAY )
		return difftime( tEnd, tStart ) / ( 60 * 60 * 24 );
	else if( eType == PT_HOUR )
		return difftime( tEnd, tStart ) / ( 60 * 60 );
	else
		return difftime( tEnd, tStart ) / 60;
}

void TokenizeToINT(const string& str, const string& delimiters, IntVec& vToken)
{
	vToken.clear();

	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		vToken.push_back( atoi(str.substr(lastPos, pos - lastPos).c_str()) );
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void TokenizeToSTRING(const string& str, const string& delimiters, std::vector<std::string>& vToken)
{
	vToken.clear();

	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	
	std::string token;
	while (string::npos != pos || string::npos != lastPos)
	{
		token = str.substr(lastPos, pos - lastPos);
		vToken.push_back(token);

		lastPos = str.find_first_not_of(delimiters, pos);

		pos = str.find_first_of(delimiters, lastPos);
	}
}

DWORD ConvertDBTIMESTAMPToDWORD(DBTIMESTAMP& RecvDate)
{
	if( !Help::IsAvailableDate(RecvDate.year - 2000, RecvDate.month, RecvDate.day) )
		return 0;

	CTime cRecvDate(RecvDate.year, RecvDate.month, RecvDate.day, RecvDate.hour, RecvDate.minute, 0);

	return cRecvDate.GetTime();
}

SYSTEMTIME GetSysTimeForDWORD( DWORD dwDate )
{
	enum { DEFAULT_YEAR = 2000,	DATE_YEAR_VALUE = 10000, DATE_MONTH_VALUE= 100, };

	WORD wYear = dwDate / DATE_YEAR_VALUE;
	WORD wMonth = (dwDate % DATE_YEAR_VALUE) / DATE_MONTH_VALUE;
	WORD wDay = dwDate % DATE_MONTH_VALUE;
	wYear += DEFAULT_YEAR;
	return GetSafeValueForCTimeConstructor( wYear, wMonth, wDay, 0, 0, 0 );
}

int GetRefillRaidTicketCount()
{
	return s_iRefillRaidTicketCount;
}

int GetRefillRaidTicketMax()
{
	return s_iRefillRaidTicketMax;
}

int GetRefillRaidTicketTime()
{
	return s_iRefillRaidTicketTime;
}

int GetDefaultRaidTicketCnt()
{
	return s_iDefaultRaidTicketCount;
}

float RoundOff(float fVal)
{
	fVal += 0.5;
	
	fVal = floor(fVal);
	return fVal;
}

}	// namespace