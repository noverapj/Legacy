#include "StdAfx.h"
#include "ioStrangerItem.h"
#include "ioSimpleBoxWeapon.h"
#include "ioZoneEffectWeapon.h"

ioStrangerItem::ioStrangerItem(void)
{
	ClearData();
	m_iCurCombo = 0;
	m_fCurExtraGauge = 0.0f;
	m_vEffectInfoList.clear();
	m_iCurEffectChargeCount = 0;
	m_dwChargeEffectAttackWeaponIndex = -1;
}

ioStrangerItem::ioStrangerItem( const ioStrangerItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_iMaxEffectChargeCount( rhs.m_iMaxEffectChargeCount ),
m_vEffectOffsetList( rhs.m_vEffectOffsetList ),
m_szEffectName( rhs.m_szEffectName ),
m_szBackGroundEffectName( rhs.m_szBackGroundEffectName ),
m_EffectAttackInfo( rhs.m_EffectAttackInfo ),
m_ChargeAttack( rhs.m_ChargeAttack ),
m_JumpChargeAttack( rhs.m_JumpChargeAttack ),
m_ChargeEffectAttackInfo( rhs.m_ChargeEffectAttackInfo ),
m_vChargeEffectAttackOffset( rhs.m_vChargeEffectAttackOffset ),
m_dwChargeEffectTicTime( rhs.m_dwChargeEffectTicTime ),
m_fEffectChargeEndJumpPower( rhs.m_fEffectChargeEndJumpPower ),
m_fEffectChargeAttackEndJumpPower( rhs.m_fEffectChargeAttackEndJumpPower ),
m_fVertUpLimitAngle( rhs.m_fVertUpLimitAngle ),
m_fVertDownLimitAngle( rhs.m_fVertDownLimitAngle ),
m_dwVertRotateWeight( rhs.m_dwVertRotateWeight ),
m_dwHorzRotateWeight( rhs.m_dwHorzRotateWeight ),
m_fChargeAttackNeedGauge( rhs.m_fChargeAttackNeedGauge ),
m_fEffectDashAttackNeedGauge( rhs.m_fEffectDashAttackNeedGauge ),
m_fDashTeleportNeedGauge( rhs.m_fDashTeleportNeedGauge ),
m_iMaxStrangerTeleportCount( rhs.m_iMaxStrangerTeleportCount ),
m_vStarngerTeleportStartAniList( rhs.m_vStarngerTeleportStartAniList ),
m_vStarngerTeleportAniList( rhs.m_vStarngerTeleportAniList ),
m_szChargeEffectCreateSound( rhs.m_szChargeEffectCreateSound )
{
	ClearData();
	m_iCurCombo = 0;
	m_fCurExtraGauge = 0.0f;
	m_vEffectInfoList.clear();
	m_iCurEffectChargeCount = 0;
	m_dwChargeEffectAttackWeaponIndex = -1;
}

ioStrangerItem::~ioStrangerItem(void)
{
}

ioItem* ioStrangerItem::Clone()
{
	return new ioStrangerItem( *this );
}

ioWeaponItem::WeaponSubType ioStrangerItem::GetSubType() const
{
	return ioWeaponItem::WST_STRANGER_ITEM;
}

void ioStrangerItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

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

	//Charge Effect
	m_iMaxEffectChargeCount = rkLoader.LoadInt_e( "max_effect_attack_count", 0 );
	if ( m_iMaxEffectChargeCount > 0 )
	{
		m_vEffectOffsetList.reserve( m_iMaxEffectChargeCount );
		for ( int i=0; i<m_iMaxEffectChargeCount ; ++i )
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
	}

	rkLoader.LoadString_e( "effect_name", "", szBuf, MAX_PATH );
	m_szEffectName = szBuf;
	rkLoader.LoadString_e( "background_effect_name", "", szBuf, MAX_PATH );
	m_szBackGroundEffectName = szBuf;

	m_dwChargeEffectTicTime = (DWORD)rkLoader.LoadInt_e( "charge_effect_tic_time", 0 );
	LoadWeaponInfo( "effect_attack", m_EffectAttackInfo, rkLoader );
	LoadWeaponInfoList( "charge_effect_attack", m_ChargeEffectAttackInfo, rkLoader );
	m_vChargeEffectAttackOffset.x = rkLoader.LoadFloat_e( "charge_effect_attack_offset_x", 0.0f );
	m_vChargeEffectAttackOffset.y = rkLoader.LoadFloat_e( "charge_effect_attack_offset_y", 0.0f );
	m_vChargeEffectAttackOffset.z = rkLoader.LoadFloat_e( "charge_effect_attack_offset_z", 0.0f );

	m_fEffectChargeEndJumpPower = rkLoader.LoadFloat_e( "effect_charge_end_jump_power", 0.0f );
	m_fEffectChargeAttackEndJumpPower = rkLoader.LoadFloat_e( "effect_charge_attack_end_jump_power", 0.0f );

	m_fVertUpLimitAngle		= rkLoader.LoadFloat_e( "charge_loop_vert_up_limit", 0.0f );
	m_fVertDownLimitAngle	= rkLoader.LoadFloat_e( "charge_loop_vert_down_limit", 0.0f );
	m_dwVertRotateWeight	= rkLoader.LoadInt_e( "charge_loop_rotate_vert_weight", 0 );
	m_dwHorzRotateWeight	= rkLoader.LoadInt_e( "charge_loop_rotate_horz_weight", 0 );

	rkLoader.LoadString_e( "charge_effect_create_sound", "", szBuf, MAX_PATH );
	m_szChargeEffectCreateSound = szBuf;

	//Teleport
	m_iMaxStrangerTeleportCount = rkLoader.LoadInt_e( "max_stranger_teleport_count", 0 );
	if ( m_iMaxStrangerTeleportCount > 0 )
	{
		m_vStarngerTeleportStartAniList.reserve( m_iMaxStrangerTeleportCount );
		m_vStarngerTeleportAniList.reserve( m_iMaxStrangerTeleportCount );
		TeleportStartAni StartAniInfo;
		TeleportAni AniInfo;
		D3DXVECTOR3 vPos;
		for ( int i=0 ; i < m_iMaxStrangerTeleportCount ; ++i )
		{
			StartAniInfo.Init();
			wsprintf_e( szBuf, "stranger_teleport_start%d_ani", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			StartAniInfo.szAni = szBuf;
			wsprintf_e( szBuf, "stranger_teleport_start%d_ani_rate", i+1 );
			StartAniInfo.fAniRate = rkLoader.LoadFloat( szBuf, FLOAT1 );
			m_vStarngerTeleportStartAniList.push_back( StartAniInfo );

			AniInfo.Init();
			wsprintf_e( szBuf, "stranger_teleport%d_ani", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			AniInfo.szAni = szBuf;
			wsprintf_e( szBuf, "stranger_teleport%d_ani_rate", i+1 );
			AniInfo.fAniRate = rkLoader.LoadFloat( szBuf, FLOAT1 );
			wsprintf_e( szBuf, "stranger_teleport%d_end_jump_power", i+1 );
			AniInfo.fEndJumpPower = rkLoader.LoadFloat( szBuf, FLOAT1 );

			wsprintf_e( szBuf, "stranger_teleport%d_offset_x", i+1 );
			AniInfo.vOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );
			wsprintf_e( szBuf, "stranger_teleport%d_offset_y", i+1 );
			AniInfo.vOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );
			wsprintf_e( szBuf, "stranger_teleport%d_offset_z", i+1 );
			AniInfo.vOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );
			m_vStarngerTeleportAniList.push_back( AniInfo );
		}
	}

	//Gauge
	m_fChargeAttackNeedGauge = rkLoader.LoadFloat_e( "charge_attack_need_gague", 0.0f );
	m_fEffectDashAttackNeedGauge = rkLoader.LoadFloat_e( "effect_dash_attack_need_gauge", 0.0f );
	m_fDashTeleportNeedGauge = rkLoader.LoadFloat_e( "dash_teleport_need_gauge", 0.0f );

	LoadChargeAttackAniProperty( rkLoader );
}

void ioStrangerItem::LoadChargeAttackAniProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	//Effect Charge Ani
	rkLoader.LoadString_e( "charge_effect_ani", "", szBuf, MAX_PATH );
	m_ChargeAttack.szChargeEffectAni = szBuf;
	m_ChargeAttack.fChargeEffectAniRate = rkLoader.LoadFloat_e( "charge_effect_ani_rate", FLOAT1 );

	//Charge Attack Ani
	rkLoader.LoadString_e( "charge_effect_attack_start_ani", "", szBuf, MAX_PATH );
	m_ChargeAttack.szChargeEffectAttackStartAni = szBuf;
	m_ChargeAttack.fChargeEffectAttackStartAniRate = rkLoader.LoadFloat_e( "charge_effect_attack_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_effect_attack_center_ani", "", szBuf, MAX_PATH );
	m_ChargeAttack.szChargeEffectAttackCenterAni = szBuf;
	rkLoader.LoadString_e( "charge_effect_attack_up_ani", "", szBuf, MAX_PATH );
	m_ChargeAttack.szChargeEffectAttackUpAni = szBuf;
	rkLoader.LoadString_e( "charge_effect_attack_down_ani", "", szBuf, MAX_PATH );
	m_ChargeAttack.szChargeEffectAttackDownAni = szBuf;



	rkLoader.LoadString_e( "jump_charge_effect_ani", "", szBuf, MAX_PATH );
	m_JumpChargeAttack.szChargeEffectAni = szBuf;
	m_JumpChargeAttack.fChargeEffectAniRate = rkLoader.LoadFloat_e( "charge_effect_ani_rate", FLOAT1 );

	//Charge Attack Ani
	rkLoader.LoadString_e( "jump_charge_effect_attack_start_ani", "", szBuf, MAX_PATH );
	m_JumpChargeAttack.szChargeEffectAttackStartAni = szBuf;
	m_JumpChargeAttack.fChargeEffectAttackStartAniRate = rkLoader.LoadFloat_e( "charge_effect_attack_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_charge_effect_attack_center_ani", "", szBuf, MAX_PATH );
	m_JumpChargeAttack.szChargeEffectAttackCenterAni = szBuf;
	rkLoader.LoadString_e( "jump_charge_effect_attack_up_ani", "", szBuf, MAX_PATH );
	m_JumpChargeAttack.szChargeEffectAttackUpAni = szBuf;
	rkLoader.LoadString_e( "jump_charge_effect_attack_down_ani", "", szBuf, MAX_PATH );
	m_JumpChargeAttack.szChargeEffectAttackDownAni = szBuf;
}

void ioStrangerItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_SpecialState = SS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwNextChargeEffectTime = 0;
	m_vAttackDir = ioMath::VEC3_ZERO;
	m_fCurrMotionRate = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_iCurStrangerTeleportCount = 0;
}

void ioStrangerItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );
	RemoveEffectList( pOwner );
	m_iCurEffectChargeCount = 0;
}

void ioStrangerItem::OnReleasedByEndGame( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	RemoveEffectInfoList( pStage );
	m_iCurEffectChargeCount = 0;
}

int ioStrangerItem::GetCurBullet()
{
	return m_fCurExtraGauge;
}

int ioStrangerItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

void ioStrangerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioStrangerItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_STRANGER )
			fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_STRANGER )
			fTypeRate = m_fExtraGaugeAttackDef;
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	m_fCurExtraGauge += fTypeRate;
	m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
}

int ioStrangerItem::GetNeedBullet()
{
	return (int)m_fChargeAttackNeedGauge;
}

void ioStrangerItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurExtraGauge -= fNeedGauge;
	m_fCurExtraGauge = (float)max( 0, min( m_fCurExtraGauge, m_fMaxExtraGauge ) );
}

void ioStrangerItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( true, false );
		}
	}
}

void ioStrangerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioStrangerItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioStrangerItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetStrangerSpecialState( pOwner, false );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioStrangerItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStrangerItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;

		CheckCancelReserveByConditionalSpecialDash( pOwner );
		//Enable GaugeCheck
		ProcessCancelBySpeicalDash( pOwner );
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

bool ioStrangerItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if ( m_fEffectDashAttackNeedGauge <= m_fCurExtraGauge )
		return true;
	return false;
}

void ioStrangerItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if ( !pOwner )
		return;

	if ( !m_vEffectInfoList.empty() )
		CreateEffectDashAttack( pOwner );
}

void ioStrangerItem::CreateEffectDashAttack( ioBaseChar *pOwner )
{
	if ( m_vEffectInfoList.empty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_EffectAttackInfo.m_iWeaponIdx == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s - effect attack info is no exist", __FUNCTION__ );
		return;
	}

	DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	std::list< DWORD >::iterator iter = m_vEffectInfoList.begin();
	for( ; iter != m_vEffectInfoList.end() ; ++iter )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( *iter );
		if ( pMapEffect )
		{
			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME() + m_EffectAttackInfo.m_dwCallTime;

			kFireTime.iAttributeIdx = m_EffectAttackInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = m_EffectAttackInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni  = m_EffectAttackInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = m_EffectAttackInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = m_EffectAttackInfo.m_bWoundedLoopAni;
			kFireTime.dwWeaponIdx = dwWeaponIndex++;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

			D3DXVECTOR3 vPos = pMapEffect->GetWorldPosition();
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			pMapEffect->EndEffectForce();
		}
	}

	m_vEffectInfoList.clear();

	if ( !m_szBackGroundEffectName.IsEmpty() )
		pOwner->EndEffect( m_szBackGroundEffectName );

	DecreaseGauge( m_fEffectDashAttackNeedGauge );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_EFFECT_DASH_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}



void ioStrangerItem::CallItemProcess( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	if ( m_vEffectInfoList.empty() )
		return;

	if ( (int)m_vEffectInfoList.size() > m_iMaxEffectChargeCount )
	{
		LOG.PrintTimeAndLog( 0, "%s - m_vEffectInfoList.size() > MaxEffectChargeCount", __FUNCTION__ );
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	std::list< DWORD >::iterator iter = m_vEffectInfoList.begin();
	int i=0;
	while( iter != m_vEffectInfoList.end() )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( *iter );
		if ( !pMapEffect || !pMapEffect->IsLive() )
			iter = m_vEffectInfoList.erase( iter );
		else
		{
			D3DXVECTOR3 vPosOffset = vPos + pOwner->GetWorldOrientation() * m_vEffectOffsetList[i];
			pMapEffect->SetWorldPosition( vPosOffset );
			pMapEffect->SetWorldOrientation( qtRot );
			++iter;
		}
		i++;
	}
}

void ioStrangerItem::RemoveEffectList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( !m_szBackGroundEffectName.IsEmpty() )
		pOwner->EndEffect( m_szBackGroundEffectName );

	RemoveEffectInfoList( pStage );
}

void ioStrangerItem::RemoveEffectInfoList( ioPlayStage *pStage )
{
	if ( !pStage )
		return;

	std::list< DWORD >::iterator iter = m_vEffectInfoList.begin();
	for( ; iter != m_vEffectInfoList.end() ; ++iter )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( *iter );
		if ( pMapEffect )
			pMapEffect->EndEffectForce();
	}
	m_vEffectInfoList.clear();
}

void ioStrangerItem::ClearSpecialState( ioBaseChar* pOwner )
{
	RemoveSimpleBoxWeapon( pOwner );
}

bool ioStrangerItem::IsEnableStarngerSpecialState()
{
	if( m_vEffectInfoList.empty() )
		return true;
	else if ( m_fChargeAttackNeedGauge <= m_fCurExtraGauge )
		return true;

	return false;
}

bool ioStrangerItem::IsEnableStrangerDashTeleportState()
{
	if ( m_fDashTeleportNeedGauge <= m_fCurExtraGauge )
		return true;

	return false;
}

void ioStrangerItem::SetStrangerSpecialState( ioBaseChar *pOwner, bool bJump )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_vEffectInfoList.empty() )
		ChangeToStrangerEffectCharge( pOwner, bJump );
	else if ( m_fChargeAttackNeedGauge <= m_fCurExtraGauge )
		ChangeToStrangerEffectAttackStart( pOwner, bJump );
	else if( !bJump )
		ChangeToNormalAttack( pOwner );
}

void ioStrangerItem::ChangeToStrangerEffectCharge( ioBaseChar *pOwner, bool bJump )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	m_iCurEffectChargeCount = 0;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_CurChargeAttack.Init();
	if ( bJump )
		m_CurChargeAttack = m_JumpChargeAttack;
	else
		m_CurChargeAttack = m_ChargeAttack;
	int iAniID	     = pGrp->GetAnimationIdx( m_CurChargeAttack.szChargeEffectAni );

	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ani Name is not exist", __FUNCTION__ );
		return;
	}

	float fTimeRate = m_CurChargeAttack.fChargeEffectAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	if ( pOwner->GetState() != CS_STRANGER_SPECIAL )
		pOwner->SetState( CS_STRANGER_SPECIAL );

	m_SpecialState = SS_EFFECT_CHARGING;

	if ( m_szEffectName.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s - Effect Name is not exist", __FUNCTION__ );
		return;
	}

	if ( m_iMaxEffectChargeCount <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s - Max Effect Charge Count Zero", __FUNCTION__ );
		return;
	}

	if ( m_vEffectOffsetList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "%s - Effect Charge Offset List Is Empty", __FUNCTION__ );
		return;
	}

	if ( m_dwChargeEffectTicTime <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ChargeEffectTicTime is Zero", __FUNCTION__ );
		return;
	}

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

		CreateEffectChargeEffect( pOwner, m_iCurEffectChargeCount );
		m_dwNextChargeEffectTime = FRAMEGETTIME() + m_dwChargeEffectTicTime;
	}

	if ( !m_szBackGroundEffectName.IsEmpty() )
		pOwner->AttachEffect( m_szBackGroundEffectName );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_EFFECT_CHARGE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurEffectChargeCount++;
}

void ioStrangerItem::ChangeToStrangerEffectAttackStart( ioBaseChar *pOwner, bool bJump )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_CurChargeAttack.Init();
	if ( bJump )
		m_CurChargeAttack = m_JumpChargeAttack;
	else
		m_CurChargeAttack = m_ChargeAttack;

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
	}

	int iAniID = pGrp->GetAnimationIdx( m_CurChargeAttack.szChargeEffectAttackStartAni );
	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ani Name is not exist", __FUNCTION__ );
		return;
	}

	float fTimeRate = m_CurChargeAttack.fChargeEffectAttackStartAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_SpecialState = SS_CHARGING_ATTACK_START;

	if ( pOwner->GetState() != CS_STRANGER_SPECIAL )
		pOwner->SetState( CS_STRANGER_SPECIAL );

	RemoveEffectList( pOwner );

	if ( !m_szBackGroundEffectName.IsEmpty() )
		pOwner->EndEffect( m_szBackGroundEffectName );

	DecreaseGauge( m_fChargeAttackNeedGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_EFFECT_ATTACK_START;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStrangerItem::CreateEffectChargeEffect( ioBaseChar *pOwner, int iIndex )
{
	if ( !pOwner )
		return;

	if ( m_szEffectName.IsEmpty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( iIndex >= m_iMaxEffectChargeCount )
		return;

	if ( (int)m_vEffectInfoList.size() >= m_iMaxEffectChargeCount )
		return;

	D3DXVECTOR3 vPosOffset = pOwner->GetWorldOrientation() * m_vEffectOffsetList[iIndex];
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vPosOffset;
	D3DXVECTOR3 vScale = pOwner->GetWorldScale();
	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szEffectName, vPos, vScale );
	if ( pMapEffect )
	{
		m_vEffectInfoList.push_back( pMapEffect->GetUniqueID() );
		if( !m_szChargeEffectCreateSound.IsEmpty() )
			g_SoundMgr.PlaySound( m_szChargeEffectCreateSound );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CREATE_CHARGE_EFFECT;
		kPacket << pOwner->GetWorldPosition();
		kPacket << iIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStrangerItem::ProcessStarngerSpecialState( ioBaseChar *pOwner )
{
	switch( m_SpecialState )
	{
	case SS_EFFECT_CHARGING:
		ProcessEffectCharging( pOwner );
		break;
	case SS_CHARGING_ATTACK_START:
		ProcessEffectChargeAttackStart( pOwner );
		break;
	case SS_CHARGING_ATTACK_LOOP:
		ProcessEffectChargeAttackLoop( pOwner );
		break;
	case SS_TELEPORT_START:
		ProcessTeleportStartState( pOwner );
		break;
	case SS_TELEPORT:
		ProcessTeleportState( pOwner );
		break;;
	}
}

void ioStrangerItem::ProcessEffectCharging( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( !pOwner->IsAttackKeyDown() )
	{
		m_dwNextChargeEffectTime = 0;
		pOwner->SetExtendAttackEndJump( m_fEffectChargeEndJumpPower, FLOAT1 );
		return;
	}

	if ( m_iCurEffectChargeCount >= m_iMaxEffectChargeCount )
	{
		m_dwNextChargeEffectTime = 0;
		pOwner->SetExtendAttackEndJump( m_fEffectChargeEndJumpPower, FLOAT1 );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwNextChargeEffectTime < dwCurTime )
	{
		m_dwNextChargeEffectTime = dwCurTime + m_dwChargeEffectTicTime;
		CreateEffectChargeEffect( pOwner, m_iCurEffectChargeCount++ );
	}
	
}

void ioStrangerItem::ProcessEffectChargeAttackStart( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		ChangeToStrangerEffectAttackLoop( pOwner );
		return;
	}
}

void ioStrangerItem::ChangeToStrangerEffectAttackLoop( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_CurChargeAttack.szChargeEffectAttackUpAni, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_CurChargeAttack.szChargeEffectAttackCenterAni, 0.0f, FLOAT1 );
	pGrp->SetLoopAni( m_CurChargeAttack.szChargeEffectAttackDownAni, 0.0f, 0.0f );

	m_SpecialState = SS_CHARGING_ATTACK_LOOP;

	if ( m_iCurEffectChargeCount > (int)m_ChargeEffectAttackInfo.size() )
	{
		LOG.PrintTimeAndLog( 0, "%s - Cur Effect Info Size > Attack Info ", __FUNCTION__ );
		return;
	}

	const WeaponInfo& rkInfo = m_ChargeEffectAttackInfo[m_iCurEffectChargeCount-1];
	
	DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME() + rkInfo.m_dwCallTime;
	kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = dwWeaponIndex;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
	m_vAttackDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	vPos += pOwner->GetWorldOrientation() * m_vChargeEffectAttackOffset;

	ioWeapon* pWeapon =  pOwner->ExtendFireExplicit( kFireTime, vPos, m_vAttackDir, "" );
	if ( pWeapon )
		m_dwChargeEffectAttackWeaponIndex = pWeapon->GetWeaponIndex();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_EFFECT_ATTACK_LOOP;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurEffectChargeCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStrangerItem::ProcessEffectChargeAttackLoop( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	bool bChange = false;
	if ( pOwner->IsNeedProcess() )
	{
		if ( !pOwner->IsAttackKeyDown() )
		{
			RemoveSimpleBoxWeapon( pOwner );
			pOwner->SetExtendAttackEndJump( m_fEffectChargeAttackEndJumpPower, FLOAT1 );
			return;
		}

		if ( m_fCurExtraGauge <= 0.0f )
		{
			RemoveSimpleBoxWeapon( pOwner );
			pOwner->SetExtendAttackEndJump( m_fEffectChargeAttackEndJumpPower, FLOAT1 );
			return;
		}
		
		if( CheckDirectionKey( pOwner ) )
			bChange = true;

		if( pOwner->IsJumpKeyDown() && ProcessRotateVert( pOwner, m_dwVertRotateWeight, true ) )
			bChange = true;
		else if( pOwner->IsDefenseKeyDown() && ProcessRotateVert( pOwner, m_dwVertRotateWeight, false ) )
			bChange = true;
	}
	
	ProcessRotateHorz( pOwner, m_dwHorzRotateWeight, m_CurDirKey, IsLeftRotate( pOwner, m_CurDirKey ) );
	ProcessRotateAnimation( pOwner );
	ProcessWeapon( pOwner );

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_DIR;
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<byte>( m_CurDirKey );
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	float fGauge = m_fExtraGaugeBaseDec * g_FrameTimer.GetSecPerFrame();
	DecreaseGauge( fGauge );
}

bool ioStrangerItem::CheckDirectionKey( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return false;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey	= eNewDirKey;
		return true;
	}

	return false;
}

void ioStrangerItem::ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, ioUserKeyInput::DirKeyInput eDir, bool bLeft )
{
	if( !pOwner || fRotateTime <= 0 )
		return;

	if( eDir == ioUserKeyInput::DKI_NONE )
		return;

	D3DXVECTOR3 vXZDir = m_vAttackDir;
	vXZDir.y = 0.0f;
	float fXZLength = D3DXVec3Length( &vXZDir );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );	

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw			= ioMath::ArrangeHead( RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) ) );
	fTargetYaw		= ioMath::ArrangeHead( RADtoDEG( ioMath::QuaterToYaw( qtRotate ) ) );	
	float fRotAngle	= ioMath::AngleGap( fCurYaw, fTargetYaw );

	if( fabs( fRotAngle ) < fRotateAngle )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true, true );
		vXZDir = pOwner->GetMoveDir();
		vXZDir = fXZLength * vXZDir;
		vXZDir.y = m_vAttackDir.y;
		D3DXVec3Normalize( &m_vAttackDir, &vXZDir );
	}
	else
	{
		if( bLeft )
			fRotateAngle *= -1;

		fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
		D3DXVECTOR3 vDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

		vDir = qtAngle * vDir;
		ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

		pOwner->SetTargetRotToRotate( qtAngle, true, false );
		vXZDir = pOwner->GetMoveDir();
		vXZDir = fXZLength * vXZDir;
		vXZDir.y = m_vAttackDir.y;
		D3DXVec3Normalize( &m_vAttackDir, &vXZDir );
	}
}

bool ioStrangerItem::IsLeftRotate( ioBaseChar* pOwner, ioUserKeyInput::DirKeyInput eNewDirKey )
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

bool ioStrangerItem::ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, bool bUp )
{
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
	float fRotateAngle = 360.0f * fRate;

	float fAngle = RADtoDEG( acosf( D3DXVec3Dot( &vDir, &m_vAttackDir ) ) );	
	float fLimit = 0.0f;
	if( m_vAttackDir.y > 0.0f )
	{
		fLimit = m_fVertUpLimitAngle;
	}
	else
	{
		fLimit = m_fVertDownLimitAngle;
	}

	D3DXVECTOR3 vAxis;
	D3DXVECTOR3 vYAxis = ioMath::UNIT_Y;	
	D3DXVec3Cross( &vAxis, &m_vAttackDir, &vYAxis );

	if( bUp )
	{
		float fCurrAngle = fRotateAngle;
		if( m_vAttackDir.y < 0.0f )		
			fCurrAngle = -fRotateAngle;		

		if( !IsLimitAngle( fLimit, fAngle + fCurrAngle ) )
		{
			D3DXVec3Normalize( &vAxis, &vAxis );
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fRotateAngle ) );
			m_vAttackDir = qtRot * m_vAttackDir;
			return true;
		}
	}
	else
	{
		float fCurrAngle = fRotateAngle;
		if( m_vAttackDir.y > 0.0f )		
			fCurrAngle = -fRotateAngle;	

		if( !IsLimitAngle( fLimit, fAngle + fCurrAngle ) )
		{
			D3DXVec3Normalize( &vAxis, &-vAxis );
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fRotateAngle ) );
			m_vAttackDir = qtRot * m_vAttackDir;
			return true;
		}
	}

	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
	return false;
}

bool ioStrangerItem::IsLimitAngle( float fLimitAngle, float fCurrAngle )
{
	if( fLimitAngle > 0.0f && fLimitAngle < fCurrAngle )
		return true;

	return false;
}

void ioStrangerItem::ProcessRotateAnimation( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioAnimation *pUp		= pGrp->GetLoopInPlay( m_CurChargeAttack.szChargeEffectAttackUpAni );
	ioAnimation *pCenter	= pGrp->GetLoopInPlay( m_CurChargeAttack.szChargeEffectAttackCenterAni );
	ioAnimation *pDown		= pGrp->GetLoopInPlay( m_CurChargeAttack.szChargeEffectAttackDownAni );

	if( !pUp || !pCenter || !pDown )
		return;

	D3DXVECTOR3 vYAxis = ioMath::UNIT_Y;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );
	float fGapAngle = RADtoDEG( acosf( D3DXVec3Dot( &vDir, &m_vAttackDir ) ) );

	if( m_vAttackDir.y > 0.0f )
	{
		if( m_fVertUpLimitAngle > 0.0f )
			m_fCurrMotionRate = min( FLOAT1,  fGapAngle / m_fVertUpLimitAngle );
		else
			m_fCurrMotionRate = 0.0f;
	}
	else
	{
		if( m_fVertDownLimitAngle > 0.0f )
			m_fCurrMotionRate = min( FLOAT1, fGapAngle / m_fVertDownLimitAngle );
		else
			m_fCurrMotionRate = 0.0f;
	}

	//센터
	if( m_vAttackDir.y == 0.0f || m_fCurrMotionRate == 0.0f )
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	//업
	else if( m_vAttackDir.y > 0.0f )
	{
		pUp->ForceWeight( m_fCurrMotionRate );
		pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	//다운
	else
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
		pDown->ForceWeight( m_fCurrMotionRate );
	}
}

void ioStrangerItem::ProcessWeapon( ioBaseChar *pOwner )
{
	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwChargeEffectAttackWeaponIndex ) );
	if( pSimple )
	{
		D3DXVECTOR3 vSide;
		D3DXVec3Cross( &vSide, &m_vAttackDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vSide, &vSide );
		pSimple->SetMoveDir( m_vAttackDir );
	}
}

void ioStrangerItem::RemoveSimpleBoxWeapon( ioBaseChar *pOwner )
{
	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwChargeEffectAttackWeaponIndex ) );
	if( pSimple )
		pSimple->SetWeaponDead();
	m_dwChargeEffectAttackWeaponIndex = -1;
}

void ioStrangerItem::SetStrangerTeleportState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_iMaxStrangerTeleportCount <= 0 )
		return;

	m_iCurStrangerTeleportCount = 0;

	ChangeToTeleportStartState( pOwner );

	DecreaseGauge( m_fDashTeleportNeedGauge );
}

void ioStrangerItem::ChangeToTeleportStartState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( (int)m_vStarngerTeleportStartAniList.size() <= m_iCurStrangerTeleportCount )
	{
		LOG.PrintTimeAndLog( 0, "%s -CurTeleportCount bigger than TeleportStartAniList ", __FUNCTION__ );
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const TeleportStartAni& rkInfo = m_vStarngerTeleportStartAniList[m_iCurStrangerTeleportCount];
	int iAniID = pGrp->GetAnimationIdx( rkInfo.szAni );
	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - Ani name is not exist", __FUNCTION__ );
		return;
	}

	if ( pOwner->GetState() != CS_STRANGER_SPECIAL )
		pOwner->SetState( CS_STRANGER_SPECIAL );

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
	}

	float fTimeRate = rkInfo.fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_SpecialState = SS_TELEPORT_START;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_TELEPORT_START;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurStrangerTeleportCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStrangerItem::ProcessTeleportStartState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		m_dwMotionEndTime = 0;
		ChangeToTeleportState( pOwner );
		return;
	}
}

void ioStrangerItem::ChangeToTeleportState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( (int)m_vStarngerTeleportAniList.size() <= m_iCurStrangerTeleportCount )
	{
		LOG.PrintTimeAndLog( 0, "%s -CurTeleportCount bigger than TeleportAniList ", __FUNCTION__ );
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const TeleportAni& rkInfo = m_vStarngerTeleportAniList[m_iCurStrangerTeleportCount];
	int iAniID = pGrp->GetAnimationIdx( rkInfo.szAni );
	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - Ani name is not exist", __FUNCTION__ );
		return;
	}

	float fTimeRate = rkInfo.fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	m_SpecialState = SS_TELEPORT;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_TELEPORT;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurStrangerTeleportCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	D3DXVECTOR3 vPosOffset = pOwner->GetTargetRot() * rkInfo.vOffset;
	vPosOffset = pOwner->GetWorldPosition() + vPosOffset;
	pOwner->SetWorldPosition( vPosOffset );
}

void ioStrangerItem::ProcessTeleportState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		m_dwMotionEndTime = 0;
		pOwner->SetExtendAttackEndJump( m_vStarngerTeleportAniList[m_iCurStrangerTeleportCount].fEndJumpPower, FLOAT1 );
		return;
	}
	else if ( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
	{
		if ( pOwner->IsAttackKey() )
		{
			++m_iCurStrangerTeleportCount;
			ChangeToTeleportStartState( pOwner );
			return;
		}
	}
}

void ioStrangerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	if ( iState == ST_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == ST_EFFECT_CHARGE )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		bool bJump;
		rkPacket >> bJump;
		ChangeToStrangerEffectCharge( pOwner, bJump );
	}
	else if ( iState == ST_CREATE_CHARGE_EFFECT )
	{
		int iIndex;
		rkPacket >> iIndex;
		CreateEffectChargeEffect( pOwner, iIndex );
	}

	else if ( iState == ST_EFFECT_ATTACK_START )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		bool bJump;
		rkPacket >> bJump;
		ChangeToStrangerEffectAttackStart( pOwner, bJump );
	}
	else if ( iState == ST_EFFECT_ATTACK_LOOP )
	{
		rkPacket >> m_iCurEffectChargeCount;
		ChangeToStrangerEffectAttackLoop( pOwner );
	}
	else if ( iState == ST_TELEPORT_START )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		rkPacket >> m_iCurStrangerTeleportCount;
		ChangeToTeleportStartState( pOwner );
	}
	else if ( iState == ST_TELEPORT )
	{
		rkPacket >> m_iCurStrangerTeleportCount;
		ChangeToTeleportState( pOwner );
	}
}

void ioStrangerItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	if ( iState == ST_DIR )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		if( pOwner )
			pOwner->SetTargetRotToRotate( qtRot, true );

		byte eDir;
		rkPacket >> eDir;
		m_CurDirKey = (ioUserKeyInput::DirKeyInput)eDir;

		rkPacket >> m_vAttackDir;
	}
	else if ( iState == ST_EFFECT_DASH_ATTACK )
		CreateEffectDashAttack( pOwner );
}

void ioStrangerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		{
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		return;
	}
}

//-----------------------------------------------------------------------------------------------------------
ioStrangerSpecialState::ioStrangerSpecialState()
{
	
}

ioStrangerSpecialState::~ioStrangerSpecialState()
{

}

void ioStrangerSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
	ioStrangerItem *pStrangerItem = ToStrangerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pStrangerItem )
		pStrangerItem->ClearSpecialState( pOwner );
	
}

void ioStrangerSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	fCurGravity = 0.0f;
}

void ioStrangerSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioStrangerItem *pStrangerItem = ToStrangerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pStrangerItem )
		pStrangerItem->ProcessStarngerSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioStrangerSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}
