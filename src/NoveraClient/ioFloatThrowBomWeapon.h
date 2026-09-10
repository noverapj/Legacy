#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioBaseChar;

class ioFloatThrowBomWeapon : public ioWeapon
{
	enum BombState
	{
		BS_FLOATING,
		BS_EXPLOSION,
		BS_EXPLOSION_END,
	};

	BombState m_BombState;

protected:
	ioEffect *m_pThrowMesh;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vRollAxis;

	float m_fMoveSpeed;
	float m_fFloatingPower;
	float m_fSpeedWeightRate;
	float m_fGravityRate;

	D3DXVECTOR3 m_vStartOffSet;

	ioOrientBox m_LocalAttackBox;

	bool			m_bArleadyCallWeapon;
	CEncrypt<bool>	m_bCallWeaponOnEnd;
	WeaponInfoList	m_CallWeaponInfoList;

	bool m_bCharCollision;
	bool m_bCheckCharCollision;


	bool m_bExplosionUse;
	float m_fExplosionRange;
	ioHashString m_BoomEffect;

	bool m_bUseFireStartPos;

	D3DXVECTOR3 m_vPrevDir;

	D3DXVECTOR3 m_vPrevPos;

	bool m_bSetBaseAttackDir;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void RotateMoveDir( float fAngle );
	float GetMoveSpeed();

	void SetGravityRate( float fGravityRate );

protected:
	bool SetThrowMesh( const ioHashString &szFileName );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetMoveSpeed( float fSpeed );
	virtual void SetFloatPower( float fFloatPower );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual void SetOwnerChargeFloatRate( float fRate );
	virtual void SetWeaponDead();

protected:
	void LoadCallProperty( ioINILoader &rkLoader );	
	void CallAttribute();

	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

	void Explosion( ioPlayStage *pStage );

public:
	ioFloatThrowBomWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioFloatThrowBomWeapon();
};

inline ioFloatThrowBomWeapon* ToFloatThrowBomWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_FLOAT_THROW_BOM_WEAPON )
		return NULL;

	return dynamic_cast< ioFloatThrowBomWeapon* >( pWeapon );
}

