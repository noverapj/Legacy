

#include "stdafx.h"

#include "ioTeleportZone.h"

#include "ioPlayMode.h"
#include "ioPlayStage.h"

#include "ioWorldEventReceiver.h"
#include "ioZoneAffector.h"

#include "FindListPredicateImpl.h"

ioTeleportZone::ioTeleportZone( ioEntityGroup *pGrp, ioPlayMode *pMode, int iCreateIndex )
: ioPlayEntity( pGrp, pMode )
{
	m_iCreateIndex = iCreateIndex;
	m_pExtraAffector = NULL;
}

ioTeleportZone::~ioTeleportZone()
{
	SAFEDELETE( m_pExtraAffector );
}

void ioTeleportZone::LoadProperty( ioINILoader &rkLoader )
{
	int iTargetPos = rkLoader.LoadInt_e( "target_pos_count", 0 );

	m_TargetPosList.clear();
	m_TargetPosList.reserve( iTargetPos );

	char szKey[MAX_PATH];
	for( int i=0 ; i<iTargetPos ; i++ )
	{
		D3DXVECTOR3 vPos;

		wsprintf_e( szKey, "pos%d_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "pos%d_y", i+1 );
		vPos.y = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "pos%d_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szKey, 0.0f );

		m_TargetPosList.push_back( vPos );
	}

	int iExtraAffector = rkLoader.LoadInt_e( "affector_type", 0 );
	m_pExtraAffector = ioZoneAffector::CreateAffector( (ioZoneAffector::ZoneAffectorType)iExtraAffector );
	if( m_pExtraAffector )
	{
		m_pExtraAffector->LoadProperty( rkLoader );
	}
}

void ioTeleportZone::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	if( m_TargetPosList.empty() )
		return;

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage )	return;

	FL_CollisionPlayEntity kFind( GetWorldCollisionBox(), this );
	if( !pStage->FindEntityList( kFind ) )
		return;

	const PlayEntityList &rkEntityList = kFind.GetResultList();

	PlayEntityList::const_iterator iter = rkEntityList.begin();
	for( ; iter!=rkEntityList.end() ; ++iter )
	{
		ioWorldEventReceiver *pReceiver = (*iter)->GetWorldEventReceiver();
		if( !pReceiver ) continue;

		if( !m_pExtraAffector || m_pExtraAffector->GetType() != ioZoneAffector::ZAT_POISON_ZONE )
		{
			D3DXVECTOR3 vTargetPos;
			if( m_TargetPosList.size() == 1 )
			{
				vTargetPos = m_TargetPosList.front();
			}
			else
			{
				int iPosCount = m_TargetPosList.size();
				int iRandIdx = rand() % iPosCount;
				vTargetPos = m_TargetPosList[iRandIdx];
			}

			pReceiver->OnTelepotation( vTargetPos );
		}

		if( m_pExtraAffector )
		{
			m_pExtraAffector->ApplyToReceiver( pReceiver );
		}
	}
}

void ioTeleportZone::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

bool ioTeleportZone::IsMountingAvailable() const
{
	return false;
}

void ioTeleportZone::UpdateBottomHeight() const
{
	m_fCurBottomHeight = GetWorldPosition().y;
}

ApplyWeaponExp ioTeleportZone::ApplyWeapon( ioWeapon *pWeapon )
{
	return AWE_NO_EXPLOSION;
}

bool ioTeleportZone::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return false;
}

bool ioTeleportZone::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return true;
}

bool ioTeleportZone::IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint ) const
{
	return false;
}

ioPlayEntity::PlaySubType ioTeleportZone::GetSubType() const
{
	return PST_TELEPORT_ZONE;
}

ArmorType ioTeleportZone::GetArmorType() const
{
	return AT_UNARMOR;
}

int ioTeleportZone::DontMoveEntityLevel() const
{
	return 6;
}