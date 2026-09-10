#pragma once

#include "ioWeaponItem.h"
#include "ioSpecialStateBase.h"

class ioEntityGroup;

class ioMagicStaffItem : public ioWeaponItem
{
public:
	enum InputAttackState
	{
		IAS_BASE_A,
		IAS_BASE_S,
		IAS_DASH_A,
		IAS_DASH_S,
		IAS_JUMP_A,
		IAS_JUMP_S,
	};

protected:
	DWORD m_dwTimeGap;
	DWORD m_dwChargeStartTime;

	float m_fRange;
	float m_fMagicCircleRadius;
	float m_fCircleOffSet;
	float m_fMoveSpeed;
	float m_fHeightGap;
	CEncrypt<float> m_fSkillGaugeRate;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

	D3DXVECTOR3 m_vCreatePos;
	UniqueObjID m_dwCurMapEffect;

protected:
	DWORD m_dwDefenseTicTime;
	float m_fGaugePerAllDefense;

protected:
	ioHashString m_PreGatherAnimation;
	ioHashString m_GatherAnimation;
	ioHashString m_EndGatherAnimation;
	ioHashString m_GatherEffect;

	ioHashString m_ReadyCircle;
	ioHashString m_EnableCircle;

	ioHashString m_ChargeAttackRedCircle;
	ioHashString m_ChargeAttackBlueCircle;
	ioHashString m_ChargeAttackDelay;
	ioHashString m_ChargeAttackAni;
	DWORD m_dwChargeAttackPreDelayStartTime;
	DWORD m_dwPreDelayTime;
	DWORD m_dwChargeAttackEndTime;
	DWORD m_dwChargeAttackFireTime;	
	float m_fChargeAttackAniRate;
	
	ioHashString m_AreaWeaponName;

	ioHashString m_SpecialAttackDelay;
	ioHashString m_SpecialAttack;
	Vector3Vec m_vSpecialWeaponPosList;
	IntVec m_vSpecialWeaponOffset;
	DWORDVec m_dwEnableCircleIDList;
	DWORD m_dwSpecialStartTime;
	DWORD m_dwSpecialEndTime;
	DWORD m_dwSpecialFireTime;
	DWORD m_dwSpecialAttackAttribute;
	float m_fSpecialAttackAniRate;
	int m_iMaxSpecialWeaponCnt;
	int m_iCurSpecialWeaponCnt;

protected:
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_GATHERING,
		FS_NORMAL_ATTACK,
		FS_CHARGE_ATTACK_DELAY,
		FS_CHARGE_ATTACK,
		FS_SPECIAL_ATTACK_DELAY,
		FS_SPECIAL_ATTACK,
		FS_SPECIAL_END,
		FS_INPUT_ATTACK,
	};

	FireState m_FireState;
	CEncrypt<int>  m_iCurCombo;

	DWORD m_dwInputAttackTime;
	DWORD m_dwInputAttackEndTime;
	int m_iInputAttackState;
	AttackAttribute m_cInputAttributeA;
	AttackAttribute m_cInputAttributeS;
	AttackAttribute m_cJumpInputAttributeA;
	AttackAttribute m_cJumpInputAttributeS;
	AttackAttribute m_cDashInputAttributeA;
	AttackAttribute m_cDashInputAttributeS;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

	virtual WeaponSubType GetSubType() const;

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();

	void SetChargeAttackState( ioBaseChar *pChar );
	void CreateArea( ioBaseChar *pChar );

public:
	float GetCircleRadius();

	void ClearAttackState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );

	void SetInputAttackInfo( ioBaseChar *pOwner, int iAniID, float fTimeRate );
	void SetInputAttackState( ioBaseChar *pOwner, bool bSendNet, int iState );
	bool CheckInputAttack();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner );
	void ChangeToSpecialDelayState( ioBaseChar *pOwner );
	void ChangeToSpecialAttackState( ioBaseChar *pOwner );
	void ChangeToSpecialEndState( ioBaseChar *pOwner );

	void CheckDecreaseGauge();
	void SetCurGauge( float fGauge );

	void AddSpecialAttackPosition( ioBaseChar *pChar, ioPlayStage *pStage );
	void CreateWeapon( ioBaseChar *pChar );

public:
	ioMagicStaffItem();
	ioMagicStaffItem( const ioMagicStaffItem &rhs );
	virtual ~ioMagicStaffItem();
};

inline ioMagicStaffItem* ToMagicStaffItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGIC_STAFF )
		return NULL;

	return dynamic_cast< ioMagicStaffItem* >( pWeapon );
}

inline ioMagicStaffItem* ToMagicStaffItem( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGIC_STAFF )
		return NULL;

	return dynamic_cast< ioMagicStaffItem* >( pWeapon );
}
//////////////////////////////////////////////////////////////////////////
class ioMagicStaffSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MAGICSTAFF_SPECIAL"; }
	virtual const CharState GetState(){ return CS_MAGICSTAFF_SPECIAL; }	
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );

public:
	ioMagicStaffSpecialState();
	virtual ~ioMagicStaffSpecialState();
};

inline ioMagicStaffSpecialState* ToMagicStaffSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MAGICSTAFF_SPECIAL )
		return NULL;

	return dynamic_cast< ioMagicStaffSpecialState* >( pState );
}