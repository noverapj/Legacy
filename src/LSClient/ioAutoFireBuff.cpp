
#include "stdafx.h"

#include "ioAutoFireBuff.h"

#include "ioBaseChar.h"

ioAutoFireBuff::ioAutoFireBuff()
{
}

ioAutoFireBuff::ioAutoFireBuff( const ioAutoFireBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
m_bFireOnFly( rhs.m_bFireOnFly )
{
}

ioAutoFireBuff::~ioAutoFireBuff()
{
}

void ioAutoFireBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_bFireOnFly = rkLoader.LoadBool_e( "auto_fire_on_fly_state", false );
}

ioBuff* ioAutoFireBuff::Clone()
{
	return new ioAutoFireBuff( *this );
}

void ioAutoFireBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	m_dwCheckFireTime = FRAMEGETTIME();
}

bool ioAutoFireBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	return true;
}

void ioAutoFireBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bFireOnFly )
	{
		if( CheckFlyState() )
		{
			if( m_dwCheckFireTime <= 0)
				m_dwCheckFireTime = dwCurTime;
		}
		else
			m_dwCheckFireTime = 0;
	}

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

		AutoCallAttribute();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			AutoCallAttribute();

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
		AutoCallAttribute();
		break;
	}
}

bool ioAutoFireBuff::CheckOwnerStateCheck()
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

bool ioAutoFireBuff::CheckRestoreStateCheck()
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

void ioAutoFireBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	ioBuff::EndBuff();
}

void ioAutoFireBuff::AutoCallAttribute()
{
	if( m_CallAttributeList.empty() ) return;
	if( !m_pOwner )	return;
	if( m_dwCheckFireTime <= 0 )	return;
	if( m_bFireOnFly && !CheckFlyState() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwTopTime = m_CallAttributeList[0].m_dwAttributeCallTime;
	dwTopTime += m_dwCheckFireTime;
	if( dwTopTime <= dwCurTime )
	{
		if( m_bCallExistAttacker )
		{
			ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
			//if( pCreator && pCreator->IsNeedProcess() )
			if( pCreator )
			{
				D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate();
				DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase() + 1;

				m_vWeaponIndexList.push_back( dwWeaponIndex );

				pCreator->DummyExplicit( m_CallAttributeList[0].m_WeaponInfo,
										vPos,
										dwWeaponIndex,
										false,
										m_pOwner->GetCharName(),
										m_Name,
										m_bUseCallAttWeaponCol );

				m_dwCheckFireTime = FRAMEGETTIME();
			}
		}
		else
		{
			if( !m_pOwner->IsProtectState() )
			{
				m_pOwner->BuffFireExplicit( m_CallAttributeList[0].m_WeaponInfo, m_Name );
				m_dwCheckFireTime = FRAMEGETTIME();
			}
			else if( IsIgnoreProtectBuff() )
			{
				m_pOwner->BuffFireExplicit( m_CallAttributeList[0].m_WeaponInfo, m_Name );
				m_dwCheckFireTime = FRAMEGETTIME();
			}
		}
	}
}

bool ioAutoFireBuff::CheckFlyState()
{
	if( !m_pOwner )	return false;
	if( !m_pOwner->IsFloating() ) return false;

	BuffJumpUsedType eFlyJump = (BuffJumpUsedType)m_pOwner->GetUsedBuffFlyJump();
	BuffJumpUsedType eMultiFlyJump = (BuffJumpUsedType)m_pOwner->GetUsedBuffMultiFlyJump();

	if( eMultiFlyJump == BJUT_CUR )
		return true;
	else if( eFlyJump != BJUT_NONE && eFlyJump != BJUT_END )
		return true;

	return false;
}