
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioDraculaItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioDraculaItem::ioDraculaItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurFly.Init();
	m_fCurGauge = 0.0f;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_FlyState = LFS_NONE;

	m_dwMapEffectID = -1;
}

ioDraculaItem::ioDraculaItem( const ioDraculaItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackFly( rhs.m_AttackFly ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fCheckLandHeight( rhs.m_fCheckLandHeight ),
	m_fAirJumpPower( rhs.m_fAirJumpPower ),
	m_dwMaxSpecialTime( rhs.m_dwMaxSpecialTime ),
	m_stMapEffect( rhs.m_stMapEffect )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurFly.Init();
	m_fCurGauge = 0.0f;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_dwSpecialStartTime = 0;

	m_FlyState = LFS_NONE;

	m_dwMapEffectID = -1;
}

ioDraculaItem::~ioDraculaItem()
{
}

void ioDraculaItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadFlyInfo( rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

void ioDraculaItem::LoadFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_AttackFly.Init();

	m_AttackFly.m_fStartAngle = rkLoader.LoadFloat_e( "fly_start_angle", 0.0f );
	m_AttackFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "fly_start_max_range", 0.0f );
	m_AttackFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "fly_start_duration", 0 );

	m_AttackFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "fly_rotate_speed", 0 );
	m_AttackFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_AttackFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );
	m_AttackFly.m_vBuffList.clear();

	int nBuffCnt = rkLoader.LoadInt_e( "special_buff_cnt", 0 );
	for( int k=0; k < nBuffCnt; ++k )
	{
		wsprintf_e( szKey, "special_buff%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_AttackFly.m_vBuffList.push_back( szBuff );
	}

	m_fCheckLandHeight = rkLoader.LoadFloat_e( "check_land_height", FLOAT10 );
	m_fAirJumpPower = rkLoader.LoadFloat_e( "air_jump_power", 0.f );

	m_dwMaxSpecialTime = (DWORD)rkLoader.LoadInt_e( "max_special_time", 0 );

	rkLoader.LoadString_e( "special_map_effect", "", szBuf, MAX_PATH );
	m_stMapEffect = szBuf;
}

ioItem* ioDraculaItem::Clone()
{
	return new ioDraculaItem( *this );
}

ioWeaponItem::WeaponSubType ioDraculaItem::GetSubType() const
{
	return WST_DRACULA;
}

void ioDraculaItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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
	m_TargetSkill.Clear();

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

void ioDraculaItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioDraculaItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetFlyState( pOwner, m_AttackFly, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioDraculaItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
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

void ioDraculaItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioDraculaItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	/*case SST_FLY_ATTACK_START:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );

			//ChangeFlyAttack( pOwner );
		}
		break;*/
	/*case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;

			SetCurGauge( fGauge );
		}
		break;*/
	case SST_FLY_STATE:
		{
			int iFlyState;
			rkPacket >> iFlyState;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetFlyState( pOwner, m_AttackFly, false );
		}
		break;
	case SST_ROTATE:
		{
			bool bChangeDir, bChangeUpDown;
			rkPacket >> bChangeDir >> bChangeUpDown;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			int iDir;
			rkPacket >> iDir;

			bool bSetUpKey, bSetDownKey, bOptionMove;
			rkPacket >> bSetUpKey >> bSetDownKey >> bOptionMove;

			pOwner->SetTargetRot( qtRot );

			if( bChangeDir )
			{
				m_PreDirKey = m_CurDirKey;
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
	}
}

void ioDraculaItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &vDir, &vDir );
				}
			}	
			
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

void ioDraculaItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

void ioDraculaItem::CheckKeyInput( ioBaseChar *pOwner, bool bOptionMove )
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

		m_PreDirKey = m_CurDirKey;
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

			//LOG.PrintTimeAndLog( 0, "ioDraculaItem::Send KeyChange %d,%d", (int)m_bSetUpKey, (int)m_bSetDownKey);

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

void ioDraculaItem::ChangeFlyStart( ioBaseChar *pOwner )
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

	pOwner->SetState( CS_DRACULA_FLY );

	m_FlyState = LFS_START;

	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

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

void ioDraculaItem::ChangeFlyDelay( ioBaseChar *pOwner )
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
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
}

void ioDraculaItem::ChangeFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	//DWORD dwCurTime = FRAMEGETTIME();

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = m_CurDirKey;

	int iAniID = 0;
	float fTimeRate = 0;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	/*if( eDir == ioUserKeyInput::DKI_NONE )
	{
		iAniID = pGrp->GetAnimationIdx( m_CurFly.m_DelayAni );
		fTimeRate = m_CurFly.m_fDelayAniRate;
	}
	else
	{
		iAniID = pGrp->GetAnimationIdx( m_CurFly.m_MoveAni );
		fTimeRate = m_CurFly.m_fMoveAniRate;
	}

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );*/
}

void ioDraculaItem::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
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
		/*if( pOwner->IsNeedProcess() && m_fCurGauge <= 0.0f )
		{
			ChangeFlyEnd( pOwner );
			return;
		}*/

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
					m_TargetSkill = m_Target;
					if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
						return;
				}

				m_TargetSkill.Clear();
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
	case LFS_END:
		break;
	}
}

void ioDraculaItem::ProcessRotate( ioBaseChar *pOwner )
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

bool ioDraculaItem::ProcessFlyStart( ioBaseChar *pOwner )
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

bool ioDraculaItem::ProcessFlyMove( ioBaseChar *pOwner )
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
	//if( bOptionMove == false )
	{
		// UpDown Move
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
	}

	vCurPos.y = max( vCurPos.y, fMapHeight );

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	return true;
}

int ioDraculaItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioDraculaItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioDraculaItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioDraculaItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioDraculaItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioDraculaItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioDraculaItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioDraculaItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioDraculaItem::DecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioDraculaItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioDraculaItem::IsDraculaState()
{
	switch( m_FlyState )
	{
	case LFS_START:
	case LFS_DELAY:
		return true;
	}

	return false;
}

void ioDraculaItem::UpdateExtraData( ioBaseChar *pOwner )
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
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioDraculaItem::ClearState( ioBaseChar *pOwner, bool bEraseBuff )
{
	m_FlyState = LFS_NONE;

	if( bEraseBuff )
		EraseBuff( pOwner );

	m_CurFly.Init();

	m_dwFlyStartTime = 0;
	m_dwFlyMoveTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_dwFlyAttackEndTime = 0;

	m_Target.Clear();

	if( m_dwMapEffectID != -1 )
	{
		pOwner->DestroyMapEffect( m_dwMapEffectID );
		m_dwMapEffectID = -1;
	}

}

bool ioDraculaItem::IsFlyCharCollisionSkipState()
{
	switch( m_FlyState )
	{
	case LFS_START:
		return true;
	//case LFS_MOVE:
	case LFS_DELAY:
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE || m_bSetUpKey || m_bSetDownKey )
			return true;
	}

	return false;
}

void ioDraculaItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;
}

void ioDraculaItem::OnReleased( ioBaseChar *pOwner )
{
	ClearState( pOwner );
	m_TargetSkill.Clear();
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioDraculaItem::ChangeFlyEnd( ioBaseChar *pOwner )
{
	m_FlyState = LFS_END;

	if( pOwner && pOwner->IsNeedProcess() )
	{
		//pOwner->SetExtendAttackEndJump( m_CurFly.m_fFlyJumpAmt, m_fExtendLandingRate );
		//pOwner->ClearDirDoubleClick();
	}
}

const ioHashString& ioDraculaItem::GetTargetSkill()
{
	return m_TargetSkill;
}

void ioDraculaItem::ClearTargetSkill()
{
	m_TargetSkill.Clear();
}

bool ioDraculaItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioDraculaItem::InitLuciferGauge()
{
	m_fCurGauge = 0.0f;
}

void ioDraculaItem::SetFlyState( ioBaseChar *pOwner, const DraculaFlyInfo &rkFlyInfo, bool bSendNet )
{
	m_CurFly = rkFlyInfo;
	AddBuff( pOwner );
	DecreaseGauge();
	ChangeFlyStart( pOwner );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_FlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDraculaItem::AddBuff( ioBaseChar *pOwner )
{
	// add buff
	int iBuffCnt = m_CurFly.m_vBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_CurFly.m_vBuffList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}
}

void ioDraculaItem::EraseBuff( ioBaseChar *pOwner )
{
	int iBuffCnt = m_CurFly.m_vBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_CurFly.m_vBuffList[j];
		pOwner->RemoveBuff( szBuffName );
	}
}

void ioDraculaItem::CheckFlyState( ioBaseChar *pOwner, bool bAttack )
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
}

void ioDraculaItem::ChangeLandNormalAttack( ioBaseChar *pOwner )
{
	pOwner->SetNormalAttack( 0 );
}

void ioDraculaItem::ChangeAirNornmalAttack( ioBaseChar *pOwner, bool bAttack )
{
	if( m_fAirJumpPower )
		pOwner->SetExtendAttackEndJump( m_fAirJumpPower, FLOAT1 );

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( pItem )
	{
		ioChargeComboJump2 *pChargeComboJump2 = ToChargeComboJump2( pItem->GetCurExtendJump() );

		if( pChargeComboJump2 && bAttack )
		{
			pChargeComboJump2->CheckComboJumpAttack( pOwner );
		}
		
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDraculaFlySpecialState::ioDraculaFlySpecialState()
{
}

ioDraculaFlySpecialState::~ioDraculaFlySpecialState()
{
}

void ioDraculaFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioDraculaFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioDraculaItem *pDraculaItem = ToDraculaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDraculaItem )
		pDraculaItem->ClearState( pOwner, true );
}

void ioDraculaFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioDraculaItem *pDracula = ToDraculaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDracula )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
				fPreHeightForGhostState = pOwner->GetBottomHeight();

			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}
		
		pDracula->ProcessFlyState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioDraculaFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioDraculaFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioDraculaItem *pDraculaItem = ToDraculaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pDraculaItem )
		pOwner->SetState( CS_DELAY );

	if( !pDraculaItem->IsDraculaState() )
		pOwner->SetState( CS_DELAY );

	return true;
}