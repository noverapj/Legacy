

#include "stdafx.h"

#include "ioMultiArea2Skill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioMultiArea2Skill::ioMultiArea2Skill()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurAreaWeaponCnt = 0;
}

ioMultiArea2Skill::ioMultiArea2Skill( const ioMultiArea2Skill &rhs )
: ioNormalSkill( rhs ),
 m_bDirectFire( rhs.m_bDirectFire ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_szEnableCircle( rhs.m_szEnableCircle ),
 m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
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
 m_iMaxAreaWeaponCnt( rhs.m_iMaxAreaWeaponCnt )
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurAreaWeaponCnt = 0;
}

ioMultiArea2Skill::~ioMultiArea2Skill()
{
}

void ioMultiArea2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bDirectFire = rkLoader.LoadBool_e( "skill_direct_fire", false );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_szEnableCircle = szBuf;

	m_dwEnableKeyTime = (DWORD)rkLoader.LoadInt_e( "enable_key_time", 0 );

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "loop_protect_time", 0 );

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
}

ioSkill* ioMultiArea2Skill::Clone()
{
	return new ioMultiArea2Skill( *this );
}

bool ioMultiArea2Skill::OnSkillStart( ioBaseChar *pChar )
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

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurAreaWeaponCnt = 0;

	m_vAreaWeaponPosList.clear();
	m_vAreaWeaponPosList.reserve(m_iMaxAreaWeaponCnt);

	m_State = SS_NONE;

	m_dwGapTime = FRAMEGETTIME();

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

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioMultiArea2Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	CheckCreateAreaWeapon( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );
			}
			m_dwEnableCircleIDList.clear();
		}
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
	m_iCurAreaWeaponCnt = 0;
	m_State = SS_NONE;

	m_vAreaWeaponPosList.clear();
}

void ioMultiArea2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	DWORD dwCurTime = FRAMEGETTIME();
	float fCurRange = m_fCircleRange;

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				
				if( m_bDirectFire && pChar->CheckMagicCircle() )
					AddAreaPosition( pChar, pStage );

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
				if( pChar->IsAttackKey() && m_dwGapTime < FRAMEGETTIME() )
				{					
					AddAreaPosition( pChar, pStage );
					m_dwGapTime += m_dwEnableKeyTime;

					if( pChar->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pChar->GetCharName();
						kPacket << GetName();
						kPacket << SSS_KEY;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}

				if( m_iCurAreaWeaponCnt >= m_iMaxAreaWeaponCnt )
					SetActionState( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < dwCurTime )
		{
			SetEndState( pChar );
		}
		else if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
		{
			CheckCreateAreaWeapon( pChar );
			m_dwFireStartTime = 0;
		}
		break;
	case SS_END:
		break;
	}
}

bool ioMultiArea2Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioMultiArea2Skill::SetMagicCircle( ioBaseChar *pChar )
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

void ioMultiArea2Skill::SetNormalSkill( ioBaseChar *pChar )
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
}

void ioMultiArea2Skill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;
	float fCurRange = m_fCircleRange;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
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

void ioMultiArea2Skill::SetLoopState( ioBaseChar *pChar )
{
	if( m_LoopAnimation.IsEmpty() || !pChar )
	{
		m_dwLoopStartTime = 0;
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

	EndPreDelayEffect( pChar );
}


void ioMultiArea2Skill::SetEndState( ioBaseChar *pChar )
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

void ioMultiArea2Skill::AddAreaPosition( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;

	// 첫발이 찍힐 때 스킬게이지를 소모시킨다.
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	if( m_iCurAreaWeaponCnt < m_iMaxAreaWeaponCnt )
	{
		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
		m_vAreaWeaponPosList.push_back( m_vCirclePos );
	}
	
	m_iCurAreaWeaponCnt++;

	if( !m_szEnableCircle.IsEmpty() )
	{
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szEnableCircle, m_vCirclePos, vScale );
		if( pMapEffect )
		{
			DWORD dwID = pMapEffect->GetUniqueID();
			m_dwEnableCircleIDList.push_back(dwID);
		}
	}
}

void ioMultiArea2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SSS_KEY:
		{
			AddAreaPosition( pChar, pStage );
		}
		break;
	case SSS_ACTION:
		{
			SetActionState( pChar, pStage );
		}
		break;
	}
}

void ioMultiArea2Skill::CheckCreateAreaWeapon( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_vAreaWeaponPosList.empty() )	return;

	// Create AreaWeapon
	int iCnt = m_vAreaWeaponPosList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( !m_AreaWeaponName.IsEmpty() )
		{
			ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																	"",
																	GetName(),
																	m_AreaWeaponName,
																	m_vAreaWeaponPosList[i],
																	pChar->GetTargetRot(),
																	ioAreaWeapon::CT_NORMAL );
		}
	}
	
	m_vAreaWeaponPosList.clear();

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );

			}
			m_dwEnableCircleIDList.clear();
		}
	}
}

bool ioMultiArea2Skill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioMultiArea2Skill::IsProtected( int iDefenseBreakType ) const
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
		{
			if( m_dwLoopProtectTime > 0 &&
				m_dwLoopStartTime + m_dwLoopProtectTime > FRAMEGETTIME() )
				return true;

			if( m_dwFireMotionEndTime > FRAMEGETTIME() )
				return true;
		}
		break;
	case SS_ACTION:
	case SS_END:
		return false;
	}

	return false;
}

bool ioMultiArea2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioMultiArea2Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMultiArea2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}
