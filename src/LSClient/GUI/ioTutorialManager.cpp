#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioSoundManager.h"

#include "../NetworkWrappingFunc.h"
#include "../ioBaseChar.h"
#include "../ioMyInfo.h"
#include "../WndID.h"
#include "../ioClassPrice.h"
#include "../ioComplexStringPrinter.h"
#include "../ioMyRoomMode.h"
#include "../ioNpcDefine.h"
#include "../EtcHelpFunc.h"
#include "../ioQuestManager.h"

#include "ioSP2GUIManager.h"
#include "TutorialWnd.h"
#include "ioTutorialManager.h"
#include "ioMessageBox.h"
#include "HelpWnd.h"

template<> ioTutorialManager* Singleton< ioTutorialManager >::ms_Singleton = 0;

ioTutorialManager::StepList::StepList()
{
	m_iMaxValue = 0;
}

ioTutorialManager::StepList::~StepList()
{
}

ioTutorialManager::Tutorial::Tutorial()
{

}

ioTutorialManager::Tutorial::~Tutorial()
{
	vStepList::iterator iter;
	for( iter=m_vStepList.begin() ; iter!=m_vStepList.end() ; ++iter )
	{
		StepList *pItem = *iter;
		SAFEDELETE( pItem );
	}
	m_vStepList.clear();
}

ioTutorialManager::ioTutorialManager()
{
	m_pTutorial		= NULL;
	m_iTutorialState= US_TUTORIAL_CLEAR;
	m_iCurStep	= 0;
	m_iCurValue = 0;

	m_bRandomItemDrop  = false;
	m_bCharInitPos     = false;
	m_bCharDropStepPos = false;
	m_bEnableMagicCircleMoving = false;
}

ioTutorialManager::~ioTutorialManager()
{
	ClearList();
}

void ioTutorialManager::ClearList()
{
	vTutorialList::iterator iter;
	for( iter=m_vTutorialList.begin() ; iter!=m_vTutorialList.end() ; ++iter )
	{
		Tutorial *pItem = *iter;
		SAFEDELETE( pItem );
	}
	m_vTutorialList.clear();
}

void ioTutorialManager::LoadTutorialList()
{
	ClearList();

	ioINILoader_e kLoader( "config/sp2_tutorial.ini" );

	kLoader.SetTitle_e( "Info" );
	int iMaxSet = kLoader.LoadInt_e( "MaxSet", 0 );

	char szBuf[MAX_PATH];
	for(int i = 0;i < iMaxSet;i++)
	{
		sprintf_e( szBuf, "Set%d", i+1 );
		kLoader.SetTitle( szBuf );

		Tutorial *pTutorial = new Tutorial;
		pTutorial->m_iSetCode = kLoader.LoadInt_e( "set_code", 0 );
		pTutorial->m_iMaxStep = kLoader.LoadInt_e( "max_step", 0 );

		char szString[MAX_PATH];
		for(int k = 0;k < pTutorial->m_iMaxStep;k++)
		{
			StepList *pStep = new StepList;
			sprintf_e( szBuf, "step%d_title", k );
			kLoader.LoadString( szBuf, "", szString, MAX_PATH );			
			pStep->m_szTitle = szString;
			sprintf_e( szBuf, "step%d_help1", k );
			kLoader.LoadString( szBuf, "", szString, MAX_PATH );
			pStep->m_szHelp1 = szString;
			sprintf_e( szBuf, "step%d_help2", k );
			kLoader.LoadString( szBuf, "", szString, MAX_PATH );
			pStep->m_szHelp2 = szString;
			sprintf_e( szBuf, "step%d_complete", k );
			kLoader.LoadString( szBuf, "", szString, MAX_PATH );
			pStep->m_szComplete = szString;
			sprintf_e( szBuf, "step%d_combo1", k );
			kLoader.LoadString( szBuf, "", szString, MAX_PATH );
			pStep->m_szCombo1 = szString;
			sprintf_e( szBuf, "step%d_combo2", k );
			kLoader.LoadString( szBuf, "", szString, MAX_PATH );
			pStep->m_szCombo2 = szString;
			pStep->m_iComboCount = 0;
			sprintf_e( szBuf, "step%d_max_value", k );
			pStep->m_iMaxValue = kLoader.LoadInt( szBuf, 0 );

			pTutorial->m_vStepList.push_back( pStep );
		}
		m_vTutorialList.push_back( pTutorial );
	}
}

void ioTutorialManager::SetDataGUI()
{
	if( m_pTutorial == NULL ) return;

	int iMaxStep = m_pTutorial->m_vStepList.size();
	if( !g_MyInfo.IsTutorialUser() )
		iMaxStep -= 1;

	if( !COMPARE( m_iCurStep, 0, iMaxStep ) ) return;

	StepList *pStep = m_pTutorial->m_vStepList[m_iCurStep];
	TutorialWnd *pTutorialWnd = dynamic_cast<TutorialWnd*>(g_GUIMgr.FindWnd( TUTORIAL_WND ));
	if( pTutorialWnd )
	{
		pTutorialWnd->SetTutorialStep( m_iCurStep, m_iCurValue, iMaxStep, pStep->m_iMaxValue, 
									   pStep->m_szTitle, pStep->m_szHelp1, pStep->m_szHelp2, pStep->m_szComplete );
		if( !pTutorialWnd->IsShow() )
			pTutorialWnd->ShowWnd();		
	}		
}

void ioTutorialManager::SetSound( const ioHashString &szCountUpSnd,
								  const ioHashString &szStartSnd,
								  const ioHashString &szNextSnd,
								  const ioHashString &szClearSnd )
{
	m_szCountUpSnd = szCountUpSnd;
	m_szStartSnd   = szStartSnd;
	m_szNextSnd	   = szNextSnd;
	m_szClearSnd   = szClearSnd;
}

void ioTutorialManager::InitTutorial( int iState )
{
	m_iTutorialState = iState;
	if( m_iTutorialState == US_TUTORIAL_CLEAR )
	{
		g_GUIMgr.HideWnd( TUTORIAL_WND );
		return;
	}

	int iMySetCode = m_iTutorialState / 100;
	m_iCurStep     = m_iTutorialState % 100;
	m_iCurValue    = 0;

	vTutorialList::iterator iter;
	for( iter=m_vTutorialList.begin() ; iter!=m_vTutorialList.end() ; ++iter )
	{
		Tutorial *pItem = *iter;
		if( pItem->m_iSetCode == iMySetCode )
		{
			m_pTutorial = pItem;
			break;
		}
	}

	SetDataGUI();

/*	if( m_iCurStep == STEP_6 )
		m_bRandomItemDrop = true;
	else if( m_iCurStep == STEP_14 )
		m_bCharDropStepPos = true;
	else if( m_iCurStep == STEP_16 )
		OnClear( false );
*/
	m_bCharInitPos    = false;
}

bool ioTutorialManager::IsTutorial()
{
	if( m_iTutorialState == US_TUTORIAL_CLEAR )
		return false;
	return true;
}

void ioTutorialManager::CountUpTutorial()
{
	StepList *pStep = m_pTutorial->m_vStepList[m_iCurStep];
	pStep->m_iComboCount = 0;
	if( m_iCurValue < pStep->m_iMaxValue )
	{
		m_iCurValue++;
		SetDataGUI();
		g_SoundMgr.PlaySound( m_szCountUpSnd );
	}
}

void ioTutorialManager::OnStart()
{
	if( m_pTutorial == NULL ) return;

	m_iCurStep++;
	m_iCurValue = 0;
	SetDataGUI();	
	if( m_iTutorialState == g_MyInfo.GetUserState() )
	{
		g_MyInfo.SetUserState( g_MyInfo.GetUserState() + 1 );
		SP2Packet kPacket( CTPK_TUTORIAL_STEP );
		kPacket << TUTORIAL_STEP_NEXT << g_MyInfo.GetUserState() << m_pTutorial->m_iMaxStep;
		TCPNetwork::SendToServer( kPacket );		
	}	
	m_iTutorialState++;
	g_SoundMgr.PlaySound( m_szStartSnd );
}

void ioTutorialManager::OnNext()
{
	if( m_pTutorial == NULL ) return;

	m_iCurStep++;
    m_iCurValue = 0;	
	SetDataGUI();
	
	if( m_iTutorialState == g_MyInfo.GetUserState() )
	{
		g_MyInfo.SetUserState( g_MyInfo.GetUserState() + 1 );
		SP2Packet kPacket( CTPK_TUTORIAL_STEP );
		kPacket << TUTORIAL_STEP_NEXT << g_MyInfo.GetUserState() << m_pTutorial->m_iMaxStep;
		TCPNetwork::SendToServer( kPacket );
	}
	m_iTutorialState++;	
	g_SoundMgr.PlaySound( m_szNextSnd );

/*	switch( m_iCurStep )
	{
	case STEP_6:
		m_bRandomItemDrop = true;
		break;
	case STEP_7:
	case STEP_15:
		m_bCharInitPos = true;
		break;
	case STEP_14:
		m_bCharDropStepPos = true;
		break;
	case STEP_16:
		OnClear( false );
		break;
	}
*/
}

void ioTutorialManager::OnClear( bool bSound /*= true */ )
{
	if( m_pTutorial == NULL ) return;

	if( m_iTutorialState == g_MyInfo.GetUserState() )
	{
		g_MyInfo.SetUserState( US_TUTORIAL_CLEAR );
		SP2Packet kPacket( CTPK_TUTORIAL_STEP );
		kPacket << TUTORIAL_STEP_NEXT << g_MyInfo.GetUserState() << m_pTutorial->m_iMaxStep;
		TCPNetwork::SendToServer( kPacket );

		g_QuestMgr.QuestCompleteTerm( QC_TUTORIAL_CLEAR );
	}
	m_iTutorialState = US_TUTORIAL_CLEAR;

	if( bSound )
		g_SoundMgr.PlaySound( m_szClearSnd );

	g_GUIMgr.ShowWnd( EXERCISE_SOLDIER_EVENT_WND );
}

void ioTutorialManager::OnAllSkip()
{
	if( m_pTutorial == NULL ) return;

	int iNextStep = m_iCurStep + 1;
	m_iCurStep  = STEP_5;
	m_iCurValue = 0;	
	SetDataGUI();

	if( m_iTutorialState == g_MyInfo.GetUserState() )
	{
		g_MyInfo.SetUserState( STEP_5 );
		SP2Packet kPacket( CTPK_TUTORIAL_STEP );
		kPacket << TUTORIAL_STEP_ALL_SKIP << g_MyInfo.GetUserState() << m_pTutorial->m_iMaxStep;
		TCPNetwork::SendToServer( kPacket );
	}
	m_iTutorialState = STEP_5; // ÈÆ·Ã¿Ï·áÀü

	g_SoundMgr.PlaySound( m_szNextSnd );
	OnClear( false ); 

/*	switch( iNextStep )
	{
	case STEP_6:
		m_bRandomItemDrop = true;
		break;
	case STEP_7:
	case STEP_15:
		m_bCharInitPos = true;
		break;
	case STEP_14:
		m_bCharDropStepPos = true;
		break;
	}
*/
}

bool ioTutorialManager::IsShowSkillStep()
{
	if( g_MyInfo.IsTutorialUser() )
	{
		if( m_iCurStep < STEP_5 )
			return false;
	}
	return true;
}

bool ioTutorialManager::IsShowExerciseStep()
{
	if( !IsTutorial() ) return true;
	if( !m_pTutorial ) return true;

/*	if( m_iCurStep < STEP_15 )
		return false;
*/
	return true;
}

bool ioTutorialManager::StartRandomItemDrop()
{
	if( !m_pTutorial ) return false;

	bool bReturn = m_bRandomItemDrop;
	m_bRandomItemDrop = false;
	return bReturn;
}

bool ioTutorialManager::StartCharInitPos()
{
	if( !m_pTutorial ) return false;

	bool bReturn = m_bCharInitPos;
	m_bCharInitPos = false;
	return bReturn;
}

bool ioTutorialManager::StartCharDropStepPos()
{
	if( !m_pTutorial ) return false;

	bool bReturn = m_bCharDropStepPos;
	m_bCharDropStepPos = false;
	return bReturn;
}

void ioTutorialManager::ActionCharacter( int iState )
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;

	switch( iState )
	{
	case CS_DASH_ATTACK:
		{
			if( m_iCurStep == STEP_3 )
				CountUpTutorial();			
		}
		break;
	case CS_DEFENSE:		
	case CS_WOUNDED_DEFENSE:
	case CS_ATTACK:
	case CS_JUMP:
		{
			if( m_iCurStep == STEP_1 )
				CountUpTutorial();
		}
		break;
	}
}

void ioTutorialManager::ActionJumpAttack()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;

	if( m_iCurStep == STEP_4 ) 
		CountUpTutorial();
}

void ioTutorialManager::ActionChargeAttack()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;

	if( m_iCurStep == STEP_2 )
		CountUpTutorial();
}

void ioTutorialManager::ActionDoubleJump()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;	

	if( m_iCurStep == STEP_4 )
		CountUpTutorial();
}

void ioTutorialManager::ActionHighJump()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;

	if( m_iCurStep == STEP_4 ) 
		CountUpTutorial();
}

void ioTutorialManager::ActionChangeToAiming()
{
 	if( !IsTutorial() ) return;
 	if( !m_pTutorial ) return;
}

void ioTutorialManager::ActionAimed()
{
 	if( !IsTutorial() ) return;
 	if( !m_pTutorial ) return;
}

void ioTutorialManager::ActionZeroAimed()
{
 	if( !IsTutorial() ) return;
 	if( !m_pTutorial ) return;
}

void ioTutorialManager::ActionAimingCancel()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
}


void ioTutorialManager::ActionAimingMove()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
}

void ioTutorialManager::ActionRollingDash()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;

	if( m_iCurStep == STEP_3 )
		CountUpTutorial();
}

void ioTutorialManager::ActionSuperDash()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;

	if( m_iCurStep == STEP_3 )
		CountUpTutorial();
}

void ioTutorialManager::ActionDashSpear()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
}

void ioTutorialManager::ActionAssaultDash()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;

	if( m_iCurStep == STEP_3 )  
		CountUpTutorial();
}

void ioTutorialManager::ActionWeaponCharge()
{
 	if( !IsTutorial() ) return;
 	if( !m_pTutorial ) return;
}

void ioTutorialManager::ActionDefenseMove()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
}

void ioTutorialManager::ActionAimedFireDeveloperK( const ioHashString &rszPublicID )
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
	if( rszPublicID != Help::GetDeveloperKCharName() ) return;

	switch( m_pTutorial->m_iSetCode )
	{
	case ROBIN_SET_CODE:
	case SOLDIER_SET_CODE:
	case SPACE_SET_CODE:
	case WESTERN_SET_CODE:
		{
			if( m_iCurStep == STEP_2 ) 
				CountUpTutorial();
		}
		break;	
	}
}

void ioTutorialManager::ActionNormalFire()
{
 	if( !IsTutorial() ) return;
 	if( !m_pTutorial ) return;

	if( m_iCurStep == STEP_2 ) 
		CountUpTutorial();
}

void ioTutorialManager::ActionBombFire()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;

	if( m_iCurStep == STEP_2 )
		CountUpTutorial();
}

void ioTutorialManager::ActionMineBurying()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
}

int ioTutorialManager::ActionUseSkill( DWORD dwSetItemCode, const ioHashString &szSkillName 
									   ,const ioTargetMarker::MarkerState eMarkerState /*= ioTargetMarker::MS_NONE*/ )
{
	if( !IsTutorial() ) return TSR_ERROR1;
	if( !m_pTutorial ) return TSR_ERROR2;
	if( dwSetItemCode - SET_ITEM_CODE != m_pTutorial->m_iSetCode ) return TSR_ERROR3;
	
	switch( m_pTutorial->m_iSetCode )
	{
	case IRON_SET_CODE:
	case HOOK_SET_CODE:
	case WESTERN_SET_CODE:
	case FIRE_SET_CODE:
	case MEDIC_SET_CODE:
	case SAVAGE_SET_CODE:
	case ROBIN_SET_CODE:
	case CRAZY_SET_CODE:
	case ICE_SET_CODE:
	case SOLDIER_SET_CODE:
	case SPACE_SET_CODE:
	case SHADOW_SET_CODE:
	case DEFENDER_SET_CODE:
	case BLUEDRAGON_SET_CODE:
		if( m_iCurStep == STEP_5 )
		{
			CountUpTutorial();
		}
		break;
	default:
		return TSR_ERROR4;
	}

	return TSR_OK;
}

void ioTutorialManager::ActionMagicCircleMoving()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
	if( !m_bEnableMagicCircleMoving ) return;

	if( m_iCurStep == STEP_2 ) 
	{
		CountUpTutorial();
		m_bEnableMagicCircleMoving = false; 
	}
}

void ioTutorialManager::ActionEnterPlaza()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
}

void ioTutorialManager::ActionTextTickEnd()
{
// 	if( IsShowShopWnd() )
// 	{
// 		ShopWnd *pShopWnd = dynamic_cast<ShopWnd*>(g_GUIMgr.FindWnd( SHOP_WND ));
// 		if( pShopWnd && !pShopWnd->IsShow() )
// 		{
// 			pShopWnd->SetTutorialUI();
// 			pShopWnd->ShowWnd();
// 		}			
// 	}
}

void ioTutorialManager::ActionPickItem()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
}

ioTutorialManager& ioTutorialManager::GetSingleton()
{
	return Singleton<ioTutorialManager>::GetSingleton();
}

void ioTutorialManager::ActionDropDevoloperK( DWORD dwCharIndex )
{
	if( dwCharIndex != NPC_INDEX ) return;
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
}

void ioTutorialManager::ActionBuyExercise()
{
	if( !IsTutorial() ) return;
	if( !m_pTutorial ) return;
}