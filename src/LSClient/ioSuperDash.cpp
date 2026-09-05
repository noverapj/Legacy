
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioSuperDash.h"


#include "ItemDefine.h"

ioSuperDash::ioSuperDash()
{
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

ioSuperDash::ioSuperDash( const ioSuperDash &rhs )
: ioExtendDash( rhs ),
  m_DashAction( rhs.m_DashAction ),
  m_dwActionPreDelay( rhs.m_dwActionPreDelay ),
  m_dwActionDelay( rhs.m_dwActionDelay ),
  m_fActionTimeRate( rhs.m_fActionTimeRate ),
  m_vForceInfoList( rhs.m_vForceInfoList )
{
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

ioExtendDash* ioSuperDash::Clone()
{
	return new ioSuperDash( *this );
}

ioSuperDash::~ioSuperDash()
{
}

void ioSuperDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "dash_action", "", szBuf, MAX_PATH );
	m_DashAction = szBuf;

	m_dwActionPreDelay = rkLoader.LoadInt_e( "dash_action_pre_delay", 0 );
	m_dwActionDelay = rkLoader.LoadInt_e( "dash_action_delay", 0 );
	m_fActionTimeRate = rkLoader.LoadFloat_e( "dash_action_time_rate", FLOAT1 );

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "dash_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "dash_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "dash_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vForceInfoList.push_back( kInfo );
	}
}

DashType ioSuperDash::GetType() const
{
	return DT_SUPER;
}

bool ioSuperDash::StartDash( ioBaseChar *pOwner )
{
	if( m_DashAction.IsEmpty() )
		return false;

	ioExtendDash::StartDash( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_DashAction );
	if( iAniID == -1 )
		return false;

	m_dwActionEndTime = 0;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fActionTimeRate, 0.0f, m_dwActionPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * m_fActionTimeRate;
	dwDuration += m_dwActionPreDelay;

	CheckKeyReserveTime( pOwner, iAniID, m_fActionTimeRate, m_dwActionPreDelay );

	m_dwActionEndTime = dwCurTime;
	m_dwActionEndTime += dwDuration;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetReservedSliding( m_vForceInfoList, iAniID, m_fActionTimeRate, m_dwActionPreDelay );
	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );

	pOwner->CheckCharColSkipTime( iAniID, m_fActionTimeRate, 0 );
	
	g_TutorialMgr.ActionSuperDash();
	return true;
}

void ioSuperDash::CheckKeyReserve( ioBaseChar *pOwner )
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

void ioSuperDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + m_dwActionDelay + pOwner->GetActionStopTotalDelay();

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
		ProcessKeyReserve( pOwner );
		return;
		/*
		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
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
		pOwner->SetState( CS_DELAY );
		*/
	}
}
