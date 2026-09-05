#include "StdAfx.h"

#include "BossMonsterGaugeWnd.h"

bool BossMonsterGaugeWnd::m_bPosToDefualt = false;

BossMonsterGaugeWnd::BossMonsterGaugeWnd()
{
	m_pIconBackOrange = NULL;
	m_pIconBackRed    = NULL;
	m_pGaugeBack      = NULL;
	m_pGaugeOrange    = NULL;
	m_pGaugeRed       = NULL;
	m_pMonsterIcon    = NULL;

	m_iBlindOffSetY   = 0;
	m_iPreBlindOffSetY= 0;
	m_iDefaultX       = 0;
	m_iDefaultY       = 0;

	m_eGaugeType = GAUGE_HALF_BOSS;
	m_fMaxHP = m_fCurHP = m_fAniHP = 0.0f;
	m_fCurAniTime = 0.0f;

	m_bScreen = false;
	m_fScreenTime = 0.0f;

	InitShowAniState();
}

BossMonsterGaugeWnd::~BossMonsterGaugeWnd()
{
	SAFEDELETE( m_pIconBackOrange );
	SAFEDELETE( m_pIconBackRed );
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGaugeOrange );
	SAFEDELETE( m_pGaugeRed );
	SAFEDELETE( m_pMonsterIcon );

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "BossMonsterGaugeWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "BossMonsterGaugeWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void BossMonsterGaugeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{	
	if( szType == "IconBackOrange" )
	{
		SAFEDELETE( m_pIconBackOrange );
		m_pIconBackOrange = pImage;
	}
	else if( szType == "IconBackRed" )
	{
		SAFEDELETE( m_pIconBackRed );
		m_pIconBackRed = pImage;
	}
	else if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGaugeBack );
		m_pGaugeBack = pImage;
	}
	else if( szType == "GaugeOrange" )
	{
		SAFEDELETE( m_pGaugeOrange );
		m_pGaugeOrange = pImage;
	}
	else if( szType == "GaugeRed" )
	{
		SAFEDELETE( m_pGaugeRed );
		m_pGaugeRed = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void BossMonsterGaugeWnd::UpdateGaugeAni()
{	
	float fTime = (float)FRAMEGETTIME();
	if( fTime - m_fScreenTime > FLOAT100 )
	{
		m_fScreenTime = fTime;
		m_bScreen = !m_bScreen;
	}

	if( m_fAniHP > m_fCurHP )
	{
		float fTick = FLOAT1000;
		float fGapTime = fTime - m_fCurAniTime;
		m_fAniHP = max( m_fCurHP, m_fAniHP - ( fGapTime / fTick ) );
		if( fGapTime >= fTick )
			m_fCurAniTime = fTime;
	}
}

void BossMonsterGaugeWnd::OnRender()
{
	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND ) 
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();	

	m_fMaxHP = max( FLOAT1, m_fMaxHP );      //
	if( m_pGaugeBack )
		m_pGaugeBack->Render( iXPos + 74, iYPos + 19 );

	switch( m_eGaugeType )
	{
	case GAUGE_HALF_BOSS:
		{
			if( m_pGaugeOrange )
			{
				float fHPRate = m_fCurHP / m_fMaxHP;
				float fWidth = m_pGaugeOrange->GetWidth() * fHPRate;
				m_pGaugeOrange->RenderWidthCut( iXPos + 76, iYPos + 21, 0, fWidth );
				if( fHPRate <= 0.30f && m_bScreen )
				{
					// Screen x 1
					m_pGaugeOrange->RenderWidthCut( iXPos + 76, iYPos + 21, 0, fWidth, UI_RENDER_SCREEN );
				}

				if( m_fAniHP > m_fCurHP )
				{
					float fAniWidth = m_pGaugeOrange->GetWidth() * ( m_fAniHP / m_fMaxHP );
					// Screen x 2
					m_pGaugeOrange->RenderWidthCut( iXPos + 76, iYPos + 21, fWidth, fAniWidth, UI_RENDER_SCREEN );
					m_pGaugeOrange->RenderWidthCut( iXPos + 76, iYPos + 21, fWidth, fAniWidth, UI_RENDER_SCREEN );
				}
			}

			if( m_pIconBackOrange )
			{
				m_pIconBackOrange->Render( iXPos + 5, iYPos - 20, UI_RENDER_SCREEN );
			}

			if( !m_szMonsterMent.IsEmpty() )
			{
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
				g_FontMgr.SetAlignType( TAT_LEFT );
				g_FontMgr.SetTextColor( 255, 126, 0 );
				g_FontMgr.PrintText( iXPos + 82, iYPos + 2, FONT_SIZE_13, STR(1), m_szMonsterMent.c_str() );
			}
		}
		break;
	case GAUGE_LAST_BOSS:
		{
			if( m_pGaugeRed )
			{
				float fHPRate = m_fCurHP / m_fMaxHP;
				float fWidth = m_pGaugeRed->GetWidth() * fHPRate;
				m_pGaugeRed->RenderWidthCut( iXPos + 76, iYPos + 21, 0, fWidth );
				if( fHPRate <= 0.30f && m_bScreen )
				{
					// Screen x 1
					m_pGaugeRed->RenderWidthCut( iXPos + 76, iYPos + 21, 0, fWidth, UI_RENDER_SCREEN );
				}

				if( m_fAniHP > m_fCurHP )
				{
					float fAniWidth = m_pGaugeRed->GetWidth() * ( m_fAniHP / m_fMaxHP );
					// Screen x 2
					m_pGaugeRed->RenderWidthCut( iXPos + 76, iYPos + 21, fWidth, fAniWidth, UI_RENDER_SCREEN );
					m_pGaugeRed->RenderWidthCut( iXPos + 76, iYPos + 21, fWidth, fAniWidth, UI_RENDER_SCREEN );
				}
			}

			if( m_pIconBackRed )
			{
				m_pIconBackRed->Render( iXPos + 5, iYPos - 20, UI_RENDER_SCREEN );
			}

			if( !m_szMonsterMent.IsEmpty() )
			{
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
				g_FontMgr.SetAlignType( TAT_LEFT );
				g_FontMgr.SetTextColor( 255, 60, 0 );
				g_FontMgr.PrintText( iXPos + 82, iYPos + 2, FONT_SIZE_13, STR(2), m_szMonsterMent.c_str() );
			}
		}
		break;
	}

	if( m_pMonsterIcon )
	{
		m_pMonsterIcon->SetScale( FLOAT09, FLOAT09 );
		m_pMonsterIcon->Render( iXPos + 51, iYPos + 27, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void BossMonsterGaugeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	UpdateAniState();
	UpdateGaugeAni();
	ioMovingWnd::OnProcess( fTimePerSec );
}

void BossMonsterGaugeWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "BossMonsterGaugeWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "BossMonsterGaugeWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();

	if( m_bPosToDefualt )
	{
		m_bPosToDefualt = false;
		SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}
}

void BossMonsterGaugeWnd::SetBossMonster( int iGaugeType, const ioHashString &rkMonsterName, const ioHashString &rkMonsterIcon, ioHashString &rkMonsterMent )
{
	m_eGaugeType = (GaugeType)iGaugeType;
	m_szMonsterName = rkMonsterName;
	m_szMonsterMent = rkMonsterMent;
	
	SAFEDELETE( m_pMonsterIcon );
	m_pMonsterIcon = g_UIImageSetMgr.CreateImageByFullName( rkMonsterIcon );

	m_fAniHP = 0.0f;
}

void BossMonsterGaugeWnd::UpdateBossMonster( float fCurHP, float fMaxHP )
{
	float fPrevHP = m_fCurHP;
	m_fCurHP = fCurHP;
	m_fMaxHP = fMaxHP;
	
	if( m_fAniHP == 0.0f && m_fCurHP > 0.0f )
	{	
		m_fAniHP = fMaxHP;
	}
	else if( fPrevHP != m_fCurHP )
	{
		m_fCurAniTime = FRAMEGETTIME();
	}
}

void BossMonsterGaugeWnd::SetBlindOffSet( int iYOffSet )
{
	m_iBlindOffSetY = iYOffSet;
}

void BossMonsterGaugeWnd::InitAniState()
{
	m_ShowAniState.SetCurState( SAT_NONE );
}

void BossMonsterGaugeWnd::InitShowAniState()
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
		pState->Add( SAT_NONE,  SAT_NONE );
	}
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

void BossMonsterGaugeWnd::UpdateAniState()
{
	if( UpdateShowAni() )
		return;

	UpdateDefaultPos();

	// blind ani
	if( m_iBlindOffSetY != m_iPreBlindOffSetY )
	{
		int iCurYPos = m_iDefaultY + m_iBlindOffSetY;
		if( !COMPARE( iCurYPos, 0, ( Setting::Height() - GetHeight() ) + 1) )
			return;
		SetWndPos( m_iDefaultX, iCurYPos );
		m_iPreBlindOffSetY = m_iBlindOffSetY;	
	}
}

void BossMonsterGaugeWnd::ChangeShowAniState( ShowAniType eShowAniType )
{
	m_ShowAniState.ChangeState( (int) eShowAniType );
}

bool BossMonsterGaugeWnd::UpdateShowAni()
{
	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_NONE )
		m_ShowAniState.ChangeState( SAT_END );

	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_START_ROUND )
		m_ShowAniState.ChangeState( SAT_DELAY );

	if( g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_END )
		m_ShowAniState.ChangeState( SAT_NONE );

	m_ShowAniState.UpdateTime();

	if( m_ShowAniState.GetCurState() == SAT_UP )
	{
		enum { MINUS_Y_POS = 140 };
		int iStartYPos = MINUS_Y_POS + m_iDefaultY;
		int iCurYpos = m_iDefaultY - ( cos( (D3DX_PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * iStartYPos );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_DOWN )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( sin( (D3DX_PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_UP_STOP )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( cos( (D3DX_PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_STOP )
	{
		SetWndPos( m_iDefaultX, m_iDefaultY );
		m_ShowAniState.ChangeState( SAT_END );
		return true;
	}

	return false;
}

int BossMonsterGaugeWnd::GetCurShowAniState()
{
	return m_ShowAniState.GetCurState();
}

void BossMonsterGaugeWnd::UpdateDefaultPos()
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
			int iYPos = GetYPos() - m_iBlindOffSetY;
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = iYPos;
		}
	}
}

void BossMonsterGaugeWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	float fDefCenterY = (float)( m_iDefYPos + GetHeight() / 2 );
	float fDefCenterYRate = fDefCenterY / 600.0f;

	m_iDefYPos = (int)( Setting::Height() * fDefCenterYRate ) - GetHeight() / 2;

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY + m_iBlindOffSetY );
	}
}