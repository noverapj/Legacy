#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioNanginItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
	};

	enum SpecialState
	{
		SS_NONE,
		SS_TARGET_FIND,
		SS_TARGET_FAIL,
		SS_TRACE_PRE,
		SS_TRACE_ATTACK,
		SS_TRACE_END,
		SS_CONDITION_SPECIAL_DASH,
	};

	enum SubStateSync
	{
		SSS_NONE,
		SSS_TARGET_FIND,
	};

protected:
	ChargeState		m_ChargeState;
	SpecialState	m_SpecialState;

	DWORD			m_dwAttackStartTime;
	int				m_iCurCombo;

	CEncrypt<float>	m_fNeedGauge;

	//D꾹 이동관련
	bool			m_bChangeDir;
	bool			m_bExtendMotionSetted;
	bool			m_bFirstExtendMoveMotion;
	float			m_fExtendMoveAniRate;
	float			m_fMoveSpeedRateExtend;
	ioHashString	m_ExtendMoveFront;

	//D꾹 이동 이펙트 관련
	CEncrypt<float>	m_fGatheringScaleRate;
	CEncrypt<float>	m_fGatheringScaleMaxRate;
	float			m_fCurrScaleRate;
	ioHashString	m_GatherEffect;
	ioHashString	m_GatherAuraEffect;
	float			m_fHeightOffSet;
	UniqueObjID		m_dwGatherEffectID;
	UniqueObjID		m_dwGatherAuraEffectID;
	float			m_fMaxScaleRate;

	//D꾹 이동 애니메이션
	ioHashStringVec	m_PreGatherAnimationList;
	ioHashString	m_GatherAnimation;
	
	//타겟팅 관련
	float			m_fTargetAngle;
	float			m_fTargetMaxRange;
	float			m_fTargetMinRange;
	float			m_fTargetRangeRate;
	float			m_fTargetCurrRange;

	DWORD			m_dwTargetDuration;
	DWORD			m_dwTargetingStart;
	DWORD			m_dwTargetingDurationCheckTime;

	//애니메이션 관련
	DWORD			m_dwAnimationStartTime;
	DWORD			m_dwAnimationEndTime;

	//타겟팅 실패
	AttackAttribute	m_TargetFail;

	//추적 대기 모션	
	AttackAttribute	m_ChargeTracePre;

	//추적관련
	WeaponInfo		m_ChargeWeaponInfo;
	D3DXVECTOR3		m_vAttackDir;
	DWORD			m_dwWeaponBaseIndex;
	float			m_fChargeTraceNextJumpAmt;
	float			m_fChargeTraceJumpJudgment;

	//추적종료
	AttackAttribute	m_ChargeTraceEnd;
	DWORD			m_dwTraceEndDelayTime;
	DWORD			m_dwCurrTraceEndTime;
	bool			m_bReserveClear;

	//특별 조건 대쉬	
	ioHashString	m_szConditionSpecialDashBack;
	ioHashString	m_szConditionSpecialDashRt;
	ioHashString	m_szConditionSpecialDashLt;
	
		
	float			m_fConditionSpecialDashBackRate;
	float			m_fConditionSpecialDashRtRate;
	float			m_fConditionSpecialDashLtRate;	
	float			m_fConditionSpecialDashForceAmt;
	float			m_fConditionSpecialDashForceFric;

	DWORD			m_dwConditionSpecialDashEndTime;
	DWORD			m_dwConditionSpecialDashForceTime;
	D3DXVECTOR3		m_vForceDir;
	
	int				m_eCurreCallerType;	

public:
	void InitAll();
	void InitSpecial();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadConditionSpecialDash( ioINILoader &rkLoader );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );	
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );	
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void CheckExtendDashState( ioBaseChar *pOwner );	

	virtual float GetAttackSpeedRate() const;
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnAttackFire( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );

public:
	void ClearState( ioBaseChar* pOwner );
	void ProcessNanginState( ioBaseChar *pOwner );

protected:
	void OnTargetFind( ioBaseChar *pOwner, ioPlayStage* pStage );
	void OnGatheringEffect( ioBaseChar *pOwner, ioPlayStage* pStage );
	void OnTargeting( ioBaseChar *pOwner, ioPlayStage* pStage );
	void OnTracePre( ioBaseChar *pOwner, ioPlayStage* pStage );
	void OnTraceAttack( ioBaseChar *pOwner, ioPlayStage* pStage );
	void OnTraceEnd( ioBaseChar *pOwner, ioPlayStage* pStage );
	void OnTargetFail( ioBaseChar *pOwner, ioPlayStage* pStage );
	void OnConditionSpecialDash( ioBaseChar *pOwner, ioPlayStage* pStage );

	void SetActionAnimation( ioBaseChar *pOwner, const AttackAttribute& rkAttr );

	void ChangeToTargetFind( ioBaseChar *pOwner );
	void ChangeToTracePre( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ChangeToFail( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ChangeToTraceAttack( ioBaseChar *pOwner );
	void ChangeToTraceAttackEnd( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ChangeConditionSpecialDash( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir, int eCallerType );

	void CheckExtendMoveAniState( ioBaseChar *pOwner );		

public:	
	bool IsCharCollisionSkipState();
	ioNanginItem::SpecialState GetSpecialState();
	void SetPositionOwner( ioBaseChar *pOwner, D3DXVECTOR3 vPos );

protected:
	bool CompareAimedTarget( ioBaseChar *pOwner );	

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

public:
	ioNanginItem();
	ioNanginItem( const ioNanginItem &rhs );
	virtual ~ioNanginItem();
};

inline ioNanginItem* ToNanginItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_NANGIN_ITEM )
		return NULL;

	return dynamic_cast< ioNanginItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioNanginSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NANGIN_SPECIAL"; }
	virtual const CharState GetState(){ return CS_NANGIN_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual bool IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const;
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );

protected:
	int GetNanginSpeicalState( ioBaseChar* pOwner );

public:
	ioNanginSpecialState();
	virtual ~ioNanginSpecialState();
};

inline ioNanginSpecialState* ToNanginSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NANGIN_SPECIAL )
		return NULL;

	return dynamic_cast< ioNanginSpecialState* >( pState );
}