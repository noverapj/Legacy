#pragma once
#include "iodummychar.h"
class ioTargetTrace16Dummy :
	public ioDummyChar
{
private:
	enum TraceDummyState
	{
		TDS_START,
		TDS_DELAY,
		TDS_SENSING,
		TDS_TRACING,
		TDS_TRACE_WAIT,
		TDS_ATTACK,
		TDS_JUMP,
		TDS_LANDING_ATTACK,
		TDS_RETURN,
	};

	enum DummyCharSyncType
	{
		DCST_TRACING		= 1000,
		DCST_TRACING_ATTACK = 1001,
		DCST_JUMP			= 1002,
		DCST_LAND_ATTACK	= 1003,
		DCST_RETURN			= 1004,
	};

private:
	TraceDummyState	m_TraceDummyState;
	float			m_fJumpPower;
	int				m_iLandingAttackCode;
	int				m_iCurAttackCount;

	DWORD			m_dwJumpStartTime;

	ioHashString	m_szJumppingAnimation;
	float			m_fJumppingAniRate;

	DWORDVec		m_vWeaponIndexList;

	ioHashString	m_szReturnAnimation;
	float			m_fReturnAniRate;
	float			m_fReturnMoveSpeed;
	DWORD			m_dwReturnEndTime;
	DWORD			m_dwReturnDuration;
	float			m_fReturnOwnerGap;

	void SetAttackAni( ioBaseChar *pOwner, int iAttackCode );
	void ClearWeaponIndexList();
private:
	virtual void ProcessPos( float fTimePerSec );
	void SetSensingState();
	void SetTracingState( ioBaseChar *pOwner );
	void SetTracingAttack( ioBaseChar *pOwner );
	void SetJumpState();
	void SetLandingAttack( ioBaseChar *pOwner );
	void SetTraceWaitState( ioBaseChar *pOwner );
	void SetReturnState( ioBaseChar *pOwner );

	void ProcessSensingState( ioBaseChar *pOwner );
	void ProcessTracingState( ioBaseChar *pOwner, float fTimerPerSec );
	void ProcessAttackState( ioBaseChar *pOwner, float fTimerPerSec, DWORD dwPreTime );
	void ProcessJumpState( ioBaseChar *pOwner );
	void ProcessTraceMoving( ioBaseChar *pOwner, ioPlayStage *pStage, D3DXVECTOR3 vMoveDir, float fMoveAmt );
	void ProcessGravityMove( float fTimePerSec );
	void ProcessReturnState( ioBaseChar *pOwner );

	void CheckTraceAttack( ioBaseChar *pOwner );
	bool CheckLanding( ioBaseChar *pOwner );

	ioHashString GetSearchTarget( ioPlayStage *pStage, bool bSecond );
private:
	virtual void SetDelayState( bool bNet );
	virtual void DropZoneCheck();
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );

	virtual void CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
public:
	ioTargetTrace16Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioTargetTrace16Dummy(void);
};

inline ioTargetTrace16Dummy* ToTargetTrace16Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE16 )
		return NULL;

	return dynamic_cast< ioTargetTrace16Dummy* >( pDummy );
}