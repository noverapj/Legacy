#pragma once


class ioINILoader;
class ioBaseChar;
class ioItemMaker;
class ioGrowthUpInfo;

#include "ioItemFeature.h"
#include "PlayEntityHelpStruct.h"
#include "ioItemExpert.h"
#include "CostumeEnumType.h"
#include "AccessoryEnumType.h"

#define ITEM_CREATE_NO_LIMIT -1

typedef struct tagChangeSkillInfo
{
	ioHashString m_BuffName;
	bool m_bChange;

	tagChangeSkillInfo()
	{
		m_bChange = false;
	}
} ChangeSkillInfo;

typedef std::vector<ChangeSkillInfo> ChangeSkillInfoList;

class ioItem
{
public:
	enum ItemType
	{
		IT_NONE,
		IT_WEAPON,
		IT_ARMOR,
		IT_HELMET,
		IT_CLOAK,
		IT_OBJECT,
		IT_WEAR,
		IT_RING,
		IT_NECKLACE,
		IT_BRACELET,
	};

	enum GaugeType
	{
		GT_NONE,
		GT_DAMAGE,
	};

	enum WeaponActionType
	{
		WAT_NONE,
		WAT_GRAFFITI,
	};

	enum ModeCrownType
	{
		MCT_NONE,
		MCT_HIDDEN_KING,
		MCT_DOUBLE_CROWN,
	};

protected:
	ioHashString m_GroupName;
	ioHashString m_TitleName;

	ModeCrownType m_CrownType;
	TeamType m_CrownTeamType;

	bool m_bVisible;
	ioItemMaker *m_pCreator;
	int		m_iItemGroupIndex;	
	DWORD	m_dwItemCode;

	int		m_iItemCreateIndex;
	DWORD	m_dwItemFeature;
	int		m_iItemCreateMaxLimit;

	CEncrypt<int> m_iReinforce;
	CEncrypt<DWORD> m_dwMaleCustom;
	CEncrypt<DWORD> m_dwFemaleCustom;

	StatAttributeType m_StatIndex;
	ioHashString m_DelayCameraBuff;

	float m_fCamHeight;
	float m_fCamDistance;
	float m_fCamFov;
	DWORD m_dwCamZoomTime;

	float m_fSkillCamHeight;
	float m_fSkillCamDistance;
	float m_fSkillCamFov;
	DWORD m_dwSkillCamZoomTime;

	float m_fRunCamHeight;
	float m_fRunCamDistance;
	float m_fRunCamFov;
	DWORD m_dwRunCamZoomTime;
	
	float m_fChargingCamHeight;
	float m_fChargingCamDistance;
	float m_fChargingCamFov;
	DWORD m_dwChargingCamZoomTime;

	float m_fArmorClass;
	float m_fSpeedClass;

	ioHashString m_Name;
	ioHashString m_SkillName;

	ioHashString m_ItemEquipDefaultEffect;
	ioHashString m_ItemEquipEffect;
	ioHashString m_ItemEquipEffectHw;
	ioHashString m_CurItemEquipEffect;

	ioHashString m_ExtendEquipEffect;
	ioHashString m_ExtendEquipEffectHw;
	ioHashString m_CurExtendEquipEffect;

	ioHashString m_DropEffect;
	ioHashString m_FieldEffect;
	ioHashString m_PickEnableEffect;

	ioHashString m_ProtectionEffect;

	ioHashString m_FieldEntityGrp;
	ioHashString m_ItemLargeIconFile;

	typedef std::map< DWORD, ioHashString > DecoInfoMap;
	DecoInfoMap m_FieldEntityGrpMap;
	DecoInfoMap m_ItemLargeIconMap;

	typedef std::map< DWORD, ioHashString > SkillInfoMap;
	SkillInfoMap m_SkillNameMap;

	D3DCOLORVALUE m_GlowColor;

	ioHashString m_OwnerName;

	DWORD m_dwSetItemCode;

	ioHashString m_ItemDesc;
	ioHashString m_FuncDesc;

	bool m_bAimedBall;
	ioHashString m_AimedTarget;

// change skill by buff
	CEncrypt<float> m_fChangeSkillHPRate;
	CEncrypt<float> m_fChangeSkillExtendGauge;

	ioHashString m_ChangeSkillBuff;
	ioHashString m_ChangeSkillNameByBuff;
	
	ioHashString m_ChangeSkillEffect;
	UniqueObjID m_dwChangeSkillEffect;

	ChangeSkillInfoList m_ReserveChangeSkillInfo;

// For GrowthInfo
	IntVec m_vGrowthInfoNum;
	
// For Skill Gauge
	GaugeType m_SkillGaugeType;
	CEncrypt<bool> m_bHasSkillGauge;
	CEncrypt<bool> m_bUpdateEnableSkillGauge;
	CEncrypt<float> m_fMaxSkillGauge;
	CEncrypt<float> m_fCurSkillGauge;
	CEncrypt<float> m_fRecoverSkillGauge;
	CEncrypt<float> m_fDefaultGauge;
	CEncrypt<DWORD> m_dwTicCheckTime;

// For Circle OffSet
	float m_fMagicCircleOffSet;
	float m_fMagicCircleYOffSet;

// For ExtraItem
	int m_iEnableClass;
	int m_iManual;

	CEncrypt<int> m_iCharOnlySexType;

// For Animation
	//ioHashStringVec m_AnimationList;

// For Equip Buff
	ioHashStringVec m_EquipBuffList;

	float m_fStartSkillGaugeRate;

protected:
	// weapon action
	WeaponActionType m_WeaponActionType;
	CEncrypt<int> m_iWeaponActionMaxCnt;

protected:
	CostumeInfo m_CostumeInfo;
	DWORD m_dwCostumeMaleCustom;
	DWORD m_dwCostumeFemaleCustom;

protected:
	int m_nGradeType;

protected:       // Decoration Info
	virtual const ioHashString& FindFieldEntityGrp() const;
	const ioHashString& FindLargeIconName() const;
	const ioHashString& FindSkillName( int iIndex ) const;
	bool HasSkillName( const ioHashString &szName );

	void SkillNameMapShuffle();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ItemType GetType() const = 0;
	virtual ioItem* Clone() = 0;
	
	virtual bool UseCreateIndexItem() const;
	virtual DWORD GetSetFocusColor() const;

	void SetGroupName( const ioHashString& szGroupName );
	void SetTitleName( const ioHashString& szTitleName );

	void ReLoadProperty();

	virtual void Initialize( ioBaseChar *pOwner, ioPlayStage* pStage );
	virtual void InitializeBeforeEquip( ioBaseChar *pOwner, ioPlayStage* pStage );

	float GetStartSkillGaugeRate(){ return m_fStartSkillGaugeRate; }

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetExtendEquipEffect( ioBaseChar *pOwner );
	virtual void ReleaseExtendEquipEffect( ioBaseChar *pOwner );

	virtual void AddGlowLine( ioBaseChar *pOwner );
	virtual void RemoveGlowLine( ioBaseChar *pOwner );

	virtual ioHashString GetTargetName();
	virtual void ClearTarget();
	virtual void SetItemType();

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner );
	virtual void CheckWoundedState( ioBaseChar *pOwner );

public:
	inline void SetItemCreateIndex( int dwIndex ) 
	{
		m_iItemCreateIndex = dwIndex; 
	}
	inline void SetOwnerName( const ioHashString &rkName ) { m_OwnerName = rkName; }
	void SetCreator( ioItemMaker *pCreator );

	void ClearIfOwnerName( const ioHashString &rkUser );
	void EraseOwnerName();

public:
	inline int   GetItemGroupIndex() const { return m_iItemGroupIndex; }
	inline StatAttributeType GetStatIndex() const { return m_StatIndex; }
	inline DWORD GetItemCode() const { return m_dwItemCode; }

	inline int	 GetItemCreateIndex() const { return m_iItemCreateIndex; }
	inline int   GetItemCreateLimit() const { return m_iItemCreateMaxLimit; }

	inline const ioHashString& GetName() const { return m_Name; }
	inline const ioHashString& GetFieldEntityGrp() const { return m_FieldEntityGrp; }
	inline const ioHashString& GetFieldEffect() const { return m_FieldEffect; }
	inline const ioHashString& GetPickEnableEffect() const { return m_PickEnableEffect; }
	inline const ioHashString& GetDropEffect() const { return m_DropEffect; }
	inline const ioHashString& GetProtectionEffect() const { return m_ProtectionEffect; }

	const ioHashString& GetItemEquipEffect() const;
	const ioHashString& GetItemEquipEffect( bool bMale ) const;
	
	inline const ioHashString& GetOwnerName(){ return m_OwnerName; }

	inline bool HasFieldEffect() const { return !m_FieldEffect.IsEmpty(); }
	inline bool HasPickEnableEffect() const { return !m_PickEnableEffect.IsEmpty(); }
	inline bool HasDropEffect() const { return !m_DropEffect.IsEmpty(); }

	inline bool HasOwnerName() const { return !m_OwnerName.IsEmpty(); }
	bool HasFeature( DWORD dwFeature ) const;

	inline const ioHashString& GetItemLargeIconName() const { return m_ItemLargeIconFile; }
	inline const ioHashString& GetDelayCameraBuff() const { return m_DelayCameraBuff; }

	inline float GetItemCamHeight() const { return m_fCamHeight; }
	inline float GetItemCamDistance() const { return m_fCamDistance; }
	inline float GetItemCamFov() const { return m_fCamFov; }
	inline DWORD GetItemCamZoomTime() const { return m_dwCamZoomTime; }

	inline float GetSkillCamHeight() const { return m_fSkillCamHeight; }
	inline float GetSkillCamDistance() const { return m_fSkillCamDistance; }
	inline float GetSkillCamFov() const { return m_fSkillCamFov; }
	inline DWORD GetSkillCamZoomTime() const { return m_dwSkillCamZoomTime; }

	inline float GetRunCamHeight() const { return m_fRunCamHeight; }
	inline float GetRunCamDistance() const { return m_fRunCamDistance; }
	inline float GetRunCamFov() const { return m_fRunCamFov; }
	inline DWORD GetRunCamZoomTime() const { return m_dwRunCamZoomTime; }

	inline float GetChargingCamHeight() const { return m_fChargingCamHeight; }
	inline float GetChargingCamDistance() const { return m_fChargingCamDistance; }
	inline float GetChargingCamFov() const { return m_fChargingCamFov; }
	inline DWORD GetChargingCamZoomTime() const { return m_dwChargingCamZoomTime; }

	float GetItemArmorClass( const ioHashString &szName );
	float GetItemArmorClassNPC( const ioHashString &szName );

	inline void AddItemArmorClass( float fAddArmorClass ){ m_fArmorClass += fAddArmorClass; }
	inline void AddItemSpeedClass( float fAddSpeedClass ){ m_fSpeedClass += fAddSpeedClass; }

	inline DWORD GetSetItemCode() const { return m_dwSetItemCode; }

	inline const ioHashString& GetDescription() const { return m_ItemDesc; }
	inline const ioHashString& GetFuncDescription() const { return m_FuncDesc; }

	inline ModeCrownType GetCrownType() const { return m_CrownType; }
	inline TeamType	GetItemTeamType() const { return m_CrownTeamType; }
	
	virtual void SetVisible( ioBaseChar *pOwner, bool bVisible );
	inline bool IsVisible() const { return m_bVisible; }

	inline float GetMagicCircleOffSet() const { return m_fMagicCircleOffSet; }
	inline float GetMagicCircleYOffSet() const { return m_fMagicCircleYOffSet; }

	inline void SetItemReinforce( int iReinforce ) { m_iReinforce = iReinforce; }
	inline int GetItemReinforce() const { return m_iReinforce; }

	inline const ioHashString GetAimedTarget() { return m_AimedTarget; }
	inline int GetSkillGaugeType() const { return m_SkillGaugeType; }
	void SetItemCustom( DWORD dwMaleCustom, DWORD dwFemaleCustom );
	DWORD GetItemMaleCustom() const;
	DWORD GetItemFemaleCustom() const;
	DWORD GetItemMaleCustombyBase() const;
	DWORD GetItemFemaleCustombyBase() const;

	// 스킬 공유 (현재는 배틀모드에서만 사용)
	void ShareSkillGauge();

public:
	inline bool HasSkill() const { return !m_SkillName.IsEmpty(); }
	inline const ioHashString& GetSkillName() const { return m_SkillName; }

	void SetSkillName( const ioHashString &rkSkillName );
	bool GetExtendSkillName( OUT ioHashString& szExtendSkill );

	void CheckChangeSkillByBuff( ioBaseChar *pOwner );
	void CheckChangeSkillByHP( ioBaseChar *pOwner );
	void CheckChangeSkillByGauge( ioBaseChar *pOwner );
	void CheckReserveChangeSkillByBuff( ioBaseChar *pOwner );

	void ReserveChangeSkillByBuff( const ioHashString &rkBuffName, bool bChange );
	void ChangeSkillByBuff( ioBaseChar *pOwner, const ioHashString &rkBuffName, bool bChange );
	void ChangeSkillByHP( ioBaseChar *pOwner );
	void ChangeSkillByGauge( ioBaseChar *pOwner );

	bool ApplyCurSkillName( ioBaseChar *pOwner, const ioHashString &szName );

// For Skill Gauge
public:
	inline bool HasSkillGauge() const { return m_bHasSkillGauge; }
	float GetMaxSkillGauge( const ioHashString &szName );
	float GetExtendSkillGauge( const ioHashString &szName );
	float GetMaxExtendSkillGauge( const ioHashString &szName );
	inline float GetCurSkillGuage() const { return m_fCurSkillGauge; }
	inline float GetRecoverySkillGuage() const { return m_fRecoverSkillGauge; }

	void ResetTicCheck();
	void UpdateRecoverySkillGauge( ioBaseChar *pOwner );
	bool IsEnableUpdateSkillGauge( const ioHashString &szName );

	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar ) { return true; }
	virtual float GetExtraGauge() { return FLOAT1; }
	virtual void ReduceNeedGauge( float fUseGauge, ioBaseChar *pOwner );
	
	void ReduceSkillGauge( float fUseGauge );
	void SetCurSkillGauge( float fCurGauge );

protected:
	void UpdateNormalSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );

// For Bullet
public:
	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual int GetCurSecoundBullet();
	virtual int GetMaxSecoundBullet();
	virtual void SetCurSecoundBullet( int iCurBullet );

public:
	virtual int GetGrowthInfoNum( int iGrowthNum ) const;
	virtual int GetGrowthInfoNumByType( int iType, int &iGrowthNum ) const;

	int GetGrowthLevel( const ioHashString &szName );
	ioGrowthUpInfo* GetGrowthUpInfoByType( int iType, const ioHashString &szName );

public:
	inline int GetEnableClass() const { return m_iEnableClass; }
	inline int GetManual() const { return m_iManual; }

// weapon action
public:
	inline WeaponActionType CheckWeaponActionType() const { return m_WeaponActionType; }

	int GetGraffitiWeaponActionMax();


public:
	virtual void ClearCurChargeRateForSkill();

	virtual float GetChargeDamageRateForSkill();
	virtual float GetChargeForceRateForSkill();
	virtual float GetChargeBlowRateForSkill();
	virtual float GetChargeRangeRateForSkill();
	virtual float GetChargeSpeedRateForSkill();
	virtual float GetChargeFloatRateForSkill();
	virtual float GetChargeScaleRateForSkill();
	virtual float GetChargeExplosionRateForSkill();
	virtual float GetChargeWoundedTimeRateForSkill();

public:
	//void PrepareLoadingAnimationList( const ioHashString &rkAniName );

	int GetCharOnlySexType() const;

public:
	inline float GetBaseMaxGauge() const { return m_fMaxSkillGauge; }

	inline float GetBaseArmorClass() const { return m_fArmorClass; }
	inline float GetBaseSpeedClass() const { return m_fSpeedClass; }

public:
	virtual void SetCurEquipEffect( bool bMale );
	virtual void SetCostume( int nCostumeCode, bool bIsOwnerChar );
	void SetCostumeCustom( DWORD dwMaleCustom, DWORD dwFemaleCustom );
	int GetCostumeCode();

public:
	inline int GetGradeType() const { return m_nGradeType; }
	
	//액세서리
protected:
	int m_iAccessoryRandomNum;
	int m_iAccessoryCompoundCode;
	int m_iAccessoryCompoundValue;

public:
	void SetAccessoryRandomNum( int iNum ) { m_iAccessoryRandomNum = iNum; }
	const int& GetAccessoryRandomNum() const { return m_iAccessoryRandomNum; }
	void SetAccessoryCompoundInfo( int iCode, int iValue ) { m_iAccessoryCompoundCode = iCode;	m_iAccessoryCompoundValue = iValue; }
	const int& GetAccessoryCompoundCode() const { return m_iAccessoryCompoundCode; }
	const int& GetAccessoryCompoundValue() const { return m_iAccessoryCompoundValue; }

public:
	ioItem();
	ioItem( const ioItem &rhs );
	virtual ~ioItem();
};

typedef std::list< ioItem* > ItemList;
typedef std::vector< ioItem* > ItemVector;
typedef std::vector< const ioItem* > ConstItemVector;

