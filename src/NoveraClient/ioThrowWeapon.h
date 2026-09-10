#pragma once

#include "ioWeapon.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;

class ioThrowWeapon : public ioWeapon
{
protected:
	ioEntityGroup *m_pFireMesh;

	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vRollAxis;
	ioOrientBox m_LocalAttackBox;

	float	m_fCurMoveRange;
	float	m_fDownSpeed;

	float	m_fMeshHalfHeight;
	float	m_fPingPongPower;
	
	enum ThrowState
	{
		TS_MOVE,
		TS_DOWN,
		TS_PINGPONG,
	};

	ThrowState m_State;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void SetWallCollision( const D3DXVECTOR3 &vFaceNrm );

protected:
	bool SetFireMesh( const ioHashString &szFileName );
	void SetState( ThrowState eState );

public:
	virtual bool InitResource();

	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	void OnMoveState( float fTimePerSec );
	void OnDownState( float fTimePerSec, ioPlayStage *pStage );
	void OnPingPongState( float fTimePerSec, ioPlayStage *pStage );
	
	void RollWeapon( float fTimePerSec );

public:
	ioThrowWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioThrowWeapon();
};

inline ioThrowWeapon* ToThrowWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_THROW )
		return NULL;

	return dynamic_cast< ioThrowWeapon* >( pWeapon );
}

