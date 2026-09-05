
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioTargetMoveAttackSkill2.h"

#include "WeaponDefine.h"

ioTargetMoveAttackSkill2::ioTargetMoveAttackSkill2()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_iCurCombo = 0;
	m_vTargetList.clear();
	m_iCurAttackCount = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioTargetMoveAttackSkill2::ioTargetMoveAttackSkill2( const ioTargetMoveAttackSkill2 &rhs )
: ioNormalSkill( rhs ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_LandAttackEndMotion( rhs.m_LandAttackEndMotion ),
 m_fLandAttackEndMotionRate( rhs.m_fLandAttackEndMotionRate ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
 m_dwAttackDuration( rhs.m_dwAttackDuration ),
 m_LoopMotion( rhs.m_LoopMotion ),
 m_fLoopMotionRate( rhs.m_fLoopMotionRate ),
 m_vComboList( rhs.m_vComboList ),
 m_fTargetRange( rhs.m_fTargetRange ),
 m_fTargetMinRange( rhs.m_fTargetMinRange ),
 m_fTargetAngle( rhs.m_fTargetAngle ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_dwProtectedTime( rhs.m_dwProtectedTime ),
 
 m_AttackType( rhs.m_AttackType ),
 m_iMaxAttackCount( rhs.m_iMaxAttackCount ),
 m_RunRotateAttack( rhs.m_RunRotateAttack ),
 m_dwRotateSpeed( rhs.m_dwRotateSpeed ),
 m_dwRunJumpTime( rhs.m_dwRunJumpTime ),
 m_fRunSpeed( rhs.m_fRunSpeed ),
 m_dwRotateAttackProtectedTime( rhs.m_dwRotateAttackProtectedTime )
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_iCurCombo = 0;
	m_vTargetList.clear();

	m_iCurAttackCount = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioTargetMoveAttackSkill2::~ioTargetMoveAttackSkill2()
{
}

void ioTargetMoveAttackSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "land_attack_end_motion", "", szBuf, MAX_PATH );
	m_LandAttackEndMotion = szBuf;
	m_fLandAttackEndMotionRate = rkLoader.LoadFloat_e( "land_attack_end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	m_dwAttackDuration = (DWORD)rkLoader.LoadInt_e( "attack_duration", 0 );

	rkLoader.LoadString_e( "loop_motion", "", szBuf, MAX_PATH );
	m_LoopMotion = szBuf;
	m_fLoopMotionRate = rkLoader.LoadFloat_e( "loop_motion_rate", FLOAT1 );

	int iComboCnt = rkLoader.LoadInt_e( "attack_combo_cnt", 0 );
	m_vComboList.clear();

	if( iComboCnt > 0 )
	{
		m_vComboList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "attack_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboList.push_back( kComboAttack );
		}
	}

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTargetMinRange = min( m_fTargetMinRange, m_fTargetRange );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwProtectedTime = (DWORD)rkLoader.LoadInt_e("reserve_protected_time", 0 );

	m_AttackType = (AttackType)rkLoader.LoadInt_e( "attack_type", AT_COMBO );
	m_iMaxAttackCount = rkLoader.LoadInt_e( "max_attack_count", 0 );
	LoadAttackAttribute_e( "rotate_attack", m_RunRotateAttack, rkLoader );
	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );
	m_dwRunJumpTime = (DWORD)rkLoader.LoadInt_e( "run_jump_time", 0 );
	m_fRunSpeed = rkLoader.LoadFloat_e( "run_speed", 0.0f );
	m_dwRotateAttackProtectedTime = (DWORD)rkLoader.LoadInt_e("rotate_attack_protected_time", 0 );

	LoadAttack( rkLoader );
	LoadLandAttack( rkLoader );
}

void ioTargetMoveAttackSkill2::LoadAttack( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	// Action Attack
	m_AttackAttribute.Init();
	m_AttackAttribute.m_AttackAnimation = m_SkillMotionName;
	m_AttackAttribute.m_fAttackAniRate  = m_fAnimationTimeRate;
	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );

	m_AttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_AttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_AttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 1 );
	m_AttackAttribute.m_vWeaponInfoList.clear();
	m_AttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

void ioTargetMoveAttackSkill2::LoadLandAttack( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	// ActionEnd Attack
	m_LandAttackAttribute.Init();
	m_LandAttackAttribute.m_AttackAnimation = m_LandAttackEndMotion;
	m_LandAttackAttribute.m_fAttackAniRate  = m_fLandAttackEndMotionRate;

	m_LandAttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "land_attack_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		m_LandAttackAttribute.m_vForceInfoList.reserve( iForceCnt );
	}

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "land_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "land_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "land_attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_LandAttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "land_attack_type_count", 1 );
	m_LandAttackAttribute.m_vWeaponInfoList.clear();
	m_LandAttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "land_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "land_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "land_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "land_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "land_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_LandAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

ioSkill* ioTargetMoveAttackSkill2::Clone()
{
	return new ioTargetMoveAttackSkill2( *this );
}

bool ioTargetMoveAttackSkill2::OnSkillStart( ioBaseChar *pChar )
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
	m_dwReserveStartTime = 0;
	m_dwAttackMotionEndTime = 0;
	m_iCurCombo = 0;
	m_vTargetList.clear();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_State = SS_NONE;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();
	
	if( m_bInitJumpPower )
	{
		pChar->SetGravityAmt( 0.0f );
		pChar->SetCurJumpPowerAmt( 0.0f );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetWaitState( pChar, false );
	}

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	SetSkillNameChatBubble( pChar );

	m_iCurAttackCount = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	return true;
}

void ioTargetMoveAttackSkill2::OnSkillEnd( ioBaseChar *pChar )
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

	pChar->SetMagicCircleTarget( "" );

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwReserveEndTime = 0;
	m_dwAttackMotionEndTime = 0;
	m_dwPreDelayStartTime = 0;

	m_State = SS_NONE;
}

void ioTargetMoveAttackSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				SetWaitState( pChar, false );
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
			SetAttackState( pChar );
		}
		else
		{
			CheckControl( pChar );
			ProcessControl( pChar, pStage );

			CheckMove( pChar, pStage );
			ProcessMove( pChar, pStage );
		}
		break;
	case SS_ATTACK:
		{
			ProcessAttack( pChar );
			if ( m_dwAttackMotionEndTime < dwCurTime )
			{
				if ( m_AttackType == AT_COMBO)
					SetReserveState( pChar );
				else if ( m_AttackType == AT_ROTATE_ATTACK && pChar->IsNeedProcess() )
					SetRotateAttack( pChar );
			}
		}
		break;
	case SS_RESERVE:
		{
			CheckComboFire( pChar, pStage );
			if ( m_dwReserveStartTime + m_dwAttackDuration < dwCurTime )
			{
				SetEndState( pChar );
			}			
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwReserveStartTime + m_dwAttackDuration < dwCurTime )
			{
				SetEndState( pChar );
			}

			CheckComboFire( pChar,pStage );
		}
		break;
	case SS_ROTATE_ATTACK:
		ProcessRotateAttackState( pChar );
		break;
	case SS_END:
		{
			
		}
		break;
	}
}

bool ioTargetMoveAttackSkill2::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioTargetMoveAttackSkill2::SetNormalSkill( ioBaseChar *pChar )
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
		m_dwFireStartTime += fFireTime * m_AttackAttribute.m_fAttackAniRate;
	else
		m_dwFireStartTime = 0;

	pChar->AttachEffect( m_StartEffect );

	//
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

void ioTargetMoveAttackSkill2::SetWaitState( ioBaseChar *pChar, bool bSend )
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

	if ( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_WAIT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackSkill2::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
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

	/*
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_CREATE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	*/
}

void ioTargetMoveAttackSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSycState;
	rkPacket >> iSycState;

	switch( iSycState )
	{
	case SST_CREATE:
		SetActionState( pChar, pStage );
		break;
	case SST_UPDATE:
		if( m_State == SS_ACTION )
		{
			int iTargetState;
			rkPacket >> iTargetState;
			rkPacket >> m_vCirclePos;
			rkPacket >> m_vCircleMoveDir;

			m_TargetState = (TargetState)iTargetState;
		}
		break;
	case SST_FIRE:
		{
			FindTarget( pChar, pStage );
			SetNextCombo( pChar, false );
		}		
		break;
	case SST_WAIT:
		{
			SetWaitState( pChar, false );
		}
		break;
	case SST_ROTATE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );
			SetRotateAttack( pChar );
		}
		break;
	case SST_KEYINPUT:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );
			int iDir;
			rkPacket >> iDir;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		}
		break;
	}
}

bool ioTargetMoveAttackSkill2::IsEndState() const
{
	if( m_State == SS_END )
		return true;
	return false;
}

void ioTargetMoveAttackSkill2::SetAttackState( ioBaseChar *pChar )
{
	m_State = SS_ATTACK;
	DWORD dwCurTime = FRAMEGETTIME();

	pChar->DestroyMagicCircle();

	if( m_LandAttackEndMotion.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_LandAttackEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fLandAttackEndMotionRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );
	
	m_dwMotionStartTime = m_dwFireStartTime = dwCurTime;
	m_dwAttackMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwAttackMotionEndTime += m_dwAnimationEndDelay;

	// Attack
	float fFireTime = pChar->GetFirstFireTime( m_LandAttackEndMotion );
	if( fFireTime > 0.0f )
		m_dwFireStartTime += fFireTime * m_LandAttackAttribute.m_fAttackAniRate;
	else
		m_dwFireStartTime = 0;

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fCurEndRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" );

	pChar->AttachEffect( m_EndEffect );
}

bool ioTargetMoveAttackSkill2::IsProtected( int iDefenseBreakType ) const
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
		switch( m_State )
		{
		case SS_WAIT:
		case SS_ACTION:
		case SS_ATTACK:
			return true;
			break;
		case SS_RESERVE:
		case SS_LOOP:
			{
				if ( m_dwReserveStartTime + m_dwProtectedTime > dwCurTime )
					return true;
			}
			break;
		case SS_ROTATE_ATTACK:
			{
				if ( m_dwReserveStartTime + m_dwRotateAttackProtectedTime > dwCurTime )
					return true;
			}
			break;
		}
	}

	return false;
}

bool ioTargetMoveAttackSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END )
		return true;
	
	return false;
}

bool ioTargetMoveAttackSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioTargetMoveAttackSkill2::CheckLanding( ioBaseChar *pChar )
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

void ioTargetMoveAttackSkill2::CheckMove( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioTargetMoveAttackSkill2::ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioTargetMoveAttackSkill2::ProcessAttack( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_State != SS_ATTACK ) return;
	if( m_dwFireStartTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireStartTime < FRAMEGETTIME() && !m_LandAttackAttribute.m_vWeaponInfoList.empty() )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		pChar->SkillFireExplicit( m_LandAttackAttribute.m_vWeaponInfoList.front(),
								  GetName(),
								  m_dwWeaponBaseIndex,
								  vPos );

		m_dwFireStartTime = 0;
	}
}

void ioTargetMoveAttackSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

	ioTraceWeaponType2 *pTraceWeapon = ToTraceWeaponType2( pWeapon );
	if( pTraceWeapon && !m_CurTargetName.IsEmpty() )
	{
		pTraceWeapon->SetTraceTarget( m_CurTargetName );
	}
}

float ioTargetMoveAttackSkill2::GetFlightDuration( float fCurGravity )
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
		LOG.PrintTimeAndLog( 0, "ioTargetMoveAttackSkill2 - [CheckValue] TopHeight: %.2f, FlightDuration: %.2f", fTopHeight, fResult );
	}
#endif

	return fResult;
}


void ioTargetMoveAttackSkill2::CheckControl( ioBaseChar *pChar )
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

		kPacket << SST_UPDATE;
		kPacket << m_TargetState;
		kPacket << m_vCirclePos;
		kPacket << m_vCircleMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackSkill2::ProcessControl( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioTargetMoveAttackSkill2::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )return;

	m_State = SS_END;
	m_iCurCombo = 0;
}

void ioTargetMoveAttackSkill2::SetReserveState( ioBaseChar *pChar )
{
	if ( !pChar )return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_RESERVE;

	ioEntityGroup *pGrp = pChar->GetGroup();
	
	int iAniID = pGrp->GetAnimationIdx( m_LoopMotion );
	if( iAniID == -1 )	return;

	float fRate = m_fLoopMotionRate;
	if ( fRate <= 0.0f )
		fRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/fRate );

	m_dwReserveStartTime = m_dwFireStartTime = dwCurTime;
	m_dwReserveEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

}

void ioTargetMoveAttackSkill2::CheckComboFire( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar )
		return;

	const AttributeList &rkComboList = m_vComboList;

	if( !COMPARE( m_iCurCombo, 0, (int)rkComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < dwCurTime )
	{
		if( pChar->IsNeedProcess() )
		{
			bool bReserveSkill = false;
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				bReserveSkill = true;
			}

			if( !bReserveSkill )
			{
				if( pChar->IsAttackKey() )
				{
					FindTarget( pChar, pStage );
					SetNextCombo( pChar, true );
					return;
				}
			}
		}
	}
	
	return;
}

void ioTargetMoveAttackSkill2::SetNextCombo( ioBaseChar *pChar, bool bSend )
{
	if( !pChar ) return;

	m_dwEnableReserveTime = 0;
	pChar->ClearSkillReserveKey();

	const AttributeList &rkComboList = m_vComboList;

	int iMaxCombo = rkComboList.size();
	if( !COMPARE( m_iCurCombo, 0, iMaxCombo ) )
		return;

	m_State = SS_LOOP;
	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = rkComboList[m_iCurCombo];

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetCurJumpPowerAmt( 0.0f );

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_CurTargetName.Clear();

	if( m_iCurCombo == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	int iTargetCnt = m_vTargetList.size();
	int iCurMaxCnt = iTargetCnt;
	int iCurFireCnt = 0;
	if ( iTargetCnt == 0 )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		
		pChar->SkillFireExplicit( kAttack.m_vWeaponInfoList.front(),
			GetName(),
			m_dwWeaponBaseIndex + m_iCurCombo,
			vPos );

		iCurFireCnt++;
	}
	for(int i=0; i<iTargetCnt; ++i)
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		m_CurTargetName = m_vTargetList[i].m_Name;

		pChar->SkillFireExplicit( kAttack.m_vWeaponInfoList.front(),
			GetName(),
			m_dwWeaponBaseIndex + m_iCurCombo,
			vPos );

		iCurFireCnt++;
	}

	if( iCurMaxCnt > 0 && iCurFireCnt < iCurMaxCnt )
	{
		m_CurTargetName.Clear();

		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		m_CurTargetName = m_vTargetList[i].m_Name;
		pChar->SkillFireExplicit( kAttack.m_vWeaponInfoList.front(),
			GetName(),
			m_dwWeaponBaseIndex + m_iCurCombo,
			vPos );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = kAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( kAttack.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndTimeRate,
			fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( kAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	m_vTargetList.clear();
	m_iCurCombo++;

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_FIRE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioTargetMoveAttackSkill2::FindTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;

	ioHashString szTarget;
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTargetChar = (*iter);
		if( !pTargetChar )	continue;

		if( pTargetChar == pChar )
			continue;

		if( pTargetChar->GetTeam() == pChar->GetTeam() )
			continue;

		if( !IsEnableTargetState( pTargetChar, true ) )
			continue;

		D3DXVECTOR3 vDiff = pTargetChar->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap >= m_fTargetAngle )
			continue;

		BaseCharDistanceSq kInfo;
		kInfo.m_Name = pTargetChar->GetCharName();
		kInfo.m_fDistanceSq = fLengthSq;
		m_vTargetList.push_back( kInfo );
	}

	if( !m_vTargetList.empty() )
		std::sort( m_vTargetList.begin(), m_vTargetList.end(), BaseCharDistanceSqSort2() );
}

bool ioTargetMoveAttackSkill2::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
			
	if( !pTarget->IsEnableTarget() )		
		return false;

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	return true;
}

bool ioTargetMoveAttackSkill2::IsNoDropState() const
{
	if ( m_State == SS_WAIT )
		return true;

	return false;
}

void ioTargetMoveAttackSkill2::SetRotateAttack( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_RunRotateAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	m_State = SS_ROTATE_ATTACK;

	m_iCurAttackCount++;

	float fTimeRate = m_RunRotateAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_RunRotateAttack.m_dwPreDelay;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_RunRotateAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = m_dwReserveStartTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_RunRotateAttack.m_dwEndDelay;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pChar->SetReservedSliding( m_RunRotateAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if ( pChar->IsNeedProcess() )
		pChar->SetTargetRotToDir( pChar->GetMoveDir(), true );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_ROTATE_ATTACK;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackSkill2::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey = eNewDirKey;

		if ( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SST_KEYINPUT;
			kPacket << pChar->GetTargetRot();
			kPacket << m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTargetMoveAttackSkill2::ProcessRotate( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	if( m_dwRotateSpeed <= 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateSpeed;
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

void ioTargetMoveAttackSkill2::ProcessRotateAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( pChar->IsNeedProcess() )
	{
		if ( CheckDropZone( pChar ) )
		{
			if ( m_iCurAttackCount < m_iMaxAttackCount )
			{
				SetWaitState( pChar, true );
				return;
			}
			else
			{
				SetEndState( pChar );
				return;
			}
		}

		if ( m_dwMotionEndTime < dwCurTime )
		{
			if ( m_iCurAttackCount < m_iMaxAttackCount )
			{
				SetWaitState( pChar, true );
				return;
			}
			else
			{
				SetEndState( pChar );
				return;
			}
		}

		if ( m_dwReserveStartTime + m_dwRunJumpTime < dwCurTime && pChar->IsJumpKey() &&
			m_iCurAttackCount < m_iMaxAttackCount )
		{
			SetWaitState( pChar, true );
			return;
		}

		CheckKeyInput( pChar );
	}

	ProcessRotate( pChar );

	pChar->SetCurMoveSpeed( m_fRunSpeed );
}

bool ioTargetMoveAttackSkill2::CheckDropZone( ioBaseChar *pChar )
{
	if ( !pChar )
		return true;

	float fHeight = pChar->GetBottomHeight();
	if ( fHeight != 0.0f )
		return false;

	return true;

}
