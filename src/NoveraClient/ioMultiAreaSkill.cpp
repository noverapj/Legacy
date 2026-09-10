

#include "stdafx.h"

#include "ioMultiAreaSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

ioMultiAreaSkill::ioMultiAreaSkill()
{
	m_dwCurMapEffect = -1;

	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFirstLoopStartTime = 0;

	m_iCurAreaWeaponCnt = 0;

	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;
}

ioMultiAreaSkill::ioMultiAreaSkill( const ioMultiAreaSkill &rhs )
: ioNormalSkill( rhs ),
 m_RedCircle( rhs.m_RedCircle ),
 m_BlueCircle( rhs.m_BlueCircle ),
 m_bCheckDropZone( rhs.m_bCheckDropZone ),
 m_fGaugePerTic( rhs.m_fGaugePerTic ),
 m_dwTicTime( rhs.m_dwTicTime ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
  m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_RedGuidCircle( rhs.m_RedGuidCircle ),
 m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_dwLoopProtectTime( rhs.m_dwLoopProtectTime ),
 m_bDisableActionProtect( rhs.m_bDisableActionProtect ),
 m_iMaxAreaWeaponCnt( rhs.m_iMaxAreaWeaponCnt ),
 m_bPreventRecoveryGauge( rhs.m_bPreventRecoveryGauge ),
 m_TeleportInfo( rhs.m_TeleportInfo ),
 m_bEnableDefenceKey( rhs.m_bEnableDefenceKey ),
 m_bCheckMaxAreaCnt( rhs.m_bCheckMaxAreaCnt ),
 m_bSetLoopTime( rhs.m_bSetLoopTime ),
 m_dwLoopTime( rhs.m_dwLoopTime ),
 m_bForceReduceGauge( rhs.m_bForceReduceGauge )
{
	m_dwCurMapEffect = -1;

	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFirstLoopStartTime = 0;

	m_iCurAreaWeaponCnt = 0;

	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;
}

ioMultiAreaSkill::~ioMultiAreaSkill()
{
}

void ioMultiAreaSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bCheckDropZone = rkLoader.LoadBool_e( "check_drop_zone", false );

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;

	m_fGaugePerTic	= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime		= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "loop_protect_time", 0 );

	m_bDisableActionProtect = rkLoader.LoadInt_e( "disable_action_protect", false );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_guid_circle", "", szBuf, MAX_PATH );
	m_RedGuidCircle = szBuf;
	rkLoader.LoadString_e( "blud_guid_circle", "", szBuf, MAX_PATH );
	m_BlueGuidCircle = szBuf;

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_iMaxAreaWeaponCnt = rkLoader.LoadInt_e( "max_area_weapon_cnt", 0 );

	m_bPreventRecoveryGauge = rkLoader.LoadBool_e( "prevent_recovery_gauge_on_skill", false );

	LoadAttackAttribute_e( "teleport_info", m_TeleportInfo, rkLoader );
	m_bEnableDefenceKey = rkLoader.LoadBool_e( "set_enable_defense_key", false );
	m_bCheckMaxAreaCnt = rkLoader.LoadBool_e( "check_max_area_cnt", false );
	m_bSetLoopTime = rkLoader.LoadBool_e( "set_loop_time", false );
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_time", 0 );
	m_bForceReduceGauge = rkLoader.LoadBool_e( "force_reduce_gauge", false );
}

ioSkill* ioMultiAreaSkill::Clone()
{
	return new ioMultiAreaSkill( *this );
}

bool ioMultiAreaSkill::OnSkillStart( ioBaseChar *pChar )
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

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwCurMapEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFirstLoopStartTime = 0;

	m_iCurAreaWeaponCnt = 0;

	m_State = SS_NONE;

	pChar->SetSkillProtection();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar );
	}

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	m_dwCheckTime			= FRAMEGETTIME();
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	// 성장관련 요소 추가.
	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage )
	{
		ioGrowthNormalUpInfo *pUpInfo = NULL;

		pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
		if( pUpInfo )
			m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

		m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
		m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );

		pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
		if( pUpInfo )
			m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);			
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioMultiAreaSkill::OnSkillEnd( ioBaseChar *pChar )
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

	pChar->EndMapEffect( m_dwCurMapEffect );
	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwFirstLoopStartTime = 0;
	m_iCurAreaWeaponCnt = 0;
	m_State = SS_NONE;
}

void ioMultiAreaSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	DWORD dwCurTime = FRAMEGETTIME();
	float fCurRange = m_fCircleRange;

	float fCurTicGauge = m_fGaugePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		if( fCurTicGauge != 0.0f && m_bReduceGauge )
		{
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
			{
				SetEndState( pChar );
				return;
			}
		}
	}

	if ( m_bSetLoopTime && m_dwSkillStartTime + m_dwLoopTime < dwCurTime )
	{
		SetEndState( pChar );
		return;
	}

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
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
				if( pChar->IsAttackKey() )
				{
					SetActionState( pChar, pStage );
					return;
				}

				if ( m_bEnableDefenceKey && pChar->IsDefenseKey() )
				{
					SetTeleportState( pChar, pStage );
					return;
				}
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < dwCurTime )
		{
			if ( m_bCheckMaxAreaCnt && m_iCurAreaWeaponCnt >= m_iMaxAreaWeaponCnt )
				SetTeleportState( pChar, pStage );
			else if( m_iCurAreaWeaponCnt < m_iMaxAreaWeaponCnt )
				SetLoopState( pChar );
			else
			{
				if( m_fGaugePerTic > 0.0f )
				{
					pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem() ) );
				}
				SetEndState( pChar );
			}
		}
		else
		{
			if( m_dwFireStartTime < dwCurTime )
			{
				CheckCreateAreaWeapon( pChar, pStage );
			}
		}
		break;
	case SS_TELEPORT:
		{
			if( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

bool ioMultiAreaSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioMultiAreaSkill::SetMagicCircle( ioBaseChar *pChar )
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

void ioMultiAreaSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = dwCurTime;
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = m_dwActionStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->AttachEffect( m_StartEffect );

	if( m_fGaugePerTic == 0.0f && !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioMultiAreaSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_bCheckDropZone )
	{
		D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
		float fMapHeight = pStage->GetMapHeightNoneEntity( vTargetPos.x, vTargetPos.z, vTargetPos.y+3.0f );
		if( fMapHeight <= 0.0f )
			return;
	}

	m_State = SS_ACTION;

	float fCurRange = m_fCircleRange;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

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

	SetNormalSkill( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SSS_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultiAreaSkill::SetLoopState( ioBaseChar *pChar )
{
	if( m_LoopAnimation.IsEmpty() || !pChar )
	{
		m_dwLoopStartTime = 0;
		m_dwFirstLoopStartTime = 0;
		return;
	}

	SetMagicCircle( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_State = SS_LOOP;
	m_dwLoopStartTime = FRAMEGETTIME();
	if( m_dwFirstLoopStartTime == 0 )
		m_dwFirstLoopStartTime = m_dwLoopStartTime;

	EndPreDelayEffect( pChar );

	if( ( m_bForceReduceGauge || m_fGaugePerTic > 0.0f ) && ! m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioMultiAreaSkill::SetEndState( ioBaseChar *pChar )
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

	pChar->AttachEffect( m_EndEffect );
}

void ioMultiAreaSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	if( iSyncState == SSS_ACTION )
	{
		SetActionState( pChar, pStage );
	}
	else if ( iSyncState == SSS_TELEPORT )
	{
		rkPacket >> m_vCirclePos;
		SetTeleportState( pChar, pStage );
	}
}

void ioMultiAreaSkill::CheckCreateAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
	{
		m_iCurAreaWeaponCnt++;
		m_dwFireStartTime = 0;

		if ( m_bCheckMaxAreaCnt && m_iCurAreaWeaponCnt >= m_iMaxAreaWeaponCnt )
			return;

		// Create AreaWeapon
		if( !m_AreaWeaponName.IsEmpty() )
		{
			ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																	"",
																	GetName(),
																	m_AreaWeaponName,
																	m_vCirclePos,
																	pChar->GetTargetRot(),
																	ioAreaWeapon::CT_NORMAL );
		}
	}
}

bool ioMultiAreaSkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioMultiAreaSkill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_State )
	{
	case SS_NONE:
		return false;
	case SS_LOOP:
	case SS_ACTION:
		{
			if( m_dwFirstLoopStartTime > 0 &&
				m_dwLoopProtectTime > 0 &&
				m_dwFirstLoopStartTime + m_dwLoopProtectTime > FRAMEGETTIME() )
				return true;

			if( m_bDisableActionProtect )
				return false;

			if( m_dwFireMotionEndTime > FRAMEGETTIME() )
				return true;
		}
		break;
	case SS_END:
		return false;
	}

	return false;
}

bool ioMultiAreaSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioMultiAreaSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMultiAreaSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioMultiAreaSkill::SetTeleportState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	if( m_bCheckDropZone )
	{
		D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
		float fMapHeight = pStage->GetMapHeightNoneEntity( vTargetPos.x, vTargetPos.z, vTargetPos.y+3.0f );
		if( fMapHeight <= 0.0f )
			return;
	}

	float fCurRange = m_fCircleRange;

	if( pChar->CheckMagicCircle() )
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( m_TeleportInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_TeleportInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_TeleportInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->RefreshFireTimeList( nAniID, m_TeleportInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetReservedSliding( m_TeleportInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_State = SS_TELEPORT;

	pChar->SetWorldPosition( m_vCirclePos );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SSS_TELEPORT;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}