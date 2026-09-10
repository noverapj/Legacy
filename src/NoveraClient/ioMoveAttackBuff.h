#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioMoveAttackBuff : public ioBuff
{
protected:
	enum BuffState
	{
		BS_Delay,
		BS_Attack,
		BS_End,
	};

protected:
	BuffState m_BuffState;

	DWORD m_dwMotionEndTime;
	int m_nCurWeaponCnt;
	
	DWORD m_dwWeaponFireTime;
	bool m_bWound;

protected:
	CEncrypt<bool> m_bEnableWeaponSkill;
	CEncrypt<bool> m_bEnableArmorSkill;
	CEncrypt<bool> m_bEnableHelmetSkill;
	CEncrypt<bool> m_bEnableCloakSkill;
	bool m_bDisableDownState;

	ioHashStringVec m_RemoveBuffList;

	int m_nTotalWeaponCnt;

	float m_fStartJumpPower;
	float m_fEndJumpPower;
	
	DWORD m_dwCameraEvent;
	ioHashString m_szCameraBuff;

	IntVec m_EnableInputKeyState;
	IntVec m_BuffEndState;

	AttackAttribute m_NormalAttack;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

	bool CheckEnableUseSkill( int iSlot );

protected:
	void Init();
	void LoadExtraProperty( ioINILoader &rkLoader );
	bool CheckWoundState( ioBaseChar *pOwner );

protected:
	bool CheckEnableKey( ioBaseChar *pOwner );
	bool CheckBuffEndState( ioBaseChar *pOwner );
	void CheckInputKey( ioBaseChar *pOwner );
	bool CheckAttackKey( ioBaseChar *pOwner );

protected:
	void SetAttackState( ioBaseChar *pOwner );
	
protected:
	void SetCameraBuff( ioBaseChar *pOwner );
	void EndCameraBuff( ioBaseChar *pOwner );
	void SetCameraEvent( ioBaseChar *pOwner );
	void EndCameraEvent( ioBaseChar *pOwner );

public:
	ioMoveAttackBuff();
	ioMoveAttackBuff( const ioMoveAttackBuff &rhs );
	virtual ~ioMoveAttackBuff();
};

inline ioMoveAttackBuff* ToMoveAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_MOVE_ATTACK )
		return NULL;

	return dynamic_cast< ioMoveAttackBuff* >( pBuff );
}