#include "StdAfx.h"

#include "../ioVoiceChat/Fourier.h"
#include "SoldierSelectWnd.h"
#include "SoldierExerciseWnd.h"

SoldierExerciseBtn::SoldierExerciseBtn()
{
	m_bForceOver   = false;
	m_eBoughtType  = SBT_NONE;
	m_iClassType   = -1;
	m_iNeedLevel = -1;
	m_pLock		   = NULL;

	m_pArrow	  = NULL;
	m_bArrow      = false;
	m_iDownHeight = 0;

	m_bScreenAction = false;
	m_bScreenSoldier= false;
	m_dwScreenTime  = 500;
	m_dwScreenStartTime = 0;

	m_eNeedLevelType    = ioSetItemInfo::NLT_NONE;
	m_pSoldierIcon      = NULL;
	m_pSoldierSubIcon = NULL;
	m_pLockNum          = NULL;
	m_pLockBattleMark   = NULL;
	m_pLockMedalMark	= NULL;
	m_pLockAwardMark    = NULL;
	m_pBoughtType		= NULL;
	m_pExerciseGeneral  = NULL;
	m_pExercisePcroom   = NULL;
	m_pExerciseEvent    = NULL;
	m_pNewMark          = NULL;
	m_pNewEffect        = NULL;
	m_pHitMark          = NULL;
	m_pHitEffect        = NULL;
	m_pSaleMark          = NULL;
	m_pSaleEffect        = NULL;
	m_pEventMark         = NULL;
	m_pEventEffect       = NULL;
	m_pIconBack          = NULL;

	m_dwNewEffectWaitTime = 0;
	m_dwNewEffectCurTime  = 0;
	m_iMarkType           = 0; // ORDER_DEFAULT
	m_bMarkEffect         = false;

	m_fIconScale = 1.f;
}

SoldierExerciseBtn::~SoldierExerciseBtn()
{
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
	SAFEDELETE( m_pLock );
	SAFEDELETE( m_pArrow );

	SAFEDELETE( m_pLockNum );
	SAFEDELETE( m_pLockBattleMark );
	SAFEDELETE( m_pLockMedalMark );
	SAFEDELETE( m_pLockAwardMark );
	SAFEDELETE( m_pBoughtType );
	SAFEDELETE( m_pExerciseGeneral );
	SAFEDELETE( m_pExercisePcroom );
	SAFEDELETE( m_pExerciseEvent );
	SAFEDELETE( m_pNewMark );
	SAFEDELETE( m_pNewEffect );
	SAFEDELETE( m_pHitMark );
	SAFEDELETE( m_pHitEffect );
	SAFEDELETE( m_pSaleMark );
	SAFEDELETE( m_pSaleEffect );
	SAFEDELETE( m_pEventMark );
	SAFEDELETE( m_pEventEffect );
	SAFEDELETE( m_pIconBack );
}

void SoldierExerciseBtn::SetSoldierIcon( ioUIRenderImage *pSoldierIcon, float fIconScale, bool bScreenAction )
{
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
	m_pSoldierIcon = pSoldierIcon;

	if( m_pSoldierIcon )
		m_pSoldierIcon->SetScale( fIconScale );

	m_bScreenAction = bScreenAction;
	m_bScreenSoldier= false;
	m_dwScreenTime  = 500;
	m_dwScreenStartTime = 0;
	m_fIconScale = fIconScale;
}

void SoldierExerciseBtn::SetSoldierSubIcon( ioUIRenderImage *pSubIcon )
{
	SAFEDELETE( m_pSoldierSubIcon );
	m_pSoldierSubIcon = pSubIcon;

	if( m_pSoldierSubIcon )
		m_pSoldierSubIcon->SetScale( m_fIconScale );
}

void SoldierExerciseBtn::SetBoughtType( SoldierBoughtType eBoughtType )
{
	m_eBoughtType = eBoughtType;
}

void SoldierExerciseBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_bForceOver )
		ioButton::OnDrawOvered( iXPos, iYPos );
	else
		ioButton::OnDrawNormal( iXPos, iYPos );
}

void SoldierExerciseBtn::RenderImageList( int iXPos, int iYPos, UIRenderType eRenderType )
{
	UIElementList::iterator iter;
	for( iter=m_ElementList.begin() ; iter!=m_ElementList.end() ; ++iter )
	{
		(*iter)->Render( iXPos, iYPos, eRenderType , TFO_BILINEAR );
	}
}

void SoldierExerciseBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	RenderSoldierIcon( iXPos, iYPos );
	RenderNewMark( iXPos, iYPos );
	RenderType( iXPos, iYPos );
	RenderArrow( iXPos, iYPos );
	RenderNeedLv( iXPos, iYPos );
}

void SoldierExerciseBtn::RenderNewMark( int iXPos, int iYPos )
{
	enum 
	{ 
		X_NEW_OFFSET   = 23,
		Y_NEW_OFFSET   = 2,
	};

	// new mark
	if( m_iMarkType == SHOP_MARK_TYPE_NEW )
	{
		if( m_pNewMark )
			m_pNewMark->Render( iXPos + X_NEW_OFFSET, iYPos + Y_NEW_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_bMarkEffect )
		{
			if( m_pNewEffect )
				m_pNewEffect->Render( iXPos + X_NEW_OFFSET, iYPos + Y_NEW_OFFSET, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_HIT )
	{
		if( m_pHitMark )
			m_pHitMark->Render( iXPos + X_NEW_OFFSET, iYPos + Y_NEW_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_bMarkEffect )
		{
			if( m_pHitEffect )
				m_pHitEffect->Render( iXPos + X_NEW_OFFSET, iYPos + Y_NEW_OFFSET, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_SALE )
	{
		if( m_pSaleMark )
			m_pSaleMark->Render( iXPos + X_NEW_OFFSET, iYPos + Y_NEW_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_bMarkEffect )
		{
			if( m_pSaleEffect )
				m_pSaleEffect->Render( iXPos + X_NEW_OFFSET, iYPos + Y_NEW_OFFSET, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_EVENT )
	{
		if( m_pEventMark )
			m_pEventMark->Render( iXPos + X_NEW_OFFSET, iYPos + Y_NEW_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_bMarkEffect )
		{
			if( m_pEventEffect )
				m_pEventEffect->Render( iXPos + X_NEW_OFFSET, iYPos + Y_NEW_OFFSET, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
}

void SoldierExerciseBtn::RenderNeedLv( int iXPos, int iYPos )
{
	if( m_eBoughtType != SBT_NONE ) return;
	if( m_eNeedLevelType == ioSetItemInfo::NLT_NONE ) return;

	enum { LOCK_OFFSET_X = 18, 
		LOCK_OFFSET_Y = 12, 

		GRADE_OFFSET_X = 21, 
		GRADE_OFFSET_Y = 22, 

		X_MARK_OFFSET  = 24,
		Y_MARK_OFFSET  = 20,

		X_LOCK_NUM_OFFSET = 31,
		Y_LOCK_NUM_OFFSET = 34,

		X_LOCK_ONE_NUM_OFFSET = 32,
	};

	if( m_pLock )
		m_pLock->Render( iXPos + LOCK_OFFSET_X, iYPos + LOCK_OFFSET_Y, UI_RENDER_NORMAL );

	if( m_eNeedLevelType != ioSetItemInfo::NLT_GRADE )
	{
		if( m_pLockNum )
		{
			m_pLockNum->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
			int iXOffset = X_LOCK_NUM_OFFSET;
			if( ( m_iNeedLevel / 10 ) == 0 ) // 1자리
				iXOffset = X_LOCK_ONE_NUM_OFFSET;
			m_pLockNum->RenderNum( iXPos + iXOffset, iYPos + Y_LOCK_NUM_OFFSET , m_iNeedLevel, -FLOAT1, FLOAT1, UI_RENDER_NORMAL );
		}
	}

	if( m_eNeedLevelType == ioSetItemInfo::NLT_GRADE )
	{
		g_LevelMgr.RenderGrade( iXPos + GRADE_OFFSET_X, iYPos + GRADE_OFFSET_Y, m_iNeedLevel, TEAM_PRIVATE_1 , UI_RENDER_NORMAL);
	}
	else if( m_eNeedLevelType == ioSetItemInfo::NLT_BATTLE )
	{
		if( m_pLockBattleMark )
			m_pLockBattleMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
	}
	else if( m_eNeedLevelType == ioSetItemInfo::NLT_AWARD )
	{
		if( m_pLockAwardMark )
			m_pLockAwardMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
	}
}

void SoldierExerciseBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Lock" )
	{
		SAFEDELETE( m_pLock );
		m_pLock = pImage;
	}
	else if( szType == "Arrow" )
	{
		SAFEDELETE( m_pArrow );
		m_pArrow = pImage;
		if( m_pArrow )
			m_pArrow->SetScale( ARROW_SCALE );
	}
	else if( szType == "LockNum" )
	{
		SAFEDELETE( m_pLockNum );
		m_pLockNum = pImage;
	}
	else if( szType == "LockBattleMark" )
	{
		SAFEDELETE( m_pLockBattleMark );
		m_pLockBattleMark = pImage;
	}
	else if( szType == "MedalMark" )
	{
		SAFEDELETE( m_pLockMedalMark );
		m_pLockMedalMark = pImage;
	}
	else if( szType == "LockAwardMark" )
	{
		SAFEDELETE( m_pLockAwardMark );
		m_pLockAwardMark = pImage;
	}
	else if( szType == "BoughtType" )
	{
		SAFEDELETE( m_pBoughtType );
		m_pBoughtType = pImage;
	}
	else if( szType == "ExerciseGeneral" )
	{
		SAFEDELETE( m_pExerciseGeneral );
		m_pExerciseGeneral = pImage;
	}
	else if( szType == "ExercisePcroom" )
	{
		SAFEDELETE( m_pExercisePcroom );
		m_pExercisePcroom = pImage;
	}
	else if( szType == "NewMark" )
	{
		SAFEDELETE( m_pNewMark );
		m_pNewMark = pImage;
		if( m_pNewMark )
			m_pNewMark->SetScale( NEW_SCALE );
	}
	else if( szType == "NewEffect" )
	{
		SAFEDELETE( m_pNewEffect );
		m_pNewEffect = pImage;
		if( m_pNewEffect )
			m_pNewEffect->SetScale( NEW_SCALE );
	}
	else if( szType == "HitMark" )
	{
		SAFEDELETE( m_pHitMark );
		m_pHitMark = pImage;
		if( m_pHitMark )
			m_pHitMark->SetScale( NEW_SCALE );
	}
	else if( szType == "HitEffect" )
	{
		SAFEDELETE( m_pHitEffect );
		m_pHitEffect = pImage;
		if( m_pHitEffect )
			m_pHitEffect->SetScale( NEW_SCALE );
	}
	else if( szType == "SaleMark" )
	{
		SAFEDELETE( m_pSaleMark );
		m_pSaleMark = pImage;
		if( m_pSaleMark )
			m_pSaleMark->SetScale( NEW_SCALE );
	}
	else if( szType == "SaleEffect" )
	{
		SAFEDELETE( m_pSaleEffect );
		m_pSaleEffect = pImage;
		if( m_pSaleEffect )
			m_pSaleEffect->SetScale( NEW_SCALE );
	}
	else if( szType == "EventMark" )
	{
		SAFEDELETE( m_pEventMark );
		m_pEventMark = pImage;
		if( m_pEventMark )
			m_pEventMark->SetScale( NEW_SCALE );
	}
	else if( szType == "EventEffect" )
	{
		SAFEDELETE( m_pEventEffect );
		m_pEventEffect = pImage;
		if( m_pEventEffect )
			m_pEventEffect->SetScale( NEW_SCALE );
	}
	else if( szType == "ExerciseEvent" )
	{
		SAFEDELETE( m_pExerciseEvent );
		m_pExerciseEvent = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
		if( m_pIconBack )
			m_pIconBack->SetScale( NEW_SCALE );
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void SoldierExerciseBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
	m_bArrow                = xElement.GetBoolAttribute_e( "UseArrow" );
	m_iDownHeight           = xElement.GetIntAttribute_e( "DownHeight" );
	m_dwNewEffectWaitTime   = xElement.GetIntAttribute_e( "NewEffectWaitTime" );
}

void SoldierExerciseBtn::RenderSoldierIcon( int iXPos, int iYPos )
{
	if( !m_pSoldierIcon ) return;

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		m_pSoldierIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_GRAY, TFO_BILINEAR );
		if( m_pSoldierSubIcon )
			m_pSoldierSubIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_GRAY, TFO_BILINEAR );
	}
	else
	{
		if( m_pIconBack )
			m_pIconBack->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		m_pSoldierIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_bClicked || m_bOver || m_bScreenSoldier )
			m_pSoldierIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_SCREEN, TFO_BILINEAR );
		if( m_pSoldierSubIcon )
		{
			m_pSoldierSubIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_NORMAL, TFO_BILINEAR );
			if( m_bClicked || m_bOver || m_bScreenSoldier )
				m_pSoldierSubIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
}

void SoldierExerciseBtn::RenderType( int iXPos, int iYPos )
{
	if( m_eBoughtType == SBT_NONE ) return;

	iXPos += GetWidth() / 2;
	iYPos += GetHeight() / 2;
	if( m_eBoughtType == SBT_BOUGHT )
	{
		if( m_pBoughtType )
			m_pBoughtType->Render( iXPos, iYPos );
	}
	else if( m_eBoughtType == SBT_EXERCISE_GENERAL )
	{
		if( m_pExerciseGeneral )
			m_pExerciseGeneral->Render( iXPos, iYPos );
	}
	else if( m_eBoughtType == SBT_EXERCISE_PCROOM )
	{
		if( m_pExercisePcroom )
			m_pExercisePcroom->Render( iXPos, iYPos );
	}
	else if( m_eBoughtType == SBT_EXERCISE_EVENT )
	{
		if( m_pExerciseEvent )
			m_pExerciseEvent->Render( iXPos, iYPos );
	}
}

void SoldierExerciseBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	UpdateAniState();

	if( m_pSoldierIcon )
	{
		if( m_bScreenAction && 
			m_eBoughtType != SBT_BOUGHT && 
			m_eBoughtType != SBT_EXERCISE_PCROOM && 
			m_eBoughtType != SBT_EXERCISE_EVENT  && 
			!HasWndStyle( IWS_INACTIVE ) && 
			m_eNeedLevelType == ioSetItemInfo::NLT_NONE )
		{
			if( m_dwScreenStartTime == 0 )
				m_dwScreenStartTime = FRAMEGETTIME();

			DWORD dwGapTime = FRAMEGETTIME() - m_dwScreenStartTime;
			if( dwGapTime > m_dwScreenTime )
			{
				m_bScreenSoldier = !m_bScreenSoldier;
				m_dwScreenStartTime = FRAMEGETTIME();
			}
		}
	}

	ProcessNewEffect();
}

void SoldierExerciseBtn::InitAniState()
{
	enum { DELAY_START_DOWN = 100, DELAY_START_UP_STOP = 100, DELAY_DOWN_UP = 150, DELAY_OUT = 100,};

	FSMState *pState = new FSMState(AT_NONE);
	if( pState )
		pState->Add( AT_START_DOWN,  AT_START_DOWN );
	m_AniState.Add( pState );

	pState = new FSMState(AT_START_DOWN);
	if( pState )
	{
		pState->Add( AT_START_DOWN, AT_START_UP, DELAY_START_DOWN );
		pState->Add( AT_WHITE_OUT,  AT_WHITE_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_START_UP);
	if( pState )
	{
		pState->Add( AT_START_UP, AT_START_STOP, DELAY_START_UP_STOP );
		pState->Add( AT_WHITE_OUT,  AT_WHITE_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_START_STOP);
	if( pState )
	{
		pState->Add( AT_START_STOP, AT_DOWN, DELAY_START_UP_STOP );
		pState->Add( AT_WHITE_OUT,  AT_WHITE_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_DOWN);
	if( pState )
	{
		pState->Add( AT_DOWN, AT_UP, DELAY_DOWN_UP );
		pState->Add( AT_WHITE_OUT,  AT_WHITE_OUT );
		pState->Add( AT_NONE,  AT_NONE );

	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_UP);
	if( pState )
	{
		pState->Add( AT_UP, AT_DOWN, DELAY_DOWN_UP);
		pState->Add( AT_WHITE_OUT,  AT_WHITE_OUT );
		pState->Add( AT_NONE,  AT_NONE );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_WHITE_OUT);
	if( pState )
	{
		pState->Add( AT_WHITE_OUT, AT_MOUSE_OVER, DELAY_OUT);
		pState->Add( AT_MOUSE_LEAVE, AT_START_DOWN );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_MOUSE_OVER);
	if( pState )
		pState->Add( AT_MOUSE_LEAVE,  AT_START_DOWN );
	m_AniState.Add( pState );	

	m_AniState.SetCurState( AT_NONE );
}

void SoldierExerciseBtn::UpdateAniState()
{
	if( !m_bArrow ) return;
	if( m_eBoughtType != SBT_NONE ) return;
	if( !g_TutorialMgr.IsTutorial() ) 
	{
		if( m_AniState.GetCurState() != AT_NONE )
			m_AniState.SetCurState( AT_NONE );
		return;
	}

	m_AniState.UpdateTime();

	if( g_TutorialMgr.IsShowExerciseStep() && !HasWndStyle( IWS_INACTIVE ) )
		m_AniState.ChangeState( AT_START_DOWN );
	else
		m_AniState.SetCurState( AT_NONE );
}

void SoldierExerciseBtn::RenderArrow( int iXPos, int iYPos )
{
	if( !m_pArrow ) return;
	if( m_eBoughtType != SBT_NONE ) return;
	if( m_AniState.GetCurState() == AT_NONE || m_AniState.GetCurState() == AT_MOUSE_OVER ) return;

	enum { X_OFFSET = -5, Y_OFFSET_DOWN = -48, Y_OFFSET_UP = -54,};

	if( m_AniState.GetCurState() == AT_START_DOWN )
	{
		enum { HEIGHT_DOWN = 114, START_Y_OFFSET = -176, };

		float fRate = ( (float) m_AniState.GetCurTime() / (float) m_AniState.GetDelay() );
		int iYOffSet = START_Y_OFFSET + ( fRate * HEIGHT_DOWN );

		m_pArrow->SetAlpha((BYTE) 255.0f * fRate );
		m_pArrow->SetColor( 255, 255, 255 );
		m_pArrow->Render( iXPos + X_OFFSET, iYPos + iYOffSet, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( m_AniState.GetCurState() == AT_START_UP )
	{
		m_pArrow->SetAlpha( 255 );
		m_pArrow->SetColor( 255, 255, 255 );
		m_pArrow->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET_UP, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( m_AniState.GetCurState() == AT_START_STOP )
	{
		m_pArrow->SetAlpha( 255 );
		m_pArrow->SetColor( 255, 255, 255 );
		m_pArrow->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET_DOWN, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( m_AniState.GetCurState() == AT_WHITE_OUT )
	{
		float fRate = ( (float)m_AniState.GetCurTime() / (float)m_AniState.GetDelay() );
		int iColor = 255.0f * fRate;
		m_pArrow->SetColor(iColor, iColor, iColor );
		int iAlpha = 255.0f * ( FLOAT1 - fRate );
		m_pArrow->SetAlpha( iAlpha );
		m_pArrow->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET_DOWN, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );

	}
	else if( m_AniState.GetCurState() == AT_DOWN )
	{
		// 가속 2->1->0
		int iYOffSet = cos( (PI/2 * m_AniState.GetCurTime()  ) / m_AniState.GetDelay() ) * m_iDownHeight;	
		m_pArrow->SetAlpha( 255 );
		m_pArrow->SetColor( 255, 255, 255 );
		m_pArrow->Render( iXPos + X_OFFSET, iYPos + ( Y_OFFSET_DOWN - iYOffSet ), UI_RENDER_NORMAL, TFO_BILINEAR);
	}
	else if( m_AniState.GetCurState() == AT_UP )
	{
		// 감속 0->1->2
		int iYOffSet = sin( (PI/2 * m_AniState.GetCurTime()  ) / m_AniState.GetDelay() ) * m_iDownHeight;	
		m_pArrow->SetAlpha( 255 );
		m_pArrow->SetColor( 255, 255, 255 );
		m_pArrow->Render( iXPos + X_OFFSET, iYPos + ( Y_OFFSET_DOWN - iYOffSet ), UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void SoldierExerciseBtn::iwm_create()
{
	if( m_bArrow )
		InitAniState();
}

void SoldierExerciseBtn::iwm_mouseleave( const ioMouse& mouse )
{
	ioWnd::iwm_mouseleave( mouse );
}

void SoldierExerciseBtn::iwm_mouseover( const ioMouse& mouse )
{
	ioButton::iwm_mouseover( mouse );
}

void SoldierExerciseBtn::SetNeedTypeLv( ioSetItemInfo::NeedLevelType eType, int iNeedLv )
{
	m_eNeedLevelType = eType;
	m_iNeedLevel     = iNeedLv;
}

void SoldierExerciseBtn::ProcessNewEffect()
{
	if( m_iMarkType == SHOP_MARK_TYPE_NONE )
		return;

	if( m_dwNewEffectCurTime == 0 )
	{
		m_dwNewEffectCurTime = FRAMEGETTIME();
		return;
	}

	if( FRAMEGETTIME() - m_dwNewEffectCurTime < m_dwNewEffectWaitTime )
		return;
	m_dwNewEffectCurTime = FRAMEGETTIME();

	m_bMarkEffect = !m_bMarkEffect;
}