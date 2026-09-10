

#include "stdafx.h"
#include "ioExtendDash.h"

#include "ioBaseChar.h"

#include "ItemDefine.h"

ioExtendDash::ioExtendDash()
{
	m_dwReserveEnableTime = 0;
	m_dwReserveEnableEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_bReloadCheck = false;
}

ioExtendDash::ioExtendDash( const ioExtendDash &rhs )
: m_fDashExtraGauge( rhs.m_fDashExtraGauge ),
  m_dwDashExtraTic( rhs.m_dwDashExtraTic ),
  m_iReloadCnt( rhs.m_iReloadCnt )
{
	m_dwReserveEnableTime = 0;
	m_dwReserveEnableEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_bReloadCheck = false;
}

ioExtendDash::~ioExtendDash()
{
}

void ioExtendDash::LoadProperty( ioINILoader &rkLoader )
{
	m_fDashExtraGauge = rkLoader.LoadFloat_e( "dash_extra_gauge", 0.0f );
	m_dwDashExtraTic = rkLoader.LoadInt_e( "dash_extra_tic", 0 );

	m_iReloadCnt = rkLoader.LoadInt_e( "reload_bullet_cnt", 0 );
}

void ioExtendDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
}

bool ioExtendDash::StartDash( ioBaseChar *pOwner )
{
	m_dwReserveEnableTime = 0;
	m_dwReserveEnableEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_bReloadCheck = false;

	return true;
}

void ioExtendDash::ProcessDash( ioBaseChar *pOwner )
{
}

bool ioExtendDash::IsEnableExtraDashAttack()
{
	return true;
}

bool ioExtendDash::IsEnableExtraOtherDashAttack()
{
	return true;
}

bool ioExtendDash::IsEnableDashAttackEndDash()
{
	return true;
}

void ioExtendDash::ExtendDashEnd( ioBaseChar *pOwner )
{
}

bool ioExtendDash::GetAutoTargetValue( IN float &fDashRange, IN float &fDashMinAngle, IN float &fDashMaxAngle,
									   OUT float &fRange, OUT float &fMinAngle, OUT float &fMaxAngle )
{
	return false;
}

bool ioExtendDash::IsEnableDefense()
{
	return false;
}

float ioExtendDash::CheckKeyReserveTime( ioBaseChar *pOwner, int iAniID, float fTimeRate, DWORD dwPredelay )
{
	if( !pOwner )
		return 0.0f;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return 0.0f;

	DWORD dwCurTime = FRAMEGETTIME();

	float fEventTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	float fEventEndTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve_end" ) * fTimeRate;	
	if( fEventTime > 0 )
		m_dwReserveEnableTime = dwCurTime + fEventTime + dwPredelay;

	if( fEventEndTime > 0 )
		m_dwReserveEnableEndTime = dwCurTime + fEventEndTime + dwPredelay;

	return fEventTime;
}

void ioExtendDash::CheckKeyReserve( ioBaseChar *pOwner )
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

void ioExtendDash::ProcessKeyReserve( ioBaseChar *pOwner )
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

		return;
	}
		
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
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();

	pOwner->SetState( CS_DELAY );
}

void ioExtendDash::CheckReload( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	if( !pItem ) return;

	if( !ToBowItem(pItem) && !ToFireWeaponItem(pItem) && !ToAutoShotItem(pItem) )
		return;

	int iMaxBullet = pItem->GetMaxBullet();
	if( iMaxBullet <= 0 ) return;

	int iNewBullet = pItem->GetCurBullet() + m_iReloadCnt;
	pItem->SetCurBullet( iNewBullet );

}

bool ioExtendDash::IsEnableDashFromDashAttack( ioBaseChar *pOwner )
{
	return false;
}


void ioExtendDash::SetNextDashState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{

}