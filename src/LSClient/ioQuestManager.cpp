
#include "stdafx.h"

#include "ioQuestManager.h"

#include "GUI/TotalResultMainWnd.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

template<> ioQuestManager* Singleton< ioQuestManager >::ms_Singleton = 0;
ioQuestManager::ioQuestManager()
{
	m_pQuestHelpOver   = NULL;
	m_pPlayStage       = NULL;
	m_bQuestInfoChange = false;
	m_bQuestAlarmChange= false;
}

ioQuestManager::~ioQuestManager()
{
	m_QuestOccurList.clear();
	m_QuestAttainList.clear();
	m_QuestRewardMap.clear();
	m_QuestDelAndFailList.clear();
	ClearQuestHelp();
	ClearQuestVariety();
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
}

void ioQuestManager::ClearQuestHelp()
{
	QuestHelpListMap::iterator iter = m_QuestHelpListMap.begin();
	for(;iter != m_QuestHelpListMap.end();++iter)
	{
		QuestHelpList &rkList = iter->second;
		for(int i = 0;i < (int)rkList.m_QuestHelp.size();i++)
		{
			QuestHelp *pHelp = rkList.m_QuestHelp[i];
			if( !pHelp )
				continue;
			SAFEDELETE( pHelp->m_pImage );
			SAFEDELETE( pHelp );
		}
	}
	m_QuestHelpListMap.clear();

	SAFEDELETE( m_pQuestHelpOver );
}

void ioQuestManager::EraseCompleteReserve( QuestComplete eComplete, int iValue1, int iValue2, int iValue3 )
{
	if( m_CompleteReserve.empty() )
		return;

	vCompleteReserve::iterator iter = m_CompleteReserve.begin();
	for(;iter != m_CompleteReserve.end();++iter)
	{
		CompleteReserve &kReserve = *iter;
		if( kReserve.m_eComplete != eComplete ) continue;
		if( kReserve.m_iValue1 != iValue1 ) continue;
		if( kReserve.m_iValue2 != iValue2 ) continue;
		if( kReserve.m_iValue3 != iValue3 ) continue;

		m_CompleteReserve.erase( iter );
		return;
	}
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

	LOG.PrintTimeAndLog( 0, "ioQuestManager::CreateQuest None Class : %s", rClassName.c_str() );
	return NULL;
}

void ioQuestManager::LoadINIData()
{
	LOG.PrintTimeAndLog( 0, "Start - Quest Data Load" );
	ClearQuestVariety();

	{
		ioINILoader_e kLoader( "config/sp2_quest_info.ini" );
		kLoader.SetTitle_e( "common" );
		bool bDisable = kLoader.LoadBool_e( "all_quest_disable", false );		
		if( bDisable )
		{
			LOG.PrintTimeAndLog( 0, "End - Quest Data not load(quest disable on)" );
			return;
		}

		int iMaxQuest = kLoader.LoadInt_e( "max_quest", 0 );
		for(int i = 0;i < iMaxQuest;i++)
		{
			char szTitle[MAX_PATH] = "";
			sprintf_e( szTitle, "quest%d", i + 1 );
			kLoader.SetTitle( szTitle );

			char szClassName[MAX_PATH] = "";
			kLoader.LoadString_e( "class_name", "", szClassName, MAX_PATH );

			DWORD dwMainIndex = kLoader.LoadInt_e( "main_index", 0 );
			int iMaxSubQuest = kLoader.LoadInt_e( "max_sub_quest", 0 );			
			for(int j = 0;j < iMaxSubQuest;j++)
			{
				QuestParent *pQuestData = pQuestData = CreateQuest( szClassName );
				if( pQuestData )
				{
					pQuestData->SetQuestData( dwMainIndex, j + 1, kLoader );
					m_QuestVariety.push_back( pQuestData );		
				}
			}
		}
//		LOG.PrintTimeAndLog( 0, "Max Quest : %d", (int)m_QuestVariety.size() );
	}

	//////////////////////////////////////////////////////////////////////////
	{
		ioINILoader_e kLoader( "config/sp2_quest_present.ini" );
		kLoader.SetTitle_e( "common" );

		int iMaxReward = kLoader.LoadInt_e( "max_present", 0 );
		for(int i = 0;i < iMaxReward;i++)
		{
			char szTitle[MAX_PATH] = "";
			sprintf_e( szTitle, "Present%d", i + 1 );
			kLoader.SetTitle( szTitle );

			QuestReward kReward;
			kReward.m_iPresentType	 = kLoader.LoadInt_e( "PresentType", 0 );
			kReward.m_iPresentValue1 = kLoader.LoadInt_e( "PresentValue1", 0 );
			kReward.m_iPresentValue2 = kLoader.LoadInt_e( "PresentValue2", 0 );      
			kReward.m_bDirectReward  = kLoader.LoadBool_e( "PresentDirect", false );

			m_QuestRewardMap.insert( QuestRewardMap::value_type( kLoader.LoadInt_e( "RewardIndex", 0 ), kReward ) );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	{
		ClearQuestHelp();

//		m_pQuestHelpOver = g_UIImageSetMgr.CreateImageByFullName( "BaseImage008#icon0_questover" ); //사용 안함

		ioINILoader_e kLoader( "config/sp2_quest_help.ini" );
		kLoader.SetTitle_e( "common" );

		// 자동개행
		float fReceiveSplitWidth   = kLoader.LoadFloat_e( "receive_split_width", 0.0f );
		float fCompleteSplitWidth  = kLoader.LoadFloat_e( "complete_split_width", 0.0f );
		float fObjectiveSplitWidth = kLoader.LoadFloat_e( "objective_split_width", 0.0f );
		int   iShortLineOffsetX = kLoader.LoadInt_e( "shortline_offsetx", 0 );
		int   iLongLineOffsetX  = kLoader.LoadInt_e( "longline_offsetx", 0 );
		//

		int iMaxHelp = kLoader.LoadInt_e( "max_help", 0 );
		for(int i = 0;i < iMaxHelp;i++)
		{
			char szTitle[MAX_PATH], szKey[MAX_PATH], szBuf[MAX_PATH];
			sprintf_e( szTitle, "help%d", i + 1 );
			kLoader.SetTitle( szTitle );
			
			DWORD dwIndex = kLoader.LoadInt_e( "index", 0 );
			int iMaxLine = kLoader.LoadInt_e( "max_line", 0 );

			QuestHelpList kQuestHelpList;
			for(int j = 0;j < iMaxLine;j++)
			{
				QuestHelp *pQuestHelp = new QuestHelp;
				if( !pQuestHelp )
					continue;
				sprintf_e( szKey, "line%d_offsetx", j + 1 );
				pQuestHelp->m_iOffSetX = kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "line%d_offsety", j + 1 );
				pQuestHelp->m_iOffSetY = kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "line%d_image", j + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				pQuestHelp->m_pImage = g_UIImageSetMgr.CreateImageByFullName( szBuf );

				sprintf_e( szKey, "line%d_size", j + 1 );
				float fSize = kLoader.LoadFloat( szKey, 0.0f );

				sprintf_e( szKey, "line%d_count", j + 1 );
				int iLineCount = kLoader.LoadInt( szKey, 0 );
				for(int k = 0;k < iLineCount;k++)
				{
					sprintf_e( szKey, "line%d_color%d", j + 1, k + 1 );
					kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
					char *pTemp = NULL;
					DWORD dwColor = strtoul( szBuf, &pTemp, 16 );					
					pQuestHelp->m_szHelp.SetTextStyle( TS_NORMAL );
					pQuestHelp->m_szHelp.SetTextColor( dwColor );

					sprintf_e( szKey, "line%d_space%d", j + 1, k + 1 );
					int iSpace = kLoader.LoadInt( szKey, 0 );
					for(int s = 0;s < iSpace;s++)
						pQuestHelp->m_szHelp.AddTextPiece( fSize, " " );  //띄어쓰기

					sprintf_e( szKey, "line%d_text%d", j + 1, k + 1 );
					kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
					pQuestHelp->m_szHelp.AddTextPiece( fSize, szBuf );
				}

				// 자동개행
				if( fReceiveSplitWidth != 0.0f  )
				{
					enum { MAX_RECEIVE_INDEX = 100000, MAX_COMPLETE_INDEX = 200000, MAX_SHORT_LINE = 4, MAX_LOOP = 20, };
					float fSplitWidth = 0.0f;
					if( dwIndex < MAX_RECEIVE_INDEX )
						fSplitWidth = fReceiveSplitWidth;
					else if( dwIndex < MAX_COMPLETE_INDEX )
						fSplitWidth = fCompleteSplitWidth;
					else
						fSplitWidth = fObjectiveSplitWidth;

					for (int k = 0; k < MAX_LOOP ; k++)
					{
						int iOffSetWidth = iLongLineOffsetX;
						if( dwIndex < MAX_RECEIVE_INDEX && kQuestHelpList.m_QuestHelp.size() < MAX_SHORT_LINE )
							iOffSetWidth = iShortLineOffsetX;

						if( pQuestHelp->m_szHelp.GetFullWidth() < ( fSplitWidth - iOffSetWidth ) )
						{
							kQuestHelpList.m_QuestHelp.push_back( pQuestHelp );
							break;
						}

						ioComplexStringPrinter kPrinterA, kPrinterB;
						pQuestHelp->m_szHelp.SplitFullText( kPrinterA, kPrinterB, ( fSplitWidth - iOffSetWidth ) );

						// 앞문장
						QuestHelp *pQuestHelpA = new QuestHelp;
						if( !pQuestHelpA )
							continue;
						if( kQuestHelpList.m_QuestHelp.size() < MAX_SHORT_LINE )
							pQuestHelpA->m_iOffSetX = iShortLineOffsetX;
						else
							pQuestHelpA->m_iOffSetX = iLongLineOffsetX;
						pQuestHelpA->m_iOffSetY = pQuestHelp->m_iOffSetY;
						kPrinterA.CopyFullText( pQuestHelpA->m_szHelp );
						kQuestHelpList.m_QuestHelp.push_back( pQuestHelpA ); 

						// 뒷문장
						if( kQuestHelpList.m_QuestHelp.size() < MAX_SHORT_LINE )
							pQuestHelp->m_iOffSetX = iShortLineOffsetX;
						else
							pQuestHelp->m_iOffSetX = iLongLineOffsetX;
						kPrinterB.CopyFullText( pQuestHelp->m_szHelp );
					}
				}
				//
				else
					kQuestHelpList.m_QuestHelp.push_back( pQuestHelp );
			}
			m_QuestHelpListMap.insert( QuestHelpListMap::value_type( dwIndex, kQuestHelpList ) );
		}
//		LOG.PrintTimeAndLog( 0, "Max Quest Help : %d", (int)m_QuestHelpListMap.size() );
	}

	LOG.PrintTimeAndLog( 0, "End - Quest Data Load" );
}

void ioQuestManager::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void ioQuestManager::QuestOccurTerm( QuestOccur eOccur, int iValue1 /* = 0 */, int iValue2 /* = 0 */, int iValue3 /* = 0  */ )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( !pQuest ) continue;
		if( !pQuest->IsCheckOccurTerm( eOccur, iValue1, iValue2, iValue3 ) ) continue;

		//퀘스트 발생 UI 오픈 
		AddQuestOccurList( pQuest );
	}
}

void ioQuestManager::QuestCompleteTerm( QuestComplete eComplete, int iValue1 /* = 0 */, int iValue2 /* = 0 */, int iValue3 /* = 0  */ )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	EraseCompleteReserve( eComplete, iValue1, iValue2, iValue3 );

	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( !pQuest ) continue;
		if( !pQuest->IsCheckCompleteTerm( eComplete, iValue1, iValue2, iValue3 ) ) continue;

		//퀘스트 달성 UI 오픈 
		AddQuestAttainList( pQuest );
	}
}

void ioQuestManager::QuestCompleteTermReserve( QuestComplete eComplete, int iValue1 /* = 0 */, int iValue2 /* = 0 */, int iValue3 /* = 0  */ )
{
	if( !P2PNetwork::IsNetworkPlaying() ) 
		return;

	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( !pQuest ) continue;
		if( !pQuest->IsMeProgressQuest() ) continue;

		CompleteReserve kReserve;
		kReserve.m_eComplete = eComplete;
		kReserve.m_iValue1   = iValue1;
		kReserve.m_iValue2   = iValue2;
		kReserve.m_iValue3   = iValue3;

		m_CompleteReserve.push_back( kReserve );
		break;
	}
}

void ioQuestManager::QuestAlreadyAttain( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( !pQuest ) continue;
		if( pQuest->GetMainIndex() != dwMainIndex ) continue;
		if( pQuest->GetSubIndex() != dwSubIndex ) continue;
		
		if( !pQuest->IsCheckAlreadyAttain() )
			return;

		//퀘스트 달성 UI 오픈 
		AddQuestAttainList( pQuest );
	}
}

void ioQuestManager::QuestCompleteNextOccur( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( !pQuest ) continue;
		if( pQuest->GetMainIndex() != dwMainIndex ) continue;
		if( pQuest->GetSubIndex() != dwSubIndex ) continue;

		QuestParent *pNextQuest = GetQuest( pQuest->GetNextMainIndex(), pQuest->GetNextSubIndex() );
		if( !pNextQuest ) 
			return;		

		ioQuest *pMyQuest = g_MyInfo.GetQuest();
		if( pMyQuest )
		{
			QuestData &kQuest = pMyQuest->GetQuestData( pNextQuest->GetNextMainIndex(), pNextQuest->GetNextSubIndex() );
			if( kQuest.GetMainIndex() > 0 )
				return;         // 이미 보유하고 있는 퀘스트.
		}

		//퀘스트 발생 UI 오픈 
		AddQuestOccurList( pNextQuest );
	}
}

QuestParent *ioQuestManager::GetQuest( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestVariety::iterator iter = m_QuestVariety.begin();
	for(;iter != m_QuestVariety.end();++iter)
	{
		QuestParent *pQuestParent = *iter;
		if( pQuestParent->GetMainIndex() != dwMainIndex ) continue;
		
		if( pQuestParent->GetSubIndex() == dwSubIndex )
			return pQuestParent;
	}
	return NULL;
}

void ioQuestManager::AddQuestOccurList( QuestParent *pQuestParent )
{
	if( !pQuestParent ) return;

	{
		vQuestVariety::iterator iter = m_QuestOccurList.begin();
		for(;iter != m_QuestOccurList.end();++iter)
		{
			QuestParent *pQuest = *iter;
			if( pQuest == pQuestParent )
				return;
		}
	}
	m_QuestOccurList.push_back( pQuestParent );
}

QuestParent *ioQuestManager::FirstQuestOccurList()
{
	if( !m_pPlayStage ) return NULL;
	if( m_QuestOccurList.empty() ) return NULL;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() || g_BattleRoomMgr.IsBroadcastModeType() ) 
			return NULL;
	}

	vQuestVariety::iterator iter = m_QuestOccurList.begin();
	for(;iter != m_QuestOccurList.end();++iter)
	{
		QuestParent *pQuestParent = *iter;
		if( pQuestParent == NULL ) continue;
		if( m_pPlayStage->GetModeType() == MT_NONE			|| 
			m_pPlayStage->GetModeType() == MT_MYROOM		|| 
			m_pPlayStage->GetModeType() == MT_TRAINING		||
			m_pPlayStage->GetModeType() == MT_HEADQUARTERS	||
			m_pPlayStage->GetModeType() == MT_HOUSE	)
		{
			return pQuestParent;
		}

		if( m_pPlayStage->IsRoundSetEnd() && !pQuestParent->IsRoundEndOccurShow() && !TotalResultMainWnd::IsLevelGaugeAniEnd() ) continue;

		if( pQuestParent->IsPlayOccurShow() || 
			m_pPlayStage->IsRoundSetEnd() )
		{
			return pQuestParent;
		}
	}	
	return NULL;
}

void ioQuestManager::EraseQuestOccurList( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestVariety::iterator iter = m_QuestOccurList.begin();
	for(;iter != m_QuestOccurList.end();++iter)
	{
		QuestParent *pQuest = *iter;
		if( pQuest->GetMainIndex() != dwMainIndex ) continue;
		if( pQuest->GetSubIndex() != dwSubIndex ) continue;

		m_QuestOccurList.erase( iter );
		return;
	}
}

void ioQuestManager::AddQuestAttainList( QuestParent *pQuestParent )
{
	if( !pQuestParent ) return;

	{
		vQuestVariety::iterator iter = m_QuestAttainList.begin();
		for(;iter != m_QuestAttainList.end();++iter)
		{
			QuestParent *pQuest = *iter;
			if( pQuest == pQuestParent )
				return;
		}	
	}
	m_QuestAttainList.push_back( pQuestParent );
}

QuestParent *ioQuestManager::FirstQuestAttainList()
{
	if( !m_pPlayStage ) return NULL;
	if( m_QuestAttainList.empty() ) return NULL;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{ 
		if( g_BattleRoomMgr.IsTournamentModeType() || g_BattleRoomMgr.IsBroadcastModeType() ) 
			return NULL;
	}

	vQuestVariety::iterator iter = m_QuestAttainList.begin();
	for(;iter != m_QuestAttainList.end();++iter)
	{
		QuestParent *pQuestParent = *iter;
		if( pQuestParent == NULL ) continue;
		if( m_pPlayStage->GetModeType() == MT_NONE			|| 
			m_pPlayStage->GetModeType() == MT_MYROOM		|| 
			m_pPlayStage->GetModeType() == MT_TRAINING		||
			m_pPlayStage->GetModeType() == MT_HEADQUARTERS	||
			m_pPlayStage->GetModeType() == MT_HOUSE	)
		{
			return pQuestParent;
		}
	
		if( m_pPlayStage->IsRoundSetEnd() && !pQuestParent->IsRoundEndCompleteShow() && !TotalResultMainWnd::IsLevelGaugeAniEnd() ) continue;

		if( pQuestParent->IsPlayCompleteShow() || 
			m_pPlayStage->IsRoundSetEnd() )
		{
            return pQuestParent;
		}
	}	
	return NULL;
}

void ioQuestManager::EraseQuestAttainList( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestVariety::iterator iter = m_QuestAttainList.begin();
	for(;iter != m_QuestAttainList.end();++iter)
	{
		QuestParent *pQuestParent = *iter;
		if( pQuestParent->GetMainIndex() != dwMainIndex ) continue;
		if( pQuestParent->GetSubIndex() != dwSubIndex ) continue;

		m_QuestAttainList.erase( iter );
		return;
	}
}

void ioQuestManager::AddQuestDelAndFailList( bool bDelete, QuestParent *pQuestParent )
{
	{
		// 창섭씨 요청에 의해서 블럭함 2011.04.06 : http://192.168.1.132/publicData/board/bug/bugContent.asp?idx=8603&boardIDX=110
		return;
	}

	if( !pQuestParent ) return;

	{
		vQuestDelAndFail::iterator iter = m_QuestDelAndFailList.begin();
		for(;iter != m_QuestDelAndFailList.end();++iter)
		{
			QuestDelAndFail &rkData = *iter;
			if( rkData.m_pQuestParent == pQuestParent )
				return;
		}	
	}

	QuestDelAndFail kData;
	kData.m_bDelete = bDelete;
	kData.m_pQuestParent = pQuestParent;
	m_QuestDelAndFailList.push_back( kData );
}

QuestParent *ioQuestManager::FirstQuestDelAndFailList( bool &rbDelete )
{
	if( m_QuestDelAndFailList.empty() ) return NULL;

	QuestDelAndFail &rkData = m_QuestDelAndFailList[0];
	rbDelete = rkData.m_bDelete;
	return rkData.m_pQuestParent;
}

void ioQuestManager::EraseQuestDelAndFailList( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestDelAndFail::iterator iter = m_QuestDelAndFailList.begin();
	for(;iter != m_QuestDelAndFailList.end();++iter)
	{
		QuestDelAndFail &rkData = *iter;
		if( rkData.m_pQuestParent == NULL ) continue;
		if( rkData.m_pQuestParent->GetMainIndex() != dwMainIndex ) continue;
		if( rkData.m_pQuestParent->GetSubIndex() != dwSubIndex ) continue;

		m_QuestDelAndFailList.erase( iter );
		return;
	}
}

void ioQuestManager::QuestInfoChange()
{
	m_bQuestInfoChange = true;
	QuestAlarmChange();
}

bool ioQuestManager::IsQuestInfoChange()
{
	bool bReturn = m_bQuestInfoChange;
	m_bQuestInfoChange = false;
	return bReturn;
}

void ioQuestManager::QuestAlarmChange()
{
	m_bQuestAlarmChange = true;
}

bool ioQuestManager::IsQuestAlarmChange()
{
	bool bReturn = m_bQuestAlarmChange;
	m_bQuestAlarmChange = false;
	return bReturn;
}

bool ioQuestManager::IsInvitedPass()
{	
	ioQuest *pQuest = g_MyInfo.GetQuest();
	if( !pQuest ) return false;

	int iMaxProgress = pQuest->GetMaxQuest( QS_PROGRESS );
	for(int i = 0;i < iMaxProgress;i++)
	{
		QuestData kQuestData = pQuest->GetProcessToArray( i );
		QuestParent *pQuestParent = kQuestData.GetLinkQuest();
		if( !pQuestParent ) continue;
		if( pQuestParent->IsInvitedPass() )
			return true;		
	}

	return false;
}

bool ioQuestManager::IsAllDirectPresent( QuestParent *pQuestParent )
{
	if( !pQuestParent )
		return true;

	int iMaxReward = pQuestParent->GetMaxRewardPresent();
	for(int i = 0;i < iMaxReward;i++)
	{
		QuestRewardMap::iterator iter = m_QuestRewardMap.find( pQuestParent->GetRewardPresentIndex( i ) );
		if( iter != m_QuestRewardMap.end() )
		{
			QuestReward &kReward = iter->second;		
			if( !kReward.m_bDirectReward )
				return false;		
		}
	}
	return true;
}

const ioHashString &ioQuestManager::GetPresentName( DWORD dwIndex )
{
	static ioHashString szReturn;
	QuestRewardMap::iterator iter = m_QuestRewardMap.find( dwIndex );
	if( iter != m_QuestRewardMap.end() )
	{
		QuestReward &kReward = iter->second;		
		
		char szPresentName[MAX_PATH] = "";
		if( kReward.m_iPresentType == PRESENT_PESO || kReward.m_iPresentType == PRESENT_GRADE_EXP )
		{
			sprintf( szPresentName, "%s", g_PresentMgr.GetPresentValue2Text( kReward.m_iPresentType, kReward.m_iPresentValue1, kReward.m_iPresentValue2 ).c_str() );
		}
		else
		{
			sprintf( szPresentName, "%s%s", g_PresentMgr.GetPresentValue1Text( kReward.m_iPresentType, kReward.m_iPresentValue1, kReward.m_iPresentValue2 ).c_str(),
											g_PresentMgr.GetPresentValue2Text( kReward.m_iPresentType, kReward.m_iPresentValue1, kReward.m_iPresentValue2 ).c_str() );
		}
		szReturn = szPresentName;		
	}
	else
	{
		szReturn.Clear();
	}
	return szReturn;
}

void ioQuestManager::GetPresentData( DWORD dwIndex, int &rPresentType, int &rPresentValue1, int &rPresentValue2, bool &rPresentDirect )
{
	rPresentDirect = false;
	rPresentType = rPresentValue1 = rPresentValue2 = 0;
	QuestRewardMap::iterator iter = m_QuestRewardMap.find( dwIndex );
	if( iter != m_QuestRewardMap.end() )
	{
		QuestReward &kReward = iter->second;		
		rPresentType = kReward.m_iPresentType;
		rPresentValue1 = kReward.m_iPresentValue1;
		rPresentValue2 = kReward.m_iPresentValue2;
		rPresentDirect = kReward.m_bDirectReward;
	}
}

void ioQuestManager::DirectPresentAlarm( DWORD dwMainIndex, DWORD dwSubIndex, SP2Packet &rkPacket )
{
	QuestParent *pQuestParent = GetQuest( dwMainIndex, dwSubIndex );
	if( !pQuestParent ) return;

	int iPresentSize;
	rkPacket >> iPresentSize;
	for(int i = 0;i < iPresentSize;i++)
	{
		bool bDirectPresent = false;
		rkPacket >> bDirectPresent;

		if( bDirectPresent )
		{
			DWORD dwPresentIndex = pQuestParent->GetRewardPresentIndex( i );
			if( dwPresentIndex > 0 )
			{
				int iPresentType, iPresentValue1, iPresentValue2;
				GetPresentData( dwPresentIndex, iPresentType, iPresentValue1, iPresentValue2, bDirectPresent );
				if( iPresentType > 0 )
				{
					switch( iPresentType )
					{
					case PRESENT_SOLDIER:
						{
							if( iPresentValue2 == 0 )
							{
								g_ChatMgr.SetSoldierObtainMsg( iPresentValue1, 0, CPT_MORTMAIN, EXERCISE_NONE );
							}
							else
							{
								g_ChatMgr.SetSoldierObtainMsg( iPresentValue1, iPresentValue2, CPT_TIME, EXERCISE_NONE );
							}
						}
						break;	
					case PRESENT_DECORATION:
						{
							int iClassType = iPresentValue1 / 100000;
							int iSexType   = (iPresentValue1 % 100000) / 1000;
							int iDecoType  = iPresentValue1 % 1000;
							g_ChatMgr.SetDecoObtainMsg( iClassType, iSexType, iDecoType, iPresentValue2 );
						}
						break;
					case PRESENT_ETC_ITEM:
						{			
							g_ChatMgr.SetEtcItemObtainMsg( iPresentValue1, iPresentValue2 );
						}
						break;
					case PRESENT_PESO:
						{
							g_ChatMgr.SetPesoObtainMsg( iPresentValue1 );
						}
						break;
					case PRESENT_EXTRAITEM:
						{
							if( (iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_2) == 0 )
								g_ChatMgr.SetExtraItemObtainMsg( iPresentValue1, ((iPresentValue2%PRESENT_EXTRAITEM_DIVISION_1) / PRESENT_EXTRAITEM_DIVISION_2), EPT_MORTMAIN, 0 );
							else
								g_ChatMgr.SetExtraItemObtainMsg( iPresentValue1, ((iPresentValue2%PRESENT_EXTRAITEM_DIVISION_1) / PRESENT_EXTRAITEM_DIVISION_2), EPT_TIME, (iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_2) );
						}
						break;
					case PRESENT_GRADE_EXP:
						{
							g_ChatMgr.SetGradeExpObtainMsg( iPresentValue1 );
						}
						break;
					case PRESENT_MEDALITEM:
						{
							g_ChatMgr.SetMedalItemObtainMsg( iPresentValue1, iPresentValue2 );
						}
						break;
					case PRESENT_COSTUME:
						{
							if( (iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_2) == 0 )
								g_ChatMgr.SetCostumeObtainMsg( iPresentValue1, EPT_MORTMAIN, 0 );
							else
								g_ChatMgr.SetCostumeObtainMsg( iPresentValue1, EPT_TIME, (iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_2) );
						}
						break;
					}
				}
			}
		}
	}
}

int ioQuestManager::GetMaxQuestHelp( QuestParent *pQuestParent, DWORD dwState )
{
	if( !pQuestParent ) 
		return 0;

	DWORD dwHelpIndex = pQuestParent->GetHelpIndex();
	if( dwState == QS_ATTAIN || dwState == QS_COMPLETE )
		dwHelpIndex += QUEST_COMPLETE_HELP_ADD;

	QuestHelpListMap::iterator iter = m_QuestHelpListMap.find( dwHelpIndex );
	if( iter != m_QuestHelpListMap.end() )
	{
		QuestHelpList &kQuestHelpList = iter->second;
		return kQuestHelpList.m_QuestHelp.size();
	}
	return 0;
}

int ioQuestManager::GetMaxQuestPurposeHelp( QuestParent *pQuestParent )
{
	if( !pQuestParent ) 
		return 0;

	DWORD dwHelpIndex = pQuestParent->GetHelpIndex() + QUEST_PURPOSE_HELP_ADD;
	QuestHelpListMap::iterator iter = m_QuestHelpListMap.find( dwHelpIndex );
	if( iter != m_QuestHelpListMap.end() )
	{
		QuestHelpList &kQuestHelpList = iter->second;
		return kQuestHelpList.m_QuestHelp.size();
	}
	return 0;
}

float ioQuestManager::GetMaxQuestHelpWidth( QuestParent *pQuestParent, DWORD dwState )
{
	if( !pQuestParent ) 
		return 0.0f;

	float fFullWidth = 0.0f;
	DWORD dwHelpIndex = pQuestParent->GetHelpIndex();
	if( dwState == QS_ATTAIN || dwState == QS_COMPLETE )
		dwHelpIndex += QUEST_COMPLETE_HELP_ADD;

	QuestHelpListMap::iterator iter = m_QuestHelpListMap.find( dwHelpIndex );
	if( iter != m_QuestHelpListMap.end() )
	{
		QuestHelpList &kQuestHelpList = iter->second;
		for(int i = 0, iLineCount = 0;i < (int)kQuestHelpList.m_QuestHelp.size();i++)
		{
			QuestHelp *pQuestHelp = kQuestHelpList.m_QuestHelp[i];	
			if( !pQuestHelp ) 
				continue;
			fFullWidth += pQuestHelp->m_szHelp.GetFullWidth() + FLOAT10;
		}
	}
	return fFullWidth;
}

void ioQuestManager::RenderQuestProgressHelp( QuestParent *pQuestParent, int iXPos, int iYPos, int iScrollPos, int iMaxLine, float fCurWidth )
{
	if( !pQuestParent ) 
		return;

	enum { YGAP = 21, };
	QuestHelpListMap::iterator iter = m_QuestHelpListMap.find( pQuestParent->GetHelpIndex() );
	if( iter != m_QuestHelpListMap.end() )
	{
		QuestHelpList &kQuestHelpList = iter->second;
		for(int i = 0, iLineCount = 0;i < (int)kQuestHelpList.m_QuestHelp.size();i++)
		{
			if( iLineCount >= iMaxLine )
				return;

			QuestHelp *pQuestHelp = kQuestHelpList.m_QuestHelp[i];			
			if( !pQuestHelp )
				continue;
			if( pQuestHelp->m_pImage )
			{		
				int iOffsetArray = iScrollPos - i;
				int iStartH = min( pQuestHelp->m_pImage->GetHeight(), max( 0, iOffsetArray * YGAP ) );
				int iEndH   = min( ((iMaxLine + max( 0, iOffsetArray)) - iLineCount) * YGAP, pQuestHelp->m_pImage->GetHeight() );
				pQuestHelp->m_pImage->RenderHeightCut( iXPos, ( iYPos + (iLineCount * YGAP) ) - iStartH, iStartH, iEndH );

				if( m_pQuestHelpOver ) //
				{
					const int iAddSize = 25;
					int iOverHeight = ( iEndH - iStartH );
					if( iStartH == 0 )
						iOverHeight += iAddSize;
					if( iEndH == pQuestHelp->m_pImage->GetHeight() )
						iOverHeight += iAddSize;

					int iOverStartH = max( 0, iStartH );
					int iOverEndH   = min( iOverHeight + iStartH, m_pQuestHelpOver->GetHeight() );
					m_pQuestHelpOver->RenderHeightCut( iXPos - iAddSize, ( iYPos + (iLineCount * YGAP) ) - (iOverStartH + iAddSize), iOverStartH, iOverEndH );
				}
			}

			if( i < iScrollPos )
			{
				fCurWidth -= pQuestHelp->m_szHelp.GetFullWidth();
				continue;
			}
			
			if( fCurWidth > 0.0f )
			{
				fCurWidth -= pQuestHelp->m_szHelp.PrintFullTextWidthDirectCut( iXPos + pQuestHelp->m_iOffSetX, iYPos + (iLineCount * YGAP) + pQuestHelp->m_iOffSetY, TAT_LEFT, fCurWidth );
			}			
			iLineCount++;
		}
	}
}

void ioQuestManager::RenderQuestPurposeHelp( QuestParent *pQuestParent, int iXPos, int iYPos )
{
	if( !pQuestParent ) 
		return;

	enum { YGAP = 18, };
	QuestHelpListMap::iterator iter = m_QuestHelpListMap.find( pQuestParent->GetHelpIndex() + QUEST_PURPOSE_HELP_ADD );
	if( iter != m_QuestHelpListMap.end() )
	{
		QuestHelpList &kQuestHelpList = iter->second;
		for(int i = 0;i < (int)kQuestHelpList.m_QuestHelp.size();i++)
		{
			QuestHelp *pQuestHelp = kQuestHelpList.m_QuestHelp[i];			
			if( !pQuestHelp )
				continue;
			pQuestHelp->m_szHelp.PrintFullText( iXPos, iYPos + i * YGAP, TAT_LEFT );
		}
	}
}

void ioQuestManager::RenderQuestCompleteHelp( QuestParent *pQuestParent, int iXPos, int iYPos )
{
	if( !pQuestParent ) 
		return;

	enum { YGAP = 19, };
	QuestHelpListMap::iterator iter = m_QuestHelpListMap.find( pQuestParent->GetHelpIndex() + QUEST_COMPLETE_HELP_ADD );
	if( iter != m_QuestHelpListMap.end() )
	{
		QuestHelpList &kQuestHelpList = iter->second;
		for(int i = 0;i < (int)kQuestHelpList.m_QuestHelp.size();i++)
		{
			QuestHelp *pQuestHelp = kQuestHelpList.m_QuestHelp[i];			
			if( !pQuestHelp )
				continue;
			if( pQuestHelp->m_pImage )
			{		
				pQuestHelp->m_pImage->Render( iXPos, ( iYPos + (i * YGAP) ) - ( pQuestHelp->m_pImage->GetWidth() / 2) );
			}
			pQuestHelp->m_szHelp.PrintFullText( iXPos + pQuestHelp->m_iOffSetX, iYPos + (i * YGAP) + pQuestHelp->m_iOffSetY, TAT_CENTER );			
		}
	}
}

void ioQuestManager::Process()
{
	if( !m_pPlayStage ) return;

	ioQuest *pQuest = g_MyInfo.GetQuest();
	if( !pQuest ) return;

	int iMaxProgress = pQuest->GetMaxQuest( QS_PROGRESS );
	for(int i = 0;i < iMaxProgress;i++)
	{
		 QuestData kQuestData = pQuest->GetProcessToArray( i );
		 QuestParent *pQuestParent = kQuestData.GetLinkQuest();
		 if( !pQuestParent ) continue;

		 pQuestParent->Process( m_pPlayStage );
	}
}

void ioQuestManager::ProcessChangeStage()
{
	ProcessCompleteReserve();   // 완료 예약된 퀘스트 처리

	ioQuest *pQuest = g_MyInfo.GetQuest();
	if( !pQuest ) return;

	int iMaxProgress = pQuest->GetMaxQuest( QS_PROGRESS );
	for(int i = 0;i < iMaxProgress;i++)
	{
		QuestData kQuestData = pQuest->GetProcessToArray( i );
		QuestParent *pQuestParent = kQuestData.GetLinkQuest();
		if( !pQuestParent ) continue;

		pQuestParent->ChangeStageInit();
	}
}

void ioQuestManager::ProcessCompleteReserve()
{
	if( m_CompleteReserve.empty() )
		return;

	vCompleteReserve kCopyReserve;
	kCopyReserve.resize( m_CompleteReserve.size() );
	std::copy( m_CompleteReserve.begin(), m_CompleteReserve.end(), kCopyReserve.begin() );

	vCompleteReserve::iterator iter = kCopyReserve.begin();
	for(;iter != kCopyReserve.end();++iter)
	{
		CompleteReserve &kReserve = *iter;
		QuestCompleteTerm( kReserve.m_eComplete, kReserve.m_iValue1, kReserve.m_iValue2, kReserve.m_iValue3 );
	}
	kCopyReserve.clear();
	m_CompleteReserve.clear();
}

bool ioQuestManager::IsOwnerCharExtraItem( DWORD dwItemCode )
{
	if( !m_pPlayStage ) return false;

	ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
	if( !pOwnerChar ) return false;

	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		ioItem *pItem = pOwnerChar->GetEquipedItem( i );
		if( pItem && pItem->GetItemCode() == dwItemCode )
			return true;
	}
	return false;
}