
#include "stdafx.h"

#include "ioWallZone.h"

#include "ioPlayMode.h"
#include "ioPlayStage.h"

#include "FindListPredicateImpl.h"

ioWallZone::ioWallZone( ioEntityGroup *pGrp, ioPlayMode *pMode, int iCreateIndex ) : ioPlayEntity( pGrp, pMode )
{
	m_iCreateIndex = iCreateIndex;
}

ioWallZone::~ioWallZone()
{
}

void ioWallZone::LoadProperty( ioINILoader &rkLoader )
{

}

void ioWallZone::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
/*	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage )	return;

	FL_CollisionPlayEntity kFind( GetWorldCollisionBox(), this );
	if( !pStage->FindEntityList( kFind ) )
		return;

	const PlayEntityList &rkEntityList = kFind.GetResultList();
*/
}

void ioWallZone::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

bool ioWallZone::IsMountAvailable() const
{
	return false;
}

bool ioWallZone::IsMountingAvailable() const
{
	return false;
}

bool ioWallZone::IsNowMountEnable() const
{
	return true;
}

void ioWallZone::UpdateBottomHeight() const
{
	m_fCurBottomHeight = GetWorldPosition().y;
}

ApplyWeaponExp ioWallZone::ApplyWeapon( ioWeapon *pWeapon )
{
	return AWE_NO_EXPLOSION;
}

bool ioWallZone::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon ) return false;

	if( !IsCollisionAvailableRange( pWeapon ) )
		return false;

	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	return true;
}

bool ioWallZone::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return false;
}
	
bool ioWallZone::IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint ) const
{
	return ioPlayEntity::IsWoundedCollision( rkAttackBox, pColPoint );
}

ioPlayEntity::PlaySubType ioWallZone::GetSubType() const
{
	return PST_WALL_ZONE;
}

ArmorType ioWallZone::GetArmorType() const
{
	return AT_UNARMOR;
}

int ioWallZone::DontMoveEntityLevel() const
{
	return 6;
}