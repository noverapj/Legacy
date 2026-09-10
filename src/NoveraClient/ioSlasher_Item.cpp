
#include "stdafx.h"

#include "ioSlasher_Item.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioSlasher_Item::ioSlasher_Item()
{	
	m_fCurGauge = 0.0f;
	Init();
}

ioSlasher_Item::ioSlasher_Item( const ioSlasher_Item &rhs )
	: ioWeaponItem( rhs ),
	  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_fMaxGauge( rhs.m_fMaxGauge ),
	  m_fNeedGauge( rhs.m_fNeedGauge ),  
	  m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	  m_fColAngle( rhs. m_fColAngle ),
	  m_fColRange( rhs.m_fColRange ),
	  m_bSetDownAim( rhs.m_bSetDownAim ),
	  m_RunInfo( rhs.m_RunInfo ),
	  m_nMaxCombo( rhs.m_nMaxCombo ),
	  m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
	  m_dwSecondChargeTime( rhs.m_dwSecondChargeTime ),
	  m_bSetAutoAttack( rhs.m_bSetAutoAttack )
{
	m_fCurGauge = 0.0f;
	Init();
}

ioSlasher_Item::~ioSlasher_Item()
{
}

void ioSlasher_Item::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	LoadAttackProperty( rkLoader );
	
	char szBuf[MAX_PATH];

	//게이지 설정
	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );

	//타겟 설정
	m_fColAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fColRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	m_bSetDownAim = rkLoader.LoadBool_e( "set_down_target", false );
	
	//Run속도
	m_RunInfo.Init();
	m_RunInfo.m_fFirstMaxMoveDist = rkLoader.LoadFloat_e( "first_run_dist", 0.0f );
	m_RunInfo.m_fFirstRunSpeed = rkLoader.LoadFloat_e( "first_run_speed", 0.0f );
	m_RunInfo.m_dwFirstRunTime = rkLoader.LoadInt_e( "first_run_duration", 0 );
	
	m_RunInfo.m_sFirstInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "first_run_attack_type", NO_WEAPON_ATTRIBUTE );
	m_RunInfo.m_sFirstInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "first_run_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "first_run_attack_type_wounded", "", szBuf, MAX_PATH );
	m_RunInfo.m_sFirstInfo.m_WoundedAnimation = szBuf;
	m_RunInfo.m_sFirstInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "first_run_attack_type_wounded_time", 0 );
	m_RunInfo.m_sFirstInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "first_run_attack_type_wounded_loop_ani", false );

	m_RunInfo.m_fSecondMaxMoveDist = rkLoader.LoadFloat_e( "second_run_dist", 0.0f );
	m_RunInfo.m_fSecondRunSpeed = rkLoader.LoadFloat_e( "second_run_speed", 0.0f );
	m_RunInfo.m_dwSecondRunTime = rkLoader.LoadInt_e( "second_run_duration", 0 );

	m_RunInfo.m_sSecondInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "second_run_attack_type", NO_WEAPON_ATTRIBUTE );
	m_RunInfo.m_sSecondInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "second_run_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "second_run_attack_type_wounded", "", szBuf, MAX_PATH );
	m_RunInfo.m_sSecondInfo.m_WoundedAnimation = szBuf;
	m_RunInfo.m_sSecondInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "second_run_attack_type_wounded_time", 0 );
	m_RunInfo.m_sSecondInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "second_run_attack_type_wounded_loop_ani", false );

	//달리기 애니메이션 관련
	rkLoader.LoadString_e( "first_run_delay_ani", "", szBuf, MAX_PATH );
	m_RunInfo.m_szFirstRunReadyAni = szBuf;
	m_RunInfo.m_fFirstRunReadyAniRate = rkLoader.LoadFloat_e( "first_run_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "first_run_loop_ani", "", szBuf, MAX_PATH );
	m_RunInfo.m_szFirstRunLoopAni = szBuf;
	rkLoader.LoadString_e( "first_run_end_ani", "", szBuf, MAX_PATH );
	m_RunInfo.m_szFirstRunEndAni = szBuf;
	m_RunInfo.m_fFirstRunEndAniRate = rkLoader.LoadFloat_e( "first_run_end_ani_rate", FLOAT1 );

	//달리기 애니메이션 관련
	rkLoader.LoadString_e( "second_run_delay_ani", "", szBuf, MAX_PATH );
	m_RunInfo.m_szSecondRunReadyAni = szBuf;
	m_RunInfo.m_fSecondRunReadyAniRate = rkLoader.LoadFloat_e( "second_run_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "second_run_loop_ani", "", szBuf, MAX_PATH );
	m_RunInfo.m_szSecondRunLoopAni = szBuf;
	rkLoader.LoadString_e( "second_run_end_ani", "", szBuf, MAX_PATH );
	m_RunInfo.m_szSecondRunEndAni = szBuf;
	m_RunInfo.m_fSecondRunEndAniRate = rkLoader.LoadFloat_e( "second_run_end_ani_rate", FLOAT1 );

	m_dwSecondChargeTime = rkLoader.LoadInt_e( "second_charge_time", 0 );	

	m_bSetAutoAttack = rkLoader.LoadBool_e( "set_auto_combo", false );
}

ioItem* ioSlasher_Item::Clone()
{
	return new ioSlasher_Item( *this );
}

ioWeaponItem::WeaponSubType ioSlasher_Item::GetSubType() const
{
	return WST_SLASHER_ITEM;
}

/////////////////////기본 공격
//공격 설정
void ioSlasher_Item::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioSlasher_Item::CheckNormalAttack( ioBaseChar *pOwner )
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

//무기 적용
void ioSlasher_Item::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

//동기화
void ioSlasher_Item::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case WS_SPECIAL_STATE_INIT:
		{
			SetCurRunInfo( m_RunInfo, true );
			pOwner->SetState( CS_SLASHER_SPECIAL );
			m_SlasherSpecialState = SSS_Find_Target;
		}
		break;
	case WS_SPECIAL_STATE_PROCESS:
		{
			RecvSpecialState( pOwner, rkPacket );
		}
		break;
	}
}

bool ioSlasher_Item::IsChargeAttackState( ioBaseChar *pOwner )
{
	return false;
}

///////////////////////게이지 관련///////////////////////////////////////////////
//데이터 갱신 (게이지 회복)
void ioSlasher_Item::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if ( pOwner->GetState() == CS_JUMP && pOwner->GetJumpState() == JS_JUMP_ATTACK )
	{
		return;
	}

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
	case CS_JUMP:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		break;
	}
}

//게이지 설정 및 데이터 얻기
int ioSlasher_Item::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioSlasher_Item::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioSlasher_Item::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioSlasher_Item::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioSlasher_Item::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioSlasher_Item::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioSlasher_Item::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioSlasher_Item::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioSlasher_Item::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

//게이지 사용
void ioSlasher_Item::DecreaseGauge( float fNeedGauge )
{
	m_fCurGauge -= fNeedGauge;
	m_fCurGauge = (float)max( 0.f, min( m_fCurGauge, m_fMaxGauge ) );
}

//게이지 사용이 가능한가?
bool ioSlasher_Item::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////
//추가 읽기
void ioSlasher_Item::LoadAttackProperty( ioINILoader &rkLoader )
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

	m_nMaxCombo = rkLoader.LoadInt_e( "charge_combo_max_cnt", 0 );
	m_vChargeComboAttList.clear();
	for( int i=0; i < m_nMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szKey, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}
}

//동기화
void ioSlasher_Item::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	case SSS_Find_Target:
		{
			ioHashString szTarget;
			rkPacket >> szTarget;

			ioBaseChar *pTarget = pOwner->GetBaseChar( szTarget );
			if ( pTarget )
			{
				D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
				D3DXVec3Normalize( &vDir, &vDir );
				pOwner->SetTargetRotToDir( vDir, true );
			}

			DecreaseGauge( m_fNeedGauge );
			SetCurRunInfo( m_CurRunInfo, true );
			SetRunReadyState( pOwner );
		}
		break;
	case SSS_Char_Rot:
		{
			int nDirKey;
			rkPacket >> nDirKey;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nDirKey;
			if ( m_CurDirKey != ioUserKeyInput::DKI_NONE )
			{
				pOwner->SetTargetRotToDirKey( m_CurDirKey );
			}

			DecreaseGauge( m_fNeedGauge );
			SetCurRunInfo( m_CurRunInfo, true );
			SetRunReadyState( pOwner );
		}
		break;
	case SSS_Combo_Attack:
		{
			rkPacket >> m_nCurCombo;

			int nDirKey;
			rkPacket >> nDirKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nDirKey;
			pOwner->SetTargetRotToDirKey( m_CurDirKey );

			//연타시 4타 이상일때 타격 가능하게 만듬
			pOwner->InitActionStopDelay( true );

			const AttackAttribute &pAttribute = m_vChargeComboAttList[m_nCurCombo];

			//공격 준비 애니메이션 설정
			ioEntityGroup *pGrp = pOwner->GetGroup();

			int iAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackAnimation );
			if ( iAniID == -1)
				return;

			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			float fTimeRate  = pAttribute.m_fAttackAniRate;

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

			pOwner->SetReservedSliding( pAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

			pOwner->RefreshFireTimeList( iAniID, pAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );

			DWORD dwCurTime = FRAMEGETTIME();
			m_dwComboEndMotionEndTime = dwCurTime;
			m_dwComboEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
		}
		break;
	case SSS_Create_Attach_Weapon:
		{
			rkPacket >> m_dwWeaponBaseIndex;

			//무기생성 (몸에 달려있는 무기)
			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();
			kFireTime.iAttributeIdx = m_sCurWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = m_sCurWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni  = m_sCurWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = m_sCurWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = m_sCurWeaponInfo.m_bWoundedLoopAni;
			kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
		}
		break;
	case SSS_Second_Run:
		{
			int nDirKey;
			rkPacket >> nDirKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nDirKey;
			pOwner->SetTargetRotToDirKey( m_CurDirKey );
			SetSecondRunState( pOwner );
		}
		break;
	case SSS_End:
		{
			m_SlasherSpecialState = SSS_End;			
		}
		break;
	}
}

//초기화
void ioSlasher_Item::Init()
{
	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.clear();
	
	m_WeaponItemState = WS_NONE;
	m_SlasherSpecialState = SSS_None;

	m_nNormalCombo = 0;

	m_fCurRunSpeed = 0.0f;
	m_dwCurRunTime = 0;
	m_fCurMaxMoveDist =0.0f;

	m_CurRunInfo.Init();
	m_sCurWeaponInfo.Init();

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	
	m_dwRunReadyMotionEndTime = 0;
	m_dwRunEndMotionEndTime = 0;
	m_dwComboEndMotionEndTime = 0;
	m_dwComboReserveTime = 0;

	m_dwCheckRunTime = 0;
	m_fCurMoveDist = 0.0f;
	
	m_nCurCombo = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bIsSecondCharge = false;

	ClearCancelInfo();

	m_fCurRunReadyAniRate = FLOAT1;
	m_fCurRunEndAniRate = FLOAT1;

	m_szCurRunReadyAni.Clear();
	m_szCurRunLoopAni.Clear();
	m_szCurRunEndAni.Clear();
}

//차징 확인
void ioSlasher_Item::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	//게이지 확인
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && GetCurBullet() >= GetNeedBullet() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetCurRunInfo( m_RunInfo, true );
			//특별 상태로 변경
			SetSlasherSpecialState( pOwner );
		}
	}
	else
	{
		//일반 공격
		SetNormalAttackState( pOwner );
	}
}

//기본 공격 상태로 변경
void ioSlasher_Item::SetNormalAttackState( ioBaseChar *pOwner )
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
void ioSlasher_Item::ClearState( ioBaseChar *pOwner )
{
	Init();
}

//캐릭터를 특별 상태로 변경 (공격 상태에서 특별 상태로 변경)
void ioSlasher_Item::SetSlasherSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	pOwner->SetState( CS_SLASHER_SPECIAL );
	
	m_SlasherSpecialState = SSS_Find_Target;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//캐릭터 특별 상태 프로세스
void ioSlasher_Item::ProcessSlasherSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	switch( m_SlasherSpecialState )
	{
	case SSS_Find_Target:
		{			
			FindTarget( pOwner );
			SetRotateState( pOwner );
		}
		break;
	case SSS_Run_Ready:
		{
			if ( m_dwRunReadyMotionEndTime < dwCurTime )
			{				
				CreateAttachWeapon( pOwner );
				//달리기 상태로 변경
				SetRunLoopState( pOwner );
			}
		}
		break;
	case SSS_Run_Loop:
		{
			CheckMove( pOwner );

			if ( m_fCurMoveDist >= m_fCurMaxMoveDist || 
				 m_dwCheckRunTime + m_dwCurRunTime < dwCurTime )
			{
				SetRunEndState( pOwner );
			}
		}
		break;
	case SSS_Run_End:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				if ( !m_bIsSecondCharge && pOwner->IsAttackKey() )
				{
					m_SlasherSpecialState = SSS_Combo_Check;
					return;
				}

				if ( m_bIsSecondCharge )
				{
					ReserveInputKey( pOwner );
				}

				//애니메이션 끝나면 종료
				if ( m_dwRunEndMotionEndTime > 0 && m_dwRunEndMotionEndTime < dwCurTime )
				{
					CheckReserveState( pOwner );				
				}
			}
		}
		break;
	case SSS_Combo_Attack:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				if ( !CheckComboInput( pOwner ) )
				{					
					//애니메이션 끝나면 종료
					if ( m_dwComboEndMotionEndTime > 0 && m_dwComboEndMotionEndTime < dwCurTime )
					{
						CheckReserveState( pOwner );				
					}					
				}	
			}			
		}
		break;
	case SSS_Combo_Check:
		{
			if ( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
								
				if ( pOwner->IsAttackKeyDown() && m_nCurCombo < m_nMaxCombo && m_dwComboReserveTime < dwCurTime )
				{
					m_dwAttackStartTime = dwCurTime;
					m_SlasherSpecialState = SSS_Combo_Attack;
					return;
				}
				else if ( m_bSetAutoAttack && m_nCurCombo < m_nMaxCombo && m_dwComboReserveTime < dwCurTime )
				{
					SetComboAttackState( pOwner );
					return;
				}
				else if ( !m_bSetAutoAttack && !pOwner->IsAttackKeyDown() && m_dwComboEndMotionEndTime == 0 )
				{
					SetComboAttackState( pOwner );
					return;
				}
				//애니메이션 끝나면 종료
				else if ( m_dwComboEndMotionEndTime > 0 && m_dwComboEndMotionEndTime < dwCurTime )
				{
					CheckReserveState( pOwner );				
				}
			}			
		}
		break;
	case SSS_End:
		{
			pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

void ioSlasher_Item::SetSpecialState( SlasherSpecialState eType )
{
	m_SlasherSpecialState = eType;
}

//캐릭터 통과 상태
bool ioSlasher_Item::IsSlasherSpecialCharCollisionSkipState()
{
	switch( m_SlasherSpecialState )
	{
	case SSS_Run_Loop:
		return true;
	}

	return false;
}

//상태 변경
void ioSlasher_Item::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	pAttacker->ClearAttackFireTimeAndSkill();

	//텔레포트 상태로 변경
	SetRunEndState( pAttacker );
}

/////////////////////////////////////////////////////////////////////////////////
//가장 가까운 타겟 검사
bool ioSlasher_Item::FindTarget( ioBaseChar *pOwner )
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
	pOwner->GetTargetCharListBySphere( vPos, m_fColRange, m_fColAngle, vTargetList );

	//타겟 리스트가 비어있다면 특별 종료
	if ( vTargetList.empty() )
		return false;

	//타겟 리스트 소팅
	SortTargetList( pOwner, vTargetList );
	
	return true;
}

//타겟 리스트 가까운 순서로 정렬
void ioSlasher_Item::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList )
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
bool ioSlasher_Item::IsEnableTarget( ioBaseChar *pTarget )
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

void ioSlasher_Item::SetRotateState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	DecreaseGauge( m_fNeedGauge );
	SetCurRunInfo( m_CurRunInfo, true );
	SetRunReadyState( pOwner );
	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	//회전
	if ( !m_SortList.empty() )
	{
		ioBaseChar *pTarget = m_SortList[0].pChar;
		if ( pTarget )
		{
			D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vDir, &vDir );
			pOwner->SetTargetRotToDir( vDir, true );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << WS_SPECIAL_STATE_PROCESS;
				kPacket << SSS_Find_Target;
				kPacket << pTarget->GetCharName();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}	
		}
	}
	else 
	{
		if( pOwner->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();

			m_CurDirKey = eNewDirKey;
			pOwner->SetTargetRotToDirKey( m_CurDirKey );					
		}
		else 
		{			
			m_CurDirKey = ioUserKeyInput::DKI_NONE;			
		}		

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << SSS_Char_Rot;
			kPacket << m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}		
	}	
}

//이동 모션 변경
void ioSlasher_Item::SetRunReadyState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szCurRunReadyAni );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate  = m_fCurRunReadyAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

	m_dwRunReadyMotionEndTime = FRAMEGETTIME();
	m_dwRunReadyMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_SlasherSpecialState = SSS_Run_Ready;
}

void ioSlasher_Item::SetRunLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szCurRunLoopAni );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );	

	m_dwCheckRunTime = FRAMEGETTIME();
	m_SlasherSpecialState = SSS_Run_Loop;
}

void ioSlasher_Item::SetRunEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szCurRunEndAni );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate  = m_fCurRunEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunEndMotionEndTime = dwCurTime;
	m_dwRunEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwAttackStartTime = dwCurTime;
	SetCurAttackAniForInputJump( m_szCurRunEndAni, fTimeRate );

	m_SlasherSpecialState = SSS_Run_End;
}

void ioSlasher_Item::SetComboAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || m_nMaxCombo <= 0 )
		return;

	//연타시 4타 이상일때 타격 가능하게 만듬
	pOwner->InitActionStopDelay( true );
	
	const AttackAttribute &pAttribute = m_vChargeComboAttList[m_nCurCombo];

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate  = pAttribute.m_fAttackAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

	pOwner->SetReservedSliding( pAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pOwner->RefreshFireTimeList( iAniID, pAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwComboReserveTime = dwCurTime;
	m_dwComboReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwComboEndMotionEndTime = dwCurTime;
	m_dwComboEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_SlasherSpecialState = SSS_Combo_Check;
	
	SetCurAttackAniForInputJump( pAttribute.m_AttackAnimation, fTimeRate );

	SetCharRotate( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SSS_Combo_Attack;
		kPacket << m_nCurCombo;
		kPacket << m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	

	m_nCurCombo++;
}

void ioSlasher_Item::SetSecondRunState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	SetCurRunInfo( m_CurRunInfo, false );
	
	m_bIsSecondCharge = true;
		
	SetCharRotate( pOwner );

	m_dwWeaponBaseIndex++;

	SetRunReadyState( pOwner );

	if( pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SSS_Second_Run;
		kPacket << m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

/////////////////////////////////////////////////////////////////////////////////
//이동
void ioSlasher_Item::CheckMove( ioBaseChar *pOwner )
{	
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;

	D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fCurRunSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fCurRunSpeed * fDivideTimePerSec;
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
			SetRunEndState( pOwner );
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

void ioSlasher_Item::CreateAttachWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	//무기생성 (몸에 달려있는 무기)
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_sCurWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_sCurWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_sCurWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_sCurWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_sCurWeaponInfo.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SSS_Create_Attach_Weapon;
		kPacket << m_dwWeaponBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioSlasher_Item::CheckComboInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || m_bIsSecondCharge )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKeyDown() )
	{
		if( m_nCurCombo < m_nMaxCombo && 
			m_dwAttackStartTime + m_dwSecondChargeTime < dwCurTime )
		{
			SetSecondRunState( pOwner );
		}		
		return true;
	}
	else if ( m_nCurCombo < m_nMaxCombo && m_dwComboReserveTime < dwCurTime )
	{
		SetComboAttackState( pOwner );
		return true;
	}

	return false;
}

void ioSlasher_Item::SetCharRotate( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( m_CurDirKey == eNewDirKey )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;
			return;
		}

		m_CurDirKey = eNewDirKey;
		pOwner->SetTargetRotToDirKey( m_CurDirKey );
	}
}

/////////////////////////////////////////////////////////////////////////////////
//예약상태
void ioSlasher_Item::ReserveInputKey(ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;
}

void ioSlasher_Item::CheckReserveState( ioBaseChar *pOwner )
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

	m_SlasherSpecialState = SSS_End;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SSS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioSlasher_Item::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioSlasher_Item::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioSlasher_Item::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

/////////////////////////////////////////////////////////////////////////////////

void ioSlasher_Item::SetCurRunInfo( const SlasherRunInfo& RunInfo, bool bFirst )
{
	m_CurRunInfo = RunInfo;
	m_fCurMoveDist = 0.0f;

	if ( bFirst )
	{
		//달리기 옵션
		m_fCurMaxMoveDist = m_CurRunInfo.m_fFirstMaxMoveDist;
		m_dwCurRunTime = m_CurRunInfo.m_dwFirstRunTime;
		m_fCurRunSpeed = m_CurRunInfo.m_fFirstRunSpeed;
		m_sCurWeaponInfo = m_CurRunInfo.m_sFirstInfo;
		//애니 설정
		m_szCurRunReadyAni = m_CurRunInfo.m_szFirstRunReadyAni;
		m_fCurRunReadyAniRate = m_CurRunInfo.m_fFirstRunReadyAniRate;
		m_szCurRunLoopAni = m_CurRunInfo.m_szFirstRunLoopAni;
		m_szCurRunEndAni = m_CurRunInfo.m_szFirstRunEndAni;
		m_fCurRunEndAniRate = m_CurRunInfo.m_fFirstRunEndAniRate;
	}
	else
	{
		//달리기 옵션
		m_fCurMaxMoveDist = m_CurRunInfo.m_fSecondMaxMoveDist;
		m_dwCurRunTime = m_CurRunInfo.m_dwSecondRunTime;
		m_fCurRunSpeed = m_CurRunInfo.m_fSecondRunSpeed;
		m_sCurWeaponInfo = m_CurRunInfo.m_sSecondInfo;
		//애니 설정
		m_szCurRunReadyAni = m_CurRunInfo.m_szSecondRunReadyAni;
		m_fCurRunReadyAniRate = m_CurRunInfo.m_fSecondRunReadyAniRate;
		m_szCurRunLoopAni = m_CurRunInfo.m_szSecondRunLoopAni;
		m_szCurRunEndAni = m_CurRunInfo.m_szSecondRunEndAni;
		m_fCurRunEndAniRate = m_CurRunInfo.m_fSecondRunEndAniRate;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioSlasherSpecialState::ioSlasherSpecialState()
{
}

ioSlasherSpecialState::~ioSlasherSpecialState()
{
}

void ioSlasherSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSlasherSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioSlasher_Item *pSlasherItem = ToSlasherItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSlasherItem )
		pSlasherItem->ClearState( pOwner );
}

bool ioSlasherSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioSlasher_Item *pSlasherItem = ToSlasherItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pSlasherItem && pSlasherItem->IsSlasherSpecialCharCollisionSkipState() )
		return true;
	
	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioSlasherSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSlasher_Item *pSlasherItem = ToSlasherItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSlasherItem )
		pSlasherItem->ProcessSlasherSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSlasherSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}