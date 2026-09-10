
#include "stdafx.h"

#include "ioObjectRunningComboAttackItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioObjectRunningComboAttackItem::ioObjectRunningComboAttackItem()
{
	Init();
}

ioObjectRunningComboAttackItem::ioObjectRunningComboAttackItem( const ioObjectRunningComboAttackItem &rhs )
	: ioObjectItem( rhs ),
	  m_dwProtectTime( rhs.m_dwProtectTime ),
      m_bEnableObjectDefense( rhs.m_bEnableObjectDefense ),
	  m_NormalRunInfo( rhs.m_NormalRunInfo),
	  m_JumpRunInfo( rhs.m_JumpRunInfo ),
	  m_ChargeRunInfo( rhs.m_ChargeRunInfo ),
	  m_nMaxChargeRunCnt( rhs.m_nMaxChargeRunCnt ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_fJumpHeightGap( rhs.m_fJumpHeightGap )
{
	Init();
}

ioObjectRunningComboAttackItem::~ioObjectRunningComboAttackItem()
{
	m_ChargeRunInfo.clear();
}

void ioObjectRunningComboAttackItem::LoadProperty( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH];
	ioObjectItem::LoadProperty( rkLoader );
	
	m_bEnableObjectDefense = rkLoader.LoadBool_e( "enable_object_defense", false );
	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );

	LoadRunInfo( rkLoader, "normal", m_NormalRunInfo );
	LoadRunInfo( rkLoader, "jump", m_JumpRunInfo );

	m_nMaxChargeRunCnt = rkLoader.LoadInt_e( "charge_cnt", 0 );
	m_ChargeRunInfo.clear();
	for ( int i=0; i<m_nMaxChargeRunCnt; i++ )
	{
		wsprintf( szKey, "charge%d", i+1 );

		RunInfo sRunInfo;
		LoadRunInfo( rkLoader, szKey, sRunInfo );
		m_ChargeRunInfo.push_back( sRunInfo );
	}
	
	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );
}

ioItem* ioObjectRunningComboAttackItem::Clone()
{
	return new ioObjectRunningComboAttackItem( *this );
}

ioObjectItem::ObjectSubType ioObjectRunningComboAttackItem::GetObjectSubType() const
{
	return OST_RUNNING_COMBO_ATTACK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//공격 설정(초기화)
void ioObjectRunningComboAttackItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	Init();

	//공격 시간 초기화
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackStartTime = dwCurTime;

	m_WeaponState = WS_CHECK_CHARGING;
	pOwner->SetState( CS_ATTACK );
	//LOG.PrintTimeAndLog( 0, "!!!!#OBJECT_RUNATTACK_SetNormalAttack");
}

//차징 공격 확인
void ioObjectRunningComboAttackItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponState )
	{
	case WS_CHECK_CHARGING:
		CheckCharging( pOwner );
		break;	
	}
}

//점프 공격 설정(초기화)
bool ioObjectRunningComboAttackItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	Init();
	
	SetCurRunInfo( false, true, false );
	SetRunningAttackState( pOwner );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//동기화
void ioObjectRunningComboAttackItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( (PacketState)iState )
	{
	case PS_RUN_READY:
		{
			rkPacket >> m_bSetNormalAttack;
			rkPacket >> m_bSetJumpAttack;
			rkPacket >> m_bSetChargeAttack;
			
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetState( CS_OBJECT_RUNNING_COMBO_ATTACK );

			SetCurRunInfo( m_bSetNormalAttack, m_bSetJumpAttack, m_bSetChargeAttack );
			m_RunningAttackState = RAS_READY;
		}
		break;
	case PS_CREATE_WEAPON:
		{
			DWORD dwWeaponBase;
			rkPacket >> dwWeaponBase;

			//무기 생성
			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = m_CurInfo.m_nRunWeaponNum;
			kFireTime.iResistanceIdx = 0;
			kFireTime.szWoundedAni  = "";
			kFireTime.dwWoundedAniDuration = 0;
			kFireTime.bLoopWoundedAni = false;

			kFireTime.dwWeaponIdx = dwWeaponBase;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		}
		break;
	case PS_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

			if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
			{
				m_bTargetRot = false;
			}
		}
		break;
	case PS_CHARGE_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			rkPacket >> m_nCurChargeRun;
			
			pOwner->SetWorldPosition( vPos );

			SetCurRunInfo( false, false, true );
			SetRunningReadyState( pOwner );
		}
		break;
	case PS_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			bool bJump;
			rkPacket >> bJump;

			pOwner->SetWorldPosition( vPos );

			if ( bJump )
				pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, true );
			else
				pOwner->SetState( CS_DELAY, false );
		}
		break;
	}
}

//아이템 장착 시(초기화)
void ioObjectRunningComboAttackItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );

	Init();

	//무적 시간 초기화
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckProtectTime = dwCurTime;
}

//추가 무기 설정
void ioObjectRunningComboAttackItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

////////////////////////////////////////////////////////////////////////////////////////////////////////

//무적 상태인가
bool ioObjectRunningComboAttackItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwCheckProtectTime + m_dwProtectTime > FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioObjectRunningComboAttackItem::IsAutoTarget() const
{
	return false;
}

bool ioObjectRunningComboAttackItem::IsOneTimeWeaponItem() const
{
	return false;
}

//대쉬 가능한가
bool ioObjectRunningComboAttackItem::IsEnableDash() const
{
	return false;
}

//방어 가능한가
bool ioObjectRunningComboAttackItem::IsEnableDefense() const
{
	return m_bEnableObjectDefense;
}

//캐릭터 통과 상태
bool ioObjectRunningComboAttackItem::IsCharCollisionSkipState()
{
	switch( m_RunningAttackState )
	{
	case RAS_LOOP:
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//특별 프로세스
void ioObjectRunningComboAttackItem::ProcessRunningAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_RunningAttackState )
	{
	case RAS_READY:
		SetRunningReadyState( pOwner );
		break;
	case RAS_READY_END:
		SetRunningLoopState( pOwner );
		break;
	case RAS_LOOP:
		ProcessLoopState( pOwner );
		break;
	case RAS_LOOP_END:
		SetRunningEndState( pOwner );
		break;
	case RAS_END:
		SetRunningAttackEndState( pOwner );
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioObjectRunningComboAttackItem::LoadRunInfo( ioINILoader &rkLoader, char* szType, RunInfo &sRunInfo )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_ready", szType );
	LoadAttackAttribute( szKey, sRunInfo.m_RunReady, rkLoader );

	wsprintf_e( szKey, "%s_loop_ani", szType );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	sRunInfo.m_szRunLoopAni = szBuf;

	wsprintf_e( szKey, "%s_loop_ani_rate", szType );
	sRunInfo.m_fRunLoopAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

	wsprintf_e( szKey, "%s_end", szType );
	LoadAttackAttribute( szKey, sRunInfo.m_RunEnd, rkLoader );

	wsprintf_e( szKey, "%s_ready_run_speed", szType );
	sRunInfo.m_fReadyRunSpeed = rkLoader.LoadFloat( szKey, 0.0f );
	
	wsprintf_e( szKey, "%s_run_speed", szType );
	sRunInfo.m_fRunSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_run_duration", szType );
	sRunInfo.m_dwRunDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_run_rotate_speed", szType );
	sRunInfo.m_dwRunRotateSpeed = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_jump_power", szType );
	sRunInfo.m_fJumpPower = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_run_attack_type", szType );
	sRunInfo.m_nRunWeaponNum = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
}

void ioObjectRunningComboAttackItem::SetCurRunInfo( bool bNormal, bool bJump, bool bCombo )
{
	m_bSetNormalAttack = bNormal;
	m_bSetJumpAttack = bJump;
	m_bSetChargeAttack = bCombo;

	if ( bNormal )
		m_CurInfo = m_NormalRunInfo;
	else if ( bJump )
		m_CurInfo = m_JumpRunInfo;
	else if ( bCombo && COMPARE( m_nCurChargeRun, 0, m_nMaxChargeRunCnt ) )
		m_CurInfo = m_ChargeRunInfo[m_nCurChargeRun];
}

//초기화
void ioObjectRunningComboAttackItem::Init()
{
	m_WeaponState = WS_NONE;
	m_RunningAttackState = RAS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	
	m_bSetNormalAttack = false;
	m_bSetJumpAttack = false;	
	m_bSetChargeAttack = false;	
	m_nCurChargeRun = 0;

	m_dwCheckProtectTime = 0;
	m_dwAttackStartTime = 0;

	m_dwMotionEndTime = 0;
	m_dwCheckLoopTime = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_CurInfo.Init();
}

//차징 확인
void ioObjectRunningComboAttackItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetChargeAttackState( pOwner );
	}
	else
		SetNormalAttackState( pOwner );
}

//특별 설정
void ioObjectRunningComboAttackItem::SetRunningAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	pOwner->SetState( CS_OBJECT_RUNNING_COMBO_ATTACK );

	m_RunningAttackState = RAS_READY;

	if( pOwner->IsNeedSendNetwork() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << PS_RUN_READY;
		kPacket << m_bSetNormalAttack;
		kPacket << m_bSetJumpAttack;
		kPacket << m_bSetChargeAttack;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//일반 공격
void ioObjectRunningComboAttackItem::SetNormalAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();
	
	SetCurRunInfo( true, false, false );
	SetRunningAttackState( pOwner );
}

void ioObjectRunningComboAttackItem::SetChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();

	SetCurRunInfo( false, false, true );
	SetRunningAttackState( pOwner );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//달리기 준비 상태 설정
void ioObjectRunningComboAttackItem::SetRunningReadyState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_CurInfo.m_RunReady.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_CurInfo.m_RunReady.m_fAttackAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );	
	pOwner->SetJumpPower( m_CurInfo.m_fJumpPower );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_RunningAttackState = RAS_READY_END;
}

//달리기 루프 상태 설정
void ioObjectRunningComboAttackItem::SetRunningLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime > dwCurTime )
	{
		pOwner->SetCurMoveSpeed( m_CurInfo.m_fReadyRunSpeed );
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_CurInfo.m_szRunLoopAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_CurInfo.m_fRunLoopAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	CrateWeapon( pOwner );	

	m_dwCheckLoopTime = dwCurTime;	
	m_RunningAttackState = RAS_LOOP;	
}

//달리기 종료 상태 설정
void ioObjectRunningComboAttackItem::SetRunningEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	pOwner->SetCurMoveSpeed( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_CurInfo.m_RunEnd.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_CurInfo.m_RunEnd.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurInfo.m_RunEnd.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );	
	pOwner->SetReservedSliding( m_CurInfo.m_RunEnd.m_vForceInfoList, iAniID, fTimeRate, 0 );

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwWeaponBaseIndex, iAniID, m_CurInfo.m_RunEnd.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_RunningAttackState = RAS_END;
}

//특별 종료
void ioObjectRunningComboAttackItem::SetRunningAttackEndState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwMotionEndTime > dwCurTime )
		return;

	if ( m_bSetChargeAttack )
	{
		m_nCurChargeRun++;

		if ( COMPARE( m_nCurChargeRun, 0, m_nMaxChargeRunCnt ) )
		{
			SetCurRunInfo( false, false, true );
			SetRunningReadyState( pOwner );

			if ( pOwner->IsNeedSendNetwork() )
			{
				D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << PS_CHARGE_END;
				kPacket << vPos;
				kPacket << m_nCurChargeRun;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}

	bool bEndJump = false;
	if ( IsFloatState( pOwner ) )
	{
		pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );
		bEndJump = true;
	}
	else
		pOwner->SetState( CS_DELAY, false );

	if ( pOwner->IsNeedSendNetwork() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << PS_END;
		kPacket << vPos;
		kPacket << bEndJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//무기 생성(캐릭터에 붙음)
void ioObjectRunningComboAttackItem::CrateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwWeaponBase = pOwner->IncreaseWeaponIndexBase();

	//무기 생성
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_CurInfo.m_nRunWeaponNum;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	kFireTime.dwWeaponIdx = dwWeaponBase;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );

	//패킷 전송
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << PS_CREATE_WEAPON;
		kPacket << dwWeaponBase;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//달리기 루프 프로세스
void ioObjectRunningComboAttackItem::ProcessLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	CheckKeyInput( pOwner );
	CheckRotate( pOwner );
	pOwner->SetCurMoveSpeed( m_CurInfo.m_fRunSpeed );
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckLoopTime + m_CurInfo.m_dwRunDuration < dwCurTime )
	{
		m_RunningAttackState = RAS_LOOP_END;
		g_WeaponMgr.DestroyReserveZoneWeapon( pOwner->GetCharName() );
	}
}

//키입력
void ioObjectRunningComboAttackItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

			m_bTargetRot = false;
			m_bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;

			//패킷 전송
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << PS_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			//패킷 전송
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << PS_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

//회전
void ioObjectRunningComboAttackItem::CheckRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_CurInfo.m_dwRunRotateSpeed <= 0 ) 
		return;

	if( m_bTargetRot ) 
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_CurInfo.m_dwRunRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

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

bool ioObjectRunningComboAttackItem::IsFloatState( ioBaseChar *pOwner )
{	
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );

	if ( fabs( fMapHeight - vPos.y ) >= m_fJumpHeightGap )
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioObjectRunningComboSpecialState::ioObjectRunningComboSpecialState()
{
}

ioObjectRunningComboSpecialState::~ioObjectRunningComboSpecialState()
{
}

void ioObjectRunningComboSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioObjectRunningComboSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

bool ioObjectRunningComboSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioObjectItem *pObjectItem = pOwner->GetObject();
	if ( pObjectItem )
	{
		ioObjectRunningComboAttackItem *pRunningComboAttackItem = ToObjectRunningComboAttackItem( pObjectItem );
		if( pRunningComboAttackItem && pRunningComboAttackItem->IsCharCollisionSkipState() )
			return true;
	}
	return false;
}

void ioObjectRunningComboSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioObjectRunningComboAttackItem *pRunningComboAttackItem = ToObjectRunningComboAttackItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pRunningComboAttackItem )
		pRunningComboAttackItem->ProcessRunningAttackState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioObjectRunningComboSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}