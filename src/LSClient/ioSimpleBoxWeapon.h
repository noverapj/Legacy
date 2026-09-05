#pragma once


#include "ioWeapon.h"

class ioEntityGroup;
class ioPlayStage;

class ioSimpleBoxWeapon : public ioWeapon
{
protected:

	enum WeaponState
	{
		WS_INIT,
		WS_LIVE,
	};

	ioEffect *m_pMesh;
	WeaponState m_State;

	D3DXVECTOR3 m_vMoveDir;
	
	DWORD m_dwDurationTime;
	DWORD m_dwWeaponDeadTime;

	bool m_bOwnerAttach;
	bool m_bCheckDeadTime;

	bool m_bRotateMoveDir;
	float m_fRotateY;
	float m_fRotateX;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void ProcessAttach( float fTimePerSec, ioPlayStage *pStage );
	void ProcessMove( float fTimePerSec, ioPlayStage *pStage );

protected:
	bool SetMesh( const ioHashString &szFileName );
	
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	void SetPointDir( const D3DXVECTOR3 &vPoint );
	void SetOwnerAttach( bool bAttach );
	void SetCheckDeadTime( bool bCheckDeadTime );

public:
	float GetMoveSpeed();	
	void ClearData();

public:
	ioSimpleBoxWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioSimpleBoxWeapon();
};

inline ioSimpleBoxWeapon* ToSimpleBoxWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_SIMPLE_BOX )
		return NULL;

	return dynamic_cast< ioSimpleBoxWeapon* >( pWeapon );
}

