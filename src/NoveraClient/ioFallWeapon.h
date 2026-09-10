#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEffect;
class ioPlayStage;

class ioFallWeapon : public ioWeapon
{
public:
	enum FallWeaponState
	{
		FS_FALL,
		FS_STOP,
	};

protected:
	FallWeaponState m_FallWeaponState;

	ioEffect *m_pFallWeapon;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vStartPos;

	float m_fStartHeight;

	DWORD m_dwFirstStopTime;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool CreateEffect( const ioHashString &szFileName );

public:
	float GetMoveSpeed();
	bool IsFalling() const;

protected:
	void SetFalling();
	void SetStoping( float fHeight );

	void OnFalling( float fTimePerSec, ioPlayStage *pStage );
	void OnStoping( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioFallWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioFallWeapon();
};

inline ioFallWeapon* ToFallWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_FALL_WEAPON )
		return NULL;

	return dynamic_cast< ioFallWeapon* >( pWeapon );
}

