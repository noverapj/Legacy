

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioDefaultCounter.h"

ioDefaultCounter::ioDefaultCounter()
{
}

ioDefaultCounter::ioDefaultCounter( const ioDefaultCounter &rhs )
: ioCounterAttack( rhs ),
  m_bLastJump( rhs.m_bLastJump ),
  m_fLastJumpPower( rhs.m_fLastJumpPower )
{
}

ioDefaultCounter::~ioDefaultCounter()
{
}

void ioDefaultCounter::LoadProperty( ioINILoader &rkLoader )
{
	ioCounterAttack::LoadProperty( rkLoader );

	m_bLastJump = rkLoader.LoadBool( "counter_attack_last_jump", false );
	m_fLastJumpPower = rkLoader.LoadFloat( "counter_attack_last_jump_power", 0.f );
}

ioCounterAttack* ioDefaultCounter::Clone()
{
	return new ioDefaultCounter( *this );
}

CounterAttackType ioDefaultCounter::GetType() const
{
	return CAT_DEFAULT;
}

void ioDefaultCounter::ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	ioCounterAttack::ApplyCAT( pOwner, rkPacket );
}

bool ioDefaultCounter::StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState )
{
	if ( iCounterAttackKeyState == CAKS_ATTACK_KEY )
	{
		if( !ioCounterAttack::StartCAT( pOwner, iCounterAttackKeyState ) )
			return false;

		return true;
	}
	else
		return false;
}

void ioDefaultCounter::ProcessCAT( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_bLastJump )
	{
		DWORD dwCurTime = FRAMEGETTIME();

		if( pOwner->GetAnimationEndTime() + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				pOwner->ClearAttackFireTimeAndSkill();
				pOwner->ClearReservedSliding();

				pOwner->SetSKillEndJumpState( m_fLastJumpPower, false, false, true, true );
			}
		}
		else
			pOwner->ProcessDefaultCAT();
	}
	else
		pOwner->ProcessDefaultCAT();
}

