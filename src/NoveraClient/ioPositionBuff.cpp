
#include "stdafx.h"
#include "ioPositionBuff.h"

ioPositionBuff::ioPositionBuff()
{
}

ioPositionBuff::ioPositionBuff( const ioPositionBuff &rhs )
	: ioBuff( rhs ),
	m_DummyCharName( rhs.m_DummyCharName ),
	m_stEraseBuff( rhs.m_stEraseBuff ),
	m_bEraseGravity( rhs.m_bEraseGravity )
{
}

ioPositionBuff::~ioPositionBuff()
{
}

void ioPositionBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "find_dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;

	rkLoader.LoadString_e( "erase_buff_name", "", szBuf, MAX_PATH );
	m_stEraseBuff = szBuf;

	m_bEraseGravity = rkLoader.LoadBool_e( "erase_gravity", false );
}

ioBuff* ioPositionBuff::Clone()
{
	return new ioPositionBuff( *this );
}

void ioPositionBuff::StartBuff( ioBaseChar *pOwner )
{
	m_nFirstDummyID = -1;

	ioBuff::StartBuff( pOwner );

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioPositionBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioPositionBuff::SetAction()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( pOwner->GetState() != CS_DIE &&
		pOwner->GetState() != CS_LOADING &&
		pOwner->GetState() != CS_OBSERVER &&
		pOwner->GetState() != CS_VIEW )
	{
		pOwner->SetState( CS_LOCKUP_BUFF );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		ioBaseChar *pCreater = m_pOwner->GetBaseChar(m_CreateChar);
		if( pCreater == NULL || m_DummyCharName.IsEmpty() )
		{
			SetReserveEndBuff();
			return;
		}

		if( !IsLive() )
			return;

		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pCreater, m_DummyCharName );
		if( !pDummy )
		{
			SetReserveEndBuff();
			return;
		}

		pDummy->AddWoundedTargetName( m_pOwner->GetCharName() );
	}
}

void ioPositionBuff::ProcessBuff( float fTimePerSec )
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

	if( m_CreateChar.IsEmpty() )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar *pCreater = m_pOwner->GetBaseChar(m_CreateChar);
	if( pCreater == NULL || m_DummyCharName.IsEmpty() )
	{
		SetReserveEndBuff();
		return;
	}

	if( !IsLive() )
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pCreater, m_DummyCharName );
	if( !pDummy )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_nFirstDummyID == - 1)
		m_nFirstDummyID = pDummy->GetDummyCharIdx();

	//같은 더미가 2마리 이상 존재시 강제 버프 삭제
	if( g_DummyCharMgr.GetDummyCharCntToName( pCreater, m_DummyCharName ) > 1 )
	{
		SetReserveEndBuff();
		return;
	}
	
	if( m_nFirstDummyID != -1 && pDummy->GetDummyCharIdx() != m_nFirstDummyID )
	{
		SetReserveEndBuff();
		return;
	}
	
	const D3DXVECTOR3& vPos = pDummy->GetWorldPosition();
	m_pOwner->SetWorldPosition( vPos );
	if ( m_bEraseGravity && m_pOwner->IsFloating() )
		m_pOwner->SetGravityAmt( 0.0f );
}

void ioPositionBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner )
	{
		if( pOwner->IsPrisonerMode() || pOwner->GetState() == CS_LOCKUP_BUFF )
		{
			pOwner->SetState( CS_DELAY );
		}
		if( !m_stEraseBuff.IsEmpty() && pOwner->HasBuff(m_stEraseBuff) )
			pOwner->RemoveBuff( m_stEraseBuff );
	}

	ioBuff::EndBuff();
}