

#include "stdafx.h"

#include "ioWhipSkill.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioWhipSkill::ioWhipSkill()
{
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwSkillEndMotionEndTime = 0;

	m_bSetHandMesh = false;
}

ioWhipSkill::ioWhipSkill( const ioWhipSkill &rhs )
: ioAttackSkill( rhs ),
 m_SkillEndAnimation( rhs.m_SkillEndAnimation ),
 m_fSkillEndAniRate( rhs.m_fSkillEndAniRate ),
 m_StartWhipEffect( rhs.m_StartWhipEffect ),
 m_EndWhipEffect( rhs.m_EndWhipEffect )
{
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwSkillEndMotionEndTime = 0;

	m_bSetHandMesh = false;
}

ioWhipSkill::~ioWhipSkill()
{
}

void ioWhipSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "whip_end_animation", "", szBuf, MAX_PATH );
	m_SkillEndAnimation = szBuf;
	
	m_fSkillEndAniRate = rkLoader.LoadFloat_e( "whip_end_ani_rate", FLOAT1 );
	
	rkLoader.LoadString_e( "whip_start_effect", "", szBuf, MAX_PATH );
	m_StartWhipEffect = szBuf;
	rkLoader.LoadString_e( "whip_end_effect", "", szBuf, MAX_PATH );
	m_EndWhipEffect = szBuf;
}

ioSkill* ioWhipSkill::Clone()
{
	return new ioWhipSkill( *this );
}

bool ioWhipSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_SkillState != SS_END )
		return false;

	if( m_dwMotionEndTime > FRAMEGETTIME() )
		return false;

	return true;
}

bool ioWhipSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_SkillState == SS_END )
			return false;

		if( m_SkillState == SS_ACTION )
		{
			if( m_dwFireMotionEndTime > FRAMEGETTIME() )
				return true;
		}
	}

	return false;
}

bool ioWhipSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwSkillEndMotionEndTime = 0;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	return ioAttackSkill::OnSkillStart( pChar );
}

void ioWhipSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelay = GetPreDelayTime();
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;

			AttackSkillFire( pChar );
			EndPreDelayEffect( pChar );
		}
		break;
	case SS_ACTION:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );

			if( m_dwMotionEndTime < dwCurTime )
			{
				SetSkillEndState( pChar );
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioWhipSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_ACTION;

	if( !m_StartWhipEffect.IsEmpty() )
	{
		pChar->AttachEffect( m_StartWhipEffect );
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioWhipSkill::SetSkillEndState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_END ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndAnimation );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSkillEndAniRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fSkillEndAniRate;

	m_SkillState = SS_END;

	if( !m_StartWhipEffect.IsEmpty() )
	{
		pChar->EndEffect( m_StartWhipEffect, false );
	}

	if( !m_EndWhipEffect.IsEmpty() )
	{
		pChar->AttachEffect( m_EndWhipEffect );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWhipSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	if( !m_StartWhipEffect.IsEmpty() )
		pChar->EndEffect( m_StartWhipEffect, false );

	if( !m_EndWhipEffect.IsEmpty() )
		pChar->EndEffect( m_EndWhipEffect, false );
}

void ioWhipSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	SetSkillEndState( pChar );
}

void ioWhipSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}
