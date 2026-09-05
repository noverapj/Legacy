#include "StdAfx.h"
#include "ioAccCriticalRateBuff.h"

ioAccCriticalRateBuff::ioAccCriticalRateBuff(void)
{
	m_fAddCriticalRate = 0.0f;
}

ioAccCriticalRateBuff::ioAccCriticalRateBuff( const ioAccCriticalRateBuff &rhs ):
ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState )
{
	m_fAddCriticalRate = 0.0f;
}

ioAccCriticalRateBuff::~ioAccCriticalRateBuff(void)
{
}

ioBuff* ioAccCriticalRateBuff::Clone()
{
	return new ioAccCriticalRateBuff( *this );
}

void ioAccCriticalRateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

void ioAccCriticalRateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	ioItem *pItem= GetOwnerItem();
	if ( !pItem )
	{
		SetReserveEndBuff();
		return;
	}

	float fNum = 0.f;
	ioSkill *pSkill = pOwner->GetEquipedSkill( m_SkillName );
	if( pSkill )
		fNum =  pSkill->GetAccessoryRandomNum() / FLOAT1000;

	if ( fNum <= 0.0f )
	{
		SetReserveEndBuff();
		return;
	}

	m_fAddCriticalRate = fNum;
	pOwner->AddCriticalRate( m_fAddCriticalRate );
}

bool ioAccCriticalRateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	ioItem *pItem= GetOwnerItem();
	if ( !pItem )
	{
		SetReserveEndBuff();
		return false;
	}

	float fNum = 0.f;
	ioSkill *pSkill = pOwner->GetEquipedSkill( m_SkillName );
	if( pSkill )
		fNum =  pSkill->GetAccessoryRandomNum() / FLOAT1000;

	if ( fNum <= 0.0f )
	{
		SetReserveEndBuff();
		return false;
	}

	m_fAddCriticalRate = fNum;
	pOwner->AddCriticalRate( m_fAddCriticalRate );
	return true;
}

void ioAccCriticalRateBuff::ProcessBuff( float fTimePerSec )
{
	CheckOwnerStateCheck();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioAccCriticalRateBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}
}

void ioAccCriticalRateBuff::EndBuff()
{
	m_pOwner->RemoveCriticalRate( m_fAddCriticalRate );
	ioBuff::EndBuff();
}