
#include "stdafx.h"

#include "ioTargetEquipObjectItemBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioNpcChar.h"


ioTargetEquipObjectItemBuff::ioTargetEquipObjectItemBuff()
{
	m_ObjectState = OE_NONE;
}

ioTargetEquipObjectItemBuff::ioTargetEquipObjectItemBuff( const ioTargetEquipObjectItemBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_vObjectItemList( rhs.m_vObjectItemList ),
m_bCreatorItemCheck( rhs.m_bCreatorItemCheck )
{
	m_dwCurObjectItem = 0;	
	m_ObjectState = OE_NONE;
}

ioTargetEquipObjectItemBuff::~ioTargetEquipObjectItemBuff()
{
}

void ioTargetEquipObjectItemBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );

	int iCnt = rkLoader.LoadInt_e( "create_item_cnt", 0 );

	m_vObjectItemList.clear();
	m_vObjectItemList.reserve( iCnt );

	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "create_item_code%d", i+1 );
		DWORD dwItemCode = rkLoader.LoadInt( szKey, 0 );

		m_vObjectItemList.push_back( dwItemCode );
	}

	m_bCreatorItemCheck = rkLoader.LoadBool_e( "creator_item_check", false );
}

ioBuff* ioTargetEquipObjectItemBuff::Clone()
{
	return new ioTargetEquipObjectItemBuff( *this );
}

void ioTargetEquipObjectItemBuff::StartBuff( ioBaseChar *pOwner )
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "BuffCheck - %s, StartTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	m_dwBuffStartTime = dwCurTime;
	if( m_iOperationType == OT_DELAY )
	{
		m_dwBuffStartTime += m_dwDelayDuration;
	}

	DWORD dwCurDuration = GetBuffDuration() * pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
	m_dwBuffEndTime = m_dwBuffStartTime + dwCurDuration;
	m_dwCheckTime = dwCurTime;

	m_bLive = true;
	
	m_dwCurObjectItem = 0;
	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioTargetEquipObjectItemBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;

		DWORD dwCurDuration = GetBuffDuration() * pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
		m_dwBuffStartTime = m_dwBuffEndTime - dwCurDuration;
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	m_dwCurObjectItem = 0;
	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioTargetEquipObjectItemBuff::SetExtraDurationByGrowth( float fDuration )
{
	m_dwExtraDurationByGrowth = 0;

	if( m_bDisableGrowth )
		return;

	if( GetBuffDuration() == 0 ) return;
	if( fDuration == 0.0f ) return;

	m_dwExtraDurationByGrowth = (DWORD)fDuration;

	DWORD dwCurDuration = GetBuffDuration() * m_pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
	m_dwBuffEndTime = m_dwBuffStartTime + dwCurDuration + m_dwExtraDurationByGrowth;
}

void ioTargetEquipObjectItemBuff::SetAction()
{
	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckChangeSkill( true );

	CheckChangeColor( true );
	CheckBuffDesc();
	
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioBaseChar *pOwner = GetOwner();

	if( pOwner && pStage )
		SetObjectItemEquip( pStage, pOwner );

	m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	m_bBuffStart = true;
}

void ioTargetEquipObjectItemBuff::EndBuff()
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioBaseChar *pOwner = GetOwner();

	if( pOwner && pStage )
		ReleaseObject( pStage, pOwner, true );

	ioBuff::EndBuff();
}

void ioTargetEquipObjectItemBuff::ProcessBuff( float fTimePerSec )
{
	CheckCreatorItem();
	CheckOwnerStateCheck();

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
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ProcessActive( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	ProcessCheckObject();
}

void ioTargetEquipObjectItemBuff::ProcessDelay( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckEnableDelayState() )
		return;

	if( GetBuffDuration() == 0 )
	{
		if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
		{
			SetAction();
		}
		return;
	}

	if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
	{
		SetAction();
	}

	if( m_dwBuffEndTime <= dwCurTime )
	{		
		SetReserveEndBuff();
		return;
	}

	CheckCallingProcess();
}

void ioTargetEquipObjectItemBuff::ProcessActive( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;

	CheckCallingProcess();

	if( GetBuffDuration() == 0 )
		return;	

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioBaseChar *pOwner;
	

	if( pStage )
		pOwner = pStage->GetBaseChar( m_OwnerName );

	if( !pOwner )
		return;

	if( m_dwBuffEndTime <= FRAMEGETTIME() )
	{		
		SetReserveEndBuff();
		return;
	}
}

void ioTargetEquipObjectItemBuff::ProcessCheckObject()
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

			if( pObject->GetItemCode() != m_dwCurObjectItem )
			{
				SetReserveEndBuff();
				return;
			}
		}
	}
}

void ioTargetEquipObjectItemBuff::SetObjectItemEquip( ioPlayStage *pStage, ioBaseChar* pOwner )
{	
	if( !pStage )	
		return;

	if( !pOwner )
		return;

	D3DXVECTOR3 vCreatePos = pOwner->GetMidPositionByRate();

	ioHashString szOwnerItem;
	if( GetOwnerItem() )
	{
		szOwnerItem = GetOwnerItem()->GetName();
	}

	int iSize = m_vObjectItemList.size();
	m_dwCurObjectItem = 0;

	if( iSize > 0 )
	{
		IORandom eRandom;
		eRandom.SetRandomSeed( timeGetTime() );
		int iRand = eRandom.Random( iSize );

		if( iRand >= 0 && iRand < iSize )
			m_dwCurObjectItem = m_vObjectItemList[iRand];
		else
			LOG.PrintTimeAndLog( 0, "Err - Random fuction - yunga");
	}
	
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsNeedSendNetwork() )
	{
		if( pStage->GetModeType() == MT_TEAM_SURVIVAL_AI )
		{
			ioNpcChar* pNPC = ToNpcChar( pOwner );
			if( pNPC )
			{
				if( pNPC->IsAI() )
				{
					//SKIP
					return;
				}
			}
		}
		
		pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

		SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_dwCurObjectItem;
		kPacket << szOwnerItem;
		kPacket << pOwner->GetCharName();
		kPacket << true;
		kPacket << static_cast<int>( ioObjectItem::OCT_EQUIP_BUFF1 );

		//OCT_EQUIP_BUFF1 Àü¿ë
		kPacket << GetName();
		TCPNetwork::SendToServer( kPacket );
	}
	else
	{
		if( m_dwCurObjectItem != 0 )
		{
			pOwner->NewReserveEquipObjectItem( m_dwCurObjectItem, ioObjectItem::OCT_EQUIP_BUFF1, GetName() );
		}
	}
}

bool ioTargetEquipObjectItemBuff::ReleaseObject( ioPlayStage *pStage, ioBaseChar* pOwner, bool bSyncCharState /*= true */ )
{
	if( !pStage )
		return false;

	if( !pOwner )
		return false;

	if( m_ObjectState != OE_EQUIP )
		return false;

	if( bSyncCharState )
		pOwner->ReleaseObjectItem( __FUNCTION__, pOwner->GetState() );
	else
		pOwner->ReleaseObjectItem( __FUNCTION__ );

	return true;
}

void ioTargetEquipObjectItemBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{	
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}
}

void ioTargetEquipObjectItemBuff::CheckCreatorItem()
{
	if ( !m_bCreatorItemCheck )
		return;
	if ( !m_pOwner )
		return;
	ioBaseChar *pCreater = m_pOwner->GetBaseChar( m_CreateChar );
	if ( !pCreater )
		return;
	ioWeaponItem* pAttackerItem = pCreater->GetWeapon();
	if ( !pAttackerItem )
		return;
	if( pAttackerItem->GetName() != m_ItemName )
	{
		SetReserveEndBuff();
		return;
	}
}

