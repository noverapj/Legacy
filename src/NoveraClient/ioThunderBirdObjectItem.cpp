
#include "stdafx.h"

#include "ioThunderBirdObjectItem.h"
#include "ioThunderBirdItem.h"
#include "WeaponDefine.h"

ioThunderBirdObjectItem::ioThunderBirdObjectItem()
{
	Init();
}

ioThunderBirdObjectItem::ioThunderBirdObjectItem( const ioThunderBirdObjectItem &rhs )
	: ioObjectItem( rhs ),
      m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_dwProtectTime( rhs.m_dwProtectTime ),
	  m_fNormalUseGauge( rhs.m_fNormalUseGauge ),
	  m_szChargeDelay( rhs.m_szChargeDelay ),
	  m_fChargeDelayRate( rhs.m_fChargeDelayRate ),
	  m_szChargeFrontMove( rhs.m_szChargeFrontMove ),
	  m_fChargeFrontMoveRate( rhs.m_fChargeFrontMoveRate ),
	  m_szChargeBackMove( rhs.m_szChargeBackMove ),
	  m_fChargeBackMoveRate( rhs.m_fChargeBackMoveRate ),
	  m_szChargeRightMove( rhs.m_szChargeRightMove ),
	  m_fChargeRightMoveRate( rhs.m_fChargeRightMoveRate ),
	  m_szChargeLeftMove( rhs.m_szChargeLeftMove ),
	  m_fChargeLeftMoveRate( rhs.m_fChargeLeftMoveRate ),
	  m_ChargeFireInfo( rhs.m_ChargeFireInfo ),
	  m_dwReloadTime( rhs.m_dwReloadTime )
{
	Init();
}

ioThunderBirdObjectItem::~ioThunderBirdObjectItem()
{
}

void ioThunderBirdObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );
	
	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	m_AttackReadyAniList.clear();
	//공격 준비 애니메이션
	int iMaxCombo = GetMaxCombo();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_fNormalUseGauge = rkLoader.LoadFloat_e( "use_normal_gauge", 0.0f );

	rkLoader.LoadString_e( "charge_delay", "", szBuf, MAX_PATH );
	m_szChargeDelay = szBuf;
	m_fChargeDelayRate = rkLoader.LoadFloat_e( "charge_delay_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_front_move", "", szBuf, MAX_PATH );
	m_szChargeFrontMove = szBuf;
	m_fChargeFrontMoveRate = rkLoader.LoadFloat_e( "charge_front_move_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_back_move", "", szBuf, MAX_PATH );
	m_szChargeBackMove = szBuf;
	m_fChargeBackMoveRate = rkLoader.LoadFloat_e( "charge_back_move_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_right_move", "", szBuf, MAX_PATH );
	m_szChargeRightMove = szBuf;
	m_fChargeRightMoveRate = rkLoader.LoadFloat_e( "charge_right_move_rate", FLOAT1 );
	
	rkLoader.LoadString_e( "charge_left_move", "", szBuf, MAX_PATH );
	m_szChargeLeftMove = szBuf;
	m_fChargeLeftMoveRate = rkLoader.LoadFloat_e( "charge_left_move_rate", FLOAT1 );

	wsprintf_e( szKey, "charge_weapon_attribute_index", i+1 );
	m_ChargeFireInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "charge_weapon_attribute_resist_index", i+1 );
	m_ChargeFireInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );
		
	wsprintf_e( szKey, "charge_weapon_wounded_animation", i+1 );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_ChargeFireInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "charge_weapon_wounded_duration", i+1 );
	m_ChargeFireInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "charge_weapon_wounded_loop_ani", i+1 );
	m_ChargeFireInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	m_dwReloadTime = rkLoader.LoadInt_e( "charge_weapon_reload_time", 0 );
}

ioItem* ioThunderBirdObjectItem::Clone()
{
	return new ioThunderBirdObjectItem( *this );
}

ioObjectItem::ObjectSubType ioThunderBirdObjectItem::GetObjectSubType() const
{
	return OST_THUNDER_BIRD;
}

void ioThunderBirdObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();	

	Init();

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_nNormalCombo = 0;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_WeaponState = WS_NONE;
	}
	else
	{
		//공격 준비 애니메이션 설정
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwAttackStartTime = dwCurTime;
		m_nNormalCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_WeaponState = WS_CHECK_CHARGING;
	}
}

void ioThunderBirdObjectItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponState )
	{
	case WS_CHECK_CHARGING:
		CheckCharging( pOwner );
		break;	
	case WS_NORMAL_ATTACK:
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

bool ioThunderBirdObjectItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) 
		return false;

	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectAttachItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectAttachItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate = pAttr->m_fAttackAniRate;

	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioThunderBirdObjectItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );

	CheckAirJump( pOwner );
}

void ioThunderBirdObjectItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_NORMAL_ATTACK:
		{
			rkPacket >> m_nNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			SetNormalAttackState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_INIT:
		{
			pOwner->SetThunderBirdObjectSpecialState();
		}
		break;
	case WS_SPECIAL_STATE_PROCESS:
		{
			RecvSpecialState( pOwner, rkPacket );
		}
		break;
	}
}

void ioThunderBirdObjectItem::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;	

	switch( nSpecialState )
	{
	case SAS_BOMB:
		SetBombState( pOwner );
		break;
	case SAS_END:
		{
			m_SpecialAttackState = SAS_END;
			pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

void ioThunderBirdObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );

	Init();

	//무적 시간 초기화
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckProtectTime = dwCurTime;
}

void ioThunderBirdObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioThunderBirdObjectItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwCheckProtectTime + m_dwProtectTime > FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioThunderBirdObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioThunderBirdObjectItem::IsEnableDash() const
{
	return true;
}

bool ioThunderBirdObjectItem::IsEnableDefense() const
{
	return false;
}

void ioThunderBirdObjectItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	bool bCheckSDSkillBuff = pOwner->HasBuff( BT_CHECK_STATE_END_ATTACK );
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if ( bCheckSDSkillBuff )				
				SetBombState( pOwner );
			else
				SetSpecialState( pOwner );
		}
	}
	else
		SetNormalAttackState( pOwner );
}

void ioThunderBirdObjectItem::SetNormalAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	if( !CheckGauge( pOwner ) )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_nNormalCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_nNormalCombo );
	m_WeaponState = WS_NORMAL_ATTACK;
	DecreaseGauge( pOwner, m_fNormalUseGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_WeaponState;
		kPacket << m_nNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioThunderBirdObjectItem::SetSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetThunderBirdObjectSpecialState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioThunderBirdObjectItem::DecreaseGauge( ioBaseChar *pOwner, float fUseGauge )
{
	if ( !pOwner )
		return;

	ioThunderBirdItem *pThunderBirdItem = ToThunderBirdItem( pOwner->GetWeapon() );
	if ( pThunderBirdItem )
	{
		int nCurBullet = pThunderBirdItem->GetCurBullet();
		int nNewBullet = nCurBullet - fUseGauge;
		if ( nNewBullet <= 0 )
			nNewBullet = 0;

		pThunderBirdItem->SetCurBullet( nNewBullet );
	}
}

bool ioThunderBirdObjectItem::CheckGauge( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioThunderBirdItem *pThunderBirdItem = ToThunderBirdItem( pOwner->GetWeapon() );
	if ( pThunderBirdItem )
	{
		int nCurBullet = pThunderBirdItem->GetCurBullet();
		return (nCurBullet != 0);
	}
	return false;
}

void ioThunderBirdObjectItem::Init()
{
	m_WeaponState = WS_NONE;
	m_SpecialAttackState = SAS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
		
	m_dwCheckProtectTime = 0;
	m_dwAttackStartTime = 0;

	m_StartDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetDelay = false;
	m_dwCheckReloadTime = 0;
}

void ioThunderBirdObjectItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if ( CheckInputKey( pOwner ) || CheckThunderBirdGauge( pOwner ) )
	{
		SetSpecialEndState( pOwner );
		return;
	}
	
	switch( m_SpecialAttackState )
	{
	case SAS_NONE:
		SetMoveState( pOwner );
		break;
	case SAS_MOVE:
		CheckMoveAni( pOwner );
		CreateWeapon( pOwner );
		break;
	case SAS_END:
		break;
	}
}

bool ioThunderBirdObjectItem::IsEnableMoveState()
{
	if( m_SpecialAttackState == SAS_MOVE )
		return true;

	return false;
}

void ioThunderBirdObjectItem::SetBombState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioThunderBirdItem *pThunderBirdItem = ToThunderBirdItem( pOwner->GetWeapon() );
	if ( pThunderBirdItem )
		pThunderBirdItem->ChangeToBombState( pOwner, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SAS_BOMB;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioThunderBirdObjectItem::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_StartDirKey = pOwner->GetCurDirKey();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if ( pOwner->IsSettedDirection() )
	{
		m_bSetDelay = false;

		ioUserKeyInput::DirKeyInput eXformDir;
		eXformDir = pOwner->GetXformDir( m_StartDirKey );

		switch( eXformDir )
		{
		case ioUserKeyInput::DKI_UP:
			pGrp->SetLoopAni( m_szChargeFrontMove, FLOAT100, FLOAT1, FLOAT1/m_fChargeFrontMoveRate );
			break;
		case ioUserKeyInput::DKI_RIGHTUP:
		case ioUserKeyInput::DKI_RIGHT:
		case ioUserKeyInput::DKI_RIGHTDOWN:
			pGrp->SetLoopAni( m_szChargeRightMove, FLOAT100, FLOAT1, FLOAT1/m_fChargeRightMoveRate );
			break;
		case ioUserKeyInput::DKI_DOWN:
			pGrp->SetLoopAni( m_szChargeBackMove, FLOAT100, FLOAT1, FLOAT1/m_fChargeBackMoveRate );
			break;
		case ioUserKeyInput::DKI_LEFTDOWN:
		case ioUserKeyInput::DKI_LEFT:
		case ioUserKeyInput::DKI_LEFTUP:
			pGrp->SetLoopAni( m_szChargeLeftMove, FLOAT100, FLOAT1, FLOAT1/m_fChargeLeftMoveRate );
			break;
		}
	}
	else
	{
		m_bSetDelay = true;
		pGrp->SetLoopAni( m_szChargeDelay, FLOAT100, FLOAT1, FLOAT1/m_fChargeDelayRate );
	}

	m_SpecialAttackState = SAS_MOVE;

	if( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( true );
}

void ioThunderBirdObjectItem::CheckMoveAni( ioBaseChar *pOwner )
{
	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir )
		{	
			m_bSetDelay = false;

			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_StartDirKey );

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_szChargeFrontMove, FLOAT100, FLOAT1, FLOAT1/m_fChargeFrontMoveRate );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_szChargeRightMove, FLOAT100, FLOAT1, FLOAT1/m_fChargeRightMoveRate );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_szChargeBackMove, FLOAT100, FLOAT1, FLOAT1/m_fChargeBackMoveRate );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_szChargeLeftMove, FLOAT100, FLOAT1, FLOAT1/m_fChargeLeftMoveRate );
				break;
			}
		}
	}
	else
	{		
		if( !m_bSetDelay )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szChargeDelay, FLOAT100, FLOAT1, FLOAT1/m_fChargeDelayRate );

			m_bSetDelay = true;
		}
	}
}

void ioThunderBirdObjectItem::SetSpecialEndState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	m_SpecialAttackState = SAS_END;

	pOwner->SetState( CS_DELAY );
	
	if( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SAS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioThunderBirdObjectItem::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckReloadTime + m_dwReloadTime > dwCurTime )
		return;

	pOwner->NormalAttackFireExplicit( m_ChargeFireInfo );
	m_dwCheckReloadTime = dwCurTime;
}

bool ioThunderBirdObjectItem::CheckInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return true;

	if ( !pOwner->IsNeedProcess() )
		return false;
	
	if ( pOwner->IsJumpKeyDown() )
	{
		pOwner->SetJumpState();
		m_SpecialAttackState = SAS_END;
		return false;
	}

	if ( !pOwner->IsAttackKeyDown() )
		return true;

	return false;
}

bool ioThunderBirdObjectItem::CheckThunderBirdGauge( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return true;

	ioThunderBirdItem *pThunderBirdItem = ToThunderBirdItem( pOwner->GetWeapon() );
	if ( !pThunderBirdItem )
		return true;

	if ( pThunderBirdItem->GetCurBullet() <= 0 )
		return true;

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
ioThunderBirdObjectSpecialState::ioThunderBirdObjectSpecialState()
{
}

ioThunderBirdObjectSpecialState::~ioThunderBirdObjectSpecialState()
{
}

void ioThunderBirdObjectSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioThunderBirdObjectSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioThunderBirdObjectSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioThunderBirdObjectItem *pThunderBirdObjectItem = ToThunderBirdObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pThunderBirdObjectItem )
		pThunderBirdObjectItem->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioThunderBirdObjectSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioThunderBirdObjectSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	pOwner->ChangeDirectionByInputDir( true );
}