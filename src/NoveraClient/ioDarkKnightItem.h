#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDarkKnightItem : public ioWeaponItem
{
friend class ioDarkKnightSpecialState;
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_EXTEND_ATTACK_FIRE,
		CS_CONDITION_SPECIAL_DASH_NORMAL,
		CS_CONDITION_SPECIAL_DASH_JUMP,
		CS_SPECIAL_FINAL_BY_NORMAL,
		CS_SPECIAL_FINAL_BY_JUMP,
		CS_SPECIAL_FINAL_BY_DASH,
		CS_EQUIP_OBJECT,
	};

	enum SyncState
	{
		SS_NORMAL_ATTACK,
		SS_EXTEND_ATTACK,
		SS_EXTEND_ATTACK_BY_SPECIAL_DASH,
		SS_SPECIAL_DASH_BY_JUMP,
		SS_SPECIAL_DASH_BY_NORMAL,
		SS_SPECIAL_FINAL_BY_NORMAL,
		SS_SPECIAL_FINAL_BY_JUMP,
		SS_SPECIAL_FINAL_BY_DASH,
		SS_EQUIP_OBJECT,
	};

protected:
	ChargeState			m_ChargeState;	

	//Normal
	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;

	//차징
	ioHashStringVec		m_PreAnimationList;
	CEncrypt<float>		m_fNeedGauge;

	CEncrypt<float>		m_fObjectGaugeAttack;
	CEncrypt<float>		m_fObjectGaugeAttackDef;
	CEncrypt<float>		m_fObjectGaugeWound;
	CEncrypt<float>		m_fObjectGaugeWoundDef;

	CEncrypt<float>		m_fObjectZeroHPGaugeAttack;
	CEncrypt<float>		m_fObjectZeroHPGaugeAttackDef;
	CEncrypt<float>		m_fObjectZeroHPGaugeWound;
	CEncrypt<float>		m_fObjectZeroHPGaugeWoundDef;

	//Extend
	AttributeList		m_vExtendAttributeList;

	//대쉬 캔슬(노말, 대쉬공격)
	ioHashString		m_szConditionSpecialDashFront;
	ioHashString		m_szConditionSpecialDashBack;
	ioHashString		m_szConditionSpecialDashRt;
	ioHashString		m_szConditionSpecialDashLt;

	float				m_fConditionSpecialDashFrontRate;
	float				m_fConditionSpecialDashBackRate;
	float				m_fConditionSpecialDashRtRate;
	float				m_fConditionSpecialDashLtRate;	
	float				m_fConditionSpecialDashForceAmt;
	float				m_fConditionSpecialDashForceFric;

	DWORD				m_dwConditionSpecialDashEndTime;
	DWORD				m_dwConditionSpecialDashForceTime;
	D3DXVECTOR3			m_vForceDir;

	int					m_eFinalShotCallerType;
	int					m_eCurreCallerType;
	int					m_iCurrConditionSpecialDashCount;
	int					m_iMaxConditionSpecialDashCount;

	ioHashStringVec		m_ConditionSpecialDashEffectList;
	UniqueObjID			m_CurrSpecialDashEffectID;
	
	//대쉬 캔슬 풀차징 공격(노말,점프,대쉬)
	int					m_iCurrSpecialComboByNormal;
	int					m_iCurrSpecialComboByJump;
	int					m_iCurrSpecialCmoboByDash;
		
	DWORD				m_dwSpecialByNormalEndTime;
	DWORD				m_dwSpecialByJumpEndTime;
	DWORD				m_dwSpecialByDashEndTime;
		
	DWORD				m_dwSpecialByNormalReseveTime;
	DWORD				m_dwSpecialByJumpReseveTime;
	DWORD				m_dwSpecialByDashReseveTime;

	AttributeList		m_vSpecialByNormalAttributeList;
	AttributeList		m_vSpecialByJumpAttributeList;
	AttributeList		m_vSpecialByDashAttributeList;

protected:
	DWORD				m_dwObjectItem;
	DWORD				m_dwObjectEquipDealyTime;
	DWORD				m_dwObjectEquipDealyStartTime;

	AttackAttribute		m_ObjectEquipAttribute;

	IntVec				m_vDisableChangeObjectBuff;

protected:
	bool				m_bReserveExtendDash;

public:
	void Init();
	void ClearSpecial( ioBaseChar* pOwner );
	ChargeState GetState(){ return m_ChargeState; }

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadConditionSpecialDash( ioINILoader &rkLoader );
	void LoadAttribtue( IN ioINILoader &rkLoader, IN const char* szTtitle, OUT AttributeList& rkAttribute );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool SetExtendDashState( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );	
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	virtual void CheckExtendDashState( ioBaseChar *pOwner );
		
	virtual void CallItemProcess( ioBaseChar* pOwner );
	virtual bool CheckLanding( ioBaseChar *pChar );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnAttackFire( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void OnConditionSpecialDash( ioBaseChar *pOwner, ioPlayStage* pStage );	

	void OnConditionSpecialFinalByDash( ioBaseChar *pOwner, ioPlayStage* pStage );
	void OnConditionSpecialFinalByJump( ioBaseChar *pOwner, ioPlayStage* pStage );
	void OnConditionSpecialFinalByNormal( ioBaseChar *pOwner, ioPlayStage* pStage );

public:
	void CheckObject( ioBaseChar *pOwner, DWORD dwCurrTime );
	void CheckObjectEquip( ioBaseChar *pOwner );

public:
	void SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, IN bool bExtra, IN bool bAutoTarget, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve );

protected:
	void ChangeConditionSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

protected:
	void ChangeToCharge( ioBaseChar *pOwner, int iCurCombo );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToExtendAttackFire( ioBaseChar *pOwner );
		
	void ChangeConditionSpecialDashByNormal( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir, int eCallerType );
	void ChangeConditionSpecialDashByJump( ioBaseChar *pOwner );

	void ChangeConditionFinalShotByNormal( ioBaseChar *pOwner );
	void ChangeConditionFinalShotByJump( ioBaseChar *pOwner );
	void ChangeConditionFinalShotByDash( ioBaseChar *pOwner );

	void ChangeExtendDashStateByonditionSpecialDash( ioBaseChar *pOwner );

protected:
	void ChangeEffect( ioBaseChar *pOwner );

public:
	void GetConditionSpecialValue( OUT int& eCurrType, OUT int& iCurrConditionSpecialDashCount );
	void SetConditionSpecialValue( int eCurrType, int iCurrConditionSpecialDashCount );

public:
	bool IsEquipObject( ioBaseChar *pOwner );

protected:	
	void ChangeEquipObjectState( ioBaseChar* pOwner );
	void ReleaseObjectReserve( ioBaseChar *pOwner, ioObjectItem* pObject );

public:	
	void ReleaseObject( ioBaseChar *pOwner, bool bNeedProcess = true );

	virtual void ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem );
	virtual void ReleaseObjectState( ioBaseChar* pOwner, ioObjectItem* pReleasedObject );

public:
	void SetReserveExtendDash( bool bDash ){ m_bReserveExtendDash = bDash; }

public:
	void FillInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

public:
	ioDarkKnightItem();
	ioDarkKnightItem( const ioDarkKnightItem &rhs );
	virtual ~ioDarkKnightItem();
};

inline ioDarkKnightItem* ToDarkKnightItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DARKKNIGHT_ITEM )
		return NULL;

	return dynamic_cast< ioDarkKnightItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioDarkKnightSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_DARKKNIGHT_SPECIAL"; }
	virtual const CharState GetState(){ return CS_DARKKNIGHT_SPECIAL; }	
	virtual bool IsIgnoreAttackOverDelay();
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

protected:
	bool IsCheckTimeFreezing();

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioDarkKnightSpecialState();
	virtual ~ioDarkKnightSpecialState();
};

inline ioDarkKnightSpecialState* ToDarkKnightSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_DARKKNIGHT_SPECIAL )
		return NULL;

	return dynamic_cast< ioDarkKnightSpecialState* >( pState );
}