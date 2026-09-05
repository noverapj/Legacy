
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioObjectRunningAttackItem.h"

#include "WeaponDefine.h"

ioObjectRunningAttackItem::ioObjectRunningAttackItem()
{
	Init();
}

ioObjectRunningAttackItem::ioObjectRunningAttackItem( const ioObjectRunningAttackItem &rhs )
	: ioObjectItem( rhs ),
	  m_dwProtectTime( rhs.m_dwProtectTime ),
      m_bEnableObjectDefense( rhs.m_bEnableObjectDefense ),
	  m_szNormalRunReadyAni( rhs.m_szNormalRunReadyAni ),
	  m_fNormalRunReadyAniRate( rhs.m_fNormalRunReadyAniRate ),
	  m_szNormalRunLoopAni( rhs.m_szNormalRunLoopAni ),
	  m_szNormalRunEndAni( rhs.m_szNormalRunEndAni ),
	  m_fNormalRunEndAniRate( rhs.m_fNormalRunEndAniRate ),
	  m_szChargeRunReadyAni( rhs.m_szChargeRunReadyAni ),
	  m_fChargeRunReadyAniRate( rhs.m_fChargeRunReadyAniRate ),
	  m_szChargeRunLoopAni( rhs.m_szChargeRunLoopAni ),
	  m_szChargeRunEndAni( rhs.m_szChargeRunEndAni ),
	  m_fChargeRunEndAniRate( rhs.m_fChargeRunEndAniRate ),
	  m_szJumpRunReadyAni( rhs.m_szJumpRunReadyAni ),
	  m_fJumpRunReadyAniRate( rhs.m_fJumpRunReadyAniRate ),
	  m_szJumpRunLoopAni( rhs.m_szJumpRunLoopAni ),
	  m_szJumpRunEndAni( rhs.m_szJumpRunEndAni ),
	  m_fJumpRunEndAniRate( rhs.m_fJumpRunEndAniRate ),
	  m_fNormalReadyRunSpeed( rhs.m_fNormalReadyRunSpeed ),
	  m_fNormalRunSpeed( rhs.m_fNormalRunSpeed ),
	  m_dwNormalRunDuration( rhs.m_dwNormalRunDuration ),
	  m_dwNormalRunRotateSpeed( rhs.m_dwNormalRunRotateSpeed ),
	  m_nNormalRunWeaponNum( rhs.m_nNormalRunWeaponNum ),
	  m_fChargeReadyRunSpeed( rhs.m_fChargeReadyRunSpeed ),
	  m_fChargeRunSpeed( rhs.m_fChargeRunSpeed ),
	  m_dwChargeRunDuration( rhs.m_dwChargeRunDuration ),
	  m_dwChargeRunRotateSpeed( rhs.m_dwChargeRunRotateSpeed ),
	  m_nChargeRunWeaponNum( rhs.m_nChargeRunWeaponNum ),
	  m_fJumpReadyRunSpeed( rhs.m_fJumpReadyRunSpeed ),
	  m_fJumpRunSpeed( rhs.m_fJumpRunSpeed ),
	  m_dwJumpRunDuration( rhs.m_dwJumpRunDuration ),
	  m_dwJumpRunRotateSpeed( rhs.m_dwJumpRunRotateSpeed ),
	  m_nJumpRunWeaponNum( rhs.m_nJumpRunWeaponNum ),
	  m_vEndForceInfoList( rhs.m_vEndForceInfoList )
{
	Init();
}

ioObjectRunningAttackItem::~ioObjectRunningAttackItem()
{
}

void ioObjectRunningAttackItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );
	
	m_bEnableObjectDefense = rkLoader.LoadBool_e( "enable_object_defense", false );
	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	//일반
	rkLoader.LoadString_e( "normal_ready_ani", "", szBuf, MAX_PATH );
	m_szNormalRunReadyAni = szBuf;
	m_fNormalRunReadyAniRate = rkLoader.LoadFloat_e( "normal_ready_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "normal_loop_ani", "", szBuf, MAX_PATH );
	m_szNormalRunLoopAni = szBuf;
	rkLoader.LoadString_e( "normal_end_ani", "", szBuf, MAX_PATH );
	m_szNormalRunEndAni = szBuf;
	m_fNormalRunEndAniRate = rkLoader.LoadFloat_e( "normal_end_ani_rate", FLOAT1 );

	//차지
	rkLoader.LoadString_e( "charge_ready_ani", "", szBuf, MAX_PATH );
	m_szChargeRunReadyAni = szBuf;
	m_fChargeRunReadyAniRate = rkLoader.LoadFloat_e( "charge_ready_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "charge_loop_ani", "", szBuf, MAX_PATH );
	m_szChargeRunLoopAni = szBuf;
	rkLoader.LoadString_e( "charge_end_ani", "", szBuf, MAX_PATH );
	m_szChargeRunEndAni = szBuf;
	m_fChargeRunEndAniRate = rkLoader.LoadFloat_e( "charge_end_ani_rate", FLOAT1 );

	//점프
	rkLoader.LoadString_e( "jump_ready_ani", "", szBuf, MAX_PATH );
	m_szJumpRunReadyAni = szBuf;
	m_fJumpRunReadyAniRate = rkLoader.LoadFloat_e( "jump_ready_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "jump_loop_ani", "", szBuf, MAX_PATH );
	m_szJumpRunLoopAni = szBuf;
	rkLoader.LoadString_e( "jump_end_ani", "", szBuf, MAX_PATH );
	m_szJumpRunEndAni = szBuf;
	m_fJumpRunEndAniRate = rkLoader.LoadFloat_e( "jump_end_ani_rate", FLOAT1 );

	//슬라이딩 옵션 설정
	m_fNormalReadyRunSpeed = rkLoader.LoadFloat_e( "normal_ready_run_speed", 0.0f );
	m_fNormalRunSpeed = rkLoader.LoadFloat_e( "normal_run_speed", 0.0f );
	m_dwNormalRunDuration = rkLoader.LoadInt_e( "normal_run_duration", 0 );
	m_dwNormalRunRotateSpeed = rkLoader.LoadInt_e( "normal_run_rotate_speed", 0);
	m_nNormalRunWeaponNum = rkLoader.LoadInt_e( "normal_run_attack_type", NO_WEAPON_ATTRIBUTE );

	m_fChargeReadyRunSpeed = rkLoader.LoadFloat_e( "charge_ready_run_speed", 0.0f );
	m_fChargeRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_dwChargeRunDuration = rkLoader.LoadInt_e( "charge_run_duration", 0 );
	m_dwChargeRunRotateSpeed = rkLoader.LoadInt_e( "charge_run_rotate_speed", 0);
	m_nChargeRunWeaponNum = rkLoader.LoadInt_e( "charge_run_attack_type", NO_WEAPON_ATTRIBUTE );

	m_fJumpReadyRunSpeed = rkLoader.LoadFloat_e( "jump_ready_run_speed", 0.0f );
	m_fJumpRunSpeed = rkLoader.LoadFloat_e( "jump_run_speed", 0.0f );
	m_dwJumpRunDuration = rkLoader.LoadInt_e( "jump_run_duration", 0 );
	m_dwJumpRunRotateSpeed = rkLoader.LoadInt_e( "jump_run_rotate_speed", 0);
	m_nJumpRunWeaponNum = rkLoader.LoadInt_e( "jump_run_attack_type", NO_WEAPON_ATTRIBUTE );

	m_vEndForceInfoList.clear();
	int nForceCnt = rkLoader.LoadInt_e( "end_force_cnt", 0 );
	if( nForceCnt > 0 )
		m_vEndForceInfoList.reserve( nForceCnt );

	for( int i=0; i < nForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "end_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "end_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "end_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_vEndForceInfoList.push_back( kInfo );
	}
}

ioItem* ioObjectRunningAttackItem::Clone()
{
	return new ioObjectRunningAttackItem( *this );
}

ioObjectItem::ObjectSubType ioObjectRunningAttackItem::GetObjectSubType() const
{
	return OST_RUNNING_ATTACK;
}

//공격 설정(초기화)
void ioObjectRunningAttackItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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
void ioObjectRunningAttackItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponState )
	{
	case WS_CHECK_CHARGING:
		CheckCharging( pOwner );
		break;	
	}
}

//점프 공격 설정(초기화)
bool ioObjectRunningAttackItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	Init();
	
	m_bSetJumpAttack = true;
	SetRunningAttackState( pOwner, m_fJumpReadyRunSpeed, m_fJumpRunSpeed, m_dwJumpRunDuration, m_dwJumpRunRotateSpeed, m_nJumpRunWeaponNum );

	return true;
}

//동기화
void ioObjectRunningAttackItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( (PacketState)iState )
	{
	case PS_RUN_READY:
		{
			rkPacket >> m_fCurReadyRunSpeed;
			rkPacket >> m_fCurRunSpeed;
			rkPacket >> m_dwCurRunDuration;
			rkPacket >> m_nCurRunRotateSpeed;
			rkPacket >> m_nCurRunWeaponNum;
			rkPacket >> m_bSetNormalAttack;
			rkPacket >> m_bSetChargeAttack;
			rkPacket >> m_bSetJumpAttack;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetState( CS_OBJECT_RUNNING_ATTACK );
			//LOG.PrintTimeAndLog( 0, "!!!!#OBJECT_RUNATTACK_SetRunningAttackState");
			m_RunningAttackState = RAS_READY;

			//LOG.PrintTimeAndLog( 0, "!!!!#OBJECT_RUNATTACK_PS_RUN_READY");
		}
		break;
	case PS_CREATE_WEAPON:
		{
			DWORD dwWeaponBase;
			rkPacket >> dwWeaponBase;

			//무기 생성
			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = m_nCurRunWeaponNum;
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
	case PS_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetState( CS_DELAY, false );
			pOwner->SetWorldPosition( vPos );

			//LOG.PrintTimeAndLog( 0, "!!!!#OBJECT_RUNATTACK_PS_END");
		}
	}
}

//아이템 장착 시(초기화)
void ioObjectRunningAttackItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );

	Init();

	//무적 시간 초기화
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckProtectTime = dwCurTime;
}

//추가 무기 설정
void ioObjectRunningAttackItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

//무적 상태인가
bool ioObjectRunningAttackItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwCheckProtectTime + m_dwProtectTime > FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioObjectRunningAttackItem::IsOneTimeWeaponItem() const
{
	return false;
}

//대쉬 가능한가
bool ioObjectRunningAttackItem::IsEnableDash() const
{
	return false;
}

//방어 가능한가
bool ioObjectRunningAttackItem::IsEnableDefense() const
{
	return m_bEnableObjectDefense;
}

//캐릭터 통과 상태
bool ioObjectRunningAttackItem::IsCharCollisionSkipState()
{
	switch( m_RunningAttackState )
	{
	case RAS_LOOP:
		return true;
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//초기화
void ioObjectRunningAttackItem::Init()
{
	m_WeaponState = WS_NONE;
	m_RunningAttackState = RAS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_fCurReadyRunSpeed = 0.0f;
	m_fCurRunSpeed = 0.0f;
	m_dwCurRunDuration = 0;
	m_nCurRunRotateSpeed = 0;

	m_bSetNormalAttack = false;
	m_bSetChargeAttack = false;
	m_bSetJumpAttack = false;	

	m_dwCheckProtectTime = 0;
	m_dwAttackStartTime = 0;

	m_dwRunReadyEndTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwRunLoopEndTime = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

//차징 확인
void ioObjectRunningAttackItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			//차지 공격 변경
			m_bSetChargeAttack = true;
			SetRunningAttackState( pOwner, m_fChargeReadyRunSpeed, m_fChargeRunSpeed, m_dwChargeRunDuration, m_dwChargeRunRotateSpeed, m_nChargeRunWeaponNum );
		}
	}
	else
	{
		//일반 공격 변경
		SetNormalAttackState( pOwner );
	}
}

//특별 설정
void ioObjectRunningAttackItem::SetRunningAttackState( ioBaseChar *pOwner, float fReadySpeed, float fLoopSpeed, DWORD dwDuration, DWORD dwRotateSpeed, int nAttackType )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	m_fCurReadyRunSpeed = fReadySpeed;
	m_fCurRunSpeed = fLoopSpeed;
	m_dwCurRunDuration = dwDuration;
	m_nCurRunRotateSpeed = dwRotateSpeed;
	m_nCurRunWeaponNum = nAttackType;

	pOwner->SetState( CS_OBJECT_RUNNING_ATTACK );
	//LOG.PrintTimeAndLog( 0, "!!!!#OBJECT_RUNATTACK_SetRunningAttackState");

	m_RunningAttackState = RAS_READY;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << PS_RUN_READY;
		kPacket << m_fCurReadyRunSpeed;
		kPacket << m_fCurRunSpeed;
		kPacket << m_dwCurRunDuration;
		kPacket << m_nCurRunRotateSpeed;
		kPacket << m_nCurRunWeaponNum;
		kPacket << m_bSetNormalAttack;
		kPacket << m_bSetChargeAttack;
		kPacket << m_bSetJumpAttack;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//특별 프로세스
void ioObjectRunningAttackItem::ProcessRunningAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

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

//일반 공격
void ioObjectRunningAttackItem::SetNormalAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();
	
	m_bSetNormalAttack = true;
	SetRunningAttackState( pOwner, m_fNormalReadyRunSpeed, m_fNormalRunSpeed, m_dwNormalRunDuration, m_dwNormalRunRotateSpeed, m_nNormalRunWeaponNum );
}

//달리기 준비 상태 설정
void ioObjectRunningAttackItem::SetRunningReadyState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = -1;
	float fTimeRate = FLOAT1;
	
	if ( m_bSetNormalAttack )
	{
		iAniID = pGrp->GetAnimationIdx( m_szNormalRunReadyAni );
		fTimeRate = m_fNormalRunReadyAniRate;
	}	
	else if ( m_bSetChargeAttack )
	{
		iAniID = pGrp->GetAnimationIdx( m_szChargeRunReadyAni );
		fTimeRate = m_fChargeRunReadyAniRate;
	}	
	else if ( m_bSetJumpAttack )
	{
		iAniID = pGrp->GetAnimationIdx( m_szJumpRunReadyAni );
		fTimeRate = m_fJumpRunReadyAniRate;
	} 

	if ( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunReadyEndTime = dwCurTime;
	m_dwRunReadyEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_RunningAttackState = RAS_READY_END;

	//LOG.PrintTimeAndLog( 0, "!!!!#OBJECT_RUNATTACK_SetRunningReadyState");
}

//달리기 루프 상태 설정
void ioObjectRunningAttackItem::SetRunningLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwRunReadyEndTime > dwCurTime )
	{
		pOwner->SetCurMoveSpeed( m_fCurReadyRunSpeed );
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = -1;

	if ( m_bSetNormalAttack )
	{
		iAniID = pGrp->GetAnimationIdx( m_szNormalRunLoopAni );
	}	
	else if ( m_bSetChargeAttack )
	{
		iAniID = pGrp->GetAnimationIdx( m_szChargeRunLoopAni );
	}	
	else if ( m_bSetJumpAttack )
	{
		iAniID = pGrp->GetAnimationIdx( m_szJumpRunLoopAni );
	} 

	if ( iAniID == -1 )
		return;
	
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );
	
	//무기 생성 (캐릭터에 붙이는)
	CrateWeapon( pOwner );
	
	m_dwCheckLoopTime = dwCurTime;
	
	m_RunningAttackState = RAS_LOOP;	

	//LOG.PrintTimeAndLog( 0, "!!!!#OBJECT_RUNATTACK_SetRunningLoopState");
}

//무기 생성(캐릭터에 붙음)
void ioObjectRunningAttackItem::CrateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwWeaponBase = pOwner->IncreaseWeaponIndexBase();

	//무기 생성
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nCurRunWeaponNum;
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
		kPacket << m_nCurRunWeaponNum;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//달리기 루프 프로세스
void ioObjectRunningAttackItem::ProcessLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	CheckKeyInput( pOwner );
	CheckRotate( pOwner );
	pOwner->SetCurMoveSpeed( m_fCurRunSpeed );

	if ( m_dwCheckLoopTime + m_dwCurRunDuration < dwCurTime )
	{
		m_RunningAttackState = RAS_LOOP_END;
		//무기 제거
		g_WeaponMgr.DestroyReserveZoneWeapon( pOwner->GetCharName() );
	}
}

//키입력
void ioObjectRunningAttackItem::CheckKeyInput( ioBaseChar *pOwner )
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
void ioObjectRunningAttackItem::CheckRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_nCurRunRotateSpeed <= 0 ) 
		return;

	if( m_bTargetRot ) 
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_nCurRunRotateSpeed;
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

//달리기 종료 상태 설정
void ioObjectRunningAttackItem::SetRunningEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	pOwner->SetCurMoveSpeed( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = -1;
	float fTimeRate = FLOAT1;

	if ( m_bSetNormalAttack )
	{
		iAniID = pGrp->GetAnimationIdx( m_szNormalRunEndAni );
		fTimeRate = m_fNormalRunEndAniRate;
	}	
	else if ( m_bSetChargeAttack )
	{
		iAniID = pGrp->GetAnimationIdx( m_szChargeRunEndAni );
		fTimeRate = m_fChargeRunEndAniRate;
	}	
	else if ( m_bSetJumpAttack )
	{
		iAniID = pGrp->GetAnimationIdx( m_szJumpRunEndAni );
		fTimeRate = m_fJumpRunEndAniRate;
	} 

	if ( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunLoopEndTime = dwCurTime;
	m_dwRunLoopEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	//슬라이딩 설정
	pOwner->SetReservedSliding( m_vEndForceInfoList, iAniID, fTimeRate, 0 );

	m_RunningAttackState = RAS_END;

	//LOG.PrintTimeAndLog( 0, "!!!!#OBJECT_RUNATTACK_SetRunningEndState");
}

//특별 종료
void ioObjectRunningAttackItem::SetRunningAttackEndState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwRunLoopEndTime > dwCurTime )
		return;

	pOwner->SetState( CS_DELAY, false );

	if ( pOwner->IsNeedSendNetwork() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << PS_END;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//LOG.PrintTimeAndLog( 0, "!!!!#OBJECT_RUNATTACK_SetRunningAttackEndState");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioObjectRunningAttackSpecialState::ioObjectRunningAttackSpecialState()
{
}

ioObjectRunningAttackSpecialState::~ioObjectRunningAttackSpecialState()
{
}

void ioObjectRunningAttackSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioObjectRunningAttackSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

bool ioObjectRunningAttackSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioObjectItem *pObjectItem = pOwner->GetObject();
	if ( pObjectItem )
	{
		ioObjectRunningAttackItem *pRunningAttackItem = ToObjectRunningAttackItem( pObjectItem );
		if( pRunningAttackItem && pRunningAttackItem->IsCharCollisionSkipState() )
			return true;
	}
	return false;
}

void ioObjectRunningAttackSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioObjectRunningAttackItem *pRunningAttackItem = ToObjectRunningAttackItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pRunningAttackItem )
		pRunningAttackItem->ProcessRunningAttackState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
	}
}