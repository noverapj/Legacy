
#include "stdafx.h"

#include "ioCallByStateBuff.h"

#include "ioBaseChar.h"

ioCallByStateBuff::ioCallByStateBuff()
{
}

ioCallByStateBuff::ioCallByStateBuff( const ioCallByStateBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
m_bStartOnDelay( rhs.m_bStartOnDelay ),
m_bStartOnRun( rhs.m_bStartOnRun ),
m_bStartOnDash( rhs.m_bStartOnDash ),
m_bStartOnJump( rhs.m_bStartOnJump ),
m_bStartOnAttack( rhs.m_bStartOnAttack ),
m_bStartOnDefense( rhs.m_bStartOnDefense ),
m_bStartOnWound( rhs.m_bStartOnWound ),
m_bStartOnGetUp( rhs.m_bStartOnGetUp )
{
}

ioCallByStateBuff::~ioCallByStateBuff()
{
}

void ioCallByStateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );

	m_bStartOnDelay = rkLoader.LoadBool_e( "start_call_on_delay", false );
	m_bStartOnRun = rkLoader.LoadBool_e( "start_call_on_run", false );
	m_bStartOnDash = rkLoader.LoadBool_e( "start_call_on_dash", false );
	m_bStartOnJump = rkLoader.LoadBool_e( "start_call_on_jump", false );
	m_bStartOnAttack = rkLoader.LoadBool_e( "start_call_on_attack", false );
	m_bStartOnDefense = rkLoader.LoadBool_e( "start_call_on_defense", false );
	m_bStartOnWound = rkLoader.LoadBool_e( "start_call_on_wound", false );
	m_bStartOnGetUp = rkLoader.LoadBool_e( "start_call_on_getup", false );
}

ioBuff* ioCallByStateBuff::Clone()
{
	return new ioCallByStateBuff( *this );
}

void ioCallByStateBuff::StartBuff( ioBaseChar *pOwner )
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		if( m_iBuffType != BT_CAMERA_BUFF )
			LOG.PrintTimeAndLog( 0, "BuffCheck - %s, StartTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( !pOwner )
		return;

	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwBuffStartTime = dwCurTime;
	if( m_iOperationType == OT_DELAY )
		m_dwBuffStartTime += m_dwDelayDuration;
	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();

	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioCallByStateBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
		m_dwBuffStartTime = m_dwBuffEndTime - GetBuffDuration();
	}

	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioCallByStateBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			if( GetOwner() && CheckRestoreStateCheck() )
				GetOwner()->SetState( CS_DELAY, m_bSendDelayNetWork );

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallByState();
		break;
	case OT_DELAY:
		ProcessDelay();
		break;
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallByState();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				if( CheckRestoreStateCheck() )
				{
					m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				}

				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioCallByStateBuff::SetAction()
{
	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != m_pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeColor( true );

	CheckChangeSkill( true );

	m_bBuffStart = true;

	CheckBuffDesc(); // buff Time 값 계산 이후에 호출할 것 버프에 따라서 Time 값이 필요하다.

	m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}
}

bool ioCallByStateBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioCallByStateBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

void ioCallByStateBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	ioBuff::EndBuff();
}

void ioCallByStateBuff::CheckCallByState()
{
	if( !m_pOwner )	return;
	CharState eState = m_pOwner->GetState();

	bool bSuccessCall = false;
	if( m_bStartOnDelay &&
		( eState == CS_DELAY || eState == CS_SPECIAL_MOTION ) )
		bSuccessCall = true;

	if( m_bStartOnRun &&
		( eState == CS_RUN ) )
		bSuccessCall = true;

	if( m_bStartOnDash &&
		( eState == CS_DASH || eState == CS_EXTEND_DASH ) )
		bSuccessCall = true;

	if( m_bStartOnJump &&
		( eState == CS_JUMP ) )
		bSuccessCall = true;

	if( m_bStartOnAttack &&
		( eState == CS_ATTACK || eState == CS_DASH_ATTACK || eState == CS_NEW_JUMP_ATTACK || eState == CS_COUNTER_ATTACK ) )
		bSuccessCall = true;

	if( m_bStartOnDefense &&
		( eState == CS_DEFENSE || eState == CS_EXTEND_DEFENSE ) )
		bSuccessCall = true;

	if( m_bStartOnWound )
	{
		if( eState == CS_WOUNDED )
			bSuccessCall = true;

		if(	( eState == CS_BLOW_WOUNDED || eState == CS_BOUND_BLOW_WOUNDED ) && 
			m_pOwner->GetBlowWoundedState() != BW_BLOW_ENDED && m_pOwner->GetBlowWoundedState() != BW_GETUP )
		{
			bSuccessCall = true;		
		}
	}

	if( m_bStartOnGetUp )
	{
		if( ( eState == CS_BLOW_WOUNDED || eState == CS_BOUND_BLOW_WOUNDED ) &&
			m_pOwner->GetBlowWoundedState() == BW_GETUP )
		{
			bSuccessCall = true;
		}
	}

	if( bSuccessCall )
	{
		CheckCallingProcess();
		SetReserveEndBuff();
	}
}

void ioCallByStateBuff::ProcessDelay()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckEnableDelayState() )
		return;

	if( GetBuffDuration() == 0 )
	{
		if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
		{
			SetAction();
		}
		return;
	}

	if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
	{
		SetAction();
	}

	if( m_dwBuffEndTime <= dwCurTime )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_bBuffStart )
	{
		CheckCallByState();
	}
}