

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioGrowthUpInfo.h"

#include "ioMoveAttackSkill3.h"

#include "WeaponDefine.h"

ioMoveAttackSkill3::ioMoveAttackSkill3()
{
	m_bSetHandMesh = false;
}

ioMoveAttackSkill3::ioMoveAttackSkill3( const ioMoveAttackSkill3 &rhs )
: ioAttackSkill( rhs ),
 m_bDirection( rhs.m_bDirection ),
 m_SkillLoopAnimation( rhs.m_SkillLoopAnimation ),
 m_fSkillLoopAniRate( rhs.m_fSkillLoopAniRate ),
 m_EndAttack( rhs.m_EndAttack ),
 m_LoopAttack( rhs.m_LoopAttack ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_bDisableGrowth( rhs.m_bDisableGrowth ),
 m_fGuagePerTic( rhs.m_fGuagePerTic ),
 m_dwTicTime( rhs.m_dwTicTime ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;
}

ioMoveAttackSkill3::~ioMoveAttackSkill3()
{
}

void ioMoveAttackSkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_bDirection = rkLoader.LoadBool_e( "direction", false );

	LoadAttackAttribute_e( "end_attack", m_EndAttack, rkLoader );

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_SkillLoopAnimation = szBuf;
	m_fSkillLoopAniRate = rkLoader.LoadFloat_e( "loop_ani_rate", FLOAT1 );

	m_LoopAttack.m_iWeaponIdx = rkLoader.LoadInt_e( "loop_attack_type", NO_WEAPON_ATTRIBUTE );
	m_LoopAttack.m_iResistanceIdx = rkLoader.LoadInt_e( "loop_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "loop_attack_type_wounded", "", szBuf, MAX_PATH );
	m_LoopAttack.m_WoundedAnimation = szBuf;
	m_LoopAttack.m_dwWoundedDuration = rkLoader.LoadInt_e( "loop_attack_type_wounded_time", 0 );
	m_LoopAttack.m_bWoundedLoopAni = rkLoader.LoadBool_e( "loop_attack_type_loop_ani", false );

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic   = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime      = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioMoveAttackSkill3::Clone()
{
	return new ioMoveAttackSkill3( *this );
}

bool ioMoveAttackSkill3::OnSkillStart( ioBaseChar *pChar )
{
	// 기본 무기 손에서 제거
	m_bSetHandMesh = false;
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_NONE;
	m_dwSkillMapEffect = -1;
	m_dwLoopWeaponIdx = 0;

	m_dwCheckTime			= FRAMEGETTIME();
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		ioPlayStage *pStage = pChar->GetCreator();
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );


			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);			
		}
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return ioAttackSkill::OnSkillStart( pChar );
}

void ioMoveAttackSkill3::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_EndAttack.m_AttackEndAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_EndAttack.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->AttachEffect( m_SkillEffectName );
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_EndAttack.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										0 );


	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, m_EndAttack.m_dwPreDelay );

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	pChar->SetReservedSliding( m_EndAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );
}

void ioMoveAttackSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_NONE:
		{
			if( m_dwPreDelayStartTime + GetPreDelayTime() < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;

				if( m_SkillUseType == SUT_JUMP )
					SetSkillLoopState( pChar );
				else
					SetSkillPreActionState( pChar );

				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_PRE_ACTION:
		ProcessSkillPreActionState( pChar );
		break;
	case SS_LOOP:
		ProcessSkillLoopState( pChar );
		break;
	case SS_ACTION:
		ProcessSkillActionState( pChar );
		break;
	}

	if( pStage && pChar && m_dwSkillMapEffect != -1 )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		vPos.y = pChar->GetBottomHeight();;
		ioMapEffect *pEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
		if( pEffect )
			pEffect->SetWorldPosition( vPos );
	}
}

void ioMoveAttackSkill3::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
	pChar->SetSkillMoveEnable( false, false );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage && m_dwSkillMapEffect != -1 )
	{
		pStage->DestroyMapEffect( m_dwSkillMapEffect );
		m_dwSkillMapEffect = -1;
	}

	ioAttackSkill::OnSkillEnd( pChar );
}

void ioMoveAttackSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon )
		return;

	if( m_SkillState == SS_LOOP )
	{
		m_dwLoopWeaponIdx = pWeapon->GetWeaponIndex();
	}
	
	ioBaseChar *pChar = pWeapon->GetOwner();
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone && pChar )
	{
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		pZone->SetCollisionDir( vDir );
		pZone->SetZoneValue( true, true );
	}

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioMoveAttackSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	SetSkillActionState( pChar );
}

bool ioMoveAttackSkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_PRE_ACTION:
	case SS_LOOP:
	case SS_END:
		{
			return false;
		}
		break;
	case SS_ACTION:
	case SS_END_ATTACK:
		if( m_dwMotionEndTime > FRAMEGETTIME() )
			return false;
	}
	
	return true;
}

bool ioMoveAttackSkill3::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_SkillUseType == SUT_JUMP )
			return true;
		break;
	case SS_PRE_ACTION:
	case SS_ACTION:
	case SS_END_ATTACK:
		return false;
	case SS_LOOP:
		return true;
	}

	return false;
}

bool ioMoveAttackSkill3::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioMoveAttackSkill3::IsAttackEndState() const
{
	return IsEndState();
}

bool ioMoveAttackSkill3::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return false;
}

bool ioMoveAttackSkill3::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMoveAttackSkill3::ProcessSkillPreActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		SetSkillLoopState( pChar );
	}
}

void ioMoveAttackSkill3::ProcessSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );
		if( fCurTicGauge != 0.0f )
		{
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
			{
				SetSkillEndState( pChar );
				return;
			}
		}
	}

	if( pChar->IsNeedProcess() )
	{
		if( pChar->IsAttackKey() )
			SetSkillActionState( pChar );
	}
}

void ioMoveAttackSkill3::ProcessSkillActionState( ioBaseChar *pChar )
{
	if( CheckLanding( pChar ) )
	{
		SetSkillEndAttackState( pChar );
	}
}

void ioMoveAttackSkill3::SetSkillPreActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_SkillState = SS_PRE_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( m_SkillUseType != SUT_JUMP )
		pChar->SetJumpPower( m_fCharJumpPower );
}

void ioMoveAttackSkill3::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopAnimation );
	if( iAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fSkillLoopAniRate );

	pChar->SetSkillMoveEnable( true, m_bDirection );

	D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
	pChar->SkillFireExplicit( m_LoopAttack, GetName(), m_dwWeaponBaseIndex, vCurPos );

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage && m_dwSkillMapEffect == -1 )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		vPos.y = pChar->GetBottomHeight();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pEffect = pStage->CreateMapEffect( m_SkillMapEffectName, vPos, vScale );
		if( pEffect )
			m_dwSkillMapEffect = pEffect->GetUniqueID();
	}
}

void ioMoveAttackSkill3::SetSkillActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_SkillState = SS_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_EndAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_EndAttack.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->SetSkillMoveEnable( false, false );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwLoopWeaponIdx );
	if( pWeapon )
	{
		pWeapon->SetWeaponDead();
	}

	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		pChar->ReduceCurSkillGaugeBySkill( pItem, pItem->GetMaxSkillGauge( pChar->GetCharName() ) );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioMoveAttackSkill3::SetSkillEndAttackState( ioBaseChar *pChar )
{
	m_SkillState = SS_END_ATTACK;

	AttackSkillFire( pChar );

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage && m_dwSkillMapEffect != -1 )
	{
		pStage->DestroyMapEffect( m_dwSkillMapEffect );
		m_dwSkillMapEffect = -1;
	}
}

void ioMoveAttackSkill3::SetSkillEndState( ioBaseChar *pChar )
{
	m_SkillState = SS_END;
}

bool ioMoveAttackSkill3::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}