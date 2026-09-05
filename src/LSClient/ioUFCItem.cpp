

#include "stdafx.h"

#include "ioUFCItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioUFCItem::ioUFCItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bEquipedOnHand = false;
	m_dwMotionEndTime = 0;
	m_dwPoseEffectIndex = 0;

	m_dwMoveForceTime = 0;
	m_vMoveForceDir = ioMath::VEC3_ZERO;
}

ioUFCItem::ioUFCItem( const ioUFCItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_NormalSAttribute( rhs.m_NormalSAttribute ),
  m_NormalAAttribute( rhs.m_NormalAAttribute ),
  m_ChargeSAttribute( rhs.m_ChargeSAttribute ),
  m_ChargeAAttribute( rhs.m_ChargeAAttribute ),
  m_RetreatDAttribute( rhs.m_RetreatDAttribute ),
  m_SkillASDAttribute( rhs.m_SkillASDAttribute ),
  m_SkillADAttribute( rhs.m_SkillADAttribute ),
  m_SkillASAttribute( rhs.m_SkillASAttribute ),
  m_SkillASDExtraAttribute( rhs.m_SkillASDExtraAttribute ),
  m_SkillADExtraAttribute( rhs.m_SkillADExtraAttribute ),
  m_SkillASExtraAttribute( rhs.m_SkillASExtraAttribute ),
  m_szRetreatMoveFr( rhs.m_szRetreatMoveFr ),
  m_szRetreatMoveBk( rhs.m_szRetreatMoveBk ),
  m_szRetreatMoveRt( rhs.m_szRetreatMoveRt ),
  m_szRetreatMoveLt( rhs.m_szRetreatMoveLt ),
  m_fRetreatMoveRateFr( rhs.m_fRetreatMoveRateFr ),
  m_fRetreatMoveRateBk( rhs.m_fRetreatMoveRateBk ),
  m_fRetreatMoveRateRt( rhs.m_fRetreatMoveRateRt ),
  m_fRetreatMoveRateLt( rhs.m_fRetreatMoveRateLt ),
  m_fRetreatMoveForce( rhs.m_fRetreatMoveForce ),
  m_fRetreatMoveFriction( rhs.m_fRetreatMoveFriction ),
  m_szPoseSetAni( rhs.m_szPoseSetAni ),
  m_szPoseReleaseAni( rhs.m_szPoseReleaseAni ),
  m_fPoseSetAniRate( rhs.m_fPoseSetAniRate ),
  m_fPoseReleaseAniRate( rhs.m_fPoseReleaseAniRate ),
  m_fMaxGauge( rhs.m_fMaxGauge ),
  m_fNeedGauge( rhs.m_fNeedGauge ),
  m_vGaugeMaxBuffList( rhs.m_vGaugeMaxBuffList ),
  m_szPoseEffect( rhs.m_szPoseEffect )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_fCurGauge = 0.0f;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bEquipedOnHand = false;
	m_dwMotionEndTime = 0;
	m_dwPoseEffectIndex = 0;

	m_dwMoveForceTime = 0;
	m_vMoveForceDir = ioMath::VEC3_ZERO;
}

ioUFCItem::~ioUFCItem()
{
}

void ioUFCItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH] = "";

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

	LoadAttackAttribute_e( "normal_s_attack", m_NormalSAttribute, rkLoader );
	LoadAttackAttribute_e( "normal_a_attack", m_NormalAAttribute, rkLoader );
	LoadAttackAttribute_e( "charge_s_attack", m_ChargeSAttribute, rkLoader );
	LoadAttackAttribute_e( "charge_a_attack", m_ChargeAAttribute, rkLoader );
	LoadAttackAttribute_e( "retreat_d_attack", m_RetreatDAttribute, rkLoader );
	LoadAttackAttribute_e( "skill_asd_attack", m_SkillASDAttribute, rkLoader );
	LoadAttackAttribute_e( "skill_ad_attack", m_SkillADAttribute, rkLoader );
	LoadAttackAttribute_e( "skill_as_attack", m_SkillASAttribute, rkLoader );
	LoadAttackAttribute_e( "skill_asd_extra_attack", m_SkillASDExtraAttribute, rkLoader );
	LoadAttackAttribute_e( "skill_ad_extra_attack", m_SkillADExtraAttribute, rkLoader );
	LoadAttackAttribute_e( "skill_as_extra_attack", m_SkillASExtraAttribute, rkLoader );

	rkLoader.LoadString_e( "retreat_move_ani_fr", "", szBuf, MAX_PATH );
	m_szRetreatMoveFr = szBuf;
	rkLoader.LoadString_e( "retreat_move_ani_bk", "", szBuf, MAX_PATH );
	m_szRetreatMoveBk = szBuf;
	rkLoader.LoadString_e( "retreat_move_ani_rt", "", szBuf, MAX_PATH );
	m_szRetreatMoveRt = szBuf;
	rkLoader.LoadString_e( "retreat_move_ani_lt", "", szBuf, MAX_PATH );
	m_szRetreatMoveLt = szBuf;

	m_fRetreatMoveRateFr = rkLoader.LoadFloat_e( "retreat_move_ani_rate_fr", 0 );
	m_fRetreatMoveRateBk = rkLoader.LoadFloat_e( "retreat_move_ani_rate_bk", 0 );
	m_fRetreatMoveRateRt = rkLoader.LoadFloat_e( "retreat_move_ani_rate_rt", 0 );
	m_fRetreatMoveRateLt = rkLoader.LoadFloat_e( "retreat_move_ani_rate_lt", 0 );

	m_fRetreatMoveForce = rkLoader.LoadFloat_e( "retreat_move_force", 0 );
	m_fRetreatMoveFriction = rkLoader.LoadFloat_e( "retreat_move_friction", 0 );

	rkLoader.LoadString_e( "pose_set_ani", "", szBuf, MAX_PATH );
	m_szPoseSetAni = szBuf;
	rkLoader.LoadString_e( "pose_release_ani", "", szBuf, MAX_PATH );
	m_szPoseReleaseAni = szBuf;

	m_fPoseSetAniRate = rkLoader.LoadFloat_e( "pose_set_ani_rate", 0 );
	m_fPoseReleaseAniRate = rkLoader.LoadFloat_e( "pose_release_ani_rate", 0 );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	int iBuffCnt = rkLoader.LoadInt_e( "gauge_max_buff_cnt", 0 );
	for( i = 0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "gauge_max_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vGaugeMaxBuffList.push_back( szBuf );
	}

	rkLoader.LoadString_e( "pose_effect", "", szBuf, MAX_PATH );
	m_szPoseEffect = szBuf;
}

ioItem* ioUFCItem::Clone()
{
	return new ioUFCItem( *this );
}

ioWeaponItem::WeaponSubType ioUFCItem::GetSubType() const
{
	return WST_UFC_ITEM;
}

void ioUFCItem::OnReleased( ioBaseChar *pOwner )
{
	m_fCurGauge = 0.0f;
	m_bEquipedOnHand = false;
	DestroyPoseEffect( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

void ioUFCItem::SetNoneState( ioBaseChar* pOwner, int eNewState /*= 0 */ )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_dwMotionEndTime = 0;
	m_dwKeyReserveTime = 0;
}

void ioUFCItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;

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

void ioUFCItem::CheckNormalAttack( ioBaseChar *pOwner )
{
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

				DWORD dwCurTime = FRAMEGETTIME();
				if( pOwner->GetCharKeyReserveEnableTime() < dwCurTime )
				{
					if( pOwner->IsDefenseKey() )
					{
						if( m_bCharged )
							ChangeToSpecialAttackFire( pOwner, SS_CHARGE_S );
						else
							ChangeToSpecialAttackFire( pOwner, SS_NORMAL_S );
					}
					else if( pOwner->IsJumpKey() )
					{
						if( m_bCharged )
							ChangeToSpecialAttackFire( pOwner, SS_CHARGE_A );
						else
							ChangeToSpecialAttackFire( pOwner, SS_NORMAL_A );
					}
				}
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_RETREAT_MOVE:
		ProcessRetreatMoveState( pOwner );
		break;
	case CS_SPECIAL_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;
			}
			pOwner->CheckExtraAniJump();
			pOwner->CheckAniRotate();
		
			DWORD dwCurTime = FRAMEGETTIME();
			if ( m_dwMotionEndTime < dwCurTime )
			{
				ProcessReserveKeyInput( pOwner );
				return;
			}
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
				CheckReserve( pOwner );
		}
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
}

void ioUFCItem::ChangeToSpecialAttackFire( ioBaseChar *pOwner, int iSpecialState )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_ChargeState = CS_SPECIAL_ATTACK_FIRE;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	AttackAttribute cInfo;
	bool bGaugeUse = false;
	switch( iSpecialState )
	{
	case SS_NORMAL_S:	cInfo = m_NormalSAttribute;		break;
	case SS_NORMAL_A:	cInfo = m_NormalAAttribute;		break;
	case SS_CHARGE_S:	cInfo = m_ChargeSAttribute;		break;
	case SS_CHARGE_A:	cInfo = m_ChargeAAttribute;		break;
	case SS_RETREAT_D:	cInfo = m_RetreatDAttribute;	break;
	case SS_SKILL_ASD:
		if( IsEnableGauge() )
		{
			cInfo = m_SkillASDExtraAttribute;
			bGaugeUse = true;
		}
		else
			cInfo = m_SkillASDAttribute;
		break;
	case SS_SKILL_AD:
		if( IsEnableGauge() )
		{
			cInfo = m_SkillADExtraAttribute;
			bGaugeUse = true;
		}
		else
			cInfo = m_SkillADAttribute;
		break;
	case SS_SKILL_AS:
		if( IsEnableGauge() )
		{
			cInfo = m_SkillASExtraAttribute;
			bGaugeUse = true;
		}
		else
			cInfo = m_SkillASAttribute;
		break;
	case SS_SKILL_ASD_EXTRA:
		cInfo = m_SkillASDExtraAttribute;
		bGaugeUse = true;
		break;
	case SS_SKILL_AD_EXTRA:
		cInfo = m_SkillADExtraAttribute;
		bGaugeUse = true;
		break;
	case SS_SKILL_AS_EXTRA:
		cInfo = m_SkillASExtraAttribute;
		bGaugeUse = true;
		break;
	}
	if( bGaugeUse )
	{
		if( pOwner->IsNeedSendNetwork() )
			iSpecialState += SS_EXTRA;

		CheckDecreaseGauge();
		ReleaseGaugeMaxBuff( pOwner );
	}
	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( cInfo, true );

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	pOwner->SetBaseAttackExtraAniJump( iAniID, cInfo.m_fAttackAniRate, cInfo.m_dwPreDelay );
	pOwner->SetAniRotate( iAniID, cInfo.m_fAttackAniRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_szCurAni = cInfo.m_AttackAnimation;
	m_fCurAniRate = cInfo.m_fAttackAniRate;
	m_dwInputCancelCheckTime = dwCurTime;
	m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * cInfo.m_fAttackAniRate );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * cInfo.m_fAttackAniRate;
	m_dwKeyReserveTime += dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << iSpecialState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUFCItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

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

void ioUFCItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

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
		}
		if( iExtendCnt <= m_iCurCombo + 1 )
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		else
			pOwner->ApplyNormalAttackResult( m_iCurCombo );
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
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );
	pOwner->CheckKeyReserveTime( iAniID, m_fCurAniRate );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUFCItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	switch( iState )
	{
	case CS_ATTACK_FIRE:
		{
			bool bCharged;
			rkPacket >> m_iCurCombo;
			rkPacket >> bCharged;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner, bCharged );
		}
		break;
	case CS_RETREAT_MOVE:
		{
			D3DXVECTOR3 vNewPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vNewPos;
			rkPacket >> qtRot;

			ioPlayStage *pStage = pOwner->GetCreator();
			if ( !pStage )
				return;

			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			int eCurrDir;
			rkPacket >> eCurrDir;
			ChangeToRetreatMoveState( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ) );
		}
		break;
	case CS_SPECIAL_ATTACK_FIRE:
		{
			int iSpecialState;
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> iSpecialState;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToSpecialAttackFire( pOwner, iSpecialState );
		}
		break;
	case CS_POSE_CHANGE:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> m_bEquipedOnHand;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );
			SetPoseChangeState( pOwner );

			m_bEquipedOnHand = !m_bEquipedOnHand;
		}
		break;
	}
}

bool ioUFCItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioUFCItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
		case CS_SPECIAL_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}

	return false;
}

void ioUFCItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}

bool ioUFCItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioUFCItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioUFCItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioUFCItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

//Dash
void ioUFCItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash* pChargeDash = ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );
		m_pExtendDash->ProcessDash( pOwner );
	}
}

bool ioUFCItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_RETREAT_MOVE:
		return false;
	}

	return true;
}

bool ioUFCItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

void ioUFCItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	if ( pOwner->IsNeedProcess() )
		pOwner->SetCurNormalAttackItem( this );

	ChangeToRetreatMoveState( pOwner, eCurrDir );
}

void ioUFCItem::ChangeToRetreatMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir )
{
	if ( !pOwner )
		return;

	if ( eDir == ioUserKeyInput::DKI_NONE )
		return;

	if( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();

	// KeyDir
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
	case 7:
		if( !m_szRetreatMoveFr.IsEmpty() )
			CheckFireAndMoveAni( pOwner, m_szRetreatMoveFr, m_fRetreatMoveRateFr, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_szRetreatMoveRt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveRt, m_fRetreatMoveRateRt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_szRetreatMoveFr.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveFr, m_fRetreatMoveRateFr, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_szRetreatMoveRt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveRt, m_fRetreatMoveRateRt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_szRetreatMoveBk.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveBk, m_fRetreatMoveRateBk, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_szRetreatMoveBk.IsEmpty() )
			CheckFireAndMoveAni( pOwner, m_szRetreatMoveBk, m_fRetreatMoveRateBk, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_szRetreatMoveLt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveLt, m_fRetreatMoveRateLt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_szRetreatMoveBk.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveBk, m_fRetreatMoveRateBk, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_szRetreatMoveLt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveLt, m_fRetreatMoveRateLt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_szRetreatMoveFr.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveFr, m_fRetreatMoveRateFr, vMoveDir );
		}
		break;
	}

	m_ChargeState = CS_RETREAT_MOVE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << (int)eDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUFCItem::CheckFireAndMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwFullAniTime = pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_dwMotionEndTime = dwCurTime + dwFullAniTime;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwMoveForceTime = dwCurTime + dwTime;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwKeyReserveTime += dwCurTime;

	m_vMoveForceDir = vDir;
}

void ioUFCItem::ProcessRetreatMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if( pOwner->IsAttackKey() )
		{
			ChangeToSpecialAttackFire( pOwner, SS_RETREAT_D );
			return;
		}
		CheckReserve( pOwner );
	}
	if( m_dwMoveForceTime > 0 && m_dwMoveForceTime < dwCurTime )
	{
		m_dwMoveForceTime = 0;
		pOwner->SetForcePower( m_vMoveForceDir, m_fRetreatMoveForce, m_fRetreatMoveFriction );
	}
}

void ioUFCItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( bFullTime )
	{
		SetPoseChangePreState( pOwner );
		m_bEquipedOnHand = !m_bEquipedOnHand;
		return;
	}
	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

void ioUFCItem::SetPoseChangePreState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	if ( pOwner->IsNeedProcess() )
		pOwner->SetCurNormalAttackItem( this );
	if( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	m_ChargeState = CS_POSE_CHANGE_PRE;	
}

void ioUFCItem::SetPoseChangeState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	m_ChargeState = CS_POSE_CHANGE;	

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	ioHashString szAni;
	float fAniRate;
	if( m_bEquipedOnHand )
	{
		szAni = m_szPoseSetAni;
		fAniRate = m_fPoseSetAniRate;

		CreatePoseEffect( pOwner );
	}
	else
	{
		szAni = m_szPoseReleaseAni;
		fAniRate = m_fPoseReleaseAniRate;

		DestroyPoseEffect( pOwner );
	}

	int iAniID	     = pGrp->GetAnimationIdx( szAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_bEquipedOnHand;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioUFCItem::CheckOnHand()
{
	return m_bEquipedOnHand;
}

void ioUFCItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioUFCItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioUFCItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioUFCItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioUFCItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioUFCItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioUFCItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( IsEnableGauge() )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_UFC )
			fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_UFC )
			fTypeRate = m_fExtraGaugeAttackDef;
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	m_fCurGauge += fCurGauge;
	if( m_fCurGauge >= m_fNeedGauge )
	{
		m_fCurGauge = m_fNeedGauge;
		SetGaugeMaxBuff( pOwner );
	}
}

void ioUFCItem::SetGaugeMaxBuff( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	int iBuffCnt = m_vGaugeMaxBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->AddNewBuffWithAnimateTime( m_vGaugeMaxBuffList[i], pChar->GetCharName(), m_Name, NULL, 0 );
	}
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUFCItem::ReleaseGaugeMaxBuff( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	int iBuffCnt = m_vGaugeMaxBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->RemoveBuff( m_vGaugeMaxBuffList[i] );
	}
}

void ioUFCItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	SetGaugeMaxBuff( pOwner );
}

void ioUFCItem::CreatePoseEffect( ioBaseChar *pChar )
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

void ioUFCItem::DestroyPoseEffect( ioBaseChar *pChar )
{
	if( m_dwPoseEffectIndex != 0 )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwPoseEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();

		m_dwPoseEffectIndex = 0;
	}
}