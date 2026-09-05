

#include "stdafx.h"

#include "ioLimitActionBuff.h"
#include "ioBaseChar.h"



ioLimitActionBuff::ioLimitActionBuff()
{
}

ioLimitActionBuff::ioLimitActionBuff( const ioLimitActionBuff &rhs )
: ioBuff( rhs ),
 m_bLimitAttack( rhs.m_bLimitAttack ),
 m_bLimitDefense( rhs.m_bLimitDefense ),
 m_bLimitJump( rhs.m_bLimitJump ),
 m_bLimitExtendJump( rhs.m_bLimitExtendJump ),
 m_bLimitSkill( rhs.m_bLimitSkill ),
 m_bLimitDash( rhs.m_bLimitDash ),
 m_bEnableOwnerItemSkill( rhs.m_bEnableOwnerItemSkill ),
 m_bDisableDownState( rhs.m_bDisableDownState ),
 m_bDisableDropState( rhs.m_bDisableDropState )
{
}

ioLimitActionBuff::~ioLimitActionBuff()
{
}

void ioLimitActionBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bLimitAttack = rkLoader.LoadBool_e( "limit_attack", false );
	m_bLimitDefense = rkLoader.LoadBool_e( "limit_defense", false );
	m_bLimitJump = rkLoader.LoadBool_e( "limit_jump", false );
	m_bLimitExtendJump = rkLoader.LoadBool_e( "limit_extend_jump", false );
	m_bLimitSkill = rkLoader.LoadBool_e( "limit_skill", false );
	m_bLimitDash = rkLoader.LoadBool_e( "limit_dash", false );

	m_bEnableOwnerItemSkill = rkLoader.LoadBool_e( "enable_owner_item_skill", false );

	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropState = rkLoader.LoadBool_e( "disable_drop_state", false );
}

ioBuff* ioLimitActionBuff::Clone()
{
	return new ioLimitActionBuff( *this );
}

void ioLimitActionBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_pOwner )
		m_pOwner->SetLimitActionBuff( m_bLimitAttack, m_bLimitDefense, m_bLimitJump, m_bLimitExtendJump, m_bLimitSkill, m_bLimitDash );
}

bool ioLimitActionBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_pOwner )
		m_pOwner->SetLimitActionBuff( m_bLimitAttack, m_bLimitDefense, m_bLimitJump, m_bLimitExtendJump, m_bLimitSkill, m_bLimitDash );

	return true;
}

void ioLimitActionBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;

	CheckOwnerStateCheck();

	DWORD dwCurTime = FRAMEGETTIME();
	float fCurTicGauge = GetCurTicGauge();
	int iTickGap = GetCurTicGap();
	int iCurTicTime = GetCurTicTime();

	/*if( m_pOwner )
	{
		ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
		if( !pCreator )
		{
			SetReserveEndBuff();
			return;
		}
	}*/

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

			if( fCurTicGauge != 0.0f )
			{
				if( !IsRightOwnerItem() )
				{
					SetReserveEndBuff();
					return;
				}

				if( m_pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, fCurTicGauge ) <= 0.0f )
				{
					// 점프중일때는 끝나지 않게...
					if( m_pOwner->GetState() != CS_JUMP || m_pOwner->GetJumpState() != JS_JUMPPING )
					{
						SetReserveEndBuff();
						return;
					}
				}
			}
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			// 점프중일때는 끝나지 않게...
			if( m_pOwner->GetState() != CS_JUMP || m_pOwner->GetJumpState() != JS_JUMPPING )
			{
				if( m_dwBuffEndTime <= FRAMEGETTIME() )
				{
					SetReserveEndBuff();
					return;
				}
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioLimitActionBuff::EndBuff()
{
	if( m_pOwner )
		m_pOwner->RemoveLimitActionBuff( m_Name );

	ioBuff::EndBuff();
}

void ioLimitActionBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{	
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}

	if ( m_bDisableDownState || m_bDisableDropState )
	{
		if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
	}
}
