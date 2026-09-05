
#include "stdafx.h"

#include "ioCreateWeaponAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioCreateWeaponAttackSkill::ioCreateWeaponAttackSkill()
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_nCurOffset = 0;
}

ioCreateWeaponAttackSkill::ioCreateWeaponAttackSkill( const ioCreateWeaponAttackSkill &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_StartAttack( rhs.m_StartAttack ),
	  m_LastAttack( rhs.m_LastAttack ),
	  m_vOffsetList( rhs.m_vOffsetList )
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_nCurOffset = 0;
}

ioCreateWeaponAttackSkill::~ioCreateWeaponAttackSkill()
{
	m_vOffsetList.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCreateWeaponAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
		
	//스킬 무적 시간
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadAttackAttribute( "start_attack", m_StartAttack, rkLoader );
	LoadAttackAttribute( "last_attack", m_LastAttack, rkLoader );

	char szBuf[MAX_PATH] = "";
	m_vOffsetList.clear();

	int nOffsetCnt = rkLoader.LoadInt_e( "start_attack_type_count", 0 );
	for ( int i=0; i< nOffsetCnt; ++i )
	{
		D3DXVECTOR3 vOffset;
		wsprintf_e( szBuf, "start_attack_type%d_offset_x", i+1 );
		vOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "start_attack_type%d_offset_y", i+1 );
		vOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "start_attack_type%d_offset_z", i+1 );
		vOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vOffsetList.push_back( vOffset );
	}
}

ioSkill* ioCreateWeaponAttackSkill::Clone()
{
	return new ioCreateWeaponAttackSkill( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioCreateWeaponAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_nCurOffset = 0;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
		
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	
	
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioCreateWeaponAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetAttackState( pChar, &m_StartAttack, SS_START_ATTACK );
			}
		}
		break;
	case SS_START_ATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetAttackState( pChar, &m_LastAttack, SS_LAST_ATTACK );
		}
		break;
	case SS_LAST_ATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar );
		}
		break;
	}
}

void ioCreateWeaponAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
}

bool ioCreateWeaponAttackSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{		
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioCreateWeaponAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_START_ATTACK:
	case SS_LAST_ATTACK:
		return false;
	case SS_END:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioCreateWeaponAttackSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCreateWeaponAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioMissileWeapon *pioMissileWeapon = ToMissileWeapon( pWeapon );
	if ( pioMissileWeapon )
	{
		ioBaseChar* pOwner = pioMissileWeapon->GetOwner();
		if ( pOwner )
		{
			if ( !m_vOffsetList.empty() && COMPARE( m_nCurOffset, 0, (int)m_vOffsetList.size() ) )
			{
				D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * m_vOffsetList[m_nCurOffset];
				D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vDir;
				D3DXVec3Normalize( &vDir, &vDir );

				pioMissileWeapon->SetStartPosition( vPos, pStage );
				pioMissileWeapon->SetPosition( vPos );
				pioMissileWeapon->SetMoveDir( -vDir );

				m_nCurOffset++;
			}
		}
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pOwner = pZone->GetOwner();
		if( pOwner )
		{
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCreateWeaponAttackSkill::SetAttackState( ioBaseChar *pChar, const AttackAttribute* pAttackAttribute, SkillState eState )
{
	if ( !pChar || !pAttackAttribute )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( pAttackAttribute->m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = pAttackAttribute->m_fAttackAniRate;
	DWORD dwPreDelay = pAttackAttribute->m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
		
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->RefreshFireTimeList( nAniID, pAttackAttribute->m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( pAttackAttribute->m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_SkillState = eState;
}

void ioCreateWeaponAttackSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_END;
}