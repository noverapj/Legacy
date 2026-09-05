#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioTraceWeapon5 : public ioWeapon
{
public:
	enum TraceState
	{
		TS_ROTATE,
		TS_TRACING,
		TS_EXPLOSION,
		TS_END,
	};

protected:
	TraceState m_State;

	ioHashString	m_FireSound;
	// Rotate
	CEncrypt<float>	m_fRotateSpeed;
	CEncrypt<float> m_fRotateRange;

	// Move
	float	m_fCurMoveSpeed;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	// Trace
	CEncrypt<float> m_fTargetTracePosRate;
	CEncrypt<float> m_fDownTargetTracePosRate;

	DWORD m_dwTraceEnableTime;
	CEncrypt<float> m_fTraceSpeed;
	CEncrypt<float> m_fTraceRange;
	CEncrypt<float> m_fTraceRotSpeed;
	CEncrypt<float> m_fTraceEndRange;

	D3DXVECTOR3 m_vPrePos;
	D3DXVECTOR3 m_vMoveDir;
	ioHashString m_TargetName;

	CEncrypt<DWORD> m_dwTraceDuration;
	DWORD m_dwTraceEndTime;
	
	//
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;

	CEncrypt<bool>	m_bOnlyCharCollision;

	bool m_bSetDeadArriveTarget;

	// Explosion
	CEncrypt<bool>	m_bExplosionWeapon;
	CEncrypt<float>	m_fExplosionRange;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void OnRotate( ioPlayStage *pStage, float fTimePerSec );
	void OnTracing( ioPlayStage *pStage, float fTimePerSec );

	float GetMoveSpeed();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

	virtual void SetOwnerChargeScaleRate( float fRate );

	virtual void SetWeaponDead();

public:
	void SetTraceState( ioPlayStage *pStage );
	void SetTraceTarget( ioHashString &szName );
	void SetEndState( ioPlayStage *pStage );
	void SetExplosionState( ioPlayStage *pStage, bool bSend );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	bool CheckDownTargetState( ioBaseChar *pTarget );

public:
	virtual bool IsIgnoreMapCollision() const;
	virtual bool IsIgnoreTestMapCollision() const;

public:
	ioTraceWeapon5( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioTraceWeapon5();
};

inline ioTraceWeapon5* ToTraceWeapon5( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_TRACE5 )
		return NULL;

	return dynamic_cast< ioTraceWeapon5* >( pWeapon );
}

