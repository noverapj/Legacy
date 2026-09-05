
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioChargeShotItem.h"

#include "WeaponDefine.h"

ioChargeShotItem::ioChargeShotItem()
{
	m_dwAttackStartTime = 0;
	m_ChargeShotState = CSS_NONE;
	m_iCurCombo = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_FireStartDirKey = ioUserKeyInput::DKI_NONE;
	m_bFirstFireMoveMotion = true;
	m_bSetChargingMoveAni = false;

	m_FireEndAnimationTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bAttackCancel = false;
}

ioChargeShotItem::ioChargeShotItem( const ioChargeShotItem &rhs )
: ioWeaponItem( rhs ),
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
m_BranchDelayAni( rhs.m_BranchDelayAni ),
m_MotionInfoList( rhs.m_MotionInfoList ),
m_fFireMoveAniRate( rhs.m_fFireMoveAniRate ),
m_FireEndAnimation( rhs.m_FireEndAnimation ),
m_fFireEndAnimation( rhs.m_fFireEndAnimation )
{
	m_dwAttackStartTime = 0;
	m_ChargeShotState = CSS_NONE;
	m_iCurCombo = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_FireStartDirKey = ioUserKeyInput::DKI_NONE;
	m_bFirstFireMoveMotion = true;
	m_bSetChargingMoveAni = false;

	m_FireEndAnimationTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bAttackCancel = false;
}

ioChargeShotItem::~ioChargeShotItem()
{
}

void ioChargeShotItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAni = szBuf;

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

	m_MotionInfoList.clear();
	m_MotionInfoList.reserve( iExtendCnt );
	for( int i=0; i < iExtendCnt; ++i )
	{
		FireMotionInfo kInfo;

		wsprintf_e( szKey, "fire_move_front%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireMoveFront = szBuf;

		wsprintf_e( szKey, "fire_move_back%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireMoveBack = szBuf;

		wsprintf_e( szKey, "fire_move_left%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireMoveLeft = szBuf;

		wsprintf_e( szKey, "fire_move_right%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireMoveRight = szBuf;

		m_MotionInfoList.push_back( kInfo );
	}

	m_fFireMoveAniRate = rkLoader.LoadFloat_e( "fire_move_ani_rate", FLOAT1 );


	rkLoader.LoadString_e( "attack_end_animation", "", szBuf, MAX_PATH );
	m_FireEndAnimation = szBuf;
	m_fFireEndAnimation = rkLoader.LoadFloat_e( "attack_end_time_rate", FLOAT1 );


	m_fAirJumpPower = rkLoader.LoadFloat_e( "jump_air_power", 0.0f );
	m_fAirForcePower = rkLoader.LoadFloat_e( "jump_air_force_power", 0.0f );
}

ioItem* ioChargeShotItem::Clone()
{
	return new ioChargeShotItem( *this );
}

ioWeaponItem::WeaponSubType ioChargeShotItem::GetSubType() const
{
	return WST_CHARGE_SHOT;
}

void ioChargeShotItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_iSkillInput = -1;
	m_bAttackCancel = false;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_dwExtendCancelTime = 0;
	m_dwKeyReserveTime = 0;

	int iExtendCnt = m_vExtendAttributeList.size();
	if( m_vExtendAttributeList.empty() )
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

		int iAniID = pGrp->GetAnimationIdx( m_BranchDelayAni );
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

void ioChargeShotItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeShotState )
	{
	case CSS_CHARGING:
		OnCharging( pOwner );
		break;
	case CSS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case CSS_AUTO_SHOT:
		OnAutoShot( pOwner );
		break;
	case CSS_AUTO_SHOT_END:
		OnAutoShotEnd( pOwner );
		break;
	}

	CheckChargingMoveState( pOwner );
}

void ioChargeShotItem::OnCharging( ioBaseChar *pOwner )
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

void ioChargeShotItem::OnAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

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
	if( m_dwFireStartTime+m_dwFireEndTime < dwCurTime )
	{
		// 버튼 뗌 : 종료동작으로...
		if( !pOwner->IsAttackKeyDown() )
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

		m_iCurCombo++;
		SetChangeFireMotion( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeShotState;
			kPacket << m_iCurCombo;
			kPacket << false;			// first
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
}

void ioChargeShotItem::OnAutoShotEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_FireEndAnimationTime < dwCurTime )
	{
		m_ChargeShotState = CSS_NONE;

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

void ioChargeShotItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioChargeShotItem::ApplyChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioChargeShotItem::ChangeToAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeShotState = CSS_AUTO_SHOT;
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( !m_bChargeCharRotate );

	m_FireStartDirKey = pOwner->GetCurDirKey();

	m_bFirstFireMoveMotion = true;
	m_bSetChargingMoveAni = true;

	m_iCurCombo = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
		pGrp->ClearAllLoopAni( FLOAT10, true );

	SetChangeFireMotion( pOwner );
}

void ioChargeShotItem::ChangeToAutoShotEnd( ioBaseChar *pOwner )
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

bool ioChargeShotItem::SetChangeFireMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	int iSize = m_vExtendAttributeList.size();
	if( iSize <= 0 ) return false;

	int iIndex = m_iCurCombo % iSize;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	pOwner->InitActionStopDelay( true );

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	pGrp->ClearAllActionAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vExtendAttributeList[iIndex];
	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pGrp->SetActionAni( pAttr->m_AttackAnimation, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
	}

	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwExtendCancelTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_cancel" ) * fTimeRate;
	if( m_dwExtendCancelTime > 0 )
		m_dwExtendCancelTime += dwCurTime;

	return true;
}

void ioChargeShotItem::ApplyChangeAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeShotState = CSS_AUTO_SHOT;
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_FireStartDirKey = pOwner->GetCurDirKey();

	m_bFirstFireMoveMotion = true;
	m_bSetChargingMoveAni = true;

	m_iCurCombo = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
		pGrp->ClearAllLoopAni( FLOAT10, true );
}

void ioChargeShotItem::ApplySetChangeFireMotion( ioBaseChar *pOwner )
{
	int iSize = m_vExtendAttributeList.size();
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

	const AttackAttribute *pAttr = &m_vExtendAttributeList[iIndex];
	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pGrp->SetActionAni( pAttr->m_AttackAnimation, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
	}

	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwExtendCancelTime = m_dwFireStartTime + pGrp->GetAnimationEventTime_e( iAniID, "attack_cancel" ) * fTimeRate;
	return;
}

void ioChargeShotItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case CSS_AUTO_SHOT_END:
		ChangeToAutoShotEnd( pOwner );
		break;
	}
}

void ioChargeShotItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
}

bool ioChargeShotItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeShotState == CSS_CHARGING ||
			m_ChargeShotState == CSS_AUTO_SHOT )
			return true;
	}

	return false;
}

void ioChargeShotItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )	return;

	int iSize = m_vExtendAttributeList.size();
	if( iSize <= 0 ) return;

	int iIndex = m_iCurCombo % iSize;
	if( !COMPARE( iIndex, 0, (int)m_MotionInfoList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bFirstFireMoveMotion )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bSetChargingMoveAni = false;
			m_bFirstFireMoveMotion = false;

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_FireStartDirKey );

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_MotionInfoList[iIndex].m_FireMoveFront, FLOAT100, FLOAT1, m_fFireMoveAniRate );
			}
			else
			{
				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_MotionInfoList[iIndex].m_FireMoveFront, FLOAT100, FLOAT1, m_fFireMoveAniRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_MotionInfoList[iIndex].m_FireMoveRight, FLOAT100, FLOAT1, m_fFireMoveAniRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_MotionInfoList[iIndex].m_FireMoveBack, FLOAT100, FLOAT1, m_fFireMoveAniRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_MotionInfoList[iIndex].m_FireMoveLeft, FLOAT100, FLOAT1, m_fFireMoveAniRate );
					break;
				}
			}

			g_TutorialMgr.ActionAimingMove();
		}
	}
	else
	{
		if( !m_bSetChargingMoveAni )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bSetChargingMoveAni = true;
		}
	}
}

void ioChargeShotItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_bSetChargingMoveAni = false;

	m_iCurCombo = 0;
}

void ioChargeShotItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

bool ioChargeShotItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeShotItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeShotItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioChargeShotItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioChargeShotItem::CheckCancelReserve( ioBaseChar *pOwner )
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

void ioChargeShotItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioChargeShotItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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
