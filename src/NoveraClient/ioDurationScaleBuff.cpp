

#include "stdafx.h"

#include "ioDurationScaleBuff.h"

#include "ioBaseChar.h"

ioDurationScaleBuff::ioDurationScaleBuff()
{
	m_iGrade = 0;
	m_fScaleValue = 0.0f;
	m_fScaleBalance = 0.0f;
}

ioDurationScaleBuff::ioDurationScaleBuff( const ioDurationScaleBuff &rhs )
: ioBuff( rhs )
{
	m_iGrade = rhs.m_iGrade;
	m_fScaleValue = rhs.m_fScaleValue;
	m_fScaleBalance = rhs.m_fScaleBalance;
}

ioDurationScaleBuff::~ioDurationScaleBuff()
{
}

void ioDurationScaleBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_iGrade = rkLoader.LoadInt_e( "scale_grade", 20 );
	m_fScaleValue = rkLoader.LoadFloat_e( "scale_increase", FLOAT1 );
	m_fScaleBalance = rkLoader.LoadFloat_e( "scale_balance", FLOAT1 );
}

ioBuff* ioDurationScaleBuff::Clone()
{
	return new ioDurationScaleBuff( *this );
}

void ioDurationScaleBuff::ProcessBuff( float fTimePerSec )
{
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

float ioDurationScaleBuff::GetScaleByPlayLevel( int iPlayLevel )
{
	float fScale = ( ( iPlayLevel - 1 ) * m_fScaleValue ) / 
		( ( iPlayLevel - 1 ) + m_fScaleValue );
	fScale /= m_fScaleBalance;
	
	return fScale;
}

void ioDurationScaleBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	if(m_pOwner->IsHasCrown()) return;

	float fScale = FLOAT1 + GetScaleByPlayLevel( m_iGrade );
	fScale = max( fScale, FLOAT1 );
	m_pOwner->SetWorldScale( D3DXVECTOR3(fScale, fScale, fScale) );
}

bool ioDurationScaleBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime) )
		return false;

	if( m_pOwner->IsHasCrown() )
		return false;

	float fScale = FLOAT1 + GetScaleByPlayLevel( m_iGrade );
	fScale = max( fScale, FLOAT1 );
	m_pOwner->SetWorldScale( D3DXVECTOR3(fScale, fScale, fScale) );

	return true;
}

void ioDurationScaleBuff::EndBuff()
{
	if(!m_pOwner) return;
	if(m_pOwner->IsHasCrown()) return;

	float fScale = FLOAT1 + m_pOwner->GetScaleByPlayLevel( 1 );
	fScale = max( fScale, FLOAT1 );

	D3DXVECTOR3 vCurScale = m_pOwner->GetWorldScale();
	if( vCurScale.y == 0.1f )
		m_pOwner->SetWorldScale( D3DXVECTOR3(fScale, 0.1f, fScale) );
	else
		m_pOwner->SetWorldScale( D3DXVECTOR3(fScale, fScale, fScale) );

	ioBuff::EndBuff();
}

