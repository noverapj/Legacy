

#include "stdafx.h"

#include "ioNoWoundBuff.h"
#include "ioBaseChar.h"

ioNoWoundBuff::ioNoWoundBuff()
{
}

ioNoWoundBuff::ioNoWoundBuff( const ioNoWoundBuff &rhs )
: ioBuff( rhs ),
 m_fAlphaRate( rhs.m_fAlphaRate ),
 m_dwSlerpTime( rhs.m_dwSlerpTime ),
 m_bTargetBuff( rhs.m_bTargetBuff ),
 m_bEnableBuffCheck( rhs.m_bEnableBuffCheck ),
 m_bEnableBuffCheckWound( rhs.m_bEnableBuffCheckWound ),
 m_bEnableBuffCheckBlowWound( rhs.m_bEnableBuffCheckBlowWound ),
 m_bEnableBuffCheckStun( rhs.m_bEnableBuffCheckStun ),
 m_bDisableBuffCheck( rhs.m_bDisableBuffCheck ),
 m_bDisableBuffCheckBuffFlying( rhs.m_bDisableBuffCheckBuffFlying ),
 m_bDisableBuffCheckBreakFall( rhs.m_bDisableBuffCheckBreakFall ),
 m_bSetDefBuff( rhs.m_bSetDefBuff ),
 m_bDisableDownState( rhs.m_bDisableDownState )
{
}

ioNoWoundBuff::~ioNoWoundBuff()
{
}

void ioNoWoundBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fAlphaRate = rkLoader.LoadFloat_e( "alpha_rate", FLOAT1 );
	m_dwSlerpTime = rkLoader.LoadInt_e( "slerp_time", 1 );
	m_bTargetBuff = rkLoader.LoadBool_e( "target_buff", false );

	m_bEnableBuffCheck          = rkLoader.LoadBool_e( "enable_buff_check", false );
	m_bEnableBuffCheckWound     = rkLoader.LoadBool_e( "enable_buff_check_wound", false );
	m_bEnableBuffCheckBlowWound = rkLoader.LoadBool_e( "enable_buff_check_blow_wound", false );
	m_bEnableBuffCheckStun      = rkLoader.LoadBool_e( "enable_buff_check_stun", false );

	m_bDisableBuffCheck           = rkLoader.LoadBool_e( "disable_buff_check", false );
	m_bDisableBuffCheckBuffFlying = rkLoader.LoadBool_e( "disable_buff_check_buff_flying", false );
	m_bDisableBuffCheckBreakFall  = rkLoader.LoadBool_e( "disable_buff_check_BreakFall", false );

	m_bSetDefBuff = rkLoader.LoadBool_e( "set_defense_buff", false );
	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioNoWoundBuff::Clone()
{
	return new ioNoWoundBuff( *this );
}

void ioNoWoundBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bSlerpEnd = false;
	pOwner->SetNoWoundState( true );
}

bool ioNoWoundBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bSlerpEnd = false;

	if( pOwner )
	{
		pOwner->SetNoWoundState( true );
	}

	return true;
}

void ioNoWoundBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;

	if ( CheckOwnerStateCheck() )
	{
		SetReserveEndBuff();
		return;
	}

	if( !m_bTargetBuff && !m_pOwner->IsCanNoWound() )
	{
		SetReserveEndBuff();
		return;
	}

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
}

void ioNoWoundBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner )
	{
		if( !m_pOwner->IsCanInvisible() )
			pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );

		pOwner->SetNoWoundState( false );
	}

	ioBuff::EndBuff();
}

bool ioNoWoundBuff::EnableBuffCheck( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if ( m_bSetDefBuff )
		return true;

	if( !m_bEnableBuffCheck )
		return false;
	
	CharState eState = pOwner->GetState();
	if( m_bEnableBuffCheckWound && eState == CS_WOUNDED )
		return true;

	if( m_bEnableBuffCheckBlowWound && eState == CS_BLOW_WOUNDED )
		return true;

	if( m_bEnableBuffCheckStun )
	{
		if( pOwner->HasBuff( BT_STUN_STATUE ) ||
			pOwner->HasBuff( BT_ENABLE_WOUND_STUN_STATUE ) ||
			pOwner->HasBuff( BT_CLOSEORDER ) ||
			pOwner->HasBuff( BT_HIT_COUNT_HOLD ) )
			return true;
	}

	return false;
}

bool ioNoWoundBuff::DisableBuffCheck( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !m_bDisableBuffCheck )
		return false;

	if( pOwner->GetState() == CS_JUMP )
	{
		if( m_bDisableBuffCheckBuffFlying )
		{
			if( pOwner->GetUsedBuffJump() == BJUT_CUR || pOwner->GetUsedBuffJump() == BJUT_CONTINUE )
				return true;
			if( pOwner->GetUsedBuffFlyJump() == BJUT_CUR || pOwner->GetUsedBuffFlyJump() == BJUT_CONTINUE )
				return true;
			if( pOwner->GetUsedBuffMultiFlyJump() == BJUT_CUR || pOwner->GetUsedBuffMultiFlyJump() == BJUT_CONTINUE )
				return true;
		}

		if( m_bDisableBuffCheckBreakFall )
		{
			if( pOwner->GetJumpState() == JS_BREAK_FALL )
				return true;
		}
	}

	return false;
}

bool ioNoWoundBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) 
		return true;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{	
		if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
	}

	return bDown;
}