
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMutantItem.h"

#include "ioChargeComboJump2.h"

#include "WeaponDefine.h"

ioMutantItem::ioMutantItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_fCurCoolTime = 0.0f;

	m_bSetUroborusSkill = false;
	m_MsState = MSS_NONE;
	m_SpecialMovePreDelayWeaponIndex = 0;
	m_dwContactWeaponIndex = 0;
}

ioMutantItem::ioMutantItem( const ioMutantItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
	m_dwNeedCoolTime( rhs.m_dwNeedCoolTime ),
	m_fRecoveryTime( rhs.m_fRecoveryTime ),
	m_AttackInfo( rhs.m_AttackInfo ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_fSpecialMovePreDelayAttackAngle( rhs.m_fSpecialMovePreDelayAttackAngle ),
	m_SpecialMovePreDelayAttack( rhs.m_SpecialMovePreDelayAttack ),
	m_fSpecialMoveSpeed( rhs.m_fSpecialMoveSpeed ),
	m_fSpecialMoveMaxRange( rhs.m_fSpecialMoveMaxRange ),
	m_fSpecialMoveVolumRate( rhs.m_fSpecialMoveVolumRate ),
	m_dwSpecialMoveRotateTime( rhs.m_dwSpecialMoveRotateTime ),
	m_vSpecialMovejumpInfo( rhs.m_vSpecialMovejumpInfo ),
	m_SpecialMove_MoveAttack( rhs.m_SpecialMove_MoveAttack ),
	m_fArrowWeaponAngle( rhs.m_fArrowWeaponAngle )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_fCurCoolTime = 0.0f;

	m_bSetUroborusSkill = false;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_dwFullAnimationTime = 0;

	m_MsState = MSS_NONE;
	m_SpecialMovePreDelayWeaponIndex = 0;
	m_dwContactWeaponIndex = 0;
}

ioMutantItem::~ioMutantItem()
{
}

void ioMutantItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iExtendCnt = rkLoader.LoadInt( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_AttackInfo.Init();
	//
	LoadAttackAttribute( "add_extend_attack", m_AttackInfo.m_AddExtendAttackAttribute, rkLoader );

	rkLoader.LoadString( "ghoststealer_ani", "", szBuf, MAX_PATH );
	m_AttackInfo.m_Ani = szBuf;

	m_AttackInfo.m_fAniRate = rkLoader.LoadFloat( "ghoststealer_ani_rate", FLOAT1 );

	m_AttackInfo.m_fMoveSpeed = rkLoader.LoadFloat( "ghoststealer_move_speed", 0.0f );
	m_AttackInfo.m_fMoveRate = rkLoader.LoadFloat( "ghoststealer_move_rate", 0.0f );
	m_AttackInfo.m_fMoveAngle = rkLoader.LoadFloat( "ghoststealer_move_angle", 0.0f );

	m_AttackInfo.m_fEndJumpAmt = rkLoader.LoadFloat( "ghoststealer_end_jump_amt", 0.0f );
	m_AttackInfo.m_fCurAngle = rkLoader.LoadFloat( "ghoststealer_land_angle", 45.f );

	m_AttackInfo.m_fMoveLength = rkLoader.LoadFloat( "ghoststealer_move_length", 0.f );

	m_AttackInfo.m_fMoveHeightRate = rkLoader.LoadFloat( "ghoststealer_move_height_rate", FLOAT1 );
	
	//
	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt( "max_cool_time", 0 );
	m_dwNeedCoolTime = (DWORD)rkLoader.LoadInt( "use_cool_time", 0 );
	m_fRecoveryTime = rkLoader.LoadFloat_e( "recover_cool_time", 0.0f );

	LoadAttackAttribute( "mutant_special_predelay", m_SpecialMovePreDelayAttack, rkLoader );
	m_fSpecialMovePreDelayAttackAngle = rkLoader.LoadFloat_e( "mutant_special_predelay_attack_angle", 0.f );

	m_fSpecialMoveSpeed = rkLoader.LoadFloat_e( "mutant_special_move_speed", FLOAT0001 );
	m_fSpecialMoveMaxRange = rkLoader.LoadFloat_e( "mutant_special_move_max_range", 0.f );
	m_fSpecialMoveVolumRate = rkLoader.LoadFloat_e( "mutant_special_move_volume_rate", 0.f );

	m_dwSpecialMoveRotateTime = (DWORD)rkLoader.LoadInt_e( "mutant_special_move_rotate_time", 100000 );

	//중간에 점프 정보
	
	int nCnt = rkLoader.LoadInt( "mutant_jump_cnt", 0 );
	for( int i = 0; i < nCnt; i++ )
	{
		wsprintf( szBuf, "mutant_jump_range%d", i+1 );
		MutantSpecialJumpPowerInfo Info;
		Info.fRange = rkLoader.LoadFloat( szBuf, 0.f );
		wsprintf( szBuf, "mutant_jump_power%d", i+1 );
		Info.fJumpPower = rkLoader.LoadFloat( szBuf, 0.f );

		wsprintf( szBuf, "mutant_jump_ani%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		Info.stJumpAni = szBuf;

		wsprintf( szBuf, "mutant_jump_ani_rate%d", i+1 );
		Info.fJumpAniRate = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vSpecialMovejumpInfo.push_back( Info );
	}

	LoadAttackAttribute( "mutant_special_move_attack", m_SpecialMove_MoveAttack, rkLoader );

	m_fArrowWeaponAngle = rkLoader.LoadFloat( "mutant_jump_arrow_weapon_angle", 0.f );
}

ioItem* ioMutantItem::Clone()
{
	return new ioMutantItem( *this );
}

ioWeaponItem::WeaponSubType ioMutantItem::GetSubType() const
{
	return WST_MUTANT_ITEM;
}


float ioMutantItem::GetSpecialMoveMaxRange()
{
	return m_fSpecialMoveMaxRange;
}


float ioMutantItem::GetSpecialMoveSpeed()
{
	return m_fSpecialMoveSpeed;
}


float ioMutantItem::GetSpecialMoveVolumeRate()
{
	return m_fSpecialMoveVolumRate;
}


void ioMutantItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioMutantItem::CheckNormalAttack( ioBaseChar *pOwner )
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
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}


void ioMutantItem::OnCharging( ioBaseChar *pOwner )
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

void ioMutantItem::OnAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFullAnimationTime < dwCurTime )
	{
		pOwner->NormalAttackOver();

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

		/*if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
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
		}*/
	}
}

void ioMutantItem::OnAddExtendAttackFire( ioBaseChar *pOwner )
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

void ioMutantItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
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
		//DecreaseCoolTime();

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

void ioMutantItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	}
}

void ioMutantItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;
	switch( iSyncType )
	{
	case SST_JUMP:
		{
			float fMoveRange;
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> fMoveRange;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRot );

			SetSpecialEndJump( pOwner, fMoveRange );
		}
		break;
	case SST_ROTATE:
		{
			bool bChangeDir;
			rkPacket >> bChangeDir;

			D3DXQUATERNION qtRot;
			D3DXVECTOR3	vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			int iDir;
			rkPacket >> iDir;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			if( bChangeDir )
			{
				m_PreDirKey = m_CurDirKey;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

				/*if( !bOptionMove )
					ChangeFlyMove( pOwner );*/
			}
		}
		break;
	}
}


void ioMutantItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

	ioRunContactWeapon	*pRunContactWeapon = ToRunContactWeapon( pWeapon );
	ioBaseChar* pOwner = NULL;
	if( pRunContactWeapon )
		pOwner = pRunContactWeapon->GetOwner();

	if( pRunContactWeapon && pOwner && pOwner->GetState() == CS_MUTANT_SPECIAL_STATE )
	{
		m_dwContactWeaponIndex = pRunContactWeapon->GetWeaponIndex();
		pWeapon->SetLiveTime( 10000 );
		pWeapon->SetCollisionCancel( true );
	}

	ioUroborusWeapon4 *pUroborus4 = ToUroborusWeapon4( pWeapon );
	if( pUroborus4 && pUroborus4->IsMutanteWeaponMode() )
	{
		m_SpecialMovePreDelayWeaponIndex = pWeapon->GetWeaponIndex();

		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &ioMath::UNIT_Y );
			D3DXVec3Normalize( &vAxis, &vAxis );

			D3DXQUATERNION qtNewRot;
			float fCurAngle = ioMath::ArrangeHead( m_fSpecialMovePreDelayAttackAngle );
			D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

			vDir = qtNewRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );

			pUroborus4->SetMoveDir( vDir );
		}
	}

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );

	if( pArrow )
	{
		pOwner = pArrow->GetOwner();
		if( pOwner )
		{
			int n = pArrow->GetWeaponItemType();
			D3DXVECTOR3 vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vAimedDir, &vAimedDir );

			if( n == (int)(ioArrowWeapon::WIT_MUTANT)&& bJumpAttack )
			{
				D3DXVECTOR3 vAxis;
				D3DXVec3Cross( &vAxis, &vAimedDir, &ioMath::UNIT_Y );
				D3DXVec3Normalize( &vAxis, &vAxis );

				D3DXQUATERNION qtNewRot;
				float fCurAngle = ioMath::ArrangeHead( m_fArrowWeaponAngle );
				D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

				vAimedDir = qtNewRot * vAimedDir;

				pWeapon->SetMoveDir( vAimedDir );
			}
			else
			{
				pWeapon->SetMoveDir( vAimedDir );
			}
		}
	}
}

bool ioMutantItem::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioMutantItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !bCheck ) return false;

	return false;
}

bool ioMutantItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	return false;
}

void ioMutantItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetChargingState( false );
}

void ioMutantItem::GetAutoTargetValue( ioBaseChar *pOwner,
	float &fRange, float &fMinAngle, float &fMaxAngle,
	AutoTargetType eType )
{
	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;
}


void ioMutantItem::CheckReserve( ioBaseChar *pOwner )
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

void ioMutantItem::ClearState()
{
	m_ChargeState = CS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

bool ioMutantItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioMutantItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMutantItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioMutantItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

int ioMutantItem::GetMaxBullet()
{
	return (int)m_dwMaxCoolTime;
}

int ioMutantItem::GetNeedBullet()
{
	return (int)m_dwNeedCoolTime;
}

int ioMutantItem::GetCurBullet()
{
	DWORD dwCurCoolTime = m_fCurCoolTime;
	dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

	return (int)dwCurCoolTime;
}

void ioMutantItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	DWORD dwCurBullet = iCurBullet;
	dwCurBullet = max( 0, min( dwCurBullet, m_dwMaxCoolTime ) );

	m_fCurCoolTime = (float)dwCurBullet;
}

void ioMutantItem::OnReleased( ioBaseChar *pOwner )
{
	InitCoolTime();

	ioAttackableItem::OnReleased( pOwner );
}

void ioMutantItem::InitCoolTime()
{
	m_fCurCoolTime = 0.0f;
}

void ioMutantItem::MaxCoolTime()
{
	m_fCurCoolTime = (float)m_dwMaxCoolTime;
}

void ioMutantItem::DecreaseCoolTime()
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

void ioMutantItem::UpdateExtraData( ioBaseChar *pOwner )
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


void ioMutantItem::IncreaseChangeDamage( float fDamage, int iType, bool bBuff )
{
	if( iType == SSGT_GHOST_STEALER || iType == SSGT_NONE )
	{
		m_fCurCoolTime += fDamage;
		m_fCurCoolTime = max( 0.0f, m_fCurCoolTime );
		m_fCurCoolTime = min( m_fCurCoolTime, (float)m_dwMaxCoolTime );
	}
}


void ioMutantItem::SetSpecialMoveInitialize( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_MsState = MSS_PRE_DELAY;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SpecialMovePreDelayAttack.m_AttackAnimation );
	float fTimeRate = m_SpecialMovePreDelayAttack.m_fAttackAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwSpecialPreDelayAniFullTime = dwCurTime + (pGrp->GetAnimationFullTime( iAniID ) * fTimeRate);

	pOwner->RefreshFireTimeList( iAniID,
		m_SpecialMovePreDelayAttack.m_vWeaponInfoList,
		FTT_EXTEND_ATTACK,
		fTimeRate,
		0 );
}

void ioMutantItem::CheckExtendAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_MsState )
	{
	case MSS_PRE_DELAY:
		if( dwCurTime > m_dwSpecialPreDelayAniFullTime )
		{
			ChangeSpecialMoveState(pOwner);
			
		}
		break;
	case MSS_MOVE:
		CheckKeyInput(pOwner);
		ProcessRotate( pOwner );
		break;
	}
}


void ioMutantItem::ChangeSpecialMoveState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_MsState = MSS_MOVE;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_SpecialMove_MoveAttack.m_AttackAnimation );
	float fTimeRate  = m_SpecialMove_MoveAttack.m_fAttackAniRate;

	//pGrp->SetActionAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->RefreshFireTimeList( iAniID, m_SpecialMove_MoveAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_SpecialMove_MoveAttack.m_fAttackAniRate, m_SpecialMove_MoveAttack.m_dwPreDelay );

	//pOwner->SetCharCollisionSkipTime( dwCurTime, 10000 );
}


ioMutantItem::MutantSpecialState ioMutantItem::GetSpecialState()
{
	return m_MsState;
}


void ioMutantItem::SetEndSpecialState( ioBaseChar* pOwner )
{
	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_SpecialMovePreDelayWeaponIndex );

	ioUroborusWeapon4* pWeapon4 = ToUroborusWeapon4( pWeapon );
	if( pWeapon4 )
		pWeapon4->SetEndState( false, false, false, true );

	pWeapon =	g_WeaponMgr.FindWeapon( pOwner, m_dwContactWeaponIndex );
	ioRunContactWeapon *pRunContactWeapon = ToRunContactWeapon( pWeapon );
	if( pRunContactWeapon )
		pRunContactWeapon->SetWeaponDead();

	m_dwContactWeaponIndex = 0;
	m_SpecialMovePreDelayWeaponIndex = 0;

	//pOwner->SetCharCollisionSkipTime( 0, 0 );
}


void ioMutantItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
	}

	if( bChangeDir && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ROTATE;
		kPacket << bChangeDir;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioMutantItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_dwSpecialMoveRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwSpecialMoveRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}


void ioMutantItem::SetSpecialEndJump( ioBaseChar* pOwner, float fCurMoveRange )
{
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetCurMoveSpeed( 0.f );


	bool bFind = false;
	for( int i = 0; i < (int)m_vSpecialMovejumpInfo.size(); i++ )
	{
		const MutantSpecialJumpPowerInfo& Info = m_vSpecialMovejumpInfo[i];
		if( Info.fRange >= fCurMoveRange )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();

			bFind = true;
			pOwner->SetSKillEndJumpState( Info.fJumpPower, false, true, false, true );

			if( pGrp )
			{
				int iAniID = pGrp->GetAnimationIdx( Info.stJumpAni );
				
				if( iAniID > 0 )
				{
					pGrp->ClearAllActionAni( FLOAT100, true );
					pGrp->ClearAllLoopAni( FLOAT100, true );

					pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/Info.fJumpAniRate, 0.0f, 0.0f, true );
				}
			}
			

			break;
		}
	}

	if( !bFind )
		pOwner->SetSKillEndJumpState( FLOAT10, false, true, false, true );

	if( pOwner->IsNeedProcess() )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_JUMP;
		kPacket << fCurMoveRange;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioMutantItemSpecialState::ioMutantItemSpecialState()
{
}

ioMutantItemSpecialState::~ioMutantItemSpecialState()
{
}

void ioMutantItemSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMutantItemSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMutantItem *pMutantItem = ToMutantItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMutantItem )
		pMutantItem->SetEndSpecialState( pOwner );
}

void ioMutantItemSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioMutantItem *pMutantItem = ToMutantItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMutantItem )
		return;

	pMutantItem->CheckExtendAttack( pOwner );
	float fMaxRange = pMutantItem->GetSpecialMoveMaxRange();

	if(	pOwner->IsNeedProcess() && (fMaxRange < m_fCurRange || !pOwner->IsDefenseKeyDown() ) )
	{
		pMutantItem->SetSpecialEndJump( pOwner, m_fCurRange );
		return;
	}

	if( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
		if( vPos.y < (fMapHeight+FLOAT10) )
			pMutantItem->SetSpecialEndJump( pOwner, m_fCurRange );
	}
}

void ioMutantItemSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioMutantItemSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	ioMutantItem *pMutantItem = ToMutantItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMutantItem )
		return false;

	if( pMutantItem->GetSpecialState() == ioMutantItem::MSS_NONE || pMutantItem->GetSpecialState() == ioMutantItem::MSS_PRE_DELAY )
		return true;

	float fMoveSpeed = pMutantItem->GetSpecialMoveSpeed();
	float fMaxRange = pMutantItem->GetSpecialMoveMaxRange();
	float fVolumeRate = pMutantItem->GetSpecialMoveVolumeRate();

	if( fMaxRange < m_fCurRange )
		return true;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		float fFinalMoveAmt = fMoveSpeed * fDivideTimePerSec;
		if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
		{
			vMoveAmt = vMoveDir * fFinalMoveAmt;
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		float fCurLength = D3DXVec3Length( &vTotalMove );
		m_fCurRange += fCurLength;

		float fAddSineValue = max( 0.f , Help::GetHalfSineValue( fMaxRange, m_fCurRange ) );
		float fCap = fAddSineValue - m_fPreAddSineValue;

		m_fPreAddSineValue = fAddSineValue;

		//LOG.PrintTimeAndLog( 0, "%.2f   ---  %.2f", fCap, fAddSineValue );

		vTotalMove -= D3DXVECTOR3( 0.f, fCap * fVolumeRate, 0.f );

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

void ioMutantItemSpecialState::SetMutantMoveInfo( ioBaseChar* pOwner )
{
	m_fPreAddSineValue = 0.0f;
	m_fCurRange = 0.0f;

	ioMutantItem *pMutantItem = ToMutantItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMutantItem )
		pMutantItem->SetSpecialMoveInitialize( pOwner );
}