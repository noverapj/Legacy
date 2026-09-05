#pragma once

#include "../io3DEngine/ioTimer.h"
#include "ioTargetMarker.h"

class ioPlayStage;

class ioHealItemMarker : public ioTargetMarker
{
public:
	virtual MarkerType GetMarkerType() const;
	virtual void Update( ioPlayStage *pStage );

protected:
	virtual UniqueObjID CreateEffect( ioPlayStage *pStage, const ioHashString &szEffect );

public:
	virtual void SetMarkerState( ioPlayStage *pStage, MarkerState eState );

protected:
	void UpdateReadyState( ioPlayStage *pStage );
	void UpdateAimedState( ioPlayStage *pStage );

	D3DXVECTOR3 GetHealItemPoint() const;

public:
	ioHealItemMarker( ioBaseChar *pOwner );
	virtual ~ioHealItemMarker();
};

inline ioHealItemMarker* ToHealItemMarker( ioTargetMarker *pMarker )
{
	if( !pMarker || pMarker->GetMarkerType() != ioTargetMarker::MT_HEAL_ITEM )
		return NULL;

	return dynamic_cast< ioHealItemMarker* >( pMarker );
}

