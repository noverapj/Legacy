

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioCreateWeaponBuff2.h"

ioCreateWeaponBuff2::ioCreateWeaponBuff2()
{
	m_dwWeaponDeadIndex = 0;
}

ioCreateWeaponBuff2::ioCreateWeaponBuff2( const ioCreateWeaponBuff2 &rhs )
	: ioBuff( rhs ),
	m_bDisableDownState( rhs.m_bDisableDownState ),
	m_bDisableDropDownState( rhs.m_bDisableDropDownState ),
	m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
	m_bEscapeCatchState( rhs.m_bEscapeCatchState ),
	m_bEnableOutLine( rhs.m_bEnableOutLine ),
	m_fOutLineR( rhs.m_fOutLineR ),
	m_fOutLineG( rhs.m_fOutLineG ),
	m_fOutLineB( rhs.m_fOutLineB ),
	m_fOutLineAlpha( rhs.m_fOutLineAlpha ),
	m_fOutLineThickness( rhs.m_fOutLineThickness ),
	m_WeaponInfoList( rhs.m_WeaponInfoList ),
	m_fFireHeightOffSet( rhs.m_fFireHeightOffSet )
{
	m_dwWeaponDeadIndex = 0;
}

ioCreateWeaponBuff2::~ioCreateWeaponBuff2()
{
}

void ioCreateWeaponBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropDownState = rkLoader.LoadBool_e( "disable_drop_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_bEscapeCatchState = rkLoader.LoadBool_e( "escape_catch_state", false );

	m_bEnableOutLine = rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR = rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG = rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB = rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha = rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness = rkLoader.LoadFloat_e( "outline_thickness", 2.0f );
	//////////////////////////////////////////////////////////////////////////
	m_fFireHeightOffSet = rkLoader.LoadFloat_e( "fire_offset_height", 0.f );

	int nMaxFireCnt = rkLoader.LoadInt_e( "max_fire_cnt", 0 );
	m_WeaponInfoList.reserve( nMaxFireCnt );


	for( int i=0 ; i<nMaxFireCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "fire_weapon_index_%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "fire_weapon_index_resistance_%d", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "fire_weapon_time_%d", i+1 );
		kInfo.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "fire_weapon_wound_ani_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "fire_weapon_wound_duration_%d", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "fire_weapon_wound_loop_ani_%d", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_WeaponInfoList.push_back( kInfo );
	}	
}

ioBuff* ioCreateWeaponBuff2::Clone()
{
	return new ioCreateWeaponBuff2( *this );
}

void ioCreateWeaponBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}

	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	m_BuffState = BS_NONE;
}

bool ioCreateWeaponBuff2::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}

	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	return true;
}

void ioCreateWeaponBuff2::StartFireCheckTime()
{
	m_BuffState = BS_FIRE;
	m_dwFireStartTime = FRAMEGETTIME();

	m_nFireIndex = 0;
	if( m_WeaponInfoList.size() )
	{
		m_dwNextFireTime = m_dwFireStartTime + m_WeaponInfoList[0].m_dwCallTime;
	}
	else
	{
		m_BuffState = BS_END;
		SetReserveEndBuff();
	}
	
}

void ioCreateWeaponBuff2::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pCreator = GetCreator();

	if( !pCreator )
	{
		SetReserveEndBuff();
		return;
	}

	if( pCreator->GetState() == CS_DIE )
	{
		SetReserveEndBuff();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	CheckCallingProcess();

	if( GetBuffDuration() == 0 )
	{		
		return;
	}

	if( m_dwBuffEndTime <= FRAMEGETTIME() )
	{		
		SetReserveEndBuff();
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_BuffState )
	{
	case BS_NONE:
		break;
	case BS_FIRE:
		if( m_nFireIndex >= (int)m_WeaponInfoList.size() )
		{
			SetReserveEndBuff();
			m_BuffState = BS_END;
			return;
		}

		if( m_dwNextFireTime <= dwCurTime )
		{
			CreateWeapon();
			m_nFireIndex++;
			if( m_nFireIndex < (int)m_WeaponInfoList.size() )
			{
				m_dwNextFireTime = dwCurTime + m_WeaponInfoList[m_nFireIndex].m_dwCallTime;
			}
			else
			{
				SetReserveEndBuff();
				m_BuffState = BS_END;
				return;
			}
		}
		break;
	case BS_WAIT:
		break;
	case BS_END:
		SetReserveEndBuff();
		break;
	default: SetReserveEndBuff(); break;
	}

}


void ioCreateWeaponBuff2::CreateWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = GetCreator();
	if( !pCreator )
		return;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	vPos.y += m_fFireHeightOffSet;
	DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase();
	ioWeapon* pWeapon = pCreator->DummyExplicit( m_WeaponInfoList[m_nFireIndex],
		vPos,
		dwWeaponIndex,
		false,
		pOwner->GetCharName(),
		m_Name,
		m_bUseCallAttWeaponCol );

	if( pWeapon )
	{
		pWeapon->SetPosition( vPos );
		pWeapon->SetMoveDir( D3DXVECTOR3(0.f, -FLOAT1, 0.f) );
	}
}


void ioCreateWeaponBuff2::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	if( m_bEnableOutLine )
	{
		m_pOwner->RestoreOutLineChange( COT_BUFF_PROTECTION );
	}

	ioBuff::EndBuff();

	if( 0 < m_dwWeaponDeadIndex )
	{
		ioBaseChar *pOwner = GetOwner();
		if( !pOwner )
			return;

		ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
		if( !pCreator )
			return;

		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwWeaponDeadIndex );
		if( pWeapon )	
			pWeapon->SetWeaponDead();
	}
}