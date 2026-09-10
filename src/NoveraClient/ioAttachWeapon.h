#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioAttachWeapon : public ioWeapon
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

	CEncrypt<DWORD> m_dwTraceEnableTime;
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


	//µå¸±
	float m_fOff_Y;
	float m_fOff_Look;
	int	  m_nAttachAllyCnt;
	std::vector< ioHashString > m_vAttachName;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

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

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	float GetMoveSpeed();
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );

protected:
	void OnMove( ioPlayStage *pStage, float fTimePerSec );	
	void OnTracing( ioPlayStage *pStage, float fTimePerSec );

	void SetTraceState( ioPlayStage *pStage );

protected:
	bool CheckDownTargetState( ioBaseChar *pTarget );

public:
	ioAttachWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioAttachWeapon();
};

inline ioAttachWeapon* ToAttachWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ATTACH_WEAPON )
		return NULL;

	return dynamic_cast< ioAttachWeapon* >( pWeapon );
}

