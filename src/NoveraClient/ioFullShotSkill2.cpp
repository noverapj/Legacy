

#include "stdafx.h"

#include "ioFullShotSkill2.h"


#include "ioBaseChar.h"



#include "WeaponDefine.h"
#include "ItemDefine.h"

ioFullShotSkill2::ioFullShotSkill2()
{
	m_vSkillDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_bSetHandMesh = false;

	m_iWeaponWay = 0;
	m_dwStateChangeTime = 0;
	m_dwStartSkillProtectedTime = 0;
}

ioFullShotSkill2::ioFullShotSkill2( const ioFullShotSkill2 &rhs )
: ioAttackSkill( rhs ),
 m_iMaxWeaponWay( rhs.m_iMaxWeaponWay ),
 m_BulletInfoList( rhs.m_BulletInfoList ),
 m_bUseDependencyWeapon( rhs.m_bUseDependencyWeapon ),
 m_LoopStateAniName( rhs.m_LoopStateAniName ),
 m_fLoopStateAniRate( rhs.m_fLoopStateAniRate ),
 m_dwLoopStateDuration( rhs.m_dwLoopStateDuration ),
 m_ActionEndAniName( rhs.m_ActionEndAniName ),
 m_fActionEndAniRate( rhs.m_fActionEndAniRate ),
 m_ActionCancelAniName( rhs.m_ActionCancelAniName ),
 m_fActionCancelAniRate( rhs.m_fActionCancelAniRate ),
 m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime )
{
	m_vSkillDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_bSetHandMesh = false;

	m_iWeaponWay = 0;
	m_dwStateChangeTime = 0;
	m_dwStartSkillProtectedTime = 0;
}

ioFullShotSkill2::~ioFullShotSkill2()
{
}

ioSkill* ioFullShotSkill2::Clone()
{
	return new ioFullShotSkill2( *this );
}

void ioFullShotSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	m_iMaxWeaponWay = rkLoader.LoadInt_e( "max_bullet_cnt", 1 );
	m_iMaxWeaponWay = max( 1, m_iMaxWeaponWay );

	m_BulletInfoList.clear();
	for( int i=0; i < m_iMaxWeaponWay; ++i )
	{
		BulletInfo kInfo;
		
		wsprintf_e( szKey, "bullet_angle%d", i+1 );
		kInfo.m_fYawAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "bullet_range_offset%d", i+1 );
		kInfo.m_fRangeOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "bullet_height_offset%d", i+1 );
		kInfo.m_fHeightOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		m_BulletInfoList.push_back( kInfo );
	}

	//
	m_dwSkillProtectedTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_bUseDependencyWeapon = rkLoader.LoadBool_e( "use_dependency_weapon", false );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopStateAniName = szBuf;

	m_fLoopStateAniRate = rkLoader.LoadFloat_e( "loop_ani_rate", FLOAT1 );

	m_dwLoopStateDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );

	rkLoader.LoadString_e( "action_end_animation", "", szBuf, MAX_PATH );
	m_ActionEndAniName = szBuf;

	m_fActionEndAniRate = rkLoader.LoadFloat_e( "action_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "action_cancel_animation", "", szBuf, MAX_PATH );
	m_ActionCancelAniName = szBuf;

	m_fActionCancelAniRate = rkLoader.LoadFloat_e( "action_cancel_ani_rate", FLOAT1 );
}

bool ioFullShotSkill2::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioFullShotSkill2::OnSkillStart( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_bChangeDirRot = false;
	m_bReduceGauge = false;
	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	m_dwStateChangeTime = 0;

	m_SkillSoundID = 0;

	m_bChangeGrapplingTargetPos = true;

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_dwStartSkillProtectedTime = dwCurTime;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = dwCurTime + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = dwCurTime + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = dwCurTime;

	m_iWeaponWay = 0;
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;

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

void ioFullShotSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

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
		if( m_dwMotionEndTime < dwCurTime )
		{
			SetSkillLoopState( pChar );
		}
		else
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
		}
		break;
	case SS_LOOP:
		if( m_dwStateChangeTime+m_dwLoopStateDuration < dwCurTime )
		{
			SetSkillActionEndState( pChar );
		}
		else
		{
			if( !CheckDependencyWeapon( pChar ) )
			{
				SetSkillCancelState( pChar );
			}
		}
		break;
	case SS_ACTION_END:
		if( m_dwMotionEndTime < dwCurTime )
		{
			SetSkillEndState( pChar );
		}
		break;
	case SS_CANCEL:
		if( m_dwMotionEndTime < dwCurTime )
		{
			SetSkillEndState( pChar );
		}
		break;
	case SS_END:
		{
		}
		break;
	}
}

void ioFullShotSkill2::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );
	SetSkillActionState( pChar );
}

void ioFullShotSkill2::SetSkillActionState( ioBaseChar *pChar )
{
	m_SkillState = SS_ACTION;
	m_dwStateChangeTime = FRAMEGETTIME();

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioFullShotSkill2::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_LOOP;
	m_dwStateChangeTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_LoopStateAniName );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fLoopStateAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioFullShotSkill2::SetSkillActionEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_ACTION_END;
	m_dwStateChangeTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_ActionEndAniName );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fActionEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	g_WeaponMgr.CheckSkillDependencyWeaponAction( pChar->GetCharName(), GetName() );
}

void ioFullShotSkill2::SetSkillCancelState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_CANCEL;
	m_dwStateChangeTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_ActionCancelAniName );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fActionCancelAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioFullShotSkill2::SetSkillEndState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_END;
	m_dwStateChangeTime = dwCurTime;
}

void ioFullShotSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioFireWeaponItem *pFire = ToFireWeaponItem( GetOwnerItem() );
	ioLaserItem *pLaser = ToLaserItem( GetOwnerItem() );
	ioBowItem *pBow = ToBowItem( GetOwnerItem() );
	ioPowerChargeItem *pPower = ToPowerChargeItem( GetOwnerItem() );
	ioCountChargeItem *pCount = ToCountChargeItem( GetOwnerItem() );
	ioRangeChargeItem *pRange = ToRangeChargeItem( GetOwnerItem() );
	ioRotateChargeItem *pRotate = ToRotateChargeItem( GetOwnerItem() );

	if( pFire )
	{
		D3DXVECTOR3 vZero( 0.0f, 0.0f, 0.0f );

		if( pFire->GetAimedSkillUse() && pFire->GetAimedDirSet() )
		{
			if( m_vSkillDir == vZero )
			{
				m_vSkillDir = pFire->ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			}
		}

		if( m_iMaxWeaponWay <= m_iWeaponWay )
			return;

		float fCurAngle = 0.0f;
		fCurAngle = m_BulletInfoList[m_iWeaponWay].m_fYawAngle;
			
		fCurAngle = ioMath::ArrangeHead( fCurAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		if( m_vSkillDir == vZero )
		{
			D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vMoveDir = qtAngle * vMoveDir;
		}
		else
		{
			if( m_vSkillDir.y != 0 )
			{
				D3DXVECTOR3 vYAxis( 0.0f, FLOAT1, 0.0f );
				D3DXVECTOR3 vRotAxis1, vRotAxis2;

				// vRotAxis1은 스킬Dir과 Y축의 법선벡터
				D3DXVec3Cross( &vRotAxis1, &vYAxis, &m_vSkillDir );

				// vRotAxis2는 스킬Dir과 vRotAxis1의 법선벡터
				D3DXVec3Cross( &vRotAxis2, &vRotAxis1, &m_vSkillDir );

				D3DXVec3Normalize( &vRotAxis2, &vRotAxis2 );
				D3DXQuaternionRotationAxis( &qtAngle, &vRotAxis2, DEGtoRAD(fCurAngle) );
			}

			vMoveDir = qtAngle * m_vSkillDir;
		}

		pWeapon->SetMoveDir( vMoveDir );

		m_iWeaponWay++;
	}
	else if( pBow )
	{
		if( pBow->IsChargeBulletType() )
			pBow->InitCoolTime();

		D3DXVECTOR3 vZero( 0.0f, 0.0f, 0.0f );

		if( pBow->GetAimedSkillUse() && pBow->GetAimedDirSet() )
		{
			if( m_vSkillDir == vZero )
			{
				m_vSkillDir = pBow->ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			}
		}

		if( m_iMaxWeaponWay <= m_iWeaponWay )
			return;

		float fCurAngle = 0.0f;
		fCurAngle = m_BulletInfoList[m_iWeaponWay].m_fYawAngle;

		fCurAngle = ioMath::ArrangeHead( fCurAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		if( m_vSkillDir == vZero )
		{
			D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vMoveDir = qtAngle * vMoveDir;
		}
		else
		{
			if( m_vSkillDir.y != 0.0f )
			{
				D3DXVECTOR3 vYAxis( 0.0f, FLOAT1, 0.0f );
				D3DXVECTOR3 vRotAxis1, vRotAxis2;

				// vRotAxis1은 스킬Dir과 Y축의 법선벡터
				D3DXVec3Cross( &vRotAxis1, &vYAxis, &m_vSkillDir );

				// vRotAxis2는 스킬Dir과 vRotAxis1의 법선벡터
				D3DXVec3Cross( &vRotAxis2, &vRotAxis1, &m_vSkillDir );

				D3DXVec3Normalize( &vRotAxis2, &vRotAxis2 );
				D3DXQuaternionRotationAxis( &qtAngle, &vRotAxis2, DEGtoRAD(fCurAngle) );
			}

			vMoveDir = qtAngle * m_vSkillDir;
		}

		pWeapon->SetMoveDir( vMoveDir );

		m_iWeaponWay++;
	}
	else if( pLaser )
	{
		pLaser->InitCoolTime();
		D3DXVECTOR3 vZero( 0.0f, 0.0f, 0.0f );

		if( pLaser->GetAimedSkillUse() && pLaser->GetAimedDirSet() )
		{
			if( m_vSkillDir == vZero )
			{
				m_vSkillDir = pLaser->ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			}
		}

		if( m_iMaxWeaponWay <= m_iWeaponWay )
			return;

		float fCurAngle = 0.0f;
		fCurAngle = m_BulletInfoList[m_iWeaponWay].m_fYawAngle;
			
		fCurAngle = ioMath::ArrangeHead( fCurAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		if( m_vSkillDir == vZero )
		{
			D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vMoveDir = qtAngle * vMoveDir;
		}
		else
		{
			if( m_vSkillDir.y != 0 )
			{
				D3DXVECTOR3 vYAxis( 0.0f, FLOAT1, 0.0f );
				D3DXVECTOR3 vRotAxis1, vRotAxis2;

				// vRotAxis1은 스킬Dir과 Y축의 법선벡터
				D3DXVec3Cross( &vRotAxis1, &vYAxis, &m_vSkillDir );

				// vRotAxis2는 스킬Dir과 vRotAxis1의 법선벡터
				D3DXVec3Cross( &vRotAxis2, &vRotAxis1, &m_vSkillDir );

				D3DXVec3Normalize( &vRotAxis2, &vRotAxis2 );
				D3DXQuaternionRotationAxis( &qtAngle, &vRotAxis2, DEGtoRAD(fCurAngle) );
			}

			vMoveDir = qtAngle * m_vSkillDir;
		}

		pWeapon->SetMoveDir( vMoveDir );

		m_iWeaponWay++;
	}
	else if( pPower || pCount || pRange || pRotate )
	{
		if( m_iMaxWeaponWay <= m_iWeaponWay )
			return;

		float fCurAngle = 0.0f;
		float fCurRange = 0.0f;
		float fCurHeight = 0.0f;

		fCurAngle = m_BulletInfoList[m_iWeaponWay].m_fYawAngle;
		fCurRange = m_BulletInfoList[m_iWeaponWay].m_fRangeOffSet;
		fCurHeight = m_BulletInfoList[m_iWeaponWay].m_fHeightOffSet;

		fCurAngle = ioMath::ArrangeHead( fCurAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
		vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vMoveDir = qtAngle * vMoveDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		D3DXVECTOR3 vPos = pWeapon->GetOwner()->GetMidPositionByRate();
		if( fCurRange > 0.0f )
			vPos += vMoveDir*fCurRange;

		if( fCurHeight > 0.0f )
			vPos.y += fCurHeight;

		pWeapon->SetStartPosition( vPos, pStage );
		pWeapon->SetMoveDir( vMoveDir );

		m_iWeaponWay++;

		// Charge...
		float fCurDamageRate = GetOwnerItem()->GetChargeDamageRateForSkill();
		float fCurForceRate = GetOwnerItem()->GetChargeForceRateForSkill();
		float fCurBlowRate = GetOwnerItem()->GetChargeBlowRateForSkill();
		float fCurRangeRate = GetOwnerItem()->GetChargeRangeRateForSkill();
		float fCurSpeedRate = GetOwnerItem()->GetChargeSpeedRateForSkill();
		float fCurFloatRate = GetOwnerItem()->GetChargeFloatRateForSkill();
		float fCurScaleRate = GetOwnerItem()->GetChargeScaleRateForSkill();
		float fCurExplosionRate = GetOwnerItem()->GetChargeExplosionRateForSkill();
		float fCurWoundedTimeRate = GetOwnerItem()->GetChargeWoundedTimeRateForSkill();

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );
		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );
		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );
		pWeapon->SetOwnerChargeScaleRate( fCurScaleRate );
		pWeapon->SetOwnerChargeExplosionRate( fCurExplosionRate );
		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );
	}
	else
	{
		if( m_iMaxWeaponWay <= m_iWeaponWay )
			return;

		float fCurAngle = 0.0f;
		float fCurRange = 0.0f;
		float fCurHeight = 0.0f;

		fCurAngle = m_BulletInfoList[m_iWeaponWay].m_fYawAngle;
		fCurRange = m_BulletInfoList[m_iWeaponWay].m_fRangeOffSet;
		fCurHeight = m_BulletInfoList[m_iWeaponWay].m_fHeightOffSet;
			
		fCurAngle = ioMath::ArrangeHead( fCurAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
		vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vMoveDir = qtAngle * vMoveDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		D3DXVECTOR3 vPos = pWeapon->GetOwner()->GetMidPositionByRate();
		if( fCurRange > 0.0f )
			vPos += vMoveDir*fCurRange;
		
		if( fCurHeight > 0.0f )
			vPos.y += fCurHeight;

		pWeapon->SetStartPosition( vPos, pStage );
		pWeapon->SetMoveDir( vMoveDir );

		m_iWeaponWay++;
	}
}

void ioFullShotSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	ioFireWeaponItem *pFire = ToFireWeaponItem( GetOwnerItem() );
	ioLaserItem *pLaser = ToLaserItem( GetOwnerItem() );
	ioBowItem *pBow = ToBowItem( GetOwnerItem() );
	if( pFire )
	{
		pFire->SetAimedSkillUse( false );
		pFire->SetAimedDirSet( false );
	}
	else if( pLaser )
	{
		pLaser->SetNoneState( pChar );
	}
	else if( pBow )
	{
		pBow->SetAimedSkillUse( false );
		pBow->SetAimedDirSet( false );
	}
	m_vSkillDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

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

	//
	g_WeaponMgr.CheckSkillDependencyWeaponDead( pChar->GetCharName(), GetName() );
}

bool ioFullShotSkill2::IsUseAutoTargetValue()
{
	if( !m_bUseAutoTargetValue )
		return false;

	bool bResult = true;

	ioFireWeaponItem *pFire = ToFireWeaponItem( GetOwnerItem() );
	ioLaserItem *pLaser = ToLaserItem( GetOwnerItem() );
	ioBowItem *pBow = ToBowItem( GetOwnerItem() );
	if( pFire )
	{
		if( pFire->IsAimedState() && pFire->GetAimedDirSet() )
		{
			bResult = false;
		}
	}
	else if( pLaser )
	{
		if( pLaser->IsAimedState() && pLaser->GetAimedDirSet() )
			bResult = false;
	}
	else if( pBow )
	{
		if( pBow->IsAimedState() && pBow->GetAimedDirSet() )
			bResult = false;
	}

	return bResult;
}

void ioFullShotSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
}

bool ioFullShotSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioFullShotSkill2::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

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
		if( m_dwStartSkillProtectedTime+m_dwSkillProtectedTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioFullShotSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_ACTION:
		return false;
	case SS_LOOP:
		return false;
	case SS_ACTION_END:
		return false;
	case SS_END:
		return true;
	}

	return false;
}

bool ioFullShotSkill2::CheckDependencyWeapon( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( !m_bUseDependencyWeapon )
		return true;

	return g_WeaponMgr.HasSkillDependencyWeapon( pChar->GetCharName(), GetName() );
}

