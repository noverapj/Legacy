
#include "stdafx.h"

#include "ioTargetMoveBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "SkillDefine.h"

ioTargetMoveBuff::ioTargetMoveBuff()
{
	
}

ioTargetMoveBuff::ioTargetMoveBuff( const ioTargetMoveBuff &rhs )
	: ioBuff( rhs ),
	  m_dwCameraEvent( rhs.m_dwCameraEvent ),
	  m_szCameraBuff( rhs.m_szCameraBuff ),
	  m_fJumpPower( rhs.m_fJumpPower ),
	  m_fGravityRate( rhs.m_fGravityRate ),
	  m_fCircleRange( rhs.m_fCircleRange ),
	  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	  m_fCircleOffSet( rhs.m_fCircleOffSet ),
	  m_ReadyCircle( rhs.m_ReadyCircle ),
	  m_szDelayAni( rhs.m_szDelayAni ),
	  m_dwDelayTime( rhs.m_dwDelayTime ),
	  m_szJumpAni( rhs.m_szJumpAni ),
	  m_fJumpAniRate( rhs.m_fJumpAniRate ),
	  m_szAttackAni( rhs.m_szAttackAni ),
	  m_fAttackAniRate( rhs.m_fAttackAniRate ),
	  m_AttackAttribute( rhs.m_AttackAttribute ),
	  m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
	  m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
	  m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
	  m_bEnableCloakSkill( rhs.m_bEnableCloakSkill )
{
}

ioTargetMoveBuff::~ioTargetMoveBuff()
{

}

void ioTargetMoveBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_dwCameraEvent = rkLoader.LoadInt_e( "camera_event", 0 );

	rkLoader.LoadString_e( "camera_buff", "", szBuf, MAX_PATH );
	m_szCameraBuff = szBuf;

	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	m_fGravityRate = rkLoader.LoadFloat_e( "gravity_rate", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	//마법진 설정
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "pre_delay_ani", "", szBuf, MAX_PATH );
	m_szDelayAni = szBuf;
	m_dwDelayTime = rkLoader.LoadInt_e( "pre_delay_time", 0 );

	rkLoader.LoadString_e( "jump_ani", "", szBuf, MAX_PATH );
	m_szJumpAni = szBuf;
	m_fJumpAniRate = rkLoader.LoadFloat_e( "jump_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "attack_ani", "", szBuf, MAX_PATH );
	m_szAttackAni = szBuf;
	m_fAttackAniRate = rkLoader.LoadFloat_e( "attack_ani_rate", FLOAT1 );
	
	m_AttackAttribute.m_vForceInfoList.clear();
	int nForceCnt = rkLoader.LoadInt_e( "attack_force_cnt", 0 );
	if( nForceCnt > 0 )
	{
		m_AttackAttribute.m_vForceInfoList.reserve( nForceCnt );
	}

	for( int i=0; i < nForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_AttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int nAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 1 );
	m_AttackAttribute.m_vWeaponInfoList.clear();
	m_AttackAttribute.m_vWeaponInfoList.reserve( nAttackTypeCnt );

	for( int i=0 ; i<nAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}

	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );
}

ioBuff* ioTargetMoveBuff::Clone()
{
	return new ioTargetMoveBuff( *this );
}

void ioTargetMoveBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_BuffState = BS_NONE;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_fFlightDuration = 0.0f;
	m_dwCheckJumpTime = 0;
	m_vMoveDir = ioMath::VEC3_ZERO;
	m_fMoveSpeed = 0.0f;
	m_dwAttackEndTime = 0;
	m_bWound = false;
}

bool ioTargetMoveBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_BuffState = BS_NONE;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_fFlightDuration = 0.0f;
	m_dwCheckJumpTime = 0;
	m_vMoveDir = ioMath::VEC3_ZERO;
	m_fMoveSpeed = 0.0f;
	m_dwAttackEndTime = 0;
	m_bWound = false;
	
	return true;
}

void ioTargetMoveBuff::ProcessBuff( float fTimePerSec )
{	
	if( CheckWoundState( m_pOwner ) )
	{ 
		SetEndState( m_pOwner );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_BuffState )
	{
	case BS_NONE:
		{
			SetPreDelayState( m_pOwner );
			DestroyObjectItemSkill( m_pOwner );
		}
		break;
	case BS_DELAY:
		{
			if ( m_dwCheckDelay + m_dwDelayTime < dwCurTime )
			{
				SetJumpState( m_pOwner );
			}
		}
		break;
	case BS_JUMP:
		{
			SetMoveState( m_pOwner );
		}
		break;
	case BS_MOVE:
		{
			if( IsFloatState( m_pOwner ) )
			{
				UpdateMagicCircle( m_pOwner );
				CheckMove( m_pOwner );
			}
			else
			{
				SetAttackState( m_pOwner );
			}
		}
		break;
	case BS_ATTACK:
		{
			if ( m_dwAttackEndTime < dwCurTime )
			{
				SetEndState( m_pOwner );
			}
		}
		break;
	}
}

void ioTargetMoveBuff::EndBuff()
{	
	if ( m_BuffState != BS_END )
	{
		SetEndState( m_pOwner );
	}

	ioBuff::EndBuff();
}

void ioTargetMoveBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{	
}

//////////////////////////////////////////////////////////////////////////////////
//카메라 설정
void ioTargetMoveBuff::SetCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_szCameraBuff.IsEmpty() )
		//pOwner->AddNewBuff( m_szCameraBuff, "", "", NULL );
		pOwner->ReserveAddNewBuff( m_szCameraBuff, "", "", NULL );
}

void ioTargetMoveBuff::EndCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->RemoveBuff( m_szCameraBuff );
}

void ioTargetMoveBuff::SetCameraEvent( ioBaseChar *pOwner )
{
	if ( !pOwner)
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_CameraEventMgr.CreateCE( m_dwCameraEvent, pStage );
}

void ioTargetMoveBuff::EndCameraEvent( ioBaseChar *pOwner )
{
	ioCameraEvent* pCamera = g_CameraEventMgr.GetCurCameraEvent();
	if( pCamera && pCamera->GetCECode() == m_dwCameraEvent )
	{
		g_CameraEventMgr.NotifyCEDestroyed();
	}
}

//////////////////////////////////////////////////////////////////////////////////
//마법진 생성
void ioTargetMoveBuff::CreateMagicCircle( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( !pOwner->CheckMagicCircle() )
	{
		pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fCircleOffSet );
	}
}

//마법진 업데이트
void ioTargetMoveBuff::UpdateMagicCircle( ioBaseChar *pOwner )
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

////////////////////////////////////////////////////////////////////////////////
//상태 변경
void ioTargetMoveBuff::SetCharBuffState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	CharState nState = pOwner->GetState();
	switch( nState )
	{
	case CS_BUFF_ACTION:
		return;
	}

	pOwner->SetState( CS_BUFF_ACTION );
}

void ioTargetMoveBuff::SetPreDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szDelayAni );

	if ( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	SetCharBuffState( pOwner );

	if ( pOwner->IsNeedProcess() )
	{
		SetCameraEvent( pOwner );
		SetCameraBuff( pOwner );
	}

	m_dwCheckDelay = FRAMEGETTIME();

	m_BuffState = BS_DELAY;
}

void ioTargetMoveBuff::SetJumpState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szJumpAni );

	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fJumpAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_BuffState = BS_JUMP;
}

void ioTargetMoveBuff::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	float fCurGravity = pOwner->GetCharGravity();
	m_fFlightDuration = GetFlightDuration( fCurGravity );

	pOwner->SetJumpPower( m_fJumpPower );	
	pOwner->SetGravityBuffInfo( m_Name, m_fGravityRate );

	CreateMagicCircle( m_pOwner );

	m_dwCheckJumpTime = FRAMEGETTIME();
	m_BuffState = BS_MOVE;
}

void ioTargetMoveBuff::SetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szAttackAni );

	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fAttackAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->RefreshFireTimeList( iAniID, m_AttackAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackEndTime = dwCurTime;
	m_dwAttackEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_BuffState = BS_ATTACK;
}

void ioTargetMoveBuff::SetEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	pOwner->SetMagicCircleTarget( "" );
	pOwner->DestroyMagicCircle();

	if ( pOwner->IsNeedProcess() )
	{
		EndCameraBuff( pOwner );
		EndCameraEvent( pOwner );
	}

	pOwner->RemoveGravityBuffInfo( m_Name );

	if ( !m_bWound )
	{
		pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}

	SetReserveEndBuff();

	m_BuffState = BS_END;
}

//////////////////////////////////////////////////////////////////////////////////////////
//이동 시간 얻기
float ioTargetMoveBuff::GetFlightDuration( float fCurGravity )
{
	float fTopTime;
	fTopTime = 0.0f;

	float fNewGravity = fCurGravity * m_fGravityRate;
	if( fNewGravity > 0.0f )
		fTopTime = m_fJumpPower / fNewGravity;

	float fResult = 2 * fTopTime * FLOAT1000;

	return fResult;
}

//이동확인
void ioTargetMoveBuff::CheckMove( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	//방향 얻기
	D3DXVECTOR3 vDir = m_vCirclePos - pOwner->GetWorldPosition();
	vDir.y = 0.0f;

	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		float fGapTime = m_fFlightDuration - ( dwCurTime - m_dwCheckJumpTime );

		float fNewSpeed = m_fMoveSpeed;
		if( fGapTime > 0 )
		{
			fNewSpeed = fRange / (float)(fGapTime / FLOAT1000);
		}

		m_fMoveSpeed = fNewSpeed;

		//이동 방향 설정
		D3DXVec3Normalize( &m_vMoveDir, &vDir );
	}

	if( m_fMoveSpeed == 0.0f ) 
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vMoveDir * m_fMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
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
			pOwner->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	//캐릭터 이동방향으로 회전
	pOwner->SetTargetRotToDir( m_vMoveDir, true );
}

//떠있는 상태인가
bool ioTargetMoveBuff::IsFloatState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;
	
	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
		return true;

	return false;
}

//캐릭터 통과 상태
bool ioTargetMoveBuff::IsCharCollisionSkipState()
{
	switch( m_BuffState )
	{
	case BS_MOVE:
		return true;
	}

	return false;
}

//상태 확인
bool ioTargetMoveBuff::CheckWoundState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	CharState eState = pOwner->GetState();

	if( m_bEnableWoundCancel )
	{
		switch( eState )
		{
		case CS_BLOW_WOUNDED:
		case CS_BOUND_BLOW_WOUNDED:
		case CS_WOUNDED:
		case CS_DROP_ZONE_DOWN:
			m_bWound = true;
			break;
		}

		if( m_bWound )
		{
			return true;
		}
	}

	return false;
}

bool ioTargetMoveBuff::CheckEnableUseSkill( int iSlot )
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

void ioTargetMoveBuff::DestroyObjectItemSkill( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	for( int i=0; i < ES_OBJECT; i++ )
	{
		ioCreateItemBuffSkill *pBuffSkill = ToCreateItemBuffSkill( pOwner->GetEquipedSkill( i ) );
		if( pBuffSkill )
			pBuffSkill->RemoveSkillBuff( pOwner );
	}
}