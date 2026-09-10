#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioExplosionWeapon : public ioWeapon
{
protected:
	enum ExplosionState
	{
		ES_DELAY,
		ES_EXPLOSION,
	};

	ExplosionState m_State;

protected:
	ioEffect *m_pFireMesh;
	ioOrientBox m_LocalAttackBox;

	ioHashString m_BoomEffect;

	DWORD m_dwDuration;
	float m_fExplosionRange;
	bool m_bSetForcePos;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetFireMesh( const ioHashString &szFileName );

public:
	void Explosion( ioPlayStage *pStage );
	bool IsExplosion() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	
public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;
	virtual bool IsCanMineExplosion();
	virtual DWORD GetCollisionEnableTime() const;

protected:
	void OnDelayState( float fTimePerSec, ioPlayStage *pStage );

public:
	ioExplosionWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioExplosionWeapon();
};

inline ioExplosionWeapon* ToExplosionWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_EXPLOSION )
		return NULL;

	return dynamic_cast< ioExplosionWeapon* >( pWeapon );
}

