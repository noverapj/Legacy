

#include "stdafx.h"

#include "ioMultiArea3Skill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioMultiArea3Skill::ioMultiArea3Skill()
{
	ClearData();
}

ioMultiArea3Skill::ioMultiArea3Skill( const ioMultiArea3Skill &rhs )
: ioNormalSkill( rhs ),
 m_SkillMotionList( rhs.m_SkillMotionList ),
 m_WeaponInfoList( rhs.m_WeaponInfoList ),
 m_RedCircle( rhs.m_RedCircle ),
 m_BlueCircle( rhs.m_BlueCircle ),
 m_bCheckDropZone( rhs.m_bCheckDropZone ),
 m_fCircleMaxRange( rhs.m_fCircleMaxRange ),
 m_fCircleMinRange( rhs.m_fCircleMinRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_RedGuidCircle( rhs.m_RedGuidCircle ),
 m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
 m_iMaxCombo( rhs.m_iMaxCombo ),
 m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
 m_dwEnableTeleportTime( rhs.m_dwEnableTeleportTime ),
 m_TeleportStartEffect( rhs.m_TeleportStartEffect ),
 m_TeleportEndEffect( rhs.m_TeleportEndEffect )
{
	ClearData();
}

ioMultiArea3Skill::~ioMultiArea3Skill()
{
}

void ioMultiArea3Skill::ClearData()
{
	m_dwCurMapEffect = -1;

	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwSkillStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwSkillLoopStartTime = 0;
	m_iCurCombo = 0;
	
	m_dwEnableNextTime = 0;

	m_vStartPos = ioMath::VEC3_ZERO;
}

void ioMultiArea3Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "attack_type_count", 0 );
	m_iMaxCombo = iCnt;
	m_WeaponInfoList.clear();
	m_WeaponInfoList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );;

		m_WeaponInfoList.push_back( kInfo );
	}

	m_SkillMotionList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		SkillMotion kInfo;
		kInfo.Init();
		wsprintf_e( szKey, "attack_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_Motion = szBuf;

		wsprintf_e( szKey, "attack_motion%d_rate", i+1 );
		kInfo.m_fMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "attack_motion%d_end_delay", i+1 );
		kInfo.m_dwMotionEndDelay = (DWORD)rkLoader.LoadInt( szKey, 0 );

		m_SkillMotionList.push_back( kInfo );
	}

	m_bCheckDropZone = rkLoader.LoadBool_e( "check_drop_zone", false );

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;

	m_fCircleMaxRange = rkLoader.LoadFloat_e( "magic_circle_max_range", 0.0f );
	m_fCircleMinRange = rkLoader.LoadFloat_e( "magic_circle_min_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_guid_circle", "", szBuf, MAX_PATH );
	m_RedGuidCircle = szBuf;
	rkLoader.LoadString_e( "blue_guid_circle", "", szBuf, MAX_PATH );
	m_BlueGuidCircle = szBuf;

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_dwSkillLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	m_dwEnableKeyTime = (DWORD)rkLoader.LoadInt_e( "enable_key_time", 0 );
	m_dwEnableTeleportTime = (DWORD)rkLoader.LoadInt_e( "enable_teleport_time", 0 );

	rkLoader.LoadString_e( "teleport_start_effect", "", szBuf, MAX_PATH );
	m_TeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "teleport_end_effect", "", szBuf, MAX_PATH );
	m_TeleportEndEffect = szBuf;
}

ioSkill* ioMultiArea3Skill::Clone()
{
	return new ioMultiArea3Skill( *this );
}

bool ioMultiArea3Skill::OnSkillStart( ioBaseChar *pChar )
{
	/*if( !HasSkillMotion() )
		return false;*/

	m_dwSkillStartTime = FRAMEGETTIME();
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

	ClearData();

	pChar->SetSkillProtection();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetSkillLoopState( pChar );
	}

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !m_bReduceGauge )
	{
		float fGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_vStartPos = pChar->GetWorldPosition();

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioMultiArea3Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->EndMapEffect( m_dwCurMapEffect );

	DestroyMagicCircle( pChar );

	/*if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
	}*/

	ClearData();
}

void ioMultiArea3Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				SetSkillLoopState( pChar );
				CreateMagicCircle( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if( m_iCurCombo == 0 )
			{
				UpdateMagicCircle( pChar );

				if( m_dwSkillLoopDuration > 0 && m_dwSkillLoopStartTime + m_dwSkillLoopDuration < dwCurTime )
					SetSkillEndState( pChar );
			}

			if( pChar->IsNeedProcess() )
			{
				if( m_iCurCombo > 0 && m_dwEnableNextTime < dwCurTime )
				{
					SetSkillEndState( pChar );
					return;
				}

				if( COMPARE( m_iCurCombo, 0, m_iMaxCombo ) )
				{
					if( pChar->IsAttackKey()  )
						SetSkillActionState( pChar, pStage );
				}
				else
				{
					if( pChar->IsJumpKey() )
						SetSkillTeleportState( pChar );
				}
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetSkillLoopState( pChar );
			}

			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				int iCurCombo = min( m_iCurCombo, (int)m_WeaponInfoList.size() );
				pChar->SkillFireExplicit( m_WeaponInfoList[iCurCombo],
										  GetName(),
										  m_dwWeaponBaseIndex+m_iCurCombo,
										  m_vCirclePos );
				m_dwFireStartTime = 0;
				m_iCurCombo++;
			}
		}
		break;
	case SS_TELEPORT:
		SetSkillEndState( pChar );
		break;
	case SS_END:
		break;
	}
}

bool ioMultiArea3Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioMultiArea3Skill::CreateMagicCircle( ioBaseChar *pChar )
{
	if( !pChar->CheckMagicCircle() )
	{
		// 팀 체크
		if( pChar->GetTeam() == TEAM_RED )
		{
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
		}
		else if( pChar->GetTeam() == TEAM_BLUE )
		{
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
		}
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			{
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
			}
			else
			{
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
			}
		}
	}
}

void ioMultiArea3Skill::UpdateMagicCircle( ioBaseChar *pChar )
{
	if( !pChar->CheckMagicCircle() )
		return;

	if( m_iCurCombo > 0 )
		return;

	// 뱡향 변화 체크
	pChar->CheckChangeDirectionForMagicCircle();

	// MagicCircle 위치 갱신
	//pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleMaxRange, m_fCircleHeightGap, true, true );
	pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleMaxRange, m_fCircleMinRange, m_fCircleHeightGap, false );

	// MagicCircle 위치방향으로 캐릭터 회전
	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
}

void ioMultiArea3Skill::EndMagicCircle( ioBaseChar *pChar )
{
	if( m_iCurCombo > 0 )
		return;

	if( !pChar->CheckMagicCircle() )
		return;

	pChar->CheckMagicCircleVisible( false );
	pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleMaxRange, m_fCircleHeightGap, false );
	pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleMaxRange, m_fCircleMinRange, m_fCircleHeightGap, false );
	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	// team circle
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}

	if( pMapEffect )
	{
		m_dwCurMapEffect = pMapEffect->GetUniqueID();
	}
}

void ioMultiArea3Skill::DestroyMagicCircle( ioBaseChar *pChar )
{
	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();
}

void ioMultiArea3Skill::SetNormalSkill( ioBaseChar *pChar )
{
	if( !COMPARE( m_iCurCombo, 0, m_iMaxCombo ) )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	//ioHashString szSkillMotion = GetSkillMotionName();
	ioHashString szSkillMotion = m_SkillMotionList[m_iCurCombo].m_Motion;

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	//float fAniRate = GetSkillMotionRate();
	float fAniRate = m_SkillMotionList[m_iCurCombo].m_fMotionRate;
	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = dwCurTime;
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	//m_dwMotionEndTime += m_dwAnimationEndDelay;
	m_dwMotionEndTime += m_SkillMotionList[m_iCurCombo].m_dwMotionEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = m_dwActionStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	if( COMPARE( m_iCurCombo, 0, m_iMaxCombo-1 ) )
		m_dwEnableNextTime = m_dwMotionEndTime + m_dwEnableKeyTime;
	else
		m_dwEnableNextTime = m_dwMotionEndTime + m_dwEnableTeleportTime;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;
}

void ioMultiArea3Skill::SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_iCurCombo >= m_iMaxCombo )
		return;

	if( m_bCheckDropZone )
	{
		D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
		float fMapHeight = pStage->GetMapHeightNoneEntity( vTargetPos.x, vTargetPos.z, vTargetPos.y+3.0f );
		if( fMapHeight <= 0.0f )
			return;
	}

	m_State = SS_ACTION;

	EndMagicCircle( pChar );
	SetNormalSkill( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultiArea3Skill::SetSkillTeleportState( ioBaseChar *pChar )
{
	if( !pChar )	return;

	m_State = SS_TELEPORT;

	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage )
	{
		D3DXVECTOR3 vScale = pChar->GetWorldScale();
		pStage->CreateMapEffect( m_TeleportStartEffect, m_vStartPos, vScale );
		pStage->CreateMapEffect( m_TeleportEndEffect, m_vCirclePos, vScale );
	}

	pChar->SetWorldPosition( m_vCirclePos );

	if( !m_SkillSoundName.IsEmpty() && pChar )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		m_SkillSoundID = g_SoundMgr.PlaySound( m_SkillSoundName, pGrp->GetParentSceneNode() );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_TELEPORT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultiArea3Skill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( m_SkillLoopMotion.IsEmpty() || !pChar )
	{
		m_dwSkillLoopStartTime = 0;
		return;
	}

	m_State = SS_LOOP;

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_SkillLoopMotion, FLOAT100 );

	m_dwSkillLoopStartTime = FRAMEGETTIME();

	EndPreDelayEffect( pChar );
}


void ioMultiArea3Skill::SetSkillEndState( ioBaseChar *pChar )
{
	if( m_State == SS_END )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = SS_END;
	pChar->DestroyMagicCircle();

	if( m_SkillEndMotion.IsEmpty() )
	{
		m_dwMotionStartTime = dwCurTime;
		m_dwMotionEndTime = m_dwMotionStartTime;
		return;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndMotionRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;
}

void ioMultiArea3Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	switch( iState )
	{
	case SSC_ACTION:
		rkPacket >> m_iCurCombo;
		SetSkillActionState( pChar, pStage );
		break;
	case SSC_TELEPORT:
		SetSkillTeleportState( pChar );
		break;
	}
}

bool ioMultiArea3Skill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioMultiArea3Skill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime+m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	switch( m_State )
	{
	case SS_NONE:
		return false;
	case SS_LOOP:
	case SS_ACTION:
		{
			if( m_dwFireMotionEndTime > FRAMEGETTIME() )
				return true;
		}
		break;
	case SS_END:
		return false;
	}

	return false;
}

bool ioMultiArea3Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioMultiArea3Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMultiArea3Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone && pZone->GetOwner())
	{
		D3DXVECTOR3 vDir = pZone->GetOwner()->GetMoveDir();
		pZone->SetCollisionDir( vDir );
		pZone->SetZoneValue( false, true );
	}
}

