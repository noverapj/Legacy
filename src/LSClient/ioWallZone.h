#pragma once


#include "ioPlayEntity.h"

class ioWeapon;
class ioPlayMode;
class ioINILoader;
class ioWallZone : public ioPlayEntity
{
protected:
	int m_iCreateIndex;

public:
	inline int GetCreateIndex() const { return m_iCreateIndex; }

// for ioGameEntity-----------------------------
public:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );

	virtual bool IsMountAvailable() const;		// 자신의 위에 다른 오브젝트를 Mount 할수 있는 객체인가?
	virtual bool IsMountingAvailable() const;
	virtual bool IsNowMountEnable() const;

protected:
	virtual void UpdateBottomHeight() const;

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
	ioWallZone( ioEntityGroup *pGrp, ioPlayMode *pMode, int iCreateIndex );
	virtual ~ioWallZone();
};

inline ioWallZone* ToWallZone( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_WALL_ZONE )
		return NULL;

	return dynamic_cast< ioWallZone* >( pPlay );
}

inline const ioWallZone* ToWallZoneConst( const ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_WALL_ZONE )
		return NULL;

	return dynamic_cast< const ioWallZone* >( pPlay );
}

