

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioGhostStateBuff.h"


ioGhostStateBuff::ioGhostStateBuff()
{
}

ioGhostStateBuff::ioGhostStateBuff( const ioGhostStateBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
m_fReduceGaugeOnMissile( rhs.m_fReduceGaugeOnMissile ),
m_fReduceGaugeOnExplosion( rhs.m_fReduceGaugeOnExplosion )
{
}

ioGhostStateBuff::~ioGhostStateBuff()
{
}

void ioGhostStateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_fReduceGaugeOnMissile = rkLoader.LoadFloat_e( "reduce_gauge_on_missile", 0.0f );
	m_fReduceGaugeOnExplosion = rkLoader.LoadFloat_e( "reduce_gauge_on_explosion", 0.0f );
}

ioBuff* ioGhostStateBuff::Clone()
{
	return new ioGhostStateBuff( *this );
}

void ioGhostStateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	m_bReduceGauge = false;

	m_bFirstSend = false;
}

bool ioGhostStateBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	m_bReduceGauge = false;

	m_bFirstSend = false;

	return true;
}

void ioGhostStateBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	if ( m_iOperationType == OT_PASSIVE )
	{
		if( m_fGaugePerTick <= 0.0f || m_dwTickTime <= 0 )
			return;
	}
	
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
	case OT_DELAY:
		break;
	case OT_ACTIVE:
		if( m_dwBuffEndTime <= FRAMEGETTIME() )
		{
			SetReserveEndBuff();
			if( m_pOwner && m_pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_PASSIVE_GAUGE );
				kPacket << m_pOwner->GetCharName();
				kPacket << -1;
				kPacket << 0;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		else if ( !m_bFirstSend )
		{
			m_bFirstSend = true;
			if( m_pOwner && m_pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_PASSIVE_GAUGE );
				kPacket << m_pOwner->GetCharName();
				kPacket << -1;
				kPacket << 0;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		break;
	case OT_PASSIVE:
		float fCurTicGauge = m_fGaugePerTick / m_fTicGaugeRateByGrowth;
		int iTickGap = (int)( dwCurTime - m_dwCheckTime );
		int iCurTicTime = m_dwTickTime + m_dwTicTimeByGrowth;
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );
			if( fCurTicGauge != 0.0f )
			{
				for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
				{
					ioSkill *pSkill = m_pOwner->GetEquipedSkill(i);
					ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);

					if( !pPassiveSkill )
						continue;

					if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
						continue;

					if( pPassiveSkill->UsePassiveGauge() )
					{
						if( m_pOwner )
						{
							ioPlayStage *pStage = m_pOwner->GetCreator();
							if( pStage )
							{
								D3DXVECTOR3 vCurPos = m_pOwner->GetWorldPosition();
								float fMapHeight = pStage->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y+3.0f );
								if( fMapHeight <= 0.0f )
								{
									m_bReduceGauge = true;
									pPassiveSkill->ReduceCurPassiveGauge( fCurTicGauge );
								}
								else
								{
									m_bReduceGauge = false;
								}
							}
						}
					}
				}
			}
		}
		break;
	}
}

bool ioGhostStateBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioGhostStateBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

void ioGhostStateBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	ioBuff::EndBuff();
}

bool ioGhostStateBuff::OnReduceGauge() const
{
	return m_bReduceGauge;
}

bool ioGhostStateBuff::CheckPassiveGauge() const
{
	for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
	{
		ioSkill *pSkill = m_pOwner->GetEquipedSkill(i);
		ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);

		if( !pPassiveSkill )
			continue;

		if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
			continue;

		if( pPassiveSkill->UsePassiveGauge() )
		{
			if( m_pOwner )
			{
				if( pPassiveSkill->GetCurPassiveGauge() > 0.0f )
					return true;
			}
		}
	}
	return false;
}

bool ioGhostStateBuff::CheckDuration() const
{
	if ( m_iOperationType == OT_ACTIVE )
	{
		if( m_dwBuffEndTime > FRAMEGETTIME() && m_bLive )
			return true;
	}

	return false;
}

float ioGhostStateBuff::SetReduceGauge( IgnoreGhostStateType eIgnoreGhostType )
{
	if( !m_pOwner )	return -FLOAT1;
	if( eIgnoreGhostType == IGT_NONE )	return -FLOAT1;

	float fCheckGauge;
	for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
	{
		ioSkill *pSkill = m_pOwner->GetEquipedSkill(i);
		ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);

		if( !pPassiveSkill )
			continue;

		if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
			continue;

		if( pPassiveSkill->UsePassiveGauge() )
		{
			ioPlayStage *pStage = m_pOwner->GetCreator();
			if( pStage )
			{
				if( eIgnoreGhostType == IGT_MISSILE )
					fCheckGauge = pPassiveSkill->ReduceCurPassiveGauge( m_fReduceGaugeOnMissile );
				else if( eIgnoreGhostType == IGT_EXPLOSION )
					fCheckGauge = pPassiveSkill->ReduceCurPassiveGauge( m_fReduceGaugeOnExplosion );
			}
		}
	}
	return fCheckGauge;
}
