#pragma once


#include "ioPlayEntity.h"

class ioWeapon;
class ioPlayMode;

class ioConveyer : public ioPlayEntity
{
protected:
	int m_iCreateIndex;

	D3DXVECTOR3 m_vMoveDir;
	float m_fMoveSpeed;

	bool m_bEnableFunction;
	bool m_bNotMovePushStruct;

public:
	inline int  GetCreateIndex() const { return m_iCreateIndex; }

// for ioGameEntity-----------------------------
public:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );

	virtual bool IsMountingAvailable() const;

protected:
	virtual void UpdateBottomHeight() const;

//----------------------------------------------

// for ioPlayEntity-----------------------------
public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;
	virtual bool IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint = NULL ) const;

	virtual PlaySubType GetSubType() const;
	virtual ArmorType GetArmorType() const;

public:
	virtual int  DontMoveEntityLevel() const;
	virtual void LoadProperty( ioINILoader &rkLoader );

//----------------------------------------------

public:
	void EnableWorldFunction( bool bEnable );

public:
	ioConveyer( ioEntityGroup *pGrp, ioPlayMode *pMode, int iCreateIndex );
	virtual ~ioConveyer();
};

inline ioConveyer* ToConveyer( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_CONVEYER )
		return NULL;

	return dynamic_cast< ioConveyer* >( pPlay );
}

inline const ioConveyer* ToConveyerConst( ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_CONVEYER )
		return NULL;

	return dynamic_cast< const ioConveyer* >( pPlay );
}

