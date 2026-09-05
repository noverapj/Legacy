

#include "stdafx.h"

#include "ioAngleFire5Skill.h"
#include "WeaponDefine.h"

ioAngleFire5Skill::ioAngleFire5Skill()
{
	ClearData();
}

ioAngleFire5Skill::ioAngleFire5Skill( const ioAngleFire5Skill &rhs )
: ioAttackSkill( rhs ),
 m_SkillUpMotion( rhs.m_SkillUpMotion ),
 m_SkillDownMotion( rhs.m_SkillDownMotion ),
 m_JumpSkillUpMotion( rhs.m_JumpSkillUpMotion ),
 m_JumpSkillDownMotion( rhs.m_JumpSkillDownMotion ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_SkillUpLoopMotion( rhs.m_SkillUpLoopMotion ),
 m_SkillDownLoopMotion( rhs.m_SkillDownLoopMotion ),
 m_JumpSkillLoopMotion( rhs.m_JumpSkillLoopMotion ),
 m_JumpSkillUpLoopMotion( rhs.m_JumpSkillUpLoopMotion ),
 m_JumpSkillDownLoopMotion( rhs.m_JumpSkillDownLoopMotion ),
 m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_fMaxAngle( rhs.m_fMaxAngle ),
 m_fMinAngle( rhs.m_fMinAngle ),
 m_dwFireGuidDuration( rhs.m_dwFireGuidDuration ),
 m_ExtraGuidEffect( rhs.m_ExtraGuidEffect ),
 m_fAimRange( rhs.m_fAimRange ),
 m_vAimInfoList( rhs.m_vAimInfoList ),
 m_szAimFireEffect( rhs.m_szAimFireEffect ),
 m_szZeroAimFireEffect( rhs.m_szZeroAimFireEffect ),
 m_szFireBuffList( rhs.m_szFireBuffList ),
 m_bVisibleAimMarkOnlyOwner( rhs.m_bVisibleAimMarkOnlyOwner ),
 m_szWarpStartMotion( rhs.m_szWarpStartMotion ),
 m_szWarpJumpStartMotion( rhs.m_szWarpJumpStartMotion ),
 m_szWarpEndMotion( rhs.m_szWarpEndMotion ),
 m_fWarpStartMotionRate( rhs.m_fWarpStartMotionRate ),
 m_fWarpJumpStartMotionRate( rhs.m_fWarpJumpStartMotionRate ),
 m_fWarpEndMotionRate( rhs.m_fWarpEndMotionRate ),
 m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
 m_fEndJumpValue( rhs.m_fEndJumpValue )
{
	ClearData();
}

ioAngleFire5Skill::~ioAngleFire5Skill()
{
	ClearData();
}

void ioAngleFire5Skill::ClearData()
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwLoopStartTime	= 0;
	m_dwAimCheckTime	= 0;
	m_dwZeroAimCheckTime= 0;
	m_fCurMotionRate	= 0.0f;

	m_dwFireStartTime = 0;
	m_dwExtraGuidEffect = -1;

	m_szTargetName.Clear();

	m_bZeroAimFire = false;
}

void ioAngleFire5Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "skill_up_motion", "", szBuf, MAX_PATH );
	m_SkillUpMotion = szBuf;
	rkLoader.LoadString_e( "skill_down_motion", "", szBuf, MAX_PATH );
	m_SkillDownMotion = szBuf;
	
	rkLoader.LoadString_e( "jump_skill_up_motion", "", szBuf, MAX_PATH );
	m_JumpSkillUpMotion = szBuf;
	rkLoader.LoadString_e( "jump_skill_down_motion", "", szBuf, MAX_PATH );
	m_JumpSkillDownMotion = szBuf;

	rkLoader.LoadString_e( "skill_up_loop_motion", "", szBuf, MAX_PATH );
	m_SkillUpLoopMotion = szBuf;
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	rkLoader.LoadString_e( "skill_down_loop_motion", "", szBuf, MAX_PATH );
	m_SkillDownLoopMotion = szBuf;

	rkLoader.LoadString_e( "jump_skill_up_loop_motion", "", szBuf, MAX_PATH );
	m_JumpSkillUpLoopMotion = szBuf;
	rkLoader.LoadString_e( "jump_skill_loop_motion", "", szBuf, MAX_PATH );
	m_JumpSkillLoopMotion = szBuf;
	rkLoader.LoadString_e( "jump_skill_down_loop_motion", "", szBuf, MAX_PATH );
	m_JumpSkillDownLoopMotion = szBuf;

	rkLoader.LoadString_e( "warp_start_motion", "", szBuf, MAX_PATH );
	m_szWarpStartMotion = szBuf;
	rkLoader.LoadString_e( "warp_jump_start_motion", "", szBuf, MAX_PATH );
	m_szWarpJumpStartMotion = szBuf;
	rkLoader.LoadString_e( "warp_end_motion", "", szBuf, MAX_PATH );
	m_szWarpEndMotion = szBuf;

	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_fWarpStartMotionRate = rkLoader.LoadFloat_e( "warp_start_motion_rate", FLOAT1 );
	m_fWarpJumpStartMotionRate = rkLoader.LoadFloat_e( "warp_jump_start_motion_rate", FLOAT1 );
	m_fWarpEndMotionRate = rkLoader.LoadFloat_e( "warp_end_motion_rate", FLOAT1 );
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.f );
	m_fEndJumpValue = rkLoader.LoadFloat_e( "end_jump_value", 0.1f );

	// Limit Angle
	m_fMaxAngle = rkLoader.LoadFloat_e( "angle_max", 45.0f );
	m_fMinAngle = rkLoader.LoadFloat_e( "angle_min", 45.0f );

	rkLoader.LoadString_e( "extra_guid_effect", "", szBuf, MAX_PATH );
	m_ExtraGuidEffect = szBuf;

	m_dwFireGuidDuration = rkLoader.LoadInt_e( "fire_guid_effect_duration", 0 );

	m_fAimRange = rkLoader.LoadFloat_e( "aim_target_range", 0.0f );
	int iCnt = rkLoader.LoadInt_e( "aim_info_count", 0 );
	m_vAimInfoList.clear();
	m_vAimInfoList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		AimInfo kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "aim%d_mark_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_szAimMark = szBuf;
		wsprintf_e( szKey, "Zero_aim%d_mark_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_szZeroAimMark = szBuf;
		wsprintf_e( szKey, "aim%d_time", i+1 );
		kInfo.m_dwAimTime = (DWORD)rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "zero_aim%d_time", i+1 );
		kInfo.m_dwZeroAimTime = (DWORD)rkLoader.LoadInt( szKey, 0 );
		
		wsprintf_e( szKey, "aim%d_attack", i+1 );
		LoadWeaponInfo( szKey, kInfo.m_AimWeaponInfoList, rkLoader );

		wsprintf_e( szKey, "zero_aim%d_attack", i+1 );
		LoadWeaponInfo( szKey, kInfo.m_ZeroAimWeaponInfoList, rkLoader );
		
		m_vAimInfoList.push_back(kInfo);
	}

	rkLoader.LoadString_e( "aim_fire_effect", "", szBuf, MAX_PATH );
	m_szAimFireEffect = szBuf;
	rkLoader.LoadString_e( "zero_aim_fire_effect", "", szBuf, MAX_PATH );
	m_szZeroAimFireEffect = szBuf;

	enum { FireBuffCount = 10, };
	for( int i=0; i < FireBuffCount; ++i )
	{
		wsprintf_e( szKey, "fire_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString szBuffName = szBuf;

		m_szFireBuffList.push_back( szBuf );
	}

	m_bVisibleAimMarkOnlyOwner = rkLoader.LoadBool_e( "visible_aim_mark_only_owner", false );
}

void ioAngleFire5Skill::LoadWeaponInfo( const char *szWeaponInfo,
										WeaponInfoList &rkWeaponInfoList,
										ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "%s_count", szWeaponInfo );
	int iAttackCount = rkLoader.LoadInt( szKey, 0 );
	for( int i=0; i<iAttackCount; ++i )
	{
		WeaponInfo kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "%s_type%d", szWeaponInfo, i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_resist", szWeaponInfo, i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_wounded", szWeaponInfo, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "%s_type%d_wounded_time", szWeaponInfo, i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s_type%d_wounded_loop_ani", szWeaponInfo, i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		rkWeaponInfoList.push_back( kInfo );
	}
}

ioSkill* ioAngleFire5Skill::Clone()
{
	return new ioAngleFire5Skill( *this );
}

bool ioAngleFire5Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	ClearData();

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

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar, g_SkillMaker.GetPlayStage() );
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

	pChar->AttachEffect( m_SkillEffectName );
	pChar->ClearReservedSliding();
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetCurMoveSpeed( 0.0f );
	pChar->SetJumpPower( 0.0f );
	pChar->SetCurJumpPowerAmt( 0.0f);
	pChar->SetGravityAmt( 0.0f );

	CheckSkillDesc( pChar );

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	m_iAimType = 0;
	m_szWarpTargetList.clear();

	SetSkillNameChatBubble( pChar );
	return true;
}

void ioAngleFire5Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
		{
			CheckAimTime();
			CheckKeyInput( pChar );
			CheckTarget( pChar, pStage );
			CheckVDir( pChar );
			UpdateAimMark( pChar, pStage );

			if( pChar->IsNeedProcess() )
			{
				if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
				{
					SetFireState( pChar );
					return;
				}
				else if( pChar->IsAttackKey() )
				{
					SetFireState( pChar );
					return;
				}
			}
		}
		break;
	case SS_FIRE:
		{
			if( COMPARE( m_iAimType, 0, (int)m_vAimInfoList.size() ) )
			{
				if( m_dwMotionEndTime < FRAMEGETTIME() )
					SetLoopState( pChar, pStage );
			}

			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
			}
		}
		break;
	case SS_WARP:
		{
			if( m_dwMotionEndTime < FRAMEGETTIME() )
			{
				SetWarpEndState( pChar );
			}
		}
		break;
	case SS_WARP_END:
		{
			if( m_dwMotionEndTime < FRAMEGETTIME() )
				m_SkillState = SS_END;
		}
		break;
	}
}

void ioAngleFire5Skill::CheckVDir( ioBaseChar *pChar )
{
	if( m_szTargetName.IsEmpty() )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )	return;

	D3DXVECTOR3 vPos1 = pChar->GetWorldPosition();
	D3DXVECTOR3 vPos2 = pTarget->GetWorldPosition();

	pChar->SetTargetRotToTargetPos( vPos2, true );

	m_vAttackDir = vPos2 - vPos1;
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	float fAngle = 0.0f;
	if( m_vAttackDir != vSightDir )
	{
		float fDotValue = D3DXVec3Dot( &m_vAttackDir, &vSightDir );
		fAngle = RADtoDEG( acosf(fDotValue) );
	}

	m_fCurMotionRate = 0.0f;
	m_fCurMotionRate = fAngle /FLOAT90;

	ioEntityGroup	*pGrp = pChar->GetGroup();
	ioAnimation		*pUp, *pCenter, *pDown;
	pUp = pCenter = pDown = NULL;

	if( pChar->IsJumpingSkillState() )
	{
		pUp = pGrp->GetLoopInPlay( m_JumpSkillUpLoopMotion );
		pCenter = pGrp->GetLoopInPlay( m_JumpSkillLoopMotion );
		pDown = pGrp->GetLoopInPlay( m_JumpSkillDownLoopMotion );
	}
	else
	{
		pUp = pGrp->GetLoopInPlay( m_SkillUpLoopMotion );
		pCenter = pGrp->GetLoopInPlay( m_SkillLoopMotion );
		pDown = pGrp->GetLoopInPlay( m_SkillDownLoopMotion );
	}
	if( !pUp || !pCenter || !pDown )
	{
		LOG.PrintTimeAndLog( 0, "ioAngleFire2Skill::CheckVDir - Motion Error" );
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

void ioAngleFire5Skill::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	EndPreDelayEffect( pChar );
	pChar->EndEffect( m_SkillEffectName, false );

	if( m_dwExtraGuidEffect != -1 )
	{
		pChar->EndEffect( m_dwExtraGuidEffect, false );
		m_dwExtraGuidEffect = -1;
	}

	EndCameraBuff( pChar );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCurMoveSpeed( 0.0f );

	m_SkillState = SS_NONE;
	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	DestroyAimMark( pChar );
}

bool ioAngleFire5Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_WARP:
	case SS_WARP_END:
		return false;
	case SS_FIRE:
		{
			if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
				return false;

			if( COMPARE( m_iAimType, 0, (int)m_vAimInfoList.size() ) )
				return false;
		}
		return true;
	case SS_END:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			return true;
		return false;
	}

	return true;
}

bool ioAngleFire5Skill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_WARP:
	case SS_WARP_END:
		return false;
	case SS_FIRE:
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;

		break;
	}

	return false;
}

void ioAngleFire5Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SNT_CHANGE:
		{
			m_dwAimCheckTime = FRAMEGETTIME();
			m_dwZeroAimCheckTime = 0;
			DestroyAimMark( pChar );

			rkPacket >> m_szTargetName;
		}
		break;
	case SNT_ATTACK:
		{
			rkPacket >> m_vAttackDir;
			rkPacket >> m_bZeroAimFire;
			rkPacket >> m_vAttackPos;
			rkPacket >> m_vTargetPos;
			rkPacket >> m_bTargetJump;
			SetFireState( pChar );
		}
		break;
	case SNT_WARP:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			SetWarpState( pChar, szTargetName );
		}
		break;
	case SNT_WARP_END:
		{
			SetWarpEndState( pChar, rkPacket );
		}
		break;
	case SNT_WARP_ADD:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			m_szWarpTargetList.push_back( szTargetName );
		}
	}
}

void ioAngleFire5Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioTraceWeaponType2	*pTrace2 = ToTraceWeaponType2( pWeapon );
	if( pTrace2 )
	{
		pTrace2->SetMoveDir( m_vAttackDir );
		pTrace2->SetTraceTarget( m_szTargetName );
	}

	ioTraceWeapon7 *pTrace7 = ToTraceWeapon7( pWeapon );
	if( pTrace7 )
		pTrace7->SetTraceTarget( m_szTargetName );
}

void ioAngleFire5Skill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	EndPreDelayEffect( pChar );

	m_SkillState = SS_LOOP;
	m_fCurMotionRate = 0.0f;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	if( pChar->IsJumpingSkillState() )
	{
		pGrp->SetLoopAni( m_JumpSkillUpLoopMotion, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpSkillDownLoopMotion, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpSkillLoopMotion, 0.0f, FLOAT1 );
	}
	else
	{
		pGrp->SetLoopAni( m_SkillUpLoopMotion, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_SkillDownLoopMotion, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_SkillLoopMotion, 0.0f, FLOAT1 );
	}
	if( pChar->IsNeedProcess() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_ExtraGuidEffect );
		if( pEffect )
		{
			m_dwExtraGuidEffect = pEffect->GetUniqueID();
		}
	}
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
	m_dwAimCheckTime = dwCurTime;
}

void ioAngleFire5Skill::SetFireState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DestroyAimMark( pChar );
	AttackSkillFire( pChar );

	ioPlayStage *pStage = pChar->GetCreator();
	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( pStage && pTarget )
	{
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		if( m_bZeroAimFire )
		{
			if( !m_szZeroAimFireEffect.IsEmpty() )
			{
				pTarget->AttachEffect( m_szZeroAimFireEffect );
			}
			else
			{
				if( COMPARE( m_iAimType, 0, (int)m_vAimInfoList.size() ) )
				{
					ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_vAimInfoList[m_iAimType].m_szZeroAimMark, vPos, vScale );
					if( pMapEffect )
						m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = pMapEffect->GetUniqueID();
				}
			}
		}
		else
		{
			if( !m_szAimFireEffect.IsEmpty() )
			{
				pTarget->AttachEffect( m_szAimFireEffect );
			}
		}
	}

	m_SkillState = SS_FIRE;
	
	int iFireBuffCount = m_szFireBuffList.size();
	if( iFireBuffCount != 0 )
	{
		for( int i = 0; i < iFireBuffCount; i++ )
			pChar->AddNewBuff( m_szFireBuffList[i], pChar->GetCharName(), "", NULL );
	}
	if( m_dwExtraGuidEffect != -1 )
	{
		pChar->EndEffect( m_dwExtraGuidEffect, false );
		m_dwExtraGuidEffect = -1;
	}

	m_vAttackPos = pChar->GetWorldPosition();
	if( pTarget )
		m_vTargetPos = pTarget->GetWorldPosition();
	else
		m_vTargetPos = m_vAttackPos;

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( m_vTargetPos.x, m_vTargetPos.z );
	if ( m_vTargetPos.y - fMapHeight >= m_fJumpHeightGap )
		m_bTargetJump =  true;
	else 
	{
		m_bTargetJump = false;
		if( fMapHeight > m_vTargetPos.y )
			m_vTargetPos.y = fMapHeight;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SNT_ATTACK;
		kPacket << m_vAttackDir;
		kPacket << m_bZeroAimFire;
		kPacket << m_vAttackPos;
		kPacket << m_vTargetPos;
		kPacket << m_bTargetJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAngleFire5Skill::SetWarpState( ioBaseChar *pChar, ioHashString szTargetName )
{
	if( !pChar ) return;

	m_SkillState = SS_WARP;
	m_szWarpTargetList.push_back( szTargetName );

	if( pChar->IsJumpingSkillState() )
		SetWarpAnimation( pChar, m_szWarpJumpStartMotion, m_fWarpJumpStartMotionRate );
	else
		SetWarpAnimation( pChar, m_szWarpStartMotion, m_fWarpStartMotionRate );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SNT_WARP;
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAngleFire5Skill::SetWarpEndState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	m_SkillState = SS_WARP_END;

	pChar->SetWorldPosition( m_vTargetPos );
	pChar->SetTargetRotToTargetPos( m_vAttackPos, true );
	if( m_bTargetJump )
		pChar->SetSkillNextJumpAmt( m_fEndJumpValue );

	int iCount = m_szWarpTargetList.size();
	for( int i = 0; i < iCount; i++ )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szWarpTargetList[i] );
		if( !pTarget )
			continue;
	
		pTarget->SetWorldPosition( m_vAttackPos );
		pTarget->SetTargetRotToTargetPos( m_vTargetPos, true );
	}
	SetWarpAnimation( pChar, m_szWarpEndMotion, m_fWarpEndMotionRate );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SNT_WARP_END;
		kPacket << iCount;
		for( int i = 0; i < iCount; i++ )
			kPacket << m_szWarpTargetList[i];

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAngleFire5Skill::SetWarpEndState( ioBaseChar *pChar, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	m_SkillState = SS_WARP_END;

	pChar->SetWorldPosition( m_vTargetPos );
	pChar->SetTargetRotToTargetPos( m_vAttackPos, true );
	if( m_bTargetJump )
		pChar->SetSkillNextJumpAmt( m_fEndJumpValue );

	int iCount;
	rkPacket >> iCount;	
	for( int i = 0; i < iCount; i++ )
	{
		ioHashString szTargetName;
		rkPacket >> szTargetName;

		ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
		if( !pTarget )
			return;
	
		pTarget->SetWorldPosition( m_vAttackPos );
		pTarget->SetTargetRotToTargetPos( m_vTargetPos, true );
	}
	SetWarpAnimation( pChar, m_szWarpEndMotion, m_fWarpEndMotionRate );
}

void ioAngleFire5Skill::SetWarpAnimation( ioBaseChar *pOwner, ioHashString szMotion, float fMotionRate )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iCurSkillAniID = pGrp->GetAnimationIdx( szMotion );
	if( iCurSkillAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fMotionRate );
	
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fMotionRate;
}

void ioAngleFire5Skill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	WeaponInfoList vWeaponInfoList;

	if( !COMPARE( m_iAimType, 0, (int)m_vAimInfoList.size() ) )
	{
		m_SkillState = SS_END;
		return;
	}

	if( m_bZeroAimFire )
		vWeaponInfoList = m_vAimInfoList[m_iAimType].m_ZeroAimWeaponInfoList;
	else
		vWeaponInfoList = m_vAimInfoList[m_iAimType].m_AimWeaponInfoList;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage ) return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( pTarget )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
		if( pMapEffect )
			pMapEffect->SetWorldPosition( pTarget->GetMidPositionByRate() );
	}
	else
	{
		pStage->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
	}

	if( m_bRefreshFireTimeList )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		if( pTarget )
			vPos = pTarget->GetWorldPosition();

		++m_iAimType;

		pChar->RefreshFireTimeList( iAniID,
									vWeaponInfoList,
									FTT_SKILL_ATTACK,
									fTimeRate,
									dwPreDelay, false, vPos );
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
		m_dwAttackEndTime = dwCurTime;

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
		if( pChar->IsJumpingSkillState() )
			pGrp->SetActionAni( m_JumpSkillUpMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		else
			pGrp->SetActionAni( m_SkillUpMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else								// center & down
	{
		if( pChar->IsJumpingSkillState() )
			pGrp->SetActionAni( m_JumpSkillDownMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		else
			pGrp->SetActionAni( m_SkillDownMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

bool ioAngleFire5Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioAngleFire5Skill::CheckAimTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !COMPARE( m_iAimType, 0, (int)m_vAimInfoList.size() ) )
	{
		m_SkillState = SS_END;
		return;	
	}

	if( m_dwAimCheckTime > 0 &&
		m_dwAimCheckTime + m_vAimInfoList[m_iAimType].m_dwAimTime < dwCurTime )
	{
		m_dwAimCheckTime = 0;
		m_dwZeroAimCheckTime = dwCurTime;

		if( !m_szTargetName.IsEmpty() )
			m_bZeroAimFire = true;
		else
			m_bZeroAimFire = false;
	}
	else if( m_dwZeroAimCheckTime > 0 &&
			 m_dwZeroAimCheckTime + m_vAimInfoList[m_iAimType].m_dwZeroAimTime < dwCurTime )
	{
		m_dwAimCheckTime = dwCurTime;
		m_dwZeroAimCheckTime = 0;
		m_bZeroAimFire = false;
	}
}

void ioAngleFire5Skill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsJumpKey() )
	{
		m_bJumpKey = true;
		m_bDefenseKey = false;
	}
	else if( pOwner->IsDefenseKey() )
	{
		m_bJumpKey = false;
		m_bDefenseKey = true;
	}
	else
	{
		m_bJumpKey = false;
		m_bDefenseKey = false;
	}
}

void ioAngleFire5Skill::CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )	return;
	if( !pChar )	return;


	if( !pChar->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fAimRange*m_fAimRange;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	
	m_vTargetInfoList.clear();
	int iSize = rkTargetList.size();
	m_vTargetInfoList.reserve(iSize);
	
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( pTarget == pChar )
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 
			continue;
		
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		TargetInfo	kInfo;
		kInfo.m_TargetName = pTarget->GetCharName();
		kInfo.m_fLenth = fDistSq;
		
		float fEnableDistSq = m_fAimRange*m_fAimRange;
		if( fDistSq < fEnableDistSq )	// Enemy
		{
			m_vTargetInfoList.push_back(kInfo);
		}
	}

	if( m_szTargetName.IsEmpty() )
	{
		int iCnt = m_vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			float fDistSq = m_vTargetInfoList[i].m_fLenth;
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				fPrevDistSq = fDistSq;
				m_szTargetName = m_vTargetInfoList[i].m_TargetName;
			}
		}

		if( !m_szTargetName.IsEmpty() )
		{
			m_dwAimCheckTime = FRAMEGETTIME();
			m_dwZeroAimCheckTime = 0;
			DestroyAimMark( pChar );
			m_bZeroAimFire = false;

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SNT_CHANGE;
				kPacket << m_szTargetName;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else if( !m_vTargetInfoList.empty() )
	{
		int iCurTarget = -1;

		int iCnt = m_vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			if( m_szTargetName == m_vTargetInfoList[i].m_TargetName )
				iCurTarget = i;
		}

		if( iCurTarget != -1 )
		{
			bool bTargetChange = false;
			if( m_bJumpKey )
			{
				iCurTarget++;
				bTargetChange = true;
				if( iCurTarget >= iCnt )
					iCurTarget = 0;
			}
			else if( m_bDefenseKey )
			{
				bTargetChange = true;
				iCurTarget--;
				if( iCurTarget < 0 )
					iCurTarget = iCnt-1;
			}

			m_szTargetName = m_vTargetInfoList[iCurTarget].m_TargetName;

			if( bTargetChange )
			{
				m_dwAimCheckTime = FRAMEGETTIME();
				m_dwZeroAimCheckTime = 0;
				m_bZeroAimFire = false;
				DestroyAimMark( pChar );

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SNT_CHANGE;
					kPacket << m_szTargetName;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		else
			m_szTargetName.Clear();
	}
	else
		m_szTargetName.Clear();
}

void ioAngleFire5Skill::UpdateAimMark( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if( m_bVisibleAimMarkOnlyOwner && !pChar->IsNeedProcess() )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( !pTarget )
	{
		DestroyAimMark( pChar );
		return;
	}

	if( !COMPARE( m_iAimType, 0, (int)m_vAimInfoList.size() ) )
	{
		m_SkillState = SS_END;
		return;
	}

	if( m_dwAimCheckTime > 0 )
	{
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		if( m_vAimInfoList[m_iAimType].m_dwAimMarkID == -1 )
		{			
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_vAimInfoList[m_iAimType].m_szAimMark, vPos, vScale );
			if( pMapEffect )
				m_vAimInfoList[m_iAimType].m_dwAimMarkID = pMapEffect->GetUniqueID(); 
		}
		else if( m_vAimInfoList[m_iAimType].m_dwAimMarkID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_vAimInfoList[m_iAimType].m_dwAimMarkID );
			if( pMapEffect )
				pMapEffect->SetWorldPosition( vPos );
		}

		if( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID != -1 )
		{
			pStage->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
			m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = -1;
		}
	}
	else if( m_dwZeroAimCheckTime > 0 )
	{
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		if( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID == -1 )
		{
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_vAimInfoList[m_iAimType].m_szZeroAimMark, vPos, vScale );
			if( pMapEffect )
				m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = pMapEffect->GetUniqueID(); 
		}
		else if( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
			if( pMapEffect )
				pMapEffect->SetWorldPosition( vPos );
		}

		if( m_vAimInfoList[m_iAimType].m_dwAimMarkID != -1 )
		{
			pStage->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwAimMarkID );
			m_vAimInfoList[m_iAimType].m_dwAimMarkID = -1;
		}
	}
}

void ioAngleFire5Skill::DestroyAimMark( ioBaseChar *pChar )
{
	if( !pChar )	return;

	if( m_bVisibleAimMarkOnlyOwner && !pChar->IsNeedProcess() )
		return;

	if( !COMPARE( m_iAimType, 0, (int)m_vAimInfoList.size() ) )
	{
		m_SkillState = SS_END;
		return;
	}

	if( m_vAimInfoList[m_iAimType].m_dwAimMarkID != -1 )
	{
		pChar->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwAimMarkID );
		m_vAimInfoList[m_iAimType].m_dwAimMarkID = -1;
	}

	if( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID != -1 )
	{
		pChar->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
		m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = -1;
	}
}

void ioAngleFire5Skill::SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock )
{
	if( !pOwner ) return;
	if( !pTarget ) return;
	if( pOwner->GetTeam() == pTarget->GetTeam() ) return;

	ioHashString szTargetName = pTarget->GetCharName();
	if( m_SkillState == SS_FIRE )
	{
		SetWarpState( pOwner, szTargetName );
	}
	else if( m_SkillState == SS_WARP )
	{
		m_szWarpTargetList.push_back( szTargetName );

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SNT_WARP_ADD;
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}