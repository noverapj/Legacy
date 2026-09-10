#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioJumpBoostBuff : public ioBuff
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
	
	// 용병의 bottomHeight값 저장 (바닥이 없어 m_pOwner->GetBottomHeight()의 값이 0.0f일때 마지막으로 가지고 있던 m_pOwner->GetBottomHeight()값으로 
	// 처리를 해야되기때문에 저장)
	float m_fOwnerCharBottomHeight;

	// 부스터 시에만 
	bool			m_bEnableBoostAttack;
	AttackAttribute m_BoostAttackAttribute;

protected:
	bool m_bEnableEndJump;
	float m_fEndJumpPower;
	bool m_bMustEndJump;
	bool m_bInitCurSpeed;
	bool m_bIsCanStopByKeyInput;
	float m_fMaxHeight;				// 점프 부스트 사용 시 올라갈 수 있는 최대 높이
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

	// 현재 이 버프가 적용되어있는 상태라면 셋팅한 높이 제한 값에 따라 용병이 뛰어 오를 수 있는 Y축을 제한되게 함, 용병 Y 위치가 제한 값보다 같거나 크면 true
	bool CheckAxisLimitByBuff( ioBaseChar* pChar );

protected:
	void SetFlyDelayState();
	void SetFlyMoveState();
	void SetFlyBoostState();
	void AreaWeaponFire();

public:
	ioJumpBoostBuff();
	ioJumpBoostBuff( const ioJumpBoostBuff &rhs );
	virtual ~ioJumpBoostBuff();
};


inline ioJumpBoostBuff* ToJumpBoostBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_JUMP_BOOST_BUFF )
		return NULL;

	return dynamic_cast< ioJumpBoostBuff* >( pBuff );
}


