#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioReturnMissileWeapon2 : public ioWeapon
{
public:
	enum TraceState
	{
		TS_ROTATE,
		TS_TRACING,
		TS_END,
	};

protected:
	TraceState m_State;

	ioHashString	m_FireSound;
	ioHashStringVec m_AddBuffList;

	bool			m_bAddedBuffList;

	// Move
	CEncrypt<float>	m_fCurMoveSpeed;
	float	m_fCurMoveRange;
	CEncrypt<float> m_fMaxMoveRange;
	CEncrypt<float> m_fCheckMaxRange;

	D3DXVECTOR3 m_vMoveDir;

	D3DXVECTOR3 m_vPrePos;

	//
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;


protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );


public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

	virtual void SetOwnerChargeScaleRate( float fRate );

	virtual void SetWeaponDead();


protected:
	bool CreateFireBall( const ioHashString &szFileName );

public:
	virtual bool IsIgnoreMapCollision() const;

public:
	ioReturnMissileWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioReturnMissileWeapon2();
};

inline ioReturnMissileWeapon2* ToReturnMissileWeapon2( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_RETURN_MISSILE_WEAPON2 )
		return NULL;

	return dynamic_cast< ioReturnMissileWeapon2* >( pWeapon );
}

