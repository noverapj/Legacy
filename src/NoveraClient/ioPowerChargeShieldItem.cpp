

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioPowerChargeShieldItem.h"

#include "WeaponDefine.h"
#include "ioChargeComboDefenseJump.h"

ioPowerChargeShieldItem::ioPowerChargeShieldItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_bSetChargingMoveAni = false;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = false;
	m_bEnableDefenseState = false;
	m_dwShieldDummyIndex = -1;
}

ioPowerChargeShieldItem::ioPowerChargeShieldItem( const ioPowerChargeShieldItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_GatheringEffect( rhs.m_GatheringEffect ),
	m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
	m_ChargingMoveAnimation( rhs.m_ChargingMoveAnimation ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
	m_iDisableWeaponType( rhs.m_iDisableWeaponType ),
	m_AniDefenseFrontMove( rhs.m_AniDefenseFrontMove ),
	m_AniDefenseBackMove( rhs.m_AniDefenseBackMove ),
	m_AniDefenseRightMove( rhs.m_AniDefenseRightMove ),
	m_AniDefenseLeftMove( rhs.m_AniDefenseLeftMove ),

	m_szShieldDummyName( rhs.m_szShieldDummyName ),
	m_vShieldDummyOffset( rhs.m_vShieldDummyOffset ),
	m_bEnableExtraGauge( rhs.m_bEnableExtraGauge ),
	m_fNeedExtraGauge( rhs.m_fNeedExtraGauge ),
	m_fShieldDamageRate( rhs.m_fShieldDamageRate )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_bSetChargingMoveAni = false;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = false;
	m_bEnableDefenseState = false;
	m_dwShieldDummyIndex = -1;
}

ioPowerChargeShieldItem::~ioPowerChargeShieldItem()
{
}

void ioPowerChargeShieldItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );
	DestoryShieldDummy( pOwner, false );
}

void ioPowerChargeShieldItem::LoadProperty( ioINILoader &rkLoader )
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

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );
	m_GatheringMaxSound = szBuf;

	rkLoader.LoadString_e( "attack_move_ani", "", szBuf, MAX_PATH );
	m_ChargingMoveAnimation = szBuf;

	m_iDisableWeaponType = rkLoader.LoadInt_e( "disable_weapon_type", 0 );

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "ani_defense_front_move", "victory_defense_move_fr.ani", szBuf, MAX_PATH );
	m_AniDefenseFrontMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_back_move", "victory_defense_move_bk.ani", szBuf, MAX_PATH );
	m_AniDefenseBackMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_right_move", "victory_defense_move_rt.ani", szBuf, MAX_PATH );
	m_AniDefenseRightMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_left_move", "victory_defense_move_lt.ani", szBuf, MAX_PATH );
	m_AniDefenseLeftMove = szBuf;

	rkLoader.LoadString_e( "shield_dummy_name", "", szBuf, MAX_PATH );
	m_szShieldDummyName = szBuf;
	m_vShieldDummyOffset.x = rkLoader.LoadFloat_e( "shield_dummy_offset_x", 0.0f );
	m_vShieldDummyOffset.y = rkLoader.LoadFloat_e( "shield_dummy_offset_y", 0.0f );
	m_vShieldDummyOffset.z = rkLoader.LoadFloat_e( "shield_dummy_offset_z", 0.0f );

	m_bEnableExtraGauge = rkLoader.LoadBool_e( "enable_extra_gauge", false );
	m_fNeedExtraGauge = rkLoader.LoadFloat( "need_shield_gauge", 0.0f );
	m_fShieldDamageRate = rkLoader.LoadFloat_e( "shield_damage_rate", 1.0f );
}

ioItem* ioPowerChargeShieldItem::Clone()
{
	return new ioPowerChargeShieldItem( *this );
}

ioWeaponItem::WeaponSubType ioPowerChargeShieldItem::GetSubType() const
{
	return WST_POWER_CHARGE_SHIELD;
}

void ioPowerChargeShieldItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

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

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioPowerChargeShieldItem::CheckNormalAttack( ioBaseChar *pOwner )
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
	}

	CheckChargingMoveState( pOwner );
}

void ioPowerChargeShieldItem::OnCharging( ioBaseChar *pOwner )
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

void ioPowerChargeShieldItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;
				if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
				{
					if( m_dwMaxChargeTime > dwGapTime )
						m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
					else
						m_fCurChargeRate = FLOAT1;
				}

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
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}


		// MaxCheck
		if( m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime )
		{
			ChangeToAttackFire( pOwner, true );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, true );
	}
}

void ioPowerChargeShieldItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
	{
		m_GatheringEffectID = pEffect->GetUniqueID();
	}

	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << false;		// Max Charge
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeShieldItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
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

	bool bMaxAttack = false;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
	if( bCharged )
	{
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
			pOwner->SetAutoTarget( ATT_NORMAL );

			bool bHasWeapon = false;
			if( m_iDisableWeaponType > 0 )
			{
				bHasWeapon = g_WeaponMgr.HasWeaponType( pOwner, m_iDisableWeaponType );
			}

			if( !bHasWeapon && m_fCurChargeRate == FLOAT1 && !m_ExtendMaxAttribute.m_AttackAnimation.IsEmpty() )
			{
				bMaxAttack = true;
				pOwner->SetNormalAttackByAttribute( m_ExtendMaxAttribute, true, fCurChargeAniRate, fCurForceSlidingRate );
			}
			else
				pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo], true, fCurChargeAniRate, fCurForceSlidingRate );
		}

		if( iExtendCnt <= m_iCurCombo + 1 )
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		else
			pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_ATTACK_FIRE;
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_NORMAL_ATTACK;
	}


	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << bMaxAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeShieldItem::ApplyAttackFire( ioBaseChar *pOwner, bool bCharged, bool bMaxAttack )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
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
			pOwner->SetAutoTarget( ATT_NORMAL );

			if( bMaxAttack )
				pOwner->SetNormalAttackByAttribute( m_ExtendMaxAttribute, true, fCurChargeAniRate, fCurForceSlidingRate );
			else
				pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo], true, fCurChargeAniRate, fCurForceSlidingRate );
		}

		if( iExtendCnt <= m_iCurCombo + 1 )
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		else
			pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_ATTACK_FIRE;
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_NORMAL_ATTACK;
	}
}

void ioPowerChargeShieldItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	bool bCharged;
	bool bMaxAttack;

	switch( iState )
	{
	case CS_NONE:
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
		break;
	case CS_GATHERING:
		{
			bool bMaxCharge;
			rkPacket >> bMaxCharge;

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
				ChangeToGathering( pOwner );
		}
		break;
	case CS_ATTACK_FIRE:
	case CS_NORMAL_ATTACK:
		rkPacket >> bCharged;
		rkPacket >> bMaxAttack;
		ApplyAttackFire( pOwner, bCharged, bMaxAttack );
		break;
	case CS_DUMMY_CREATE:
		CreateShieldDummy( pOwner );
		break;
	case CS_DUMMY_DELETE:
		{
			bool bEffect;
			rkPacket >> bEffect;
			DestoryShieldDummy( pOwner, bEffect );
		}
		break;
	}
}

void ioPowerChargeShieldItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	ioGraffitiWeapon *pGraffiti = ToGraffitiWeapon( pWeapon );

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
	else if( pGraffiti )
	{
		g_WeaponMgr.CheckMaxGraffitiWeaponCnt( pWeapon, GetGraffitiWeaponActionMax() );
	}
}

bool ioPowerChargeShieldItem::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioPowerChargeShieldItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		case CS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}

	return false;
}

void ioPowerChargeShieldItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ChargingMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_GatherAnimation );
			if( pAni )
			{
				dwPlayTime = pAni->GetCurTime();
			}

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = true;
		}
	}
	else
	{
		if( m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_GatherAnimation );
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ChargingMoveAnimation );
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = false;
		}
	}
}

void ioPowerChargeShieldItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	m_fCurChargeRate = 0.0f;

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

bool ioPowerChargeShieldItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioPowerChargeShieldItem::GetAutoTargetValue( ioBaseChar *pOwner,
	float &fRange, float &fMinAngle, float &fMaxAngle,
	AutoTargetType eType )
{
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
}

float ioPowerChargeShieldItem::GetChargeDamageRateForSkill()
{
	float fCurDamageRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		fCurDamageRate += fCurMaxDamage * m_fCurChargeRateForSkill;
	}

	return fCurDamageRate;
}

float ioPowerChargeShieldItem::GetChargeForceRateForSkill()
{
	float fCurForceRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		fCurForceRate += fCurMaxForce * m_fCurChargeRateForSkill;
	}

	return fCurForceRate;
}

float ioPowerChargeShieldItem::GetChargeBlowRateForSkill()
{
	float fCurBlowRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRateForSkill;
	}

	return fCurBlowRate;
}

float ioPowerChargeShieldItem::GetChargeRangeRateForSkill()
{
	float fCurRangeRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		fCurRangeRate += fCurMaxRange * m_fCurChargeRateForSkill;
	}

	return fCurRangeRate;
}

float ioPowerChargeShieldItem::GetChargeSpeedRateForSkill()
{
	float fCurSpeedRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRateForSkill;
	}

	return fCurSpeedRate;
}

float ioPowerChargeShieldItem::GetChargeFloatRateForSkill()
{
	float fCurFloatRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
		fCurFloatRate += fCurMaxFloat * m_fCurChargeRateForSkill;
	}

	return fCurFloatRate;
}

float ioPowerChargeShieldItem::GetChargeScaleRateForSkill()
{
	float fCurScaleRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurChargeRateForSkill;
	}

	return fCurScaleRate;
}

float ioPowerChargeShieldItem::GetChargeExplosionRateForSkill()
{
	float fCurExplosionRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRateForSkill;
	}

	return fCurExplosionRate;
}

void ioPowerChargeShieldItem::ClearCurChargeRateForSkill()
{
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
}

bool ioPowerChargeShieldItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}


bool ioPowerChargeShieldItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( pOwner->IsPrisonerMode() ) return false;
	if( pOwner->IsCatchMode() ) return false;
	if( pOwner->IsHasCrown() ) return false;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetChargingState( true );
	pOwner->SetDefenseMoveEnable( true, false );

	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = true;

	m_bDefenseEnableSet = false;
	m_dwDefenseStartTime = FRAMEGETTIME();
	m_dwDefenseEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurCombo = 0;

	if ( pOwner->IsNeedProcess() && GetNeedBullet() < GetCurBullet() )
		CreateShieldDummy( pOwner );

	return true;
}


void ioPowerChargeShieldItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_bDefenseEnableSet && m_dwDefenseStartTime+m_dwDefensePreDuration <= dwCurTime )
	{
		m_bDefenseEnableSet = true;
		pOwner->SetDefenseEnable( true );
	}

	ProcessShieldDummy( pOwner );

	if( m_dwDefenseEndTime > 0 )
	{
		if( m_dwDefenseEndTime + m_dwDefenseEndDuration < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
				{
					m_iSkillInput = -1;
				}
				else if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
				{
					pOwner->SetDefenseState();
				}
				else if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
				{
					pOwner->SetJumpState();
				}
				else if( m_bEnableAttackState )
				{
					if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
					{
						pOwner->SetNormalAttack( m_iCurCombo );
					}
				}
				else
				{
					pOwner->SetState( CS_DELAY );
				}
			}
			else
			{
				pOwner->SetState( CS_DELAY );
			}
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				bool bReserveSkill = false;
				int iSkillNum = pOwner->GetSkillKeyInput();
				if( iSkillNum >= 0 && !pOwner->IsChatModeState( true ) )
				{
					m_iSkillInput = iSkillNum;
					bReserveSkill = true;
				}

				if( pOwner->IsDefenseKeyDown() && !bReserveSkill && !m_bEnableDefenseState )
				{
					m_bEnableDefenseState = true;
					m_bEnableAttackState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsAttackKey() && !bReserveSkill && !m_bEnableAttackState )
				{
					m_bEnableAttackState = true;
					m_bEnableDefenseState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsJumpKey() && !bReserveSkill && !m_bEnableJumpState )
				{
					m_bEnableJumpState = true;
					m_bEnableAttackState = false;
					m_bEnableDefenseState = false;
				}
			}
		}

		return;
	}

	if( !pOwner->IsNeedProcess() ) return;

	if( !m_bDefenseEnableSet )
		return;

	if( m_dwDefenseStartTime+m_dwDefenseDuration > dwCurTime )
		return;

	if( m_dwExtendDefenseWoundedTime > 0 )
	{
		if( m_dwExtendDefenseWoundedTime > dwCurTime )
			return;
		else
			m_dwExtendDefenseWoundedTime = 0;
	}

	if( !pOwner->IsDefenseKeyDown() )
	{
		EndDefense( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeState;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( pOwner->IsAttackKeyDown() && pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
	{
		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() )
		{
			EndDefense( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			if( pOwner->SetUseSkill(0, SUT_NORMAL) )
			{
				return;
			}
		}
	}
}


void ioPowerChargeShieldItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanDefenseMove( false ) )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bDefenseFirstMoveMotion )
		{
			m_bDefenseMotionSetted = false;
			m_bDefenseFirstMoveMotion = false;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			CheckDefenseMoveAniDir( pOwner );

			/*
			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( pOwner->GetDefenseDirKey() );

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_AniDefenseFrontMove, FLOAT100, FLOAT1 );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_AniDefenseRightMove, FLOAT100, FLOAT1 );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_AniDefenseBackMove, FLOAT100, FLOAT1 );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_AniDefenseLeftMove, FLOAT100, FLOAT1 );
				break;
			}
			*/

			g_TutorialMgr.ActionDefenseMove();
		}
	}
	else
	{
		if( !m_bDefenseMotionSetted )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			
			int iAniID = pOwner->GetDefenseAnimationIdx();
			pGrp->SetLoopAni( iAniID, 50 );

			m_bDefenseMotionSetted = true;
		}
	}
}

void ioPowerChargeShieldItem::EndDefense( ioBaseChar *pOwner )
{
	pOwner->SetDefenseMoveEnable( false, false );
	pOwner->SetDefenseEnable( false );

	m_ChargeState = CS_NONE;
	m_dwDefenseEndTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pOwner->GetDelayAnimationIdx();

		pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100 );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetLoopAni( iAniID, (float)m_dwDefenseEndDuration );
	}

	if ( pOwner->IsNeedProcess() )
		DestoryShieldDummy( pOwner, false );
}



void ioPowerChargeShieldItem::CheckDefenseMoveAniDir( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	// KeyDir
	D3DXVECTOR3 vDefenseDir = pOwner->GetDefenseDir();
	D3DXVec3Normalize( &vDefenseDir, &vDefenseDir );

	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir.y = 0.0f;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtDefenseRot;
	ioMath::CalcDirectionQuaternion( qtDefenseRot, -vDefenseDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtDefenseRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	ioHashString szCurMoveAni;

	switch( iCnt )
	{
	case 0:
		if( !m_AniDefenseFrontMove.IsEmpty() )
			szCurMoveAni = m_AniDefenseFrontMove;
		break;
	case 7:
		if( !m_AniDefenseFrontMove.IsEmpty() )
			szCurMoveAni = m_AniDefenseFrontMove;
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_AniDefenseRightMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseRightMove;
		}
		else
		{
			if( !m_AniDefenseFrontMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseFrontMove;
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_AniDefenseRightMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseRightMove;
		}
		else
		{
			if( !m_AniDefenseBackMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseBackMove;
		}
		break;
	case 3:
	case 4:
		if( !m_AniDefenseBackMove.IsEmpty() )
			szCurMoveAni = m_AniDefenseBackMove;
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_AniDefenseLeftMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseLeftMove;
		}
		else
		{
			if( !m_AniDefenseBackMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseBackMove;
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_AniDefenseLeftMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseLeftMove;
		}
		else
		{
			if( !m_AniDefenseFrontMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseFrontMove;
		}
		break;
	}

	if( !szCurMoveAni.IsEmpty() )
	{
		pGrp->SetLoopAni( szCurMoveAni, FLOAT100, FLOAT1 );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioPowerChargeShieldItem::CheckDefenseMoveAniDir MoveAni Fail!" );
	}
}


void ioPowerChargeShieldItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iChargeState;
	rkPacket >> iChargeState;
	rkPacket >> m_iCurCombo;

	if( iChargeState == CS_NONE )
	{
		EndDefense( pOwner );
	}
}

void ioPowerChargeShieldItem::CreateShieldDummy( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_szShieldDummyName.IsEmpty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vShieldDummyOffset );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szShieldDummyName, iCurIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
	if ( !pDummy )
		return;

	m_dwShieldDummyIndex = pDummy->GetDummyCharIdx();
	
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_DUMMY_CREATE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeShieldItem::DestoryShieldDummy( ioBaseChar *pOwner, bool bEffect /*= true */ )
{
	if ( !pOwner )
		return;

	ioDummyChar *pShield = g_DummyCharMgr.FindDummyChar( pOwner, m_dwShieldDummyIndex );
	if ( !pShield || pShield->GetState() == DCS_DIE )
		return;
	
	pShield->SetDieState( bEffect );

	m_dwShieldDummyIndex = -1;
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_DUMMY_DELETE;
		kPacket << m_iCurCombo;
		kPacket << bEffect;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeShieldItem::ProcessShieldDummy( ioBaseChar *pOwner )
{
	ioDummyChar *pShield = g_DummyCharMgr.FindDummyChar( pOwner, m_dwShieldDummyIndex );
	if ( pShield )
	{
		D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vShieldDummyOffset );
		pShield->SetWorldOrientation( qtRot );
		pShield->SetWorldPosition( vPos );
	}
}

void ioPowerChargeShieldItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
	if ( pOwner->IsNeedProcess() )
		DestoryShieldDummy( pOwner );
}

int ioPowerChargeShieldItem::GetNeedBullet()
{
	return (int)m_fNeedExtraGauge;
}

int ioPowerChargeShieldItem::GetCurBullet()
{
	return m_fCurExtraGauge;
}

int ioPowerChargeShieldItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

void ioPowerChargeShieldItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioPowerChargeShieldItem::DecreaseBullet( int iBullet )
{
	if( iBullet <= 0 )
		return;

	float fCurBullet = iBullet;
	m_fCurExtraGauge = max( 0, m_fCurExtraGauge - fCurBullet );
}

void ioPowerChargeShieldItem::DecreaseBulletByShieldDummy( float fDamage )
{
	if( fDamage <= 0.0f )
		return;

	float fCurBullet = fDamage * m_fShieldDamageRate;
	m_fCurExtraGauge = max( 0, m_fCurExtraGauge - fCurBullet );
}

void ioPowerChargeShieldItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_bEnableExtraGauge )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		{
			if( m_fCurExtraGauge < m_fMaxExtraGauge )
			{
				float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

				m_fCurExtraGauge += fGauge;
				m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
			}
		}
		break;
	case CS_EXTEND_DEFENSE:
		{
			ioDummyChar *pShield = g_DummyCharMgr.FindDummyChar( pOwner, m_dwShieldDummyIndex );
			if ( pShield )
			{
				float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;

				m_fCurExtraGauge -= fGauge;
				m_fCurExtraGauge = max( 0, m_fCurExtraGauge );
			}
		}
		break;
	case CS_JUMP:
		{
			ioDummyChar *pShield = g_DummyCharMgr.FindDummyChar( pOwner, m_dwShieldDummyIndex );
			if ( pShield )
			{
				float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;

				m_fCurExtraGauge -= fGauge;
				m_fCurExtraGauge = max( 0, m_fCurExtraGauge );
			}
			else if( pOwner->GetJumpState() == JS_JUMPPING )
			{
				if( m_fCurExtraGauge < m_fMaxExtraGauge )
				{
					float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

					m_fCurExtraGauge += fGauge;
					m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
				}
			}
		}
		break;
	}

	if ( m_fCurExtraGauge <= 0.0f )
		DestoryShieldDummy( pOwner );
}

void ioPowerChargeShieldItem::ProcessJumppingByNoNeedProcess( ioBaseChar *pOwner, float fHeightGap )
{
	ProcessShieldDummy( pOwner );
}

