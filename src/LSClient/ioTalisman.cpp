

#include "stdafx.h"

#include "ioTalisman.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioTalisman::ioTalisman()
{
	m_dwEndTime = 0;
	m_dwTalismanEffect = -1;
}

ioTalisman::ioTalisman( const ioTalisman &rhs )
: m_iTalismanNum( rhs.m_iTalismanNum ),
  m_TalismanEffect( rhs.m_TalismanEffect ),
  m_dwDuration( rhs.m_dwDuration ),
  m_vTargetBuffList( rhs.m_vTargetBuffList )
{
	m_dwEndTime = 0;
	m_dwTalismanEffect = -1;
}

ioTalisman::~ioTalisman()
{
}

ioTalisman* ioTalisman::Clone()
{
	return new ioTalisman( *this );
}

void ioTalisman::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	m_iTalismanNum = rkLoader.LoadInt_e( "talisman_num", 0 );

	rkLoader.LoadString_e( "talisman_effect", "", szBuf, MAX_PATH );
	m_TalismanEffect = szBuf;

	m_dwDuration = rkLoader.LoadInt_e( "talisman_duration", 0 );
	m_vTargetBuffList.clear();

	int iBuffCnt = rkLoader.LoadInt_e( "talisman_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		char szKey[MAX_PATH] = "";
		wsprintf_e( szKey, "talisman_buff%d", i+1 );

		ioHashString szBuffName;
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szBuffName = szBuf;

		m_vTargetBuffList.push_back( szBuffName );
	}
}

void ioTalisman::InitTalisman( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	
	m_OwnerName = pOwner->GetCharName();
	m_dwEndTime = FRAMEGETTIME() + m_dwDuration;

	if( m_TalismanEffect.IsEmpty() ) return;

	ioEffect *pEffect = pOwner->AttachEffect( m_TalismanEffect, NULL );
	if( pEffect )
	{
		m_dwTalismanEffect = pEffect->GetUniqueID();
	}
}

bool ioTalisman::DestroyTalisman( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	if( m_dwTalismanEffect == -1 ) return false;

	pOwner->EndEffect( m_dwTalismanEffect, false );
	return true;
}

DWORD ioTalisman::GetCurLiveTime()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	if( dwCurTime < m_dwEndTime )
		return (m_dwEndTime - dwCurTime);
	else
		return 0;
}

void ioTalisman::SetCurLiveTime( DWORD dwCurGapTime )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurGapTime > 0 )
		m_dwEndTime = dwCurTime + dwCurGapTime;
	else
		m_dwEndTime = dwCurTime;
}

bool ioTalisman::AddTalismanBuff( ioBaseChar *pOwner )
{
	if( pOwner->IsProtectState() )
		return false;

	if( pOwner->HasBuff( BT_ABSOLUTE_PROTECTION ) )
		return false;

	ioHashStringVec vBuffList;

	bool bResult = false;
	int iCnt = m_vTargetBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_vTargetBuffList[i];

		if( !szBuffName.IsEmpty() )
		{
			if( pOwner->AddNewBuffWithAnimateTime( szBuffName, "", "", NULL, 0 ) )
			{
				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}
	}

	return bResult;
}

bool ioTalisman::RemoveTalismanBuff( ioBaseChar *pOwner )
{
	ioHashStringVec vBuffList;

	bool bResult = false;
	int iCnt = m_vTargetBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_vTargetBuffList[i];

		if( !szBuffName.IsEmpty() && pOwner->RemoveBuff( szBuffName ) )
			bResult = true;
	}

	return bResult;
}
