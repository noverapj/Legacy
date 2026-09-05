#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioGrapplingExpansionJump : public ioExtendJump
{
	bool				m_bUsedComboAttack;
	DWORD				m_dwJumpChargeTime;

	bool				m_bEnableHighJump;
	float				m_fExtraJumpPowerRate;
	bool				m_bUseHeighJump;

enum ChageEnumState
{
	CES_NONE,
	CES_NORMAL,
	CES_CHARGING,
	CES_CHARGE_ATTACK,
};

	// NormalJump Attack
protected:
	AttackAttribute		m_AttackAttribute;
	AttackAttribute		m_ChargeAttackAttribute;

	bool				m_bReserveJumpAttack;

	ioHashString		m_stChargingAni;
	ChageEnumState		m_charge_state;

	bool				m_bUseNormalJumpAttack;

protected:
	DWORD				m_dwChargingStartTime;
	DWORD				m_dwChargingTime;

	//charge attack
protected:
	CEncrypt<bool>		m_bUsedChargeAttack;
	CEncrypt<bool>		m_bUsedStrikerChargeAttack;

	CEncrypt<float>		m_fChargeGravityRate;
	CEncrypt<float>		m_fChargeJumpPower;
	CEncrypt<float>		m_fChargeBaseRange;
	float				m_fCurCharMoveSpeed;

	D3DXVECTOR3			m_vCharMoveDir;
	DWORD				m_dwFlightTime;

	ioHashString		m_ChargeAttackLandAni;
	float				m_fChargeAttackLandAniRate;

	ItemJumpAttackInfo	m_ItemLandAttackInfo;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessLanding( ioBaseChar *pOwner );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual void SetLandingState( ioBaseChar *pOwner );
		
	bool IsEnableLandAttack();
	bool IsUsedHighJump();

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadAttack( ioINILoader &rkLoader );
	void LoadChargeAttack( ioINILoader &rkLoader );

	void SetNormalJumpAttack( ioBaseChar *pOwner );
	void SetChargeJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void SetChargingState( ioBaseChar *pOwner );
	void ProcessCharging( ioBaseChar *pOwner );

public:
	inline float GetChargeGravityRate() const { return m_fChargeGravityRate; }

	bool IsChargeAttackState();
	bool IsJumpAttackReserve();

	inline const ioHashString& GetChargeLandAni() const { return m_ChargeAttackLandAni; }
	inline float GetChargeLandAniRate() const { return m_fChargeAttackLandAniRate; }

protected:
	void ProcessDelay( ioBaseChar *pOwner );

public:
	void SetReserveJumpAttack( ioBaseChar *pOwner );

public:
	ioGrapplingExpansionJump();
	ioGrapplingExpansionJump( const ioGrapplingExpansionJump &rhs );
	virtual ~ioGrapplingExpansionJump();
};

inline ioGrapplingExpansionJump* ToGrapplingExpansionJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_GRAPPLING_EXPANSION_JUMP )
		return NULL;

	return dynamic_cast< ioGrapplingExpansionJump* >( pJump );
}

