

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioJumpTargetShotSkill.h"

#include "WeaponDefine.h"

ioJumpTargetShotSkill::ioJumpTargetShotSkill()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
}

ioJumpTargetShotSkill::ioJumpTargetShotSkill( const ioJumpTargetShotSkill &rhs )
: ioNormalSkill( rhs ),
 m_ShootAttribute( rhs.m_ShootAttribute ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_fSkillGravityRateOnDownMove( rhs.m_fSkillGravityRateOnDownMove ),
 m_fSkillEndDownDist( rhs.m_fSkillEndDownDist ),
 m_fSkillEndJumpPower( rhs.m_fSkillEndJumpPower )
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
}

ioJumpTargetShotSkill::~ioJumpTargetShotSkill()
{
}

void ioJumpTargetShotSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

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

	m_fSkillGravityRateOnDownMove = rkLoader.LoadFloat_e( "skill_gravity_rate_on_down_move", FLOAT1 );
	m_fSkillEndDownDist = rkLoader.LoadFloat_e( "skill_end_down_dist", FLOAT1 );
	m_fSkillEndJumpPower = rkLoader.LoadFloat_e( "skill_end_jump_power", FLOAT1 );

	LoadAttackAttribute( "shoot_attack", m_ShootAttribute, rkLoader );
}

ioSkill* ioJumpTargetShotSkill::Clone()
{
	return new ioJumpTargetShotSkill( *this );
}

bool ioJumpTargetShotSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	m_dwShootEnableTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_dwFireStartTime = 0;

	m_bShooted = false;

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

void ioJumpTargetShotSkill::OnSkillEnd( ioBaseChar *pChar )
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

	m_dwPreDelayStartTime = 0;

	m_State = SS_NONE;
}

void ioJumpTargetShotSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

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
			SetUpMoveState( pChar, pStage );
		}
		break;
	case SS_UP_MOVE:
		{
			float fCurheight = pChar->GetWorldPosition().y;
			if( m_fCheckHeight > fCurheight )
			{
				SetDownMoveState( pChar, pStage );
				return;
			}

			m_fCheckHeight = pChar->GetWorldPosition().y;
		}
		break;
	case SS_DOWN_MOVE:
		{
			ProcessDownMoveState( pChar, pStage );
		}
		break;
	}
}

bool ioJumpTargetShotSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioJumpTargetShotSkill::SetNormalSkill( ioBaseChar *pChar )
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

	D3DXVECTOR3 vEffectDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_SkillMapEffectName, vEffectDir );
	if( pMapEffect )
	{
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, iCurSkillAniID, m_AttackAttribute.m_vWeaponInfoList, FTT_SKILL_ATTACK, m_AttackAttribute.m_fAttackAniRate, m_AttackAttribute.m_dwPreDelay );

	if( m_State != SS_END )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioJumpTargetShotSkill::SetWaitState( ioBaseChar *pChar )
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

void ioJumpTargetShotSkill::SetUpMoveState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_UP_MOVE;
	m_TargetState = TS_DELAY;

	// CharMove
	m_dwActionStartTime = FRAMEGETTIME();

	pChar->SetJumpPower( m_fCharJumpPower );

	m_fCheckHeight = pChar->GetWorldPosition().y;
}

void ioJumpTargetShotSkill::SetDownMoveState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( pChar->IsNeedProcess() && !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );	
	}

	m_fCheckHeight = pChar->GetWorldPosition().y;

	m_State = SS_DOWN_MOVE;

	m_dwShootTime = 0;
}

void ioJumpTargetShotSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int iSycState;
	rkPacket >> iSycState;

	switch( iSycState )
	{
	case SYNC_ATTACK:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vFirePos;
			D3DXVECTOR3 vFireDir;
			rkPacket >> vPos;
			rkPacket >> qtRot;
			rkPacket >> vFirePos;
			rkPacket >> vFireDir;

			pChar->SetWorldPosition( vPos );
			pChar->SetWorldOrientation( qtRot );

			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
			pChar->SkillFireExplicit( m_ShootAttribute.m_vWeaponInfoList[0], GetName(), m_dwWeaponBaseIndex, vFirePos, vFireDir );
		}
		break;
	case SYNC_END:
		{
			m_State = SS_END_IN_AIR;
		}
		break;
	}
}

bool ioJumpTargetShotSkill::IsEndState() const
{
	if( m_State == SS_END || m_State == SS_END_IN_AIR )
		return true;

	return false;
}

bool ioJumpTargetShotSkill::IsAttackEndState() const
{
	if( m_State == SS_END || m_State == SS_END_IN_AIR )
		return true;
	return false;
}

void ioJumpTargetShotSkill::SetEndState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_State = SS_END;
	DWORD dwCurTime = FRAMEGETTIME();

	pChar->DestroyMagicCircle();

	if( m_SkillEndMotion.IsEmpty() )
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

	m_dwMotionStartTime = m_dwFireStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;
}

void ioJumpTargetShotSkill::ProcessDownMoveState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	if( !pChar->IsNeedProcess() )
		return;

	if( CheckLanding( pChar ) )
	{
		SetEndState( pChar );
		return;
	}

	float fDist = m_fCheckHeight - pChar->GetWorldPosition().y;
	fDist = max( fDist, 0.0f );

	if( m_fSkillEndDownDist > 0.0f && m_fSkillEndDownDist <= fDist )
	{
		m_State = SS_END_IN_AIR;

		pChar->DestroyMagicCircle();

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << (int)SYNC_END;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}

	if( pChar->IsNeedProcess() )
	{
		float fValue = 0.0f;
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
		if( pUpInfo )
			fValue = pUpInfo->GetValue(pStage);

		float fCurRange = m_fCircleRange + fValue;

		if( pChar->CheckMagicCircle() )
		{
			pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, false );
		}

		if( m_dwShootEnableTime < FRAMEGETTIME()  && pChar->IsAttackKey() )
		{
			ioEntityGroup *pGrp = pChar->GetGroup();
			if( pGrp )
			{
				int iAniID = pGrp->GetAnimationIdx( m_ShootAttribute.m_AttackAnimation );
				if( iAniID == -1 )
				{
					return;
				}

				float fAniRate = m_ShootAttribute.m_fAttackAniRate;
				DWORD dwPreDelay = m_ShootAttribute.m_dwPreDelay;

				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );

				m_dwShootTime = m_dwShootEnableTime = FRAMEGETTIME();
				m_dwShootTime += pGrp->GetAnimationEventTime( iAniID, "fire_s" ) * fAniRate;
				m_dwShootEnableTime += pGrp->GetAnimationEventTime( iAniID, "key_reserve" ) * fAniRate;
			}
		}
		else if( m_dwShootTime > 0 && m_dwShootTime < FRAMEGETTIME() )
		{
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
			D3DXVECTOR3 vDir = vTargetPos - vPos;
			D3DXVec3Normalize( &vDir, &vDir );
			pChar->SkillFireExplicit( m_ShootAttribute.m_vWeaponInfoList[0], GetName(), m_dwWeaponBaseIndex, vPos, vDir );
			m_dwShootTime = 0;

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << (int)SYNC_ATTACK;
				kPacket << pChar->GetWorldPosition();
				kPacket << pChar->GetWorldOrientation();
				kPacket << vPos;
				kPacket << vDir;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

float ioJumpTargetShotSkill::GetSkillGravityRate()
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		if( m_State == SS_DOWN_MOVE )
		{
			return m_fSkillGravityRateOnDownMove;
		}
		else
		{
			return m_fSkillGravityRate;
		}
	case SUT_DOWN:
		if( m_fSkillGravityDownRate > 0.0f )
			return m_fSkillGravityDownRate;
		return m_fSkillGravityRate;
	case SUT_JUMP:
		if( m_fSkillGravityJumpRate > 0.0f )
			return m_fSkillGravityJumpRate;
		return m_fSkillGravityRate;
	}

	return m_fSkillGravityRate;
}

bool ioJumpTargetShotSkill::IsProtected( int iDefenseBreakType ) const
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
		case SS_NONE:
		case SS_WAIT:
		case SS_UP_MOVE:
			return true;
		}
	}

	return false;
}

bool ioJumpTargetShotSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioJumpTargetShotSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioJumpTargetShotSkill::CheckLanding( ioBaseChar *pChar )
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

void ioJumpTargetShotSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon )
		return;

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
