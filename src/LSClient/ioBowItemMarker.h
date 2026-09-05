#pragma once

#include "../io3DEngine/ioTimer.h"
#include "ioTargetMarker.h"

class ioUIRenderImage;

class ioBowItemMarker : public ioTargetMarker
{
public:
	virtual void Update( ioPlayStage *pStage );
	virtual MarkerType GetMarkerType() const;

	virtual void SetMarkerState( ioPlayStage *pStage, MarkerState eState );

protected:
	virtual UniqueObjID CreateEffect( ioPlayStage *pStage, const ioHashString &szEffect );

protected:
	void UpdateReadyState( ioPlayStage *pStage );
	void UpdateAimingState( ioPlayStage *pStage );
	void UpdateAimedState( ioPlayStage *pStage );
	void UpdateZeroAimedState( ioPlayStage *pStage );
	void UpdateJumpAimState( ioPlayStage *pStage );

	D3DXVECTOR3 GetBowItemPoint() const;
	D3DXVECTOR3 GetBowItemJumpPoint() const;

public:
	ioBowItemMarker( ioBaseChar *pOwner );
	virtual ~ioBowItemMarker();
};

inline ioBowItemMarker* ToBowItemMarker( ioTargetMarker *pMarker )
{
	if( !pMarker || pMarker->GetMarkerType() != ioTargetMarker::MT_BOW_ITEM )
		return NULL;

	return dynamic_cast< ioBowItemMarker* >( pMarker );
}

