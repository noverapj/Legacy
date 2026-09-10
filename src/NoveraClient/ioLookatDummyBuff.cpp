

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioLookatDummyBuff.h"

ioLookatDummyBuff::ioLookatDummyBuff()
{
}

ioLookatDummyBuff::ioLookatDummyBuff( const ioLookatDummyBuff &rhs )
	: ioBuff( rhs ),
	  m_szDummyName( rhs.m_szDummyName )
{
}

ioLookatDummyBuff::~ioLookatDummyBuff()
{
}

ioBuff* ioLookatDummyBuff::Clone()
{
	return new ioLookatDummyBuff( *this );
}

void ioLookatDummyBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "look_dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;
}

void ioLookatDummyBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioLookatDummyBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
	{
		return false;
	}

	return true;
}

void ioLookatDummyBuff::ProcessBuff( float fTimePerSec )
{	
	if ( !m_pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if ( pCreator )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pCreator, m_szDummyName );
		if ( pDummy )
		{
			LookAtDummyPos( pDummy->GetWorldPosition() );
		}
	}

	SetReserveEndBuff();
	return;
}

void ioLookatDummyBuff::EndBuff()
{	
	ioBuff::EndBuff();
}

void ioLookatDummyBuff::LookAtDummyPos( D3DXVECTOR3 vPos )
{
	D3DXQUATERNION qtTargetRot;
	D3DXVECTOR3 vTargetDir = m_pOwner->GetWorldPosition() - vPos;
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vXZDir( vTargetDir.x, 0.0f, vTargetDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	ioMath::CalcDirectionQuaternion( qtTargetRot, vXZDir );

	m_pOwner->SetTargetRot( qtTargetRot );
}