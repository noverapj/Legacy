#pragma once

#include "../io3DEngine/ioTimer.h"
#include "ioTargetMarker.h"

class ioPlayStage;

class ioOtherMarker : public ioTargetMarker
{
public:
	virtual MarkerType GetMarkerType() const;
	virtual void SetMarkerState( ioPlayStage *pStage, MarkerState eState );
	virtual void Update( ioPlayStage *pStage );

protected:
	virtual UniqueObjID CreateEffect( ioPlayStage *pStage, const ioHashString &szEffect );

protected:
	void UpdateReadyState( ioPlayStage *pStage );
	void UpdateAimingState( ioPlayStage *pStage );
	void UpdateAimedState( ioPlayStage *pStage );
	void UpdateZeroAimedState( ioPlayStage *pStage );
	void UpdateJumpAimState( ioPlayStage *pStage );
	void UpdateHeadAimedState( ioPlayStage *pStage );

	D3DXVECTOR3 GetFireItemPoint() const;
	D3DXVECTOR3 GetFireItemJumpPoint() const;

public:
	ioOtherMarker( ioBaseChar *pOwner );
	virtual ~ioOtherMarker();
};

inline ioOtherMarker* ToOtherMarker( ioTargetMarker *pMarker )
{
	if( !pMarker || pMarker->GetMarkerType() != ioTargetMarker::MT_OTHER_MARKER )
		return NULL;

	return dynamic_cast< ioOtherMarker* >( pMarker );
}

