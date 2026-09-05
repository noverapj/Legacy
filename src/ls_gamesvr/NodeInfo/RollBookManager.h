#pragma once

class ioUserRollBook;

enum RollBookType
{
	RBT_NONE		= 0,
	RBT_NEWBIE		= 1,
	RBT_RETURN		= 2,
	RBT_DEFAULT		= 3,
};

enum RollBookRenewalType
{
	RBR_PROGRESS			= 0,
	RPR_CHANGE_RESET		= 1,
	RPR_CHANGE_RETURN		= 2,
};

class RollBookManager	: public Singleton< RollBookManager >
{
protected:
	struct RollBookInfo
	{
		int iRollBookType;
		int iNextRollBook;
		int iResetStartRollBook;
	};

public:
	struct RewardInfo
	{
		int iType;
		int iValue1;
		int iValue2;
		int iPresentPeriod;
		int iMent;

		RewardInfo()
		{
			iType			= 0;
			iValue1			= 0;
			iValue2			= 0;
			iPresentPeriod	= 0;
			iMent			= 0;
		}
	};

public:
	RollBookManager();
	virtual ~RollBookManager();

	void Init();
	void Destroy();

	void LoadInI(BOOL bReload = FALSE);
public:
	static RollBookManager& GetSingleton();

public:
	int GetNextStartTable(int iCurTable, BOOL bReset);
	BOOL SendReward(User* pUser, int iTable, int iNumber);
	void GetRewardInfo(int iTable, int iNumber, RewardInfo& stRewardInfo);

	BOOL IsReset(CTime& cCompareTime, int iCurTable);
	BOOL IsNewbie(CTime& cJoinTime);
	BOOL IsReturnUser(CTime& cCompareTime);

	int JudgmentUserRollBookType(CTime& cJoinTime, CTime& cLogoutTime);

	int GetFirstStartTable(int iUserType);
	int GetResetTerm(int iUserType);

	int GetNextRollBookLine(int iTable, int iCurLine);

	int GetRollBookType(int iTable);

	inline int GetRenewalHour() { return m_iRenewalHour; }

protected:
	typedef std::vector<RewardInfo> RewardVector;
	typedef boost::unordered_map<DWORD, RewardVector> AllRewardTable;
	//typedef boost::unordered_map<DWORD, DWORD> NextTableInfo;
	typedef boost::unordered_map<DWORD, RollBookInfo> RollBookTable;

protected:
	int m_iRenewalHour;

	int m_iNewbieJudgmentDay;
	int m_iReturnUserJudgementDay;

	int m_iNewbieStartTable;
	int m_iReturnUserStartTable;
	int m_iDefaultUserStartTable;

	int m_iNewbieResetTerm;
	int m_iReturnUserResetTerm;
	int m_iDefaultUserResetTerm;

	AllRewardTable m_mRewardTable;			//모든 출석 테이블 정보
	RollBookTable m_mRollBookTable;

	//NextTableInfo m_mNextTable;			//출석부를 완료 했을 경우 다음 출석부 정보
	//NextTableInfo m_mResetStartTable;	//리셋시 시작 테이블 정보
};

#define g_RollBookMgr RollBookManager::GetSingleton()