

#include "stdafx.h"

#include "ioGrowthUpInfo.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioUroborusSkill2.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioUroborusSkill2::ioUroborusSkill2()
{
	m_bSetHandMesh = false;
	m_SkillState = USS_NONE;
	m_szUroborusTargetName.Clear();
	m_iCurLinearCombo = 0;
	m_dwCheckStartTime = 0;
}

ioUroborusSkill2::ioUroborusSkill2( const ioUroborusSkill2 &rhs )
: ioAttackSkill( rhs ),
 m_UroborusMotion( rhs.m_UroborusMotion ),
 m_fTargetAngle( rhs.m_fTargetAngle ),
 m_fTargetRange( rhs.m_fTargetRange ),
 m_UroborusEndMotion( rhs.m_UroborusEndMotion ),
 m_fUroborusEndMotionRate( rhs.m_fUroborusEndMotionRate ),
 m_TargetColType( rhs.m_TargetColType ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_fUpHeight( rhs.m_fUpHeight ),
 m_fUnderHeight( rhs.m_fUnderHeight ),
 m_fWidth( rhs.m_fWidth ),
 m_fSkillRange( rhs.m_fSkillRange ),
 m_fSkillAngle( rhs.m_fSkillAngle ),
 m_fOffSet( rhs.m_fOffSet ),
 m_vComboList( rhs.m_vComboList ),
 m_fTargetMidRate( rhs.m_fTargetMidRate ),
 m_fTargetDownMidRate( rhs.m_fTargetDownMidRate ),
 m_bAutoComboAction( rhs.m_bAutoComboAction ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	m_bSetHandMesh = false;
	m_SkillState = USS_NONE;
	m_szUroborusTargetName.Clear();
	m_iCurLinearCombo = 0;
	m_dwCheckStartTime = 0;
}

ioUroborusSkill2::~ioUroborusSkill2()
{
}

void ioUroborusSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//
	rkLoader.LoadString_e( "uroborus_motion", "", szBuf, MAX_PATH );
	m_UroborusMotion = szBuf;

	m_fTargetAngle = rkLoader.LoadFloat_e( "uroborus_target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "uroborus_target_range", 0.0f );

	rkLoader.LoadString_e( "uroborus_end_motion", "", szBuf, MAX_PATH );
	m_UroborusEndMotion = szBuf;
	m_fUroborusEndMotionRate = rkLoader.LoadFloat_e( "uroborus_end_motion_rate", FLOAT1 );

	// uroborus autotarget
	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_collision_type", TCT_SPHERE );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fSkillRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fSkillAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fOffSet = rkLoader.LoadFloat_e( "target_offset", 0.0f );

	m_fUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fUpHeight = max( FLOAT1, m_fUpHeight );
	m_fUnderHeight = rkLoader.LoadFloat_e( "target_under_height", 0.0f );
	m_fUnderHeight = max( FLOAT1, m_fUnderHeight );
	m_fWidth = rkLoader.LoadFloat_e( "target_width", 0.0f );

	m_fTargetMidRate = rkLoader.LoadFloat_e( "target_mid_rate", FLOAT05 );
	m_fTargetDownMidRate = rkLoader.LoadFloat_e( "target_down_mid_rate", 0.0f );

	// combo
	m_bAutoComboAction = rkLoader.LoadBool_e( "auto_combo_action", false );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	int iComboCnt = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_vComboList.clear();

	if( iComboCnt > 0 )
	{
		m_vComboList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboList.push_back( kComboAttack );
		}
	}
}

ioSkill* ioUroborusSkill2::Clone()
{
	return new ioUroborusSkill2( *this );
}

bool ioUroborusSkill2::OnSkillStart( ioBaseChar *pChar )
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
	m_szUroborusTargetName.Clear();
	m_iCurLinearCombo = 0;
	m_dwCurSkillProtectTime = 0;
	m_dwCheckStartTime = 0;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( StartPreDelayAnimation( pChar ) )
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
	else				// predelay 가 없으면 일단 실패하게 처리
	{
		return false;
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

void ioUroborusSkill2::OnSkillEnd( ioBaseChar *pChar )
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
	m_szUroborusTargetName.Clear();
	m_iCurLinearCombo = 0;
	m_dwCurSkillProtectTime = 0;
}

bool ioUroborusSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioUroborusSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

void ioUroborusSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				if( pChar && pChar->IsNeedProcess() )
				{
					m_dwPreDelayStartTime = 0;
					EndPreDelayEffect( pChar );
					AttackSkillFire( pChar );
				}
			}
		}
		break;
	case USS_FIRE:
		break;
	case USS_UROBORUS:
		if( m_dwCheckStartTime > 0 && m_dwCheckStartTime < dwCurTime )
		{
			m_dwCheckStartTime = 0;
			LOG.PrintTimeAndLog( 0, "BUG - ioUroborusSkill2 : USS_UROBORUS" );
		}
		break;
	case USS_COMBO:
		if( m_dwCheckStartTime > 0 && m_dwCheckStartTime < dwCurTime )
		{
			m_dwCheckStartTime = 0;
			LOG.PrintTimeAndLog( 0, "BUG - ioUroborusSkill2 : USS_UROBORUS" );
		}

		CheckLinearCombo( pChar, false );
		break;
	}
}

void ioUroborusSkill2::SetSkillEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState == USS_COMBO )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_UroborusEndMotion );
	if( iAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = USS_END;
	m_dwCheckStartTime = 0;

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

void ioUroborusSkill2::CheckComboState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( pChar->GetState() != CS_USING_SKILL )
		return;

	CheckLinearCombo( pChar, true );
}

bool ioUroborusSkill2::IsProtected( int iDefenseBreakType ) const
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

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case USS_FIRE:
		return true;
	case USS_UROBORUS:
	case USS_COMBO:
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
		break;
	}

	return false;
}

void ioUroborusSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case US_PRE_INFO:
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			AttackSkillFire( pChar );

			rkPacket >> m_vUroborusDir;
			rkPacket >> m_vTargetPos;
			rkPacket >> m_szUroborusTargetName;
		}
		break;
	case US_COMBO:
		{
			rkPacket >> m_iCurLinearCombo;

			SetNextLinearCombo( pChar );

			if( IsUseAutoTargetValue() )
			{
				pChar->ApplyAutoTargetInfo( rkPacket );
			}
		}
		break;
	}
}

bool ioUroborusSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case USS_NONE:
	case USS_UROBORUS:
		return false;
	}

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

bool ioUroborusSkill2::IsEnableReserve() const
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

void ioUroborusSkill2::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	m_SkillState = USS_FIRE;
	m_dwCheckStartTime = FRAMEGETTIME() + 1500;

	if( pChar && pChar->IsNeedProcess() )
	{
		CheckUroborusAutoTarget( pChar );
		CheckPreUroborusInfo( pChar );
	}

	if( pChar && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << US_PRE_INFO;
		kPacket << m_vUroborusDir;
		kPacket << m_vTargetPos;
		kPacket << m_szUroborusTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUroborusSkill2::CheckPreUroborusInfo( ioBaseChar *pChar )
{
	if( !pChar ) return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vSightDir, &ioMath::UNIT_Y );

	// uroborus target pos
	D3DXQUATERNION qtTargetRot;
	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	D3DXQuaternionRotationAxis( &qtTargetRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vTargetDir = qtTargetRot * vSightDir;
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	m_vTargetPos = pChar->GetMidPositionByRate() + (m_fTargetRange * vTargetDir);

	// uroborus dir
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szUroborusTargetName );
	if( pTarget )
	{
		if( CheckDownAimState( pTarget ) )
			m_vUroborusDir = pTarget->GetMidPositionByRate(m_fTargetDownMidRate) - pChar->GetMidPositionByRate();
		else
			m_vUroborusDir = pTarget->GetMidPositionByRate(m_fTargetMidRate) - pChar->GetMidPositionByRate();
	}
	else
	{
		m_vUroborusDir = vSightDir;
	}

	D3DXVec3Normalize( &m_vUroborusDir, &m_vUroborusDir );
}

void ioUroborusSkill2::SetUroborusMotionState( ioBaseChar *pChar, ioWeapon *pWeapon )
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
	m_dwEnableReserveTime = 0;
	m_dwCheckStartTime = FRAMEGETTIME() + 1500;

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += fMoveTime;

	m_dwCurSkillProtectTime = m_dwMotionEndTime;
}

void ioUroborusSkill2::CheckUroborusAutoTarget( ioBaseChar *pChar )
{
	m_szUroborusTargetName.Clear();

	if( !pChar ) return;
	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage ) return;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	if( m_fOffSet != 0.0f )
		vPos += vSightDir * m_fOffSet;

	// 충돌타입에 따라 리스트 가져오기
	switch( m_TargetColType )
	{
	case TCT_SPHERE:
		pChar->GetTargetCharListBySphere( vPos, m_fSkillRange, m_fSkillAngle, vTargetList );
		break;
	case TCT_CYLINDER:
		pChar->GetTargetCharListByCylinder( vPos, m_fSkillRange, m_fSkillAngle, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	case TCT_BOX:
		// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
		vPos += vSightDir * m_fSkillRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
		vPos -= vOffset;
		pChar->GetTargetCharListByBox( vPos, m_fSkillRange, m_fWidth, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	}

	if( pChar->GetExperienceMode() == EMS_EXPERIENCE )
		vTargetList.clear();
	
	float fTargeRange = std::numeric_limits<float>::infinity();

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )
			continue;
		
		if( pChar == pTarget )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;
			
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fCurRange = D3DXVec3Length( &vDiff );

		if( fCurRange < fTargeRange )
		{
			fTargeRange = fCurRange;
			m_szUroborusTargetName = pTarget->GetCharName();
		}
	}
}

void ioUroborusSkill2::SetNextLinearCombo( ioBaseChar *pChar )
{
	if( !pChar ) return;

	m_dwEnableReserveTime = 0;
	m_dwCheckStartTime = 0;

	const AttributeList &rkComboList = m_vComboList;

	int iMaxCombo = rkComboList.size();
	if( !COMPARE( m_iCurLinearCombo, 0, iMaxCombo ) )
		return;

	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	AttackAttribute kAttack = rkComboList[m_iCurLinearCombo];

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
	}

	if( m_iCurLinearCombo == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
										iAniID,
										kAttack.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	//
	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( m_SkillState != USS_COMBO )
	{
		m_SkillState = USS_COMBO;
		m_dwCheckStartTime = FRAMEGETTIME() + 1500;

		m_dwCurSkillProtectTime = 0;

		if( m_dwSkillProtectTime > 0 )
			m_dwCurSkillProtectTime = dwCurTime + m_dwSkillProtectTime;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << US_COMBO;
		kPacket << m_iCurLinearCombo;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}

void ioUroborusSkill2::CheckLinearCombo( ioBaseChar *pChar, bool bFirst )
{
	if( !pChar )
		return;

	const AttributeList &rkComboList = m_vComboList;
	if( !COMPARE( m_iCurLinearCombo, 0, (int)rkComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bAutoComboAction )
	{
		if( m_dwMotionEndTime < dwCurTime )
		{
			if( pChar->IsNeedProcess() )
			{
				SetNextLinearCombo( pChar );
			}
		}
		return;
	}

	bool bEnableCheck = false;
	if( !bFirst && m_dwEnableReserveTime == 0 )
		return;

	if( m_dwEnableReserveTime < dwCurTime )
	{
		if( pChar->IsNeedProcess() )
		{
			bool bReserveSkill = false;
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				bReserveSkill = true;
			}

			if( !bReserveSkill )
			{
				if( bFirst || pChar->IsAttackKey() )
				{
					SetNextLinearCombo( pChar );
					return;
				}
			}
			else if( bFirst )
			{
				SetNextLinearCombo( pChar );
				return;
			}
		}
	}
}

bool ioUroborusSkill2::CheckDownAimState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() )
		return true;

	return false;
}



