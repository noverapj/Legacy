

#include "stdafx.h"

#include "ioTraceSkill2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ioNpcChar.h"

ioTraceSkill2::ioTraceSkill2()
{
	m_dwFireStartTime = 0;
	m_bSetHandMesh = false;
	m_bInitJumpPower = true;

	m_State = SS_NONE;
}

ioTraceSkill2::ioTraceSkill2( const ioTraceSkill2 &rhs )
	: ioNormalSkill( rhs ),
	m_fCharJumpPower( rhs.m_fCharJumpPower ),
	m_WaitAni( rhs.m_WaitAni ),
	m_dwWaitDuration( rhs.m_dwWaitDuration ),
	m_dwWaitProtectionTime( rhs.m_dwWaitProtectionTime ),
	m_ActionLoopAni( rhs.m_ActionLoopAni ),
	m_FirstAttackAttribute( rhs.m_FirstAttackAttribute ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_FireMotionAni( rhs.m_FireMotionAni ),
	m_fFireMotionRate( rhs.m_fFireMotionRate),
	m_dwWaitTimeForAttack( rhs.m_dwWaitTimeForAttack ),
	m_szWarpExceptTargetBuffList( rhs.m_szWarpExceptTargetBuffList ),
	m_bCheckDropZone( rhs.m_bCheckDropZone ),
	m_LandAttack( rhs.m_LandAttack ),
	m_strJumpAni( rhs.m_strJumpAni ),
	m_fJumpAniRate( rhs.m_fJumpAniRate ),
	m_fTargetOffSetRate( rhs.m_fTargetOffSetRate ),
	m_fTargetYOffSet( rhs.m_fTargetYOffSet ),
	m_fJumpPowerInJump( rhs.m_fJumpPowerInJump ),
	m_dwPreDelayTimeInJump( rhs.m_dwPreDelayTimeInJump ),
	m_iTargetLimitCount( rhs.m_iTargetLimitCount ),
	m_GrapplingAttack( rhs.m_GrapplingAttack ),
	m_bReverseDirGrapplingAttack( rhs.m_bReverseDirGrapplingAttack ),
	m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
	m_fCharHeightRate( rhs.m_fCharHeightRate )
{
	m_State = SS_NONE;
	m_bInitJumpPower = true;

	m_dwFireStartTime = 0;
	m_bSetHandMesh = false;
	m_bAttackedWeapon = false;
}

ioTraceSkill2::~ioTraceSkill2()
{
}

void ioTraceSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = ""; 

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTargetMinRange = min( m_fTargetMinRange, m_fTargetRange );
	m_iTargetLimitCount = rkLoader.LoadInt_e( "target_limit_count", 0 );

	m_dwWaitProtectionTime = (DWORD)rkLoader.LoadInt_e( "wait_protection_time", 0 );

	float fTest = m_fTargetAngle;

	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_TargetEffect = szBuf;

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	//////////////////////////////////////////////////////////////////////////
	//점프
	m_fCharJumpPower = rkLoader.LoadFloat_e( "jump_power", 0 );

	rkLoader.LoadString_e( "skill_motion", "", szBuf, MAX_PATH );
	m_strJumpAni = szBuf;

	m_fJumpAniRate = rkLoader.LoadFloat_e( "animation_time_rate", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	//점프해서 공격
	rkLoader.LoadString_e( "skill_uroborus_motion", "", szBuf, MAX_PATH );
	m_FireMotionAni = szBuf;
	m_fFireMotionRate = rkLoader.LoadFloat_e( "skill_uroborus_motion_ani_rate", FLOAT1 );
	
	WeaponInfo weapon;
	weapon.m_iWeaponIdx = rkLoader.LoadInt_e( "skill_uroborus_attack_type", 0 );
	m_FirstAttackAttribute.m_vWeaponInfoList.push_back( weapon );

	//공격후 기달리는 시간
	m_dwWaitTimeForAttack = (DWORD)rkLoader.LoadInt_e( "wait_time_for_attack", 0 );
	//////////////////////////////////////////////////////////////////////////
	//오프셋
	m_fTargetOffSetRate = rkLoader.LoadFloat_e("Target_OffSetRate", 0 );
	m_fTargetYOffSet = rkLoader.LoadFloat_e("Target_Y_OffSet", 0 );

	//랜드 어택
	rkLoader.LoadString_e( "skill_land_attack_ani", "", szBuf, MAX_PATH );
	m_LandAttack.m_AttackAnimation = szBuf;

	int iAttackCount = rkLoader.LoadInt( "skill_land_attack_type_count", 0 );
	if ( iAttackCount == 0 )
	{
		weapon.m_iWeaponIdx = rkLoader.LoadInt_e( "skill_land_attack_type", 0);
		m_LandAttack.m_vWeaponInfoList.push_back( weapon );
	}
	else
	{
		m_LandAttack.m_vWeaponInfoList.reserve( iAttackCount );

		for( int i=0 ; i<iAttackCount ; i++ )
		{
			WeaponInfo kInfo;

			wsprintf_e( szKey, "skill_land_attack_type%d", i+1 );
			kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "skill_land_attack_type%d_resist", i+1 );
			kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "skill_land_attack_type%d_wounded", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szKey, "skill_land_attack_type%d_wounded_time", i+1 );
			kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "skill_land_attack_type%d_wounded_loop_ani", i+1 );
			kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			m_LandAttack.m_vWeaponInfoList.push_back( kInfo );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//???
	m_bCheckDropZone = rkLoader.LoadBool_e( "check_drop_zone", false );

	//////////////////////////////////////////////////////////////////////////
	//점프중에 셋팅
	m_fJumpPowerInJump = rkLoader.LoadFloat_e( "jump_power_In_Jump", 0 );
	m_dwPreDelayTimeInJump = rkLoader.LoadInt_e( "pre_delay_time_In_Jump", 0 );

	LoadAttackAttribute_e( "grappling_attack", m_GrapplingAttack, rkLoader );

	m_bReverseDirGrapplingAttack = rkLoader.LoadBool_e( "reverse_dir_grappling_attack", false );
	m_fTargetLoopGapRange = rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );
	m_fCharHeightRate = rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
}

ioSkill* ioTraceSkill2::Clone()
{
	return new ioTraceSkill2( *this );
}


void ioTraceSkill2::SetPreAction( ioBaseChar *pChar )
{
	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );
}


void ioTraceSkill2::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_strJumpAni );
	if( iCurSkillAniID == -1 )	return;

	float fTimeRate = m_fJumpAniRate;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );

	if( m_bSkillInJump )
		pChar->SetJumpPower( m_fJumpPowerInJump );
	else
		pChar->SetJumpPower( m_fCharJumpPower );

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwPreActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_State = SS_PRE_ACTION;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}


void ioTraceSkill2::SetWaitState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwWaitStartTime = dwCurTime;
	m_State = SS_WAIT;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
}


void ioTraceSkill2::SetFireState( ioBaseChar *pChar )
{
	FindTarget( pChar );
}


void ioTraceSkill2::SetActionLoopState( ioBaseChar *pChar )
{

}


void ioTraceSkill2::SetExtraActionState( ioBaseChar *pChar )
{

}


bool ioTraceSkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	m_bSkillInJump = false;
	if( pChar->IsJumpingSkillState() || pChar->GetState() == CS_LUCIFER_FLY )
	{
		m_bSkillInJump = true;
		//m_fCharJumpPower = m_fJumpPowerInJump;
		//m_dwPreDelayTime = m_dwPreDelayTimeInJump;
	}

	m_State = SS_NONE;
	m_bReduceGauge = false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	m_dwAttackEndTime = 0;

	m_dwWaitStartTime = 0;
	m_fCurChargeRate = 0.0f;

	m_bSetHandMesh = false;

	m_bAttackedWeapon = false;

	m_dwPreActionEndTime = 0;
	m_dwWaitStartTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillProtection();

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		SetPreAction( pChar );

	SetSkillNameChatBubble( pChar );

	CheckChangeDirRot( pChar );

	m_szGrapTargetName.Clear();
	m_dwGrapplingStartTime = 0;

	return true;
}

void ioTraceSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	int iSlot = GetOwnerItem()->GetType() - 1;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetPreAction( pChar );
			}
		}
		break;
	case SS_PRE_ACTION://올라가고 있는 상태
		if( m_dwPreActionEndTime < dwCurTime )
		{
			SetWaitState( pChar );
		}
		break;
	case SS_WAIT:
		if(  pChar->IsNeedProcess() )//&& (m_dwWaitStartTime + m_dwWaitDuration) <= dwCurTime )
		{
			FindTarget( pChar );
			StartFireMotion( pChar );
		}
		break;
	case SS_FIRE:
		{
			if( m_dwFireEventTime <= dwCurTime )
			{
				Fire( pChar );
				m_State = SS_ACTION_LOOP;
			}

			if( m_dwFireEndTime <= dwCurTime )
				m_State = SS_END;

			break;
		}
	case SS_ACTION_LOOP:
		{
			if( pChar->IsNeedProcess() && (m_dwFireEndTime+m_dwWaitTimeForAttack) <= dwCurTime )
			{
				if( m_bAttackedWeapon )
				{
					SetActionState( pChar );
				}
				else
				{
					m_dwAttackEndTime = dwCurTime;
					m_State = SS_ACTION;
				}
				break;
			}
		}
		break;
	case SS_ACTION:
		if( m_dwFireEndTime && m_dwFireEndTime <= dwCurTime )
			m_State = SS_END;

		break;
	case SS_GRAPPLING_ATTACK:
		{
			//if ( !CheckTargetState( pChar ) )
			//{
			//	m_State = SS_END;
			//	return;
			//}
			if( m_dwFireEndTime && m_dwFireEndTime <= dwCurTime )
			{
				m_State = SS_END;
				return;
			}
		}
		break;
	case SS_EXTRA_ACTION:
		{
		}
		break;
	}
}


void ioTraceSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_dwWaitStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;
	m_vTargetList.clear();
}


bool ioTraceSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}


void ioTraceSkill2::Fire(  ioBaseChar *pOwner  )
{
	m_dwAttackEndTime = 0;

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
	{
		// 타겟의 개수만큼 충돌체 생성
		int iTargetCnt = m_vTargetList.size();
		int iCurMaxCnt = iTargetCnt;
		if( iCurMaxCnt == 0 )
			iCurMaxCnt = 1;

		int iCurFireCnt = 0;
		for(int i=0; i<iTargetCnt; ++i)
		{
			m_CurTargetName = m_vTargetList[i];

			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			pOwner->SkillFireExplicit(m_FirstAttackAttribute.m_vWeaponInfoList.front(),
				GetName(),
				m_dwWeaponBaseIndex+i,
				vPos);

			iCurFireCnt++;
		}

		if( iCurMaxCnt > 0 && iCurFireCnt < iCurMaxCnt )
		{
			m_CurTargetName.Clear();

			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			pOwner->SkillFireExplicit( m_FirstAttackAttribute.m_vWeaponInfoList.front(),
				GetName(),
				m_dwWeaponBaseIndex,
				vPos);
		}

		m_dwFireStartTime = 0;
	}
}


float ioTraceSkill2::GetSkillGravityRate()
{
	if( m_State != SS_ACTION_LOOP )
		return FLOAT1;

	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_fSkillGravityRate;
	case SUT_DOWN:
		if( m_fSkillGravityDownRate > 0.0f )
			return m_fSkillGravityDownRate;
		return m_fSkillGravityRate;
	case SUT_JUMP:
		if( m_fSkillGravityJumpRate > 0.0f )
			return m_fSkillGravityJumpRate;
		return m_fSkillGravityRate;
	}

	return m_fSkillGravityRate;
}


bool ioTraceSkill2::IsNoDropState() const
{
	if( m_State == SS_WAIT || m_State == SS_FIRE || m_State == SS_ACTION_LOOP //|| m_State == SS_ACTION_WARP
		|| m_State == SS_ACTION || m_State == SS_GRAPPLING_ATTACK)
		return true;

	return false;
}


bool ioTraceSkill2::IsEndState() const
{
	if( m_State == SS_WAIT )
	{
		if( m_dwWaitStartTime+m_dwWaitDuration < FRAMEGETTIME() )
			return true;
	}

	return false;
}


D3DXVECTOR3 ioTraceSkill2::CalcReturnUroborusPosition( ioWeapon *pWeapon )
{
	m_bAttackedWeapon = true;

	ioBaseChar *pChar = pWeapon->GetOwner();

	D3DXVECTOR3 vOffSet = pChar->GetWorldPosition() - pWeapon->GetPosition();
	vOffSet.y = 0;
	D3DXVec3Normalize( &vOffSet, &vOffSet );
	vOffSet = vOffSet * m_fTargetOffSetRate;

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vReturnPos;
	vReturnPos = pChar->GetWorldPosition() + D3DXVECTOR3(0, m_fTargetYOffSet, 0 ) - vOffSet;

	return vReturnPos;
}


SkillCharColType ioTraceSkill2::IsCharSkipState() const
{
	if( m_EnableCharCollisionSkip == SCCT_ALL )
	{
		switch( m_State )
		{
		case SS_ACTION_LOOP:
			return SCCT_ALL;
		default:
			return SCCT_NONE;
		}
	}

	return m_EnableCharCollisionSkip;
}


bool ioTraceSkill2::IsProtected( int iDefenseBreakType ) const
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

	switch( m_State )
	{
	case SS_NONE:
		break;
	case SS_PRE_ACTION:
		return true;
	case SS_FIRE:
	case SS_WAIT:
	case SS_ACTION_LOOP:
	case SS_ACTION:
	case SS_EXTRA_ACTION:
		if( m_dwWaitStartTime + m_dwWaitProtectionTime > dwCurTime )
			return true;
		break;
	/*case SS_ACTION:
	case SS_EXTRA_ACTION:
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
		break;*/
	}

	return false;
}


bool ioTraceSkill2::IsEnableReserve() const
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
	case SS_PRE_ACTION:
	case SS_WAIT:
	case SS_ACTION_LOOP:
		return false;
	}

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < dwCurTime )
		return true;

	return false;
}


bool ioTraceSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
	case SS_PRE_ACTION:
	case SS_ACTION_LOOP:
	case SS_WAIT:
	case SS_ACTION:
	case SS_GRAPPLING_ATTACK:
		return false;
	case SS_EXTRA_ACTION:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
			return true;
	case SS_END:
		return true;

		break;
	}

	return false;
}


void ioTraceSkill2::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_FirstAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = FLOAT1;//GetAttackTimeRate();
	DWORD dwPreDelay = m_FirstAttackAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_FirstAttackAttribute.m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStartTime = dwFireStartTime;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

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
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	m_bReduceGauge = true;
	//

	SetSkillActionState( pChar );
}

void ioTraceSkill2::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}


bool ioTraceSkill2::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}


void ioTraceSkill2::SetWarpTargetList( const ioHashString &szTargetName )
{
	if( szTargetName.IsEmpty() )	return;

	ioHashStringVec::iterator iter = std::find( m_vWarpTargetList.begin(),
		m_vWarpTargetList.end(),
		szTargetName );

	if( iter == m_vWarpTargetList.end() )
		m_vWarpTargetList.push_back( szTargetName );
}


void ioTraceSkill2::FindTarget( ioBaseChar *pOwner )
{
	m_vWarpTargetList.clear();

	ioHashString szTarget;

	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	// target list
	m_vTargetList.clear();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	int nTargetCnt = 0;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		if( nTargetCnt >= m_iTargetLimitCount )
			break;

		ioBaseChar *pCurChar = *iter;

		if( pCurChar == pOwner )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar, true ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap >= m_fTargetAngle )
			continue;

		pCurChar->AttachEffect( m_TargetEffect );
		szTarget = pCurChar->GetCharName();
		m_vTargetList.push_back(szTarget);
		nTargetCnt++;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		if( !m_vTargetList.empty() )
		{
			int iSize = m_vTargetList.size();

			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_TARGET;
			kPacket << iSize;

			for( int i=0; i < iSize; ++i )
				kPacket << m_vTargetList[i];

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


void ioTraceSkill2::StartFireMotion( ioBaseChar *pOwner )
{
	m_dwFireEventTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_FireMotionAni );

	pGrp->ClearAllLoopAni( 5.0f );
	pGrp->ClearAllActionAni( 5.0f );
	//pGrp->SetLoopAni( iAniID, 5.0f );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fFireMotionRate, 0.0f, 0.0f, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEventTime = m_dwFireStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_fFireMotionRate;
	m_dwFireEndTime = m_dwFireStartTime + pGrp->GetAnimationFullTime( iAniID ) * m_fFireMotionRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * m_fFireMotionRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	m_State = SS_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SSC_FIREMOTION;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTraceSkill2::SetActionState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_LandAttack.m_AttackAnimation );
	if ( m_LandAttack.m_fAttackAniRate > 0.0f )
		fAniRate = m_LandAttack.m_fAttackAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fAniRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	m_dwFireEndTime = dwCurTime;
	m_dwFireEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	DWORD dwCurBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_LandAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fAniRate,
		0 );

	//////////////////////////////////////////////////////////////////////////
	m_State = SS_ACTION;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioTraceSkill2::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() )
		return false;

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	return true;
}

void ioTraceSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_ATTACK && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_CHANGE3 && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );
	else if( pWeapon->IsUseGrapplingTarget() && !m_szGrapTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_szGrapTargetName );

	//
	ioUroborusWeapon3 *pUroborus3 = ToUroborusWeapon3( pWeapon );
	if( pUroborus3 )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_CurTargetName.IsEmpty() )
			{
				pUroborus3->SetTraceTargetName( m_CurTargetName );
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_CurTargetName );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &vDir, &vDir );

					//if( m_bWeaponLifeUse )
					{
						//pUroborus3->SetLifeTime( m_fWeaponLifeTime );
						pUroborus3->SetOneTimeColCheck( true );
					}
				}
			}
			
			pUroborus3->SetMoveDir( vDir );
			pUroborus3->SetSkillWeapon( true );
		}
	}

	//
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( pZone->GetPosition(), true );
			//pZone->SetGrapplingTargetName( m_GrapplingTargetName );
		}
	}


	/*ioTraceWeaponType2 *pTraceWeapon = ToTraceWeaponType2( pWeapon );
	if( pTraceWeapon )
	{
		pTraceWeapon->SetTraceTarget( m_CurTargetName );
	}*/
}

void ioTraceSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_TARGET:
		{
			int iSize;
			rkPacket >> iSize;

			m_vTargetList.clear();
			for( int i=0; i < iSize; ++i )
			{
				ioHashString szTargetName;
				rkPacket >> szTargetName;

				m_vTargetList.push_back( szTargetName );
			}
		}
		break;
	case SSC_FIREMOTION:
		{
			StartFireMotion( pChar );
		}
		break;
	case SSC_ACTION:
		{
			SetActionState( pChar );
		}
		break;
	case SSC_GRAPPLING_ATTACK:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos;
			rkPacket >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotAndMoveDirChange( qtRot );

			rkPacket >> m_szGrapTargetName;

			SetGrapplingAttack( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_szGrapTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetTargetRotToRotate( qtRot, true, false );
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
	}

}

bool ioTraceSkill2::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime <= FRAMEGETTIME() )
		return true;

	return false;
}

bool ioTraceSkill2::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) 
		return false;

	if( m_State != SS_ACTION )
		return false;

	m_szGrapTargetName = szTargetName;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szGrapTargetName );
	if( !pTarget ) 
		return false;

	SetGrapplingAttack( pChar );

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
		bNPC = true;

	if( ToNpcChar( pChar ) )
		bAttackerNPC = true;

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
		pTarget->SetTargetRotToRotate( qtCharRot, true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioTraceSkill2::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetLoopGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

void ioTraceSkill2::SetGrapplingAttack( ioBaseChar *pChar )
{
	if( !pChar ) 
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if ( m_GrapplingAttack.m_fAttackAniRate > 0.0f )
		fAniRate = m_GrapplingAttack.m_fAttackAniRate;
	int iAniID = pGrp->GetAnimationIdx( m_GrapplingAttack.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fAniRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	m_dwFireEndTime = dwCurTime;
	m_dwFireEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwGrapplingStartTime = dwCurTime;

	DWORD dwCurBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_GrapplingAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fAniRate,
		0 );

	//////////////////////////////////////////////////////////////////////////
	m_State = SS_GRAPPLING_ATTACK;

	if ( pChar->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pChar->IsSettedDirection() )
		{
			eNewDirKey = pChar->CheckCurDirKey();
			pChar->SetTargetRotToDirKey( eNewDirKey );
			pChar->SetWorldOrientation( pChar->GetTargetRot() );
		}
		else if ( m_bReverseDirGrapplingAttack )
		{
			//방향키 입력이 없다면 반대방향으로 공격
			D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationYawPitchRoll( &qtRot, DEGtoRAD(180.0f), 0.0f, 0.0f );
			vDir = qtRot * vDir;
			ioMath::CalcDirectionQuaternion( qtRot, -vDir );
			pChar->SetTargetRotAndMoveDirChange( qtRot );
			pChar->SetWorldOrientation( pChar->GetTargetRot() );
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_GRAPPLING_ATTACK;
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetWorldOrientation();
			kPacket << m_szGrapTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioTraceSkill2::CheckTargetState( ioBaseChar *pChar )
{
	if ( !pChar || m_szGrapTargetName.IsEmpty() )
		return false;

	ioBaseChar* pTarget = pChar->GetBaseChar( m_szGrapTargetName );
	if( !pTarget )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwGrapplingStartTime + g_App.GetGrapplingCheckDelay() < dwCurTime && pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
		return false;

	return true;
}

