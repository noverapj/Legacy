

#include "stdafx.h"

#include "ioThunderBirdItem.h"
#include "ioThunderBirdObjectItem.h"
#include "WeaponDefine.h"

ioThunderBirdItem::ioThunderBirdItem()
{
	Init();
}

ioThunderBirdItem::ioThunderBirdItem( const ioThunderBirdItem &rhs )
	: ioWeaponItem( rhs ),
	  m_fNeedExtraGauge( rhs.m_fNeedExtraGauge ),	
	  m_dwObjectItem( rhs.m_dwObjectItem ),
	  m_ObjectEquipAnimation( rhs.m_ObjectEquipAnimation ),
	  m_fObjectEquipAnimationRate( rhs.m_fObjectEquipAnimationRate ),
	  m_PreGatherAnimationList( rhs.m_PreGatherAnimationList ),
	  m_PreGatherAnimationListW( rhs.m_PreGatherAnimationListW ),
	  m_GatherAnimation( rhs.m_GatherAnimation ),	
	  m_ExtendMoveFront( rhs.m_ExtendMoveFront ),
	  m_GatherAnimationW( rhs.m_GatherAnimationW ),	
	  m_ExtendMoveFrontW( rhs.m_ExtendMoveFrontW ),
	  m_fExtendMoveAniRate( rhs.m_fExtendMoveAniRate ),
	  m_fMoveSpeedRateExtend( rhs.m_fMoveSpeedRateExtend ),
	  m_szSDMotion( rhs.m_szSDMotion ),
	  m_fSDMotionRate( rhs.m_fSDMotionRate )
{
	Init();
}

ioThunderBirdItem::~ioThunderBirdItem()
{
}

void ioThunderBirdItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];	
	m_fNeedExtraGauge = rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );
	m_dwObjectItem = static_cast<DWORD>( rkLoader.LoadInt_e( "object_item", 0 ) );

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

	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szBuf, "pre_gather_animation_w%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_PreGatherAnimationListW.push_back( ioHashString( szBuf ) );
	}

	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "gather_animation_w", "", szBuf, MAX_PATH );
	m_GatherAnimationW = szBuf;

	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;

	rkLoader.LoadString_e( "extend_move_front", "", szBuf, MAX_PATH );
	m_ExtendMoveFront = szBuf;	

	rkLoader.LoadString_e( "extend_move_front_w", "", szBuf, MAX_PATH );
	m_ExtendMoveFrontW = szBuf;	

	m_fExtendMoveAniRate	= rkLoader.LoadFloat_e( "extend_move_ani_rate", FLOAT1 );
	m_fMoveSpeedRateExtend	= rkLoader.LoadFloat_e( "extend_move_speed_rate", 0.f );

	rkLoader.LoadString_e( "special_blow_ani", "", szBuf, MAX_PATH );
	m_szSDMotion = szBuf;	

	m_fSDMotionRate = rkLoader.LoadFloat_e( "special_blow_ani_rate", FLOAT1 );
}

ioItem* ioThunderBirdItem::Clone()
{
	return new ioThunderBirdItem( *this );
}

ioWeaponItem::WeaponSubType ioThunderBirdItem::GetSubType() const
{
	return WST_THUNDER_BIRD_ITEM;
}

void ioThunderBirdItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();		
	
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

	int iPreGatherCnt = m_PreGatherAnimationList.size();
	if ( !pOwner->IsMale() )
		iPreGatherCnt = m_PreGatherAnimationListW.size();

	ioHashString szPreAni;
	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		szPreAni = m_PreGatherAnimationList[iCurCombo];

		if ( !pOwner->IsMale() )
			szPreAni = m_PreGatherAnimationListW[iCurCombo];

		int iAniID = pGrp->GetAnimationIdx( szPreAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioThunderBirdItem::CheckNormalAttack( ioBaseChar *pOwner )
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

bool ioThunderBirdItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
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

void ioThunderBirdItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioThunderBirdItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

void ioThunderBirdItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioThunderBirdItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( m_fExtraGaugeBaseInc <= 0.0f )
		return;

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_THUNDER_BIRD_OBJECT_SPECIAL:
		{
			float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
			m_fCurExtraGauge -= fGauge;
			m_fCurExtraGauge = max( m_fCurExtraGauge, 0.0f );
		}
		break;
	default:
		if( m_ChargeState == CS_GATHERING )
		{
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		break;
	}
}

float ioThunderBirdItem::GetAttackSpeedRate() const
{
	if( m_ChargeState == CS_GATHERING )
		return m_fMoveSpeedRateExtend;

	return FLOAT1;
}

bool ioThunderBirdItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioThunderBirdItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

bool ioThunderBirdItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) 
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return false;

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

void ioThunderBirdItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseObject( pOwner );

	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioThunderBirdItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{
	pOwner->EndEffect( m_dwGatherEffectID );

	m_bEquipObjectItem = false;
	m_fCurExtraGauge = 0.0f;
	m_ChargeState = CS_NONE;
}

void ioThunderBirdItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
			rkPacket >> nBullet;

			SetCurBullet( nBullet );
			pOwner->SetTargetRot( qtRot );

			ResetGathering( pOwner );
		}
		break;
	case ST_BOMB_MOTION:
		ChangeToBombState( pOwner, false );
		break;
	case ST_RESET_ATTACK_MOVE:
		{
			pOwner->SetAttackMoveEnable( false );
		}
		break;
	}	
}

int ioThunderBirdItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioThunderBirdItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioThunderBirdItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

int ioThunderBirdItem::GetNeedBullet()
{
	return (int)m_fNeedExtraGauge;
}

void ioThunderBirdItem::Init()
{
	m_ChargeState				= CS_NONE;

	m_dwAttackStartTime			= 0;
	m_iCurCombo					= 0;
	m_iCurAirJump				= 0;
	m_dwGatherEffectID			= 0;

	m_fAirJumpPowerRate			= 0.0f;
	m_fCurExtraGauge			= 0.0f;

	m_bFirstExtendMoveMotion	= false;
	m_bExtendMotionSetted		= false;
	m_bEquipObjectItem = false;

	m_dwSDMotionEndTime = 0;
}

void ioThunderBirdItem::ResetGathering( ioBaseChar* pOwner )
{
	pOwner->EndEffect( m_dwGatherEffectID );
	pOwner->SetAttackMoveEnable( false );

	m_ChargeState = CS_EQUIP_OBJECT;
	EquipObjectItem( pOwner );	
}

void ioThunderBirdItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	bool bCheckSDSkillBuff = pOwner->HasBuff( BT_CHECK_STATE_END_ATTACK );
	if( pOwner->IsAttackKeyDown() && ( pOwner->IsEquipEnableObject() || bCheckSDSkillBuff ) )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if ( bCheckSDSkillBuff )
				ChangeToBombState( pOwner, true );
			else
				ChangeToGatheringState( pOwner );
		}
	}
	else
		ChangeToAttackFire( pOwner );
}

void ioThunderBirdItem::OnGathering( ioBaseChar *pOwner )
{
	DWORD dwCurTinme	= FRAMEGETTIME();
	bool bNoKeyDown		= false;

	ioPlayStage* pStage	= pOwner->GetCreator();	
	if( !pStage )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( ( m_fCurExtraGauge >= m_fNeedExtraGauge && !pOwner->IsAttackKeyDown() ) || m_fCurExtraGauge == m_fMaxExtraGauge )
			bNoKeyDown = true;
		else if ( !pOwner->IsAttackKeyDown() )
		{
			m_fCurExtraGauge = 0.0f;
			pOwner->SetAttackMoveEnable( false );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<byte>( ST_RESET_ATTACK_MOVE );
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			ChangeToAttackFire( pOwner );
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
			kPacket << GetCurBullet();
			P2PNetwork::SendToAllPlayingUser( kPacket );

			ResetGathering( pOwner );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ResetGathering( pOwner );
		}
	}
}

void ioThunderBirdItem::ChangeToGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioHashString szCurGatherAni = m_GatherAnimation;
	if ( !pOwner->IsMale() )
		szCurGatherAni = m_GatherAnimationW;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( szCurGatherAni, FLOAT100 );
	
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	ioEffect* pEffect = NULL;
	pEffect = pOwner->AttachEffect( m_GatherEffect );
	if( pEffect )
		m_dwGatherEffectID = pEffect->GetUniqueID();
	
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

void ioThunderBirdItem::ChangeToAttackFire( ioBaseChar *pOwner )
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

void ioThunderBirdItem::CheckExtendMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	ioHashString szCurExtendMove = m_ExtendMoveFront;
	if ( !pOwner->IsMale() )
		szCurExtendMove = m_ExtendMoveFrontW;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstExtendMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx( szCurExtendMove );
			pGrp->ClearLoopAni( iAniID, FLOAT100 );

			m_bExtendMotionSetted	 = false;
			m_bFirstExtendMoveMotion = false;

			if( bChangeDir )
				m_bChangeDir = true;

			pGrp->SetLoopAni( szCurExtendMove, FLOAT100, FLOAT1, FLOAT1/m_fExtendMoveAniRate );

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
			iAniID = pGrp->GetAnimationIdx( szCurExtendMove );
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bExtendMotionSetted = true;
			m_bChangeDir = false;
		}
	}
}

bool ioThunderBirdItem::IsEquipObject( ioBaseChar *pOwner )
{
	if( pOwner && m_dwObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
			return true;
	}

	return false;
}

void ioThunderBirdItem::EquipObjectItem( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetWereWolfState( m_ObjectEquipAnimation, m_fObjectEquipAnimationRate );
	m_bEquipObjectItem = true;

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
								
				pObject->SetObjectCreateItem( GetName() );
				pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				pOwner->EquipItem( pObject );
			}
		}
	}
}

void ioThunderBirdItem::ReleaseObject( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->EndEffect( m_dwGatherEffectID );

	if( pOwner && m_dwObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__ );

			m_bEquipObjectItem = false;
			m_fCurExtraGauge = 0.0f;
			m_ChargeState = CS_NONE;
		}
	}
}

void ioThunderBirdItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	if( m_ChargeState == CS_EQUIP_OBJECT )
	{
		pOwner->EndEffect( m_dwGatherEffectID );

		ReleaseObject( pOwner );
	}
	else
	{
		pOwner->EndEffect( m_dwGatherEffectID );

		m_ChargeState = CS_NONE;
		m_fCurExtraGauge = 0.0f;
	}
}

void ioThunderBirdItem::FillThunderBirdInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket << static_cast<BYTE>( m_ChargeState );
	rkPacket << m_fCurExtraGauge;
}

void ioThunderBirdItem::SetThunderBirdInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE eState = 0;
	float fGauge = 0;
	float fSacleRate = 0.0f;
	rkPacket >> eState >> m_fCurExtraGauge;
	m_ChargeState = static_cast<ChargeState>( eState );

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && eState == CS_EQUIP_OBJECT )
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
		pOwner->SetWereWolfState( m_ObjectEquipAnimation, m_fObjectEquipAnimationRate );
		m_bEquipObjectItem = true;
	}
}

bool ioThunderBirdItem::CheckReleaseObjectItem( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !pOwner->IsNeedProcess() )
		return false;
	
	if ( !m_bEquipObjectItem )
		return false;

	if( GetCurBullet() > 0 )
		return false;

	switch ( pOwner->GetState() )
	{
	case CS_ATTACK:
	case CS_JUMP:
	case CS_EXTEND_DASH:
	case CS_THUNDER_BIRD_OBJECT_SPECIAL:
		return false;
	}

	ReleaseObject( pOwner );

	return true;
}

void ioThunderBirdItem::ChangeToBombState( ioBaseChar *pOwner, bool bSendPacket )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szSDMotion );
	float fTimeRate = m_fSDMotionRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwSDMotionEndTime = dwCurTime;
	m_dwSDMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetState( CS_THUNDER_BIRD_BOMB_STATE );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_ChargeState = CS_BOMB_MOTION;

	if( bSendPacket && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_BOMB_MOTION );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioThunderBirdItem::ProcessBombState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->GetState() != CS_THUNDER_BIRD_BOMB_STATE )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwSDMotionEndTime < dwCurTime )
		pOwner->SetState( CS_DELAY );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
ioThunderBirdBombSpecialState::ioThunderBirdBombSpecialState()
{
}

ioThunderBirdBombSpecialState::~ioThunderBirdBombSpecialState()
{
}

void ioThunderBirdBombSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioThunderBirdBombSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioThunderBirdBombSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioThunderBirdItem *pThunderBirdItem = ToThunderBirdItem( pOwner->GetWeapon() );
	if( pThunderBirdItem )
		pThunderBirdItem->ProcessBombState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}