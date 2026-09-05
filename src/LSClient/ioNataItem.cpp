
#include "stdafx.h"

#include "ioNataItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioNataItem::ioNataItem()
{	
	m_fCurGauge = 0.0f;
	Init();
}

ioNataItem::ioNataItem( const ioNataItem &rhs )
	: ioWeaponItem( rhs ),
	  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_vNormalChargeComboAttList( rhs.m_vNormalChargeComboAttList ),
	  m_nNormalChargeMaxCombo( rhs.m_nNormalChargeMaxCombo ),
	  m_fNormalEndJumpPower( rhs.m_fNormalEndJumpPower ),
	  m_fNormalUseGauge( rhs.m_fNormalUseGauge ),
	  m_vDashChargeComboAttList( rhs.m_vDashChargeComboAttList ),
	  m_nDashChargeMaxCombo( rhs.m_nDashChargeMaxCombo ),
	  m_fDashEndJumpPower( rhs.m_fDashEndJumpPower ),
	  m_fDashUseGauge( rhs.m_fDashUseGauge ),
	  m_vJumpChargeComboAttList( rhs.m_vJumpChargeComboAttList ),
	  m_nJumpChargeMaxCombo( rhs.m_nJumpChargeMaxCombo ),
	  m_fJumpEndJumpPower( rhs.m_fJumpEndJumpPower ),
	  m_fJumpUseGauge( rhs.m_fJumpUseGauge ),
	  m_fMaxGauge( rhs.m_fMaxGauge ),
	  m_fNeedGauge( rhs.m_fNeedGauge ),  
	  m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	  m_szFlyCameraBuff( rhs.m_szFlyCameraBuff ),
	  m_fFlyUseGauge( rhs.m_fFlyUseGauge ),
	  m_fFlyDecGauge( rhs.m_fFlyDecGauge ),
	  m_FlyPre( rhs.m_FlyPre ),
	  m_szFlyDelayAni( rhs.m_szFlyDelayAni ),
	  m_fFlyDelayAniRate( rhs.m_fFlyDelayAniRate ),
	  m_szFlyMoveFrAni( rhs.m_szFlyMoveFrAni ),
	  m_fFlyMoveFrAniRate( rhs.m_fFlyMoveFrAniRate ),
	  m_szFlyMoveBkAni( rhs.m_szFlyMoveBkAni ),
	  m_fFlyMoveBkAniRate( rhs.m_fFlyMoveBkAniRate ),
	  m_szFlyMoveLtAni( rhs.m_szFlyMoveLtAni ),
	  m_fFlyMoveLtAniRate( rhs.m_fFlyMoveLtAniRate ),
	  m_szFlyMoveRtAni( rhs.m_szFlyMoveRtAni ),
	  m_fFlyMoveRtAniRate( rhs.m_fFlyMoveRtAniRate ),
	  m_fFlyMoveForceAmt( rhs.m_fFlyMoveForceAmt ),
	  m_fFlyMoveForceFric( rhs.m_fFlyMoveForceFric ),
	  m_bFlyEnableDownAim( rhs.m_bFlyEnableDownAim ),
	  m_fFlyAimAngle( rhs.m_fFlyAimAngle ),
	  m_fFlyAimRange( rhs.m_fFlyAimRange ),
	  m_fFlyMinRange( rhs.m_fFlyMinRange ),
	  m_fFlyMarkerRange( rhs.m_fFlyMarkerRange ),
	  m_szFlyReadyMarker( rhs.m_szFlyReadyMarker ),
	  m_szFlyAimingMarker( rhs.m_szFlyAimingMarker ),
	  m_szFlyAimedMarker( rhs.m_szFlyAimedMarker ),
	  m_FlyTargetAttack( rhs.m_FlyTargetAttack ),
	  m_szFlyTargetMoveAni( rhs.m_szFlyTargetMoveAni ),
	  m_fFlyTargetMoveAniRate( rhs.m_fFlyTargetMoveAniRate ),
	  m_fMaxTargetMoveDist( rhs.m_fMaxTargetMoveDist ),
	  m_fTargetMoveSpeed( rhs.m_fTargetMoveSpeed ),
	  m_fTargetCheckDist( rhs.m_fTargetCheckDist ),
	  m_szFlyTargetAttachLeftAni( rhs.m_szFlyTargetAttachLeftAni ),
	  m_szFlyTargetAttachRightAni( rhs.m_szFlyTargetAttachRightAni ),
	  m_fFlyTargetAttachAniRate( rhs.m_fFlyTargetAttachAniRate ),
	  m_dwTargetAttachRotateSpeed( rhs.m_dwTargetAttachRotateSpeed ),
	  m_vTargetAttachOffset( rhs.m_vTargetAttachOffset ),
	  m_fTargetAttachEndDist( rhs.m_fTargetAttachEndDist ),
	  m_dwFlyChargeAttackTime( rhs.m_dwFlyChargeAttackTime ),
	  m_fFlyChargeComboUseGauge( rhs.m_fFlyChargeComboUseGauge ),
	  m_vFlyChargeComboAttList( rhs.m_vFlyChargeComboAttList ),
	  m_fFlyChargeComboReserveEndDist( rhs.m_fFlyChargeComboReserveEndDist ),
	  m_nFlyMaxCombo( rhs.m_nFlyMaxCombo ),
	  m_fFlyNormalAttackUseGauge( rhs.m_fFlyNormalAttackUseGauge ),
	  m_FlyNormalAttack( rhs.m_FlyNormalAttack ),
	  m_fFlyNormalAttackReserveEndDist( rhs.m_fFlyNormalAttackReserveEndDist ),
	  m_fFlyChangeTargetUseGauge( rhs.m_fFlyChangeTargetUseGauge ),
	  m_fFlyChangeTargetColRange( rhs.m_fFlyChangeTargetColRange ),
	  m_fFlyChangeTargetColAngle( rhs.m_fFlyChangeTargetColAngle ),
	  m_fFlyChangeTargetColUpHeight( rhs.m_fFlyChangeTargetColUpHeight ),
	  m_fFlyChangeTargetColUnderHeight( rhs.m_fFlyChangeTargetColUnderHeight ),
	  m_bFlyChangeEnableTargetDown( rhs.m_bFlyChangeEnableTargetDown ),
	  m_fFlyHighDecreaseHeight( rhs.m_fFlyHighDecreaseHeight ),
	  m_fFlyHighDecreaseFlyTic( rhs.m_fFlyHighDecreaseFlyTic ),
	  m_dwChangeAttactDirTime( rhs.m_dwChangeAttactDirTime ),
	  m_szFlyChargeMoveAni( rhs.m_szFlyChargeMoveAni ),
	  m_fFlyChargeMoveAniRate( rhs.m_fFlyChargeMoveAniRate ),
	  m_fMaxChargeMoveDist( rhs.m_fMaxChargeMoveDist ),
	  m_fChargeMoveSpeed( rhs.m_fChargeMoveSpeed ),
	  m_szFlyReturnMoveAni( rhs.m_szFlyReturnMoveAni ),
	  m_fFlyReturnMoveAniRate( rhs.m_fFlyReturnMoveAniRate ),
	  m_fMaxReturnMoveDist( rhs.m_fMaxReturnMoveDist ),
	  m_fReturnMoveSpeed( rhs.m_fReturnMoveSpeed ),
	  m_fReturnMoveCheckDist( rhs.m_fReturnMoveCheckDist ),
	  m_fReturnMoveEndDist( rhs.m_fReturnMoveEndDist ),
	  m_fFlyEndJumpPower( rhs.m_fFlyEndJumpPower ),
	  m_szTargetAttachEffect( rhs.m_szTargetAttachEffect ),
	  m_fTargetAttachEffectHeight( rhs.m_fTargetAttachEffectHeight )
{
	m_fCurGauge = 0.0f;
	Init();
}

ioNataItem::~ioNataItem()
{
}

void ioNataItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	LoadAttackProperty( rkLoader );	
	LoadFlyProperty( rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

ioItem* ioNataItem::Clone()
{
	return new ioNataItem( *this );
}

ioWeaponItem::WeaponSubType ioNataItem::GetSubType() const
{
	return WST_NATA_ITEM;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	Init();

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_nNormalCombo = 0;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_WeaponItemState = WS_NONE;
	}
	else
	{
		//공격 준비 애니메이션 설정
		ioEntityGroup *pGrp = pOwner->GetGroup();

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwAttackStartTime = dwCurTime;
		m_nNormalCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_WeaponItemState = WS_CHECK_CHARGING;
	}
}

void ioNataItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponItemState )
	{
	case WS_CHECK_CHARGING:
		{
			CheckCharging( pOwner );
		}
		break;
	case WS_NORMAL_ATTACK:
		{
			//input_jump_s 이벤트
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

void ioNataItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if ( bFullTime && IsEnableFlyGauge() )
	{
		SetNataFlySpecialState( pOwner, true );
		return;
	}
	
	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

void ioNataItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
	
	ioMissileWeapon *pMissileWeapon = ToMissileWeapon( pWeapon );
	if ( !m_AimedTarget.IsEmpty() && pMissileWeapon )
	{
		ioBaseChar *pChar = pMissileWeapon->GetOwner();
		if ( !pChar )
			return;

		ioBaseChar *pTarget = pChar->GetBaseChar( m_AimedTarget );
		if ( !pTarget )
			return;

		D3DXVECTOR3 vTargetDir = pTarget->GetMidPositionByRate() - pMissileWeapon->GetPosition();
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );
		pMissileWeapon->SetMoveDir( vTargetDir );
	}

	ioOwnerAttackWeapon *pOwnerWeapon = ToOwnerAttackWeapon( pWeapon );
	if( !m_AimedTarget.IsEmpty() && pOwnerWeapon )
	{
		ioBaseChar *pChar = pOwnerWeapon->GetOwner();
		if ( !pChar )
			return;

		ioPlayStage *pStage = pChar->GetCreator();
		if ( !pStage )
			return;

		pOwnerWeapon->SetTargetName( m_AimedTarget, pStage );
	}
}

void ioNataItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_NORMAL_ATTACK:
		{
			rkPacket >> m_nNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			SetNormalAttackState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_INIT_COMBO:
		{
			bool bNormal, bDash, bJump;
			rkPacket >> bNormal;
			rkPacket >> bDash;
			rkPacket >> bJump;

			SetNataComboSpecialState( pOwner, bNormal, bDash, bJump );
		}
		break;
	case WS_SPECIAL_STATE_INIT_FLY:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			bool bStartJump;
			rkPacket >> bStartJump;

			SetNataFlySpecialState( pOwner, bStartJump );
		}
		break;
	case WS_SPECIAL_STATE_PROCESS_COMBO:
		{
			RecvComboSpecialState( pOwner, rkPacket );
		}
		break;
	case WS_SPECIAL_STATE_PROCESS_FLY:
		{
			RecvFlySpecialState( pOwner, rkPacket );
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::Init()
{	
	m_WeaponItemState = WS_NONE;
	m_NataComboSpecialState = NCSS_None;
	m_nNormalCombo = 0;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	m_dwComboEndMotionEndTime = 0;
	m_dwComboReserveTime = 0;

	m_nCurCombo = 0;
	m_nCurMaxCombo = 0;
	m_vCurChargeComboAttList.clear();
	m_fCurEndJumpPower = 0.0f;

	m_NataFlySpecialState = NFSS_None;
	m_AimedTarget.Clear();
	m_dwFlyEndMotionEndTime = 0;
	m_dwFlyForceTime = 0;
	m_vFlyForceDir = ioMath::VEC3_ZERO;
	m_fCurTargetMoveDist = 0.0f;
	m_fCurTargetAttachAngle = 0.0f;
	m_dwFlyAttackStartTime = 0;

	m_dwFlyComboEndMotionEndTime = 0;
	m_dwFlyComboReserveTime = 0;

	m_nFlyCurCombo = 0;
	m_bFlyAttachLeftDir = false;
	m_bFlyAttachRightDir = true;
	m_dwCheckChangeAttactDirTime = 0;

	m_fCurChargeMoveDist = 0.0f;
	m_fCurReturnMoveDist = 0.0f;
	m_bSetReturnMoveCheck = false;

	m_fCurFlyEndDist = 0.0f;
	m_dwCurChargeComboForceTime = 0;
	m_fCurChargeComboForceAmt = 0.0f;
	m_fCurChargeComboForceFric = 0.0f;

	m_bFlyStartJump = false;

	ClearCancelInfo();
}

void ioNataItem::LoadAttackProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int nExtendCnt = rkLoader.LoadInt_e( "attack_ready_cnt", 0 );	
	m_AttackReadyAniList.clear();

	for( int i=0; i<nExtendCnt; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( szBuf );
	}
	
	m_nNormalChargeMaxCombo = rkLoader.LoadInt_e( "normal_charge_combo_max_cnt", 0 );
	m_vNormalChargeComboAttList.clear();
	for( int i=0; i < m_nNormalChargeMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf( szKey, "normal_charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_vNormalChargeComboAttList.push_back( kAttribute );
	}	
	m_fNormalEndJumpPower = rkLoader.LoadFloat_e( "normal_end_jump_power", 0.0f );
	m_fNormalUseGauge = rkLoader.LoadFloat_e( "normal_use_gauge", 0.0f );

	m_nDashChargeMaxCombo = rkLoader.LoadInt_e( "dash_charge_combo_max_cnt", 0 );
	m_vDashChargeComboAttList.clear();
	for( int i=0; i < m_nDashChargeMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf( szKey, "dash_charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_vDashChargeComboAttList.push_back( kAttribute );
	}	
	m_fDashEndJumpPower = rkLoader.LoadFloat_e( "dash_end_jump_power", 0.0f );
	m_fDashUseGauge = rkLoader.LoadFloat_e( "dash_use_gauge", 0.0f );

	m_nJumpChargeMaxCombo = rkLoader.LoadInt_e( "jump_charge_combo_max_cnt", 0 );
	m_vJumpChargeComboAttList.clear();
	for( int i=0; i < m_nJumpChargeMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf( szKey, "jump_charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_vJumpChargeComboAttList.push_back( kAttribute );
	}	
	m_fJumpEndJumpPower = rkLoader.LoadFloat_e( "Jump_end_jump_power", 0.0f );
	m_fJumpUseGauge = rkLoader.LoadFloat_e( "jump_use_gauge", 0.0f );
}

void ioNataItem::RecvComboSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	case NCSS_Combo_Attack:
		{
			rkPacket >> m_nCurCombo;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetComboAttackState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case NCSS_End:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			rkPacket >> m_fCurEndJumpPower;
			pOwner->SetSKillEndJumpState( m_fCurEndJumpPower, false, false, false, true );
		}
		break;
	}
}

void ioNataItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	if( IsEnableComboGauge( true, false, false ) && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetNataComboSpecialState( pOwner, true, false, false );
	}
	else
		SetNormalAttackState( pOwner );
}

void ioNataItem::SetNormalAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_nNormalCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_nNormalCombo );
	m_WeaponItemState = WS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_WeaponItemState;
		kPacket << m_nNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;
			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	case CS_NATA_FLY:
		{		
			float fGauge = m_fFlyDecGauge * fTimePerSec;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fFlyHighDecreaseHeight < fGapHeight )
					fGauge = m_fFlyHighDecreaseFlyTic * fTimePerSec;
			}

			m_fCurGauge -= fGauge;
			m_fCurGauge = max( 0.0f, m_fCurGauge );
		}
		return;
	}
}

int ioNataItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioNataItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioNataItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioNataItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

int ioNataItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

void ioNataItem::DecreaseGauge( float fUseGauge )
{
	m_fCurGauge -= fUseGauge;
	m_fCurGauge = (float)max( 0.f, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioNataItem::IsEnableComboGauge( bool bNormal, bool bDash, bool bJump )
{
	float fUseGauge;
	if ( bNormal )
		fUseGauge = m_fNormalUseGauge;
	else if ( bDash )
		fUseGauge = m_fDashUseGauge;
	else if ( bJump )
		fUseGauge = m_fJumpUseGauge;

	if( m_fCurGauge >= fUseGauge )
		return true;

	return false;
}

bool ioNataItem::IsEnableFlyGauge()
{
	if( m_fCurGauge >= m_fFlyUseGauge )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::ClearState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	if ( pOwner->IsNeedProcess() )
		pOwner->RemoveBuff( m_szFlyCameraBuff );

	Init();
}

void ioNataItem::SetNataComboSpecialState( ioBaseChar *pOwner, bool bNormal, bool bDash, bool bJump )
{
	if( !pOwner )
		return;
	
	Init();

	pOwner->SetState( CS_NATA_COMBO );
	m_NataComboSpecialState = NCSS_Combo_Attack;
	
	if ( bNormal )
	{
		m_vCurChargeComboAttList = m_vNormalChargeComboAttList;
		m_nCurMaxCombo = m_nNormalChargeMaxCombo;
		m_fCurEndJumpPower = m_fNormalEndJumpPower;
		DecreaseGauge( m_fNormalUseGauge );
	}
	else if ( bDash )
	{
		m_vCurChargeComboAttList = m_vDashChargeComboAttList;
		m_nCurMaxCombo = m_nDashChargeMaxCombo;
		m_fCurEndJumpPower = m_fDashEndJumpPower;
		DecreaseGauge( m_fDashUseGauge );
	}
	else if ( bJump )
	{
		m_vCurChargeComboAttList = m_vJumpChargeComboAttList;
		m_nCurMaxCombo = m_nJumpChargeMaxCombo;
		m_fCurEndJumpPower = m_fJumpEndJumpPower;
		DecreaseGauge( m_fJumpUseGauge );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT_COMBO;
		kPacket << bNormal;
		kPacket << bDash;
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessNataComboSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();	
	switch( m_NataComboSpecialState )
	{	
	case NCSS_Combo_Attack:
		{
			if ( !CheckComboInput( pOwner ) )
			{					
				if ( m_dwComboEndMotionEndTime > 0 && m_dwComboEndMotionEndTime < dwCurTime )
					CheckReserveState( pOwner );	
			}		
		}
		break;
	case NCSS_Combo_Check:
		{
			if ( pOwner->IsAttackKey() && m_nCurCombo < m_nCurMaxCombo && m_dwComboReserveTime < dwCurTime )
			{
				m_NataComboSpecialState = NCSS_Combo_Attack;
				return;
			}
			else if ( m_dwComboEndMotionEndTime > 0 && m_dwComboEndMotionEndTime < dwCurTime )
				CheckReserveState( pOwner );
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetComboAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_vCurChargeComboAttList.empty() || m_nCurMaxCombo <= 0 )
		return;
	
	pOwner->InitActionStopDelay( true );
	
	const AttackAttribute &pAttribute = m_vCurChargeComboAttList[m_nCurCombo];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackAnimation );
	if ( iAniID == -1)
		return;
	
	float fTimeRate  = pAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute.m_dwPreDelay;
	DWORD dwEndDelay = pAttribute.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwComboReserveTime = dwCurTime;
	m_dwComboReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_dwComboEndMotionEndTime = dwCurTime;
	m_dwComboEndMotionEndTime += dwPreDelay;
	m_dwComboEndMotionEndTime += dwEndDelay;
	m_dwComboEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pAttribute.m_AttackEndAnimation.IsEmpty() )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = pAttribute.m_fAttackEndAniRate;

		pGrp->SetActionAni( pAttribute.m_AttackEndAnimation, 0.0f, FLOAT100, FLOAT1, FLOAT1/fEndTimeRate, fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackEndAnimation );
		m_dwComboEndMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( pAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, pAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	
	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_NataComboSpecialState = NCSS_Combo_Check;
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	
	SetCurAttackAniForInputJump( pAttribute.m_AttackAnimation, fTimeRate );

	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS_COMBO;
			kPacket << NCSS_Combo_Attack;
			kPacket << m_nCurCombo;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}

	m_nCurCombo++;
}

bool ioNataItem::CheckComboInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_nCurCombo < m_nCurMaxCombo && m_dwComboReserveTime < dwCurTime )
	{
		SetComboAttackState( pOwner );
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::ReserveInputKey(ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;
}

void ioNataItem::CheckReserveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_bAttackKeyReserved )
	{
		SetReserveAttack( pOwner );
		return;
	}
	else if( m_bDefenseKeyReserved && pOwner->IsDefenseKeyDown() )
	{
		SetReserveDefence( pOwner );
		return;
	}
	else if( m_bJumpKeyReserved )
	{
		SetReserveJump( pOwner );
		return;
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_COMBO;
		kPacket << NCSS_End;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << m_fCurEndJumpPower;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetSKillEndJumpState( m_fCurEndJumpPower, false, false, true, true );
}

void ioNataItem::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioNataItem::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioNataItem::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::LoadFlyProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString( "fly_camera_buff", "", szBuf, MAX_PATH );	
	m_szFlyCameraBuff = szBuf;

	m_fFlyUseGauge = rkLoader.LoadFloat_e( "fly_use_gauge", 0.0f );
	m_fFlyDecGauge = rkLoader.LoadFloat_e( "fly_dec_gauge", 0.0f );

	LoadAttackAttribute_e( "fly_pre", m_FlyPre, rkLoader );

	rkLoader.LoadString( "fly_delay_ani", "", szBuf, MAX_PATH );	
	m_szFlyDelayAni = szBuf;
	m_fFlyDelayAniRate = rkLoader.LoadFloat_e( "fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString( "fly_move_fr_ani", "", szBuf, MAX_PATH );	
	m_szFlyMoveFrAni = szBuf;
	m_fFlyMoveFrAniRate = rkLoader.LoadFloat_e( "fly_move_fr_ani_rate", FLOAT1 );

	rkLoader.LoadString( "fly_move_bk_ani", "", szBuf, MAX_PATH );	
	m_szFlyMoveBkAni = szBuf;
	m_fFlyMoveBkAniRate = rkLoader.LoadFloat_e( "fly_move_bk_ani_rate", FLOAT1 );

	rkLoader.LoadString( "fly_move_lt_ani", "", szBuf, MAX_PATH );	
	m_szFlyMoveLtAni = szBuf;
	m_fFlyMoveLtAniRate = rkLoader.LoadFloat_e( "fly_move_lt_ani_rate", FLOAT1 );

	rkLoader.LoadString( "fly_move_rt_ani", "", szBuf, MAX_PATH );	
	m_szFlyMoveRtAni = szBuf;
	m_fFlyMoveRtAniRate = rkLoader.LoadFloat_e( "fly_move_rt_ani_rate", FLOAT1 );

	m_fFlyMoveForceAmt = rkLoader.LoadFloat_e( "fly_force_move", FLOAT1 );
	m_fFlyMoveForceFric = rkLoader.LoadFloat_e( "fly_force_friction", FLOAT1 );	

	m_bFlyEnableDownAim = rkLoader.LoadBool_e( "fly_enable_down_aim", false );
	   
	m_fFlyAimAngle = rkLoader.LoadFloat_e( "fly_aim_angle", 45.0f );
	m_fFlyAimRange = rkLoader.LoadFloat_e( "fly_aim_range", FLOAT1000 );
	m_fFlyMinRange = rkLoader.LoadFloat_e( "fly_aim_min_range", 0.0f );
	m_fFlyMarkerRange = rkLoader.LoadFloat_e( "fly_marker_range", 0.0f );

	rkLoader.LoadString( "fly_ready_marker", "", szBuf, MAX_PATH );	
	m_szFlyReadyMarker = szBuf;
	rkLoader.LoadString( "fly_aiming_marker", "", szBuf, MAX_PATH );	
	m_szFlyAimingMarker = szBuf;
	rkLoader.LoadString( "fly_aimed_marker", "", szBuf, MAX_PATH );	
	m_szFlyAimedMarker = szBuf;

	LoadAttackAttribute_e( "fly_target_attack", m_FlyTargetAttack, rkLoader );

	rkLoader.LoadString( "fly_target_move_ani", "", szBuf, MAX_PATH );	
	m_szFlyTargetMoveAni = szBuf;
	m_fFlyTargetMoveAniRate = rkLoader.LoadFloat_e( "fly_target_move_ani_rate", FLOAT1 );

	m_fMaxTargetMoveDist = rkLoader.LoadFloat_e( "fly_target_move_dist", 0.0f );
	m_fTargetMoveSpeed = rkLoader.LoadFloat_e( "fly_target_move_speed", 0.0f );
	m_fTargetCheckDist = rkLoader.LoadFloat_e( "fly_target_check_dist", 0.0f );

	rkLoader.LoadString( "fly_target_attach_left_ani", "", szBuf, MAX_PATH );	
	m_szFlyTargetAttachLeftAni = szBuf;
	rkLoader.LoadString( "fly_target_attach_right_ani", "", szBuf, MAX_PATH );	
	m_szFlyTargetAttachRightAni = szBuf;

	m_fFlyTargetAttachAniRate = rkLoader.LoadFloat_e( "fly_target_attach_ani_rate", FLOAT1 );

	m_dwTargetAttachRotateSpeed = rkLoader.LoadInt_e( "fly_target_attach_rotate_speed", 360 );

	m_vTargetAttachOffset.x = rkLoader.LoadFloat_e( "fly_target_attach_offset_x", 0.0f );
	m_vTargetAttachOffset.y = rkLoader.LoadFloat_e( "fly_target_attach_offset_y", 0.0f );
	m_vTargetAttachOffset.z = rkLoader.LoadFloat_e( "fly_target_attach_offset_z", 0.0f );

	m_fTargetAttachEndDist = rkLoader.LoadFloat_e( "fly_target_attach_end_dist", 0.0f );

	m_dwFlyChargeAttackTime = rkLoader.LoadInt_e( "fly_charge_attack_time", 0 );

	m_nFlyMaxCombo = rkLoader.LoadInt_e( "fly_charge_combo_max_cnt", 0 );
	m_vFlyChargeComboAttList.clear();
	for( int i=0; i < m_nFlyMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf( szKey, "fly_charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_vFlyChargeComboAttList.push_back( kAttribute );
	}	
	
	LoadAttackAttribute_e( "fly_normal_attack", m_FlyNormalAttack, rkLoader );

	m_fFlyChargeComboUseGauge = rkLoader.LoadFloat_e( "fly_charge_combo_use_gauge", 0.0f );
	m_fFlyNormalAttackUseGauge = rkLoader.LoadFloat_e( "fly_normal_attack_use_gauge", 0.0f );

	m_fFlyChangeTargetUseGauge = rkLoader.LoadFloat_e( "fly_change_target_use_gauge", 0.0f );
	m_fFlyChangeTargetColRange = rkLoader.LoadFloat_e( "fly_change_target_range", 0.0f );
	m_fFlyChangeTargetColAngle = rkLoader.LoadFloat_e( "fly_change_target_angle", 0.0f );
	m_fFlyChangeTargetColUpHeight = rkLoader.LoadFloat_e( "fly_change_target_up_height", 0.0f );
	m_fFlyChangeTargetColUnderHeight = rkLoader.LoadFloat_e( "fly_change_target_under_height", 0.0f );
	m_bFlyChangeEnableTargetDown = rkLoader.LoadBool_e( "fly_change_target_enable_down_aim", false );
	   
	m_fFlyHighDecreaseFlyTic = rkLoader.LoadFloat_e( "fly_high_decrease_fly_tic", 0.0f );
	m_fFlyHighDecreaseHeight = rkLoader.LoadFloat_e( "fly_high_decrease_height", 0.0f );

	m_dwChangeAttactDirTime = rkLoader.LoadInt_e( "fly_target_attach_change_dir_time", 0 );

	rkLoader.LoadString( "fly_charge_move_ani", "", szBuf, MAX_PATH );	
	m_szFlyChargeMoveAni = szBuf;
	m_fFlyChargeMoveAniRate = rkLoader.LoadFloat_e( "fly_charge_move_ani_rate", FLOAT1 );

	m_fMaxChargeMoveDist = rkLoader.LoadFloat_e( "fly_charge_move_dist", 0.0f );
	m_fChargeMoveSpeed = rkLoader.LoadFloat_e( "fly_charge_move_speed", 0.0f );

	rkLoader.LoadString( "fly_return_move_ani", "", szBuf, MAX_PATH );	
	m_szFlyReturnMoveAni = szBuf;
	m_fFlyReturnMoveAniRate = rkLoader.LoadFloat_e( "fly_return_move_ani_rate", FLOAT1 );

	m_fMaxReturnMoveDist = rkLoader.LoadFloat_e( "fly_return_move_dist", 0.0f );
	m_fReturnMoveSpeed = rkLoader.LoadFloat_e( "fly_return_move_speed", 0.0f );
	m_fReturnMoveCheckDist = rkLoader.LoadFloat_e( "fly_return_move_check_dist", 0.0f );

	m_fFlyEndJumpPower = rkLoader.LoadFloat_e( "fly_end_jump_power", 0.0f );

	m_fFlyChargeComboReserveEndDist = rkLoader.LoadFloat_e( "fly_charge_combo_reserve_end_dist", 0.0f );
	m_fFlyNormalAttackReserveEndDist = rkLoader.LoadFloat_e( "fly_normal_attack_reserve_end_dist", 0.0f );
	m_fReturnMoveEndDist = rkLoader.LoadFloat_e( "fly_return_move_end_dist", 0.0f );

	rkLoader.LoadString( "fly_target_attach_effect", "", szBuf, MAX_PATH );	
	m_szTargetAttachEffect = szBuf;

	m_fTargetAttachEffectHeight = rkLoader.LoadFloat_e( "fly_target_attach_effect_height", 0.0f );
}

void ioNataItem::RecvFlySpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotAndMoveDirChange( qtRot );

	switch( nSpecialState )
	{
	case NFSS_Fly_Delay:
		{
			SetFlyDelayState( pOwner );
		}
		break;
	case NFSS_Fly_Move:
		{
			int nKey;
			rkPacket >> nKey;

			SetFlyMoveState( pOwner, (ioUserKeyInput::DirKeyInput)nKey );		
		}
		break;
	case NFSS_Find_Target:
		{
			rkPacket >> m_AimedTarget;
			
			if ( !m_AimedTarget.IsEmpty() )
			{
				pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			}
		}
		break;
	case NFSS_Target_Attack:
		{
			SetFlyTargetAttackState( pOwner );
		}
		break;
	case NFSS_Target_Move:
		{
			SetFlyTargetMoveState( pOwner );
		}
		break;
	case NFSS_Target_Attach:
		{
			rkPacket >> m_fCurTargetAttachAngle;

			rkPacket >> m_bFlyAttachLeftDir;
			rkPacket >> m_bFlyAttachRightDir;

			bool bAttachEffect;
			rkPacket >> bAttachEffect;

			SetFlyTargetAttachState( pOwner, bAttachEffect );
		}
		break;
	case NFSS_Check_Attack:
		{
			SetFlyCheckAttackState( pOwner );
		}
		break;
	case NFSS_Charge_Move:
		{
			SetFlyChargeMoveState( pOwner );
		}
		break;
	case NFSS_Combo_Attack:
		{
			rkPacket >> m_nFlyCurCombo;
			
			SetFlyComboAttackState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case NFSS_Return_Move:
		{
			bool bSetReturnMoveCheck;
			rkPacket >> bSetReturnMoveCheck;

			rkPacket >> m_bFlyAttachLeftDir;
			rkPacket >> m_bFlyAttachRightDir;

			SetFlyReturnMoveState( pOwner, bSetReturnMoveCheck );
		}
		break;
	case NFSS_Normal_Attack:
		{
			SetFlyNormalAttackState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetNataFlySpecialState( ioBaseChar *pOwner, bool bStartJump )
{
	if ( !pOwner )
		return;

	Init();

	pOwner->SetState( CS_NATA_FLY );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM, m_szFlyReadyMarker, m_szFlyAimingMarker, m_szFlyAimedMarker, "", "" );
	pOwner->SetTargetMarkerRange( m_fFlyMarkerRange );

	DecreaseGauge( m_fFlyUseGauge );
	m_fCurEndJumpPower = m_fFlyEndJumpPower;
	m_bFlyStartJump = bStartJump;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT_FLY;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << bStartJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessNataFlyState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_NataFlySpecialState )
	{
	case NFSS_None:
		SetFlyPreState( pOwner );
		break;
	case NFSS_Fly_Pre:
		ProcessFlyPreState( pOwner );
		break;
	case NFSS_Fly_Delay:
		ProcessFlyDelayState( pOwner );
		break;
	case NFSS_Fly_Move:
		ProcessFlyMoveState( pOwner );
		break;
	case NFSS_Target_Attack:
		ProcessFlyTargetAttackState( pOwner );
		break;
	case NFSS_Target_Move:
		ProcessFlyTargetMoveState( pOwner );
		break;
	case NFSS_Target_Attach:
		ProcessFlyTargetAttachState( pOwner );
		break;
	case NFSS_Check_Attack:
		ProcessFlyCheckAttackState( pOwner );
		break;
	case NFSS_Charge_Move:
		ProcessFlyChargeMoveState( pOwner );
		break;
	case NFSS_Combo_Attack:
		ProcessFlyComboAttackState( pOwner );
		break;
	case NFSS_Return_Move:
		ProcessFlyReturnMoveState( pOwner );
		break;
	case NFSS_Normal_Attack:
		ProcessFlyNormalAttackState( pOwner );
		break;
	}
}

bool ioNataItem::IsFlyCollisionSkipState()
{
	switch( m_NataFlySpecialState )
	{
	case NFSS_Target_Move:
	case NFSS_Target_Attach:
	case NFSS_Charge_Move:
	case NFSS_Return_Move:
		return true;
	}

	return false;
}

bool ioNataItem::IsFlySendControlInfoToUser()
{
	switch( m_NataFlySpecialState )
	{
	case NFSS_Target_Move:
	case NFSS_Charge_Move:
	case NFSS_Return_Move:
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyPreState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
		
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_FlyPre.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_FlyPre.m_fAttackAniRate;
	DWORD dwPreDelay = m_FlyPre.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFlyEndMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;	
	
	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_FlyPre.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	if ( m_bFlyStartJump )
		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_NataFlySpecialState = NFSS_Fly_Pre;
}

void ioNataItem::ProcessFlyPreState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
		
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFlyEndMotionEndTime < dwCurTime )
	{
		if ( pOwner->IsNeedProcess() )
		{
			pOwner->AddNewBuff( m_szFlyCameraBuff, pOwner->GetCharName(), "", NULL );

			if ( pOwner->IsSettedDirection() )
				SetFlyMoveState( pOwner, pOwner->CheckCurDirKey() );
			else
				SetFlyDelayState( pOwner );
		}
	}

	pOwner->CheckExtraAniJump();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
		
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szFlyDelayAni, FLOAT100, FLOAT1, FLOAT1/m_fFlyDelayAniRate );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_NataFlySpecialState = NFSS_Fly_Delay;

	if ( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Fly_Delay;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessFlyDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	if ( CheckFlyEndInput( pOwner ) )
	{
		CheckReserveState( pOwner );
		return;
	}

	if ( FindTarget( pOwner ) )
	{
		SetFlyTargetAttackState( pOwner );
		return;
	}
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	if ( CheckMoveInput( pOwner ) )
	{
		SetFlyMoveState( pOwner, pOwner->CheckCurDirKey() );
		return;
	}
}

bool ioNataItem::CheckMoveInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( pOwner->IsSettedDirection() )
		return true;

	return false;
}

bool ioNataItem::CheckFlyEndInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( m_fCurGauge <= 0.0f || pOwner->IsJumpKey() )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir )
{
	if( !pOwner )
		return;
	
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
		SetFlyMoveAni( pOwner, m_szFlyMoveFrAni, m_fFlyMoveFrAniRate, vMoveDir );
		break;
	case 1:
		if( iRest > 15 )
			SetFlyMoveAni( pOwner, m_szFlyMoveRtAni, m_fFlyMoveRtAniRate, vMoveDir );
		else
			SetFlyMoveAni( pOwner, m_szFlyMoveFrAni, m_fFlyMoveFrAniRate, vMoveDir );
		break;
	case 2:
		if( iRest <= 40 )
			SetFlyMoveAni( pOwner, m_szFlyMoveRtAni, m_fFlyMoveRtAniRate, vMoveDir );
		else
			SetFlyMoveAni( pOwner, m_szFlyMoveBkAni, m_fFlyMoveBkAniRate, vMoveDir );
		break;
	case 3:
	case 4:
		SetFlyMoveAni( pOwner, m_szFlyMoveBkAni, m_fFlyMoveBkAniRate, vMoveDir );
		break;
	case 5:
		if( iRest > 5 )
			SetFlyMoveAni( pOwner, m_szFlyMoveLtAni, m_fFlyMoveLtAniRate, vMoveDir );
		else
			SetFlyMoveAni( pOwner, m_szFlyMoveBkAni, m_fFlyMoveBkAniRate, vMoveDir );
		break;
	case 6:
		if( iRest <= 30 )
			SetFlyMoveAni( pOwner, m_szFlyMoveLtAni, m_fFlyMoveLtAniRate, vMoveDir );
		else
			SetFlyMoveAni( pOwner, m_szFlyMoveFrAni, m_fFlyMoveFrAniRate, vMoveDir );
		break;
	}

	if ( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Fly_Move;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << (int)eDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::SetFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
		
	int iAniID = pGrp->GetAnimationIdx( szAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = fAniRate;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFlyEndMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_dwFlyForceTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_vFlyForceDir = vDir;

	m_NataFlySpecialState = NFSS_Fly_Move;
}

void ioNataItem::ProcessFlyMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( CheckFlyEndInput( pOwner ) )
	{
		CheckReserveState( pOwner );
		return;
	}

	if ( FindTarget( pOwner ) )
	{
		SetFlyTargetAttackState( pOwner );
		return;
	}
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwFlyEndMotionEndTime < dwCurTime )
	{
		SetFlyDelayState( pOwner );
		return;
	}

	if( m_dwFlyForceTime > 0 && m_dwFlyForceTime < dwCurTime )
	{
		pOwner->SetForcePower( m_vFlyForceDir, m_fFlyMoveForceAmt, m_fFlyMoveForceFric );
		m_dwFlyForceTime = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioNataItem::FindTarget( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	ioHashString szTarget;
	m_AimedTarget.Clear();
	
	if( pOwner->CheckAimedTarget( m_fFlyAimAngle, m_fFlyMinRange, m_fFlyAimRange, szTarget, m_bFlyEnableDownAim ) )
	{
		m_AimedTarget = szTarget;
		pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );		
	}

	if( CompareAimedTarget( pOwner ) )
		m_AimedTarget.Clear();

	if ( m_AimedTarget.IsEmpty() )
		return false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Find_Target;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

bool ioNataItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) 
		return false;

	float fAimRange = m_fFlyAimRange;
	float fAimAngle = m_fFlyAimAngle;

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = pOwner->GetMoveDir();

	float fHalfCosine = cosf( DEGtoRAD( fAimAngle ) * FLOAT05 );

	float fLimitDistSq = fAimRange * fAimRange;
	float fMinDistSq = m_fFlyMinRange * m_fFlyMinRange;

	// Ball
	if( pBall )
	{
		vDiff = pBall->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
				return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyTargetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_FlyTargetAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_FlyTargetAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_FlyTargetAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetReservedSliding( m_FlyTargetAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_FlyTargetAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFlyEndMotionEndTime = dwCurTime;
	m_dwFlyEndMotionEndTime += dwPreDelay;
	m_dwFlyEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	m_NataFlySpecialState = NFSS_Target_Attack;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Target_Attack;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessFlyTargetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if ( pTarget )
	{
		D3DXVECTOR3 vTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();					
		pOwner->SetTargetRotToDir( vTargetDir, true );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFlyEndMotionEndTime < dwCurTime )
	{
		if ( pOwner->IsNeedProcess() )
			SetFlyTargetMoveState( pOwner );
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyTargetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( m_szFlyTargetMoveAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fFlyTargetMoveAniRate );	

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_NataFlySpecialState = NFSS_Target_Move;
	m_fCurTargetMoveDist = 0.0f;

	if ( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Target_Move;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessFlyTargetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	D3DXVECTOR3 vTargetDir = pOwner->GetWorldOrientation() * -ioMath::UNIT_Z;
	if ( !m_AimedTarget.IsEmpty() )
	{
		ioBaseChar* pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if ( pTarget )
		{
			vTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();					
			pOwner->SetTargetRotToDir( vTargetDir, true );
		}

		if ( D3DXVec3LengthSq( &vTargetDir ) <= m_fTargetCheckDist * m_fTargetCheckDist )
		{
			if ( pOwner->IsNeedProcess() )
				SetFlyTargetAttachState( pOwner, true );
			return;
		}
	}

	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	if ( m_fCurTargetMoveDist >= m_fMaxTargetMoveDist || CheckTargetMove( pOwner, m_fCurTargetMoveDist, m_fTargetMoveSpeed, vTargetDir ) )
		CheckReserveState( pOwner );
}

bool ioNataItem::CheckTargetMove( ioBaseChar *pOwner, float &fCurMoveDist, float fMoveSpeed, D3DXVECTOR3 vMoveDir )
{
	if( !pOwner ) 
		return true;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return true;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( fMoveSpeed > 0.0f && vMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = fMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = vMoveDir * fFinalMoveAmt;
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

			D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
			vDiff = vTotalMove - vDiff;
			float fMoveDist = D3DXVec3Length( &vDiff );
			fCurMoveDist += fMoveDist;

			if( pOwner->GetForceAmt() != 0.0f )
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
						vCurPos.z,
						pOwner,
						false,
						pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}

		if ( bCol )
			return true;
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyTargetAttachState( ioBaseChar *pOwner, bool bAttachEffect )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if ( m_bFlyAttachLeftDir )
		pGrp->SetLoopAni( m_szFlyTargetAttachLeftAni, FLOAT100, FLOAT1, FLOAT1/m_fFlyTargetAttachAniRate );
	else if ( m_bFlyAttachRightDir )
		pGrp->SetLoopAni( m_szFlyTargetAttachRightAni, FLOAT100, FLOAT1, FLOAT1/m_fFlyTargetAttachAniRate );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_NataFlySpecialState = NFSS_Target_Attach;
	m_fCurFlyEndDist = m_fTargetAttachEndDist;
	//pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_nFlyCurCombo = 0;
	m_dwFlyComboReserveTime = 0;
	m_dwFlyComboEndMotionEndTime = 0;
	m_dwFlyAttackStartTime = 0;

	if ( bAttachEffect)
		pOwner->AttachEffect( m_szTargetAttachEffect );

	if ( pOwner->IsNeedProcess() )
		m_fCurTargetAttachAngle = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetWorldOrientation() ) );

	if ( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Target_Attach;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << m_fCurTargetAttachAngle;
		kPacket << m_bFlyAttachLeftDir;
		kPacket << m_bFlyAttachRightDir;
		kPacket << bAttachEffect;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessFlyTargetAttachState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	if ( !CheckEnableAttachTarget( pOwner ) || CheckFlyEndInput( pOwner ) || !CheckTargetAttachPos( pOwner ) )
	{
		CheckReserveState( pOwner );
		return;
	}

	if ( CheckAttachAttackInput( pOwner ) )
	{
		SetFlyCheckAttackState( pOwner );
		return;
	}

	if ( CheckAttachDefenceInput( pOwner ) )
	{
		SetFlyTargetChangeState( pOwner );
		return;
	}

	if ( CheckChangeAttachDir( pOwner ) )
		return;
}

bool ioNataItem::CheckTargetAttachPos( ioBaseChar *pOwner )
{
	if ( !pOwner || m_AimedTarget.IsEmpty() )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if ( !pTarget )
		return false;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwTargetAttachRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	m_fCurTargetAttachAngle = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetWorldOrientation() ) );

	if ( m_bFlyAttachLeftDir )
	{
		m_fCurTargetAttachAngle += fRotateAngle;
		if ( m_fCurTargetAttachAngle >= 360.0f )
		{
			m_fCurTargetAttachAngle -= 360.0f;
			m_fCurTargetAttachAngle = max( m_fCurTargetAttachAngle, 0.0f );
		}	
	}
	else if ( m_bFlyAttachRightDir )
	{
		m_fCurTargetAttachAngle -= fRotateAngle;
		if ( m_fCurTargetAttachAngle <= -360.0f )
		{
			m_fCurTargetAttachAngle += 360.0f;
			m_fCurTargetAttachAngle = min( m_fCurTargetAttachAngle, 0.0f );
		}	
	}
	
	float fCurOwnerPosY = pOwner->GetWorldPosition().y;

	float fYawR = DEGtoRAD( m_fCurTargetAttachAngle );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
	
	D3DXVECTOR3 vNewPos = pTarget->GetWorldPosition() + qtRotate * m_vTargetAttachOffset;
	float fMapHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z );
	if ( vNewPos.y < fMapHeight )
		vNewPos.y = fMapHeight;

	pOwner->SetWorldPosition( vNewPos );

	D3DXVECTOR3 vPos = vNewPos;
	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	if ( fabs( vPos.y - fCurOwnerPosY ) >= m_fTargetAttachEffectHeight )
		pOwner->AttachEffect( m_szTargetAttachEffect );

	D3DXVECTOR3 vTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();					
	pOwner->SetTargetRotToDir( vTargetDir, true );

	return true;
}

bool ioNataItem::CheckAttachAttackInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( pOwner->IsAttackKey() )
		return true;

	return false;
}

bool ioNataItem::CheckAttachDefenceInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( pOwner->IsDefenseKey() )
		return true;

	return false;
}

bool ioNataItem::CheckEnableAttachTarget( ioBaseChar *pOwner )
{
	if ( !pOwner || m_AimedTarget.IsEmpty() )
		return false;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if ( !pTarget )
		return false;

	switch( pTarget->GetState() )
	{
	case CS_DROP_ZONE_DOWN:
	//case CS_FALL:
		return false;
	}

	if ( pTarget->HasHideBuff() )
		return false;

	D3DXVECTOR3 vDist = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	if ( D3DXVec3LengthSq( &vDist ) >= m_fCurFlyEndDist * m_fCurFlyEndDist )
		return false;
	
	return true;
}

bool ioNataItem::CheckChangeAttachDir( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckChangeAttactDirTime + m_dwChangeAttactDirTime > dwCurTime )
		return false;

	if ( !m_bFlyAttachLeftDir && pOwner->IsLeftKeyDown() )
	{
		m_dwCheckChangeAttactDirTime = dwCurTime;
		m_bFlyAttachLeftDir = true;
		m_bFlyAttachRightDir = false;
		SetFlyTargetAttachState( pOwner, false );
		return true;
	}
	else if ( !m_bFlyAttachRightDir && pOwner->IsRightKeyDown() )
	{
		m_dwCheckChangeAttactDirTime = dwCurTime;
		m_bFlyAttachLeftDir = false;
		m_bFlyAttachRightDir = true;
		SetFlyTargetAttachState( pOwner, false );
		return true;
	}
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyCheckAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_dwFlyAttackStartTime = FRAMEGETTIME();
	m_NataFlySpecialState = NFSS_Check_Attack;

	if ( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Check_Attack;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessFlyCheckAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	CheckFlyAttackInput( pOwner );
}

void ioNataItem::CheckFlyAttackInput( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	if( m_fFlyChargeComboUseGauge <= m_fCurGauge && pOwner->IsAttackKeyDown() )
	{
		if( m_dwFlyAttackStartTime + m_dwFlyChargeAttackTime <= FRAMEGETTIME() && m_nFlyCurCombo < m_nFlyMaxCombo )
			SetFlyChargeMoveState( pOwner );
	}
	else if ( m_fFlyNormalAttackUseGauge <= m_fCurGauge )
		SetFlyNormalAttackState( pOwner );
	else
		CheckReserveState( pOwner );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyChargeMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( m_szFlyChargeMoveAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fFlyChargeMoveAniRate );	

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_NataFlySpecialState = NFSS_Charge_Move;
	m_fCurChargeMoveDist = 0.0f;

	if ( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Charge_Move;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessFlyChargeMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	D3DXVECTOR3 vTargetDir = pOwner->GetWorldOrientation() * -ioMath::UNIT_Z;
	if ( !m_AimedTarget.IsEmpty() )
	{
		ioBaseChar* pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if ( pTarget )
		{
			vTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();					
			pOwner->SetTargetRotToDir( vTargetDir, true );
		}
	}

	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	if ( m_fCurChargeMoveDist >= m_fMaxChargeMoveDist || CheckTargetMove( pOwner, m_fCurChargeMoveDist, m_fChargeMoveSpeed, vTargetDir ) )
	{
		if ( pOwner->IsNeedProcess() )
		{
			DecreaseGauge( m_fFlyChargeComboUseGauge );
			SetFlyComboAttackState( pOwner );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyComboAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_vFlyChargeComboAttList.empty() || m_nFlyMaxCombo <= 0 )
		return;

	pOwner->InitActionStopDelay( true );

	const AttackAttribute &pAttribute = m_vFlyChargeComboAttList[m_nFlyCurCombo];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	float fTimeRate  = pAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute.m_dwPreDelay;
	DWORD dwEndDelay = pAttribute.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFlyComboReserveTime = dwCurTime;
	m_dwFlyComboReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_dwFlyComboEndMotionEndTime = dwCurTime;
	m_dwFlyComboEndMotionEndTime += dwPreDelay;
	m_dwFlyComboEndMotionEndTime += dwEndDelay;
	m_dwFlyComboEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwCurChargeComboForceTime = dwCurTime; 
	m_dwCurChargeComboForceTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;

	if( pAttribute.m_AttackEndAnimation.IsEmpty() )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = pAttribute.m_fAttackEndAniRate;

		pGrp->SetActionAni( pAttribute.m_AttackEndAnimation, 0.0f, FLOAT100, FLOAT1, FLOAT1/fEndTimeRate, fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackEndAnimation );
		m_dwFlyComboEndMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	//pOwner->SetReservedSliding( pAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, pAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	
	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_NataFlySpecialState = NFSS_Combo_Attack;
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	m_fCurFlyEndDist = m_fFlyChargeComboReserveEndDist;
	m_fCurChargeComboForceAmt = 0.0f;
	m_fCurChargeComboForceFric = 0.0f;

	if ( !pAttribute.m_vForceInfoList.empty() )
	{
		m_fCurChargeComboForceAmt = pAttribute.m_vForceInfoList[0].m_fForceMovePower;
		m_fCurChargeComboForceFric = pAttribute.m_vForceInfoList[0].m_fForceMoveFriction;
	}

	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
			kPacket << NFSS_Combo_Attack;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			kPacket << m_nFlyCurCombo;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}

	m_nFlyCurCombo++;
}

void ioNataItem::ProcessFlyComboAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	/*
	if ( CheckEnableAttachTarget( pOwner ) && m_dwFlyComboReserveTime < dwCurTime )
		FlyReserveInputKey( pOwner, true, false, false );
	*/

	if ( m_dwFlyComboEndMotionEndTime < dwCurTime )
	{
		/*if ( CheckFlyReserveState( pOwner ) )
			return;
		*/

		if ( pOwner->IsNeedProcess() )
			CheckReserveState( pOwner );
			//SetFlyReturnMoveState( pOwner, false );
		return;
	}

	D3DXVECTOR3 vTargetDir = pOwner->GetWorldOrientation() * -ioMath::UNIT_Z;
	if ( !m_AimedTarget.IsEmpty() )
	{
		ioBaseChar* pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if ( pTarget )
		{
			vTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();					
			pOwner->SetTargetRotToDir( vTargetDir, true );
		}
	}

	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	if( m_dwCurChargeComboForceTime > 0 && m_dwCurChargeComboForceTime < dwCurTime )
	{
		m_dwCurChargeComboForceTime = 0;
		pOwner->SetForcePower( vTargetDir, m_fCurChargeComboForceAmt, m_fCurChargeComboForceFric );
	}

	if ( CheckFlyComboAttackInput( pOwner ) )
	{
		SetFlyComboAttackState( pOwner );
		return;
	}
}

bool ioNataItem::CheckFlyComboAttackInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFlyComboReserveTime < dwCurTime && pOwner->IsAttackKey() && m_nFlyCurCombo < m_nFlyMaxCombo )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyReturnMoveState( ioBaseChar *pOwner, bool bCheckDist )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( m_szFlyReturnMoveAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fFlyReturnMoveAniRate );	

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_NataFlySpecialState = NFSS_Return_Move;
	m_fCurReturnMoveDist = 0.0f;
	m_bSetReturnMoveCheck = bCheckDist;
	m_fCurFlyEndDist = m_fReturnMoveEndDist;

	if ( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Return_Move;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << bCheckDist;
		kPacket << m_bFlyAttachLeftDir;
		kPacket << m_bFlyAttachRightDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessFlyReturnMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( !CheckEnableAttachTarget( pOwner ) )
	{
		CheckReserveState( pOwner );
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vMoveDir = pOwner->GetWorldOrientation() * ioMath::UNIT_Z;
	if ( !m_AimedTarget.IsEmpty() )
	{
		ioBaseChar* pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if ( pTarget )
		{
			D3DXVECTOR3 vRotateDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();			
			pOwner->SetTargetRotToDir( vRotateDir, true );

			float fYawR = DEGtoRAD( m_fCurTargetAttachAngle );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vNewPos = pTarget->GetWorldPosition() + qtRotate * m_vTargetAttachOffset;
			float fMapHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z );
			if ( vNewPos.y < fMapHeight )
				vNewPos.y = fMapHeight;
			
			D3DXVECTOR3 vPos = vNewPos;
			if( pStage && pOwner->IsAxisLimit() )
				pStage->CheckAxisLimit( &vPos );

			vMoveDir = vPos - pOwner->GetWorldPosition();

			if ( m_bSetReturnMoveCheck && D3DXVec3LengthSq( &vMoveDir ) <= m_fReturnMoveCheckDist * m_fReturnMoveCheckDist )
			{
				if ( pOwner->IsNeedProcess() )
					SetFlyTargetAttachState( pOwner, false );
				return;
			}		
		}
	}

	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if ( m_fCurReturnMoveDist >= m_fMaxReturnMoveDist || CheckTargetMove( pOwner, m_fCurReturnMoveDist, m_fReturnMoveSpeed, vMoveDir ) )
	{
		if ( pOwner->IsNeedProcess() )
			SetFlyTargetAttachState( pOwner, true );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyNormalAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_FlyNormalAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_FlyNormalAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_FlyNormalAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_FlyNormalAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_FlyNormalAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFlyComboReserveTime = dwCurTime;
	m_dwFlyComboReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_dwFlyComboEndMotionEndTime = dwCurTime;
	m_dwFlyComboEndMotionEndTime += dwPreDelay;
	m_dwFlyComboEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_NataFlySpecialState = NFSS_Normal_Attack;
	m_fCurFlyEndDist = m_fFlyNormalAttackReserveEndDist;
	DecreaseGauge( m_fFlyNormalAttackUseGauge );

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Normal_Attack;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataItem::ProcessFlyNormalAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( CheckEnableAttachTarget( pOwner ) && m_dwFlyComboReserveTime < dwCurTime )
		FlyReserveInputKey( pOwner, true, true, true );

	if ( m_dwFlyComboEndMotionEndTime < dwCurTime )
	{
		if ( CheckFlyReserveState( pOwner ) )
			return;

		if ( pOwner->IsNeedProcess() )
			SetFlyReturnMoveState( pOwner, true );
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::SetFlyTargetChangeState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( FindChangeTarget( pOwner ) )
	{
		DecreaseGauge( m_fFlyChangeTargetUseGauge );
		SetFlyTargetAttackState( pOwner );
		return;
	}
}

bool ioNataItem::FindChangeTarget( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	//타겟 리스트
	BaseCharList vTargetList;

	//위치 설정
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	//타겟 리스트 얻기
	pOwner->GetTargetCharListByCylinder( vPos, m_fFlyChangeTargetColRange, m_fFlyChangeTargetColAngle, m_fFlyChangeTargetColUpHeight, m_fFlyChangeTargetColUnderHeight, vTargetList );

	if ( vTargetList.empty() )
		return false;

	//타겟 리스트 소팅
	if ( !SortChangeTargetList( pOwner, vTargetList ) )
		return false;

	return true;
}

bool ioNataItem::SortChangeTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList )
{
	if( !pOwner )	
		return false;
	
	SortList ChangeTargetList;
	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	
	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar != pOwner )
		{
			if ( pOwner->GetTeam() == pCurChar->GetTeam() )
				continue;

			if ( !IsChangeEnableTarget( pOwner, pCurChar ) )
				continue;

			vDiff = pCurChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );

			DistInfo sDist;
			sDist.szCharName = pCurChar->GetCharName();
			sDist.fDistSq = fCurDistSq;
			ChangeTargetList.push_back( sDist );
		}
	}

	if ( ChangeTargetList.empty() )
		return false;

	std::sort( ChangeTargetList.begin(), ChangeTargetList.end() );

	m_AimedTarget = ChangeTargetList[0].szCharName;
	pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_FLY;
		kPacket << NFSS_Find_Target;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	return true;
}

bool ioNataItem::IsChangeEnableTarget( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if( !pOwner || !pTarget )
		return false;
	
	if( pTarget->GetCharName() == m_AimedTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	CharState eTargetState = pTarget->GetState();
	switch( eTargetState )
	{
	case CS_BLOW_WOUNDED:
		{
			if ( m_bFlyChangeEnableTargetDown && !pTarget->IsFloatingState() )
				return true;

			return false;
		}
	case CS_BOUND_BLOW_WOUNDED:
		{
			if ( m_bFlyChangeEnableTargetDown && !pTarget->IsFloatingState() )
				return true;

			return false;
		}
	case CS_ETC_STATE:
		{
			if ( m_bFlyChangeEnableTargetDown && pTarget->IsApplyDownState(false) )
				return true;

			return false;
		}
	case CS_USING_SKILL:
		{
			if ( m_bFlyChangeEnableTargetDown && pTarget->IsApplyDownState(false) )
				return true;

			return false;
		}
	case CS_FROZEN:
		{
			if( m_bFlyChangeEnableTargetDown && pTarget->GetBlowFrozenState() )
				return true;

			return false;
		}
	case CS_STOP_MOTION:
		{
			if( m_bFlyChangeEnableTargetDown && pTarget->GetBlowStopMotionState() )
				return true;

			return false;
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataItem::FlyReserveInputKey( ioBaseChar *pOwner, bool bAttack, bool bDefence, bool bJump )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( bAttack && pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( bDefence && pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( bJump && pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

bool ioNataItem::CheckFlyReserveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( m_bAttackKeyReserved )
	{
		SetFlyReserveAttack( pOwner );
		return true;
	}
	else if( m_bDefenseKeyReserved )
	{
		SetFlyReserveDefence( pOwner );
		return true;
	}
	else if( m_bJumpKeyReserved )
	{
		SetFlyReserveJump( pOwner );
		return true;
	}

	return false;
}

void ioNataItem::SetFlyReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	SetFlyCheckAttackState( pOwner );
	m_bAttackKeyReserved = false;
}

void ioNataItem::SetFlyReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	SetFlyTargetChangeState( pOwner );
	m_bDefenseKeyReserved = false;
}

void ioNataItem::SetFlyReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS_COMBO;
		kPacket << NCSS_End;
		kPacket << m_fCurEndJumpPower;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetSKillEndJumpState( m_fCurEndJumpPower, false, false, true, true );
	m_bJumpKeyReserved = false;	
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioNataComboSpecialState::ioNataComboSpecialState()
{
}

ioNataComboSpecialState::~ioNataComboSpecialState()
{
}

void ioNataComboSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNataComboSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioNataItem *pNataItem = ToNataItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNataItem )
		pNataItem->ClearState( pOwner );
}

void ioNataComboSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNataItem *pNataItem = ToNataItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNataItem )
		pNataItem->ProcessNataComboSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNataComboSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioNataFlySpecialState::ioNataFlySpecialState()
{
}

ioNataFlySpecialState::~ioNataFlySpecialState()
{
}

void ioNataFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNataFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioNataItem *pNataItem = ToNataItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNataItem )
		pNataItem->ClearState( pOwner );
}

bool ioNataFlySpecialState::IsSendControlInfoToUser( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return false;

	ioNataItem *pNataItem = ToNataItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNataItem )
		return pNataItem->IsFlySendControlInfoToUser();

	return true; 
}

bool ioNataFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioNataItem *pNataItem = ToNataItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pNataItem && pNataItem->IsFlyCollisionSkipState( ) )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioNataFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNataItem *pNataItem = ToNataItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pNataItem )
		pNataItem->ProcessNataFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNataFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioNataFlySpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	fCurGravity = 0.0f;
}