

#include "stdafx.h"

#include "ioRollingDash.h"

#include "ioBaseChar.h"


#include "ItemDefine.h"

ioRollingDash::ioRollingDash()
{
	m_dwActionEndTime = 0;
}

ioRollingDash::ioRollingDash( const ioRollingDash &rhs )
: ioExtendDash( rhs ),
  m_DashAction( rhs.m_DashAction ),
  m_dwActionDelay( rhs.m_dwActionDelay ),
  m_fActionTimeRate( rhs.m_fActionTimeRate ),
  m_dwActionPreDelay( rhs.m_dwActionPreDelay ),
  m_vForceInfoList( rhs.m_vForceInfoList ),
  m_bUseDashEndDir( rhs.m_bUseDashEndDir ),
  m_bEnableDashEndAttack( rhs.m_bEnableDashEndAttack ),
  m_DashEndDAttack( rhs.m_DashEndDAttack ),
  m_DashEndSAttack( rhs.m_DashEndSAttack ),
  m_DashEndAAttack( rhs.m_DashEndAAttack )
{
	m_dwActionEndTime = 0;
	m_bUseDashEndAttack = false;
}

ioExtendDash* ioRollingDash::Clone()
{
	return new ioRollingDash( *this );
}

ioRollingDash::~ioRollingDash()
{
}

void ioRollingDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "dash_action", "", szBuf, MAX_PATH );
	m_DashAction = szBuf;

	m_dwActionDelay = rkLoader.LoadInt_e( "dash_action_delay", 0 );
	m_dwActionPreDelay = rkLoader.LoadInt_e( "dash_action_pre_delay", 0 );
	m_fActionTimeRate = rkLoader.LoadFloat_e( "dash_action_time_rate", FLOAT1 );

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "dash_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		m_vForceInfoList.reserve( iForceCnt );
	}

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "dash_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "dash_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vForceInfoList.push_back( kInfo );
	}

	m_bUseDashEndDir = rkLoader.LoadBool_e( "dash_end_dir", false );
	m_bEnableDashEndAttack = rkLoader.LoadBool_e( "enable_dash_end_attack", false );

	LoadAttackAttribute_e( "dash_end_d_attack", m_DashEndDAttack, rkLoader );
	LoadAttackAttribute_e( "dash_end_s_attack", m_DashEndSAttack, rkLoader );
	LoadAttackAttribute_e( "dash_end_a_attack", m_DashEndAAttack, rkLoader );
}

DashType ioRollingDash::GetType() const
{
	return DT_ROLLING_AIM;
}

bool ioRollingDash::StartDash( ioBaseChar *pOwner )
{
	if( m_DashAction.IsEmpty() )
		return false;

	ioExtendDash::StartDash( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_DashAction );
	if( iAniID == -1 )
		return false;

	m_dwActionEndTime = 0;
	m_bUseDashEndAttack = false;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100,	1.0f, FLOAT1/m_fActionTimeRate, 0.0f, m_dwActionPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * m_fActionTimeRate;
	dwDuration += m_dwActionPreDelay;

	CheckKeyReserveTime( pOwner, iAniID, m_fActionTimeRate, m_dwActionPreDelay );

	m_dwActionEndTime = dwCurTime;
	m_dwActionEndTime += dwDuration;

	pOwner->CheckCharColSkipTime( iAniID, m_fActionTimeRate, m_dwActionPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetReservedSliding( m_vForceInfoList, iAniID, m_fActionTimeRate, m_dwActionPreDelay );
	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );
	
	g_TutorialMgr.ActionRollingDash();

	return true;
}

void ioRollingDash::CheckKeyReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 && !pOwner->IsChatModeState( true ) )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill && !pOwner->IsChatModeState( true ) )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill && !pOwner->IsChatModeState( true ) )
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


void ioRollingDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true, false );

	int iSpecialState;
	rkPacket >> iSpecialState;
	if( iSpecialState != SS_NONE )
		ChangeToSpecialAttackFire( pOwner, iSpecialState );
}

void ioRollingDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + m_dwActionDelay + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{			
			// Check Reload
			if( !m_bReloadCheck )
			{
				m_bReloadCheck = true;
				CheckReload( pOwner );
			}

			if( m_bEnableDashEndAttack && !m_bUseDashEndAttack )
			{
				if( pOwner->IsAttackKey() )
				{
					ChangeToSpecialAttackFire( pOwner, SS_DASH_END_D );
					return;
				}
				else if( pOwner->IsDefenseKey() )
				{
					ChangeToSpecialAttackFire( pOwner, SS_DASH_END_S );
					return;
				}
				else if( pOwner->IsJumpKey() )
				{
					ChangeToSpecialAttackFire( pOwner, SS_DASH_END_A );
					return;
				}
			}
			else
				CheckKeyReserve( pOwner );
		}
	}
	else
	{
		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					if( m_bUseDashEndDir )
					{
						ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();
						if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
						{
							float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
							fYawD = ioMath::ArrangeHead( fYawD );

							float fYawR = DEGtoRAD( fYawD );
							D3DXQUATERNION qtRotate;
							D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

							pOwner->SetTargetRotToRotate( qtRotate, true, false );
						}
					}

					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}
		
		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
		}

		// 대기
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pOwner->SetState( CS_DELAY );
	}
}

void ioRollingDash::ChangeToSpecialAttackFire( ioBaseChar *pOwner, int iSpecialState )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_bEnableJumpState = false;
	m_bEnableAttackState = false;
	m_bEnableDefenseState = false;
	m_bUseDashEndAttack = true;
	m_iSkillInput = -1;

	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	AttackAttribute cInfo;
	switch( iSpecialState )
	{
	case SS_DASH_END_D:	cInfo = m_DashEndDAttack;	break;
	case SS_DASH_END_S:	cInfo = m_DashEndSAttack;	break;
	case SS_DASH_END_A:	cInfo = m_DashEndAAttack;	break;
	}

	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( cInfo, true );

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	float fTimeRate = cInfo.m_fAttackAniRate;
	DWORD dwPreDelay = cInfo.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwActionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * cInfo.m_fAttackAniRate );
	CheckKeyReserveTime( pOwner, iAniID, fTimeRate, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << pOwner->GetTargetRot();
		kPacket << iSpecialState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}