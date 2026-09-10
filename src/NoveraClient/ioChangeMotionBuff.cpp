
#include "stdafx.h"

#include "ioChangeMotionBuff.h"

#include "ioBaseChar.h"



ioChangeMotionBuff::ioChangeMotionBuff()
{
}

ioChangeMotionBuff::ioChangeMotionBuff( const ioChangeMotionBuff &rhs )
: ioBuff( rhs ),
 m_DelayAnimation( rhs.m_DelayAnimation ),
 m_MoveAnimation( rhs.m_MoveAnimation ),
 m_JumpReadyAni( rhs.m_JumpReadyAni ),
 m_JumppingAni( rhs.m_JumppingAni ),
 m_JumpEndAni( rhs.m_JumpEndAni ),
 m_fJumpEndAniRate( rhs.m_fJumpEndAniRate )
{
}

ioChangeMotionBuff::~ioChangeMotionBuff()
{
}

void ioChangeMotionBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

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
}

ioBuff* ioChangeMotionBuff::Clone()
{
	return new ioChangeMotionBuff( *this );
}

void ioChangeMotionBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bOneTimeCheck = false;
	pOwner->SetChangeMotion( true, m_DelayAnimation, m_MoveAnimation );
	pOwner->SetChangeJumpMotion( true, m_JumpReadyAni, m_JumppingAni, m_JumpEndAni, m_fJumpEndAniRate );
}

bool ioChangeMotionBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bOneTimeCheck = false;
	pOwner->SetChangeMotion( true, m_DelayAnimation, m_MoveAnimation );
	pOwner->SetChangeJumpMotion( true, m_JumpReadyAni, m_JumppingAni, m_JumpEndAni, m_fJumpEndAniRate );
	return true;
}

void ioChangeMotionBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;

	if( !m_bOneTimeCheck )
	{
		// 상태변화없이 버프만 걸린 경우를 위해
		if( m_pOwner->GetState() == CS_DELAY )
			m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		else if( m_pOwner->GetState() == CS_RUN )
			m_pOwner->SetState( CS_RUN );

		m_bOneTimeCheck = true;
	}
	
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
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

void ioChangeMotionBuff::EndBuff()
{
	m_pOwner->SetChangeMotion( false, "", "" );
	m_pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	// 종료시 동작 갱신 위해
	if( m_pOwner->GetState() == CS_DELAY )
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	else if( m_pOwner->GetState() == CS_RUN )
		m_pOwner->SetState( CS_RUN );

	ioBuff::EndBuff();
}

