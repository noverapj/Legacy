#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioOneTargetWeapon : public ioWeapon
{
protected:
	ioEffect *m_pFireBall;

	float	m_fCurMoveSpeed;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	D3DXVECTOR3 m_vStartOffSet;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;
	ChangeDirInfoList m_ChangeDirInfoList;

	bool	m_bSetGraceClose;
	bool	m_bFirst;
	bool m_bDisableOwnerScale;

	const WeaponAttribute *m_pSecondAttr;
	SecondAttributeList m_vSecondAttrList;

	WeaponInfo	m_CallWeaponInfo;
	ChangeDirInfoList m_CallAttFireList;
	bool m_bArleadyCallWeapon;


	ioHashString m_stTargetName;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();
	float GetMaxRange();

	void SetSecondAttribute( SecondAttributeList vSecondList );

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

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	bool isCallAttribute();
	void CallAttribute( ioPlayEntity *pEntity );

public:
	void SetOneTargetName( const ioHashString& stName );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioOneTargetWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioOneTargetWeapon();
};

inline ioOneTargetWeapon* ToOneTargetWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ONE_TARGET_WEAPON )
		return NULL;

	return dynamic_cast< ioOneTargetWeapon* >( pWeapon );
}

