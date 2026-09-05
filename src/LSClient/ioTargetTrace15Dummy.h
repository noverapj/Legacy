#pragma once
#include "ioDummyChar.h"

class ioTargetTrace15Dummy :	public ioDummyChar
{
protected:
	enum TraceState
	{
		TS_START,
		TS_DELAY,
		TS_SENSING,
		TS_TARGET_TRACE,
		TS_TARGET_TRACE_WAIT,
		TS_TARGET_ATTACK,
		TS_END,
	};
protected:
	TraceState			m_TargetTraceState;
	DWORD				m_dwFireStartTime;
	WeaponInfo			m_StartWeaponInfo;

	int					m_iCurAttackCount;
	std::vector<int>	m_vAttackCode;

	bool					m_bNoDropZoneMove;

	float					m_fCurTraceSpeed;
	float					m_fFirstSensingMoveSpeed;
	DWORD					m_dwFirstSensingMoveEndTime;
	DWORD					m_dwFirstSensingMoveDuration;
protected:
	void		 ProcessStartState( ioBaseChar *pOwner );
	void		 ProcessTraceSensing( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec );
	void		 ProcessTraceMoving( ioBaseChar *pOwner, ioPlayStage *pStage, D3DXVECTOR3 vMoveDir, float fMoveAmt, bool bNoDropZoneMove );
	void		 ProcessTraceTracing( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec );
	void		 ProcessTraceAttack( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime );

	void		 SetSensingState( float fMoveSpeed );
	void		 SetTargetTracingState( ioPlayStage *pStage, bool bSendNetwork );
	void		 SetTraceAttack( ioBaseChar *pOwner );
	void		 SetTargetTraceWaitState();

	void		 CheckTargetTraceAttack( ioBaseChar *pOwner, ioPlayStage *pStage );

	ioHashString GetSearchTarget( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSecond );
	
public:
	virtual void InitDummyCharInfo(	int iIndex,
				DWORD dwCreateGapTime,
				const D3DXVECTOR3 &vPos,
				const D3DXVECTOR3 &vScale,
				float fStartAngle=0.0f, bool bNetWork = false );

	virtual void LoadProperty( ioINILoader &rkLoader );
public:
	virtual void SetDelayState( bool bNet );

	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( ioBaseChar* pOwner, float fTimePerSec );

	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
protected:
	virtual void DropZoneCheck();
public:
	ioTargetTrace15Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioTargetTrace15Dummy(void);
};

inline ioTargetTrace15Dummy* ToTargetTrace15Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE15 )
		return NULL;

	return dynamic_cast< ioTargetTrace15Dummy* >( pDummy );
}