
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioGhostStealer.h"

#include "ioChargeComboJump3.h"

#include "WeaponDefine.h"

ioGhostStealer::ioGhostStealer()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_fCurCoolTime = 0.0f;

	m_bSetUroborusSkill = false;
}

ioGhostStealer::ioGhostStealer( const ioGhostStealer &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_szRetreatFr( rhs.m_szRetreatFr ),
	m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
	m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
	m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
	m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
	m_dwNeedCoolTime( rhs.m_dwNeedCoolTime ),
	m_fRecoveryTime( rhs.m_fRecoveryTime ),
	m_AttackInfo( rhs.m_AttackInfo ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	//m_FakeAni( rhs.m_FakeAni ),
	m_FakeAniList( rhs.m_FakeAniList ),
	//m_fFakeAniRate( rhs.m_fFakeAniRate )
	m_fFakeAniRateList( rhs.m_fFakeAniRateList ),
	m_dwFakeAttackCnt( rhs.m_dwFakeAttackCnt )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_fCurCoolTime = 0.0f;

	m_bSetUroborusSkill = false;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_dwFullAnimationTime = 0;
}

ioGhostStealer::~ioGhostStealer()
{
}

void ioGhostStealer::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_AttackInfo.Init();
	//
	LoadAttackAttribute_e( "add_extend_attack", m_AttackInfo.m_AddExtendAttackAttribute, rkLoader );

	rkLoader.LoadString_e( "ghoststealer_ani", "", szBuf, MAX_PATH );
	m_AttackInfo.m_Ani = szBuf;

	m_AttackInfo.m_fAniRate = rkLoader.LoadFloat_e( "ghoststealer_ani_rate", FLOAT1 );

	m_AttackInfo.m_fMoveSpeed = rkLoader.LoadFloat_e( "ghoststealer_move_speed", 0.0f );
	m_AttackInfo.m_fMoveRate = rkLoader.LoadFloat_e( "ghoststealer_move_rate", 0.0f );
	m_AttackInfo.m_fMoveAngle = rkLoader.LoadFloat_e( "ghoststealer_move_angle", 0.0f );

	m_AttackInfo.m_fEndJumpAmt = rkLoader.LoadFloat_e( "ghoststealer_end_jump_amt", 0.0f );
	m_AttackInfo.m_fCurAngle = rkLoader.LoadFloat_e( "ghoststealer_land_angle", 45.f );

	m_AttackInfo.m_fMoveLength = rkLoader.LoadFloat_e( "ghoststealer_move_length", 0.f );

	m_AttackInfo.m_fMoveHeightRate = rkLoader.LoadFloat_e( "ghoststealer_move_height_rate", FLOAT1 );
	//
	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );

	//
	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_dwNeedCoolTime = (DWORD)rkLoader.LoadInt_e( "use_cool_time", 0 );
	m_fRecoveryTime = rkLoader.LoadFloat_e( "recover_cool_time", 0.0f );

	//
	m_dwFakeAttackCnt = rkLoader.LoadInt_e( "fake_ani_cnt", 0 );
	for( i = 0; i < (int)m_dwFakeAttackCnt; i++ )
	{
		wsprintf_e( szKey, "fake_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_FakeAniList.push_back( szBuf );

		wsprintf_e( szKey, "fake_ani%d_rate", i+1 );
		float fRateValue = rkLoader.LoadFloat( szBuf, FLOAT1 );
		m_fFakeAniRateList.push_back( fRateValue );
	}
	
	//m_FakeAni = szBuf;
	//m_fFakeAniRate = rkLoader.LoadFloat_e( "fake_ani_rate", FLOAT1 );
}

ioItem* ioGhostStealer::Clone()
{
	return new ioGhostStealer( *this );
}

ioWeaponItem::WeaponSubType ioGhostStealer::GetSubType() const
{
	return WST_GHOST_STEALER;
}

void ioGhostStealer::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwKeyReserveTime = 0;

	m_bSetUroborusSkill = false;

	ClearCancelInfo();

	//LOG.PrintTimeAndLog( 0, "NormalAttack%d", iCurCombo );

	int iExtendCnt = m_vExtendAttributeList.size();
	if( iExtendCnt == 0 || !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE_CHARGE_NORAML;
		m_iCurCombo = iCurCombo;

		pOwner->SetChargingState( false );

		const AttackAttribute *pAttr = GetAttackAttribute( AT_NORMAL, iCurCombo );
		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );

		m_dwKeyReserveTime = 0;
		float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * pAttr->m_fAttackAniRate;
		if( fReserve > 0.0f )
			m_dwKeyReserveTime = pAttr->m_dwPreDelay + dwCurTime + fReserve;
		m_dwFullAnimationTime = dwCurTime + (pGrp->GetAnimationFullTime( iAniID ) * pAttr->m_fAttackAniRate + pAttr->m_dwPreDelay);
	}
	else
	{
		int n = pOwner->GetState();

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
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioGhostStealer::CheckExtendAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
}

void ioGhostStealer::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		OnAttackFire( pOwner );
		break;
	case CS_ADD_EXTEND_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		OnAddExtendAttackFire( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;

			if( m_dwKeyReserveTime >0 &&
				m_dwKeyReserveTime < dwCurTime &&
				pOwner->IsDirKeyDoubleClick() )
			{
				D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
				//ChangeToRetreatState( pOwner, m_FakeAni, m_fFakeAniRate, vLook );
				ChangeToRetreatState( pOwner, m_FakeAniList[m_iCurCombo], m_fFakeAniRateList[m_iCurCombo], vLook );
			}
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_NONE_CHARGE_NORAML:
		if( m_dwKeyReserveTime >0 &&
				m_dwKeyReserveTime < dwCurTime &&
				pOwner->IsDirKeyDoubleClick() )
			{
				D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
				//ChangeToRetreatState( pOwner, m_FakeAni, m_fFakeAniRate, vLook );
				ChangeToRetreatState( pOwner, m_FakeAniList[m_iCurCombo], m_fFakeAniRateList[m_iCurCombo], vLook );
			}
		break;
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	}
}

bool ioGhostStealer::IsEnableFakeAttackState()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( (m_ChargeState == CS_NORMAL_ATTACK || m_ChargeState == CS_NONE_CHARGE_NORAML) &&
		m_dwKeyReserveTime >0 &&
		m_dwKeyReserveTime < dwCurTime && (int)m_dwFakeAttackCnt > m_iCurCombo )
		return true;

	return false;
}

void ioGhostStealer::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && m_fCurCoolTime >= m_dwNeedCoolTime)
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAttackFire( pOwner, true );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioGhostStealer::OnAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFullAnimationTime < dwCurTime )
	{
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		/*if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}*/

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
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	if( m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );

		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
		{
			ChangeToAddExtandAttackFire( pOwner );
			return;
		}
		else
		if( pOwner->IsNeedProcess() && pOwner->IsJumpKeyDown() )
		{
			D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition();

			D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vLook, &ioMath::UNIT_Y );
			D3DXQUATERNION qtNewRot;
			D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(m_AttackInfo.m_fCurAngle) );
			vLook = qtNewRot * vLook;
			D3DXVec3Normalize( &vLook, &vLook );
			vLook = vLook * m_AttackInfo.m_fMoveLength;
			vTargetPos = vTargetPos + vLook;

			pOwner->SetGhostStealerState( m_AttackInfo.m_Ani, 
				m_AttackInfo.m_fAniRate, 
				vTargetPos, 
				m_AttackInfo.m_fMoveSpeed, 
				m_AttackInfo.m_fMoveRate, 
				m_AttackInfo.m_fEndJumpAmt, m_AttackInfo.m_fMoveHeightRate );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << UC_SET;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			return;
		}
		else if( pOwner->IsNeedProcess() && pOwner->IsDirKeyDoubleClick() )
		{
			D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
			//ChangeToRetreatState( pOwner, m_FakeAni, m_fFakeAniRate, vLook );
			ChangeToRetreatState( pOwner, m_FakeAniList[m_iCurCombo], m_fFakeAniRateList[m_iCurCombo], vLook );
		}
	}
}

void ioGhostStealer::OnAddExtendAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFullAnimationTime < dwCurTime )
	{
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
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
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	if( m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

void ioGhostStealer::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	m_dwFireStartTime = 0;

	pOwner->SetChargingState( false );


	m_dwCancelCheckTime = dwCurTime;
	m_dwInputCancelCheckTime = dwCurTime;

	if( bCharged )
	{
		DecreaseCoolTime();

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

			m_dwKeyReserveTime = 0;
			float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
			if( fReserve > 0.0f )
				m_dwKeyReserveTime = dwPreDelay + dwCurTime + fReserve;
			m_dwFullAnimationTime = dwCurTime + (pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay);

			m_ChargeState = CS_ATTACK_FIRE;
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

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID	     = pGrp->GetAnimationIdx( m_szCurAni );
			float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fCurAniRate;
			if( fReserve > 0.0f )
				m_dwKeyReserveTime = pAttribute->m_dwPreDelay + dwCurTime + fReserve;
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_NORMAL_ATTACK;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGhostStealer::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		{
			pOwner->SetChargingState( false );
			m_ChargeState = CS_NONE;
		}
		break;
	case CS_ATTACK_FIRE:
		ChangeToAttackFire( pOwner, true );
		break;
	case CS_NORMAL_ATTACK:
		ChangeToAttackFire( pOwner, false );
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

void ioGhostStealer::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case UC_SET:
		{
			D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition();

			D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vLook, &ioMath::UNIT_Y );
			D3DXQUATERNION qtNewRot;
			D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(m_AttackInfo.m_fCurAngle) );
			vLook = qtNewRot * vLook;
			D3DXVec3Normalize( &vLook, &vLook );
			vLook = vLook * m_AttackInfo.m_fMoveLength;
			vTargetPos = vTargetPos + vLook;

			pOwner->SetGhostStealerState( m_AttackInfo.m_Ani, 
				m_AttackInfo.m_fAniRate, 
				vTargetPos, 
				m_AttackInfo.m_fMoveSpeed, 
				m_AttackInfo.m_fMoveRate, 
				m_AttackInfo.m_fEndJumpAmt, m_AttackInfo.m_fMoveHeightRate );
		}
		break;
	case UC_ADD_SET:
		{
			D3DXQUATERNION qtTargetRot;
			rkPacket >> qtTargetRot;
			pOwner->SetTargetRot( qtTargetRot );
			ChangeToAddExtandAttackFire( pOwner );
			break;
		}
	}
}

void ioGhostStealer::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioGhostStealer::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_ATTACK_FIRE:
			return true;
		}
	}

	return false;
}

bool ioGhostStealer::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	//if( !IsChargeDash() ) return false;
	if( !bCheck ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		/*switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		case CS_COMBO:
			return true;
		}*/
		if( IsEnableFakeAttackState() )
			return true;
	}

	return false;
}

bool ioGhostStealer::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	return false;
}

void ioGhostStealer::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetChargingState( false );
}

void ioGhostStealer::GetAutoTargetValue( ioBaseChar *pOwner,
	float &fRange, float &fMinAngle, float &fMaxAngle,
	AutoTargetType eType )
{
	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;
}

void ioGhostStealer::OnRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int nCombo = (m_iCurCombo+1) % GetMaxCombo();

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

			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( nCombo );
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
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( nCombo );
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

void ioGhostStealer::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	m_ChargeState = CS_RETREAT;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwRetreatEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	m_dwRetreatForceTime = dwCurTime + dwTime;

	m_vForceDir = vDir;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << szAni;
		kPacket << fCurRate;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioGhostStealer::CheckRetreat( ioBaseChar *pOwner, bool bFront )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

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

void ioGhostStealer::CheckReserve( ioBaseChar *pOwner )
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

void ioGhostStealer::ClearState()
{
	m_ChargeState = CS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

bool ioGhostStealer::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioGhostStealer::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioGhostStealer::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioGhostStealer::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

int ioGhostStealer::GetMaxBullet()
{
	return (int)m_dwMaxCoolTime;
}

int ioGhostStealer::GetNeedBullet()
{
	return (int)m_dwNeedCoolTime;
}

int ioGhostStealer::GetCurBullet()
{
	DWORD dwCurCoolTime = m_fCurCoolTime;
	dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

	return (int)dwCurCoolTime;
}

void ioGhostStealer::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	DWORD dwCurBullet = iCurBullet;
	dwCurBullet = max( 0, min( dwCurBullet, m_dwMaxCoolTime ) );

	m_fCurCoolTime = (float)dwCurBullet;
}

void ioGhostStealer::OnReleased( ioBaseChar *pOwner )
{
	InitCoolTime();

	ioAttackableItem::OnReleased( pOwner );
}

void ioGhostStealer::InitCoolTime()
{
	m_fCurCoolTime = 0.0f;
}

void ioGhostStealer::MaxCoolTime()
{
	m_fCurCoolTime = (float)m_dwMaxCoolTime;
}

void ioGhostStealer::DecreaseCoolTime()
{
	if( m_fCurCoolTime < (float)m_dwNeedCoolTime )
	{
		m_fCurCoolTime = 0.0f;
	}
	else
	{
		m_fCurCoolTime -= (float)m_dwNeedCoolTime;
	}
}

void ioGhostStealer::UpdateExtraData( ioBaseChar *pOwner )
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

void ioGhostStealer::ChangeToAddExtandAttackFire( ioBaseChar *pOwner )
{
	pOwner->InitActionStopDelay( true );

	ClearState();

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_AttackInfo.m_AddExtendAttackAttribute.m_AttackAnimation );
	float fTimeRate = m_AttackInfo.m_AddExtendAttackAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_AttackInfo.m_AddExtendAttackAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( 30, true );
	pGrp->ClearAllActionAni( 30.0f, true );

	if( m_AttackInfo.m_AddExtendAttackAttribute.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, 30.0f, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, 30.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

		pGrp->SetActionAni( m_AttackInfo.m_AddExtendAttackAttribute.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/m_AttackInfo.m_AddExtendAttackAttribute.m_fAttackEndAniRate,
			fWaitTime, 0.0f, true );
	}
	float fCurRate = m_AttackInfo.m_AddExtendAttackAttribute.m_fAttackAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;
	pOwner->RefreshFireTimeList( iAniID, m_AttackInfo.m_AddExtendAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, dwPreDelay );

	m_dwFullAnimationTime = dwCurTime + (pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay);

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_AttackInfo.m_AddExtendAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	float fFire = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStartTime = dwPreDelay + dwCurTime + fFire;

	m_dwKeyReserveTime = 0;
	float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserve > 0.0f )
		m_dwKeyReserveTime = dwPreDelay + dwCurTime + fReserve;

	m_szCurAni = m_AttackInfo.m_AddExtendAttackAttribute.m_AttackAnimation;
	m_fCurAniRate = fTimeRate;

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	m_ChargeState = CS_ADD_EXTEND_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << UC_ADD_SET;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGhostStealer::IncreaseChangeDamage( float fDamage, int iType, bool bBuff )
{
	if( iType == SSGT_GHOST_STEALER || iType == SSGT_NONE )
	{
		m_fCurCoolTime += fDamage;
		m_fCurCoolTime = max( 0.0f, m_fCurCoolTime );
		m_fCurCoolTime = min( m_fCurCoolTime, (float)m_dwMaxCoolTime );
	}
}

bool ioGhostStealer::IsFakeAttackTiming( ioBaseChar *pOwner )
{
	if( m_ChargeState == CS_RETREAT )
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioGhostStealerSpecialState::ioGhostStealerSpecialState()
{
}

ioGhostStealerSpecialState::~ioGhostStealerSpecialState()
{
}

void ioGhostStealerSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioGhostStealerSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioGhostStealerSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	if( m_fGhostStealerMaxRange < m_fGhostStealerCurRange )
	{
		pOwner->SetSKillEndJumpState( m_fGhostStealerEndJumpAmt, false, true, false );

		if( pOwner->IsAttackKeyDown() )
		{
			ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
			if( pItem )
			{
				ioChargeComboJump3 *pChargeComboJump3 = ToChargeComboJump3( pItem->GetCurExtendJump() );
				if( pChargeComboJump3 )
					pChargeComboJump3->CheckComboJumpAttack( pOwner );
			}
			return;
		}
	}
}

void ioGhostStealerSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioGhostStealerSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		float fFinalMoveAmt = m_fGhostStealerMoveSpeed * fDivideTimePerSec;
		if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
		{
			vMoveAmt = m_GhostStealerDir * fFinalMoveAmt;
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		float fCurLength = D3DXVec3Length( &vTotalMove );
		m_fGhostStealerCurRange += fCurLength;

		float fAddSineValue = max( 0.f , Help::GetHalfSineValue( m_fGhostStealerMaxRange, m_fGhostStealerCurRange ) );
		float fCap = fAddSineValue - m_fPreGhostStealerAddSineValue;

		m_fPreGhostStealerAddSineValue = fAddSineValue;

		//LOG.PrintTimeAndLog( 0, "%.2f   ---  %.2f", fCap, fAddSineValue );

		vTotalMove += D3DXVECTOR3( 0.f, fCap * m_fGhostStealerVolumeRate, 0.f );

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,	vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}

		if( bCol )
		{
			pOwner->SetSKillEndJumpState( 0.1f, false, true, false );
			return true;
		}
	}

	if( pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	return true;
}

void ioGhostStealerSpecialState::SetGhostStealerInfo( D3DXVECTOR3 vMoveDir, float fMoveSpeed, float fMoveRange, float fEndJumpAmt, float fVolumeRate )
{
	m_GhostStealerDir = vMoveDir;
	m_fGhostStealerMoveSpeed = fMoveSpeed;
	m_fGhostStealerMaxRange = fMoveRange;
	m_fGhostStealerEndJumpAmt = fEndJumpAmt;
	m_fGhostStealerVolumeRate = fVolumeRate;

	m_fPreGhostStealerAddSineValue = 0.0f;
	m_fGhostStealerCurRange = 0.0f;
}