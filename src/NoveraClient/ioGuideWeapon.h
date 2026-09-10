#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"
class ioUroborusGuideArrow;

class ioGuideWeapon : public ioWeapon
{
public:
	enum TraceState
	{
		TS_MOVE,
		TS_TRACING,
		TS_END,
	};

protected:
	TraceState m_State;

	// Move
	ChangeDirInfoList m_ChangeDirInfoList;
	float	m_fCurMoveSpeed;
	float	m_fCurMoveRange;

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

	bool m_bMapCheck;

	//
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;
	float m_fStartHeight;
	//

	int m_iTest;

	CEncrypt<bool>	m_bOnlyCharCollision;

	bool bArleaySetSkill;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void OnMove( ioPlayStage *pStage, float fTimePerSec );	
	void OnTracing( ioPlayStage *pStage, float fTimePerSec );

	float GetMoveSpeed();
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	virtual void SetWeaponDead();

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual DWORD GetCollisionEnableTime() const;

	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;

	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	void SetTraceState( ioPlayStage *pStage );
	void SetTraceTarget( ioHashString &szName );

	virtual void SetOwnerChargeScaleRate( float fRate );
	virtual void SetMapCollision();

protected:
	bool CreateFireBall( const ioHashString &szFileName );

	bool CheckDownTargetState( ioBaseChar *pTarget );
	bool CheckOwnerState();

public:
	int GetState() const { return m_State; }

public:
	ioGuideWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioGuideWeapon();
};

inline ioGuideWeapon* ToGuideWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_GUIDE )
		return NULL;

	return dynamic_cast< ioGuideWeapon* >( pWeapon );
}

