#pragma once

#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioRotateReturnWeapon : public ioWeapon
{
public:
	enum WeaponState
	{
		WS_INIT,
		WS_MOVE,
		WS_RETURN,
		WS_END,
	};

	enum WeaponSyncState
	{
		WSS_RETURN,
	};

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	ioEffect*		m_pFireBall;
	ioOrientBox		m_LocalAttackBox;

protected:
	WeaponState		m_State;
	
	DWORD			m_dwLiveTime;

	float			m_fMoveSpeed;

	float			m_fMaxMoveRange;
	float			m_fMaxEndRange;
	float			m_fMaxEndRangeOffset;

	float			m_fReturnMovSpeed;
	float			m_fReturnRotSpeed;
	float			m_fCurMoveRange;
	
	D3DXVECTOR3		m_vStartPos;
	D3DXVECTOR3		m_vMoveDir;

	bool			m_bDisableCollisionReturn;
	DWORD			m_dwCollisionSync;

protected:
	ioHashString	m_FireSound;

protected:
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual WeaponType GetType() const;
	
protected:
	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage = NULL );
	bool CreateFireBall( const ioHashString &szFileName );

protected:	
	virtual void SetOwnerChargeScaleRate( float fRate );
	float GetMoveSpeed();

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	virtual const ioOrientBox& GetLocalAttackBox() const;

public:
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );

protected:
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	void ProcessMove( ioPlayStage *pStage, ioBaseChar* pOwner, float fTimePerSec );
	void ProcessRotate( ioPlayStage *pStage, ioBaseChar* pOwner, float fTimePerSec );

protected:
	void OnMoveState( ioPlayStage *pStage, ioBaseChar* pOwner, float fTimePerSec );
	void OnReturnState( ioPlayStage *pStage, ioBaseChar* pOwner, float fTimePerSec );

public:
	bool CheckWeaponDead( ioBaseChar* pOwner );
	virtual void SetWeaponDead();

public:
	virtual bool IsIgnoreMapCollision() const;
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

public:
	ioRotateReturnWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioRotateReturnWeapon();
};

inline ioRotateReturnWeapon* ToRotateReturnWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ROTATE_RETURN_WEAPON )
		return NULL;

	return dynamic_cast< ioRotateReturnWeapon* >( pWeapon );
}

