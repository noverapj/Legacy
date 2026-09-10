

#include "stdafx.h"

#include "ioCircleMoveAndKeySkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

ioCircleMoveAndKeySkill::ioCircleMoveAndKeySkill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;
	m_dwAreaWeaponIndex = 0;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_bCreateStruct = false;

	m_bAddKey = false;
	m_dwCheckStartTime = 0;
}

ioCircleMoveAndKeySkill::ioCircleMoveAndKeySkill( const ioCircleMoveAndKeySkill &rhs )
: ioNormalSkill( rhs ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_TargetCircleEffect( rhs.m_TargetCircleEffect ),
 m_SkillMotion2( rhs.m_SkillMotion2 ),
 m_fSkillMotion2Rate( rhs.m_fSkillMotion2Rate ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_RedGuidCircle( rhs.m_RedGuidCircle ),
 m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
 m_iStructNum( rhs.m_iStructNum ),
 m_fStructOffset( rhs.m_fStructOffset ),
 m_fStructHeightOffset( rhs.m_fStructHeightOffset ),
 m_WeaponInfo( rhs.m_WeaponInfo ),
 m_dwAddDuration( rhs.m_dwAddDuration ),
 m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
 m_dwCheckDeadTime( rhs.m_dwCheckDeadTime ),
 m_PreDelayType( rhs.m_PreDelayType )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;
	m_dwAreaWeaponIndex = 0;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_bCreateStruct = false;

	m_bAddKey = false;
	m_dwCheckStartTime = 0;
}

ioCircleMoveAndKeySkill::~ioCircleMoveAndKeySkill()
{
}

void ioCircleMoveAndKeySkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_PreDelayType = (PreDelayType)rkLoader.LoadInt_e( "predelay_type", PDT_WAIT );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

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

	rkLoader.LoadString_e( "skill_motion2", "", szBuf, MAX_PATH );
	m_SkillMotion2 = szBuf;
	m_fSkillMotion2Rate = rkLoader.LoadFloat_e( "skill_motion2_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	// Struct
	m_iStructNum = rkLoader.LoadInt_e( "struct_num", 0 );
	m_fStructOffset = rkLoader.LoadFloat_e( "struct_offset", 0.0f );
	m_fStructHeightOffset = rkLoader.LoadFloat_e( "struct_height_offset", 0.0f );

	// key
	m_dwAddDuration = rkLoader.LoadInt_e( "key_add_duration", 0 );
	m_dwEnableKeyTime = rkLoader.LoadInt_e( "key_enable_time", 0 );

	m_dwCheckDeadTime = (DWORD)rkLoader.LoadInt_e( "check_dead_time", 0 );

	// Attack
	wsprintf_e( szKey, "skill_attack_type" );
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "skill_attack_type_resist" );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "skill_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "skill_attack_type_wounded_time" );
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "skill_attack_type_wounded_loop_ani" );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );
}

ioSkill* ioCircleMoveAndKeySkill::Clone()
{
	return new ioCircleMoveAndKeySkill( *this );
}

bool ioCircleMoveAndKeySkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_bCreateStruct = false;

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

	m_bAddKey = false;
	m_dwCheckStartTime = 0;

	m_dwAreaWeaponIndex = 0;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( m_PreDelayType == PDT_WAIT )
		SetWaitState( pChar );
	else if( m_PreDelayType == PDT_NORMAL )
		m_State = SS_NONE;

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

void ioCircleMoveAndKeySkill::OnSkillEnd( ioBaseChar *pChar )
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

	g_WeaponMgr.CheckSkillDependencyWeaponDead( pChar->GetCharName(), GetName() );
}

void ioCircleMoveAndKeySkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurRange = m_fCircleRange;
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetActionState( pChar, pStage );
			}
		}
		break;
	case SS_WAIT:
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
		{
			if( m_dwCheckDeadTime > 0 && m_dwMotionStartTime + m_dwCheckDeadTime < dwCurTime )
			{
				m_State = SS_END;
				return;
			}

			// SetCreateWeaponDead 발생까지 대기
			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
				pChar->SkillFireExplicit( m_WeaponInfo,
										  GetName(),
										  m_dwWeaponBaseIndex,
										  m_vCirclePos );

				m_dwFireStartTime = 0;
			}
		}
		break;
	case SS_ACTION2:
		if( pChar->IsNeedProcess() )
		{
			DWORD dwCheck1 = m_dwCheckStartTime + m_dwEnableKeyTime;
			DWORD dwCheck2 = m_dwCheckStartTime + m_dwAddDuration;

			// 키연타 체크
			if( !m_bAddKey && dwCheck1 < dwCheck2 )
			{
				if( m_dwCheckStartTime > 0 && COMPARE( dwCurTime, dwCheck1, dwCheck2 ) )
				{
					if( pChar->IsAttackKey() )
					{
						m_bAddKey = true;
					}
				}
			}

			if( dwCheck2 < dwCurTime )
			{
				if( m_bAddKey )
				{
					m_bAddKey = false;
					m_dwCheckStartTime = dwCurTime;

					pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem() ) );
				}
				else
				{
					m_State = SS_END;
				}
			}
		}
		break;
	}
}

bool ioCircleMoveAndKeySkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCircleMoveAndKeySkill::SetWaitState( ioBaseChar *pChar )
{
	m_State = SS_WAIT;

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

void ioCircleMoveAndKeySkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;

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
	if( m_PreDelayType == PDT_NORMAL )
		m_vCirclePos = pChar->GetWorldPosition();

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

void ioCircleMoveAndKeySkill::SetNormalSkill( ioBaseChar *pChar )
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
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = dwCurTime;
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_dwFireStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;

	pChar->AttachEffect( m_StartEffect );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioCircleMoveAndKeySkill::SetActionState2( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillMotion2 );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fAniRate = m_fSkillMotion2Rate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = dwCurTime;
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->AttachEffect( m_StartEffect );

	//
	if( m_dwCurMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurMapEffect, false );

		m_dwCurMapEffect = -1;
	}

	pChar->DestroyMagicCircle();

	m_State = SS_ACTION2;
	m_bAddKey = false;
	m_dwCheckStartTime = dwCurTime;

	SetCreateAreaWeapon( pChar, pStage );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_ACTION2;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveAndKeySkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	if( iSyncState == SSS_ACTION )
	{
		SetActionState( pChar, pStage );
	}
	else if( iSyncState == SSS_ACTION2 )
	{
		SetActionState2( pChar, pStage );
	}
}

void ioCircleMoveAndKeySkill::SetCreateAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	// Create AreaWeapon
	if( !m_AreaWeaponName.IsEmpty() )
	{
		D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();

		vDir.y = 0.0f;
		D3DXVec3Normalize(&vDir, &vDir);

		D3DXVECTOR3 vCreatePos = m_vCirclePos - (m_fStructOffset*vDir);
		vCreatePos.y += m_fStructHeightOffset;

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vDir );

		ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																"",
																GetName(),
																m_AreaWeaponName,
																m_vCirclePos,
																pChar->GetTargetRot(),
																ioAreaWeapon::CT_SKILL_DEPENDENCY );

		if( pArea )
		{
			m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
		}
	}

	CreateStruct( pChar, pStage );
}

bool ioCircleMoveAndKeySkill::IsProtected( int iDefenseBreakType ) const
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
	case SS_WAIT:
		return false;
	case SS_ACTION:
		return true;
	case SS_ACTION2:
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioCircleMoveAndKeySkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_ACTION2 && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioCircleMoveAndKeySkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMoveAndKeySkill::CreateStruct( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	if( m_iStructNum == 0 ) return;
	if( m_bCreateStruct ) return;

	m_bCreateStruct = true;

	D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();

	vDir.y = 0.0f;
	D3DXVec3Normalize(&vDir, &vDir);

	D3DXVECTOR3 vCreatePos = m_vCirclePos - (m_fStructOffset*vDir);
	vCreatePos.y += m_fStructHeightOffset;

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vDir );

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			int iDestroyIndex = 0;
			ioHashString szItemName;
			if( GetOwnerItem() )
				szItemName = GetOwnerItem()->GetName();

			SP2Packet kPacket( CTPK_PUSHSTRUCT_CREATE );
			kPacket << iDestroyIndex;
			kPacket << m_iStructNum;
			kPacket << vCreatePos;
			kPacket << qtTargetRot;
			kPacket << pChar->GetCharName();
			kPacket << szItemName;
			kPacket << 0;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		// 싱글모드 뿐만아니라 로비에서도 이쪽으로 오기때문에 주의가 필요함
		DWORD dwSeed = timeGetTime();
		int iCurIndex = g_PushStructListMgr.GetCurLocalIndex() + 1;
		pStage->CreatePushStruct( m_iStructNum,
								  iCurIndex,
								  vCreatePos,
								  qtTargetRot,
								  pChar->GetCharName(),
								  0,
								  dwSeed,
								  0,
								  true );
	}
}

void ioCircleMoveAndKeySkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioCircleMoveAndKeySkill::DestroyAreaWeapon()
{
	if( m_dwAreaWeaponIndex == 0 )
		return;

	g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
	m_dwAreaWeaponIndex = 0;
}

void ioCircleMoveAndKeySkill::SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner ) return;
	if( !pStage ) return;

	SetActionState2( pOwner, pStage );
}

int ioCircleMoveAndKeySkill::GetSkillState()
{
	return (int)m_State;
}


