#pragma once

#include "ioAttackableItem.h"

class ioINILoader;
class ioWeapon;
class ioAniCollisionBoxGrp;

typedef struct tagSummonAttackInfo
{
	DWORD m_dwCreateTime;
	ioHashString m_SummonEffect;
	ioHashString m_SummonSound;
	AttackAttribute m_SummonAttribute;

	tagSummonAttackInfo()
	{
		m_dwCreateTime = 0;
		m_SummonAttribute.Init();
	}

} SummonAttackInfo;

typedef std::vector< SummonAttackInfo > SummonAttackInfoList;


class ioWeaponItem : public ioAttackableItem
{
public:
	enum WeaponSubType
	{
		WST_CHARGE_CONTACT,
		WST_FIRE,
		WST_HOOK,
		WST_MAGIC_STAFF,
		WST_HEAL_STAFF,
		WST_BOW,
		WST_MINE,
		WST_OBJECT,
		WST_LASER,
		WST_SHIELD,
		WST_CREATE_STAFF,
		WST_POWER_CHARGE,
		WST_AUTO_SHOT,
		WST_CHARGE_AREAWEAPON,
		WST_CHARGE_SHOT,
		WST_COUNT_CHARGE,
		WST_RUN_CHARGE,
		WST_RANGE_CHARGE,
		WST_ROTATE_CHARGE,
		WST_SUMMON_CHARGE,
		WST_LINEAR_CHARGE,
		WST_COUNT_CHARGE2,
		WST_SHIELD2,
		WST_SHIELD3,
		WST_SUMMON_CHARGE2,
		WST_POWER_CHARGE2,
		WST_GRAPPLING,
		WST_DUMMY_CHAR,
		WST_CROSS_BOW,
		WST_CHARGE_MOVE,
		WST_LINEAR_CHARGE2,
		WST_POWER_CHARGE3,
		WST_CHARGE_MOVE2,
		WST_POWER_CHARGE4,
		WST_MAGIC_STAFF2,
		WST_UROBORUS,
		WST_RUN_CHARGE2,
		WST_RUN_CHARGE3,
		WST_WEREWOLF,
		WST_POWER_CHARGE5,
		WST_CONSECUTION,
		WST_BUFF_ADD,
		WST_GATOTKACA,
		WST_RACHEL,
		WST_ILJIMAE,
		WST_ALTER_EGO,
		WST_WITCH,
		WST_EXCALIBUR,
		WST_MECHANICS,
		WST_CHARGE_COMBO,
		WST_POWER_CHARGE6,
		WST_CHARGE_COMMAND,
		WST_CHARGE_CONTACT2,
		WST_PHANTOM,
		WST_MAGIC_STAFF3,
		WST_YUMIRA_ITEM,
		WST_LUCIFER,
		WST_CHARGE_COMBO_BOW,
		WST_NAKORURU,
		WST_DESTROYER,
		WST_MICHAEL,
		WST_CHARGE_SHOT2,
		WST_GOOMIHO,
		WST_PETERPAN,
		WST_SUCCUBUS,
		WST_CYVOGUE,
		WST_TRINITY,
		WST_WIND_LORD,
		WST_SHAOLIN,
		WST_ANUBIS,
		WST_SCATHIITEM,
		WST_GHOST_STEALER,
		WST_JINMORI_ITEM,
		WST_DRACULA,
		WST_PROJECT_K,
		WST_POWER_CHARGE_SHIELD,
		WST_HAKUMEN,
		WST_DEVIL,
		WST_MAIDITEM,
		WST_FLASHITEM,
		WST_EVY_ITEM,
		WST_PSYCO_ITEM,
		WST_SUDDEN_ATTACK_ITEM,
		WST_SLASHER_ITEM,
		WST_DUMMY_CHAR2,
		WST_MAGIC_STAFF4,
		WST_TITAN_ITEM,
		WST_SUPER_DIMENSION_ITEM,
		WST_TRACE_ITEM,
		WST_KAELTHAS_ITEM,
		WST_GALAXY_ITEM,
		WST_DRAGON_SLAYER_ITEM,
		WST_NANGIN_ITEM,
		WST_STRIKER_ITEM,
		WST_DON_QUIXOTE_ITEM,
		WST_DRAGONNEST_ASSASASSIN_ITEM,
		WST_THUNDER_BIRD_ITEM,
		WST_ALICE_ITEM,
		WST_LEE_ITEM,
		WST_VALKYRIE,
		WST_PARKILPYO_ITEM,
		WST_JORO_EXPANSION_ITEM,
		WST_EARTHQUAKE,
		WST_PENTJAK_SILAT_ITEM,
		WST_JACK_ITEM,
		WST_SHADOWRANGER_ITEM,
		WST_NINJA_EXPANSION_ITEM,
		WST_DARKKNIGHT_ITEM,
		WST_JUMPER_ITEM,
		WST_ROBIN_EXPANSION_ITEM,
		WST_ENGINEER_ITEM,
		WST_GEOMJE_ITEM,
		WST_FIRE_MAGE_EXPANSION_ITEM,
		WST_ZEUS_ITEM,
		WST_SABER_ITEM,
		WST_OUTBOXER_ITEM,
		WST_ANGEL_OF_DEATH_ITEM,
		WST_HADES_ITEM,
		WST_MAGIC_SWORD_ITEM,
		WST_NATA_ITEM,
		WST_TIGER_EXPASION_ITEM,
		WST_LEESUNSIN_ITEM,
		WST_RESEARCHER,
		WST_BLADE_ITEM,
		WST_GRAPPLING_EXPANSION_ITEM,
		WST_BULLFIGHT_ITEM,
		WST_JEONWOOCHI_ITEM,
		WST_SAWANG_ITEM,
		WST_CHEMISTRY_ITEM,
		WST_CHAIN_MAGICIAN_ITEM,
		WST_RED_HOOD_ITEM,
		WST_BOW2,
		WST_STRIDER_ITEM,
		WST_NEPHILIM_ITEM,
		WST_APOSTATE_MONK_ITEM,
		WST_MUTANT_ITEM,
		WST_GANESA,
		WST_SAMURAI_EXPANSION_ITEM,
		WST_MIR_WARRIOR,
		WST_CUCHULAIN_ITEM,
		WST_GALFORD_ITEM,
		WST_ONIMUSHA_ITEM,
		WST_WESTGUN_EXTEND_ITEM,
		WST_DRUID,
		WST_SASINMU,
		WST_DARKSEAS_ITEM,
		WST_PREDATOR_ITEM,
		WST_CROW_ITEM,
		WST_SUMMON_CHARGE3,
		WST_BLACK_SPEAR_ITEM,
		WST_MEPHISTO,
		WST_GUNNER,
		WST_TRAP_ITEM,
		WST_BANGTAN_ITEM,
		WST_WITCH_DOCTOR,
		WST_SURFER,
		WST_BUBBLE_FIGHTER,
		WST_URIEL_ITEM,
		WST_PATRIOT_ITEM,
		WST_WINDGOD_ITEM,
		WST_WARP_ITEM,
		WST_DRILL_ITEM,
		WST_STRANGER_ITEM,
		WST_GARGOYLE_ITEM,
		WST_HACKER_ITEM,
		WST_CANNON_ITEM,
		WST_MAGICENGINEER_ITEM,
		WST_JINRAH_ITEM,
		WST_KUNGFU_ITEM,
		WST_NIGHTMARE_ITEM,
		WST_AZAZEL_ITEM,
		WST_MEGA_AGENT_ITEM,
		WST_NEO_ITEM,
		WST_SANTA_ITEM,
		WST_JACK_FROST_ITEM,
		WST_AUTOMATON_ITEM,
		WST_GARDENER_ITEM,
		WST_BASEBALL_ITEM,
		WST_UFC_ITEM,
		WST_SOCCER_ITEM,
		WST_VULCAN_ITEM,
		WST_BUBBLE_ITEM,
		WST_CHANGE_ON_ITEM,
		WST_MURIMGOSU_ITEM,
		WST_GHOST_ITEM,
		WST_SLOPE_ITEM,
		WST_ODIN_ITEM,
		WST_HANZO_ITEM,
		WST_PHOENIXBOW_ITEM,
		WST_TIMEGATE_ITEM,
		WST_OMEN_ITEM,
		WST_MAD_BLASTER_ITEM,
		WST_DR_SPIDER_ITEM,
		WST_LUPIN_ITEM,
		WST_MIDNIGHT_ITEM,
		WST_ROCKET_LAUNCHER_ITEM,
		WST_DIMENSIONIST_ITEM,
		WST_PAPARAZZI_ITEM,
		WST_TYR_ITEM,
		WST_NUCLEARSOLDIER_ITEM,	// 2020-01-29 신규용병 레어(뉴클리어 솔져) 추가
		WST_FROZEN_ITEM,			// 2020-03-02 신규용병 얼음신 추가 
		WST_POSEIDON_ITEM,
		WST_SKY_WALKER_ITEM,
		WST_GWONWANG_ITEM,			// 2020-05-21 신규용병 레어(권왕) 추가
		WST_GENISYSARMY_ITEM,		// 2020-06-25 신규용병 레어(제네시스) 추가
		WST_LILLY_ITEM,
		WST_HERACLESS_ITEM,
		WST_SIZE_ITEM,				// 2020-09-14 신규용병 레어(사이즈) 추가 
		WST_EVA_ITEM,				// 2020-10-16 신규용병 레어(이브) 추가
		WST_MAGNETIC_ITEM,			// 2020-11-19신규용병 레어(마그네틱) 추가
		WST_TAOIST_ITEM
	};

protected:
	ioHashString m_EquipMesh;
	DecoInfoMap  m_EquipMeshMap;

	ioHashString m_CurEquipMesh;
	ioHashString m_ChangeEquipMesh;

	ioHashString m_szReadyMarker;
	ioHashString m_szAimingMarker;
	ioHashString m_szAimedMarker;
	ioHashString m_szZeroAimedMarker;

	ioHashString m_StartMotion;
	ioHashString m_UIMotion;

	ioHashString m_WinMotion;
	ioHashString m_LoseMotion;
	ioHashString m_DrawMotion;

	ioHashString m_SpecialMotion;
	DWORD m_dwSpecialMotionCamera;

	ioHashStringVec m_AwardMotionList;

	ioAniCollisionBoxGrp *m_pAniCollisionGrp;
	ioAniCollisionBoxGrp *m_pChangeAniCollisionGrp;
	ioAniCollisionBoxGrp *m_pCurAniCollisionGrp;

	float m_fCollisionBoxScale;
	float m_fCollisionBoxOffset;

	CEncrypt<DWORD> m_dwChargeTime;

	int m_iWeaponSoundType;

	ioHashString m_DieMotion;
	ioHashString m_DieBackMotion;
	ioHashString m_BlowDieAction;
	ioHashString m_BackBlowDieAction;
	ioHashString m_DownBlowDieAction;
	ioHashString m_BackdownBlowDieAction;
	ioHashString m_KillEffect;

// Decoration Info
protected:
	const ioHashString& FindEquipMesh() const;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ItemType GetType() const;
	virtual void SetItemType();

public:
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnReleasedByEndGame( ioBaseChar *pOwner );

	virtual void AddGlowLine( ioBaseChar *pOwner );
	virtual void RemoveGlowLine( ioBaseChar *pOwner );	
	virtual void ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject );

	virtual void WeaponWounded( ioWeapon *pWeapon, ioBaseChar *pWoundChar);

	virtual bool IsCmdCheckEnableTime();
	virtual bool IsCmdSuccess();

public:
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableJumpKeyEtcAction( ioBaseChar *pOwner );

public:
	void UpdateChargeTime( ioBaseChar *pOwner );

	void SetChangeEquipMesh( bool bChangeEquip );

public:
	inline ioAniCollisionBoxGrp* GetAniCollisionBoxGrp() const { return m_pCurAniCollisionGrp; }
	inline float GetCollisionBoxScale() const { return m_fCollisionBoxScale; }
	inline float GetCollisionBoxOffset() const { return m_fCollisionBoxOffset; }

	//inline const ioHashString& GetEquipMesh( ioBaseChar *pOwner ) const { return m_CurEquipMesh; }
	virtual const ioHashString& GetEquipMesh( ioBaseChar *pOwner ) const;
	inline const ioHashString& GetStartMotion() const { return m_StartMotion; }
	inline const ioHashString& GetUIMotion() const { return m_UIMotion; }

	inline const ioHashString& GetWinMotion() const { return m_WinMotion; }
	inline const ioHashString& GetLoseMotion() const { return m_LoseMotion; }
	inline const ioHashString& GetDrawMotion() const { return m_DrawMotion; }

	const ioHashString& GetAwardMotion( int iAniCode ) const;

	inline bool HasWinMotion() const { return !m_WinMotion.IsEmpty(); }
	inline bool HasLoseMotion() const { return !m_LoseMotion.IsEmpty(); }
	inline bool HasDrawMotion() const { return !m_DrawMotion.IsEmpty(); }

	inline int GetWeaponSoundType() { return m_iWeaponSoundType; }

	inline const ioHashString& GetSpecialMotion() const { return m_SpecialMotion; }
	inline DWORD GetSpecialMotionCamera() const { return m_dwSpecialMotionCamera; }

	virtual bool IsCanSendControl() const;

	inline const ioHashString& GetDieMotion() const { return m_DieMotion; }
	inline const ioHashString& GetDieBackMotion() const { return m_DieBackMotion; }
	inline const ioHashString& GetBlowDieAction() const { return m_BlowDieAction; }
	inline const ioHashString& GetBackBlowDieAction() const { return m_BackBlowDieAction; }
	inline const ioHashString& GetDownBlowDieAction() const { return m_DownBlowDieAction; }
	inline const ioHashString& GetBackdownBlowDieAction() const { return m_BackdownBlowDieAction; }
	inline const ioHashString& GetKillEffect() const { return m_KillEffect; }

public:
	virtual WeaponSubType GetSubType() const = 0;

public:
	ioWeaponItem();
	ioWeaponItem( const ioWeaponItem &rhs );
	virtual ~ioWeaponItem();
};

inline ioWeaponItem* ToWeaponItem( ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_WEAPON )
		return NULL;

	return dynamic_cast< ioWeaponItem* >( pItem );
}

inline const ioWeaponItem* ToWeaponItemConst( const ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_WEAPON )
		return NULL;

	return dynamic_cast< const ioWeaponItem* >( pItem );
}
