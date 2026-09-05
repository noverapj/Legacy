

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioComboMoveAttack2Skill.h"

#include "WeaponDefine.h"

ioComboMoveAttack2Skill::ioComboMoveAttack2Skill()
{
	ClearData();
}

ioComboMoveAttack2Skill::ioComboMoveAttack2Skill( const ioComboMoveAttack2Skill &rhs )
: ioNormalSkill( rhs ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_TargetCircleEffect( rhs.m_TargetCircleEffect ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_WaitAni( rhs.m_WaitAni ),
 m_dwWaitDuration( rhs.m_dwWaitDuration ),
 m_dwWaitProtectionTime( rhs.m_dwWaitProtectionTime ),
 m_ActionLoopAni( rhs.m_ActionLoopAni ),
 m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_dwStartTime( rhs.m_dwStartTime ),
 m_fBackRange( rhs.m_fBackRange ),
 m_fHeightRange( rhs.m_fHeightRange ),
 m_vComboList( rhs.m_vComboList ),
 m_ComboInputTimeList( rhs.m_ComboInputTimeList ),
 m_ComboInputAniList( rhs.m_ComboInputAniList ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	ClearData();
}

ioComboMoveAttack2Skill::~ioComboMoveAttack2Skill()
{
	m_vComboList.clear();
	m_ComboInputTimeList.clear();
	m_ComboInputAniList.clear();
}

void ioComboMoveAttack2Skill::ClearData()
{
	m_State = SS_NONE;
	m_bSetHandMesh = false;

	m_dwTargetCircleEffect = -1;

	m_dwPreActionEndTime = 0;
	m_dwWaitStartTime = 0;

	m_fCurCharMoveSpeed = 0.0f;

	m_iCurLinearCombo = 0;
	m_dwComboInputEnableTime = 0;
	m_dwNextComboTime = 0;
	m_dwCheckStartTime = 0;
	m_dwAttackEndTime = 0;

	m_bSetInputAni = false;
	m_dwCurSkillProtectTime = 0;

	m_TargetNameList.clear();
}

void ioComboMoveAttack2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "target_circle_effect", "", szBuf, MAX_PATH );
	m_TargetCircleEffect = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	// wait
	rkLoader.LoadString_e( "wait_ani", "", szBuf, MAX_PATH );
	m_WaitAni = szBuf;

	m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "wait_duration", 0 );
	m_dwWaitProtectionTime = (DWORD)rkLoader.LoadInt_e( "wait_protection_time", 0 );

	// action loop
	rkLoader.LoadString_e( "action_loop_ani", "", szBuf, MAX_PATH );
	m_ActionLoopAni = szBuf;

	// Action Attack
	m_LandAttackAttribute.Init();
	LoadAttackAttribute_e( "action_attack", m_LandAttackAttribute, rkLoader );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_dwStartTime = (DWORD)rkLoader.LoadInt_e( "enable_start_time", 0 );

	m_fBackRange = rkLoader.LoadFloat_e( "back_move_range", 0.0f );
	m_fHeightRange = rkLoader.LoadFloat_e( "back_move_height_range", 0.0f );

	// Combo Attack
	int iComboCnt = rkLoader.LoadInt_e( "max_combo_cnt", 0 );
	m_vComboList.clear();
	m_ComboInputTimeList.clear();
	m_ComboInputAniList.clear();

	for( int i=0; i < iComboCnt; ++i )
	{
		// combo attack
		wsprintf_e( szKey, "combo_attack%d", i+1 );

		AttackAttribute kAttackInfo;
		LoadAttackAttribute( szKey, kAttackInfo, rkLoader );


		m_vComboList.push_back( kAttackInfo );

		// combo input time
		wsprintf_e( szKey, "combo_attack%d_next", i+1 );
		DWORD dwNextTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		m_ComboInputTimeList.push_back( dwNextTime );

		// combo input ani
		wsprintf_e( szKey, "combo_attack%d_next_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ComboInputAniList.push_back( szBuf );
	}
}

ioSkill* ioComboMoveAttack2Skill::Clone()
{
	return new ioComboMoveAttack2Skill( *this );
}

bool ioComboMoveAttack2Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	if( !pChar )
		return false;

	ClearData();
	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_fCurChargeRate = 0.0f;

	m_SkillSoundID = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetPreAction( pChar );
	}

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	m_TargetNameList.clear();

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioComboMoveAttack2Skill::OnSkillEnd( ioBaseChar *pChar )
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

	if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect, false );
		m_dwSkillMapEffect = -1;
	}

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
		//g_SoundMgr.StopSound( m_SkillSoundName, 0 );
	}

	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	if( m_dwTargetCircleEffect != -1 )
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_dwComboInputEnableTime = 0;
	m_dwNextComboTime = 0;

	m_State = SS_NONE;
	m_bSetInputAni = false;
}

void ioComboMoveAttack2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetPreAction( pChar );
			}
		}
		break;
	case SS_PRE_ACTION:
		if( m_dwPreActionEndTime < dwCurTime )
		{
			SetWaitState( pChar );
		}
		break;
	case SS_WAIT:
		if( m_dwWaitStartTime + m_dwWaitDuration >= dwCurTime )
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
			pChar->UpdateMagicCirclePosAll2( m_fCircleMoveSpeed, m_fCircleRange, vCharPos.y, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() ||
					pChar->IsDefenseKey() ||
					pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					SetActionLoopState( pChar );
					return;
				}
			}
		}
/*
		else
			SetSkillEndState();*/
		break;
	case SS_ACTION_LOOP:
		{
			if( CheckLanding( pChar ) )
				AttackSkillFire( pChar );
				//SetActionState( pChar );
			else
				ProcessMove( pChar, pStage );
		}
		break;
	case SS_ACTION:
		{
			if( m_dwCheckStartTime <= dwCurTime )
			{
				if( m_dwCheckStartTime + m_dwStartTime <= dwCurTime )
					SetSkillEndState();

				if( m_dwCheckStartTime + m_dwStartTime > dwCurTime )
				{
					if( pChar->IsNeedProcess() && pChar->IsJumpKey() )
					{
						SetSkillMove( pStage, pChar );
						SetLinearComboState(pChar);
					}
				}
			}
		}
		break;
	case SS_COMBO:
		if( !m_bSetInputAni && m_dwComboInputEnableTime < dwCurTime )
		{
			m_bSetInputAni = true;

			ioEntityGroup *pGrp = pChar->GetGroup();
			int iAniID = pGrp->GetAnimationIdx( m_CurInputAni );
			if( iAniID != -1 )
			{
				pGrp->ClearAllActionAni( FLOAT10, true );
				pGrp->ClearAllLoopAni( FLOAT10, true );

				float fAniRate = FLOAT1;

				DWORD dwAniTime = pGrp->GetAnimationFullTime( iAniID );
				DWORD dwGapTime = m_dwNextComboTime - m_dwComboInputEnableTime;
				if( dwGapTime > 0 && dwAniTime > 0 )
				{
					fAniRate = (float)dwGapTime / dwAniTime;
				}

				pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate );
			}
		}

		if( pChar->IsNeedProcess() )
		{
			CheckLinearCombo( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioComboMoveAttack2Skill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar )	return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_LandAttackAttribute.m_AttackAnimation );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = m_LandAttackAttribute.m_fAttackAniRate;

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iCurSkillAniID,
										m_LandAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										m_LandAttackAttribute.m_dwPreDelay );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	//
	SetExtraAniJump( pChar, iCurSkillAniID, fTimeRate );

	pChar->CheckCharColSkipSkillTime( iCurSkillAniID, fTimeRate, m_LandAttackAttribute.m_dwPreDelay );

	pChar->SetReservedSliding( m_LandAttackAttribute.m_vForceInfoList, iCurSkillAniID, fTimeRate, m_LandAttackAttribute.m_dwPreDelay );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	m_dwCheckStartTime = dwFireStartTime;
	m_State = SS_ACTION;

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	if( m_dwTargetCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );
		m_dwTargetCircleEffect = -1;
	}

	pChar->DestroyMagicCircle();
}

void ioComboMoveAttack2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )	return;
	if( !pStage )	return;
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SS_ACTION_LOOP:
		SetActionLoopState( pChar );
		break;
	case SSC_ACTION:
		{
			ioHashString szTargetName;
			D3DXQUATERNION qtTargetRot;
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vNewPos;
			rkPacket >> szTargetName;
			rkPacket >> qtTargetRot;
			ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
			if( pTarget )
				pTarget->SetTargetRot( qtTargetRot );
			rkPacket >> qtRot;
			rkPacket >> vNewPos;
			pChar->SetTargetRot( qtRot );
			pChar->SetWorldPosition( vNewPos );			
			SetLinearComboState( pChar );
		}
		break;
	case SSC_COMBO:
		{
			int iState, iCombo;
			rkPacket >> iState >> iCombo;

			m_State = (SkillState)iState;
			m_iCurLinearCombo = iCombo;

			SetNextLinearCombo( pChar, false );
		}
		break;
	}
}

bool ioComboMoveAttack2Skill::IsProtected( int iDefenseBreakType ) const
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
		if( m_dwCurSkillProtectTime > 0 &&
			m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioComboMoveAttack2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioComboMoveAttack2Skill::IsNoDropState() const
{
	if( m_State == SS_WAIT )
		return true;

	if( m_State == SS_COMBO )
		return true;

	if( m_State == SS_END && m_dwMotionEndTime > FRAMEGETTIME() )
		return true;

	return false;
}

bool ioComboMoveAttack2Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioComboMoveAttack2Skill::IsAttackEndState() const
{
	if( m_State == SS_COMBO && m_dwNextComboTime < FRAMEGETTIME() )
		return true;

	if( m_State == SS_END && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;

	if( m_State == SS_WAIT && m_dwWaitStartTime + m_dwWaitDuration < FRAMEGETTIME() )
		return true;

	return false;
}

void ioComboMoveAttack2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

void ioComboMoveAttack2Skill::SetLinearComboState( ioBaseChar *pChar )
{
	if( m_State == SS_END )	return;
	if( !pChar )	return;

	m_State = SS_COMBO;
	m_iCurLinearCombo = 0;
	m_bSetInputAni = false;

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = 0;

	pChar->AttachEffect( m_StartEffect );

	//
	if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect, false );
		m_dwSkillMapEffect = -1;
	}

	if( !m_SkillSoundName.IsEmpty() && pChar )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		m_SkillSoundID = g_SoundMgr.PlaySound( m_SkillSoundName, pGrp->GetParentSceneNode() );
	}

	D3DXVECTOR3 vEffectDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_SkillMapEffectName, vEffectDir );
	if( pMapEffect )
	{
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	SetNextLinearCombo( pChar, false );
}

void ioComboMoveAttack2Skill::SetNextLinearCombo( ioBaseChar *pChar, bool bSend )
{
	m_dwEnableReserveTime = 0;
	m_bSetInputAni = false;
	m_CurInputAni.Clear();

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
		return;

	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_vComboList[m_iCurLinearCombo].m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = m_vComboList[m_iCurLinearCombo].m_fAttackAniRate;
	DWORD dwPreDelay = m_vComboList[m_iCurLinearCombo].m_dwPreDelay;
	DWORD dwInputTime = 0;

	if( COMPARE( m_iCurLinearCombo, 0, (int)m_ComboInputTimeList.size() ) )
	{
		dwInputTime = m_ComboInputTimeList[m_iCurLinearCombo];
		m_CurInputAni = m_ComboInputAniList[m_iCurLinearCombo];
	}

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
										m_vComboList[m_iCurLinearCombo].m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_vComboList[m_iCurLinearCombo].m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwFireMotionEndTime = dwCurTime + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveTime > 0.0f )
	{
		m_dwEnableReserveTime = dwCurTime + fReserveTime;
	}

	if( m_vComboList[m_iCurLinearCombo].m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = m_vComboList[m_iCurLinearCombo].m_fAttackEndAniRate;

		pGrp->SetActionAni( m_vComboList[m_iCurLinearCombo].m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( m_vComboList[m_iCurLinearCombo].m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	m_dwComboInputEnableTime = m_dwMotionEndTime;
	m_dwNextComboTime = m_dwComboInputEnableTime + dwInputTime;
	m_dwMotionEndTime += dwInputTime;

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
	//

	pChar->SetReservedSliding( m_vComboList[m_iCurLinearCombo].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_COMBO;
		kPacket << m_State;
		kPacket << m_iCurLinearCombo;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}

void ioComboMoveAttack2Skill::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( COMPARE( dwCurTime, m_dwComboInputEnableTime, m_dwNextComboTime ) )
	{
		if( pChar->IsNeedProcess() )
		{
			bool bReserveSkill = false;
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				bReserveSkill = true;
			}

			if( pChar->IsAttackKey() && !bReserveSkill )
			{
				SetNextLinearCombo( pChar, true );
				return;
			}
		}
	}
}

void ioComboMoveAttack2Skill::SetSkillMove( ioPlayStage *pStage, ioBaseChar *pChar )
{
	if( !pStage )	return;
	if( !pChar )	return;

	if( !pChar->IsNeedProcess() )
		return;

	ioBaseChar *pTargetChar = NULL;
	float fPreHP = FLOAT1000;
	int iCnt = m_TargetNameList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetNameList[i] );
		if( !pTarget )	continue;
		if( !pTarget->IsFloating() )	continue;
		if( !pTarget->HasCollisionAvailableBuff() ) continue;
		if( pTarget->GetState() == CS_SKIP_STUN )	continue;
		if( Help::CheckTargetNoCollision( pTarget->GetState() ) ) continue;
		if( pTarget->HasHideBuff() )	continue;
		if( !pStage->IsCollisionCheckByMode() )	continue;
		if( pTarget->IsSkipState() ) continue;

		float fCurHP = pTarget->GetHP().m_fCurValue;
		if( fCurHP < fPreHP )
		{
			fPreHP = fCurHP;
			pTargetChar = pTarget;
		}
	}

	if( pTargetChar )
	{
		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition();
		D3DXVECTOR3 vOwnerPos = pChar->GetWorldPosition();
		D3DXVECTOR3 vTargetDir = vTargetPos - vOwnerPos;
		vTargetDir.y = 0.0f;
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );
		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vTargetDir );
		pTargetChar->SetTargetRot( qtTargetRot );

		D3DXVECTOR3 vOffset = D3DXVECTOR3( 0.0f, m_fHeightRange, 0.0f );
		vTargetDir = pTargetChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );
		D3DXVECTOR3 vNewPos = vTargetPos + vOffset - ( vTargetDir * m_fBackRange );
		D3DXQUATERNION qtOwnerRot;
		ioMath::CalcDirectionQuaternion( qtOwnerRot, -vTargetDir );
		pChar->SetTargetRot( qtOwnerRot );
		pChar->SetWorldPosition( vNewPos );

		pStage->SetCameraToOwnerExplicit( pChar );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_ACTION;
			kPacket << pTargetChar->GetCharName();
			kPacket << qtTargetRot;
			kPacket << qtOwnerRot;
			kPacket << vNewPos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		SetSkillEndState();
	}
}

void ioComboMoveAttack2Skill::SetSkillEndState()
{
	m_State = SS_END;
}

void ioComboMoveAttack2Skill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

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

void ioComboMoveAttack2Skill::SetPreAction( ioBaseChar *pChar )
{
	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );
}

void ioComboMoveAttack2Skill::SetWaitState( ioBaseChar *pChar )
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

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_WaitAni );
	float fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( 5.0f );
	pGrp->ClearAllActionAni( 5.0f );
	pGrp->SetLoopAni( iCurSkillAniID, 5.0f );

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	if( !m_TargetCircleEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_TargetCircleEffect, vPos );

		if( pMapEffect )
		{
			if( m_dwTargetCircleEffect != -1 )
				pChar->EndMapEffect( m_dwTargetCircleEffect, false );

			m_dwTargetCircleEffect = pMapEffect->GetUniqueID();
		}
	}
}

void ioComboMoveAttack2Skill::SetActionLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	m_State = SS_ACTION_LOOP;

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_ActionLoopAni );
	float fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( 5.0f );
	pGrp->ClearAllActionAni( 5.0f );
	pGrp->SetLoopAni( iCurSkillAniID, 5.0f );

	float fCurRange = m_fCircleRange;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		}
		else
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
		}
	}

	if( pMapEffect )
	{
		if( m_dwCurMapEffect != -1 )
			pChar->EndMapEffect( m_dwCurMapEffect, false );

		m_dwCurMapEffect = pMapEffect->GetUniqueID();

		if( m_dwTargetCircleEffect != -1 )
		{
			pChar->EndMapEffect( m_dwTargetCircleEffect, false );
			m_dwTargetCircleEffect = -1;
		}
	}

	// CharMove
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	float fCurGravity = pChar->GetCharGravity();
	float fFlightDuration = GetFlightDuration( vCharPos, fCurGravity );

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = m_vCirclePos - vCharPos;
	vDir.y = 0.0f;

	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		m_fCurCharMoveSpeed = 0.0f;
		if( fFlightDuration > 0.0f )
		{
			m_fCurCharMoveSpeed = fRange / fFlightDuration;
		}

		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pChar->SetTargetRotAndMoveDirChange( qtRot, false );
		pChar->SetWorldOrientation( qtRot );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ACTION_LOOP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioComboMoveAttack2Skill::CheckLanding( ioBaseChar *pChar )
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

void ioComboMoveAttack2Skill::ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage )
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

float ioComboMoveAttack2Skill::GetFlightDuration( const D3DXVECTOR3 &vCharPos, float fCurGravity )
{
	float fGapHeight = fabsf(vCharPos.y-m_vCirclePos.y);
	float fGapTime = 0.0f;

	float fNewGravity = fCurGravity * GetSkillGravityRate();
	if( fNewGravity > 0.0f )
	{
		fGapTime = sqrt( 2.0f*fGapHeight/fNewGravity );
	}

	return fGapTime;
}

void ioComboMoveAttack2Skill::SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock )
{
	if( bBlock ) return;
	if( !pOwner )	return;
	if( !pTarget )	return;

	ioHashString	szTargetName = pTarget->GetCharName();
	if( szTargetName.IsEmpty() )	return;

	int iCnt = m_TargetNameList.size();
	bool bAlreadyCollision = false;
	for( int i=0; i<iCnt; ++i )
	{
		if( m_TargetNameList[i] == szTargetName )
			bAlreadyCollision = true;
	}

	if( !bAlreadyCollision )
	{
		m_TargetNameList.push_back( szTargetName );
	}
}