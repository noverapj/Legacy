#ifndef _PracticeNodeManager_h_
#define _PracticeNodeManager_h_
#include "boost\unordered_map.hpp"


class CPracticeNode;
class ServerNode;

typedef boost::unordered_map<int, CPracticeNode * > mPracticeNode;
typedef mPracticeNode::iterator mPracticeNode_iter;

const int PracticeSectionCount = 4;
const int PracticePresentCOUNT = 5;
struct SPracticePresent
{
	int         m_iPracticeRewadType;				// 수련장 보상 타입
	int         m_iPracticeRewadCode;				// 수련장 보상 인덱스
	int			m_iPracticeRewadCount;				// 수련장 보상 인덱스
	SPracticePresent()
	{
		Init();
	}

	void Init()
	{
		m_iPracticeRewadType = m_iPracticeRewadCode = m_iPracticeRewadCount = 0;
	}
};

struct SPracticeReward
{
	int         m_iPracticeRewadStart;				// 수련장 보상 시작 순위
	int         m_iPracticeRewadEnd;				// 수련장 보상 종료 순위
	ioHashString m_szPracticeRewardID;
	int         m_iPracticeRewadment;				// 수련장 보상 메시지
	int         m_iPracticeRewadperiad;				// 수련장 보상 보관기간

	SPracticePresent m_kPresent[PracticePresentCOUNT];

	SPracticeReward()
	{
		Init();
	}

	void Init()
	{
		m_iPracticeRewadStart = m_iPracticeRewadEnd = m_iPracticeRewadment = m_iPracticeRewadperiad = 0;

		for(int k = 0;k < PracticePresentCOUNT;k++)
		{
			m_kPresent[k].Init();
		}
	}

	bool IsReward(int iNumber)
	{
		if( m_iPracticeRewadStart <= iNumber && iNumber  <= m_iPracticeRewadEnd)
		{
			return true;
		}
		return false;
	}
};

typedef std::vector< SPracticeReward > vSPracticeReward;

class CPracticeNodeManager : public SuperParent
{
protected:
	static CPracticeNodeManager *sg_Instance;

public:
	enum State_Type
	{
		ST_NONE			= 0,
		ST_PLAY_PROCEED = 1,
		ST_REWARD		= 2,
		ST_STANDBY		= 3,
		ST_DATA_INIT	= 4,
	};

protected:
	ioHashString m_szSenderId;
	mPracticeNode m_mPracticeNode;
	vSPracticeReward m_SPracticeReward;
	State_Type m_eStateType;

	// 보상처리 요일시간
	int m_iRankRewardWeek;
	int m_iRankRewardHour;
	int m_iRankRewardMinute;

	DWORD m_dwInitTime;
	bool m_bLoading;

public:
	static CPracticeNodeManager &GetInstance();
	static void ReleaseInstance();

private:     	/* Singleton Class */
	CPracticeNodeManager();
	virtual ~CPracticeNodeManager();

public:
	void ReleaseMemoryPool();

	void LoadINIData();

	bool IsExistPracticeNode( int iPracticeIndex);

	CPracticeNode* FindPracticeNode( int iPracticeIndex );

	CPracticeNode* CreatePracticeNode( int iPracticeIndex );

	void SortRankAll(bool bFirst);
	void SortRankPracticeIndex( int iPracticeIndex );

	void ProcessPractice();
	void ProcessState(State_Type eStateType);

	void PrecessReward();
	void PrecessDataInit();

	void INIList(ServerNode *pSender, DWORD dwUserIndex);

	void BlockUserDelete(DWORD dwUserIndex);

	void NickNameChange(DWORD dwUserIndex, ioHashString	szNickname);
};

#define g_PracticeNodeManager CPracticeNodeManager::GetInstance()

#endif //_PracticeNodeManager_h_
