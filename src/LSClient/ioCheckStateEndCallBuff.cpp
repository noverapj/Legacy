
#include "stdafx.h"

#include "ioCheckStateEndCallBuff.h"

#include "WeaponDefine.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioCheckStateEndCallBuff::ioCheckStateEndCallBuff()
{
	m_BuffState = BS_WAIT;
}

ioCheckStateEndCallBuff::ioCheckStateEndCallBuff( const ioCheckStateEndCallBuff &rhs )
	: ioBuff( rhs ),
	  m_nCheckState( rhs.m_nCheckState ),
	  m_bSetWait( rhs.m_bSetWait ),
	  m_OwnerCallBuffList( rhs.m_OwnerCallBuffList )
{	
	m_BuffState = BS_WAIT;
}

ioCheckStateEndCallBuff::~ioCheckStateEndCallBuff()
{	
}

ioBuff* ioCheckStateEndCallBuff::Clone()
{
	return new ioCheckStateEndCallBuff( *this );
}

void ioCheckStateEndCallBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_nCheckState = rkLoader.LoadInt_e( "check_state", CS_DELAY );
	m_bSetWait = rkLoader.LoadBool_e( "set_wait", false );

	LoadOwnerCallBuffList( rkLoader );
}

void ioCheckStateEndCallBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_BuffState = BS_WAIT;
}

bool ioCheckStateEndCallBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_BuffState = BS_WAIT;

	return true;
}

void ioCheckStateEndCallBuff::ProcessBuff( float fTimePerSec )
{	
	ioBuff::ProcessBuff( fTimePerSec );
	
	switch( m_BuffState )
	{
	case BS_WAIT:
		CheckCharState( m_pOwner );
		break;
	case BS_END:
		SetReserveEndBuff();
		break;
	}
}

void ioCheckStateEndCallBuff::EndBuff()
{
	ioBuff::EndBuff();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckStateEndCallBuff::LoadOwnerCallBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_call_buff_cnt", 0 );
	m_OwnerCallBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerCallBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_call_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerCallBuffList.push_back( szBuf );
	}
}

void ioCheckStateEndCallBuff::SetOwnerCallBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( m_OwnerCallBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerCallBuffList.size();
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerCallBuffList[i];

		if( !szBuffName.IsEmpty() )
			//pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );	
			pOwner->ReserveAddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckStateEndCallBuff::CheckCharState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
		
	if ( m_bSetWait )
		return;

	if ( IsEnableCall( pOwner ) )
		SetCallState( pOwner );
	else
		SetReserveEndBuff();
}

bool ioCheckStateEndCallBuff::IsEnableCall( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();	
	if ( nOwnerState == m_nCheckState )
		return true;
	
	return false;
}

void ioCheckStateEndCallBuff::SetCallState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	m_BuffState = BS_END;
	
	SetOwnerCallBuffList( m_pOwner );
}

bool ioCheckStateEndCallBuff::IsWait()
{
	return m_bSetWait;
}