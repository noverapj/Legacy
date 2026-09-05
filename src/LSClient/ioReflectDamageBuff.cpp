
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioReflectDamageBuff.h"

ioReflectDamageBuff::ioReflectDamageBuff()
{
}

ioReflectDamageBuff::ioReflectDamageBuff( const ioReflectDamageBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_ReflectDamageInfoVec( rhs.m_ReflectDamageInfoVec ),
m_bAirReflect( rhs.m_bAirReflect ),
m_bEnableEscapeCatchAttackReflect( rhs.m_bEnableEscapeCatchAttackReflect )
{
}

ioReflectDamageBuff::~ioReflectDamageBuff()
{
}

void ioReflectDamageBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );	
	
	m_ReflectDamageInfoVec.clear();
	int iCnt = rkLoader.LoadInt_e( "reflect_attack_type_cnt", 0 );	
		
	if( iCnt > 0 )
		m_ReflectDamageInfoVec.reserve( iCnt );

	for( int i = 0; i < iCnt; ++i )
	{
		ReflectDamageInfo ReflectAttackInfo;
		ReflectAttackInfo.m_WeaponInfo.Init();
		
		wsprintf_e( szKey, "reflect_attack%d_type", i+1  );
		ReflectAttackInfo.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "reflect_attack%d_type_resist", i+1 );
		ReflectAttackInfo.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "reflect_attack%d_type_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ReflectAttackInfo.m_WeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "reflect_attack%d_type_wounded_loop_ani", i+1 );
		ReflectAttackInfo.m_WeaponInfo.m_bWoundedLoopAni  = rkLoader.LoadBool( szKey, false ); 
		ReflectAttackInfo.m_WeaponInfo.m_dwCallTime       = 0;

		wsprintf_e( szKey, "reflect_attack%d_type_min_damage", i+1 );
		ReflectAttackInfo.m_fRangeMinDamage = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "reflect_attack%d_type_max_damage", i+1 );
		ReflectAttackInfo.m_fRangeMaxDamage = rkLoader.LoadFloat( szKey, 0.0f );

		m_ReflectDamageInfoVec.push_back( ReflectAttackInfo );
	}
	
	m_bAirReflect					  = rkLoader.LoadBool_e( "reflect_enable_float", false );
	m_bEnableEscapeCatchAttackReflect = rkLoader.LoadBool_e( "enable_escape_catch_attack_reflect", false );	
}

ioBuff* ioReflectDamageBuff::Clone()
{
	return new ioReflectDamageBuff( *this );
}

void ioReflectDamageBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );	
}

bool ioReflectDamageBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGlobalTime ) )
		return false;

	return true;
}

void ioReflectDamageBuff::ProcessBuff( float fTimePerSec )
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

void ioReflectDamageBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioReflectDamageBuff::CheckOwnerStateCheck()
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

const WeaponInfo& ioReflectDamageBuff::GetReflectAttack( float fDamage )
{
	static WeaponInfo KNone;
	ReflectDamageInfoVec::const_iterator iter = m_ReflectDamageInfoVec.begin();
	for( ; iter != m_ReflectDamageInfoVec.end(); ++iter )
	{
		const ReflectDamageInfo& rkInfo = *iter;
		if( rkInfo.m_fRangeMinDamage <= fDamage && fDamage <= rkInfo.m_fRangeMaxDamage )
			return rkInfo.m_WeaponInfo;		
	}

	return KNone;
}

