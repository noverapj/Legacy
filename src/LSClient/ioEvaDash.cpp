#include "stdafx.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioEvaDash.h"

ioEvaDash::ioEvaDash()
{
	m_DashState					= CDS_NONE;
	m_dwActionEndTime			= 0;
	m_dwReserveEnableTime		= 0;		// 에어리어웨폰 범위 안에서는 대시 시 텔레포트 사용 가능
	m_iSkillInput				= -1;
	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;
	m_vTeleportPos				= ioMath::VEC3_ZERO;
	m_vTargetPos				= ioMath::VEC3_ZERO;
	m_dwReserveAttackTime		= 0;
	m_dwTeleportEndDelayTime	= 0;
	m_bEnableTeleportDash		= false;
}

ioEvaDash::ioEvaDash( const ioEvaDash &rhs )
: ioExtendDash( rhs ),
m_AttackAttribute( rhs.m_AttackAttribute ),

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
m_dwTeleportEndDelay( rhs.m_dwTeleportEndDelay ),
m_bTeleport( rhs.m_bTeleport),
m_szBuffName(rhs.m_szBuffName)
{
	m_DashState					= CDS_NONE;
	m_dwActionEndTime			= 0;
	m_dwReserveEnableTime		= 0;		// 에어리어웨폰 범위 안에서는 대시 시 텔레포트 사용 가능
	m_iSkillInput				= -1;
	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;
	m_vTeleportPos				= ioMath::VEC3_ZERO;
	m_vTargetPos				= ioMath::VEC3_ZERO;
	m_dwReserveAttackTime		= 0;
	m_dwTeleportEndDelayTime	= 0;
	m_bEnableTeleportDash		= false;
}

ioExtendDash* ioEvaDash::Clone()
{
	return new ioEvaDash( *this );
}

ioEvaDash::~ioEvaDash()
{
}

DashType ioEvaDash::GetType() const
{
	return DT_EVA;
}

void ioEvaDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// 기본
	LoadAttackAttribute_e( "dash_action_attack", m_AttackAttribute, rkLoader );

	// 에어리어웨폰 범위 안에서는 대시 시 텔레포트 사용 가능
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

	// 텔레포트 범위 체크
	m_bTeleport = rkLoader.LoadBool_e( "enable_charge_area_dash", false );
	rkLoader.LoadString_e( "charge_area_name", "", szBuf, MAX_PATH );
	m_szBuffName = szBuf;

}

bool ioEvaDash::IsCanEvaTeleportDash( ioBaseChar *pOwner )
{
	// 에어리어웨폰 범위 안에서는 대시 시 텔레포트 사용
	if( m_bTeleport )
	{
		ioAreaWeapon* pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponInRangeByName( m_szBuffName, pOwner->GetCharName(), pOwner->GetWorldPosition() );
		if( pAreaWeapon )
		{
			return true;
		}
	}

	return false;
}

bool ioEvaDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return false;	}

	// 이미 대시 중인지 체크 2020-11-20
	if( m_DashState != CDS_NONE	)
	{
		return false;
	}

	// 에어리어웨폰 범위 안에서는 대시 시 텔레포트 사용
	if( IsCanEvaTeleportDash( pOwner ) )
	{
		TeleportStartDash( pOwner );
		return true;
	}

	// 2020-11-18
	ioExtendDash::StartDash( pOwner );
	
	// 기본 대시 
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioEvaDash::StartDash - %s Animation is Not Exist",
								m_AttackAttribute.m_AttackAnimation.c_str() );
		return false;
	}

	float fTimeRate = m_AttackAttribute.m_fAttackAniRate;

	m_dwActionEndTime = 0;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
							     m_AttackAttribute.m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 m_AttackAttribute.m_dwPreDelay );

	pOwner->InitExtendAttackTagInfo();

	if( pOwner->IsNeedProcess() )
		pOwner->SetDashAttackEndJumpInfo( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)m_AttackAttribute.m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = m_AttackAttribute.m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_AttackAttribute.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_AttackAttribute.m_vForceInfoList, iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );
	pOwner->SetInputCancelInfo( m_AttackAttribute.m_AttackAnimation, m_AttackAttribute.m_fAttackAniRate );

	DWORD dwTrackingTime = FRAMEGETTIME() + m_AttackAttribute.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_iSkillInput			= -1;
	m_bEnableDefenseState	= false;
	m_bEnableAttackState	= false;
	m_bEnableJumpState		= false;
	pOwner->ClearDirDoubleClick();

	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );

	g_TutorialMgr.ActionAssaultDash();
	return true;
}

void ioEvaDash::TeleportStartDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_TeleportDashStartAction.IsEmpty() ) return;
	if( m_DashState != CDS_NONE ) 
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwTeleportEndDelayTime > dwCurTime )
	{
		return;
	}

	ioExtendDash::StartDash( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_TeleportDashStartAction );
	if( iAniID == -1 )
	{
		return;
	}

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

	m_DashState = CDS_TELEPORT_START;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)CDS_TELEPORT_START;
		kPacket << true;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return;
}

void ioEvaDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();
	if( dwEndTime > dwCurTime )
	{
		// 에어리어웨폰 범위 안에서는 대시 시 텔레포트 사용 가능
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			CheckKeyReserve( pOwner );
		}
	}
	else
	{
		// 대기
		m_dwActionEndTime		= 0;
		m_dwReserveEnableTime	= 0;
		switch( m_DashState )
		{
			case CDS_TELEPORT_START:
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
					m_DashState = CDS_TELEPORT_END_ACTION;
				}

				break;
			}

			case CDS_TELEPORT_END:
			{
				ProcessKeyReserve( pOwner );
				m_dwActionEndTime = 0;
				m_DashState = CDS_NONE;
				pOwner->SetState( CS_DELAY );
				m_bEnableTeleportDash = false;
				break;
			}

			case CDS_NONE:
			{
				ProcessKeyReserve( pOwner );
				break;
			}
		}

		if( m_dwActionEndTime > 0 && m_dwActionEndTime < dwCurTime )
		{
			ProcessKeyReserve( pOwner );
			m_dwActionEndTime = 0;
			if( m_DashState == CDS_TELEPORT_END || m_DashState == CDS_OTHER_ACTION )
			{
				m_DashState = CDS_NONE;
			}
		}
	}

	if( m_DashState == CDS_TELEPORT_END_ACTION )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		pStage->CreateMapEffect( m_szTeleportEndEffect, m_vTeleportPos, ioMath::UNIT_ALL );

		ioPlayStage* pPlayStage = pOwner->GetCreator();
		float fMapHeight = pPlayStage->GetMapHeightOnlyTerrain( m_vTeleportPos.x, m_vTeleportPos.z );
		if ( m_vTeleportPos.y - fMapHeight >= m_fJumpHeightGap )
		{
			m_DashState = CDS_OTHER_ACTION;
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
			m_DashState = CDS_TELEPORT_END;
		}
	}
}

void ioEvaDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) {	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) {	return;	}

	DWORD dwDelayTime	= 0;
	DWORD dwCurTime		= FRAMEGETTIME();

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
		case CDS_NONE:
		{
			break;
		}
		case CDS_TELEPORT_START:
		{
			rkPacket >> m_bEnableTeleportDash;
			TeleportStartDash( pOwner );
		}
		break;
	}
}

void ioEvaDash::CheckKeyReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;		
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
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

void ioEvaDash::SetOtherAction( ioBaseChar *pOwner )
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

void ioEvaDash::ExtendDashEnd( ioBaseChar *pOwner )
{
	m_bEnableTeleportDash	= false;
	m_DashState				= CDS_NONE;

	// 2020-11-18
	m_dwActionEndTime		= 0;
	m_dwReserveEnableTime	= 0;
	m_iSkillInput			= -1;
	m_bEnableDefenseState	= false;
	m_bEnableAttackState	= false;
	m_bEnableJumpState		= false;
	pOwner->ClearDirDoubleClick();
}