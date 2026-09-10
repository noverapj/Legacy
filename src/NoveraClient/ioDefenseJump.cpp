

#include "stdafx.h"

#include "ioDefenseJump.h"
#include "ioBaseChar.h"

ioDefenseJump::ioDefenseJump()
{
	m_bUsedDefense = false;
}

ioDefenseJump::ioDefenseJump( const ioDefenseJump &rhs )
: ioExtendJump( rhs ),
 m_fDefenseEnableHeight( rhs.m_fDefenseEnableHeight ),
 m_JumpDefenseAnimation( rhs.m_JumpDefenseAnimation ),
 m_JumpAniDefenseWounded( rhs.m_JumpAniDefenseWounded),
 m_JumpAniDefenseCriticalWounded( rhs.m_JumpAniDefenseCriticalWounded ),
 m_fDefenseAniRate( rhs.m_fDefenseAniRate ),
 m_dwDefenseDuration( rhs.m_dwDefenseDuration ),
 m_DefenseLandAni( rhs.m_DefenseLandAni ),
 m_fDefenseLandAniRate( rhs.m_fDefenseLandAniRate )
{	
	m_bUsedDefense = false;
}

ioExtendJump* ioDefenseJump::Clone()
{
	return new ioDefenseJump( *this );
}

ioDefenseJump::~ioDefenseJump()
{
}

JumpType ioDefenseJump::GetType() const
{
	return JT_DEFENSE_ATTACK;
}

void ioDefenseJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];
	
	m_fDefenseEnableHeight = rkLoader.LoadFloat_e( "jump_defense_enable_hight", 0.0f );
	
	rkLoader.LoadString_e( "jump_defense_animation", "", szBuf, MAX_PATH );
	m_JumpDefenseAnimation = szBuf;
	rkLoader.LoadString_e( "jump_ani_defense_wounded", "", szBuf, MAX_PATH );
	m_JumpAniDefenseWounded = szBuf;
	rkLoader.LoadString_e( "jump_ani_defense_criticalwounded", "", szBuf, MAX_PATH );
	m_JumpAniDefenseCriticalWounded = szBuf;

	rkLoader.LoadString_e( "jump_defense_land_animation", "", szBuf, MAX_PATH );
	m_DefenseLandAni = szBuf;
	m_fDefenseLandAniRate = rkLoader.LoadFloat_e( "jump_defense_land_ani_rate", FLOAT1 );

	m_fDefenseAniRate = rkLoader.LoadFloat_e( "jump_defense_ani_rate", FLOAT1 );
	m_dwDefenseDuration = rkLoader.LoadInt_e( "jump_defense_duration", 0 );
}

void ioDefenseJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bUsedDefense = false;

	m_dwDefenseEnableTime = 0;
	m_dwDefenseReleaseTime = 0;
}

void ioDefenseJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( CheckDefenseRelease( pOwner ) )
	{
		ReleaseDefense( pOwner );
		return;
	}

	if( IsCanDefense( pOwner, fHeightGap ) && !pOwner->IsBuffLimitDefense() )
	{
		SetDefense( pOwner );
		return;
	}
}

bool ioDefenseJump::CheckDefenseRelease( ioBaseChar *pOwner )
{
	if( !m_bUsedDefense || pOwner->IsDefenseKeyDown() )
		return false;

	return true;
}

bool ioDefenseJump::IsCanDefense( ioBaseChar *pOwner, float fHeightGap )
{
	if( pOwner->IsPrisonerMode() || pOwner->GetUsedJumpAttack() ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		!pOwner->IsDefenseKeyDown() || m_bUsedDefense )
	{
		return false;
	}

	if( pOwner->IsJumpKeyDown() || pOwner->IsAttackKeyDown() )
		return false;

	if( fHeightGap < m_fDefenseEnableHeight )
		return false;

	if( m_dwDefenseReleaseTime > 0 && m_dwDefenseReleaseTime > FRAMEGETTIME() )
		return false;

	return true;
}

void ioDefenseJump::SetDefense( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_JumpDefenseAnimation );
	float fAniRate = m_fDefenseAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );

	DWORD dwKeyPressTime = FRAMEGETTIME();
	m_dwDefenseEnableTime  = dwKeyPressTime +
							 pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwDefenseReleaseTime = m_dwDefenseEnableTime + m_dwDefenseDuration;

	m_bUsedDefense = true;
	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpDefense( dwKeyPressTime, m_dwDefenseEnableTime, m_dwDefenseReleaseTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << true;				// defense
		kPacket << dwKeyPressTime;
		kPacket << m_dwDefenseEnableTime;
		kPacket << m_dwDefenseReleaseTime;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDefenseJump::ReleaseDefense( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwReleaseTime = dwCurTime + m_dwDefenseDuration;

	m_bUsedDefense = false;

	if( m_dwDefenseEnableTime < dwCurTime )
	{
		m_dwDefenseReleaseTime = dwReleaseTime;
		pOwner->ReleaseJumpDefense( dwReleaseTime );
	}
	else
	{
		pOwner->ReleaseJumpDefense( m_dwDefenseReleaseTime );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << false;				// defense
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDefenseJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	bool bDefense;
	rkPacket >> bDefense;

	if( bDefense )
	{
		SetDefense( pOwner );
	}
	else
	{
		ReleaseDefense( pOwner );
	}
}

ioHashString ioDefenseJump::GetJumpAniDefenseWounded()
{
	return m_JumpAniDefenseWounded;
}

ioHashString ioDefenseJump::GetJumpAniDefenseCriticalWounded()
{
	return m_JumpAniDefenseCriticalWounded;
}

bool ioDefenseJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	if( m_bUsedDefense )
	{
		if( pOwner && pOwner->IsAttackKeyDown() && !pOwner->IsJumpKeyDown() )
			return true;

		return false;
	}

	return true;
}

