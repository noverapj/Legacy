#pragma once
#include "boost\unordered_map.hpp"

class MatchNode;
class MatchUser;

typedef vector< MatchNode* > vMatchNode;
typedef vMatchNode::iterator vMatchNode_iter;
typedef boost::unordered_map<int, MatchNode*> mMatchNode;
typedef mMatchNode::iterator mMatchNode_iter;

typedef vector< MatchUser* > vMatchUser;
typedef vMatchUser::iterator vMatchUser_iter;
typedef boost::unordered_map<int, MatchUser*> mMatchUser;
typedef mMatchUser::iterator mMatchUser_iter;

typedef boost::unordered_multimap< int, MatchUser* > mUserMatchingMap;				// key : 蜡历 MMR 
typedef mUserMatchingMap::iterator findIter;				// key : 蜡历 MMR 

typedef struct tagMatchNodeSort
{
	MatchNode*	m_pNode;
	int			m_iScore;

	tagMatchNodeSort()
	{
		m_pNode		= NULL;
		m_iScore	= 0;
	}

}MatchNodeSort;

typedef vector< MatchNodeSort > vMatchNodeSort;
class SortMatchNode
{
public:
	bool operator()( const MatchNodeSort& lhs , const MatchNodeSort& rhs ) const
	{
		if( lhs.m_iScore > rhs.m_iScore )
			return true;
		
		return false;
	}
};

class MatchNodeManager
{
	static MatchNodeManager* sg_Instance;
	
protected:
	mMatchUser			m_mMatchUser;
	mUserMatchingMap	m_mMachingMap;							// key : 蜡历 MMR 备埃
	std::pair<mUserMatchingMap::iterator, mUserMatchingMap::iterator> m_machingRangeIter;


	MemPooler< MatchNode >	m_MatchMemNode;
	MemPooler< MatchUser >	m_UserMemNode;

	DWORDVec			m_vTierPoint;
	DWORDVec			m_vWinPoint;
	DWORDVec			m_vLosePoint;
	DWORDVec			m_vPlayTier;

protected:

	bool				m_bActive;

	int					m_iTopRankYear;
	int					m_iTopRankMonth;
	int					m_iTopRankDay;

	int					m_iWeekStartHour;
	int					m_iWeekEndHour;

	int					m_iWeekendStartHour;
	int					m_iWeekendEndHour;

	int					m_iVacationStartMonth;
	int					m_iVacationEndMonth;
	
	int					m_iVacationStartDay;
	int					m_iVacationEndDay;

	int					m_iVacationStartHour;
	int					m_iVacationEndHour;

	int					m_iScale;
	int					m_iExpansionTime;
	int					m_iWinStreakScale;
	int					m_iLoseStreakScale;
	int					m_iMatchTimeout;
	int					m_iMatchStep;

public:
	static MatchNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();
	void LoadINI();

public:
	bool IsActive(){ return m_bActive; }

public:
	
public:
	MatchNode* CreateMatchNode();	
	void RemoveMatchNode( MatchNode* pNode );

	MatchUser* CreateMatchUser( DWORD dwUserIndex, int iMatchPoint, int iWinStreakCount, int iLoseStreakCount );
	void RemoveMatchUser( MatchUser* pUser );

	bool IsExistMatchUser( DWORD dwUserIndex );

	MatchUser* FindMatchUser( DWORD dwUserIndex );

	void GetTierPoint( DWORDVec& vTier ){	vTier =  m_vTierPoint;  }
	int  GetExpansionTime(){		return m_iExpansionTime;	}
	int  GetScale(){		return m_iScale;	}
	int  GetWinStreakScale(){		return m_iWinStreakScale;	}
	int  GetLoseStreakScale(){		return m_iLoseStreakScale;	}
	int  GetMatchingTimeout(){		return m_iMatchTimeout;	}
	void GetTierPoint( int iMatchPoint, int& iStartTier, int& iEndTier, int& iTierPoint );

public:

	void MatchProcess();
	void RankSeasonProcess();

	void DoMatchUser( MatchUser* pUser );
	void SendMatchErrorResult( MatchUser* pUser, int iResult );
	void SendMatchSuccessResult( MatchUser* pNode, MatchUser* pCompetitionNode );

	MatchUser* GetMatchingUser( MatchUser* pUser, int iscale, mUserMatchingMap::iterator& );
	
private:
	MatchNodeManager();
	virtual ~MatchNodeManager();	
};
#define g_MatchNodeManager MatchNodeManager::GetInstance()
