#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioTraceWeapon6 : public ioWeapon
{
public:
	enum TraceState
	{
		TS_MOVE,
		TS_SENSING,
		TS_TRACING,
		TS_RETURNING,
		TS_END,
	};

protected:
	TraceState m_State;

	//return
	D3DXVECTOR3 m_vStartPos;
	ioHashString m_szReturnTargetName;

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

	//collision
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;
	ioHashString m_BoomEffect;

	CEncrypt<float> m_fStartHeight;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void SetReturnState( ioPlayStage *pStage );

	void OnMove( ioPlayStage *pStage, float fTimePerSec );	
	void OnSensing( ioPlayStage *pStage, float fTimePerSec );
	bool OnTracing( ioHashString& szTarget, ioPlayStage *pStage, float fTimePerSec );
	void OnReturning( ioPlayStage *pStage, float fTimePerSec );

	bool IsEnableTargetState( ioBaseChar *pChar );

	float GetMoveSpeed();
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );

public:
	void SetOwnerWeaponCollisionInfo( exReserveWeaponCollisionInfo exCollisionInfo );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );

	virtual DWORD GetCollisionEnableTime() const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void SetWeaponDead();

protected:
	virtual void SetOwnerChargeScaleRate( float fRate );

public: 

	void SetTraceState( ioPlayStage *pStage, bool bSendNetwork );
	void SetTraceTarget( ioHashString &szName );		
	bool ProcessTrace( D3DXVECTOR3 vPos, D3DXVECTOR3 vTargetPos, float fTimePerSec, bool bIgnoreTraceTime=false );
	
protected:
	bool CreateFireBall( const ioHashString &szFileName );

public:
	virtual bool IsIgnoreTestMapCollision() const;

public:
	ioTraceWeapon6( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioTraceWeapon6();
};

inline ioTraceWeapon6* ToTraceWeapon6( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_TRACE6 )
		return NULL;

	return dynamic_cast< ioTraceWeapon6* >( pWeapon );
}

