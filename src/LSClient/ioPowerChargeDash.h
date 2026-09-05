#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioPowerChargeDash : public ioExtendDash
{
public:
	enum ChargeDashState
	{
		CDS_NONE,
		CDS_CHARGING,
		CDS_NORMAL_ATTACK,
		CDS_EXTEND_ATTACK,
		CDS_TITAN,
		CDS_TITAN_MOVE_FAIL,
		CDS_TITAN_MOVE_STATE,
		CDS_TITAN_MOVE_RESERVED,
		CDS_TITAN_MOVE_ANI,
	};

	enum DashExtendType
	{
		DET_NONE,
		DET_TITAN,
		DET_NINJA_EXPANSION,
		DET_GEOMJE,
		DET_MIR_WARRIOR,
		DET_KUNGFU,
		DET_MAGICSTAFF,
		DET_WILDBOXER,
		DET_HAWKEYE_EXPANSION,
		DET_SMILE_JOKER,
		DET_SALADIN,
		DET_SKY_WALKER,
	};

protected:
	ChargeDashState m_DashState;
	DashExtendType  m_DashExtendType;

	AttackAttribute m_AttackAttribute;
	AttackAttribute m_ExtendAttackAttribute;

	DWORD m_dwChargingTime;
	DWORD m_dwChargingStartTime;

	ioHashString m_szChargingAni;

	CEncrypt<float> m_fChargingSpeedRate;

	DWORD m_dwFireStartTime;
	DWORD m_dwCreateWeaponIndex;
	bool m_bDashCancelEnable;

	// 대시 D 캔슬 D~ 가능 시간
	DWORD m_dwEnableChargingDashAttackTime;
	// 대시 D 캔슬 D~ 차징 시간
	DWORD m_dwChargingCancelChargeAttackStartTime;
	DWORD m_dwCancelChargingTime;
	// 대시 D 캔슬 가능 시간 텀
	DWORD m_dwEnableDashAttackTime;

	// D~ 방향
	ioUserKeyInput::DirKeyInput	m_eSpecialDashCurrDirkey;

protected:
	// 타이탄
	TitanExtendInfo m_DashTitanExtend;
	DWORD			m_dwExtendMoveSlidingTime;

protected:
	//진화 진자
	DWORD			m_dwEnableNinjaDashTime;

protected:
	bool m_bExtraOtherDashAttack;

	// 스카이워커
protected:
	// 대시 중 캔슬(공격, 점프) 가능한 시간
	DWORD m_dwDashCancelActionTime;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsEnableExtraDashAttack();
	virtual bool IsEnableExtraOtherDashAttack();

public:
	void SetPowerDash( ioBaseChar *pOwner );
	bool CheckPowerDash( ioBaseChar *pOwner );
	void ProcessExtraDash( ioBaseChar *pOwner );
	void ProcessExtraDashByNoNeedProcess( ioBaseChar *pOwner );

	bool IsPowerChargingState();

	void PowerDashEnd( ioBaseChar *pOwner );

	inline float GetChargingSpeedRate() const { return m_fChargingSpeedRate; }


	int GetCurState() { return m_DashState; }

protected:
	void ProcessCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToExtendAttack( ioBaseChar *pOwner, bool bChainExtendAttack );

public:
	ioPowerChargeDash();
	ioPowerChargeDash( const ioPowerChargeDash &rhs );
	virtual ~ioPowerChargeDash();

	//타이탄
protected:
	void LoadTitanExtendInfo( ioINILoader &rkLoader );
	bool CheckIsEnableTitanExtendAttack( ioBaseChar *pOwner );
	void SetTitanExtendAttack( ioBaseChar *pOwner );
	void ProcessTitanExtendAttack(  ioBaseChar *pOwner );
	void SetTitanExtendMoveAni( ioBaseChar *pOwner );
	void ProcessTitanExtendMoveAni( ioBaseChar *pOwner );
	void TitanExtendMoveFail( ioBaseChar *pOwner, bool bSend );
	D3DXVECTOR3	CheckTitanExtendDir( ioBaseChar *pOwner );

	bool CheckIsEnableWildBoxerExtendAttack( ioBaseChar *pOwner );
	void SetWildBoxerExtendAttack( ioBaseChar *pOwner );

	void SetHawkEyeExtendAttack( ioBaseChar *pOwner );

public:
	bool IsTitanExtendMoveState();

	//진화 닌자
public:
	bool CheckDbKey( ioBaseChar *pOwner );
	virtual bool IsEnableDashFromDashAttack( ioBaseChar *pOwner );
	void SetExtraDashAttackInfo( ioBaseChar *pOwner, int nAni, float fTimeRate, DWORD dwPreDelay, int iCurCombo );

	//미르 전사
public:
	bool CheckIsEnableMirExtendAttack( ioBaseChar *pOwner );
	void SetMirExtendAttack( ioBaseChar *pOwner );

public:
	bool CheckIsEnableSmileJokerAttack( ioBaseChar *pOwner );
	void SetSmileJokerAttack( ioBaseChar *pOwner );
	// 스카이 워커 D~ 방향 정하는 함수
	void CheckCancelReserveByConditionalSkyWalkerSpecialDash( ioBaseChar *pOwner );

protected:
	ioHashString	m_stAniJump;
	float			m_fAniJumpRate;
	DWORD			m_dwActionStartTime;
	DWORD			m_dwActionEndTime;
	bool CheckAniJump_Input_Jump_S( ioBaseChar *pOwner );
	bool CheckCancelBySpeicalDash( ioBaseChar *pOwner );
	bool CheckCancel( ioBaseChar *pOwner );
};

inline ioPowerChargeDash* ToPowerChargeDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_POWER_CHARGE )
		return NULL;

	return dynamic_cast< ioPowerChargeDash* >( pDash );
}

