

#include "stdafx.h"

#include "ioResearcher.h"
#include "WeaponDefine.h"

ioResearcher::ioResearcher()
{
	Init();
}

ioResearcher::ioResearcher( const ioResearcher &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_PreGatherAnimation( rhs.m_PreGatherAnimation ),
	m_UpGatherAnimation( rhs.m_UpGatherAnimation ),
	m_CenterGatherAnimation( rhs.m_CenterGatherAnimation ),
	m_DownGatherAnimation( rhs.m_DownGatherAnimation ),
	m_UpExtendAttackAnimation( rhs.m_UpExtendAttackAnimation ),
	m_DownExtendAttackAnimation( rhs.m_DownExtendAttackAnimation ),
	m_GatherMaxEffect( rhs.m_GatherMaxEffect ),
	m_dwHorzRotateWeight( rhs.m_dwHorzRotateWeight ),
	m_dwVertRotateWeight( rhs.m_dwVertRotateWeight ),
	m_fVertUpLimitAngle( rhs.m_fVertUpLimitAngle ),
	m_fVertDownLimitAngle( rhs.m_fVertDownLimitAngle ),
	m_ChargingAttackAttribute( rhs.m_ChargingAttackAttribute ),
	m_ExtendAttackInfoList( rhs.m_ExtendAttackInfoList ),
	m_fOverHeatChekHeight( rhs.m_fOverHeatChekHeight ),
	m_OverHeatAttackAttribute( rhs.m_OverHeatAttackAttribute ),
	m_OverHeatMaleMeshList( rhs.m_OverHeatMaleMeshList ),
	m_OverHeatFemaleMeshList( rhs.m_OverHeatFemaleMeshList ),
	m_OverHeatBuffList( rhs.m_OverHeatBuffList ),
	m_ChangeToDashRearFlyInfo( rhs.m_ChangeToDashRearFlyInfo ),
	m_ChangeToJumpRearFlyInfo( rhs.m_ChangeToJumpRearFlyInfo ),
	m_fRearDashGauge( rhs.m_fRearDashGauge ),
	m_fRearJumpGauge( rhs.m_fRearJumpGauge ),
	m_GuideEffect( rhs.m_GuideEffect ),
	m_OverHeatEffect( rhs.m_OverHeatEffect )
{
	Init();
}

ioResearcher::~ioResearcher()
{
}

void ioResearcher::Init()
{	
	m_ChargeState				= CS_NONE;
	m_GatheringState			= GS_FIRE;

	m_iCurCombo					= 0;	
	m_dwKeyReserveTime			= 0;
	
	m_dwAttackStartTime			= 0;
	m_dwExtendAttackEndTime		= 0;
	m_dwOverHeatAttackEndTime	= 0;
	m_dwChargingAttackEndTime	= 0;

	m_dwCurrExtendAttackIndex	= 0;

	m_fCurrMotionRate			= 0.0f;

	m_GatherEffectID			= -1;
	m_GatherMaxEffectID			= -1;
	m_GuideEffectID				= -1;
	m_OverHeatEffectID			= -1;

	m_bSkillUseItemMesh			= false;
	m_bBuffSetInvisible			= false;
	m_bObjectUseItemMesh		= false;

	m_vAttackDir				= ioMath::VEC3_ZERO;
	m_CurDirKey					= ioUserKeyInput::DKI_NONE;
}

void ioResearcher::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	EndGatherEffect( pOwner );
	EndGuideEffect( pOwner );

	m_ChargeState				= CS_NONE;
	m_iCurCombo					= 0;	
	m_dwKeyReserveTime			= 0;
	m_dwOverHeatAttackEndTime	= 0;
	m_dwCurrExtendAttackIndex	= 0;

	m_fCurrMotionRate			= 0.0f;

	m_vAttackDir				= ioMath::VEC3_ZERO;
	m_CurDirKey					= ioUserKeyInput::DKI_NONE;
}

void ioResearcher::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );

	rkLoader.LoadString_e( "pre_gather_animation", "", szKey, MAX_PATH );
	m_PreGatherAnimation = szKey;

	rkLoader.LoadString_e( "up_gather_animation", "", szKey, MAX_PATH );
	m_UpGatherAnimation = szKey;

	rkLoader.LoadString_e( "center_gather_animation", "", szKey, MAX_PATH );
	m_CenterGatherAnimation = szKey;

	rkLoader.LoadString_e( "down_gather_animation", "", szKey, MAX_PATH );
	m_DownGatherAnimation = szKey;
		
	rkLoader.LoadString_e( "up_extend_attack_animation", "", szKey, MAX_PATH );
	m_UpExtendAttackAnimation = szKey;

	rkLoader.LoadString_e( "down_extend_attack_animation", "", szKey, MAX_PATH );
	m_DownExtendAttackAnimation = szKey;

	rkLoader.LoadString_e( "max_gather_effect", "", szKey, MAX_PATH );
	m_GatherMaxEffect = szKey;

	rkLoader.LoadString_e( "guide_effect", "", szKey, MAX_PATH );
	m_GuideEffect = szKey;

	rkLoader.LoadString_e( "over_heat_effect", "", szKey, MAX_PATH );
	m_OverHeatEffect = szKey;

	m_dwHorzRotateWeight	= rkLoader.LoadInt_e( "horz_rotate_weight", 1000 );
	m_dwVertRotateWeight	= rkLoader.LoadInt_e( "vert_rotate_weight", 1500 );
	m_fVertUpLimitAngle		= rkLoader.LoadFloat_e( "vert_rotate_up_limit_angle", 45.0f );
	m_fVertDownLimitAngle	= rkLoader.LoadFloat_e( "vert_rotate_down_limit_angle", 15.0f );

	LoadAttackAttribute( "charging_attack", m_ChargingAttackAttribute, rkLoader );

	int iMax = rkLoader.LoadInt_e( "extend_max_count", 0 );
	for( int i = 0; i < iMax; ++i )
	{
		ExtendAttackInfo Info;
		wsprintf_e( szKey, "extend%d_attack", i + 1 );
		LoadAttackAttribute( szKey, Info.m_ExtendAttack, rkLoader );

		wsprintf_e( szKey, "extend%d_gauge_effect", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_Effect = szBuf;

		wsprintf_e( szKey, "extend%d_gauge_max", i + 1 );
		Info.m_fGaugeMax = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "extend%d_gauge_min", i + 1 );
		Info.m_fGaugeMin = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "extend%d_use_gauge", i + 1 );
		Info.m_fAddGauge = rkLoader.LoadInt( szKey, 0 );

		m_ExtendAttackInfoList.push_back( Info );
	}

	m_fOverHeatChekHeight = rkLoader.LoadFloat( "over_heat_gauge_check_height", 5.0f );
	LoadAttackAttribute( "over_heat_attack", m_OverHeatAttackAttribute, rkLoader );	
	LoadMeshProperty( rkLoader ); 
	LoadBuffProperty( rkLoader );

	LoadFlyInfo( rkLoader, "dash_rear_fly", m_ChangeToDashRearFlyInfo );
	m_fRearDashGauge = rkLoader.LoadFloat_e( "dash_rear_gauge", 15.0f );

	LoadFlyInfo( rkLoader, "jump_rear_fly", m_ChangeToJumpRearFlyInfo );
	m_fRearJumpGauge = rkLoader.LoadFloat_e( "jump_rear_gauge", 15.0f );
}

void ioResearcher::LoadMeshProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iMeshCnt = rkLoader.LoadInt_e( "over_heat_male_mesh_cnt", 0 );
	for( int i = 0; i < iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "over_heat_male_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_OverHeatMaleMeshList.push_back( szBuf );
	}

	iMeshCnt = rkLoader.LoadInt_e( "over_heat_female_mesh_cnt", 0 );
	for( int i = 0; i < iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "over_heat_female_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_OverHeatFemaleMeshList.push_back( szBuf );
	}
}

void ioResearcher::LoadBuffProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iMeshCnt = rkLoader.LoadInt_e( "over_heat_buff_cnt", 0 );
	for( int i = 0; i < iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "over_heat_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_OverHeatBuffList.push_back( szBuf );
	}
}

void ioResearcher::LoadFlyInfo( ioINILoader &rkLoader, const char* szName, ValkyrieFlyInfo& DestInfo )
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

ioItem* ioResearcher::Clone()
{
	return new ioResearcher( *this );
}

ioWeaponItem::WeaponSubType ioResearcher::GetSubType() const
{
	return WST_RESEARCHER;
}

void ioResearcher::OnEquiped( ioBaseChar *pOwner )
{
	ioAttackableItem::OnEquiped( pOwner );

	EndGatherEffect( pOwner );
	EndGuideEffect( pOwner );
	EndOverHeatEffect( pOwner );

	Init();

	LoadGatheringState( pOwner );
}

void ioResearcher::OnReleased( ioBaseChar *pOwner )
{
	ioAttackableItem::OnReleased( pOwner );

	if( m_GatheringState == GS_FIRE && GetCurBullet() >= GetMaxBullet() )
		m_GatheringState = GS_OVERHEAT;

	EndGatherEffect( pOwner );
	EndGuideEffect( pOwner );
	EndOverHeatEffect( pOwner );

	ReleaseMesh( pOwner );
	OnOverHeatRemoveBuff( pOwner );
	SaveGatheringState( pOwner );

	pOwner->ShowEquipItemMesh( true, ES_WEAPON );

	Init();
}

void ioResearcher::EndGatherEffect( ioBaseChar *pOwner )
{
	if( m_GatherEffectID != -1 )
	{
		pOwner->EndEffect( m_GatherEffectID );
		m_GatherEffectID = -1;
	}

	if( m_GatherMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatherMaxEffectID );
		m_GatherMaxEffectID = -1;
	}
}

void ioResearcher::EndGuideEffect( ioBaseChar *pOwner )
{
	if( m_GuideEffectID != -1 )
	{
		pOwner->EndEffect( m_GuideEffectID, false );
		m_GuideEffectID = -1;
	}
}

void ioResearcher::EndOverHeatEffect( ioBaseChar *pOwner )
{
	if( m_OverHeatEffectID != -1 )
	{
		pOwner->EndEffect( m_OverHeatEffectID, false );
		m_OverHeatEffectID = -1;
	}
}

// 어트리뷰트 호출 
void ioResearcher::SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, IN bool bExtra, IN bool bAutoTarget, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve, OUT DWORD& dwWeaponIndexBase )
{
	if( !pAttribute )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	dwWeaponIndexBase = pOwner->SetNormalAttackByAttribute( *pAttribute );

	m_szCurAni					= pAttribute->m_AttackAnimation;
	m_fCurAniRate				= pAttribute->m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;

	ioEntityGroup* pGrp	= pOwner->GetGroup();
	int iAniID			= pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	float fTimeRate		= pAttribute->m_fAttackAniRate;
	DWORD dwPreDelay	= pAttribute->m_dwPreDelay;	
	dwAttackEnd			= dwCurTime;
	dwAttackEnd			+= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( dwReserve > 0 )
		dwReserve += dwCurTime;

	if( bAutoTarget )
	{
		DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	if( bExtra )
	{
		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
	}
}

void ioResearcher::SetAttackAttribute( ioBaseChar *pOwner, const AttackAttribute& Attr )
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
	m_dwCurrWeaponIdx = pOwner->GetWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( Attr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	float fEventTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fEventTime > 0 )
	{
		m_dwKeyReserveTime = FRAMEGETTIME() + fEventTime + dwPreDelay;
	}
	else
	{
		m_dwKeyReserveTime = FRAMEGETTIME();
	}
}

// 노말 공격 
void ioResearcher::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();
	EndGuideEffect( pOwner );
	EndGatherEffect( pOwner );

	m_ChargeState				= CS_CHARGING;
	m_dwAttackStartTime			= FRAMEGETTIME();
	m_iCurCombo					= iCurCombo;
	m_dwCurrExtendAttackIndex	= 0;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimation );
	float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
}

void ioResearcher::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_CHARGING_ATTACK:
		OnChargingAttack( pOwner );
		break;
	case CS_EXTEND_FIRE:
		OnExtendAttack( pOwner );
		break;
	case CS_ATTACK_FIRE:
		OnNormalAttack( pOwner );
		break;
	case CS_OVERHEAT_FIRE:
		OnOverHeatAttack( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	}
}

void ioResearcher::OnNormalAttack( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioResearcher::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime	= FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();
	ClearKeyReserve();

	DWORD dwEndTime			= 0;	
	DWORD dwWeaponIndexBase	= 0;
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	SetAttack( pOwner, pAttribute, false, true, dwEndTime, m_dwKeyReserveTime, dwWeaponIndexBase );

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_ATTACK_FIRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// D꾹 
void ioResearcher::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && m_GatheringState == GS_FIRE )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToChargingAttackFire( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToAttackFire( pOwner );
	}
}

void ioResearcher::ChangeToChargingAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime	= FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();
	ClearKeyReserve();

	DWORD dwEndTime			= 0;	
	DWORD dwWeaponIndexBase	= 0;
	SetAttack( pOwner, &m_ChargingAttackAttribute, false, true, m_dwChargingAttackEndTime, m_dwKeyReserveTime, dwWeaponIndexBase );

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_CHARGING_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_CHARGING_ATTACK_FIRE );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioResearcher::OnChargingAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwChargingAttackEndTime < FRAMEGETTIME() )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			if( m_dwChargingAttackEndTime < FRAMEGETTIME() )
			{
				ChangeToGathering( pOwner );
			}
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				pOwner->SetState( CS_DELAY );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << static_cast<byte>( SS_CHARGING_ATTACK_CANCEL );
					kPacket << pOwner->GetTargetRot();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
}

// 특별 
void ioResearcher::ChangeToGathering( ioBaseChar *pOwner )
{
	pOwner->StopAutoTargetTracking();
	pOwner->SetChargingState( true );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_UpGatherAnimation, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_CenterGatherAnimation, 0.0f, FLOAT1 );
	pGrp->SetLoopAni( m_DownGatherAnimation, 0.0f, 0.0f );

	EndGuideEffect( pOwner );
	ioEffect* pGuildeEffect = pOwner->AttachEffect( m_GuideEffect );
	if( pGuildeEffect )
		m_GuideEffectID = pGuildeEffect->GetUniqueID();

	m_ChargeState	= CS_GATHERING;
	m_vAttackDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_GATHERING );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioResearcher::CheckDirectionKey( ioBaseChar* pOwner )
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

void ioResearcher::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() )
	{
		ChangeToExtendAttackFire( pOwner );
	}
	else
	{
		CheckMaxEffect( pOwner );
		CheckChargeEffect( pOwner );

		bool bChnage = false;
		if( CheckDirectionKey( pOwner ) )
			bChnage = true;

		ProcessRotateHorz( pOwner, m_dwHorzRotateWeight, m_CurDirKey, IsLeftRotate( pOwner, m_CurDirKey ) );

		if( pOwner->IsNeedProcess() && pOwner->IsJumpKeyDown() && ProcessRotateVert( pOwner, m_dwVertRotateWeight, UDS_UP ) )
		{
			bChnage = true;
		}
		else if( pOwner->IsNeedProcess() && pOwner->IsDefenseKeyDown() && ProcessRotateVert( pOwner, m_dwVertRotateWeight, UDS_DOWN ) )
		{
			bChnage = true;
		}
		ProcessRotateAnimation( pOwner );

		if( bChnage && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( SS_GATHERING_DIR );
			kPacket << pOwner->GetTargetRot();
			kPacket << static_cast<byte>( m_CurDirKey );
			kPacket << m_vAttackDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioResearcher::CheckMaxEffect( ioBaseChar *pOwner )
{
	if( GetCurBullet() == GetMaxBullet() )
	{
		ioEffect* pEffect = pOwner->GetEffectByFileName( m_GatherMaxEffect );
		if( !pEffect )
		{
			EndGatherEffect( pOwner );
			ioEffect* pNewEffect = pOwner->AttachEffect( m_GatherMaxEffect );
			if( pNewEffect )
			{
				m_GatherMaxEffectID = pNewEffect->GetUniqueID();
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<byte>( SS_MAX_EFFECT );
				kPacket << pOwner->GetTargetRot();
				kPacket << m_fCurExtraGauge;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioResearcher::CheckChargeEffect( ioBaseChar* pOwner )
{
	ioEffect* pEffect = pOwner->AttachEffect( m_GatherEffect );
	if( pEffect )
		m_GatherEffectID = pEffect->GetUniqueID();

	for( ExtendAttackInfoList::iterator iter = m_ExtendAttackInfoList.begin(); iter != m_ExtendAttackInfoList.end(); ++iter )
	{
		const ExtendAttackInfo& Info = *iter;
		if( Info.m_fGaugeMin <= GetCurBullet() && GetCurBullet() <= Info.m_fGaugeMax )
		{
			ioEffect* pEffect = pOwner->GetEffectByFileName( Info.m_Effect );
			if( !pEffect )
			{
				EndGatherEffect( pOwner );
				ioEffect* pNewEffect = pOwner->AttachEffect( Info.m_Effect );
				if( pNewEffect )
				{
					m_GatherEffectID = pNewEffect->GetUniqueID();
				}

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << static_cast<byte>( SS_CHARGE_EFFECT );
					kPacket << pOwner->GetTargetRot();
					kPacket << m_fCurExtraGauge;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
}

bool ioResearcher::IsLeftRotate( ioBaseChar* pOwner, DirKey eNewDirKey )
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

bool ioResearcher::IsLimitAngle( float fLimitAngle, float fCurrAngle )
{
	if( fLimitAngle > 0.0f && fLimitAngle < fCurrAngle )
		return true;

	return false;
}

bool ioResearcher::ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, DirKey eDir, bool bLeft )
{
	if( !pOwner || fRotateTime <= 0 )
		return false;

	if( eDir == ioUserKeyInput::DKI_NONE )
		return false;

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

	return true;
}

bool ioResearcher::ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, UpDownState eState )
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

	if( eState == UDS_UP )
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
	else if( eState == UDS_DOWN )
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

void ioResearcher::ProcessRotateAnimation( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioAnimation *pUp		= pGrp->GetLoopInPlay( m_UpGatherAnimation );
	ioAnimation *pCenter	= pGrp->GetLoopInPlay( m_CenterGatherAnimation );
	ioAnimation *pDown		= pGrp->GetLoopInPlay( m_DownGatherAnimation );

	if( !pUp || !pCenter || !pDown )
		return;

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

//특별 공격 
void ioResearcher::ChangeToExtendAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	EndGatherEffect( pOwner );
	EndGuideEffect( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->SetState( CS_ATTACK );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetChargingState( false );
	ClearCancelInfo();
	ClearKeyReserve();

	m_ChargeState = CS_EXTEND_FIRE;
	//m_GatheringState = GS_OVERHEAT_PRE;
	
	for( ExtendAttackInfoList::iterator iter = m_ExtendAttackInfoList.begin(); iter != m_ExtendAttackInfoList.end(); ++iter )
	{
		const ExtendAttackInfo& Info = *iter;
		if( Info.m_fGaugeMin <= GetCurBullet() && GetCurBullet() <= Info.m_fGaugeMax )
		{
			DWORD dwKeyRserve = 0;
			SetAttack( pOwner, &Info.m_ExtendAttack, false, true, m_dwExtendAttackEndTime, dwKeyRserve, m_dwCurrExtendAttackIndex );
			ChangeToExtendAttackFireAnimation( pOwner, &Info.m_ExtendAttack );
			SetUseExtraGauge( Info.m_fAddGauge );
			break;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_EXTEND_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurExtraGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioResearcher::ChangeToExtendAttackFireAnimation( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute )
{
	ioEntityGroup* pGrp	= pOwner->GetGroup();
	ioAnimation* pCenter = pGrp->GetActionInPlay( pAttribute->m_AttackAnimation );
	float fTimeRate		= pAttribute->m_fAttackAniRate;
	DWORD dwPreDelay	= pAttribute->m_dwPreDelay;	
	
	//센터
	if( m_vAttackDir.y == 0.0f || m_fCurrMotionRate == 0.0f )
	{	
		if( pCenter )
			pCenter->ForceWeight( FLOAT1 );
	}
	//업
	else if( m_vAttackDir.y > 0.0f )
	{
		pGrp->SetActionAni( m_UpExtendAttackAnimation, 0.0f, 0.0f, m_fCurrMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		if( pCenter )
			pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
	}
	//다운
	else
	{
		pGrp->SetActionAni( m_DownExtendAttackAnimation, 0.0f, 0.0f, m_fCurrMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		if( pCenter )
			pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
	}
}

void ioResearcher::OnExtendAttack( ioBaseChar *pOwner )
{
	if( m_dwExtendAttackEndTime < FRAMEGETTIME() )
	{
		if( !OnOverHeatCheck( pOwner ) )
			pOwner->SetState( CS_DELAY );
	}
}

// 과열 상태 
bool ioResearcher::OnOverHeatCheck( ioBaseChar* pOwner )
{
	switch( m_GatheringState )
	{
	case GS_FIRE:
	//case GS_OVERHEAT_PRE:
		{
			if( GetCurBullet() >= GetMaxBullet() )
			{
				m_GatheringState = GS_OVERHEAT;
				pOwner->ShowEquipItemMesh( false, ES_WEAPON );
				ChangeMesh( pOwner );				
				EndGatherEffect( pOwner );
				EndGuideEffect( pOwner );
				ChangeToOverHeatState( pOwner );
				return true;
			}
		}
		break;
	}

	return false;
}

void ioResearcher::ChangeToOverHeatState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->SetChargingState( false );
	pOwner->InitExtendAttackTagInfo();
	AttachOverHeatEffect( pOwner );

	ClearCancelInfo();
	ClearKeyReserve();

	m_ChargeState = CS_OVERHEAT_FIRE;	
	SetAttack( pOwner, &m_OverHeatAttackAttribute, false, true, m_dwOverHeatAttackEndTime, m_dwKeyReserveTime, m_dwCurrExtendAttackIndex );
	ChangeToExtendAttackFireAnimation( pOwner, &m_OverHeatAttackAttribute );
}

void ioResearcher::OnOverHeatAttack( ioBaseChar *pOwner )
{
	if( m_dwOverHeatAttackEndTime < FRAMEGETTIME() )
	{
		pOwner->SetState( CS_DELAY );
		OnOverHeatAddBuff( pOwner );
	}
}

void ioResearcher::ChangeToOverHeatEndState( ioBaseChar *pOwner )
{
	m_GatheringState = GS_FIRE;
	
	ReleaseMesh( pOwner );
	pOwner->ShowEquipItemMesh( true, ES_WEAPON );
	OnOverHeatRemoveBuff( pOwner );
	EndOverHeatEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_OVER_HEAT_END );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioResearcher::AttachOverHeatEffect( ioBaseChar* pOwner )
{
	EndOverHeatEffect( pOwner );

	ioEffect* pEffect = pOwner->AttachEffect( m_OverHeatEffect );
	if( pEffect )
	{
		m_OverHeatEffectID = pEffect->GetUniqueID();
	}
}

void ioResearcher::ChangeMesh( ioBaseChar* pOwner )
{
	if( pOwner->IsMale() )
	{
		for( int i = 0; i < (int)m_OverHeatMaleMeshList.size(); ++i )
			pOwner->AddEntity( m_OverHeatMaleMeshList[i] );
	}
	else
	{
		for( int i = 0; i < (int)m_OverHeatFemaleMeshList.size(); ++i )
			pOwner->AddEntity( m_OverHeatFemaleMeshList[i] );
	}
}

void ioResearcher::ReleaseMesh( ioBaseChar* pOwner )
{
	if( pOwner->IsMale() )
	{
		for( int i = 0; i < (int)m_OverHeatMaleMeshList.size(); ++i )
			pOwner->DestroyEntity( m_OverHeatMaleMeshList[i] );
	}
	else
	{
		for( int i = 0; i < (int)m_OverHeatFemaleMeshList.size(); ++i )
			pOwner->DestroyEntity( m_OverHeatFemaleMeshList[i] );
	}
}

void ioResearcher::OnOverHeatAddBuff( ioBaseChar* pOwner )
{
	for( int i = 0; i < (int)m_OverHeatBuffList.size(); ++i )
		pOwner->AddNewBuff( m_OverHeatBuffList[i], pOwner->GetCharName(), GetName(), NULL );
}

void ioResearcher::OnOverHeatRemoveBuff( ioBaseChar* pOwner )
{
	for( int i = 0; i < (int)m_OverHeatBuffList.size(); ++i )
		pOwner->RemoveBuff( m_OverHeatBuffList[i] );
}

void ioResearcher::SaveGatheringState( ioBaseChar* pOwner )
{
	ioResearcherCache* pCache = ToResearcherCache( g_SoldierCacheManager.GetCache( pOwner->GetCharName(), SoldierCache::CT_RESSEARCHER ) );
	if( !pCache )
	{
		pCache = ToResearcherCache( g_SoldierCacheManager.NewCache( pOwner->GetCharName(), SoldierCache::CT_RESSEARCHER ) );
	}

	pCache->m_GatheringState = m_GatheringState;
}

void ioResearcher::LoadGatheringState( ioBaseChar* pOwner )
{
	ioResearcherCache* pCache = ToResearcherCache( g_SoldierCacheManager.GetCache( pOwner->GetCharName(), SoldierCache::CT_RESSEARCHER ) );
	if( pCache )
	{
		m_GatheringState = pCache->m_GatheringState;
	}
	else
	{
		m_GatheringState = GS_FIRE;
	}
}

void ioResearcher::CheckCurrItemtemMeshProcess( ioBaseChar* pOwner )
{
	//스킬 사용시에 메쉬 처리
	CheckSkillInvisibleWeapon( pOwner );

	//오브젝트 아이템을 끼고 있을때 메쉬 처리
	CheckEquipOject( pOwner );

	//invisible_weapon을 세팅한 버프
	CheckInvisibleWeaponBuff( pOwner );

	//특별 모션
	CheckSpecialMotion( pOwner );
}

void ioResearcher::CheckSpecialMotion( ioBaseChar* pOwner )
{
	if( m_bSkillUseItemMesh || m_bBuffSetInvisible || m_bObjectUseItemMesh || pOwner->GetState() == CS_USING_SKILL )
		return;

	if( pOwner->GetState() == CS_SPECIAL_MOTION || pOwner->GetState() == CS_ETCITEM_MOTION )
	{			
		if( !m_bSpeicalMotionMesh )
		{
			m_bSpeicalMotionMesh = true;
			ReleaseMesh( pOwner );
			EndOverHeatEffect( pOwner );

			pOwner->ShowEquipItemMesh( false, ES_WEAPON );
		}
	}
	else
	{
		if( m_bSpeicalMotionMesh )
		{
			m_bSpeicalMotionMesh = false;

			ReleaseMesh( pOwner );
			EndOverHeatEffect( pOwner );

			if( m_GatheringState == GS_OVERHEAT )
			{
				ChangeMesh( pOwner );
				AttachOverHeatEffect( pOwner );
				pOwner->ShowEquipItemMesh( false, ES_WEAPON );
			}
			else
			{
				pOwner->ShowEquipItemMesh( true, ES_WEAPON );
			}
		}
	}	
}

void ioResearcher::CheckSkillInvisibleWeapon( ioBaseChar* pOwner )
{
	if( m_bBuffSetInvisible || m_bObjectUseItemMesh || m_bSpeicalMotionMesh )
		return;

	if( pOwner->GetState() == CS_ETCITEM_MOTION || pOwner->GetState() == CS_SPECIAL_MOTION )
		return;

	ioSkill* pSkill = pOwner->GetCurrActiveSkill();
	if( pOwner->GetState() == CS_USING_SKILL )
	{
		if( pSkill && pSkill->CheckUseItemMesh() )
		{
			if( !m_bSkillUseItemMesh )
			{
				m_bSkillUseItemMesh = true;
				ReleaseMesh( pOwner );
				EndOverHeatEffect( pOwner );
			}
		}
		else
		{
			ReleaseMesh( pOwner );
			EndOverHeatEffect( pOwner );

			if( m_GatheringState == GS_OVERHEAT )
			{
				ChangeMesh( pOwner );
				AttachOverHeatEffect( pOwner );

				pOwner->ShowEquipItemMesh( false, ES_WEAPON );
			}
		}
	}
	else
	{
		if( m_bSkillUseItemMesh )
		{
			m_bSkillUseItemMesh = false;

			ReleaseMesh( pOwner );
			EndOverHeatEffect( pOwner );

			if( m_GatheringState == GS_OVERHEAT )
			{
				ChangeMesh( pOwner );
				AttachOverHeatEffect( pOwner );
			}
		}
	}
}

void ioResearcher::CheckEquipOject( ioBaseChar* pOwner )
{
	if( m_bSkillUseItemMesh || m_bBuffSetInvisible || m_bSpeicalMotionMesh || pOwner->GetState() == CS_SPECIAL_MOTION )
		return;

	if( pOwner->GetState() == CS_ETCITEM_MOTION || pOwner->GetState() == CS_SPECIAL_MOTION || pOwner->GetState() == CS_USING_SKILL )
		return;

	//오브젝트 아이템 사용유무
	if( pOwner->GetObject() )
	{
		if( !m_bObjectUseItemMesh )
		{
			m_bObjectUseItemMesh = true;

			ReleaseMesh( pOwner );
			EndOverHeatEffect( pOwner );
		}
	}
	else
	{
		if( m_bObjectUseItemMesh )
		{
			m_bObjectUseItemMesh = false;

			ReleaseMesh( pOwner );
			EndOverHeatEffect( pOwner );

			if( m_GatheringState == GS_OVERHEAT )
			{
				ChangeMesh( pOwner );
				AttachOverHeatEffect( pOwner );

				pOwner->ShowEquipItemMesh( false, ES_WEAPON );
			}			
		}
	}
}

void ioResearcher::CheckInvisibleWeaponBuff( ioBaseChar* pOwner )
{
	if( m_bSkillUseItemMesh || m_bObjectUseItemMesh || m_bSpeicalMotionMesh )
		return;

	if( pOwner->GetState() == CS_ETCITEM_MOTION || pOwner->GetState() == CS_SPECIAL_MOTION || pOwner->GetState() == CS_USING_SKILL )
		return;

	if( 0 < pOwner->GetInvisibleSetBuffCount() )
	{
		if( !m_bBuffSetInvisible )
		{
			m_bBuffSetInvisible = true;

			ReleaseMesh( pOwner );
			EndOverHeatEffect( pOwner );
		}
	}
	else
	{
		if( m_bBuffSetInvisible )
		{
			m_bBuffSetInvisible = false;

			ReleaseMesh( pOwner );
			EndOverHeatEffect( pOwner );

			if( m_GatheringState == GS_OVERHEAT )
			{
				ChangeMesh( pOwner );
				AttachOverHeatEffect( pOwner );

				pOwner->ShowEquipItemMesh( false, ES_WEAPON );
			}
		}
	}
}

void ioResearcher::ClearSpecialState( ioBaseChar* pOwner )
{
	ClearWeapon( pOwner );

	m_ChargeState				= CS_NONE;
	m_RearState					= RS_NONE;

	m_dwKeyReserveTime			= 0;	

	m_dwCurrWeaponIdx			= 0;	
	m_dwCurrExtendAttackIndex	= 0;

	m_dwUpStartTime				= 0;
	m_dwOverHeatAttackEndTime	= 0;
}

void ioResearcher::ClearWeapon( ioBaseChar* pOwner )
{
	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCurrWeaponIdx ) );
	if( pZone )
		pZone->SetWeaponDead();
}

void ioResearcher::ChangeToRearFlySpecialState( ioBaseChar *pOwner, RearState eState )
{
	if( !pOwner )
		return;

	pOwner->SetState( CS_RESEARCHER_REAR_FLY );
	DirKey eDir = pOwner->CheckCurDirKey();
	if( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetRotToDirKey( eDir );
	}

	m_RearState = eState;

	switch( eState )
	{
	case RS_DASH:
		SetAttackAttribute( pOwner, m_ChangeToDashRearFlyInfo.m_Attribute );
		SetUseExtraGauge( m_fRearDashGauge );
		break;
	case RS_JUMP:
		SetAttackAttribute( pOwner, m_ChangeToJumpRearFlyInfo.m_Attribute );
		SetUseExtraGauge( m_fRearJumpGauge );
		break;
	}

	m_dwUpStartTime		= FRAMEGETTIME();
	m_fUpCurrRange		= 0.0f;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_CHANGE_REAR_FLY );
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<byte>( eState );
		kPacket << static_cast<byte>( eDir );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioResearcher::ProcessRearFlySpecialState( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	switch( m_RearState )
	{
	case RS_DASH:
		if( ProcessMove( pOwner, m_ChangeToDashRearFlyInfo, ioMath::VEC3_ZERO, m_dwUpStartTime, m_fUpCurrRange, false ) != MS_MOVE )
		{
			if( OnOverHeatCheck( pOwner ) )
			{
				m_RearState = RS_OVER_HEAT_ATTACK;
			}
			else
			{
				pOwner->SetSKillEndJumpState( m_ChangeToDashRearFlyInfo.m_fChargeFlyJumpAmt );
			}
		}
		break;
	case RS_JUMP:
		if( ProcessMove( pOwner, m_ChangeToJumpRearFlyInfo, ioMath::VEC3_ZERO, m_dwUpStartTime, m_fUpCurrRange, false ) != MS_MOVE )
		{

			if( OnOverHeatCheck( pOwner ) )
			{
				m_RearState = RS_OVER_HEAT_ATTACK;
			}
			else
			{
				pOwner->SetSKillEndJumpState( m_ChangeToJumpRearFlyInfo.m_fChargeFlyJumpAmt );
			}	
		}
		break;
	case RS_OVER_HEAT_ATTACK:
		{
			if( m_dwOverHeatAttackEndTime < FRAMEGETTIME() )
			{
				pOwner->SetSKillEndJumpState( m_ChangeToDashRearFlyInfo.m_fChargeFlyJumpAmt );
				OnOverHeatAddBuff( pOwner );
			}
		}
		break;
	}
}

ioResearcher::MoveState ioResearcher::ProcessMove( ioBaseChar *pOwner, const ValkyrieFlyInfo& rkFlyInfo, const D3DXVECTOR3& vFindDir, DWORD& dwStartTime, float& fCurrRange, bool bCheckLanding )
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

bool ioResearcher::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioResearcher::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:		
		case CS_GATHERING:
			if( !bCheck )
				return false;

			return true;
		}
	}
	return false;
}

bool ioResearcher::IsEnableJumpKeyEtcAction( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_EXTEND_FIRE )
			return false;
	}

	return true;
}

void ioResearcher::FillInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{	
	rkPacket << (byte)m_ChargeState;
	rkPacket << (byte)m_GatheringState;
	rkPacket << m_fCurExtraGauge;
}

void ioResearcher::SetInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	byte eChargeState, eGatheringState;
	rkPacket >> eChargeState >> eGatheringState;
	rkPacket >> m_fCurExtraGauge;

	if( eChargeState == CS_GATHERING )
	{
		ChangeToGathering( pOwner );
		CheckChargeEffect( pOwner );
		CheckMaxEffect( pOwner );
	}

	if( eGatheringState == GS_OVERHEAT )
	{
		m_GatheringState = (GatheringState)eGatheringState;
		pOwner->ShowEquipItemMesh( false, ES_WEAPON );

		ChangeMesh( pOwner );
		AttachOverHeatEffect( pOwner );
		OnOverHeatAddBuff( pOwner );
		EndGatherEffect( pOwner );
		EndGuideEffect( pOwner );
	}
}

void ioResearcher::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	byte eState;
	rkPacket >> eState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	switch( eState )
	{
	case SS_ATTACK_FIRE:
		{	
			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case SS_CHARGING_ATTACK_FIRE:
		{
			ChangeToChargingAttackFire( pOwner );
		}
		break;
	case SS_CHARGING_ATTACK_CANCEL:
		{
			pOwner->SetState( CS_DELAY );
		}
		break;
	case SS_GATHERING:
		{
			ChangeToGathering( pOwner );
		}
		break;
	case SS_GATHERING_DIR:
		{
			byte eDir;
			rkPacket >> eDir;
			m_CurDirKey = (DirKey)eDir;
			rkPacket >> m_vAttackDir;
		}
		break;
	case SS_CHARGE_EFFECT:
		{
			rkPacket >> m_fCurExtraGauge;
			CheckMaxEffect( pOwner );
		}
		break;
	case SS_MAX_EFFECT:
		{
			rkPacket >> m_fCurExtraGauge;
			CheckMaxEffect( pOwner );
		}
		break;
	case SS_EXTEND_ATTACK:
		{
			rkPacket >> m_fCurExtraGauge;
			ChangeToExtendAttackFire( pOwner );
		}
		break;
	case SS_CHANGE_REAR_FLY:
		{
			byte eRearState, eDirKey;
			rkPacket >> eRearState >> eDirKey;
			pOwner->SetTargetRotToDirKey( (DirKey)eDirKey );			
			ChangeToRearFlySpecialState( pOwner, (RearState)eRearState );
		}
		break;
	case SS_OVER_HEAT_END:
		{
			ChangeToOverHeatEndState( pOwner );
		}
		break;
	}	
}

void ioResearcher::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();		
	switch( m_ChargeState )
	{
	case CS_GATHERING:
		{
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		break;
	default:
		{			
			if( m_GatheringState == GS_OVERHEAT && pOwner->GetState() != CS_SPECIAL_MOTION && pOwner->GetState() != CS_USING_SKILL && pOwner->GetState() != CS_ETCITEM_MOTION )
			{
				float fGapHeight = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
				if( fGapHeight < m_fOverHeatChekHeight && !pOwner->GetObject() && 0 == pOwner->GetInvisibleSetBuffCount() )
				{
					float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
					m_fCurExtraGauge -= fGauge;
					m_fCurExtraGauge = max( 0, m_fCurExtraGauge );

					if( m_fCurExtraGauge <= 0 )
					{
						ChangeToOverHeatEndState( pOwner );
					}
				}
			}
		}
		break;
	}
}

int ioResearcher::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioResearcher::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioResearcher::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioResearcher::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioResearcher::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge += fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioResearcher::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioMissileWeapon* pMissile = ToMissileWeapon( pWeapon );
	if( pMissile && m_dwCurrExtendAttackIndex == pMissile->GetWeaponIndex() )
	{
		pMissile->SetMoveDir( m_vAttackDir );
		m_dwCurrExtendAttackIndex = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
ioResearcherRearFlySpecialState::ioResearcherRearFlySpecialState()
{
}

ioResearcherRearFlySpecialState::~ioResearcherRearFlySpecialState()
{
}

void ioResearcherRearFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioResearcherRearFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioResearcher *pResearcher = ToResearcher( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pResearcher && eNewState != CS_ATTACK )
		pResearcher->ClearSpecialState( pOwner );
}

bool ioResearcherRearFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioResearcherRearFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioResearcher *pResearcher = ToResearcher( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pResearcher )
		pResearcher->ProcessRearFlySpecialState( pOwner, pOwner->GetCreator() );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioResearcherRearFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioResearcherRearFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}