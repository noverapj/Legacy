#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioRotationWeapon : public ioWeapon
{
protected:
	ioEffect *m_pFireBall;

	int m_iCreateItemIndex;
	float m_fRotationTime;

	D3DXQUATERNION m_qtStartRot;
	D3DXVECTOR3 m_vOwnerPos;
	D3DXVECTOR3 m_vDiffPos;
	D3DXVECTOR3 m_vCenterPos;

	bool m_bIndependenceChar;
	bool m_bEnableDownState;
	bool m_bRemoveByWoundState;
	bool m_bRemoveBySkillEnd;

	bool m_bSetGraceClose;
	bool m_bFirst;
	bool m_bReverseRotation;
public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetCreateItem( const ioHashString &rkName );
	virtual void SetWeaponDead();

	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	inline int GetCreateItemIndex() const { return m_iCreateItemIndex; };
	inline bool IsRemoveByWoundState() const { return m_bRemoveByWoundState; }

protected:
	void SetGraceClose();

	bool CreateFireBall( const ioHashString &szFileName );
	bool CheckCreateItem();
	bool CheckOwnerState();
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );


public:
	ioRotationWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioRotationWeapon();
};

inline ioRotationWeapon* ToRotationWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ROTATE )
		return NULL;

	return dynamic_cast< ioRotationWeapon* >( pWeapon );
}

