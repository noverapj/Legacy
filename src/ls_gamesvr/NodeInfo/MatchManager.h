#pragma once

typedef struct tagTopRank
{
	ioHashString	m_szNickname;
	int				m_iUserIndex;
	int				m_iRanking;
	int				m_iMatchPoint;
	int				m_iUserLevel;
	int				m_iWinCount;
	int				m_iLoseCount;

	tagTopRank()
	{
		m_iUserIndex = 0;
		m_iRanking = 0;
		m_iMatchPoint = 0;
		m_iUserLevel = 0;
		m_iWinCount = 0;
		m_iLoseCount = 0;
	}

}stTopRank;

typedef std::vector< stTopRank > vTopRankList;

typedef struct tagUserTeam
{
	int				m_iUserIndex;
	int				m_iTeamType;

	tagUserTeam()
	{
		m_iUserIndex = 0;
		m_iTeamType = 0;
	}

}stUserTeam;

typedef std::vector< stUserTeam > vUserTeam;

class MatchManager
{
public:
	MatchManager();
	virtual ~MatchManager();

	void	LoadINI();
	void	SendUserInfo( DWORD dwBlueUserIndex, DWORD dwRedUserIndex, int iStartTier, int iEndTier );
	bool	MatchEnterRoom( DWORD dwBlueUserIndex, DWORD dwRedUserIndex, bool bRevenge );
	void	DBToData( CQueryResultData *query_data );

	int GetPesoBonus( int iWinStreakCount );

	bool IsEnableMatch();

	DWORD GetOpenTime( int iDay );
	DWORD GetCloseTime( int iDay );

	int GetDefaultMMR()	{ return m_iDefaultMMR; }
	int GetMinimumMMR()	{ return m_iMinimumMMR; }

	int GetWinPoint()	{ return m_iWinPoint; }
	int GetLosePoint()	{ return m_iLosePoint; }
	int GetSuccessionMMRRate() { return m_iSuccessionRate; }

	bool UserELO() { return m_bUseELO; }
	bool CheckMinimumMMR() { return m_bCheckMinimumMMR; }

private:
	vTopRankList m_vTopRankList;
	IntVec m_vPesoBonus;
	int m_iDefaultMMR;
	int m_iMinimumMMR;
	int m_iLosePoint;
	int m_iWinPoint;
	int m_iSuccessionRate;

	bool m_bUseELO;
	bool m_bCheckMinimumMMR;


	DWORD m_dwOpenTime[7];
	DWORD m_dwCloseTime[7];
};

#define g_MatchManager (*cSingleton<class MatchManager>::GetInstance())