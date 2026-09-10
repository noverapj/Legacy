#include "StdAfx.h"
#include "ioUrielItem.h"
#include "ioZoneEffectWeapon.h"


ioUrielItem::ioUrielItem(void)
{
	ClearData();
	m_fCurBullet = 0.0f;
	m_iCurCombo = 0;
	m_SpecialState = SS_NONE;
	m_WoundedState = WS_DELAY;
}

ioUrielItem::ioUrielItem( const ioUrielItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
//Charge
m_ChargeAttack( rhs.m_ChargeAttack ),
m_DashChargeAttack( rhs.m_DashChargeAttack ),
//Wounded 특별
m_WoundedAttackInfoList( rhs.m_WoundedAttackInfoList ),
m_dwWoundedDelayTime( rhs.m_dwWoundedDelayTime ),
m_vWoundedAttackOwnerBuffList( rhs.m_vWoundedAttackOwnerBuffList ),
//Gauge
m_fMaxBullet( rhs.m_fMaxBullet ),
m_fChargeAttackNeedBullet( rhs.m_fChargeAttackNeedBullet ),
m_fWoundedAttackNeedBullet( rhs.m_fWoundedAttackNeedBullet ),
m_fIncreseGaugeTic( rhs.m_fIncreseGaugeTic )
{
	ClearData();
	m_fCurBullet = 0.0f;
	m_iCurCombo = 0;
	m_SpecialState = SS_NONE;
	m_WoundedState = WS_DELAY;
}

ioUrielItem::~ioUrielItem(void)
{
}

ioWeaponItem::WeaponSubType ioUrielItem::GetSubType() const
{
	return WST_URIEL_ITEM;
}

ioItem* ioUrielItem::Clone()
{
	return new ioUrielItem( *this );
}

void ioUrielItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	//Attack Ready
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(szBuf) );
		}
	}

	//D~
	rkLoader.LoadString( "charge_attack_ani", "", szBuf, MAX_PATH );
	m_ChargeAttack.m_szChargeAttackAni = szBuf;
	m_ChargeAttack.m_fChargeAttackAniRate = rkLoader.LoadFloat_e( "charge_attack_ani_rate", 1.0f );
	m_ChargeAttack.m_fChargeAttackAngle = rkLoader.LoadFloat_e( "charge_attack_angle", 0.0f );
	m_ChargeAttack.m_fMaxChargeAttackRange = rkLoader.LoadFloat_e( "charge_attack_max_range", 0.0f );
	m_ChargeAttack.m_iChargeWeaponType = rkLoader.LoadInt_e( "charge_attack_weapon_type", 0 );
	m_ChargeAttack.m_fChargeAttackEndPower = rkLoader.LoadFloat_e( "charge_attack_end_jump_power", 0.0f );
	m_ChargeAttack.m_fChargeAttackSpeed = rkLoader.LoadFloat_e( "charge_attack_speed", 0.0f );
	m_ChargeAttack.m_dwEndDelay= rkLoader.LoadInt_e( "charge_attack_end_delay", 0 );
	LoadAttackAttribute_e( "charge_add_attack", m_ChargeAttack.m_AddAttack, rkLoader );
	
	//Dash D~
	rkLoader.LoadString( "dash_charge_attack_ani", "", szBuf, MAX_PATH );
	m_DashChargeAttack.m_szChargeAttackAni = szBuf;
	m_DashChargeAttack.m_fChargeAttackAniRate = rkLoader.LoadFloat_e( "dash_charge_attack_ani_rate", 1.0f );
	m_DashChargeAttack.m_fChargeAttackAngle = rkLoader.LoadFloat_e( "dash_charge_attack_angle", 0.0f );
	m_DashChargeAttack.m_fMaxChargeAttackRange = rkLoader.LoadFloat_e( "dash_charge_attack_max_range", 0.0f );
	m_DashChargeAttack.m_iChargeWeaponType = rkLoader.LoadInt_e( "dash_charge_attack_weapon_type", 0 );
	m_DashChargeAttack.m_fChargeAttackEndPower = rkLoader.LoadFloat_e( "dash_charge_attack_end_jump_power", 0.0f );
	m_DashChargeAttack.m_fChargeAttackSpeed = rkLoader.LoadFloat_e( "dash_charge_attack_speed", 0.0f );
	m_DashChargeAttack.m_dwEndDelay= rkLoader.LoadInt_e( "dash_charge_attack_end_delay", 0 );
	LoadAttackAttribute_e( "dash_charge_add_attack", m_DashChargeAttack.m_AddAttack, rkLoader );

	//Gauge
	m_fMaxBullet = rkLoader.LoadFloat_e( "max_gauge", 0 );
	m_fIncreseGaugeTic = rkLoader.LoadFloat_e( "increase_gauge_tic", 0.0f );
	m_fChargeAttackNeedBullet = rkLoader.LoadFloat_e( "need_charging_attack_gauge", 0.0f );
	m_fWoundedAttackNeedBullet = rkLoader.LoadFloat_e( "need_wounded_attack_gauge", 0.0f );

	//Wounded Attack
	m_WoundedAttackInfoList.clear();
	int iCnt = rkLoader.LoadInt( "wounded_extend_attack_max", 0 );
	for( int i = 0; i < iCnt ; ++i )
	{
		WoundedAttackInfo Info;

		wsprintf_e( szBuf, "wounded_extend_attack%d_min_height", i+1 );
		Info.fMinHeight	= rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "wounded_extend_attack%d_max_height", i+1 );
		Info.fMaxHeight	= rkLoader.LoadFloat( szBuf, 5.0f );

		wsprintf_e( szBuf, "wounded_extend_attack%d", i+1 );
		LoadAttackAttribute( szBuf, Info.Attack, rkLoader );

		m_WoundedAttackInfoList.push_back( Info );
	}
	m_dwWoundedDelayTime = rkLoader.LoadInt_e( "wounded_attack_delay_time", 0 );
	int iMaxBuffCount = rkLoader.LoadInt_e( "max_wounded_attack_owner_buff_count", 0 );
	m_vWoundedAttackOwnerBuffList.clear();
	if ( iMaxBuffCount > 0 )
	{
		m_vWoundedAttackOwnerBuffList.reserve( iMaxBuffCount );
		for( int i=0; i < iMaxBuffCount; ++i )
		{
			wsprintf_e( szBuf, "wounded_attack_owner_buff%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_vWoundedAttackOwnerBuffList.push_back( ioHashString(szBuf) );
		}
	}

}

void ioUrielItem::ClearData()
{
	m_ChargeState = CS_NONE;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwKeyReserveTime = 0;
	m_dwAttackFireTime = 0;

	m_vChargeAttackMoveDir = ioMath::VEC3_ZERO;
}

void ioUrielItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();
	ClearData();

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
			pOwner->StopAutoTargetTracking();
	}
}

void ioUrielItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	}
}

void ioUrielItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && CheckChargeAttackGauge() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetChargeAttack( pOwner, false );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioUrielItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
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
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_NORMAL_ATTACK;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

void ioUrielItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioUrielItem::SetChargeAttack( ioBaseChar *pOwner, bool bDash )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	if ( !bDash )
		m_CurChargeAttack = m_ChargeAttack;
	else
		m_CurChargeAttack = m_DashChargeAttack;

	ClearChargeWeapon( pOwner );
	m_szTargetName.Clear();
	m_dwChargeWeaponIndex = 0;
	m_fCurChargeAttackRange = 0.f;

	int iAniID = pGrp->GetAnimationIdx( m_CurChargeAttack.m_szChargeAttackAni );
	if ( iAniID == -1 )
		return;
	
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetState( CS_URIEL_STATE );

	//Clear Weapon
	ClearChargeWeapon( pOwner );

	m_SpecialState = SS_ATTACK;

	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eCurrDir = pOwner->CheckCurDirKey();
		if( eCurrDir != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eCurrDir );

		m_vChargeAttackMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &m_vChargeAttackMoveDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( m_CurChargeAttack.m_fChargeAttackAngle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		m_vChargeAttackMoveDir = qtNewRot * m_vChargeAttackMoveDir;
		D3DXVec3Normalize( &m_vChargeAttackMoveDir, &m_vChargeAttackMoveDir );
	}

	float fRate = 1.0f;
	if ( m_CurChargeAttack.m_fChargeAttackAniRate > 0.0f )
		fRate = m_CurChargeAttack.m_fChargeAttackAniRate;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );
	pOwner->CheckCharColSkipTime( iAniID, fRate, 0 );
	pOwner->IncreaseWeaponIndexBase();
	m_dwChargeWeaponIndex = pOwner->GetWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart				= FRAMEGETTIME();
	kFireTime.iAttributeIdx			= m_CurChargeAttack.m_iChargeWeaponType;
	kFireTime.iResistanceIdx		= 0;
	kFireTime.szWoundedAni			= "";
	kFireTime.dwWoundedAniDuration	= 0;
	kFireTime.bLoopWoundedAni		= false;
	kFireTime.dwWeaponIdx			= m_dwChargeWeaponIndex;
	kFireTime.eFireTimeType			= FTT_EXTEND_ATTACK;

	pOwner->ExtendFireExplicit( kFireTime,  pOwner->GetMidPositionByRate(), m_vChargeAttackMoveDir, "" );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fRate;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << bDash;
		kPacket << m_vChargeAttackMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	DecreaseChargeAttackGauge();
}

bool ioUrielItem::CheckChargeWeaponLive( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;
	ioZoneEffectWeapon* pZoneWeapon = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwChargeWeaponIndex ) );
	if( pZoneWeapon && pZoneWeapon->IsLive() )
		return true;

	return false;
}

void ioUrielItem::ClearChargeWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwChargeWeaponIndex ) );
	if( pZone )
		pZone->SetWeaponDead();
}

void ioUrielItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	switch( m_SpecialState )
	{
	case SS_ATTACK:
		ProcessSpecialAttack( pOwner );
		break;
	case SS_ATTACK_END_DELAY:
		ProcessChargeAttackEndDelay( pOwner );
		break;
	case SS_ADD_ATTACK:
		ProcessSpecialAddAttack( pOwner );
		break;
	case SS_WOUNDED_ATTACK:
		ProcessWoundedAttackState( pOwner );
		break;
	}
}

void ioUrielItem::ProcessSpecialAttack( ioBaseChar *pOwner )
{
	if ( !pOwner)
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if ( !CheckChargeWeaponLive( pOwner ) )
		{
			ChangeToChargeAttackEndDelayState( pOwner );
			return;
		}

		if ( IsEnableAddAttack( pOwner ) )
		{
			ChangeToAddAttack( pOwner );
			return;
		}

		if ( m_fCurChargeAttackRange > m_CurChargeAttack.m_fMaxChargeAttackRange )
		{
			ChangeToChargeAttackEndDelayState( pOwner );
			return;
		}
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fDistance = m_CurChargeAttack.m_fChargeAttackSpeed * fTimePerSec;
	D3DXVECTOR3 vMoveAmt = ioMath::VEC3_ZERO;
	vMoveAmt = m_vChargeAttackMoveDir * fDistance;
	m_fCurChargeAttackRange += fDistance;

	if( vMoveAmt == ioMath::VEC3_ZERO )
		return;

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
}

void ioUrielItem::ChangeToChargeAttackEndDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SpecialState = SS_ATTACK_END_DELAY;
	ClearChargeWeapon( pOwner );
	m_dwChargeAttackEndDelayTime = FRAMEGETTIME() + m_CurChargeAttack.m_dwEndDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK_END_DELAY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

void ioUrielItem::ProcessChargeAttackEndDelay( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( IsEnableAddAttack( pOwner ) )
	{
		ChangeToAddAttack( pOwner );
		return;
	}

	if ( m_dwChargeAttackEndDelayTime && m_dwChargeAttackEndDelayTime < FRAMEGETTIME() )
	{
		m_dwChargeAttackEndDelayTime = 0;
		pOwner->SetExtendAttackEndJump( m_CurChargeAttack.m_fChargeAttackEndPower, 1.0f, false, true, true, false );
		return;
	}
}

void ioUrielItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if ( pWoundChar->IsNeedProcess() && fDamage > 0.0f )
	{
		if ( !m_szTargetName.IsEmpty() )
			return;

		m_szTargetName = pWoundChar->GetCharName();

		if( pWoundChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << ST_SET_TARGET;
			kPacket << m_szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioUrielItem::IsEnableAddAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( m_dwKeyReserveTime <= 0 || m_dwKeyReserveTime > FRAMEGETTIME() )
		return false;

	if ( !pOwner->IsAttackKey() )
		return false;

	if ( m_szTargetName.IsEmpty() )
		return false;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( !pTarget )
	{
		m_szTargetName.Clear();
		return false;
	}

	return true;
}

void ioUrielItem::ChangeToAddAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurChargeAttack.m_AddAttack.m_AttackAnimation.IsEmpty() )
		return;

	ClearChargeWeapon( pOwner );

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	ClearKeyReserve();

	m_SpecialState = SS_ADD_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_CurChargeAttack.m_AddAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_CurChargeAttack.m_AddAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_CurChargeAttack.m_AddAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CurChargeAttack.m_AddAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	
	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_CurChargeAttack.m_AddAttack.m_dwEndDelay;

	m_dwAttackFireTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	SetCurAttackAniForInputJump( m_CurChargeAttack.m_AddAttack.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );


	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ADD_ATTACK;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUrielItem::ProcessSpecialAddAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwAttackFireTime && m_dwAttackFireTime < FRAMEGETTIME() )
	{
		m_dwAttackFireTime = 0;
		AddAttackFire( pOwner );
	}

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		ClearChargeWeapon( pOwner );
		pOwner->SetExtendAttackEndJump( m_CurChargeAttack.m_fChargeAttackEndPower, 1.0f, false, true, true, false );
	}
}

void ioUrielItem::AddAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_CurChargeAttack.m_AddAttack.m_vWeaponInfoList.empty() )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( !pTarget )
		return;

	D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetMidPositionByRate() - vPos;
	vDir.y = 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );

	DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_CurChargeAttack.m_AddAttack.m_vWeaponInfoList.front().m_iWeaponIdx;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = dwWeaponIndex;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ADD_ATTACK_FIRE;
		kPacket << vPos;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

bool ioUrielItem::CheckChargeAttackGauge() const
{
	if ( m_fCurBullet >= m_fChargeAttackNeedBullet )
		return true;

	return false;
}

void ioUrielItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		if( m_fCurBullet < m_fMaxBullet )
		{
			float fGauge = m_fIncreseGaugeTic * fTimePerSec;

			m_fCurBullet += fGauge;
			m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
		}
		return;
	}
}

void ioUrielItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxBullet ) );

	m_fCurBullet = fCurBullet;
}

int ioUrielItem::GetNeedBullet()
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioBaseChar *pOwner = NULL;
	if( pStage )
		pOwner = pStage->GetBaseChar( m_OwnerName );

	if( !pOwner )
		return 0;

	return m_fChargeAttackNeedBullet;
}

void ioUrielItem::DecreaseChargeAttackGauge()
{
	m_fCurBullet -= m_fChargeAttackNeedBullet;
	m_fCurBullet = max( m_fCurBullet, 0 );
}

void ioUrielItem::DecreaseWoundedAttackGauge()
{
	//m_fCurBullet -= m_fWoundedAttackNeedBullet;
	//m_fCurBullet = max( m_fCurBullet, 0 );
	m_fCurBullet = 0;
}

bool ioUrielItem::CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( pOwner->IsPrisonerMode() )
		return false;

	if( pOwner->IsCatchMode() )
		return false;

	if( pOwner->GetState() != CS_WOUNDED && pOwner->GetState() != CS_BLOW_WOUNDED && pOwner->GetState() != CS_BOUND_BLOW_WOUNDED && pOwner->GetState() != CS_BLOW_EXTEND_WOUND )
		return false;

	if( !CheckWoundedAttackGauge() )
		return false;

	switch( m_WoundedState )
	{
	case WS_DELAY:
		{
			CheckWoundExtendUseDelay( pOwner );
			return false;
		}
		break;
	case WS_TIME_CONDITION_CHECK:
		{
			if( m_dwWoundedStartTime + m_dwWoundedDelayTime < FRAMEGETTIME() )
			{
				if ( !pOwner )
					return false;

				int iSKillKey = pOwner->GetSkillKeyInput();
				if ( iSKillKey != 0 )
					return false;

				float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
				int iSize = m_WoundedAttackInfoList.size();
				for( int i=0 ; i<iSize ; ++i )
				{
					const WoundedAttackInfo& Info = m_WoundedAttackInfoList[i];
					if( COMPARE( fHeightGap, Info.fMinHeight, Info.fMaxHeight ) )
					{
						BlowWoundedState eBlowState = pOwner->GetBlowWoundedState();				
						if( eBlowState == BW_BLOW_ENDED )
							return false;
						else if ( ChangeToWoundedAttacFire( pOwner, Info.Attack ) )
						{
							if( pOwner->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
								FillItemGaugePacketHeader( pOwner, kPacket );
								kPacket << ST_WOUNDED_ATTACK;
								kPacket << i;
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}
							return true;
						}
					}
				}
			}
		}
		break;
	}

	return false;
}

//피격 후 상태 초기화 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ioUrielItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->GetState() != CS_WOUNDED && pOwner->GetState() != CS_BLOW_WOUNDED && pOwner->GetState() != CS_BOUND_BLOW_WOUNDED && pOwner->GetState() != CS_BLOW_EXTEND_WOUND )
	{
		m_WoundedState	= WS_DELAY;
	}
}

bool ioUrielItem::CheckWoundedAttackGauge() const
{
	if ( m_fCurBullet >= m_fWoundedAttackNeedBullet )
		return true;

	return false;
}

void ioUrielItem::CheckWoundExtendUseDelay( ioBaseChar* pOwner )
{
	m_dwWoundedStartTime		= FRAMEGETTIME();
	m_WoundedState				= WS_TIME_CONDITION_CHECK;
}

bool ioUrielItem::ChangeToWoundedAttacFire( ioBaseChar *pOwner, const AttackAttribute& rkAttack )
{
	if( !pOwner )
		return false;

	if ( rkAttack.m_AttackAnimation.IsEmpty() )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetState( CS_URIEL_STATE );
	ClearChargeWeapon( pOwner );
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	ClearKeyReserve();

	m_SpecialState = SS_WOUNDED_ATTACK;
	
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( rkAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return false;

	float fTimeRate = rkAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = rkAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( rkAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		rkAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += rkAttack.m_dwEndDelay;

	m_dwAttackFireTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	SetCurAttackAniForInputJump( rkAttack.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	DecreaseWoundedAttackGauge();
	AddWoundedAttackOwnerBuff( pOwner );

	return true;
}

void ioUrielItem::AddWoundedAttackOwnerBuff( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	int iCnt = m_vWoundedAttackOwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
		pOwner->AddNewBuff( m_vWoundedAttackOwnerBuffList[i], pOwner->GetCharName(), GetName(), NULL );
}

void ioUrielItem::ProcessWoundedAttackState( ioBaseChar* pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap > 5.0f )
			pOwner->SetFallState( true );
		else
			pOwner->SetState(CS_DELAY);
	}

}

bool ioUrielItem::IsProcessMove( ioBaseChar *pOwner )
{
	if ( m_SpecialState == SS_WOUNDED_ATTACK )
		return false;
	return true;
}

bool ioUrielItem::IsCollisionAvailableWeapon( ioBaseChar *pOwner )
{
	if ( m_SpecialState == SS_WOUNDED_ATTACK )
		return false;
	return true;
}

void ioUrielItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	if ( iState == ST_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );

		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == ST_CHARGE_ATTACK )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotToRotate( qtRot, true );

		bool bDash;
		rkPacket >> bDash;
		rkPacket >> m_vChargeAttackMoveDir;
		SetChargeAttack( pOwner, bDash );
	}
	else if ( iState == ST_CHARGE_ATTACK_END_DELAY )
		ChangeToChargeAttackEndDelayState( pOwner );
	else if ( iState == ST_CHARGE_ADD_ATTACK )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotToRotate( qtRot, true );
		ChangeToAddAttack( pOwner );
	}
	else if ( iState == ST_SET_TARGET )
		rkPacket >> m_szTargetName;
	else if ( iState == ST_CHARGE_ADD_ATTACK_FIRE )
	{
		if ( m_CurChargeAttack.m_AddAttack.m_vWeaponInfoList.empty() )
			return;

		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
		if ( !pTarget )
			return;

		D3DXVECTOR3 vPos,vDir;
		rkPacket >> vPos;
		rkPacket >> vDir;
		DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.iAttributeIdx = m_CurChargeAttack.m_AddAttack.m_vWeaponInfoList.front().m_iWeaponIdx;
		kFireTime.iResistanceIdx = 0;
		kFireTime.szWoundedAni  = "";
		kFireTime.dwWoundedAniDuration = 0;
		kFireTime.bLoopWoundedAni = false;
		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}
	else if ( iState == ST_WOUNDED_ATTACK )
	{
		int iIndex;
		rkPacket >> iIndex;
		if ( iIndex >= (int)m_WoundedAttackInfoList.size() )
			return;

		ChangeToWoundedAttacFire( pOwner, m_WoundedAttackInfoList[iIndex].Attack );
	}
}

void ioUrielItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{

}



//-----------------------------------------------------------------------------------------------------------

ioUrielSpecialState::ioUrielSpecialState()
{

}

ioUrielSpecialState::~ioUrielSpecialState()
{

}

void ioUrielSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioUrielSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioUrielItem *pUrielItem = ToUrielItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pUrielItem )
	{
		pUrielItem->ProcessSpecialState( pOwner );
	}
}

bool ioUrielSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioUrielItem *pUrielItem = ToUrielItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pUrielItem )
		return pUrielItem->IsProcessMove( pOwner );

	return false;
}

void ioUrielSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioUrielSpecialState::IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon )
{
	ioUrielItem *pUrielItem = ToUrielItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pUrielItem )
		return pUrielItem->IsCollisionAvailableWeapon( pOwner );

	return true;
}
