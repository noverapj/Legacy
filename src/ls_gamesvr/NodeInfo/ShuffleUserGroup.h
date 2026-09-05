#pragma once
#include "UserNodeManager.h"

struct UserGroup
{
	IntVec m_vUserIndex;
	int    m_iKillDeathLevel;

	UserGroup()
	{
		Init();
	}

	void Init()
	{
		m_vUserIndex.clear();
		m_iKillDeathLevel = 0;
	}

	inline int GetUserCnt() const
	{
		return static_cast<int>( m_vUserIndex.size() );
	}

	int GetAverageLevel()
	{
		if( m_vUserIndex.empty() )
			return 0;

		return m_iKillDeathLevel / GetUserCnt();
	}

	void Add( DWORD dwUserIdex, int iKillDeathLevel )
	{
		m_vUserIndex.push_back( dwUserIdex );
		m_iKillDeathLevel += iKillDeathLevel;
	}
};
typedef std::list<UserGroup> UserGroupList;