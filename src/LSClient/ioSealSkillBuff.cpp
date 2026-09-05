

#include "stdafx.h"

#include "ioNpcChar.h"

#include "ioSealSkillBuff.h"


ioSealSkillBuff::ioSealSkillBuff()
{
}

ioSealSkillBuff::ioSealSkillBuff( const ioSealSkillBuff &rhs )
: ioBuff( rhs ),
  m_bDisableDownState( rhs.m_bDisableDownState ),
  m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
  m_dwAccumulateDuration( rhs.m_dwAccumulateDuration ),
  m_dwAccumulateDurationNpc( rhs.m_dwAccumulateDurationNpc ),
  m_dwAccumulateDurationBossNpc( rhs.m_dwAccumulateDurationBossNpc ),
  m_iMaxCount( rhs.m_iMaxCount ),
  m_vNoStopPassiveSkill( rhs.m_vNoStopPassiveSkill )
{
}

ioSealSkillBuff::~ioSealSkillBuff()
{
}

void ioSealSkillBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );

	m_iMaxCount = rkLoader.LoadInt_e( "max_accumulate_count", 0 );
	m_dwAccumulateDuration = (DWORD)rkLoader.LoadInt_e( "accumulate_duration", 0.0f );
	m_dwAccumulateDurationNpc = (DWORD)rkLoader.LoadInt_e( "accumulate_duration_npc", 0.0f );
	m_dwAccumulateDurationBossNpc = (DWORD)rkLoader.LoadInt_e( "accumulate_duration_boss_npc", 0.0f );

	m_vNoStopPassiveSkill.clear();
	int iSkillCnt = rkLoader.LoadInt_e( "no_stop_passive_skill_cnt", 0 );
	for( int i=0; i < iSkillCnt; ++i )
	{
		wsprintf_e( szKey, "no_stop_passive_skill%d", i+1 );

		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString szSkillName = szBuf;

		m_vNoStopPassiveSkill.push_back( szSkillName );
	}
}

ioBuff* ioSealSkillBuff::Clone()
{
	return new ioSealSkillBuff( *this );
}

void ioSealSkillBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}
	m_iCurCount = 0;

	if( pOwner && pOwner->GetEquipSlot() )
	{
		m_dwStopPassiveSkillID = pOwner->GetEquipSlot()->StopEquipedPassiveSkill( m_vNoStopPassiveSkill );
	}
}

bool ioSealSkillBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	m_iCurCount = 0;

	if( m_pOwner && m_pOwner->GetEquipSlot() )
	{
		m_dwStopPassiveSkillID = m_pOwner->GetEquipSlot()->StopEquipedPassiveSkill( m_vNoStopPassiveSkill );
	}
	return true;
}

void ioSealSkillBuff::ProcessBuff( float fTimePerSec )
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
				SetReserveEndBuffEx();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

bool ioSealSkillBuff::CheckOwnerStateCheck()
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
			SetReserveEndBuffEx();
			return true;
		}
	}

	return false;
}

bool ioSealSkillBuff::CheckRestoreStateCheck()
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

void ioSealSkillBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	m_iCurCount = 0;

	if( m_pOwner && m_pOwner->GetEquipSlot() )
	{
		m_pOwner->GetEquipSlot()->RestoreEquipedPassiveSkill( m_vNoStopPassiveSkill, m_dwStopPassiveSkillID );
	}

	ioBuff::EndBuff();
}

void ioSealSkillBuff::AccumulateBuff()
{
	if( m_iCurCount+1 >= m_iMaxCount )
		return;
	if( !IsLive() )
		return;

	m_iCurCount++;

	ioNpcChar *pNpc = ToNpcChar( m_pOwner );
	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
			m_dwBuffEndTime += m_dwAccumulateDuration;
	else
	{
		if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
			m_dwBuffEndTime += m_dwAccumulateDurationBossNpc;
		else
			m_dwBuffEndTime += m_dwAccumulateDurationNpc;
	}
}

void ioSealSkillBuff::SetReserveEndBuffEx()
{
	if( !m_pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_pOwner->GetState() != CS_DROP_ZONE_DOWN )
		SetReserveEndBuff();
}