#include "StdAfx.h"
#include "ioMission.h"
#include "ioMissionManager.h"


ioMission::ioMission(void):
m_dwStartDay( 0 ),
m_iMagicCode( 0 ),
m_iValue1( 0 ),
m_iValue2( 0 ),
m_iValue3( 0 ),
m_iValue4( 0 ),
m_byClearMissionType( MIS_NONE ),
m_iCurAchieveValue( 0 ),
m_iMissionType( MISSION_CLASS_NONE ),
m_iPresentNum( 0 ),
m_BackFrameType( MBFT_NORMAL ),
m_AttainShowStyle( MASS_VALUE ),
m_MissionSortType( MSORT_NONE),
m_bShowToolTipReward( false ),
m_bIsCostume( false )
{
	m_IconName.Clear();
	m_szRewardTextVec.clear();
}

ioMission::ioMission( const ioMission &rhs ):
m_dwStartDay( rhs.m_dwStartDay ),
m_iMagicCode( rhs.m_iMagicCode ),
m_iValue1( rhs.m_iValue1 ),
m_iValue2( rhs.m_iValue2 ),
m_iValue3( rhs.m_iValue3 ),
m_iValue4( rhs.m_iValue4 ),
m_byClearMissionType( rhs.m_byClearMissionType ),
m_iCurAchieveValue( rhs.m_iCurAchieveValue ),
m_iMissionType( rhs.m_iMissionType ),
m_iPresentNum( rhs.m_iPresentNum ),
m_IconName( rhs.m_IconName ),
m_vTitle( rhs.m_vTitle ),
m_BackFrameType( rhs.m_BackFrameType ),
m_AttainShowStyle( rhs.m_AttainShowStyle ),
m_szRewardTextVec( rhs.m_szRewardTextVec ),
m_MissionSortType( rhs.m_MissionSortType ),
m_bShowToolTipReward( rhs.m_bShowToolTipReward ),
m_bIsCostume( rhs.m_bIsCostume )
{

}

ioMission::~ioMission(void)
{
}

void ioMission::Init( const char* szINIKeyName, ioINILoader &kLoader )
{
	char szBuf[MAX_PATH] = "",szValue[MAX_PATH] = "";
	enum { MAX_PRESENT_TEXT_LINE = 4 };
	//StartDay
	m_dwStartDay = (DWORD)kLoader.LoadInt( "start_date", 0 );

	//Code
	sprintf_s_e( szBuf, "%s_code", szINIKeyName );
	m_iMagicCode = kLoader.LoadInt( szBuf, 0 );

	//Type
	sprintf_s_e( szBuf, "%s_type", szINIKeyName );
	m_iMissionType = kLoader.LoadInt( szBuf, 0 );

	//Title
	LoadTitleInfo( szINIKeyName, kLoader );
	
	//Icon
	ZeroMemory( szValue, sizeof(szValue) );
	sprintf_s_e( szBuf, "%s_icon", szINIKeyName );
	kLoader.LoadString( szBuf, "", szValue, MAX_PATH );
	m_IconName = szValue;

	//Value
	ZeroMemory( szValue, sizeof(szValue) );
	sprintf_s_e( szBuf, "%s_values", szINIKeyName );
	kLoader.LoadString( szBuf, "", szValue, MAX_PATH );
	StringVector kString = ioStringConverter::Split( szValue, "." );
	for ( int i=0 ; i<(int)kString.size() ; ++i )
	{
		if ( i== 0 )
			m_iValue1 =atoi( kString[i].c_str() );
		if ( i== 1 )
			m_iValue2 =atoi( kString[i].c_str() );
		if ( i== 2 )
			m_iValue3 =atoi( kString[i].c_str() );
		if ( i== 3 )
			m_iValue4 =atoi( kString[i].c_str() );
	}

	//선물 타입
	sprintf_s_e( szBuf, "%s_present", szINIKeyName );
	m_iPresentNum = kLoader.LoadInt( szBuf, 0 );

	//frame type
	sprintf_s_e( szBuf, "%s_back_frame_type", szINIKeyName );
	m_BackFrameType = (MissionBackFrameType)kLoader.LoadInt( szBuf, MBFT_NORMAL );

	//Attain Show Style 
	sprintf_s_e( szBuf, "%s_attin_show_style", szINIKeyName );
	m_AttainShowStyle = (MissionAttainShowStyle)kLoader.LoadInt( szBuf, MASS_VALUE );

	//Reward
	for ( int iRewardCnt = 1 ; iRewardCnt<MAX_PRESENT_TEXT_LINE+1 ; ++iRewardCnt )
	{
		ioHashString szRewardText;
		szRewardText.Clear();
		ZeroMemory( szValue, sizeof(szValue) );
		sprintf_s_e( szBuf, "%s_present_text%d", szINIKeyName, iRewardCnt );
		kLoader.LoadString( szBuf, "", szValue, MAX_PATH );
		if( strcmp( szValue, "" ) == 0) 
			break;
		szRewardText = szValue;
		m_szRewardTextVec.push_back( szRewardText );
	}

	sprintf_s_e( szBuf, "%s_show_tooltip_reward", szINIKeyName );
	m_bShowToolTipReward = kLoader.LoadBool( szBuf, false );

	m_byClearMissionType = MIS_NONE;

	sprintf_s_e( szBuf, "%s_costume", szINIKeyName );
	m_bIsCostume = kLoader.LoadBool( szBuf, false );
}

void ioMission::LoadTitleInfo( const char* szINIKeyName, ioINILoader& kLoader )
{
	enum { TITLE_LINE = 2, MAX_TEXT_COLOR = 10, };
	char szColorKeyName[MAX_PATH] = "";
	char szTextKeyName[MAX_PATH] = "";

	char szColorValue[MAX_PATH]="";
	char szTextValue[MAX_PATH]="";

	vTitleText			vItemGrade;

	for(int iLine = 1; iLine < TITLE_LINE + 1; ++iLine )
	{
		ColorText kColorText;
		vTitleLine vColorTextLine;

		for( int iSentence = 1 ; iSentence<MAX_TEXT_COLOR ; ++iSentence )
		{
			memset(szColorKeyName, 0, sizeof(szColorKeyName));
			memset(szTextKeyName, 0, sizeof(szTextKeyName));
			memset(szColorValue, 0, sizeof(szColorValue));
			memset(szTextValue, 0, sizeof(szTextValue));

			//color, text
			StringCbPrintf_e( szColorKeyName, sizeof(szColorKeyName), "%s_title%d_color%d" ,szINIKeyName, iLine, iSentence  );
			StringCbPrintf_e( szTextKeyName, sizeof(szTextKeyName), "%s_title%d_text%d",szINIKeyName, iLine, iSentence );
			//color, text value
			kLoader.LoadString(szColorKeyName, "", szColorValue, MAX_PATH);
			if( strcmp( szColorValue, "" ) == 0) 
				break;
			char *pTemp = NULL;	
			kColorText.m_dwColor = strtoul(szColorValue, &pTemp, 16);

			kLoader.LoadString( szTextKeyName, "", szTextValue, MAX_PATH );	
			if( strcmp( szTextValue, "" ) == 0) 
				break;

			Help::ChangeCharSet( szTextValue, '#', ' ' );

			kColorText.m_szText = szTextValue;
			vColorTextLine.push_back(kColorText);
			kColorText.Clear();
		}

		if( vColorTextLine.empty() ) 
		{
			if( iLine == 1 )
			{
				kColorText.m_dwColor = 0;
				kColorText.m_szText  = "Untitled";
				vColorTextLine.push_back( kColorText ); 
				kColorText.Clear();
			}
		}
		if( !vColorTextLine.empty() ) 
			m_vTitle.push_back( vColorTextLine );
	}
}

bool ioMission::Process()
{
	return false;
}

void ioMission::SetCurAchieveValue( int iCurAchieveValue )
{
	if ( iCurAchieveValue > GetMaxAchieveValue() )
		iCurAchieveValue = GetMaxAchieveValue();
	m_iCurAchieveValue = iCurAchieveValue;
}

void ioMission::SetClearMissionType( BYTE byCurAchieveValue )
{
	if ( COMPARE( byCurAchieveValue, MIS_PROGRESS, (MIS_COMPLETE + 1) ) )
		m_byClearMissionType = byCurAchieveValue;
	else
		LOG.PrintTimeAndLog( 0, "ioMissionManager::SetClearMissionType : Wrong Mission Type : %d", byCurAchieveValue );
}

void ioMission::SetMissionState( int iCurAchieveValue, BYTE byCurAchieveValue )
{
	SetCurAchieveValue( iCurAchieveValue );
	SetClearMissionType( byCurAchieveValue );
	SetSortType();
}

void ioMission::SetSortType()
{
	if ( m_iMissionType == MISSION_CLASS_DAILY_ALL_CLEAR || m_iMissionType == MISSION_CLASS_WEEKLY_ALL_CLEAR ||
		 m_iMissionType == MISSION_CLASS_MONTHLY_ALL_CLEAR )
		m_MissionSortType = MSORT_ALL_CLEAR;
	else if ( m_byClearMissionType == MIS_PROGRESS )
		m_MissionSortType = MSORT_PROGRESS;
	else if ( m_byClearMissionType == MIS_ATTAIN )
		m_MissionSortType = MSORT_ATTAIN;
	else if ( m_byClearMissionType == MIS_COMPLETE )
		m_MissionSortType = MSORT_COMPLETE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

ioLoginTimeMission::ioLoginTimeMission( void ) :
m_iLoginwSecond( 0 ),
m_dwProcessTime( 0 ),
m_iGapMinute( 0 )
{

}

ioLoginTimeMission::ioLoginTimeMission( const ioLoginTimeMission &rhs ):
ioMission( rhs ),
m_iLoginwSecond( rhs.m_iLoginwSecond ),
m_dwProcessTime( rhs.m_dwProcessTime ),
m_iGapMinute( rhs.m_iGapMinute )
{
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	m_NowTime = CTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
}

void ioLoginTimeMission::Init( const char* szINIKeyName, ioINILoader &kLoader )
{
	m_dwProcessTime = FRAMEGETTIME();
	m_iGapMinute = 0;
}

ioLoginTimeMission::~ioLoginTimeMission( void )
{

}

bool ioLoginTimeMission::Process()
{
	if ( m_byClearMissionType != MIS_PROGRESS )
		return false;

	enum { PROCESS_TIC = 30000, };

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwProcessTime + PROCESS_TIC < dwCurTime )
	{
		m_dwProcessTime = dwCurTime;

		SYSTEMTIME rkServerTime;
		g_MyInfo.GetServerTime( rkServerTime );
		CTime CurServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
		
		CTimeSpan GapTime = CurServerTime - m_NowTime;

		int iGapMinute = GapTime.GetTotalMinutes() - m_iGapMinute;
		m_iGapMinute = GapTime.GetTotalMinutes();
		SetCurAchieveValue( m_iCurAchieveValue + iGapMinute );
		  
		if ( m_iCurAchieveValue >= GetMaxAchieveValue() )
		{
			SP2Packet kPacket( CTPK_MISSION_TIME_CHECK );
			kPacket << m_iMagicCode;
			TCPNetwork::SendToServer( kPacket );
		}
		return true;
	}

	return false;
}

ioMission* ioLoginTimeMission::Clone()
{
	return new ioLoginTimeMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioFishingMission::ioFishingMission( void )
{

}

ioFishingMission::ioFishingMission( const ioFishingMission &rhs ):
ioMission( rhs )
{

}

ioFishingMission::~ioFishingMission( void )
{

}

ioMission* ioFishingMission::Clone()
{
	return new ioFishingMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioPesoCollectMission::ioPesoCollectMission( void )
{

}

ioPesoCollectMission::ioPesoCollectMission( const ioPesoCollectMission &rhs ):
ioMission( rhs )
{

}

ioPesoCollectMission::~ioPesoCollectMission( void )
{

}

ioMission* ioPesoCollectMission::Clone()
{
	return new ioPesoCollectMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioMonsterKillMission::ioMonsterKillMission( void )
{

}

ioMonsterKillMission::ioMonsterKillMission( const ioMonsterKillMission &rhs ):
ioMission( rhs )
{

}

ioMonsterKillMission::~ioMonsterKillMission( void )
{

}

ioMission* ioMonsterKillMission::Clone()
{
	return new ioMonsterKillMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioItemBuyMission::ioItemBuyMission( void )
{

}

ioItemBuyMission::ioItemBuyMission( const ioItemBuyMission &rhs ):
ioMission( rhs )
{

}

ioItemBuyMission::~ioItemBuyMission( void )
{

}

ioMission* ioItemBuyMission::Clone()
{
	return new ioItemBuyMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioCloverMission::ioCloverMission( void )
{

}

ioCloverMission::ioCloverMission( const ioCloverMission &rhs ):
ioMission( rhs )
{

}

ioCloverMission::~ioCloverMission( void )
{

}

ioMission* ioCloverMission::Clone()
{
	return new ioCloverMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioExcavationMission::ioExcavationMission( void )
{

}

ioExcavationMission::ioExcavationMission( const ioExcavationMission &rhs ):
ioMission( rhs )
{

}

ioExcavationMission::~ioExcavationMission( void )
{

}

ioMission* ioExcavationMission::Clone()
{
	return new ioExcavationMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioItemUseMission::ioItemUseMission( void )
{

}

ioItemUseMission::ioItemUseMission( const ioItemUseMission &rhs ):
ioMission( rhs )
{

}

ioItemUseMission::~ioItemUseMission( void )
{

}

ioMission* ioItemUseMission::Clone()
{
	return new ioItemUseMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioItemReinforceMission::ioItemReinforceMission( void )
{

}

ioItemReinforceMission::ioItemReinforceMission( const ioItemReinforceMission &rhs ):
ioMission( rhs )
{

}

ioItemReinforceMission::~ioItemReinforceMission( void )
{

}

ioMission* ioItemReinforceMission::Clone()
{
	return new ioItemReinforceMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioModePlayMission::ioModePlayMission( void )
{

}

ioModePlayMission::ioModePlayMission( const ioModePlayMission &rhs ):
ioMission( rhs )
{

}

ioModePlayMission::~ioModePlayMission( void )
{

}

ioMission* ioModePlayMission::Clone()
{
	return new ioModePlayMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioDailyCompleteMisson::ioDailyCompleteMisson( void )
{

}

ioDailyCompleteMisson::ioDailyCompleteMisson( const ioDailyCompleteMisson &rhs ):
ioMission( rhs )
{
	
}

ioDailyCompleteMisson::~ioDailyCompleteMisson( void )
{

}

ioMission* ioDailyCompleteMisson::Clone()
{
	return new ioDailyCompleteMisson( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioDailyAllClearMisson::ioDailyAllClearMisson( void )
{

}

ioDailyAllClearMisson::~ioDailyAllClearMisson( void )
{

}

ioMission* ioDailyAllClearMisson::Clone()
{
	return new ioDailyAllClearMisson( *this );
}

ioDailyAllClearMisson::ioDailyAllClearMisson( const ioDailyAllClearMisson &rhs ) :
ioMission( rhs )
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioWeeklyCompleteMission::ioWeeklyCompleteMission( void )
{

}

ioWeeklyCompleteMission::ioWeeklyCompleteMission( const ioWeeklyCompleteMission &rhs ):
ioMission( rhs )
{

}

ioWeeklyCompleteMission::~ioWeeklyCompleteMission( void )
{

}

ioMission* ioWeeklyCompleteMission::Clone()
{
	return new ioWeeklyCompleteMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioWeeklyAllClearMission::ioWeeklyAllClearMission( void )
{

}

ioWeeklyAllClearMission::ioWeeklyAllClearMission( const ioWeeklyAllClearMission &rhs ):
ioMission( rhs )
{

}

ioWeeklyAllClearMission::~ioWeeklyAllClearMission( void )
{

}

ioMission* ioWeeklyAllClearMission::Clone()
{
	return new ioWeeklyAllClearMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioMonthlyAllClearMission::ioMonthlyAllClearMission( void )
{

}

ioMonthlyAllClearMission::ioMonthlyAllClearMission( const ioMonthlyAllClearMission &rhs ) :
ioMission( rhs )
{

}

ioMonthlyAllClearMission::~ioMonthlyAllClearMission( void )
{

}

ioMission* ioMonthlyAllClearMission::Clone()
{
	return new ioMonthlyAllClearMission( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
ioAdditiveUseMission::ioAdditiveUseMission( void )
{

}

ioAdditiveUseMission::ioAdditiveUseMission( const ioAdditiveUseMission &rhs ):
ioMission( rhs )
{

}

ioAdditiveUseMission::~ioAdditiveUseMission( void )
{

}

ioMission* ioAdditiveUseMission::Clone()
{
	return new ioAdditiveUseMission( *this );
}
