#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargeComboJump : public ioExtendJump
{
public:
	enum ExtendJumpType
	{
		EJT_NONE,
		EJT_VALKYRIE,
		EJT_MUTANTE,
		EJT_WITCH,
		EJT_SLOPE,
		EJT_MIDNIGHT_SCYTHE,
		EJT_MIDNIGHT_GUN,
		EJT_LILLY
	};

	enum ChageComboState
	{
		CCS_NONE,
		CCS_NORMAL,
		CCS_CHARGE,
		CCS_GATHER,
		CCS_GATHER_ATTACK,
		CCS_GATHER_FAIL,
		CCS_DOUBLE,
		CCS_DELAY,
		CCS_FLASH,
		CCS_FLASH_END,
		CCS_DASH,
		CCS_SET_UROBORUS,		// 순수 동기화용
		CCS_MUTANT_MOVE_STATE,
		CCS_WITCH_DASH,
	};
	
	enum DoubleJumpSync
	{
		DJS_DEFAULT,
		DJS_DOUBLE_JUMP_FLY,
	};

protected:
	ExtendJumpType	m_eExtendJumpType;
	ChageComboState m_ChargeComboState;

	float m_fJumpSpeedRateFR;
	float m_fJumpSpeedRateSI;
	float m_fJumpSpeedRateBG;

	bool m_bUsedComboAttack;
	bool m_bUsedDoubleJump;
	bool m_bUsedFlash;
	bool m_bUsedGatherAttack;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

	// NormalJump Attack
protected:
	AttributeList m_ComboAttackList;
	int m_iCurComboCnt;

	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;

	bool m_bReserveJumpAttack;

	// GatherJump Attack
protected:
	CEncrypt<bool> m_bEnableGatherJump;

	DWORD m_dwChargingStartTime;
	DWORD m_dwChargingTime;
	bool m_bDsiableCharging;
	ioHashString m_szChargingAni;

	DWORD m_dwGatherStartTime;
	DWORD m_dwGatherTime;
	ioHashString m_szGatherAni;

	// uroborus
protected:
	DWORD m_dwCreateWeaponIndex;
	DWORD m_dwFireStartTime;

	ioHashString m_UroborusGuidEffect;
	UniqueObjID m_GuidEffectID;

	ioHashString m_UpUroborusAni;
	ioHashString m_DownUroborusAni;
	float m_fUpUroborusAniRate;
	float m_fDownUroborusAniRate;

	CEncrypt<float> m_fUroborusMoveSpeed;
	CEncrypt<float> m_fUroborusMoveRate;
	CEncrypt<float> m_fUroborusMoveAngle;

	CEncrypt<float> m_fUroborusMinAngle;
	CEncrypt<float> m_fUroborusMaxAngle;
	CEncrypt<float> m_fUroborusCurAngle;

	CEncrypt<float> m_fUroborusEndJumpAmt;

	DWORD m_dwUroborusAngleTime;

	// FlashJump Attack
protected:
	CEncrypt<bool> m_bEnableFlashJump;

	ioHashString m_FlashDelayAni;
	CEncrypt<float> m_fFlashDelayRate;

	CEncrypt<float> m_fFlashForce;
	CEncrypt<float> m_fFlashGravityAmt;

	DWORD m_dwFlashDelayEndTime;
	DWORD m_dwFlashEndTime;

	AttackAttribute m_FlashJumpAttack;

	// DoubleJump
protected:
	CEncrypt<bool> m_bEnableDoubleJump;
	bool m_bNewJumpAttackState;
	bool m_bUseWitchDash;

	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpAniRate;
	ioHashStringVec m_vDoubleJumpAniList;

	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;

	float m_fDoubleJumpSpeedRateFR;
	float m_fDoubleJumpSpeedRateSI;
	float m_fDoubleJumpSpeedRateBG;

	// Jump Dash
protected:
	CEncrypt<bool> m_bEnableJumpDash;
	float m_fDashAngleVert;
	float m_fDashAngleHorz;

	float m_fJumpDashAniRate;
	ioHashStringVec m_vJumpDashAniList;

	ForceInfoList m_vForceInfoList;
	DWORD m_dwActionEndTime;

	bool m_bDashEnd;
	bool m_bJumpAttackReserve;
	bool m_bDashAmtDisable;
	bool m_bDashKeyReserveDisable;
	bool m_bInputDirRotate;
	bool m_bDoubleJumpDashEnable;

	float m_fDashEndJumpPower;

	//뮤턴트
protected:
	DWORD m_dwMutantDefenceKeyMaxChargeTime;
	DWORD m_dwMutantDefenceKeyDownStartTime;
	bool  m_bDefenceKeyUpedCheck;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual void SetLandingState( ioBaseChar *pOwner );
		
	bool IsNoProcessGravity();

	int GetCurChargeComboState();

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

//special
public:
	void ChangeToDoubleJumpFly( ioBaseChar *pOwner );
	void ChangeToValkyrieSpeicalState( ioBaseChar *pOwner );
	void ChangeToWitchSpeicalState( ioBaseChar *pOwner );
	void ChangeToMidnightScytheSpecialState( ioBaseChar* pOwner );
	void ChangeToMidnightGunSpecialState( ioBaseChar* pOwner );
	void ChangeToLillySpecialState( ioBaseChar* pOwner );

	// gather
protected:
	void SetChargingState( ioBaseChar *pOwner );

	void SetGatherJump( ioBaseChar *pOwner );
	void ChangeToGatherJumpAttack( ioBaseChar *pOwner );

	void SetGatherFail( ioBaseChar *pOwner, bool bSend );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessGather( ioBaseChar *pOwner );
	void ProcessGatherAttack( ioBaseChar *pOwner );

	void CheckCurUroborusAngle( ioBaseChar *pOwner );
	D3DXVECTOR3 CheckUroborusDir( ioBaseChar *pOwner );

	// Double Jump
protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJump( ioBaseChar *pOwner );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	// Flash Jump
public:
	bool SetFlashJumpAttack( ioBaseChar *pOwner );
	float GetCurFlashForceAmt();
	float GetCurFlashGravityAmt();

	bool IsCanFlashJumpAttack( ioBaseChar *pOwner );

protected:
	void ProcessDelay( ioBaseChar *pOwner );
	void ProcessFlashMove( ioBaseChar *pOwner );
	void ProcessFlashEndMove( ioBaseChar *pOwner );
	void ProcessWitchDash( ioBaseChar *pOwner );

	// Jump Dash
public:
	bool IsJumpDashState();
	bool IsJumpAttackReserve();

protected:
	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	int GetJumpDashAni( ioBaseChar *pOwner );

	void ProcessJumpDash( ioBaseChar *pOwner );
	void RotateMoveDir( ioBaseChar* pOwner, float fAngleH, float fAngleV );
	
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

	//뮤턴트
protected:
	bool CheckEnableMutantMoveState( ioBaseChar *pOwner );
	void SetMutantMoveState( ioBaseChar *pOwner );
	void LoadMutantMoveInfo( ioINILoader &rkLoader );

	bool CheckEnableSlopeState( ioBaseChar *pOwner );
	void SetSlopeState( ioBaseChar *pOwner );

	// etc
protected:
	void UpdateGuidEffect( ioBaseChar *pOwner );

public:
	virtual bool IsDoubleBuffFlyChange( ioBaseChar* pOwner );	
	virtual bool CheckUseExtendJump( ioBaseChar* pOwner );

	virtual bool IsEnableDashEndJump();

public:
	ioChargeComboJump();
	ioChargeComboJump( const ioChargeComboJump &rhs );
	virtual ~ioChargeComboJump();
};

inline ioChargeComboJump* ToChargeComboJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_CHARGE_COMBO )
		return NULL;

	return dynamic_cast< ioChargeComboJump* >( pJump );
}

