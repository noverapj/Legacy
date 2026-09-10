
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioComboMoveAttack4Skill.h"

#include "WeaponDefine.h"

ioComboMoveAttack4Skill::ioComboMoveAttack4Skill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;

	m_bSetHandMesh = false;
	m_dwPreActionEndTime = 0;	
	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_dwCurProtectTime = 0;
	m_dwAttackStartTime = 0;
	m_dwAttackEndTime = 0;

	m_bNoAttackDelay = false;
}

ioComboMoveAttack4Skill::ioComboMoveAttack4Skill( const ioComboMoveAttack4Skill &rhs )
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
	m_AttackAttributeA( rhs.m_AttackAttributeA ),
	m_AttackAttributeS( rhs.m_AttackAttributeS ),
	m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
	m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
	m_dwProtectTime( rhs.m_dwProtectTime ),
	m_dwOrgLoopDuration( rhs.m_dwOrgLoopDuration ),
	m_fJumpPowerInJump( rhs.m_fJumpPowerInJump ),
	m_strPreActionAni_Jump( rhs.m_strPreActionAni_Jump ),
	m_fPreActionAniRate_Jump( rhs.m_fPreActionAniRate_Jump ),
	m_dwAttackDurationTime( rhs.m_dwAttackDurationTime )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_bSetHandMesh = false;

	m_dwPreActionEndTime = 0;

	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_dwCurProtectTime = 0;
	m_dwAttackStartTime = 0;
	m_dwAttackEndTime = 0;

	m_bNoAttackDelay = false;
}

ioComboMoveAttack4Skill::~ioComboMoveAttack4Skill()
{
}

void ioComboMoveAttack4Skill::LoadProperty( ioINILoader &rkLoader )
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

	wsprintf_e( szBuf, "charging_combo_attack_a");	
	LoadAttackAttribute( szBuf, m_AttackAttributeA, rkLoader );	
	
	wsprintf_e( szBuf, "charging_combo_attack_s");	
	LoadAttackAttribute( szBuf, m_AttackAttributeS, rkLoader );	

	// Jump
	m_fJumpPowerInJump = rkLoader.LoadFloat_e( "jump_power_InJump", 0.0f);

	// JumpMotion
	rkLoader.LoadString_e( "jump_skill_motion", "", szBuf, MAX_PATH );
	m_strPreActionAni_Jump = szBuf;
	m_fPreActionAniRate_Jump = rkLoader.LoadFloat_e( "jump_animation_time_rate", FLOAT1 );

	m_dwAttackDurationTime = rkLoader.LoadInt_e( "skill_attack_duration", 0 );
}

ioSkill* ioComboMoveAttack4Skill::Clone()
{
	return new ioComboMoveAttack4Skill( *this );
}

bool ioComboMoveAttack4Skill::OnSkillStart( ioBaseChar *pChar )
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
	
	m_dwAttackStartTime = 0;
	m_dwAttackEndTime = 0;

	m_bNoAttackDelay = false;

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
	
	m_bSkillInJump = false;
	if( m_SkillUseType == SUT_JUMP )
		m_bSkillInJump = true;

	pChar->SetGravityAmt( 0.0f );

	CheckCharMoveDir( pChar );

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetPreActionState( pChar );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioComboMoveAttack4Skill::OnSkillEnd( ioBaseChar *pChar )
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
	m_bNoAttackDelay = false;
	m_dwAttackStartTime = 0;
	m_dwAttackEndTime = 0;
}

void ioComboMoveAttack4Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
			if ( !m_bNoAttackDelay )
			{
				if ( m_dwLoopDurationStart > 0 && m_dwLoopDurationEnd < dwCurTime )
				{
					SetEndState( pChar );
				}
			}
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			CheckLoopState( pChar, pStage );				
			
			if ( m_dwAttackStartTime > 0 && m_dwAttackEndTime < dwCurTime )
			{
				SetEndState(pChar);
			}			
		}
		break;
	case SS_ACTION:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{				
				SetLoopState( pChar, false );
			}
		}
		break;
	case SS_END_JUMP:
		{

		}
		break;
	}
}

void ioComboMoveAttack4Skill::SetEndState( ioBaseChar *pChar )
{

	if( !pChar ) return;

	m_dwLoopDurationEnd = 0;

	m_State = SS_END_JUMP;

	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	m_dwCurMapEffect = -1;
}

bool ioComboMoveAttack4Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioComboMoveAttack4Skill::SetPreActionState( ioBaseChar *pChar )
{
	EndPreDelayEffect( pChar );

	m_State = SS_PRE_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();
	float fAniRate = GetSkillMotionRate();
	if ( !m_bSkillInJump )
	{
		pChar->SetJumpPower( m_fCharJumpPower * m_fSkillGravityRate );
	}
	else
	{
		szSkillMotion = m_strPreActionAni_Jump;
		fAniRate = m_fPreActionAniRate_Jump;
		pChar->SetJumpPower( m_fJumpPowerInJump );
	}

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	
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

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioComboMoveAttack4Skill::SetLoopState( ioBaseChar *pChar, bool bSend )
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

void ioComboMoveAttack4Skill::CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
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
			SetActionState( pChar, pStage, K_ATTACK ,true );
		}
		else if ( pChar->IsDefenseKey() && !bReserveSkill )
		{
			SetActionState( pChar, pStage, K_DEFENSE ,true );
		}
		else if ( pChar->IsJumpKey() && !bReserveSkill )
		{
			SetActionState( pChar, pStage, K_JUMP ,true );
		}		
	}
}


void ioComboMoveAttack4Skill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, int iKey, bool bSend )
{	

	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	if( m_State == SS_END_JUMP )
		return;

	AttackAttribute TempAttackAttribute;

	switch( iKey )
	{
	case K_ATTACK:
		TempAttackAttribute = m_AttackAttribute;
		break;
	case K_DEFENSE:
		TempAttackAttribute = m_AttackAttributeS;
		break;
	case K_JUMP:
		TempAttackAttribute = m_AttackAttributeA;
		break;
	}

	int iAniID = pGrp->GetAnimationIdx( TempAttackAttribute.m_AttackAnimation );
	float fAniRate = TempAttackAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = TempAttackAttribute.m_dwPreDelay * fAniRate;
	DWORD dwDelay = TempAttackAttribute.m_dwEndDelay * fAniRate;
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );	

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	//Refresh
	if( !TempAttackAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			TempAttackAttribute.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fAniRate,
			dwPreDelay 
			);
	}

	//모션 종료시간
	DWORD dwCurTime = FRAMEGETTIME();

	if ( !m_bNoAttackDelay )
	{
		m_dwAttackStartTime = dwCurTime;
		m_dwAttackEndTime = dwCurTime + m_dwAttackDurationTime;
		m_bNoAttackDelay = true;
	}

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += dwPreDelay + dwDelay;

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

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		kPacket << iKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_State = SS_ACTION;
}

void ioComboMoveAttack4Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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
			int iKey;
			rkPacket >> vPos >> qtRot >> iKey;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );

			SetActionState( pChar, pStage, iKey, false );
		}
		break;
	}
}

bool ioComboMoveAttack4Skill::IsProtected( int iDefenseBreakType ) const
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

bool ioComboMoveAttack4Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if ( m_State != SS_END_JUMP )
		return false;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( m_fSkillEndNextJumpAmt > 0.0f )
	{
		if( fHeightGap > FLOAT10 )
			return false;
	}

	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;

//	return false;
}

bool ioComboMoveAttack4Skill::IsAttackEndState() const
{	
	if( m_State == SS_END_JUMP )
		return true;

	return false;
}

bool ioComboMoveAttack4Skill::IsNoDropState() const
{
	if( m_State == SS_LOOP || m_State == SS_ACTION )
		return true;

	return false;
}

bool ioComboMoveAttack4Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioComboMoveAttack4Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = pWeapon->GetOwner();
	auto pArrow = ToArrowWeapon( pWeapon );
	if( pChar )
	{
		if ( pArrow )
		{
			D3DXVECTOR3 vDiff = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDiff, &vDiff );
			pWeapon->SetMoveDir( vDiff );
		}
		else
		{
			D3DXVECTOR3 vDiff = m_vCirclePos - pChar->GetMidPositionByRate();
			D3DXVec3Normalize( &vDiff, &vDiff );
			pWeapon->SetMoveDir( vDiff );		
		}
	}
}

void ioComboMoveAttack4Skill::CheckCharMoveDir( ioBaseChar *pChar )
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