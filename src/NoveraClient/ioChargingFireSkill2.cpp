

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioChargingFireSkill2.h"

#include "WeaponDefine.h"

ioChargingFireSkill2::ioChargingFireSkill2()
{
	m_State = SS_NONE;

	m_dwChargingEffect = -1;

	m_bSetHandMesh = false;

	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_fCurChargeRate = 0.0f;

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioChargingFireSkill2::ioChargingFireSkill2( const ioChargingFireSkill2 &rhs )
: ioNormalSkill( rhs ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_dwMaxChargingTime( rhs.m_dwMaxChargingTime ),
 m_SkillChargingEffect( rhs.m_SkillChargingEffect ),
 m_dwCharRotateSpeed( rhs.m_dwCharRotateSpeed ),
 m_dwLoopDuration( rhs.m_dwLoopDuration )
{
	m_State = SS_NONE;

	m_dwChargingEffect = -1;

	m_bSetHandMesh = false;

	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_fCurChargeRate = 0.0f;

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioChargingFireSkill2::~ioChargingFireSkill2()
{
}

void ioChargingFireSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_dwMaxChargingTime = rkLoader.LoadInt_e( "charging_max_time", 0 );

	rkLoader.LoadString_e( "skill_charging_effect", "", szBuf, MAX_PATH );
	m_SkillChargingEffect = szBuf;

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_dwCharRotateSpeed = rkLoader.LoadInt_e( "char_rotate_speed", 0 );

	// Action Attack
	m_AttackAttribute.Init();
	LoadAttackAttribute_e( "charging_attack", m_AttackAttribute, rkLoader );
}

ioSkill* ioChargingFireSkill2::Clone()
{
	return new ioChargingFireSkill2( *this );
}

bool ioChargingFireSkill2::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_dwChargingEffect = -1;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_fCurChargeRate = 0.0f;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioChargingFireSkill2::OnSkillEnd( ioBaseChar *pChar )
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

	EndCameraBuff( pChar );

	//DestroyMapEffect
	if( m_dwChargingEffect != -1 )
		pChar->EndMapEffect( m_dwChargingEffect, false );

	m_dwChargingEffect = -1;

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_fCurChargeRate = 0.0f;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_State = SS_NONE;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void ioChargingFireSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	int iSlot = GetOwnerItem()->GetType() - 1;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckCharRotateKey( pChar );
			ProcessCharRotateDir( pChar );
			ProcessLoopState( pChar, pStage );

			if( m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
			{
				SetActionState( pChar, pStage, false );
				return;
			}

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() ||
					pChar->IsDefenseKey() ||
					pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					SetActionState( pChar, pStage, true );
					return;
				}
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_dwFireEndTime > 0 && m_dwFireEndTime < dwCurTime )
			{
				if( m_dwChargingEffect != -1 )
					pChar->EndMapEffect( m_dwChargingEffect, false );

				m_dwFireEndTime = 0;
			}
		}

		if( m_dwChargingEffect != -1 )
		{
			pChar->UpdateAttachMapEffect( m_dwChargingEffect );
		}
		break;
	}
}

bool ioChargingFireSkill2::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioChargingFireSkill2::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar ) return;

	pChar->AttachEffect( m_StartEffect );
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioChargingFireSkill2::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	EndPreDelayEffect( pChar );

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;
	m_dwLoopStartTime = dwCurTime;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_dwChargingEffect != -1 )
	{
		pChar->EndEffect( m_dwChargingEffect, false );
	}

	ioMapEffect *pChargingEff = pChar->CreateMapEffectAttachChar( m_SkillChargingEffect );
	if( pChargingEff )
	{
		m_dwChargingEffect = pChargingEff->GetUniqueID();
	}

	/*
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LOOP;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	*/
}

void ioChargingFireSkill2::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	if( m_State == SS_ACTION )
		return;

	SetNormalSkill( pChar );

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_ACTION;

	// Charging Rate
	DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;
	if( m_dwLoopStartTime > 0 && m_dwMaxChargingTime > 0 )
	{
		if( m_dwMaxChargingTime > dwGapTime )
			m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargingTime;
		else
			m_fCurChargeRate = FLOAT1;
	}

	float fCurChargeAniRate = FLOAT1;
	float fCurForceSlidingRate = FLOAT1;
	if( m_fCurChargeRate > 0.0f )
	{
		if( m_fMaxChargeAniRate > 0.0f )
		{
			float fCurMaxAniRate = max( 0.0f, (m_fMaxChargeAniRate - FLOAT1) );
			fCurChargeAniRate += fCurMaxAniRate * m_fCurChargeRate;
		}

		if( m_fMaxForceSlidingRate > 0.0f )
		{
			float fCurMaxSlidingRate = max( 0.0f, (m_fMaxForceSlidingRate - FLOAT1) );
			fCurForceSlidingRate += fCurMaxSlidingRate * m_fCurChargeRate;
		}
	}
	//

	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	float fAniRate = m_AttackAttribute.m_fAttackAniRate * fCurChargeAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = m_AttackAttribute.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( iAniID != -1 )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	// Attack
	if( !m_AttackAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											m_AttackAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fAniRate,
											dwPreDelay );
	}

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	float fFireStart = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" );
	if( fFireStart > 0.0f )
	{
		m_dwFireStartTime = dwCurTime + fFireStart + dwPreDelay;
	}

	float fFireEnd = pGrp->GetAnimationEventTime_e( iAniID, "fire_e" ) * fAniRate;
	if( fFireEnd > 0.0f )
	{
		m_dwFireEndTime = dwCurTime + fFireEnd + dwPreDelay;
	}

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fAniRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd + dwPreDelay;
	else
		m_dwAttackEndTime = 0;

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fAniRate, dwPreDelay );

	pChar->AttachEffect( m_EndEffect );

	pChar->SetReservedSliding( m_AttackAttribute.m_vForceInfoList, iAniID, fAniRate, dwPreDelay, fCurForceSlidingRate );

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargingFireSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_LOOP:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );

			SetLoopState( pChar );
		}
		break;
	case SSC_ACTION:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );

			SetActionState( pChar, pStage, false );
		}
		break;
	case SSC_DIR:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iCurKey;
			rkPacket >> iCurKey;

			m_bTargetRot = false;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iCurKey;
		}
		break;
	}
}

bool ioChargingFireSkill2::IsProtected( int iDefenseBreakType ) const
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

	switch( m_State )
	{
	case SS_LOOP:
		return false;
	case SS_ACTION:
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioChargingFireSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioChargingFireSkill2::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioChargingFireSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioChargingFireSkill2::ProcessLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( m_dwChargingEffect == -1 )
		return;

	pChar->UpdateAttachMapEffect( m_dwChargingEffect );

	// Charging Rate
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;
	if( m_dwLoopStartTime > 0 && m_dwMaxChargingTime > 0 )
	{
		if( m_dwMaxChargingTime > dwGapTime )
			m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargingTime;
		else
			m_fCurChargeRate = FLOAT1;
	}

	float fCurScaleRate = FLOAT1;
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurChargeRate;

		ioMapEffect *pMapEff = pStage->FindMapEffect( m_dwChargingEffect );
		if( pMapEff && fCurScaleRate > FLOAT1 )
		{
			ioEffect *pEff = pMapEff->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vScale( fCurScaleRate, fCurScaleRate, fCurScaleRate );
				pEff->GetParentNode()->SetScale( vScale );
			}
		}
	}
}

void ioChargingFireSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = pWeapon->GetOwner();

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
}

void ioChargingFireSkill2::CheckCharRotateKey( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_DIR;
			kPacket << pChar->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioChargingFireSkill2::ProcessCharRotateDir( ioBaseChar *pChar )
{
	if( m_dwCharRotateSpeed == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwCharRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
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
		m_bTargetRot = true;
		pChar->SetTargetRotToRotate( qtRotate, true );
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
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pChar->SetTargetRotToRotate( qtNewRot, true );
}






