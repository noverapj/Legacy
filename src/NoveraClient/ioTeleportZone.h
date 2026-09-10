#pragma once


#include "ioPlayEntity.h"

class ioWeapon;
class ioPlayMode;
class ioINILoader;
class ioZoneAffector;

class ioTeleportZone : public ioPlayEntity
{
protected:
	int m_iCreateIndex;
	Vector3Vec m_TargetPosList;

	ioZoneAffector *m_pExtraAffector;

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
	ioTeleportZone( ioEntityGroup *pGrp, ioPlayMode *pMode, int iCreateIndex );
	virtual ~ioTeleportZone();
};

inline ioTeleportZone* ToTeleportZone( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_TELEPORT_ZONE )
		return NULL;

	return dynamic_cast< ioTeleportZone* >( pPlay );
}

inline const ioTeleportZone* ToTeleportZoneConst( ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_TELEPORT_ZONE )
		return NULL;

	return dynamic_cast< const ioTeleportZone* >( pPlay );
}

