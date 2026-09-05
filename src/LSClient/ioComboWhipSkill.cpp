
#include "stdafx.h"

#include "ioComboWhipSkill.h"
#include "ioBaseChar.h"

ioComboWhipSkill::ioComboWhipSkill()
{
	m_bSetHandMesh = false;
	
	m_dwCurWhipEffect = -1;
	m_dwFinishWhipEffect = -1;

	m_dwWhipWeaponIndex = 0;

	m_fCurWhipRate = 0.0f;
	m_dwCurSkillProtectTime = 0;

	m_dwEffectLoopCheckTime = 0;
	m_dwEffectLoopDuration = 0;
}

ioComboWhipSkill::ioComboWhipSkill( const ioComboWhipSkill &rhs )
: ioAttackSkill( rhs ),
 m_LoopAniName( rhs.m_LoopAniName ),
 m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_dwAddDuration( rhs.m_dwAddDuration ),
 m_dwEnableTime( rhs.m_dwEnableTime ),
 m_fLoopTimeRate( rhs.m_fLoopTimeRate ),
 m_WhipInfoList( rhs.m_WhipInfoList ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_FinishMotion( rhs.m_FinishMotion ),
 m_fFinishMotionRate( rhs.m_fFinishMotionRate ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	m_bSetHandMesh = false;
	
	m_dwCurWhipEffect = -1;
	m_dwFinishWhipEffect = -1;

	m_dwWhipWeaponIndex = 0;

	m_fCurWhipRate = 0.0f;
	m_dwCurSkillProtectTime = 0;

	m_dwEffectLoopCheckTime = 0;
	m_dwEffectLoopDuration = 0;
}

ioComboWhipSkill::~ioComboWhipSkill()
{
}

void ioComboWhipSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_dwAddDuration = (DWORD)rkLoader.LoadInt_e( "key_add_duration", 0 );
	m_dwEnableTime = (DWORD)rkLoader.LoadInt_e( "key_enable_time", 0 );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH);
	m_LoopAniName = szBuf;

	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );
	m_fLoopTimeRate = rkLoader.LoadFloat_e( "loop_time_rate", FLOAT1);

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;

	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "finish_motion", "", szBuf, MAX_PATH );
	m_FinishMotion = szBuf;

	m_fFinishMotionRate = rkLoader.LoadFloat_e( "finish_motion_rate", FLOAT1 );


	// whipinfo
	m_WhipInfoList.clear();

	int iSize = rkLoader.LoadInt_e( "whip_info_cnt", 0 );
	for( int i=0; i < iSize; ++i )
	{
		WhipInfo kInfo;

		wsprintf_e( szKey, "whip_rate%d", i+1 );
		kInfo.m_fWhipRate = rkLoader.LoadFloat( szKey, 0.0f );

		// normal
		wsprintf_e( szKey, "whip_effect%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WhipEffect = szBuf;

		wsprintf_e( szKey, "whip_attack%d_type", i+1 );
		kInfo.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "whip_attack%d_type_resist", i+1 );
		kInfo.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "whip_attack%d_type_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "whip_attack%d_type_wounded_time", i+1 );
		kInfo.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "whip_attack%d_type_wounded_loop_ani", i+1 );
		kInfo.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		// finish
		wsprintf_e( szKey, "finish_whip_effect%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FinishWhipEffect = szBuf;

		wsprintf_e( szKey, "finish_whip_attack%d_type", i+1 );
		kInfo.m_FinishWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "finish_whip_attack%d_type_resist", i+1 );
		kInfo.m_FinishWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "finish_whip_attack%d_type_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FinishWeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "finish_whip_attack%d_type_wounded_time", i+1 );
		kInfo.m_FinishWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "finish_whip_attack%d_type_wounded_loop_ani", i+1 );
		kInfo.m_FinishWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_WhipInfoList.push_back( kInfo );
	}
}

ioSkill* ioComboWhipSkill::Clone()
{
	return new ioComboWhipSkill( *this );
}

bool ioComboWhipSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_SkillState = SS_READY;
	m_dwLoopStartTime = 0;

	// 키연타 체크 초기 셋팅.
	m_dwCheckStartTime = 0;
	m_bAddKey = false;

	m_bSetHandMesh = false;
	m_dwWhipWeaponIndex = 0;

	m_fCurWhipRate = 0.0f;

	m_dwEffectLoopCheckTime = 0;
	m_dwEffectLoopDuration = 0;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	if( m_dwCurWhipEffect != -1 )
	{
		pChar->EndEffect( m_dwCurWhipEffect, false );
		m_dwCurWhipEffect = -1;
	}

	if( m_dwFinishWhipEffect != -1 )
	{
		pChar->EndEffect( m_dwFinishWhipEffect, false );
		m_dwFinishWhipEffect = -1;
	}

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	return ioAttackSkill::OnSkillStart( pChar );
}


void ioComboWhipSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_READY:	// 준비모션, preDelayTime 관련 처리.
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
			{
				SetActionState( pChar );
				m_dwPreDelayStartTime = 0;
			}
		}
		break;

	case SS_ACTION:	// 채찍이 늘어났다 줄어드는 모션.
		{
			// 애니메이션 종료시 SetEndState() 호출
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar, false );
				return;
			}

			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetLoopState( pChar );
				return;
			}
		}
		break;

	case SS_LOOP:	// 공격 루프 모션.
		{
			// 루프 지속 시간이 지났다면 연타성공으로 간주.
			if( m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetFinishState( pChar );
				return;
			}

			if( pChar->IsNeedProcess() )
			{
				DWORD dwCheck1 = m_dwCheckStartTime + m_dwEnableTime;
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
					}
					else
					{
						SetEndState( pChar, true );
						return;
					}
				}
			}

			// 이펙트 loop 체크
			CheckEffectLoop( pChar );
		}
		break;
	case SS_FINISH:	// 피니쉬 어택.
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			CheckFinishWhipInfo( pChar );
			m_dwFireStartTime = 0;
		}
		break;
	case SS_END:	// 종료.
		break;
	}
}

void ioComboWhipSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	m_bSetHandMesh = false;
	m_fCurWhipRate = 0.0f;

	m_dwEffectLoopCheckTime = 0;
	m_dwEffectLoopDuration = 0;

	if( m_dwCurWhipEffect != -1 )
	{
		pChar->EndEffect( m_dwCurWhipEffect, false );
		m_dwCurWhipEffect = -1;
	}

	if( m_dwFinishWhipEffect != -1 )
	{
		pChar->EndEffect( m_dwFinishWhipEffect, false );
		m_dwFinishWhipEffect = -1;
	}

	if( m_dwWhipWeaponIndex > 0 )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWhipWeaponIndex );
		if( pWeapon )
		{
			m_dwWhipWeaponIndex = 0;
			pWeapon->SetWeaponDead();
		}
	}
}

void ioComboWhipSkill::SetActionState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetSkillMotionName() );
	if( iAniID == -1 )	return;

	m_SkillState = SS_ACTION;

	DWORD dwCurTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate = GetSkillMotionRate();
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, 0 );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioComboWhipSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState == SS_END )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_LoopAniName );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fLoopTimeRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwEffectLoopCheckTime = dwCurTime;
	m_dwEffectLoopDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwLoopStartTime = dwCurTime;

	// 연타 체크용.
	m_dwCheckStartTime = dwCurTime;

	// CheckRange Rate
	DWORD dwGapTime = 0;
	DWORD dwDuration = 0;
	if( m_dwMotionStartTime < m_dwMotionEndTime && m_dwMotionStartTime < dwCurTime )
	{
		dwGapTime = dwCurTime - m_dwMotionStartTime;
		dwDuration = m_dwMotionEndTime - m_dwMotionStartTime;
	}

	m_fCurWhipRate = 0.0f;
	DWORD dwHalf = dwDuration * FLOAT05;
	if( dwGapTime > 0 && dwHalf > 0 )
	{
		if( dwGapTime > dwHalf )
		{
			DWORD dwCurGap = dwGapTime - dwHalf;
			float fRate = (float)dwCurGap / dwHalf;

			m_fCurWhipRate = FLOAT1 - fRate;
		}
		else if( dwGapTime <= dwHalf )
		{
			m_fCurWhipRate = (float)dwGapTime / dwHalf;
		}
	}

	//
	m_dwWhipWeaponIndex = pChar->IncreaseWeaponIndexBase();
	CheckCurWhipInfo( pChar );

	m_SkillState = SS_LOOP;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)m_SkillState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioComboWhipSkill::CheckCurWhipInfo( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( m_WhipInfoList.empty() )
		return;

	int iSize = m_WhipInfoList.size();
	float fCheckRate = 0.0f;
	for( int i=0; i < iSize; ++i )
	{
		float fWhipRate = m_WhipInfoList[i].m_fWhipRate;

		if( COMPARE( m_fCurWhipRate, fCheckRate, fWhipRate ) )
		{
			m_CurWhipEffect.Clear();

			if( m_dwCurWhipEffect != -1 )
			{
				pChar->EndEffect( m_dwCurWhipEffect, false );
				m_dwCurWhipEffect = -1;
			}

			ioEffect *pEffect = pChar->AttachEffect( m_WhipInfoList[i].m_WhipEffect );
			if( pEffect )
			{
				m_CurWhipEffect = m_WhipInfoList[i].m_WhipEffect;
				m_dwCurWhipEffect = pEffect->GetUniqueID();
			}

			// 충돌체 처리.
			D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
			pChar->SkillFireExplicit( m_WhipInfoList[i].m_WeaponInfo,
									  GetName(),
									  m_dwWhipWeaponIndex,
									  vCurPos );

			return;
		}

		fCheckRate = fWhipRate;
	}
}

void ioComboWhipSkill::SetFinishState( ioBaseChar *pChar )
{
	if( m_dwWhipWeaponIndex > 0 )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWhipWeaponIndex );
		if( pWeapon )
		{
			m_dwWhipWeaponIndex = 0;
			pWeapon->SetWeaponDead();
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	
	m_SkillState = SS_FINISH;
	m_dwPreDelayStartTime = 0;

	m_dwFireStartTime = dwCurTime;
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_FinishMotion );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_fFinishMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );
	
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwEnableReserveTime = 0;
	float fReserveValue = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveValue > 0.0f )
	{
		m_dwEnableReserveTime = dwCurTime + fReserveValue;
	}
}

void ioComboWhipSkill::CheckFinishWhipInfo( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( m_WhipInfoList.empty() )
		return;

	int iSize = m_WhipInfoList.size();
	float fCheckRate = 0.0f;
	for( int i=0; i < iSize; ++i )
	{
		float fWhipRate = m_WhipInfoList[i].m_fWhipRate;

		if( COMPARE( m_fCurWhipRate, fCheckRate, fWhipRate ) )
		{
			if( m_dwFinishWhipEffect != -1 )
			{
				pChar->EndEffect( m_dwFinishWhipEffect, false );
				m_dwFinishWhipEffect = -1;
			}

			ioEffect *pEffect = pChar->AttachEffect( m_WhipInfoList[i].m_FinishWhipEffect );
			if( pEffect )
			{
				m_dwFinishWhipEffect = pEffect->GetUniqueID();
			}

			// 충돌체 처리.
			D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
			pChar->SkillFireExplicit( m_WhipInfoList[i].m_FinishWeaponInfo,
									  GetName(),
									  m_dwWeaponBaseIndex,
									  vCurPos );

			return;
		}

		fCheckRate = fWhipRate;
	}
}

void ioComboWhipSkill::SetEndState( ioBaseChar *pChar, bool bSend )
{
	if( m_SkillState != SS_ACTION && m_SkillState != SS_LOOP )
		return;

	if( m_dwWhipWeaponIndex > 0 )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWhipWeaponIndex );
		if( pWeapon )
		{
			m_dwWhipWeaponIndex = 0;
			pWeapon->SetWeaponDead();
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_END;
	m_dwPreDelayStartTime = 0;

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;

	if( !pChar  ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fEndMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwEnableReserveTime = 0;
	float fReserveValue = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveValue > 0.0f )
	{
		m_dwEnableReserveTime = dwCurTime + fReserveValue;
	}

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)m_SkillState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioComboWhipSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioComboWhipSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );
}

void ioComboWhipSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SS_LOOP:
		SetLoopState( pChar );
		break;
	case SS_END:
		SetEndState( pChar, false );
		break;
	}
}

bool ioComboWhipSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_END:
	case SS_FINISH:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		break;
	}

	return false;
}

bool ioComboWhipSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;

	return false;
}

void ioComboWhipSkill::CheckEffectLoop( ioBaseChar *pChar )
{
	if( m_dwCurWhipEffect == -1 )
		return;

	if( m_CurWhipEffect.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwEffectLoopCheckTime > 0 &&
		m_dwEffectLoopCheckTime + m_dwEffectLoopDuration < dwCurTime )
	{
		m_dwEffectLoopCheckTime = dwCurTime;

		pChar->EndEffect( m_dwCurWhipEffect, false );
		m_dwCurWhipEffect = -1;

		ioEffect *pEffect = pChar->AttachEffect( m_CurWhipEffect );
		if( pEffect )
		{
			m_dwCurWhipEffect = pEffect->GetUniqueID();
		}
	}
}


