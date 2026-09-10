#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioCannonBallWeapon : public ioWeapon
{
protected:
	ioEffect *m_pBombMesh;
	ioHashString m_BoomEffect;

	D3DXVECTOR3 m_vMoveDir;
	ioOrientBox m_LocalAttackBox;

	DWORD m_dwExplosionTime;
	float m_fExplosionRange;
	float m_fFloatingPower;
	
	float m_fMoveSpeed;

	float m_fXOffSet;
	float m_fYOffSet;

protected:
	enum BombState
	{
		BS_FLOATING,
		BS_EXPLOSION
	};
	BombState m_BombState;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetBombMesh( const ioHashString &szFileName );

public:
	void RotateMoveDir( float fAngle );
	
	float GetMoveSpeed() const;

	bool IsFloating() const;
	bool IsExplosion() const;

	void Explosion( ioPlayStage *pStage );
	
protected:
	void OnFloating( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual DWORD GetCollisionEnableTime() const;

	virtual void SetOwnerChargeFloatRate( float fRate );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCanMineExplosion();

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioCannonBallWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioCannonBallWeapon();
};

inline ioCannonBallWeapon* ToCannonBallWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_CANNON_BALL )
		return NULL;

	return dynamic_cast< ioCannonBallWeapon* >( pWeapon );
}

