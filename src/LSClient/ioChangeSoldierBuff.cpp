
#include "stdafx.h"

#include "ioChangeSoldierBuff.h"

#include "ioBaseChar.h"

ioChangeSoldierBuff::ioChangeSoldierBuff()
{
}

ioChangeSoldierBuff::ioChangeSoldierBuff( const ioChangeSoldierBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
m_EnableEffect( rhs.m_EnableEffect )
{
}

ioChangeSoldierBuff::~ioChangeSoldierBuff()
{
}

void ioChangeSoldierBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );

	rkLoader.LoadString_e( "enable_effect", "", szBuf, MAX_PATH );
	m_EnableEffect = szBuf;
}

ioBuff* ioChangeSoldierBuff::Clone()
{
	return new ioChangeSoldierBuff( *this );
}

void ioChangeSoldierBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}
	m_dwEnableEffectID = -1;
}

bool ioChangeSoldierBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}
	m_dwEnableEffectID = -1;
	return true;
}

void ioChangeSoldierBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
	{
		if( m_EnableEffect.IsEmpty() )
			continue;

		ioSkill *pSkill = m_pOwner->GetEquipedSkill(i);
		ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);

		if( !pPassiveSkill )
			continue;

		if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
			continue;

		if( pPassiveSkill->UsePassiveGauge() )
		{
			if( pPassiveSkill->GetCurPassiveGauge() >= pPassiveSkill->GetMaxPassiveGauge() )
			{
				if( m_dwEnableEffectID == -1 )
				{
					ioEffect *pEffect = m_pOwner->AttachEffect( m_EnableEffect );
					if( pEffect )
						m_dwEnableEffectID = pEffect->GetUniqueID();
				}
			}
			else
			{
				if( m_dwEnableEffectID != -1 )
				{
					m_pOwner->EndEffect( m_dwEnableEffectID, false );
					m_dwEnableEffectID = -1;
				}
			}
		}
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

bool ioChangeSoldierBuff::CheckOwnerStateCheck()
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

bool ioChangeSoldierBuff::CheckRestoreStateCheck()
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

void ioChangeSoldierBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	if( m_dwEnableEffectID != -1 )
	{
		m_pOwner->EndEffect( m_dwEnableEffectID, false );
		m_dwEnableEffectID = -1;
	}

	ioBuff::EndBuff();
}

bool ioChangeSoldierBuff::EnableChangeSoldier()
{	
	for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
	{
		ioSkill *pSkill = m_pOwner->GetEquipedSkill(i);
		ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);
		
		if( !pPassiveSkill )
			continue;

		if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
			continue;

		if( pPassiveSkill->UsePassiveGauge() )
		{
			if( pPassiveSkill->GetCurPassiveGauge() >= pPassiveSkill->GetMaxPassiveGauge() )
			{
				CharState eState = m_pOwner->GetState();
				if( eState == CS_USING_SKILL )
					return false;

				if( m_pOwner->GetUsedBuffFlyJump() != BJUT_NONE && m_pOwner->GetUsedBuffFlyJump() != BJUT_END )
					return false;

				if( eState == CS_DELAY || eState == CS_RUN ||
					eState == CS_FALL || eState == CS_ETC_STATE ||
					eState == CS_CHANGE_WAIT || eState == CS_ICE_STATE || eState == CS_FROZEN ||
					eState == CS_STOP_MOTION || eState == CS_CLOSEORDER ||
					eState == CS_PANIC_MOVE )
				{
					return true;				
				}

				if( eState == CS_JUMP && m_pOwner->GetJumpState() == JS_JUMPPING )
				{
					return true;
				}

				if( IsWoundedState() )
				{
					return true;				
				}

				if( !m_pOwner->IsJumppingState() && m_pOwner->IsFloatingState() )
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool ioChangeSoldierBuff::IsWoundedState()
{
	if( !m_pOwner )
		return false;

	if( m_pOwner->IsApplyDownState() || m_pOwner->IsApplyDownState(false) )
		return true;

	CharState eState = m_pOwner->GetState();
	if( eState == CS_WOUNDED ||
		eState == CS_GRAPPLING_WOUNDED || eState == CS_GRAPPLING_WOUNDED_SKILL ||
		eState == CS_GRAPPLING_WOUNDED_SWING || eState == CS_JUMP_GRAPPLING_WOUNDED ||
		eState == CS_GRAPPLING_WOUNDED_ITEM ||
		eState == CS_GRAPPLING_WOUNDED_BUFF )
		return true;

	return false;
}