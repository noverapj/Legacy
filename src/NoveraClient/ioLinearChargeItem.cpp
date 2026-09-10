
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioLinearChargeItem.h"

#include "WeaponDefine.h"

ioLinearChargeItem::ioLinearChargeItem()
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

ioLinearChargeItem::ioLinearChargeItem( const ioLinearChargeItem &rhs )
: ioWeaponItem( rhs ),
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
m_AttackEndAttribute( rhs.m_AttackEndAttribute ),
m_BranchDelayAni( rhs.m_BranchDelayAni ),
m_MotionInfoList( rhs.m_MotionInfoList ),
m_fFireMoveAniRate( rhs.m_fFireMoveAniRate ),
m_bOneTimeChargeAttack( rhs.m_bOneTimeChargeAttack )
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

ioLinearChargeItem::~ioLinearChargeItem()
{
}

void ioLinearChargeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAni = szBuf;

	m_bOneTimeChargeAttack = rkLoader.LoadBool_e( "onetime_charge_attack", false );

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

	LoadAttackAttribute_e( "attack_end_attack", m_AttackEndAttribute, rkLoader );

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

	m_bCharRot = rkLoader.LoadBool_e( "enable_charge_rotate", false );
}

ioItem* ioLinearChargeItem::Clone()
{
	return new ioLinearChargeItem( *this );
}

ioWeaponItem::WeaponSubType ioLinearChargeItem::GetSubType() const
{
	return WST_LINEAR_CHARGE;
}

void ioLinearChargeItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioLinearChargeItem::CheckNormalAttack( ioBaseChar *pOwner )
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

	//CheckChargingMoveState( pOwner );
}

void ioLinearChargeItem::OnCharging( ioBaseChar *pOwner )
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

void ioLinearChargeItem::OnAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	//
	if( pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
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
	//

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
		if( !SetChangeFireMotion( pOwner ) )
		{
			// 종료동작으로...
			ChangeToAutoShotEnd( pOwner );

			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeShotState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
			return;
		}

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

void ioLinearChargeItem::OnAutoShotEnd( ioBaseChar *pOwner )
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

void ioLinearChargeItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioLinearChargeItem::ApplyChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioLinearChargeItem::ChangeToAutoShot( ioBaseChar *pOwner )
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

void ioLinearChargeItem::ChangeToAutoShotEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_AttackEndAttribute.m_AttackAnimation.IsEmpty() )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_iSkillInput = -1;
	m_bAttackCancel = false;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_dwExtendCancelTime = 0;
	m_dwKeyReserveTime = 0;

	m_ChargeShotState = CSS_AUTO_SHOT_END;

	int iAniID = pGrp->GetAnimationIdx( m_AttackEndAttribute.m_AttackAnimation );
	float fTimeRate = FLOAT1;
	if( m_AttackEndAttribute.m_fAttackAniRate >= 0.0f )
		fTimeRate = m_AttackEndAttribute.m_fAttackAniRate;

	pOwner->SetAutoTarget( ATT_NORMAL );

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	if( iAniID == -1 )
	{
		m_FireEndAnimationTime = dwCurTime;
		return;
	}

	pOwner->InitActionStopDelay( true );

	pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
	pOwner->RefreshFireTimeList( iAniID, m_AttackEndAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_AttackEndAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	m_FireEndAnimationTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + m_AttackEndAttribute.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
}

bool ioLinearChargeItem::SetChangeFireMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	int iSize = m_vExtendAttributeList.size();
	if( iSize <= 0 ) return false;

	if( m_bOneTimeChargeAttack && m_iCurCombo >= iSize )
		return false;

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
		pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, 0 );
	}

	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwExtendCancelTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_cancel" ) * fTimeRate;
	if( m_dwExtendCancelTime > 0 )
		m_dwExtendCancelTime += dwCurTime;

	return true;
}

void ioLinearChargeItem::ApplyChangeAutoShot( ioBaseChar *pOwner )
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

void ioLinearChargeItem::ApplySetChangeFireMotion( ioBaseChar *pOwner )
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

	pOwner->InitActionStopDelay( true );
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
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwExtendCancelTime = m_dwFireStartTime + pGrp->GetAnimationEventTime_e( iAniID, "attack_cancel" ) * fTimeRate;
	return;
}

void ioLinearChargeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioLinearChargeItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioLinearChargeItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeShotState == CSS_CHARGING ||
			m_ChargeShotState == CSS_AUTO_SHOT )
			return true;
	}

	return false;
}

bool ioLinearChargeItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeShotState )
		{
		case CSS_AUTO_SHOT:
			{
				if( !bCheck )
					return false;
			}
			return true;
		}
	}

	return false;
}

void ioLinearChargeItem::CheckChargingMoveState( ioBaseChar *pOwner )
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

void ioLinearChargeItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_bSetChargingMoveAni = false;

	m_iCurCombo = 0;
}

void ioLinearChargeItem::GetAutoTargetValue( ioBaseChar *pOwner,
											 float &fRange, float &fMinAngle, float &fMaxAngle,
											 AutoTargetType eType )
{
	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;
}

bool ioLinearChargeItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioLinearChargeItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioLinearChargeItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioLinearChargeItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioLinearChargeItem::CheckCancelReserve( ioBaseChar *pOwner )
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

void ioLinearChargeItem::CheckReserve( ioBaseChar *pOwner )
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