
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioHitCountHoldBuff2.h"

#include "BuffDefine.h"

ioHitCountHoldBuff2::ioHitCountHoldBuff2()
{
	m_iCurHitCount = 0;
}

ioHitCountHoldBuff2::ioHitCountHoldBuff2( const ioHitCountHoldBuff2 &rhs )
	: ioBuff( rhs ),
	m_bSetFloatState( rhs.m_bSetFloatState ),
	m_FloatBuffInfo( rhs.m_FloatBuffInfo ),
	m_HoldAnimation( rhs.m_HoldAnimation ),
	m_HoldAirAnimation( rhs.m_HoldAirAnimation ),
	m_iMaxHitCount( rhs.m_iMaxHitCount ),
	m_bEnableTeamAttack( rhs.m_bEnableTeamAttack ),
	m_bUseSuccubus( rhs.m_bUseSuccubus ),
	m_bUseHades( rhs.m_bUseHades ),
	m_bMidnight( rhs.m_bMidnight )
{
	m_iCurHitCount = 0;
	m_bNotUseFloatState = false;
}

ioHitCountHoldBuff2::~ioHitCountHoldBuff2()
{
}

void ioHitCountHoldBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "";

	m_iMaxHitCount = rkLoader.LoadInt_e( "hit_count", 0 );
	m_bEnableTeamAttack = rkLoader.LoadBool_e( "enable_team_attack", false );

	// FloatInfo
	m_bSetFloatState = rkLoader.LoadBool_e( "set_float_state", false );
	m_FloatBuffInfo.m_bStopAction = rkLoader.LoadBool_e( "float_action_stop", false );

	m_FloatBuffInfo.m_fFloatJumpPower = rkLoader.LoadFloat_e( "float_jump_power", 0.0f );
	m_FloatBuffInfo.m_fFloatGravityAmt = rkLoader.LoadFloat_e( "float_gravity_amt", 0.0f );
	m_FloatBuffInfo.m_fFloatFrictionAmt = rkLoader.LoadFloat_e( "float_friction_amt", 0.0f );
	m_FloatBuffInfo.m_fFloatBlowRate = rkLoader.LoadFloat_e( "float_blow_rate", FLOAT1 );
	m_FloatBuffInfo.m_fFloatSpeedRate = rkLoader.LoadFloat_e( "float_speed_rate", FLOAT1 );
	m_FloatBuffInfo.m_fFloatEndJumpPower = rkLoader.LoadFloat_e( "float_end_jump_power", 0.1f );

	m_FloatBuffInfo.m_bEndJumpState = rkLoader.LoadBool_e( "enable_end_jump_state", false );

	// 버프 특성상 이부분은 하드 코딩으로..
	m_FloatBuffInfo.m_bEnableDamage = true;
	m_FloatBuffInfo.m_bEnableTeamAttack = m_bEnableTeamAttack;

	m_FloatBuffInfo.m_iCancelType = rkLoader.LoadInt_e( "cancel_type", 0 );
	m_FloatBuffInfo.m_iMoveType = rkLoader.LoadInt_e( "move_type", 0 );
	m_FloatBuffInfo.m_iLandType = rkLoader.LoadInt_e( "land_type", 0 );

	m_FloatBuffInfo.m_dwFloatMinTime = rkLoader.LoadInt_e( "float_min_time", 0 );
	m_FloatBuffInfo.m_dwFloatMaxTime = rkLoader.LoadInt_e( "float_max_time", 0 );

	rkLoader.LoadString_e( "hold_animation", "", szBuf, MAX_PATH );
	m_HoldAnimation = szBuf;
	rkLoader.LoadString_e( "hold_air_animation", "", szBuf, MAX_PATH );
	m_HoldAirAnimation = szBuf;

	m_bUseSuccubus	= rkLoader.LoadBool_e( "only_succubus", 0 );
	m_bUseHades		= rkLoader.LoadBool_e( "only_hades", 0 );
	m_bMidnight		= rkLoader.LoadBool_e( "only_midnight", 0 );
}

ioBuff* ioHitCountHoldBuff2::Clone()
{
	return new ioHitCountHoldBuff2( *this );
}

void ioHitCountHoldBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( !pOwner )
		return;

	m_iCurHitCount = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( m_bSetFloatState )
	{
		if( !m_HoldAirAnimation.IsEmpty() )
		{
			pGrp->ClearAllLoopAni( 0.0f, true );
			pGrp->ClearAllActionAni( 0.0f, true );
			pGrp->SetLoopAni( m_HoldAirAnimation, FLOAT100 );
		}

		ioAffectWoundedValueBuff *pBuff = ToAffectWoundedValue( pOwner->GetAffectWoundedValueBuff() );
		if( pBuff )
		{
			if( pBuff->CheckTeamAttack() )
			{
				if( GetOwner() && GetCreator() && GetOwner()->GetTeam() != GetCreator()->GetTeam() )
				{
					m_FloatBuffInfo.m_fFloatJumpPower *= pBuff->GetFloatJumpPowerRate();
					m_FloatBuffInfo.m_fFloatGravityAmt *= pBuff->GetFloatGravityRate();
					m_FloatBuffInfo.m_fFloatFrictionAmt *= pBuff->GetFloatFrictionRate();
				}
			}
			else
			{
				m_FloatBuffInfo.m_fFloatJumpPower *= pBuff->GetFloatJumpPowerRate();
				m_FloatBuffInfo.m_fFloatGravityAmt *= pBuff->GetFloatGravityRate();
				m_FloatBuffInfo.m_fFloatFrictionAmt *= pBuff->GetFloatFrictionRate();
			}
		}

		pOwner->SetFloatStateValue( m_FloatBuffInfo );
		pOwner->SetState( CS_FLOAT_STATE );
	}
	/*else
	{
		if( !m_HoldAnimation.IsEmpty() )
		{
			pGrp->ClearAllLoopAni( 0.0f, true );
			pGrp->ClearAllActionAni( 0.0f, true );
			pGrp->SetLoopAni( m_HoldAnimation, FLOAT100 );
		}
		pOwner->SetState( CS_ETC_STATE );
	}*/
}

bool ioHitCountHoldBuff2::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( m_bSetFloatState )
	{
		if( !m_HoldAirAnimation.IsEmpty() )
		{
			pGrp->ClearAllLoopAni( 0.0f, true );
			pGrp->ClearAllActionAni( 0.0f, true );
			pGrp->SetLoopAni( m_HoldAirAnimation, FLOAT100 );
		}

		ioAffectWoundedValueBuff *pBuff = ToAffectWoundedValue( pOwner->GetAffectWoundedValueBuff() );
		if( pBuff )
		{
			if( pBuff->CheckTeamAttack() )
			{
				if( GetOwner() && GetCreator() && GetOwner()->GetTeam() != GetCreator()->GetTeam() )
				{
					m_FloatBuffInfo.m_fFloatJumpPower *= pBuff->GetFloatJumpPowerRate();
					m_FloatBuffInfo.m_fFloatGravityAmt *= pBuff->GetFloatGravityRate();
					m_FloatBuffInfo.m_fFloatFrictionAmt *= pBuff->GetFloatFrictionRate();
				}
			}
			else
			{
				m_FloatBuffInfo.m_fFloatJumpPower *= pBuff->GetFloatJumpPowerRate();
				m_FloatBuffInfo.m_fFloatGravityAmt *= pBuff->GetFloatGravityRate();
				m_FloatBuffInfo.m_fFloatFrictionAmt *= pBuff->GetFloatFrictionRate();
			}
		}

		pOwner->SetFloatStateValue( m_FloatBuffInfo );
		pOwner->SetState( CS_FLOAT_STATE );
	}
	/*else
	{
		if( !m_HoldAnimation.IsEmpty() )
		{
			pGrp->ClearAllLoopAni( 0.0f, true );
			pGrp->ClearAllActionAni( 0.0f, true );
			pGrp->SetLoopAni( m_HoldAnimation, FLOAT100 );
		}
		pOwner->SetState( CS_ETC_STATE );
	}*/

	return true;
}


void ioHitCountHoldBuff2::SetFloatState( bool b )
{
	m_bSetFloatState = b;
}


void ioHitCountHoldBuff2::ProcessBuff( float fTimePerSec )
{
	if( m_iMaxHitCount > 0 && m_iMaxHitCount - m_iCurHitCount <= 0 )
	{
		SetReserveEndBuff();
		return;
	}

	CheckOwnerState();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_bUseHades )
	{
		ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
		if( !pCreator || 
			( pCreator->GetState() != CS_HADES_ATTACK && 
			pCreator->GetState() != CS_AUTOMATON_SPECIAL && 
			pCreator->GetState() != CS_USING_SKILL) )
			SetReserveEndBuff();
	}
	else if( m_bMidnight )
	{
		ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
		if( !pCreator || ( pCreator->GetState() != CS_MIDNIGHT_SPECIAL && pCreator->GetState() != CS_USING_SKILL) )
			SetReserveEndBuff();
	}
}

void ioHitCountHoldBuff2::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( pOwner->GetState() == CS_DROP_ZONE_DOWN )
		return;

	if( m_bNotUseFloatState )
		return;

	if( pOwner->GetState() != CS_DIE &&
		pOwner->GetState() != CS_LOADING &&
		pOwner->GetState() != CS_OBSERVER &&
		pOwner->GetState() != CS_VIEW )
	{
		CharState eState = pOwner->GetState();
		if( m_bSetFloatState && eState != CS_FLOAT_STATE )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( !m_HoldAirAnimation.IsEmpty() )
			{
				pGrp->ClearAllLoopAni( 0.0f, true );
				pGrp->ClearAllActionAni( 0.0f, true );
				pGrp->SetLoopAni( m_HoldAirAnimation, FLOAT100 );
			}
			pOwner->SetState( CS_FLOAT_STATE );
		}
		/*else if( !m_bSetFloatState && eState != CS_ETC_STATE )
		{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( !m_HoldAnimation.IsEmpty() )
		{
		pGrp->ClearAllLoopAni( 0.0f, true );
		pGrp->ClearAllActionAni( 0.0f, true );
		pGrp->SetLoopAni( m_HoldAnimation, FLOAT100 );
		}
		pOwner->SetState( CS_ETC_STATE );
		}*/
	}
}


void ioHitCountHoldBuff2::SetNotUseFloatState( bool b)
{
	m_bNotUseFloatState = b;
}

void ioHitCountHoldBuff2::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		ioBuff::EndBuff();
		return;
	}

	if( pOwner->GetState() == CS_FLOAT_STATE )
	{
		if( m_FloatBuffInfo.m_bEndJumpState )
		{
			// HARDCODE : 동기화 위해서 0.1f 부여
			pOwner->SetSKillEndJumpState( m_FloatBuffInfo.m_fFloatEndJumpPower, false, true );
		}
		else
		{
			BlowWoundAniInfo kInfo;
			pOwner->SetBlowWoundedState( true, kInfo );
		}
	}
	/*else if( pOwner->GetState() == CS_ETC_STATE )
	{
		pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		pOwner->SetDashFailStart();
	}*/

	ioBuff::EndBuff();
}

void ioHitCountHoldBuff2::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_iCurHitCount;
}

void ioHitCountHoldBuff2::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurHitCount;
}

void ioHitCountHoldBuff2::IncreaseHitCount()
{
	m_iCurHitCount++;
	m_iCurHitCount = min( m_iCurHitCount, m_iMaxHitCount );
}