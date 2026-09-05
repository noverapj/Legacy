#pragma once

#include "ioWeaponItem.h"

class ioEntityGroup;

class ioAnubisItem : public ioWeaponItem
{
protected:
	DWORD m_dwTimeGap;
	DWORD m_dwChargeStartTime;
	DWORD m_dwMiddleChargeTime;

	float m_fRange;
	float m_fMagicCircleRadius;
	float m_fMoveSpeed;
	float m_fHeightGap;
	CEncrypt<float> m_fSkillGaugeRate;

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

	ioHashString m_CreateAreaWeaponAnimation;
	float		 m_fCreateAreaWeaponRate;

	/*ioHashString m_ExtendAttackAnimation;
	float		 m_fExtendAttackRate;*/
	

	D3DXVECTOR3 m_vAreaPos;
	DWORD m_dwCreatedAreaWeaponIndex;
	ioHashString m_AreaWeaponName;
	D3DXVECTOR3 m_ExtendAttackPos;

	AttackAttribute m_ExtendAttack;

	DWORD m_dwMotionEndTime;
	DWORD m_dwFireTime;

	ioHashString m_stCheckBuff;
	int  m_nExtendAttackCnt;


protected:
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_GATHERING,
		FS_NORMAL_ATTACK,
		FS_AREA_CREATING,
		FS_AREA_CREATED,
		FS_EXTEND_ATTACKING,
		FS_EXTEND_ATTACKED,
	};

	FireState m_FireState;
	CEncrypt<int>  m_iCurCombo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	//virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

	virtual WeaponSubType GetSubType() const;

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void OnReleased( ioBaseChar *pOwner );

public:
	float GetCircleRadius();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToExtendAttack( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );

	void ChangeToCreateArea( ioBaseChar *pOwner, const D3DXVECTOR3& vPos );
	void CreateAreaWeapon( ioBaseChar *pOwner, const D3DXVECTOR3& vPos );
	void CreateExtendAttack( ioBaseChar *pOwner );
	void DestroyAreaWeapon( ioBaseChar *pOwner );

	ioAreaWeapon* FindAreaWeapon( ioBaseChar *pOwner );

public:
	ioAnubisItem();
	ioAnubisItem( const ioAnubisItem &rhs );
	virtual ~ioAnubisItem();
};

inline ioAnubisItem* ToAnubisItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ANUBIS )
		return NULL;

	return dynamic_cast< ioAnubisItem* >( pWeapon );
}
