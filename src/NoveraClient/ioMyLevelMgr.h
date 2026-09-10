#pragma once

class ioUIRenderImage;

#define MAX_INT_VALUE           2147483647
class ioMyLevelMgr : public Singleton< ioMyLevelMgr >
{
private:
	struct MortmainCharInfo
	{
		int m_iBelowGradeLevel;  // 예) 상사1 이하( 훈련병 ~ 상사1 )
		int m_iHaveMortmainChar;

		MortmainCharInfo()
		{
			m_iBelowGradeLevel  = 0;			
			m_iHaveMortmainChar = 0;
		}
	};

	typedef std::vector< MortmainCharInfo* > vMortmainCharInfo;
	vMortmainCharInfo m_vMortmainCharInfoVector;

private:
	// Class Level
	DWORDVec m_vSoldierLevelExp;
	int      m_iSoldierMaxLevel;

	// Grade Level
	DWORDVec m_vGradeLevelExp;
	int      m_iGradeMaxUP;	
	int      m_iGradeMaxLevel;

	// Award Level
	DWORDVec m_vAwardLevelExp;
	int      m_iAwardMaxLevel;

	// Medal Level
	DWORDVec m_vMedalLevelExp;
	int      m_iMedalMaxLevel;

	// Party Level
	float m_fPartyConstantA;
	int   m_iPartyConstantB;
	int   m_iPartyConstantC;
	int   m_iPartyConstantD;

	// Ladder Level
	float m_fLadderConstantA;
	int   m_iLadderConstantB;
	int   m_iLadderConstantC;
	int   m_iLadderConstantD;

	// Hero Level 
	float m_fHeroConstantA;
	int   m_iHeroConstantB;
	int   m_iHeroConstantC;
	int   m_iHeroConstantD;

	// Solo Level
	float m_fSoloConstantA;
	int   m_iSoloConstantB;
	int   m_iSoloConstantC;
	int   m_iSoloConstantD;	

	// Fishing Level
	DWORDVec m_vFishingLevelExp;
	int		 m_iFishingMaxLevel;

	int   m_iGradeLevelForEtcBonus;

	// Excavation
	DWORDVec m_vExcavationLevelExp;
	int		 m_iExcavationMaxLevel;

	//TowerDef
	DWORDVec	m_vecTowerDefLvlExp;
	int			m_nTowerDefMaxLvl;
	int			m_nAutolvltime;
	int			m_nAutolvl;

protected:           /*LevelUnit, Icon*/
	typedef std::map< DWORD, ioUIRenderImage* > TypeResourceMap;
	TypeResourceMap m_GradeLevelIconMap;
	TypeResourceMap m_GradeLevelBlueIconMap;
	TypeResourceMap m_GradeLevelRedIconMap;
	TypeResourceMap m_GradeLevelOrangeIconMap;
	TypeResourceMap m_GradeLevelWhiteIconMap;
	TypeResourceMap m_GradeLevelGrayIconMap;

	ioUIRenderImage   *m_pGradeIOWhiteMark;
	ioUIRenderImage   *m_pGradeIOOrangeMark;
	ioUIRenderImage   *m_pGradeIOBlueMark;
	ioUIRenderImage   *m_pGradeIORedMark;
	ioUIRenderImage   *m_pGradeIOGrayMark;

	ioUIRenderImage   *m_pGradeGMWhiteMark;
	ioUIRenderImage   *m_pGradeGMOrangeMark;
	ioUIRenderImage   *m_pGradeGMBlueMark;
	ioUIRenderImage   *m_pGradeGMRedMark;
	ioUIRenderImage   *m_pGradeGMGrayMark;

	ioUIRenderImage   *m_pBlueClassLevelText;
	ioUIRenderImage   *m_pBlueClassLevelNum;
	ioUIRenderImage   *m_pRedClassLevelText;
	ioUIRenderImage   *m_pRedClassLevelNum;
	DWORD              m_dwResGradeLevelUnit;

	ioUIRenderImage   *m_pLobbyOffLineMark;
	ioUIRenderImage   *m_pMessengerOffLineMark;

	// 영웅전 칭호
	TypeResourceMap m_HeroTitleIconMap;
	TypeResourceMap m_HeroTitleTextMap;

	typedef std::map< DWORD, ioHashString > TypeStringMap;
	TypeStringMap   m_HeroTitleStrMap;


	typedef std::map< DWORD, float > TypeFloatMap;
	TypeFloatMap    m_HeroTitleBonusMap;

protected:
	int m_iAddGradeLevel;
	int m_iRoomEnterSafetyLevel;
	int m_iRoomKillDeathSafetyLevel; //실계 계산용
	int m_iRoomEnterLevelMax;
	int m_iBattleEnterLevelMax;
	int m_iLeaveSafetyRoomKo;
	int m_iKillDeathMinLevelGap;

public:
	void LoadINIInfo();

protected:
	void ClearAllResource();

public:
	ioUIRenderImage *GetGradeLevelIcon( DWORD dwGradeLevel );
	ioUIRenderImage *GetGradeBlueLevelIcon( DWORD dwGradeLevel );
	ioUIRenderImage *GetGradeRedLevelIcon( DWORD dwGradeLevel );
	ioUIRenderImage *GetGradeOrangeLevelIcon( DWORD dwGradeLevel );
	ioUIRenderImage *GetGradeWhiteLevelIcon( DWORD dwGradeLevel );
	ioUIRenderImage *GetGradeGrayLevelIcon( DWORD dwGradeLevel );
	ioUIRenderImage *GetHeroTitleIcon( DWORD dwHeroTitle );
	ioUIRenderImage *GetHeroTitleText( DWORD dwHeroTitle );
	const ioHashString &GetHeroTitleString( DWORD dwHeroTitle );
	float GetHeroTitleBonus( DWORD dwHeroTitle );
	
	int GetNextLevelupExp( int iCurLv );
	int GetNextGradeupExp( int iCurLv );
	int GetMaxGradeUp();
	int GetMaxGradeLevel();
	int GetNextAwardupExp( int iCurLv );
	int GetNextMedalupExp( int iCurLv );

	__int64 GetNextPartyupExp( int iCurLv );
	__int64 GetNextLadderupExp( int iCurLv );
	__int64 GetNextHeroupExp( int iCurLv );
	__int64 GetNextSoloupExp( int iCurLv );

	int GetNextFishingLevelUpExp( int iCurLv );
	int GetMaxFishingLevel();

	int GetNumHaveMortmainChar( int iGradeLv );
	int GetNextNumHaveMortmainChar( int iGradeLv );

	int GetAddGradeLevel(){ return m_iAddGradeLevel; }
	int GetRoomEnterSafetyLevel(){ return m_iRoomEnterSafetyLevel; }
	int GetRoomKillDeathSafetyLevel(){ return m_iRoomKillDeathSafetyLevel; }
	int GetRoomEnterLevelMax(){ return m_iRoomEnterLevelMax; }
	int GetBattleEnterLevelMax(){ return m_iBattleEnterLevelMax; }
	int GetLeaveSafetyRoomKo(){ return m_iLeaveSafetyRoomKo; }

	int GetGradeLevelForEtcBonus() const { return m_iGradeLevelForEtcBonus; }
	int GetKillDeathMinLevelGap() const { return m_iKillDeathMinLevelGap; }

	int GetNextExcavationLevelUpExp( int iCurLv );
	int GetMaxExcavationLevel();

	int GetNextAOSLevelUpExp(int iCurLv);
	int GetMaxAOSLevel();

	int GetSoldierMaxLevel()		{ return m_iSoldierMaxLevel; }

public:
	ioUIRenderImage *GetGradeBigIcon( int iGradeLevel ) const;
	ioUIRenderImage *GetGradeCircleIcon( int iGradeLevel ) const;
	void GetGradeName( int iGradeLevel, char *szBuf, int iBufSize, bool bStepGap, bool bStep = true ) const;

public:
	void RenderGrade( const ioHashString &szName, int iXPos, int iYPos, DWORD dwGradeLevel, TeamType eTeam, UIRenderType eRenderType = UI_RENDER_NORMAL, DWORD dwColor = 0xFFFFFFFF );
	void RenderGrade( int iXPos, int iYPos, DWORD dwGradeLevel, TeamType eTeam, UIRenderType eRenderType = UI_RENDER_NORMAL, DWORD dwColor = 0xFFFFFFFF );
	void RenderClassLevel( int iXPos, int iYPos, int iClassLevel, TeamType eTeam, bool bOwner, UIRenderType eRenderType = UI_RENDER_NORMAL );

public:
	void RenderLobbyOfflineMark( int iXPos, int iYPos, UIRenderType eRenderType = UI_RENDER_NORMAL, DWORD dwColor = 0xFFFFFFFF );
	void RenderMessengerOfflineMark( int iXPos, int iYPos, UIRenderType eRenderType = UI_RENDER_NORMAL, DWORD dwColor = 0xFFFFFFFF );

public:
	void RenderHeroTitleIcon( DWORD dwHeroTitle, int iXPos, int iYPos, float fScale = FLOAT1 );
	void RenderHeroTitleText( DWORD dwHeroTitle, int iXPos, int iYPos );

public:
	static ioMyLevelMgr& GetSingleton();

public:
	ioMyLevelMgr();
	virtual ~ioMyLevelMgr();
};

#define g_LevelMgr ioMyLevelMgr::GetSingleton()

