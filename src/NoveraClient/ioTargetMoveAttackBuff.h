#pragma once

#include "ioBuff.h"

class ioTargetMoveAttackBuff : public ioBuff
{
protected:
	enum Buff_State
	{
		BS_None,
		BS_Wait,
		BS_Move,
		BS_Telport_Move,
		BS_Action,
		BS_End
	};

protected:
	Buff_State m_BuffState;
	bool m_bWound;
	DWORD m_dwMotionEndTime;
	ioHashString m_szTargetName;
	float m_fCurMoveDist;

	float m_fCurTargetYAngle;
	float m_fCurTargetRange;
	float m_fCurMoveSpeed;

	DWORD m_dwEnableAttackTime;

protected:
	ioHashStringVec m_RemoveBuffList;
	IntVec m_EnableInputKeyState;
	IntVec m_BuffEndState;
	float m_fEndJumpPower;

	AttackAttribute m_NormalMove;
	AttackAttribute m_JumpMove;

	float m_fNormalTargetYAngle;
	float m_fNormalTargetRange;
	float m_fNormalMoveSpeed;

	float m_fJumpTargetYAngle;
	float m_fJumpTargetRange;
	float m_fJumpMoveSpeed;

	float m_fMaxMoveDist;

	int m_nAttachWeapon;

	AttackAttribute m_NormalAttack;
	AttackAttribute m_JumpAttack;

	bool m_bEnableWeaponSkill;
	bool m_bEnableArmorSkill;
	bool m_bEnableHelmetSkill;
	bool m_bEnableCloakSkill;

	float m_fUseMoveGauge;
	float m_fFloatingGap;

	DWORD m_dwAttackTime;

	float m_fGapHeight;

	bool m_bTeleportMove;
	D3DXVECTOR3 m_vTargetTeleportOffset;
	D3DXVECTOR3 m_vNoTargetTeleportOffset;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void Init();
	bool CheckWoundState();
	bool CheckEnableKey( ioBaseChar *pOwner );
	bool CheckBuffEndState( ioBaseChar *pOwner );

protected:
	void SetMoveState( ioBaseChar *pOwner );
	void SetTeleportMoveState( ioBaseChar *pOwner );
	void SetActionState( ioBaseChar *pOwner, bool bSendNetwork );
	void SetEndState( ioBaseChar *pOwner );
	
protected:
	bool FindTarget( ioBaseChar *pOwner, bool bFloating );
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList, ioHashString &szTargetName, bool bFloating );

protected:
	void CreateAttachWeapon( ioBaseChar *pOwner );
	
protected:
	bool ProcessMove( ioBaseChar *pOwner, float fMoveSpeed, D3DXVECTOR3 vMoveDir );

public:
	bool IsCharCollisionSkipState();
	void ApplyActionState( ioBaseChar *pWoundChar );
	bool CheckEnableUseSkill( int iSlot );

public:
	ioTargetMoveAttackBuff(void);
	ioTargetMoveAttackBuff( const ioTargetMoveAttackBuff &rhs );
	~ioTargetMoveAttackBuff(void);
};

inline ioTargetMoveAttackBuff* ToTargetMoveAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TARGET_MOVE_ATTACK )
		return NULL;

	return dynamic_cast<ioTargetMoveAttackBuff*>( pBuff );
}