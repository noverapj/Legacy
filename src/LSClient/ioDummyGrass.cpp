
#include "stdafx.h"
#include "ioDummyGrass.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "BuffDefine.h"

ioDummyGrass::ioDummyGrass( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_bStartInit = false;
	m_State = DCS_START;
}

ioDummyGrass::~ioDummyGrass()
{
}

void ioDummyGrass::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "wound_ani", "", szBuf, MAX_PATH );
	m_szWoundedAni =  szBuf;

	m_fWounedAniRate = rkLoader.LoadFloat_e( "wound_ani_rate", FLOAT1 );
}


void ioDummyGrass::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_LOOP_ATTACK:
		ProcessLoopAttackState( fTimePerSec );
		break;
	case DCS_WOUNDED:
		if( dwCurTime > m_dwMotionEndTime )
			SetDelayState( false );
		break;
	}
}

void ioDummyGrass::ProcessStartState( float fTimePerSec )
{	
	DWORD dwCurTime = FRAMEGETTIME();

	if ( !m_bStartInit )
	{
		D3DXVECTOR3 vPos = GetWorldPosition();
		float fMapHeight = GetBottomHeight();

		vPos.y = fMapHeight + m_fDummyCharHeightOffset;		
		SetWorldPosition( vPos );

		m_bStartInit = true;

		if( m_HP.m_fCurValue > 0.0f && fMapHeight <= 0.0f )
		{
			m_HP.m_fCurValue = 0.0f;
			m_LastAttacker = m_OwnerName;
			SetDieState( true );
		}
	}

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioDummyGrass::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;
	}
}

void ioDummyGrass::ProcessLoopAttackState( float fTimePerSec )
{	
}


//더미 죽는 상태 설정
void ioDummyGrass::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	if ( !m_szExplosionSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szExplosionSound );
	}

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioDummyGrass::IsActivateWeapon()
{
	if( m_State == DCS_START )
		return false;

	return true;
}


int ioDummyGrass::GetDummyGrassType()
{
	return DGT_GRASS1;
}


bool ioDummyGrass::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( pWeapon )
		return true;

	return false;
}


ApplyWeaponExp ioDummyGrass::ApplyWeapon( ioWeapon *pWeapon )
{
	float fPreCurHP = m_HP.m_fCurValue;
	float fDamage = pWeapon->GetDamage( this ) * m_fDamageRate;
	float fPushPower = pWeapon->GetDefensePushPower( fDamage );

	if( !m_bNoDamage && !m_bPiercingProtected )
		m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );

	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = m_HP.m_fMaxValue;
	}

	SetWoundedState();

	return AWE_EXPLOSION;
}


void ioDummyGrass::SetWoundedState()
{
	if( !m_pGroup )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_pGroup->ClearAllActionAni( 0.0f, true );
	m_pGroup->ClearAllLoopAni( 0.0f, true );

	int iAniID = -1;
	float fTimeRate = FLOAT1;	

	iAniID = m_pGroup->GetAnimationIdx( m_szWoundedAni );

	if( iAniID == -1 )
		return;

	m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/m_fWounedAniRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime + m_pGroup->GetAnimationFullTime( iAniID ) * m_fWounedAniRate;

	m_State = DCS_WOUNDED;
}