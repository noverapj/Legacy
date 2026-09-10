

#include "stdafx.h"

#include "ioGrowthUpInfo.h"

#include "ioBaseChar.h"
#include "ioBreakFallBuff.h"

ioBreakFallBuff::ioBreakFallBuff()
{
	m_bBreakFallState = false;
	m_dwBreakFallLandingProtectEndTime = 0;
}

ioBreakFallBuff::ioBreakFallBuff( const ioBreakFallBuff &rhs )
: ioBuff( rhs ),
m_BreakFallInfo( rhs.m_BreakFallInfo ),
m_fEnableHeightMin( rhs.m_fEnableHeightMin ),
m_fEnableHeightMax( rhs.m_fEnableHeightMax ),
m_BreakFallLandAni( rhs.m_BreakFallLandAni ),
m_fBreakFallLandAniRate( rhs.m_fBreakFallLandAniRate ),
m_fJumppingPowerRate( rhs.m_fJumppingPowerRate ),
m_bInitialGauge( rhs.m_bInitialGauge ),
m_fBuffNeedGauge( rhs.m_fBuffNeedGauge ),
m_bBreakFallProtect( rhs.m_bBreakFallProtect )
{
	m_bBreakFallState = false;
	m_dwBreakFallLandingProtectEndTime = 0;
}

ioBreakFallBuff::~ioBreakFallBuff()
{
}

void ioBreakFallBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_BreakFallInfo.m_BreakFallType = (BreakFallType)rkLoader.LoadInt_e( "break_fall_type", BFT_NORMAL );

	wsprintf_e( szBuf, "break_fall_attack" );
	LoadAttackAttribute( szBuf, m_BreakFallInfo.m_BreakFallAttack, rkLoader );

	m_fEnableHeightMin = rkLoader.LoadFloat_e( "enable_height_min", 0.0f );
	m_fEnableHeightMax = rkLoader.LoadFloat_e( "enable_height_max", 0.0f );

	m_fJumppingPowerRate = rkLoader.LoadFloat_e( "jumpping_power_rate", FLOAT1 );
	m_bInitialGauge = rkLoader.LoadBool_e( "initial_gauge", false );
	m_fBuffNeedGauge = rkLoader.LoadFloat_e( "buff_need_gauge", 0.0f );

	rkLoader.LoadString_e( "break_fall_land_ani", "", szBuf, MAX_PATH );
	m_BreakFallLandAni = szBuf;
	m_fBreakFallLandAniRate = rkLoader.LoadFloat_e( "break_fall_ani_land_rate", FLOAT1 );

	m_bBreakFallProtect = rkLoader.LoadBool_e( "break_fall_protect", false );
}

ioBuff* ioBreakFallBuff::Clone()
{
	return new ioBreakFallBuff( *this );
}

void ioBreakFallBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioBreakFallBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGlobalTime ) )
		return false;

	return true;
}

void ioBreakFallBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		if( m_pOwner->GetState() == CS_JUMP )
			CheckBreakFallLanding();
		else if( m_pOwner->GetState() == CS_BLOW_WOUNDED )
			CheckBreakFall();
		else if( m_pOwner->GetState() == CS_BOUND_BLOW_WOUNDED )
			CheckBreakFall();
		else if( m_pOwner->GetState() == CS_BLOW_EXTEND_WOUND )
			CheckBreakFall();
		else if( m_pOwner->IsDownState() )
			m_bBreakFallState = false;

		CheckCallingAreaWeapon();
		break;
	}
}

void ioBreakFallBuff::SetBreakFallState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioEntityGroup* pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioItem *pOwnerItem = NULL;
	DWORD dwCurTime = FRAMEGETTIME();
	m_bBreakFallState = false;

	switch( m_BreakFallInfo.m_BreakFallType )
	{
	case BFT_NORMAL:
	case BFT_ATTACK:
		pOwnerItem = pOwner->GetEquipedItem( m_ItemName );
		if( pOwnerItem )
		{
			//int iSlot = m_pOwnerItem->GetType() - 1;
			int iSlot = pOwnerItem->GetType() - 1;
			ioSkill *pSkill = m_pOwner->GetEquipedSkill(iSlot);
			ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);
			/*if( !pPassiveSkill )
				return;*/

			if( GetSkillName().IsEmpty() )
				return;

			/*if( pPassiveSkill->GetName() != GetSkillName() )
				return;*/

			if( pPassiveSkill && pPassiveSkill->UsePassiveGauge() )
			{
				float fCurGauge = pPassiveSkill->GetCurPassiveGauge();
				if( m_fBuffNeedGauge <= fCurGauge )
				{
					pOwner->SetBreakFallState( GetName() );

					fCurGauge -= m_fBuffNeedGauge;
					pPassiveSkill->SetCurPassiveGauge( fCurGauge );

					m_bBreakFallState = true;
				}
			}
			else if( pOwnerItem->HasSkillGauge() && m_fBuffNeedGauge > 0.0f )
			{
				float fCurGauge = pOwnerItem->GetCurSkillGuage();
				if( fCurGauge >= m_fBuffNeedGauge )
				{
					pOwner->SetBreakFallState( GetName() );

					fCurGauge -= m_fBuffNeedGauge;
					pOwnerItem->SetCurSkillGauge( fCurGauge );

					m_bBreakFallState = true;
				}
			}
			else
			{
				pOwner->SetBreakFallState( GetName() );

				m_bBreakFallState = true;
			}
		}
		break;
	case BFT_JUMP:
		pOwnerItem = pOwner->GetEquipedItem( m_ItemName );
		//if( m_pOwnerItem )
		if( pOwnerItem )
		{
			//int iSlot = m_pOwnerItem->GetType() - 1;
			int iSlot = pOwnerItem->GetType() - 1;
			ioSkill *pSkill = m_pOwner->GetEquipedSkill(iSlot);
			ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);
			/*if( !pPassiveSkill )
				return;*/

			if( GetSkillName().IsEmpty() )
				return;

			/*if( pPassiveSkill->GetName() != GetSkillName() )
				return;*/

			if( pPassiveSkill && pPassiveSkill->UsePassiveGauge() )
			{
				float fCurGauge = pPassiveSkill->GetCurPassiveGauge();
				if( m_fBuffNeedGauge <= fCurGauge )
				{
					pOwner->SetChangeJumppingState( m_fJumppingPowerRate, false, 0 );

					fCurGauge -= m_fBuffNeedGauge;
					pPassiveSkill->SetCurPassiveGauge( fCurGauge );
				}
			}
			else if( pOwnerItem->HasSkillGauge() && m_fBuffNeedGauge > 0.0f )
			{
				float fCurGauge = pOwnerItem->GetCurSkillGuage();
				if( fCurGauge >= m_fBuffNeedGauge )
				{
					pOwner->SetBreakFallState( GetName() );

					fCurGauge -= m_fBuffNeedGauge;
					pOwnerItem->SetCurSkillGauge( fCurGauge );

					m_bBreakFallState = true;
				}
			}
		}
		break;
	}
}

void ioBreakFallBuff::SetBreakFallLanding()
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = m_pOwner->GetGroup();
	if( !pGrp ) return;

	// HARDCODE: GetJumpEndAnimationIdx() 이후GetJumpEndAnimationRate() 순서지킬것.
	//           Index 체크할때 AniRate도 설정되기 때문
	int iJumpEndIdx = m_pOwner->GetJumpEndAnimationIdx();
	float fAniRate = m_pOwner->GetJumpEndAnimationRate();

	if( !m_BreakFallLandAni.IsEmpty() )
	{
		iJumpEndIdx = pGrp->GetAnimationIdx( m_BreakFallLandAni );
		fAniRate = m_fBreakFallLandAniRate;
	}

	if( iJumpEndIdx == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioBreakFallBuff::SetBreakFallLanding() - Landing Animation File Error!" );
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( 50.0f, true );

	float fCurRate = fAniRate;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoForItem(GT_BREAK_FALL_LAND));
	if( pUpInfo )
	{
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		float fValue = pUpInfo->GetValue(pStage);
		fValue = FLOAT1 - fValue/FLOAT100;

		if( fValue <= 0.0f )
			fValue = FLOAT1;

		fCurRate *= fValue;
	}

	pGrp->SetActionAni( iJumpEndIdx, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate, 0.0f, 0.0f, true );

	DWORD dwJumpStateEndTime = dwCurTime;
	dwJumpStateEndTime += pGrp->GetAnimationFullTime( iJumpEndIdx ) * fCurRate;

	m_pOwner->SetBreakFallLanding( dwJumpStateEndTime );

	int iAniID = pGrp->GetAnimationIdx( m_BreakFallInfo.m_BreakFallAttack.m_AttackAnimation );
	if( iJumpEndIdx != -1 && iAniID != -1 )
	{
		m_dwBreakFallLandingProtectEndTime = dwCurTime + ( pGrp->GetAnimationEventTime_e( iJumpEndIdx, "firemotion_e" ) * fAniRate );
		m_bBreakFallState = false;
	}
}

void ioBreakFallBuff::CheckBreakFall()
{
	if( !m_pOwner->IsNeedProcess() ) return;
	if( m_pOwner->IsPrisonerMode() ) return;
	if( m_pOwner->IsCatchMode() ) return;
	if( m_pOwner->HasDisableSkillFlameBuff() ) return;
	if( !m_pOwner->IsDownState() ) return;

	if( (m_BreakFallInfo.m_BreakFallType != BFT_JUMP) &&
		m_BreakFallInfo.m_BreakFallAttack.m_AttackAnimation.IsEmpty() )
		return;

	float fHeightGap = 0.0f;
	if( m_pOwner->CheckGhostState() )
		fHeightGap = m_pOwner->GetWorldPosition().y - m_pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = m_pOwner->GetWorldPosition().y - m_pOwner->GetBottomHeight();
	
	if( fHeightGap <= 0 )
		return;

	float fMaxHeight = m_fEnableHeightMax;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoForItem(GT_BREAK_FALL_HEIGHT));
	if( pUpInfo )
	{
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		float fValue = pUpInfo->GetValue(pStage);
		fMaxHeight += fValue;
	}

	if( !COMPARE( fHeightGap, m_fEnableHeightMin, fMaxHeight ) )
		return;

	switch( m_BreakFallInfo.m_BreakFallType )
	{
	case BFT_NORMAL:
		if( m_pOwner->IsJumpKeyDown() || m_pOwner->IsDefenseKeyDown() )
			SetBreakFallState();
		break;
	case BFT_JUMP:
		if( m_pOwner->IsJumpKeyDown() )
		{
			SetBreakFallState();
		}
		break;
	case BFT_ATTACK:
		if( m_pOwner->IsAttackKeyDown() )
			SetBreakFallState();
		break;
	}
}

void ioBreakFallBuff::CheckBreakFallLanding()
{
	if( m_pOwner->GetJumpState() != JS_BREAK_FALL ) return;
	if( !m_pOwner->IsDownState() ) return;

	float fHeightGap = 0.0f;
	if( m_pOwner->CheckGhostState() )
		fHeightGap = m_pOwner->GetWorldPosition().y - m_pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = m_pOwner->GetWorldPosition().y - m_pOwner->GetBottomHeight();

	if( fHeightGap > 0.0f ) return;

	SetBreakFallLanding();
}

void ioBreakFallBuff::EndBuff()
{
	ioBuff::EndBuff();
}

BreakFallInfo ioBreakFallBuff::GetBreakFallInfo() const
{
	return m_BreakFallInfo;
}

bool ioBreakFallBuff::IsProtected() const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bBreakFallProtect && ( m_bBreakFallState || m_dwBreakFallLandingProtectEndTime >= dwCurTime ) )
		return true;

	return false;
}