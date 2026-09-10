

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioRunChargeItem2.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioRunChargeItem2::ioRunChargeItem2()
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwRunningEndTime = 0;
	m_dwRunningAttackEndTime = 0;
	m_dwRunningDefenseEndTime = 0;
	m_dwFinishAttackEndTime = 0;

	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFireDuration = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;
	m_dwRunningStartTime = 0;
}

ioRunChargeItem2::ioRunChargeItem2( const ioRunChargeItem2 &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAni( rhs.m_AttackReadyAni ),
m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute ),
m_ChargeDefenseAttribute( rhs.m_ChargeDefenseAttribute ),
m_FinishAttribute( rhs.m_FinishAttribute ),
m_RunningAni( rhs.m_RunningAni ),
m_fRunningAniRate( rhs.m_fRunningAniRate ),
m_dwRunningDuration( rhs.m_dwRunningDuration ),
m_dwRotateTime( rhs.m_dwRotateTime ),
m_fRunSpeed( rhs.m_fRunSpeed ),
m_szRetreatFr( rhs.m_szRetreatFr ),
m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
m_bOnlyTeamCharColSkip( rhs.m_bOnlyTeamCharColSkip )
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwRunningStartTime = 0;

	m_dwRunningEndTime = 0;
	m_dwRunningAttackEndTime = 0;
	m_dwRunningDefenseEndTime = 0;
	m_dwFinishAttackEndTime = 0;

	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFireDuration = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;
}

ioRunChargeItem2::~ioRunChargeItem2()
{
}

void ioRunChargeItem2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "charge_branch", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	wsprintf_e( szBuf, "charge_attack" );
	LoadAttackAttribute( szBuf, m_ChargeAttackAttribute, rkLoader );

	wsprintf_e( szBuf, "charge_defense" );
	LoadAttackAttribute( szBuf, m_ChargeDefenseAttribute, rkLoader );

	wsprintf_e( szBuf, "finish_attack" );
	LoadAttackAttribute( szBuf, m_FinishAttribute, rkLoader );

	// Run Ani
	rkLoader.LoadString_e( "charge_running_ani", "", szBuf, MAX_PATH );
	m_RunningAni = szBuf;
	m_fRunningAniRate = rkLoader.LoadFloat_e( "charge_running_ani_rate", FLOAT1 );

	m_dwRunningDuration = (DWORD)rkLoader.LoadInt_e( "charge_running_duration", 0 );

	// Run & Rotate
	m_fRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );

	m_bOnlyTeamCharColSkip = rkLoader.LoadBool_e( "only_team_char_col_skip", false );

	//
	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );
}

ioItem* ioRunChargeItem2::Clone()
{
	return new ioRunChargeItem2( *this );
}

ioWeaponItem::WeaponSubType ioRunChargeItem2::GetSubType() const
{
	return WST_RUN_CHARGE2;
}

void ioRunChargeItem2::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwRunningStartTime = 0;

	m_dwRunningEndTime = 0;
	m_dwRunningAttackEndTime = 0;
	m_dwRunningDefenseEndTime = 0;
	m_dwFinishAttackEndTime = 0;

	m_dwKeyReserveTime = 0;
	m_dwFireDuration = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
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

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAni );
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

void ioRunChargeItem2::ClearState()
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void ioRunChargeItem2::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_RUN:
		OnRunState( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	case CS_FINISH_ATTACK:
		OnFinishAttackState( pOwner );
		break;
	}
}

void ioRunChargeItem2::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToRunState( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioRunChargeItem2::OnRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime > 0 && m_dwRetreatEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

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
			ClearState();
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState();
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

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}

		if( m_dwRetreatForceTime > 0 && m_dwRetreatForceTime < dwCurTime )
		{
			m_dwRetreatForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fRetreatForceAmt, m_fRetreatForceFric );
		}
	}
}

void ioRunChargeItem2::OnRunState( ioBaseChar *pOwner )
{
	switch( m_RunState )
	{
	case RS_RUN:
		OnRunning( pOwner );
		break;
	case RS_ATTACK_KEY:
		OnRunningAttackKey( pOwner );
		break;
	case RS_DEFENSE_KEY:
		OnRunningDefenseKey( pOwner );
		break;
	}
}

void ioRunChargeItem2::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
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

void ioRunChargeItem2::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
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

void ioRunChargeItem2::ChangeToRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeState = CS_RUN;

	ChangeToRunning( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_RunState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRunChargeItem2::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CS_FINISH_ATTACK:
		ChangeToFinishAttack( pOwner );
		break;
	case CS_RUN:
		{
			int iRunState;
			rkPacket >> iRunState;

			switch( iRunState )
			{
			case RS_RUN:
				ChangeToRunState( pOwner );
				break;
			case RS_ATTACK_KEY:
				ChangeToRunningAttack( pOwner );
				break;
			case RS_DEFENSE_KEY:
				ChangeToRunningDefense( pOwner );
				break;
			}
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
	}
}

void ioRunChargeItem2::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SST_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;

			DWORD dwCurTime = FRAMEGETTIME();
			int iAniID = 0;

			if( iDir == ioUserKeyInput::DKI_NONE )
			{
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			}
			else
			{
				m_bTargetRot = false;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}
		}
		break;
	}
}

void ioRunChargeItem2::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	bool bRun = false;
	if( m_ChargeState == CS_RUN && m_RunState == RS_RUN )
		bRun = true;

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( bRun, false );
		}
	}

	if( bRun )
	{
		pWeapon->SetLiveTime( m_dwRunningDuration );
		pWeapon->SetCollisionCancel( true );
	}
}

bool ioRunChargeItem2::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_RUN:
		case CS_FINISH_ATTACK:
			return true;
		}
	}

	return false;
}

bool ioRunChargeItem2::IsMoveAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_RUN:
			{
				if( m_RunState == RS_RUN )
					return true;
			}
			break;
		}
	}

	return false;
}

void ioRunChargeItem2::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );
}

bool ioRunChargeItem2::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioRunChargeItem2::CheckRunningMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pOwner->IsNeedProcess() && pStage )
		pStage->SetTargetLookAt( true );

	pOwner->SetCurMoveSpeed( m_fRunSpeed );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
}

void ioRunChargeItem2::OnRunning( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime )
	{
		m_dwRunningEndTime = 0;

		ChangeToFinishAttack( pOwner );
		return;
	}
	else
	{
		if( pOwner && pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKey() )
				ChangeToRunningAttack( pOwner );
			else if( pOwner->IsDefenseKey() )
				ChangeToRunningDefense( pOwner );
			else if( pOwner->IsJumpKey() )
			{
				ClearState();
				pOwner->SetJumpState();
				return;
			}
		}

		// Skill Use
		int iSkillNum = pOwner->GetSkillKeyInput();
		if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
		{
			ClearState();

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

		CheckRunningMove( pOwner );
	}
}

void ioRunChargeItem2::OnRunningAttackKey( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	bool bCheckTimeOver = false;
	if( m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime )
		bCheckTimeOver = true;

	if( m_dwRunningAttackEndTime > 0 && m_dwRunningAttackEndTime < dwCurTime )
	{
		m_dwRunningAttackEndTime = 0;

		m_iSkillInput = -1;
		m_bEnableDefenseState = false;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
			pGrp->ClearAllActionAni( FLOAT100 );

		m_RunState = RS_RUN;
	}
	else if( !bCheckTimeOver && m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if( pOwner && pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKey() )
				ChangeToRunningAttack( pOwner );
			else if( pOwner->IsDefenseKey() )
				ChangeToRunningDefense( pOwner );
			else if( pOwner->IsJumpKey() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
			{
				ClearState();
				pOwner->SetJumpState();
				return;
			}
		}
	}

	CheckRunningMove( pOwner );
}

void ioRunChargeItem2::OnRunningDefenseKey( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	bool bCheckTimeOver = false;
	if( m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime )
		bCheckTimeOver = true;

	if( m_dwRunningDefenseEndTime > 0 && m_dwRunningDefenseEndTime < dwCurTime )
	{
		m_dwRunningDefenseEndTime = 0;

		m_iSkillInput = -1;
		m_bEnableDefenseState = false;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
			pGrp->ClearAllActionAni( FLOAT100 );

		m_RunState = RS_RUN;
	}
	else if( !bCheckTimeOver && m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if( pOwner && pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKey() )
				ChangeToRunningAttack( pOwner );
			else if( pOwner->IsDefenseKey() )
				ChangeToRunningDefense( pOwner );
			else if( pOwner->IsJumpKey() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
			{
				ClearState();
				pOwner->SetJumpState();
				return;
			}
		}
	}

	CheckRunningMove( pOwner );
}

void ioRunChargeItem2::OnFinishAttackState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFinishAttackEndTime > 0 && m_dwFinishAttackEndTime < dwCurTime )
	{
		m_dwFinishAttackEndTime = 0;

		// 예약처리
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

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
			ClearState();
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState();
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

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}

void ioRunChargeItem2::ChangeToRunning( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningEndTime = dwCurTime;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		float fAniRate = FLOAT1;
		if( m_fRunningAniRate > 0.0f )
			fAniRate = m_fRunningAniRate;

		int iAniID = pGrp->GetAnimationIdx( m_RunningAni );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fAniRate );
		m_dwRunningEndTime += m_dwRunningDuration;
	}

	m_RunState = RS_RUN;
	m_dwRunningStartTime = dwCurTime;

	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunningEndTime, m_bOnlyTeamCharColSkip );
}

void ioRunChargeItem2::ChangeToRunningAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningAttackEndTime = dwCurTime;
	m_dwKeyReserveTime = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( m_ChargeAttackAttribute.m_AttackAnimation );
		float fAniRate = m_ChargeAttackAttribute.m_fAttackAniRate;
		
		if( fAniRate < 0.0f )
			fAniRate = FLOAT1;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fAniRate );

		pOwner->RefreshFireTimeList( iAniID, m_ChargeAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fAniRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( m_ChargeAttackAttribute.m_vForceInfoList, iAniID, fAniRate, 0 );

		m_dwRunningAttackEndTime += pGrp->GetAnimationFullTime(iAniID) * fAniRate;

		float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
		if( fKeyReserve > 0.0f )
			m_dwKeyReserveTime = dwCurTime + fKeyReserve;
	}

	m_RunState = RS_ATTACK_KEY;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_RunState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRunChargeItem2::ChangeToRunningDefense( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningDefenseEndTime = dwCurTime;
	m_dwKeyReserveTime = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( m_ChargeDefenseAttribute.m_AttackAnimation );
		float fAniRate = m_ChargeDefenseAttribute.m_fAttackAniRate;

		if( fAniRate < 0.0f )
			fAniRate = FLOAT1;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fAniRate );

		pOwner->RefreshFireTimeList( iAniID, m_ChargeDefenseAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fAniRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( m_ChargeDefenseAttribute.m_vForceInfoList, iAniID, fAniRate, 0 );

		m_dwRunningDefenseEndTime += pGrp->GetAnimationFullTime(iAniID) * fAniRate;

		float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
		if( fKeyReserve > 0.0f )
			m_dwKeyReserveTime = dwCurTime + fKeyReserve;
	}

	m_RunState = RS_DEFENSE_KEY;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_RunState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRunChargeItem2::ChangeToFinishAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFinishAttackEndTime = dwCurTime;

	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pOwner->InitActionStopDelay( true );

		pOwner->ClearAttackFireTimeAndSkill();

		int iAniID = pGrp->GetAnimationIdx( m_FinishAttribute.m_AttackAnimation );

		float fCurRate = m_FinishAttribute.m_fAttackAniRate;
		if( fCurRate <= 0.0f )
			fCurRate = FLOAT1;

		DWORD dwPreDelay = m_FinishAttribute.m_dwPreDelay;
		DWORD dwEndDelay = m_FinishAttribute.m_dwEndDelay;

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate, 0.0f, dwPreDelay, true );

		pOwner->RefreshFireTimeList( iAniID, m_FinishAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( m_FinishAttribute.m_vForceInfoList, iAniID, fCurRate, 0 );

		m_dwFinishAttackEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;
		m_dwFinishAttackEndTime += dwEndDelay;

		DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
		if( dwKeyReserve > 0 )
			m_dwKeyReserveTime = dwCurTime + dwKeyReserve + dwPreDelay;
	}

	m_ChargeState = CS_FINISH_ATTACK;
	m_RunState = RS_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );
}

void ioRunChargeItem2::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_bTargetRot = false;
			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_ROTATE;
				kPacket << (int)m_CurDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_ROTATE;
				kPacket << (int)m_CurDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioRunChargeItem2::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fGapRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fGapRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	D3DXVECTOR3 vDir = pOwner->GetMoveDir();
	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vDir, &vTargetDir );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtAngle;

	float fDotValue = D3DXVec3Dot( &vDir, &vTargetDir );
	if( fDotValue < -0.98f )
		D3DXQuaternionRotationAxis( &qtAngle, &ioMath::UNIT_Y, DEGtoRAD(fRotateAngle) );
	else
		D3DXQuaternionRotationAxis( &qtAngle, &vAxis, DEGtoRAD(fRotateAngle) );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

void ioRunChargeItem2::CheckReserve( ioBaseChar *pOwner )
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

bool ioRunChargeItem2::CheckRetreat( ioBaseChar *pOwner, bool bFront )
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

	ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );

	return true;
}