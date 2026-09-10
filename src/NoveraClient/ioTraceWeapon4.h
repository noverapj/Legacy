#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioTraceWeapon4 : public ioWeapon
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

	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;
	CEncrypt<float> m_fStartHeight;

	//
	int m_iTest;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void OnMove( ioPlayStage *pStage, float fTimePerSec );	
	void OnSensing( ioPlayStage *pStage, float fTimePerSec );
	void OnTracing( ioPlayStage *pStage, float fTimePerSec );

	bool IsEnableTargetState( ioBaseChar *pChar );

	float GetMoveSpeed();
	void CheckChangeDir();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual DWORD GetCollisionEnableTime() const;

	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;

	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	void SetTraceState( ioPlayStage *pStage, bool bSendNetwork );
	void SetTraceTarget( ioHashString &szName );

	virtual void SetOwnerChargeScaleRate( float fRate );

	void RotateMoveDir( float fAngleH, float fAngleV );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioTraceWeapon4( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioTraceWeapon4();
};

inline ioTraceWeapon4* ToTraceWeapon4( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_TRACE4 )
		return NULL;

	return dynamic_cast< ioTraceWeapon4* >( pWeapon );
}

