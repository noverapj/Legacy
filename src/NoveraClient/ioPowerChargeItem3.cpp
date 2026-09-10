

#include "stdafx.h"

#include "ioPowerChargeItem3.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioPowerChargeItem3::ioPowerChargeItem3()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_bSetChargingMoveAni = false;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_bPoseChange = false;
	m_dwPoseEffectIndex = 0;
}

ioPowerChargeItem3::ioPowerChargeItem3( const ioPowerChargeItem3 &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
  m_RemoveMaxAttribute( rhs.m_RemoveMaxAttribute ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_GatheringEffect( rhs.m_GatheringEffect ),
  m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
  m_ChargingMoveAnimation( rhs.m_ChargingMoveAnimation ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
  m_GatheringMaxBuff( rhs.m_GatheringMaxBuff ),
  m_RetreatAttribute( rhs.m_RetreatAttribute ),
  m_iDisableWeaponType( rhs.m_iDisableWeaponType ),
  m_bEnableMaxCharge( rhs.m_bEnableMaxCharge ),
  m_AttackFly( rhs.m_AttackFly ),
  m_szPoseSetAni( rhs.m_szPoseSetAni ),
  m_fPoseSetAniRate( rhs.m_fPoseSetAniRate ),
  m_PoseAttribute( rhs.m_PoseAttribute ),
  m_PoseNextAttribute( rhs.m_PoseNextAttribute ),
  m_szPoseEffect( rhs.m_szPoseEffect )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_bSetChargingMoveAni = false;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_bPoseChange = false;
	m_dwPoseEffectIndex = 0;
}

ioPowerChargeItem3::~ioPowerChargeItem3()
{
}

void ioPowerChargeItem3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

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

	wsprintf_e( szBuf, "remove_max_attack" );
	LoadAttackAttribute( szBuf, m_RemoveMaxAttribute, rkLoader );

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

	int iMaxBuff = rkLoader.LoadInt_e( "gathering_max_buff_cnt", 0 );
	m_GatheringMaxBuff.clear();
	for( i=0; i < iMaxBuff; ++i )
	{
		ioHashString szBuffName;

		wsprintf_e( szKey, "gathering_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		szBuffName = szBuf;

		m_GatheringMaxBuff.push_back( szBuffName );
	}

	// retreat
	LoadAttackAttribute_e( "retreat_attack", m_RetreatAttribute, rkLoader );

	m_iDisableWeaponType = rkLoader.LoadInt_e( "disable_weapon_type", 0 );
	m_bEnableMaxCharge = rkLoader.LoadBool_e( "enable_max_charge", false );

	m_AttackFly.Init();

	rkLoader.LoadString_e( "fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_StartAni = szBuf;
	m_AttackFly.m_fStartAniRate = rkLoader.LoadFloat_e( "fly_start_ani_rate", FLOAT1 );
	m_AttackFly.m_fStartAngle = rkLoader.LoadFloat_e( "fly_start_angle", 0.0f );
	m_AttackFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "fly_start_max_range", 0.0f );
	m_AttackFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "fly_start_duration", 0 );

	rkLoader.LoadString_e( "fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_DelayAni = szBuf;
	m_AttackFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "fly_delay_ani_rate", FLOAT1 );
	m_AttackFly.m_dwDelayDurationMin = (DWORD)rkLoader.LoadInt_e( "fly_delay_duration_min", 0 );

	rkLoader.LoadString_e( "landing_animation", "", szBuf, MAX_PATH );
	m_AttackFly.m_szLandingAni = szBuf;
	m_AttackFly.m_fLandingAniRate = rkLoader.LoadFloat_e( "landing_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_MoveAni = szBuf;
	m_AttackFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "fly_move_ani_rate", FLOAT1 );
	m_AttackFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "fly_rotate_speed", 0 );
	m_AttackFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_AttackFly.m_fDownSpeed = rkLoader.LoadFloat_e( "fly_down_speed", 0.0f );
	m_AttackFly.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "fly_jump_amt", 0.0f );

	rkLoader.LoadString_e( "pose_set_ani", "", szBuf, MAX_PATH );
	m_szPoseSetAni = szBuf;
	m_fPoseSetAniRate = rkLoader.LoadFloat_e( "pose_set_ani_rate", 0 );
	LoadAttackAttribute_e( "pose_change_attack", m_PoseAttribute, rkLoader );
	LoadAttackAttribute_e( "pose_change_next_attack", m_PoseNextAttribute, rkLoader );

	rkLoader.LoadString_e( "pose_effect", "", szBuf, MAX_PATH );
	m_szPoseEffect = szBuf;
}

ioItem* ioPowerChargeItem3::Clone()
{
	return new ioPowerChargeItem3( *this );
}

ioWeaponItem::WeaponSubType ioPowerChargeItem3::GetSubType() const
{
	return WST_POWER_CHARGE3;
}

void ioPowerChargeItem3::OnReleased( ioBaseChar *pOwner )
{
	int iBuffCnt = m_GatheringMaxBuff.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->RemoveBuff( m_GatheringMaxBuff[i] );
	}
	DestroyPoseEffect( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

void ioPowerChargeItem3::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwRetreatEndTime = 0;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();

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
	if( m_bPoseChange )
	{
		ChangeToPoseChangeAttack( pOwner );
	}
	else if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
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
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioPowerChargeItem3::CheckNormalAttack( ioBaseChar *pOwner )
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

				if( m_dwKeyReserveTime > 0 &&
					m_dwKeyReserveTime < dwCurTime &&
					pOwner->IsDirKeyDoubleClick() &&
					!pOwner->HasBuff(BT_DISABLE_DASH) )
				{
					ChangeToRetreatAttack( pOwner );

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << (int)m_ChargeState;
						kPacket << m_iCurCombo;
						kPacket << pOwner->GetTargetRot();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
					return;
				}
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_NORMAL_ATTACK:
	case CS_POSE_CHANGE_ATTACK:
	case CS_POSE_CHANGE_NEXT_ATTACK:
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
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	case CS_POSE_CHANGE_PRE:
		SetPoseChangeState( pOwner );
		break;
	case CS_POSE_CHANGE:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
			pOwner->SetState( CS_DELAY );
			return;
		}
		break;
	}

	CheckChargingMoveState( pOwner );
}

void ioPowerChargeItem3::OnCharging( ioBaseChar *pOwner )
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

void ioPowerChargeItem3::OnGathering( ioBaseChar *pOwner )
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
		if( m_bEnableMaxCharge && m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime )
		{
			ChangeToAttackFire( pOwner, true );
			g_TutorialMgr.ActionChargeAttack();
			return;
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
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, true );
		g_TutorialMgr.ActionChargeAttack();
	}
}

void ioPowerChargeItem3::ChangeToGathering( ioBaseChar *pOwner )
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

void ioPowerChargeItem3::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();
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

	m_dwCancelCheckTime = dwCurTime;
	m_dwInputCancelCheckTime = dwCurTime;

	bool bMaxAttack = false;
	bool bMaxBuffRemove = false;
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

			bool bCheckMax = false;
			if( m_bEnableMaxCharge && m_fCurChargeRate == FLOAT1 )
				bCheckMax = true;

			if( bCheckMax && CheckHasMaxBuff( pOwner ) && !m_RemoveMaxAttribute.m_AttackAnimation.IsEmpty() )
			{
				bMaxAttack = true;
				bMaxBuffRemove = true;

				pOwner->SetNormalAttackByAttribute( m_RemoveMaxAttribute, true, fCurChargeAniRate, fCurForceSlidingRate );

				m_szCurAni = m_RemoveMaxAttribute.m_AttackAnimation;
				m_fCurAniRate = m_RemoveMaxAttribute.m_fAttackAniRate * fCurChargeAniRate;

				int iBuffCnt = m_GatheringMaxBuff.size();
				for( int i=0; i < iBuffCnt; ++i )
				{
					pOwner->RemoveBuff( m_GatheringMaxBuff[i] );
				}
			}
			else
			{
				bool bHasWeapon = false;
				if( m_iDisableWeaponType > 0 )
				{
					bHasWeapon = g_WeaponMgr.HasWeaponType( pOwner, m_iDisableWeaponType );
				}

				if( !bHasWeapon && bCheckMax && !m_ExtendMaxAttribute.m_AttackAnimation.IsEmpty() )
				{
					bMaxAttack = true;
					pOwner->SetNormalAttackByAttribute( m_ExtendMaxAttribute, true, fCurChargeAniRate, fCurForceSlidingRate );

					m_szCurAni = m_ExtendMaxAttribute.m_AttackAnimation;
					m_fCurAniRate = m_ExtendMaxAttribute.m_fAttackAniRate * fCurChargeAniRate;

					int iBuffCnt = m_GatheringMaxBuff.size();
					for( int i=0; i < iBuffCnt; ++i )
					{
						ioHashString szBuffName = m_GatheringMaxBuff[i];
						if( !szBuffName.IsEmpty() )
						{
							pOwner->AddNewBuffWithAnimateTime( szBuffName, pOwner->GetCharName(), GetName(), NULL, 0 );
						}
					}
				}
				else
				{
					pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo], true, fCurChargeAniRate, fCurForceSlidingRate );

					m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
					m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate * fCurChargeAniRate;
				}

				ioEntityGroup *pGrp = pOwner->GetGroup();
				int iAniID = pGrp->GetAnimationIdx( m_szCurAni );
				
				m_dwKeyReserveTime = dwCurTime;
				m_dwKeyReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fCurAniRate;
			}
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

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;
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
		kPacket << bMaxBuffRemove;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeItem3::ApplyAttackFire( ioBaseChar *pOwner, bool bCharged, bool bMaxAttack, bool bMaxBuffRemove )
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

			if( bMaxBuffRemove )
			{
				pOwner->SetNormalAttackByAttribute( m_RemoveMaxAttribute, true, fCurChargeAniRate, fCurForceSlidingRate );

				int iBuffCnt = m_GatheringMaxBuff.size();
				for( int i=0; i < iBuffCnt; ++i )
				{
					pOwner->RemoveBuff( m_GatheringMaxBuff[i] );
				}
			}
			else
			{
				if( bMaxAttack )
				{
					pOwner->SetNormalAttackByAttribute( m_ExtendMaxAttribute, true, fCurChargeAniRate, fCurForceSlidingRate );

					int iBuffCnt = m_GatheringMaxBuff.size();
					for( int i=0; i < iBuffCnt; ++i )
					{
						ioHashString szBuffName = m_GatheringMaxBuff[i];
						if( !szBuffName.IsEmpty() )
						{
							pOwner->AddNewBuffWithAnimateTime( szBuffName, pOwner->GetCharName(), GetName(), NULL, 0 );
						}
					}
				}
				else
					pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo], true, fCurChargeAniRate, fCurForceSlidingRate );
			}
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

void ioPowerChargeItem3::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

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
		{
			bool bCharged;
			bool bMaxAttack;
			bool bMaxBuffRemove;

			rkPacket >> bCharged >> bMaxAttack >> bMaxBuffRemove;
			ApplyAttackFire( pOwner, bCharged, bMaxAttack, bMaxBuffRemove );
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
	case CS_POSE_CHANGE:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );
			SetPoseChangeState( pOwner );
		}
		break;
	case CS_POSE_CHANGE_ATTACK:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToPoseChangeAttack( pOwner );
		}
		break;
	case CS_POSE_CHANGE_NEXT_ATTACK:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToPoseChangeNextAttack( pOwner );
		}
		break;
	}
}

void ioPowerChargeItem3::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	float fCurScaleRate, fCurExplosionRate, fCurLiveTimeRate;
	
	fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
	fCurScaleRate = fCurExplosionRate = fCurLiveTimeRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f && !bJumpAttack )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );

		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
		
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );
	
		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
        fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurRangeRate += fCurMaxRange * m_fCurChargeRate;
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;

		fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

		fCurScaleRate += fCurMaxScale * m_fCurChargeRate;
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRate;
		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );

		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );

		pWeapon->SetOwnerChargeScaleRate( fCurScaleRate );
		pWeapon->SetOwnerChargeExplosionRate( fCurExplosionRate );
		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
	}

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

bool ioPowerChargeItem3::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioPowerChargeItem3::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioPowerChargeItem3::CheckChargingMoveState( ioBaseChar *pOwner )
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

void ioPowerChargeItem3::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
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

bool ioPowerChargeItem3::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioPowerChargeItem3::GetAutoTargetValue( ioBaseChar *pOwner,
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

float ioPowerChargeItem3::GetChargeDamageRateForSkill()
{
	float fCurDamageRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		fCurDamageRate += fCurMaxDamage * m_fCurChargeRateForSkill;
	}

	return fCurDamageRate;
}

float ioPowerChargeItem3::GetChargeForceRateForSkill()
{
	float fCurForceRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		fCurForceRate += fCurMaxForce * m_fCurChargeRateForSkill;
	}

	return fCurForceRate;
}

float ioPowerChargeItem3::GetChargeBlowRateForSkill()
{
	float fCurBlowRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRateForSkill;
	}

	return fCurBlowRate;
}

float ioPowerChargeItem3::GetChargeRangeRateForSkill()
{
	float fCurRangeRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		fCurRangeRate += fCurMaxRange * m_fCurChargeRateForSkill;
	}

	return fCurRangeRate;
}

float ioPowerChargeItem3::GetChargeSpeedRateForSkill()
{
	float fCurSpeedRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRateForSkill;
	}

	return fCurSpeedRate;
}

float ioPowerChargeItem3::GetChargeFloatRateForSkill()
{
	float fCurFloatRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
		fCurFloatRate += fCurMaxFloat * m_fCurChargeRateForSkill;
	}

	return fCurFloatRate;
}

float ioPowerChargeItem3::GetChargeScaleRateForSkill()
{
	float fCurScaleRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurChargeRateForSkill;
	}

	return fCurScaleRate;
}

float ioPowerChargeItem3::GetChargeExplosionRateForSkill()
{
	float fCurExplosionRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRateForSkill;
	}

	return fCurExplosionRate;
}

void ioPowerChargeItem3::ClearCurChargeRateForSkill()
{
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
}

bool ioPowerChargeItem3::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
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

void ioPowerChargeItem3::ChangeToRetreatAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

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

	m_dwRetreatEndTime += pGrp->GetAnimationFullTime( iAniID );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pOwner->SetReservedSliding( m_RetreatAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
}

void ioPowerChargeItem3::OnRetreatState( ioBaseChar *pOwner )
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

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
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

bool ioPowerChargeItem3::CheckHasMaxBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_GatheringMaxBuff.empty() )
		return false;

	int iSize = m_GatheringMaxBuff.size();
	for( int i=0; i < iSize; ++i )
	{
		if( pOwner->HasBuff( m_GatheringMaxBuff[i] ) )
			return true;
	}

	return false;
}

void ioPowerChargeItem3::SetFlyState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_SMILE_JOKER_FLY );
	ChangeFlyStart( pOwner );
	m_fFlyCurRange = 0.0f;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_FlyState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeItem3::ChangeFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackFly.m_StartAni );
	float fTimeRate  = m_AttackFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_FlyState = SFS_START;

	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwFlyMoveEndTime = 0;
}

void ioPowerChargeItem3::ChangeFlyDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackFly.m_DelayAni );
	float fTimeRate  = m_AttackFly.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_FlyState = SFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwFlyMoveEndTime = 0;
}

void ioPowerChargeItem3::ChangeFlyEnd( ioBaseChar *pOwner, bool bFlyReset )
{
	if( !pOwner )
		return;

	m_FlyState = SFS_END;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	ioChargeComboJump3 *pChargeComboJump3 = ToChargeComboJump3( GetCurExtendJump() );
	if( pChargeComboJump3 && ( bFlyReset || fHeightGap <= FLOAT10 ) )
		pChargeComboJump3->ResetUsedFly();

	if( pOwner && pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_AttackFly.m_fFlyJumpAmt, m_fExtendLandingRate );
		pOwner->ClearDirDoubleClick();
	}
}

void ioPowerChargeItem3::ChangeFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = 0;
	float fTimeRate = 0;
	m_dwFlyMoveEndTime = 0;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
	{
		iAniID = pGrp->GetAnimationIdx( m_AttackFly.m_DelayAni );
		fTimeRate = m_AttackFly.m_fDelayAniRate;
	}
	else
	{
		iAniID = pGrp->GetAnimationIdx( m_AttackFly.m_MoveAni );
		fTimeRate = m_AttackFly.m_fMoveAniRate;
	}
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioPowerChargeItem3::SetChangeLandState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_iCurCombo = 0;
	ClearKeyReserve();
	m_FlyState = SFS_LAND;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pOwner->ClearAttackFireTimeAndSkill();

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackFly.m_szLandingAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_AttackFly.m_fLandingAniRate, 0.0f, 0.0f, true );

	m_dwFlyMoveEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_AttackFly.m_fLandingAniRate );
}

void ioPowerChargeItem3::SetChangeLandEndState( ioBaseChar *pOwner, bool bSend )
{
	m_iCurCombo = 0;
	ClearKeyReserve();
	ClearState( pOwner );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetState( CS_DELAY );

	ioChargeComboJump3 *pChargeComboJump3 = ToChargeComboJump3( GetCurExtendJump() );
	if( pChargeComboJump3 )
		pChargeComboJump3->ResetUsedFly();

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_LAND_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeItem3::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_STATE:
		CheckFlyState( pOwner, rkPacket );
		break;
	case SST_ROTATE:
		{
			bool bChangeDir;
			rkPacket >> bChangeDir;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			int iDir;
			rkPacket >> iDir;

			pOwner->SetTargetRot( qtRot );
			if( bChangeDir )
			{
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
				ChangeFlyMove( pOwner );
			}
		}
		break;
	case SST_LAND_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			SetChangeLandEndState( pOwner, false );
		}
		break;
	}
}

void ioPowerChargeItem3::CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iFlyState;
	rkPacket >> iFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	if( iFlyState == SFS_START )
		SetFlyState( pOwner, false );
}

void ioPowerChargeItem3::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;
	}
	
	if( bChangeDir )
	{
		ChangeFlyMove( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_ROTATE;
			kPacket << bChangeDir;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioPowerChargeItem3::ClearState( ioBaseChar *pOwner )
{
	m_FlyState = SFS_NONE;
	m_dwFlyStartTime = 0;
	m_dwFlyMoveEndTime = 0;
}

bool ioPowerChargeItem3::IsFlyCharCollisionSkipState()
{
	switch( m_FlyState )
	{
	case SFS_START:
	case SFS_DELAY:
		return true;
	}

	return false;
}

void ioPowerChargeItem3::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FlyState )
	{
	case SFS_START:
		pOwner->SetCurMoveSpeed( 0 );
		if( m_dwFlyStartTime + m_AttackFly.m_dwStartDuration < dwCurTime )
		{
			ChangeFlyDelay( pOwner );
			return;
		}
		else if( !ProcessFlyStart( pOwner ) )
		{
			ChangeFlyDelay( pOwner );
			return;
		}
		break;
	case SFS_DELAY:
		if( pOwner->IsNeedProcess() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				// 스킬 존재체크
				ioSkill *pSkill = pOwner->GetEquipedSkill( iSkillNum );
				if( pSkill &&
					!ToPassiveSkill(pSkill) &&
					pSkill->IsCanJumpingSkill() &&
					!pOwner->IsChatModeState( true ) &&
					!pOwner->IsBuffLimitSkill() )
				{
					if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
						return;
				}
			}

			if( !pOwner->IsDefenseKeyDown() && m_dwFlyStartTime + m_AttackFly.m_dwDelayDurationMin < dwCurTime )
			{
				ChangeFlyEnd( pOwner, false );
				return;
			}
		}
		if( !ProcessFlyMove( pOwner ) )
		{
			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		break;
	case SFS_LAND:
		if( m_dwFlyMoveEndTime > 0 && m_dwFlyMoveEndTime < FRAMEGETTIME() )
		{
			SetChangeLandEndState( pOwner, true );
		}
		break;
	case SFS_END:
		break;
	}
}

bool ioPowerChargeItem3::ProcessFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_AttackFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_AttackFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_AttackFly.m_fStartMaxRange / (float)m_AttackFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_AttackFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_AttackFly.m_fStartMaxRange - m_fFlyCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	return bRangeLimit;
}

bool ioPowerChargeItem3::ProcessFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurMoveSpeed = m_AttackFly.m_fMoveSpeed * fTimePerSec;
	D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

	bool bCol = false;
	if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
		pOwner->Translate( vMoveAmt );

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
	float fCurUpDownSpeed = m_AttackFly.m_fDownSpeed * fTimePerSec;
	vCurPos.y -= fCurUpDownSpeed;
	if( vCurPos.y - fMapHeight < FLOAT10 )
	{
		vCurPos.y = fMapHeight;
		SetChangeLandState( pOwner );
		return true;
	}
	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );
	return false;
}

void ioPowerChargeItem3::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_AttackFly.m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_AttackFly.m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

void ioPowerChargeItem3::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( bFullTime && !m_szPoseSetAni.IsEmpty() && !m_bPoseChange )
	{
		SetPoseChangePreState( pOwner );
		return;
	}
	m_bPoseChange = false;
	DestroyPoseEffect( pOwner );

	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

void ioPowerChargeItem3::SetPoseChangePreState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	if( pOwner->IsNeedProcess() )
		pOwner->SetCurNormalAttackItem( this );
	if( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	m_ChargeState = CS_POSE_CHANGE_PRE;	
}

void ioPowerChargeItem3::SetPoseChangeState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	m_ChargeState = CS_POSE_CHANGE;	
	m_bPoseChange = true;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szPoseSetAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fPoseSetAniRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fPoseSetAniRate;

	CreatePoseEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeItem3::ChangeToPoseChangeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
		pOwner->SetCurNormalAttackItem( this );
	if( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();
	const AttackAttribute *pAttribute = &m_PoseAttribute;
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}
	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_POSE_CHANGE_ATTACK;
	m_bPoseChange = false;
	DestroyPoseEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioPowerChargeItem3::GetMaxCombo() const
{
	int iMaxCombo = ioAttackableItem::GetMaxCombo();
	if( m_bPoseChange )
		iMaxCombo = 1;

	return iMaxCombo;
}

void ioPowerChargeItem3::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( !bDefense )
	{
		ChangeToPoseChangeNextAttack( pAttacker );
	}	
}

void ioPowerChargeItem3::ChangeToPoseChangeNextAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
		pOwner->SetCurNormalAttackItem( this );
	if( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();
	const AttackAttribute *pAttribute = &m_PoseNextAttribute;
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}
	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_POSE_CHANGE_NEXT_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeItem3::CreatePoseEffect( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwPoseEffectIndex == 0 )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioEffect *pEffect = pGrp->AttachEffect( m_szPoseEffect, NULL, ioMath::QUAT_IDENTITY );
			if( !pEffect )	
				return;

			m_dwPoseEffectIndex = pEffect->GetUniqueID();
		}
	}
}

void ioPowerChargeItem3::DestroyPoseEffect( ioBaseChar *pChar )
{
	if( m_dwPoseEffectIndex != 0 )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwPoseEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();

		m_dwPoseEffectIndex = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
ioSmileJokerFlyState::ioSmileJokerFlyState()
{
}

ioSmileJokerFlyState::~ioSmileJokerFlyState()
{
}

void ioSmileJokerFlyState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSmileJokerFlyState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if ( eNewState != CS_SMILE_JOKER_FLY )
	{
		ioPowerChargeItem3 *pPowerItem3 = ToPowerChargeItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pPowerItem3 )
			pPowerItem3->ClearState( pOwner );
	}
}

bool ioSmileJokerFlyState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioPowerChargeItem3 *pPowerItem3 = ToPowerChargeItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPowerItem3 && pPowerItem3->IsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioSmileJokerFlyState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioPowerChargeItem3 *pPowerItem3 = ToPowerChargeItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPowerItem3 )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
				fPreHeightForGhostState = pOwner->GetBottomHeight();

			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}
		pPowerItem3->ProcessFlyState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSmileJokerFlyState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSmileJokerFlyState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}