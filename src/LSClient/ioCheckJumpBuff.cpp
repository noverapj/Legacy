
#include "stdafx.h"

#include "ioCheckJumpBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioCheckJumpBuff::ioCheckJumpBuff()
{
}

ioCheckJumpBuff::ioCheckJumpBuff( const ioCheckJumpBuff &rhs )
	: ioBuff( rhs ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_nMaxCnt( rhs.m_nMaxCnt )
{
}

ioCheckJumpBuff::~ioCheckJumpBuff()
{
}

void ioCheckJumpBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	
	m_nMaxCnt = rkLoader.LoadInt_e( "max_buff_cnt", 0 );
	LoadOwnerBuffList( rkLoader );
}

ioBuff* ioCheckJumpBuff::Clone()
{
	return new ioCheckJumpBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckJumpBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioCheckJumpBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	return true;
}

void ioCheckJumpBuff::ProcessBuff( float fTimePerSec )
{
	if ( m_pOwner && m_pOwner->GetState() == CS_JUMP )
	{
		if ( CheckBuff( m_pOwner ) )
		{
			SetOwnerBuffList( m_pOwner );
			SetReserveEndBuff();
			return;
		}		
	}
}

void ioCheckJumpBuff::EndBuff()
{
	ioBuff::EndBuff();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//버프 리스트 읽기
void ioCheckJumpBuff::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioCheckJumpBuff::SetOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();

	//해당 버프를 생성
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i];

		if( !szBuffName.IsEmpty() )
			//pOwner->AddNewBuff( szBuffName, m_CreateChar, "", NULL );	
			pOwner->ReserveAddNewBuff( szBuffName, m_CreateChar, "", NULL );
	}
}

bool ioCheckJumpBuff::CheckBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	int nBuffCnt = 0;
	const BaseCharList &pBaseCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = pBaseCharList.begin(); iter != pBaseCharList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) 
			continue;

		for( int j=0; j<(int)m_OwnerBuffList.size(); ++j )
		{
			ioHashString szBuffName = m_OwnerBuffList[j];
			if( !szBuffName.IsEmpty() )
			{
				if ( pChar->HasBuff( m_CreateChar, szBuffName ) )	
					nBuffCnt++;
			}
		}
	}

	if ( nBuffCnt < m_nMaxCnt )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////