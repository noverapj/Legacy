

#include "stdafx.h"

#include "ioGalaxyItem.h"
#include "ioGalaxyObjectItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioGalaxyItem::ioGalaxyItem()
{
	Init();
}

ioGalaxyItem::ioGalaxyItem( const ioGalaxyItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGPGauge( rhs.m_fNeedGPGauge ),	
	m_dwObjectItem( rhs.m_dwObjectItem ),
	m_ObjectEquipAnimation( rhs.m_ObjectEquipAnimation ),
	m_fObjectEquipAnimationRate( rhs.m_fObjectEquipAnimationRate ),
	m_fGatheringScaleRate( rhs.m_fGatheringScaleRate ),
	m_fGatheringScaleMaxRate( rhs.m_fGatheringScaleMaxRate ),
	m_PreGatherAnimationList( rhs.m_PreGatherAnimationList ),
	m_GatherAnimation( rhs.m_GatherAnimation ),	
	m_GatherMeteoEffect( rhs.m_GatherMeteoEffect ),
	m_ExtendMoveFront( rhs.m_ExtendMoveFront ),
	m_fExtendMoveAniRate( rhs.m_fExtendMoveAniRate ),
	m_fMoveSpeedRateExtend( rhs.m_fMoveSpeedRateExtend ),
	m_fHeightOffSet( rhs.m_fHeightOffSet )
{
	Init();
}

ioGalaxyItem::~ioGalaxyItem()
{
}

void ioGalaxyItem::Init()
{
	m_ChargeState				= CS_NONE;

	m_dwAttackStartTime			= 0;
	m_iCurCombo					= 0;
	m_iCurAirJump				= 0;
	m_dwGatherEffectID			= 0;
	m_dwGatherMeteoEffectID		= 0;

	m_fAirJumpPowerRate			= 0.0f;
	m_fCurExtraGauge			= 0.0f;
	m_fCurrScaleRate			= FLOAT1;
		
	m_bFirstExtendMoveMotion	= false;
	m_bExtendMotionSetted		= false;
}

void ioGalaxyItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];	
	m_fNeedGPGauge				= rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );	
	m_fGatheringScaleRate		= rkLoader.LoadFloat_e( "gather_scale_rate", FLOAT1 );
	m_fGatheringScaleMaxRate	= rkLoader.LoadFloat_e( "gather_scale_max_rate", 4.0f );
	m_dwObjectItem				= static_cast<DWORD>( rkLoader.LoadInt_e( "object_item", 0 ) );

	rkLoader.LoadString_e( "object_equip_animation", "", szBuf, MAX_PATH );
	m_ObjectEquipAnimation = szBuf;
	m_fObjectEquipAnimationRate	= rkLoader.LoadFloat_e( "object_equip_animation_rate", FLOAT1 );

	int iPreCount = rkLoader.LoadInt_e( "pre_gather_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szBuf, "pre_gather_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_PreGatherAnimationList.push_back( ioHashString( szBuf ) );
	}

	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;

	rkLoader.LoadString_e( "gather_meteo_effect", "", szBuf, MAX_PATH );
	m_GatherMeteoEffect = szBuf;

	m_fHeightOffSet = rkLoader.LoadFloat_e( "gather_meteo_height_offset", FLOAT100 );

	rkLoader.LoadString_e( "extend_move_front", "", szBuf, MAX_PATH );
	m_ExtendMoveFront = szBuf;	

	m_fExtendMoveAniRate	= rkLoader.LoadFloat_e( "extend_move_ani_rate", FLOAT1 );
	m_fMoveSpeedRateExtend	= rkLoader.LoadFloat_e( "extend_move_speed_rate", 0.f );
}

ioItem* ioGalaxyItem::Clone()
{
	return new ioGalaxyItem( *this );
}

ioWeaponItem::WeaponSubType ioGalaxyItem::GetSubType() const
{
	return WST_GALAXY_ITEM;
}

void ioGalaxyItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseObject( pOwner );

	m_fCurExtraGauge	= 0.0f;
	m_fCurrScaleRate	= FLOAT1;

	ioAttackableItem::OnReleased( pOwner );
}

void ioGalaxyItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
		
	int iPreGatherCnt = m_PreGatherAnimationList.size();
	
	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;
	m_bChangeDir		= false;

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

void ioGalaxyItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
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

	CheckExtendMoveAniState( pOwner );
}

void ioGalaxyItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && pOwner->IsEquipEnableObject() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToGatheringState( pOwner );
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioGalaxyItem::ResetGathering( ioBaseChar* pOwner )
{
	pOwner->EndEffect( m_dwGatherEffectID );
	pOwner->EndMapEffect( m_dwGatherMeteoEffectID );
	pOwner->SetAttackMoveEnable( false );

	m_ChargeState = CS_EQUIP_OBJECT;
	EquipObjectItem( pOwner );	
}

void ioGalaxyItem::OnGathering( ioBaseChar *pOwner )
{
	DWORD dwCurTinme	= FRAMEGETTIME();
	float fOffset		= m_fHeightOffSet;
	bool bNoKeyDown		= false;
	ioPlayStage* pStage	= pOwner->GetCreator();	
	if( !pStage )
		return;

	if( pOwner->IsNeedProcess() )
	{
		fOffset += 50.0f;
		if( !pOwner->IsAttackKeyDown() || m_fCurExtraGauge == m_fMaxExtraGauge )
		{
			bNoKeyDown = true;
		}
	}

	if( bNoKeyDown )
	{		
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_RESET_GATHER );
			kPacket << pOwner->GetTargetRot();
			kPacket << m_fCurrScaleRate;
			kPacket << GetCurBullet();
			P2PNetwork::SendToAllPlayingUser( kPacket );

			ResetGathering( pOwner );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ResetGathering( pOwner );
		}
	}
	else
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		m_fCurrScaleRate = m_fCurrScaleRate + m_fGatheringScaleRate * fTimePerSec;
		m_fCurrScaleRate = min( m_fCurrScaleRate, m_fGatheringScaleMaxRate );
		
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwGatherMeteoEffectID );
		if( pMapEffect )
		{
			D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y + fOffset, pOwner->GetWorldPosition().z );
			pMapEffect->SetWorldPosition( vPos );

			ioEffect* pEffect = pMapEffect->GetEffect();
			if( pEffect )
				pEffect->GetParentNode()->SetScale( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate );
		}
	}
}

void ioGalaxyItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

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

void ioGalaxyItem::ChangeToGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );
	
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	ioEffect* pEffect = NULL;
	pEffect = pOwner->AttachEffect( m_GatherEffect );
	if( pEffect )
		m_dwGatherEffectID = pEffect->GetUniqueID();
	
	ioPlayStage* pStage = pOwner->GetCreator();
	if( pStage )
	{
		D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y + m_fHeightOffSet, pOwner->GetWorldPosition().z );
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_GatherMeteoEffect, vPos, D3DXVECTOR3( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate ) );
		if( pMapEffect )
		{
			m_dwGatherMeteoEffectID = pMapEffect->GetUniqueID();
		}
	}

	m_ChargeState				= CS_GATHERING;
	m_bFirstExtendMoveMotion	= true;
	m_bExtendMotionSetted		= true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_GATHER );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGalaxyItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case ST_GATHER:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToGatheringState( pOwner );
		}
		break;
	case ST_RESET_GATHER:
		{
			int nBullet;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;			
			rkPacket >> m_fCurrScaleRate;
			rkPacket >> nBullet;

			SetCurBullet( nBullet );
			pOwner->SetTargetRot( qtRot );

			ResetGathering( pOwner );
		}
		break;
	}	
}

bool ioGalaxyItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioGalaxyItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

bool ioGalaxyItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;
		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime + 1;

			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

bool ioGalaxyItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "%s - AttackAttribute Not Exist", __FUNCTION__ );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - JumpAttack Animation Not Exist", __FUNCTION__ );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioGalaxyItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioGalaxyItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( m_fExtraGaugeBaseInc <= 0.0f )
		return;

	if( pOwner->IsSystemState() )
		return;

	if( m_ChargeState == CS_GATHERING )
	{
		float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
		m_fCurExtraGauge += fGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
	}
}

int ioGalaxyItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioGalaxyItem::GetNeedBullet()
{
	return (int)m_fNeedGPGauge;
}

int ioGalaxyItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioGalaxyItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

bool ioGalaxyItem::IsEquipObject( ioBaseChar *pOwner )
{
	if( pOwner && m_dwObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
		{
			return true;
		}
	}

	return false;
}

void ioGalaxyItem::EquipObjectItem( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetWereWolfState( m_ObjectEquipAnimation, m_fObjectEquipAnimationRate );
	
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_dwObjectItem > 0 )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

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

				//스케일값을 전달 후 장착을 통해 오브젝트 아이템 내부의 equip effect를 스케일링함				
				pObject->SetObjectCreateItem( GetName() );
				pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				pOwner->EquipItem( pObject );
			}
		}
	}
}

void ioGalaxyItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	if( m_ChargeState == CS_EQUIP_OBJECT )
	{
		pOwner->EndEffect( m_dwGatherEffectID );
		pOwner->EndMapEffect( m_dwGatherMeteoEffectID );

		ReleaseObject( pOwner );
	}
	else
	{
		pOwner->EndEffect( m_dwGatherEffectID );
		pOwner->EndMapEffect( m_dwGatherMeteoEffectID );

		m_ChargeState = CS_NONE;
		m_fCurExtraGauge = 0.0f;
		m_fCurrScaleRate = FLOAT1;
	}
}

void ioGalaxyItem::EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem )
{	
	ioObjectItem *pObject = ToObjectItem( pObjectItem );
	if( !pObject )
		return;

	if( m_dwObjectItem != pObject->GetItemCode() )
		return;

	//스케일/인덱스 값 전달
	ioGalaxyObjectItem* pObjectGalaxy = ToGalaxyObjectItem( pObject );
	if( pObjectGalaxy )
	{
		pObjectGalaxy->SetEquipMeshScaleRate( m_fCurrScaleRate );
		pObjectGalaxy->SetCurrFireIndex( GetCurBullet() );
		pObjectGalaxy->SetCurrHeightOffSet( m_fHeightOffSet );
	}
}

void ioGalaxyItem::ReleaseObject( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->EndEffect( m_dwGatherEffectID );
	pOwner->EndMapEffect( m_dwGatherMeteoEffectID );

	if( pOwner && m_dwObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__ );

			m_fCurExtraGauge = 0.0f;
			m_fCurrScaleRate = FLOAT1;
			m_ChargeState = CS_NONE;
		}
	}
}

void ioGalaxyItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioGalaxyItem::CheckExtendMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstExtendMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_ExtendMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );

			m_bExtendMotionSetted	 = false;
			m_bFirstExtendMoveMotion = false;

			if( bChangeDir )
				m_bChangeDir = true;

			pGrp->SetLoopAni( m_ExtendMoveFront, FLOAT100, FLOAT1, FLOAT1/m_fExtendMoveAniRate );

			g_TutorialMgr.ActionAimingMove();
		}
		else
		{
			m_bChangeDir = false;
		}
	}
	else
	{
		if( !m_bExtendMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_ExtendMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bExtendMotionSetted = true;
			m_bChangeDir = false;
		}
	}
}

void ioGalaxyItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{
	m_fCurExtraGauge = 0.0f;
	m_fCurrScaleRate = FLOAT1;
	m_ChargeState = CS_NONE;

	pOwner->EndEffect( m_dwGatherEffectID );
	pOwner->EndMapEffect( m_dwGatherMeteoEffectID );
}

void ioGalaxyItem::FillGalaxyInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket << static_cast<BYTE>( m_ChargeState );
	rkPacket << m_fCurExtraGauge;
	rkPacket << m_fCurrScaleRate;
}

void ioGalaxyItem::SetGalaxyInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE eState = 0;
	float fGauge = 0;
	float fSacleRate = 0.0f;
	rkPacket >> eState >> m_fCurExtraGauge >> m_fCurrScaleRate;
	m_ChargeState = static_cast<ChargeState>( eState );

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && eState == CS_EQUIP_OBJECT )
	{
		ioItem *pItem = pStage->CreateItem( m_dwObjectItem );
		ioObjectItem *pObject = ToObjectItem( pItem );
		if( pObject )
		{
			pObject->ReLoadProperty();
			EquipObjectItemByAttakable( pOwner, pObject );

			//스케일값을 전달 후 장착을 통해 오브젝트 아이템 내부의 equip effect를 스케일링함			
			pObject->SetObjectCreateItem( GetName() );
			pOwner->EquipItem( pObject );
		}
	}
}

float ioGalaxyItem::GetAttackSpeedRate() const
{
	if( m_ChargeState == CS_GATHERING )
		return m_fMoveSpeedRateExtend;

	return FLOAT1;
}

void ioGalaxyItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}