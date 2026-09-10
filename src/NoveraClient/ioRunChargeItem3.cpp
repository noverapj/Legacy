

#include "stdafx.h"

#include "ioRunChargeItem3.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioRunChargeItem3::ioRunChargeItem3()
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwRunningEndTime = 0;
	m_dwRunningAutoEndTime = 0;
	m_dwRunningKeyEndTime = 0;
	m_dwFinishAttackEndTime = 0;

	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFireDuration = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;
	m_dwRunningStartTime = 0;

	m_RunningLoopSoundID = 0;
	m_SpecialRunningLoopSoundID = 0;
}

ioRunChargeItem3::ioRunChargeItem3( const ioRunChargeItem3 &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAni( rhs.m_AttackReadyAni ),
m_ChargeAutoAttribute( rhs.m_ChargeAutoAttribute ),
m_SpecialChargeAutoAttribute( rhs.m_SpecialChargeAutoAttribute ),
m_ChargeKeyAttribute( rhs.m_ChargeKeyAttribute ),
m_SpecialChargeKeyAttribute( rhs.m_SpecialChargeKeyAttribute ),
m_FinishAttribute( rhs.m_FinishAttribute ),
m_SpecialFinishAttribute( rhs.m_SpecialFinishAttribute ),
m_RunningAni( rhs.m_RunningAni ),
m_fRunningAniRate( rhs.m_fRunningAniRate ),
m_dwRunningDuration( rhs.m_dwRunningDuration ),
m_bEnableSpeicalRunning( rhs.m_bEnableSpeicalRunning ),
m_SpecialRunningAni( rhs.m_SpecialRunningAni ),
m_fSpecialRunningAniRate( rhs.m_fSpecialRunningAniRate ),
m_dwSpecialRunningDuration( rhs.m_dwSpecialRunningDuration ),
m_dwRotateTime( rhs.m_dwRotateTime ),
m_dwSpecialRotateTime( rhs.m_dwSpecialRotateTime ),
m_fRunSpeed( rhs.m_fRunSpeed ),
m_fSpecialRunSpeed( rhs.m_fSpecialRunSpeed ),
m_szRetreatFr( rhs.m_szRetreatFr ),
m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
m_bOnlyTeamCharColSkip( rhs.m_bOnlyTeamCharColSkip ),
m_RunningLoopSound( rhs.m_RunningLoopSound ),
m_SpecialRunningLoopSound( rhs.m_SpecialRunningLoopSound ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwRunningStartTime = 0;

	m_dwRunningEndTime = 0;
	m_dwRunningAutoEndTime = 0;
	m_dwRunningKeyEndTime = 0;
	m_dwFinishAttackEndTime = 0;

	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFireDuration = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_RunningLoopSoundID = 0;
	m_SpecialRunningLoopSoundID = 0;
	m_fCurGauge = 0.0f;
}

ioRunChargeItem3::~ioRunChargeItem3()
{
}

void ioRunChargeItem3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "charge_branch", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	rkLoader.LoadString_e( "running_loop_sound", "", szBuf, MAX_PATH );
	m_RunningLoopSound = szBuf;
	rkLoader.LoadString_e( "special_running_loop_sound", "", szBuf, MAX_PATH );
	m_SpecialRunningLoopSound = szBuf;

	wsprintf_e( szBuf, "charge_auto_attack" );
	LoadAttackAttribute( szBuf, m_ChargeAutoAttribute, rkLoader );
	wsprintf_e( szBuf, "charge_special_auto_attack" );
	LoadAttackAttribute( szBuf, m_SpecialChargeAutoAttribute, rkLoader );

	wsprintf_e( szBuf, "charge_key_attack" );
	LoadAttackAttribute( szBuf, m_ChargeKeyAttribute, rkLoader );
	wsprintf_e( szBuf, "charge_special_key_attack" );
	LoadAttackAttribute( szBuf, m_SpecialChargeKeyAttribute, rkLoader );

	wsprintf_e( szBuf, "finish_attack" );
	LoadAttackAttribute( szBuf, m_FinishAttribute, rkLoader );
	wsprintf_e( szBuf, "special_finish_attack" );
	LoadAttackAttribute( szBuf, m_SpecialFinishAttribute, rkLoader );

	// Run Ani
	rkLoader.LoadString_e( "charge_running_ani", "", szBuf, MAX_PATH );
	m_RunningAni = szBuf;
	m_fRunningAniRate = rkLoader.LoadFloat_e( "charge_running_ani_rate", FLOAT1 );
	m_dwRunningDuration = (DWORD)rkLoader.LoadInt_e( "charge_running_duration", 0 );

	m_bEnableSpeicalRunning = rkLoader.LoadBool_e( "enable_special_running", false );
	rkLoader.LoadString_e( "charge_special_running_ani", "", szBuf, MAX_PATH );
	m_SpecialRunningAni = szBuf;
	m_fSpecialRunningAniRate = rkLoader.LoadFloat_e( "charge_special_running_ani_rate", FLOAT1 );
	m_dwSpecialRunningDuration = (DWORD)rkLoader.LoadInt_e( "charge_special_running_duration", 0 );

	// Run & Rotate
	m_fRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_fSpecialRunSpeed = rkLoader.LoadFloat_e( "charge_special_run_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );
	m_dwSpecialRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_special_run_rotate_speed", 0 );

	m_bOnlyTeamCharColSkip = rkLoader.LoadBool_e( "only_team_char_col_skip", false );

	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

ioItem* ioRunChargeItem3::Clone()
{
	return new ioRunChargeItem3( *this );
}

ioWeaponItem::WeaponSubType ioRunChargeItem3::GetSubType() const
{
	return WST_RUN_CHARGE3;
}

void ioRunChargeItem3::OnReleased( ioBaseChar *pOwner )
{
	m_fCurGauge = 0.0f;

	ioWeaponItem::OnReleased( pOwner );
}

void ioRunChargeItem3::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwRunningStartTime = 0;

	m_dwRunningEndTime = 0;
	m_dwRunningAutoEndTime = 0;
	m_dwRunningKeyEndTime = 0;
	m_dwFinishAttackEndTime = 0;

	m_dwKeyReserveTime = 0;
	m_dwFireDuration = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_RunningLoopSoundID = 0;
	m_SpecialRunningLoopSoundID = 0;

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

void ioRunChargeItem3::ClearState()
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( m_RunningLoopSoundID != 0 )
		g_SoundMgr.StopSound( m_RunningLoopSound, m_RunningLoopSoundID );
	if( m_SpecialRunningLoopSoundID != 0 )
		g_SoundMgr.StopSound( m_SpecialRunningLoopSound, m_SpecialRunningLoopSoundID );
}

void ioRunChargeItem3::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_RUNCHARGE:
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

void ioRunChargeItem3::OnCharging( ioBaseChar *pOwner )
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

void ioRunChargeItem3::OnRetreatState( ioBaseChar *pOwner )
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

void ioRunChargeItem3::OnRunState( ioBaseChar *pOwner )
{
	switch( m_RunState )
	{
	case RS_RUN:
		OnRunning( pOwner, false );
		break;
	case RS_AUTO:
		OnRunningAutoAttack( pOwner, false );
		break;
	case RS_ATTACK_KEY:
		OnRunningKeyAttack( pOwner, false );
		break;
	case RS_SPECIAL_RUN:
		OnRunning( pOwner, true );
		break;
	case RS_SPECIAL_AUTO:
		OnRunningAutoAttack( pOwner, true );
		break;
	case RS_SPECIAL_ATTACK_KEY:
		OnRunningKeyAttack( pOwner, true );
		break;
	}
}

void ioRunChargeItem3::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioRunChargeItem3::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
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

void ioRunChargeItem3::ChangeToRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeState = CS_RUNCHARGE;

	if( !m_RunningLoopSound.IsEmpty() )
	{
		if( m_RunningLoopSoundID != 0 )
			g_SoundMgr.StopSound( m_RunningLoopSound, m_RunningLoopSoundID );
		if( m_SpecialRunningLoopSoundID != 0 )
			g_SoundMgr.StopSound( m_SpecialRunningLoopSound, m_SpecialRunningLoopSoundID );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
			m_RunningLoopSoundID = g_SoundMgr.PlaySound( m_RunningLoopSound, pGrp->GetParentSceneNode(), PSM_LOOP | PSM_THREAD );
	}

	ChangeToRunning( pOwner, false );
}

void ioRunChargeItem3::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case CS_RUNCHARGE:
		{
			int iRunState;
			rkPacket >> iRunState;

			switch( iRunState )
			{
			case RS_RUN:
				ChangeToRunState( pOwner );
				break;
			case RS_AUTO:
				ChangeToRunningAutoAttack( pOwner, false );
				break;
			case RS_ATTACK_KEY:
				ChangeToRunningKeyAttack( pOwner, false );
				break;
			case RS_SPECIAL_RUN:
				ChangeToRunning( pOwner, true );
				break;
			case RS_SPECIAL_AUTO:
				ChangeToRunningAutoAttack( pOwner, true );
				break;
			case RS_SPECIAL_ATTACK_KEY:
				ChangeToRunningKeyAttack( pOwner, true );
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

void ioRunChargeItem3::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioRunChargeItem3::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	bool bRun = false;
	if( m_ChargeState == CS_RUNCHARGE && (m_RunState == RS_RUN || m_RunState == RS_SPECIAL_RUN) )
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

bool ioRunChargeItem3::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_RUNCHARGE:
		case CS_FINISH_ATTACK:
			return true;
		}
	}

	return false;
}

bool ioRunChargeItem3::IsMoveAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_RUNCHARGE:
			{
				if( m_RunState == RS_RUN || m_RunState == RS_SPECIAL_RUN )
					return true;
			}
			break;
		}
	}

	return false;
}

void ioRunChargeItem3::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );

	if( m_RunningLoopSoundID != 0 )
		g_SoundMgr.StopSound( m_RunningLoopSound, m_RunningLoopSoundID );
	if( m_SpecialRunningLoopSoundID != 0 )
		g_SoundMgr.StopSound( m_SpecialRunningLoopSound, m_SpecialRunningLoopSoundID );
}

bool ioRunChargeItem3::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioRunChargeItem3::CheckRunningMove( ioBaseChar *pOwner, bool bSpecial )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pOwner->IsNeedProcess() && pStage )
		pStage->SetTargetLookAt( true );

	if( bSpecial )
		pOwner->SetCurMoveSpeed( m_fSpecialRunSpeed );
	else
		pOwner->SetCurMoveSpeed( m_fRunSpeed );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner, bSpecial );
}

void ioRunChargeItem3::OnRunning( ioBaseChar *pOwner, bool bSpecial )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime )
	{
		m_dwRunningEndTime = 0;

		ChangeToFinishAttack( pOwner, bSpecial );
		return;
	}
	else
	{
		if( pOwner && pOwner->IsNeedProcess() )
		{
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

			if( pOwner->IsAttackKey() )
				ChangeToRunningKeyAttack( pOwner, bSpecial );
			else if( pOwner->IsJumpKey() )
			{
				ClearState();
				pOwner->SetJumpState();
				return;
			}
			else if( !bSpecial && m_bEnableSpeicalRunning && pOwner->IsDefenseKey() && IsEnableGauge() )
			{
				ChangeToRunning( pOwner, true );
				return;
			}
			else
				ChangeToRunningAutoAttack( pOwner, bSpecial );
		}

		CheckRunningMove( pOwner, bSpecial );
	}
}

void ioRunChargeItem3::OnRunningAutoAttack( ioBaseChar *pOwner, bool bSpecial )
{
	DWORD dwCurTime = FRAMEGETTIME();

	bool bCheckTimeOver = false;
	if( m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime )
		bCheckTimeOver = true;

	if( m_dwRunningAutoEndTime > 0 && m_dwRunningAutoEndTime < dwCurTime )
	{
		m_dwRunningAutoEndTime = 0;

		m_iSkillInput = -1;
		m_bEnableDefenseState = false;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
			pGrp->ClearAllActionAni( FLOAT100 );

		if( bSpecial )
			m_RunState = RS_SPECIAL_RUN;
		else
			m_RunState = RS_RUN;
	}
	else if( !bCheckTimeOver && m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if( pOwner && pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKey() )
			{
				ChangeToRunningKeyAttack( pOwner, bSpecial );
			}
			else if( pOwner->IsJumpKey() )
			{
				ClearState();
				pOwner->SetJumpState();
				return;
			}
			else if( !bSpecial && m_bEnableSpeicalRunning && pOwner->IsDefenseKey() && IsEnableGauge() )
			{
				ChangeToRunning( pOwner, true );
				return;
			}
		}
	}

	CheckRunningMove( pOwner, bSpecial );
}

void ioRunChargeItem3::OnRunningKeyAttack( ioBaseChar *pOwner, bool bSpecial )
{
	DWORD dwCurTime = FRAMEGETTIME();

	bool bCheckTimeOver = false;
	if( m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime )
		bCheckTimeOver = true;

	if( m_dwRunningKeyEndTime > 0 && m_dwRunningKeyEndTime < dwCurTime )
	{
		m_dwRunningKeyEndTime = 0;

		m_iSkillInput = -1;
		m_bEnableDefenseState = false;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
			pGrp->ClearAllActionAni( FLOAT100 );

		if( bSpecial )
			m_RunState = RS_SPECIAL_RUN;
		else
			m_RunState = RS_RUN;
	}
	else if( !bCheckTimeOver && m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if( pOwner && pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKey() )
			{
				ChangeToRunningKeyAttack( pOwner, bSpecial );
			}
			else if( pOwner->IsJumpKey() )
			{
				ClearState();
				pOwner->SetJumpState();
				return;
			}
			else if( !bSpecial && m_bEnableSpeicalRunning && pOwner->IsDefenseKey() && IsEnableGauge() )
			{
				ChangeToRunning( pOwner, true );
				return;
			}
		}
	}

	CheckRunningMove( pOwner, bSpecial );
}

void ioRunChargeItem3::OnFinishAttackState( ioBaseChar *pOwner )
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

void ioRunChargeItem3::ChangeToRunning( ioBaseChar *pOwner, bool bSpecial )
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
		if( bSpecial && m_fSpecialRunningAniRate > 0.f )
			fAniRate = m_fSpecialRunningAniRate;
		else if( !bSpecial && m_fRunningAniRate > 0.0f )
			fAniRate = m_fRunningAniRate;

		int iAniID = 0;
		if( bSpecial )
			iAniID = pGrp->GetAnimationIdx( m_SpecialRunningAni );
		else
			iAniID = pGrp->GetAnimationIdx( m_RunningAni );
		
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fAniRate );
		if( bSpecial )
			m_dwRunningEndTime += m_dwSpecialRunningDuration;
		else
			m_dwRunningEndTime += m_dwRunningDuration;
	}
	if( bSpecial )
	{
		if( !m_SpecialRunningLoopSound.IsEmpty() )
		{
			if( m_RunningLoopSoundID != 0 )
				g_SoundMgr.StopSound( m_RunningLoopSound, m_RunningLoopSoundID );
			if( m_SpecialRunningLoopSoundID != 0 )
				g_SoundMgr.StopSound( m_SpecialRunningLoopSound, m_SpecialRunningLoopSoundID );

			if( pGrp )
				m_SpecialRunningLoopSoundID = g_SoundMgr.PlaySound( m_SpecialRunningLoopSound, pGrp->GetParentSceneNode(), PSM_LOOP | PSM_THREAD );
		}
		m_RunState = RS_SPECIAL_RUN;
		CheckDecreaseGauge();
	}
	else
		m_RunState = RS_RUN;

	m_dwRunningStartTime = dwCurTime;

	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunningEndTime, m_bOnlyTeamCharColSkip );

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

void ioRunChargeItem3::ChangeToRunningAutoAttack( ioBaseChar *pOwner, bool bSpecial )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningAutoEndTime = dwCurTime;
	m_dwKeyReserveTime = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );

		AttackAttribute cAttackInfo;
		if( bSpecial )
			cAttackInfo = m_SpecialChargeAutoAttribute;
		else
			cAttackInfo = m_ChargeAutoAttribute;

		int iAniID = pGrp->GetAnimationIdx( cAttackInfo.m_AttackAnimation );
		float fAniRate = cAttackInfo.m_fAttackAniRate;
		
		if( fAniRate < 0.0f )
			fAniRate = FLOAT1;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fAniRate );

		pOwner->RefreshFireTimeList( iAniID, cAttackInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fAniRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( cAttackInfo.m_vForceInfoList, iAniID, fAniRate, 0 );

		m_dwRunningAutoEndTime += pGrp->GetAnimationFullTime(iAniID) * fAniRate;

		float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
		if( fKeyReserve > 0.0f )
			m_dwKeyReserveTime = dwCurTime + fKeyReserve;
	}
	if( bSpecial )
		m_RunState = RS_SPECIAL_AUTO;
	else
		m_RunState = RS_AUTO;

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

void ioRunChargeItem3::ChangeToRunningKeyAttack( ioBaseChar *pOwner, bool bSpecial )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningKeyEndTime = dwCurTime;
	m_dwKeyReserveTime = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );

		AttackAttribute cAttackInfo;
		if( bSpecial )
			cAttackInfo = m_SpecialChargeKeyAttribute;
		else
			cAttackInfo = m_ChargeKeyAttribute;

		int iAniID = pGrp->GetAnimationIdx( cAttackInfo.m_AttackAnimation );
		float fAniRate = cAttackInfo.m_fAttackAniRate;

		if( fAniRate < 0.0f )
			fAniRate = FLOAT1;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fAniRate );

		pOwner->RefreshFireTimeList( iAniID, cAttackInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fAniRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( cAttackInfo.m_vForceInfoList, iAniID, fAniRate, 0 );

		m_dwRunningKeyEndTime += pGrp->GetAnimationFullTime(iAniID) * fAniRate;

		float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
		if( fKeyReserve > 0.0f )
			m_dwKeyReserveTime = dwCurTime + fKeyReserve;
	}
	if( bSpecial )
		m_RunState = RS_SPECIAL_ATTACK_KEY;
	else
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

void ioRunChargeItem3::ChangeToFinishAttack( ioBaseChar *pOwner, bool bSpecial )
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

		AttackAttribute cAttackInfo;
		if( bSpecial )
			cAttackInfo = m_SpecialFinishAttribute;
		else
			cAttackInfo = m_FinishAttribute;

		int iAniID = pGrp->GetAnimationIdx( cAttackInfo.m_AttackAnimation );

		float fCurRate = cAttackInfo.m_fAttackAniRate;
		if( fCurRate <= 0.0f )
			fCurRate = FLOAT1;

		DWORD dwPreDelay = cAttackInfo.m_dwPreDelay;
		DWORD dwEndDelay = cAttackInfo.m_dwEndDelay;

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate, 0.0f, dwPreDelay, true );

		pOwner->RefreshFireTimeList( iAniID, cAttackInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( cAttackInfo.m_vForceInfoList, iAniID, fCurRate, 0 );

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

void ioRunChargeItem3::CheckKeyInput( ioBaseChar *pOwner )
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

void ioRunChargeItem3::ProcessRotate( ioBaseChar *pOwner, bool bSpecial )
{
	if( !pOwner ) return;
	if( bSpecial && m_dwSpecialRotateTime <= 0 ) return;
	if( !bSpecial && m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = FLOAT1;
	if( bSpecial )
		fRate = fFrameGap / m_dwSpecialRotateTime;
	else
		fRate = fFrameGap / m_dwRotateTime;

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

void ioRunChargeItem3::CheckReserve( ioBaseChar *pOwner )
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

bool ioRunChargeItem3::CheckRetreat( ioBaseChar *pOwner, bool bFront )
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

void ioRunChargeItem3::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioRunChargeItem3::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioRunChargeItem3::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioRunChargeItem3::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioRunChargeItem3::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioRunChargeItem3::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioRunChargeItem3::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}