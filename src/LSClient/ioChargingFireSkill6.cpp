

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioChargingFireSkill6.h"

#include "WeaponDefine.h"

ioChargingFireSkill6::ioChargingFireSkill6()
{
	m_State = SS_NONE;

	m_dwChargingEffect = -1;

	m_bSetHandMesh = false;

	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwFireStartTime = 0;

	m_fCurChargeRate = 0.0f;
}

ioChargingFireSkill6::ioChargingFireSkill6( const ioChargingFireSkill6 &rhs )
: ioNormalSkill( rhs ),
 m_EnemyBuffList( rhs.m_EnemyBuffList ),
 m_Attribute_a( rhs.m_Attribute_a ),
 m_Attribute_s( rhs.m_Attribute_s ),
 m_Attribute_d( rhs.m_Attribute_d ),
 m_TargetColType( rhs.m_TargetColType ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_fSkillRange( rhs.m_fSkillRange ),
 m_fSkillAngle( rhs.m_fSkillAngle ),
 m_fOffSet( rhs.m_fOffSet ),
 m_fUpHeight( rhs.m_fUpHeight ),
 m_fUnderHeight( rhs.m_fUnderHeight ),
 m_fWidth( rhs.m_fWidth ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_szFailAni( rhs.m_szFailAni ),
 m_fFailAniRate( rhs.m_fFailAniRate ),
 m_dwMaxChargingTime( rhs.m_dwMaxChargingTime ),
 m_szChargingEffect( rhs.m_szChargingEffect ),
 m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	m_State = SS_NONE;

	m_dwChargingEffect = -1;

	m_bSetHandMesh = false;

	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwFireStartTime = 0;

	m_fCurChargeRate = 0.0f;
}

ioChargingFireSkill6::~ioChargingFireSkill6()
{
}

void ioChargingFireSkill6::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szTitle[MAX_PATH] = "", szBuf[MAX_PATH] = "";

	m_dwMaxChargingTime = rkLoader.LoadInt_e( "charging_max_time", 0 );

	rkLoader.LoadString_e( "charging_effect", "", szBuf, MAX_PATH );
	m_szChargingEffect = szBuf;

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	rkLoader.LoadString_e( "charging_fail_ani", "", szBuf, MAX_PATH );
	m_szFailAni = szBuf;
	m_fFailAniRate = rkLoader.LoadFloat_e( "charging_fail_ani_rate", FLOAT1 );

	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	// Action Attack
	m_Attribute_a.Init();
	LoadAttackAttribute_e( "attack_key_a", m_Attribute_a, rkLoader );
	m_Attribute_s.Init();
	LoadAttackAttribute_e( "attack_key_s", m_Attribute_s, rkLoader );
	m_Attribute_d.Init();
	LoadAttackAttribute_e( "attack_key_d", m_Attribute_d, rkLoader );

	// enemy buff
	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_collision_type", TCT_SPHERE );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fSkillRange = rkLoader.LoadFloat_e( "apply_range", 0.0f );
	m_fSkillAngle = rkLoader.LoadFloat_e( "apply_angle", 0.0f );
	m_fOffSet = rkLoader.LoadFloat_e( "apply_position_offset", 0.0f );

	m_fUpHeight = rkLoader.LoadFloat_e( "apply_up_height", 0.0f );
	m_fUpHeight = max( FLOAT1, m_fUpHeight );
	m_fUnderHeight = rkLoader.LoadFloat_e( "apply_under_height", 0.0f );
	m_fUnderHeight = max( FLOAT1, m_fUnderHeight );
	m_fWidth = rkLoader.LoadFloat_e( "apply_width", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "enemy_buff_cnt", 0 );
	m_EnemyBuffList.clear();

	if( iCnt <= 0 ) return;

	m_EnemyBuffList.reserve( iCnt );

	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "enemy_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_EnemyBuffList.push_back( tBuffInfo );
			}
		}
	}

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioChargingFireSkill6::Clone()
{
	return new ioChargingFireSkill6( *this );
}

bool ioChargingFireSkill6::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;

	m_dwChargingEffect = -1;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_fCurChargeRate = 0.0f;

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

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	SetCameraBuff( pChar );
	CheckSkillDesc( pChar );

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetNormalSkill( pChar );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioChargingFireSkill6::OnSkillEnd( ioBaseChar *pChar )
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

	//DestroyMapEffect
	if( m_dwChargingEffect != -1 )
	{
		pChar->EndMapEffect( m_dwChargingEffect, false );
	}

	m_dwChargingEffect = -1;

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_fCurChargeRate = 0.0f;
	m_dwFireStartTime = 0;

	m_State = SS_NONE;
}

void ioChargingFireSkill6::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	int iSlot = GetOwnerItem()->GetType() - 1;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetNormalSkill( pChar );
				SetEnemyBuffList( pChar, pStage );
			}
		}
		break;
	case SS_PRE_ACTION:
		if( m_dwPreActionEndTime > 0 && m_dwPreActionEndTime < dwCurTime )
		{
			SetLoopState( pChar );
			return;
		}
		break;
	case SS_LOOP:
		{
			ProcessLoopState( pChar, pStage );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() )
				{
					SetActionState( pChar, pStage, IT_ATTACK );
					return;
				}
				else if( pChar->IsDefenseKey() )
				{
					SetActionState( pChar, pStage, IT_DEFENSE );
					return;
				}
				else if( pChar->IsJumpKey() )
				{
					SetActionState( pChar, pStage, IT_JUMP );
					return;
				}
			}

			if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
				SetFailState( pChar );
		}
		break;
	case SS_ACTION:
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			if( m_dwChargingEffect != -1 )
			{
				pChar->EndMapEffect( m_dwChargingEffect, false );
				m_dwChargingEffect = -1;
			}

			m_dwFireStartTime = 0;
		}

		if( m_dwChargingEffect != -1 )
		{
			pChar->UpdateAttachMapEffect( m_dwChargingEffect );
		}
		break;
	}
}

void ioChargingFireSkill6::SetNormalSkill( ioBaseChar *pChar )
{
	m_State = SS_PRE_ACTION;
	EndPreDelayEffect( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_dwPreActionEndTime = dwCurTime;
	m_dwPreActionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
}

void ioChargingFireSkill6::SetEnemyBuffList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner || !pStage )
		return;

	if( m_EnemyBuffList.empty() )
		return;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

	// 충돌타입에 따라 리스트 가져오기
	switch( m_TargetColType )
	{
	case TCT_SPHERE:
		pOwner->GetTargetCharListBySphere( vPos, m_fSkillRange, m_fSkillAngle, vTargetList );
		break;
	case TCT_CYLINDER:
		pOwner->GetTargetCharListByCylinder( vPos, m_fSkillRange, m_fSkillAngle, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	case TCT_BOX:
		// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos += vDir * m_fSkillRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
		vPos -= vOffset;
		pOwner->GetTargetCharListByBox( vPos, m_fSkillRange, m_fWidth, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	}

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		vTargetList.clear();

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )								continue;
		if( !pTarget->IsNeedProcess() )				continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;
		if( pTarget->GetTeam() == pOwner->GetTeam()	)
			continue;

		if( !pStage->IsCollisionCheckByMode() )		continue;
		if( !pTarget->CheckGrapplingExceptionState() ) continue;
		if( pTarget->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( pTarget->GetState() == CS_SKIP_STUN ) continue;
		if( Help::CheckTargetNoCollision( pTarget->GetState() ) ) continue;
		if( !pTarget->HasCollisionAvailableBuff() ) continue;
		if( pTarget->IsProtectState() )	continue;
		if( pTarget->IsSkipState() )	continue;
		if( pTarget->HasHideBuff() )	continue;
		if( pTarget->IsCanNoWound() )	continue;
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) ) continue;

		pTarget->SetRandomSeed(0);

		int iCnt = m_EnemyBuffList.size();
		for( int i=0; i<iCnt; ++i )
		{
			const ioHashString& szBuffName = m_EnemyBuffList[i].m_BuffName;
			if( !szBuffName.IsEmpty() )
			{
				pTarget->AddNewBuff( szBuffName, pOwner->GetCharName(), szItemName, this, false );
				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}

		if( bResult )
		{
			int iCurBuffCnt = vBuffList.size();
			if( pTarget->IsNeedSendNetwork() )
			{
				if( Help::CheckEnableTimeGapUser() )
				{
					LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff8 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
				}
				SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
				kPacket << pTarget->GetCharName();
				kPacket << pOwner->GetCharName();
				kPacket << szItemName;
				kPacket << pTarget->GetRandomSeed();
				kPacket << true;					// 타격횟수 체크
				kPacket << 0.0f;
				kPacket << 0.0f;
				kPacket << 0.0f;
				kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
				kPacket << iCurBuffCnt;

				for( i=0; i < iCurBuffCnt; i++ )
				{
					kPacket << vBuffList[i];
				}

				kPacket << false;					// Use Force

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	return;
}

void ioChargingFireSkill6::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;
	m_dwLoopStartTime = dwCurTime;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_dwChargingEffect != -1 )
	{
		pChar->EndEffect( m_dwChargingEffect, false );
	}

	ioMapEffect *pChargingEff = pChar->CreateMapEffectAttachChar( m_szChargingEffect );
	if( pChargingEff )
	{
		m_dwChargingEffect = pChargingEff->GetUniqueID();
	}

	m_dwMotionEndTime = m_dwLoopStartTime + m_dwLoopDuration;
}

void ioChargingFireSkill6::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, InputType eType )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	if( m_State == SS_ACTION )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_ACTION;

	// Charging Rate
	DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;
	if( m_dwLoopStartTime > 0 && m_dwMaxChargingTime > 0 )
	{
		if( m_dwMaxChargingTime > dwGapTime )
			m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargingTime;
		else
			m_fCurChargeRate = FLOAT1;
	}

	AttackAttribute stAttribute;
	stAttribute.Init();
	if( eType == IT_ATTACK )
		stAttribute = m_Attribute_d;
	else if( eType == IT_DEFENSE )
		stAttribute = m_Attribute_s;
	else if( eType == IT_JUMP )
		stAttribute = m_Attribute_a;

	if( stAttribute.m_vWeaponInfoList.empty() )
		return;
	
	int iAniID = pGrp->GetAnimationIdx( stAttribute.m_AttackAnimation );
	float fAniRate = stAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = stAttribute.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( iAniID != -1 )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	// Attack
	if( !stAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											stAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fAniRate,
											dwPreDelay );
	}

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
	if( fReserveTime > 0.0f )
	{
		m_dwEnableReserveTime = dwCurTime + fReserveTime +dwPreDelay;
	}

	float fFireStart = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	if( fFireStart > 0.0f )
	{
		m_dwFireStartTime = dwCurTime + fFireStart + dwPreDelay;
	}

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fAniRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd + dwPreDelay;
	else
		m_dwAttackEndTime = 0;

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)eType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargingFireSkill6::SetFailState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szFailAni );
	if( iAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = SS_FAIL;
	m_dwAttackEndTime = 0;

	float fTimeRate  = m_fFailAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioChargingFireSkill6::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;

	int iType;
	rkPacket >> iType;
	SetActionState( pChar, pStage, (InputType)iType );
}

bool ioChargingFireSkill6::IsProtected( int iDefenseBreakType ) const
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
	case SS_PRE_ACTION:
	case SS_LOOP:
		{
			if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
				return true;
			else
				return false;
		}
		break;
	case SS_ACTION:
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioChargingFireSkill6::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioChargingFireSkill6::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioChargingFireSkill6::ProcessLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( m_dwChargingEffect == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pChar->UpdateAttachMapEffect( m_dwChargingEffect );

	// Charging Rate
	DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;
	if( m_dwLoopStartTime > 0 && m_dwMaxChargingTime > 0 )
	{
		if( m_dwMaxChargingTime > dwGapTime )
			m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargingTime;
		else
			m_fCurChargeRate = FLOAT1;
	}

	float fCurScaleRate = FLOAT1;
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurChargeRate;

		ioMapEffect *pMapEff = pStage->FindMapEffect( m_dwChargingEffect );
		if( pMapEff && fCurScaleRate > FLOAT1 )
		{
			ioEffect *pEff = pMapEff->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vScale( fCurScaleRate, fCurScaleRate, fCurScaleRate );
				pEff->GetParentNode()->SetScale( vScale );
			}
		}
	}
}

void ioChargingFireSkill6::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = pWeapon->GetOwner();

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}
