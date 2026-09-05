

#include "stdafx.h"

#include "ioWitchFlyBuff.h"

#include "ioBaseChar.h"

ioWitchFlyBuff::ioWitchFlyBuff()
{
}

ioWitchFlyBuff::ioWitchFlyBuff( const ioWitchFlyBuff &rhs )
: ioBuff( rhs ),
m_AttackWitchFly( rhs.m_AttackWitchFly )
{
}

ioWitchFlyBuff::~ioWitchFlyBuff()
{
}

void ioWitchFlyBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_AttackWitchFly.Init();

	rkLoader.LoadString_e( "witch_fly_effect", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_WitchFlyEffect = szBuf;

	rkLoader.LoadString_e( "witch_fly_wait_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_WaitAni = szBuf;
	m_AttackWitchFly.m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "witch_fly_wait_duration", 0 );

	wsprintf_e( szBuf, "witch_fly_attack" );
	LoadAttackAttribute( szBuf, m_AttackWitchFly.m_WitchAttackAttribute, rkLoader );

	m_AttackWitchFly.m_dwChargeTime = (DWORD)rkLoader.LoadInt_e( "witch_fly_charge_time", 0 );
	rkLoader.LoadString_e( "witch_fly_charge_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_ChargeAni = szBuf;

	rkLoader.LoadString_e( "witch_fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_StartAni = szBuf;
	m_AttackWitchFly.m_fStartAniRate = rkLoader.LoadFloat_e( "witch_fly_start_ani_rate", FLOAT1 );

	m_AttackWitchFly.m_fStartAngle = rkLoader.LoadFloat_e( "witch_fly_start_angle", 0.0f );
	m_AttackWitchFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "witch_fly_start_max_range", 0.0f );
	m_AttackWitchFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "witch_fly_start_duration", 0 );

	rkLoader.LoadString_e( "witch_fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_DelayAni = szBuf;
	m_AttackWitchFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "witch_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "witch_fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_MoveAni = szBuf;
	m_AttackWitchFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "witch_fly_move_ani_rate", FLOAT1 );

	m_AttackWitchFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "witch_fly_move_speed", FLOAT100 );
	m_AttackWitchFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "witch_fly_updown_speed", FLOAT100 );

	m_AttackWitchFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "witch_fly_rotate_speed", 0 );

	m_AttackWitchFly.m_fWitchFlyJumpAmt = rkLoader.LoadFloat_e( "witch_fly_jump_amt", 0.0f );
}

ioBuff* ioWitchFlyBuff::Clone()
{
	return new ioWitchFlyBuff( *this );
}

void ioWitchFlyBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( pOwner )
		pOwner->SetWitchFlyStateSkill( GetName(), m_AttackWitchFly );
}

bool ioWitchFlyBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( pOwner )
		pOwner->SetWitchFlyStateSkill( GetName(), m_AttackWitchFly );

	return true;
}

void ioWitchFlyBuff::ProcessBuff( float fTimePerSec )
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

	if( m_pOwner->GetState() != CS_WITCH_FLY )
	{
		m_pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}
}

void ioWitchFlyBuff::EndBuff()
{
	ioBuff::EndBuff();
}

