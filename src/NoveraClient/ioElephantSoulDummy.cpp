#include "StdAfx.h"
#include "ioElephantSoulDummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioElephantSoulDummy::ioElephantSoulDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode ), m_nAttackCnt( 0 ), m_DelayState( EDS_NONE ), m_dwLoopEndTime( 0 ), m_dwAreaweaponIndex( 0 )
{
}


ioElephantSoulDummy::~ioElephantSoulDummy(void)
{
}

void ioElephantSoulDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioElephantSoulDummy::LoadInitProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	int iAttackCnt = rkLoader.LoadInt_e( "dummy_attack_cnt", 0 );

	for( int i = 0; i < iAttackCnt; ++i )
	{
		wsprintf_e( szBuf, "dummy_attack%d_code", i+1 );
		int iAttackCode = rkLoader.LoadInt( szBuf, 0 );
		m_vAttackCode.push_back( iAttackCode );
	}
	
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation		= szBuf;
	m_dwLoopTime		= rkLoader.LoadInt( "loop_time", 0 );
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName	= szBuf;
}

void ioElephantSoulDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_ATTACK:
		ProcessAttackState( fTimePerSec, dwPreTime );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );
		
	ProcessEffectUpdate( fTimePerSec );
}

void ioElephantSoulDummy::ProcessStartState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if ( vPos.y < fMapHeight )
	{
		vPos.y = fMapHeight;
		SetWorldPosition( vPos );
	}


	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioElephantSoulDummy::ProcessDelayState( float fTimePerSec )
{

	switch( m_DelayState )
	{
	case EDS_LOOP:
		{
			ProcessDelayLoop( fTimePerSec );
		}
		break;
	default:
		{
			ProcessDelayNone( fTimePerSec );
		}
		break;
	}
}

void ioElephantSoulDummy::ProcessDelayNone( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( COMPARE( m_nAttackCnt, 0, (int)m_vAttackCode.size() ) )
	{		
		if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
		{
			m_dwDelayStartTime		= 0;
			m_iReserveAttackCode	= m_vAttackCode[m_nAttackCnt];

			SetAttackState();
			m_nAttackCnt++;
		}	
	}
	else
	{
		SetDelayLoopState( fTimePerSec );
	}

}

void ioElephantSoulDummy::SetDelayLoopState( float fTimePerSec )
{
	m_DelayState = EDS_LOOP;

	if( !m_LoopAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_LoopAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );
			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_fMoveAniRate );
		}
	}

	m_dwLoopEndTime = FRAMEGETTIME() + m_dwLoopTime;
	if( GetOwner() )
	{
		ioAreaWeapon* pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( GetOwner()->GetCharName(), "", "", m_AreaWeaponName, GetWorldPosition(), ioMath::QUAT_IDENTITY, ioAreaWeapon::CT_NORMAL );
		if( pAreaWeapon )
		{			
			m_dwAreaweaponIndex = pAreaWeapon->GetAreaWeaponIndex();			
		}
	}
}

void ioElephantSoulDummy::ProcessDelayLoop( float fTimePerSec )
{
	if( m_dwLoopEndTime < FRAMEGETTIME() )
	{
		SetDieState( true );
	}
}

void ioElephantSoulDummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( true );

	if( m_dwAreaweaponIndex != 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaweaponIndex );
		m_dwAreaweaponIndex = 0;
	}

	if( m_pModeParent && m_pModeParent->GetCreator() )
		m_pModeParent->GetCreator()->RemoveSummonDummyInfo( m_iCreateIndex );
}