#pragma once

#include "ioWeaponItem.h"
#include "WeaponAttribute.h"

class ioWeapon;

class ioObjectItem : public ioWeaponItem
{
public:
	enum ObjectSubType
	{
		OST_THROW,
		OST_THROWBOMB,
		OST_DRINK,
		OST_EXCAVATION, 
		OST_CONTACT_WEAPON,
		OST_WEREWOLF,
		OST_SMARTBOOM,
		OST_POWEREDARMOR,
		OST_POWER_CHARGE,
		OST_CHARGE_SHOT,
		OST_DOUBLE_SHOT,
		OST_JINMORI,
		OST_TARGET_SHOT,
		OST_RUNNING_ATTACK,
		OST_GALAXY,
		OST_COUNT,
		OST_ATTACH,
		OST_THUNDER_BIRD,
		OST_ALICE,
		OST_PARKILPYO,
		OST_BLADE_CHARGE,
		OST_SPIRAL_CHARGE,
		OST_VEHICLE_DRIVER,		
		OST_DARKKNIGHT,
		OST_RUNNING_COMBO_ATTACK,
		OST_ONIMUSHA,
		OST_DRUID,
		OST_JACK_FROST,
		OST_MIDNIGHT_SCYTHE,
		OST_MIDNIGHT_GUN,
		OST_GWONWANG	// 권왕 변신 2020-06-08
	};

	enum EnableSlotType
	{
		EST_ALL,
		EST_TYPE1,
	};

	enum ObejctCreatorType
	{
		OCT_NONE,
		OCT_SOILDER,
		OCT_EQUIP_SKILL,
		OCT_EQUIP_BUFF1,
		OCT_EQUIP_BUFF2,
		OCT_STEAL_EQUIP_BUFF,
	};

protected:
	ObejctCreatorType m_eObejctCreatorType;

	BuffInfoList m_OwnerBuffList;

	ioHashString	m_ObjectCreateItem;			//해당 오브젝트 아이템을 생성한 무기 아이템
	ioHashString	m_ControlBuff;				//오브젝트를 생성한 버프
	DWORD			m_dwObjectReleaseTime;
	bool			m_bObjectReleaseTCPSend;

	float			m_fOffSet;
	float			m_fHeightOffSet;

	CEncrypt<bool>	m_bEnableWeaponSkill;
	int				m_nEableWeaponSkillType;

	CEncrypt<bool>	m_bEnableArmorSkill;
	int				m_nEnableArmorSkillType;

	CEncrypt<bool>	m_bEnableHelmetSkill;
	int				m_nEnableHelmetSkillType;

	CEncrypt<bool>	m_bEnableCloakSkill;
	int				m_nEnableCloakSkillType;

	CEncrypt<bool>	m_bEnableBuffFlyJump;
	CEncrypt<bool>	m_bDisableNormalAttack;

	EquipSlot		m_dwOwerItemEquipSlot;	

	bool			m_bPreventRecoveryEquipSKillGauge;
	bool			m_bPreventRecoveryAllSKillGauge;

	CEncrypt<int>	m_iMaxUseCount;
	int				m_iCurUseCount;

	ioHashString	m_ActiveCountBuff;
	ioHashString	m_FemaleEquipMesh;

	ioHashStringVec	m_ObjectReleaseSelctionBuffList;
	ioHashString	m_szDeleteFieldItemAni;

public:	// overide ioItem
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ItemType GetType() const;

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual const ioHashString& GetEquipMesh( ioBaseChar *pOwner ) const;

public:	// overide ioWeaponItem
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsUseDelete() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;

	virtual WeaponSubType GetSubType() const;

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

public:
	virtual bool IsProtectState();
	virtual bool EnableReduceGaugeOnSwitchBuff();

public:
	virtual ObjectSubType GetObjectSubType() const = 0;

protected:
	void Init();
	void LoadBuffList( ioINILoader &rkLoader );

public:
	virtual int GetGrowthInfoNum( int iGrowthNum ) const;
	virtual int GetGrowthInfoNumByType( int iType, int &iGrowthNum ) const;

public:	
	void SetObjectCreateItem( const ioHashString& szItemName );
	void SetObejctCreateType( ObejctCreatorType eType ){ m_eObejctCreatorType = eType; }
	ObejctCreatorType GetObejctCreateType(){ return m_eObejctCreatorType; }

public:	
	bool CheckEnableUseSkill( int iSlot, ioBaseChar *pOwner );
	bool CheckEnableFlyJumpBuff() const { return m_bEnableBuffFlyJump; }

	void SetControlBuff( const ioHashString& szControlBuff ){ m_ControlBuff = szControlBuff; }
	const ioHashString &GetControlBuffName() { return m_ControlBuff; }

	const bool CheckPreventRecoveryEquipSkillGauge() { return m_bPreventRecoveryEquipSKillGauge; }
	const bool CheckPreventRecoveryAllSkillGauge() { return m_bPreventRecoveryAllSKillGauge; }
	const ioHashString &GetObjectCreateItem() { return m_ObjectCreateItem; }

public:
	virtual void CheckAttackCount( ioBaseChar* pOwner );
	virtual void CheckActiveCount( ioBaseChar *pOwner );
	inline bool IsCanNormalAttack() { return !m_bDisableNormalAttack; }

	//공격 콤보수를 초기화 할 것인가? (콤보수 유지)
	virtual bool IsAttackComboInit() const;

public:
	virtual void OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner ){};
	
	virtual bool CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner );
	virtual bool CheckReleaseByNormalAttack( ioPlayStage *pStage, ioBaseChar *pOwner );

	virtual bool CheckObjectReleaseSelectionBuff( ioBaseChar *pOwner );
	
	virtual void SetObjectReleaseTCPSend( bool bSend ){ m_bObjectReleaseTCPSend = bSend; }
	virtual bool IsObjectReleaseTCPSend(){ return m_bObjectReleaseTCPSend; }

public:
	virtual bool IsNormalAttackStop(){ return false; }	
	virtual ioHashString& GetDeleteFieldItemAni()	{ return m_szDeleteFieldItemAni; }

public:
	ioObjectItem();
	ioObjectItem( const ioObjectItem &rhs );
	virtual ~ioObjectItem();
};

inline ioObjectItem* ToObjectItem( ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_OBJECT )
		return NULL;

	return dynamic_cast< ioObjectItem* >( pItem );
}

inline const ioObjectItem* ToObjectItemConst( const ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_OBJECT )
		return NULL;

	return dynamic_cast< const ioObjectItem* >( pItem );
}

