
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioPaparazziItem.h"

#include "WeaponDefine.h"
#include "ioExtendAssaultDash.h"

ioPaparazziItem::ioPaparazziItem()
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

	m_iCurBullet = 0;
	m_fTempBullet = 0.0f;

	m_iCurLinearCombo = 0;
}

ioPaparazziItem::ioPaparazziItem( const ioPaparazziItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_fFireEndAnimation( rhs.m_fFireEndAnimation ),
	m_vWeaponFireInfoList( rhs.m_vWeaponFireInfoList ),
	m_stChargeAnimation( rhs.m_stChargeAnimation ),
	m_fChargeAniRate( rhs.m_fChargeAniRate ),
	m_dwAttackFullTime( rhs.m_dwAttackFullTime ),
	m_fWeaponRangeRate( rhs.m_fWeaponRangeRate ),
	m_dwAttackRateLimitTime( rhs.m_dwAttackRateLimitTime ),
	m_iMaxBullet( rhs.m_iMaxBullet ),
	m_iSpecialAttackNeedBullet( rhs.m_iSpecialAttackNeedBullet ),
	m_fBulletIncreaseRate( rhs.m_fBulletIncreaseRate ),
	m_vSpecialComboList( rhs.m_vSpecialComboList ),
	m_vSpecialComboEndAniList( rhs.m_vSpecialComboEndAniList ),
	m_vSpecialComboBranchAniList( rhs.m_vSpecialComboBranchAniList ),
	m_dwHalfChargeTime( rhs.m_dwHalfChargeTime ),
	m_HalfAttack( rhs.m_HalfAttack ),
	m_iMaxLinearCombo( rhs.m_iMaxLinearCombo ),
	m_bBulletGaugeInit( rhs.m_bBulletGaugeInit )
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

	m_iCurBullet = 0;
	m_fTempBullet = 0.0f;

	m_iCurLinearCombo = 0;
}

ioPaparazziItem::~ioPaparazziItem()
{
}

void ioPaparazziItem::LoadProperty( ioINILoader &rkLoader )
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

	int iFireCnt = rkLoader.LoadInt_e( "weapon_fire_cnt", 0 );
	m_vWeaponFireInfoList.clear();
	m_vWeaponFireInfoList.reserve( iFireCnt );

	rkLoader.LoadString_e( "charge_motion", "", szBuf, MAX_PATH );
	m_stChargeAnimation = szBuf;

	m_fChargeAniRate = rkLoader.LoadFloat_e( "charge_animation_time_rate", FLOAT1 );

	m_dwAttackFullTime = (DWORD)rkLoader.LoadInt_e( "charge_full_time", 1000 );

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

	m_fFireEndAnimation = rkLoader.LoadFloat_e( "linear_combo_end_time_rate", FLOAT1 );


	m_fAirJumpPower = rkLoader.LoadFloat_e( "jump_air_power", 0.0f );
	m_fAirForcePower = rkLoader.LoadFloat_e( "jump_air_force_power", 0.0f );

	m_iMaxBullet				= rkLoader.LoadInt_e( "max_bullet", 100 );
	m_iSpecialAttackNeedBullet	= rkLoader.LoadInt_e( "need_special_attack_bullet", 30 );
	m_fBulletIncreaseRate		= rkLoader.LoadFloat_e( "bullet_increase_rate", FLOAT1 );

	m_iMaxLinearCombo = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_vSpecialComboList.clear();

	if( m_iMaxLinearCombo > 0 )
	{
		m_vSpecialComboList.reserve( m_iMaxLinearCombo );

		for( int i=0; i < m_iMaxLinearCombo; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vSpecialComboList.push_back( kComboAttack );

			ioHashString szEndAni;
			wsprintf_e( szKey, "linear_combo%d_end_ani", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szEndAni = szBuf;

			m_vSpecialComboEndAniList.push_back( szEndAni );

			// 브렌치 애니는 공격 애니보다 갯수가 1개 차이로 적어야하기 때문에....

			wsprintf_e( szKey, "linear_combo%d_branch_ani", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szEndAni = szBuf;

			m_vSpecialComboBranchAniList.push_back( szEndAni );
		}
	}

	// 약 D~ charging Time 셋팅
	m_dwHalfChargeTime = rkLoader.LoadInt_e( "half_charging_time", 300 );
	LoadAttackAttribute( "half_attack", m_HalfAttack, rkLoader );

	m_bBulletGaugeInit  = rkLoader.LoadBool_e( "bullet_gauage_init_by_change_char", false );
}

ioItem* ioPaparazziItem::Clone()
{
	return new ioPaparazziItem( *this );
}

ioWeaponItem::WeaponSubType ioPaparazziItem::GetSubType() const
{
	return WST_PAPARAZZI_ITEM;
}

void ioPaparazziItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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
	if( m_vSpecialComboList.empty() )
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

void ioPaparazziItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeShotState )
	{
	case CSS_CHARGING:
		OnCharging( pOwner );
		break;
	case CSS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserveByConditionalSpecialDash( pOwner );

			if( ProcessCancelBySpeicalDash(pOwner) )
				return;

			ioAttackableItem::CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case CSS_AUTO_SHOT:
	case CSS_AUTO_SHOT_STOP:
	case CSS_AUTO_SHOT_BRANCH:
		OnAutoShot( pOwner );
		break;
	case CSS_HALF_CHARGE_ATTACK:
		OnHalfChargeAttack( pOwner );
		break;
	case CSS_AUTO_SHOT_END:
		OnAutoShotEnd( pOwner );
		break;
	}
}

void ioPaparazziItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAutoShot( pOwner );
		}
	}
	else if( pOwner->IsAttackKeyRelease() && ( m_dwAttackStartTime + m_dwHalfChargeTime <= FRAMEGETTIME() && m_dwAttackStartTime + m_dwChargeTime > FRAMEGETTIME() ) )
	{   
		if( m_iCurBullet > 0 )
		{
			// 약 D~
			ChangeToHalfChargeAttack( pOwner );
			DecreaseBulletsToDoSpecialAttack();
		}
		else
		{
			pOwner->SetChargingState( false );
			ChangeToNormalAttack( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioPaparazziItem::OnAutoShot( ioBaseChar *pOwner )
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
				if( !m_vSpecialComboEndAniList.empty() )
				{
					ChangeToAutoShotEnd( pOwner );

					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_ChargeShotState;
					P2PNetwork::SendToAllPlayingUser( kPacket );
					return;
				}
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
	if( m_iCurLinearCombo == m_iMaxLinearCombo && m_dwFireEndTime < dwCurTime )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			if( m_ChargeShotState != CSS_AUTO_SHOT_BRANCH )
			{
				if( !m_vSpecialComboEndAniList.empty() )
				{
					ChangeToAutoShotEnd( pOwner );

					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_ChargeShotState;
					P2PNetwork::SendToAllPlayingUser( kPacket );
					return;
				}
			}
		}
		else
		{
			// 각 공격 후 다음 공격 애니 전까지 이어지는 브렌치 애니 실행
			if( m_dwFireEndTime < dwCurTime && m_ChargeShotState != CSS_AUTO_SHOT_BRANCH )
			{
				m_dwFireEndTime = 0;
				if( !m_vSpecialComboBranchAniList.empty() )
				{
					ChangeToAutoShotBranch( pOwner );

					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_ChargeShotState;
					P2PNetwork::SendToAllPlayingUser( kPacket );
					return;
				}
			}

			if( m_FireEndAnimationTime < dwCurTime && m_ChargeShotState == CSS_AUTO_SHOT_BRANCH )
			{
				m_FireEndAnimationTime = 0;
				m_ChargeShotState = CSS_AUTO_SHOT_END;
				m_iCurLinearCombo = 0;

				// 대기
				pOwner->SetState( CS_DELAY );

				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeShotState;
				P2PNetwork::SendToAllPlayingUser( kPacket );

				m_ChargeShotState = CSS_NONE;
				return;
			}
		}
	}
	
	// 버튼 뗌 : 종료동작으로...
	if( m_ChargeShotState != CSS_AUTO_SHOT_STOP )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			if( m_FireEndAnimationTime < dwCurTime && m_ChargeShotState != CSS_AUTO_SHOT_BRANCH )
			{
				if( m_dwFireEndTime > dwCurTime )
					ChangeToAutoShotStop( pOwner );
				else
				{
					if( !m_vSpecialComboEndAniList.empty() )
					{
						ChangeToAutoShotEnd( pOwner );

						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << (int)m_ChargeShotState;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
				
				return;
			}
		}
		else
		{
			// 각 공격 후 다음 공격 애니 전까지 이어지는 브렌치 애니 실행
			if( m_dwFireEndTime < dwCurTime && m_ChargeShotState != CSS_AUTO_SHOT_BRANCH )
			{
				m_dwFireEndTime = 0;
				if( !m_vSpecialComboBranchAniList.empty() )
				{
					ChangeToAutoShotBranch( pOwner );

					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_ChargeShotState;
					P2PNetwork::SendToAllPlayingUser( kPacket );
					return;
				}
			}
			else if( m_FireEndAnimationTime > dwCurTime && m_ChargeShotState == CSS_AUTO_SHOT_BRANCH )
				return;
		}
	}
	else if( m_dwFireEndTime < dwCurTime )
	{
		if( !m_vSpecialComboEndAniList.empty() )
		{
			ChangeToAutoShotEnd( pOwner );

			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeShotState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
			return;
		}
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

	ProcessChargeAttack( pOwner );
	return;
}


void ioPaparazziItem::ProcessChargeAttack( ioBaseChar *pOwner )
{
	CheckLinearCombo( pOwner, false );
}


void ioPaparazziItem::CreateWeapon( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iFireTimeCnt = m_vWeaponFireInfoList.size();
	if( iFireTimeCnt <= 0 )
		return;

	if( !COMPARE( m_iCurCombo, 0, iFireTimeCnt) )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	DWORD dwWeaponIndex = m_dwWeaponBaseIndex + m_iCurCombo;

	pOwner->NormalAttackFireExplicit( m_vWeaponFireInfoList[m_iCurCombo].m_WeaponInfo );

	m_iCurCombo++;
}


void ioPaparazziItem::OnAutoShotEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_FireEndAnimationTime < dwCurTime )
	{
		m_ChargeShotState = CSS_NONE;

		m_iCurLinearCombo = 0;

		// 대기
		pOwner->SetState( CS_DELAY );

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

void ioPaparazziItem::OnHalfChargeAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireEndTime < dwCurTime )
	{
		m_dwAttackStartTime = 0;
		m_dwFireEndTime		= 0;
		m_ChargeShotState	= CSS_NONE;
		// 대기
		pOwner->SetState( CS_DELAY );
	}
}

void ioPaparazziItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioPaparazziItem::ApplyChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioPaparazziItem::ChangeToAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeShotState = CSS_AUTO_SHOT;
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionExplicitIfDirChanged( !m_bChargeCharRotate );

	m_bSetChargingMoveAni = true;

	m_iCurCombo = 0;
	// 브렌치 / 엔드 애니
	m_FireEndAnimationTime = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
		pGrp->ClearAllLoopAni( FLOAT10, true );

	CheckLinearCombo( pOwner, true );
}


void ioPaparazziItem::ChangeToAutoShotStop( ioBaseChar *pOwner )
{
	m_ChargeShotState = CSS_AUTO_SHOT_STOP;
}

void ioPaparazziItem::ChangeToAutoShotBranch( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( !COMPARE( ( m_iCurLinearCombo - 1 ), 0, (int)m_vSpecialComboBranchAniList.size() ) )
		return;

	m_iSkillInput = -1;
	m_bAttackCancel = false;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_dwExtendCancelTime = 0;
	m_dwKeyReserveTime = 0;

	m_ChargeShotState = CSS_AUTO_SHOT_BRANCH;

	int iAniID = pGrp->GetAnimationIdx( m_vSpecialComboBranchAniList[ m_iCurLinearCombo - 1 ] );
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

void ioPaparazziItem::ChangeToAutoShotEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( !COMPARE( ( m_iCurLinearCombo - 1 ), 0, (int)m_vSpecialComboEndAniList.size() ) )
		return;

	m_iSkillInput = -1;
	m_bAttackCancel = false;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_dwExtendCancelTime = 0;
	m_dwKeyReserveTime = 0;

	m_ChargeShotState = CSS_AUTO_SHOT_END;

	int iAniID = pGrp->GetAnimationIdx( m_vSpecialComboEndAniList[ m_iCurLinearCombo - 1 ] );
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

bool ioPaparazziItem::SetChangeFireMotion( ioBaseChar *pOwner )
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

	if( !m_stChargeAnimation.IsEmpty() )
	{
		iAniID = pGrp->GetAnimationIdx( m_stChargeAnimation );
		fTimeRate = m_fChargeAniRate;

		pGrp->SetActionAni( m_stChargeAnimation, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
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

void ioPaparazziItem::ApplyChangeAutoShot( ioBaseChar *pOwner )
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

void ioPaparazziItem::ApplySetChangeFireMotion( ioBaseChar *pOwner )
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

	if( !m_stChargeAnimation.IsEmpty() )
	{
		iAniID = pGrp->GetAnimationIdx( m_stChargeAnimation );
		fTimeRate = m_fChargeAniRate;

		pGrp->SetActionAni( m_stChargeAnimation, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
	}

	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwExtendCancelTime = m_dwFireStartTime + pGrp->GetAnimationEventTime_e( iAniID, "attack_cancel" ) * fTimeRate;
	return;
}

void ioPaparazziItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
			rkPacket >> m_iCurLinearCombo;

			SetNextLinearCombo( pOwner );
		}
		break;
	case CSS_CANCEL_DASH:
		{
			ioExtendAssaultDash *pExtendAssaultDash = ToExtendAssaultDash( GetCurExtendDash() );
			if ( !pExtendAssaultDash )
				return;

			int iDirKey = 0;;
			rkPacket >> iDirKey;
			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iDirKey, ioAttackableItem::CCSD_NORMAL );
		}
		break;
	case CSS_AUTO_SHOT_BRANCH:
		ChangeToAutoShotBranch( pOwner );
		break;
	case CSS_AUTO_SHOT_END:
		ChangeToAutoShotEnd( pOwner );
		break;
	case CSS_HALF_CHARGE_ATTACK:
		ChangeToHalfChargeAttack( pOwner );
		break;
	}
}

void ioPaparazziItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
}

bool ioPaparazziItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeShotState == CSS_CHARGING ||
			m_ChargeShotState == CSS_AUTO_SHOT )
			return true;
	}

	return false;
}


void ioPaparazziItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_bSetChargingMoveAni = false;

	m_iCurCombo = 0;
}

void ioPaparazziItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

bool ioPaparazziItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioPaparazziItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioPaparazziItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioPaparazziItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioPaparazziItem::CheckCancelReserve( ioBaseChar *pOwner )
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

void ioPaparazziItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioPaparazziItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

int ioPaparazziItem::GetCurBullet()
{
	return m_iCurBullet;
}

int ioPaparazziItem::GetMaxBullet()
{
	return m_iMaxBullet;
}

int ioPaparazziItem::GetNeedBullet()
{
	return m_iSpecialAttackNeedBullet;
}

void ioPaparazziItem::SetCurBullet( int iCurBullet )
{
	if( m_bBulletGaugeInit )
		return;

	if( iCurBullet < 0 )
		return;

	m_iCurBullet = max( 0, min( iCurBullet, m_iMaxBullet ) );
	m_fTempBullet = (float)m_iCurBullet;
}

void ioPaparazziItem::DecreaseBulletsToDoSpecialAttack()
{
	if( m_iCurBullet >= m_iSpecialAttackNeedBullet )
	{
		m_iCurBullet -= m_iSpecialAttackNeedBullet;
		m_fTempBullet = (float)m_iCurBullet;
	}
}

void ioPaparazziItem::InitExtraGauge()
{
	m_iCurBullet		= 0;
	m_fCurExtraGauge	= 0;

}

void ioPaparazziItem::UpdateExtraData( ioBaseChar *pOwner )
{
	// 여기에서 bullet 업데이트를 한다.
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( m_iCurBullet >= m_iMaxBullet )
	{
		m_iCurBullet = m_iMaxBullet;
		return;
	}

	if( m_iCurBullet < m_iMaxBullet )
	{
		m_fTempBullet	+= fTimePerSec * m_fBulletIncreaseRate;
		m_iCurBullet	= (int)m_fTempBullet;

		m_fCurExtraGauge = m_fTempBullet;
		m_fCurExtraGauge = min( m_fCurExtraGauge, (float)m_iMaxBullet );
	}
}

void ioPaparazziItem::SetNextLinearCombo( ioBaseChar *pChar )
{
	if( !pChar ) return;

	m_dwKeyReserveTime = 0;

	const AttributeList &rkComboList = m_vSpecialComboList;

	int iMaxCombo = rkComboList.size();
	if( !COMPARE( m_iCurLinearCombo, 0, iMaxCombo ) )
		return;

	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	AttackAttribute kAttack = rkComboList[m_iCurLinearCombo];

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	if( m_ChargeShotState == CSS_AUTO_SHOT_STOP )
	{
		m_dwFireStartTime = 0;
		return;
	}

	pChar->InitActionStopDelay( true );

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_iCurLinearCombo == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
		iAniID,
		kAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireEndTime = dwCurTime;
	m_dwFireEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	m_dwFireStartTime = dwCurTime;

	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwKeyReserveTime = dwCurTime;
	m_dwKeyReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}

	//

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );

	//
	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeShotState = CSS_AUTO_SHOT;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << (int)m_ChargeShotState;
		kPacket << m_iCurCombo;
		kPacket << m_iCurLinearCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( pChar->IsNeedProcess() )
		m_iCurLinearCombo++;
}

void ioPaparazziItem::CheckLinearCombo( ioBaseChar *pChar, bool bFirst )
{
	if( !pChar )
		return;

	const AttributeList &rkComboList = m_vSpecialComboList;
	if( !COMPARE( m_iCurLinearCombo, 0, (int)rkComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireEndTime < dwCurTime )
	{
		if( pChar->IsNeedProcess() )
		{
			SetNextLinearCombo( pChar );
		}
	}
}

void ioPaparazziItem::CheckComboState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	
	CheckLinearCombo( pChar, true );
}

void ioPaparazziItem::CheckPaparazziState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	// 공격 상태가 아니고 CurLinearCombo가 0이 아니면 CurLinearCombo를 0으로 변경
	if( pOwner->GetState() != CS_ATTACK && m_iCurLinearCombo > 0 )
		m_iCurLinearCombo = 0;
}

void ioPaparazziItem::ChangeToHalfChargeAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	m_dwKeyReserveTime	= 0;
	m_bAttackCancel		= false;

	// 2019-08-30
	m_dwFireStartTime = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_HalfAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pOwner->SetNormalAttackByAttribute( m_HalfAttack );
	m_szCurAni					= m_HalfAttack.m_AttackAnimation;
	m_fCurAniRate				= m_HalfAttack.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeShotState			= CSS_HALF_CHARGE_ATTACK;

	float fTimeRate = m_HalfAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_HalfAttack.m_dwPreDelay;

	m_dwFireEndTime = dwCurTime;
	m_dwFireEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireEndTime += dwPreDelay + m_HalfAttack.m_dwEndDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeShotState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioPaparazziItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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

void ioPaparazziItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioExtendAssaultDash *pExtendAssaultDash = ToExtendAssaultDash( GetCurExtendDash() );
	if ( !pExtendAssaultDash )
		return;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	pExtendAssaultDash->StartDash( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)CSS_CANCEL_DASH;
		kPacket << m_iCurCombo;
		kPacket << (int)eCurrDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioPaparazziItem::SetExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		m_iCurLinearCombo = 0;
		return m_pExtendDash->StartDash( pOwner );
	}

	return false;
}