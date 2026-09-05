#pragma once


#include "ioItem.h"

class ioEntityGroup;
class ioWeaponItem;
class ioArmorItem;
class ioHelmetItem;
class ioCloakItem;
class ioObjectItem;
class ioWearItem;
class ioBaseChar;
class ioSkill;

class ioEquipSlot
{
private:
	ioItem* m_EquipItemSlot[MAX_EQUIP_SLOT];
	ioSkill* m_EquipSkillSlot[MAX_EQUIP_SLOT];
	ioSkill* m_EquipSkillSlotW[MAX_EQUIP_SLOT];
	ioSkill* m_EquipAccessoryCompoundSkillSlot[MAX_ACCESSORY_SLOT];
	bool m_DropWaitSlot[MAX_EQUIP_SLOT];
	bool m_EquipWaitSlot[MAX_EQUIP_SLOT];
	int m_ItemDropRate;
	int m_WearDropRate;

	ioBaseChar *m_pOwnerChar;
	ioHashString m_DefaultUnderwearMesh; // body 
	
	ioHashString m_DefaultFaceMesh;
	ioHashString m_SkipFaceMesh;
	ioHashString m_CurFaceMesh;
	ioHashString m_DefaultFaceTexture;
	ioHashString m_FaceEffect;

	ioHashString m_DefaultHairMesh;
	ioHashString m_DefaultBindHairMesh;
	ioHashString m_DefaultHalfHairMesh;
	ioHashString m_DefaultBeardMesh;
	ioHashString m_DefaultHandMesh;
	ioHashString m_DefaultAccessoryMesh;

private:
	int m_iSetItemLev;

protected:
	struct EquipItemInfo
	{
		DWORD dwItemCode;
		DWORD dwItemMaleCustom;
		DWORD dwItemFemaleCustom;
		CustomTextureList vCustomTexList;
		EquipItemInfo()
		{
			dwItemCode = dwItemMaleCustom = dwItemFemaleCustom = 0;
		}
	};
	EquipItemInfo m_ItemSlot[MAX_EQUIP_SLOT];

public:
	void ChangeDefaultFace( const ioHashString &szFace, const ioHashString &szEffect );
	void ChangeDefaultHair( const ioHashString &szHair );
	void ChangeDefaultBeard( const ioHashString &szBeard );
	void ChangeDefaultAccessory( const ioHashString &szAccessory );
	void ChangeDefaultUnderwear( const ioHashString &szUnderwear ); // body

public:
	ioItem* EquipItem( ioItem *pItem, bool bThread = false );	

	ioItem* EquipWeapon( ioItem *pItem, bool bThread = false );
	ioItem* EquipArmor( ioItem *pItem, bool bThread = false );
	ioItem* EquipHelmet( ioItem *pItem, bool bThread = false );
	ioItem* EquipCloak( ioItem *pItem, bool bThread = false );
	ioItem* EquipObject( ioItem *pItem, bool bThread = false );
	ioItem* EquipWear( ioItem *pItem, bool bThread = false );
	//ioItem* EquipAccessory( ioItem *pItem, DWORD dwType, bool bThread = false );
	ioItem* EquipRing( ioItem *pItem, bool bThread = false );
	ioItem* EquipNecklace( ioItem *pItem, bool bThread = false );
	ioItem* EquipBracelet( ioItem *pItem, bool bThread = false );

	ioItem* EquipItemMesh( ioItem *pItem, bool bShow, bool bThread = false );

	ioItem* EquipWeaponMesh( ioItem *pItem, bool bShow, bool bThread = false );
	ioItem* EquipArmorMesh( ioItem *pItem, bool bShow, bool bThread = false );
	ioItem* EquipHelmetMesh( ioItem *pItem, bool bShow, bool bThread = false );
	ioItem* EquipCloakMesh( ioItem *pItem, bool bShow, bool bThread = false );
	ioItem* EquipObjectMesh( ioItem *pItem, bool bShow, bool bThread = false );
	ioItem* EquipWearMesh( ioItem *pItem, bool bShow, bool bThread = false );

	ioItem* EquipHelmetWearOnHelmetMesh( ioItem *pItem );

	void ChangeEquipedSkill( EquipSlot eSlot, ioItem *pOwnerItem );
	void ChangeEquipedSkill( EquipSlot eSlot, const ioHashString &rkSkillName );

	DWORD StopEquipedPassiveSkill( ioHashStringVec &rkNoStopList );
	void RestoreEquipedPassiveSkill( ioHashStringVec &rkNoStopList, const DWORD &dwID );

	bool CheckNoStopSkill( const ioHashString& szSkill, ioHashStringVec &rkNoStopList );

public:
	ioItem* ReleaseItem( EquipSlot eSlot );
	ioItem* ReleaseItem( DWORD dwItemCode, int iItemCreateIndex );

	ioItem* ReleaseWeaponSlot();
	ioItem* ReleaseArmorSlot();
	ioItem* ReleaseHelmetSlot();
	ioItem* ReleaseCloakSlot();
	ioItem* ReleaseObjectSlot();
	ioItem* ReleaseWearSlot();

	ioItem* ReleaseItemMesh( EquipSlot eSlot );
	ioItem* ReleaseWeaponMesh();
	ioItem* ReleaseArmorMesh();
	ioItem* ReleaseHelmetMesh();
	ioItem* ReleaseCloakMesh();
	ioItem* ReleaseObjectMesh();
	ioItem* ReleaseWearMesh();

	ioItem* ReleaseRingSlot();
	ioItem* ReleaseNecklaceSlot();
	ioItem* ReleaseBraceletSlot();

	void RestoreWeaponCustomize();
	void RestoreArmorCustomize();
	void RestoreHelmetCustomize();
	void RestoreCloakCustomize();

	bool IsEquipSlot( EquipSlot eSlot );
	
protected:   // 장비 텍스쳐 커스텀
	void EquipSlotData( int iSlot, DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom );
	void EquipSlotEntity( int iSlot, ioEntity *pEntity );
	void ReleaseSlotData( int iSlot );

public:
	bool IsCustomItemCheck( DWORD dwCustomIndex, bool bMale );
	void AddEntityEvent( ioEntity *pEntity );

public:
	void ClearItemOwnerByOutUser( const ioHashString &rkName );

public:
	void InitDefaultMeshNameList();
	void ChangeDefaultMeshNameList( const ioHashStringVec &rkNameList );
	int CheckDropItem( bool bMove );
	int CheckArmorDropItem();

private:
	ioEntity *AddEntityToOwner( const ioHashString &rkMeshName, bool bSex = false, bool bThread = false );
	bool RemoveEntityInOwner( const ioHashString &rkMeshName, bool bSex = false );

	void AddEquipedSkill( EquipSlot eSlot, ioItem *pOwnerItem );
	void RemoveEquipedSkillBuffInOwner( EquipSlot eSlot );
	void RemoveEquipedSkill( EquipSlot eSlot );

public:
	ioWeaponItem* GetWeapon() const;
	ioArmorItem* GetArmor() const;
	ioHelmetItem* GetHelmet() const;
	ioCloakItem* GetCloak() const;
	ioObjectItem* GetObject() const;
	ioWearItem* GetWear() const;

public:
	EquipSlot CheckDropWearItem( bool bDie );
	EquipSlot CheckDropObjectItem( bool bDie );

	ioItem* GetEquipedItem( int iSlot );
	ioItem* GetEquipedItemByCode( DWORD dwCode );
	ioItem* GetEquipedItem( const ioHashString &szItemName );
	int GetEquipedItemCnt();

	ioSkill* GetEquipedSkill( int iSlot );
	ioSkill* GetEquipedSkill( const ioHashString &szSkillName );
	ioSkill* GetUseEnableSkill( int iSlot, bool bHelpMsg );
	ioSkill* GetEquipedSetItemSkill();
	ioSkill* GetAccessoryCompoundSkill( int iSlot );

	void SetDropWaitSlot( int eSlot, bool bWait );
	void SetEquipWaitSlot( int eSlot, bool bWait );

	bool IsEquipWait();
	bool IsEquipWait( int eSlot );
	bool IsDropWait( int eSlot );

public:
	bool IsDefaultHair( const ioHashString &rkName ) const;
	void CheckCurHair( ioItem *pItem, bool bRemove );

	void ShowHeadMesh( ioItem *pItem, bool bShow );
	void ShowFaceMesh( bool bShow );

public:
	inline int GetSetItemLev() { return m_iSetItemLev; }

private:
	void UpdateSetItemLev();

	ioItem* EquipHelmetWearOnHelmet( ioItem *pItem );
	ioItem* EquipHelmetWearOnWear( ioItem *pItem );

	ioItem* EquipCloakWearOnCloak( ioItem *pItem );

	void UpdateDefaultFaceTextureName();

	bool IsHideHairMeshTiming();

public:
	void SetChangeEquipMesh( ioItem *pItem, bool bChangeEquip );

public:
	inline const ioHashString& GetDefaultFaceMesh() const { return m_DefaultFaceMesh; }
	inline const ioHashString& GetCurFaceMesh() const { return m_CurFaceMesh; }
	inline const ioHashString& GetDefaultFaceTexture() const { return m_DefaultFaceTexture; }

	inline const ioHashString& GetDefaultHairMesh() const { return m_DefaultHairMesh; }
	inline const ioHashString& GetDefaultBindHairMesh() const { return m_DefaultBindHairMesh; }
	inline const ioHashString& GetDefaultHalfHairMesh() const { return m_DefaultHalfHairMesh; }
	inline const ioHashString& GetDefaultBeardMesh() const { return m_DefaultBeardMesh; }
	inline const ioHashString& GetDefaultHandMesh() const { return m_DefaultHandMesh; }
	inline const ioHashString& GetDefaultAccessoryMesh() const { return m_DefaultAccessoryMesh; }
	inline const ioHashString& GetDefaultUnderwearMesh() const { return m_DefaultUnderwearMesh; } // body

	const ioHashString& GetCurHandMesh() const;
	const EquipItemInfo GetItemInfo( int iSlot );
	DWORD GetItemCustomIndex( bool bMale, int iSlot );


public:
	ioEquipSlot( ioBaseChar *pOwner );
	~ioEquipSlot();
};

