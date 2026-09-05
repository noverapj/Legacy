#ifndef _PracticeNode_h_
#define _PracticeNode_h_
#pragma once
#include "boost\unordered_map.hpp"


#include "PracticeNodeManager.h"

#define PRACTICERANK 99999999

class Practicer
{
public:
	Practicer(DWORD dwAccountIdx, ioHashString szNickName, int iPracticeIdx, int iPracticeTime, CTime UpdateTime) 
		: m_dwAccountIdx(dwAccountIdx), m_szNickName(szNickName), m_iPracticeIdx(iPracticeIdx), m_iPracticeTime(iPracticeTime), m_iPracticeRank(PRACTICERANK), m_tPracticeUpdateTime(UpdateTime)
	{}

	void  Init();

	void SetAccountIdx(DWORD dwAccountIdx)	{	m_dwAccountIdx = dwAccountIdx;	}
	DWORD GetAccountIdx()		{ return m_dwAccountIdx;	}

	void SetNickName(ioHashString szNickName)	{	m_szNickName = szNickName;	}
	ioHashString GetNickName()		{ return m_szNickName;	}

	void SetPracticeIdx(int iPracticeIdx)	{	m_iPracticeIdx = iPracticeIdx;	}
	int GetPracticeIdx()		{ return m_iPracticeIdx;	}

	void SetPracticeTime(int iPracticeTime)	{	m_iPracticeTime = iPracticeTime;	}
	int GetPracticeTime()		{ return m_iPracticeTime;	}

	void SetPracticeRank(int iPracticeRank)	{	m_iPracticeRank = iPracticeRank;	}
	int GetPracticeRank()		{ return m_iPracticeRank;	}

	void SetPracticeUpdateTime(CTime tPracticeUpdateTime)	{	m_tPracticeUpdateTime = tPracticeUpdateTime;	}
	CTime GetPracticeUpdateTime()		{ return m_tPracticeUpdateTime;	}

private:
	DWORD	m_dwAccountIdx;
	ioHashString m_szNickName;
	int		m_iPracticeIdx;
	int		m_iPracticeTime;
	int		m_iPracticeRank;
	CTime	m_tPracticeUpdateTime;              // 수련장 타임변경시간
};




// 래더 포인트로 팀 정렬
class PracticerSort
{
public:
	bool operator()( Practicer* lhs , Practicer* rhs ) const
	{
		if(lhs->GetPracticeTime() < rhs->GetPracticeTime())
		{
			return true;
		}
		if(lhs->GetPracticeTime() == rhs->GetPracticeTime())
		{
			if(lhs->GetPracticeUpdateTime() < lhs->GetPracticeUpdateTime())
			{
				return true;
			}
		}

		return false;
	}
};

typedef boost::unordered_map<DWORD, Practicer* > mPracticer;
typedef std::vector<Practicer*> vPracticer;
typedef mPracticer::iterator mPracticer_iter;
typedef std::pair<DWORD, Practicer*> mypair;



class CPracticeNode
{
private:
	int m_iPracticeIdx;
	mPracticer m_mPracticer;
	vPracticer	m_vPracticer;

public:
	CPracticeNode();
	~CPracticeNode();

	void Init();

	void SetPracticeIdx(int iPracticeIdx)	{	m_iPracticeIdx = iPracticeIdx;	}
	int GetPracticeIdx()		{ return m_iPracticeIdx;	}

	bool IsExistPracticer( DWORD dwAccountIdx);

	Practicer* FindPracticer( DWORD dwAccountIdx );

	Practicer* FindPracticerNumber( int iNumber );

	Practicer* CreatePracticer( DWORD dwAccountIdx, ioHashString szNickName, int iPracticeTime, CTime Update_time );

	void SortAll();

	void BlockUserDelete(DWORD dwUserIndex);
	void NickNameChange(DWORD dwUserIndex, ioHashString	szNickname);
};


#endif //_PracticeNode_h_