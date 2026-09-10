#pragma once

#include "../io3DEngine/ioTimer.h"
#include "ioTargetMarker.h"

class ioPlayStage;

class ioCrossBowMarker : public ioTargetMarker
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

	D3DXVECTOR3 GetFireItemPoint() const;
	D3DXVECTOR3 GetFireItemJumpPoint() const;

public:
	ioCrossBowMarker( ioBaseChar *pOwner );
	virtual ~ioCrossBowMarker();
};

inline ioCrossBowMarker* ToCrossBowMarker( ioTargetMarker *pMarker )
{
	if( !pMarker || pMarker->GetMarkerType() != ioTargetMarker::MT_CROSS_BOW )
		return NULL;

	return dynamic_cast< ioCrossBowMarker* >( pMarker );
}

