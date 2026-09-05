#pragma once

#include "ioGameEntity.h"

class ioOpcodeShape;
class ioPlayStage;

class ioWorldDecoration : public ioGameEntity
{
protected:
	ioPlayStage* m_pCreator;
	
	bool m_bUseChangeAlpha;
	int m_iChangeAlphaRate;

public:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );
	virtual int GetTypeID() const;

public:
	virtual bool IsMountingAvailable() const;

protected:
	virtual void UpdateBottomHeight() const;
	
public:
	void SetCreator( ioPlayStage *pStage );
	void SetLoopAni( int iAniID );
	void SetRenderGroup( RenderGroupID eGroupID );
	
	void SetChangeAlphaRate( bool bUse, int iRate );

public:
	bool HasOpcodeShape() const;
	ioOpcodeShape* GetOpcodeShape() const;
	UniqueObjID GetUniqueID() const;

public:
	ioWorldDecoration( ioEntityGroup *pGrp );
	virtual ~ioWorldDecoration();
};

inline ioWorldDecoration* ToWorldDecoration( ioGameEntity *pObj )
{
	if( !pObj || pObj->GetTypeID() != ioGameEntity::GET_WORLD_DECO )
		return NULL;

	return dynamic_cast< ioWorldDecoration* >( pObj );
}

