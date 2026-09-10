#pragma once


class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioExtendDash
{
protected:
	float m_fDashExtraGauge;
	DWORD m_dwDashExtraTic;	

	// key reserve
	DWORD m_dwReserveEnableTime;
	DWORD m_dwReserveEnableEndTime;

	int m_iSkillInput;
	bool m_bEnableDefenseState;
	bool m_bEnableAttackState;
	bool m_bEnableJumpState;

	// bullet reload
	bool m_bReloadCheck;
	CEncrypt<int> m_iReloadCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const = 0;
	virtual ioExtendDash* Clone() = 0;

public:
	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );
	virtual void ExtendDashEnd( ioBaseChar *pOwner );

	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsEnableExtraDashAttack();
	virtual bool IsEnableExtraOtherDashAttack();
	virtual bool IsEnableDashAttackEndDash();
	virtual bool IsEnableDefense();

	virtual bool GetAutoTargetValue( IN float &fDashRange, IN float &fDashMinAngle, IN float &fDashMaxAngle,
									 OUT float &fRange, OUT float &fMinAngle, OUT float &fMaxAngle );

	virtual bool IsEnableDashFromDashAttack( ioBaseChar *pOwner );
	virtual void SetNextDashState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

protected:
	// key reserve
	virtual float CheckKeyReserveTime( ioBaseChar *pOwner, int iAniID, float fTimeRate, DWORD dwPredelay=0 );

	virtual void CheckKeyReserve( ioBaseChar *pOwner );
	virtual void ProcessKeyReserve( ioBaseChar *pOwner );

	// bullet reload
	virtual void CheckReload( ioBaseChar *pOwner );

public:
	ioExtendDash();
	ioExtendDash( const ioExtendDash &rhs );
	virtual ~ioExtendDash();
};

