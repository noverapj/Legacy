#pragma once


#include "ioWeapon.h"

class ioEffect;
class ioMapEffect;
class ioPlayStage;

class ioNuclearWeapon : public ioWeapon
{
protected:
	ioEffect *m_pNuclear;

	UniqueObjID m_SensorID;

	ioOrientBox m_LocalAttackBox;

	ioHashString m_RedSensor;
	ioHashString m_BlueSensor;
	ioHashString m_BoomEffect;

	float	m_fExplosionRange;
	float	m_fStartHeight;

	D3DXVECTOR3 m_vStartPos;

	enum NuclearState
	{
		NS_FALL,
		NS_EXPLOSION
	};
	NuclearState m_NuclearState;

	bool m_bEffectOnlyTeam;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	float GetMoveSpeed();

	bool IsFalling() const;
	void Explosion( ioPlayStage *pStage );

protected:
	bool CreateNuclear( const ioHashString &szFileName );
	void DestroyNuclear();

	void OnFalling( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual DWORD GetCollisionEnableTime() const;

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioNuclearWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioNuclearWeapon();
};

inline ioNuclearWeapon* ToNuclearWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_NUCLEAR )
		return NULL;

	return dynamic_cast< ioNuclearWeapon* >( pWeapon );
}

