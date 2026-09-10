#include "StdAfx.h"
#include "ioMidnightScytheObjectItem.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMidnightScytheObjectItem::ioMidnightScytheObjectItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_dwPreAttackEndTime = 0;
	m_iCurCombo = 0;
	m_fCurCmdGauge = 0.0f;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bIsGaugeUseUp = false;
	m_bObjectRelease = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_ScytheState = SS_NONE;

	ClearState();
	ClearCmdInfo();
}

ioMidnightScytheObjectItem::ioMidnightScytheObjectItem( const ioMidnightScytheObjectItem &rhs )
	: ioObjectItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_PreAttackGround( rhs.m_PreAttackGround ),
	m_PreAttackJump( rhs.m_PreAttackJump ),
	m_ChargeCmdInfoList( rhs.m_ChargeCmdInfoList ),
	m_vCmdGauge( rhs.m_vCmdGauge ),
	m_dwInputGapTime( rhs.m_dwInputGapTime ),
	m_dwInputMaxTime( rhs.m_dwInputMaxTime ),
	m_dwInputStartGapTime( rhs.m_dwInputStartGapTime ),
	m_fPreAttackJumpGravity( rhs.m_fPreAttackJumpGravity )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_dwPreAttackEndTime = 0;
	m_iCurCombo = 0;
	m_fCurCmdGauge = 0.0f;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bIsGaugeUseUp = false;
	m_bObjectRelease = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_ScytheState = SS_NONE;

	ClearState();
	ClearCmdInfo();
}

ioMidnightScytheObjectItem::~ioMidnightScytheObjectItem()
{
}

void ioMidnightScytheObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	LoadAttackAttribute_e( "pre_attack_ground", m_PreAttackGround, rkLoader );
	LoadAttackAttribute_e( "pre_attack_jump", m_PreAttackJump, rkLoader );
	m_fPreAttackJumpGravity = rkLoader.LoadFloat_e( "pre_attack_jump_gravity_amt", 0.0f );

	LoadCmdInfoList( rkLoader );

	m_dwInputMaxTime = (DWORD)rkLoader.LoadInt_e( "input_max_time", 0 );
	m_dwInputGapTime = (DWORD)rkLoader.LoadInt_e( "input_gap_time", 0 );
	m_dwInputStartGapTime = (DWORD)rkLoader.LoadInt_e( "input_start_gap_time", 0 );
}

ioItem* ioMidnightScytheObjectItem::Clone()
{
	return new ioMidnightScytheObjectItem( *this );
}

ioObjectItem::ObjectSubType ioMidnightScytheObjectItem::GetObjectSubType() const
{
	return OST_MIDNIGHT_SCYTHE;
}

void ioMidnightScytheObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	ClearState();
	ClearCmdInfo();

	m_iChargeCombo = 0;

	m_bCharged = false;
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

void ioMidnightScytheObjectItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;

	case CS_FULL_GATHERING:
		OnFullGathering( pOwner );
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
				}

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				if( CheckCmdInputState( pOwner ) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioMidnightScytheObjectItem::ChangeToPreAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	AttackAttribute rkAttribute;
	ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
	if( !pMidnightItem )
		return;

	if( pMidnightItem->GetObjectEquipState() == ioMidnightItem::OES_GROUND )
		rkAttribute = m_PreAttackGround;
	else
		rkAttribute = m_PreAttackJump;

	DWORD dwCurTime = FRAMEGETTIME();
	//m_ChargeState = CS_PRE_ATTACK;
	pOwner->SetState( CS_MIDNIGHT_SCYTHE );
	m_ScytheState = SS_PRE_ATTACK;

	InitAniRotate();
	pOwner->NormalAttackOver( false );

	if( pMidnightItem->GetObjectEquipState() == ioMidnightItem::OES_GROUND )
		pOwner->SetAutoTarget( ATT_NORMAL );

	ClearKeyReserve();
	ClearCancelInfo();
	pOwner->ClearDirKey();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();

	int iAniID	     = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	float fTimeRate  = rkAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;
	DWORD dwEndDelay = rkAttribute.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, rkAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_dwPreAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay + dwEndDelay;

	float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fKeyReserve > 0.0f )
		m_dwKeyReserveTime = dwCurTime + fKeyReserve + dwPreDelay;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, 1.0f, false );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	SetAniRotate( pOwner, iAniID, fTimeRate );
}

void ioMidnightScytheObjectItem::OnPreAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( m_dwPreAttackEndTime < FRAMEGETTIME() )
	{
		pOwner->ClearDirDoubleClick();
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
		ClearState();
		m_ScytheState = SS_NONE;
		pOwner->SetState( CS_DELAY );
	}
}

void ioMidnightScytheObjectItem::OnProcessScytheState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	switch( m_ScytheState )
	{
	case SS_PRE_ATTACK:
		{
			OnPreAttack( pOwner );
		}
		break;
	}
}

bool ioMidnightScytheObjectItem::IsPreAttackState()
{
	if( m_ScytheState == SS_PRE_ATTACK )
		return true;

	return false;
}

void ioMidnightScytheObjectItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToFullGatherState( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioMidnightScytheObjectItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;
	ClearCancelInfo();

	pOwner->InitExtendAttackTagInfo();

	bool bMaxAttack = false;
	bool bMaxBuffRemove = false;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
		m_szCurAni = pAttribute->m_AttackAnimation;
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ATTACK_FIRE;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMidnightScytheObjectItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	pOwner->SetState( CS_DELAY );
}

void ioMidnightScytheObjectItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_ATTACK_FIRE:
		{
			rkPacket >> m_iCurCombo;

			bool bCharged;
			rkPacket >> bCharged;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner, bCharged );
		}
		break;

	case SST_CMD_STATE:
		{
			rkPacket >> m_iCurCombo;

			ioHashString szCmdInfo;
			int iCmdCombo;
			rkPacket >> szCmdInfo >> iCmdCombo;

			ApplyCmdSet( pOwner, szCmdInfo, iCmdCombo );
		}
		break;
	case SST_CMD_STATE_D:
		{
			rkPacket >> m_iCurCombo;

			ioHashString szCmdInfo;
			int iCmdCombo;
			rkPacket >> szCmdInfo >> iCmdCombo;

			ApplyCmdSetD( pOwner, szCmdInfo, iCmdCombo );
		}
		break;
	case SST_CMD_STATE_S:
		{
			rkPacket >> m_iCurCombo;

			ioHashString szCmdInfo;
			int iCmdCombo;
			rkPacket >> szCmdInfo >> iCmdCombo;

			ApplyCmdSetS( pOwner, szCmdInfo, iCmdCombo );
		}
		break;
	}
}

void ioMidnightScytheObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioMidnightScytheObjectItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioMidnightScytheObjectItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioMidnightScytheObjectItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioMidnightScytheObjectItem::ProcessCancel( ioBaseChar *pOwner )
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

			/*if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetDashState( false );
				pOwner->SendDashState( false );

				return true;
			}
			else*/ if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
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

bool ioMidnightScytheObjectItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioMidnightScytheObjectItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

bool ioMidnightScytheObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioMidnightScytheObjectItem::IsEnableDash() const
{
	return true;
}

bool ioMidnightScytheObjectItem::IsEnableDefense() const
{
	return true;
}

void ioMidnightScytheObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	ioItem::OnEquiped( pOwner );

	ChangeToPreAttack( pOwner );
	m_bIsGaugeUseUp = false;
	m_bObjectRelease = false;
}

void ioMidnightScytheObjectItem::OnReleased( ioBaseChar *pOwner )
{
	//ioItem::OnReleased( pOwner );

	ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
	if( !pMidnightItem )
		return;

	if( /*pOwner->IsNeedProcess() &&*/ !m_bIsGaugeUseUp )
		pMidnightItem->ChangeToClawAttack( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

void ioMidnightScytheObjectItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
}

void ioMidnightScytheObjectItem::ChangeToFullGatherState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
	if( !pMidnightItem )
		return;

	//pMidnightItem->ReleaseObject( pOwner );
	m_bObjectRelease = true;
}

void ioMidnightScytheObjectItem::ChangeEnd( ioBaseChar *pOwner )
{
	m_ChargeState = CS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY );
}

void ioMidnightScytheObjectItem::OnFullGathering( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( /*pOwner->IsNeedProcess() &&*/ m_dwMotionEndTime < dwCurTime )
		ChangeEnd( pOwner );

}

void ioMidnightScytheObjectItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
{
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

	/*if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}

void ioMidnightScytheObjectItem::OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
	if( !pMidnightItem )
		return;

	if( pMidnightItem->GetCurExtraGauge() <= 0.0f && !m_bObjectRelease && pOwner->GetState() != CS_CMD_STATE )
	{
		m_bIsGaugeUseUp = true;
		m_bObjectRelease = true;
		//pMidnightItem->ReleaseObject( pOwner );
		//pMidnightItem->ChangeToClawAttack( pOwner );
	}

	if( m_bObjectRelease )
		pMidnightItem->ReleaseObject( pOwner );
}

void	ioMidnightScytheObjectItem::LoadCmdInfoList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iCmdInoCnt = rkLoader.LoadInt_e( "max_cmd_info_cnt", 0 );
	if( iCmdInoCnt > 0 )
	{
		for( int i=0; i < iCmdInoCnt; ++i )
		{
			ChargeCmdInfo kCmdInfo;

			wsprintf_e( szKey, "cmd_info%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_COMMAND_CNT );
			kCmdInfo.m_Cmd = szBuf;

			wsprintf_e( szKey, "cmd_info%d_use_type", i+1 );
			kCmdInfo.m_CmdUseType = (CmdUseType)rkLoader.LoadInt( szKey, CUT_NORMAL );

			wsprintf_e( szKey, "cmd_info%d_skill_slot", i+1 );
			kCmdInfo.m_iSkillSlot = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "cmd_info%d_combo_cnt", i+1 );
			int iMaxCmdCombo = rkLoader.LoadInt( szKey, 0 );
			if( iMaxCmdCombo > 0 )
				kCmdInfo.m_SubInfoList.reserve( iMaxCmdCombo );

			for( int j=0; j < iMaxCmdCombo; ++j )
			{
				ChargeCmdSubInfo kSubInfo;

				wsprintf_e( szKey, "cmd_info%d_combo%d", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack, rkLoader );

				wsprintf_e( szKey, "cmd_info%d_combo%d_use_d", i+1, j+1 );
				kSubInfo.m_bCmdAttack_D = rkLoader.LoadBool( szKey, false );

				wsprintf_e( szKey, "cmd_info%d_combo%d_use_s", i+1, j+1 );
				kSubInfo.m_bCmdAttack_S = rkLoader.LoadBool( szKey, false );

				wsprintf_e( szKey, "cmd_info%d_combo%d_d", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack_D, rkLoader );

				wsprintf_e( szKey, "cmd_info%d_combo%d_s", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack_S, rkLoader );

				kCmdInfo.m_SubInfoList.push_back( kSubInfo );
			}

			m_ChargeCmdInfoList.push_back( kCmdInfo );

			float	gauge;
			wsprintf_e( szKey, "cmd_gauge%d", i + 1 );
			gauge = rkLoader.LoadFloat( szKey, 0.0f );
			m_vCmdGauge.push_back( gauge );
		}
	}
}

bool ioMidnightScytheObjectItem::ChangeToNewCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	int iCmdGaugeIndex = 0;
	ChargeCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		ChargeCmdInfo &rkInfo = (*iter);

		if( strcmp(rkInfo.m_Cmd.c_str(), m_CurCmdInfo) == 0 )
		{
			if( rkInfo.m_CmdUseType == CUT_SKILL )
			{
				if( !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(rkInfo.m_iSkillSlot, SUT_NORMAL))
				{
					ClearState();
					m_ChargeState = CS_NONE;

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << (int)m_ChargeState;
						kPacket << m_iCurCombo;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					return true;
				}

				m_iCmdInputType = CIT_ALL;
				m_PreDirKey = ioUserKeyInput::DKI_NONE;

				m_iCurCmdIndex = 0;
				memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

				m_bCmdInputD = false;
				m_bCmdInputS = false;

				m_dwInputStartTime = 0;
				m_dwInputCheckTime = 0;

				return false;
			}
			else
			{
				ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;

				if( rkSubInfoList.empty() )
				{
					m_iCmdInputType = CIT_ALL;
					m_PreDirKey = ioUserKeyInput::DKI_NONE;

					m_iCurCmdIndex = 0;
					memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

					m_bCmdInputD = false;
					m_bCmdInputS = false;

					m_dwInputStartTime = 0;
					m_dwInputCheckTime = 0;
					return false;
				}

				m_iCurCmdCombo = 0;
				m_CurChargeCmdInfo = rkInfo;

				if( iCmdGaugeIndex < (int)m_vCmdGauge.size() )
					m_fCurCmdGauge = m_vCmdGauge[iCmdGaugeIndex];
				else
					m_fCurCmdGauge = 0.0f;

				pOwner->SetCmdAttackState();
				SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack, m_fCurCmdGauge );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << SST_CMD_STATE;
					kPacket << m_iCurCombo;
					kPacket << rkInfo.m_Cmd;
					kPacket << m_iCurCmdCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				// send after...
				m_iCmdInputType = CIT_ALL;
				m_PreDirKey = ioUserKeyInput::DKI_NONE;

				m_iCurCmdIndex = 0;
				memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

				m_bCmdInputD = false;
				m_bCmdInputS = false;

				m_dwInputStartTime = 0;
				m_dwInputCheckTime = 0;

				return true;
			}
		}

		++iter;
		++iCmdGaugeIndex;
	}

	//
	m_iCmdInputType = CIT_ALL;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_bCmdInputD = false;
	m_bCmdInputS = false;

	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;

	m_iCurCmdCombo = 0;
	m_CurChargeCmdInfo.Init();

	return false;
}

bool ioMidnightScytheObjectItem::ChangeToComboCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	if( m_CurChargeCmdInfo.m_Cmd.IsEmpty() )
	{
		return ChangeToNewCmdSet( pOwner );
	}

	if( !m_bCmdInputD && !m_bCmdInputS && strcmp(m_CurChargeCmdInfo.m_Cmd.c_str(), m_CurCmdInfo) != 0 )
	{
		return ChangeToNewCmdSet( pOwner );
	}

	ChargeCmdSubInfoList &rkSubInfoList = m_CurChargeCmdInfo.m_SubInfoList;
	if( !COMPARE( m_iCurCmdCombo, 0, (int)rkSubInfoList.size() ) )
	{
		return ChangeToNewCmdSet( pOwner );
	}

	if( rkSubInfoList[m_iCurCmdCombo].m_bCmdAttack_D && m_bCmdInputD )
	{
		pOwner->SetCmdAttackState();
		SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack_D, m_fCurCmdGauge );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_CMD_STATE_D;
			kPacket << m_iCurCombo;
			kPacket << m_CurChargeCmdInfo.m_Cmd;
			kPacket << m_iCurCmdCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		// send after...
		m_iCmdInputType = CIT_ALL;
		m_PreDirKey = ioUserKeyInput::DKI_NONE;

		m_iCurCmdIndex = 0;
		memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

		m_bCmdInputD = false;
		m_bCmdInputS = false;

		m_dwInputStartTime = 0;
		m_dwInputCheckTime = 0;

		m_iCurCmdCombo = 0;
		m_CurChargeCmdInfo.Init();

		return true;
	}
	else if( rkSubInfoList[m_iCurCmdCombo].m_bCmdAttack_S && m_bCmdInputS )
	{
		pOwner->SetCmdAttackState();
		SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack_S, m_fCurCmdGauge );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_CMD_STATE_S;
			kPacket << m_iCurCombo;
			kPacket << m_CurChargeCmdInfo.m_Cmd;
			kPacket << m_iCurCmdCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		// send after...
		m_iCmdInputType = CIT_ALL;
		m_PreDirKey = ioUserKeyInput::DKI_NONE;

		m_iCurCmdIndex = 0;
		memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

		m_bCmdInputD = false;
		m_bCmdInputS = false;

		m_dwInputStartTime = 0;
		m_dwInputCheckTime = 0;

		m_iCurCmdCombo = 0;
		m_CurChargeCmdInfo.Init();

		return true;
	}

	//
	m_iCurCmdCombo++;

	int iMaxCombo = rkSubInfoList.size();
	if( !COMPARE( m_iCurCmdCombo, 0, iMaxCombo ) )
		m_iCurCmdCombo = 0;


	pOwner->SetCmdAttackState();
	SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack, m_fCurCmdGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CMD_STATE;
		kPacket << m_iCurCombo;
		kPacket << m_CurChargeCmdInfo.m_Cmd;
		kPacket << m_iCurCmdCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send after...
	m_iCmdInputType = CIT_ALL;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_bCmdInputD = false;
	m_bCmdInputS = false;

	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;

	return true;
}

bool ioMidnightScytheObjectItem::ChangeToCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	if( m_ChargeCmdInfoList.empty() )
	{
		m_iCmdInputType = CIT_ALL;
		m_PreDirKey = ioUserKeyInput::DKI_NONE;

		m_iCurCmdIndex = 0;
		memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

		m_bCmdInputD = false;
		m_bCmdInputS = false;

		m_dwInputStartTime = 0;
		m_dwInputCheckTime = 0;

		m_iCurCmdCombo = 0;
		m_CurChargeCmdInfo.Init();

		return false;
	}

	if( m_CurChargeCmdInfo.m_Cmd.IsEmpty() )
		return ChangeToNewCmdSet( pOwner );

	return ChangeToComboCmdSet( pOwner );
}

bool ioMidnightScytheObjectItem::CheckCmdInput( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
	if( !pMidnightItem )
		return false;

	if( pMidnightItem->GetCurExtraGauge() <= 0.0f )
		return false;

	if( m_iCmdInputType == CIT_ALL || m_iCmdInputType == CIT_ACTION )
	{
		if( m_bCmdInputD || m_bCmdInputS )
			return false;

		int iMaxCnt = m_CurChargeCmdInfo.m_SubInfoList.size();
		if( COMPARE( m_iCurCmdCombo, 0, iMaxCnt ) )
		{
			bool bEnableD = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_bCmdAttack_D;
			bool bEnableS = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_bCmdAttack_S;

			if( bEnableD && !m_bCmdInputD && pOwner->IsAttackKey() )
			{
				m_bCmdInputD = true;
				return true;
			}
			else if( bEnableS && !m_bCmdInputS && pOwner->IsDefenseKey() )
			{
				m_bCmdInputS = true;
				return true;
			}
		}
	}

	if( m_iCmdInputType == CIT_ALL || m_iCmdInputType == CIT_DIR )
	{
		int iCurMaxCmd = MAX_COMMAND_CNT - 1;
		iCurMaxCmd = max( 0, iCurMaxCmd );
		DWORD dwCurTime = FRAMEGETTIME();

		ioUserKeyInput::DirKeyInput eDirKey = pOwner->GetFourDirKey();
		if( m_PreDirKey == eDirKey )
			return true;

		char szChar = '\0';

		switch( eDirKey )
		{
		case ioUserKeyInput::DKI_NONE:
			m_PreDirKey = eDirKey;
			return true;
		case ioUserKeyInput::DKI_RIGHTUP:
		case ioUserKeyInput::DKI_RIGHTDOWN:
		case ioUserKeyInput::DKI_LEFTDOWN:
		case ioUserKeyInput::DKI_LEFTUP:
			return true;
		case ioUserKeyInput::DKI_UP:
			szChar = '1';
			break;
		case ioUserKeyInput::DKI_RIGHT:
			szChar = '2';
			break;
		case ioUserKeyInput::DKI_DOWN:
			szChar = '3';
			break;
		case ioUserKeyInput::DKI_LEFT:
			szChar = '4';
			break;
		}

		if( !COMPARE( m_iCurCmdIndex, 0, iCurMaxCmd ) )
			return false;

		m_CurCmdInfo[m_iCurCmdIndex] = szChar;

		m_PreDirKey = eDirKey;
		m_iCurCmdIndex++;
		m_dwInputCheckTime = dwCurTime+m_dwInputGapTime;

		return true;
	}

	return false;
}

void ioMidnightScytheObjectItem::SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fGauge, float fExtraAniRate, float fExtraForceRate )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	InitAniRotate();
	pOwner->NormalAttackOver( false );
	pOwner->SetAutoTarget( ATT_NORMAL );

	int iAniID	     = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	float fTimeRate  = rkAttr.m_fAttackAniRate * fExtraAniRate;
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

	m_dwCmdAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay + dwEndDelay;

	float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fKeyReserve > 0.0f )
		m_dwKeyReserveTime = dwCurTime + fKeyReserve + dwPreDelay;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( rkAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, fExtraForceRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	SetAniRotate( pOwner, iAniID, fTimeRate );

	pOwner->IncreaseWeaponIndexBase();

	ioMidnightItem* pMidnight = ToMidnightItem( pOwner->GetWeapon() );
	if( pMidnight )
		pMidnight->SetUseExtraGauge( fGauge );
}

void ioMidnightScytheObjectItem::ClearState()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

void ioMidnightScytheObjectItem::ClearCmdInfo()
{
	m_iCmdInputType = CIT_ALL;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_bCmdInputD = false;
	m_bCmdInputS = false;

	m_dwCmdAttackEndTime = 0;
	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;

	m_iCurCmdCombo = 0;
	m_CurChargeCmdInfo.Init();
}

void ioMidnightScytheObjectItem::CheckCmdState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->CheckExtraAniJump();

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->CheckExtendAttackEndJump() )
			return;
	}

	if( m_dwCmdAttackEndTime > 0 && m_dwCmdAttackEndTime < dwCurTime )
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
		CheckAniRotate( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
				CheckReserve( pOwner );

			if( CheckCmdInputState( pOwner ) )
				return;
		}
	}
}

void ioMidnightScytheObjectItem::SetCmdInputStart( int iType, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsChatModeState( false ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_PreDirKey = ioUserKeyInput::DKI_NONE;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_bCmdInputD = false;
	m_bCmdInputS = false;

	m_iCurCmdIndex = 0;
	m_iCmdInputType = (CmdInputType)iType;

	m_dwInputStartTime = dwCurTime;
	m_dwInputCheckTime = dwCurTime+m_dwInputStartGapTime;
}

void ioMidnightScytheObjectItem::SetCmdInputEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return;

	ChangeToCmdSet( pOwner );
}

bool ioMidnightScytheObjectItem::CheckCmdInputState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return false;

	/*if( GetCurBullet() < GetNeedBullet() )
		return false;*/

	if( CheckCmdInputTime() )
	{
		return ChangeToCmdSet( pOwner );
	}

	CheckCmdInput( pOwner );

	return false;
}

bool ioMidnightScytheObjectItem::CheckCmdInputTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwInputStartTime+m_dwInputMaxTime < dwCurTime )
	{
		return true;
	}

	if( m_dwInputCheckTime < dwCurTime )
	{
		return true;
	}

	return false;
}

void ioMidnightScytheObjectItem::ClearCmdTime()
{
	m_dwInputStartTime = 0;
}

void ioMidnightScytheObjectItem::ApplyCmdSet( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo )
{
	if( !pOwner ) return;

	int iCmdGaugeIndex = 0;
	ChargeCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		ChargeCmdInfo &rkInfo = (*iter);
		if( strcmp(rkInfo.m_Cmd.c_str(), szCmdInfo.c_str()) == 0 )
		{
			if( rkInfo.m_CmdUseType == CUT_SKILL )
				return;

			//if( CheckMamaHahaCmd( rkInfo.m_CmdUseType ) && !CheckEnableMamaHahaCmd( pOwner, rkInfo.m_CmdUseType ) )
				//return;

			ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;
			if( !COMPARE( iCmdCombo, 0, (int)rkSubInfoList.size() ) )
				return;

			float fCmdGauge = 0.0f;
			if( iCmdGaugeIndex < (int)m_vCmdGauge.size() )
				fCmdGauge = m_vCmdGauge[iCmdGaugeIndex];

			pOwner->SetCmdAttackState();
			SetCmdAttack( pOwner, rkSubInfoList[iCmdCombo].m_CmdAttack, fCmdGauge );
			return;
		}

		++iter;
		++iCmdGaugeIndex;
	}
}

void ioMidnightScytheObjectItem::ApplyCmdSetD( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo )
{
	if( !pOwner ) return;

	int iCmdGaugeIndex = 0;
	ChargeCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		ChargeCmdInfo &rkInfo = (*iter);
		if( strcmp(rkInfo.m_Cmd.c_str(), szCmdInfo.c_str()) == 0 )
		{
			if( rkInfo.m_CmdUseType == CUT_SKILL )
				return;

			//if( CheckMamaHahaCmd( rkInfo.m_CmdUseType ) && !CheckEnableMamaHahaCmd( pOwner, rkInfo.m_CmdUseType ) )
				//return;

			ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;
			if( !COMPARE( iCmdCombo, 0, (int)rkSubInfoList.size() ) )
				return;

			if( !rkSubInfoList[iCmdCombo].m_bCmdAttack_D )
				return;

			float fCmdGauge = 0.0f;
			if( iCmdGaugeIndex < (int)m_vCmdGauge.size() )
				fCmdGauge = m_vCmdGauge[iCmdGaugeIndex];

			pOwner->SetCmdAttackState();
			SetCmdAttack( pOwner, rkSubInfoList[iCmdCombo].m_CmdAttack_D, fCmdGauge );
			return;
		}

		++iter;
		++iCmdGaugeIndex;
	}
}

void ioMidnightScytheObjectItem::ApplyCmdSetS( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo )
{
	if( !pOwner ) return;

	int iCmdGaugeIndex = 0;
	ChargeCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		ChargeCmdInfo &rkInfo = (*iter);
		if( strcmp(rkInfo.m_Cmd.c_str(), szCmdInfo.c_str()) == 0 )
		{
			if( rkInfo.m_CmdUseType == CUT_SKILL )
				return;

			//if( CheckMamaHahaCmd( rkInfo.m_CmdUseType ) && !CheckEnableMamaHahaCmd( pOwner, rkInfo.m_CmdUseType ) )
				//return;

			ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;
			if( !COMPARE( iCmdCombo, 0, (int)rkSubInfoList.size() ) )
				return;

			if( !rkSubInfoList[iCmdCombo].m_bCmdAttack_S )
				return;

			float fCmdGauge = 0.0f;
			if( iCmdGaugeIndex < (int)m_vCmdGauge.size() )
				fCmdGauge = m_vCmdGauge[iCmdGaugeIndex];

			pOwner->SetCmdAttackState();
			SetCmdAttack( pOwner, rkSubInfoList[iCmdCombo].m_CmdAttack_S, fCmdGauge );
			return;
		}

		++iter;
		++iCmdGaugeIndex;
	}
}

void ioMidnightScytheObjectItem::FillMidnightScytheInfo( ioBaseChar* pOwner, SP2Packet& rkPacket )
{

}

void ioMidnightScytheObjectItem::SetMidngihtScytheInfo( ioBaseChar* pOwner, SP2Packet& rkPacket )
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioMidnightScytheState::ioMidnightScytheState()
{
}

ioMidnightScytheState::~ioMidnightScytheState()
{
}

void	ioMidnightScytheState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void	ioMidnightScytheState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}
bool	ioMidnightScytheState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	return false;
}

void	ioMidnightScytheState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMidnightScytheObjectItem* pMidnightObject = ToObjectMidnightScytheItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMidnightObject )
	{
		pMidnightObject->OnProcessScytheState( pOwner );
	}
}

void	ioMidnightScytheState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool	ioMidnightScytheState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return false;
}

void	ioMidnightScytheState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioMidnightScytheObjectItem *pMidnightScythe = ToObjectMidnightScytheItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMidnightScythe )
	{
		if( pMidnightScythe->IsPreAttackState() )
			fCurGravity = pMidnightScythe->GetPreAttackJumpGravity();
		else
			fCurGravity = 0.0f;
	}
}