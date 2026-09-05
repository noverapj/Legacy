
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioChargeShotItem2.h"

#include "WeaponDefine.h"

ioChargeShotItem2::ioChargeShotItem2()
{
	m_dwAttackStartTime = 0;
	m_ChargeShotState = CSS_NONE;
	m_iCurCombo = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_bSetChargingMoveAni = false;

	m_FireEndAnimationTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bAttackCancel = false;
}

ioChargeShotItem2::ioChargeShotItem2( const ioChargeShotItem2 &rhs )
	: ioWeaponItem( rhs ),
	//m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	//m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
	//m_BranchDelayAni( rhs.m_BranchDelayAni ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_FireEndAnimation( rhs.m_FireEndAnimation ),
	m_fFireEndAnimation( rhs.m_fFireEndAnimation ),
	m_vWeaponFireInfoList( rhs.m_vWeaponFireInfoList ),
	m_stChargeAnimation( rhs.m_stChargeAnimation ),
	m_fChargeAniRate( rhs.m_fChargeAniRate ),
	m_dwAttackFullTime( rhs.m_dwAttackFullTime ),
	m_fWeaponRangeRate( rhs.m_fWeaponRangeRate ),
	m_dwAttackRateLimitTime( rhs.m_dwAttackRateLimitTime ),
	m_dwAttackMinTime( rhs.m_dwAttackMinTime )
{
	m_dwAttackStartTime = 0;
	m_ChargeShotState = CSS_NONE;
	m_iCurCombo = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_bSetChargingMoveAni = false;

	m_FireEndAnimationTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bAttackCancel = false;
}

ioChargeShotItem2::~ioChargeShotItem2()
{
}

void ioChargeShotItem2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	//m_BranchDelayAni = szBuf;

	int iFireCnt = rkLoader.LoadInt_e( "weapon_fire_cnt", 0 );
	m_vWeaponFireInfoList.clear();
	m_vWeaponFireInfoList.reserve( iFireCnt );

	rkLoader.LoadString_e( "charge_motion", "", szBuf, MAX_PATH );
	m_stChargeAnimation = szBuf;

	m_fChargeAniRate = rkLoader.LoadFloat_e( "charge_animation_time_rate", FLOAT1 );

	m_dwAttackFullTime = (DWORD)rkLoader.LoadInt_e( "charge_full_time", 1000 );

	m_dwAttackMinTime = (DWORD)rkLoader.LoadInt_e( "charge_at_least_fire_time", 0 );

	m_fWeaponRangeRate = rkLoader.LoadFloat_e( "weapon_range_rate", FLOAT1 );

	m_dwAttackRateLimitTime = (DWORD)rkLoader.LoadInt_e( "charge_rate_limit_time", 0 );

	for( int i=0; i < iFireCnt; ++i )
	{
		WeaponFireInfo kFireInfo;
		DWORD dwFireTime = 0;
		float fFireAngle = 0.0f;

		wsprintf_e( szKey, "weapon_fire_time%d", i+1 );
		dwFireTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "weapon_fire_angle%d", i+1 );
		fFireAngle = rkLoader.LoadFloat( szKey, 0.0f );

		kFireInfo.m_dwWeaponFireTime = dwFireTime;
		kFireInfo.m_fWeaponFireAngle = fFireAngle;

		wsprintf_e( szKey, "weapon_wounded_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kFireInfo.m_WeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "weapon_wounded_duration%d", i+1 );
		kFireInfo.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "weapon_wounded_loop_ani%d", i+1 );
		kFireInfo.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "weapon_attribute_index%d", i+1 );
		kFireInfo.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "weapon_attribute_resist_index%d", i+1 );
		kFireInfo.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		m_vWeaponFireInfoList.push_back( kFireInfo );
	}


	rkLoader.LoadString_e( "attack_end_animation", "", szBuf, MAX_PATH );
	m_FireEndAnimation = szBuf;
	m_fFireEndAnimation = rkLoader.LoadFloat_e( "attack_end_time_rate", FLOAT1 );


	m_fAirJumpPower = rkLoader.LoadFloat_e( "jump_air_power", 0.0f );
	m_fAirForcePower = rkLoader.LoadFloat_e( "jump_air_force_power", 0.0f );
}

ioItem* ioChargeShotItem2::Clone()
{
	return new ioChargeShotItem2( *this );
}

ioWeaponItem::WeaponSubType ioChargeShotItem2::GetSubType() const
{
	return WST_CHARGE_SHOT;
}

void ioChargeShotItem2::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_iSkillInput = -1;
	m_bAttackCancel = false;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_dwExtendCancelTime = 0;
	m_dwKeyReserveTime = 0;

	int iExtendCnt = m_vWeaponFireInfoList.size();
	if( m_vWeaponFireInfoList.empty() )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeShotState = CSS_NONE;
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

		m_ChargeShotState = CSS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioChargeShotItem2::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeShotState )
	{
	case CSS_CHARGING:
		OnCharging( pOwner );
		break;
	case CSS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			ioAttackableItem::CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case CSS_AUTO_SHOT:
	case CSS_AUTO_MIN_SHOT:
		OnAutoShot( pOwner );
		break;
	case CSS_AUTO_SHOT_END:
		OnAutoShotEnd( pOwner );
		break;
	}
}

void ioChargeShotItem2::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAutoShot( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeShotState;
				kPacket << m_iCurCombo;
				kPacket << true;			// first
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioChargeShotItem2::OnAutoShot( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !pOwner->IsNeedProcess() )
	{
		ProcessChargeAttack( pOwner );
		return;
	}

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

	// Cancel Check
	CheckCancelReserve( pOwner );
	if( m_dwExtendCancelTime > 0 )
	{
		if( m_dwExtendCancelTime < dwCurTime )
		{
			if( m_bAttackCancel )
			{
				ChangeToAutoShotEnd( pOwner );

				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeShotState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
				return;
			}
			else if( m_iSkillInput == 0 )
			{
				if( !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
				{
					m_ChargeShotState = CSS_NONE;
					return;
				}
				else
				{
					m_iSkillInput = -1;
				}
			}
		}
	}

	// Change Check
	if( m_dwFireStartTime + m_dwAttackFullTime < dwCurTime )
	{
		ChangeToAutoShotEnd( pOwner );

		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeShotState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
		return;
	}
	
	//if( m_dwFireStartTime/*+m_dwFireEndTime*/ < dwCurTime )
	{
		// 버튼 뗌 : 종료동작으로...
		if( m_ChargeShotState != CSS_AUTO_MIN_SHOT )
		{
			if( !pOwner->IsAttackKeyDown() )
			{
				if( m_dwFireStartTime + m_dwAttackMinTime > dwCurTime )
					ChangeToMinAutoShot( pOwner );
				else
					ChangeToAutoShotEnd( pOwner );

				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeShotState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
				return;
			}
		}
		else if( m_dwFireStartTime + m_dwAttackMinTime < dwCurTime )
		{
			ChangeToAutoShotEnd( pOwner );

			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeShotState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
			return;
		}

		// 스킬 체크
		if( pOwner->IsDefenseKeyDown() && !pOwner->IsBuffLimitSkill() )
		{
			if( pOwner->SetUseSkill(0, SUT_NORMAL))
			{
				m_ChargeShotState = CSS_NONE;

				return;
			}
		}

		//m_iCurCombo++;
		//SetChangeFireMotion( pOwner );
		ProcessChargeAttack( pOwner );

		/*if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeShotState;
			kPacket << m_iCurCombo;
			kPacket << false;			// first
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}*/
		return;
	}
}


void ioChargeShotItem2::ProcessChargeAttack( ioBaseChar *pOwner)
{
	int iFireTimeCnt = m_vWeaponFireInfoList.size();
	if( iFireTimeCnt <= 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !COMPARE( m_iCurCombo, 0, iFireTimeCnt) )
		return;

	DWORD dwFireTime = m_vWeaponFireInfoList[m_iCurCombo].m_dwWeaponFireTime;
	if( dwFireTime == 0 )
		return;

	if( dwFireTime > 0 )
		dwFireTime += m_dwFireStartTime;

	if( dwCurTime > dwFireTime )
	{
		CreateWeapon( pOwner );

		/*if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)CSS_CREATE_WEAPON;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}*/
	}
}


void ioChargeShotItem2::CreateWeapon( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iFireTimeCnt = m_vWeaponFireInfoList.size();
	if( iFireTimeCnt <= 0 )
		return;

	if( !COMPARE( m_iCurCombo, 0, iFireTimeCnt) )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	DWORD dwWeaponIndex = m_dwWeaponBaseIndex + m_iCurCombo;

	//pOwner->SkillFireExplicit( m_vWeaponFireInfoList[m_iCurCombo].m_WeaponInfo, GetName(), dwWeaponIndex, vCurPos );
	pOwner->NormalAttackFireExplicit( m_vWeaponFireInfoList[m_iCurCombo].m_WeaponInfo );

	m_iCurCombo++;
}


void ioChargeShotItem2::OnAutoShotEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_FireEndAnimationTime < dwCurTime )
	{
		m_ChargeShotState = CSS_NONE;

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			//if( pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
			//{
				m_iSkillInput = -1;
				return;
			//}
			
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

void ioChargeShotItem2::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if ( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );
		m_szCurAni = pAttribute->m_AttackAnimation;

		m_dwCancelCheckTime = dwCurTime;
		m_dwInputCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeShotState = CSS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeShotState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeShotItem2::ApplyChangeToNormalAttack( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if ( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeShotState = CSS_NORMAL_ATTACK;
}

void ioChargeShotItem2::ChangeToAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeShotState = CSS_AUTO_SHOT;
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionExplicitIfDirChanged( !m_bChargeCharRotate );

	m_bSetChargingMoveAni = true;

	m_iCurCombo = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
		pGrp->ClearAllLoopAni( FLOAT10, true );

	SetChangeFireMotion( pOwner );
}


void ioChargeShotItem2::ChangeToMinAutoShot( ioBaseChar *pOwner )
{
	m_ChargeShotState = CSS_AUTO_MIN_SHOT;
}

void ioChargeShotItem2::ChangeToAutoShotEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	m_iSkillInput = -1;
	m_bAttackCancel = false;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_dwExtendCancelTime = 0;
	m_dwKeyReserveTime = 0;

	m_ChargeShotState = CSS_AUTO_SHOT_END;

	//pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );

	int iAniID = pGrp->GetAnimationIdx( m_FireEndAnimation );
	float fTimeRate = m_fFireEndAnimation;

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	if( iAniID == -1 )
	{
		m_FireEndAnimationTime = dwCurTime;
		return;
	}

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	m_FireEndAnimationTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
}

bool ioChargeShotItem2::SetChangeFireMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	int iSize = m_vWeaponFireInfoList.size();
	if( iSize <= 0 ) return false;

	int iIndex = m_iCurCombo % iSize;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return false;

	pOwner->InitActionStopDelay( true );

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	pGrp->ClearAllActionAni( 0.0f, true );

	//const AttackAttribute *pAttr = &m_vExtendAttributeList[iIndex];
	if( !m_stChargeAnimation.IsEmpty() )
	{
		iAniID = pGrp->GetAnimationIdx( m_stChargeAnimation );
		fTimeRate = m_fChargeAniRate;

		pGrp->SetActionAni( m_stChargeAnimation, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );

		//pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		//pOwner->IncreaseWeaponIndexBase();
	}

	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwExtendCancelTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_cancel" ) * fTimeRate;
	if( m_dwExtendCancelTime > 0 )
		m_dwExtendCancelTime += dwCurTime;

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	return true;
}

void ioChargeShotItem2::ApplyChangeAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeShotState = CSS_AUTO_SHOT;
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_bSetChargingMoveAni = true;

	m_iCurCombo = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
		pGrp->ClearAllLoopAni( FLOAT10, true );
}

void ioChargeShotItem2::ApplySetChangeFireMotion( ioBaseChar *pOwner )
{
	int iSize = m_vWeaponFireInfoList.size();
	if( iSize <= 0 ) return;

	int iIndex = m_iCurCombo % iSize;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( FLOAT10, true );

	//const AttackAttribute *pAttr = &m_vExtendAttributeList[iIndex];
	if( !m_stChargeAnimation.IsEmpty() )
	{
		iAniID = pGrp->GetAnimationIdx( m_stChargeAnimation );
		fTimeRate = m_fChargeAniRate;

		pGrp->SetActionAni( m_stChargeAnimation, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );

		//pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		//pOwner->IncreaseWeaponIndexBase();
	}

	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwExtendCancelTime = m_dwFireStartTime + pGrp->GetAnimationEventTime_e( iAniID, "attack_cancel" ) * fTimeRate;
	return;
}

void ioChargeShotItem2::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CSS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ApplyChangeToNormalAttack( pOwner );
		break;
	case CSS_AUTO_SHOT:
		{
			bool bFirst;
			rkPacket >> bFirst;

			if( bFirst )
				ApplyChangeAutoShot( pOwner );
			else
				m_ChargeShotState = CSS_AUTO_SHOT;

			ApplySetChangeFireMotion( pOwner );
		}
		break;
	case CSS_CREATE_WEAPON:
		CreateWeapon(pOwner);
		break;
	case CSS_AUTO_SHOT_END:
		ChangeToAutoShotEnd( pOwner );
		break;
	}
}

void ioChargeShotItem2::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( false, false );
		}
	}

	ioIgnoreMissileWeapon2 *pWeaponMissile = ToIgnoreMissileWeapon2( pWeapon );
	if( pWeaponMissile )
	{
		DWORD dwCurTime = FRAMEGETTIME();

		DWORD dwCap = (dwCurTime - m_dwFireStartTime);
		float fRate = (float)dwCap / (float)m_dwAttackFullTime;
		fRate = min( FLOAT1, fRate );

		float fLimitRate = (float)m_dwAttackRateLimitTime / (float)m_dwAttackFullTime;
		fRate = min( fRate, fLimitRate );

		fRate *= m_fWeaponRangeRate;
		pWeapon->SetOwnerChargeRangeRate( max(1.0f + fRate, 0.f) );
	}
}

bool ioChargeShotItem2::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeShotState == CSS_CHARGING ||
			m_ChargeShotState == CSS_AUTO_SHOT )
			return true;
	}

	return false;
}


void ioChargeShotItem2::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_bSetChargingMoveAni = false;

	m_iCurCombo = 0;
}

void ioChargeShotItem2::GetAutoTargetValue( ioBaseChar *pOwner,
	float &fRange, float &fMinAngle, float &fMaxAngle,
	AutoTargetType eType )
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

	return;
}

bool ioChargeShotItem2::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeShotItem2::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeShotItem2::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioChargeShotItem2::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioChargeShotItem2::CheckCancelReserve( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	bool bReserveSkill = false;
	if( pOwner->IsAttackKeyDown() && pOwner->IsDefenseKeyDown() )
	{
		m_iSkillInput = 0;
		bReserveSkill = true;
	}

	if( !bReserveSkill && !pOwner->IsAttackKeyDown() )
	{
		m_bAttackCancel = true;
	}
}

void ioChargeShotItem2::CheckReserve( ioBaseChar *pOwner )
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

bool ioChargeShotItem2::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeShotState )
		{
		case CSS_AUTO_SHOT:
			return true;
		}
	}

	return false;
}
