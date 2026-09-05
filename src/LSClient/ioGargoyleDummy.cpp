#include "StdAfx.h"
#include "ioGargoyleDummy.h"


ioGargoyleDummy::ioGargoyleDummy( ioEntityGroup *pGrp, ioPlayMode *pMode ): 
ioDummyChar( pGrp, pMode )
{
	m_GargoyleDummyState = GDS_START;
}

ioGargoyleDummy::~ioGargoyleDummy(void)
{
}

void ioGargoyleDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "gargoyle_move_ani", "", szBuf, MAX_PATH );
	m_szGargoyleMoveAni	= szBuf;
	m_fGargoyleMoveAniRate	= rkLoader.LoadFloat_e( "gargoyle_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gargoyle_charge_ani", "", szBuf, MAX_PATH );
	m_szGargoyleChargeAni	= szBuf;
	m_fGargoyleChargeAniRate	= rkLoader.LoadFloat_e( "gargoyle_charge_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gargoyle_charge_move_ani", "", szBuf, MAX_PATH );
	m_szGargoyleChargeMoveAni	= szBuf;
	m_fGargoyleChargeMoveAniRate	= rkLoader.LoadFloat_e( "gargoyle_charge_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gargoyle_attack_ani", "", szBuf, MAX_PATH );
	m_szGargoyleAttackAni	= szBuf;
	m_fGargoyleAttackAniRate	= rkLoader.LoadFloat_e( "gargoyle_attack_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gargoyle_charge_attack_ani", "", szBuf, MAX_PATH );
	m_szGargoyleChargeAttackAni	= szBuf;
	m_fGargoyleChargeAttackAniRate	= rkLoader.LoadFloat_e( "gargoyle_charge_attack_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gargoyle_jump_ani", "", szBuf, MAX_PATH );
	m_szGargoyleJumpAni	= szBuf;
	m_fGargoyleJumpAniRate	= rkLoader.LoadFloat_e( "gargoyle_jump_ani_rate", FLOAT1 );

	LoadWeaponInfo( "gargoyle_attack_info", m_GargoyleAttackInfo, rkLoader );
	LoadWeaponInfo( "gargoyle_charge_attack_info", m_GargoyleChargeAttackInfo, rkLoader );

	m_vGargoyleAttackOffset.x = rkLoader.LoadFloat_e( "gargoyle_attack_offset_x", 0.0f );
	m_vGargoyleAttackOffset.y = rkLoader.LoadFloat_e( "gargoyle_attack_offset_y", 0.0f );
	m_vGargoyleAttackOffset.z = rkLoader.LoadFloat_e( "gargoyle_attack_offset_z", 0.0f );

	m_vGargoyleChargeAttackOffset.x = rkLoader.LoadFloat_e( "gargoyle_charge_attack_offset_x", 0.0f );
	m_vGargoyleChargeAttackOffset.y = rkLoader.LoadFloat_e( "gargoyle_charge_attack_offset_y", 0.0f );
	m_vGargoyleChargeAttackOffset.z = rkLoader.LoadFloat_e( "gargoyle_charge_attack_offset_z", 0.0f );
}

void ioGargoyleDummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	switch( m_GargoyleDummyState )
	{
	case GDS_ATTACK:
		ProcessAttackState();
		break;
	case GDS_CHARGE_ATTACK:
		ProcessChargeAttackState();
		break;
	}
	ProcessPos( fTimerPerSec );
	ProcessEffectUpdate( fTimerPerSec );
}

void ioGargoyleDummy::ProcessPos( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	SetWorldPosition( pOwner->GetWorldPosition() );
	SetWorldOrientation( pOwner->GetTargetRot() );
}

void ioGargoyleDummy::SetLoopAni( const ioHashString& rkszAni, const float& rkfAniRate )
{
	if ( rkszAni.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( rkszAni );
	if( iAniID == -1 )
		return;

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, rkfAniRate );
}

DWORD ioGargoyleDummy::SetActionAni( const ioHashString& rkszAni, const float& rkfAniRate )
{
	if ( rkszAni.IsEmpty() )
		return 0;

	int iAniID = m_pGroup->GetAnimationIdx( rkszAni );
	if( iAniID == -1 )
		return 0;

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT1, rkfAniRate );

	m_dwFireStartTime = (DWORD)m_pGroup->GetAnimationEventTime_e( iAniID, "fire_s" ) * rkfAniRate;
	if ( m_dwFireStartTime > 0 )
		m_dwFireStartTime += FRAMEGETTIME();

	return m_pGroup->GetAnimationFullTime( iAniID ) * rkfAniRate;
}

void ioGargoyleDummy::SetGargoyleMoveState( ioBaseChar *pOwner )
{
	m_GargoyleDummyState = GDS_MOVE;
	SetLoopAni( m_szGargoyleMoveAni, m_fGargoyleMoveAniRate );
}

void ioGargoyleDummy::SetGargoyleChargeState( ioBaseChar *pOwner )
{
	m_GargoyleDummyState = GDS_CHARGE;
	SetLoopAni( m_szGargoyleChargeAni, m_fGargoyleChargeAniRate );
}

void ioGargoyleDummy::SetGargoyleChargeMoveState( ioBaseChar *pOwner )
{
	m_GargoyleDummyState = GDS_CHARGE_MOVE;
	SetLoopAni( m_szGargoyleChargeMoveAni, m_fGargoyleMoveAniRate );
}

DWORD ioGargoyleDummy::SetGargoyleAttackState( ioBaseChar *pOwner )
{
	m_GargoyleDummyState = GDS_ATTACK;
	return SetActionAni( m_szGargoyleAttackAni, m_fGargoyleAttackAniRate );
}

DWORD ioGargoyleDummy::SetGargoyleChargeAttackState( ioBaseChar *pOwner, const ioHashString& szTarget )
{
	m_szTargetName = szTarget;
	m_GargoyleDummyState = GDS_CHARGE_ATTACK;
	return SetActionAni( m_szGargoyleChargeAttackAni, m_fGargoyleChargeAttackAniRate );
}

DWORD ioGargoyleDummy::SetGargoyleJumpState( ioBaseChar *pOwner )
{
	m_GargoyleDummyState = GDS_JUMP;
	return SetActionAni( m_szGargoyleJumpAni, m_fGargoyleJumpAniRate );
}

void ioGargoyleDummy::ProcessAttackState()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	if ( m_dwFireStartTime && m_dwFireStartTime < FRAMEGETTIME() )
	{
		m_dwFireStartTime = 0;
		D3DXVECTOR3 vPosOffset = GetWorldOrientation() * m_vGargoyleAttackOffset;
		D3DXVECTOR3 vCurPos = GetMidPositionByRate() + vPosOffset;

		
		D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vCurDir, &vCurDir );
		FireTime kFireTime;

		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.iAttributeIdx  = m_GargoyleAttackInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = m_GargoyleAttackInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni   = m_GargoyleAttackInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration  = m_GargoyleAttackInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = m_GargoyleAttackInfo.m_bWoundedLoopAni;
		ioWeapon* pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
	}
}

void ioGargoyleDummy::ProcessChargeAttackState()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	if ( m_dwFireStartTime && m_dwFireStartTime < FRAMEGETTIME() )
	{
		m_dwFireStartTime = 0;
		D3DXVECTOR3 vPosOffset = GetWorldOrientation() * m_vGargoyleChargeAttackOffset;
		D3DXVECTOR3 vCurPos = GetMidPositionByRate() + vPosOffset;
		D3DXVECTOR3 vCurDir = ioMath::VEC3_ZERO;
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
		if ( !pTarget )
			vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		else
			vCurDir = pTarget->GetWorldPosition() - GetWorldPosition();
		D3DXVec3Normalize( &vCurDir, &vCurDir );

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.iAttributeIdx  = m_GargoyleChargeAttackInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = m_GargoyleChargeAttackInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni   = m_GargoyleChargeAttackInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration  = m_GargoyleChargeAttackInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = m_GargoyleChargeAttackInfo.m_bWoundedLoopAni;
		ioWeapon* pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
	}
}
