

#include "stdafx.h"

#include "ioLimitMoveBuff.h"
#include "ioBaseChar.h"

#include "ioGrowthUpInfo.h"

ioLimitMoveBuff::ioLimitMoveBuff()
{
	m_fCurSpeed = 0.0f;
	m_fExtraSpeed = 0.0f;
}

ioLimitMoveBuff::ioLimitMoveBuff( const ioLimitMoveBuff &rhs )
: ioBuff( rhs ),
 m_fAddSpeed( rhs.m_fAddSpeed ),
 m_DelayAnimation( rhs.m_DelayAnimation ),
 m_MoveAnimation( rhs.m_MoveAnimation ),
 m_JumpReadyAni( rhs.m_JumpReadyAni ),
 m_JumppingAni( rhs.m_JumppingAni ),
 m_JumpEndAni( rhs.m_JumpEndAni ),
 m_fJumpEndAniRate( rhs.m_fJumpEndAniRate ),
 m_bDisableDownState( rhs.m_bDisableDownState ),
 m_bSetDontCheckUseBuff( rhs.m_bSetDontCheckUseBuff )
{
	m_fCurSpeed = 0.0f;
	m_fExtraSpeed = 0.0f;
}

ioLimitMoveBuff::~ioLimitMoveBuff()
{
}

void ioLimitMoveBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fAddSpeed = rkLoader.LoadFloat_e( "speed_add_value", 0.0f );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );
	m_DelayAnimation = szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_MoveAnimation = szBuf;

	rkLoader.LoadString_e( "jump_ready_animation", "", szBuf, MAX_PATH );
	m_JumpReadyAni = szBuf;
	rkLoader.LoadString_e( "jump_animation", "", szBuf, MAX_PATH );
	m_JumppingAni = szBuf;
	rkLoader.LoadString_e( "jump_end_animation", "", szBuf, MAX_PATH );
	m_JumpEndAni = szBuf;

	m_fJumpEndAniRate = rkLoader.LoadFloat_e( "jump_end_ani_rate", 0.0f );

	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );

	m_bSetDontCheckUseBuff = rkLoader.LoadBool_e( "dont_check_use_buff", false );
}

ioBuff* ioLimitMoveBuff::Clone()
{
	return new ioLimitMoveBuff( *this );
}

void ioLimitMoveBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( !m_ItemName.IsEmpty() )
	{
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoForItem(GT_SPEED_BUFF_UP));
		if( pUpInfo )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			m_fExtraSpeed = pUpInfo->GetValue(pStage);
		}
	}

	m_fCurSpeed = m_fAddSpeed + m_fExtraSpeed;
	pOwner->SetExtraMoveSpeed( m_fCurSpeed );
	pOwner->SetChangeMotion( true, m_DelayAnimation, m_MoveAnimation );
	pOwner->SetChangeJumpMotion( true, m_JumpReadyAni, m_JumppingAni, m_JumpEndAni, m_fJumpEndAniRate );
}

bool ioLimitMoveBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;

	if( m_iOperationType != OT_SWITCH )
	{
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
		m_dwBuffStartTime = m_dwBuffEndTime - GetBuffDuration();
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != m_pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	m_fCurSpeed = m_fAddSpeed + m_fExtraSpeed;
	pOwner->SetExtraMoveSpeed( m_fCurSpeed );
	pOwner->SetChangeMotion( true, m_DelayAnimation, m_MoveAnimation );
	pOwner->SetChangeJumpMotion( true, m_JumpReadyAni, m_JumppingAni, m_JumpEndAni, m_fJumpEndAniRate );

	if( m_pOwner->GetState() == CS_DELAY )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
	else if( m_pOwner->GetState() == CS_RUN )
	{
		m_pOwner->SetState( CS_RUN );
	}

	return true;
}

void ioLimitMoveBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;
	
	CheckOwnerStateCheck();

	if( !m_pOwner->IsCanLimitMove(m_Name) && !m_bSetDontCheckUseBuff )
	{
		m_pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	float fCurTicGauge = GetCurTicGauge();
	int iTickGap = GetCurTicGap();
	int iCurTicTime = GetCurTicTime();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

			if( fCurTicGauge != 0.0f )
			{
				if( !IsRightOwnerItem() )
				{
					SetReserveEndBuff();
					return;
				}

				if( m_pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, fCurTicGauge ) <= 0.0f )
				{
					// 점프중일때는 끝나지 않게...
					if( m_pOwner->GetState() != CS_JUMP || m_pOwner->GetJumpState() != JS_JUMPPING )
					{
						SetReserveEndBuff();
						return;
					}
				}
			}
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			// 점프중일때는 끝나지 않게...
			if( m_pOwner->GetState() != CS_JUMP || m_pOwner->GetJumpState() != JS_JUMPPING )
			{
				if( m_dwBuffEndTime <= FRAMEGETTIME() )
				{
					SetReserveEndBuff();
					return;
				}
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioLimitMoveBuff::EndBuff()
{
	m_pOwner->RemoveExtraMoveSpeed( m_fCurSpeed );
	m_pOwner->SetChangeMotion( false, "", "" );
	m_pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	if( m_pOwner->GetState() == CS_DELAY )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
	else if( m_pOwner->GetState() == CS_RUN )
	{
		m_pOwner->SetState( CS_RUN );
	}

	ioBuff::EndBuff();
}

void ioLimitMoveBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_fExtraSpeed;
}

void ioLimitMoveBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fExtraSpeed;
}

void ioLimitMoveBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{	
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}
}

bool ioLimitMoveBuff::IsCheckUseBuff() const
{
	return !m_bSetDontCheckUseBuff;
}