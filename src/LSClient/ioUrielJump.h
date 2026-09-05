#pragma once
#include "ioextendjump.h"

class ioUrielJump :	public ioExtendJump
{
protected:
	enum UrielJumpState
	{
		UJS_NONE,
		UJS_DOUBLE_JUMP,
		UJS_CHARGE,
		UJS_NORMAL_ATTACK,
		UJS_CHARGE_ATTACK,
		UJS_CHARGE_ATTACK_END_DELAY,
		UJS_CHARGE_ADD_ATTACK,
		UJS_S_HOLD,
		UJS_DASH,
	};

	enum SyncType
	{
		ST_CHARGE,
		ST_NORMAL_ATTACK,
		ST_DOUBLE_JUMP,
		ST_S_HOLD,
		ST_DASH,
		ST_CHARGE_ATTACK,
		ST_CHARGE_ATTACK_END_DELAY,
		ST_CHARGE_ADD_ATTACK,
		ST_CHARGE_ADD_ATTACK_FIRE,
		ST_SET_TARGET,
	};
protected:
	UrielJumpState m_UrielJumpState;

	bool			m_bReserveJumpAttack;

	int				m_iMaxJumpCnt;
	int				m_iCurJumpCnt;

	//jump attack
	int				m_iCurComboCnt;
	DWORD			m_dwEnableComboTime_S;
	DWORD			m_dwEnableComboTime_E;
	AttributeList	m_ComboAttackList;

	//Charge
	ioHashStringVec m_AttackKeyChargingAniList;
	DWORD			m_dwChargingTime;
	DWORD			m_dwChargingStartTime;

	//Duble Jump
	ioHashStringVec m_vDoubleJumpAniList;
	float			m_fDoubleJumpAniRate;
	bool			m_bUsedDoubleJump;
	float			m_fDoubleJumpPower;
	float			m_fDoubleJumpForce;
	float			m_fDoubleJumpEnableHeight;
	DWORD			m_dwDoubleJumpEnableTime;
	float			m_fJumpSpeedRateFR;
	float			m_fJumpSpeedRateSI;
	float			m_fJumpSpeedRateBG;

	float			m_fDoubleJumpSpeedRateFR;
	float			m_fDoubleJumpSpeedRateSI;
	float			m_fDoubleJumpSpeedRateBG;

	//S Key State
	ioHashString	m_szS_HoldAni;
	float			m_fS_HoldAniRate;
	bool			m_bUsedS_HoldAni;
	DWORD			m_dwS_HoldEnableTime;
	float			m_fS_HoldEnableHeight;
	DWORD			m_dwMotionEndTime;
	float			m_fS_HoldEndJumpPowerRate;
	DWORD			m_dwS_HoldKeyProtectTime;

	DWORD			m_dwS_HoldKeyProtectEndTime;

	//Dash
	AttackAttribute	m_vJumpDashAttack;
	int				m_iMaxJumpDashCount;
	int				m_iCurJumpDashCount;
	float			m_fJumpDashEndJumpPowerRate;

	//D~
	ioHashString	m_szJumpChargeAttackAni;
	float			m_fJumpChargeAttackAniRate;
	float			m_fJumpChargeAttackAngle;
	float			m_fMaxJumpChargeAttackRange;
	int				m_iJumpChargeWeaponType;
	float			m_fJumpChargeAttackEndJumpPowerRate;
	float			m_fJumpChargeAttackSpeed;
	AttackAttribute	m_JumpChargeAddAttack;
	DWORD			m_dwJumpChargeEndDelay;

	ioHashString	m_szTargetName;
	DWORD			m_dwJumpChargeWeaponIndex;
	float			m_fCurJumpChargeAttackRange;
	D3DXVECTOR3		m_vJumpChargeAttackMoveDir;
	DWORD			m_dwAttackFireTime;
	DWORD			m_dwJumpChargeEndDelayEndTime;
protected:
	void ClearData();
protected:	//double jump
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJump( ioBaseChar *pOwner );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

protected:	//D
	void SetChargingState( ioBaseChar *pOwner );
	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	void SetComboJumpAttack( ioBaseChar *pOwner );
	

protected:
	//S Key State
	bool IsEnableS_HoldState( ioBaseChar *pOwner, float fHeightGap );
	void SetS_HoldState( ioBaseChar *pOwner );
	void ProcessS_HoldState( ioBaseChar *pOwner );
	void SetS_HoldEnd( ioBaseChar *pOwner );

	//Dash
	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	void SetJumpDashState( ioBaseChar *pOwner );
	void ProcessJumpDashState( ioBaseChar *pOwner );

	//D~
	void SetChargeAttackState( ioBaseChar *pOwner );
	void SetJumpChargeAddAttack( ioBaseChar *pOwner );
	void SetJumpChargeEndDelayState( ioBaseChar *pOwner );
	void ProcessChargeAttackState( ioBaseChar *pOwner );
	void ProcessChargeAddAttackState( ioBaseChar *pOwner );
	void ProcessChargeAttackEndDelayState( ioBaseChar *pOwner );
	bool IsEnableJumpChargeAddAttack( ioBaseChar *pOwner );
	bool IsCanChargeAttackState( ioBaseChar *pOwner );
	void JumpChargeAddAttackFire( ioBaseChar *pOwner );
	bool CheckChargeWeaponLive( ioBaseChar *pOwner );
	void ClearChargeWeapon( ioBaseChar *pOwner );
	

	//Gravity
public:
	float GetCurGravityAmt( int iJumpState );
	bool IsCanMoveState( int iJumpState );

	bool IsChargeAttacked();
	
public:
	void ClearState( ioBaseChar *pOwner, int eNewState );

public:
	inline bool IsJumpAttackReserve() { return m_bReserveJumpAttack; }
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

	virtual void InitJump();

	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );
	virtual void SetNextJumpItemState( ioBaseChar *pOwner, ioBaseChar *pWounder, bool bDefense, ChangeNextItemType type, ioWeapon* pWeapon, float fDamage );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void SetLandingState( ioBaseChar *pOwner );
public:
	ioUrielJump(void);
	ioUrielJump( const ioUrielJump &rhs );
	virtual ~ioUrielJump(void);
};

inline ioUrielJump* ToUrielJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_URIEL_JUMP )
		return NULL;

	return dynamic_cast< ioUrielJump* >( pJump );
}