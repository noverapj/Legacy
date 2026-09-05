

#include "stdafx.h"

#include "ioGrowthUpInfo.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioUroborusSkill.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioUroborusSkill::ioUroborusSkill()
{
	m_bSetHandMesh = false;
	m_SkillState = USS_NONE;
}

ioUroborusSkill::ioUroborusSkill( const ioUroborusSkill &rhs )
: ioAttackSkill( rhs ),
 m_UroborusMotion( rhs.m_UroborusMotion ),
 m_fTargetAngle( rhs.m_fTargetAngle ),
 m_fTargetRange( rhs.m_fTargetRange ),
 m_fUroborusAngle( rhs.m_fUroborusAngle ),
 m_UroborusEndMotion( rhs.m_UroborusEndMotion ),
 m_fUroborusEndMotionRate( rhs.m_fUroborusEndMotionRate )
{
	m_bSetHandMesh = false;
	m_SkillState = USS_NONE;
}

ioUroborusSkill::~ioUroborusSkill()
{
}

void ioUroborusSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	//
	rkLoader.LoadString_e( "uroborus_motion", "", szBuf, MAX_PATH );
	m_UroborusMotion = szBuf;

	m_fTargetAngle = rkLoader.LoadFloat_e( "uroborus_target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "uroborus_target_range", 0.0f );

	m_fUroborusAngle = rkLoader.LoadFloat_e( "uroborus_fire_angle", 0.0f );

	rkLoader.LoadString_e( "uroborus_end_motion", "", szBuf, MAX_PATH );
	m_UroborusEndMotion = szBuf;
	m_fUroborusEndMotionRate = rkLoader.LoadFloat_e( "uroborus_end_motion_rate", FLOAT1 );
}

ioSkill* ioUroborusSkill::Clone()
{
	return new ioUroborusSkill( *this );
}

bool ioUroborusSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->SetSkillProtection();

	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = dwCurTime + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = dwCurTime + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	// ExtraAttackForce
	ioGrowthAttackForceUpInfo *pForceInfo = NULL;
	pForceInfo = ToAttackForceUpInfo(GetGrowthUpInfoByType(GT_SKILL_ATTACK_FORCE, pChar->GetCharName()));
	m_fExtraAttackMoveForce = 0.0f;
	m_fExtraAttackMoveFriction = 0.0f;

	if( pForceInfo )
	{
		m_fExtraAttackMoveForce = pForceInfo->GetForceValue(pStage);
		m_fExtraAttackMoveFriction = pForceInfo->GetFrictionValue(pStage);
	}

	SetExtraAttackForce();

	m_bSetHandMesh = false;
	m_SkillState = USS_NONE;
	m_fCurChargeRate = 0.0f;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		AttackSkillFire( pChar );
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += dwCurTime;
		else
			dwTrackingTime = dwCurTime + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioUroborusSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	m_bSetHandMesh = false;
	m_SkillState = USS_NONE;
	m_fCurChargeRate = 0.0f;
}

bool ioUroborusSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioUroborusSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = NULL;
	if( pWeapon )
		pChar = pWeapon->GetOwner();

	ioUroborusWeapon2 *pUroborus2 = ToUroborusWeapon2( pWeapon );
	if( pUroborus2 )
	{
		pUroborus2->SetMoveDir( m_vUroborusDir );
		pUroborus2->SetTargetMovePos( m_vTargetPos );
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}

	//
	if( pChar )
	{
		ioUroborusItem *pUroborusItem = ToUroborusItem( pChar->GetPriorityItem( SPT_ATTACK ) );
		if( pUroborusItem )
			pUroborusItem->InitUroborusForSkill();
	}
}

void ioUroborusSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_SkillState )
	{
	case USS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				AttackSkillFire( pChar );
			}
		}
		break;
	}
}

void ioUroborusSkill::SetSkillEndState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_UroborusEndMotion );
	if( iAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = USS_END;

	m_dwAttackEndTime = 0;
	float fTimeRate  = m_fUroborusEndMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
}

bool ioUroborusSkill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_SkillState )
	{
	case USS_FIRE:
		return true;
	case USS_UROBORUS:
		if( m_dwFireMotionEndTime > 0 && m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

void ioUroborusSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
}

bool ioUroborusSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case USS_NONE:
		return false;
	}

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

bool ioUroborusSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case USS_END:
		if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
			return false;

		if( m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

void ioUroborusSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	m_SkillState = USS_FIRE;

	CheckPreUroborusInfo( pChar );
}

void ioUroborusSkill::CheckPreUroborusInfo( ioBaseChar *pChar )
{
	if( !pChar ) return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vSightDir, &ioMath::UNIT_Y );

	// uroborus dir
	ioUroborusItem *pUroborusItem = ToUroborusItem( pChar->GetPriorityItem( SPT_ATTACK ) );
	if( pUroborusItem && pUroborusItem->IsUroborusForSkill() )
	{
		m_vUroborusDir = pUroborusItem->CheckUroborusDirForSkill( pChar );
	}
	else
	{
		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( m_fUroborusAngle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		m_vUroborusDir = qtNewRot * vSightDir;
		D3DXVec3Normalize( &m_vUroborusDir, &m_vUroborusDir );
	}

	// uroborus target pos
	D3DXQUATERNION qtTargetRot;
	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	D3DXQuaternionRotationAxis( &qtTargetRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vTargetDir = qtTargetRot * vSightDir;
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	m_vTargetPos = pChar->GetMidPositionByRate() + (m_fTargetRange * vTargetDir);
}

void ioUroborusSkill::SetUroborusMotionState( ioBaseChar *pChar, ioWeapon *pWeapon )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	ioUroborusWeapon2 *pUroborus2 = ToUroborusWeapon2( pWeapon );
	if( !pUroborus2 ) return;

	D3DXVECTOR3 vDiff = m_vTargetPos - pUroborus2->GetPosition();
	float fMaxRange = D3DXVec3Length( &vDiff );

	float fMoveTime = 0.0f;
	if( pUroborus2->GetTargetMoveSpeed() > 0.0f )
		fMoveTime = fMaxRange / pUroborus2->GetTargetMoveSpeed() * FLOAT1000;

	int iAniID = pGrp->GetAnimationIdx( m_UroborusMotion );
	float fAniTime = pGrp->GetAnimationFullTime( iAniID );
	float fTimeRate = FLOAT1;
	if( fAniTime > 0.0f )
		fTimeRate = fMoveTime / fAniTime;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_SkillState = USS_UROBORUS;

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += fMoveTime;

	m_dwFireMotionEndTime = m_dwMotionEndTime;
}







