

#ifndef _ioFlameDashWeapon_h_
#define _ioFlameDashWeapon_h_

#include "ioWeapon.h"

class ioEffect;

class ioFlameDashWeapon : public ioWeapon
{
protected:
	ioEffect *m_pFlame;

	D3DXVECTOR3 m_vMoveDir;
	float	m_fCurMoveRange;
	DWORD m_dwFlameStartTime;
	DWORD m_dwFlameLiveTime;

public:
	void RotateMoveDir( float fAngle );
	D3DXVECTOR3 GetPosition();
	void SetFlameLiveTime( DWORD dwLiveTime );

protected:
	void CreateFlame( const ioHashString &szFileName );
	void DestroyFlame();

public:
	virtual void InitResource();
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource();

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual WeaponType GetType() const;

public:
	virtual const ioAxisAlignBox& GetBoundBox() const;
	virtual float GetBoundRadius() const;

public:
	ioFlameDashWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioFlameDashWeapon();
};

inline ioFlameDashWeapon* ToFlameDashWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_FLAME_DASH )
		return NULL;

	return static_cast< ioFlameDashWeapon* >( pWeapon );
}

#endif
