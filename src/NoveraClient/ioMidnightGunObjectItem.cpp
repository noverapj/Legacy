
#include "stdafx.h"

#include "ioMidnightGunObjectItem.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMidnightGunObjectItem::ioMidnightGunObjectItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_iWeaponWay	= 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_dwCancelDashMotionEndTime = 0;

	m_bCharged = false;
}

ioMidnightGunObjectItem::ioMidnightGunObjectItem( const ioMidnightGunObjectItem &rhs )
	: ioObjectItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
	m_PreAttackGround( rhs.m_PreAttackGround ),
	m_PreAttackJump( rhs.m_PreAttackJump ),
	m_CancelDashAttribute( rhs.m_CancelDashAttribute ),
	m_PreBulletSetList( rhs.m_PreBulletSetList ),
	m_PreJumpBulletSetList( rhs.m_PreJumpBulletSetList ),
	m_ChargeBulletSetList( rhs.m_ChargeBulletSetList ),
	m_JumpBulletSetList( rhs.m_JumpBulletSetList ),
	m_fCancelDashEndJumpAmt( rhs.m_fCancelDashEndJumpAmt ),
	m_fCancelDashRandRate( rhs.m_fCancelDashRandRate )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_iWeaponWay	= 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_dwCancelDashMotionEndTime = 0;

	m_bCharged = false;
}

ioMidnightGunObjectItem::~ioMidnightGunObjectItem()
{
}

void ioMidnightGunObjectItem::LoadProperty( ioINILoader &rkLoader )
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

	wsprintf_e( szBuf, "gathering_max_attack" );
	LoadAttackAttribute( szBuf, m_ExtendMaxAttribute, rkLoader );

	LoadAttackAttribute_e( "pre_attack_ground", m_PreAttackGround, rkLoader );
	LoadAttackAttribute_e( "pre_attack_jump", m_PreAttackJump, rkLoader );

	LoadAttackAttribute_e( "cancel_dash", m_CancelDashAttribute, rkLoader );
	m_fCancelDashEndJumpAmt = rkLoader.LoadFloat_e( "cancel_dash_end_jump_amt", 0.0f );
	m_fCancelDashRandRate   = rkLoader.LoadFloat_e( "cancel_dash_rand_rate", FLOAT1 );

	LoadBulletProperty( rkLoader, "pre_attack_bullet", m_PreBulletSetList );
	LoadBulletProperty( rkLoader, "pre_attack_jump_bullet", m_PreJumpBulletSetList );
	LoadBulletProperty( rkLoader, "gathering_max_attack_bullet", m_ChargeBulletSetList );
	LoadBulletProperty( rkLoader, "extend_jump_attack_bullet", m_JumpBulletSetList );
}

ioItem* ioMidnightGunObjectItem::Clone()
{
	return new ioMidnightGunObjectItem( *this );
}

void ioMidnightGunObjectItem::LoadBulletProperty( ioINILoader &rkLoader, const ioHashString& szTitle, BulletInfoList& rkList )
{
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_max_cnt", szTitle.c_str() );
	int iMax = rkLoader.LoadInt( szKey, 0 );
	for( int i = 0; i < iMax; ++i )
	{
		BulletInfo kInfo;

		wsprintf_e( szKey, "%s%d_angle_h", szTitle.c_str(), i + 1 );
		kInfo.m_fYawAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s%d_angle_v", szTitle.c_str(), i + 1 );
		kInfo.m_fPitchAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s%d_range_offset", szTitle.c_str(), i + 1 );
		kInfo.m_fRangeOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s%d_height_offset", szTitle.c_str(), i + 1 );
		kInfo.m_fHeightOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		rkList.push_back( kInfo );
	}
}

ioObjectItem::ObjectSubType ioMidnightGunObjectItem::GetObjectSubType() const
{
	return OST_MIDNIGHT_GUN;
}

void ioMidnightGunObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

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

void ioMidnightGunObjectItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	/*case CS_GATHERING:
		OnGathering( pOwner );
		break;*/
	case CS_FULL_GATHERING:
		OnFullGathering( pOwner );
		break;
	/*case CS_COMBO:
		OnComboState( pOwner );
		break;*/
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

				CheckCancelReserveByConditionalSpecialDash( pOwner );
				//Enable GaugeCheck
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioMidnightGunObjectItem::OnCharging( ioBaseChar *pOwner )
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

void ioMidnightGunObjectItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
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
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni		= pAttribute->m_AttackAnimation;
		m_fCurAniRate	= pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime	= dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMidnightGunObjectItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case GS_CANCEL_DASH:
		{
			D3DXVECTOR3	position;
			rkPacket >> position;
			int iCurDir, iCallerType;
			rkPacket >> iCurDir >> iCallerType;

			pOwner->SetWorldPosition( position );

			SetChangeConditionalSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( iCurDir ), iCallerType );
		}
		break;

	case GS_JUMP_CHARGE_ATTACK:
		{
			D3DXVECTOR3	position;
			rkPacket >> position;
			pOwner->SetWorldPosition( position );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToJumpChargeAttack( pOwner );
		}
		break;
	}
}

void ioMidnightGunObjectItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case CS_FULL_GATHERING:
		{
			rkPacket >> m_iCurCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToFullGatherState( pOwner );
		}
		break;
	
	case CS_ATTACK_FIRE:
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

	case CS_END:
		{
			rkPacket >> m_iCurCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
			if( pMidnightItem )
				pMidnightItem->OnReleaseObject( pOwner );
		}
	}
}

void ioMidnightGunObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

	ioMissileWeapon* pMissile = ToMissileWeapon( pWeapon );	
	if( pMissile )
	{
		ioBaseChar* pOwner = pMissile->GetOwner();
		if( pOwner )
		{
			BulletInfoList* pInfo	= NULL;
			CharState eState	= pOwner->GetState();
			if( eState == CS_MIDNIGHT_GUN )
			{
				ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
				if( m_GunState == GS_PRE_ATTACK && pMidnightItem && pMidnightItem->GetObjectEquipState() == ioMidnightItem::OES_JUMP )
				{
					pInfo = &m_PreJumpBulletSetList;
				}
				else if( m_GunState == GS_JUMP_CHARGE_ATTACK )
				{
					pInfo = &m_JumpBulletSetList;
				}
			}
			else if( m_ChargeState == CS_FULL_GATHERING )
			{
				pInfo = &m_ChargeBulletSetList;
			}

			if( pInfo && COMPARE( m_iWeaponWay, 0, (int)pInfo->size() ) )
			{
				float fCurYawAngle		= 0.0f;
				float fCurPitchAngle	= 0.0f;

				fCurYawAngle	= pInfo->at( m_iWeaponWay ).m_fYawAngle;
				fCurPitchAngle	= pInfo->at( m_iWeaponWay ).m_fPitchAngle;

				fCurYawAngle	= ioMath::ArrangeHead( fCurYawAngle );
				fCurPitchAngle	= ioMath::ArrangeHead( fCurPitchAngle );

				D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
				D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

				D3DXQUATERNION qtRot;
				D3DXVECTOR3 vAxis;
				D3DXVec3Cross( &vAxis, &vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

				// vertical
				D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fCurYawAngle) );
				vMoveDir = qtRot * vMoveDir;

				// horizon
				D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fCurPitchAngle) );
				vMoveDir = qtRot * vMoveDir;	
				pWeapon->SetMoveDir( vMoveDir );
				m_iWeaponWay++;
			}
		}
	}
}

bool ioMidnightGunObjectItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioMidnightGunObjectItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !pOwner ) 
		return	false;

	//if( !IsChargeDash() ) return false;
	//if( !m_bCharged ) return false;

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

void ioMidnightGunObjectItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioMidnightGunObjectItem::ProcessCancel( ioBaseChar *pOwner )
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

			if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
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

bool ioMidnightGunObjectItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectJinMoRiItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectJinMoRiItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioMidnightGunObjectItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

bool ioMidnightGunObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioMidnightGunObjectItem::IsEnableDash() const
{
	return true;
}

bool ioMidnightGunObjectItem::IsEnableDefense() const
{
	return true;
}

void ioMidnightGunObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	ioItem::OnEquiped( pOwner );

	m_iWeaponWay	= 0;

	ChangeToPreAttack( pOwner );
}

/*void ioMidnightGunObjectItem::OnReleased( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if ( fHeightGap >= FLOAT1 )
	{
		pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, true, false, true );
		SetJumpState();
		//ioItem::OnReleased( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );

		ioAttackableItem::OnReleased( pOwner );
	}
}*/

void ioMidnightGunObjectItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
}

void ioMidnightGunObjectItem::ChangeToFullGatherState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_iWeaponWay	= 0;

	pOwner->SetAttackMoveEnable( false );

	m_ChargeState = CS_FULL_GATHERING;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_ExtendMaxAttribute.m_AttackAnimation );
	float fTimeRate  = m_ExtendMaxAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_ExtendMaxAttribute.m_dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, iAniID, m_ExtendMaxAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMidnightGunObjectItem::ChangeEnd( ioBaseChar *pOwner )
{
	m_ChargeState = CS_END;

	ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
	if( !pMidnightItem )
		return;

	pMidnightItem->ReleaseObject( pOwner );

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMidnightGunObjectItem::OnFullGathering( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
		ChangeEnd( pOwner );
}

void ioMidnightGunObjectItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
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

void ioMidnightGunObjectItem::OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
	if( !pMidnightItem )
		return;

	if( pMidnightItem->GetCurExtraGauge() <= 0.0f )
	{
		pMidnightItem->ReleaseObject( pOwner );
		//pMidnightItem->ChangeToClawAttack( pOwner );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCancelDashMotionEndTime < dwCurTime && m_GunState == GS_CANCEL_DASH )
		m_GunState = GS_NONE;
}

bool ioMidnightGunObjectItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	switch( m_GunState )
	{
	case GS_CANCEL_DASH:
		return false;
	}

	if( pOwner->GetState() == CS_ATTACK )
		return true;

	return false;
}

void ioMidnightGunObjectItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CancelDashAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_CancelDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CancelDashAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_MIDNIGHT_GUN );

	m_GunState = GS_CANCEL_DASH;
	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = m_CancelDashAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CancelDashAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CancelDashAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwCancelDashMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<int>( m_GunState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioMidnightGunObjectItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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

void ioMidnightGunObjectItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
{
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
				if ( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
					iCurCombo = 0;
				pOwner->SetNormalAttack( iCurCombo );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
	m_GunState	= GS_NONE;
}

void ioMidnightGunObjectItem::ProcessCancelDashState( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCancelDashMotionEndTime < dwCurTime )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
		{
			pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, false, true, false, true );

			m_GunState	= GS_NONE;
			//pOwner->SetState( CS_DELAY );
		}
		else
			ChangeReserveKeyInputState( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioMidnightGunObjectItem::ChangeToPreAttack( ioBaseChar* pOwner )
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

	m_iWeaponWay	= 0;
	DWORD dwCurTime = FRAMEGETTIME();
	//m_ChargeState = CS_PRE_ATTACK;
	pOwner->SetState( CS_MIDNIGHT_GUN );
	m_GunState = GS_PRE_ATTACK;

	InitAniRotate();
	pOwner->NormalAttackOver( false );
	pOwner->SetAutoTarget( ATT_NORMAL );

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

	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, 1.0f );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	SetAniRotate( pOwner, iAniID, fTimeRate );

	pOwner->IncreaseWeaponIndexBase();
}

void ioMidnightGunObjectItem::OnPreAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( m_dwPreAttackEndTime < FRAMEGETTIME() )
	{
		m_ChargeState = CS_NONE;
		m_GunState = GS_NONE;
		pOwner->SetCurMoveSpeed( 0.0f );

		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
		{
			pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, true, false, true );
			SetJumpState();
		}
		else
			pOwner->SetState( CS_DELAY );
	}
}

void ioMidnightGunObjectItem::OnProcessGunState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	switch( m_GunState )
	{
	case GS_PRE_ATTACK:
		{
			OnPreAttack( pOwner );
		}
		break;

	case GS_CANCEL_DASH:
		{
			ProcessCancelDashState( pOwner );
		}
		break;
		
	case GS_JUMP_CHARGE_ATTACK:
		{
			ProcessJumpChargeAttack( pOwner );
		}
		break;
	}
}

bool ioMidnightGunObjectItem::IsCollisionSkipState()
{
	if( m_GunState == GS_CANCEL_DASH )
		return	true;

	return	false;
}

void ioMidnightGunObjectItem::ChangeToJumpChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTEND_JUMP_ATTACK );
	if( !pAttr ) return;

	m_iWeaponWay	= 0;

	pOwner->SetState( CS_MIDNIGHT_GUN );
	m_GunState = GS_JUMP_CHARGE_ATTACK;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int	iAniID	     = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float	fAniRate  = pAttr->m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = pAttr->m_dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fAniRate,
		dwPreDelay );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	SetAirJump( pOwner, iAniID, fAniRate );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<int>( m_GunState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMidnightGunObjectItem::ProcessJumpChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		ChangeEnd( pOwner );

		ioMidnightItem* pMidnightItem	= ToMidnightItem( pOwner->GetWeapon() );
		if( pMidnightItem )
			pMidnightItem->SetObjectReleseAutoJump( true );
	}
}

bool ioMidnightGunObjectItem::IsJumpChargeAttackState( ioBaseChar* pOwner )
{
	if( m_GunState == GS_JUMP_CHARGE_ATTACK /*|| m_GunState == GS_PRE_ATTACK*/ )
		return true;

	ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
	if( m_GunState == GS_PRE_ATTACK && pMidnightItem && pMidnightItem->GetObjectEquipState() == ioMidnightItem::OES_JUMP )
		return true;

	return false;
}

void ioMidnightGunObjectItem::FillMidnightScytheInfo( ioBaseChar* pOwner, SP2Packet& rkPacket )
{

}

void ioMidnightGunObjectItem::SetMidngihtScytheInfo( ioBaseChar* pOwner, SP2Packet& rkPacket )
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioMidnightGunState::ioMidnightGunState()
{
}

ioMidnightGunState::~ioMidnightGunState()
{
}

void	ioMidnightGunState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void	ioMidnightGunState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}
bool	ioMidnightGunState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioMidnightGunObjectItem*	pMidnightObject	= ToObjectMidnightGunItem( pOwner->GetWeapon() );
	if( pMidnightObject )
		return	pMidnightObject->IsCollisionSkipState();

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void	ioMidnightGunState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMidnightGunObjectItem* pMidnightObject = ToObjectMidnightGunItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMidnightObject )
	{
		pMidnightObject->OnProcessGunState( pOwner );
	}
}

void	ioMidnightGunState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool	ioMidnightGunState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioMidnightGunObjectItem* pMidnightObject = ToObjectMidnightGunItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMidnightObject )
	{
		if( pMidnightObject->IsJumpChargeAttackState( pOwner ) )
			return true;
	}

	return false;
}
