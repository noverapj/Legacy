

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioRunningAttackSkill2.h"

#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"


ioRunningAttackSkill2::ioRunningAttackSkill2()
{
	ClearData();

	m_bSetHandMesh = false;
}

ioRunningAttackSkill2::ioRunningAttackSkill2( const ioRunningAttackSkill2 &rhs )
: ioAttackSkill( rhs ),
 m_dwLoopTime( rhs.m_dwLoopTime ),
 m_bUseFireEnd( rhs.m_bUseFireEnd ),
 m_bDirection( rhs.m_bDirection ),
 m_ActionBuffList( rhs.m_ActionBuffList ),
 m_RunningReadyAni( rhs.m_RunningReadyAni ),
 m_RunningEndAni( rhs.m_RunningEndAni ),
 m_fRunningReadyAniRate( rhs.m_fRunningReadyAniRate ),
 m_fRunningEndAniRate( rhs.m_fRunningEndAniRate ),
 m_fRunSpeed( rhs.m_fRunSpeed ),
 m_dwRotateTime( rhs.m_dwRotateTime ),
 m_dwRunEndEnableTime( rhs.m_dwRunEndEnableTime ),
 m_bDisableRunColSkip( rhs.m_bDisableRunColSkip )
{
	ClearData();

	m_bSetHandMesh = false;
}

ioRunningAttackSkill2::~ioRunningAttackSkill2()
{
}

void ioRunningAttackSkill2::ClearData()
{
	m_RunningState = RST_NONE;

	m_dwRunningStartTime = 0;
	m_dwRunningReadyEndTime = 0;
	m_dwRunningEndTime = 0;
	m_dwRunningEndEndTime = 0;

	m_dwFireDuration = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void ioRunningAttackSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwLoopTime = rkLoader.LoadInt_e( "run_duration", 0 );
	m_bUseFireEnd	 = rkLoader.LoadBool_e( "use_fire_end", false );
	m_bDirection = rkLoader.LoadBool_e( "direction", false );

	rkLoader.LoadString_e( "running_ready_ani", "", szBuf, MAX_PATH );
	m_RunningReadyAni = szBuf;
	m_fRunningReadyAniRate = rkLoader.LoadFloat_e( "running_ready_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "running_end_ani", "", szBuf, MAX_PATH );
	m_RunningEndAni = szBuf;
	m_fRunningEndAniRate = rkLoader.LoadFloat_e( "running_end_ani_rate", FLOAT1 );

	m_dwRunEndEnableTime = (DWORD)rkLoader.LoadInt_e( "run_end_enable_time", 0 );

	m_fRunSpeed = rkLoader.LoadFloat_e( "run_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "run_rotate_speed", 0 );

	m_bDisableRunColSkip = rkLoader.LoadBool_e( "disable_run_col_skip", false );

	// buff
	m_ActionBuffList.clear();
	int iCnt = rkLoader.LoadInt_e( "action_buff_list", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "action_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ActionBuffList.push_back( szBuf );
	}
}

ioSkill* ioRunningAttackSkill2::Clone()
{
	return new ioRunningAttackSkill2( *this );
}

bool ioRunningAttackSkill2::IsProtected( int iDefenseBreakType ) const
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
	if( m_RunningState == RST_READY )
	{
		// firemotion_e가 없었을 경우
		if( m_dwFireMotionEndTime == 0 )
			return true;

		// firemotion_e가 설정된 경우
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
	}

	if( m_RunningState == RST_RUN && m_dwFireMotionEndTime > dwCurTime )
		return true;

	if( m_RunningState == RST_END )
		return false;

	return false;
}

bool ioRunningAttackSkill2::IsEndState() const
{
	if( m_RunningState == RST_END )
		return true;

	return false;
}

bool ioRunningAttackSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwRunningEndEndTime == 0 )
		return false;

	if( m_dwRunningEndEndTime + dwActionStop < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioRunningAttackSkill2::OnSkillStart( ioBaseChar *pChar )
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

	ClearData();

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

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetReadyState( pChar );
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

void ioRunningAttackSkill2::SetReadyState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_RunningReadyAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fTimeRate = FLOAT1;
	if( m_fRunningReadyAniRate > 0.0f )
		fTimeRate = m_fRunningReadyAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningReadyEndTime = dwCurTime;
	m_dwRunningReadyEndTime += (pGrp->GetAnimationFullTime(iAniID) * fTimeRate);

	m_dwFireMotionEndTime = 0;
	float fMotionEndTime = pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;
	if( fMotionEndTime > 0.0f )
	{
		m_dwFireMotionEndTime = dwCurTime;
		m_dwFireMotionEndTime += fMotionEndTime;
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	m_RunningState = RST_READY;

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioRunningAttackSkill2::SetRunState( ioBaseChar *pChar )
{
	if( m_SkillUseType != SUT_NORMAL )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	if( pChar->IsNeedProcess() && !m_ActionBuffList.empty() )
	{
		pChar->SetRandomSeed( 0 );

		bool bResult = false;
		ioHashStringVec vBuffList;

		ioHashString szItemName;
		ioItem *pItem = GetOwnerItem();
		if( pItem )
		{
			szItemName = pItem->GetName();
		}

		float fValue = 0.0f;
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
		if( pUpInfo )
		{
			ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
			fValue = pUpInfo->GetValue(pStage);
		}

		int iCnt = m_ActionBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_ActionBuffList[i];

			if( pChar->IsHasCrown() && m_bDisableKingSkill )
				continue;
			else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
				continue;

			if( !szBuffName.IsEmpty() )
			{
				ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pChar->GetCharName(), szItemName, this, 0 );
				if( pBuff )
				{
					pBuff->SetExtraDurationByGrowth( fValue );

					bResult = true;
					vBuffList.push_back( szBuffName );
				}
			}
		}

		if( bResult )
		{
			int iCurBuffCnt = vBuffList.size();
			if( pChar->IsNeedSendNetwork() )
			{
				if( Help::CheckEnableTimeGapUser() )
				{
					LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff4 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
				}
				SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
				kPacket << pChar->GetCharName();
				kPacket << pChar->GetCharName();
				kPacket << szItemName;
				kPacket << pChar->GetRandomSeed();
				kPacket << false;					// 타격횟수 체크
				kPacket << 0.0f;
				kPacket << fValue;
				kPacket << 0.0f;
				kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
				kPacket << iCurBuffCnt;

				for( i=0; i < iCurBuffCnt; i++ )
				{
					kPacket << vBuffList[i];
				}

				kPacket << false;					// Use Force

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	AttackSkillFire( pChar );

	m_dwRunningStartTime = FRAMEGETTIME();
}

void ioRunningAttackSkill2::AttackSkillFire( ioBaseChar *pChar )
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
	m_dwRunningEndTime = m_dwMotionEndTime;

	if( !m_bDisableRunColSkip )
		pChar->SetCharColSkipSkillTime( dwCurTime, m_dwMotionEndTime );

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	if( !m_bUseFireEnd )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	DWORD dwFireTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireDuration = m_dwLoopTime - dwFireTime;


	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, 0 );

	m_RunningState = RST_RUN;
}

void ioRunningAttackSkill2::SetRunEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	// clear buff
	int iBuffCnt = m_ActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->RemoveBuff( m_ActionBuffList[i] );
	}

	g_WeaponMgr.DestroyReserveContactWeapon( pChar->GetCharName() );
	g_WeaponMgr.DestroyReserveZoneWeapon( pChar->GetCharName() );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_RunningEndAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fTimeRate = FLOAT1;
	if( m_fRunningEndAniRate > 0.0f )
		fTimeRate = m_fRunningEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningEndEndTime = dwCurTime;
	m_dwRunningEndEndTime += (pGrp->GetAnimationFullTime(iAniID) * fTimeRate);

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->SetCharColSkipSkillTime( 0, 0 );

	m_RunningState = RST_END;
}

void ioRunningAttackSkill2::SetChangeState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	// clear buff
	int iBuffCnt = m_ActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->RemoveBuff( m_ActionBuffList[i] );
	}

	g_WeaponMgr.DestroyReserveContactWeapon( pChar->GetCharName() );
	g_WeaponMgr.DestroyReserveZoneWeapon( pChar->GetCharName() );

	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->SetCharColSkipSkillTime( 0, 0 );
}

void ioRunningAttackSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_RunningState )
	{
	case RST_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				if( m_SkillUseType == SUT_NORMAL )
					SetReadyState( pChar );
				else
					SetRunState( pChar );
			}
		}
		break;
	case RST_READY:
		{
			// 예외처리 : 스킬이 시작한 직후와 끝나기 직전 일정 시간동안 입력을 받지 않는다.
			//            초근거리에서 스킬이 발동하는 걸 막기 위함.

			// ...

			if( m_dwRunningReadyEndTime < dwCurTime )
			{
				m_dwRunningReadyEndTime = 0;
				SetRunState( pChar );
				return;
			}
		}
		break;
	case RST_RUN:
		{
			if( m_dwRunningEndTime < dwCurTime )
			{
				m_dwRunningEndTime = 0;
				SetRunEndState( pChar );
				return;
			}
			else
			{
				// RunningStop
				if( pChar->IsNeedProcess() && (m_dwRunningStartTime+m_dwRunEndEnableTime < dwCurTime) )
				{
					bool bChangeState = false;

					if( pChar->IsAttackKey() )
					{
						m_RunningState = RST_ATTACK_KEY;
						bChangeState = true;
					}
					else if( pChar->IsDefenseKey() )
					{
						m_RunningState = RST_DEFENSE_KEY;
						bChangeState = true;
					}
					else if( pChar->IsJumpKey() )
					{
						m_RunningState = RST_JUMP_KEY;
						bChangeState = true;
					}

					if( bChangeState )
					{
						m_dwRunningEndTime = 0;
						SetChangeState( pChar );

						if( pChar->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
							kPacket << pChar->GetCharName();
							kPacket << GetName();
							kPacket << SST_RUN;
							kPacket << (int)m_RunningState;
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
						return;
					}
				}

				CheckKeyInput( pChar );
				ProcessRotate( pChar );

				pChar->SetCurMoveSpeed( m_fRunSpeed );
			}
		}
		break;
	}
}

void ioRunningAttackSkill2::OnSkillEnd( ioBaseChar *pChar )
{
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

	// clear buff
	int iBuffCnt = m_ActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->RemoveBuff( m_ActionBuffList[i] );
	}

	RemoveExtraAttackForce();
	EndPreDelayEffect( pChar );

	pChar->EndEffect( m_SkillEffectName );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCharColSkipSkillTime( 0, 0 );

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );
}

void ioRunningAttackSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			if( pChar )
			{
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( true, true );
			}
		}
	}
	else if( !m_bUseFireEnd )
	{
		pWeapon->SetLiveTime( m_dwFireDuration );
	}
}

void ioRunningAttackSkill2::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

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
			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

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

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SST_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SST_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioRunningAttackSkill2::ProcessRotate( ioBaseChar *pOwner )
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
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

void ioRunningAttackSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	if( iType == SST_ROTATE )
	{
		int iDir;
		rkPacket >> iDir;
		rkPacket >> m_bLeftRot;

		DWORD dwCurTime = FRAMEGETTIME();
		int iAniID = 0;

		if( iDir == ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;
		}
		else
		{
			m_bTargetRot = false;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		}
	}
	else if( iType == SST_RUN )
	{
		int iStateType;
		rkPacket >> iStateType;

		switch( iStateType )
		{
		case RST_END:
			{
				m_dwRunningEndTime = 0;
				SetRunEndState( pChar );
			}
			break;
		case RST_ATTACK_KEY:
		case RST_DEFENSE_KEY:
		case RST_JUMP_KEY:
			{
				m_dwRunningEndTime = 0;
				m_RunningState = (RunningState)iStateType;
				SetChangeState( pChar );
			}
			break;
		}
	}
}
