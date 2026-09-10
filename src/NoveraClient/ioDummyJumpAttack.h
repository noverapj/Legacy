#pragma once
#include "iodummychar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummyJumpAttack :public ioDummyChar
{
public:
	enum DummyJumpAttackState
	{
		DJS_START,
		DJS_JUMP,
		DJS_SENSING,
		DJS_MOVE_ATTACK,
		DJS_LAND_ATTACK,
		DJS_END,
	};

protected:		
	DummyJumpAttackState	m_JumpAttackState;
	float					m_fStartMaxRange;
	float					m_fFlyCurRange;
	DWORD					m_dwStartDuration;
	float					m_fStartFlyAngle;

	ioHashString			m_szTargetName;

	int						m_iMoveAttackCode;
	int						m_iLandAttackCode;

	ioHashString			m_JumpAni;
	float					m_fJumpAniRate;

	DWORD					m_dwLandAttackAniEndTime;

	int						m_iMaxAttackCount;
	int						m_iCurAttackCount;

	D3DXVECTOR3				m_vMoveAttackOffset;
	D3DXVECTOR3				m_vLandAttackOffset;

	DWORD					m_dwMoveAttackTime;
	DWORD					m_dwMoveAttackStartTime;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	bool ProcessMoveAttack( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime );
	void CheckAttackTime( ioBaseChar *pOwner, DWORD dwPreTime, D3DXVECTOR3 vOffset = ioMath::VEC3_ZERO );
	void SetSensingState();
	void SetMoveAttackState();
	void SetJumpState();
	void SetLandAttack();
	void CheckLive();
	bool SetAttackAttribute( ioBaseChar *pOwner, int iAttackCode, DWORD& dwEndTime );
protected:
	bool ProcessJump( ioBaseChar *pOwner, float fTimerPerSec );
	void SensingTarget( ioBaseChar *pOwner );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	//virtual bool IsActivateWeapon();
	//virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );

public:
	ioDummyJumpAttack( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyJumpAttack();
};

inline ioDummyJumpAttack* ToDummyJumpAttack( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_JUMP_ATTACK )
		return NULL;

	return dynamic_cast< ioDummyJumpAttack* >( pDummy );
}

