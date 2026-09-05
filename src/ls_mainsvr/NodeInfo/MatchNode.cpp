#include "../stdafx.h"

#include "MatchNode.h"
#include "../EtcHelpFunc.h"


MatchUser::MatchUser()
{
	Init();
}

MatchUser::~MatchUser()
{
}

void MatchUser::Init()
{
	m_dwUserIndex = 0;
	m_iMatchPoint = 0;
	m_iWinStreakCount = 0;
	m_iLoseStreakCount = 0;
	m_iTierPoint  = 0;

	m_iWinCount = 0;
	m_iLoseCount = 0;
	m_iUserLevel = 0; 
	m_eMatchState = MatchReady;
	m_iMatchNodeIndex = 0;
	m_dwStartTime = 0;
	m_ctStartTime = 0;
	m_dwWinStreakStartTime = 0;
	m_ctWinStreakStartTime = 0;
	m_iScale		= 0;
	m_bWinStreadMatch	= false;
}

void MatchUser::SetMatchUser( DWORD dwUserIndex, int iMatchPoint, int iWinStreakCount, int iTierPoint, int iLoseStreakCount )
{
	m_dwUserIndex = dwUserIndex;
	m_iMatchPoint = iMatchPoint;
	m_iWinStreakCount = iWinStreakCount;
	m_iTierPoint  = iTierPoint;
	m_iLoseStreakCount = iLoseStreakCount;
}

void MatchUser::SetStartTime()
{
	m_dwStartTime =  Help::ConvertCTimeToDate( CTime::GetCurrentTime() );
	m_ctStartTime = CTime::GetCurrentTime();
}

void MatchUser::SetWinStreakStartTime()
{
	m_dwWinStreakStartTime =  Help::ConvertCTimeToDate( CTime::GetCurrentTime() );
	m_ctWinStreakStartTime = CTime::GetCurrentTime();
}


bool MatchUser::IsTimeOver()
{
	CTime kCurTime = CTime::GetCurrentTime();
	CTimeSpan kGapTime = kCurTime - m_ctStartTime;

	if( kGapTime.GetTotalSeconds() >= g_MatchNodeManager.GetMatchingTimeout() )
		return true;

	return false;
}

bool MatchUser::IsWinStreadMatch()
{
	return m_bWinStreadMatch;
}

void MatchUser::SetWinStreadMatch( bool bWinStreak )
{
	m_bWinStreadMatch = bWinStreak;
}


MatchNode::MatchNode( int iIndex ) : m_iIndex( iIndex )
{
	m_vMatchUser.clear();
	m_vMatchBlueTeam.clear();
	m_vMatchRedTeam.clear();
}

MatchNode::~MatchNode()
{
}

void MatchNode::Init()
{
	m_vMatchUser.clear();
	m_vMatchBlueTeam.clear();
	m_vMatchRedTeam.clear();
}

int	MatchNode::GetMatchingScore( MatchUser* pUser )
{
	// 0 ~ 1199 브론즈
	// 1200 ~ 1399 실버
	// 1400 ~ 1599 골드
	// 1600 ~ 1799 플래티넘
	// 1800 ~ 1999 다이아몬드
	// 2000 ~ max 마스터
	// 동일계급(1000) 1계급차(800) 2계급차(600) 3계급차(400) 4계급차(200) 5계급차(0)
	
	int iScore = 0;
	
	// 1. 같은 티어 찾기
	int istep = 0;
	istep = GetMatchStep( pUser->GetMatchPoint() );

	if( istep == 0 )
		iScore = 1000 - ( istep * 100 );

	LOG.PrintTimeAndLog( 0,"[info][match] 1.match [%d] user[%d] userpoint[%d] step[%d] score[%d] ", m_iIndex, pUser->GetUserIndex(), pUser->GetMatchPoint(), istep, iScore );

	// 2. 매칭확장시간 확인
	int iINIScale = g_MatchNodeManager.GetScale();
	if( iScore == 0 && iINIScale > 0 )
	{
		CTime ctCurTime = CTime::GetCurrentTime();
		CTimeSpan ctGapTime = ctCurTime - pUser->GetStartctTime();

		int iDelayTime = g_MatchNodeManager.GetExpansionTime();
		int iscale = ctGapTime.GetTotalSeconds() / iDelayTime;

		if( iscale == 0 )
			return iScore;

		if( iINIScale < iscale )
			iscale = iINIScale;

		istep = GetMatchStep( pUser->GetMatchPoint() );

		if( istep <= iscale ) 
			iScore = 1000 - ( istep * 100 );

		LOG.PrintTimeAndLog( 0,"[info][match] 2.match [%d] user[%d] userpoint[%d] score[%d] step[%d] scale[%d] delaytime[%d] INIdelaytime[%d]", m_iIndex, pUser->GetUserIndex(), pUser->GetMatchPoint(), iScore, istep, iscale, (int)ctGapTime.GetTotalSeconds(), iDelayTime );
	}

	return iScore;
}

void MatchNode::FillTeamList( SP2Packet &rkPacket )
{
	int iStartTier = 0;
	int iEndTier = 0;

	if( GetUserCount() != MAX_MATCHING_COUNT )
	{
		for( int i=0 ; i<MAX_MATCHING_COUNT ; ++i )
			PACKET_GUARD_VOID( rkPacket.Write( 0 ) );
		return;
	}
	else
	{
		static vMatchUserSort usersort;
		usersort.clear();

		vMatchUser_iter iter = m_vMatchUser.begin();
		for(iter = m_vMatchUser.begin();iter != m_vMatchUser.end();iter++)
		{
			MatchUser* pCursor = *iter;
			if( !pCursor )
				continue;

			MatchUserSort st;
			st.m_dwUserIndex = pCursor->GetUserIndex();
			st.m_iMatchPoint = pCursor->GetMatchPoint();
			st.m_iUserLevel  = pCursor->GetUserLevel();

			usersort.push_back( st );
		}

		if( (int)usersort.size() > 0 )
		{
			int iTemp = 0;
			g_MatchNodeManager.GetTierPoint( usersort[0].m_iMatchPoint, iStartTier, iEndTier, iTemp );
		}
		

		// 팀균형 맞추기
		std::sort( usersort.begin(), usersort.end(), SortMatchUser() );

		for( int i=0 ; i<(int)usersort.size()-1 ; ++i )
		{
			m_vMatchBlueTeam.push_back( usersort[i].m_dwUserIndex );
			m_vMatchRedTeam.push_back( usersort[i+1].m_dwUserIndex );
		}
	}
	

	for( int i=0 ;  i<(int)m_vMatchBlueTeam.size() ; i++)
	{
		PACKET_GUARD_VOID( rkPacket.Write( m_vMatchBlueTeam[i] ) );
	}

	for( int i=0 ;  i<(int)m_vMatchRedTeam.size() ; i++)
	{
		PACKET_GUARD_VOID( rkPacket.Write( m_vMatchRedTeam[i] ) );
	}

	PACKET_GUARD_VOID( rkPacket.Write( iStartTier ) );
	PACKET_GUARD_VOID( rkPacket.Write( iEndTier ) );

	LOG.PrintTimeAndLog( 0, "[info][match] starttier [%d] endtier [%d]", iStartTier, iEndTier );
}

int MatchNode::GetAverageMatchPoint()
{
	int iAvgPoint = 0;
	int iUserCount = 0;

	vMatchUser_iter iter = m_vMatchUser.begin();
	for(iter = m_vMatchUser.begin();iter != m_vMatchUser.end();iter++)
	{
		MatchUser* pCursor = *iter;
		if( !pCursor ) continue;

		iUserCount++;
		iAvgPoint += pCursor->GetMatchPoint();
	}

	if( iUserCount == 0 )
		return 0;

	iAvgPoint = iAvgPoint / iUserCount;
	return iAvgPoint;
}

int MatchNode::GetTotalStreakCount()
{
	int iTotalStreakCount = 0;

	vMatchUser_iter iter = m_vMatchUser.begin();
	for(iter = m_vMatchUser.begin();iter != m_vMatchUser.end();iter++)
	{
		MatchUser* pCursor = *iter;
		if( !pCursor ) continue;

		iTotalStreakCount += pCursor->GetWinStreakCount();
	}

	return iTotalStreakCount;
}

void MatchNode::GetUserList( DWORDVec& vUserIndex )
{
	vMatchUser_iter iter = m_vMatchUser.begin();
	for(iter = m_vMatchUser.begin();iter != m_vMatchUser.end();iter++)
	{
		MatchUser* pCursor = *iter;
		if( !pCursor ) continue;

		vUserIndex.push_back( pCursor->GetUserIndex() );
	}
}

bool MatchNode::IsDelete()
{
	vMatchUser_iter iter = m_vMatchUser.begin();
	for(iter = m_vMatchUser.begin();iter != m_vMatchUser.end();iter++)
	{
		MatchUser* pCursor = *iter;
		if( !pCursor ) continue;
		if( pCursor->GetMatchState() != MatchUser::MatchDelete )
			return false;
	}

	return true;
}

int MatchNode::GetMatchStep( int iMatchPoint )
{
	// 0 ~ 1199 브론즈
	// 1200 ~ 1399 실버
	// 1400 ~ 1599 골드
	// 1600 ~ 1799 플래티넘
	// 1800 ~ 1999 다이아몬드
	// 2000 ~ max 마스터

	int ipoint = 0, istep = 0;

	vMatchUser_iter iter = m_vMatchUser.begin();
	for(iter = m_vMatchUser.begin();iter != m_vMatchUser.end();iter++)
	{
		MatchUser* pCursor = *iter;
		if( pCursor )
		{
			ipoint = pCursor->GetMatchPoint();
			break;
		}
	}

	LOG.PrintTimeAndLog( 0,"[info][match] matchstep [%d] userpoint[%d] nodepoint[%d]", m_iIndex, iMatchPoint, ipoint );

	DWORDVec vTier;
	vTier.clear();
	g_MatchNodeManager.GetTierPoint( vTier );

	int icount = (int)vTier.size();

	for( int i=0 ; i<icount ; ++i )
	{
		if( i == 0 )
		{
			if( iMatchPoint <= vTier[i] )
			{
				if( ipoint <= vTier[i] )
				{
					return 0;
				}
				else
				{
					for( int k=0 ; k<icount ; ++k )
					{
						if( ipoint > vTier[k] && ipoint <= vTier[k+1] )
						{
							return k+1;
						}
					}
					return icount;
				}
			}
		}
		else if( i == icount-1 )
		{
			if( iMatchPoint > vTier[i] )
			{
				if( ipoint > vTier[i] )
				{
					return 0;
				}
				else
				{
					for( int k=0 ; k<icount ; ++k )
					{
						if( ipoint > vTier[k] && ipoint <= vTier[k+1] )
						{
							return i - k;
						}
					}
					return icount;
				}
			}
			else
			{
				if( iMatchPoint > vTier[i-1] && iMatchPoint <= vTier[i] )
				{
					if( ipoint > vTier[i-1] && ipoint <= vTier[i] )
					{
						return 0;
					}
					else
					{
						for( int k=0 ; k<icount ; ++k )
						{
							if( ipoint <= vTier[k] )
							{
								return i;
							}
						
							if( ipoint > vTier[k] && ipoint <= vTier[k+1] )
							{
								if( iMatchPoint < ipoint )
								{
									return k - i + 1;
								}
								else
								{
									return i - k - 1;
								}
							}
						}
						return icount - i;
					}
				}
			}
		}
		else
		{
			if( iMatchPoint > vTier[i-1] && iMatchPoint <= vTier[i] )
			{
				if( ipoint > vTier[i-1] && ipoint <= vTier[i] )
				{
					return 0;
				}
				else
				{
					for( int k=0 ; k<icount ; ++k )
					{
						if( ipoint <= vTier[k] )
						{
							return i;
						}
						
						if( ipoint > vTier[k] && ipoint <= vTier[k+1] )
						{
							if( iMatchPoint < ipoint )
							{
								return k - i + 1;
							}
							else
							{
								return i - k - 1;
							}
						}
					}
					return icount - i;
				}
			}
		}
	}

	return 0;
}
