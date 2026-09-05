#pragma once

#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEffect;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;

class ioThrowReturnWeapon : public ioWeapon
{
protected:
	const WeaponAttribute *m_pSecondAttr;

	ioEffect *m_pThrowMesh;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vRollAxis;

	DWORD m_dwReturnDelay;
	DWORD m_dwStartReturnTime;

	float m_fCurMoveRange;
	float m_fCurMoveDistance;
	ioOrientBox m_LocalAttackBox;

	enum ThrowReturnState
	{
		TRS_NONE,
		TRS_RETRUN,
	};
	ThrowReturnState m_ReturnState;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void RotateMoveDir( float fAngle );
	float GetMoveSpeed();

	void SetReturnWay();
	void SetSecondAttribute( int iAttributeIdx );

public:
	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );

protected:
	bool SetThrowMesh( const ioHashString &szFileName );
	bool CheckMaxRange();

	virtual const WeaponAttribute* GetAttribute() const;

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

public:
	ioThrowReturnWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioThrowReturnWeapon();
};

inline ioThrowReturnWeapon* ToThrowReturnWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_THROW_RETURN )
		return NULL;

	return dynamic_cast< ioThrowReturnWeapon* >( pWeapon );
}

