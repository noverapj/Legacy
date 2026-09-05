

#include "stdafx.h"

#include "ioTeleportDash.h"

#include "ioBaseChar.h"


#include "ItemDefine.h"

ioTeleportDash::ioTeleportDash()
{
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_DashState = DS_NONE;

	m_vTeleportPos = ioMath::VEC3_ZERO;
	m_vTargetPos = ioMath::VEC3_ZERO;

	m_dwReserveAttackTime = 0;

	m_bEnableTeleportDash = false;

	m_dwTeleportEndDelayTime = 0;
}

ioTeleportDash::ioTeleportDash( const ioTeleportDash &rhs )
: ioExtendDash( rhs ),
  m_TeleportDashStartAction( rhs.m_TeleportDashStartAction ),
  m_fDashSpeedRate( rhs.m_fDashSpeedRate ),
  m_fTeleportOffset( rhs.m_fTeleportOffset ),
  m_szTeleportStartEffect( rhs.m_szTeleportStartEffect ),
  m_szTeleportEndEffect( rhs.m_szTeleportEndEffect ),
  m_TeleportDashEndAction( rhs.m_TeleportDashEndAction ),
  m_fOtherActionJumpPower( rhs.m_fOtherActionJumpPower ),
  m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
  m_TeleportJumpAttribute( rhs.m_TeleportJumpAttribute ),
  m_fTeleportJumpPower( rhs.m_fTeleportJumpPower ),
  m_dwTeleportEndDelay( rhs.m_dwTeleportEndDelay )
{
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_DashState = DS_NONE;

	m_vTeleportPos = ioMath::VEC3_ZERO;
	m_vTargetPos = ioMath::VEC3_ZERO;

	m_dwReserveAttackTime = 0;

	m_bEnableTeleportDash = false;

	m_dwTeleportEndDelayTime = 0;
}

ioExtendDash* ioTeleportDash::Clone()
{
	return new ioTeleportDash( *this );
}

ioTeleportDash::~ioTeleportDash()
{
}

void ioTeleportDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "teleport_dash_animation", "", szBuf, MAX_PATH );
	m_TeleportDashStartAction = szBuf;

	m_fDashSpeedRate = rkLoader.LoadFloat_e( "teleport_dash_run_max_speed_rate", FLOAT1 );

	LoadAttackAttribute( "teleport_dash_end_action", m_TeleportDashEndAction, rkLoader );

	m_fTeleportOffset = rkLoader.LoadFloat_e( "teleport_offset", FLOAT1 );

	rkLoader.LoadString_e( "teleport_start_effect", "", szBuf, MAX_PATH );
	m_szTeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "teleport_end_effect", "", szBuf, MAX_PATH );
	m_szTeleportEndEffect = szBuf;

	m_fOtherActionJumpPower = rkLoader.LoadFloat_e( "dash_jump_power", FLOAT1 );

	LoadAttackAttribute( "teleport_jump", m_TeleportJumpAttribute, rkLoader );

	m_fTeleportJumpPower = rkLoader.LoadFloat_e( "teleport_jump_power", 0.0f );

	m_dwTeleportEndDelay = (DWORD)rkLoader.LoadInt( "teleport_end_delay", 0 );

	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );
}

DashType ioTeleportDash::GetType() const
{
	return DT_TELEPORT;
}

void ioTeleportDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )	return;

	int iDashState;
	rkPacket >> iDashState;
	DASH_STATE eDashState = (DASH_STATE)iDashState;

	switch( eDashState )
	{
	case DS_TELEPORT_START:
		{
			rkPacket >> m_bEnableTeleportDash;
		}
		break;
	}
}

bool ioTeleportDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( m_TeleportDashStartAction.IsEmpty() ) return false;
	if( m_DashState != DS_NONE ) 
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwTeleportEndDelayTime > dwCurTime )
		return false;

	ioExtendDash::StartDash( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_TeleportDashStartAction );
	if( iAniID == -1 )
		return false;

	m_dwActionEndTime = 0;

	float fTimeRate = pOwner->GetRunDashAniRate();
	fTimeRate *= m_fDashSpeedRate;

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );

	pOwner->SetDashExtraInfo();

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );

	DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwActionEndTime = dwCurTime + dwDuration;

	m_DashState = DS_TELEPORT_START;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)DS_TELEPORT_START;
		kPacket << true;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioTeleportDash::SetEndAction( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_TeleportDashEndAction.m_AttackAnimation.IsEmpty() ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_TeleportDashEndAction.m_AttackAnimation );
	if( iAniID == -1 ) return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_TeleportDashEndAction.m_fAttackAniRate, 0.0f, 0.0f, true );

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * m_TeleportDashEndAction.m_fAttackAniRate;

	CheckKeyReserveTime( pOwner, iAniID, m_TeleportDashEndAction.m_fAttackAniRate );

	m_dwActionEndTime = dwCurTime;
	m_dwActionEndTime += dwDuration;
	m_dwActionEndTime += m_TeleportDashEndAction.m_dwEndDelay;
}

void ioTeleportDash::SetOtherAction( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_TeleportJumpAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = m_TeleportJumpAttribute.m_dwPreDelay;
	float fTimeRate  = m_TeleportJumpAttribute.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->RefreshFireTimeList( iAniID, m_TeleportJumpAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay, true );

	m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	m_dwReserveAttackTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;

	pOwner->SetSKillEndJumpState( m_fTeleportJumpPower, false, false, false );
}

void ioTeleportDash::CheckKeyReserve( ioBaseChar *pOwner )
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

void ioTeleportDash::ProcessDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 피격 예외
	CharState charstate = pOwner->GetState();
	if( pOwner->GetState() != CS_EXTEND_DASH )
	{
		m_dwActionEndTime = 0;
		m_DashState = DS_NONE;
		m_bEnableTeleportDash = false;
	}

	// 예약 처리
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();
	if( dwEndTime > dwCurTime )
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			CheckKeyReserve( pOwner );
		}
	}
	else
	{
		// 대기
		m_dwActionEndTime = 0;
		m_dwReserveEnableTime = 0;

		switch( m_DashState )
		{
		case DS_TELEPORT_START:
			{
				ioPlayStage *pStage = pOwner->GetCreator();
				if ( pStage )
				{
					D3DXVECTOR3 vOffset = pOwner->GetMoveDir() * m_fTeleportOffset;

					m_vTeleportPos = pOwner->GetWorldPosition() + vOffset;
					m_vTargetPos = m_vTeleportPos + vOffset;

					float fMapHeight = pStage->GetMapHeightOnlyTerrain( m_vTeleportPos.x, m_vTeleportPos.z );
					if( pOwner && fMapHeight > 0.0f && fMapHeight > m_vTeleportPos.y )
						m_vTeleportPos.y = fMapHeight;

					D3DXVECTOR3 vPrePos = pOwner->GetWorldPosition();

					pOwner->SetWorldPosition( m_vTeleportPos );
					pOwner->SetTargetRotToTargetPos( m_vTargetPos, true );

					pStage->CreateMapEffect( m_szTeleportStartEffect, vPrePos, ioMath::UNIT_ALL );

					// 차원술사 게이지 / 불렛 감소
					ioDimensionistItem* pDimensionItem = ToDimensionistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
					if( pDimensionItem )
						pDimensionItem->DecreaseBulletsToDoDash();
					m_DashState = DS_TELEPORT_END_ACTION;
				}
			}
			break;
		case DS_TELEPORT_END:
			{
				ProcessKeyReserve( pOwner );
				m_dwActionEndTime = 0;
				m_DashState = DS_NONE;
				pOwner->SetState( CS_DELAY );
				m_bEnableTeleportDash = false;
			}
			break;
		}
		if( m_dwActionEndTime > 0 && m_dwActionEndTime < dwCurTime )
		{
			ProcessKeyReserve( pOwner );
			m_dwActionEndTime = 0;
			if( m_DashState == DS_TELEPORT_END || m_DashState == DS_OTHER_ACTION )
				m_DashState = DS_NONE;
		}
	}

	if( m_DashState == DS_TELEPORT_END_ACTION )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		pStage->CreateMapEffect( m_szTeleportEndEffect, m_vTeleportPos, ioMath::UNIT_ALL );

		ioPlayStage* pPlayStage = pOwner->GetCreator();
		float fMapHeight = pPlayStage->GetMapHeightOnlyTerrain( m_vTeleportPos.x, m_vTeleportPos.z );
		if ( m_vTeleportPos.y - fMapHeight >= m_fJumpHeightGap )
		{
			m_DashState = DS_OTHER_ACTION;
			SetOtherAction( pOwner );
		}
		else
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID = pGrp->GetAnimationIdx( m_TeleportDashEndAction.m_AttackAnimation );
			if ( iAniID == -1 )
			{
				pOwner->SetState( CS_DELAY );
				return;
			}

			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwPreDelay = m_TeleportDashEndAction.m_dwPreDelay;
			float fTimeRate  = m_TeleportDashEndAction.m_fAttackAniRate;

			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

			pOwner->RefreshFireTimeList( iAniID, m_TeleportDashEndAction.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay, true );

			m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
			m_dwReserveAttackTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;

			m_dwTeleportEndDelayTime = dwCurTime + m_dwTeleportEndDelay;

			m_bEnableTeleportDash = false;
			m_DashState = DS_TELEPORT_END;
		}
	}
}

void ioTeleportDash::ExtendDashEnd( ioBaseChar *pOwner )
{
	m_bEnableTeleportDash = false;
	m_DashState = DS_NONE;
}

void ioTeleportDash::CheckRefreshDashAction( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// 종료모션중이면 무시
	if( m_dwActionEndTime > 0 ) return;

	if( m_TeleportDashStartAction.IsEmpty() ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_TeleportDashStartAction );
	if( iAniID == -1 ) return;

	float fTimeRate = pOwner->GetRunDashAniRate();
	fTimeRate *= m_fDashSpeedRate;

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );
}

bool ioTeleportDash::IsEndAction()
{
	if( m_dwActionEndTime > 0 )
		return true;

	return false;
}