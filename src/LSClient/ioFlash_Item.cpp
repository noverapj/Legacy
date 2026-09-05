
#include "stdafx.h"

#include "ioFlash_Item.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

#include "ioExtendDash.h"
#include "ioNpcChar.h"

ioFlash_Item::ioFlash_Item()
{	
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_fCurGauge = 0.0f;

	Init();
}

ioFlash_Item::ioFlash_Item( const ioFlash_Item &rhs )
	: ioWeaponItem( rhs ),
	  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_fMaxGauge( rhs.m_fMaxGauge ),
	  m_fNeedGauge( rhs.m_fNeedGauge ),  
	  m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	  m_szRunDelayAni( rhs.m_szRunDelayAni ),
	  m_fRunDelayAniRate( rhs.m_fRunDelayAniRate ),
	  m_dwRunDelay_PreDelay( rhs.m_dwRunDelay_PreDelay ),
	  m_szRunLoopAniName( rhs.m_szRunLoopAniName ),
	  m_szRunEndAni( rhs.m_szRunEndAni ),
	  m_fRunEndAniRate( rhs.m_fRunEndAniRate ),
	  m_dwRunEnd_PreDelay( rhs.m_dwRunEnd_PreDelay ),
	  m_nRunAttachWeaponNum( rhs.m_nRunAttachWeaponNum ),
	  m_nRunEndAttackWeaponNum( rhs.m_nRunEndAttackWeaponNum ),
	  m_nRunEndAttackResistNum( rhs.m_nRunEndAttackResistNum ),
	  m_szRunEndAttackWoundedAni( rhs.m_szRunEndAttackWoundedAni ),
	  m_dwRunEndAttackWoundedDuration( rhs.m_dwRunEndAttackWoundedDuration ),
	  m_bRunEndAttackWoundedLoop( rhs.m_bRunEndAttackWoundedLoop ),
	  m_vRunEndAttackOffset( rhs.m_vRunEndAttackOffset ),
	  m_fRunSpeed( rhs.m_fRunSpeed ),
	  m_dwRunTime( rhs.m_dwRunTime ),
	  m_bEnableDownAim( rhs.m_bEnableDownAim ),
	  m_fAimAngle( rhs.m_fAimAngle ),
	  m_fAimRange( rhs.m_fAimRange ),
	  m_fMinRange( rhs.m_fMinRange ),
	  m_fMarkerRange( rhs.m_fMarkerRange ),
	  m_fMaxMarkerRangeRate( rhs.m_fMaxMarkerRangeRate ),
	  m_fMaxAimRangeRate( rhs.m_fMaxAimRangeRate ),
	  m_fMaxAimAngleRate( rhs.m_fMaxAimAngleRate ),
	  m_nMaxTeleportCnt( rhs.m_nMaxTeleportCnt ),
	  m_TeleportList( rhs.m_TeleportList ),
	  m_fTeleportUseGauge( rhs.m_fTeleportUseGauge ),
	  m_fTeleportRange( rhs.m_fTeleportRange ),
	  m_fMaxMoveDist( rhs.m_fMaxMoveDist ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_fTeleportHeightGap( rhs.m_fTeleportHeightGap ),
	  m_bUseKeyMoveFlashSpecialState( rhs.m_bUseKeyMoveFlashSpecialState ),
	  m_szFlashSpecialMoveAni( rhs.m_szFlashSpecialMoveAni ),
	  m_fFlashSpecialMoveAniRate( rhs.m_fFlashSpecialMoveAniRate ),
	  m_dwJumpFlashSpecialCheckTime( rhs.m_dwJumpFlashSpecialCheckTime ),
	  m_bUseExtendDashAutoTargetValue( rhs.m_bUseExtendDashAutoTargetValue ),
	  m_bUseCheckTarget2State( rhs.m_bUseCheckTarget2State ),
	  m_szJumpFlashLoopAni( rhs.m_szJumpFlashLoopAni ),
	  m_fJumpFlashLoopAniRate( rhs.m_fJumpFlashLoopAniRate )
{
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_fCurGauge = 0.0f;

	Init();
}

ioFlash_Item::~ioFlash_Item()
{
}

void ioFlash_Item::LoadProperty( ioINILoader &rkLoader )
{
	//기본 파일 읽기
	ioWeaponItem::LoadProperty( rkLoader );

	m_AttackReadyAniList.clear();

	char szBuf[MAX_PATH];

	//공격 준비 애니메이션
	int iMaxCombo = GetMaxCombo();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}
		
	//게이지 설정
	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );

	//Run애니메이션
	rkLoader.LoadString_e( "run_delay_ani_name", "", szBuf, MAX_PATH );
	m_szRunDelayAni = szBuf;
	m_fRunDelayAniRate = rkLoader.LoadFloat_e( "run_delay_ani_rate", FLOAT1 );
	m_dwRunDelay_PreDelay = rkLoader.LoadInt_e( "run_delay_ani_predelay", 0 );

	rkLoader.LoadString_e( "run_loop_ani_name", "", szBuf, MAX_PATH );
	m_szRunLoopAniName = szBuf;

	rkLoader.LoadString_e( "run_end_ani_name", "", szBuf, MAX_PATH );
	m_szRunEndAni = szBuf;
	m_fRunEndAniRate = rkLoader.LoadFloat_e( "run_end_ani_rate", FLOAT1 );
	m_dwRunEnd_PreDelay = rkLoader.LoadInt_e( "run_end_ani_predelay", 0 );

	//Run속도
	m_fMaxMoveDist = rkLoader.LoadFloat_e( "run_dist", 0.0f );
	m_fRunSpeed = rkLoader.LoadFloat_e( "run_speed", 0.0f );
	m_dwRunTime = rkLoader.LoadInt_e( "run_duration", 0 );

	//Run 공격
	m_nRunAttachWeaponNum = rkLoader.LoadInt_e( "run_attach_type", NO_WEAPON_ATTRIBUTE );
	m_nRunEndAttackWeaponNum = rkLoader.LoadInt_e( "run_end_attack_type", NO_WEAPON_ATTRIBUTE);
	m_nRunEndAttackResistNum = rkLoader.LoadInt_e( "run_end_attack_resist", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "run_end_attack_wounded", "", szBuf, MAX_PATH );
	m_szRunEndAttackWoundedAni = szBuf;
	
	m_dwRunEndAttackWoundedDuration = rkLoader.LoadInt_e( "run_end_attack_wounded_time", 0);
	m_bRunEndAttackWoundedLoop = rkLoader.LoadBool_e( "run_end_attack_wounded_loop_ani", false );

	m_vRunEndAttackOffset.x = rkLoader.LoadFloat_e( "run_end_attack_offset_x", 0.0f );
	m_vRunEndAttackOffset.y = rkLoader.LoadFloat_e( "run_end_attack_offset_y", 0.0f );
	m_vRunEndAttackOffset.z = rkLoader.LoadFloat_e( "run_end_attack_offset_z", 0.0f );

	//조준
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );
		
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );
	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );

	m_fMaxMarkerRangeRate = rkLoader.LoadFloat_e( "max_mark_range_rate", FLOAT1 );
	m_fMaxAimRangeRate = rkLoader.LoadFloat_e( "max_aim_range_rate", FLOAT1 );
	m_fMaxAimAngleRate = rkLoader.LoadFloat_e( "max_aim_angle_rate", FLOAT1 );

	m_nMaxTeleportCnt = rkLoader.LoadInt_e( "max_teleport_cnt", 0);

	m_fEndJumpPower = rkLoader.LoadFloat_e( "teleport_end_jump_power", 0.0f );

	//텔레포트 공격 옵션 읽기
	LoadTeleportAttribute( rkLoader );

	m_bUseKeyMoveFlashSpecialState = rkLoader.LoadBool_e( "use_key_move_flash_special_state", false );
	rkLoader.LoadString_e( "flash_special_move_ani", "", szBuf, MAX_PATH );
	m_szFlashSpecialMoveAni = szBuf;
	m_fFlashSpecialMoveAniRate = rkLoader.LoadFloat_e( "flash_special_move_ani_rate", FLOAT1 );
	m_dwJumpFlashSpecialCheckTime = (DWORD)rkLoader.LoadInt_e( "jump_flash_special_check_time", 0 );
	m_bUseExtendDashAutoTargetValue = rkLoader.LoadBool_e( "use_extend_dash_auto_traget_value", false );
	m_bUseCheckTarget2State = rkLoader.LoadBool_e( "use_check_target2_state", false );

	rkLoader.LoadString_e( "jump_flash_loop_ani", "", szBuf, MAX_PATH );
	m_szJumpFlashLoopAni = szBuf;
	m_fJumpFlashLoopAniRate = rkLoader.LoadFloat_e( "jump_flash_loop_ani_rate", FLOAT1 );
}

ioItem* ioFlash_Item::Clone()
{
	return new ioFlash_Item( *this );
}

ioWeaponItem::WeaponSubType ioFlash_Item::GetSubType() const
{
	return WST_FLASHITEM;
}

//공격 준비 애니메이션 설정
void ioFlash_Item::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();	
	
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
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

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

//공격 상태 확인 (실제 프로세스)
void ioFlash_Item::CheckNormalAttack( ioBaseChar *pOwner )
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

//생성한 무기에 옵션 변경
void ioFlash_Item::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

//데이터 갱신 (게이지 회복)
void ioFlash_Item::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

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

//아이템 동기화 부분
void ioFlash_Item::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
			InitSpecialState();

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
			pOwner->ClearReservedSliding();
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurMoveSpeed( 0.0f );

			rkPacket >> m_bJumpFlashSpecialState;

			if ( m_bJumpFlashSpecialState )
			{
				if ( !m_szJumpFlashLoopAni.IsEmpty() )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if ( pGrp )
					{
						pGrp->ClearAllActionAni( FLOAT100, true );
						pGrp->ClearAllLoopAni( FLOAT100, true );
						float fCurRate = FLOAT1;
						if( m_fFlashSpecialMoveAniRate > 0.0f )
							fCurRate = m_fJumpFlashLoopAniRate;
						pGrp->SetLoopAni( m_szJumpFlashLoopAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );
					}

				}
				if ( m_dwJumpFlashSpecialCheckTime > 0 )
					m_dwJumpFlashSpecialCheckChargeEndTime = FRAMEGETTIME() + m_dwJumpFlashSpecialCheckTime;
			}
			else if ( m_bUseKeyMoveFlashSpecialState )
			{
				float fCurRate = FLOAT1;
				if( m_fFlashSpecialMoveAniRate > 0.0f )
					fCurRate = m_fFlashSpecialMoveAniRate;

				ioEntityGroup *pGrp = pOwner->GetGroup();
				if ( pGrp )
				{
					if ( !pOwner->IsSettedDirection() )
						pGrp->SetActionAni( m_AttackReadyAniList.front(), FLOAT100, FLOAT1, FLOAT1/fCurRate );
					else
					{
						m_bSetFlashSpecialMoveAni = true;
						pGrp->SetActionAni( m_szFlashSpecialMoveAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );
					}
				}
			}

			pOwner->SetState( CS_FLASH_SPECIAL );
			m_FlashSpecialState = FMSS_FIND_TARGET;
		}
		break;
	case WS_SPECIAL_STATE_PROCESS:
		{
			RecvSpecialState( pOwner, rkPacket );
		}
		break;
	case WS_SPECIAL_FIND:
		{
			m_AimedTarget.Clear();
			m_FlashSpecialState = FMSS_FIND_TARGET;
			m_bSetTarget = false;
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}
		break;
	}
}

void ioFlash_Item::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;
	
	bool bUsedExtra;
	rkPacket >> bUsedExtra;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true );

	pOwner->ApplyAutoTargetInfo( rkPacket );
}

//동기화
void ioFlash_Item::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	case FMSS_FIND_TARGET:
		{
			rkPacket >> m_AimedTarget;
			rkPacket >> m_bAimedBall;
			if ( m_bUseCheckTarget2State )
				m_FlashSpecialState = FMSS_CHECK_CHARGING2;
			else
				m_FlashSpecialState = FMSS_CHECK_CHARGING;

			if ( !m_AimedTarget.IsEmpty() )
			{
				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			}
		}
		break;
	case FMSS_CHAR_ROT:
		{
			int nInputKey;
			rkPacket >> nInputKey;

			m_InputDirKey = (ioUserKeyInput::DirKeyInput)nInputKey;
			pOwner->SetTargetRotToDirKey( m_InputDirKey );

			m_AimedTarget.Clear();
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		}
		break;
	case FMSS_CREATE_ATTACH_WEAPON:
		{
			rkPacket >> m_dwRunWeaponBaseIndex;

			//무기생성 (몸에 달려있는 무기)
			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = m_nRunAttachWeaponNum;
			kFireTime.iResistanceIdx = 0;
			kFireTime.szWoundedAni  = "";
			kFireTime.dwWoundedAniDuration = 0;
			kFireTime.bLoopWoundedAni = false;

			kFireTime.dwWeaponIdx = m_dwRunWeaponBaseIndex;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
		}
		break;
	case FMSS_RUN_FIRE:
		{
			rkPacket >> m_fCurChargeRate;
			rkPacket >> m_AimedTarget;
			rkPacket >> m_vTargetDir;
			//게이지 감소
			DecreaseGauge( m_fNeedGauge );			
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
			m_bSetMoveAttack = true;
			SetRunDelayState( pOwner );
		}
		break;
	case FMSS_MOVE_END:
		{
			SetRunEndState( pOwner, false );
		}
		break;
	case FMSS_RUN_END_INPUT:
		{
			rkPacket >> m_dwTeleportWeaponBaseIndex;
			rkPacket >> m_bCheckMove;
			m_bEnableTeleport = false;
			DecreaseGauge( m_fTeleportUseGauge );
			if ( m_bCheckMove )
			{
				m_bCheckMove = SetTeleportMove( pOwner );
			}
			SetTeleportState( pOwner );
		}
		break;
	case FMSS_ENABLE_TELEPORT:
		{
			rkPacket >> m_bEnableTeleport;
		}
		break;
	case FMSS_SET_TARGET:
		{
			rkPacket >> m_szTeleportTargetName;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			SetRunEndState( pOwner, true );
		}
		break;
	case FMSS_TELEPORT:
		{
			rkPacket >> m_bCheckMove;
			DecreaseGauge( m_fTeleportUseGauge );
			if ( m_bCheckMove )
			{
				m_bCheckMove = SetTeleportMove( pOwner );
			}
			SetTeleportState( pOwner );
		}
		break;
	case FMSS_TELEPORT_MOVE:
		{
			D3DXVECTOR3 vPos, vDir;
			rkPacket >> vPos;
			rkPacket >> vDir;

			//적용
			pOwner->SetWorldPosition( vPos );

			//회전
			pOwner->SetTargetRotToDir( vDir, true );
		}
		break;
	case FMSS_END:
		{
			m_FlashSpecialState = FMSS_END;			
		}
		break;
	case FMSS_END_JUMP:
		{
			pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, true );
		}
		break;
	}
}

//차징 공격 상태인가
bool ioFlash_Item::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_WeaponItemState == WS_CHECK_CHARGING )
			return true;
	}

	return false;
}

//게이지 설정 및 데이터 얻기
int ioFlash_Item::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioFlash_Item::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioFlash_Item::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioFlash_Item::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioFlash_Item::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioFlash_Item::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioFlash_Item::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioFlash_Item::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioFlash_Item::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

//게이지 사용
void ioFlash_Item::DecreaseGauge( float fNeedGauge )
{
	m_fCurGauge -= fNeedGauge;
	m_fCurGauge = (float)max( 0.f, min( m_fCurGauge, m_fMaxGauge ) );
}

//장비 장착 시
void ioFlash_Item::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM, m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );
	pOwner->SetTargetMarkerRange( m_fMarkerRange );
}

//장비 해제 시
void ioFlash_Item::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );
	ioWeaponItem::OnReleased( pOwner );
}

////////////////////////////////////////////////////////////////////////////
///////////기본 공격 상태///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

//초기화
void ioFlash_Item::Init()
{	
	m_WeaponItemState = WS_NONE;
	m_dwAttackStartTime = 0;
	m_dwCheckRunTime = 0;

	m_FlashSpecialState = FMSS_NONE;
	
	InitSpecialState();

	m_nCurTeleportCnt = 0;
	m_dwRunWeaponBaseIndex = 0;
	m_szTeleportTargetName.Clear();
	m_dwTeleportWeaponBaseIndex = 0;
	m_dwTeleportAttackEndTime = 0;
	m_dwTeleportReserveTime = 0;
	m_bEnableTeleport = true;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	m_bCheckMove = true;
}

void ioFlash_Item::InitSpecialState()
{
	m_AimedTarget.Clear();
	m_InputDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetMoveAttack = false;
	m_bSetTarget = false;
	m_vTargetDir = ioMath::VEC3_ZERO;
	m_fCurMoveDist = 0.0f;
	m_bJumpFlashSpecialState = false;
	m_bSetFlashSpecialMoveAni = false;
	m_dwJumpFlashSpecialCheckChargeEndTime = 0;
	m_dwSpecialStateStartTime = 0;
}

//차징 확인
void ioFlash_Item::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess()  )	
		return;

	//게이지 확인
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && m_fCurGauge >= m_fNeedGauge )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			//특별 상태로 변경
			SetFlashSpecialState( pOwner, false );
		}
	}
	else
	{
		//일반 공격
		SetNormalAttackState( pOwner );
	}
}

//기본 공격 상태로 변경
void ioFlash_Item::SetNormalAttackState( ioBaseChar *pOwner )
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

//떠있는 상태 인가
bool ioFlash_Item::IsFloatState( ioBaseChar *pOwner )
{	
	if ( !pOwner )
		return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );

	if ( fabs( fMapHeight - vPos.y ) >= 20.0f )
	{
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////
///////////////////특별 상태////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//특별 상태 초기화
void ioFlash_Item::ClearState( ioBaseChar *pOwner )
{
	m_FlashSpecialState = FMSS_NONE;
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	InitSpecialState();
}

//게이지 사용이 가능한가?
bool ioFlash_Item::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

//현재 특별 상태
ioFlash_Item::FlashSpecialState ioFlash_Item::GetFlashSpecialState()
{
	return m_FlashSpecialState;
}

//캐릭터를 특별 상태로 변경 (공격 상태에서 특별 상태로 변경)
void ioFlash_Item::SetFlashSpecialState( ioBaseChar *pOwner, bool bJump )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	InitSpecialState();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	m_bJumpFlashSpecialState = bJump;
	
	if ( m_bJumpFlashSpecialState )
	{
		if ( !m_szJumpFlashLoopAni.IsEmpty() )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if ( pGrp )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				float fCurRate = FLOAT1;
				if( m_fFlashSpecialMoveAniRate > 0.0f )
					fCurRate = m_fJumpFlashLoopAniRate;
				pGrp->SetLoopAni( m_szJumpFlashLoopAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			}
			
		}
		if ( m_dwJumpFlashSpecialCheckTime > 0 )
			m_dwJumpFlashSpecialCheckChargeEndTime = FRAMEGETTIME() + m_dwJumpFlashSpecialCheckTime;
	}
	else if ( m_bUseKeyMoveFlashSpecialState )
	{
		float fCurRate = FLOAT1;
		if( m_fFlashSpecialMoveAniRate > 0.0f )
			fCurRate = m_fFlashSpecialMoveAniRate;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if ( pGrp )
		{
			if ( !pOwner->IsSettedDirection() )
				pGrp->SetActionAni( m_AttackReadyAniList.front(), FLOAT100, FLOAT1, FLOAT1/fCurRate );
			else
			{
				m_bSetFlashSpecialMoveAni = true;
				pGrp->SetLoopAni( m_szFlashSpecialMoveAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			}
		}
	}

	m_dwSpecialStateStartTime = FRAMEGETTIME();

	pOwner->SetState( CS_FLASH_SPECIAL );
	m_FlashSpecialState = FMSS_FIND_TARGET;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT;
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//대쉬 차지시 설정
void ioFlash_Item::SetDashCharge( ioBaseChar *pOwner )
{
	Init();
	CheckFindTarget( pOwner );
	CheckRotation( pOwner );
	DecreaseGauge( m_fNeedGauge );			
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	SetRunDelayState( pOwner );
	CreateAttachWeapon( pOwner );
}

//캐릭터 특별 상태 프로세스
void ioFlash_Item::ProcessFlashSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	CheckChargeRate();
	switch( m_FlashSpecialState )
	{
	case FMSS_FIND_TARGET:
		{
			CheckFindTarget( pOwner );
			if ( m_bUseCheckTarget2State )
				m_FlashSpecialState = FMSS_CHECK_CHARGING2;
			else
				m_FlashSpecialState = FMSS_CHECK_CHARGING;
		}
		break;
	case FMSS_CHECK_CHARGING:
		{
			//캐릭터 회전
			SetCharacterRot( pOwner );
			
			//타겟을 항상 바라보도록 설정
			CheckRotation( pOwner );
			
			//타겟이 없다면 재검색
			if ( (!m_bSetMoveAttack && m_AimedTarget.IsEmpty()) ||
				 IsTargetHasHideBuff( pOwner, m_AimedTarget ) ||
				 ( m_bSetTarget && D3DXVec3Length( &m_vTargetDir ) > GetChargeAimRange()) )
			{
				m_FlashSpecialState = FMSS_FIND_TARGET;
				m_bSetTarget = false;
			}

			CheckInputKey( pOwner );
		}
		break;
	case FMSS_CHECK_CHARGING2:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if ( m_AimedTarget.IsEmpty() )
				{
					m_AimedTarget.Clear();
					m_FlashSpecialState = FMSS_FIND_TARGET;
					m_bSetTarget = false;
				}
				else if ( !CheckTarget( pOwner ) )
				{
					m_AimedTarget.Clear();
					m_FlashSpecialState = FMSS_FIND_TARGET;
					m_bSetTarget = false;
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << WS_SPECIAL_FIND;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}

				CheckInputKey( pOwner );
			}
		}
		break;
	case FMSS_MOVE_DELAY:
		{			
			if ( m_dwRunDelayMotionEndTime < dwCurTime )
			{				
				//달리기 상태로 변경
				SetRunLoopState( pOwner );
			}
		}
		break;
	case FMSS_MOVE_RUN:
		{
			CheckMove( pOwner );

			if ( pOwner->IsNeedProcess() )
			{
				//거리가 되었거나, 시간이 지나면 달리기 종료
				if ( m_fCurMoveDist >= m_fCurMaxMoveDist || 
					( m_dwCheckRunTime + m_dwRunTime < dwCurTime ) )
				{				
					SetRunEndState( pOwner, true );			
				}
			}
		}
		break;
	case FMSS_MOVE_END:
		{
			CheckRunEndInputKey( pOwner );

			//애니메이션 끝나면 종료
			if ( m_dwRunEndMotionEndTime < dwCurTime )
			{
				CheckReserveInput( pOwner );				
			}
		}
		break;
	case FMSS_TELEPORT:
		{
			CheckTeleportInputKey( pOwner );
						
			//텔레포트 종료 조건 (키를 누른 후 애니메이션이 종료되면 종료)
			if ( m_dwTeleportAttackEndTime > 0 && m_dwTeleportAttackEndTime < dwCurTime )
			{
				CheckReserveInput( pOwner );	
			}
		}
		break;
	case FMSS_END:
		{			
			if ( IsFloatState( pOwner ) )
			{	
				if ( pOwner->IsNeedProcess() )
				{
					pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );

					if ( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << WS_SPECIAL_STATE_PROCESS;
						kPacket << FMSS_END_JUMP;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
			else
			{
				pOwner->SetState( CS_DELAY );
			}
		}
		break;
	}

	CheckFlashSpecialMoveState( pOwner );
	CheckJumpFlasSpecialCheckChargeState( pOwner );
}

//캐릭터 통과 상태
bool ioFlash_Item::IsFlashSpecialCharCollisionSkipState()
{
	switch( m_FlashSpecialState )
	{
	case FMSS_MOVE_RUN:
		return true;
	}

	return false;
}

//키입력 확인
void ioFlash_Item::CheckInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if ( !m_bSetMoveAttack && !pOwner->IsAttackKeyDown() )
	{
		DecreaseGauge( m_fNeedGauge );			
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		m_bSetMoveAttack = true;
		SetRunDelayState( pOwner );
		CreateAttachWeapon( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << FMSS_RUN_FIRE;
			kPacket << m_fCurChargeRate;
			kPacket << m_AimedTarget;
			kPacket << m_vTargetDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//이동 모션 변경
void ioFlash_Item::SetRunDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner)
		return;

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szRunDelayAni );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate  = m_fRunDelayAniRate;
	DWORD dwPreDelay = m_dwRunDelay_PreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );	

	m_dwRunDelayMotionEndTime = FRAMEGETTIME();
	m_dwRunDelayMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwRunDelayMotionEndTime += dwPreDelay;

	m_FlashSpecialState = FMSS_MOVE_DELAY;

	if( m_fCurChargeRate > 0.0f )
		m_fCurMaxMoveDist = m_fMaxMoveDist * ( FLOAT1 + m_fCurChargeRate );
	else
		m_fCurMaxMoveDist = m_fMaxMoveDist;
}

void ioFlash_Item::SetRunLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner)
		return;

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szRunLoopAniName );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );	

	m_dwCheckRunTime = FRAMEGETTIME();
	m_FlashSpecialState = FMSS_MOVE_RUN;
}

void ioFlash_Item::SetRunEndState( ioBaseChar *pOwner, bool bSend )
{
	if ( !pOwner)
		return;

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szRunEndAni );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate  = m_fRunEndAniRate;
	DWORD dwPreDelay = m_dwRunEnd_PreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );	

	m_dwRunEndMotionEndTime = FRAMEGETTIME();
	m_dwRunEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwRunEndMotionEndTime += dwPreDelay;

	m_dwTeleportReserveTime = FRAMEGETTIME();
	m_dwTeleportReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	//RunEnd 후 무기
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nRunEndAttackWeaponNum;
	kFireTime.iResistanceIdx = m_nRunEndAttackResistNum;
	kFireTime.szWoundedAni  = m_szRunEndAttackWoundedAni;
	kFireTime.dwWoundedAniDuration = m_dwRunEndAttackWoundedDuration;
	kFireTime.bLoopWoundedAni = m_bRunEndAttackWoundedLoop;

	kFireTime.dwWeaponIdx = m_dwRunWeaponBaseIndex + 1;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vRunEndAttackOffset );
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	m_FlashSpecialState = FMSS_MOVE_END;

	if ( bSend && pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << FMSS_MOVE_END;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//회전
void ioFlash_Item::CheckRotation( ioBaseChar *pOwner )
{
	if ( m_AimedTarget.IsEmpty() )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if ( pTarget )
	{
		m_vTargetDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		m_bSetTarget = true;

		D3DXVECTOR3 vDir = m_vTargetDir;
		D3DXVec3Normalize( &vDir, &vDir );
		pOwner->SetTargetRotToDir( vDir, true );
	}
}

bool ioFlash_Item::CheckTarget( ioBaseChar *pOwner )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;
	
	m_vTargetDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
	D3DXVec3Normalize( &m_vTargetDir, &m_vTargetDir );
	m_bSetTarget = true;

	D3DXVECTOR3 vDir = m_vTargetDir;
	D3DXVec3Normalize( &vDir, &vDir );

	float fChargeAng = GetChargeAimAngle();
	float fCurAngle = ioMath::ArrangeHead( fChargeAng );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fChargeRange = GetChargeAimRange();
	float fTargetRangeSq = fChargeRange * fChargeRange;

	if( pTarget->GetExperienceMode() != EMS_NONE )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
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

//캐릭터 회전
void ioFlash_Item::SetCharacterRot( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	//방향키 입력이 있다면
	if( pOwner->IsSettedDirection() )
	{
		//현재 입력된 방향키 확인하고 변경
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		m_InputDirKey = eNewDirKey;
	}

	//입력된 방향키와 현재 캐릭터의 방향이 다르다면
	if ( m_InputDirKey != pOwner->GetCurDirKey() )
	{
		//입력된 방향으로 회전
		pOwner->SetTargetRotToDirKey( m_InputDirKey );
		//타겟이름 지움
		m_AimedTarget.Clear();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << FMSS_CHAR_ROT;
			kPacket << (int)m_InputDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		m_InputDirKey = pOwner->GetCurDirKey();
	}
}

//이동
void ioFlash_Item::CheckMove( ioBaseChar *pOwner )
{	
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	
	D3DXVECTOR3 vDir;
	if ( m_AimedTarget.IsEmpty() )
	{
		vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}
	else 
	{
		vDir = m_vTargetDir;
		pOwner->SetTargetRotToDir( vDir, true );
	}
	D3DXVec3Normalize( &vDir, &vDir );

	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		
		if( m_fRunSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fRunSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
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
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
			vDiff = vTotalMove - vDiff;
			float fMoveDist = D3DXVec3Length( &vDiff );
			m_fCurMoveDist += fMoveDist;

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
		if ( bCol )
		{			
			m_FlashSpecialState = FMSS_END;
			return;
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();

	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}	
}

//타겟 찾기
void ioFlash_Item::CheckFindTarget( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioHashString szTarget;
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	float fAimAngle = GetChargeAimAngle();
	float fAimRange = GetChargeAimRange();

	pOwner->SetTargetMarkerRange( fAimRange );

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, m_bEnableDownAim ) )
	{
		m_AimedTarget = szTarget;
		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );		
	}
	else
	{
		m_AimedTarget.Clear();
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		return;
	}

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << FMSS_FIND_TARGET;
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

float ioFlash_Item::GetChargeAimRange()
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurAimRangeRate = FLOAT1;
		float fCurMaxAimRange = max( 0.0f, (m_fMaxAimRangeRate - FLOAT1) );

		fCurAimRangeRate += fCurMaxAimRange * m_fCurChargeRate;

		float fCurRange = m_fAimRange * fCurAimRangeRate;

		return fCurRange;
	}

	return m_fAimRange;
}

float ioFlash_Item::GetChargeAimAngle()
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurAimAngleRate = FLOAT1;
		float fCurMaxAimAngle = max( 0.0f, (m_fMaxAimAngleRate - FLOAT1) );

		fCurAimAngleRate += fCurMaxAimAngle * m_fCurChargeRate;

		float fCurAngle = m_fAimAngle * fCurAimAngleRate;
		return fCurAngle;
	}

	return m_fAimAngle;
}

bool ioFlash_Item::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) 
		return false;

	float fAimRange = GetChargeAimRange();
	float fAimAngle = GetChargeAimAngle();
	
	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = pOwner->GetMoveDir();

	float fHalfCosine = cosf( DEGtoRAD( fAimAngle ) * FLOAT05 );

	float fLimitDistSq = fAimRange * fAimRange;
	float fMinDistSq = m_fMinRange * m_fMinRange;
	
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

bool ioFlash_Item::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioFlash_Item::GetAutoTargetValue( ioBaseChar *pOwner,
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

			if ( m_bUseExtendDashAutoTargetValue && m_pExtendDash )
			{
				float fDashRange = 0.0f;
				float fDashMinAngle = 0.0f;
				float fDashMaxAngle = 0.0f;

				if( m_pExtendDash->GetAutoTargetValue( m_fDashAutoTargetRange, m_fDashAutoTargetMinAngle, m_fDashAutoTargetMaxAngle,
					fDashRange, fDashMinAngle, fDashMaxAngle ) )
				{
					fRange = fDashRange;
					fMinAngle = fDashMinAngle;
					fMaxAngle = fDashMaxAngle;
					return;
				}
			}
			

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

//달리기 종료 후 키입력 확인
void ioFlash_Item::CheckRunEndInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwTeleportReserveTime < FRAMEGETTIME() && !m_szTeleportTargetName.IsEmpty() && 
		 m_bEnableTeleport && m_nCurTeleportCnt < m_nMaxTeleportCnt && 
		 m_fCurGauge >= m_fTeleportUseGauge && pOwner->IsAttackKey() )
	{
		m_dwTeleportWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
		m_bEnableTeleport = false;
		DecreaseGauge( m_fTeleportUseGauge );
		if ( m_bCheckMove )
		{
			m_bCheckMove = SetTeleportMove( pOwner );
		}
		SetTeleportState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << FMSS_RUN_END_INPUT;
			kPacket << m_dwTeleportWeaponBaseIndex;
			kPacket << m_bCheckMove;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if ( m_dwTeleportReserveTime < FRAMEGETTIME() )
	{
		if( pOwner->IsAttackKey() )
			m_bAttackKeyReserved = true;

		if( pOwner->IsDefenseKey() )
			m_bDefenseKeyReserved = true;

		if( pOwner->IsJumpKey() )
			m_bJumpKeyReserved = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
////텔레포트///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//데이터 읽기
void ioFlash_Item::LoadTeleportAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_fTeleportUseGauge = rkLoader.LoadFloat_e( "teleport_need_gauge", 0.0f );
	m_fTeleportRange = rkLoader.LoadFloat_e( "teleport_range", 0.0f);
	m_fTeleportHeightGap = rkLoader.LoadFloat_e( "teleport_height_gap", 0.0f );
	m_TeleportList.clear();
	if( m_nMaxTeleportCnt > 0 )
	{
		m_TeleportList.reserve( m_nMaxTeleportCnt );

		for( int i=0; i < m_nMaxTeleportCnt; ++i )
		{
			AttackAttribute kAttack;

			wsprintf_e( szBuf, "teleport_attack%d", i+1 );
			LoadAttackAttribute( szBuf, kAttack, rkLoader );

			m_TeleportList.push_back( kAttack );
		}
	}
}

//텔레포트 상태 변경
void ioFlash_Item::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	pAttacker->ClearAttackFireTimeAndSkill();

	if( pWoundChar->IsNeedProcess() )
	{
		if ( pWoundChar && pAttacker )
		{
			m_szTeleportTargetName = pWoundChar->GetCharName();

			if( pWoundChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pAttacker, kPacket );
				kPacket << WS_SPECIAL_STATE_PROCESS;
				kPacket << FMSS_SET_TARGET;
				kPacket << m_szTeleportTargetName;
				kPacket << pAttacker->GetWorldPosition();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	if( !P2PNetwork::IsNetworkPlaying() || !pAttacker->GetCreator()->IsNetworkMode() || ToNpcChar( pWoundChar ) )
	{
		SetRunEndState( pAttacker, false );
		return;
	}
}

/*
//텔레포트 타겟 이름 설정(맞은 쪽에서 공격자에 대해 패킷 전송)
void ioFlash_Item::SetTeleportTarget( ioBaseChar *pTarget, ioBaseChar *pAttacker )
{	
	if ( pTarget && pAttacker )
	{
		m_szTeleportTargetName = pTarget->GetCharName();

		if( pTarget->IsNeedProcess() && pTarget->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << FMSS_SET_TARGET;
			kPacket << m_szTeleportTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}
*/

//텔레포트 상태 변경
void ioFlash_Item::SetTeleportState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->ClearSkillReserveKey();

	const AttributeList &rkTeleportList = m_TeleportList;
	
	if( (int)rkTeleportList.size() <= m_nCurTeleportCnt )
	{
		m_FlashSpecialState = FMSS_END;
		return;
	}

	AttackAttribute kAttack = rkTeleportList[m_nCurTeleportCnt];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );	

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwTeleportAttackEndTime = dwCurTime;
	m_dwTeleportAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwTeleportAttackEndTime += dwPreDelay;

	m_dwTeleportReserveTime = dwCurTime;
	m_dwTeleportReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	
	pOwner->RefreshFireTimeListWithBase( m_dwTeleportWeaponBaseIndex + m_nCurTeleportCnt,
		iAniID,
		kAttack.m_vWeaponInfoList,
		FTT_EXTEND_ATTACK,
		fTimeRate,
		dwPreDelay );	

	m_nCurTeleportCnt++;
	m_FlashSpecialState = FMSS_TELEPORT;
}

//텔레포트 키 입력 확인
void ioFlash_Item::CheckTeleportInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_bEnableTeleport && m_nCurTeleportCnt < m_nMaxTeleportCnt && m_fCurGauge >= m_fTeleportUseGauge && 
		 m_dwTeleportReserveTime < FRAMEGETTIME() && pOwner->IsAttackKey() )
	{
		m_bEnableTeleport = false;
		DecreaseGauge( m_fTeleportUseGauge );
		if ( m_bCheckMove )
		{
			m_bCheckMove = SetTeleportMove( pOwner );
		}
		SetTeleportState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << FMSS_TELEPORT;
			kPacket << m_bCheckMove;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if ( m_dwTeleportReserveTime < FRAMEGETTIME() )
	{
		if( pOwner->IsAttackKey() )
			m_bAttackKeyReserved = true;

		if( pOwner->IsDefenseKey() )
			m_bDefenseKeyReserved = true;

		if( pOwner->IsJumpKey() )
			m_bJumpKeyReserved = true;
	}
}

//텔레포트 이동
bool ioFlash_Item::SetTeleportMove( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || m_szTeleportTargetName.IsEmpty() )
		return false;

	//타겟 얻기
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTeleportTargetName );
	if ( !pTarget )
		return false;

	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();

	bool bAvailable[TD_MAX];
	for (int nDir=0; nDir<TD_MAX; nDir++)
	{
		bAvailable[nDir] = true;

		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
		
		switch( (TeleportDir)nDir )
		{
		case TD_Forward:
			vDir = ioMath::UNIT_Z;
			break;
		case TD_Right:
			vDir = ioMath::UNIT_X;
			break;
		case TD_Backward:
			vDir = -ioMath::UNIT_Z;
			break;
		case TD_Left:	
			vDir = -ioMath::UNIT_X;
			break;
		}	

		D3DXVECTOR3 vTempPos = vTargetPos + ( vDir * m_fTeleportRange);
		float fMapHeight = pOwner->GetCreator()->GetMapHeightOnlyTerrain( vTempPos.x, vTempPos.z );
		if ( fMapHeight > vTempPos.y + m_fTeleportHeightGap || fMapHeight <= 0.0f )
		{
			bAvailable[nDir] = false;
		}
	}

	//모두 이동 불가면 종료
	if ( !bAvailable[TD_Forward] && !bAvailable[TD_Right] && !bAvailable[TD_Backward] && !bAvailable[TD_Left] )
		return false;

	IORandom random;
	DWORD dwSeed = timeGetTime();
	random.SetRandomSeed( dwSeed );

	bool bTeleport = false;
	D3DXVECTOR3 vTeleportPos;

	while( !bTeleport )
	{		
		int nTeleportDir = random.Random( (unsigned int)TD_Forward, (unsigned int)TD_MAX );
		if ( bAvailable[nTeleportDir] )
		{
			switch( (TeleportDir)nTeleportDir )
			{
			case TD_Forward:
				vTeleportPos = vTargetPos + (ioMath::UNIT_Z * m_fTeleportRange);
				break;
			case TD_Right:
				vTeleportPos = vTargetPos + (ioMath::UNIT_X * m_fTeleportRange);
				break;
			case TD_Backward:
				vTeleportPos = vTargetPos - (ioMath::UNIT_Z * m_fTeleportRange);
				break;
			case TD_Left:
				vTeleportPos = vTargetPos - (ioMath::UNIT_X * m_fTeleportRange);	
				break;
			}	
			bTeleport = true;
		}
	}	
	
	if ( bTeleport )
	{
		//적용
		pOwner->SetWorldPosition( vTeleportPos );
	
		//회전
		D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - vTeleportPos;
		D3DXVec3Normalize( &vDir, &vDir );
		pOwner->SetTargetRotToDir( vDir, true );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << FMSS_TELEPORT_MOVE;
			kPacket << vTeleportPos;
			kPacket << vDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return true;
}

//텔레포트 가능(맞은 쪽에서 공격자에 대해 패킷 전송)
void ioFlash_Item::SetEnableTeleport( ioBaseChar *pTarget, ioBaseChar *pAttacker )
{
	if ( pTarget && pAttacker )
	{
		ioHashString szTarget = pTarget->GetCharName();
		if ( szTarget.IsEmpty() || m_szTeleportTargetName.IsEmpty() || szTarget != m_szTeleportTargetName )
			return;

		m_bEnableTeleport = true;

		if( pTarget->IsNeedProcess() && pTarget->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << FMSS_ENABLE_TELEPORT;
			kPacket << m_bEnableTeleport;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//예약상태
void ioFlash_Item::CheckReserveInput( ioBaseChar *pOwner )
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
		
	m_FlashSpecialState = FMSS_END;
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << FMSS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioFlash_Item::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if ( !IsFloatState(pOwner) )
	{
		pOwner->SetNormalAttack(0);
	}
	m_bAttackKeyReserved = false;
}

void ioFlash_Item::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioFlash_Item::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

//타겟이 하이드 버프를 갖고 있는가?
bool ioFlash_Item::IsTargetHasHideBuff( ioBaseChar *pOwner, ioHashString szTarget )
{
	if ( !pOwner || szTarget.IsEmpty() )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( szTarget );
	if ( pTarget && pTarget->HasHideBuff() )
	{
		return true;
	}

	return false;
}

void ioFlash_Item::CreateAttachWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess())
		return;

	//무기생성 (몸에 달려있는 무기)
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nRunAttachWeaponNum;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	m_dwRunWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	kFireTime.dwWeaponIdx = m_dwRunWeaponBaseIndex;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << FMSS_CREATE_ATTACH_WEAPON;
		kPacket << m_dwRunWeaponBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFlash_Item::CheckChargeRate()
{
	if ( m_FlashSpecialState == FMSS_FIND_TARGET || m_FlashSpecialState == FMSS_CHECK_CHARGING || m_FlashSpecialState == FMSS_CHECK_CHARGING2 )
	{
		if ( m_dwMaxChargeTime > 0 )
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_dwSpecialStateStartTime;
			if( m_dwMaxChargeTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				m_fCurChargeRate = FLOAT1;
		}
		else
			m_fCurChargeRate = 0.0f;
	}
}

void ioFlash_Item::CheckFlashSpecialMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_FlashSpecialState != FMSS_FIND_TARGET && m_FlashSpecialState != FMSS_CHECK_CHARGING && m_FlashSpecialState != FMSS_CHECK_CHARGING2 )
		return;

	if ( m_bJumpFlashSpecialState )
		return;

	if ( m_szFlashSpecialMoveAni.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;
	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection()  )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bSetFlashSpecialMoveAni = true;

			float fCurRate = FLOAT1;
			if( m_fFlashSpecialMoveAniRate > 0.0f )
				fCurRate = m_fFlashSpecialMoveAniRate;

			pGrp->SetLoopAni( m_szFlashSpecialMoveAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
	else
	{
		if( m_bSetFlashSpecialMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList.front() );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fFlashSpecialMoveAniRate > 0.0f )
				fCurRate = m_fFlashSpecialMoveAniRate;

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetFlashSpecialMoveAni = false;
		}
	}
}

void ioFlash_Item::CheckJumpFlasSpecialCheckChargeState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_FlashSpecialState != FMSS_FIND_TARGET && m_FlashSpecialState != FMSS_CHECK_CHARGING && m_FlashSpecialState != FMSS_CHECK_CHARGING2 )
		return;

	if ( !m_bJumpFlashSpecialState )
		return;

	if ( m_dwJumpFlashSpecialCheckChargeEndTime && m_dwJumpFlashSpecialCheckChargeEndTime < FRAMEGETTIME() )
	{
		m_dwJumpFlashSpecialCheckChargeEndTime = 0;
		pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );
		DecreaseGauge( m_fNeedGauge );
		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << FMSS_END_JUMP;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}
}

bool ioFlash_Item::FlashProcessMove( ioBaseChar *pOwner )
{
	if ( m_FlashSpecialState != FMSS_FIND_TARGET && m_FlashSpecialState != FMSS_CHECK_CHARGING && m_FlashSpecialState != FMSS_CHECK_CHARGING2 )
		return true;
	
	if ( m_bJumpFlashSpecialState )
		return true;

	if ( !m_bUseKeyMoveFlashSpecialState )
		return true;

	return false;
}

bool ioFlash_Item::IsCanKeyMove()
{
	if ( m_FlashSpecialState != FMSS_FIND_TARGET && m_FlashSpecialState != FMSS_CHECK_CHARGING && m_FlashSpecialState != FMSS_CHECK_CHARGING2 )
		return false;

	if ( !m_bUseKeyMoveFlashSpecialState )
		return false;

	return true;
}

bool ioFlash_Item::CheckSkipFallState()
{
	if ( m_bJumpFlashSpecialState )
		return true;
	return false;
}

void ioFlash_Item::ProcessKeyInput( ioBaseChar* pOwner )
{
	if ( !m_bUseCheckTarget2State )
		return;

	if ( m_FlashSpecialState == FMSS_FIND_TARGET || m_FlashSpecialState == FMSS_CHECK_CHARGING || m_FlashSpecialState == FMSS_CHECK_CHARGING2 )
		pOwner->ChangeDirectionByInputDir( false );

}

bool ioFlash_Item::IsEnableBlowWounded( ioBaseChar *pOwner )
{
	if ( m_bJumpFlashSpecialState )
		return true;
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioFlashSpecialState::ioFlashSpecialState()
{
}

ioFlashSpecialState::~ioFlashSpecialState()
{
}

void ioFlashSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioFlashSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioFlash_Item *pFlashItem = ToFlashItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pFlashItem )
		pFlashItem->ClearState( pOwner );
}

bool ioFlashSpecialState::IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const 
{ 
	ioFlash_Item *pFlashItem = ToFlashItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pFlashItem && pFlashItem->IsFlashSpecialCharCollisionSkipState() )
		return true;

	return false; 
}

bool ioFlashSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioFlash_Item *pFlashItem = ToFlashItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pFlashItem && pFlashItem->IsFlashSpecialCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioFlashSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioFlash_Item *pFlash_Item = ToFlashItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pFlash_Item )
		pFlash_Item->ProcessFlashSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioFlashSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	ioFlash_Item *pFlashItem = ToFlashItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pFlashItem && pFlashItem->CheckSkipFallState() )
		return;

	bool FallCheck = false;
	if( GetFlashSpecialState( pOwner ) == ioFlash_Item::FMSS_CHECK_CHARGING || GetFlashSpecialState( pOwner ) == ioFlash_Item::FMSS_CHECK_CHARGING2 )
		FallCheck = true;

	if( FallCheck )
	{				
		float fMinFallinRate = ( pOwner->GetWorldCollisionBox().GetExtents( 1 ) * 2.0f ) * pOwner->GetMinFallHeightRate();
		float fCharPosGap = pOwner->GetWorldPosition().y - fBottomHeight;
		if( fCharPosGap > fMinFallinRate )
			pOwner->SetFallState( true );
	}
}

bool ioFlashSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioFlash_Item *pFlash_Item = ToFlashItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pFlash_Item )
		return pFlash_Item->FlashProcessMove( pOwner );
	return true;
}

void ioFlashSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioFlash_Item *pFlash_Item = ToFlashItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pFlash_Item )
		return pFlash_Item->ProcessKeyInput( pOwner );
	
}

bool ioFlashSpecialState::IsEnableMoveState( const ioBaseChar* pOwner ) const
{
	return true;
}

bool ioFlashSpecialState::IsCanKeyMove( ioBaseChar* pOwner )
{
	ioFlash_Item *pFlash_Item = ToFlashItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pFlash_Item )
		return pFlash_Item->IsCanKeyMove();
	return false;
}

int ioFlashSpecialState::GetFlashSpecialState( ioBaseChar* pOwner )
{
	ioFlash_Item *pFlash_Item = ToFlashItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pFlash_Item )
		return static_cast<int>( ioFlash_Item::FMSS_CHECK_CHARGING );

	return static_cast<int>( pFlash_Item->GetFlashSpecialState() );
}

bool ioFlashSpecialState::IsEnableBlowWounded( ioBaseChar *pOwner )
{
	ioFlash_Item *pFlash_Item = ToFlashItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pFlash_Item )
		return pFlash_Item->IsEnableBlowWounded( pOwner );

	return true;
}

