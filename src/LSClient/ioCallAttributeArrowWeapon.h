#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioCallAttributeArrowWeapon : public ioWeapon
{
protected:
	ioEffect *m_pFireBall;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	bool	m_bSetGraceClose;
	bool	m_bFirst;

	const WeaponAttribute *m_pSecondAttr;
	SecondAttributeList m_vSecondAttrList;
	
	bool			m_bArleadyCallWeapon;
	CEncrypt<bool>	m_bCallWeaponOnEnd;
	WeaponInfoList	m_CallWeaponInfoList;

	bool	m_bCollision;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void RotateMoveDir( float fAngle );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void DestroyFireBall();

	void SetGraseClose();
	void LoadCallAttributeList( ioINILoader &rkLoader );

public:
	void SetSecondAttribute( SecondAttributeList vSecondList );

	float GetMoveSpeed();
	float GetMaxRange();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual void SetWeaponDead();

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );
	void CallAttribute();

public:
	virtual void SetMapCollision();

public:
	ioCallAttributeArrowWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioCallAttributeArrowWeapon();
};

inline ioCallAttributeArrowWeapon* ToCallAttributeArrowWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_CALL_ATTRIBUTE_ARROW )
		return NULL;

	return dynamic_cast< ioCallAttributeArrowWeapon* >( pWeapon );
}

