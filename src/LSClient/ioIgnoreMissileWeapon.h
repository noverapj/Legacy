#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioIgnoreMissileWeapon : public ioWeapon
{
protected:
	ioEffect *m_pFireBall;

	float	m_fCurMoveSpeed;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;
	ChangeDirInfoList m_ChangeDirInfoList;

	bool	m_bSetGraceClose;
	bool	m_bFirst;
	bool	m_bEnableMapCollision;
	bool	m_bArleadyCallWeapon;
	bool	m_bDurationEndCallWeapon;

	const WeaponAttribute *m_pSecondAttr;
	SecondAttributeList m_vSecondAttrList;

protected:
	WeaponInfo			m_CallWeaponInfo;
	ChangeDirInfoList	m_CallAttFireList;

protected:
	CEncrypt<float> m_fExtraOffsetX;
	CEncrypt<float> m_fExtraOffsetY;
	CEncrypt<float> m_fExtraOffsetZ;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool m_bSetColCnt;
	int m_nColCnt;
	int m_nColMaxCnt;

public:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();

	void SetSecondAttribute( SecondAttributeList vSecondList );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void SetGraceClose();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );
	
public:
	void LoadCallAttribute( ioINILoader &rkLoader );
	void LoadCallAttributeFireDirectionList( ioINILoader &rkLoader );

public:
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetMoveSpeed( float fSpeed );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void CallAttribute();

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioIgnoreMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioIgnoreMissileWeapon();
};

inline ioIgnoreMissileWeapon* ToIgnoreMissileWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_IGNORE_MISSILE )
		return NULL;

	return dynamic_cast< ioIgnoreMissileWeapon* >( pWeapon );
}

