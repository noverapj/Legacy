#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioFlyBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_NONE,
		BS_DIE,
		BS_ATTACK,
	};

protected:
	float m_fFlyStartJumpPower;
	float m_fFlyJumpPower;
	float m_fFlyMoveSpeed;
	float m_fFlyFrictionAmt;
	float m_fFlyOffHeight;
	float m_fFlyGravityAmt;

	DWORD m_dwFlyRotateTime;
	
	DWORD m_dwFlyOffTime;
	DWORD m_dwPreCheckTime;

	ioHashString m_FlyDelayAnimation;
	ioHashString m_FlyDelayEffect;

	ioHashString m_FlyMoveAnimation;
	ioHashString m_FlyMoveEffect;

	ioHashString m_FlyBoostAnimation;
	ioHashString m_FlyBoostEffect;

	ioHashString m_FlyExtraEffect;

	DWORD m_dwFlyBoostEndTime;
	DWORD m_dwFlyBoostDuration;

	bool m_bSetFlyState;

	FlyMoveType m_FlyMoveType;
	float m_fPreMoveSpeed;

	UniqueObjID m_dwCurEffectID;
	CEncrypt<bool> m_bUseNoSkillState;

	CEncrypt<bool> m_bEscapeCatchState;

	bool m_bUseFireAttack;
	ioHashString m_stAreaWeaponName;
	float m_fAreaWeaponOffset;
	
	ioHashString m_stAttackAni;
	float	m_fAttackAniRate;
	DWORD m_dwAttackMotionEndTime;
	

protected:
	bool m_bEnableEndJump;
	float m_fEndJumpPower;
	bool m_bMustEndJump;
	bool m_bInitCurSpeed;
	bool m_bIsCanStopByKeyInput;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	FlyMoveType GetFlyMoveType();

	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void SetFlyDelayState();
	void SetFlyMoveState();
	void SetFlyBoostState();
	void AreaWeaponFire();

public:
	ioFlyBuff();
	ioFlyBuff( const ioFlyBuff &rhs );
	virtual ~ioFlyBuff();
};


inline ioFlyBuff* ToFlyBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_FLYING_CONTRAL )
		return NULL;

	return dynamic_cast< ioFlyBuff* >( pBuff );
}


