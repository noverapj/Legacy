#pragma once

#include "ioWeaponItem.h"
#include "ioSpecialStateBase.h"

class ioSawangItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vSlideAttributeList;
	AttributeList m_vAirSlideAttributeList;
	AttributeList m_vFinishAttributeList;
	AttributeList m_vAirFinishAttributeList;

public:
	enum SawangStateType
	{
		SST_NORMAL,
		SST_JUMP,
		SST_DASH,
	};

protected:
	enum
	{
		STATE_NONE,
		STATE_READY,
		STATE_CHARGING,
		STATE_SLIDE,
		STATE_SLIDE_END_WAIT,
		STATE_SECOND_SLIDE1,
		STATE_SECOND_SLIDE2,
		STATE_ATTACK_FIRE,
		STATE_EXTEND_ATTACK,
		STATE_FINISH_ATTACK,
		STATE_END,

		SYNC_CHARGE_ANI,
		SYNC_CHANGE_TARGET,
	};

protected:
	int m_SawangState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;
	int m_iExtendCombo;

	// Â÷Â¡
	ioHashString    m_szChargeAni;
	ioHashString    m_szChargeLoopAni;
	ioHashString    m_szChargeMoveAni;
	CEncrypt<float> m_fChargeMoveAniRate;
	CEncrypt<float> m_fChargeMoveSpeed;
	CEncrypt<bool>  m_bChargeRotateAttack;

	bool m_bSetChargeMove;
	bool m_bSlideAttacked;

	DWORD m_dwChargeEffectID;
	float m_fCharginfEffectScale;

	// Å¸°Ù
	DWORD			m_dwTargetCheckDuration;
	DWORD			m_dwTargetingEnableTime;

	ioHashString    m_szTargetEffect;
	CEncrypt<float> m_fTargetMinScaleRate;
	CEncrypt<float> m_fTargetMaxScaleRate;
	CEncrypt<float> m_fTargetMaxRange;
	CEncrypt<float> m_fTargetMinRange;
	CEncrypt<DWORD> m_dwTargetRangeTime;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fSecondTargetRange1;
	CEncrypt<float> m_fSecondTargetAngle1;
	CEncrypt<float> m_fSecondTargetRange2;
	CEncrypt<float> m_fSecondTargetAngle2;
	float m_fTargetRange;
	TargetWoundType m_TargetWoundType;

	ioHashString m_szAimEffect;
	DWORD m_dwAimEffectID;

	// Æ¯º°ÀÌµ¿
	ioHashString    m_szChargeSlideAni;
	CEncrypt<float> m_fChargeSlideSpeed;
	CEncrypt<int>   m_iSlideAttachType;
	CEncrypt<DWORD> m_dwChargeSlideEndWait;
	DWORD           m_dwChargeSlideEndTime;

	ioHashString    m_szChargeSecondSlide1Ani;
	CEncrypt<DWORD> m_dwChargeSecondSlide1Speed;
	ioHashString    m_szChargeSecondSlide2Ani;
	CEncrypt<DWORD> m_dwChargeSecondSlide2Speed;

	CEncrypt<float> m_fSlideEndJumpAmt;
	CEncrypt<float> m_fSlideEndJumpEnableHeight;

	D3DXVECTOR3 m_vMoveDir;
	float       m_fSlideSpeed;
	float       m_fMoveAmt;

protected:
	CEncrypt<bool>  m_bUsedExtraGauge;
	CEncrypt<int>   m_fNeedGauge;

	CEncrypt<DWORD> m_dwComboAttackTimeEnd;

	DWORD           m_dwFinishAttackCheckTime;
	CEncrypt<DWORD> m_dwFinishAttackWait;
	bool m_bSetReserveFinishAttack;

	float	m_fSawangJumpSlideMoveRange;
	float   m_fSawangJumpSlideTaregtCheckRange;
	float	m_fSawangJumpSlideAngle;

	float	m_fSawangDashSlideMoveRange;
	float   m_fSawangDashSlideTargetChecfkRange;
	float	m_fSawangDashSlideAngle;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	int GetNeedBullet() { return m_fNeedGauge; }

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	bool IsUsedExtraGauge() const { return m_bUsedExtraGauge; }

protected:
	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	void ClearState( ioBaseChar *pOwner );
	void ProcessSawangState( ioBaseChar *pOwner );

protected:
	void ProcessReady( ioBaseChar *pOwner );
	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessSlide( ioBaseChar *pOwner );
	void ProcessSlideEndWait( ioBaseChar *pOwner );
	void ProcessSecondSlide1( ioBaseChar *pOwner );
	void ProcessSecondSlide2( ioBaseChar *pOwner );
	void ProcessSlideMove( ioBaseChar *pOwner, D3DXVECTOR3 vTotalMove );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessExtendAttack( ioBaseChar *pOwner );
	void ProcessFinishAttack( ioBaseChar *pOwner );

	bool ChangeStateByReserveKeyInput( ioBaseChar *pOwner );

	void SetExtendAttack( ioBaseChar *pOwner );
	void SetFinishAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );

private:
	void SetChargingState( ioBaseChar *pOwner );
	void SetSecondSlideState1( ioBaseChar *pOwner );
	void SetSecondSlideState2( ioBaseChar *pOwner );
	void SetNextExtendCombo( ioBaseChar *pOwner );
	void CheckMoveAniState( ioBaseChar *pOwner );
	void CheckTarget( ioBaseChar *pOwner, float fAngle, float fTargetCheckRange, float fRange, bool bSend );
	ioBaseChar *FindTarget( ioBaseChar *pOwner, float fAngle, float fRange );
	void CreateAttachWeapon( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner, bool bSend = false );

public:
	void SetSlideState( ioBaseChar *pOwner, SawangStateType SawangType );

private:
	void CreateChargeEffect( ioBaseChar *pOwner );
	void DestroyChargeEffect( ioBaseChar *pOwner );
	void UpdateChargeEffect( ioBaseChar *pOwner );
	void UpdateAimEffect( ioBaseChar *pOwner );
	void DestroyAimEffect( ioBaseChar *pOwner );

public:
	virtual float GetAttackSpeedRate() const;
	bool IsNoDropState();
	bool IsCharColSkipState();
	bool IsEnableGauge();
	bool IsEnableMoveState();

public:
	inline void SetState( int iState ) { m_SawangState = iState; }
	inline int  GetState()             { return m_SawangState; }

public:
	ioSawangItem();
	ioSawangItem( const ioSawangItem &rhs );
	virtual ~ioSawangItem();
};

inline ioSawangItem* ToSawangItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SAWANG_ITEM )
		return NULL;

	return static_cast< ioSawangItem* >( pItem );
}





//-----------------------------------------------------------------------------------------------------------


class ioSawangSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SAWANG_STATE"; }
	virtual const CharState GetState(){ return CS_SAWANG_STATE; }
	//virtual bool IsEnableBlowWounded(){ return false; }
	virtual bool IsEnableDashState( ioBaseChar* pOwner );
	virtual bool IsNoDropState( ioBaseChar* pOwner );

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;

	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
public:
	ioSawangSpecialState();
	virtual ~ioSawangSpecialState();
};

inline ioSawangSpecialState* ToSawangSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SAWANG_STATE )
		return NULL;

	return dynamic_cast< ioSawangSpecialState* >( pState );
}