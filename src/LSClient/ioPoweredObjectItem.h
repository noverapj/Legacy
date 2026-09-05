#pragma once

#include "ioObjectItem.h"

//class ioWeapon;
class ioPoweredObjectItem : public ioObjectItem
{
protected:

	struct ScaleInfo
	{
		float m_fScaleRate;
		DWORD m_dwScaleTime;
		AttackAttribute m_AttackAttribute;
		ScaleInfo()
		{
			Init();
		}
		void Init()
		{
			m_fScaleRate = 0;
			m_dwScaleTime = 0;
			m_AttackAttribute.Init();
		}
	};
	typedef std::vector<ScaleInfo> ScaleInfoList;
	ScaleInfoList m_vChargingInfoList;
	ioHashString m_szChargingSound;
	DWORD m_dwChargingSoundID;

	WeaponInfoList m_WoundedAttackList;

	ioHashString m_EquipedEffect;
	ioHashString m_WeaponChargingEffect;
	UniqueObjID m_WeaponChargingEffectID;

	CEncrypt<DWORD> m_dwPoweredTime;
	float m_fMaxScaleRate;
	float m_fMinScaleRate;
	DWORD m_fMaxScaleTime;

	D3DXVECTOR3 m_vLinkOffsetPos;

	int m_iCurChargingList;

	DWORD m_dwMotionEndTime;

	bool m_bAttacked;

	CEncrypt<int> m_iReduceGaugeSlot;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual ObjectSubType GetObjectSubType() const;
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDefense() const;
	virtual bool IsEnableDash() const;
	virtual const AttackAttribute* GetAttackAttribute( AttackType eType, int iSubIdx = 0 ) const;
	virtual const WeaponInfoList& GetAttackAttributeIdx( AttackType eType, int iSubIdx = 0 ) const;
	virtual const ForceInfoList& GetForceInfoList( AttackType eType, int iSubIdx = 0 ) const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	void SetAttackMotionTime( ioBaseChar *pOwner, int iAniID, float fTimeRate );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual bool CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner );

public:
	virtual void OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner );
	const WeaponInfoList& GetWoundedAttributeList( ioBaseChar *pOwner );

	D3DXVECTOR3 GetLinkPosOffset( ioEffect* pEffect, ioBaseChar* pOwner );
	D3DXVECTOR3 GetWoundedPos(){ return m_vLinkOffsetPos; }

protected:
	void ReleaseChargingEffect( ioBaseChar *pOwner );
	void ReduceSkillGauge( ioBaseChar *pOwner, int iSlot );

public:
	const void SetAttacked()       { m_bAttacked = true; }
	const bool IsAttacked()        { return m_bAttacked; }
	const DWORD GetMotionEndTime() { return m_dwMotionEndTime; }
	const bool IsAttackEnd();

public:
	ioPoweredObjectItem();
	ioPoweredObjectItem( const ioPoweredObjectItem &rhs );
	virtual ~ioPoweredObjectItem();
};

inline ioPoweredObjectItem* ToPoweredObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_POWEREDARMOR )
		return NULL;

	return dynamic_cast< ioPoweredObjectItem* >( pObject );
}

