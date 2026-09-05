#pragma once
#include "ioextendjump.h"

class ioSkyWalkerJump :	public ioExtendJump
{
protected:
	enum SkyWalkerJumpState
	{
		SWJS_NONE,
		SWJS_DOUBLE_JUMP,
		SWJS_LOW_JUMP_HEIGHT,
		SWJS_CHARGE,
		SWJS_NORMAL_ATTACK,
		SWJS_CHARGE_ATTACK,
		SWJS_CHARGE_ATTACK_END_DELAY,
		SWJS_DASH,
	};

	enum SyncType
	{
		ST_CHARGE,
		ST_NORMAL_ATTACK,
		ST_DOUBLE_JUMP,
		ST_DASH,
		ST_DASH_LOW_HEIGHT_END,
		ST_CHARGE_ATTACK,
		ST_CHARGE_ATTACK_END_DELAY,
		ST_SET_TARGET,
	};

	enum DashDirType
	{
		DDT_FIRST_TYPE,
		DDT_SECOND_TYPE
	};
protected:
	SkyWalkerJumpState m_SkyWalkerJumpState;

	bool			m_bReserveJumpAttack;

	// 점프 D~ 시 앤드 점핑 후 추가 점프 횟수
	bool			m_bEnableChargeAttAfterSeveralJump;

	int				m_iMaxChargeAttAfterJumpCnt;
	int				m_iMaxJumpCnt;
	int				m_iCurJumpCnt;

	//jump attack
	int				m_iCurComboCnt;
	DWORD			m_dwEnableComboTime_S;
	DWORD			m_dwEnableComboTime_E;
	DWORD			m_dwEnableChargeAttack;
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

	DWORD			m_dwMotionEndTime;

	//Dash
	// 대각선 대시 시 x축 회전 앵글 값
	float m_fJumpAttackDiagonalDashAngle;
	// 상하 대시 시 x축 회전 앵글 값
	float m_fJumpAttackTopDashAngle;
	// 직진 대시
	AttackAttribute	m_vJumpStraightDashAttack;
	// 상단 대각선 대시
	AttackAttribute	m_vJumpTopDiagonalDashAttack;
	// 상단 대시
	AttackAttribute	m_vJumpTopDashAttack;
	// 하단 대각선 대시
	AttackAttribute	m_vJumpBottomDiagonalDashAttack;
	// 하단 대시
	AttackAttribute	m_vJumpBottomDashAttack;
	float m_fJumpAttackBottomDashAngle;

	ioUserKeyInput::DirKeyInput m_ePreInputDirKey;
	ioUserKeyInput::DirKeyInput m_eCurInputDirKey;

	int m_iSameJumpDashDirCount;
	// 점프 대시 같은 방향 연속 대시 제한 최대 횟수
	int m_iSameJumpDashLimitCount;
	
	// 점프 아래쪽 대시 직 후 맵 높이 Y값과 용병 위치 Y값의 차 값이 아래 변수 값이 이하일 경우 앤드 점프하지 않음
	float m_fInactiveEndJumpDistance;

	// 점프 대시 사용 시 소모되는 스카이워커 게이지 값
	float	m_fJumpDashUseGauge;

	struct DirectionInfo
	{
		DashDirType		m_eDashDirType;
		D3DXVECTOR3 m_vDir;

		DirectionInfo( DashDirType eDashDirType, D3DXVECTOR3 vDir ) : m_eDashDirType( eDashDirType ), m_vDir( vDir )
		{}
	};
	typedef std::vector< DirectionInfo > DirInfoVec;

	DashDirType		m_eAttDirType;

	DirInfoVec		m_vDirInfo;

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
	DWORD			m_dwJumpChargeEndDelay;

	// 점프 D~ 사용 시 소모되는 스카이워커 게이지 값
	float			m_fJumpChargeAttackUseGauge;

	ioHashString	m_szTargetName;
	DWORD			m_dwJumpChargeWeaponIndex;
	float			m_fCurJumpChargeAttackRange;
	D3DXVECTOR3		m_vJumpChargeAttackMoveDir;
	DWORD			m_dwAttackFireTime;
	DWORD			m_dwJumpChargeEndTime;
	DWORD			m_dwJumpChargeEndDelayEndTime;
	
	// 점프 D~ 공격 시 용병 위치가 이동하지 않으면 
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
	//Dash
	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	void SetJumpDashState( ioBaseChar *pOwner, bool bSend = true, D3DXVECTOR3 vRecvCharDir = ioMath::VEC3_ZERO );
	void ProcessJumpDashState( ioBaseChar *pOwner );

	//D~
	void SetChargeAttackState( ioBaseChar *pOwner );
	void SetJumpChargeEndDelayState( ioBaseChar *pOwner );
	void ProcessChargeAttackState( ioBaseChar *pOwner );
	void ProcessChargeAddAttackState( ioBaseChar *pOwner );
	void ProcessChargeAttackEndDelayState( ioBaseChar *pOwner );
	bool IsCanChargeAttackState( ioBaseChar *pOwner );
	bool CheckChargeWeaponLive( ioBaseChar *pOwner );
	void ClearChargeWeapon( ioBaseChar *pOwner );


	//Gravity
public:
	float GetCurGravityAmt( int iJumpState );
	bool IsCanMoveState( int iJumpState );

	bool IsChargeAttacked();
	bool IsDashAttacked();

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
	virtual void SetJumpState();
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );
	virtual void SetNextJumpItemState( ioBaseChar *pOwner, ioBaseChar *pWounder, bool bDefense, ChangeNextItemType type, ioWeapon* pWeapon, float fDamage );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void SetLandingState( ioBaseChar *pOwner );

	float CalculateAngle( D3DXVECTOR3 v1, D3DXVECTOR3 v2 );
	ioSkyWalkerJump::DashDirType CheckAttackDirByAngle( ioBaseChar *pOwner, D3DXVECTOR3 &vDir );
public:
	ioSkyWalkerJump(void);
	ioSkyWalkerJump( const ioSkyWalkerJump &rhs );
	virtual ~ioSkyWalkerJump(void);
};

inline ioSkyWalkerJump* ToSkyWalkerJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_SKY_WALKER_JUMP )
		return NULL;

	return dynamic_cast< ioSkyWalkerJump* >( pJump );
}