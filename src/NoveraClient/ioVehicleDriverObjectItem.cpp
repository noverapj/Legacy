#include "stdafx.h"

#include "ioVehicleDriverObjectItem.h"
#include "ioVehicleDummy.h"

#include "WeaponDefine.h"
#include "ioZoneEffectWeapon.h"
#include "ioPassengerBuff.h"
#include "FindPredicateImpl.h"

ioVehicleDriverObjectItem::ioVehicleDriverObjectItem()
{
	Init();
}

ioVehicleDriverObjectItem::ioVehicleDriverObjectItem( const ioVehicleDriverObjectItem &rhs )
	: ioObjectItem( rhs ),
	m_szRideStartAnimation( rhs.m_szRideStartAnimation ),
	m_fRideStartTimeRate( rhs.m_fRideStartTimeRate ),
	m_szRideRunLoopAnimation( rhs.m_szRideRunLoopAnimation ),
	m_fRideRunLoopTimeRate( rhs.m_fRideRunLoopTimeRate ),
	m_fRideRunSpeed( rhs.m_fRideRunSpeed ),
	m_dwRideRunLoopDuration( rhs.m_dwRideRunLoopDuration ),
	m_dwRideRotateTime( rhs.m_dwRideRotateTime ),
	m_szRidingDummyCharName( rhs.m_szRidingDummyCharName ),
	m_szRideJumpReadyAnimation( rhs.m_szRideJumpReadyAnimation ),
	m_fJumpReadyTimeRate( rhs.m_fJumpReadyTimeRate ),
	m_fRideJumpReadySpeed( rhs.m_fRideJumpReadySpeed ),
	m_fRideJumpPower( rhs.m_fRideJumpPower ),
	m_dwRideJumpRotateTime( rhs.m_dwRideJumpRotateTime ), 
	m_szRideJumppingAnimation( rhs.m_szRideJumppingAnimation ),
	m_fRideJumppingTimeRate( rhs.m_fRideJumppingTimeRate ),
	m_fRideJumppingSpeed( rhs.m_fRideJumppingSpeed ),
	m_szRideLandingAnimation( rhs.m_szRideLandingAnimation ),
	m_fRideLandingTimeRate( rhs.m_fRideLandingTimeRate ),
	m_fRideLandingSpeed( rhs.m_fRideLandingSpeed ),
	m_fRideChangeToLandHeight( rhs.m_fRideChangeToLandHeight ),
	m_LandingWeaponInfo( rhs.m_LandingWeaponInfo ),
	m_szRideEndAnimation( rhs.m_szRideEndAnimation ),
	m_fRideEndTimeRate( rhs.m_fRideEndTimeRate ),
	m_DriftLeftAttribute( rhs.m_DriftLeftAttribute ),
	m_DriftRightAttribute( rhs.m_DriftRightAttribute ),
	m_fDriftAngle( rhs.m_fDriftAngle ),
	m_fDriftSpeed( rhs.m_fDriftSpeed ),
	m_fDriftRotateTime( rhs.m_fDriftRotateTime ),
	m_fDriftForce( rhs.m_fDriftForce ),
	m_dwDriftDuration( rhs.m_dwDriftDuration ),
	m_dwDriftAttackGapTime( rhs.m_dwDriftAttackGapTime ),
	m_iTargetMax( rhs.m_iTargetMax ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_fTargetMaxAngle( rhs.m_fTargetMaxAngle ),
	m_dwAimmingCheckTime( rhs.m_dwAimmingCheckTime ),
	m_dwZeroAimedCheckTime( rhs.m_dwZeroAimedCheckTime ),
	m_dwAimmingFireGapTime( rhs.m_dwAimmingFireGapTime ),
	m_AimedTargetFireWeaponInfo( rhs.m_AimedTargetFireWeaponInfo ),
	m_ZeroAimedTargetFireWeaponInfo( rhs.m_ZeroAimedTargetFireWeaponInfo ),
	m_fNoneTargetMaxRange( rhs.m_fNoneTargetMaxRange ),
	m_fNoneTargetYOffset( rhs.m_fNoneTargetYOffset ),
	m_NoneTargetFireWeaponInfo( rhs.m_NoneTargetFireWeaponInfo ),
	m_iPassengerMax( rhs.m_iPassengerMax ),
	m_szPassengerBuff( rhs.m_szPassengerBuff ),
	m_szPassengerKickBuff( rhs.m_szPassengerKickBuff ),
	m_AttachWeaponInfoList( rhs.m_AttachWeaponInfoList )
{
	Init();
}

ioVehicleDriverObjectItem::~ioVehicleDriverObjectItem()
{
}

void ioVehicleDriverObjectItem::Init()
{
	m_eVehicleDriverState		= VDS_NONE;
	m_AimState					= ioTargetMarker::MS_AIMED;
	m_CurDirKey					= ioUserKeyInput::DKI_NONE;

	m_iRidingDummyCharIndex		= 0;

	m_dwRideStartEndTime		= 0;
	m_dwRideRunLoopStartTime	= 0;
	m_dwRideEndTime				= 0;

	m_dwJumpReadyEndTime		= 0;
	m_dwRideLandingEndTime		= 0;
	m_dwRideDriftEndTime		= 0;
	m_dwAimmingStartTime		= 0;
	m_dwZeroAimedStartTime		= 0;
	m_dwAimmingFireStartTime	= 0;
	m_dwDriftAttackStartTime	= 0;

	m_fTargetYaw				= 0.0f;	
	m_bLeftRot					= false;
}

void ioVehicleDriverObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	//Delay
	rkLoader.LoadString_e( "ride_start_animation", "", szBuf, MAX_PATH );
	m_szRideStartAnimation = szBuf;
	m_fRideStartTimeRate = rkLoader.LoadFloat_e( "ride_start_animation_time_rate", FLOAT1 );

	//Run
	rkLoader.LoadString_e( "ride_run_animation", "", szBuf, MAX_PATH );
	m_szRideRunLoopAnimation = szBuf;
	m_fRideRunLoopTimeRate = rkLoader.LoadFloat_e( "ride_run_animation_time_rate", FLOAT1 );

	m_fRideRunSpeed			= rkLoader.LoadFloat_e( "ride_run_speed", FLOAT1000 );
	m_dwRideRunLoopDuration	= rkLoader.LoadInt_e( "ride_run_duration", 10000 );
	m_dwRideRotateTime		= rkLoader.LoadInt_e( "ride_rotate_speed", 1000 );

	//dummy
	rkLoader.LoadString_e( "riding_dummy_char_name", "", szBuf, MAX_PATH );
	m_szRidingDummyCharName = szBuf;

	//Jump Ready
	rkLoader.LoadString_e( "ride_jump_ready", "", szBuf, MAX_PATH );
	m_szRideJumpReadyAnimation = szBuf;
	m_fJumpReadyTimeRate	= rkLoader.LoadFloat_e( "ride_jump_ready_time_rate", FLOAT1 );
	m_fRideJumpReadySpeed	= rkLoader.LoadFloat_e( "ride_jump_ready_speed", FLOAT1000 );	
	m_fRideJumpPower		= rkLoader.LoadFloat_e( "ride_jump_power", 1500.0f );
	m_dwRideJumpRotateTime	= rkLoader.LoadInt_e( "ride_jump_rotate_speed", 1000 );
	
	//Jumpping
	rkLoader.LoadString_e( "ride_jumpping", "", szBuf, MAX_PATH );
	m_szRideJumppingAnimation = szBuf;
	m_fRideJumppingTimeRate = rkLoader.LoadFloat_e( "ride_jumpping_time_rate", FLOAT1 );
	m_fRideJumppingSpeed	= rkLoader.LoadFloat_e( "ride_jumpping_speed", FLOAT1000 );

	//Lading
	rkLoader.LoadString_e( "ride_landing_animation", "", szBuf, MAX_PATH );
	m_szRideLandingAnimation = szBuf;
	m_fRideLandingTimeRate		= rkLoader.LoadFloat_e( "ride_landing_animation_time_rate", FLOAT1 );
	m_fRideLandingSpeed			= rkLoader.LoadFloat_e( "ride_landing_speed", FLOAT1000 );
	m_fRideChangeToLandHeight	= rkLoader.LoadFloat_e( "ride_change_to_land_height", FLOAT10 );	

	LoadWeaponInfo( "ride_randing", m_LandingWeaponInfo, rkLoader );

	//Drift
	LoadAttackAttribute_e( "drift_left_attack", m_DriftLeftAttribute, rkLoader );
		
	LoadAttackAttribute_e( "drift_right_attack", m_DriftRightAttribute, rkLoader );

	m_fDriftAngle			= rkLoader.LoadFloat_e( "drift_angle",FLOAT90 );
	m_fDriftSpeed			= rkLoader.LoadFloat_e( "drift_speed",FLOAT90 );
	m_fDriftRotateTime		= rkLoader.LoadFloat_e( "drift_rotate_speed",FLOAT90 );
	m_fDriftForce			= rkLoader.LoadFloat_e( "drift_force", FLOAT100 );
	m_dwDriftDuration		= rkLoader.LoadInt_e( "drift_duration", 3000 );
	m_dwDriftAttackGapTime	= rkLoader.LoadInt_e( "drift_attack_gap_time", 300 );

	//end
	rkLoader.LoadString_e( "ride_end_animation", "", szBuf, MAX_PATH );
	m_szRideEndAnimation	= szBuf;
	m_fRideEndTimeRate		= rkLoader.LoadFloat_e( "ride_end_animation_time_rate", FLOAT1 );

	//multiple target
	m_iTargetMax			= rkLoader.LoadFloat_e( "target_max", 8 );
	m_fTargetAngle			= rkLoader.LoadFloat_e( "target_angle", 45.0f );
	m_fTargetMaxRange		= rkLoader.LoadFloat_e( "target_max_range", FLOAT1000 );
	m_fTargetMinRange		= rkLoader.LoadFloat_e( "target_min_range", 0.0f );	
	m_fTargetMaxAngle		= rkLoader.LoadFloat_e( "target_max_angle", 125.0f );

	m_dwAimmingCheckTime	= rkLoader.LoadInt_e( "aimming_time", 300 );
	m_dwZeroAimedCheckTime	= rkLoader.LoadInt_e( "zero_aimed_time", 300 );
	m_dwAimmingFireGapTime	= rkLoader.LoadInt_e( "aimming_fire_gap_time", 300 );

	//none target
	m_fNoneTargetMaxRange	= rkLoader.LoadFloat_e( "none_target_max_range", 200.0f );
	m_fNoneTargetYOffset	= rkLoader.LoadFloat_e( "none_target_y_offset", 70.0f );

	//Weapon
	LoadWeaponInfo( "aimed_target_fire", m_AimedTargetFireWeaponInfo, rkLoader );
	LoadWeaponInfo( "zero_aimed_target_fire", m_ZeroAimedTargetFireWeaponInfo, rkLoader );
	LoadWeaponInfo( "none_target_fire", m_NoneTargetFireWeaponInfo, rkLoader );

	//Passenger
	m_iPassengerMax	= rkLoader.LoadInt_e( "passenger_max", 1 );
	rkLoader.LoadString_e( "passenger_buff", "", szBuf, MAX_PATH );
	m_szPassengerBuff = szBuf;

	rkLoader.LoadString_e( "passenger_kick_buff", "", szBuf, MAX_PATH );
	m_szPassengerKickBuff = szBuf;

	int iSize = rkLoader.LoadInt_e( "attach_weapon_type_count", 0 );
	for( int i=0; i<iSize; ++i )
	{
		WeaponInfo kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "attach_weapon_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attach_weapon_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attach_weapon_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attach_weapon_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attach_weapon_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttachWeaponInfoList.push_back( kInfo );
	}
}

ioItem* ioVehicleDriverObjectItem::Clone()
{		
	return new ioVehicleDriverObjectItem( *this );
}

ioObjectItem::ObjectSubType ioVehicleDriverObjectItem::GetObjectSubType() const
{
	return OST_VEHICLE_DRIVER;
}

bool ioVehicleDriverObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioVehicleDriverObjectItem::IsEnableDefense() const
{
	return false;
}

bool ioVehicleDriverObjectItem::IsEnableDash() const
{
	return false;
}

void ioVehicleDriverObjectItem::OnEquiped( ioBaseChar *pOwner )
{	
	ioObjectItem::OnEquiped( pOwner );
	
	//용병이 마커를 가지고 있는 용병일 경우 기존 마커가 삭제됨으로 백업
	pOwner->BackupCurrMarker();

	//논 타겟
	pOwner->SetTargetMarker( ioTargetMarker::MT_HEAL_ITEM, m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );
	pOwner->SetTargetMarkerRange( m_fNoneTargetMaxRange );
	pOwner->SetTargetMarkerYOffset( m_fNoneTargetYOffset );

	//타겟
	pOwner->SetMultipleTargetMarker( ioTargetMarker::MT_HEAL_ITEM, m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "", m_iTargetMax );
	pOwner->SetMultipleTargetMarkerRange( m_fTargetMaxRange );

	Init();
	
	ChangeToDelayState( pOwner->GetCreator(), pOwner );
	SetAttachWeapon( pOwner );
}

void ioVehicleDriverObjectItem::OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	ioObjectItem::OnProcessState( pStage, pOwner );

	switch( m_eVehicleDriverState )
	{
	case VDS_DELAY:
		{
			CheckDelayState( pStage, pOwner );
		}
		break;
	case VDS_RUN:
		{
			CheckRunState( pStage, pOwner );
		}
		break;
	case VDS_JUMP_READY:
		{
			CheckJumpReadyState( pStage, pOwner );			
		}
		break;
	case VDS_JUMPPING:
		{
			CheckJumppingState( pStage, pOwner );
		}
		break;
	case VDS_LADING:
		{
			CheckLandingChangeState( pStage, pOwner );
		}
		break;
	case VDS_DRIFT:
		{
			CheckDriftState( pStage, pOwner );
		}
		break;
	}

	CheckPassenger( pOwner, pStage );
}

void ioVehicleDriverObjectItem::CreateDummy( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vDir		= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos	= pOwner->GetWorldPosition();
	int iCurIndex			= g_DummyCharMgr.GetCurLocalIndex();
	ioPlayStage *pStage		= pOwner->GetCreator();

	if( pStage )
	{
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_szRidingDummyCharName, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
		if( pDummy )
		{
			m_iRidingDummyCharIndex = iCurIndex;
		}
	}
}

void ioVehicleDriverObjectItem::DestroyDummy( ioBaseChar* pOwner )
{	
	if( pOwner && m_iRidingDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iRidingDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iRidingDummyCharIndex = 0;
	}
}

void ioVehicleDriverObjectItem::SetRidingDummyCharIndex( ioBaseChar *pOwner, int iCurIndex )
{
	if( !pOwner )
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iCurIndex );
	if( pDummy )
	{
		m_iRidingDummyCharIndex = iCurIndex;
	}
}

ioVehicleDummy* ioVehicleDriverObjectItem::GetVehicle( ioBaseChar* pOwner )
{
	ioDummyChar* pDummyChar = g_DummyCharMgr.FindDummyChar( pOwner, m_iRidingDummyCharIndex );
	return ToVehicleDummy( pDummyChar );
}

int ioVehicleDriverObjectItem::SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, OUT DWORD& dwStartTime, DWORD dwPreDelay )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAnimation );		
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fTimeRate, 0.0f, dwPreDelay, true );
		dwStartTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

		return iAniID;
	}

	return -1;
}

int ioVehicleDriverObjectItem::SetLoopAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAnimation );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );

		return iAniID;
	}

	return -1;
}

void ioVehicleDriverObjectItem::AttackFire( IN ioBaseChar* pOwner, IN const WeaponInfo& rkAttribute, IN const D3DXVECTOR3& vFireDir, IN const D3DXVECTOR3& vFirePos )
{
	FireTime kFireTime;
	kFireTime.dwStart				= FRAMEGETTIME();
	kFireTime.iAttributeIdx			= rkAttribute.m_iWeaponIdx;
	kFireTime.iResistanceIdx		= rkAttribute.m_iResistanceIdx;
	kFireTime.szWoundedAni			= rkAttribute.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= rkAttribute.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni		= rkAttribute.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx			= pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

	pOwner->ExtendFireExplicit( kFireTime, vFirePos, vFireDir, GetName() );
}

void ioVehicleDriverObjectItem::ChangeToDelayState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	m_eVehicleDriverState	= VDS_DELAY;
	m_dwRideStartEndTime	= FRAMEGETTIME();

	CreateDummy( pOwner );	
	pOwner->SetState( CS_VEHICLE_DRIVER );
		
	SetActionAni( pOwner, m_szRideStartAnimation, m_fRideStartTimeRate, m_dwRideStartEndTime, 0 );
}

void ioVehicleDriverObjectItem::CheckDelayState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	if( m_dwRideStartEndTime < FRAMEGETTIME() )
	{
		ChangeToRunState( pStage, pOwner );
	}
}

void ioVehicleDriverObjectItem::ChangeToRunState( ioPlayStage *pStage, ioBaseChar* pOwner, bool bStateValueReset /* = true */ )
{
	//동기화에 의해서 State가 풀리는 경우가 있어서 Change 함수마다 State를 갱신
	pOwner->SetState( CS_VEHICLE_DRIVER );

	m_eVehicleDriverState	= VDS_RUN;
	m_fTargetYaw			= 0.0f;
	m_CurDirKey				= ioUserKeyInput::DKI_NONE;

	SetLoopAni( pOwner, m_szRideRunLoopAnimation, m_fRideRunLoopTimeRate );

	if( bStateValueReset )
	{
		m_dwRideRunLoopStartTime = FRAMEGETTIME();
		m_dwAimmingStartTime	 = FRAMEGETTIME();
	}

	ioVehicleDirverState* pState = ToVehicleDirverState( pOwner->GetSpecialState() );
	if( pState )
		pState->SetDriverState( VDS_RUN );

}

void ioVehicleDriverObjectItem::CheckRunState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	if( m_dwRideRunLoopStartTime + m_dwRideRunLoopDuration < FRAMEGETTIME() )
	{
		ChangeToEndState( pStage, pOwner );
	}
	else
	{
		CheckJumpKeyInput( pStage, pOwner );
		if( !CheckDefenceInput( pStage, pOwner ) )
		{
			CheckRotateKeyInput( pStage, pOwner );
			ProcessRotate( pOwner, m_dwRideRotateTime, m_fTargetYaw );

			pOwner->SetCurMoveSpeed( m_fRideRunSpeed );

			CheckAimming( pOwner );
			CheckMultipleTarget( pOwner );
			CheckAttackInput( pStage, pOwner );
		}
	}
}

void ioVehicleDriverObjectItem::CheckRotateKeyInput( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != ioUserKeyInput::DKI_NONE )
	{
		if( m_CurDirKey != eNewDirKey )
		{
			m_fTargetYaw	= 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			m_CurDirKey		= eNewDirKey;
			m_bLeftRot		= IsLeftRotate( pOwner, m_CurDirKey );

			//동기화에 의해서 State가 풀리는 경우가 있어서 Change 함수마다 State를 갱신
			pOwner->SetState( CS_VEHICLE_DRIVER );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<byte>( SS_RUN );
				kPacket << pOwner->GetTargetRot();
				kPacket << pOwner->GetWorldPosition();
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << m_fTargetYaw;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		m_bLeftRot = false;
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey	 = ioUserKeyInput::DKI_NONE;
			m_fTargetYaw = 0.0f;

			//동기화에 의해서 State가 풀리는 경우가 있어서 Change 함수마다 State를 갱신
			pOwner->SetState( CS_VEHICLE_DRIVER );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<byte>( SS_RUN );
				kPacket << pOwner->GetTargetRot();
				kPacket << pOwner->GetWorldPosition();
				kPacket << m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << m_fTargetYaw;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioVehicleDriverObjectItem::CheckJumpKeyInput( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( pOwner->IsCharDropZonePos() )
	{
		if( pOwner->IsJumpKey() && !pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ChangeToJumpReadyState( pStage, pOwner );
		}
	}
	else
	{
		if( pOwner->IsJumpKey() && !pOwner->IsFloating() && !pOwner->IsBuffLimitJump() )
		{
			ChangeToJumpReadyState( pStage, pOwner );
		}
	}
}

bool ioVehicleDriverObjectItem::CheckDefenceInput( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsDefenseKey() && !pOwner->IsFloating() )
	{
		if( m_dwDriftAttackStartTime == 0 || m_dwDriftAttackStartTime + m_dwDriftAttackGapTime < dwCurTime )
		{
			m_dwDriftAttackStartTime = dwCurTime;
			ChangeToDriftState( pStage, pOwner );
			return true;
		}
	}

	return false;
}

void ioVehicleDriverObjectItem::CheckAttackInput( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKey() )
	{		
		if( m_dwAimmingFireStartTime == 0 || m_dwAimmingFireStartTime + m_dwAimmingFireGapTime < dwCurTime )
		{
			m_dwAimmingFireStartTime = dwCurTime;

			//타겟이 없으면 NONE 타겟
			if( m_TargetedList.empty() )
			{
				NoneTargetAimmingAttackFire( pOwner );
			}
			else
			{
				AimmingAttackFire( pOwner );
			}
		}
	}
}

void ioVehicleDriverObjectItem::ProcessRotate( IN ioBaseChar *pOwner, IN DWORD dwRotateTime, IN float fTargetYaw )
{
	if( !pOwner )
		return;

	if( dwRotateTime <= 0 )
		return;

	if( fTargetYaw == 0.0f )
		return;

	float fFrameGap		= g_FrameTimer.GetCurFrameGap();
	float fRate			= fFrameGap / dwRotateTime;
	float fRotateAngle	= 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	float fYawD = ioMath::ArrangeHead( fTargetYaw );
	float fYawR = DEGtoRAD( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	float fCurYaw	= RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw		= RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw		= ioMath::ArrangeHead( fCurYaw );
	fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{		
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
	}
	else
	{
		if( m_bLeftRot )
			fRotateAngle *= -1;

		fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
		D3DXVECTOR3 vDir = pOwner->GetMoveDir();

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

		vDir = qtAngle * vDir;
		ioMath::CalcDirectionQuaternion( qtAngle, -vDir );
		pOwner->SetTargetRotToRotate( qtAngle, true, false );
	}
}


void ioVehicleDriverObjectItem::ProcessDriftRotate( IN ioBaseChar *pOwner, IN DWORD dwRotateTime, IN float fTargetYaw )
{
	if( !pOwner )
		return;

	if( dwRotateTime <= 0 )
		return;

	if( fTargetYaw == 0.0f )
		return;

	float fFrameGap		= g_FrameTimer.GetCurFrameGap();
	float fRate			= fFrameGap / dwRotateTime;
	float fRotateAngle	= 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	float fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );

	fCurYaw		= ioMath::ArrangeHead( fCurYaw );
	fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		float fYawR = DEGtoRAD( fTargetYaw );

		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
	}
	else
	{
		if( m_bLeftRot )
			fRotateAngle *= -1;
	
		fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
		D3DXVECTOR3 vDir = pOwner->GetMoveDir();
	
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
	
		vDir = qtAngle * vDir;
		ioMath::CalcDirectionQuaternion( qtAngle, -vDir );	
		pOwner->SetTargetRotToRotate( qtAngle, true, false );
	}
}


void ioVehicleDriverObjectItem::ChangeToJumpReadyState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	//동기화에 의해서 State가 풀리는 경우가 있어서 Change 함수마다 State를 갱신
	pOwner->SetState( CS_VEHICLE_DRIVER );

	m_eVehicleDriverState = VDS_JUMP_READY;

	pOwner->SetJumpPower( m_fRideJumpPower );
	m_dwJumpReadyEndTime	= FRAMEGETTIME();

	SetActionAni( pOwner, m_szRideJumpReadyAnimation, m_fJumpReadyTimeRate, m_dwJumpReadyEndTime, 0 );

	ioVehicleDirverState* pState = ToVehicleDirverState( pOwner->GetSpecialState() );
	if( pState )
		pState->SetDriverState( VDS_JUMP_READY );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_JUMP_READY );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioVehicleDriverObjectItem::CheckJumpReadyState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	pOwner->SetCurMoveSpeed( m_fRideJumpReadySpeed );
	if( m_dwJumpReadyEndTime < FRAMEGETTIME() )
	{
		ChangeToJumppingState( pStage, pOwner );
	}
	else
	{
		CheckRotateKeyInput( pStage, pOwner );		
		ProcessRotate( pOwner, m_dwRideJumpRotateTime, m_fTargetYaw );

		CheckAimming( pOwner );
		CheckMultipleTarget( pOwner );
		CheckAttackInput( pStage, pOwner );
	}
}

void ioVehicleDriverObjectItem::ChangeToJumppingState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	//동기화에 의해서 State가 풀리는 경우가 있어서 Change 함수마다 State를 갱신
	pOwner->SetState( CS_VEHICLE_DRIVER );

	m_eVehicleDriverState		= VDS_JUMPPING;
	m_dwRideJumppingStartTime	= FRAMEGETTIME();

	SetActionAni( pOwner, m_szRideJumppingAnimation, m_fRideJumppingTimeRate, m_dwRideJumppingStartTime, 0 );

	ioVehicleDirverState* pState = ToVehicleDirverState( pOwner->GetSpecialState() );
	if( pState )
		pState->SetDriverState( VDS_JUMPPING );
}

void ioVehicleDriverObjectItem::CheckJumppingState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	pOwner->SetCurMoveSpeed( m_fRideJumppingSpeed );
	if( pOwner->GetWorldPosition().y - pOwner->GetBottomHeight() <= m_fRideChangeToLandHeight )
	{
		ChangeToRandingState( pStage, pOwner );
	}
	else
	{
		CheckRotateKeyInput( pStage, pOwner );	
		ProcessRotate( pOwner, m_dwRideJumpRotateTime, m_fTargetYaw );

		CheckAimming( pOwner );
		CheckMultipleTarget( pOwner );
		CheckAttackInput( pStage, pOwner );
	}
}

void ioVehicleDriverObjectItem::ChangeToRandingState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	//동기화에 의해서 State가 풀리는 경우가 있어서 Change 함수마다 State를 갱신
	pOwner->SetState( CS_VEHICLE_DRIVER );

	m_eVehicleDriverState	= VDS_LADING;
	m_dwRideLandingEndTime	= FRAMEGETTIME();

	SetActionAni( pOwner, m_szRideLandingAnimation, m_fRideLandingTimeRate, m_dwRideLandingEndTime, 0 );
	AttackFire( pOwner, m_LandingWeaponInfo, pOwner->GetMoveDir(), pOwner->GetMidPositionByRate() );

	ioVehicleDirverState* pState = ToVehicleDirverState( pOwner->GetSpecialState() );
	if( pState )
		pState->SetDriverState( VDS_LADING );
}

void ioVehicleDriverObjectItem::CheckLandingChangeState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	pOwner->SetCurMoveSpeed( m_fRideLandingSpeed );
	if( m_dwRideLandingEndTime < FRAMEGETTIME() )
	{
		ChangeToRunState( pStage, pOwner, false );
	}
	else
	{
		CheckRotateKeyInput( pStage, pOwner );
		ProcessRotate( pOwner, m_dwRideJumpRotateTime, m_fTargetYaw );

		CheckAimming( pOwner );
		CheckMultipleTarget( pOwner );
		CheckAttackInput( pStage, pOwner );
	}
}

bool ioVehicleDriverObjectItem::IsLeftRotate( ioBaseChar* pOwner, DirKey eNewDirKey )
{
	float fYawD = 0.0f;
	if( eNewDirKey == ioUserKeyInput::DKI_NONE )
		return false;

	fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );	
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir	= qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vDir		= pOwner->GetMoveDir();
	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
	{
		float fCurYaw, fTargetYaw;
		fCurYaw		= RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
		fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

		fCurYaw		= ioMath::ArrangeHead( fCurYaw );
		fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );

		float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
		if( fRotAngle < 0.0f )
			return true;
	}

	return false;
}

void ioVehicleDriverObjectItem::ChangeToDriftState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	//동기화에 의해서 State가 풀리는 경우가 있어서 Change 함수마다 State를 갱신
	pOwner->SetState( CS_VEHICLE_DRIVER );

	m_eVehicleDriverState	= VDS_DRIFT;
	m_dwRideDriftEndTime	= FRAMEGETTIME() + m_dwDriftDuration;
	
	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3	vDir = qtCurRot * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
	float fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );

	pOwner->SetForcePower( vDir, m_fDriftForce );

	if( m_bLeftRot )
		m_fTargetYaw = fCurYaw - m_fDriftAngle;
	else
		m_fTargetYaw = fCurYaw + m_fDriftAngle;
	
	DWORD dwEndTime = 0;
	if( m_bLeftRot )
	{
		int iAniID = SetActionAni( pOwner, m_DriftLeftAttribute.m_AttackAnimation, m_DriftLeftAttribute.m_fAttackAniRate, dwEndTime, m_DriftLeftAttribute.m_dwPreDelay );
		pOwner->RefreshFireTimeList( iAniID, m_DriftLeftAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_DriftLeftAttribute.m_fAttackAniRate, 0 );
	}
	else
	{		
		int iAniID = SetActionAni( pOwner, m_DriftRightAttribute.m_AttackAnimation, m_DriftRightAttribute.m_fAttackAniRate, dwEndTime, m_DriftRightAttribute.m_dwPreDelay );
		pOwner->RefreshFireTimeList( iAniID, m_DriftRightAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_DriftRightAttribute.m_fAttackAniRate, 0 );
	}

	ioVehicleDirverState* pState = ToVehicleDirverState( pOwner->GetSpecialState() );
	if( pState )
	{
		pState->SetDriverState( VDS_DRIFT );
		pState->SetDriftLeft( m_bLeftRot );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_DRIFT );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_bLeftRot;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioVehicleDriverObjectItem::CheckDriftState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	pOwner->SetCurMoveSpeed( m_fDriftSpeed );

	if( m_dwRideDriftEndTime < FRAMEGETTIME() )
	{
		ChangeToRunState( pStage, pOwner, false );
	}
	else
	{
		ProcessDriftRotate( pOwner, m_fDriftRotateTime, m_fTargetYaw );

		CheckAimming( pOwner );
		CheckMultipleTarget( pOwner );
		CheckAttackInput( pStage, pOwner );
	}
}

void ioVehicleDriverObjectItem::ChangeToEndState( ioPlayStage *pStage, ioBaseChar* pOwner )
{
	m_eVehicleDriverState	= VDS_END;
	m_dwRideEndTime			= FRAMEGETTIME();

	DestroyDummy( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_szRideEndAnimation );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fRideEndTimeRate, 0.0f, 0.0f, true );
		m_dwRideEndTime += pGrp->GetAnimationFullTime(iAniID) * m_fRideEndTimeRate;
	}
}

void ioVehicleDriverObjectItem::CheckAimming( ioBaseChar* pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_AimState )
	{
	case ioTargetMarker::MS_AIMED:
		if( m_dwAimmingStartTime + m_dwAimmingCheckTime < dwCurTime )
		{
			if( 0 < m_dwZeroAimedCheckTime )
			{
				m_AimState				= ioTargetMarker::MS_ZERO_AIMED;
				m_dwZeroAimedStartTime	= dwCurTime;
			}
			else
			{
				m_AimState				= ioTargetMarker::MS_AIMED;
				m_dwAimmingStartTime	= dwCurTime;
			}
		}
		break;
	case ioTargetMarker::MS_ZERO_AIMED:
		if( m_dwZeroAimedStartTime + m_dwZeroAimedCheckTime < dwCurTime )
		{
			m_AimState				= ioTargetMarker::MS_AIMED;
			m_dwAimmingStartTime	= dwCurTime;
		}
		break;
	}
}

void ioVehicleDriverObjectItem::CheckMultipleTarget( ioBaseChar* pOwner )
{
	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	FD_RangeMultipleTarget kFindPred( pOwner, m_fTargetMaxRange, m_fTargetMinRange, m_fTargetMaxAngle, false );
	
	//타겟 리스트 가져오기
	BaseCharList TargetList, NotTargetList;
	kFindPred.GetFindCharList( pStage->GetBaseCharList(), TargetList, NotTargetList );
	
	if( TargetList.empty() )
	{
		pOwner->SetTargetMarkerTarget( "", false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		m_TargetedList.clear();
	}
	else
	{
		pOwner->SetTargetMarkerTarget( "", false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

		//새 타겟팅 추가 및 상태 갱신
		CheckNewMultipleTarget( pOwner, TargetList );
		m_TargetedList = TargetList;
	}

	//타겟팅 해제
	ClearNewMultipleTarget( pOwner, NotTargetList );
}

void ioVehicleDriverObjectItem::CheckNewMultipleTarget( ioBaseChar* pOwner, BaseCharList& TargetList )
{
	for( BaseCharList::iterator iter = TargetList.begin(); iter != TargetList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;
		if( pTarget )
		{
			//타겟팅 갱신
			if( pOwner->IsMultipleTargetMarkerTargeted( pTarget->GetCharName() ) )
			{
				int iMarkerIndex = pOwner->GetMultipleTargetMarkerIndex( pTarget->GetCharName() );
				if( iMarkerIndex != NONE_TARGET && m_AimState != pOwner->GetMultipleMarkerState( iMarkerIndex ) )
				{
					SetTargeted( pOwner, iMarkerIndex );
				}
			}
			//새 타겟팅 추가
			else
			{
				int iMarkerIndex = pOwner->SetMultipleTargetMarkerTarget( pTarget->GetCharName(), false );
				if( iMarkerIndex != NONE_TARGET )
				{
					SetTargeted( pOwner, iMarkerIndex );
				}
			}
		}
	}
}

void ioVehicleDriverObjectItem::SetTargeted( ioBaseChar* pOwner, int iMarkerIndex )
{
	switch( m_AimState )
	{
	case ioTargetMarker::MS_AIMED:
		pOwner->SetMultipleTargetMarkerState( ioTargetMarker::MS_AIMED, iMarkerIndex );
		break;
	case ioTargetMarker::MS_ZERO_AIMED:
		pOwner->SetMultipleTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED, iMarkerIndex );
		break;
	}
}

void ioVehicleDriverObjectItem::ClearNewMultipleTarget( ioBaseChar* pOwner, BaseCharList& TargetList )
{
	for( BaseCharList::iterator iter = TargetList.begin(); iter != TargetList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;
		if( pTarget )
		{
			int iMarkerIndex = pOwner->GetMultipleTargetMarkerIndex( pTarget->GetCharName() );
			if( iMarkerIndex != NONE_TARGET )
			{
				pOwner->SetMultipleTargetMarkerState( ioTargetMarker::MS_READY, iMarkerIndex );
				pOwner->SetMultipleTargetMarkerTarget( "", false );
			}
		}
	}
}

void ioVehicleDriverObjectItem::AimmingAttackFire( ioBaseChar* pOwner )
{	
	if( m_TargetedList.empty() )
		return;

	std::vector<D3DXVECTOR3> vAimedFireList;
	std::vector<D3DXVECTOR3> vZeroAimedFireList;

	for( BaseCharList::iterator iter = m_TargetedList.begin(); iter != m_TargetedList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;
		if( !pTarget )
			continue;

		D3DXVECTOR3 vFireDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vFireDir, &vFireDir );

		int iMarkerIndex = pOwner->GetMultipleTargetMarkerIndex( pTarget->GetCharName() );
		if( iMarkerIndex == NONE_TARGET )
			continue;
		
		switch( pOwner->GetMultipleMarkerState( iMarkerIndex ) )
		{
		case ioTargetMarker::MS_AIMED:
			{
				pOwner->SetMultipleTargetMarkerState( ioTargetMarker::MS_AIMED, iMarkerIndex );
				AttackFire( pOwner, m_AimedTargetFireWeaponInfo, vFireDir, pOwner->GetMidPositionByRate() );
				vAimedFireList.push_back( vFireDir );
			}
			break;
		case ioTargetMarker::MS_ZERO_AIMED:
			{
				pOwner->SetMultipleTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED, iMarkerIndex );
				AttackFire( pOwner, m_ZeroAimedTargetFireWeaponInfo, vFireDir, pOwner->GetMidPositionByRate() );
				vZeroAimedFireList.push_back( vFireDir );
			}
			break;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_AIMMING_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (int)vAimedFireList.size();
		for( int i = 0; i < (int)vAimedFireList.size(); ++i )
		{
			kPacket << vAimedFireList[i];
		}
		kPacket << (int)vZeroAimedFireList.size();
		for( int i = 0; i < (int)vZeroAimedFireList.size(); ++i )
		{
			kPacket << vZeroAimedFireList[i];
		}
		
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioVehicleDriverObjectItem::NoneTargetAimmingAttackFire( ioBaseChar* pOwner )
{	
	D3DXVECTOR3 vFireDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vFireDir, &vFireDir );
	AttackFire( pOwner, m_NoneTargetFireWeaponInfo, vFireDir, pOwner->GetMidPositionByRate() );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_NONE_AIMMING_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioVehicleDriverObjectItem::CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	bool bRet = ioObjectItem::CheckReleaseProcess( pStage, pOwner );
	if( bRet )
		return true;

	if( FRAMEGETTIME() < m_dwRideEndTime )
	{
		return true;
	}

	if( pOwner->GetState() != CS_VEHICLE_DRIVER )
	{
		return true;
	}

	return false;
}

void ioVehicleDriverObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
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

void ioVehicleDriverObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );

	DestroyDummy( pOwner );	

	Init();
	
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );
	pOwner->SetMultipleTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "", 0 );
	pOwner->SetMultipleTargetMarkerRange( 0.0f );

	//백업한 마커를 복구
	pOwner->RestoreBackupMarker();

	int iCnt = m_vAttachWeaponIndexList.size();	
	for( int i=0; i<iCnt; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_vAttachWeaponIndexList[i] );
		ioRotateTargetWeapon *pRotate = ToRotateTargetWeapon( pWeapon );
		if( pRotate )
			continue;

		if( pWeapon )
			pWeapon->SetWeaponDead();
	}
}

void ioVehicleDriverObjectItem::CheckPassenger( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( !pStage || !pOwner || !pOwner->IsNeedProcess() )
		return;

	//탑승 여부는 탑승할 유저가 판단 후, 지정된 인원보다 탑승인원이 많으면 스킬시전자 쪽에서 지정된 인원외의 유저는
	//Kick 버프를 걸어서 추방
	int iCheckCount = 0;
	const BaseCharList& rkList = pStage->GetBaseCharList();
	ioHashStringVec CandidateList;
	for( BaseCharList::const_iterator iter = rkList.begin(); iter != rkList.end(); ++iter )
	{
		const ioBaseChar* pConstTarget = *iter;
		if( !pConstTarget )
			continue;
				
		if( !pConstTarget->HasBuff( BT_PASSENGER_BUFF ) )
			continue;

		CandidateList.push_back( pConstTarget->GetCharName() );
	}
	
	//BaseCharList는 const 형태만 제공함으로 for문을 두번나누어서 탑승유저의 ioBaseChar 포인터에 접근
	ioHashStringVec RemoveCharList;
	for( ioHashStringVec::iterator iter = CandidateList.begin(); iter != CandidateList.end(); ++iter )
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( *iter );
		if( !pTarget )
			continue;

		ioPassengerBuff* pBuff = ToPassengerBuff( pTarget->GetBuff( m_szPassengerBuff ) );
		if( !pBuff )
			continue;

		//탑승했는지 여부 체크
		if( pBuff->GetPassengerState() != ioPassengerBuff::PS_GET_ON )
			continue;

		if( iCheckCount < m_iPassengerMax )
		{
			iCheckCount++;
		}
		else
		{
			pTarget->AddNewBuff( m_szPassengerKickBuff, pOwner->GetCharName(), "", NULL );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
				kPacket << pTarget->GetCharName();
				kPacket << pOwner->GetCharName();
				kPacket << "";
				kPacket << pTarget->GetRandomSeed();
				kPacket << false;					// 타격횟수 체크
				kPacket << 0.0f;
				kPacket << 0.0f;
				kPacket << 0.0f;
				kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
				kPacket << 1;
				kPacket << m_szPassengerKickBuff;
				kPacket << false;					// Use Force
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioVehicleDriverObjectItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	ioObjectItem::OnItemGaugeOK( pOwner, rkPacket );
	
	byte eState;
	rkPacket >> eState;
		
	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;

	rkPacket >> qtRot;
	rkPacket >> vPos;

	pOwner->SetTargetRot( qtRot );
	pOwner->SetWorldPosition( vPos );

	switch( eState )
	{
	case SS_RUN:
		{
			int iCurKey = ioUserKeyInput::DKI_NONE;
			rkPacket >> iCurKey;
			m_CurDirKey = (DirKey)iCurKey;

			rkPacket >> m_bLeftRot;
			rkPacket >> m_fTargetYaw;

			//동기화에 의해서 State가 풀리는 경우가 있어서 Change 함수마다 State를 갱신
			pOwner->SetState( CS_VEHICLE_DRIVER );
		}
		break;
	case SS_JUMP_READY:
		{
			ChangeToJumpReadyState( pOwner->GetCreator(), pOwner );
		}
		break;
	case SS_DRIFT:
		{
			rkPacket >> m_bLeftRot;
			ChangeToDriftState( pOwner->GetCreator(), pOwner );
		}
		break;
	case SS_NONE_AIMMING_ATTACK:
		{
			NoneTargetAimmingAttackFire( pOwner );
		}
		break;
	case SS_AIMMING_ATTACK:
		{
			int iAimedCount = 0;
			rkPacket >> iAimedCount;
			for( int i = 0; i < iAimedCount; ++i )
			{
				D3DXVECTOR3 vFireDir = ioMath::VEC3_ZERO;
				rkPacket >> vFireDir;
				AttackFire( pOwner, m_AimedTargetFireWeaponInfo, vFireDir, pOwner->GetMidPositionByRate() );
			}

			int iZeroAimedCount = 0;
			rkPacket >> iZeroAimedCount;
			for( int i = 0; i < iZeroAimedCount; ++i )
			{
				D3DXVECTOR3 vFireDir = ioMath::VEC3_ZERO;
				rkPacket >> vFireDir;
				AttackFire( pOwner, m_ZeroAimedTargetFireWeaponInfo, vFireDir, pOwner->GetMidPositionByRate() );
			}
		}
		break;
	}
}

void ioVehicleDriverObjectItem::SetAttachWeapon( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_AttachWeaponInfoList.empty() )
		return;

	if( !m_vAttachWeaponIndexList.empty() )
		return;

	m_vAttachWeaponIndexList.clear();

	D3DXVECTOR3 vCurDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	DWORD dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	int iCnt = m_AttachWeaponInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kInfo = m_AttachWeaponInfoList[i];
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx  = kInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = kInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni   = kInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration  = kInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = kInfo.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = dwBaseWeaponIndex + i;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		if( kFireTime.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
		{
			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
			if( pWeapon )
			{
				ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
				if( pZone )
				{
					pZone->SetCollisionDir( vCurDir );
					pZone->SetZoneValue( true, false );
				}

				ioAdhesiveMissileWeapon3 *pAdhesiveWeapon3 = ToAdhesiveMissileWeapon3( pWeapon );
				if( pAdhesiveWeapon3 )
				{
					pAdhesiveWeapon3->CheckOwnerAttach( true );
				}

				ioAttachAeraWeapon *pAttachAreaWeapon = ToAttachAeraWeapon( pWeapon );
				if ( pAttachAreaWeapon )
				{
					pAttachAreaWeapon->CheckOwnerAttach( true );
				}
			}
		}

		m_vAttachWeaponIndexList.push_back( kFireTime.dwWeaponIdx );
	}
}
//-----------------------------------------------------------------------------------------------------------

ioVehicleDirverState::ioVehicleDirverState()
{
	m_eDriverState	= ioVehicleDriverObjectItem::VDS_NONE;
	m_bDriftLeft	= false;
}

ioVehicleDirverState::~ioVehicleDirverState()
{
}

void ioVehicleDirverState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioVehicleDirverState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioVehicleDirverState::CheckSpecialState( ioBaseChar* pOwner )
{
	if( pOwner )
	{
		int iSkillNum = pOwner->GetSkillInput();
		if( iSkillNum >= 0 && !pOwner->IsBuffLimitSkill() )
		{
			if( pOwner->GetChatModeState() == CMS_CHAT )
				pOwner->SetChatModeBox();

			pOwner->SetUseSkill( iSkillNum, SUT_NORMAL );
		}
	}
}

void ioVehicleDirverState::ProcessKeyInput( ioBaseChar* pOwner )
{
}

void ioVehicleDirverState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{

}

bool ioVehicleDirverState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense,TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	//상대편은 밀림과 충돌 처리
	if( eOwnerTeam != eOtherTeam )
		return false;

	return true;
}

bool ioVehicleDirverState::IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const
{
	//자신이 탑승한 Entity에 대한 충돌 및 밀림 처리를 못하도록 한다.
	ioDummyChar* pDummy = ToDummyChar( pColEntity );
	if( pDummy && ToVehicleDummy( pDummy ) )
	{
		return true;
	}

	return false;
}