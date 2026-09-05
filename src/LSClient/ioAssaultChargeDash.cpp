

#include "stdafx.h"

#include "ioAssaultChargeDash.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ItemDefine.h"
#include "WeaponDefine.h"


ioAssaultChargeDash::ioAssaultChargeDash()
{
	ClearData();
}

ioAssaultChargeDash::ioAssaultChargeDash( const ioAssaultChargeDash &rhs )
: ioExtendDash( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_fOffSet( rhs.m_fOffSet ),
 m_CircleEffectRed( rhs.m_CircleEffectRed ),
 m_CircleEffectBlue( rhs.m_CircleEffectBlue ),
 m_fChargingSpeedRate( rhs.m_fChargingSpeedRate ),
 m_szDelayAnimation( rhs.m_szDelayAnimation ),
 m_fDelayAniRate( rhs.m_fDelayAniRate ),
 m_szLoopAnimation( rhs.m_szLoopAnimation ),
 m_fLoopAniRate( rhs.m_fLoopAniRate ),
 m_dwLoopDuration( rhs.m_dwLoopDuration),
 m_dwMaxChargeTime( rhs.m_dwMaxChargeTime ),
 m_szGatheringEffect( rhs.m_szGatheringEffect ),
 m_szMaxGatheringsound( rhs.m_szMaxGatheringsound ),
 m_fMaxDamageRate( rhs.m_fMaxDamageRate ),
 m_fMaxForceRate( rhs.m_fMaxForceRate ),
 m_fMaxBlowRate( rhs.m_fMaxBlowRate ),
 m_fMaxRangeRate( rhs.m_fMaxRangeRate ),
 m_fMaxSpeedRate( rhs.m_fMaxSpeedRate ),
 m_fMaxChargeAniRate( rhs.m_fMaxChargeAniRate ),
 m_fMaxForceSlidingRate( rhs.m_fMaxForceSlidingRate ),
 m_fMaxTargetRangeRate( rhs.m_fMaxTargetRangeRate ),
 m_fMaxTargetAngleRate( rhs.m_fMaxTargetAngleRate ),
 m_fMaxFloatRate( rhs.m_fMaxFloatRate ),
 m_fMaxScaleRate( rhs.m_fMaxScaleRate ),
 m_fMaxExplosionRate( rhs.m_fMaxExplosionRate ),
 m_fMaxWoundedTimeRate( rhs.m_fMaxWoundedTimeRate ),
 m_fMaxLiveTimeRate( rhs.m_fMaxLiveTimeRate ),
 m_bUseDashElementAtt( rhs.m_bUseDashElementAtt ),
 m_MagicElementAtt( rhs.m_MagicElementAtt )
{
	ClearData();
}

ioAssaultChargeDash::~ioAssaultChargeDash()
{
}

void ioAssaultChargeDash::ClearData()
{
	m_fCurChargingRate		= 0.0f;

	m_dwActionEndTime		= 0;
	m_dwReserveEnableTime	= 0;
	m_dwLoopEndTime			= 0;
	m_dwPreDelayEndTime		= 0;
	m_dwGatheringStartTime	= 0;

	m_bOnCharging	= true;

	m_DashState = CDS_PREDELAY;
	m_CurDirKey	= ioUserKeyInput::DKI_NONE;

	m_dwGatheringEffectID = -1;
}

void ioAssaultChargeDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "circle_effect_red", "", szBuf, MAX_PATH );
	m_CircleEffectRed = szBuf;
	rkLoader.LoadString_e( "circle_effect_blue", "", szBuf, MAX_PATH );
	m_CircleEffectBlue = szBuf;

	m_fOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );

	m_fChargingSpeedRate = rkLoader.LoadFloat_e( "dash_charge_speed_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_delay_animation", "", szBuf, MAX_PATH );	m_szDelayAnimation = szBuf;
	m_fDelayAniRate		= rkLoader.LoadFloat_e( "delay_animation_time_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_loop_animation", "", szBuf, MAX_PATH );	m_szLoopAnimation = szBuf;
	m_fLoopAniRate		= rkLoader.LoadFloat_e( "loop_animation_time_rate", FLOAT1 );
	m_dwLoopDuration	= (DWORD)rkLoader.LoadInt_e( "loop_animation_duration", 0 );

	// Gathering
	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );		m_szGatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );	m_szMaxGatheringsound = szBuf;

	m_dwMaxChargeTime = rkLoader.LoadInt_e( "dash_gathering_max_time", 0 );
	m_fMaxDamageRate = rkLoader.LoadFloat_e( "dash_gathering_max_damage_rate", FLOAT1 );
	m_fMaxForceRate = rkLoader.LoadFloat_e( "dash_gathering_max_force_rate", FLOAT1 );
	m_fMaxBlowRate = rkLoader.LoadFloat_e( "dash_gathering_max_blow_rate", FLOAT1 );

	m_fMaxRangeRate = rkLoader.LoadFloat_e( "dash_gathering_max_range_rate", FLOAT1 );
	m_fMaxSpeedRate = rkLoader.LoadFloat_e( "dash_gathering_max_speed_rate", FLOAT1 );
	m_fMaxChargeAniRate = rkLoader.LoadFloat_e( "dash_gathering_max_ani_rate", 0.0f );
	m_fMaxForceSlidingRate = rkLoader.LoadFloat_e( "dash_gathering_max_force_sliding_rate", 0.0f );

	m_fMaxTargetRangeRate = rkLoader.LoadFloat_e( "dash_gathering_max_target_range_rate", 0.0f );
	m_fMaxTargetAngleRate = rkLoader.LoadFloat_e( "dash_gathering_max_target_angle_rate", 0.0f );

	m_fMaxFloatRate = rkLoader.LoadFloat_e( "dash_gathering_max_float_rate", 0.0f );

	m_fMaxScaleRate = rkLoader.LoadFloat_e( "dash_gathering_max_scale_rate", 0.0f );
	m_fMaxExplosionRate = rkLoader.LoadFloat_e( "dash_gathering_max_explosion_rate", 0.0f );
	m_fMaxWoundedTimeRate = rkLoader.LoadFloat_e( "dash_gathering_max_wounded_time_rate", FLOAT1 );
	m_fMaxLiveTimeRate = rkLoader.LoadFloat_e( "dash_gathering_max_livetime_rate", FLOAT1 );


	rkLoader.LoadString_e( "dash_action_animation", "", szBuf, MAX_PATH );
	m_AttackAttribute.m_AttackAnimation = szBuf;
	m_AttackAttribute.m_fAttackAniRate  = rkLoader.LoadFloat_e( "animation_time_rate", FLOAT1 );
	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );	

	m_AttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "dash_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_AttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "dash_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "dash_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_AttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 1 );
	m_AttackAttribute.m_vWeaponInfoList.clear();
	m_AttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	char szKey[MAX_PATH];
	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
        kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}

	// elelment attack
	m_bUseDashElementAtt = rkLoader.LoadBool_e( "use_dash_element_att", false );
	m_MagicElementAtt.Init();

	LoadAttackAttribute_e( "dash_fire_element_attack", m_MagicElementAtt.m_FireAttack, rkLoader );
	LoadAttackAttribute_e( "dash_ice_element_attack", m_MagicElementAtt.m_IceAttack, rkLoader );
	LoadAttackAttribute_e( "dash_light_element_attack", m_MagicElementAtt.m_LightAttack, rkLoader );
}

DashType ioAssaultChargeDash::GetType() const
{
	return DT_ASSAULT_CHARGE;
}

ioExtendDash* ioAssaultChargeDash::Clone()
{
	return new ioAssaultChargeDash( *this );
}

void ioAssaultChargeDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )	return;

	rkPacket >> m_fCurChargingRate;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	
	int iSubType;
	rkPacket >> iSubType;

	pOwner->SetTargetRotToRotate( qtRot, false );

	SetFireState( pOwner, iSubType, false );	
}

bool ioAssaultChargeDash::StartDash( ioBaseChar *pOwner )
{
	ioExtendDash::StartDash( pOwner );

	ClearData();

	if( pOwner->IsOwnerChar() && pOwner->GetCurItemSkillGauge() < m_fDashExtraGauge )
		return false;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );

	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );

	g_TutorialMgr.ActionAssaultDash();

	m_CurDirKey = pOwner->CheckCurDirKey();

	if( m_szDelayAnimation.IsEmpty() )
	{
		// 딜레이 애니메이션이 없다.
		SetChargingState( pOwner );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID = pGrp->GetAnimationIdx( m_szDelayAnimation );
		if( iAniID == -1 )
		{
			LOG.PrintTimeAndLog( 0, "ioAssaultChargeDash::StartDash - %s Animation is Not Exist",
								 m_szDelayAnimation.c_str() );

			return false;
		}

		float fTimeRate = m_fDelayAniRate;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

		m_dwPreDelayEndTime = pGrp->GetAnimationFullTime( iAniID ) + FRAMEGETTIME();
	}

	return true;
}

void ioAssaultChargeDash::CheckChargeInfo( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	if( !pOwner )	return;
	if( !pWeapon )	return;

	float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	float fCurScaleRate, fCurExplosionRate, fCurWoundedTimeRate, fCurLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
	fCurScaleRate = fCurExplosionRate = fCurWoundedTimeRate = fCurLiveTimeRate = FLOAT1;

	if( m_fCurChargingRate > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );

		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );

		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );

		float fCurMaxWoundedTime = max( 0.0f, (m_fMaxWoundedTimeRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargingRate;
		fCurForceRate += fCurMaxForce * m_fCurChargingRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargingRate;

		fCurRangeRate += fCurMaxRange * m_fCurChargingRate;
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargingRate;

		fCurFloatRate += fCurMaxFloat * m_fCurChargingRate;

		fCurScaleRate += fCurMaxScale * m_fCurChargingRate;
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargingRate;

		fCurWoundedTimeRate += fCurMaxWoundedTime * m_fCurChargingRate;
		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargingRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );

		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );

		pWeapon->SetOwnerChargeScaleRate( fCurScaleRate );
		pWeapon->SetOwnerChargeExplosionRate( fCurExplosionRate );

		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );
		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
	}
}

void ioAssaultChargeDash::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
{
	pWeapon->SetCreateItem( szName );

	CheckChargeInfo( pOwner, pWeapon );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pZone->SetCollisionDir( vDir );
		pZone->SetZoneValue( false, false );
		return;
	}
}

void ioAssaultChargeDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch(m_DashState)
	{
	case CDS_PREDELAY:
		{
			if( m_dwPreDelayEndTime > 0 && m_dwPreDelayEndTime < dwCurTime )
			{
				m_dwPreDelayEndTime = 0;
				SetChargingState( pOwner );
			}
		}
		break;
	case CDS_CHARGING:
		{
			if( pOwner->IsNeedProcess() )
			{
				if( m_dwLoopEndTime > 0 && m_dwLoopEndTime < dwCurTime )
				{
					m_dwLoopEndTime = 0;
					SetFireState( pOwner, ioAttackableItem::MET_NONE, true );
					return;
				}

				CheckDirKey( pOwner );

				if( m_bOnCharging )
				{
					DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

					if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
					{
						if( m_dwMaxChargeTime > dwGapTime )
						{
							m_fCurChargingRate = (float)dwGapTime / m_dwMaxChargeTime;

							if( CheckElementAttack( pOwner ) )
								return;
						}
						else
						{
							SetFireState( pOwner, ioAttackableItem::MET_NONE, true );
							m_fCurChargingRate = FLOAT1;
						}
					}
				}
				else
				{
					SetFireState( pOwner, ioAttackableItem::MET_NONE, true );
				}
			}
		}
		break;
	case CDS_FIRE:
		{
			DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

			if( dwEndTime > dwCurTime )
			{
				if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
				{
					// 예약체크
					CheckKeyReserve( pOwner );

					// Check Reload
					if( !m_bReloadCheck )
					{
						m_bReloadCheck = true;
						CheckReload( pOwner );
					}
				}
			}
			else
			{
				ProcessKeyReserve( pOwner );
				return;
			}
		}
		break;
	}
}

void ioAssaultChargeDash::CheckDirKey( ioBaseChar *pOwner )
{
	if( pOwner && pOwner->IsNeedProcess() )
	{
		if( pOwner->IsDirKeyRelease( m_CurDirKey ) )
			m_bOnCharging = false;
	}
}

void ioAssaultChargeDash::SetChargingState( ioBaseChar* pOwner )
{
	if( m_DashState == CDS_CHARGING )
		return;

	DWORD	dwCurTime = FRAMEGETTIME();
	m_DashState = CDS_CHARGING;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_fLoopAniRate;
	m_dwLoopStartTime = dwCurTime;
	if( m_dwLoopDuration > 0 )
	{
		m_dwLoopEndTime = dwCurTime + m_dwLoopDuration;
	}
	m_dwGatheringStartTime = dwCurTime;

	if( m_dwGatheringEffectID == -1 && !m_szGatheringEffect.IsEmpty() )
	{
		ioEffect *pEffect = pOwner->AttachEffect( m_szGatheringEffect );
		if( pEffect )
			m_dwGatheringEffectID = pEffect->GetUniqueID();
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioAssaultChargeDash::SetFireState( ioBaseChar *pOwner, int iSubType, bool bSend )
{
	if( m_DashState == CDS_FIRE )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_DashState = CDS_FIRE;
	m_dwActionEndTime = dwCurTime;

	if( m_dwGatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_dwGatheringEffectID );
		m_dwGatheringEffectID = -1;
	}

	AttackAttribute kAttribute = m_AttackAttribute;

	switch( iSubType )
	{
	case ioAttackableItem::MET_FIRE:
		kAttribute = m_MagicElementAtt.m_FireAttack;
		break;
	case ioAttackableItem::MET_ICE:
		kAttribute = m_MagicElementAtt.m_IceAttack;
		break;
	case ioAttackableItem::MET_LIGHT:
		kAttribute = m_MagicElementAtt.m_LightAttack;
		break;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( kAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = kAttribute.m_fAttackAniRate;

	pOwner->SetNoWoundState( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
								 kAttribute.m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 kAttribute.m_dwPreDelay );

	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)kAttribute.m_dwPreDelay, true );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwDuration = kAttribute.m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + kAttribute.m_dwEndDelay;
	m_dwActionEndTime += dwDuration;

	m_dwReserveEnableTime = 0;
	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( kAttribute.m_vForceInfoList, iAniID, fTimeRate, kAttribute.m_dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, kAttribute.m_dwPreDelay );

	DWORD dwTrackingTime = dwCurTime + kAttribute.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_fCurChargingRate;
		kPacket << pOwner->GetTargetRot();
		kPacket << iSubType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioAssaultChargeDash::CheckElementAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !m_bUseDashElementAtt )
		return false;

	if( pOwner->IsAttackKey() )
	{
		SetFireState( pOwner, ioAttackableItem::MET_FIRE, true );
		return true;
	}
	else if( pOwner->IsDefenseKey() )
	{
		SetFireState( pOwner, ioAttackableItem::MET_ICE, true );
		return true;
	}
	else if( pOwner->IsJumpKey() )
	{
		SetFireState( pOwner, ioAttackableItem::MET_LIGHT, true );
		return true;
	}

	return false;
}
