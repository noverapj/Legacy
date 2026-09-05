

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioMoveAttackSkill.h"

#include "WeaponDefine.h"

ioMoveAttackSkill::ioMoveAttackSkill()
{
	m_iWeaponWay = 0;
	m_bSetHandMesh = false;
	m_iEndWeapon = 0;
}

ioMoveAttackSkill::ioMoveAttackSkill( const ioMoveAttackSkill &rhs )
: ioAttackSkill( rhs ),
 m_dwLoopTime( rhs.m_dwLoopTime ),
 m_bUseFireEnd( rhs.m_bUseFireEnd ),
 m_bDirection( rhs.m_bDirection ),
 m_iMaxWeaponWay( rhs.m_iMaxWeaponWay ),
 m_BulletInfoList( rhs.m_BulletInfoList ),
 m_iEndWeapon( rhs.m_iEndWeapon )
{
	m_bSetHandMesh = false;
}

ioMoveAttackSkill::~ioMoveAttackSkill()
{
}

void ioMoveAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

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

	m_dwLoopTime = rkLoader.LoadInt_e( "run_duration", 0 );
	m_bUseFireEnd	 = rkLoader.LoadBool_e( "use_fire_end", false );
	m_bDirection = rkLoader.LoadBool_e( "direction", false );

	m_iEndWeapon = rkLoader.LoadInt_e( "end_weapon", 0 );
}

ioSkill* ioMoveAttackSkill::Clone()
{
	return new ioMoveAttackSkill( *this );
}

bool ioMoveAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	// 기본 무기 손에서 제거
	m_bSetHandMesh = false;
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_iWeaponWay = 0;

	return ioAttackSkill::OnSkillStart( pChar );
}

void ioMoveAttackSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->AttachEffect( m_SkillEffectName );
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										GetAttribute().m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										0 );


	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += m_dwLoopTime;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( !m_bUseFireEnd )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, GetAttribute().m_dwPreDelay );

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, 0 );
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;


	if( pChar )
	{
		pChar->SetSkillMoveEnable( true, m_bDirection );

		// 사용 아이템 처리
		if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
		{
			pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
			m_bSetHandMesh = true;
		}
	}
}

void ioMoveAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		return;
	}

	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
	{
		m_dwPreDelayStartTime = 0;

		AttackSkillFire( pChar );
		EndPreDelayEffect( pChar );
	}
}

void ioMoveAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	CallEndWeapon( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
	pChar->SetSkillMoveEnable( false, false );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	ioAttackSkill::OnSkillEnd( pChar );
}

void ioMoveAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !m_bUseFireEnd )
	{
		pWeapon->SetLiveTime( m_dwLoopTime );
	}

	ioBaseChar *pChar = pWeapon->GetOwner();
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone && pChar )
	{
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		pZone->SetCollisionDir( vDir );
		pZone->SetZoneValue( false, true );
	}

	ioRotationWeapon *pRotation = ToRotationWeapon( pWeapon );
	if( pRotation )
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

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

bool ioMoveAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMoveAttackSkill::CallEndWeapon( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_iEndWeapon;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = pChar->GetWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_SKILL_ATTACK;
	ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		pZone->SetCollisionDir( vDir );
		pZone->SetZoneValue( false, true );
	}
}