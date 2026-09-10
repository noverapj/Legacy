

#include "stdafx.h"

#include "ioBaseChar.h"


#include "ioNormalFireSkill.h"

ioNormalFireSkill::ioNormalFireSkill()
{
	m_dwFireStartTime = 0;
}

ioNormalFireSkill::ioNormalFireSkill( const ioNormalFireSkill &rhs )
: ioNormalSkill( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute )
{
	m_dwFireStartTime = 0;
}

ioNormalFireSkill::~ioNormalFireSkill()
{
}

void ioNormalFireSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	LoadAttackAttribute_e( "attack", m_AttackAttribute, rkLoader );
}

ioSkill* ioNormalFireSkill::Clone()
{
	return new ioNormalFireSkill( *this );
}

bool ioNormalFireSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	// hand mesh를 predelay 시점에 하기에...
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwFireStartTime = 0;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	return true;
}

void ioNormalFireSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			m_dwFireStartTime = 0;
			SetNormalFire( pChar );
		}

		CheckExtraAniJump( pChar );
		return;
	}

	DWORD dwPreDelayTime = GetPreDelayTime();
	if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );

		SetNormalSkill( pChar );
	}
}

void ioNormalFireSkill::OnSkillEnd( ioBaseChar *pChar )
{
	m_dwFireStartTime = 0;

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	ioNormalSkill::OnSkillEnd( pChar );
}

bool ioNormalFireSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

void ioNormalFireSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	//
	SetExtraAniJump( pChar, iCurSkillAniID, fTimeRate );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioNormalFireSkill::SetNormalFire( ioBaseChar *pChar )
{
	if( !pChar ) return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	int iCnt = m_AttackAttribute.m_vWeaponInfoList.size();
	for( int i=0; i < iCnt; ++i )
	{
		pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList[i],
								  GetName(),
								  m_dwWeaponBaseIndex+i,
								  vPos );
	}
}





