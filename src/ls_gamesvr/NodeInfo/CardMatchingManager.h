#pragma once

#include "../Util/Singleton.h"


class CardMatchingManager : public Singleton< CardMatchingManager >
{
public:
	struct stCardMatchingReward 
	{
		int m_iType;
		DWORD m_dwValue1;
		DWORD m_dwValue2;

		stCardMatchingReward()
		{
			m_iType		= 0;
			m_dwValue1	= 0;
			m_dwValue2	= 0;
		}
	};

	typedef std::map< int, stCardMatchingReward > mapAllReward;		// 전체 보상품 map
	
private:
	int m_iState;								// 보상 상태
	int m_iPeriod;								// 선물함 보관 기간
	DWORD m_dwMent;								// 선물함 멘트 코드
	DWORD m_dwTimeLimitSec;						// 제한 시간 
	float m_fClover_M_Prob;						// 일반 카드 사용시 클로버 확률
	
	mapAllReward m_mapCardMatchingSectionReward;	// 일반 보상 
	mapAllReward m_mapCardMatchingLuckyReward;		// 미션 보상 	
	
public:
	CardMatchingManager();
	virtual ~CardMatchingManager();

	void Init();
	void Destroy();

public:
	BOOL LoadINIData( const ioHashString &rkFileName );

	// 보상 구성품 전체 반환
	CardMatchingManager::mapAllReward GetMapCardMatchingSectionReward() const { return m_mapCardMatchingSectionReward; }
	CardMatchingManager::mapAllReward GetMapCardMatchingLuckyReward() const { return m_mapCardMatchingLuckyReward; }

	DWORD GetMent() const { return m_dwMent; }
	int GetPeriod() const { return m_iPeriod; }
	int GetState() const { return m_iState; }
	DWORD GetTimeLimitSec() const { return m_dwTimeLimitSec; }
	float GetCloverProb() const { return m_fClover_M_Prob; }
	
public:
	static CardMatchingManager& GetSingleton();
};

#define g_CardMatchingMgr CardMatchingManager::GetSingleton()