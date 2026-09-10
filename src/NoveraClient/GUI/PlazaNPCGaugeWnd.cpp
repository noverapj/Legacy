#include "StdAfx.h"
#include "PlazaNPCGaugeWnd.h"


bool PlazaNPCGaugeWnd::m_bPosToDefualt = false;

PlazaNPCGaugeWnd::PlazaNPCGaugeWnd()
{
	m_pIconBack[GAUGE_NORMAL_NPC]		= NULL;
	m_pIconBack[GAUGE_AWAKENING_NPC]    = NULL;
	m_pGauge[GAUGE_NORMAL_NPC]			= NULL;
	m_pGauge[GAUGE_AWAKENING_NPC]       = NULL;

	m_pGaugeBack		= NULL;
	m_pGaugeWhite		= NULL;	
	m_pWhiteLine		= NULL;
	m_pMonsterIcon		= NULL;
	m_pNpcImgName		= NULL;
	m_pAttackEffect1	= NULL;
	m_pAttackEffect2	= NULL;

	m_pRaidGauge		= NULL;
	m_pRaidGaugeBack	= NULL;
	m_pRaidGaugeWhite	= NULL;

	m_iBlindOffSetY   = 0;
	m_iPreBlindOffSetY= 0;
	m_iDefaultX       = 0;
	m_iDefaultY       = 0;
	m_nVibY				= 0;

	m_eGaugeType = GAUGE_NORMAL_NPC;
	m_fMaxHP = m_fCurHP = m_fAniHP = 0.0f;
	m_fCurAniTime = 0.0f;

	m_bScreen = false;
	m_fScreenTime = 0.0f;
	m_fBlinkTime = 0.f;

	m_dwTimer = 0;

	m_pAniState = new ioAnimationState;

	m_bIsRaidMode = false;
	m_nCurRank = 0;
	m_nTotalPlayer = 0;
}

PlazaNPCGaugeWnd::~PlazaNPCGaugeWnd()
{
	SAFEDELETE( m_pIconBack[GAUGE_NORMAL_NPC] );
	SAFEDELETE( m_pIconBack[GAUGE_AWAKENING_NPC] );
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGaugeWhite );
	SAFEDELETE( m_pGauge[GAUGE_NORMAL_NPC] );
	SAFEDELETE( m_pGauge[GAUGE_AWAKENING_NPC] );
	SAFEDELETE( m_pWhiteLine );
	SAFEDELETE( m_pMonsterIcon );
	SAFEDELETE( m_pNpcImgName );
	SAFEDELETE( m_pAttackEffect1 );
	SAFEDELETE( m_pAttackEffect2 );
	SAFEDELETE( m_pAniState );

	SAFEDELETE( m_pRaidGauge );
	SAFEDELETE( m_pRaidGaugeBack );
	SAFEDELETE( m_pRaidGaugeWhite );

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "PlazaNPCGaugeWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "PlazaNPCGaugeWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void PlazaNPCGaugeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{	
	if( szType == "IconBackOrange" )
	{
		SAFEDELETE( m_pIconBack[GAUGE_NORMAL_NPC] );
		m_pIconBack[GAUGE_NORMAL_NPC] = pImage;
	}
	else if( szType == "IconBackRed" )
	{
		SAFEDELETE( m_pIconBack[GAUGE_AWAKENING_NPC] );
		m_pIconBack[GAUGE_AWAKENING_NPC] = pImage;
	}
	else if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGaugeBack );
		m_pGaugeBack = pImage;
	}
	else if( szType == "GaugeOrange" )
	{
		SAFEDELETE( m_pGauge[GAUGE_NORMAL_NPC] );
		m_pGauge[GAUGE_NORMAL_NPC] = pImage;
	}
	else if( szType == "GaugeRed" )
	{
		SAFEDELETE( m_pGauge[GAUGE_AWAKENING_NPC] );
		m_pGauge[GAUGE_AWAKENING_NPC] = pImage;
	}
	else if( szType == "GaugeWhite" )
	{
		SAFEDELETE( m_pGaugeWhite );
		m_pGaugeWhite = pImage;
	}
	else if( szType == "WhiteLine" )
	{
		SAFEDELETE( m_pWhiteLine );
		m_pWhiteLine = pImage;
	}
	else if( szType == "AttackEffect1" )
	{
		SAFEDELETE( m_pAttackEffect1 );
		m_pAttackEffect1 = pImage;
	}
	else if( szType == "AttackEffect2" )
	{
		SAFEDELETE( m_pAttackEffect2 );
		m_pAttackEffect2 = pImage;
	}
	else if( szType == "RaidGauge" )
	{
		SAFEDELETE( m_pRaidGauge );
		m_pRaidGauge = pImage;
	}
	else if( szType == "RaidGaugeWhite" )
	{
		SAFEDELETE( m_pRaidGaugeWhite );
		m_pRaidGaugeWhite = pImage;
	}
	else if( szType == "RaidGaugeBack" )
	{
		SAFEDELETE( m_pRaidGaugeBack );
		m_pRaidGaugeBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PlazaNPCGaugeWnd::UpdateGaugeAni()
{	
	float fTime = (float)FRAMEGETTIME();
	if( fTime - m_fScreenTime > FLOAT100 )
	{
		m_fScreenTime = fTime;
		m_bScreen = !m_bScreen;
	}

	if( (m_fBlinkTime != 0.f) )
	{
		if(fTime - m_fBlinkTime > FLOAT150)
		{
			m_fBlinkTime = 0.f;
		}
		else
		{
			int nPos = (int)(fTime - m_fBlinkTime) / 35; 

			switch( nPos )
			{
			case 0: m_nVibY = - 3; break;
			case 1:	m_nVibY = 3; break;
			case 2: m_nVibY = -2; break;
			case 3: m_nVibY = 2; break;
			default: m_nVibY = 0; break;
			}
		}
	}

	if( m_fAniHP > m_fCurHP )
	{
		float fTick = FLOAT500;
		float fGapTime = fTime - m_fCurAniTime;

		m_fAniHP = max( m_fCurHP, m_fAniHP - ( fGapTime / fTick ) );
	}
}

void PlazaNPCGaugeWnd::RenderBossMonster( int iXPos, int iYPos )
{
	iYPos = iYPos + m_nVibY;

	m_fMaxHP = max( FLOAT1, m_fMaxHP );      //
	if( m_pGaugeBack )
		m_pGaugeBack->Render( iXPos + 55, iYPos + 22 );

	if( m_pWhiteLine )
	{
		m_pWhiteLine->Render( iXPos + 107, iYPos + 57 );

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 155, iYPos + 63, FONT_SIZE_14, "도주까지" );

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( D3DCOLOR_XRGB(255, 60, 0) );

		DWORD dwTimer, dwHour, dwMin, dwSec;
		dwTimer = (m_dwTimer - FRAMEGETTIME()) / 1000;
		dwHour = dwTimer / 3600;
		dwMin = (dwTimer % 3600) / 60;
		dwSec = (dwTimer % 3600) % 60;

		g_FontMgr.PrintText( iXPos + 215, iYPos + 63, FONT_SIZE_14, "%02d:%02d:%02d", (int)dwHour, (int)dwMin, (int)dwSec );
	}

	if( m_pGauge[m_eGaugeType] )
	{
		float fHPRate = m_fCurHP / m_fMaxHP;
		float fWidth = m_pGauge[m_eGaugeType]->GetWidth() * fHPRate;

		m_pGauge[m_eGaugeType]->RenderWidthCut( iXPos + 55, iYPos + 22, 0, fWidth );
		if( fHPRate <= 0.30f && m_bScreen )
			m_pGauge[m_eGaugeType]->RenderWidthCut( iXPos + 55, iYPos + 22, 0, fWidth, UI_RENDER_SCREEN );

		if( m_fAniHP > m_fCurHP )
		{
			float fAniWidth = m_pGauge[m_eGaugeType]->GetWidth() * ( m_fAniHP / m_fMaxHP );
			// Screen x 2
			m_pGauge[m_eGaugeType]->RenderWidthCut( iXPos + 55, iYPos + 22, fWidth, fAniWidth, UI_RENDER_SCREEN );
			m_pGauge[m_eGaugeType]->RenderWidthCut( iXPos + 55, iYPos + 22, fWidth, fAniWidth, UI_RENDER_SCREEN );
		}

		if( m_fBlinkTime != 0.f )
		{
			float fRate = max( 0.f, FRAMEGETTIME() - m_fBlinkTime );
			float fAlpha = FLOAT09 - FLOAT09 * (fRate / FLOAT150);

			if( m_pGaugeWhite )
			{
				m_pGaugeWhite->SetAlpha( 255 * fAlpha );
				m_pGaugeWhite->RenderWidthCut( iXPos + 55, iYPos + 22, 0, fWidth, UI_RENDER_NORMAL );
			}
		}

		if( m_pNpcImgName )
			m_pNpcImgName->Render( iXPos + 210, iYPos + 30, UI_RENDER_ADD );

		if( m_pIconBack[m_eGaugeType] )
			m_pIconBack[m_eGaugeType]->Render( iXPos + 42, iYPos + 37, UI_RENDER_SCREEN );

		if( m_pMonsterIcon )
		{
			m_pMonsterIcon->SetScale( 0.66f, 0.66f );
			m_pMonsterIcon->Render( iXPos + 42, iYPos + 37, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		if( m_fBlinkTime != 0.f )
		{
			float fRate = max( 0.f, FRAMEGETTIME() - m_fBlinkTime );
			float fAlpha = FLOAT09 - FLOAT09 * (fRate / FLOAT150);

			if( m_pAttackEffect1 && m_pAttackEffect2 )
			{
				if( m_stEffectPos.x % 2 == 0)
				{
					m_pAttackEffect1->SetAlpha( 255 * fAlpha );
					m_pAttackEffect1->Render( iXPos + 37 + m_stEffectPos.x, iYPos + 37 + m_stEffectPos.y);
				}
				else
				{
					m_pAttackEffect2->SetAlpha( 255 * fAlpha );
					m_pAttackEffect2->Render( iXPos + 37 + m_stEffectPos.x, iYPos + 37 + m_stEffectPos.y);
				}
			}
		}
	}
}

void PlazaNPCGaugeWnd::RenderRaidBossMonster( int iXPos, int iYPos )
{
	iYPos = iYPos + m_nVibY;

	m_fMaxHP = max( FLOAT1, m_fMaxHP );      //
	if( m_pRaidGaugeBack )
		m_pRaidGaugeBack->Render( iXPos + 15, iYPos + 6 );

	if( m_nCurRank > 0 && m_pWhiteLine )
	{
		m_pWhiteLine->Render( iXPos + 35, iYPos + 41 );

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 141, iYPos + 47, FONT_SIZE_14, "현재순위 : %d위(총%d명)", m_nCurRank, m_nTotalPlayer );
	}

	if( m_pRaidGauge )
	{
		float fHPRate = m_fCurHP / m_fMaxHP;
		float fWidth = m_pRaidGauge->GetWidth() * fHPRate;

		m_pRaidGauge->RenderWidthCut( iXPos + 15, iYPos + 6, 0, fWidth );
		if( fHPRate <= 0.30f && m_bScreen )
			m_pRaidGauge->RenderWidthCut( iXPos + 15, iYPos + 6, 0, fWidth, UI_RENDER_SCREEN );

		if( m_fAniHP > m_fCurHP )
		{
			float fAniWidth = m_pRaidGauge->GetWidth() * ( m_fAniHP / m_fMaxHP );
			// Screen x 2
			m_pRaidGauge->RenderWidthCut( iXPos + 15, iYPos + 6, fWidth, fAniWidth, UI_RENDER_SCREEN );
			m_pRaidGauge->RenderWidthCut( iXPos + 15, iYPos + 6, fWidth, fAniWidth, UI_RENDER_SCREEN );
		}

		if( m_fBlinkTime != 0.f )
		{
			float fRate = max( 0.f, FRAMEGETTIME() - m_fBlinkTime );
			float fAlpha = FLOAT09 - FLOAT09 * (fRate / FLOAT150);

			if( m_pRaidGaugeWhite )
			{
				m_pRaidGaugeWhite->SetAlpha( 255 * fAlpha );
				m_pRaidGaugeWhite->RenderWidthCut( iXPos + 15, iYPos + 6, 0, fWidth, UI_RENDER_NORMAL );
			}
		}

		if( m_pNpcImgName )
			m_pNpcImgName->Render( iXPos + 141, iYPos + 13, UI_RENDER_ADD );

		if( m_pIconBack[m_eGaugeType] )
			m_pIconBack[m_eGaugeType]->Render( iXPos + 1, iYPos + 21, UI_RENDER_SCREEN );

		if( m_pMonsterIcon )
		{
			m_pMonsterIcon->SetScale( 0.66f, 0.66f );
			m_pMonsterIcon->Render( iXPos + 1, iYPos + 21, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		if( m_fBlinkTime != 0.f )
		{
			float fRate = max( 0.f, FRAMEGETTIME() - m_fBlinkTime );
			float fAlpha = FLOAT09 - FLOAT09 * (fRate / FLOAT150);

			if( m_pAttackEffect1 && m_pAttackEffect2 )
			{
				if( m_stEffectPos.x % 2 == 0)
				{
					m_pAttackEffect1->SetAlpha( 255 * fAlpha );
					m_pAttackEffect1->Render( iXPos - 10 + m_stEffectPos.x, iYPos + 27 + m_stEffectPos.y);
				}
				else
				{
					m_pAttackEffect2->SetAlpha( 255 * fAlpha );
					m_pAttackEffect2->Render( iXPos - 10 + m_stEffectPos.x, iYPos + 27 + m_stEffectPos.y);
				}
			}
		}
	}
}

void PlazaNPCGaugeWnd::OnRender()
{
	if( !m_pAniState->IsRender() )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();	

	if ( m_bIsRaidMode )
		RenderRaidBossMonster( Setting::HalfWidth() - 141, Setting::Height() * 0.13f );
	else
		RenderBossMonster( iXPos, iYPos );
}

void PlazaNPCGaugeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	UpdateAniState();
	UpdateGaugeAni();
	ioMovingWnd::OnProcess( fTimePerSec );
}

void PlazaNPCGaugeWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "PlazaNPCGaugeWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "PlazaNPCGaugeWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

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

void PlazaNPCGaugeWnd::SetBossMonster( int iGaugeType, const ioHashString &rkMonsterName, const ioHashString &rkMonsterIcon, const ioHashString &rkMonsterImgName, DWORD dwAliveTime )
{
	m_eGaugeType = (GaugeType)iGaugeType;
	m_szMonsterName = rkMonsterName;

	SAFEDELETE( m_pMonsterIcon );
	m_pMonsterIcon = g_UIImageSetMgr.CreateImageByFullName( rkMonsterIcon );

	SAFEDELETE( m_pNpcImgName );
	m_pNpcImgName = g_UIImageSetMgr.CreateImageByFullName( rkMonsterImgName );

	m_fBlinkTime = 0.f;
	m_fAniHP = 0.0f;
	m_nVibY = 0;

	m_dwTimer = dwAliveTime + FRAMEGETTIME();
	m_bIsRaidMode = false;
}

void PlazaNPCGaugeWnd::UpdateBossMonster( float fCurHP, float fMaxHP )
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
		if( m_fAniHP == fPrevHP )
			m_fCurAniTime = FRAMEGETTIME();

		m_fBlinkTime = FRAMEGETTIME();

		m_IORandom.Randomize();
		int nRandomX = m_IORandom.Random(0, 20);
		int nRandomY = m_IORandom.Random(0, 20);

		m_stEffectPos.x = nRandomX - 10;
		m_stEffectPos.y = nRandomY - 10;
	}
}

void PlazaNPCGaugeWnd::SetRaidBossMonster( int iGaugeType, const ioHashString &rkMonsterName, const ioHashString &rkMonsterIcon, const ioHashString &rkMonsterImgName )
{
	m_eGaugeType = (GaugeType)iGaugeType;
	m_szMonsterName = rkMonsterName;

	SAFEDELETE( m_pMonsterIcon );
	m_pMonsterIcon = g_UIImageSetMgr.CreateImageByFullName( rkMonsterIcon );

	SAFEDELETE( m_pNpcImgName );
	m_pNpcImgName = g_UIImageSetMgr.CreateImageByFullName( rkMonsterImgName );

	m_fBlinkTime = 0.f;
	m_fAniHP = 0.0f;
	m_nVibY = 0;
	m_bIsRaidMode = true;
	m_nCurRank = 0;
	m_nTotalPlayer = 0;
}

void PlazaNPCGaugeWnd::UpdateRaidBossMonster( float fCurHP, float fMaxHP, int nCurRank, int nTotalPlayer )
{
	m_nCurRank = nCurRank;
	m_nTotalPlayer = nTotalPlayer;

	float fPrevHP = m_fCurHP;
	m_fCurHP = fCurHP;
	m_fMaxHP = fMaxHP;

	if( m_fAniHP == 0.0f && m_fCurHP > 0.0f )
	{	
		m_fAniHP = fMaxHP;
	}
	else if( fPrevHP != m_fCurHP )
	{
		if( m_fAniHP == fPrevHP )
			m_fCurAniTime = FRAMEGETTIME();

		m_fBlinkTime = FRAMEGETTIME();

		m_IORandom.Randomize();
		int nRandomX = m_IORandom.Random(0, 20);
		int nRandomY = m_IORandom.Random(0, 20);

		m_stEffectPos.x = nRandomX - 10;
		m_stEffectPos.y = nRandomY - 10;
	}
}

void PlazaNPCGaugeWnd::SetBlindOffSet( int iYOffSet )
{
	m_iBlindOffSetY = iYOffSet;
}

void PlazaNPCGaugeWnd::InitAniState()
{
	m_pAniState->InitAniState();
}

void PlazaNPCGaugeWnd::UpdateAniState()
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

void PlazaNPCGaugeWnd::ChangeShowAniState( ioAnimationState::ShowAniType eShowAniType )
{
	m_pAniState->ChangeShowAniState( eShowAniType );
}

bool PlazaNPCGaugeWnd::UpdateShowAni()
{
	int iCurYpos = m_iDefaultY;

	if(m_pAniState->UpdateShowAni(m_iDefaultX, m_iDefaultY, iCurYpos))
	{
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}

	return false;
}

int PlazaNPCGaugeWnd::GetCurShowAniState()
{
	return m_pAniState->GetCurShowAniState();
}

void PlazaNPCGaugeWnd::UpdateDefaultPos()
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

void PlazaNPCGaugeWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
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