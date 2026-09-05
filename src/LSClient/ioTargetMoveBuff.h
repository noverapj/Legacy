#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioTargetMoveBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_NONE,
		BS_DELAY,
		BS_JUMP,
		BS_MOVE,
		BS_ATTACK,
		BS_END,
	};
	
protected:
	//카메라
	DWORD m_dwCameraEvent;
	ioHashString m_szCameraBuff;

	//점프 파워, 중력
	float m_fJumpPower;
	float m_fGravityRate;

	//마법진 설정
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	ioHashString m_ReadyCircle;

	ioHashString m_szDelayAni;
	DWORD m_dwDelayTime;
	DWORD m_dwCheckDelay;

	ioHashString m_szJumpAni;
	float m_fJumpAniRate;

	ioHashString m_szAttackAni;
	float m_fAttackAniRate;

	AttackAttribute m_AttackAttribute;

	CEncrypt<bool> m_bEnableWeaponSkill;
	CEncrypt<bool> m_bEnableArmorSkill;
	CEncrypt<bool> m_bEnableHelmetSkill;
	CEncrypt<bool> m_bEnableCloakSkill;
	
protected:
	BuffState m_BuffState;
	
	D3DXVECTOR3 m_vCirclePos;

	//이동 시간
	float m_fFlightDuration;
	DWORD m_dwCheckJumpTime;

	//이동 방향
	D3DXVECTOR3 m_vMoveDir;
	float m_fMoveSpeed;

	DWORD m_dwAttackEndTime;

	bool m_bWound;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	
public:
	void ApplyExtraBuffInfo( SP2Packet &rkPacket );	

protected:
	void SetCameraBuff( ioBaseChar *pOwner );
	void EndCameraBuff( ioBaseChar *pOwner );
	void SetCameraEvent( ioBaseChar *pOwner );
	void EndCameraEvent( ioBaseChar *pOwner );

protected:
	void CreateMagicCircle( ioBaseChar *pOwner );
	void UpdateMagicCircle( ioBaseChar *pOwner );

protected:
	void SetCharBuffState( ioBaseChar *pOwner );
	void SetPreDelayState( ioBaseChar *pOwner );
	void SetJumpState( ioBaseChar *pOwner );
	void SetMoveState( ioBaseChar *pOwner );
	void SetAttackState( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner );

protected:
	float GetFlightDuration( float fCurGravity );
	void CheckMove( ioBaseChar *pOwner );
	bool IsFloatState( ioBaseChar *pOwner );
	bool CheckWoundState( ioBaseChar *pOwner );
	void DestroyObjectItemSkill( ioBaseChar *pOwner );

public:
	bool IsCharCollisionSkipState();

	bool CheckEnableUseSkill( int iSlot );

public:
	ioTargetMoveBuff();
	ioTargetMoveBuff( const ioTargetMoveBuff &rhs );
	virtual ~ioTargetMoveBuff();
};

inline ioTargetMoveBuff* ToTargetMoveBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TARGET_MOVE )
		return NULL;

	return dynamic_cast< ioTargetMoveBuff* >( pBuff );
}