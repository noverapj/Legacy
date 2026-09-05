#include "stdafx.h"

#include "../EtcHelpFunc.h"

#include "User.h"
#include "ioQuest.h"
#include "ioEtcItemManager.h"
#include "ioItemInfoManager.h"
#include "QuestVariety.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

#include <strsafe.h>

QuestParent::QuestParent()
{
	m_dwMainIndex = m_dwSubIndex = 0;
	m_dwPerformType = 0;

	m_bOneDayStyle = false;
	m_bRepeatStyle = false;
	m_bGuildStyle  = false;
	m_bPCRoomStyle = false;

	m_eChannelingType = CNT_NONE;

	m_dwOccurValue = m_dwCompleteValue = 0;

	m_iOccurModeType = m_iOccurRoomStyle = m_iCompleteModeType = m_iCompleteRoomStyle = -1;

	m_dwPeriodHour = 0;

	m_bRewardSelectStyle = false;
	m_iRewardSelectNum = 1;
	
	m_dwPrevMainIndex = m_dwPrevSubIndex = 0;

	m_dwNextMainIndex = m_dwNextSubIndex = 0;

	m_wStartYear = m_wStartMonth = m_wStartDate = m_wStartHour = 0;
	m_wEndYear = m_wEndMonth = m_wEndDate = m_wEndHour = 0;

	m_bCompleteGameAlarm = false;
	m_bCompleteWebAlarm  = false;

	m_bAlive = true;

	m_dwStartTime = 0;
	m_dwEndTime	= 0;
	m_bPossible = false;
}

QuestParent::~QuestParent()
{
	m_vRewardPresent.clear();
	m_CustomValue.clear();
}

DWORD QuestParent::GetRewardPresentIndex( int iArray )
{
	if( !COMPARE( iArray, 0, (int)m_vRewardPresent.size() ) ) return 0;

	return m_vRewardPresent[iArray];
}

bool QuestParent::lsCheckPreQuest(User *pUser)
{
	if( !pUser ) return false;

	if(m_dwPrevMainIndex == 0 && m_dwPrevSubIndex == 0)
	{
		return true;
	}

	ioQuest *pQuest = pUser->GetQuest();
	if(NULL == pQuest)
	{
		return false;
	}

	if( pQuest->IsQuestComplete(m_dwPrevMainIndex, m_dwPrevSubIndex))
	{
		return true;
	}

	return false;
}

bool QuestParent::lsCheckOccurTerm( User *pUser, int iValue1 /* = 0 */, int iValue2 /* = 0 */, int iValue3 /* = 0  */ )
{
	if( !pUser ) return false;

	return true;
}

bool QuestParent::lsCheckAttainTerm( User *pUser, QuestData kQuestData, int iValue1 /* = 0 */, int iValue2 /* = 0 */, int iValue3 /* = 0  */ )
{
	if( !pUser ) return false;

	return true;
}

bool QuestParent::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !pUser ) return false;
	if( !pQuestData ) return false;
	if( IsPCRoomStyle() )
	{
		if( !pUser->IsPCRoomAuthority() )
			return false;
	}
	return true;
}

int QuestParent::GetCustomValue( int iIdx )
{
	if( !COMPARE( iIdx, 0, (int)m_CustomValue.size() ) ) 
		return 0;

	return m_CustomValue[iIdx];
}

bool QuestParent::SetQuestData(ioHashString szClass, DWORD dwMainIndex, DWORD dwSubIndex, ioINILoader &rkLoader )
{
	m_szClass = szClass;
	m_dwMainIndex = dwMainIndex;
	m_dwSubIndex  = dwSubIndex;

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	sprintf_s( szKey, "sub%d_title", m_dwSubIndex );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_szTitle = szBuf;

	sprintf_s( szKey, "sub%d_perform_type", m_dwSubIndex );	
	m_dwPerformType = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_oneday_style", m_dwSubIndex );
	m_bOneDayStyle = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_oneday_cnt", m_dwSubIndex );
	m_iOneDayCount = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_repeat_style", m_dwSubIndex );
	m_bRepeatStyle = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_guild_style", m_dwSubIndex );
	m_bGuildStyle = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_pcroom_style", m_dwSubIndex );
	m_bPCRoomStyle = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_channeling_type", m_dwSubIndex );
	m_eChannelingType = (ChannelingType)rkLoader.LoadInt( szKey, CNT_NONE );

	sprintf_s( szKey, "sub%d_occur_value", m_dwSubIndex );	
	m_dwOccurValue = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_occur_mode", m_dwSubIndex );
	m_iOccurModeType = rkLoader.LoadInt( szKey, -1 );

	sprintf_s( szKey, "sub%d_occur_room_style", m_dwSubIndex );
	m_iOccurRoomStyle = rkLoader.LoadInt( szKey, -1 );

	sprintf_s( szKey, "sub%d_complete_value", m_dwSubIndex );	
	m_dwCompleteValue = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_complete_mode", m_dwSubIndex );
	m_iCompleteModeType = rkLoader.LoadInt( szKey, -1 );

	sprintf_s( szKey, "sub%d_complete_room_style", m_dwSubIndex );
	m_iCompleteRoomStyle = rkLoader.LoadInt( szKey, -1 );

	sprintf_s( szKey, "sub%d_period_hour", m_dwSubIndex );	
	m_dwPeriodHour = rkLoader.LoadInt( szKey, 0 );

	m_vRewardPresent.clear();
	sprintf_s( szKey, "sub%d_max_reward", m_dwSubIndex );
	int iMaxReward = rkLoader.LoadInt( szKey, 0 );
	int i = 0;
	for(;i < iMaxReward;i++)
	{
		sprintf_s( szKey, "sub%d_reward_present%d", m_dwSubIndex, i + 1 );
		m_vRewardPresent.push_back( rkLoader.LoadInt( szKey, 0 ) );
	}

	sprintf_s( szKey, "sub%d_rewardselect_style", m_dwSubIndex );
	m_bRewardSelectStyle = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_rewardselect_num", m_dwSubIndex );
	m_iRewardSelectNum = rkLoader.LoadInt( szKey, 1 );

	sprintf_s( szKey, "sub%d_prev_main_index", m_dwSubIndex );	
	m_dwPrevMainIndex = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_prev_sub_index", m_dwSubIndex );	
	m_dwPrevSubIndex = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_next_main_index", m_dwSubIndex );	
	m_dwNextMainIndex = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_next_sub_index", m_dwSubIndex );	
	m_dwNextSubIndex = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_complete_game_alarm", m_dwSubIndex );	
	m_bCompleteGameAlarm = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_complete_web_alarm", m_dwSubIndex );	
	m_bCompleteWebAlarm = rkLoader.LoadBool( szKey, false );

	m_CustomValue.clear();
	int iDefCustom = -999999999;
	for(i = 0;i < MAX_CUSTOM_VALUE;i++)
	{
		sprintf_s( szKey, "sub%d_custom_value%d", m_dwSubIndex, i + 1 );	
		int iCustomValue = rkLoader.LoadInt( szKey, iDefCustom );
		if( iCustomValue == iDefCustom ) 
			break;
		m_CustomValue.push_back( iCustomValue );
	}

	// 이벤트 퀘스트 시간 로드
	if( m_dwPerformType == QP_EVENT )
	{
		sprintf_s( szKey, "sub%d_start_year", m_dwSubIndex );			
		m_wStartYear  = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "sub%d_start_month", m_dwSubIndex );		
		m_wStartMonth = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "sub%d_start_date", m_dwSubIndex );
		m_wStartDate  = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "sub%d_start_hour", m_dwSubIndex );
		m_wStartHour  = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "sub%d_end_year", m_dwSubIndex );
		m_wEndYear    = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "sub%d_end_month", m_dwSubIndex );
		m_wEndMonth   = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "sub%d_end_date", m_dwSubIndex );
		m_wEndDate    = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "sub%d_end_hour", m_dwSubIndex );
		m_wEndHour    = rkLoader.LoadInt( szKey, 0 );

		m_bAlive = IsCheckAlive();
	}

	//퀘스트 관련 날짜 저장
	sprintf_s( szKey, "sub%d_start_year", m_dwSubIndex );			
	WORD wStartYear  = rkLoader.LoadInt( szKey, 0 );
	sprintf_s( szKey, "sub%d_start_month", m_dwSubIndex );		
	WORD wStartMonth = rkLoader.LoadInt( szKey, 0 );
	sprintf_s( szKey, "sub%d_start_date", m_dwSubIndex );
	WORD wStartDate  = rkLoader.LoadInt( szKey, 0 );
	sprintf_s( szKey, "sub%d_start_hour", m_dwSubIndex );
	WORD wStartHour  = rkLoader.LoadInt( szKey, 0 );
	sprintf_s( szKey, "sub%d_end_year", m_dwSubIndex );
	WORD wEndYear    = rkLoader.LoadInt( szKey, 0 );
	sprintf_s( szKey, "sub%d_end_month", m_dwSubIndex );
	WORD wEndMonth   = rkLoader.LoadInt( szKey, 0 );
	sprintf_s( szKey, "sub%d_end_date", m_dwSubIndex );
	WORD wEndDate    = rkLoader.LoadInt( szKey, 0 );
	sprintf_s( szKey, "sub%d_end_hour", m_dwSubIndex );
	WORD wEndHour    = rkLoader.LoadInt( szKey, 0 );

	m_dwStartTime	= Help::ConvertYYMMDDHHMMToDate(wStartYear, wStartMonth, wStartDate ,wStartHour,0);
	m_dwEndTime		= Help::ConvertYYMMDDHHMMToDate(wEndYear, wEndMonth, wEndDate ,wEndHour,0);
	return true;
}

bool QuestParent::AttainProcess( QuestData *pQuestData, int iCount )
{
	if( pQuestData->GetCurrentData() >= (DWORD)iCount )
	{
		// 달성 처리
		pQuestData->SetState( QS_ATTAIN );
		
		// 달성 완료 시간
		CTime cTime = CTime::GetCurrentTime();
		pQuestData->SetDate( cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );
		return true;
	}
	return false;
}

void QuestParent::ProcessRewardComplete( User *pUser )
{

}

bool QuestParent::IsCheckAlive()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	if( COMPARE( Help::ConvertYYMMDDHHMMToDate( st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute ), 
				 Help::ConvertYYMMDDHHMMToDate( m_wStartYear, m_wStartMonth, m_wStartDate, m_wStartHour, 0 ),
				 Help::ConvertYYMMDDHHMMToDate( m_wEndYear, m_wEndMonth, m_wEndDate, m_wEndHour, 0 ) ) )
	{
		return true;
	}
	return false;
}

DWORD QuestParent::GetStartDateData()
{
	QuestDataParent kDataParent;
	kDataParent.SetDate( m_wStartYear, m_wStartMonth, m_wStartDate, m_wStartHour, 0 );
	return kDataParent.GetDateData();
}

DWORD QuestParent::GetEndDateData()
{
	QuestDataParent kDataParent;
	kDataParent.SetDate( m_wEndYear, m_wEndMonth, m_wEndDate, m_wEndHour, 0 );
	return kDataParent.GetDateData();
}
//////////////////////////////////////////////////////////////////////////
bool QuestBasic::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestMonsterModeClear::QuestMonsterModeClear()
{
}

QuestMonsterModeClear::~QuestMonsterModeClear()
{

}

bool QuestMonsterModeClear::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CLEAR_COUNT ) );
}

//////////////////////////////////////////////////////////////////////////
QuestEnterBattlePvPMode::QuestEnterBattlePvPMode()
{
}

QuestEnterBattlePvPMode::~QuestEnterBattlePvPMode()
{

}

bool QuestEnterBattlePvPMode::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

//////////////////////////////////////////////////////////////////////////
QuestEnterBattlePvPModeKO::QuestEnterBattlePvPModeKO()
{
}

QuestEnterBattlePvPModeKO::~QuestEnterBattlePvPModeKO()
{

}

bool QuestEnterBattlePvPModeKO::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( KILL_COUNT ) );
}

//////////////////////////////////////////////////////////////////////////
QuestEnterBattlePvPModeWin::QuestEnterBattlePvPModeWin()
{
}

QuestEnterBattlePvPModeWin::~QuestEnterBattlePvPModeWin()
{

}

bool QuestEnterBattlePvPModeWin::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( WIN_COUNT ) );
}

//////////////////////////////////////////////////////////////////////////
QuestPvEMonsterKill::QuestPvEMonsterKill()
{
}

QuestPvEMonsterKill::~QuestPvEMonsterKill()
{

}

bool QuestPvEMonsterKill::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( KILL_COUNT ) );
}

//////////////////////////////////////////////////////////////////////////
QuestGradeUP::QuestGradeUP()
{
}

QuestGradeUP::~QuestGradeUP()
{
}

bool QuestGradeUP::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	if( !pUser->IsDeveloper() )
	{
		int iGradeLevel = pUser->GetGradeLevel();
		// 예외 처리
		if( (int)pQuestData->GetCurrentData() > iGradeLevel )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestGradeUP::IsCheckCompleteTerm : %s - Level Error : %d - %d", 
									pUser->GetPublicID().c_str(), iGradeLevel, pQuestData->GetCurrentData() );

			return false;
		}
	}

	return AttainProcess( pQuestData, GetCustomValue( COMPLETE_GRADE ) );
}

bool QuestGradeUP::IsCheckQuestCompeleteUser( User *pUser )
{
	// 계급 퀘스트 보상 받을때 한번더 체크한다
	if( GetCustomValue( QuestGradeUP::COMPLETE_GRADE ) > (pUser->GetGradeLevel() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestGradeUP Quest Insert Try. Grade Quest Cheat User: %s : %d - %d", pUser->GetPublicID().c_str(), m_dwMainIndex, m_dwSubIndex);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
QuestTimeGrowthTry::QuestTimeGrowthTry()
{
}

QuestTimeGrowthTry::~QuestTimeGrowthTry()
{
}

bool QuestTimeGrowthTry::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

//////////////////////////////////////////////////////////////////////////
QuestTimeGrowthSuccess::QuestTimeGrowthSuccess()
{
}

QuestTimeGrowthSuccess::~QuestTimeGrowthSuccess()
{
}

bool QuestTimeGrowthSuccess::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( GROWTH_UP_LEVEL ) );
}

//////////////////////////////////////////////////////////////////////////
QuestPesoGrowthTry::QuestPesoGrowthTry()
{
}

QuestPesoGrowthTry::~QuestPesoGrowthTry()
{
}

bool QuestPesoGrowthTry::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

//////////////////////////////////////////////////////////////////////////
QuestFishingTry::QuestFishingTry()
{
}

QuestFishingTry::~QuestFishingTry()
{

}

bool QuestFishingTry::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

//////////////////////////////////////////////////////////////////////////
QuestFishingSuccess::QuestFishingSuccess()
{
}

QuestFishingSuccess::~QuestFishingSuccess()
{

}

bool QuestFishingSuccess::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( FISHING_SUCCESS_COUNT ) );
}

//////////////////////////////////////////////////////////////////////////
QuestFishingFailed::QuestFishingFailed()
{
}

QuestFishingFailed::~QuestFishingFailed()
{

}

bool QuestFishingFailed::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( FISHING_FAIELD_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestFishingLevelUP::QuestFishingLevelUP()
{
}

QuestFishingLevelUP::~QuestFishingLevelUP()
{

}

bool QuestFishingLevelUP::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	if( !pUser->IsDeveloper() )
	{
		int iFishingLevel = pUser->GetFishingLevel();
		// 예외 처리
		if( (int)pQuestData->GetCurrentData() > iFishingLevel )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestFishingLevelUP::IsCheckCompleteTerm : %s - Level Error : %d - %d", 
									pUser->GetPublicID().c_str(), iFishingLevel, pQuestData->GetCurrentData() );
			pQuestData->SetCurrentData( iFishingLevel );
		}
	}

	return AttainProcess( pQuestData, GetCustomValue( CHECK_FISHING_LEVEL ) );
}

//////////////////////////////////////////////////////////////////////////
QuestFishingSellPeso::QuestFishingSellPeso()
{
}

QuestFishingSellPeso::~QuestFishingSellPeso()
{
}

bool QuestFishingSellPeso::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( COMPLETE_COUNT ) );
}

//////////////////////////////////////////////////////////////////////////
QuestFishingSuccessItem::QuestFishingSuccessItem()
{
}

QuestFishingSuccessItem::~QuestFishingSuccessItem()
{

}

bool QuestFishingSuccessItem::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( COMPLETE_COUNT ) );
}

//////////////////////////////////////////////////////////////////////////
QuestBattlePvPModeAwardAcquire::QuestBattlePvPModeAwardAcquire()
{
}

QuestBattlePvPModeAwardAcquire::~QuestBattlePvPModeAwardAcquire()
{

}

bool QuestBattlePvPModeAwardAcquire::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( AWARD_COUNT ) );
}

//////////////////////////////////////////////////////////////////////////
QuestSoldierPractice::QuestSoldierPractice()
{
}

QuestSoldierPractice::~QuestSoldierPractice()
{

}

bool QuestSoldierPractice::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

//////////////////////////////////////////////////////////////////////////
QuestExtraItemReinforceSuccess::QuestExtraItemReinforceSuccess()
{
}

QuestExtraItemReinforceSuccess::~QuestExtraItemReinforceSuccess()
{

}

bool QuestExtraItemReinforceSuccess::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( EXTRAITEM_REINFORCE_SUCCESS ) );
}

//////////////////////////////////////////////////////////////////////////
QuestSoldierLevelUP::QuestSoldierLevelUP()
{
}

QuestSoldierLevelUP::~QuestSoldierLevelUP()
{
}

bool QuestSoldierLevelUP::lsCheckOccurTerm(User *pUser, int iValue1, int iValue2, int iValue3)
{
	int iClassType = GetCustomValue( SOLDIER_CODE );
	
	bool bCheck =  pUser->IsCharClassType(iClassType);
	if(false == bCheck)
	{
		return false;
	}
	
	int iLevelUp = pUser->GetClassExpert()->GetClassLevel(iClassType);
	if( iLevelUp < GetCustomValue( SOLDIER_LIMIT_LEVEL ) )
		return false;

	return true;
}

bool QuestSoldierLevelUP::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	if( !pUser->IsDeveloper() )
	{
		int iClassType = GetCustomValue( SOLDIER_CODE );
		int iClassLevel= pUser->GetClassLevelByType( iClassType, true );
		// 예외 처리
		if( (int)pQuestData->GetCurrentData() > iClassLevel )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestSoldierLevelUP::IsCheckCompleteTerm : %s - Level Error : (%d)%d - %d", 
									pUser->GetPublicID().c_str(), iClassType, iClassLevel, pQuestData->GetCurrentData() );

			return false;
			//pQuestData->SetCurrentData( iClassLevel );
		}
	}

	return AttainProcess( pQuestData, GetCustomValue( SOLDIER_LEVEL ) );
}

bool QuestSoldierLevelUP::IsCheckQuestCompeleteUser( User* pUser )
{
	// 계급 퀘스트 보상 받을때 한번더 체크한다

	int iClassType = GetCustomValue( SOLDIER_CODE );
	int iTargetClassLevel = pUser->GetClassLevelByType( iClassType, true  );
	if( GetCustomValue( SOLDIER_LEVEL ) > iTargetClassLevel )
	{
		LOG.PrintTimeAndLog( 0, "QuestSoldierLevelUP Cheat User:%s, <%d, %d>", pUser->GetPublicID().c_str(), iClassType, iTargetClassLevel );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
QuestOpenManual::QuestOpenManual()
{
}

QuestOpenManual::~QuestOpenManual()
{
}

bool QuestOpenManual::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

//////////////////////////////////////////////////////////////////////////
QuestLoginCount::QuestLoginCount()
{
}

QuestLoginCount::~QuestLoginCount()
{
}

bool QuestLoginCount::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( LOGIN_COUNT ) );
}

DWORD QuestLoginCount::GetPresetMagicData()
{
	CTime cTime = CTime::GetCurrentTime();

	return ( ( cTime.GetYear() - QuestDataParent::DEFAULT_YEAR ) * 10000 ) + ( cTime.GetMonth() * 100 ) + cTime.GetDay();
}

//////////////////////////////////////////////////////////////////////////
QuestEnterPlaza::QuestEnterPlaza()
{
}

QuestEnterPlaza::~QuestEnterPlaza()
{

}

bool QuestEnterPlaza::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

//////////////////////////////////////////////////////////////////////////
QuestGetPotion::QuestGetPotion()
{
}

QuestGetPotion::~QuestGetPotion()
{

}

bool QuestGetPotion::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}

//////////////////////////////////////////////////////////////////////////
QuestTutorial::QuestTutorial()
{
}

QuestTutorial::~QuestTutorial()
{
}

bool QuestTutorial::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

//////////////////////////////////////////////////////////////////////////
QuestPresentReceive::QuestPresentReceive()
{

}

QuestPresentReceive::~QuestPresentReceive()
{

}

bool QuestPresentReceive::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( RECV_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestCampJoin::QuestCampJoin()
{
}

QuestCampJoin::~QuestCampJoin()
{

}

bool QuestCampJoin::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestEnterCampBattle::QuestEnterCampBattle()
{
}

QuestEnterCampBattle::~QuestEnterCampBattle()
{
}

bool QuestEnterCampBattle::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestCampBattleKO::QuestCampBattleKO()
{
}

QuestCampBattleKO::~QuestCampBattleKO()
{
}

bool QuestCampBattleKO::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( KILL_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestCampBattleWin::QuestCampBattleWin()
{
}

QuestCampBattleWin::~QuestCampBattleWin()
{
}

bool QuestCampBattleWin::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( WIN_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestCampSeasonReward::QuestCampSeasonReward()
{

}

QuestCampSeasonReward::~QuestCampSeasonReward()
{
}

bool QuestCampSeasonReward::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestAwardAcquire::QuestAwardAcquire()
{
}

QuestAwardAcquire::~QuestAwardAcquire()
{
}

bool QuestAwardAcquire::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestPrisonerDrop::QuestPrisonerDrop()
{
}

QuestPrisonerDrop::~QuestPrisonerDrop()
{
}

bool QuestPrisonerDrop::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestPrisonerSave::QuestPrisonerSave()
{
}

QuestPrisonerSave::~QuestPrisonerSave()
{
}

bool QuestPrisonerSave::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestBattleLevel::QuestBattleLevel()
{
}

QuestBattleLevel::~QuestBattleLevel()
{
}

bool QuestBattleLevel::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	if( !pUser->IsDeveloper() )
	{
		int iPartyLevel = pUser->GetPartyLevel();
		// 예외 처리
		if( (int)pQuestData->GetCurrentData() > iPartyLevel )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestBattleLevel::IsCheckCompleteTerm : %s - Level Error : %d - %d", 
									pUser->GetPublicID().c_str(), iPartyLevel, pQuestData->GetCurrentData() );
			pQuestData->SetCurrentData( iPartyLevel );
		}
	}

	return AttainProcess( pQuestData, GetCustomValue( CHECK_BATTLE_LEVEL ) );
}
//////////////////////////////////////////////////////////////////////////
QuestAwardLevel::QuestAwardLevel()
{
}

QuestAwardLevel::~QuestAwardLevel()
{
}

bool QuestAwardLevel::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	if( !pUser->IsDeveloper() )
	{
		int iAwardLevel = 0;
		if( pUser->GetAward() )
			iAwardLevel = pUser->GetAward()->GetAwardLevel();
		// 예외 처리
		if( (int)pQuestData->GetCurrentData() > iAwardLevel )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestAwardLevel::IsCheckCompleteTerm : %s - Level Error : %d - %d", 
									pUser->GetPublicID().c_str(), iAwardLevel, pQuestData->GetCurrentData() );
			pQuestData->SetCurrentData( iAwardLevel );
		}
	}

	return AttainProcess( pQuestData, GetCustomValue( CHECK_AWARD_LEVEL ) );
}
//////////////////////////////////////////////////////////////////////////
QuestContribute::QuestContribute()
{
}

QuestContribute::~QuestContribute()
{
}

bool QuestContribute::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestDropKill::QuestDropKill()
{
}

QuestDropKill::~QuestDropKill()
{
}

bool QuestDropKill::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( KILL_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestMultiKill::QuestMultiKill()
{
}

QuestMultiKill::~QuestMultiKill()
{
}

bool QuestMultiKill::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( KILL_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestPvPConsecution::QuestPvPConsecution()
{
}

QuestPvPConsecution::~QuestPvPConsecution()
{
}

bool QuestPvPConsecution::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CONSECUTION_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestCampConsecution::QuestCampConsecution()
{
}

QuestCampConsecution::~QuestCampConsecution()
{
}

bool QuestCampConsecution::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CONSECUTION_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestEtcItemUse::QuestEtcItemUse()
{
}

QuestEtcItemUse::~QuestEtcItemUse()
{
}

bool QuestEtcItemUse::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( USE_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestEtcItemCnt::QuestEtcItemCnt()
{
}

QuestEtcItemCnt::~QuestEtcItemCnt()
{
}

bool QuestEtcItemCnt::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}

void QuestEtcItemCnt::ProcessRewardComplete( User *pUser )
{
	if( !pUser ) return;
	if( GetCustomValue( BOOLEAN_ETC_ITEM_DELETE ) != 1 ) return;

	ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
	if( !pUserEtcItem ) return;

	int iDeleteCnt = GetCustomValue( CHECK_COUNT );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	pUserEtcItem->GetEtcItem( GetCustomValue( ETC_ITEM_CODE ), kSlot );
	
	if( !kSlot.IsUse() ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Quest Reward - Delete Etc Item None Use Type(%s/%d)", pUser->GetPublicID().c_str(), kSlot.m_iType );
		return;      // 사용 타입이 아니다.
	}

	kSlot.AddUse( -( min( kSlot.GetUse(), iDeleteCnt ) ) );
	if( kSlot.GetUse() > 0 )
	{	
		pUserEtcItem->SetEtcItem( kSlot );
	}
	else
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType, LogDBClient::ET_DEL );
	}
	pUser->SaveEtcItem();
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Quest Reward - Delete Etc Item (%s/%d/%d/%d)", pUser->GetPublicID().c_str(), kSlot.m_iType, kSlot.m_iValue1, iDeleteCnt );
}
//////////////////////////////////////////////////////////////////////////
QuestRequestFriend::QuestRequestFriend()
{
}

QuestRequestFriend::~QuestRequestFriend()
{
}

bool QuestRequestFriend::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestMakeFriends::QuestMakeFriends()
{
}

QuestMakeFriends::~QuestMakeFriends()
{
}

bool QuestMakeFriends::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( FRIEND_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestModePlayTime::QuestModePlayTime()
{
}

QuestModePlayTime::~QuestModePlayTime()
{
}

bool QuestModePlayTime::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_PLAY_MIN ) );
}

bool QuestModePlayTime::IsAbuseCheck( DWORD dwGapTime, int iGapValue )
{
	// 분단위로 전송함.
	// 1분을 50초로 계산하여 미리 오면 어뷰즈 발생
	int iExpectTime = Help::GetQuestAbuseSecond() * iGapValue;
	if( (int)dwGapTime < iExpectTime )
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////
QuestStoneAttack::QuestStoneAttack()
{
}

QuestStoneAttack::~QuestStoneAttack()
{
}

bool QuestStoneAttack::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestKingAttack::QuestKingAttack()
{
}

QuestKingAttack::~QuestKingAttack()
{
}

bool QuestKingAttack::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestCrownHoldTime::QuestCrownHoldTime()
{
}

QuestCrownHoldTime::~QuestCrownHoldTime()
{
}

bool QuestCrownHoldTime::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestBossModeBecomeBoss::QuestBossModeBecomeBoss()
{
}

QuestBossModeBecomeBoss::~QuestBossModeBecomeBoss()
{
}

bool QuestBossModeBecomeBoss::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestBossModeBosswithKill::QuestBossModeBosswithKill()
{
}

QuestBossModeBosswithKill::~QuestBossModeBosswithKill()
{
}

bool QuestBossModeBosswithKill::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestMortmainSoldierCount::QuestMortmainSoldierCount()
{
}

QuestMortmainSoldierCount::~QuestMortmainSoldierCount()
{
}

bool QuestMortmainSoldierCount::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestHitCount::QuestHitCount()
{
}

QuestHitCount::~QuestHitCount()
{
}

bool QuestHitCount::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestDefenceCount::QuestDefenceCount()
{
}

QuestDefenceCount::~QuestDefenceCount()
{
}

bool QuestDefenceCount::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestPickItem::QuestPickItem()
{
}

QuestPickItem::~QuestPickItem()
{
}

bool QuestPickItem::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestCampLevel::QuestCampLevel()
{
}

QuestCampLevel::~QuestCampLevel()
{
}

bool QuestCampLevel::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	if( !pUser->IsDeveloper() )
	{
		int iLadderLevel = pUser->GetLadderLevel();
		// 예외 처리
		if( (int)pQuestData->GetCurrentData() > iLadderLevel )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestCampLevel::IsCheckCompleteTerm : %s - Level Error : %d - %d", 
									pUser->GetPublicID().c_str(), iLadderLevel, pQuestData->GetCurrentData() );
			pQuestData->SetCurrentData( iLadderLevel );
		}
	}

	return AttainProcess( pQuestData, GetCustomValue( CHECK_CAMP_LEVEL ) );
}
//////////////////////////////////////////////////////////////////////////
QuestBuyItem::QuestBuyItem()
{
}

QuestBuyItem::~QuestBuyItem()
{
}

bool QuestBuyItem::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestMaxFriendSlot::QuestMaxFriendSlot()
{
}

QuestMaxFriendSlot::~QuestMaxFriendSlot()
{
}

bool QuestMaxFriendSlot::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestRealTimeCount::QuestRealTimeCount()
{
}

QuestRealTimeCount::~QuestRealTimeCount()
{
}

bool QuestRealTimeCount::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}

bool QuestRealTimeCount::IsAbuseCheck( DWORD dwGapTime, int iGapValue )
{
	// 초단위로 전송함.
	// 1분을 50초로 계산하여 미리 오면 어뷰즈 발생
	int iExpectTime = (float)Help::GetQuestAbuseSecond() * ( (float)iGapValue / 60 );
	if( (int)dwGapTime < iExpectTime )
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
QuestPlayTimeCount::QuestPlayTimeCount()
{
}

QuestPlayTimeCount::~QuestPlayTimeCount()
{
}

bool QuestPlayTimeCount::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}

bool QuestPlayTimeCount::IsAbuseCheck( DWORD dwGapTime, int iGapValue )
{
	// 초단위로 전송함.
	// 1분을 50초로 계산하여 미리 오면 어뷰즈 발생
	int iExpectTime = (float)Help::GetQuestAbuseSecond() * ( (float)iGapValue / 60 );
	if( (int)dwGapTime < iExpectTime )
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
QuestFriendRecommendPlayTime::QuestFriendRecommendPlayTime()
{
}

QuestFriendRecommendPlayTime::~QuestFriendRecommendPlayTime()
{
}

bool QuestFriendRecommendPlayTime::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}

bool QuestFriendRecommendPlayTime::IsAbuseCheck( DWORD dwGapTime, int iGapValue )
{
	// 초단위로 전송함.
	// 1분을 50초로 계산하여 미리 오면 어뷰즈 발생
	int iExpectTime = (float)Help::GetQuestAbuseSecond() * ( (float)iGapValue / 60 );
	if( (int)dwGapTime < iExpectTime )
		return true;
	return false;
}

void QuestFriendRecommendPlayTime::ProcessRewardComplete( User *pUser )
{
	if( !pUser ) return;
	
	DWORD dwTableIndex = pUser->GetFriendRecommendTableIndex();
	if( dwTableIndex == 0 )
	{
		// 버그
		char szLog[2048] = "";
		sprintf_s( szLog, "FriendRecommend Table Error : %s", pUser->GetPublicID().c_str() );
		SP2Packet kPacket2( LUPK_LOG );
		kPacket2 << "ServerError";
		kPacket2 << szLog;
		g_UDPNode.SendLog( kPacket2 );
	}
	else
	{
		g_DBClient.OnUpdateFriendRecommendData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dwTableIndex );
	}
}
//////////////////////////////////////////////////////////////////////////
QuestQuickStartOption::QuestQuickStartOption()
{
}

QuestQuickStartOption::~QuestQuickStartOption()
{
}

bool QuestQuickStartOption::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestBuySoldier::QuestBuySoldier()
{
}

QuestBuySoldier::~QuestBuySoldier()
{
}

bool QuestBuySoldier::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}

void QuestBuySoldier::ProcessRewardComplete( User *pUser )
{
	if( !pUser ) return;

	int iMaxSoldier = GetCustomValue( MAX_SOLDIER );
	for(int i = 0;i < iMaxSoldier;i++)
	{
		int iClassType = GetCustomValue( SOLDIER_CODE + i );
		if( pUser->IsCharPeriodTime( iClassType ) )
		{
			g_ItemPriceMgr.SetQuestSoldierCollected( iClassType );
		}
	}		 
}
//////////////////////////////////////////////////////////////////////////
QuestPvETimeAttack::QuestPvETimeAttack()
{
}

QuestPvETimeAttack::~QuestPvETimeAttack()
{
}

bool QuestPvETimeAttack::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestSoccerBallHit::QuestSoccerBallHit()
{
}

QuestSoccerBallHit::~QuestSoccerBallHit()
{
}

bool QuestSoccerBallHit::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestSoccerGoal::QuestSoccerGoal()
{
}

QuestSoccerGoal::~QuestSoccerGoal()
{
}

bool QuestSoccerGoal::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestSoccerAssist::QuestSoccerAssist()
{
}

QuestSoccerAssist::~QuestSoccerAssist()
{
}

bool QuestSoccerAssist::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestSoccerSave::QuestSoccerSave()
{
}

QuestSoccerSave::~QuestSoccerSave()
{
}

bool QuestSoccerSave::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestExcavationTry::QuestExcavationTry()
{
}

QuestExcavationTry::~QuestExcavationTry()
{
}

bool QuestExcavationTry::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestExcavationLevelUP::QuestExcavationLevelUP()
{
}

QuestExcavationLevelUP::~QuestExcavationLevelUP()
{
}

bool QuestExcavationLevelUP::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	if( !pUser->IsDeveloper() )
	{
		int iExcavationLevel = pUser->GetExcavationLevel();
		// 예외 처리
		if( (int)pQuestData->GetCurrentData() > iExcavationLevel )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestExcavationLevelUP::IsCheckCompleteTerm : %s - Level Error : %d - %d", 
									pUser->GetPublicID().c_str(), iExcavationLevel, pQuestData->GetCurrentData() );
			pQuestData->SetCurrentData( iExcavationLevel );
		}
	}

	return AttainProcess( pQuestData, GetCustomValue( CHECK_EXCAVATION_LEVEL ) );
}
//////////////////////////////////////////////////////////////////////////
QuestExcavationSuccess::QuestExcavationSuccess()
{
}

QuestExcavationSuccess::~QuestExcavationSuccess()
{
}

bool QuestExcavationSuccess::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestExcavationFail::QuestExcavationFail()
{
}

QuestExcavationFail::~QuestExcavationFail()
{
}

bool QuestExcavationFail::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestExcavationTime::QuestExcavationTime()
{
}

QuestExcavationTime::~QuestExcavationTime()
{
}

bool QuestExcavationTime::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}

bool QuestExcavationTime::IsAbuseCheck( DWORD dwGapTime, int iGapValue )
{
	// 초단위로 전송함.
	// 1분을 50초로 계산하여 미리 오면 어뷰즈 발생
	int iExpectTime = (float)Help::GetQuestAbuseSecond() * ( (float)iGapValue / 60 );
	if( (int)dwGapTime < iExpectTime )
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
QuestScreenShot::QuestScreenShot()
{
}

QuestScreenShot::~QuestScreenShot()
{
}

bool QuestScreenShot::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestMovieMode::QuestMovieMode()
{
}

QuestMovieMode::~QuestMovieMode()
{
}

bool QuestMovieMode::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestMakeMovie::QuestMakeMovie()
{
}

QuestMakeMovie::~QuestMakeMovie()
{
}

bool QuestMakeMovie::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestExtraItemAcquire::QuestExtraItemAcquire()
{
}

QuestExtraItemAcquire::~QuestExtraItemAcquire()
{
}

bool QuestExtraItemAcquire::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestExtraItemEquip::QuestExtraItemEquip()
{
}

QuestExtraItemEquip::~QuestExtraItemEquip()
{
}

bool QuestExtraItemEquip::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestExtraItemEquipKo::QuestExtraItemEquipKo()
{
}

QuestExtraItemEquipKo::~QuestExtraItemEquipKo()
{
}

bool QuestExtraItemEquipKo::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT));		// 2019-02-19 by bckim, 퀘스트 오류 수정 
}
//////////////////////////////////////////////////////////////////////////
QuestGangsiKill::QuestGangsiKill()
{
}

QuestGangsiKill::~QuestGangsiKill()
{
}

bool QuestGangsiKill::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestGangsiHumanKill::QuestGangsiHumanKill()
{
}

QuestGangsiHumanKill::~QuestGangsiHumanKill()
{
}

bool QuestGangsiHumanKill::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestGangsiAliveTime::QuestGangsiAliveTime()
{
}

QuestGangsiAliveTime::~QuestGangsiAliveTime()
{
}

bool QuestGangsiAliveTime::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestGangsiHumanWin::QuestGangsiHumanWin()
{
}

QuestGangsiHumanWin::~QuestGangsiHumanWin()
{
}

bool QuestGangsiHumanWin::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestHitAttackAttribute::QuestHitAttackAttribute()
{
}

QuestHitAttackAttribute::~QuestHitAttackAttribute()
{
}

bool QuestHitAttackAttribute::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestGuildLevel::QuestGuildLevel()
{
}

QuestGuildLevel::~QuestGuildLevel()
{
}

bool QuestGuildLevel::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestGuildLevelMaintenance::QuestGuildLevelMaintenance()
{
}

QuestGuildLevelMaintenance::~QuestGuildLevelMaintenance()
{
}

bool QuestGuildLevelMaintenance::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestGuildTeamWin::QuestGuildTeamWin()
{
}

QuestGuildTeamWin::~QuestGuildTeamWin()
{
}

bool QuestGuildTeamWin::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( WIN_COUNT ) );
}
//////////////////////////////////////////////////////////////////////////
QuestGuildTeamPlayTime::QuestGuildTeamPlayTime()
{
}

QuestGuildTeamPlayTime::~QuestGuildTeamPlayTime()
{
}

bool QuestGuildTeamPlayTime::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData, GetCustomValue( CHECK_PLAY_MIN ) );
}

bool QuestGuildTeamPlayTime::IsAbuseCheck( DWORD dwGapTime, int iGapValue )
{
	// 분단위로 전송함.
	// 1분을 50초로 계산하여 미리 오면 어뷰즈 발생
	int iExpectTime = Help::GetQuestAbuseSecond() * iGapValue;
	if( (int)dwGapTime < iExpectTime )
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
QuestPlayTimeRepeat::QuestPlayTimeRepeat()
{
}

QuestPlayTimeRepeat::~QuestPlayTimeRepeat()
{
}

bool QuestPlayTimeRepeat::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;
	
	int iRepeatMinute = GetCustomValue( REPEAT_TIME );
	// 퀘스트가 완료처리되지 않고 반복되는 퀘스트이므로 완료되면 상품 지급한다. 달성 처리는 하지 않는다.
	if( pQuestData->GetCurrentData() >= (DWORD)iRepeatMinute )
	{
		/* 무결성 체크 - 스피드핵으로 클라이언트에서 시간을 빨리 돌리면 안되므로 시간을 따로 체크한다
		   2분정도는 봐준다. */
		CTime cCurTime = CTime::GetCurrentTime();
		CTimeSpan cGapTime( 0, 0, iRepeatMinute - 2, 0 );
		CTime cPrevTime( Help::GetSafeValueForCTimeConstructor( pQuestData->GetYear(), pQuestData->GetMonth(), pQuestData->GetDay(), pQuestData->GetHour(),
																pQuestData->GetMinute(), 0 ) );
		cPrevTime += cGapTime;
		if( cCurTime >= cPrevTime )
		{		
			// 달성 시간 기록하고 카운트 초기화		
			pQuestData->SetDate( cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), cCurTime.GetHour(), cCurTime.GetMinute() );
			pQuestData->SetCurrentData( 0 );

			SP2Packet kPacket( STPK_QUEST_REPEAT_REWARD );
			kPacket << GetMainIndex() << GetSubIndex();
			kPacket << GetMaxRewardPresent();
			// 반복 보상 지급
			bool bPresent = false;
			for(int k = 0;k < GetMaxRewardPresent();k++)
			{
				bool bDirect = g_QuestMgr.SendRewardPresent( pUser, GetRewardPresentIndex( k ) );
				kPacket << bDirect;				
				if( !bDirect )
					bPresent = true;
			}
			kPacket << pUser->GetMoney() << pUser->GetGradeLevel() << pUser->GetGradeExpert();		
			pUser->SendMessage( kPacket );

			if( bPresent )
			{
				 pUser->SendPresentMemory();
				//pUser->_OnSelectPresent( 30 );     // 메모리에만 저장하니까 요청하지 않는다. 2011.04.12 LJH
			}
			// 로그 DB에 기록
			g_LogDBClient.OnInsertQuest( pUser, GetMainIndex(), GetSubIndex(), LogDBClient::QT_REPEAT_REWARD );
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "QuestPlayTimeRepeat Abuse!!: %s - %d:%d:%d:%d != %d:%d:%d:%d", pUser->GetPublicID().c_str(),
									cCurTime.GetMonth(), cCurTime.GetDay(), cCurTime.GetHour(), cCurTime.GetMinute(),
									cPrevTime.GetMonth(), cPrevTime.GetDay(), cPrevTime.GetHour(), cPrevTime.GetMinute() );
		}
	}
	return false;
}

bool QuestPlayTimeRepeat::IsAbuseCheck( DWORD dwGapTime, int iGapValue )
{
	// 분단위로 전송함.
	// 1분을 50초로 계산하여 미리 오면 어뷰즈 발생
	int iExpectTime = Help::GetQuestAbuseSecond() * iGapValue;
	if( (int)dwGapTime < iExpectTime )
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
QuestDormantUser::QuestDormantUser()
{
}

QuestDormantUser::~QuestDormantUser()
{
}

bool QuestDormantUser::lsCheckOccurTerm( User *pUser, int iValue1, int iValue2, int iValue3 )
{
	if( !pUser ) return false;

	// 최종 접속일로부터 현재 접속까지의 시간(DAY)
	CTime cCurTime = CTime::GetCurrentTime();
	// 최종 접속일로부터 현재 접속까지의 시간(Sec)
	CTime cLogOutTime2( Help::GetSafeValueForCTimeConstructor( pUser->GetLastLogOutTime().GetYear(), pUser->GetLastLogOutTime().GetMonth(),
																pUser->GetLastLogOutTime().GetDay(), pUser->GetLastLogOutTime().GetHour(), pUser->GetLastLogOutTime().GetMinute(), pUser->GetLastLogOutTime().GetSecond() ) );
	CTime cLogInTime2( Help::GetSafeValueForCTimeConstructor( cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), cCurTime.GetHour(), cCurTime.GetMinute(), cCurTime.GetSecond() ) ); 
	CTimeSpan cGapTime2 = cLogInTime2 - cLogOutTime2;

	int iTotalDays    = (int)cGapTime2.GetDays();


	if( iTotalDays < GetCustomValue( LIMIT_DAY ) )
	{
		return false;
	}

	return true;
}

bool QuestDormantUser::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

bool QuestDormantUser::IsCheckQuestCompeleteUser( User *pUser )
{ 
	pUser->SetLastLogOutTime(CTime::GetCurrentTime());
	return false;
}

//////////////////////////////////////////////////////////////////////////
QuestDormantUserCustom::QuestDormantUserCustom()
{
}

QuestDormantUserCustom::~QuestDormantUserCustom()
{
}

bool QuestDormantUserCustom::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
//////////////////////////////////////////////////////////////////////////
QuestDevKMove::QuestDevKMove()
{
}

QuestDevKMove::~QuestDevKMove()
{
}

bool QuestDevKMove::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}

/////////////////////////////////////////////////////////////////////////
QuestPracticeSuccess::QuestPracticeSuccess()
{

}

QuestPracticeSuccess::~QuestPracticeSuccess()
{

}

bool QuestPracticeSuccess::lsCheckAttainTerm( User *pUser, QuestData kQuestData, int iValue1 /* = 0 */, int iValue2 /* = 0 */, int iValue3 /* = 0  */ )
{
	if( !pUser ) return false;

	if( GetMainIndex() == 0 )
	{
		return false;
	}

	// 이미 획득한 퀘인지 확인
	if( pUser->GetQuest()->IsQuestAttainCheck( GetMainIndex(), GetSubIndex() ))
	{
		return false;
	}

	if( kQuestData.GetState() == QS_COMPLETE && !pUser->IsDeveloper() )
	{
		return false;
	}

	if( GetCustomValue(LIMIT_GRADE) > iValue1)
	{
		return false;
	}

	if( GetCustomValue(PRACTICE_NUMB) != iValue2)
	{
		return false;
	}

	if( GetCustomValue(STARCOUNT) > iValue3)
	{
		return false;
	}

	QuestData kResultData = pUser->GetQuest()->SetQuestCurrentData( kQuestData ); //어뷰즈 체크

	SP2Packet kQuest_Packet( STPK_QUEST_ATTAIN );
	kQuest_Packet << QUEST_ATTAIN_OK << 1 << kResultData.GetIndexData() << kResultData.GetValueData() << kResultData.GetMagicData() << kResultData.GetDateData();
	pUser->SendMessage( kQuest_Packet );

	if( kResultData.GetState() == QS_ATTAIN )
	{
		// 로그 DB에 기록
		g_LogDBClient.OnInsertQuest( pUser, kResultData.GetMainIndex(), kResultData.GetSubIndex(), LogDBClient::QT_ATTAIN );
	}

	return true;
}

bool QuestPracticeSuccess::IsCheckCompleteTerm( User *pUser, QuestData *pQuestData )
{
	if( !QuestParent::IsCheckCompleteTerm( pUser, pQuestData ) ) 
		return false;

	return AttainProcess( pQuestData );
}
