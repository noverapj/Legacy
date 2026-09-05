

#include "stdafx.h"

#include "ioNpcChar.h"

#include "ioSpeedAccumlateBuff.h"

ioSpeedAccumlateBuff::ioSpeedAccumlateBuff()
{
}

ioSpeedAccumlateBuff::ioSpeedAccumlateBuff( const ioSpeedAccumlateBuff &rhs )
: ioBuff( rhs ),
  m_bDisableDownState( rhs.m_bDisableDownState ),
  m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
  m_dwAccumulateDuration( rhs.m_dwAccumulateDuration ),
  m_dwAccumulateDurationNpc( rhs.m_dwAccumulateDurationNpc ),
  m_dwAccumulateDurationBossNpc( rhs.m_dwAccumulateDurationBossNpc ),
  m_iMaxCount( rhs.m_iMaxCount ),
  m_fSpeedRate( rhs.m_fSpeedRate )
{
}

ioSpeedAccumlateBuff::~ioSpeedAccumlateBuff()
{
}

void ioSpeedAccumlateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );

	m_iMaxCount = rkLoader.LoadInt_e( "max_accumulate_count", 0 );
	m_dwAccumulateDuration = (DWORD)rkLoader.LoadInt_e( "accumulate_duration", 0.0f );
	m_dwAccumulateDurationNpc = (DWORD)rkLoader.LoadInt_e( "accumulate_duration_npc", 0.0f );
	m_dwAccumulateDurationBossNpc = (DWORD)rkLoader.LoadInt_e( "accumulate_duration_boss_npc", 0.0f );

	m_fSpeedRate = rkLoader.LoadFloat_e( "accumlate_speed_rate", 0.0f );
}

ioBuff* ioSpeedAccumlateBuff::Clone()
{
	return new ioSpeedAccumlateBuff( *this );
}

void ioSpeedAccumlateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	m_iCurCount = 0;
	m_fCurAccumSpeedRate = 0.0f;

	m_fCurAccumSpeedRate += m_fSpeedRate;
	m_fCurAccumSpeedRate = max( m_fMinAccumSpeedRate, min(m_fCurAccumSpeedRate, m_fMaxAccumSpeedRate) );
}

bool ioSpeedAccumlateBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	m_iCurCount = 0;
	m_fCurAccumSpeedRate = 0.0f;

	return true;
}

void ioSpeedAccumlateBuff::ProcessBuff( float fTimePerSec )
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

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

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

bool ioSpeedAccumlateBuff::CheckOwnerStateCheck()
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

bool ioSpeedAccumlateBuff::CheckRestoreStateCheck()
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

void ioSpeedAccumlateBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	m_iCurCount = 0;
	m_fCurAccumSpeedRate = 0.0f;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		if( pOwner->GetState() == CS_DELAY )
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		else if( pOwner->GetState() == CS_RUN )
			pOwner->SetState( CS_RUN );
		else if( pOwner->GetState() == CS_DASH )
			pOwner->SetState( CS_DASH );
		else if( pOwner->GetState() == CS_EXTEND_DASH )
			pOwner->CheckRefreshExtendDashState();
	}

	ioBuff::EndBuff();
}

void ioSpeedAccumlateBuff::AccumulateBuff()
{
	if( m_iCurCount+1 >= m_iMaxCount )
		return;
	if( !IsLive() )
		return;

	m_iCurCount++;

	m_fCurAccumSpeedRate += m_fSpeedRate;
	m_fCurAccumSpeedRate = max( m_fMinAccumSpeedRate, min(m_fCurAccumSpeedRate, m_fMaxAccumSpeedRate) );

	ioNpcChar *pNpc = ToNpcChar( m_pOwner );
	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
	{
		m_dwBuffEndTime += m_dwAccumulateDuration;
	}
	else
	{
		if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
		{
			m_dwBuffEndTime += m_dwAccumulateDurationBossNpc;
		}
		else
		{
			m_dwBuffEndTime += m_dwAccumulateDurationNpc;
		}
	}
}


