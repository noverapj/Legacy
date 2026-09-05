
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMephistoItem.h"
#include "ioZoneEffectWeapon.h"

ioMephistoItem::ioMephistoItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_dwAttackEndTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCurMephistoGauge = 0.0f;

	m_dwMephistoChargeCheckTime = 0;
	m_iMephistoDashAttackCount = 0;

	m_fMephistoFlyCurRange = 0;
	m_dwMephistoFlyStartTime = 0;

	m_szCreateDummyChar.Clear();
	m_iDummyCharIndex = 0;

	m_DirKey = ioUserKeyInput::DKI_NONE;
}

ioMephistoItem::ioMephistoItem( const ioMephistoItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_fMaxMephistoGauge( rhs.m_fMaxMephistoGauge ),
m_fDecreaseGaugeOnDash( rhs.m_fDecreaseGaugeOnDash ),
m_fDecreaseGaugeOnDashAttack( rhs.m_fDecreaseGaugeOnDashAttack ),
m_fDecreaseGaugeOnFly( rhs.m_fDecreaseGaugeOnFly ),
m_fDecreaseGaugeOnFlying( rhs.m_fDecreaseGaugeOnFlying ),
m_fIncreaseGaugeTime( rhs.m_fIncreaseGaugeTime ),
m_MephistoDashAttack( rhs.m_MephistoDashAttack ),
m_ChargeDashAnimation( rhs.m_ChargeDashAnimation ),
m_fMephistoDashSpeed( rhs.m_fMephistoDashSpeed ),
m_dwMephistoChargeTime( rhs.m_dwMephistoChargeTime ),
m_MephistoChargeEffect( rhs.m_MephistoChargeEffect ),
m_fMephistoChargeOffset( rhs.m_fMephistoChargeOffset ),
m_vMephistoChargeEffectAngle( rhs.m_vMephistoChargeEffectAngle ),
m_fMephistoDashAniRate( rhs.m_fMephistoDashAniRate ),
m_ChargeJumpAnimation( rhs.m_ChargeJumpAnimation ),
m_fChargeJumpAniRate( rhs.m_fChargeJumpAniRate ),
m_fMephistoJumpForce( rhs.m_fMephistoJumpForce ),
m_fMephistoJumpLookForce( rhs.m_fMephistoJumpLookForce ),
m_MephistoFlyAttack( rhs.m_MephistoFlyAttack ),
m_MephistoFlyStartAnimation( rhs.m_MephistoFlyStartAnimation ),
m_MephistoFlyStartEffect( rhs.m_MephistoFlyStartEffect ),
m_MephistoFlyDelayAnimation( rhs.m_MephistoFlyDelayAnimation ),
m_MephistoFlyMoveAnimation( rhs.m_MephistoFlyMoveAnimation ),
m_MephistoFlyUpAnimation( rhs.m_MephistoFlyUpAnimation ),
m_MephistoFlyDownAnimation( rhs.m_MephistoFlyDownAnimation ),
m_fMephistoFlyStartAniRate( rhs.m_fMephistoFlyStartAniRate ),
m_fMephistoFlyStartAngle( rhs.m_fMephistoFlyStartAngle ),
m_fMephistoFlyStartMaxRange( rhs.m_fMephistoFlyStartMaxRange ),
m_dwMephistoFlyStartDuration( rhs.m_dwMephistoFlyStartDuration ),
m_fMephistoFlyDelayAniRate( rhs.m_fMephistoFlyDelayAniRate ),
m_fMephistoFlyMoveAniRate( rhs.m_fMephistoFlyMoveAniRate ),
m_fMephistoFlyUpAniRate( rhs.m_fMephistoFlyUpAniRate ),
m_fMephistoFlyDownAniRate( rhs.m_fMephistoFlyDownAniRate ),
m_fMephistoFlyMoveSpeed( rhs.m_fMephistoFlyMoveSpeed ),
m_dwMephistoFlyDuration( rhs.m_dwMephistoFlyDuration ),
m_bEnableMephistoFlyOnBlowState( rhs.m_bEnableMephistoFlyOnBlowState ),
m_bEnableMephistoFlyOnDefenseState( rhs.m_bEnableMephistoFlyOnDefenseState ),
m_bMephistoFlyCheckWounded( rhs.m_bMephistoFlyCheckWounded ),
m_bMephistoFlyCheckBlowWounded( rhs.m_bMephistoFlyCheckBlowWounded ),
m_bMephistoFlyCheckBoundBlowWounded( rhs.m_bMephistoFlyCheckBoundBlowWounded ),
m_bMephistoFlyCheckDown( rhs.m_bMephistoFlyCheckDown ),
m_fMephistoFlyAlphaRate( rhs.m_fMephistoFlyAlphaRate ),
m_fMephistoFlyEndJumpAmt( rhs.m_fMephistoFlyEndJumpAmt ),
m_szDummyChar( rhs.m_szDummyChar ),
m_szDummyCharOnWounded( rhs.m_szDummyCharOnWounded )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_dwAttackEndTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCurMephistoGauge = 0.0f;

	m_dwMephistoChargeCheckTime = 0;
	m_iMephistoDashAttackCount = 0;

	m_fMephistoFlyCurRange = 0;
	m_dwMephistoFlyStartTime = 0;

	m_szCreateDummyChar.Clear();
	m_iDummyCharIndex = 0;

	m_DirKey = ioUserKeyInput::DKI_NONE;
}

ioMephistoItem::~ioMephistoItem()
{
}

void ioMephistoItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	int iCnt = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0 ; i<iCnt; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_fMaxMephistoGauge          = rkLoader.LoadFloat_e( "max_mephisto_gauge", 0 );
	m_fDecreaseGaugeOnDash       = rkLoader.LoadFloat_e( "decrease_gauge_on_dash", 0 );
	m_fDecreaseGaugeOnDashAttack = rkLoader.LoadFloat_e( "decrease_gauge_on_dash_attack", 0 );
	m_fDecreaseGaugeOnFly        = rkLoader.LoadFloat_e( "decrease_gauge_on_fly", 0 );
	m_fDecreaseGaugeOnFlying     = rkLoader.LoadFloat_e( "decrease_gauge_on_flying", 0 );
	m_fIncreaseGaugeTime         = rkLoader.LoadFloat_e( "increase_gauge_time", 0.0f );

	LoadAttackAttribute( "mephisto_dash_attack", m_MephistoDashAttack, rkLoader );

	rkLoader.LoadString_e( "mephisto_dash_animation", "", szBuf, MAX_PATH );
	m_ChargeDashAnimation  = szBuf;
	m_fMephistoDashAniRate = rkLoader.LoadFloat_e( "mephisto_dash_ani_rate", FLOAT1 );
	m_fMephistoDashSpeed   = rkLoader.LoadFloat_e( "mephisto_dash_speed", FLOAT1 );
	
	m_dwMephistoChargeTime = (DWORD)rkLoader.LoadInt_e( "mephisto_dash_attack_charge_time", 1000 );

	rkLoader.LoadString_e( "mephisto_jump_animation", "", szBuf, MAX_PATH );
	m_ChargeJumpAnimation    = szBuf;
	m_fChargeJumpAniRate     = rkLoader.LoadFloat_e( "mephisto_jump_ani_rate", 0.f );
	m_fMephistoJumpForce     = rkLoader.LoadFloat_e( "mephisto_jump_force", 0.f );
	m_fMephistoJumpLookForce = rkLoader.LoadFloat_e( "mephisto_jump_look_force", 0.f );

	rkLoader.LoadString_e( "mephisto_charge_effect", "", szBuf, MAX_PATH );
	m_MephistoChargeEffect  = szBuf;
	m_fMephistoChargeOffset = rkLoader.LoadFloat_e( "mephisto_charge_effect_offset", FLOAT1 );

	iCnt = m_MephistoDashAttack.m_vWeaponInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		char szKey[MAX_PATH] = "";
		wsprintf( szKey, "mephisto_charge_effect_angle%d", i+1 );
		float fAngle = rkLoader.LoadFloat( szKey, 0.0f );
		m_vMephistoChargeEffectAngle.push_back( fAngle );
	}

	LoadAttackAttribute( "mephisto_fly_attack", m_MephistoFlyAttack, rkLoader );

	rkLoader.LoadString_e( "mephisto_fly_start_animation", "", szBuf, MAX_PATH );
	m_MephistoFlyStartAnimation = szBuf;
	rkLoader.LoadString_e( "mephisto_fly_start_effect", "", szBuf, MAX_PATH );
	m_MephistoFlyStartEffect = szBuf;
	rkLoader.LoadString_e( "mephisto_fly_delay_animation", "", szBuf, MAX_PATH );
	m_MephistoFlyDelayAnimation = szBuf;
	rkLoader.LoadString_e( "mephisto_fly_move_animation", "", szBuf, MAX_PATH );
	m_MephistoFlyMoveAnimation = szBuf;
	rkLoader.LoadString_e( "mephisto_fly_up_animation", "", szBuf, MAX_PATH );
	m_MephistoFlyUpAnimation = szBuf;
	rkLoader.LoadString_e( "mephisto_fly_down_animation", "", szBuf, MAX_PATH );
	m_MephistoFlyDownAnimation = szBuf;
	
	m_fMephistoFlyStartAniRate   = rkLoader.LoadFloat_e( "mephisto_fly_start_ani_rate", FLOAT1 );
	m_fMephistoFlyStartAngle     = rkLoader.LoadFloat_e( "mephisto_fly_start_angle", 180.0f );
	m_fMephistoFlyStartMaxRange  = rkLoader.LoadFloat_e( "mephisto_fly_start_max_range", 0.0f );
	m_dwMephistoFlyStartDuration = rkLoader.LoadInt_e( "mephisto_fly_start_duration", FLOAT1 );
	m_fMephistoFlyDelayAniRate   = rkLoader.LoadFloat_e( "mephisto_fly_delay_ani_rate", FLOAT1 );
	m_fMephistoFlyMoveAniRate    = rkLoader.LoadFloat_e( "mephisto_fly_move_ani_rate", FLOAT1 );
	m_fMephistoFlyUpAniRate      = rkLoader.LoadFloat_e( "mephisto_fly_up_ani_rate", FLOAT1 );
	m_fMephistoFlyDownAniRate    = rkLoader.LoadFloat_e( "mephisto_fly_down_ani_rate", FLOAT1 );
	m_fMephistoFlyMoveSpeed      = rkLoader.LoadFloat_e( "mephisto_fly_move_speed", FLOAT500 );
	m_dwMephistoFlyDuration      = rkLoader.LoadInt_e( "mephisto_fly_duration", 1000 );

	m_bEnableMephistoFlyOnBlowState     = rkLoader.LoadBool_e( "mephisto_fly_enable_blow_state", false );
	m_bEnableMephistoFlyOnDefenseState  = rkLoader.LoadBool_e( "mephisto_fly_enable_defense_state", false );
	m_bMephistoFlyCheckWounded          = rkLoader.LoadBool_e( "mephisto_fly_check_wounded", false );
	m_bMephistoFlyCheckBlowWounded      = rkLoader.LoadBool_e( "mephisto_fly_check_blow_wounded", false );
	m_bMephistoFlyCheckBoundBlowWounded = rkLoader.LoadBool_e( "mephisto_fly_check_bound_blow_wounded", false );
	m_bMephistoFlyCheckDown             = rkLoader.LoadBool_e( "mephisto_fly_check_down", false );

	m_fMephistoFlyAlphaRate = rkLoader.LoadFloat_e( "mephisto_fly_alpha_rate", FLOAT1 );
	m_fMephistoFlyEndJumpAmt = rkLoader.LoadFloat_e( "mephisto_fly_end_jump_amt", 0.0f );

	rkLoader.LoadString( "dummy_char", "", szBuf, MAX_PATH );
	m_szDummyChar = szBuf;

	rkLoader.LoadString( "dummy_char_on_wounded", "", szBuf, MAX_PATH );
	m_szDummyCharOnWounded = szBuf;
}

void ioMephistoItem::OnReleased( ioBaseChar *pOwner )
{
	ClearMephistoState( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

ioItem* ioMephistoItem::Clone()
{
	return new ioMephistoItem( *this );
}

ioWeaponItem::WeaponSubType ioMephistoItem::GetSubType() const
{
	return WST_MEPHISTO;
}

float ioMephistoItem::GetAttackSpeedRate() const
{
	switch( m_ChargeState )
	{
	case CS_MEPHISTO_FLY_STATE:
		return m_fMephistoFlyMoveSpeed;
	}

	return m_fAttackSpeedRate;
}

void ioMephistoItem::ClearMephistoState( ioBaseChar *pOwner )
{
	m_ChargeState = CS_NONE;

	m_dwDefenseChargeStartTime = 0;
	m_dwWoundedChargeStartTime = 0;

	m_dwAttackStartTime = 0;
	m_dwAttackEndTime = 0;

	m_dwMephistoChargeCheckTime = 0;
	m_iMephistoDashAttackCount = 0;

	m_fMephistoFlyCurRange = 0;
	m_dwMephistoFlyStartTime = 0;

	pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	pOwner->SetHideChar( false );

	ioPet *pPet = g_PetMgr.FindPet( pOwner );
	if( pPet )
		pPet->SetForceHide( false );

	DestroyChargeEffect( pOwner );
	DestroyDummyChar( pOwner );

	m_DirKey = ioUserKeyInput::DKI_NONE;
}

void ioMephistoItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearCancelInfo();

	m_DirKey = ioUserKeyInput::DKI_NONE;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

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

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioMephistoItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;
	case CS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioMephistoItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			if( EnableMephistoFlyState() )
			{
				SetMephistoFlyStartState( pOwner );
			}
			else
			{
				ChangeToNormalAttack( pOwner );
			}
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioMephistoItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

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

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_NORMAL_ATTACK;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMephistoItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case CS_NONE:
		{
			rkPacket >> m_iCurCombo;
			pOwner->SetAttackMoveEnable( false );
			pOwner->SetChargingState( false );
			m_ChargeState = CS_NONE;
		}
		break;
	case SS_NORMAL_ATTACK:
		{
			rkPacket >> m_iCurCombo;
			ChangeToNormalAttack( pOwner );
		}
		break;
	case SS_MEPHISTO_DASH:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );
			SetMephistoDashState( pOwner );
		}
		break;
	case SS_MEPHISTO_DASH_ATTACK:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			rkPacket >> m_iMephistoDashAttackCount;

			SetMephistoDashAttackState( pOwner );
		}
		break;
	case SS_MEPHISTO_DASH_JUMP:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			SetMephistoDashJumpState( pOwner );
		}
		break;
	case SS_MEPHISTO_FLY_START:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			rkPacket >> m_szCreateDummyChar;

			SetMephistoFlyStartState( pOwner );

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				m_iDummyCharIndex = g_DummyCharMgr.GetCurLocalIndex();
				ioDummyChar *pDummy = pStage->CreateDummyChar( m_szCreateDummyChar, m_iDummyCharIndex, pOwner->GetCharName(), pOwner->GetWorldPosition(), 0.0f, 0, true );
				if( pDummy )
					pDummy->SetWorldPosition( pOwner->GetWorldPosition() );
			}
		}
		break;
	case SS_MEPHISTO_FLY:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			SetMephistoFlyState( pOwner );
		}
		break;
	case SS_MEPHISTO_FLY_MOVE:
		{
			int iFlyState = 0;
			rkPacket >> iFlyState;
			m_FlyState = (FlyState)iFlyState;

			SetMephistoFlyAnimation( pOwner, m_FlyState );
		}
		break;
	case SS_MEPHISTO_FLY_ATTACK:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			SetMephistoFlyAttackState( pOwner );
		}
		break;
	case SS_MEPHISTO_FLY_END:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			SetMephistoFlyEndState( pOwner );
		}
		break;
	case SS_MEPHISTO_DIR_KEY:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			int iDirKey;
			rkPacket >> iDirKey;
			ioUserKeyInput::DirKeyInput eDir = (ioUserKeyInput::DirKeyInput)iDirKey;
			pOwner->SetTargetRotToDirKey( eDir );
		}
		break;
	}
}

void ioMephistoItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioMephistoItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioMephistoItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
}

bool ioMephistoItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioMephistoItem::GetAutoTargetValue( ioBaseChar *pOwner,
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
	}
}

//Gauge가 Delay, Run, Dash 상태 에서 차는 것이 아닌 맞거나 맞았을 때 차는 방식으로 변경
void ioMephistoItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check bullet
	if( m_fMaxMephistoGauge <= m_fCurMephistoGauge )
		return;

	float fRecoveryTime = 0.0f;
	// check owner state
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		fRecoveryTime = m_fIncreaseGaugeTime;
		break;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = fRecoveryTime * fTimePerSec;

	m_fCurMephistoGauge += fGauge;
	m_fCurMephistoGauge = min( m_fCurMephistoGauge, m_fMaxMephistoGauge );
}

int ioMephistoItem::GetMaxBullet()
{
	return (int)m_fMaxMephistoGauge;
}

int ioMephistoItem::GetCurBullet()
{
	DWORD dwCurCoolTime = m_fCurMephistoGauge;
	dwCurCoolTime = max( 0, min( dwCurCoolTime, m_fMaxMephistoGauge ) );

	return (int)dwCurCoolTime;
}

void ioMephistoItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	DWORD dwCurBullet = iCurBullet;
	dwCurBullet = max( 0, min( dwCurBullet, m_fMaxMephistoGauge ) );

	m_fCurMephistoGauge = (float)dwCurBullet;
}

void ioMephistoItem::InitCoolTime()
{
	m_fCurMephistoGauge = 0.0f;
}

void ioMephistoItem::MaxCoolTime()
{
	m_fCurMephistoGauge = (float)m_fMaxMephistoGauge;
}

bool ioMephistoItem::EnableMoveState()
{
	switch ( m_ChargeState )
	{
	case CS_MEPHISTO_DASH_STATE:
	case CS_MEPHISTO_FLY_STATE:
		return true;
	}

	return false;
}

void ioMephistoItem::SetMephistoDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_ChargeState = CS_MEPHISTO_DASH_STATE;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_ChargeDashAnimation );

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / m_fMephistoDashAniRate );
	}

	if( pOwner->GetState() != CS_MEPHISTO_STATE )
	{
		m_dwMephistoChargeCheckTime = FRAMEGETTIME() + m_dwMephistoChargeTime;
		m_iMephistoDashAttackCount = 1;

		CreateChargeEffect( pOwner );
		pOwner->SetState( CS_MEPHISTO_STATE );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_MEPHISTO_DASH;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMephistoItem::SetMephistoDashAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_MephistoDashAttack.m_AttackAnimation );

		float fTimeRate  = m_MephistoDashAttack.m_fAttackAniRate;
		DWORD dwPreDelay = m_MephistoDashAttack.m_dwPreDelay;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetReservedSliding( m_MephistoDashAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

		DWORD dwCurTime = FRAMEGETTIME();

		m_dwAttackStartTime = dwCurTime;
		m_dwAttackStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
		m_dwAttackEndTime = dwCurTime;
		m_dwAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
		m_dwAttackEndTime += dwPreDelay;
	}

	if( pOwner->IsNeedProcess() )
	{
		m_fCurMephistoGauge -= m_fDecreaseGaugeOnDashAttack;
		m_fCurMephistoGauge = max( 0.0f, m_fCurMephistoGauge );
	}

	m_ChargeState = CS_MEPHISTO_DASH_ATTACK_STATE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_MEPHISTO_DASH_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iMephistoDashAttackCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMephistoItem::SetMephistoDashJumpState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_ChargeJumpAnimation );

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fChargeJumpAniRate, 0.0f, 0.0f );
	}

	pOwner->SetJumpPower( m_fMephistoJumpForce );
	pOwner->SetForcePower( pOwner->GetMoveDir(), m_fMephistoJumpLookForce, FLOAT1, true );

	m_ChargeState = CS_MEPHISTO_DASH_JUMP_STATE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_MEPHISTO_DASH_JUMP;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMephistoItem::SetMephistoFlyStartState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_MephistoFlyStartAnimation );

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fMephistoFlyStartAniRate, 0.0f, 0.0f );
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		ioBaseChar *pChar = pStage->GetOwnerChar();
		if( pChar && pChar->GetTeam() == pOwner->GetTeam() )
		{
			pOwner->CreateMapEffectBySkill( m_MephistoFlyStartEffect, pOwner->GetMidPositionByRate() );
		}
	}

	if( pOwner->IsNeedProcess() )
	{
		switch( pOwner->GetState() )
		{
		case CS_WOUNDED:
		case CS_BLOW_WOUNDED:
		case CS_BOUND_BLOW_WOUNDED:
			{
				m_fCurMephistoGauge = 0;
			}
			break;
		default:
			{
				m_fCurMephistoGauge -= m_fDecreaseGaugeOnFly;
				m_fCurMephistoGauge = max( 0, m_fCurMephistoGauge );
			}
			break;
		}

		CreateDummyChar( pOwner );
	}

	m_dwMephistoFlyStartTime = FRAMEGETTIME();
	m_ChargeState = CS_MEPHISTO_FLY_START_STATE;
	m_FlyState = FS_NONE;

	pOwner->SetState( CS_MEPHISTO_STATE );
	pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE * m_fMephistoFlyAlphaRate );
	pOwner->SetHideChar( true );

	ioBaseChar *pChar = pStage->GetOwnerChar();
	if( pChar->GetTeam() != pOwner->GetTeam() )
	{
		ioPet *pPet = g_PetMgr.FindPet( pOwner );
		if( pPet )
			pPet->SetForceHide( true );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_MEPHISTO_FLY_START;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_szCreateDummyChar;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMephistoItem::SetMephistoFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_MephistoFlyDelayAnimation );

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / m_fMephistoFlyDelayAniRate );
	}

	m_ChargeState = CS_MEPHISTO_FLY_STATE;

	ioUserKeyInput::DirKeyInput eDir = pOwner->GetDirKey();
	pOwner->SetTargetRotToDirKey( eDir );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_MEPHISTO_FLY;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMephistoItem::SetMephistoFlyAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = -1;
	if( pGrp )
	{
		iAniID = pGrp->GetAnimationIdx( m_MephistoFlyAttack.m_AttackAnimation );

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_MephistoFlyAttack.m_fAttackAniRate, 0.0f, m_MephistoFlyAttack.m_dwPreDelay );
	}

	m_ChargeState = CS_MEPHISTO_FLY_ATTACK_STATE;

	pOwner->RefreshFireTimeListWithBase( pOwner->IncreaseWeaponIndexBase(),
										 iAniID,
										 m_MephistoFlyAttack.m_vWeaponInfoList,
										 FTT_NORMAL_ATTACK,
										 m_MephistoFlyAttack.m_fAttackAniRate,
										 m_MephistoFlyAttack.m_dwPreDelay );

	m_dwAttackEndTime = FRAMEGETTIME();
	m_dwAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_MephistoFlyAttack.m_fAttackAniRate;
	m_dwAttackEndTime += m_MephistoFlyAttack.m_dwPreDelay;

	m_ChargeState = CS_MEPHISTO_FLY_ATTACK_STATE;

	pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	pOwner->SetHideChar( false );

	ioPet *pPet = g_PetMgr.FindPet( pOwner );
	if( pPet )
		pPet->SetForceHide( false );

	DestroyDummyChar( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_MEPHISTO_FLY_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMephistoItem::SetMephistoFlyEndState( ioBaseChar *pOwner )
{
	DestroyDummyChar( pOwner );
	pOwner->SetSKillEndJumpState( m_fMephistoFlyEndJumpAmt, false, true, false );

	m_ChargeState = CS_MEPHISTO_FLY_END_STATE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_MEPHISTO_FLY_END;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMephistoItem::ProcessMephistoDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsJumpKeyDown() )
		{
			SetMephistoDashJumpState( pOwner );
		}
		else if( !pOwner->IsAttackKeyDown() )
		{
			SetMephistoDashAttackState( pOwner );
		}
		else
		{
			m_fCurMephistoGauge -= m_fDecreaseGaugeOnDash * g_FrameTimer.GetSecPerFrame();
			m_fCurMephistoGauge = max( 0.0f, m_fCurMephistoGauge );

			if( m_fCurMephistoGauge <= 0.0f )
			{
				SetMephistoDashAttackState( pOwner );
			}
		}
	}

	CheckMaphistoChargeAttack( pOwner );
	UpdateChargeEffect( pOwner );
}

void ioMephistoItem::ProcessMephistoDashAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwAttackStartTime > 0 && m_dwAttackStartTime < FRAMEGETTIME() )
	{
		//m_dwAttackStartTime = 0;

		for( int i=0; i<m_iMephistoDashAttackCount; ++i )
		{
			if( COMPARE( i, 0, (int)m_MephistoDashAttack.m_vWeaponInfoList.size() ) )
			{
				D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;

				if( COMPARE( i, 0, (int)m_vMephistoChargeEffectAngle.size() ) )
				{
					float fAngle = ioMath::ArrangeHead( m_vMephistoChargeEffectAngle[i] );

					D3DXQUATERNION qtAngle;
					D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

					vDir = qtAngle * pOwner->GetMoveDir();
					D3DXVec3Normalize( &vDir, &vDir );
				}
				
				pOwner->NormalAttackFireExplicit( m_MephistoDashAttack.m_vWeaponInfoList[i], vDir );
			}
		}

		DestroyChargeEffect( pOwner );

		m_ChargeState = CS_MEPHISTO_DASH_ATTACK_END_STATE;
	}
	/*else if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		m_dwAttackEndTime = 0;
		pOwner->SetState( CS_DELAY );
	}*/
}

void ioMephistoItem::ProcessMephistoDashAttackEndState( ioBaseChar* pOwner )
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		m_dwAttackEndTime = 0;
		m_ChargeState = CS_NONE;
		pOwner->SetState( CS_DELAY );
	}
}

void ioMephistoItem::ProcessMephistoJumpState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		m_fCurMephistoGauge -= m_fDecreaseGaugeOnDash * g_FrameTimer.GetSecPerFrame();
		m_fCurMephistoGauge = max( 0.0f, m_fCurMephistoGauge );

		if( CheckLanding( pOwner ) )
			SetMephistoDashState( pOwner );
	}

	CheckMaphistoChargeAttack( pOwner );
	UpdateChargeEffect( pOwner );
}

void ioMephistoItem::ProcessMephistoFlyStartState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fMephistoFlyStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return;

	if( m_dwMephistoFlyStartDuration == 0 )
		return;

	float fCurSpeed = m_fMephistoFlyStartMaxRange / (float)m_dwMephistoFlyStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fMephistoFlyCurRange + fDistance < m_fMephistoFlyStartMaxRange )
	{
		vMoveAmt = vMoveDir * fDistance;

		m_fMephistoFlyCurRange += fDistance;

		bool bCol = false;
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
		{
			pOwner->Translate( vMoveAmt );
		}

		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		if( pOwner->CheckAxisLimit( &vPos ) )
			pOwner->SetWorldPosition( vPos );
	}
	else if( pOwner->IsNeedProcess() )
	{
		SetMephistoFlyState( pOwner );
	}
}

void ioMephistoItem::ProcessMephistoFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;
	
	/*m_fCurMephistoGauge -= m_fDecreaseGaugeOnFlying * g_FrameTimer.GetSecPerFrame();
	m_fCurMephistoGauge = max( 0.0f, m_fCurMephistoGauge );
	if( m_fCurMephistoGauge <= 0.0f )
	{
		SetMephistoFlyEndState( pOwner );
		return;
	}*/

	if( m_dwMephistoFlyStartTime + m_dwMephistoFlyDuration < FRAMEGETTIME() )
	{
		SetMephistoFlyEndState( pOwner );
		return;
	}

	if( pOwner->IsAttackKey() )
	{
		SetMephistoFlyAttackState( pOwner );
		return;
	}

	if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
	{
		if( pOwner->IsJumpKeyDown() )
		{
			if( m_FlyState != FS_MOVE_UP )
			{
				SetMephistoFlyAnimation( pOwner, FS_MOVE_UP );
			}

			m_FlyState = FS_MOVE_UP;
		}
		else if( pOwner->IsDefenseKeyDown() )
		{
			if( m_FlyState != FS_MOVE_DOWN )
			{
				SetMephistoFlyAnimation( pOwner, FS_MOVE_DOWN );
			}

			m_FlyState = FS_MOVE_DOWN;
		}
		else
		{
			if( m_FlyState != FS_MOVE )
			{
				SetMephistoFlyAnimation( pOwner, FS_MOVE );
			}
			m_FlyState = FS_MOVE;
		}
	}
	else
	{
		if( pOwner->IsJumpKeyDown() )
		{
			if( m_FlyState != FS_UP )
			{
				SetMephistoFlyAnimation( pOwner, FS_UP );
			}

			m_FlyState = FS_UP;
		}
		else if( pOwner->IsDefenseKeyDown() )
		{
			if( m_FlyState != FS_DOWN )
			{
				SetMephistoFlyAnimation( pOwner, FS_DOWN );
			}

			m_FlyState = FS_DOWN;
		}
		else
		{
			if( m_FlyState != FS_DELAY )
			{
				SetMephistoFlyAnimation( pOwner, FS_DELAY );
			}
			m_FlyState = FS_DELAY;
		}
	}
}

void ioMephistoItem::ProcessMephistoFlyAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		m_dwAttackEndTime = 0;
		pOwner->SetSKillEndJumpState( m_fMephistoFlyEndJumpAmt, false, true, false );
	}
}

bool ioMephistoItem::CheckLanding( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
	{
		return false;
	}

	return true;
}

void ioMephistoItem::CheckMaphistoChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		return;
	}

	if( pOwner->IsNeedProcess() && m_iMephistoDashAttackCount >= (int)m_MephistoDashAttack.m_vWeaponInfoList.size() )
	{
		m_iMephistoDashAttackCount = (int)m_MephistoDashAttack.m_vWeaponInfoList.size();
		return;
	}

	if( m_dwMephistoChargeCheckTime < FRAMEGETTIME() )
	{
		CreateChargeEffect( pOwner );
		
		m_dwMephistoChargeCheckTime = FRAMEGETTIME() + m_dwMephistoChargeTime;

		if( pOwner->IsNeedProcess() )
		{
			m_iMephistoDashAttackCount++;
			m_iMephistoDashAttackCount = min( m_iMephistoDashAttackCount, (int)m_MephistoDashAttack.m_vWeaponInfoList.size() );
		}
	}
}

bool ioMephistoItem::CheckMephistoDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !m_bEnableMephistoFlyOnDefenseState )
		return false;

	if( m_dwDefenseChargeStartTime == 0 )
	{
		m_dwDefenseChargeStartTime = FRAMEGETTIME();
	}
	else if( !pOwner->IsAttackKeyDown() )
	{
		m_dwDefenseChargeStartTime = FRAMEGETTIME();
	}
	else if( m_dwDefenseChargeStartTime > 0 && m_dwDefenseChargeStartTime + m_dwChargeTime < FRAMEGETTIME() && pOwner->IsAttackKeyDown() )
	{
		if( EnableMephistoFlyState() )
		{
			SetMephistoFlyStartState( pOwner );
			return true;
		}
	}

	return false;
}

void ioMephistoItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( !EnableMoveState() )
		return;

	ioUserKeyInput::DirKeyInput eDirKey = pOwner->CheckCurDirKey();
	if( eDirKey != m_DirKey )
	{
		m_DirKey = eDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)SS_MEPHISTO_DIR_KEY;
			kPacket << pOwner->GetWorldPosition();
			kPacket << (int)eDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMephistoItem::CreateDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	m_iDummyCharIndex = g_DummyCharMgr.GetCurLocalIndex();
	m_szCreateDummyChar.Clear();
	switch( pOwner->GetState() )
	{
	case CS_WOUNDED:
	case CS_BLOW_WOUNDED:
	case CS_BOUND_BLOW_WOUNDED:
		{
			m_szCreateDummyChar = m_szDummyCharOnWounded;
		}
		break;
	default:
		{
			m_szCreateDummyChar = m_szDummyChar;
			
		}
		break;
	}

	if( !m_szCreateDummyChar.IsEmpty() )
	{
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_szCreateDummyChar, m_iDummyCharIndex, pOwner->GetCharName(), pOwner->GetWorldPosition(), 0.0f, 0, true );
		if( pDummy )
			pDummy->SetWorldPosition( pOwner->GetWorldPosition() );
	}
}

void ioMephistoItem::DestroyDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyCharIndex );
	if( pDummy )
	{
		pDummy->SetDieState( true );
		m_iDummyCharIndex = 0;
	}
}

bool ioMephistoItem::ProcessCancel( ioBaseChar *pOwner )
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

			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
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

bool ioMephistoItem::IsWoundAfterUseExtendState()
{
	return m_bEnableMephistoFlyOnBlowState;
}

bool ioMephistoItem::CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType )
{
	if( !pOwner )
		return false;

	switch( pOwner->GetState() )
	{
	case CS_WOUNDED:
		if( !m_bMephistoFlyCheckWounded )
		{
			return false;
		}
		break;
	case CS_BLOW_WOUNDED:
		{
			if( pOwner->IsApplyDownState( false ) && !m_bMephistoFlyCheckDown )
			{
				return false;
			}
			else if( !pOwner->IsApplyDownState( false ) && !m_bMephistoFlyCheckBlowWounded )
			{
				return false;
			}
		}
		break;
	case CS_BOUND_BLOW_WOUNDED:
		{
			if( pOwner->IsApplyDownState( false ) && !m_bMephistoFlyCheckDown )
			{
				return false;
			}
			else if( !pOwner->IsApplyDownState( false ) && !m_bMephistoFlyCheckBoundBlowWounded )
			{
				return false;
			}
		}
		break;
	}

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
		if( EnableMephistoFlyState() )
		{
			SetMephistoFlyStartState( pOwner );
			return true;
		}
	}

	return false;
}

void ioMephistoItem::SetMephistoFlyAnimation( ioBaseChar *pOwner, FlyState eState )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioHashString szAnimation;
	float fRate = FLOAT1;
	switch( eState )
	{
	case FS_DELAY:
		{
			szAnimation = m_MephistoFlyDelayAnimation;
			fRate = m_fMephistoFlyDelayAniRate;
		}
		break;
	case FS_MOVE:
		{
			szAnimation = m_MephistoFlyMoveAnimation;
			fRate = m_fMephistoFlyMoveAniRate;
		}
		break;
	case FS_UP:
	case FS_MOVE_UP:
		{
			szAnimation = m_MephistoFlyUpAnimation;
			fRate = m_fMephistoFlyUpAniRate;
		}
		break;
	case FS_DOWN:
	case FS_MOVE_DOWN:
		{
			szAnimation = m_MephistoFlyDownAnimation;
			fRate = m_fMephistoFlyDownAniRate;
		}
		break;
	}

	int iAniID = pGrp->GetAnimationIdx( szAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_MEPHISTO_FLY_MOVE;
		kPacket << (int)eState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMephistoItem::CreateChargeEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int iEffectAray = m_iMephistoDashAttackCount - 1;
	iEffectAray = min( iEffectAray, (int)m_vMephistoChargeEffectAngle.size() );
	float fAngle = ioMath::ArrangeHead( m_vMephistoChargeEffectAngle[iEffectAray] );

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

	D3DXVECTOR3 vDir = qtAngle * pOwner->GetMoveDir();
	D3DXVec3Normalize( &vDir, &vDir );
	vDir *= m_fMephistoChargeOffset;
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vDir;
	ioMapEffect *pEffect = pOwner->CreateMapEffectBySkill( m_MephistoChargeEffect, vPos );
	if( pEffect )
	{
		m_vEffectID.push_back( pEffect->GetUniqueID() );
	}
}

void ioMephistoItem::DestroyChargeEffect( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		int iCnt = m_vEffectID.size();
		for( int i=0; i<iCnt; ++i )
		{
			pStage->DestroyMapEffect( m_vEffectID[i] );
		}
		m_vEffectID.clear();
	}
}

void ioMephistoItem::UpdateChargeEffect( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	for( int i=0; i<(int)m_vEffectID.size(); ++i )
	{
		D3DXVECTOR3 vDir = pOwner->GetMoveDir() * m_fMephistoChargeOffset;
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vDir;

		ioMapEffect *pEffect = pStage->FindMapEffect( m_vEffectID[i] );
		if( pEffect )
		{
			float fAngle = ioMath::ArrangeHead( m_vMephistoChargeEffectAngle[i] );

			D3DXQUATERNION qtAngle;
			D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

			D3DXVECTOR3 vDir = qtAngle * pOwner->GetMoveDir();
			D3DXVec3Normalize( &vDir, &vDir );
			vDir *= m_fMephistoChargeOffset;
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vDir;

			pEffect->SetWorldPosition( vPos );
		}
	}
}

bool ioMephistoItem::EnableMephistoDashState()
{
	return m_fCurMephistoGauge >= m_fDecreaseGaugeOnDash ? true : false;
}

bool ioMephistoItem::EnableMephistoFlyState()
{
	return m_fCurMephistoGauge >= m_fDecreaseGaugeOnFly ? true : false;
}

bool ioMephistoItem::IsMephistoFlyCharCollisionSkipState()
{
	if( m_ChargeState == CS_MEPHISTO_FLY_START_STATE || m_ChargeState == CS_MEPHISTO_FLY_STATE )
		return true;

	return false;
}

bool ioMephistoItem::IsFlyState()
{
	if( m_ChargeState == CS_MEPHISTO_FLY_START_STATE || m_ChargeState == CS_MEPHISTO_FLY_STATE )
		return true;

	return false;
}

bool ioMephistoItem::IsNoWoundState()
{
	if( m_ChargeState == CS_MEPHISTO_FLY_START_STATE )
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioMephistoSpecialState::ioMephistoSpecialState()
{
}

ioMephistoSpecialState::~ioMephistoSpecialState()
{
}

void ioMephistoSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMephistoSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMephistoItem *pMephisto = ToMephistoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMephisto )
		pMephisto->ClearMephistoState( pOwner );
}

bool ioMephistoSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioMephistoItem *pMephisto = ToMephistoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMephisto && pMephisto->IsMephistoFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioMephistoSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMephistoItem *pMephisto = ToMephistoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMephisto )
	{
		pMephisto->CheckKeyInput( pOwner );

		switch( pMephisto->GetChargeState() )
		{
		case ioMephistoItem::CS_MEPHISTO_DASH_STATE:
			{
				pMephisto->ProcessMephistoDashState( pOwner );
			}
			break;
		case ioMephistoItem::CS_MEPHISTO_DASH_ATTACK_STATE:
			{
				pMephisto->ProcessMephistoDashAttackState( pOwner );
			}
			break;
		case ioMephistoItem::CS_MEPHISTO_DASH_ATTACK_END_STATE:
			{
				pMephisto->ProcessMephistoDashAttackEndState( pOwner );
			}
			break;
		case ioMephistoItem::CS_MEPHISTO_DASH_JUMP_STATE:
			{
				pMephisto->ProcessMephistoJumpState( pOwner );
			}
			break;
		case ioMephistoItem::CS_MEPHISTO_FLY_START_STATE:
			{
				pMephisto->ProcessMephistoFlyStartState( pOwner );
			}
			break;
		case ioMephistoItem::CS_MEPHISTO_FLY_STATE:
			{
				pMephisto->ProcessMephistoFlyState( pOwner );
			}
			break;
		case ioMephistoItem::CS_MEPHISTO_FLY_ATTACK_STATE:
			{
				pMephisto->ProcessMephistoFlyAttackState( pOwner );
			}
			break;
		}
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
	}
}

void ioMephistoSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioMephistoSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
	float fSpeed = 0.0f;

	ioMephistoItem *pMephisto = ToMephistoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMephisto )
	{
		switch( pMephisto->GetChargeState() )
		{
		case ioMephistoItem::CS_MEPHISTO_DASH_STATE:
		case ioMephistoItem::CS_MEPHISTO_DASH_JUMP_STATE:
			{
				vDir = pOwner->GetMoveDir();
				fSpeed = pMephisto->GetMephistoDashSpeed();
			}
			break;
		case ioMephistoItem::CS_MEPHISTO_FLY_STATE:
			{
				switch( pMephisto->GetFlyState() )
				{
				case ioMephistoItem::FS_DELAY:
					{
						vDir = pOwner->GetMoveDir();
						fSpeed = 0.0f;
					}
					break;
				case ioMephistoItem::FS_MOVE:
					{
						vDir = pOwner->GetMoveDir();
						fSpeed = pMephisto->GetMephistoFlySpeed();
					}
					break;
				case ioMephistoItem::FS_UP:
					{
						vDir = ioMath::UNIT_Y;
						fSpeed = pMephisto->GetMephistoFlySpeed();
					}
					break;
				case ioMephistoItem::FS_DOWN:
					{
						vDir = -ioMath::UNIT_Y;
						fSpeed = pMephisto->GetMephistoFlySpeed();
					}
					break;
				case ioMephistoItem::FS_MOVE_UP:
					{
						vDir = pOwner->GetMoveDir();
						vDir += ioMath::UNIT_Y;
						D3DXVec3Normalize( &vDir, &vDir );
						fSpeed = pMephisto->GetMephistoFlySpeed();
					}
					break;
				case ioMephistoItem::FS_MOVE_DOWN:
					{
						vDir = pOwner->GetMoveDir();
						vDir -= ioMath::UNIT_Y;
						D3DXVec3Normalize( &vDir, &vDir );
						fSpeed = pMephisto->GetMephistoFlySpeed();
					}
					break;
				}
			}
			break;
		}
	}

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( fSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = fSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
			{
				vMoveAmt = vDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}

		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		float fCurLength = D3DXVec3Length( &vTotalMove );

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

					float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );
					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
		
		// 비행 하강중 맵을 뚫고 내려가는 현상에 대한 예외처리.
		D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
		float fMapHeight = pOwner->GetBottomHeight();
		if( vCurPos.y <= fMapHeight )
		{
			vCurPos.y = fMapHeight;
			pOwner->SetWorldPosition( vCurPos );
		}
	}

	if( pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	if( pMephisto->GetChargeState() == ioMephistoItem::CS_MEPHISTO_DASH_STATE ||
		pMephisto->GetChargeState() == ioMephistoItem::CS_MEPHISTO_DASH_ATTACK_STATE ||
		pMephisto->GetChargeState() == ioMephistoItem::CS_MEPHISTO_DASH_ATTACK_END_STATE ||
		pMephisto->GetChargeState() == ioMephistoItem::CS_MEPHISTO_DASH_JUMP_STATE )
	{
		float fCurGravity = pStage->GetRoomGravity();
		float fCurTestGravity = FLOAT1;
		bool bDropZone = false;

		if( pOwner->CheckGhostState() )
		{
			D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
			float fMapHeight = pStage->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, pOwner->GetMidHeightByRate() );
			if( fMapHeight <= 0.0f )
			{
				bDropZone = true;
				if( pOwner->GetCurJumpPowerAmt() <= 0.0f && vCurPos.y == pOwner->GetPreHeightForGhostState() )
				{
					fCurTestGravity = 0.0f;
				}
			}
			else
			{
				pOwner->SetPreHeightForGhostState( fMapHeight );
			}
		}

		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

		if( pOwner->GetCurJumpPowerAmt() > 0.0f )
		{
			vPos.y += pOwner->GetCurJumpPowerAmt() * fTimePerSec;
		}

		bool bPreFrameIsHigh = false;

		float fMapHeight = 0.0f;

		if( !pOwner->HasBuff(BT_DROP_ZONE) )
			fMapHeight = pOwner->GetBottomHeight();

		if( ( pOwner->CheckGhostState() && bDropZone ) && pOwner->GetState() != CS_DROP_ZONE_DOWN )
			fMapHeight = pOwner->GetPreHeightForGhostState();

		if( vPos.y > fMapHeight )
		{
			float fGravity = pOwner->GetGravityAmt();
			fGravity += ( fCurGravity * fCurTestGravity * pOwner->GetBuffGravityRate() ) * fTimePerSec;
			pOwner->SetGravityAmt( fGravity );
			vPos.y -= pOwner->GetGravityAmt() * fTimePerSec;
			bPreFrameIsHigh = true;
		}

		pOwner->SetCurFrameDrop( false );
		pOwner->SetCurGravityGapAmt( pOwner->GetGravityAmt() + pOwner->GetCurJumpPowerAmt() );

		// 현재 하강중이고, 지면에 착지 or 더 낮게 내려간 경우
		if( pOwner->GetCurJumpPowerAmt() <= pOwner->GetGravityAmt() && vPos.y <= fMapHeight )
		{
			vPos.y = fMapHeight;

			// m_fGravityAmt 초기화전에 체크
			pOwner->CheckIceBound();

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetCurFrameDrop( bPreFrameIsHigh );

			if( pOwner->CheckGrapplingSwingCollision() )
				pOwner->CheckSkillGrapplingCancelBuff( "", true );
		}

		// 맵을 머리로 뚫는 현상 방지용
		if( pOwner->GetCurJumpPowerAmt() > pOwner->GetGravityAmt() )
		{
			D3DXVECTOR3 vDiff = vPos - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vDiff, &vDiff );

			ioRay kRay( pOwner->GetMidPositionByRate(0.75), vDiff );
			if( pStage->CheckMapCollision( pOwner, kRay, vPos ) )
			{
				vPos = pOwner->GetWorldPosition();
			}
		}

		if( pStage && pOwner->IsFlyMapLimit() )
		{
			pStage->CheckFlyMapLimit( &vPos );
		}

		pOwner->SetWorldPosition( vPos );
	}
	return true;
}

void ioMephistoSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioMephistoItem *pMephistoItem = ToMephistoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMephistoItem && pMephistoItem->EnableMoveState() )
		pOwner->ChangeDirectionByInputDir( false );
}

bool ioMephistoSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	ioMephistoItem *pMephistoItem = ToMephistoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMephistoItem && pMephistoItem->GetChargeState() == ioMephistoItem::CS_MEPHISTO_FLY_STATE )
		return true;

	return false;
}

//Gauge가 Delay, Run, Dash 상태 에서 차는 것이 아닌 맞거나 맞았을 때 차는 방식으로 변경
void ioMephistoItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( m_fMaxMephistoGauge <= m_fCurMephistoGauge )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_MEPHISTO )
			fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_MEPHISTO )
			fTypeRate = m_fExtraGaugeAttackDef;
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	m_fCurMephistoGauge += fTypeRate;
	m_fCurMephistoGauge = min( m_fCurMephistoGauge, m_fMaxMephistoGauge );
}