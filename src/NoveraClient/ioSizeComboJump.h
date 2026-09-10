#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioSizeComboJump : public ioExtendJump
{
public:
	enum SizeComboJumpState
	{
		SCJS_NONE,
		SCJS_CHARGE,
		SCJS_NORMAL_ATTACK,
		SCJS_CHARGING_ATTACK,
		SCJS_CHARGING_END,
		SCJS_CHARGING_TARGET,
		SCJS_DASH,				// D~ 이어서 +--> 대시
		SCJS_EXTEND_CHARGE,		// D~ +D, +D~
	};

protected:
	SizeComboJumpState	m_SizeComboJumpState;		// 기본 차지 점프 상태
	DWORD				m_dwChargingTime;			// 기본 공격 차징 시간
	DWORD				m_dwChargingStartTime;		//
	AttackAttribute		m_ChargingAni;				// 기본 공격 차징 준비 애니

	// 기본 점프 공격
	bool				m_bReserveJumpAttack;		//
	int					m_iMaxComboCnt;				// 점프 공격 카운트
	int					m_iCurComboCnt;				//
	DWORD				m_dwEnableComboTime_S;		//
	DWORD				m_dwEnableComboTime_E;		//
	DWORD				m_dwActionEndTime;			//
	AttributeList		m_AttackAttributeList;		// 기본 점프 공격

	// D~
	bool				m_bScale;
	float				m_fScaleValue;				// +, -
	float				m_fExtendScaleValue;		//
	D3DXVECTOR3			m_vBeforeScale;
	bool				m_bExtendChargingAttack;	// 
	bool				m_bJumpAttackReserve;

	// extend combo D~, +-->, +D
	AttributeList		m_ExtendAttackList;

	// extend Charge combo +D~, +-->, +D
	AttributeList		m_ExtendChargeAttackList;

	// D~ 크기가 작아지며 전방 위치로 이동하면서 공격
	D3DXVECTOR3			m_vMoveDir;
	float				m_fSlideSpeed;
	float				m_fMoveAmt;
	float				m_fTargetRange;
	float				m_fChargeSlideSpeed;
	float				m_fJumpSlideMoveRange;
	float				m_fJumpSlideTaregtCheckRange;
	float				m_fJumpSlideAngle;
	TargetWoundType		m_TargetWoundType;
	bool				m_bAimedBall;
	ioHashString		m_AimedTarget;

	// D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	float				m_fSlideEndJumpAmt;
	float				m_fSlideEndJumpEnableHeight;

	// D~ 이어서 +--> 대시
	bool				m_bUsedDashJumpAttack;
	DWORD				m_dwJumpDashStartTime;
	DWORD				m_dwJumpDashEndTime;

	// D~ 이어서 +D~ 이나 +D
	DWORD				m_dwExtendChargingTime;
	DWORD				m_dwExtendChargingStartTime;

	// 이펙트
	ioHashString		m_szScaleEffectCreateSound;
	ioHashString		m_szScaleEffectName;		// 커질때
	DWORD				m_dwScaleEffectID;
	ioHashString		m_szExtendScaleEffectName;	// 작아질때
	DWORD				m_dwExtendScaleEffectID;

	// D~ 에서 +D~ 이어서 +--> 할때 중력값 적용
	float				m_fComboDashGravityAmt;

	// 이동 속도 제한 2020-10-29
	ioHashString		m_SpeedDownBuff;



protected:
	void			SetChargingState( ioBaseChar *pOwner );
	void			ProcessCharging( ioBaseChar *pOwner );

		
public:	// +D 		
	void			SetComboJumpAttack( ioBaseChar *pOwner );
	void			ProcessComboJumpAttack( ioBaseChar *pOwner );

	// 이펙트 이동
	void			ProcessScaleEffect( ioBaseChar* pOwner );

protected:	// D~
	void			CheckExtendComboAttack( ioBaseChar *pOwner );
	void			SetExtendComboChargingJumpAttack( ioBaseChar *pOwner, bool bExtendChargeCombo = false );
	void			ProcessComboChargingJumpAttack( ioBaseChar *pOwner );
	void			ProcessSlideMove( ioBaseChar *pOwner, D3DXVECTOR3 vTotalMove );
	void			CheckTarget( ioBaseChar *pOwner, float fAngle, float fTargetCheckRange, float fRange, bool bSend );
	ioBaseChar		*FindTarget( ioBaseChar *pOwner, float fAngle, float fRange );
	void			SetEndState( ioBaseChar *pOwner );

	bool			CheckEnableExtendComboState( ioBaseChar *pOwner );								// 게이지 체크
	void			CreateScaleEffectChargeEffect( ioBaseChar *pOwner, bool bExtendChargeCombo );	// 이펙트

protected:	// D~ 이어서 --> 대시 아니면 +D~ 이후에 +--> 대시
	bool			IsCanJumpDash( ioBaseChar *pOwner );
	void			SetJumpDash( ioBaseChar *pOwner, bool bExtendChargingAttack = false );
	void			ProcessJumpDash( ioBaseChar *pOwner );	
	void			ProcessExtendCharging( ioBaseChar *pOwner );	// +D~

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;
	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );
	bool	IsDashAttackState();
	float	GetCurGravityAmt();
	bool	IsExtendComboChargingJumpAttack();

public:	// 기본 점프 함수
	virtual void InitJump();
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessLanding( ioBaseChar *pOwner );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );

public:
	ioSizeComboJump();
	ioSizeComboJump( const ioSizeComboJump &rhs );
	virtual ~ioSizeComboJump();
};

inline ioSizeComboJump* ToSizeComboJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_SIZECOMBO_JUMP )
	{
		return NULL;
	}

	return dynamic_cast< ioSizeComboJump* > (pJump);
}