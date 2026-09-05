

#include "stdafx.h"

#include "ioAirComboAttackSkill.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioAirComboAttackSkill::ioAirComboAttackSkill()
{
	ClearData();	
}

ioAirComboAttackSkill::ioAirComboAttackSkill( const ioAirComboAttackSkill &rhs )
: ioAttackSkill( rhs ),
  m_szLoopAnimation( rhs.m_szLoopAnimation ),
  m_fLoopAniRate( rhs.m_fLoopAniRate ),
  m_dwLoopDuration( rhs.m_dwLoopDuration ),
  m_dwLoopProtectTime( rhs.m_dwLoopProtectTime ),
  m_SecondAttackAttribute( rhs.m_SecondAttackAttribute ),
  m_szLoopEffect( rhs.m_szLoopEffect )
{
	ClearData();
}

ioAirComboAttackSkill::~ioAirComboAttackSkill()
{
}

void ioAirComboAttackSkill::ClearData()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_PREDELAY;
	m_vCurMoveDir = ioMath::VEC3_ZERO;
	
	m_dwLoopStartTime = 0;
}

void ioAirComboAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	//Loop Animation
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );	m_szLoopAnimation = szBuf;
	m_fLoopAniRate		= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwLoopDuration	= (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwLoopProtectTime	= (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	//Second Attack
	LoadAttackAttribute_e( "second_attack", m_SecondAttackAttribute, rkLoader );

	rkLoader.LoadString_e( "skill_loop_effect", "", szBuf, MAX_PATH );
	m_szLoopEffect = szBuf;
}

ioSkill* ioAirComboAttackSkill::Clone()
{
	return new ioAirComboAttackSkill( *this );
}

bool ioAirComboAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_vCurMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	return ioAttackSkill::OnSkillStart( pChar );
}

void ioAirComboAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	switch( m_SkillState )
	{
	case SS_PREDELAY:
		{
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ATTACK:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetSkillLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetSecondAtack( pChar );

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SSC_KEY;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioAirComboAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}


	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	
	if( !m_szLoopEffect.IsEmpty() )
		pChar->EndEffect( m_szLoopEffect );

	ClearData();
}

bool ioAirComboAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;
	
	switch( m_SkillState )
	{
	case SS_PREDELAY:
	case SS_ATTACK:
		return false;
	case SS_LOOP:
	case SS_END:	
		if( m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
			return true;
		return false;
	}

	return true;
}

bool ioAirComboAttackSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwLoopStartTime + m_dwLoopProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioAirComboAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioAirComboAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )	return;
	if( !pStage )	return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_KEY:
		{
			SetSecondAtack( pChar );
		}
		break;
	}
}

void ioAirComboAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
	
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		pZone->SetCollisionDir( m_vCurMoveDir );
		pZone->SetZoneValue( false, true );
	}

	ioZoneEffect2Weapon *pZone2 = ToZoneEffect2Weapon( pWeapon );
	if( pZone2 )
	{
		pZone2->SetCollisionDir( m_vCurMoveDir );
		pZone2->SetZoneValue( false, true );
	}
}


void ioAirComboAttackSkill::AttackSkillFire( ioBaseChar *pChar )
{
	m_SkillState = SS_ATTACK;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

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

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( GetAttribute().m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = GetAttribute().m_fAttackEndAniRate;

		pGrp->SetActionAni( GetAttribute().m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndTimeRate,
			fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}


	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioAirComboAttackSkill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_LOOP )	return;
	if( !pChar )	return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopAnimation );
	if( iAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	m_dwLoopStartTime = dwCurTime;

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += m_dwLoopDuration;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szLoopAnimation, FLOAT100, FLOAT1, m_fLoopAniRate );

	if( !m_szLoopEffect.IsEmpty() )
		pChar->AttachEffect( m_szLoopEffect );
}

void ioAirComboAttackSkill::SetSecondAtack( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_SkillState == SS_END )	return;

	m_SkillState = SS_END;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SecondAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate  = m_SecondAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_SecondAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_SecondAttackAttribute.m_dwEndDelay;

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, false );
	pGrp->ClearAllActionAni( FLOAT100, false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_SecondAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	pChar->SetReservedSliding( m_SecondAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
		
	if( !m_szLoopEffect.IsEmpty() )
		pChar->EndEffect( m_szLoopEffect );
}
