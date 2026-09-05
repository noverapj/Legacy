

#include "stdafx.h"

#include "ioCircleMove4Skill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


ioCircleMove4Skill::ioCircleMove4Skill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;

	m_bSetElementAtt = false;
	m_iCurElement = ioAttackableItem::MET_FIRE;
}

ioCircleMove4Skill::ioCircleMove4Skill( const ioCircleMove4Skill &rhs )
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
 m_RedGuidCircle( rhs.m_RedGuidCircle ),
 m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
 m_ElementCircleInfoList( rhs.m_ElementCircleInfoList )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;

	m_bSetElementAtt = false;
	m_iCurElement = ioAttackableItem::MET_FIRE;
}

ioCircleMove4Skill::~ioCircleMove4Skill()
{
}

void ioCircleMove4Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

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

	// element circle
	LoadElementCircleInfo( rkLoader );
}

void ioCircleMove4Skill::LoadElementCircleInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_ElementCircleInfoList.clear();

	int iCircleCnt = rkLoader.LoadInt_e( "element_circle_cnt", 0 );
	if( iCircleCnt <= 0 )
		return;

	m_ElementCircleInfoList.reserve( iCircleCnt );

	for( int i=0; i < iCircleCnt; ++i )
	{
		ElementCircleInfo kCircleInfo;

		// circle
		wsprintf_e( szKey, "element_circle%d_fr", i+1 );
		kCircleInfo.m_fCircleFr = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "element_circle%d_si", i+1 );
		kCircleInfo.m_fCircleSi = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "element_circle%d_ready_eff", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCircleInfo.m_CircleReadyEff = szBuf;
		wsprintf_e( szKey, "element_circle%d_eff", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCircleInfo.m_CircleEff = szBuf;

		// att pos
		wsprintf_e( szKey, "element_circle%d_att_fr", i+1 );
		kCircleInfo.m_fAttackFr = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "element_circle%d_att_si", i+1 );
		kCircleInfo.m_fAttackSi = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "element_circle%d_att_he", i+1 );
		kCircleInfo.m_fAttackHe = rkLoader.LoadFloat( szKey, 0.0f );

		// att & eff
		// fire
		wsprintf_e( szKey, "element_circle%d_fire_eff", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCircleInfo.m_FireEff = szBuf;

		wsprintf_e( szKey, "element_circle%d_fire_att", i+1 );
		kCircleInfo.m_FireAtt.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "element_circle%d_fire_att_resist", i+1 );
		kCircleInfo.m_FireAtt.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "element_circle%d_fire_att_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCircleInfo.m_FireAtt.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "element_circle%d_fire_att_wounded_time", i+1 );
		kCircleInfo.m_FireAtt.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "element_circle%d_fire_att_wounded_loop_ani", i+1 );
		kCircleInfo.m_FireAtt.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		// ice
		wsprintf_e( szKey, "element_circle%d_ice_eff", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCircleInfo.m_IceEff = szBuf;

		wsprintf_e( szKey, "element_circle%d_ice_att", i+1 );
		kCircleInfo.m_IceAtt.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "element_circle%d_ice_att_resist", i+1 );
		kCircleInfo.m_IceAtt.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "element_circle%d_ice_att_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCircleInfo.m_IceAtt.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "element_circle%d_ice_att_wounded_time", i+1 );
		kCircleInfo.m_IceAtt.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "element_circle%d_ice_att_wounded_loop_ani", i+1 );
		kCircleInfo.m_IceAtt.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		// light
		wsprintf_e( szKey, "element_circle%d_light_eff", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCircleInfo.m_LightEff = szBuf;

		wsprintf_e( szKey, "element_circle%d_light_att", i+1 );
		kCircleInfo.m_LightAtt.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "element_circle%d_light_att_resist", i+1 );
		kCircleInfo.m_LightAtt.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "element_circle%d_light_att_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCircleInfo.m_LightAtt.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "element_circle%d_light_att_wounded_time", i+1 );
		kCircleInfo.m_LightAtt.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "element_circle%d_light_att_wounded_loop_ani", i+1 );
		kCircleInfo.m_LightAtt.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_ElementCircleInfoList.push_back( kCircleInfo );
	}
}

ioSkill* ioCircleMove4Skill::Clone()
{
	return new ioCircleMove4Skill( *this );
}

bool ioCircleMove4Skill::OnSkillStart( ioBaseChar *pChar )
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

	m_bSetElementAtt = false;
	m_iCurElement = ioAttackableItem::MET_FIRE;

	ClearElementInfo();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	pChar->SetSkillProtection();

	m_dwProtectTime = 0;

	SetWaitState( pChar );

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

void ioCircleMove4Skill::OnSkillEnd( ioBaseChar *pChar )
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

	ClearElementEff( pChar );

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;
}

void ioCircleMove4Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurRange = m_fCircleRange;
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
			m_vCirclePos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( m_vCirclePos, true, false );

			UpdateElementEff( pChar, pStage );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() )
				{
					SetActionState( pChar, pStage );
					return;
				}
				else if( pChar->IsDefenseKey() )
				{
					ChangeElement( pChar );
					return;
				}
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < dwCurTime )
		{
			SetEndState( pChar );
		}
		else if( !m_bSetElementAtt && m_dwFireStartTime < dwCurTime )
		{
			SetElementAtt( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

bool ioCircleMove4Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCircleMove4Skill::SetNormalSkill( ioBaseChar *pChar )
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

	m_dwEnableReserveTime = 0;
	float fReserve = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;
	if( fReserve > 0.0f )
	{
		m_dwEnableReserveTime = dwCurTime + fReserve;
	}

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->AttachEffect( m_StartEffect );

	if( !m_bReduceGauge && m_State != SS_END )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioCircleMove4Skill::SetWaitState( ioBaseChar *pChar )
{
	m_State = SS_WAIT;
	m_dwPreDelayStartTime = 0;

	// circle
	if( !pChar->CheckMagicCircle() )
	{
		// 팀 체크
		if( pChar->GetTeam() == TEAM_RED )
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
		else if( pChar->GetTeam() == TEAM_BLUE )
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
			else
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
		}
	}

	// char pos eff
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

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	ClearElementEff( pChar );
	SetElementEff( pChar );
}

void ioCircleMove4Skill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;

	float fCurRange = m_fCircleRange;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	else if( pChar->GetTeam() == TEAM_BLUE )
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		}
		else
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
		}
	}

	if( pMapEffect )
	{
		if( m_dwCurMapEffect != -1 )
			pChar->EndMapEffect( m_dwCurMapEffect, false );

		m_dwCurMapEffect = pMapEffect->GetUniqueID();

		if( m_dwTargetCircleEffect != -1 )
		{
			pChar->EndMapEffect( m_dwTargetCircleEffect, false );
			m_dwTargetCircleEffect = -1;
		}
	}

	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );

	SetElementAttEff( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SSS_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMove4Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SSS_ACTION:
		SetActionState( pChar, pStage );
		break;
	case SSS_CHANGE:
		{
			rkPacket >> m_iCurElement;
		
			ClearElementEff( pChar );
			SetElementEff( pChar );
		}
		break;
	}
}

bool ioCircleMove4Skill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

void ioCircleMove4Skill::SetEndState( ioBaseChar *pChar )
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

	ClearElementEff( pChar );

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

bool ioCircleMove4Skill::IsProtected( int iDefenseBreakType ) const
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

	if( m_State == SS_NONE || m_State == SS_WAIT || m_State == SS_END )
	{
		return false;
	}

	return false;
}

bool ioCircleMove4Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioCircleMove4Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMove4Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioCircleMove4Skill::ChangeElement( ioBaseChar *pChar )
{
	switch( m_iCurElement )
	{
		case ioAttackableItem::MET_FIRE:
			m_iCurElement = ioAttackableItem::MET_ICE;
			break;
		case ioAttackableItem::MET_ICE:
			m_iCurElement = ioAttackableItem::MET_LIGHT;
			break;
		case ioAttackableItem::MET_LIGHT:
			m_iCurElement = ioAttackableItem::MET_FIRE;
			break;
	}

	ClearElementEff( pChar );
	SetElementEff( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SSS_CHANGE;
		kPacket << m_iCurElement;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMove4Skill::ClearElementInfo()
{
	ElementCircleInfoList::iterator iter = m_ElementCircleInfoList.begin();
	while( iter != m_ElementCircleInfoList.end() )
	{
		(*iter).InitID();

		++iter;
	}
}

void ioCircleMove4Skill::SetElementEff( ioBaseChar *pChar )
{
	// element
	D3DXVECTOR3 vPos, vDir, vSide;
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	ioMapEffect *pMapEffect = NULL;

	ElementCircleInfoList::iterator iter = m_ElementCircleInfoList.begin();
	while( iter != m_ElementCircleInfoList.end() )
	{
		pMapEffect = NULL;

		// element circle eff
		if( pChar->IsNeedProcess() )
		{
			vPos = m_vCirclePos;

			vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			vPos += vDir * (*iter).m_fCircleFr;

			D3DXVec3Cross( &vSide, &ioMath::UNIT_Y, &vDir );
			D3DXVec3Normalize( &vSide, &vSide );
			vPos += vSide * (*iter).m_fCircleSi;
	
			pMapEffect = pChar->CreateMapEffectBySkill( (*iter).m_CircleReadyEff, vPos );
			if( pMapEffect )
			{
				(*iter).m_CircleEffID = pMapEffect->GetUniqueID();
			}
		}

		// element att eff
		vPos = pChar->GetWorldPosition();

		vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		vPos += vDir * (*iter).m_fAttackFr;

		D3DXVec3Cross( &vSide, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSide, &vSide );
		vPos += vSide * (*iter).m_fAttackSi;

		vPos.y += (*iter).m_fAttackHe;

		switch( m_iCurElement )
		{
		case ioAttackableItem::MET_FIRE:
			pMapEffect = pChar->CreateMapEffectBySkill( (*iter).m_FireEff, vPos );
			break;
		case ioAttackableItem::MET_ICE:
			pMapEffect = pChar->CreateMapEffectBySkill( (*iter).m_IceEff, vPos );
			break;
		case ioAttackableItem::MET_LIGHT:
			pMapEffect = pChar->CreateMapEffectBySkill( (*iter).m_LightEff, vPos );
			break;
		}
		
		if( pMapEffect )
		{
			(*iter).m_AttEffID = pMapEffect->GetUniqueID();
		}
		
		++iter;
	}
}

void ioCircleMove4Skill::ClearElementEff( ioBaseChar *pChar )
{
	ElementCircleInfoList::iterator iter = m_ElementCircleInfoList.begin();
	while( iter != m_ElementCircleInfoList.end() )
	{
		if( (*iter).m_CircleEffID != -1 )
		{
			pChar->EndMapEffect( (*iter).m_CircleEffID, false );
			(*iter).m_CircleEffID = -1;
		}

		if( (*iter).m_AttEffID != -1 )
		{
			pChar->EndMapEffect( (*iter).m_AttEffID, false );
			(*iter).m_AttEffID = -1;
		}

		++iter;
	}
}

void ioCircleMove4Skill::SetElementAttEff( ioBaseChar *pChar )
{
	D3DXVECTOR3 vPos, vDir, vSide;
	ioMapEffect *pMapEffect = NULL;

	ElementCircleInfoList::iterator iter = m_ElementCircleInfoList.begin();
	while( iter != m_ElementCircleInfoList.end() )
	{
		if( (*iter).m_CircleEffID != -1 )
		{
			pChar->EndMapEffect( (*iter).m_CircleEffID, false );
			(*iter).m_CircleEffID = -1;
		}

		vPos = m_vCirclePos;

		vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		vPos += vDir * (*iter).m_fCircleFr;

		D3DXVec3Cross( &vSide, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSide, &vSide );
		vPos += vSide * (*iter).m_fCircleSi;
		
		pMapEffect = pChar->CreateMapEffectBySkill( (*iter).m_CircleEff, vPos );

		++iter;
	}
}

void ioCircleMove4Skill::SetElementAtt( ioBaseChar *pChar )
{
	m_bSetElementAtt = true;

	D3DXVECTOR3 vPos, vTargetPos, vMoveDir, vDir, vSide;
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	ioWeapon *pWeapon = NULL;

	ElementCircleInfoList::iterator iter = m_ElementCircleInfoList.begin();
	while( iter != m_ElementCircleInfoList.end() )
	{
		pWeapon = NULL;

		// end eff
		if( (*iter).m_AttEffID != -1 )
		{
			pChar->EndMapEffect( (*iter).m_AttEffID, false );
			(*iter).m_AttEffID = -1;
		}

		// target pos
		vTargetPos = m_vCirclePos;

		vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		vTargetPos += vDir * (*iter).m_fCircleFr;

		D3DXVec3Cross( &vSide, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSide, &vSide );
		vTargetPos += vSide * (*iter).m_fCircleSi;

		// start pos
		vPos = pChar->GetWorldPosition();

		vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		vPos += vDir * (*iter).m_fAttackFr;

		D3DXVec3Cross( &vSide, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSide, &vSide );
		vPos += vSide * (*iter).m_fAttackSi;

		vPos.y += (*iter).m_fAttackHe;

		// move dir
		vMoveDir = vTargetPos - vPos;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		// weapon gen
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

		switch( m_iCurElement )
		{
		case ioAttackableItem::MET_FIRE:
			pWeapon = pChar->SkillFireExplicit( (*iter).m_FireAtt, GetName(), m_dwWeaponBaseIndex, vPos );
			break;
		case ioAttackableItem::MET_ICE:
			pWeapon = pChar->SkillFireExplicit( (*iter).m_IceAtt, GetName(), m_dwWeaponBaseIndex, vPos );
			break;
		case ioAttackableItem::MET_LIGHT:
			pWeapon = pChar->SkillFireExplicit( (*iter).m_LightAtt, GetName(), m_dwWeaponBaseIndex, vPos );
			break;
		}

		if( pWeapon )
		{
			pWeapon->SetMoveDir( vMoveDir );
		}

		++iter;
	}	
}

void ioCircleMove4Skill::UpdateElementEff( ioBaseChar *pChar, ioPlayStage *pStage )
{
	// element
	D3DXVECTOR3 vPos, vDir, vSide;
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	ioMapEffect *pMapEffect = NULL;

	ElementCircleInfoList::iterator iter = m_ElementCircleInfoList.begin();
	while( iter != m_ElementCircleInfoList.end() )
	{
		pMapEffect = NULL;

		// element circle eff
		if( pChar->IsNeedProcess() )
		{
			vPos = m_vCirclePos;

			vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			vPos += vDir * (*iter).m_fCircleFr;

			D3DXVec3Cross( &vSide, &ioMath::UNIT_Y, &vDir );
			D3DXVec3Normalize( &vSide, &vSide );
			vPos += vSide * (*iter).m_fCircleSi;

			pMapEffect = pStage->FindMapEffect( (*iter).m_CircleEffID );
			if( pMapEffect )
			{
				pMapEffect->SetWorldPosition( vPos );
				pMapEffect->SetWorldOrientation( qtRot );
			}
		}

		// element att eff
		vPos = pChar->GetWorldPosition();

		vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		vPos += vDir * (*iter).m_fAttackFr;

		D3DXVec3Cross( &vSide, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSide, &vSide );
		vPos += vSide * (*iter).m_fAttackSi;

		vPos.y += (*iter).m_fAttackHe;

		pMapEffect = pStage->FindMapEffect( (*iter).m_AttEffID );
		
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( vPos );
			pMapEffect->SetWorldOrientation( qtRot );
		}
		
		++iter;
	}
}

