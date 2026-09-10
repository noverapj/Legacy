
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioDevilItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioDevilItem::ioDevilItem()
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

	m_dwFlyForceTime = 0;

	m_FlyState = DFS_NONE;
	m_FlyAttackState = DFAS_NONE;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0;
	m_dwFireStartTime = 0;
}

ioDevilItem::ioDevilItem( const ioDevilItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackFly( rhs.m_AttackFly ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
	m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
	m_fHighDecreaseGauge( rhs.m_fHighDecreaseGauge ),
	m_bSpecialAttackRotate( rhs.m_bSpecialAttackRotate ),
	m_fSpecialAttackSpeed( rhs.m_fSpecialAttackSpeed )
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

	m_dwFlyForceTime = 0;

	m_FlyState = DFS_NONE;
	m_FlyAttackState = DFAS_NONE;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0; 
	m_dwFireStartTime = 0;

	m_dwNextFireTime = 0;
	m_dwFireBallAttackEndTime = 0;

	m_bAttackKeyDown = false;
}

ioDevilItem::~ioDevilItem()
{
}

void ioDevilItem::LoadProperty( ioINILoader &rkLoader )
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
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGauge = rkLoader.LoadFloat_e( "high_decrease_gauge", 3.0f );
}

void ioDevilItem::LoadFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_AttackFly.Init();

	rkLoader.LoadString_e( "fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_StartAni = szBuf;
	m_AttackFly.m_fStartAniRate = rkLoader.LoadFloat_e( "fly_start_ani_rate", FLOAT1 );

	m_AttackFly.m_fStartAngle = rkLoader.LoadFloat_e( "fly_start_angle", 0.0f );
	m_AttackFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "fly_start_max_range", 0.0f );
	m_AttackFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "fly_start_duration", 0 );

	rkLoader.LoadString_e( "fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_MoveAni = szBuf;
	m_AttackFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "fly_move_ani_rate", FLOAT1 );

	m_AttackFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "fly_rotate_speed", 0 );
	m_AttackFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_AttackFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_AttackFly.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "fly_jump_amt", 0.0f );


	//공중 자동 공격
	m_AttackFly.m_MoveAttack.Init();
	LoadAttackAttribute_e( "fly_auto_attack", m_AttackFly.m_MoveAttack, rkLoader );
	m_AttackFly.m_dwFlyAttackTimeGap = (DWORD)rkLoader.LoadInt_e( "fly_auto_attack_time_gap", 1000 );

	//공중 수동 공격
	m_AttackFly.m_FireBallAttack.Init();
	LoadAttackAttribute_e( "fly_manual_attack", m_AttackFly.m_FireBallAttack, rkLoader );

	m_AttackFly.m_fFlyFireNeedGauge = rkLoader.LoadFloat( "fly_manual_attack_fire_gauge", FLOAT0001 );

	m_AttackFly.m_dwCancelTime = (DWORD)rkLoader.LoadInt( "fly_cancel_time", 0 );
}

ioItem* ioDevilItem::Clone()
{
	return new ioDevilItem( *this );
}

ioWeaponItem::WeaponSubType ioDevilItem::GetSubType() const
{
	return WST_DEVIL;
}

void ioDevilItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioDevilItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioDevilItem::OnCharging( ioBaseChar *pOwner )
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

void ioDevilItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioDevilItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioDevilItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_ATTACK_START:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );

			ChangeFlyAttack( pOwner );
		}
		break;
	case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;

			//SetEnableEffect( pOwner );
			SetCurGauge( fGauge );
		}
		break;
	case SST_FLY_STATE:
		{
			CheckFlyState( pOwner, rkPacket );
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

void ioDevilItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioDevilItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		}
	}

	return false;
}

void ioDevilItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioDevilItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioDevilItem::GetAutoTargetValue( ioBaseChar *pOwner,
	float &fRange, float &fMinAngle, float &fMaxAngle,
	AutoTargetType eType )
{
	if( IsChargeAutoTarget(pOwner) )
	{
		float fCurTargetRangeRate, fCurTargetAngleRate;
		fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

		float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
		float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

		fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
		fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

		fRange = m_fAutoTargetRange * fCurTargetRangeRate;
		fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
		fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
	}
	else
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
}

void ioDevilItem::CheckKeyInput( ioBaseChar *pOwner, bool bOptionMove )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey && eNewDirKey != ioUserKeyInput::DKI_NONE )
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
			//ChangeFlyMove( pOwner );

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

void ioDevilItem::SetFlyState( ioBaseChar *pOwner, const DevilFlyInfo &rkFlyInfo, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_DEVIL_FLY );
	
	m_CurFly = rkFlyInfo;

	ChangeFlyStart( pOwner );
	m_fFlyCurRange = 0.0f;

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

void ioDevilItem::ChangeFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurFly.m_StartAni );
	float fTimeRate  = m_CurFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_FlyState = DFS_START;

	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_dwFlyForceTime = 0;
}

void ioDevilItem::ChangeFlyMove( ioBaseChar *pOwner )
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

	m_FlyState = DFS_MOVE;

	/*if( eDir == ioUserKeyInput::DKI_NONE )
	{
		iAniID = pGrp->GetAnimationIdx( m_CurFly.m_DelayAni );
		fTimeRate = m_CurFly.m_fDelayAniRate;
	}
	else*/
	{
		iAniID = pGrp->GetAnimationIdx( m_CurFly.m_MoveAni );
		fTimeRate = m_CurFly.m_fMoveAniRate;
	}

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwNextFireTime = FRAMEGETTIME();
	m_dwNextFireTime += m_CurFly.m_dwFlyAttackTimeGap;
}

void ioDevilItem::ChangeFlyAttack( ioBaseChar *pOwner )
{
	//LOG.PrintTimeAndLog( 0, "ioDevilItem::ChangeFlyAttack" );
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurFly.m_FireBallAttack.m_AttackAnimation );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_CurFly.m_FireBallAttack.m_fAttackAniRate );

	m_FlyState = DFS_FLY_ATTACK;
	m_FlyAttackState = DFAS_DELAY;

	m_dwFlyStartTime = 0;
	m_dwFlyMoveTime = 0;
	m_dwFlyAttackTime = dwCurTime;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_dwFireBallAttackEndTime = m_dwFireStartTime = m_dwFireEndTime = m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_CurFly.m_FireBallAttack.m_fAttackAniRate);
	m_dwFireBallAttackEndTime = m_dwMotionEndTime;
	m_dwFireStartTime += (pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_CurFly.m_FireBallAttack.m_fAttackAniRate);

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_ATTACK_START;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_fCurGauge = max( 0.f, m_fCurGauge - m_CurFly.m_fFlyFireNeedGauge );
}

void ioDevilItem::FlyFireBallAttackFire( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	int iAniID		= pGrp->GetAnimationIdx( m_CurFly.m_FireBallAttack.m_AttackAnimation );
	float fTimeRate  = m_CurFly.m_FireBallAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurFly.m_FireBallAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurFly.m_FireBallAttack.m_dwEndDelay;

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;

	if( m_CurFly.m_FireBallAttack.m_vWeaponInfoList.size() )
		kFireTime.iAttributeIdx = m_CurFly.m_FireBallAttack.m_vWeaponInfoList[0].m_iWeaponIdx;
	else
		kFireTime.iAttributeIdx = 0;

	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	m_FlyState = DFS_FLY_ATTACK;
	m_FlyAttackState = DFAS_ATTACK;

	return;
}

void ioDevilItem::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FlyState )
	{
	case DFS_START:
		/*pOwner->InitForcePower();
		pOwner->SetJumpPower( 0 );
		pOwner->SetAirJumpPower( 0 );
		pOwner->ClearReservedSliding();*/
		pOwner->SetCurMoveSpeed( 0 );

		if( m_dwFlyStartTime+m_CurFly.m_dwStartDuration < dwCurTime )
		{
			//ChangeFlyDelay( pOwner );
			ChangeFlyMove( pOwner );
			return;
		}
		else if( !ProcessFlyStart( pOwner ) )
		{
			//ChangeFlyDelay( pOwner );
			ChangeFlyMove( pOwner );
			return;
		}
		break;
	case DFS_MOVE:
		if( pOwner->IsNeedProcess() && m_fCurGauge <= 0.0f )
		{
			ChangeFlyEnd( pOwner );
			return;
		}

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

			
			if( pOwner->IsAttackKeyDown() )//&& m_fCurGauge >= m_CurFly.m_fFlyFireNeedGauge )
			{
				if( m_bAttackKeyDown == false )
					m_dwFlyCancelStartTime = dwCurTime;

				m_bAttackKeyDown = true;

				if( (m_dwFlyCancelStartTime + m_CurFly.m_dwCancelTime) <= dwCurTime )
				{
					ChangeFlyEnd(pOwner);
					return;
				}

				if( pOwner->IsAttackKey() )//&& m_bAttackKeyDown )
				{
					ChangeFlyAttack( pOwner );
					m_bAttackKeyDown = false;
					return;
				}
			}
			else
			if( pOwner->IsAttackKey() )//&& m_bAttackKeyDown )
			{
				ChangeFlyAttack( pOwner );
				m_bAttackKeyDown = false;
				return;
			}
			else	
				m_bAttackKeyDown = false;
		}

		CheckFlyAutoFire( pOwner );

		ProcessFlyMove( pOwner );
		// CheckKeyInput 내부에서 상태가 바뀔수 있다
		CheckKeyInput( pOwner );

		ProcessRotate( pOwner );
		break;
	case DFS_FLY_ATTACK:
		if( m_FlyAttackState == DFAS_DELAY )
		{
			if(	m_dwFireStartTime && m_dwFireStartTime <= dwCurTime )
			{
				FlyFireBallAttackFire( pOwner );
			}
		}
		else if( m_FlyAttackState == DFAS_ATTACK  )
		{
			if( m_dwFireBallAttackEndTime  < dwCurTime )
			{
				//ChangeFlyEnd( pOwner );
				ChangeFlyMove( pOwner );
			}
			/*else if( pOwner->IsNeedSendNetwork() || !pOwner->GetCreator()->IsNetworkMode() || !(P2PNetwork::IsNetworkPlaying()) )
			{
				WaitTargetReturn( pOwner );
			}*/
		}
		else if( m_FlyAttackState == DFAS_ATTACKED )
		{
			if( m_dwMotionStartTime && m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				ChangeFlyEnd( pOwner );
			}
		}
		break;
	case DFS_END:
		break;
	}
}

void ioDevilItem::ProcessRotate( ioBaseChar *pOwner )
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

bool ioDevilItem::ProcessFlyStart( ioBaseChar *pOwner )
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

bool ioDevilItem::ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove )
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
	//if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fCurMoveSpeed = (bOptionMove == true ) ? m_fSpecialAttackSpeed * fTimePerSec : m_CurFly.m_fMoveSpeed * fTimePerSec;
		D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );

	//특공시 up_down은 옵션 처리한다.
	if( bOptionMove == false )
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

void ioDevilItem::CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iFlyState;
	rkPacket >> iFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iFlyState )
	{
	case DFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetFlyState( pOwner, m_AttackFly, false );
			return;
		}
		break;
	}
}

int ioDevilItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioDevilItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioDevilItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioDevilItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioDevilItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioDevilItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioDevilItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioDevilItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioDevilItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioDevilItem::IsDevilFlyState()
{
	switch( m_FlyState )
	{
	case DFS_START:
	case DFS_MOVE:
		return true;
	}

	return false;
}

void ioDevilItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DEVIL_FLY:
		{
			if( GetFlyState() != DFS_START && GetFlyState() != DFS_MOVE &&
				GetFlyState() != DFS_FLY_ATTACK )
				return;

			float fTest = m_fCurGauge;
			float fGauge = m_fDecreaseGauge * fTimePerSec;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseGauge * fTimePerSec;
			}

			m_fCurGauge -= fGauge;
			m_fCurGauge = max( 0.0f, m_fCurGauge );
		}
		break;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		break;
	}
}

void ioDevilItem::ClearState( ioBaseChar *pOwner )
{
	m_FlyState = DFS_NONE;

	m_CurFly.Init();

	m_dwFlyStartTime = 0;
	m_dwFlyMoveTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_dwFlyAttackEndTime = 0;

	m_bAttackKeyDown = false;

	m_dwFlyForceTime = 0;
}

void ioDevilItem::CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_dwKeyReserveTime = 0;
	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwFlyMoveTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwFlyForceTime = dwCurTime + dwTime;

	m_vFlyForceDir = vDir;
}

bool ioDevilItem::IsFlyCharCollisionSkipState()
{
	switch( m_FlyState )
	{
	case DFS_START:
		return true;
	case DFS_MOVE:
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE || m_bSetUpKey || m_bSetDownKey )
			return true;
	}

	return false;
}

void ioDevilItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioDevilItem::OnReleased( ioBaseChar *pOwner )
{
	ClearState( pOwner );
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioDevilItem::ChangeFlyEnd( ioBaseChar *pOwner )
{
	m_FlyState = DFS_END;

	if( pOwner && pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_CurFly.m_fFlyJumpAmt, m_fExtendLandingRate );
		pOwner->ClearDirDoubleClick();
	}
}

void ioDevilItem::CheckFlyAutoFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurTime < m_dwNextFireTime || m_CurFly.m_MoveAttack.m_vWeaponInfoList.size() <= 0 )
		return;
		

	m_dwNextFireTime += m_CurFly.m_dwFlyAttackTimeGap;

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;

	kFireTime.iAttributeIdx = m_CurFly.m_MoveAttack.m_vWeaponInfoList[0].m_iWeaponIdx;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
}

void ioDevilItem::DecreaseFlyDashGauge( float fGauge )
{
	m_fCurGauge = max( 0.f, m_fCurGauge - fGauge );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDevilFlySpecialState::ioDevilFlySpecialState()
{
}

ioDevilFlySpecialState::~ioDevilFlySpecialState()
{
}

void ioDevilFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioDevilFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioDevilItem *pDevilItem = ToDevilItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDevilItem )
		pDevilItem->ClearState( pOwner );
}

bool ioDevilFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioDevilItem *pDevilItem = ToDevilItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDevilItem && pDevilItem->IsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioDevilFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioDevilItem *pDevilItem = ToDevilItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDevilItem )
	{			
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
				fPreHeightForGhostState = pOwner->GetBottomHeight();

			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}

		pDevilItem->ProcessFlyState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioDevilFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioDevilFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioDevilItem *pDevilItem = ToDevilItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pDevilItem )
		return false;

	if( pDevilItem->GetFlyState() == ioDevilItem::DFS_END )
		pOwner->SetState( CS_DELAY );

	return true;
}