#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioMissileWeapon6 : public ioWeapon
{
protected:
	ioEffect *m_pFireBall;

	float	m_fCurMoveSpeed;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	D3DXVECTOR3 m_vStartOffSet;
	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;

	bool m_bSetGraceClose;
	bool m_bFirst;
	bool m_bDisableOwnerScale;

	WeaponInfo	m_CallWeaponInfo;
	ChangeDirInfoList m_CallAttFireList;
	bool m_bArleadyCallWeapon;

	bool m_bSetMoveDirFunc;
	bool m_bSetForceStartPos;

	bool m_bCreateDummy;
	ioHashString m_szDummyName;
	float m_fDummyRange;

	bool m_bPassLinkSkeleton;

	ioHashString m_szWeaponEffect_Blue;
	ioHashString m_szWeaponEffect_Red;

	std::vector<D3DXVECTOR3> m_MoveInfo;
	int m_iMoveIndex;
	float m_fFinalMaxRange;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	float GetMoveSpeed();
	float GetMaxRange();

	bool ChangeWeaponEffect( const ioHashString &szEffectName );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void SetGraceClose();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetMoveSpeed( float fSpeed );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetWeaponDead();

public:
	void LoadCallAttribute( ioINILoader &rkLoader );
	void LoadCallAttributeFireDirectionList( ioINILoader &rkLoader );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	bool isCallAttribute();
	void CallAttribute( ioPlayEntity *pEntity );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );
	bool CheckNextMoveDir();

public:
	void SetMoveInfo( std::vector<D3DXVECTOR3>& cInfo );

protected:
	void CreateDummy();

public:
	ioMissileWeapon6( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioMissileWeapon6();
};

inline ioMissileWeapon6* ToMissileWeapon6( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_MISSILE6 )
		return NULL;

	return dynamic_cast< ioMissileWeapon6* >( pWeapon );
}