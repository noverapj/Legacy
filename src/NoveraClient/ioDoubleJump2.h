#pragma once

#include "ioExtendJump.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioDoubleJump2 : public ioExtendJump
{
public:
	enum ProcessJumpState
	{
		PJS_NONE,
		PJS_NORMAL_ATTACK,
		PJS_END,
	};

	enum SyncState
	{
		SSC_ATTACK,
		SSC_DASH,
		SSC_DOUBLE_JUMP,
	};

	CEncrypt<bool> m_bEnableHighJump;

protected:
	ProcessJumpState m_JumpState;

	AttributeList m_ComboAttackList;
	int m_iCurComboCnt;

	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;
	DWORD m_dwMotionEndTime;

	float m_fDoubleJumpSpeedRate;

	float m_fExtendJumpSpeedRateFR;
	float m_fExtendJumpSpeedRateSI;
	float m_fExtendJumpSpeedRateBG;

	bool m_bReserveJumpAttack;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;
	DWORD m_dwChargingStartTime;

	CEncrypt<float> m_fJumpAttackEndJumpAmt;

	// DoubleJump
protected:
	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpAniRate;
	ioHashString m_DoubleJumpAni;

	bool m_bUsedDoubleJump;

	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;

protected:
	TargetWoundType	m_TargetWoundType;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetOffset;
	CEncrypt<float> m_fDownTargetOffset;
	ioHashString    m_TargetName;

protected:
	AttributeList m_ExternalComboAttackList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );
	
	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual bool IsEnableJumpLandEndDash();

	float GetCurGravityAmt();

	virtual void SetLandingState( ioBaseChar *pOwner );

protected:
	void SetComboJumpAttack( ioBaseChar *pOwner );
	void LoadComboList( ioINILoader &rkLoader );

	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

protected:
	void ChangeToJumpAttack( ioBaseChar *pOwner );

protected:
	void CheckTarget( ioBaseChar *pOwner );

public:
	D3DXVECTOR3 GetFireDir( ioBaseChar *pOwner );

public:
	void AddExternalComboAttack( const AttackAttribute* ExternalComboAttack );
	void ClearExternalComboAttackList();
	bool IsExternalComboAttackList();

public:
	ioDoubleJump2();
	ioDoubleJump2( const ioDoubleJump2 &rhs );
	virtual ~ioDoubleJump2();
};

inline ioDoubleJump2* ToDoubleJump2( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_DOUBLE_JUMP2 )
		return NULL;

	return dynamic_cast< ioDoubleJump2* >( pJump );
}

