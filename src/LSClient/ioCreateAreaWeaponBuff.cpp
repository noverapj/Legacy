
#include "stdafx.h"

#include "ioCreateAreaWeaponBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioCreateAreaWeaponBuff::ioCreateAreaWeaponBuff()
{
}

ioCreateAreaWeaponBuff::ioCreateAreaWeaponBuff( const ioCreateAreaWeaponBuff &rhs )
	: ioBuff( rhs ),
	  m_szFindAreaWeapon( rhs.m_szFindAreaWeapon ),
	  m_szCreateAreaWeapon( rhs.m_szCreateAreaWeapon ),
	  m_dwCreateAreaWeaponTime( rhs.m_dwCreateAreaWeaponTime ),
	  m_bKeyControl( rhs.m_bKeyControl )
{
}

ioCreateAreaWeaponBuff::~ioCreateAreaWeaponBuff()
{
}

void ioCreateAreaWeaponBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "find_area_weapon", "", szBuf, MAX_PATH );
	m_szFindAreaWeapon = szBuf;

	rkLoader.LoadString_e( "create_area_weapon", "", szBuf, MAX_PATH );
	m_szCreateAreaWeapon = szBuf;

	m_dwCreateAreaWeaponTime = rkLoader.LoadInt_e( "create_area_weapon_time", 0 );

	m_bKeyControl = rkLoader.LoadBool_e( "enable_key_control", false );
}

ioBuff* ioCreateAreaWeaponBuff::Clone()
{
	return new ioCreateAreaWeaponBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCreateAreaWeaponBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioCreateAreaWeaponBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	return true;
}

void ioCreateAreaWeaponBuff::ProcessBuff( float fTimePerSec )
{
	if ( !m_pOwner )
	{
		SetReserveEndBuff();
		return;
	}

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

	if( m_bKeyControl )
	{
		if ( m_pOwner->IsNeedProcess() && m_pOwner->IsDefenseKey() )
			CreateAreaWeapon( m_pOwner );
	}
	else
	{
		if ( m_pOwner->IsNeedProcess() && m_dwBuffStartTime + m_dwCreateAreaWeaponTime < FRAMEGETTIME() )
			CreateAreaWeapon( m_pOwner );
	}
}

void ioCreateAreaWeaponBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioCreateAreaWeaponBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	SetReserveEndBuff();
	CreateAreaWeapon( pOwner );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCreateAreaWeaponBuff::CreateAreaWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	SetReserveEndBuff();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioAreaWeapon *pFindAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_szFindAreaWeapon, pOwner->GetCharName() );
	if ( pFindAreaWeapon )
	{
		g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_szCreateAreaWeapon, pFindAreaWeapon->GetStartPos(), pFindAreaWeapon->GetRotation(), pFindAreaWeapon->GetCreateType() );
		g_AreaWeaponMgr.DestroyAreaWeapon( pFindAreaWeapon->GetAreaWeaponIndex() );
	}
}