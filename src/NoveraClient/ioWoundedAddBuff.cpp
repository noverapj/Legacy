
#include "stdafx.h"

#include "ioWoundedAddBuff.h"

#include "ioBaseChar.h"

ioWoundedAddBuff::ioWoundedAddBuff()
{
}

ioWoundedAddBuff::ioWoundedAddBuff( const ioWoundedAddBuff &rhs )
	: ioBuff( rhs ),
	  m_bEnableOutLine( rhs.m_bEnableOutLine ),
	  m_fOutLineR( rhs.m_fOutLineR ),
	  m_fOutLineG( rhs.m_fOutLineG ),
	  m_fOutLineB( rhs.m_fOutLineB ),
	  m_fOutLineAlpha( rhs.m_fOutLineAlpha ),
	  m_fOutLineThickness( rhs.m_fOutLineThickness ),
	  m_szAddBuffList( rhs.m_szAddBuffList ),
	  m_szRemoveBuffList( rhs.m_szRemoveBuffList ),
	  m_szWoundEffect( rhs.m_szWoundEffect )
{
}

ioWoundedAddBuff::~ioWoundedAddBuff()
{
}

void ioWoundedAddBuff::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	ioBuff::LoadProperty( rkLoader );
	
	m_bEnableOutLine = rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR = rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG = rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB = rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha = rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness = rkLoader.LoadFloat_e( "outline_thickness", 2.0f );

	rkLoader.LoadString_e( "wound_effect", "", szBuf, MAX_PATH );
	m_szWoundEffect = szBuf;

	LoadAddBuffList( rkLoader );
	LoadRemoveBuffList( rkLoader );
}

ioBuff* ioWoundedAddBuff::Clone()
{
	return new ioWoundedAddBuff( *this );
}

void ioWoundedAddBuff::StartBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioBuff::StartBuff( pOwner );
	
	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	Init();
	RemoveBuff( pOwner );
}

bool ioWoundedAddBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if ( !pOwner )
		return false;

	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	Init();
	RemoveBuff( pOwner );
	return true;
}

void ioWoundedAddBuff::ProcessBuff( float fTimePerSec )
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

void ioWoundedAddBuff::EndBuff()
{
	ioBuff::EndBuff();

	if( m_pOwner && m_bEnableOutLine )
		m_pOwner->RestoreOutLineChange( COT_BUFF_PROTECTION );
}

void ioWoundedAddBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	pOwner->AttachEffect( m_szWoundEffect );

	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = pOwner->GetBaseChar( szAttacker );
	if ( !pAttacker )
		return;

	if ( !pAttacker->GetBuff( BT_CREATE_WEAPON_BUFF3 ) )
	{
		int nAddBuffCnt = m_szAddBuffList.size();
		for( int i=0; i<nAddBuffCnt; ++i )
		pAttacker->AddNewBuff( m_szAddBuffList[i], pOwner->GetCharName(), "", NULL );
	}

	SetReserveEndBuff();
}

void ioWoundedAddBuff::LoadAddBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "add_buff_cnt", 0 );
	m_szAddBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_szAddBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szTitle, "add_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_szAddBuffList.push_back( szBuf );
	}
}

void ioWoundedAddBuff::LoadRemoveBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	m_szRemoveBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_szRemoveBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szTitle, "remove_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_szRemoveBuffList.push_back( szBuf );
	}
}

void ioWoundedAddBuff::Init()
{
	m_iCurActiveCount = m_iMaxActiveCount;
}

void ioWoundedAddBuff::RemoveBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	for ( int i=0; i<(int)m_szRemoveBuffList.size(); i++ )
	{
		ioBuff *pBuff = pOwner->GetBuff( m_szRemoveBuffList[i], pOwner->GetCharName() );
		if ( pBuff )
			pBuff->SetReserveEndBuff();
	}
}

void ioWoundedAddBuff::SetAddBuff( ioBaseChar *pAttacker )
{
	if ( !pAttacker )
		return;

	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->AttachEffect( m_szWoundEffect );

	if ( !pAttacker->GetBuff( BT_CREATE_WEAPON_BUFF3 ) )
	{
		int nAddBuffCnt = m_szAddBuffList.size();
		for( int i=0; i<nAddBuffCnt; ++i )
			pAttacker->AddNewBuff( m_szAddBuffList[i], pOwner->GetCharName(), "", NULL );
	}

	SetReserveEndBuff();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << pAttacker->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}