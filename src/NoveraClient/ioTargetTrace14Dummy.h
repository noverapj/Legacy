#pragma once
#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;
class ioTargetTrace14Dummy : public ioDummyChar
{
protected:
	enum TraceState
	{
		TS_START,
		TS_OWNER_ROTATE_MOVE,
		TS_TARGET_TRACE,
		TS_TARGET_FIRST_ATTACK,
		TS_TARGET_ROTATE,
		TS_TARGET_SENSING_STATE,
		TS_TARGET_SECOND_ATTACK,
		TS_END,
	};
protected:
	TraceState		m_TargetTraceState;
	DWORD			m_dwLifeTime;
	
	//Owner Rot
	DWORD			m_dwRotateSpeed;
	float			m_fCurOwnerAttachAngle;
	D3DXVECTOR3		m_vAttachOffset;
	ioHashString	m_szOwnerRotAni;
	float			m_fOwnerRotAniRate;

	//Target
	ioHashString	m_szTargetName;

	//Move
	float			m_fTargetArriveDist;
	ioHashString	m_szTraceMoveAni;
	float			m_fTraceMoveAniRate;
	float			m_fTraceTargetMoveSpeed;

	//Attack
	int				m_iAttackCount;
	//First Attack
	AttackAttribute m_FirstAttack;
	//Sensing
	DWORD			m_dwSecondTargetSensingTime;
	DWORD			m_dwSecondTargetSensingStartTime;
	//Second Attack
	AttackAttribute m_SecondAttack;

	//Target Rotate
	float			m_fCurTargetAttachAngle;
	float			m_fTargetRotateMaxAngleMove;
	float			m_fCurTargetRotateAngleMove;
	float			m_fTargetRotateSpeed;
	float			m_fTargetRotateLength;
	float			m_fTargetRotateYaw;
	D3DXVECTOR3		m_vTargetCenterPos;

	bool			m_bCheckOwnerItem;
	DWORD			m_dwCreateItemCode;

	

protected:
	void SetOwnerRotateMoveState( ioBaseChar *pOwner );
	void SetTargetTraceState( ioBaseChar *pOwner );
	void SetTargetFirstAttackState( ioBaseChar *pOwner );
	void SetTargetLook( ioBaseChar *pOwner );
	void SetTargetRotateState( ioBaseChar *pOwner );
	void SetSensingState( ioBaseChar *pOwner );
	void SetTargetSecondAttackState( ioBaseChar *pOwner );
	void SetLoopAni( const ioHashString& szLoopAni, float fTimeRate );
	void SetActionAni( ioBaseChar *pOwner,const AttackAttribute& rkCurAttribute );

	bool CheckLive( ioBaseChar *pOwner );
	bool CheckOwnerAttachPos( ioBaseChar *pOwner );
	bool CheckTarget( ioBaseChar *pOwner );
	bool CheckTargetAttackState( ioBaseChar *pOwner );
	bool CheckTargetRotatePos( ioBaseChar *pOwner );

	bool FindTarget( ioBaseChar *pOwner );

	void ProcessStartState( ioBaseChar *pOwner );
	void ProcessOwnerRotateMoveState( ioBaseChar *pOwner );
	void ProcessTargetTraceState( ioBaseChar *pOwner, float fTimerPerSec );
	void ProcessTargetMoveState( ioBaseChar *pOwner, float fTimerPerSec );
	void ProcessTargetFirstAttackState( ioBaseChar *pOwner );
	void ProcessTargetRotateState( ioBaseChar *pOwner );
	void ProcessTargetSensingState( ioBaseChar *pOwner );
	void ProcessTargetSecondAttackState( ioBaseChar *pOwner );

	bool CheckOwnerDestroyState( ioBaseChar *pOwner );
public:
	void SetTargetTraceState( ioBaseChar *pOwner, ioHashString szTargetName );

public:
	virtual void InitDummyCharInfo(	int iIndex,
		DWORD dwCreateGapTime,
		const D3DXVECTOR3 &vPos,
		const D3DXVECTOR3 &vScale,
		float fStartAngle=0.0f, bool bNetWork = false );

	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );
public:
	ioTargetTrace14Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioTargetTrace14Dummy(void);
};

inline ioTargetTrace14Dummy* ToTargetTrace14Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE14 )
		return NULL;

	return dynamic_cast< ioTargetTrace14Dummy* >( pDummy );
}