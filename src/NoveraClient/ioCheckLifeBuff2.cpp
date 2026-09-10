
#include "stdafx.h"

#include "ioCheckLifeBuff2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioCheckLifeBuff2::ioCheckLifeBuff2()
{
	m_bSetCreaterBuff = false;
}

ioCheckLifeBuff2::ioCheckLifeBuff2( const ioCheckLifeBuff2 &rhs )
	: ioBuff( rhs ),
      m_szCheckBuff( rhs.m_szCheckBuff ),
	  m_fCheckDist( rhs.m_fCheckDist ),
	  m_szCreaterBuff( rhs.m_szCreaterBuff )
{
	m_bSetCreaterBuff = false;
}

ioCheckLifeBuff2::~ioCheckLifeBuff2()
{
}

void ioCheckLifeBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	m_fCheckDist = rkLoader.LoadFloat_e( "check_dist", 0.0f );

	rkLoader.LoadString_e( "creater_buff", "", szBuf, MAX_PATH );
	m_szCreaterBuff = szBuf;
}

ioBuff* ioCheckLifeBuff2::Clone()
{
	return new ioCheckLifeBuff2( *this );
}

void ioCheckLifeBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bSetCreaterBuff = false;

	ioPlayStage *pStage = m_pOwner->GetCreator();
	if ( !pStage )
		return;

	const BaseCharList &pBaseCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = pBaseCharList.begin(); iter != pBaseCharList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) 
			continue;

		if ( pChar->HasBuff( m_CreateChar, m_Name ) )
		{
			SetReserveEndBuff();
			return;
		}
	}

	ioBaseChar *pCreater = GetCreator();
	if ( pCreater )
	{
		//pCreater->AddNewBuff( m_szCreaterBuff, "", "", NULL );
		pCreater->ReserveAddNewBuff( m_szCreaterBuff, "", "", NULL );
		m_bSetCreaterBuff = true;
	}
}

bool ioCheckLifeBuff2::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bSetCreaterBuff = false;

	return true;
}

void ioCheckLifeBuff2::ProcessBuff( float fTimePerSec )
{
	if ( IsCheckBuff() )
	{
		SetReserveEndBuff();
		return;
	}

	ioBuff::ProcessBuff( fTimePerSec );
}

void ioCheckLifeBuff2::EndBuff()
{
	ioBuff::EndBuff();

	if ( m_bSetCreaterBuff )
	{
		ioBaseChar *pCreater = GetCreator();
		if ( pCreater )
			pCreater->RemoveBuff( m_szCreaterBuff, true );
	}
}

bool ioCheckLifeBuff2::IsCheckBuff()
{
	ioBaseChar *pCreater = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreater || !pCreater->HasBuff( m_szCheckBuff ) )
		return true;
	
	if ( !m_pOwner )
		return true;
	
	if ( m_pOwner->HasHideBuff() )
		return true;
	
	D3DXVECTOR3 vDistance = pCreater->GetWorldPosition() - m_pOwner->GetWorldPosition();
	float fDist = D3DXVec3LengthSq( &vDistance );
	if ( (m_fCheckDist * m_fCheckDist) <= fDist )
		return true;
	
	return false;
}