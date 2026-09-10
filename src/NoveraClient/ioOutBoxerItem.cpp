#include "StdAfx.h"
#include "ioOutBoxerItem.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"

ioOutBoxerItem::ioOutBoxerItem(void)
{
	ClearData();
	m_fCurBullet = 0.0f;
}

ioOutBoxerItem::ioOutBoxerItem( const ioOutBoxerItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_FirstStepAni( rhs.m_FirstStepAni ),
m_fFirstStepAniRate( rhs.m_fFirstStepAniRate ),
m_MoveAniFr( rhs.m_MoveAniFr ),
m_fMoveAniFrRate( rhs.m_fMoveAniFrRate ),
m_MoveAniBk( rhs.m_MoveAniBk ),
m_fMoveAniBkRate( rhs.m_fMoveAniBkRate ),
m_MoveAniRt( rhs.m_MoveAniRt ),
m_fMoveAniRtRate( rhs.m_fMoveAniRtRate ),
m_MoveAniLt( rhs.m_MoveAniLt ),
m_fMoveAniLtRate( rhs.m_fMoveAniLtRate ),
m_fMoveForce( rhs.m_fMoveForce ),
m_fMoveForceFriction( rhs.m_fMoveForceFriction ),
m_BoxerAttack( rhs.m_BoxerAttack ),
m_SecondStepAni( rhs.m_SecondStepAni ),
m_fSecondStepAniRate( rhs.m_fSecondStepAniRate ),
m_dwEnableSecondStepJumpTime( rhs.m_dwEnableSecondStepJumpTime ),
m_dwSecondStepChargeTime( rhs.m_dwSecondStepChargeTime ),
m_SecondStepChargeTimeAni( rhs.m_SecondStepChargeTimeAni ),
m_SecondStepAttack( rhs.m_SecondStepAttack ),
m_SecondStepDefenseAni( rhs.m_SecondStepDefenseAni ),
m_fSecondStepDefenseAniRate( rhs.m_fSecondStepDefenseAniRate ),
m_iNeedBullet( rhs.m_iNeedBullet ),
m_fMaxBullet( rhs.m_fMaxBullet ),
m_fIncreseGaugeDelayTic( rhs.m_fIncreseGaugeDelayTic ),
m_fIncreseGaugeRunTic( rhs.m_fIncreseGaugeRunTic ),
m_fIncreseGaugeDefaultTic( rhs.m_fIncreseGaugeDefaultTic ),
m_fDecreaseGaugeMoveState( rhs.m_fDecreaseGaugeMoveState ),
m_fDecreaseGaugeBoxerAttackState( rhs.m_fDecreaseGaugeBoxerAttackState ),
m_fDecreaseGaugeSecondStepAttackState( rhs.m_fDecreaseGaugeSecondStepAttackState ),
m_fDecreaseGaugeSecondStepDefenceState( rhs.m_fDecreaseGaugeSecondStepDefenceState ),
m_fDecreaseGaugeSecondStepDefenceWeapon( rhs.m_fDecreaseGaugeSecondStepDefenceWeapon )
{
	ClearData();
	m_fCurBullet = 0.0f;
}

ioOutBoxerItem::~ioOutBoxerItem(void)
{
}

void ioOutBoxerItem::ClearData()
{
	m_OutBoxerChargeState = OBCS_NONE;
	m_BoxerNextStepState = BNSS_NONE;

	m_iCurCombo = 0;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_iCurMovingCount = 0;
	m_vMoveDir = ioMath::VEC3_ZERO;
	m_dwMoveTime = 0;
	m_CurMoveAni.Clear();
	m_fCurMoveAniRate = 0.0f;

	m_dwSecondStepStartTime = 0;
	m_iCurSecondStepDefenseCount = 0;

}

ioWeaponItem::WeaponSubType ioOutBoxerItem::GetSubType() const
{
	return WST_OUTBOXER_ITEM;
}

ioItem* ioOutBoxerItem::Clone()
{
	return new ioOutBoxerItem( *this );
}

void ioOutBoxerItem::LoadProperty( ioINILoader &rkLoader )
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


	//first step
	rkLoader.LoadString( "first_step_ani", "", szBuf, MAX_PATH );
	m_FirstStepAni = szBuf;
	m_fFirstStepAniRate = rkLoader.LoadFloat_e( "first_step_ani_rate", FLOAT1 );

	//move
	rkLoader.LoadString_e( "move_ani_fr", "", szBuf, MAX_PATH );
	m_MoveAniFr = szBuf;
	m_fMoveAniFrRate = rkLoader.LoadFloat_e( "move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "move_ani_bk", "", szBuf, MAX_PATH );
	m_MoveAniBk = szBuf;
	m_fMoveAniBkRate = rkLoader.LoadFloat_e( "move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "move_ani_rt", "", szBuf, MAX_PATH );
	m_MoveAniRt = szBuf;
	m_fMoveAniRtRate = rkLoader.LoadFloat_e( "move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "move_ani_lt", "", szBuf, MAX_PATH );
	m_MoveAniLt = szBuf;
	m_fMoveAniLtRate = rkLoader.LoadFloat_e( "move_ani_rate_lt", FLOAT1 );

	m_fMoveForce = rkLoader.LoadFloat_e( "move_force", 0.0f );
	m_fMoveForceFriction = rkLoader.LoadFloat_e( "move_force_friction", 0.0f );

	//Attack
	LoadAttackAttribute( "boxer_attack", m_BoxerAttack, rkLoader );

	//second step
	rkLoader.LoadString( "second_step_ani", "", szBuf, MAX_PATH );
	m_SecondStepAni = szBuf;
	m_fSecondStepAniRate = rkLoader.LoadFloat_e( "second_step_ani_rate", FLOAT1 );
	m_dwEnableSecondStepJumpTime = rkLoader.LoadInt_e( "enable_second_step_jump_time", 0 );

	//second step charging
	m_dwSecondStepChargeTime = (DWORD)rkLoader.LoadInt_e( "second_step_charge_time", 0 );
	rkLoader.LoadString( "second_step_charge_time_ani", "", szBuf, MAX_PATH );
	m_SecondStepChargeTimeAni = szBuf;

	//Second Step Attack
	LoadAttackAttribute( "second_step_attack", m_SecondStepAttack, rkLoader );

	//Second Step Defense
	rkLoader.LoadString( "second_step_defense_ani", "", szBuf, MAX_PATH );
	m_SecondStepDefenseAni = szBuf;
	m_fSecondStepDefenseAniRate = rkLoader.LoadFloat_e( "second_step_defense_ani_rate", FLOAT1 );

	//Gauge
	m_iNeedBullet = rkLoader.LoadInt_e( "need_bullet", 0 );
	m_fMaxBullet = rkLoader.LoadFloat_e( "max_bullet", 0.0f );

	m_fIncreseGaugeDelayTic = rkLoader.LoadFloat_e( "increase_gauge_delay_tic", 0.0f );
	m_fIncreseGaugeRunTic = rkLoader.LoadFloat_e( "increase_gauge_run_tic", 0.0f );
	m_fIncreseGaugeDefaultTic = rkLoader.LoadFloat_e( "increase_gauge_default_tic", 0.0f );

	m_fDecreaseGaugeMoveState = rkLoader.LoadFloat_e( "decrease_gauge_move_state", 0.0f );
	m_fDecreaseGaugeBoxerAttackState = rkLoader.LoadFloat_e( "decrease_gauge_boxer_attack_state", 0.0f );
	m_fDecreaseGaugeSecondStepAttackState = rkLoader.LoadFloat_e( "decrease_gauge_second_step_attack_state", 0.0f );
	m_fDecreaseGaugeSecondStepDefenceState = rkLoader.LoadFloat_e( "decrease_gauge_second_step_defense_state", 0.0f );
	m_fDecreaseGaugeSecondStepDefenceWeapon = rkLoader.LoadFloat_e( "decrease_gauge_second_step_defense_weapon", 0.0f );
}

void ioOutBoxerItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	ClearData();
}

void ioOutBoxerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_OutBoxerChargeState = OBCS_NONE;
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

		m_OutBoxerChargeState = OBCS_CHARGING;
		m_dwMotionStartTime = FRAMEGETTIME();
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

bool ioOutBoxerItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioOutBoxerItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
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

void ioOutBoxerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_OutBoxerChargeState )
	{
	case OBCS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case OBCS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case OBCS_FIRST_STEP:
		ProcessFirstStepState( pOwner );
		break;
	case OBCS_BOXER_ATTACK:
		ProcessBoxerAttackState( pOwner );
		break;
	case OBCS_MOVE:
		ProcessMoveState( pOwner );
		break;
	case OBCS_SECOND_STEP:
		ProcessSecondStepState( pOwner );
		break;
	case OBCS_SECOND_STEP_CHARGING:
		ProcessSecondStepCharging( pOwner );
		break;
	case OBCS_SECOND_STEP_ATTACK:
		ProcessSecondStepAttackState( pOwner );
		break;
	case OBCS_SECOND_STEP_DEFENSE:
		ProcessSecondStepDefenseState( pOwner );
		break;
	}
}

void ioOutBoxerItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && (int)m_fCurBullet > 0 )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToFirstStepState( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
		return;
	}
}

void ioOutBoxerItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_OutBoxerChargeState = OBCS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_OutBoxerChargeState );
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOutBoxerItem::ChangeToFirstStepState( ioBaseChar *pOwner )
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

	int iAniID	     = pGrp->GetAnimationIdx( m_FirstStepAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fFirstStepAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_OutBoxerChargeState = OBCS_FIRST_STEP;

	m_BoxerNextStepState = BNSS_FIRST_STEP;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_OutBoxerChargeState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOutBoxerItem::ProcessFirstStepState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != ioUserKeyInput::DKI_NONE )
	{
		ChangeToMoveState( pOwner, eNewDirKey );
		return;
	}

	//Attack State
	if ( !pOwner->IsAttackKeyDown() )
	{
		ChangeToBoxerAttackState( pOwner, false );
		return;
	}
}

void ioOutBoxerItem::ChangeToMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey )
{
	if( !pOwner )
		return;

	m_iCurMovingCount++;

	m_CurMoveAni.Clear();
	m_fCurMoveAniRate = 0.0f;

	CheckMoveAni( pOwner, eDirKey );

	m_OutBoxerChargeState = OBCS_MOVE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_OutBoxerChargeState );
		kPacket <<(int)eDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fDecreaseGaugeMoveState );
}

void ioOutBoxerItem::CheckMoveAni( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey )
{
	DWORD dwCurTime = FRAMEGETTIME();

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = eDirKey;
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
	case 7:
		if( !m_MoveAniFr.IsEmpty() )
			SetMoveAni( pOwner, m_MoveAniFr, m_fMoveAniFrRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_MoveAniRt.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniRt, m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_MoveAniFr.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniFr, m_fMoveAniFrRate, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_MoveAniRt.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniRt, m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_MoveAniBk.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniBk, m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_MoveAniBk.IsEmpty() )
			SetMoveAni( pOwner, m_MoveAniBk, m_fMoveAniBkRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_MoveAniLt.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniLt, m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_MoveAniBk.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniBk, m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_MoveAniLt.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniLt, m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_MoveAniFr.IsEmpty() )
				SetMoveAni( pOwner, m_MoveAniFr, m_fMoveAniFrRate, vMoveDir );
		}
		break;
	}
}

void ioOutBoxerItem::SetMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	ClearCancelInfo();

	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	if ( iAniID == - 1)
		return;

	m_CurMoveAni = szAni;
	m_fCurMoveAniRate = fAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->ClearCharColSkipTime();
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += FRAMEGETTIME();

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwMoveTime = dwCurTime + dwTime;

	SetCurAttackAniForInputJump( szAni, fAniRate );

	m_vMoveDir = vDir;
}

void ioOutBoxerItem::ProcessMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() )
	{
		if ( m_dwMotionEndTime < dwCurTime )
		{
			if ( (int)m_fCurBullet > 0 )
				ChangeToNextStep( pOwner );
			else
				ChangeToEndState( pOwner );
			return;
		}

		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;

		if ( m_BoxerNextStepState == BNSS_FIRST_STEP )
		{
			if ( !pOwner->IsAttackKeyDown() )
			{
				ChangeToBoxerAttackState( pOwner, false );
				return;
			}
		}
		else if ( m_BoxerNextStepState == BNSS_SECOND_STEP )
		{
			if ( pOwner->IsAttackKey() && (int)m_fCurBullet > 0 )
			{
				ChangeToSecondStepChargeState( pOwner );
				return;
			}
		}
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && (int)m_fCurBullet > 0 )
		{
			ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
			if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			{
				ChangeToMoveState( pOwner, eNewDirKey );
				return;
			}
		}
	}

	if( m_dwMoveTime > 0 && m_dwMoveTime < FRAMEGETTIME() )
	{
		m_dwMoveTime = 0;
		pOwner->SetForcePower( m_vMoveDir, m_fMoveForce, m_fMoveForceFriction );
	}
}

void ioOutBoxerItem::ChangeToNextStep( ioBaseChar *pOwner )
{
	if ( m_BoxerNextStepState == BNSS_FIRST_STEP )
	{
		ChangeToFirstStepState( pOwner );
		return;
	}
	else if ( m_BoxerNextStepState == BNSS_SECOND_STEP )
	{
		ChangeToSecondStepState( pOwner );
		return;
	}
}

void ioOutBoxerItem::ChangeToBoxerAttackState( ioBaseChar *pOwner, bool bReduceGauge )
{
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_OutBoxerChargeState = OBCS_BOXER_ATTACK;

	m_BoxerNextStepState = BNSS_SECOND_STEP;
	
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_BoxerAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;
	float fTimeRate = m_BoxerAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_BoxerAttack.m_dwPreDelay;

	pOwner->RefreshFireTimeList( iAniID, m_BoxerAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	pOwner->SetReservedSliding( m_BoxerAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + dwPreDelay; 
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_dwMotionEndTime += m_BoxerAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwPreDelay + dwCurTime;

	SetCurAttackAniForInputJump( m_BoxerAttack.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = FRAMEGETTIME();
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if ( bReduceGauge )
		DecreaseGauge( m_fDecreaseGaugeBoxerAttackState );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_OutBoxerChargeState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOutBoxerItem::ProcessBoxerAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
	{
		if ( (int)m_fCurBullet > 0 )
			ChangeToSecondStepState( pOwner );
		else
			ChangeToEndState( pOwner );
		return;
	}

	CheckCancelReserve( pOwner );

	if( ProcessCancel(pOwner) )
		return;

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && (int)m_fCurBullet > 0 )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
		{
			ChangeToMoveState( pOwner, eNewDirKey );
			return;
		}

		CheckReserve( pOwner );
	}

	if ( ProcessBoxerAttackReserveKeyInput( pOwner ) )
		return;
}

bool ioOutBoxerItem::ProcessBoxerAttackReserveKeyInput( ioBaseChar *pOwner )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return true;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		ChangeToSecondStepDefenseState( pOwner );
		return true;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return true;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ChangeToSecondStepChargeState( pOwner );
		return true;
	}

	return false;
}

void ioOutBoxerItem::ChangeToSecondStepState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_SecondStepAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fSecondStepAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_OutBoxerChargeState = OBCS_SECOND_STEP;

	m_dwSecondStepStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_OutBoxerChargeState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOutBoxerItem::ProcessSecondStepState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_dwSecondStepStartTime + m_dwEnableSecondStepJumpTime < FRAMEGETTIME() )
	{
		if ( pOwner->IsJumpKey() )
			pOwner->SetJumpState();
	}

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != ioUserKeyInput::DKI_NONE )
	{
		ChangeToMoveState( pOwner, eNewDirKey );
		return;
	}

	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	if ( pOwner->IsAttackKey() )
	{
		ChangeToSecondStepChargeState( pOwner );
		return;
	}

	if ( pOwner->IsDefenseKey() )
	{
		ChangeToSecondStepDefenseState( pOwner );
		return;
	}

}

void ioOutBoxerItem::ChangeToSecondStepChargeState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	ClearKeyReserve();

	int iAniID = pGrp->GetAnimationIdx( m_SecondStepChargeTimeAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = (float)m_dwSecondStepChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_OutBoxerChargeState = OBCS_SECOND_STEP_CHARGING;
	m_dwMotionStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_OutBoxerChargeState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOutBoxerItem::ProcessSecondStepCharging( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwSecondStepChargeTime <= FRAMEGETTIME() )
		{
			ChangeToBoxerAttackState( pOwner, true );
			return;
		}
	}
	else
	{
		ChangeToSecondStepAttackState( pOwner );
		return;
	}
}

void ioOutBoxerItem::ChangeToSecondStepAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_OutBoxerChargeState = OBCS_SECOND_STEP_ATTACK;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	int iAniID = pGrp->GetAnimationIdx( m_SecondStepAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;
	float fTimeRate = m_SecondStepAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_SecondStepAttack.m_dwPreDelay;

	pOwner->RefreshFireTimeList( iAniID, m_SecondStepAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	pOwner->SetReservedSliding( m_SecondStepAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + dwPreDelay; 
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_dwMotionEndTime += m_SecondStepAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwPreDelay + dwCurTime;

	SetCurAttackAniForInputJump( m_SecondStepAttack.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = FRAMEGETTIME();
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_OutBoxerChargeState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fDecreaseGaugeSecondStepAttackState );
}

void ioOutBoxerItem::ProcessSecondStepAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
	{
		if ( (int)m_fCurBullet > 0 )
			ChangeToSecondStepState( pOwner );
		else
			ChangeToEndState( pOwner );
		return;
	}

	CheckCancelReserve( pOwner );
	if( ProcessCancel(pOwner) )
		return;

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && (int)m_fCurBullet > 0 )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
		{
			ChangeToMoveState( pOwner, eNewDirKey );
			return;
		}
	}
}

void ioOutBoxerItem::ChangeToSecondStepDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearKeyReserve();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_SecondStepDefenseAni );
	if ( iAniID == - 1)
		return;

	float fTimeRate = m_fSecondStepDefenseAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	SetCurAttackAniForInputJump( m_SecondStepDefenseAni, fTimeRate );

	m_OutBoxerChargeState = OBCS_SECOND_STEP_DEFENSE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_OutBoxerChargeState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	DecreaseGauge( m_fDecreaseGaugeSecondStepDefenceState );
	m_iCurSecondStepDefenseCount++;

}

void ioOutBoxerItem::ProcessSecondStepDefenseState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
	{
		if ( (int)m_fCurBullet > 0 )
			ChangeToSecondStepState( pOwner );
		else
			ChangeToEndState( pOwner );
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && (int)m_fCurBullet > 0 )
	{
		if ( pOwner->IsDefenseKey() )
		{
			ChangeToSecondStepDefenseState( pOwner );
			return;
		}

		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
		{
			ChangeToMoveState( pOwner, eNewDirKey );
			return;
		}
	}
}

void ioOutBoxerItem::ChangeToEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetState( CS_DELAY );
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( OBCS_END );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioOutBoxerItem::IsEnableChargeDefenseCheck()
{
	if ( m_OutBoxerChargeState == OBCS_SECOND_STEP_DEFENSE )
		return true;
	return false;
}

void ioOutBoxerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		{
			if( m_fCurBullet < m_fMaxBullet )
			{
				float fGauge = m_fIncreseGaugeDelayTic * fTimePerSec;

				m_fCurBullet += fGauge;
				m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
			}
		}
		return;
	case CS_DASH:
		{
			if( m_fCurBullet < m_fMaxBullet )
			{
				float fGauge = m_fIncreseGaugeRunTic * fTimePerSec;

				m_fCurBullet += fGauge;
				m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
			}
		}
		return;
	case CS_ATTACK:
		{
			if ( m_OutBoxerChargeState == OBCS_NORMAL_ATTACK )
			{
				if( m_fCurBullet < m_fMaxBullet )
				{
					float fGauge = m_fIncreseGaugeDefaultTic * fTimePerSec;

					m_fCurBullet += fGauge;
					m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
				}
			}
		}
		return;
	default:
		{
			if( m_fCurBullet < m_fMaxBullet )
			{
				float fGauge = m_fIncreseGaugeDefaultTic * fTimePerSec;

				m_fCurBullet += fGauge;
				m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
			}
		}
		return;
	}
}

void ioOutBoxerItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurBullet -= fNeedGauge;
	m_fCurBullet = (float)max( 0, min( m_fCurBullet, m_fMaxBullet ) );
}

void ioOutBoxerItem::CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner )
{
	if( m_OutBoxerChargeState == OBCS_SECOND_STEP_DEFENSE )
		DecreaseGauge( m_fDecreaseGaugeSecondStepDefenceWeapon );
}

void ioOutBoxerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btState;
	rkPacket >> btState;
	if ( btState == OBCS_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( btState == OBCS_FIRST_STEP )
		ChangeToFirstStepState( pOwner );
	else if ( btState == OBCS_MOVE )
	{
		int iDirKey;
		rkPacket >> iDirKey;
		ChangeToMoveState( pOwner, (ioUserKeyInput::DirKeyInput )iDirKey );
	}
	else if ( btState == OBCS_BOXER_ATTACK )
		ChangeToBoxerAttackState( pOwner, false );
	else if ( btState == OBCS_SECOND_STEP )
		ChangeToSecondStepState( pOwner );
	else if ( btState == OBCS_SECOND_STEP_CHARGING )
		ChangeToSecondStepChargeState( pOwner );
	else if ( btState == OBCS_SECOND_STEP_ATTACK )
		ChangeToSecondStepAttackState( pOwner );
	else if ( btState == OBCS_SECOND_STEP_DEFENSE )
		ChangeToSecondStepChargeState( pOwner );
	else if ( btState == OBCS_END )
		pOwner->SetState( CS_DELAY );
	
}

void ioOutBoxerItem::GetAnimationNameByState( OutBoxerChargeState state, ioHashString& szAni, float& fRate )
{
	switch( state )
	{
	case OBCS_FIRST_STEP:
		{
			szAni = m_FirstStepAni;
			fRate = m_fFirstStepAniRate;
		}
		return;
	case OBCS_MOVE:
		{
			szAni = m_CurMoveAni;
			fRate = m_fCurMoveAniRate;
		}
		return;
	case OBCS_SECOND_STEP:
		{
			szAni = m_SecondStepAni;
			fRate = m_fSecondStepAniRate;
		}
		return;
	case OBCS_SECOND_STEP_DEFENSE:
		{
			szAni = m_SecondStepDefenseAni;
			fRate = m_fSecondStepDefenseAniRate;
		}
		return;
	}
}

void ioOutBoxerItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKey() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
}

bool ioOutBoxerItem::IsNoDropState( ioBaseChar *pOwner )
{
	if ( m_OutBoxerChargeState == OBCS_MOVE )
		return true;

	return false;
}

void ioOutBoxerItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}
