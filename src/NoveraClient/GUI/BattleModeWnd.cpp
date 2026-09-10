#include "StdAfx.h"

#include "../ioVoiceChat/Fourier.h"
#include "../io2DCircleGauge.h"
#include "../ioNpcChar.h"
#include "../ioGuildMarkMgr.h"
#include "../ioTournamentManager.h"
#include "../ioTournamentTeamManager.h"

#include "BattleModeWnd.h"
#include "UserInfoToolTip.h"
#include "SoldierSelectWnd.h"

#include "../ioBattleMode.h"

class ioCircleGaugeQuad : public IUIQuad
{
protected:
	io2DCircleGauge *m_pGauge;

public:
	virtual void AddInfo( ioQuadRenderArray *pQuadArray )
	{
		pQuadArray->AddDirectQuadInfo( this );
	}

	virtual int GetQuadCount() const
	{
		return 0;
	}

public:
	virtual bool IsDirectQuad() const
	{
		return true;
	}

	virtual void RenderDirect( ioRenderSystem *pRenderSystem )
	{
		if( m_pGauge )
		{
			m_pGauge->Render( pRenderSystem );
		}
	}

public:
	ioCircleGaugeQuad( io2DCircleGauge *pGauge )
	{
		m_pGauge = pGauge;
	}
	virtual ~ioCircleGaugeQuad() {}
};
//////////////////////////////////////////////////////////////////////////
BattleMode3DChar::BattleMode3DChar()
{
	m_pUI3DRender = NULL;
	m_pAvata      = NULL;
	
	m_iOffsetXPos = 0;
	m_iOffsetYPos = 0;
}

BattleMode3DChar::~BattleMode3DChar()
{
	SAFEDELETE( m_pAvata );
	SAFEDELETE( m_pUI3DRender );
}

void BattleMode3DChar::Initialize()
{
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void BattleMode3DChar::SetChar( ioBaseChar *pChar, bool bLeftChar )
{
	if( !pChar ) return;
	if( !m_pUI3DRender )	return;

	const CHARACTER &rkInfo = pChar->GetCharacterInfo();

	SAFEDELETE( m_pAvata );
	m_pAvata = m_pUI3DRender->CreateChar( rkInfo, "ResultChar" );
	if( !m_pAvata )
	{
		LOG.PrintTimeAndLog( 0, "BattleMode3DChar::SetAvata - CreateAvata Fail" );
		return;
	}

	m_pAvata->DestroyHandAndBody();

	if( bLeftChar )
	{
		D3DXVECTOR3 vCharPos( 0.0f, 0.0f, 0.0f );
		m_pAvata->SetPosition( vCharPos );
		m_pAvata->SetScale( 1.5f );
		m_pAvata->SetYaw( 130.0f );
		m_pUI3DRender->SetLookAt( vCharPos );
		m_iOffsetXPos = -165;
		m_iOffsetYPos = 50;
	}
	else
	{
		D3DXVECTOR3 vCharPos( 0.0f, 0.0f, 0.0f );
		m_pAvata->SetPosition( vCharPos );
		m_pAvata->SetScale( 1.50f );
		m_pAvata->SetYaw( 220.0f );
		m_pUI3DRender->SetLookAt( vCharPos );
		m_iOffsetXPos = -200;
		m_iOffsetYPos = 50;
	}

	// 코스튬 관련 (챔피언모드 헬멧)
	ioHelmetItem *pItem = pChar->GetHelmet();
	int TempCostume = 200001;
	if( pItem )
	{
		m_pAvata->EquipItem( pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom(), pItem->GetCostumeCode() );
	}
	else
	{
		m_pAvata->ReleaseItem( ES_HELMET );
	}
	m_pAvata->SetLoopAni( "public_delay.ani" );
}

void BattleMode3DChar::FaceChange( const char *szTexPostfix, int iDuration )
{
	if( m_pAvata )
	{
		m_pAvata->DirectFaceChange( szTexPostfix, iDuration );
	}
}

void BattleMode3DChar::SetHelmChange( ioBaseChar *pChar )
{
	if( pChar == NULL ) 
		return;

	if( m_pAvata )
	{
		ioHelmetItem *pHelmet = pChar->GetHelmet();
		if( pHelmet )
			m_pAvata->EquipItem( pHelmet->GetItemCode(), pHelmet->GetItemMaleCustom(), pHelmet->GetItemFemaleCustom() );
		else
			m_pAvata->ReleaseItem( ES_HELMET );
	}
}

void BattleMode3DChar::Update( float fTimePerSec )
{
	if( m_pAvata )
	{
		m_pAvata->Update( fTimePerSec );
	}
}

void BattleMode3DChar::Render( int iXPos, int iYPos )
{
	if( m_pAvata && m_pUI3DRender )
	{
		D3DXMATRIX matUI = m_pUI3DRender->MakeUIProjection( iXPos + m_iOffsetXPos, iYPos + m_iOffsetYPos );
		m_pUI3DRender->Render( &matUI, true );
	}
}
//////////////////////////////////////////////////////////////////////////
bool BattleModeTimeGaugeWnd::m_bPosToDefualt = false;
BattleModeTimeGaugeWnd::BattleModeTimeGaugeWnd() : m_pPlayStage( NULL )
{
	m_pCircleGauge = NULL;
	m_pCircleSuddenDeathGauge = NULL;

	m_pTimeIcon    = NULL;
	m_pTimeNumber  = NULL;

	m_pBack         = NULL;
	m_pMold         = NULL;
	m_pWarningBack  = NULL;

	m_pTimeText			= NULL;
	m_pTimeTextRed		= NULL;
	m_pTimeNumText		= NULL;
	m_pTimeNumTextRed	= NULL;
	m_pTimeBack			= NULL;
	m_pTimeBackRed		= NULL;

	m_pPowerBackEffectL	= NULL;
	m_pPowerBackGaugeL	= NULL;
	m_pPowerGaugeL		= NULL;
	m_pPowerGaugeEffectL= NULL;
	m_pPowerGaugeMaxL	= NULL;
	m_pPowerBackEffectR	= NULL;
	m_pPowerBackGaugeR	= NULL;
	m_pPowerGaugeR		= NULL;
	m_pPowerGaugeEffectR= NULL;
	m_pPowerGaugeMaxR	= NULL;

	m_pBlueTeamText		= NULL;
	m_pLeftBackLine		= NULL;
	m_pLeftBackLineLight= NULL;
	m_pLeftGaugeBack	= NULL;
	m_pLeftGauge100		= NULL;
	m_pLeftGaugeGlow	= NULL;
	m_pLeftGaugeRed		= NULL;
	m_pLeftBlueNameBar	= NULL;
	m_pLeftRedNameBar	= NULL;

	m_pRedTeamText	= NULL;
	m_pRightBackLine	= NULL;
	m_pRightBackLineLight= NULL;
	m_pRightGaugeBack	= NULL;
	m_pRightGauge100	= NULL;
	m_pRightGaugeGlow	= NULL;
	m_pRightGaugeRed	= NULL;
	m_pRightBlueNameBar	= NULL;
	m_pRightRedNameBar	= NULL;

	m_pScoreColon         = NULL;
	m_pBlueScoreNum			= NULL;
	m_pRedScoreNum			= NULL;

	m_pCheerBack		= NULL;
	m_pChampCheerPoint	= NULL;
	m_pChampCheerNumber	= NULL;
	m_pChampCheerText	= NULL;
	m_pChallCheerPoint	= NULL;
	m_pChallCheerNumber	= NULL;
	m_pChallCheerText	= NULL;
	m_pCheerTimeBack	= NULL;

	m_pCircleGaugeNor   = NULL;
	m_pCircleGaugeDan   = NULL;

	m_pAcceptBack = NULL;
	m_pAcceptPoint = NULL;

	m_pBattleWin	 = NULL;

	m_iBlindOffSetY     = 0;
	m_iPreBlindOffSetY  = 0;

	m_iDefaultX = 0;
	m_iDefaultY = 0;
	
	m_bTimeAlert		= false;
	m_dwTimeAlertTime   = 0;

	m_fLeftCurrentHPRate	= 0.0f;
	m_fLeftUserHPRate		= 0.0f;
	m_iLeftUserGrade		= 0;
	m_bLeftGaugeStart		= true;
	m_dwLfetGaugeTime		= 0;
	m_pLeftCharIcon         = NULL;

	m_fRightCurrentHPRate	= 0.0f;
	m_fRightUserHPRate		= 0.0f;
	m_iRightUserGrade		= 0;
	m_bRightGaugeStart      = true;
	m_dwRightGaugeTime		= 0;
	m_pRightCharIcon        = NULL;

	m_fAlphaValue			= 0.0f;

	m_iChampionWinCount			= 0;
	m_fChampionWinEffectRate	= 0.0f;
	m_dwChampionWinEffectTime	= 0;

	m_iMaxPowerValue	= 0;
	m_dwLeftPowerGaugeTime = 0;
	m_fLeftPowerRate	= 0.0f;
	m_fLeftPowerCurrentRate	= 0.0f;
	m_dwRightPowerGaugeTime = 0;
	m_fRightPowerRate	= 0.0f;
	m_fRightPowerCurrentRate= 0.0f;
	m_dwPowerGaugeTime	= 0;
	m_fPowerGaugeRate	= 0.0f;
	m_bPowerGaugeRateReverse = false;

	m_dwWarningSecond	= 10;

	m_iCheerRemainTime = 100;
	m_iCheerFullTime   = 100;
	m_iCheerLimitTime  = 30000;
	m_dwCheerSendTime  = 0;

	m_iOwnerReserveCharType = -1;
	m_bMouseLBtnDown = false;

	m_bCheerPossible = true;
	m_dwCheerTabID   = 0;

	InitShowAniState();
	InitWarningAniState();

	m_iCurFightNPCStage = 0;

	// 제한 시간
	m_iFullSecond   = 0;
	m_iRemainSecond = 0;

	m_bSuddenDeadth = false;

	m_iSuddnDeathFullTime = 0;
	m_iSuddnDeathRemainTime = 0;
}

BattleModeTimeGaugeWnd::~BattleModeTimeGaugeWnd()
{
	SAFEDELETE( m_pCircleGauge );
	SAFEDELETE( m_pCircleSuddenDeathGauge );
	SAFEDELETE( m_pTimeIcon );
	SAFEDELETE( m_pTimeNumber ); 

	SAFEDELETE( m_pBack );         
	SAFEDELETE( m_pMold );         
	SAFEDELETE( m_pWarningBack );

	SAFEDELETE( m_pTimeText );
	SAFEDELETE( m_pTimeTextRed );
	SAFEDELETE( m_pTimeNumText );
	SAFEDELETE( m_pTimeNumTextRed );
	SAFEDELETE( m_pTimeBack );
	SAFEDELETE( m_pTimeBackRed );

	SAFEDELETE( m_pPowerBackEffectL );
	SAFEDELETE( m_pPowerBackGaugeL );
	SAFEDELETE( m_pPowerGaugeL );
	SAFEDELETE( m_pPowerGaugeEffectL );
	SAFEDELETE( m_pPowerGaugeMaxL );
	SAFEDELETE( m_pPowerBackEffectR );
	SAFEDELETE( m_pPowerBackGaugeR );
	SAFEDELETE( m_pPowerGaugeR );
	SAFEDELETE( m_pPowerGaugeEffectR );
	SAFEDELETE( m_pPowerGaugeMaxR );

	SAFEDELETE( m_pBlueTeamText );
	SAFEDELETE( m_pLeftBackLine );
	SAFEDELETE( m_pLeftBackLineLight );
	SAFEDELETE( m_pLeftGaugeBack );
	SAFEDELETE( m_pLeftGauge100 );
	SAFEDELETE( m_pLeftGaugeGlow );
	SAFEDELETE( m_pLeftGaugeRed );
	SAFEDELETE( m_pLeftBlueNameBar );
	SAFEDELETE( m_pLeftRedNameBar );

	SAFEDELETE( m_pRedTeamText );
	SAFEDELETE( m_pRightBackLine );
	SAFEDELETE( m_pRightBackLineLight );
	SAFEDELETE( m_pRightGaugeBack );
	SAFEDELETE( m_pRightGauge100 );
	SAFEDELETE( m_pRightGaugeGlow );
	SAFEDELETE( m_pRightGaugeRed );
	SAFEDELETE( m_pRightBlueNameBar );
	SAFEDELETE( m_pRightRedNameBar );

	SAFEDELETE( m_pLeftCharIcon );
	SAFEDELETE( m_pRightCharIcon );

	SAFEDELETE( m_pScoreColon );
	SAFEDELETE( m_pBlueScoreNum );
	SAFEDELETE( m_pRedScoreNum );

	SAFEDELETE( m_pCheerBack );
	SAFEDELETE( m_pChampCheerPoint );
	SAFEDELETE( m_pChampCheerNumber );
	SAFEDELETE( m_pChampCheerText );
	SAFEDELETE( m_pChallCheerPoint );
	SAFEDELETE( m_pChallCheerNumber );
	SAFEDELETE( m_pChallCheerText );
	SAFEDELETE( m_pCheerTimeBack );

	SAFEDELETE( m_pCircleGaugeNor );
	SAFEDELETE( m_pCircleGaugeDan );

	SAFEDELETE( m_pAcceptBack );
	SAFEDELETE( m_pAcceptPoint );

	SAFEDELETE( m_pBattleWin );

	ComGradeMap::iterator iCreator = m_ComGradeList.begin();
	for(;iCreator != m_ComGradeList.end();iCreator++)
	{
		SAFEDELETE( iCreator->second );
	}
	m_ComGradeList.clear();

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;
}

void BattleModeTimeGaugeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TimeIcon" )
	{
		SAFEDELETE( m_pTimeIcon );
		m_pTimeIcon = pImage;
	}
	else if( szType == "TimeNumber" )
	{
		SAFEDELETE( m_pTimeNumber );
		m_pTimeNumber = pImage;
	}	
	else if( szType == "Back" )
	{
		SAFEDELETE( m_pBack );
		m_pBack = pImage;
	}
	else if( szType == "Mold" )
	{
		SAFEDELETE( m_pMold );
		m_pMold = pImage;
	}
	else if( szType == "WarningBack" )
	{
		SAFEDELETE( m_pWarningBack );
		m_pWarningBack = pImage;
	}
	else if( szType == "TimeText" )
	{
		SAFEDELETE( m_pTimeText );
		m_pTimeText = pImage;
	}
	else if( szType == "TimeTextRed" )
	{
		SAFEDELETE( m_pTimeTextRed );
		m_pTimeTextRed = pImage;
	}
	else if( szType == "TimeNumText" )
	{
		SAFEDELETE( m_pTimeNumText );
		m_pTimeNumText = pImage;
	}
	else if( szType == "TimeNumTextRed" )
	{
		SAFEDELETE( m_pTimeNumTextRed );
		m_pTimeNumTextRed = pImage;
	}
	else if( szType == "TimeBack" )
	{
		SAFEDELETE( m_pTimeBack );
		m_pTimeBack = pImage;
	}
	else if( szType == "TimeBackRed" )
	{
		SAFEDELETE( m_pTimeBackRed );
		m_pTimeBackRed = pImage;
	}
	else if( szType == "TeamBlue" )
	{
		SAFEDELETE( m_pBlueTeamText );
		m_pBlueTeamText = pImage;
	}
	else if( szType == "LeftBackLine" )
	{
		SAFEDELETE( m_pLeftBackLine );
		m_pLeftBackLine = pImage;
	}
	else if( szType == "LeftBackLineLight" )
	{
		SAFEDELETE( m_pLeftBackLineLight );
		m_pLeftBackLineLight = pImage;
	}
	else if( szType == "LeftHPGaugeBack" )
	{
		SAFEDELETE( m_pLeftGaugeBack );
		m_pLeftGaugeBack = pImage;
	}
	else if( szType == "LeftGauge100" )
	{
		SAFEDELETE( m_pLeftGauge100 );
		m_pLeftGauge100 = pImage;
	}
	else if( szType == "LeftGaugeGlow" )
	{
		SAFEDELETE( m_pLeftGaugeGlow );
		m_pLeftGaugeGlow = pImage;
	}
	else if( szType == "LeftGaugeRed" )
	{
		SAFEDELETE( m_pLeftGaugeRed );
		m_pLeftGaugeRed = pImage;
	}
	else if( szType == "LeftBlueNameBar" )
	{
		SAFEDELETE( m_pLeftBlueNameBar );
		m_pLeftBlueNameBar = pImage;
	}
	else if( szType == "LeftRedNameBar" )
	{
		SAFEDELETE( m_pLeftRedNameBar );
		m_pLeftRedNameBar = pImage;
	}
	else if( szType == "TeamRed" )
	{
		SAFEDELETE( m_pRedTeamText );
		m_pRedTeamText = pImage;
	}
	else if( szType == "RightBackLine" )
	{
		SAFEDELETE( m_pRightBackLine );
		m_pRightBackLine = pImage;
	}
	else if( szType == "RightBackLineLight" )
	{
		SAFEDELETE( m_pRightBackLineLight );
		m_pRightBackLineLight = pImage;
	}
	else if( szType == "RightHPGaugeBack" )
	{
		SAFEDELETE( m_pRightGaugeBack );
		m_pRightGaugeBack = pImage;
	}
	else if( szType == "RightGauge100" )
	{
		SAFEDELETE( m_pRightGauge100 );
		m_pRightGauge100 = pImage;
	}
	else if( szType == "RightGaugeGlow" )
	{
		SAFEDELETE( m_pRightGaugeGlow );
		m_pRightGaugeGlow = pImage;
	}
	else if( szType == "RightGaugeRed" )
	{
		SAFEDELETE( m_pRightGaugeRed );
		m_pRightGaugeRed = pImage;
	}
	else if( szType == "RightBlueNameBar" )
	{
		SAFEDELETE( m_pRightBlueNameBar );
		m_pRightBlueNameBar = pImage;
	}
	else if( szType == "RightRedNameBar" )
	{
		SAFEDELETE( m_pRightRedNameBar );
		m_pRightRedNameBar = pImage;
	}
	else if( szType == "PowerBackEffectL" )
	{
		SAFEDELETE( m_pPowerBackEffectL );
		m_pPowerBackEffectL = pImage;
	}
	else if( szType == "PowerBackGaugeL" )
	{
		SAFEDELETE( m_pPowerBackGaugeL );
		m_pPowerBackGaugeL = pImage;
	}
	else if( szType == "PowerGaugeL" )
	{
		SAFEDELETE( m_pPowerGaugeL );
		m_pPowerGaugeL = pImage;
	}
	else if( szType == "PowerGaugeEffectL" )
	{
		SAFEDELETE( m_pPowerGaugeEffectL );
		m_pPowerGaugeEffectL = pImage;
	}
	else if( szType == "PowerGaugeMaxL" )
	{
		SAFEDELETE( m_pPowerGaugeMaxL );
		m_pPowerGaugeMaxL = pImage;
	}
	else if( szType == "PowerBackEffectR" )
	{
		SAFEDELETE( m_pPowerBackEffectR );
		m_pPowerBackEffectR = pImage;
	}
	else if( szType == "PowerBackGaugeR" )
	{
		SAFEDELETE( m_pPowerBackGaugeR );
		m_pPowerBackGaugeR = pImage;
	}
	else if( szType == "PowerGaugeR" )
	{
		SAFEDELETE( m_pPowerGaugeR );
		m_pPowerGaugeR = pImage;
	}
	else if( szType == "PowerGaugeEffectR" )
	{
		SAFEDELETE( m_pPowerGaugeEffectR );
		m_pPowerGaugeEffectR = pImage;
	}
	else if( szType == "PowerGaugeMaxR" )
	{
		SAFEDELETE( m_pPowerGaugeMaxR );
		m_pPowerGaugeMaxR = pImage;
	}
	else if( szType == "CheerBack" )
	{
		SAFEDELETE( m_pCheerBack );
		m_pCheerBack = pImage;
	}
	else if( szType == "ChampCheerPoint" )
	{
		SAFEDELETE( m_pChampCheerPoint );
		m_pChampCheerPoint = pImage;
	}
	else if( szType == "ChampCheerNumber" )
	{
		SAFEDELETE( m_pChampCheerNumber );
		m_pChampCheerNumber = pImage;
	}
	else if( szType == "ChampCheerText" )
	{
		SAFEDELETE( m_pChampCheerText );
		m_pChampCheerText = pImage;
	}
	else if( szType == "ChallCheerPoint" )
	{
		SAFEDELETE( m_pChallCheerPoint );
		m_pChallCheerPoint = pImage;
	}
	else if( szType == "ChallCheerNumber" )
	{
		SAFEDELETE( m_pChallCheerNumber );
		m_pChallCheerNumber = pImage;
	}
	else if( szType == "ChallCheerText" )
	{
		SAFEDELETE( m_pChallCheerText );
		m_pChallCheerText = pImage;
	}
	else if( szType == "CheerTimeBack" )
	{
		SAFEDELETE( m_pCheerTimeBack );
		m_pCheerTimeBack = pImage;
	}
	else if( szType == "AcceptBack" )
	{
		SAFEDELETE( m_pAcceptBack );
		m_pAcceptBack = pImage;
	}
	else if( szType == "AcceptPoint" )
	{
		SAFEDELETE( m_pAcceptPoint );
		m_pAcceptPoint = pImage;
	}
	else if( szType == "FightNpcClearWin" )
	{
		SAFEDELETE( m_pBattleWin );
		m_pBattleWin = pImage;
	}
	else if ( szType == "ScoreColon" )
	{
		SAFEDELETE( m_pScoreColon );
		m_pScoreColon = pImage;
	}
	else if ( szType == "BlueScoreNum" )
	{
		SAFEDELETE( m_pBlueScoreNum );
		m_pBlueScoreNum = pImage;
	}
	else if ( szType == "RedScoreNum" )
	{
		SAFEDELETE( m_pRedScoreNum );
		m_pRedScoreNum = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void BattleModeTimeGaugeWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void BattleModeTimeGaugeWnd::ClearCharSeqInfo()
{
	m_iChampionWinCount = 0;
	m_szLeftUserName.Clear();
	m_szRightUserName.Clear();
	m_szRightUserViewName.Clear();
	m_CharSeqData.clear();
}


void BattleModeTimeGaugeWnd::RemoveCharSeqInfo( const ioHashString &rkName )
{
	CharSeqDataVec::iterator iter = m_CharSeqData.begin();
	for(;iter != m_CharSeqData.end();iter++)
	{
		CharSeqData &rkData = *iter;
		if( rkData.m_kName == rkName )
		{
			m_CharSeqData.erase( iter );
			break;
		}
	}
}

void BattleModeTimeGaugeWnd::SetChampionUser( ioBaseChar *pChar )
{
	if( pChar == NULL ) return;

	
}

void BattleModeTimeGaugeWnd::SetBattleModeUser( const ioHashString &rkName, TeamType eTeam )
{
	ioBaseChar *pChar = m_pPlayStage->GetBaseChar( rkName );
	if( pChar == NULL ) return;

	ioHashString szCharName = pChar->GetCharName();

	if(eTeam == TEAM_BLUE)
	{
		// 같은 이름이면 캐릭터 아이콘만 갱신
		if( m_szLeftUserName == pChar->GetCharName() )
		{
			SetCharChange( pChar );
			return;
		}

		m_szLeftUserName		= pChar->GetCharName();
		m_dwLfetGaugeTime		= FRAMEGETTIME();
		m_bLeftGaugeStart		= true;
		m_fLeftCurrentHPRate	= 0.0f;

		if( pChar->IsOwnerChar() )
			m_iOwnerReserveCharType = -1;           // 예약 용병 초기화

		if( !m_pLeftCharIcon )
		{
			m_pLeftCharIcon = new BattleMode3DChar;
			m_pLeftCharIcon->Initialize();
		}

		if( m_pLeftCharIcon )
		{
			m_pLeftCharIcon->SetChar( pChar, true );
		}
	}
	else if(eTeam == TEAM_RED)
	{
		if( m_szRightUserName == pChar->GetCharName() )
		{
			SetCharChange( pChar );
			return;
		}

		m_szRightUserName		= pChar->GetCharName();
		m_szRightUserViewName   = szCharName;
		m_dwRightGaugeTime		= FRAMEGETTIME();
		m_bRightGaugeStart		= true;
		m_fRightCurrentHPRate	= 0.0f;

		if( pChar->IsOwnerChar() )
			m_iOwnerReserveCharType = -1;           // 예약 용병 초기화

		if( !m_pRightCharIcon )
		{
			m_pRightCharIcon = new BattleMode3DChar;
			m_pRightCharIcon->Initialize();
		}

		if( m_pRightCharIcon )
		{
			m_pRightCharIcon->SetChar( pChar, false );
		}
	}
}

void BattleModeTimeGaugeWnd::SetFaceChange( const ioHashString &rkName, const char *szTexPostfix, int iDuration )
{
	if( m_szLeftUserName == rkName)
	{
		if( m_pLeftCharIcon )
			m_pLeftCharIcon->FaceChange( szTexPostfix, iDuration );
	}

	if( m_szRightUserName == rkName)
	{
		if( m_pRightCharIcon )
			m_pRightCharIcon->FaceChange( szTexPostfix, iDuration );
	}
}

void BattleModeTimeGaugeWnd::SetCharChange( ioBaseChar *pChar )
{
	if( pChar == NULL ) return;

	if( pChar->GetCharName() == m_szLeftUserName )
	{
		if( m_pLeftCharIcon )
			m_pLeftCharIcon->SetChar( pChar, true );
	}
	else if( pChar->GetCharName() == m_szRightUserName )
	{
		if( m_pRightCharIcon )
			m_pRightCharIcon->SetChar( pChar, false );
	}
}

void BattleModeTimeGaugeWnd::SetHelmChange( ioBaseChar *pChar )
{
	if( pChar == NULL ) return;

	if( pChar->GetCharName() == m_szLeftUserName )
	{
		if( m_pLeftCharIcon )
			m_pLeftCharIcon->SetHelmChange( pChar );
	}
	else if( pChar->GetCharName() == m_szRightUserName )
	{
		if( m_pRightCharIcon )
			m_pRightCharIcon->SetHelmChange( pChar );
	}
}


int BattleModeTimeGaugeWnd::GetMeSeqCount()
{
	if( m_szLeftUserName == g_MyInfo.GetPublicID() )
		return 0;
	if( m_szRightUserName == g_MyInfo.GetPublicID() )
		return 0;

	int iReturn = 0;
	bool bMeSeqOK = false;       // 관전자는 포함이 안된다.
	CharSeqDataVec::iterator iter = m_CharSeqData.begin();
	for(;iter != m_CharSeqData.end();iter++)
	{
		CharSeqData &rkData = *iter;
		iReturn++;

		if( rkData.m_kName == g_MyInfo.GetPublicID() )
		{
			bMeSeqOK = true;
			break;
		}
	}

	if( !bMeSeqOK )
		return 0;

	return iReturn;
}

int BattleModeTimeGaugeWnd::GetMeCheerType()
{
	CharSeqDataVec::iterator iter = m_CharSeqData.begin();
	for(;iter != m_CharSeqData.end();iter++)
	{
		CharSeqData &rkData = *iter;
		if( rkData.m_kName == g_MyInfo.GetPublicID() )
		{
			return rkData.m_iCheer;
		}
	}
	return 0;
}

ioUIRenderImage *BattleModeTimeGaugeWnd::GetComGradeRes( int iGrade )
{
	ComGradeMap::iterator iter = m_ComGradeList.find( iGrade );
	if( iter != m_ComGradeList.end() )
		return iter->second;

	return NULL;
}

void BattleModeTimeGaugeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	UpdateAniState();

	ioMovingWnd::OnProcess( fTimePerSec );

	OnTimeProcess();
	OnLeftHealthProcess( fTimePerSec );
	OnRightHealthProcess( fTimePerSec );
	OnLeftPowerProcess( fTimePerSec );
	OnRightPowerProcess( fTimePerSec );
	OnPowerGaugeProcess( fTimePerSec );
	
	OnMouseProcess();

	UpdateGauge();
}

void BattleModeTimeGaugeWnd::OnTimeProcess()
{
	DWORD dwCheckTime = 250;
	DWORD dwGapTime   = FRAMEGETTIME() - m_dwTimeAlertTime;
	if( dwGapTime > dwCheckTime )
	{
		m_dwTimeAlertTime = FRAMEGETTIME();
		m_bTimeAlert	  = !m_bTimeAlert;
		return;
	}

	if( m_bTimeAlert )
	{
		m_fAlphaValue = sinf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / dwCheckTime );
		if( m_fAlphaValue > FLOAT1 )
			m_fAlphaValue = FLOAT1;
	}
	else
	{
		m_fAlphaValue = FLOAT1 - sinf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / dwCheckTime );
		if( m_fAlphaValue < 0.0f )
			m_fAlphaValue = 0.0f;
	}
	
	dwGapTime = FRAMEGETTIME() - m_dwChampionWinEffectTime;
	if( dwGapTime > 1000 )
	{
		m_dwChampionWinEffectTime = FRAMEGETTIME();
		m_fChampionWinEffectRate  = FLOAT1;
		return;
	}

	m_fChampionWinEffectRate = sinf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / 1000 );
	if( m_fChampionWinEffectRate > FLOAT1 )
		m_fChampionWinEffectRate = FLOAT1;
}

void BattleModeTimeGaugeWnd::OnLeftHealthProcess( float fTimePerSec )
{
	if( m_pPlayStage == NULL ) return;

	ioBaseChar *pChar = m_pPlayStage->GetBaseChar( m_szLeftUserName );
	if( pChar == NULL ) return;
	
	if( m_pLeftCharIcon )
	{
		m_pLeftCharIcon->Update( fTimePerSec );
	}

	float fHPRate = pChar->GetHP().m_fCurValue / pChar->GetHP().m_fMaxValue;
	if( m_bLeftGaugeStart )
	{
		enum { START_TIME = 0/*5000*/, GAUGE_UP_TIME = 1500, };
		DWORD dwGapTime = FRAMEGETTIME() - m_dwLfetGaugeTime;
		
		if( dwGapTime < START_TIME + GAUGE_UP_TIME )
			m_fLeftUserHPRate = fHPRate * sinf( ( D3DX_PI / 2 ) * ( (float)( dwGapTime - START_TIME ) / GAUGE_UP_TIME ) );
		else
			m_fLeftUserHPRate = fHPRate;

		if( m_fLeftUserHPRate >= fHPRate )
		{
			m_bLeftGaugeStart	 = false;
			m_iLeftUserGrade     = pChar->GetLevel();
			m_fLeftCurrentHPRate = m_fLeftUserHPRate;
			m_dwLfetGaugeTime	 = FRAMEGETTIME(); 
		}
	}
	else
	{
		if( m_fLeftUserHPRate != fHPRate )
		{
			m_fLeftUserHPRate = fHPRate;
			m_dwLfetGaugeTime = FRAMEGETTIME();
		}

		if( m_fLeftCurrentHPRate > m_fLeftUserHPRate )
		{
			float fGapRate = m_fLeftCurrentHPRate - m_fLeftUserHPRate;
			float fTimeRate= FLOAT1 - sinf( ( D3DX_PI / 2 ) * (float)( FRAMEGETTIME() - m_dwLfetGaugeTime ) / 20000 );
			m_fLeftCurrentHPRate = m_fLeftUserHPRate + ( fGapRate * fTimeRate );
			if( m_fLeftCurrentHPRate < m_fLeftUserHPRate )
				m_fLeftCurrentHPRate = m_fLeftUserHPRate;
		}
		else
		{
			m_fLeftCurrentHPRate = m_fLeftUserHPRate;
		}
	}
}

void BattleModeTimeGaugeWnd::OnRightHealthProcess( float fTimePerSec )
{
	if( m_pPlayStage == NULL ) return;

	ioBaseChar *pChar = m_pPlayStage->GetBaseChar( m_szRightUserName );
	if( pChar == NULL ) return;

	if( m_pRightCharIcon )
	{
		m_pRightCharIcon->Update( fTimePerSec );
	}

	float fHPRate = pChar->GetHP().m_fCurValue / pChar->GetHP().m_fMaxValue;
	if( m_bRightGaugeStart )
	{
		enum { START_TIME = 5000, GAUGE_UP_TIME = 1500, };
		DWORD dwGapTime = FRAMEGETTIME() - m_dwRightGaugeTime;
		if( dwGapTime < START_TIME )
			return;
		else if( dwGapTime < START_TIME + GAUGE_UP_TIME )
			m_fRightUserHPRate = fHPRate * sinf( ( D3DX_PI / 2 ) * ( (float)( dwGapTime - START_TIME ) / GAUGE_UP_TIME ) );
		else
			m_fRightUserHPRate = fHPRate;

		if( m_fRightUserHPRate >= fHPRate )
		{
			m_bRightGaugeStart    = false;
			m_iRightUserGrade     = pChar->GetLevel();
			m_fRightCurrentHPRate = m_fRightUserHPRate;
			m_dwRightGaugeTime	  = FRAMEGETTIME(); 
		}
	}
	else
	{
		if( m_fRightUserHPRate != fHPRate )
		{
			m_fRightUserHPRate = fHPRate;
			m_dwRightGaugeTime = FRAMEGETTIME();
		}

		if( m_fRightCurrentHPRate > m_fRightUserHPRate )
		{
			float fGapRate = m_fRightCurrentHPRate - m_fRightUserHPRate;
			float fTimeRate= FLOAT1 - sinf( ( D3DX_PI / 2 ) * (float)( FRAMEGETTIME() - m_dwRightGaugeTime ) / 20000 );
			m_fRightCurrentHPRate = m_fRightUserHPRate + ( fGapRate * fTimeRate );
			if( m_fRightCurrentHPRate < m_fRightUserHPRate )
				m_fRightCurrentHPRate = m_fRightUserHPRate;
		}
		else
		{
			m_fRightCurrentHPRate = m_fRightUserHPRate;
		}
	}
}

void BattleModeTimeGaugeWnd::OnLeftPowerProcess( float fTimePerSec )
{
	if( m_bLeftGaugeStart || m_iMaxPowerValue == 0 )
	{
		m_fLeftPowerRate = m_fLeftPowerCurrentRate = 0.0f;
	}
	else
	{
		int   i = 0;
		int   iTimePoint = 0;
		float fTimeSecond = (float)m_iRemainSecond;
		for(i = 0;i < (int)m_TimePowerData.size();i++)
		{
			if( fTimeSecond <= m_TimePowerData[i].m_fLimitValue )
				iTimePoint = m_TimePowerData[i].m_iPointValue;
		}

		int   iHealthPoint = 0;
		float fHealthRate  = ( m_fRightUserHPRate - m_fLeftUserHPRate ) * 100;
		for(i = 0;i < (int)m_HealthPowerData.size();i++)
		{
			if( fHealthRate >= m_HealthPowerData[i].m_fLimitValue )
				iHealthPoint = m_HealthPowerData[i].m_iPointValue;
		}

		int iTotalPowerPoint = iTimePoint + iHealthPoint;
		float fPowerRate = (float)iTotalPowerPoint / m_iMaxPowerValue;

		if( m_fLeftPowerRate != fPowerRate )
		{
			m_fLeftPowerRate = fPowerRate;
			m_dwLeftPowerGaugeTime = FRAMEGETTIME();
		}

		if( m_fLeftPowerCurrentRate > m_fLeftPowerRate )
		{
			float fGapRate = m_fLeftPowerCurrentRate - m_fLeftPowerRate;
			float fTimeRate= FLOAT1 - (float)( FRAMEGETTIME() - m_dwLeftPowerGaugeTime ) / 20000;
			m_fLeftPowerCurrentRate = m_fLeftPowerRate + ( fGapRate * fTimeRate );
			if( m_fLeftPowerCurrentRate < m_fLeftPowerRate )
				m_fLeftPowerCurrentRate = m_fLeftPowerRate;
		}
		else if( m_fLeftPowerCurrentRate < m_fLeftPowerRate )
		{
			float fGapRate = m_fLeftPowerRate - m_fLeftPowerCurrentRate;
			float fTimeRate= FLOAT1 - (float)( FRAMEGETTIME() - m_dwLeftPowerGaugeTime ) / 20000;
			m_fLeftPowerCurrentRate = m_fLeftPowerRate - ( fGapRate * fTimeRate );
			if( m_fLeftPowerCurrentRate > m_fLeftPowerRate )
				m_fLeftPowerCurrentRate = m_fLeftPowerRate;
		}

		m_fLeftPowerCurrentRate = max( 0.0f, min( FLOAT1, m_fLeftPowerCurrentRate ) );
	}
}

void BattleModeTimeGaugeWnd::OnRightPowerProcess( float fTimePerSec )
{
	if( m_bRightGaugeStart || m_iMaxPowerValue == 0 )
		m_fRightPowerRate = m_fRightPowerCurrentRate = 0.0f;
	else
	{
		int   i = 0;
		int   iTimePoint = 0;
		float fTimeSecond = (float)m_iRemainSecond;
		for(i = 0;i < (int)m_TimePowerData.size();i++)
		{
			if( fTimeSecond <= m_TimePowerData[i].m_fLimitValue )
				iTimePoint = m_TimePowerData[i].m_iPointValue;
		}

		int   iHealthPoint = 0;
		float fHealthRate  = ( m_fLeftUserHPRate - m_fRightUserHPRate ) * 100;
		for(i = 0;i < (int)m_HealthPowerData.size();i++)
		{
			if( fHealthRate >= m_HealthPowerData[i].m_fLimitValue )
				iHealthPoint = m_HealthPowerData[i].m_iPointValue;
		}

		int iTotalPowerPoint = iTimePoint + iHealthPoint;
		float fPowerRate = (float)iTotalPowerPoint / m_iMaxPowerValue;
		if( m_fRightPowerRate != fPowerRate )
		{
			m_fRightPowerRate = fPowerRate;
			m_dwRightPowerGaugeTime = FRAMEGETTIME();
		}

		if( m_fRightPowerCurrentRate > m_fRightPowerRate )
		{
			float fGapRate = m_fRightPowerCurrentRate - m_fRightPowerRate;
			float fTimeRate= FLOAT1 - (float)( FRAMEGETTIME() - m_dwRightPowerGaugeTime ) / 20000;
			m_fRightPowerCurrentRate = m_fRightPowerRate + ( fGapRate * fTimeRate );
			if( m_fRightPowerCurrentRate < m_fRightPowerRate )
				m_fRightPowerCurrentRate = m_fRightPowerRate;
		}
		else if( m_fRightPowerCurrentRate < m_fRightPowerRate )
		{
			float fGapRate = m_fRightPowerRate - m_fRightPowerCurrentRate;
			float fTimeRate= FLOAT1 - (float)( FRAMEGETTIME() - m_dwRightPowerGaugeTime ) / 20000;
			m_fRightPowerCurrentRate = m_fRightPowerRate - ( fGapRate * fTimeRate );
			if( m_fRightPowerCurrentRate > m_fRightPowerRate )
				m_fRightPowerCurrentRate = m_fRightPowerRate;
		}

		m_fRightPowerCurrentRate = max( 0.0f, min( FLOAT1, m_fRightPowerCurrentRate ) );
	}
}

void BattleModeTimeGaugeWnd::OnPowerGaugeProcess( float fTimePerSec )
{
	const DWORD dwCheckTime = 250;
	DWORD dwGapTime = FRAMEGETTIME() - m_dwPowerGaugeTime;
	if( dwGapTime > dwCheckTime )
	{
		m_dwPowerGaugeTime = FRAMEGETTIME();
		m_bPowerGaugeRateReverse = !m_bPowerGaugeRateReverse;
	}
	else
	{
		if( m_bPowerGaugeRateReverse )
			m_fPowerGaugeRate  = (float)dwGapTime / dwCheckTime;
		else
			m_fPowerGaugeRate  = FLOAT1 - (float)dwGapTime / dwCheckTime;

	}
}

bool BattleModeTimeGaugeWnd::IsMouseCheck( RECT &rkRect, ioHashString &rkName, const ioHashString &rkPrevName, const bool &rkPrevBtnDown )
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse == NULL ) return false;

	if( ::PtInRect( &rkRect, pMouse->GetMousePos() ) )
	{
		if( rkPrevName != rkName )
		{
			g_SoundMgr.PlaySound( "interface/public_mouseover_00.wav" );
		}

		m_MouseOverName = rkName;
		if( pMouse->IsLBtnDown() )
		{
			if( !rkPrevBtnDown )
				g_SoundMgr.PlaySound( "interface/public_mouseclick_00.wav" );
			m_bMouseLBtnDown = true;
		}
		else if( pMouse->IsLBtnUp() )
		{
			g_App.OpenMannerWnd( m_MouseOverName.c_str(), true );
		}
		else if( pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_MouseOverName, UserInfoToolTip::TT_GLOBAL );
		return true;
	}
	return false;
}

void BattleModeTimeGaugeWnd::OnMouseProcess()
{
	bool bPrevMouseDown = m_bMouseLBtnDown;
	ioHashString szPrevOverName = m_MouseOverName;
	
	m_bMouseLBtnDown = false;
	m_MouseOverName.Clear();
	
	if( !g_App.IsVisibleMouse() )
		return;

	if( g_GUIMgr.GetPreOverWnd() != NULL && g_GUIMgr.GetPreOverWnd() != this )
	{
		if( g_GUIMgr.GetPreOverWnd() != g_GUIMgr.GetDeskTop() )  
			return;
	}

	RECT rcRect;
	int iXPos = GetDerivedPosX() - 60;
	int iYPos = GetDerivedPosY();

	// 챔피언 
	// 3D Char
	rcRect.left   = iXPos + 19;
	rcRect.top    = iYPos - 33;
	rcRect.right  = rcRect.left + 109;
	rcRect.bottom = rcRect.top + 93; 
	if( IsMouseCheck( rcRect, m_szLeftUserName, szPrevOverName, bPrevMouseDown ) ) return;

	// Name Bar
	rcRect.left   = iXPos + 28;
	rcRect.top    = iYPos + 60;
	rcRect.right  = rcRect.left + 177;
	rcRect.bottom = rcRect.top + 21; 
	if( IsMouseCheck( rcRect, m_szLeftUserName, szPrevOverName, bPrevMouseDown ) ) return;
	
	// 도전자
	// 3D Char
	rcRect.left   = iXPos + 752;
	rcRect.top    = iYPos - 33;
	rcRect.right  = rcRect.left + 109;
	rcRect.bottom = rcRect.top + 93; 
	if( IsMouseCheck( rcRect, m_szRightUserName, szPrevOverName, bPrevMouseDown ) ) return;

	// Name Bar
	rcRect.left   = iXPos + 675;
	rcRect.top    = iYPos + 60;
	rcRect.right  = rcRect.left + 177;
	rcRect.bottom = rcRect.top + 21;
	if( IsMouseCheck( rcRect, m_szRightUserName, szPrevOverName, bPrevMouseDown ) ) return;

	// 대기자
	int iCount = 0;
	CharSeqDataVec::iterator iter = m_CharSeqData.begin();
	for(;iter != m_CharSeqData.end();iter++,iCount++)
	{
		CharSeqData &rkData = *iter;
		
		int iWidthSize = min( 95.0f, g_FontMgr.GetTextWidth( rkData.m_kName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_11 ) ) + 20;

		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( rkData.m_kName, dwGuildIndex, dwGuildMark );
		if( dwGuildIndex != 0 )
			iWidthSize += 18;

		RECT rcNameRect;
		rcNameRect.left   = iXPos + 834 - iWidthSize;
		rcNameRect.top    = iYPos + 89 + ( iCount * 16 );
		rcNameRect.right  = rcNameRect.left + iWidthSize;
		rcNameRect.bottom = rcNameRect.top + 16;
		if( IsMouseCheck( rcNameRect, rkData.m_kName, szPrevOverName, bPrevMouseDown ) ) return;
	}
}

void BattleModeTimeGaugeWnd::UpdateTime( int iRemainTime, int iMaxTime )
{
	// 남은 플레이 시간
	int iRemainSecond	= (float)iRemainTime / 1000;
	if( iRemainSecond > 0 && m_iRemainSecond != iRemainSecond )
	{
		if( iRemainSecond < (int)m_dwWarningSecond )
		{
			if( !m_szWarningTickSnd.IsEmpty() )
			{
				g_SoundMgr.PlaySound( m_szWarningTickSnd );
			}
		}
	}		
	m_iRemainSecond	= iRemainSecond;
	m_iRemainSecond  = iRemainTime/1000;
	m_iFullSecond = iMaxTime/1000;
}

void BattleModeTimeGaugeWnd::UpdateSuddenDeathTime( DWORD dwRemainTime, DWORD dwFullTime )
{
	m_iSuddnDeathFullTime = (int)(dwFullTime)/1000;
	m_iSuddnDeathRemainTime = (int)(dwRemainTime)/1000;
}

void BattleModeTimeGaugeWnd::UpdateGauge()
{
	enum 
	{ 
		BACK_XOFFSET = 44,
		BACK_YOFFSET = 50,
	};	

	if( m_pCircleGauge )
	{
		float fGaugeRate = 0.0f;
		if( m_iFullSecond > 0 )
		{
			fGaugeRate = (float)m_iRemainSecond / (float)m_iFullSecond;
		}
		int iPosX = GetDerivedPosX() + TIME_GAUGE_POS_X;
		int iPosY = GetDerivedPosY();
		m_pCircleGauge->UpdateGauge( iPosX-BACK_XOFFSET,
			iPosY-BACK_YOFFSET,
			fGaugeRate );
	}

	if( 0 < m_iRemainSecond )
		m_iSuddnDeathRemainTime = 0;

	if( m_pCircleSuddenDeathGauge )
	{
		float fGaugeRate = 0.0f;
		if( m_iSuddnDeathRemainTime > 0 )
		{
			fGaugeRate = (float)m_iSuddnDeathRemainTime / (float)m_iSuddnDeathFullTime;
		}

		m_pCircleSuddenDeathGauge->UpdateGauge( GetDerivedPosX()-BACK_XOFFSET,
			GetDerivedPosY()-BACK_YOFFSET,
			fGaugeRate );
	}
}

void BattleModeTimeGaugeWnd::OnRender()
{
	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() - 60;           // 준희씨 좌표가 윈도우 사이즈 880을 기반으로 잡아서 윈도우 사이즈를 760으로 고정하는 대신 -60을 해줌.
	int iYPos = GetDerivedPosY();

	OnTimeRender( iXPos, iYPos );
	OnRenderBlueTeamParticipant( iXPos, iYPos );

	OnRenderRedTeamParticipant( iXPos, iYPos );
	OnSeqUserListRenter( iXPos, iYPos );
	
	OnRenderScore( iXPos, iYPos );

	// 배틀모드 시간 체크
	RenderTimeGauge( iXPos + 60 + TIME_GAUGE_POS_X, iYPos );
}

void BattleModeTimeGaugeWnd::RenderTimeGauge( int iXPos, int iYPos )
{
	if( !m_pBack || !m_pWarningBack || !m_pCircleGauge || !m_pMold || !m_pTimeIcon || !m_pTimeNumber || !m_pCircleSuddenDeathGauge )
		return;


	enum { BACK_XOFFSET = 11, BACK_YOFFSET = 6, };
	// 뒷배경 출력
	m_pBack->Render( iXPos+BACK_XOFFSET, iYPos+BACK_YOFFSET );

	if( m_WarningAniState.GetCurState() == WAT_HIDING)
	{
		float fRate = FLOAT1 - ( (float) m_WarningAniState.GetCurTime() / (float) m_WarningAniState.GetDelay() );
		m_pWarningBack->SetAlpha( 255.0f * fRate );
		m_pWarningBack->Render( iXPos+BACK_XOFFSET, iYPos+BACK_YOFFSET );
	}

	if( m_pCircleGauge->IsNeedRender() )
	{
		// 소멸은 g_UIRenderer가 해준다.
		ioCircleGaugeQuad *pQuad = new ioCircleGaugeQuad( m_pCircleGauge );
		if( pQuad )
		{
			g_UIRenderer.AddUIQuad( pQuad );
		}
	}

	if( m_pCircleSuddenDeathGauge->IsNeedRender() )
	{
		ioCircleGaugeQuad *pQuad = new ioCircleGaugeQuad( m_pCircleSuddenDeathGauge );
		if( pQuad )
		{
			g_UIRenderer.AddUIQuad( pQuad );
		}
	}

	int iSecondTime = 0;
	if( g_BattleRoomMgr.IsTournamentModeType() )
	{		
		if( 0 < m_iRemainSecond )
			iSecondTime = m_iRemainSecond;
		else
			iSecondTime = m_iSuddnDeathRemainTime;
	}
	else
	{
		iSecondTime = m_iRemainSecond;		
	}

	if( iSecondTime <= 0 )
		iSecondTime = 0;

	// 윗부분 출력...
	m_pMold->Render( iXPos+BACK_XOFFSET, iYPos+BACK_YOFFSET, UI_RENDER_ADD );
	m_pTimeIcon->Render( iXPos+68, iYPos+13, UI_RENDER_ADD );

	enum { MIN_SECOND_POS = 10, MIN_THIRD_POS = 100 , FIRST_XOFFSET = 77, SECOND_XOFFST = 84, THIRD_XOFFST = 91, };
	m_pTimeNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	if( iSecondTime < MIN_SECOND_POS )
	{
		m_pTimeNumber->RenderNum( iXPos + FIRST_XOFFSET, iYPos + 14, 0, FLOAT1 , FLOAT1, UI_RENDER_ADD );
		m_pTimeNumber->RenderNum( iXPos + SECOND_XOFFST, iYPos + 14, 0, FLOAT1 , FLOAT1, UI_RENDER_ADD );
		m_pTimeNumber->RenderNum( iXPos + THIRD_XOFFST, iYPos + 14, iSecondTime, FLOAT1 , FLOAT1, UI_RENDER_ADD );
	}
	else if( iSecondTime < MIN_THIRD_POS )
	{
		m_pTimeNumber->RenderNum( iXPos + FIRST_XOFFSET, iYPos + 14, 0, FLOAT1 , FLOAT1, UI_RENDER_ADD );
		m_pTimeNumber->RenderNum( iXPos + SECOND_XOFFST, iYPos + 14, iSecondTime, FLOAT1 , FLOAT1, UI_RENDER_ADD );
	}
	else
		m_pTimeNumber->RenderNum( iXPos + FIRST_XOFFSET, iYPos + 14, iSecondTime, FLOAT1 , FLOAT1, UI_RENDER_ADD );
}

void BattleModeTimeGaugeWnd::OnTimeRender( int iXPos, int iYPos )
{
	bool bAlertStart = false;
	if( m_iRemainSecond < (int)m_dwWarningSecond )
		bAlertStart = true;
		
	ioUIRenderImage *pTimeBack	  = m_pTimeBack;
	ioUIRenderImage *pTimeNumText = m_pTimeNumText;
	ioUIRenderImage *pTimeText    = m_pTimeText;
	if( bAlertStart && m_bTimeAlert )
	{
		pTimeBack	= m_pTimeBackRed;
		pTimeNumText= m_pTimeNumTextRed;
		pTimeText	= m_pTimeTextRed;
	}

	if( pTimeBack )
	{
		pTimeBack->Render( iXPos + 367, iYPos + 2 );
	}

	if( pTimeNumText )
	{
		pTimeNumText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		pTimeNumText->RenderNum( iXPos + 440, iYPos + 20, m_iRemainSecond, -2.0f, 0.7f );
	}

	if( pTimeText )
	{
		pTimeText->Render( iXPos + 420, iYPos + 4 );
	}	
}

void BattleModeTimeGaugeWnd::OnRenderBlueTeamParticipant( int iXPos, int iYPos )
{
	if( m_pLeftGaugeBack )
		m_pLeftGaugeBack->Render( iXPos + 22, iYPos + 64/*36*/ );
	
	if( m_pLeftCharIcon )
	{
		m_pLeftCharIcon->Render( iXPos - 150, iYPos - 130 );
	}

	if( m_pBlueTeamText )
		m_pBlueTeamText->Render( iXPos + 211 + 60, iYPos + 45 );

	if( m_pLeftGaugeRed )
	{
		float fGaugeWidth = m_pLeftGaugeRed->GetWidth();
		float fGaugeValue = fGaugeWidth - ( fGaugeWidth * m_fLeftCurrentHPRate );  
		m_pLeftGaugeRed->RenderWidthCut( iXPos + 135, iYPos + 67, min( fGaugeWidth, max( 0, fGaugeValue ) ), fGaugeWidth );
	}

	if( m_pLeftGauge100 )
	{
		float fGaugeWidth = m_pLeftGauge100->GetWidth();
		float fGaugeValue = fGaugeWidth - ( fGaugeWidth * m_fLeftUserHPRate ); 
		m_pLeftGauge100->RenderWidthCut( iXPos + 135, iYPos + 67, min( fGaugeWidth, max( 0, fGaugeValue ) ), fGaugeWidth );
	}

	if( m_pLeftGaugeGlow )
	{
		float fGaugeWidth = m_pLeftGaugeGlow->GetWidth();
		float fGaugeValue = fGaugeWidth - ( fGaugeWidth * m_fLeftUserHPRate ); 
		m_pLeftGaugeGlow->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaValue );
		m_pLeftGaugeGlow->RenderWidthCut( iXPos + 135, iYPos + 67, min( fGaugeWidth, max( 0, fGaugeValue ) ), fGaugeWidth );
	}

	if( m_pLeftBlueNameBar )
		m_pLeftBlueNameBar->Render( iXPos + 8, iYPos + 89 );

	int iWeightX = 60;
	float fNameWidth = 130.0f;
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetTextColor( 0, 150, 255 );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.PrintTextWidthCut( iXPos + iWeightX + 155, iYPos + 44, FONT_SIZE_14, fNameWidth, STR(1), m_szLeftUserName.c_str() );

	g_LevelMgr.RenderGrade( iXPos + iWeightX + 158, iYPos + 44, g_MyInfo.GetGradeLevel(), TEAM_BLUE );

	if( m_pLeftBackLine )
		m_pLeftBackLine->Render( iXPos + 123, iYPos + 63 );

	if( m_pLeftBackLineLight )
	{
		m_pLeftBackLineLight->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaValue );
		m_pLeftBackLineLight->Render( iXPos + 123, iYPos + 63 );
	}
}

void BattleModeTimeGaugeWnd::OnRenderRedTeamParticipant( int iXPos, int iYPos )
{
	if( m_pRightGaugeBack )
		m_pRightGaugeBack->Render( iXPos + 476, iYPos + 64/*36*/ );

	if( m_pRightCharIcon )
	{
		m_pRightCharIcon->Render( iXPos + 600, iYPos - 130 );
	}

	if( m_pRedTeamText )
		m_pRedTeamText->Render( iXPos + 450 + 60, iYPos + 45 );

	if( m_pRightGaugeRed )
	{
		float fGaugeWidth = m_pRightGaugeRed->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fRightCurrentHPRate; 
		m_pRightGaugeRed->RenderWidthCut( iXPos + 489, iYPos + 67, 0, min( fGaugeWidth, fGaugeValue ) );
	}

	if( m_pRightGauge100 )
	{
		float fGaugeWidth = m_pRightGauge100->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fRightUserHPRate; 
		m_pRightGauge100->RenderWidthCut( iXPos + 489, iYPos + 67, 0, min( fGaugeWidth, fGaugeValue ) );
	}

	if( m_pRightGaugeGlow )
	{
		float fGaugeWidth = m_pRightGaugeGlow->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fRightUserHPRate; 
		m_pRightGaugeGlow->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaValue );
		m_pRightGaugeGlow->RenderWidthCut( iXPos + 489, iYPos + 67, 0, min( fGaugeWidth, fGaugeValue ) );
	}

	bool bMyChar = ( g_MyInfo.GetPublicID() == m_szRightUserName );
	
	if( m_pRightRedNameBar )
		m_pRightRedNameBar->Render( iXPos + 642, iYPos + 89 );

	//길드 마크
	int iStartX = 615;
	int iWeightX = 40;

	float fNameWidth = 130.0f;
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetTextColor( 255, 60, 0 );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.PrintTextWidthCut( iXPos + iStartX + 4 + iWeightX, iYPos + 44, FONT_SIZE_14, fNameWidth, STR(2), m_szRightUserViewName.c_str() );

	g_LevelMgr.RenderGrade( iXPos + iStartX + iWeightX - 17, iYPos + 44, m_iRightUserGrade, TEAM_RED );

	if( m_pRightBackLine )
		m_pRightBackLine->Render( iXPos + 477, iYPos + 63 );

	if( m_pRightBackLineLight )
	{
		m_pRightBackLineLight->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaValue );
		m_pRightBackLineLight->Render( iXPos + 477, iYPos + 63 );
	}
}

void BattleModeTimeGaugeWnd::OnRenderScore( int iXPos, int iYPos )
{
	if( m_pScoreColon )
		m_pScoreColon->Render( iXPos + 380 + 60, iYPos + 69 );

	ioPlayMode* pPlayMode = m_pPlayStage->GetPlayMode();
	ioBattleMode* pBattleMode = ToBattleMode( pPlayMode );

	if( m_pBlueScoreNum )
		m_pBlueScoreNum->RenderNum( iXPos + 348 + 60, iYPos + 56, pBattleMode->GetScore( TEAM_BLUE ) );

	if( m_pRedScoreNum )
		m_pRedScoreNum->RenderNum( iXPos + 393 + 60, iYPos + 56, pBattleMode->GetScore( TEAM_RED ) );
}

void BattleModeTimeGaugeWnd::OnSeqUserListRenter( int iXPos, int iYPos )
{
	if( m_CharSeqData.empty() ) return;

	int iCount = 0;
	CharSeqDataVec::iterator iter = m_CharSeqData.begin();
	for(;iter != m_CharSeqData.end();iter++,iCount++)
	{
		CharSeqData &rkData = *iter;
		if( rkData.m_kName == g_MyInfo.GetPublicID() )
		{
			//길드 마크
			int iStartX = 834;
			DWORD dwGuildIndex, dwGuildMark;
			g_UserInfoMgr.GetGuildInfo( rkData.m_kName, dwGuildIndex, dwGuildMark );
			if( dwGuildIndex != 0 )
			{
				g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + iStartX - 18, iYPos + 91 + ( iCount * 16 ) );
				iStartX -= 18;
			}

			float fNameWidth = 95.0f;
			g_FontMgr.SetAlignType( TAT_RIGHT );
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetTextColor( 0, 150, 255 );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.PrintTextWidthCut( iXPos + iStartX, iYPos + 90 + ( iCount * 16 ), FONT_SIZE_11, fNameWidth, STR(1), rkData.m_kName.c_str() );
			iStartX -= min( fNameWidth, g_FontMgr.GetTextWidth( rkData.m_kName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_11 ) ) + 17;

			g_LevelMgr.RenderGrade( iXPos + iStartX, iYPos + 88 + ( iCount * 16 ), rkData.m_iLevel, TEAM_BLUE );
			iStartX -= 74;
		}
		else
		{
			//길드 마크
			int iStartX = 834;
			DWORD dwGuildIndex, dwGuildMark;
			g_UserInfoMgr.GetGuildInfo( rkData.m_kName, dwGuildIndex, dwGuildMark );
			if( dwGuildIndex != 0 )
			{
				g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + iStartX - 18, iYPos + 91 + ( iCount * 16 ) );
				iStartX -= 18;
			}

			float fNameWidth = 95.0f;
			g_FontMgr.SetAlignType( TAT_RIGHT );
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetTextColor( 255, 60, 0 );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.PrintTextWidthCut( iXPos + iStartX, iYPos + 90 + ( iCount * 16 ), FONT_SIZE_11, fNameWidth, STR(2), rkData.m_kName.c_str() );
			iStartX -= min( fNameWidth, g_FontMgr.GetTextWidth( rkData.m_kName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_11 ) ) + 17;

			g_LevelMgr.RenderGrade( iXPos + iStartX, iYPos + 88 + ( iCount * 16 ), rkData.m_iLevel, TEAM_RED );
		}
	}
}

void BattleModeTimeGaugeWnd::OnPowerGaugeLeftRender( int iXPos, int iYPos )
{
	if( m_pPowerBackEffectL )
	{
		float fAlphaRate = FLOAT08 * m_fPowerGaugeRate;
		m_pPowerBackEffectL->SetAlpha( (float)MAX_ALPHA_RATE * ( 0.2f + fAlphaRate ) );
		m_pPowerBackEffectL->Render( iXPos + 199, iYPos + 51 );
	}

	if( m_pPowerBackGaugeL )
		m_pPowerBackGaugeL->Render( iXPos + 207, iYPos + 61 );

	if( m_pPowerGaugeL )
	{
		float fGaugeWidth = m_pPowerGaugeL->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fLeftPowerCurrentRate; 
		m_pPowerGaugeL->RenderWidthCut( iXPos + 207, iYPos + 61, fGaugeWidth - fGaugeValue, fGaugeWidth );
	}

	if( m_pPowerGaugeEffectL )
	{
		if( m_fLeftPowerCurrentRate < FLOAT1 )
			m_pPowerGaugeEffectL->SetAlpha( (float)MAX_ALPHA_RATE * m_fPowerGaugeRate );
		else 
			m_pPowerGaugeEffectL->SetAlpha( MAX_ALPHA_RATE );
		float fGaugeWidth = m_pPowerGaugeEffectL->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fLeftPowerCurrentRate; 
		m_pPowerGaugeEffectL->RenderWidthCut( iXPos + 207, iYPos + 61, fGaugeWidth - fGaugeValue, fGaugeWidth );
	}

	if( m_fLeftPowerCurrentRate >= 0.99f )
	{
		if( m_pPowerGaugeMaxL && m_bPowerGaugeRateReverse )
		{
			m_pPowerGaugeMaxL->Render( iXPos + 207, iYPos + 61 );
		}
	}
}

void BattleModeTimeGaugeWnd::OnPowerGaugeRightRender( int iXPos, int iYPos )
{
	if( m_pPowerBackEffectR )
	{
		float fAlphaRate = FLOAT08 * m_fPowerGaugeRate;
		m_pPowerBackEffectR->SetAlpha( (float)MAX_ALPHA_RATE * ( 0.2f + fAlphaRate ) );
		m_pPowerBackEffectR->Render( iXPos + 476, iYPos + 51 );
	}

	if( m_pPowerBackGaugeR )
		m_pPowerBackGaugeR->Render( iXPos + 483, iYPos + 61 );

	if( m_pPowerGaugeR )
	{
		float fGaugeWidth = m_pPowerGaugeR->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fRightPowerCurrentRate; 
		m_pPowerGaugeR->RenderWidthCut( iXPos + 483, iYPos + 61, 0, fGaugeValue );
	}

	if( m_pPowerGaugeEffectR )
	{
		if( m_fRightPowerCurrentRate < FLOAT1 )
			m_pPowerGaugeEffectR->SetAlpha( (float)MAX_ALPHA_RATE * m_fPowerGaugeRate );
		else
			m_pPowerGaugeEffectR->SetAlpha( MAX_ALPHA_RATE );
		float fGaugeWidth = m_pPowerGaugeEffectR->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fRightPowerCurrentRate; 
		m_pPowerGaugeEffectR->RenderWidthCut( iXPos + 483, iYPos + 61, 0, fGaugeValue );
	}

	if( m_fRightPowerCurrentRate >= 0.99f )
	{
		if( m_pPowerGaugeMaxR && m_bPowerGaugeRateReverse )
		{
			m_pPowerGaugeMaxR->Render( iXPos + 483, iYPos + 61 );
		}
	}
}

void BattleModeTimeGaugeWnd::iwm_create()
{
	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();

	if( m_bPosToDefualt )
	{
		m_bPosToDefualt = false;
		SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}

	m_pCircleGaugeNor = new io2DCircleGauge;
	if( m_pCircleGaugeNor)                
		m_pCircleGaugeNor->InitResource( "chapion_cheer_time_nor.dds", 360, 0, 0, io2DCircleGauge::ST_6OCLOCK );


	m_pCircleGaugeDan = new io2DCircleGauge;
	if( m_pCircleGaugeDan )
		m_pCircleGaugeDan->InitResource( "chapion_cheer_time_death.dds", 360, 0, 0, io2DCircleGauge::ST_6OCLOCK );


	// 시간 게이지
	m_pCircleGauge = new io2DCircleGauge;
	if( m_pCircleGauge)                
		m_pCircleGauge->InitResource( "score_gauge.dds", 300, 23, 23, io2DCircleGauge::ST_6OCLOCK );

	m_pCircleSuddenDeathGauge = new io2DCircleGauge;
	if( m_pCircleSuddenDeathGauge)                
		m_pCircleSuddenDeathGauge->InitResource( "score_gauge2.dds", 300, 23, 23, io2DCircleGauge::ST_6OCLOCK );

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();

	if( m_bPosToDefualt )
	{
		m_bPosToDefualt = false;
		SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}
}

void BattleModeTimeGaugeWnd::iwm_show()
{
	m_iAcceptX = 0;
	m_AcceptPointState = APS_RIGHT;
	m_dwAcceptPointCheckTime = FRAMEGETTIME();
}

void BattleModeTimeGaugeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	
}

void BattleModeTimeGaugeWnd::SetBlindOffSet( int iYOffSet )
{
	m_iBlindOffSetY = iYOffSet;
}

void BattleModeTimeGaugeWnd::InitAniState()
{
	m_ShowAniState.SetCurState( SAT_NONE );
}

void BattleModeTimeGaugeWnd::InitShowAniState()
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

void BattleModeTimeGaugeWnd::InitWarningAniState()
{
	enum { DELAY_HIDING = 250, DELAY_DONT_RENDER = 250, };
	m_WarningAniState.SetCurState( WAT_NONE );

	FSMState *pState = new FSMState(WAT_NONE);
	if( pState )
		pState->Add( WAT_HIDING,  WAT_HIDING );
	m_WarningAniState.Add( pState );

	pState = new FSMState(WAT_HIDING);
	if( pState )
	{
		pState->Add( WAT_HIDING,  WAT_DONT_RENDER , DELAY_HIDING);
		pState->Add( WAT_NONE, WAT_NONE );
	}
	m_WarningAniState.Add( pState );	

	pState = new FSMState(WAT_DONT_RENDER);
	if( pState )
	{
		pState->Add( WAT_DONT_RENDER,  WAT_HIDING , DELAY_DONT_RENDER);
		pState->Add( WAT_NONE, WAT_NONE );
	}
	m_WarningAniState.Add( pState );	
}

void BattleModeTimeGaugeWnd::UpdateAniState()
{
	if( UpdateShowAni() )
		return;

	UpdateDefaultPos();

	// 10초전 ani
	m_WarningAniState.UpdateTime();
	enum { WARNING_SECOND = 10,};
	if( COMPARE( m_iRemainSecond, 1, WARNING_SECOND+1) )
		m_WarningAniState.ChangeState( WAT_HIDING );

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

void BattleModeTimeGaugeWnd::ChangeShowAniState( ShowAniType eShowAniType )
{
	m_ShowAniState.ChangeState( (int) eShowAniType );
}

bool BattleModeTimeGaugeWnd::UpdateShowAni()
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
		int iCurYpos = m_iDefaultY - ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * iStartYPos );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_DOWN )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( sin( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_UP_STOP )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
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

int BattleModeTimeGaugeWnd::GetCurShowAniState()
{
	return m_ShowAniState.GetCurState();
}

void BattleModeTimeGaugeWnd::UpdateDefaultPos()
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

void BattleModeTimeGaugeWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
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
		SetWndPos( m_iDefaultX, m_iDefaultY + m_iBlindOffSetY );
	}
}

void BattleModeTimeGaugeWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwWarningSecond	= xElement.GetIntAttribute_e( "WarningSecond" );
	m_szWarningTickSnd	= xElement.GetStringAttribute_e( "WarningTickSnd" );

	m_iMaxPowerValue = xElement.GetIntAttribute_e( "MaxPower" );
	int i = 0;
	char szKey[MAX_PATH] = "";
	int iMaxTimePower= xElement.GetIntAttribute_e( "MaxTimePower" );
	for(i = 0;i < iMaxTimePower;i++)
	{
		PowerGaugeData kData;

		sprintf_s_e( szKey, "TPValue%d", i + 1 );
		kData.m_fLimitValue = xElement.GetFloatAttribute( szKey );

		sprintf_s_e( szKey, "TPPoint%d", i + 1 );
		kData.m_iPointValue = xElement.GetIntAttribute( szKey );
		m_TimePowerData.push_back( kData );
	}

	int iMaxHealthPower = xElement.GetIntAttribute_e( "MaxHPPower" );
	for(i = 0;i < iMaxHealthPower;i++)
	{
		PowerGaugeData kData;

		sprintf_s_e( szKey, "HPValue%d", i + 1 );
		kData.m_fLimitValue = xElement.GetFloatAttribute( szKey );

		sprintf_s_e( szKey, "HPPoint%d", i + 1 );
		kData.m_iPointValue = xElement.GetIntAttribute( szKey );
		m_HealthPowerData.push_back( kData );
	}

	m_iCheerLimitTime = xElement.GetIntAttribute_e( "CheerLimitTime" );

	m_dwAcceptPointTime = xElement.GetIntAttribute_e( "AcceptPointTime" );

	// AI 난이도
	int iMaxComGrade = xElement.GetIntAttribute_e( "MaxComGrade" );
	for(i = 0;i < iMaxComGrade;i++)
	{
		int iGradeKey = i + 1;
		sprintf_s_e( szKey, "Image%d", iGradeKey );		
		m_ComGradeList.insert( ComGradeMap::value_type( iGradeKey, g_UIImageSetMgr.CreateImageByFullName( xElement.GetStringAttribute( szKey ) ) ) );
	}
}

void BattleModeTimeGaugeWnd::OnProcessFightNPC()
{
	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwAcceptPointCheckTime;

	float fRate = 0.0f;
	if( dwGapTime < m_dwAcceptPointTime )
	{
		fRate = (float)dwGapTime / (float)m_dwAcceptPointTime;
		int iGapX = 98 * fRate;

		switch( m_AcceptPointState )
		{
		case APS_RIGHT:
			m_iAcceptX = iGapX;
			break;
		case APS_LEFT:
			m_iAcceptX = 98 - iGapX;
			break;
		}
	}
	else
	{
		switch( m_AcceptPointState )
		{
		case APS_RIGHT:
			{
				m_iAcceptX = 98;
				m_AcceptPointState = APS_LEFT;
			}
			break;
		case APS_LEFT:
			{
				m_iAcceptX = 0;
				m_AcceptPointState = APS_RIGHT;
			}
			break;
		}

		m_dwAcceptPointCheckTime = dwCurTime;
	}
}

void BattleModeTimeGaugeWnd::OnChallengerNPCRender( int iXPos, int iYPos )
{
	if( m_pRightGaugeBack )
		m_pRightGaugeBack->Render( iXPos + 476, iYPos + 36 );

	if( m_pRightCharIcon )
	{
		m_pRightCharIcon->Render( iXPos + 600, iYPos - 150 );
	}

	int iNewXPos = iXPos+60-20;


	if( m_pRightGaugeRed )
	{
		float fGaugeWidth = m_pRightGaugeRed->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fRightCurrentHPRate; 
		m_pRightGaugeRed->RenderWidthCut( iXPos + 489, iYPos + 39, 0, min( fGaugeWidth, fGaugeValue ) );
	}

	if( m_pRightGauge100 )
	{
		float fGaugeWidth = m_pRightGauge100->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fRightUserHPRate; 
		m_pRightGauge100->RenderWidthCut( iXPos + 489, iYPos + 39, 0, min( fGaugeWidth, fGaugeValue ) );
	}

	if( m_pRightGaugeGlow )
	{
		float fGaugeWidth = m_pRightGaugeGlow->GetWidth();
		float fGaugeValue = fGaugeWidth * m_fRightUserHPRate; 
		m_pRightGaugeGlow->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaValue );
		m_pRightGaugeGlow->RenderWidthCut( iXPos + 489, iYPos + 39, 0, min( fGaugeWidth, fGaugeValue ) );
	}

	if( m_pRightRedNameBar )
		m_pRightRedNameBar->Render( iXPos + 642, iYPos + 61 );

	int iStartX = 834;
	float fNameWidth = 130.0f;
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.PrintTextWidthCut( iXPos + iStartX, iYPos + 62, FONT_SIZE_14, fNameWidth, "%s", m_szRightUserViewName.c_str() );
	iStartX -= min( fNameWidth, g_FontMgr.GetTextWidth( m_szRightUserViewName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_14 ) );

	// render com
	ioUIRenderImage *pComGrade = GetComGradeRes( m_iCurFightNPCStage );
	if( pComGrade )
	{
		iStartX -= pComGrade->GetWidth();
		pComGrade->Render( iXPos+iStartX, iYPos+63 );
	}

	if( m_pRightBackLine )
		m_pRightBackLine->Render( iXPos + 477, iYPos + 35 );

	if( m_pRightBackLineLight )
	{
		m_pRightBackLineLight->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaValue );
		m_pRightBackLineLight->Render( iXPos + 477, iYPos + 35 );
	}
}

void BattleModeTimeGaugeWnd::OnFightNPCRender( int iXPos, int iYPos )
{
	enum
	{
		START_X	= -3,
		END_X	= 95,

		Y_POS	= -4,
	};

	int iNewXPos = iXPos+60+632;
	int iNewYPos = iYPos+86;

	if( !g_BattleRoomMgr.IsNoChallenger() )
	{
		if( m_pAcceptBack )
			m_pAcceptBack->Render( iNewXPos, iNewYPos );

		if( m_pAcceptPoint )
			m_pAcceptPoint->Render( iNewXPos+START_X+m_iAcceptX, iNewYPos+Y_POS, UI_RENDER_SCREEN );
	}
}
//////////////////////////////////////////////////////////////////////////
BattleModeFunctionWnd::BattleModeFunctionWnd() : m_pPlayStage( NULL )
{
	m_pBackFrm = NULL;
	m_pBackFrmLine = NULL;

	InitValue();
}

BattleModeFunctionWnd::~BattleModeFunctionWnd()
{
	SAFEDELETE( m_pBackFrm );
	SAFEDELETE( m_pBackFrmLine );
}

void BattleModeFunctionWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;

	SetFunctionButtonTitle();
}

void BattleModeFunctionWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BackFrm" )
	{
		SAFEDELETE( m_pBackFrm );
		m_pBackFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void BattleModeFunctionWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioWnd::AddRenderImage( szType, pImage );
}

void BattleModeFunctionWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	
}

void BattleModeFunctionWnd::iwm_show()
{
	InitValue();

	HideChildWnd( ID_TAG_REQUEST );
	HideChildWnd( ID_TAG_CANCEL );
	HideChildWnd( ID_CHAR_CHANGE );
	HideChildWnd( ID_INTRUDE );
}

void BattleModeFunctionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if(!m_pPlayStage)
		return;

	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_TAG_REQUEST:
	case ID_TAG_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			m_bTag = (dwID == ID_TAG_REQUEST) ? true : false;

			SwitchingTag( false );
		}
		break;
	case ID_CHAR_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			SelectSoldier();
		}
		break;
	case ID_INTRUDE:
		if( cmd == IOBN_BTNUP )
		{
			Entry();
		}
		break;
	case MESSAGE_BOX: // 난입 
		if( cmd == IOBN_BTNUP )
		{
			if( param != IDYES )
				return;

			ioBaseChar* pMyChar = m_pPlayStage->GetOwnerChar();
			int iNextUserHPRate = (int)((pMyChar->GetHP().m_fCurValue / pMyChar->GetHP().m_fMaxValue) * 100.0f);
			int iTagFlag = g_BattleRoomMgr.GetBattleModeTag(g_MyInfo.GetPublicID());
			bool bTagFlag = iTagFlag > 0 ? true : false;
			SP2Packet kPacket( CTPK_BATTLE_MODE_ENTRY_START );
			kPacket << g_MyInfo.GetPublicID() << iNextUserHPRate << bTagFlag;
			TCPNetwork::SendToServer( kPacket );
		}
		break;
	}
}

void BattleModeFunctionWnd::InitValue()
{
	// 리소스 초기화
	if( m_pBackFrm )
		m_pBackFrm->SetAlpha( MAX_ALPHA_RATE );
	if( m_pBackFrmLine )
		m_pBackFrmLine->SetAlpha( MAX_ALPHA_RATE );

	m_bShowFunctionButton = false;
	m_dwTagBtnDisableTime = 2000;			// 태그 신청 / 취소 버튼 Disable 상태 유저 시간 2초

	m_bTag = false;

	ShowFunctionButton(false);
}

void BattleModeFunctionWnd::SetChallenger()
{
	ShowWnd();
}

void BattleModeFunctionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;


}

void BattleModeFunctionWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderChallenger( iXPos, iYPos );
}

void BattleModeFunctionWnd::OnRenderChallenger( int iXPos, int iYPos )
{
	
}

void BattleModeFunctionWnd::ShowFunctionButton( bool bShow )
{
	m_bShowFunctionButton = bShow;

	if( m_bShowFunctionButton )
	{
		ShowChildWnd( ID_TAG_REQUEST );
		HideChildWnd( ID_TAG_CANCEL );
		ShowChildWnd( ID_CHAR_CHANGE );
		ShowChildWnd( ID_INTRUDE );
	}
	else
	{
		HideChildWnd( ID_TAG_REQUEST );
		HideChildWnd( ID_TAG_CANCEL );
		HideChildWnd( ID_CHAR_CHANGE );
		HideChildWnd( ID_INTRUDE );
	}
}

void BattleModeFunctionWnd::ShowTagRequestButton( bool bShow )
{
	if( bShow )
	{
		HideChildWnd( ID_TAG_REQUEST );
		ShowChildWnd( ID_TAG_CANCEL );
		// 태그 요청이면 태그 취소 버튼이 1.5초 동안 Disable상태 됨
		ioButton* pCancelBtn = (ioButton*)FindChildWnd( ID_TAG_CANCEL );
		if(pCancelBtn)
			pCancelBtn->SetInActive();
	}
	else
	{
		ShowChildWnd( ID_TAG_REQUEST );
		HideChildWnd( ID_TAG_CANCEL );
		// 태그 요청 취소면 태그 신청 버튼이 1.5초 동안 Disable상태 됨
		ioButton* pTagReqBtn = (ioButton*)FindChildWnd( ID_TAG_REQUEST );
		if(pTagReqBtn)
			pTagReqBtn->SetInActive();
	}
}

void BattleModeFunctionWnd::ActiveTagBtn()
{
	ioButton* pCancelBtn = (ioButton*)FindChildWnd( ID_TAG_CANCEL );
	ioButton* pTagReqBtn = (ioButton*)FindChildWnd( ID_TAG_REQUEST );
	
	if( pTagReqBtn )
	{
		if( pTagReqBtn->IsDisabled() )
			pTagReqBtn->SetActive();
	}
	if( pCancelBtn )
	{
		if( pCancelBtn->IsDisabled() )
			pCancelBtn->SetActive();
	}
}

void BattleModeFunctionWnd::SetFunctionButtonTitle()
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_INTRUDE ) );
	if( pButton && m_pPlayStage )
	{
		char szBuf[MAX_PATH];
		ioPlayMode* pPlayMode = m_pPlayStage->GetPlayMode();
		ioBattleMode* pBattleMode = ToBattleMode( pPlayMode );
		wsprintf( szBuf, STR(1), pBattleMode->GetCurEntryCount(), pBattleMode->GetMaxEntryCount() );
		pButton->SetTitleText(szBuf);
	}
	pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_CHAR_CHANGE ) );
	if( pButton )
		pButton->SetTitleText( STR(2) );
	pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_TAG_REQUEST ) );
	if( pButton )
		pButton->SetTitleText( STR(3) );
	pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_TAG_CANCEL ) );
	if( pButton )
		pButton->SetTitleText( STR(4) );
}

void BattleModeFunctionWnd::ResetEntryButtonTitle( int iEntryCount )
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_INTRUDE ) );
	if( pButton && m_pPlayStage )
	{
		char szBuf[MAX_PATH];
		ioPlayMode* pPlayMode = m_pPlayStage->GetPlayMode();
		ioBattleMode* pBattleMode = ToBattleMode( pPlayMode );
		wsprintf( szBuf, STR(1), iEntryCount, pBattleMode->GetMaxEntryCount() );
		pButton->SetTitleText(szBuf);
		if( iEntryCount <= 0)
			pButton->SetInActive();
	}
}

void BattleModeFunctionWnd::SwitchingTag( bool bFuncKey )
{
	// 태그 버튼 Disable상태 체크
	ioPlayMode* pPlayMode = m_pPlayStage->GetPlayMode();
	ioBattleMode* pBattleMode = ToBattleMode( pPlayMode );
	if( !pBattleMode )
		return;
	if( pBattleMode->GetTagBtnDisableTime() > 0 )
		return;

	if( bFuncKey )
		m_bTag = !m_bTag;
	ioBaseChar* pMyChar = m_pPlayStage->GetOwnerChar();
	if( pMyChar->GetState() == CS_DIE )
	{
		g_ChatMgr.SetSystemMsg( STR(1) );		// 죽어서 태그 요청 불가
		return;
	}

	SP2Packet kPacket( CTPK_BATTLE_MODE_TAG_WAIT_FOR_REQUEST );
	int iTagUserHPRate = (int)((pMyChar->GetHP().m_fCurValue / pMyChar->GetHP().m_fMaxValue) * 100.0f);
	kPacket << g_MyInfo.GetPublicID() << iTagUserHPRate << m_bTag;
	TCPNetwork::SendToServer( kPacket );
	
	// 태그 요청 / 취소 성공되거나 에러 패킷 올때가지 비활성화
	if( m_bTag )
	{
		ShowTagRequestButton(true);
	}
	else //if ( m_bTag == false )
	{
		ShowTagRequestButton(false);
	}
	pBattleMode->StartTagButtonInactive();
}

void BattleModeFunctionWnd::SelectSoldier()
{
	// 현재는 배틀모드에서는 *키 Press하면 용병 선택 창 뜸
	if( ioPlayMode::GetModeType() == MT_BATTLE )
	{
		ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
		if( pOwnerChar->GetState() == CS_DIE )
		{
			g_ChatMgr.SetSystemMsg( STR(1) );		// 죽어서 용병 변경 불가
			return;
		}

		SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
		if( pSoldier && pSoldier->IsShow() )
		{
			pSoldier->HideWnd();
		}
		else
		{
			if( pOwnerChar->IsOwnerChar() )
			{
				if( P2PNetwork::IsNetworkPlaying() )
				{
					if( pSoldier && !pSoldier->IsShow() )
					{
						pSoldier->SetSelectWaitShow( -1, m_pPlayStage->IsNetworkMode() );
					}
				}
			}
		}
	}
}

void BattleModeFunctionWnd::Entry()
{
	ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
	if( pOwnerChar->GetState() == CS_DIE )
	{
		g_ChatMgr.SetSystemMsg( STR(2) );		// 죽어서 용병 변경 불가
		return;
	}

	ioPlayMode* pPlayMode = m_pPlayStage->GetPlayMode();
	ioBattleMode* pBattleMode = ToBattleMode( pPlayMode );
	if( pBattleMode && pBattleMode->GetCurEntryCount() <= 0 )
	{
		g_ChatMgr.SetSystemMsg( STR(3) );
		return;
	}
	

	g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(1) );
}

//////////////////////////////////////////////////////////////////////////
BattleModeBlueSideUserInfoWnd::BattleModeBlueSideUserInfoWnd() : m_pPlayStage( NULL )
{
	InitValue();
}

BattleModeBlueSideUserInfoWnd::~BattleModeBlueSideUserInfoWnd()
{
	SAFEDELETE( m_pSaveUserSlotBg );
	SAFEDELETE( m_pSaveUserHpBg );
	SAFEDELETE( m_pSaveUserHp );
	SAFEDELETE( m_pSaveUserSoldierSlot );
	SAFEDELETE( m_pTagBG );
	SAFEDELETE( m_pEntryBG );
	SAFEDELETE( m_pSaveUserEntryGaugeBG );
	SAFEDELETE( m_pSaveUserEntryGauge );

	SAFEDELETE( m_pSaveTagState_Icon );
	SAFEDELETE( m_pSaveEntryState_Text );
	SAFEDELETE( m_pSaveEntryState_Icon );
	SAFEDELETE( m_pSaveEntryState_Text );

	SAFEDELETE( m_pSaveHP_Text );

	m_vBattleModeUserInfo.clear();
}

void BattleModeBlueSideUserInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
		{
			SAFEDELETE( m_vBattleModeUserInfo[i]->m_pSoldierIconBack );
			m_vBattleModeUserInfo[i]->m_pSoldierIconBack = pImage;
		}
	}
	else if( szType == "BlueUserSlot_BG" )
	{
		SAFEDELETE( m_pSaveUserSlotBg );
		m_pSaveUserSlotBg = pImage;
	}
	else if( szType == "BlueUser_HP_BG" )
	{
		SAFEDELETE( m_pSaveUserHpBg );
		m_pSaveUserHpBg = pImage;
	}
	else if( szType == "BlueUser_HP" )
	{
		SAFEDELETE( m_pSaveUserHp );
		m_pSaveUserHp = pImage;
	}
	else if( szType == "BlueSide_SoldierSlot_BG" )
	{
		SAFEDELETE( m_pSaveUserSoldierSlot );
		m_pSaveUserSoldierSlot = pImage;
	}
	else if( szType == "BlueSide_Tag_BG" )
	{
		SAFEDELETE( m_pTagBG );
		m_pTagBG = pImage;
	}
	else if( szType == "BlueSide_Entry_BG" )
	{
		SAFEDELETE( m_pEntryBG );
		m_pEntryBG = pImage;
	}
	else if( szType == "BlueUser_EntryGauge_BG" )
	{
		SAFEDELETE( m_pSaveUserEntryGaugeBG );
		m_pSaveUserEntryGaugeBG = pImage;
	}
	else if( szType == "BlueUser_EntryGauge" )
	{
		SAFEDELETE( m_pSaveUserEntryGauge );
		m_pSaveUserEntryGauge = pImage;
	}
	else if( szType == "BlueUser_TagAlarm_Icon" )
	{
		SAFEDELETE( m_pSaveTagState_Icon );
		m_pSaveTagState_Icon = pImage;
	}
	else if( szType == "BlueUser_TagAlarm_Text" )
	{
		SAFEDELETE( m_pSaveTagState_Text );
		m_pSaveTagState_Text = pImage;
	}
	else if( szType == "BlueUser_EntryAlarm_Icon" )
	{
		SAFEDELETE( m_pSaveEntryState_Icon );
		m_pSaveEntryState_Icon = pImage;
	}
	else if( szType == "BlueUser_EntryAlarm_Text" )
	{
		SAFEDELETE( m_pSaveEntryState_Text );
		m_pSaveEntryState_Text = pImage;
	}
	else if( szType == "BlueUser_HP_Text" )			// HP 텍스트 이미지
	{
		SAFEDELETE( m_pSaveHP_Text );
		m_pSaveHP_Text = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void BattleModeBlueSideUserInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
}

void BattleModeBlueSideUserInfoWnd::iwm_hide()
{

}

void BattleModeBlueSideUserInfoWnd::InitValue()
{
	m_vBattleModeUserInfo.clear();

	m_pSaveUserSlotBg = NULL;
	m_pSaveUserHpBg = NULL;
	m_pSaveUserHp = NULL;
	m_pSaveUserSoldierSlot = NULL;

	m_pSaveUserEntryGaugeBG = NULL;
	m_pSaveUserEntryGauge = NULL;

	m_pSaveTagState_Icon = NULL;
	m_pSaveTagState_Text = NULL;
	m_pSaveEntryState_Icon = NULL;
	m_pSaveEntryState_Text = NULL;

	m_pSaveHP_Text = NULL;

	m_pTagBG = NULL;
	m_pEntryBG = NULL;
}

void BattleModeBlueSideUserInfoWnd::SetBattleModeUser( ioBaseChar* pChar, int iWaitPosIdx )
{
	if( pChar == NULL ) return;
	// 문제가 있어서 임시로 막음
	//return;

	for(int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++)
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == pChar->GetCharName() )
			return;
	}

	m_vBattleModeUserInfo.push_back( new BattleModeUserInfo );

	int iIndex = (int)m_vBattleModeUserInfo.size() - 1;
	
	m_vBattleModeUserInfo[iIndex]->m_NickName = pChar->GetCharName();
	m_vBattleModeUserInfo[iIndex]->m_Team = TEAM_BLUE;
	const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( m_vBattleModeUserInfo[iIndex]->m_NickName );
	m_vBattleModeUserInfo[iIndex]->m_iGrade = kInfo.m_iGradeLevel;
	int iClassType = pChar->GetCharacterInfo().m_class_type;
	// 아이콘 이미지 객체 초기화
	SAFEDELETE( m_vBattleModeUserInfo[iIndex]->m_pSoldierIcon );
	SAFEDELETE( m_vBattleModeUserInfo[iIndex]->m_pSoldierIconBack );
	SAFEDELETE( m_vBattleModeUserInfo[iIndex]->m_pTagReq );
	SAFEDELETE( m_vBattleModeUserInfo[iIndex]->m_pIntrude );
	
	m_vBattleModeUserInfo[iIndex]->m_pSoldierIcon = g_MyInfo.GetMySoldierIcon( iClassType );
	m_vBattleModeUserInfo[iIndex]->m_bTagReq = false;
	m_vBattleModeUserInfo[iIndex]->m_bIntrude = false;
	m_vBattleModeUserInfo[iIndex]->m_iWaitPosIdx	= iWaitPosIdx;
	
	std::sort( m_vBattleModeUserInfo.begin(), m_vBattleModeUserInfo.end(), WaitPosIdxSort() );
}

void BattleModeBlueSideUserInfoWnd::ChangeSoldierIcon( ioBaseChar* pChar )
{
	for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == pChar->GetCharName() )
		{
			int iClassType = pChar->GetCharacterInfo().m_class_type;
			m_vBattleModeUserInfo[i]->m_pSoldierIcon = g_MyInfo.GetMySoldierIcon( iClassType );
		}
	}
}

void BattleModeBlueSideUserInfoWnd::SwapUserWaitPosIdx( ioBaseChar* pChar_IN, int iWaitPosIdx_IN, ioBaseChar* pChar_OUT, int iWaitPosIdx_OUT )
{
	for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == pChar_IN->GetCharName() )
			m_vBattleModeUserInfo[i]->m_iWaitPosIdx = iWaitPosIdx_IN;
		if( m_vBattleModeUserInfo[i]->m_NickName == pChar_OUT->GetCharName() )
			m_vBattleModeUserInfo[i]->m_iWaitPosIdx = iWaitPosIdx_OUT;
	}
	std::sort( m_vBattleModeUserInfo.begin(), m_vBattleModeUserInfo.end(), WaitPosIdxSort() );
}

void BattleModeBlueSideUserInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	OnProcessContinue();
}

void BattleModeBlueSideUserInfoWnd::OnProcessContinue()
{
	
}

void BattleModeBlueSideUserInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderUserInfo( iXPos, iYPos );

	OnRenderAlarm( iXPos, iYPos );
}

void BattleModeBlueSideUserInfoWnd::OnRenderUserInfo( int iXPos, int iYPos )
{
	// Y축 Top에서 거리가 120
	int iUserInfoYPos = iYPos + 120;

	for(int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++)
	{ 
		if( m_vBattleModeUserInfo[i]->m_Team == TEAM_BLUE )
		{
			// 출전하고 있는 사람은 건너 뜀
			if( m_vBattleModeUserInfo[i]->m_NickName == g_BattleRoomMgr.FindFightUserInBattleByTeam( TEAM_BLUE ) )
			{
				iUserInfoYPos -= 60;
				continue;
			}

			m_vBattleModeUserInfo[i]->m_pUserHpBg = m_pSaveUserHpBg;
			m_vBattleModeUserInfo[i]->m_pUserHp = m_pSaveUserHp;
			m_vBattleModeUserInfo[i]->m_pUserSoldierSlot = m_pSaveUserSoldierSlot;
			m_vBattleModeUserInfo[i]->m_pUserSlotBg = m_pSaveUserSlotBg;
			m_vBattleModeUserInfo[i]->m_pUserEntryGaugeBg = m_pSaveUserEntryGaugeBG;
			m_vBattleModeUserInfo[i]->m_pUserEntryGauge = m_pSaveUserEntryGauge;
			m_vBattleModeUserInfo[i]->m_pHP_Text = m_pSaveHP_Text;

			// 태그 요청 or 난입 상태 체크
			if( g_BattleRoomMgr.GetBattleModeTag( m_vBattleModeUserInfo[i]->m_NickName ) == ioBattleMode::TAG_POSSIBLE_REQ )
			{
				m_vBattleModeUserInfo[i]->m_pAlarmIcon = m_pSaveTagState_Icon;
				m_vBattleModeUserInfo[i]->m_pAlarmText = m_pSaveTagState_Text;
			}
			else if( g_BattleRoomMgr.CheckUserEntryState( m_vBattleModeUserInfo[i]->m_NickName ) )
			{
				m_vBattleModeUserInfo[i]->m_pAlarmIcon = m_pSaveEntryState_Icon;
				m_vBattleModeUserInfo[i]->m_pAlarmText = m_pSaveEntryState_Text;
			}
			else 
			{
				m_vBattleModeUserInfo[i]->m_pAlarmIcon = NULL;
				m_vBattleModeUserInfo[i]->m_pAlarmText = NULL;
			}

			float fUserHPRate = 0.0f;
			ioBaseChar* pChar = m_pPlayStage->GetBaseChar( m_vBattleModeUserInfo[i]->m_NickName );
			if( pChar )
			{
				// HP 표시
				if( m_vBattleModeUserInfo[i]->m_pUserHpBg )
					m_vBattleModeUserInfo[i]->m_pUserHpBg->Render( iXPos + 50, iUserInfoYPos - 6 + ( i * 60 ), UI_RENDER_NORMAL );

				CharState eCharState = pChar->GetState();
				if( m_vBattleModeUserInfo[i]->m_pUserHp  && m_pPlayStage )
				{
					float fGaugeWidth = 0.0f;
					float fGaugeValue = 0.0f;
					if( pChar && pChar->GetState() != CS_DIE )
					{
						fUserHPRate = (float)pChar->GetHP().m_fCurValue / (float)pChar->GetHP().m_fMaxValue;
						fGaugeWidth = m_vBattleModeUserInfo[i]->m_pUserHp->GetWidth();
						fGaugeValue = fGaugeWidth * fUserHPRate; 
					}
					m_vBattleModeUserInfo[i]->m_pUserHp->RenderWidthCut( iXPos + 50, iUserInfoYPos - 6 + ( i * 60 ), 0, min( fGaugeWidth, fGaugeValue ) );
				}

				if( m_vBattleModeUserInfo[i]->m_pUserSlotBg )
					m_vBattleModeUserInfo[i]->m_pUserSlotBg->Render( iXPos + 55, iUserInfoYPos + 5 + ( i * 60 ), UI_RENDER_NORMAL );

				if( m_vBattleModeUserInfo[i]->m_pUserEntryGaugeBg )
					m_vBattleModeUserInfo[i]->m_pUserEntryGaugeBg->Render( iXPos + 48, iUserInfoYPos + 24 + ( i * 60 ), UI_RENDER_NORMAL );
				if( m_vBattleModeUserInfo[i]->m_pUserEntryGauge )
				{
					if( eCharState != CS_DIE )
						m_vBattleModeUserInfo[i]->m_pUserEntryGauge->Render( iXPos + 49, iUserInfoYPos + 24 + ( i * 60 ), UI_RENDER_NORMAL );
					else
						m_vBattleModeUserInfo[i]->m_pUserEntryGauge->Render( iXPos + 49, iUserInfoYPos + 24 + ( i * 60 ), UI_RENDER_GRAY );
				}

				if( m_vBattleModeUserInfo[i]->m_pSoldierIcon )
				{
					m_vBattleModeUserInfo[i]->m_pSoldierIcon->SetScale( 0.80f );
					if( eCharState != CS_DIE )	// 죽지 않았을 경우
						m_vBattleModeUserInfo[i]->m_pSoldierIcon->Render( iXPos + 34, iUserInfoYPos + 12 + ( i * 60 ), UI_RENDER_NORMAL );
					else if( eCharState == CS_DIE )
						m_vBattleModeUserInfo[i]->m_pSoldierIcon->Render( iXPos + 34, iUserInfoYPos + 12 + ( i * 60 ), UI_RENDER_GRAY );
				}

				if( eCharState != CS_DIE )	// 죽지 않았을 경우
				{
					// 상태 (태그 요청 or 난입)
					if( m_vBattleModeUserInfo[i]->m_pAlarmIcon )
						m_vBattleModeUserInfo[i]->m_pAlarmIcon->Render( iXPos + 16, iUserInfoYPos + 30 + ( i * 60 ), UI_RENDER_NORMAL );
					if( m_vBattleModeUserInfo[i]->m_pAlarmText )
						m_vBattleModeUserInfo[i]->m_pAlarmText->Render( iXPos + 29, iUserInfoYPos + 22 + ( i * 60 ), UI_RENDER_NORMAL );
				}

				// HP Text 이미지
				if( m_vBattleModeUserInfo[i]->m_pHP_Text )
					m_vBattleModeUserInfo[i]->m_pHP_Text->Render( iXPos + 63, iUserInfoYPos - 5 + ( i * 60 ), UI_RENDER_NORMAL );

				int iTextYPos = iUserInfoYPos;
				if( i == 0 )
					iTextYPos += 7;
				else if( i == 2 )
					iTextYPos -= 7;
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_LEFT );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );		// 블루팀 텍스트 색상
				g_FontMgr.PrintText( iXPos + 81, iTextYPos + ( i * 67 ), FONT_SIZE_11, m_vBattleModeUserInfo[i]->m_NickName.c_str() );

				int IHP_Percent = (int)(fUserHPRate * 100.0f);
				char szBuf[MAX_PATH] = "";
				wsprintf( szBuf, "%d%%%%", IHP_Percent );
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_LEFT );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );		// 블루팀 HP 텍스트 색상
				g_FontMgr.PrintText( iXPos + 145, iTextYPos - 13 + ( i * 67 ), FONT_SIZE_11, szBuf );

				// 계급 
				g_LevelMgr.RenderGrade( m_vBattleModeUserInfo[i]->m_NickName, iXPos + 62, iTextYPos + ( i * 67 ), m_vBattleModeUserInfo[i]->m_iGrade, TEAM_BLUE );
			}
		}
	}
}

void BattleModeBlueSideUserInfoWnd::OnRenderAlarm( int iXPos, int iYPos )
{
	ioUIRenderImage* pTagSoldierIcon = NULL;

	DWORD dwCurTime = FRAMEGETTIME();

	AlarmOrderList::iterator alarmOrderIter = m_AlarmOrderList.begin();

	if( m_pTagBG && m_pEntryBG )
	{
		int iAlarmIndex = 0;
		while(alarmOrderIter != m_AlarmOrderList.end())
		{
			if( alarmOrderIter->m_eAlarmType == TAG_ALARM )
			{
				if( dwCurTime <= alarmOrderIter->m_dwEndTime )
				{
					m_pTagBG->Render( iXPos + 12, iYPos + 248 + ( iAlarmIndex * 85 ) );

					// 용병 아이콘 추출
					for(int j = 0; j < (int)m_vBattleModeUserInfo.size(); j++)
					{
						if( m_vBattleModeUserInfo[j]->m_NickName == alarmOrderIter->m_NickName )
						{
							if( m_vBattleModeUserInfo[j]->m_pSoldierIcon )
							{
								pTagSoldierIcon = m_vBattleModeUserInfo[j]->m_pSoldierIcon;
								pTagSoldierIcon->Render( iXPos + 27, iYPos + 276 + ( iAlarmIndex * 85 ), UI_RENDER_NORMAL, TFO_BILINEAR );

								g_FontMgr.SetTextStyle( TS_NORMAL );
								g_FontMgr.SetAlignType( TAT_CENTER );
								g_FontMgr.SetBkColor( 0, 0, 0 );
								g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );		// 블루팀 텍스트 색상
								g_FontMgr.PrintText( iXPos + 100, iYPos + 269 + ( iAlarmIndex * 85 ), FONT_SIZE_11, m_vBattleModeUserInfo[j]->m_NickName.c_str() );
							}
							break;
						}
					}
					alarmOrderIter++;
				}
				else		// 삭제
					m_AlarmOrderList.erase( alarmOrderIter++ );
			}
			else if( alarmOrderIter->m_eAlarmType == ENTRY_ALARM )
			{
				if( dwCurTime <= alarmOrderIter->m_dwEndTime )
				{
					m_pEntryBG->Render( iXPos + 12, iYPos + 248 + ( iAlarmIndex * 85 ) );

					// 용병 아이콘 추출
					for(int j = 0; j < (int)m_vBattleModeUserInfo.size(); j++)
					{
						if( m_vBattleModeUserInfo[j]->m_NickName == alarmOrderIter->m_NickName )
						{
							if( m_vBattleModeUserInfo[j]->m_pSoldierIcon )
							{
								pTagSoldierIcon = m_vBattleModeUserInfo[j]->m_pSoldierIcon;
								pTagSoldierIcon->Render( iXPos + 27, iYPos + 276 + ( iAlarmIndex * 85 ), UI_RENDER_NORMAL, TFO_BILINEAR );

								g_FontMgr.SetTextStyle( TS_NORMAL );
								g_FontMgr.SetAlignType( TAT_CENTER );
								g_FontMgr.SetBkColor( 0, 0, 0 );
								g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );		// 블루팀 텍스트 색상
								g_FontMgr.PrintText( iXPos + 100, iYPos + 269 + ( iAlarmIndex * 85 ), FONT_SIZE_11, m_vBattleModeUserInfo[j]->m_NickName.c_str() );
							}
							break;
						}
					}
					alarmOrderIter++;
				}
				else			// 삭제
					m_AlarmOrderList.erase( alarmOrderIter++ );
			}
			iAlarmIndex++;
		}
	}
}

void BattleModeBlueSideUserInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	
}

void BattleModeBlueSideUserInfoWnd::ShowTag( ioHashString& UserName )
{
	for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == UserName )
		{
			AlarmOrder alarm;
			alarm.m_dwEndTime = FRAMEGETTIME() + 10000;		// 임시 10초 - 2.5초
			alarm.m_eAlarmType = TAG_ALARM;
			alarm.m_NickName = UserName;
			m_AlarmOrderList.push_back( alarm );
			return;
		}
	}
}

void BattleModeBlueSideUserInfoWnd::ShowEntry( ioHashString& UserName )
{
	for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == UserName )
		{
			AlarmOrder alarm;
			alarm.m_dwEndTime = FRAMEGETTIME() + 10000;		// 임시 10초 - 2.5초
			alarm.m_eAlarmType = ENTRY_ALARM;
			alarm.m_NickName = UserName;
			m_AlarmOrderList.push_back( alarm );
			return;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
BattleModeRedSideUserInfoWnd::BattleModeRedSideUserInfoWnd() : m_pPlayStage( NULL )
{
	InitValue();
}

BattleModeRedSideUserInfoWnd::~BattleModeRedSideUserInfoWnd()
{
	SAFEDELETE( m_pSaveUserSlotBg );
	SAFEDELETE( m_pSaveUserHpBg );
	SAFEDELETE( m_pSaveUserHp );
	SAFEDELETE( m_pSaveUserSoldierSlot );
	SAFEDELETE( m_pTagBG );
	SAFEDELETE( m_pEntryBG );
	SAFEDELETE( m_pSaveUserEntryGaugeBG );
	SAFEDELETE( m_pSaveUserEntryGauge );

	SAFEDELETE( m_pSaveTagState_Icon );
	SAFEDELETE( m_pSaveTagState_Text );
	SAFEDELETE( m_pSaveEntryState_Icon );
	SAFEDELETE( m_pSaveEntryState_Text );

	SAFEDELETE( m_pSaveHP_Text );

	m_vBattleModeUserInfo.clear();
}

void BattleModeRedSideUserInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
		{
			SAFEDELETE( m_vBattleModeUserInfo[i]->m_pSoldierIconBack );
			m_vBattleModeUserInfo[i]->m_pSoldierIconBack = pImage;
		}
	}
	else if( szType == "RedUserSlot_BG" )
	{
		SAFEDELETE( m_pSaveUserSlotBg );
		m_pSaveUserSlotBg = pImage;
	}
	else if( szType == "RedUser_HP_BG" )
	{
		SAFEDELETE( m_pSaveUserHpBg );
		m_pSaveUserHpBg = pImage;
	}
	else if( szType == "RedUser_HP" )
	{
		SAFEDELETE( m_pSaveUserHp );
		m_pSaveUserHp = pImage;
	}
	else if( szType == "RedSide_SoldierSlot_BG" )
	{
		SAFEDELETE( m_pSaveUserSoldierSlot );
		m_pSaveUserSoldierSlot = pImage;
	}
	else if( szType == "RedSide_Tag_BG" )
	{
		SAFEDELETE( m_pTagBG );
		m_pTagBG = pImage;
	}
	else if( szType == "RedSide_Entry_BG" )
	{
		SAFEDELETE( m_pEntryBG );
		m_pEntryBG = pImage;
	}
	else if( szType == "RedUser_EntryGauge_BG" )
	{
		SAFEDELETE( m_pSaveUserEntryGaugeBG );
		m_pSaveUserEntryGaugeBG = pImage;
	}
	else if( szType == "RedUser_EntryGauge" )
	{
		SAFEDELETE( m_pSaveUserEntryGauge );
		m_pSaveUserEntryGauge = pImage;
	}
	else if( szType == "RedUser_TagAlarm_Icon" )
	{
		SAFEDELETE( m_pSaveTagState_Icon );
		m_pSaveTagState_Icon = pImage;
	}
	else if( szType == "RedUser_TagAlarm_Text" )
	{
		SAFEDELETE( m_pSaveTagState_Text );
		m_pSaveTagState_Text = pImage;
	}
	else if( szType == "RedUser_EntryAlarm_Icon" )
	{
		SAFEDELETE( m_pSaveEntryState_Icon );
		m_pSaveEntryState_Icon = pImage;
	}
	else if( szType == "RedUser_EntryAlarm_Text" )
	{
		SAFEDELETE( m_pSaveEntryState_Text );
		m_pSaveEntryState_Text = pImage;
	}
	else if( szType == "RedUser_HP_Text" )			// HP 텍스트 이미지
	{
		SAFEDELETE( m_pSaveHP_Text );
		m_pSaveHP_Text = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void BattleModeRedSideUserInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
}

void BattleModeRedSideUserInfoWnd::iwm_hide()
{

}

void BattleModeRedSideUserInfoWnd::InitValue()
{
	m_vBattleModeUserInfo.clear();

	m_pSaveUserSlotBg = NULL;
	m_pSaveUserHpBg = NULL;
	m_pSaveUserHp = NULL;
	m_pSaveUserSoldierSlot = NULL;

	m_pTagBG = NULL;
	m_pEntryBG = NULL;

	m_pSaveUserEntryGaugeBG = NULL;
	m_pSaveUserEntryGauge = NULL;

	m_pSaveTagState_Icon = NULL;
	m_pSaveTagState_Text = NULL;
	m_pSaveEntryState_Icon = NULL;
	m_pSaveEntryState_Text = NULL;

	m_pSaveHP_Text = NULL;
}

void BattleModeRedSideUserInfoWnd::SetBattleModeUser( ioBaseChar* pChar, int iWaitPosIdx )
{
	if( pChar == NULL ) return;

	for(int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++)
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == pChar->GetCharName() )
			return;
	}

	m_vBattleModeUserInfo.push_back( new BattleModeUserInfo );

	int iIndex = (int)m_vBattleModeUserInfo.size() - 1;

	if( iIndex >= 3 )
		return;

	m_vBattleModeUserInfo[iIndex]->m_NickName = pChar->GetCharName();
	m_vBattleModeUserInfo[iIndex]->m_Team = TEAM_RED;
	const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( m_vBattleModeUserInfo[iIndex]->m_NickName );
	m_vBattleModeUserInfo[iIndex]->m_iGrade = kInfo.m_iGradeLevel;
	int iClassType = pChar->GetCharacterInfo().m_class_type;
	// 아이콘 이미지 객체 초기화
	SAFEDELETE( m_vBattleModeUserInfo[iIndex]->m_pSoldierIcon );
	SAFEDELETE( m_vBattleModeUserInfo[iIndex]->m_pSoldierIconBack );
	SAFEDELETE( m_vBattleModeUserInfo[iIndex]->m_pTagReq );
	SAFEDELETE( m_vBattleModeUserInfo[iIndex]->m_pIntrude );

	m_vBattleModeUserInfo[iIndex]->m_pSoldierIcon = g_MyInfo.GetMySoldierIcon( iClassType );
	m_vBattleModeUserInfo[iIndex]->m_bTagReq = false;
	m_vBattleModeUserInfo[iIndex]->m_bIntrude = false;
	m_vBattleModeUserInfo[iIndex]->m_iWaitPosIdx	= iWaitPosIdx;

	std::sort( m_vBattleModeUserInfo.begin(), m_vBattleModeUserInfo.end(), WaitPosIdxSort() );
}

void BattleModeRedSideUserInfoWnd::ChangeSoldierIcon( ioBaseChar* pChar )
{
	for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == pChar->GetCharName() )
		{
			int iClassType = pChar->GetCharacterInfo().m_class_type;
			m_vBattleModeUserInfo[i]->m_pSoldierIcon = g_MyInfo.GetMySoldierIcon( iClassType );
		}
	}
}

void BattleModeRedSideUserInfoWnd::SwapUserWaitPosIdx( ioBaseChar* pChar_IN, int iWaitPosIdx_IN, ioBaseChar* pChar_OUT, int iWaitPosIdx_OUT )
{
	for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == pChar_IN->GetCharName() )
			m_vBattleModeUserInfo[i]->m_iWaitPosIdx = iWaitPosIdx_IN;
		if( m_vBattleModeUserInfo[i]->m_NickName == pChar_OUT->GetCharName() )
			m_vBattleModeUserInfo[i]->m_iWaitPosIdx = iWaitPosIdx_OUT;
	}
	std::sort( m_vBattleModeUserInfo.begin(), m_vBattleModeUserInfo.end(), WaitPosIdxSort() );
}

void BattleModeRedSideUserInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	OnProcessContinue();
}

void BattleModeRedSideUserInfoWnd::OnProcessContinue()
{

}

void BattleModeRedSideUserInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderUserInfo( iXPos, iYPos );

	OnRenderAlarm( iXPos, iYPos );
}

void BattleModeRedSideUserInfoWnd::OnRenderUserInfo( int iXPos, int iYPos )
{
	// Y축 Top에서 거리가 120
	int iUserInfoYPos = iYPos + 120;
	
	for(int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++)
	{
		if( m_vBattleModeUserInfo[i]->m_Team == TEAM_RED )
		{
			// 출전하고 있는 사람은 건너 뜀
			if( m_vBattleModeUserInfo[i]->m_NickName == g_BattleRoomMgr.FindFightUserInBattleByTeam( TEAM_RED ) )
			{
				iUserInfoYPos -= 60;
				continue;
			}

			m_vBattleModeUserInfo[i]->m_pUserHpBg = m_pSaveUserHpBg;
			m_vBattleModeUserInfo[i]->m_pUserHp = m_pSaveUserHp;
			m_vBattleModeUserInfo[i]->m_pUserSoldierSlot = m_pSaveUserSoldierSlot;
			m_vBattleModeUserInfo[i]->m_pUserSlotBg = m_pSaveUserSlotBg;
			m_vBattleModeUserInfo[i]->m_pUserEntryGaugeBg = m_pSaveUserEntryGaugeBG;
			m_vBattleModeUserInfo[i]->m_pUserEntryGauge = m_pSaveUserEntryGauge;
			m_vBattleModeUserInfo[i]->m_pHP_Text = m_pSaveHP_Text;

			// 태그 요청 or 난입 상태 체크
			if( g_BattleRoomMgr.GetBattleModeTag( m_vBattleModeUserInfo[i]->m_NickName ) == ioBattleMode::TAG_POSSIBLE_REQ )
			{
				m_vBattleModeUserInfo[i]->m_pAlarmIcon = m_pSaveTagState_Icon;
				m_vBattleModeUserInfo[i]->m_pAlarmText = m_pSaveTagState_Text;
			}
			else if( g_BattleRoomMgr.CheckUserEntryState( m_vBattleModeUserInfo[i]->m_NickName ) )
			{
				m_vBattleModeUserInfo[i]->m_pAlarmIcon = m_pSaveEntryState_Icon;
				m_vBattleModeUserInfo[i]->m_pAlarmText = m_pSaveEntryState_Text;
			}
			else 
			{
				m_vBattleModeUserInfo[i]->m_pAlarmText = NULL;
				m_vBattleModeUserInfo[i]->m_pAlarmIcon = NULL;
			}

			float fUserHPRate = 0.0f;
			ioBaseChar* pChar = m_pPlayStage->GetBaseChar( m_vBattleModeUserInfo[i]->m_NickName );
			if( pChar )
			{
				// HP 표시
				if( m_vBattleModeUserInfo[i]->m_pUserHpBg )
					m_vBattleModeUserInfo[i]->m_pUserHpBg->Render( iXPos + 13, iUserInfoYPos - 6 + ( i * 60 ), UI_RENDER_NORMAL, TFO_BILINEAR );

				CharState eCharState = pChar->GetState();
				if( m_vBattleModeUserInfo[i]->m_pUserHp && m_pPlayStage )
				{
					float fGaugeWidth = 0.0f;
					float fGaugeValue = 0.0f;
					if( eCharState != CS_DIE )
					{
						fUserHPRate = (float)pChar->GetHP().m_fCurValue / (float)pChar->GetHP().m_fMaxValue;
						fGaugeWidth = m_vBattleModeUserInfo[i]->m_pUserHp->GetWidth();
						fGaugeValue = fGaugeWidth - ( fGaugeWidth * fUserHPRate ); 
					}

					m_vBattleModeUserInfo[i]->m_pUserHp->RenderWidthCut( iXPos + 13, iUserInfoYPos - 6 + ( i * 60 ), min( fGaugeWidth, max( 0, fGaugeValue ) ), fGaugeWidth );
				}

				if( m_vBattleModeUserInfo[i]->m_pUserSlotBg )
					m_vBattleModeUserInfo[i]->m_pUserSlotBg->Render( iXPos + 23, iUserInfoYPos + 5 + ( i * 60 ), UI_RENDER_NORMAL, TFO_BILINEAR );

				if( m_vBattleModeUserInfo[i]->m_pUserEntryGaugeBg )
					m_vBattleModeUserInfo[i]->m_pUserEntryGaugeBg->Render( iXPos + 37, iUserInfoYPos + 24 + ( i * 60 ), UI_RENDER_NORMAL );
				if( m_vBattleModeUserInfo[i]->m_pUserEntryGauge )
				{
					if( eCharState != CS_DIE )
						m_vBattleModeUserInfo[i]->m_pUserEntryGauge->Render( iXPos + 37, iUserInfoYPos + 24 + ( i * 60 ), UI_RENDER_NORMAL );
					else
						m_vBattleModeUserInfo[i]->m_pUserEntryGauge->Render( iXPos + 37, iUserInfoYPos + 24 + ( i * 60 ), UI_RENDER_GRAY );
				}

				if( m_vBattleModeUserInfo[i]->m_pSoldierIcon )
				{
					m_vBattleModeUserInfo[i]->m_pSoldierIcon->SetScale( 0.80f );
					if( pChar && eCharState != CS_DIE )	// 죽지 않았을 경우
						m_vBattleModeUserInfo[i]->m_pSoldierIcon->Render( iXPos + 167, iUserInfoYPos + 12 + ( i * 60 ), UI_RENDER_NORMAL, TFO_BILINEAR );
					else if( pChar && eCharState == CS_DIE )	// 죽었을 경우
						m_vBattleModeUserInfo[i]->m_pSoldierIcon->Render( iXPos + 167, iUserInfoYPos + 12 + ( i * 60 ), UI_RENDER_GRAY, TFO_BILINEAR );
				}

				if( eCharState != CS_DIE )	// 죽지 않았을 경우
				{
					// 상태 (태그 요청 or 난입)
					if( m_vBattleModeUserInfo[i]->m_pAlarmIcon )
						m_vBattleModeUserInfo[i]->m_pAlarmIcon->Render( iXPos + 133, iUserInfoYPos + 30 + ( i * 60 ), UI_RENDER_NORMAL );
					if( m_vBattleModeUserInfo[i]->m_pAlarmText )
						m_vBattleModeUserInfo[i]->m_pAlarmText->Render( iXPos + 145, iUserInfoYPos + 22 + ( i * 60 ), UI_RENDER_NORMAL );
				}

				// HP Text 이미지
				if( m_vBattleModeUserInfo[i]->m_pHP_Text )
					m_vBattleModeUserInfo[i]->m_pHP_Text->Render( iXPos + 30, iUserInfoYPos - 5 + ( i * 60 ), UI_RENDER_NORMAL, TFO_BILINEAR );

				int iTextYPos = iUserInfoYPos;
				if(i == 0)
					iTextYPos += 7;
				else if( i == 2 )
					iTextYPos -= 7;
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_LEFT );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );		// 레드팀 텍스트 색상
				g_FontMgr.PrintText( iXPos + 51, iTextYPos + ( i * 67 ), FONT_SIZE_11, m_vBattleModeUserInfo[i]->m_NickName.c_str() );

				int IHP_Percent = (int)(fUserHPRate * 100.0f);
				char szBuf[MAX_PATH] = "";
				wsprintf( szBuf, "%d%%%%", IHP_Percent );
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_LEFT );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );		// 레드팀 HP 텍스트 색상
				g_FontMgr.PrintText( iXPos + 113, iTextYPos - 13 + ( i * 67 ), FONT_SIZE_11, szBuf );

				// 계급 
				g_LevelMgr.RenderGrade( m_vBattleModeUserInfo[i]->m_NickName, iXPos + 32, iTextYPos + ( i * 67 ), m_vBattleModeUserInfo[i]->m_iGrade, TEAM_RED );
			}
		}
	}
}

void BattleModeRedSideUserInfoWnd::OnRenderAlarm( int iXPos, int iYPos )
{
	ioUIRenderImage* pTagSoldierIcon = NULL;

	DWORD dwCurTime = FRAMEGETTIME();

	AlarmOrderList::iterator alarmOrderIter = m_AlarmOrderList.begin();

	if( m_pTagBG && m_pEntryBG )
	{
		// 레드팀 알람 렌더링 할때 Y축이 좀 이상해서 보정값 추가함
		iYPos += 40;
		int iAlarmIndex = 0;
		while(alarmOrderIter != m_AlarmOrderList.end())
		{
			if( alarmOrderIter->m_eAlarmType == TAG_ALARM )
			{
				if( dwCurTime <= alarmOrderIter->m_dwEndTime )
				{
					m_pTagBG->Render( iXPos + 20, iYPos + 248 + ( iAlarmIndex * 85 ) );

					// 용병 아이콘 추출
					for(int j = 0; j < (int)m_vBattleModeUserInfo.size(); j++)
					{
						if( m_vBattleModeUserInfo[j]->m_NickName == alarmOrderIter->m_NickName )
						{
							if( m_vBattleModeUserInfo[j]->m_pSoldierIcon )
							{
								pTagSoldierIcon = m_vBattleModeUserInfo[j]->m_pSoldierIcon;
								pTagSoldierIcon->Render( iXPos + 180, iYPos + 283 + ( iAlarmIndex * 85 ), UI_RENDER_NORMAL, TFO_BILINEAR );

								g_FontMgr.SetTextStyle( TS_NORMAL );
								g_FontMgr.SetAlignType( TAT_CENTER );
								g_FontMgr.SetBkColor( 0, 0, 0 );
								g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );		// 레드팀 텍스트 색상
								g_FontMgr.PrintText( iXPos + 105, iYPos + 269 + ( iAlarmIndex * 85 ), FONT_SIZE_11, m_vBattleModeUserInfo[j]->m_NickName.c_str() );
							}
							break;
						}
					}
					alarmOrderIter++;
				}
				else		// 삭제
					m_AlarmOrderList.erase( alarmOrderIter++ );
			}
			else if( alarmOrderIter->m_eAlarmType == ENTRY_ALARM )
			{
				if( dwCurTime <= alarmOrderIter->m_dwEndTime )
				{
					m_pEntryBG->Render( iXPos + 20, iYPos + 248 + ( iAlarmIndex * 85 ) );

					// 용병 아이콘 추출
					for(int j = 0; j < (int)m_vBattleModeUserInfo.size(); j++)
					{
						if( m_vBattleModeUserInfo[j]->m_NickName == alarmOrderIter->m_NickName )
						{
							if( m_vBattleModeUserInfo[j]->m_pSoldierIcon )
							{
								pTagSoldierIcon = m_vBattleModeUserInfo[j]->m_pSoldierIcon;
								pTagSoldierIcon->Render( iXPos + 180, iYPos + 283 + ( iAlarmIndex * 85 ), UI_RENDER_NORMAL, TFO_BILINEAR );

								g_FontMgr.SetTextStyle( TS_NORMAL );
								g_FontMgr.SetAlignType( TAT_CENTER );
								g_FontMgr.SetBkColor( 0, 0, 0 );
								g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );		// 레드팀 텍스트 색상
								g_FontMgr.PrintText( iXPos + 105, iYPos + 269 + ( iAlarmIndex * 85 ), FONT_SIZE_11, m_vBattleModeUserInfo[j]->m_NickName.c_str() );
							}
							break;
						}
					}
					alarmOrderIter++;
				}
				else			// 삭제
					m_AlarmOrderList.erase( alarmOrderIter++ );
			}
			iAlarmIndex++;
		}
	}
}

void BattleModeRedSideUserInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	
}

void BattleModeRedSideUserInfoWnd::ShowTag( ioHashString& UserName )
{
	for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == UserName )
		{
			AlarmOrder alarm;
			alarm.m_dwEndTime = FRAMEGETTIME() + 10000;		// 임시 10초 - 2.5초
			alarm.m_eAlarmType = TAG_ALARM;
			alarm.m_NickName = UserName;
			m_AlarmOrderList.push_back( alarm );
			return;
		}
	}
}

void BattleModeRedSideUserInfoWnd::ShowEntry( ioHashString& UserName )
{
	for( int i = 0; i < (int)m_vBattleModeUserInfo.size(); i++ )
	{
		if( m_vBattleModeUserInfo[i]->m_NickName == UserName )
		{
			AlarmOrder alarm;
			alarm.m_dwEndTime = FRAMEGETTIME() + 10000;		// 임시 10초 - 2.5초
			alarm.m_eAlarmType = ENTRY_ALARM;
			alarm.m_NickName = UserName;
			m_AlarmOrderList.push_back( alarm );
			return;
		}
	}
}