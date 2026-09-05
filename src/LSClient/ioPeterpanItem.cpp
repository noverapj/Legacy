
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


#include "ioPeterpanItem.h"

#include "WeaponDefine.h"

ioPeterpanItem::ioPeterpanItem()
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

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;

	m_dwFlyForceTime = 0;

	m_FlyState = PFS_NONE;
	m_dwEnableEffect = 0;
	m_dwFlyMoveEffect = 0;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwFlyDashAttackWeaponIndex = 0;
	m_dwAreaWeaponIndex = 0;

	m_dwWarningLandTime = 0;
	m_bWarningLanding = false;
}

ioPeterpanItem::ioPeterpanItem( const ioPeterpanItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackFly( rhs.m_AttackFly ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
	m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
	m_fHighDecreaseGauge( rhs.m_fHighDecreaseGauge ),
	m_EnableEffect( rhs.m_EnableEffect ),
	//m_DashAttack( rhs.m_DashAttack ),
	m_fFlyAttackMoveSpeed( rhs.m_fFlyAttackMoveSpeed ),
	m_dwAreaWeaponName( rhs.m_dwAreaWeaponName )
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

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;

	m_dwFlyForceTime = 0;

	m_FlyState = PFS_NONE;
	m_dwEnableEffect = 0;
	m_dwFlyMoveEffect = 0;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0; 
	m_dwFireStartTime = 0;
	m_dwFlyDashAttackWeaponIndex = 0;
	m_dwAreaWeaponIndex = 0;
	m_dwWarningLandTime = 0;
	m_bWarningLanding = false;
}

ioPeterpanItem::~ioPeterpanItem()
{
}

void ioPeterpanItem::LoadProperty( ioINILoader &rkLoader )
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

	rkLoader.LoadString_e( "attack_area_weapon_name", "", szBuf, MAX_PATH );
	m_dwAreaWeaponName = szBuf;

	//m_DashAttack.Init();
	//LoadAttackAttribute_e( "fly_dash_jump_attack", m_DashAttack, rkLoader );

	LoadFlyInfo( rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGauge = rkLoader.LoadFloat_e( "high_decrease_gauge", 3.0f );

	rkLoader.LoadString_e( "enable_effect", "", szBuf, MAX_PATH );
	m_EnableEffect = szBuf;
}

void ioPeterpanItem::LoadFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_AttackFly.Init();

	rkLoader.LoadString_e( "fly_effect", "", szBuf, MAX_PATH );
	m_AttackFly.m_FlyEffect = szBuf;

	rkLoader.LoadString_e( "fly_move_effect", "", szBuf, MAX_PATH );
	m_AttackFly.m_FlyMoveEffect = szBuf;

	rkLoader.LoadString_e( "fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_StartAni = szBuf;
	m_AttackFly.m_fStartAniRate = rkLoader.LoadFloat_e( "fly_start_ani_rate", FLOAT1 );

	m_AttackFly.m_fStartAngle = rkLoader.LoadFloat_e( "fly_start_angle", 0.0f );
	m_AttackFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "fly_start_max_range", 0.0f );
	m_AttackFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "fly_start_duration", 0 );

	rkLoader.LoadString_e( "fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_MoveAni = szBuf;
	m_AttackFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "fly_move_ani_rate", FLOAT1 );

	m_AttackFly.m_fSpeed = rkLoader.LoadFloat_e( "peterpan_fly_speed", 0.0f );
	m_AttackFly.m_fFrictionAmt = rkLoader.LoadFloat_e( "peterpan_fly_frictionamt", 0.0f );
	m_AttackFly.m_fGravityAmt = rkLoader.LoadFloat_e( "peterpan_fly_gravityamt", 0.0f );
	m_AttackFly.m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "peterpan_fly_rotatespeed", 0 );
	m_AttackFly.m_fOffHeight = rkLoader.LoadFloat_e( "peterpan_fly_offheight", 0.0f );
	m_AttackFly.m_dwHeightOffTime = (DWORD)rkLoader.LoadInt_e( "peterpan_fly_offtime", 0 );
	m_AttackFly.m_fJumpPower = rkLoader.LoadFloat_e( "peterpan_fly_jumppower", 0);

	m_AttackFly.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "fly_jump_amt", 0.0f );
}

ioItem* ioPeterpanItem::Clone()
{
	return new ioPeterpanItem( *this );
}

ioWeaponItem::WeaponSubType ioPeterpanItem::GetSubType() const
{
	return WST_PETERPAN;
}

void ioPeterpanItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioPeterpanItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioPeterpanItem::OnCharging( ioBaseChar *pOwner )
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

void ioPeterpanItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioPeterpanItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioPeterpanItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;

			SetEnableEffect( pOwner );
			SetCurGauge( fGauge );
		}
		break;
	case SST_FLY_STATE:
		{
			CheckFlyState( pOwner, rkPacket );
		}
		break;
	case SST_FLY_JUMP:
		{
			pOwner->SetJumpPower( m_CurFly.m_fJumpPower );
		}
		break;
	case SST_FLY_END:
		{
			ChangeFlyEnd( pOwner );
		}
		break;
	}
}

void ioPeterpanItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioPeterpanItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioPeterpanItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioPeterpanItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioPeterpanItem::GetAutoTargetValue( ioBaseChar *pOwner,
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


void ioPeterpanItem::SetFlyState( ioBaseChar *pOwner, const PeterpanFlyInfo &rkFlyInfo, bool bSendNet )
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

	pOwner->SetState( CS_PETERPAN_FLY );
	ReleaseEnableEffect( pOwner );
	ReleaseFlyMoveEffect( pOwner );

	m_CurFly = rkFlyInfo;

	if( !m_CurFly.m_FlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurFly.m_FlyEffect, false );
		pOwner->AttachEffect( m_CurFly.m_FlyEffect );
	}

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

void ioPeterpanItem::ChangeFlyStart( ioBaseChar *pOwner )
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

	m_FlyState = PFS_START;

	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_dwFlyForceTime = 0;
}

void ioPeterpanItem::ChangeFlyMove( ioBaseChar *pOwner )
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

	int iAniID	     = pGrp->GetAnimationIdx( m_CurFly.m_MoveAni );
	float fTimeRate  = m_CurFly.m_fMoveAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_FlyState = PFS_MOVE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_dwFlyForceTime = 0;

	//////////////////////////////////////////////////////////////////////////
	pOwner->SetFlyStateValue( "",
		0, m_CurFly.m_fSpeed,
		m_CurFly.m_fFrictionAmt, m_CurFly.m_fGravityAmt,
		m_CurFly.m_dwRotateSpeed,
		m_CurFly.m_fOffHeight,
		m_CurFly.m_dwHeightOffTime,
		FMT_PETERPAN );

	pOwner->SetCurMoveSpeed( pOwner->GetMaxSpeed() );

	D3DXQUATERNION qtRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
		pOwner->GetCharName(),
		"",
		m_dwAreaWeaponName,
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		qtRot,
		ioAreaWeapon::CT_TARGET_ATTACHED );

	if( pAreaWeapon )
	{
		m_dwAreaWeaponIndex = pAreaWeapon->GetAreaWeaponIndex();
	}
}

void ioPeterpanItem::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FlyState )
	{
	case PFS_START:
		m_bWarningLanding = false;
		pOwner->SetCurMoveSpeed( 0 );

		if( m_dwFlyStartTime+m_CurFly.m_dwStartDuration < dwCurTime )
		{
			ChangeFlyMove( pOwner );
			return;
		}
		else if( !ProcessFlyStart( pOwner ) )
		{
			ChangeFlyMove( pOwner );
			return;
		}
		break;
	case PFS_MOVE:
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
					m_TargetSkill = m_Target;
					if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
						return;
				}

				m_TargetSkill.Clear();
			}
		}

		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
		{
			ChangeFlyEnd( pOwner );
			return;
		}

		if( pOwner->IsNeedProcess() && pOwner->IsJumpKey() )
		{
			pOwner->SetJumpPower( m_CurFly.m_fJumpPower );
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_FLY_JUMP;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		//지면에 닿는지 체크
		if( pOwner->IsNeedProcess() )
		{
			float fHeightGap = 0.0f;
			fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
			if( fHeightGap > m_CurFly.m_fOffHeight )
			{
				m_bWarningLanding = false;
				m_dwWarningLandTime = dwCurTime;
			}
			else 
			{
				if( !m_bWarningLanding )
				{
					m_dwWarningLandTime = dwCurTime;
					m_bWarningLanding = true;
				}
				else
				{
					if( (dwCurTime - m_dwWarningLandTime ) > m_CurFly.m_dwHeightOffTime )
					{
						ChangeFlyEnd( pOwner );
						return;
					}
				}
			}
		}
		break;

	case PFS_END:
		break;
	}
}

bool ioPeterpanItem::ProcessFlyStart( ioBaseChar *pOwner )
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

void ioPeterpanItem::CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iFlyState;
	rkPacket >> iFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iFlyState )
	{
	case PFS_START:
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

int ioPeterpanItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioPeterpanItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioPeterpanItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioPeterpanItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioPeterpanItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioPeterpanItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioPeterpanItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioPeterpanItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioPeterpanItem::SetEnableEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableEffect == 0 && !m_EnableEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_EnableEffect );
		if( pEffect )
		{
			m_dwEnableEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << SST_EFFECT;
			kPacket << m_fCurGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioPeterpanItem::ReleaseEnableEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableEffect != 0 )
		pChar->EndEffect( m_dwEnableEffect, false );

	m_dwEnableEffect = 0;
}

void ioPeterpanItem::CheckEnableGauge( ioBaseChar *pChar )
{
	if( m_fCurGauge >= m_fNeedGauge )
		SetEnableEffect( pChar );
}

bool ioPeterpanItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioPeterpanItem::IsPeterpanState()
{
	switch( m_FlyState )
	{
	case PFS_START:
	case PFS_MOVE:
		return true;
	}

	return false;
}

void ioPeterpanItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_PETERPAN_FLY:
		{
			if( GetFlyState() != PFS_START && GetFlyState() != PFS_MOVE )
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
		return;
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

void ioPeterpanItem::ClearState( ioBaseChar *pOwner )
{
	m_FlyState = PFS_NONE;

	// 순서중요
	if( pOwner && !m_CurFly.m_FlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurFly.m_FlyEffect, false );
	}

	ReleaseFlyMoveEffect( pOwner );

	m_CurFly.Init();

	m_dwFlyStartTime = 0;
	m_dwFlyMoveTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_dwFlyAttackEndTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_dwFlyDashAttackWeaponIndex = 0;

	m_dwFlyForceTime = 0;

	m_Target.Clear();

	DestroyAreaWeapon();
}

void ioPeterpanItem::CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
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

bool ioPeterpanItem::IsFlyCharCollisionSkipState()
{
	switch( m_FlyState )
	{
	case PFS_START:
	case PFS_MOVE:
		return true;
	}

	return false;
}

void ioPeterpanItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;
}

void ioPeterpanItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseEnableEffect( pOwner );
	ReleaseFlyMoveEffect( pOwner );

	ClearState( pOwner );
	m_TargetSkill.Clear();
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioPeterpanItem::ChangeFlyEnd( ioBaseChar *pOwner )
{
	m_FlyState = PFS_END;
	ReleaseFlyMoveEffect( pOwner );

	//if( pOwner && pOwner->IsNeedProcess() )
	{
		//pOwner->SetExtendAttackEndJump( m_CurFly.m_fFlyJumpAmt, m_fExtendLandingRate );
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->SetSKillEndJumpState( m_CurFly.m_fFlyJumpAmt, false, false, false, true );
	}

	if( pOwner->IsNeedProcess() )
	{
		pOwner->ClearDirDoubleClick();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

const ioHashString& ioPeterpanItem::GetTargetSkill()
{
	return m_TargetSkill;
}

void ioPeterpanItem::ClearTargetSkill()
{
	m_TargetSkill.Clear();
}

bool ioPeterpanItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioPeterpanItem::SetFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

	if( !m_CurFly.m_FlyMoveEffect.IsEmpty() )
	{
		ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_CurFly.m_FlyMoveEffect, m_vFlyForceDir );
		if( pMapEffect )
		{
			m_dwFlyMoveEffect = pMapEffect->GetUniqueID();
		}
	}
}

void ioPeterpanItem::UpdateFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect == 0 )
		return;

	pChar->UpdateAttachMapEffect( m_dwFlyMoveEffect, true );
}

void ioPeterpanItem::ReleaseFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

}

void ioPeterpanItem::InitPeterpanGauge()
{
	m_fCurGauge = 0.0f;
}

void ioPeterpanItem::DestroyAreaWeapon()
{
	if( m_dwAreaWeaponIndex > 0 )
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );

	m_dwAreaWeaponIndex = 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioPeterpanFlySpecialState::ioPeterpanFlySpecialState()
{
}

ioPeterpanFlySpecialState::~ioPeterpanFlySpecialState()
{
}

void ioPeterpanFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioPeterpanFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioPeterpanItem *pPeterpanItem = ToPeterpanItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPeterpanItem )
		pPeterpanItem->ClearState( pOwner );
}

bool ioPeterpanFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioPeterpanItem *pPeterpan = ToPeterpanItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pPeterpan && pPeterpan->IsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioPeterpanFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioPeterpanItem *pPeterpan = ToPeterpanItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPeterpan )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
				fPreHeightForGhostState = pOwner->GetBottomHeight();

			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}

		pPeterpan->ProcessFlyState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioPeterpanFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioPeterpanFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPeterpanItem *pPeterpan = ToPeterpanItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pPeterpan )
		return true;

	if( pPeterpan->GetFlyState() == ioPeterpanItem::PFS_END )
	{
		pOwner->SetState( CS_DELAY );
		return true;
	}

	if( pPeterpan->GetFlyState() == ioPeterpanItem::PFS_MOVE )
		return false;

	return true;
}