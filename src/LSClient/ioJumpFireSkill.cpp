

#include "stdafx.h"

#include "ioJumpFireSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

#include "ioBombMissileWeapon.h"

ioJumpFireSkill::ioJumpFireSkill()
{
}

ioJumpFireSkill::ioJumpFireSkill( const ioJumpFireSkill &rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_fSkillJumpPower( rhs.m_fSkillJumpPower ),
	m_iHeightOffset( rhs.m_iHeightOffset )
{	
}

ioJumpFireSkill::~ioJumpFireSkill()
{
}

void ioJumpFireSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_dwSkillProtectTime	= rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_fSkillJumpPower		= rkLoader.LoadFloat_e( "skill_jump_power", 1500.0f );
	m_iHeightOffset			= rkLoader.LoadInt_e( "height_offset", 0 );
}

ioSkill* ioJumpFireSkill::Clone()
{
	return new ioJumpFireSkill( *this );
}

bool ioJumpFireSkill::OnSkillStart( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillStart( pOwner );

	m_SkillState	= SS_PRE;
	m_bSetHandMesh	= false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( false );
	}

	if( GetSkillUseType() != SUT_JUMP )
	{
		pOwner->SetJumpPower( m_fSkillJumpPower );
	}

	return true;
}

void ioJumpFireSkill::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pOwner, pStage );
		}
		break;
	case SS_ATTACK:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				m_SkillState = SS_END;
			}
		}
		break;
	}
}

void ioJumpFireSkill::CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime  = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pOwner );
		SetSkillActionState( pOwner );
	}
}

void ioJumpFireSkill::AttackSkillFire( ioBaseChar *pOwner )
{
	ioAttackSkill::AttackSkillFire( pOwner );
}

void ioJumpFireSkill::SetSkillActionState( ioBaseChar *pOwner )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_ATTACK;
	AttackSkillFire( pOwner );
}

void ioJumpFireSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillEnd( pOwner );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}	
}

bool ioJumpFireSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioJumpFireSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBombMissileWeapon* pBombWeapon = ToBombMissileWeapon( pWeapon );
	if( pBombWeapon )
	{
		ioBaseChar* pOwner = pWeapon->GetOwner();
		if( pOwner )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			vPos.y += m_iHeightOffset;

			pBombWeapon->SetStartPosition( vPos, pStage );
			pBombWeapon->SetMoveDir( D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
		}
	}
}

bool ioJumpFireSkill::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_ATTACK:
	case SS_END:
		return true;
	}
	return false;
}

bool ioJumpFireSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioJumpFireSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( 0.0f < m_fSkillEndNextJumpAmt )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioJumpFireSkill::IsAttackEndState() const
{
	if( m_fSkillEndNextJumpAmt == 0.0f )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

SkillEndJumpType ioJumpFireSkill::GetSkillEndNextJumpMustEnable()
{
	return SEJT_JUMP;
}

bool ioJumpFireSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( 0 < m_dwSkillProtectTime && FRAMEGETTIME() < m_dwSkillStartTime + m_dwSkillProtectTime )
		return true;

	return false;
}