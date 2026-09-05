#pragma once

struct ModeContribute
{
	bool m_bLastResultSync;     // 최종 결과시 기여도를 받았으면 개별 갱신을 하지 않는다.
	int m_iDamage;				// 데미지 & 힐량
	int m_iAttackCnt;			// 유효 타격 횟수
	int m_iContribute;			// 최종 기여도
	int m_iPrevContributePer;

	//순위
	int m_iDamageRank;
	int m_iAttackCntRank;
	ModeContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		m_bLastResultSync = false;
		m_iDamage	    = 0;
		m_iAttackCnt    = 0;
		m_iContribute   = 0;
		m_iDamageRank   = 0;
		m_iAttackCntRank= 0;
		m_iPrevContributePer = 0;
	}
};

struct SymbolContribute : public ModeContribute
{
	int m_iSymbolDamage;		// 상징물 타격량
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)

	//순위
	int m_iSymbolDamageRank;
	int m_iKillRank;
    SymbolContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iSymbolDamage		 = 0;
		m_iKill              = 0;
		m_iSymbolDamageRank	 = 0;
		m_iKillRank			 = 0;
	}
};

struct CatchContribute : public ModeContribute
{
	int m_iPrisonerMode;        // 포로 만든 횟수	

	//순위
	int m_iPrisonerModeRank;
	CatchContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iPrisonerMode		= 0;
		m_iPrisonerModeRank = 0;
	}
};

struct HiddenkingContribute : public ModeContribute
{
	int   m_iKingTime;          // 왕관 보유 시간
	int   m_iKingPoint;         // 라스트 왕관은 x3
	int   m_iKill;				// 유저 KO(팀킬 & 자살시에 -)

	//순위
	int   m_iKingTimeRank; 
	int   m_iKingPointRank; 
	int   m_iKillRank; 
	HiddenkingContribute()
	{
		InitValues();	
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKingTime  	= 0;     
		m_iKingPoint	= 0;     
		m_iKill			= 0;
		m_iKingTimeRank	= 0; 
		m_iKingPointRank= 0; 
		m_iKillRank		= 0; 
	}
};

struct SurvivalContribute : public ModeContribute
{
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)
	int m_iDeath;               // Die 처리
	//순위
	int m_iSymbolDamageRank;
	int m_iKillRank;
	int m_iDeathRank;
    SurvivalContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;
		m_iKillRank			 = 0;
		m_iDeathRank         = 0;
	}
};

struct TeamSurvivalContribute : public ModeContribute
{
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)
	int m_iDeath;               // Die 처리
	//순위
	int m_iKillRank;
	int m_iDeathRank;
    TeamSurvivalContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;
		m_iKillRank			 = 0;
		m_iDeathRank         = 0;
	}
};

struct BossContribute : public ModeContribute
{
	int m_iKill;
	int m_iDeath;
	//순위
	int m_iSymbolDamageRank;
	int m_iKillRank;
	int m_iDeathRank;
	BossContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;
		m_iKillRank			 = 0;
		m_iDeathRank         = 0;
	}
};


struct MonsterSurvivalContribute : public ModeContribute
{
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)
	int m_iDeath;               // Die 처리
	int m_iWounded;             // 피격

	int m_nInstantLevel;				// instant level   타워디펜스모드에서 씀
	int m_nInstantExp;
	int m_nPlayTime;

	//순위
	int m_iKillRank;
	int m_iDeathRank;
	int m_iWoundedRank;
	MonsterSurvivalContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;
		m_iWounded			 = 0;
		m_iKillRank			 = 0;
		m_iDeathRank         = 0;
		m_iWoundedRank		 = 0;
		m_nInstantLevel		= 0;
		m_nInstantExp		= 0;
		m_nPlayTime			= 0;
	}
};

struct TrainingContribute : public ModeContribute
{
	int m_iKill;             
	//순위
	int m_iKillRank;
	TrainingContribute()
	{
		InitValues();
	}
	
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iKillRank			 = 0;
	}
};

struct FootballContribute : public ModeContribute
{
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)
	int m_iDeath;               // Die 처리

	int m_iGoal;			// 득점
	int m_iBallTouch;		// 볼 타격 횟수
	int m_iBallPoint;		// 볼 인접 시간
	int m_iAssist;			// 어시스트
	int m_iSaveTouch;		// 선방
	
	//순위
	int m_iKillRank;
	int m_iDeathRank;

	int m_iGoalRank;
	int m_iBallTouchRank;
	int m_iBallPointRank;
	int m_iAssistRank;
	int m_iSaveTouchRank;

	FootballContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;

		m_iGoal				 = 0;
		m_iBallTouch		 = 0;
		m_iBallPoint		 = 0;
		m_iAssist			 = 0;
		m_iSaveTouch		 = 0;
		
		m_iKillRank			 = 0;
		m_iDeathRank         = 0;

		m_iGoalRank			 = 0;
		m_iBallTouchRank	 = 0;
		m_iBallPointRank	 = 0;
		m_iAssistRank		 = 0;
		m_iSaveTouchRank	 = 0;
	}
};

struct HeroMatchContribute : public ModeContribute
{
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)
	int m_iDeath;               // Die 처리

	//순위
	int m_iKillRank;
	int m_iDeathRank;
	HeroMatchContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;

		m_iKillRank			 = 0;
		m_iDeathRank         = 0;
	}
};

struct GangsiContribute : public ModeContribute
{
	int m_iGangsiInfection;
	int m_iGangsiSurvivor;

	//순위
	int m_iGangsiInfectionRank;
	int m_iGangsiSurvivorRank;
	GangsiContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iGangsiInfection = 0;
		m_iGangsiSurvivor  = 0;

		m_iGangsiInfectionRank = 0;
		m_iGangsiSurvivorRank  = 0;
	}
};

struct HeadquartersContribute : public ModeContribute
{
	int m_iKill;             
	//순위
	int m_iKillRank;
	HeadquartersContribute()
	{
		InitValues();
	}

	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iKillRank			 = 0;
	}
};

struct FightClubContribute : public ModeContribute
{
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)
	int m_iDeath;               // Die 처리
	//순위
	int m_iSymbolDamageRank;
	int m_iKillRank;
	int m_iDeathRank;
	FightClubContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;
		m_iKillRank			 = 0;
		m_iDeathRank         = 0;
	}
};

struct DobuleCrownContribute : public ModeContribute
{
	int   m_iKingTime;          // 왕관 보유 시간
	int   m_iKingPoint;         // 라스트 왕관은 x3
	int   m_iKill;				// 유저 KO(팀킬 & 자살시에 -)

	//순위
	int   m_iKingTimeRank; 
	int   m_iKingPointRank; 
	int   m_iKillRank; 
	DobuleCrownContribute()
	{
		InitValues();	
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKingTime  	= 0;     
		m_iKingPoint	= 0;     
		m_iKill			= 0;
		m_iKingTimeRank	= 0; 
		m_iKingPointRank= 0; 
		m_iKillRank		= 0; 
	}
};

struct ShuffleBonusContribute : public ModeContribute
{
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)
	int m_iDeath;               // Die 처리
	//순위
	int m_iSymbolDamageRank;
	int m_iKillRank;
	int m_iDeathRank;
	ShuffleBonusContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;
		m_iKillRank			 = 0;
		m_iDeathRank         = 0;
	}
};

struct HouseContribute : public ModeContribute
{
	int m_iKill;             
	//순위
	int m_iKillRank;
	HouseContribute()
	{
		InitValues();
	}

	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iKillRank			 = 0;
	}
};

struct RaidContribute : public ModeContribute
{
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)
	int m_iDeath;               // Die 처리
	//순위
	int m_iSymbolDamageRank;
	int m_iKillRank;
	int m_iDeathRank;
	RaidContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;
		m_iKillRank			 = 0;
		m_iDeathRank         = 0;
	}
};

struct FlagCaptureContribute : public ModeContribute
{
	int   m_iFlagTime;          // 깃발 보유 시간
	int   m_iFlagPoint;         // 깃발로 획득한 점수
	int   m_iKill;				// 유저 KO(팀킬 & 자살시에 -)
	int	  m_iDeath;             // Die 처리

	//순위
	int   m_iFlagTimeRank; 
	int   m_iFlagPointRank; 
	int   m_iKillRank;
	int   m_iDeathRank;
	FlagCaptureContribute()
	{
		InitValues();	
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iFlagTime  	= 0;     
		m_iFlagPoint	= 0;     
		m_iKill			= 0;
		m_iDeath		= 0;
		m_iFlagTimeRank	= 0; 
		m_iFlagPointRank= 0; 
		m_iKillRank		= 0; 
		m_iDeathRank	= 0;
	}
};

struct ArenaContribute : public ModeContribute
{
	int m_iPrisonerMode;        // 포로 만든 횟수	

	//순위
	int m_iPrisonerModeRank;
	ArenaContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iPrisonerMode		= 0;
		m_iPrisonerModeRank = 0;
	}
};

struct BattleModeContribute : public ModeContribute
{
	int m_iKill;                // 유저 KO(팀킬 & 자살시에 -)
	int m_iDeath;               // Die 처리
	//순위
	int m_iSymbolDamageRank;
	int m_iKillRank;
	int m_iDeathRank;
	BattleModeContribute()
	{
		InitValues();
	}
	virtual void InitValues()
	{
		ModeContribute::InitValues();
		m_iKill              = 0;
		m_iDeath             = 0;
		m_iKillRank			 = 0;
		m_iDeathRank         = 0;
	}
};

struct ContributeInfo
{
	int m_iRecordArray;
	int m_iContributePoint;
	int m_iGradeLevel;
	ioHashString m_szName;
	int m_iRank;

	ContributeInfo()
	{
		m_iRecordArray		= -1;
		m_iContributePoint  = 0;
		m_iRank			    = 0;
	}
};

class ContributeSort : public std::binary_function< const ContributeInfo&, const ContributeInfo&, bool >
{
public:
	bool operator()( const ContributeInfo &lhs , const ContributeInfo &rhs ) const
	{
		if( lhs.m_iRecordArray == -1 )
		{
			if( rhs.m_iRecordArray == -1 )
				return true;
			else
				return false;
		}
		else if( rhs.m_iRecordArray == -1 )
		{
			return true;
		}

		if( lhs.m_iContributePoint > rhs.m_iContributePoint )
		{
			return true;
		}

		return false;
	}
};

class ContributeRankSort : public std::binary_function< const ContributeInfo&, const ContributeInfo&, bool >
{
public:
	bool operator()( const ContributeInfo &lhs , const ContributeInfo &rhs ) const
	{		
		if( lhs.m_iContributePoint > rhs.m_iContributePoint )
		{
			return true;
		}
		else if( lhs.m_iContributePoint == rhs.m_iContributePoint )
		{
			if( lhs.m_iGradeLevel > rhs.m_iGradeLevel )
			{
				return true;
			}			
			else if( lhs.m_iGradeLevel == rhs.m_iGradeLevel )
			{
				if( lhs.m_szName.GetHashCode() > rhs.m_szName.GetHashCode() )
				{
					return true;
				}
			}
		}

		return false;
	}
};


class R_ContributeRankSort : public std::binary_function< const ContributeInfo&, const ContributeInfo&, bool >
{
public:
	bool operator()( const ContributeInfo &lhs , const ContributeInfo &rhs ) const
	{		
		if( lhs.m_iContributePoint < rhs.m_iContributePoint )
		{
			return true;
		}
		else if( lhs.m_iContributePoint == rhs.m_iContributePoint )
		{
			if( lhs.m_iGradeLevel > rhs.m_iGradeLevel )
			{
				return true;
			}			
			else if( lhs.m_iGradeLevel == rhs.m_iGradeLevel )
			{
				if( lhs.m_szName.GetHashCode() > rhs.m_szName.GetHashCode() )
				{
					return true;
				}
			}
		}

		return false;
	}
};

typedef std::vector< ContributeInfo > ContributeInfoList;

