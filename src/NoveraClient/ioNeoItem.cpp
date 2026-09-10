#include "StdAfx.h"

#include "ioNeoItem.h"
#include "WeaponDefine.h"

ioNeoItem::ioNeoItem(void)
{
	ClearData();
}

ioNeoItem::ioNeoItem( const ioNeoItem &rhs ):
ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_szGatheringAni( rhs.m_szGatheringAni ),
	m_fGatheringAniRate( rhs.m_fGatheringAniRate ),
	m_szFlyChargingAni( rhs.m_szFlyChargingAni ),
	m_fFlyChargingAniRate( rhs.m_fFlyChargingAniRate ),
	m_szFlyGatheringAni( rhs.m_szFlyGatheringAni ),
	m_fFlyGatheringAniRate( rhs.m_fFlyGatheringAniRate ),
	m_NormalEffectAttribute( rhs.m_NormalEffectAttribute ),
	m_JumpEffectAttribute( rhs.m_JumpEffectAttribute ),
	m_DashEffectAttribute( rhs.m_DashEffectAttribute ),
	m_FlyEffectAttribute( rhs.m_FlyEffectAttribute ),
	m_FlyInfo( rhs.m_FlyInfo ),
	m_JumpFlyInfo( rhs.m_JumpFlyInfo ),
	m_fLandingRate( rhs.m_fLandingRate ),
	m_fEnableEndJumpHeight( rhs.m_fEnableEndJumpHeight ),
	m_FlyFireInfo( rhs.m_FlyFireInfo ),
	m_szChargeEffectAni( rhs.m_szChargeEffectAni ),
	m_szJumpChargeEffectAni( rhs.m_szJumpChargeEffectAni ),
	m_szDashChargeEffectAni( rhs.m_szDashChargeEffectAni ),
	m_szFlyChargeEffectAni( rhs.m_szFlyChargeEffectAni ),
	m_fChargeEffectAniRate( rhs.m_fChargeEffectAniRate ),
	m_fChargeEffectEndJumpAmt( rhs.m_fChargeEffectEndJumpAmt ),
	m_dwEffectAttackRepeatTime( rhs.m_dwEffectAttackRepeatTime ),
	m_vEffectOffsetList( rhs.m_vEffectOffsetList ),
	m_szEffectName( rhs.m_szEffectName ),
	m_szBoardEffectName( rhs.m_szBoardEffectName ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
	m_dwGatheringTime( rhs.m_dwGatheringTime ),
	m_dwFlyChargeTime( rhs.m_dwFlyChargeTime ),
	m_dwFlyGatheringTime( rhs.m_dwFlyGatheringTime )
{
	ClearData();

	m_iCurEffectChargeIndex = 0;
	m_fCurGauge = 0.0f;
}

ioNeoItem::~ioNeoItem(void)
{
	ClearData();
}

void ioNeoItem::ClearData()
{
	m_ChargeState = CS_NONE;
	
	m_iCurCombo = 0;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_szTargetName.Clear();
	m_bEffectAttackKey = false;
}

void ioNeoItem::ClearFlyData( ioBaseChar *pOwner )
{
	m_FlyState = SFS_NONE;
	m_CurFlyInfo.Init();
	m_fFlyCurRange = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vFlyForceDir = ioMath::VEC3_ZERO;
	m_dwFlyForceTime = 0;
	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;
	m_fCurTargetRange = 0.0f;
	m_fCurTargetAngle = 0.0f;
	m_fCurTargetUpHeight = 0.0f;
	m_fCurTargetDownHeight = 0.0f;
	m_CurTargetWoundType = TWT_DOWN_WOUND_EXCEPT;
	m_szTargetName.Clear();

	m_dwUpStartTime	= 0;
	m_fUpCurrRange = 0.0f;
	m_vAttackDir = ioMath::VEC3_ZERO;

	RemoveWeapon( pOwner );
	m_WeaponIndexList.clear();

	if( pOwner )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pOwner->ShowEquipItemMesh( true, ES_WEAPON );
	}
	if( m_dwBoardEffectIndex != 0 )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwBoardEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();

		m_dwBoardEffectIndex = 0;
	}
}

void ioNeoItem::ClearAttackState( ioBaseChar *pOwner )
{
}

ioItem* ioNeoItem::Clone()
{
	return new ioNeoItem( *this );
}

ioWeaponItem::WeaponSubType ioNeoItem::GetSubType() const
{
	return WST_NEO_ITEM;
}

void ioNeoItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	ClearData();
}

void ioNeoItem::LoadProperty( ioINILoader &rkLoader )
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

	m_fLandingRate = rkLoader.LoadFloat_e( "landing_rate", FLOAT1 );
	m_fEnableEndJumpHeight = rkLoader.LoadFloat_e( "enable_end_jump_height", 0.0f );
	
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;
	m_fGatheringAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "fly_charging_animation", "", szBuf, MAX_PATH );
	m_szFlyChargingAni = szBuf;
	m_fFlyChargingAniRate = rkLoader.LoadFloat_e( "fly_charging_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "fly_gathering_animation", "", szBuf, MAX_PATH );
	m_szFlyGatheringAni = szBuf;
	m_fFlyGatheringAniRate = rkLoader.LoadFloat_e( "fly_gathering_ani_rate", FLOAT1 );

	LoadFlyInfo( rkLoader );
	LoadJumpFlyInfo( rkLoader );
	LoadFlyInfo( rkLoader, "fly_fire", m_FlyFireInfo );
	LoadAttackAttribute_e( "normal_effect_attack", m_NormalEffectAttribute, rkLoader );
	LoadAttackAttribute_e( "jump_effect_attack", m_JumpEffectAttribute, rkLoader );
	LoadAttackAttribute_e( "dash_effect_attack", m_DashEffectAttribute, rkLoader );
	LoadAttackAttribute_e( "fly_effect_attack", m_FlyEffectAttribute, rkLoader );

	rkLoader.LoadString_e( "charge_effect_ani", "", szBuf, MAX_PATH );
	m_szChargeEffectAni = szBuf;
	rkLoader.LoadString_e( "jump_charge_effect_ani", "", szBuf, MAX_PATH );
	m_szJumpChargeEffectAni = szBuf;
	rkLoader.LoadString_e( "dash_charge_effect_ani", "", szBuf, MAX_PATH );
	m_szDashChargeEffectAni = szBuf;
	rkLoader.LoadString_e( "fly_charge_effect_ani", "", szBuf, MAX_PATH );
	m_szFlyChargeEffectAni = szBuf;
	m_fChargeEffectAniRate = rkLoader.LoadFloat_e( "charge_effect_ani_rate", FLOAT1 );
	m_fChargeEffectEndJumpAmt = rkLoader.LoadFloat_e( "charge_effect_end_jump_amt", FLOAT1 );
	m_dwEffectAttackRepeatTime = rkLoader.LoadInt_e( "charge_effect_repeat_time", 0 );

	int iCount = rkLoader.LoadInt_e( "max_effect_attack_count", 0 );
	for( int i = 0; i < iCount; ++i )
	{
		D3DXVECTOR3 vOffsetPos;
		wsprintf_e( szBuf, "effect_attack%d_offset_x", i+1 );
		vOffsetPos.x = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "effect_attack%d_offset_y", i+1 );
		vOffsetPos.y = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "effect_attack%d_offset_z", i+1 );
		vOffsetPos.z = rkLoader.LoadFloat( szBuf, 0.0f );
		m_vEffectOffsetList.push_back( vOffsetPos );
	}
	rkLoader.LoadString_e( "effect_name", "", szBuf, MAX_PATH );
	m_szEffectName = szBuf;
	rkLoader.LoadString_e( "board_effect", "", szBuf, MAX_PATH );
	m_szBoardEffectName = szBuf;

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", FLOAT1 );

	m_dwGatheringTime = (DWORD)rkLoader.LoadInt_e( "gathering_time", 0 );
	m_dwFlyChargeTime = (DWORD)rkLoader.LoadInt_e( "fly_charge_time", 0 );
	m_dwFlyGatheringTime = (DWORD)rkLoader.LoadInt_e( "fly_gathering_time", 0 );
}

void ioNeoItem::LoadFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_FlyInfo.Init();

	//Fly Start
	rkLoader.LoadString_e( "fly_start_ani", "", szBuf, MAX_PATH );
	m_FlyInfo.m_StartAni = szBuf;
	m_FlyInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "fly_start_ani_rate", FLOAT1 );

	m_FlyInfo.m_fStartAngle = rkLoader.LoadFloat_e( "fly_start_angle", 0.0f );
	m_FlyInfo.m_fStartMaxRange = rkLoader.LoadFloat_e( "fly_start_max_range", 0.0f );
	m_FlyInfo.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "fly_start_duration", 0 );

	//Delay
	rkLoader.LoadString_e( "fly_delay_ani", "", szBuf, MAX_PATH );
	m_FlyInfo.m_DelayAni = szBuf;
	m_FlyInfo.m_fDelayAniRate = rkLoader.LoadFloat_e( "fly_delay_ani_rate", FLOAT1 );

	//Move
	rkLoader.LoadString_e( "fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_FlyInfo.m_MoveAniFr = szBuf;
	rkLoader.LoadString_e( "fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_FlyInfo.m_MoveAniBk = szBuf;
	rkLoader.LoadString_e( "fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_FlyInfo.m_MoveAniRt = szBuf;
	rkLoader.LoadString_e( "fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_FlyInfo.m_MoveAniLt = szBuf;

	m_FlyInfo.m_fMoveAniRate = rkLoader.LoadFloat_e( "fly_move_ani_rate", FLOAT1 );
	m_FlyInfo.m_fMoveForceAmt = rkLoader.LoadFloat_e( "fly_force_move", 0.0f );
	m_FlyInfo.m_fMoveForceFric = rkLoader.LoadFloat_e( "fly_force_friction", FLOAT1 );

	//Target
	m_FlyInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_FlyInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_FlyInfo.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_FlyInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_FlyInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );
	m_FlyInfo.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );

	//Attack Move
	LoadAttackAttribute_e( "fly_attack_move", m_FlyInfo.m_AttackMove, rkLoader );
	m_FlyInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "fly_attack_move_speed", 0.0f );
	m_FlyInfo.m_fAttackMoveRange = rkLoader.LoadFloat_e( "fly_attack_move_range", FLOAT100 );
	m_FlyInfo.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "fly_jump_amt", 0.0f );
}

void ioNeoItem::LoadJumpFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_JumpFlyInfo.Init();

	//Fly Start
	rkLoader.LoadString_e( "jump_fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpFlyInfo.m_StartAni = szBuf;
	m_JumpFlyInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "jump_fly_start_ani_rate", FLOAT1 );

	m_JumpFlyInfo.m_fStartAngle = rkLoader.LoadFloat_e( "jump_fly_start_angle", 0.0f );
	m_JumpFlyInfo.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_fly_start_max_range", 0.0f );
	m_JumpFlyInfo.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_fly_start_duration", 0 );

	//Delay
	rkLoader.LoadString_e( "jump_fly_delay_ani", "", szBuf, MAX_PATH );
	m_JumpFlyInfo.m_DelayAni = szBuf;
	m_JumpFlyInfo.m_fDelayAniRate = rkLoader.LoadFloat_e( "jump_fly_delay_ani_rate", FLOAT1 );

	//Move
	rkLoader.LoadString_e( "jump_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_JumpFlyInfo.m_MoveAniFr = szBuf;
	rkLoader.LoadString_e( "jump_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_JumpFlyInfo.m_MoveAniBk = szBuf;
	rkLoader.LoadString_e( "jump_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_JumpFlyInfo.m_MoveAniRt = szBuf;
	rkLoader.LoadString_e( "jump_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_JumpFlyInfo.m_MoveAniLt = szBuf;

	m_JumpFlyInfo.m_fMoveAniRate = rkLoader.LoadFloat_e( "jump_fly_move_ani_rate", FLOAT1 );
	m_JumpFlyInfo.m_fMoveForceAmt = rkLoader.LoadFloat_e( "jump_fly_force_move", 0.0f );
	m_JumpFlyInfo.m_fMoveForceFric = rkLoader.LoadFloat_e( "jump_fly_force_friction", FLOAT1 );

	//Target
	m_JumpFlyInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_fly_target_wound_type", TWT_ALL );
	m_JumpFlyInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_fly_target_angle", 45.0f );
	m_JumpFlyInfo.m_fTargetRange = rkLoader.LoadFloat_e( "jump_fly_target_range", 45.0f );
	m_JumpFlyInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_fly_target_up_height", 45.0f );
	m_JumpFlyInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_fly_target_down_height", 45.0f );
	m_JumpFlyInfo.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "jump_fly_target_marker_range", 45.0f );

	//Attack Move
	LoadAttackAttribute_e( "jump_fly_attack_move", m_JumpFlyInfo.m_AttackMove, rkLoader );
	m_JumpFlyInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "jump_fly_attack_move_speed", 0.0f );
	m_JumpFlyInfo.m_fAttackMoveRange = rkLoader.LoadFloat_e( "jump_fly_attack_move_range", FLOAT100 );
	m_JumpFlyInfo.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "jump_fly_jump_amt", 0.0f );
}

void ioNeoItem::LoadFlyInfo( ioINILoader &rkLoader, const char* szName, ValkyrieFlyInfo& DestInfo )
{
	DestInfo.Init();

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_wait_ani", szName );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	DestInfo.m_WaitAnimation = szBuf;

	wsprintf_e( szKey, "%s_wait_duration", szName );
	DestInfo.m_dwWaitDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_wait_duration", szName );
	DestInfo.m_dwChargeFlyMinSpeedDuration = (DWORD)rkLoader.LoadInt(szKey, 0 );

	wsprintf_e( szKey, "%s_angle", szName );
	DestInfo.m_fChargeFlyAngle = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_max_range", szName );
	DestInfo.m_fChargeFlyMaxRange = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_max_speed", szName );
	DestInfo.m_fChargeFlyMaxSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_min_speed", szName );
	DestInfo.m_fChargeFlyMinSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_accel_speed", szName );
	DestInfo.m_fChargeFlyAccelSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_jump_amt", szName );
	DestInfo.m_fChargeFlyJumpAmt = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_attack", szName );
	LoadAttackAttribute( szKey, DestInfo.m_Attribute, rkLoader );
}

//Normal Attack
void ioNeoItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_szTargetName.Clear();

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

bool ioNeoItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioNeoItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
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

void ioNeoItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
			{
				ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
				if( eNewDirKey != ioUserKeyInput::DKI_NONE )
				{
					pOwner->SetTargetRotToDirKey( eNewDirKey );
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << static_cast<byte>( ST_SET_DIR );
						kPacket << (int)eNewDirKey;
						kPacket << pOwner->GetWorldPosition();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
				return;
			}
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioNeoItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeGatheringState( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioNeoItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	int iMaxEffectCount = m_vEffectOffsetList.size();
	int iCurEffectCount = m_vEffectInfoList.size();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && iCurEffectCount > 0 )
	{
		if( m_dwMotionEndTime <= FRAMEGETTIME() )
			SetAttackState( pOwner, true, OBS_NORMAL );
	}
	else if( iCurEffectCount < iMaxEffectCount )
		SetHalfAttackState( pOwner, true, OBS_NORMAL );
	else
		ChangeToNormalAttack( pOwner );
}

bool ioNeoItem::IsEnableChargeState( bool bHalf )
{
	int iMaxEffectCount = m_vEffectOffsetList.size();
	int iCurEffectCount = m_vEffectInfoList.size();
	if( bHalf && iCurEffectCount < iMaxEffectCount )
		return true;
	else if( !bHalf && iCurEffectCount > 0 )
		return true;

	return false;
}

void ioNeoItem::SetAttackState( ioBaseChar *pOwner, bool bSendNet, int iState )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetState( CS_NEO_ATTACK );
	m_ChargeState = CS_EFFECT_ATTACK;
	m_OBotState = (OBotState)iState;
	m_bEffectAttackKey = true;

	AttackAttribute cInfo;
	switch( iState )
	{
	case OBS_NORMAL:	cInfo = m_NormalEffectAttribute;	break;
	case OBS_JUMP:		cInfo = m_JumpEffectAttribute;		break;
	case OBS_DASH:		cInfo = m_DashEffectAttribute;		break;
	}

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurAni = cInfo.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	pOwner->SetAutoTarget( ATT_NORMAL );

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	float fTimeRate = cInfo.m_fAttackAniRate;
	DWORD dwPreDelay = cInfo.m_dwPreDelay;

	D3DXVECTOR3 vEffectPos = RemoveEffect( pOwner );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->SetReservedSliding( cInfo.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->RefreshFireTimeList( iAniID, cInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay, false, vEffectPos );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * cInfo.m_fAttackAniRate );
	
	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << iState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::SetHalfAttackState( ioBaseChar *pOwner, bool bSendNet, int iState )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true  );

	pOwner->SetState( CS_NEO_ATTACK );
	m_ChargeState = CS_EFFECT_CHARGING;
	m_OBotState = (OBotState)iState;

	ioHashString szCurAni;
	switch( iState )
	{
	case OBS_NORMAL:	szCurAni = m_szChargeEffectAni;		break;
	case OBS_JUMP:		szCurAni = m_szJumpChargeEffectAni;	break;
	case OBS_DASH:		szCurAni = m_szDashChargeEffectAni;	break;
	}

	int iAniID = pGrp->GetAnimationIdx( szCurAni );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ani Name is not exist", __FUNCTION__ );
		return;
	}

	float fTimeRate = m_fChargeEffectAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0 );	

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
	}
	CreateEffectChargeEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << iState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::SetFlyAttackState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_FlyState = SFS_CHARGE_ATTACK;
	m_bEffectAttackKey = true;
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_szCurAni = m_FlyEffectAttribute.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	pOwner->SetAutoTarget( ATT_NORMAL );

	int iAniID = pGrp->GetAnimationIdx( m_FlyEffectAttribute.m_AttackAnimation );
	float fTimeRate = m_FlyEffectAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_FlyEffectAttribute.m_dwPreDelay;

	D3DXVECTOR3 vEffectPos = RemoveEffect( pOwner );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->SetReservedSliding( m_FlyEffectAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->RefreshFireTimeList( iAniID, m_FlyEffectAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay, false, vEffectPos );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * m_FlyEffectAttribute.m_fAttackAniRate );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_NEO_FLY );
		kPacket << static_cast<byte>( m_FlyState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::SetFlyHalfAttackState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true  );

	m_FlyState = SFS_CHARGE_EFFECT;
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	int iAniID = pGrp->GetAnimationIdx( m_szFlyChargeEffectAni );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ani Name is not exist", __FUNCTION__ );
		return;
	}

	float fTimeRate = m_fChargeEffectAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0 );	

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
	}
	CreateEffectChargeEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_NEO_FLY );
		kPacket << static_cast<byte>( m_FlyState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::CreateEffectChargeEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_szEffectName.IsEmpty() )
		return;

	int iEffectCount = m_vEffectOffsetList.size();
	if( iEffectCount == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s - Effect Charge Offset List Is Empty", __FUNCTION__ );
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vPosOffset = pOwner->GetWorldOrientation() * m_vEffectOffsetList[m_iCurEffectChargeIndex];
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vPosOffset;
	D3DXVECTOR3 vScale = pOwner->GetWorldScale();
	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szEffectName, vPos, vScale );
	if( pMapEffect )
	{
		EffectInfo cInfo;
		cInfo.m_dwEffectID = pMapEffect->GetUniqueID();
		cInfo.m_iPosIndex = m_iCurEffectChargeIndex;
		m_vEffectInfoList.push_back( cInfo );
	}

	m_iCurEffectChargeIndex++;
	if( m_iCurEffectChargeIndex >= iEffectCount )
		m_iCurEffectChargeIndex = 0;
}

void ioNeoItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( m_vEffectInfoList.empty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	EffectInfoVec::iterator iter = m_vEffectInfoList.begin();
	while( iter != m_vEffectInfoList.end() )
	{
		EffectInfo cInfo = *iter;
		ioMapEffect* pMapEffect = pStage->FindMapEffect( cInfo.m_dwEffectID );
		if( !pMapEffect || !pMapEffect->IsLive() )
			iter = m_vEffectInfoList.erase( iter );
		else
		{
			D3DXVECTOR3 vPosOffset = vPos + pOwner->GetWorldOrientation() * m_vEffectOffsetList[cInfo.m_iPosIndex];
			pMapEffect->SetWorldPosition( vPosOffset );
			pMapEffect->SetWorldOrientation( qtRot );
			++iter;
		}
	}
}

void ioNeoItem::ChangeGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szGatheringAni );
	float fTimeRate  = m_fGatheringAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_GATHERING;
	if( m_dwGatheringTime == 0 )
		m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );
	else
		m_dwMotionEndTime = dwCurTime + m_dwGatheringTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

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

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( !bFullTime || !IsEnableGauge() )
	{
		float fJumpPower = pOwner->GetJumpPower() * m_fJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		if( m_pExtendJump )
		{
			m_pExtendJump->SetJumpping( pOwner, bFullTime );
		}
	}
	else
	{
		SetFlyReadyState( pOwner, false );
	}
}

void ioNeoItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

bool ioNeoItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
		return false;

	fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );
	return false;
}

void ioNeoItem::SetFlyReadyState( ioBaseChar *pOwner, bool bJumpState )
{
	if( !pOwner )
		return;

	ClearFlyData();
	m_dwMotionEndTime = 0;

	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	if( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetRotToDirKey( eDir );
	}

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetState( CS_NEO_FLY );
	CreateBoardEffect( pOwner );
	CheckDecreaseGauge();

	m_CurFlyInfo.Init();
	if( bJumpState )
		m_CurFlyInfo = m_JumpFlyInfo;
	else
		m_CurFlyInfo = m_FlyInfo;

	m_fCurTargetRange = m_CurFlyInfo.m_fTargetRange;
	m_fCurTargetAngle = m_CurFlyInfo.m_fTargetAngle;
	m_fCurTargetUpHeight = m_CurFlyInfo.m_fTargetUpHeight;
	m_fCurTargetDownHeight = m_CurFlyInfo.m_fTargetDownHeight;
	m_CurTargetWoundType = m_CurFlyInfo.m_TargetWoundType;
	m_FlyState = SFS_READY;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_NEO_FLY );
		kPacket << static_cast<byte>( m_FlyState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << bJumpState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::SetFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ShowEquipItemMesh( false, ES_WEAPON );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurFlyInfo.m_StartAni );
	float fTimeRate  = m_CurFlyInfo.m_fStartAniRate;
	DWORD dwPreDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_FlyState = SFS_START;

	m_dwMotionStartTime = FRAMEGETTIME(); 
	pOwner->SetTargetMarkerRange( m_CurFlyInfo.m_fTargetMarkerRange );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_NEO_FLY );
		kPacket << static_cast<byte>( m_FlyState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::ProcessFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime	= FRAMEGETTIME();
	switch( m_FlyState )
	{
	case SFS_READY:
		SetFlyState( pOwner );
		break;
	case SFS_START:
		ProcessFlyStartState( pOwner );
		break;
	case SFS_DELAY:
		ProcessFlyDelayState( pOwner );
		break;
	case SFS_MOVE:
		ProcessFlyMoveState( pOwner );
		break;
	case SFS_ATTACK_MOVE:
		{
			DWORD dwCurTime = FRAMEGETTIME();
			if( m_dwMotionEndTime < dwCurTime )
			{
				ChangeFlyEnd( pOwner );
				return;
			}
		}
		break;
	case SFS_FIRE:
		if( ProcessMove( pOwner, m_FlyFireInfo, ioMath::VEC3_ZERO, m_dwUpStartTime, m_fUpCurrRange, false ) != MS_MOVE )
		{
			pOwner->SetSKillEndJumpState( m_FlyFireInfo.m_fChargeFlyJumpAmt );
		}
		ProcessWeapon( pOwner );
		break;
	case SFS_CHARGING:
		ProcessFlyChargingState( pOwner );
		break;
	case SFS_GATHERING:
		ProcessFlyGatheringState( pOwner );
		break;
	case SFS_CHARGE_EFFECT:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
			ChangeFlyDelay( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		break;
	case SFS_CHARGE_ATTACK:
		if( m_bEffectAttackKey && pOwner->IsAttackKeyRelease() )
			m_bEffectAttackKey = false;

		int iCurEffectCount = m_vEffectInfoList.size();
		if( m_bEffectAttackKey && m_dwMotionStartTime + m_dwEffectAttackRepeatTime < dwCurTime && iCurEffectCount > 0 )
		{
			SetFlyAttackState( pOwner, true );
			return;
		}
		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
			ChangeFlyDelay( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		break;
	}
	ProcessFlyMoving( pOwner );
}

void ioNeoItem::ProcessFlyChargingState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionEndTime <= FRAMEGETTIME() )
		{
			ChangeFlyGatheringState( pOwner );
		}
	}
	else
	{
		ChangeFlyAttackMove( pOwner );
	}
}

void ioNeoItem::ProcessFlyGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && IsEnableChargeState( false ) )
	{
		if( m_dwMotionEndTime <= FRAMEGETTIME() )
		{
			SetFlyAttackState( pOwner, true );
		}
	}
	else if( IsEnableChargeState( true )  )
		SetFlyHalfAttackState( pOwner, true );
	else
		ChangeFlyAttackMove( pOwner );
}

void ioNeoItem::ProcessAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime	= FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_EFFECT_CHARGING:
		if( m_OBotState == OBS_JUMP )
		{
			pOwner->SetJumpPower( 0 );
			pOwner->SetAirJumpPower( 0 );
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
		}
		if( m_dwMotionEndTime < dwCurTime )
		{
			if( m_OBotState == OBS_JUMP )
				pOwner->SetFallState( true );
			else
				pOwner->SetState( CS_DELAY );
			return;
		}
		break;
	case CS_EFFECT_ATTACK:
		if( m_OBotState == OBS_JUMP )
		{
			pOwner->SetJumpPower( 0 );
			pOwner->SetAirJumpPower( 0 );
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
		}
		if( m_bEffectAttackKey && pOwner->IsAttackKeyRelease() )
			m_bEffectAttackKey = false;

		int iCurEffectCount = m_vEffectInfoList.size();
		if( m_bEffectAttackKey && m_dwMotionStartTime + m_dwEffectAttackRepeatTime < dwCurTime && iCurEffectCount > 0 )
		{
			SetAttackState( pOwner, true, m_OBotState );
			return;
		}
		if( m_dwMotionEndTime < dwCurTime )
		{
			if( m_OBotState == OBS_JUMP )
				pOwner->SetSKillEndJumpState( m_fChargeEffectEndJumpAmt );
			else
				pOwner->SetState( CS_DELAY );
			return;
		}
		break;
	}
}

ioNeoItem::MoveState ioNeoItem::ProcessMove( ioBaseChar *pOwner, const ValkyrieFlyInfo& rkFlyInfo, const D3DXVECTOR3& vFindDir, DWORD& dwStartTime, float& fCurrRange, bool bCheckLanding )
{
	if( !pOwner )
		return MS_END;

	DWORD dwCurTime		= FRAMEGETTIME();
	float fTimePerSec	= g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vMoveDir = ioMath::VEC3_ZERO;
	if( vFindDir == ioMath::VEC3_ZERO )
	{
		D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( rkFlyInfo.m_fChargeFlyAngle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		vMoveDir = qtNewRot * vTargetDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
	}
	else
	{
		vMoveDir = vFindDir;
	}

	float fCurSpeed = rkFlyInfo.m_fChargeFlyMinSpeed;
	DWORD dwGapTime = dwCurTime - ( dwStartTime + rkFlyInfo.m_dwChargeFlyMinSpeedDuration );

	if( dwStartTime + rkFlyInfo.m_dwChargeFlyMinSpeedDuration < dwCurTime )
	{
		float fTimeRate = (float)dwGapTime / FLOAT1000;
		float fCurAccel = rkFlyInfo.m_fChargeFlyAccelSpeed * fTimeRate;

		fCurSpeed += fCurAccel;
	}

	if( fCurSpeed >= rkFlyInfo.m_fChargeFlyMaxSpeed )
		fCurSpeed = rkFlyInfo.m_fChargeFlyMaxSpeed;

	float fDistance = fCurSpeed * fTimePerSec;

	MoveState eRangeLimit = MS_END;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( fCurrRange + fDistance < rkFlyInfo.m_fChargeFlyMaxRange )
	{
		eRangeLimit = MS_MOVE;
		vMoveAmt = vMoveDir * fDistance;
		fCurrRange += fDistance;
	}
	else
	{
		eRangeLimit = MS_END;

		float fGapDistance = rkFlyInfo.m_fChargeFlyMaxRange - fCurrRange;
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
	{
		pOwner->SetWorldPosition( vPos );
	}

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( bCheckLanding && fHeightGap <= FLOAT10 )
	{
		return MS_LANDING;
	}

	return eRangeLimit;
}

void ioNeoItem::ProcessFlyStartState( ioBaseChar *pOwner )
{
	if( m_dwMotionStartTime + m_CurFlyInfo.m_dwStartDuration < FRAMEGETTIME() )
	{
		ChangeFlyDelay( pOwner );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		return;
	}
	else if( !ProcessFlyStartMoving( pOwner ) )
	{
		ChangeFlyDelay( pOwner );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		return;
	}
}

bool ioNeoItem::ProcessFlyStartMoving( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	float fJumpPoweru = pOwner->GetCurJumpPowerAmt();

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurFlyInfo.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurFlyInfo.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurFlyInfo.m_fStartMaxRange / (float)m_CurFlyInfo.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_CurFlyInfo.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurFlyInfo.m_fStartMaxRange - m_fFlyCurRange;
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

void ioNeoItem::ChangeFlyDelay( ioBaseChar *pOwner )
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

	int iAniID	     = pGrp->GetAnimationIdx( m_CurFlyInfo.m_DelayAni );
	float fTimeRate  = m_CurFlyInfo.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_FlyState = SFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwMotionStartTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_NEO_FLY );
		kPacket << static_cast<byte>( m_FlyState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioNeoItem::ProcessFlyDelayState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_fCurGauge <= 0.f )
	{
		ChangeFlyEnd( pOwner );
		return;
	}

	CheckAttTarget( pOwner );

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsAttackKey() )
		{
			ChangeFlyChargingState( pOwner );
			return;
		}
		if( pOwner->IsJumpKey() )
		{
			ChangeFlyEnd( pOwner );
			return;
		}
		if( pOwner->IsDefenseKey() )
		{
			ChangeFlyFireState( pOwner, ioUserKeyInput::DKI_NONE );
			return;
		}
	}
	CheckFlyKeyInput( pOwner );
}

void ioNeoItem::CheckFlyKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;

	// Check Dir
	if( m_FlyState == SFS_DELAY )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			if( eNewDirKey != ioUserKeyInput::DKI_NONE )
				bChangeDir = true;

			m_CurDirKey = eNewDirKey;
		}
	}

	if( bChangeDir )
	{
		ChangeFlyMove( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_NEO_FLY );
			kPacket << static_cast<byte>( SFS_MOVE );
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetTargetRot();
			kPacket << bChangeDir;
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioNeoItem::ChangeFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = m_CurDirKey;
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
		if( !m_CurFlyInfo.m_MoveAniFr.IsEmpty() )
			CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniFr, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_CurFlyInfo.m_MoveAniRt.IsEmpty() )
				CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniRt, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_CurFlyInfo.m_MoveAniFr.IsEmpty() )
				CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniFr, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_CurFlyInfo.m_MoveAniRt.IsEmpty() )
				CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniRt, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_CurFlyInfo.m_MoveAniBk.IsEmpty() )
				CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniBk, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_CurFlyInfo.m_MoveAniBk.IsEmpty() )
			CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniBk, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_CurFlyInfo.m_MoveAniLt.IsEmpty() )
				CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniLt, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_CurFlyInfo.m_MoveAniBk.IsEmpty() )
				CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniBk, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_CurFlyInfo.m_MoveAniLt.IsEmpty() )
				CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniLt, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_CurFlyInfo.m_MoveAniFr.IsEmpty() )
				CheckFlyMoveAni( pOwner, m_CurFlyInfo.m_MoveAniFr, m_CurFlyInfo.m_fMoveAniRate, vMoveDir );
		}
		break;
	}

	m_FlyState = SFS_MOVE;
	m_dwMotionStartTime = 0;
}

void ioNeoItem::CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
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

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwFlyForceTime = dwCurTime + dwTime;

	m_vFlyForceDir = vDir;
}

void ioNeoItem::ProcessFlyMoveState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_fCurGauge <= 0.f )
	{
		ChangeFlyEnd( pOwner );
		return;
	}
	CheckAttTarget( pOwner );

	if( m_dwMotionEndTime < dwCurTime )
	{
		ChangeFlyDelay( pOwner );
		return;
	}	
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsAttackKey() )
		{
			ChangeFlyChargingState( pOwner );
			return;
		}
		if( pOwner->IsJumpKey() )
		{
			ChangeFlyEnd( pOwner );
			return;
		}
		if( pOwner->IsDefenseKey() )
		{
			ChangeFlyFireState( pOwner, ioUserKeyInput::DKI_NONE );
			return;
		}
	}
	CheckFlyKeyInput( pOwner );
}

void ioNeoItem::ChangeFlyChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szFlyChargingAni );
	float fTimeRate  = m_fFlyChargingAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_FlyState = SFS_CHARGING;
	if( m_dwFlyChargeTime == 0 )
		m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	else
		m_dwMotionEndTime = dwCurTime + m_dwFlyChargeTime;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_NEO_FLY );
		kPacket << static_cast<byte>( m_FlyState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::ChangeFlyGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szFlyGatheringAni );
	float fTimeRate  = m_fFlyGatheringAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_FlyState = SFS_GATHERING;
	if( m_dwFlyGatheringTime == 0 )
		m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	else
		m_dwMotionEndTime = dwCurTime + m_dwFlyGatheringTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_NEO_FLY );
		kPacket << static_cast<byte>( m_FlyState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::ChangeFlyAttackMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurFlyInfo.m_fAttackMoveSpeed <= 0.0f )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurFlyInfo.m_AttackMove.m_AttackAnimation );
	float fTimeRate = m_CurFlyInfo.m_AttackMove.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurFlyInfo.m_AttackMove.m_dwPreDelay;
	DWORD dwEndDelay = m_CurFlyInfo.m_AttackMove.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_CurFlyInfo.m_AttackMove.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->SetReservedSliding( m_CurFlyInfo.m_AttackMove.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

	DWORD dwMoveDuration = 0;
	if( pOwner->IsNeedProcess() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );

		bool bTargetOk = false;
		if( pTarget && CheckTargetValidate(pOwner) )
			bTargetOk = true;

		m_dwMotionEndTime = dwCurTime;
		if( !bTargetOk )
		{
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			dwMoveDuration = (DWORD)(m_CurFlyInfo.m_fAttackMoveRange / m_CurFlyInfo.m_fAttackMoveSpeed * FLOAT1000);
			m_dwMotionEndTime += dwMoveDuration;

			m_vFlyMoveDir = vDir;
			m_fFlyMoveSpeed = m_CurFlyInfo.m_fAttackMoveSpeed;
		}
		else
		{
			D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			dwMoveDuration = (DWORD)(fRange / m_CurFlyInfo.m_fAttackMoveSpeed * FLOAT1000);
			m_dwMotionEndTime += dwMoveDuration;

			m_vFlyMoveDir = vDir;
			m_fFlyMoveSpeed = m_CurFlyInfo.m_fAttackMoveSpeed;
		}
	}
	m_FlyState = SFS_ATTACK_MOVE;
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_NEO_FLY );
		kPacket << static_cast<byte>( m_FlyState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vFlyMoveDir;
		kPacket << m_fFlyMoveSpeed;
		kPacket << dwMoveDuration;
		kPacket << dwCurBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::ChangeFlyEnd( ioBaseChar *pOwner )
{
	m_FlyState = SFS_END;

	if( pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_CurFlyInfo.m_fFlyJumpAmt, FLOAT1, false, true, true );
	}
}

void ioNeoItem::ChangeFlyFireState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		eDir = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eDir );
	}

	m_FlyState = SFS_FIRE;

	SetAttackAttribute( pOwner, m_FlyFireInfo.m_Attribute );
	m_dwUpStartTime	= FRAMEGETTIME();
	m_fUpCurrRange = 0.0f;

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_FlyFireInfo.m_fChargeFlyAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	m_vAttackDir = -vMoveDir;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_FLY_FIRE );
		kPacket << static_cast<byte>( eDir );
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNeoItem::SetAttackAttribute( ioBaseChar *pOwner, const AttackAttribute& Attr )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( Attr.m_AttackAnimation );
	float fTimeRate  = Attr.m_fAttackAniRate;
	DWORD dwPreDelay = Attr.m_dwPreDelay;
	DWORD dwEndDelay = Attr.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, Attr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( Attr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

void ioNeoItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
	if( pSimple )
	{
		RemoveWeapon( pSimple->GetOwner() );
		m_WeaponIndexList.push_back( pSimple->GetWeaponIndex() );
	}
}

void ioNeoItem::ProcessWeapon( ioBaseChar *pOwner )
{
	for( int i = 0; i < (int)m_WeaponIndexList.size(); ++i )
	{
		ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( g_WeaponMgr.FindWeapon( pOwner, m_WeaponIndexList[i] ) );
		if( pSimple )
		{
			D3DXVECTOR3 vPos	= pOwner->GetWorldPosition();			
			D3DXVECTOR3 vSide;
			D3DXVec3Cross( &vSide, &m_vAttackDir, &ioMath::UNIT_Y );
			D3DXVec3Normalize( &vSide, &vSide );
			pSimple->SetMoveDir( m_vAttackDir );
		}
	}
}

void ioNeoItem::RemoveWeapon( ioBaseChar *pOwner )
{
	for( IntVec::iterator iter = m_WeaponIndexList.begin(); iter != m_WeaponIndexList.end(); )
	{
		ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( g_WeaponMgr.FindWeapon( pOwner, *iter ) );
		if( pSimple )
		{
			pSimple->SetWeaponDead();			
			iter = m_WeaponIndexList.erase( iter );
		}
		else
		{
			iter++;
		}
	}	
}

void ioNeoItem::ProcessFlyMoving( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	if( m_dwFlyForceTime > 0 && m_dwFlyForceTime < dwCurTime )
	{
		m_dwFlyForceTime = 0;
		pOwner->SetForcePower( m_vFlyForceDir, m_CurFlyInfo.m_fMoveForceAmt, m_CurFlyInfo.m_fMoveForceFric );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
	vCurPos.y = max( vCurPos.y, fMapHeight );

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fFlyMoveSpeed > 0.0f && m_vFlyMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fFlyMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vFlyMoveDir * fFinalMoveAmt;
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
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}

//Search Target
void ioNeoItem::CheckAttTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( m_szTargetName.IsEmpty() && !m_bAimedBall )
	{
		FindAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_szTargetName.Clear();
			m_bAimedBall = true;
		}

		if( !m_szTargetName.IsEmpty() || m_bAimedBall )
		{
			pOwner->SetTargetMarkerTarget( m_szTargetName, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			
		}
	}
	else if( !CheckTargetValidate( pOwner ) )
	{
		FindAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_szTargetName.Clear();
			m_bAimedBall = true;
		}

		if( !m_szTargetName.IsEmpty() || m_bAimedBall )
		{
			pOwner->SetTargetMarkerTarget( m_szTargetName, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}
		else
		{
			pOwner->SetTargetMarkerTarget( m_szTargetName, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}
	}
}

void ioNeoItem::FindAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_szTargetName.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fCurTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fCurTargetRange * m_fCurTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;
		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fCurTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fCurTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_szTargetName = szTarget;
		}
	}
}

bool ioNeoItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget( false ) )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurTargetWoundType ) )
		return false;

	return true;
}

bool ioNeoItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fCurTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fCurTargetRange * m_fCurTargetRange;

	if( m_bAimedBall )
	{
		ioBall *pBall = pOwner->FindBall();
		if( !pBall )
			return false;

		D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fCurTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fCurTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
		if( !pTarget )
			return false;

		if( !IsEnableTargetState( pTarget ) )
			return false;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			return false;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fCurTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fCurTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	return true;
}

bool ioNeoItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) 
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fCurTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fCurTargetRange * m_fCurTargetRange;

	D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
	vDiff = vTargetPos - vCurPos;
	float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

	// 각도 체크
	if( fCurAngle != 0.0f )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vDiff2 = vDiff;
		vDiff2.y = 0.0f;
		D3DXVec3Normalize( &vDiff2, &vDiff2 );

		float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
		if( fCurValue < fHalfValue )
			return false;
	}

	// 높이 체크
	float fHeightGap = vTargetPos.y - vCurPos.y;
	if( fHeightGap > 0.0f && fHeightGap > m_fCurTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fCurTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

bool ioNeoItem::IsCollisionSkipState()
{
	switch( m_FlyState )
	{
	case SFS_START:
	case SFS_MOVE:
		return true;
	}

	return false;
}

void ioNeoItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );
}

void ioNeoItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	ioPlayStage *pStage = pOwner->GetCreator();
	RemoveEffectList( pStage );
	m_iCurEffectChargeIndex = 0;
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioNeoItem::OnReleasedByEndGame( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	RemoveEffectList( pStage );
	m_iCurEffectChargeIndex = 0;
}

void ioNeoItem::RemoveEffectList( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	EffectInfoVec::iterator iter = m_vEffectInfoList.begin();
	for( ; iter != m_vEffectInfoList.end() ; ++iter )
	{
		EffectInfo cInfo = *iter;
		ioMapEffect* pMapEffect = pStage->FindMapEffect( cInfo.m_dwEffectID );
		if( pMapEffect )
			pMapEffect->EndEffectForce();
	}
	m_vEffectInfoList.clear();
	m_iCurEffectChargeIndex = 0;
}

D3DXVECTOR3 ioNeoItem::RemoveEffect( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
	if( !pOwner )
		return vPos;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return vPos;

	if( m_vEffectInfoList.empty() )
		return vPos;

	EffectInfoVec::iterator iter = m_vEffectInfoList.begin();
	EffectInfo cInfo = *iter;
	ioMapEffect* pMapEffect = pStage->FindMapEffect( cInfo.m_dwEffectID );
	if( pMapEffect )
	{
		vPos = pMapEffect->GetWorldPosition();
		pMapEffect->EndEffectForce();
	}
	m_vEffectInfoList.erase( iter );
	return vPos;
}

void ioNeoItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btState;
	rkPacket >> btState;
	rkPacket >> m_iCurCombo;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( btState )
	{
	case CS_NORMAL_ATTACK:
		{
			if( !m_bChargeAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeToNormalAttack( pOwner );
		}
		break;
	case CS_GATHERING:
		ChangeGatheringState( pOwner );
		break;
	case CS_EFFECT_ATTACK:
		{
			int iState;
			rkPacket >> iState;
			SetAttackState( pOwner, false, iState );
		}
		break;
	case CS_EFFECT_CHARGING:
		{
			int iState;
			rkPacket >> iState;
			SetHalfAttackState( pOwner, false, iState );
		}
		break;
	}
}

void ioNeoItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btSyncType;
	rkPacket >> btSyncType;
	switch( btSyncType )
	{
	case ST_SET_TARGET:
		rkPacket >> m_szTargetName;
		break;
	case ST_SET_DIR:
		{
			int iDirKey;
			rkPacket >> iDirKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToDirKey( m_CurDirKey );
		}
		break;
	case ST_NEO_FLY:
		ApplyFly( pOwner, rkPacket );
		break;
	case ST_FLY_FIRE:
		{
			byte eDirKey;
			rkPacket >> eDirKey;
			pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)eDirKey );	
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ChangeFlyFireState( pOwner, (ioUserKeyInput::DirKeyInput)eDirKey );
		}
		break;
	}
}

void ioNeoItem::ApplyFly( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btFlyType;
	rkPacket >> btFlyType;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( btFlyType )
	{
	case SFS_READY:
		{
			bool bJumpState;
			rkPacket >> bJumpState;

			SetFlyReadyState( pOwner, bJumpState );
		}
		break;
	case SFS_START:
		SetFlyState( pOwner );
		break;
	case SFS_DELAY:
		ChangeFlyDelay( pOwner );
		break;
	case SFS_MOVE:
		{
			bool bChangeDir;
			int iDir;
			rkPacket >> bChangeDir;
			rkPacket >> iDir;

			if( bChangeDir )
			{
				pOwner->SetTargetRot( qtRot );
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
				ChangeFlyMove( pOwner );
			}
		}
		break;
	case SFS_ATTACK_MOVE:
		{
			DWORD dwMoveDuration;
			DWORD dwWeaponIndex;
			rkPacket >> m_vFlyMoveDir ;
			rkPacket >> m_fFlyMoveSpeed ;
			rkPacket >> dwMoveDuration;
			rkPacket >> dwWeaponIndex;

			m_dwMotionEndTime = FRAMEGETTIME() + dwMoveDuration;
			pOwner->SetWeaponIndexBase( dwWeaponIndex );
			ChangeFlyAttackMove( pOwner );
		}
		break;
	case SFS_CHARGE_ATTACK:
		SetFlyAttackState( pOwner, false );
		break;
	case SFS_CHARGE_EFFECT:
		SetFlyHalfAttackState( pOwner, false );
		break;
	case SFS_CHARGING:
		ChangeFlyChargingState( pOwner );
		break;
	case SFS_GATHERING:
		ChangeFlyGatheringState( pOwner );
		break;
	}
}

void ioNeoItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( pWoundChar->IsNeedProcess() && fDamage > 0.0f )
	{
		if( !m_szTargetName.IsEmpty() )
			return;

		m_szTargetName = pWoundChar->GetCharName();
		if( pWoundChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << static_cast<byte>( ST_SET_TARGET );
			kPacket << m_szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}	
}

void ioNeoItem::CreateBoardEffect( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwBoardEffectIndex == 0 )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioEffect *pEffect = pGrp->AttachEffect( m_szBoardEffectName, NULL, ioMath::QUAT_IDENTITY );
			if( !pEffect )	
				return;

			m_dwBoardEffectIndex = pEffect->GetUniqueID();
		}
	}
}

void ioNeoItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioNeoItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioNeoItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioNeoItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioNeoItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioNeoItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioNeoItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
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
	case CS_NEO_FLY:
		if( m_fCurGauge > 0.f )
		{
			float fGauge = m_fDecreaseGauge * fTimePerSec;

			m_fCurGauge -= fGauge;
			m_fCurGauge = max( 0.0f, m_fCurGauge );
		}
		return;
	}
}
//-----------------------------------------------------------------------------------------------------------
ioNeoFlySpecialState::ioNeoFlySpecialState()
{
}

ioNeoFlySpecialState::~ioNeoFlySpecialState()
{
}

void ioNeoFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNeoFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNeoItem )
		pNeoItem->ClearFlyData( pOwner );
}

bool ioNeoFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNeoItem && pNeoItem->IsCollisionSkipState( ) )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioNeoFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNeoItem )
		pNeoItem->ProcessFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNeoFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioNeoFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}

ioNeoAttackSpecialState::ioNeoAttackSpecialState()
{
}

ioNeoAttackSpecialState::~ioNeoAttackSpecialState()
{
}

void ioNeoAttackSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNeoAttackSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNeoItem )
		pNeoItem->ClearAttackState( pOwner );
}

void ioNeoAttackSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNeoItem )
		pNeoItem->ProcessAttackState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNeoAttackSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioNeoAttackSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	return true;
}