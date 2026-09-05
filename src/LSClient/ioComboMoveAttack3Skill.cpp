

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioComboMoveAttack3Skill.h"

#include "WeaponDefine.h"

ioComboMoveAttack3Skill::ioComboMoveAttack3Skill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;

	m_bSetHandMesh = false;
	m_dwPreActionEndTime = 0;	
	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;
	
	m_dwCurProtectTime = 0;
	m_fCurCharJumpPower = 0.0f;
}

ioComboMoveAttack3Skill::ioComboMoveAttack3Skill( const ioComboMoveAttack3Skill &rhs )
: ioNormalSkill( rhs ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_dwProtectTime( rhs.m_dwProtectTime ),
 m_dwOrgLoopDuration( rhs.m_dwOrgLoopDuration ),
 m_fReduceSkillGauge( rhs.m_fReduceSkillGauge ),
 m_SkillActionType( rhs.m_SkillActionType ),
 m_fActionJumpPower( rhs.m_fActionJumpPower ),
 m_dwActionRotateSpeed( rhs.m_dwActionRotateSpeed ),
 m_fActionJumpMoveSpeed( rhs.m_fActionJumpMoveSpeed ),
 m_fJumpSkillCharJumpPower( rhs.m_fJumpSkillCharJumpPower )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_bSetHandMesh = false;

	m_dwPreActionEndTime = 0;
	
	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_dwCurProtectTime = 0;
	m_fCurCharJumpPower = 0.0f;
}

ioComboMoveAttack3Skill::~ioComboMoveAttack3Skill()
{
}

void ioComboMoveAttack3Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );	
	m_dwProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwOrgLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );
		
	wsprintf_e( szBuf, "charging_combo_attack");	
	LoadAttackAttribute( szBuf, m_AttackAttribute, rkLoader );	

	//skill start시 최초에 줄여줄 게이지
	m_fReduceSkillGauge = rkLoader.LoadFloat_e( "reduce_skill_gauge", 0.0f );
	m_SkillActionType = (SkillActionType)rkLoader.LoadInt_e( "skill_action_type", SAT_NONE );

	m_fActionJumpPower = rkLoader.LoadFloat_e( "action_jump_power", 0.0f );
	m_dwActionRotateSpeed = (DWORD)rkLoader.LoadInt_e( "action_rotate_speed", 0 );
	m_fActionJumpMoveSpeed = rkLoader.LoadFloat_e( "action_jump_move_speed", 0.0f );
	m_fJumpSkillCharJumpPower = rkLoader.LoadFloat_e( "jump_skill_char_jump_power", 0.0f );
}

ioSkill* ioComboMoveAttack3Skill::Clone()
{
	return new ioComboMoveAttack3Skill( *this );
}

bool ioComboMoveAttack3Skill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwPreActionEndTime = 0;
	m_dwMotionEndTime = 0;
	
	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	
	m_dwCurProtectTime = 0;

	m_State = SS_NONE;

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

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{		
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	CheckCharMoveDir( pChar );

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetPreActionState( pChar );
	}

	SetSkillNameChatBubble( pChar );

	//기본 게이지 감소 (마법진 게이지)
	if ( m_SkillActionType == SAT_ONE_COUNT_ATTACK && m_fReduceSkillGauge > 0.0f )
		pChar->ReduceNeedGaugeBySkill( this, m_fReduceSkillGauge );

	if ( m_iMaxActiveCount > 0 )
		m_iCurActiveCount = m_iMaxActiveCount;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	if ( m_SkillActionType == SAT_JUMP_ATTACK && m_SkillUseType == SUT_JUMP )
		m_fCurCharJumpPower = m_fJumpSkillCharJumpPower;
	else
		m_fCurCharJumpPower = m_fCharJumpPower;
	
	return true;
}

void ioComboMoveAttack3Skill::OnSkillEnd( ioBaseChar *pChar )
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

	pChar->DestroyMagicCircle();
	
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_dwCurProtectTime = 0;

	m_State = SS_NONE;	
}

void ioComboMoveAttack3Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{	
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetPreActionState( pChar );
			}
		}
		break;

	case SS_PRE_ACTION:
		{	
			
			if( m_dwPreActionEndTime > 0 && m_dwPreActionEndTime < dwCurTime )
			{	
				SetLoopState( pChar, false );
				return;
			}						
		}
		break;

	case SS_LOOP: 
		{	

			if( COMPARE( dwCurTime, m_dwLoopDurationStart, m_dwLoopDurationEnd ) )
			{	
				// 뱡향 변화 체크
				pChar->CheckChangeDirectionForMagicCircle();
				
				// MagicCircle 위치 갱신
				pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );
				
				// MagicCircle 위치방향으로 캐릭터 회전
				D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
				pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

				CheckLoopState( pChar, pStage );				
			}
			else
				SetEndJumpState(pChar);			
		}
		break;

	case SS_ACTION:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{				
				if ( m_SkillActionType == SAT_NONE)
					SetLoopState( pChar, false );
				else if ( m_SkillActionType == SAT_ONE_COUNT_ATTACK )
					SetEndJumpState( pChar );
			}
		}
		break;
	case SS_JUMP_ACTION:
		{
			if( m_fActionJumpPower > 0.0f )
			{
				if( CheckLanding( pChar ) )
				{
					SetEndState( pChar );
					return;
				}
			}

			if( m_iMaxActiveCount > 0 && m_iCurActiveCount <= 0 )
			{
				if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
				{
					SetEndJumpState( pChar );
					return;
				}
			}
			else
			{
				if( pChar->IsNeedProcess() && pChar->IsJumpKey() )
				{
					SetLoopState( pChar, true );
					return;
				}
			}

			CheckKeyInput( pChar );
			ProcessRotate( pChar );
			ProcessJumpMove( pChar, pStage );

		}
		break;
	case SS_END_JUMP:
		{
			
		}
		break;
	}
}

void ioComboMoveAttack3Skill::SetEndJumpState( ioBaseChar *pChar ){

	if( !pChar ) return;
	
	m_dwLoopDurationEnd = 0;

	m_State = SS_END_JUMP;

	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	m_dwCurMapEffect = -1;
}

bool ioComboMoveAttack3Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioComboMoveAttack3Skill::SetPreActionState( ioBaseChar *pChar )
{
	EndPreDelayEffect( pChar );
	
	m_State = SS_PRE_ACTION;
	pChar->SetJumpPower( m_fCurCharJumpPower * m_fSkillGravityRate );

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();
	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/(fTimeRate), 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_dwPreActionEndTime = dwCurTime;
	m_dwPreActionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_dwCurProtectTime = dwCurTime + m_dwProtectTime;

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if ( m_SkillActionType != SAT_ONE_COUNT_ATTACK )
	{
		if( !m_bReduceGauge )
		{
			pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
			m_bReduceGauge = true;
		}
	}
}

void ioComboMoveAttack3Skill::SetLoopState( ioBaseChar *pChar, bool bSend )
{
	if( !pChar ) return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;
		
	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;
	
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_dwLoopDurationStart == 0 )
	{
		m_dwLoopDurationStart = dwCurTime;
		m_dwLoopDurationEnd = dwCurTime + m_dwOrgLoopDuration;
	}


	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}


	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LOOP;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioComboMoveAttack3Skill::CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;	

	if( m_dwPreDelayStartTime != 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	if( pChar->IsNeedProcess() )
	{
		//키입력 여부 검사
		bool bReserveSkill = false;
		int iSkillNum = pChar->GetSkillKeyInput();
		if( iSkillNum >= 0 )
		{
			bReserveSkill = true;
		}

		if( pChar->IsAttackKey() && !bReserveSkill )
		{
			if ( m_SkillActionType == SAT_JUMP_ATTACK )
				SetJumpActionState( pChar );
			else
				SetActionState( pChar, pStage, true );		
		}		
	}
}


void ioComboMoveAttack3Skill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend )
{	

	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	
	if( m_State == SS_END_JUMP )
		return;
	
	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	float fAniRate = m_AttackAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = m_AttackAttribute.m_dwPreDelay * fAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );	
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	//Refresh
	if( !m_AttackAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			m_AttackAttribute.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fAniRate,
			dwPreDelay 
		);
	}
	
	//모션 종료시간
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += dwPreDelay;
	
	//발사모션부분
	m_dwFireMotionEndTime = dwCurTime + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	if( pChar->CheckMagicCircle() )
	{		
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	//
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

	if ( m_SkillActionType == SAT_ONE_COUNT_ATTACK )
	{
		if( !m_bReduceGauge )
		{
			pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
			m_bReduceGauge = true;
		}
	}

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
	m_State = SS_ACTION;
}

void ioComboMoveAttack3Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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

			SetLoopState( pChar, false );
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
	case SSC_JUMP_ACTION:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );
			SetJumpActionState( pChar );
		}
		break;
	case SSC_KEY_CHANGE:
		{
			D3DXQUATERNION qtCharRot;
			rkPacket >> qtCharRot;

			pChar->SetTargetRotToRotate( qtCharRot, true, false );
			int nKey;
			rkPacket >> nKey;
			m_PreDirKey = (ioUserKeyInput::DirKeyInput)nKey;
			rkPacket >> nKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nKey;
		}
		break;
	}
}

bool ioComboMoveAttack3Skill::IsProtected( int iDefenseBreakType ) const
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
	
	DWORD dwTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_PRE_ACTION:
		return true;
		break;
	case SS_LOOP:
	case SS_ACTION:
				
		if( m_dwCurProtectTime < dwTime )
			return false;
		else
			return true;				
		break;

	case SS_END_JUMP:		
			return false;
		break;
	}

	return false;
}

bool ioComboMoveAttack3Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioComboMoveAttack3Skill::IsAttackEndState() const
{	
	if( m_State == SS_END_JUMP )
		return true;
	
	return false;
}

bool ioComboMoveAttack3Skill::IsNoDropState() const
{
	if( m_State == SS_LOOP || m_State == SS_ACTION )
		return true;

	return false;
}

bool ioComboMoveAttack3Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioComboMoveAttack3Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = pWeapon->GetOwner();

	if( pChar )
	{
		D3DXVECTOR3 vDiff = m_vCirclePos - pChar->GetMidPositionByRate();
		D3DXVec3Normalize( &vDiff, &vDiff );
		pWeapon->SetMoveDir( vDiff );		
	}
}

void ioComboMoveAttack3Skill::CheckCharMoveDir( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pChar->CheckCurDirKey();
		
		float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pChar->SetTargetRotToRotate( qtRotate, true, false );
	}
}

void ioComboMoveAttack3Skill::SetJumpActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	pChar->SetJumpPower(0.0f);
	pChar->SetGravityAmt( 0.0f );
	pChar->SetJumpPower( m_fActionJumpPower );

	if( m_State == SS_END_JUMP )
		return;

	m_dwLoopDurationStart = 0;
	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	float fAniRate = m_AttackAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = m_AttackAttribute.m_dwPreDelay * fAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );	

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	//Refresh
	if( !m_AttackAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			m_AttackAttribute.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fAniRate,
			dwPreDelay 
			);
	}

	//모션 종료시간
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += dwPreDelay;

	//발사모션부분
	m_dwFireMotionEndTime = dwCurTime + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	if( pChar->CheckMagicCircle() )
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, true, true );

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	//
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
	pChar->DestroyMagicCircle();
	m_State = SS_JUMP_ACTION;
	m_iCurActiveCount--;
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_JUMP_ACTION;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

bool ioComboMoveAttack3Skill::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar )
		return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
		return false;
	else
		return true;
}

void ioComboMoveAttack3Skill::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar || !pChar->IsNeedProcess() )
		return;
	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
	}

	if( bChangeDir )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_KEY_CHANGE;
		kPacket << pChar->GetTargetRot();
		kPacket << (int)m_PreDirKey;
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioComboMoveAttack3Skill::ProcessRotate( ioBaseChar *pChar )
{
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwActionRotateSpeed;
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

	if( fDotValue >= 0.98f )
	{
		pChar->SetTargetRotToRotate( qtRotate, true );
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
	pChar->SetTargetRotToRotate( qtNewRot, true );
}

void ioComboMoveAttack3Skill::ProcessJumpMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;
	if( !pStage )
		return;

	if( m_fActionJumpMoveSpeed == 0.0f )
		return;
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = vMoveDir * m_fActionJumpMoveSpeed * fDivideTimePerSec;
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

void ioComboMoveAttack3Skill::SetEndState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_dwLoopDurationEnd = 0;

	m_State = SS_END;

	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	m_dwCurMapEffect = -1;
}