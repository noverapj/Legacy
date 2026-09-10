#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioTraceWeaponType2 : public ioWeapon
{
public:
	enum TraceState
	{
		TS_MOVE,
		TS_TRACING,
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

	bool m_bMapCheck;
	
	//
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;
	float m_fStartHeight;
	//

	CEncrypt<bool>	m_bOnlyCharCollision;

	//충돌시 Dir로 어택 방향을 체크
	bool		m_bCheckAttackDir;

	bool	m_bCallAreaWeaponExceptChar;
	bool	m_bNoCallAreaWeaponOnContactChar;

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
	void SetTraceTarget( const ioHashString &szName );

	virtual void SetOwnerChargeScaleRate( float fRate );

	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;

	virtual void CallReserveAreaWeapon();

protected:
	bool CreateFireBall( const ioHashString &szFileName );

	bool CheckDownTargetState( ioBaseChar *pTarget );

public:
	ioTraceWeaponType2( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioTraceWeaponType2();
};

inline ioTraceWeaponType2* ToTraceWeaponType2( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_TRACE2 )
		return NULL;

	return dynamic_cast< ioTraceWeaponType2* >( pWeapon );
}

