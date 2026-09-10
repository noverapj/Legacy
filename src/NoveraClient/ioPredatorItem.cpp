
#include "stdafx.h"

#include "ioPredatorItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioPredatorItem::ioPredatorItem()
{	
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	Init();
}

ioPredatorItem::ioPredatorItem( const ioPredatorItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_szWaitPreDelayAni( rhs.m_szWaitPreDelayAni ),
	m_fWaitPreDelayAniRate( rhs.m_fWaitPreDelayAniRate ),
	m_szWaitLoopAni( rhs.m_szWaitLoopAni ),
	m_szWaitMoveAni( rhs.m_szWaitMoveAni ),
	m_fWaitMoveAniRate( rhs.m_fWaitMoveAniRate ),
	m_szAimLoopAni( rhs.m_szAimLoopAni ),
	m_szAimMoveAni_Front( rhs.m_szAimMoveAni_Front ),
	m_szAimMoveAni_Back( rhs.m_szAimMoveAni_Back ),
	m_szAimMoveAni_Left( rhs.m_szAimMoveAni_Left ),
	m_szAimMoveAni_Right( rhs.m_szAimMoveAni_Right ),
	m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),
	m_fMarkerRange( rhs.m_fMarkerRange ),
	m_szReadyMarker( rhs.m_szReadyMarker ),
	m_szAimingMarker( rhs.m_szAimingMarker ),
	m_szAimedMarker( rhs.m_szAimedMarker ),
	m_fAimAngle( rhs.m_fAimAngle ),
	m_fAimRange( rhs.m_fAimRange ),
	m_fAimMinRange( rhs.m_fAimMinRange ),
	m_AimUpAni( rhs.m_AimUpAni ),
	m_AimCenterAni( rhs.m_AimCenterAni ),
	m_AimDownAni( rhs.m_AimDownAni ),
	m_dwAimBipeMoveTime( rhs.m_dwAimBipeMoveTime ),
	m_dwAimBipeReturnTime( rhs.m_dwAimBipeReturnTime ),
	m_AimFireUpAni( rhs.m_AimFireUpAni ),
	m_AimFireCenterAni( rhs.m_AimFireCenterAni ),
	m_AimFireDownAni( rhs.m_AimFireDownAni ),
	m_AimAttack( rhs.m_AimAttack ),
	m_ComboAttackList( rhs.m_ComboAttackList ),
	m_ComboFireUpList( rhs.m_ComboFireUpList ),
	m_ComboFireCenterList( rhs.m_ComboFireCenterList ),
	m_ComboFireDownList( rhs.m_ComboFireDownList ),
	m_ComboAimUpAni( rhs.m_ComboAimUpAni ),
	m_ComboAimCenterAni( rhs.m_ComboAimCenterAni ),
	m_ComboAimDownAni( rhs.m_ComboAimDownAni ),
	m_ComboChargeAttack( rhs.m_ComboChargeAttack ),
	m_ComboDashAttack( rhs.m_ComboDashAttack ),
	m_nMaxSpecialComboCnt( rhs.m_nMaxSpecialComboCnt ),
	m_dwComboChargeTime( rhs.m_dwComboChargeTime ),
	m_szJumpWaitPreDelayAni( rhs.m_szJumpWaitPreDelayAni ),
	m_fJumpWaitPreDelayAniRate( rhs.m_fJumpWaitPreDelayAniRate ),
	m_szJumpWaitLoopAni( rhs.m_szJumpWaitLoopAni ),
	m_szJumpAimLoopAni( rhs.m_szJumpAimLoopAni ),
	m_fJumpMarkerRange( rhs.m_fJumpMarkerRange ),
	m_fJumpAimAngle( rhs.m_fJumpAimAngle ),
	m_fJumpAimRange( rhs.m_fJumpAimRange ),
	m_fJumpAimMinRange( rhs.m_fJumpAimMinRange ),
	m_JumpAimUpAni( rhs.m_JumpAimUpAni ),
	m_JumpAimCenterAni( rhs.m_JumpAimCenterAni ),
	m_JumpAimDownAni( rhs.m_JumpAimDownAni ),
	m_JumpAimFireUpAni( rhs.m_JumpAimFireUpAni ),
	m_JumpAimFireCenterAni( rhs.m_JumpAimFireCenterAni ),
	m_JumpAimFireDownAni( rhs.m_JumpAimFireDownAni ),
	m_JumpAimAttack( rhs.m_JumpAimAttack ),
	m_JumpComboAttackList( rhs.m_JumpComboAttackList ),
	m_JumpComboFireUpList( rhs.m_JumpComboFireUpList ),
	m_JumpComboFireCenterList( rhs.m_JumpComboFireCenterList ),
	m_JumpComboFireDownList( rhs.m_JumpComboFireDownList ),
	m_JumpComboAimUpAni( rhs.m_JumpComboAimUpAni ),
	m_JumpComboAimCenterAni( rhs.m_JumpComboAimCenterAni ),
	m_JumpComboAimDownAni( rhs.m_JumpComboAimDownAni ),
	m_JumpComboChargeAttack( rhs.m_JumpComboChargeAttack ),
	m_JumpComboDashAttack( rhs.m_JumpComboDashAttack ),
	m_nMaxJumpSpecialComboCnt( rhs.m_nMaxJumpSpecialComboCnt ),
	m_fEndJumpPower( rhs.m_fEndJumpPower ),
	m_dwJumpSpecialStateTime( rhs.m_dwJumpSpecialStateTime )
{
	m_fCurExtraGauge = 0.0f;
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	Init();
}

ioPredatorItem::~ioPredatorItem()
{
}

void ioPredatorItem::LoadProperty( ioINILoader &rkLoader )
{
	//기본 파일 읽기
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	//공격 준비 애니메이션
	int nMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < nMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", FLOAT100 );

	rkLoader.LoadString_e( "wait_pre_delay_ani", "", szBuf, MAX_PATH );
	m_szWaitPreDelayAni = szBuf;
	m_fWaitPreDelayAniRate = rkLoader.LoadFloat_e( "wait_pre_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "wait_loop_ani", "", szBuf, MAX_PATH );
	m_szWaitLoopAni = szBuf;

	rkLoader.LoadString_e( "wait_move_ani", "", szBuf, MAX_PATH );
	m_szWaitMoveAni = szBuf;

	m_fWaitMoveAniRate = rkLoader.LoadFloat_e( "wait_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "aim_loop_ani", "", szBuf, MAX_PATH );
	m_szAimLoopAni = szBuf;

	rkLoader.LoadString_e( "aim_move_front_ani", "", szBuf, MAX_PATH );
	m_szAimMoveAni_Front = szBuf;
	rkLoader.LoadString_e( "aim_move_back_ani", "", szBuf, MAX_PATH );
	m_szAimMoveAni_Back = szBuf;
	rkLoader.LoadString_e( "aim_move_left_ani", "", szBuf, MAX_PATH );
	m_szAimMoveAni_Left = szBuf;
	rkLoader.LoadString_e( "aim_move_right_ani", "", szBuf, MAX_PATH );
	m_szAimMoveAni_Right = szBuf;

	m_fAimMoveAniRate = rkLoader.LoadFloat_e( "aim_move_ani_rate", FLOAT1 );

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );
	rkLoader.LoadString( "ready_marker", "", szBuf, MAX_PATH );	
	m_szReadyMarker = szBuf;
	rkLoader.LoadString( "aiming_marker", "", szBuf, MAX_PATH );	
	m_szAimingMarker = szBuf;
	rkLoader.LoadString( "aimed_marker", "", szBuf, MAX_PATH );	
	m_szAimedMarker = szBuf;

	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fAimMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	rkLoader.LoadString( "aim_up_ani", "", szBuf, MAX_PATH );	
	m_AimUpAni = szBuf;
	rkLoader.LoadString( "aim_center_ani", "", szBuf, MAX_PATH );	
	m_AimCenterAni = szBuf;
	rkLoader.LoadString( "aim_down_ani", "", szBuf, MAX_PATH );	
	m_AimDownAni = szBuf;

	rkLoader.LoadString( "aim_fire_up_ani", "", szBuf, MAX_PATH );	
	m_AimFireUpAni = szBuf;	  
	rkLoader.LoadString( "aim_fire_center_ani", "", szBuf, MAX_PATH );	
	m_AimFireCenterAni = szBuf;
	rkLoader.LoadString( "aim_fire_down_ani", "", szBuf, MAX_PATH );	
	m_AimFireDownAni = szBuf;

	m_dwAimBipeMoveTime = rkLoader.LoadInt_e( "aim_move_bipe_time", 1000 );
	m_dwAimBipeReturnTime = rkLoader.LoadInt_e( "aim_return_bipe_time", 500 );

	LoadAttackAttribute( "aim_attack", m_AimAttack, rkLoader );
	LoadAttackAttribute( "combo_charge_attack", m_ComboChargeAttack, rkLoader );
	LoadAttackAttribute( "combo_dash_attack", m_ComboDashAttack, rkLoader );

	m_ComboAttackList.clear();
	m_ComboFireUpList.clear();
	m_ComboFireCenterList.clear();
	m_ComboFireDownList.clear();

	m_nMaxSpecialComboCnt = rkLoader.LoadInt_e( "max_special_combo_attack", 0 );

	for ( int i=0; i<m_nMaxSpecialComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_ComboAttackList.push_back( sAttackAttribute );

		wsprintf_e( szKey, "combo_attack%d_up", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ComboFireUpList.push_back( szBuf );

		wsprintf_e( szKey, "combo_attack%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ComboFireCenterList.push_back( szBuf );

		wsprintf_e( szKey, "combo_attack%d_down", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ComboFireDownList.push_back( szBuf );
	}

	rkLoader.LoadString( "combo_aim_up_ani", "", szBuf, MAX_PATH );	
	m_ComboAimUpAni = szBuf;
	rkLoader.LoadString( "combo_aim_center_ani", "", szBuf, MAX_PATH );	
	m_ComboAimCenterAni = szBuf;
	rkLoader.LoadString( "combo_aim_down_ani", "", szBuf, MAX_PATH );	
	m_ComboAimDownAni = szBuf;

	m_dwComboChargeTime = rkLoader.LoadInt_e( "combo_charge_time", 0 );

	rkLoader.LoadString_e( "jump_wait_pre_delay_ani", "", szBuf, MAX_PATH );
	m_szJumpWaitPreDelayAni = szBuf;

	m_fJumpWaitPreDelayAniRate = rkLoader.LoadFloat_e( "jump_wait_pre_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_wait_loop_ani", "", szBuf, MAX_PATH );
	m_szJumpWaitLoopAni = szBuf;

	rkLoader.LoadString_e( "jump_aim_loop_ani", "", szBuf, MAX_PATH );
	m_szJumpAimLoopAni = szBuf;

	m_fJumpMarkerRange = rkLoader.LoadFloat_e( "jump_marker_range", 0.0f );

	m_fJumpAimAngle = rkLoader.LoadFloat_e( "jump_aim_angle", 45.0f );
	m_fJumpAimRange = rkLoader.LoadFloat_e( "jump_aim_range", FLOAT1000 );
	m_fJumpAimMinRange = rkLoader.LoadFloat_e( "jump_aim_min_range", 0.0f );

	rkLoader.LoadString( "jump_aim_up_ani", "", szBuf, MAX_PATH );	
	m_JumpAimUpAni = szBuf;
	rkLoader.LoadString( "jump_aim_center_ani", "", szBuf, MAX_PATH );	
	m_JumpAimCenterAni = szBuf;
	rkLoader.LoadString( "jump_aim_down_ani", "", szBuf, MAX_PATH );	
	m_JumpAimDownAni = szBuf;

	rkLoader.LoadString( "jump_aim_fire_up_ani", "", szBuf, MAX_PATH );	
	m_JumpAimFireUpAni = szBuf;	  
	rkLoader.LoadString( "jump_aim_fire_center_ani", "", szBuf, MAX_PATH );	
	m_JumpAimFireCenterAni = szBuf;
	rkLoader.LoadString( "jump_aim_fire_down_ani", "", szBuf, MAX_PATH );	
	m_JumpAimFireDownAni = szBuf;

	LoadAttackAttribute( "jump_aim_attack", m_JumpAimAttack, rkLoader );
	LoadAttackAttribute( "jump_combo_charge_attack", m_JumpComboChargeAttack, rkLoader );
	LoadAttackAttribute( "jump_combo_dash_attack", m_JumpComboDashAttack, rkLoader );

	m_JumpComboAttackList.clear();
	m_JumpComboFireUpList.clear();
	m_JumpComboFireCenterList.clear();
	m_JumpComboFireDownList.clear();

	m_nMaxJumpSpecialComboCnt = rkLoader.LoadInt_e( "jump_max_special_combo_attack", 0 );

	for ( int i=0; i<m_nMaxJumpSpecialComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "jump_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_JumpComboAttackList.push_back( sAttackAttribute );

		wsprintf_e( szKey, "jump_combo_attack%d_up", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_JumpComboFireUpList.push_back( szBuf );

		wsprintf_e( szKey, "jump_combo_attack%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_JumpComboFireCenterList.push_back( szBuf );

		wsprintf_e( szKey, "jump_combo_attack%d_down", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_JumpComboFireDownList.push_back( szBuf );
	}

	rkLoader.LoadString( "jump_combo_aim_up_ani", "", szBuf, MAX_PATH );	
	m_JumpComboAimUpAni = szBuf;
	rkLoader.LoadString( "jump_combo_aim_center_ani", "", szBuf, MAX_PATH );	
	m_JumpComboAimCenterAni = szBuf;
	rkLoader.LoadString( "jump_combo_aim_down_ani", "", szBuf, MAX_PATH );	
	m_JumpComboAimDownAni = szBuf;

	m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_end_power", 0.0f );
	m_dwJumpSpecialStateTime = rkLoader.LoadInt_e( "jump_loop_time", 0 );
}

ioItem* ioPredatorItem::Clone()
{
	return new ioPredatorItem( *this );
}

ioWeaponItem::WeaponSubType ioPredatorItem::GetSubType() const
{
	return WST_PREDATOR_ITEM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::SetNormalAttack( ioBaseChar *pOwner, int nCurCombo )
{
	ClearCancelInfo();	

	Init();

	DWORD dwCurTime = FRAMEGETTIME();
	int nComboCnt = GetMaxCombo();
	if( !COMPARE( nCurCombo, 0, nComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, nCurCombo );

		m_nNormalCombo = 0;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_WeaponItemState = WS_NONE;
	}
	else
	{
		//공격 준비 애니메이션 설정
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int nAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[nCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( nAniID );
		pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwAttackStartTime = dwCurTime;
		m_nNormalCombo = nCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime( nAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_WeaponItemState = WS_CHECK_CHARGING;
	}
}

void ioPredatorItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponItemState )
	{
	case WS_CHECK_CHARGING:
		CheckCharging( pOwner );
		break;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioPredatorItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_WeaponItemState == WS_CHECK_CHARGING )
			return true;
	}

	return false;
}

bool ioPredatorItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	switch( m_SpecialState )
	{
	case SS_AIM_ATTACK:
	case SS_COMBO_ATTACK:
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );	

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pZone->SetCollisionDir( vDir );
		pZone->SetZoneValue( true, false );
	}

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	if( pArrow )
	{
		D3DXVECTOR3 vAimedDir = ReCalcMoveDir( pOwner, pWeapon );
		pWeapon->SetMoveDir( vAimedDir );
	}

	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	if( pMissile )
	{
		D3DXVECTOR3 vAimedDir = ReCalcMoveDir( pOwner, pMissile );
		pWeapon->SetMoveDir( vAimedDir );
	}

	ioNuclearWeapon *pNuclearWeapon = ToNuclearWeapon( pWeapon );
	if ( pNuclearWeapon )
	{		
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget )
			pNuclearWeapon->SetStartPosition( pTarget->GetWorldPosition(), pStage );
		else
		{
			ioTargetMarker *pTargetMarker = pOwner->GetTargetMarker();
			if ( pTargetMarker )
			{
				ioMapEffect *pEffect = pTargetMarker->GetCurMarker( pStage );
				if ( pEffect )
					pNuclearWeapon->SetStartPosition( pEffect->GetWorldPosition(), pStage );
			}
		}
	}
}

D3DXVECTOR3 ioPredatorItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget )
	{
		m_vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		return m_vAimedDir;
	}

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();
	vTargetPos = pTarget->GetMidPositionByRate();

	D3DXVECTOR3 vAimedDir = vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	m_vAimedDir = vAimedDir;

	return vAimedDir;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

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
	case WS_SPECIAL_STATE_INIT:
		{
			bool bJump;
			rkPacket >> bJump;

			Init();

			m_bSpecialStateJump = bJump;

			if ( m_bSpecialStateJump )
			{
				m_bEnableDownAim = true;
				m_bSpecialStateMove = false;
			}
			else
			{
				m_bEnableDownAim = false;
				m_bSpecialStateMove = true;
			}

			pOwner->SetState( CS_PREDATOR_SPECIAL );
			SetPreDelayState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_PROCESS:
		RecvSpecialState( pOwner, rkPacket );
		break;
	}
}

void ioPredatorItem::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	case SS_FIND_TARGET:
		{
			SetFindTargetState( pOwner, false );
		}
		break;
	case SS_AIMED_TARGET:
		{
			rkPacket >> m_AimedTarget;

			int nDir;
			rkPacket >> nDir;

			m_AimStartDirKey = (ioUserKeyInput::DirKeyInput)nDir;

			SetAimedTargetState( pOwner );
		}
		break;
	case SS_AIM_ATTACK:
		{
			rkPacket >> m_AimedTarget;

			if ( m_bSpecialStateJump )
				SetAimedFireState( pOwner, m_JumpAimAttack );
			else
				SetAimedFireState( pOwner, m_AimAttack );
		}
		break;
	case SS_COMBO_ATTACK:
		{
			rkPacket >> m_AimedTarget;
			rkPacket >> m_nCurSpecialComboCnt;

			if ( m_bSpecialStateJump )
				SetComboAttackState( pOwner, m_JumpComboAttackList );
			else
				SetComboAttackState( pOwner, m_ComboAttackList );
		}
		break;
	case SS_CHECK_COMBO:
		{
			SetCheckComboState( pOwner );
		}
		break;
	case SS_DASH_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if ( m_bSpecialStateJump )
				SetComboSpecialState( pOwner, m_JumpComboDashAttack, SS_DASH_ATTACK, true );
			else
				SetComboSpecialState( pOwner, m_ComboDashAttack, SS_DASH_ATTACK, true );
		};
		break;
	case SS_CHARGE_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if ( m_bSpecialStateJump )
				SetComboSpecialState( pOwner, m_JumpComboChargeAttack, SS_CHARGE_ATTACK, false );
			else
				SetComboSpecialState( pOwner, m_ComboChargeAttack, SS_CHARGE_ATTACK, false );
		};
		break;
	case SS_INIT:
		{
			ClearState( pOwner );
		}
		break;
	case SS_END:
		{
			m_SpecialState = SS_END;			
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		if( m_fCurExtraGauge < m_fMaxExtraGauge )
		{
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		return;
	}
}

void ioPredatorItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

int ioPredatorItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioPredatorItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioPredatorItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioPredatorItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::Init()
{	
	m_WeaponItemState = WS_NONE;
	m_SpecialState = SS_NONE;

	m_dwAttackStartTime = 0;
	m_dwComboAttackStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwAimTrackingStartTime = 0;
	m_dwAimTrackingEndTime = 0;
	m_fCurAimAngle = 0.0f;
	m_dwFireTime = 0;
	m_dwEnableReserveKeyTime = 0;
	m_nCurSpecialComboCnt = 0;
	m_bSetMove = false;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bEnableDownAim = false;
	m_bSpecialStateJump = false;
	m_bSpecialStateMove = false;
	m_dwSpecialStateTime = 0;
}

void ioPredatorItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess()  )	
		return;

	if( m_fCurExtraGauge >= m_fNeedGauge && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetPredatorSpecialState( pOwner, false );
	}
	else
		SetNormalAttackState( pOwner );
}

void ioPredatorItem::SetNormalAttackState( ioBaseChar *pOwner )
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::ClearState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );
	m_AimedTarget.Clear();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if( pBiped )	
		pBiped->ReleaseExtraRotation();

	Init();
}

void ioPredatorItem::SetPredatorSpecialState( ioBaseChar *pOwner, bool bJump )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	Init();

	m_bSpecialStateJump = bJump;

	if ( m_bSpecialStateJump )
	{
		m_bEnableDownAim = true;
		m_bSpecialStateMove = false;
	}
	else
	{
		m_bEnableDownAim = false;
		m_bSpecialStateMove = true;
	}

	pOwner->SetState( CS_PREDATOR_SPECIAL );
	SetPreDelayState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT;
		kPacket << m_bSpecialStateJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPredatorItem::ProcessPredatorSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SpecialState )
	{
	case SS_PRE_DELAY:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				m_dwSpecialStateTime = dwCurTime + m_dwJumpSpecialStateTime;
				pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM, m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, "", "" );

				if ( m_bSpecialStateJump )
					pOwner->SetTargetMarkerRange( m_fJumpMarkerRange );
				else
					pOwner->SetTargetMarkerRange( m_fMarkerRange );

				SetFindTargetState( pOwner, false );
			}
		}
		break;
	case SS_FIND_TARGET:
		{
			if ( m_bSpecialStateJump && m_dwSpecialStateTime < dwCurTime )
			{
				pOwner->SetFallState( true );
				//CheckReserveState( pOwner );
				return;
			}

			if ( FindTarget( pOwner ) )
			{
				if ( pOwner->IsNeedProcess() )
					SetAimedTargetState( pOwner );
				return;
			}

			if ( m_bSpecialStateMove )
				CheckFindTargetMoveAni( pOwner );

			CheckInputKey( pOwner );
		}
		break;
	case SS_AIMED_TARGET:
		{
			if ( m_bSpecialStateJump && m_dwSpecialStateTime < dwCurTime )
			{
				CheckReserveState( pOwner );
				return;
			}

			if ( !FindTarget( pOwner ) )
			{
				if ( pOwner->IsNeedProcess() )
					SetFindTargetState( pOwner, true );
				return;
			}

			if ( m_bSpecialStateJump )
				ProcessTargetTracking( pOwner, m_JumpAimUpAni, m_JumpAimCenterAni, m_JumpAimDownAni );
			else
				ProcessTargetTracking( pOwner, m_AimUpAni, m_AimCenterAni, m_AimDownAni );

			UpdateAimDir( pOwner );

			if ( m_bSpecialStateMove )
				CheckAimedTargetMoveAni( pOwner );

			CheckInputKey( pOwner );
		}
		break;
	case SS_AIM_ATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				CheckReserveState( pOwner );
				return;
			}
			
			if ( m_dwFireTime > 0 )
			{
				if ( m_dwFireTime < dwCurTime )
				{
					if ( m_bSpecialStateJump )
						SetAimedFireAni( pOwner, m_JumpAimFireUpAni, m_JumpAimFireCenterAni, m_JumpAimFireDownAni );
					else
						SetAimedFireAni( pOwner, m_AimFireUpAni, m_AimFireCenterAni, m_AimFireDownAni );
				}
				else
				{
					if (!m_AimedTarget.IsEmpty())
					{
						if ( !CheckTargetValidate( pOwner ) )
							m_AimedTarget.Clear();
					}
					else
						FindTarget( pOwner );

					if ( m_bSpecialStateJump )
						ProcessTargetTracking( pOwner, m_JumpAimUpAni, m_JumpAimCenterAni, m_JumpAimDownAni );
					else
						ProcessTargetTracking( pOwner, m_AimUpAni, m_AimCenterAni, m_AimDownAni );

					UpdateAimDir( pOwner );
				}
			}

			if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
				CheckComboInputKey( pOwner );
		}
		break;
	case SS_CHECK_COMBO:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if( pOwner->IsAttackKeyDown() )
				{
					if( m_dwComboAttackStartTime + m_dwComboChargeTime <= dwCurTime )
					{
						if ( m_bSpecialStateJump )
							SetComboSpecialState( pOwner, m_JumpComboChargeAttack, SS_CHARGE_ATTACK, false );
						else
							SetComboSpecialState( pOwner, m_ComboChargeAttack, SS_CHARGE_ATTACK, false );
					}
				}
				else
				{
					if ( m_bSpecialStateJump )
					{
						if ( m_nCurSpecialComboCnt < m_nMaxJumpSpecialComboCnt )
							SetComboAttackState( pOwner, m_JumpComboAttackList );
						else
							CheckReserveState( pOwner );
					}
					else
					{
						if ( m_nCurSpecialComboCnt < m_nMaxSpecialComboCnt )
							SetComboAttackState( pOwner, m_ComboAttackList );
						else
							CheckReserveState( pOwner );
					}
				}
			}
		}
		break;
	case SS_COMBO_ATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				CheckReserveState( pOwner );
				return;
			}

			if ( m_dwFireTime > 0 )
			{
				if ( m_dwFireTime < dwCurTime )
				{
					if ( m_bSpecialStateJump )
						SetAimedFireAni( pOwner, m_JumpComboFireUpList[m_nCurSpecialComboCnt], m_JumpComboFireCenterList[m_nCurSpecialComboCnt], m_JumpComboFireDownList[m_nCurSpecialComboCnt] );
					else
						SetAimedFireAni( pOwner, m_ComboFireUpList[m_nCurSpecialComboCnt], m_ComboFireCenterList[m_nCurSpecialComboCnt], m_ComboFireDownList[m_nCurSpecialComboCnt] );
				}
				else
				{
					if (!m_AimedTarget.IsEmpty())
					{
						if ( !CheckTargetValidate( pOwner ) )
							m_AimedTarget.Clear();
					}
					else
						FindTarget( pOwner );

					if ( m_bSpecialStateJump )
						ProcessTargetTracking( pOwner, m_JumpComboAimUpAni, m_JumpComboAimCenterAni, m_JumpComboAimDownAni );
					else
						ProcessTargetTracking( pOwner, m_ComboAimUpAni, m_ComboAimCenterAni, m_ComboAimDownAni );

					UpdateAimDir( pOwner );
				}
			}

			if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
				CheckComboInputKey( pOwner );
		}
		break;
	case SS_CHARGE_ATTACK:
	case SS_DASH_ATTACK:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if ( m_dwMotionEndTime < dwCurTime )
				{
					CheckReserveState( pOwner );
					return;
				}
				else
				{
					if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
						ReserveInputKey( pOwner );
				}
			}
		}
		break;
	case SS_END:
		{
			if ( m_bSpecialStateJump )
				pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );
			else
				pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

ioPredatorItem::SpecialState ioPredatorItem::GetSpecialState()
{
	return m_SpecialState;
}

bool ioPredatorItem::IsJumpSpecialState()
{
	return m_bSpecialStateJump;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::CheckInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( !m_bSpecialStateJump )
	{
		if ( pOwner->IsJumpKeyDown() )
		{
			pOwner->SetJumpState();
			return;
		}

		if ( pOwner->IsDefenseKeyDown() )
		{
			pOwner->SetDefenseState();
			return;
		}
	}

	if ( !pOwner->IsAttackKeyDown() )
	{
		SetUseExtraGauge( m_fNeedGauge );

		if ( m_bSpecialStateJump )
			SetAimedFireState( pOwner, m_JumpAimAttack );
		else
			SetAimedFireState( pOwner, m_AimAttack );
		return;
	}
}

void ioPredatorItem::CheckComboInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( CheckInputDash( pOwner ) )
		return;

	if ( pOwner->IsAttackKeyDown() )
	{
		SetCheckComboState( pOwner );
		return;
	}
}

bool ioPredatorItem::CheckInputDash( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( pOwner->IsDirKeyDoubleClick() )
	{
		pOwner->ClearDirDoubleClick();

		ioUserKeyInput::DirKeyInput CurKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( CurKey );

		if ( m_bSpecialStateJump )
			SetComboSpecialState( pOwner, m_JumpComboDashAttack, SS_DASH_ATTACK, true );
		else
			SetComboSpecialState( pOwner, m_ComboDashAttack, SS_DASH_ATTACK, true );

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::SetPreDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	ioHashString szPreAni = m_szWaitPreDelayAni;
	float fTimeRate = m_fWaitPreDelayAniRate;

	if ( m_bSpecialStateJump )
	{
		szPreAni = m_szJumpWaitPreDelayAni;
		fTimeRate = m_fJumpWaitPreDelayAniRate;
	}

	int nAniID = pGrp->GetAnimationIdx( szPreAni );
	if( nAniID == -1 )	
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );	

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0 );
	
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_SpecialState = SS_PRE_DELAY;
}

void ioPredatorItem::SetFindTargetState( ioBaseChar *pOwner, bool bSendNetwork )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	ioHashString szLoopAni = m_szWaitLoopAni;
	if ( m_bSpecialStateJump )
		szLoopAni = m_szJumpWaitLoopAni;

	if( m_bSpecialStateMove && pOwner->IsSettedDirection() )
	{
		pGrp->SetLoopAni( m_szWaitMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fWaitMoveAniRate );
		m_bSetMove = true;
	}
	else
	{		
		pGrp->SetLoopAni( szLoopAni, FLOAT100 );
		m_bSetMove = false;
	}	

	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if( pBiped )	
		pBiped->ReleaseExtraRotation();

	m_SpecialState = SS_FIND_TARGET;

	if ( pOwner->GetMarkerState() != ioTargetMarker::MS_READY )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	if( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( false );

	if ( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_FIND_TARGET;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPredatorItem::SetAimedTargetState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	ioHashString szLoopAni = m_szAimLoopAni;
	if ( m_bSpecialStateJump )
		szLoopAni = m_szJumpAimLoopAni;

	if ( m_bSpecialStateMove && pOwner->IsNeedProcess() )
		m_AimStartDirKey = pOwner->GetCurDirKey();

	if( m_bSpecialStateMove && pOwner->IsSettedDirection() )
	{
		int nAniID = 0;
		nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Front);
		pGrp->ClearLoopAni( nAniID, FLOAT100 );
		nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Back);
		pGrp->ClearLoopAni( nAniID, FLOAT100 );
		nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Left);
		pGrp->ClearLoopAni( nAniID, FLOAT100 );
		nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Right);
		pGrp->ClearLoopAni( nAniID, FLOAT100 );

		ioUserKeyInput::DirKeyInput eXformDir;
		eXformDir = pOwner->GetXformDir( m_AimStartDirKey );

		switch( eXformDir )
		{
		case ioUserKeyInput::DKI_UP:
			pGrp->SetLoopAni( m_szAimMoveAni_Front, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
			break;
		case ioUserKeyInput::DKI_RIGHTUP:
		case ioUserKeyInput::DKI_RIGHT:
		case ioUserKeyInput::DKI_RIGHTDOWN:
			pGrp->SetLoopAni( m_szAimMoveAni_Right, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
			break;
		case ioUserKeyInput::DKI_DOWN:
			pGrp->SetLoopAni( m_szAimMoveAni_Back, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
			break;
		case ioUserKeyInput::DKI_LEFTDOWN:
		case ioUserKeyInput::DKI_LEFT:
		case ioUserKeyInput::DKI_LEFTUP:
			pGrp->SetLoopAni( m_szAimMoveAni_Left, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
			break;
		}

		m_bSetMove = true;
	}
	else
	{		
		pGrp->SetLoopAni( szLoopAni, FLOAT100 );

		m_bSetMove = false;
	}

	if ( m_bSpecialStateJump )
	{
		pGrp->SetLoopAni( m_JumpAimUpAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpAimDownAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpAimCenterAni, 0.0f, FLOAT1 );
	}
	else
	{
		pGrp->SetLoopAni( m_AimUpAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_AimDownAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_AimCenterAni, 0.0f, FLOAT1 );
	}

	m_SpecialState = SS_AIMED_TARGET;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, false );

	if ( pOwner->GetMarkerState() != ioTargetMarker::MS_AIMED )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( true );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_AIMED_TARGET;
		kPacket << m_AimedTarget;
		kPacket << (int)m_AimStartDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPredatorItem::SetAimedFireState( ioBaseChar *pOwner, const AttackAttribute& AimAttack )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int	nAniID = pGrp->GetAnimationIdx( AimAttack.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate   = AimAttack.m_fAttackAniRate;
	DWORD dwPreDelay  = AimAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if ( m_bSpecialStateJump )
	{
		pGrp->SetLoopAni( m_JumpAimUpAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpAimDownAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpAimCenterAni, 0.0f, FLOAT1 );
	}
	else
	{
		pGrp->SetLoopAni( m_AimUpAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_AimDownAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_AimCenterAni, 0.0f, FLOAT1 );
	}

	pOwner->RefreshFireTimeList( nAniID, AimAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( AimAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate + dwPreDelay;

	m_dwEnableReserveKeyTime = pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwEnableReserveKeyTime > 0 )
		m_dwEnableReserveKeyTime += dwCurTime;

	m_dwFireTime = dwCurTime;
	m_dwFireTime += pGrp->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;

	if ( m_bSpecialStateJump )
		ProcessTargetTracking( pOwner, m_JumpAimUpAni, m_JumpAimCenterAni, m_JumpAimDownAni );
	else
		ProcessTargetTracking( pOwner, m_AimUpAni, m_AimCenterAni, m_AimDownAni );

	m_SpecialState = SS_AIM_ATTACK;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_AIM_ATTACK;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPredatorItem::SetAimedFireAni( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	if( m_vAimedDir.y >= 0.0f )
		AimedFireToUp( pGrp, stUp, stCenter, fWeight, FLOAT1, 0, true );
	else
		AimedFireToDown( pGrp, stCenter, stDown, fWeight, FLOAT1, 0, true );

	m_dwFireTime = 0;
}

void ioPredatorItem::SetComboAttackState( ioBaseChar *pOwner, const AttributeList& ComboList )
{
	if ( !pOwner )
		return;

	pOwner->InitActionStopDelay( true );

	if ( !COMPARE( m_nCurSpecialComboCnt, 0, (int)ComboList.size() ) )
		return;

	const AttackAttribute& ComboAttack = ComboList[m_nCurSpecialComboCnt];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int	nAniID = pGrp->GetAnimationIdx( ComboAttack.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate   = ComboAttack.m_fAttackAniRate;
	DWORD dwPreDelay  = ComboAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if ( m_bSpecialStateJump )
	{
		pGrp->SetLoopAni( m_JumpComboAimUpAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpComboAimDownAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpComboAimCenterAni, 0.0f, FLOAT1 );
	}
	else
	{
		pGrp->SetLoopAni( m_ComboAimUpAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ComboAimDownAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ComboAimCenterAni, 0.0f, FLOAT1 );
	}

	pOwner->RefreshFireTimeList( nAniID, ComboAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( ComboAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate + dwPreDelay;

	m_dwEnableReserveKeyTime = pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwEnableReserveKeyTime > 0 )
		m_dwEnableReserveKeyTime += dwCurTime;

	m_dwFireTime = dwCurTime;
	m_dwFireTime += pGrp->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;

	if ( m_bSpecialStateJump )
		ProcessTargetTracking( pOwner, m_JumpComboAimUpAni, m_JumpComboAimCenterAni, m_JumpComboAimDownAni );
	else
		ProcessTargetTracking( pOwner, m_ComboAimUpAni, m_ComboAimCenterAni, m_ComboAimDownAni );

	m_SpecialState = SS_COMBO_ATTACK;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_COMBO_ATTACK;
		kPacket << m_AimedTarget;
		kPacket << m_nCurSpecialComboCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPredatorItem::SetCheckComboState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_nCurSpecialComboCnt++;
	m_dwComboAttackStartTime = FRAMEGETTIME();
	m_SpecialState = SS_CHECK_COMBO;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_CHECK_COMBO;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPredatorItem::SetComboSpecialState( ioBaseChar *pOwner, const AttackAttribute& Special, SpecialState eSpecialState, bool bRemoveMarker )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( Special.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = Special.m_fAttackAniRate;
	DWORD dwPreDelay = Special.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( nAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( Special.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, Special.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwEnableReserveKeyTime = pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwEnableReserveKeyTime > 0 )
		m_dwEnableReserveKeyTime += dwCurTime;

	m_SpecialState = eSpecialState;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	if ( bRemoveMarker )
		pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if( pBiped )	
		pBiped->ReleaseExtraRotation();

	if( pOwner->IsNeedProcess() )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );

		DWORD dwTrackingTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << m_SpecialState;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetTargetRot();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioPredatorItem::IsEnableMoveState()
{
	if ( !m_bSpecialStateMove )
		return false;

	switch ( m_SpecialState )
	{
	case SS_FIND_TARGET:
	case SS_AIMED_TARGET:
		return true;
	}

	return false;
}

void ioPredatorItem::CheckFindTargetMoveAni( ioBaseChar *pOwner )
{
	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir )
		{	
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szWaitMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fWaitMoveAniRate );

			m_bSetMove = true;
		}
	}
	else
	{		
		if( m_bSetMove )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szWaitLoopAni, FLOAT100 );

			m_bSetMove = false;
		}
	}
}

void ioPredatorItem::CheckAimedTargetMoveAni( ioBaseChar *pOwner )
{
	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir )
		{	
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );

			int nAniID = 0;
			nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Front);
			pGrp->ClearLoopAni( nAniID, FLOAT100 );
			nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Back);
			pGrp->ClearLoopAni( nAniID, FLOAT100 );
			nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Left);
			pGrp->ClearLoopAni( nAniID, FLOAT100 );
			nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Right);
			pGrp->ClearLoopAni( nAniID, FLOAT100 );

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_AimStartDirKey );

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_szAimMoveAni_Front, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_szAimMoveAni_Right, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_szAimMoveAni_Back, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_szAimMoveAni_Left, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
				break;
			}

			m_bSetMove = true;
		}
	}
	else
	{		
		if( m_bSetMove )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );

			int nAniID = 0;
			nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Front);
			pGrp->ClearLoopAni( nAniID, FLOAT100, true );
			nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Back);
			pGrp->ClearLoopAni( nAniID, FLOAT100, true );
			nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Left);
			pGrp->ClearLoopAni( nAniID, FLOAT100, true );
			nAniID = pGrp->GetAnimationIdx(m_szAimMoveAni_Right);
			pGrp->ClearLoopAni( nAniID, FLOAT100, true );

			m_bSetMove = false;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioPredatorItem::FindTarget( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioHashString szTarget;
	float fAngle = m_fAimAngle;
	float fMinRange = m_fAimMinRange;
	float fRange = m_fAimRange;

	if ( m_bSpecialStateJump )
	{
		fAngle = m_fJumpAimAngle;
		fMinRange = m_fJumpAimMinRange;
		fRange = m_fJumpAimRange;
	}

	if( pOwner->CheckAimedTarget( fAngle, fMinRange, fRange, szTarget, m_bEnableDownAim ) )
	{
		if ( m_AimedTarget == szTarget )
			return true;

		m_AimedTarget = szTarget;
		pOwner->SetTargetMarkerTarget( m_AimedTarget, false );

		if ( pOwner->GetMarkerState() != ioTargetMarker::MS_AIMED )
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

		return true;
	}
	else
	{
		m_AimedTarget.Clear();

		if ( pOwner->GetMarkerState() != ioTargetMarker::MS_READY )
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

		return false;
	}
}

bool ioPredatorItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;

	float fAngle = m_fAimAngle;
	float fRange = m_fAimRange;

	if ( m_bSpecialStateJump )
	{
		fAngle = m_fJumpAimAngle;
		fRange = m_fJumpAimRange;
	}

	float fCurAngle = ioMath::ArrangeHead( fAngle );
	float fHalfValue = cosf( DEGtoRAD( fCurAngle * FLOAT05 ) );
	float fTargetRangeSq = fRange * fRange;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	if( !m_bEnableDownAim && pTarget->IsApplyDownState(false) )
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

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::ProcessTargetTracking( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp = pGrp->GetLoopInPlay( stUp );
	ioAnimation *pCenter = pGrp->GetLoopInPlay( stCenter );
	ioAnimation *pDown = pGrp->GetLoopInPlay( stDown );

	if( !pUp || !pCenter || !pDown )
		return;

	D3DXVECTOR3 vAimedDir;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )
	{
		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}

	m_vAimedDir = vAimedDir;
}

float ioPredatorItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioPredatorItem::UpdateAimDir( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if( !pBiped )	
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bStateCheck = false;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
		vTargetPos = pTarget->GetWorldPosition();

	float fRotAngle = 0.0f;

	bool bTarget = true;
	if( pTarget && !pTarget->IsEnableAimTarget( m_bEnableDownAim ) )
		bTarget = false;
	else if( m_bAimedBall && !pBall )
		bTarget = false;
	else if( !pTarget && !m_bAimedBall )
		bTarget = false;

	if( !bTarget )
	{
		float fTotal, fGap;
		DWORD dwEndTime = m_dwAimTrackingEndTime + m_dwAimBipeReturnTime;
		fTotal = fGap = 0.0f;

		fTotal = FLOAT500;
		if( dwEndTime > dwCurTime )
			fGap = (float)(dwEndTime - dwCurTime);
		else
			m_dwAimTrackingEndTime = 0;

		if( m_dwAimTrackingEndTime == 0 )
		{
			m_fCurAimAngle = 0.0f;
			pBiped->ReleaseExtraRotation();
			return;
		}

		float fRate = 0.0f;
		if( fTotal > 0.0f )
			fRate = fGap / fTotal;

		fRotAngle = m_fCurAimAngle * fRate;
	}
	else
	{
		D3DXVECTOR3 vSightDir = vTargetPos - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vSightDir, &vSightDir );

		D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
		D3DXVec3Normalize( &vXZDir, &vXZDir );

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

		float fCurYaw, fTargetYaw;
		fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
		fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
		fCurYaw = RADtoDEG( ioMath::QuaterToYaw(pOwner->GetTargetRot()) );
		fCurYaw = ioMath::ArrangeHead( fCurYaw );

		m_fCurAimAngle = DEGtoRAD( ioMath::AngleGap( fCurYaw, fTargetYaw ) );

		float fAimTimeRate = 1.0;
		DWORD dwTimeGap = dwCurTime - m_dwAimTrackingStartTime;
		if( dwTimeGap < m_dwAimBipeMoveTime )
			fAimTimeRate = (float)dwTimeGap / (float)m_dwAimBipeMoveTime;

		fRotAngle = m_fCurAimAngle * fAimTimeRate;
	}

	D3DXQUATERNION qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, fRotAngle, 0.0f );
	pBiped->SetExtraRotation( qtExtraRot, true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::AimedFireToUp( ioEntityGroup *pGrp, ioHashString szFireUp, ioHashString szFireCenter, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	if( fWeight > 0.0f )
		pGrp->SetActionAni( szFireUp, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );

	pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
}

void ioPredatorItem::AimedFireToDown( ioEntityGroup *pGrp, ioHashString szFireCenter, ioHashString szFireDown, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	if( fWeight > 0.0f )
		pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate,0.0f, dwPreDelay, bHold, bAniFx1 );

	pGrp->SetActionAni( szFireDown, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPredatorItem::ReserveInputKey(ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

void ioPredatorItem::CheckReserveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_bAttackKeyReserved )
	{
		SetReserveAttack( pOwner );
		return;
	}
	else if( m_bDefenseKeyReserved )
	{
		SetReserveDefence( pOwner );
		return;
	}
	else if ( m_bJumpKeyReserved )
	{
		SetReserveJump( pOwner );
		return;
	}

	m_SpecialState = SS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioPredatorItem::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioPredatorItem::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioPredatorItem::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioPredatorSpecialState::ioPredatorSpecialState()
{
}

ioPredatorSpecialState::~ioPredatorSpecialState()
{
}

void ioPredatorSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioPredatorSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPredatorItem )
		pPredatorItem->ClearState( pOwner );
}

bool ioPredatorSpecialState::IsEnableBlowWounded( ioBaseChar *pOwner )
{ 
	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPredatorItem && pPredatorItem->IsJumpSpecialState() )
		return true;

	return false;
}

void ioPredatorSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPredatorItem )
		pPredatorItem->ProcessPredatorSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioPredatorSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPredatorItem && pPredatorItem->IsJumpSpecialState() )
		return;

	ioSpecialStateBase::CheckFallStateChange( pOwner, fMinFallHeightRate, fBottomHeight, bBottomMaxCheck, fMaxBottomHeight );
}

void ioPredatorSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pPredatorItem )
		return;

	switch ( pPredatorItem->GetSpecialState() )
	{
	case ioPredatorItem::SS_FIND_TARGET:
		pOwner->ChangeDirectionByInputDir( false );
		break;
	case ioPredatorItem::SS_AIMED_TARGET:
		pOwner->ChangeDirectionByInputDir( true );
		break;
	}
}

bool ioPredatorSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetWeapon() );
	if( pPredatorItem && pPredatorItem->IsEnableAttackDashCheck( pOwner, true ) )
		return true;

	return false;
}

bool ioPredatorSpecialState::IsNoDropState( ioBaseChar* pOwner )
{ 
	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPredatorItem && pPredatorItem->IsJumpSpecialState() )
		return true;

	return false;
}