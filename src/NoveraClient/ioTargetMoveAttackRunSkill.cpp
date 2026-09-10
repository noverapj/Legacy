

#include "stdafx.h"

#include "ioTargetMoveAttackRunSkill.h"
#include "WeaponDefine.h"

ioTargetMoveAttackRunSkill::ioTargetMoveAttackRunSkill()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
}

ioTargetMoveAttackRunSkill::ioTargetMoveAttackRunSkill( const ioTargetMoveAttackRunSkill &rhs )
: ioNormalSkill( rhs ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
 m_RunAttackAttribute( rhs.m_RunAttackAttribute ),
 m_szRunEffectName( rhs.m_szRunEffectName ),
 m_RunActionBuffList( rhs.m_RunActionBuffList ),
 m_dwRunDurationTime( rhs.m_dwRunDurationTime ),
 m_fRunningJumpAmt( rhs.m_fRunningJumpAmt ),
 m_fRunSpeed( rhs.m_fRunSpeed ),
 m_dwRotateTime( rhs.m_dwRotateTime ),
 m_RunningEndAni( rhs.m_RunningEndAni ),
 m_fRunningEndAniRate( rhs.m_fRunningEndAniRate ),
 m_fRunningEndSpeed( rhs.m_fRunningEndSpeed )
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
}

ioTargetMoveAttackRunSkill::~ioTargetMoveAttackRunSkill()
{
}

void ioTargetMoveAttackRunSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	LoadAttackAttribute( "land_attack", m_LandAttackAttribute, rkLoader );
	LoadAttackAttribute( "run_attack", m_RunAttackAttribute, rkLoader );

	m_RunActionBuffList.clear();
	int iCnt = rkLoader.LoadInt_e( "run_action_buff_list", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "run_action_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_RunActionBuffList.push_back( szBuf );
	}
	m_dwRunDurationTime = rkLoader.LoadInt_e( "run_duration", 0 );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "run_rotate_speed", 0 );
	m_fRunningJumpAmt = rkLoader.LoadFloat_e( "running_jump_amt", 0.0f );	
	m_fRunSpeed	= rkLoader.LoadFloat_e( "run_speed", 0.0f );
	rkLoader.LoadString_e( "run_effect", "", szBuf, MAX_PATH );
	m_szRunEffectName = szBuf;	

	rkLoader.LoadString_e( "running_end_ani", "", szBuf, MAX_PATH );
	m_RunningEndAni = szBuf;
	m_fRunningEndAniRate = rkLoader.LoadFloat_e( "running_end_ani_rate", FLOAT1 );
	m_fRunningEndSpeed = rkLoader.LoadFloat_e( "end_run_speed", m_fRunSpeed );
}

ioSkill* ioTargetMoveAttackRunSkill::Clone()
{
	return new ioTargetMoveAttackRunSkill( *this );
}

bool ioTargetMoveAttackRunSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_dwFireStartTime = 0;
	m_fCharMoveSpeed = 0.0f;
	m_fCurCharMoveSpeed = 0.0f;
	m_fFlightDuration = 0.0f;

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_State = SS_NONE;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetWaitState( pChar );
	}

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioTargetMoveAttackRunSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	int iBuffCnt = m_RunActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
		pChar->RemoveBuff( m_RunActionBuffList[i] );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetMagicCircleTarget( "" );

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwActionStartTime = 0;

	m_dwPreDelayStartTime = 0;

	m_State = SS_NONE;
}

void ioTargetMoveAttackRunSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			if( m_dwPreDelayStartTime == 0 )
				return;

			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				EndPreDelayEffect( pChar );
				SetWaitState( pChar );
			}
		}
		break;
	case SS_WAIT:
		if( m_dwFireStartTime < dwCurTime )
		{
			SetActionState( pChar, pStage );
		}
		break;
	case SS_ACTION:
		if( CheckLanding( pChar ) )
		{
			SetLandState( pChar );
		}
		else
		{
			CheckControl( pChar );
			ProcessControl( pChar, pStage );

			CheckMove( pChar, pStage );
			ProcessMove( pChar, pStage );
		}
		break;
	case SS_LAND:
		if( m_dwMotionEndTime < dwCurTime )
			SetRunState( pChar );
		break;
	case SS_RUN:
	case SS_RUN_JUMP:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				m_dwMotionEndTime = 0;
				SetRunEndState( pChar );
				return;
			}
			else
			{
				if( m_State == SS_RUN && pChar->IsJumpKey() && !pChar->IsFloating() )
				{
					pChar->SetCurJumpPowerAmt( 0.0f );
					pChar->SetForcePowerAmt( 0.0f );
					pChar->SetGravityAmt( 0.0f );

					m_State = SS_RUN_JUMP;
					pChar->SetJumpPower( m_fRunningJumpAmt );

					if( pChar->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pChar->GetCharName();
						kPacket << GetName();
						kPacket << SS_JUMP;
						kPacket << (int)m_State;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
				else if( m_State == SS_RUN_JUMP && !pChar->IsFloating() )
				{
					m_State = SS_RUN;
				}
				
				CheckKeyInput( pChar );
				ProcessRotate( pChar );

				pChar->SetCurMoveSpeed( m_fRunSpeed );
			}
		}
		break;
	case SS_END:
		break;
	}
}

bool ioTargetMoveAttackRunSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioTargetMoveAttackRunSkill::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	
	m_dwMotionStartTime = m_dwFireStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = 0;

	// Attack
	float fFireTime = pChar->GetFirstFireTime( szSkillMotion );
	if( fFireTime > 0.0f )
		m_dwFireStartTime += fFireTime * m_fAnimationTimeRate;
	else
		m_dwFireStartTime = 0;

	D3DXVECTOR3 vEffectDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_SkillMapEffectName, vEffectDir );
	if( pMapEffect )
	{
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();
	}

	if( m_State != SS_END )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioTargetMoveAttackRunSkill::SetWaitState( ioBaseChar *pChar )
{	
	m_State = SS_WAIT;

	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioTargetMoveAttackRunSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;
	m_TargetState = TS_DELAY;

	float fCurGravity = pChar->GetCharGravity();
	m_fFlightDuration = GetFlightDuration( fCurGravity );

	if( m_fFlightDuration > 0.0f )
	{
		m_fCharMoveSpeed = m_fCircleOffSet / (m_fFlightDuration / FLOAT1000);
	}

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );	
	}

	pChar->UpdateMagicCirclePos( 0.0f, m_fCircleOffSet, m_fCircleHeightGap, true, false );
	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	// CharMove
	m_dwActionStartTime = FRAMEGETTIME();
	m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pChar->SetMoveDirByRotate( qtRot );
	}

	pChar->SetJumpPower( m_fCharJumpPower );
	m_vCircleMoveDir = m_vCharMoveDir;
}

void ioTargetMoveAttackRunSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSycState;
	rkPacket >> iSycState;

	switch( iSycState )
	{
	case SS_CREATE:
		SetActionState( pChar, pStage );
		break;
	case SS_UPDATE:
		if( m_State == SS_ACTION )
		{
			int iTargetState;
			rkPacket >> iTargetState;
			rkPacket >> m_vCirclePos;
			rkPacket >> m_vCircleMoveDir;

			m_TargetState = (TargetState)iTargetState;
		}
		break;
	case SS_JUMP:
		{
			pChar->SetCurJumpPowerAmt( 0.0f );
			pChar->SetForcePowerAmt( 0.0f );
			pChar->SetGravityAmt( 0.0f );

			m_State = SS_RUN_JUMP;
			pChar->SetJumpPower( m_fRunningJumpAmt );
		}
		break;
	case SS_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			if( iDir == ioUserKeyInput::DKI_NONE )
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			else
			{
				m_bTargetRot = false;
				m_CurDirKey = ioUserKeyInput::DirKeyInput( iDir );
			}
		}
		break;
	}
}

bool ioTargetMoveAttackRunSkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

void ioTargetMoveAttackRunSkill::SetLandState( ioBaseChar *pChar )
{
	m_State = SS_LAND;
	DWORD dwCurTime = FRAMEGETTIME();

	pChar->DestroyMagicCircle();

	if( m_LandAttackAttribute.m_AttackAnimation.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_LandAttackAttribute.m_AttackAnimation );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_LandAttackAttribute.m_fAttackAniRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	pChar->RefreshFireTimeList( iCurSkillAniID, m_LandAttackAttribute.m_vWeaponInfoList, FTT_SKILL_ATTACK, fCurEndRate, 0 );

	m_dwMotionStartTime = m_dwFireStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_LandAttackAttribute.m_dwEndDelay;

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fCurEndRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" );
}

void ioTargetMoveAttackRunSkill::SetRunState( ioBaseChar *pChar )
{
	m_State = SS_RUN;

	if( pChar->IsNeedProcess() && !m_RunActionBuffList.empty() )
	{
		pChar->SetRandomSeed( 0 );

		bool bResult = false;
		ioHashStringVec vBuffList;

		ioHashString szItemName;
		ioItem *pItem = GetOwnerItem();
		if( pItem )
			szItemName = pItem->GetName();

		float fValue = 0.0f;
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
		if( pUpInfo )
		{
			ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
			fValue = pUpInfo->GetValue(pStage);
		}

		int iCnt = m_RunActionBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_RunActionBuffList[i];

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
					LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff3 : %s, %d", GetName().c_str(), FRAMEGETTIME() );

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
					kPacket << vBuffList[i];

				kPacket << false;					// Use Force

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	m_dwActionStartTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_RunAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_RunAttackAttribute.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->AttachEffect( m_szRunEffectName );
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_RunAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										0 );


	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += m_dwRunDurationTime;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	pChar->SetCharColSkipSkillTime( dwCurTime, m_dwMotionEndTime );
	pChar->SetReservedSliding( m_RunAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );
}

void ioTargetMoveAttackRunSkill::SetRunEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	// clear buff
	int iBuffCnt = m_RunActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
		pChar->RemoveBuff( m_RunActionBuffList[i] );

	g_WeaponMgr.DestroyReserveContactWeapon( pChar->GetCharName() );
	g_WeaponMgr.DestroyReserveZoneWeapon( pChar->GetCharName() );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_RunningEndAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pChar->SetCurMoveSpeed( m_fRunningEndSpeed );
	pChar->InitActionStopDelay( true );

	float fTimeRate = FLOAT1;
	if( m_fRunningEndAniRate > 0.0f )
		fTimeRate = m_fRunningEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += (pGrp->GetAnimationFullTime(iAniID) * fTimeRate);

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->SetCharColSkipSkillTime( 0, 0 );

	m_State = SS_END;
}

bool ioTargetMoveAttackRunSkill::IsProtected( int iDefenseBreakType ) const
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
	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_State != SS_LAND )
			return true;

		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
	}

	if( (m_State == SS_RUN || m_State == SS_RUN_JUMP) && m_dwFireMotionEndTime > dwCurTime )
		return true;

	return false;
}

bool ioTargetMoveAttackRunSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioTargetMoveAttackRunSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioTargetMoveAttackRunSkill::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();
	
	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}

void ioTargetMoveAttackRunSkill::CheckMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		float fGapTime = m_fFlightDuration - (dwCurTime - m_dwActionStartTime);

		float fNewSpeed = m_fCurCharMoveSpeed;
		if( fGapTime > 0 )
		{
			fNewSpeed = fRange / (float)(fGapTime / FLOAT1000);
		}

		m_fCurCharMoveSpeed = fNewSpeed;

		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pChar->SetMoveDirByRotate( qtRot );
	}
}

void ioTargetMoveAttackRunSkill::ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCurCharMoveSpeed == 0.0f ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vCharMoveDir * m_fCurCharMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			pChar->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );
}

void ioTargetMoveAttackRunSkill::CheckKeyInput( ioBaseChar *pOwner )
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
				kPacket << SS_ROTATE;
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
				kPacket << SS_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioTargetMoveAttackRunSkill::ProcessRotate( ioBaseChar *pOwner )
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

void ioTargetMoveAttackRunSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

float ioTargetMoveAttackRunSkill::GetFlightDuration( float fCurGravity )
{
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fSkillGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = m_fCharJumpPower / fNewGravity;
		fTopHeight = (m_fCharJumpPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fResult = 2 * fTopTime * FLOAT1000;

#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "ioTargetMoveAttackRunSkill - [CheckValue] TopHeight: %.2f, FlightDuration: %.2f", fTopHeight, fResult );
	}
#endif

	return fResult;
}


void ioTargetMoveAttackRunSkill::CheckControl( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	bool bUpdate = false;
	if( m_TargetState == TS_DELAY )
	{
		if( pChar->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
			if( eDir != ioUserKeyInput::DKI_NONE )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				m_vCircleMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			}

			m_TargetState = TS_MOVE;
			bUpdate = true;
		}
	}
	else if( m_TargetState == TS_MOVE )
	{
		if( !pChar->IsSettedDirection() )
		{
			m_TargetState = TS_DELAY;
			bUpdate = true;
		}
		else
		{
			if( pChar->IsChangeDirection() )
			{
				ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
				if( eDir != ioUserKeyInput::DKI_NONE )
				{
					float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
					fYawD = ioMath::ArrangeHead( fYawD );

					float fYawR = DEGtoRAD( fYawD );
					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

					m_vCircleMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				}

				bUpdate = true;
			}
		}
	}

	if( bUpdate && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SS_UPDATE;
		kPacket << m_TargetState;
		kPacket << m_vCirclePos;
		kPacket << m_vCircleMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackRunSkill::ProcessControl( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_TargetState == TS_MOVE && m_fCircleMoveSpeed > 0.0f )
	{
		D3DXVECTOR3 vPrePos, vCurPos, vPos;
		vPos = m_vCircleMoveDir;
		vPos *= m_fCircleMoveSpeed * g_FrameTimer.GetSecPerFrame();

		vPrePos = vCurPos = m_vCirclePos;
		vCurPos += vPos;

		float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pChar, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;

		m_vCirclePos = vCurPos;

		if( pChar )
		{
			pChar->SetMagicCirclePos( true, m_vCirclePos );
		}
	}
}


