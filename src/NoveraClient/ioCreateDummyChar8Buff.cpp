
#include "stdafx.h"

#include "ioCreateDummyChar8Buff.h"

#include "WeaponDefine.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioCreateDummyChar8Buff::ioCreateDummyChar8Buff()
{
}

ioCreateDummyChar8Buff::ioCreateDummyChar8Buff( const ioCreateDummyChar8Buff &rhs )
	: ioBuff( rhs ),
	  m_vDummyOffset( rhs.m_vDummyOffset ),
	  m_szDummyName( rhs.m_szDummyName ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList )
{	
}

ioCreateDummyChar8Buff::~ioCreateDummyChar8Buff()
{	
}

ioBuff* ioCreateDummyChar8Buff::Clone()
{
	return new ioCreateDummyChar8Buff( *this );
}

void ioCreateDummyChar8Buff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;

	m_vDummyOffset.x = rkLoader.LoadFloat_e( "dummy_offset_x", 0.0f );
	m_vDummyOffset.y = rkLoader.LoadFloat_e( "dummy_offset_y", 0.0f );
	m_vDummyOffset.z = rkLoader.LoadFloat_e( "dummy_offset_z", 0.0f );	

	int nBuffCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();
	for ( int i=0; i<nBuffCnt; i++ )
	{
		wsprintf_e( szKey, "owner_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_OwnerBuffList.push_back( szBuf );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCreateDummyChar8Buff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_BuffState = BS_START;

	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();
	for ( int i=0; i<nBuffCnt; i++ )
		pOwner->AddNewBuff( m_OwnerBuffList[i], pOwner->GetCharName(), "", NULL );
}

bool ioCreateDummyChar8Buff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_BuffState = BS_START;

	int nBuffCnt = (int)m_OwnerBuffList.size();
	for ( int i=0; i<nBuffCnt; i++ )
		pOwner->AddNewBuff( m_OwnerBuffList[i], pOwner->GetCharName(), "", NULL );

	return true;
}

void ioCreateDummyChar8Buff::ProcessBuff( float fTimePerSec )
{	
	if ( !m_pOwner )
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
	
	switch( m_BuffState )
	{
	case BS_START:
			CreateDummy( m_pOwner );
		break;
	case BS_CreateDummy:
		break;
	}
}

void ioCreateDummyChar8Buff::EndBuff()
{
	ioBuff::EndBuff();

	if ( m_pOwner )
	{
		int nBuffCnt = (int)m_OwnerBuffList.size();
		for ( int i=0; i<nBuffCnt; i++ )
		{
			ioBuff *pBuff = m_pOwner->GetBuff( m_OwnerBuffList[i] );
			if ( pBuff )
				pBuff->SetReserveEndBuff();
		}
	}
}

void ioCreateDummyChar8Buff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int nBuffState;
	rkPacket >> nBuffState;

	switch( (BuffState)nBuffState )
	{
	case BS_CreateDummy:
		{			
			int nIndex;
			rkPacket >> nIndex;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szDummyName );

			m_BuffState = BS_CreateDummy;

			ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, nIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCreateDummyChar8Buff::CreateDummy( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szDummyName );

	m_BuffState = BS_CreateDummy;

	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyOffset );

	int nCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, nCurIndex, pOwner->GetCharName(),	vPos, 0.0f, 0, true );
	if ( !pDummy )
		return;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CreateDummy;
		kPacket << nCurIndex;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}