#pragma once

#include "FindPredicate.h"

class FL_CollisionPlayEntity : public FindListPredicate
{
protected:
	ioOrientBox m_CollisionBox;
	ioPlayEntity *m_pOwner;

	PlayEntityList m_ResultList;

public:
	virtual bool FindList( const GameEntityList &rkList, ioPlayStage *pStage );

public:
	const PlayEntityList& GetResultList() const { return m_ResultList; }

public:
	FL_CollisionPlayEntity( const ioOrientBox &rkBox, ioPlayEntity *pOwner );
	virtual ~FL_CollisionPlayEntity();
};

