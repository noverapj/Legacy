#pragma once
#include "ioDummyChar.h"
class ioCircleTraceDummy :	public ioDummyChar
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
	WeaponInfo				m_DummyAttack;
	D3DXVECTOR3				m_DummyAttackOffset;
	/////

	//Effect
	ioHashString			m_szCircleLastPositionEffect;
	DWORD					m_dwCircleLastPositionEffectID;
	float					m_fCircleLastEffectOffsetX;
	float					m_fCircleLastEffectOffsetY;

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
	float GetHeightOffset() { return m_fDummyCharHeightOffset; }
protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
public:
	ioCircleTraceDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioCircleTraceDummy(void);
};

inline ioCircleTraceDummy* ToCircleTraceDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_CIRCLE_TRACE )
		return NULL;

	return dynamic_cast< ioCircleTraceDummy* >( pDummy );
}