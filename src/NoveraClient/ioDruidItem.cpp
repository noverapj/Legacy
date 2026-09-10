

#include "stdafx.h"

#include "ioDruidItem.h"
#include "ioDruidObjectItem.h"
#include "ioChargeComboJump3.h"
#include "ioDruidEagleDummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "FindPredicateImpl.h"
#include "ioZoneEffectWeapon.h"

ioDruidItem::ioDruidItem()
{
	Init();
}

ioDruidItem::ioDruidItem( const ioDruidItem &rhs )
	: ioWeaponItem( rhs ),		
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fNeedExtendGauge( rhs.m_fNeedExtendGauge ),
	m_dwHalfChargeTime( rhs. m_dwHalfChargeTime ),
	m_HalfAttack( rhs.m_HalfAttack ),
	m_dwObjectItem( rhs.m_dwObjectItem ),	
	m_EquipAttribute( rhs.m_EquipAttribute ),
	m_DashEquipAttribute( rhs.m_DashEquipAttribute ),
	m_JumpEquipAttribute( rhs.m_JumpEquipAttribute ), 
	m_PreGatherAnimationList( rhs.m_PreGatherAnimationList ),
	m_szRleaseObjectEffect( rhs.m_szRleaseObjectEffect ),
	m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
	m_fHighDecreaseGauge( rhs.m_fHighDecreaseGauge )
{
	Init();
}

ioDruidItem::~ioDruidItem()
{
}

void ioDruidItem::Init()
{
	m_ChargeState				= CS_NONE;
	m_EquipObjectState			= EOS_NONE;

	m_dwAttackStartTime			= 0;
	m_iCurCombo					= 0;
	m_iCurAirJump				= 0;

	m_fAirJumpPowerRate			= 0.0f;
	m_fCurExtraGauge			= 0.0f;
}

void ioDruidItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fNeedGauge		= rkLoader.LoadFloat_e( "need_extra_gauge", 0 );
	m_fNeedExtendGauge	= rkLoader.LoadFloat_e( "need_extend_gauge", 0 );
	m_dwHalfChargeTime	= rkLoader.LoadInt_e( "half_charging_time", 0 );
	m_dwObjectItem		= rkLoader.LoadInt_e( "object_item", 0 );
	
	LoadAttackAttribute( "half_attack", m_HalfAttack, rkLoader );
	LoadAttackAttribute( "equip", m_EquipAttribute, rkLoader );
	LoadAttackAttribute( "dash_equip", m_DashEquipAttribute, rkLoader );
	LoadAttackAttribute( "jump_equip", m_JumpEquipAttribute, rkLoader );

	int iPreCount = rkLoader.LoadInt_e( "pre_gather_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szBuf, "pre_gather_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_PreGatherAnimationList.push_back( ioHashString( szBuf ) );
	}

	rkLoader.LoadString_e( "release_effect", "", szBuf, MAX_PATH );
	m_szRleaseObjectEffect		= szBuf;
	m_fHighDecreaseHeight		= rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGauge		= rkLoader.LoadFloat_e( "high_decrease_gauge", 3.0f );
}

ioItem* ioDruidItem::Clone()
{
	return new ioDruidItem( *this );
}

ioWeaponItem::WeaponSubType ioDruidItem::GetSubType() const
{
	return WST_DRUID;
}

void ioDruidItem::OnReleased( ioBaseChar *pOwner )
{
	if( IsEquipObject( pOwner ) )
		ReleaseObject( pOwner );

	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioDruidItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioDruidItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
	case CS_HALF_ATTACK_FIRE:
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

void ioDruidItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			if( GetNeedBullet() <= GetCurBullet() && pOwner->IsEquipEnableObject() )
			{
				ChangeEquipObjectState( pOwner );
			}
			else
			{
				ChangeToHalfAttackFire( pOwner );
			}
		}
	}
	else
	{
		if( m_dwAttackStartTime + m_dwHalfChargeTime <= dwCurTime )
		{
			ChangeToHalfAttackFire( pOwner );
		}
		else
		{
			ChangeToAttackFire( pOwner );
		}
	}
}

void ioDruidItem::ChangeToAttackFire( ioBaseChar *pOwner )
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

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
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

void ioDruidItem::ChangeToHalfAttackFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	pOwner->SetNormalAttackByAttribute( m_HalfAttack );
	m_szCurAni					= m_HalfAttack.m_AttackAnimation;
	m_fCurAniRate				= m_HalfAttack.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_HALF_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_HALF_ATTACK );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

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
	case ST_HALF_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_iCurCombo;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToHalfAttackFire( pOwner );
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

bool ioDruidItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioDruidItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

bool ioDruidItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
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

void ioDruidItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioDruidItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( pOwner->IsSystemState() )
		return;

	if( m_ChargeState == CS_EQUIP_OBJECT )
		return;

	if( pOwner->GetState() == CS_DRUID_EAGLE_FLY )
	{
		float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;

		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
			float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
			float fGapHeight = fabs( vCurPos.y - fMapHeight );

			if( m_fHighDecreaseHeight < fGapHeight )
				fGauge = m_fHighDecreaseGauge * fTimePerSec;
		}

		m_fCurExtraGauge -= fGauge;
		m_fCurExtraGauge = max( m_fCurExtraGauge, 0 );
	}
	else
	{
		if( pOwner->GetState() == CS_DELAY || pOwner->GetState() == CS_RUN )
		{
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
	}
}

void ioDruidItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	ioObjectItem *pObjectItem = pOwner->GetObject();
	if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
	{
		float fRate = 0.0f;
		switch( iType )
		{
		case EGT_WOUNDED:
				fRate = m_fExtraGaugeWoundDec;
			break;
		}

		m_fCurExtraGauge -= fRate;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
	}
}

int ioDruidItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioDruidItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioDruidItem::GetNeedExtendBullet()
{
	return (int)m_fNeedExtendGauge;
}

int ioDruidItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioDruidItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

bool ioDruidItem::IsEquipObject( ioBaseChar *pOwner )
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

void ioDruidItem::ChangeEquipObjectState( ioBaseChar *pOwner, EquipObjectState eState /*= EOS_NONE*/ )
{
	if( !pOwner )
		return;

	if( !ChangeToObjectState( pOwner, eState ) )
	{
		ChangeToAttackFire( pOwner );
		return;
	}

	m_ChargeState		= CS_EQUIP_OBJECT;
	m_EquipObjectState	= eState;

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

void ioDruidItem::EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem )
{	
	ioObjectItem *pObject = ToObjectItem( pObjectItem );
	if( !pObject )
		return;

	if( m_dwObjectItem != pObject->GetItemCode() )
		return;

	ioDruidObjectItem* pDruid = ToDruidObjectItem( pObject );
	if( pDruid )
	{
		pDruid->ChangeToEquipObjectState( m_EquipObjectState );
	}

	m_EquipObjectState = EOS_NONE;
}

void ioDruidItem::ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem )
{
	ioDruidObjectItem* pDruid = ToDruidObjectItem( pObjectItem );
	if( pDruid )
	{
		ReleaseObject( pOwner );
	}
}

void ioDruidItem::ReleaseObject( ioBaseChar *pOwner, bool bNeedProcess /* = true */ )
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

void ioDruidItem::ReleaseObjectState( ioBaseChar* pOwner, ioObjectItem* pReleasedObject )
{	
	m_ChargeState = CS_NONE;

	if( IsEquipObject( pOwner ) )
	{
		pOwner->AttachEffect( m_szRleaseObjectEffect );
	}

	switch( pOwner->GetState() )
	{
	case CS_EXTEND_DASH:
		SetExtendDashState( pOwner );
		break;
	case CS_JUMP:
		pOwner->SetState( CS_DRUID_EAGLE_FLY );
		break;
	}
}

bool ioDruidItem::ChangeToObjectState( ioBaseChar* pOwner, EquipObjectState eState )
{
	DWORD dwCurTime = FRAMEGETTIME();

	AttackAttribute* pAttribute = NULL;
	switch( eState )
	{
	case EOS_NORMAL:
		{
			pAttribute = &m_EquipAttribute;	
		}
		break;
	case EOS_DASH:
		{	
			pAttribute = &m_DashEquipAttribute;
		}
		break;
	case EOS_JUMP:
		{	
			pAttribute = &m_JumpEquipAttribute;
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

void ioDruidItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioDruidItem::FillDruidInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket << static_cast<BYTE>( m_ChargeState );
	rkPacket << m_fCurExtraGauge;
}

void ioDruidItem::SetDruidInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDruidEagleFlyState::ioDruidEagleFlyState() : 
	m_fCurrFlyRange( 0.0f ),
	m_dwStartTime( 0 ),
	m_dwCharingStartTime( 0 ),
	m_vMoveDir( ioMath::VEC3_ZERO ),
	m_ReadyMarkerID( -1 ),
	m_AimedMarkerID( -1 ),
	m_AttackKeyType( AKT_NONE ),
	m_dwCurrWeaponIdx( 0 ),
	m_AttackType( AT_NONE ),
	m_UpDownKey( UDK_NONE ),
	m_eDirKey( ioUserKeyInput::DKI_NONE ),
	m_dwTargetingStartTime( 0 )
{
}

ioDruidEagleFlyState::~ioDruidEagleFlyState()
{
}

void ioDruidEagleFlyState::LoadProperty( ioINILoader &rkLoader )
{
	ioSpecialStateBase::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_DummyCharName		= szBuf;

	m_fStartAutoTargetRange		= rkLoader.LoadFloat_e( "start_auto_target_range", 0.0f );
	m_fStartAutoTargetMinAngle	= rkLoader.LoadFloat_e( "start_auto_target_min_angle", 0.0f );
	m_fStartAutoTargetMaxAngle	= rkLoader.LoadFloat_e( "start_auto_target_max_angle", 0.0f );
	m_dwChargeTime				= rkLoader.LoadInt_e( "charge_time", 0 );
	m_dwWeaponIdx				= rkLoader.LoadInt_e( "target_fly_attack_weapon", 0 );
	m_fEndJumpPower				= rkLoader.LoadFloat_e( "end_jump_power", 0.0f );
	m_fChangeJumpPower			= rkLoader.LoadFloat_e( "change_jump_power", 0.0f );
	m_fCheckHeightMin			= rkLoader.LoadFloat_e( "height_check_min", 0.0f );

	LoadTargetMarker( rkLoader );
	LoadFindFly( rkLoader );
	LoadTargetFly( rkLoader );
	LoadUpFly( rkLoader );
}

void ioDruidEagleFlyState::LoadTargetMarker( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "ready_marker", "", szBuf, MAX_PATH );
	m_szReadyMarker			= szBuf;
	m_fReadyMarkerOffset	= rkLoader.LoadFloat_e( "ready_marker_offset", 0.0f );

	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedMarker			= szBuf;

	m_fTargetMaxRange		= rkLoader.LoadFloat_e( "target_find_max_range", 0.0f );
	m_fTargetMaxAngle		= rkLoader.LoadFloat_e( "target_find_max_angle", 0.0f );
	m_fTargetMinAngle		= rkLoader.LoadFloat_e( "target_find_min_angle", 0.0f );
	m_dwTargetingTime		= rkLoader.LoadInt_e( "target_duration", 0 );
}

void ioDruidEagleFlyState::LoadFindFly( ioINILoader &rkLoader )
{
	m_FindTargetEagleFlyInfo.m_fMaxRange				= FLT_MAX;
	m_FindTargetEagleFlyInfo.m_fFlyAccelSpeed			= rkLoader.LoadFloat_e( "target_find_fly_accel_speed", 0 );
	m_FindTargetEagleFlyInfo.m_fFlyMinSpeed				= rkLoader.LoadFloat_e( "target_find_fly_min_speed", 0 );
	m_FindTargetEagleFlyInfo.m_fFlyMaxSpeed				= rkLoader.LoadFloat_e( "target_find_fly_max_speed", 0 );
	m_FindTargetEagleFlyInfo.m_dwFlyMinSpeedDuration	= rkLoader.LoadInt_e( "target_find_fly_min_speed_duration", 0 );
}

void ioDruidEagleFlyState::LoadTargetFly( ioINILoader &rkLoader )
{
	m_TargetAttackEagleFlyInfo.m_fFlyAccelSpeed			= rkLoader.LoadFloat_e( "target_fly_accel_speed", 0 );
	m_TargetAttackEagleFlyInfo.m_fFlyMinSpeed			= rkLoader.LoadFloat_e( "target_fly_min_speed", 0 );
	m_TargetAttackEagleFlyInfo.m_fFlyMaxSpeed			= rkLoader.LoadFloat_e( "target_fly_max_speed", 0 );
	m_TargetAttackEagleFlyInfo.m_dwFlyMinSpeedDuration	= rkLoader.LoadInt_e( "target_fly_min_speed_duration", 0 );
}

void ioDruidEagleFlyState::LoadUpFly( ioINILoader &rkLoader )
{
	m_UpFlyInfo.m_fMaxRange				= rkLoader.LoadFloat_e( "up_fly_max_range", 0 );
	m_UpFlyInfo.m_fFlyAccelSpeed		= rkLoader.LoadFloat_e( "up_fly_accel_speed", 0 );
	m_UpFlyInfo.m_fFlyMinSpeed			= rkLoader.LoadFloat_e( "up_fly_min_speed", 0 );
	m_UpFlyInfo.m_fFlyMaxSpeed			= rkLoader.LoadFloat_e( "up_fly_max_speed", 0 );
	m_fUpAngle							= rkLoader.LoadFloat_e( "up_fly_angle", 0 );
	m_UpFlyInfo.m_dwFlyMinSpeedDuration	= rkLoader.LoadInt_e( "up_fly_min_speed_duration", 0 );
}

void ioDruidEagleFlyState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	//더미 생성
	CreateDummy( pOwner );

	ProcessKeyCheck( pOwner, false );

	if( pOwner->IsNeedProcess() )
	{
		FD_GrapplingAutoTarget kFindPred( pOwner, m_fStartAutoTargetMinAngle, m_fStartAutoTargetMaxAngle, m_fStartAutoTargetRange, false );
		ioBaseChar *pTarget = pOwner->GetCreator()->FindUser( kFindPred );

		//시작하자 마자 공격
		if( pTarget )
			ChangeToTargetAttackPre( pOwner, pOwner->GetCreator(), pTarget->GetCharName(), AT_NORMAL, m_eDirKey );
		else
			ChangeToTargetAttackPre( pOwner, pOwner->GetCreator(), "", AT_NORMAL, m_eDirKey );

		ChangeToTargetAttack( pOwner );
	}
	
	//m_AttackKeyType 타입 사용에 대한 부분은 헤더 참조
	if( pOwner->IsAttackKeyDown() )
		m_AttackKeyType = AKT_PRESS;
	else
		m_AttackKeyType = AKT_NONE;
}

void ioDruidEagleFlyState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	DestroyReadyMarker( pOwner );
	DestroyAimedMarker( pOwner );

	ClearWeapon( pOwner );
	DestroyDummy( pOwner );

	pOwner->SetInvisibleState( false, true );

	if( pOwner->IsInvisibleFace() )
		pOwner->ShowFaceMesh( false );
}

bool ioDruidEagleFlyState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{	
	return true;
}

void ioDruidEagleFlyState::CheckSpecialState( ioBaseChar* pOwner )
{
	//m_AttackKeyType 타입 사용에 대한 부분은 헤더 참조
	if( !pOwner->IsAttackKeyDown() )
		m_AttackKeyType = AKT_NONE;

	ioItem* pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( pItem == NULL )
	{
		pOwner->SetState( CS_DELAY );	
		pOwner->SetFallState( true );
		return;
	}
	else
	{
		pOwner->SetInvisibleState( true, true, true );

		switch( m_SpecialFlyState )
		{
		case SFS_TARGET_FIND:
			OnFindTarget( pOwner );
			break;
		case SFS_TARGET_ATTACK_PRE:
			OnTargetAttackPre( pOwner );
			break;
		case SFS_TARGET_ATTACK:
			OnTargetAttack( pOwner );
			break;
		case SFS_UP:
			OnUp( pOwner );
			break;
		}
	}
}

void ioDruidEagleFlyState::ChangeToFindTarget( ioBaseChar* pOwner )
{
	m_SpecialFlyState	= SFS_TARGET_FIND;
	m_vPreDir			= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_vMoveDir			= m_vPreDir;
	m_dwStartTime		= FRAMEGETTIME();
	m_fCurrFlyRange		= 0.0f;
	
	//이전 마커 삭제
	DestroyReadyMarker( pOwner );
	DestroyAimedMarker( pOwner );

	//새 마커 등록
	CreateReadyMarker( pOwner );

	//더미 애니메이션 변경
	ioDruidEagleDummy* pDummy = GetDummy( pOwner );
	if( pDummy )
		pDummy->SetFly();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_TARGET_FIND;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidEagleFlyState::OnFindTarget( ioBaseChar* pOwner )
{
	UpdateReadyMarker( pOwner );
	UpdateAimedMarker( pOwner, m_TargetName );
	ProcessKeyCheck( pOwner );

	//방향키 입력 갱신
	pOwner->SetTargetRotToDirKey( m_eDirKey );
	m_vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	int eState = ProcessFlyMove( pOwner, m_FindTargetEagleFlyInfo, m_dwStartTime );
	if( eState == MS_MOVE || eState == MS_COLL )
	{	
		if( !pOwner->IsNeedProcess() )
			return;

		//아이템 게이지가 0 이상이면 타겟을 찾는다.
		if( CheckItemGauge( pOwner ) )
		{
			UpdateTarget( pOwner );
			
			//m_AttackKeyType 타입 사용에 대한 부분은 헤더 참조
			if( m_AttackKeyType == AKT_NONE )
			{
				if( pOwner->IsAttackKeyDown() )
				{
					ChangeToTargetAttackPre( pOwner, pOwner->GetCreator(), m_TargetName, AT_NORMAL, ioUserKeyInput::DKI_NONE );
					return;
				}
			}
			
			if( pOwner->IsDirKeyDoubleClick() )
			{
				ChangeToHumanJumpAttack( pOwner );
			}
		}
		//아이템 게이지가 0이되면 종료
		else
		{	
			ChangeToEnd( pOwner );
		}
	}
}

void ioDruidEagleFlyState::UpdateTarget( ioBaseChar* pOwner )
{
	FD_GrapplingAutoTarget kFindPred( pOwner, m_fTargetMinAngle, m_fTargetMaxAngle, m_fTargetMaxRange, false );
	ioBaseChar *pTarget = pOwner->GetCreator()->FindUser( kFindPred );
	if( pTarget )
	{
		//타겟이 바뀌었다면 에임 마커로 교체(갱신)
		if( m_TargetName != pTarget->GetCharName() )
		{
			if( m_dwTargetingStartTime + m_dwTargetingTime < FRAMEGETTIME() )
				ChangeTarget( pOwner, pTarget );
		}
	}
	else
	{
		//타겟이 없다면 레디 마커로 교체
		if( !m_TargetName.IsEmpty() )
		{
			RemoveTarget( pOwner );
		}
	}
}

void ioDruidEagleFlyState::ChangeTarget( ioBaseChar* pOwner, ioBaseChar* pTarget )
{
	m_TargetName			= pTarget->GetCharName();
	m_dwTargetingStartTime	= FRAMEGETTIME();

	DestroyAimedMarker( pOwner );
	DestroyReadyMarker( pOwner );
	CreateAimedMarker( pOwner, pTarget );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_UPDATE_TARGET;
		kPacket << m_TargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidEagleFlyState::RemoveTarget( ioBaseChar* pOwner )
{
	DestroyAimedMarker( pOwner );
	CreateReadyMarker( pOwner );
	m_TargetName.Clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_REMOVE_TARGET;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidEagleFlyState::ChangeToTargetAttackPre( ioBaseChar* pOwner, ioPlayStage* pStage, const ioHashString& szTargetName, AttackType eType, ioUserKeyInput::DirKeyInput eKey )
{
	m_SpecialFlyState		= SFS_TARGET_ATTACK_PRE;
	m_AttackType			= eType;
	m_dwTargetingStartTime	= 0;
	m_TargetName			= szTargetName;

	ioBaseChar* pTarget = pStage->GetBaseChar( szTargetName );
	if( pTarget )
	{
		m_vPreDir								= pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		m_TargetAttackEagleFlyInfo.m_fMaxRange	= D3DXVec3Length( &m_vPreDir );
	}
	else
	{
		m_vPreDir								= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		m_TargetAttackEagleFlyInfo.m_fMaxRange	= m_fReadyMarkerOffset;
	}

	if( eKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );
		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
		m_vPreDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}

	D3DXVec3Normalize( &m_vPreDir, &m_vPreDir );
	m_dwCharingStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_TARGET_ATTACK_PRE;
		kPacket	<< szTargetName;
		kPacket	<< (byte)eType;
		kPacket << (byte)eKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidEagleFlyState::OnTargetAttackPre( ioBaseChar* pOwner )
{
	int eState = ProcessFlyMove( pOwner, m_FindTargetEagleFlyInfo, m_dwStartTime );
	if( !pOwner->IsNeedProcess() )
		return;

	if( eState == MS_END )
	{
		ChangeToEnd( pOwner );
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwCharingStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToBear( pOwner );
		}
	}
	else
	{
		ChangeToTargetAttack( pOwner );
	}
}

void ioDruidEagleFlyState::ChangeToTargetAttack( ioBaseChar* pOwner )
{
	m_SpecialFlyState	= SFS_TARGET_ATTACK;
	m_vMoveDir			= m_vPreDir;

	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	
	m_dwStartTime	= FRAMEGETTIME();
	m_fCurrFlyRange	= 0.0f;
	m_TargetName.Clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_TARGET_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	D3DXVECTOR3 vRot = -m_vMoveDir;
	vRot.y = 0.0f;
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, vRot );
	pOwner->SetTargetRot( qtRot );

	ioDruidEagleDummy* pDummy = GetDummy( pOwner );
	if( pDummy )
		pDummy->SetDownFly();

	if( m_AttackType == AT_NORMAL )
	{
		m_dwCurrWeaponIdx = pOwner->GetWeaponIndexBase();
		FireTime kFireTime;
		kFireTime.dwStart				= FRAMEGETTIME();
		kFireTime.iAttributeIdx			= m_dwWeaponIdx;
		kFireTime.iResistanceIdx		= 0;
		kFireTime.szWoundedAni			= "";
		kFireTime.dwWoundedAniDuration	= 0;
		kFireTime.bLoopWoundedAni		= false;
		kFireTime.dwWeaponIdx			= m_dwCurrWeaponIdx;
		kFireTime.eFireTimeType			= FTT_EXTEND_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, pOwner->GetMidPositionByRate(), pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ), "" );
	}
}

void ioDruidEagleFlyState::OnTargetAttack( ioBaseChar* pOwner )
{
	int eState = ProcessFlyMove( pOwner, m_TargetAttackEagleFlyInfo, m_dwStartTime );
	if( eState == MS_END )
	{
		ClearWeapon( pOwner );
		ChangeToUp( pOwner );
		m_AttackType = AT_NONE;
	}
}

void ioDruidEagleFlyState::ChangeToUp( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fUpAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	m_vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	m_SpecialFlyState	= SFS_UP;
	m_dwStartTime		= FRAMEGETTIME();
	m_fCurrFlyRange		= 0.0f;

	DestroyAimedMarker( pOwner );

	ioDruidEagleDummy* pDummy = GetDummy( pOwner );
	if( pDummy )
	{
		pDummy->SetUpFly();
	}
}

void ioDruidEagleFlyState::OnUp( ioBaseChar* pOwner )
{
	if( ProcessFlyMove( pOwner, m_UpFlyInfo, m_dwStartTime ) == MS_END )
	{
		ChangeToFindTarget( pOwner );
	}
}

void ioDruidEagleFlyState::ProcessKeyCheck( ioBaseChar *pOwner, bool bSync /*= true */ )
{
	if( !pOwner->IsNeedProcess() )
		return;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( pOwner->IsJumpKeyDown() )
	{
		m_UpDownKey = UDK_UP;
	}
	else if( pOwner->IsDefenseKeyDown() && m_fCheckHeightMin <= fHeightGap )
	{
		m_UpDownKey = UDK_DOWN;
	}
	else
	{
		m_UpDownKey = UDK_NONE;
	}

	m_eDirKey = pOwner->CheckCurDirKey();

	if( bSync && ( m_eDirKey != ioUserKeyInput::DKI_NONE || m_UpDownKey != UDK_NONE ) && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_UPDOWN_KEY;
		kPacket	<< (byte)m_UpDownKey;
		kPacket << (int)m_eDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioDruidEagleFlyState::ProcessFlyMove( ioBaseChar *pOwner, const EagleFlyInfo& FlyInfo, DWORD dwStartTime )
{
	if( !pOwner )
		return MS_END;

	D3DXVECTOR3 vUp( 0.0f, FLOAT1, 0.0f );
	D3DXVECTOR3 VDown( 0.0f, -FLOAT1, 0.0f );

	DWORD dwCurTime		= FRAMEGETTIME();
	float fTimePerSec	= g_FrameTimer.GetSecPerFrame();

	//엑셀 가속
	float fCurSpeed = FlyInfo.m_fFlyMinSpeed;
	DWORD dwGapTime = dwCurTime - ( dwStartTime + FlyInfo.m_dwFlyMinSpeedDuration );
	if( dwStartTime + FlyInfo.m_dwFlyMinSpeedDuration < dwCurTime )
	{
		float fTimeRate = (float)dwGapTime / FLOAT1000;
		float fCurAccel = FlyInfo.m_fFlyAccelSpeed * fTimeRate;
		fCurSpeed += fCurAccel;
	}

	//최대 속도 못 넘어가게
	if( fCurSpeed >= FlyInfo.m_fFlyMaxSpeed )
		fCurSpeed = FlyInfo.m_fFlyMaxSpeed;

	float fDistance = fCurSpeed * fTimePerSec;

	//이동 거리 계산 및 이동 종료 체크
	MoveState eRangeLimit = MS_END;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fCurrFlyRange + fDistance < FlyInfo.m_fMaxRange )
	{
		eRangeLimit = MS_MOVE;
		vMoveAmt = m_vMoveDir * fDistance;
		m_fCurrFlyRange += fDistance;

		switch( m_UpDownKey )
		{
		case UDK_UP:
			vMoveAmt += vUp * fDistance;
			break;
		case UDK_DOWN:
			vMoveAmt += VDown * fDistance;
			break;
		}
		m_UpDownKey = UDK_NONE;
	}
	else
	{
		eRangeLimit = MS_END;

		float fGapDistance = FlyInfo.m_fMaxRange - m_fCurrFlyRange;
		vMoveAmt = m_vMoveDir * fGapDistance;
	}
	
	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
		else
			return eRangeLimit == MS_END ? MS_END : MS_COLL;
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
	{
		pOwner->SetWorldPosition( vPos );
	}

	float fHeightGap = pOwner->GetMidPositionByRate().y - pOwner->GetBottomHeight();
	if( fHeightGap <= 15.0f )
	{
		return MS_LANDING;
	}

	return eRangeLimit;
}

void ioDruidEagleFlyState::ChangeToBear( ioBaseChar* pOwner )
{
	m_dwStartTime		= FRAMEGETTIME();
	m_SpecialFlyState	= SFS_END;

	ioDruidItem* pItem = ToDruidItem( pOwner->GetWeapon() );
	if( pItem )
	{
		pItem->ChangeEquipObjectState( pOwner, ioDruidItem::EOS_JUMP );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_CHANGE_BEAR;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidEagleFlyState::ChangeToEnd( ioBaseChar* pOwner )
{
	m_dwStartTime		= FRAMEGETTIME();
	m_SpecialFlyState	= SFS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidEagleFlyState::ChangeToHumanJumpAttack( ioBaseChar* pOwner )
{
	ioAttackableItem* pItem = pOwner->GetWeapon();
	if( pItem == NULL )
		return;

	ioChargeComboJump3* pJump = ToChargeComboJump3( pItem->GetCurExtendJump() );
	if( pJump == NULL )
		return;
	
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetJumpPower( m_fChangeJumpPower );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurGravityGapAmt( 0.0f );
	pOwner->SetJumpState( false );
	pOwner->SetJumpAttackState();
	pJump->SetJumpping( pOwner, false );
	pJump->CheckComboJumpAttack( pOwner );
	pJump->SetComboJumpAttack( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_CHANGE_HUMAN;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioDruidEagleFlyState::CheckItemGauge( ioBaseChar* pOwner )
{
	ioDruidItem* pItem = ToDruidItem( pOwner->GetWeapon() );
	if( !pItem || pItem->GetCurBullet() <= 0 )
		return false;

	return true;
}

void ioDruidEagleFlyState::CreateReadyMarker( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vScale	= D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + m_fReadyMarkerOffset * vDir;

	ioMapEffect	*pMapEffect = pOwner->GetCreator()->CreateMapEffect( m_szReadyMarker, vPos, vScale );
	if( pMapEffect )
	{
		m_ReadyMarkerID = pMapEffect->GetUniqueID();
	}
}

void ioDruidEagleFlyState::UpdateReadyMarker( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + m_fReadyMarkerOffset * vDir;

	ioMapEffect	*pMapEffect = pOwner->GetCreator()->FindMapEffect( m_ReadyMarkerID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vPos );
	}
}

void ioDruidEagleFlyState::DestroyReadyMarker( ioBaseChar* pOwner )
{
	ioMapEffect	*pMapEffect = pOwner->GetCreator()->FindMapEffect( m_ReadyMarkerID );
	if( pMapEffect )
	{
		pOwner->GetCreator()->DestroyMapEffect( m_ReadyMarkerID );
		m_ReadyMarkerID = -1;
	}
}

void ioDruidEagleFlyState::CreateAimedMarker( ioBaseChar* pOwner, ioBaseChar* pTarget )
{
	ioMapEffect	*pMapEffect = pOwner->GetCreator()->CreateMapEffect( m_szAimedMarker, pTarget->GetMidPositionByRate(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pMapEffect )
	{
		m_AimedMarkerID = pMapEffect->GetUniqueID();
	}
}

void ioDruidEagleFlyState::UpdateAimedMarker( ioBaseChar* pOwner, const ioHashString& TargetName )
{
	if( TargetName.IsEmpty() )
		return;

	if( !pOwner->GetCreator() )
		return;

	ioBaseChar* pTarget = pOwner->GetCreator()->GetBaseChar( TargetName );
	if( !pTarget )
		return;

	ioMapEffect	*pMapEffect = pOwner->GetCreator()->FindMapEffect( m_AimedMarkerID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( pTarget->GetMidPositionByRate() );
	}

}

void ioDruidEagleFlyState::DestroyAimedMarker( ioBaseChar* pOwner )
{
	ioMapEffect	*pMapEffect = pOwner->GetCreator()->FindMapEffect( m_AimedMarkerID );
	if( pMapEffect )
	{
		pOwner->GetCreator()->DestroyMapEffect( m_AimedMarkerID );
		m_AimedMarkerID = -1;
	}
}

void ioDruidEagleFlyState::ClearWeapon( ioBaseChar* pOwner )
{
	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCurrWeaponIdx ) );
	if( pZone )
		pZone->SetWeaponDead();
}

bool ioDruidEagleFlyState::CheckWeaponLive( ioBaseChar *pOwner )
{	
	ioZoneEffectWeapon* pZoneWeapon = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCurrWeaponIdx ) );
	if( pZoneWeapon && pZoneWeapon->IsLive() )
		return true;

	return false;
}

void ioDruidEagleFlyState::CreateDummy( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vDir		= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos	= pOwner->GetMidPositionByRate();
	int iCurIndex			= g_DummyCharMgr.GetCurLocalIndex();
	ioPlayStage *pStage		= pOwner->GetCreator();

	if( pStage )
	{
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
		if( pDummy )
		{
			pDummy->SetMoveState();
			m_DummyCharIndex = iCurIndex;
		}
	}
}

void ioDruidEagleFlyState::DestroyDummy( ioBaseChar* pOwner )
{	
	if( pOwner && m_DummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_DummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_DummyCharIndex = 0;
	}
}

ioDruidEagleDummy* ioDruidEagleFlyState::GetDummy( ioBaseChar* pOwner )
{
	ioDummyChar* pDummyChar = g_DummyCharMgr.FindDummyChar( pOwner, m_DummyCharIndex );
	return ToDruidEagleDummy( pDummyChar );
}

bool ioDruidEagleFlyState::IsStateEnd( ioBaseChar* pOwner )
{
	 if( m_dwStartTime + 50 < FRAMEGETTIME() && m_SpecialFlyState == SFS_END )
		 return true;

	 return false;
}

void ioDruidEagleFlyState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{	
	if( m_SpecialFlyState == SFS_END )
	{
		if( m_fEndJumpPower > 0.0f )
		{
			pOwner->SetSKillEndJumpState( m_fEndJumpPower );
		}
		else
		{
			float fMinFallinRate = ( pOwner->GetWorldCollisionBox().GetExtents( 1 ) * 2.0f ) * pOwner->GetMinFallHeightRate();
			float fCharPosGap = pOwner->GetWorldPosition().y - fBottomHeight;
			if( fCharPosGap > fMinFallinRate )
				pOwner->SetFallState( true );
		}
	}
}

bool ioDruidEagleFlyState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}

void ioDruidEagleFlyState::FillJoinSync(  SP2Packet &rkPacket, ioBaseChar* pOwner )
{

}

void ioDruidEagleFlyState::ApplyJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{

}

void ioDruidEagleFlyState::ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	ioSpecialStateBase::ApplyStateSync( rkPacket, pOwner );

	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;
	rkPacket >> vPos >> qtRot;

	pOwner->SetWorldPosition( vPos );
	pOwner->SetWorldOrientation( qtRot );

	int eState;
	rkPacket >> eState;

	switch( eState )
	{
	case SS_TARGET_FIND:
		{
			ChangeToFindTarget( pOwner );
		}
		break;
	case SS_UPDATE_TARGET:
		{
			ioHashString szTarget;
			rkPacket >> szTarget;

			if( pOwner->GetCreator() )
			{
				ioBaseChar* pTarget = pOwner->GetCreator()->GetBaseChar( szTarget );
				if( pTarget )
					ChangeTarget( pOwner, pTarget );
			}
		}
		break;
	case SS_REMOVE_TARGET:
		{
			RemoveTarget( pOwner );
		}
		break;
	case SS_TARGET_ATTACK_PRE:
		{
			ioHashString szCharName;
			rkPacket >> szCharName;

			byte eAttackType, eKey;
			rkPacket >> eAttackType >> eKey;

			ChangeToTargetAttackPre( pOwner, pOwner->GetCreator(), szCharName, static_cast<AttackType>( eAttackType ), static_cast<ioUserKeyInput::DirKeyInput>( eKey ) );
		}
		break;
	case SS_TARGET_ATTACK:
		{
			ChangeToTargetAttack( pOwner );
		}
		break;
	case SS_UPDOWN_KEY:
		{
			byte eKey;
			rkPacket >> eKey;
			m_UpDownKey = static_cast<UpDownKey>( eKey );

			int eDirKey;
			rkPacket >> eDirKey;
			m_eDirKey = static_cast<DIRKEY>( eDirKey );
		}
		break;
	case SS_CHANGE_BEAR:
		{
			ChangeToBear( pOwner );
		}
		break;
	case SS_CHANGE_HUMAN:
		{
			ChangeToHumanJumpAttack( pOwner );
		}
		break;
	case SS_END:
		{
			ChangeToEnd( pOwner );
		}
		break;

	}
}