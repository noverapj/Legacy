#pragma once

struct RoundInfo
{
	ioHashString m_szName;
	TeamType	 m_Team;
	int m_iContribute;
	int m_iContributePer;
	ioHashString m_szContributeRank;
	int m_iTotalKill;
	int m_iTotalDeath;
	int m_iUniqueTotalKill;
	int m_iUniqueTotalDeath;
	int m_iVictories;
	int m_iLadderPoint;
	int m_iLevel;
	int m_iMonsterCoin;
	int m_iGoldMonsterCoin;
	int m_iRaidCoin;
	bool m_bOwner;
	bool m_bPCRoomBonus;
	bool m_bFriendBonus;
	bool m_bGuildBonus;
	DWORD m_dwGuildIndex;
	DWORD m_dwGuildMark;

	int m_iGoal;
	int m_iGoalAssist;

	int m_iFightWin;
	int m_iFightLose;
	int m_iFightVictory;

	int m_iStarCnt;
	bool m_bNPC;
	bool m_bEnable;
	int m_iHunterCoinCnt;
	RoundInfo()
	{
		m_Team = TEAM_NONE;
		m_iContribute = 0;
		m_iContributePer = 0;
		m_iTotalKill  = 0;
		m_iTotalDeath = 0;
		m_iUniqueTotalKill  = 0;
		m_iUniqueTotalDeath = 0;
		m_iVictories  = 0;
		m_iLadderPoint= 0;
		m_iLevel	  = 0;
		m_iMonsterCoin= 0;
		m_iGoldMonsterCoin = 0;
		m_iRaidCoin = 0;
		m_bOwner      = false;
		m_bPCRoomBonus= false;
		m_bFriendBonus= false;
		m_bGuildBonus = false;
		m_dwGuildIndex= 0;
		m_dwGuildMark = 0;

		m_iGoal = 0;
		m_iGoalAssist = 0;

		m_iFightWin		= 0;
		m_iFightLose	= 0;
		m_iFightVictory	= 0;

		m_iStarCnt = 0;
		m_bNPC = false;
		m_bEnable = false;
		m_iHunterCoinCnt = 0;
	}
};

class RoundInfoSort : public std::binary_function< const RoundInfo&, const RoundInfo&, bool >
{
public:
	bool operator()( const RoundInfo &lhs , const RoundInfo &rhs ) const
	{
		if( lhs.m_szName.IsEmpty() )
		{
			if( rhs.m_szName.IsEmpty() )
				return true;
			else
				return false;
		}
		else if( !lhs.m_szName.IsEmpty() && rhs.m_szName.IsEmpty() )
		{
			return true;
		}


		if( lhs.m_iContribute > rhs.m_iContribute )
		{
			return true;
		}
		else if( lhs.m_iContribute == rhs.m_iContribute )
		{
			if( lhs.m_iLevel > rhs.m_iLevel )         
			{
				return true;
			}
			else if( lhs.m_iLevel == rhs.m_iLevel )
			{
				int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );     
				if( iCmpValue < 0 )
					return true;
			}
		}	
		return false;
	}
};

class RoundInfoKoSort : public std::binary_function< const RoundInfo&, const RoundInfo&, bool >
{
public:
	bool operator()( const RoundInfo &lhs , const RoundInfo &rhs ) const
	{
		if( lhs.m_szName.IsEmpty() )
		{
			if( rhs.m_szName.IsEmpty() )
				return true;
			else
				return false;
		}
		else if( !lhs.m_szName.IsEmpty() && rhs.m_szName.IsEmpty() )
		{
			return true;
		}

		if( lhs.m_iUniqueTotalKill > rhs.m_iUniqueTotalKill )  
		{
			return true;
		}
		else if( lhs.m_iUniqueTotalKill == rhs.m_iUniqueTotalKill )
		{
			if( lhs.m_iUniqueTotalDeath < rhs.m_iUniqueTotalDeath )
			{
				return true;
			}
			else if( lhs.m_iUniqueTotalDeath == rhs.m_iUniqueTotalDeath	)
			{
				if( lhs.m_iLevel > rhs.m_iLevel )
				{
					return true;
				}
				else if( lhs.m_iLevel == rhs.m_iLevel )
				{
					int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );     
					if( iCmpValue < 0 )
						return true;
				}
			}
		}		
		return false;
	}
};

class RoundInfoLevelSort : public std::binary_function< const RoundInfo&, const RoundInfo&, bool >
{
public:
	bool operator()( const RoundInfo &lhs , const RoundInfo &rhs ) const
	{
		if( lhs.m_szName.IsEmpty() )
		{
			if( rhs.m_szName.IsEmpty() )
				return true;
			else
				return false;
		}
		else if( !lhs.m_szName.IsEmpty() && rhs.m_szName.IsEmpty() )
		{
			return true;
		}

		if( lhs.m_iLevel > rhs.m_iLevel )
		{
			return true;
		}
		else if( lhs.m_iLevel == rhs.m_iLevel )
		{
			int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );     
			if( iCmpValue < 0 )
				return true;
		}
		return false;
	}
};

class RoundInfoStarSort : public std::binary_function< const RoundInfo&, const RoundInfo&, bool >
{
public:
	bool operator()( const RoundInfo &lhs , const RoundInfo &rhs ) const
	{
		if( lhs.m_szName.IsEmpty() )
		{
			if( rhs.m_szName.IsEmpty() )
				return true;
			else
				return false;
		}
		else if( !lhs.m_szName.IsEmpty() && rhs.m_szName.IsEmpty() )
		{
			return true;
		}

		if( lhs.m_iStarCnt > rhs.m_iStarCnt )  
		{
			return true;
		}
		else if( lhs.m_iStarCnt == rhs.m_iStarCnt )
		{
			if( lhs.m_iUniqueTotalKill > rhs.m_iUniqueTotalKill )  
			{
				return true;
			}
			else if( lhs.m_iUniqueTotalKill == rhs.m_iUniqueTotalKill )
			{
				if( lhs.m_iUniqueTotalDeath < rhs.m_iUniqueTotalDeath )
				{
					return true;
				}
				else if( lhs.m_iUniqueTotalDeath == rhs.m_iUniqueTotalDeath	)
				{
					if( lhs.m_iLevel > rhs.m_iLevel )
					{
						return true;
					}
					else if( lhs.m_iLevel == rhs.m_iLevel )
					{
						int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );     
						if( iCmpValue < 0 )
							return true;
					}
				}
			}
		}		
		return false;
	}
};

//-------------------------------------------------------------------------------------------------------------------------
struct RoundHistory
{
	int iBluePoint;
	int iRedPoint;
	
	RoundHistory()
	{
		iBluePoint = 0;
		iRedPoint  = 0;
	}
};

typedef std::vector< RoundHistory > vRoundHistory;

//-------------------------------------------------------------------------------------------------------------------------
class TrainingRoundInfoSort : public std::binary_function< const RoundInfo&, const RoundInfo&, bool >
{
public:
	bool operator()( const RoundInfo &lhs , const RoundInfo &rhs ) const
	{
		if( lhs.m_iTotalKill > rhs.m_iTotalKill )
		{
			return true;
		}
		else if( lhs.m_iTotalKill == rhs.m_iTotalKill )
		{
			if( lhs.m_iTotalDeath < rhs.m_iTotalDeath )
			{
				return true;
			}
			else if( lhs.m_iTotalDeath == rhs.m_iTotalDeath	)
			{
				int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );
				if( iCmpValue < 0 )
					return true;
			}
		}

		return false;
	}
};
//--------------------------------------------------------------------------------------------------------------------------

struct ResultInfo
{
	ioHashString m_szName;
	TeamType	m_Team;
	int m_iContribute;
	int m_iContributePer;
	ioHashString m_szContributeRank;
	int m_iTotalKill;
	int m_iTotalDeath;
	int m_iUniqueTotalKill;
	int m_iUniqueTotalDeath;
	int m_iVictories;
	int m_iLadderPoint;
	int m_iLevel;
	int m_iRankDiff;
	bool m_bOwner;
	bool m_bPCRoomBonus;
	bool m_bFriendBonus;
	bool m_bGuildBonus;
	DWORD m_dwGuildIndex;
	DWORD m_dwGuildMark;

	int m_iGoal;
	int m_iStarCount;
	bool m_bNPC;
	int m_iHunterCoinCnt;
	
	ResultInfo()
	{
		m_Team = TEAM_NONE;

		m_iContribute = 0;
		m_iContributePer = 0;
		m_iTotalKill  = 0;
		m_iTotalDeath = 0;
		m_iUniqueTotalKill  = 0;
		m_iUniqueTotalDeath = 0;
		m_iVictories  = 0;
		m_iLadderPoint= 0;
		m_iLevel	  = 0;
		m_iRankDiff   = 0;
		m_bOwner      = false;
		m_bPCRoomBonus= false;
		m_bFriendBonus= false;
		m_bGuildBonus = false;
		m_dwGuildIndex= 0;
		m_dwGuildMark = 0;

		m_iGoal = 0;
		m_iStarCount = 0;
		m_bNPC = false;
		m_iHunterCoinCnt = 0;
	}
};

class ResultInfoSort : public std::binary_function< const ResultInfo&, const ResultInfo&, bool >
{
public:
	bool operator()( const ResultInfo &lhs , const ResultInfo &rhs ) const
	{
		if( lhs.m_iContribute > rhs.m_iContribute )
		{
			return true;
		}
		else if( lhs.m_iContribute == rhs.m_iContribute )
		{
			if( lhs.m_iLevel > rhs.m_iLevel )
			{
				return true;
			}
			else if( lhs.m_iLevel == rhs.m_iLevel )
			{
				int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );
				if( iCmpValue < 0 )
					return true;
			}
		}			
		return false;
	}
};

typedef std::vector< ResultInfo > ResultInfoList;

struct FinalInfo
{
	ioHashString m_szName;
	TeamType	m_Team;
	int m_iContribute;
	int m_iContributePer;
	ioHashString m_szContributeRank;
	int m_iRank;
	int m_iTotalKill;
	int m_iTotalDeath;
	int m_iUniqueTotalKill;
	int m_iUniqueTotalDeath;
	int m_iVictories;
	int m_iLadderPoint;
	int m_iLevel;
	int m_iResultPeso;
	int m_iResultExp;
	int m_iResultLadderPoint;
	bool m_bOwner;
	bool m_bLevelUP;
	bool m_bPCRoomBonus;
	bool m_bFriendBonus;
	bool m_bGuildBonus;
	DWORD m_dwGuildIndex;
	DWORD m_dwGuildMark;

	int m_iGoal;

	//UI 관련 정보
	int   m_iCurPeso;
	int   m_iCurExp;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingAniTime;
	bool  m_bOwnerScreen;
	int   m_iRollingInfoArray;
	bool  m_bRollingEndAniStart;
	float m_fRollingEndAniValue;
	int   m_iPvEClassType;
	DWORD m_dwPvECharIndex;
	int   m_iTreasureCardCnt;

	//셔플 별
	int m_iStarCount;
	bool m_bNPC;
	ioHashString m_szNPCName;
	IntVec vResultClassType;
	IntVec vResultClassGender;
	int m_iHunterCoinCnt;
	
	FinalInfo()
	{
		m_Team					= TEAM_NONE;

		m_iContribute			= 0;
		m_iContributePer		= 0;
		m_iRank					= 0;
		m_iTotalKill			= 0;
		m_iTotalDeath			= 0;
		m_iUniqueTotalKill		= 0;
		m_iUniqueTotalDeath		= 0;
		m_iVictories			= 0;
		m_iLadderPoint			= 0;
		m_iLevel				= 0;
		m_iResultPeso			= 0;
		m_iResultExp			= 0;
		m_iResultLadderPoint	= 0;
		m_bOwner				= false;
		m_bLevelUP				= false;
		m_bPCRoomBonus			= false;
		m_bFriendBonus			= false;
		m_bGuildBonus			= false;
		m_dwGuildIndex			= 0;
		m_dwGuildMark			= 0;

		m_iGoal					= 0;

		m_iCurPeso				= -1;
		m_iCurExp				= -1;
		m_dwRollingTime			= 0;
		m_dwRollingAniTime		= 0;
		m_bOwnerScreen			= false;
		m_iRollingInfoArray		= 0;
		m_bRollingEndAniStart	= false;
		m_fRollingEndAniValue	= 0.0f;
		m_iPvEClassType			= 0;
		m_dwPvECharIndex		= 0;
		m_iTreasureCardCnt		= 0;

		m_iStarCount			= 0;
		m_bNPC					= false;
		m_iHunterCoinCnt = 0;
	}
};

class FinalInfoSort : public std::binary_function< const FinalInfo&, const FinalInfo&, bool >
{
public:
	bool operator()( const FinalInfo &lhs , const FinalInfo &rhs ) const
	{
		if( lhs.m_iContributePer > rhs.m_iContributePer )
		{
			return true;
		}
		else if( lhs.m_iContribute == rhs.m_iContribute )
		{
			if( lhs.m_iLevel > rhs.m_iLevel )
			{
				return true;
			}
			else if( lhs.m_iLevel == rhs.m_iLevel )
			{
				int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );
				if( iCmpValue < 0 )
					return true;
			}
		}			
		return false;
	}
};

class FinalInfoKoSort : public std::binary_function< const FinalInfo&, const FinalInfo&, bool >
{
public:
	bool operator()( const FinalInfo &lhs , const FinalInfo &rhs ) const
	{
		if( lhs.m_iTotalKill > rhs.m_iTotalKill )
		{
			return true;
		}
		else if( lhs.m_iTotalKill == rhs.m_iTotalKill )
		{
			if( lhs.m_iTotalDeath < rhs.m_iTotalDeath )
			{
				return true;
			}
			else if( lhs.m_iTotalDeath == rhs.m_iTotalDeath )
			{
				if( lhs.m_iLevel > rhs.m_iLevel )
				{
					return true;
				}
				else if( lhs.m_iLevel == rhs.m_iLevel )
				{
					int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );
					if( iCmpValue < 0 )
						return true;
				}
			}
		}			
		return false;
	}
};

class FinalInfoLevelSort : public std::binary_function< const FinalInfo&, const FinalInfo&, bool >
{
public:
	bool operator()( const FinalInfo &lhs , const FinalInfo &rhs ) const
	{
		if( lhs.m_iLevel > rhs.m_iLevel )
		{
			return true;
		}
		else if( lhs.m_iLevel == rhs.m_iLevel )
		{
			int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );
			if( iCmpValue < 0 )
				return true;
		}
		return false;
	}
};

class FinalInfoShuffleStarSort : public std::binary_function< const FinalInfo&, const FinalInfo&, bool >
{
public:
	bool operator()( const FinalInfo &lhs , const FinalInfo &rhs ) const
	{
		if( lhs.m_iStarCount > rhs.m_iStarCount )
		{
			return true;
		}
		else if( lhs.m_iStarCount == rhs.m_iStarCount )
		{
			int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );
			if( iCmpValue < 0 )
				return true;
		}
		return false;
	}
};

typedef std::vector< FinalInfo > FinalInfoList;

struct PartyStateInfo
{
	ioHashString m_szName;
	TeamType	 m_Team;
	bool         m_bPartyOwner;
	bool         m_bOwner;
	int          m_iLevel;

	PartyStateInfo()
	{
		m_Team			= TEAM_NONE;
		m_bPartyOwner	= false;		
		m_iLevel		= 0;		
		m_bOwner        = false;
	}
};

class PartyStateInfoSort : public std::binary_function< const PartyStateInfo&, const PartyStateInfo&, bool >
{
public:
	bool operator()( const PartyStateInfo &lhs , const PartyStateInfo &rhs ) const
	{
		if( (int)lhs.m_bPartyOwner > (int)rhs.m_bPartyOwner )
		{
			return true;
		}
		else if( lhs.m_bPartyOwner == rhs.m_bPartyOwner )
		{
			if( lhs.m_iLevel > rhs.m_iLevel )
			{
				return true;
			}
			else if( lhs.m_iLevel == rhs.m_iLevel )
			{
				int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );
				if( iCmpValue < 0 )
					return true;
			}			
		}
		return false;
	}
};

typedef std::vector< PartyStateInfo > PartyStateInfoList;

struct BossContributeInfo
{
	ioHashString m_szName;
	int          m_iContribute;
	BossContributeInfo()
	{
		m_iContribute = 0;
	}
};
typedef std::vector< BossContributeInfo > BossContributeInfoList;
class BossContributeInfoSort : public std::binary_function< const BossContributeInfo&, const BossContributeInfo&, bool >
{
public:
	bool operator()( const BossContributeInfo &lhs , const BossContributeInfo &rhs ) const
	{
		if( lhs.m_iContribute > rhs.m_iContribute )
		{
			return true;
		}
		else if( lhs.m_iContribute == rhs.m_iContribute )
		{
			int iCmpValue = stricmp( lhs.m_szName.c_str(), rhs.m_szName.c_str() );
			if( iCmpValue < 0 )
				return true;
		}
		return false;
	}
};

