
#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/Safesprintf.h"
#include "../io3DEngine/ioStringManager.h"

#include "../Setting.h"
#include "../ioPlayMode.h"
#include "../ioMonsterSurvivalMode.h"
#include "../ioDungeonAMode.h"
#include "../TextColorTable.h"
#include "../ioBaseChar.h"

#include "CenterHelpWnd.h"

CenterHelpWnd::CenterHelpWnd()
{
	m_pHelpBack	  = NULL;
	m_dwFirstDelay= 0;
	m_iAlphaRate  = MAX_ALPHA_RATE;
	m_dwState	  = STATE_DELAY;
	m_dwCurrentTime = 0;
	m_dwFadeTime  = 0;
}

CenterHelpWnd::~CenterHelpWnd()
{
	SAFEDELETE( m_pHelpBack );
	m_Printer.ClearList();
}

void CenterHelpWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szShowSound		= xElement.GetStringAttribute_e( "ShowSound" );
	m_dwFadeTime		= xElement.GetIntAttribute_e( "FadeTime" );
}

void CenterHelpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "HelpBack" )
	{
		SAFEDELETE( m_pHelpBack );
		m_pHelpBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CenterHelpWnd::SetCenterHelp( ioComplexStringPrinter &rkPrinter, DWORD dwFirstDelay, DWORD dwShowTime )
{
	m_Printer.ClearList();
	rkPrinter.CopyFullText( m_Printer );
	m_dwFirstDelay  = dwFirstDelay;
	m_dwShowTime    = dwShowTime;

	SetState( STATE_DELAY );
	ShowWnd();
}

void CenterHelpWnd::iwm_show()
{
	if( !m_szShowSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szShowSound );
}

void CenterHelpWnd::iwm_hide()
{
	
}

void CenterHelpWnd::SetState( DWORD dwState )
{
	m_dwState       = dwState;
	m_dwCurrentTime = FRAMEGETTIME();
	m_dwFadeTime    = max( 100, m_dwFadeTime );
	switch( m_dwState )
	{
	case STATE_DELAY:
	case STATE_FADEIN:
		m_iAlphaRate = 0;
		break;
	case STATE_VIEW:
	case STATE_FADEOUT:
		m_iAlphaRate = MAX_ALPHA_RATE;
		break;
	}
}

void CenterHelpWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	DWORD dwGapTime  = FRAMEGETTIME() - m_dwCurrentTime;
	switch( m_dwState )
	{
	case STATE_DELAY:
		if( dwGapTime > m_dwFirstDelay )
		{
			SetState( STATE_FADEIN );
		}
		break;
	case STATE_FADEIN:
		if( dwGapTime < m_dwFadeTime )
		{
			m_iAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwGapTime / m_dwFadeTime );			
		}
		else
		{
			SetState( STATE_VIEW );
		}
		break;
	case STATE_VIEW:
		if( dwGapTime > m_dwShowTime )
		{
			SetState( STATE_FADEOUT );
		}
		break;
	case STATE_FADEOUT:
		if( dwGapTime < m_dwFadeTime )
		{
			m_iAlphaRate = MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * (float)( (float)dwGapTime / m_dwFadeTime ) );
		}
		else
		{
			HideWnd();
		}
		break;
	}
}

void CenterHelpWnd::OnRender()
{
	switch( m_dwState )
	{
	case STATE_FADEIN:
	case STATE_VIEW:
	case STATE_FADEOUT:
		ioWnd::OnRender();	
		int iXPos = GetDerivedPosX() + ( Setting::Width() / 2 );
		int iYPos = GetDerivedPosY() + ( Setting::Height() / 2 );
		if( m_pHelpBack )
		{
			m_pHelpBack->SetAlpha( m_iAlphaRate );
			m_pHelpBack->Render( iXPos - 215, iYPos + 114 );
		}
		m_Printer.PrintFullText( iXPos, iYPos + 102, TAT_CENTER, m_iAlphaRate );
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MonsterSurvivalHelWnd::MonsterSurvivalHelWnd()
{
	m_pContinueText	= NULL;
	m_pContinueCount= NULL;
	m_pClearText	= NULL;
	m_pGameOverText	= NULL;
	m_pKOText		= NULL;
	m_pRevivalDelay	= NULL;
	m_pAttackKeyText= NULL;
	m_pDeathTimeText= NULL;

	m_pPlayMode = NULL;

	m_dwContinueTime = m_dwCurContinueTime = m_dwCurContinueSec = 0;
	m_dwZoneCollisionTime = m_dwZoneCurCollisionTime = 0;
	m_dwCurGoldMonsterCoinTime = m_dwCurKOTime = 0;
	m_dwCurDeathTime = 0;
}

MonsterSurvivalHelWnd::~MonsterSurvivalHelWnd()
{
	SAFEDELETE( m_pContinueText );
	SAFEDELETE( m_pContinueCount );
	SAFEDELETE( m_pClearText );
	SAFEDELETE( m_pGameOverText );
	SAFEDELETE( m_pKOText );
	SAFEDELETE( m_pRevivalDelay );
	SAFEDELETE( m_pAttackKeyText );
	SAFEDELETE( m_pDeathTimeText );
}

void MonsterSurvivalHelWnd::SetPlayMode( ioPlayMode *pPlayMode )
{
	m_pPlayMode = pPlayMode;
}

void MonsterSurvivalHelWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ContinueText" )
	{
		SAFEDELETE( m_pContinueText );
		m_pContinueText = pImage;
	}
	else if( szType == "ContinueCount" )
	{
		SAFEDELETE( m_pContinueCount );
		m_pContinueCount = pImage;
	}
	else if( szType == "ClearText" )
	{
		SAFEDELETE( m_pClearText );
		m_pClearText = pImage;
	}
	else if( szType == "GameOverText" )
	{
		SAFEDELETE( m_pGameOverText );
		m_pGameOverText = pImage;
	}
	else if( szType == "KOText" )
	{
		SAFEDELETE( m_pKOText );
		m_pKOText = pImage;
	}
	else if( szType == "RevivalDelay" )
	{
		SAFEDELETE( m_pRevivalDelay );
		m_pRevivalDelay = pImage;
	}
	else if( szType == "AttackKeyText" )
	{
		SAFEDELETE( m_pAttackKeyText );
		m_pAttackKeyText = pImage;
	}
	else if( szType == "DeathTimeText" )
	{
		SAFEDELETE( m_pDeathTimeText );
		m_pDeathTimeText = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MonsterSurvivalHelWnd::SetContinueTime( DWORD dwContinueTime )
{
	m_dwContinueTime = dwContinueTime;
	if( m_dwContinueTime != 0 )
	{
		m_dwCurContinueTime = FRAMEGETTIME();	
		m_dwCurContinueSec  = 0xFFFFFFFF;
		m_dwCurContinueSecTime = FRAMEGETTIME();
	}
	else
	{
		m_dwCurContinueTime = 0;	
		m_dwCurContinueSec  = 0;
	}
}

void MonsterSurvivalHelWnd::SetZoneCollisionTime( DWORD dwZoneCollisionTime )
{
	m_dwZoneCollisionTime    = dwZoneCollisionTime;
	m_dwZoneCurCollisionTime = FRAMEGETTIME();
}

void MonsterSurvivalHelWnd::SetKOTime( DWORD dwCurTime )
{
	m_dwCurKOTime = dwCurTime;

	if( !m_pPlayMode ) return;
	if( m_pPlayMode->GetModeType() == MT_MONSTER_SURVIVAL )
	{
		ioMonsterSurvivalMode *pMonsterMode = ToMonsterSurvivalMode( m_pPlayMode );
		if( !pMonsterMode || pMonsterMode->IsAllUserDie() )	
			m_dwCurKOTime = 0;
	}
	else if( m_pPlayMode->GetModeType() == MT_DUNGEON_A )
	{
		ioDungeonAMode *pDungeonAMode = ToDungeonAMode( m_pPlayMode );
		if( !pDungeonAMode || pDungeonAMode->IsAllUserDie() )	
			m_dwCurKOTime = 0;
	}
}

void MonsterSurvivalHelWnd::SetDeathTime( DWORD dwCurTime )
{
	if( m_dwZoneCollisionTime == 0 && m_dwContinueTime == 0 )         // 클리어와 컨티뉴가 우선
		m_dwCurDeathTime = dwCurTime;
	else
		m_dwCurDeathTime = 0;
}

void MonsterSurvivalHelWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_pPlayMode && m_pPlayMode->IsRoundSetEnd() )
	{
		HideWnd();
	}
	else
	{
		OnKOProcess();
		OnDeathTimeProcess();
	}
}

void MonsterSurvivalHelWnd::OnKOProcess()
{
	if( m_dwCurKOTime == 0 ) return;
	if( FRAMEGETTIME() - m_dwCurKOTime > 2000 )
	{
		m_dwCurKOTime = 0;
	}
	else
	{
		if( !m_pPlayMode )
		{
			m_dwCurKOTime = 0;
		}
		else if( m_pPlayMode->GetModeType() == MT_MONSTER_SURVIVAL )
		{
			ioMonsterSurvivalMode *pMonsterMode = ToMonsterSurvivalMode( m_pPlayMode );
			if( !pMonsterMode || pMonsterMode->IsAllUserDie() )
			{
				m_dwCurKOTime = 0;
			}
		}
		else if( m_pPlayMode->GetModeType() == MT_DUNGEON_A )
		{
			ioDungeonAMode *pDungeonAMode = ToDungeonAMode( m_pPlayMode );
			if( !pDungeonAMode || pDungeonAMode->IsAllUserDie() )
			{
				m_dwCurKOTime = 0;
			}
		}
		else
		{
			ioBaseChar *pOwnerChar = m_pPlayMode->GetOwnerChar();
			if( !pOwnerChar || !pOwnerChar->IsPrisonerMode() && pOwnerChar->GetState() != CS_DIE )
			{
				m_dwCurKOTime = 0;
			}
		}
	}
}

void MonsterSurvivalHelWnd::OnDeathTimeProcess()
{
	if( m_dwCurDeathTime == 0 ) return;
	
	if( FRAMEGETTIME() - m_dwCurDeathTime > 2000 )
		m_dwCurDeathTime = 0;
}

void MonsterSurvivalHelWnd::OnRender()
{
	ioWnd::OnRender();	
	int iXPos = GetDerivedPosX() + ( Setting::Width() / 2 );
	int iYPos = GetDerivedPosY() + ( Setting::Height() / 2 );

	RenderGoldMonsterCoinHelp( iXPos, iYPos );
	RenderContinueHelp( iXPos, iYPos );
	RenderMagicZoneHelp( iXPos, iYPos );
	RenderKOHelp( iXPos, iYPos );
	RenderDeathTimeHelp( iXPos, iYPos );
}

bool MonsterSurvivalHelWnd::IsRenderGoldMonsterCoinHelp()
{
	if( !m_pPlayMode ) return false;

	if( m_pPlayMode->GetModeType() == MT_MONSTER_SURVIVAL )
	{
		ioMonsterSurvivalMode *pMonsterMode = ToMonsterSurvivalMode( m_pPlayMode );
		if( !pMonsterMode ) return false;

		return pMonsterMode->IsRenderGoldMonsterCoinHelp();
	}

	ioDungeonAMode *pDungeonAMode = ToDungeonAMode( m_pPlayMode );
	if( !pDungeonAMode ) return false;

	return pDungeonAMode->IsRenderGoldMonsterCoinHelp();
}

void MonsterSurvivalHelWnd::RenderGoldMonsterCoinHelp( int iXPos, int iYPos )
{
	if( !IsRenderGoldMonsterCoinHelp() )
	{
		m_dwCurGoldMonsterCoinTime = FRAMEGETTIME();
		return;
	}
    
	bool bAllUserDie = false;
	
	if( !m_pPlayMode ) return;

	if( m_pPlayMode->GetModeType() == MT_MONSTER_SURVIVAL )
	{
		ioMonsterSurvivalMode *pMonsterMode = ToMonsterSurvivalMode( m_pPlayMode );
		if( !pMonsterMode ) return;	
		if( !m_pRevivalDelay || !m_pAttackKeyText ) return;
		if( m_dwCurDeathTime != 0 ) return;

		bAllUserDie = pMonsterMode->IsAllUserDie();
	}
	else if( m_pPlayMode->GetModeType() == MT_DUNGEON_A )
	{
		ioDungeonAMode *pDungeonAMode = ToDungeonAMode( m_pPlayMode );
		if( !pDungeonAMode ) return;	
		if( !m_pRevivalDelay || !m_pAttackKeyText ) return;
		if( m_dwCurDeathTime != 0 ) return;

		bAllUserDie = true;
	}

	{
		int iAlphaRate  = MAX_ALPHA_RATE;
		DWORD dwGapTime = FRAMEGETTIME() - m_dwCurGoldMonsterCoinTime;
		DWORD dwFadeTime = 300;
		if( dwGapTime < dwFadeTime )
			iAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwGapTime / dwFadeTime );

		// 부활 대기중
		if( !bAllUserDie )
		{
			m_pRevivalDelay->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
			m_pRevivalDelay->SetAlpha( iAlphaRate );
			m_pRevivalDelay->Render( iXPos, iYPos + 42 );
		}
		//
		char szHelpText[MAX_PATH] = "";
		SafeSprintf( szHelpText, sizeof( szHelpText ), STR(1) );
		int iTextResSize = 41; // 41은 이미지 사이즈
		int iCenterSize = ( g_FontMgr.GetTextWidth( szHelpText, TS_OUTLINE_FULL_2X, 0.83f ) + iTextResSize ) / 2;     
		m_pAttackKeyText->SetAlpha( iAlphaRate );
		m_pAttackKeyText->Render( iXPos - iCenterSize, iYPos + 98 );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_YELLOW );
		g_FontMgr.PrintTextAlpha( ( iXPos - iCenterSize ) + iTextResSize, iYPos + 97, 0.83f, iAlphaRate, STR(2), szHelpText );
	}
}

void MonsterSurvivalHelWnd::RenderContinueHelp( int iXPos, int iYPos )
{
	if( m_dwContinueTime == 0 ) return;
	if( m_dwZoneCollisionTime != 0 ) return;
	if( !m_pGameOverText || !m_pContinueText || !m_pContinueCount ) return;

	DWORD dwGameOverTime = 1000;
	DWORD dwGap = FRAMEGETTIME() - m_dwCurContinueTime;
	if( dwGap <= dwGameOverTime )       //
	{
		int iAlphaRate   = MAX_ALPHA_RATE;
		if( dwGap < 100 )
			iAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwGap / 100 );
		else if( dwGap > 800 )
		{
			dwGap -= 800;
			iAlphaRate = MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * min( FLOAT1, (float)( (float)dwGap / 200 ) ) );
		}

		m_pGameOverText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		m_pGameOverText->SetAlpha( iAlphaRate );
		m_pGameOverText->Render( iXPos, iYPos - 74 );	
	}
	else
	{
		DWORD dwAlphaGap = dwGap - dwGameOverTime;
		int iAlphaRate   = MAX_ALPHA_RATE;
		DWORD dwFadeTime = 300;
		if( dwAlphaGap < dwFadeTime )
			iAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwAlphaGap / dwFadeTime );

		m_pContinueText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		m_pContinueText->SetAlpha( iAlphaRate );
		m_pContinueText->Render( iXPos, iYPos - 140 );

		DWORD dwMaxSec = ( m_dwContinueTime / 1000 );
		DWORD dwNextSec = dwGap / 1000;
		if( m_dwCurContinueSec != dwNextSec )
		{
			m_dwCurContinueSecTime = FRAMEGETTIME();
			m_dwCurContinueSec = dwNextSec;	
			if( m_dwCurContinueSec > dwMaxSec )
				m_dwCurContinueSec = dwMaxSec;
		}
		DWORD dwSecGapTime= FRAMEGETTIME() - m_dwCurContinueSecTime;
		int iSecAlphaRate = MAX_ALPHA_RATE;
		DWORD dwSecFadeTime = 200;
		DWORD dwSecFadeOutTime = 800;
		if( dwSecGapTime < dwSecFadeTime )
			iSecAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwSecGapTime / dwSecFadeTime );
/*		else if( dwSecGapTime > dwSecFadeOutTime )
		{
			dwSecGapTime -= dwSecFadeOutTime;
			iSecAlphaRate = MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * min( FLOAT1, (float)( (float)dwSecGapTime / dwSecFadeTime ) ) );
		}
*/
		m_pContinueCount->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		m_pContinueCount->SetAlpha( iSecAlphaRate );
		int iNumber = dwMaxSec - m_dwCurContinueSec;
		m_pContinueCount->RenderNum( iXPos - 14, iYPos - 110, dwMaxSec - m_dwCurContinueSec, -28.0f );

		if( m_pPlayMode && m_pPlayMode->GetModeType() != MT_DUNGEON_A )
		{
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintTextAlpha( iXPos, iYPos + 65, 0.83f, iAlphaRate, STR(1) );
		}
	}	
}

void MonsterSurvivalHelWnd::RenderMagicZoneHelp( int iXPos, int iYPos )
{
	if( m_dwZoneCollisionTime == 0 ) return;
	if( !m_pClearText ) return;

	int iAlphaRate  = MAX_ALPHA_RATE;
	DWORD dwGapTime = FRAMEGETTIME() - m_dwZoneCurCollisionTime;
	DWORD dwFadeTime = 300;
	if( dwGapTime < dwFadeTime )
		iAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwGapTime / dwFadeTime );

	m_pClearText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
	m_pClearText->SetAlpha( iAlphaRate );
	m_pClearText->Render( iXPos, iYPos + 42 );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( 0, 142, 254 );
	g_FontMgr.PrintTextAlpha( iXPos, iYPos + 129, 0.83f, iAlphaRate, STR(1) );
}

void MonsterSurvivalHelWnd::RenderKOHelp( int iXPos, int iYPos )
{
	if( m_dwCurKOTime == 0 ) return;
	if( !m_pKOText ) return;

	DWORD dwGap = FRAMEGETTIME() - m_dwCurKOTime;
	int iAlphaRate   = MAX_ALPHA_RATE;
	DWORD dwFadeTime = 300;
	DWORD dwFadeOutTime = 1700;
	if( dwGap < dwFadeTime )
		iAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwGap / dwFadeTime );
	else if( dwGap > dwFadeOutTime )
	{
		dwGap -= dwFadeOutTime;
		iAlphaRate = MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * min( FLOAT1, (float)( (float)dwGap / dwFadeTime ) ) );
	}

	m_pKOText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
	m_pKOText->SetAlpha( iAlphaRate );
	m_pKOText->Render( iXPos, iYPos - 85 );		
}

void MonsterSurvivalHelWnd::RenderDeathTimeHelp( int iXPos, int iYPos )
{
	if( m_dwCurDeathTime == 0 ) return;
	if( !m_pDeathTimeText ) return;
	
	DWORD dwGap = FRAMEGETTIME() - m_dwCurDeathTime;
	int iAlphaRate   = MAX_ALPHA_RATE;
	DWORD dwFadeTime = 300;
	DWORD dwFadeOutTime = 1700;
	if( dwGap < dwFadeTime )
		iAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwGap / dwFadeTime );
	else if( dwGap > dwFadeOutTime )
	{
		dwGap -= dwFadeOutTime;
		iAlphaRate = MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * min( FLOAT1, (float)( (float)dwGap / dwFadeTime ) ) );
	}

	m_pDeathTimeText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
	m_pDeathTimeText->SetAlpha( iAlphaRate );
	m_pDeathTimeText->Render( iXPos, iYPos + 66 );	

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintTextAlpha( iXPos, iYPos + 129, 0.83f, iAlphaRate, STR(1) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShuffleCenterHelpWnd::ShuffleCenterHelpWnd()
{
	m_pAddMonsterText = NULL;
	m_pAddStarText    = NULL;
	m_pAddStarIcon	  = NULL;

	m_pAddBuffBoxText = NULL;
	m_pAddBuffBoxIcon = NULL;

	m_pGetBuffBoxText = NULL;
	m_pGetBuffBoxIcon = NULL;
	
	m_iShowType		= 0;

	m_dwFirstDelay  = 0;
	m_iAlphaRate    = MAX_ALPHA_RATE;
	m_dwState	    = STATE_DELAY;
	m_dwCurrentTime = 0;
	m_dwFadeTime    = 0;
}

ShuffleCenterHelpWnd::~ShuffleCenterHelpWnd()
{
	SAFEDELETE( m_pAddMonsterText );
	SAFEDELETE( m_pAddStarText );
	SAFEDELETE( m_pAddStarIcon );	

	SAFEDELETE( m_pAddBuffBoxText );
	SAFEDELETE( m_pAddBuffBoxIcon );
	SAFEDELETE( m_pGetBuffBoxText );
	SAFEDELETE( m_pGetBuffBoxIcon );

	m_Printer.ClearList();
}

void ShuffleCenterHelpWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szShowSound = xElement.GetStringAttribute_e( "ShowSound" );
	m_dwFadeTime  = xElement.GetIntAttribute_e( "FadeTime" );
}

void ShuffleCenterHelpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "AddMonsterText" )
	{
		SAFEDELETE( m_pAddMonsterText );
		m_pAddMonsterText = pImage;
	}
	else if( szType == "AddStarText" )
	{
		SAFEDELETE( m_pAddStarText );
		m_pAddStarText = pImage;
	}
	else if( szType == "AddBuffBoxText" )
	{
		SAFEDELETE( m_pAddBuffBoxText );
		m_pAddBuffBoxText = pImage;
	}
	else if( szType == "GetBuffBoxText" )
	{
		SAFEDELETE( m_pGetBuffBoxText );
		m_pGetBuffBoxText = pImage;
	}
	else if( szType == "AddBuffBoxIcon" )
	{
		SAFEDELETE( m_pAddBuffBoxIcon );
		m_pAddBuffBoxIcon = pImage;
	}
	else if( szType == "AddStarIcon" )
	{
		SAFEDELETE( m_pAddStarIcon );
		m_pAddStarIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ShuffleCenterHelpWnd::SetCenterHelp( int iShowType, const ioHashString& szMentText, float fScale, DWORD dwColor, DWORD dwFirstDelay, DWORD dwShowTime )
{
	 //몬스터 출현 멘트중이면 다른 멘트 무시..
	if( m_iShowType == ST_ADD_MONSTER )
		return;

	m_iShowType = iShowType;

	m_Printer.ClearList();
	m_Printer.SetBkColor( 0, 0, 0 );
	m_Printer.SetTextStyle( TS_OUTLINE_FULL_2X  );
	m_Printer.SetTextColor( dwColor );
	m_Printer.AddTextPiece( fScale, szMentText.c_str() );

	m_dwFirstDelay = dwFirstDelay;
	m_dwShowTime   = dwShowTime;

	SetState( STATE_DELAY );
	ShowWnd();
}

void ShuffleCenterHelpWnd::SetCenterHelpGetBuffItem( int iShowType, 
													 const ioHashString& szChrName,
													 const ioHashString& szItemName,
													 const ioHashString& szIconName,
													 float fScale,
													 DWORD dwColor,
													 DWORD dwFirstDelay,
													 DWORD dwShowTime )
{
	//몬스터 출현 멘트중이면 다른 멘트 무시..
	if( m_iShowType == ST_ADD_MONSTER )
		return;

	m_iShowType = iShowType;

	m_Printer.ClearList();
	m_Printer.SetBkColor( 0, 0, 0 );
	m_Printer.SetTextStyle( TS_OUTLINE_FULL_2X  );
	m_Printer.SetTextColor( 0xFF0199FE );
	m_Printer.AddTextPiece( fScale, szChrName.c_str() );

	m_Printer.SetTextColor( 0xFFFFFFFF );
	m_Printer.AddTextPiece( fScale, "님 " );

	m_Printer.SetTextColor( dwColor );
	m_Printer.AddTextPiece( fScale, szItemName.c_str() );

	m_Printer.SetTextColor( 0xFFFFFFFF );
	m_Printer.AddTextPiece( fScale, " 획득" );

	m_dwFirstDelay = dwFirstDelay;
	m_dwShowTime   = dwShowTime;

	SetGetBuffItemIcon( szIconName );
	SetState( STATE_DELAY );
	ShowWnd();
}

void ShuffleCenterHelpWnd::SetGetBuffItemIcon( const ioHashString& szIconName )
{
	if( m_iShowType == ST_GET_BUFF_BOX && !szIconName.IsEmpty() )
	{
		SAFEDELETE( m_pGetBuffBoxIcon );
		m_pGetBuffBoxIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		if( m_pGetBuffBoxIcon )
		{
			m_pGetBuffBoxIcon->SetPosition( 0, 208 );
			m_pGetBuffBoxIcon->SetScale( 0.65f );
		}
	}
}

void ShuffleCenterHelpWnd::iwm_show()
{
	if( !m_szShowSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szShowSound );
}

void ShuffleCenterHelpWnd::iwm_hide()
{

}

void ShuffleCenterHelpWnd::SetState( DWORD dwState )
{
	m_dwState       = dwState;
	m_dwCurrentTime = FRAMEGETTIME();
	m_dwFadeTime    = max( 100, m_dwFadeTime );

	switch( m_dwState )
	{
	case STATE_DELAY:
	case STATE_FADEIN:
		m_iAlphaRate = 0;
		break;
	case STATE_VIEW:
	case STATE_FADEOUT:
		m_iAlphaRate = MAX_ALPHA_RATE;
		break;
	}
}

void ShuffleCenterHelpWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	DWORD dwGapTime  = FRAMEGETTIME() - m_dwCurrentTime;
	switch( m_dwState )
	{
	case STATE_DELAY:
		if( dwGapTime > m_dwFirstDelay )
		{
			SetState( STATE_FADEIN );
		}
		break;
	case STATE_FADEIN:
		if( dwGapTime < m_dwFadeTime )
		{
			m_iAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwGapTime / m_dwFadeTime );			
		}
		else
		{
			SetState( STATE_VIEW );
		}
		break;
	case STATE_VIEW:
		if( dwGapTime > m_dwShowTime )
		{
			SetState( STATE_FADEOUT );
		}
		break;
	case STATE_FADEOUT:
		if( dwGapTime < m_dwFadeTime )
		{
			m_iAlphaRate = MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * (float)( (float)dwGapTime / m_dwFadeTime ) );
		}
		else
		{
			m_iShowType = ST_NONE;
			HideWnd();
		}
		break;
	}
}

void ShuffleCenterHelpWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	iXPos += ( Setting::Width() / 2 );	

	switch( m_dwState )
	{
	case STATE_FADEIN:
	case STATE_VIEW:
	case STATE_FADEOUT:
		{
			ioWnd::OnRender();

			switch( m_iShowType )
			{
			case ST_ADD_MONSTER:
				{
					iYPos += ( Setting::Height() / 2 );
					OnAddMonsterRender( iXPos, iYPos );
				}
				break;
			case ST_ADD_STAR:
				{
					OnIconAndTextRender( iXPos, iYPos, m_pAddStarIcon, m_pAddStarText );
				}
				break;
			case ST_ADD_BUFF_BOX:
				{
					OnIconAndTextRender( iXPos, iYPos, m_pAddBuffBoxIcon, m_pAddBuffBoxText );
				}
				break;
			case ST_GET_BUFF_BOX:
				{
					OnIconAndTextRender( iXPos, iYPos, m_pGetBuffBoxIcon, m_pGetBuffBoxText );
				}
				break;
			}
		}
		break;
	}
}

void ShuffleCenterHelpWnd::OnAddMonsterRender( int iXPos, int iYPos )
{
	if( !m_pAddMonsterText )
		return;
	
	m_pAddMonsterText->SetAlpha( m_iAlphaRate );
	m_pAddMonsterText->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	m_Printer.PrintFullText( iXPos, iYPos + 129, TAT_CENTER );
}

void ShuffleCenterHelpWnd::OnIconAndTextRender( int iXPos, int iYPos, ioUIRenderImage* pIcon, ioUIRenderImage* pText )
{
	if( !pText || !pIcon )
		return;

	int iIconWidth = pIcon->GetWidth() * pIcon->GetScale().x;
	int iX = iXPos - ( pText->GetWidth() + iIconWidth ) * FLOAT05;

	int iOffsetX = pIcon->GetOffsetX() * pIcon->GetScale().x;
	int iOffsetY = pIcon->GetOffsetY() * pIcon->GetScale().y;

	pIcon->SetAlpha( m_iAlphaRate );
	pIcon->Render( iX + iOffsetX, iYPos + iOffsetY * FLOAT05, UI_RENDER_NORMAL, TFO_BILINEAR );

	pText->SetAlpha( m_iAlphaRate );
	pText->Render( iX + iIconWidth, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

	m_Printer.PrintFullText( iXPos, iYPos + 247, TAT_CENTER, m_iAlphaRate );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RaidReadyCountWnd::RaidReadyCountWnd()
{
	m_pCountText	 = NULL;
	m_pBgEffectText = NULL;
	m_pGoText = NULL;

	m_pPlayMode = NULL;

	m_RenderType = RT_None;

	for (int i=0;i<10;i++)
		m_pCountNum[i] = NULL;
}

RaidReadyCountWnd::~RaidReadyCountWnd()
{
	SAFEDELETE( m_pCountText );
	SAFEDELETE( m_pBgEffectText );
	SAFEDELETE( m_pGoText );

	for (int i=0;i<10;i++)
		SAFEDELETE( m_pCountNum[i] );
}

void RaidReadyCountWnd::SetPlayMode( ioPlayMode *pPlayMode )
{
	m_pPlayMode = pPlayMode;
}

void RaidReadyCountWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );

	m_szCountSound = xElement.GetStringAttribute_e( "Raid_Count" );
	m_szGoSound = xElement.GetStringAttribute_e( "Raid_Go" );
}

void RaidReadyCountWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CountText" )
	{
		SAFEDELETE( m_pCountText );
		m_pCountText = pImage;
	}
	else if( szType == "CountNum1" )
	{
		SAFEDELETE( m_pCountNum[0] );
		m_pCountNum[0] = pImage;
	}
	else if( szType == "CountNum2" )
	{
		SAFEDELETE( m_pCountNum[1] );
		m_pCountNum[1] = pImage;
	}
	else if( szType == "CountNum3" )
	{
		SAFEDELETE( m_pCountNum[2] );
		m_pCountNum[2] = pImage;
	}
	else if( szType == "CountNum4" )
	{
		SAFEDELETE( m_pCountNum[3] );
		m_pCountNum[3] = pImage;
	}
	else if( szType == "CountNum5" )
	{
		SAFEDELETE( m_pCountNum[4] );
		m_pCountNum[4] = pImage;
	}
	else if( szType == "CountNum6" )
	{
		SAFEDELETE( m_pCountNum[5] );
		m_pCountNum[5] = pImage;
	}
	else if( szType == "CountNum7" )
	{
		SAFEDELETE( m_pCountNum[6] );
		m_pCountNum[6] = pImage;
	}
	else if( szType == "CountNum8" )
	{
		SAFEDELETE( m_pCountNum[7] );
		m_pCountNum[7] = pImage;
	}
	else if( szType == "CountNum9" )
	{
		SAFEDELETE( m_pCountNum[8] );
		m_pCountNum[8] = pImage;
	}
	else if( szType == "CountNum10" )
	{
		SAFEDELETE( m_pCountNum[9] );
		m_pCountNum[9] = pImage;
	}
	else if( szType == "BgEffect" )
	{
		SAFEDELETE( m_pBgEffectText );
		m_pBgEffectText = pImage;
	}
	else if( szType == "GoText" )
	{
		SAFEDELETE( m_pGoText );
		m_pGoText = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void RaidReadyCountWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_pPlayMode && m_pPlayMode->IsRoundSetEnd() )
		HideWnd();

	switch(m_RenderType)
	{
	case RT_Count:
		ProcessCount();
		break;
	case RT_Go:
		ProcessGO();
		break;
	}
}

void RaidReadyCountWnd::OnRender()
{
	ioWnd::OnRender();	
	int iXPos = GetDerivedPosX() + ( Setting::Width() / 2 );
	int iYPos = GetDerivedPosY() + ( Setting::Height() / 2 );
	
	switch(m_RenderType)
	{
	case RT_Count:
		RenderCount( iXPos, iYPos );
		break;
	case RT_Go:
		RenderGO( iXPos, iYPos );
		break;
	}
}

void RaidReadyCountWnd::ProcessCount()
{	
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGap = dwCurTime - m_dwCurCountTime;
	DWORD dwGapSec = dwGap / 1000;

	if( m_dwCurCountSec != dwGapSec )
	{
		m_dwCurCountChangeTime = dwCurTime;
		m_dwCurCountSec = dwGapSec;
		g_SoundMgr.PlaySound( m_szCountSound );
	}

	if ( m_dwCurCountSec < m_dwMaxCountSec )
	{
		m_nCountTextAlpha = MAX_ALPHA_RATE;
		m_nCountNumAlpha = MAX_ALPHA_RATE;

		if( dwGap < FT_Count_Text )
			m_nCountTextAlpha = (float)MAX_ALPHA_RATE * (float)( (float)dwGap / FT_Count_Text );

		DWORD dwSecGapTime = dwCurTime - m_dwCurCountChangeTime;
		if( dwSecGapTime < FT_Count_Num )
			m_nCountNumAlpha = (float)MAX_ALPHA_RATE * (float)( (float)dwSecGapTime / FT_Count_Num );
	}
	else
		SetGoState();
}

void RaidReadyCountWnd::ProcessGO()
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwGoTime;

	switch( m_GoState )
	{
	case GS_Start:
		{
			if( dwGapTime <= FT_Go_Start )
				m_fGoBgAlphaRate = FLOAT1 - ( (float)dwGapTime / FT_Go_Text );
			else
			{
				m_GoState = GS_Delay;
				m_dwGoTime = dwCurTime;
				m_fGoTextAlphaRate = FLOAT1;
			}
		}
		break;
	case GS_Delay:
		{
			if( dwGapTime > FT_Go_Delay )
			{
				m_GoState = GS_Text_Fade;
				m_dwGoTime = dwCurTime;
				m_fGoTextAlphaRate = FLOAT1;
			}
		}
		break;
	case GS_Text_Fade:
		{
			if( dwGapTime <= FT_Go_Text )
				m_fGoTextAlphaRate = FLOAT1 - ( (float)dwGapTime / FT_Go_Text );
			else
				HideWnd();
		}
		break;
	}
	
	DWORD dwEffectGapTime = dwCurTime - m_dwGoEffectTime;
	float fTimeRate = (float)dwEffectGapTime / FT_Go_Effect;
	if( fTimeRate < FLOAT1 )
	{
		float fCurRate = sinf( ( D3DX_PI / 2 ) * fTimeRate );

		m_fGoEffectAlphaRate = FLOAT1 - fCurRate;
		m_fGoEffectScaleRate = FLOAT1 + (1.2f * fCurRate);
	}
	else
	{
		m_fGoEffectAlphaRate = 0.0f;
		m_fGoEffectScaleRate = 0.0f;
	}
}

void RaidReadyCountWnd::RenderCount( int iXPos, int iYPos )
{
	if( m_dwMaxCountSec == 0 ) 
		return;

	if( !m_pCountText || !m_pBgEffectText ) 
		return;
	
	for (int i=0; i<10; i++)
	{
		if( !m_pCountNum[i] )
			return;
	}

	m_pBgEffectText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
	m_pBgEffectText->SetAlpha( MAX_ALPHA_RATE );
	m_pBgEffectText->SetScale( 8.0f );
	m_pBgEffectText->Render( iXPos, iYPos - 35, UI_RENDER_NORMAL, TFO_BILINEAR );

	m_pCountText->SetAlpha( m_nCountTextAlpha );
	m_pCountText->Render( iXPos, iYPos - 162 );

	int nCount = m_dwMaxCountSec-m_dwCurCountSec-1;
	if (COMPARE( nCount,0,10 ))
	{
		m_pCountNum[nCount]->SetAlpha( m_nCountNumAlpha );
		m_pCountNum[nCount]->Render( iXPos, iYPos - 35 );
	}

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintTextAlpha( iXPos, iYPos + 59, FONT_SIZE_16, m_nCountTextAlpha, "10초 후 보스가 등장합니다." );
}

void RaidReadyCountWnd::RenderGO( int iXPos, int iYPos )
{
	if (!m_pGoText || !m_pBgEffectText)
		return;
	
	switch(m_GoState)
	{
	case GS_Start:
		{
			m_pBgEffectText->SetAlpha( m_fGoBgAlphaRate );
			m_pBgEffectText->Render( iXPos, iYPos );

			m_pGoText->SetScale( FLOAT1 );
			m_pGoText->SetAlpha( (float)MAX_ALPHA_RATE );
			m_pGoText->Render( iXPos, iYPos );
		}
		break;
	case GS_Delay:
		{
			m_pGoText->SetScale( FLOAT1 );
			m_pGoText->SetAlpha( (float)MAX_ALPHA_RATE );
			m_pGoText->Render( iXPos, iYPos );
		}
		break;
	case GS_Text_Fade:
		{
			m_pGoText->SetScale( FLOAT1 );
			m_pGoText->SetAlpha( (float)MAX_ALPHA_RATE * m_fGoTextAlphaRate );
			m_pGoText->Render( iXPos, iYPos );
		}
		break;
	}

	if( m_fGoEffectAlphaRate > 0.0f )
	{
		m_pGoText->SetScale( m_fGoEffectScaleRate );
		m_pGoText->SetAlpha( (float)MAX_ALPHA_RATE * m_fGoEffectAlphaRate );
		m_pGoText->Render( iXPos, iYPos );
	}
}

void RaidReadyCountWnd::SetCountState( DWORD dwMaxCountTime )
{
	m_RenderType = RT_Count;
	m_nCountTextAlpha = 0;
	m_nCountNumAlpha = 0;

	m_dwMaxCountSec = dwMaxCountTime / 1000;

	if( m_dwMaxCountSec != 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		m_dwCurCountTime = dwCurTime;
		m_dwCurCountChangeTime = dwCurTime;
		m_dwCurCountSec  = 0xFFFFFFFF;
		ShowWnd();
	}
	else
	{
		m_dwCurCountTime = 0;	
		m_dwCurCountSec  = 0;
		HideWnd();
	}
}

void RaidReadyCountWnd::SetGoState()
{
	m_RenderType = RT_Go;
	m_GoState = GS_Start;
	m_fGoTextAlphaRate = 0.0f;
	m_fGoBgAlphaRate = 0.0f;
	m_fGoEffectAlphaRate = 0.0f;
	m_fGoEffectScaleRate = 0.0f;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwGoTime = dwCurTime;
	m_dwGoEffectTime = dwCurTime;

	g_SoundMgr.PlaySound( m_szGoSound );
}