#pragma once

#include "../io3DEngine/ioOrientBox.h"
#include "../io3DEngine/ioEntityGroup.h"
#include "CharacterInfo.h"

class ioItem;
class ioEffect;
class ioEntity;

class ioCreateChar : public ioEntityEvent
{
public:
	enum CreateCharSlot
	{
		CCS_WEAPON,
		CCS_ARMOR,
		CCS_HELMET,
		CCS_CLOAK,
		MAX_CCS
	};

protected:
	ioEntityGroup *m_pGrp;
	RaceType m_RaceType;
	ioHashString m_Name;
	bool	m_bMale;
	bool	m_bThreadLoading;

	D3DCOLORVALUE m_SkinColor;
	D3DCOLORVALUE m_SkinOutLineColor;

	ioHashString m_HairColorTex;
	D3DCOLORVALUE m_HairOutLineColor;

	ioHashString m_BeardColorTex;
	D3DCOLORVALUE m_BeardOutLineColor;

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
	EquipItemInfo m_ItemSlot[MAX_CCS];

	ioHashString m_DefaultFaceMesh;
	ioHashString m_SkipFaceMesh;
	ioHashString m_CurFaceMesh;
	ioHashString m_FaceEffect;

	ioHashString m_DefaultUnderwearMesh; // body
	ioHashString m_DefaultHairMesh;
	ioHashString m_DefaultBindHairMesh;
	ioHashString m_DefaultHalfHairMesh;
	ioHashString m_DefaultHandMesh;

	ioHashString m_DefaultBeardMesh;	
	ioHashString m_DefaultEyeBrowMesh;
	ioHashString m_DefaultAccessoryMesh;	

	ioOrientBox m_WorldCollisionBox;

	ioHashString m_DefaultFaceTexture;

	ioHashString m_szChangeSkeleton;

	int   m_iFaceType;
	DWORD m_dwFaceChangeTime;

protected:
	DWORD     		m_dwMotionItemAniMaxTime;
	DWORD     		m_dwMotionItemAniCurTime;
	ioHashString	m_MotionItemAniName;
	bool            m_bMotionItemDelay;

public:
	void Initialize( RaceType eType, bool bMale, bool bThread );

	void SetName( const ioHashString &rkName ) { m_Name = rkName; }
	void ChangeSkeleton( const ioHashString &rkSkeleton );
	void SetSkinColor( const D3DCOLORVALUE &rkSkin, const D3DCOLORVALUE &rkOutLine );
	
	void SetHairColor( const ioHashString &szHairColorTex, const D3DCOLORVALUE &rkOutLine );
	void SetBeardColor( const ioHashString &szBeardColorTex, const D3DCOLORVALUE &rkOutLine );

	void InheritToNewChar( ioCreateChar *pNewChar );

	void SetMannequins( bool bSexClass );
	void SetDefaultFace( const ioHashString &szFace, const ioHashString &szEffect, int iFaceType );
	void SetDefaultHair( const ioHashString &szHair );
	void SetDefaultBeard( const ioHashString &szBeard );
	void SetDefaultAccessory( const ioHashString &szAccessory );
	void SetDefaultUnderwear( const ioHashString &szUnderwear ); // body

	void SetThreadLoading( bool bThread ){ m_bThreadLoading = bThread; }

public:
	virtual void AddEntityEvent( ioEntity *pEntity );

public:
	void EquipItem( DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumCode=0 );

protected:
	void EquipSlotData( int iSlot, DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom );
	void EquipSlotEntity( int iSlot, ioEntity *pEntity );
	void EquipWeapon( const ioItem *pItem, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom );
	void EquipArmor( const ioItem *pItem, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumeCode=0 );
	void EquipHelmet( const ioItem *pItem, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumeCode=0 );
	void EquipCloak( const ioItem *pItem, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumeCode=0 );

public:
	void ReleaseAllEquipItems();
	void ReleaseItem( int iSlot );

	void ReleaseEquipEffect( int iSlot );

protected:
	void ReleaseSlotData( int iSlot );
	void ReleaseWeaponSlot();
	void ReleaseArmorSlot( bool bDefault=true );
	void ReleaseHelmetSlot( bool bDefault=true );
	void ReleaseCloakSlot();

public:
	const ioItem* GetSlotItem( int iSlot );

protected:
	ioEntity *AddEntityToOwner( const ioHashString &rkMeshName, bool bSex = false );
	bool RemoveEntityInOwner( const ioHashString &rkMeshName, bool bSex = false );

	void UpdateBound();

	int GetAnimationIdxAfterConvertName( const ioHashString &szAniName, bool bMale ) const;
	void SetLoopAni( int iAniID, float fFadeIn = 0.0f );

public:
	void SetPosition( float fXPos, float fYPos, float fZPos );
	void SetPosition( const D3DXVECTOR3 &vPos );
	
	void SetPitch( float fPitch );
	void SetYaw( float fYaw );
	
	void RotateX( float fPitch );
	void RotateY( float fYaw );
	
	void SetScale( float fScale );
	void SetScale( const D3DXVECTOR3 &vScale );
	void SetScale( float fXScale, float fYScale, float fZScale );

public:
	void SetActionAni( const ioHashString &szAniName, float fFadeIn = FLOAT100, float fFadeOut = FLOAT100 );
	void SetLoopAni( const ioHashString &szAniName, float fFadeIn = 0.0f );
	bool SetWeaponShopMotion();
	ioHashString GetWeaponShopMotion();
	void SetAwardWeaponMotion( int iAniCode );
	void ClearAllAnimations( bool bNotifyClearEvent = false );
	void SetMotionItemAni( const ioHashString &rkMotion );

public:
	void DirectFaceChange( const char *szNewFaceName, int iDuration );
	void FaceChange( const char *szTexPostfix, int iDuration, const ioHashString &szType );
	void ChangeFaceTexture( const ioHashString &szNewFaceTexture );
	void RestoreDefaultFace();

public:
	void DestroyHandAndBody();
	bool DestroyEntity( const ioHashString &szName );
	bool DestroyEntity( int iIndex );

public:
	void ChangeHairOutLineThick( float fThick );
	void ChangeHeadOutLineThick( float fThick );
	void RestoreOutLineThick();

protected:
	void ApplyOutLineByChangedThick( ioEntity *pEntity, float fThick );

protected:
	void UpdateDefaultFaceTextureName();
	void CheckFaceChanging();
	void StopFaceChange();
	void UpdateMotionItemAni();

	RaceDetailType GetRaceDetailType() const;

public:
	ioEffect* AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner );
	void EndEffect( const ioHashString &rkEffect, bool bGrace );

public:
	DWORD GetEquipedItemCode( int iSlot ) const;
	int GetClassType();

	D3DXVECTOR3 GetWorldPosition() const;
	D3DXVECTOR3 GetMidPositionByRate( float fRate ) const;

	inline const ioHashString& GetName() const { return m_Name; }
	inline const ioOrientBox& GetWorldCollisionBox() const { return m_WorldCollisionBox; }
	inline ioEntityGroup* GetGroup() const { return m_pGrp; }

	float GetYaw();

	inline bool IsMale() const { return m_bMale; }
public:
	const ioHashString& GetDefaultHairMesh() const { return m_DefaultHairMesh; }
	const ioHashString& GetDefaultBindHairMesh() const { return m_DefaultBindHairMesh; }
	const ioHashString& GetDefaultHalfHairMesh() const { return m_DefaultHalfHairMesh; }
	const ioHashString& GetDefaultBeardMesh() const { return m_DefaultBeardMesh; }
	const ioHashString& GetDefaultUnderwearMesh() const { return m_DefaultUnderwearMesh; } // body
	const ioHashString& GetDefaultHandMesh() const { return m_DefaultHandMesh; }

public:
	void Update( float fTimePerSec );

public:
	bool IsCustomItemCheck( DWORD dwCustomIndex );
	void CustomTextureDownLoadComplete( const ioHashString &rkDstTex, const DWORD dwCustomIndex, const DWORD dwSubIndex );
    
public:
	void ReloadEquipItem( DWORD dwItemCode, DWORD dwMaleCustom, DWORD dwFemaleCustom );

	void ShowFaceMesh( bool bShow );

protected:
	CostumeInfo m_CostumeSlot[MAX_CCS];

public:
	void SetCostumeSlot( int nSlot, int nCostumeCode );
	const CostumeInfo* GetCostumeSlot( int nSlot );
	ioHashString GetCostumeEffect( const CostumeInfo* pCostumeInfo );

public:
	ioCreateChar( ioEntityGroup *pGrp );
	virtual ~ioCreateChar();
};

