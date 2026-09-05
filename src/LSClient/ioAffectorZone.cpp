

#include "stdafx.h"

#include "ioAffectorZone.h"

#include "ioPlayMode.h"
#include "ioPlayStage.h"

#include "ioZoneAffector.h"

#include "FindListPredicateImpl.h"

ioAffectorZone::ioAffectorZone( ioEntityGroup *pGrp, ioPlayMode *pMode, int iCreateIndex )
: ioPlayEntity( pGrp, pMode )
{
	m_pAffector = NULL;
	m_iCreateIndex = iCreateIndex;
}

ioAffectorZone::~ioAffectorZone()
{
	SAFEDELETE( m_pAffector );
}

void ioAffectorZone::LoadProperty( ioINILoader &rkLoader )
{
	int iType = rkLoader.LoadInt_e( "affector_type", 0 );

	m_pAffector = ioZoneAffector::CreateAffector( (ioZoneAffector::ZoneAffectorType)iType );
	if( m_pAffector )
	{
		m_pAffector->LoadProperty( rkLoader );
	}
}

void ioAffectorZone::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	if( !m_pAffector )	return;

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage )	return;

	FL_CollisionPlayEntity kFind( GetWorldCollisionBox(), this );
	if( !pStage->FindEntityList( kFind ) )
		return;

	const PlayEntityList &rkEntityList = kFind.GetResultList();

	// 이전에 적용받은 Entity와 현재 Entity간의 체크
	AppliedEntitySet::iterator iSet = m_AppliedEntitySet.begin();
	while( iSet != m_AppliedEntitySet.end() )
	{
		const ioPlayEntity *pApplied = *iSet;

		PlayEntityList::const_iterator iFind = std::find( rkEntityList.begin(),
														  rkEntityList.end(),
														  pApplied );

		if( iFind == rkEntityList.end() )
		{
			iSet = m_AppliedEntitySet.erase( iSet );
		}
		else
		{
			++iSet;
		}
	}

	// 효과 적용
	PlayEntityList::const_iterator iEntity = rkEntityList.begin();
	for( ; iEntity!=rkEntityList.end() ; ++iEntity )
	{
		ApplyAffector( *iEntity );
	}
}

void ioAffectorZone::ApplyAffector( ioPlayEntity *pEntity )
{
	if( !m_pAffector || !pEntity )
		return;

	// 이미 적용된적이 있나?
	if( m_AppliedEntitySet.find( pEntity ) == m_AppliedEntitySet.end() )
	{
		ioWorldEventReceiver *pReceiver = pEntity->GetWorldEventReceiver();
		if( pReceiver )
		{
			m_pAffector->ApplyToReceiver( pReceiver );
			m_AppliedEntitySet.insert( pEntity );
		}
	}
}

void ioAffectorZone::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

bool ioAffectorZone::IsMountingAvailable() const
{
	return false;
}

void ioAffectorZone::UpdateBottomHeight() const
{
	m_fCurBottomHeight = GetWorldPosition().y;
}

ApplyWeaponExp ioAffectorZone::ApplyWeapon( ioWeapon *pWeapon )
{
	return AWE_NO_EXPLOSION;
}

bool ioAffectorZone::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return false;
}

bool ioAffectorZone::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return true;
}

bool ioAffectorZone::IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint ) const
{
	return false;
}

ioPlayEntity::PlaySubType ioAffectorZone::GetSubType() const
{
	return PST_AFFECTOR_ZONE;
}

ArmorType ioAffectorZone::GetArmorType() const
{
	return AT_UNARMOR;
}

int ioAffectorZone::DontMoveEntityLevel() const
{
	return 6;
}