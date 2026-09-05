#include "StdAfx.h"
#include "ioAttackKeyEscapeBuff.h"


ioAttackKeyEscapeBuff::ioAttackKeyEscapeBuff(void)
{
}

ioAttackKeyEscapeBuff::ioAttackKeyEscapeBuff( const ioAttackKeyEscapeBuff &rhs )
	: ioBuff( rhs ),
	m_dwTotalAttackKeyCount( rhs.m_dwTotalAttackKeyCount ),
	m_dwReduceTime( rhs.m_dwReduceTime ),
	m_fDamage( rhs.m_fDamage ),
	m_DummyCharName( rhs.m_DummyCharName ),
	m_vDestroyDummyList( rhs.m_vDestroyDummyList ),
	m_szDummyActionAni( rhs.m_szDummyActionAni ),
	m_fDummyActionAniRate( rhs.m_fDummyActionAniRate )
{
}

ioAttackKeyEscapeBuff::~ioAttackKeyEscapeBuff(void)
{
	m_vDestroyDummyList.clear();
}

void ioAttackKeyEscapeBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_dwTotalAttackKeyCount = (DWORD)rkLoader.LoadInt_e( "total_attack_key_count", 0 );
	m_dwReduceTime = (DWORD)rkLoader.LoadInt_e( "reduce_time", 0 );
	m_fDamage = rkLoader.LoadFloat_e( "damage", 0.0f );

	rkLoader.LoadString_e( "find_dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;

	m_vDestroyDummyList.clear();
	int iCnt = rkLoader.LoadInt_e( "destroy_dummy_cnt", 0 );
	for( int i = 0; i < iCnt; i++ )
	{
		sprintf_e( szKey, "destroy_dummy%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vDestroyDummyList.push_back( szBuf );
	}

	rkLoader.LoadString_e( "dummy_action_ani", "", szBuf, MAX_PATH );
	m_szDummyActionAni = szBuf;
	m_fDummyActionAniRate = rkLoader.LoadFloat_e( "dummy_action_ani_rate", 0.0f );
}

ioBuff* ioAttackKeyEscapeBuff::Clone()
{
	return new ioAttackKeyEscapeBuff( *this );
}

void ioAttackKeyEscapeBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwCurrentAttackKeyCount = 0;
}

bool ioAttackKeyEscapeBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioAttackKeyEscapeBuff::ProcessBuff( float fTimePerSec )
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

	ProcessKeyInput();
}

void ioAttackKeyEscapeBuff::EndBuff()
{
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );

	if( pCreator && !m_vDestroyDummyList.empty() )
	{
		g_DummyCharMgr.DestroyDummyCharByNameList( pCreator, m_vDestroyDummyList );
	}

	ioBuff::EndBuff();
}

void ioAttackKeyEscapeBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );
}

void ioAttackKeyEscapeBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
}

void ioAttackKeyEscapeBuff::ProcessKeyInput()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( pOwner->IsAttackKey() && pOwner->IsNeedProcess() && m_dwCurrentAttackKeyCount < m_dwTotalAttackKeyCount )
	{
		m_dwBuffEndTime -= m_dwReduceTime;
		m_dwCurrentAttackKeyCount++;

		EscapeKeyAction( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioAttackKeyEscapeBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	m_dwBuffEndTime -= m_dwReduceTime;

	EscapeKeyAction( pOwner );
}

void ioAttackKeyEscapeBuff::EscapeKeyAction( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	pOwner->ApplyDamage( m_fDamage, false );

	ioBaseChar* pCreateChar = pOwner->GetCreator()->GetBaseChar( m_CreateChar );
	ioDummyChar* pDummy = g_DummyCharMgr.FindDummyCharToName( pCreateChar, m_DummyCharName );
	if( pDummy )
	{
		ioEntityGroup* pGrp = pDummy->GetGroup();
		if( pGrp )
			pGrp->SetActionAni( m_szDummyActionAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fDummyActionAniRate );
	}
}