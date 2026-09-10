
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioLinearChargeItem2.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioLinearChargeItem2::ioLinearChargeItem2()
{
	m_dwAttackStartTime = 0;
	m_ChargeShotState = CSS_NONE;
	m_iCurCombo = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_FireEndAnimationTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioLinearChargeItem2::ioLinearChargeItem2( const ioLinearChargeItem2 &rhs )
: ioWeaponItem( rhs ),
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
m_AttackEndAttribute( rhs.m_AttackEndAttribute ),
m_RetreatAttribute( rhs.m_RetreatAttribute ),
m_BranchDelayAni( rhs.m_BranchDelayAni )
{
	m_dwAttackStartTime = 0;
	m_ChargeShotState = CSS_NONE;
	m_iCurCombo = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_FireEndAnimationTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioLinearChargeItem2::~ioLinearChargeItem2()
{
}

void ioLinearChargeItem2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

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

	LoadAttackAttribute_e( "attack_end_attack", m_AttackEndAttribute, rkLoader );

	// retreat
	LoadAttackAttribute_e( "retreat_attack", m_RetreatAttribute, rkLoader );
}

ioItem* ioLinearChargeItem2::Clone()
{
	return new ioLinearChargeItem2( *this );
}

ioWeaponItem::WeaponSubType ioLinearChargeItem2::GetSubType() const
{
	return WST_LINEAR_CHARGE2;
}

void ioLinearChargeItem2::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

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

void ioLinearChargeItem2::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeShotState )
	{
	case CSS_CHARGING:
		OnCharging( pOwner );
		break;
	case CSS_NORMAL_ATTACK:
		//input_jump_s 이벤트
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case CSS_AUTO_SHOT:
		OnAutoShot( pOwner );
		break;
	case CSS_FINISH_ATTACK:
		OnFinishAttack( pOwner );
		break;
	case CSS_RETREAT:
		OnRetreatState( pOwner );
		break;
	}
}

void ioLinearChargeItem2::OnCharging( ioBaseChar *pOwner )
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
				kPacket << pOwner->GetTargetRot();
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

void ioLinearChargeItem2::OnAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwKeyReserveTime > 0 &&
		m_dwKeyReserveTime < dwCurTime &&
		pOwner->IsDirKeyDoubleClick() &&
		!pOwner->HasBuff(BT_DISABLE_DASH) )
	{
		ChangeToRetreatAttack( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeShotState;
			kPacket << pOwner->GetTargetRot();
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	if( m_dwFireStartTime+m_dwFireEndTime < dwCurTime )
	{
		// 방향 체크
		pOwner->SetTargetRotToDirKey( pOwner->GetDirKey() );

		// 점프 체크
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetJumpState();
			return;
		}

		// Finish 체크
		if( m_bEnableAttackState )
		{
			ChangeToFinishAttack( pOwner );

			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeShotState;
			kPacket << pOwner->GetTargetRot();
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
			return;
		}

		// Change Check
		m_iCurCombo++;
		if( !SetChangeFireMotion( pOwner ) )
		{
			ProcessReserve( pOwner );
			return;
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeShotState;
			kPacket << pOwner->GetTargetRot();
			kPacket << m_iCurCombo;
			kPacket << false;			// first
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
	else
	{
		CheckReserve( pOwner );
	}
}

void ioLinearChargeItem2::OnFinishAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->CheckExtraAniJump();

	if( pOwner->IsNeedProcess() )
	{
		// 점프상태로 전환...
		if( pOwner->CheckExtendAttackEndJump() )
		{
			return;
		}
	}

	if( m_FireEndAnimationTime < dwCurTime )
	{
		m_ChargeShotState = CSS_NONE;
		ProcessReserve( pOwner );
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

void ioLinearChargeItem2::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	m_szCurAni.Clear();
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if ( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );
		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}	

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeShotState = CSS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeShotState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLinearChargeItem2::ApplyChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioLinearChargeItem2::ChangeToAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	m_ChargeShotState = CSS_AUTO_SHOT;

	m_iCurCombo = 0;
	SetChangeFireMotion( pOwner );
}

void ioLinearChargeItem2::ChangeToFinishAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_AttackEndAttribute.m_AttackAnimation.IsEmpty() )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_dwKeyReserveTime = 0;

	m_ChargeShotState = CSS_FINISH_ATTACK;

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
	pOwner->InitExtendAttackTagInfo();

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

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, m_AttackEndAttribute.m_dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, m_AttackEndAttribute.m_dwPreDelay );
}

bool ioLinearChargeItem2::SetChangeFireMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	int iSize = m_vExtendAttributeList.size();
	if( iSize <= 0 ) return false;

	if( m_iCurCombo >= iSize )
		return false;

	int iIndex = m_iCurCombo % iSize;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	pOwner->InitActionStopDelay( true );

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vExtendAttributeList[iIndex];
	if( pAttr )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );

		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		pGrp->SetActionAni( pAttr->m_AttackAnimation, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, 0 );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	float fValue = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwKeyReserveTime = dwCurTime;
	m_dwKeyReserveTime += fValue;


	return true;
}

void ioLinearChargeItem2::ApplyChangeAutoShot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeShotState = CSS_AUTO_SHOT;
	m_iCurCombo = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
		pGrp->ClearAllLoopAni( FLOAT10, true );
}

void ioLinearChargeItem2::ApplySetChangeFireMotion( ioBaseChar *pOwner )
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
		pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, 0 );
	}

	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;
	return;
}

void ioLinearChargeItem2::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, false, false );

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
	case CSS_FINISH_ATTACK:
		{
			ChangeToFinishAttack( pOwner );
		}
		break;
	}
}

void ioLinearChargeItem2::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioLinearChargeItem2::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeShotState == CSS_CHARGING ||
			m_ChargeShotState == CSS_AUTO_SHOT )
			return true;
	}

	return false;
}

bool ioLinearChargeItem2::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !bCheck ) return false;

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

void ioLinearChargeItem2::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_iCurCombo = 0;
}

bool ioLinearChargeItem2::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
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

void ioLinearChargeItem2::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioLinearChargeItem2::CheckReserve( ioBaseChar *pOwner )
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

void ioLinearChargeItem2::ProcessReserve( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// 예약처리
	pOwner->NormalAttackOver();

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
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() )
	{
		pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL );
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

	pOwner->SetState( CS_DELAY );

	return;
}

void ioLinearChargeItem2::OnRetreatState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime < dwCurTime )
	{
		m_ChargeShotState = CSS_NONE;

		if( pOwner->IsSettedDirection() )
		{
			pOwner->SetTargetRotToDirKey( pOwner->GetDirKey() );
		}

		ProcessReserve( pOwner );
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

void ioLinearChargeItem2::ChangeToRetreatAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeShotState = CSS_RETREAT;
	m_dwRetreatEndTime = dwCurTime;

	m_dwKeyReserveTime = 0;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	if( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_RetreatAttribute.m_AttackAnimation );
	float fTimeRate = m_RetreatAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_RetreatAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();

	m_dwRetreatEndTime += pGrp->GetAnimationFullTime( iAniID );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pOwner->SetReservedSliding( m_RetreatAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
}

bool ioLinearChargeItem2::IsMustBlowWoundState()
{
	switch( m_ChargeShotState )
	{
	case CSS_FINISH_ATTACK:
		return true;
	}

	return false;
}



