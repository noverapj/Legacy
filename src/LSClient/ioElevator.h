#pragma once

#include "ioPlayEntity.h"

class ioWeapon;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioElevator : public ioPlayEntity
{
protected:
	enum ElevatorState
	{
		ES_STOP,
		ES_BOTTOM,
		ES_MOVE_UP,
		ES_TOP,
		ES_MOVE_DOWN,
	};

	int m_iCreateIndex;

	ElevatorState m_State;
	float m_fMoveSpeed;
	DWORD m_dwWaitTime;
	DWORD m_dwFirstWaitTime;

	DWORD m_dwMoveNextStartTime;
	D3DXVECTOR3 m_vStartPos;
	D3DXVECTOR3 m_vEndPos;

protected:
	MountedEntityList m_vMountEntityList;
	MountedWeaponList m_vMountWeaponList;

protected:
	bool m_bEnableFunction;

protected:
	void UpdatePositionLoop( float fTimePerSec, DWORD dwCurTime );
	void MovePosition( float fTimePerSec, DWORD dwCurTime );
	void NotifyStateChanged( ElevatorState ePreState, DWORD dwCurTime );

protected:
	void UpdateMountedEntityList();
	void TranslateMountEntity( const D3DXVECTOR3 &vMoveAmt );

	void UpdateMountedWeaponList();
	void TranslateMountWeapon( const D3DXVECTOR3 &vMoveAmt );

public:
	void EnableWorldFunction( bool bEnable );
	void OnElevatorChanged( SP2Packet &rkPacket );

public:
	void FillCurElevatorInfo( SP2Packet &rkPacket );
	void ApplyCurElevatorInfo( SP2Packet &rkPacket );

public:
	inline int  GetCreateIndex() const { return m_iCreateIndex; }

// for ioGameEntity-----------------------------
public:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );

	virtual bool IsMountAvailable() const;
	virtual bool IsMountingAvailable() const;
	virtual bool IsNowMountEnable() const;

	virtual void Translate( const D3DXVECTOR3 &vMove );

protected:
	virtual void UpdateBottomHeight() const;

//----------------------------------------------

// for ioPlayEntity-----------------------------
public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );

	virtual PlaySubType GetSubType() const;
	virtual ArmorType GetArmorType() const;

public:
	virtual int  DontMoveEntityLevel() const;
	virtual void LoadProperty( ioINILoader &rkLoader );

//----------------------------------------------

public:
	ioElevator( ioEntityGroup *pGrp, ioPlayMode *pMode, int iCreateIndex );
	virtual ~ioElevator();
};

inline ioElevator* ToElevator( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_ELEVATOR )
		return NULL;

	return dynamic_cast< ioElevator* >( pPlay );
}

inline const ioElevator* ToElevatorConst( ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_ELEVATOR )
		return NULL;

	return dynamic_cast< const ioElevator* >( pPlay );
}

typedef std::list< ioElevator* > ElevatorList;

