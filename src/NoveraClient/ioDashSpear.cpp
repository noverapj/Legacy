

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioDashSpear.h"

ioDashSpear::ioDashSpear()
{
	m_dwPreAniEndTime = 0;
	m_dwRunAniEndTime = 0;
	m_dwEndAniEndTime = 0;
}

ioDashSpear::ioDashSpear( const ioDashSpear &rhs )
: ioExtendDash( rhs ),
  m_Attribute( rhs.m_Attribute ),
  m_DashPreAnimation( rhs.m_DashPreAnimation ),
  m_DashAnimation( rhs.m_DashAnimation ),
  m_DashEndAnimation( rhs.m_DashEndAnimation ),
  m_fDashAniRate( rhs.m_fDashAniRate ),
  m_fDashPreAniRate( rhs.m_fDashPreAniRate ),
  m_fDashEndRate( rhs.m_fDashEndRate ),
  m_fSpeed( rhs.m_fSpeed ),
  m_dwDuration( rhs.m_dwDuration )
{
	m_dwPreAniEndTime = 0;
	m_dwRunAniEndTime = 0;
	m_dwEndAniEndTime = 0;
}

ioExtendDash* ioDashSpear::Clone()
{
	return new ioDashSpear( *this );
}

ioDashSpear::~ioDashSpear()
{
}

void ioDashSpear::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "dash_start", "", szBuf, MAX_PATH );
	m_DashPreAnimation = szBuf;
	rkLoader.LoadString_e( "dash_run", "", szBuf, MAX_PATH );
	m_DashAnimation = szBuf;
	rkLoader.LoadString_e( "dash_end", "", szBuf, MAX_PATH );
	m_DashEndAnimation = szBuf;

	m_fDashPreAniRate = rkLoader.LoadFloat_e( "dash_start_ani_rate", FLOAT1 );
	m_fDashAniRate = rkLoader.LoadFloat_e( "dash_run_ani_rate", FLOAT1 );
	m_fDashEndRate = rkLoader.LoadFloat_e( "dash_end_ani_rate", FLOAT1 );

	m_fSpeed = rkLoader.LoadFloat_e( "run_speed", 600.0f );
	m_dwDuration = rkLoader.LoadInt_e( "run_duration", 700 );

	m_Attribute.m_AttackAnimation = m_DashPreAnimation;
	m_Attribute.m_fAttackAniRate = m_fDashPreAniRate;

	char szKey[MAX_PATH];
	int iAttackTypeCnt = rkLoader.LoadInt_e( "dash_spear_type_count", 0 );
	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "dash_spear_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "dash_spear_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "dash_spear_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "dash_spear_type%d_wounded_time", i+1 );
        kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
		
		wsprintf_e( szKey, "dash_spear_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_Attribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

DashType ioDashSpear::GetType() const
{
	return DT_SPEAR;
}

bool ioDashSpear::StartDash( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_DashPreAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDashSpear::StartDash - %s Animation is Not Exist",
								m_DashPreAnimation.c_str() );
		return false;
	}

	if( pOwner->IsOwnerChar() && pOwner->GetCurItemSkillGauge() < m_fDashExtraGauge )
		return false;
	
	DWORD dwStartTime = FRAMEGETTIME();
	DWORD dwDurationTime = pGrp->GetAnimationFullTime( iAniID ) * m_fDashPreAniRate;
	m_dwPreAniEndTime = dwStartTime + dwDurationTime;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
							     m_Attribute.m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 m_fDashPreAniRate,
								 0 );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetState( CS_EXTEND_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDashPreAniRate, 0.0f, 0.0f, true );
	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );

	g_TutorialMgr.ActionDashSpear();
	return true;
}

void ioDashSpear::ProcessDash( ioBaseChar *pOwner )
{
	if( pOwner->GetState() != CS_EXTEND_DASH )
	{
		pOwner->ClearAttackFireTimeAndSkill();
		pOwner->ClearReservedSliding();

		pOwner->SetState( CS_DELAY );
	}

	if( m_dwPreAniEndTime == 0 )
	{
		if( m_dwRunAniEndTime < FRAMEGETTIME() )
		{
			if( !m_bRunEnd )
			{
				m_bRunEnd = true;

				ioEntityGroup *pGrp = pOwner->GetGroup();
				int iAniID = pGrp->GetAnimationIdx( m_DashEndAnimation );
				if( iAniID == -1 )
				{
					LOG.PrintTimeAndLog( 0, "ioDashSpear::ProcessDash()-EndAni is Not Exist." );
					m_dwEndAniEndTime = 0;
					return;
				}

				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDashEndRate, 0.0f, 0.0f, true );

				m_dwEndAniEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * m_fDashEndRate;

				pOwner->ClearAttackFireTimeAndSkill();
				pOwner->ClearReservedSliding();
			}
			else
			{
				if( m_dwEndAniEndTime < FRAMEGETTIME() )
				{
					pOwner->ClearAttackFireTimeAndSkill();
					pOwner->ClearReservedSliding();
					pOwner->SetState( CS_DELAY );
					return;
				}
			}
		}
		else
		{
			pOwner->SetCurMoveSpeed( m_fSpeed );
		}
		return;
	}

	if( m_dwPreAniEndTime < FRAMEGETTIME() )
	{
		m_dwPreAniEndTime = 0;
		SetDashAni( pOwner );
	}
}

void ioDashSpear::SetDashAni( ioBaseChar *pOwner )
{
	m_bRunEnd = false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_DashAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDashSpear::SetDashAni()-RunAni is Not Exist." );
		m_dwRunAniEndTime = 0;
		return;
	}

	float fTimeRate = pOwner->GetRunDashAniRate() * m_fDashAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );

	m_dwRunAniEndTime = FRAMEGETTIME() + m_dwDuration;
}
