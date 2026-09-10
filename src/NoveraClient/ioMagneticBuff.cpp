 

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioMagneticBuff.h"


ioMagneticBuff::ioMagneticBuff()
{
	m_Magnet = MAGNET_NONE;
	m_dwCurDelayTime = 0;
}

ioMagneticBuff::ioMagneticBuff( const ioMagneticBuff &rhs )
: ioBuff( rhs ),
m_fMagneticRange( rhs.m_fMagneticRange ),
m_fPushFriction( rhs.m_fPushFriction ),
m_bFrictionLinear( rhs.m_bFrictionLinear ),
m_fPushPower( rhs.m_fPushPower ),
m_dwMagneticRefreshTime( rhs.m_dwMagneticRefreshTime ),
m_PushAnimation( rhs.m_PushAnimation ),
m_fPushAniRate( rhs.m_fPushAniRate ),
m_dwPushDelayTime( rhs.m_dwPushDelayTime ),
m_TargetBuffOnPush( rhs.m_TargetBuffOnPush ),
m_PullAnimation( rhs.m_PullAnimation ),
m_fPullAniRate( rhs.m_fPullAniRate ),
m_dwPullDelayTime( rhs.m_dwPullDelayTime ),
m_TargetBuffOnPull( rhs.m_TargetBuffOnPull ),
m_EffectN( rhs.m_EffectN ),
m_EffectS( rhs.m_EffectS )
{
	m_Magnet = MAGNET_NONE;
	m_dwCurDelayTime = 0;
}

ioMagneticBuff::~ioMagneticBuff()
{
}

void ioMagneticBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fMagneticRange = rkLoader.LoadFloat_e( "magnetic_range", 0.0f );

	// Push
	m_fPushFriction = rkLoader.LoadFloat_e( "push_power_friction", 0.0f );
	m_bFrictionLinear = rkLoader.LoadBool_e( "push_friction_linear", false );
	m_fPushPower = rkLoader.LoadFloat_e( "push_power", 0.0f );

	m_dwMagneticRefreshTime = (DWORD)rkLoader.LoadInt_e( "magnetic_refresh_time", 0 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "push_animation", "", szBuf, MAX_PATH );
	m_PushAnimation = szBuf;
	m_fPushAniRate = rkLoader.LoadFloat_e( "push_animation_rate", FLOAT1 );
	m_dwPushDelayTime = (DWORD)rkLoader.LoadInt_e( "push_delay_time", 0 );
	rkLoader.LoadString_e( "target_buff_on_push", "", szBuf, MAX_PATH );
	m_TargetBuffOnPush = szBuf;

	rkLoader.LoadString_e( "pull_animation", "", szBuf, MAX_PATH );
	m_PullAnimation = szBuf;
	m_fPullAniRate = rkLoader.LoadFloat_e( "pull_animation_rate", FLOAT1 );
	m_dwPullDelayTime = (DWORD)rkLoader.LoadInt_e( "pull_delay_time", 0 );
	rkLoader.LoadString_e( "target_buff_on_pull", "", szBuf, MAX_PATH );
	m_TargetBuffOnPull = szBuf;

	rkLoader.LoadString_e( "buff_effect_n", "", szBuf, MAX_PATH );
	m_EffectN = szBuf;
	rkLoader.LoadString_e( "buff_effect_s", "", szBuf, MAX_PATH );
	m_EffectS = szBuf;
}

ioBuff* ioMagneticBuff::Clone()
{
	return new ioMagneticBuff( *this );
}

void ioMagneticBuff::StartBuff( ioBaseChar *pOwner )
{
	IORandom random;
	DWORD dwSeed = 0;
	dwSeed = pOwner->GetRandomSeed();
	random.SetRandomSeed( dwSeed );
	int iValue = random.Random( MAGNET_S )+1;
	m_Magnet = (Magnet)iValue;

	m_dwLastPushTime = 0;
	m_dwCurDelayTime = 0;

	m_BuffState = BS_ENABLE;

	if( m_Magnet == MAGNET_N )
		m_BuffEffect = m_EffectN;
	else
		m_BuffEffect = m_EffectS;

	m_iEtcStateCnt = pOwner->GetCurEtcStateCnt();

	ioBuff::StartBuff( pOwner );
}

bool ioMagneticBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwLastPushTime = 0;
	m_BuffState = BS_ENABLE;

	m_dwCurDelayTime = 0;

	m_iEtcStateCnt = pOwner->GetCurEtcStateCnt();

	return true;
}

void ioMagneticBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			if( GetOwner() && CheckRestoreStateCheck() )
				GetOwner()->SetState( CS_DELAY, m_bSendDelayNetWork );

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				if( CheckRestoreStateCheck() )
				{
					m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				}

				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if( CheckOwnerState() )
		ProcessBuffState();
}

bool ioMagneticBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

void ioMagneticBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	
	if( pOwner &&
		pOwner->GetCurEtcStateCnt() == m_iEtcStateCnt &&
		pOwner->GetState() == CS_ETC_STATE )
	{
		pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		m_pOwner->SetDashFailStart();
	}

	ioBuff::EndBuff();
}

void ioMagneticBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << (int)m_BuffState;
	rkPacket << (int)m_Magnet;
}

void ioMagneticBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	m_BuffState = (BuffState)iState;
	int iMagnet;
	rkPacket >> iMagnet;
	m_Magnet = (Magnet)iMagnet;
}

bool ioMagneticBuff::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return false;
	}

	if( pOwner->GetCharName() == m_CreateChar )
		return false;

	switch( pOwner->GetState() )
	{
	case CS_USING_SKILL:
	case CS_FROZEN:
	case CS_DRINK:		
		return false;
	}

	if( pOwner->IsSystemState() )
	{
		SetReserveEndBuff();
		return false;
	}

	if( pOwner->HasBuff( BT_ABSOLUTE_PROTECTION ) ||
		pOwner->HasBuff( BT_STATE_PROTECTION ) ||
		pOwner->HasBuff( BT_STATE_PROTECTION_BY_PASSIVE ) ||
		pOwner->HasBuff( BT_NOT_MOVE_PROTECT ) )
	{
		SetReserveEndBuff();
		return false;
	}

	if( pOwner->GetCurEtcStateCnt() != m_iEtcStateCnt )
	{
		SetReserveEndBuff();
		return false;
	}

	if( pOwner->GetProtectionMode() != PM_NONE )
		return false;

	if( pOwner->IsChatModeState( false ) )
	{
		SetReserveEndBuff();
		return false;
	}

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
	{
		SetReserveEndBuff();
		return false;
	}

	if( pOwner->IsCanNoWound() )
	{
		SetReserveEndBuff();
		return false;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
		if( pFootBall && pFootBall->IsDisableAttackMode() )
		{
			SetReserveEndBuff();
			return false;
		}
	}

	return true;
}

void ioMagneticBuff::ProcessBuffState()
{
	if( IsOwnerBuff() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_BuffState == BS_DELAY )
	{
		if( m_dwCurDelayTime > 0 && m_dwLastPushTime + m_dwCurDelayTime < FRAMEGETTIME() )
		{
			m_dwCurDelayTime = 0;
			if( pOwner->GetState() == CS_ETC_STATE )
			{
				pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}
		}

		if( m_dwLastPushTime + m_dwMagneticRefreshTime < FRAMEGETTIME() )
		{
			m_BuffState = BS_ENABLE;
		}
		else
			return;
	}

	ioBaseChar *pSelectTarget = NULL;
	float fLastLength = 10000.0f;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )
			continue;

		if( pTarget->GetCharName() == pOwner->GetCharName() )
			continue;

		if( !pTarget->HasBuff(BT_MAGNETIC) )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
		{
			if( pTarget->IsMagneticBuffOwner() )
				continue;
		}

		D3DXVECTOR3 vPosOwner = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vPosTarget = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDist = vPosTarget - vPosOwner;
		float fLength = D3DXVec3Length( &vDist );
		if( fLength > m_fMagneticRange )
			continue;

		if( fLastLength > fLength )
		{
			fLastLength = fLength;
			pSelectTarget = pTarget;
		}
	}

	if( pSelectTarget )
	{
		D3DXVECTOR3 vPosOwner = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vPosTarget = pSelectTarget->GetMidPositionByRate();

		D3DXVECTOR3 vOwnerDir = vPosTarget - vPosOwner;
		D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );

		D3DXVECTOR3 vTargetDir = vPosOwner - vPosTarget;
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		if( pSelectTarget->GetMagnet() == pOwner->GetMagnet() )
		{
			pOwner->CheckMagneticBuff(vTargetDir, (int)PS_PUSH );
			pOwner->SetTargetRotToTargetPos( vPosTarget, false );
			return;
		}
		else
		{
			pOwner->CheckMagneticBuff(vOwnerDir, (int)PS_PULL );
			pOwner->SetTargetRotToTargetPos( vPosTarget, false );
			return;
		}
	}
}

bool ioMagneticBuff::EnableMagnetic()
{
	if( m_BuffState == BS_ENABLE )
		return true;

	return false;
}

void ioMagneticBuff::SetPush( D3DXVECTOR3 vPushDir, int iState )
{
	if( IsOwnerBuff() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	pOwner->SetForcePower( vPushDir, m_fPushPower, m_fPushFriction, m_bFrictionLinear );
	m_dwLastPushTime = FRAMEGETTIME();
	m_BuffState = BS_DELAY;

	pOwner->SetState(CS_ETC_STATE);
	m_iEtcStateCnt = pOwner->GetCurEtcStateCnt();

	SetMotion( iState );

	if( iState == PS_PUSH )
	{
		//pOwner->AddNewBuff( m_TargetBuffOnPush, m_CreateChar, "", NULL );
		pOwner->ReserveAddNewBuff( m_TargetBuffOnPush, m_CreateChar, "", NULL );
	}
	else
	{
		//pOwner->AddNewBuff( m_TargetBuffOnPull, m_CreateChar, "", NULL );
		pOwner->ReserveAddNewBuff( m_TargetBuffOnPull, m_CreateChar, "", NULL );
	}
}

void ioMagneticBuff::SetMotion( int iState )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = -1;
	float fAniRate = FLOAT1;
	m_dwCurDelayTime = 0;

	if( iState == PS_PUSH )
	{
		iAniID = pGrp->GetAnimationIdx( m_PushAnimation );
		fAniRate = m_fPushAniRate;
		m_dwCurDelayTime = m_dwPushDelayTime;
	}
	else
	{
		iAniID = pGrp->GetAnimationIdx( m_PullAnimation );
		fAniRate = m_fPullAniRate;
		m_dwCurDelayTime = m_dwPullDelayTime;
	}

	if( m_dwCurDelayTime == 0 )
		m_dwCurDelayTime = pGrp->GetAnimationFullTime( iAniID );

	if( !pOwner->IsApplyDownState(false) )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fPushAniRate );
}

bool ioMagneticBuff::IsOwnerBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( pOwner->GetCharName() == m_CreateChar )
		return true;

	return false;
}

int ioMagneticBuff::GetMagnet()
{
	return (int)m_Magnet;
}