#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioTraceReturnWeapon : public ioWeapon
{
public:
	enum TraceState
	{
		TS_MOVE,
		TS_SENSING,
		TS_TRACING,
		TS_RETURN,
	};

protected:
	TraceState m_State;

	// Move
	float m_fCurMoveRange;
	float m_fCurMoveSpeed;
	ChangeDirInfoList m_ChangeDirInfoList;

	// Sensing
	TargetWoundType m_TargetWoundType;

	DWORD m_dwSensingEnableTime;
	DWORD m_dwSensingDuration;
	CEncrypt<float> m_fSensingRange;
	CEncrypt<float> m_fSensingAngle;

	DWORD m_dwSensingStartTime;

	// Trace
	CEncrypt<DWORD> m_dwTraceRotSpeed;

	D3DXVECTOR3 m_vPrePos;
	D3DXVECTOR3 m_vMoveDir;
	ioHashString m_TargetName;
	
	//
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;

	//
	DWORD m_dwReturnStartTime;
	DWORD m_dwReturnDuration;
	float m_fReturnEndRange;

	int m_iTest;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	bool CheckReturnTargetValidate( ioPlayStage *pStage );
	bool CheckReturnEndRange();

	void OnMove( ioPlayStage *pStage, float fTimePerSec );	
	void OnSensing( ioPlayStage *pStage, float fTimePerSec );
	void OnTracing( ioPlayStage *pStage, float fTimePerSec );
	void OnTracingReturn( ioPlayStage *pStage, float fTimePerSec );

	void ChangeToReturnState( ioPlayStage *pStage, bool bSendNetwork );

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

	virtual DWORD GetCollisionEnableTime() const;

	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;

	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

	void SetTraceTarget( ioHashString &szName );
	void SetTraceState( ioPlayStage *pStage, bool bSendNetwork );
	void SetReturnState( ioPlayStage *pStage, bool bSendNetwork );

	virtual void SetOwnerChargeScaleRate( float fRate );


public:
	ioTraceReturnWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioTraceReturnWeapon();
};

inline ioTraceReturnWeapon* ToTraceReturnWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_TRACE_RETURN )
		return NULL;

	return dynamic_cast< ioTraceReturnWeapon* >( pWeapon );
}

