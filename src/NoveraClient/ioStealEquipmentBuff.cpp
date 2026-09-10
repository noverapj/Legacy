
#include "stdafx.h"

#include "ioStealEquipmentBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ItemDefine.h"


ioStealEquipmentBuff::ioStealEquipmentBuff()
{
	m_BuffEffectID = -1;

	m_pTargetChar = NULL;
	m_bStealEquip = false;
}

ioStealEquipmentBuff::ioStealEquipmentBuff( const ioStealEquipmentBuff &rhs )
: ioBuff( rhs ),
 m_bEnableTargetAim( rhs.m_bEnableTargetAim ),
 m_iEndType( rhs.m_iEndType ),
 m_fExtraRate( rhs.m_fExtraRate ),
 m_AddBuffList( rhs.m_AddBuffList ),
 m_bAddedBuffList( rhs.m_bAddedBuffList ),
 m_pTargetChar( rhs.m_pTargetChar ),
 m_vEnableStealEquipParts( rhs.m_vEnableStealEquipParts ),
 m_bStealEquip( rhs.m_bStealEquip )
{
	m_BuffEffectID = -1;
}

ioStealEquipmentBuff::~ioStealEquipmentBuff()
{
}

void ioStealEquipmentBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bEnableTargetAim = rkLoader.LoadBool_e( "enable_target_aim", false );
	m_iEndType = (EndType)rkLoader.LoadInt_e( "enable_end_type", ET_DEFAULT );

	m_fExtraRate = rkLoader.LoadFloat_e( "extra_rate", FLOAT1 );

	bool bEnableStealEquipParts = false;
	bEnableStealEquipParts = rkLoader.LoadBool_e( "steal_weapon", false );
	m_vEnableStealEquipParts.push_back( bEnableStealEquipParts );
	bEnableStealEquipParts = rkLoader.LoadBool_e( "steal_armor", false );
	m_vEnableStealEquipParts.push_back( bEnableStealEquipParts );
	bEnableStealEquipParts = rkLoader.LoadBool_e( "steal_helmet", false );
	m_vEnableStealEquipParts.push_back( bEnableStealEquipParts );
	bEnableStealEquipParts = rkLoader.LoadBool_e( "steal_cloak", false );
	m_vEnableStealEquipParts.push_back( bEnableStealEquipParts );
}

ioBuff* ioStealEquipmentBuff::Clone()
{
	return new ioStealEquipmentBuff( *this );
}

void ioStealEquipmentBuff::StartBuff( ioBaseChar *pOwner )
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
		m_dwBuffStartTime += m_dwDelayDuration;

	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
	m_dwCheckTime = dwCurTime;

	m_BuffEffectID = -1;
	
	m_bLive = true;

	if ( m_iEndType == ET_NOCHANGE )
		CheckingOwnerChar( pOwner );
}

bool ioStealEquipmentBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
		m_dwBuffStartTime = m_dwBuffEndTime - GetBuffDuration();
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioStealEquipmentBuff::SetStealTargetChar( ioBaseChar* pChar )
{
	m_pTargetChar = pChar;
}

void ioStealEquipmentBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_iOperationType == OT_DELAY )
	{
		if ( m_iEndType != ET_NOCHANGE )
		{	
			if( m_bBuffStart )
			{
				if( !pOwner->IsCanChangeTeam() )
				{
					pOwner->CheckUseBuffSkill( m_Name, false );
					SetReserveEndBuff();
					return;
				}
			}
		}
	}
	else
	{
		switch( m_iEndType )
		{
		case ET_DEFAULT:
			{
				if( !pOwner->IsCanChangeTeam() )
				{
					pOwner->CheckUseBuffSkill( m_Name, false );
					SetReserveEndBuff();
					return;
				}
			}
			break;
		case ET_NODASH:
			{
				if( !pOwner->IsCanChangeTeamEx() )
				{
					pOwner->CheckUseBuffSkill( m_Name, false );
					SetReserveEndBuff();
					return;
				}
			}
			break;
		}
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
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		{
			ioBuff::ProcessBuff( fTimePerSec );
			// 훔치기 시작
			if( m_bStealEquip && m_pTargetChar )
			{
				m_bStealEquip = false;
				SetAction();
				//SetReserveEndBuff();
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioStealEquipmentBuff::EndBuff()
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		if( m_iBuffType != BT_CAMERA_BUFF )
			LOG.PrintTimeAndLog( 0, "BuffCheck - %s, EndTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	CheckInvisibleWeapon( true );
	CheckInvisibleCloak( true );
	CheckInvisibleHelmet( true );
	CheckChangeSkill( false );
	CheckChangeColor( false );

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		if( m_BuffEffectID != -1)
			pOwner->EndEffect( m_BuffEffectID, false );

		pOwner->AttachEffect( m_BuffEndEffect, GetEffectEventOwner() );

		if( m_bDescEqualDuration )
			pOwner->EndEmoticonByName( m_DescName );

		if( IsBadStateBuff() )
		{
			if( !pOwner->IsOwnerChar() )
				pOwner->SetEmoticon( m_EndEmoticon, EMT_BAD_STATE );
		}
		else
		{
			if( !pOwner->IsOwnerChar() )
				pOwner->SetEmoticon( m_EndEmoticon, EMT_SKILL_BUFF );
		}

		pOwner->CheckGaugeTeam( false );
	}

	if( m_bLoopSoundPlayed && !m_LoopSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
		//g_SoundMgr.StopSound( m_LoopSound, 0 );
	}

	DestroyAreaWeapon();
	CheckBuffEndBuff();
	CheckCallWeaponEnd();
}

void ioStealEquipmentBuff::SetAction()
{
	ChangeEquipItem();
	m_bBuffStart = true;
}

void ioStealEquipmentBuff::ProcessDelay( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckEnableDelayState() )
		return;

	if( GetBuffDuration() == 0 )
	{
		if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
			SetAction();
		return;
	}

	if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
		SetAction();

	if( m_dwBuffEndTime <= dwCurTime )
	{
		SetReserveEndBuff();
		return;
	}

	CheckCallingProcess();
}

void ioStealEquipmentBuff::ChangeEquipItem()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}
	
	if( !pOwner->CheckGrapplingExceptionState() )
	{
		SetReserveEndBuff();
		return;
	}

	if( !m_pTargetChar )
		return;

	if( m_pTargetChar->IsGangsi() )
		return;

	float fPrevSkillGauge[MAX_INVENTORY] = { 0.0f, 0.0f, 0.0f, 0.0f };
	DWORD dwItemCode = 0;

	int i, iItemIndex;
	ioHashString szOwner;

	bool bMyPreEquipItemParts[ EP_MAX ] = { false, false, false, false };

	for(i = 0;i < MAX_INVENTORY;i++)
	{
		ioItem *pItem = pOwner->GetEquipedItem( i );
		if( pItem )
		{
			bMyPreEquipItemParts[ i ] = true;
			fPrevSkillGauge[i] = pItem->GetCurSkillGuage();
		}
	}

	for( i = 0; i < (int)m_vEnableStealEquipParts.size(); i++ )
	{
		if( !m_vEnableStealEquipParts[ i ] )
			continue;

		ioItem *pStealItem = m_pTargetChar->GetEquipedItem( i );

		// 상대방의 특정 부위 장비가 없을 경우, 자신의 특정 부위 장비가 없을 경우 장비 아이템 복사 동작 하지 않음
		if( !pStealItem )
			continue;
		if( !bMyPreEquipItemParts[ i ] )
			continue;

		dwItemCode = pStealItem->GetItemCode();
		iItemIndex = pStealItem->GetItemCreateIndex();

		if( dwItemCode > 0 )
		{
			ioPlayStage *pStage = pOwner->GetCreator();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
				kPacket << pOwner->GetWorldPosition();
				kPacket << dwItemCode;
				kPacket << pStealItem->GetName();
				kPacket << pOwner->GetCharName();
				kPacket << true;
				kPacket << static_cast<int>( ioObjectItem::OCT_STEAL_EQUIP_BUFF );
				TCPNetwork::SendToServer( kPacket );
			}
			else if( g_App.IsSingleMode() )
			{
				ioItem *pItem = pStage->CreateItem( dwItemCode );
				if( pItem )
				{
					pItem->ReLoadProperty();
					pItem->SetOwnerName( szOwner );
					pItem->SetItemReinforce( 0 );


					ioItem *pPreItem = pOwner->EquipItem( pItem, true );
					SAFEDELETE( pPreItem );
				
					if( ToSuddenAttackItem( pItem ) )
						ToSuddenAttackItem( pItem )->SetInitialize( szOwner, m_pOwner->GetCreator(), pOwner->GetSuddenMeshType() );
					else if( ToDummyCharItem2( pItem ) )
						ToDummyCharItem2( pItem )->SetInitialize( szOwner, m_pOwner->GetCreator(), pOwner->GetSuddenMeshType() );
					else
					{
						pItem->Initialize( pOwner, pStage );
					}

					///////////////////////
					int iSlot = (int)dwItemCode / DEFAULT_BASIC_ITEM_CODE;
					ioItem *pEquiped = pOwner->GetEquipedItem( iSlot );
					int iEquipedBullet = -1;
					float fEquipedGauge = 0.0f;

					if( pEquiped )
					{
						iEquipedBullet = pEquiped->GetCurBullet();
						fEquipedGauge = pEquiped->GetCurSkillGuage();
					}				
				}
			}
		}
	}

	for(i = 0;i < MAX_INVENTORY;i++)
	{
		ioItem *pItem = pOwner->GetEquipedItem( i );
		if( pItem && dwItemCode > 0 )
		{
			pItem->SetCurSkillGauge( fPrevSkillGauge[i] );
		}
	}
}

void ioStealEquipmentBuff::CheckingOwnerChar( ioBaseChar *pOwner )
{
	if ( !pOwner || m_CreateChar.IsEmpty() )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	ioBaseChar* pCreater = pStage->GetBaseChar( m_CreateChar );
	if ( !pCreater )
		return;
	const BaseCharList charlist = pStage->GetBaseCharList();
	for ( int i=0 ; i< (int)charlist.size() ; ++i)
	{
		if ( charlist[i]->IsOwnerChar() )
		{
			if ( charlist[i]->GetTeam() == pCreater->GetTeam() )
				SetReserveEndBuff();
		}
	}
}
