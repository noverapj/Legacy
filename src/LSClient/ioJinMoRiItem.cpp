

#include "stdafx.h"

#include "ioJinMoRiItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioJinMoRiItem::ioJinMoRiItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedGPState = false;
	m_fCurExtraGauge = 0.0f;
	m_dwGPStateEffect = -1;
}

ioJinMoRiItem::ioJinMoRiItem( const ioJinMoRiItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_fNeedGPGauge( rhs.m_fNeedGPGauge ),
	m_ChangeGPStateAttack( rhs.m_ChangeGPStateAttack ),
	m_ReleaseGPStateAttack( rhs.m_ReleaseGPStateAttack ),
	m_vGPStateBuffList( rhs.m_vGPStateBuffList ),
	m_vReleaseGPStateBuffList( rhs.m_vReleaseGPStateBuffList ),
	m_dwGPStateObjectItem( rhs.m_dwGPStateObjectItem ),
	m_GPStateEffect( rhs.m_GPStateEffect ),
	m_dwGatheringTime( rhs.m_dwGatheringTime ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
	m_DashExtraAttack_left( rhs.m_DashExtraAttack_left ),
	m_DashExtraAttack_right( rhs.m_DashExtraAttack_right ),
	m_DashExtraAttack_up( rhs.m_DashExtraAttack_up ),
	m_DashExtraAttack_down( rhs.m_DashExtraAttack_down ),
	m_fExtendTargetAngle(rhs.m_fExtendTargetAngle ),
	m_fExtendTargetRange( rhs.m_fExtendTargetRange ),
	m_fExtendTargetUpHeight( rhs.m_fExtendTargetUpHeight ),
	m_fExtendTargetDownHeight( rhs.m_fExtendTargetDownHeight ),
	m_fTargetAngle(rhs.m_fTargetAngle ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
	m_fTargetDownHeight( rhs.m_fTargetDownHeight )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedGPState = false;
	m_fCurExtraGauge = 0.0f;
	m_dwGPStateEffect = -1;
}

ioJinMoRiItem::~ioJinMoRiItem()
{
}

void ioJinMoRiItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//
	m_fNeedGPGauge = rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );

	//
	wsprintf_e( szBuf, "change_gp_attack" );
	LoadAttackAttribute( szBuf, m_ChangeGPStateAttack, rkLoader );

	wsprintf_e( szBuf, "release_gp_attack" );
	LoadAttackAttribute( szBuf, m_ReleaseGPStateAttack, rkLoader );

	m_vGPStateBuffList.clear();
	m_vReleaseGPStateBuffList.clear();

	int iBuffCnt = rkLoader.LoadInt_e( "gp_buff_cnt", 0 );
	for( int k=0; k < iBuffCnt; ++k )
	{
		wsprintf_e( szKey, "gp_buff%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_vGPStateBuffList.push_back( szBuff );
	}

	int iReleaseBuffCnt = rkLoader.LoadInt_e( "release_gp_buff_cnt", 0 );
	for( int l=0; l < iReleaseBuffCnt; ++l )
	{
		wsprintf_e( szKey, "release_gp_buff%d", l+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_vReleaseGPStateBuffList.push_back( szBuff );
	}

	m_dwGPStateObjectItem = (DWORD)rkLoader.LoadInt_e( "gp_object_item", 0 );

	rkLoader.LoadString_e( "enable_gp_effect", "", szBuf, MAX_PATH );
	m_GPStateEffect = szBuf;


	m_dwGatheringTime = (DWORD)rkLoader.LoadInt_e( "gathering_max_time", 0 );
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	//////////////////////////////////////////////////////////////////////////
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

	m_dwMaxComboDuration = (DWORD)rkLoader.LoadInt_e( "charge_combo_max_time", 1 );

	//left mode
	wsprintf_e( szBuf, "dash_extra_attack_left" );
	LoadAttackAttribute( szBuf, m_DashExtraAttack_left, rkLoader );

	//right mode
	wsprintf_e( szBuf, "dash_extra_attack_right" );
	LoadAttackAttribute( szBuf, m_DashExtraAttack_right, rkLoader );

	//up mode
	wsprintf_e( szBuf, "dash_extra_attack_up" );
	LoadAttackAttribute( szBuf, m_DashExtraAttack_up, rkLoader );

	//down mode
	wsprintf_e( szBuf, "dash_extra_attack_down" );
	LoadAttackAttribute( szBuf, m_DashExtraAttack_down, rkLoader );

	m_fTargetAngle			= rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange			= rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight		= rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetDownHeight		= rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	m_fExtendTargetAngle			= rkLoader.LoadFloat_e( "extend_target_angle", 0.0f );
	m_fExtendTargetRange			= rkLoader.LoadFloat_e( "extend_target_range", 0.0f );
	m_fExtendTargetUpHeight		= rkLoader.LoadFloat_e( "extend_target_up_height", 0.0f );
	m_fExtendTargetDownHeight		= rkLoader.LoadFloat_e( "extend_target_down_height", 0.0f );
}

ioItem* ioJinMoRiItem::Clone()
{
	return new ioJinMoRiItem( *this );
}

ioWeaponItem::WeaponSubType ioJinMoRiItem::GetSubType() const
{
	return WST_JINMORI_ITEM;
}

void ioJinMoRiItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseGPState( pOwner );
	ReleaseEnableGPStateEffect( pOwner );
	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioJinMoRiItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

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
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioJinMoRiItem::CheckNormalAttack( ioBaseChar *pOwner )
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
	case CS_COMBO:
		{
			pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->CheckExtendAttackEndJump() )
					return;

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				if( m_dwKeyReserveTime > 0 &&
					m_dwKeyReserveTime < dwCurTime &&
					pOwner->IsDirKeyDoubleClick() &&
					!pOwner->HasBuff(BT_DISABLE_DASH) )
				{
					if( ChangeToRetreatAttack( pOwner, RET_ATTACK_TYPE_NONE ) )
						return;
					/*if( ChangeToRetreatAttack( pOwner ) )
					{
						if( pOwner->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
							FillItemGaugePacketHeader( pOwner, kPacket );
							kPacket << ST_RETREAT_ATTACK;
							kPacket << pOwner->GetTargetRot();
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
						return;
					}*/
				}

				OnComboState( pOwner );
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
	case CS_DASH_EXTEND_ATTACK:
		if( /*pOwner->IsNeedProcess() &&*/ m_dwMotionEndTime < dwCurTime )
			ChangeEnd( pOwner );

		break;
	}
}

void ioJinMoRiItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToGatheringState( pOwner );
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}


void ioJinMoRiItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	DWORD dwCurTinme = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		if( dwCurTinme > m_dwMotionEndTime )
		{
			if( IsEnableGPState( pOwner ) )
			{
				SetUseGPState( pOwner );
				return;
			}
			else
			{
				ChangeToComboState( pOwner );
			}
		}
	}
	else
	{
		ChangeToComboState( pOwner );
	}
}


void ioJinMoRiItem::OnComboState( ioBaseChar *pOwner )
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
					kPacket << ST_SET_NEXT_COMBO;
					kPacket << m_iChargeCombo;
					kPacket << pOwner->GetTargetRot();
					pOwner->FillAutoTargetInfo( kPacket );
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				m_iChargeCombo++;
			}
			else if( pOwner->IsDirKeyDoubleClick() )
			{
				ChangeToRetreatAttack( pOwner, RET_ATTACK_TYPE_NONE );
			}
			else if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
			{
				m_iSkillInput = -1;
				return;
			}
		}
	}
}


void ioJinMoRiItem::OnRetreatState( ioBaseChar *pOwner )
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

		// 점프
		/*if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ChangeRetreatEndJump(pOwner);
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			if( ChangeDashExtendAttack(pOwner, ioJinMoRiItem::DAT_S) )
				return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			if( ChangeDashExtendAttack(pOwner, ioJinMoRiItem::DAT_D) )
				return;
		}*/

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


void ioJinMoRiItem::ChangeToComboState( ioBaseChar *pOwner )
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

	if( pOwner->IsNeedProcess() )
	{
		//타겟을 찾고
		FindDashTarget(pOwner);

		//어느 방향으로 돌진할지 정한다
		CheckDashMoveDir(pOwner);

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -m_vDashMoveDir );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_SET_NEXT_COMBO;
		kPacket << m_iChargeCombo;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 후
	m_iChargeCombo++;
}

void ioJinMoRiItem::ChangeToAttackFire( ioBaseChar *pOwner )
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
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioJinMoRiItem::ChangeToGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_GatherAnimation );
	float fTimeRate  = m_fGatherAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_GATHERING;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioJinMoRiItem::ChangeToRetreatAttack( ioBaseChar *pOwner, int nAttackType )
{
	if( !pOwner ) return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeState = CS_RETREAT;
	m_dwRetreatEndTime = dwCurTime;

	m_dwKeyReserveTime = 0;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->InitActionStopDelay( true );

	RetreatAttackType nDashAttackType = RET_ATTACK_TYPE_NONE;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir == ioUserKeyInput::DKI_LEFT )
		{
			nDashAttackType = RET_ATTACK_TYPE1;
		}
		else if( eDir == ioUserKeyInput::DKI_RIGHT )
		{
			nDashAttackType = RET_ATTACK_TYPE2;
		}
		else if( eDir == ioUserKeyInput::DKI_UP )
		{
			nDashAttackType = RET_ATTACK_TYPE3;
		}
		else if( eDir == ioUserKeyInput::DKI_DOWN )
		{
			nDashAttackType = RET_ATTACK_TYPE4;
		}

		/*else
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}*/
	}
	else
		nDashAttackType = (RetreatAttackType)nAttackType;

	pOwner->ClearDirDoubleClick();


	/*if( !pOwner ) return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;*/

	//DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	pOwner->InitActionStopDelay( true );

	pOwner->ClearReservedSliding();

	m_ChargeState = CS_DASH_EXTEND_ATTACK;

	/*if( pOwner->IsNeedProcess() )
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
	}*/

	if( pOwner->IsNeedProcess() )
	{
		//타겟을 찾고
		FindExtendAttackTarget(pOwner);

		//어느 방향으로 돌진할지 정한다
		CheckDashMoveDir(pOwner);

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -m_vDashMoveDir );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	int iAniID = 0;
	float fTimeRate = 0.f;
	pOwner->IncreaseWeaponIndexBase();

	if( nDashAttackType == ioJinMoRiItem::RET_ATTACK_TYPE1 )
	{
		iAniID = pGrp->GetAnimationIdx( m_DashExtraAttack_left.m_AttackAnimation );
		fTimeRate = m_DashExtraAttack_left.m_fAttackAniRate;
		pOwner->RefreshFireTimeList( iAniID, m_DashExtraAttack_left.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->SetReservedSliding( m_DashExtraAttack_left.m_vForceInfoList, iAniID, fTimeRate, 0 );
	}
	else if( nDashAttackType == ioJinMoRiItem::RET_ATTACK_TYPE2 )
	{
		iAniID = pGrp->GetAnimationIdx( m_DashExtraAttack_right.m_AttackAnimation );
		fTimeRate = m_DashExtraAttack_right.m_fAttackAniRate;
		pOwner->RefreshFireTimeList( iAniID, m_DashExtraAttack_right.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->SetReservedSliding( m_DashExtraAttack_right.m_vForceInfoList, iAniID, fTimeRate, 0 );
	}
	else if( nDashAttackType == ioJinMoRiItem::RET_ATTACK_TYPE3 )
	{
		iAniID = pGrp->GetAnimationIdx( m_DashExtraAttack_up.m_AttackAnimation );
		fTimeRate = m_DashExtraAttack_up.m_fAttackAniRate;
		pOwner->RefreshFireTimeList( iAniID, m_DashExtraAttack_up.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->SetReservedSliding( m_DashExtraAttack_up.m_vForceInfoList, iAniID, fTimeRate, 0 );
	}
	else if( nDashAttackType == ioJinMoRiItem::RET_ATTACK_TYPE4 )
	{
		iAniID = pGrp->GetAnimationIdx( m_DashExtraAttack_down.m_AttackAnimation );
		fTimeRate = m_DashExtraAttack_down.m_fAttackAniRate;
		pOwner->RefreshFireTimeList( iAniID, m_DashExtraAttack_down.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->SetReservedSliding( m_DashExtraAttack_down.m_vForceInfoList, iAniID, fTimeRate, 0 );
	}
	else
	{
		return false;
	}

	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwTrackingTime = FRAMEGETTIME();
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_RETREAT_ATTACK;
		kPacket << m_ChargeState;
		kPacket << (int)nDashAttackType;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}


void ioJinMoRiItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	rkPacket >> m_iCurCombo;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	ChangeToAttackFire( pOwner);
}

void ioJinMoRiItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iSubType;
	rkPacket >> iSubType;

	switch( iSubType )
	{
	case ST_SET_GP_EFFECT:
		SetEnableGPStateEffect( pOwner );
		break;
	case ST_SET_NEXT_COMBO:
		{
			rkPacket >> m_iChargeCombo;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			SetNextComboState( pOwner );
		}
		break;
	case ST_RETREAT_ATTACK:
		{
			int nState;
			int nAttackType;
			D3DXQUATERNION qtRot;
			rkPacket >> nState;
			m_ChargeState = (ChargeState)nState;
			rkPacket >> nAttackType;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToRetreatAttack( pOwner, nAttackType );
		}
		break;
	case ST_END:
		ChangeEnd( pOwner );
		break;
	}
}

void ioJinMoRiItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioJinMoRiItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioJinMoRiItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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
		case CS_COMBO:
			return true;
		}
	}

	return false;
}

void ioJinMoRiItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioJinMoRiItem::ProcessCancel( ioBaseChar *pOwner )
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

bool ioJinMoRiItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioJinMoRiItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioJinMoRiItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioJinMoRiItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioJinMoRiItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( pOwner->GetHP().m_fCurValue <= 0.0f )
	{
		m_fCurExtraGauge = 0.0f;
		return;
	}

	bool bGPState = IsGPState();
	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_YUMIRA )
		{
			if( bGPState )
				fTypeRate = m_fExtraGaugeAttackDec;
			else
				fTypeRate = m_fExtraGaugeAttack;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_YUMIRA )
		{
			if( bGPState )
				fTypeRate = m_fExtraGaugeAttackDefDec;
			else
				fTypeRate = m_fExtraGaugeAttackDef;
		}
		break;
	case EGT_WOUNDED:
		if( bGPState )
			fTypeRate = m_fExtraGaugeWoundDec;
		else
			fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		if( bGPState )
			fTypeRate = m_fExtraGaugeWoundDefDec;
		else
			fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	if( fTypeRate <= 0.0f )
		return;

	if( bGPState )
	{
		m_fCurExtraGauge -= fCurGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
	}
	else
	{
		m_fCurExtraGauge += fCurGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
	}
}

bool ioJinMoRiItem::IsGPState()
{
	return m_bUsedGPState;
}

void ioJinMoRiItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	bool bGPState = IsGPState();

	// 이펙트 체크
	if( !bGPState && m_fCurExtraGauge >= m_fNeedGPGauge &&
		pOwner->IsNeedProcess() )
	{
		SetEnableGPStateEffect( pOwner );
	}

	if( pOwner->GetHP().m_fCurValue <= 0.0f )
	{
		m_fCurExtraGauge = 0.0f;
		ReleaseEnableGPStateEffect( pOwner );
		return;
	}

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( bGPState )
	{
		if( m_fExtraGaugeBaseDec <= 0.0f )
			return;
	}
	else
	{
		if( m_fExtraGaugeBaseInc <= 0.0f )
			return;
	}

	if( pOwner->IsSystemState() )
		return;

	if( bGPState )
	{
		float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
		m_fCurExtraGauge -= fGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
	}
	else
	{
		float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
		m_fCurExtraGauge += fGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
	}
}

int ioJinMoRiItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioJinMoRiItem::GetNeedBullet()
{
	return (int)m_fNeedGPGauge;
}

int ioJinMoRiItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioJinMoRiItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

bool ioJinMoRiItem::IsEnableGPState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_bUsedGPState )
		return false;

	if( !pOwner->IsEquipEnableObject() )
		return false;

	if( m_fCurExtraGauge < m_fNeedGPGauge )
		return false;

	return true;
}

void ioJinMoRiItem::SetUseGPState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// GP 사용가능 이펙트 제거 필요. 
	ReleaseEnableGPStateEffect( pOwner );

	// change state & motion
	pOwner->SetWereWolfState( m_ChangeGPStateAttack.m_AttackAnimation, m_ChangeGPStateAttack.m_fAttackAniRate );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChangeGPStateAttack.m_AttackAnimation );
	float fTimeRate = m_ChangeGPStateAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_ChangeGPStateAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_ChangeGPStateAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add buff
	int iBuffCnt = m_vGPStateBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vGPStateBuffList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	// create jinmori objectitem
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_dwGPStateObjectItem > 0 )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pOwner->GetWorldPosition();
			kPacket << m_dwGPStateObjectItem;
			kPacket << GetName();
			kPacket << pOwner->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwGPStateObjectItem );
			if( pItem )
			{
				pItem->ReLoadProperty();
				ioObjectItem *pObject = ToObjectItem( pItem );
				if( pObject )
				{
					pObject->SetObjectCreateItem( GetName() );
					pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				}
				pOwner->EquipItem( pItem );
			}
		}
	}

	m_bUsedGPState = true;

	if( pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GP_STATE );
		kPacket << pOwner->GetCharName();
		kPacket << GP_STATE_SET;
		kPacket << m_fCurExtraGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJinMoRiItem::ApplyUseGPState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// GP 사용가능 이펙트 제거 필요. 
	ReleaseEnableGPStateEffect( pOwner );

	// change state & motion
	pOwner->SetWereWolfState( m_ChangeGPStateAttack.m_AttackAnimation, m_ChangeGPStateAttack.m_fAttackAniRate );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChangeGPStateAttack.m_AttackAnimation );
	float fTimeRate = m_ChangeGPStateAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_ChangeGPStateAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_ChangeGPStateAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add buff
	int iBuffCnt = m_vGPStateBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vGPStateBuffList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	m_bUsedGPState = true;
}

void ioJinMoRiItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{
	ReleaseGPState( pOwner, true );
}

void ioJinMoRiItem::ReleaseGPState( ioBaseChar *pOwner, bool bObjReleased )
{
	if( !pOwner ) return;

	// remove buff
	int iBuffCnt = m_vGPStateBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vGPStateBuffList[j];
		pOwner->RemoveBuff( szBuffName );
	}

	m_bUsedGPState = false;

	if( bObjReleased )
		return;

	if( pOwner && m_dwGPStateObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwGPStateObjectItem )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__ );
		}
	}
}

void ioJinMoRiItem::SetEnableGPStateEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( pChar && m_dwGPStateEffect == -1 && !m_GPStateEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_GPStateEffect );
		if( pEffect )
		{
			m_dwGPStateEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << ST_SET_GP_EFFECT;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioJinMoRiItem::ReleaseEnableGPStateEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwGPStateEffect != -1 )
	{
		pChar->EndEffect( m_dwGPStateEffect, false );
	}

	m_dwGPStateEffect = -1;
}

bool ioJinMoRiItem::CheckReleaseGPState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !pOwner->IsNeedProcess() )
		return false;

	if( !m_bUsedGPState )
		return false;

	if( GetCurBullet() > 0 && pOwner->GetHP().m_fCurValue > 0.0f )
		return false;

	if( pOwner->GetState() == CS_ATTACK )
		return false;

	m_fCurExtraGauge = 0.0f;
	ReleaseGPState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GP_STATE );
		kPacket << pOwner->GetCharName();
		kPacket << GP_STATE_RELEASE;
		kPacket << m_fCurExtraGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioJinMoRiItem::SetUseExtraGauge( float fAmt )
{
	if( !IsGPState() )
		return;

	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}


void ioJinMoRiItem::SetNextComboState( ioBaseChar *pOwner )
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

		//
		DWORD dwTrackingTime = dwCurTime + dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}


/*void ioJinMoRiItem::ChangeDashEndJump( ioBaseChar *pOwner )
{
	m_ChargeState = CS_DASH_JUMP;
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetSKillEndJumpState( m_DashAttackEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioJinMoRiItem::ChangeDashExtendAttack( ioBaseChar *pOwner, int nDashAttackType )
{
	if( !pOwner ) return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	pOwner->InitActionStopDelay( true );

	pOwner->ClearReservedSliding();

	m_ChargeState = CS_DASH_EXTEND_ATTACK;

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

	if( pOwner->IsNeedProcess() )
	{
		//타겟을 찾고
		FindExtendAttackTarget(pOwner);

		//어느 방향으로 돌진할지 정한다
		CheckDashMoveDir(pOwner);

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -m_vDashMoveDir );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	int iAniID = 0;
	float fTimeRate = 0.f;
	pOwner->IncreaseWeaponIndexBase();

	if( nDashAttackType == ioJinMoRiItem::DAT_S )
	{
		iAniID = pGrp->GetAnimationIdx( m_DashExtraAttack_s.m_AttackAnimation );
		fTimeRate = m_DashExtraAttack_s.m_fAttackAniRate;
		pOwner->RefreshFireTimeList( iAniID, m_DashExtraAttack_s.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->SetReservedSliding( m_DashExtraAttack_s.m_vForceInfoList, iAniID, fTimeRate, 0 );
	}
	else if( nDashAttackType == ioJinMoRiItem::DAT_D )
	{
		iAniID = pGrp->GetAnimationIdx( m_DashExtraAttack_d.m_AttackAnimation );
		fTimeRate = m_DashExtraAttack_d.m_fAttackAniRate;
		pOwner->RefreshFireTimeList( iAniID, m_DashExtraAttack_d.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->SetReservedSliding( m_DashExtraAttack_d.m_vForceInfoList, iAniID, fTimeRate, 0 );
	}
	else
	{
		return false;
	}

	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwTrackingTime = FRAMEGETTIME();
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << nDashAttackType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}


	return true;
}*/


void ioJinMoRiItem::FindExtendAttackTarget( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fExtendTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fExtendTargetRange * m_fExtendTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fExtendTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fExtendTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_stDashTarget = szTarget;
		}
	}
}


void ioJinMoRiItem::CheckDashMoveDir( ioBaseChar *pOwner )
{
	DWORD dwMoveDuration = 0;
	if( pOwner->IsNeedProcess() )
	{
		m_vDashMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				m_vDashMoveDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
				m_vDashMoveDir.y = 0.f;
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_stDashTarget );
			if( pTarget )			
			{
				m_vDashMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
				m_vDashMoveDir.y = 0.f;
			}
		}

		D3DXVec3Normalize( &m_vDashMoveDir, &m_vDashMoveDir );
	}
}


bool ioJinMoRiItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	if( pTarget->IsApplyDownState(false) )
		return false;

	return true;
}

void ioJinMoRiItem::FindDashTarget( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_stDashTarget = szTarget;
		}
	}
}

void ioJinMoRiItem::ChangeEnd( ioBaseChar *pOwner )
{
	m_ChargeState = CS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY );
}