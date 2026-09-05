

#include "stdafx.h"

#include "ioFireSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "iolupinItem.h"

ioFireSkill::ioFireSkill()
{
	m_bSetHandMesh = false;

	m_TargetCharList.clear();
	m_dwTartgetAttackFireTime = 0;
	m_vTargetPos = ioMath::VEC3_ZERO;
}

ioFireSkill::ioFireSkill( const ioFireSkill &rhs )
: ioAttackSkill( rhs ),
m_TargetColType( rhs.m_TargetColType ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_fSkillRange( rhs.m_fSkillRange ),
m_fSkillAngle( rhs.m_fSkillAngle ),
m_fOffSet( rhs.m_fOffSet ),
m_fHeightOffSet( rhs.m_fHeightOffSet ),
m_fUpHeight( rhs.m_fUpHeight ),
m_fUnderHeight( rhs.m_fUnderHeight ),
m_fWidth( rhs.m_fWidth ),
m_iMaxFireCnt( rhs.m_iMaxFireCnt ),
m_FireType( rhs.m_FireType ),
m_bNoDropState( rhs.m_bNoDropState ),
m_vWeaponOffset( rhs.m_vWeaponOffset )
{
	m_bSetHandMesh = false;

	m_TargetCharList.clear();
	m_dwTartgetAttackFireTime = 0;
	m_vTargetPos = ioMath::VEC3_ZERO;
}

ioFireSkill::~ioFireSkill()
{
}

void ioFireSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_collision_type", TCT_SPHERE );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fSkillRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fSkillAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fOffSet = rkLoader.LoadFloat_e( "target_offset", 0.0f );
	m_fHeightOffSet = rkLoader.LoadFloat_e( "target_height_offset", 0.0f );

	m_fUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fUpHeight = max( FLOAT1, m_fUpHeight );
	m_fUnderHeight = rkLoader.LoadFloat_e( "target_under_height", 0.0f );
	m_fUnderHeight = max( FLOAT1, m_fUnderHeight );
	m_fWidth = rkLoader.LoadFloat_e( "target_width", 0.0f );

	m_iMaxFireCnt = rkLoader.LoadInt_e( "max_fire_cnt", 1 );
	m_iMaxFireCnt = max( 1, m_iMaxFireCnt );

	m_FireType = (FireType)rkLoader.LoadInt_e( "fire_type", FT_NORMAL );
	m_bNoDropState = rkLoader.LoadBool_e( "no_drop_state", false );

	m_vWeaponOffset.x = rkLoader.LoadFloat_e( "weapon_offset_x", 0.0f );
	m_vWeaponOffset.y = rkLoader.LoadFloat_e( "weapon_offset_y", 0.0f );
	m_vWeaponOffset.z = rkLoader.LoadFloat_e( "weapon_offset_z", 0.0f );
}

ioSkill* ioFireSkill::Clone()
{
	return new ioFireSkill( *this );
}

bool ioFireSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_TargetCharList.clear();
	m_dwTartgetAttackFireTime = 0;
	m_vTargetPos = ioMath::VEC3_ZERO;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	// 스킬 사용시 루팡 버프 해제
	ioItem *pItem = GetOwnerItem();
	iolupinItem *pLupinItem = ToLupinItem( pItem );
	if( pLupinItem )
		pLupinItem->ClearBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh ) 
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( pLupinItem )
	{
		// 버프 시작
		ioHashString stBuffName = pLupinItem->GetWeaponSkillBuffName();
		pChar->AddNewBuff( stBuffName, pChar->GetCharName(), "", NULL );
		pLupinItem->ClearTargetCharByWeaponSkill();
	}

	return true;
}

void ioFireSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FireType )
	{
	case FT_NORMAL:
		ioAttackSkill::OnProcessState( pChar, pStage );
		break;
	case FT_TARGET:
		{
			if( m_dwPreDelayStartTime == 0 )
			{
				CheckExtraAniJump( pChar );
				CheckAniRotate( pChar );

				if( m_dwTartgetAttackFireTime > 0 && m_dwTartgetAttackFireTime < dwCurTime )
				{
					m_dwTartgetAttackFireTime = 0;
					CheckTargetFire( pChar );
				}
				return;
			}

			DWORD dwPreDelay = GetPreDelayTime();

			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetTargetAttackAction( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	}
}

void ioFireSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
}

void ioFireSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioFireSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	if( GetOwnerItem() )
	{
		GetOwnerItem()->ClearCurChargeRateForSkill();
		ioItem* pItem = GetOwnerItem();
		iolupinItem* pLupinItem = ToLupinItem( pItem );
		if( pLupinItem )
		{
			ioBuff* pBuff = pChar->GetBuff( BT_STEAL_EQUIPMENT_BUFF );
			if( pBuff )
			{
				pBuff->SetReserveEndBuff();
			}
		}
	}

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}


	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	m_bSetHandMesh		= false;
	m_dwMotionEndTime	= 0;
}

bool ioFireSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioFireSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioBombMissileWeapon *pBombMissile = ToBombMissileWeapon( pWeapon );
	ioTraceWeaponType2 *pTrace2 = ToTraceWeaponType2( pWeapon );

	if( (pArrow || pMissile || pBombMissile || pTrace2) && m_vTargetPos != ioMath::VEC3_ZERO )
	{
		D3DXVECTOR3 vDir = m_vTargetPos - pWeapon->GetPosition();
		D3DXVec3Normalize( &vDir, &vDir );

		pWeapon->SetMoveDir( vDir );
	}

	ioSylphidMissileWeapon *pSylphid = ToSylphidMissileWeapon( pWeapon );
	if( pSylphid )
	{
		g_WeaponMgr.CheckSylphidMissileWeapon( pWeapon );
	}

	ioDestroyCallWeapon *pDestroyCallWeapon = ToDestroyCallWeapon( pWeapon );
	if ( pDestroyCallWeapon )
	{
		ioBaseChar *pOwner = pWeapon->GetOwner();
		if ( pOwner )
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + pOwner->GetWorldOrientation() * m_vWeaponOffset;
			D3DXVECTOR3 vMoveDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vMoveDir, &vMoveDir );

			pDestroyCallWeapon->SetPosition( vPos );
			pDestroyCallWeapon->SetMoveDir( vMoveDir );
		}
	}

	ioContactWeapon *pContact = ToContactWeapon( pWeapon );
	if ( pContact )
		pContact->SetSkillWeapon( true );
}

void ioFireSkill::FindTargetList( ioBaseChar *pOwner )
{
	m_TargetCharList.clear();

	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	BaseCharList vTargetList;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	if( m_fOffSet != 0.0f )
		vPos += vSightDir * m_fOffSet;

	// 충돌타입에 따라 리스트 가져오기
	switch( m_TargetColType )
	{
	case TCT_SPHERE:
		pOwner->GetTargetCharListBySphere( vPos, m_fSkillRange, m_fSkillAngle, vTargetList );
		break;
	case TCT_CYLINDER:
		pOwner->GetTargetCharListByCylinder( vPos, m_fSkillRange, m_fSkillAngle, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	case TCT_BOX:
		// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
		vPos += vSightDir * m_fSkillRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
		vPos -= vOffset;
		pOwner->GetTargetCharListByBox( vPos, m_fSkillRange, m_fWidth, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	}

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		vTargetList.clear();

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )
			continue;

		if( pTarget == pOwner )
			continue;


		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 	
			continue;

		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );

		BaseCharDistanceSq kTarget;
		kTarget.m_Name = pTarget->GetCharName();
		kTarget.m_fDistanceSq = fDistanceSq;
		kTarget.m_vPos = pTarget->GetMidPositionByRate();

		m_TargetCharList.push_back( kTarget );
	}

	if( !m_TargetCharList.empty() )
	{
		std::sort( m_TargetCharList.begin(), m_TargetCharList.end(), BaseCharDistanceSqSort() );
	}
}

void ioFireSkill::SetTargetAttackAction( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pOwner->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwTartgetAttackFireTime = m_dwFireMotionEndTime = dwCurTime;

	m_dwTartgetAttackFireTime += dwPreDelay;
	m_dwTartgetAttackFireTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	//
	SetExtraAniJump( pOwner, iAniID, fTimeRate );

	//
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
	//

	// Col Skip
	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( m_dwTartgetAttackFireTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( GetAttribute().m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = GetAttribute().m_fAttackEndAniRate;

		pGrp->SetActionAni( GetAttribute().m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	pOwner->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}

	SetSkillActionState( pOwner );
	FindTargetList( pOwner );
}

void ioFireSkill::CheckTargetFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_TargetCharList.empty() )
	{
		m_vTargetPos = ioMath::VEC3_ZERO;

		pOwner->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
								   GetName(),
								   m_dwWeaponBaseIndex,
								   pOwner->GetWorldPosition() );

		return;
	}

	BaseCharDistanceSqList::iterator iter = m_TargetCharList.begin();

	int iWeaponCnt = 0;
	while( iter != m_TargetCharList.end() )
	{
		m_vTargetPos = iter->m_vPos;

		pOwner->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
								   GetName(),
								   m_dwWeaponBaseIndex+iWeaponCnt,
								   pOwner->GetWorldPosition() );

		iWeaponCnt++;

		if( iWeaponCnt >= m_iMaxFireCnt )
			break;

		++iter;
	}
}


bool ioFireSkill::IsNoDropState() const
{	
	if( !m_bNoDropState || GetSkillUseType() != SUT_JUMP || m_fSkillEndNextJumpAmt == 0.0f )
		return ioAttackSkill::IsNoDropState();
	
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
		return false;

	return true;
}

bool ioFireSkill::IsEndState() const
{
	if( !m_bNoDropState || GetSkillUseType() != SUT_JUMP || m_fSkillEndNextJumpAmt == 0.0f )
		return ioAttackSkill::IsEndState();
	
	return false;
}

bool ioFireSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( !m_bNoDropState || GetSkillUseType() != SUT_JUMP || m_fSkillEndNextJumpAmt == 0.0f )
		return ioAttackSkill::IsSkillMotionEnd( dwActionStop, pOwner );

	return false;
}

bool ioFireSkill::IsAttackEndState() const
{
	if( !m_bNoDropState || GetSkillUseType()  != SUT_JUMP || m_fSkillEndNextJumpAmt == 0.0f )
		return ioAttackSkill::IsAttackEndState();

	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

SkillEndJumpType ioFireSkill::GetSkillEndNextJumpMustEnable()
{
	if( !m_bNoDropState || GetSkillUseType() != SUT_JUMP || m_fSkillEndNextJumpAmt == 0.0f )
		return ioAttackSkill::GetSkillEndNextJumpMustEnable();

	return SEJT_JUMP;
}