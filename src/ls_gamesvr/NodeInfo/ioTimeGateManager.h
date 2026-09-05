#pragma once

#include "../Util/Singleton.h"


class ioTimeGate;

#define TIMEGATE_RANDOM_MAX 100

class ioTimeGateManager
{
private:
	static ioTimeGateManager *sg_Instance;
public:
	ioTimeGateManager();
	virtual ~ioTimeGateManager();

	
	//typedef std::vector< int, int > TimeGateRewardItemListVec;
	typedef std::map< int, IntOfTwo > TimeGateRewardListMap;

	

	typedef struct TimeGateInfo
	{
		int iList;
		int iItemCount;
		int iAccRand;			// 누적 랜덤
		int iRand;				// 리스트 랜덤
		int iAlarm;
		int iType;
		int iQuantity;
		
		
		TimeGateRewardListMap m_RewardListMap;

		TimeGateInfo()
		{
			iList		= 0;
			iItemCount	= 0;
			iAccRand	= 0;
			iRand		= 0;
			iAlarm		= 0;
			iType		= 0;
			iQuantity	= 0;
			m_RewardListMap.clear();
		}
	};

	void Init();
	void Destroy();
	int GetTimeGateCoolTime() { return m_iCoolTime; }
	int GetTimeGateRandomList();
	int GetTimeGateReward( int &iItemList, int &iPresentType, int&iValue1, int& iValue2, bool& bAlarm );
	void SendTimeGatePresent( int iPresentType, int iValue1, int iValue2, User* pUser  );

	static ioTimeGateManager& GetSingleton();

public:
	void LoadINI();
	int GetSameRandomReward( int iRand, TimeGateInfo& rewardInfo );
protected:
	void LoadTimeGate( ioINILoader &rkLoader );

public:
	bool m_bINILoading;
	IORandom m_RandomBoxRandom;
	int m_iPresent_ment;
	int m_iPresent_period;
	int m_iItemListCount;
	int m_iCoolTime;

	typedef std::map< DWORD, TimeGateInfo > TimeGateInfoMap;		
	TimeGateInfoMap m_TimeGateInfoMap;					// 타임 게이트 보상 목록
	
	typedef std::vector<TimeGateInfo> vTimeGateReward;
	vTimeGateReward m_vTimeGateReward;

	typedef vTimeGateReward::iterator vTimeGateReward_iter;

	typedef std::vector<int> vTimeGateRandom;
	vTimeGateRandom m_vTimeGateRandom;
};


//내림차순 정렬
class TimeGateRewardRandomSort
{
public:
	bool operator()( const ioTimeGateManager::TimeGateInfo &lhs , const ioTimeGateManager::TimeGateInfo &rhs ) const
	{
		if( lhs.iAccRand < rhs.iAccRand )
			return true;
		return false;
	}
};

//오름차순
class TimeGateRewardAscSort
{
public:
	bool operator()( const ioTimeGateManager::TimeGateInfo &lhs , const ioTimeGateManager::TimeGateInfo &rhs ) const
	{
		if( lhs.iRand < rhs.iRand )
			return true;
		return false;
	}
};

#define g_TimeGateMgr ioTimeGateManager::GetSingleton()
