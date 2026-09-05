#include "StdAfx.h"

#include "../ioVoiceChat/Fourier.h"
#include "../io2DCircleGauge.h"
#include "../ioNpcChar.h"
#include "../ioGuildMarkMgr.h"
#include "../ioTournamentManager.h"
#include "../ioTournamentTeamManager.h"

#include "FightClubWnd.h"
#include "UserInfoToolTip.h"

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
FightClub3DChar::FightClub3DChar()
{
	m_pUI3DRender = NULL;
	m_pAvata      = NULL;
	
	m_iOffsetXPos = 0;
	m_iOffsetYPos = 0;
}

FightClub3DChar::~FightClub3DChar()
{
	SAFEDELETE( m_pAvata );
	SAFEDELETE( m_pUI3DRender );
}

void FightClub3DChar::Initialize()
{
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void FightClub3DChar::SetChar( ioBaseChar *pChar, bool bLeftChar )
{
	if( !pChar ) return;
	if( !m_pUI3DRender )	return;

	const CHARACTER &rkInfo = pChar->GetCharacterInfo();

	SAFEDELETE( m_pAvata );
	m_pAvata = m_pUI3DRender->CreateChar( rkInfo, "ResultChar" );
	if( !m_pAvata )
	{
		LOG.PrintTimeAndLog( 0, "FightClub3DChar::SetAvata - CreateAvata Fail" );
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

void FightClub3DChar::FaceChange( const char *szTexPostfix, int iDuration )
{
	if( m_pAvata )
	{
		m_pAvata->DirectFaceChange( szTexPostfix, iDuration );
	}
}

void FightClub3DChar::SetHelmChange( ioBaseChar *pChar )
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

void FightClub3DChar::Update( float fTimePerSec )
{
	if( m_pAvata )
	{
		m_pAvata->Update( fTimePerSec );
	}
}

void FightClub3DChar::Render( int iXPos, int iYPos )
{
	if( m_pAvata && m_pUI3DRender )
	{
		D3DXMATRIX matUI = m_pUI3DRender->MakeUIProjection( iXPos + m_iOffsetXPos, iYPos + m_iOffsetYPos );
		m_pUI3DRender->Render( &matUI, true );
	}
}
//////////////////////////////////////////////////////////////////////////
bool FightClubTimeGaugeWnd::m_bPosToDefualt = false;
FightClubTimeGaugeWnd::FightClubTimeGaugeWnd() : m_pPlayStage( NULL )
{
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

	m_pChampionText		= NULL;
	m_pLeftBackLine		= NULL;
	m_pLeftBackLineLight= NULL;
	m_pLeftGaugeBack	= NULL;
	m_pLeftGauge100		= NULL;
	m_pLeftGaugeGlow	= NULL;
	m_pLeftGaugeRed		= NULL;
	m_pLeftBlueNameBar	= NULL;
	m_pLeftRedNameBar	= NULL;

	m_pChallengerText	= NULL;
	m_pRightBackLine	= NULL;
	m_pRightBackLineLight= NULL;
	m_pRightGaugeBack	= NULL;
	m_pRightGauge100	= NULL;
	m_pRightGaugeGlow	= NULL;
	m_pRightGaugeRed	= NULL;
	m_pRightBlueNameBar	= NULL;
	m_pRightRedNameBar	= NULL;

	m_pChampionWinBar	= NULL;
	m_pChampionWinEff	= NULL;
	m_pChampionWinNum	= NULL;

	m_pSeqMeBar         = NULL;
	m_pNextMeBattle     = NULL;
	m_pOwnerReserveChar = NULL;
	m_pOwnerReserveSubChar = NULL;

	m_pCheerBack		= NULL;
	m_pChampCheerPoint	= NULL;
	m_pChampCheerNumber	= NULL;
	m_pChampCheerText	= NULL;
	m_pChallCheerPoint	= NULL;
	m_pChallCheerNumber	= NULL;
	m_pChallCheerText	= NULL;
	m_pCheerTimeBack	= NULL;

	m_pFightNpcClearBack= NULL;
	m_pFightNpcClearWin = NULL;

	m_pCircleGaugeNor   = NULL;
	m_pCircleGaugeDan   = NULL;

	m_pAcceptBack = NULL;
	m_pAcceptPoint = NULL;

	m_pFightNPCRoundText = NULL;
	m_pFightNPCRoundNum = NULL;

	m_iBlindOffSetY     = 0;
	m_iPreBlindOffSetY  = 0;

	m_iDefaultX = 0;
	m_iDefaultY = 0;
	
	m_iRemainSecond		= 0;
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

	m_bUseFightNPC = false;
	m_iCurFightNPCStage = 0;
}

FightClubTimeGaugeWnd::~FightClubTimeGaugeWnd()
{
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

	SAFEDELETE( m_pChampionText );
	SAFEDELETE( m_pLeftBackLine );
	SAFEDELETE( m_pLeftBackLineLight );
	SAFEDELETE( m_pLeftGaugeBack );
	SAFEDELETE( m_pLeftGauge100 );
	SAFEDELETE( m_pLeftGaugeGlow );
	SAFEDELETE( m_pLeftGaugeRed );
	SAFEDELETE( m_pLeftBlueNameBar );
	SAFEDELETE( m_pLeftRedNameBar );

	SAFEDELETE( m_pChallengerText );
	SAFEDELETE( m_pRightBackLine );
	SAFEDELETE( m_pRightBackLineLight );
	SAFEDELETE( m_pRightGaugeBack );
	SAFEDELETE( m_pRightGauge100 );
	SAFEDELETE( m_pRightGaugeGlow );
	SAFEDELETE( m_pRightGaugeRed );
	SAFEDELETE( m_pRightBlueNameBar );
	SAFEDELETE( m_pRightRedNameBar );

	SAFEDELETE( m_pChampionWinBar );
	SAFEDELETE( m_pChampionWinEff );
	SAFEDELETE( m_pChampionWinNum );
	
	SAFEDELETE( m_pLeftCharIcon );
	SAFEDELETE( m_pRightCharIcon );

	SAFEDELETE( m_pSeqMeBar );
	SAFEDELETE( m_pNextMeBattle );
	SAFEDELETE( m_pOwnerReserveChar );
	SAFEDELETE( m_pOwnerReserveSubChar );

	SAFEDELETE( m_pCheerBack );
	SAFEDELETE( m_pChampCheerPoint );
	SAFEDELETE( m_pChampCheerNumber );
	SAFEDELETE( m_pChampCheerText );
	SAFEDELETE( m_pChallCheerPoint );
	SAFEDELETE( m_pChallCheerNumber );
	SAFEDELETE( m_pChallCheerText );
	SAFEDELETE( m_pCheerTimeBack );
	SAFEDELETE( m_pFightNpcClearBack );
	SAFEDELETE( m_pFightNpcClearWin );

	SAFEDELETE( m_pCircleGaugeNor );
	SAFEDELETE( m_pCircleGaugeDan );

	SAFEDELETE( m_pAcceptBack );
	SAFEDELETE( m_pAcceptPoint );

	SAFEDELETE( m_pFightNPCRoundText );
	SAFEDELETE( m_pFightNPCRoundNum );

	ComGradeMap::iterator iCreator = m_ComGradeList.begin();
	for(;iCreator != m_ComGradeList.end();iCreator++)
	{
		SAFEDELETE( iCreator->second );
	}
	m_ComGradeList.clear();

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "FightClubTimeGaugeWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "FightClubTimeGaugeWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void FightClubTimeGaugeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TimeText" )
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
	else if( szType == "ChampionText" )
	{
		SAFEDELETE( m_pChampionText );
		m_pChampionText = pImage;
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
	else if( szType == "LeftGaugeBack" )
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
	else if( szType == "ChallengerText" )
	{
		SAFEDELETE( m_pChallengerText );
		m_pChallengerText = pImage;
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
	else if( szType == "RightGaugeBack" )
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
	else if( szType == "ChampionWinBar" )
	{
		SAFEDELETE( m_pChampionWinBar );
		m_pChampionWinBar = pImage;
	}
	else if( szType == "ChampionWinEff" )
	{
		SAFEDELETE( m_pChampionWinEff );
		m_pChampionWinEff = pImage;
	}
	else if( szType == "ChampionWinNum" )
	{
		SAFEDELETE( m_pChampionWinNum );
		m_pChampionWinNum = pImage;
	}
	else if( szType == "SeqMeBar" )
	{
		SAFEDELETE( m_pSeqMeBar );
		m_pSeqMeBar = pImage;
	}
	else if( szType == "NextMeBattle" )
	{
		SAFEDELETE( m_pNextMeBattle );
		m_pNextMeBattle = pImage;
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
	else if( szType == "FightNpcClearBack" )
	{
		SAFEDELETE( m_pFightNpcClearBack );
		m_pFightNpcClearBack = pImage;
	}
	else if( szType == "FightNpcClearWin" )
	{
		SAFEDELETE( m_pFightNpcClearWin );
		m_pFightNpcClearWin = pImage;
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
	else if( szType == "FightNPCRoundText" )
	{
		SAFEDELETE( m_pFightNPCRoundText );
		m_pFightNPCRoundText = pImage;
	}
	else if( szType == "FightNPCRoundNum" )
	{
		SAFEDELETE( m_pFightNPCRoundNum );
		m_pFightNPCRoundNum = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void FightClubTimeGaugeWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void FightClubTimeGaugeWnd::ClearCharSeqInfo()
{
	m_iChampionWinCount = 0;
	m_szLeftUserName.Clear();
	m_szRightUserName.Clear();
	m_szRightUserViewName.Clear();
	m_CharSeqData.clear();
}

void FightClubTimeGaugeWnd::InsertCharSeqInfo( const ioHashString &rkName, int iState, int iSeq, int iFightVictories, int iCheer, int iLevel )
{
	if( iState == FIGHTCLUB_CHAMPION )
	{
		if( m_pPlayStage == NULL ) return;

		ioBaseChar *pChar = m_pPlayStage->GetBaseChar( rkName );
		if( pChar == NULL ) return;

		SetChampionUser( pChar );
		m_iChampionWinCount = iFightVictories;
	}
	else if( iState == FIGHTCLUB_CHALLENGER )
	{
		if( m_pPlayStage == NULL ) return;

		ioBaseChar *pChar = m_pPlayStage->GetBaseChar( rkName );
		if( pChar == NULL ) return;

		SetChallengerUser( pChar );
	}
	else
	{
		if( g_BattleRoomMgr.IsObserverUser( rkName ) )
			return;
		
		bool bAdd = true;
		CharSeqDataVec::iterator iter = m_CharSeqData.begin();
		for(;iter != m_CharSeqData.end();iter++)
		{
			CharSeqData &rkData = *iter;
			if( rkData.m_kName == rkName )
			{
				bAdd = false;

				rkData.m_iSeq		= iSeq;
				rkData.m_iLevel		= iLevel;
				rkData.m_iCheer     = iCheer;

				// 버튼 교체
				if( rkData.m_kName == g_MyInfo.GetPublicID() )
				{
					if( (int)m_dwCheerTabID != rkData.m_iCheer )
					{
						CheckCheerButton( (DWORD)rkData.m_iCheer );
					}
				}
				break;
			}
		}

		if( bAdd )
		{
			CharSeqData kData;
			kData.m_kName		= rkName;
			kData.m_iSeq		= iSeq;
			kData.m_iLevel		= iLevel;
			kData.m_iCheer		= iCheer;
			m_CharSeqData.push_back( kData );

			// 버튼 교체
			if( kData.m_kName == g_MyInfo.GetPublicID() )
			{
				if( (int)m_dwCheerTabID != kData.m_iCheer )
				{
					CheckCheerButton( (DWORD)kData.m_iCheer );
				}
			}
		}
		std::sort( m_CharSeqData.begin(), m_CharSeqData.end(), CharSeqDataSort() );
	}
}

void FightClubTimeGaugeWnd::InsertNPCSeqInfo( const ioHashString &rkName, int iState, int iStage )
{
	if( iState == FIGHTCLUB_CHALLENGER )
	{
		if( m_pPlayStage == NULL ) return;

		ioBaseChar *pChar = m_pPlayStage->GetBaseChar( rkName );
		if( pChar == NULL ) return;

		SetChallengerUser( pChar );
		m_iCurFightNPCStage = iStage;
	}
}

void FightClubTimeGaugeWnd::RemoveCharSeqInfo( const ioHashString &rkName )
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

void FightClubTimeGaugeWnd::SetChampionUser( ioBaseChar *pChar )
{
	if( pChar == NULL ) return;

	m_szLeftUserName		= pChar->GetCharName();
	m_dwLfetGaugeTime		= FRAMEGETTIME();
	m_bLeftGaugeStart		= true;
	m_fLeftCurrentHPRate	= 0.0f;

	if( pChar->IsOwnerChar() )
		m_iOwnerReserveCharType = -1;           // 예약 용병 초기화

	if( !m_pLeftCharIcon )
	{
		m_pLeftCharIcon = new FightClub3DChar;
		m_pLeftCharIcon->Initialize();
	}

	if( m_pLeftCharIcon )
	{
		m_pLeftCharIcon->SetChar( pChar, true );
	}
}

void FightClubTimeGaugeWnd::SetChallengerUser( ioBaseChar *pChar )
{
	if( pChar == NULL ) return;

	ioHashString szCharName = pChar->GetCharName();
	if( ToNpcChar( pChar ) )
	{
		szCharName = ToNpcChar( pChar )->GetViewName();
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
		m_pRightCharIcon = new FightClub3DChar;
		m_pRightCharIcon->Initialize();
	}

	if( m_pRightCharIcon )
	{
		m_pRightCharIcon->SetChar( pChar, false );
	}
}

void FightClubTimeGaugeWnd::SetFaceChange( const ioHashString &rkName, const char *szTexPostfix, int iDuration )
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

void FightClubTimeGaugeWnd::SetCharChange( ioBaseChar *pChar )
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

void FightClubTimeGaugeWnd::SetHelmChange( ioBaseChar *pChar )
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

void FightClubTimeGaugeWnd::SetOwnerReserveChar( int iCharType )
{
	SAFEDELETE( m_pOwnerReserveChar );
	SAFEDELETE( m_pOwnerReserveSubChar );
	m_iOwnerReserveCharType = iCharType;

	m_pOwnerReserveChar = g_MyInfo.GetMySoldierIcon( m_iOwnerReserveCharType );
	m_pOwnerReserveSubChar = g_MyInfo.GetSoldierSubIcon( m_iOwnerReserveCharType );
	if( m_pOwnerReserveChar )
		m_pOwnerReserveChar->SetScale( 0.40f );
	if( m_pOwnerReserveSubChar )
		m_pOwnerReserveSubChar->SetScale( 0.40f );
}

int FightClubTimeGaugeWnd::GetMeSeqCount()
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

int FightClubTimeGaugeWnd::GetMeCheerType()
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

ioUIRenderImage *FightClubTimeGaugeWnd::GetComGradeRes( int iGrade )
{
	ComGradeMap::iterator iter = m_ComGradeList.find( iGrade );
	if( iter != m_ComGradeList.end() )
		return iter->second;

	return NULL;
}

void FightClubTimeGaugeWnd::OnProcess( float fTimePerSec )
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
	OnCheerTimeProcess( fTimePerSec );

	if( m_bUseFightNPC )
	{
		OnProcessFightNPC();
	}

	OnMouseProcess();
}

void FightClubTimeGaugeWnd::OnTimeProcess()
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

void FightClubTimeGaugeWnd::OnLeftHealthProcess( float fTimePerSec )
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
		enum { START_TIME = 5000, GAUGE_UP_TIME = 1500, };
		DWORD dwGapTime = FRAMEGETTIME() - m_dwLfetGaugeTime;
		if( dwGapTime < START_TIME )
			return;
		else if( dwGapTime < START_TIME + GAUGE_UP_TIME )
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

void FightClubTimeGaugeWnd::OnRightHealthProcess( float fTimePerSec )
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

void FightClubTimeGaugeWnd::OnLeftPowerProcess( float fTimePerSec )
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

void FightClubTimeGaugeWnd::OnRightPowerProcess( float fTimePerSec )
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

void FightClubTimeGaugeWnd::OnPowerGaugeProcess( float fTimePerSec )
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

void FightClubTimeGaugeWnd::OnCheerTimeProcess( float fTimePerSec )
{
	// 1초에 1회만 전송한다.
	if( m_dwCheerSendTime > 0 )
	{
		if( FRAMEGETTIME() - m_dwCheerSendTime > 1500 )
		{
			m_dwCheerSendTime = 0;

			// Send 
			SendCheerData();
		}
	}

	// UI 관련
	if( IsCheerPossible() )
	{
		int iLimitCurrTime = max( 0, m_iCheerRemainTime - (  m_iCheerFullTime - m_iCheerLimitTime ) );
		float fGaugeRate = (float)iLimitCurrTime / (float)m_iCheerLimitTime; 
		if( iLimitCurrTime > 5000 )
		{
			if( m_pCircleGaugeNor )
			{
				int iXPos = GetDerivedPosX() - 60;       
				int iYPos = GetDerivedPosY();


				m_pCircleGaugeNor->UpdateGauge( iXPos + (GetWidth() / 2) + 28, iYPos + 105, fGaugeRate );
			}
		}
		else if( m_pCircleGaugeDan )
		{
			int iXPos = GetDerivedPosX() - 60;       
			int iYPos = GetDerivedPosY();

			m_pCircleGaugeDan->UpdateGauge( iXPos + (GetWidth() / 2) + 28, iYPos + 105, fGaugeRate );
		}
	}
}

bool FightClubTimeGaugeWnd::IsMouseCheck( RECT &rkRect, ioHashString &rkName, const ioHashString &rkPrevName, const bool &rkPrevBtnDown )
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

void FightClubTimeGaugeWnd::OnMouseProcess()
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

void FightClubTimeGaugeWnd::UpdateTime( int iRemainTime, int iMaxTime )
{
	// 응원 시간	
	m_iCheerRemainTime = iRemainTime;
	m_iCheerFullTime   = iMaxTime;

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
	
	CheckCheerPossible();
}

void FightClubTimeGaugeWnd::OnRender()
{
	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() - 60;           // 준희씨 좌표가 윈도우 사이즈 880을 기반으로 잡아서 윈도우 사이즈를 760으로 고정하는 대신 -60을 해줌.
	int iYPos = GetDerivedPosY();

	OnTimeRender( iXPos, iYPos );
	OnChampionRender( iXPos, iYPos );

	if( m_bUseFightNPC )
	{
		OnChallengerNPCRender( iXPos, iYPos );
		OnFightNPCRender( iXPos, iYPos );
	}
	else
	{
		OnChallengerRender( iXPos, iYPos );
		OnSeqUserListRenter( iXPos, iYPos );
	}

	OnPowerGaugeLeftRender( iXPos, iYPos );
	OnPowerGaugeRightRender( iXPos, iYPos );

	if( !m_bUseFightNPC )
	{
		OnCheerRender( iXPos, iYPos );
		OnCheerTimeRender( iXPos, iYPos );
	}
}

void FightClubTimeGaugeWnd::OnTimeRender( int iXPos, int iYPos )
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
		pTimeBack->Render( iXPos + 367, iYPos + 15 );
	}

	if( pTimeNumText )
	{
		pTimeNumText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		pTimeNumText->RenderNum( iXPos + 440, iYPos + 26, m_iRemainSecond, -2.0f );
	}

	if( pTimeText )
	{
		pTimeText->Render( iXPos + 420, iYPos + 7 );
	}	
}

void FightClubTimeGaugeWnd::OnChampionRender( int iXPos, int iYPos )
{
	if( m_pLeftGaugeBack )
		m_pLeftGaugeBack->Render( iXPos + 22, iYPos + 36 );
	
	if( m_pLeftCharIcon )
	{
		m_pLeftCharIcon->Render( iXPos - 150, iYPos - 150 );
	}

	if( m_pChampionText )
		m_pChampionText->Render( iXPos + 129, iYPos + 21 );

	if( m_pLeftGaugeRed )
	{
		float fGaugeWidth = m_pLeftGaugeRed->GetWidth();
		float fGaugeValue = fGaugeWidth - ( fGaugeWidth * m_fLeftCurrentHPRate );  
		m_pLeftGaugeRed->RenderWidthCut( iXPos + 135, iYPos + 39, min( fGaugeWidth, max( 0, fGaugeValue ) ), fGaugeWidth );
	}

	if( m_pLeftGauge100 )
	{
		float fGaugeWidth = m_pLeftGauge100->GetWidth();
		float fGaugeValue = fGaugeWidth - ( fGaugeWidth * m_fLeftUserHPRate ); 
		m_pLeftGauge100->RenderWidthCut( iXPos + 135, iYPos + 39, min( fGaugeWidth, max( 0, fGaugeValue ) ), fGaugeWidth );
	}

	if( m_pLeftGaugeGlow )
	{
		float fGaugeWidth = m_pLeftGaugeGlow->GetWidth();
		float fGaugeValue = fGaugeWidth - ( fGaugeWidth * m_fLeftUserHPRate ); 
		m_pLeftGaugeGlow->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaValue );
		m_pLeftGaugeGlow->RenderWidthCut( iXPos + 135, iYPos + 39, min( fGaugeWidth, max( 0, fGaugeValue ) ), fGaugeWidth );
	}

	bool bMyChar = ( g_MyInfo.GetPublicID() == m_szLeftUserName );
	if( bMyChar )
	{
		if( m_pLeftBlueNameBar )
			m_pLeftBlueNameBar->Render( iXPos + 8, iYPos + 61 );

		g_LevelMgr.RenderGrade( iXPos + 46, iYPos + 62, g_MyInfo.GetGradeLevel(), TEAM_BLUE );

		float fNameWidth = 130.0f;
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetTextColor( 0, 150, 255 );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.PrintTextWidthCut( iXPos + 63, iYPos + 62, FONT_SIZE_14, fNameWidth, STR(1), m_szLeftUserName.c_str() );
		
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szLeftUserName, dwGuildIndex, dwGuildMark );
		if( dwGuildIndex != 0 )
		{
			g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 63 + min( fNameWidth, g_FontMgr.GetTextWidth( m_szLeftUserName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_14 ) ), iYPos + 65 );
		}
	}
	else
	{
		if( m_pLeftRedNameBar )
			m_pLeftRedNameBar->Render( iXPos + 8, iYPos + 61 );

		g_LevelMgr.RenderGrade( iXPos + 46, iYPos + 62, m_iLeftUserGrade, TEAM_RED );

		float fNameWidth = 130.0f;
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetTextColor( 255, 60, 0 );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.PrintTextWidthCut( iXPos + 63, iYPos + 62, FONT_SIZE_14, fNameWidth, STR(2), m_szLeftUserName.c_str() );

		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szLeftUserName, dwGuildIndex, dwGuildMark );
		if( dwGuildIndex != 0 )
		{
			g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 63 + min( fNameWidth, g_FontMgr.GetTextWidth( m_szLeftUserName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_14 ) ), iYPos + 65 );
		}
	}

	// 연승
	if( m_iChampionWinCount >= 2 )
	{
		if( m_pChampionWinEff )
		{
			float fScale = FLOAT08 * m_fChampionWinEffectRate;
			m_pChampionWinEff->SetScale( fScale + 0.7f );
			m_pChampionWinEff->SetAlpha( MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * m_fChampionWinEffectRate ) );
			m_pChampionWinEff->Render( iXPos + 82, iYPos + 106 );
		}

		if( m_pChampionWinBar )
		{	
			m_pChampionWinBar->Render( iXPos + 41, iYPos + 64 );
		}
		
		if( m_pChampionWinNum )
		{
			if( m_iChampionWinCount < 10 )
			{
				m_pChampionWinNum->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pChampionWinNum->SetVertFormatting( ioUIRenderImage::VF_CENTER );
				m_pChampionWinNum->RenderNum( iXPos + 83, iYPos + 106, m_iChampionWinCount, 0.0f, FLOAT1 );
			}
			else if( m_iChampionWinCount < 100 )
			{
				m_pChampionWinNum->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pChampionWinNum->SetVertFormatting( ioUIRenderImage::VF_CENTER );
				m_pChampionWinNum->RenderNum( iXPos + 83, iYPos + 106, m_iChampionWinCount, -2.0f, 0.75f );
			}
			else
			{
				m_pChampionWinNum->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pChampionWinNum->SetVertFormatting( ioUIRenderImage::VF_CENTER );
				m_pChampionWinNum->RenderNum( iXPos + 83, iYPos + 106, m_iChampionWinCount, -2.0f, 0.55f );
			}
		}
	}

	if( m_pLeftBackLine )
		m_pLeftBackLine->Render( iXPos + 123, iYPos + 35 );

	if( m_pLeftBackLineLight )
	{
		m_pLeftBackLineLight->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaValue );
		m_pLeftBackLineLight->Render( iXPos + 123, iYPos + 35 );
	}
}

void FightClubTimeGaugeWnd::OnChallengerRender( int iXPos, int iYPos )
{
	if( m_pRightGaugeBack )
		m_pRightGaugeBack->Render( iXPos + 476, iYPos + 36 );

	if( m_pRightCharIcon )
	{
		m_pRightCharIcon->Render( iXPos + 600, iYPos - 150 );
	}

	if( m_pChallengerText )
		m_pChallengerText->Render( iXPos + 656, iYPos + 21 );

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

	bool bMyChar = ( g_MyInfo.GetPublicID() == m_szRightUserName );
	if( bMyChar )
	{
		if( m_pRightBlueNameBar )
			m_pRightBlueNameBar->Render( iXPos + 642, iYPos + 61 );
		
		//길드 마크
		int iStartX = 834;
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szRightUserName, dwGuildIndex, dwGuildMark );
		if( dwGuildIndex != 0 )
		{
			g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + iStartX - 16, iYPos + 65 );
			iStartX -= 16;
		}

		float fNameWidth = 130.0f;
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetTextColor( 0, 150, 255 );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.PrintTextWidthCut( iXPos + iStartX, iYPos + 62, FONT_SIZE_14, fNameWidth, STR(1), m_szRightUserViewName.c_str() );
		iStartX -= min( fNameWidth, g_FontMgr.GetTextWidth( m_szRightUserViewName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_14 ) ) + 17;

		g_LevelMgr.RenderGrade( iXPos + iStartX, iYPos + 62, g_MyInfo.GetGradeLevel(), TEAM_BLUE );
	}
	else
	{
		if( m_pRightRedNameBar )
			m_pRightRedNameBar->Render( iXPos + 642, iYPos + 61 );

		//길드 마크
		int iStartX = 834;
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szRightUserName, dwGuildIndex, dwGuildMark );
		if( dwGuildIndex != 0 )
		{
			g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + iStartX - 16, iYPos + 65 );
			iStartX -= 16;
		}

		float fNameWidth = 130.0f;
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetTextColor( 255, 60, 0 );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.PrintTextWidthCut( iXPos + iStartX, iYPos + 62, FONT_SIZE_14, fNameWidth, STR(2), m_szRightUserViewName.c_str() );
		iStartX -= min( fNameWidth, g_FontMgr.GetTextWidth( m_szRightUserViewName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_14 ) ) + 17;

		g_LevelMgr.RenderGrade( iXPos + iStartX, iYPos + 62, m_iRightUserGrade, TEAM_RED );
	}

	if( m_pRightBackLine )
		m_pRightBackLine->Render( iXPos + 477, iYPos + 35 );

	if( m_pRightBackLineLight )
	{
		m_pRightBackLineLight->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaValue );
		m_pRightBackLineLight->Render( iXPos + 477, iYPos + 35 );
	}
}

void FightClubTimeGaugeWnd::OnSeqUserListRenter( int iXPos, int iYPos )
{
	if( m_CharSeqData.empty() ) return;

	int iCount = 0;
	CharSeqDataVec::iterator iter = m_CharSeqData.begin();
	for(;iter != m_CharSeqData.end();iter++,iCount++)
	{
		CharSeqData &rkData = *iter;
		if( rkData.m_kName == g_MyInfo.GetPublicID() )
		{
			if( m_pSeqMeBar )
				m_pSeqMeBar->Render( iXPos + 708, iYPos + 89 + ( iCount * 16 ) );			
			
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

			if( iCount == 0 && m_pNextMeBattle )
			{
				m_pNextMeBattle->Render( iXPos + iStartX, iYPos + 87 );
				if( m_bTimeAlert )
					m_pNextMeBattle->Render( iXPos + iStartX, iYPos + 87, UI_RENDER_SCREEN );
				iStartX -= 14;

				if( m_iOwnerReserveCharType != -1 )
				{
					// 다음 출전 용병
					if( m_pOwnerReserveChar )
					{
						m_pOwnerReserveChar->Render( iXPos + iStartX, iYPos + 96, UI_RENDER_NORMAL, TFO_BILINEAR );
						if( m_bTimeAlert )
							m_pOwnerReserveChar->Render( iXPos + iStartX, iYPos + 96, UI_RENDER_SCREEN, TFO_BILINEAR );
					}
					if( m_pOwnerReserveSubChar )
					{
						m_pOwnerReserveSubChar->Render( iXPos + iStartX, iYPos + 96, UI_RENDER_NORMAL, TFO_BILINEAR );
						if( m_bTimeAlert )
							m_pOwnerReserveSubChar->Render( iXPos + iStartX, iYPos + 96, UI_RENDER_SCREEN, TFO_BILINEAR );
					}
				}
			}
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

void FightClubTimeGaugeWnd::OnPowerGaugeLeftRender( int iXPos, int iYPos )
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

void FightClubTimeGaugeWnd::OnPowerGaugeRightRender( int iXPos, int iYPos )
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

void FightClubTimeGaugeWnd::OnCheerRender( int iXPos, int iYPos )
{
	if( m_pCheerBack )
	{
		m_pCheerBack->Render( iXPos + 388, iYPos + 62 );
	}

	if( m_dwCheerTabID == ID_CHAMPION_BTN || m_szLeftUserName == g_MyInfo.GetPublicID() )
	{
		if( m_dwCheerTabID != 0 )
		{
			if( m_pChampCheerPoint )
				m_pChampCheerPoint->Render( iXPos + 387, iYPos + 71 );
		}

		if( m_pChampCheerText )
			m_pChampCheerText->Render( iXPos + 424, iYPos + 73 );
	}
	else if( m_dwCheerTabID == ID_CHALLENGER_BTN || m_szRightUserName == g_MyInfo.GetPublicID() )
	{
		if( m_dwCheerTabID != 0 )
		{
			if( m_pChallCheerPoint )
				m_pChallCheerPoint->Render( iXPos + 466, iYPos + 71 );
		}

		if( m_pChallCheerText )
			m_pChallCheerText->Render( iXPos + 424, iYPos + 73 );
	}
	else
	{
		if( m_pChampCheerText )
			m_pChampCheerText->Render( iXPos + 424, iYPos + 73 );
	}

	if( m_pChampCheerNumber )
	{
		m_pChampCheerNumber->RenderNum( iXPos + 408, iYPos + 72, GetCheerCount( FIGHTCLUB_CHAMPION ) );
	}

	if( m_pChallCheerNumber )
	{
		m_pChallCheerNumber->RenderNum( iXPos + 453, iYPos + 72, GetCheerCount( FIGHTCLUB_CHALLENGER ) );
	}
}

void FightClubTimeGaugeWnd::OnCheerTimeRender( int iXPos, int iYPos )
{
	if( !IsCheerPossible() ) return;
	if( m_pCircleGaugeNor == NULL ) return;
	if( m_pCircleGaugeDan == NULL ) return;

	if( m_pCheerTimeBack )
		m_pCheerTimeBack->Render( iXPos + 421, iYPos + 118 );
	
	int iLimitCurrTime = max( 0, m_iCheerRemainTime - (  m_iCheerFullTime - m_iCheerLimitTime ) );
	if( iLimitCurrTime > 5000 )
	{
		if( m_pCircleGaugeNor->IsNeedRender() )
		{
			// 소멸은 g_UIRenderer가 해준다.
			ioCircleGaugeQuad *pQuad = new ioCircleGaugeQuad( m_pCircleGaugeNor );
			if( pQuad )
			{
				g_UIRenderer.AddUIQuad( pQuad );
			}
		}
	}
	else if( m_pCircleGaugeDan->IsNeedRender() )
	{
		// 소멸은 g_UIRenderer가 해준다.
		ioCircleGaugeQuad *pQuad = new ioCircleGaugeQuad( m_pCircleGaugeDan );
		if( pQuad )
		{
			g_UIRenderer.AddUIQuad( pQuad );
		}
	}
}

void FightClubTimeGaugeWnd::iwm_create()
{
	m_bUseFightNPC = false;

	int iXPos = g_GUIMgr.GUIPosLoadInt( "FightClubTimeGaugeWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "FightClubTimeGaugeWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

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

	m_pCircleGaugeNor = new io2DCircleGauge;
	if( m_pCircleGaugeNor)                
		m_pCircleGaugeNor->InitResource( "chapion_cheer_time_nor.dds", 360, 0, 0, io2DCircleGauge::ST_6OCLOCK );


	m_pCircleGaugeDan = new io2DCircleGauge;
	if( m_pCircleGaugeDan )
		m_pCircleGaugeDan->InitResource( "chapion_cheer_time_death.dds", 360, 0, 0, io2DCircleGauge::ST_6OCLOCK );

	HideChildWnd( ID_ENABLE_CHALLENGER );
	HideChildWnd( ID_NO_CHALLENGER );
}

void FightClubTimeGaugeWnd::iwm_show()
{
	m_iAcceptX = 0;
	m_AcceptPointState = APS_RIGHT;
	m_dwAcceptPointCheckTime = FRAMEGETTIME();
}

void FightClubTimeGaugeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHAMPION_BTN:
	case ID_CHALLENGER_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			CheckCheerButton( dwID );
		}
		break;
	case ID_ENABLE_CHALLENGER:
	case ID_NO_CHALLENGER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				g_BattleRoomMgr.SetNoChallenger( !g_BattleRoomMgr.IsNoChallenger() );
				g_BattleRoomMgr.SendBattleRoomOption();
			}
		}
		break;
	}
}

int FightClubTimeGaugeWnd::GetCheerCount( int iCheerType )
{
	int iReturnCount = 0;
	CharSeqDataVec::iterator iter = m_CharSeqData.begin();
	for(;iter != m_CharSeqData.end();iter++ )
	{
		CharSeqData &rkData = *iter;
		if( rkData.m_kName == g_MyInfo.GetPublicID() )
		{
			if( (int)m_dwCheerTabID == iCheerType )
				iReturnCount++;                      // 내꺼는 현재 클릭기준으로 바로 적용
		}
		else
		{
			if( rkData.m_iCheer == iCheerType )
				iReturnCount++;
		}
	}
	return iReturnCount;
}

void FightClubTimeGaugeWnd::SendCheerData()
{
	// Send 
	if( m_dwCheerTabID != GetMeCheerType() )
	{
		SP2Packet kPacket( CTPK_FIGHTCLUB_CHEER );
		kPacket << m_dwCheerTabID;
		TCPNetwork::SendToServer( kPacket );
	}
}

void FightClubTimeGaugeWnd::CheckCheerButton( DWORD dwID )
{
	bool bFirstSend = false;
	if( m_dwCheerTabID == 0 )
		bFirstSend = true;

	m_dwCheerTabID = dwID;
	CheckRadioButton( ID_CHAMPION_BTN, ID_CHALLENGER_BTN, m_dwCheerTabID );

	if( bFirstSend )
	{
		SendCheerData();       // 첫 응원은 바로 보낸다.
	}
	else
	{
		if( m_dwCheerSendTime == 0 )
			m_dwCheerSendTime = FRAMEGETTIME();
	}
}

bool FightClubTimeGaugeWnd::IsCheerPossible()
{
	if( GetMeSeqCount() == 0 )        // 파이터이거나 관전자는 SeqCount가 0 이다.
	{
		return false;
	}

	if( m_bUseFightNPC )
		return false;                  // AI 모드는 응원 없음.
	
	if( m_iCheerRemainTime <= 0 || 
		m_iCheerFullTime <= 0 || 
		m_iCheerLimitTime <= 0 || 
		m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND )
	{
		return false;
	}

	int iLimitCurrTime = max( 0, m_iCheerRemainTime - (  m_iCheerFullTime - m_iCheerLimitTime ) );
	if( iLimitCurrTime <= 0 ) return false;

	return true;
}

void FightClubTimeGaugeWnd::CheckCheerPossible()
{
	if( !m_bCheerPossible )
	{
		if( IsCheerPossible() )
		{
			m_bCheerPossible = true;
			ShowChildWnd( ID_CHAMPION_BTN );
			ShowChildWnd( ID_CHALLENGER_BTN );
			CheckCheerButton( 0 );       // 선택 안함 
		}
	}
	else
	{
		// 응원 불가 체크
		if( !IsCheerPossible() )
		{
			m_bCheerPossible = false;
			HideChildWnd( ID_CHAMPION_BTN );
			HideChildWnd( ID_CHALLENGER_BTN );

			if( m_dwCheerSendTime > 0 )
			{
				m_dwCheerSendTime = 0;

				SendCheerData();
			}
		}
	}
}

void FightClubTimeGaugeWnd::SetBlindOffSet( int iYOffSet )
{
	m_iBlindOffSetY = iYOffSet;
}

void FightClubTimeGaugeWnd::InitAniState()
{
	m_ShowAniState.SetCurState( SAT_NONE );
}

void FightClubTimeGaugeWnd::InitShowAniState()
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

void FightClubTimeGaugeWnd::UpdateAniState()
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

void FightClubTimeGaugeWnd::ChangeShowAniState( ShowAniType eShowAniType )
{
	m_ShowAniState.ChangeState( (int) eShowAniType );
}

bool FightClubTimeGaugeWnd::UpdateShowAni()
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

int FightClubTimeGaugeWnd::GetCurShowAniState()
{
	return m_ShowAniState.GetCurState();
}

void FightClubTimeGaugeWnd::UpdateDefaultPos()
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

void FightClubTimeGaugeWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
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

void FightClubTimeGaugeWnd::ParseExtraInfo( ioXMLElement &xElement )
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

void FightClubTimeGaugeWnd::SetUseFightNPC( bool bUse, int iRoundReadyCharCount )
{
	m_bUseFightNPC = bUse;

	if( m_bUseFightNPC )
	{
		if( g_BattleRoomMgr.IsNoChallenger() )
		{
			ShowChildWnd( ID_NO_CHALLENGER );
			HideChildWnd( ID_ENABLE_CHALLENGER );
		}
		else
		{
			ShowChildWnd( ID_ENABLE_CHALLENGER );
			HideChildWnd( ID_NO_CHALLENGER );
		}
	}
	else
	{
		HideChildWnd( ID_ENABLE_CHALLENGER );
		HideChildWnd( ID_NO_CHALLENGER );
	}
}

void FightClubTimeGaugeWnd::OnProcessFightNPC()
{
	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND )
	{
		HideChildWnd( ID_ENABLE_CHALLENGER );
		HideChildWnd( ID_NO_CHALLENGER );
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

	if( g_BattleRoomMgr.IsNoChallenger() )
	{
		ShowChildWnd( ID_NO_CHALLENGER );
		HideChildWnd( ID_ENABLE_CHALLENGER );
	}
	else
	{
		ShowChildWnd( ID_ENABLE_CHALLENGER );
		HideChildWnd( ID_NO_CHALLENGER );
	}
}

void FightClubTimeGaugeWnd::OnChallengerNPCRender( int iXPos, int iYPos )
{
	if( m_pRightGaugeBack )
		m_pRightGaugeBack->Render( iXPos + 476, iYPos + 36 );

	if( m_pRightCharIcon )
	{
		m_pRightCharIcon->Render( iXPos + 600, iYPos - 150 );
	}

	int iNewXPos = iXPos+60-20;

	if( m_pFightNPCRoundText )
		m_pFightNPCRoundText->Render( iNewXPos+643, iYPos+21 );

	if( m_pFightNPCRoundNum )
	{
		m_pFightNPCRoundNum->RenderNum( iNewXPos+697, iYPos+19, m_iCurFightNPCStage );
	}

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

void FightClubTimeGaugeWnd::OnFightNPCRender( int iXPos, int iYPos )
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

	// 승 표시
	if( m_pFightNpcClearBack && m_pFightNpcClearWin )
	{
		int iStartX = 350;    
		for(int i = 0;i < MAX_FIGHT_NPC_WIN_COUNT;i++)
		{
			m_pFightNpcClearBack->Render( iXPos + iStartX, iYPos + 79 );
			if( i < m_iCurFightNPCStage - 1 )
				m_pFightNpcClearWin->Render( iXPos + iStartX - 2, iYPos + 79 );

			iStartX += 37;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
FightClubReadyWnd::FightClubReadyWnd()
{
	m_pBackFrm		= NULL;
	m_pBackFrmLine	= NULL;
	m_pReadyText    = NULL;
	m_pReadyTextEffect = NULL;

	m_pBlackBack	   = NULL;
	m_pGoBackEffect	   = NULL;
	m_pGoText		   = NULL;
	m_pGoTextEffect	   = NULL;

	m_pRoundText = NULL;
	m_pRoundNum = NULL;
	m_pFinalText = NULL;

	m_pFightText = NULL;

	m_pPlayStage = NULL;

	InitValue();
}

FightClubReadyWnd::~FightClubReadyWnd()
{
	SAFEDELETE( m_pBackFrm );
	SAFEDELETE( m_pBackFrmLine );
	SAFEDELETE( m_pReadyText );
	SAFEDELETE( m_pReadyTextEffect );

	SAFEDELETE( m_pBlackBack );
	SAFEDELETE( m_pGoBackEffect );
	SAFEDELETE( m_pGoText );
	SAFEDELETE( m_pGoTextEffect );

	SAFEDELETE( m_pRoundText );
	SAFEDELETE( m_pRoundNum );
	SAFEDELETE( m_pFinalText );
	SAFEDELETE( m_pFightText );
}

void FightClubReadyWnd::InitValue()
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwBackReadyState = BR_BACK_OPEN;
	m_fBackReadyRate   = 0.0f;
	m_dwBackReadyTime  = dwCurTime;

	m_dwGoState		= GA_DELAY;
	m_fGoAniRate	= FLOAT1;
	m_dwGoAniTime	= dwCurTime;

	m_fFightAlphaRate = 0.0f;
	m_fFightScaleRate = 0.0f;
	m_dwFightAniTime = dwCurTime;

	// 이전에 페이드 아웃된 이미지 원복
	if( m_pBackFrm )
		m_pBackFrm->SetAlpha( MAX_ALPHA_RATE );
	if( m_pBackFrmLine )
		m_pBackFrmLine->SetAlpha( MAX_ALPHA_RATE );
	if( m_pReadyText )
		m_pReadyText->SetAlpha( MAX_ALPHA_RATE );
	if( m_pReadyTextEffect )
		m_pReadyTextEffect->SetAlpha( MAX_ALPHA_RATE );
	if( m_pBlackBack )
		m_pBlackBack->SetAlpha( MAX_ALPHA_RATE );
	if( m_pGoBackEffect )
		m_pGoBackEffect->SetAlpha( MAX_ALPHA_RATE );
	if( m_pGoText )
		m_pGoText->SetAlpha( MAX_ALPHA_RATE );
	if( m_pGoTextEffect )
		m_pGoTextEffect->SetAlpha( MAX_ALPHA_RATE );

	if( m_pRoundText )
		m_pRoundText->SetAlpha( MAX_ALPHA_RATE );
	if( m_pRoundNum )
		m_pRoundNum->SetAlpha( MAX_ALPHA_RATE );
	if( m_pFinalText )
		m_pFinalText->SetAlpha( MAX_ALPHA_RATE );
	if( m_pFightText )
		m_pFightText->SetAlpha( MAX_ALPHA_RATE );

	m_bUseFightNPC = false;

	m_iMaxStage = 0;
	m_iCurStage = 0;
}

void FightClubReadyWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void FightClubReadyWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BackFrmLine" )
	{
		SAFEDELETE( m_pBackFrmLine );
		m_pBackFrmLine = pImage;
		if( m_pBackFrmLine )
		{
			m_pBackFrmLine->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		}
	}
	else if( szType == "ReadyText" )
	{
		SAFEDELETE( m_pReadyText );
		m_pReadyText = pImage;
		if( m_pReadyText )
		{
			m_pReadyText->SetVertFormatting( ioUIRenderImage::VF_CENTER );
			m_pReadyText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		}
	}
	else if( szType == "ReadyTextEffect" )
	{
		SAFEDELETE( m_pReadyTextEffect );
		m_pReadyTextEffect = pImage;
		if( m_pReadyTextEffect )
		{
			m_pReadyTextEffect->SetVertFormatting( ioUIRenderImage::VF_CENTER );
			m_pReadyTextEffect->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		}
	}
	else if( szType == "BlackBack" )
	{
		SAFEDELETE( m_pBlackBack );
		m_pBlackBack = pImage;
		if( m_pBlackBack )
		{
			m_pBlackBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pBlackBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		}
	}
	else if( szType == "GoBackEffect" )
	{
		SAFEDELETE( m_pGoBackEffect );
		m_pGoBackEffect = pImage;
		if( m_pGoBackEffect )
		{
			m_pGoBackEffect->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pGoBackEffect->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		}
	}
	else if( szType == "GoText" )
	{
		SAFEDELETE( m_pGoText );
		m_pGoText = pImage;
		if( m_pGoText )
		{
			m_pGoText->SetVertFormatting( ioUIRenderImage::VF_CENTER );
			m_pGoText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		}
	}
	else if( szType == "GoTextEffect" )
	{
		SAFEDELETE( m_pGoTextEffect );
		m_pGoTextEffect = pImage;
		if( m_pGoTextEffect )
		{
			m_pGoTextEffect->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pGoTextEffect->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		}
	}
	else if( szType == "RoundText" )
	{
		SAFEDELETE( m_pRoundText );
		m_pRoundText = pImage;

		if( m_pRoundText )
		{
			m_pRoundText->SetVertFormatting( ioUIRenderImage::VF_CENTER );
		}
	}
	else if( szType == "RoundNum" )
	{
		SAFEDELETE( m_pRoundNum );
		m_pRoundNum = pImage;

		if( m_pRoundNum )
		{
			m_pRoundNum->SetVertFormatting( ioUIRenderImage::VF_CENTER );
		}
	}
	else if( szType == "FinalText" )
	{
		SAFEDELETE( m_pFinalText );
		m_pFinalText = pImage;

		if( m_pFinalText )
		{
			m_pFinalText->SetVertFormatting( ioUIRenderImage::VF_CENTER );
		}
	}
	else if( szType == "FightText" )
	{
		SAFEDELETE( m_pFightText );
		m_pFightText = pImage;

		if( m_pFightText )
		{
			m_pFightText->SetVertFormatting( ioUIRenderImage::VF_CENTER );
			m_pFightText->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		}
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void FightClubReadyWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szReadySnd = xElement.GetStringAttribute_e( "ReadySnd" );
	m_szGoSnd    = xElement.GetStringAttribute_e( "GoSnd" );

	ioHashString szSndName;

	m_vRoundSnd.clear();

	int iRoundSndCnt = xElement.GetIntAttribute_e( "RoundSndCnt" );
	for( int i=0; i < iRoundSndCnt; ++i )
	{
		char szBuf[MAX_PATH] = "";

		StringCbPrintf( szBuf, sizeof( szBuf ), "RoundSnd%d", i+1 );
		szSndName = xElement.GetStringAttribute( szBuf );

		m_vRoundSnd.push_back( szSndName );
	}

	m_szFinalRoundSnd = xElement.GetStringAttribute_e( "FinalRoundSnd" );
	m_szFightSnd = xElement.GetStringAttribute_e( "FightSnd" );
}

void FightClubReadyWnd::iwm_show()
{
	InitValue();
}

void FightClubReadyWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void FightClubReadyWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_bUseFightNPC )
	{
		OnProcessRound();
		OnProcessFight();
	}
	else
	{
		OnProcessGo();
		OnProcessBackReady();
	}
}

void FightClubReadyWnd::OnProcessBackReady()
{
	if( m_dwBackReadyState == BR_HIDE )
		return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwBackReadyTime;
	switch( m_dwBackReadyState )
	{
	case BR_BACK_OPEN:
		if( dwGapTime < BR_BACK_OPEN_TIME )
		{
			m_fBackReadyRate = (float)dwGapTime / BR_BACK_OPEN_TIME;
		}
		else
		{
			m_dwBackReadyState = BR_SLIDE;
			m_dwBackReadyTime  = FRAMEGETTIME();
			m_fBackReadyRate   = 0.0f;

			if( !m_szReadySnd.IsEmpty() )
			{
				g_SoundMgr.PlaySound( m_szReadySnd );
			}
		}
		break;
	case BR_SLIDE:
		if( dwGapTime <= BR_SLIDE_TIME )
		{
			m_fBackReadyRate = (float)dwGapTime / BR_SLIDE_TIME;
		}
		else
		{
			m_dwBackReadyState = BR_EFFECT_SLIDE;
			m_dwBackReadyTime  = FRAMEGETTIME();
			m_fBackReadyRate   = FLOAT1;
		}
		break;
	case BR_EFFECT_SLIDE:
		if( dwGapTime <= BR_EFFECT_SLIDE_TIME )
		{
			m_fBackReadyRate = FLOAT1 - sinf( ( D3DX_PI / 2 ) * ( (float)dwGapTime / BR_EFFECT_SLIDE_TIME ) );
		}
		else
		{
			m_dwBackReadyState = BR_DELAY;
			m_dwBackReadyTime  = FRAMEGETTIME();
			m_fBackReadyRate   = 0.0f;
		}
		break;
	case BR_DELAY:
		if( dwGapTime >= BR_DELAY_TIME )
		{
			m_dwBackReadyState = BR_FADEOUT;
			m_dwBackReadyTime  = FRAMEGETTIME();
			m_fBackReadyRate   = FLOAT1;
		}
		break;
	case BR_FADEOUT:
		if( dwGapTime <= BR_FADEOUT_TIME )
		{
			m_fBackReadyRate = FLOAT1 - ( (float)dwGapTime / BR_FADEOUT_TIME );
		}
		else
		{
			m_dwBackReadyState = BR_HIDE;

			// Go Ani Start
			m_dwGoState		= GA_START;
			m_fGoAniRate	= 0.0f;
			m_dwGoAniTime	= FRAMEGETTIME();

			if( !m_szGoSnd.IsEmpty() )
			{
				g_SoundMgr.PlaySound( m_szGoSnd );
			}
		}
		break;
	}
}

void FightClubReadyWnd::OnProcessGo()
{
	if( m_dwGoState == GA_DELAY ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwGoAniTime;
	switch( m_dwGoState )
	{
	case GA_START:
		if( dwGapTime <= GA_START_TIME )
		{
			m_fGoAniRate = sinf( ( D3DX_PI / 2 ) * ( (float)dwGapTime / GA_START_TIME ) );
		}
		else
		{
			m_dwGoState		= GA_EFFECT_FADEOUT;
			m_dwGoAniTime	= FRAMEGETTIME();
			m_fGoAniRate    = FLOAT1;
		}
		break;
	case GA_EFFECT_FADEOUT:
		if( dwGapTime <= GA_EFFECT_FADEOUT_TIME )
		{
			m_fGoAniRate = FLOAT1 - ( (float)dwGapTime / GA_EFFECT_FADEOUT_TIME );
		}
		else
		{
			m_dwGoState		= GA_HALF_DELAY;
			m_dwGoAniTime	= FRAMEGETTIME();
			m_fGoAniRate    = FLOAT1;
		}
		break;
	case GA_HALF_DELAY:
		if( dwGapTime >= GA_HALF_DELAY_TIME )
		{
			m_dwGoState		= GA_FADEOUT;
			m_dwGoAniTime	= FRAMEGETTIME();
			m_fGoAniRate    = FLOAT1;
		}
		break;
	case GA_FADEOUT:
		if( dwGapTime <= GA_FADEOUT_TIME )
		{
			m_fGoAniRate = FLOAT1 -( (float)dwGapTime / GA_FADEOUT_TIME );
		}
		else
		{
			m_dwGoState = GA_END;
			HideWnd();
		}
		break;
	}
}

void FightClubReadyWnd::OnProcessRound()
{
	if( m_dwBackReadyState == BR_HIDE )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwBackReadyTime;

	switch( m_dwBackReadyState )
	{
	case BR_BACK_OPEN:
		if( dwGapTime < BR_BACK_OPEN_TIME )
		{
			m_fBackReadyRate = (float)dwGapTime / BR_BACK_OPEN_TIME;
		}
		else
		{
			m_dwBackReadyState = BR_SLIDE;
			m_dwBackReadyTime  = dwCurTime;
			m_fBackReadyRate   = 0.0f;

			int iCurArray = m_iCurStage - 1;
			if( COMPARE( iCurArray, 0, (int)m_vRoundSnd.size() ) )
			{
				if( !m_vRoundSnd[iCurArray].IsEmpty() )
					g_SoundMgr.PlaySound( m_vRoundSnd[iCurArray] );
			}
			else
			{
				if( !m_szFinalRoundSnd.IsEmpty() )
					g_SoundMgr.PlaySound( m_szFinalRoundSnd );
			}
		}
		break;
	case BR_SLIDE:
		if( dwGapTime <= BR_SLIDE_TIME )
		{
			m_fBackReadyRate = (float)dwGapTime / BR_SLIDE_TIME;
		}
		else
		{
			m_dwBackReadyState = BR_EFFECT_SLIDE;
			m_dwBackReadyTime  = dwCurTime;
			m_fBackReadyRate   = FLOAT1;
		}
		break;
	case BR_EFFECT_SLIDE:
		if( dwGapTime <= BR_EFFECT_SLIDE_TIME )
		{
			m_fBackReadyRate = FLOAT1 - sinf( ( D3DX_PI / 2 ) * ( (float)dwGapTime / BR_EFFECT_SLIDE_TIME ) );
		}
		else
		{
			m_dwBackReadyState = BR_DELAY;
			m_dwBackReadyTime  = dwCurTime;
			m_fBackReadyRate   = 0.0f;
		}
		break;
	case BR_DELAY:
		if( dwGapTime >= BR_DELAY_TIME )
		{
			m_dwBackReadyState = BR_FADEOUT;
			m_dwBackReadyTime  = dwCurTime;
			m_fBackReadyRate   = FLOAT1;
		}
		break;
	case BR_FADEOUT:
		if( dwGapTime <= BR_FADEOUT_TIME )
		{
			m_fBackReadyRate = FLOAT1 - ( (float)dwGapTime / BR_FADEOUT_TIME );
		}
		else
		{
			m_dwBackReadyState = BR_HIDE;

			// Fight Ani Start
			m_dwGoState		= GA_START;
			m_fGoAniRate	= 0.0f;
			m_dwGoAniTime	= dwCurTime;
			m_dwFightAniTime = dwCurTime;

			if( !m_szFightSnd.IsEmpty() )
				g_SoundMgr.PlaySound( m_szFightSnd );
		}
		break;
	}
}

void FightClubReadyWnd::OnProcessFight()
{
	if( m_dwGoState == GA_DELAY ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwGoAniTime;

	switch( m_dwGoState )
	{
	case GA_START:
		if( dwGapTime <= GA_START_TIME )
		{
			m_fGoAniRate = sinf( ( D3DX_PI / 2 ) * ( (float)dwGapTime / GA_START_TIME ) );
		}
		else
		{
			m_dwGoState		= GA_EFFECT_FADEOUT;
			m_dwGoAniTime	= dwCurTime;
			m_fGoAniRate    = FLOAT1;
		}
		break;
	case GA_EFFECT_FADEOUT:
		if( dwGapTime <= GA_EFFECT_FADEOUT_TIME )
		{
			m_fGoAniRate = FLOAT1 - ( (float)dwGapTime / GA_EFFECT_FADEOUT_TIME );
		}
		else
		{
			m_dwGoState		= GA_HALF_DELAY;
			m_dwGoAniTime	= dwCurTime;
			m_fGoAniRate    = FLOAT1;
		}
		break;
	case GA_HALF_DELAY:
		if( dwGapTime >= GA_HALF_DELAY_TIME )
		{
			m_dwGoState		= GA_FADEOUT;
			m_dwGoAniTime	= dwCurTime;
			m_fGoAniRate    = FLOAT1;
		}
		break;
	case GA_FADEOUT:
		if( dwGapTime <= GA_FADEOUT_TIME )
		{
			m_fGoAniRate = FLOAT1 -( (float)dwGapTime / GA_FADEOUT_TIME );
		}
		else
		{
			m_dwGoState = GA_END;
			HideWnd();
		}
		break;
	}

	// 추가 출력 관련
	m_fFightAlphaRate = 0.0f;
	m_fFightScaleRate = 0.0f;

	DWORD dwFightGapTime = dwCurTime - m_dwFightAniTime;

	float fTimeRate = (float)dwFightGapTime / FLOAT1000;
	float fCurRate = 0.0f;
	if( fTimeRate < FLOAT1 )
	{
		fCurRate = sinf( ( D3DX_PI / 2 ) * fTimeRate );

		m_fFightAlphaRate = FLOAT1 - fCurRate;
		m_fFightScaleRate = FLOAT1 + (1.5f * fCurRate);
	}
}

void FightClubReadyWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderBackFrm( iXPos, iYPos );

	if( m_bUseFightNPC )
	{
		OnRenderRound( iXPos, iYPos );
		OnRenderFight( iXPos, iYPos );
	}
	else
	{
		OnRenderReady( iXPos, iYPos );
		OnRenderGO( iXPos, iYPos );
	}
}

void FightClubReadyWnd::OnRenderBackFrm( int iXPos, int iYPos )
{
	if( m_dwBackReadyState == BR_HIDE ) return;
	if( m_pBackFrm == NULL || m_pBackFrmLine == NULL ) return;

	iYPos = (float)Setting::Height() * 0.45f;

	if( m_dwBackReadyState == BR_FADEOUT )
	{
		m_pBackFrm->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
		m_pBackFrmLine->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
	}

	enum{ START_H=29, ADD_H=83, };
	if( m_dwBackReadyState == BR_BACK_OPEN )
		m_pBackFrm->SetSize( Setting::Width(), START_H + ( (float)ADD_H * m_fBackReadyRate )  );
	else
		m_pBackFrm->SetSize( Setting::Width(), START_H + ADD_H );	
	m_pBackFrm->Render( iXPos, iYPos - ( (float)m_pBackFrm->GetHeight() * FLOAT05 ) );

	m_pBackFrmLine->Render( Setting::Width() / 2, iYPos - ( (float)m_pBackFrm->GetHeight() * FLOAT05 ) - 1 );
	m_pBackFrmLine->Render( Setting::Width() / 2, ( iYPos - ( (float)m_pBackFrm->GetHeight() * FLOAT05 ) ) + m_pBackFrm->GetHeight() - 9 );
}

void FightClubReadyWnd::OnRenderReady( int iXPos, int iYPos )
{
	if( m_pReadyText == NULL || m_pReadyTextEffect == NULL ) return;
	if( m_dwBackReadyState == BR_BACK_OPEN || m_dwBackReadyState == BR_HIDE ) return;

	enum{ READY_X=112, READY_EFFECT_X=42,};
	iXPos = ( (float)Setting::Width() / 2 ) - READY_X;
	iYPos = (float)Setting::Height() * 0.45f;

	if( m_dwBackReadyState == BR_SLIDE )
	{
		m_pReadyText->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
		m_pReadyText->Render( iXPos + ( READY_X * m_fBackReadyRate ), iYPos );
	}
	else
	{
		if( m_dwBackReadyState == BR_FADEOUT )
			m_pReadyText->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
		else
			m_pReadyText->SetAlpha( MAX_ALPHA_RATE );
		m_pReadyText->Render( iXPos + READY_X, iYPos );
	}

	if( m_dwBackReadyState == BR_EFFECT_SLIDE )
	{
		m_pReadyTextEffect->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
		m_pReadyTextEffect->Render( ( iXPos + READY_X + READY_EFFECT_X ) - ( READY_EFFECT_X * m_fBackReadyRate ), iYPos );
	}
}

void FightClubReadyWnd::OnRenderGO( int iXPos, int iYPos )
{
	if( m_dwGoState == GA_START )
	{
		if( m_pBlackBack )
		{
			float fAlphaRate = 0.6f * ( FLOAT1 - m_fGoAniRate );
			m_pBlackBack->SetAlpha( (float)MAX_ALPHA_RATE * fAlphaRate );
			m_pBlackBack->SetSize( Setting::Width(), Setting::Height() );
			m_pBlackBack->Render( iXPos, iYPos );
		}
		
		iXPos = ( (float)Setting::Width() / 2 );
		iYPos = (float)Setting::Height() * 0.45f;

		if( m_pGoBackEffect )
		{
			float fHeight = 77.0f + ( 231.0f * m_fGoAniRate );
			m_pGoBackEffect->SetAlpha( (float)MAX_ALPHA_RATE * ( FLOAT1 - m_fGoAniRate ) );
			m_pGoBackEffect->SetSize( Setting::Width(), fHeight );
			m_pGoBackEffect->Render( 0, iYPos - ( fHeight / 2 ) );
		}

		if( m_pGoText )
		{
			m_pGoText->Render( iXPos, iYPos );
		}

		if( m_pGoTextEffect )
		{			
			m_pGoTextEffect->SetScale( FLOAT1 );
			m_pGoTextEffect->SetAlpha( MAX_ALPHA_RATE );
			m_pGoTextEffect->Render( iXPos, iYPos - 2, UI_RENDER_NORMAL, TFO_BILINEAR ); 

			float fScale = 3.5f * m_fGoAniRate;
			m_pGoTextEffect->SetScale( FLOAT1 + fScale );
			m_pGoTextEffect->SetAlpha( MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * m_fGoAniRate ) );
			m_pGoTextEffect->Render( iXPos, iYPos - 2, UI_RENDER_NORMAL, TFO_BILINEAR ); 
		}
	}
	else if( m_dwGoState == GA_EFFECT_FADEOUT )
	{
		iXPos = ( (float)Setting::Width() / 2 );
		iYPos = (float)Setting::Height() * 0.45f;

		if( m_pGoText )
		{
			m_pGoText->Render( iXPos, iYPos );
		}

		if( m_pGoTextEffect )
		{			
			m_pGoTextEffect->SetScale( FLOAT1 );
			m_pGoTextEffect->SetAlpha( (float)MAX_ALPHA_RATE * m_fGoAniRate );
			m_pGoTextEffect->Render( iXPos, iYPos - 2, UI_RENDER_NORMAL, TFO_BILINEAR ); 
		}
	}
	else if( m_dwGoState == GA_HALF_DELAY )
	{
		iXPos = ( (float)Setting::Width() / 2 );
		iYPos = (float)Setting::Height() * 0.45f;

		if( m_pGoText )
		{
			m_pGoText->Render( iXPos, iYPos );
		}
	}
	else if( m_dwGoState == GA_FADEOUT )
	{
		if( m_pGoText )
		{
			iXPos = ( (float)Setting::Width() / 2 );
			iYPos = (float)Setting::Height() * 0.45f;

			m_pGoText->SetAlpha( (float)MAX_ALPHA_RATE * m_fGoAniRate );
			m_pGoText->Render( iXPos, iYPos );
		}
	}
}

void FightClubReadyWnd::OnRenderRound( int iXPos, int iYPos )
{
	if( m_pFinalText == NULL || m_pRoundText == NULL || m_pRoundNum == NULL ) return;
	if( m_dwBackReadyState == BR_BACK_OPEN || m_dwBackReadyState == BR_HIDE ) return;

	enum
	{
		ROUND_TEXT		= 236,
		FINAL_TEXT		= 194,
		NUM_TEXT		= 5,
	};

	if( m_iCurStage >= m_iMaxStage )
	{
		int iTextOffSet = (ROUND_TEXT+FINAL_TEXT) / 2;

		iXPos = ( (float)Setting::Width() / 2 ) - iTextOffSet;
		iYPos = (float)Setting::Height() * 0.45f;

		int iFinalXPos = iXPos;
		int iRoundXPos = iXPos+FINAL_TEXT;

		if( m_dwBackReadyState == BR_SLIDE )
		{
			m_pFinalText->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
			m_pFinalText->Render( iFinalXPos - (iTextOffSet * (1.0f-m_fBackReadyRate)), iYPos );

			m_pRoundText->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
			m_pRoundText->Render( iRoundXPos - (iTextOffSet * (1.0f-m_fBackReadyRate)), iYPos );
		}
		else
		{
			if( m_dwBackReadyState == BR_FADEOUT )
			{
				m_pFinalText->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
				m_pRoundText->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
			}
			else
			{
				m_pFinalText->SetAlpha( MAX_ALPHA_RATE );
				m_pRoundText->SetAlpha( MAX_ALPHA_RATE );
			}

			m_pFinalText->Render( iFinalXPos, iYPos );
			m_pRoundText->Render( iRoundXPos, iYPos );
		}
	}
	else
	{
		int iTextOffSet = (ROUND_TEXT+NUM_TEXT+2) / 2;

		iXPos = ( (float)Setting::Width() / 2 ) - iTextOffSet;
		iYPos = (float)Setting::Height() * 0.45f;

		int iRoundXPos = iXPos;
		int iNumXPos = iXPos+ROUND_TEXT+2;

		if( m_dwBackReadyState == BR_SLIDE )
		{
			m_pRoundText->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
			m_pRoundText->Render( iRoundXPos - (iTextOffSet * (1.0f-m_fBackReadyRate)), iYPos );

			m_pRoundNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
			m_pRoundNum->RenderNum( iNumXPos - (iTextOffSet * (1.0f-m_fBackReadyRate)), iYPos, m_iCurStage );
		}
		else
		{
			if( m_dwBackReadyState == BR_FADEOUT )
			{
				m_pRoundText->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
				m_pRoundNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fBackReadyRate );
			}
			else
			{
				m_pRoundText->SetAlpha( MAX_ALPHA_RATE );
				m_pRoundNum->SetAlpha( MAX_ALPHA_RATE );
			}

			m_pRoundText->Render( iRoundXPos, iYPos );
			m_pRoundNum->RenderNum( iNumXPos, iYPos, m_iCurStage );
		}
	}
}

void FightClubReadyWnd::OnRenderFight( int iXPos, int iYPos )
{
	int iNewXPos = ( (float)Setting::Width() / 2 );
	int iNewYPos = (float)Setting::Height() * 0.45f;

	if( m_dwGoState == GA_START )
	{
		if( m_pBlackBack )
		{
			float fAlphaRate = 0.6f * ( FLOAT1 - m_fGoAniRate );
			m_pBlackBack->SetAlpha( (float)MAX_ALPHA_RATE * fAlphaRate );
			m_pBlackBack->SetSize( Setting::Width(), Setting::Height() );
			m_pBlackBack->Render( iXPos, iYPos );
		}
		
		if( m_pFightText )
		{
			m_pFightText->SetScale( FLOAT1 );
			m_pFightText->SetAlpha( (float)MAX_ALPHA_RATE );
			m_pFightText->Render( iNewXPos, iNewYPos );
		}
	}
	else if( m_dwGoState == GA_EFFECT_FADEOUT )
	{
		if( m_pFightText )
		{
			m_pFightText->SetScale( FLOAT1 );
			m_pFightText->SetAlpha( (float)MAX_ALPHA_RATE );
			m_pFightText->Render( iNewXPos, iNewYPos );
		}
	}
	else if( m_dwGoState == GA_HALF_DELAY )
	{
		if( m_pFightText )
		{
			m_pFightText->SetScale( FLOAT1 );
			m_pFightText->SetAlpha( (float)MAX_ALPHA_RATE );
			m_pFightText->Render( iNewXPos, iNewYPos );
		}
	}
	else if( m_dwGoState == GA_FADEOUT )
	{
		if( m_pFightText )
		{
			m_pFightText->SetScale( FLOAT1 );
			m_pFightText->SetAlpha( (float)MAX_ALPHA_RATE * m_fGoAniRate );
			m_pFightText->Render( iNewXPos, iNewYPos );
		}
	}

	// 추가 출력
	if( m_pFightText && m_fFightAlphaRate > 0.0f )
	{
		m_pFightText->SetScale( m_fFightScaleRate );
		m_pFightText->SetAlpha( (float)MAX_ALPHA_RATE * m_fFightAlphaRate );
		m_pFightText->Render( iNewXPos, iNewYPos );
	}
}

void FightClubReadyWnd::ShowReadyWnd( bool bUseFightNPC, int iMaxStage, int iCurStage )
{
	ShowWnd();

	m_bUseFightNPC = bUseFightNPC;
	m_iMaxStage = iMaxStage;
	m_iCurStage = iCurStage;
}
//////////////////////////////////////////////////////////////////////////
FightClubRoundEndWnd::FightClubRoundEndWnd()
{
	m_pBackFrm              = NULL;
	m_pBackFrmLine          = NULL;
	m_pRoundEndKO			= NULL;
	m_pRoundEndTimeOut		= NULL;
	m_pRoundEndPerfect		= NULL;
	m_pRoundEndKOEffect		= NULL;
	m_pRoundEndPerfectEffect= NULL;

	m_pRewardBack			= NULL;
	m_pPesoIcon				= NULL;
	m_pPesoText				= NULL;
	m_pPesoNumber			= NULL;
	m_pGradeIcon			= NULL;
	m_pExpText				= NULL;
	m_pExpNumber			= NULL;

	m_pWinText				= NULL;
	m_pVictoryWinText		= NULL;
	m_pVictoryWinNum		= NULL;
	m_pLoseText				= NULL;

	m_pChampionText			= NULL;
	m_pChampVictoryText		= NULL;
	m_pChampWinText			= NULL;
	m_pChampWinNum			= NULL;
	m_pChallengerWinText	= NULL;

	//Tournament
	m_pBlueRoundNum			= NULL;
	m_pBlueRoundText		= NULL;
	m_pBlueFinalRoundText	= NULL;
	m_pBlueNextGoText		= NULL;
	m_pBlueNextFailText		= NULL;
	m_pBlueChampionText		= NULL;
	m_pBlueHalfChampionText	= NULL;

	m_pNPCFightWin = NULL;
	m_pNPCFightFinalWin = NULL;

	InitValue();
}

FightClubRoundEndWnd::~FightClubRoundEndWnd()
{
	g_MyInfo.InitLevelUPData();

	SAFEDELETE( m_pBackFrm );
	SAFEDELETE( m_pBackFrmLine );
	SAFEDELETE( m_pRoundEndKO );
	SAFEDELETE( m_pRoundEndTimeOut );
	SAFEDELETE( m_pRoundEndPerfect );
	SAFEDELETE( m_pRoundEndKOEffect );
	SAFEDELETE( m_pRoundEndPerfectEffect );

	SAFEDELETE( m_pRewardBack );
	SAFEDELETE( m_pPesoIcon );
	SAFEDELETE( m_pPesoText );
	SAFEDELETE( m_pPesoNumber );
	SAFEDELETE( m_pGradeIcon );
	SAFEDELETE( m_pExpText );
	SAFEDELETE( m_pExpNumber );

	SAFEDELETE( m_pWinText );
	SAFEDELETE( m_pVictoryWinText );
	SAFEDELETE( m_pVictoryWinNum );
	SAFEDELETE( m_pLoseText );

	SAFEDELETE( m_pChampionText );
	SAFEDELETE( m_pChampVictoryText );
	SAFEDELETE( m_pChampWinText );
	SAFEDELETE( m_pChampWinNum );
	SAFEDELETE( m_pChallengerWinText );

	SAFEDELETE( m_pNPCFightWin );
	SAFEDELETE( m_pNPCFightFinalWin );

	//Tournament
	SAFEDELETE( m_pBlueRoundNum );
	SAFEDELETE( m_pBlueRoundText );
	SAFEDELETE( m_pBlueFinalRoundText );
	SAFEDELETE( m_pBlueNextGoText );
	SAFEDELETE( m_pBlueNextFailText );
	SAFEDELETE( m_pBlueChampionText );
	SAFEDELETE( m_pBlueHalfChampionText );
}

void FightClubRoundEndWnd::InitValue()
{
	m_dwRoundEndType = ROUNDEND_KO;
	m_dwRoundEndTime = FRAMEGETTIME();
	m_fRoundEndRate  = 0.0f;
	m_dwRoundEndState= ROUNDEND_START;
	m_bRoundEndSync  = false;

	m_dwRewardState  = REWARD_NONE;
	m_fRewardRate    = 0.0f;
	m_iRewardExp	 = 0;
	m_iRewardPeso	 = 0;
	m_iWinerVictory  = 0;
	m_szWinerName.Clear();
	m_szLoserName.Clear();
	m_bChampWin		 = false;
	m_iMeSeqCount    = 0;
	m_dwRewardTime   = FRAMEGETTIME();

	m_dwWinnerUserIndex = 0;
	m_dwLoseUserIndex   = 0;

	// 리소스 초기화
	if( m_pBackFrm )
		m_pBackFrm->SetAlpha( MAX_ALPHA_RATE );
	if( m_pBackFrmLine )
		m_pBackFrmLine->SetAlpha( MAX_ALPHA_RATE );
	if( m_pRoundEndKO )
		m_pRoundEndKO->SetAlpha( MAX_ALPHA_RATE );
	if( m_pRoundEndTimeOut )
		m_pRoundEndTimeOut->SetAlpha( MAX_ALPHA_RATE );
	if( m_pRoundEndPerfect )
		m_pRoundEndPerfect->SetAlpha( MAX_ALPHA_RATE );
	if( m_pRoundEndKOEffect )
		m_pRoundEndKOEffect->SetAlpha( MAX_ALPHA_RATE );
	if( m_pRoundEndPerfectEffect )
		m_pRoundEndPerfectEffect->SetAlpha( MAX_ALPHA_RATE );

	m_bUseFightNPC = false;
	m_iMaxNPCStage = 0;
	m_iCurNPCStage = 0;

	m_iTournamentCurRound = 0;
	m_bTournamentModeType = false;
}

int FightClubRoundEndWnd::GetNumberCountRand( int iNumber )
{
	if( iNumber < 10 )
		iNumber = rand()%9+1;
	else if( iNumber < 100 )
		iNumber = rand()%90+10;
	else if( iNumber < 1000 )
		iNumber = rand()%900+100;
	else if( iNumber < 10000 )
		iNumber = rand()%9000+1000;
	else
		iNumber = rand()%90000+10000;
	return iNumber;
}

ioUIRenderImage *FightClubRoundEndWnd::GetGradeLevel( int iGradeLevel )
{
	const int iGradeIconPage = 35;
	const ioHashString szGradeIconMainName = "Grade";
	const ioHashString szGradeIconSubName  = "grade_";
	char szGradeIconName[MAX_PATH] = "";
	sprintf( szGradeIconName, "%s%d#%s%.2d", szGradeIconMainName.c_str(), ( iGradeLevel / iGradeIconPage ) + 1, szGradeIconSubName.c_str(), iGradeLevel ); //Except Extracting Hangeul
	return g_UIImageSetMgr.CreateImageByFullName( szGradeIconName );
}

void FightClubRoundEndWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void FightClubRoundEndWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BackFrmLine" )
	{
		SAFEDELETE( m_pBackFrmLine );
		m_pBackFrmLine = pImage;
		if( m_pBackFrmLine )
		{
			m_pBackFrmLine->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		}
	}
	else if( szType == "RoundEndKO" )
	{
		SAFEDELETE( m_pRoundEndKO );
		m_pRoundEndKO = pImage;
	}
	else if( szType == "RoundEndKOEffect" )
	{
		SAFEDELETE( m_pRoundEndKOEffect );
		m_pRoundEndKOEffect = pImage;
	}
	else if( szType == "RoundEndTimeOut" )
	{
		SAFEDELETE( m_pRoundEndTimeOut );
		m_pRoundEndTimeOut = pImage;
	}
	else if( szType == "RoundEndPerfect" )
	{
		SAFEDELETE( m_pRoundEndPerfect );
		m_pRoundEndPerfect = pImage;
	}
	else if( szType == "RoundEndPerfectEffect" )
	{
		SAFEDELETE( m_pRoundEndPerfectEffect );
		m_pRoundEndPerfectEffect = pImage;
	}
	else if( szType == "RewardBack" )
	{
		SAFEDELETE( m_pRewardBack );
		m_pRewardBack = pImage;

		if( m_pRewardBack )
		{
			m_pRewardBack->SetHorzFormatting( ioUIRenderImage::HF_CENTER );			
		}
	}
	else if( szType == "PesoIcon" )
	{
		SAFEDELETE( m_pPesoIcon );
		m_pPesoIcon = pImage;
	}
	else if( szType == "PesoText" )
	{
		SAFEDELETE( m_pPesoText );
		m_pPesoText = pImage;
	}
	else if( szType == "PesoNumber" )
	{
		SAFEDELETE( m_pPesoNumber );
		m_pPesoNumber = pImage;
	}
	else if( szType == "ExpText" )
	{
		SAFEDELETE( m_pExpText );
		m_pExpText = pImage;
	}
	else if( szType == "ExpNumber" )
	{
		SAFEDELETE( m_pExpNumber );
		m_pExpNumber = pImage;
	}
	else if( szType == "WinText" )
	{
		SAFEDELETE( m_pWinText );
		m_pWinText = pImage;

		if( m_pWinText )
		{
			m_pWinText->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );			
			m_pWinText->SetVertFormatting( ioUIRenderImage::VF_STRETCH );			
		}
	}
	else if( szType == "VictoryWinText" )
	{
		SAFEDELETE( m_pVictoryWinText );
		m_pVictoryWinText = pImage;
	}
	else if( szType == "VictoryWinNum" )
	{
		SAFEDELETE( m_pVictoryWinNum );
		m_pVictoryWinNum = pImage;
	}
	else if( szType == "LoseText" )
	{
		SAFEDELETE( m_pLoseText );
		m_pLoseText = pImage;

		if( m_pLoseText )
		{
			m_pLoseText->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );			
			m_pLoseText->SetVertFormatting( ioUIRenderImage::VF_STRETCH );			
		}
	}
	else if( szType == "ChampionText" )
	{
		SAFEDELETE( m_pChampionText );
		m_pChampionText = pImage;
	}
	else if( szType == "ChampVictoryText" )
	{
		SAFEDELETE( m_pChampVictoryText );
		m_pChampVictoryText = pImage;
	}
	else if( szType == "ChampWinText" )
	{
		SAFEDELETE( m_pChampWinText );
		m_pChampWinText = pImage;
	}
	else if( szType == "ChampWinNum" )
	{
		SAFEDELETE( m_pChampWinNum );
		m_pChampWinNum = pImage;
	}
	else if( szType == "ChallengerWinText" )
	{
		SAFEDELETE( m_pChallengerWinText );
		m_pChallengerWinText = pImage;
	}
	else if( szType == "NPCFightWinText" )
	{
		SAFEDELETE( m_pNPCFightWin );
		m_pNPCFightWin = pImage;
	}
	else if( szType == "NPCFightFinalWinText" )
	{
		SAFEDELETE( m_pNPCFightFinalWin );
		m_pNPCFightFinalWin = pImage;
	}
	else if( szType == "BlueRoundNum" )
	{
		SAFEDELETE( m_pBlueRoundNum );
		m_pBlueRoundNum = pImage;
	}
	else if( szType == "BlueRoundText" )
	{
		SAFEDELETE( m_pBlueRoundText );
		m_pBlueRoundText = pImage;
	}
	else if( szType == "BlueFinalRoundText" )
	{
		SAFEDELETE( m_pBlueFinalRoundText );
		m_pBlueFinalRoundText = pImage;
	}
	else if( szType == "BlueNextGoText" )
	{
		SAFEDELETE( m_pBlueNextGoText );
		m_pBlueNextGoText = pImage;
	}
	else if( szType == "BlueNextFailText" )
	{
		SAFEDELETE( m_pBlueNextFailText );
		m_pBlueNextFailText = pImage;
	}
	else if( szType == "BlueChampionText" )
	{
		SAFEDELETE( m_pBlueChampionText );
		m_pBlueChampionText = pImage;
	}
	else if( szType == "BlueHalfChampionText" )
	{
		SAFEDELETE( m_pBlueHalfChampionText );
		m_pBlueHalfChampionText = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void FightClubRoundEndWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szKoSnd		= xElement.GetStringAttribute_e( "KoSnd" );
	m_szWinSnd		= xElement.GetStringAttribute_e( "WinSnd" );
	m_szLoseSnd		= xElement.GetStringAttribute_e( "LoseSnd" );
	m_szPerfectSnd	= xElement.GetStringAttribute_e( "PerfectSnd" );
	m_szRollingSnd	= xElement.GetStringAttribute_e( "RollingSnd" );
	m_szTimeOutSnd	= xElement.GetStringAttribute_e( "TimeOutSnd" );
}

void FightClubRoundEndWnd::SetRoundEnd( int iEndType, ioHashString &rkLeaveName )
{
	ShowWnd();

	m_dwRoundEndType = iEndType;
	if( m_dwRoundEndType == ROUNDEND_LEAVEROOM )
	{
		m_dwRoundEndState		= ROUNDEND_LEAVEROOM_START;
		m_szRoundEndLeaveUser	= rkLeaveName;
	}
	else
	{
		m_dwRoundEndState       = ROUNDEND_START;

		switch( m_dwRoundEndType )
		{
		case ROUNDEND_KO:
			if( !m_szKoSnd.IsEmpty() )
				g_SoundMgr.PlaySound( m_szKoSnd );
			break;
		case ROUNDEND_TIMEOUT:
			if( !m_szTimeOutSnd.IsEmpty() )
				g_SoundMgr.PlaySound( m_szTimeOutSnd );
			break;
		case ROUNDEND_PERFECT:
			if( !m_szPerfectSnd.IsEmpty() )
				g_SoundMgr.PlaySound( m_szPerfectSnd );
			break;
		}
	}

	m_BonusData.clear();
	m_PlayClassType.clear();
	g_MyInfo.InitLevelUPData();
}

void FightClubRoundEndWnd::SetRoundResult( DWORD dwResultTime, ioHashString &rkChamp, ioHashString &rkChall, int iChampTotalExp, int iChampTotalPeso, int iChampVictory, bool bChampWin, float fChampBonusArray[ioPlayMode::BA_MAX], int iMeSeqCount )
{
	m_bRoundEndSync = true;

	m_iMeSeqCount = iMeSeqCount;
	if( bChampWin )
	{
		m_szWinerName	= rkChamp;
		m_szLoserName   = rkChall;
		m_iWinerVictory	= iChampVictory;
		m_bChampWin     = true;
	}
	else 
	{
		m_szWinerName	= rkChall;
		m_szLoserName   = rkChamp;
		m_iWinerVictory	= iChampVictory;
		m_bChampWin     = false;
	}

	if( rkChamp == g_MyInfo.GetPublicID() )
	{
		// 내 정보		
		SAFEDELETE( m_pGradeIcon );
		m_pGradeIcon = GetGradeLevel( g_MyInfo.GetGradeLevel() );

		m_iRewardExp = iChampTotalExp;
		m_iRewardPeso= iChampTotalPeso;

		SetBonus( fChampBonusArray, true );
	}
	else if( rkChall == g_MyInfo.GetPublicID() )
	{
		// 내 정보		
		SAFEDELETE( m_pGradeIcon );
		m_pGradeIcon = GetGradeLevel( g_MyInfo.GetGradeLevel() );

		m_iRewardExp = iChampTotalExp;
		m_iRewardPeso= iChampTotalPeso;

		SetBonus( fChampBonusArray, false );
	}
	else
	{
		// 관전자 정보
		m_iRewardExp = iChampTotalExp;
		m_iRewardPeso= iChampTotalPeso;
	}

	m_bTournamentModeType = g_BattleRoomMgr.IsTournamentModeType();
	if( m_bTournamentModeType )
		SetTournamentResult();
}

void FightClubRoundEndWnd::SetTournamentResult()
{	
	TournamentNode* pTournament = g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex() );
	if( pTournament )
	{
		ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
		if( pUserTournament )
		{
			DWORD dwTeamIndex = pUserTournament->GetTeamIndex( g_BattleRoomMgr.GetTournamentIndex() );
			TeamGlobalData* pTeam = g_TournamentTeamMgr.GetTeamData( dwTeamIndex, false );
			if( pTeam )
			{
				m_iTournamentCurRound  = Help::TournamentCurrentRoundWithTeam( pTournament->GetTournamentStartTeamCount(), pTeam->GetTourPos() );					
				return;
			}
		}

		//여기로 왔다면 관전자
		int iTourPos = pTournament->GetState() - TournamentNode::STATE_TOURNAMENT;
		m_iTournamentCurRound  = Help::TournamentCurrentRoundWithTeam( pTournament->GetTournamentStartTeamCount(), iTourPos );
	}
}

void FightClubRoundEndWnd::SetFightNPCInfo( bool bUseFightNPC, int iMaxStage, int iCurStage )
{
	m_bUseFightNPC = bUseFightNPC;
	m_iMaxNPCStage = iMaxStage;
	m_iCurNPCStage = iCurStage;

	if( m_szWinerName == g_MyInfo.GetPublicID() )
	{
		if( m_bUseFightNPC && m_iCurNPCStage == m_iMaxNPCStage )
		{
			g_QuestMgr.QuestCompleteTerm( QC_CHAMPION_AI_CLEAR );
		}
	}
}

void FightClubRoundEndWnd::SetBonus( float fBonusArray[ioPlayMode::BA_MAX], bool bChamp )
{
	m_BonusData.clear();

	// 보너스 표시 우선 순위
	/*
	◎ 배치 우선순위 - 0
	※ 용병 레벨업 : +보너스페소
	◎ 배치 우선순위 - 1
	※ 기여도 : x기여도%
	◎ 배치 우선순위 - 2
	※ 명성페널티 : x10%, x50% → 명성상태에 따라서 적용
	> 전체 보상배율에 (x)형태로 적용
	◎ 배치 우선순위 - 3
	※ 용병보너스 : +0~100% (용병보유수량에 따라서 차등 적용)
	◎ 배치 우선순위 - 4
	※ 래더타이틀 페소 보너스 : +0~100% (타이틀에 따라서 차등 적용)
	◎ 배치 우선순위 - 5
	※ 길드보너스 : +15%~30% (길드랭크에 따라서 적용, 진영전에서만 적용)
	◎ 배치 우선순위 - 6
	※ 친구보너스 : +5% (친구등록 후 30분 지난 후부터 적용)
	◎ 배치 우선순위 - 7
	※ 미션보너스 : +10% → 미션모드(파워/히든) 플레이시 적용
	※ PC방보너스 : +20% → PC방 상태가 활성화된 이후 적용
	※ 이벤트보너스 : +?% → 이벤트 셋팅시 지정된 보너스 적용
	※ 우선순위 : 이벤트보너스 > PC방보너스 > 미션보너스
	◎ 배치 우선순위 - 8
	※ 연속게임 : +?% → 연속해서 동일 모드
	◎ 배치 우선순위 - 9
	※ 수상보너스 : +?% → 수상획득시 지정된 보너스를 적용, (-)도 가능
	◎ 배치 우선순위 - 10
	※ 특별아이템 보너스 : +?% → 경험치/페소 (+)특별아이템 사용시 지정된 보너스 적용
	◎ 배치 우선순위 - 11
	※ 개인연승 보너스 : +10% x 연승횟수(2회 이상일때 실제연승횟수-1 값)
	*/
	
	int iCurBonusIcon = 0;

	// 챔피언 모드 전용 연승 보너스
	if( fBonusArray[ioPlayMode::BA_VICTORIES_PESO] != 0.0f )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		// 챔피언 모드에서는 페소 & 경험치 모두 보너스 반영
		BonusData kData;
		kData.m_dwBonusID	= BONUS_VICTORIES_PESO;
		kData.m_iBonusPeso  = (float)( fBonusArray[ioPlayMode::BA_VICTORIES_PESO] * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_VICTORIES_PESO] * FLOAT100 ) + 0.001f;
		if( bChamp )
			kData.m_szTitle     = "연승";
		else
			kData.m_szTitle     = "연승격파";

		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}

	// 용병 레벨업
	for(int i = 0;i < (int)m_PlayClassType.size();i++)
	{
		if( g_MyInfo.IsClassLevelUP( m_PlayClassType[i] ) )
		{
			if( iCurBonusIcon >= MAX_BONUS ) return;

			BonusData kData;
			kData.m_dwBonusID	= BONUS_LEVELUP;
			kData.m_iBonusPeso  = g_MyInfo.GetClassLevelUPPeso( m_PlayClassType[i] );
			kData.m_iBonusExp   = 0;
			kData.m_szTitle     = "레벨업";
			m_BonusData.push_back( kData );
			iCurBonusIcon++;
		}
	}

	// 명성 페널티 
	if( g_MyInfo.GetBlockPointPer() != FLOAT1 )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_MANNER;
		kData.m_iBonusPeso  = (float)( g_MyInfo.GetBlockPointPer() * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = (float)( g_MyInfo.GetBlockPointPer() * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "명성제한";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}

	// 용병 보유 보너스
	if( fBonusArray[ioPlayMode::BA_SOLDIER_CNT] != 0.0f )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_SOLDIER;
		kData.m_iBonusPeso  = 0;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_SOLDIER_CNT] * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "용병 보유";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}

	//권한 아이템 보너스
	if( fBonusArray[ioPlayMode::BA_ETC_ITEM] != 0.0f )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_ETC_ITEM;
		kData.m_iBonusPeso  = (float)( fBonusArray[ioPlayMode::BA_ETC_ITEM] * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_ETC_ITEM] * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "아이템";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}

	if( fBonusArray[ioPlayMode::BA_ETC_ITEM_EXP]  != 0.0f ||
		fBonusArray[ioPlayMode::BA_ETC_ITEM_PESO] != 0.0f  )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_ETC_ITEM_EXP_PESO;
		kData.m_iBonusPeso  = (float)( fBonusArray[ioPlayMode::BA_ETC_ITEM_PESO] * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_ETC_ITEM_EXP] * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "아이템";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}

	// 길드 보너스
	if( fBonusArray[ioPlayMode::BA_GUILD] != 0.0f )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_GUILD;
		kData.m_iBonusPeso  = (float)( fBonusArray[ioPlayMode::BA_GUILD] * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_GUILD] * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "길드";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}

	// 친구 보너스
	if( fBonusArray[ioPlayMode::BA_FRIEND] != 0.0f ) 
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_FRIEND;
		kData.m_iBonusPeso  = (float)( fBonusArray[ioPlayMode::BA_FRIEND] * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_FRIEND] * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "친구";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}

	// PCRoom 보너스
	if( fBonusArray[ioPlayMode::BA_PCROOM_EXP] != 0.0f || fBonusArray[ioPlayMode::BA_PCROOM_PESO] != 0.0f )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_PCROOM;
		kData.m_iBonusPeso  = (float)( fBonusArray[ioPlayMode::BA_PCROOM_PESO] * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_PCROOM_EXP] * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "PC방";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	} // 미션 보너스
	else if( fBonusArray[ioPlayMode::BA_PLAYMODE] != 0.0f )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_MODE;
		kData.m_iBonusPeso  = (float)( fBonusArray[ioPlayMode::BA_PLAYMODE] * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_PLAYMODE] * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "모드";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}

	// 이벤트 페소 보너스
	// Event 보너스
	if( fBonusArray[ioPlayMode::BA_EVENT] != 0.0f &&
		fBonusArray[ioPlayMode::BA_EVENT_PESO] != 0.0f )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_EVENT_PESO_EXP;
		kData.m_iBonusPeso  = (float)( fBonusArray[ioPlayMode::BA_EVENT_PESO] * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_EVENT] * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "이벤트";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}
	else if( fBonusArray[ioPlayMode::BA_EVENT_PESO] != 0.0f )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_PESO_EVENT;
		kData.m_iBonusPeso  = (float)( fBonusArray[ioPlayMode::BA_EVENT_PESO] * FLOAT100 ) + 0.001f;
		kData.m_iBonusExp   = 0;
		kData.m_szTitle     = "이벤트";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}
	else if( fBonusArray[ioPlayMode::BA_EVENT] != 0.0f )
	{
		// 아이콘 제한
		if( iCurBonusIcon >= MAX_BONUS ) return;

		BonusData kData;
		kData.m_dwBonusID	= BONUS_EVENT;
		kData.m_iBonusPeso  = 0;
		kData.m_iBonusExp   = (float)( fBonusArray[ioPlayMode::BA_EVENT] * FLOAT100 ) + 0.001f;
		kData.m_szTitle     = "이벤트";
		m_BonusData.push_back( kData );
		iCurBonusIcon++;
	}
}

void FightClubRoundEndWnd::SetPlayClassType( int iClassType )
{
	m_PlayClassType.push_back( iClassType );
}

void FightClubRoundEndWnd::iwm_show()
{
	InitValue();
}

void FightClubRoundEndWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	OnProcessRoundEnd();
	OnProcessReward();
}

void FightClubRoundEndWnd::OnProcessRoundEnd()
{
	if( m_dwRoundEndState == ROUNDEND_END ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwRoundEndTime;
	switch( m_dwRoundEndState )
	{
	case ROUNDEND_START:
		if( dwGapTime < ROUNDEND_START_TIME )
		{
			m_fRoundEndRate = (float)dwGapTime / ROUNDEND_START_TIME;
		}
		else
		{
			m_dwRoundEndState = ROUNDEND_EFFECT;
			m_dwRoundEndTime  = FRAMEGETTIME();
			m_fRoundEndRate   = FLOAT1;
		}
		break;
	case ROUNDEND_EFFECT:
		if( dwGapTime < ROUNDEND_EFFECT_TIME )
		{
			m_fRoundEndRate = FLOAT1 - ( (float)dwGapTime / ROUNDEND_EFFECT_TIME );
		}
		else
		{
			m_dwRoundEndState = ROUNDEND_DELAY;
			m_dwRoundEndTime  = FRAMEGETTIME();
			m_fRoundEndRate   = 0.0f;
		}
		break;
	case ROUNDEND_LEAVEROOM_START:
		if( dwGapTime < ROUNDEND_LEAVEROOM_START_TIME )
		{
			m_fRoundEndRate = (float)dwGapTime / ROUNDEND_LEAVEROOM_START_TIME;
		}
		else
		{
			m_dwRoundEndState = ROUNDEND_DELAY;
			m_dwRoundEndTime  = FRAMEGETTIME();
			m_fRoundEndRate   = FLOAT1;
		}
		break;
	case ROUNDEND_DELAY:
		if( m_bRoundEndSync )
		{
			//
			if( dwGapTime > ROUNDEND_DELAY_TIME )
			{
				m_dwRoundEndState = ROUNDEND_FADEOUT;
				m_dwRoundEndTime  = FRAMEGETTIME();
				m_fRoundEndRate   = FLOAT1;
			}
		}
		else if( dwGapTime > ROUNDEND_DELAY_TIME * 10 )
		{
			m_dwRoundEndState = ROUNDEND_FADEOUT;
			m_dwRoundEndTime  = FRAMEGETTIME();
			m_fRoundEndRate   = FLOAT1;
		}
		break;
	case ROUNDEND_FADEOUT:
		if( dwGapTime < ROUNDEND_FADEOUT_TIME )
		{
			m_fRoundEndRate = FLOAT1 - ( (float)dwGapTime / ROUNDEND_FADEOUT_TIME );
		}
		else
		{
			m_dwRoundEndState = ROUNDEND_END;
			m_dwRoundEndTime  = FRAMEGETTIME();
			m_fRoundEndRate   = 0.0f;

			if( m_bRoundEndSync )
			{
				m_dwRewardState = REWARD_ROLLING;
				m_dwRewardTime  = FRAMEGETTIME();
				m_fRewardRate   = 0.0f;

				if( m_szWinerName == g_MyInfo.GetPublicID() )
				{
					if( !m_szWinSnd.IsEmpty() )
						g_SoundMgr.PlaySound( m_szWinSnd );

					if( m_iRewardExp > 0 || m_iRewardPeso > 0 )
					{
						if( !m_szRollingSnd.IsEmpty() )
							g_SoundMgr.PlaySound( m_szRollingSnd );
					}
				}
				else if( m_szLoserName == g_MyInfo.GetPublicID() )
				{
					if( !m_szLoseSnd.IsEmpty() )
						g_SoundMgr.PlaySound( m_szLoseSnd );

					if( m_iRewardExp > 0 || m_iRewardPeso > 0 )
					{
						if( !m_szRollingSnd.IsEmpty() )
							g_SoundMgr.PlaySound( m_szRollingSnd );
					}
				}
			}
		}
		break;
	case ROUNDEND_END:
		break;
	}
}

void FightClubRoundEndWnd::OnProcessReward()
{
	if( m_dwRewardState == REWARD_NONE ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwRewardTime;
	switch( m_dwRewardState )
	{
	case REWARD_ROLLING:
		if( dwGapTime <= REWARD_ROLLING_TIME )
		{
			m_fRewardRate =  (float)dwGapTime / REWARD_ROLLING_TIME;
		}
		else 
		{
			m_fRewardRate = FLOAT1;
		}
		break;
	}
}

void FightClubRoundEndWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderRoundEnd( iXPos, iYPos );
	if( m_bTournamentModeType )
	{
		OnRenderCustomTournamentWinLose( iXPos, iYPos );
		OnRenderCustomTournamentReward( iXPos, iYPos );
	}
	else
	{
		OnRenderWinLose( iXPos, iYPos );
		OnRenderReward( iXPos, iYPos );
	}

	OnRenderCheerResult( iXPos, iYPos );
}

void FightClubRoundEndWnd::OnRenderRoundEnd( int iXPos, int iYPos )
{
	if( m_dwRoundEndState == ROUNDEND_END ) return;

	iXPos = ( (float)Setting::Width() / 2 );
	iYPos = (float)Setting::Height() * 0.45f;

	ioUIRenderImage *pRoundEndText = NULL;
	ioUIRenderImage *pRoundEndTextEffect = NULL;
	switch( m_dwRoundEndType )
	{
	case ROUNDEND_KO:
		{
			pRoundEndText		= m_pRoundEndKO;
			pRoundEndTextEffect	= m_pRoundEndKOEffect;
		}
		break;
	case ROUNDEND_TIMEOUT:
		{
			pRoundEndText		= m_pRoundEndTimeOut;
		}
		break;
	case ROUNDEND_PERFECT:
		{
			pRoundEndText		= m_pRoundEndPerfect;
			pRoundEndTextEffect	= m_pRoundEndPerfectEffect;
		}
		break;
	case ROUNDEND_LEAVEROOM:
		{
			if( m_pBackFrm == NULL || m_pBackFrmLine == NULL ) return;

			enum{ START_H=29, ADD_H=83, };
			if( m_dwRoundEndState == ROUNDEND_LEAVEROOM_START )
				m_pBackFrm->SetSize( Setting::Width(), START_H + ( (float)ADD_H * m_fRoundEndRate )  );
			else
				m_pBackFrm->SetSize( Setting::Width(), START_H + ADD_H );	
			m_pBackFrm->SetAlpha( (float)MAX_ALPHA_RATE * m_fRoundEndRate );
			m_pBackFrm->Render( 0, iYPos - ( (float)m_pBackFrm->GetHeight() * FLOAT05 ) );

			m_pBackFrmLine->SetAlpha( (float)MAX_ALPHA_RATE * m_fRoundEndRate );
			m_pBackFrmLine->Render( iXPos, iYPos - ( (float)m_pBackFrm->GetHeight() * FLOAT05 ) - 1 );
			m_pBackFrmLine->Render( iXPos, ( iYPos - ( (float)m_pBackFrm->GetHeight() * FLOAT05 ) ) + m_pBackFrm->GetHeight() - 9 );

			ioComplexStringPrinter kPrinter;
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
			kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_18, STR(1), m_szRoundEndLeaveUser.c_str() );
			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_18, STR(2) );
			kPrinter.PrintFullText( iXPos, iYPos - 9, TAT_CENTER, (float)MAX_ALPHA_RATE * m_fRoundEndRate );
			kPrinter.ClearList();
		}
		return;      // 완전 다른 결과이므로 아래 랜더부분 패스
	}

	if( pRoundEndText )
	{
		pRoundEndText->SetScale( FLOAT1 );
		if( m_dwRoundEndState == ROUNDEND_FADEOUT )
			pRoundEndText->SetAlpha( (float)MAX_ALPHA_RATE * m_fRoundEndRate );
		else
			pRoundEndText->SetAlpha( MAX_ALPHA_RATE );
		pRoundEndText->Render( iXPos, iYPos );

		if( m_dwRoundEndType != ROUNDEND_TIMEOUT && m_dwRoundEndState == ROUNDEND_START )
		{
			float fScale = FLOAT1 + ( 4.0f * (1.0f - m_fRoundEndRate ) );
			pRoundEndText->SetScale( fScale );
			pRoundEndText->SetAlpha( (float)MAX_ALPHA_RATE * m_fRoundEndRate );
			pRoundEndText->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	if( m_dwRoundEndState == ROUNDEND_EFFECT )
	{
		if( pRoundEndTextEffect )
		{
			pRoundEndTextEffect->SetScale( 2.0f );
			pRoundEndTextEffect->SetAlpha( (float)MAX_ALPHA_RATE * m_fRoundEndRate );
			pRoundEndTextEffect->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

void FightClubRoundEndWnd::OnRenderReward( int iXPos, int iYPos )
{
	if( m_dwRewardState == REWARD_NONE ) return;
	if( m_szWinerName != g_MyInfo.GetPublicID() && m_szLoserName != g_MyInfo.GetPublicID() ) return;

	iXPos = ( (float)Setting::Width() / 2 );
	iYPos = ( (float)Setting::Height() * 0.32f ) - 102;

	if( m_pRewardBack )
	{
		m_pRewardBack->Render( iXPos, iYPos + 152 );
	}

	int iStartX = 110; 
	int iRewardPeso      = m_iRewardPeso;
	if( m_fRewardRate < 0.75f )
		iRewardPeso = GetNumberCountRand( iRewardPeso );
	int iPesoNumberCount = GetNumberCount( iRewardPeso );
	int iPesoNumberSize  = ( iPesoNumberCount - 1 ) * 21;		// 숫자 1개가 21픽셀

	int iRewardExp      = m_iRewardExp;
	if( m_fRewardRate < 1.00f )
		iRewardExp = GetNumberCountRand( iRewardExp );
	int iExpNumberCount  = GetNumberCount( iRewardExp );
	int iExpNumberSize   = ( iExpNumberCount - 1 ) * 21;		// 숫자 1개가 21픽셀
	iStartX += (float)(iPesoNumberSize + iExpNumberSize) / 2;

	if( m_pPesoIcon )
	{
		m_pPesoIcon->SetScale( 0.63f );
		m_pPesoIcon->Render( iXPos + 19 - iStartX, iYPos + 161, UI_RENDER_NORMAL, TFO_BILINEAR );
		iStartX -= 38;
	}

	if( m_pPesoNumber )
	{
		m_pPesoNumber->RenderNum( iXPos - iStartX, iYPos + 149, iRewardPeso, -3.0f );
		iStartX -= ( 24 + iPesoNumberSize );
	}

	if( m_pPesoText )
	{
		iStartX += 3;
		m_pPesoText->Render( iXPos - iStartX, iYPos + 152 );
		iStartX -= m_pPesoText->GetWidth();
	}

	if( m_pGradeIcon )
	{
		iStartX -= 27;
		m_pGradeIcon->SetScale( 0.63f );
		m_pGradeIcon->Render( iXPos - iStartX, iYPos + 161, UI_RENDER_NORMAL, TFO_BILINEAR );
		iStartX -= 19;
	}

	if( m_pExpNumber )
	{
		m_pExpNumber->RenderNum( iXPos - iStartX, iYPos + 149, iRewardExp, -3.0f );
		iStartX -= ( 24 + iExpNumberSize );
	}

	if( m_pExpText )
	{
		iStartX += 3;
		m_pExpText->Render( iXPos - iStartX, iYPos + 152 );
	}
	OnRenderBonus( iXPos, iYPos );
}

void FightClubRoundEndWnd::OnRenderCustomTournamentReward( int iXPos, int iYPos )
{
	if( m_dwRewardState == REWARD_NONE ) return;

	iXPos = ( (float)Setting::Width() / 2 );
	iYPos = ( (float)Setting::Height() * 0.32f ) - 102;

	if( m_pRewardBack )
		m_pRewardBack->Render( iXPos, iYPos + 152 );

	int iPesoSize = GetNumberCount( m_iRewardPeso ) * 21; // 숫자 1개가 21픽셀	
	int iWidth = 43;
	iWidth += iPesoSize;
	iWidth += m_pPesoText->GetWidth();
	iWidth = iWidth /2;

	if( m_pPesoIcon )
	{
		m_pPesoIcon->SetScale( 0.63f );
		m_pPesoIcon->Render( iXPos + 19 - iWidth, iYPos + 161, UI_RENDER_NORMAL, TFO_BILINEAR );
		iWidth -= 38;
	}

	if( m_pPesoNumber )
	{
		m_pPesoNumber->RenderNum( iXPos - iWidth, iYPos + 149, m_iRewardPeso, -3.0f );
		iWidth -= iPesoSize;
	}

	if( m_pPesoText )
		m_pPesoText->Render( iXPos - iWidth + 3, iYPos + 152 );
}

void FightClubRoundEndWnd::OnRenderWinLose( int iXPos, int iYPos )
{
	if( m_dwRewardState == REWARD_NONE ) return;

	iXPos = ( (float)Setting::Width() / 2 );
	iYPos = ( (float)Setting::Height() * 0.32f ) - 102;

	if( m_szWinerName == g_MyInfo.GetPublicID() )
	{
		if( m_bUseFightNPC )
		{
			OnRenderFightNPCWin( iXPos, iYPos );
		}
		else
		{
			if( m_iWinerVictory < 2 )
			{
				if( m_pWinText )
				{
					m_pWinText->SetScale( FLOAT1 );
					m_pWinText->SetAlpha( MAX_ALPHA_RATE );
					m_pWinText->Render( iXPos, iYPos + 77 );

					float fRate  = min( FLOAT1, m_fRewardRate * 4.0f );
					float fScale = 2.0f * fRate;
					m_pWinText->SetScale( FLOAT1 + fScale );
					m_pWinText->SetAlpha( MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * fRate ) );
					m_pWinText->Render( iXPos, iYPos + 77, UI_RENDER_NORMAL, TFO_BILINEAR ); 
				}
			}
			else
			{
				if( m_pVictoryWinText && m_pVictoryWinNum )
				{
					float fNumberWidth	= 69.0f;
					float fNumberHeight	= 96.0f;
					float fTextWidth	= 189.0f;
					float fTextHeigth	= 110.0f;
					float fNumberGapRate= 0.058f;

					int iStartX = ( fNumberWidth + fTextWidth ) / 2;
					int iWinNumberCount  = GetNumberCount( m_iWinerVictory );
					int iWinNumberSize   = ( iWinNumberCount - 1 ) * ( fNumberWidth + ( fNumberWidth * fNumberGapRate ) );		
					iStartX += (float)iWinNumberSize / 2;

					m_pVictoryWinNum->SetAlpha( MAX_ALPHA_RATE );
					m_pVictoryWinNum->RenderNum( iXPos - iStartX, iYPos + 22, m_iWinerVictory, -4.0f, FLOAT1 );
					iStartX -= iWinNumberSize + fNumberWidth;

					m_pVictoryWinText->SetScale( FLOAT1 );
					m_pVictoryWinText->SetAlpha( MAX_ALPHA_RATE );
					m_pVictoryWinText->Render( iXPos - iStartX, iYPos + 22 );
				}
			}
		}
	}
	else if( m_szLoserName == g_MyInfo.GetPublicID() )
	{
		if( m_pLoseText )
		{
			m_pLoseText->SetScale( FLOAT1 );
			m_pLoseText->SetAlpha( MAX_ALPHA_RATE );
			m_pLoseText->Render( iXPos, iYPos + 77 );

			float fRate  = min( FLOAT1, m_fRewardRate * 4.0f );
			float fScale = 2.0f * fRate;
			m_pLoseText->SetScale( FLOAT1 + fScale );
			m_pLoseText->SetAlpha( MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * fRate ) );
			m_pLoseText->Render( iXPos, iYPos + 77, UI_RENDER_NORMAL, TFO_BILINEAR ); 
		}
	}
	else
	{
		// 대기자 결과 
		if( m_bChampWin )
		{
			if( m_bUseFightNPC )
			{
				OnRenderFightNPCWin( iXPos, iYPos );
			}
			else
			{
				// 챔피언 승
				if( m_iWinerVictory < 2 )
				{
					float fNumberWidth		= 50.0f;
					float fChampTextWidth	= 196.0f;
					float fChampWinTextWidth= 126.0f;

					int iStartX = ( fNumberWidth + fChampTextWidth + fChampWinTextWidth + 8 ) / 2;

					if( m_pChampionText )
						m_pChampionText->Render( iXPos - iStartX, iYPos + 54 );
					iStartX -= fChampTextWidth;

					if( m_pChampWinNum )
					{
						iStartX -= 9;
						m_pChampWinNum->RenderNum( iXPos - iStartX, iYPos + 55, m_iWinerVictory );
						iStartX -= ( fNumberWidth + 1 );
					}

					if( m_pChampWinText )
						m_pChampWinText->Render( iXPos - iStartX, iYPos + 54 );
				}
				else
				{
					float fNumberWidth		= 50.0f;
					float fChampTextWidth	= 196.0f;
					float fChampWinTextWidth= 68.0f;
					float fNumberGapRate	= 0.058f;

					int iStartX = ( fNumberWidth + fChampTextWidth + fChampWinTextWidth + 8 ) / 2;
					int iWinNumberCount  = GetNumberCount( m_iWinerVictory );
					int iWinNumberSize   = ( iWinNumberCount - 1 ) * ( fNumberWidth + ( fNumberWidth * fNumberGapRate ) );		
					iStartX += (float)iWinNumberSize / 2;

					if( m_pChampionText )
						m_pChampionText->Render( iXPos - iStartX, iYPos + 54 );
					iStartX -= fChampTextWidth;

					if( m_pChampWinNum )
					{
						iStartX -= 9;
						m_pChampWinNum->RenderNum( iXPos - iStartX, iYPos + 55, m_iWinerVictory, -( fNumberWidth * fNumberGapRate ) );
						iStartX -= ( iWinNumberSize + fNumberWidth + 1 );
					}

					if( m_pChampVictoryText )
						m_pChampVictoryText->Render( iXPos - iStartX, iYPos + 54 );
				}
			}
		}
		else
		{
			// 도전자 승			
			if( m_pChallengerWinText )
				m_pChallengerWinText->Render( iXPos - ( m_pChallengerWinText->GetWidth() / 2 ), iYPos + 54 );
		}
		
		if( m_iMeSeqCount > 0 )
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			if( m_iMeSeqCount <= 1 )
			{
				kPrinter.AddTextPiece( FONT_SIZE_18, STR(1) );
			}
			else
			{
				kPrinter.AddTextPiece( FONT_SIZE_18, STR(2), m_iMeSeqCount );
			}

			kPrinter.PrintFullText( iXPos, iYPos + 22, TAT_CENTER );
			kPrinter.ClearList();
		}
	}
}

void FightClubRoundEndWnd::OnRenderCustomTournamentWinLose( int iXPos, int iYPos )
{
	if( m_dwRewardState == REWARD_NONE ) return;

	iXPos = ( (float)Setting::Width() / 2 );
	iYPos = ( (float)Setting::Height() * 0.32f ) - 102;

	//우승, 준우승
	if( m_iTournamentCurRound == 1 )
		OnRenderCustomTournamentChampion( iXPos, iYPos );
	//결승 진출
	else if( m_iTournamentCurRound == 2 )
		OnRenderCustomTournamentFinalRound( iXPos, iYPos );
	//N강
	else
		OnRenderCustomTournamentRound( iXPos, iYPos );
}

void FightClubRoundEndWnd::OnRenderCustomTournamentChampion( int iXPos, int iYPos )
{
	if( !m_pBlueChampionText || !m_pBlueHalfChampionText )
		return;

	if( g_MyInfo.GetPublicID() == m_szWinerName || g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
	{
		m_pBlueChampionText->SetVertFormatting( ioUIRenderImage::VF_CENTER );
		m_pBlueChampionText->Render( iXPos, iYPos + 60 );
	}
	else
	{
		m_pBlueHalfChampionText->SetVertFormatting( ioUIRenderImage::VF_CENTER );
		m_pBlueHalfChampionText->Render( iXPos, iYPos + 60 );
	}
}

void FightClubRoundEndWnd::OnRenderCustomTournamentFinalRound( int iXPos, int iYPos )
{
	if( !m_pBlueNextGoText || !m_pBlueNextFailText || ! m_pBlueFinalRoundText )
		return;

	if( g_MyInfo.GetPublicID() == m_szWinerName || g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
	{
		int iWidth = m_pBlueFinalRoundText->GetWidth();
		iWidth += 10;
		iWidth += m_pBlueNextGoText->GetWidth();
		iWidth = iWidth / 2;

		m_pBlueFinalRoundText->Render( iXPos - iWidth, iYPos + 60 );
		iWidth -= m_pBlueFinalRoundText->GetWidth();
		m_pBlueNextGoText->Render( iXPos - iWidth + 10, iYPos + 60  );		
	}
	else
	{
		int iWidth = m_pBlueFinalRoundText->GetWidth();
		iWidth += 10;
		iWidth += m_pBlueNextFailText->GetWidth();
		iWidth = iWidth / 2;

		m_pBlueFinalRoundText->Render( iXPos - iWidth, iYPos + 60 );		
		iWidth -= m_pBlueFinalRoundText->GetWidth();
		m_pBlueNextFailText->Render( iXPos - iWidth + 10, iYPos + 60 );
	}
}

void FightClubRoundEndWnd::OnRenderCustomTournamentRound( int iXPos, int iYPos )
{
	if( !m_pBlueRoundNum || !m_pBlueRoundText || ! m_pBlueNextGoText || !m_pBlueNextFailText )
		return;

	if( g_MyInfo.GetPublicID() == m_szWinerName || g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
	{
		int iWidth = 52 * GetNumberCount( m_iTournamentCurRound ); //라운드 숫자 글자 크기 계산
		iWidth += m_pBlueRoundText->GetWidth();
		iWidth += 10;
		iWidth += m_pBlueNextGoText->GetWidth();
		iWidth = iWidth / 2;

		m_pBlueRoundNum->RenderNum( iXPos - iWidth, iYPos + 60, m_iTournamentCurRound, -10.0f );
		iWidth -= 52 * GetNumberCount( m_iTournamentCurRound );
		m_pBlueRoundText->Render( iXPos - iWidth, iYPos + 60  );
		iWidth -= m_pBlueRoundText->GetWidth();
		m_pBlueNextGoText->Render( iXPos - iWidth + 10, iYPos + 60 );
	}
	else
	{
		int iWidth = 52 * GetNumberCount( m_iTournamentCurRound ); //라운드 숫자 글자 크기 계산		
		iWidth += m_pBlueRoundText->GetWidth();
		iWidth += 10;
		iWidth += m_pBlueNextFailText->GetWidth();
		iWidth = iWidth / 2;

		m_pBlueRoundNum->RenderNum( iXPos - iWidth, iYPos + 60, m_iTournamentCurRound, -10.0f );
		iWidth -= 52 * GetNumberCount( m_iTournamentCurRound );
		m_pBlueRoundText->Render( iXPos - iWidth, iYPos + 60  );
		iWidth -= m_pBlueRoundText->GetWidth();
		m_pBlueNextFailText->Render( iXPos - iWidth + 10, iYPos + 60 );
	}
}

void FightClubRoundEndWnd::OnRenderFightNPCWin( int iXPos, int iYPos )
{
	if( !m_pWinText || !m_pVictoryWinNum || !m_pNPCFightWin || !m_pNPCFightFinalWin )
		return;

	if( m_iCurNPCStage < m_iMaxNPCStage )	// 진행 스테이지 승수
	{
		float fNumberWidth	= 69.0f;
		float fNumberHeight	= 96.0f;
		float fTextWidth	= 98.0f;
		float fTextHeigth	= 110.0f;
		float fNumberGapRate= 0.058f;

		int iStartX = ( fNumberWidth + fTextWidth ) / 2;
		int iWinNumberCount  = GetNumberCount( m_iCurNPCStage );
		int iWinNumberSize   = ( iWinNumberCount - 1 ) * ( fNumberWidth + ( fNumberWidth * fNumberGapRate ) );		
		iStartX += (float)iWinNumberSize / 2;

		m_pVictoryWinNum->SetAlpha( MAX_ALPHA_RATE );
		m_pVictoryWinNum->RenderNum( iXPos - iStartX, iYPos + 22, m_iCurNPCStage, -4.0f, FLOAT1 );
		iStartX -= iWinNumberSize + fNumberWidth;

		m_pNPCFightWin->SetScale( FLOAT1 );
		m_pNPCFightWin->SetAlpha( MAX_ALPHA_RATE );
		m_pNPCFightWin->Render( iXPos - iStartX, iYPos + 22 );
	}
	else								// 최종 스테이지
	{
		int iOffSetX = 209;
		int iNewXPos = iXPos - iOffSetX;

		m_pNPCFightFinalWin->SetScale( FLOAT1 );
		m_pNPCFightFinalWin->SetAlpha( MAX_ALPHA_RATE );
		m_pNPCFightFinalWin->Render( iNewXPos+13, iYPos+22 );

		m_pWinText->SetScale( FLOAT1 );
		m_pWinText->SetAlpha( MAX_ALPHA_RATE );
		m_pWinText->Render( iNewXPos+204+100, iYPos+22+55 );
	}
}

void FightClubRoundEndWnd::OnRenderCheerResult( int iXPos, int iYPos )
{
	// 응원자에 대한 보상
	if( m_dwRewardState == REWARD_NONE ) return;
	if( (m_szWinerName == g_MyInfo.GetPublicID()) || (m_szLoserName == g_MyInfo.GetPublicID()) ) return;
	if( m_bTournamentModeType ) return;
	
	iXPos = ( (float)Setting::Width() / 2 );
	iYPos = ( (float)Setting::Height() * 0.32f ) - 102;

	int iRewardPeso = m_iRewardPeso;

	int iStartX = 50; 
	int iPesoNumberCount = GetNumberCount( iRewardPeso );
	int iPesoNumberSize  = ( iPesoNumberCount - 1 ) * 21;		// 숫자 1개가 21픽셀

	if( m_pRewardBack )
	{
		m_pRewardBack->Render( iXPos, iYPos + 152 );
	}

	if( m_pPesoIcon )
	{
		m_pPesoIcon->SetScale( 0.63f );
		m_pPesoIcon->Render( iXPos + 19 - iStartX, iYPos + 161, UI_RENDER_NORMAL, TFO_BILINEAR );
		iStartX -= 38;
	}

	if( m_pPesoNumber )
	{
		m_pPesoNumber->RenderNum( iXPos - iStartX, iYPos + 149, iRewardPeso, -3.0f );
		iStartX -= ( 24 + iPesoNumberSize );
	}

	if( m_pPesoText )
	{
		iStartX += 3;
		m_pPesoText->Render( iXPos - iStartX, iYPos + 152 );
		iStartX -= m_pPesoText->GetWidth();
	}

	if( m_pRewardBack )
	{
		m_pRewardBack->Render( iXPos, iYPos + 184 );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_OUTLINE_2X );

	if(m_iRewardPeso == 0)
	{
		DWORD dwCheerTabID = 0;
		FightClubTimeGaugeWnd *pFightClubGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
		if( pFightClubGauge )
		{
			dwCheerTabID = pFightClubGauge->GetCheerTabID();
		}

		if(dwCheerTabID == 0)
		{
			kPrinter.SetTextColor( 255, 103, 41 ); // 주황
			kPrinter.AddTextPiece( FONT_SIZE_14, "내가 응원한 유저가 승리하면 페소 보너스를 받을 수 있습니다" );
		}
		else
		{
			if(m_bChampWin && (dwCheerTabID==1))
			{
				// 어뷰즈에 의해 페소가 0인 경우
				kPrinter.SetTextColor( 255, 189, 2 ); // 노랑
				kPrinter.AddTextPiece( FONT_SIZE_14, "내가 응원한 유저가 승리했습니다" );
			}
			else if(!m_bChampWin && (dwCheerTabID==2))
			{
				// 어뷰즈에 의해 페소가 0인 경우
				kPrinter.SetTextColor( 255, 189, 2 ); // 노랑
				kPrinter.AddTextPiece( FONT_SIZE_14, "내가 응원한 유저가 승리했습니다" );
			}
			else
			{
				kPrinter.SetTextColor( 255, 103, 41 ); // 주황
				kPrinter.AddTextPiece( FONT_SIZE_14, "내가 응원한 유저가 패배했습니다" );
			}
		}
	}
	else
	{
		kPrinter.SetTextColor( 255, 189, 2 ); // 노랑
		kPrinter.AddTextPiece( FONT_SIZE_14, "내가 응원한 유저가 승리했습니다" );
	}

	kPrinter.PrintFullText( iXPos, iYPos + 187, TAT_CENTER );
	kPrinter.ClearList();
}

void FightClubRoundEndWnd::OnRenderBonus( int iXPos, int iYPos )
{
	int iMaxBonus = m_BonusData.size();
	for(int i = 0;i < iMaxBonus;i++)
	{
		BonusData &rkData = m_BonusData[i];

		if( m_pRewardBack )
		{
			m_pRewardBack->Render( iXPos, iYPos + 184 + (i * 24) );
		}
		OnRenderBonusText( rkData, iXPos, iYPos + 186 + (i * 24) );
	}
}

void FightClubRoundEndWnd::OnRenderBonusText( const BonusData &rkData, int iXPos, int iYPos )
{
	char szBonusPeso[MAX_PATH] = "";
	char szBonusExp[MAX_PATH]  = "";
	SafeSprintf( szBonusPeso, sizeof( szBonusPeso ), "+%d％", rkData.m_iBonusPeso );
	SafeSprintf( szBonusExp, sizeof( szBonusExp ), "+%d％", rkData.m_iBonusExp );

	switch( rkData.m_dwBonusID )
	{
	case BONUS_SOLDIER:
		break;
	case BONUS_GUILD:
		break;
	case BONUS_PCROOM:
		break;
	case BONUS_MODE:
		break;
	case BONUS_MANNER:
		{
			SafeSprintf( szBonusPeso, sizeof( szBonusPeso ), "x%d％", rkData.m_iBonusPeso );
			SafeSprintf( szBonusExp, sizeof( szBonusExp ), "x%d％", rkData.m_iBonusExp );
		}
		break;
	case BONUS_FRIEND:
		break;
	case BONUS_EVENT:
		break;
	case BONUS_PESO_EVENT:
		break;
	case BONUS_ETC_ITEM:
		break;
	case BONUS_CAMP_BATTLE:
		break;
	case BONUS_AWARD:
		break;
	case BONUS_ETC_ITEM_EXP_PESO: 
		break;
	case BONUS_VICTORIES_PESO:
		break;
	case BONUS_EVENT_PESO_EXP:
		break;
	case BONUS_LEVELUP:
		{
			sprintf( szBonusPeso, "+%d", rkData.m_iBonusPeso );
		}
		break;
	case BONUS_CONTRIBUTE:
		{
			SafeSprintf( szBonusPeso, sizeof( szBonusPeso ), "x%d％", rkData.m_iBonusPeso );
			SafeSprintf( szBonusExp, sizeof( szBonusExp ), "x%d％", rkData.m_iBonusExp );
		}
		break;
	case BONUS_HERO_TITLE_PESO:
		break;
	case BONUS_MODE_CONSECUTIVELY:
		break;
	default:
		return;
	}

	ioComplexStringPrinter kPrinter;

	// 페소
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetTextColor( 255, 189, 2 );
	if( rkData.m_iBonusPeso > 0 )
	{
		kPrinter.AddTextPiece( FONT_SIZE_14, "%s %s", rkData.m_szTitle.c_str(), szBonusPeso );
	}
	else
	{
		kPrinter.AddTextPiece( FONT_SIZE_14, "-     " );
	}
	kPrinter.PrintFullText( iXPos - 8, iYPos, TAT_RIGHT );
	kPrinter.ClearList();

	// 경험치
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetTextColor( 255, 103, 41 );
	if( rkData.m_iBonusExp > 0 )
	{
		kPrinter.AddTextPiece( FONT_SIZE_14, "%s %s", rkData.m_szTitle.c_str(), szBonusExp );
	}
	else
	{
		kPrinter.AddTextPiece( FONT_SIZE_14, "     -" );
	}
	kPrinter.PrintFullText( iXPos + 8, iYPos, TAT_LEFT );
	kPrinter.ClearList();
}
//////////////////////////////////////////////////////////////////////////
FightClubChallengerWnd::FightClubChallengerWnd()
{
	m_pBackFrm = NULL;
	m_pBackFrmLine = NULL;
	m_pNewChallengerText = NULL;

	InitValue();
}

FightClubChallengerWnd::~FightClubChallengerWnd()
{
	SAFEDELETE( m_pBackFrm );
	SAFEDELETE( m_pBackFrmLine );
	SAFEDELETE( m_pNewChallengerText );
}

void FightClubChallengerWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void FightClubChallengerWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BackFrmLine" )
	{
		SAFEDELETE( m_pBackFrmLine );
		m_pBackFrmLine = pImage;
		if( m_pBackFrmLine )
		{
			m_pBackFrmLine->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		}
	}
	else if( szType == "NewChallengerText" )
	{
		SAFEDELETE( m_pNewChallengerText );
		m_pNewChallengerText = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void FightClubChallengerWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szChallengerSnd = xElement.GetStringAttribute_e( "ChallengerSnd" );
}

void FightClubChallengerWnd::iwm_show()
{
	InitValue();
}

void FightClubChallengerWnd::InitValue()
{
	m_dwChallengerState = CHALLENGER_START;
	m_dwChallengerTime = FRAMEGETTIME();

	m_fChallengerRate = 0.0f;

	// 리소스 초기화
	if( m_pBackFrm )
		m_pBackFrm->SetAlpha( MAX_ALPHA_RATE );
	if( m_pBackFrmLine )
		m_pBackFrmLine->SetAlpha( MAX_ALPHA_RATE );
}

void FightClubChallengerWnd::SetChallenger()
{
	ShowWnd();

	m_dwChallengerState = CHALLENGER_START;

	if( !m_szChallengerSnd.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szChallengerSnd );
	}
}

void FightClubChallengerWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	OnProcessChallenger();
}

void FightClubChallengerWnd::OnProcessChallenger()
{
	if( m_dwChallengerState == CHALLENGER_END ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwChallengerTime;

	switch( m_dwChallengerState )
	{
	case CHALLENGER_START:
		if( dwGapTime < CHALLENGER_START_TIME )
		{
			m_fChallengerRate = (float)dwGapTime / CHALLENGER_START_TIME;
		}
		else
		{
			m_dwChallengerState = CHALLENGER_DELAY;
			m_dwChallengerTime  = FRAMEGETTIME();
			m_fChallengerRate   = FLOAT1;
		}
		break;
	case CHALLENGER_DELAY:
		if( dwGapTime > CHALLENGER_DELAY_TIME )
		{
			m_dwChallengerState = CHALLENGER_FADEOUT;
			m_dwChallengerTime  = FRAMEGETTIME();
			m_fChallengerRate   = FLOAT1;
		}
		break;
	case CHALLENGER_FADEOUT:
		if( dwGapTime < CHALLENGER_FADEOUT_TIME )
		{
			m_fChallengerRate = FLOAT1 - ( (float)dwGapTime / CHALLENGER_FADEOUT_TIME );
		}
		else
		{
			m_dwChallengerState = CHALLENGER_END;
			m_dwChallengerTime  = FRAMEGETTIME();
			m_fChallengerRate   = 0.0f;
		}
		break;
	case CHALLENGER_END:
		break;
	}
}

void FightClubChallengerWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderChallenger( iXPos, iYPos );
}

void FightClubChallengerWnd::OnRenderChallenger( int iXPos, int iYPos )
{
	if( m_dwChallengerState == CHALLENGER_END ) return;

	iXPos = ( (float)Setting::Width() / 2 );
	iYPos = (float)Setting::Height() * 0.45f;

	if( m_pBackFrm == NULL || m_pBackFrmLine == NULL ) return;

	enum{ START_H=29, ADD_H=83, };

	if( m_dwChallengerState == CHALLENGER_START )
		m_pBackFrm->SetSize( Setting::Width(), START_H + ( (float)ADD_H * m_fChallengerRate )  );
	else
		m_pBackFrm->SetSize( Setting::Width(), START_H + ADD_H );

	m_pBackFrm->SetAlpha( (float)MAX_ALPHA_RATE * m_fChallengerRate );
	m_pBackFrm->Render( 0, iYPos - ( (float)m_pBackFrm->GetHeight() * FLOAT05 ) );

	m_pBackFrmLine->SetAlpha( (float)MAX_ALPHA_RATE * m_fChallengerRate );
	m_pBackFrmLine->Render( iXPos, iYPos - ( (float)m_pBackFrm->GetHeight() * FLOAT05 ) - 1 );
	m_pBackFrmLine->Render( iXPos, ( iYPos - ( (float)m_pBackFrm->GetHeight() * FLOAT05 ) ) + m_pBackFrm->GetHeight() - 9 );

	if( m_pNewChallengerText )
	{
		m_pNewChallengerText->SetAlpha( (float)MAX_ALPHA_RATE * m_fChallengerRate );
		m_pNewChallengerText->Render( iXPos-185, iYPos-41 );
	}
}
//////////////////////////////////////////////////////////////////////////
FightClubFightNPCContinueWnd::FightClubFightNPCContinueWnd()
{
	m_pContinueText = NULL;
	m_pContinueNum = NULL;

	InitValue();
}

FightClubFightNPCContinueWnd::~FightClubFightNPCContinueWnd()
{
	SAFEDELETE( m_pContinueText );
	SAFEDELETE( m_pContinueNum );
}

void FightClubFightNPCContinueWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ContinueText" )
	{
		SAFEDELETE( m_pContinueText );
		m_pContinueText = pImage;
	}
	else if( szType == "ContinueNum" )
	{
		SAFEDELETE( m_pContinueNum );
		m_pContinueNum = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void FightClubFightNPCContinueWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iStartCnt = xElement.GetIntAttribute_e( "ContinueTime" );

	m_ContinueBackSnd	= xElement.GetStringAttribute_e( "ContinueBackSnd" );
	m_ContinueCntSnd	= xElement.GetStringAttribute_e( "ContinueCntSnd" );
	m_ContinueCntEndSnd	= xElement.GetStringAttribute_e( "ContinueCntEndSnd" );
}

void FightClubFightNPCContinueWnd::iwm_hide()
{
	g_SoundMgr.StopSound( m_ContinueBackSnd, 0 );
	g_SoundMgr.StopSound( m_ContinueCntSnd, 0 );
	g_SoundMgr.StopSound( m_ContinueCntEndSnd, 0 );
}

void FightClubFightNPCContinueWnd::InitValue()
{
	m_iCurCnt = m_iStartCnt;
	m_iCurStage = 0;

	m_bRequestServer = false;
}

void FightClubFightNPCContinueWnd::SetContinue( int iStage, const ioHashString &szChampName )
{
	ShowWnd();

	InitValue();

	m_iCurStage = iStage;
	m_ChampName = szChampName;

	m_dwContinueStartTime = FRAMEGETTIME();

	int iXPos = ( (float)Setting::Width() / 2 );
	int iYPos = ( (float)Setting::Height() * 0.32f ) - 102;

	ioWnd *pRetryBtn = FindChildWnd( ID_RETRY_BTN );
	if( pRetryBtn )
	{
		pRetryBtn->SetWndPos( 300-137, iYPos+274 );
	}

	ioWnd *pRoomBtn = FindChildWnd( ID_ROOM_BTN );
	if( pRoomBtn )
	{
		pRoomBtn->SetWndPos( 300+1, iYPos+274 );
	}

	g_SoundMgr.PlaySound( m_ContinueBackSnd, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void FightClubFightNPCContinueWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	OnProcessContinue();
}

void FightClubFightNPCContinueWnd::OnProcessContinue()
{
	DWORD dwMaxTime = m_iStartCnt * 1000;
	DWORD dwGapTime = FRAMEGETTIME() - m_dwContinueStartTime;

	int iPreCnt = m_iCurCnt;

	if( dwMaxTime < dwGapTime )
		m_iCurCnt = 0;
	else
		m_iCurCnt = (dwMaxTime - dwGapTime) / 1000;

	if( iPreCnt != m_iCurCnt )
	{
		if( m_iCurCnt > 0 )
			g_SoundMgr.PlaySound( m_ContinueCntSnd );
		else
			g_SoundMgr.PlaySound( m_ContinueCntEndSnd );
	}
}

void FightClubFightNPCContinueWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderContinue( iXPos, iYPos );
}

void FightClubFightNPCContinueWnd::OnRenderContinue( int iXPos, int iYPos )
{
	iXPos = ( (float)Setting::Width() / 2 );
	iYPos = ( (float)Setting::Height() * 0.32f ) - 102;

	// continue text
	if( m_pContinueText )
	{
		m_pContinueText->Render( iXPos-51, iYPos+49 );
	}

	// count text
	if( m_pContinueNum )
	{
		m_pContinueNum->RenderNum( iXPos-47, iYPos+72, m_iCurCnt );
	}

	// 라운드 text
	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_18, "%d라운드에 다시 도전 하시겠습니까?", m_iCurStage );

	kPrinter.PrintFullText( iXPos, iYPos+245, TAT_CENTER );
	kPrinter.ClearList();
}

void FightClubFightNPCContinueWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if( m_bRequestServer )
		return;

	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_RETRY_BTN:
		if( cmd == IOBN_BTNUP && (m_ChampName == g_MyInfo.GetPublicID()) )
		{
			m_bRequestServer = true;

			SP2Packet kPacket( CTPK_FIGHT_NPC );
			kPacket << FIGHT_NPC_RETRY;
			TCPNetwork::SendToServer( kPacket );
		}
		break;
	case ID_ROOM_BTN:
		if( cmd == IOBN_BTNUP && (m_ChampName == g_MyInfo.GetPublicID()) )
		{
			m_bRequestServer = true;

			SP2Packet kPacket( CTPK_FIGHT_NPC );
			kPacket << FIGHT_NPC_END;
			TCPNetwork::SendToServer( kPacket );
		}
		break;
	}
}