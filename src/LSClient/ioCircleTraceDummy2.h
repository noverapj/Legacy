#pragma once
#include "ioDummyChar.h"
class ioCircleTraceDummy2 :	public ioDummyChar
{
protected:
	enum CircleTraceDummyState
	{
		CTDS_START,
		CTDS_MOVE,
	};

	enum DummySyncState
	{
		DSS_CHANGE_TARGET_CIRCLE = 1000,
	};

	struct WeaponFiredTarget
	{
		ioHashString szTargetName;
		DWORD		 dwFireTime;
	};

	typedef std::vector<WeaponFiredTarget> WeaponFiredTargetList;

protected:

	/////Circle Move
	CircleTraceDummyState	m_CircleTraceDummyState;
	Vector3Vec				m_CirclePositionList;
	D3DXVECTOR3				m_vTargetCirclePos;
	D3DXVECTOR3				m_vTargetCircleDir;
	float					m_fTargetCircleHeightOffset;
	float					m_fCurMoveRange;
	float					m_fMaxMoveRange;
	float					m_fCircleTraceSpeed;
	/////

	/////Target
	WeaponFiredTargetList	m_WeaponFiredTargetList;
	DWORD					m_dwAttackTicTime;
	WeaponInfoList			m_DummyAttackList;
	D3DXVECTOR3				m_DummyAttackOffset;
	/////

	//Effect
	ioHashString			m_szCircleLastPositionEffect;
	DWORD					m_dwCircleLastPositionEffectID;


	//무기 순서
	int						m_nWeaponIndex;

protected:
	void SetState( CircleTraceDummyState eState );
	void SetNextTargetCircle( bool bSend );

	void ProcessStart( ioBaseChar *pOwner, ioPlayStage *pStage );
	void ProcessMoveState( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimerPerSec );
	void ProcessSensingAttack( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimerPerSec );

	void SearchTarget( ioBaseChar *pOwner, ioPlayStage *pStage, ioHashStringVec& vTargetList );

	bool CheckTargetState( ioBaseChar *pOwner, ioBaseChar *pTarget );
	bool CheckSkipBuff( ioBaseChar *pTarget );

	void CheckTargetWeaponFire( ioBaseChar *pOwner, ioPlayStage *pStage, const ioHashStringVec& vTargetList );
	void CreateWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioHashString szTargetName );

public:
	void AddCirclePosition( D3DXVECTOR3 vPos );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

public:
	virtual bool IsDummyCharMidSync(){ return true; }
	virtual void FillDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket );
	virtual void ApplyDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket );

public:
	ioCircleTraceDummy2( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioCircleTraceDummy2(void);
};

inline ioCircleTraceDummy2* ToCircleTraceDummy2( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_CIRCLE_TRACE2 )
		return NULL;

	return dynamic_cast< ioCircleTraceDummy2* >( pDummy );
}