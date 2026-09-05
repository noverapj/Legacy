#pragma once

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCounterAttack
{
public:
	enum CounterAttackState
	{
		CAS_None,
		CAS_ChargeContactWeapon,
	};

protected:
	AttackAttribute m_CounterAttack;
	AttackAttribute m_JumpKeyCounterAttack;

	vAniRotateInfoList m_vAniRotateInfoList;

	int m_iCurAniRotate;
	DWORD m_dwAniRotateTime;
	float m_fAniRotateAngle;
	bool			m_bEnableDashCheck;
	DWORD			m_dwInputSpecialDashTime;
	DWORD			m_dwInputCancelTime;
	DWORD			m_dwInputCancelJumpTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual CounterAttackType GetType() const = 0;
	virtual ioCounterAttack* Clone() = 0;


public:
	virtual bool StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState );
	virtual void ProcessCAT( ioBaseChar *pOwner );

	virtual void ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void InitAniRotate();
	virtual void SetAniRotate( ioBaseChar *pChar, int iAniID, float fAniRate );
	virtual void CheckAniRotate( ioBaseChar *pChar );

public:
	virtual bool IsEnableDashCheck() { return m_bEnableDashCheck; }
	virtual void SetInputSpecialDash( ioBaseChar *pOwner, const AttackAttribute& rkAttri );
	virtual bool CheckInputSpecialDash( ioBaseChar *pOwner );
	virtual bool CheckInputCancel( ioBaseChar *pOwner );
	virtual	bool CheckInputCancelJump( ioBaseChar* pOwner );
	virtual	void SetInputCancelJump( ioBaseChar* pOwner, const AttackAttribute& rkAtt );

public:
	ioCounterAttack();
	ioCounterAttack( const ioCounterAttack &rhs );
	virtual ~ioCounterAttack();
};

