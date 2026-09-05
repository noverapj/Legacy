

#include "stdafx.h"

#include "ioRangeBuffSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"

ioRangeBuffSkill::ioRangeBuffSkill()
{
	m_dwAttackKeyEndEnableTime = 0;
}

ioRangeBuffSkill::ioRangeBuffSkill( const ioRangeBuffSkill &rhs )
: ioRangeSkill( rhs ),
m_dwFloatActionFireTime( rhs.m_dwFloatActionFireTime ),
m_dwAttackKeyInputEnableTime( rhs.m_dwAttackKeyInputEnableTime )
{
	m_bExtraAction = false;
	m_dwAttackKeyEndEnableTime = 0;
}

ioRangeBuffSkill::~ioRangeBuffSkill()
{
	m_bExtraAction = false;
}

void ioRangeBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioRangeSkill::LoadProperty( rkLoader );

	m_dwFloatActionFireTime = rkLoader.LoadInt_e( "motion_duration", 0 );
	m_dwAttackKeyInputEnableTime = rkLoader.LoadInt_e( "attack_key_input_enable_time", 0 );
}

ioSkill* ioRangeBuffSkill::Clone()
{
	return new ioRangeBuffSkill( *this );
}

void ioRangeBuffSkill::SetBuffList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner ) return;

	/*
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	*/

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( FLOAT08 );
	vPos += m_vEffectDir * m_fOffSet;

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
		vPos += m_vEffectDir * m_fSkillRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
		vPos -= vOffset;
		pOwner->GetTargetCharListByBox( vPos, m_fSkillRange, m_fWidth, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	}

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		vTargetList.clear();

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);

		if( !pChar || IsCollisioned(pChar) )	continue;
		if( !pChar->IsNeedProcess() )			continue;
		if( pChar->GetState() == CS_SKIP_STUN )	continue;
		if( Help::CheckTargetNoCollision( pChar->GetState() ) ) continue;
		if( !pChar->HasCollisionAvailableBuff() ) continue;
		if( !pStage->IsCollisionCheckByMode() )	continue;
		if( pOwner->GetCharName() == pChar->GetCharName() )
			continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( pChar->IsSkipState() ) continue;

		D3DXVECTOR3 vStartPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vEndPos = pChar->GetMidPositionByRate();
		if( m_bCheckCollisionLine && !pStage->CheckCollisionLine( vStartPos, vEndPos ) )
			continue;

		if( !pChar->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		bool bEnableTeamAttack = pChar->IsEnableTeamAttack();

		if( pOwner->GetTeam() == pChar->GetTeam() && SetTeamBuffList( pOwner, pChar ) )	// Team
		{
			if( m_bEnableAttackCnt )
				pOwner->CheckTeamBuffAttackCnt( pChar );
		}
		
		if( (bEnableTeamAttack || pOwner->GetTeam() != pChar->GetTeam()) )	// Enemy
		{
			bool bEnemyBuffSuccess = false;

			if( m_EnemyBuffType == EBT_NORMAL )
			{
				bEnemyBuffSuccess = SetEnemyBuffList( pOwner, pChar );
			}
			else if( m_EnemyBuffType == EBT_RANDOM )
			{
				bEnemyBuffSuccess = SetEnemyRandomBuffList( pOwner, pChar );
			}
			else if( m_EnemyBuffType == EBT_DIRECTION )
			{
				bEnemyBuffSuccess = SetEnemyDirectionBuffList( pOwner, pChar );
			}

			if( bEnemyBuffSuccess && m_bEnableAttackCnt )
			{
				//pOwner->CheckRealComboHit( pChar->GetCharName() );
				pOwner->CheckComboSkillHit();
				pOwner->CheckAttackCnt( pChar );
			}
		}
	}

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}
}


void ioRangeBuffSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_RangeState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
		}
		break;
	case RS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionStartTime < FRAMEGETTIME() )
		{	
			ioEntityGroup *pGrp = pChar->GetGroup();
			int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );
			if( iAniID == -1 )
			{				
				if( m_dwMotionStartTime > 0 && m_dwMotionStartTime+m_dwFloatActionFireTime >= FRAMEGETTIME() )
				{					
					SetExtraActionState( pChar, pStage );
					SetBuffList( pChar, pStage );
				}
				if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
				{						
					SetEndState( pChar );
				}
			}
			else
			{
				if( 0 < m_dwMotionEndTime &&m_dwMotionEndTime < FRAMEGETTIME() )
				{	
					m_dwMotionStartTime = 0;
					SetLoopState( pChar, pStage );
				}
			}
		}
		break;
	case RS_LOOP:
		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < FRAMEGETTIME() )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
		}
		else if( m_dwLoopStartTime > 0 && ( m_dwLoopStartTime+m_dwDuration >= FRAMEGETTIME() ) )
		{
			SetBuffList( pChar, pStage );
		}

		if ( pChar->IsNeedProcess() )
		{
			if ( m_dwAttackKeyInputEnableTime > 0 && 
				m_dwAttackKeyEndEnableTime < FRAMEGETTIME() && pChar->IsAttackKey() )
			{
				m_dwLoopStartTime = 0;
				SetEndState( pChar );
				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << RS_END;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		break;
	}
}


void ioRangeBuffSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iAniID == -1 )
	{
		if ( !m_bNotUseKeyReserve )
			m_dwEnableReserveTime = FRAMEGETTIME();
		LOG.PrintTimeAndLog( 0, "ioRangeBuffSkill::SetLoopState() - %s, Not Exist LoopAnimation",
								 GetName().c_str() );
	}
	else
	{
		pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );
		if ( !m_bNotUseKeyReserve )
		{
			m_dwEnableReserveTime = FRAMEGETTIME();
			m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( FLOAT08 ) + m_vEffectDir * m_fOffSet;
	pChar->CreateMapEffectBySkill( m_RangeEffect, vPos );

	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_LoopHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_RangeState = RS_LOOP;

	SetOwnerBuffList( pChar );

	m_dwAttackKeyEndEnableTime = FRAMEGETTIME() + m_dwAttackKeyInputEnableTime;
}

void ioRangeBuffSkill::SetExtraActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_bExtraAction )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( FLOAT08 ) + m_vEffectDir * m_fOffSet;
	pChar->CreateMapEffectBySkill( m_RangeEffect, vPos );

	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	SetOwnerBuffList( pChar );
	m_bExtraAction = true;
}

bool ioRangeBuffSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioRangeSkill::OnSkillStart(pChar) )
		return false;

	m_bExtraAction = false;
	m_dwAttackKeyEndEnableTime = 0;

	return true;
}

void ioRangeBuffSkill::OnSkillEnd( ioBaseChar *pChar )
{
	if( !m_bReduceGauge && pChar )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	ioRangeSkill::OnSkillEnd( pChar );

	m_bExtraAction = false;
	m_dwAttackKeyEndEnableTime = 0;
}

void ioRangeBuffSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;
	switch( iType )
	{
	case RS_END:
		{
			m_dwLoopStartTime = 0;
			SetEndState( pOwner );
		}
		break;
	default:
		{
			BaseCharList vTargetList;
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( FLOAT08 );
			vPos += m_vEffectDir * m_fOffSet;

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
				vPos += m_vEffectDir * m_fSkillRange * FLOAT05; 
				D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
				vPos -= vOffset;
				pOwner->GetTargetCharListByBox( vPos, m_fSkillRange, m_fWidth, m_fUpHeight, m_fUnderHeight, vTargetList );
				break;
			}

			if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
				vTargetList.clear();

			BaseCharList::iterator iter;
			for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
			{
				ioBaseChar *pChar = (*iter);

				if( !pChar || IsCollisioned(pChar) ) continue;
				if( !pChar->IsNeedProcess() ) continue;
				if( !pStage->IsCollisionCheckByMode() ) continue;
				if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;

				D3DXVECTOR3 vStartPos = pOwner->GetMidPositionByRate();
				D3DXVECTOR3 vEndPos = pChar->GetMidPositionByRate();
				if( m_bCheckCollisionLine && !pStage->CheckCollisionLine( vStartPos, vEndPos ) )
					continue;

				if( !pChar->CheckTargetWoundType(m_TargetWoundType) )
					continue;

				bool bEnableTeamAttack = pChar->IsEnableTeamAttack();

				if( pOwner->GetCharName() == pChar->GetCharName() && !IsCollisioned(pChar) )
				{
					SetOwnerBuffList( pChar );
				}
				else if( pOwner->GetTeam() == pChar->GetTeam() && SetTeamBuffList( pOwner, pChar ) )	// Team
				{
					pOwner->CheckTeamBuffAttackCnt( pChar );
				}

				if( (bEnableTeamAttack || pOwner->GetTeam() != pChar->GetTeam()) && SetEnemyBuffList( pOwner, pChar ) )	// Enemy
				{
					//pOwner->CheckRealComboHit( pChar->GetCharName() );
					pOwner->CheckComboSkillHit();
					pOwner->CheckAttackCnt( pChar );
				}
			}
		}
	}
}

void ioRangeBuffSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
