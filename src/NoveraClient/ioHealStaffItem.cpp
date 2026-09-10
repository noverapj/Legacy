

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioHealStaffItem.h"

ioHealStaffItem::ioHealStaffItem()
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_HealState = HS_NONE;

	m_bAimedBall = false;
}

ioHealStaffItem::ioHealStaffItem( const ioHealStaffItem &rhs )
: ioWeaponItem( rhs ),
 m_PreGatherAnimation( rhs.m_PreGatherAnimation ),
 m_GatherAnimation( rhs.m_GatherAnimation ),
 m_EndGatherAnimation( rhs.m_EndGatherAnimation ),
 m_fAimRange( rhs.m_fAimRange ),
 m_fMinRange( rhs.m_fMinRange ),
 m_fAimAngle( rhs.m_fAimAngle ),
 m_fGaugePerAllDefense( rhs.m_fGaugePerAllDefense ), 
 m_dwDefenseTicTime( rhs.m_dwDefenseTicTime ),
 m_fMarkerRange( rhs.m_fMarkerRange ),
 m_GatherEffect( rhs.m_GatherEffect ),
 m_fSkillGaugeRate( rhs.m_fSkillGaugeRate ),
 m_bEnemyTarget( rhs.m_bEnemyTarget )
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_HealState = HS_NONE;

	m_bAimedBall = false;
}

ioHealStaffItem::~ioHealStaffItem()
{
}

void ioHealStaffItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "pre_gather_animation", "", szBuf, MAX_PATH );
	m_PreGatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;
	rkLoader.LoadString_e( "end_gather_animation", "", szBuf, MAX_PATH );
	m_EndGatherAnimation = szBuf;

	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", 800.0f );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", FLOAT100 );
	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 400.0f );

	m_bEnemyTarget = rkLoader.LoadBool_e( "enable_enemy_target", false );

	m_fGaugePerAllDefense = rkLoader.LoadFloat_e( "gauge_per_defense", 5.0f );
	m_dwDefenseTicTime = rkLoader.LoadInt_e( "defense_tic_time", 500 );
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );
}

ioItem* ioHealStaffItem::Clone()
{
	return new ioHealStaffItem( *this );
}

void ioHealStaffItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_HEAL_ITEM,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );
}

void ioHealStaffItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	ioWeaponItem::OnReleased( pOwner );
}

void ioHealStaffItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_GatherEffect );
}

void ioHealStaffItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	bool bEnable = false;
	if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL, false ) )
		bEnable = true;

	// 스킬 사용가능 여부를 먼저 설정해 둘 것
	pOwner->SetTargetMarkerEnable( bEnable );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	if( iCurCombo == 0 )
	{
		m_HealState = HS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		
		int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimation );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_HealState = HS_NONE;
	}
}

void ioHealStaffItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_HealState = HS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_HealState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHealStaffItem::ChangeToAiming( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );

	m_HealState = HS_AIMING;
	m_AimedTarget.Clear();

	pOwner->AttachEffect( m_GatherEffect );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_HealState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHealStaffItem::ChangeToAimed( ioBaseChar *pOwner, ioHashString szTarget )
{
	m_HealState = HS_AIMED;
	m_AimedTarget = szTarget;
	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_HealState;
		kPacket << m_iCurCombo;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHealStaffItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_HealState )
	{
	case HS_CHARGING:
		OnCharging( pOwner );
		break;
	case HS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case HS_AIMING:
		OnAiming( pOwner );
		break;
	case HS_AIMED:
		OnAimed( pOwner );
		break;
	}

	if( m_HealState == HS_AIMING ||	m_HealState == HS_AIMED )
	{
		if( pOwner->IsNeedProcess() && pOwner->ChangeDirectionExplicitIfKeyChanged( false ) )
		{
			if( m_HealState != HS_AIMING )
			{
				pOwner->EndEffect( m_GatherEffect );
				ChangeToAiming( pOwner );
			}
		}
	}
}

void ioHealStaffItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			pOwner->EndEffect( m_GatherEffect );
			ChangeToAiming( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioHealStaffItem::OnAiming( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )	// 버튼 뗌
	{
		pOwner->SetChargingState( false );
		pOwner->SetState( CS_DELAY );

		m_AimedTarget.Clear();
		m_HealState = HS_NONE;
		pOwner->EndEffect( m_GatherEffect );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_HealState;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
	
	//Targeting
	ioHashString szTarget;
	if( pOwner->CheckAimedTargetByStaff( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, !m_bEnemyTarget ) )
	{
		ChangeToAimed( pOwner, szTarget );	// 타겟 세팅됨
	}
	else
	{
		if( pOwner->IsNeedProcess() )
		{
			bool bEnable = false;
			if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL, false ) )
				bEnable = true;

			pOwner->SetTargetMarkerEnable( bEnable );

			if( pOwner->IsDefenseKeyDown() && !pOwner->IsBuffLimitSkill() )
			{
				if(pOwner->SetUseSkill(0, SUT_NORMAL))
				{
					pOwner->SetChargingState( false );
					m_HealState = HS_NONE;
					return;
				}
			}
		}
	}
}

void ioHealStaffItem::OnAimed( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsAttackKeyDown() )	// 조준모드중 상대 따라다니기
		{
			if( CheckCurrentTargetValid( pOwner, false ) )
			{
				bool bEnable = false;
				if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL, false ) )
					bEnable = true;

				pOwner->SetTargetMarkerEnable( bEnable );

				TrackingTarget( pOwner );
				if( pOwner->IsDefenseKeyDown() && !pOwner->IsBuffLimitSkill() )
				{
					if(pOwner->SetUseSkill(0, SUT_NORMAL))
					{
						pOwner->SetChargingState( false );
						m_HealState = HS_NONE;
						return;
					}
				}
			}
			else
			{
				pOwner->EndEffect( m_GatherEffect );
				ChangeToAiming( pOwner );
			}
		}
		else
		{
			pOwner->SetChargingState( false );
			pOwner->SetState( CS_DELAY );

			m_AimedTarget.Clear();
			m_HealState = HS_NONE;
			
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_HealState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else
	{
		if( CheckCurrentTargetValid( pOwner, false ) )
		{
			TrackingTarget( pOwner );
		}
	}
}

bool ioHealStaffItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > m_fAimRange * m_fAimRange )
		return false;

	if( D3DXVec3LengthSq( &vDiff ) < m_fMinRange * m_fMinRange )
		return false;

	if( bAngleAlsoCheck )
	{
		D3DXVECTOR3 vCharDir;
		vCharDir = pOwner->GetMoveDir();
		
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
		if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
			return false;
	}

	return true;
}

void ioHealStaffItem::TrackingTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	pOwner->SetTargetRotToTargetPos( pTarget, true );
}

void ioHealStaffItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	ioHashString szTarget;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case HS_NORMAL_ATTACK:
		ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
		m_HealState = HS_NORMAL_ATTACK;
		break;
	case HS_AIMING:
		pOwner->EndEffect( m_GatherEffect );
		ChangeToAiming( pOwner );
		break;
	case HS_AIMED:
		rkPacket >> szTarget;
		ChangeToAimed( pOwner, szTarget );
		break;
	case HS_NONE:
		m_HealState = HS_NONE;
		m_AimedTarget.Clear();

		pOwner->EndEffect( m_GatherEffect );
		pOwner->SetState( CS_DELAY );
		break;
	}
}

ioWeaponItem::WeaponSubType ioHealStaffItem::GetSubType() const
{
	return WST_HEAL_STAFF;
}

bool ioHealStaffItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_HealState == HS_CHARGING ||
			m_HealState == HS_AIMING ||
			m_HealState == HS_AIMED )
			return true;
	}

	return false;
}

void ioHealStaffItem::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
{
	if( m_dwTicCheckTime == 0 )
		return;

	float fRecoveryGauge = 0.0f;

	if( pOwner->GetState() == CS_RUN ||
		pOwner->GetState() == CS_DELAY ||
		pOwner->GetState() == CS_SPECIAL_MOTION ||
		pOwner->GetState() == CS_ETCITEM_MOTION ||
		IsCanUpdateGauge(pOwner) )
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetDelayRunGaugeRate();
	}
	else if( pOwner->GetState() == CS_DIE ||
			 pOwner->GetState() == CS_VIEW ||
			 pOwner->GetState() == CS_LOADING ||
			 pOwner->GetState() == CS_OBSERVER )
	{
		bUpdate = false;
	}
	else
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetEtcGaugeRate();
	}


	DWORD dwCurTime = FRAMEGETTIME();
	int iCurRecoveryTic = (int)pOwner->GetDefaultRecoveryGaugeTic();

	float fPreGauge = m_fCurSkillGauge;
	float fNewRecovery = fRecoveryGauge * fRate;

	int iTickGap = (int)( dwCurTime - m_dwTicCheckTime );
	if( iTickGap >= iCurRecoveryTic )
	{
		while( iTickGap >= iCurRecoveryTic )
		{
			if( bUpdate )
			{
				m_fCurSkillGauge = min( m_fCurSkillGauge+fNewRecovery, FLOAT100 );
			}

			iTickGap -= iCurRecoveryTic;
		}

		m_dwTicCheckTime = dwCurTime - iTickGap;
	}

//#ifndef SHIPPING 
//	if( Setting::ShowExtraInfo() )
//	{
//		float fMaxGauge = GetMaxSkillGauge( pOwner->GetCharName() );
//
//		if( fMaxGauge < FLOAT100 )
//		{
//			if( fPreGauge == 0.0f && m_fCurSkillGauge > 0.0f )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, MaxGauge: %f", GetSkillName().c_str(), fMaxGauge );
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryStart: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//
//			if( fPreGauge < fMaxGauge && m_fCurSkillGauge >= fMaxGauge )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryEnd: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//		}
//	}
//#endif

	if( fRate > 0.0f )
		m_bUpdateEnableSkillGauge = bUpdate;
	else
		m_bUpdateEnableSkillGauge = false;
}

bool ioHealStaffItem::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}