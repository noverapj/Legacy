#include "StdAfx.h"
#include "ioAttackRangeCheckBuff.h"


ioAttackRangeCheckBuff::ioAttackRangeCheckBuff(void)
{
}

ioAttackRangeCheckBuff::ioAttackRangeCheckBuff( const ioAttackRangeCheckBuff &rhs )
	: ioBuff( rhs ),
	m_fBuffEnableRange( rhs.m_fBuffEnableRange ),
	m_vRangeOverBuff( rhs.m_vRangeOverBuff )
{

}

ioAttackRangeCheckBuff::~ioAttackRangeCheckBuff(void)
{
}

void ioAttackRangeCheckBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	m_fBuffEnableRange = rkLoader.LoadFloat_e( "buff_enable_range", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "range_over_buff_cnt", 0 );
	for( int i = 0; i < iCnt; i++ )
	{
		sprintf_s( szKey, "range_over_buff%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		
		m_vRangeOverBuff.push_back( szBuf );
	}
}

ioBuff* ioAttackRangeCheckBuff::Clone()
{
	return new ioAttackRangeCheckBuff( *this );
}

void ioAttackRangeCheckBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

}

bool ioAttackRangeCheckBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioAttackRangeCheckBuff::ProcessBuff( float fTimePerSec )
{
	if( !m_pOwner )
		return;

	ioEntityGroup* pGrp = m_pOwner->GetGroup();
	if( !pGrp )
		return;

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
		{
			ioBuff::ProcessBuff( fTimePerSec );
		}

		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	CheckRange();
}

void ioAttackRangeCheckBuff::EndBuff()
{
	
	ioBuff::EndBuff();
}

void ioAttackRangeCheckBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );
}

void ioAttackRangeCheckBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
}

void ioAttackRangeCheckBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	
}

void ioAttackRangeCheckBuff::CheckRange()
{
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;
	
	ioBaseChar* pCreateChar = pOwner->GetCreator()->GetBaseChar( m_CreateChar );
	if( !pCreateChar )
		return;

	D3DXVECTOR3 vDiff = pOwner->GetWorldPosition() - pCreateChar->GetWorldPosition();
	float fRange = D3DXVec3Length( &vDiff );

	if( fRange > m_fBuffEnableRange )
	{
		for( int i = 0; i < m_vRangeOverBuff.size(); i++ )
		{
			ioBuff* pBuff = pOwner->GetBuff( m_vRangeOverBuff[i] );
		
			if( !pBuff )
				pOwner->ReserveAddNewBuff( m_vRangeOverBuff[i], m_CreateChar, m_ItemName, NULL );
		}
	}
	else
	{
		for( int i = 0; i < m_vRangeOverBuff.size(); i++ )
		{
			ioBuff* pBuff = pOwner->GetBuff( m_vRangeOverBuff[i] );

			if( pBuff )
				pBuff->SetReserveEndBuff();
		}
	}
}