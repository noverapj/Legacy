

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

#include "ioRotateConsecutionSkill.h"

ioRotateConsecutionSkill::ioRotateConsecutionSkill()
{
	m_iWeaponFireCnt = 0;

	m_bMotionSetted = false;
	m_bFirstMoveMotion = true;

	m_bSetHandMesh = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bTargetRot = false;
	m_bLeftRot = false;
}

ioRotateConsecutionSkill::ioRotateConsecutionSkill( const ioRotateConsecutionSkill &rhs )
: ioAttackSkill( rhs ),
 m_WeaponInfo( rhs.m_WeaponInfo ),
 m_vWeaponFireInfoList( rhs.m_vWeaponFireInfoList ),
 m_fSkillMotionRate( rhs.m_fSkillMotionRate ),
 m_SkillMotionLt( rhs.m_SkillMotionLt ),
 m_SkillMotionRt( rhs.m_SkillMotionRt ),
 m_dwRotateTime( rhs.m_dwRotateTime ),
 m_MoveType( rhs.m_MoveType ),
 m_fMoveSpeed( rhs.m_fMoveSpeed )
{
	m_iWeaponFireCnt = 0;

	m_bMotionSetted = false;
	m_bFirstMoveMotion = true;

	m_bSetHandMesh = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bTargetRot = false;
	m_bLeftRot = false;
}

ioRotateConsecutionSkill::~ioRotateConsecutionSkill()
{
}

void ioRotateConsecutionSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "skill_motion_lt", "", szBuf, MAX_PATH );
	m_SkillMotionLt = szBuf;
	rkLoader.LoadString_e( "skill_motion_rt", "", szBuf, MAX_PATH );
	m_SkillMotionRt = szBuf;

	m_fSkillMotionRate = rkLoader.LoadFloat_e( "skill_motion_rate", FLOAT1 );
	m_dwRotateTime = rkLoader.LoadInt_e( "skill_rotate_speed", 0 );

	int iFireCnt = rkLoader.LoadInt_e( "weapon_fire_cnt", 0 );
	m_vWeaponFireInfoList.clear();
	m_vWeaponFireInfoList.reserve( iFireCnt );

	for( int i=0; i < iFireCnt; ++i )
	{
		WeaponFireInfo kFireInfo;
		DWORD dwFireTime = 0;
		float fFireAngle = 0.0f;

		wsprintf_e( szBuf, "weapon_fire_time%d", i+1 );
		dwFireTime = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "weapon_fire_angle%d", i+1 );
		fFireAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		kFireInfo.m_dwWeaponFireTime = dwFireTime;
		kFireInfo.m_fWeaponFireAngle = fFireAngle;

		wsprintf_e( szKey, "weapon_wounded_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kFireInfo.m_WeaponInfo.m_WoundedAnimation = szBuf;
		wsprintf_e( szKey, "weapon_wounded_duration%d", i+1 );
		kFireInfo.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "weapon_wounded_loop_ani%d", i+1 );
		kFireInfo.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "weapon_attribute_index%d", i+1 );
		kFireInfo.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "weapon_attribute_resist_index%d", i+1 );
		kFireInfo.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		m_vWeaponFireInfoList.push_back( kFireInfo );
	}

	// move
	m_MoveType = (MoveType)rkLoader.LoadInt_e( "enable_move", EMT_NONE );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
}

ioSkill* ioRotateConsecutionSkill::Clone()
{
	return new ioRotateConsecutionSkill( *this );
}

void ioRotateConsecutionSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->AttachEffect( m_SkillEffectName );

	if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect );
		m_dwSkillMapEffect = -1;
	}

	ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_SkillMapEffectName, pChar->GetMoveDir() );
	if( pMapEffect )
	{
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, 0 );
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	if( pChar )
	{
		m_iWeaponFireCnt = 0;
		m_bMotionSetted = true;
		m_bFirstMoveMotion = true;

		// 사용 아이템 처리
		if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
		{
			pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
			m_bSetHandMesh = true;
		}
	}
}

bool ioRotateConsecutionSkill::OnSkillStart( ioBaseChar *pChar )
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

	m_dwSkillMapEffect = -1;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

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

	// 기본 무기 손에서 제거
	m_iWeaponFireCnt = 0;
	m_bMotionSetted = false;
	m_bFirstMoveMotion = true;

	m_bSetHandMesh = false;
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	switch( m_MoveType )
	{
	case EMT_REVERSE:
		{
			D3DXQUATERNION qtAngle;
			D3DXVECTOR3 vMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			ioMath::CalcDirectionQuaternion( qtAngle, vMoveDir );
			pChar->SetMoveDirByRotate( qtAngle );
		}
		break;
	case EMT_FRONT:
		{
			pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
		}
		break;
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	m_iWeaponFireCnt = 0;

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

void ioRotateConsecutionSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwSkillMapEffect != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
			if( pMapEffect )
			{
				D3DXVECTOR3 vPos = pChar->GetWorldPosition();
				pMapEffect->SetWorldPosition( vPos );
			}
		}

		CheckKeyInput( pChar );

		ProcessRotate( pChar );
		ProcessAttack( pChar );

		if( m_MoveType != EMT_NONE )
			pChar->SetCurMoveSpeed( m_fMoveSpeed );

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

void ioRotateConsecutionSkill::OnSkillEnd( ioBaseChar *pChar )
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
		pChar->EndMapEffect( m_dwSkillMapEffect );
		m_dwSkillMapEffect = -1;
	}

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCurMoveSpeed( 0.0f );

	m_SkillUseType = SUT_NORMAL;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );
}

void ioRotateConsecutionSkill::ProcessAttack( ioBaseChar *pOwner )
{
	int iFireTimeCnt = m_vWeaponFireInfoList.size();
	if( iFireTimeCnt <= 0 )
		return;

	if( !pOwner || !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( !COMPARE( m_iWeaponFireCnt, 0, iFireTimeCnt) )
		return;

	DWORD dwFireTime = m_vWeaponFireInfoList[m_iWeaponFireCnt].m_dwWeaponFireTime;
	if( dwFireTime == 0 )
		return;

	if( dwFireTime > 0 )
		dwFireTime += m_dwMotionStartTime;

	if( dwCurTime > dwFireTime )
	{
		float fAngle = m_vWeaponFireInfoList[m_iWeaponFireCnt].m_fWeaponFireAngle;
		CreateWeapon( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SNT_ATTACK;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioRotateConsecutionSkill::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;

		switch( m_MoveType )
		{
		case EMT_NONE:
			{
				pOwner->SetTargetRotToRotate( qtRotate, true, false );

				int iAniID = 0;
				ioEntityGroup *pGrp = pOwner->GetGroup();
				iAniID = pGrp->GetAnimationIdx(m_SkillMotionLt);
				pGrp->ClearLoopAni( iAniID, FLOAT100, true );
				iAniID = pGrp->GetAnimationIdx(m_SkillMotionRt);
				pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			}
			break;
		case EMT_REVERSE:
			{
				pOwner->SetTargetRotToRotate( qtRotate, true, true );

				D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				ioMath::CalcDirectionQuaternion( qtRotate, vMoveDir );

				pOwner->SetMoveDirByRotate(qtRotate);
			}
			break;
		case EMT_FRONT:
			{
				pOwner->SetTargetRotToRotate( qtRotate, true, false );
			}
			break;
		}

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

	switch( m_MoveType )
	{
	case EMT_REVERSE:
		{
			pOwner->SetTargetRotToRotate( qtAngle, true, true );

			D3DXVECTOR3 vMoveDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			ioMath::CalcDirectionQuaternion( qtAngle, vMoveDir );

			pOwner->SetMoveDirByRotate( qtAngle );
		}
		break;
	case EMT_NONE:
	case EMT_FRONT:
		{
			pOwner->SetTargetRotToRotate( qtAngle, true, false );
		}
		break;
	}
}

void ioRotateConsecutionSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SNT_ATTACK:
		{
			CreateWeapon( pChar );
		}
		break;
	case SNT_DIRCHANGE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			DWORD dwCurTime = FRAMEGETTIME();
			ioEntityGroup *pGrp = pChar->GetGroup();
			int iAniID = 0;

			if( iDir == ioUserKeyInput::DKI_NONE )
			{
				if( m_MoveType == EMT_NONE )
				{
					iAniID = pGrp->GetAnimationIdx(m_SkillMotionLt);
					pGrp->ClearLoopAni( iAniID, FLOAT100, true );
					iAniID = pGrp->GetAnimationIdx(m_SkillMotionRt);
					pGrp->ClearLoopAni( iAniID, FLOAT100, true );
				}

				m_bMotionSetted = true;
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			}
			else
			{
				m_bTargetRot = false;
				m_bMotionSetted = false;
				m_bFirstMoveMotion = false;

				float fRate = m_fSkillMotionRate;

				if( m_MoveType == EMT_NONE )
				{
					iAniID = pGrp->GetAnimationIdx(m_SkillMotionLt);
					pGrp->ClearLoopAni( iAniID, FLOAT100 );
					iAniID = pGrp->GetAnimationIdx(m_SkillMotionRt);
					pGrp->ClearLoopAni( iAniID, FLOAT100 );

					if( m_bLeftRot )
						pGrp->SetLoopAni( m_SkillMotionLt, FLOAT100, FLOAT1, FLOAT1/fRate );
					else
						pGrp->SetLoopAni( m_SkillMotionRt, FLOAT100, FLOAT1, FLOAT1/fRate );
				}

				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}
		}
		break;
	}
}

void ioRotateConsecutionSkill::CreateWeapon( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iFireTimeCnt = m_vWeaponFireInfoList.size();
	if( iFireTimeCnt <= 0 )
		return;

	if( !COMPARE( m_iWeaponFireCnt, 0, iFireTimeCnt) )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	DWORD dwWeaponIndex = m_dwWeaponBaseIndex + m_iWeaponFireCnt;

	pOwner->SkillFireExplicit( m_vWeaponFireInfoList[m_iWeaponFireCnt].m_WeaponInfo, GetName(), dwWeaponIndex, vCurPos );

	m_iWeaponFireCnt++;
}

void ioRotateConsecutionSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

bool ioRotateConsecutionSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioRotateConsecutionSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

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

			float fRate = m_fSkillMotionRate;

			if( m_MoveType == EMT_NONE )
			{
				iAniID = pGrp->GetAnimationIdx(m_SkillMotionLt);
				pGrp->ClearLoopAni( iAniID, FLOAT100 );
				iAniID = pGrp->GetAnimationIdx(m_SkillMotionRt);
				pGrp->ClearLoopAni( iAniID, FLOAT100 );

				if( m_bLeftRot )
					pGrp->SetLoopAni( m_SkillMotionLt, FLOAT100, FLOAT1, FLOAT1/fRate );
				else
					pGrp->SetLoopAni( m_SkillMotionRt, FLOAT100, FLOAT1, FLOAT1/fRate );
			}

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SNT_DIRCHANGE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( !m_bMotionSetted )
		{
			if( m_MoveType == EMT_NONE )
			{
				iAniID = pGrp->GetAnimationIdx(m_SkillMotionLt);
				pGrp->ClearLoopAni( iAniID, FLOAT100, true );
				iAniID = pGrp->GetAnimationIdx(m_SkillMotionRt);
				pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			}

			m_bMotionSetted = true;
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SNT_DIRCHANGE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}




