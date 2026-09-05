#pragma once

#include "ioSizeItem.h"
#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioSizeComboDash : public ioExtendDash
{
public:
	enum ChargeDashState
	{
		CDS_NONE,
		CDS_CHARGE,
		CDS_NORMAL_ATTACK,
		CDS_EXTEND_COMBO,
		CDS_COMBO_ATTACK,		
		CDS_EXTEND_CHARGE,	// +D, +D~
		CDS_STATE_END,
		CDS_CHARGING_TARGET,
	};

protected:
	ChargeDashState				m_DashState;

	// D~ 크기가 커지면서 상대를 내려친다.
	bool						m_bScale;
	float						m_fScaleValue;
	float						m_fExtendScaleValue;	
	D3DXVECTOR3					m_vBeforeScale;

	// extend combo D~, +-->, +D
	AttributeList				m_ExtendAttackList;

	// extend combo +D~, +-->, +D
	AttributeList				m_ExtendChargeAttackList;

	DWORD						m_dwChargingTime;			
	DWORD						m_dwChargingStartTime;		

	// +D~ : 크기가 작아지면서 상대의 위치로 이동하며 공격
	DWORD						m_dwExtendChargingTime;
	DWORD						m_dwExtendChargingStartTime;
	ioHashStringVec				m_ExtendChargingAniList;
	bool						m_bExtendChargingAttack;

	// +D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	D3DXVECTOR3					m_vMoveDir;
	float						m_fSlideSpeed;
	float						m_fMoveAmt;
	float						m_fTargetRange;
	float						m_fChargeSlideSpeed;
	float						m_fSlideMoveRange;
	float						m_fSlideTaregtCheckRange;
	float						m_fSlideAngle;
	TargetWoundType				m_TargetWoundType;
	bool						m_bAimedBall;
	ioHashString				m_AimedTarget;
	float						m_fSlideEndJumpAmt;
	float						m_fSlideEndJumpEnableHeight;

	ioHashStringVec				m_ChargingAniList;
	ioHashString				m_szGatheringAni;
	float						m_fChargingSpeedRate;

	int							m_iCurComboCnt;
	DWORD						m_dwFireTime;
	DWORD						m_dwCancelCheckTime;
	DWORD						m_dwActionEndTime;
	DWORD						m_dwActionStartTime;
	DWORD						m_dwPreReserveEnableTime;

	bool						m_bSetCombo;
	float						m_fCurChargeRate;

	ioHashString				m_szInputEventAnimation;
	float						m_fInputEventAnimationRate;	
	ioUserKeyInput::DirKeyInput	m_eSpecialDashCurrDirkey;

	// 이펙트
	ioHashString				m_szScaleEffectCreateSound;
	ioHashString				m_szScaleEffectName;		// 커질때
	DWORD						m_dwScaleEffectID;
	ioHashString				m_szExtendScaleEffectName;	// 작아질때
	DWORD						m_dwExtendScaleEffectID;

	// 일정 시간 이후에 이동
	DWORD						m_dwExtendChargingMoveCoolTime;
	DWORD						m_dwExtendSlideMoveCoolTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ExtendDashEnd( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );
	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	bool IsPowerChargingState();

	inline float GetChargingSpeedRate() const { return m_fChargingSpeedRate; }

protected:
	// D~
	void ChangeToCharging( ioBaseChar *pOwner );
	void ProcessCharging( ioBaseChar *pOwner );
	void SetExtendComboState( ioBaseChar *pOwner, bool bExtendChargeCombo = false );
	void ProcessExtendComboDash( ioBaseChar *pOwner );
	void CheckExtendReserve( ioBaseChar *pOwner );
	bool CheckEnableExtendComboState( ioBaseChar *pOwner );	// 게이지 체크

	void CheckExtraReserve( ioBaseChar *pOwner );
	virtual void ProcessKeyReserve( ioBaseChar *pOwner );

	// D
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ProcessNormalComboDash( ioBaseChar *pOwner );
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

	// +D~ : 크기가 작아지면서 상대의 우치로 이동하며 공격
	void ProcessExtendCharging( ioBaseChar *pOwner );
	void ChangeToExtendCharging( ioBaseChar *pOwner );	

	bool CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner );

	// 콤보 공격 
	void SetComboDash( ioBaseChar *pOwner, bool bCharge = false );

	bool CheckAniJump_Input_Jump_S( ioBaseChar *pOwner );
	void CheckTarget( ioBaseChar *pOwner, float fAngle, float fTargetCheckRange, float fRange, bool bSend );
	ioBaseChar *FindTarget( ioBaseChar *pOwner, float fAngle, float fRange );
	void SetEndState( ioBaseChar *pOwner );
	void ProcessSlideMove( ioBaseChar *pOwner, D3DXVECTOR3 vTotalMove );

	// 이펙트
	void CreateScaleEffectChargeEffect( ioBaseChar *pOwner, bool bExtendChargeCombo );

public:	// 이펙트 이동
	void ProcessScaleEffect( ioBaseChar* pOwner );

	// CS_FALL 
	bool IsNoDropState( ioBaseChar *pOwner );

public:
	ioSizeComboDash();
	ioSizeComboDash( const ioSizeComboDash &rhs );
	virtual ~ioSizeComboDash();
};

inline ioSizeComboDash* ToSizeComboDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_SIZE_COMBO )
		return NULL;

	return dynamic_cast< ioSizeComboDash* >( pDash );
}

