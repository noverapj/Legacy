

#include "stdafx.h"

#include "ioDefenseAttack2Skill.h"
#include "ioNpcChar.h"
#include "ioPlayStage.h"

ioDefenseAttack2Skill::ioDefenseAttack2Skill()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_GrapplingTargetName.Clear();
	m_dwSwingTime = 0;
}

ioDefenseAttack2Skill::ioDefenseAttack2Skill( const ioDefenseAttack2Skill &rhs )
: ioAttackSkill( rhs ),
 m_DefenseLoopAni( rhs.m_DefenseLoopAni ),
 m_fDefenseLoopAniRate( rhs.m_fDefenseLoopAniRate ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_SkillSwingMotion( rhs.m_SkillSwingMotion ),
 m_fSkillSwingMotionRate( rhs.m_fSkillSwingMotionRate ),
 m_SkillSwingedMotion( rhs.m_SkillSwingedMotion ),
 m_fSkillSwingedMotionRate( rhs.m_fSkillSwingedMotionRate ),
 m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
 m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
 m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
 m_fCharHeightRate( rhs.m_fCharHeightRate )
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_GrapplingTargetName.Clear();
	m_dwSwingTime = 0;
}

ioDefenseAttack2Skill::~ioDefenseAttack2Skill()
{
}

void ioDefenseAttack2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "defense_loop_ani", "", szBuf, MAX_PATH );
	m_DefenseLoopAni = szBuf;
	m_fDefenseLoopAniRate = rkLoader.LoadFloat_e( "defense_loop_ani_rate", FLOAT1 );

	// Grappling
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwSkillLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	rkLoader.LoadString_e( "skill_swing_motion", "", szBuf, MAX_PATH );
	m_SkillSwingMotion = szBuf;

	m_fSkillSwingMotionRate = rkLoader.LoadFloat_e( "skill_swing_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "skill_swinged_motion", "", szBuf, MAX_PATH );
	m_SkillSwingedMotion = szBuf;
	m_fSkillSwingedMotionRate = rkLoader.LoadFloat_e( "skill_swinged_motion_rate", FLOAT1 );

	m_fTargetLoopGapRange = rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );
	m_fCharHeightRate = rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );

	int i;
	int iBuffCnt = rkLoader.LoadInt_e( "swing_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_wound_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingWoundBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_cancel_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}
}

ioSkill* ioDefenseAttack2Skill::Clone()
{
	return new ioDefenseAttack2Skill( *this );
}

bool ioDefenseAttack2Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_bChangeDirRot = false;
	m_bReduceGauge = false;
	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

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
		SetDefenseLoopState( pChar );
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
	CheckChangeDirRot( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_GrapplingTargetName.Clear();
	m_dwSwingTime = 0;

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioDefenseAttack2Skill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
}

void ioDefenseAttack2Skill::SetSkillActionState( ioBaseChar *pChar )
{
	m_SkillState = SS_FIRE;
}

void ioDefenseAttack2Skill::OnSkillEnd( ioBaseChar *pChar )
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
	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_GrapplingTargetName.Clear();
	m_dwSwingTime = 0;
}

bool ioDefenseAttack2Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioDefenseAttack2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pChar->SetTargetRotToRotate( qtRot, true, false );

	switch( iState )
	{
	case SSC_LOOP:
		{
			ioHashString szTargetName;
			rkPacket >> m_GrapplingTargetName;

			SetSkillLoopState( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetTargetRotToRotate( qtRot, true, false );
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_SWING:
		{
			int eNewKeyInput;
			rkPacket >> eNewKeyInput;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)eNewKeyInput;

			CheckDirKey( pChar );
			SetSkillSwingState( pChar );
		}
		break;
	}
}

void ioDefenseAttack2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );
}

void ioDefenseAttack2Skill::SetDefenseLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_DefenseLoopAni );
	float fTimeRate = m_fDefenseLoopAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_SkillState = SS_DEFENSE;

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

bool ioDefenseAttack2Skill::IsCanDefenseStateSkill()
{
	switch( m_SkillState )
	{
	case SS_DEFENSE:
		return true;
	}

	return false;
}

void ioDefenseAttack2Skill::SetDefenseStateSkill( ioBaseChar *pChar )
{
	if( !pChar ) return;

	AttackSkillFire( pChar );
}

bool ioDefenseAttack2Skill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( szTargetName.IsEmpty() ) return false;
	if( !m_GrapplingTargetName.IsEmpty() ) return false;
	m_GrapplingTargetName = szTargetName;

	SetSkillLoopState( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LOOP;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioDefenseAttack2Skill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetLoopGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioDefenseAttack2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_LOOP:
		return false;
	case SS_DEFENSE:
	case SS_FIRE:
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
			return true;
		return false;
	}

	return false;
}

bool ioDefenseAttack2Skill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	switch( m_SkillState )
	{
	case SS_DEFENSE:
		return false;
	case SS_FIRE:
	case SS_SWING:
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
	case SS_LOOP:
		if( m_dwSkillLoopProtectTime > 0 && m_dwSkillLoopStartTime + m_dwSkillLoopProtectTime > dwCurTime )
			return true;

		return false;
	}

	return false;
}

bool ioDefenseAttack2Skill::IsCanSwitchSkill() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_SkillState != SS_DEFENSE )
		return false;

	if( !m_bReduceGauge )
		return false;

	if( m_dwEnableSwitchTime > 0 && m_dwEnableSwitchTime < FRAMEGETTIME() )
		return true;

	return false;
}

void ioDefenseAttack2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				SetDefenseLoopState( pChar );
			}
		}
		break;
	case SS_DEFENSE:
		break;
	case SS_FIRE:
		break;
	case SS_LOOP:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwSkillLoopStartTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillEndState( pChar );
					return;
				}
			}
			else
			{
				SetSkillEndState( pChar );
				return;
			}

			if( m_dwSkillLoopStartTime > 0 && m_dwSkillLoopStartTime+m_dwSkillLoopDuration < dwCurTime )
				SetSkillSwingState( pChar );

			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				CheckDirKey( pChar );
				SetSkillSwingState( pChar );
			}
		}
		break;
	case SS_SWING:
		{
			if( m_dwSwingTime > 0 && m_dwSwingTime <= dwCurTime )
			{
				ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
				if( !pTarget ) return;

				// for NPC
				bool bNPC = false;
				if( ToNpcChar( pTarget ) )
				{
					bNPC = true;
				}

				// local or 훈련소 or NPC
				if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
				{
					if( pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL && pTarget->IsNeedProcess() )
					{
						pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
						pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(),
														 m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
					}
				}
				
				if( pTarget && pTarget->IsNeedProcess() && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
				{
					pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
					pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(),
													 m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
				}

				SetSkillEndState( pChar );

				m_dwSwingTime = 0;
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioDefenseAttack2Skill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_FIRE )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_SkillState = SS_LOOP;

	m_dwSkillLoopStartTime = FRAMEGETTIME();
}

void ioDefenseAttack2Skill::SetSkillSwingState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillSwingMotionRate > 0.0f )
		fAniRate = m_fSkillSwingMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillSwingMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	m_dwSwingTime = dwCurTime;
	m_dwSwingTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	m_SkillState = SS_SWING;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_SWING;
		kPacket << pChar->GetTargetRot();
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDefenseAttack2Skill::SetSkillEndState( ioBaseChar *pChar )
{
	m_SkillState = SS_END;
}

void ioDefenseAttack2Skill::CheckDirKey( ioBaseChar *pChar )
{
	if( !pChar )	return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey == ioUserKeyInput::DKI_NONE )	return;
	if( eNewDirKey == m_CurDirKey )	return;

	D3DXVECTOR3 vCurMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		pChar->SetTargetRotAndMoveDirChange( qtRotate );
		ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
			D3DXVECTOR3 vDir = pChar->GetMoveDir();

			if( m_fTargetLoopGapRange > 0.0f )
				vPos += m_fTargetLoopGapRange * vDir;

			pTarget->SetWorldPosition( vPos );
		}
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pChar->SetTargetRotAndMoveDirChange( qtRotate );
	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( pTarget )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		pTarget->SetWorldPosition( vPos );
	}
}