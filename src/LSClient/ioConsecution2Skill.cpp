

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"
#include "ioPlayStage.h"

#include "ioConsecution2Skill.h"

ioConsecution2Skill::ioConsecution2Skill()
{
	ClearData();
}

ioConsecution2Skill::ioConsecution2Skill( const ioConsecution2Skill &rhs )
: ioAttackSkill( rhs ),
 m_szTargetMarker( rhs.m_szTargetMarker ),
 m_szResultMarker( rhs.m_szResultMarker ),
 m_fMarkerOffset( rhs.m_fMarkerOffset ),
 m_szSkillLoopMotion( rhs.m_szSkillLoopMotion ), 
 m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_SkillMotionFr( rhs.m_SkillMotionFr ),
 m_SkillMotionBk( rhs.m_SkillMotionBk ),
 m_SkillMotionLt( rhs.m_SkillMotionLt ),
 m_SkillMotionRt( rhs.m_SkillMotionRt ),
 m_vWeaponFireInfoList( rhs.m_vWeaponFireInfoList ),
 m_fSkillMotionRate( rhs.m_fSkillMotionRate )
{
	ClearData();
}

ioConsecution2Skill::~ioConsecution2Skill()
{
}

void ioConsecution2Skill::ClearData()
{
	m_SkillState = SS_NONE;

	m_iWeaponFireCnt = 0;

	m_bMotionSetted = false;
	m_bLoopMotionSetted = false;
	m_bFirstMoveMotion = true;

	m_bSetHandMesh = false;

	m_dwLoopStartTime = 0;

	m_dwTargetMarkerID = -1;
}

void ioConsecution2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "target_marker", "", szBuf, MAX_PATH );
	m_szTargetMarker = szBuf;

	rkLoader.LoadString_e( "result_target_marker", "", szBuf, MAX_PATH );
	m_szResultMarker = szBuf;
	m_fMarkerOffset = rkLoader.LoadFloat_e( "target_marker_offset", 0.0f );

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szSkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate	= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration		= (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	rkLoader.LoadString_e( "skill_motion_fr", "", szBuf, MAX_PATH );
	m_SkillMotionFr = szBuf;

	rkLoader.LoadString_e( "skill_motion_bk", "", szBuf, MAX_PATH );
	m_SkillMotionBk = szBuf;

	rkLoader.LoadString_e( "skill_motion_lt", "", szBuf, MAX_PATH );
	m_SkillMotionLt = szBuf;

	rkLoader.LoadString_e( "skill_motion_rt", "", szBuf, MAX_PATH );
	m_SkillMotionRt = szBuf;

	m_fSkillMotionRate = rkLoader.LoadFloat_e( "skill_motion_rate", FLOAT1 );

	int iFireCnt = rkLoader.LoadInt_e( "weapon_fire_cnt", 0 );
	m_vWeaponFireInfoList.clear();
	m_vWeaponFireInfoList.reserve( iFireCnt );

	for( int i=0; i < iFireCnt; ++i )
	{
		WeaponFireInfo kFireInfo;
		DWORD dwFireTime = 0;
		float fFireAngle = 0.0f;

		wsprintf_e( szKey, "weapon_fire_time%d", i+1 );
		dwFireTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "weapon_fire_angle%d", i+1 );
		fFireAngle = rkLoader.LoadFloat( szKey, 0.0f );

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
}

ioSkill* ioConsecution2Skill::Clone()
{
	return new ioConsecution2Skill( *this );
}

void ioConsecution2Skill::AttackSkillFire( ioBaseChar *pChar )
{
	m_SkillState = SS_ACTION;
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->AttachEffect( m_SkillEffectName );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, 0 );
	pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem() ) );

	if( pChar )
	{
		m_iWeaponFireCnt = 0;
		m_bMotionSetted = true;
		m_bFirstMoveMotion = true;

		pChar->ChangeDirectionExplicitIfDirChanged( true );
		m_StartDirKey = pChar->GetCurDirKey();
	}
}

bool ioConsecution2Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	ClearData();

	m_dwAttackEndTime = 0;
	m_dwLoopStartTime = 0;

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
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
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

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioConsecution2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();

			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;

				pChar->SetSkillMoveEnable( true, false );
				EndPreDelayEffect( pChar );
				SetSkillLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				AttackSkillFire( pChar );

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SSC_START_FIRE;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			CheckMoveAniState( pChar );
		}
		break;
	case SS_ACTION:
		{
			CheckMoveAniState( pChar );
			ProcessAttack( pChar );
		}
		break;
	}

	if( m_dwTargetMarkerID != -1 && pChar->IsNeedProcess() )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos += ( vDir * m_fMarkerOffset );
		vPos.y = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false );

		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwTargetMarkerID );
		if( pMapEffect )
			pMapEffect->SetWorldPosition( vPos );
	}
}

void ioConsecution2Skill::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	m_bSetHandMesh = false;

	EndPreDelayEffect( pChar );
	pChar->EndEffect( m_SkillEffectName );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	if( m_dwTargetMarkerID != -1 )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos += ( vDir * m_fMarkerOffset );
		ioPlayStage *pStage = pChar->GetCreator();
		if( pStage )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwTargetMarkerID );
			pMapEffect->EndEffectForce();
			m_dwTargetMarkerID = -1;
		}
	}

	ClearData();
}

bool ioConsecution2Skill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	if( m_dwSkillLoopDuration > 0 && m_dwLoopStartTime + m_dwSkillLoopDuration > FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioConsecution2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_LOOP:
		return false;
	case SS_ACTION:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;
		break;
	}

	return true;
}

void ioConsecution2Skill::SetSkillLoopState( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	m_SkillState = SS_LOOP;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = iAniID = pGrp->GetAnimationIdx( m_szSkillLoopMotion );
	
	float fRate = m_fSkillLoopMotionRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );

	if( !m_szTargetMarker.IsEmpty() && pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos += ( vDir * m_fMarkerOffset );

		ioPlayStage *pStage = pOwner->GetCreator();
		vPos.y = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false );
		if( pStage )
		{
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szTargetMarker, vPos, vScale );
			if( pMapEffect )
				m_dwTargetMarkerID = pMapEffect->GetUniqueID();
		}
	}

	m_iWeaponFireCnt = 0;
	m_bMotionSetted = true;
	m_bLoopMotionSetted = true;
	m_bFirstMoveMotion = true;

	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	m_StartDirKey = pOwner->GetCurDirKey();
}

void ioConsecution2Skill::ProcessAttack( ioBaseChar *pOwner )
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

		if( !m_szResultMarker.IsEmpty() && pOwner->IsNeedProcess() )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

			D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vPos += ( vDir * m_fMarkerOffset );

			ioPlayStage *pStage = pOwner->GetCreator();
			vPos.y = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false );
			if( pStage )
				pStage->CreateMapEffect( m_szResultMarker, vPos, vScale );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_ON_FIRE;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioConsecution2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_START_FIRE:
		AttackSkillFire( pChar );
		break;
	case SSC_ON_FIRE:
		CreateWeapon( pChar );
		break;
	}
}

void ioConsecution2Skill::CreateWeapon( ioBaseChar *pOwner )
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

void ioConsecution2Skill::CheckMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bFirstMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_szSkillLoopMotion);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_SkillMotionFr);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_SkillMotionBk);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_SkillMotionLt);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_SkillMotionRt);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );

			m_bMotionSetted = false;
			m_bFirstMoveMotion = false;
			m_bLoopMotionSetted = false;

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_StartDirKey );

			float fRate = m_fSkillMotionRate;

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_SkillMotionFr, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_SkillMotionRt, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_SkillMotionBk, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_SkillMotionLt, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			}
		}
	}
	else
	{
		if( !m_bLoopMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_szSkillLoopMotion);
			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fSkillMotionRate );

			m_bLoopMotionSetted = true;
		}
		if( !m_bMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_SkillMotionFr);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_SkillMotionBk);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_SkillMotionLt);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_SkillMotionRt);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bMotionSetted = true;
		}
	}
}

void ioConsecution2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

bool ioConsecution2Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}