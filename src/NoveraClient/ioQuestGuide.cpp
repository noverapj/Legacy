
#include "stdafx.h"

#include "ioQuestGuide.h"

#include "GUI/ManualWnd.h"
#include "GUI/NewShopWnd.h"
#include "GUI/MyInventoryWnd.h"
#include "GUI/SoldierPracticeWnd.h"
#include "GUI/PartyMenuWnd.h"
#include "GUI/PartyVersusWnd.h"
#include "GUI/LobbyMainWnd.h"
#include "GUI/PlazaMainWnd.h"
#include "GUI/LadderTeamWnd.h"
#include "GUI/HelpWnd.h"
#include "GUI/QuestWnd.h"
#include "GUI/QuickLobbyWnd.h"
#include "GUI/TrainingFuncWnd.h"

template<> ioQuestGuide* Singleton< ioQuestGuide >::ms_Singleton = 0;
ioQuestGuide::ioQuestGuide()
{
	m_pGuideFrm     = NULL;
	m_pGuideFrmTail = NULL;
	m_pQuestIconBack= NULL;
	m_pQuestInfoBtn = NULL;
	m_pCostumeMark = NULL;

	m_bMouseOverView = false;
}

ioQuestGuide::~ioQuestGuide()
{
	ClearResource();
}

ioQuestGuide& ioQuestGuide::GetSingleton()
{
	return Singleton< ioQuestGuide >::GetSingleton();
}

void ioQuestGuide::ClearResource()
{
	SAFEDELETE( m_pGuideFrm );
	SAFEDELETE( m_pGuideFrmTail );
	SAFEDELETE( m_pQuestIconBack );
	SAFEDELETE( m_pQuestInfoBtn );
	SAFEDELETE( m_pCostumeMark );

	m_GuideList.clear();
}

void ioQuestGuide::LoadINIData()
{
	ClearResource();

	ioINILoader_e kLoader( "config/sp2_quest_guide.ini" );	
	{
		kLoader.SetTitle_e( "Resource" );
		char szBuf[MAX_PATH] = "";
		kLoader.LoadString_e( "MainFrame", "", szBuf, sizeof( szBuf ) );
		m_pGuideFrm = g_GUIMgr.CreateFrame( szBuf );
		if( m_pGuideFrm )
		{
			m_pGuideFrm->SetSize( 172, 55 );
		}

		kLoader.LoadString_e( "FrameTail", "", szBuf, sizeof( szBuf ) );
		m_pGuideFrmTail = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		kLoader.LoadString_e( "IconBack", "", szBuf, sizeof( szBuf ) );
		m_pQuestIconBack = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		kLoader.LoadString_e( "InfoBtn", "", szBuf, sizeof( szBuf ) );
		m_pQuestInfoBtn = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		kLoader.LoadString_e( "CostumeMark", "", szBuf, sizeof( szBuf ) );
		m_pCostumeMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	}

	kLoader.SetTitle_e( "Common" );
	int iMaxGuide = kLoader.LoadInt_e( "MaxGuide", 0 );
	{
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		for(int i = 0;i < iMaxGuide;i++)
		{
			sprintf_e( szKey, "Guide%d", i + 1 );
			kLoader.SetTitle( szKey );

			DWORD dwMainIndex = kLoader.LoadInt_e( "MainIndex", 0 );
			DWORD dwSubIndex  = kLoader.LoadInt_e( "SubIndex", 0 );

			QuestParent *pQuestParent = g_QuestMgr.GetQuest( dwMainIndex, dwSubIndex );
			if( !pQuestParent )
			{
				if( dwMainIndex != 0 )
					LOG.PrintTimeAndLog( 0, "ioQuestGuide::LoadINIData : None Quest(%d,%d)", dwMainIndex, dwSubIndex );
				continue;
			}

			GuideData kData;
			kData.m_pQuestParent = pQuestParent;
			
			int iMaxData = kLoader.LoadInt_e( "MaxGuideData", 0 );
			for(int j = 0;j < iMaxData;j++)
			{
				sprintf_e( szKey, "GuideData%d", j + 1 );
				kLoader.LoadString( szKey, "", szBuf, sizeof( szBuf ) );

				GuideElement kElement;
				kElement.m_Element = ioStringConverter::Split( szBuf, "." );
				kData.m_ElementList.push_back( kElement );
			}
			m_GuideList.push_back( kData );
		}
	}
//	LOG.PrintTimeAndLog( 0, "Max Guide : %d", (int)m_GuideList.size() );
}

void ioQuestGuide::CreateGuideWnd()
{
	int iMaxGuide = m_GuideList.size();
	for(int i = 0;i < iMaxGuide;i++)
	{
		GuideData &rkData = m_GuideList[i];
		if( !rkData.m_pQuestParent ) continue;

		rkData.m_pGuideWnd = g_GUIMgr.AddWnd( "XML/questguidewnd.xml" );
		if( rkData.m_pGuideWnd )
		{
			rkData.m_pGuideWnd->SetID( QUEST_GUIDE_1 + i );
			QuestGuideWnd *pQuestGuideWnd = dynamic_cast< QuestGuideWnd * >( rkData.m_pGuideWnd );
			pQuestGuideWnd->SetQuestParent( rkData.m_pQuestParent );
		}
	}
}

double ioQuestGuide::GetAngle( int iStartX, int iStartY, int iEndX, int iEndY )
{
	double temp_angle;
	temp_angle = RADtoDEG( atan2( (double)(iEndY - iStartY), (double)(iEndX - iStartX) ) );	

	if( temp_angle < 0 )
		temp_angle *= -1;
	else
		temp_angle = 360 - temp_angle;

	if(temp_angle > 360)
		temp_angle -= 360;

	if( !COMPARE( temp_angle, 0, 360 ) )
		temp_angle = 0;
	return temp_angle;
}

void ioQuestGuide::ProcessGuideMove()
{
	int iMaxGuide = m_GuideList.size();
	for(int i = 0;i < iMaxGuide;i++)
	{
		GuideData &rkData = m_GuideList[i];
		if( !rkData.m_pQuestParent ) continue;
		if( !rkData.m_bRender ) continue;
		if( rkData.m_dwCurrentTime == 0 ) continue;
		
		DWORD dwGapTime = FRAMEGETTIME() - rkData.m_dwCurrentTime;
		if( dwGapTime < 200 )
		{
			int iDist = (int)( (rkData.m_fArrivalXPos - rkData.m_fViewXPos )*(rkData.m_fArrivalXPos - rkData.m_fViewXPos ) + 
				  		       (rkData.m_fArrivalYPos - rkData.m_fViewYPos )*(rkData.m_fArrivalYPos - rkData.m_fViewYPos ) );		
			iDist = sqrt( (double)iDist ) / 50;

			double fAngle =  GetAngle( rkData.m_fViewXPos, rkData.m_fViewYPos, rkData.m_fArrivalXPos, rkData.m_fArrivalYPos );
			rkData.m_fViewXPos += (double)dwGapTime / 200 * iDist * cos( fAngle );
			rkData.m_fViewYPos -= (double)dwGapTime / 200 * iDist * sin( fAngle );
		}
		else
		{
			rkData.m_fViewXPos = rkData.m_fArrivalXPos;
			rkData.m_fViewYPos = rkData.m_fArrivalYPos;
			rkData.m_dwCurrentTime = 0;
		}
	}
}

void ioQuestGuide::ProcessGuide()
{
	int iMaxGuide = m_GuideList.size();
	for(int i = 0;i < iMaxGuide;i++)
	{
		GuideData &rkData = m_GuideList[i];
		rkData.m_bRender  = false;
		rkData.m_dwWndID  = 0;
		
		if( !rkData.m_pQuestParent ) continue;
		if( !rkData.m_pQuestParent->IsQuestGuideShow() ) continue;

		float fPrevArrivalXPos = rkData.m_fArrivalXPos;
		float fPrevArrivalYPos = rkData.m_fArrivalYPos;
		vGuideElement::reverse_iterator riter = rkData.m_ElementList.rbegin();
		for(;riter != rkData.m_ElementList.rend();riter++)
		{
			if( ParseGuide( *riter, rkData.m_fArrivalXPos, rkData.m_fArrivalYPos, rkData.m_dwWndID ) )
			{
				rkData.m_bRender = true;
				if( rkData.m_fViewXPos == -999.0f || rkData.m_fViewYPos == -999.0f )
				{
					rkData.m_fViewXPos = rkData.m_fArrivalXPos;
					rkData.m_fViewYPos = rkData.m_fArrivalYPos;
					rkData.m_dwCurrentTime = 0;
				}
				else if( fPrevArrivalXPos != rkData.m_fArrivalXPos || fPrevArrivalYPos != rkData.m_fArrivalYPos )
				{
					rkData.m_dwCurrentTime = FRAMEGETTIME();
				}
				break;
			}
		}

		if( rkData.m_bRender )
		{
			// 동일한 위치에 있으면 가이드 표시 안함
			for(int j = 0;j < i;j++)
			{
				GuideData &rkCheckData = m_GuideList[j];
				if( !rkCheckData.m_bRender ) continue;
				if( rkCheckData.m_fViewXPos == rkData.m_fViewXPos &&
					rkCheckData.m_fViewYPos == rkData.m_fViewYPos )
				{
					rkData.m_bRender = false;
				}
			}
		}

		if( !rkData.m_bRender )
		{
			rkData.m_fViewXPos = rkData.m_fViewYPos = -999.0f;
		}
	}

	ProcessGuideMove();
	ProcessGuideWnd();
}

void ioQuestGuide::ProcessGuideWnd()
{
	bool bNoRenderGuide = g_GUIMgr.IsShow( PRACTICE_HELP_WND );
	int iMaxGuide = m_GuideList.size();
	for(int i = 0;i < iMaxGuide;i++)
	{
		GuideData &rkData = m_GuideList[i];

		rkData.m_bInfoScreen = false;
		if( !rkData.m_pQuestParent ) continue;
		if( !rkData.m_pGuideWnd ) continue;

		ErrorReport::SetPosition( 1400, 1 );
		if( bNoRenderGuide || !rkData.m_pQuestParent->IsQuestGuideShow() || !rkData.m_bRender )
		{
			ErrorReport::SetPosition( 1400, 2 );
			rkData.m_pGuideWnd->HideWnd();
			ErrorReport::SetPosition( 1400, 3 );
		}
		else
		{			
			ErrorReport::SetPosition( 1400, 4 );
			rkData.m_pGuideWnd->ShowWnd();

			ErrorReport::SetPosition( 1400, 5 );
			int iXPos = rkData.m_fViewXPos;
			int iYPos = rkData.m_fViewYPos;

			if( !m_pGuideFrm )
				continue;
			//
			const int iCutWidth = 14;
			int iFrameXPos = max( iCutWidth, iXPos - ( m_pGuideFrm->GetWidth() / 2 ) );
			int iFrameYPos = iYPos - 54;	
			if( iFrameXPos + m_pGuideFrm->GetWidth() > Setting::Width() - iCutWidth )        // 
				iFrameXPos = (Setting::Width() - iCutWidth) - m_pGuideFrm->GetWidth();

			rkData.m_pGuideWnd->SetWndPos( iFrameXPos + 155, iFrameYPos + 3 );
			ErrorReport::SetPosition( 1400, 6 );

			QuestGuideWnd *pQuestGuideWnd = dynamic_cast< QuestGuideWnd * >( rkData.m_pGuideWnd );
			rkData.m_bInfoScreen = pQuestGuideWnd->IsButtonScreen();

			ErrorReport::SetPosition( 1400, 7 );
			g_GUIMgr.MoveZorderLayer( rkData.m_dwWndID, rkData.m_pGuideWnd->GetID() );
			ErrorReport::SetPosition( 1400, 8 );
		}
	}
}

bool ioQuestGuide::ParseGuide( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	if( rkElement.m_Element.empty() )
		return false;

	if( ParseGuideFun( rkElement, rfXPos, rfYPos, rWndID ) )
		return true;
	else
	{
		DWORD dwID = atoi( rkElement.m_Element[0].c_str() );
		if( dwID == 0 ) return false;

		ioWnd *pParentWnd = g_GUIMgr.FindWnd( dwID );
		if( !pParentWnd ) return false;
		if( !pParentWnd->IsShow() ) return false;

		rWndID = dwID;
		rfXPos  = pParentWnd->GetDerivedPosX() + ( pParentWnd->GetWidth() / 2 );
		rfYPos  = pParentWnd->GetDerivedPosY();

		int iSize = rkElement.m_Element.size();
		for(int i = 1;i < iSize;i++)
		{
			ioWnd *pWnd = pParentWnd->FindChildWnd( atoi( rkElement.m_Element[i].c_str() ) );
			if( !pWnd ) return false;
			if( !pWnd->IsShow() ) return false;

			pParentWnd = pWnd;
			rfXPos = pParentWnd->GetDerivedPosX() + ( pParentWnd->GetWidth() / 2 );
			rfYPos = pParentWnd->GetDerivedPosY();
		}
		return true;
	}
	return false;
}

bool ioQuestGuide::ParseGuideFun( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	if( rkElement.m_Element[0] == "Manual" )
		return ParseManual( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "Shop" )
		return ParseShop( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "MyItem" )
		return ParseMyItem( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "Practice" )
		return ParsePractice( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "QuickStart" )
		return ParseQuickStart( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "BattleSelect" )
		return ParseBattleSelect( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "BattleList" )
		return ParseBattleList( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "BattleCreate" )
		return ParseBattleCreate( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "PlazaCreate" ) 
		return ParsePlazaCreate( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "CampCreate" )
		return ParseCampCreate( rkElement, rfXPos, rfYPos, rWndID );
	else if( rkElement.m_Element[0] == "BottomBar" )
		return ParseBottomBar( rkElement, rfXPos, rfYPos, rWndID );
	return false;
}

bool ioQuestGuide::ParseManual( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	ManualListWnd *pManualListWnd = dynamic_cast<ManualListWnd*>(g_GUIMgr.FindWnd( MANUAL_LIST_WND ));
	if( !pManualListWnd )
		return false;
	if( !pManualListWnd->IsShow() )
		return false;	
	if( rkElement.m_Element.size() < 2 )      // 인자는 한개만 사용함 : 공략 Index
		return false;

	if( pManualListWnd->QuestGuide( atoi( rkElement.m_Element[1].c_str() ), rfXPos, rfYPos ) )
	{
		rWndID = pManualListWnd->GetID();
		return true;
	}	
	return false;
}

bool ioQuestGuide::ParseShop( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	NewShopWnd *pNewShop = dynamic_cast<NewShopWnd *>(g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( !pNewShop ) return false;
	if( !pNewShop->IsShow() ) return false;
	if( rkElement.m_Element.size() < 8 ) return false; // 인자 7개 사용 : MainTab ID, SubTab ID, Btn( Type, MainCode, SubCode, PriceID, BuyID )

	IntVec vValue;
	for(int i = 1;i < 8;i++)
		vValue.push_back( atoi( rkElement.m_Element[i].c_str() ) );
	
	if( pNewShop->QuestGuide( vValue[0], vValue[1], vValue[2], vValue[3], vValue[4], vValue[5], vValue[6], rfXPos, rfYPos ) )
	{
		rWndID = pNewShop->GetID();
		return true;
	}
	return false;
}

bool ioQuestGuide::ParseMyItem( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	MyInventoryWnd *pMyItem = dynamic_cast<MyInventoryWnd *>(g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	if( !pMyItem ) return false;
	if( !pMyItem->IsShow() ) return false;
	if( rkElement.m_Element.size() < 9 ) return false; // 인자 8개 사용 : MainTab ID, SubTab ID, Btn( Type, MainCode, SubCode1, SubCode2, SubCode3, SubCode4 )

	IntVec vValue;
	for(int i = 1;i < 9;i++)
		vValue.push_back( atoi( rkElement.m_Element[i].c_str() ) );

	if( pMyItem->QuestGuide( vValue, rfXPos, rfYPos ) )
	{
		rWndID = pMyItem->GetID();
		return true;
	}
	return false;
}

bool ioQuestGuide::ParsePractice( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	SoldierPracticeSelectWnd *pPracticeWnd = dynamic_cast<SoldierPracticeSelectWnd *>(g_GUIMgr.FindWnd( SOLDIER_PRACTICE_SELECT_WND ) );
	if( !pPracticeWnd ) return false;
	if( !pPracticeWnd->IsShow() ) return false;
	if( rkElement.m_Element.size() < 2 ) return false; // 인자 1개 사용 : 용병 타입

	int iClassType = atoi( rkElement.m_Element[1].c_str() );
	if( pPracticeWnd->QuestGuide( iClassType, rfXPos, rfYPos ) )
	{
		rWndID = pPracticeWnd->GetID();
		return true;
	}
	return false;
}

bool ioQuestGuide::ParseQuickStart( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	BattleQuickStartOption *pSelectWnd = dynamic_cast<BattleQuickStartOption*>(g_GUIMgr.FindWnd( BATTLE_QUICKSTART_OPTION ));
	if( !pSelectWnd ) return false;
	if( !pSelectWnd->IsShow() ) return false;
	if( rkElement.m_Element.size() < 2 ) return false; // 인자 1개 사용 : 용병 타입

	int iModeIndex = atoi( rkElement.m_Element[1].c_str() );
	if( pSelectWnd->QuestGuide( iModeIndex, rfXPos, rfYPos ) )
	{
		rWndID = pSelectWnd->GetID();
		return true;
	}
	return false;
}

bool ioQuestGuide::ParseBattleSelect( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	PartyShamBattleWnd *pShamBattleWnd = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
	if( !pShamBattleWnd ) return false;
	if( !pShamBattleWnd->IsShow() ) return false;
	if( rkElement.m_Element.size() < 3 ) return false; // 인자 2개 사용 : 모드 인덱스, 맵 인덱스

	int iModeSelectIndex = atoi( rkElement.m_Element[1].c_str() );
	int iMapSelectIndex  = atoi( rkElement.m_Element[2].c_str() );
	if( pShamBattleWnd->QuestGuide( iModeSelectIndex, iMapSelectIndex, rfXPos, rfYPos ) )
	{
		rWndID = pShamBattleWnd->GetID();
		return true;
	}
	return false;
}

bool ioQuestGuide::ParseBattleList( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	if( rkElement.m_Element.size() < 2 ) // 인자 1개 사용 : 모드 인덱스
		return false; 

	int iModeSelectIndex = atoi( rkElement.m_Element[1].c_str() );

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
		if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
		{
			if( pLobbyMainWnd->QuestGuide( iModeSelectIndex, rfXPos, rfYPos ) )
			{
				rWndID = pLobbyMainWnd->GetID();
				return true;
			}
		}

		TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
		if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
		{
			if( pTrainingFuncWnd->QuestGuide( iModeSelectIndex, rfXPos, rfYPos ) )
			{
				rWndID = pTrainingFuncWnd->GetID();
				return true;
			}
		}
		return false;
	}
	else
	{
		QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
		if( !pWnd )
			return false;

		if( !pWnd->IsShow() )
			return false;

		if( pWnd->QuestGuide( iModeSelectIndex, rfXPos, rfYPos ) )
		{
			rWndID = pWnd->GetID();
			return true;
		}
	}

	return false;
}

bool ioQuestGuide::ParseBattleCreate( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	BattleQuickStartOption *pSelectWnd = dynamic_cast<BattleQuickStartOption*>(g_GUIMgr.FindWnd( BATTLE_QUICKSTART_OPTION ));
	if( !pSelectWnd ) return false;
	if( (pSelectWnd->IsShow()) && (pSelectWnd->GetKindWnd() == BattleQuickStartOption::E_CREATEBATTLE))
	{
		if( rkElement.m_Element.size() < 2 ) return false; // 인자 1개 사용 : 용병 타입

		int iModeIndex = atoi( rkElement.m_Element[1].c_str() );
		if( pSelectWnd->QuestGuide( iModeIndex, rfXPos, rfYPos ) )
		{
			rWndID = pSelectWnd->GetID();
			return true;
		}
		return false;
	}

	PartyCreateWnd *pPartyCreateWnd = dynamic_cast<PartyCreateWnd*>(g_GUIMgr.FindWnd( PARTY_CREATE_WND ));
	if( !pPartyCreateWnd ) return false;
	if( !pPartyCreateWnd->IsShow() ) return false;
	if( rkElement.m_Element.size() < 2 ) return false; // 인자 1개 사용 : 모드 인덱스

	int iModeIndex = atoi( rkElement.m_Element[1].c_str() );
	if( pPartyCreateWnd->QuestGuide( iModeIndex, rfXPos, rfYPos ) )
	{
		rWndID = pPartyCreateWnd->GetID();
		return true;
	}
	return false;
}

bool ioQuestGuide::ParsePlazaCreate( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	PlazaCreateWnd *pPlazaCreateWnd = dynamic_cast<PlazaCreateWnd*>(g_GUIMgr.FindWnd( PLAZA_CREATE_WND ));
	if( !pPlazaCreateWnd ) return false;
	if( !pPlazaCreateWnd->IsShow() ) return false;
	if( rkElement.m_Element.size() < 3 ) return false; // 인자 2개 사용 : 모드 인덱스, 맵 인덱스

	int iModeIndex = atoi( rkElement.m_Element[1].c_str() );
	int iMapIndex = atoi( rkElement.m_Element[2].c_str() );
	if( pPlazaCreateWnd->QuestGuide( iModeIndex, iMapIndex, rfXPos, rfYPos ) )
	{
		rWndID = pPlazaCreateWnd->GetID();
		return true;
	}
	return false;
}

bool ioQuestGuide::ParseCampCreate( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	LadderCreateWnd *pLadderCreateWnd = dynamic_cast<LadderCreateWnd*>(g_GUIMgr.FindWnd( LADDER_TEAM_CREATE_WND ));
	if( !pLadderCreateWnd ) return false;
	if( !pLadderCreateWnd->IsShow() ) return false;
	if( rkElement.m_Element.size() < 2 ) return false; // 인자 1개 사용 : 모드 인덱스

	int iModeIndex = atoi( rkElement.m_Element[1].c_str() );
	if( pLadderCreateWnd->QuestGuide( iModeIndex, rfXPos, rfYPos ) )
	{
		rWndID = pLadderCreateWnd->GetID();
		return true;
	}
	return false;
}

bool ioQuestGuide::ParseBottomBar( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID )
{
	HelpWnd *pHelpWnd = dynamic_cast<HelpWnd*>(g_GUIMgr.FindWnd( HELP_WND ));
	if( !pHelpWnd ) return false;
	if( !pHelpWnd->IsShow() ) return false;
	if( rkElement.m_Element.size() < 2 ) return false; // 인자 1개 사용 : 버튼 ID

	int iBtnID = atoi( rkElement.m_Element[1].c_str() );
	if( pHelpWnd->QuestGuide( iBtnID, rfXPos, rfYPos ) )
	{
		rWndID = pHelpWnd->GetID();
		return true;
	}
	return false;
}

void ioQuestGuide::RenderGuide( DWORD dwWndID )
{
	if( !m_pGuideFrm || !m_pGuideFrmTail || !m_pQuestIconBack )
		return;

	int iMaxGuide = m_GuideList.size();
	for(int i = 0;i < iMaxGuide;i++)
	{
		GuideData &rkData = m_GuideList[i];
		if( !rkData.m_bRender ) continue;
		if( rkData.m_dwWndID != dwWndID ) continue;

		RenderGuide( rkData );
	}

	RenderUI();
}

void ioQuestGuide::RenderGuide( const GuideData &rkData )
{
	if( !rkData.m_pQuestParent ) return;
    if( !rkData.m_pQuestParent->IsMeProgressQuest() ) return;

	int iXPos = rkData.m_fViewXPos;
	int iYPos = rkData.m_fViewYPos;
	
	if( !m_pGuideFrm || !m_pGuideFrmTail || !m_pQuestIconBack )
		return;

	const int iCutWidth = 14;
	int iFrameXPos = max( iCutWidth, iXPos - ( m_pGuideFrm->GetWidth() / 2 ) );
	int iFrameYPos = iYPos - 54;	
	if( iFrameXPos + m_pGuideFrm->GetWidth() > Setting::Width() - iCutWidth )        // 
		iFrameXPos = (Setting::Width() - iCutWidth) - m_pGuideFrm->GetWidth();
	m_pGuideFrm->Render( iFrameXPos, iFrameYPos );
	//
	m_pGuideFrmTail->Render( iXPos - ( m_pGuideFrmTail->GetWidth() / 2 ), iYPos - 1 );

	//
	m_pQuestIconBack->SetScale( 0.50f );
	m_pQuestIconBack->Render( iFrameXPos + 25, iFrameYPos + 27, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	ioUIRenderImage *pQuestIcon = rkData.m_pQuestParent->GetQuestIcon();
	if( pQuestIcon )
	{
		pQuestIcon->SetScale( 0.50f );
		pQuestIcon->Render( iFrameXPos + 25, iFrameYPos + 27, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// UI코스튬 관련 (코스튬 마크)
	if ( rkData.m_pQuestParent->IsCostume() && m_pCostumeMark )
	{
		m_pCostumeMark->SetScale( 0.50f );
		m_pCostumeMark->Render( iFrameXPos + 25, iFrameYPos + 27, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_pQuestInfoBtn )
	{
		m_pQuestInfoBtn->Render( iFrameXPos + 155, iFrameYPos + 3 );
		if( rkData.m_bInfoScreen )
		{
			m_pQuestInfoBtn->Render( iFrameXPos + 155, iFrameYPos + 3, UI_RENDER_SCREEN );
		}
	}

	RenderQuestTitle( iFrameXPos + 48, iFrameYPos + 8, rkData.m_pQuestParent, rkData.m_pQuestParent->GetProgressResultOnlyNumber().c_str() );
}

void ioQuestGuide::RenderQuestTitle( int iXPos, int iYPos, QuestParent *pQuestParent, const char *szResult )
{
	if( !pQuestParent ) return;

	// 한줄 & 두줄 체크
	float fLimitWidth = 114.0f;
	char szBuffer[MAX_PATH] = "";
	if( strlen( szResult ) == 0 )
		sprintf( szBuffer, "%s", pQuestParent->GetProgressText().c_str() );
	else
		sprintf( szBuffer, "%s %s", pQuestParent->GetProgressText().c_str(), szResult );

	if( g_FontMgr.GetTextWidth( szBuffer, TS_NORMAL, FONT_SIZE_11 ) <= fLimitWidth )
	{
		// 2줄짜리
		// 퀘스트 명
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 91, 55, 20 );
		g_FontMgr.PrintTextWidthCut( iXPos, iYPos + 6, FONT_SIZE_11, fLimitWidth, pQuestParent->GetTitle().c_str() );

		// 퀘스트 목적
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( 0, 0, 0 );
		kPrinter.AddTextPiece( FONT_SIZE_11, pQuestParent->GetProgressText().c_str() );
		if( strlen( szResult ) > 0 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(1), szResult );
		}
		kPrinter.PrintFullText( iXPos, iYPos + 20, TAT_LEFT );
	}
	else
	{
		// 3줄짜리
		// 퀘스트 명
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 91, 55, 20 );
		g_FontMgr.PrintTextWidthCut( iXPos, iYPos, FONT_SIZE_11, fLimitWidth, pQuestParent->GetTitle().c_str() );

		// 퀘스트 목적
		int iResultXPos = 0;
		sprintf( szBuffer, "%s", pQuestParent->GetProgressText().c_str() );	
		g_FontMgr.SetTextColor( 0, 0, 0 );		
		char szDst[MAX_PATH] = "";
		memset( szDst, 0, sizeof( szDst ) );
		int iLen = Help::StringCutFun( FONT_SIZE_11, fLimitWidth, TS_NORMAL, szDst, sizeof(szDst), szBuffer );
		g_FontMgr.PrintText( iXPos, iYPos + 14, FONT_SIZE_11, szDst );
		if( iLen < (int)strlen( szBuffer ) )
		{
			float fWidthCut = fLimitWidth - g_FontMgr.GetTextWidth( szResult, TS_NORMAL, FONT_SIZE_11 );
			g_FontMgr.PrintTextWidthCut( iXPos, iYPos + 28, FONT_SIZE_11, fWidthCut, &szBuffer[iLen] );
			iResultXPos = 4 + min( fWidthCut, g_FontMgr.GetTextWidth( &szBuffer[iLen], TS_NORMAL, FONT_SIZE_11 ) );
		}

		// 
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + iResultXPos, iYPos + 28, FONT_SIZE_11, szResult );
	}
}

void ioQuestGuide::RenderUI()
{
	if( !m_bMouseOverView ) return;

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	ioWnd *pCurrWnd = g_GUIMgr.GetPreOverWnd();

	char szUIText[MAX_PATH] = "";
	if( pCurrWnd == NULL )
		sprintf( szUIText, "[Parent:NULL][Over:NULL]" );
	else
	{
		if( pCurrWnd->GetParent() )
		{
			
			sprintf( szUIText, "[Parent:%d:%s][Over:%d:%s]", pCurrWnd->GetParent()->GetID(), pCurrWnd->GetParent()->GetClassNameEX(), pCurrWnd->GetID(), pCurrWnd->GetClassNameEX() );
		}
		else
		{
			sprintf( szUIText, "[Parent:NULL:NULL][Over:%d:%s]", pCurrWnd->GetID(), pCurrWnd->GetClassNameEX() );
		}
	}

	int iXPos = pMouse->GetMousePos().x;
	int iYPos = pMouse->GetMousePos().y - 20;

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_17, szUIText );
}