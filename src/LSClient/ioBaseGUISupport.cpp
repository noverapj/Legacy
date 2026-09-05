#pragma once
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioBaseGUISupport.h"

#include "ioMagicCircle.h"
#include "ioGuildMarkMgr.h"
#include "ioLadderTeamMgr.h"
#include "ioCharEmoticon.h"
#include "ioShuffleModeUIRender.h"
#include "ioNpcChar.h"

#include "ioFlagCaptureMode.h"

ioOwnerMark * ioBaseGUISupport::m_pOwnerMark = NULL;
bool ioBaseGUISupport::m_bTeamCircleCheck;

DWORD ioBaseGUISupport::m_dwGaugeTic;
DWORD ioBaseGUISupport::m_dwAlphaTic;

//사용 안함
ioHashString ioBaseGUISupport::m_StartEmoticon;
ioHashString ioBaseGUISupport::m_ProtectEmoticon;
ioHashString ioBaseGUISupport::m_ExitRoomEmoticon;
ioHashString ioBaseGUISupport::m_EnablePickEmoticon;
ioHashString ioBaseGUISupport::m_ResistanceEmoticon;
ioHashString ioBaseGUISupport::m_NoneGaugeEmoticon;
ioHashString ioBaseGUISupport::m_ProtectWound;
ioHashString ioBaseGUISupport::m_DefenseBreakEmoticon;

//사용중
ioHashString ioBaseGUISupport::m_KillEmoticon;
ioHashStringVec ioBaseGUISupport::m_KillComboEmoticonList;
ioHashString ioBaseGUISupport::m_KillComboMaxEmoticon;
ioHashString ioBaseGUISupport::m_DeathEmoticon;
ioHashString ioBaseGUISupport::m_HitBlueIcon;
ioHashString ioBaseGUISupport::m_HitRedIcon;
ioHashString ioBaseGUISupport::m_MaxComboEmotionRed;
ioHashString ioBaseGUISupport::m_MaxComboEmotionBlue;
ioHashStringVec ioBaseGUISupport::m_IconComboNumberList;
ioHashStringVec ioBaseGUISupport::m_ComboEmotionRedList;
ioHashStringVec ioBaseGUISupport::m_ComboEmotionBlueList;
ioHashString ioBaseGUISupport::m_IconComboMax;
ioHashString ioBaseGUISupport::m_DisableAttackEmoticon;
ioHashString ioBaseGUISupport::m_ScreenShotEmoticon;
ioHashString ioBaseGUISupport::m_HomePageEmoticon;
ioHashStringVec ioBaseGUISupport::m_OwnerRankEmoticon;
ioHashStringVec ioBaseGUISupport::m_OtherRankEmoticon;
ioHashString ioBaseGUISupport::m_CoinEventEmoticon;
ioHashString ioBaseGUISupport::m_BossEmoticon;

// PvE 이모티콘
ioHashString ioBaseGUISupport::m_TargetCommandEmoticon;
ioHashString ioBaseGUISupport::m_TargetingEmoticon;

ioBaseGUISupport::ioBaseGUISupport( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode, ioBaseChar* pOwner )
 : m_pGrp( pGrp ), m_pCreator( pCreator ), m_pMode( pMode ), m_pOwner( pOwner )
{
	m_pTargetMarker			= NULL;
	m_pBackUpMarker			= NULL;
	m_pTargetMarkerOther	= NULL;
	m_MultipleTargetMax		= 0;

	m_pMagicCircle = NULL;

	m_pTeamCircle = NULL;
	m_pGauge = NULL;
	m_bDisableGauge = false;

	m_bVisibleBottomCircle = true;

	// For Guage Alpha
	m_bGaugeAlpha = false;
	m_dwGaugeCheckTime = 0;

	m_iHeroTitle = 0;

	m_pFlagPointGauge = NULL;
	m_bEnableFlagPointGauge = false;

	m_iMonsterRewardExp = 0;
	m_iMonsterRewardPeso= 0;
	m_dwMonsterRewardTime = 0;

	m_pRunningManTitleLeft   = NULL;
	m_pRunningManTitleCenter = NULL;
	m_pRunningManTitleRight  = NULL;

	m_pCharEmoticon = new ioCharEmoticon;
	m_pCharChatBubble = new ioCharChatBubble;
	m_pShuffleCharUI = new ioShuffleCharStateUI;

	m_dwMovieCapStartCurTime = 0;
	m_dwMovieCapStartTime    = 0;
	m_iMovieStartSec = -1;
	m_dwMovieCapturingCurTime = 0;

	m_dwNamedTitleCode = 0;
	m_nNamedTitleLevel = 0;
	m_byNamedTitlePremium = 0;
	m_pNamedTitleMark = NULL;
}

ioBaseGUISupport::~ioBaseGUISupport()
{
	if( m_pTargetMarker )
		m_pTargetMarker->DestroyMarker( m_pCreator );
	SAFEDELETE( m_pTargetMarker );	

	if( m_pBackUpMarker )
		m_pBackUpMarker->DestroyMarker( m_pCreator );
	SAFEDELETE( m_pBackUpMarker );

	if( m_pTargetMarkerOther )
		m_pTargetMarkerOther->DestroyMarker( m_pCreator );
	SAFEDELETE( m_pTargetMarkerOther );

	DestroyTeamCircle();

	SAFEDELETE( m_pFlagPointGauge );

	SAFEDELETE( m_pCharEmoticon );
	SAFEDELETE( m_pCharChatBubble );
	SAFEDELETE( m_pShuffleCharUI );
	SAFEDELETE( m_pNamedTitleMark );
}

//Load INI
void ioBaseGUISupport::_InitStaticINI_GUIValue( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	int i;

	rkLoader.SetTitle_e( "hp_gauge" );
	m_dwGaugeTic = rkLoader.LoadInt_e( "gauge_tic", 500 );
	m_dwAlphaTic = rkLoader.LoadInt_e( "alpha_tic", 300 );

	rkLoader.SetTitle_e( "common" );

	//Emoticon
	m_StartEmoticon = szBuf;
	rkLoader.LoadString_e( "protect_emoticon", "", szBuf, MAX_PATH );
	m_ProtectEmoticon = szBuf;
	rkLoader.LoadString_e( "exit_room_emoticon", "", szBuf, MAX_PATH );
	m_ExitRoomEmoticon = szBuf;
	rkLoader.LoadString_e( "kill_emoticon", "", szBuf, MAX_PATH );
	m_KillEmoticon = szBuf;
	m_KillComboEmoticonList.clear();
	for( i=0 ; i<100 ; ++i )
	{
		ioHashString szIconName;
		ZeroMemory( szKey, MAX_PATH );
		ZeroMemory( szBuf, MAX_PATH );
		wsprintf_e( szKey, "kill_combo_emoticon%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szIconName = szBuf;
		if ( szIconName.IsEmpty() )
			break;

		m_KillComboEmoticonList.push_back( szIconName );
	}

	rkLoader.LoadString_e( "kill_combo_max_emoticon", "", szBuf, MAX_PATH );
	m_KillComboMaxEmoticon = szBuf;
	rkLoader.LoadString_e( "death_emoticon", "", szBuf, MAX_PATH );
	m_DeathEmoticon = szBuf;
	rkLoader.LoadString_e( "enable_pick_item_emoticon", "", szBuf, MAX_PATH );
	m_EnablePickEmoticon = szBuf;

	rkLoader.LoadString_e( "hit_emoticon_blue", "", szBuf, MAX_PATH );
	m_HitBlueIcon = szBuf;
	rkLoader.LoadString_e( "hit_emoticon_red", "", szBuf, MAX_PATH );
	m_HitRedIcon = szBuf;

	//rkLoader.LoadString_e( "combo_emoticon_blue", "", szBuf, MAX_PATH );
	//m_ComboBlueIcon = szBuf;
	//rkLoader.LoadString_e( "combo_emoticon_red", "", szBuf, MAX_PATH );
	//m_ComboRedIcon = szBuf;
	
	//rkLoader.LoadString_e( "combo_number_emoticon", "", szBuf, MAX_PATH );
	//m_IconComboNumber = szBuf;
	
	rkLoader.LoadString_e( "max_combo_emoticon_blue", "", szBuf, MAX_PATH );
	m_MaxComboEmotionBlue = szBuf;
	rkLoader.LoadString_e( "max_combo_emoticon_red", "", szBuf, MAX_PATH );
	m_MaxComboEmotionRed = szBuf;
	
	m_IconComboNumberList.clear();
	for( i=0 ; i<1000 ; ++i )
	{
		ioHashString szIconName;
		ZeroMemory( szKey, MAX_PATH );
		ZeroMemory( szBuf, MAX_PATH );
		wsprintf_e( szKey, "combo_number_emoticon%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szIconName = szBuf;
		if ( szIconName.IsEmpty() )
			break;
		m_IconComboNumberList.push_back( szIconName );
	}

	m_ComboEmotionRedList.clear();
	for( i=0 ; i<1000 ; ++i )
	{
		ioHashString szIconName;
		ZeroMemory( szKey, MAX_PATH );
		ZeroMemory( szBuf, MAX_PATH );
		wsprintf_e( szKey, "combo_emoticon_red%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szIconName = szBuf;
		if ( szIconName.IsEmpty() )
			break;

		m_ComboEmotionRedList.push_back( szIconName );
	}

	m_ComboEmotionBlueList.clear();
	for( i=0 ; i<1000 ; ++i )
	{
		ioHashString szIconName;
		ZeroMemory( szKey, MAX_PATH );
		ZeroMemory( szBuf, MAX_PATH );
		wsprintf_e( szKey, "combo_emoticon_blue%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szIconName = szBuf;
		if ( szIconName.IsEmpty() )
			break;

		m_ComboEmotionBlueList.push_back( szIconName );
	}

	rkLoader.LoadString_e( "combo_number_max_emoticon", "", szBuf, MAX_PATH );
	m_IconComboMax = szBuf;
	rkLoader.LoadString_e( "resistance_emoticon", "", szBuf, MAX_PATH );
	m_ResistanceEmoticon = szBuf;
	rkLoader.LoadString_e( "disable_attack_emoticon", "", szBuf, MAX_PATH );
	m_DisableAttackEmoticon = szBuf;
	rkLoader.LoadString_e( "none_gauge_emoticon", "", szBuf, MAX_PATH );
	m_NoneGaugeEmoticon = szBuf;
	rkLoader.LoadString_e( "screenshot_emoticon", "", szBuf, MAX_PATH );
	m_ScreenShotEmoticon = szBuf;
	rkLoader.LoadString_e( "homepage_emoticon", "", szBuf, MAX_PATH );
	m_HomePageEmoticon = szBuf;
	rkLoader.LoadString_e( "protect_wound_emoticon", "", szBuf, MAX_PATH );
	m_ProtectWound = szBuf;
	rkLoader.LoadString_e( "coin_event_emoticon", "", szBuf, MAX_PATH );
	m_CoinEventEmoticon = szBuf;
	rkLoader.LoadString_e( "defense_break_emoticon", "", szBuf, MAX_PATH );
	m_DefenseBreakEmoticon = szBuf;
	rkLoader.LoadString_e( "boss_emoticon", "", szBuf, MAX_PATH );
	m_BossEmoticon = szBuf;

	// PvE 이모티콘
	rkLoader.LoadString_e( "target_command_emoticon", "", szBuf, MAX_PATH );
	m_TargetCommandEmoticon = szBuf;
	rkLoader.LoadString_e( "targeting_emoticon", "", szBuf, MAX_PATH );
	m_TargetingEmoticon = szBuf;;

	m_OwnerRankEmoticon.clear();
	m_OwnerRankEmoticon.reserve(MAX_PLAYER);

	rkLoader.LoadString_e( "owner_rank_emoticon", "", szKey, MAX_PATH );
	ioHashString szOwnerRank = szKey;

	if( !szOwnerRank.IsEmpty() )
	{
		for( i=0; i < MAX_PLAYER; ++i )
		{
			if( i < 10 )
				wsprintf_e( szBuf, "%s_0%d", szOwnerRank.c_str(), i+1 );
			else
				wsprintf_e( szBuf, "%s_%d", szOwnerRank.c_str(), i+1 );

			m_OwnerRankEmoticon.push_back( szBuf );
		}
	}

	m_OtherRankEmoticon.clear();
	m_OtherRankEmoticon.reserve(MAX_PLAYER);

	rkLoader.LoadString_e( "other_rank_emoticon", "", szKey, MAX_PATH );
	ioHashString szOtherRank = szKey;

	if( !szOtherRank.IsEmpty() )
	{
		for( i=0; i < MAX_PLAYER; ++i )
		{
			if( i < 10 )
				wsprintf_e( szBuf, "%s_0%d", szOtherRank.c_str(), i+1 );
			else
				wsprintf_e( szBuf, "%s_%d", szOtherRank.c_str(), i+1 );

			m_OtherRankEmoticon.push_back( szBuf );
		}
	}

	//Check TeamCircle
	m_bTeamCircleCheck = rkLoader.LoadBool_e( "team_circle_check", false );

	ioCharEmoticon::LoadStaticValues();
	ioCharChatBubble::LoadStaticValues();


}

void ioBaseGUISupport::InitINI_GUIValue( ioINILoader &rkLoader )
{
	if( m_pShuffleCharUI && !IsNpcChar( m_pOwner ) )
		m_pShuffleCharUI->Load();
}


// TargetMarker ------------------------------------------------------------------------------------------------------------------------------------------------------

ioTargetMarker* ioBaseGUISupport::CreateMarkerTemplete( ioTargetMarker::MarkerType eMarkType )
{
	ioTargetMarker* pMarker = NULL;
	switch( eMarkType )
	{
	case ioTargetMarker::MT_FIRE_ITEM:
		pMarker = new ioFireItemMarker( m_pOwner );
		break;
	case ioTargetMarker::MT_HEAL_ITEM:
		pMarker = new ioHealItemMarker( m_pOwner );
		break;
	case ioTargetMarker::MT_BOW_ITEM:
		pMarker = new ioBowItemMarker( m_pOwner );
		break;
	case ioTargetMarker::MT_CROSS_BOW:
		pMarker = new ioCrossBowMarker( m_pOwner );
		break;
	case ioTargetMarker::MT_OTHER_MARKER:
		pMarker = new ioOtherMarker( m_pOwner );
		break;
	default:
		pMarker = NULL;
		break;
	}

	return pMarker;
}

void ioBaseGUISupport::BackupCurrMarker()
{
	if( m_pTargetMarker )
	{
		m_pBackUpMarker	= m_pTargetMarker;
		m_pTargetMarker	= NULL;
	}
}

void ioBaseGUISupport::RestoreBackupMarker()
{
	if( m_pBackUpMarker )
	{
		m_pTargetMarker = m_pBackUpMarker;
		m_pBackUpMarker	= NULL;
	}
}

void ioBaseGUISupport::SetTargetMarker( ioTargetMarker::MarkerType eMarkType,
										const ioHashString& szReadyEffect,
										const ioHashString& szAimingEffect,
										const ioHashString& szAimedEffect,
										const ioHashString& szZeroAimedEffect,
										const ioHashString& szHeadAimedEffect,
										TargetedType eType /*= TT_SINGLE */ )
{
	ioTargetMarker* pMarker = NULL;
	switch( eType )
	{
	case TT_SINGLE:
		{
			if( m_pTargetMarker )
				m_pTargetMarker->DestroyMarker( m_pCreator );


			SAFEDELETE( m_pTargetMarker );
			m_pTargetMarker = CreateMarkerTemplete( eMarkType );
			

			if( m_pTargetMarker )
				m_pTargetMarker->SetMarkerEffect( szReadyEffect, szAimingEffect, szAimedEffect, szZeroAimedEffect, szHeadAimedEffect );
		}
		break;
	case TT_OTHER:
		{
			if( m_pTargetMarkerOther )
				m_pTargetMarkerOther->DestroyMarker( m_pCreator );

			SAFEDELETE( m_pTargetMarkerOther );
			m_pTargetMarkerOther = CreateMarkerTemplete( eMarkType );

			if( m_pTargetMarkerOther )
				m_pTargetMarkerOther->SetMarkerEffect( szReadyEffect, szAimingEffect, szAimedEffect, szZeroAimedEffect, szHeadAimedEffect );
		}
		break;
	}
}

void ioBaseGUISupport::SetSpecialMarker( const ioHashString &szSpecialAimedEffect )
{
	if( m_pTargetMarker )
		m_pTargetMarker->SetSpecialMarker( szSpecialAimedEffect );
}

void ioBaseGUISupport::SetMultipleTargetMarker( ioTargetMarker::MarkerType eMarkType, 
												const ioHashString& szReadyEffect,
												const ioHashString& szAimingEffect,
												const ioHashString& szAimedEffect,
												const ioHashString& szZeroAimedEffect,
												const ioHashString& szHeadAimedEffect,
												int					iMultipleTargetMax )
{
	for( ioMultipleTargetMarker::iterator iter = m_MultipleTargetMarker.begin(); iter != m_MultipleTargetMarker.end(); ++iter )
	{
		ioTargetMarker* pMarker = *iter;
		if( pMarker )
			pMarker->DestroyMarker( m_pCreator );

		SAFEDELETE( pMarker );
	}
	m_MultipleTargetMarker.clear();
	m_MultipleTargetMax	= iMultipleTargetMax;

	for( int i = 0; i < iMultipleTargetMax; ++i )
	{
		ioTargetMarker* pMarker = CreateMarkerTemplete( eMarkType );
		if( pMarker )
		{	
			pMarker->SetMarkerEffect( szReadyEffect, szAimingEffect, szAimedEffect, szZeroAimedEffect,szHeadAimedEffect );
			m_MultipleTargetMarker.push_back( pMarker );
		}
	}
}

void ioBaseGUISupport::SetTargetMarkerEnable( bool bEnable, TargetedType eType /*= TT_SINGLE */ )
{
	switch( eType )
	{
	case TT_SINGLE:
		if( m_pTargetMarker )
		{
			m_pTargetMarker->SetUseEnable( bEnable );
		}
		break;
	case TT_OTHER:
		if( m_pTargetMarkerOther )
		{
			m_pTargetMarkerOther->SetUseEnable( bEnable );
		}
		break;
	}
}

void ioBaseGUISupport::SetMultipleTargetMarkerEnable( bool bEnable, int iMarkerIndex )
{
	if( COMPARE( iMarkerIndex, 0, (int)m_MultipleTargetMarker.size() ) )
	{
		ioTargetMarker* pMarker = m_MultipleTargetMarker[iMarkerIndex];
		if( pMarker )
			pMarker->SetUseEnable( bEnable );
	}
}

void ioBaseGUISupport::SetTargetMarkerState( ioTargetMarker::MarkerState eState, TargetedType eType /*= TT_SINGLE */ )
{
	switch( eType )
	{
	case TT_SINGLE:
		if( m_pTargetMarker )
		{
			m_pTargetMarker->SetMarkerState( m_pCreator, eState );
		}
		break;
	case TT_OTHER:
		if( m_pTargetMarkerOther )
		{
			m_pTargetMarkerOther->SetMarkerState( m_pCreator, eState );
		}
		break;
	}
}

void ioBaseGUISupport::SetMultipleTargetMarkerState( ioTargetMarker::MarkerState eState, int iMarkerIndex )
{
	if( COMPARE( iMarkerIndex, 0, (int)m_MultipleTargetMarker.size() ) )
	{
		ioTargetMarker* pMarker = m_MultipleTargetMarker[iMarkerIndex];
		if( pMarker )
			pMarker->SetMarkerState( m_pCreator, eState );
	}
}

void ioBaseGUISupport::SetTargetMarkerTarget( const ioHashString &szTarget, bool bBall, TargetedType eType /*= TT_SINGLE */ )
{
	switch( eType )
	{
	case TT_SINGLE:
		if( m_pTargetMarker )
		{
			m_pTargetMarker->SetTarget( szTarget, bBall );
		}
		break;
	case TT_OTHER:
		if( m_pTargetMarkerOther )
		{
			m_pTargetMarkerOther->SetTarget( szTarget, bBall );
		}
		break;
	}
}

int ioBaseGUISupport::SetMultipleTargetMarkerTarget( const ioHashString &szTarget, bool bBall )
{
	int iMarkerIndex = 0;
	for( ioMultipleTargetMarker::iterator iter = m_MultipleTargetMarker.begin(); iter != m_MultipleTargetMarker.end(); ++iter, ++iMarkerIndex )
	{
		ioTargetMarker* pMarker = *iter;
		
		if( pMarker->GetTargetChar() && pMarker->GetTargetChar()->GetCharName() == szTarget )
		{
			return iMarkerIndex;
		}

		if( pMarker && !pMarker->IsTarget() )
		{
			pMarker->SetTarget( szTarget, bBall );			
			return iMarkerIndex;
		}
	}

	return NONE_TARGET;
}

void ioBaseGUISupport::SetTargetMarkerRange( float fRange, TargetedType eType /*= TT_SINGLE */ )
{
	switch( eType )
	{
	case TT_SINGLE:
		if( m_pTargetMarker )
		{
			m_pTargetMarker->SetMarkerRange( fRange );
		}
		break;
	case TT_OTHER:
		if( m_pTargetMarkerOther )
		{
			m_pTargetMarkerOther->SetMarkerRange( fRange );
		}
		break;
	}
}

void ioBaseGUISupport::SetTargetMarkerYOffset( float fYOffset, TargetedType eType /*= TT_SINGLE */ )
{
	switch( eType )
	{
	case TT_SINGLE:
		if( m_pTargetMarker )
		{
			m_pTargetMarker->SetMarkerYOffset( fYOffset );
		}
		break;
	case TT_OTHER:
		if( m_pTargetMarkerOther )
		{
			m_pTargetMarkerOther->SetMarkerYOffset( fYOffset );
		}
		break;
	}
}

void ioBaseGUISupport::SetMultipleTargetMarkerRange( float fRange )
{
	for( ioMultipleTargetMarker::iterator iter = m_MultipleTargetMarker.begin(); iter != m_MultipleTargetMarker.end(); ++iter )
	{
		ioTargetMarker* pMarker = *iter;
		if( pMarker )
			pMarker->SetMarkerRange( fRange );
	}
}

void ioBaseGUISupport::SetMultipleTargetMarkerRange( float fRange, int iMarkerIndex )
{
	if( COMPARE( iMarkerIndex, 0, (int)m_MultipleTargetMarker.size() ) )
	{
		ioTargetMarker* pMarker = m_MultipleTargetMarker[iMarkerIndex];
		if( pMarker )
			pMarker->SetMarkerRange( fRange );
	}
}

void ioBaseGUISupport::SetTargetMarkerJumpAngle( float fAngle, TargetedType eType /*= TT_SINGLE */ )
{
	switch( eType )
	{
	case TT_SINGLE:
		if( m_pTargetMarker )
		{
			m_pTargetMarker->SetJumpMarkerAngle( fAngle );
		}
		break;
	case TT_OTHER:
		if( m_pTargetMarkerOther )
		{
			m_pTargetMarkerOther->SetJumpMarkerAngle( fAngle );
		}
		break;
	}
}

void ioBaseGUISupport::SetMultipleTargetMarkerJumpAngle( float fAngle, int iMarkerIndex )
{
	if( COMPARE( iMarkerIndex, 0, (int)m_MultipleTargetMarker.size() ) )
	{
		ioTargetMarker* pMarker = m_MultipleTargetMarker[iMarkerIndex];
		if( pMarker )
			pMarker->SetJumpMarkerAngle( fAngle );
	}
}

bool ioBaseGUISupport::IsMultipleTargetMarkerTargeted( const ioHashString &szTarget )
{
	for( ioMultipleTargetMarker::iterator iter = m_MultipleTargetMarker.begin(); iter != m_MultipleTargetMarker.end(); ++iter )
	{
		ioTargetMarker* pMarker = *iter;

		if( pMarker->GetTargetChar() && pMarker->GetTargetChar()->GetCharName() == szTarget )
		{
			return true;
		}	
	}

	return false;
}

int ioBaseGUISupport::GetMultipleTargetMarkerIndex( const ioHashString &szTarget )
{
	int iMarkerIndex = 0;
	for( ioMultipleTargetMarker::iterator iter = m_MultipleTargetMarker.begin(); iter != m_MultipleTargetMarker.end(); ++iter, ++iMarkerIndex )
	{
		ioTargetMarker* pMarker = *iter;

		if( pMarker->GetTargetChar() && pMarker->GetTargetChar()->GetCharName() == szTarget )
		{
			return iMarkerIndex;
		}
	}

	return NONE_TARGET;
}

void ioBaseGUISupport::UpdateMarker()
{
	if( m_pTargetMarker && m_pTargetMarker->GetMarkerState() != ioTargetMarker::MS_NONE )
		m_pTargetMarker->Update( m_pCreator );

	if( m_pTargetMarkerOther && m_pTargetMarkerOther->GetMarkerState() != ioTargetMarker::MS_NONE )
		m_pTargetMarkerOther->Update( m_pCreator );

	for( ioMultipleTargetMarker::iterator iter = m_MultipleTargetMarker.begin(); iter != m_MultipleTargetMarker.end(); ++iter )
	{
		ioTargetMarker* pMarker = *iter;
		if( pMarker && pMarker->GetMarkerState() != ioTargetMarker::MS_NONE )
			pMarker->Update( m_pCreator );
	}
}

void ioBaseGUISupport::ClearTargetMarker()
{
	if( m_pTargetMarker )
		m_pTargetMarker->SetMarkerState( m_pCreator, ioTargetMarker::MS_NONE );

	ClearMultipleTargetMarker();
}

void ioBaseGUISupport::ClearMultipleTargetMarker()
{
	for( ioMultipleTargetMarker::iterator iter = m_MultipleTargetMarker.begin(); iter != m_MultipleTargetMarker.end(); ++iter )
	{
		ioTargetMarker* pMarker = *iter;
		if( pMarker )
			pMarker->SetMarkerState( m_pCreator, ioTargetMarker::MS_NONE );
	}
}

ioTargetMarker::MarkerState ioBaseGUISupport::GetMarkerState( TargetedType eType /*= TT_SINGLE */ )
{
	switch( eType )
	{
	case TT_SINGLE:
		if( m_pTargetMarker )
		{
			return m_pTargetMarker->GetMarkerState();
		}
		break;
	case TT_OTHER:
		if( m_pTargetMarkerOther )
		{
			return m_pTargetMarkerOther->GetMarkerState();
		}
		break;
	}

	return ioTargetMarker::MS_NONE;
}

ioTargetMarker::MarkerState ioBaseGUISupport::GetMultipleMarkerState( int iMarkerIndex )
{
	if( COMPARE( iMarkerIndex, 0, (int)m_MultipleTargetMarker.size() ) )
	{
		ioTargetMarker* pMarker = m_MultipleTargetMarker[iMarkerIndex];
		if( pMarker )
			return pMarker->GetMarkerState();
	}

	return ioTargetMarker::MS_NONE;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioBaseGUISupport::CreateMagicCircle( const ioHashString &ReadyCircle,
	const ioHashString &EnableCircle,
	const ioHashString &GuidCircle,
	float fOffSet )
{
	DestroyMagicCircle();

	if( ReadyCircle.IsEmpty() || EnableCircle.IsEmpty() )
		return;

	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vOffSet = m_pOwner->GetMoveDir() * fOffSet;

	vPos.y = 0;
	vPos += vOffSet;
	vPos.y = m_pCreator->GetMapHeight( vPos.x, vPos.z );
	if( vPos.y <= 0.0f )
	{
		vPos.y = m_pOwner->GetBottomHeight();
	}

	m_pCreator->CheckAxisLimit( vPos );

	m_pMagicCircle = new ioMagicCircle;
	m_pMagicCircle->InitResource( ReadyCircle, EnableCircle, GuidCircle, vPos, m_pCreator );
	m_pMagicCircle->SetVisible( false );
	m_pMagicCircle->SetTargetRot( m_pOwner->GetTargetRot() );
	m_pMagicCircle->UpdatePos( false, vPos, m_pOwner, m_pCreator );
	g_TutorialMgr.SetEnableMagicCircleMoving( true );
}

void ioBaseGUISupport::CreateMagicCircle( const ioHashString &ReadyCircle,
	const ioHashString &EnableCircle,
	const ioHashString &GuidCircle,
	D3DXVECTOR3 vPos,
	float fOffSet )
{
	DestroyMagicCircle();

	if( ReadyCircle.IsEmpty() || EnableCircle.IsEmpty() )
		return;

	D3DXVECTOR3 vOffSet = m_pOwner->GetMoveDir() * fOffSet;

	vPos.y = 0;
	vPos += vOffSet;
	vPos.y = m_pCreator->GetMapHeight( vPos.x, vPos.z );
	if( vPos.y <= 0.0f )
	{
		vPos.y = m_pOwner->GetBottomHeight();
	}

	m_pCreator->CheckAxisLimit( vPos );

	m_pMagicCircle = new ioMagicCircle;
	m_pMagicCircle->InitResource( ReadyCircle, EnableCircle, GuidCircle, vPos, m_pCreator );
	m_pMagicCircle->SetVisible( false );
	m_pMagicCircle->SetTargetRot( m_pOwner->GetTargetRot() );
	m_pMagicCircle->UpdatePos( false, vPos, m_pOwner, m_pCreator );
	g_TutorialMgr.SetEnableMagicCircleMoving( true );
}

void ioBaseGUISupport::CreateMagicCircleForBlock( const ioHashString &ReadyCircle,
	const ioHashString &EnableCircle,
	const ioHashString &GuidCircle,
	float fOffSet,
	bool bHeightCheck )
{
	DestroyMagicCircle();

	if( ReadyCircle.IsEmpty() )
		return;

	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vOffSet = m_pOwner->GetMoveDir() * fOffSet;

	vPos.y = 0;
	vPos += vOffSet;
	if( bHeightCheck )
		vPos.y = m_pCreator->GetMapHeight( vPos.x, vPos.z );
	if( vPos.y <= 0.0f )
	{
		vPos.y = m_pOwner->GetBottomHeight();
	}

	m_pMagicCircle = new ioMagicCircle;
	m_pMagicCircle->InitResource( ReadyCircle, EnableCircle, GuidCircle, vPos, m_pCreator );
	m_pMagicCircle->SetVisible( false );
	m_pMagicCircle->SetTargetRot( m_pOwner->GetTargetRot() );

	ioMapEffect *pMapEffect = m_pCreator->FindMapEffect( m_pMagicCircle->GetReadyCircelID() );
	if( pMapEffect )
	{
		ioEffect *pEffect = pMapEffect->GetEffect();
		if( pEffect && bHeightCheck )
		{
			vPos.y = m_pCreator->GetMapHeightForBlockGuid( vPos, pEffect->GetCollisionBox() );
		}
	}

	if( vPos.y <= 0.0f )
	{
		vPos.y = m_pOwner->GetBottomHeight();
	}

	m_pMagicCircle->UpdatePos( false, vPos, m_pOwner, m_pCreator );
}

void ioBaseGUISupport::DestroyMagicCircle()
{
	if( m_pMagicCircle )
		m_pMagicCircle->DestroyCircle( m_pCreator );

	SAFEDELETE( m_pMagicCircle );
}

D3DXVECTOR3 ioBaseGUISupport::GetMagicCircleStartPos()
{
	return m_pMagicCircle->GetStartPos();
}

D3DXVECTOR3 ioBaseGUISupport::GetMagicCirclePos( float fOffSet, bool bNoChangePos ) const
{
	if( m_pMagicCircle )
		return m_pMagicCircle->GetCurPos();

	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vOffSet = m_pOwner->GetMoveDir() * fOffSet;

	vPos.y = 0;
	vPos += vOffSet;

	if( !bNoChangePos )
	{
		vPos.y = m_pCreator->GetMapHeight( vPos.x, vPos.z );
		if( vPos.y <= 0.0f )
		{
			vPos.y = m_pOwner->GetWorldPosition().y;
		}
	}
	else
	{
		vPos.y = m_pOwner->GetWorldPosition().y;
	}

	return vPos;
}

D3DXVECTOR3 ioBaseGUISupport::GetMagicCirclePos( float fOffSet, float fYOffSet ) const
{
	if( m_pMagicCircle )
		return m_pMagicCircle->GetCurPos();

	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vOffSet = m_pOwner->GetMoveDir() * fOffSet;

	vPos.y = 0;
	vPos += vOffSet;

	vPos.y = m_pOwner->GetWorldPosition().y + fYOffSet;

	return vPos;
}

D3DXVECTOR3 ioBaseGUISupport::GetMagicCirclePrePos()
{
	return m_pMagicCircle->GetPrePos();
}

D3DXQUATERNION ioBaseGUISupport::GetMagicCircleRot()
{
	return m_pMagicCircle->GetTargetRot();
}

void ioBaseGUISupport::ChangeMagicCircle( bool bVisible )
{
	if( m_pMagicCircle )
	{
		m_pMagicCircle->SetVisible( bVisible );
	}
}

void ioBaseGUISupport::CheckMagicCircleVisible( bool bEnableRender )
{
	if( m_pMagicCircle )
	{
		m_pMagicCircle->SetEnableRender( bEnableRender );
	}
}

void ioBaseGUISupport::UpdateMagicCirclePos( float fMoveSpeed, float fRange, float fHeightGap, bool bEnableSkill, bool bNetwork )
{
	if( !m_pMagicCircle ) 
		return;

	if( m_pMagicCircle->GetStartPos() != m_pMagicCircle->GetCurPos() )
		g_TutorialMgr.ActionMagicCircleMoving();

	if( bNetwork )
	{
		if( m_pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_UPDATE_MAGIC_CIRCLE );
			kPacket << m_pOwner->GetCharName();
			kPacket << bEnableSkill;
			kPacket << m_pMagicCircle->GetCurPos();
			kPacket << false;		// Set Visible Time
			kPacket << false;		// CharInfo
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		if( m_pOwner->IsOwnerChar() )
			m_pMagicCircle->UpdatePos( bEnableSkill, m_pMagicCircle->GetCurPos(), m_pOwner, m_pCreator );

		return;
	}

	if( !m_pOwner->IsNeedProcess() )
	{
		//m_pMagicCircle->UpdatePos( bEnableSkill, m_pMagicCircle->GetCurPos(), m_pOwner, m_pCreator );
		return;
	}

	float fMapHeight = 0.0f;
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3 vPos, vPrePos, vCurPos;

	bool bSend = false;
	// 사신 머리 스킬 테스트
	ioBaseChar *pTargetChar = m_pCreator->GetBaseChar( GetMagicCircleTarget() );
	if( pTargetChar )
	{
		qtRotate= pTargetChar->GetWorldOrientation();
		vPos    = pTargetChar->GetWorldPosition();
		vCurPos = m_pMagicCircle->GetCurPos();
		vPrePos = vCurPos;
		vCurPos = vPos;	
	}
	else
	{
		ioUserKeyInput::DirKeyInput eDir = m_pOwner->CheckCurDirKey();
		if( eDir == ioUserKeyInput::DKI_NONE )
		{
			vCurPos = m_pMagicCircle->GetCurPos();
			fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
			if( fMapHeight > 0.0f )
			{
				vCurPos.y = fMapHeight;
			}
		}
		else
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

			vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vPos *= fMoveSpeed * g_FrameTimer.GetSecPerFrame();
			vCurPos = m_pMagicCircle->GetCurPos();
			vPrePos = vCurPos;
			vCurPos += vPos;
		}

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
		{
			vCurPos.y = fMapHeight;
		}
	}	

	D3DXVECTOR3 vCharPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vCurXZ = vCurPos;

	vCharPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vCharPos;

	if( D3DXVec3Length( &vDiff ) > fRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vCharPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	m_pMagicCircle->UpdatePos( bEnableSkill, vCurPos, m_pOwner, m_pCreator );
	m_pMagicCircle->SetTargetRot( qtRotate );
}

void ioBaseGUISupport::UpdateMagicCirclePos( float fMoveSpeed, float fMinRange, float fMaxRange, float fHeightGap, bool bEnableSkill, D3DXVECTOR3 vStartPos, bool bNetwork )
{
	if( !m_pMagicCircle ) 
		return;

	if( m_pMagicCircle->GetStartPos() != m_pMagicCircle->GetCurPos() )
		g_TutorialMgr.ActionMagicCircleMoving();

	if( bNetwork )
	{
		if( m_pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_UPDATE_MAGIC_CIRCLE );
			kPacket << m_pOwner->GetCharName();
			kPacket << bEnableSkill;
			kPacket << m_pMagicCircle->GetCurPos();
			kPacket << false;		// Set Visible Time
			kPacket << false;		// CharInfo
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		if( m_pOwner->IsOwnerChar() )
			m_pMagicCircle->UpdatePos( bEnableSkill, m_pMagicCircle->GetCurPos(), m_pOwner, m_pCreator );

		return;
	}

	if( !m_pOwner->IsNeedProcess() )
	{
		//m_pMagicCircle->UpdatePos( bEnableSkill, m_pMagicCircle->GetCurPos(), m_pOwner, m_pCreator );
		return;
	}

	float fMapHeight = 0.0f;
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3 vPos, vPrePos, vCurPos;

	bool bSend = false;
	// 사신 머리 스킬 테스트
	ioBaseChar *pTargetChar = m_pCreator->GetBaseChar( GetMagicCircleTarget() );
	if( pTargetChar )
	{
		qtRotate= pTargetChar->GetWorldOrientation();
		vPos    = pTargetChar->GetWorldPosition();
		vCurPos = m_pMagicCircle->GetCurPos();
		vPrePos = vCurPos;
		vCurPos = vPos;	
	}
	else
	{
		ioUserKeyInput::DirKeyInput eDir = m_pOwner->CheckCurDirKey();
		if( eDir == ioUserKeyInput::DKI_NONE )
		{
			vCurPos = m_pMagicCircle->GetCurPos();
			fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
			if( fMapHeight > 0.0f )
			{
				vCurPos.y = fMapHeight;
			}
		}
		else
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

			vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vPos *= fMoveSpeed * g_FrameTimer.GetSecPerFrame();
			vCurPos = m_pMagicCircle->GetCurPos();
			vPrePos = vCurPos;
			vCurPos += vPos;
		}

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
		{
			vCurPos.y = fMapHeight;
		}
	}	

	D3DXVECTOR3 vCurXZ = vCurPos;

	vStartPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vStartPos;

	if( D3DXVec3Length( &vDiff ) < fMinRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fMinRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vStartPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	if( D3DXVec3Length( &vDiff ) > fMaxRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fMaxRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vStartPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	m_pMagicCircle->UpdatePos( bEnableSkill, vCurPos, m_pOwner, m_pCreator );
	m_pMagicCircle->SetTargetRot( qtRotate );
}

void ioBaseGUISupport::UpdateMagicCirclePos( float fMoveSpeed, float fMinRange, float fMaxRange, float fMinAngle, float fMaxAngle, float fHeightGap, bool bEnableSkill, D3DXVECTOR3 vStartPos, bool bNetwork /* = false */ )
{
	if( !m_pMagicCircle ) 
		return;

	if( m_pMagicCircle->GetStartPos() != m_pMagicCircle->GetCurPos() )
		g_TutorialMgr.ActionMagicCircleMoving();

	if( bNetwork )
	{
		if( m_pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_UPDATE_MAGIC_CIRCLE );
			kPacket << m_pOwner->GetCharName();
			kPacket << bEnableSkill;
			kPacket << m_pMagicCircle->GetCurPos();
			kPacket << false;		// Set Visible Time
			kPacket << false;		// CharInfo
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		if( m_pOwner->IsOwnerChar() )
			m_pMagicCircle->UpdatePos( bEnableSkill, m_pMagicCircle->GetCurPos(), m_pOwner, m_pCreator );

		return;
	}

	if( !m_pOwner->IsNeedProcess() )
	{
		//m_pMagicCircle->UpdatePos( bEnableSkill, m_pMagicCircle->GetCurPos(), m_pOwner, m_pCreator );
		return;
	}

	float fMapHeight = 0.0f;
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3 vPos, vPrePos, vCurPos;

	bool bSend = false;
	// 사신 머리 스킬 테스트
	ioBaseChar *pTargetChar = m_pCreator->GetBaseChar( GetMagicCircleTarget() );
	if( pTargetChar )
	{
		qtRotate= pTargetChar->GetWorldOrientation();
		vPos    = pTargetChar->GetWorldPosition();
		vCurPos = m_pMagicCircle->GetCurPos();
		vPrePos = vCurPos;
		vCurPos = vPos;	
	}
	else
	{
		ioUserKeyInput::DirKeyInput eDir = m_pOwner->CheckCurDirKey();
		if( eDir == ioUserKeyInput::DKI_NONE )
		{
			vCurPos = m_pMagicCircle->GetCurPos();
			fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
			if( fMapHeight > 0.0f )
			{
				vCurPos.y = fMapHeight;
			}
		}
		else
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

			vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vPos *= fMoveSpeed * g_FrameTimer.GetSecPerFrame();
			vCurPos = m_pMagicCircle->GetCurPos();
			vPrePos = vCurPos;
			vCurPos += vPos;

			D3DXQUATERNION qtCharRot;
			D3DXVECTOR3 vDir = m_pOwner->GetWorldPosition() - vCurPos;
			ioMath::CalcDirectionQuaternion( qtCharRot, vDir );
			float fTempYaw, fTempPitch, fTempRoll;
			ioMath::QuaterToEuler( qtCharRot, fTempYaw, fTempPitch, fTempRoll );
			float fAngle	= RADtoDEG( fTempYaw );
			if( fAngle < 0.0f )
				fAngle	= 360.0f - abs( fAngle );
			
			bool bAngleLimit = false;
			if( ( fMinAngle >= fMaxAngle && ( fAngle < fMinAngle && fAngle > fMaxAngle ) ) || ( fMinAngle < fMaxAngle && ( fAngle < fMinAngle || fAngle > fMaxAngle ) ) )
				bAngleLimit = true;

			if( bAngleLimit )
			{
				D3DXVECTOR3 vNewPos = m_pMagicCircle->GetCurPos() - m_pOwner->GetWorldPosition();
				float fLength = D3DXVec3Length( &vDir );
				D3DXVec3Normalize( &vNewPos, &vNewPos );
				vNewPos *= fLength;
				vCurPos = m_pOwner->GetWorldPosition() + vNewPos;
			}
		}

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
		{
			vCurPos.y = fMapHeight;
		}
	}	

	D3DXVECTOR3 vCurXZ = vCurPos;

	vStartPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vStartPos;

	if( D3DXVec3Length( &vDiff ) < fMinRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fMinRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vStartPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	if( D3DXVec3Length( &vDiff ) > fMaxRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fMaxRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vStartPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	m_pMagicCircle->UpdatePos( bEnableSkill, vCurPos, m_pOwner, m_pCreator );
	m_pMagicCircle->SetTargetRot( qtRotate );
}

void ioBaseGUISupport::UpdateMagicCirclePosAll( float fMoveSpeed, float fRange, float fHeightGap, bool bEnableSkill )
{
	if( !m_pMagicCircle ) 
		return;

	if( m_pMagicCircle->GetStartPos() != m_pMagicCircle->GetCurPos() )
		g_TutorialMgr.ActionMagicCircleMoving();

	float fMapHeight = 0.0f;
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3 vPos, vPrePos, vCurPos;

	bool bSend = false;
	ioUserKeyInput::DirKeyInput eDir = m_pOwner->CheckCurDirKey();
	if( eDir == ioUserKeyInput::DKI_NONE )
	{
		vCurPos = m_pMagicCircle->GetCurPos();
		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
		{
			vCurPos.y = fMapHeight;
		}
	}
	else
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

		vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos *= fMoveSpeed * g_FrameTimer.GetSecPerFrame();
		vCurPos = m_pMagicCircle->GetCurPos();
		vPrePos = vCurPos;
		vCurPos += vPos;
	}

	fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
	if( fMapHeight > 0.0f )
	{
		vCurPos.y = fMapHeight;
	}

	D3DXVECTOR3 vCharPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vCurXZ = vCurPos;

	vCharPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vCharPos;

	if( D3DXVec3Length( &vDiff ) > fRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vCharPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	m_pMagicCircle->UpdatePos( bEnableSkill, vCurPos, m_pOwner, m_pCreator );
	m_pMagicCircle->SetTargetRot( qtRotate );
}

void ioBaseGUISupport::UpdateMagicCirclePosAll( float fMoveSpeed, float fMaxRange, float fMinRange, float fHeightGap, bool bEnableSkill )
{
	if( !m_pMagicCircle ) 
		return;

	if( m_pMagicCircle->GetStartPos() != m_pMagicCircle->GetCurPos() )
		g_TutorialMgr.ActionMagicCircleMoving();

	float fMapHeight = 0.0f;
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3 vPos, vPrePos, vCurPos;

	bool bSend = false;
	ioUserKeyInput::DirKeyInput eDir = m_pOwner->CheckCurDirKey();
	if( eDir == ioUserKeyInput::DKI_NONE )
	{
		vCurPos = m_pMagicCircle->GetCurPos();
		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
		{
			vCurPos.y = fMapHeight;
		}
	}
	else
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

		vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos *= fMoveSpeed * g_FrameTimer.GetSecPerFrame();
		vCurPos = m_pMagicCircle->GetCurPos();
		vPrePos = vCurPos;
		vCurPos += vPos;
	}

	fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
	if( fMapHeight > 0.0f )
	{
		vCurPos.y = fMapHeight;
	}

	D3DXVECTOR3 vCharPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vCurXZ = vCurPos;

	vCharPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vCharPos;

	if( D3DXVec3Length( &vDiff ) > fMaxRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fMaxRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vCharPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}
	else if( D3DXVec3Length( &vDiff ) < fMinRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fMinRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vCharPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	m_pMagicCircle->UpdatePos( bEnableSkill, vCurPos, m_pOwner, m_pCreator );
	m_pMagicCircle->SetTargetRot( qtRotate );
}

void ioBaseGUISupport::UpdateMagicCircleToCirclePosAll( float fMoveSpeed,
	float fMaxRange,
	float fMinRange,
	float fHeightGap,
	float fHeightMin,
	float fHeightMax,
	D3DXVECTOR3 vCharPos,
	bool bEnableSkill,
	bool bEnableHeightGap )
{
	if( !m_pMagicCircle ) 
		return;

	if( m_pMagicCircle->GetStartPos() != m_pMagicCircle->GetCurPos() )
		g_TutorialMgr.ActionMagicCircleMoving();

	float fMapHeight = 0.0f;
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3 vPos, vPrePos, vCurPos;

	bool bSend = false;
	ioUserKeyInput::DirKeyInput eDir = m_pOwner->CheckCurDirKey();
	if( eDir == ioUserKeyInput::DKI_NONE )
	{
		vCurPos = m_pMagicCircle->GetCurPos();
		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
		{
			vCurPos.y = fMapHeight;
		}
	}
	else
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

		vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos *= fMoveSpeed * g_FrameTimer.GetSecPerFrame();
		vCurPos = m_pMagicCircle->GetCurPos();
		vPrePos = vCurPos;
		vCurPos += vPos;
	}

	fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );

	bool bDropZone = true;
	if( m_pCreator->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y + 3.0f ) > 0.0f )
		bDropZone = false;

	if( COMPARE( fMapHeight, fHeightMin, fHeightMax ) || bDropZone || !bEnableHeightGap )	
	{
		if( fMapHeight > 0.0f )
		{
			vCurPos.y = fMapHeight;
		}
	}
	else
	{		
		return;
	}

	D3DXVECTOR3 vCurXZ = vCurPos;

	vCharPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vCharPos;

	if( D3DXVec3Length( &vDiff ) > fMaxRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fMaxRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vCharPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}
	else if( D3DXVec3Length( &vDiff ) < fMinRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fMinRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vCharPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	m_pMagicCircle->UpdatePos( bEnableSkill, vCurPos, m_pOwner, m_pCreator );
	m_pMagicCircle->SetTargetRot( qtRotate );
}

void ioBaseGUISupport::UpdateMagicCirclePosForBlock( float fMoveSpeed, float fRange, float fHeightGap, bool bEnableSkill, bool bCheckHeight )
{
	if( !m_pMagicCircle ) 
		return;

	if( m_pMagicCircle->GetStartPos() != m_pMagicCircle->GetCurPos() )
		g_TutorialMgr.ActionMagicCircleMoving();

	float fMapHeight = 0.0f;
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3 vPos, vPrePos, vCurPos;

	ioMapEffect *pMapEffect = m_pCreator->FindMapEffect( m_pMagicCircle->GetReadyCircelID() );
	ioEffect *pEffect = NULL;
	if( pMapEffect )
	{
		pEffect = pMapEffect->GetEffect();
	}

	bool bSend = false;
	ioUserKeyInput::DirKeyInput eDir = m_pOwner->CheckCurDirKey();
	if( eDir == ioUserKeyInput::DKI_NONE )
	{
		vCurPos = m_pMagicCircle->GetCurPos();
	}
	else
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

		vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos *= fMoveSpeed * g_FrameTimer.GetSecPerFrame();
		vCurPos = m_pMagicCircle->GetCurPos();
		vPrePos = vCurPos;
		vCurPos += vPos;
	}

	if( pEffect )
	{
		fMapHeight = m_pCreator->GetMapHeightForBlockGuid( vCurPos, pEffect->GetCollisionBox() );
		if( fMapHeight > 0.0f && bCheckHeight )//
		{
			vCurPos.y = fMapHeight;
		}
	}
	else
	{
		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f && bCheckHeight )
		{
			vCurPos.y = fMapHeight;
		}
	}

	D3DXVECTOR3 vCharPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vCurXZ = vCurPos;

	vCharPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vCharPos;

	if( D3DXVec3Length( &vDiff ) > fRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vCharPos + vDiff;
		vCurPos.y = fCurHeight;

		if( pEffect )
		{
			fMapHeight = m_pCreator->GetMapHeightForBlockGuid( vCurPos, pEffect->GetCollisionBox() );
			if( fMapHeight > 0.0f )
			{
				vCurPos.y = fMapHeight;
			}
		}
		else
		{
			fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
			if( fMapHeight > 0.0f )
			{
				vCurPos.y = fMapHeight;
			}
		}
	}

	m_pMagicCircle->UpdatePos( bEnableSkill, vCurPos, m_pOwner, m_pCreator );
	m_pMagicCircle->SetTargetRot( qtRotate );
}

void ioBaseGUISupport::UpdateMagicCirclePosAll2( float fMoveSpeed, float fRange, float fLimitHeight, bool bEnableSkill )
{
	if( !m_pMagicCircle ) 
		return;

	float fMapHeight = 0.0f;
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3 vPos, vPrePos, vCurPos;

	bool bSend = false;
	ioUserKeyInput::DirKeyInput eDir = m_pOwner->CheckCurDirKey();
	if( eDir == ioUserKeyInput::DKI_NONE )
	{
		vCurPos = m_pMagicCircle->GetCurPos();
		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
		{
			vCurPos.y = fMapHeight;
		}
	}
	else
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

		vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos *= fMoveSpeed * g_FrameTimer.GetSecPerFrame();
		vCurPos = m_pMagicCircle->GetCurPos();
		vPrePos = vCurPos;
		vCurPos += vPos;
	}

	fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
	if( fMapHeight > 0.0f )
	{
		vCurPos.y = fMapHeight;
	}

	D3DXVECTOR3 vCharPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vCurXZ = vCurPos;

	vCharPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	// Range Check
	D3DXVECTOR3 vDiff = vCurXZ - vCharPos;
	if( D3DXVec3Length( &vDiff ) > fRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vCharPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	// Limit Height Check
	if( vCurPos.y > fLimitHeight )
	{
		// no update
		return;
	}

	m_pMagicCircle->UpdatePos( bEnableSkill, vCurPos, m_pOwner, m_pCreator );
	m_pMagicCircle->SetTargetRot( qtRotate );
}

void ioBaseGUISupport::UpdateMagicCirclePosByStartPos( float fMoveSpeed, float fRange, float fHeightGap, bool bEnableSkill )
{
	if( !m_pMagicCircle ) 
		return;

	if( m_pMagicCircle->GetStartPos() != m_pMagicCircle->GetCurPos() )
		g_TutorialMgr.ActionMagicCircleMoving();

	float fMapHeight = 0.0f;
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3 vPos, vPrePos, vCurPos;

	bool bSend = false;
	ioUserKeyInput::DirKeyInput eDir = m_pOwner->CheckCurDirKey();
	if( eDir == ioUserKeyInput::DKI_NONE )
	{
		vCurPos = m_pMagicCircle->GetCurPos();
		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
		{
			vCurPos.y = fMapHeight;
		}
	}
	else
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

		vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos *= fMoveSpeed * g_FrameTimer.GetSecPerFrame();
		vCurPos = m_pMagicCircle->GetCurPos();
		vPrePos = vCurPos;
		vCurPos += vPos;
	}

	fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
	if( fMapHeight > 0.0f )
	{
		vCurPos.y = fMapHeight;
	}

	D3DXVECTOR3 vCircleStartPos = m_pMagicCircle->GetStartPos();
	D3DXVECTOR3 vCurXZ = vCurPos;

	vCircleStartPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vCircleStartPos;

	if( D3DXVec3Length( &vDiff ) > fRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= fRange;

		float fCurHeight = vCurPos.y;
		vCurPos = vCircleStartPos + vDiff;
		vCurPos.y = fCurHeight;

		fMapHeight = m_pCreator->GetMapHeight( vCurPos.x, vCurPos.z, m_pOwner, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;
	}

	m_pMagicCircle->UpdatePos( bEnableSkill, vCurPos, m_pOwner, m_pCreator );
	m_pMagicCircle->SetTargetRot( qtRotate );
}

void ioBaseGUISupport::SetMagicCirclePos( bool bEnableSkill, const D3DXVECTOR3& vPos )
{
	if( !m_pMagicCircle ) 
		return;

	m_pMagicCircle->UpdatePos( bEnableSkill, vPos, m_pOwner, m_pCreator );
}

void ioBaseGUISupport::SyncMagicCirclePos( bool bVisibleTime, bool bCharInfo )
{
	if( !m_pMagicCircle ) 
		return;

	if( !m_pOwner->IsNeedSendNetwork() ) 
		return;

	SP2Packet kPacket( CUPK_UPDATE_MAGIC_CIRCLE );
	kPacket << m_pOwner->GetCharName();
	kPacket << false;
	kPacket << m_pMagicCircle->GetCurPos();
	kPacket << bVisibleTime;		// Set Visible Time

	kPacket << bCharInfo;
	if( bCharInfo )
	{
		kPacket << (int)m_pOwner->GetDirKey();
		kPacket << m_pOwner->IsReverseDirection();
		kPacket << m_pOwner->GetWorldPosition();
		kPacket << m_pOwner->GetTargetRot();
		kPacket << m_pOwner->GetMoveDir();
	}
	P2PNetwork::SendToAllPlayingUser( kPacket );
}

void ioBaseGUISupport::SetMagicCircleTarget( const ioHashString &szCircleTarget )
{
	m_szMagicCircleTarget = szCircleTarget;
}

D3DXVECTOR3 ioBaseGUISupport::GetMagicCircleTargetPos()
{
	ioBaseChar *pTarget = m_pCreator->GetBaseChar( GetMagicCircleTarget() );
	if( !pTarget )
		pTarget = m_pOwner;

	return pTarget->GetWorldPosition();
}

D3DXQUATERNION ioBaseGUISupport::GetMagicCircleTargetRot()
{
	ioBaseChar *pTarget = m_pCreator->GetBaseChar( GetMagicCircleTarget() );
	if( !pTarget )
		pTarget = m_pOwner;

	return pTarget->GetWorldOrientation();
}

bool ioBaseGUISupport::CheckMagicCircle() const
{
	if( m_pMagicCircle )
		return true;

	return false;
}

bool ioBaseGUISupport::ApplyMagicCircle( SP2Packet &rkPacket )
{
	if( !CheckMagicCircle() )
		return false;

	bool bEnableSkill, bVisibleTime;
	D3DXVECTOR3 vPosition;
	rkPacket >> bEnableSkill;
	rkPacket >> vPosition;
	rkPacket >> bVisibleTime;

	m_pMagicCircle->UpdatePos( bEnableSkill, vPosition, m_pOwner, m_pCreator );

	if( bVisibleTime )
		m_pMagicCircle->SetVisibleTime( m_pCreator );

	bool bCharInfo;
	rkPacket >> bCharInfo;

	if( bCharInfo )
	{
		int iDirKey;
		rkPacket >> iDirKey;
		m_pOwner->SetNetworkKeyInput( (ioUserKeyInput::DirKeyInput)iDirKey );

		bool bReverseDirection;
		rkPacket >> bReverseDirection;
		m_pOwner->SetOnlyReverseDirection( bReverseDirection );

		D3DXVECTOR3 vRecvPos;
		rkPacket >> vRecvPos;
		m_pOwner->SetWorldPosition( vRecvPos );

		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vMoveDir;
		rkPacket >> qtRot >> vMoveDir;
		m_pOwner->SetTargetRot( qtRot );
		m_pOwner->SetMoveDirection( vMoveDir );
	}

	return true;
}

bool ioBaseGUISupport::CheckChangeDirectionForMagicCircle()
{
	if( m_pOwner->IsNeedProcess() && m_pOwner->IsChangeDirection() )
	{
		if( m_pOwner->IsNeedSendNetwork() )
		{
			SyncMagicCirclePos( false, true );
		}
		return true;
	}

	return false;
}

bool ioBaseGUISupport::IsEasterEgg()
{
	return ( m_pOwner->GetCharName() == "개발자H" );
}

// TeamCircle And Gauge ------------------------------------------------------------------------------------------------------------------------------------------------------
void ioBaseGUISupport::InitOwnerMark()
{
	ClearOwnerMark();
	m_pOwnerMark = new ioOwnerMark;
}

void ioBaseGUISupport::ClearOwnerMark()
{
	SAFEDELETE( m_pOwnerMark );
}

void ioBaseGUISupport::ReSetOwnerMarkTeam()
{
	switch( m_pOwner->GetTeam() )
	{
	case TEAM_BLUE:
	case TEAM_RED:
		if( m_pOwnerMark && ( g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) || m_pCreator->IsStealthMode() ) )
			m_pOwnerMark->SetTeam( m_pOwner->GetTeam() );
		break;
	default:
		if( m_pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		{
			if( m_pOwnerMark && ( g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) || m_pCreator->IsStealthMode() ) )
				m_pOwnerMark->SetTeam( TEAM_BLUE );
		}
		else
		{
			if( m_pOwnerMark && ( g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) || m_pCreator->IsStealthMode() ) )
				m_pOwnerMark->SetTeam( TEAM_RED );
		}
		break;
	}	
}

void ioBaseGUISupport::SetEnableFlagPointGauge( bool bEnable )
{
	m_bEnableFlagPointGauge = bEnable;

	if( m_bEnableFlagPointGauge )
		m_dwFlagPointStartTime = REALGETTIME();
}

void ioBaseGUISupport::SetTeamUI( TeamType eType )
{
	if( !m_pGauge )
	{
		m_pGauge = new ioGaugeHelper;
		m_pGauge->InitResource();
	}

	if( !m_pTeamCircle )
	{
		m_pTeamCircle = new ioTeamCircle;
	}

	if( !m_pFlagPointGauge )
	{
		m_pFlagPointGauge = new ioFlagPointGaugeHelper;
		m_pFlagPointGauge->InitResource();
	}

	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	vPos.y = m_pOwner->GetBottomHeight();

	switch( m_pOwner->GetTeam() )
	{
	case TEAM_BLUE:
	case TEAM_RED:
		m_pGauge->SetTeam( eType );

		if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
			m_pOwnerMark->SetTeam( eType );

		m_pTeamCircle->SetTeam( m_pCreator, eType, vPos, m_pOwner->IsOwnerChar() );
		break;
	default:
		{
			if( m_pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			{
				m_pGauge->SetTeam( TEAM_BLUE );
				m_pTeamCircle->SetTeam( m_pCreator, TEAM_BLUE, vPos, m_pOwner->IsOwnerChar() );

				if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
					m_pOwnerMark->SetTeam( TEAM_BLUE );
			}
			else
			{
				m_pGauge->SetTeam( TEAM_RED );
				m_pTeamCircle->SetTeam( m_pCreator, TEAM_RED, vPos, m_pOwner->IsOwnerChar() );

				if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
					m_pOwnerMark->SetTeam( TEAM_RED );
			}
		}		
		break;
	}

	if( m_pMode->GetModeType() == MT_FLAG_CAPTURE )
	{
		if( m_bEnableFlagPointGauge || m_pFlagPointGauge )
		{
			m_pFlagPointGauge->SetTeam( m_pOwner->GetTeam(), m_pOwner->IsOwnerChar() );
		}
	}
}

void ioBaseGUISupport::DestroyTeamCircle()
{
	if( m_pTeamCircle )
		m_pTeamCircle->DestroyEffect( m_pCreator );

	SAFEDELETE( m_pTeamCircle );
}

void ioBaseGUISupport::UpdateBottomCircle()
{
	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	if( m_pTeamCircle )
	{
		bool bShadowVisible = IsVisibleBottomShadow();
		bool bCircleVisible = IsVisibleBottomCircle();

		vPos.y = m_pOwner->GetBottomHeight() + FLOAT1;
		m_pTeamCircle->UpdatePos( m_pCreator, vPos, bCircleVisible, bShadowVisible );
	}
}

bool ioBaseGUISupport::IsVisibleBottomCircle()
{
	if( !m_pOwner->IsCanInvisible() && !m_pGrp->IsVisible() )
		return false;

	bool bCheckMode = false;
	if( g_GUIMgr.IsNoneUIMode() ||
		m_pCreator->HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) ||
		m_pCreator->HasCameraFixedStyle( CAMERA_STYLE_VIEW  ) )
		bCheckMode = true;

	if( bCheckMode && !Setting::NoUIModeTeamCircleShow() )
		return false;

	if ( m_pOwner->IsInvisibleBottomCircleBySkill() )
		return false;

	switch( m_pOwner->GetState() )
	{
	case CS_ESCAPE_PRISONER:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_DIE:
		return false;
	}

	if( m_pTeamCircle )
	{
		if( m_bTeamCircleCheck )
		{
			if( !m_pOwner->IsOwnerChar() )
				return false;
		}

		return m_bVisibleBottomCircle;
	}

	return false;
}

bool ioBaseGUISupport::IsVisibleBottomShadow()
{
	if( !m_pGrp->IsVisible() )
		return false;

	switch( m_pOwner->GetState() )
	{
	case CS_ESCAPE_PRISONER:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_DIE:
		return false;
	}

	if( m_pTeamCircle )
	{
		D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate( FLOAT1 );
		vPos.y += 40.0f;

		int iXPos, iYPos;
		if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
			return true;
	}

	return false;
}

void ioBaseGUISupport::SetBottomCircleVisible( bool bVisible )
{
	m_bVisibleBottomCircle = bVisible;
}

void ioBaseGUISupport::CheckGaugeTeam( bool bChange )
{
	if ( !m_pGauge || !m_pTeamCircle )		
		return;
	m_pOwner->SetEnableChangeTeam( bChange );

	TeamType eCurTeamType = m_pOwner->GetTeam();
	TeamType eSingleTeamType = m_pOwner->GetSingleConvertTeam();

	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	vPos.y = m_pOwner->GetBottomHeight();

	if( bChange )
	{
		switch( eCurTeamType )
		{
		case TEAM_BLUE:
			m_pGauge->SetTeam( TEAM_RED );

			if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
				m_pOwnerMark->SetTeam( TEAM_RED );

			m_pTeamCircle->SetTeam( m_pCreator, TEAM_RED, vPos, m_pOwner->IsOwnerChar() );
			break;
		case TEAM_RED:
			m_pGauge->SetTeam( TEAM_BLUE );

			if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
				m_pOwnerMark->SetTeam( TEAM_BLUE );

			m_pTeamCircle->SetTeam( m_pCreator, TEAM_BLUE, vPos, m_pOwner->IsOwnerChar() );
			break;
		default:
			{
				if( eSingleTeamType == TEAM_BLUE )
				{
					m_pGauge->SetTeam( TEAM_RED );
					m_pTeamCircle->SetTeam( m_pCreator, TEAM_RED, vPos, m_pOwner->IsOwnerChar() );

					if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
						m_pOwnerMark->SetTeam( TEAM_RED );
				}
				else
				{
					m_pGauge->SetTeam( TEAM_BLUE );
					m_pTeamCircle->SetTeam( m_pCreator, TEAM_BLUE, vPos, m_pOwner->IsOwnerChar() );

					if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
						m_pOwnerMark->SetTeam( TEAM_BLUE );
				}
			}		
			break;
		}
	}
	else
	{
		switch( eCurTeamType )
		{
		case TEAM_BLUE:
		case TEAM_RED:
			m_pGauge->SetTeam( eCurTeamType );

			if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
				m_pOwnerMark->SetTeam( eCurTeamType );

			m_pTeamCircle->SetTeam( m_pCreator, eCurTeamType, vPos, m_pOwner->IsOwnerChar() );
			break;
		default:
			{
				if( eSingleTeamType == TEAM_BLUE )
				{
					m_pGauge->SetTeam( TEAM_BLUE );
					m_pTeamCircle->SetTeam( m_pCreator, TEAM_BLUE, vPos, m_pOwner->IsOwnerChar() );

					if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
						m_pOwnerMark->SetTeam( TEAM_BLUE );
				}
				else
				{
					m_pGauge->SetTeam( TEAM_RED );
					m_pTeamCircle->SetTeam( m_pCreator, TEAM_RED, vPos, m_pOwner->IsOwnerChar() );

					if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
						m_pOwnerMark->SetTeam( TEAM_RED );
				}
			}		
			break;
		}	
	}
}

void ioBaseGUISupport::SetIsOwnerCharUI( TeamType eType )
{
	if( m_pTeamCircle )
	{
		D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
		vPos.y = m_pOwner->GetBottomHeight();

		switch( m_pOwner->GetTeam() )
		{
		case TEAM_BLUE:
		case TEAM_RED:
			m_pTeamCircle->SetTeam( m_pCreator, eType, vPos, m_pOwner->IsOwnerChar() );
			break;
		default:
			{
				if( m_pOwner->GetSingleConvertTeam() == TEAM_BLUE )
					m_pTeamCircle->SetTeam( m_pCreator, TEAM_BLUE, vPos, m_pOwner->IsOwnerChar() );
				else
					m_pTeamCircle->SetTeam( m_pCreator, TEAM_RED, vPos, m_pOwner->IsOwnerChar() );
			}
			break;
		}
	}
}

void ioBaseGUISupport::UpdateGaugeGUI()
{
	DWORD dwCurTime = FRAMEGETTIME();
	StatusValue& OwnerHP = m_pOwner->GetHP();
	if( m_pGauge )
	{
		m_pGauge->UpdateGauge( OwnerHP.m_fCurValue, OwnerHP.m_fMaxValue, ( g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) || m_pCreator->IsStealthMode() ) );

		if( OwnerHP.m_fCurValue <= 0.0f )
		{
			if( !m_bGaugeAlpha )
			{
				if( m_dwGaugeCheckTime == 0 )
				{
					m_dwGaugeCheckTime = dwCurTime;
					m_bGaugeAlpha = true;
				}
				else
				{
					if( (dwCurTime-m_dwGaugeCheckTime) > m_dwGaugeTic )
					{
						m_dwGaugeCheckTime = dwCurTime;
						m_bGaugeAlpha = true;
					}
				}
			}
			else
			{
				if( (dwCurTime-m_dwGaugeCheckTime) > m_dwAlphaTic )
				{
					m_dwGaugeCheckTime = dwCurTime;
					m_bGaugeAlpha = false;
				}
			}
		}
		else
		{
			m_bGaugeAlpha = false;
		}
	}

	if( m_pOwner->IsOwnerChar() && m_pOwnerMark )
		m_pOwnerMark->Update();
	else if( m_pOwnerMark && ( g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) || m_pCreator->IsStealthMode() ) )
		m_pOwnerMark->Update();

	if( m_pFlagPointGauge && m_pMode->GetModeType() == MT_FLAG_CAPTURE )
	{	
		ioFlagCaptureMode* pMode = ToFlagCaptureMode( m_pMode );
		float fMaxPoint = pMode->GetMaxPoint();
		float fCurPoint = ( ( REALGETTIME() - m_dwFlagPointStartTime ) / 1000 * pMode->GetPointPerSec() ) + pMode->GetBasePoint();

		int iXPos, iYPos;
		D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate( FLOAT1 );
		Setting::Check2DInScreen( vPos, iXPos, iYPos );

		DWORD dwPassedTime = REALGETTIME() - m_dwFlagPointStartTime;
		if( !COMPARE( fCurPoint, 0.0f, fMaxPoint ) )
			fCurPoint = fMaxPoint;
		if( !COMPARE( dwPassedTime, 0, 999000 ) )
			dwPassedTime = 999000;
		m_pFlagPointGauge->UpdateFlagPointGauge( iXPos, iYPos - 90, fCurPoint, fMaxPoint, dwPassedTime );
	}
}

void ioBaseGUISupport::RenderGaugeBar( int iGaugeXPos, int iGaugeYPos, int iOwnerIconOffsetX, int iOwnerIconOffsetY, int iOwnerIconOffsetExY  )
{
	if( m_pOwner->GetSpecialHide() ) 
		return;
	if( m_pOwner->IsDisableGauge() ) 
		return;

	const CharState& OwnerState = m_pOwner->GetState();
	if(	OwnerState == CS_ESCAPE_PRISONER || 
		OwnerState == CS_READY ||
		OwnerState == CS_VIEW ||
		OwnerState == CS_OBSERVER ||
		OwnerState == CS_LOADING ||
		OwnerState == CS_DIE ||
		(!m_pOwner->IsCanInvisible() && !m_pGrp->IsVisible()) )
	{
		m_bGaugeAlpha = false;

		if( m_pGauge )
			m_pGauge->SetGaugeAlpha( m_bGaugeAlpha );

		return;
	}

	if( g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) || m_pCreator->IsStealthMode() )
		ObserverRenderGauge( iOwnerIconOffsetX, iOwnerIconOffsetY, iOwnerIconOffsetExY );
	else
		NormalRenderGauge( iOwnerIconOffsetX, iOwnerIconOffsetY, iOwnerIconOffsetExY );

	RenderMonsterReward();

#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_SHADOW );

		g_FontMgr.PrintText( iGaugeXPos, iGaugeYPos + 10, 0.6f, m_pOwner->GetStateText() );
		if( OwnerState == CS_JUMP )
		{
			g_FontMgr.PrintText( iGaugeXPos, iGaugeYPos + 22, 0.6f, m_pOwner->GetJumpStateText() );
		}
		else if( OwnerState == CS_ATTACK )
		{
			g_FontMgr.PrintText( iGaugeXPos, iGaugeYPos + 22, 0.6f, m_pOwner->GetAttackStateText() );
		}

		g_FontMgr.PrintText( iGaugeXPos, iGaugeYPos - 22.0f, 0.6f, "%d/%d", (int)m_pOwner->GetHP().m_fCurValue,
			(int)m_pOwner->GetHP().m_fMaxValue );

		for(int i = 0;i < MAX_INVENTORY;i++)
		{
			if( m_pOwner->GetEquipSlot() )
			{
				ioItem *pItem = m_pOwner->GetEquipSlot()->GetEquipedItem( i );
				if( pItem )
				{
					g_FontMgr.PrintText( iGaugeXPos, iGaugeYPos - ( 37.0f + (i*15) ), 0.6f, "%d : %d / %d", i, (int)pItem->GetItemMaleCustom(), (int)pItem->GetItemFemaleCustom() );
				}
			}
		}
	}
#endif
}

void ioBaseGUISupport::NormalRenderGauge( int iOwnerIconOffsetX, int iOwnerIconOffsetY, int iOwnerIconOffsetExY )
{
	int iXPos = 0;
	int iYPos = 0;

	bool bPractice = false;
	if(m_pMode->GetModeType() == MT_PRACTICE)
		bPractice = true;

	if( m_pOwner->GetCurActiveSkill() && m_pOwner->GetCurActiveSkill()->HideHPGauge() )
		return;

	if( m_pGauge )
	{
		D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate( FLOAT1 );
		vPos.y += 40.0f;

		if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
		{
			g_FontMgr.SetTextStyle( TS_BOLD_OUTLINE_FULL );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			TeamType eCurTeamType = m_pOwner->GetTeam();
			TeamType eCurSingleTeamType = m_pOwner->GetSingleConvertTeam();
			TeamType eNamedTitleTeamType;

			if( m_pOwner->HasChangeTeamBuff( false ) )
			{
				switch( eCurTeamType )
				{
				case TEAM_BLUE:
					eCurTeamType = TEAM_RED;
					break;
				case TEAM_RED:
					eCurTeamType = TEAM_BLUE;
					break;
				default:
					if( eCurSingleTeamType == TEAM_BLUE )
						eCurSingleTeamType = TEAM_RED;
					else
						eCurSingleTeamType = TEAM_BLUE;
					break;
				}	
			}

			switch( eCurTeamType )
			{
			case TEAM_BLUE:
				{
					g_FontMgr.SetTextColor( 0, 150, 255 );
					eNamedTitleTeamType = TEAM_BLUE;
				}
				break;
			case TEAM_RED:
				{
					g_FontMgr.SetTextColor( 255, 60, 0 );
					eNamedTitleTeamType = TEAM_RED;
				}
				break;
			default: //개인전
				{
					if( eCurSingleTeamType == TEAM_BLUE )
					{
						g_FontMgr.SetTextColor( 0, 150, 255 );
						eNamedTitleTeamType = TEAM_BLUE;
					}
					else
					{
						g_FontMgr.SetTextColor( 255, 60, 0 );
						eNamedTitleTeamType = TEAM_RED;
					}
				}				
				break;
			}

			if( m_pOwner->IsVisibleID() )
			{
				enum { UPPER_Y_OFFSET = -12, GUILD_Y_OFFSET = -11, LADDER_TITLE_Y_OFFSET = -16, };
				if( m_pCreator->GetModeType() == MT_MYROOM && !m_pCreator->IsNetworkMode() )
				{
					if( m_pOwner->IsOwnerChar() )
					{
						g_FontMgr.SetAlignType( TAT_LEFT );

						DWORD dwGuildIndex, dwGuildMark;
						g_UserInfoMgr.GetGuildInfo( g_MyInfo.GetPublicID(), dwGuildIndex, dwGuildMark );
						int iNameWidth = g_FontMgr.GetTextWidth( g_MyInfo.GetPublicID().c_str(), TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
						int iHalfWidth = ( iNameWidth + 20 + g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 1 ) / 2;	// 20 은 계급사이즈

						if(!bPractice)
							g_LevelMgr.RenderGrade( g_MyInfo.GetPublicID(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, m_pOwner->GetLevel(), TEAM_BLUE );
						g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + UPPER_Y_OFFSET, FONT_SIZE_12, g_MyInfo.GetPublicID().c_str() );

						g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );

						// 래더전 타이틀
						if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && m_iHeroTitle != 0 )
						{
							g_FontMgr.SetAlignType( TAT_CENTER );
							g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( m_iHeroTitle ).c_str() );
						}
						else if( IsEasterEgg() )
						{
							g_FontMgr.SetAlignType( TAT_CENTER );
							g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "< 세주씨에게 발린자 >" );
						}
						else if ( m_dwNamedTitleCode > 0 )
							g_NamedTitleInfoMgr.RenderNamedTitle( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, m_dwNamedTitleCode, m_nNamedTitleLevel, m_byNamedTitlePremium, eNamedTitleTeamType, m_pNamedTitleMark );

						// 런닝맨 타이틀
						m_pOwner->RunningManName( iXPos, iYPos - 31 );
					}
				}
				else
				{
					g_FontMgr.SetAlignType( TAT_LEFT );
					const int& iOwnerLevel = m_pOwner->GetLevel();
					const ioHashString& szOwnerName = m_pOwner->GetCharName().c_str();
					char szCharName[MAX_PATH] = "";
					sprintf( szCharName, "%s", szOwnerName.c_str() );
					DWORD dwGuildIndex, dwGuildMark;
					g_UserInfoMgr.GetGuildInfo( szOwnerName, dwGuildIndex, dwGuildMark );
					int iGuildSmallMarkSize = g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark );
					int iNameWidth = g_FontMgr.GetTextWidth( szCharName, TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
					int iHalfWidth = ( iNameWidth + 20 + iGuildSmallMarkSize + 1 ) / 2;	// 20 은 계급사이즈

					if(!bPractice)
					{
						switch( eCurTeamType )
						{
						case TEAM_BLUE:
							g_LevelMgr.RenderGrade( szOwnerName, iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, iOwnerLevel, TEAM_BLUE );
							break;
						case TEAM_RED:
							g_LevelMgr.RenderGrade( szOwnerName, iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, iOwnerLevel, TEAM_RED );
							break;
						default: //개인전
							if( eCurSingleTeamType == TEAM_BLUE )
								g_LevelMgr.RenderGrade( szOwnerName, iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, iOwnerLevel, TEAM_BLUE );
							else
								g_LevelMgr.RenderGrade( szOwnerName, iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, iOwnerLevel, TEAM_RED );
							break;
						}
					}
					if(!bPractice)
					{
						g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + UPPER_Y_OFFSET, FONT_SIZE_12, szCharName );
						g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );
					}
					else
						g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 10, iYPos + UPPER_Y_OFFSET, FONT_SIZE_12, szCharName );

					if(!bPractice)
					{
						// 래더전 타이틀
						if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && m_iHeroTitle != 0 )
						{
							g_FontMgr.SetAlignType( TAT_CENTER );
							g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( m_iHeroTitle ).c_str() );
						}
						else if( IsEasterEgg() )
						{
							g_FontMgr.SetAlignType( TAT_CENTER );
							g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "< 세주씨에게 발린자 >" );
						}
						else if ( m_dwNamedTitleCode > 0 )
							g_NamedTitleInfoMgr.RenderNamedTitle( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, m_dwNamedTitleCode, m_nNamedTitleLevel, m_byNamedTitlePremium, eNamedTitleTeamType, m_pNamedTitleMark );

						// 런닝맨 타이틀
						m_pOwner->RunningManName( iXPos, iYPos - 31 );
					}
				}
			}			

			if( m_pOwner->IsVisibleHP() )
			{
				if( m_pMode && m_pMode->GetModeState() != ioPlayMode::MS_PLAY )
					m_bGaugeAlpha = false;

				enum { LEVEL_X_OFFSET = -1, LEVEL_Y_OFFSET = 16, };
				if(!bPractice)
				{
					switch( eCurTeamType )
					{
					case TEAM_BLUE:
					case TEAM_RED:
						if(Help::IsMonsterDungeonMode(m_pCreator->GetModeType()))
						{
							g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, m_pOwner->GetGrowthInstantLevel()+1, eCurTeamType, m_pOwner->IsOwnerChar() );
						}
						else
						{
							g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, m_pOwner->GetClassLevel(), eCurTeamType, m_pOwner->IsOwnerChar() );
						}
						break;
					default: //개인전
						{
							int iClassLevel = m_pOwner->GetClassLevel();
							ioLSMouse *pMouse = g_App.GetMouse();
							if( !pMouse || pMouse->GetTraining() ) 
								iClassLevel = 0;

							if( eCurSingleTeamType == TEAM_BLUE )
								g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, iClassLevel, TEAM_BLUE, m_pOwner->IsOwnerChar() );
							else
								g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, iClassLevel, TEAM_RED, m_pOwner->IsOwnerChar() );
						}
						break;
					}			
				}		
				enum { GAUGE_Y_OFFSET = 6, };
				m_pGauge->SetGaugeAlpha( m_bGaugeAlpha );
				m_pGauge->RenderGauge( iXPos, iYPos + GAUGE_Y_OFFSET, false );
			}
		}
	}
	// owner mark
	if( m_pOwner->IsOwnerChar() && m_pOwner->IsVisibleID() )
	{
		int iTempXPos = 0, iTempYPos = 0;
		ioMapEffect *pMapEffect = m_pOwner->GetCreator()->FindMapEffect( m_pOwner->GetExcavationGuideID() );
		if( pMapEffect )
		{
			D3DXVECTOR3 vPos = pMapEffect->GetWorldPosition();
			Setting::Check2DInScreen( vPos, iTempXPos, iTempYPos );
			iTempYPos -= pMapEffect->GetEffect()->GetBoundRadius();
			iYPos = iTempYPos;
		}
		else
		{
			if( m_pCharEmoticon && m_pCharEmoticon->IsNeedRender() )
				iYPos += iOwnerIconOffsetExY;
			else if( m_pCharChatBubble && m_pCharChatBubble->IsBubbleChat() )
				iYPos += iOwnerIconOffsetExY;
			else if( m_pFlagPointGauge && m_bEnableFlagPointGauge )
				iYPos += iOwnerIconOffsetExY;
			else
				iYPos += iOwnerIconOffsetY;
		}

		if( m_pOwnerMark )
		{
			if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && m_iHeroTitle != 0 )
				m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos - 14 );
			else if( IsEasterEgg() )
				m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos - 14 );
			else if( !m_szRunningManName.IsEmpty() )
				m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos - 14 );
			else if( m_dwNamedTitleCode > 0 )
				m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos - 14 );
			else
				m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos );
		}
	}

	// Flag Point Gauge
	if( m_pFlagPointGauge && m_pMode->GetModeType() == MT_FLAG_CAPTURE && m_bEnableFlagPointGauge )
	{
		D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate( FLOAT1 );
		Setting::Check2DInScreen( vPos, iXPos, iYPos );

		m_pFlagPointGauge->RenderFlagPointGauge( iXPos, iYPos - 90 );
	}
}

void ioBaseGUISupport::ObserverRenderGauge( int iOwnerIconOffsetX, int iOwnerIconOffsetY, int iOwnerIconOffsetExY )
{
	int iXPos = 0;
	int iYPos = 0;
	TeamType eNamedTitleTeamType;

	bool bPractice = false;
	if(m_pMode->GetModeType() == MT_PRACTICE)
		bPractice = true;

	if( m_pGauge )
	{
		D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate( FLOAT1 );
		vPos.y += 40.0f;

		if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
		{
			g_FontMgr.SetTextStyle( TS_BOLD_OUTLINE_FULL );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			switch( m_pOwner->GetTeam() )
			{
			case TEAM_BLUE:
				{
					eNamedTitleTeamType = TEAM_BLUE;
					g_FontMgr.SetTextColor( 0, 150, 255 );
				}
				break;
			case TEAM_RED:
				{
					eNamedTitleTeamType = TEAM_RED;
					g_FontMgr.SetTextColor( 255, 60, 0 );
				}
				break;
			default: //개인전
				{
					if( m_pOwner->GetSingleConvertTeam() == TEAM_BLUE )
					{
						eNamedTitleTeamType = TEAM_BLUE;
						g_FontMgr.SetTextColor( 0, 150, 255 );
					}
					else
					{
						eNamedTitleTeamType = TEAM_RED;
						g_FontMgr.SetTextColor( 255, 60, 0 );
					}
				}				
				break;
			}

			if( m_pOwner->IsVisibleID() )
			{
				enum { NAME_Y_OFFSET = -15, UPPER_Y_OFFSET = -14, GUILD_Y_OFFSET = -13, LADDER_TITLE_Y_OFFSET = -16,};
				if( m_pCreator->GetModeType() == MT_MYROOM && !m_pCreator->IsNetworkMode() )
				{
					if( m_pOwner->IsOwnerChar() )
					{
						g_FontMgr.SetAlignType( TAT_LEFT );

						DWORD dwGuildIndex, dwGuildMark;
						g_UserInfoMgr.GetGuildInfo( g_MyInfo.GetPublicID(), dwGuildIndex, dwGuildMark );
						int iNameWidth = g_FontMgr.GetTextWidth( g_MyInfo.GetPublicID().c_str(), TS_BOLD_OUTLINE_FULL, FONT_SIZE_14 );
						int iHalfWidth = ( iNameWidth + 20 + g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 1 ) / 2;	// 20 은 계급사이즈
						if(!bPractice)
						g_LevelMgr.RenderGrade( g_MyInfo.GetPublicID(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, m_pOwner->GetLevel(), TEAM_BLUE );
						g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + NAME_Y_OFFSET, FONT_SIZE_14, g_MyInfo.GetPublicID().c_str() );

						g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );

						// 래더전 타이틀
						if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && m_iHeroTitle != 0 )
						{
							g_FontMgr.SetAlignType( TAT_CENTER );
							g_FontMgr.PrintText( iXPos, (iYPos + NAME_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( m_iHeroTitle ).c_str() );
						}
						else if( IsEasterEgg() )
						{
							g_FontMgr.SetAlignType( TAT_CENTER );
							g_FontMgr.PrintText( iXPos, (iYPos + NAME_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "< 세주씨에게 발린자 >" );
						}
						else if ( m_dwNamedTitleCode > 0 )
							g_NamedTitleInfoMgr.RenderNamedTitle( iXPos, (iYPos + NAME_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, m_dwNamedTitleCode, m_nNamedTitleLevel, m_byNamedTitlePremium, eNamedTitleTeamType, m_pNamedTitleMark );

						// 런닝맨 타이틀
						m_pOwner->RunningManName( iXPos, iYPos - 33 );
					}
				}
				else
				{
					g_FontMgr.SetAlignType( TAT_LEFT );

					char szCharName[MAX_PATH] = "";
					const ioHashString& szOwnerName = m_pOwner->GetCharName().c_str();
					sprintf( szCharName, "%s", szOwnerName.c_str() );
					DWORD dwGuildIndex, dwGuildMark;
					g_UserInfoMgr.GetGuildInfo( szOwnerName, dwGuildIndex, dwGuildMark );
					int iNameWidth = g_FontMgr.GetTextWidth( szCharName, TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
					int iHalfWidth = ( iNameWidth + 20+ g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 1 ) / 2;	// 20 은 계급사이즈

					if(!bPractice)
					{
					switch( m_pOwner->GetTeam() )
					{
						case TEAM_BLUE:
							g_LevelMgr.RenderGrade( szOwnerName, iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, m_pOwner->GetLevel(), TEAM_BLUE );
							break;
						case TEAM_RED:
							g_LevelMgr.RenderGrade( szOwnerName, iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, m_pOwner->GetLevel(), TEAM_RED );
							break;
						default: //개인전
							if( m_pOwner->GetSingleConvertTeam() == TEAM_BLUE )
								g_LevelMgr.RenderGrade( szOwnerName, iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, m_pOwner->GetLevel(), TEAM_BLUE );
							else
								g_LevelMgr.RenderGrade( szOwnerName, iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, m_pOwner->GetLevel(), TEAM_RED );
							break;
						}
					}
					if(!bPractice)
					{
						g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + NAME_Y_OFFSET, FONT_SIZE_12, szCharName );
						g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );
					}
					else
						g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 10, iYPos + NAME_Y_OFFSET, FONT_SIZE_12, szCharName );

					if(!bPractice)
					{
					// 래더전 타이틀
					if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && m_iHeroTitle != 0 )
					{
						g_FontMgr.SetAlignType( TAT_CENTER );
						g_FontMgr.PrintText( iXPos, (iYPos + NAME_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( m_iHeroTitle ).c_str() );
					}
					else if( IsEasterEgg() )
					{
						g_FontMgr.SetAlignType( TAT_CENTER );
						g_FontMgr.PrintText( iXPos, (iYPos + NAME_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "< 세주씨에게 발린자 >" );
					}
					else if ( m_dwNamedTitleCode > 0 )
						g_NamedTitleInfoMgr.RenderNamedTitle( iXPos, (iYPos + NAME_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, m_dwNamedTitleCode, m_nNamedTitleLevel, m_byNamedTitlePremium, eNamedTitleTeamType, m_pNamedTitleMark );

						// 런닝맨 타이틀
						m_pOwner->RunningManName( iXPos, iYPos - 33 );
					}
				}
			}			

			if( m_pOwner->IsVisibleHP() )
			{
				if( m_pMode && m_pMode->GetModeState() != ioPlayMode::MS_PLAY )
					m_bGaugeAlpha = false;
				if(!bPractice)
				{
					enum { LEVEL_X_OFFSET = -1, LEVEL_Y_OFFSET = 16, };
					switch( m_pOwner->GetTeam() )
					{
					case TEAM_BLUE:
					case TEAM_RED:
						g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, m_pOwner->GetClassLevel(), m_pOwner->GetTeam(), m_pOwner->IsOwnerChar() );
						break;
					default: //개인전
						if( m_pOwner->GetSingleConvertTeam() == TEAM_BLUE )
							g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, m_pOwner->GetClassLevel(), TEAM_BLUE, m_pOwner->IsOwnerChar() );
						else
							g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, m_pOwner->GetClassLevel(), TEAM_RED, m_pOwner->IsOwnerChar() );
						break;
					}					
				}
				enum { GAUGE_Y_OFFSET = 3, };
				m_pGauge->SetGaugeAlpha( m_bGaugeAlpha );
				m_pGauge->RenderGauge( iXPos, iYPos + GAUGE_Y_OFFSET, true );
			}
		}
	}

	// owner mark
	if( m_pCreator && m_pOwner->IsVisibleID() && ( g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) || m_pCreator->IsStealthMode() ) )
	{
		if( m_pCreator->GetCameraTargetChar() && m_pCreator->GetCameraTargetChar()->GetCharName() == m_pOwner->GetCharName() )
		{
			int iTempXPos = 0, iTempYPos = 0;
			ioMapEffect *pMapEffect = m_pOwner->GetCreator()->FindMapEffect( m_pOwner->GetExcavationGuideID() );
			if( pMapEffect )
			{
				D3DXVECTOR3 vPos = pMapEffect->GetWorldPosition();
				Setting::Check2DInScreen( vPos, iTempXPos, iTempYPos );
				iTempYPos -= pMapEffect->GetEffect()->GetBoundRadius();
				iYPos = iTempYPos;
			}
			else
			{
				if( m_pCharEmoticon && m_pCharEmoticon->IsNeedRender() )
					iYPos += iOwnerIconOffsetExY;
				else if( m_pCharChatBubble && m_pCharChatBubble->IsBubbleChat() )
					iYPos += iOwnerIconOffsetExY;
				else
					iYPos += iOwnerIconOffsetY;
			}

			if( m_pOwnerMark )
			{
				if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && m_iHeroTitle != 0 )
					m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos - 14 );
				else if( IsEasterEgg() )
					m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos - 14 );
				else if( !m_szRunningManName.IsEmpty() )
					m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos - 14 );
				else if( m_dwNamedTitleCode > 0 )
					m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos - 14 );
				else
					m_pOwnerMark->Render( iXPos + iOwnerIconOffsetX, iYPos );
			}
		}
	}
}

void ioBaseGUISupport::SetMonsterReward( int iMonsterRewardExp, int iMonsterRewardPeso )
{
	m_iMonsterRewardExp	 = iMonsterRewardExp;
	m_iMonsterRewardPeso = iMonsterRewardPeso;
	m_dwMonsterRewardTime= FRAMEGETTIME();
}

void ioBaseGUISupport::RenderMonsterReward()
{
	if( m_dwMonsterRewardTime == 0 ) 
		return;

	int iXPos = 0;
	int iYPos = 0;

	D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate( FLOAT1 );
	vPos.y -= FLOAT100;

	if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
	{
		char szExpText[MAX_PATH] = "";
		SafeSprintf( szExpText, sizeof( szExpText ), "+%d 경험치", m_iMonsterRewardExp );

		char szPesoText[MAX_PATH] = "";
		SafeSprintf( szPesoText, sizeof( szPesoText ), "+%d 페소", m_iMonsterRewardPeso );

		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );

		DWORD dwGapTime = FRAMEGETTIME() - m_dwMonsterRewardTime;
		if( COMPARE( dwGapTime, 0, 100 + 1 ) )
		{
			// 200%크기로 생성 (알파값50%) 100% 크기로 축소됨 (알파값100%)
			float fScaleRate = (float)( 100 - dwGapTime ) / 100;
			float fAlphaRate = (float)dwGapTime / 100;
			fAlphaRate = min( FLOAT1, FLOAT05 + (fAlphaRate / 2) );

			// 경험치 
			g_FontMgr.SetTextColor( 255, 130, 0 );
			g_FontMgr.PrintTextAlpha( iXPos, iYPos - 20, FONT_SIZE_17 * (1.0f + fScaleRate), (float)MAX_ALPHA_RATE * fAlphaRate, szExpText );

			// 페소
			g_FontMgr.SetTextColor( 0, 150, 255 );
			g_FontMgr.PrintTextAlpha( iXPos, iYPos, FONT_SIZE_17 * (1.0f + fScaleRate), (float)MAX_ALPHA_RATE * fAlphaRate, szPesoText );
		}
		else if( COMPARE( dwGapTime, 100, 1300 + 1 ) )
		{
			dwGapTime -= 100;
			// 0,-56까지 올라감 (알파값100%)
			float fYPosRate = (float)dwGapTime / 1200;

			// 경험치 
			g_FontMgr.SetTextColor( 255, 130, 0 );
			g_FontMgr.PrintText( iXPos, iYPos - 20 - ( 26 * fYPosRate ), FONT_SIZE_17, szExpText );

			// 페소
			g_FontMgr.SetTextColor( 0, 150, 255 );
			g_FontMgr.PrintText( iXPos, iYPos - ( 26 * fYPosRate ), FONT_SIZE_17, szPesoText );
		}
		else
		{
			if( dwGapTime >= 1900 )
			{
				m_dwMonsterRewardTime = 0;
				m_iMonsterRewardExp	  = 0;
				m_iMonsterRewardPeso  = 0;
				return;
			}
			dwGapTime -= 1300;

			// 0,-69까지 올라감 (알파값0%)
			float fYPosRate = (float)dwGapTime / 600;
			float fAlphaRate = min( FLOAT1, (float)( 600 - dwGapTime ) / 600 );
			// 경험치 
			g_FontMgr.SetTextColor( 255, 130, 0 );
			g_FontMgr.PrintTextAlpha( iXPos, iYPos - 46 - ( 13 * fYPosRate ), FONT_SIZE_17, (float)MAX_ALPHA_RATE * fAlphaRate, szExpText );

			// 페소
			g_FontMgr.SetTextColor( 0, 150, 255 );
			g_FontMgr.PrintTextAlpha( iXPos, iYPos - 26 - ( 13 * fYPosRate ), FONT_SIZE_17,  (float)MAX_ALPHA_RATE * fAlphaRate, szPesoText );
		}
	}
}

bool ioBaseGUISupport::GetRenderGaugeRect( RECT *pRect ) const
{
	D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;

	int iXPos =0, iYPos = 0;
	if( !Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
		return false;

	int iBottomX, iBottomY;
	if( !Setting::Check2DInScreen( m_pOwner->GetWorldPosition(), iBottomX, iBottomY ) )
		return false;

	char szIDText[MAX_PATH];

	int iLevel = m_pOwner->GetClassLevel();
	if( ioPlayMode::GetModeType() == MT_ARENA )
		iLevel = 0;

	if( m_pCreator->GetModeType() == MT_MYROOM && !m_pCreator->IsNetworkMode() )
	{
		if( m_pOwner->IsOwnerChar() )
		{
			int iClassType = g_MyInfo.GetClassType( m_pOwner->GetIndex() );
			wsprintf( szIDText, "Lv%d %s", iLevel, g_MyInfo.GetClassName( iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( iClassType ) ) );
		}
		else
		{
			wsprintf( szIDText, "Lv%d %s", iLevel, m_pOwner->GetCharName().c_str() );
		}
	}
	else
	{
		wsprintf( szIDText, "Lv%d %s", iLevel, m_pOwner->GetCharName().c_str() );
	}

	int iIDWidth = g_FontMgr.GetTextWidth( szIDText, TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
	if( m_pGauge )
	{
		iIDWidth = max( iIDWidth, m_pGauge->GetGaugeBackWidth(g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID())) );
	}

	pRect->top    = iYPos - 11;
	pRect->bottom = iBottomY;
	pRect->left  = iXPos - iIDWidth * FLOAT05;
	pRect->right = iXPos + iIDWidth * FLOAT05;

	return true;
}

void ioBaseGUISupport::RunningManName( int iXPos, int iYPos )
{
	if( m_szRunningManName.IsEmpty() ) 
		return;
	if( m_pRunningManTitleLeft == NULL || m_pRunningManTitleCenter == NULL || m_pRunningManTitleRight == NULL ) 
		return;

	int iSize = g_FontMgr.GetTextWidth( m_szRunningManName.c_str(), TS_NORMAL, FONT_SIZE_12 );

	iXPos -= ( iSize + 14 ) / 2;

	m_pRunningManTitleLeft->Render( iXPos, iYPos );

	if( m_pRunningManTitleCenter->GetWidth() != iSize )
	{
		m_pRunningManTitleCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pRunningManTitleCenter->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pRunningManTitleCenter->SetSize( iSize, m_pRunningManTitleCenter->GetHeight() );
	}

	m_pRunningManTitleCenter->Render( iXPos + 7, iYPos );
	m_pRunningManTitleRight->Render( iXPos + 7 + iSize, iYPos );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.PrintText( iXPos + 7, iYPos + 3, FONT_SIZE_12, STR(1), m_szRunningManName.c_str() );
}

void ioBaseGUISupport::InitRaceInfoForRunningMan( const int& dwRunningManDecoIndex, const CHARACTER &rkCharInfo )
{
	// 런닝맨모드 치장 적용
	CHARACTER kCharInfo = rkCharInfo;
	ioCatchRunningManMode *pRunningManMode = ToCatchRunningManMode( m_pMode );
	if( pRunningManMode )
	{		
		pRunningManMode->GetModeFixedDeco( dwRunningManDecoIndex, m_pOwner, kCharInfo, m_pOwner->GetTeam() );	
		m_pRunningManTitleLeft   = pRunningManMode->GetModeNameBarLeft();
		m_pRunningManTitleCenter = pRunningManMode->GetModeNameBarCenter();
		m_pRunningManTitleRight  = pRunningManMode->GetModeNameBarRight();
	}
	else if( dwRunningManDecoIndex > 0 )
	{
		ioTrainingMode *pTrainingMode = ToTrainingMode( m_pMode );
		if( pTrainingMode )
		{
			pTrainingMode->GetModeFixedDeco( dwRunningManDecoIndex, m_pOwner, kCharInfo, m_pOwner->GetTeam() );	
			m_pRunningManTitleLeft   = pTrainingMode->GetModeNameBarLeft();
			m_pRunningManTitleCenter = pTrainingMode->GetModeNameBarCenter();
			m_pRunningManTitleRight  = pTrainingMode->GetModeNameBarRight();
		}
	}
}

bool ioBaseGUISupport::IsVisibleID()
{
	bool bCheckMode = false;
	if( g_GUIMgr.IsNoneUIMode() ||
		m_pCreator->HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) ||
		m_pCreator->HasCameraFixedStyle( CAMERA_STYLE_VIEW  ) )
		bCheckMode = true;

	if( bCheckMode && !Setting::NoUIModeIDShow() )
		return false;

	if( m_pMode && m_pMode->GetModeType() == MT_FIGHT_CLUB )
		return false;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
//Emotion
void ioBaseGUISupport::EndEmoticonByName( const ioHashString &rkDescName )
{
	if( !rkDescName.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->EndEmoticonByName( rkDescName );
	}
}

void ioBaseGUISupport::SetLongTimeLiveEmoticon( const ioHashString &szEmoticon )
{
	if( !szEmoticon.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->SetLongTimeLiveEmoticon( szEmoticon );
	}
}

void ioBaseGUISupport::SetLongTimeLiveEmoticonAndScale( const ioHashString &szEmoticon, bool bFast )
{
	if( !szEmoticon.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->SetLongTimeLiveEmoticonAndScale( szEmoticon, bFast );
	}
}

void ioBaseGUISupport::EndLongTimeLiveEmoticon()
{
	if( m_pCharEmoticon )
	{
		m_pCharEmoticon->EndLongTimeLiveEmoticon();
	}
}

void ioBaseGUISupport::SetEmoticon( const ioHashString &szEmoticonName, EmoticonType eType, DWORD dwDuration )
{
	if( !szEmoticonName.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->SetEmoticon( szEmoticonName, eType, m_pOwner->IsOwnerChar(), dwDuration );
	}
}

void ioBaseGUISupport::SetNamedEmoticon( const ioHashString &szIconName, const ioHashString &szDescName, DWORD dwColor , DWORD dwDuration /*= 0*/ )
{
	if( !szIconName.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->SetNamedEmoticon( szIconName, szDescName, dwColor, m_pOwner->IsOwnerChar(), dwDuration );
	}
}

void ioBaseGUISupport::SetNamedUnLimitedEmoticon( const ioHashString &szIconName, const ioHashString &szDescName, DWORD dwColor )
{
	if( !szIconName.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->SetNamedUnLimitedEmoticon( m_pOwner, szIconName, szDescName, dwColor );
	}
}

void ioBaseGUISupport::Set3DChatEmoticon( int iChatLen, bool bNoSound )
{
	if( iChatLen > 0 && m_pCharEmoticon )
	{
		m_pCharEmoticon->Set3DChatEmoticon( m_pOwner->GetCharacterInfo(), iChatLen, m_pOwner->IsOwnerChar(), bNoSound );
	}
}

void ioBaseGUISupport::SetHitEmoticon( const ioHashString &szEmoticonName, const ioHashString &szIconComboNum, int iHitComboCnt )
{
	if( !szEmoticonName.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->SetHitCombo( szEmoticonName, szIconComboNum, iHitComboCnt, m_pOwner->IsOwnerChar() );
	}
}

void ioBaseGUISupport::SetKillPointEmoticon( const ioHashString &szEmoticonName, const ioHashString &szIconNum, const ioHashString &szPer, int iNum )
{
	if( !szEmoticonName.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->SetKillPoint( szEmoticonName, szIconNum, szPer, iNum, m_pOwner->IsOwnerChar() );
	}
}

void ioBaseGUISupport::SetFishingGradeEmoticon( const ioHashString &szEmoticonName, const ioHashString &szGrade, DWORD dwDuration )
{
	if( !szEmoticonName.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->SetFishingGrade( szEmoticonName, szGrade, m_pOwner->IsOwnerChar(), dwDuration );
	}
}

void ioBaseGUISupport::SetFishingLevelUpEmoticon( const ioHashString &szEmoticonName, const ioHashString &szLevelUp, DWORD dwDuration )
{
	if( !szEmoticonName.IsEmpty() && m_pCharEmoticon )
	{
		m_pCharEmoticon->SetFishingLevelUp( szEmoticonName, szLevelUp, m_pOwner->IsOwnerChar(), dwDuration );
	}
}

void ioBaseGUISupport::SetUserRankEmoticon()
{
	if( !m_pCharEmoticon )
		return;

	if( !m_pMode )
		return;

	if( m_OwnerRankEmoticon.empty() && m_OtherRankEmoticon.empty() )
		return;

	int iRank = m_pMode->GetCurRank( m_pOwner->GetCharName() );
	if( iRank == -1 )
		return;

	iRank = max(1, min( iRank, MAX_PLAYER ));

	ioHashString szEmoticon;

	int iOwnerCnt = m_OwnerRankEmoticon.size();
	int iOtherCnt = m_OtherRankEmoticon.size();

	if( m_pOwner->IsOwnerChar() && iOwnerCnt > 0 )
	{
		szEmoticon = m_OwnerRankEmoticon[iRank-1];
	}
	else if( iOtherCnt > 0 )
	{
		ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
		if( pOwner && pOwner->GetTeam() == m_pOwner->GetTeam() )
		{
			szEmoticon = m_OtherRankEmoticon[iRank-1];
		}
	}

	if( !szEmoticon.IsEmpty() )
	{
		m_pCharEmoticon->SetUserRankEmoticon( szEmoticon, "", iRank, m_pOwner->IsOwnerChar() );
	}
}

void ioBaseGUISupport::SetTargetCommandEmoticon()
{
	if( m_TargetCommandEmoticon.IsEmpty() ) 
		return;

	SetEmoticon( m_TargetCommandEmoticon );
}

void ioBaseGUISupport::SetTargetingEmoticon()
{
	if( m_TargetingEmoticon.IsEmpty() ) 
		return;

	SetEmoticon( m_TargetingEmoticon );
}

void ioBaseGUISupport::SetMovieStartEmoticon( DWORD dwStartTime )
{
	if( dwStartTime == 0 )
	{
		if( m_dwMovieCapturingCurTime == 0 )
			m_dwMovieCapturingCurTime = FRAMEGETTIME();
	}
	else
	{
		m_dwMovieCapturingCurTime= 0 ;
		m_dwMovieCapStartCurTime = FRAMEGETTIME();
		m_dwMovieCapStartTime    = dwStartTime;
		m_iMovieStartSec		 = -1;
	}
}

void ioBaseGUISupport::SetMovieCutEmoticon()
{
	m_iMovieStartSec		 = -1;
	m_dwMovieCapStartCurTime = 0;
	m_dwMovieCapturingCurTime= 0;

	const ioHashString &szEmoticonName = g_MovieMaker.GetMovieEmoticonName( ioMovieMaker::COUNT_CUT );
	SetEmoticon( szEmoticonName, EMT_MOVIE_EVENT, 900 );
}

void ioBaseGUISupport::SetPraiseEmoticon( int iType, int iValue )
{
	ioModeHelpManager::HelpType eType = (ioModeHelpManager::HelpType)iType;
	const ioHashString *szIcon = g_ModeHelpMgr.GetIconName( eType );
	const vHelpText *pHelpList = g_ModeHelpMgr.GetHelpText( eType );

	if( !pHelpList || !szIcon )
	{
		LOG.PrintTimeAndLog( 0, "%s Emoticon is Not Exist", __FUNCTION__ );
		return;
	}

	if( pHelpList->empty() ) return;

	ioHashString szDesc, szTmp1, szTmp2;
	char szBuf[MAX_PATH] = "";

	DWORD dwColor = 0;
	int i = 0;
	for(vHelpText::const_iterator iter = pHelpList->begin(); iter != pHelpList->end(); ++iter, i++)
	{
		const HelpText &rkText = *iter;

		if( i == 0 )
		{
			szTmp1 = rkText.m_szHelp;
			dwColor = rkText.m_dwColor;
		}
		else if( i == 1 )
		{
			szTmp2 = rkText.m_szHelp;
		}
	}

	if( iType == ioModeHelpManager::HT_CONTRIBUTE_PRAISE )
	{
		wsprintf_e( szBuf, "%s %d %s", szTmp1.c_str(), iValue, szTmp2.c_str() );
		szDesc = szBuf;
	}
	else
	{
		szDesc = szTmp1;
	}

	SetNamedEmoticon( *szIcon, szDesc, dwColor );
}

void ioBaseGUISupport::SetDisableAttackEmoticon()
{
	if( m_pOwner->IsOnlyMyEmoticon() ) 
		return;

	if( !m_pOwner->IsOwnerChar() )
		SetEmoticon( m_DisableAttackEmoticon );
}

void ioBaseGUISupport::SetScreenShotEmoticon()
{
	SetEmoticon( m_ScreenShotEmoticon, EMT_GAME_EVENT );

	if( m_pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EMOTICON_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << EMOTICON_SCREEN_SHOT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseGUISupport::SetHomePageEmoticon()
{
	SetEmoticon( m_HomePageEmoticon, EMT_GAME_EVENT );

	if( m_pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EMOTICON_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << EMOTICON_HOME_PAGE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseGUISupport::SetBossLevelEmoticon( int iLevel )
{
	char szBossLevelEmoticon[MAX_PATH] = "";
	StringCbPrintf( szBossLevelEmoticon, sizeof( szBossLevelEmoticon ), m_BossEmoticon.c_str(), iLevel );
	SetEmoticon( ioHashString( szBossLevelEmoticon ), EMT_DEFAULT, 0xFFFFFF00 );        //무한	
}

void ioBaseGUISupport::SetNetworkBadEmoticon()
{
	const ioHashString *szIcon = g_ModeHelpMgr.GetIconName( ioModeHelpManager::HT_NETWORK_BAD );
	const vHelpText *pHelpList = g_ModeHelpMgr.GetHelpText( ioModeHelpManager::HT_NETWORK_BAD );
	if( !pHelpList || !szIcon )
	{
		LOG.PrintTimeAndLog( 0, "%s Emoticon is Not Exist", __FUNCTION__ );
		return;
	}

	if( pHelpList->empty() ) 
		return;

	const HelpText &rkText = (*pHelpList->begin());

	ioHashString szDesc = rkText.m_szHelp;
	DWORD dwColor = rkText.m_dwColor;

	SetNamedEmoticon( *szIcon, szDesc, dwColor );
}

void ioBaseGUISupport::SetBlockInterPlayEmoticon( const ioHashString& szInterPlayDesc, DWORD dwInterPlayDsecColor )
{
	//이모티콘 설정
	const ioHashString *szIcon	= g_ModeHelpMgr.GetIconName( ioModeHelpManager::HT_BLOCK_INTERPLAY );
	ioHashString szDesc			= szInterPlayDesc;
	DWORD dwColor				= dwInterPlayDsecColor;

	if( szDesc.IsEmpty() )
	{
		const vHelpText *pHelpList = g_ModeHelpMgr.GetHelpText( ioModeHelpManager::HT_BLOCK_INTERPLAY );
		if( pHelpList && !pHelpList->empty() )
		{
			const HelpText &rkText = (*pHelpList->begin());	
			szDesc	= rkText.m_szHelp;
			dwColor	= rkText.m_dwColor;
		}
	}
	SetNamedUnLimitedEmoticon( *szIcon, szDesc, dwColor );
}

void ioBaseGUISupport::EndUnLimitedEmoticon()
{
	if ( m_pCharEmoticon )
		m_pCharEmoticon->EndUnLimitedEmoticon();
}

void ioBaseGUISupport::UpdateEmoticon( bool bSystemVisible, ioBaseChar *pCameraTarget )
{
	bool bVisible = true;

	if( !bSystemVisible || (!m_pOwner->IsCanInvisible() && !m_pGrp->IsVisible()) )
	{
		bVisible = false;
	}
	else
	{
		switch( m_pOwner->GetState() )
		{
		case CS_ESCAPE_PRISONER:
		case CS_READY:
		case CS_LOADING:
		case CS_OBSERVER:
		case CS_VIEW:
			bVisible = false;
			break;
		case CS_DIE:
			if( m_pOwner->GetDieState() == DS_FADE_OUT )
			{
				bVisible = false;
			}
			break;
		}
	}

	if( pCameraTarget )
	{
		D3DXVECTOR3 vHeadPos = m_pOwner->GetMidPositionByRate( FLOAT1 );
		if( !m_pOwner->IsVisibleID() )
			vHeadPos.y += 20.0f;
		else if ( ( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && m_iHeroTitle != 0 ) || m_dwNamedTitleCode > 0)
			vHeadPos.y += 60.0f;
		else
			vHeadPos.y += 40.0f;

		m_pCharEmoticon->UpdateEmoticon( vHeadPos, pCameraTarget->GetWorldPosition(), bVisible );
	}
	else
	{
		m_pCharEmoticon->NoNeedRender();
	}
}

void ioBaseGUISupport::ProcessBossModeChar()
{
	if( !m_pOwner->IsBoss() ) 
		return;

	// 이모티콘 세팅
	if( m_pCharEmoticon && !m_pCharEmoticon->IsEmoticonEvent() )
	{
		const CharState& OwnerState = m_pOwner->GetState();
		if( OwnerState != CS_READY && 
			OwnerState != CS_DIE && 
			OwnerState != CS_VIEW && 
			OwnerState != CS_LOADING && 
			OwnerState != CS_OBSERVER )
		{
			ioBossMode *pBossMode = ToBossMode( m_pMode );
			if( pBossMode )
			{
				SetBossLevelEmoticon( pBossMode->GetCurBossLevel() );
			}			
		}
	}
}

void ioBaseGUISupport::ProcessMovieMakerEmoticon()
{
	if( m_dwMovieCapStartCurTime != 0 ) 
	{
		int iPrevSec = m_iMovieStartSec;
		m_iMovieStartSec = (FRAMEGETTIME() - m_dwMovieCapStartCurTime) / 1000;
		if( iPrevSec != m_iMovieStartSec )
		{
			int iMaxSec = m_dwMovieCapStartTime / 1000;
			if( m_iMovieStartSec == iMaxSec )
			{
				m_dwMovieCapStartCurTime = 0;
				m_iMovieStartSec = -1;	
			}
			else
			{
				if( m_iMovieStartSec == iMaxSec - 1 )
				{
					// 스타트!!!
					const ioHashString &szEmoticonName = g_MovieMaker.GetMovieEmoticonName( m_iMovieStartSec );
					if( m_pOwner->IsOwnerChar() || m_pCreator->GetModeType() == MT_MYROOM )
					{			
						m_dwMovieCapturingCurTime = 0;
						SetEmoticon( szEmoticonName, EMT_MOVIE_EVENT, 900 );
					}
					else if( CheckTeamEmoticon() )         // 팀 이모티콘이 나오게되면 동영상 이모티콘은 나오지 않는다.
					{
						m_dwMovieCapturingCurTime = FRAMEGETTIME();
						SetEmoticon( szEmoticonName, EMT_MOVIE_EVENT, 900 );    
					}
					else
					{
						m_dwMovieCapturingCurTime = FRAMEGETTIME();
						SetEmoticon( szEmoticonName, EMT_MOVIE_EVENT, 0xFFFFFF00 );        //무한
					}
				}
				else
				{
					const ioHashString &szEmoticonName = g_MovieMaker.GetMovieEmoticonName( m_iMovieStartSec );
					SetEmoticon( szEmoticonName, EMT_MOVIE_EVENT, 900 );
				}
			}
		}
	}	

	if( m_dwMovieCapturingCurTime != 0 )
	{
		if( m_pCharEmoticon && !m_pCharEmoticon->IsEmoticonEvent() )
		{
			m_dwMovieCapturingCurTime = FRAMEGETTIME();
			if( !CheckTeamEmoticon() )
			{
				const ioHashString &szEmoticonName = g_MovieMaker.GetMovieEmoticonName( ioMovieMaker::COUNT_START );
				SetEmoticon( szEmoticonName, EMT_CHAT_ICON, 0xFFFFFF00 );        //무한
			}
		}
	}
}

void ioBaseGUISupport::UpdateChatBubble()
{
	if( !m_pCharChatBubble || !m_pCharChatBubble->IsBubbleChat() )
		return;

	D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;

	int iXPos = 0;
	int iYPos = 0;
	Setting::Check2DInScreen( vPos, iXPos, iYPos );
	m_pCharChatBubble->UpdateChatBubble( iXPos, iYPos - 76 );
}

void ioBaseGUISupport::RenderEmoticon( const RECT *pTop, const RECT *pBottom, bool bCollisionRender )
{
	ErrorReport::SetPosition( 1200, 600 );

	if( m_pCharChatBubble && m_pCharChatBubble->IsBubbleChat() )
		return;

	if( !m_pCharEmoticon || !m_pCharEmoticon->IsNeedRender() )
		return;

	if( m_bEnableFlagPointGauge )
		return;

	ErrorReport::SetPosition( 1200, 601 );

	if( !pTop || !pBottom )
	{
		ErrorReport::SetPosition( 1200, 602 );
		m_pCharEmoticon->RenderEmoticon();
		ErrorReport::SetPosition( 1200, 603 );
		return;
	}

	ErrorReport::SetPosition( 1200, 604 );

	RECT rcDest;
	RECT rcEmoticon = m_pCharEmoticon->GetEmoticonRenderRect();

	ErrorReport::SetPosition( 1200, 605 );

	if( IntersectRect( &rcDest, pTop, &rcEmoticon ) || IntersectRect( &rcDest, pBottom, &rcEmoticon ) )
	{
		ErrorReport::SetPosition( 1200, 606 );

		if( bCollisionRender )
		{
			ErrorReport::SetPosition( 1200, 607 );
			m_pCharEmoticon->RenderEmoticon();
			ErrorReport::SetPosition( 1200, 608 );
		}
	}
	else
	{
		ErrorReport::SetPosition( 1200, 609 );

		if( !bCollisionRender )
		{
			ErrorReport::SetPosition( 1200, 610 );
			m_pCharEmoticon->RenderEmoticon();
			ErrorReport::SetPosition( 1200, 611 );
		}
	}

	ErrorReport::SetPosition( 1200, 612 );
}


void ioBaseGUISupport::SetChatBubble( const char *szID, const char *szChat, bool bWholeChat, DWORD dwCurrentTime, DWORD dwDuration )
{
	if( szChat == NULL ) 
		return;
	if( strlen( szChat ) == 0 ) 
		return;

	if( !m_pCharChatBubble ) 
		return;

	m_pCharChatBubble->SetChatBubble( szID, szChat, bWholeChat );
	m_pCharChatBubble->SetChatBubbleTime( dwCurrentTime );

	if( dwDuration > 0 )
		m_pCharChatBubble->SetChatBubbleDuration( dwDuration );

	m_pCharChatBubble->SetChatBubbleAniTime( 0 );  // 시작 애니메이션은 하지 않는다.
}

void ioBaseGUISupport::SetChatBubble( const char *szID, const char *szChat, bool bWholeChat )
{
	if( !m_pCharChatBubble ) 
		return;

	m_pCharChatBubble->SetChatBubble( szID, szChat, bWholeChat );
}

void ioBaseGUISupport::SetChatBubbleEmoticon( const ioHashString &rkEmoticonName )
{
	if( !m_pCharChatBubble ) 
		return;

	m_pCharChatBubble->SetChatBubbleEmoticon( rkEmoticonName );
}

void ioBaseGUISupport::Set3DChatBubbleEmoticon( int iChatLen, bool bNoSound )
{
	if( iChatLen > 0 && m_pCharChatBubble )
	{
		m_pCharChatBubble->Set3DChatBubbleEmoticon( m_pOwner->GetCharacterInfo(), iChatLen, m_pOwner->IsOwnerChar(), bNoSound );
	}
}

bool ioBaseGUISupport::IsVisibleBubbleChat()
{
	bool bCheckMode = false;
	if( g_GUIMgr.IsNoneUIMode() ||
		m_pCreator->HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) ||
		m_pCreator->HasCameraFixedStyle( CAMERA_STYLE_VIEW  ) )
		bCheckMode = true;

	if( bCheckMode && !Setting::NoUIModeChatBubbleShow() )
		return false;

	return true;
}

void ioBaseGUISupport::UpdateShuffleModeUI( bool bSystemVisible, ioBaseChar *pCameraTarget )
{
	if( m_pShuffleCharUI )
		m_pShuffleCharUI->UpdateShuffleModeUI( bSystemVisible, m_pOwner, pCameraTarget );
}

void ioBaseGUISupport::RenderShuffleModeUI( const RECT *pTop, const RECT *pBottom, bool bCollisionRender )
{
	if( m_pCharChatBubble && m_pCharChatBubble->IsBubbleChat() )
		return;

	if( m_pCharEmoticon &&m_pCharEmoticon->IsNeedRender() )
		return;

	if( m_pShuffleCharUI )
		m_pShuffleCharUI->RenderShuffleModeUI( m_pOwner );
}

bool ioBaseGUISupport::IsEmoticonEvent()
{
	if( m_pCharEmoticon )
		return m_pCharEmoticon->IsEmoticonEvent();

	return false;
}

void ioBaseGUISupport::ProcessBattleRoomTeamEmoticon()
{
	if( !CheckTeamEmoticon() ) 
		return;
	if( !m_pCharEmoticon ) 
		return;
	if( m_pCharEmoticon->IsEmoticonEvent() ) 
		return;
	switch( m_pOwner->GetState() )
	{
	case CS_DIE:
	case CS_VIEW:
	case CS_LOADING:
	case CS_OBSERVER:
		return;
	}

	const ioHashString &szEmoticonName = g_BattleRoomMgr.GetTeamEmoticon( m_pOwner->GetCharName() );
	if( !szEmoticonName.IsEmpty() )
		SetEmoticon( szEmoticonName, EMT_CHAT_ICON, 0xFFFFFF00 );        //무한
}

bool ioBaseGUISupport::CheckTeamEmoticon() const
{
	if( !g_BattleRoomMgr.IsTeamEmoticon() ) 
		return false;
	if( !m_pCreator ) 
		return false;
	if( m_pCreator->GetModeType() == MT_MYROOM || m_pCreator->GetModeType() == MT_TRAINING || m_pCreator->GetModeType() == MT_HEADQUARTERS || m_pCreator->GetModeType() == MT_HOUSE ) 
		return false;

	return true;
}

void ioBaseGUISupport::SetNamedTitle( DWORD dwCode, int nLevel, BYTE byPremium )
{
	m_dwNamedTitleCode = dwCode;
	m_nNamedTitleLevel = nLevel;
	m_byNamedTitlePremium = byPremium;

	SAFEDELETE( m_pNamedTitleMark );

	NamedTitleGrade sNamedTitleGrade;
	if ( !g_NamedTitleInfoMgr.GetNamedTitleGrade( m_dwNamedTitleCode, m_nNamedTitleLevel, sNamedTitleGrade ) )
		return;

	m_pNamedTitleMark = g_UIImageSetMgr.CreateImageByFullName( sNamedTitleGrade.m_szMark );
}