

#include "stdafx.h"

#include "ioSimpleDash.h"

#include "ioBaseChar.h"


#include "ItemDefine.h"

ioSimpleDash::ioSimpleDash()
{
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

ioSimpleDash::ioSimpleDash( const ioSimpleDash &rhs )
: ioExtendDash( rhs ),
  m_DashAction( rhs.m_DashAction ),
  m_DashEndAction( rhs.m_DashEndAction ),
  m_fDashEndAniRate( rhs.m_fDashEndAniRate ),
  m_fDashSpeedRate( rhs.m_fDashSpeedRate )
{
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

ioExtendDash* ioSimpleDash::Clone()
{
	return new ioSimpleDash( *this );
}

ioSimpleDash::~ioSimpleDash()
{
}

void ioSimpleDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "dash_animation", "", szBuf, MAX_PATH );
	m_DashAction = szBuf;

	m_fDashSpeedRate = rkLoader.LoadFloat_e( "dash_run_max_speed_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_end_animation", "", szBuf, MAX_PATH );
	m_DashEndAction = szBuf;

	m_fDashEndAniRate = rkLoader.LoadFloat_e( "dash_end_ani_rate", FLOAT1 );
}

DashType ioSimpleDash::GetType() const
{
	return DT_SIMPLE;
}

bool ioSimpleDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( m_DashAction.IsEmpty() ) return false;

	ioExtendDash::StartDash( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_DashAction );
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

	return true;
}

void ioSimpleDash::SetEndAction( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_DashEndAction.IsEmpty() ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_DashEndAction );
	if( iAniID == -1 ) return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDashEndAniRate, 0.0f, 0.0f, true );

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * m_fDashEndAniRate;

	CheckKeyReserveTime( pOwner, iAniID, m_fDashEndAniRate );

	m_dwActionEndTime = dwCurTime;
	m_dwActionEndTime += dwDuration;
}

void ioSimpleDash::CheckKeyReserve( ioBaseChar *pOwner )
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

void ioSimpleDash::ProcessDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	bool bEndMotion = false;
	if( m_dwActionEndTime > 0 )
		bEndMotion = true;

	// 점프 체크
	if( !bEndMotion && pOwner->IsJumpKey() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	if( !bEndMotion )
		return;

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
					pOwner->SetNormalAttack( 0 );
				}
			}

			m_dwActionEndTime = 0;
			m_dwReserveEnableTime = 0;
			return;
		}
		
		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			m_iSkillInput = -1;

			m_dwActionEndTime = 0;
			m_dwReserveEnableTime = 0;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			pOwner->SetDefenseState();

			m_dwActionEndTime = 0;
			m_dwReserveEnableTime = 0;
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetJumpState();

			m_dwActionEndTime = 0;
			m_dwReserveEnableTime = 0;
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

			m_dwActionEndTime = 0;
			m_dwReserveEnableTime = 0;
			return;
		}

		// 대기
		m_dwActionEndTime = 0;
		m_dwReserveEnableTime = 0;

		pOwner->SetState( CS_DELAY );
	}
}

void ioSimpleDash::CheckRefreshDashAction( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// 종료모션중이면 무시
	if( m_dwActionEndTime > 0 ) return;

	if( m_DashAction.IsEmpty() ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_DashAction );
	if( iAniID == -1 ) return;

	float fTimeRate = pOwner->GetRunDashAniRate();
	fTimeRate *= m_fDashSpeedRate;

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );
}

bool ioSimpleDash::IsEndAction()
{
	if( m_dwActionEndTime > 0 )
		return true;

	return false;
}

