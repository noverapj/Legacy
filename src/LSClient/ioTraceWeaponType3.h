#pragma once

// 
// ioTraceWeaponType3
// 공격자의 매직써클을 추적하는 미사일 웨폰
// 

#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioTraceWeaponType3 : public ioWeapon
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

	DWORD m_dwSensingStartTime;

	// Trace
	CEncrypt<float> m_fTraceSpeed;
	CEncrypt<float> m_fTraceRange;
	CEncrypt<float> m_fTraceRotSpeed;

	D3DXVECTOR3 m_vPrePos;
	D3DXVECTOR3 m_vMoveDir;

	CEncrypt<DWORD> m_dwTraceDuration;
	DWORD m_dwTraceEndTime;

	// For Explosion Wait
	DWORD m_dwExplosionTime;
	DWORD m_dwExplosionWaitTime;

	bool m_bMapCheck;
	
	//
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;
	ioHashString m_BoomEffect;
	float m_fStartHeight;
	//

	// 바닥 표적
	CEncrypt<bool>	m_bUseLandMark;
	ioHashString	m_szLandMarkRed;
	ioHashString	m_szLandMarkBlue;
	DWORD			m_dwLandMarkID;

	ioHashString	m_szDeadWeapon;
	DWORD			m_dwDeadWeaponID;

	bool			m_bFirst;

	WeaponInfoList	m_CallWeaponInfoList;
	bool m_bArleadyCallWeapon;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void SetExplosionState( ioPlayStage *pStage );

	void OnMove( ioPlayStage *pStage, float fTimePerSec );	
	void OnSensing( ioPlayStage *pStage, float fTimePerSec );
	void OnTracing( ioPlayStage *pStage, float fTimePerSec );
	void OnWaitExplosion( ioPlayStage *pStage );
	void OnExplosion( float fTimePerSec );
	void CallAttribute();

	bool IsEnableTargetState( ioBaseChar *pChar );

	float GetMoveSpeed();
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadCallAttributeList( ioINILoader &rkLoader );

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

	void SetExplosionWaitState( ioPlayStage *pStage, bool bSendNetwork );

	virtual void SetOwnerChargeScaleRate( float fRate );

protected:
	bool CreateFireBall( const ioHashString &szFileName );

public:
	ioTraceWeaponType3( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioTraceWeaponType3();
};

inline ioTraceWeaponType3* ToTraceWeaponType3( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_TRACE3 )
		return NULL;

	return dynamic_cast< ioTraceWeaponType3* >( pWeapon );
}

