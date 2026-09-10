
#include "stdafx.h"

#include "ioDummyAttachBuff2.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ItemDefine.h"

ioDummyAttachBuff2::ioDummyAttachBuff2()
{
	m_BuffState = BS_None;
}

ioDummyAttachBuff2::ioDummyAttachBuff2( const ioDummyAttachBuff2 &rhs )
	: ioBuff( rhs ),
	  m_vDummyOffset( rhs.m_vDummyOffset ),
	  m_szDummyName( rhs.m_szDummyName ),
	  m_RemoveBuffList( rhs.m_RemoveBuffList )
{
	m_BuffState = BS_None;
}

ioDummyAttachBuff2::~ioDummyAttachBuff2()
{
}

void ioDummyAttachBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;

	m_vDummyOffset.x = rkLoader.LoadFloat_e( "dummy_offset_x", 0.0f );
	m_vDummyOffset.y = rkLoader.LoadFloat_e( "dummy_offset_y", 0.0f );
	m_vDummyOffset.z = rkLoader.LoadFloat_e( "dummy_offset_z", 0.0f );

	LoadRemoveBuffList( rkLoader );
}

ioBuff* ioDummyAttachBuff2::Clone()
{
	return new ioDummyAttachBuff2( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyAttachBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_BuffState = BS_None;
}

bool ioDummyAttachBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_BuffState = BS_None;

	return true;
}

void ioDummyAttachBuff2::ProcessBuff( float fTimePerSec )
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

	switch ( m_BuffState )
	{
	case BS_None:
		{
			if ( !SetOwnerState( m_pOwner ) )
			{
				SetReserveEndBuff();
				return;
			}

			CreateDummy( m_pOwner );
		}
		break;
	case BS_Loop:
		{
			if ( !CheckOwnerState( m_pOwner ) )
			{
				SetReserveEndBuff();
				return;
			}

			SetDummyPos( m_pOwner );
		}
		break;
	}	
}

void ioDummyAttachBuff2::EndBuff()
{
	ProcessEnd( m_pOwner );

	ioBuff::EndBuff();	
}

void ioDummyAttachBuff2::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case BS_CreateDummy:
		{
			int nIndex;
			rkPacket >> nIndex;

			D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyOffset );

			ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, nIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyAttachBuff2::LoadRemoveBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	m_RemoveBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_RemoveBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "remove_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_RemoveBuffList.push_back( szBuf );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyAttachBuff2::CreateDummy( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyOffset );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, iCurIndex, pOwner->GetCharName(),	vPos, 0.0f, 0, true );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CreateDummy;
		kPacket << iCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyAttachBuff2::SetDummyPos( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szDummyName );
	if ( pDummyChar )
	{
		D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyOffset );
		pDummyChar->SetWorldPosition( vPos );
		pDummyChar->SetWorldOrientation( qtRot );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDummyAttachBuff2::SetOwnerState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioObjectItem *pObjectItem = pOwner->GetObject();
	ioObjectAttachItem *pAttachItem = ToObjectAttachItem( pObjectItem );
	if( !pAttachItem )
		return false;

	pOwner->SetState( CS_OBJECT_ATTACH );

	m_BuffState = BS_Loop;

	return true;
}

bool ioDummyAttachBuff2::CheckOwnerState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	switch ( pOwner->GetState() )
	{
	case CS_JUMP:
	case CS_OBJECT_ATTACH:
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyAttachBuff2::ProcessEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_RemoveBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->RemoveBuff( m_RemoveBuffList[i], true );

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szDummyName );
	if ( pDummyChar )
		pDummyChar->SetDieState( true );
}