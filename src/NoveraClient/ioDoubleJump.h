#pragma once

#include "ioExtendJump.h"
#include "ioAttackableItem.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioDoubleJump : public ioExtendJump
{
public:
	enum DoubleJumpState
	{
		DJS_NONE,
		DJS_DOUBLE,
		DJS_DASH,
		DJS_ELEMENT_ATTACK,
	};

protected:
	DoubleJumpState m_CurJumpState;

	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpSpeedRateFR;
	float m_fDoubleJumpSpeedRateSI;
	float m_fDoubleJumpSpeedRateBG;

	float m_fDoubleJumpAniRate;
	ioHashStringVec m_vDoubleJumpAniList;

	bool m_bUsedDoubleJump;
	bool m_bUseLandJumpAttack;
	bool m_bUsedHighJump;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;
	CEncrypt<float> m_fGravityAmt;
	
	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;

// Dash JumpAttack
protected:
	bool m_bEnableJumpDash;
	AttackAttribute m_DashJumpAttack;
	DWORD m_dwActionEndTime;

	bool m_bDashEnd;
	bool m_bJumpAttackReserve;

// element att
protected:
	CEncrypt<bool> m_bUseJumpElementAtt;
	MagicElementInfo m_MagicElementAtt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual void SetLandingState( ioBaseChar *pOwner );

	virtual bool IsCanJumpAttack( ioBaseChar *pOwner );
	virtual DWORD GetChargingTime( ioBaseChar *pChar );

public:
	bool IsUseLandJumpAttack();
	float GetCurGravityAmt();

	bool IsJumpDashState();
	bool IsJumpAttackReserve();
	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );

	int GetCurJumpState();

protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	void ProcessJumpDash( ioBaseChar *pOwner );
	
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

	void ChangeToElementAttack( ioBaseChar *pOwner, int iSubType );

public:
	ioDoubleJump();
	ioDoubleJump( const ioDoubleJump &rhs );
	virtual ~ioDoubleJump();
};

inline ioDoubleJump* ToDoubleJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_DOUBLE_JUMP )
		return NULL;

	return dynamic_cast< ioDoubleJump* >( pJump );
}

