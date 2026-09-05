

#include "stdafx.h"


#include "ioBaseChar.h"

#include "ioDownJumppingBuff.h"

ioDownJumppingBuff::ioDownJumppingBuff()
{
}

ioDownJumppingBuff::ioDownJumppingBuff( const ioDownJumppingBuff &rhs )
: ioBuff( rhs ),
m_fJumppingPowerRate( rhs.m_fJumppingPowerRate ),
m_dwEnableTime( rhs.m_dwEnableTime ),
m_bInitialGauge( rhs.m_bInitialGauge )
{
}

ioDownJumppingBuff::~ioDownJumppingBuff()
{
}

void ioDownJumppingBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fJumppingPowerRate = rkLoader.LoadFloat_e( "jumpping_power_rate", FLOAT1 );
	m_dwEnableTime = rkLoader.LoadInt_e( "enable_check_time", 0 );

	m_bInitialGauge = rkLoader.LoadBool_e( "initial_gauge", false );
}

ioBuff* ioDownJumppingBuff::Clone()
{
	return new ioDownJumppingBuff( *this );
}

void ioDownJumppingBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioDownJumppingBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGlobalTime ) )
		return false;

	return true;
}

void ioDownJumppingBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		if( m_pOwner->GetState() == CS_BLOW_WOUNDED ||
			m_pOwner->GetState() == CS_BOUND_BLOW_WOUNDED )
		{
			CheckDownJumpping();
		}

		CheckCallingAreaWeapon();
		break;
	}
}

void ioDownJumppingBuff::CheckDownJumpping()
{
	if( !m_pOwner ) return;
	if( !m_pOwner->IsOwnerChar() ) return;
	if( m_pOwner->IsPrisonerMode() ) return;
	if( m_pOwner->IsCatchMode() ) return;
	if( m_pOwner->HasBuff(BT_ENABLE_WOUND_STUN_STATUE) )	return;

	if( m_pOwner->HasDisableSkillFlameBuff() ) return;
	if( !m_pOwner->IsBlowWoundedDownState( m_dwEnableTime ) ) return;

	if( m_pOwner->IsJumpKeyDown() )
	{
		m_pOwner->SetChangeJumppingState( m_fJumppingPowerRate, false, 0 );

		/*if( m_pOwnerItem && m_bInitialGauge )
		{
			m_pOwnerItem->SetCurSkillGauge( 0.0f );
		}*/
		if( !m_ItemName.IsEmpty() && m_bInitialGauge )
		{
			ioItem *pOwnerItem = m_pOwner->GetEquipedItem( m_ItemName );
			if( pOwnerItem )
				pOwnerItem->SetCurSkillGauge( 0.0f );
			//m_pOwnerItem->SetCurSkillGauge( 0.0f );
		}

		if( isPassiveSkillGauageReduce() )
		{
			for( int i = 0; i < 4; ++i )
			{
				ioSkill *pSkill = m_pOwner->GetEquipedSkill( i );
				ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
				if( pPassiveSkill )
				{
					pPassiveSkill->CheckUseProcess( m_pOwner );
				}
			}
		}		
	}
}

void ioDownJumppingBuff::EndBuff()
{
	ioBuff::EndBuff();
}
