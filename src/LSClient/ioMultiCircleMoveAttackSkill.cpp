

#include "stdafx.h"

#include "ioMultiCircleMoveAttackSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

ioMultiCircleMoveAttackSkill::ioMultiCircleMoveAttackSkill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwCircleCheckTime = 0;

	m_dwAttackCheckTime = 0;
	m_dwLoopStartTime = 0;
}

ioMultiCircleMoveAttackSkill::ioMultiCircleMoveAttackSkill( const ioMultiCircleMoveAttackSkill &rhs )
	: ioNormalSkill( rhs ),
	m_fCircleRange( rhs.m_fCircleRange ),
	m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	m_fCircleOffSet( rhs.m_fCircleOffSet ),
	m_ReadyCircle( rhs.m_ReadyCircle ),
	m_TargetCircleEffect( rhs.m_TargetCircleEffect ),
	m_SkillEndMotion( rhs.m_SkillEndMotion ),
	m_fEndMotionRate( rhs.m_fEndMotionRate ),
	m_StartEffect( rhs.m_StartEffect ),
	m_EndEffect( rhs.m_EndEffect ),
	m_RedEffect( rhs.m_RedEffect ),
	m_BlueEffect( rhs.m_BlueEffect ),
	m_dwCircleCheckDuration( rhs.m_dwCircleCheckDuration ),
	m_RedGuidCircle( rhs.m_RedGuidCircle ),
	m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
	m_bUseAttack( rhs.m_bUseAttack ),
	m_fAttackOffset( rhs.m_fAttackOffset ),
	m_fAttackHeightOffset( rhs.m_fAttackHeightOffset ),
	m_bEnableControl( rhs.m_bEnableControl ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration ),
	m_LoopAnimation( rhs.m_LoopAnimation ),
	m_dwLoopTime( rhs.m_dwLoopTime ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fMoveRange( rhs.m_fMoveRange ),
	m_GaugeUseType( rhs.m_GaugeUseType ),
	m_bHandMeshOnDelayAni( rhs.m_bHandMeshOnDelayAni ),
	m_createWeaponTpye( rhs.m_createWeaponTpye ),
	m_stActoinAni( rhs.m_stActoinAni ),
	m_fActionAniRate( rhs.m_fActionAniRate ),
	m_fWarpFindLength( rhs.m_fWarpFindLength ),
	m_dwFireDelayTime( rhs.m_dwFireDelayTime ),
	m_stLoopAni( rhs.m_stLoopAni ),
	m_fLoopAniRate( rhs.m_fLoopAniRate ),
	m_fFireWeaponOffSet( rhs.m_fFireWeaponOffSet ),
	m_dwFireWeaponIndex( rhs.m_dwFireWeaponIndex ),
	m_nMaxMoveCnt( rhs.m_nMaxMoveCnt )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwCircleCheckTime = 0;

	m_dwAttackCheckTime = 0;
	m_dwLoopStartTime = 0;
	m_bCreatedWeapon = false;
}

ioMultiCircleMoveAttackSkill::~ioMultiCircleMoveAttackSkill()
{
}

void ioMultiCircleMoveAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	m_bEnableControl = rkLoader.LoadBool_e( "enable_control", false );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwSkillProtectTime    = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_dwLoopProtectDuration = (DWORD)rkLoader.LoadInt_e( "loop_protect_duration", 0 );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "area_weapon_move_speed", 0.0f );
	m_fMoveRange = rkLoader.LoadFloat_e( "area_weapon_move_range", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "target_circle_effect", "", szBuf, MAX_PATH );
	m_TargetCircleEffect = szBuf;

	rkLoader.LoadString_e( "red_guid_circle", "", szBuf, MAX_PATH );
	m_RedGuidCircle = szBuf;
	rkLoader.LoadString_e( "blud_guid_circle", "", szBuf, MAX_PATH );
	m_BlueGuidCircle = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_dwCircleCheckDuration = (DWORD)rkLoader.LoadInt_e( "magic_circle_view", 0 );

	// Attack
	m_bUseAttack = rkLoader.LoadBool_e( "use_attack", false );
	m_fAttackOffset = rkLoader.LoadFloat_e( "attack_offset", 0.0f );
	m_fAttackHeightOffset = rkLoader.LoadFloat_e( "attack_height_offset", 0.0f );

	m_GaugeUseType = (GaugeUseType)rkLoader.LoadInt_e( "gauge_use_type", GUT_ON_KEY_INPUT );

	m_bHandMeshOnDelayAni = rkLoader.LoadBool_e( "hand_mesh_on_delay_ani", false );

	m_createWeaponTpye = (CreateWeaponType)rkLoader.LoadInt_e( "create_weapon_type", CWT_NONE );

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString( "skill_action_motion", "", szBuf, MAX_PATH );
	m_stActoinAni = szBuf;
	m_fActionAniRate = rkLoader.LoadFloat( "skill_action_motion_rate", FLOAT1 );

	rkLoader.LoadString( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_stLoopAni = szBuf;
	m_fLoopAniRate = rkLoader.LoadFloat( "skill_loop_motion_rate", FLOAT1 );

	m_fWarpFindLength = rkLoader.LoadFloat( "warp_find_length", 0.0f);

	m_dwFireDelayTime = rkLoader.LoadInt( "fire_delay_time", 3000 );

	m_fFireWeaponOffSet = rkLoader.LoadFloat( "fire_weapon_height_offset", 500.0 );

	m_dwFireWeaponIndex = rkLoader.LoadInt( "fire_weapon_index", 0);

	m_nMaxMoveCnt = rkLoader.LoadInt( "max_move_cnt", 0);
}


ioSkill* ioMultiCircleMoveAttackSkill::Clone()
{
	return new ioMultiCircleMoveAttackSkill( *this );
}

bool ioMultiCircleMoveAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_bCreatedWeapon = false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

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

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	pChar->SetSkillProtection();

	m_dwProtectTime = 0;
	m_dwLoopStartTime = 0;
	m_dwAttackCheckTime = FRAMEGETTIME();

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_vMoveDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	SetWaitState( pChar, false );

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	if( m_GaugeUseType == GUT_ON_SKILL_USE || m_GaugeUseType == GUT_BOTH )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_nCurMoveCnt = 0;

	return true;
}

void ioMultiCircleMoveAttackSkill::OnSkillEnd( ioBaseChar *pChar )
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
	m_State = SS_NONE;
}

void ioMultiCircleMoveAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	DWORD dwCurTime = FRAMEGETTIME();
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
		fValue = pUpInfo->GetValue(pStage);

	float fCurRange = m_fCircleRange + fValue;
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_State )
	{
	case SS_WAIT:
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );

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
					SetActionState( pChar, true );
				}
			}
		}
		break;
	case SS_ACTION:
		if( pChar->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
		{
			m_bWaitFire = FindEnemy( pChar );
			if( m_bWaitFire )
				SetFireWaitState( pChar, true );
			else
			{
				if( m_nCurMoveCnt >= m_nMaxMoveCnt )
					SetEndState( pChar, true );
				else
					SetWaitState( pChar, true );
			}
			return;
		}

		break;
	case SS_FIRE_WAIT:
		if( pChar->CheckMagicCircle() )
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
		}

		if( pChar->IsNeedProcess() && m_dwFireTime < dwCurTime )
		{
			if( m_bWaitFire == false )
			{
				if( pChar->IsNeedProcess() )
				{
					if( pChar->IsAttackKey() ||
						pChar->IsDefenseKey() ||
						pChar->IsJumpKey() ||
						pChar->GetSkillKeyInput() == iSlot )
					{
						SetActionState( pChar, true );
					}
				}
				return;
			}

			m_bWaitFire = false;
			//Fire
			FireWeapon( pChar, true );

			if( m_nCurMoveCnt >= m_nMaxMoveCnt )
			{
				SetEndState( pChar, true );
			}
		}
		break;
	//case SS_LOOP:
	//	{
	//		if( m_dwLoopStartTime > 0 )
	//		{
	//			//CheckControl( pChar );
	//			//ProcessControl( pChar, pStage );
	//		}

	//		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < dwCurTime )
	//		{
	//			m_dwLoopStartTime = 0;
	//			SetEndState( pChar );
	//		}
	//	}
	//	break;
	case SS_END:
		break;
	}
}


void ioMultiCircleMoveAttackSkill::FireWeapon( ioBaseChar *pOwner, bool bSendNet )
{
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + D3DXVECTOR3( 0.f, m_fFireWeaponOffSet, 0.f );

	//m_dwFireWeaponIndex

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	CreateMagicCircle( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;

	kFireTime.iAttributeIdx = m_dwFireWeaponIndex;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	pOwner->IncreaseWeaponIndexBase();
	kFireTime.dwWeaponIdx = pOwner->GetWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

	D3DXVECTOR3 vTargetDir = m_vFireDestPos - vPos;
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	m_vAttackDir = vTargetDir;

	pOwner->ExtendFireExplicit( kFireTime, vPos, vTargetDir, "" );

	if( pOwner->IsNeedProcess() && bSendNet )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();

		kPacket << SS_FIRE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vFireDestPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioMultiCircleMoveAttackSkill::CreateMagicCircle( ioBaseChar *pOwner )
{
	if( !pOwner->CheckMagicCircle() )
	{
		// 팀 체크
		if( pOwner->GetTeam() == TEAM_RED )
		{
			pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
		}
		else if( pOwner->GetTeam() == TEAM_BLUE )
		{
			pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
		}
		else
		{
			if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			{
				pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
			}
			else
			{
				pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
			}
		}
	}
}

bool ioMultiCircleMoveAttackSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioMultiCircleMoveAttackSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = FRAMEGETTIME();
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = m_dwActionStartTime = FRAMEGETTIME();
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->AttachEffect( m_StartEffect );

	if( m_State != SS_END &&
		( m_GaugeUseType == GUT_ON_KEY_INPUT || m_GaugeUseType == GUT_BOTH ) )
	{
		if( GetOwnerItem() )
		{
			pChar->ReduceNeedGaugeBySkill( this, GetOwnerItem()->GetMaxSkillGauge( pChar->GetCharName() ) );
			m_bReduceGauge = true;
		}
	}
}

void ioMultiCircleMoveAttackSkill::SetFireWaitState( ioBaseChar *pOwner, bool bSendNet )
{
	m_State = SS_FIRE_WAIT;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireTime = dwCurTime + m_dwFireDelayTime;

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_stLoopAni );
	if( iCurSkillAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fLoopAniRate );

	m_bWaitFire = true;

	if( pOwner->IsNeedProcess() && bSendNet )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();

		kPacket << SS_FIRE_WAIT;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultiCircleMoveAttackSkill::SetWaitState( ioBaseChar *pChar, bool bSendNet )
{
	m_State = SS_WAIT;
	m_dwPreDelayStartTime = 0;

	if( m_dwCircleCheckDuration > 0 )
		m_dwCircleCheckTime = FRAMEGETTIME();
	else
		m_dwCircleCheckTime = 0;

	CreateMagicCircle( pChar );

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

	if( m_bHandMeshOnDelayAni )
	{
		if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
		{
			pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
			m_bSetHandMesh = true;
		}
	}

	if( bSendNet && pChar->IsNeedProcess() )
	{
		//SendPacket
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();

			kPacket << SS_WAIT;
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMultiCircleMoveAttackSkill::SetActionState( ioBaseChar *pChar, bool bSendNet )
{
	m_State = SS_ACTION;

	m_dwCircleCheckTime = 0;

	float fCurRange = m_fCircleRange;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

	if( pChar->IsNeedProcess() )
	{
		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
		pChar->SetWorldPosition( m_vCirclePos );
	}

	ioEntityGroup *pGrp = pChar->GetGroup();


	int iCurSkillAniID = pGrp->GetAnimationIdx( m_stActoinAni );
	if( iCurSkillAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fActionAniRate );

	m_dwMotionStartTime = FRAMEGETTIME();
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fActionAniRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;

	pChar->DestroyMagicCircle();

	m_nCurMoveCnt++;

	if( pChar->IsNeedSendNetwork() && bSendNet )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SS_ACTION;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioMultiCircleMoveAttackSkill::FindEnemy(ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	ioHashString szTarget;
	float fCurGap = 99999.0f;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == pOwner->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		//if( !IsEnableTargetState( pTarget ) )
			//continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRange = D3DXVec3Length( &vDiff );

		// 각도 체크
		//if( fCurAngle != 0.0f )
		//{
		//	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		//	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		//	D3DXVec3Normalize( &vDir, &vDir );
		//	D3DXVECTOR3 vDiff2 = vDiff;
		//	vDiff2.y = 0.0f;
		//	D3DXVec3Normalize( &vDiff2, &vDiff2 );

		//	float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
		//	if( fCurValue < fHalfValue )
		//		continue;
		//}

		//// 높이 체크
		//float fHeightGap = vTargetPos.y - vCurPos.y;
		//if( fHeightGap > 0.0f && fHeightGap > m_CurFly.m_fTargetUpHeight )
		//	continue;

		/*if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurFly.m_fTargetDownHeight )
			continue;*/

		// 거리 체크
		if( fDiffRange < m_fWarpFindLength && fCurGap > fDiffRange )
		{
			
			fCurGap = fDiffRange;
			szTarget = pTarget->GetCharName();
		}
	}

	if( !szTarget.IsEmpty() )
	{
		//m_vFireDir
		ioBaseChar* pTagetChar = pOwner->GetBaseChar(szTarget);
		if( pTagetChar )
			m_vFireDestPos = pTagetChar->GetMidPositionByRate();
		else
			m_vFireDestPos = pOwner->GetMidPositionByRate();
		return true;
	}

	return false;
}


void ioMultiCircleMoveAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;

	rkPacket >> vPos;
	rkPacket >> qtRot;

	switch( iSyncState )
	{
	case SS_WAIT:
		SetWaitState( pChar, false );
		break;
	case SS_ACTION:
		SetActionState( pChar, false );
		break;
	case SS_FIRE_WAIT:
		SetFireWaitState( pChar, false );
		break;
	case SS_FIRE:
		rkPacket >> m_vFireDestPos;
		FireWeapon( pChar, false );
		break;
	case SS_END:
		SetEndState( pChar, false );
		break;
	}

	pChar->SetWorldPosition( vPos );
	pChar->SetTargetRot( qtRot );
	
}

bool ioMultiCircleMoveAttackSkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

void ioMultiCircleMoveAttackSkill::SetEndState( ioBaseChar *pChar, bool bSendNet )
{
	m_State = SS_END;

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	if( m_dwTargetCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );
		m_dwTargetCircleEffect = -1;
	}

	pChar->DestroyMagicCircle();

	//pChar->SetState( CS_DELAY, false );

	if( pChar->IsNeedProcess() && bSendNet )
	{
		if( pChar->IsNeedSendNetwork() && bSendNet )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();

			kPacket << SS_END;
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	/*if( m_SkillEndMotion.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndMotionRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->AttachEffect( m_EndEffect );*/
}

bool ioMultiCircleMoveAttackSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > dwCurTime )
		return true;

	switch( m_State )
	{
	case SS_NONE:
	case SS_WAIT:
	case SS_END:
		return false;
	case SS_ACTION:
		return true;
	}

	return false;
}

bool ioMultiCircleMoveAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioMultiCircleMoveAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}


void ioMultiCircleMoveAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon )
	{
		pWeapon->SetMoveDir( m_vAttackDir );
	}
}