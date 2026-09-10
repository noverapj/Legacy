#pragma once


class ioNpcChar;
class ioEntityGroup;
class ioPlayStage;
class ioPlayMode;
class ioAI;
class ioChampionAI;

class ioAIDatabase;

class ioNpcMgr  : public Singleton< ioNpcMgr >
{
protected:
	struct FieldNPCData
	{
		ioHashString m_szNpcViewName;
		ioHashString m_szGradeTitle;
		ioHashString m_szNpcInfo_ini;
		ioHashString m_szNpcRaceInfo_ini;
		ioHashString m_szNpcAI_ini;
		ioHashString m_szNpcAI_xml;

		int	m_iNpcType;
		int	m_iRandomAppearance;
		IntVec m_vEquipItemList;
		FieldNPCData()
		{
			m_iNpcType = 0;
			m_iRandomAppearance = 0;
		}
	};
	typedef std::map< DWORD, FieldNPCData * > FieldNPCDataMap;
	FieldNPCDataMap m_FieldNPCMap;

protected:
	DWORD  m_dwGradeUpTime;
	struct GradeTable
	{
		int m_iLimitGrade;
		int m_iMinimumRand;
		int m_iMaximumRand;
		GradeTable()
		{
			m_iLimitGrade = m_iMinimumRand = m_iMaximumRand = 0;
		}
	};
	typedef std::vector< GradeTable > vGradeTable;
	vGradeTable m_GradeTable;

	struct GradeAbility
	{
		int   m_iGradeLevel;       // 체크 레벨

		// 변경 사항
		bool  m_bEquipItem;        // true = 장비 장착 - AI 교체
		ioHashString m_szSkeleton; // 본 교체
		float m_fRecoveryHpRate;   // HP 회복량
		DWORD m_dwProtected;       // 무적 상태
        bool  m_bLevelUpMotion;    // 레벨업 애니메이션

		// 능력치
		float m_fArmorClass;
		float m_fMaxArmorClass;

		float m_fSpeedClass;
		float m_fMaxSpeedClass;
		GradeAbility()
		{
			m_iGradeLevel = 0;

			m_dwProtected = 0;
			m_bEquipItem = m_bLevelUpMotion = false;
			m_fRecoveryHpRate = 0.0f;

			m_fArmorClass = m_fMaxArmorClass = 0.0f;
			m_fSpeedClass = m_fMaxSpeedClass = 0.0f;
		}
	};
	typedef std::vector< GradeAbility > vGradeAbility;
	vGradeAbility m_GradeAbility;

	struct GradeEquipRandTable
	{
		ioHashString m_szViewName;
		// 장착 아이템 & AI
		int   m_iEquipItem[MAX_INVENTORY];
		ioHashString m_szAI;
		GradeEquipRandTable()
		{
			memset( m_iEquipItem, 0, sizeof( m_iEquipItem ) );
		}
	};
	typedef std::vector< GradeEquipRandTable > vGradeEquipRandTable;
	vGradeEquipRandTable m_GradeEquipRandTable;
	GradeEquipRandTable  m_GradeZeroEquip;

protected:
	// will be used while loading NPC AI
	ioAIDatabase*		m_AIDB;

protected:
	const ioNpcMgr::FieldNPCData *FindFieldNPC( DWORD dwCode );

protected:
	void LoadGradeTable();

public:
	void LoadFieldNPC();
	void ClearFeidNPC();

	void BuildAdditionalNPC( ioXMLElement* npclist );
	void BuildAdditionalNPCLabel( FieldNPCData *pData, ioXMLElement* node );
	void BuildAdditionalNPCRef(	FieldNPCData *pData, ioXMLElement* node );
	void BuildAdditionalNPCEquip( FieldNPCData *pData, ioXMLElement* node );	

public:
	ioNpcChar* CreateNpcCharByCharacter( SP2Packet &rkPacket, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode );
	ioNpcChar* CreateNpcCharByMyCharacter( SP2Packet &rkPacket, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode );
	ioNpcChar* CreateNpcChar( DWORD dwCode, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode );
	ioNpcChar* CreateNpcChar( DWORD dwNPCIndex, DWORD dwCode, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode );
	ioNpcChar* CreateNpcChar( ioEntityGroup *pGrp, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode, int iNpcType );

	ioAI* CreateAI( ioNpcChar *pCreator, 
					ioPlayStage *pPlayStage, 
					const ioHashString &rkINIFileName );
	ioAI* CreateAI( ioNpcChar *pCreator, 
					ioPlayStage *pPlayStage, 
					const ioHashString &rkINIFileName, 
					const ioHashString &rlXMLFileName );
	ioChampionAI* CreateChampionAI( 
					ioNpcChar *pCreator, 
					ioPlayStage *pPlayStage, 
					const ioHashString &rkFileName );

public:
	void  ChangeRandomEquipItem( ioNpcChar *pCreator, bool bEquipItem );
	DWORD GetRandomGradeUpTime(){ return m_dwGradeUpTime; }
	void  GetRandomGradeTable( int iGradeLevel, int &rMinimumRand, int &rMaximumRand );
	void  GetGradeAbilityValue( int iGradeLevel, bool &rEquipItem, DWORD &rProtected, bool &rLevelUpMotion, float &rRecoveryHpRate, ioHashString &rSkeleton );
	void  GetGradeAbilityArmor( int iGradeLevel, float &rArmor, float &rMaxArmor );
	void  GetGradeAbilitySpeed( int iGradeLevel, float &rSpeed, float &rMaxSpeed );

public:
	static ioNpcMgr& GetSingleton();

public:
	ioNpcMgr();
	virtual ~ioNpcMgr();
};

#define g_NpcMgr ioNpcMgr::GetSingleton()

