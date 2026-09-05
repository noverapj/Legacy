

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioReflectBuff.h"

ioReflectBuff::ioReflectBuff()
{
}

ioReflectBuff::ioReflectBuff( const ioReflectBuff &rhs )
: ioBuff( rhs ),
m_ReflectInfo( rhs.m_ReflectInfo ),
m_bDisableDownState( rhs.m_bDisableDownState )
{
}

ioReflectBuff::~ioReflectBuff()
{
}

void ioReflectBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_ReflectInfo.m_fReflectPowerRate = rkLoader.LoadFloat_e( "reflect_rate", FLOAT1 );
	m_ReflectInfo.m_fReflectMinPower = rkLoader.LoadFloat_e( "reflect_min_power", 0.0f );
	m_ReflectInfo.m_fReflectMaxPower = rkLoader.LoadFloat_e( "reflect_max_power", 0.0f );
	m_ReflectInfo.m_fDecreaseDamageRate = rkLoader.LoadFloat_e( "damage_rate", FLOAT1 );

	m_ReflectInfo.m_ReflectSideType = (AttackSide)rkLoader.LoadInt_e( "reflect_side", AS_NONE );
	m_ReflectInfo.m_bAirReflect = rkLoader.LoadBool_e( "reflect_enable_float", false );
	m_ReflectInfo.m_bStateProtectIgnoreReflect = rkLoader.LoadBool_e( "ignore_state_protect", false );
	m_ReflectInfo.m_bEnableEscapeCatchAttackReflect = rkLoader.LoadBool_e( "enable_escape_catch_attack_reflect", false );
	m_ReflectInfo.m_bIgnoreOtherReflect = rkLoader.LoadBool_e( "ignore_other_reflect_buff", false );
	m_ReflectInfo.m_fAirReflectRate = rkLoader.LoadFloat_e( "air_reflect_rate", FLOAT1 );

	char szBuf[MAX_PATH] = "";
	
	rkLoader.LoadString_e( "reflect_effect", "", szBuf, MAX_PATH );
	m_ReflectInfo.m_ReflectEffect = szBuf;

	rkLoader.LoadString_e( "reflect_ani_name", "", szBuf, MAX_PATH );
	m_ReflectInfo.m_ReflectAni = szBuf;
	m_ReflectInfo.m_fReflectAniRate = rkLoader.LoadFloat_e( "reflect_ani_rate", FLOAT1 );

	m_ReflectInfo.m_Name = GetName();

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioReflectBuff::Clone()
{
	return new ioReflectBuff( *this );
}

void ioReflectBuff::StartBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioBuff::StartBuff( pOwner );

	ioItem *pOwnerItem = pOwner->GetEquipedItem( m_ItemName );

	if( pOwnerItem )
	{
		m_ReflectInfo.m_iSlot = pOwnerItem->GetType()-1;
	}

	/*if( m_pOwnerItem )
	{
		m_ReflectInfo.m_iSlot = m_pOwnerItem->GetType()-1;
	}*/

	pOwner->AddReflectInfo( m_ReflectInfo );
}

bool ioReflectBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !pOwner )
		return false;

	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGlobalTime ) )
		return false;

	ioItem *pOwnerItem = pOwner->GetEquipedItem( m_ItemName );
	if( pOwnerItem )
	{
		m_ReflectInfo.m_iSlot = pOwnerItem->GetType()-1;
	}

	/*if( m_pOwnerItem )
	{
		m_ReflectInfo.m_iSlot = m_pOwnerItem->GetType()-1;
	}*/

	pOwner->AddReflectInfo( m_ReflectInfo );
	return true;
}

void ioReflectBuff::ProcessBuff( float fTimePerSec )
{
	if( m_iOperationType != OT_PASSIVE )
	{
		CheckOwnerStateCheck();
	}	

	switch( m_iOperationType )
	{
	case OT_SWITCH:
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

void ioReflectBuff::EndBuff()
{
	m_pOwner->RemoveReflectInfo( GetName() );
	ioBuff::EndBuff();
}

void ioReflectBuff::CheckOwnerStateCheck()
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

