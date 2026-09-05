
#include "stdafx.h"

#include "ioHelipCopterAttachBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

#include "ioDummyAttachBuff3.h"

ioHelipCopterAttachBuff::ioHelipCopterAttachBuff()
{
	ClearData();
}

ioHelipCopterAttachBuff::ioHelipCopterAttachBuff( const ioHelipCopterAttachBuff &rhs )
	: ioBuff( rhs ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_dwCameraEvent( rhs.m_dwCameraEvent ),
	  m_szCameraBuff( rhs.m_szCameraBuff ),
	  m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
	  m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
	  m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
	  m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),
	  m_fCircleRange( rhs.m_fCircleRange ),
	  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	  m_fCircleOffSet( rhs.m_fCircleOffSet ),
	  m_ReadyCircle( rhs.m_ReadyCircle ),
	  m_CurFireWeaponInfo( rhs.m_CurFireWeaponInfo ),
	  m_szDummyName( rhs.m_szDummyName ),
	  m_vPosOffset( rhs.m_vPosOffset ),
	  m_szGaugeBuff( rhs.m_szGaugeBuff ),
	  m_CheckBuffList( rhs.m_CheckBuffList ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_bRotateCircle( rhs.m_bRotateCircle ),
	  m_szLoopAni( rhs.m_szLoopAni ),
	  m_fLoopAniRate( rhs.m_fLoopAniRate ),
	  m_szAttackAni( rhs.m_szAttackAni ),
	  m_fAttackAniRate( rhs.m_fAttackAniRate ),
	  m_bEnableEndJumpByJumpKey( rhs.m_bEnableEndJumpByJumpKey ),
	  m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
	  m_CheckCreatorState( rhs.m_CheckCreatorState ),
	  m_iGaugeSlot( rhs.m_iGaugeSlot ),
	  m_bDontCamCheck( rhs.m_bDontCamCheck )
{
	ClearData();
}

ioHelipCopterAttachBuff::~ioHelipCopterAttachBuff()
{
}

void ioHelipCopterAttachBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_dwCameraEvent = rkLoader.LoadInt_e( "camera_event", 0 );

	rkLoader.LoadString_e( "camera_buff", "", szBuf, MAX_PATH );
	m_szCameraBuff = szBuf;
	
	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	//마법진 설정
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	m_CurFireWeaponInfo.m_nWeaponType = rkLoader.LoadInt_e( "weapon_type", NO_WEAPON_ATTRIBUTE );
	m_CurFireWeaponInfo.m_vWeaponOffset.x = rkLoader.LoadFloat_e( "weapon_offset_x", 0.0f );
	m_CurFireWeaponInfo.m_vWeaponOffset.y = rkLoader.LoadFloat_e( "weapon_offset_y", 0.0f );
	m_CurFireWeaponInfo.m_vWeaponOffset.z = rkLoader.LoadFloat_e( "weapon_offset_z", 0.0f );
	m_CurFireWeaponInfo.m_dwCheckAttackTime = rkLoader.LoadInt_e( "weapon_time", 0 );

	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;

	m_vPosOffset.x = rkLoader.LoadFloat_e( "pos_offset_x", 0.0f );
	m_vPosOffset.y = rkLoader.LoadFloat_e( "pos_offset_y", 0.0f );
	m_vPosOffset.z = rkLoader.LoadFloat_e( "pos_offset_z", 0.0f );
	
	rkLoader.LoadString_e( "gauge_buff", "", szBuf, MAX_PATH );
	m_szGaugeBuff = szBuf;

	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );

	m_bRotateCircle = rkLoader.LoadBool_e( "rotate_circle", false );

	rkLoader.LoadString_e( "loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	m_fLoopAniRate = rkLoader.LoadFloat_e( "loop_ani_rate", 0.0f );
	
	rkLoader.LoadString_e( "attack_ani", "", szBuf, MAX_PATH );
	m_szAttackAni = szBuf;
	m_fAttackAniRate = rkLoader.LoadFloat_e( "attack_ani_rate", 0.0f );

	m_bEnableEndJumpByJumpKey = rkLoader.LoadBool_e( "enable_end_jump_by_jump_key", false );

	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "weapon_float_min_power", 0.0f );

	m_CheckCreatorState = (CharState)rkLoader.LoadInt_e( "check_creator_state", CS_HELICOPTER );

	m_iGaugeSlot = rkLoader.LoadInt_e( "gauge_solt", 0 );

	m_bDontCamCheck = rkLoader.LoadBool_e( "dont_cam_check", false );

	LoadOwnerBuffList( rkLoader );
	LoadCheckBuffList( rkLoader );
}

ioBuff* ioHelipCopterAttachBuff::Clone()
{
	return new ioHelipCopterAttachBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioHelipCopterAttachBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	ClearData();

	if ( !CheckAllBuff( pOwner ) )
	{
		SetReserveEndBuff();
		return;
	}
}

bool ioHelipCopterAttachBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	ClearData();
	
	return true;
}

void ioHelipCopterAttachBuff::ClearData()
{
	m_BuffState = BS_None;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_dwAttackTime = 0;
	m_bSetEndJump = false;
	m_dwFireStartTime = 0;
	m_dwMotionEndTime = 0;
	m_bDestoryDummy = false;
	m_fWeaponMoveSpeed = 0.0f;
	m_fWeaponFloatPower = 0.0f;
}

void ioHelipCopterAttachBuff::ProcessBuff( float fTimePerSec )
{
	if( m_pOwner->IsNeedProcess() && 
		m_dwBuffEndTime <= FRAMEGETTIME() )
	{
		SetEndState( m_pOwner );
		return;
	}

	if ( !CheckGauge( m_pOwner ) )
	{
		m_bDestoryDummy = true;
		SetEndState( m_pOwner, true );
		
		return;
	
	}

	switch ( m_BuffState )
	{
	case BS_None:
		{
			//바로 버프를 검사 하면 지우기 예약 상태이기 때문에 정확한 확인이 불가능.
			if ( !CheckMyBuff( m_pOwner ) )
			{
				SetReserveEndBuff();
				return;
			}

			SetOwnerBuffList( m_pOwner );
			SetMoveState( m_pOwner );
			SetLoopAni( m_pOwner, m_szLoopAni, m_fLoopAniRate );
		}
		break;
	case BS_Move:
		{
			if ( !CheckCreatorState() || !SetPos( m_pOwner ) )
			{
				SetEndState( m_pOwner, true );
				return;
			}

			UpdateMagicCircle( m_pOwner );
			CheckInputKey( m_pOwner );			
		}
		break;
	case BS_Attack:
		{
			DWORD dwCurTime = FRAMEGETTIME();
			if ( m_dwMotionEndTime < dwCurTime )
			{
				m_BuffState = BS_Move;
				SetLoopAni( m_pOwner, m_szLoopAni, m_fLoopAniRate );
				return;
			}

			if ( m_dwFireStartTime &&  m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
				CreateWeapon( m_pOwner );
				return;
			}
		}
		break;
	case BS_End:
		break;
	}
}

void ioHelipCopterAttachBuff::EndBuff()
{
	ProcessEnd( m_pOwner );

	ioBuff::EndBuff();
}

void ioHelipCopterAttachBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case BS_StartGauge:
		{
			SetGaugeStart( pOwner );
		}
		break;
	case BS_CreateWeapon:
		{
			DWORD dwBaseWeaponIndex;
			rkPacket >> dwBaseWeaponIndex;

			D3DXVECTOR3 vPos, vDir;
			rkPacket >> vPos;
			rkPacket >> vDir;
			bool bCheckFloat;
			rkPacket >> bCheckFloat;
			if ( bCheckFloat )
			{
				rkPacket >> m_fWeaponMoveSpeed;
				rkPacket >> m_fWeaponFloatPower;
			}

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = m_CurFireWeaponInfo.m_nWeaponType;
			kFireTime.iResistanceIdx = 0;
			kFireTime.szWoundedAni  = "";
			kFireTime.dwWoundedAniDuration = 0;
			kFireTime.bLoopWoundedAni = false;
			kFireTime.dwWeaponIdx = dwBaseWeaponIndex;
			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( pWeapon )
			{
				pWeapon->SetPosition( vPos );
				pWeapon->SetMoveDir( vDir );
				if ( bCheckFloat )
				{
					pWeapon->SetMoveSpeed( m_fWeaponMoveSpeed );
					pWeapon->SetFloatPower( m_fWeaponFloatPower );
				}
			}
		}
		break;
	case BS_Attack:
		SetAttackState( pOwner );
		break;
	case BS_End:
		{
			rkPacket >> m_bSetEndJump;
			rkPacket >> m_bDestoryDummy;
			SetEndState( pOwner, m_bSetEndJump );
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//버프 리스트 읽기
void ioHelipCopterAttachBuff::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioHelipCopterAttachBuff::SetOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();

	//해당 버프를 생성
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i];

		if( !szBuffName.IsEmpty() )
			//pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );	
			pOwner->ReserveAddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}

//버프 리스트 읽기
void ioHelipCopterAttachBuff::LoadCheckBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "check_buff_cnt", 0 );
	m_CheckBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_CheckBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "check_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_CheckBuffList.push_back( szBuf );
	}
}

//////////////////////////////////////////////////////////////////////////////////
//카메라 설정
void ioHelipCopterAttachBuff::SetCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_szCameraBuff.IsEmpty() )
		//pOwner->AddNewBuff( m_szCameraBuff, "", "", NULL );
		pOwner->ReserveAddNewBuff( m_szCameraBuff, "", "", NULL );
}

void ioHelipCopterAttachBuff::EndCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->RemoveBuff( m_szCameraBuff );
}

void ioHelipCopterAttachBuff::SetCameraEvent( ioBaseChar *pOwner )
{
	if ( !pOwner)
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_CameraEventMgr.CreateCE( m_dwCameraEvent, pStage );
}

void ioHelipCopterAttachBuff::EndCameraEvent( ioBaseChar *pOwner )
{
	ioCameraEvent* pCamera = g_CameraEventMgr.GetCurCameraEvent();
	if( pCamera && pCamera->GetCECode() == m_dwCameraEvent )
	{
		g_CameraEventMgr.NotifyCEDestroyed();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioHelipCopterAttachBuff::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	m_BuffState = BS_Move;

	CharState nState = pOwner->GetState();
	switch( nState )
	{
	case CS_HELICOPTER_ATTACH:
		return;
	}

	pOwner->SetState( CS_HELICOPTER_ATTACH );

	SetPos( pOwner );
	CreateMagicCircle( pOwner );

	if ( pOwner->IsNeedProcess() )
	{
		SetCameraEvent( pOwner );
		SetCameraBuff( pOwner );
	}
}

void ioHelipCopterAttachBuff::SetEndState( ioBaseChar *pOwner, bool bEndJump /*= false */ )
{
	if ( !pOwner )
		return;

	m_BuffState = BS_End;

	m_bSetEndJump = bEndJump;

	SetReserveEndBuff();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_End;
		kPacket << m_bSetEndJump;
		kPacket << m_bDestoryDummy;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHelipCopterAttachBuff::ProcessEnd( ioBaseChar *pOwner )
{
	if ( !pOwner || m_BuffState != BS_End )
		return;
	
	int nBuffCnt = (int)m_OwnerBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->RemoveBuff( m_OwnerBuffList[i], true );	

	if ( !m_szLoopAni.IsEmpty() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if ( !pGrp )
			return;
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
	}

	if ( pOwner->IsFloating() )
	{
		if ( m_bSetEndJump )
			pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, true );
		else
			pOwner->SetFallState( true );
	}
	else
		pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );

	pOwner->SetMagicCircleTarget( "" );
	pOwner->DestroyMagicCircle();

	if ( pOwner->IsNeedProcess() )
	{
		EndCameraBuff( pOwner );
		EndCameraEvent( pOwner );
	}

	if ( m_bDestoryDummy )
		g_DummyCharMgr.DestroyDummyCharByName( m_pOwner, m_szDummyName );

	pOwner->ShowWeaponItemMesh( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioHelipCopterAttachBuff::CheckAllBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	const BaseCharList &pBaseCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = pBaseCharList.begin(); iter != pBaseCharList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) 
			continue;

		if ( pChar->HasBuff( m_CreateChar, m_Name ) )			
			return false;
	}

	return true;
}

bool ioHelipCopterAttachBuff::CheckMyBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;
	
	int nMaxCnt = (int)m_CheckBuffList.size();
	for ( int i=0; i< nMaxCnt; ++i )
	{
		if ( pOwner->HasBuff( m_CreateChar, m_CheckBuffList[i] ) )
			return false;
	}

	return true;
}

bool ioHelipCopterAttachBuff::CheckCreatorState()
{
	ioBaseChar *pCreator = GetCreator();
	if ( !pCreator )
		return false;

	if ( pCreator->GetState() != m_CheckCreatorState )
		return false;

	return true;
}

bool ioHelipCopterAttachBuff::SetPos( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioBaseChar *pCreator = GetCreator();
	if ( !pCreator )
		return false;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pCreator, m_szDummyName );
	if ( !pDummy )
		return false;

	D3DXQUATERNION qtRot = pDummy->GetWorldOrientation();
	D3DXVECTOR3 vNewPos = pDummy->GetWorldPosition() + qtRot * m_vPosOffset;

	pOwner->SetWorldPosition( vNewPos );
	if ( m_bRotateCircle && m_vCirclePos != ioMath::VEC3_ZERO )
		m_pOwner->SetTargetRotToTargetPos( m_vCirclePos, true );
	else
		pOwner->SetTargetRotToRotate( qtRot, true );

	return true;
}

bool ioHelipCopterAttachBuff::CheckEnableUseSkill( int iSlot )
{
	switch( iSlot )
	{
	case ES_WEAPON:
		if( m_bEnableWeaponSkill )
			return true;
		break;
	case ES_ARMOR:
		if( m_bEnableArmorSkill )
			return true;
		break;
	case ES_HELMET:
		if( m_bEnableHelmetSkill )
			return true;
		break;
	case ES_CLOAK:
		if( m_bEnableCloakSkill )
			return true;
		break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
//마법진 생성
void ioHelipCopterAttachBuff::CreateMagicCircle( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( !pOwner->CheckMagicCircle() )
	{
		pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fCircleOffSet );
	}
}

//마법진 업데이트
void ioHelipCopterAttachBuff::UpdateMagicCircle( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->CheckMagicCircle() )
	{
		//뱡향 체크
		pOwner->CheckChangeDirectionForMagicCircle();
		//위치 갱신
		pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

		m_vCirclePos = pOwner->GetMagicCirclePos( m_fCircleOffSet, false );
	}
}

//////////////////////////////////////////////////////////////////////////////////

void ioHelipCopterAttachBuff::CheckInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( pOwner->IsJumpKey() )
	{
		if ( m_CreateChar != pOwner->GetCharName() )
		{
			SetEndState( m_pOwner, m_bEnableEndJumpByJumpKey );
			return;
		}
	}

	if ( pOwner->IsAttackKey() )
	{
		if ( !m_szAttackAni.IsEmpty() )
			SetAttackState( pOwner );
		else
		{
			DWORD dwCurTime = FRAMEGETTIME();
			if ( m_dwAttackTime + m_CurFireWeaponInfo.m_dwCheckAttackTime < dwCurTime )
			{
				if ( m_dwAttackTime == 0 )
					SetGaugeStart( pOwner );

				m_dwAttackTime = dwCurTime;
				CreateWeapon( pOwner );
			}
		}
	}
}

void ioHelipCopterAttachBuff::SetGaugeStart( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioBaseChar *pCreater = GetCreator();
	if ( !pCreater )
		return;

	ioBuff *pBuff = pCreater->GetBuff( m_szGaugeBuff );
	ioDummyAttachBuff3 *pDummyAttachBuff3 = ToDummyAttachBuff3( pBuff );
	if ( pDummyAttachBuff3 )
		pDummyAttachBuff3->SetReduceStartGauge();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_StartGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHelipCopterAttachBuff::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioBaseChar *pCreator = GetCreator();
	if ( !pCreator )
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pCreator, m_szDummyName );
	if ( !pDummy )
		return;

	DWORD dwBaseWeaponIndex = pOwner->GetWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_CurFireWeaponInfo.m_nWeaponType;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;		

	D3DXQUATERNION qtRot = pDummy->GetWorldOrientation();
	D3DXVECTOR3 vPos = pDummy->GetWorldPosition() + qtRot * m_CurFireWeaponInfo.m_vWeaponOffset;
	D3DXVECTOR3 vDir = m_vCirclePos - vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	bool bCheckFloat = false;
	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
	{
		pWeapon->SetPosition( vPos );
		if ( CheckWeaponFloatInfo( pOwner, pWeapon, pDummy ) )
		{
			bCheckFloat = true;
			vDir.y = 0.0f;
			D3DXVec3Normalize( &vDir, &vDir );
			pWeapon->SetMoveDir( vDir );
			pWeapon->SetMoveSpeed( m_fWeaponMoveSpeed );
			pWeapon->SetFloatPower( m_fWeaponFloatPower );
		}
		else
			pWeapon->SetMoveDir( vDir );
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CreateWeapon;
		kPacket << dwBaseWeaponIndex;
		kPacket << vPos;
		kPacket << vDir;
		kPacket << bCheckFloat;
		if ( bCheckFloat )
		{
			kPacket << m_fWeaponMoveSpeed;
			kPacket << m_fWeaponFloatPower;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioHelipCopterAttachBuff::CheckGauge( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( m_iGaugeSlot > ES_OBJECT )
		return false;

	if( pOwner->IsNeedProcess()  && GetCreatorName() == pOwner->GetCharName() && GetCurTicGauge() != 0.0f )
	{
		float fCurTicGauge = GetCurTicGauge();
		int iTickGap = GetCurTicGap();
		int iCurTicTime = GetCurTicTime();

		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = FRAMEGETTIME() - ( iTickGap - iCurTicTime );

			if ( m_ItemName.IsEmpty() )
			{
				ioItem *pItem = pOwner->GetEquipedItem( m_iGaugeSlot );
				if ( !pItem || !pItem->HasSkillGauge() )
					return false;

				pItem->ReduceSkillGauge( fCurTicGauge );

				if ( pItem->GetCurSkillGuage() <= 0.0f )
					return false;
			}
			else
			{
				if( m_pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, fCurTicGauge ) <= 0.0f )
					return false;
			}
			

			
		}
	}

	return true;
}

void ioHelipCopterAttachBuff::SetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_BuffState = BS_Attack;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szAttackAni );
	if( iAniID == -1 )	
		return;

	float fTimeRate = FLOAT1;
	if ( m_fAttackAniRate > 0.0f )
		fTimeRate = m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = m_dwFireStartTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Attack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHelipCopterAttachBuff::SetLoopAni( ioBaseChar *pOwner, const ioHashString& szAniName, float fRate )
{
	if ( !pOwner )
		return;

	if ( szAniName.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( szAniName );
	if( iAniID == -1 )	
		return;

	if ( fRate <= 0.0f )
		fRate = FLOAT1;

	pOwner->ShowWeaponItemMesh( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );
}

bool ioHelipCopterAttachBuff::CheckWeaponFloatInfo( ioBaseChar *pOwner, ioWeapon *pWeapon, ioDummyChar *pDummy )
{
	if ( m_fWeaponFloatMinPower <= 0.0f )
		return false;

	if ( !pOwner || !pWeapon )
		return false;

	const WeaponAttribute *pAttr = g_WeaponMgr.FindAttribute( pWeapon->GetAttributeIndex() );
	if ( !pAttr )
		return false;

	m_fWeaponFloatPower = 0.0f;
	m_fWeaponMoveSpeed = 0.0f;

	D3DXVECTOR3 vWeaponPos = ioMath::VEC3_ZERO;
	if ( pDummy )
	{
		D3DXQUATERNION qtRot = pDummy->GetWorldOrientation();
		vWeaponPos = pDummy->GetWorldPosition() + qtRot * m_CurFireWeaponInfo.m_vWeaponOffset;
	}
	else
		vWeaponPos = pOwner->GetWorldPosition();

	D3DXVECTOR3 vWeapnDir = m_vCirclePos - vWeaponPos;
	float fRange = D3DXVec3Length( &vWeapnDir );
	vWeapnDir.y = 0.0f;
	D3DXVec3Normalize( &vWeapnDir, &vWeapnDir );

	if( fRange > 0.0f )
	{
		float fRate = fRange / m_fCircleRange ;
		m_fWeaponFloatPower = pAttr->m_fFloatPower * fRate;
		m_fWeaponFloatPower = max( m_fWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * FLOAT1;

		float fFlightDuration = 0.0f;
		float fGapHeight = pOwner->GetMidHeightByRate(1.0f) - vWeaponPos.y;
		float fGapHeight2 = vWeaponPos.y - m_vCirclePos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, m_fWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				m_fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);
		}
		else if( COMPARE(fGapHeight2, 0.0f, -20.0f) && fRange > 0.0f )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, m_fWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				m_fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, m_fWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				m_fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);
		}

		return true;
	}

	return false;
}

float ioHelipCopterAttachBuff::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fGapTime;
	fTopTime = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
	}

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01;
		fGapTime += 0.01;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float ioHelipCopterAttachBuff::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioHelipCopterAttachSpecialState::ioHelipCopterAttachSpecialState()
{
}

ioHelipCopterAttachSpecialState::~ioHelipCopterAttachSpecialState()
{
}

void ioHelipCopterAttachSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioHelipCopterAttachSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioHelipCopterAttachSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{	
}

void ioHelipCopterAttachSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioHelipCopterAttachSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}