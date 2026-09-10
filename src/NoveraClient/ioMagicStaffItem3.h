#pragma once

#include "ioWeaponItem.h"

class ioEntityGroup;

class ioMagicStaffItem3 : public ioWeaponItem
{
public:
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_GATHERING,
		FS_NORMAL_ATTACK,
		FS_ELEMENT_ATTACK,
		FS_GATHERING_ATTACK,
	};

protected:
	FireState m_FireState;

	CEncrypt<int>  m_iCurCombo;

	DWORD m_dwTimeGap;
	DWORD m_dwChargeStartTime;

	DWORD m_dwGatheringStartTime;

	ioHashString m_PreGatherAnimation;
	ioHashString m_GatherAnimation;
	ioHashString m_GatherEffect;

	ioHashString m_TargetCircle;
	DWORD m_dwEnableChrageCircleID;

	ioHashString m_ReadyCircle;
	ioHashString m_EnableCircle;

	float m_fRange;
	float m_fMagicCircleRadius;
	float m_fMoveSpeed;
	float m_fHeightGap;

	CEncrypt<float> m_fSkillGaugeRate;

	// Gather att
	SummonAttackInfoList m_GatherEffInfo;
	DWORD m_dwNextGatherTime;

	DWORD m_dwGatherAttEndTime;
	D3DXVECTOR3 m_vCirclePos;

	UniqueObjID m_GatherOffSetEffID;

	ioHashString m_GatherAttAni;
	float m_fGatherAttAniRate;

	D3DXVECTOR3 m_GatherAttOffset;
	D3DXVECTOR3 m_GatherAttPos;

	ioHashString m_AreaWeaponName;

	CEncrypt<DWORD> m_dwMaxGatherAttCnt;
	CEncrypt<DWORD> m_dwCurGatherAttCnt;

	// ElementAttack
	MagicElementInfo m_MagicElementAtt;
	DWORD m_dwMagicElementAttEndTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

	virtual WeaponSubType GetSubType() const;

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	float GetCircleRadius();

	inline FireState GetCurFireState() const { return m_FireState; }

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnGatheringAttack( ioBaseChar *pOwner );
	void OnElementAttack( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToGatheringAttack( ioBaseChar *pOwner );
	void ChangeToElementAttack( ioBaseChar *pOwner, int iSubType );

	bool CheckNextGatherInfo();
	bool CheckNextGather( ioBaseChar *pOwner );

	ioHashString GetCurGatherEffect();
	ioHashString GetCurGatherSound();

protected:
	void SetCancelFire( ioBaseChar *pOwner );

public:
	ioMagicStaffItem3();
	ioMagicStaffItem3( const ioMagicStaffItem3 &rhs );
	virtual ~ioMagicStaffItem3();
};

inline ioMagicStaffItem3* ToMagicStaffItem3( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGIC_STAFF3 )
		return NULL;

	return dynamic_cast< ioMagicStaffItem3* >( pWeapon );
}

inline ioMagicStaffItem3* ToMagicStaffItem3( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGIC_STAFF3 )
		return NULL;

	return dynamic_cast< ioMagicStaffItem3* >( pWeapon );
}

