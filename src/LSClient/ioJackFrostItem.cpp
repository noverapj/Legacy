

#include "stdafx.h"

#include "ioJackFrostItem.h"
#include "WeaponDefine.h"

ioJackFrostItem::ioJackFrostItem()
{
	m_dwChargeStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwCreateObjectTime = 0;

	m_iMaxCreateObjCnt = 1;
	
	m_MineState = MS_NONE;
	m_iCurCombo = 0;
}

ioJackFrostItem::ioJackFrostItem( const ioJackFrostItem &rhs )
: ioWeaponItem( rhs ),
 m_BranchDelayAni( rhs.m_BranchDelayAni ),
 m_CreateEffect( rhs.m_CreateEffect ),
 m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
 m_iMaxCreateObjCnt( rhs.m_iMaxCreateObjCnt ),
 m_bImmediatelyEquip( rhs.m_bImmediatelyEquip ),
 m_dwCreateObjectItemCode( rhs.m_dwCreateObjectItemCode ),
 m_fCreateOffset( rhs.m_fCreateOffset )
{
	m_MineState = MS_NONE;
	m_iCurCombo = 0;
	m_dwChargeStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwCreateObjectTime = 0;
}

ioJackFrostItem::~ioJackFrostItem()
{
}

void ioJackFrostItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_iMaxCreateObjCnt = rkLoader.LoadInt_e( "object_max_cnt", 1 );

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAni = szBuf;
	rkLoader.LoadString_e( "create_item_effect", "", szBuf, MAX_PATH );
	m_CreateEffect = szBuf;

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}
	m_bImmediatelyEquip = rkLoader.LoadBool_e( "immediately_equip", false );
	m_dwCreateObjectItemCode = rkLoader.LoadInt_e( "create_item_code", 0 );
	m_fCreateOffset = rkLoader.LoadFloat_e( "create_position_offset", 0.0f );
}

ioItem* ioJackFrostItem::Clone()
{
	return new ioJackFrostItem( *this );
}

void ioJackFrostItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		m_MineState = MS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		pOwner->SetState( CS_ATTACK );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		
		int iAniID = pGrp->GetAnimationIdx( m_BranchDelayAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_MineState = MS_NONE;
	}
}

void ioJackFrostItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
	m_MineState = MS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MineState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJackFrostItem::ChangeToCreateObject( ioBaseChar *pOwner )
{
	const AttackAttribute &rkAttr = m_vExtendAttributeList[m_iCurCombo];

	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	float fTimeRate   = rkAttr.m_fAttackAniRate;
	DWORD dwPreDelay  = rkAttr.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	m_dwCreateObjectTime = FRAMEGETTIME() + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate + dwPreDelay;
	m_MineState = MS_CREATE_OBJECT;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MineState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJackFrostItem::CreateObjectItem( ioBaseChar *pOwner )
{
	DeleteObjectItem( pOwner );

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioHashString szOwnerItem = GetName();
	float fRotateAngle = 360.f / m_iMaxCreateObjCnt;
	for( int i = 0; i < m_iMaxCreateObjCnt; i++ )
	{
		D3DXVECTOR3 vCreatePos = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		float fCurRotateAngle = ioMath::ArrangeHead( fRotateAngle * i );

		D3DXQUATERNION	qtNewRot;
		D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fCurRotateAngle), 0.0f, 0.0f );
		vCreatePos = qtNewRot * vCreatePos;

		vCreatePos *= m_fCreateOffset;
		vCreatePos += pOwner->GetMidPositionByRate();

		float fCheckHeight = pStage->GetMapHeight( vCreatePos.x, vCreatePos.z, NULL, false );
		float fGap = fCheckHeight - vCreatePos.y;
		if( fGap > 50.0f )
			vCreatePos = pOwner->GetMidPositionByRate();
		else if( fGap > 0.0f )
			vCreatePos.y = fCheckHeight;

		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			if( pOwner->IsNeedSendNetwork() )
			{
				if( m_bImmediatelyEquip )
					pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

				SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
				kPacket << vCreatePos;
				kPacket << m_dwCreateObjectItemCode;
				kPacket << szOwnerItem;
				kPacket << pOwner->GetCharName();
				kPacket << m_bImmediatelyEquip;

				if( m_bImmediatelyEquip )
					kPacket << static_cast<int>( ioObjectItem::OCT_EQUIP_SKILL );

				TCPNetwork::SendToServer( kPacket );
			}
		}
		else
		{
			ioItem *pItem = pStage->CreateItem( m_dwCreateObjectItemCode );
			if( !pItem )	return;

			pItem->ReLoadProperty();

			ioObjectItem *pObject = ToObjectItem( pItem );
			if( pObject )
			{
				pObject->SetObjectCreateItem( szOwnerItem );

				if( m_bImmediatelyEquip )
					pObject->SetObejctCreateType( ioObjectItem::OCT_EQUIP_SKILL );
			}

			// 싱글모드 뿐만아니라 로비에서도 이쪽에서 처리된다.
			if( m_bImmediatelyEquip )
			{
				pOwner->EquipItem( pItem );
			}
			else
			{
				ioFieldItem *pField = pStage->CreateFieldItem( pItem, false );
				if( pField )
				{
					pField->SetWorldPositionByServerSide( vCreatePos );
					pField->SetDropState( pField->GetWorldPosition() );
					pField->CreateDropEffect();
					pField->SetTeamType( pOwner->GetTeam() );
				}
				m_vCreateObjectIndex.push_back( pItem->GetItemCreateIndex() );
			}
		}
		pOwner->CreateMapEffectBySkill( m_CreateEffect, vCreatePos );
	}
}

void ioJackFrostItem::DeleteObjectItem( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	int iCreateObjectCount = m_vCreateObjectIndex.size();
	if( iCreateObjectCount != 0 )
	{
		for( int i = 0; i < iCreateObjectCount; i++ )
			pStage->ReleaseFieldItemByIndex( m_vCreateObjectIndex[i] );

		m_vCreateObjectIndex.clear();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)MS_DELETE_OBJECT;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioJackFrostItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_MineState )
	{
	case MS_CHARGING:
		OnCharging( pOwner );
		break;
	case MS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case MS_CREATE_OBJECT:
		OnCreateObjectItem( pOwner );
		break;
	}
}

void ioJackFrostItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToCreateObject( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioJackFrostItem::OnCreateObjectItem( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwCreateObjectTime > 0 && m_dwCreateObjectTime < FRAMEGETTIME() )
	{
		m_dwCreateObjectTime = 0;
		CreateObjectItem( pOwner );
	}
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		pOwner->SetState( CS_DELAY );
	}
}

void ioJackFrostItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case MS_NORMAL_ATTACK:
		ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
		m_MineState = MS_NORMAL_ATTACK;
		break;
	case MS_CREATE_OBJECT:
		ChangeToCreateObject( pOwner );
		break;
	case MS_DELETE_OBJECT:
		DeleteObjectItem( pOwner );
		break;
	case MS_NONE:
		pOwner->SetState( CS_DELAY );
		m_MineState = MS_NONE;
		break;
	}
}

ioWeaponItem::WeaponSubType ioJackFrostItem::GetSubType() const
{
	return WST_JACK_FROST_ITEM;
}

bool ioJackFrostItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_MineState == MS_CHARGING ||
			m_MineState == MS_CREATE_OBJECT )
			return true;
	}

	return false;
}

int ioJackFrostItem::GetMaxBullet()
{
	return m_iMaxCreateObjCnt;
}

const WeaponInfo& ioJackFrostItem::GetExtendAttributeIdx() const
{
	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
	{
		return m_vExtendAttributeList[m_iCurCombo].m_vWeaponInfoList.front();
	}

	static WeaponInfo tempWeaponInfo;
	return tempWeaponInfo;
}

void ioJackFrostItem::AddCreateObjectItemInfo( int iCreateObjectIndex )
{
	m_vCreateObjectIndex.push_back( iCreateObjectIndex );
}