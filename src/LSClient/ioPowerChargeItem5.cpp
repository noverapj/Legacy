

#include "stdafx.h"

#include "ioPowerChargeItem5.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioPowerChargeItem5::ioPowerChargeItem5()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwAttackLandEndTime = 0;
	m_dwFlightTime = 0;

	m_fCharMoveSpeed = 0.0f;
	m_fCurCharMoveSpeed = 0.0f;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	m_bChargeMove = false;
	m_bFirstChargeMoveMotion = true;
	m_bChargeMotionSetted = false;
	m_bChangeDir = false;
	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bUsedExtraGauge = false;
}

ioPowerChargeItem5::ioPowerChargeItem5( const ioPowerChargeItem5 &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
  m_AttackLandAttribute( rhs.m_AttackLandAttribute ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_GatheringEffect( rhs.m_GatheringEffect ),
  m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
  m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
  m_fChargeJumpPower( rhs.m_fChargeJumpPower ),
  m_fChargeGravityRate( rhs.m_fChargeGravityRate ),
  m_fChargeMaxRange( rhs.m_fChargeMaxRange ),
  m_fChargeBaseRange( rhs.m_fChargeBaseRange ),
  m_fDashMoveSpeed( rhs.m_fDashMoveSpeed ),
  m_ChargeMoveFront( rhs.m_ChargeMoveFront ),
  m_ChargeMoveBack( rhs.m_ChargeMoveBack ),
  m_ChargeMoveLeft( rhs.m_ChargeMoveLeft ),
  m_ChargeMoveRight( rhs.m_ChargeMoveRight ),
  m_fChargeMoveAniRate( rhs.m_fChargeMoveAniRate ),
  m_fChargeMoveSpeedRate( rhs.m_fChargeMoveSpeedRate ),
  m_bChargeMove( rhs.m_bChargeMove ),
  m_JumpChargeAniList( rhs.m_JumpChargeAniList ),
  m_ExtraGaugeBuffList( rhs.m_ExtraGaugeBuffList ),
  m_AreaWeaponList( rhs.m_AreaWeaponList ),
  m_iAreaWeaponMax( rhs.m_iAreaWeaponMax ),
  m_fJumpChargeAttackAniRate( rhs.m_fJumpChargeAttackAniRate ),
  m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
  m_bEnableExtraGauge( rhs.m_bEnableExtraGauge )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwAttackLandEndTime = 0;
	m_dwFlightTime = 0;

	m_fCharMoveSpeed = 0.0f;
	m_fCurCharMoveSpeed = 0.0f;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	m_bFirstChargeMoveMotion = true;
	m_bChargeMotionSetted = false;
	m_bChangeDir = false;
	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bUsedExtraGauge = false;
}

ioPowerChargeItem5::~ioPowerChargeItem5()
{
}

void ioPowerChargeItem5::LoadProperty( ioINILoader &rkLoader )
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

	m_fChargeJumpPower = rkLoader.LoadFloat_e( "charge_jump_power", 0.0f );
	m_fChargeGravityRate = rkLoader.LoadFloat_e( "charge_gravity_rate", FLOAT1 );
	m_fChargeMaxRange = rkLoader.LoadFloat_e( "charge_max_range", 0.0f );
	m_fChargeBaseRange = rkLoader.LoadFloat_e( "charge_base_range", 0.0f );
	m_fDashMoveSpeed = rkLoader.LoadFloat_e( "extend_run_charge_move_speed", 0.0f );

	wsprintf_e( szBuf, "attack_land_attack" );
	LoadAttackAttribute( szBuf, m_AttackLandAttribute, rkLoader );

	rkLoader.LoadString_e( "charge_move_front", "", szBuf, MAX_PATH );
	m_ChargeMoveFront = szBuf;
	rkLoader.LoadString_e( "charge_move_back", "", szBuf, MAX_PATH );
	m_ChargeMoveBack = szBuf;
	rkLoader.LoadString_e( "charge_move_left", "", szBuf, MAX_PATH );
	m_ChargeMoveLeft = szBuf;
	rkLoader.LoadString_e( "charge_move_right", "", szBuf, MAX_PATH );
	m_ChargeMoveRight = szBuf;

	m_fChargeMoveAniRate = rkLoader.LoadFloat_e( "charge_move_ani_rate", FLOAT1 );
	m_fChargeMoveSpeedRate = rkLoader.LoadFloat_e( "charge_move_speed_rate", 0.0f );
	m_bChargeMove = rkLoader.LoadBool_e( "enable_charge_move", false );
	
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );
	m_iAreaWeaponMax = rkLoader.LoadInt_e( "area_weapon_max", 0 );
	for( int i = 0; i < m_iAreaWeaponMax; ++i )
	{
		wsprintf_e( szKey, "area_weapon_name%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AreaWeaponList.push_back( ioHashString(szBuf) );

		wsprintf_e( szKey, "jump_charge_attack_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_JumpChargeAniList.push_back( ioHashString(szBuf) );
	}
	m_fJumpChargeAttackAniRate = rkLoader.LoadFloat_e( "jump_charge_attack_ani_rate", FLOAT1 );

	m_bEnableExtraGauge = rkLoader.LoadBool_e( "extra_gauge_enable", false );

	m_ExtraGaugeBuffList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "extra_gauge_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "extra_gauge_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ExtraGaugeBuffList.push_back( szBuf );
	}
}

ioItem* ioPowerChargeItem5::Clone()
{
	return new ioPowerChargeItem5( *this );
}

ioWeaponItem::WeaponSubType ioPowerChargeItem5::GetSubType() const
{
	return WST_POWER_CHARGE5;
}

void ioPowerChargeItem5::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCharMoveSpeed = 0.0f;
	m_fCurCharMoveSpeed = 0.0f;

	m_dwAttackLandEndTime = 0;
	m_dwFlightTime = 0;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

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
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
		m_bChangeDir = false;

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
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioPowerChargeItem5::OnReleased( ioBaseChar *pOwner )
{
	RemoveExtraGaugeBuff( pOwner );
	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioPowerChargeItem5::CheckNormalAttack( ioBaseChar *pOwner )
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
			pOwner->CheckExtraAniJump();

			OnAttackFire( pOwner );
		}
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_LAND:
		{
			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioPowerChargeItem5::OnCharging( ioBaseChar *pOwner )
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
		ChangeToNormalFire( pOwner );
	}
}

void ioPowerChargeItem5::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 점프키 누름. 점프상태 전환
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}
	else if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
	{
		pOwner->ChangeDirectionByInputDir( false );
		pOwner->SetDashState( false );
		pOwner->SendDashState( false );
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
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, true );
	}

	if( m_bChargeMove )
		CheckChargeMoveAniState( pOwner );
}

void ioPowerChargeItem5::CheckChargeMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstChargeMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_ChargeMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_ChargeMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_ChargeMoveLeft);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_ChargeMoveRight);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			
			m_bChargeMotionSetted = false;
			m_bFirstChargeMoveMotion = false;

			if( bChangeDir )
				m_bChangeDir = true;

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_ChargeMoveFront, FLOAT100, FLOAT1, m_fChargeMoveAniRate );
			}
			else
			{
				ioUserKeyInput::DirKeyInput eXformDir;
				eXformDir = pOwner->GetXformDir( m_ChargeStartDirKey );

				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_ChargeMoveFront, FLOAT100, FLOAT1, m_fChargeMoveAniRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_ChargeMoveRight, FLOAT100, FLOAT1, m_fChargeMoveAniRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_ChargeMoveBack, FLOAT100, FLOAT1, m_fChargeMoveAniRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_ChargeMoveLeft, FLOAT100, FLOAT1, m_fChargeMoveAniRate );
					break;
				}
			}
		}
		else
		{
			m_bChangeDir = false;
		}
	}
	else
	{
		if( !m_bChargeMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_ChargeMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_ChargeMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_ChargeMoveLeft);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_ChargeMoveRight);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bChargeMotionSetted = true;
			m_bChangeDir = false;
		}
	}
}

void ioPowerChargeItem5::OnAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	if( CheckLanding( pOwner ) )
	{
		ChangeToLand( pOwner );
	}
	else
	{
		ChargeAttackMove( pOwner, pStage );
	}
}

void ioPowerChargeItem5::ChangeToGathering( ioBaseChar *pOwner )
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

	if( m_bChargeMove )
	{
		pOwner->SetAttackMoveEnable( true );

		m_bFirstChargeMoveMotion = true;
		m_bChargeMotionSetted = true;
		m_ChargeStartDirKey = pOwner->GetCurDirKey();
	}
	else
		pOwner->SetAttackMoveEnable( false );

	float fCurGravity = pOwner->GetCharGravity();
	float fFlightDuration = GetFlightDuration( fCurGravity );
	if( fFlightDuration > 0.0f )
	{
		m_fCharMoveSpeed = (m_fChargeBaseRange+m_fChargeMaxRange) / (fFlightDuration / FLOAT1000);
	}

	m_dwFlightTime = (DWORD)fFlightDuration;

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

float ioPowerChargeItem5::GetAttackSpeedRate() const
{
	if( m_bChargeMove && !m_bFirstChargeMoveMotion && !m_bChargeMotionSetted && m_fChargeMoveSpeedRate > 0.f )
		return m_fChargeMoveSpeedRate;
	else
		return m_fAttackSpeedRate;
}

void ioPowerChargeItem5::ChangeToNormalFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

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

	m_szCurAni.Clear();
	m_fCurAniRate = FLOAT1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeItem5::ChangeToDashAttackFire( ioBaseChar *pOwner )
{
	m_fCharMoveSpeed = m_fDashMoveSpeed;
	m_dwGatheringStartTime = 0;
	m_fCurChargeRate = FLOAT1;
	m_fCurChargeRateForSkill = FLOAT1;

	ChangeToAttackFire( pOwner, false, true );
}

void ioPowerChargeItem5::ChangeToAttackFire( ioBaseChar *pOwner, bool bSendPacket, bool bChargeRateInit )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

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

	m_szCurAni.Clear();
	m_fCurAniRate = FLOAT1;

	if( !bChargeRateInit )
	{
		m_fCurChargeRate = 0.0f;
		m_fCurChargeRateForSkill = 0.0f;
	}
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

		int iAniID = -1;
		float fTimeRate = FLOAT1;
		DWORD dwPreDelay = 0;
		ioEntityGroup *pGrp = pOwner->GetGroup();

		pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo], true, fCurChargeAniRate, fCurForceSlidingRate );

		iAniID = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
		fTimeRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
		dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir.y = 0.0f;
	D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

	float fMaxRange = m_fChargeBaseRange + m_fChargeMaxRange;
	float fCurRange = m_fChargeBaseRange + m_fChargeMaxRange * m_fCurChargeRate;
	float fCurRate = FLOAT1;
	if( fMaxRange > 0.0f )
		fCurRate = fCurRange / fMaxRange;

	m_fCurCharMoveSpeed = m_fCharMoveSpeed * fCurRate;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
	pOwner->SetTargetRotAndMoveDirChange( qtRot, false );
	pOwner->SetJumpPower( m_fChargeJumpPower );

	m_ChargeState = CS_ATTACK_FIRE;

	if( bSendPacket && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << qtRot;
		kPacket << bChargeRateInit;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeItem5::ChangeToLand( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	ClearKeyReserve();
	pOwner->ClearAttackFireTimeAndSkill();

	int iAniID = pGrp->GetAnimationIdx( m_AttackLandAttribute.m_AttackAnimation );
	float fTimeRate = m_AttackLandAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_AttackLandAttribute.m_dwPreDelay;

	pOwner->SetNormalAttackByAttribute( m_AttackLandAttribute, true );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwAttackLandEndTime = dwCurTime;
	m_dwAttackLandEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwKeyReserveTime = 0;
	float fValue = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fValue > 0.0f )
	{
		m_dwKeyReserveTime = dwCurTime + (DWORD)fValue;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	m_ChargeState = CS_LAND;
}

void ioPowerChargeItem5::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

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
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot, false );

			bool bChargeRateInit;
			rkPacket >> bChargeRateInit;
			ChangeToAttackFire( pOwner, false, bChargeRateInit );
		}
		break;
	case CS_NORMAL_ATTACK:
		ChangeToNormalFire( pOwner );
		break;
	}
}

void ioPowerChargeItem5::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

	ioBaseChar *pOwner = pWeapon->GetOwner();

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( pOwner )
		{
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}

	bool bInputLiveTime = false;
	DWORD dwLiveTime = 0;

	ioChargeComboJump2 *pCharComboJump2 = ToChargeComboJump2( GetCurExtendJump() );
	ioChargeComboJump4 *pCharComboJump4 = ToChargeComboJump4( GetCurExtendJump() );
	ioChargeComboDefenseJump *pCharComboDefense = ToChargeComboDefenseJump( GetCurExtendJump() );

	if( IsNoDropState( pOwner ) )
	{
		bInputLiveTime = true;
		dwLiveTime = m_dwFlightTime * 10;
	}
	else if( pOwner && pOwner->GetState() == CS_JUMP )
	{
		if( pOwner->GetJumpState() == JS_JUMP_ATTACK )
		{
			if( pCharComboJump2 &&
				pCharComboJump2->GetCurChargeComboState() == ioChargeComboJump2::CCS_EXTEND_ATTACK )
			{
				bInputLiveTime = true;
				dwLiveTime = pCharComboJump2->GetFlightTime() * 10;
			}
			else if( pCharComboJump4 && pCharComboJump4->GetCurChargeComboState() == ioChargeComboJump4::CCS_EXTEND_ATTACK )
			{
				bInputLiveTime = true;
				dwLiveTime = pCharComboJump4->GetFlightTime() * 10;
			}
			else if( pCharComboDefense && pCharComboDefense->GetCurChargeComboState() == ioChargeComboDefenseJump::CCS_EXTEND_ATTACK )
			{
				bInputLiveTime = true;
				dwLiveTime = pCharComboDefense->GetFlightTime() * 10;
			}
		}
	}

	if( bInputLiveTime )
	{
		pWeapon->SetLiveTime( dwLiveTime );
	}
}

bool ioPowerChargeItem5::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioPowerChargeItem5::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioPowerChargeItem5::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

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

bool ioPowerChargeItem5::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioPowerChargeItem5::GetAutoTargetValue( ioBaseChar *pOwner,
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

float ioPowerChargeItem5::GetChargeDamageRateForSkill()
{
	float fCurDamageRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		fCurDamageRate += fCurMaxDamage * m_fCurChargeRateForSkill;
	}

	return fCurDamageRate;
}

float ioPowerChargeItem5::GetChargeForceRateForSkill()
{
	float fCurForceRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		fCurForceRate += fCurMaxForce * m_fCurChargeRateForSkill;
	}

	return fCurForceRate;
}

float ioPowerChargeItem5::GetChargeBlowRateForSkill()
{
	float fCurBlowRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRateForSkill;
	}

	return fCurBlowRate;
}

float ioPowerChargeItem5::GetChargeRangeRateForSkill()
{
	float fCurRangeRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		fCurRangeRate += fCurMaxRange * m_fCurChargeRateForSkill;
	}

	return fCurRangeRate;
}

float ioPowerChargeItem5::GetChargeSpeedRateForSkill()
{
	float fCurSpeedRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRateForSkill;
	}

	return fCurSpeedRate;
}

float ioPowerChargeItem5::GetChargeFloatRateForSkill()
{
	float fCurFloatRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
		fCurFloatRate += fCurMaxFloat * m_fCurChargeRateForSkill;
	}

	return fCurFloatRate;
}

float ioPowerChargeItem5::GetChargeScaleRateForSkill()
{
	float fCurScaleRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurChargeRateForSkill;
	}

	return fCurScaleRate;
}

float ioPowerChargeItem5::GetChargeExplosionRateForSkill()
{
	float fCurExplosionRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRateForSkill;
	}

	return fCurExplosionRate;
}

void ioPowerChargeItem5::ClearCurChargeRateForSkill()
{
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
}

bool ioPowerChargeItem5::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( !pChar->CheckLandingInfo( fHeightGap ) )
		return false;

	return true;
}

float ioPowerChargeItem5::GetFlightDuration( float fCurGravity )
{
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fChargeGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = m_fChargeJumpPower / fNewGravity;
		fTopHeight = (m_fChargeJumpPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fResult = 2 * fTopTime * FLOAT1000;

	return fResult;
}

void ioPowerChargeItem5::ChargeAttackMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCurCharMoveSpeed == 0.0f ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vCharMoveDir * m_fCurCharMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			pChar->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );

}

bool ioPowerChargeItem5::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	if( !IsChargeRotate() )
		return false;

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

bool ioPowerChargeItem5::IsNoDropState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			return true;
		}
	}

	return false;
}

DWORD ioPowerChargeItem5::GetJumpChargeTime( ioBaseChar *pChar ) const
{
	return m_dwJumpChargeTime;
}

void ioPowerChargeItem5::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( pOwner )
	{
		int eState = pOwner->GetPreState();
		if( bFullTime && m_dwJumpChargeTime > 0 && !m_bUsedExtraGauge && m_fCurExtraGauge >= m_fMaxExtraGauge &&
			( eState == CS_DELAY || eState == CS_RUN || eState == CS_SPECIAL_MOTION || eState == CS_ETCITEM_MOTION ) )
		{
			ChangeToSpecialReady( pOwner, true );
			return;
		}
	}
	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

void ioPowerChargeItem5::ChangeToSpecialReady( ioBaseChar *pOwner, bool bSendPacket )
{
	if ( !pOwner )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_POWERCHARGE_SPECIAL );
	m_ChargeState = CS_SPECIAL_READY;
	m_iCurCombo = 0;
	m_dwJumpChargeEndTime = FRAMEGETTIME();
	m_bUsedExtraGauge = true;

	if( bSendPacket && pOwner->IsNeedSendNetwork() )
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

void ioPowerChargeItem5::ChangeToSpecialAttack( ioBaseChar *pOwner, bool bSendPacket )
{
	if ( !pOwner )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	if( pOwner->GetState() != CS_POWERCHARGE_SPECIAL )
		pOwner->SetState( CS_POWERCHARGE_SPECIAL );

	m_ChargeState = CS_SPECIAL_ATTACK;
	
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iCount = m_JumpChargeAniList.size();
	int iWeaponIndex = m_iCurCombo;
	if( iCount <= iWeaponIndex )
		iWeaponIndex = 0;

	int iAniID = pGrp->GetAnimationIdx( m_JumpChargeAniList[iWeaponIndex] );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fJumpChargeAttackAniRate );
	
	m_dwJumpChargeEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fJumpChargeAttackAniRate;
	m_dwJumpChargeFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_fJumpChargeAttackAniRate;
	m_dwReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fJumpChargeAttackAniRate;
	m_bReserveKeyInput = false;
	m_iCurCombo++;

	if( bSendPacket && pOwner->IsNeedSendNetwork() )
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

void ioPowerChargeItem5::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_SPECIAL_READY:
		if( m_dwJumpChargeEndTime != 0 && m_dwJumpChargeEndTime < dwCurTime )
			ChangeToSpecialAttack( pOwner, true );
		break;

	case CS_SPECIAL_ATTACK:
		if( m_dwJumpChargeFireTime != 0 && m_dwJumpChargeFireTime < dwCurTime )
		{
			CreateArea( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			m_dwJumpChargeFireTime = 0;
		}
		else if( m_dwReserveKeyTime != 0 && m_dwReserveKeyTime < dwCurTime && 
			m_iCurCombo < m_iAreaWeaponMax && pOwner->IsAttackKey() )
		{
			m_dwReserveKeyTime = 0;
			m_bReserveKeyInput = true;
		}
		else if( m_dwJumpChargeEndTime != 0 && m_dwJumpChargeEndTime < dwCurTime )
		{
			if( m_bReserveKeyInput )
			{
				ChangeToSpecialAttack( pOwner, true );
			}
			else
			{
				m_ChargeState = CS_NONE;
				m_dwJumpChargeEndTime = 0;
				pOwner->SetState( CS_DELAY );
			}
		}
		break;
	}
}

void ioPowerChargeItem5::ClearSpecialState( ioBaseChar *pOwner )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_dwJumpChargeEndTime = 0;
	m_dwJumpChargeFireTime = 0;
	m_dwReserveKeyTime = 0;
	m_bReserveKeyInput = false;
	m_iCurCombo = 0;
}

void ioPowerChargeItem5::CreateArea( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int iCount = m_AreaWeaponList.size();
	int iWeaponIndex = m_iCurCombo - 1;
	if( iWeaponIndex < 0 )
		iWeaponIndex = 0;

	if( iCount <= iWeaponIndex )
		return;	

	g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", GetName(), m_AreaWeaponList[iWeaponIndex], 
									  pOwner->GetWorldPosition(), pOwner->GetTargetRot(), ioAreaWeapon::CT_NORMAL );
}

int ioPowerChargeItem5::GetMaxBullet()
{
	if( m_bEnableExtraGauge )
		return (int)m_fMaxExtraGauge;
	else
		return m_iMaxBullet;
}

int ioPowerChargeItem5::GetCurBullet()
{
	if( m_bEnableExtraGauge )
	{
		float fCurCoolTime = m_fCurExtraGauge;
		fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

		return (int)fCurCoolTime;
	}
	else
		return m_iCurBullet;
}

void ioPowerChargeItem5::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

int ioPowerChargeItem5::GetNeedBullet()
{
	return m_fNeedExtraGauge;
}

void ioPowerChargeItem5::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !m_bEnableExtraGauge )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( m_bUsedExtraGauge )
	{
		if( m_fExtraGaugeBaseDec <= 0.0f )
			return;
	}
	else
	{
		if( m_fExtraGaugeBaseInc <= 0.0f )
			return;
	}

	if( m_bUsedExtraGauge )
	{
		float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
		m_fCurExtraGauge -= fGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
		
		if( m_fCurExtraGauge <= 0.0f )
			RemoveExtraGaugeBuff( pOwner );
	}
	else
	{
		int iState = pOwner->GetState();
		switch( iState )
		{
		case CS_DELAY:
		case CS_DASH:
		case CS_RUN:
			{
				float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
				m_fCurExtraGauge += fGauge;
				m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
			}
			break;
		}
	}
}

void ioPowerChargeItem5::RemoveExtraGaugeBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_ExtraGaugeBuffList.empty() )
		return;

	int iBuffCnt = m_ExtraGaugeBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->RemoveBuff( m_ExtraGaugeBuffList[i] );
	}

	m_bUsedExtraGauge = false;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << CS_REMOVE_BUFF;
		kPacket << m_bUsedExtraGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
//////////////////////////////////////////////////////////////////////////
ioPowerChargeSpecialState::ioPowerChargeSpecialState()
{
}

ioPowerChargeSpecialState::~ioPowerChargeSpecialState()
{
}

void ioPowerChargeSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioPowerChargeSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioPowerChargeItem5 *pPowerItem5 = ToPowerChargeItem5( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPowerItem5 )
		pPowerItem5->ClearSpecialState( pOwner );
}

void ioPowerChargeSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioPowerChargeItem5 *pPowerItem5 = ToPowerChargeItem5( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPowerItem5 )
		pPowerItem5->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioPowerChargeSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}