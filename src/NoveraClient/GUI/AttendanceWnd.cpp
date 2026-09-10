
#include "StdAfx.h"

#include "AttendanceWnd.h"

#include "../ioDateHelp.h"
#include "../ioUserSpiritManager.h"

ScaleAniTrack::ScaleAniTrack()
{
	m_AniState = AS_NONE;

	m_dwAniStartTime = 0;
	m_fScaleAniRate  = FLOAT1;
	m_fAlphaAniRate  = FLOAT1;
}

void ScaleAniTrack::SetPlay()
{
	m_AniState		 = AS_PLAY;
	m_dwAniStartTime = FRAMEGETTIME();
}

void ScaleAniTrack::SetEnd()
{
	m_AniState = AS_END;
	m_fScaleAniRate = FLOAT1;
	m_fAlphaAniRate = FLOAT1;
}

void ScaleAniTrack::OnProcess()
{
	if( m_AniState == AS_PLAY )
	{
		if( FRAMEGETTIME() - m_dwAniStartTime < SCALE_ANI )
		{
			float fRate = min( FLOAT1, (float)(FRAMEGETTIME() - m_dwAniStartTime) / (float)SCALE_ANI );
			m_fScaleAniRate = 3.0f * FLOAT1 - fRate;
			m_fAlphaAniRate =  fRate;
		}
		else
		{
			SetEnd();
		}
	}
}

//////////////////////////////////////////////////////////////////////////

CalendarInfo::CalendarInfo()
{
	m_iWeekOfDay    = 0;
	m_iDay          = 0;
	m_iXPos         = 0;
	m_iYPos         = 0;

	m_iPresentType  = 0;
	m_iValue1       = 0;
	m_iValue2       = 0;

	m_bHidden       = false;
	m_bChecked      = false;
	m_bReward       = false;

	m_iIconIndex = -1;
}

CalendarInfo::~CalendarInfo()
{
}

bool CalendarInfo::IsEmpty()
{
	if( m_iDay == 0 )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////

AttendanceWnd::AttendanceWnd()
{
	m_pOutLine		      = NULL;
	m_pTodayOutLine		  = NULL;
	m_pAccrueTtitleBG     = NULL;
	m_pCalendarItemIconBG = NULL;
	m_pItemIconBG		  = NULL;
	m_pHiddenIcon		  = NULL;
	m_pGetIcon			  = NULL;
	m_pAttendanceIcon	  = NULL;	
	m_NoneIcon			  = NULL;
	m_TodayRewardIcon	  = NULL;
	m_AccrueRewardIcon    = NULL;
	m_pTodaySubIcon = NULL;
	m_pAccrueSubIcon = NULL;

	for( int i = 0; i < MAX_EFFECT; ++i )
	{
		m_pGetEffect[i] = NULL;
	}

	m_TodayGetEffect.Init();
	m_AccrueGetEffect.Init();

	m_iCurrAccrueTerm	  = 0;
	m_iCurrAccrueMinTerm  = 0;
	m_iCurrAccrueMaxTerm  = 0;
	
	m_iCurYear  = 0;
	m_iCurMonth = 0;
	m_iCurDay	= 0;

	m_iCalendarCurrFocusIndex = 0;
}

AttendanceWnd::~AttendanceWnd()
{
	SAFEDELETE( m_pOutLine );
	SAFEDELETE( m_pTodayOutLine );
	SAFEDELETE( m_pAccrueTtitleBG );
	SAFEDELETE( m_pCalendarItemIconBG );
	SAFEDELETE( m_pItemIconBG );
	SAFEDELETE( m_pHiddenIcon );
	SAFEDELETE( m_pGetIcon );
	SAFEDELETE( m_pAttendanceIcon );
	SAFEDELETE( m_NoneIcon );
	SAFEDELETE( m_TodayRewardIcon );
	SAFEDELETE( m_AccrueRewardIcon );
	SAFEDELETE( m_pTodaySubIcon );
	SAFEDELETE( m_pAccrueSubIcon );

	for( int i = 0; i < MAX_EFFECT; ++i )
	{
		SAFEDELETE( m_pGetEffect[i] );
	}

	Destroy();
}

void AttendanceWnd::Destroy()
{
	ioUIRenderImageList::iterator iter = m_vImageList.begin();
	for( ; iter != m_vImageList.end(); ++iter )
	{
		ioUIRenderImage* PImage = *iter;
		SAFEDELETE( PImage );
	}
	m_vImageList.clear();

	iter = m_vSubImageList.begin();
	for( ; iter != m_vSubImageList.end(); ++iter )
	{
		ioUIRenderImage* PImage = *iter;
		SAFEDELETE( PImage );
	}
	m_vSubImageList.clear();

	m_CalendarInfoMap.clear();
	m_TodayAttendanceMap.clear();
	m_AccrueAttendanceMap.clear();

	SAFEDELETE( m_TodayRewardIcon );
	SAFEDELETE( m_AccrueRewardIcon );
	SAFEDELETE( m_pTodaySubIcon );
	SAFEDELETE( m_pAccrueSubIcon );

	m_TodayGetEffect.Init();
	m_AccrueGetEffect.Init();

	m_iCurYear  = 0;
	m_iCurMonth = 0;
	m_iCurDay	= 0;

	m_iCurrAccrueTerm          = 0;
	m_iCurrAccrueMaxTerm       = 0;
	m_iCurrAccrueMinTerm       = 0;
	m_iCalendarCurrFocusIndex  = 0;
	m_bCurrTodayHidden         = false;
	m_bCurrAccureReward		   = false;

	m_CurrAccureRewardState   = ASS_NONE;
	m_CurrTodayRewardState    = ASS_NONE;
	m_dwTodayRewardStartTime  = 0;
	m_dwAccrueRewardStartTime = 0;
	m_dwTodayRewardStartTime  = 0;
	m_dwAccrueRewardStartTime = 0;
}

void AttendanceWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "AccrueTtitleBG" )
	{
		SAFEDELETE( m_pAccrueTtitleBG );
		m_pAccrueTtitleBG = pImage;
	}
	else if( szType == "CalendarItemIconBG" )
	{
		SAFEDELETE( m_pCalendarItemIconBG );
		m_pCalendarItemIconBG = pImage;

		if( m_pCalendarItemIconBG )
			m_pCalendarItemIconBG->SetScale( 0.7f, 0.7f );
	}
	else if( szType == "ItemIconBG" )
	{
		SAFEDELETE( m_pItemIconBG );
		m_pItemIconBG = pImage;
	}
	else if( szType == "HiddenIcon" )
	{
		SAFEDELETE( m_pHiddenIcon );
		m_pHiddenIcon = pImage;
	}
	else if( szType == "GetIcon" )
	{
		SAFEDELETE( m_pGetIcon );
		m_pGetIcon = pImage;
	}
	else if( szType == "AttendanceIcon" )
	{
		SAFEDELETE( m_pAttendanceIcon );
		m_pAttendanceIcon = pImage;
	}
	else if( szType == "NoneIcon" )
	{
		SAFEDELETE( m_NoneIcon );
		m_NoneIcon = pImage;
	}
	else if( szType == "Effect1" )
	{
		SAFEDELETE( m_pGetEffect[0] );
		m_pGetEffect[0] = pImage;
	}
	else if( szType == "Effect2" )
	{
		SAFEDELETE( m_pGetEffect[1] );
		m_pGetEffect[1] = pImage;
	}
	else if( szType == "Effect3" )
	{
		SAFEDELETE( m_pGetEffect[2] );
		m_pGetEffect[2] = pImage;
	}
	else if( szType == "Effect4" )
	{
		SAFEDELETE( m_pGetEffect[3] );
		m_pGetEffect[3] = pImage;
	}
	else if( szType == "Effect5" )
	{
		SAFEDELETE( m_pGetEffect[4] );
		m_pGetEffect[4] = pImage;
	}
	else if( szType == "Effect6" )
	{
		SAFEDELETE( m_pGetEffect[5] );
		m_pGetEffect[5] = pImage;
	}
	else if( szType == "Effect7" )
	{
		SAFEDELETE( m_pGetEffect[6] );
		m_pGetEffect[6] = pImage;
	}
	else if( szType == "Effect8" )
	{
		SAFEDELETE( m_pGetEffect[7] );
		m_pGetEffect[7] = pImage;
	}
	else if( szType == "Effect9" )
	{
		SAFEDELETE( m_pGetEffect[8] );
		m_pGetEffect[8] = pImage;
	}
	else if( szType == "Effect10" )
	{
		SAFEDELETE( m_pGetEffect[9] );
		m_pGetEffect[9] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void AttendanceWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "OutLine" )
	{
		SAFEDELETE( m_pOutLine );
		m_pOutLine = pFrame;
	}
	else if( szType == "TodayOutLine" )
	{
		SAFEDELETE( m_pTodayOutLine );
		m_pTodayOutLine = pFrame;
		if( m_pTodayOutLine )
			m_pTodayOutLine->SetSize( 71, 67 );
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void AttendanceWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szGetSound   = xElement.GetStringAttribute( "GetSound" );
	m_szStampSound = xElement.GetStringAttribute( "StampSound" );
}

void AttendanceWnd::iwm_create()
{
}

void AttendanceWnd::LoadReward( int iYear, int iMonth )
{
	ioINILoader_e kLoader( "config/sp2_attendance_reward.ini" );
	kLoader.SetTitle( "common" );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	int iMaxLoad = kLoader.LoadInt( "max_load", 0 );

	for( int i = 0; i < iMaxLoad; i++ )
	{
		kLoader.SetTitle( "common" );

		sprintf_s( szKey, "load_date_%d", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		DWORD dwTtitle = kLoader.LoadInt( szKey, 0 );

		if( iYear * 100 + iMonth == dwTtitle )
		{
			LoadTodayAttance( kLoader, dwTtitle );
			LoadAccrueAttance( kLoader, dwTtitle );
		}
	}
}

void AttendanceWnd::LoadTodayAttance( ioINILoader& rkLoader, DWORD dwTtitle )
{
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	
	sprintf_s( szKey, "%d", dwTtitle );
	rkLoader.SetTitle( szKey );
	
	for( int i = 0; i < MAX_DAY; i++ )
	{
		TodayAttendanceReward Reward;

		sprintf_s( szKey, "today_reward%d_type", i + 1 );
		Reward.m_iPresentType = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "today_reward%d_value1", i + 1 );
		Reward.m_iValue1 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "today_reward%d_value2", i + 1 );
		Reward.m_iValue2 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "today_reward%d_hidden", i + 1 );
		Reward.m_bHidden = rkLoader.LoadBool( szKey, false );

		DWORD dwKey =  dwTtitle * 100 + ( i + 1 );
		if( Reward.IsEmpty() )
			continue;		

		m_TodayAttendanceMap.insert( TodayAttendanceMap::value_type( dwKey, Reward ) );
	}
}

void AttendanceWnd::LoadAccrueAttance( ioINILoader& rkLoader, DWORD dwTtitle )
{
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	sprintf_s( szKey, "%d", dwTtitle );
	rkLoader.SetTitle( szKey );
		
	for( int i = 0; i < ACCRUE_MAX; i++ )
	{
		AccrueAttendanceReward Reward;

		sprintf_s( szKey, "accrue_reward%d_term", i + 1 );
		int iTerm = rkLoader.LoadInt( szKey, 0 );
		if( iTerm == 0 )
			continue;

		Reward.m_iAccureTerm = iTerm;

		sprintf_s( szKey, "accrue_reward%d_type", i + 1 );
		Reward.m_iPresentType = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "accrue_reward%d_value1", i + 1 );
		Reward.m_iValue1 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "accrue_reward%d_value2", i + 1 );
		Reward.m_iValue2 = rkLoader.LoadInt( szKey, 0 );
		
		if( Reward.IsEmpty() )
			continue;

		Reward.m_iIconIndex = GetPresentIcon( 0.45, Reward.m_iPresentType, Reward.m_iValue1, Reward.m_iValue2 );
		m_AccrueAttendanceMap.insert( AccrueAttendanceMap::value_type( iTerm, Reward ) );
	}
}

void AttendanceWnd::iwm_show()
{
	Destroy();

	//접속한 달의 1일 기준으로 생성
	m_iCurYear  = g_MyInfo.GetAttendanceConnectTime() / 10000;
	m_iCurMonth = (g_MyInfo.GetAttendanceConnectTime() % 10000) / 100;
	m_iCurDay   = (g_MyInfo.GetAttendanceConnectTime() % 10000) % 100;	

	if( !Help::IsAvailableDate( m_iCurYear, m_iCurMonth, m_iCurDay ) )
		return;

	LoadReward( m_iCurYear, m_iCurMonth );
	InitCalendar( m_iCurYear + 2000, m_iCurMonth, 1 );
	InitCurrAccrueReward();
	CheckAttendanceButton();

#if defined( USE_GA )
	// WND_ATTENDANCE
	g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FATTENDANCE" );
#endif
}

void AttendanceWnd::iwm_hide()
{
	Destroy();
}

bool AttendanceWnd::iwm_esc()
{
	HideWnd();
	return true;
}

bool AttendanceWnd::iwm_spacebar()
{
	ioAttendanceCheckButton* pButton = dynamic_cast<ioAttendanceCheckButton*>( FindChildWnd( ID_ATTENDANCE ) );
	if( pButton->IsShow() )
	{
		iwm_command( FindChildWnd( ID_ATTENDANCE ), IOBN_BTNUP, 0 );
	}
	else
		iwm_command( FindChildWnd( ID_ATTENDANCED ), IOBN_BTNUP, 0 );
	
	return true;
}

int AttendanceWnd::GetPresentIcon( float fScale, int iPresentType, int iValue1, int iValue2 )
{
	ioHashString szIconName = g_PresentMgr.GetPresentIconText(iPresentType, iValue1, iValue2);
	ioHashString szSubIconName = g_PresentMgr.GetPresentSubIconText(iPresentType, iValue1, iValue2);
	if( !szIconName.IsEmpty() )
	{
		ioUIRenderImage* pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		ioUIRenderImage* pSubIcon = g_UIImageSetMgr.CreateImageByFullName( szSubIconName );
		if( pIcon )
		{
			pIcon->SetPosition( 0, 0 );
			pIcon->SetScale( fScale, fScale );
			m_vImageList.push_back( pIcon );

			if( pSubIcon )
			{
				pSubIcon->SetPosition( 0, 0 );
				pSubIcon->SetScale( fScale, fScale );
			}
			m_vSubImageList.push_back( pSubIcon );

			return m_vImageList.size() - 1;
		}
		else
		{
			SAFEDELETE( pIcon );
		}
	}

	return -1;
}

ioUIRenderImage* AttendanceWnd::GetPresentIconImage( float fScale, int iPresentType, int iValue1, int iValue2 )
{
	ioHashString szIconName = g_PresentMgr.GetPresentIconText(iPresentType, iValue1, iValue2);
	ioUIRenderImage* pIcon = NULL;
	if( !szIconName.IsEmpty() )
	{
		pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		if( pIcon )
		{
			pIcon->SetPosition( 0, 0 );
			pIcon->SetScale( fScale, fScale );
		}
		else
		{
			SAFEDELETE( pIcon );
		}
	}
	return pIcon;
}

ioUIRenderImage* AttendanceWnd::GetPresentSubIconImage( float fScale, int iPresentType, int iValue1, int iValue2 )
{
	ioHashString szIconName = g_PresentMgr.GetPresentSubIconText(iPresentType, iValue1, iValue2);
	ioUIRenderImage* pIcon = NULL;
	if( !szIconName.IsEmpty() )
	{
		pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		if( pIcon )
		{
			pIcon->SetPosition( 0, 0 );
			pIcon->SetScale( fScale, fScale );
		}
		else
		{
			SAFEDELETE( pIcon );
		}
	}
	return pIcon;
}

void AttendanceWnd::GetEffectList( OUT ioUIRenderImageList &rkImageList )
{
	rkImageList.clear();
	rkImageList.reserve( MAX_EFFECT );

	for( int i = 0; i< MAX_EFFECT; ++i )
	{
		if( m_pGetEffect[i] )
			rkImageList.push_back( m_pGetEffect[i] );
	}
}

void AttendanceWnd::GetCurCalendarInfo( CalendarToolTipInfo& Info )
{
	CalendarInfoMap::iterator iter = m_CalendarInfoMap.find( m_iCalendarCurrFocusIndex );
	if( iter != m_CalendarInfoMap.end() )
	{
		CalendarInfo& rkInfo = iter->second;
		Info.m_iPresentType  = rkInfo.m_iPresentType;
		Info.m_iValue1       = rkInfo.m_iValue1;
		Info.m_iValue2       = rkInfo.m_iValue2;
		if( rkInfo.m_bChecked )
			Info.m_bHidden = false;
		else
			Info.m_bHidden = rkInfo.m_bHidden;
	}
}

CalendarInfo& AttendanceWnd::FindCalendarInfo( DWORD dwKey )
{
	static CalendarInfo Knone;
	CalendarInfoMap::iterator iter = m_CalendarInfoMap.find( dwKey );
	if( iter == m_CalendarInfoMap.end() )
	{
		return Knone;
	}

	return iter->second;
}

AttendanceWnd::TodayAttendanceReward& AttendanceWnd::GetTodayPresentInfo( DWORD dwDateKey /*Ex : 130903 */ )
{
	static TodayAttendanceReward kNoneReward;

	TodayAttendanceMap::iterator iter  = m_TodayAttendanceMap.find( dwDateKey );
	if( iter != m_TodayAttendanceMap.end() )
	{
		return iter->second;
	}

	return kNoneReward;
}

AttendanceWnd::AccrueAttendanceReward& AttendanceWnd::GetAccruePresentInfo( int iAccrueKey )
{
	static AccrueAttendanceReward kNoneReward;

	AccrueAttendanceMap::iterator iter  = m_AccrueAttendanceMap.find( iAccrueKey );
	if( iter != m_AccrueAttendanceMap.end() )
	{
		return iter->second;
	}

	return kNoneReward;
}

int AttendanceWnd::GetRenderImageStartHeight( ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax )
{
	if( iMax == iCur ) return 0;
	if( !pImage )	return 0;

	float fGap = (float)iCur / (float)iMax;
	float fGapHeight = (float)iSrcHeight * fGap;
	return pImage->GetOffsetY() + ( (iSrcHeight / 2) - fGapHeight );
}

void AttendanceWnd::InitCalendar( int iCurrYear, int iCurrMonth, int iCurrDay )
{
	m_szTodayRewardText1 = "-";
	m_szTodayRewardText2 = "-";
	
	int iThisMonth = iCurrMonth;
	int iWeekOfDay = DateHelp::GetWeekDay( iCurrYear, iCurrMonth, iCurrDay );

	//위에서 구한 첫 날짜를 기준으로 순차적으로 날짜 세팅
	for( int iWeek = 0; iWeek <= MAX_WEEKS; iWeek++ )
	{
		int iDayOfWeek = 0;
		if( iWeek == 0 )
			iDayOfWeek = iWeekOfDay;

		for( ; iDayOfWeek < WEEKS; ++iDayOfWeek )
		{
			CalendarInfo Info;
			Info.m_iDay       = iCurrDay;
			Info.m_iWeekOfDay = iDayOfWeek;
			Info.m_iXPos	  = WEEKS_X + iDayOfWeek * WEEKS_X_OFFSET;
			Info.m_iYPos	  = WEEKS_Y + iWeek * WEEKS_Y_OFFSET;

			DWORD dwKey = (iCurrYear - 2000) * 10000 +  iCurrMonth * 100 + iCurrDay;
			if( iThisMonth != iCurrMonth )
				return;

			const TodayAttendanceReward& rkReward = GetTodayPresentInfo( dwKey );
			
			Info.m_bHidden      = rkReward.m_bHidden;
			Info.m_bReward      = !rkReward.IsEmpty();
			Info.m_bChecked     = g_MyInfo.HasAttendanceRecord( dwKey );
			Info.m_iPresentType = rkReward.m_iPresentType;
			Info.m_iValue1      = rkReward.m_iValue1;
			Info.m_iValue2      = rkReward.m_iValue2;

			if( Info.m_bChecked )
				Info.m_AniTrack.SetEnd();

			Info.m_iIconIndex = GetPresentIcon( 0.7f, rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 );
			m_CalendarInfoMap.insert( CalendarInfoMap::value_type( dwKey, Info ) );

			if( dwKey == g_MyInfo.GetAttendanceConnectTime() )
			{
				m_CurrTodayRewardState = ASS_NONE;
				if( Info.m_bChecked )
				{
					m_CurrTodayRewardState = ASS_STAMP;
					ioAttendanceCheckButton* pButton = dynamic_cast<ioAttendanceCheckButton*>( FindChildWnd( ID_ATTENDANCE ) );
					if( pButton )
						pButton->SetEnd();
				}

				m_bCurrTodayHidden = Info.m_bHidden;

				SAFEDELETE( m_TodayRewardIcon );
				SAFEDELETE( m_pTodaySubIcon );
				m_TodayRewardIcon = GetPresentIconImage( FLOAT1, rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 );
				m_pTodaySubIcon = GetPresentSubIconImage( FLOAT1, rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 );

				if( !rkReward.IsEmpty() )
				{
					m_szTodayRewardText1 = g_PresentMgr.GetPresentValue1Text( rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 );
					m_szTodayRewardText2 = g_PresentMgr.GetPresentValue2Text( rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 );
				}
			}

			DateHelp::IncreaseDay( iCurrYear, iCurrMonth, iCurrDay );
		}
	}
}

void AttendanceWnd::InitCurrAccrueReward()
{
	m_szAccrueRewardText1 = "-";
	m_szAccrueRewardText2 = "-";
	
	m_iCurrAccrueTerm    = g_MyInfo.GetAttendanceRecordCount();
	m_iCurrAccrueMinTerm = 0;
	m_iCurrAccrueMaxTerm = std::numeric_limits<int>::infinity();
	SAFEDELETE( m_AccrueRewardIcon );
	SAFEDELETE( m_pAccrueSubIcon );

	int iPrevTerm = 0;
	AccrueAttendanceMap::iterator Accrueiter = m_AccrueAttendanceMap.begin();
	for( ; Accrueiter != m_AccrueAttendanceMap.end(); ++Accrueiter )
	{
		AccrueAttendanceReward& rkReward = Accrueiter->second;
		if( m_iCurrAccrueTerm < rkReward.m_iAccureTerm )
		{
			m_iCurrAccrueMinTerm = iPrevTerm;
			m_iCurrAccrueMaxTerm = rkReward.m_iAccureTerm;
			m_AccrueRewardIcon   = GetPresentIconImage( FLOAT1, rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 );
			m_pAccrueSubIcon = GetPresentSubIconImage( FLOAT1, rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 );

			m_szAccrueRewardText1 = g_PresentMgr.GetPresentValue1Text( rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 );
			m_szAccrueRewardText2 = g_PresentMgr.GetPresentValue2Text( rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 );
			return;
		}
		else
		{
			rkReward.m_bReward = true;
		}

		iPrevTerm = rkReward.m_iAccureTerm;
	}
}

void AttendanceWnd::SetReward( int iTodayPresentType, int iTodayValue1, int iTodayValue2, int iAccruePresentType, int iAccrueValue1, int iAccrueValue2 )
{
	m_iCurrAccrueTerm = g_MyInfo.GetAttendanceRecordCount();

	ioAttendanceCheckButton* pButton = dynamic_cast<ioAttendanceCheckButton*>( FindChildWnd( ID_ATTENDANCE ) );
	if( pButton )
	{
		pButton->SetEnd();
		pButton->HideWnd();
		ioAttendanceCheckedButton* pCheckedButton = dynamic_cast<ioAttendanceCheckedButton*>( FindChildWnd( ID_ATTENDANCED ) );
		if ( pCheckedButton && !pCheckedButton->IsShow() )
			pCheckedButton->SetStart();
	}

	TodayAttendanceReward& rkToday = GetTodayPresentInfo( g_MyInfo.GetAttendanceConnectTime() );
	
	if( !rkToday.IsEmpty() && rkToday.m_iPresentType == iTodayPresentType && rkToday.m_iValue1 == iTodayValue1 && rkToday.m_iValue2 == iTodayValue2 )
	{
		m_CurrTodayRewardState = ASS_CALENDAR;
		m_bCurrTodayHidden = false;
		m_dwTodayRewardStartTime = FRAMEGETTIME();
		g_SoundMgr.PlaySound( m_szStampSound );
	}	

	AccrueAttendanceReward& rkAccrue = GetAccruePresentInfo( m_iCurrAccrueTerm );
	if( !rkAccrue.IsEmpty() && rkAccrue.m_iPresentType == iAccruePresentType && rkAccrue.m_iValue1 == iAccrueValue1 && rkAccrue.m_iValue2 == iAccrueValue2 )
	{		
		m_CurrAccureRewardState = ASS_CALENDAR;
		m_dwAccrueRewardStartTime = FRAMEGETTIME();
		g_SoundMgr.PlaySound( m_szStampSound );
	}
}

void AttendanceWnd::AddAttendanceRecord( DWORD dwKey )
{
	CalendarInfo& rkInfo = FindCalendarInfo( dwKey );
	if( !rkInfo.IsEmpty() )
	{
		rkInfo.m_bChecked = true;
		rkInfo.m_AniTrack.SetPlay();
	}
}

void AttendanceWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ATTENDANCE:
		if( cmd == IOBN_BTNUP )
		{
			SP2Packet kPacket( CTPK_ATTENDANCE_CHECK );
			kPacket << 0;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		break;
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_ATTENDANCED:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void AttendanceWnd::OnCalanderProcess()
{
	CalendarInfoMap::iterator iter = m_CalendarInfoMap.begin();
	for( ; iter != m_CalendarInfoMap.end(); ++iter )
	{
		CalendarInfo& rkInfo = iter->second;
		rkInfo.m_AniTrack.OnProcess();
	}
}

void AttendanceWnd::OnAniProcess()
{
	m_TodayGetEffect.Process();
	m_AccrueGetEffect.Process();

	m_CurrAccureScaleAniTrack.OnProcess();
	m_CurrTodayBigAniTrack.OnProcess();
	m_CurrAccureBigAniTrack.OnProcess();

	DWORD dwCurrTime = FRAMEGETTIME();

	bool bStampSound = false;
	bool bEffectSound = false;
	if( m_CurrTodayRewardState == ASS_CALENDAR && m_dwAccrueRewardStartTime + 1000 < dwCurrTime )
	{
		m_CurrTodayRewardState = ASS_EFFECT;
		bEffectSound = true;

		ioUIRenderImageList vImageList;
		GetEffectList( vImageList );
		m_TodayGetEffect.DestroySpriteList();
		m_TodayGetEffect.Init();
		m_TodayGetEffect.AddSpriteList( vImageList );
		m_TodayGetEffect.SetType( ioSprite::AT_PLAY_ONCE );
		m_TodayGetEffect.SetAnimationGapTime( 100 );
		m_TodayGetEffect.SetPlay();
	}
	else if( m_CurrTodayRewardState == ASS_EFFECT && m_TodayGetEffect.IsStop() )
	{
		m_CurrTodayRewardState = ASS_STAMP;
		m_CurrTodayBigAniTrack.SetPlay();
		bStampSound = true;
	}

	if( m_CurrAccureRewardState == ASS_CALENDAR && m_dwAccrueRewardStartTime + 1000 < dwCurrTime )
	{
		m_CurrAccureRewardState = ASS_EFFECT;
		bEffectSound = true;

		ioUIRenderImageList vImageList;
		GetEffectList( vImageList );
		m_AccrueGetEffect.DestroySpriteList();
		m_AccrueGetEffect.Init();
		m_AccrueGetEffect.AddSpriteList( vImageList );
		m_AccrueGetEffect.SetType( ioSprite::AT_PLAY_ONCE );
		m_AccrueGetEffect.SetAnimationGapTime( 100 );
		m_AccrueGetEffect.SetPlay();
	}
	else if( m_CurrAccureRewardState == ASS_EFFECT && m_AccrueGetEffect.IsStop() )
	{
		m_CurrAccureRewardState = ASS_STAMP;
		AccrueAttendanceReward& rkAccrue = GetAccruePresentInfo( m_iCurrAccrueTerm );
		rkAccrue.m_bReward = true;

		m_CurrAccureBigAniTrack.SetPlay();
		m_CurrAccureScaleAniTrack.SetPlay();
		bStampSound = true;
	}

	if( bEffectSound )
		g_SoundMgr.PlaySound( m_szGetSound );

	if( bStampSound )
		g_SoundMgr.PlaySound( m_szStampSound );
}

void AttendanceWnd::OnToolTipProcess()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	CalendarInfoMap::iterator iter = m_CalendarInfoMap.begin();
	for( ; iter != m_CalendarInfoMap.end(); ++iter )
	{
		CalendarInfo& rkInfo  = iter->second;

		if( iXPos + rkInfo.m_iXPos <= pMouse->GetMousePos().x && iXPos + rkInfo.m_iXPos + WEEKS_X_OFFSET >= pMouse->GetMousePos().x&&
			iYPos + rkInfo.m_iYPos <= pMouse->GetMousePos().y && iYPos + rkInfo.m_iYPos + WEEKS_Y_OFFSET >= pMouse->GetMousePos().y )
		{
			m_iCalendarCurrFocusIndex = iter->first;
			return;
		}
	}

	m_iCalendarCurrFocusIndex = 0;
}

void AttendanceWnd::OnProcess( float fTimePerSec )
{
	OnCalanderProcess();
	OnAniProcess();
	OnToolTipProcess();
}

void AttendanceWnd::OnRender()
{
	ioWnd::OnRender();	

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnTtitleRender( iXPos, iYPos );
	OnCalanderRender( iXPos, iYPos );	
	OnAccrueRewardRender( iXPos, iYPos );
	OnAccrueTtitleRender( iXPos, iYPos );
	OnTodayAndAccrueRewardRender( iXPos, iYPos );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 17, iYPos + 486, FONT_SIZE_11, "※ <개발자K와 출석 Check> 달력은 매월 1일마다 초기화됩니다." );
	
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 17, iYPos + 502, FONT_SIZE_11, "※ <출석체크 하기(SPACE)> 버튼을 누르면 선물함으로 보상이 지급됩니다." );
};

void AttendanceWnd::OnTtitleRender( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	if( m_iCurMonth < 10 )
		g_FontMgr.PrintText( iXPos + 513, iYPos + 29, FONT_SIZE_26, "0%d", m_iCurMonth );
	else
		g_FontMgr.PrintText( iXPos + 513, iYPos + 29, FONT_SIZE_26, "%d", m_iCurMonth );

	g_FontMgr.PrintText( iXPos + 539, iYPos + 29, FONT_SIZE_26, "월" );
}

void AttendanceWnd::OnCalanderRender( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintText( iXPos + 52, iYPos + 87, FONT_SIZE_12, "일" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 117, iYPos + 87, FONT_SIZE_12, "월" );
	g_FontMgr.PrintText( iXPos + 182, iYPos + 87, FONT_SIZE_12, "화" );
	g_FontMgr.PrintText( iXPos + 247, iYPos + 87, FONT_SIZE_12, "수" );
	g_FontMgr.PrintText( iXPos + 312, iYPos + 87, FONT_SIZE_12, "목" );
	g_FontMgr.PrintText( iXPos + 377, iYPos + 87, FONT_SIZE_12, "금" );
	g_FontMgr.PrintText( iXPos + 442, iYPos + 87, FONT_SIZE_12, "토" );

	if( m_pOutLine )
	{
		m_pOutLine->SetSize( 461, 405 );
		m_pOutLine->Render( iXPos + 17, iYPos + 76 );
	}

	int iTodayXPos = 0;
	int iTodayYPos = 0;

	CalendarInfoMap::iterator iter = m_CalendarInfoMap.begin();
	for( ; iter != m_CalendarInfoMap.end(); ++iter )
	{
		const CalendarInfo& rkInfo = iter->second;
		
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );

		if( rkInfo.m_iWeekOfDay == 0 )
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

		g_FontMgr.PrintText( iXPos + 4 + rkInfo.m_iXPos, iYPos + 4 + rkInfo.m_iYPos, FONT_SIZE_12, "%d", rkInfo.m_iDay );

		if( rkInfo.m_bHidden && !rkInfo.m_bChecked )
		{
			if( m_pHiddenIcon )
			{
				if( m_pCalendarItemIconBG )
					m_pCalendarItemIconBG->Render( iXPos + 36 + rkInfo.m_iXPos, iYPos + 33 + rkInfo.m_iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

				m_pHiddenIcon->SetScale( 0.7f, 0.7f );
				m_pHiddenIcon->Render( iXPos + 36 + rkInfo.m_iXPos, iYPos + 33 + rkInfo.m_iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
		else
		{
			ioUIRenderImage* pIcon = NULL;
			ioUIRenderImage* pSubIcon = NULL;
			if( COMPARE( rkInfo.m_iIconIndex, 0, (int)m_vImageList.size() ) )
				pIcon = m_vImageList[rkInfo.m_iIconIndex];
			if( COMPARE( rkInfo.m_iIconIndex, 0, (int)m_vSubImageList.size() ) )
				pSubIcon = m_vSubImageList[rkInfo.m_iIconIndex];

			if( pIcon )
			{
				if( m_pCalendarItemIconBG )
					m_pCalendarItemIconBG->Render( iXPos + 36 + rkInfo.m_iXPos, iYPos + 33 + rkInfo.m_iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

				if ( !rkInfo.m_bChecked )
					pIcon->Render( iXPos + 36 + rkInfo.m_iXPos, iYPos + 33 + rkInfo.m_iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				else
					pIcon->Render( iXPos + 36 + rkInfo.m_iXPos, iYPos + 33 + rkInfo.m_iYPos, UI_RENDER_GRAY, TFO_BILINEAR );
			}
			if( pSubIcon )
			{
				if ( !rkInfo.m_bChecked )
					pSubIcon->Render( iXPos + 36 + rkInfo.m_iXPos, iYPos + 33 + rkInfo.m_iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				else
					pSubIcon->Render( iXPos + 36 + rkInfo.m_iXPos, iYPos + 33 + rkInfo.m_iYPos, UI_RENDER_GRAY, TFO_BILINEAR );
			}
		}

		if( rkInfo.m_bReward )
		{
			if( m_pGetIcon && rkInfo.m_bChecked )
			{
				m_pGetIcon->SetAlpha( rkInfo.m_AniTrack.m_fAlphaAniRate * 255 );
				m_pGetIcon->SetScale( rkInfo.m_AniTrack.m_fScaleAniRate, rkInfo.m_AniTrack.m_fScaleAniRate );
				m_pGetIcon->Render( iXPos + 1 + rkInfo.m_iXPos, iYPos + 21 + rkInfo.m_iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
		else
		{
			if( m_pAttendanceIcon && rkInfo.m_bChecked )
			{
				m_pAttendanceIcon->SetAlpha( rkInfo.m_AniTrack.m_fAlphaAniRate * 255 );
				m_pAttendanceIcon->SetScale( rkInfo.m_AniTrack.m_fScaleAniRate, rkInfo.m_AniTrack.m_fScaleAniRate );
				m_pAttendanceIcon->Render( iXPos + 13 + rkInfo.m_iXPos, iYPos + 11 + rkInfo.m_iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}

		if( m_iCurDay == rkInfo.m_iDay )
		{
			iTodayXPos = iXPos + rkInfo.m_iXPos - 3;
			iTodayYPos = iYPos + rkInfo.m_iYPos - 2;
		}
	}

	if( m_pTodayOutLine )	
		m_pTodayOutLine->Render( iTodayXPos, iTodayYPos, UI_RENDER_NORMAL );	
}

void AttendanceWnd::OnAccrueTtitleRender( int iXPos, int iYPos )
{
	if( m_pAccrueTtitleBG )
		m_pAccrueTtitleBG->Render( iXPos + 520, iYPos + 92 );
	
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
	g_FontMgr.SetBkColor( 0xFFFFFF );
	g_FontMgr.PrintText( iXPos + 602, iYPos + 103, FONT_SIZE_15, "현재 누적 출석 %d일", m_iCurrAccrueTerm );
}

void AttendanceWnd::OnAccrueRewardRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );	
	g_FontMgr.SetBkColor( 0, 0, 0 );
	
	AccrueAttendanceMap::const_iterator iter = m_AccrueAttendanceMap.begin();
	for( int i = 0; iter != m_AccrueAttendanceMap.end(); ++iter, ++i )
	{
		const AccrueAttendanceReward& rkReward = iter->second;
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 516 + i * 43, iYPos + 140, FONT_SIZE_13, "%d일", rkReward.m_iAccureTerm );

		ioUIRenderImage* pIcon = NULL;
		ioUIRenderImage* pSubIcon = NULL;
		if( COMPARE( rkReward.m_iIconIndex, 0, (int)m_vImageList.size() ) )
			pIcon = m_vImageList[rkReward.m_iIconIndex];
		if( COMPARE( rkReward.m_iIconIndex, 0, (int)m_vSubImageList.size() ) )
			pSubIcon = m_vSubImageList[rkReward.m_iIconIndex];

		if( pIcon )
		{
			if( m_pItemIconBG )
			{
				m_pItemIconBG->SetScale( 0.45f, 0.45f );
				m_pItemIconBG->Render( iXPos + 517 + i *43, iYPos + 203, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			}
			pIcon->Render( iXPos + 517 + i *43, iYPos + 203, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		if( pSubIcon )
			pSubIcon->Render( iXPos + 517 + i *43, iYPos + 203, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_iCurrAccrueTerm < rkReward.m_iAccureTerm )
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			if( m_pGetIcon && rkReward.m_bReward )
			{
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
				if( m_iCurrAccrueTerm == rkReward.m_iAccureTerm )
				{
					m_pGetIcon->SetAlpha( m_CurrAccureScaleAniTrack.m_fAlphaAniRate * 255 );
					m_pGetIcon->SetScale( m_CurrAccureScaleAniTrack.m_fScaleAniRate, m_CurrAccureScaleAniTrack.m_fScaleAniRate );
				}
				else
				{
					m_pGetIcon->SetAlpha( 255 );
					m_pGetIcon->SetScale( FLOAT1, FLOAT1 );
				}
				m_pGetIcon->Render( iXPos + 498 + i *43, iYPos + 185, UI_RENDER_NORMAL );
			}
		}

		g_FontMgr.PrintTextWidthCut( iXPos + 516 + i * 43, iYPos + 232, FONT_SIZE_10, 43,"%s", g_PresentMgr.GetPresentValue1Text( rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 ) );
		g_FontMgr.PrintTextWidthCut( iXPos + 516 + i * 43, iYPos + 244, FONT_SIZE_10, 43,"%s", g_PresentMgr.GetPresentValue2Text( rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2 ) );
	}

	if( m_pOutLine )
	{
		m_pOutLine->SetSize( 221, 152 );
		m_pOutLine->Render( iXPos + 493, iYPos + 110 );
	}
}

void AttendanceWnd::OnTodayAndAccrueRewardRender( int iXPos, int iYPos )
{
	if( m_pOutLine )
	{
		m_pOutLine->SetSize( 221, 150 );
		m_pOutLine->Render( iXPos + 492, iYPos + 272 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.PrintText( iXPos + 548, iYPos + 280, FONT_SIZE_13, "오늘의 상품" );
	g_FontMgr.PrintText( iXPos + 656, iYPos + 280, FONT_SIZE_13, "누적 출석 상품" );
	
	if( m_TodayRewardIcon )
	{		
		if( m_pItemIconBG )
		{
			m_pItemIconBG->SetScale( FLOAT1, FLOAT1 );
			m_pItemIconBG->Render( iXPos + 549, iYPos + 342, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}

		if( m_bCurrTodayHidden && m_CurrTodayRewardState != ASS_STAMP)
		{
			m_pHiddenIcon->SetScale( FLOAT1, FLOAT1 );
			m_pHiddenIcon->Render( iXPos + 549, iYPos + 342, UI_RENDER_NORMAL, TFO_BILINEAR );			
		}
		else
		{
			m_TodayRewardIcon->Render( iXPos + 549, iYPos + 342, UI_RENDER_NORMAL, TFO_BILINEAR );
			if( m_pTodaySubIcon )
				m_pTodaySubIcon->Render( iXPos + 549, iYPos + 342, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		if( m_pGetIcon && m_CurrTodayRewardState == ASS_STAMP )
		{
			m_pGetIcon->SetAlpha( m_CurrTodayBigAniTrack.m_fAlphaAniRate * 255 );
			m_pGetIcon->SetScale( m_CurrTodayBigAniTrack.m_fScaleAniRate, m_CurrTodayBigAniTrack.m_fScaleAniRate );
			m_pGetIcon->Render( iXPos + 500, iYPos + 344, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	else
	{
		if( m_NoneIcon )
			m_NoneIcon->Render( iXPos + 549, iYPos + 342, UI_RENDER_MULTIPLY, TFO_BILINEAR );			
	}

	if( m_bCurrTodayHidden && m_CurrTodayRewardState != ASS_STAMP )
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 548, iYPos + 389, FONT_SIZE_11, 108, "???" );
		g_FontMgr.PrintTextWidthCut( iXPos + 548, iYPos + 403, FONT_SIZE_11, 108, "???" );	
	}
	else
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 548, iYPos + 389, FONT_SIZE_11, 108, "%s", m_szTodayRewardText1.c_str() );
		g_FontMgr.PrintTextWidthCut( iXPos + 548, iYPos + 403, FONT_SIZE_11, 108, "%s", m_szTodayRewardText2.c_str() );	
	}

	if( m_AccrueRewardIcon )
	{
		if( m_pItemIconBG )
		{			
			m_pItemIconBG->SetScale( FLOAT1, FLOAT1 );
			m_pItemIconBG->Render( iXPos + 656, iYPos + 342, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}

		int iMax  = m_iCurrAccrueMaxTerm - m_iCurrAccrueMinTerm;
		int iCurr = m_iCurrAccrueTerm - m_iCurrAccrueMinTerm;

		OnRenderGaugeIcon( iXPos + 656, iYPos + 342, m_AccrueRewardIcon, m_AccrueRewardIcon->GetHeight(), iCurr, iMax, UI_RENDER_NORMAL );
		if( m_pAccrueSubIcon )
			m_pAccrueSubIcon->Render( iXPos + 656, iYPos + 342, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_pGetIcon && m_CurrAccureRewardState == ASS_STAMP )
		{
			m_pGetIcon->SetAlpha( m_CurrAccureBigAniTrack.m_fAlphaAniRate * 255 );
			m_pGetIcon->SetScale( m_CurrAccureBigAniTrack.m_fScaleAniRate, m_CurrAccureBigAniTrack.m_fScaleAniRate );
			m_pGetIcon->Render( iXPos + 608, iYPos + 344, UI_RENDER_NORMAL, TFO_BILINEAR );		
		}
	}
	else
	{
		if( m_NoneIcon )
			m_NoneIcon->Render( iXPos + 656, iYPos + 342, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	g_FontMgr.PrintTextWidthCut( iXPos + 656, iYPos + 389, FONT_SIZE_11, 108, "%s", m_szAccrueRewardText1.c_str() );
	g_FontMgr.PrintTextWidthCut( iXPos + 656, iYPos + 403, FONT_SIZE_11, 108, "%s", m_szAccrueRewardText2.c_str() );

	m_TodayGetEffect.Render( iXPos + 549, iYPos + 342, 2.0f, UI_RENDER_SCREEN, TFO_BILINEAR );
	m_AccrueGetEffect.Render( iXPos + 657, iYPos + 342, 2.0f, UI_RENDER_SCREEN, TFO_BILINEAR );
}

void AttendanceWnd::OnRenderGaugeIcon( int iXPos, int iYPos, ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax, UIRenderType eType )
{
	int iRealHeight = GetRenderImageStartHeight( pImage, iSrcHeight, iCur, iMax );

	pImage->Render( iXPos, iYPos, UI_RENDER_GRAY );
	if( eType == UI_RENDER_SCREEN )
	{
		pImage->Render( iXPos, iYPos, UI_RENDER_GRAY, TFO_NEAREST, false, UI_RENDER_SCREEN );
	}

	pImage->RenderHeightCut( iXPos, iYPos, iRealHeight, pImage->GetHeight(), UI_RENDER_NORMAL );
	if( eType == UI_RENDER_SCREEN )
	{
		pImage->RenderHeightCut( iXPos, iYPos, iRealHeight, pImage->GetHeight(), eType );
	}

	if( iRealHeight + 2 <= pImage->GetHeight() )
	{
		pImage->SetColor( 0, 0, 0 );
		pImage->RenderHeightCut( iXPos, iYPos, iRealHeight, iRealHeight + 2, UI_RENDER_NORMAL );
		pImage->SetColor( 255, 255, 255 );
	}
}

void AttendanceWnd::CheckAttendanceButton()
{
	DWORD dwCurrToday = g_MyInfo.GetAttendanceConnectTime();
	CalendarInfo& rkInfo = FindCalendarInfo( dwCurrToday );
	if ( rkInfo.IsEmpty() )
		return;
	ioAttendanceCheckButton* pButton = dynamic_cast<ioAttendanceCheckButton*>( FindChildWnd( ID_ATTENDANCE ) );
	ioAttendanceCheckedButton* pCheckedButton = dynamic_cast<ioAttendanceCheckedButton*>( FindChildWnd( ID_ATTENDANCED ) );
	if ( !pButton || !pCheckedButton )
		return;
	if( !rkInfo.m_bChecked )
	{
		if( !pButton->IsShow() )
		{
			pButton->SetStart();
			pButton->ShowWnd();
		}
		if ( pCheckedButton->IsShow() )
			pCheckedButton->HideWnd();
	}
	else
	{
		if( pButton->IsShow() )
		{
			pButton->SetEnd();
			pButton->HideWnd();
		}
		if ( !pCheckedButton->IsShow() )
			pCheckedButton->SetStart();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

ioAttendanceCheckButton::ioAttendanceCheckButton()
{	
	m_bOver   = false;
	m_bEnd    = false;
	m_dwOverTimer = FRAMEGETTIME();
}

ioAttendanceCheckButton::~ioAttendanceCheckButton()
{
}

void ioAttendanceCheckButton::SetEnd()
{
	m_bEnd  = true;
	m_bOver = false;
}

void ioAttendanceCheckButton::SetStart()
{
	m_bEnd  = false;
	m_bOver = false;
}

void ioAttendanceCheckButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_bEnd )
		return;

	ioButton::OnProcess( fTimePerSec );

	if( FRAMEGETTIME() - m_dwOverTimer > 500 )
	{
		m_dwOverTimer = FRAMEGETTIME();
		m_bOver = !m_bOver;
	}
}

void ioAttendanceCheckButton::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_bOver )
	{
		OnDrawOvered( iXPos, iYPos );
		PrintOverTitle( iXPos, iYPos );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

CalendarToolTip::CalendarToolTip()
{
	Clear();
}

CalendarToolTip::~CalendarToolTip()
{
	Clear();
}

void CalendarToolTip::OnProcess( float fTimePerSec )
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;
	
	CalendarToolTipInfo kInfo;
	AttendanceWnd *pWnd = dynamic_cast<AttendanceWnd*> ( g_GUIMgr.FindWnd( ATTENDANCE_WND ) );
	if( pWnd )
	{
		pWnd->GetCurCalendarInfo( kInfo );
	}

	if( IsShow() )
	{
		if( !SetInfo( kInfo.m_iPresentType, kInfo.m_iValue1, kInfo.m_iValue2, kInfo.m_bHidden ) )
		{
			HideWnd();
		}
		else
		{
			pMouse->SetToolTipWndID( 0 );
			pMouse->SetToolTipCursor();

			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );
			ProcessPosition();
		}
	}
	else
	{
		if( SetInfo( kInfo.m_iPresentType, kInfo.m_iValue1, kInfo.m_iValue2, kInfo.m_bHidden ) ) // setinfo 후에 showwnd
		{
			ShowWnd();

			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );
			ProcessPosition();
		}
	}
}

bool CalendarToolTip::SetInfo( int iPresentType, int iValue1, int iValue2, bool bHidden )
{
	m_szDescArray.ClearList();

	m_iPresentType = iPresentType;
	m_iValue1	   = iValue1;
	m_iValue2      = iValue2;

	if( m_iPresentType == 0 && m_iValue1 == 0 && m_iValue2 == 0 )
	{
		Clear();
		return false;
	}

	// desc
	m_szDescArray.SetTextStyle( TS_NORMAL );
	m_szDescArray.SetBkColor( 0, 0, 0 );	
	m_szDescArray.SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회색

	ioHashString szText1 = "???";
	ioHashString szText2 = "";
	if( !bHidden )
	{
		szText1 = g_PresentMgr.GetPresentValue1Text( iPresentType, iValue1, iValue2 );
		szText2 = g_PresentMgr.GetPresentValue2Text( iPresentType, iValue1, iValue2 );
	}

	if( !szText1.IsEmpty() )
		m_szDescArray.AddTextPiece( FONT_SIZE_12, "%s ", szText1.c_str() );

	if( !szText2.IsEmpty() )
		m_szDescArray.AddTextPiece( FONT_SIZE_12, "%s", szText2.c_str() );
	
	//
	int iMaxWidth  = m_szDescArray.GetFullWidth() + SIDE_TEXT_GAP;
	int iMaxHeight = MIN_HEIGHT;

	SetSize( iMaxWidth, iMaxHeight );

	return true;
}

void CalendarToolTip::iwm_show()
{
}

void CalendarToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;

	if( !m_szDescArray.IsEmpty() )
	{
		m_szDescArray.PrintFullText( iXPos, iYPos, TAT_LEFT );
	}
}

void CalendarToolTip::Clear()
{
	m_iPresentType = 0;
	m_iValue1	   = 0;
	m_iValue2      = 0;

	m_szDescArray.ClearList();
}

void CalendarToolTip::ProcessPosition()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		int iOffSetXY = 8;
		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
			iXPos -= ( GetWidth() + iOffSetXY );
		else 
			iXPos += iOffSetXY;

		if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
			iYPos -= ( GetHeight() + iOffSetXY );
		else 
			iYPos += iOffSetXY;

		if( GetParent() )
		{
			iXPos -= GetParent()->GetDerivedPosX();
			iYPos -= GetParent()->GetDerivedPosY();
		}

		SetWndPos( iXPos, iYPos );
		pMouse->SetToolTipCursor();
	}
}


//////////////////////////////////////////////////////////////////////////

ioAttendanceCheckedButton::ioAttendanceCheckedButton()
{
	m_bOver   = false;
	m_dwOverTimer = FRAMEGETTIME();
}

ioAttendanceCheckedButton::~ioAttendanceCheckedButton()
{

}

void ioAttendanceCheckedButton::SetStart()
{
	ShowWnd();
	m_bOver = false;
}

void ioAttendanceCheckedButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;


	ioButton::OnProcess( fTimePerSec );

	if( FRAMEGETTIME() - m_dwOverTimer > 500 )
	{
		m_dwOverTimer = FRAMEGETTIME();
		m_bOver = !m_bOver;
	}
}

void ioAttendanceCheckedButton::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	if( m_bOver )
	{
		OnDrawOvered( iXPos, iYPos );
		PrintOverTitle( iXPos, iYPos );
	}
}


//////////////////////////////////////////////////////////////////////////