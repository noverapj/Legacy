#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;
class ioLeeSunSin : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_EXTEND_FIRE,
		CS_ASSULT_FIRE,
		CS_GATHERING,
	};

	enum SyncState
	{
		SS_ATTACK_FIRE,
		SS_GATHERING,
		SS_GATHERING_DIR,
		SS_GATHERING_END_BY_JUMP,
		SS_GATHERING_END_BY_SKILL,
		SS_GATHERING_END_BY_RUN,
		SS_WOUNDED_EXTEND_PRE,
		SS_EXTEND_ATTACK,
		SS_ASSULT_ATTACK,
	};

	enum WoundedState
	{
		WS_DELAY,
		WS_TIME_CONDITION_CHECK,
		WS_PRE,
		WS_ACTION,
	};

	enum GatheState
	{
		GS_NONE,
		GS_GATHER,
	};
	
	enum ExtendType
	{
		ET_NORMAL,
		ET_WOUNDED,
	};

	struct WoundedInfo
	{
		BlowTargetType	eBlowType;
		float			fMinHeight;
		float			fMaxHeight;

		ioHashString	BranchAnimation;
		float			fAnimationRate;

		WoundedInfo()
		{
			eBlowType		= BTT_NONE;
			fMinHeight		= 0.0f;
			fMaxHeight		= 0.0f;
			fAnimationRate	= 0.0f;
		}
	};
	typedef std::vector<WoundedInfo> WoundedInfoList;

protected:
	ChargeState					m_ChargeState;
	GatheState					m_GatherState;

	DWORD						m_dwAttackStartTime;	
	int							m_iCurCombo;
	CEncrypt<float>				m_fNeedGauge;
	CEncrypt<float>				m_fNeedGaugeByMax;

protected:
	ioHashString				m_PreGatherAnimation;
	ioHashString				m_GatherAnimation;
	ioHashString				m_GatherEffect;
	ioHashString				m_GatherMaxEffect;	
	
	UniqueObjID					m_GatherEffectID;
	bool						m_bCurrMaxEffect;

protected:
	ioUserKeyInput::DirKeyInput	m_CurrDirKey;

protected:
	AttackAttribute				m_ExtendAttack;
	AttackAttribute				m_WoundedExtendAttack;
	AttackAttribute				m_AssultExtendAttack;
	AttackAttribute				m_FullChargeExtendAttack;
	AttackAttribute				m_FullChargeWoundedExtendAttack;

	DWORD						m_dwExtendAttackEnd;
	DWORD						m_dwAssultDashCount;
	DWORD						m_dwAssultDashCurrCount;

protected:
	WoundedState				m_WoundedState;
	
	DWORD						m_dwWoundedChargeTime;
	DWORD						m_dwWoundedStartChargeTime;
	DWORD						m_dwWoundedPreEndTime;
	WoundedInfoList				m_WoundedInfoList;
	ioHashStringVec				m_WoundedExtendUseBuffList;

public:
	void Init();
	void EndEffect( ioBaseChar* pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadWoundedInfo( IN ioINILoader &rkLoader );
	void LoadBuffProperty( ioINILoader &rkLoader );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

public:
	void SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, IN bool bExtra, IN bool bAutoTarget, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve );

protected:	
	void OnNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnAsulltAttack( ioBaseChar *pOwner );

protected:
	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToExtendAttackFire( ioBaseChar *pOwner, ExtendType eType );
	void ChangeToExtendAttackFire( ioBaseChar *pOwner, AttackAttribute* pAttribute );	
	void ChangeToAssultAttackFire( ioBaseChar *pOwner );

	void ChangeToGatherEndByJump( ioBaseChar *pOwner );
	void ChangeToGatherEndBySkill( ioBaseChar *pOwner );
	void ChangeToGatherEndByRun( ioBaseChar *pOwner );

public:
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual bool IsWoundAfterUseExtendState(){ return true; }
	virtual bool CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType );

protected:
	void CheckWoundExtendUseDelay( ioBaseChar* pOwner, BlowTargetType eType );
	void ChangeToWoundedPreState( ioBaseChar* pOwner, const ioHashString& szAnmationName, float fTimeRate );

	bool CheckWoundExtendUsePre( ioBaseChar* pOwner, BlowTargetType eType );
	void OnWoundedExtensUseAddBuff( ioBaseChar* pOwner );

public:
	virtual void CallItemProcess( ioBaseChar* pOwner );

public:
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

public:
	void UpdateEffect( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	int GetNeedGaugeByMax();

public:
	ioLeeSunSin();
	ioLeeSunSin( const ioLeeSunSin &rhs );
	virtual ~ioLeeSunSin();
};

inline ioLeeSunSin* ToLeeSunSin( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LEESUNSIN_ITEM )
		return NULL;

	return dynamic_cast< ioLeeSunSin* >( pItem );
}

