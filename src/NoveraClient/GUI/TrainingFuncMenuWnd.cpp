#include "StdAfx.h"

#include "TrainingFuncMenuWnd.h"

#include "TrainingFuncWnd.h"
#include "GuildHQEtcItemNeedPopup.h"
#include "HomeEtcItemNeedPopup.h"
#include "../ioVoiceChat/Fourier.h"
#include "../ioEtcItem.h"
#include "../ioHouseMode.h"
#include "../ioPlayStage.h"
#include "../ioDateHelp.h"

TrainingFuncMenuWnd::TrainingFuncMenuWnd()
{
	m_iDefaultX			= 0;
	m_iDefaultY			= 0;

	m_bChangeResolution	= false;
	m_bStartAni			= false;

	m_pCurrMode			= NULL;

	InitShowAniState();
}

TrainingFuncMenuWnd::~TrainingFuncMenuWnd()
{
	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "TrainingFuncMenuWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "TrainingFuncMenuWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void TrainingFuncMenuWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "TrainingFuncMenuWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "TrainingFuncMenuWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && COMPARE( iYPos, 0, Setting::Height() ) )
		SetWndPos( iXPos, iYPos );

	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();

	m_bStartAni = false;
}

void TrainingFuncMenuWnd::iwm_show()
{
	m_bStartAni = true;
}

void TrainingFuncMenuWnd::iwm_hide()
{
	m_bStartAni = false;
}

void TrainingFuncMenuWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BATTLE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ShowTrainingFuncWnd( TrainingFuncWnd::ID_BATTLE );
		}
		break;
	case ID_PLAZA_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ShowTrainingFuncWnd( TrainingFuncWnd::ID_PLAZA );
		}
		break;
	case ID_TRADE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ShowTrainingFuncWnd( TrainingFuncWnd::ID_TRADE );
		}
		break;
	case ID_HEAD_QUATER_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ShowTrainingFuncWnd( ID_HEAD_QUATER_BTN );
		}
		break;
	case ID_HOME_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ShowTrainingFuncWnd( ID_HOME_BTN );
		}
		else if( cmd == IOEX_BTNUP )
		{
			HomeEtcItemNeedPopup* pWnd = dynamic_cast<HomeEtcItemNeedPopup*>( g_GUIMgr.FindWnd( HOUSE_NEED_POPUP ) );
			if( pWnd )
				pWnd->ShowWnd();
		}
		break;
	case ID_GUILD_HQ_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ShowTrainingFuncWnd( ID_GUILD_HQ_BTN );
		}
		else if( cmd == IOEX_BTNUP )
		{
			ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
			if( pGuildData )
			{				
				if( pGuildData->GetGuildMasterName() == g_MyInfo.GetPublicID() )
				{
					GuildHQEtcItemNeedPopup* pWnd = dynamic_cast<GuildHQEtcItemNeedPopup*>( g_GUIMgr.FindWnd( GUILD_HQ_NEED_POPUP ) );
					if( pWnd )
					{
						pWnd->SetType( GuildHQEtcItemNeedPopup::PUT_NEED_HQ );
						ioUserEtcItem* pInven = g_MyInfo.GetUserEtcItem();
						if( pInven )
						{
							ioUserEtcItem::ETCITEMSLOT Slot;
							if( pInven->GetEtcItem( ioEtcItem::EIT_ETC_CREATE_GUILD_HQ, Slot ) && Slot.m_iValue1 > 0 )
							{
								pWnd->SetType( GuildHQEtcItemNeedPopup::PUT_USE_HQ );
							}
						}
						pWnd->ShowWnd();
					}
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrainingFuncMenuWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	//////////////////////////////////////////////////////////////////////////
	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_NONE )
		m_ShowAniState.ChangeState( SAT_END  );

	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_START_ROUND )
	{
		if( g_TutorialMgr.IsShowSkillStep() )
		{
			m_ShowAniState.ChangeState( SAT_DELAY );
		}
	}

	if( g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_END )
		m_ShowAniState.ChangeState( SAT_NONE );

	m_ShowAniState.UpdateTime();

	if( m_ShowAniState.GetCurState() == SAT_UP )
	{
		enum { PLUS_Y_POS = 75 };
		int iStartYPos = PLUS_Y_POS + ( Setting::Height() - ( m_iDefaultY + GetHeight() ) );
		int iCurYpos = m_iDefaultY + ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * iStartYPos );	
		SetWndPos( m_iDefaultX, iCurYpos );
	}
	else if( m_ShowAniState.GetCurState() == SAT_DOWN )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY + ( sin( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
	}
	else if( m_ShowAniState.GetCurState() == SAT_UP_STOP )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY + ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
	}
	else if( m_ShowAniState.GetCurState() == SAT_STOP )
	{
		SetWndPos( m_iDefaultX, m_iDefaultY );
		m_ShowAniState.ChangeState( SAT_END );
	}

	if( SkipRender() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_bChangeResolution )
	{
		m_bChangeResolution = false;
		ResetWindow();
	}

	UpdateDefaultPos();
	UpdateHQButton();
	UpdateHomeButton();
	UpdateGuildHQButton();
}

void TrainingFuncMenuWnd::UpdateHQButton()
{
	SetChildWndStyleRemove( ID_HEAD_QUATER_BTN, IWS_INACTIVE );
	if( m_pCurrMode && m_pCurrMode->GetModeType() == MT_HEADQUARTERS )
	{
		ioHeadquartersMode* pMode = ToHeadquartersMode( m_pCurrMode );
		if( pMode && pMode->GetMasterName() == g_MyInfo.GetPublicID() )
		{			
			SetChildWndStyleAdd( ID_HEAD_QUATER_BTN, IWS_INACTIVE );
		}
	}
}

void TrainingFuncMenuWnd::UpdateHomeButton()
{
	SetChildWndStyleRemove( ID_HOME_BTN, IWS_INACTIVE );
	SetChildWndStyleAdd( ID_HOME_BTN, IWS_EXACTIVE );

	if( m_pCurrMode && m_pCurrMode->GetModeType() == MT_HOUSE )
	{
		ioHouseMode* pMode = ToHouseMode( m_pCurrMode );
		if( pMode && pMode->GetMasterName() == g_MyInfo.GetPublicID() )
		{	
			SetChildWndStyleAdd( ID_HOME_BTN, IWS_INACTIVE );
			SetChildWndStyleRemove( ID_HOME_BTN, IWS_EXACTIVE );
		}
	}
	else
	{
		if( g_MyInfo.IsHouseOpen() )
		{
			SetChildWndStyleRemove( ID_HOME_BTN, IWS_EXACTIVE );
		}
	}
}

void TrainingFuncMenuWnd::UpdateGuildHQButton()
{	
	SetChildWndStyleRemove( ID_GUILD_HQ_BTN, IWS_INACTIVE );
	SetChildWndStyleRemove( ID_GUILD_HQ_BTN, IWS_EXACTIVE );
	if( m_pCurrMode && m_pCurrMode->GetModeType() == MT_TRAINING && m_pCurrMode->GetCreator()->GetPlazaModeType() == PT_GUILD )
	{
		SetChildWndStyleAdd( ID_GUILD_HQ_BTN, IWS_INACTIVE );
	}
	else
	{
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !pGuildData || !pGuildData->GetGuildRoomOpen() )
		{
			SetChildWndStyleAdd( ID_GUILD_HQ_BTN, IWS_EXACTIVE );
		}
	}
}

void TrainingFuncMenuWnd::OnRender()
{
	if( SkipRender() )
		return;
	
	ioMovingWnd::OnRender();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrainingFuncMenuWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /*= false */ )
{	
	float fPosGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	m_iDefXPos = iTempDefXPos + fPosGap;
	m_iDefYPos = iTempDefYPos + fPosGap;

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();

		SetWndPos( m_iDefaultX, m_iDefaultY );
		m_bChangeResolution = true;
	}
}

void TrainingFuncMenuWnd::ResetWindow()
{
	int iXPos = GetXPos();
	int iYPos = GetYPos();

	ioMovingWnd::SetWndPos( iXPos, iYPos );

	DWORD dwWndStyle = GetWndStyle();
	SetWndStyle( dwWndStyle );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrainingFuncMenuWnd::InitShowAniState()
{
	enum { DELAY_DELAY = 165, DELAY_UP = 198, DELAY_DOWN = 66, DELAY_UP_STOP = 66, };

	m_ShowAniState.SetCurState( SAT_NONE );

	FSMState *pState = new FSMState(SAT_NONE);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
		pState->Add( SAT_END,  SAT_END );
	}
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_START_ROUND);
	if( pState )
		pState->Add( SAT_DELAY,  SAT_DELAY);
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_DELAY);
	if( pState )
		pState->Add( SAT_DELAY,  SAT_UP , DELAY_DELAY);
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_UP);
	if( pState )
		pState->Add( SAT_UP,  SAT_DOWN , DELAY_UP);
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_DOWN);
	if( pState )
		pState->Add( SAT_DOWN,  SAT_UP_STOP , DELAY_DOWN );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_UP_STOP);
	if( pState )
		pState->Add( SAT_UP_STOP,  SAT_STOP , DELAY_UP_STOP );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_STOP);
	if( pState )
		pState->Add( SAT_END,  SAT_END );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_END);
	if( pState )
	{
		pState->Add( SAT_NONE,  SAT_NONE );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
	}
	m_ShowAniState.Add( pState );
}

void TrainingFuncMenuWnd::ChangeShowAniState( ShowAniType eShowAniType )
{
	m_ShowAniState.ChangeState( (int) eShowAniType );
}

int TrainingFuncMenuWnd::GetCurState() const
{
	return m_ShowAniState.GetCurState();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrainingFuncMenuWnd::UpdateDefaultPos()
{
	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ))
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			if( COMPARE( GetYPos(), 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = GetYPos();
		}
	}
}

bool TrainingFuncMenuWnd::SkipRender()
{
	bool bSkipRender = false;

	if( g_GUIMgr.IsSkipUIByBlind() )
		bSkipRender = true;

	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND )
		bSkipRender = true;

	if ( bSkipRender && m_bStartAni )
	{
		m_bStartAni = false;
		for( int i=ID_BATTLE_BTN; i<ID_BTN_MAX; ++i )
			HideChildWnd(i);
	}
	else if ( !bSkipRender && !m_bStartAni )
	{
		m_bStartAni = true;
		for( int i=ID_BATTLE_BTN; i<ID_BTN_MAX; ++i )
			ShowChildWnd(i);
	}
	return bSkipRender;
}

void TrainingFuncMenuWnd::ShowTrainingFuncWnd( DWORD dwID )
{
	TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
	if( pTrainingFuncWnd )
	{
		if( dwID == ID_HEAD_QUATER_BTN )
		{
			pTrainingFuncWnd->ShowHeadquartersMsg();
			pTrainingFuncWnd->ResetCurrMenu();
		}
		else if( dwID == ID_HOME_BTN )
		{
			pTrainingFuncWnd->ShowHomeMsg();
			pTrainingFuncWnd->ResetCurrMenu();
		}
		else if( dwID == ID_GUILD_HQ_BTN )
		{
			pTrainingFuncWnd->ShowGuildHQMsg();
			pTrainingFuncWnd->ResetCurrMenu();
		}
		else
		{
			if( pTrainingFuncWnd->GetCurrMenu() == dwID )
			{
				pTrainingFuncWnd->HideWnd();
				pTrainingFuncWnd->ResetCurrMenu();
			}
			else
				pTrainingFuncWnd->ShowMenu( dwID );
		}
	}
}