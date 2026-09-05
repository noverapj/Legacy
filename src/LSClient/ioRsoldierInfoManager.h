#pragma once

enum RSoldierType
{
	DEFAULT_R_CHAR_CODE = 500,
	DEFAULT_SET_CHAR_CODE = 800,
};

enum SoldierSoundType
{
	SST_Start,
	SST_WeaponSkill,
	SST_ArmorSkill,
	SST_HelmetSkill,
	SST_CloakSkill,
	SST_Kill,
	SST_Die,
};

typedef struct tagRandomApplyBuff
{
	int m_nApplyChance;
	int m_nMaxChance;
	ioHashStringVec m_AttackOwnerBuff;
	ioHashStringVec m_WoundOwnerBuff;
}RandomApplyBuff;

typedef std::map<int, RandomApplyBuff> RandomApplyBuffList;

typedef struct tagSoldierSetBuff
{
	int m_nSoldierSetCnt;
	IntVec m_nAddStat;
}SoldierSetBuff;

typedef std::vector<SoldierSetBuff> SoldierSetBuffList;

typedef std::map<int, SoldierSetBuffList> SoldierSetDataList;

typedef struct tagSoldierSoundData
{
	ioHashStringVec m_StartSound;
	ioHashStringVec m_WeaponSkillSound;
	ioHashStringVec m_ArmorSkillSound;
	ioHashStringVec m_HelmetSkillSound;
	ioHashStringVec m_CloakSkillSound;
	ioHashStringVec m_KillSound;
	ioHashStringVec m_DieSound;
}SoldierSoundData;

typedef std::map<int, SoldierSoundData> SoldierSoundDataList;

typedef struct tagSoldierDefaultDeco
{
	int m_nSex;	
	int m_nFace;
	int m_nHair;
	int m_nSkinColor;
	int m_nHairColor;
	int m_nUnderwear;
}SoldierDefaultDeco;

typedef std::map<int, SoldierDefaultDeco> SoldierDefaultDecoList;

class ioRSoldierInfoManager : public Singleton< ioRSoldierInfoManager >
{
public:
	static ioRSoldierInfoManager& GetSingleton();

protected:
	std::vector<IntVec> m_vRSoldierAddStateList;

protected:
	IntVec m_vRandomBuffDisbleModeType;
	RandomApplyBuffList m_RandomApplyBuffList;

protected:
	int m_nSoldierSetDataMaxCnt;
	std::map<int, int> m_SoldierSetNumList;
	SoldierSetDataList m_SoldierSetDataList;

protected:
	SoldierSoundDataList m_SoldierSoundDataList;
	ioHashString m_szCurPlaySound;

protected:
	SoldierDefaultDecoList m_SoldierDefaultDecoList;

public:
	void Initialize();
	void LoadRSoldier();

	int GetSoldierAddStat(int iIndex, int iCurRSoldierCount );
	bool IsRSoldierType( const CHARACTER& rkChar );

protected:
	void LoadRandomApplyBuff();
	void LoadOwnerBuff( ioINILoader &rkLoader, const char* szName, ioHashStringVec &BuffList );

public:
	bool CheckEnableBuff( ioBaseChar* pOwner );
	void ApplyRandomBuff( ioBaseChar *pAttackChar, ioBaseChar *pWoundChar );
	void OnRandomBuff( SP2Packet &rkPacket, ioPlayStage *pStage );

protected:
	void LoadSoldierSet();

protected:
	bool GetSoldierSetData( int nCode, SoldierSetBuffList& vSoldierSetBuffList );

public:
	int GetSoldierSetNum( int nCode );
	int GetSoldierSetAddStat( int nIndex, int nCode, int nCnt );
	int GetSoldierSetDataMaxCnt() { return m_nSoldierSetDataMaxCnt; }
	bool IsSoldierSet( const CHARACTER& rkChar );

protected:
	void LoadSoldierSound();
	void LoadSound( ioINILoader &rkLoader, const char* szName, ioHashStringVec &SoundList );

public:
	void PlaySoldierSound( ioBaseChar *pChar, SoldierSoundType eType );

protected:
	ioHashString GetRandomSound( const ioHashStringVec &SoundList );

protected:
	void LoadSoldierDefaultDeco();

public:
	bool GetSoldierDefaultDeco( CHARACTER &rkCharInfo );

public:
	ioRSoldierInfoManager(void);
	virtual ~ioRSoldierInfoManager(void);
};

#define g_RSoldierInfoManager ioRSoldierInfoManager::GetSingleton()