
#include "stdafx.h"

#include "ioDummyCheckBuff.h"

#include "ioBaseChar.h"

ioDummyCheckBuff::ioDummyCheckBuff()
{
}

ioDummyCheckBuff::ioDummyCheckBuff( const ioDummyCheckBuff &rhs )
	: ioBuff( rhs ),
	  m_bEnableOutLine( rhs.m_bEnableOutLine ),
	  m_fOutLineR( rhs.m_fOutLineR ),
	  m_fOutLineG( rhs.m_fOutLineG ),
	  m_fOutLineB( rhs.m_fOutLineB ),
	  m_fOutLineAlpha( rhs.m_fOutLineAlpha ),
	  m_fOutLineThickness( rhs.m_fOutLineThickness ),
	  m_szDummyName( rhs.m_szDummyName )
{
}

ioDummyCheckBuff::~ioDummyCheckBuff()
{
}

void ioDummyCheckBuff::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	ioBuff::LoadProperty( rkLoader );
	
	m_bEnableOutLine = rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR = rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG = rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB = rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha = rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness = rkLoader.LoadFloat_e( "outline_thickness", 2.0f );

	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;
}

ioBuff* ioDummyCheckBuff::Clone()
{
	return new ioDummyCheckBuff( *this );
}

void ioDummyCheckBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	
	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}
}

bool ioDummyCheckBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	return true;
}

void ioDummyCheckBuff::ProcessBuff( float fTimePerSec )
{	
	if ( !CheckLiveState( GetOwner() ) )
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
}

void ioDummyCheckBuff::EndBuff()
{
	ioBuff::EndBuff();
}

bool ioDummyCheckBuff::CheckLiveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szDummyName );
	if ( !pDummy )
		return false;

	return true;
}