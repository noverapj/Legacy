

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioRachelItem.h"

#include "WeaponDefine.h"

ioRachelItem::ioRachelItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCurCoolTime = 0.0f;
}

ioRachelItem::ioRachelItem( const ioRachelItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAni( rhs.m_AttackReadyAni ),
m_ExtendAttribute( rhs.m_ExtendAttribute ),
m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
m_GatherAnimation( rhs.m_GatherAnimation ),
m_GatheringEffect( rhs.m_GatheringEffect ),
m_fGatherAniRate( rhs.m_fGatherAniRate ),
m_iMaxLobeliaCnt( rhs.m_iMaxLobeliaCnt ),
m_SylphidInfo( rhs.m_SylphidInfo ),
m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
m_fRecoveryTime( rhs.m_fRecoveryTime )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCurCoolTime = 0.0f;
}

ioRachelItem::~ioRachelItem()
{
}

void ioRachelItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	LoadAttackAttribute_e( "extend_attack", m_ExtendAttribute, rkLoader );

	wsprintf_e( szBuf, "gathering_max_attack" );
	LoadAttackAttribute( szBuf, m_ExtendMaxAttribute, rkLoader );

	rkLoader.LoadString_e( "attack_ready_ani", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;

	m_iMaxLobeliaCnt = rkLoader.LoadInt_e( "max_lobelia_cnt", 0 );

	LoadSylphidInfo( rkLoader );
}

void ioRachelItem::LoadSylphidInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	m_SylphidInfo.m_dwDefaultNeed = (DWORD)rkLoader.LoadInt_e( "sylphid_default_need", 0 );
	rkLoader.LoadString_e( "sylphid_default_effect", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_DefaultEffect = szBuf;

	m_SylphidInfo.m_fDashAttackForceAmt = rkLoader.LoadFloat_e( "sylphid_dash_force_amt", 0.0f );
	m_SylphidInfo.m_fDashAttackFriction = rkLoader.LoadFloat_e( "sylphid_dash_friction", 0.0f );
	m_SylphidInfo.m_dwDashAttackNeed = (DWORD)rkLoader.LoadInt_e( "sylphid_dash_need", 0 );
	rkLoader.LoadString_e( "sylphid_dash_attack_effect", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_DashAttackEffect = szBuf;

	m_SylphidInfo.m_fJumpDashForceAmt = rkLoader.LoadFloat_e( "sylphid_jump_dash_force_amt", 0.0f );
	m_SylphidInfo.m_fJumpDashFriction = rkLoader.LoadFloat_e( "sylphid_jump_dash_friction", 0.0f );
	m_SylphidInfo.m_dwJumpDashNeed = (DWORD)rkLoader.LoadInt_e( "sylphid_jump_dash_need", 0 );
	rkLoader.LoadString_e( "sylphid_jump_dash_effect", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_JumpDashEffect = szBuf;

	m_SylphidInfo.m_fUpJumpPowerAmt = rkLoader.LoadFloat_e( "sylphid_up_jump_power", 0.0f );
	m_SylphidInfo.m_fUpJumpSpeedRate = rkLoader.LoadFloat_e( "sylphid_up_jump_speed_rate", FLOAT1 );
	m_SylphidInfo.m_dwUpJumpNeed = (DWORD)rkLoader.LoadInt_e( "sylphid_up_jump_need", 0 );
	rkLoader.LoadString_e( "sylphid_up_jump_effect", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_UpJumpEffect = szBuf;

	m_SylphidInfo.m_fDownJumpGrabityRate = rkLoader.LoadFloat_e( "sylphid_down_jump_gravity_rate", FLOAT1 );
	m_SylphidInfo.m_fDownJumpSpeedRate = rkLoader.LoadFloat_e( "sylphid_down_jump_speed_rate", FLOAT1 );
	m_SylphidInfo.m_dwDownJumpNeed = (DWORD)rkLoader.LoadInt_e( "sylphid_down_jump_need", 0 );
	rkLoader.LoadString_e( "sylphid_down_jump_effect", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_DownJumpEffect = szBuf;

	m_SylphidInfo.m_fUpJumpPowerAmt2 = rkLoader.LoadFloat_e( "sylphid_up_jump_power2", 0.0f );
	m_SylphidInfo.m_fUpJumpSpeedRate2 = rkLoader.LoadFloat_e( "sylphid_up_jump_speed_rate2", FLOAT1 );
	m_SylphidInfo.m_dwUpJumpNeed2 = (DWORD)rkLoader.LoadInt_e( "sylphid_up_jump_need2", 0 );
	rkLoader.LoadString_e( "sylphid_up_jump_effect2", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_UpJumpEffect2 = szBuf;

	m_SylphidInfo.m_fDownJumpGrabityRate2 = rkLoader.LoadFloat_e( "sylphid_down_jump_gravity_rate2", FLOAT1 );
	m_SylphidInfo.m_fDownJumpSpeedRate2 = rkLoader.LoadFloat_e( "sylphid_down_jump_speed_rate2", FLOAT1 );
	m_SylphidInfo.m_dwDownJumpNeed2 = (DWORD)rkLoader.LoadInt_e( "sylphid_down_jump_need2", 0 );
	rkLoader.LoadString_e( "sylphid_down_jump_effect2", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_DownJumpEffect2 = szBuf;

	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_fRecoveryTime = rkLoader.LoadFloat_e( "recover_cool_time", 0.0f );
}

ioItem* ioRachelItem::Clone()
{
	return new ioRachelItem( *this );
}

ioWeaponItem::WeaponSubType ioRachelItem::GetSubType() const
{
	return WST_RACHEL;
}

void ioRachelItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_AttackReadyAni.IsEmpty() )
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

void ioRachelItem::CheckNormalAttack( ioBaseChar *pOwner )
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
	case CS_NORMAL_ATTACK:
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
	}
}

void ioRachelItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioRachelItem::OnGathering( ioBaseChar *pOwner )
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
				pOwner->SetAttackMoveEnable( false );
				pOwner->SetChargingState( false );

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
			return;
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioRachelItem::ChangeToGathering( ioBaseChar *pOwner )
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
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRachelItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

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

void ioRachelItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bMax )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	m_dwCancelCheckTime = dwCurTime;
	m_dwInputCancelCheckTime = dwCurTime;

	m_szCurAni.Clear();
	m_fCurAniRate = FLOAT1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetAutoTarget( ATT_NORMAL );

	if( bMax && !m_ExtendMaxAttribute.m_AttackAnimation.IsEmpty() )
	{
		pOwner->SetNormalAttackByAttribute( m_ExtendMaxAttribute, true );

		m_szCurAni = m_ExtendMaxAttribute.m_AttackAnimation;
		m_fCurAniRate = m_ExtendMaxAttribute.m_fAttackAniRate;
	}
	else
	{
		pOwner->SetNormalAttackByAttribute( m_ExtendAttribute, true );

		m_szCurAni = m_ExtendAttribute.m_AttackAnimation;
		m_fCurAniRate = m_ExtendAttribute.m_fAttackAniRate;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	m_dwKeyReserveTime = dwCurTime;
	m_dwKeyReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fCurAniRate;

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << bMax;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRachelItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
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

		m_ChargeState = CS_NONE;
		break;
	case CS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			bool bMax;
			rkPacket >> bMax;

			ChangeToAttackFire( pOwner, bMax );
		}
		break;
	}
}

void ioRachelItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}

	ioLobeliaWeapon *pLobelia = ToLobeliaWeapon( pWeapon );
	if( pLobelia && m_iMaxLobeliaCnt > 0 )
	{
		g_WeaponMgr.CheckMaxLobeliaWeaponCnt( pWeapon, m_iMaxLobeliaCnt );
	}
}

bool ioRachelItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioRachelItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}
}

bool ioRachelItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioRachelItem::GetAutoTargetValue( ioBaseChar *pOwner,
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
		switch( eType )
		{
		case ATT_NORMAL:
			fRange = m_fAutoTargetRange;
			fMinAngle = m_fAutoTargetMinAngle;
			fMaxAngle = m_fAutoTargetMaxAngle;
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
	}
}

bool ioRachelItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
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

void ioRachelItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check bullet
	if( m_dwMaxCoolTime <= m_fCurCoolTime )
		return;

	// check owner state
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		break;
	default:
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fRecoveryTime * fTimePerSec;

	m_fCurCoolTime += fGauge;
	m_fCurCoolTime = (float)min( m_fCurCoolTime, (float)m_dwMaxCoolTime );
}

int ioRachelItem::GetMaxBullet()
{
	return (int)m_dwMaxCoolTime;
}

int ioRachelItem::GetCurBullet()
{
	DWORD dwCurCoolTime = m_fCurCoolTime;
	dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

	return (int)dwCurCoolTime;
}

void ioRachelItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	DWORD dwCurBullet = iCurBullet;
	dwCurBullet = max( 0, min( dwCurBullet, m_dwMaxCoolTime ) );

	m_fCurCoolTime = (float)dwCurBullet;
}

void ioRachelItem::InitCoolTime()
{
	m_fCurCoolTime = 0.0f;
}

void ioRachelItem::MaxCoolTime()
{
	m_fCurCoolTime = (float)m_dwMaxCoolTime;
}

void ioRachelItem::DecreaseCoolTime( int iNeedBullet )
{
	if( m_fCurCoolTime < (float)iNeedBullet )
	{
		m_fCurCoolTime = 0.0f;
	}
	else
	{
		m_fCurCoolTime -= (float)iNeedBullet;
	}
}


