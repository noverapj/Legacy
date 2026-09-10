#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSummonChargeItem3 : public ioWeaponItem
{
protected:
	enum SummonChargeState
	{
		SCS_NONE,
		SCS_CHARGING,
		SCS_GATHERING,
		SCS_ATTACK_FIRE,
		SCS_RETREAT,
		SCS_READY,
		SCS_SUMMON,
		SCS_CREATE_DUMMY,
	};

	SummonChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	UniqueObjID m_GatheringEffectID;

	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

protected:
	ioHashString m_AttackReadyAni;
	SummonAttackInfoList m_vSummonAttackInfoList;

	CEncrypt<int> m_iMaxSummonCnt;
	CEncrypt<int> m_iCurSummonCnt;

	DWORD m_dwNextSummonTime;

	int m_iLimitSummonCnt;

	// Retreat
	ioHashString m_szRetreatFr;
	float m_fRetreatFrRate;

	DWORD m_dwRetreatEndTime;

	D3DXVECTOR3 m_vForceDir;
	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

	int					m_iBuffUseEnableJumpCount;
	ioHashString		m_szDoubleJumpFlyBuff;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;
	CEncrypt<bool>	m_bEnableExtraGauge;

	bool m_bSummonSkullHeroEnable;
	AttackAttribute m_SummonAttack;
	DWORD m_dwMotionEndTime;
	DWORD m_dwFireStartTime;
	vDummyCharLoadInfoList m_DummyCharList;

protected:
	CEncrypt<float> m_fSkillGaugeRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );

	const ioHashString& GetDoubleJumpFlyBuff(){ return m_szDoubleJumpFlyBuff; }
	int GetBuffUseEnableJumpCount(){ return m_iBuffUseEnableJumpCount; }

	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge();
	bool IsEnableExtraGauge() const { return m_bEnableExtraGauge; }

	void ClearSpecialState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	ioHashString GetCurSummonEffect();
	ioHashString GetCurSummonSound();

	bool CheckNextSummon( ioBaseChar *pOwner );
	bool CheckNextSummonTime();

	void SetSummonReadyState( ioBaseChar *pOwner );
	void SetSummonSkullHeroState( ioBaseChar *pOwner );
	void SummonSkullHeroDummy( ioBaseChar *pOwner );
	ioDummyChar* CreateDummyChar( ioBaseChar *pOwner, ioPlayStage *pStage, ioHashString szDummyCharName, 
									int iDummyIndex, D3DXVECTOR3 vPos, float fStartAngle = 0 );
public:
	ioSummonChargeItem3();
	ioSummonChargeItem3( const ioSummonChargeItem3 &rhs );
	virtual ~ioSummonChargeItem3();
};

inline ioSummonChargeItem3* ToSummonChargeItem3( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SUMMON_CHARGE3 )
		return NULL;

	return dynamic_cast< ioSummonChargeItem3* >( pItem );
}

class ioSummonSkullHeroSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SUMMON_SKULLHERO_SPECIAL"; }
	virtual const CharState GetState(){ return CS_SUMMON_SKULLHERO_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioSummonSkullHeroSpecialState();
	virtual ~ioSummonSkullHeroSpecialState();
};

inline ioSummonSkullHeroSpecialState* ToSummonSkullHeroSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SUMMON_SKULLHERO_SPECIAL )
		return NULL;

	return dynamic_cast< ioSummonSkullHeroSpecialState* >( pState );
}