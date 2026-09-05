
#include "stdafx.h"

#include "ioNoviceGradeEffectBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioNoviceGradeEffectBuff::ioNoviceGradeEffectBuff()
{
	m_bCreateEffect = false;
}

ioNoviceGradeEffectBuff::ioNoviceGradeEffectBuff( const ioNoviceGradeEffectBuff &rhs )
	: ioBuff( rhs ),
	  m_szNoviceGradeEmotion( rhs.m_szNoviceGradeEmotion ),
	  m_szNoviceGradeEffect( rhs.m_szNoviceGradeEffect ),
	  m_dwNoviceGradeEmotionTime( rhs.m_dwNoviceGradeEmotionTime )
{
	m_bCreateEffect = false;
}

ioNoviceGradeEffectBuff::~ioNoviceGradeEffectBuff()
{
}

void ioNoviceGradeEffectBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "novice_emotion", "", szBuf, MAX_PATH );
	m_szNoviceGradeEmotion = szBuf;

	rkLoader.LoadString_e( "novice_effect", "", szBuf, MAX_PATH );
	m_szNoviceGradeEffect = szBuf;

	m_dwNoviceGradeEmotionTime = rkLoader.LoadInt_e( "novice_emotion_time", 0 );
}

ioBuff* ioNoviceGradeEffectBuff::Clone()
{
	return new ioNoviceGradeEffectBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNoviceGradeEffectBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetEmoticon( m_szNoviceGradeEmotion, EMT_SKILL_BUFF, m_dwNoviceGradeEmotionTime );
	m_bCreateEffect = false;
}

bool ioNoviceGradeEffectBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioNoviceGradeEffectBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if ( m_pOwner )
	{
		if ( !m_bCreateEffect && m_pOwner->GetAwakeType() == AWAKE_NONE )
		{
			m_pOwner->AttachEffect( m_szNoviceGradeEffect );
			m_bCreateEffect = true;
		}
		else if ( m_bCreateEffect && m_pOwner->GetAwakeType() != AWAKE_NONE )
		{
			m_pOwner->EndEffect( m_szNoviceGradeEffect, false );
			m_bCreateEffect = false;
		}
	}
}

void ioNoviceGradeEffectBuff::EndBuff()
{
	ioBuff::EndBuff();

	if ( m_pOwner )
		m_pOwner->EndEffect( m_szNoviceGradeEffect, false );
}