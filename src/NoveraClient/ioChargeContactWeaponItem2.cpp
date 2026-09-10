

#include "stdafx.h"

#include "ioChargeContactWeaponItem2.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioChargeContactWeaponItem2::ioChargeContactWeaponItem2()
{
	m_ChargeState = CS_NONE;
	m_ExtraType = ET_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;
	m_dwChargeExtraEndTime = 0;

	m_bEnableExtraRetreat = false;
}

ioChargeContactWeaponItem2::ioChargeContactWeaponItem2( const ioChargeContactWeaponItem2 &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_szRetreatFr( rhs.m_szRetreatFr ),
  m_szRetreatBack( rhs.m_szRetreatBack ),
  m_szRetreatLt( rhs.m_szRetreatLt ),
  m_szRetreatRt( rhs.m_szRetreatRt ),
  m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
  m_fRetreatBackRate( rhs.m_fRetreatBackRate ),
  m_fRetreatLtRate( rhs.m_fRetreatLtRate ),
  m_fRetreatRtRate( rhs.m_fRetreatRtRate ),
  m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
  m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
  m_ChargeExtraAttack( rhs.m_ChargeExtraAttack ),
  m_ChargeExtraAttack2( rhs.m_ChargeExtraAttack2 ),
  m_CounterExtraAttack( rhs.m_CounterExtraAttack ),
  m_JumpExtraAttack( rhs.m_JumpExtraAttack ),
  m_bEnableExtraRetreat( rhs.m_bEnableExtraRetreat )
{
	m_ChargeState = CS_NONE;
	m_ExtraType = ET_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;
	m_dwChargeExtraEndTime = 0;
}

ioChargeContactWeaponItem2::~ioChargeContactWeaponItem2()
{
}

void ioChargeContactWeaponItem2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_back", "", szBuf, MAX_PATH );
	m_szRetreatBack = szBuf;
	m_fRetreatBackRate = rkLoader.LoadFloat_e( "retreat_animation_back_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_right", "", szBuf, MAX_PATH );
	m_szRetreatRt = szBuf;
	m_fRetreatRtRate = rkLoader.LoadFloat_e( "retreat_animation_right_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_left", "", szBuf, MAX_PATH );
	m_szRetreatLt = szBuf;
	m_fRetreatLtRate = rkLoader.LoadFloat_e( "retreat_animation_left_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );

	LoadAttackAttribute_e( "charge_extra_attack", m_ChargeExtraAttack, rkLoader );
	LoadAttackAttribute_e( "charge_extra_attack2", m_ChargeExtraAttack2, rkLoader );
	LoadAttackAttribute_e( "counter_extra_attack", m_CounterExtraAttack, rkLoader );
	LoadAttackAttribute_e( "jump_extra_attack", m_JumpExtraAttack, rkLoader );

	m_bEnableExtraRetreat = rkLoader.LoadBool_e( "enable_extra_retreat", false );
}

ioItem* ioChargeContactWeaponItem2::Clone()
{
	return new ioChargeContactWeaponItem2( *this );
}

ioWeaponItem::WeaponSubType ioChargeContactWeaponItem2::GetSubType() const
{
	return WST_CHARGE_CONTACT2;
}

void ioChargeContactWeaponItem2::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;
	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;
	m_dwChargeExtraEndTime = 0;

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
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
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioChargeContactWeaponItem2::CheckNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			if( m_bCharged )
				pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( m_bCharged )
				{
					if( pOwner->CheckExtendAttackEndJump() )
						return;

					if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime + dwLooseTime < dwCurTime )
					{
						CheckCancelReserve( pOwner );
						if( ProcessCancel(pOwner) )
							return;

						if( pOwner->IsAttackKeyDown() )
						{
							ChangeToExtraAttack( pOwner, ET_NONE );
							return;
						}
						if( pOwner->IsJumpKeyDown() && !m_JumpExtraAttack.m_AttackAnimation.IsEmpty() )
						{
							ChangeToExtraAttack( pOwner, ET_JUMP_EXTRA );
							return;
						}
						if( pOwner->IsSettedDirection() )
						{
							if( CheckRetreat( pOwner, true ) )
								return;
						}
					}
				}
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_CHARGE_EXTRA:
		OnChargeExtra( pOwner );
		break;
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	}
}

bool ioChargeContactWeaponItem2::IsEnableExtraAttack()
{
	switch( m_ExtraType )
	{
	case ET_NONE:
	case ET_EXTRA2:
	case ET_JUMP_EXTRA:
	case ET_COUNTER_EXTRA:
		break;
	case ET_CHARGE:
	case ET_COUNTER:
		if( !m_ChargeExtraAttack.m_AttackAnimation.IsEmpty() )
			return true;
	case ET_EXTRA1:
		if( !m_ChargeExtraAttack2.m_AttackAnimation.IsEmpty() )
			return true;
	}
	return false;
}

bool ioChargeContactWeaponItem2::IsEnableExtraRetreat()
{
	if( !m_bEnableExtraRetreat )
		return false;

	if( m_ExtraType == ET_COUNTER_EXTRA )
		return false;

	return true;
}

void ioChargeContactWeaponItem2::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAttackFire( pOwner, true );
			g_TutorialMgr.ActionChargeAttack();
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioChargeContactWeaponItem2::OnChargeExtra( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwChargeExtraEndTime > 0 && m_dwChargeExtraEndTime < dwCurTime )
	{
		if( pOwner->IsNeedProcess() )
		{
			ProcessReserveKeyInput( pOwner );
		}
		else
		{
			ClearState();
			pOwner->SetState( CS_DELAY );
		}

		return;
	}
	else
	{
		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			{
				if( pOwner->IsAttackKeyDown() && IsEnableExtraAttack() )
				{
					ChangeToExtraAttack( pOwner, ET_NONE );
					return;
				}
				if( pOwner->IsSettedDirection() && IsEnableExtraRetreat() )
				{
					if( CheckRetreat( pOwner, true ) )
						return;
				}

				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;

				CheckReserve( pOwner );
			}
		}
	}
}

void ioChargeContactWeaponItem2::OnRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime > 0 && m_dwRetreatEndTime < dwCurTime )
	{
		if( pOwner->IsNeedProcess() )
		{
			ProcessReserveKeyInput( pOwner );
		}
		else
		{
			ClearState();
			pOwner->SetState( CS_DELAY );
		}

		return;
	}
	else
	{
		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			{
				if( pOwner->IsAttackKey() && IsEnableExtraAttack() )
				{
					ChangeToExtraAttack( pOwner, ET_NONE );
					return;
				}
				if( pOwner->IsJumpKeyDown() && m_ExtraType == ET_CHARGE && !m_JumpExtraAttack.m_AttackAnimation.IsEmpty() )
				{
					ChangeToExtraAttack( pOwner, ET_JUMP_EXTRA );
					return;
				}

				CheckReserve( pOwner );
			}
		}

		if( m_dwRetreatForceTime > 0 && m_dwRetreatForceTime < dwCurTime )
		{
			m_dwRetreatForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fRetreatForceAmt, m_fRetreatForceFric );
		}
	}
}

void ioChargeContactWeaponItem2::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged, bool bSendNet )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;
	m_ExtraType = ET_NONE;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	if( m_bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
			m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
			
			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;

			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
			float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
			DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

			pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		m_ExtraType = ET_CHARGE;
	}
	else
	{
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
	}

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );
	m_dwKeyReserveTime = 0;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fCurAniRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	m_ChargeState = CS_ATTACK_FIRE;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_ExtraType;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem2::ChangeToExtraAttack( ioBaseChar *pOwner, ExtraType eType )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	m_ChargeState = CS_CHARGE_EXTRA;
	ClearCancelInfo();

	if( eType == ET_NONE )
	{
		if( m_ExtraType == ET_CHARGE )
			m_ExtraType = ET_EXTRA1;
		else if( m_ExtraType == ET_EXTRA1 )
			m_ExtraType = ET_EXTRA2;
		else if( m_ExtraType == ET_COUNTER )
			m_ExtraType = ET_COUNTER_EXTRA;
	}
	else
		m_ExtraType = eType;

	AttackAttribute cAttack = m_ChargeExtraAttack;
	switch( m_ExtraType )
	{
	case ET_EXTRA2:			cAttack = m_ChargeExtraAttack2;		break;
	case ET_JUMP_EXTRA:		cAttack = m_JumpExtraAttack;		break;
	case ET_COUNTER_EXTRA:
		if( !m_CounterExtraAttack.m_AttackAnimation.IsEmpty() )
			cAttack = m_CounterExtraAttack;
		break;
	}
	SetExtraAttack( pOwner, cAttack );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_ExtraType;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem2::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;
	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	ClearCancelInfo();
	m_ChargeState = CS_RETREAT;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwRetreatEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	m_dwRetreatForceTime = dwCurTime + dwTime;

	m_vForceDir = vDir;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << szAni;
		kPacket << fCurRate;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem2::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_ATTACK_FIRE:
		{
			int iChargeType;
			rkPacket >> iChargeType;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, false );

			bool bCharged = false;
			switch( iChargeType )
			{
			case ET_NONE:
				bCharged = false;
				break;
			case ET_CHARGE:
				bCharged = true;
				break;
			}

			ChangeToAttackFire( pOwner, bCharged );
		}
		break;
	case CS_RETREAT:
		{
			ioHashString szAni;
			float fAniRate;
			D3DXVECTOR3 vDir;

			rkPacket >> szAni >> fAniRate >> vDir;

			ChangeToRetreatState( pOwner, szAni, fAniRate, vDir );
		}
		break;
	case CS_CHARGE_EXTRA:
		{
			int iChargeType;
			rkPacket >> iChargeType;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, false );

			ChangeToExtraAttack( pOwner, (ExtraType)iChargeType );
		}
		break;
	}
}

void ioChargeContactWeaponItem2::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
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

bool ioChargeContactWeaponItem2::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioChargeContactWeaponItem2::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !m_bCharged ) return false;

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

bool ioChargeContactWeaponItem2::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioChargeContactWeaponItem2::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioChargeContactWeaponItem2::SetExtraAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->NormalAttackOver( false );

	pOwner->SetAutoTarget( ATT_NORMAL );

	int iAniID	     = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	float fTimeRate  = rkAttr.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttr.m_dwPreDelay;
	DWORD dwEndDelay = rkAttr.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, rkAttr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_dwChargeExtraEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay + dwEndDelay;

	m_dwKeyReserveTime = dwCurTime;

	float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fKeyReserve > 0.0f )
		m_dwKeyReserveTime = dwCurTime + fKeyReserve + dwPreDelay;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( rkAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_szCurAni = rkAttr.m_AttackAnimation;
	m_fCurAniRate = rkAttr.m_fAttackAniRate;

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;
}

bool ioChargeContactWeaponItem2::CheckRetreat( ioBaseChar *pOwner, bool bFront )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

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

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
		if( !m_szRetreatFr.IsEmpty() )
		{
			if( bFront && iRest > 15 )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;			
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::CheckRetreat - No Front Ani" );
		}
		break;
	case 7:
		if( !m_szRetreatFr.IsEmpty() )
		{
			if( bFront && iRest <= 30 )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::CheckRetreat - No Front Ani" );
		}
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::CheckRetreat - No Right Ani" );
		}
		else
		{
			if( bFront && !m_szRetreatFr.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::CheckRetreat - No Right Ani" );
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::CheckRetreat - No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_szRetreatBack.IsEmpty() )
		{
			ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
			return true;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::CheckRetreat - No Back Ani" );
		}
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::CheckRetreat - No Left Ani" );
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::CheckRetreat - No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem2::CheckRetreat - No Left Ani" );
		}
		else
		{
			if( bFront && !m_szRetreatFr.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	}

	return false;
}

void ioChargeContactWeaponItem2::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	ClearState();
}

void ioChargeContactWeaponItem2::ClearState()
{
	m_ChargeState = CS_NONE;
	m_ExtraType = ET_NONE;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_bCharged = false;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;
	m_dwChargeExtraEndTime = 0;
}

void ioChargeContactWeaponItem2::SetChargeAttackByCounter( ioBaseChar *pOwner )
{
	ClearCancelInfo();

	m_bCharged = false;
	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;
	m_dwChargeExtraEndTime = 0;

	m_iCurCombo = 0;
	pOwner->SetState( CS_ATTACK );
	pOwner->SetCurNormalAttackItem( this );

	ChangeToCounterAttack( pOwner );
}

void ioChargeContactWeaponItem2::ChangeToCounterAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	const AttackAttribute *pAttr = GetAttackAttribute( AT_COUNTER_ATTACK );
	if( !pAttr )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = true;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	m_szCurAni = pAttr->m_AttackAnimation;
	m_fCurAniRate = pAttr->m_fAttackAniRate;

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );
	pOwner->SetNormalAttackByAttribute( *pAttr );

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	m_dwKeyReserveTime = 0;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fCurAniRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	m_ChargeState = CS_ATTACK_FIRE;
	m_ExtraType = ET_COUNTER;
}


