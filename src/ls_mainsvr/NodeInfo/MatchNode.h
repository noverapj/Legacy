#pragma once
#include "../DataBase/LogDBClient.h"
#include "MatchNodeManager.h"


class MatchUser
{
public:
	enum eMatchState
	{
		MatchReady		= 0, // 매칭 노드 추가 전 상태
		MatchEnter		= 1, // 매칭 노드 추가 후 상태
		MatchDelete		= 2, // 매칭 후 삭제 대기
	};

public:
	MatchUser();
	virtual ~MatchUser();

	void				Init();
	int					m_iMatchPoint;

protected:
	DWORD				m_dwUserIndex;
	int					m_iWinStreakCount;
	int					m_iLoseStreakCount;
	int					m_iTierPoint;



	int					m_iWinCount;
	int					m_iLoseCount;
	
	
	
	
	int					m_iUserLevel;
	
	int					m_iMatchNodeIndex;
	int					m_iScale;
	eMatchState			m_eMatchState;
	DWORD				m_dwStartTime;
	DWORD				m_dwWinStreakStartTime;
	//int					m_iSearchCount;
	CTime				m_ctStartTime;
	CTime				m_ctWinStreakStartTime;
	bool				m_bWinStreadMatch;

public:

	DWORD				GetUserIndex(){			return m_dwUserIndex;		}
	eMatchState			GetMatchState(){		return m_eMatchState;		}
	int					GetMatchNodeIndex(){	return m_iMatchNodeIndex;	}
	int					GetMatchPoint(){		return m_iMatchPoint;		}
	int					GetUserLevel(){			return m_iUserLevel;		}
	DWORD				GetStartTime(){			return m_dwStartTime;		}
	DWORD				GetWinSterakStartTime(){			return m_dwWinStreakStartTime;		}
	
	//int					GetSearchCount(){		return m_iSearchCount;		}
	int					GetWinStreakCount(){	return m_iWinStreakCount;	}
	int					GetLoseStreakCount(){	return m_iLoseStreakCount;	}
	int					GetUserTierPoint(){		return m_iTierPoint;		}
	CTime				GetStartctTime(){		return m_ctStartTime;		}
	CTime				GetWinStreakStartctTime()	{		return m_ctWinStreakStartTime;		}
	int					GetUserScale()	{	return m_iScale; }


	void				SetMatchUser( DWORD dwUserIndex, int iMatchPoint, int iWinStreakCount, int iTierPoint, int iLoseStreakCount );
	void				SetMatchState( eMatchState eStat ){		m_eMatchState = eStat;		}
	void				SetStartTime();
	void				SetWinStreakStartTime();
	void				SetMatchNodeIndex( int iIndex ){	m_iMatchNodeIndex = iIndex;		}
	//void				SetSearchCount( int count){			m_iSearchCount = count;			}

	bool				IsTimeOver();

	bool				IsWinStreadMatch();
	void				SetWinStreadMatch( bool bWinStreak );
	void				SetUserScale( int iScale )	{	m_iScale = iScale;  }
	
};

typedef struct tagMatchUserSort
{
	DWORD		m_dwUserIndex;
	int			m_iMatchPoint;
	int			m_iUserLevel;

	tagMatchUserSort()
	{
		m_dwUserIndex	= 0;
		m_iMatchPoint	= 0;
		m_iUserLevel	= 0;
	}

}MatchUserSort;

typedef vector< MatchUserSort > vMatchUserSort;

class SortMatchUser
{
public:
	bool operator()( const MatchUserSort& lhs , const MatchUserSort& rhs ) const
	{
		//if( lhs.m_iMatchPoint > rhs.m_iMatchPoint )
		if( lhs.m_iUserLevel > rhs.m_iUserLevel )
			return true;
		
		return false;
	}
};

class MatchNode
{
public:
	MatchNode( int iIndex = 0 );
	virtual ~MatchNode();

	void				Init();

protected:

	int					m_iIndex;

	vMatchUser			m_vMatchUser;
	DWORDVec			m_vMatchBlueTeam;
	DWORDVec			m_vMatchRedTeam;

public:

	int					GetIndex(){		return m_iIndex;		}
	int					GetMatchingScore( MatchUser* pUser );
	int					GetAverageMatchPoint();
	//int					GetMatchStep( int iAvgPoint, int iMatchPoint );
	int					GetMatchStep( int iMatchPoint );
	int					GetTotalStreakCount();
	void				GetUserList( DWORDVec& vUserIndex );
	

	BYTE				GetUserCount(){		return m_vMatchUser.size();	}
	bool				IsDelete();

	void				RemoveMatchUser( MatchUser* pUser );
	void				FillTeamList( SP2Packet &rkPacket );
};


//내림차순 정렬
class MatchSort
{
public:
	bool operator()( const MatchUser *lhs , const MatchUser *rhs ) const
	{
		if( lhs->m_iMatchPoint > rhs->m_iMatchPoint )
			return true;
		return false;
	}
};
