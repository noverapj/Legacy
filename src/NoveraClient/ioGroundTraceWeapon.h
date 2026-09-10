#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioGroundTraceWeapon : public ioWeapon
{
public:
	enum TraceState
	{
		TS_MOVE,
		TS_SENSING,
		TS_TRACING,
		TS_WAIT_EXPLOSION,
		TS_EXPLOSION,
	};

protected:
	TraceState m_State;

	// Move
	ChangeDirInfoList m_ChangeDirInfoList;
	float	m_fCurMoveSpeed;
	float	m_fCurMoveRange;

	// Sensing
	TargetWoundType m_TargetWoundType;

	DWORD m_dwSensingEnableTime;
	DWORD m_dwSensingDuration;
	CEncrypt<float> m_fSensingRange;
	CEncrypt<float> m_fSensingAngle;

	DWORD m_dwSensingStartTime;

	// Trace
	CEncrypt<float> m_fTraceSpeed;
	CEncrypt<float> m_fTraceRange;
	CEncrypt<float> m_fTraceRotSpeed;

	D3DXVECTOR3 m_vPrePos;
	D3DXVECTOR3 m_vMoveDir;
	ioHashString m_TargetName;

	CEncrypt<DWORD> m_dwTraceDuration;
	DWORD m_dwTraceEndTime;

	// For Explosion Wait
	DWORD m_dwExplosionTime;
	DWORD m_dwExplosionWaitTime;

	// For Explosion
	DWORD m_dwExplosionStart;
	DWORD m_dwExplosionDuration;
	float m_fExplosionRange;

	bool m_bMapCheck;
	
	//
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;
	ioHashString m_BoomEffect;
	
	float m_fStartHeight;
	float m_fEnableMoveHeight;


	int m_iTest;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	void SetExplosionState( ioPlayStage *pStage );

	void OnMove( ioPlayStage *pStage, float fTimePerSec );	
	void OnSensing( ioPlayStage *pStage, float fTimePerSec );
	void OnTracing( ioPlayStage *pStage, float fTimePerSec );
	void OnWaitExplosion( ioPlayStage *pStage );
	void OnExplosion( float fTimePerSec );

	bool IsEnableTargetState( ioBaseChar *pChar );

	float GetMoveSpeed();
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual bool IsCanMineExplosion();

	virtual DWORD GetCollisionEnableTime() const;

	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;

	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	void SetTraceState( ioPlayStage *pStage, bool bSendNetwork );
	void SetTraceTarget( ioHashString &szName );

	void SetExplosionWaitState( ioPlayStage *pStage, bool bSendNetwork );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void ProcessMoving( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec );

public:
	ioGroundTraceWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioGroundTraceWeapon();
};

inline ioGroundTraceWeapon* ToGroundTraceWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_GROUND_TRACE )
		return NULL;

	return dynamic_cast< ioGroundTraceWeapon* >( pWeapon );
}

