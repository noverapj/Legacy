

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioTargetMissile3Skill.h"

#include "WeaponDefine.h"

ioTargetMissile3Skill::ioTargetMissile3Skill()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
}

ioTargetMissile3Skill::ioTargetMissile3Skill( const ioTargetMissile3Skill &rhs )
: ioNormalSkill( rhs ),
 m_WeaponInfo( rhs.m_WeaponInfo ),
 m_WeaponInfoAir( rhs.m_WeaponInfoAir ),
 m_fGuagePerTic( rhs.m_fGuagePerTic ),
 m_dwTicTime( rhs.m_dwTicTime ),
 m_szLoopAnimation( rhs.m_szLoopAnimation ),
 m_fLoopAniRate( rhs.m_fLoopAniRate ),
 m_fTargetRange( rhs.m_fTargetRange ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_fHeightOffSet( rhs.m_fHeightOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle )
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
}

ioTargetMissile3Skill::~ioTargetMissile3Skill()
{
}

void ioTargetMissile3Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "", szBuf[MAX_PATH] = "";

	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "attack_weapon", NO_WEAPON_ATTRIBUTE );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "attack_weapon_resist", NO_WEAPON_ATTRIBUTE );

	m_fGuagePerTic	= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime		= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "attack_weapon_wounded", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "attack_weapon_wounded_time", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attack_weapon_wounded_loop_ani", false );

	m_WeaponInfoAir.m_iWeaponIdx = rkLoader.LoadInt_e( "attack_weapon_air", NO_WEAPON_ATTRIBUTE );
	m_WeaponInfoAir.m_iResistanceIdx = rkLoader.LoadInt_e( "attack_weapon_air_resist", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "attack_weapon_air_wounded", "", szBuf, MAX_PATH );
	m_WeaponInfoAir.m_WoundedAnimation = szBuf;
	m_WeaponInfoAir.m_dwWoundedDuration = rkLoader.LoadInt_e( "attack_weapon_air_wounded_time", 0 );
	m_WeaponInfoAir.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attack_weapon_air_wounded_loop_ani", false );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_szLoopAnimation = szBuf;
	m_fLoopAniRate = rkLoader.LoadFloat_e( "loop_ani_rate", 1.0f );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fTargetRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	m_fHeightOffSet = rkLoader.LoadFloat_e( "fire_height_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
}

ioSkill* ioTargetMissile3Skill::Clone()
{
	return new ioTargetMissile3Skill( *this );
}

bool ioTargetMissile3Skill::OnSkillStart( ioBaseChar *pChar )
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

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_State = SS_NONE;

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

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_dwCheckTime = FRAMEGETTIME() + m_dwTicTime;
	return true;
}

void ioTargetMissile3Skill::OnSkillEnd( ioBaseChar *pChar )
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

void ioTargetMissile3Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
		if( pChar->IsNeedProcess() )
		{
			if( pChar->CheckMagicCircle() )
			{
				float fValue = 0.0f;
				ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
				if( pUpInfo )
					fValue = pUpInfo->GetValue(pStage);
			
				float fCurRange = m_fCircleRange + fValue;
				pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, false );
			}

			if( m_dwCheckTime < dwCurTime )
			{
				m_dwCheckTime = dwCurTime + m_dwTicTime;

				if( m_fGuagePerTic != 0.0f )
				{
					if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), m_fGuagePerTic ) <= 0.0f )
					{
						m_State = SS_END;

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
				}
			}

			if( pChar->IsAttackKey() )
			{
				SetNormalSkill( pChar );

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << (int)SYNC_SET_NORMAL_SKILL;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		break;
	case SS_FIRE:
		{
			if( pChar->IsNeedProcess() )
			{
				if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
				{
					CheckTarget( pChar );
					SetFireState( pChar );
					m_State = SS_END;
				}
			}
		}
		break;
	}
}

bool ioTargetMissile3Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioTargetMissile3Skill::SetNormalSkill( ioBaseChar *pChar )
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
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	// Attack
	float fFireTime = pChar->GetFirstFireTime( szSkillMotion );
	if( fFireTime > 0.0f )
		m_dwFireStartTime += fFireTime * fTimeRate;
	else
		m_dwFireStartTime = 0;

	D3DXVECTOR3 vEffectDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_SkillMapEffectName, vEffectDir );
	if( pMapEffect )
	{
		D3DXVECTOR3 vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
		vCirclePos.y += m_fHeightOffSet;
		pMapEffect->SetWorldPosition( vCirclePos );
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();
	}

	m_State = SS_FIRE;

	//추가 게이지 감소
	ioItem *pItem = GetOwnerItem();
	if( pItem && !m_bReduceGauge )
	{
		float fUseGauge = pItem->GetMaxSkillGauge( pChar->GetCharName() );
		pChar->ReduceNeedGaugeBySkill( this, fUseGauge );
		m_bReduceGauge = true;
	}
}

void ioTargetMissile3Skill::SetWaitState( ioBaseChar *pChar )
{	
	m_State = SS_WAIT;

	EndPreDelayEffect( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_szLoopAnimation );
		if( iAniID == -1 )	return;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fLoopAniRate );
	}

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );	
	}
}

void ioTargetMissile3Skill::SetFireState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	D3DXVECTOR3 vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
	vCirclePos.y += m_fHeightOffSet;

	float fAngle[8];
	for( int i=0; i<8; ++i )
	{
		fAngle[i] = i * 45;
	}

	if( m_vTargetList.empty() )
	{
		for( int i=0; i<8; ++i )
		{
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle[i]) );
			D3DXVECTOR3 vDir = qtRot * pChar->GetMoveDir();
			D3DXVec3Normalize( &vDir, &vDir );
			pChar->SkillFireExplicit( m_WeaponInfo, GetName(), m_dwWeaponBaseIndex, vCirclePos, vDir );
		}
	}
	else
	{
		int iCnt = m_vTargetList.size();
		for( int i=0; i<8; ++i )
		{
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
			D3DXVECTOR3 vDir = pChar->GetMoveDir();

			if( COMPARE( i, 0, iCnt ) )
			{
				ioBaseChar *pTarget = pChar->GetBaseChar( m_vTargetList[i].name );
				if( pTarget )
				{
					vDir = pTarget->GetMidPositionByRate() - vCirclePos;
					D3DXVec3Normalize( &vDir, &vDir );

					if( m_vTargetList[i].is_floating )
					{
						pChar->SkillFireExplicit( m_WeaponInfoAir, GetName(), m_dwWeaponBaseIndex, vCirclePos, vDir );
					}
					else
					{
						pChar->SkillFireExplicit( m_WeaponInfo, GetName(), m_dwWeaponBaseIndex, vCirclePos, vDir );
					}
				}
			}
		}
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)SYNC_FIRE;
		kPacket << vCirclePos;
		int iCnt = m_vTargetList.size();
		kPacket << iCnt;
		for( int i=0; i<iCnt; ++i )
		{
			kPacket << m_vTargetList[i].name;
			kPacket << m_vTargetList[i].distance;
			kPacket << m_vTargetList[i].is_floating;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMissile3Skill::CheckTarget( ioBaseChar *pChar )
{
	m_vTargetList.clear();

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDiff = vTargetPos - vCirclePos;
		float fDiffRange = D3DXVec3Length( &vDiff );
		if( fDiffRange > m_fTargetRange )
			continue;

		TargetInfo	kInfo;
		kInfo.name = pTarget->GetCharName();
		kInfo.distance = fDiffRange;
		kInfo.is_floating = pTarget->IsFloating();

		m_vTargetList.push_back( kInfo );
	}

	if( !m_vTargetList.empty() )
	{
		auto sort_target = [](TargetInfo rhs, TargetInfo lhs){return rhs.distance > lhs.distance;};
		std::sort( m_vTargetList.begin(), m_vTargetList.end(), sort_target );
	}
}

bool ioTargetMissile3Skill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() ) 
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
		return false;

	return true;
}

void ioTargetMissile3Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int iSycState;
	rkPacket >> iSycState;

	switch( iSycState )
	{
	case SYNC_SET_NORMAL_SKILL:
		SetNormalSkill( pChar );
		break;
	case SYNC_FIRE:
		{
			m_vTargetList.clear();
			D3DXVECTOR3 vPos;
			int iCnt;
			rkPacket >> vPos;
			rkPacket >> iCnt;
			for( int i=0; i< iCnt; ++i )
			{
				TargetInfo kInfo;
				rkPacket >> kInfo.name;
				rkPacket >> kInfo.distance;
				rkPacket >> kInfo.is_floating;
				m_vTargetList.push_back( kInfo );
			}

			float fAngle[8];
			for( int i=0; i<8; ++i )
			{
				fAngle[i] = i * 45;
			}

			if( m_vTargetList.empty() )
			{
				for( int i=0; i<8; ++i )
				{
					m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

					D3DXQUATERNION qtRot;
					D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle[i]) );
					D3DXVECTOR3 vDir = qtRot * pChar->GetMoveDir();
					D3DXVec3Normalize( &vDir, &vDir );
					pChar->SkillFireExplicit( m_WeaponInfo, GetName(), m_dwWeaponBaseIndex, vPos, vDir );
				}
			}
			else
			{
				int iCnt = m_vTargetList.size();
				for( int i=0; i<8; ++i )
				{
					m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
					D3DXVECTOR3 vDir = pChar->GetMoveDir();

					if( COMPARE( i, 0, iCnt ) )
					{
						ioBaseChar *pTarget = pChar->GetBaseChar( m_vTargetList[i].name );
						if( pTarget )
						{
							vDir = pTarget->GetMidPositionByRate() - vPos;
							D3DXVec3Normalize( &vDir, &vDir );

							if( m_vTargetList[i].is_floating )
							{
								pChar->SkillFireExplicit( m_WeaponInfoAir, GetName(), m_dwWeaponBaseIndex, vPos, vDir );
							}
							else
							{
								pChar->SkillFireExplicit( m_WeaponInfo, GetName(), m_dwWeaponBaseIndex, vPos, vDir );
							}
						}
					}
				}
			}

			m_State = SS_END;
		}
		break;
	case SYNC_END:
		m_State = SS_END;
		break;
	}
}

bool ioTargetMissile3Skill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioTargetMissile3Skill::IsAttackEndState() const
{
	if( m_State == SS_END )
		return true;
	return false;
}

bool ioTargetMissile3Skill::IsProtected( int iDefenseBreakType ) const
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

	return false;
}

bool ioTargetMissile3Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

void ioTargetMissile3Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
