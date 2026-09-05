

#include "stdafx.h"

#include "ioCircleAttackSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

ioCircleAttackSkill::ioCircleAttackSkill()
{
	m_State = SS_NONE;
	m_AreaWeaponState = AS_DELAY;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;
	m_dwAreaWeaponIndex = 0;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwCircleCheckTime = 0;

	//m_bCreateStruct = false;

	m_dwAttackCheckTime = 0;
	m_dwLoopStartTime = 0;
}

ioCircleAttackSkill::ioCircleAttackSkill( const ioCircleAttackSkill &rhs )
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
	m_RedEffect( rhs.m_RedEffect ),
	m_BlueEffect( rhs.m_BlueEffect ),
	m_dwCircleCheckDuration( rhs.m_dwCircleCheckDuration ),
	m_RedGuidCircle( rhs.m_RedGuidCircle ),
	m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
	//m_iStructNum( rhs.m_iStructNum ),
	//m_fStructOffset( rhs.m_fStructOffset ),
	//m_fStructHeightOffset( rhs.m_fStructHeightOffset ),
	m_bUseAttack( rhs.m_bUseAttack ),
	m_fAttackOffset( rhs.m_fAttackOffset ),
	m_fAttackHeightOffset( rhs.m_fAttackHeightOffset ),
	m_CallAttributeList( rhs.m_CallAttributeList ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration ),
	m_LoopAnimation( rhs.m_LoopAnimation ),
	m_dwLoopTime( rhs.m_dwLoopTime ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fMoveRange( rhs.m_fMoveRange ),
	//m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	//m_DummyCharList( rhs.m_DummyCharList ),
	m_GaugeUseType( rhs.m_GaugeUseType ),
	m_bHandMeshOnDelayAni( rhs.m_bHandMeshOnDelayAni ),
	m_stLastAttackMotion( rhs.m_stLastAttackMotion ),
	m_fLastAttackMotionRate( rhs.m_fLastAttackMotionRate ),
	m_stLastAttackFindBuff( rhs.m_stLastAttackFindBuff ),
	m_stLastAttackAddBuff( rhs.m_stLastAttackAddBuff )
{
	m_State = SS_NONE;
	m_AreaWeaponState = AS_DELAY;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;
	m_dwAreaWeaponIndex = 0;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwCircleCheckTime = 0;

	//m_bCreateStruct = false;

	m_dwAttackCheckTime = 0;
	m_dwLoopStartTime = 0;
	m_bCreatedWeapon = false;
}

ioCircleAttackSkill::~ioCircleAttackSkill()
{
	/*m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();*/
}

void ioCircleAttackSkill::LoadProperty( ioINILoader &rkLoader )
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

	// Struct
	/*m_iStructNum = rkLoader.LoadInt_e( "struct_num", 0 );
	m_fStructOffset = rkLoader.LoadFloat_e( "struct_offset", 0.0f );
	m_fStructHeightOffset = rkLoader.LoadFloat_e( "struct_height_offset", 0.0f );*/

	// Attack
	m_bUseAttack = rkLoader.LoadBool_e( "use_attack", false );
	m_fAttackOffset = rkLoader.LoadFloat_e( "attack_offset", 0.0f );
	m_fAttackHeightOffset = rkLoader.LoadFloat_e( "attack_height_offset", 0.0f );

	LoadCallAttributeList( rkLoader );

	//
	/*m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );*/

	m_GaugeUseType = (GaugeUseType)rkLoader.LoadInt_e( "gauge_use_type", GUT_ON_KEY_INPUT );

	m_bHandMeshOnDelayAni = rkLoader.LoadBool_e( "hand_mesh_on_delay_ani", false );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "last_attack_motion", "", szBuf, MAX_PATH );
	m_stLastAttackMotion = szBuf;
	m_fLastAttackMotionRate = rkLoader.LoadFloat_e( "last_attack_motion_rate", FLOAT1 );
	rkLoader.LoadString_e( "last_attack_find_buff", "", szBuf, MAX_PATH );
	m_stLastAttackFindBuff = szBuf;
	rkLoader.LoadString_e( "last_attack_add_buff", "", szBuf, MAX_PATH );
	m_stLastAttackAddBuff = szBuf;
}

void ioCircleAttackSkill::LoadCallAttributeList( ioINILoader &rkLoader )
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

ioSkill* ioCircleAttackSkill::Clone()
{
	return new ioCircleAttackSkill( *this );
}

bool ioCircleAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	//m_bCreateStruct = false;

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

	m_AreaWeaponState = AS_DELAY;

	m_dwProtectTime = 0;
	m_dwLoopStartTime = 0;
	m_dwAttackCheckTime = FRAMEGETTIME();

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_vMoveDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_dwAreaWeaponIndex = 0;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	//SetWaitState( pChar );
	SetActionState( pChar );

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

	//////////////////////////////////////////////////////////////////////////
	//CreateMapEffect
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, 0.0f, 0.0f, true, true );
	}

	float fCurOffSet = GetOwnerItem()->GetMagicCircleOffSet();
	if( fCurOffSet == 0.0f )
		fCurOffSet = m_fCircleOffSet;

	m_vCirclePos = pChar->GetMagicCirclePos( fCurOffSet, m_bCircleNoChangeY );

	return true;
}

void ioCircleAttackSkill::OnSkillEnd( ioBaseChar *pChar )
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

	DestroyAreaWeapon();
}

void ioCircleAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	DWORD dwCurTime = FRAMEGETTIME();
	/*ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
		fValue = pUpInfo->GetValue(pStage);*/

	float fCurRange = m_fCircleRange;// + fValue;
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_State )
	{
	//case SS_WAIT:
	//	{
	//		// 뱡향 변화 체크
	//		pChar->CheckChangeDirectionForMagicCircle();

	//		// MagicCircle 위치 갱신
	//		pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );

	//		// MagicCircle 위치방향으로 캐릭터 회전
	//		D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	//		pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

	//		if( pChar->IsNeedProcess() )
	//		{
	//			if( pChar->IsAttackKey() ||
	//				pChar->IsDefenseKey() ||
	//				pChar->IsJumpKey() ||
	//				pChar->GetSkillKeyInput() == iSlot )
	//			{
	//				SetActionState( pChar, pStage );
	//			}
	//		}
	//	}
	//	break;
	case SS_ACTION:

		{
			if( m_dwFireStartTime < FRAMEGETTIME() && !m_bCreatedWeapon )
			{
				m_bCreatedWeapon = true;
				CheckCreateAreaWeapon( pChar, pStage );		
				CheckCallingAttribute( pChar );
			}

			if( m_dwMotionEndTime < FRAMEGETTIME() )
			{
				SetLoopState( pChar, pStage );
			}
		}

		break;
	case SS_LOOP:
		{
			/*if( m_dwLoopStartTime > 0 )
			{
				CheckControl( pChar );
				ProcessControl( pChar, pStage );
			}*/

			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				if( FindLastAttackTarget( pChar, pStage) )
					SetLastAction( pChar, pStage );
				else 
					SetEndState( pChar, true );

				return;
			}

			if(pChar->IsNeedProcess() && m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < dwCurTime )
			{
				m_dwLoopStartTime = 0;
				SetEndState( pChar, true );
			}
		}
		break;
	case SS_LAST_ATTACK:
		if( dwCurTime > m_dwMotionEndTime )
			SetEndState( pChar );

		break;
	case SS_END:
		break;
	}
}

bool ioCircleAttackSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCircleAttackSkill::SetNormalSkill( ioBaseChar *pChar )
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

	//m_dwEnableReserveTime = FRAMEGETTIME();
	//m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

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

/*void ioCircleAttackSkill::SetWaitState( ioBaseChar *pChar )
{
	m_State = SS_WAIT;
	m_dwPreDelayStartTime = 0;

	if( m_dwCircleCheckDuration > 0 )
		m_dwCircleCheckTime = FRAMEGETTIME();
	else
		m_dwCircleCheckTime = 0;

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
}*/

void ioCircleAttackSkill::SetActionState( ioBaseChar *pChar/*, ioPlayStage *pStage*/ )
{
	m_State = SS_ACTION;

	m_dwCircleCheckTime = 0;

	/*float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
		fValue = pUpInfo->GetValue(pStage);*/

	float fCurRange = m_fCircleRange;// + fValue;

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

	/*if( pChar->IsNeedSendNetwork() )
	{
	SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
	kPacket << pChar->GetCharName();
	kPacket << GetName();

	kPacket << SSS_ACTION;
	P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}

void ioCircleAttackSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
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

	if( m_dwCurMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurMapEffect, false );

		m_dwCurMapEffect = -1;
	}

	pChar->DestroyMagicCircle();

	m_State = SS_LOOP;
}


void ioCircleAttackSkill::SetLastAction( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_stLastAttackMotion );
	if( iCurSkillAniID == -1 )	
	{
		SetEndState( pChar );
		return;
	}

	float fTimeRate = m_fLastAttackMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_State = SS_LAST_ATTACK;

	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = FRAMEGETTIME() + dwDuration;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pChar->GetTeam() ) 
			continue;

		if( pTarget->GetBuff( m_stLastAttackFindBuff, pChar->GetCharName() ) )
			pTarget->AddNewBuff( m_stLastAttackAddBuff, pChar->GetCharName(), "", NULL );
			//pTarget->ReserveAddNewBuff( m_stLastAttackAddBuff, pChar->GetCharName(), "", NULL );
	}

	DestroyAreaWeapon();

	//sendpacke
	if( pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SS_LAST_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	//if( iSyncState == SSS_ACTION )
	//{
	//	SetActionState( pChar, pStage );
	//}
	//else 

	if( iSyncState == SS_LAST_ATTACK )
	{
		SetLastAction( pChar, pStage );
	}
	else
		SetEndState( pChar, false );


	/*if( iSyncState == SSS_UPDATE && CheckAreaWeapon() )
	{
		int iAreaWeaponState;
		rkPacket >> iAreaWeaponState;

		m_AreaWeaponState = (AreaWeaponState)iAreaWeaponState;
		rkPacket >> m_vMoveDir;
	}*/
}

void ioCircleAttackSkill::CheckCreateAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
	{
		if( m_dwCurMapEffect != -1 )
			pChar->EndMapEffect( m_dwCurMapEffect, false );

		// Create AreaWeapon
		if( !m_AreaWeaponName.IsEmpty() )
		{
			D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();

			vDir.y = 0.0f;
			D3DXVec3Normalize(&vDir, &vDir);

			D3DXVECTOR3 vCreatePos = m_vCirclePos;// - (m_fStructOffset*vDir);
			//vCreatePos.y += m_fStructHeightOffset;

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vDir );

			ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
				"",
				GetName(),
				m_AreaWeaponName,
				m_vCirclePos,
				pChar->GetTargetRot(),
				ioAreaWeapon::CT_NORMAL );

			if( pArea )
			{
				m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
			}
		}

		//CreateStruct( pChar, pStage );
		//CheckDummyChar( pChar, pStage );

		m_dwFireStartTime = 0;
	}
}


bool ioCircleAttackSkill::FindLastAttackTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pChar->GetTeam() ) 
			continue;

		if( pTarget->GetBuff( m_stLastAttackFindBuff, pChar->GetCharName() ) )
			return true;
	}

	return false;
}

bool ioCircleAttackSkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

void ioCircleAttackSkill::SetEndState( ioBaseChar *pChar, bool bSend )
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

	DestroyAreaWeapon();

	if( bSend && pChar->IsNeedProcess() )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
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


bool ioCircleAttackSkill::IsProtected( int iDefenseBreakType ) const
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
	//case SS_WAIT:
	case SS_END:
		return false;
	case SS_ACTION:
		return true;
	case SS_LOOP:
		{
			if( m_dwLoopProtectDuration == 0 )
				return true;

			if( m_dwProtectTime > dwCurTime )
				return true;
		}
	}

	return false;
}

bool ioCircleAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioCircleAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}


void ioCircleAttackSkill::CheckCallingAttribute( ioBaseChar *pChar )
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

void ioCircleAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon )
	{
		pWeapon->SetMoveDir( m_vAttackDir );
	}
}

/*void ioCircleAttackSkill::CheckControl( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	if( m_dwAreaWeaponIndex == 0 )
		return;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( !pAreaWeapon )
	{
		m_dwAreaWeaponIndex = 0;
		return;
	}

	bool bUpdate = false;
	if( m_AreaWeaponState == AS_DELAY )
	{
		if( pChar->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
			if( eDir != ioUserKeyInput::DKI_NONE )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				m_vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			}

			m_AreaWeaponState = AS_MOVE;
			bUpdate = true;
		}
	}
	else if( m_AreaWeaponState == AS_MOVE )
	{
		if( !pChar->IsSettedDirection() )
		{
			m_AreaWeaponState = AS_DELAY;
			bUpdate = true;
		}
		else
		{
			if( pChar->IsChangeDirection() )
			{
				ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
				if( eDir != ioUserKeyInput::DKI_NONE )
				{
					float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
					fYawD = ioMath::ArrangeHead( fYawD );

					float fYawR = DEGtoRAD( fYawD );
					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

					m_vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				}

				bUpdate = true;
			}
		}
	}

	if( bUpdate && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SSS_UPDATE;
		kPacket << m_AreaWeaponState;
		kPacket << m_vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}*/

/*void ioCircleAttackSkill::ProcessControl( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwAreaWeaponIndex == 0 )
		return;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( !pAreaWeapon )
	{
		m_dwAreaWeaponIndex = 0;
		return;
	}

	if( m_AreaWeaponState == AS_MOVE && m_fMoveSpeed > 0.0f )
	{
		D3DXVECTOR3 vPrePos, vCurPos, vPos;
		vPos = m_vMoveDir;
		vPos *= m_fMoveSpeed * g_FrameTimer.GetSecPerFrame();

		vPrePos = vCurPos = pAreaWeapon->GetStartPos();
		vCurPos += vPos;

		float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pChar, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;

		D3DXVECTOR3 vCreatePos = m_vCirclePos;
		D3DXVECTOR3 vCurXZ = vCurPos;

		vCreatePos.y = 0.0f;
		vCurXZ.y = 0.0f;

		D3DXVECTOR3 vDiff = vCurXZ - vCreatePos;

		if( D3DXVec3Length( &vDiff ) > m_fMoveRange )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			vDiff *= m_fMoveRange;

			float fCurHeight = vCurPos.y;
			vCurPos = vCreatePos + vDiff;
			vCurPos.y = fCurHeight;

			fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pChar, false );
			if( fMapHeight > 0.0f )
				vCurPos.y = fMapHeight;
		}

		float fGapHeight = fabs( vPrePos.y - vCurPos.y );
		if( fGapHeight > pChar->GetMidHeightByRate() )
			vCurPos = vPrePos;

		pAreaWeapon->SetAreaPosition( vCurPos, pStage );
		pChar->SetTargetRotToTargetPos( vCurPos, true, false );
	}
}*/

bool ioCircleAttackSkill::CheckAreaWeapon()
{
	if( m_dwAreaWeaponIndex == 0 )
		return false;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon )
		return true;

	return false;
}

const D3DXVECTOR3& ioCircleAttackSkill::GetAreaWeaponPos()
{
	if( m_dwAreaWeaponIndex == 0 )
		return m_vCirclePos;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon )
		return pAreaWeapon->GetStartPos();

	return m_vCirclePos;
}

const D3DXVECTOR3& ioCircleAttackSkill::GetAreaWeaponPrePos()
{
	if( m_dwAreaWeaponIndex == 0 )
		return m_vCirclePos;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon )
		return pAreaWeapon->GetPreStartPos();

	m_dwAreaWeaponIndex = 0;

	return m_vCirclePos;
}

void ioCircleAttackSkill::DestroyAreaWeapon()
{
	if( m_dwAreaWeaponIndex == 0 )
		return;

	g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
	m_dwAreaWeaponIndex = 0;
}




