

#include "stdafx.h"

#include "ioFullShotSkill.h"


#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioFullShotSkill::ioFullShotSkill()
{
	m_vSkillDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_iWeaponWay = 0;

	m_bSetHandMesh = false;

	m_dwSeed = 0;
}

ioFullShotSkill::ioFullShotSkill( const ioFullShotSkill &rhs )
: ioAttackSkill( rhs ),
 m_iMaxWeaponWay( rhs.m_iMaxWeaponWay ),
 m_BulletInfoList( rhs.m_BulletInfoList ),
 m_RightSkillMotion( rhs.m_RightSkillMotion ),
 m_LeftSkillMotion( rhs.m_LeftSkillMotion ),
 m_BackSkillMotion( rhs.m_BackSkillMotion ),
 m_bUseSpeedRandom( rhs.m_bUseSpeedRandom ),
 m_fMinSpeed( rhs.m_fMinSpeed ),
 m_fMaxSpeed( rhs.m_fMaxSpeed ),
 m_iSecondAttributeIndex( rhs.m_iSecondAttributeIndex )
{
	m_vSkillDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_bSetHandMesh = false;

	m_dwSeed = 0;
}

ioFullShotSkill::~ioFullShotSkill()
{
}

ioSkill* ioFullShotSkill::Clone()
{
	return new ioFullShotSkill( *this );
}

void ioFullShotSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_bUseSpeedRandom = rkLoader.LoadBool_e( "use_speed_random", false );
	m_fMinSpeed = rkLoader.LoadFloat_e( "min_speed", 0.0f );
	m_fMaxSpeed = rkLoader.LoadFloat_e( "max_speed", 0.0f );
	
	m_iMaxWeaponWay = rkLoader.LoadInt_e( "max_bullet_cnt", 1 );
	m_iMaxWeaponWay = max( 1, m_iMaxWeaponWay );

	char szKey[MAX_PATH];
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

	rkLoader.LoadString_e( "right_skill_motion", "", szKey, MAX_PATH );
	m_RightSkillMotion = szKey;


	rkLoader.LoadString_e( "left_skill_motion", "", szKey, MAX_PATH );
	m_LeftSkillMotion = szKey;

	rkLoader.LoadString_e( "back_skill_motion", "", szKey, MAX_PATH );
	m_BackSkillMotion = szKey;

	m_iSecondAttributeIndex = rkLoader.LoadInt_e( "throw_second_attribute", 0 );
}

bool ioFullShotSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
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

bool ioFullShotSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_bChangeDirRot = false;
	m_bReduceGauge = false;
	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_SkillSoundID = 0;

	m_bChangeGrapplingTargetPos = true;

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_iWeaponWay = 0;
	m_bSetHandMesh = false;

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
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelay;

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

void ioFullShotSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
		return;
	}

	if( pChar && pChar->IsNeedProcess() )
	{
		DWORD dwPreDelay = GetPreDelayTime();
		if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;

			AttackSkillFire( pChar );
			EndPreDelayEffect( pChar );
		}
	}
}

void ioFullShotSkill::AttackSkillFire( ioBaseChar *pChar )
{
	if( pChar && pChar->IsNeedProcess() )
	{
		CheckSkillAction( pChar );
	}

	if( pChar && pChar->IsNeedSendNetwork() )
	{
		m_dwSeed = timeGetTime();
		m_SpeedRand.SetRandomSeed( m_dwSeed );

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_dwSeed;
		kPacket << GetAttribute().m_AttackAnimation;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioAttackSkill::AttackSkillFire( pChar );
	SetSkillActionState( pChar );
}

void ioFullShotSkill::SetSkillActionState( ioBaseChar *pChar )
{
	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioFullShotSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	float fSpeed = 0.0f;
	int iGap = m_fMaxSpeed - m_fMinSpeed;
	if( m_bUseSpeedRandom && iGap > 0 )
	{
		int iRand = m_SpeedRand.Random( iGap );
		fSpeed = m_fMinSpeed + iRand;
	}

	ioBowItem2 *pBow2 = ToBowItem2( GetOwnerItem() );
	ioFireWeaponItem *pFire = ToFireWeaponItem( GetOwnerItem() );
	ioLaserItem *pLaser = ToLaserItem( GetOwnerItem() );
	ioBowItem *pBow = ToBowItem( GetOwnerItem() );
	ioPowerChargeItem *pPower = ToPowerChargeItem( GetOwnerItem() );
	ioCountChargeItem *pCount = ToCountChargeItem( GetOwnerItem() );
	ioRangeChargeItem *pRange = ToRangeChargeItem( GetOwnerItem() );
	ioRotateChargeItem *pRotate = ToRotateChargeItem( GetOwnerItem() );

	ioThrowReturnWeapon *pThrow = ToThrowReturnWeapon( pWeapon );

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

		if( fSpeed > 0.0f )
			pWeapon->SetMoveSpeed( fSpeed );

		m_iWeaponWay++;
	}
	else if( pBow2 )
	{
		D3DXVECTOR3 vZero( 0.0f, 0.0f, 0.0f );

		if( pBow2->GetAimedSkillUse() && pBow2->GetAimedDirSet() )
		{
			if( m_vSkillDir == vZero )
			{
				m_vSkillDir = pBow2->ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
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

		if( fSpeed > 0.0f )
			pWeapon->SetMoveSpeed( fSpeed );

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

		if( fSpeed > 0.0f )
			pWeapon->SetMoveSpeed( fSpeed );

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

		if( fSpeed > 0.0f )
			pWeapon->SetMoveSpeed( fSpeed );

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

		if( fSpeed > 0.0f )
			pWeapon->SetMoveSpeed( fSpeed );

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

		if( fSpeed > 0.0f )
			pWeapon->SetMoveSpeed( fSpeed );

		m_iWeaponWay++;
	}

	if( pThrow )
	{
		pThrow->SetSecondAttribute( m_iSecondAttributeIndex );

		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			pWeapon->SetStartPosition( pChar->GetCharHeadPos(1.0f), pStage );
		}
	}
}

float ioFullShotSkill::GetAttackTimeRate() const
{
	float fTimeRate = GetAttributeConst().m_fAttackAniRate;
	ioFireWeaponItem *pFire = ToFireWeaponItem( GetOwnerItem() );
	if( pFire )
	{
		fTimeRate *= pFire->GetFireTimeModifyRate();
	}

	fTimeRate *= m_fExtraAniRate;

	return fTimeRate;
}

float ioFullShotSkill::GetAttackEndTimeRate() const
{
	float fTimeRate = GetAttributeConst().m_fAttackEndAniRate;
	ioFireWeaponItem *pFire = ToFireWeaponItem( GetOwnerItem() );
	if( pFire )
	{
		fTimeRate *= pFire->GetFireTimeModifyRate();
	}

	return fTimeRate;
}

void ioFullShotSkill::OnSkillEnd( ioBaseChar *pChar )
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
}

bool ioFullShotSkill::IsUseAutoTargetValue()
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

void ioFullShotSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	rkPacket >> m_dwSeed;
	m_SpeedRand.SetRandomSeed( m_dwSeed );

	ioHashString szMotionName;
	rkPacket >> szMotionName;

	GetAttribute().m_AttackAnimation = szMotionName;

	if( m_dwPreDelayStartTime != 0 )
	{
		m_dwPreDelayStartTime = 0;
		AttackSkillFire( pChar );
		EndPreDelayEffect( pChar );
	}
}

bool ioFullShotSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioFullShotSkill::CheckSkillAction( ioBaseChar *pChar )
{
	if( pChar->IsSettedDirection() )
	{
		D3DXVECTOR3 vMoveDir = pChar->GetMoveDir();
		ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}

		vMoveDir.y = 0.0f;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		D3DXQUATERNION qtMoveRot;
		ioMath::CalcDirectionQuaternion( qtMoveRot, -vMoveDir );

		float fCurYaw, fTargetYaw;
		fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtMoveRot ) );
		fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

		fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pChar->GetTargetRot() ) );
		fCurYaw = ioMath::ArrangeHead( fCurYaw );

		float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
		fAngleGap += 360.0f;

		int iTest = (int)fAngleGap % 360;
		int iIndex = iTest / 30;

		ioHashString szSkillMotion = GetSkillMotionName();

		GetAttribute().m_AttackAnimation = szSkillMotion;
		if( iIndex > 0 && iIndex < 5 )
		{
			if( !m_RightSkillMotion.IsEmpty() )
			{
				GetAttribute().m_AttackAnimation = m_RightSkillMotion;
			}
		}
		else if( iIndex == 5 || iIndex == 6 )
		{
			if( !m_BackSkillMotion.IsEmpty() )
				GetAttribute().m_AttackAnimation = m_BackSkillMotion;
		}
		else if( iIndex > 6 && iIndex < 11 )
		{
			if( !m_LeftSkillMotion.IsEmpty() )
				GetAttribute().m_AttackAnimation = m_LeftSkillMotion;
		}
	}
}


