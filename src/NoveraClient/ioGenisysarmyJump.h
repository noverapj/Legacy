#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioGenisysarmyJump : public ioExtendJump
{
public:
	enum ChageJumpState
	{
		CJS_NONE,
		CJS_CHARGE,
		CJS_NORMAL_ATTACK,
		CJS_CHARGING_ATTACK,
	};

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:	// 기본 점프 함수
	virtual void InitJump();
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessLanding( ioBaseChar *pOwner );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );
	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual DWORD GetChargingTime( ioBaseChar *pChar );	// 점프차징 시간 2020-07-08

protected:
	ChageJumpState		m_ChargeJumpState;			// 기본 차지 점프 상태
	DWORD				m_dwChargingTime;			// 기본 공격 차징 시간
	AttackAttribute		m_ChargingAni;				// 기본 공격 차징 준비 애니

	bool				m_bReserveJumpAttack;		//
	DWORD				m_dwChargingStartTime;		//
	DWORD				m_dwJumpChargingStartTime;	//

	// 기본 점프 공격
	int					m_iMaxComboCnt;				// 점프 공격 카운트
	int					m_iCurComboCnt;				//
	DWORD				m_dwEnableComboTime_S;		//
	DWORD				m_dwEnableComboTime_E;		//
	DWORD				m_dwEnableChargeAttack;		//
	AttackAttribute		m_AttackAttribute;			// 기본 점프 공격

	bool				m_bJumpChargeEnable;		//	점프 차지 사용여부 2020-07-08
	DWORD				m_dwJumpKeyChargingTime;	//	점프 차지 시간 2020-07-08

	// 점프 후 캐릭터 이동, 타겟 회전
	float				m_fJumpSpeedRateFR;
	float				m_fJumpSpeedRateSI;
	float				m_fJumpSpeedRateBG;

protected:	// 점프 차지
	void	SetChargingState( ioBaseChar *pOwner );
	void	ProcessCharging( ioBaseChar *pOwner );

	// +D 
public:
	virtual void	CheckComboJumpAttack( ioBaseChar *pOwner );
	void			SetComboJumpAttack( ioBaseChar *pOwner );
	void			ProcessComboJumpAttack( ioBaseChar *pOwner );

public:	// D~
	void			SetComboChargingJumpAttack( ioBaseChar *pOwner );
	void			ProcessComboChargingJumpAttack( ioBaseChar *pOwner );

public:
	int		GetMaxComboCnt()	{	return m_iMaxComboCnt;	}


public:
	ioGenisysarmyJump();
	ioGenisysarmyJump( const ioGenisysarmyJump &rhs );
	virtual ~ioGenisysarmyJump();
};

inline ioGenisysarmyJump* ToGenisysarmyJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_GENISYSARMY_JUMP )
	{
		return NULL;
	}

	return dynamic_cast< ioGenisysarmyJump* > (pJump);
}