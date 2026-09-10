

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


#include "ioMechanicsFireSkill.h"

ioMechanicsFireSkill::ioMechanicsFireSkill()
{
	m_bSetHandMesh = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurFireCnt = 0;

	m_bSetMaxCharge = false;
	m_bTargetRot = false;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_SkillState = SS_NONE;
	m_dwChargeMaxTime = 0;
	m_dwFireEnableTime = 0;
	m_dwActionEndTime = 0;

	m_dwCurSkillProtectTime = 0;

	m_dwChargeEffect = 0;
	m_dwChargeMaxEffect = 0;
	m_dwUpDownEffect = 0;
}

ioMechanicsFireSkill::ioMechanicsFireSkill( const ioMechanicsFireSkill &rhs )
: ioAttackSkill( rhs ),
 m_iMaxFireCnt( rhs.m_iMaxFireCnt ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_ChargeMotion( rhs.m_ChargeMotion ),
 m_dwChargeDuration( rhs.m_dwChargeDuration ),
 m_dwFireEnableDuration( rhs.m_dwFireEnableDuration ),
  m_dwRotateTime( rhs.m_dwRotateTime ),
 m_ChargeEffect( rhs.m_ChargeEffect ),
 m_ChargeMaxEffect( rhs.m_ChargeMaxEffect ),
 m_ChargeUpEffect( rhs.m_ChargeUpEffect ),
 m_ChargeDownEffect( rhs.m_ChargeDownEffect ),
 m_fUpDownSpeed( rhs.m_fUpDownSpeed )
{
	m_bSetHandMesh = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurFireCnt = 0;

	m_bSetMaxCharge = false;
	m_bTargetRot = false;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_SkillState = SS_NONE;
	m_dwChargeMaxTime = 0;
	m_dwFireEnableTime = 0;
	m_dwActionEndTime = 0;

	m_dwCurSkillProtectTime = 0;

	m_dwChargeEffect = 0;
	m_dwChargeMaxEffect = 0;
	m_dwUpDownEffect = 0;
}

ioMechanicsFireSkill::~ioMechanicsFireSkill()
{
}

void ioMechanicsFireSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_iMaxFireCnt = rkLoader.LoadInt_e( "max_fire_cnt", 1 );

	rkLoader.LoadString_e( "charge_ani", "", szBuf, MAX_PATH );
	m_ChargeMotion = szBuf;

	m_dwChargeDuration = (DWORD)rkLoader.LoadInt_e( "charge_duration", 3000 );
	m_dwFireEnableDuration = (DWORD)rkLoader.LoadInt_e( "fire_enable_duration", 1000 );

	rkLoader.LoadString_e( "charge_effect", "", szBuf, MAX_PATH );
	m_ChargeEffect = szBuf;
	rkLoader.LoadString_e( "charge_max_effect", "", szBuf, MAX_PATH );
	m_ChargeMaxEffect = szBuf;

	rkLoader.LoadString_e( "up_move_effect", "", szBuf, MAX_PATH );
	m_ChargeUpEffect = szBuf;
	rkLoader.LoadString_e( "down_move_effect", "", szBuf, MAX_PATH );
	m_ChargeDownEffect = szBuf;

	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "skill_rotate_speed", 0 );
	m_fUpDownSpeed = rkLoader.LoadFloat_e( "updown_speed", FLOAT10 );
}

ioSkill* ioMechanicsFireSkill::Clone()
{
	return new ioMechanicsFireSkill( *this );
}

bool ioMechanicsFireSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( pChar->GetState() == CS_JUMP )
		m_bJumpSkill = true;
	else
		m_bJumpSkill = false;

	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

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

	m_dwSkillMapEffect = -1;

	m_bSetHandMesh = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurFireCnt = 0;

	m_bSetMaxCharge = false;
	m_bTargetRot = false;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_SkillState = SS_NONE;
	m_dwChargeMaxTime = 0;
	m_dwFireEnableTime = 0;
	m_dwActionEndTime = 0;

	m_dwChargeEffect = 0;
	m_dwChargeMaxEffect = 0;
	m_dwUpDownEffect = 0;

	m_dwCurSkillProtectTime = 0;

	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetChargeState( pChar, true );
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

void ioMechanicsFireSkill::SetChargeState( ioBaseChar *pChar, bool bFirst )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_ChargeMotion, FLOAT100, FLOAT1, FLOAT1 );

	if( m_dwChargeEffect > 0 )
	{
		pChar->EndEffect( m_dwChargeEffect, false );
		m_dwChargeEffect = 0;
	}

	if( m_dwChargeMaxEffect > 0 )
	{
		pChar->EndEffect( m_dwChargeMaxEffect, false );
		m_dwChargeMaxEffect = 0;
	}

	if( !m_ChargeEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_ChargeEffect );
		if( pEffect )
			m_dwChargeEffect = pEffect->GetUniqueID();
	}

	m_bSetMaxCharge = false;

	m_SkillState = SS_CHARGE;
	m_dwChargeMaxTime = dwCurTime + m_dwChargeDuration;
	m_dwFireEnableTime = m_dwChargeMaxTime + m_dwFireEnableDuration;

	// 사용 아이템 처리
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

	if( !bFirst && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_ACTION;
		kPacket << m_SkillState;
		kPacket << m_iCurFireCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMechanicsFireSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurFireCnt,
										iAniID,
										GetAttribute().m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate + dwPreDelay;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_SkillState = SS_ACTION;
	m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_ACTION;
		kPacket << m_SkillState;
		kPacket << m_iCurFireCnt;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurFireCnt++;
}

void ioMechanicsFireSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;

			SetChargeState( pChar, true );
			EndPreDelayEffect( pChar );

			if( m_dwSkillProtectTime > 0 )
				m_dwCurSkillProtectTime = dwCurTime + m_dwSkillProtectTime;
		}
		break;
	case SS_CHARGE:
		if( !m_bSetMaxCharge && m_dwChargeMaxTime > 0 && m_dwChargeMaxTime < dwCurTime )
		{
			m_bSetMaxCharge = true;
			if( m_dwChargeEffect > 0 )
			{
				pChar->EndEffect( m_dwChargeEffect, false );
				m_dwChargeEffect = 0;
			}

			if( m_dwChargeMaxEffect == 0 && !m_ChargeMaxEffect.IsEmpty() )
			{
				ioEffect *pEffect = pChar->AttachEffect( m_ChargeMaxEffect );
				if( pEffect )
					m_dwChargeMaxEffect = pEffect->GetUniqueID();
			}
		}
		else if( COMPARE(dwCurTime, m_dwChargeMaxTime, m_dwFireEnableTime) )
		{
			// attack key Check
			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				AttackSkillFire( pChar );
				return;
			}
		}
		else if( m_dwFireEnableTime > 0 && m_dwFireEnableTime < dwCurTime )
		{
			AttackSkillFire( pChar );
			return;
		}

		// check key
		CheckKeyInput( pChar );
		ProcessRotate( pChar );
		ProcessMove( pChar );
		break;
	case SS_ACTION:
		if( m_dwActionEndTime < dwCurTime )
		{
			if( m_iCurFireCnt < m_iMaxFireCnt )
				SetChargeState( pChar, false );
			else
				SetEndState( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioMechanicsFireSkill::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh && pChar )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() && pChar )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	EndPreDelayEffect( pChar );
	if( pChar )
		pChar->EndEffect( m_SkillEffectName );

	if( m_dwSkillMapEffect != -1 )
	{
		if( pChar )
			pChar->EndMapEffect( m_dwSkillMapEffect );
		m_dwSkillMapEffect = -1;
	}

	//
	m_bReduceGauge = false;

	if( pChar )
	{
		pChar->EndEffect( m_ChargeEffect, false );
		pChar->EndEffect( m_ChargeMaxEffect, false );
		pChar->EndEffect( m_ChargeUpEffect, false );
		pChar->EndEffect( m_ChargeDownEffect, false );

		m_dwChargeEffect = 0;
		m_dwChargeMaxEffect = 0;
		m_dwUpDownEffect = 0;

		pChar->SetSkillMoveEnable( false, false );
		pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
		pChar->RestoreSkillProtection();
	}

	m_SkillUseType = SUT_NORMAL;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	if( !m_SkillEndEffectName.IsEmpty() && pChar )
		pChar->AttachEffect( m_SkillEndEffectName );
}

void ioMechanicsFireSkill::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
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

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
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
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

void ioMechanicsFireSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SST_ACTION:
		ApplyActionState( pChar, rkPacket );
		break;
	case SST_KEY:
		{
			bool bChangeDir, bChangeUpDown;
			rkPacket >> bChangeDir >> bChangeUpDown;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			int iDir;
			rkPacket >> iDir;

			bool bSetUpKey, bSetDownKey;
			rkPacket >> bSetUpKey >> bSetDownKey;

			pChar->SetTargetRotToRotate( qtRot, true );

			if( bChangeDir )
			{
				m_bTargetRot = false;
				m_PreDirKey = m_CurDirKey;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}

			if( bChangeUpDown )
			{
				m_bSetUpKey = bSetUpKey;
				m_bSetDownKey = bSetDownKey;

				CheckUpDownEffect( pChar );
			}
		}
		break;
	}
}

void ioMechanicsFireSkill::ApplyActionState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState, iCurFireCnt;
	rkPacket >> iState;
	rkPacket >> iCurFireCnt;

	switch( iState )
	{
	case SS_CHARGE:
		m_iCurFireCnt = iCurFireCnt;
		SetChargeState( pOwner, false );
		break;
	case SS_ACTION:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			if( m_iCurFireCnt == iCurFireCnt )
			{
				pOwner->SetTargetRotToRotate( qtRot, true );

				AttackSkillFire( pOwner );
			}
		}
		break;
	}
}

void ioMechanicsFireSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( pOwner )
	{
		D3DXVECTOR3 vAttDir =  pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		pWeapon->SetMoveDir( vAttDir );
	}
}

bool ioMechanicsFireSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMechanicsFireSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_bTargetRot = false;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
	}

	// Check UpDown
	if( m_bSetUpKey )
	{
		if( !pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;

			if( pOwner->IsDefenseKeyDown() )
				m_bSetDownKey = true;
		}
	}
	else if( m_bSetDownKey )
	{
		if( !pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = false;

			if( pOwner->IsJumpKeyDown() )
				m_bSetUpKey = true;
		}
	}
	else
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = true;
		}

		if( pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = true;
			m_bSetDownKey = false;
		}
	}

	if( bChangeUpDown )
	{
		CheckUpDownEffect( pOwner );
	}

	if( bChangeDir || bChangeUpDown )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SST_KEY;
			kPacket << bChangeDir;
			kPacket << bChangeUpDown;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMechanicsFireSkill::ProcessMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );

	// UpDown Move
	if( m_bSetUpKey || m_bSetDownKey )
	{
		float fCurHeightAmt = 0.0f;
		float fCurUpDownSpeed = m_fUpDownSpeed * fTimePerSec;

		if( m_bSetUpKey )
			fCurHeightAmt += fCurUpDownSpeed;
		else if( m_bSetDownKey )
			fCurHeightAmt -= fCurUpDownSpeed;

		vCurPos.y += fCurHeightAmt;
	}

	vCurPos.y = max( vCurPos.y, fMapHeight );

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	return;
}

void ioMechanicsFireSkill::SetEndState( ioBaseChar *pChar )
{
	m_SkillState = SS_END;
}

bool ioMechanicsFireSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioMechanicsFireSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_CHARGE:
	case SS_ACTION:
		return false;
	case SS_END:
		return true;
	}

	return true;
}

void ioMechanicsFireSkill::CheckUpDownEffect( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_dwUpDownEffect > 0 )
	{
		pOwner->EndEffect( m_dwUpDownEffect, false );
		m_dwUpDownEffect = 0;
	}

	ioEffect *pEffect = NULL;
	if( m_bSetUpKey && !m_ChargeUpEffect.IsEmpty() )
		pEffect = pOwner->AttachEffect( m_ChargeUpEffect );
	else if( m_bSetDownKey && !m_ChargeDownEffect.IsEmpty() )
		pEffect = pOwner->AttachEffect( m_ChargeDownEffect );

	if( pEffect )
		m_dwUpDownEffect = pEffect->GetUniqueID();
}

bool ioMechanicsFireSkill::IsNoDropState() const
{
	return true;
}

bool ioMechanicsFireSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_CHARGE:
	case SS_ACTION:
		return false;
	}

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}



bool ioMechanicsFireSkill::IsJumpSkill()
{
	return m_bJumpSkill;
}