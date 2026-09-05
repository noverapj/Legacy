

#include "stdafx.h"

#include "ioCircleMove3Skill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

ioCircleMove3Skill::ioCircleMove3Skill()
{
	ClearData();
}

ioCircleMove3Skill::ioCircleMove3Skill( const ioCircleMove3Skill &rhs )
: ioNormalSkill( rhs ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
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
 m_RedGuidCircle( rhs.m_RedGuidCircle ),
 m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
 m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_dwLoopTime( rhs.m_dwLoopTime ),
 m_szRangeEffect( rhs.m_szRangeEffect ),
 m_fSearchRadius( rhs.m_fSearchRadius ),
 m_fSearchHeight( rhs.m_fSearchHeight ),
 m_bNoneStopActive( rhs.m_bNoneStopActive )
{
	ClearData();
}

ioCircleMove3Skill::~ioCircleMove3Skill()
{
}

void ioCircleMove3Skill::ClearData()
{
	m_State = SS_NONE;

	m_dwFireStartTime	= 0;
	m_dwActionStartTime	= 0;
	m_dwLoopStartTime	= 0;
	m_dwProtectTime		= 0;

	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;

	m_vFirePos.clear();
}

void ioCircleMove3Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwLoopProtectDuration = rkLoader.LoadInt_e( "loop_protect_duration", 0 );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "target_circle_effect", "", szBuf, MAX_PATH );
	m_TargetCircleEffect = szBuf;

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

	rkLoader.LoadString_e( "range_effect", "", szBuf, MAX_PATH );
	m_szRangeEffect = szBuf;
	m_fSearchRadius = rkLoader.LoadFloat_e( "search_target_radius", 0 );
	m_fSearchHeight = rkLoader.LoadFloat_e( "search_target_height", 0 );

	//////////////////////////////////////////////////////////////////////////
	m_bNoneStopActive = rkLoader.LoadBool_e(  "use_none_stop_active", false );
}

ioSkill* ioCircleMove3Skill::Clone()
{
	return new ioCircleMove3Skill( *this );
}

bool ioCircleMove3Skill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	ClearData();

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

	SetWaitState( pChar );

	if( m_bNoneStopActive )
		pChar->SetMagicCirclePos( true, pChar->GetWorldPosition() );

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

void ioCircleMove3Skill::OnSkillEnd( ioBaseChar *pChar )
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
	if( m_dwTargetCircleEffect != -1 )
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );

	m_dwTargetCircleEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	ClearData();
}

void ioCircleMove3Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				if( m_bNoneStopActive )
				{
					//pChar->SetMagicCirclePos( true, pChar->GetWorldPosition() );
					SetActionState( pChar, pStage );
					SearchTarget( pChar, pStage );
					return;
				}
				
				SetLoopState( pChar, pStage );
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

			if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < dwCurTime )
			{
				m_dwLoopStartTime = 0;
				SetActionState( pChar, pStage );
				SearchTarget( pChar, pStage );
			}


			//이부분은 좀 문제가 있어 보임( 동기화쪽에 문제가 될거 같음 )
			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetActionState( pChar, pStage );
				SearchTarget( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
			SetEndState( pChar );
			return;
		}
		CheckCreateWeapon( pChar, pStage );
		break;
	case SS_END:
		break;
	}
}

bool ioCircleMove3Skill::IsProtected( int iDefenseBreakType ) const
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

	if( m_State == SS_ACTION )
		return true;

	if( m_State == SS_LOOP )
	{
		if( m_dwLoopProtectDuration == 0 )
			return true;

		if( m_dwProtectTime > dwCurTime )
			return true;
	}

	if( m_State == SS_NONE || m_State == SS_WAIT || m_State == SS_END )
	{
		return false;
	}

	return false;
}

bool ioCircleMove3Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioCircleMove3Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMove3Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

}

void ioCircleMove3Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	ioHashString szAreaWeaponName;
	rkPacket >> szAreaWeaponName;

	int iPosCnt;
	rkPacket >> iPosCnt;
	m_vFirePos.clear();
	m_vFirePos.reserve( iPosCnt );
	for( int i=0; i<iPosCnt; ++i )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		m_vFirePos.push_back( vPos );
	}

	int iCreateType;
	rkPacket >> iCreateType;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	int iCnt = m_vFirePos.size();
	for( int i=0; i<iCnt; ++i )
	{
		D3DXVECTOR3 vCreatePos = m_vFirePos[i];

		ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																"",
																GetName(),
																szAreaWeaponName,
																vCreatePos,
																qtRot,
																iCreateType );

		if( pArea )
		{
			m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
		}
	}
}

/*bool ioCircleMove3Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}*/

void ioCircleMove3Skill::SetNormalSkill( ioBaseChar *pChar )
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

	if( m_State != SS_END )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioCircleMove3Skill::SetWaitState( ioBaseChar *pChar )
{
	m_State = SS_WAIT;
	//m_dwPreDelayStartTime = 0;

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
}

void ioCircleMove3Skill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
		fValue = pUpInfo->GetValue(pStage);

	float fCurRange = m_fCircleRange + fValue;

	m_dwPreDelayStartTime = 0;

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
	pChar->CheckMagicCircleVisible( false );

	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szRangeEffect, m_vCirclePos, vScale );
}

void ioCircleMove3Skill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_LoopAnimation.IsEmpty() || !pChar )
	{
		m_dwLoopStartTime = 0;
		return;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_State = SS_LOOP;
}

bool ioCircleMove3Skill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

void ioCircleMove3Skill::SetEndState( ioBaseChar *pChar )
{
	m_State = SS_END;

	//DestroyMapEffect
	if( m_dwTargetCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );
		m_dwTargetCircleEffect = -1;
	}

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
	
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->AttachEffect( m_EndEffect );
}

void ioCircleMove3Skill::SearchTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTargetChar = (*iter);
		if( !pTargetChar )
			continue;

		if( pChar->GetCharName() == pTargetChar->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTargetChar->GetTeam() )
			continue;

		if( pTargetChar->IsSystemState() )
			continue;
		
		if( !pTargetChar->IsEnableTarget() ) 
			continue;
		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition();
		
		float fHeightLength = vTargetPos.y - m_vCirclePos.y;
		if( m_fSearchHeight > 0.0f && fabs(fHeightLength) > m_fSearchHeight )
			continue;
		
		vTargetPos.y = m_vCirclePos.y;
		D3DXVECTOR3 vDiff = m_vCirclePos - vTargetPos;
		float fLength = D3DXVec3Length( &vDiff );
		
		if( m_fSearchRadius > 0.0f && fLength > m_fSearchRadius )
			continue;

		D3DXVECTOR3 vPos = vTargetPos;
		vPos.y = pTargetChar->GetBottomHeight();
		m_vFirePos.push_back( vPos );
	}
}

void ioCircleMove3Skill::CheckCreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
	{
		// Create AreaWeapon
		if( !m_AreaWeaponName.IsEmpty() )
		{
			int iCnt = m_vFirePos.size();
			for( int i=0; i<iCnt; ++i )
			{
				D3DXVECTOR3 vCreatePos = m_vFirePos[i];

				ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																		"",
																		GetName(),
																		m_AreaWeaponName,
																		vCreatePos,
																		pChar->GetTargetRot(),
																		ioAreaWeapon::CT_NORMAL );

				if( pArea )
				{
					m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
				}
			}

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << m_AreaWeaponName;
				kPacket << iCnt;
				for( int i=0; i<iCnt; ++i )
				{
					D3DXVECTOR3 vCreatePos = m_vFirePos[i];
					kPacket << vCreatePos;
				}
				kPacket << (int)ioAreaWeapon::CT_NORMAL;
				kPacket << pChar->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		m_dwFireStartTime = 0;
	}
}