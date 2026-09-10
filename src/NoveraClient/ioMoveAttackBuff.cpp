
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMoveAttackBuff.h"

#include "WeaponDefine.h"

ioMoveAttackBuff::ioMoveAttackBuff()
{
	Init();
}

ioMoveAttackBuff::ioMoveAttackBuff( const ioMoveAttackBuff &rhs )
	: ioBuff( rhs ),
      m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
	  m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
	  m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
	  m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_RemoveBuffList( rhs.m_RemoveBuffList ),
	  m_nTotalWeaponCnt( rhs.m_nTotalWeaponCnt ),
	  m_fStartJumpPower( rhs.m_fStartJumpPower ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_dwCameraEvent( rhs.m_dwCameraEvent ),
	  m_szCameraBuff( rhs.m_szCameraBuff ),
	  m_EnableInputKeyState( rhs.m_EnableInputKeyState ),
	  m_BuffEndState( rhs.m_BuffEndState ),
	  m_NormalAttack( rhs.m_NormalAttack )
{
	Init();
}

ioMoveAttackBuff::~ioMoveAttackBuff()
{	
	m_EnableInputKeyState.clear();
	m_BuffEndState.clear();
}

void ioMoveAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH] = "";
	int nCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	char szRemoveBuf[MAX_PATH] = "";
	for (int i=0; i<nCnt; ++i)
	{
		wsprintf_e( szRemoveBuf, "remove_buff%d_name",i+1);
		rkLoader.LoadString( szRemoveBuf, "", szBuf, MAX_PATH );
		m_RemoveBuffList.push_back( szBuf );
	}

	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	
	m_dwCameraEvent = rkLoader.LoadInt_e( "camera_event", 0 );

	rkLoader.LoadString_e( "camera_buff", "", szBuf, MAX_PATH );
	m_szCameraBuff = szBuf;

	//추가 읽기
	LoadExtraProperty( rkLoader );
}

ioBuff* ioMoveAttackBuff::Clone()
{
	return new ioMoveAttackBuff( *this );
}

void ioMoveAttackBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	Init();

	m_iMaxActiveCount = m_nTotalWeaponCnt;
	m_iCurActiveCount = m_nTotalWeaponCnt;
		
	if ( pOwner->IsNeedProcess() )
	{
		SetCameraEvent( pOwner );
		SetCameraBuff( pOwner );
	}

	pOwner->SetCurMoveSpeed(0.0f);
	pOwner->SetJumpPower(0.0f);
	pOwner->SetGravityAmt( 0.0f );
	//공중에 있는 상태
	if ( pOwner->IsFloating() )
	{
		if ( m_fStartJumpPower > 0.0f )
			pOwner->SetSKillEndJumpState( m_fStartJumpPower, false, false, false, true );
	}
}

bool ioMoveAttackBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	Init();

	m_iMaxActiveCount = m_nTotalWeaponCnt;
	m_iCurActiveCount = m_nTotalWeaponCnt;

	pOwner->SetCurMoveSpeed(0.0f);
	pOwner->SetJumpPower(0.0f);
	pOwner->SetGravityAmt( 0.0f );
	//공중에 있는 상태
	if ( pOwner->IsFloating() )
	{
		if ( m_fStartJumpPower > 0.0f )
			pOwner->SetSKillEndJumpState( m_fStartJumpPower, false, false, false, true );
	}

	return true;
}

void ioMoveAttackBuff::ProcessBuff( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( CheckWoundState( m_pOwner ) || CheckBuffEndState( m_pOwner ) || m_dwBuffEndTime < dwCurTime )
	{
		SetReserveEndBuff();
		return;
	}

	switch ( m_BuffState )
	{
	case BS_Delay:
		CheckInputKey( m_pOwner );
		break;
	case BS_Attack:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_nCurWeaponCnt >= m_nTotalWeaponCnt )
					m_BuffState = BS_End;
				else
				{
					m_pOwner->SetState( CS_DELAY, false );
					m_BuffState = BS_Delay;
				}
			}
		}
		break;
	case BS_End:
			SetReserveEndBuff();
		break;
	}
}

void ioMoveAttackBuff::EndBuff()
{	
	ioBuff::EndBuff();
	
	if ( m_pOwner->IsNeedProcess() )
	{
		EndCameraBuff( m_pOwner );
		EndCameraEvent( m_pOwner );
	}

	//피격 상태가 아니면서
	if ( !m_bWound )
	{
		m_pOwner->SetCurMoveSpeed(0.0f);
		m_pOwner->SetJumpPower(0.0f);
		m_pOwner->SetGravityAmt( 0.0f );
		//공중에 있는 상태
		if ( m_pOwner->IsFloating() )
		{
			if ( m_fEndJumpPower > 0.0f )
				m_pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false );
		}
		else
			m_pOwner->SetState( CS_DELAY );
	}

	//버프 제거
	for ( int i=0; i<(int)m_RemoveBuffList.size(); ++i )
		m_pOwner->RemoveBuff( m_RemoveBuffList[i], true );
	m_RemoveBuffList.clear();
}

void ioMoveAttackBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case BS_Attack:
		{
			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			SetAttackState( pOwner );
		}
		break;
	}
}

bool ioMoveAttackBuff::CheckEnableUseSkill( int iSlot )
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

/////////////////////////////////////////////////////////////////////////////////////

void ioMoveAttackBuff::Init()
{
	m_dwMotionEndTime = 0;
	m_dwWeaponFireTime = 0;
	m_nCurWeaponCnt = 0;

	m_bWound = false;
	
	m_BuffState = BS_Delay;
}

void ioMoveAttackBuff::LoadExtraProperty( ioINILoader &rkLoader )
{
	m_nTotalWeaponCnt = rkLoader.LoadInt_e( "total_weapon_count", 0 );

	char szBuf[MAX_PATH] = "";		
	m_fStartJumpPower = rkLoader.LoadFloat_e( "start_jump_power", 0.0f );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );
	
	m_EnableInputKeyState.clear();
	int nMaxState = rkLoader.LoadInt_e( "max_enable_input_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szBuf, "enable_input_state%d", i+1 );
		int nState = rkLoader.LoadInt( szBuf, -1 );
		m_EnableInputKeyState.push_back( nState );
	}

	m_BuffEndState.clear();
	nMaxState = rkLoader.LoadInt_e( "max_buff_end_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szBuf, "buff_end_state%d", i+1 );
		int nState = rkLoader.LoadInt( szBuf, -1 );
		m_BuffEndState.push_back( nState );
	}

	LoadAttackAttribute( "normal_attack", m_NormalAttack, rkLoader );
}

bool ioMoveAttackBuff::CheckWoundState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	CharState eState = pOwner->GetState();
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && pOwner->GetBlowFrozenState() )
			m_bWound = true;
		else if( eState == CS_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			m_bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			m_bWound = true;
		else if( eState == CS_ETC_STATE && pOwner->IsApplyDownState(false) )
			m_bWound = true;
		else if( eState == CS_STOP_MOTION && pOwner->GetBlowStopMotionState() )
			m_bWound = true;
		else if( eState == CS_ICE_STATE && pOwner->GetBlowStopMotionState() )
			m_bWound = true;
	}

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
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioMoveAttackBuff::CheckEnableKey( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();
	for ( int i=0; i<(int)m_EnableInputKeyState.size(); ++i )
	{
		if ( nOwnerState == m_EnableInputKeyState[i] )
			return true;
	}
	
	return false;
}

bool ioMoveAttackBuff::CheckBuffEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();
	for ( int i=0; i<(int)m_BuffEndState.size(); ++i )
	{
		if ( nOwnerState == m_BuffEndState[i] )
		{
			m_bWound = true;
			return true;
		}
	}

	return false;
}

void ioMoveAttackBuff::CheckInputKey(  ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() || !CheckEnableKey( pOwner ) )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime > dwCurTime )
		return;

	if ( CheckAttackKey( pOwner ) )
		return;
}

bool ioMoveAttackBuff::CheckAttackKey( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )//|| pOwner->GetState() == CS_JUMP )
		return false;

	if( m_nCurWeaponCnt < m_nTotalWeaponCnt && 
		pOwner->IsAttackKey() )
	{		
		SetAttackState( pOwner );
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////

void ioMoveAttackBuff::SetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetState( CS_BUFF_ACTION );
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_NormalAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_NormalAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_NormalAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_NormalAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_NormalAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_BuffState = BS_Attack;

	m_nCurWeaponCnt++;
	m_iCurActiveCount = m_nTotalWeaponCnt - m_nCurWeaponCnt;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << BS_Attack;
			kPacket << (int)eNewDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
//카메라 설정
void ioMoveAttackBuff::SetCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_szCameraBuff.IsEmpty() )
		//pOwner->AddNewBuff( m_szCameraBuff, "", "", NULL );
		pOwner->ReserveAddNewBuff( m_szCameraBuff, "", "", NULL );
}

void ioMoveAttackBuff::EndCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->RemoveBuff( m_szCameraBuff );
}

void ioMoveAttackBuff::SetCameraEvent( ioBaseChar *pOwner )
{
	if ( !pOwner)
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_CameraEventMgr.CreateCE( m_dwCameraEvent, pStage );
}

void ioMoveAttackBuff::EndCameraEvent( ioBaseChar *pOwner )
{
	ioCameraEvent* pCamera = g_CameraEventMgr.GetCurCameraEvent();
	if( pCamera && pCamera->GetCECode() == m_dwCameraEvent )
	{
		g_CameraEventMgr.NotifyCEDestroyed();
	}
}