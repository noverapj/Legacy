#include "stdafx.h"

#include "ioMirWarrior.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ioChargeComboJump3.h"

ioMirWarrior::ioMirWarrior()
{
	Init();
}

ioMirWarrior::ioMirWarrior( const ioMirWarrior &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),	
	m_PreAnimationList( rhs.m_PreAnimationList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_ExtendMove( rhs.m_ExtendMove ),
	m_ExtendJumpDown( rhs.m_ExtendJumpDown ),
	m_ExtendJumpMove( rhs.m_ExtendJumpMove ),
	m_ExtendDashMove( rhs.m_ExtendDashMove ),
	m_MoveAttackEndAttribute( rhs.m_MoveAttackEndAttribute ),
	m_LandingAniamation( rhs.m_LandingAniamation ),
	m_fLandingRate( rhs.m_fLandingRate ),
	m_fDropZoneJumpPowerAmt( rhs.m_fDropZoneJumpPowerAmt )

{
	Init();
}

ioMirWarrior::~ioMirWarrior()
{
}

void ioMirWarrior::Init()
{	
	m_AttackState			= AS_NONE;
	m_SpecialState			= SS_NONE;

	m_iCurCombo				= 0;	
	m_dwAttackStartTime		= 0;
	m_dwKeyReserveTime		= 0;
	m_dwExtendMoveStartTime	= 0;
	m_dwCurrWeaponIdx		= 0;

	m_dwExtendAttackEndTime	= 0;
	m_dMoveAttackEndTime	= 0;

	m_fCurrMoveRange		= 0.0f;
	m_vMoveDir				= ioMath::VEC3_ZERO;
}

void ioMirWarrior::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );

	//ATTACK
	int iPreCount = rkLoader.LoadInt_e( "pre_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szKey, "pre_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_PreAnimationList.push_back( ioHashString( szBuf ) );
	}

	//EXTEND
	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	LoadExtendMoveProperty( rkLoader, "extend_move", m_ExtendMove );
	LoadExtendMoveProperty( rkLoader, "extend_jump_down", m_ExtendJumpDown );
	LoadExtendMoveProperty( rkLoader, "extend_jump_move", m_ExtendJumpMove );
	LoadExtendMoveProperty( rkLoader, "extend_dash_move", m_ExtendDashMove );
	LoadAttackAttribute( "extend_move_end_attack", m_MoveAttackEndAttribute, rkLoader );
	
	rkLoader.LoadString_e( "extend_jump_down_landing", "", szBuf, MAX_PATH );	
	m_LandingAniamation		= szBuf;
	m_fLandingRate			= rkLoader.LoadFloat_e( "extend_jump_down_landing_rate", FLOAT1 );
	m_fDropZoneJumpPowerAmt = rkLoader.LoadFloat_e( "drop_zone_jump_power_amt", FLOAT100 );
}

void ioMirWarrior::LoadExtendMoveProperty( ioINILoader &rkLoader, const char* szTitle, ExtendMoveInfo& Info )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_min_speed", szTitle );
	Info.m_fMinSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_max_speed", szTitle );
	Info.m_fMaxSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_max_range", szTitle );
	Info.m_fMaxRange = rkLoader.LoadFloat(szKey, 0.0f );

	wsprintf_e( szKey, "%s_angle", szTitle );
	Info.m_fAngle = rkLoader.LoadFloat(szKey, 0.0f );

	wsprintf_e( szKey, "%s_accel_speed", szTitle );
	Info.m_fAccelSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_accel_time", szTitle );
	Info.m_dwAccelTime = rkLoader.LoadInt( szKey, 0.0f );

	wsprintf_e( szKey, "%s_ignore_time", szTitle );
	Info.m_dwMoveIgonreTime = rkLoader.LoadInt( szKey, 0.0f );
	
	wsprintf_e( szKey, "%s_animation", szTitle );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	Info.m_Animation = szBuf;

	wsprintf_e( szKey, "%s_animation_rate", szTitle );
	Info.m_fAnimationRate = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_attribute", szTitle );
	Info.m_dwWeaponIdx = rkLoader.LoadInt( szKey, 0 );
}

ioItem* ioMirWarrior::Clone()
{
	return new ioMirWarrior( *this );
}

ioWeaponItem::WeaponSubType ioMirWarrior::GetSubType() const
{
	return WST_MIR_WARRIOR;
}

void ioMirWarrior::OnEquiped( ioBaseChar *pOwner )
{
	ioAttackableItem::OnEquiped( pOwner );	
}

void ioMirWarrior::OnReleased( ioBaseChar *pOwner )
{
	ioAttackableItem::OnReleased( pOwner );

	ClearWeapon( pOwner );
}

// 내부 공용 Explicit 공격-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioWeapon* ioMirWarrior::AttackFire( ioBaseChar *pOwner, DWORD dwAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, DWORD dwWeaponIndex, FireTimeType eType )
{
	if ( !pOwner )
		return NULL;

	FireTime kFireTime;
	kFireTime.dwStart				= FRAMEGETTIME();
	kFireTime.iAttributeIdx			= dwAttackIndex;
	kFireTime.iResistanceIdx		= 0;
	kFireTime.szWoundedAni			= "";
	kFireTime.dwWoundedAniDuration	= 0;
	kFireTime.bLoopWoundedAni		= false;
	kFireTime.dwWeaponIdx			= dwWeaponIndex;
	kFireTime.eFireTimeType			= eType;

	return pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
}

// 노말 공격 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	int iPreGatherCnt = m_PreAnimationList.size();

	m_AttackState		= AS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		int iAniID = pGrp->GetAnimationIdx( m_PreAnimationList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioMirWarrior::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_AttackState )
	{
	case AS_CHARGING:
		OnCharging( pOwner );
		break;
	case AS_NORMAL_ATTACK:
		OnNormalAttack( pOwner );
		break;
	case AS_EXTEND_ATTACK:
		OnExtendAttack( pOwner );
		break;
	}
}

void ioMirWarrior::OnNormalAttack( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

// 노말 공격 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_AttackState = AS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_NORMAL_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// D꾹 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( GetCurBullet() >= GetNeedBullet() )
			{
				SetUseExtraGauge( GetNeedBullet() );
				ChangeToExtendAttackFire( pOwner );
			}
			else
			{
				ChangeToAttackFire( pOwner );
			}
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

// 특별 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ChangeToExtendAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	m_AttackState = AS_EXTEND_ATTACK;

	bool bComboEnd	= false;
	int iCurCombo	= m_iCurCombo;
	if(	!COMPARE( m_iCurCombo, 0, (int)m_vExtendAttributeList.size() ) )
		iCurCombo = (int)m_vExtendAttributeList.size() - 1;
	
	pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[iCurCombo] );
	m_szCurAni					= m_vExtendAttributeList[iCurCombo].m_AttackAnimation;
	m_fCurAniRate				= m_vExtendAttributeList[iCurCombo].m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( iCurCombo, bComboEnd );

	float fTimeRate		= m_vExtendAttributeList[iCurCombo].m_fAttackAniRate;
	DWORD dwPreDelay	= m_vExtendAttributeList[iCurCombo].m_dwPreDelay;
	int iAniID			= pGrp->GetAnimationIdx( m_vExtendAttributeList[iCurCombo].m_AttackAnimation );

	m_dwExtendAttackEndTime	= FRAMEGETTIME();
	if( iAniID != -1 )
		m_dwExtendAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_EXTEND_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iCurCombo++;
}

void ioMirWarrior::OnExtendAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwExtendAttackEndTime < dwCurTime )
	{
		ioWeaponItem::CheckNormalAttack( pOwner );
	}
	else
	{
		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;
	}
}

// 특별 상대로 전환 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( GetCurBullet() < GetNeedBullet() )
		return;
		
	pOwner->SetState( CS_MIR_WARRIOR_DASH_STATE );

	if( eCurrDir != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( eCurrDir );

	m_vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	switch( eCallerType )
	{
	case CCSD_JUMP:
		{			
			ChangeToExtendJumpDown( pOwner );
		}
		break;
	case CCSD_DASH_ATTACK:
		{
			ChangeToExtendDashMove( pOwner );
		}
		break;
	default:
		{		
			ChangeToExtendMove( pOwner );
		}
		break;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_CONDITIONAL_SPECIAL_DASH );		
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		kPacket << m_fCurExtraGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	SetUseExtraGauge( GetNeedBullet() );
}

bool ioMirWarrior::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	//대쉬 가능 체크(키 입력 부분도 해당 함수와 관련이 있음)
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_AttackState )
		{
		case AS_NORMAL_ATTACK:
		case AS_EXTEND_ATTACK:
			if( !bCheck )
				return false;

			return true;
		}
	}
	return false;
}

bool ioMirWarrior::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	//input_dash_s 입력 가능 여부 체크
	if( m_AttackState == AS_EXTEND_ATTACK )
	{		
		return true;
	}

	return false;
}


// 특별 상태 해제시 초기화 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ClearWeapon( ioBaseChar* pOwner )
{
	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCurrWeaponIdx ) );
	if( pZone )
		pZone->SetWeaponDead();
}

void ioMirWarrior::ClearState( ioBaseChar* pOwner )
{
	ClearWeapon( pOwner );

	m_AttackState		= AS_NONE;
	m_SpecialState		= SS_NONE;
	m_dwCurrWeaponIdx	= 0;
	m_dwKeyReserveTime	= 0;
}

// 특별 이동으로 전환 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ChangeToExtendMove( ioBaseChar *pOwner )
{
	m_dwExtendMoveStartTime			= FRAMEGETTIME() + m_ExtendMove.m_dwMoveIgonreTime;
	m_SpecialState					= SS_EXTEND_MOVE;
	m_fCurrMoveRange				= 0.0f;	

	ClearWeapon( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ExtendMove.m_Animation );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_ExtendMove.m_fAnimationRate );

	pOwner->IncreaseWeaponIndexBase();
	m_dwCurrWeaponIdx = pOwner->GetWeaponIndexBase();
	AttackFire( pOwner, m_ExtendMove.m_dwWeaponIdx, pOwner->GetMidPositionByRate(), m_vMoveDir, m_dwCurrWeaponIdx, FTT_EXTEND_ATTACK );
}

void ioMirWarrior::OnExtendMove( ioBaseChar *pOwner )
{
	if( !CheckWeaponLive( pOwner ) )
	{
		if( pOwner->IsNeedProcess() )
		{
			if( IsLandTouch( pOwner ) )
			{
				ChangeToExtendMoveEndAttack( pOwner );
			}
			else
			{
				ChangeToEnd( pOwner );
			}
		}
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	MoveState eState = ProcessExtendMove( fTimePerSec, pOwner, m_ExtendMove, true );
	switch( eState )
	{
	case MS_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			ChangeToExtendMoveEndAttack( pOwner );
		}
		break;
	case MS_END:
		ChangeToEnd( pOwner );
		break;
	}
}

// 특별 이동(지정된 각도로)로 전환 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ChangeToExtendJumpDown( ioBaseChar *pOwner )
{
	ClearWeapon( pOwner );

	m_SpecialState		= SS_EXTEND_JUMP_DOWN;
	m_fCurrMoveRange	= 0.0f;

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_ExtendJumpDown.m_fAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	m_vMoveDir = qtNewRot * m_vMoveDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ExtendJumpDown.m_Animation );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_ExtendJumpDown.m_fAnimationRate );

	pOwner->IncreaseWeaponIndexBase();
	m_dwCurrWeaponIdx = pOwner->GetWeaponIndexBase();
	AttackFire( pOwner, m_ExtendJumpDown.m_dwWeaponIdx, pOwner->GetMidPositionByRate(), m_vMoveDir, m_dwCurrWeaponIdx, FTT_EXTEND_ATTACK );
}

void ioMirWarrior::OnExtendJumpDown( ioBaseChar *pOwner )
{
	if( !CheckWeaponLive( pOwner ) )
	{
		if( pOwner->IsNeedProcess() )
		{
			if( IsLandTouch( pOwner ) )
			{
				ChangeToExtendMoveEndAttack( pOwner );
			}
			else
			{
				ChangeToEnd( pOwner );
			}
		}
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	MoveState eState = ProcessExtendMove( fTimePerSec, pOwner, m_ExtendJumpDown, false );
	switch( eState )
	{
	case MS_LANDING:
		ChangeToLading( pOwner );
		break;
	case MS_ATTACK:
	case MS_END:
		ChangeToEnd( pOwner );
		break;
	}
}

// 위의 특별 이동후 랜딩 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ChangeToLading( ioBaseChar *pOwner )
{
	ClearWeapon( pOwner );

	m_dwLandingEndTime	= FRAMEGETTIME();
	m_SpecialState		= SS_LANDING;
	
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );	

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );	
	int iAniID = pGrp->GetAnimationIdx( m_LandingAniamation );
	if( iAniID != -1 )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fLandingRate, 0.0f, 0.0f, true );
		m_dwLandingEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fLandingRate;
	}
}

void ioMirWarrior::OnLadning( ioBaseChar *pOwner )
{
	if( 0 < m_dwLandingEndTime && m_dwLandingEndTime < FRAMEGETTIME() )
	{
		m_vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		ChangeToExtendJumpMove( pOwner );
	}
}

// 특별 대쉬 랜딩 후 이동 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ChangeToExtendJumpMove( ioBaseChar *pOwner )
{
	m_dwExtendMoveStartTime			= FRAMEGETTIME() + m_ExtendJumpMove.m_dwMoveIgonreTime;
	m_SpecialState					= SS_EXTEND_JUMP_MOVE;
	m_fCurrMoveRange				= 0.0f;	

	ClearWeapon( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ExtendJumpMove.m_Animation );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_ExtendJumpMove.m_fAnimationRate );

	pOwner->IncreaseWeaponIndexBase();
	m_dwCurrWeaponIdx = pOwner->GetWeaponIndexBase();
	AttackFire( pOwner, m_ExtendJumpMove.m_dwWeaponIdx, pOwner->GetMidPositionByRate(), m_vMoveDir, m_dwCurrWeaponIdx, FTT_EXTEND_ATTACK );
}

void ioMirWarrior::OnExtendJumpMove( ioBaseChar *pOwner )
{
	if( !CheckWeaponLive( pOwner ) )
	{
		if( pOwner->IsNeedProcess() )
		{
			if( IsLandTouch( pOwner ) )
			{
				ChangeToExtendMoveEndAttack( pOwner );
			}
			else
			{
				ChangeToEnd( pOwner );
			}
		}
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	MoveState eState = ProcessExtendMove( fTimePerSec, pOwner, m_ExtendJumpMove, true );
	switch( eState )
	{
	case MS_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			ChangeToExtendMoveEndAttack( pOwner );
		}
		break;
	case MS_END:
		ChangeToEnd( pOwner );
		break;
	}
}

// 대쉬 후 특별 대쉬 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ChangeToExtendDashMove( ioBaseChar *pOwner )
{
	m_dwExtendMoveStartTime			= FRAMEGETTIME() + m_ExtendDashMove.m_dwMoveIgonreTime;
	m_SpecialState					= SS_EXTEND_DASH_MOVE;
	m_fCurrMoveRange				= 0.0f;	

	ClearWeapon( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashMove.m_Animation );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_ExtendDashMove.m_fAnimationRate );

	pOwner->IncreaseWeaponIndexBase();
	m_dwCurrWeaponIdx = pOwner->GetWeaponIndexBase();
	AttackFire( pOwner, m_ExtendDashMove.m_dwWeaponIdx, pOwner->GetMidPositionByRate(), m_vMoveDir, m_dwCurrWeaponIdx, FTT_EXTEND_ATTACK );
}

void ioMirWarrior::OnExtendDashMove( ioBaseChar *pOwner )
{
	if( !CheckWeaponLive( pOwner ) )
	{
		if( pOwner->IsNeedProcess() )
		{
			if( IsLandTouch( pOwner ) )
			{
				ChangeToExtendMoveEndAttack( pOwner );
			}
			else
			{
				ChangeToEnd( pOwner );
			}
		}
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	MoveState eState = ProcessExtendMove( fTimePerSec, pOwner, m_ExtendDashMove, true );
	switch( eState )
	{
	case MS_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			ChangeToExtendMoveEndAttack( pOwner );
		}
		break;
	case MS_END:
		ChangeToEnd( pOwner );
		break;
	}
}

// 특별 이동 후 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ChangeToExtendMoveEndAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );	
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->ClearDirDoubleClick();

	ClearCancelInfo();
	ClearWeapon( pOwner );

	m_SpecialState = SS_EXTEND_MOVE_ATTACK;	

	pOwner->SetNormalAttackByAttribute( m_MoveAttackEndAttribute );
	m_szCurAni					= m_MoveAttackEndAttribute.m_AttackAnimation;
	m_fCurAniRate				= m_MoveAttackEndAttribute.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( 0, true );

	float fTimeRate		= m_MoveAttackEndAttribute.m_fAttackAniRate;
	DWORD dwPreDelay	= m_MoveAttackEndAttribute.m_dwPreDelay;
	int iAniID			= pGrp->GetAnimationIdx( m_MoveAttackEndAttribute.m_AttackAnimation );

	m_dMoveAttackEndTime = FRAMEGETTIME();
	if( iAniID != -1 )
		m_dMoveAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_MOVE_AND_ATTACK );		
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMirWarrior::OnExtendMoveEndAttack( ioBaseChar *pOwner )
{
	if( m_dMoveAttackEndTime < FRAMEGETTIME() && !IsLandTouch( pOwner ) )
	{
		ChangeToEnd( pOwner );
	}
	else
	{
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );
			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
	}
}

// 특별 상태 Process ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ProcessSpecialState( ioBaseChar *pOwner, ioPlayStage* pStage )
{	
	switch( m_SpecialState )
	{
	case SS_EXTEND_MOVE:
		OnExtendMove( pOwner );
		break;
	case SS_EXTEND_MOVE_ATTACK:
		OnExtendMoveEndAttack( pOwner );
		break;
	case SS_EXTEND_JUMP_DOWN:
		OnExtendJumpDown( pOwner );
		break;
	case SS_EXTEND_JUMP_MOVE:
		OnExtendJumpMove( pOwner );
		break;
	case SS_EXTEND_DASH_MOVE:
		OnExtendDashMove( pOwner );
		break;
	case SS_LANDING:
		OnLadning( pOwner );
		break;	
	case SS_END:
		break;
	}
}

// 특별 상태 이동 처리 --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioMirWarrior::MoveState ioMirWarrior::ProcessExtendMove( float fTimePerSec, ioBaseChar* pOwner, const ExtendMoveInfo& rkInfo, bool bKeyCheck )
{
	//엑셀 가속
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwExtendMoveStartTime > dwCurTime )
		return MS_MOVE;

	float fCurSpeed = rkInfo.m_fMinSpeed;
	DWORD dwGapTime = dwCurTime - ( m_dwExtendMoveStartTime + rkInfo.m_dwAccelTime );

	if( m_dwExtendMoveStartTime + rkInfo.m_dwAccelTime < dwCurTime )
	{
		float fTimeRate = (float)dwGapTime / FLOAT1000;
		float fCurAccel = rkInfo.m_fAccelSpeed * fTimeRate;

		fCurSpeed += fCurAccel;
	}

	//최대 속도 못 넘어가게
	if( fCurSpeed >= rkInfo.m_fMaxSpeed )
		fCurSpeed = rkInfo.m_fMaxSpeed;

	float fDistance = fCurSpeed * fTimePerSec;

	//이동 거리 계산 및 이동 종료 체크	
	D3DXVECTOR3 vMoveAmt = ioMath::VEC3_ZERO;
	if( m_fCurrMoveRange + fDistance < rkInfo.m_fMaxRange )
	{
		vMoveAmt = m_vMoveDir * fDistance;
		m_fCurrMoveRange += fDistance;

		if( pOwner->IsAttackKey() && bKeyCheck )
		{
			if( pOwner->IsCharDropZonePos() )
				return MS_END;
			else
				return MS_ATTACK;
		}

		ProcessTranslate( pOwner, vMoveAmt );
	}
	else
	{
		float fGapDistance = rkInfo.m_fMaxRange - m_fCurrMoveRange;
		if( fGapDistance > 0.0f )
		{
			vMoveAmt = m_vMoveDir * fGapDistance;
			m_fCurrMoveRange = rkInfo.m_fMaxRange;
		}

		ProcessTranslate( pOwner, vMoveAmt );

		if( pOwner->IsCharDropZonePos() )
			return MS_END;
		else
			return MS_ATTACK;
	}

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap <= 5.0f )
	{
		return MS_LANDING;
	}

	return MS_MOVE;
}

void ioMirWarrior::ProcessTranslate( ioBaseChar* pOwner, D3DXVECTOR3 vMoveAmt )
{
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

// 특별 웨폰이 살아 있는지 체크 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioMirWarrior::CheckWeaponLive( ioBaseChar *pOwner )
{	
	ioZoneEffectWeapon* pZoneWeapon = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCurrWeaponIdx ) );
	if( pZoneWeapon && pZoneWeapon->IsLive() )
		return true;

	return false;
}

// 점핑/랜딩 중 특별 전환 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	ioAttackableItem::ProcessJumpAttack( pOwner, fHeightGap );

	ioChargeComboJump3 *pChargeComboJump = ToChargeComboJump3( GetCurExtendJump() );
	if( pChargeComboJump && pChargeComboJump->GetCurChargeComboState() == ioChargeComboJump3::CCS_EXTEND_ATTACK )
	{
		if( pOwner->IsDirKeyDoubleClick() )
		{
			ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
			SetChangeConditionalSpecialDash( pOwner, eDir, ioAttackableItem::CCSD_JUMP );
		}
	}
}

// 특별 상태 종료 -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::ChangeToEnd( ioBaseChar *pOwner )
{
	m_SpecialState = SS_END;
	pOwner->ClearDirDoubleClick();

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( pOwner->IsCharDropZonePos() || fHeightGap > 5.0f )
	{
		pOwner->SetSkillNextJumpAmt( m_fDropZoneJumpPowerAmt );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_END );		
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case AS_NORMAL_ATTACK:
		{	
			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case AS_EXTEND_ATTACK:
		{
			rkPacket >> m_iCurCombo;
			ChangeToExtendAttackFire( pOwner );
		}
		break;
	case AS_CONDITIONAL_SPECIAL_DASH:
		{
			int eCurrDir;
			int eCallerType;

			rkPacket >> eCurrDir >> eCallerType;
			rkPacket >> m_fCurExtraGauge;
			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)eCurrDir, eCallerType );
		}
		break;
	case AS_MOVE_AND_ATTACK:
		{
			ChangeToExtendMoveEndAttack( pOwner );
		}
		break;
	case AS_END:
		{
			ChangeToEnd( pOwner );
		}
		break;
	}
}

// 게이지 함수들 --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::UpdateExtraData( ioBaseChar *pOwner )
{
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		{
			m_fCurExtraGauge += m_fExtraGaugeBaseInc * fTimePerSec;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		break;
	}
}

int ioMirWarrior::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioMirWarrior::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioMirWarrior::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioMirWarrior::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioMirWarrior::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

// 기타 용병 관련 함수들 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioMirWarrior::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
}

bool ioMirWarrior::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_AttackState == AS_CHARGING )
			return true;		
	}

	return false;
}

bool ioMirWarrior::IsLandTouch( ioBaseChar *pOwner )
{
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( pOwner->IsCharDropZonePos() || fHeightGap > 5.0f )
	{
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioMirWarriorDashSpecialState::ioMirWarriorDashSpecialState()
{
}

ioMirWarriorDashSpecialState::~ioMirWarriorDashSpecialState()
{
}

void ioMirWarriorDashSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMirWarriorDashSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMirWarrior *pWarrior = ToMirWarrior( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWarrior && eNewState != CS_ATTACK )
		pWarrior->ClearState( pOwner );
}

bool ioMirWarriorDashSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	return true;
}

void ioMirWarriorDashSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMirWarrior *pWarrior = ToMirWarrior( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWarrior && pWarrior->GetSpecialState() != ioMirWarrior::SS_END )
	{
		pWarrior->ProcessSpecialState( pOwner, pOwner->GetCreator() );
	}

}

void ioMirWarriorDashSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	ioMirWarrior *pWarrior = ToMirWarrior( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pWarrior || pWarrior->GetSpecialState() == ioMirWarrior::SS_END )
	{
		float fSkillNextJumpAmt = pOwner->GetSkillNextJumpAmt();
		if( fSkillNextJumpAmt > 0.0f )
		{
			pOwner->SetSKillEndJumpState( fSkillNextJumpAmt );
		}
		else
		{
			float fMinFallinRate = ( pOwner->GetWorldCollisionBox().GetExtents( 1 ) * 2.0f ) * pOwner->GetMinFallHeightRate();
			float fCharPosGap = pOwner->GetWorldPosition().y - fBottomHeight;
			if( fCharPosGap > fMinFallinRate )
				pOwner->SetFallState( true );
		}
	}
}

bool ioMirWarriorDashSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}

bool ioMirWarriorDashSpecialState::IsStateEnd( ioBaseChar* pOwner )
{
	ioMirWarrior *pWarrior = ToMirWarrior( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pWarrior || pWarrior->GetSpecialState() == ioMirWarrior::SS_END )
	{
		return true;
	}

	return false;
}