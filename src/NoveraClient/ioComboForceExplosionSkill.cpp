
#include "stdafx.h"

#include "ioComboForceExplosionSkill.h"
#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "FindPredicateImpl.h"

ioComboForceExplosionSkill::ioComboForceExplosionSkill()
{
	ClearData();
}

ioComboForceExplosionSkill::ioComboForceExplosionSkill( const ioComboForceExplosionSkill &rhs )
: ioAttackSkill( rhs ),
  m_szLoopMotion( rhs.m_szLoopMotion ),
  m_fLoopMotionRate( rhs.m_fLoopMotionRate ),
  m_dwLoopMotionDuration( rhs.m_dwLoopMotionDuration ),
  m_FinishAttackAttribute( rhs.m_FinishAttackAttribute ),
  m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
  m_fCharHeightRate( rhs.m_fCharHeightRate ),
  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
  m_fGrapplingMinAngle( rhs.m_fGrapplingMinAngle ),
  m_fGrapplingMaxAngle( rhs.m_fGrapplingMaxAngle ),
  m_fGrapplingRange( rhs.m_fGrapplingRange ),
  m_dwKeyAddDuration( rhs.m_dwKeyAddDuration ),
  m_dwKeyEnableTime( rhs.m_dwKeyEnableTime ),
  m_szMapExpEffect( rhs.m_szMapExpEffect ),
  m_fXOffset( rhs.m_fXOffset ),
  m_fYOffset( rhs.m_fYOffset )
{
	ClearData();
}

ioComboForceExplosionSkill::~ioComboForceExplosionSkill()
{
}

void ioComboForceExplosionSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// 루프 모션
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szLoopMotion = szBuf;
	m_fLoopMotionRate		= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwLoopMotionDuration	= (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", FLOAT1 );

	// 정기폭발 모션
	m_FinishAttackAttribute.Init();
	LoadAttackAttribute_e( "finish_attack", m_FinishAttackAttribute, rkLoader );

	m_fTargetLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_loop_gap_range", FLOAT1 );
	m_fCharHeightRate		= rkLoader.LoadFloat_e( "char_height_rate", FLOAT1 );	

	m_dwSkillProtectTime	= (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_fGrapplingMinAngle	= rkLoader.LoadFloat_e( "skill_grappling_min_angle", FLOAT1 );
	m_fGrapplingMaxAngle	= rkLoader.LoadFloat_e( "skill_grappling_max_angle", FLOAT1 );
	m_fGrapplingRange		= rkLoader.LoadFloat_e( "skill_grappling_range", 0.0f );

	m_dwKeyAddDuration	= (DWORD)rkLoader.LoadInt_e( "key_add_duration", 0 );
	m_dwKeyEnableTime	= (DWORD)rkLoader.LoadInt_e( "key_enable_time", 0 );

	rkLoader.LoadString_e( "skill_map_exp_effect", "", szBuf, MAX_PATH );
	m_szMapExpEffect = szBuf;
	m_fXOffset = rkLoader.LoadFloat_e( "x_offset", 0.0f );
	m_fYOffset = rkLoader.LoadFloat_e( "y_offset", 0.0f );
}

ioSkill* ioComboForceExplosionSkill::Clone()
{
	return new ioComboForceExplosionSkill( *this );
}

void ioComboForceExplosionSkill::ClearData()
{
	m_vAttackDir = ioMath::VEC3_ZERO;
	m_vEffectPos = ioMath::VEC3_ZERO;
	
	m_SkillState = SS_READY;
	
	m_dwLoopStartTime = 0;
	m_dwBombWeaponIndex = 0;

	m_iCurCharging = 0;
	m_iPreCharging = 0;

	// 키연타 체크 초기 셋팅.
	m_dwKeyCheckStartTime = 0;
	m_bAddKey = false;

	m_fCurScaleRate = 0.0f;

	m_dwCurSkillProtectTime = 0;

	m_GrapplingTargetName.Clear();

	m_bSetHandMesh = false;
}

bool ioComboForceExplosionSkill::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();
	
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	m_dwSkillMapEffect = -1;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	return ioAttackSkill::OnSkillStart( pChar );
}


void ioComboForceExplosionSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_READY:	// 준비모션, preDelayTime 관련 처리.
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
			{
				AttackSkillFire( pChar );
				SetActionState( pChar );
				m_dwPreDelayStartTime = 0;
			}
		}
		break;

	case SS_ACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
				SetLoopState( pChar );
		}
		break;

	case SS_COMBO_LOOP:	// 공격 루프 모션.
		{
			// 따로 잡힌 타겟에 대한 유효성 체크는 하지 않는다.

			// 시간에 따라 이펙트의 크기를 조정해준다.
			DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;
			if( m_dwLoopStartTime > 0 && m_dwLoopMotionDuration > 0 )
			{
				if( m_dwLoopMotionDuration > dwGapTime )
					m_fCurScaleRate = (float)dwGapTime / m_dwLoopMotionDuration;
				else
					m_fCurScaleRate = FLOAT1;
			}

			// 루프 지속 시간이 지났다면 연타성공으로 간주.
			if( m_dwLoopStartTime + m_dwLoopMotionDuration < dwCurTime )
			{
				SetFinishState( pChar );
				return;
			}

			if( pChar->IsNeedProcess() )
			{
				DWORD dwCheck1 = m_dwKeyCheckStartTime + m_dwKeyEnableTime;
				DWORD dwCheck2 = m_dwKeyCheckStartTime + m_dwKeyAddDuration;
				int iFinishCnt = m_FinishAttackAttribute.m_vWeaponInfoList.size();

				// 키연타 체크
				if( !m_bAddKey && dwCheck1 < dwCheck2 )
				{
					if( m_dwKeyCheckStartTime > 0 && COMPARE( dwCurTime, dwCheck1, dwCheck2 ) )
					{
						if( pChar->IsAttackKey() )
						{
							m_bAddKey = true;

							m_iPreCharging = m_iCurCharging;
							m_iCurCharging++;
							if( m_iCurCharging > iFinishCnt )
								m_iCurCharging = iFinishCnt - 1;
						}
					}
				}

				if( dwCheck2 < dwCurTime )
				{
					if( m_bAddKey )
					{
						m_bAddKey = false;
						m_dwKeyCheckStartTime = dwCurTime;
					}
					else
					{
						SetFinishState( pChar );
						return;
					}
				}

				if( m_iCurCharging == iFinishCnt - 1 )
				{
					SetFinishState( pChar );
					return;
				}
			}
			
			UpdateEffect( pChar, pStage );
		}
		break;
	case SS_FINISH:
		{
			FinishAttackSkillFire( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioComboForceExplosionSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	ClearData();

	if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect, false );
		m_dwSkillMapEffect = -1;
	}

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

}

bool ioComboForceExplosionSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_GrapplingTargetName != szTargetName ) return false;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_GRAPPLING_TARGET;
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioComboForceExplosionSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetLoopGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioComboForceExplosionSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioComboForceExplosionSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
}

void ioComboForceExplosionSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SS_GRAPPLING_TARGET:
		{
			rkPacket >> m_GrapplingTargetName;

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );

			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SS_FINISH:
		rkPacket >> m_iCurCharging;
		SetFinishState( pChar );
		break;
	}
}

bool ioComboForceExplosionSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_READY:
	case SS_ACTION:
	case SS_GRAPPLING_TARGET:
	case SS_COMBO_LOOP:	
	case SS_FINISH:
		return false;
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		break;
	}

	return false;
}

bool ioComboForceExplosionSkill::IsProtected( int iDefenseBreakType ) const
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

void ioComboForceExplosionSkill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage ) return;

	m_GrapplingTargetName.Clear();
	FD_GrapplingAutoTarget kAutoPred( pChar, m_fGrapplingMinAngle, m_fGrapplingMaxAngle, m_fGrapplingRange );
	
	ioBaseChar *pTarget = pStage->FindUser( kAutoPred );
	if( pTarget )
		m_GrapplingTargetName = pTarget->GetCharName();

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											GetAttribute().m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pChar->AttachEffect( m_SkillEffectName );

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwFireStartTime = FRAMEGETTIME();

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

void ioComboForceExplosionSkill::SetActionState( ioBaseChar *pChar )
{
	if( !pChar )	return;

	m_SkillState = SS_ACTION;

	// 이펙트 / 충돌체의 발사 위치 설정.
	D3DXVECTOR3	vScale = pChar->GetWorldScale();
	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
	m_vEffectPos = pChar->GetWorldPosition();
	m_vEffectPos.y += m_fYOffset * vScale.y;
	m_vEffectPos += m_fXOffset * m_vAttackDir * vScale.x;

}

void ioComboForceExplosionSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	m_SkillState = SS_COMBO_LOOP;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;
	if( m_dwLoopStartTime > 0 && m_dwLoopMotionDuration > 0 )
	{
		if( m_dwLoopMotionDuration > dwGapTime )
			m_fCurScaleRate = (float)dwGapTime / m_dwLoopMotionDuration;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopMotion );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fLoopMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	// 연타 체크용.
	m_dwKeyCheckStartTime = dwCurTime;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		// 이펙트 처리
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

		ioMapEffect *pMapEffect = NULL;
		pMapEffect = pStage->CreateMapEffect( m_SkillMapEffectName, m_vEffectPos, vScale );
		if( pMapEffect )
		{
			m_dwSkillMapEffect = pMapEffect->GetUniqueID();

			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, m_vAttackDir );
			pMapEffect->SetWorldOrientation( qtCharRot );
		}
	}
}


void ioComboForceExplosionSkill::SetFinishState( ioBaseChar *pChar )
{
	if( m_SkillState != SS_COMBO_LOOP )	return;
	if( !pChar )	return;
	
	m_SkillState = SS_FINISH;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_FINISH;
		kPacket << m_iCurCharging;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioComboForceExplosionSkill::UpdateEffect( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;
	float fCurScaleRate = FLOAT1;
	if( m_fCurScaleRate > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurScaleRate;

		ioMapEffect *pMapEff = pStage->FindMapEffect( m_dwSkillMapEffect );
		if( pMapEff )
		{
			ioEffect *pEffect = pMapEff->GetEffect();
			if( pEffect )
			{
				D3DXVECTOR3 vScale( fCurScaleRate, fCurScaleRate, fCurScaleRate );
				pEffect->GetParentNode()->SetScale( vScale );
			}
		}
	}
}

void ioComboForceExplosionSkill::FinishAttackSkillFire( ioBaseChar* pChar )
{
	if( !pChar )	return;

	m_SkillState = SS_END;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwPreDelayStartTime = 0;

	m_dwFireStartTime = dwCurTime;
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_FinishAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_FinishAttackAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwBombWeaponIndex = pChar->IncreaseWeaponIndexBase();

	// 정기 폭발 충돌체 생성.
	pChar->SkillFireExplicit( m_FinishAttackAttribute.m_vWeaponInfoList[m_iCurCharging],
								GetName(),
								m_dwBombWeaponIndex,
								m_vEffectPos );

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		if( m_dwSkillMapEffect != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );

			if( pMapEffect )	pMapEffect->EndEffectForce();

			m_dwSkillMapEffect = -1;
		}

		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

		ioMapEffect *pMapEffect = NULL;
		pMapEffect = pStage->CreateMapEffect( m_szMapExpEffect, m_vEffectPos, vScale );
		if( pMapEffect )
		{
			m_dwSkillMapEffect = pMapEffect->GetUniqueID();

			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, m_vAttackDir );
			pMapEffect->SetWorldOrientation( qtCharRot );
		}
	}
}