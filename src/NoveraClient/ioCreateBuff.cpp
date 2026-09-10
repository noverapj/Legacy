

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioCreateBuff.h"


ioCreateBuff::ioCreateBuff()
{
}

ioCreateBuff::ioCreateBuff( const ioCreateBuff &rhs )
	: ioBuff( rhs ),
	m_vBuffList( rhs.m_vBuffList )
{
}

ioCreateBuff::~ioCreateBuff()
{
}

void ioCreateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	int nBuffCnt = rkLoader.LoadInt_e( "BuffCnt", 0 );
	m_vBuffList.reserve( nBuffCnt );
	for( int i = 0; i < nBuffCnt; i++ )
	{
		sprintf_e( szKey, "ADD_TO_CREATOR%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString stBuffName = szBuf;
		m_vBuffList.push_back( stBuffName );
	}
}

ioBuff* ioCreateBuff::Clone()
{
	return new ioCreateBuff( *this );
}

void ioCreateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

}

bool ioCreateBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioCreateBuff::ProcessBuff( float fTimePerSec )
{
	CreateBuffForCreator();

	SetReserveEndBuff();
}


bool ioCreateBuff::CreateBuffForCreator()
{
	if( !m_pOwner ) 
		return false;

	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( pCreator )
	{
		if( pCreator->GetState() == CS_DIE || pCreator->GetState() == CS_VIEW ||
			pCreator->GetState() == CS_LOADING || pCreator->GetState() == CS_OBSERVER )
			return false;

		for( int i = 0; i < (int)m_vBuffList.size(); i++ )
		{
			ioHashString stBuffName = m_vBuffList[i];
			pCreator->AddNewBuff( stBuffName, m_CreateChar, "", NULL );
			//pCreator->ReserveAddNewBuff( stBuffName, m_CreateChar, "", NULL );
		}

		return true;
	}

	return false;
}

void ioCreateBuff::EndBuff()
{
	ioBuff::EndBuff();
}