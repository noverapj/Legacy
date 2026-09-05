#pragma once

#include "ioCounterAttack.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioExtraCounter : public ioCounterAttack
{
protected:
	AttributeList m_ExtraAttackList;
	int m_iCurExtraCnt;

	int m_iSkillInput;
	bool m_bEnableDefenseState;
	bool m_bEnableAttackState;
	bool m_bEnableJumpState;

	DWORD m_dwActionEndTime;
	DWORD m_dwReserveEnableTime;

	bool m_bSetExtendCAT;

	bool			m_bUseExtendJumpKeyCA;
protected:
	// For Ani Rotate
	vAniRotateInfoList m_vAniRotateInfoList;

	int m_iCurAniRotate;
	DWORD m_dwAniRotateTime;
	float m_fAniRotateAngle;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual CounterAttackType GetType() const;
	virtual ioCounterAttack* Clone();

public:
	virtual bool StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState );
	virtual void ProcessCAT( ioBaseChar *pOwner );

	virtual void ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	void CheckAniRotate( ioBaseChar *pChar );

protected:
	bool StartAttackKeyCAT( ioBaseChar *pOwner );
	bool StartJumpKeyCAT( ioBaseChar *pOwner );
	bool StartJumpKeyDefaultCAT( ioBaseChar *pOwner );
	bool StartJumpKeyExtendCAT( ioBaseChar *pOwner );

	void SetExtendCAT( ioBaseChar *pOwner );

	void CheckReserve( ioBaseChar *pOwner );
	void CheckExtendReserve( ioBaseChar *pOwner );

public:
	ioExtraCounter();
	ioExtraCounter( const ioExtraCounter &rhs );
	virtual ~ioExtraCounter();
};

