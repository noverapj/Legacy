#include "stdafx.h"

#include "ioCircleMove5Skill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

ioCircleMove5Skill::ioCircleMove5Skill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwCircleCheckTime = 0;

	m_dwAttackCheckTime = 0;
}

ioCircleMove5Skill::ioCircleMove5Skill( const ioCircleMove5Skill &rhs )
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
	m_dwJumpCircleCheckDuration( rhs.m_dwJumpCircleCheckDuration ),
	m_RedGuidCircle( rhs.m_RedGuidCircle ),
	m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
	m_bUseAttack( rhs.m_bUseAttack ),
	m_fAttackOffset( rhs.m_fAttackOffset ),
	m_fAttackHeightOffset( rhs.m_fAttackHeightOffset ),
	m_CallAttributeList( rhs.m_CallAttributeList ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_GaugeUseType( rhs.m_GaugeUseType ),
	m_bHandMeshOnDelayAni( rhs.m_bHandMeshOnDelayAni ),
	m_bChangeOwnerPos( rhs.m_bChangeOwnerPos ),
	m_bJumpCallingAttribute( rhs.m_bJumpCallingAttribute ),
	m_CallJumpAttributeList( rhs.m_CallJumpAttributeList ),
	m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	m_DummyCharList( rhs.m_DummyCharList )
{
	m_State = SS_NONE;
	
	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwCircleCheckTime = 0;

	m_dwAttackCheckTime = 0;
}

ioCircleMove5Skill::~ioCircleMove5Skill()
{
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();
}

void ioCircleMove5Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );
		
	m_dwSkillProtectTime    = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	
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

	m_dwCircleCheckDuration = (DWORD)rkLoader.LoadInt_e( "magic_circle_duration", 0 );
	m_dwJumpCircleCheckDuration = (DWORD)rkLoader.LoadInt_e( "jump_magic_circle_duration", 0 );

	// Attack
	m_bUseAttack = rkLoader.LoadBool_e( "use_attack", false );
	m_fAttackOffset = rkLoader.LoadFloat_e( "attack_offset", 0.0f );
	m_fAttackHeightOffset = rkLoader.LoadFloat_e( "attack_height_offset", 0.0f );

	LoadCallAttributeList( rkLoader );

	m_GaugeUseType = (GaugeUseType)rkLoader.LoadInt_e( "gauge_use_type", GUT_ON_KEY_INPUT );

	m_bHandMeshOnDelayAni = rkLoader.LoadBool_e( "hand_mesh_on_delay_ani", false );

	m_bChangeOwnerPos = rkLoader.LoadBool_e( "change_owner_position", false );
	m_bJumpCallingAttribute = rkLoader.LoadBool_e( "use_jump_calling_attribute", false );

	LoadCallJumpAttributeList( rkLoader );

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );

}

void ioCircleMove5Skill::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallAttributeList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 ) 
		return;

	m_CallAttributeList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		CallAttribute kCallAttribute;

		wsprintf_e( szKey, "call_attribute%d_index", i+1 );
		kCallAttribute.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_resistance", i+1 );
		kCallAttribute.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "call_attribute%d_time", i+1 );
		kCallAttribute.m_dwAttributeCallTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		m_CallAttributeList.push_back( kCallAttribute );
	}

	std::sort( m_CallAttributeList.begin(), m_CallAttributeList.end(), CallAttributeSort() );
}

void ioCircleMove5Skill::LoadCallJumpAttributeList( ioINILoader &rkLoader )
{
	m_CallJumpAttributeList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_jump_attribute_cnt", 0 );
	if( iCnt == 0 ) 
		return;

	m_CallJumpAttributeList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		CallAttribute kCallAttribute;

		wsprintf_e( szKey, "call_jump_attribute%d_index", i+1 );
		kCallAttribute.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_jump_attribute%d_resistance", i+1 );
		kCallAttribute.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_jump_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_jump_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_jump_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "call_jump_attribute%d_time", i+1 );
		kCallAttribute.m_dwAttributeCallTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		m_CallJumpAttributeList.push_back( kCallAttribute );
	}

	std::sort( m_CallJumpAttributeList.begin(), m_CallJumpAttributeList.end(), CallAttributeSort() );
}

ioSkill* ioCircleMove5Skill::Clone()
{
	return new ioCircleMove5Skill( *this );
}

bool ioCircleMove5Skill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

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
	m_dwAttackCheckTime = FRAMEGETTIME();

	m_vAttackDir = ioMath::VEC3_ZERO;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	SetWaitState( pChar );

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

	return true;
}

void ioCircleMove5Skill::OnSkillEnd( ioBaseChar *pChar )
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

void ioCircleMove5Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	DWORD dwCurTime = FRAMEGETTIME();
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
		fValue = pUpInfo->GetValue(pStage);

	float fCurRange = m_fCircleRange + fValue;
	int iSlot = GetOwnerItem()->GetType() - 1;

	float fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	switch( m_State )
	{
	case SS_WAIT:
		{
			if ( fHeightGap > FLOAT1 && pChar->GetState() == CS_USING_SKILL )
			{
				if ( m_dwSkillStartTime > 0 && m_dwSkillStartTime + m_dwJumpCircleCheckDuration < dwCurTime )
				{
					SetEndState( pChar );
				}
			}
			else if ( m_dwSkillStartTime > 0 && m_dwSkillStartTime + m_dwCircleCheckDuration < dwCurTime )
			{
				SetEndState( pChar );
			}

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
					SetActionState( pChar, pStage );
				}
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
				
			if ( m_bChangeOwnerPos )
			{
				CheckChangePostion( pChar, pStage );
			}
			CheckDummyChar( pChar, pStage );
			SetEndState( pChar );
		}
		else
		{
			if ( !m_bJumpCallingAttribute )
			{
				CheckCallingAttribute( pChar );
			}
			else
			{
				if ( fHeightGap > FLOAT1 && pChar->GetState() == CS_USING_SKILL )
				{
					CheckCallingJumpAttribute( pChar );
				}
				else
				{
					CheckCallingAttribute( pChar );
				}
			}				
		}
		break;
	case SS_END:
		break;
	}
}

bool ioCircleMove5Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCircleMove5Skill::SetNormalSkill( ioBaseChar *pChar )
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

void ioCircleMove5Skill::SetWaitState( ioBaseChar *pChar )
{
	m_State = SS_WAIT;
	m_dwPreDelayStartTime = 0;

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

	if( m_bHandMeshOnDelayAni )
	{
		if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
		{
			pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
			m_bSetHandMesh = true;
		}
	}
}

void ioCircleMove5Skill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;

	m_dwCircleCheckTime = 0;

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
		fValue = pUpInfo->GetValue(pStage);

	float fCurRange = m_fCircleRange + fValue;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	}
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

void ioCircleMove5Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	if( iSyncState == SSS_ACTION )
	{
		SetActionState( pChar, pStage );
	}
	else if ( iSyncState == SSS_DUMMY )
	{
		g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

		D3DXQUATERNION qtRot, qtTargetRot;
		rkPacket >> qtRot >> qtTargetRot;

		pChar->SetTargetRotToRotate( qtRot, true, false );

		int iCreateCnt;
		rkPacket >> iCreateCnt;

		for( int i=0; i < iCreateCnt; ++i )
		{
			int iCurIndex;
			ioHashString szDummyCharName;
			float fStartAngle;
			D3DXVECTOR3 vTargetPos;

			rkPacket >> szDummyCharName >> iCurIndex;
			rkPacket >> fStartAngle;
			rkPacket >> vTargetPos;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
				pChar->GetCharName(),
				vTargetPos,
				fStartAngle,
				0, false );

			if( pDummy )
			{
				pDummy->SetControlSkillName( GetName() );
				pDummy->SetWorldOrientation( qtTargetRot );
			}
		}
	}
}

bool ioCircleMove5Skill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

void ioCircleMove5Skill::SetEndState( ioBaseChar *pChar )
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

bool ioCircleMove5Skill::IsProtected( int iDefenseBreakType ) const
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

bool ioCircleMove5Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioCircleMove5Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMove5Skill::CheckCallingAttribute( ioBaseChar *pChar )
{
	if( !m_bUseAttack ) return;
	if( !pChar ) return;
	if( m_CallAttributeList.empty() ) return;
	if( m_dwAttackCheckTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_vAttackDir = m_vCirclePos - pChar->GetWorldPosition();
	m_vAttackDir.y = 0.0f;
	D3DXVec3Normalize(&m_vAttackDir, &m_vAttackDir);

	D3DXVECTOR3 vCreatePos = m_vCirclePos - (m_fAttackOffset*m_vAttackDir);
	vCreatePos.y += m_fAttackHeightOffset;

	int iCallCnt = 0;
	CallAttributeList::iterator iter = m_CallAttributeList.begin();
	while( iter != m_CallAttributeList.end() )
	{
		DWORD dwCallTime = (*iter).m_dwAttributeCallTime + m_dwActionStartTime;

		if( COMPARE(dwCallTime, m_dwAttackCheckTime, dwCurTime) )
		{
			pChar->SkillFireExplicit( (*iter).m_WeaponInfo, GetName(), m_dwWeaponBaseIndex+iCallCnt, vCreatePos );
			iCallCnt++;
		}

		++iter;
	}

	m_dwAttackCheckTime = dwCurTime;
}

void ioCircleMove5Skill::CheckCallingJumpAttribute( ioBaseChar *pChar )
{
	if( !m_bUseAttack ) return;
	if( !pChar ) return;
	if( m_CallJumpAttributeList.empty() ) return;
	if( m_dwAttackCheckTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_vAttackDir = m_vCirclePos - pChar->GetWorldPosition();
	m_vAttackDir.y = 0.0f;
	D3DXVec3Normalize(&m_vAttackDir, &m_vAttackDir);

	D3DXVECTOR3 vCreatePos = m_vCirclePos - (m_fAttackOffset*m_vAttackDir);
	vCreatePos.y += m_fAttackHeightOffset;

	int iCallCnt = 0;
	CallAttributeList::iterator iter = m_CallJumpAttributeList.begin();
	while( iter != m_CallJumpAttributeList.end() )
	{
		DWORD dwCallTime = (*iter).m_dwAttributeCallTime + m_dwActionStartTime;

		if( COMPARE(dwCallTime, m_dwAttackCheckTime, dwCurTime) )
		{
			pChar->SkillFireExplicit( (*iter).m_WeaponInfo, GetName(), m_dwWeaponBaseIndex+iCallCnt, vCreatePos );
			iCallCnt++;
		}

		++iter;
	}

	m_dwAttackCheckTime = dwCurTime;
}

void ioCircleMove5Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon )
	{
		pWeapon->SetMoveDir( m_vAttackDir );
	}
}

void ioCircleMove5Skill::CheckChangePostion( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	auto vChangePos = m_vCirclePos;
	pChar->SetWorldPosition( vChangePos );
}

void ioCircleMove5Skill::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_DummyCharList.empty() )
		return;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	//
	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();
	vDir.y = 0.0f;
	D3DXVec3Normalize(&vDir, &vDir);

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	D3DXVECTOR3 vCreatePos = m_vCirclePos;

	D3DXQUATERNION qtSightRot = pChar->GetTargetRot();
	D3DXVECTOR3 vSightDir = qtSightRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vDir );

	int iSize = m_DummyCharList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
		float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

		D3DXVECTOR3 vTargetPos = vCreatePos + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
			pChar->GetCharName(),
			vTargetPos,
			fStartAngle,
			0, true );

		if( pDummy )
		{
			pDummy->SetControlSkillName( GetName() );
			pDummy->SetWorldOrientation( qtTargetRot );
		}

		DummyCharCreateInfo kInfo;
		kInfo.m_DummyCharName = szDummyCharName;
		kInfo.m_iDummyCharIdx = iCurIndex;
		kInfo.m_fDummyCharStartAngle = fStartAngle;
		kInfo.m_vTargetPos = vTargetPos;

		vCreateList.push_back( kInfo );
	}

	int iCreateCnt = vCreateList.size();
	if( pChar->IsNeedSendNetwork() && iCreateCnt > 0 )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_DUMMY;
		kPacket << pChar->GetTargetRot();
		kPacket << qtTargetRot;

		kPacket << iCreateCnt;

		for( int i=0; i < iCreateCnt; ++i )
		{
			kPacket << vCreateList[i].m_DummyCharName;
			kPacket << vCreateList[i].m_iDummyCharIdx;
			kPacket << vCreateList[i].m_fDummyCharStartAngle;
			kPacket << vCreateList[i].m_vTargetPos;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
