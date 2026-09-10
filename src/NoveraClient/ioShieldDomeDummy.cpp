#include "StdAfx.h"

#include "ioShieldDomeDummy.h"
#include "WeaponDefine.h"
#include "BuffDefine.h"


ioShieldDomeDummy::ioShieldDomeDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_pAreaWeapon	= NULL;
}


ioShieldDomeDummy::~ioShieldDomeDummy(void)
{
}

void	ioShieldDomeDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bIsMoving	= rkLoader.LoadBool( "moving_shield", false );

	m_ShieldWeaponInfo.m_iWeaponIdx	= rkLoader.LoadInt_e( "shield_weapon_index", 0 );
	m_ShieldWeaponInfo.m_iResistanceIdx	= rkLoader.LoadInt_e( "shield_weapon_resistance", 0 );

	rkLoader.LoadString_e( "shield_weapon_wound_ani", "", szBuf, MAX_PATH );
	m_ShieldWeaponInfo.m_WoundedAnimation	= szBuf;

	m_ShieldWeaponInfo.m_dwWoundedDuration	= rkLoader.LoadInt_e( "shield__weapon_wound_duration", 0 );
	m_ShieldWeaponInfo.m_bWoundedLoopAni	= rkLoader.LoadBool_e( "shield_weapon_wound_loop_ani", false );

	rkLoader.LoadString_e( "shield_area_weapon_name", "", szBuf, MAX_PATH );
	m_szAreaWeaponName	= szBuf;

	m_vCreateWeaponOffset.x	= rkLoader.LoadFloat( "shield_area_weapon_offset_x", 0.0f );
	m_vCreateWeaponOffset.y	= rkLoader.LoadFloat( "shield_area_weapon_offset_y", 0.0f );
	m_vCreateWeaponOffset.z	= rkLoader.LoadFloat( "shield_area_weapon_offset_z", 0.0f );
}

void	ioShieldDomeDummy::ProcessStartState( float fTimePerSec )
{
	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		ioBaseChar*	pOwner	= GetOwner();
		if( !pOwner )
			return;

		DWORD	dwWeaponBaseIndex	= pOwner->IncreaseWeaponIndexBase();

		D3DXVECTOR3	vPos	= GetMidPositionByRate();
		D3DXVECTOR3	vDir	= GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		FireTime kFireTime;
		kFireTime.dwWeaponIdx	= dwWeaponBaseIndex++;
		kFireTime.dwStart	= FRAMEGETTIME();
		kFireTime.eFireTimeType	= FTT_DUMMY_CHAR_ATTACK;
		kFireTime.m_iExtraValue	= m_iCreateIndex;
		kFireTime.iAttributeIdx	= m_ShieldWeaponInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx	= m_ShieldWeaponInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni	= m_ShieldWeaponInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration	= m_ShieldWeaponInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni	= m_ShieldWeaponInfo.m_bWoundedLoopAni;
		ioWeapon* pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		/*ioPrisonWeapon* pPrisonWeapon	= ToPrisonWeapon( pWeapon );
		if( pPrisonWeapon )
			pPrisonWeapon->SetPrisionPos( vPos );*/
		//CreateAreaWeapon();

		SetDelayState( false );
		return;
	}
}

void	ioShieldDomeDummy::ProcessPos( float fTimePerSec )
{
	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime	= dwCurTime;
		return;
	}

	ProcessForceMove( fTimePerSec );

	ioBaseChar*	pOwner	= GetOwner();
	if( !pOwner )
		return;

	ioPlayStage*	pStage	= pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_bIsMoving )
	{
		SetWorldPosition( pOwner->GetWorldPosition() );
		SetWorldOrientation( pOwner->GetTargetRot() );
		//m_pAreaWeapon->SetAreaPosition( pOwner->GetWorldPosition(), pStage );
	}
}

void	ioShieldDomeDummy::CreateAreaWeapon()
{
	if ( m_szAreaWeaponName.IsEmpty() )
		return;

	ioBaseChar*	pOwner	= GetOwner();
	if ( !pOwner )
		return;

	//각도, 위치
	D3DXVECTOR3	vPos	= GetWorldPosition() + m_vCreateWeaponOffset;
	D3DXQUATERNION	qtRot	= GetWorldOrientation();

	//Area 무기 생성
	m_pAreaWeapon	= g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
		"",
		"",
		m_szAreaWeaponName,
		vPos,
		qtRot,
		ioAreaWeapon::CT_NORMAL );
}