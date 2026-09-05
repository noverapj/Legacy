
#include "stdafx.h"

#include "ioSuperDimensionItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioSuperDimensionItem::ioSuperDimensionItem()
{	
	m_fCurGauge = 0.0f;
	Init();
}

ioSuperDimensionItem::ioSuperDimensionItem( const ioSuperDimensionItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),  
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_vChargeComboAttListCenter( rhs.m_vChargeComboAttListCenter ),
	m_fColAngle( rhs. m_fColAngle ),
	m_fColRange( rhs.m_fColRange ),
	m_dwSPWaitDuration( rhs.m_dwSPWaitDuration ),
	m_nMaxComboCnt( rhs.m_nMaxComboCnt ),
	m_SuperPowerAttack( rhs.m_SuperPowerAttack ),
	m_stSuperPowerAttackCenter( rhs.m_stSuperPowerAttackCenter ),
	m_stSuperPowerAttackUp( rhs.m_stSuperPowerAttackUp ),
	m_stSuperPowerAttackDown( rhs.m_stSuperPowerAttackDown ),
	m_stSuperPowerAttackCenter_Jump( rhs.m_stSuperPowerAttackCenter_Jump ),
	m_stSuperPowerAttackUp_Jump( rhs.m_stSuperPowerAttackUp_Jump ),
	m_stSuperPowerAttackDown_Jump( rhs.m_stSuperPowerAttackDown_Jump ),
	m_bSetDownAim( rhs.m_bSetDownAim ),
	m_stWeaponEffec1( rhs.m_stWeaponEffec1 ),
	m_stWeaponEffec2( rhs.m_stWeaponEffec2 ),
	m_stNormalMeshName( rhs.m_stNormalMeshName ),
	m_stExtraMeshName( rhs.m_stExtraMeshName ),
	m_vHpScaleInfo( rhs.m_vHpScaleInfo ),
	m_SpecialDashInfo( rhs.m_SpecialDashInfo ),
	m_fSpecialFireEndJumpPower( rhs.m_fSpecialFireEndJumpPower )
{
	m_fCurGauge = 0.0f;
	Init();

	m_dwEffect1 = 0;
	m_dwEffect2 = 0;

	m_nHpPercentStep = 0;
	m_nNowPercentStep = 0;
	m_nChangedNowStep = -1;
	m_bLastIncWeapon = false;
	m_bItemVisible =  true;

	m_SuperDimensionDashState = MFS_NONE;
}

ioSuperDimensionItem::~ioSuperDimensionItem()
{
}

void ioSuperDimensionItem::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	ioWeaponItem::LoadProperty( rkLoader );

	LoadAttackProperty( rkLoader );

	//게이지 설정
	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );

	//타겟 설정
	m_fColAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fColRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	m_bSetDownAim = rkLoader.LoadBool_e( "set_down_target", false );

	//초능력 대기 시간
	m_dwSPWaitDuration = rkLoader.LoadInt_e( "super_attack_duration", 0 );

	rkLoader.LoadString_e( "extra_weapon_effect1", "", szBuf, MAX_PATH );
	m_stWeaponEffec1 = szBuf;
	rkLoader.LoadString_e( "extra_weapon_effect2", "", szBuf, MAX_PATH );
	m_stWeaponEffec2 = szBuf;

	rkLoader.LoadString_e( "normal_attack_mesh", "", szBuf, MAX_PATH );
	m_stNormalMeshName = szBuf;
	rkLoader.LoadString_e( "extra_attack_mesh", "", szBuf, MAX_PATH );
	m_stExtraMeshName = szBuf;

	int nMaxHpInfoCnt = rkLoader.LoadInt_e( "hp_percent_max_cnt", 0 );
	for( int i = 0; i < nMaxHpInfoCnt; i++ )
	{
		SuperDimensionHpScaleInfo info;

		wsprintf( szBuf, "hp_percent%d", i+1 );
		info.m_fHpPercent = rkLoader.LoadFloat( szBuf, 0.f );
		wsprintf( szBuf, "hp_percent%d_weapon_scale", i+1 );
		info.m_fScale = rkLoader.LoadFloat( szBuf, FLOAT1 );
		wsprintf( szBuf, "hp_percent%d_contactweapon_scale", i+1 );
		info.m_fWeaponScale = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vHpScaleInfo.push_back( info );
	}

	rkLoader.LoadString_e( "superdimension_dash_start_ani", "", szBuf, MAX_PATH );
	m_SpecialDashInfo.m_StartAni = szBuf;
	m_SpecialDashInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "superdimension_dash_start_ani_rate", FLOAT1 );

	//대쉬공격
	LoadAttackAttribute_e( "superdimension_dash_attack_move", m_SpecialDashInfo.m_AttackMove, rkLoader );

	//대쉬마무리 공격
	LoadAttackAttribute_e( "superdimension_dash_end_attack", m_SpecialDashInfo.m_EndAttack, rkLoader );

	m_SpecialDashInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "superdimension_dash_target_wound_type", TWT_ALL );
	m_SpecialDashInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "superdimension_dash_target_angle", 0.0f );
	m_SpecialDashInfo.m_fTargetRange = rkLoader.LoadFloat_e( "superdimension_dash_target_range", 0.0f );
	m_SpecialDashInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "superdimension_dash_target_up_height", 0.0f );
	m_SpecialDashInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "superdimension_dash_target_down_height", 0.0f );

	m_SpecialDashInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "superdimension_dash_attack_move_speed", FLOAT1 );
	m_SpecialDashInfo.m_fAttackMoveRange = rkLoader.LoadFloat_e( "superdimension_dash_attack_move_range", FLOAT1 );

	rkLoader.LoadString_e( "superdimension_dash_grap_ani", "", szBuf, MAX_PATH );
	m_SpecialDashInfo.m_GrapplingAttackAni = szBuf;
	m_SpecialDashInfo.m_fGrapplingAttackAniRate = rkLoader.LoadFloat_e( "superdimension_dash_grap_ani_rate", FLOAT1 );
	// grappling
	m_SpecialDashInfo.m_GrapplingInfo.Init();
	m_SpecialDashInfo.m_GrapplingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "superdimension_dash_target_reverse_rotate", false );
	m_SpecialDashInfo.m_GrapplingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "superdimension_dash_wound_grappling_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "superdimension_dash_grap_wound_ani", "", szBuf, MAX_PATH );
	m_SpecialDashInfo.m_GrapplingInfo.m_GrapplingLoopMotion = szBuf;
	m_SpecialDashInfo.m_GrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_SpecialDashInfo.m_GrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "superdimension_dash_grap_wound_ani_rate", FLOAT1 );

	m_SpecialDashInfo.m_fGrapplingHeightRate = rkLoader.LoadFloat_e( "superdimension_dash_grappling_height_rate", 0.f );
	m_SpecialDashInfo.m_fGrapplingRange = rkLoader.LoadFloat_e( "superdimension_dash_grappling_gap_range", 0.f );

	rkLoader.LoadString_e( "superdimension_dash_loop_motion_ani", "", szBuf, MAX_PATH );
	m_SpecialDashInfo.m_GrapplingLoopAni = szBuf;
	m_SpecialDashInfo.m_fGrapplingLoopAniRate = rkLoader.LoadFloat_e( "superdimension_dash_loop_motion_ani_rate", FLOAT1 );
	m_SpecialDashInfo.m_dwGrapplingLoopRotateSpeed = (DWORD)rkLoader.LoadInt_e( "superdimension_dash_loop_rotate_speed", 0 );
	m_SpecialDashInfo.m_fGrapplingLoopHeightRate = rkLoader.LoadFloat_e( "superdimension_dash_loop_height_rate", 0.f );
	m_SpecialDashInfo.m_fGrapplingLoopGapRange = rkLoader.LoadFloat_e( "superdimension_dash_loop_gap_range", 0.f );
	m_SpecialDashInfo.m_dwGrapplingLoopMaxTime = (DWORD)rkLoader.LoadInt_e( "superdimension_dash_loop_max_time", 0 );

	rkLoader.LoadString_e( "superdimension_dash_wound_loop_motion_ani", "", szBuf, MAX_PATH );
	m_SpecialDashInfo.m_GrapplingWoundLoopAni = szBuf;
	m_SpecialDashInfo.m_fGrapplingLoopWoundAniRate = rkLoader.LoadFloat_e( "superdimension_dash_wound_loop_motion_ani_rate", FLOAT1 );

	m_SpecialDashInfo.m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "superdimension_dash_attack_end_jump_amt", 0.f );
}

ioItem* ioSuperDimensionItem::Clone()
{
	return new ioSuperDimensionItem( *this );
}

ioWeaponItem::WeaponSubType ioSuperDimensionItem::GetSubType() const
{
	return WST_SUPER_DIMENSION_ITEM;
}

/////////////////////기본 공격
//공격 설정
void ioSuperDimensionItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	Init();
	m_dwKeyReserveTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = m_AttackReadyAniList.size();
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

void ioSuperDimensionItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_WeaponItemState )
	{
	case WS_CHECK_CHARGING:
		{
			CheckCharging( pOwner );
		}
		break;
	/*case WS_CHARGING_ATTACK:
		if ( m_dwCheckWaitTime + m_dwSPWaitDuration < dwCurTime )
		{
			pOwner->SetState( CS_DELAY, false );
			return;
		}

		break;*/
	case WS_NORMAL_ATTACK:
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

//무기 적용
void ioSuperDimensionItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );	

	ioTraceWeapon5 *pTrace = ToTraceWeapon5( pWeapon );
	if( pTrace )
	{
		//초능력탄이면
		if ( !m_bSetTarget )
		{			
			pTrace->SetTraceTarget( m_szFirstTarget );
			pTrace->SetTraceState( NULL );	
		}
		else if ( m_bSetTarget )
		{
			pTrace->SetTraceTarget( m_szTargetName );
			pTrace->SetTraceState( NULL );
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
			pZone->SetZoneValue( true, false );
		}
	}

	ioContactWeapon3 *pContact3 = ToContactWeapon3( pWeapon );
	if( pContact3 )
	{
		if( m_nChangedNowStep >= 0 && m_nChangedNowStep < (int)m_vHpScaleInfo.size() )
		{
			const SuperDimensionHpScaleInfo& NowInfo = m_vHpScaleInfo[m_nChangedNowStep];
			pContact3->SetWeaponScaleRate( NowInfo.m_fWeaponScale );
		}
	}

	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
	if( pSimple )
	{
		pSimple->SetOwnerAttach( true );
		pSimple->SetCheckDeadTime( true );
		//pSimple->SetMoveDir( m_vAttackFireDir );		
	}
}

//동기화
void ioSuperDimensionItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case WS_SPECIAL_FIRE_STATE_INIT:
		{
			bool bJump;
			ioHashString stTarget;
			rkPacket >> bJump;
			rkPacket >> m_szFirstTarget;

			SetSuperDimensionFireState( pOwner, bJump );
		}
		break;
	case WS_SPECIAL_DASH_GRAP:
		{
			D3DXQUATERNION qtRot;
			ioHashString stTargetName;
			rkPacket >> qtRot;
			rkPacket >> stTargetName;

			pOwner->SetTargetRotToRotate( qtRot, true );

			//SetSuperDimensionDashGrapState( pOwner, stTargetName );
			SetItemGrapplingState( pOwner, stTargetName );
		}
		break;
	case WS_SPECIAL_DASH_LAST_ATTACK:
		{
			D3DXQUATERNION qtRot;
			ioHashString stTargetName;
			D3DXVECTOR3 vDir;

			rkPacket >> qtRot;
			rkPacket >> stTargetName;
			rkPacket >> vDir;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ioBaseChar* pTarget = pOwner->GetBaseChar( stTargetName );
			ChangeSpecialDashLoopAttackState( pOwner, pTarget, vDir );
		}
		break;
	}
}

bool ioSuperDimensionItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_SuperDimensionFireState )
		{
		case PSS_Find_Target:
		//case PSS_Wait:
		//case PSS_Combo:
		//case PSS_Teleport:
			return true;
		}
	}

	return false;
}

///////////////////////게이지 관련///////////////////////////////////////////////
//데이터 갱신 (게이지 회복)
void ioSuperDimensionItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if ( pOwner->GetState() == CS_JUMP && pOwner->GetJumpState() == JS_JUMP_ATTACK )
	{
		return;
	}

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
	}
}

//게이지 설정 및 데이터 얻기
int ioSuperDimensionItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioSuperDimensionItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioSuperDimensionItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioSuperDimensionItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioSuperDimensionItem::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioSuperDimensionItem::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioSuperDimensionItem::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioSuperDimensionItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioSuperDimensionItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

//게이지 사용
void ioSuperDimensionItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurGauge -= fNeedGauge;
	m_fCurGauge = (float)max( 0.f, min( m_fCurGauge, m_fMaxGauge ) );
}

/////////////////////////////////////////////////////////////////////////////////
//추가 읽기
void ioSuperDimensionItem::LoadAttackProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	//준비 동작
	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );	
	m_AttackReadyAniList.clear();

	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//////////////////////////////////////////////////////////////////////////
	//초능력탄 공격
	LoadAttackAttribute_e( "super_power_attack", m_SuperPowerAttack, rkLoader );
	rkLoader.LoadString( "super_power_attack_center", "", szBuf, MAX_PATH );
	m_stSuperPowerAttackCenter = szBuf;
	rkLoader.LoadString( "super_power_attack_up", "", szBuf, MAX_PATH );
	m_stSuperPowerAttackUp = szBuf;
	rkLoader.LoadString( "super_power_attack_down", "", szBuf, MAX_PATH );
	m_stSuperPowerAttackDown = szBuf;

	rkLoader.LoadString( "super_power_attack_center_jump", "", szBuf, MAX_PATH );
	m_stSuperPowerAttackCenter_Jump = szBuf;
	rkLoader.LoadString( "super_power_attack_up_jump", "", szBuf, MAX_PATH );
	m_stSuperPowerAttackUp_Jump = szBuf;
	rkLoader.LoadString( "super_power_attack_down_jump", "", szBuf, MAX_PATH );
	m_stSuperPowerAttackDown_Jump = szBuf;

	m_fSpecialFireEndJumpPower = rkLoader.LoadFloat_e( "super_attack_end_jump_amt", 0.f );
	//////////////////////////////////////////////////////////////////////////

	//차지 콤보
	int iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt", 1 );
	m_nMaxComboCnt = iComboMax;

	m_vChargeComboAttListCenter.clear();
	m_vChargeComboAttListCenter.reserve( iComboMax );

	for( int i=0; i < iComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttListCenter.push_back( kAttribute );
	}
}

//동기화
void ioSuperDimensionItem::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	/*case PSS_Find_Target:
		{	
			rkPacket >> m_dwWeaponBaseIndex;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			rkPacket >> m_szFirstTarget;

			pOwner->SetTargetRot( qtRot );
			SetSuperPowerState( pOwner );
		}
		break;*/
	case PSS_Set_Target:
		{
			rkPacket >> m_szTargetName;
			rkPacket >> m_bSetTarget;
		}
		break;
	case PSS_End:
		{
			m_SuperDimensionFireState = PSS_End;
		}
		break;
	}
}

//초기화
void ioSuperDimensionItem::Init()
{
	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.clear();

	m_dwCheckWaitTime = 0;
	m_dwCheckInputWaitTime = 0;

	m_dwComboReserveTime = 0;
	m_dwComboEndTime = 0;

	m_dwTeleportAttackEndTime = 0;
	m_dwTeleportReserveTime = 0;

	m_nNormalCombo = 0;
	m_nCurComboCnt = 0;

	m_WeaponItemState = WS_NONE;
	m_SuperDimensionFireState = PSS_None;

	m_bSetTarget = false;
	m_szTargetName.Clear();
	m_szFirstTarget.Clear();

	m_vTargetDir = ioMath::VEC3_ZERO;
	m_vTargetDist = ioMath::VEC3_ZERO;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_dwKeyReserveTime = 0;
}

//차징 확인
void ioSuperDimensionItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess()  )	
		return;

	//게이지 확인
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && GetCurBullet() >= GetNeedBullet() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			//특별 상태로 변경
			SetSuperDimensionFireState( pOwner );
			//SetSuperPowerState( pOwner );
		}
	}
	else
	{
		//일반 공격
		SetNormalAttackState( pOwner );
	}
}

//기본 공격 상태로 변경
void ioSuperDimensionItem::SetNormalAttackState( ioBaseChar *pOwner )
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

/////////////////////////////////////////////////////////////////////////////////
void ioSuperDimensionItem::ClearState( ioBaseChar *pOwner )
{
	Init();
}


//타겟 설정(맞은 쪽에서 공격자 정보에 대해 패킷 전송)
//void ioSuperDimensionItem::SetTarget( ioBaseChar *pTarget, ioBaseChar *pAttacker )
//{
//	if ( pTarget && pAttacker )
//	{
//		m_szTargetName = pTarget->GetCharName();
//		m_bSetTarget = true;
//
//		if( pTarget->IsNeedProcess() && pTarget->IsNeedSendNetwork() )
//		{
//			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
//			FillItemGaugePacketHeader( pAttacker, kPacket );
//			kPacket << WS_SPECIAL_STATE_PROCESS;
//			kPacket << PSS_Set_Target;
//			kPacket << m_szTargetName;
//			kPacket << m_bSetTarget;
//			P2PNetwork::SendToAllPlayingUser( kPacket );
//		}
//	}
//}


bool ioSuperDimensionItem::TargetCheck( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szFirstTarget );
	if( !pTarget )
		return false;

	if( pTarget->GetTeam() == pOwner->GetTeam() ) 
		return false;
	if( pTarget->GetCharName() == GetOwnerName() )
		return false;
	if( pTarget->GetExperienceMode() != EMS_NONE )
		return false;

	if( !IsEnableTargetState( pTarget ) )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fColAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fColRange * m_fColRange;

	float fCurGap = -FLOAT1;

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
	

	if( fDiffRangeSq > fTargetRangeSq )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//가장 가까운 타겟 검사
bool ioSuperDimensionItem::FindTarget( ioBaseChar *pOwner )
{
	m_szFirstTarget.Clear();

	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return false;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fColAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fColRange * m_fColRange;

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
		/*float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurFly.m_fTargetDownHeight )
			continue;*/

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
			m_szFirstTarget = szTarget;
			return true;
		}
	}

	return false;

	//m_szFirstTarget.Clear();

	//if( !pOwner )//|| !pOwner->IsNeedProcess() ) 
	//	return false;

	//ioPlayStage *pStage = pOwner->GetCreator();
	//if ( !pStage )
	//	return false;

	////타겟 리스트
	//BaseCharList vTargetList;

	////위치 설정
	//D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	////타겟 리스트 얻기
	//pOwner->GetTargetCharListBySphere( vPos, m_fColRange, m_fColAngle, vTargetList );

	////타겟 리스트가 비어있다면 특별 종료
	//if ( vTargetList.empty() )
	//	return false;

	////타겟 리스트 소팅
	//SortTargetList( pOwner, vTargetList );

	////회전
	//if ( !m_SortList.empty() )
	//{
	//	ioBaseChar *pTarget = m_SortList[0].pChar;
	//	if ( pTarget )
	//	{
	//		m_szFirstTarget = pTarget->GetCharName();

	//		D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
	//		D3DXVec3Normalize( &vDir, &vDir );
	//		pOwner->SetTargetRotToDir( vDir, true );
	//		return true;
	//	}
	//}

	//return false;
}

//타겟 리스트 가까운 순서로 정렬
void ioSuperDimensionItem::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList )
{
	if( !pOwner )	
		return;

	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.reserve( TargetList.size() );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();

	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar != pOwner )
		{
			//팀이 같거나 타겟이 될수 없으면 리스트에 저장 안함
			if ( pOwner->GetTeam() == pCurChar->GetTeam() )
				continue;

			if ( !IsEnableTarget( pCurChar ) )
				continue;

			vDiff = pCurChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );

			DistInfo sDist;
			sDist.pChar = pCurChar;
			sDist.fDistSq = fCurDistSq;
			m_SortList.push_back( sDist );
		}
	}

	std::sort( m_SortList.begin(), m_SortList.end() );
}

//타겟 가능한지 확인
bool ioSuperDimensionItem::IsEnableTarget( ioBaseChar *pTarget )
{
	if ( !pTarget )
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
			if ( !pTarget->IsFloatingState() && !m_bSetDownAim )
				return false;
		}
	case CS_BOUND_BLOW_WOUNDED:
		{
			if ( !pTarget->IsFloatingState() && !m_bSetDownAim )
				return false;
		}
	case CS_ETC_STATE:
		{
			if ( pTarget->IsApplyDownState(false) && !m_bSetDownAim )
				return false;
		}
	case CS_USING_SKILL:
		{
			if ( pTarget->IsApplyDownState(false) && !m_bSetDownAim )
				return false;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//초능력탄 애니메이션 설정
void ioSuperDimensionItem::SetSuperPowerState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	//초능력 공격 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = -1;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_bSpecialFireFromJump )
	{
		iAniID = pGrp->GetAnimationIdx( m_stSuperPowerAttackCenter_Jump );
		if( iAniID == -1 )	
			return;

		pGrp->SetActionAni( m_stSuperPowerAttackUp_Jump, 0.f, 0.f, 0.f );
		pGrp->SetActionAni( m_stSuperPowerAttackDown_Jump, 0.f, 0.f, 0.f );
		pGrp->SetActionAni( m_stSuperPowerAttackCenter_Jump, 0.f, 0.f, FLOAT1 );
	}
	else
	{
		iAniID = pGrp->GetAnimationIdx( m_stSuperPowerAttackCenter );
		if( iAniID == -1 )	
			return;

		pGrp->SetActionAni( m_stSuperPowerAttackUp, 0.f, 0.f, 0.f );
		pGrp->SetActionAni( m_stSuperPowerAttackDown, 0.f, 0.f, 0.f );
		pGrp->SetActionAni( m_stSuperPowerAttackCenter, 0.f, 0.f, FLOAT1 );
	}
	

	float fTimeRate = m_SuperPowerAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_SuperPowerAttack.m_dwPreDelay;

	pOwner->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_SuperPowerAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );	

	//슬라이딩 설정
	//pOwner->SetReservedSliding( m_SuperPowerAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckWaitTime = dwCurTime;

	m_SuperDimensionFireState = PSS_Wait;
	m_dwKeyReserveTime = 0;
	float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserve > 0.0f )
		m_dwKeyReserveTime = dwPreDelay + dwCurTime + fReserve;

	//m_PsychoSpecialState = PSS_End;

	//m_WeaponItemState = WS_CHARGING_ATTACK;

	/*if( pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << PSS_Find_Target;
		kPacket << m_dwWeaponBaseIndex;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_szFirstTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}


//타겟으로 회전
void ioSuperDimensionItem::SetRotateToTarget( ioBaseChar *pOwner )
{
	if ( !pOwner || m_szTargetName.IsEmpty() )
		return;

	//회전 타겟 얻기
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( pTarget )
	{
		//현재 위치 얻기
		D3DXVECTOR3 vOnwerPos = pOwner->GetWorldPosition();
		//타겟 위치 얻기
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		//높이 갱신		
		vOnwerPos.y = vTargetPos.y;

		//타겟 방향 설정
		m_vTargetDir = vTargetPos - vOnwerPos;

		m_vTargetDist = m_vTargetDir;

		//캐릭터 회전 및 방향 설정
		D3DXVec3Normalize( &m_vTargetDir, &m_vTargetDir );		
		pOwner->SetTargetRotToDir( m_vTargetDir, true );
	}
}

//키입력이 가능한가
//bool ioSuperDimensionItem::IsEnableInputKey( ioBaseChar *pOwner )
//{
//	if ( !pOwner || m_szTargetName.IsEmpty() || !m_bSetTarget )
//		return false;
//
//	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
//	if ( pTarget )
//	{
//		return !pTarget->HasHideBuff();
//	}
//
//	return false;
//}
//
//
//
////예약상태
//void ioSuperDimensionItem::ReserveInputKey(ioBaseChar *pOwner )
//{
//	if ( !pOwner || !pOwner->IsNeedProcess() )
//		return;
//
//	if( pOwner->IsAttackKey() )
//		m_bAttackKeyReserved = true;
//
//	if( pOwner->IsDefenseKey() )
//		m_bDefenseKeyReserved = true;
//
//	if( pOwner->IsJumpKey() )
//		m_bJumpKeyReserved = true;
//}

//void ioSuperDimensionItem::CheckReserveState( ioBaseChar *pOwner )
//{
//	if ( !pOwner && !pOwner->IsNeedProcess() )
//		return;
//
//	if( m_bAttackKeyReserved )
//	{
//		SetReserveAttack( pOwner );
//		return;
//	}
//	else if( m_bDefenseKeyReserved && pOwner->IsDefenseKeyDown() )
//	{
//		SetReserveDefence( pOwner );
//		return;
//	}
//	else if( m_bJumpKeyReserved )
//	{
//		SetReserveJump( pOwner );
//		return;
//	}
//
//	m_SuperDimensionFireState = PSS_End;
//
//	if( pOwner->IsNeedSendNetwork() )
//	{
//		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
//		FillItemGaugePacketHeader( pOwner, kPacket );
//		kPacket << WS_SPECIAL_STATE_PROCESS;
//		kPacket << PSS_End;
//		P2PNetwork::SendToAllPlayingUser( kPacket );
//	}	
//}

//void ioSuperDimensionItem::SetReserveAttack( ioBaseChar *pOwner )
//{
//	if ( !pOwner || !pOwner->IsNeedProcess() )
//		return;
//
//	//if ( !IsFloatState(pOwner) )
//	{
//		pOwner->SetNormalAttack(0);
//	}
//	m_bAttackKeyReserved = false;
//}
//
//void ioSuperDimensionItem::SetReserveDefence( ioBaseChar *pOwner )
//{
//	if ( !pOwner || !pOwner->IsNeedProcess() )
//		return;
//
//	pOwner->SetDefenseState();
//	m_bDefenseKeyReserved = false;
//}
//
//void ioSuperDimensionItem::SetReserveJump( ioBaseChar *pOwner )
//{
//	if ( !pOwner || !pOwner->IsNeedProcess() )
//		return;
//
//	pOwner->SetJumpState();
//	m_bJumpKeyReserved = false;	
//}


void ioSuperDimensionItem::ProcessSuperDimensionDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SuperDimensionDashState )
	{
	case MFS_START:
		if( dwCurTime > m_dwDashStartEndTime )
		{
			//FindDashAttTarget( pOwner );
			ChangeSpecialDashMoveState( pOwner );
		}
		break;
	case MFS_ATTACK_MOVE:
		ProcessSpecailDashAttackMove( pOwner );
		if( m_fDashAttackMoveRange > m_SpecialDashInfo.m_fAttackMoveRange )
		{
			pOwner->SetCurMoveSpeed( 0.0f );
			pOwner->SetExtendAttackEndJump( m_SpecialDashInfo.m_fFlyEndJumpAmt, m_fExtendLandingRate, false, false );
		}
		break;
	/*case MFS_GRAPPLING_MOVE:
		ProcessSpecailDashAttackMove( pOwner );
		if( m_fDashAttackMoveRange > m_SpecialDashInfo.m_fAttackMoveRange )
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
				ChangeSpecialDashLoopState( pOwner, pTarget );
			else
				pOwner->SetExtendAttackEndJump( m_SpecialDashInfo.m_fFlyEndJumpAmt, m_fExtendLandingRate );
		}
		break;*/
	case MFS_GRAPPLING_LOOP:
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
			if( !pTarget || pTarget->GetState() != CS_GRAPPLING_WOUNDED_ITEM )
			{
				pOwner->SetExtendAttackEndJump( m_SpecialDashInfo.m_fFlyEndJumpAmt, m_fExtendLandingRate );
				return;
			}

			ProcessSpecailDashLoopState( pOwner, pTarget );

			if( pOwner->IsNeedProcess() )
			{
				if( m_dwDashLoopEndTime < dwCurTime || pOwner->IsAttackKey() )
				{
					D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					/*ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
					if( eNewDirKey != ioUserKeyInput::DKI_NONE )
					{
					float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
					fYawD = ioMath::ArrangeHead( fYawD );

					float fYawR = DEGtoRAD( fYawD );
					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

					vDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					D3DXVec3Normalize( &vDir,&vDir );
					}*/
					ChangeSpecialDashLoopAttackState( pOwner, pTarget, vDir );
				}
			}
		}
		break;
	case MFS_END_ATTACK:
		if( m_dwDashEndAttackTime < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				pOwner->SetSKillEndJumpState( m_SpecialDashInfo.m_fFlyEndJumpAmt, false, false, true, true );
				//pOwner->SetExtendAttackEndJump( m_SpecialDashInfo.m_fFlyEndJumpAmt, m_fExtendLandingRate, false, false );
			}
		}
		break;
	}

}



void ioSuperDimensionItem::ChangeSpecialDashLoopAttackState( ioBaseChar *pOwner, ioBaseChar *pTarget, const D3DXVECTOR3& vDir )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( !pTarget )
		return;

	if( pTarget->GetState() != CS_GRAPPLING_WOUNDED_ITEM )
		return;

	ClearCancelInfo();

	pOwner->SetTargetRotToDir( vDir, true );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	m_SuperDimensionDashState = MFS_END_ATTACK;
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_SpecialDashInfo.m_EndAttack.m_AttackAnimation );
	float fTimeRate = m_SpecialDashInfo.m_EndAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	m_dwDashEndAttackTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_SpecialDashInfo.m_EndAttack.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_SpecialDashInfo.m_EndAttack.m_vWeaponInfoList,
		FTT_EXTEND_ATTACK,
		fTimeRate,
		dwPreDelay );

	pTarget->ProcessItemWoundGrapplingLoop( pOwner );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_DASH_LAST_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pTarget->GetCharName();
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioSuperDimensionItem::ChangeSpecialDashLoopState( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SuperDimensionDashState = MFS_GRAPPLING_LOOP;
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_SpecialDashInfo.m_GrapplingLoopAni );
	float fTimeRate = m_SpecialDashInfo.m_fGrapplingLoopAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fTimeRate );

	m_dwDashLoopEndTime = dwCurTime + m_SpecialDashInfo.m_dwGrapplingLoopMaxTime;


	//타겟
	pGrp = pTarget->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	iAniID = pGrp->GetAnimationIdx( m_SpecialDashInfo.m_GrapplingWoundLoopAni );
	fTimeRate = m_SpecialDashInfo.m_fGrapplingLoopWoundAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fTimeRate );
}


//void ioSuperDimensionItem::ChangeSpecialDashGrapMoveState( ioBaseChar *pOwner, ioBaseChar *pTarget )
//{
//	if( !pOwner )
//		return;
//
//	ioEntityGroup *pGrp = pOwner->GetGroup();
//	if( !pGrp )
//		return;
//
//	m_SuperDimensionDashState = MFS_GRAPPLING_MOVE;
//	pGrp->ClearAllLoopAni( FLOAT10, true );
//	pGrp->ClearAllActionAni( FLOAT10, true );
//
//	int iAniID = pGrp->GetAnimationIdx( m_SpecialDashInfo.m_GrapplingAttackAni );
//	float fTimeRate = m_SpecialDashInfo.m_fGrapplingAttackAniRate;
//	if( fTimeRate <= 0.0f )
//		fTimeRate = FLOAT1;
//
//	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fTimeRate );
//}


void ioSuperDimensionItem::ChangeSpecialDashMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_GrapplingTargetName.Clear();

	m_SuperDimensionDashState = MFS_ATTACK_MOVE;
	m_fDashAttackMoveRange = 0.f;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	DWORD dwPreDelay = m_SpecialDashInfo.m_AttackMove.m_dwPreDelay;

	int iAniID = pGrp->GetAnimationIdx( m_SpecialDashInfo.m_AttackMove.m_AttackAnimation );
	float fTimeRate = m_SpecialDashInfo.m_AttackMove.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_SpecialDashInfo.m_AttackMove.m_vWeaponInfoList,
		FTT_EXTEND_ATTACK,
		fTimeRate,
		dwPreDelay );

	DWORD dwMoveDuration = 0;
	//if( pOwner->IsNeedProcess() )
	{
		bool bTargetOk = false;
		D3DXVECTOR3 vTargetPos;
		D3DXVECTOR3 vDir;

		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
		if( pTarget )			
		{
			bTargetOk = true;
			vDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		}

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			m_vDashMoveDir = vDir;
		}
		else
		{
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			m_vDashMoveDir = vDir;
		}
	}
}



void ioSuperDimensionItem::ProcessSpecailDashLoopState( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if( !pTarget )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_SpecialDashInfo.m_dwGrapplingLoopRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	D3DXQUATERNION qtCharRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true, false );

	vDir = vDir * -FLOAT1;
	ioMath::CalcDirectionQuaternion( qtCharRot, vDir );
	pTarget->SetTargetRot( qtCharRot );
}


void ioSuperDimensionItem::ProcessSpecailDashAttackMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_SpecialDashInfo.m_fAttackMoveSpeed > 0.0f && m_vDashMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_SpecialDashInfo.m_fAttackMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vDashMoveDir * fFinalMoveAmt;
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
	}

	float fDistance = m_SpecialDashInfo.m_fAttackMoveSpeed * fTimePerSec;
	m_fDashAttackMoveRange += fDistance;


	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pStage && pOwner->IsAxisLimit() )
	{
		float fBottomHeight = pOwner->GetBottomHeight();
		vPos.y = max( fBottomHeight, vPos.y );
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}



void ioSuperDimensionItem::ProcessSuperDimensionFireState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SuperDimensionFireState )
	{
	case PSS_Find_Target:
		{
			/*if ( !FindTarget( pOwner ) )
			m_szFirstTarget.Clear();*/

			m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
			//DecreaseGauge( m_fNeedGauge );
			SetSuperPowerState( pOwner );

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			pOwner->InitExtendAttackTagInfo();

			pOwner->ClearReservedSliding();
			pOwner->SetForcePowerAmt( 0.0f );
		}
		break;
	case PSS_Wait:
		{			
			if( !TargetCheck( pOwner ) )
				m_szFirstTarget.Clear();

			if( pOwner->IsNeedProcess() && m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			{
				CheckReserve( pOwner );
			}

			//지정 시간 후 종료
			if ( m_dwCheckWaitTime + m_dwSPWaitDuration < dwCurTime )
			{
				m_SuperDimensionFireState = PSS_End;
				//if( pOwner->IsNeedProcess() )
				{
					pOwner->SetCurJumpPowerAmt( 0.0f );
					pOwner->SetGravityAmt( 0.0f );

					pOwner->InitExtendAttackTagInfo();

					pOwner->ClearReservedSliding();
					pOwner->SetForcePowerAmt( 0.0f );

					if( pOwner->IsNeedProcess() )
					{
						if( m_bSpecialFireFromJump )
						{
							//if( pOwner->IsNeedProcess() )
							{
								pOwner->SetSKillEndJumpState( m_fSpecialFireEndJumpPower, false, false, true, true );
							}
						}
						else
						{
							// 점프
							if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
							{
								Init();
								pOwner->SetJumpState();
								return;
							}

							// 공격
							if( m_bEnableAttackState )
							{
								Init();
								if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
								{
									pOwner->SetNormalAttack( 0 );
								}
								return;
							}

							pOwner->SetState( CS_DELAY );
						}
					}
				}

				return;
			}
			else
			{
				if( m_bSpecialFireFromJump )
					ProcessSpecialFireTargetTracking( pOwner, m_stSuperPowerAttackUp_Jump, m_stSuperPowerAttackCenter_Jump,m_stSuperPowerAttackDown_Jump );
				else
					ProcessSpecialFireTargetTracking( pOwner, m_stSuperPowerAttackUp, m_stSuperPowerAttackCenter,m_stSuperPowerAttackDown );
			}

		}
		break;
	case PSS_End:
		break;
	}
}

void ioSuperDimensionItem::SetSuperDimensionFireState( ioBaseChar *pOwner, bool bJump )
{
	if( !pOwner )//|| !pOwner->IsNeedProcess() )
		return;

	pOwner->SetState( CS_SUPER_DIMENSION_FIRE );

	m_SuperDimensionFireState = PSS_Find_Target;

	m_bSpecialFireFromJump = bJump;

	/*pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );*/
	DecreaseGauge( m_fNeedGauge );

	if( pOwner->IsNeedProcess() )
	{
		FindTarget( pOwner );

		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_FIRE_STATE_INIT;
		kPacket << bJump;
		kPacket << m_szFirstTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSuperDimensionItem::SetSuperDimensionDashGrapState( ioBaseChar *pOwner, const ioHashString& stTarget )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	
	//////////////////////////////////////////////////////////////////////////
	pOwner->SetState( CS_SUPER_DIMENSION_DASH );
	//////////////////////////////////////////////////////////////////////////

	m_dwDashStartTime = FRAMEGETTIME();

	//시작 에니
	int iAniID = pGrp->GetAnimationIdx( m_SpecialDashInfo.m_StartAni );
	float fTimeRate = m_SpecialDashInfo.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_SuperDimensionDashState = MFS_START;

	m_vDashMoveDir = ioMath::VEC3_ZERO;
	m_dwDashStartEndTime = m_dwDashStartTime + (pGrp->GetAnimationFullTime( iAniID ) * fTimeRate);

	m_szTargetName = stTarget;
}


void ioSuperDimensionItem::CallItemProcess( ioBaseChar *pOwner )
{
	//한단계식 낮추어 간다.
	//한프레임당 차례차례 알아서 조절 될 것이다.
	const StatusValue& hp_status = pOwner->GetHP();
	float fHpPercent = max( hp_status.m_fCurValue / hp_status.m_fMaxValue, 0.f );

	const SuperDimensionHpScaleInfo& oldInfo = m_vHpScaleInfo[m_nHpPercentStep];
	if( fHpPercent <= oldInfo.m_fHpPercent && !m_bLastIncWeapon && m_bItemVisible )
	{
		m_nChangedNowStep = m_nHpPercentStep;

		m_nHpPercentStep++;

		if( m_nHpPercentStep >= (int)m_vHpScaleInfo.size() && !m_bLastIncWeapon )
		{
			m_bLastIncWeapon = true;
			m_nHpPercentStep = (int)m_vHpScaleInfo.size() - 1;
		}

		ChangeWeaponScale( pOwner, oldInfo.m_fScale/* * pOwner->GetKingScaleRate()*/ );
	}
}


void ioSuperDimensionItem::SetWeaponStep( ioBaseChar* pOwner, int nWeaponStep )
{
	if( nWeaponStep >= (int)m_vHpScaleInfo.size() || nWeaponStep < 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioSuperDimensionItem::SetWeaponStep - %d", nWeaponStep );
		return;
	}

	const SuperDimensionHpScaleInfo& nowInfo = m_vHpScaleInfo[nWeaponStep];
	ChangeWeaponScale( pOwner, nowInfo.m_fScale/* * pOwner->GetKingScaleRate()*/  );

	m_nHpPercentStep = nWeaponStep+1;
	m_nHpPercentStep = min( m_nHpPercentStep, (int)(m_vHpScaleInfo.size() - 1) );
	m_nChangedNowStep = nWeaponStep;
}


void ioSuperDimensionItem::SetCharScale( float fScale, ioBaseChar *pOwner )
{
	if( m_dwEffect1 && m_nChangedNowStep >= 0 && m_bItemVisible )
	{
		const SuperDimensionHpScaleInfo& nowInfo = m_vHpScaleInfo[m_nChangedNowStep];
		ChangeWeaponScale( pOwner, nowInfo.m_fScale /** pOwner->GetKingScaleRate()*/ );
		/*ioEffect* pEffect = pOwner->GetEffectByObjID( m_dwEffect1 );
		if( pEffect )
			pEffect->ScaleActualSize( nowInfo.m_fScale * fScale );*/
	}
}


void ioSuperDimensionItem::RemoveCharScale( float fScale, ioBaseChar *pOwner )
{
	if( m_dwEffect1 && m_nChangedNowStep >= 0 && m_bItemVisible )
	{
		if( m_nChangedNowStep < (int)m_vHpScaleInfo.size() )
		{
			const SuperDimensionHpScaleInfo& nowInfo = m_vHpScaleInfo[m_nChangedNowStep];
			ChangeWeaponScale( pOwner, nowInfo.m_fScale/* * pOwner->GetKingScaleRate()*/ );
		}
		else
			ChangeWeaponScale( pOwner, FLOAT1/*pOwner->GetKingScaleRate()*/ );	
	}
}


void ioSuperDimensionItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	m_nHpPercentStep = 0;
	m_bLastIncWeapon = false;

	m_bItemVisible = true;
}


void ioSuperDimensionItem::OnReleased( ioBaseChar *pOwner )
{
	m_bItemVisible = false;

	pOwner->EndEffect(m_dwEffect1, false );
	pOwner->EndEffect(m_dwEffect2, false );

	if( !m_stExtraMeshName.IsEmpty() )
		pOwner->DestroyEntity( m_stExtraMeshName );

	ioAttackableItem::OnReleased( pOwner );
}


void ioSuperDimensionItem::ShowWeapon( ioBaseChar *pOwner )
{
	m_bLastIncWeapon = false;
	m_bItemVisible = true;

	if( m_nChangedNowStep >= 0 )
	{
		const SuperDimensionHpScaleInfo& Info = m_vHpScaleInfo[m_nChangedNowStep];

		ChangeWeaponScale( pOwner, Info.m_fScale/* * pOwner->GetKingScaleRate()*/ );
	}
}


void ioSuperDimensionItem::HideWeapon( ioBaseChar *pOwner )
{
	m_bLastIncWeapon = false;
	m_bItemVisible = false;

	pOwner->EndEffect(m_dwEffect1, false );
	pOwner->EndEffect(m_dwEffect2, false );

	if( !m_stExtraMeshName.IsEmpty() )
		pOwner->DestroyEntity( m_stExtraMeshName );
}


void ioSuperDimensionItem::ChangeWeaponScale( ioBaseChar *pOwner, float fScale )
{
	pOwner->EndEffect(m_dwEffect1, false );
	pOwner->EndEffect(m_dwEffect2, false );

	if( !m_stNormalMeshName.IsEmpty() )
		pOwner->DestroyEntity( m_stNormalMeshName );

	if( !m_stExtraMeshName.IsEmpty() )
		pOwner->DestroyEntity( m_stExtraMeshName );

	if( !m_stExtraMeshName.IsEmpty() )
		pOwner->AddEntity( m_stExtraMeshName );

	if( !m_stWeaponEffec1.IsEmpty() )
	{
		ioEffect *pEffect1 = pOwner->AttachEffect( m_stWeaponEffec1);
		if( pEffect1 )
		{
			pEffect1->ScaleActualSize( fScale );
			m_dwEffect1 = pEffect1->GetUniqueID();
		}
	}

	if( !m_stWeaponEffec2.IsEmpty() )
	{
		ioEffect *pEffect2 = pOwner->AttachEffect( m_stWeaponEffec1);
		if( pEffect2 )
		{
			pEffect2->ScaleActualSize( fScale );
			m_dwEffect2 = pEffect2->GetUniqueID();
		}
	}
}



bool ioSuperDimensionItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_SuperDimensionDashState != MFS_ATTACK_MOVE )
		return false;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( szTargetName );
	if( !pTarget ) return false;

	m_GrapplingTargetName = szTargetName;

	//ChangeSpecialDashGrapMoveState( pChar, pTarget );
	ChangeSpecialDashLoopState( pChar, pTarget );

	if( pTarget->SetItemGrapplingState( pChar->GetCharName(), m_SpecialDashInfo.m_GrapplingInfo ) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << WS_SPECIAL_DASH_GRAP;
		kPacket << pChar->GetTargetRot();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}


D3DXVECTOR3 ioSuperDimensionItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_SuperDimensionDashState == MFS_ATTACK_MOVE )//|| m_SuperDimensionDashState == MFS_GRAPPLING_MOVE )
	{
		if( m_SpecialDashInfo.m_fGrapplingRange > 0.0f )
		{
			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_SpecialDashInfo.m_fGrapplingHeightRate );
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( m_SpecialDashInfo.m_fGrapplingRange > 0.0f )
				vPos += m_SpecialDashInfo.m_fGrapplingRange * vDir;

			return vPos;
		}
	}
	else if( m_SuperDimensionDashState == MFS_GRAPPLING_LOOP || m_SuperDimensionDashState == MFS_END_ATTACK )
	{
		if( m_SpecialDashInfo.m_fGrapplingLoopGapRange > 0.0f )
		{
			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_SpecialDashInfo.m_fGrapplingLoopHeightRate );
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( m_SpecialDashInfo.m_fGrapplingLoopGapRange > 0.0f )
				vPos += m_SpecialDashInfo.m_fGrapplingLoopGapRange * vDir;

			return vPos;
		}
	}

	return ioMath::VEC3_ZERO;
}


//bool ioSuperDimensionItem::IsCharCollisionSkipState()
//{
//	switch( m_SuperDimensionDashState )
//	{
//	case MFS_START:
//	case MFS_GRAPPLING_MOVE:
//		return true;
//	}
//
//	return false;
//}


const ioHashString& ioSuperDimensionItem::FindDashAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_szTargetName.Clear();

	if( !pOwner )
		return m_szTargetName;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return m_szTargetName;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return m_szTargetName;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return m_szTargetName;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_SpecialDashInfo.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_SpecialDashInfo.m_fTargetRange * m_SpecialDashInfo.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_SpecialDashInfo.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_SpecialDashInfo.m_fTargetDownHeight )
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

	return m_szTargetName;
}


bool ioSuperDimensionItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_SpecialDashInfo.m_TargetWoundType ) )
		return false;

	return true;
}


void ioSuperDimensionItem::ProcessSpecialFireTargetTracking( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szFirstTarget );

	//타겟이 안에 계속 들어 와있나 체크가 필요

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	pUp = pGrp->GetActionInPlay( stUp );
	pCenter = pGrp->GetActionInPlay( stCenter );
	pDown = pGrp->GetActionInPlay( stDown );

	D3DXVECTOR3 vAimedDir;
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else if( m_bAimedBall && pBall )
		vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_vAttackFireDir = vAimedDir;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}

	if( pTarget )
	{
		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
		ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
		if( pSimple )
		{
			//pSimple->SetPointDir(  pTarget->GetMidPositionByRate() );		
			pSimple->SetMoveDir( vAimedDir );
		}
	}
}


float ioSuperDimensionItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}


void ioSuperDimensionItem::SetVisible( ioBaseChar *pOwner, bool bVisible )
{
	m_bItemVisible = bVisible;
	m_bVisible = bVisible;

	if( bVisible )
		ShowWeapon( pOwner );
	else
		HideWeapon( pOwner );
}


const ioHashString& ioSuperDimensionItem::GetExtraEffectName()
{
	return m_stWeaponEffec1;
}

float ioSuperDimensionItem::GetActualScale()
{
	if( m_nChangedNowStep >= 0 && m_nChangedNowStep < (int)m_vHpScaleInfo.size() )
	{
		const SuperDimensionHpScaleInfo& NowInfo = m_vHpScaleInfo[m_nChangedNowStep];
		return NowInfo.m_fScale;
	}

	return FLOAT1;
}


void ioSuperDimensionItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioSuperDimensionFireSpecialState::ioSuperDimensionFireSpecialState()
{
}

ioSuperDimensionFireSpecialState::~ioSuperDimensionFireSpecialState()
{
}

void ioSuperDimensionFireSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSuperDimensionFireSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioSuperDimensionItem *pSuperDimentoinItem = ToSuperDimensionItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSuperDimentoinItem )
		pSuperDimentoinItem->ClearState( pOwner );
}

void ioSuperDimensionFireSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSuperDimensionItem *pSuperDimensionItem = ToSuperDimensionItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSuperDimensionItem )
	{
		pSuperDimensionItem->ProcessSuperDimensionFireState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSuperDimensionFireSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSuperDimensionFireSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}

void ioSuperDimensionFireSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioSuperDimensionItem *pSuperDimentionItem = ToSuperDimensionItem(  pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSuperDimentionItem )
		fCurGravity = 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioSuperDimensionDashSpecialState::ioSuperDimensionDashSpecialState()
{
}

ioSuperDimensionDashSpecialState::~ioSuperDimensionDashSpecialState()
{
}

void ioSuperDimensionDashSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSuperDimensionDashSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
	
	ioSuperDimensionItem *pSuperDimentoinItem = ToSuperDimensionItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSuperDimentoinItem )
		pSuperDimentoinItem->ClearState( pOwner );
}

void ioSuperDimensionDashSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSuperDimensionItem *pSuperDimensionItem = ToSuperDimensionItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSuperDimensionItem )
	{
		pSuperDimensionItem->ProcessSuperDimensionDashState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSuperDimensionDashSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSuperDimensionDashSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}
