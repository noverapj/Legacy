
#include "stdafx.h"

#include "ioNightmareItem.h"
#include "ItemDefine.h"

ioNightmareItem::ioNightmareItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurFly.Init();
	m_fCurGauge = 0.0f;

	m_dwFlyStartTime = 0;
	m_FlyState = LFS_NONE;

	m_dwMapEffectID = -1;
	m_dwWeaponBaseIndex = -1;
}

ioNightmareItem::ioNightmareItem( const ioNightmareItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackFly( rhs.m_AttackFly ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fCheckLandHeight( rhs.m_fCheckLandHeight ),
	m_fAirJumpPower( rhs.m_fAirJumpPower ),
	m_dwMaxSpecialTime( rhs.m_dwMaxSpecialTime ),
	m_stMapEffect( rhs.m_stMapEffect ),
	m_FireAttribute( rhs.m_FireAttribute ),
	m_LoopAnimation( rhs.m_LoopAnimation ),
	m_dwLoopTime( rhs.m_dwLoopTime ),
	m_dwCollisionSkipTime( rhs.m_dwCollisionSkipTime )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurFly.Init();
	m_fCurGauge = 0.0f;

	m_dwFlyStartTime = 0;
	m_dwSpecialStartTime = 0;
	m_FlyState = LFS_NONE;

	m_dwMapEffectID = -1;

	for( int i = 0; i < 3; i++ )
		m_fIncreaseGauge[i] = rhs.m_fIncreaseGauge[i];
}

ioNightmareItem::~ioNightmareItem()
{
}

void ioNightmareItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	LoadFlyInfo( rkLoader, m_AttackFly, "fly" );
	LoadAttackAttribute_e( "fire_attack", m_FireAttribute, rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	
	for( int i=0; i < GAUGE_COUNT; ++i )
	{
		wsprintf_e( szBuf, "increase_gauge%d", i+1 );
		m_fIncreaseGauge[i] = rkLoader.LoadFloat( szBuf, FLOAT1 );
	}

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_duration", 0 );

	m_fCheckLandHeight = rkLoader.LoadFloat_e( "check_land_height", FLOAT10 );
	m_fAirJumpPower = rkLoader.LoadFloat_e( "air_jump_power", 0.f );
	m_dwMaxSpecialTime = (DWORD)rkLoader.LoadInt_e( "max_special_time", 0 );
	m_dwCollisionSkipTime = (DWORD)rkLoader.LoadInt_e( "collision_skip_time", 0 );

	rkLoader.LoadString_e( "special_map_effect", "", szBuf, MAX_PATH );
	m_stMapEffect = szBuf;
}

void ioNightmareItem::LoadFlyInfo( ioINILoader &rkLoader, DraculaFlyInfo& cInfo, const char *szAttack )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	cInfo.Init();

	wsprintf_e( szKey, "%s_start_angle", szAttack );
	cInfo.m_fStartAngle = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_start_max_range", szAttack );
	cInfo.m_fStartMaxRange = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_start_duration", szAttack );
	cInfo.m_dwStartDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_rotate_speed", szAttack );
	cInfo.m_dwRotateTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_move_speed", szAttack );
	cInfo.m_fMoveSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_updown_speed", szAttack );
	cInfo.m_fUpDownSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	cInfo.m_vBuffList.clear();
	wsprintf_e( szKey, "%s_special_buff_cnt", szAttack );
	int nBuffCnt = rkLoader.LoadInt( szKey, 0 );
	for( int k=0; k < nBuffCnt; ++k )
	{
		wsprintf_e( szKey, "%s_special_buff%d", szAttack, k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		cInfo.m_vBuffList.push_back( szBuff );
	}
}

ioItem* ioNightmareItem::Clone()
{
	return new ioNightmareItem( *this );
}

ioWeaponItem::WeaponSubType ioNightmareItem::GetSubType() const
{
	return WST_NIGHTMARE_ITEM;
}

void ioNightmareItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	ClearState( pOwner );

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
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

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
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

void ioNightmareItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
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

void ioNightmareItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetAttackState( pOwner, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioNightmareItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
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

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNightmareItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;
	}
}

void ioNightmareItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_STATE:
		{
			int iFlyState, iState;
			rkPacket >> iFlyState >> iState;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			float fCurGauge;
			rkPacket >> fCurGauge;
			SetCurGauge( fCurGauge );

			if( iState == e_DASH )
				SetDashFlyState( pOwner, false );
			else if( iState == e_Wound )
				SetWoundFlyState( pOwner, false );
		}
		break;
	case SST_ROTATE:
		{
			bool bChangeDir, bChangeUpDown;
			rkPacket >> bChangeDir >> bChangeUpDown;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			int iDir;
			rkPacket >> iDir;

			bool bSetUpKey, bSetDownKey, bOptionMove;
			rkPacket >> bSetUpKey >> bSetDownKey >> bOptionMove;

			if( bChangeDir )
			{
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

				if( !bOptionMove )
					ChangeFlyMove( pOwner );
			}
			if( bChangeUpDown )
			{
				m_bSetUpKey = bSetUpKey;
				m_bSetDownKey = bSetDownKey;
			}
		}
		break;
	case SST_FIRE:
		{
			int iFlyState;
			rkPacket >> iFlyState;

			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos;
			rkPacket >> qtRot;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );

			if( iFlyState == LFS_FIRE )
				SetAttackState( pOwner, false );
			else if( iFlyState == LFS_FIRE_LOOP )
				SetLoopState( pOwner, false );
			else if( iFlyState == LFS_FIRE_END )
				SetFireEndState( pOwner, false );
		}
		break;
	}
}

void ioNightmareItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioUroborusWeapon3 *pUroborus3 = ToUroborusWeapon3( pWeapon );
	if( pUroborus3 )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );
		
			pUroborus3->SetMoveDir( vDir );
		}
	}

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

void ioNightmareItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

void ioNightmareItem::CheckKeyInput( ioBaseChar *pOwner, bool bOptionMove )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;
	}

	// Check UpDown
	if( m_bSetUpKey )
	{
		if( !pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;

			if( pOwner->IsDefenseKeyDown() )
				m_bSetDownKey = true;
		}
	}
	else if( m_bSetDownKey )
	{
		if( !pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = false;

			if( pOwner->IsJumpKeyDown() )
				m_bSetUpKey = true;
		}
	}
	else
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;
			m_bSetDownKey = true;
		}

		if( pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = true;
			m_bSetDownKey = false;
		}
	}

	if( bOptionMove )
	{
		if( bChangeDir )
		{
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_ROTATE;
				kPacket << bChangeDir;
				kPacket << false;
				kPacket << pOwner->GetTargetRot();
				kPacket << (int)m_CurDirKey;
				kPacket << m_bSetUpKey;
				kPacket << m_bSetDownKey;
				kPacket << bOptionMove;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( bChangeDir || bChangeUpDown )
		{
			ChangeFlyMove( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_ROTATE;
				kPacket << bChangeDir;
				kPacket << bChangeUpDown;
				kPacket << pOwner->GetTargetRot();
				kPacket << (int)m_CurDirKey;
				kPacket << m_bSetUpKey;
				kPacket << m_bSetDownKey;
				kPacket << bOptionMove;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioNightmareItem::ChangeFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	m_fFlyCurRange = 0.f;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	pOwner->SetState( CS_NIGHTMARE_SPECIAL );
	m_FlyState = LFS_START;
	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwSpecialStartTime = FRAMEGETTIME();

	if( m_dwMapEffectID != -1 )
	{
		pOwner->DestroyMapEffect( m_dwMapEffectID );
		m_dwMapEffectID = -1;
	}

	D3DXVECTOR3 vScale = pOwner->GetWorldScale();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	ioMapEffect *pMapEffect = pOwner->CreateMapEffectBySkill( m_stMapEffect, vPos, vScale );
	if( pMapEffect )
		m_dwMapEffectID = pMapEffect->GetUniqueID();
}

void ioNightmareItem::ChangeFlyDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_FlyState = LFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwFlyStartTime = 0;
}

void ioNightmareItem::ChangeFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = m_CurDirKey;

	int iAniID = 0;
	float fTimeRate = 0;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
}

void ioNightmareItem::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FlyState )
	{
	case LFS_START:
		pOwner->SetCurMoveSpeed( 0 );

		if( m_dwFlyStartTime+m_CurFly.m_dwStartDuration < dwCurTime )
		{
			ChangeFlyDelay( pOwner );
			return;
		}
		else if( !ProcessFlyStart( pOwner ) )
		{
			ChangeFlyDelay( pOwner );
			return;
		}
		break;
	case LFS_DELAY:
		if( pOwner->IsNeedProcess() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				// 스킬 존재체크
				ioSkill *pSkill = pOwner->GetEquipedSkill( iSkillNum );
				if( pSkill &&
					!ToPassiveSkill(pSkill) &&
					pSkill->IsCanJumpingSkill() &&
					!pOwner->IsChatModeState( true ) &&
					!pOwner->IsBuffLimitSkill() )
				{
					if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
						return;
				}
			}
			if( pOwner->IsAttackKey() )
			{
				CheckFlyState( pOwner, true );
				return;
			}
			if( (m_dwSpecialStartTime + m_dwMaxSpecialTime) < dwCurTime )
			{
				CheckFlyState( pOwner, false );
				return;
			}
		}

		ProcessFlyMove( pOwner );
		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );

		if( m_dwMapEffectID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwMapEffectID );
			if( pMapEffect )
			{
				pMapEffect->SetWorldPosition( pOwner->GetMidPositionByRate() );
				pMapEffect->SetWorldOrientation( pOwner->GetWorldOrientation() );
			}
		}
		break;
	case LFS_FIRE:
		if( dwCurTime > m_dwMotionEndTime )
			SetLoopState( pOwner, true );
		break;
	case LFS_FIRE_LOOP:
		{
			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
			if( !pWeapon || !pWeapon->IsLive() )
				SetFireEndState( pOwner, true );
			else if( dwCurTime > m_dwLoopStartTime + m_dwLoopTime )
				SetFireEndState( pOwner, true );
			else if( pOwner->IsAttackKey() )
				SetFireEndState( pOwner, true );
		}
		break;
	}
}

void ioNightmareItem::SetLoopState( ioBaseChar *pOwner, bool bSend )
{
	if( m_LoopAnimation.IsEmpty() )
	{
		SetFireEndState( pOwner, bSend );
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_dwLoopStartTime = FRAMEGETTIME();
	m_FlyState = LFS_FIRE_LOOP;

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FIRE;
		kPacket << m_FlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNightmareItem::SetFireEndState( ioBaseChar *pOwner, bool bSend )
{
	m_iCurCombo = 0;
	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
	m_FlyState = LFS_FIRE_END;

	ClearKeyReserve();	
	pOwner->SetSKillEndJumpState( 0.1f, false, false, false );

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
	if( pWeapon && pWeapon->IsLive() )
		pWeapon->SetWeaponDead();

	m_dwWeaponBaseIndex = -1;
	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FIRE;
		kPacket << m_FlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNightmareItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_CurFly.m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_CurFly.m_dwRotateTime;
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

bool ioNightmareItem::ProcessFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurFly.m_fStartMaxRange / (float)m_CurFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_CurFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurFly.m_fStartMaxRange - m_fFlyCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	return bRangeLimit;
}

bool ioNightmareItem::ProcessFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fCurMoveSpeed = m_CurFly.m_fMoveSpeed * fTimePerSec;
		D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );

	//특공시 up_down은 옵션 처리한다.
	if( m_bSetUpKey || m_bSetDownKey )
	{
		float fCurHeightAmt = 0.0f;
		float fCurUpDownSpeed = m_CurFly.m_fUpDownSpeed * fTimePerSec;

		if( m_bSetUpKey )
			fCurHeightAmt += fCurUpDownSpeed;
		else if( m_bSetDownKey )
			fCurHeightAmt -= fCurUpDownSpeed;

		vCurPos.y += fCurHeightAmt;
	}

	vCurPos.y = max( vCurPos.y, fMapHeight );

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	return true;
}

int ioNightmareItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioNightmareItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioNightmareItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioNightmareItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioNightmareItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioNightmareItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurGauge -= fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioNightmareItem::IsEnableGauge( bool bDouble )
{
	if( bDouble && m_fCurGauge >= m_fNeedGauge * 2.f )
		return true;
	else if( !bDouble && m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioNightmareItem::IsDraculaState()
{
	switch( m_FlyState )
	{
	case LFS_START:
	case LFS_DELAY:
	case LFS_FIRE:
	case LFS_FIRE_LOOP:
		return true;
	}

	return false;
}

void ioNightmareItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fIncreaseGauge = m_fIncreaseGauge[0];
			int iCurBullet = GetCurBullet();
			if( COMPARE( iCurBullet, 0, GAUGE_COUNT ) )
				fIncreaseGauge = m_fIncreaseGauge[iCurBullet];

			float fGauge = m_fCurExtraGauge + fIncreaseGauge * fTimePerSec;
			SetCurExtraGauge( fGauge );

			if( m_fMaxExtraGauge == m_fCurExtraGauge )
			{
				SetCurBullet( iCurBullet + 1 );
				InitExtraGauge();
			}
		}
		return;
	}
}

void ioNightmareItem::ClearState( ioBaseChar *pOwner, bool bEraseBuff )
{
	m_FlyState = LFS_NONE;

	if( bEraseBuff )
		EraseBuff( pOwner );

	m_CurFly.Init();
	m_dwFlyStartTime = 0;

	if( m_dwMapEffectID != -1 )
	{
		pOwner->DestroyMapEffect( m_dwMapEffectID );
		m_dwMapEffectID = -1;
	}
}

bool ioNightmareItem::IsFlyCharCollisionSkipState()
{
	switch( m_FlyState )
	{
	case LFS_START:
		return true;

	case LFS_DELAY:
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE || m_bSetUpKey || m_bSetDownKey )
			return true;
	}

	return false;
}

void ioNightmareItem::OnReleased( ioBaseChar *pOwner )
{
	ClearState( pOwner );
	m_fCurGauge = 0.0f;
	InitExtraGauge();

	ioAttackableItem::OnReleased( pOwner );
}

void ioNightmareItem::SetFlyState( ioBaseChar *pOwner, const DraculaFlyInfo &rkFlyInfo, int iState, bool bSendNet )
{
	m_CurFly = rkFlyInfo;
	AddBuff( pOwner );
	ChangeFlyStart( pOwner );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_FlyState;
		kPacket << iState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioNightmareItem::SetDashFlyState( ioBaseChar *pOwner, bool bSendNet )
{
	if( pOwner && ( pOwner->GetState() == CS_NIGHTMARE_SPECIAL || pOwner->GetState() == CS_NEW_JUMP_ATTACK ) )
		return false;

	DecreaseGauge( m_fNeedGauge );
	m_bCollisionAvailable = false;

	SetFlyState( pOwner, m_AttackFly, e_DASH, bSendNet );
	return true;
}

void ioNightmareItem::SetWoundFlyState( ioBaseChar *pOwner, bool bSendNet )
{
	if( pOwner && ( pOwner->GetState() == CS_NIGHTMARE_SPECIAL || pOwner->GetState() == CS_NEW_JUMP_ATTACK ) )
		return;

	SetCurGauge( 0.f );
	m_bCollisionAvailable = true;

	SetFlyState( pOwner, m_AttackFly, e_Wound, bSendNet );
}

void ioNightmareItem::SetAttackState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	m_fFlyCurRange = 0.f;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	if( pOwner->GetState() != CS_NIGHTMARE_SPECIAL )
		pOwner->SetState( CS_NIGHTMARE_SPECIAL );
	
	m_FlyState = LFS_FIRE;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurAni = m_FireAttribute.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	pOwner->SetAutoTarget( ATT_NORMAL );
	m_dwWeaponBaseIndex = pOwner->SetNormalAttackByAttribute( m_FireAttribute, true );

	int iAniID = pGrp->GetAnimationIdx( m_FireAttribute.m_AttackAnimation );
	float fTimeRate = m_FireAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_FireAttribute.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_dwAttackStartTime = dwCurTime; 
	m_dwMotionEndTime = m_dwAttackStartTime + ( pGrp->GetAnimationFullTime( iAniID ) * m_FireAttribute.m_fAttackAniRate );
	
	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FIRE;
		kPacket << m_FlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNightmareItem::AddBuff( ioBaseChar *pOwner )
{
	// add buff
	int iBuffCnt = m_CurFly.m_vBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_CurFly.m_vBuffList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}
}

void ioNightmareItem::EraseBuff( ioBaseChar *pOwner )
{
	int iBuffCnt = m_CurFly.m_vBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_CurFly.m_vBuffList[j];
		pOwner->RemoveBuff( szBuffName );
	}
}

void ioNightmareItem::CheckFlyState( ioBaseChar *pOwner, bool bAttack )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fMapHeight = pStage->GetMapHeight( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
	float fCharHeight = pOwner->GetWorldPosition().y;
	if( (fCharHeight > fMapHeight) && ( (fCharHeight - fMapHeight) > m_fCheckLandHeight ) )
	{
		ChangeAirNornmalAttack( pOwner, bAttack );
	}
	else
	{
		if( bAttack )
			ChangeLandNormalAttack( pOwner );
		else
			pOwner->SetState( CS_DELAY );
	}
	ioIljimaeJump *pIljimaeJump = ToIljimaeJump( GetCurExtendJump() );
	if( pIljimaeJump )
		pIljimaeJump->InitJump();
}

void ioNightmareItem::ChangeLandNormalAttack( ioBaseChar *pOwner )
{
	pOwner->SetNormalAttack( 0 );
}

void ioNightmareItem::ChangeAirNornmalAttack( ioBaseChar *pOwner, bool bAttack )
{
	if( m_fAirJumpPower )
		pOwner->SetExtendAttackEndJump( m_fAirJumpPower, FLOAT1 );

	if( bAttack )
		pOwner->SetNewJumpAttackState( true );
}

bool ioNightmareItem::IsWoundAfterUseExtendState()
{
	return true;
}

bool ioNightmareItem::CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType )
{
	if( !pOwner )
		return false;

	if( m_dwWoundedChargeStartTime == 0 )
	{
		m_dwWoundedChargeStartTime = FRAMEGETTIME();
	}
	else if( !pOwner->IsAttackKeyDown() )
	{
		m_dwWoundedChargeStartTime = FRAMEGETTIME();
	}
	else if( m_dwWoundedChargeStartTime > 0 && m_dwWoundedChargeStartTime + m_dwChargeTime < FRAMEGETTIME() && pOwner->IsAttackKeyDown() )
	{
		if( IsEnableGauge( true ) )
		{
			SetWoundFlyState( pOwner, true );
			return true;
		}
	}
	return false;
}

bool ioNightmareItem::IsCollisionAvailableWeaponBySpecialState()
{
	switch( m_FlyState )
	{
	case LFS_START:
	case LFS_DELAY:
		if( m_bCollisionAvailable && m_dwSpecialStartTime + m_dwCollisionSkipTime < FRAMEGETTIME() )
			return false;

		break;
	}
	return true;
}

ioNightmareSpecialState::ioNightmareSpecialState()
{
}

ioNightmareSpecialState::~ioNightmareSpecialState()
{
}

void ioNightmareSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNightmareSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioNightmareItem *pNightmareItem = ToNightmareItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNightmareItem )
		pNightmareItem->ClearState( pOwner, true );
}

void ioNightmareSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNightmareItem *pNightmareItem = ToNightmareItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNightmareItem )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
				fPreHeightForGhostState = pOwner->GetBottomHeight();

			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}
		
		pNightmareItem->ProcessFlyState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNightmareSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioNightmareSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioNightmareItem *pNightmareItem = ToNightmareItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pNightmareItem )
		pOwner->SetState( CS_DELAY );

	if( !pNightmareItem->IsDraculaState() )
		pOwner->SetState( CS_DELAY );

	return true;
}

bool ioNightmareSpecialState::IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon )
{
	ioNightmareItem *pNightmareItem = ToNightmareItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNightmareItem  )
		return pNightmareItem->IsCollisionAvailableWeaponBySpecialState();
	else
		return true;
}