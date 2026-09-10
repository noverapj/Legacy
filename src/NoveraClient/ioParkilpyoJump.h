#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioParkilpyoJump : public ioExtendJump
{
public:
	enum ChageComboState
	{
		CCS_NONE,
		CCS_NORMAL,
		CCS_CHARGE,
		CCS_GATHER,
		CCS_CHARGE_ATTACK,
		CCS_EQUIP_OBJECT,
		CCS_DOUBLE,
	};

protected:
	DWORD m_dwChargingTime;
	ioHashString m_szChargingAni;
	DWORD m_dwGatherTime;
	ioHashString m_szGatherAni;	
	AttributeList m_ComboAttackList;

	float m_fJumpSpeedRateFR;
	float m_fJumpSpeedRateSI;
	float m_fJumpSpeedRateBG;

	int m_nMaxJumpCnt;

	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	ioHashStringVec m_vDoubleJumpAniList;
	float m_fDoubleJumpAniRate;

	float m_fDoubleJumpSpeedRateFR;
	float m_fDoubleJumpSpeedRateSI;
	float m_fDoubleJumpSpeedRateBG;

	AttackAttribute m_ExtendLandAttack;

	bool m_bApplyGravity;

protected:
	ChageComboState m_ChargeComboState;

	DWORD m_dwChargingStartTime;
	DWORD m_dwGatherStartTime;

	int m_nCurComboCnt;
	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;
	bool m_bReserveJumpAttack;
	
	int m_nCurJumpCnt;
	bool m_bUsedDoubleJump;
	bool m_bUsedChargeAttack;

public:
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	
	virtual void SetLandingState( ioBaseChar *pOwner );
	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	
	bool IsNoProcessGravity();
	
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

protected:
	void SetChargingState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner );
	void ChangeToEquipObject( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessGathering( ioBaseChar *pOwner );
	void ProcessChargeAttack( ioBaseChar *pOwner );
	void ProcessEquipObject( ioBaseChar *pOwner );
	void ProcessAirJump( ioBaseChar *pOwner );

protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJump( ioBaseChar *pOwner );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

public:
	bool IsChargeAttackState();
	inline const ioHashString& GetChargeLandAni() const { return m_ExtendLandAttack.m_AttackAnimation; }
	inline float GetChargeLandAniRate() const { return m_ExtendLandAttack.m_fAttackAniRate; }
	const AttackAttribute& GetLandAttackAttribute() const  { return m_ExtendLandAttack; }

public:
	ioParkilpyoJump();
	ioParkilpyoJump( const ioParkilpyoJump &rhs );
	virtual ~ioParkilpyoJump();
};

inline ioParkilpyoJump* ToParkilpyoJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_PARKILPYO_JUMP )
		return NULL;

	return dynamic_cast< ioParkilpyoJump* >( pJump );
}

