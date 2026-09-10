

#include "stdafx.h"

#include "ioBangtanItem.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ItemDefine.h"

ioBangtanItem::ioBangtanItem()
{
	m_ChargeState = CS_NONE;
	m_SpecialState = SS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_bSetChargingMoveAni = false;

	m_dwChangeSpecialStateReserveTime = 0;

	m_szTeleportTarget.Clear();
	m_dwTeleportCheckTime = 0;
	m_dwTeleportOwnerMarkID = 0;
	m_dwTeleportTargetMarkID = 0;

	m_iTeleportAttackCombo = 0;
	m_dwMotionEndTime = 0;

	m_dwReserveAttack = 0;
	m_bReserveAttack = false;
}

ioBangtanItem::ioBangtanItem( const ioBangtanItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_GatheringEffect( rhs.m_GatheringEffect ),
  m_ChargingMoveAnimation( rhs.m_ChargingMoveAnimation ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_dwTeleportEnableTime( rhs.m_dwTeleportEnableTime ),
  m_fTeleportOffset( rhs.m_fTeleportOffset ),
  m_szTeleportStartEffect( rhs.m_szTeleportStartEffect ),
  m_szTeleportEndEffect( rhs.m_szTeleportEndEffect ),
  m_szTeleportOwnerMark( rhs.m_szTeleportOwnerMark ),
  m_szTeleportTargetMark( rhs.m_szTeleportTargetMark ),
  m_fTeleportRange( rhs.m_fTeleportRange ),
  m_fTeleportJumpPower( rhs.m_fTeleportJumpPower ),
  m_TeleportAttackAttribute( rhs.m_TeleportAttackAttribute ),
  m_DashAddAttackAttribute( rhs.m_DashAddAttackAttribute ),
  m_iChargeAttackIndex( rhs.m_iChargeAttackIndex ),
  m_iDashChargeAttackIndex( rhs.m_iDashChargeAttackIndex ),
  m_iDashAddAttackIndex( rhs.m_iDashAddAttackIndex ),
  m_dwTeleportDelayTime( rhs.m_dwTeleportDelayTime )
{
	m_ChargeState = CS_NONE;
	m_SpecialState = SS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_bSetChargingMoveAni = false;

	m_iTeleportAttackCombo = 0;
	m_dwMotionEndTime = 0;

	m_dwReserveAttack = 0;
	m_bReserveAttack = false;

	m_dwChangeSpecialStateReserveTime = 0;
}

ioBangtanItem::~ioBangtanItem()
{
}

void ioBangtanItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH] = "";

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

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", 1.0f );

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
    m_GatheringEffect = szBuf;

	rkLoader.LoadString_e( "attack_move_ani", "", szBuf, MAX_PATH );
	m_ChargingMoveAnimation = szBuf;

	rkLoader.LoadString_e( "teleport_start_effect", "", szBuf, MAX_PATH );
	m_szTeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "teleport_end_effect", "", szBuf, MAX_PATH );
	m_szTeleportEndEffect = szBuf;

	m_dwTeleportEnableTime = rkLoader.LoadInt_e( "teleport_enable_time", 1000 );
	m_fTeleportOffset = rkLoader.LoadFloat_e( "teleport_offset", 0.0f );

	rkLoader.LoadString_e( "teleport_owner_mark", "", szBuf, MAX_PATH );
	m_szTeleportOwnerMark = szBuf;

	rkLoader.LoadString_e( "teleport_target_mark", "", szBuf, MAX_PATH );
	m_szTeleportTargetMark = szBuf;

	m_fTeleportRange = rkLoader.LoadFloat_e( "teleport_enable_range", 500.0f );

	m_fTeleportJumpPower = rkLoader.LoadFloat_e( "teleport_end_jump_power", 500.0f );

	int iCnt = rkLoader.LoadInt_e( "teleport_attack_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szBuf, "teleport_attack%d", i+1 );
		AttackAttribute TeleportAttack;
		LoadAttackAttribute( szBuf, TeleportAttack, rkLoader );
		m_TeleportAttackAttribute.push_back(TeleportAttack);
	}

	iCnt = rkLoader.LoadInt_e( "dash_add_attack_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szBuf, "dash_add_attack%d", i+1 );
		AttackAttribute DashAddAttack;
		LoadAttackAttribute( szBuf, DashAddAttack, rkLoader );
		m_DashAddAttackAttribute.push_back(DashAddAttack);
	}

	m_iChargeAttackIndex = rkLoader.LoadInt_e( "teleport_attack_index", 0 );
	m_iDashChargeAttackIndex = rkLoader.LoadInt_e( "teleport_dash_attack_index", 0 );
	m_iDashAddAttackIndex = rkLoader.LoadInt_e( "dash_add_attack_index", 0 );
	m_dwTeleportDelayTime = rkLoader.LoadInt_e( "teleport_delay_time", 0 );
}

ioItem* ioBangtanItem::Clone()
{
	return new ioBangtanItem( *this );
}

ioWeaponItem::WeaponSubType ioBangtanItem::GetSubType() const
{
	return WST_BANGTAN_ITEM;
}

void ioBangtanItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearCancelInfo();

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
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
		pGrp->ClearAllLoopAni( 100.0f, true );
		pGrp->ClearAllActionAni( 100.0f, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, 100.0f, 100.0f, 1.0f, 1.0f/fTimeRate, 0.0f, 0.0f, true );

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

void ioBangtanItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

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
	case CS_TELEPORT_ATTACK:
		break;
	}

	CheckChargingMoveState( pOwner );
}

void ioBangtanItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
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

void ioBangtanItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
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
		if( pOwner->IsDefenseKeyDown() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				pOwner->SetAttackMoveEnable( false );
				pOwner->SetChargingState( false );
				m_bSetChargingMoveAni = false;

				if( m_GatheringEffectID != -1 )
				{
					pOwner->EndEffect( m_GatheringEffectID, false );
					m_GatheringEffectID = -1;
				}

				m_ChargeState = CS_NONE;
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)SYNC_NONE;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, true );
	}
}

void ioBangtanItem::ChangeToGathering( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( 100.0f, true );
		pGrp->ClearAllLoopAni( 100.0f, true );

		float fCurRate = 1.0f;
		if( m_fGatherAniRate > 0.0f )
			fCurRate = m_fGatherAniRate;

		pGrp->SetLoopAni( m_GatherAnimation, 100.0f, 1.0f, 1.0f/fCurRate );
	}
	
	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
	{
		m_GatheringEffectID = pEffect->GetUniqueID();
	}

	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SYNC_GATHERING;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBangtanItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

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

	if( bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo], true );

				int iAniID = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
				float fTimeRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
				DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

				pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
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

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_NORMAL_ATTACK;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SYNC_ATTACK_FIRE;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBangtanItem::ChangeToTeleportAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_ChargeState = CS_TELEPORT_ATTACK;

	m_iTeleportAttackCombo = 0;

	SetTeleport( pOwner, false, false, false );
	SetTeleportAttack( pOwner, false );
}

void ioBangtanItem::SetTeleportAttack( ioBaseChar *pOwner, bool bSendNetwork )
{
	if( !pOwner )
		return;

	m_bReserveAttack = false;

	int iCnt = m_TeleportAttackAttribute.size();
	if( !COMPARE(m_iTeleportAttackCombo, 0, iCnt) )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllLoopAni( 100.0f, true );
		pGrp->ClearAllActionAni( 100.0f, true );

		AttackAttribute TeleportAttack = m_TeleportAttackAttribute[m_iTeleportAttackCombo];

		int iAniID = pGrp->GetAnimationIdx(TeleportAttack.m_AttackAnimation);
		if( iAniID == -1 )
			return;

		pOwner->InitExtendAttackTagInfo();
		pOwner->InitActionStopDelay( true );

		pGrp->SetActionAni( iAniID, 100.0f, 100.0f, 1.0f, 1.0f/TeleportAttack.m_fAttackAniRate, 0.0f, TeleportAttack.m_dwPreDelay );	
		pOwner->RefreshFireTimeList( iAniID, TeleportAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, TeleportAttack.m_fAttackAniRate, TeleportAttack.m_dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
		
		DWORD dwTrackingTime = FRAMEGETTIME() + TeleportAttack.m_dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * TeleportAttack.m_fAttackAniRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_dwMotionEndTime = m_dwReserveAttack = FRAMEGETTIME();
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * TeleportAttack.m_fAttackAniRate + TeleportAttack.m_dwPreDelay;
		m_dwMotionEndTime += pOwner->GetActionStopTotalDelay();
		m_dwReserveAttack += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * TeleportAttack.m_fAttackAniRate + TeleportAttack.m_dwPreDelay;

		m_iTeleportAttackCombo++;
	}

	if( pOwner->IsNeedSendNetwork() && bSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SYNC_TELEPORT_ATTACK;
		kPacket << m_iCurCombo;
		kPacket << m_iTeleportAttackCombo - 1;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBangtanItem::ChangeToDashAddAttack( ioBaseChar *pOwner )
{
	m_ChargeState = CS_DASH_EXTEND_ATTACK;
	m_iDashAddAttackCombo = 0;
}

void ioBangtanItem::SetDashAddAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_bReserveAttack = false;

	int iCnt = m_DashAddAttackAttribute.size();
	if( !COMPARE(m_iDashAddAttackCombo, 0, iCnt) )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllLoopAni( 100.0f, true );
		pGrp->ClearAllActionAni( 100.0f, true );

		AttackAttribute DashAddAttack = m_DashAddAttackAttribute[m_iDashAddAttackCombo];

		int iAniID = pGrp->GetAnimationIdx(DashAddAttack.m_AttackAnimation);
		if( iAniID == -1 )
			return;

		pOwner->InitExtendAttackTagInfo();
		pOwner->InitActionStopDelay( true );

		pGrp->SetActionAni( iAniID, 100.0f, 100.0f, 1.0f, 1.0f/DashAddAttack.m_fAttackAniRate, 0.0f, DashAddAttack.m_dwPreDelay );	
		pOwner->RefreshFireTimeList( iAniID, DashAddAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, DashAddAttack.m_fAttackAniRate, DashAddAttack.m_dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();

		DWORD dwTrackingTime = FRAMEGETTIME() + DashAddAttack.m_dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * DashAddAttack.m_fAttackAniRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_dwMotionEndTime = m_dwReserveAttack = FRAMEGETTIME();
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * DashAddAttack.m_fAttackAniRate + DashAddAttack.m_dwPreDelay;
		m_dwMotionEndTime += pOwner->GetActionStopTotalDelay();
		m_dwReserveAttack += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * DashAddAttack.m_fAttackAniRate + DashAddAttack.m_dwPreDelay;

		m_iDashAddAttackCombo++;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SYNC_DASH_EXTEND_ATTACK;
		kPacket << m_iCurCombo;
		kPacket << m_iDashAddAttackCombo - 1;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBangtanItem::ProcessDashAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_iDashAddAttackCombo == 0 && pOwner->IsJumpKey() )
	{
		pOwner->SetJumpState();
		return;
	}

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( m_bReserveAttack )
		{
			SetDashAddAttack( pOwner );
		}
		else
		{
			pOwner->SetState( CS_DELAY );
		}
	}
	else if( COMPARE(FRAMEGETTIME(), m_dwReserveAttack, m_dwMotionEndTime) )
	{
		if( m_bReserveAttack )
		{
			SetDashAddAttack( pOwner );
		}
		else if( pOwner->IsAttackKey() )
		{
			m_bReserveAttack = true;
		}
	}
}

void ioBangtanItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	bool bCharged;

	switch( iState )
	{
	case SYNC_NONE:
		{
			pOwner->SetAttackMoveEnable( false );
			pOwner->SetChargingState( false );
			m_bSetChargingMoveAni = false;

			if( m_GatheringEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;
			}

			m_ChargeState = CS_NONE;
		}
		break;
	case SYNC_CHANGE_NEXT_STATE:
		{
			int iAttributeIndex = 0;
			rkPacket >> iAttributeIndex;

			ioHashString szWoundCharName;
			rkPacket >> szWoundCharName;
			ioBaseChar *pWoundChar = pOwner->GetBaseChar(szWoundCharName);
			if ( !pWoundChar )
				return;

			if( iAttributeIndex == m_iDashChargeAttackIndex )
			{
				ClearTeleportInfo( pOwner );

				m_dwChangeSpecialStateReserveTime = FRAMEGETTIME() + m_dwTeleportDelayTime;

				m_szTeleportTarget = pWoundChar->GetCharName();

				m_SpecialState = SS_TELEPORT_ATTACK;
			}
			else if( iAttributeIndex == m_iChargeAttackIndex )
			{
				ClearTeleportInfo( pOwner );

				ioEffect *pEffect = NULL;
				pEffect = pOwner->AttachEffect( m_szTeleportOwnerMark );
				if( pEffect )
					m_dwTeleportOwnerMarkID = pEffect->GetUniqueID();

				pEffect = pWoundChar->AttachEffect( m_szTeleportTargetMark );
				if( pEffect )
					m_dwTeleportTargetMarkID = pEffect->GetUniqueID();

				m_szTeleportTarget = pWoundChar->GetCharName();
				m_dwTeleportCheckTime = FRAMEGETTIME();
			}
			else if( iAttributeIndex == m_iDashAddAttackIndex )
			{
				m_dwMotionEndTime = pOwner->GetAnimationEndTime();
				m_SpecialState = SS_DASH_ATTACK;
				pOwner->SetState( CS_BANGTAN_STATE );
			}
		}
		break;
	case SYNC_GATHERING:
		{
			ChangeToGathering( pOwner );
		}
		break;
	case SYNC_ATTACK_FIRE:
		{
			rkPacket >> bCharged;
			ChangeToAttackFire( pOwner, bCharged );
		}
		break;
	case SYNC_TELEPORT:
		{
			bool bBack = false;
			bool bAfterJump = false;
			rkPacket >> m_szTeleportTarget;
			rkPacket >> bBack;
			rkPacket >> bAfterJump;
			SetTeleport( pOwner, bBack, bAfterJump, false );
		}
		break;
	case SYNC_TELEPORT_ATTACK:
		{
			rkPacket >> m_iTeleportAttackCombo;
			SetTeleportAttack( pOwner, false );
		}
		break;
	case SYNC_DASH_EXTEND_ATTACK:
		{
			rkPacket >> m_iDashAddAttackCombo;
			SetDashAddAttack( pOwner );
		}
		break;
	case SYNC_CHANGE_STATE:
		{
			int iState = 0;
			rkPacket >> iState;
			m_SpecialState = (SpecialState)iState;
			if( m_SpecialState == SS_TELEPORT_ATTACK )
			{
				rkPacket >> m_szTeleportTarget;
			}

			pOwner->SetState( CS_BANGTAN_STATE );
		}
		break;
	}
}

void ioBangtanItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

bool ioBangtanItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner && pOwner->GetState() == CS_ATTACK )
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

bool ioBangtanItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner && pOwner->GetState() == CS_ATTACK )
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

void ioBangtanItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

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

			pGrp->ClearAllActionAni( 100.0f, true );
			pGrp->ClearAllLoopAni( 100.0f, true );

			float fCurRate = 1.0f;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, 100.0f, 1.0f, 1.0f/fCurRate );
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

			pGrp->ClearAllActionAni( 100.0f, true );
			pGrp->ClearAllLoopAni( 100.0f, true );

			float fCurRate = 1.0f;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, 100.0f, 1.0f, 1.0f/fCurRate );
			m_bSetChargingMoveAni = false;
		}
	}
}


void ioBangtanItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	if( !pOwner )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}
}

bool ioBangtanItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	return true;
}

bool ioBangtanItem::ProcessCancel( ioBaseChar *pOwner )
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

void ioBangtanItem::ProcessTeleportAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( m_bReserveAttack )
		{
			SetTeleportAttack( pOwner, true );
		}
		else
		{
			pOwner->SetSKillEndJumpState( m_fTeleportJumpPower );
		}
	}
	else if( COMPARE(FRAMEGETTIME(), m_dwReserveAttack, m_dwMotionEndTime) )
	{
		if( m_bReserveAttack )
		{
			SetTeleportAttack( pOwner, true );
		}
		else if( pOwner->IsAttackKey() )
		{
			m_bReserveAttack = true;
		}
	}
}

void ioBangtanItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
{
	if( IsChargeAutoTarget(pOwner) )
	{
		float fCurTargetRangeRate, fCurTargetAngleRate;
		fCurTargetRangeRate = fCurTargetAngleRate = 1.0f;

		float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - 1.0f) );
		float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - 1.0f) );

		fCurTargetRangeRate += fCurMaxTargetRange;
		fCurTargetAngleRate += fCurMaxTargetAngle;

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

void ioBangtanItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwChangeSpecialStateReserveTime > 0 && m_dwChangeSpecialStateReserveTime < FRAMEGETTIME() )
	{
		ioHashString szTemp = m_szTeleportTarget;

		pOwner->SetState( CS_BANGTAN_STATE );
		m_dwChangeSpecialStateReserveTime = 0;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)SYNC_CHANGE_STATE;
			kPacket << m_iCurCombo;
			kPacket << (int)m_SpecialState;
			kPacket << szTemp;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( m_dwTeleportCheckTime > 0 && m_dwTeleportCheckTime + m_dwTeleportEnableTime < FRAMEGETTIME() )
	{
		ClearTeleportInfo( pOwner );
	}
}

void ioBangtanItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( !pWoundChar->IsNeedProcess() )
		return;

	if( fDamage <= 0.0f )
		return;

	if( !pWoundChar )
		return;

	if( !pAttacker )
		return;
	
	if( !pWeapon )
		return;

	if( pWeapon->GetAttributeIndex() == m_iDashChargeAttackIndex )
	{
		ClearTeleportInfo( pAttacker );

		m_dwChangeSpecialStateReserveTime = FRAMEGETTIME() + m_dwTeleportDelayTime;

		m_szTeleportTarget = pWoundChar->GetCharName();

		m_SpecialState = SS_TELEPORT_ATTACK;
	}
	else if( pWeapon->GetAttributeIndex() == m_iChargeAttackIndex )
	{
		ClearTeleportInfo( pAttacker );

		ioEffect *pEffect = NULL;
		pEffect = pAttacker->AttachEffect( m_szTeleportOwnerMark );
		if( pEffect )
			m_dwTeleportOwnerMarkID = pEffect->GetUniqueID();

		pEffect = pWoundChar->AttachEffect( m_szTeleportTargetMark );
		if( pEffect )
			m_dwTeleportTargetMarkID = pEffect->GetUniqueID();

		m_szTeleportTarget = pWoundChar->GetCharName();
	}
	else if( pWeapon->GetAttributeIndex() == m_iDashAddAttackIndex )
	{
		m_dwMotionEndTime = pAttacker->GetAnimationEndTime();
		m_SpecialState = SS_DASH_ATTACK;
		pAttacker->SetState( CS_BANGTAN_STATE );
	}

	if( pWoundChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pAttacker, kPacket );
		kPacket << (int)SYNC_CHANGE_NEXT_STATE;
		kPacket << m_iCurCombo;
		kPacket << pWeapon->GetAttributeIndex();
		kPacket << pWoundChar->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioBangtanItem::IsEnableTarget( ioBaseChar *pOwner )
{
	if( m_szTeleportTarget.IsEmpty() )
	{
		return false;
	}

	if( m_dwTeleportCheckTime > 0 && m_dwTeleportCheckTime + m_dwTeleportEnableTime < FRAMEGETTIME() )
	{
		return false;
	}

	if( pOwner )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar(m_szTeleportTarget);
		if( pTarget )
		{
			D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			D3DXVECTOR3 vDiff = vTargetPos - vOwnerPos;
			float fLength = D3DXVec3Length(&vDiff);
			if( fLength <= m_fTeleportRange )
				return true;
		}
	}

	return false;
}

void ioBangtanItem::SetTeleport( ioBaseChar *pOwner, bool bBack, bool bAfterJump, bool bSendNetwork )
{
	if( !pOwner )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar(m_szTeleportTarget);
	if( !pTarget )
		return;

	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	D3DXVECTOR3 vOffset = pTarget->GetMoveDir() * m_fTeleportOffset;
	D3DXVECTOR3 vTeleportPos = vTargetPos;
	if( bBack )
	{
		vTeleportPos -= vOffset;
	}
	else
	{
		vTeleportPos += vOffset;
	}

	D3DXVECTOR3 vPrePos = pOwner->GetWorldPosition();

	pOwner->SetWorldPosition( vTeleportPos );
	pOwner->SetTargetRotToTargetPos( vTargetPos, true );
	pOwner->AttachEffect( m_szTeleportStartEffect );
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		pStage->CreateMapEffect( m_szTeleportStartEffect, vPrePos, ioMath::UNIT_ALL );
		pStage->CreateMapEffect( m_szTeleportEndEffect, vTeleportPos, ioMath::UNIT_ALL );
	}

	ioHashString szTemp = m_szTeleportTarget;
	ClearTeleportInfo( pOwner );

	if( bAfterJump )
	{
		pOwner->SetSKillEndJumpState( m_fTeleportJumpPower );
	}
	else
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
	}

	if( pOwner->IsNeedSendNetwork() && bSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SYNC_TELEPORT;
		kPacket << m_iCurCombo;
		kPacket << szTemp;
		kPacket << bBack;
		kPacket << bAfterJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBangtanItem::ClearTeleportInfo( ioBaseChar *pOwner )
{
	if( pOwner )
	{
		if( m_dwTeleportOwnerMarkID != 0 )
		{
			pOwner->EndEffect( m_dwTeleportOwnerMarkID, false );
			m_dwTeleportOwnerMarkID = 0;
		}

		if( m_dwTeleportTargetMarkID != 0 )
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar(m_szTeleportTarget);
			if( pTarget )
			{
				pTarget->EndEffect( m_dwTeleportTargetMarkID, false );
			}
			m_dwTeleportTargetMarkID = 0;
		}
	}

	m_dwTeleportCheckTime = 0;
	m_szTeleportTarget.Clear();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioBangtanSpecialState::ioBangtanSpecialState()
{
}

ioBangtanSpecialState::~ioBangtanSpecialState()
{
}

void ioBangtanSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	if( !pOwner )
		return;

	ioBangtanItem *pBangtanItem = ToBangtanItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pBangtanItem )
	{
		if( pBangtanItem->GetSpecialState() == ioBangtanItem::SS_TELEPORT_ATTACK )
			pBangtanItem->ChangeToTeleportAttackFire( pOwner );
		else if( pBangtanItem->GetSpecialState() == ioBangtanItem::SS_DASH_ATTACK )
			pBangtanItem->ChangeToDashAddAttack( pOwner );
	}
}

void ioBangtanSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioBangtanSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioBangtanItem *pBangtanItem = ToBangtanItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pBangtanItem )
	{
		if( pBangtanItem->GetSpecialState() == ioBangtanItem::SS_TELEPORT_ATTACK )
			pBangtanItem->ProcessTeleportAttack( pOwner );
		else if( pBangtanItem->GetSpecialState() == ioBangtanItem::SS_DASH_ATTACK )
			pBangtanItem->ProcessDashAttack( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
	}
}

void ioBangtanSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioBangtanSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	return true;
}