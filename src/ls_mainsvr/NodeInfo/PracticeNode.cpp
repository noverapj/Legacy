#include "../stdafx.h"
#include "PracticeNode.h"
#include "boost/range/algorithm/partial_sort.hpp"
#include "../DataBase/DBClient.h"

extern CLog OperatorLOG;

void Practicer::Init()
{
	m_szNickName.Clear();
}

CPracticeNode::CPracticeNode()
{
	m_mPracticer.clear();
	m_vPracticer.clear();
}


CPracticeNode::~CPracticeNode()
{
	Init();
}


void CPracticeNode::Init()
{
	mPracticer_iter iter  = m_mPracticer.begin();

	for( ; iter != m_mPracticer.end(); iter++ )
	{
		Practicer *pNode = iter->second;
		if( !pNode ) continue;

		pNode->Init();
		SAFEDELETE( pNode );
	}

	m_mPracticer.clear();
	m_vPracticer.clear();
}


bool CPracticeNode::IsExistPracticer( DWORD dwAccountIdx)
{
	mPracticer_iter findIter	= m_mPracticer.find(dwAccountIdx);
	if( findIter == m_mPracticer.end() )
		return false;

	return true;
}

Practicer* CPracticeNode::FindPracticer( DWORD dwAccountIdx )
{
	mPracticer_iter findIter	= m_mPracticer.find(dwAccountIdx);
	if( findIter == m_mPracticer.end() )
		return NULL;

	return findIter->second;
}

Practicer* CPracticeNode::FindPracticerNumber( int iNumber )
{
	if(m_vPracticer.size() == 0)
	{
		return NULL;
	}

	if(m_vPracticer.size() < iNumber)
	{
		return NULL;
	}
	return m_vPracticer[iNumber-1];
}


Practicer* CPracticeNode::CreatePracticer( DWORD dwAccountIdx, ioHashString szNickName, int iPracticeTime, CTime Update_time  )
{
	Practicer* pkPracticer = FindPracticer(dwAccountIdx );
	if(NULL != pkPracticer)
	{
		pkPracticer->SetPracticeTime(iPracticeTime);
	}
	else
	{
		Practicer* pkPracticer = new Practicer(dwAccountIdx, szNickName, m_iPracticeIdx, iPracticeTime, Update_time);
		m_mPracticer.insert( make_pair(dwAccountIdx, pkPracticer) );
		m_vPracticer.push_back(pkPracticer);
	}
	
	return pkPracticer;
}

void CPracticeNode::SortAll()
{
	mPracticer_iter iter  = m_mPracticer.begin();
    std::sort(m_vPracticer.begin(), m_vPracticer.end(), PracticerSort());

    for (int i = 0; i < m_vPracticer.size(); ++i)
	{	
		DWORD dwAI = m_vPracticer[i]->GetAccountIdx();
		m_vPracticer[i]->SetPracticeRank(i+1);
    }
}


void CPracticeNode::BlockUserDelete(DWORD dwUserIndex)
{
	if( 0 == dwUserIndex )
	{
		OperatorLOG.PrintTimeAndLog(0, "CPracticeNode::BlockUserDelete - %d", dwUserIndex);
		return ;
	}
	mPracticer_iter findIter	= m_mPracticer.find(dwUserIndex);
	if( findIter == m_mPracticer.end() )
	{
		OperatorLOG.PrintTimeAndLog(0, "CPracticeNode::BlockUserDelete Not User - %d", dwUserIndex);
		return ;
	}
	
	m_mPracticer.erase( dwUserIndex );
	
	// 노드에서 제외
	vPracticer::iterator iter = m_vPracticer.begin();
	while( iter != m_vPracticer.end() )
	{
		Practicer* pkPractice = *iter;
		if( pkPractice->GetAccountIdx() == dwUserIndex )
		{
			SAFEDELETE( pkPractice );
			m_vPracticer.erase( iter );
			break;
		}		
		++iter;
	}
	
	OperatorLOG.PrintTimeAndLog(0, "CPracticeNode::BlockUserDelete Kick User - %d", dwUserIndex);
	return ;
}

void CPracticeNode::NickNameChange(DWORD dwUserIndex, ioHashString	szNickname)
{
	if( 0 == dwUserIndex )
	{
		return ;
	}

	Practicer* pkPracticer = FindPracticer(dwUserIndex );
	if(NULL != pkPracticer)
	{
		pkPracticer->SetNickName(szNickname);
	}
}
