#pragma once

#include "../Util/Singleton.h"


class ioOakBarrelManager : public Singleton< ioOakBarrelManager >
{
public:
	struct stOakBarrelReward 
	{
		BYTE m_byIndex;
		DWORD m_dwRate;
		int m_iType;
		DWORD m_dwValue1;
		DWORD m_dwValue2;

		stOakBarrelReward()
		{
			m_dwRate	= 0;
			m_byIndex	= 0;
			m_iType		= 0;
			m_dwValue1	= 0;
			m_dwValue2	= 0;
		}
	};

	typedef std::map< int, stOakBarrelReward > mapOneStepReward;	// 한 단계 별 보상품 map
	typedef std::map< int, mapOneStepReward > mapAllReward;			// 전체 보상품 map
	typedef std::map< int, DWORD > mapInvalidityRate;				// 각 단계 별 개발자K 날아갈 확률 map

private:
	bool m_bOakBarrelOpen;						// 오크통 게임 활성화/비활성화 flag
	int m_iLimitSwordMax;						// 오크통 일일 최대 칼 사용 수량
	int m_iState;								// 보상 상태
	int m_iPeriod;								// 선물함 보관 기간
	DWORD m_dwMent;								// 선물함 멘트 코드
	
	int m_iInvalidityMax;						// 구멍 잔여 개수 max
	mapInvalidityRate m_mapInvalidityRate;		// DevK 날아갈 확률 (구멍 잔여 개수에 따른 레벨 단계)

	int m_iRewardStepMax;						// 보상 단계 개수
	mapAllReward m_mapOakBarrelReward;			// 보상 구성품
		
private:
	DWORD m_dwRewardRandomMax[OAK_BARREL_HOLE];	// 보상 확률 랜덤 시드

public:
	ioOakBarrelManager();
	virtual ~ioOakBarrelManager();

	void Init();
	void Destroy();

public:
	BOOL LoadINIData( const ioHashString &rkFileName );

	// 오크통 게임 활성화/비활성화 flag
	bool GetOakBarrelOpen(){ return m_bOakBarrelOpen; }

	// 구멍 최대 개수
	const int GetHoleMax(){ return m_iInvalidityMax; }
	// 보상 단계 최대 개수
	const int GetRewardStepMax(){ return m_iRewardStepMax; }
	
	// 단계별 개발자K 날아갈 확률
	const DWORD GetInvalidityRate( int iStep );
	// 단계별 보상 구성품들. 확률 계산 뒤 해당 구성품 반환
	const void GetOneStepReward( int iStep, BYTE &byIndex );

	// 오크통 보상 구성품 전체 반환
	ioOakBarrelManager::mapAllReward GetMapOakBarrelReward() const { return m_mapOakBarrelReward; }

	DWORD GetMent() const { return m_dwMent; }
	int GetPeriod() const { return m_iPeriod; }
	int GetState() const { return m_iState; }
	int GetLimitSwordMax() const { return m_iLimitSwordMax; }

public:
	static ioOakBarrelManager& GetSingleton();
};

#define g_OakBarrelMgr ioOakBarrelManager::GetSingleton()