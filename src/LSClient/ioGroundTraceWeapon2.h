#pragma once

#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioGroundTraceWeapon2 : public ioWeapon
{
public:
	enum TraceState
	{
		TS_PRE,
		TS_MOVE,
		TS_SENSING,
		TS_TRACING,
		TS_WAIT_EXPLOSION,
		TS_EXPLOSION,
	};

protected:
	TraceState m_State;
	TraceState m_PrevState;

	//Move
	ChangeDirInfoList m_ChangeDirInfoList;
	CEncrypt<float>	m_fCurMoveSpeed;
	CEncrypt<float>	m_fCurMoveRange;

	//Sensing
	TargetWoundType m_TargetWoundType;

	CEncrypt<DWORD> m_dwSensingEnableTime;
	CEncrypt<DWORD> m_dwSensingDuration;
	CEncrypt<float> m_fSensingRange;
	CEncrypt<float> m_fSensingMaxAngle;
	CEncrypt<float> m_fSensingMinAngle;
	
	CEncrypt<DWORD> m_dwSensingStartTime;

	//Trace
	CEncrypt<float> m_fTraceSpeed;
	CEncrypt<float> m_fTraceRange;
	CEncrypt<float> m_fTraceRotSpeed;
		
	D3DXVECTOR3 m_vMoveDir;
	ioHashString m_TargetName;

	CEncrypt<DWORD> m_dwTraceDuration;
	CEncrypt<DWORD> m_dwTraceEndTime;

	//For Explosion Wait
	CEncrypt<DWORD> m_dwExplosionTime;
	CEncrypt<DWORD> m_dwExplosionWaitTime;

	//For Explosion
	CEncrypt<DWORD> m_dwExplosionStart;
	CEncrypt<DWORD> m_dwExplosionDuration;
	CEncrypt<DWORD> m_dwExplosionLoopTime;
	CEncrypt<DWORD> m_dwExplosionLoopStartTime;
	
	CEncrypt<float> m_fExplosionRange;

	CEncrypt<DWORD> m_dwDealyTime;
	bool m_bWeaponDead;	

	//
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;
	ioHashString m_BoomEffect;

	CEncrypt<float> m_fStartHeight;
	CEncrypt<float> m_fEnableMoveHeight;
	
	//Call Attribute
	WeaponInfoList	m_CallWeaponInfoList;
	bool m_bArleadyCallWeapon;

	bool m_bDropZoneCheck;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void ProcessMove( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec );

protected:
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	void SetExplosionState( ioPlayStage *pStage );

	void OnMove( ioPlayStage *pStage, float fTimePerSec );	
	void OnSensing( ioPlayStage *pStage, float fTimePerSec );
	void OnTracing( ioPlayStage *pStage, float fTimePerSec );
	void OnWaitExplosion( ioPlayStage *pStage );
	void OnExplosion( float fTimePerSec );
	bool ProcessTrace( D3DXVECTOR3 vPos, D3DXVECTOR3 vTargetPos, float fTimePerSec, bool bIgnoreTraceTime=false );

	bool IsEnableTargetState( ioBaseChar *pChar );

	float GetMoveSpeed();
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );

public:
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void CallAttribute();

	void SetTraceState( ioPlayStage *pStage, bool bSendNetwork );
	void SetTraceTarget( ioHashString &szName );

	void SetExplosionWaitState( ioPlayStage *pStage, bool bWeaponDead, bool bSendNetwork );
	
	bool CheckMaxAngle( ioBaseChar*pTarget, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, float fMaxAngle );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual DWORD GetCollisionEnableTime() const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual bool IsCanMineExplosion();
	
public:
	virtual WeaponType GetType() const;

	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioGroundTraceWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioGroundTraceWeapon2();
};

inline ioGroundTraceWeapon2* ToGroundTraceWeapon2( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_GROUND_TRACE2 )
		return NULL;

	return dynamic_cast< ioGroundTraceWeapon2* >( pWeapon );
}

