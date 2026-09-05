

#include "stdafx.h"

#include "FindListPredicateImpl.h"

#include "ioPlayStage.h"


FL_CollisionPlayEntity::FL_CollisionPlayEntity( const ioOrientBox &rkBox, ioPlayEntity *pOwner )
: m_CollisionBox( rkBox )
{
	m_pOwner = pOwner;
}

FL_CollisionPlayEntity::~FL_CollisionPlayEntity()
{
}

bool FL_CollisionPlayEntity::FindList( const GameEntityList &rkList, ioPlayStage *pStage )
{
	GameEntityList::const_iterator iter;
	for( iter=rkList.begin() ; iter!=rkList.end() ; ++iter )
	{
		ioPlayEntity *pEntity = ToPlayEntity( *iter );
		if( pEntity && pEntity != m_pOwner )
		{
			ioOrientBox kColBox = pEntity->GetWorldCollisionBox();
			if( ioMath::TestIntersection( kColBox, m_CollisionBox, NULL ) )
			{
				if( pStage )
				{
					float fMaxHeight = kColBox.GetCenter().y + kColBox.GetExtents(1);
					float fYHeight = m_CollisionBox.GetCenter().y + m_CollisionBox.GetExtents(1) + FLOAT1;
					float fMapHeight = pStage->GetMapHeightByRay( pEntity->GetWorldPosition().x,
																  pEntity->GetWorldPosition().z,
																  pEntity,
																  false,
																  fMaxHeight );

					if( fMapHeight <= fYHeight )
					{
						m_ResultList.push_back( pEntity );
					}
				}
				else
				{
					m_ResultList.push_back( pEntity );
				}

				//m_ResultList.push_back( pEntity );
			}
		}
	}

	return !m_ResultList.empty();
}


