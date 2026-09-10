

#include "stdafx.h"

#include "ioTraceItem.h"

#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioTraceItem::ioTraceItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedExtraGauge = false;
}

ioTraceItem::ioTraceItem( const ioTraceItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_vDodgeAniList( rhs.m_vDodgeAniList ),
	m_fDodgeAniRate( rhs.m_fDodgeAniRate ),
	m_fDodgeForce( rhs.m_fDodgeForce ),
	m_fDodgeFrictionRate( rhs.m_fDodgeFrictionRate ),
	m_bEnableExtraGauge( rhs.m_bEnableExtraGauge ),
	m_ExtraGaugeBuffList( rhs.m_ExtraGaugeBuffList ),
	m_fNeedGauge( rhs.m_fNeedGauge )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedExtraGauge = false;
}

ioTraceItem::~ioTraceItem()
{
}

void ioTraceItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

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

	//
	m_fDodgeForce = rkLoader.LoadFloat_e( "dodge_force", 0.0f );
	m_fDodgeFrictionRate = rkLoader.LoadFloat_e( "dodge_friction_rate", 0.0f );

	m_fDodgeAniRate = rkLoader.LoadFloat_e( "dodge_ani_rate", FLOAT1 );

	m_vDodgeAniList.clear();
	m_vDodgeAniList.reserve( 4 );

	rkLoader.LoadString_e( "dodge_animation_front", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_right", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_back", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_left", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );

	//
	m_bEnableExtraGauge = rkLoader.LoadBool_e( "extra_gauge_enable", false );

	m_ExtraGaugeBuffList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "extra_gauge_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "extra_gauge_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ExtraGaugeBuffList.push_back( szBuf );
	}

	//////////////////////////////////////////////////////////////////////////
	m_fNeedGauge = rkLoader.LoadFloat_e( "extra_need_gauge", FLOAT10 );
}

ioItem* ioTraceItem::Clone()
{
	return new ioTraceItem( *this );
}

ioWeaponItem::WeaponSubType ioTraceItem::GetSubType() const
{
	return WST_TRACE_ITEM;
}

void ioTraceItem::OnReleased( ioBaseChar *pOwner )
{
	RemoveExtraGaugeBuff( pOwner );
	//m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioTraceItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
	}
	else
	{
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
}

void ioTraceItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
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
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_EXTEND_ATTACK:
		break;
	}
}

void ioTraceItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	//	LOG.PrintTimeAndLog( 0, "Charge detected and process attack fire : start : %u, charge time : %u, cur time : %u",
	//			m_dwAttackStartTime, m_dwChargeTime, FRAMEGETTIME() );

	if( pOwner->IsAttackKeyDown() /*&& IsEnableGauge()*/ )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			//DecreaseBullet();
			ChangeToAttackFire( pOwner, true );
			g_TutorialMgr.ActionChargeAttack();
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioTraceItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	// kaseton
	//LOG.PrintTimeAndLog( 0, "---> Charged : %d, cur time: %u, cur combo : %d, cur time : %u", bCharged, dwCurTime, m_iCurCombo, FRAMEGETTIME() );

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	if( m_bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
			m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;

			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
			float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
			DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

			pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
		}

		if( iExtendCnt <= m_iCurCombo + 1 )
		{
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		}
		else
		{
			pOwner->ApplyNormalAttackResult( m_iCurCombo );
		}
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
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTraceItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	bool bCharged;
	rkPacket >> m_iCurCombo;
	rkPacket >> bCharged;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	ChangeToAttackFire( pOwner, bCharged );
}

void ioTraceItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioTraceItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioTraceItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

bool ioTraceItem::SetDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir = -vMoveDir;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
	}

	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DODGE_DEFENSE );
		kPacket << pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket << vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioTraceItem::ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	D3DXVECTOR3 vMoveDir;
	rkPacket >> vMoveDir;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
}

void ioTraceItem::CheckDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDodgeDefenseEnd > dwCurTime )
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
		}
	}
	else
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
	}
}

void ioTraceItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioTraceItem::ProcessCancel( ioBaseChar *pOwner )
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

			if( m_bCharged && pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetDashState( false );
				pOwner->SendDashState( false );

				return true;
			}
			else if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
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

bool ioTraceItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioTraceItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioTraceItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioTraceItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioTraceItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( !m_bEnableExtraGauge )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_CHARGE_CONTACT )
		{
			if( m_bUsedExtraGauge )
				fTypeRate = m_fExtraGaugeAttackDec;
			else
				fTypeRate = m_fExtraGaugeAttack;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_CHARGE_CONTACT )
		{
			if( m_bUsedExtraGauge )
				fTypeRate = m_fExtraGaugeAttackDefDec;
			else
				fTypeRate = m_fExtraGaugeAttackDef;
		}
		break;
	case EGT_WOUNDED:
		if( m_bUsedExtraGauge )
			fTypeRate = m_fExtraGaugeWoundDec;
		else
			fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		if( m_bUsedExtraGauge )
			fTypeRate = m_fExtraGaugeWoundDefDec;
		else
			fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	if( fTypeRate <= 0.0f )
		return;

	if( m_bUsedExtraGauge )
	{
		m_fCurExtraGauge -= fCurGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );

		if( m_fCurExtraGauge <= 0.0f )
			RemoveExtraGaugeBuff( pOwner );
	}
	else
	{
		m_fCurExtraGauge += fCurGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );

		if( m_fCurExtraGauge >= m_fMaxExtraGauge )
			SetExtraGaugeBuff( pOwner );
	}
}

void ioTraceItem::SetUseExtraGauge( float fAmt )
{
	if( !m_bUsedExtraGauge )
		return;

	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

int ioTraceItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioTraceItem::GetNeedBullet()
{
	return m_fNeedGauge;
}

int ioTraceItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}


bool ioTraceItem::IsEnableGauge( float fGague )
{
	if( m_fCurExtraGauge >= fGague )
		return true;

	return false;
}

void ioTraceItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioTraceItem::DecreaseBullet(float fGague )
{
	m_fCurExtraGauge = max( 0.f, m_fCurExtraGauge - fGague );
}


void ioTraceItem::DecreaseAllBullet()
{
	m_fCurExtraGauge = 0.f;
}

void ioTraceItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check bullet
	/*if( m_fMaxExtraGauge <= m_fCurExtraGauge )
		return;*/

	// check owner state
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		break;
	default:
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

	m_fCurExtraGauge += fGauge;
	m_fCurExtraGauge = (float)min( m_fCurExtraGauge, (float)m_fMaxExtraGauge );
}

void ioTraceItem::SetExtraGaugeBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_ExtraGaugeBuffList.empty() )
		return;

	int iBuffCnt = m_ExtraGaugeBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->AddNewBuff( m_ExtraGaugeBuffList[i], pOwner->GetCharName(), "", NULL );
	}

	m_bUsedExtraGauge = true;
}

void ioTraceItem::RemoveExtraGaugeBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_ExtraGaugeBuffList.empty() )
		return;

	int iBuffCnt = m_ExtraGaugeBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->RemoveBuff( m_ExtraGaugeBuffList[i] );
	}

	m_bUsedExtraGauge = false;
}
