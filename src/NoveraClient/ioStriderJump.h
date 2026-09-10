#pragma once
#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioStriderJump : public ioExtendJump
{
protected:
	enum StriderJumpState
	{
		SJS_NONE,
		SJS_DOUBLE_JUMP,
		SJS_CHARGE,
		SJS_NORMAL_ATTACK,
		SJS_DASH_ATTACK,
	};
protected:
	StriderJumpState	m_StriderJumpState;

	//Attack
	AttributeList		m_ComboAttackList;
	int					m_iCurComboCnt;
	bool				m_bReserveJumpAttack;
	DWORD				m_dwEnableComboTime_S;
	DWORD				m_dwEnableComboTime_E;

	//AttackKey Charge
	ioHashStringVec		m_ChargingAniList;
	DWORD				m_dwChargingStartTime;
	DWORD				m_dwChargingTime;

	//DoubleJump
	int					m_iMaxJumpCnt;
	int					m_iCurJumpCnt;
	ioHashStringVec		m_vDoubleJumpAniList;
	float				m_fDoubleJumpAniRate;
	bool				m_bUsedDoubleJump;
	float				m_fDoubleJumpPower;
	float				m_fDoubleJumpForce;
	float				m_fDoubleJumpEnableHeight;
	DWORD				m_dwDoubleJumpEnableTime;

	//Dash
	bool				m_bEnableJumpDash;
	AttackAttribute		m_DashJumpAttack;

	DWORD				m_dwInputSpecialDashTime;

	float				m_fDashJumpAttackGravity;
protected:
	void ClearData();

	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	bool IsCanNewJumpDashAddAttack( ioBaseChar *pOwner );

	void SetDoubleJump( ioBaseChar *pOwner );
	void SetDoubleJumpAni( ioBaseChar *pOwner );
	void SetChargingState( ioBaseChar *pOwner );
	void SetComboJumpAttack( ioBaseChar *pOwner );
	void SetStriderChargeAttack( ioBaseChar *pOwner );
	void SetNewJumpDashAttack( ioBaseChar *pOwner );
	void SetNewJumpDashAddAttack( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	void ProcessNewJumpDashAttack( ioBaseChar *pOwner );
public:
	void SetJumpComboAttack( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;
public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );

	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );
	bool IsNewJumpDashAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
public:
	ioStriderJump(void);
	ioStriderJump( const ioStriderJump &rhs );
	virtual ~ioStriderJump(void);
};

inline ioStriderJump* ToStriderJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_STRIDER_JUMP )
		return NULL;

	return dynamic_cast< ioStriderJump* >( pJump );
}
