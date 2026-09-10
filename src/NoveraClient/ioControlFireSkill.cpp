

#include "stdafx.h"
#include "ioControlFireSkill.h"

ioControlFireSkill::ioControlFireSkill()
{
	m_bMotionSetted = false;
	m_bFirstMoveMotion = true;

	m_bSetHandMesh = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bTargetRot = false;
	m_bLeftRot = false;

	m_SkillState = SS_NONE;
	m_dwLoopStartTime = 0;
	m_fCurMotionRate = 0.0f;

	m_dwFireStartTime = 0;
	m_dwFireGuidEffect = -1;
	m_dwExtraGuidEffect = -1;

	m_bUp = m_bDown = m_bDir = false;
}

ioControlFireSkill::ioControlFireSkill( const ioControlFireSkill &rhs )
: ioAttackSkill( rhs ),
 m_dwRotateTime( rhs.m_dwRotateTime ),
 m_fLimitAngle( rhs.m_fLimitAngle ),
 m_fLimitVAngle( rhs.m_fLimitVAngle ),
 m_SkillUpMotion( rhs.m_SkillUpMotion ),
 m_SkillDownMotion( rhs.m_SkillDownMotion ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_SkillUpLoopMotion( rhs.m_SkillUpLoopMotion ),
 m_SkillDownLoopMotion( rhs.m_SkillDownLoopMotion ),
 m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_fXOffset( rhs.m_fXOffset ),
 m_fYOffset( rhs.m_fYOffset ),
 m_FireGuidEffect( rhs.m_FireGuidEffect ),
 m_dwFireGuidDuration( rhs.m_dwFireGuidDuration ),
 m_ExtraGuidEffect( rhs.m_ExtraGuidEffect ),
 m_bEnableTeamOnlyVisible( rhs.m_bEnableTeamOnlyVisible ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_CreateEffectState( rhs.m_CreateEffectState )
{
	m_bMotionSetted = false;
	m_bFirstMoveMotion = true;

	m_bSetHandMesh = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bTargetRot = false;
	m_bLeftRot = false;

	m_SkillState = SS_NONE;
	m_dwLoopStartTime = 0;
	m_fCurMotionRate = 0.0f;

	m_dwFireStartTime = 0;
	m_dwFireGuidEffect = -1;
	m_dwExtraGuidEffect = -1;

	m_dwCurSkillProtectTime = 0;

	m_bUp = m_bDown = m_bDir = false;
}

ioControlFireSkill::~ioControlFireSkill()
{
}

void ioControlFireSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "skill_up_motion", "", szBuf, MAX_PATH );
	m_SkillUpMotion = szBuf;

	rkLoader.LoadString_e( "skill_down_motion", "", szBuf, MAX_PATH );
	m_SkillDownMotion = szBuf;

	rkLoader.LoadString_e( "skill_up_loop_motion", "", szBuf, MAX_PATH );
	m_SkillUpLoopMotion = szBuf;

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	rkLoader.LoadString_e( "skill_down_loop_motion", "", szBuf, MAX_PATH );
	m_SkillDownLoopMotion = szBuf;

	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_dwRotateTime = rkLoader.LoadInt_e( "skill_rotate_speed", 0 );

	// Limit Angle
	m_fLimitAngle = rkLoader.LoadFloat_e( "limit_angle", 0.0f );
	m_fLimitVAngle = rkLoader.LoadFloat_e( "limit_vertical_angle", 0.0f );

	m_fXOffset = rkLoader.LoadFloat_e( "x_offset", 0.0f );
	m_fYOffset = rkLoader.LoadFloat_e( "y_offset", 0.0f );

	//effect
	m_CreateEffectState = (CreateEffectState)rkLoader.LoadInt_e( "effect_create", CES_NONE );

	rkLoader.LoadString_e( "fire_guid_effect", "", szBuf, MAX_PATH );
	m_FireGuidEffect = szBuf;

	rkLoader.LoadString_e( "extra_guid_effect", "", szBuf, MAX_PATH );
	m_ExtraGuidEffect = szBuf;

	m_dwFireGuidDuration = rkLoader.LoadInt_e( "fire_guid_effect_duration", 0 );
	m_bEnableTeamOnlyVisible = rkLoader.LoadBool_e( "enable_effect_only_team_visible", false );
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioControlFireSkill::Clone()
{
	return new ioControlFireSkill( *this );
}

bool ioControlFireSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

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

	if( 0 < m_dwSkillProtectTime )
		m_dwCurSkillProtectTime = m_dwSkillStartTime + m_dwSkillProtectTime;

	m_dwSkillMapEffect = -1;
	m_dwFireGuidEffect = -1;
	m_dwExtraGuidEffect = -1;

	m_fCurMotionRate = 0.0f;

	m_dwLoopStartTime = 0;
	m_dwFireStartTime = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bUp = m_bDown = m_bDir = false;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	m_bMotionSetted = false;
	m_bFirstMoveMotion = true;

	m_bSetHandMesh = false;
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_NONE;
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

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	if( m_CreateEffectState == CES_NONE )
		CreateEffect( pChar, pStage );

	// Limit Angle
	m_bUseLimitAngle = false;
	float fHalfAngle = m_fLimitAngle * FLOAT05;
	if( fHalfAngle > 0.0f )
	{
		D3DXQUATERNION qtAngle;
		m_qtLimitStartRot = pChar->GetTargetRot();
		D3DXVECTOR3 vDir = m_qtLimitStartRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
			
		// Right
		float fCurAngle = ioMath::ArrangeHead(fHalfAngle);
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		D3DXVECTOR3 vCurDir = qtAngle * vDir;
		ioMath::CalcDirectionQuaternion( qtAngle, -vCurDir );
		m_qtLimitRightRot = qtAngle;

		// Left
		fCurAngle = ioMath::ArrangeHead(-fHalfAngle);
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		vCurDir = qtAngle * vDir;
		ioMath::CalcDirectionQuaternion( qtAngle, -vCurDir );
		m_qtLimitLeftRot = qtAngle;

		m_bUseLimitAngle = true;
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioControlFireSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime != 0 && m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		if( m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
		{
			if( pChar->IsNeedProcess() )
				SetFireState( pChar, pStage );
			return;
		}
		else if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
		{
			SetFireState( pChar, pStage );
			return;
		}
		else
		{
			bool bNeedSend = false;
			bool bPreUp, bPreDown;
			bPreUp = m_bUp;
			bPreDown = m_bDown;

			CheckKeyInput( pChar );
			CheckHDir( pChar );
			CheckVDir( pChar );

			UpdateEffect( pChar, pStage );

			if( bPreUp != m_bUp || bPreDown != m_bDown || m_bDir )
			{
				bNeedSend = true;
			}

			if( bNeedSend && pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SNT_DIRCHANGE;
				kPacket << m_vAttackDir;
				kPacket << m_bUp;
				kPacket << m_bDown;
				kPacket << m_bDir;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		break;
	case SS_FIRE:
		if( m_dwFireStartTime < dwCurTime )
		{
			if( m_dwFireGuidEffect != -1 )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwFireGuidEffect );
				if( pMapEffect )
					pMapEffect->EndEffectForce();

				m_dwFireGuidEffect = -1;
			}
		}
		break;
	}
}

void ioControlFireSkill::CheckHDir( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	D3DXVECTOR3 vXZDir = m_vAttackDir;
	vXZDir.y = 0.0f;
	float fXZLength = D3DXVec3Length( &vXZDir );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	// limit Check
	float fStartYaw, fKeyYaw;
	fStartYaw = RADtoDEG( ioMath::QuaterToYaw( m_qtLimitStartRot ) );
	fKeyYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fStartYaw = ioMath::ArrangeHead( fStartYaw );
	fKeyYaw = ioMath::ArrangeHead( fKeyYaw );

	float fCheckAngle = ioMath::AngleGap( fStartYaw, fKeyYaw );
	fCheckAngle = ioMath::ArrangeHead( fCheckAngle );
	if( m_fLimitAngle > 0.0f && m_fLimitAngle * FLOAT05 < fCheckAngle )
	{
		qtRotate = m_qtLimitRightRot;
		if( m_bLeftRot )
			qtRotate = m_qtLimitLeftRot;
	}

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;

		pChar->SetTargetRotToRotate( qtRotate, true, true );
		vXZDir = pChar->GetMoveDir();
		vXZDir = fXZLength * vXZDir;
		vXZDir.y = m_vAttackDir.y;
		D3DXVec3Normalize( &m_vAttackDir, &vXZDir );

		int iAniID = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();

		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pChar->SetTargetRotToRotate( qtAngle, true, false );
	vXZDir = pChar->GetMoveDir();
	vXZDir = fXZLength * vXZDir;
	vXZDir.y = m_vAttackDir.y;
	D3DXVec3Normalize( &m_vAttackDir, &vXZDir );
}

void ioControlFireSkill::CheckVDir( ioBaseChar *pChar )
{
	if( m_dwRotateTime == 0 || m_fLimitVAngle == 0.f )
		return;

	D3DXVECTOR3 vYAxis = ioMath::UNIT_Y;
	if( m_vAttackDir == vYAxis || m_vAttackDir == -vYAxis )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	float fLimitHalf = m_fLimitVAngle * FLOAT05;
	float fNewCheckAngle =FLOAT90 - fLimitHalf;
	fNewCheckAngle = max( 0.0f, fNewCheckAngle );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vAttackDir, &vYAxis );

	float fDotValue1 = D3DXVec3Dot( &vYAxis, &m_vAttackDir );
	float fDotValue2 = D3DXVec3Dot( &(-vYAxis), &m_vAttackDir );
	float fGapAngle1 = RADtoDEG( acosf(fDotValue1) );
	float fGapAngle2 = RADtoDEG( acosf(fDotValue2) );

	bool bCheckUpLimit = false;
	if( m_fLimitVAngle > 0.0f && fNewCheckAngle > 0.0f && fNewCheckAngle >= fGapAngle1 )
		bCheckUpLimit = true;

	bool bCheckDownLimit = false;
	if( m_fLimitVAngle > 0.0f && fNewCheckAngle > 0.0f && fNewCheckAngle >= fGapAngle2 )
		bCheckDownLimit = true;

	if( m_bUp && !bCheckUpLimit ) // 위로
	{
		m_bUp = true;
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtRot;

		if( fGapAngle1 < fRotateAngle )
		{
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fGapAngle1) );
			m_vAttackDir = qtRot * m_vAttackDir;
		}
		else
		{
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fRotateAngle) );
			m_vAttackDir = qtRot * m_vAttackDir;
		}
	}
	else if( m_bDown && !bCheckDownLimit ) // 아래로
	{
		m_bDown = true;
		D3DXVec3Normalize( &vAxis, &-vAxis );

		D3DXQUATERNION qtRot;

		if( fGapAngle2 < fRotateAngle )
		{
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fGapAngle2) );
			m_vAttackDir = qtRot * m_vAttackDir;
		}
		else
		{
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fRotateAngle) );
			m_vAttackDir = qtRot * m_vAttackDir;
		}
	}

	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	// CheckMotion
	D3DXVECTOR3 vXZDir = m_vAttackDir;
	vXZDir.y = 0.0f;

	float fDotValue = D3DXVec3Dot( &vXZDir, &m_vAttackDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	m_fCurMotionRate = 0.0f;
	if( fLimitHalf > 0.0f )
		m_fCurMotionRate = fGapAngle / fLimitHalf;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;
	pUp = pCenter = pDown = NULL;

	pUp = pGrp->GetLoopInPlay( m_SkillUpLoopMotion );
	pCenter = pGrp->GetLoopInPlay( m_SkillLoopMotion );
	pDown = pGrp->GetLoopInPlay( m_SkillDownLoopMotion );

	if( !pUp || !pCenter || !pDown )
	{
		LOG.PrintTimeAndLog( 0, "ioControlFireSkill::CheckVDir - Motion Error" );
		return;
	}

	if( m_vAttackDir.y == 0.0f || m_fCurMotionRate == 0.0f )		// center
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	else if( m_vAttackDir.y > 0.0f )	// center & up
	{
		pUp->ForceWeight( m_fCurMotionRate );
		pCenter->ForceWeight( FLOAT1 - m_fCurMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	else								// center & down
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - m_fCurMotionRate );
		pDown->ForceWeight( m_fCurMotionRate );
	}
}

void ioControlFireSkill::CreateEffect(ioBaseChar *pChar, ioPlayStage *pStage )
{
	//Visible Check
	bool bVisible = false;
	if( m_bEnableTeamOnlyVisible )
	{
		ioBaseChar* pOwnerChar = pStage->GetOwnerChar();		
		if( pOwnerChar && pChar->GetTeam() == pOwnerChar->GetTeam() )
		{
			bVisible = true;
		}
	}
	else if( pChar->IsNeedProcess() )
	{
		bVisible = true;
	}
	
	// Map Effect
	ioMapEffect *pMapEffect = NULL;
	if( m_dwSkillMapEffect != -1 )
	{
		pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
		if( pMapEffect )
		{
			pMapEffect->EndEffectForce();
			pMapEffect->SetVisible( bVisible );
		}

		m_dwSkillMapEffect = -1;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos.y = pChar->GetMidPositionByRate( FLOAT09 ).y;
	vPos += m_fXOffset * m_vAttackDir;

	pMapEffect = pStage->CreateMapEffect( m_SkillMapEffectName, vPos, vScale );
	if( pMapEffect )
	{
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -m_vAttackDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
		pMapEffect->SetVisible( bVisible );
	}

	ioEffect *pEffect = pChar->AttachEffect( m_ExtraGuidEffect );
	
	if( pEffect )
	{
		pEffect->SetVisible( bVisible );
		m_dwExtraGuidEffect = pEffect->GetUniqueID();		
	}
}

void ioControlFireSkill::UpdateEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwSkillMapEffect == -1 )
		return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos.y = pChar->GetMidPositionByRate( FLOAT09 ).y;
	vPos += m_fXOffset * m_vAttackDir;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vPos );

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -m_vAttackDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioControlFireSkill::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	EndPreDelayEffect( pChar );
	pChar->EndEffect( m_SkillEffectName );

	if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect, false );
		m_dwSkillMapEffect = -1;
	}

	if( m_dwExtraGuidEffect != -1 )
	{
		pChar->EndEffect( m_dwExtraGuidEffect, false );
		m_dwExtraGuidEffect = -1;
	}

	if( m_dwFireGuidEffect != -1 )
	{
		pChar->EndMapEffect( m_dwFireGuidEffect, false );
		m_dwFireGuidEffect = -1;
	}

	EndCameraBuff( pChar );

	m_bUseLimitAngle = false;
	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCurMoveSpeed( 0.0f );

	m_SkillState = SS_NONE;
	m_SkillUseType = SUT_NORMAL;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );
}

void ioControlFireSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	EndPreDelayEffect( pChar );

	m_SkillState = SS_LOOP;
	m_fCurMotionRate = 0.0f;

	if( m_CreateEffectState == CES_LOOP )
		CreateEffect( pChar, pStage );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetLoopAni( m_SkillUpLoopMotion, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_SkillDownLoopMotion, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_SkillLoopMotion, 0.0f, FLOAT1 );
}

void ioControlFireSkill::SetFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	AttackSkillFire( pChar );

	m_SkillState = SS_FIRE;

	if(  m_bEnableTeamOnlyVisible )
	{
		if( m_dwSkillMapEffect != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
			if( pMapEffect )
				pMapEffect->SetVisible( true );
		}

		if( m_dwExtraGuidEffect != -1 )
		{			
			ioEffect* pEffect = pChar->GetEffectByObjID( m_dwExtraGuidEffect );
			if( pEffect )
				pEffect->SetVisible( true );
		}

		if( m_dwFireGuidEffect != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwFireGuidEffect );
			if( pMapEffect )
				pMapEffect->SetVisible( true );			
		}
	}

	if( m_CreateEffectState == CES_FIRE )
		CreateEffect( pChar, pStage );

	if( m_dwSkillMapEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwSkillMapEffect = -1;
	}

	if( m_dwExtraGuidEffect != -1 )
	{
		pChar->EndEffect( m_dwExtraGuidEffect, false );
		m_dwExtraGuidEffect = -1;
	}

	if( m_dwFireGuidEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwFireGuidEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwFireGuidEffect = -1;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos.y = pChar->GetMidPositionByRate( FLOAT09 ).y;
	vPos += m_fXOffset * m_vAttackDir;

	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_FireGuidEffect, vPos, vScale );
	if( pMapEffect )
	{
		m_dwFireGuidEffect = pMapEffect->GetUniqueID();

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -m_vAttackDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SNT_ATTACK;
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioControlFireSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			GetAttribute().m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwFireStartTime = 0;
	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	SetExtraAniJump( pChar, iAniID, fTimeRate );

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

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = 0;
	DWORD dwReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwReserveTime > 0 )
		m_dwEnableReserveTime = dwCurTime + dwReserveTime;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( m_vAttackDir.y == 0.0f || m_fCurMotionRate == 0.0f )		// center
	{
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else if( m_vAttackDir.y > 0.0f )	// center & up
	{
		pGrp->SetActionAni( m_SkillUpMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else								// center & down
	{
		pGrp->SetActionAni( m_SkillDownMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

bool ioControlFireSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioControlFireSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	// Up, Down은 해당키 눌러져 있으면 true, 아니면 false
	// Dir은 변화가 있으면 true, 없으면 false
	m_bUp = m_bDown = m_bDir = false;

	// Up, Down Check
	if( pOwner->IsJumpKeyDown() )
		m_bUp = true;
	else if( pOwner->IsDefenseKeyDown() )
		m_bDown = true;

	// Horizen Check
	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			m_bTargetRot = false;
			m_bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}
			m_bMotionSetted = false;
			m_bFirstMoveMotion = false;

			m_CurDirKey = eNewDirKey;

			m_bDir = true;
		}
	}
	else
	{
		if( !m_bMotionSetted )
		{
			m_bMotionSetted = true;
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			m_bDir = false;
		}
	}
}

bool ioControlFireSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_FIRE:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;

		return true;
	}

	return true;
}

bool ioControlFireSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_FIRE:
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;

		break;
	}

	return false;
}

void ioControlFireSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_vAttackDir;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	switch( iState )
	{
	case SNT_DIRCHANGE:
		{
			rkPacket >> m_bUp;
			rkPacket >> m_bDown;

			bool bDir;
			rkPacket >> bDir;

			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			if( bDir )
			{
				if( iDir == ioUserKeyInput::DKI_NONE )
				{
					m_bMotionSetted = true;
					m_CurDirKey = ioUserKeyInput::DKI_NONE;
				}
				else
				{
					m_bTargetRot = false;
					m_bMotionSetted = false;
					m_bFirstMoveMotion = false;

					switch( iDir )
					{
					case ioUserKeyInput::DKI_UP:
						m_CurDirKey = ioUserKeyInput::DKI_UP;
						break;
					case ioUserKeyInput::DKI_RIGHTUP:
						m_CurDirKey = ioUserKeyInput::DKI_RIGHTUP;
						break;
					case ioUserKeyInput::DKI_RIGHT:
						m_CurDirKey = ioUserKeyInput::DKI_RIGHT;
						break;
					case ioUserKeyInput::DKI_RIGHTDOWN:
						m_CurDirKey = ioUserKeyInput::DKI_RIGHTDOWN;
						break;
					case ioUserKeyInput::DKI_DOWN:
						m_CurDirKey = ioUserKeyInput::DKI_DOWN;
						break;
					case ioUserKeyInput::DKI_LEFTDOWN:
						m_CurDirKey = ioUserKeyInput::DKI_LEFTDOWN;
						break;
					case ioUserKeyInput::DKI_LEFT:
						m_CurDirKey = ioUserKeyInput::DKI_LEFT;
						break;
					case ioUserKeyInput::DKI_LEFTUP:
						m_CurDirKey = ioUserKeyInput::DKI_LEFTUP;
						break;
					default:
						m_CurDirKey = ioUserKeyInput::DKI_NONE;
						break;
					}
				}
			}
		}
		break;
	case SNT_ATTACK:
		SetFireState( pChar, pStage );
		break;
	}
}

void ioControlFireSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	pWeapon->SetMoveDir( m_vAttackDir );
}

bool ioControlFireSkill::IsProtected( int iDefenseBreakType ) const
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
	case SS_LOOP:
	case SS_FIRE:
		if( 0 < m_dwCurSkillProtectTime && dwCurTime < m_dwCurSkillProtectTime )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}


