

#include "stdafx.h"

#include "ioThrowSkill.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioThrowSkill::ioThrowSkill()
{
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_bSkillMotionEnd = false;
	m_bSetHandMesh = false;
}

ioThrowSkill::ioThrowSkill( const ioThrowSkill &rhs )
: ioAttackSkill( rhs ),
 m_SkillLoopAnimation( rhs.m_SkillLoopAnimation ),
 m_SkillEndAnimation( rhs.m_SkillEndAnimation ),
 m_fSkillEndAniRate( rhs.m_fSkillEndAniRate ),
 m_StartHandMesh( rhs.m_StartHandMesh ),
 m_EndHandMesh( rhs.m_EndHandMesh ),
 m_iSecondAttributeIndex( rhs.m_iSecondAttributeIndex )
{
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_bSkillMotionEnd = false;
	m_bSetHandMesh = false;
}

ioThrowSkill::~ioThrowSkill()
{
}

void ioThrowSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "start_hand_mesh", "", szBuf, MAX_PATH );
	m_StartHandMesh = szBuf;
	rkLoader.LoadString_e( "end_hand_mesh", "", szBuf, MAX_PATH );
	m_EndHandMesh = szBuf;

	rkLoader.LoadString_e( "throw_loop_animation", "", szBuf, MAX_PATH );
	m_SkillLoopAnimation = szBuf;
	rkLoader.LoadString_e( "throw_end_animation", "", szBuf, MAX_PATH );
	m_SkillEndAnimation = szBuf;
	
	m_fSkillEndAniRate = rkLoader.LoadFloat_e( "throw_end_ani_rate", FLOAT1 );
	m_iSecondAttributeIndex = rkLoader.LoadInt_e( "throw_second_attribute", 0 );
}

ioSkill* ioThrowSkill::Clone()
{
	return new ioThrowSkill( *this );
}

bool ioThrowSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	return m_bSkillMotionEnd;
}

bool ioThrowSkill::IsProtected( int iDefenseBreakType ) const
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
		return true;
	}

	return false;
}

bool ioThrowSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_bSkillMotionEnd = false;
	m_bSetHandMesh = false;
	m_bEquipedMesh = true;

	return ioAttackSkill::OnSkillStart( pChar );
}

void ioThrowSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioAttackSkill::OnProcessState( pChar, pStage );

	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime == 0 && FRAMEGETTIME() > m_dwMotionEndTime )
			SetSkillLoopState( pChar );
		break;
	case SS_LOOP:
		ProcessSkillLoopState( pChar );
		break;
	case SS_END:
		ProcessSkillEndState( pChar );
		break;
	}
}

void ioThrowSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID == -1 )	return;

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
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;


	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	// 기본 무기 손에서 제거
	pChar->ShowWeaponItemMesh( false );

	// 던질무기 손에 추가
	if( !pChar->IsHasCrown() && !m_bSetHandMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->AddEntity( m_StartHandMesh );
		m_bSetHandMesh = true;
	}

	// 던질 무기 장착 지점에서 제거
	if( !pChar->IsHasCrown() && m_bSetHandMesh )
	{
		if( pChar->ChangeSkillWeaponMesh( this, "", m_bVisibleEquipMesh ) )
			m_bEquipedMesh = false;
	}
}

void ioThrowSkill::SetSkillLoopState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillLoopAnimation );
	if( iCurSkillAniID != -1 )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100 );

		m_dwLoopEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iCurSkillAniID );
	}

	// 던질 무기 장착 지점에서 제거
	if( !pChar->IsHasCrown() && !m_bSetHandMesh )
	{
		if( pChar->ChangeSkillWeaponMesh( this, "", m_bVisibleEquipMesh ) )
			m_bEquipedMesh = false;
	}

	// 던진 무기 손에서 제거
	if( !pChar->IsHasCrown() && m_bSetHandMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->DestroyEntity( m_StartHandMesh );
		m_bSetHandMesh = false;
	}

	m_SkillState = SS_LOOP;
}

void ioThrowSkill::SetSkillEndState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_END ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndAnimation );
	if( iCurSkillAniID == -1 )	return;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSkillEndAniRate );

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fSkillEndAniRate;

	m_SkillState = SS_END;

	if( !pChar->IsHasCrown() && !m_bSetHandMesh && !m_EndHandMesh.IsEmpty() )
	{
		pChar->AddEntity( m_EndHandMesh );
		m_bSetHandMesh = true;
	}

	// 던진 무기 장착 지점에 추가
	if( !pChar->IsHasCrown() && !m_bEquipedMesh )
	{
		if( pChar->RestoreSkillWeaponMesh( this, "", m_bVisibleEquipMesh, false ) )
			m_bEquipedMesh = true;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioThrowSkill::ProcessSkillLoopState( ioBaseChar *pChar )
{
	if( m_dwLoopEndTime < FRAMEGETTIME() )
	{
		SetSkillEndState( pChar );
	}
}

void ioThrowSkill::ProcessSkillEndState( ioBaseChar *pChar )
{
	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_bSkillMotionEnd = true;
	}
}

void ioThrowSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	pChar->ShowWeaponItemMesh( true );

	// 던진 무기 장착지점에 장착
	if( !pChar->IsHasCrown() && !m_bEquipedMesh )
	{
		if( pChar->RestoreSkillWeaponMesh( this, "", m_bVisibleEquipMesh, false ) )
			m_bEquipedMesh = true;
	}

	// 던진 무기 손에서 제거
	if( !pChar->IsHasCrown() && m_bSetHandMesh && !m_EndHandMesh.IsEmpty() )
	{
		pChar->DestroyEntity( m_EndHandMesh );
		m_bSetHandMesh = false;
	}
}

void ioThrowSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	SetSkillEndState( pChar );
}

void ioThrowSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioThrowReturnWeapon *pThrow = ToThrowReturnWeapon( pWeapon );
	if( pThrow )
	{
		pThrow->SetSecondAttribute( m_iSecondAttributeIndex );

		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			pWeapon->SetStartPosition( pChar->GetCharHeadPos(1.0f), pStage );
		}
	}
}
