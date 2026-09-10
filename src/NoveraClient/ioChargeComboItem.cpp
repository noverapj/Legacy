

#include "stdafx.h"

#include "ioChargeComboItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioChargeComboItem::ioChargeComboItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_dwComboAttackTimeEnd = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedExtraAttack = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRetreatEndTime = 0;
}

ioChargeComboItem::ioChargeComboItem( const ioChargeComboItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
  m_vHalfChargeComboAttList( rhs.m_vHalfChargeComboAttList ),
  m_ExtraAttackAttribute( rhs.m_ExtraAttackAttribute ),
  m_CancelDashAttribute( rhs.m_CancelDashAttribute ),
  m_SpecialAttribute( rhs.m_SpecialAttribute ),
  m_vDodgeAniList( rhs.m_vDodgeAniList ),
  m_fDodgeAniRate( rhs.m_fDodgeAniRate ),
  m_fDodgeForce( rhs.m_fDodgeForce ),
  m_fDodgeFrictionRate( rhs.m_fDodgeFrictionRate ),
  m_dwMaxComboDuration( rhs.m_dwMaxComboDuration ),
  m_RetreatAttribute( rhs.m_RetreatAttribute ),
  m_ExtraGaugeAttribute( rhs.m_ExtraGaugeAttribute ),
  m_bEnableExtraGauge( rhs.m_bEnableExtraGauge ),
  m_fMaxGauge( rhs.m_fMaxGauge ),
  m_fNeedGauge( rhs.m_fNeedGauge ),
  m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
  m_dwHalfChargeTime( rhs.m_dwHalfChargeTime ),
  m_MoveAniFr( rhs.m_MoveAniFr ),
  m_fMoveAniFrRate( rhs.m_fMoveAniFrRate ),
  m_MoveAniBk( rhs.m_MoveAniBk ),
  m_fMoveAniBkRate( rhs.m_fMoveAniBkRate ),
  m_MoveAniRt( rhs.m_MoveAniRt ),
  m_fMoveAniRtRate( rhs.m_fMoveAniRtRate ),
  m_MoveAniLt( rhs.m_MoveAniLt ),
  m_fMoveAniLtRate( rhs.m_fMoveAniLtRate ),
  m_fMoveForce( rhs.m_fMoveForce ),
  m_fMoveForceFriction( rhs.m_fMoveForceFriction )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_dwComboAttackTimeEnd = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedExtraAttack = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRetreatEndTime = 0;
	m_fCurGauge = 0.0f;
}

ioChargeComboItem::~ioChargeComboItem()
{
}

void ioChargeComboItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// ready ani
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	// combo att
	int iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt", 1 );
	m_vChargeComboAttList.clear();
	m_vChargeComboAttList.reserve( iComboMax );
	for( int i=0; i < iComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}

	int iHalfComboMax = rkLoader.LoadInt_e( "half_charge_combo_max_cnt", 1 );
	m_vHalfChargeComboAttList.clear();
	m_vHalfChargeComboAttList.reserve( iComboMax );
	for( int i=0; i < iHalfComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "half_charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vHalfChargeComboAttList.push_back( kAttribute );
	}

	m_dwMaxComboDuration = (DWORD)rkLoader.LoadInt_e( "charge_combo_max_time", 1 );

	// extend att ( key : defense )
	m_ExtraAttackAttribute.Init();
	LoadAttackAttribute_e( "charge_extra_attack", m_ExtraAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "cancel_dash", m_CancelDashAttribute, rkLoader );
	LoadAttackAttribute_e( "special_attack", m_SpecialAttribute, rkLoader );

	// retreat
	LoadAttackAttribute_e( "retreat_attack", m_RetreatAttribute, rkLoader );

	// etc
	m_fDodgeForce = rkLoader.LoadFloat_e( "dodge_force", 0.0f );
	m_fDodgeFrictionRate = rkLoader.LoadFloat_e( "dodge_friction_rate", 0.0f );
	m_fDodgeAniRate = rkLoader.LoadFloat_e( "dodge_ani_rate", FLOAT1 );

	m_vDodgeAniList.clear();
	m_vDodgeAniList.reserve( 4 );
	rkLoader.LoadString_e( "dodge_animation_front", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_right", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_back", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_left", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );

	LoadAttackAttribute_e( "extra_gauge_attack", m_ExtraGaugeAttribute, rkLoader );
	m_bEnableExtraGauge = rkLoader.LoadBool_e( "extra_gauge_enable", false );
	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );

	m_dwHalfChargeTime	= rkLoader.LoadInt_e( "half_charging_time", 0 );

	rkLoader.LoadString_e( "move_ani_fr", "", szBuf, MAX_PATH );
	m_MoveAniFr = szBuf;
	m_fMoveAniFrRate = rkLoader.LoadFloat_e( "move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "move_ani_bk", "", szBuf, MAX_PATH );
	m_MoveAniBk = szBuf;
	m_fMoveAniBkRate = rkLoader.LoadFloat_e( "move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "move_ani_rt", "", szBuf, MAX_PATH );
	m_MoveAniRt = szBuf;
	m_fMoveAniRtRate = rkLoader.LoadFloat_e( "move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "move_ani_lt", "", szBuf, MAX_PATH );
	m_MoveAniLt = szBuf;
	m_fMoveAniLtRate = rkLoader.LoadFloat_e( "move_ani_rate_lt", FLOAT1 );

	m_fMoveForce = rkLoader.LoadFloat_e( "move_force", 0.0f );
	m_fMoveForceFriction = rkLoader.LoadFloat_e( "move_force_friction", 0.0f );
}

ioItem* ioChargeComboItem::Clone()
{
	return new ioChargeComboItem( *this );
}

ioWeaponItem::WeaponSubType ioChargeComboItem::GetSubType() const
{
	return WST_CHARGE_COMBO;
}

void ioChargeComboItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_dwAttackStartTime = 0;
	m_dwComboAttackTimeEnd = 0;
	m_bUsedExtraAttack = false;
	m_iChargeCombo = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRetreatEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iNormalCombo = 0;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = dwCurTime;
		m_iNormalCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioChargeComboItem::CheckNormalAttack( ioBaseChar *pOwner )
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
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;

				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_COMBO:
		{
			pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;

				if( pOwner->CheckExtendAttackEndJump() )
					return;

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				int iExtendCnt = m_vChargeComboAttList.size();
				if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
				{
					if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
					{
						if( ChangeToRetreatAttack( pOwner ) )
						{
							if( pOwner->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
								FillItemGaugePacketHeader( pOwner, kPacket );
								kPacket << (int)m_ChargeState;
								kPacket << pOwner->GetTargetRot();
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}
							return;
						}
					}
					else if( m_bEnableExtraGauge && IsEnableGauge() && pOwner->IsDefenseKey() && 
						COMPARE( m_iChargeCombo, 0, iExtendCnt ) )
					{
						if( ChangeToExtraGaugeAttack( pOwner ) )
						{
							if( pOwner->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
								FillItemGaugePacketHeader( pOwner, kPacket );
								kPacket << (int)m_ChargeState;
								kPacket << pOwner->GetTargetRot();
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}
							return;
						}
					}
				}
				OnComboState( pOwner );
			}

			if( pOwner && pOwner->GetState() == CS_ATTACK )
			{
				ioWeaponItem::CheckNormalAttack( pOwner );
			}
		}
		break;
	case CS_HALF_COMBO:
		{
			pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;

				if( pOwner->CheckExtendAttackEndJump() )
					return;

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				int iExtendCnt = m_vHalfChargeComboAttList.size();
				if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
				{
					if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
					{
						if( ChangeToRetreatAttack( pOwner ) )
						{
							if( pOwner->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
								FillItemGaugePacketHeader( pOwner, kPacket );
								kPacket << (int)m_ChargeState;
								kPacket << pOwner->GetTargetRot();
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}
							return;
						}
					}
					else if( m_bEnableExtraGauge && IsEnableGauge() && pOwner->IsDefenseKey() && 
						COMPARE( m_iChargeCombo, 0, iExtendCnt ) )
					{
						if( ChangeToExtraGaugeAttack( pOwner ) )
						{
							if( pOwner->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
								FillItemGaugePacketHeader( pOwner, kPacket );
								kPacket << (int)m_ChargeState;
								kPacket << pOwner->GetTargetRot();
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}
							return;
						}
					}
				}
				OnHalfComboState( pOwner );
			}

			if( pOwner && pOwner->GetState() == CS_ATTACK )
			{
				ioWeaponItem::CheckNormalAttack( pOwner );
			}
		}
		break;
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	case CS_EXTRAGAUGE:
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				if( ChangeToRetreatAttack( pOwner ) )
				{
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << (int)m_ChargeState;
						kPacket << pOwner->GetTargetRot();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
					return;
				}
			}
		}
		if( m_dwRetreatEndTime < dwCurTime )
		{
			m_ChargeState = CS_NONE;
			pOwner->SetState( CS_DELAY );
		}
		break;
	case CS_CANCEL_DASH:
		if( m_dwComboAttackTimeEnd < dwCurTime )
		{
			m_ChargeState = CS_NONE;
			pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

void ioChargeComboItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !m_vChargeComboAttList.empty() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToComboState( pOwner );
		}
	}
	else
	{
		if( m_dwHalfChargeTime > 0 && m_dwAttackStartTime + m_dwHalfChargeTime <= dwCurTime && 
			!m_vHalfChargeComboAttList.empty() )
			ChangeToHalfComboState( pOwner );
		else
			ChangeToAttackFire( pOwner );
	}
}

void ioChargeComboItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iNormalCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iNormalCombo );
	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboItem::ChangeToComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	m_iChargeCombo = 0;
	m_dwStartComboTime = dwCurTime;

	SetNextComboState( pOwner );
	pOwner->ApplyNormalAttackResult( m_iChargeCombo, true );

	m_ChargeState = CS_COMBO;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iChargeCombo;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 후
	m_iChargeCombo++;
}

void ioChargeComboItem::ChangeToHalfComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	m_iChargeCombo = 0;
	m_dwStartComboTime = dwCurTime;

	SetNextHalfComboState( pOwner );
	pOwner->ApplyNormalAttackResult( m_iChargeCombo, true );

	m_ChargeState = CS_HALF_COMBO;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iChargeCombo;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 후
	m_iChargeCombo++;
}

void ioChargeComboItem::SetNextComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->InitActionStopDelay( true );

	ClearKeyReserve();
	DWORD dwCurTime = FRAMEGETTIME();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	int iExtendCnt = m_vChargeComboAttList.size();
	if( COMPARE( m_iChargeCombo, 0, iExtendCnt ) )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );

		m_szCurAni = m_vChargeComboAttList[m_iChargeCombo].m_AttackAnimation;
		m_fCurAniRate = m_vChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_vChargeComboAttList[m_iChargeCombo] );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_vChargeComboAttList[m_iChargeCombo].m_AttackAnimation );
		float fTimeRate  = m_vChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;
		DWORD dwPreDelay = m_vChargeComboAttList[m_iChargeCombo].m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;

		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( m_dwKeyReserveTime > 0 )
		{
			m_dwKeyReserveTime += dwCurTime;
		}

		DWORD dwTrackingTime = dwCurTime + dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioChargeComboItem::SetNextHalfComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->InitActionStopDelay( true );

	ClearKeyReserve();
	DWORD dwCurTime = FRAMEGETTIME();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	int iExtendCnt = m_vHalfChargeComboAttList.size();
	if( COMPARE( m_iChargeCombo, 0, iExtendCnt ) )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );

		m_szCurAni = m_vHalfChargeComboAttList[m_iChargeCombo].m_AttackAnimation;
		m_fCurAniRate = m_vHalfChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_vHalfChargeComboAttList[m_iChargeCombo] );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_vHalfChargeComboAttList[m_iChargeCombo].m_AttackAnimation );
		float fTimeRate  = m_vHalfChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;
		DWORD dwPreDelay = m_vHalfChargeComboAttList[m_iChargeCombo].m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;

		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( m_dwKeyReserveTime > 0 )
		{
			m_dwKeyReserveTime += dwCurTime;
		}

		DWORD dwTrackingTime = dwCurTime + dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioChargeComboItem::SetExtraComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->InitActionStopDelay( true );

	ClearKeyReserve();
	DWORD dwCurTime = FRAMEGETTIME();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	m_bUsedExtraAttack = true;

	m_szCurAni = m_ExtraAttackAttribute.m_AttackAnimation;
	m_fCurAniRate = m_ExtraAttackAttribute.m_fAttackAniRate;

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;

	pOwner->SetNormalAttackByAttribute( m_ExtraAttackAttribute );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID	     = pGrp->GetAnimationIdx( m_ExtraAttackAttribute.m_AttackAnimation );
	float fTimeRate  = m_ExtraAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ExtraAttackAttribute.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
}

void ioChargeComboItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case CS_ATTACK_FIRE:
		{
			rkPacket >> m_iNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToAttackFire( pOwner );
		}
		break;
	case CS_COMBO:
		{
			rkPacket >> m_iChargeCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToComboState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CS_HALF_COMBO:
		{
			rkPacket >> m_iChargeCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToHalfComboState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CS_RETREAT:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true, false );
			ChangeToRetreatAttack( pOwner );
		}
		break;
	case CS_EXTRAGAUGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true, false );
			ChangeToExtraGaugeAttack( pOwner );
		}
		break;
	case CS_CANCEL_DASH:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			int eCurrDir, eCallerType;
			rkPacket >> eCurrDir;
			rkPacket >> eCallerType;
			SetChangeConditionalSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ), eCallerType );
		}
		break;
	case CS_SPECIAL_MOVE:
		{
			int iKey;
			rkPacket >> iKey;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			if( pOwner->GetState() != CS_WILDBOXER_SPECIAL )
				pOwner->SetState( CS_WILDBOXER_SPECIAL );

			m_ChargeState = CS_SPECIAL_MOVE;
			ChangeToMoveState( pOwner, (ioUserKeyInput::DirKeyInput)iKey );
		}
		break;
	case CS_SPECIAL_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ChangeToSpecialAttack( pOwner );
		}
		break;
	case CS_SPECIAL_END:
		ChangeToEndState( pOwner );
		break;
	}
}

void ioChargeComboItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iChargeCombo;

	bool bUsedExtra;
	rkPacket >> bUsedExtra;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true );

	if( bUsedExtra )
		SetExtraComboState( pOwner );
	else
	{
		switch( iState )
		{
		case CS_COMBO:
			SetNextComboState( pOwner );
			break;
		case CS_HALF_COMBO:
			SetNextHalfComboState( pOwner );
			break;
		}
	}
	pOwner->ApplyAutoTargetInfo( rkPacket );
}

void ioChargeComboItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( false, false );
		}
	}
}

bool ioChargeComboItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_COMBO:
		case CS_EXTRAGAUGE:
		case CS_HALF_COMBO:
			return true;
		}
	}

	return false;
}

bool ioChargeComboItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_COMBO:
		case CS_EXTRAGAUGE:
		case CS_HALF_COMBO:
		case CS_ATTACK_FIRE:
		case CS_CANCEL_DASH:
			return true;
		}
	}
	else if( pOwner->GetState() == CS_WILDBOXER_SPECIAL )
	{
		if( m_ChargeState == CS_SPECIAL_ATTACK )
			return true;
	}
	return false;
}

bool ioChargeComboItem::SetDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir = -vMoveDir;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
	}

	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	pOwner->SetState( CS_DODGE_DEFENSE );
	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;
	
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DODGE_DEFENSE );
		kPacket << pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket << vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioChargeComboItem::ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	D3DXVECTOR3 vMoveDir;
	rkPacket >> vMoveDir;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}

	pOwner->SetState( CS_DODGE_DEFENSE );
	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
}

void ioChargeComboItem::CheckDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDodgeDefenseEnd > dwCurTime )
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
		}
	}
	else
	{
		ProcessReserveKeyInput( pOwner );
	}
}

bool ioChargeComboItem::ProcessCancel( ioBaseChar *pOwner )
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
			m_dwCancelCheckTime = dwTime+1;
			
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

bool ioChargeComboItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fCurAniRate;
		dwInputSpecialDashTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) && IsEnableGauge() )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

bool ioChargeComboItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_COMBO:
		case CS_ATTACK_FIRE:
		case CS_HALF_COMBO:
		case CS_CANCEL_DASH:
			return true;
		}
	}
	else if( pOwner->GetState() == CS_WILDBOXER_SPECIAL )
	{
		if( m_ChargeState == CS_SPECIAL_ATTACK )
			return true;
	}
	return false;
}

void ioChargeComboItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CancelDashAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	if( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	DWORD dwPreDelay = m_CancelDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CancelDashAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	CheckDecreaseGauge();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	m_ChargeState = CS_CANCEL_DASH;
	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = m_CancelDashAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CancelDashAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CancelDashAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwComboAttackTimeEnd = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;
	m_szCurAni = m_CancelDashAttribute.m_AttackAnimation;
	m_fCurAniRate = m_CancelDashAttribute.m_fAttackAniRate;
	m_dwInputCancelCheckTime = FRAMEGETTIME();

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioChargeComboItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioChargeComboItem::OnComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwComboAttackTimeEnd < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if( m_bUsedExtraAttack )
		{
			CheckReserve( pOwner );
		}
		else
		{
			CheckReserve( pOwner );

			if( m_iChargeCombo == (int)m_vChargeComboAttList.size() )
				return;

			if( m_dwStartComboTime+m_dwMaxComboDuration > dwCurTime )
			{
				if( pOwner->IsAttackKey() )
				{
					SetNextComboState( pOwner );

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << m_ChargeState;
						kPacket << m_iChargeCombo;
						kPacket << m_bUsedExtraAttack;
						kPacket << pOwner->GetTargetRot();
						pOwner->FillAutoTargetInfo( kPacket );
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					m_iChargeCombo++;
				}
				else if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
				{
					m_iSkillInput = -1;
					return;
				}
			}
		}
	}
}

void ioChargeComboItem::OnHalfComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwComboAttackTimeEnd < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if( m_bUsedExtraAttack )
		{
			CheckReserve( pOwner );
		}
		else
		{
			CheckReserve( pOwner );

			if( m_iChargeCombo == (int)m_vHalfChargeComboAttList.size() )
				return;

			if( m_dwStartComboTime+m_dwMaxComboDuration > dwCurTime )
			{
				if( pOwner->IsAttackKey() )
				{
					SetNextHalfComboState( pOwner );

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << m_ChargeState;
						kPacket << m_iChargeCombo;
						kPacket << m_bUsedExtraAttack;
						kPacket << pOwner->GetTargetRot();
						pOwner->FillAutoTargetInfo( kPacket );
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					m_iChargeCombo++;
				}
				else if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
				{
					m_iSkillInput = -1;
					return;
				}
			}
		}
	}
}

void ioChargeComboItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

bool ioChargeComboItem::ChangeToRetreatAttack( ioBaseChar *pOwner )
{
	if( m_RetreatAttribute.m_AttackAnimation.IsEmpty() )
		return false;

	if( !pOwner ) return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeState = CS_RETREAT;
	m_dwRetreatEndTime = dwCurTime;

	m_dwKeyReserveTime = 0;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pOwner->ClearDirDoubleClick();

	int iAniID = pGrp->GetAnimationIdx( m_RetreatAttribute.m_AttackAnimation );
	float fTimeRate = m_RetreatAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_RetreatAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_RetreatAttribute.m_dwPreDelay );

	m_dwRetreatEndTime += pGrp->GetAnimationFullTime( iAniID );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pOwner->SetReservedSliding( m_RetreatAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	return true;
}

bool ioChargeComboItem::ChangeToExtraGaugeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeState = CS_EXTRAGAUGE;
	m_dwRetreatEndTime = dwCurTime;
	m_dwKeyReserveTime = 0;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_ExtraGaugeAttribute.m_AttackAnimation );
	float fTimeRate = m_ExtraGaugeAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_ExtraGaugeAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();

	m_dwRetreatEndTime += pGrp->GetAnimationFullTime( iAniID );
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pOwner->SetReservedSliding( m_ExtraGaugeAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );
	CheckDecreaseGauge();
	return true;
}

void ioChargeComboItem::OnRetreatState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime < dwCurTime )
	{
		m_ChargeState = CS_NONE;

		if( pOwner->IsSettedDirection() )
		{
			pOwner->SetTargetRotToDirKey( pOwner->GetDirKey() );
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		// 대기
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
		}
	}
}

void ioChargeComboItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioChargeComboItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioChargeComboItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioChargeComboItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioChargeComboItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioChargeComboItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioChargeComboItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioChargeComboItem::SetWildBoxerSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if( pOwner->GetState() != CS_WILDBOXER_SPECIAL )
		pOwner->SetState( CS_WILDBOXER_SPECIAL );

	m_ChargeState = CS_SPECIAL_MOVE;
	ChangeToMoveState( pOwner, ioUserKeyInput::DKI_NONE );
}

void ioChargeComboItem::ClearSpecialState( ioBaseChar *pOwner )
{
	m_vMoveDir = ioMath::VEC3_ZERO;
	m_dwMoveTime = 0;
	m_dwMotionEndTime = 0;
}

void ioChargeComboItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_SPECIAL_MOVE:
		ProcessMoveState( pOwner );
		break;
	case CS_SPECIAL_ATTACK:
		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;
		if( m_dwMotionEndTime < dwCurTime )
		{
			ChangeToEndState( pOwner );
			return;
		}
		break;
	}
}

void ioChargeComboItem::ProcessMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() )
	{
		if( m_dwMotionEndTime < dwCurTime )
		{
			ChangeToMoveState( pOwner, ioUserKeyInput::DKI_NONE );
			return;
		}

		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;

		if( !pOwner->IsAttackKeyDown() )
		{
			ChangeToSpecialAttack( pOwner );
			return;
		}
		if( pOwner->IsJumpKeyDown() )
		{
			pOwner->SetJumpState();
			return;
		}
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
			if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			{
				ChangeToMoveState( pOwner, eNewDirKey );
				return;
			}
		}
	}

	if( m_dwMoveTime > 0 && m_dwMoveTime < FRAMEGETTIME() )
	{
		m_dwMoveTime = 0;
		pOwner->SetForcePower( m_vMoveDir, m_fMoveForce, m_fMoveForceFriction );
	}
}

void ioChargeComboItem::ChangeToMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey )
{
	if( !pOwner )
		return;

	int iCnt = 0;
	int iRest = 0;
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();
	if( eDirKey != ioUserKeyInput::DKI_NONE )
	{
		ioUserKeyInput::DirKeyInput eDir = eDirKey;
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
		D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vMoveDir = D3DXVECTOR3( vSightDir.x, 0.0f, vSightDir.z );

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

		float fCurYaw, fTargetYaw;
		fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
		fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
		fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
		fCurYaw = ioMath::ArrangeHead( fCurYaw );

		float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
		fAngleGap += 360.0f;
		int iSide = (int)fAngleGap % 360;

		iCnt = iSide / 45;
		iRest = iSide % 45;
	}
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	switch( iCnt )
	{
	case 0:
	case 7:
		if( !m_MoveAniFr.IsEmpty() )
			SetMoveAni( pOwner, m_MoveAniFr, m_fMoveAniFrRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_MoveAniRt.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniRt, m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_MoveAniFr.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniFr, m_fMoveAniFrRate, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_MoveAniRt.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniRt, m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_MoveAniBk.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniBk, m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_MoveAniBk.IsEmpty() )
			SetMoveAni( pOwner, m_MoveAniBk, m_fMoveAniBkRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_MoveAniLt.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniLt, m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_MoveAniBk.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniBk, m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_MoveAniLt.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniLt, m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_MoveAniFr.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniFr, m_fMoveAniFrRate, vMoveDir );
		}
		break;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << (int)eDirKey;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboItem::ChangeToEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetState( CS_DELAY );
	m_ChargeState = CS_SPECIAL_END;
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboItem::ChangeToSpecialAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = m_SpecialAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_SpecialAttribute.m_dwEndDelay;
	float fTimeRate = m_SpecialAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_SpecialAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	if ( !m_SpecialAttribute.m_vWeaponInfoList.empty() )
		pOwner->RefreshFireTimeList( iAniID, m_SpecialAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_SpecialAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeState = CS_SPECIAL_ATTACK;
	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );

	m_szCurAni = m_SpecialAttribute.m_AttackAnimation;
	m_fCurAniRate = m_SpecialAttribute.m_fAttackAniRate;
	m_dwInputCancelCheckTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboItem::SetMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	ClearCancelInfo();

	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	if ( iAniID == - 1)
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->ClearCharColSkipTime();
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += FRAMEGETTIME();

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwMoveTime = dwCurTime + dwTime;

	SetCurAttackAniForInputJump( szAni, fAniRate );

	m_vMoveDir = vDir;
}
//////////////////////////////////////////////////////////////////////////////////
ioWildBoxerSpecialState::ioWildBoxerSpecialState()
{
}

ioWildBoxerSpecialState::~ioWildBoxerSpecialState()
{
}

void ioWildBoxerSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioWildBoxerSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioChargeComboItem *pChargeComboItem = ToChargeComboItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pChargeComboItem )
		pChargeComboItem->ClearSpecialState( pOwner );
}

void ioWildBoxerSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioChargeComboItem *pChargeComboItem = ToChargeComboItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pChargeComboItem )
		pChargeComboItem->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioWildBoxerSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioWildBoxerSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	ioChargeComboItem *pChargeComboItem = ToChargeComboItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pChargeComboItem && pChargeComboItem->IsEnableAttackDashCheck( pOwner, true ) )
		return true;

	return false;
}