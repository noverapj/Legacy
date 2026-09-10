

#include "stdafx.h"

#include "ioAliceItem.h"
#include "ioAliceObjectItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioAliceItem::ioAliceItem()
{
	Init();
}

ioAliceItem::ioAliceItem( const ioAliceItem &rhs )
	: ioWeaponItem( rhs ),		
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fExtraGaugeJumpInc( rhs.m_fExtraGaugeJumpInc ), 
	m_dwObjectItem( rhs.m_dwObjectItem ),	
	m_AliceGunEquipAttribute( rhs.m_AliceGunEquipAttribute ),
	m_AliceGunJumpEquipAttribute( rhs.m_AliceGunJumpEquipAttribute ),	
	m_AliceGunDashEquipAttribute( rhs.m_AliceGunDashEquipAttribute ),
	m_PreGatherAnimationList( rhs.m_PreGatherAnimationList ),
	m_szRleaseObjectEffect( rhs.m_szRleaseObjectEffect )
{
	Init();
}

ioAliceItem::~ioAliceItem()
{
}

void ioAliceItem::Init()
{
	m_ChargeState				= CS_NONE;

	m_dwAttackStartTime			= 0;
	m_iCurCombo					= 0;
	m_iCurAirJump				= 0;

	m_fAirJumpPowerRate			= 0.0f;
	m_fCurExtraGauge			= 0.0f;
}

void ioAliceItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	
	m_fNeedGauge			= rkLoader.LoadFloat_e( "need_extra_gauge", 0 );
	m_fExtraGaugeJumpInc	= rkLoader.LoadFloat_e( "extra_gauge_jump_inc_rate", 0.0f );

	m_dwObjectItem	= static_cast<DWORD>( rkLoader.LoadInt_e( "object_item", 0 ) );

	wsprintf_e( szBuf, "alice_gun_equip" );
	LoadAttackAttribute( szBuf, m_AliceGunEquipAttribute, rkLoader );

	wsprintf_e( szBuf, "alice_gun_jump_equip" );
	LoadAttackAttribute( szBuf, m_AliceGunJumpEquipAttribute, rkLoader );

	wsprintf_e( szBuf, "alice_gun_dash_equip" );
	LoadAttackAttribute( szBuf, m_AliceGunDashEquipAttribute, rkLoader );

	int iPreCount = rkLoader.LoadInt_e( "pre_gather_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szBuf, "pre_gather_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_PreGatherAnimationList.push_back( ioHashString( szBuf ) );
	}
	
	rkLoader.LoadString_e( "alice_gun_release_effect", "", szBuf, MAX_PATH );
	m_szRleaseObjectEffect = szBuf;
}

ioItem* ioAliceItem::Clone()
{
	return new ioAliceItem( *this );
}

ioWeaponItem::WeaponSubType ioAliceItem::GetSubType() const
{
	return WST_ALICE_ITEM;
}

void ioAliceItem::OnReleased( ioBaseChar *pOwner )
{
	if( IsEquipObject( pOwner ) )
		ReleaseObject( pOwner );

	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioAliceItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	int iPreGatherCnt = m_PreGatherAnimationList.size();

	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimationList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioAliceItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;
			}
			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioAliceItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && GetNeedBullet() <= GetCurBullet() && pOwner->IsEquipEnableObject() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			{
				ChangeEquipObjectState( pOwner );
			}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioAliceItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni		= pAttribute->m_AttackAnimation;
		m_fCurAniRate	= pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_ATTACK_FIRE;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_ATTACK );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioAliceItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;
	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case ST_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_iCurCombo;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToAttackFire( pOwner );
		}
		break;
	case ST_OBJECT_EQUIP:
		{
			byte eObjectState;
			rkPacket >> eObjectState;
			ChangeEquipObjectState( pOwner, static_cast<EquipObjectState>( eObjectState ) );
		}
		break;
	}	
}

bool ioAliceItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioAliceItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}
	return false;
}

bool ioAliceItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
		return false;

	fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioAliceItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioAliceItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( pOwner->IsSystemState() )
		return;

	if( m_ChargeState == CS_EQUIP_OBJECT )
	{
		float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
		m_fCurExtraGauge -= fGauge;
		m_fCurExtraGauge = max( m_fCurExtraGauge, 0 );
	}
	else
	{
		float fGauge = 0.0F;
		if( pOwner->GetState() == CS_JUMP )
		{
			fGauge = m_fExtraGaugeJumpInc * fTimePerSec;				
		}
		else
		{
			fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
		}

		m_fCurExtraGauge += fGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
	}
}

void ioAliceItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	bool bEquipObject = IsEquipObject( pOwner );
	float fRate = 0.0f;
	switch( iType )
	{
	case EGT_WOUNDED:
		if( bEquipObject )
			fRate = m_fExtraGaugeWoundDec;
		else
			fRate = 0.0f;
		break;
	}

	m_fCurExtraGauge -= fRate;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );

}

int ioAliceItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioAliceItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioAliceItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioAliceItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

bool ioAliceItem::IsEquipObject( ioBaseChar *pOwner )
{
	if( pOwner )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
		{
			return true;
		}
	}

	return false;
}

void ioAliceItem::ChangeEquipObjectState( ioBaseChar *pOwner, EquipObjectState eState /*= EOS_NONE*/ )
{
	if( !pOwner )
		return;

	if( !ChangeToObjectState( pOwner, eState ) )
	{
		ChangeToAttackFire( pOwner );
		return;
	}

	m_ChargeState = CS_EQUIP_OBJECT;
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();

	if( pStage && m_dwObjectItem > 0 )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );
			
			SP2Packet kUdpPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kUdpPacket );
			kUdpPacket << static_cast<byte>( ST_OBJECT_EQUIP );
			kUdpPacket << static_cast<byte>( eState );
			P2PNetwork::SendToAllPlayingUser( kUdpPacket );

			//아래 패킷으로 ioAttackableItem::EquipObjectItemByAttakable()가 호출됨
			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pOwner->GetWorldPosition();
			kPacket << m_dwObjectItem;
			kPacket << GetName();
			kPacket << pOwner->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwObjectItem );
			ioObjectItem *pObject = ToObjectItem( pItem );
			if( pObject )
			{
				pObject->ReLoadProperty();
				EquipObjectItemByAttakable( pOwner, pObject );
								
				pObject->SetObjectCreateItem( GetName() );
				pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				pOwner->EquipItem( pObject );
			}
		}
	}
}

void ioAliceItem::ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem )
{
	ioAliceObjectItem* pAlice = ToAliceObjectItem( pObjectItem );
	if( pAlice )
	{
		ReleaseObject( pOwner );
	}
}

void ioAliceItem::ReleaseObject( ioBaseChar *pOwner, bool bNeedProcess /* = true */ )
{
	if( !pOwner )
		return;

	if( bNeedProcess && !pOwner->IsNeedProcess() )
		return;

	m_ChargeState = CS_NONE;
	pOwner->AttachEffect( m_szRleaseObjectEffect );

	ioObjectItem *pObjectItem = pOwner->GetObject();
	if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
	{
		//애니메이션 초기화
		if( pOwner->GetState() == CS_DELAY )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
		}
		else if( pOwner->GetState() == CS_RUN )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__, CS_RUN );
		}
		else if( pOwner->GetState() == CS_DASH )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__, CS_DASH );
		}
		else
		{
			pOwner->ReleaseObjectItem( __FUNCTION__ );
		}
	}
}

void ioAliceItem::ReleaseObjectState( ioBaseChar* pOwner, ioObjectItem* pReleasedObject )
{	
	m_ChargeState = CS_NONE;

	if( IsEquipObject( pOwner ) )
	{
		pOwner->AttachEffect( m_szRleaseObjectEffect );
	}
}

bool ioAliceItem::ChangeToObjectState( ioBaseChar* pOwner, EquipObjectState eState )
{
	DWORD dwCurTime = FRAMEGETTIME();

	AttackAttribute* pAttribute = NULL;
	switch( eState )
	{
	case EOS_NORMAL:
		{
			pAttribute = &m_AliceGunEquipAttribute;	
		}
		break;
	case EOS_JUMP:
		{
			pAttribute = &m_AliceGunJumpEquipAttribute;

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
		}
		break;
	case EOS_DASH:
		{	
			pAttribute = &m_AliceGunDashEquipAttribute;
		}
		break;
	}

	if( !pAttribute )
		return false;
		
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;

	pOwner->SetObjectEquipState( pAttribute );

	return true;
}

void ioAliceItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioAliceItem::FillAliceInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket << static_cast<BYTE>( m_ChargeState );
	rkPacket << m_fCurExtraGauge;
}

void ioAliceItem::SetAliceInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE eState = 0;
	float fGauge = 0;
	float fSacleRate = 0.0f;
	rkPacket >> eState >> m_fCurExtraGauge;
	m_ChargeState = static_cast<ChargeState>( eState );
	
	//중간 입장에 관련된 동기화 처리중에는 변신 ANIMATION X
	ChangeToObjectState( pOwner, EOS_NONE );

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && eState == CS_EQUIP_OBJECT )
	{
		ioItem *pItem = pStage->CreateItem( m_dwObjectItem );
		ioObjectItem *pObject = ToObjectItem( pItem );
		if( pObject )
		{
			pObject->ReLoadProperty();
			EquipObjectItemByAttakable( pOwner, pObject );
						
			pObject->SetObjectCreateItem( GetName() );
			pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
			pOwner->EquipItem( pObject );
		}
	}
}