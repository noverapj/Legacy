

#include "stdafx.h"

#include "ioAlphaRateBuff.h"
#include "ioBaseChar.h"

ioAlphaRateBuff::ioAlphaRateBuff()
{
}

ioAlphaRateBuff::ioAlphaRateBuff( const ioAlphaRateBuff &rhs )
	: ioBuff( rhs ),
	m_fAlphaRate( rhs.m_fAlphaRate ),
	m_dwSlerpTime( rhs.m_dwSlerpTime ),
	//m_bTargetBuff( rhs.m_bTargetBuff ),
	m_bEnableBuffCheck( rhs.m_bEnableBuffCheck ),
	m_bEnableBuffCheckWound( rhs.m_bEnableBuffCheckWound ),
	m_bEnableBuffCheckBlowWound( rhs.m_bEnableBuffCheckBlowWound ),
	m_bEnableBuffCheckStun( rhs.m_bEnableBuffCheckStun ),
	m_bDisableBuffCheck( rhs.m_bDisableBuffCheck ),
	m_bDisableBuffCheckBuffFlying( rhs.m_bDisableBuffCheckBuffFlying ),
	m_bDisableBuffCheckBreakFall( rhs.m_bDisableBuffCheckBreakFall ),
	m_bUseDraculaOption( rhs.m_bUseDraculaOption )
{
}

ioAlphaRateBuff::~ioAlphaRateBuff()
{
}

void ioAlphaRateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fAlphaRate = rkLoader.LoadFloat_e( "alpha_rate", FLOAT1 );
	m_dwSlerpTime = rkLoader.LoadInt_e( "slerp_time", 1 );
	//m_bTargetBuff = rkLoader.LoadBool_e( "target_buff", false );

	m_bEnableBuffCheck          = rkLoader.LoadBool_e( "enable_buff_check", false );
	m_bEnableBuffCheckWound     = rkLoader.LoadBool_e( "enable_buff_check_wound", false );
	m_bEnableBuffCheckBlowWound = rkLoader.LoadBool_e( "enable_buff_check_blow_wound", false );
	m_bEnableBuffCheckStun      = rkLoader.LoadBool_e( "enable_buff_check_stun", false );

	m_bDisableBuffCheck           = rkLoader.LoadBool_e( "disable_buff_check", false );
	m_bDisableBuffCheckBuffFlying = rkLoader.LoadBool_e( "disable_buff_check_buff_flying", false );
	m_bDisableBuffCheckBreakFall  = rkLoader.LoadBool_e( "disable_buff_check_BreakFall", false );

	m_bUseDraculaOption			  = rkLoader.LoadBool_e( "use_dracula_option", false );
}

ioBuff* ioAlphaRateBuff::Clone()
{
	return new ioAlphaRateBuff( *this );
}

void ioAlphaRateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bSlerpEnd = false;
}

bool ioAlphaRateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bSlerpEnd = false;

	return true;
}

void ioAlphaRateBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;

	/*if( !m_bTargetBuff )
	{
		SetReserveEndBuff();
		return;
	}*/

	DWORD dwCurTime = FRAMEGETTIME();

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

	if( !m_pOwner->IsCanInvisible() && !m_bSlerpEnd )
	{
		DWORD dwGap = dwCurTime - m_dwBuffStartTime;
		float fGapRate = (float)dwGap / (float)m_dwSlerpTime;
		fGapRate = min( fGapRate, FLOAT1 );
		int iAlphaRate =  (1.0f - (1.0f - m_fAlphaRate)*fGapRate ) * MAX_ALPHA_RATE;

		if( fGapRate == FLOAT1 )
			m_bSlerpEnd = true;

		iAlphaRate = max( 1, iAlphaRate );
		m_pOwner->SetAlphaRateDirect( iAlphaRate );
	}

	//드라큘라 전용 코딩
	if( m_bUseDraculaOption )
	{
		if( m_pOwner && (m_pOwner->GetState() != CS_DRACULA_FLY 
						&& m_pOwner->GetState() != CS_CROW_SPECIAL_STATE
						&& m_pOwner->GetState() != CS_NIGHTMARE_SPECIAL ) )
		{
			SetReserveEndBuff();
			return;
		}
	}
}

void ioAlphaRateBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner )
	{
		if( !m_pOwner->IsCanInvisible() )
			pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	}

	ioBuff::EndBuff();
}
