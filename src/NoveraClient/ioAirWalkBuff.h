#pragma once
#include "iobuff.h"
#include "ioSpecialStateBase.h"

class ioAirWalkBuff :public ioBuff
{
//Air Walk Info
private:
	ioHashStringVec	 m_vJumpAnimationList;
	DWORD				m_dwJumpEnableTime;
	bool				m_bEnableRotate;
	bool				m_bContinueJump;

	float				m_fJumpPower;
	float				m_fJumpForce;
	float				m_fJumpSpeedRate;
	float				m_fGravityRate;
	float				m_fJumpFixedSpeed;

	float				m_fEnableHeight;
	float				m_fJumpEnableGauge;
	float				m_fAniRate;

	int					m_iMaxJumpCnt;
	bool				m_bAniLoop;

	CEncrypt<bool>		m_bEnableMoveRotate;
	CEncrypt<DWORD>		m_dwMoveRotateSpeed;

	bool				m_bJumpAttack;
	float				m_fWalkEndJumpAmt;

	float				m_fAirWalkAttackUsingGauge;

	bool				m_bReduceGauge;
	float				m_fBuffAirWalkStartGauge;

private:
	enum AirWalkAttackState
	{
		AWAS_NONE,
		AWAS_ATTACK,
	};

	enum AirWalkAttackSyncType
	{
		AWAST_ATTACK,
		AWAST_FIRE,
	};

	AirWalkAttackState	m_AirWalkAttackState;
	DWORD				m_dwWeaponBaseIndex;

	DWORD				m_dwFireStartTime;
	DWORD				m_dwMotionEndTime;

	float				m_fAirWalkAttackEndJumpPower;
	AttackAttribute		m_AirWalkAttack;
	
	ioHashString		m_szTargetName;
	float				m_fTargetRange;
	float				m_fTargetAngle;
	TargetWoundType		m_CurTargetWoundType;

private:
	void SetSpecialState( ioBaseChar *pOwner );
public:
	void ProcessPassive();

	//Air Walk Attack
public:
	void ProcessSpecialState( ioBaseChar *pOwner );
	bool CheckAirWalkAttackNoDropState();

	float GetFixedSpeed();
private:
	void ClearData();

	bool CheckSpecialState( ioBaseChar *pOwner );

	void SetAirWalkAttackState( ioBaseChar *pOwner );
	void SetAirWalkAttackEndState( ioBaseChar *pOwner );

	void ReduceAirWalkAttackGauge( ioBaseChar *pOwner );

	void ProcessAirWalkAttackState( ioBaseChar *pOwner );

	void FindTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	void FireWeapon( ioBaseChar *pOwner );


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();
public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual bool IsCanJumpAttack() const { return m_bJumpAttack; };

	virtual bool OnReduceGauge() const;

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );


public:
	ioAirWalkBuff(void);
	ioAirWalkBuff( const ioAirWalkBuff &rhs );
	virtual ~ioAirWalkBuff(void);
};

inline ioAirWalkBuff* ToAirWalkBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_BUFF_AIR_WALK )
		return NULL;

	return dynamic_cast< ioAirWalkBuff* >( pBuff );
}


//---------------------------------------------------------------------------------------------------------------------
class ioBuffAirWalkAttackState : public ioSpecialStateBase
{
public:
	virtual const char* GetStateName(){ return "CS_BUFF_AIR_WALK_ATTACK"; }
	virtual const CharState GetState(){ return CS_BUFF_AIR_WALK_ATTACK; }	
	//virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }
public:
	virtual bool IsNoDropState( ioBaseChar* pOwner );
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );
public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	//virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioBuffAirWalkAttackState();
	virtual ~ioBuffAirWalkAttackState();
};

inline ioBuffAirWalkAttackState* ToBuffAirWalkAttackState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_BUFF_AIR_WALK_ATTACK )
		return NULL;

	return dynamic_cast< ioBuffAirWalkAttackState* >( pState );
}