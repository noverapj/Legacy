#pragma once

#include "../io3DEngine/io3DCommon.h"
#include "ModeContribute.h"

class ioBaseChar;
class ioSymbolStruct;

typedef std::map< int, int > KillDeathInfoMap;

struct ModeRecord
{
	ioHashString szName;
	ioBaseChar   *pChar;

	DWORD dwCurDieTime;
	DWORD dwRevivalGap;
	int iRevivalCnt;

	KillDeathInfoMap iKillInfoMap;
	KillDeathInfoMap iDeathInfoMap;

	int iUniqueTotalKill;
	int iUniqueTotalDeath;

	int iNoDeathKillCount;

	int iCurRank;
	int iPreRank;
	
	bool bResultShow;

	// 전체 유저의 레벨업 및 획득한 경험치/페소를 서버가 동기화
	bool bResultLevelUP;
	int  iResultLevel;
	int  iResultTotalExp;
	int  iResultTotalPeso;
	int  iResultTotalLadderPoint;
	int  iResultTreasureCardCount;

	// PvE모드 가장 플레이 많이한 용병
	int   iPvEClassType;
	DWORD dwPvECharIndex;
	IntVec vResultClassType;
	IntVec vResultClassGender;

	ModeRecord();

	virtual void InitValues();

	int GetAllTotalKill();
	int GetAllTotalDeath();

	int GetUniqueTotalKill() const;
	int GetUniqueTotalDeath() const;

	const int& GetNoDeathKillCount() const;

	virtual void AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker );
	virtual void AddDeath( ModeType eMode, int iCount );
};

struct SymbolRecord : public ModeRecord
{
	ioSymbolStruct	*pSymbol;
	D3DXVECTOR3		 vRecentDiePos;
	SymbolContribute Contribute;
	
	SymbolRecord()
	{
		pSymbol = NULL;
		InitValues();
	}
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
	}
};

struct CatchRecord : public ModeRecord
{
	CatchContribute Contribute;
	DWORD dwRunningManDeco;
	ioHashString szRunningManName;

	CatchRecord()
	{
		InitValues();
	}	
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
		dwRunningManDeco = 0;
	}
};

struct HiddenkingRecord : public ModeRecord
{
	HiddenkingContribute Contribute;
	HiddenkingRecord()
	{
		InitValues();
	}		
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
	}
};

struct TrainingRecord : public ModeRecord
{
	ioSymbolStruct	*pSymbol;
	TrainingContribute Contribute;
	DWORD dwRunningManDeco;
	ioHashString szRunningManName;

	// 권한 아이템으로 본을 교체함
	int  iSkeletonEtcItem;
	TrainingRecord()
	{
		pSymbol = NULL;
		InitValues();
	}

	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
		iSkeletonEtcItem = 0;
		dwRunningManDeco = 0;
	}

	virtual void AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker );
	virtual void AddDeath( ModeType eMode, int iCount );
};

struct SurvivalRecord : public ModeRecord
{
	SurvivalContribute Contribute;
	SurvivalRecord()
	{
		InitValues();
	}	
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
	}
};

struct TeamSurvivalRecord : public ModeRecord
{
	TeamSurvivalContribute Contribute;
	TeamSurvivalRecord()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
	}
};

struct BossRecord : public ModeRecord
{
	BossContribute Contribute;
	int            iBossContribute;

	BossRecord()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
        
		iBossContribute = 0;      //보스 타격 + Damgae x 0.1f
	}
};

struct MonsterSurvivalRecord : public ModeRecord
{
	MonsterSurvivalContribute Contribute;
	MonsterSurvivalRecord()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
	}
};


struct FootballRecord : public ModeRecord
{
	FootballContribute Contribute;
	int m_iGoal;
	int m_iGoalAssist;

	FootballRecord()
	{
		InitValues();
	}	

	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();

		m_iGoal = 0;
		m_iGoalAssist = 0;
	}
};

struct HeroMatchRecord : public ModeRecord
{
	HeroMatchContribute Contribute;
	HeroMatchRecord()
	{
		InitValues();
	}	
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
	}
};


struct GangsiRecord : public ModeRecord
{
	GangsiContribute Contribute;
	GangsiRecord()
	{
		InitValues();
	}

	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
	}
};

struct HeadquartersRecord : public ModeRecord
{
	HeadquartersContribute Contribute;

	// 권한 아이템으로 본을 교체함
	int  iSkeletonEtcItem;
	HeadquartersRecord()
	{
		InitValues();
	}

	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
		iSkeletonEtcItem = 0;
	}

	virtual void AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker );
	virtual void AddDeath( ModeType eMode, int iCount );
};

enum
{
	FIGHTCLUB_WAITING,
	FIGHTCLUB_CHAMPION,
	FIGHTCLUB_CHALLENGER,
	FIGHTCLUB_OBSERVER,
};
struct FightClubRecord : public ModeRecord
{
	FightClubContribute Contribute;

	int iFightState;
	int iFightSeq; 

	int iFightWin;
	int iFightLose;
	int iFightVictories;
	int iFightSeqLoseCount;
	int iFightCheer;
	FightClubRecord()
	{
		InitValues();
		iFightWin = iFightLose = iFightSeqLoseCount = 0;
		iFightSeq = iFightVictories = iFightState = iFightCheer = 0;
	}	
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
	}
};

struct FightNPCRecord
{
	DWORD dwCode;
	ioHashString szName;
	ioHashString szSyncUser;
	TeamType eTeam;

	FightNPCRecord()
	{
		dwCode = 0;
		eTeam = TEAM_RED;
	}
};

struct DoubleCrownRecord : public ModeRecord
{
	DobuleCrownContribute Contribute;

	DWORD m_dwCrownHoldTime;

	DoubleCrownRecord()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();

		m_dwCrownHoldTime = 0;
	}
};

struct ShuffleBonusRecord : public ModeRecord
{
	ShuffleBonusContribute Contribute;

	int m_iStarCount;
	int m_iStarCountByCalcBonus;
	float m_fStarBonusPercent;

	DWORD m_dwLastDropStarTime;

	ShuffleBonusRecord()
	{
		InitValues();
	}	
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();

		m_iStarCount            = 0;
		m_iStarCountByCalcBonus = 0;
		m_fStarBonusPercent     = 0;

		m_dwLastDropStarTime = 0;
	}
};

typedef std::vector< FightNPCRecord > FightNPCRecordList;
struct TeamSurvivalAIRecord : public ModeRecord
{
	TeamSurvivalContribute Contribute;
	int m_iType;
	bool m_bDie;

	std::map<ioHashString,int> m_mapAggro;

	TeamSurvivalAIRecord()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();

		m_iType = 0;
		m_bDie = false;
	}

	void AddAggro( ioHashString& szName, int iValue )
	{
		auto it = m_mapAggro.find( szName );
		if( it != m_mapAggro.end() )
			(*it).second += iValue;
		else
			m_mapAggro.insert( std::map<ioHashString,int>::value_type(szName,iValue) );
	}
};

struct HouseRecord : public ModeRecord
{
	HouseContribute Contribute;

	// 권한 아이템으로 본을 교체함
	int  iSkeletonEtcItem;
	HouseRecord()
	{
		InitValues();
	}

	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
		iSkeletonEtcItem = 0;
	}

	virtual void AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker );
	virtual void AddDeath( ModeType eMode, int iCount );
};

struct RaidRecord : public ModeRecord
{
	RaidContribute Contribute;

	int m_iHunterCoinCount;

	RaidRecord()
	{
		InitValues();
	}	
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();

		m_iHunterCoinCount            = 0;
	}
};

struct FlagCaptureRecord : public ModeRecord
{
	FlagCaptureContribute Contribute;

	//DWORD m_dwCaptureFlagTime;

	FlagCaptureRecord()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();

		//m_dwCaptureFlagTime = 0;
	}
};

struct ArenaRecord : public ModeRecord
{
	ArenaContribute Contribute;
	DWORD dwRunningManDeco;
	ioHashString szRunningManName;

	ArenaRecord()
	{
		InitValues();
	}	
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
		dwRunningManDeco = 0;
	}
};

enum
{
	BATTLE_MODE_WAITING,
	BATTLE_MODE_CHAMPION,
	BATTLE_MODE_CHALLENGER,
	BATTLE_MODE_OBSERVER,
};
struct BattleModeRecord : public ModeRecord
{
	BattleModeContribute Contribute;

	int iBattleOrder;

	BattleModeRecord()
	{
		InitValues();
		iBattleOrder = 0;
	}	
	virtual void InitValues()
	{
		ModeRecord::InitValues();
		Contribute.InitValues();
	}
};