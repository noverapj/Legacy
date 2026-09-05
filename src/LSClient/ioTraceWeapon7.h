#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioTraceWeapon7 : public ioWeapon
{
public:
	enum TraceState
	{
		TS_MOVE,
		TS_TRACING,
		TS_EXPLOSION,
	};

protected:
	TraceState m_State;

	// Move
	ChangeDirInfoList m_ChangeDirInfoList;
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

	// Explosion
	ioHashString    m_BoomEffect;
	CEncrypt<float> m_fExplosionRange;

	DWORD m_dwExplosionStart;

	ioHashString m_szTraceMapEffect;

	CEncrypt<bool> m_bNoChangeOwnerPos;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void OnMove( ioPlayStage *pStage, float fTimePerSec );	
	void OnTracing( ioPlayStage *pStage, float fTimePerSec );
	void OnExplosion();

	float GetMoveSpeed();
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	
	virtual void CheckPreCollision( ioPlayEntity *pEntity, ioPlayStage *pStage );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual DWORD GetCollisionEnableTime() const;

	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;

	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

	virtual void SetOwnerChargeScaleRate( float fRate );

	virtual void SetWeaponDead();
	virtual void SetMapCollision();

public:
	void SetTraceTarget( ioHashString &szName );
	void SetTraceState( ioPlayStage *pStage );
	void SetExplosionState( ioPlayStage *pStage );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	bool CheckDownTargetState( ioBaseChar *pTarget );
	void SetPositionOwner( D3DXVECTOR3 vPos );

public:
	ioTraceWeapon7( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioTraceWeapon7();
};

inline ioTraceWeapon7* ToTraceWeapon7( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_TRACE7 )
		return NULL;

	return dynamic_cast< ioTraceWeapon7* >( pWeapon );
}

