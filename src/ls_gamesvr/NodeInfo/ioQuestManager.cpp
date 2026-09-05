#include "stdafx.h"

#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../EtcHelpFunc.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "UserNodeManager.h"
#include "ioQuestManager.h"
#include <strsafe.h>
#include "../MainServerNode/MainServerNode.h"

template<> ioQuestManager* Singleton< ioQuestManager >::ms_Singleton = 0;
ioQuestManager::ioQuestManager()
{
	m_dwCurrentTime = 0;
	
	m_dwOneDayQuestHour = 0;
	m_dwNextOneDayQuestDate = 0;
}

ioQuestManager::~ioQuestManager()
{
	ClearQuestVariety();

	m_QuestRewardMap.clear();
}

ioQuestManager& ioQuestManager::GetSingleton()
{
	return Singleton< ioQuestManager >::GetSingleton();
}

void ioQuestManager::ClearQuestVariety()
{
	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		SAFEDELETE( *iter );
	}
	m_QuestVariety.clear();
	m_QuestRewardMap.clear();
}

QuestParent *ioQuestManager::CreateQuest( const ioHashString &rClassName )
{
	if( rClassName == "QuestBasic" )
		return new QuestBasic;
	else if( rClassName == "QuestMonsterModeClear" )
		return new QuestMonsterModeClear;
	else if( rClassName == "QuestEnterBattlePvPMode" )
		return new QuestEnterBattlePvPMode;
	else if( rClassName == "QuestEnterBattlePvPModeKO" )
		return new QuestEnterBattlePvPModeKO;
	else if( rClassName == "QuestEnterBattlePvPModeWin" )
		return new QuestEnterBattlePvPModeWin;
	else if( rClassName == "QuestPvEMonsterKill" )
		return new QuestPvEMonsterKill;
	else if( rClassName == "QuestGradeUP" )
		return new QuestGradeUP;
	else if( rClassName == "QuestTimeGrowthTry" )
		return new QuestTimeGrowthTry;
	else if( rClassName == "QuestTimeGrowthSuccess" )
		return new QuestTimeGrowthSuccess;
	else if( rClassName == "QuestPesoGrowthTry" )
		return new QuestPesoGrowthTry;
	else if( rClassName == "QuestFishingTry" )
		return new QuestFishingTry;
	else if( rClassName == "QuestFishingSuccess" )
		return new QuestFishingSuccess;
	else if( rClassName == "QuestFishingFailed" )
		return new QuestFishingFailed;
	else if( rClassName == "QuestFishingLevelUP" )
		return new QuestFishingLevelUP;
	else if( rClassName == "QuestFishingSellPeso" )
		return new QuestFishingSellPeso;
	else if( rClassName == "QuestFishingSuccessItem" )
		return new QuestFishingSuccessItem;
	else if( rClassName == "QuestBattlePvPModeAwardAcquire" )
		return new QuestBattlePvPModeAwardAcquire;
	else if( rClassName == "QuestSoldierPractice" )
		return new QuestSoldierPractice;
	else if( rClassName == "QuestExtraItemReinforceSuccess" )
		return new QuestExtraItemReinforceSuccess;
	else if( rClassName == "QuestSoldierLevelUP" )
		return new QuestSoldierLevelUP;
	else if( rClassName == "QuestOpenManual" )
		return new QuestOpenManual;
	else if( rClassName == "QuestLoginCount" )
		return new QuestLoginCount;
	else if( rClassName == "QuestEnterPlaza" )
		return new QuestEnterPlaza;
	else if( rClassName == "QuestGetPotion" )
		return new QuestGetPotion;
	else if( rClassName == "QuestTutorial" )
		return new QuestTutorial;
	else if( rClassName == "QuestPresentReceive" )
		return new QuestPresentReceive;
	else if( rClassName == "QuestCampJoin" )
		return new QuestCampJoin;
	else if( rClassName == "QuestEnterCampBattle" )
		return new QuestEnterCampBattle;
	else if( rClassName == "QuestCampBattleKO" )
		return new QuestCampBattleKO;
	else if( rClassName == "QuestCampBattleWin" )
		return new QuestCampBattleWin;
	else if( rClassName == "QuestCampSeasonReward" )
		return new QuestCampSeasonReward;
	else if( rClassName == "QuestAwardAcquire" )
		return new QuestAwardAcquire;
	else if( rClassName == "QuestPrisonerDrop" )
		return new QuestPrisonerDrop;
	else if( rClassName == "QuestPrisonerSave" )
		return new QuestPrisonerSave;
	else if( rClassName == "QuestBattleLevel" )
		return new QuestBattleLevel;
	else if( rClassName == "QuestAwardLevel" )
		return new QuestAwardLevel;
	else if( rClassName == "QuestContribute" )
		return new QuestContribute;
	else if( rClassName == "QuestDropKill" )
		return new QuestDropKill;
	else if( rClassName == "QuestMultiKill" )
		return new QuestMultiKill;
	else if( rClassName == "QuestPvPConsecution" )
		return new QuestPvPConsecution;
	else if( rClassName == "QuestCampConsecution" )
		return new QuestCampConsecution;
	else if( rClassName == "QuestEtcItemUse" )
		return new QuestEtcItemUse;
	else if( rClassName == "QuestEtcItemCnt" )
		return new QuestEtcItemCnt;
	else if( rClassName == "QuestRequestFriend" )
		return new QuestRequestFriend;
	else if( rClassName == "QuestMakeFriends" )
		return new QuestMakeFriends;
	else if( rClassName == "QuestModePlayTime" )
		return new QuestModePlayTime;
	else if( rClassName == "QuestStoneAttack" )
		return new QuestStoneAttack;
	else if( rClassName == "QuestKingAttack" )
		return new QuestKingAttack;
	else if( rClassName == "QuestCrownHoldTime" )
		return new QuestCrownHoldTime;
	else if( rClassName == "QuestBossModeBecomeBoss" )
		return new QuestBossModeBecomeBoss;
	else if( rClassName == "QuestBossModeBosswithKill" )
		return new QuestBossModeBosswithKill;
	else if( rClassName == "QuestMortmainSoldierCount" )
		return new QuestMortmainSoldierCount;
	else if( rClassName == "QuestHitCount" )
		return new QuestHitCount;
	else if( rClassName == "QuestDefenceCount" )
		return new QuestDefenceCount;
	else if( rClassName == "QuestPickItem" )
		return new QuestPickItem;
	else if( rClassName == "QuestCampLevel" )
		return new QuestCampLevel;
	else if( rClassName == "QuestBuyItem" )
		return new QuestBuyItem;
	else if( rClassName == "QuestMaxFriendSlot" )
		return new QuestMaxFriendSlot;
	else if( rClassName == "QuestRealTimeCount" )
		return new QuestRealTimeCount;
	else if( rClassName == "QuestPlayTimeCount" )
		return new QuestPlayTimeCount;
	else if( rClassName == "QuestFriendRecommendPlayTime" )
		return new QuestFriendRecommendPlayTime;
	else if( rClassName == "QuestQuickStartOption" )
		return new QuestQuickStartOption;
	else if( rClassName == "QuestBuySoldier" )
		return new QuestBuySoldier;
	else if( rClassName == "QuestPvETimeAttack" )
		return new QuestPvETimeAttack;
	else if( rClassName == "QuestSoccerBallHit" )
		return new QuestSoccerBallHit;
	else if( rClassName == "QuestSoccerGoal" )
		return new QuestSoccerGoal;
	else if( rClassName == "QuestSoccerAssist" )
		return new QuestSoccerAssist;
	else if( rClassName == "QuestSoccerSave" )
		return new QuestSoccerSave;
	else if( rClassName == "QuestExcavationTry" )
		return new QuestExcavationTry;
	else if( rClassName == "QuestExcavationLevelUP" )
		return new QuestExcavationLevelUP;
	else if( rClassName == "QuestExcavationSuccess" )
		return new QuestExcavationSuccess;
	else if( rClassName == "QuestExcavationFail" )
		return new QuestExcavationFail;
	else if( rClassName == "QuestExcavationTime" )
		return new QuestExcavationTime;
	else if( rClassName == "QuestScreenShot" )
		return new QuestScreenShot;
	else if( rClassName == "QuestMovieMode" )
		return new QuestMovieMode;
	else if( rClassName == "QuestMakeMovie" )
		return new QuestMakeMovie;
	else if( rClassName == "QuestExtraItemAcquire" )
		return new QuestExtraItemAcquire;
	else if( rClassName == "QuestExtraItemEquip" )
		return new QuestExtraItemEquip;
	else if( rClassName == "QuestExtraItemEquipKo" )
		return new QuestExtraItemEquipKo;
	else if( rClassName == "QuestGangsiKill" )
		return new QuestGangsiKill;
	else if( rClassName == "QuestGangsiHumanKill" )
		return new QuestGangsiHumanKill;
	else if( rClassName == "QuestGangsiAliveTime" )
		return new QuestGangsiAliveTime;
	else if( rClassName == "QuestGangsiHumanWin" )
		return new QuestGangsiHumanWin;
	else if( rClassName == "QuestHitAttackAttribute" )
		return new QuestHitAttackAttribute;
	else if( rClassName == "QuestGuildLevel" )
		return new QuestGuildLevel;
	else if( rClassName == "QuestGuildLevelMaintenance" )
		return new QuestGuildLevelMaintenance;
	else if( rClassName == "QuestGuildTeamWin" )
		return new QuestGuildTeamWin;
	else if( rClassName == "QuestGuildTeamPlayTime" )
		return new QuestGuildTeamPlayTime;
	else if( rClassName == "QuestPlayTimeRepeat" )
		return new QuestPlayTimeRepeat;
	else if( rClassName == "QuestDormantUser" )
		return new QuestDormantUser;
	else if( rClassName == "QuestDormantUserCustom" )
		return new QuestDormantUserCustom;
	else if( rClassName == "QuestDevKMove" )
		return new QuestDevKMove;
	else if( rClassName == "QuestPracticeSuccess" )
		return new QuestPracticeSuccess;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuestManager::CreateQuest None Class : %s", rClassName.c_str() );
	return NULL;
}

void ioQuestManager::SetQuest(char* csClassName, DWORD dwMainIndex, DWORD dwSubIndex, ioINILoader &rkLoader, bool bCreate, DWORD dwChangeSubIndex, DWORD dwChangeNextSubIndex)
{
	if ( csClassName == NULL )
		return;

	QuestParent *pQuestData = NULL;
	//이럴 경우는 시스템에서 강제로 SubIndex를 생성하는 경우이기 때문에 무조건 Create한다.
	if ( bCreate == true )
	{
		pQuestData = CreateQuest( csClassName );
	}
	else
	{
		pQuestData = GetQuest( dwMainIndex, dwSubIndex );
	}

	if ( pQuestData == NULL )
	{
		pQuestData = CreateQuest( csClassName );
		if ( pQuestData == NULL ) return;
		pQuestData->SetQuestData( csClassName, dwMainIndex, dwSubIndex, rkLoader );
		bCreate = true;
	}
	else
	{
		pQuestData->SetQuestData( csClassName, dwMainIndex, dwSubIndex, rkLoader );
	}

	if ( dwChangeSubIndex != 0 )
	{
		pQuestData->ChangeSubItemIndex( dwChangeSubIndex );
	}

	if ( dwChangeNextSubIndex != 0 )
	{
		pQuestData->ChangeNextIndex( dwMainIndex, dwChangeNextSubIndex);
	}

	if ( bCreate == true )
		m_QuestVariety.push_back( pQuestData );
}

void ioQuestManager::LoadINIData()
{
	{
		ioINILoader kLoader;
		kLoader.ReloadFile( "config/sp2_one_day_quest.ini" );

		kLoader.SetTitle( "common" );

		m_dwOneDayQuestHour     = kLoader.LoadInt( "oneday_hour", 4 );
		m_dwNextOneDayQuestDate = kLoader.LoadInt( "oneday_current", 0 );
	}

	{
		ioINILoader kLoader;
		kLoader.ReloadFile( "config/sp2_quest_info.ini" );

		kLoader.SetTitle( "common" );

		int iMaxQuest = kLoader.LoadInt( "max_quest", 0 );
		for(int i = 0;i < iMaxQuest;i++)
		{
			char szTitle[MAX_PATH] = "";
			sprintf_s( szTitle, "quest%d", i + 1 );
			kLoader.SetTitle( szTitle );

			char szClassName[MAX_PATH] = "";
			kLoader.LoadString( "class_name", "", szClassName, MAX_PATH );

			if( strlen(szClassName) == 0 )
				continue;;

			DWORD dwMainIndex = kLoader.LoadInt( "main_index", 0 );
			int iMaxSubQuest = kLoader.LoadInt( "max_sub_quest", 0 );	
			for( int j = 0 ; j < iMaxSubQuest ; j++ )
			{
				char szKey[MAX_PATH] = "";
				sprintf_s( szKey, "sub%d_oneday_style", j );
				bool bOneDayStyle = kLoader.LoadBool( szKey, false );
				sprintf_s( szKey, "sub%d_oneday_cnt", j );
				int iOneDayCount = kLoader.LoadInt( szKey, 0 );
				if ( bOneDayStyle == true && iOneDayCount > 1 )
				{
					for ( int k = 0 ; k < iOneDayCount ; k++ )
					{
						int iChangeNextSubIndex = 0;
						if ( k != iOneDayCount - 1 )
						{
							SetQuest(szClassName, dwMainIndex, j, kLoader, true, j+k, j+k+1);
						}
						else
						{
							SetQuest(szClassName, dwMainIndex, j, kLoader, true, j+k, 0);
						}						
					}						
				}
				else
				{
					SetQuest(szClassName, dwMainIndex, j+1, kLoader);
				}
			}
		}
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Max Quest : %d", (int)m_QuestVariety.size() );
	}

	//////////////////////////////////////////////////////////////////////////
	{
		m_QuestRewardMap.clear();

		ioINILoader kLoader;
		kLoader.ReloadFile( "config/sp2_quest_present.ini" );

		kLoader.SetTitle( "common" );

		int iMaxReward = kLoader.LoadInt( "max_present", 0 );
		for(int i = 0;i < iMaxReward;i++)
		{
			char szTitle[MAX_PATH] = "";
			sprintf_s( szTitle, "Present%d", i + 1 );
			kLoader.SetTitle( szTitle );

			QuestReward kReward;
			kReward.m_iPresentType	 = kLoader.LoadInt( "PresentType", 0 );
			kReward.m_iPresentState	 = kLoader.LoadInt( "PresentAlarm", 0 );
			kReward.m_iPresentMent	 = kLoader.LoadInt( "PresentMent", 0 );
			kReward.m_iPresentPeriod = kLoader.LoadInt( "PresentPeriod", 0 );
			kReward.m_iPresentValue1 = kLoader.LoadInt( "PresentValue1", 0 );
			kReward.m_iPresentValue2 = kLoader.LoadInt( "PresentValue2", 0 );            
			kReward.m_bDirectReward  = kLoader.LoadBool( "PresentDirect", false );
			m_QuestRewardMap.insert( QuestRewardMap::value_type( kLoader.LoadInt( "RewardIndex", 0 ), kReward ) );
		}
	}
}

QuestParent *ioQuestManager::GetQuest( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( !pQuest ) continue;
		if( pQuest->GetMainIndex() == dwMainIndex && 
			pQuest->GetSubIndex() == dwSubIndex )
		{
			return pQuest;
		}
	}
  	return NULL;
}

void ioQuestManager::GetRewardPresent(DWORD dwPresentID, int& iType, int& iCode, int& iCount, int& iParam)
{
	QuestRewardMap::iterator iter = m_QuestRewardMap.find( dwPresentID );
	if( iter != m_QuestRewardMap.end() )
	{
		QuestReward &kReward = iter->second;
		iType = kReward.m_iPresentType;
		iCode = kReward.m_iPresentValue1;
		iCount = kReward.m_iPresentValue2;
		iParam = kReward.m_iPresentValue3;
	}
}

bool ioQuestManager::SendRewardPresent( User *pSendUser, DWORD dwPresentID )
{
	if( pSendUser == NULL ) return false;

	QuestRewardMap::iterator iter = m_QuestRewardMap.find( dwPresentID );
	if( iter != m_QuestRewardMap.end() )
	{
		QuestReward &kReward = iter->second;
		return pSendUser->_OnQuestRewardPresent( g_MainServer.GetSendID(), kReward.m_iPresentType, kReward.m_iPresentValue1, kReward.m_iPresentValue2, kReward.m_iPresentValue3, kReward.m_iPresentValue4,
					  						     kReward.m_iPresentState, kReward.m_iPresentMent, kReward.m_iPresentPeriod, kReward.m_bDirectReward );	
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuestManager::SendRewardPresent : None Present Index : %s - %d", pSendUser->GetPublicID().c_str(), dwPresentID );
	}
	return false;
}

bool ioQuestManager::IsSameChanneling( User *pSendUser, DWORD dwMainIndex, DWORD dwSubIndex )
{
	if( !pSendUser ) return false;

	QuestParent *pQuest = GetQuest( dwMainIndex, dwSubIndex );
	if( !pQuest ) return false;

	if( pQuest->GetChannelingType() == CNT_NONE )
		return true;

	if( pQuest->GetChannelingType() == pSendUser->GetChannelingType() )
		return true;

	return false;
}

DWORD ioQuestManager::GetPrevOneDayQuestDate()
{
	QuestDataParent kPrevTimeData;
	kPrevTimeData.SetDateData( m_dwNextOneDayQuestDate );

	CTime kCurTime( Help::GetSafeValueForCTimeConstructor( kPrevTimeData.GetYear(), kPrevTimeData.GetMonth(), kPrevTimeData.GetDay(), kPrevTimeData.GetHour(), kPrevTimeData.GetMinute(), 0 ) );
	CTimeSpan kAddTime( 0, 24, 0, 0 );
	kCurTime -= kAddTime;

	kPrevTimeData.SetDate( kCurTime.GetYear(), kCurTime.GetMonth(), kCurTime.GetDay(), kCurTime.GetHour(), kCurTime.GetMinute() );
	
	return kPrevTimeData.GetDateData();
}

void ioQuestManager::ProcessQuest()
{
	if( TIMEGETTIME() - m_dwCurrentTime < 60000 ) 
		return;

	m_dwCurrentTime = TIMEGETTIME();

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	int iLoopCount = 0;
	vQuestVariety vEventQuest;
	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( !pQuest ) continue;
		if( pQuest->GetPerformType() == QP_EVENT )
		{

			bool bPrevAlive = pQuest->IsAlive();
			bool bNextAlive = pQuest->IsCheckAlive();
			if( bPrevAlive != bNextAlive )
			{
				pQuest->SetAlive( bNextAlive );
				vEventQuest.push_back( pQuest );
			}
		}
	}
#ifdef SRC_ID
	CheckActiveQuest();
#endif
	// 모든 유저에게 전송
	SP2Packet kPacket( STPK_QUEST_EVENT_ALIVE );
	kPacket << (int)vEventQuest.size();
	for(iter = vEventQuest.begin();iter != vEventQuest.end();++iter)
	{
		QuestParent *pQuest = *iter;
		kPacket << pQuest->GetMainIndex() << pQuest->GetSubIndex() << pQuest->IsAlive();
	}
	g_UserNodeManager.SendMessageAll( kPacket );

	// 유저의 퀘스트 슬롯에서 종료된 퀘스트를 삭제함.
	for(iter = vEventQuest.begin();iter != vEventQuest.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( !pQuest->IsAlive() )
		{
			g_UserNodeManager.UserNode_QuestRemove( pQuest->GetMainIndex(), pQuest->GetSubIndex() );
		}
	}
	vEventQuest.clear();

	ProcessOneDayQuest();
}

void ioQuestManager::SendAliveEventQuest( User *pSendUser )
{
	if( !pSendUser )
		return;

	int iLoopCount = 0;
	vQuestVariety vEventQuest;	
	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( !pQuest ) continue;
		if( pQuest->GetPerformType() == QP_EVENT )
		{
			if( pQuest->IsAlive() )
			{
				vEventQuest.push_back( pQuest );      // 진행중인 이벤트 퀘스트만 전송. 클라이언트 초기화시 모두 진행하지 않는 퀘스트임.
			}
		}
	}

	// 모든 유저에게 전송
	SP2Packet kPacket( STPK_QUEST_EVENT_ALIVE );
	kPacket << (int)vEventQuest.size();
	for(iter = vEventQuest.begin();iter != vEventQuest.end();++iter)
	{
		QuestParent *pQuest = *iter;
		kPacket << pQuest->GetMainIndex() << pQuest->GetSubIndex() << pQuest->IsAlive();
	}
	pSendUser->SendMessage( kPacket );
}

void ioQuestManager::ProcessOneDayQuest()
{
	CTime kCurTime  = CTime::GetCurrentTime();
	QuestDataParent kCurTimeData;
	kCurTimeData.SetDate( kCurTime.GetYear(), kCurTime.GetMonth(), kCurTime.GetDay(), kCurTime.GetHour(), kCurTime.GetMinute() );

	QuestDataParent kCheckTimeData;
	if( m_dwNextOneDayQuestDate == 0 )
	{
		CTime kTempTime = kCurTime;
		if( kTempTime.GetHour() >= (int)m_dwOneDayQuestHour )
		{
			// 다음날 시간 체크
			CTimeSpan kAddTime( 0, 24, 0, 0 );
			kTempTime += kAddTime;
		}
		kCheckTimeData.SetDate( kTempTime.GetYear(), kTempTime.GetMonth(), kTempTime.GetDay(), (WORD)m_dwOneDayQuestHour, 0 );
		m_dwNextOneDayQuestDate = kCheckTimeData.GetDateData();

		// INI 저장
		ioINILoader kLoader( "config/sp2_one_day_quest.ini" );
		kLoader.SetTitle( "common" );
		kLoader.SaveInt( "oneday_current", m_dwNextOneDayQuestDate );
	}
	else
	{
		kCheckTimeData.SetDateData( m_dwNextOneDayQuestDate );
	}

	if( kCurTimeData.GetDateData() >= kCheckTimeData.GetDateData() )
	{
		CTime kNextTime = kCurTime;
		CTimeSpan kAddTime( 0, 24, 0, 0 );
		kNextTime += kAddTime;
		kCheckTimeData.SetDate( kNextTime.GetYear(), kNextTime.GetMonth(), kNextTime.GetDay(), (WORD)m_dwOneDayQuestHour, 0 );
		m_dwNextOneDayQuestDate = kCheckTimeData.GetDateData();

		// INI 저장
		ioINILoader kLoader( "config/sp2_one_day_quest.ini" );
		kLoader.SetTitle( "common" );
		kLoader.SaveInt( "oneday_current", m_dwNextOneDayQuestDate );

		// 초기화
		g_UserNodeManager.UserNode_QuestOneDayRemoveAll();
		// 모든 유저에게 전송 - 0이면 모두 삭제
		SP2Packet kPacket( STPK_ONEDAY_QUEST_DELETE );
		g_UserNodeManager.SendMessageAll( kPacket );
	}
}

void ioQuestManager::CheckActiveQuest()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	if( st.wMinute != 0 ) return; //1시간에 한번만  체크

	DWORD dwCurrentTime = Help::ConvertYYMMDDHHMMToDate( st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute );

	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		
		if( COMPARE( dwCurrentTime, (*iter)->GetStartTime() ,(*iter)->GetEndTime() ))
		{
			(*iter)->SetQuestPossible(true);
		}
		else
			(*iter)->SetQuestPossible(false);
	}
}
