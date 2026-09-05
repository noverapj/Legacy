#pragma once

#include "ioExtendJump.h"

class ioNataJump : public ioExtendJump
{
protected:
	enum NataJumpState
	{
		NJS_NONE,
		NJS_NORMAL,
		NJS_D_CHARGE,
		NJS_A_CHARGE,
		NJS_DOUBLE_JUMP,
	};

protected:
	NataJumpState m_NataJumpState;

	DWORD m_dwFullJumpChargeTime;
	bool m_bReserveJumpAttack;

	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;
	
	int m_iCurComboCnt;
	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;
	AttributeList m_ComboAttackList;

	ioHashStringVec m_vDoubleJumpAniList;
	float m_fDoubleJumpAniRate;
	bool m_bUsedDoubleJump;
	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	ioHashStringVec m_AttackKeyChargingAniList;
	DWORD m_dwAttackKeyChargingTime;
	DWORD m_dwJumpKeyChargingTime;
	DWORD m_dwChargingStartTime;

	float m_fJumpSpeedRateFR;
	float m_fJumpSpeedRateSI;
	float m_fJumpSpeedRateBG;

	float m_fDoubleJumpSpeedRateFR;
	float m_fDoubleJumpSpeedRateSI;
	float m_fDoubleJumpSpeedRateBG;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

	virtual DWORD GetChargingTime( ioBaseChar *pChar );
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;

protected:
	void ClearData();
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

protected:
	void SetAttackChargingState( ioBaseChar *pOwner );
	void ProcessAttackCharging( ioBaseChar *pOwner );
	void ChangeToNataComboSpecialAttack( ioBaseChar *pOwner );
	void SetComboJumpAttack( ioBaseChar *pOwner );
	
protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJump( ioBaseChar *pOwner );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

protected:
	bool IsCanJumpCharge( ioBaseChar *pOwner, float fHeightGap );
	void SetJumpChargingState( ioBaseChar *pOwner );
	bool CheckJumpCharging( ioBaseChar *pOwner );

public:
	inline bool IsJumpAttackReserve() { return m_bReserveJumpAttack; }

public:
	ioNataJump(void);
	ioNataJump( const ioNataJump &rhs );
	virtual ~ioNataJump(void);
};

inline ioNataJump* ToNataJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_NATA_JUMP )
		return NULL;

	return dynamic_cast< ioNataJump* >( pJump );
}