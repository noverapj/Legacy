#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioNormalWeapon : public ioWeapon
{
protected:
	ioEffect *m_pFireBall;

	float	m_fCurMoveSpeed;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;
	ChangeDirInfoList m_ChangeDirInfoList;

	bool	m_bFirst;
	bool m_bDisableOwnerScale;

	CEncrypt<float> m_fExtraOffsetX;
	CEncrypt<float> m_fExtraOffsetY;
	CEncrypt<float> m_fExtraOffsetZ;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();
	float GetMaxRange();

protected:
	bool CreateFireBall( const ioHashString &szFileName );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetLiveTime( DWORD dwTime );

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

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioNormalWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioNormalWeapon();
};

inline ioNormalWeapon* ToNormalWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_NORMAL )
		return NULL;

	return dynamic_cast< ioNormalWeapon* >( pWeapon );
}

