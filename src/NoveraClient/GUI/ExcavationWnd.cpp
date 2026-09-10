#include "StdAfx.h"

#include "ExcavationWnd.h"
#include "../ioExcavationManager.h"

ExcavationWnd::ExcavationWnd()
{
	m_pIcon          = NULL;
	m_pIconGray      = NULL;
	m_pHotKeyPushed  = NULL;
	m_pHotKeyReleased = NULL;
	m_pGauge         = NULL;
	m_pShovelEnable  = NULL;
	m_pShovelDisable = NULL;
	m_IconBack		 = NULL;

	m_iCurShovel = 0;
	m_fGaugeRate = 0.0f;
	m_dwRemainTime = 0;
	m_bProcessDelay = false;
	m_bLevelUpDelay = false;
}

ExcavationWnd::~ExcavationWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconGray );
	SAFEDELETE( m_pHotKeyPushed );
	SAFEDELETE( m_pHotKeyReleased );
	SAFEDELETE( m_pGauge );
	SAFEDELETE( m_pShovelEnable );
	SAFEDELETE( m_pShovelDisable );
	SAFEDELETE( m_IconBack );
}

void ExcavationWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "HotKey1" )
	{
		SAFEDELETE( m_pHotKeyPushed );
		m_pHotKeyPushed = pImage;
	}
	else if( szType == "HotKey2" )
	{
		SAFEDELETE( m_pHotKeyReleased );
		m_pHotKeyReleased = pImage;
	}
	else if( szType == "Gauge" )
	{
		SAFEDELETE( m_pGauge );
		m_pGauge = pImage;
	}
	else if( szType == "ShovelEnable" )
	{
		SAFEDELETE( m_pShovelEnable );
		m_pShovelEnable = pImage;
	}
	else if( szType == "ShovelDisable" )
	{
		SAFEDELETE( m_pShovelDisable );
		m_pShovelDisable = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_IconBack );
		m_IconBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ExcavationWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szText[TEXT_LEVEL] = xElement.GetStringAttribute_e( "TextExp" );
	m_szText[TEXT_EXCAVATION_RECHARGE] = xElement.GetStringAttribute_e( "TextExcavationRecharge" );
	m_szText[TEXT_EXCAVATION_READY] = xElement.GetStringAttribute_e( "TextExcavationReady" );
	m_szText[TEXT_EXCAVATION_COUNT] = xElement.GetStringAttribute_e( "TextExcavationCount" );

	m_szRechargeSound = xElement.GetStringAttribute_e( "RechargeSound" );
}

void ExcavationWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	int iGap = ( g_App.GetCurHeight() - UI_BASE_HEIGHT ) * 0.1f;
	iGap = max( 0, iGap );

	POINT ptClientSize;
	g_App.GetClientSize( ptClientSize );
	m_iDefaultX = ( ptClientSize.x / 2 ) - GetWidth() / 2;
	m_iDefaultY = ptClientSize.y - Y_OFFSET - iGap;

	SetWndPos( m_iDefaultX , m_iDefaultY );
}

void ExcavationWnd::iwm_create()
{
	InitShowAniState();

	POINT ptClientSize;
	g_App.GetClientSize( ptClientSize );
	m_iDefaultX = ( ptClientSize.x / 2 ) - GetWidth() / 2;
	m_iDefaultY = ptClientSize.y - Y_OFFSET;
}

void ExcavationWnd::iwm_show()
{
	g_GUIMgr.HideWnd( SKILL_LIST_WND );
	g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );
	g_GUIMgr.HideWnd( FISHING_INVENTORY_WND );

	m_ShowAniState.ChangeState( SAT_START_ROUND );
	SetWndPos( m_iDefaultX , m_iDefaultY + Y_OFFSET );

	m_iCurShovel = g_ExcavationMgr.GetShovelCount();
	m_dwRemainTime = g_ExcavationMgr.GetRemainChargeTime( false );
	ChangeUseIcon();
}

void ExcavationWnd::iwm_hide()
{
	g_GUIMgr.ShowWnd( SKILL_LIST_WND );
	g_GUIMgr.ShowWnd( MINI_SOLDIER_SELECT_WND );	

	g_ExcavationMgr.NoticeLevelAndExp();
}

void ExcavationWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if(!pWnd) return;

	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_STOP:
		if( cmd == IOBN_BTNUP )
		{
			g_ExcavationMgr.SendReleaseKit();
		}
		break;
	case ID_RECHARGE:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.ShowWnd( EXCAVATION_SHOVEL_WND );
		}
		break;
	}
}

void ExcavationWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	UpdateShowAni();

	IoUIText *pText = NULL;
	DWORD dwRemainTime = 0;

	if( !m_bProcessDelay )
	{
		pText = GetText( "exc_lv" );
		if( pText )
		{
			char szBuf[MAX_PATH] = "";
			wsprintf( szBuf, "%s %d", m_szText[TEXT_LEVEL].c_str(), g_MyInfo.GetExcavationLevel() );
			pText->SetText( szBuf );
		}

		pText = GetText( "exc_exp" );
		if( pText )
		{
			int iNextExp = max( 1, g_LevelMgr.GetNextExcavationLevelUpExp( g_MyInfo.GetExcavationLevel() ) );
			float fRate = (float)g_MyInfo.GetExcavationExpert() / (float)iNextExp;
			float fPecent = fRate * 100.0f;
			char szBuf[MAX_PATH] = "";
			wsprintf( szBuf, "%d%%%%", (int)fPecent );
			pText->SetText( szBuf );
		}

		int iNextExp = max( 1, g_LevelMgr.GetNextExcavationLevelUpExp( g_MyInfo.GetExcavationLevel() ) );
		m_fGaugeRate = (float)g_MyInfo.GetExcavationExpert() / (float)iNextExp;
	}

	if( !m_bLevelUpDelay )
	{
		m_iCurShovel = g_ExcavationMgr.GetShovelCount();

		m_dwRemainTime = g_ExcavationMgr.GetRemainChargeTime( false );

		if( REALGETTIME() >= m_dwRemainTime )
			dwRemainTime = 0;
		else
			dwRemainTime = m_dwRemainTime - REALGETTIME();
	}
	else
	{
		if( REALGETTIME() >= m_dwRemainTime )
			dwRemainTime = 0;
		else
			dwRemainTime = m_dwRemainTime - REALGETTIME();
	}

	pText = GetText( "recharge_time" );
	if( pText )
	{
		DWORD dwMin = ( dwRemainTime / 1000 ) / 60;
		DWORD dwSec = ( dwRemainTime / 1000 ) % 60;
		char szBuf[MAX_PATH] = "";
		wsprintf( szBuf, "%.2d:%.2d", dwMin, dwSec );
		pText->SetText( szBuf );
	}

	pText = GetText( "exc_count" );
	if( pText )
	{
		char szBuf[MAX_PATH] = "";
		wsprintf( szBuf, "%s %d/%d", m_szText[TEXT_EXCAVATION_COUNT].c_str(), g_ExcavationMgr.GetShovelCount(), g_ExcavationMgr.GetMaxShovelCount() );
		pText->SetText( szBuf );
	}
	
	switch( g_ExcavationMgr.GetExcavationState() )
	{
	case ioExcavationManager::ES_CHARGING:
		{
		}
		break;
	case ioExcavationManager::ES_STANDBY:
		{
			if( m_dwScaleStartTime == 0 )
				m_dwScaleStartTime = FRAMEGETTIME();

			DWORD dwGapTime = FRAMEGETTIME() - m_dwScaleStartTime;
			if( dwGapTime > 300 )
			{
				dwGapTime -= 300;
				m_dwScaleStartTime = FRAMEGETTIME() - ( dwGapTime % 300 );
			}

			float fTimeRate = (float)dwGapTime / (float)300;
			m_fCurScaleRate = 1.0f + 0.2f * sinf( D3DX_PI * fTimeRate );
		}
		break;
	case ioExcavationManager::ES_DIGGING:
		{

		}
		break;
	}
}

void ExcavationWnd::OnRender()
{
	if( m_ShowAniState.GetCurState() == SAT_DELAY || m_ShowAniState.GetCurState() == SAT_START_ROUND ) 
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pGauge )
	{
		float fWidth = max( 1.0f, m_pGauge->GetWidth() * m_fGaugeRate );
		m_pGauge->RenderWidthCut( iXPos + 92, iYPos + 110, 0, fWidth );
	}

	if( m_pShovelEnable && m_pShovelDisable )
	for( int i=0; i<g_ExcavationMgr.GetMaxShovelCount(); ++i )
	{
		if( i < m_iCurShovel )
		{
			m_pShovelEnable->Render( iXPos + 103 + i * 43, iYPos + 48 );
		}
		else
		{
			m_pShovelDisable->Render( iXPos + 103 + i * 43, iYPos + 48 );
		}
	}

	if( m_IconBack )
	{
		m_IconBack->Render( iXPos + 48, iYPos + 53, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	switch( g_ExcavationMgr.GetExcavationState() )
	{
	case ioExcavationManager::ES_DISABLE:
	case ioExcavationManager::ES_CHARGING:
	case ioExcavationManager::ES_DIGGING:
		{
			if( m_pIconGray )
				m_pIconGray->Render( iXPos + 48, iYPos + 53, UI_RENDER_GRAY );
		}
		break;
	}

	if( m_pIcon )
	{
		if( g_ExcavationMgr.GetExcavationState() == ioExcavationManager::ES_CHARGING )
		{
			m_pIcon->SetScale( 1.0f );

			float iHeight = m_pIcon->GetHeight() * ( 1.0f - g_ExcavationMgr.GetChargingRate() );
			
			m_pIcon->SetColor( TCT_DEFAULT_BLACK );
			m_pIcon->RenderHeightCut( iXPos + 48, iYPos + 53, iHeight-2, m_pIcon->GetHeight() );
			
			m_pIcon->SetColor( TCT_DEFAULT_WHITE );
			m_pIcon->RenderHeightCut( iXPos + 48, iYPos + 53, iHeight, m_pIcon->GetHeight() );

			if( m_pHotKeyPushed )
			{
				m_pHotKeyPushed->Render( iXPos + 49, iYPos + 86 );
			}
		}
		else if( g_ExcavationMgr.GetExcavationState() == ioExcavationManager::ES_STANDBY )
		{
			m_pIcon->SetScale( m_fCurScaleRate );
			m_pIcon->Render( iXPos + 48, iYPos + 53, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( m_pHotKeyPushed && m_pHotKeyReleased )
			{
				if( ( REALGETTIME() / 1000 ) % 2 == 1 )
					m_pHotKeyPushed->Render( iXPos + 49, iYPos + 86 );
				else
					m_pHotKeyReleased->Render( iXPos + 49, iYPos + 86 );
			}
		}
	}
}

void ExcavationWnd::OnRenderAfterChild()
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 160, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetVertAlign( TVA_TOP );

	if( g_ExcavationMgr.GetExcavationState() == ioExcavationManager::ES_CHARGING )
	{
		g_FontMgr.PrintText( iXPos + 48, iYPos + 43, FONT_SIZE_11, m_szText[TEXT_EXCAVATION_RECHARGE].c_str() );
	}
	else if( g_ExcavationMgr.GetExcavationState() == ioExcavationManager::ES_STANDBY )
	{
		g_FontMgr.PrintText( iXPos + 48, iYPos + 43, FONT_SIZE_11, m_szText[TEXT_EXCAVATION_READY].c_str() );
	}
}

void ExcavationWnd::InitShowAniState()
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
	{
		pState->Add( SAT_DELAY,  SAT_DELAY);
		m_ShowAniState.Add( pState );
	}

	pState = new FSMState(SAT_DELAY);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_UP , DELAY_DELAY);
		m_ShowAniState.Add( pState );
	}

	pState = new FSMState(SAT_UP);
	if( pState )
	{
		pState->Add( SAT_UP,  SAT_DOWN , DELAY_UP);
		m_ShowAniState.Add( pState );
	}

	pState = new FSMState(SAT_DOWN);
	if( pState )
	{
		pState->Add( SAT_DOWN,  SAT_UP_STOP , DELAY_DOWN );
		m_ShowAniState.Add( pState );
	}

	pState = new FSMState(SAT_UP_STOP);
	if( pState )
	{
		pState->Add( SAT_UP_STOP,  SAT_STOP , DELAY_UP_STOP );
		m_ShowAniState.Add( pState );
	}

	pState = new FSMState(SAT_STOP);
	if( pState )
	{
		pState->Add( SAT_END,  SAT_END );
		m_ShowAniState.Add( pState );
	}

	pState = new FSMState(SAT_END);
	if( pState )
	{
		pState->Add( SAT_NONE,  SAT_NONE );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
		m_ShowAniState.Add( pState );
	}
}

bool ExcavationWnd::UpdateShowAni()
{
	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_NONE )
		m_ShowAniState.ChangeState( SAT_END );

	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_START_ROUND )
		m_ShowAniState.ChangeState( SAT_DELAY );

	if( g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_END )
		m_ShowAniState.ChangeState( SAT_NONE );

	m_ShowAniState.UpdateTime();

	switch( m_ShowAniState.GetCurState() )
	{
	case SAT_UP:
		{
			POINT clientSize;
			g_App.GetClientSize(clientSize);
			int iStartYPos = Y_OFFSET + ( clientSize.y - ( m_iDefaultY + GetHeight() ) );
			int iCurYpos = m_iDefaultY + ( cos( (D3DX_PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * iStartYPos );	
			SetWndPos( m_iDefaultX, iCurYpos );
		}
		break;
	case SAT_DOWN:
		{
			enum { MAX_OFFSET_YPOS = 11 };
			int iCurYpos = m_iDefaultY + ( sin( (D3DX_PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
			SetWndPos( m_iDefaultX, iCurYpos );
		}
		break;
	case SAT_UP_STOP:
		{
			enum { MAX_OFFSET_YPOS = 11 };
			int iCurYpos = m_iDefaultY + ( cos( (D3DX_PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
			SetWndPos( m_iDefaultX, iCurYpos );
		}
		break;
	case SAT_STOP:
		{
			SetWndPos( m_iDefaultX, m_iDefaultY );
			m_ShowAniState.ChangeState( SAT_END );
		}
		break;
	}

	return false;
}


void ExcavationWnd::ChangeUseIcon()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconGray );

	ioHashString szIconName;
	if( g_ExcavationMgr.GetUseIconName( szIconName ) )
	{
		m_pIcon     = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		m_pIconGray = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	}
}

void ExcavationWnd::SetProcessDelay( bool bDelay )
{
	m_bProcessDelay = bDelay;
}

void ExcavationWnd::SetLevelUpDelay( bool bDelay )
{
	m_bLevelUpDelay = bDelay;
}