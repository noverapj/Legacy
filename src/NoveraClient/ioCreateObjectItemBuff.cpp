
#include "stdafx.h"

#include "ioCreateObjectItemBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioCreateObjectItemBuff::ioCreateObjectItemBuff()
{
	m_ObjectState = OE_NONE;
}

ioCreateObjectItemBuff::ioCreateObjectItemBuff( const ioCreateObjectItemBuff &rhs )
	: ioBuff( rhs ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_bDisableDropDownState( rhs.m_bDisableDropDownState ),
	  m_dwObjectItemCode( rhs.m_dwObjectItemCode ),
	  m_szObjectCheckBuff( rhs.m_szObjectCheckBuff ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_BuffEndState( rhs.m_BuffEndState )
{
	m_ObjectState = OE_NONE;
}

ioCreateObjectItemBuff::~ioCreateObjectItemBuff()
{
}

void ioCreateObjectItemBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "";

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropDownState = rkLoader.LoadBool_e( "disable_drop_down_state", false );
	m_dwObjectItemCode = rkLoader.LoadInt_e( "create_object_item_code", 0 );

	rkLoader.LoadString_e( "object_check_buff", "", szBuf, MAX_PATH );
	m_szObjectCheckBuff = szBuf;

	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );

	m_BuffEndState.clear();
	int iMaxState = rkLoader.LoadInt_e( "max_buff_end_state", 0 );
	for ( int i = 0; i < iMaxState; ++i )
	{
		wsprintf_e( szBuf, "buff_end_state%d", i + 1 );
		int nState = rkLoader.LoadInt( szBuf, -1 );
		m_BuffEndState.push_back( nState );
	}
}

ioBuff* ioCreateObjectItemBuff::Clone()
{
	return new ioCreateObjectItemBuff( *this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCreateObjectItemBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	SetEquipObjectItem( pOwner );
}

bool ioCreateObjectItemBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;	

	SetEquipObjectItem( pOwner );

	return true;
}

void ioCreateObjectItemBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() || CheckOwnerObject() )
		return;

	ProcessCheckObject();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioCreateObjectItemBuff::ProcessCheckObject()
{
	if( m_ObjectState != OE_NONE )
	{
		ioBaseChar *pOwner	= GetOwner();
		if( pOwner )
		{	
			ioObjectItem* pObject = pOwner->GetObject();
			if( !pObject )
			{
				SetReserveEndBuff();
				return;
			}

			if( pObject->GetItemCode() != m_dwObjectItemCode )
			{
				SetReserveEndBuff();
				return;
			}
		}
	}
}

void ioCreateObjectItemBuff::EndBuff()
{
	ioBuff::EndBuff();	

	SetReleaseObjectItem( m_pOwner );

	ioItem *pItem = GetOwnerItem();
	if ( pItem )
		pItem->SetCurSkillGauge( 0.0f );

	if( m_pOwner && 0.0f < m_fEndJumpPower && !IsBuffEndJumpDisable( m_pOwner ) )
		m_pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioCreateObjectItemBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) 
		return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioCreateObjectItemBuff::CheckOwnerObject()
{
	if( !m_pOwner ) 
		return false;

	if( m_szObjectCheckBuff.IsEmpty() )
		return false;

	ioObjectItem *pItem = m_pOwner->GetObject();
	if ( pItem )
	{
		if ( !m_pOwner->HasBuff( m_szObjectCheckBuff ) )
		{
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

void ioCreateObjectItemBuff::SetEquipObjectItem( ioBaseChar *pOwner )
{
	if( !pOwner || m_ItemName.IsEmpty() )
		 return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
				
	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			D3DXVECTOR3 vCreatePos = pOwner->GetMidPositionByRate();
			pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << vCreatePos;
			kPacket << m_dwObjectItemCode;
			kPacket << m_ItemName;
			kPacket << pOwner->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_EQUIP_BUFF2 );

			//OCT_EQUIP_BUFF1,  OCT_EQUIP_BUFF2타입에서 이름을 전달하여 오브젝트 아이템에 해당 컨트롤 버프를 지정할 수 있도록 함
			kPacket << GetName();

			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		if( m_dwObjectItemCode > 0 )
		{
			pOwner->NewReserveEquipObjectItem( m_dwObjectItemCode, ioObjectItem::OCT_EQUIP_BUFF2, GetName() );
		}
	}
}

bool ioCreateObjectItemBuff::SetReleaseObjectItem( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_ObjectState != OE_EQUIP )
		return false;

	if( m_dwObjectItemCode > 0 )
	{		
		pOwner->ObjectItemUse( m_dwObjectItemCode, pOwner->GetState() );
	}

	return true;
}

bool ioCreateObjectItemBuff::IsBuffEndJumpDisable( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	CharState etate = pOwner->GetState();
	for ( int i = 0; i<(int)m_BuffEndState.size(); ++i )
	{
		if ( etate == (CharState)m_BuffEndState[i] )
		{
			return true;
		}
	}

	return false;
}