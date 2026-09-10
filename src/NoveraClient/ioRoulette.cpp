
#include "stdafx.h"

#include "ioRoulette.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioRoulette::ioRoulette( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioPlayEntity( pGrp, pMode )
{
	m_bReserveDestroy = false;

	m_vRollAxis = ioMath::UNIT_Z;
	m_ArmorType = AT_UNARMOR;
	m_dwProcessTime = FRAMEGETTIME();
}

ioRoulette::~ioRoulette()
{
}

void ioRoulette::InitRouletteInfo( ioPlayStage *pStage, int iType, int iSubIndex, D3DXVECTOR3 vPos, D3DXVECTOR3 vScale )
{
	m_bReserveDestroy = false;
	m_vRollAxis = ioMath::UNIT_Z;

	m_pPlayStage = pStage;

	m_iRouletteType = iType;
	m_iSubIndex = iSubIndex;
	SetWorldPosition( vPos );
	SetWorldScale( vScale );
}

void ioRoulette::LoadProperty( ioINILoader &rkLoader )
{
}

void ioRoulette::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ProcessState( fTimePerSec );
}

void ioRoulette::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

void ioRoulette::ProcessState( float fTimePerSec )
{
	// Process시간 갱신
	// m_dwProcessTime = FRAMEGETTIME();
}

ApplyWeaponExp ioRoulette::ApplyWeapon( ioWeapon *pWeapon )
{
	return AWE_NO_EXPLOSION;
}

bool ioRoulette::IsNeedCheckMapCollision() const
{
	return false;
}

bool ioRoulette::IsMountAvailable() const
{
	return false;
}

bool ioRoulette::IsMountingAvailable() const
{
	return false;
}

bool ioRoulette::IsNowMountEnable() const
{
	return false;
}

ioWorldEventReceiver* ioRoulette::GetWorldEventReceiver()
{
	return dynamic_cast<ioWorldEventReceiver*>(this);
}

bool ioRoulette::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return false;
}

int ioRoulette::DontMoveEntityLevel() const
{
	return 0;
}

ioPlayEntity::PlaySubType ioRoulette::GetSubType() const
{
	return PST_ROULETTE;
}

ArmorType ioRoulette::GetArmorType() const
{
	return m_ArmorType;
}

void ioRoulette::OnTelepotation( const D3DXVECTOR3 &vTargetPos )
{
	return;
}

void ioRoulette::OnFireZoneDamage( float fDamage, const ioHashString &szBuff )
{
	return;
}

void ioRoulette::OnPoisonZoneDamage()
{
	return;
}

void ioRoulette::UpdateBottomHeight() const
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	m_fCurBottomHeight = m_pModeParent->GetMapHeight( vCurPos.x, vCurPos.z, this );
}

ioEffect* ioRoulette::AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner, bool bDependence/* =true  */ )
{
	if( !rkEffect.IsEmpty() )
	{
		return m_pGroup->AttachEffect( rkEffect, pEntityEventOwner, ioMath::QUAT_IDENTITY, ioMath::VEC3_ZERO, bDependence );
	}

	return NULL;
}

void ioRoulette::EndEffect( UniqueObjID eObjID, bool bGrace /* = true */ )
{
	if( eObjID == -1 ) return;

	m_pGroup->EndEffect( eObjID, bGrace );
}

bool ioRoulette::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return true;
}
