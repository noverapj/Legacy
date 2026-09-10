#include "StdAfx.h"
#include "ioAccCriticalDamageRateBuff.h"

ioAccCriticalDamageRateBuff::ioAccCriticalDamageRateBuff(void)
{
	m_fAddCriticalDamageRate = 0.0f;
}

ioAccCriticalDamageRateBuff::ioAccCriticalDamageRateBuff( const ioAccCriticalDamageRateBuff &rhs ):
ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState )
{
	m_fAddCriticalDamageRate = 0.0f;
}

ioAccCriticalDamageRateBuff::~ioAccCriticalDamageRateBuff(void)
{
}

ioBuff* ioAccCriticalDamageRateBuff::Clone()
{
	return new ioAccCriticalDamageRateBuff( *this );
}

void ioAccCriticalDamageRateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

void ioAccCriticalDamageRateBuff::StartBuff( ioBaseChar *pOwner )
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

	m_fAddCriticalDamageRate = fNum;
	pOwner->AddCriticalDamageRate( m_fAddCriticalDamageRate );
}

bool ioAccCriticalDamageRateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

	m_fAddCriticalDamageRate = fNum;
	pOwner->AddCriticalDamageRate( m_fAddCriticalDamageRate );
	return true;
}

void ioAccCriticalDamageRateBuff::ProcessBuff( float fTimePerSec )
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

void ioAccCriticalDamageRateBuff::CheckOwnerStateCheck()
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

void ioAccCriticalDamageRateBuff::EndBuff()
{
	m_pOwner->RemoveCriticalDamageRate( m_fAddCriticalDamageRate );
	ioBuff::EndBuff();
}
