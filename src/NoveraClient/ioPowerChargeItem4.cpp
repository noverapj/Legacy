

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioPowerChargeItem4.h"

#include "WeaponDefine.h"

ioPowerChargeItem4::ioPowerChargeItem4()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_FireStartDirKey = ioUserKeyInput::DKI_NONE;
	m_bFirstFireMoveMotion = true;
	m_bSetChargingMoveAni = false;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;
}

ioPowerChargeItem4::ioPowerChargeItem4( const ioPowerChargeItem4 &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_GatheringEffect( rhs.m_GatheringEffect ),
  m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
  m_GatherMoveFront( rhs.m_GatherMoveFront ),
  m_GatherMoveBack( rhs.m_GatherMoveBack ),
  m_GatherMoveLeft( rhs.m_GatherMoveLeft ),
  m_GatherMoveRight( rhs.m_GatherMoveRight ),
  m_RetreatAttribute( rhs.m_RetreatAttribute ),
  m_szRetreatFr( rhs.m_szRetreatFr ),
  m_szRetreatBack( rhs.m_szRetreatBack ),
  m_szRetreatLt( rhs.m_szRetreatLt ),
  m_szRetreatRt( rhs.m_szRetreatRt ),
  m_bEnableChargeFireAutoTarget( rhs.m_bEnableChargeFireAutoTarget )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_FireStartDirKey = ioUserKeyInput::DKI_NONE;
	m_bFirstFireMoveMotion = true;
	m_bSetChargingMoveAni = false;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;
}

ioPowerChargeItem4::~ioPowerChargeItem4()
{
}

void ioPowerChargeItem4::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	m_bEnableChargeFireAutoTarget = rkLoader.LoadBool_e( "charge_fire_auto_target", false );

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

	wsprintf_e( szBuf, "gathering_max_attack" );
	LoadAttackAttribute( szBuf, m_ExtendMaxAttribute, rkLoader );

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "gathering_move_fr", "", szBuf, MAX_PATH );
	m_GatherMoveFront = szBuf;

	rkLoader.LoadString_e( "gathering_move_bk", "", szBuf, MAX_PATH );
	m_GatherMoveBack = szBuf;

	rkLoader.LoadString_e( "gathering_move_rt", "", szBuf, MAX_PATH );
	m_GatherMoveRight = szBuf;

	rkLoader.LoadString_e( "gathering_move_lt", "", szBuf, MAX_PATH );
	m_GatherMoveLeft = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
    m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );
	m_GatheringMaxSound = szBuf;

	// retreat
	LoadAttackAttribute_e( "retreat_attack", m_RetreatAttribute, rkLoader );

	rkLoader.LoadString_e( "retreat_move_fr", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;

	rkLoader.LoadString_e( "retreat_move_bk", "", szBuf, MAX_PATH );
	m_szRetreatBack = szBuf;

	rkLoader.LoadString_e( "retreat_move_rt", "", szBuf, MAX_PATH );
	m_szRetreatRt = szBuf;

	rkLoader.LoadString_e( "retreat_move_lt", "", szBuf, MAX_PATH );
	m_szRetreatLt = szBuf;
}

ioItem* ioPowerChargeItem4::Clone()
{
	return new ioPowerChargeItem4( *this );
}

ioWeaponItem::WeaponSubType ioPowerChargeItem4::GetSubType() const
{
	return WST_POWER_CHARGE4;
}

void ioPowerChargeItem4::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwRetreatEndTime = 0;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_bSetChargingMoveAni = false;
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
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioPowerChargeItem4::CheckNormalAttack( ioBaseChar *pOwner )
{
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
			pOwner->CheckExtraAniJump();

			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();
			DWORD dwKeyReserveTime = pOwner->GetCharKeyReserveEnableTime();

			if( dwKeyReserveTime > 0 && dwKeyReserveTime + dwLooseTime < dwCurTime )
			{
				if( pOwner->IsNeedProcess() )
				{
					if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
					{
						pOwner->ChangeDirectionByInputDir( false );
						pOwner->SetDashState( false );
						pOwner->SendDashState( false );
						return;
					}
				}
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_NORMAL_ATTACK:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	}

	CheckChargingMoveState( pOwner );
}

void ioPowerChargeItem4::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioPowerChargeItem4::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 점프키 누름. 점프상태 전환
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;
		float fCurChargeRate = 0.0f;
		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				fCurChargeRate = FLOAT1;
		}

		if( pOwner->IsDefenseKeyDown() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				m_fCurChargeRate = fCurChargeRate;
				m_fCurChargeRateForSkill = m_fCurChargeRate;

				pOwner->SetAttackMoveEnable( false );
				pOwner->SetChargingState( false );
				m_bSetChargingMoveAni = false;

				if( m_GatheringEffectID != -1 )
				{
					pOwner->EndEffect( m_GatheringEffectID, false );
					m_GatheringEffectID = -1;
				}

				if( m_GatheringMaxEffectID != -1 )
				{
					pOwner->EndEffect( m_GatheringMaxEffectID, false );
					m_GatheringMaxEffectID = -1;
				}

				m_ChargeState = CS_NONE;
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_ChargeState;
					kPacket << m_iCurCombo;
					kPacket << false;		// delaystate
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}

		m_iMaxBullet = 100;
		m_iCurBullet = 100 * fCurChargeRate;
		
		if( (m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime) &&
			m_GatheringMaxEffectID == -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;

			ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
			if( pEffect )
			{
				m_GatheringMaxEffectID = pEffect->GetUniqueID();
			}

			if( !m_GatheringMaxSound.IsEmpty() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
				}
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << true;		// Max Charge
				kPacket << false;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
		{
			ChangeToRetreatAttack( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << pOwner->GetTargetRot();
				kPacket << pOwner->GetMoveDir();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, true );
	}
}

void ioPowerChargeItem4::ChangeToGathering( ioBaseChar *pOwner, bool bRefresh )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeState = CS_GATHERING;
	
	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
		m_GatheringEffectID = pEffect->GetUniqueID();

	if( bRefresh )
		m_dwGatheringStartTime = dwCurTime;

	pOwner->SetAttackMoveEnable( true );

	m_FireStartDirKey = pOwner->GetCurDirKey();

	m_bFirstFireMoveMotion = true;
	m_bSetChargingMoveAni = false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << false;		// Max Charge
		kPacket << bRefresh;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeItem4::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXQUATERNION qtTargetRot = pOwner->GetTargetRot();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}
	//

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
	if( bCharged )
	{
		pOwner->SetTargetRotToRotate( qtTargetRot, true, false );

		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

		float fCurChargeAniRate = FLOAT1;
		float fCurForceSlidingRate = FLOAT1;
		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				m_fCurChargeRate = FLOAT1;

			if( m_fMaxChargeAniRate > 0.0f )
			{
				float fCurMaxAniRate = max( 0.0f, (m_fMaxChargeAniRate - FLOAT1) );
				fCurChargeAniRate += fCurMaxAniRate * m_fCurChargeRate;
			}

			if( m_fMaxForceSlidingRate > 0.0f )
			{
				float fCurMaxSlidingRate = max( 0.0f, (m_fMaxForceSlidingRate - FLOAT1) );
				fCurForceSlidingRate += fCurMaxSlidingRate * m_fCurChargeRate;
			}
		}

		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			if( m_bEnableChargeFireAutoTarget )
				pOwner->SetAutoTarget( ATT_NORMAL );
			else
				pOwner->StopAutoTargetTracking();

			int iAniID = -1;
			float fTimeRate = FLOAT1;
			DWORD dwPreDelay = 0;
			ioEntityGroup *pGrp = pOwner->GetGroup();

			if( m_fCurChargeRate == FLOAT1 && !m_ExtendMaxAttribute.m_AttackAnimation.IsEmpty() )
			{
				pOwner->SetNormalAttackByAttribute( m_ExtendMaxAttribute, true, fCurChargeAniRate, fCurForceSlidingRate );

				iAniID = pGrp->GetAnimationIdx( m_ExtendMaxAttribute.m_AttackAnimation );
				fTimeRate = m_ExtendMaxAttribute.m_fAttackAniRate;
				dwPreDelay = m_ExtendMaxAttribute.m_dwPreDelay;
			}
			else
			{
				pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo], true, fCurChargeAniRate, fCurForceSlidingRate );

				iAniID = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
				fTimeRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
				dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;
			}

			pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		}

		if( iExtendCnt <= m_iCurCombo + 1 )
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		else
			pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_ATTACK_FIRE;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << CS_ATTACK_FIRE;
			kPacket << m_iCurCombo;
			kPacket << qtTargetRot;
			if( m_bEnableChargeFireAutoTarget )
				pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			if( !m_bChargeAutoTarget )
				pOwner->SetAutoTarget( ATT_NORMAL );

			pOwner->SetNormalAttackByAttribute( *pAttribute );
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_NORMAL_ATTACK;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << CS_NORMAL_ATTACK;
			kPacket << m_iCurCombo;
			if( !m_bChargeAutoTarget )
				pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioPowerChargeItem4::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		{
			bool bDelayState;
			rkPacket >> bDelayState;

			pOwner->SetAttackMoveEnable( false );
			pOwner->SetChargingState( false );
			m_bSetChargingMoveAni = false;

			if( m_GatheringEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;
			}

			if( m_GatheringMaxEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringMaxEffectID, false );
				m_GatheringMaxEffectID = -1;
			}

			m_ChargeState = CS_NONE;

			if( bDelayState )
				pOwner->SetState( CS_DELAY );
		}
		break;
	case CS_GATHERING:
		{
			bool bMaxCharge, bRefresh;
			rkPacket >> bMaxCharge >> bRefresh;

			if( bMaxCharge )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;

				if( m_GatheringMaxEffectID == -1 )
				{
					ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
					if( pEffect )
					{
						m_GatheringMaxEffectID = pEffect->GetUniqueID();
					}
				}

				if( !m_GatheringMaxSound.IsEmpty() )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if( pGrp )
					{
						g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
					}
				}
			}
			else
			{
				ChangeToGathering( pOwner, bRefresh );
			}
		}
		break;
	case CS_ATTACK_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner, true );

			if( m_bEnableChargeFireAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CS_NORMAL_ATTACK:
		{
			if( !m_bChargeAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeToAttackFire( pOwner, false );
		}
		break;
	case CS_RETREAT:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vDir;
			rkPacket >> vDir;
			pOwner->SetMoveDirection( vDir );

			ChangeToRetreatAttack( pOwner );
		}
		break;
	}
}

void ioPowerChargeItem4::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

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

bool ioPowerChargeItem4::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

bool ioPowerChargeItem4::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !bCheck ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		case CS_ATTACK_FIRE:
			return true;
		}
	}

	return false;
}

void ioPowerChargeItem4::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bFirstFireMoveMotion )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bSetChargingMoveAni = true;
			m_bFirstFireMoveMotion = false;

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_FireStartDirKey );

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_GatherMoveFront, FLOAT100, FLOAT1, m_fGatherAniRate );
			}
			else
			{
				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_GatherMoveFront, FLOAT100, FLOAT1, m_fGatherAniRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_GatherMoveRight, FLOAT100, FLOAT1, m_fGatherAniRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_GatherMoveBack, FLOAT100, FLOAT1, m_fGatherAniRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_GatherMoveLeft, FLOAT100, FLOAT1, m_fGatherAniRate );
					break;
				}
			}
		}
	}
	else
	{
		if( m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_GatherAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = false;
		}
	}
}


void ioPowerChargeItem4::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	m_fCurChargeRate = 0.0f;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}
}

bool ioPowerChargeItem4::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioPowerChargeItem4::GetAutoTargetValue( ioBaseChar *pOwner,
										    float &fRange, float &fMinAngle, float &fMaxAngle,
											AutoTargetType eType )
{
	switch( eType )
	{
	case ATT_NORMAL:
		if( IsChargeAutoTarget(pOwner) )
		{
			float fCurTargetRangeRate, fCurTargetAngleRate;
			fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

			float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
			float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

			fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
			fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

			fRange = m_fAutoTargetRange * fCurTargetRangeRate;
			fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
			fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
		}
		else
		{
			fRange = m_fAutoTargetRange;
			fMinAngle = m_fAutoTargetMinAngle;
			fMaxAngle = m_fAutoTargetMaxAngle;
		}
		return;
	case ATT_DASH:
		if( m_fDashAutoTargetRange == 0.0f )
			break;

		fRange = m_fDashAutoTargetRange;
		fMinAngle = m_fDashAutoTargetMinAngle;
		fMaxAngle = m_fDashAutoTargetMaxAngle;
		return;
	case ATT_JUMP:
		if( m_fJumpAutoTargetRange == 0.0f )
			break;

		fRange = m_fJumpAutoTargetRange;
		fMinAngle = m_fJumpAutoTargetMinAngle;
		fMaxAngle = m_fJumpAutoTargetMaxAngle;
		return;
	case ATT_COUNTER:
		if( m_fCountAutoTargetRange == 0.0f )
			break;

		fRange = m_fCountAutoTargetRange;
		fMinAngle = m_fCountAutoTargetMinAngle;
		fMaxAngle = m_fCountAutoTargetMaxAngle;
		return;
	}


	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;

	return;
}

float ioPowerChargeItem4::GetChargeDamageRateForSkill()
{
	float fCurDamageRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		fCurDamageRate += fCurMaxDamage * m_fCurChargeRateForSkill;
	}

	return fCurDamageRate;
}

float ioPowerChargeItem4::GetChargeForceRateForSkill()
{
	float fCurForceRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		fCurForceRate += fCurMaxForce * m_fCurChargeRateForSkill;
	}

	return fCurForceRate;
}

float ioPowerChargeItem4::GetChargeBlowRateForSkill()
{
	float fCurBlowRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRateForSkill;
	}

	return fCurBlowRate;
}

float ioPowerChargeItem4::GetChargeRangeRateForSkill()
{
	float fCurRangeRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		fCurRangeRate += fCurMaxRange * m_fCurChargeRateForSkill;
	}

	return fCurRangeRate;
}

float ioPowerChargeItem4::GetChargeSpeedRateForSkill()
{
	float fCurSpeedRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRateForSkill;
	}

	return fCurSpeedRate;
}

float ioPowerChargeItem4::GetChargeFloatRateForSkill()
{
	float fCurFloatRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
		fCurFloatRate += fCurMaxFloat * m_fCurChargeRateForSkill;
	}

	return fCurFloatRate;
}

float ioPowerChargeItem4::GetChargeScaleRateForSkill()
{
	float fCurScaleRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurChargeRateForSkill;
	}

	return fCurScaleRate;
}

float ioPowerChargeItem4::GetChargeExplosionRateForSkill()
{
	float fCurExplosionRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRateForSkill;
	}

	return fCurExplosionRate;
}

void ioPowerChargeItem4::ClearCurChargeRateForSkill()
{
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
}

void ioPowerChargeItem4::ChangeToRetreatAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeState = CS_RETREAT;
	m_dwRetreatEndTime = dwCurTime;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

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

			pOwner->SetMoveDirByRotate( qtRotate );
		}
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pOwner->ClearDirDoubleClick();

	ioHashString szRetreatAni = CheckRetreatAni( pOwner );

	int iAniID = pGrp->GetAnimationIdx( szRetreatAni );
	float fTimeRate = m_RetreatAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_RetreatAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();

	m_dwRetreatEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pOwner->SetReservedSliding( m_RetreatAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
}

void ioPowerChargeItem4::OnRetreatState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime < dwCurTime )
	{
		m_ChargeState = CS_NONE;
		pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ChangeToGathering( pOwner, false );
			return;
		}

		// 대기
		m_fCurChargeRate = 0.0f;
		m_fCurChargeRateForSkill = 0.0f;

		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}

		if( m_GatheringMaxEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringMaxEffectID, false );
			m_GatheringMaxEffectID = -1;
		}

		pOwner->SetState( CS_DELAY );
		
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeState;
			kPacket << m_iCurCombo;
			kPacket << true;			// delaystate
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

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

ioHashString ioPowerChargeItem4::CheckRetreatAni( ioBaseChar *pOwner )
{
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

	ioHashString szCurRetreatAni;

	switch( iCnt )
	{
	case 0:
		if( !m_szRetreatFr.IsEmpty() )
			szCurRetreatAni = m_szRetreatFr;
		break;
	case 7:
		if( !m_szRetreatFr.IsEmpty() )
			szCurRetreatAni = m_szRetreatFr;
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_szRetreatRt.IsEmpty() )
				szCurRetreatAni = m_szRetreatRt;
		}
		else
		{
			if( !m_szRetreatFr.IsEmpty() )
				szCurRetreatAni = m_szRetreatFr;
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_szRetreatRt.IsEmpty() )
				szCurRetreatAni = m_szRetreatRt;
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
				szCurRetreatAni = m_szRetreatBack;
		}
		break;
	case 3:
	case 4:
		if( !m_szRetreatBack.IsEmpty() )
			szCurRetreatAni = m_szRetreatBack;
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_szRetreatLt.IsEmpty() )
				szCurRetreatAni = m_szRetreatLt;
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
				szCurRetreatAni = m_szRetreatBack;
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_szRetreatLt.IsEmpty() )
				szCurRetreatAni = m_szRetreatLt;
		}
		else
		{
			if( !m_szRetreatFr.IsEmpty() )
				szCurRetreatAni = m_szRetreatFr;
		}
		break;
	}

	return szCurRetreatAni;
}




