
#include "stdafx.h"

#include "ioAreaStruct2Skill.h"


#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioAreaStruct2Skill::ioAreaStruct2Skill()
{
	ClearData();
}

ioAreaStruct2Skill::ioAreaStruct2Skill( const ioAreaStruct2Skill &rhs )
: ioNormalSkill( rhs ),
m_fOffSet( rhs.m_fOffSet ),
m_fYOffSet( rhs.m_fYOffSet ),
m_AreaWeaponName( rhs.m_AreaWeaponName ),
m_RedCircle( rhs.m_RedCircle ),
m_BlueCircle( rhs.m_BlueCircle ),
m_dwCircleDuration( rhs.m_dwCircleDuration ),
m_iBildStruct( rhs.m_iBildStruct ),
m_BildEffect( rhs.m_BildEffect ),
m_bEnableCheckHeight ( rhs.m_bEnableCheckHeight ),
m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
m_fDownLimitHeight( rhs.m_fDownLimitHeight ),
m_iMaxStructCnt( rhs.m_iMaxStructCnt ),
m_vForceInfoList( rhs.m_vForceInfoList ),
m_szSkillLoopMotion( rhs.m_szSkillLoopMotion ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
m_FinishAttackAttribute( rhs.m_FinishAttackAttribute ),
m_dwChargingMaxTime( rhs.m_dwChargingMaxTime ),
m_dwEnableKeyTime( rhs.m_dwEnableKeyTime )
{
	ClearData();
}

ioAreaStruct2Skill::~ioAreaStruct2Skill()
{
}

void ioAreaStruct2Skill::ClearData()
{
	m_SkillState = SS_NONE;

	m_dwCurMapEffect = -1;

	m_dwCircleEnd = 0;
	m_dwCreateTime = 0;
	m_dwAreaWeaponIndex = 0;
	m_dwFireMotionEndTime = 0;
	m_dwSkillLoopStartTime = 0;
	m_dwCheckStartTime		= 0;

	m_fCurChargeRate = 0.0f;

	m_bSetHandMesh = false;

	m_dwAttackEndTime = 0;
}

void ioAreaStruct2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_dwCircleDuration = rkLoader.LoadInt_e( "circle_duration", 0 );

	m_fOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );
	m_fYOffSet = rkLoader.LoadFloat_e( "circle_y_offset", 0.0f );

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;


	rkLoader.LoadString_e( "bild_effect", "", szBuf, MAX_PATH );
	m_BildEffect = szBuf;

	m_iBildStruct = rkLoader.LoadInt_e( "bild_struct_num", 0 );

	m_bEnableCheckHeight = rkLoader.LoadBool_e( "enable_check_height", false );

	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 0.0f );
	m_fDownLimitHeight = rkLoader.LoadFloat_e( "down_limit_height", 0.0f );
	m_iMaxStructCnt = rkLoader.LoadInt_e( "max_struct_cnt", 0 );

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "skill_motion_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		m_vForceInfoList.reserve( iForceCnt );
	}

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "skill_motion_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "skill_motion_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		wsprintf_e( szBuf, "skill_motion_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vForceInfoList.push_back( kInfo );
	}

	// LoopMotion
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szSkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate		= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration		= (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwSkillLoopProtectTime	= (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	// FinishAttack
	m_FinishAttackAttribute.Init();
	LoadAttackAttribute_e( "skill_finish_attack", m_FinishAttackAttribute, rkLoader );

	// Charging
	m_dwChargingMaxTime	= (DWORD)rkLoader.LoadInt_e( "charging_max_time", 0 );
	m_dwEnableKeyTime	= (DWORD)rkLoader.LoadInt_e( "key_enable_time", 0 );
}

ioSkill* ioAreaStruct2Skill::Clone()
{
	return new ioAreaStruct2Skill( *this );
}

bool ioAreaStruct2Skill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioAreaStruct2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_LOOP:
		return false;	
	case SS_END:	
		if( m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
			return true;
		return false;
	}

	return true;
}

bool ioAreaStruct2Skill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

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
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
	}

	if( m_dwSkillLoopStartTime + m_dwSkillLoopProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioAreaStruct2Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	if( m_AreaWeaponName.IsEmpty() && m_iBildStruct == 0 )
		return false;

	ClearData();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	pChar->SetSkillProtection();

	m_dwAttackEndTime = 0;

	//CreateMapEffect
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, 0.0f, 0.0f, true, true );
	}

	float fCurOffSet = GetOwnerItem()->GetMagicCircleOffSet();
	float fCurYOffSet = GetOwnerItem()->GetMagicCircleYOffSet();
	if( fCurOffSet == 0.0f )
		fCurOffSet = m_fOffSet;

	if( fCurYOffSet == 0.0f )
		fCurYOffSet = m_fYOffSet;

	if( fCurYOffSet != 0.0f )
		m_vCreatePos = pChar->GetMagicCirclePos( fCurOffSet, fCurYOffSet );
	else
		m_vCreatePos = pChar->GetMagicCirclePos( fCurOffSet, m_bCircleNoChangeY );

	if( m_bEnableCheckHeight )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			float fHeight = pStage->GetMapHeightNoneEntity( m_vCreatePos.x, m_vCreatePos.z, m_vCreatePos.y );
			m_vCreatePos.y = fHeight;
		}
	}
	
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCreatePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCreatePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCreatePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCreatePos );
	}

	if( pMapEffect )
	{
		DWORD dwPreDelayTime = GetPreDelayTime();

		m_dwCurMapEffect = pMapEffect->GetUniqueID();
		m_dwCircleEnd = FRAMEGETTIME() + dwPreDelayTime + m_dwCircleDuration;
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetActionState( pChar );
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioAreaStruct2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwCircleEnd > 0 && m_dwCircleEnd < FRAMEGETTIME() )
	{
		//DestroyMapEffect
		pChar->EndMapEffect( m_dwCurMapEffect );
		pChar->DestroyMagicCircle();
		m_dwCircleEnd = 0;
	}

	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetActionState( pChar );
			}
			else if( m_dwPreDelayStartTime == 0 )
			{
				CheckExtraAniJump( pChar );

				if( m_dwCreateTime != 0 && m_dwCreateTime < dwCurTime )
				{
					CreateArea( pChar, pStage );
					CreateStruct( pChar, pStage );

					pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
					m_bReduceGauge = true;

					m_dwCreateTime = 0;

					SetLoopState( pChar );
				}
			}
		}
		break;
	case SS_LOOP:
		{
			if( m_dwSkillLoopStartTime > 0 && m_dwSkillLoopStartTime + m_dwSkillLoopDuration < dwCurTime )
			{
				SetFinishAttackFire( pChar, false );
				return;
			}

			if( m_dwCheckStartTime + m_dwEnableKeyTime < dwCurTime )
			{
				if( pChar->IsAttackKey() )
				{
					m_dwCheckStartTime = dwCurTime;
					m_fCurChargeRate += (float)m_dwEnableKeyTime / m_dwChargingMaxTime;
					if( m_fCurChargeRate > FLOAT1 )
						m_fCurChargeRate = FLOAT1;
				}
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioAreaStruct2Skill::OnSkillEnd( ioBaseChar *pOwner )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	//AreaDestroy
	// 스위치 타입일 경우는 스킬 종료시 영역도 소멸
	if( IsCanSwitchSkill() )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
	}

	//DestroyMapEffect
	pOwner->EndMapEffect( m_dwCurMapEffect );
	pOwner->DestroyMagicCircle();

	ioNormalSkill::OnSkillEnd( pOwner );
}

void ioAreaStruct2Skill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );
}

void ioAreaStruct2Skill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );
}

bool ioAreaStruct2Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioAreaStruct2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
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

void ioAreaStruct2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioHashString szAreaWeaponName;
	rkPacket >> szAreaWeaponName;

	int iCreateType;
	D3DXVECTOR3 vPos;
	rkPacket >> vPos >> iCreateType;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
															"",
															GetName(),
															szAreaWeaponName,
															vPos,
															qtRot,
															iCreateType );

	if( pArea )
	{
		m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
	}
}


void ioAreaStruct2Skill::CreateArea( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_AreaWeaponName.IsEmpty() ) return;
	if( !pOwner ) return;

	if( pOwner->IsNeedProcess() )
	{
		ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
																"",
																GetName(),
																m_AreaWeaponName,
																m_vCreatePos,
																pOwner->GetTargetRot(),
																ioAreaWeapon::CT_NORMAL );
		if( pArea )
		{
			m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();

			//UPD로 생성 상황 동기화 맞춰줄것
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << m_AreaWeaponName;
				kPacket << m_vCreatePos;
				kPacket << (int)ioAreaWeapon::CT_NORMAL;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioAreaStruct2Skill::CreateStruct( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_iBildStruct == 0 )
		return;

	int iDestroyIndex = 0;
	ioPushStruct *pDelete = g_PushStructListMgr.CheckMaxStructCntByNum( pChar, m_iBildStruct, m_iMaxStructCnt );
	if( pDelete )
	{
		iDestroyIndex = pDelete->GetPushArrayIdx();
	}

	pChar->CreateMapEffectBySkill( m_BildEffect, m_vCreatePos );

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			ioHashString szItemName;
			if( GetOwnerItem() )
				szItemName = GetOwnerItem()->GetName();

			SP2Packet kPacket( CTPK_PUSHSTRUCT_CREATE );
			kPacket << iDestroyIndex;
			kPacket << m_iBildStruct;
			kPacket << m_vCreatePos;
			kPacket << pChar->GetTargetRot();
			kPacket << pChar->GetCharName();
			kPacket << szItemName;
			kPacket << 0;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		// 싱글모드 뿐만아니라 로비에서도 이쪽으로 오기때문에 주의가 필요함
		if( pDelete )
		{
			pDelete->StructDie( true );
		}

		DWORD dwSeed = timeGetTime();
		int iCurIndex = g_PushStructListMgr.GetCurLocalIndex() + 1;
		pStage->CreatePushStruct( m_iBildStruct,
								  iCurIndex,
								  m_vCreatePos,
								  pChar->GetTargetRot(),
								  pChar->GetCharName(),
								  0,
								  dwSeed,
								  0,
								  true );
	}
}

void ioAreaStruct2Skill::SetActionState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )
		return;

	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pChar->SetReservedSliding( m_vForceInfoList, iCurSkillAniID, fTimeRate, 0 );
	
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_dwCreateTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwCreateTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwCreateTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	//
	SetExtraAniJump( pChar, iCurSkillAniID, fTimeRate );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioAreaStruct2Skill::SetLoopState( ioBaseChar* pChar )
{
	if( !pChar )	return;

	m_SkillState = SS_LOOP;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_szSkillLoopMotion );
	if( iCurSkillAniID == -1 )
		return;

	float fAniRate = m_fSkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fAniRate;
	m_dwCheckStartTime = dwCurTime;

	m_dwSkillLoopStartTime = dwCurTime;
}

void ioAreaStruct2Skill::SetFinishAttackFire( ioBaseChar* pChar, bool bSend )
{
	if( !pChar )	return;
	if( m_SkillState == SS_END )	return;

	m_SkillState = SS_END;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_FinishAttackAttribute.m_AttackAnimation );
	if( iCurSkillAniID == -1 )
		return;

	float fAniRate = m_FinishAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_FinishAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_FinishAttackAttribute.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fAniRate;
	m_dwMotionEndTime += ( dwPreDelay + dwEndDelay );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iCurSkillAniID,
										m_FinishAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fAniRate,
										dwPreDelay );
	//동기화 처리.
}
