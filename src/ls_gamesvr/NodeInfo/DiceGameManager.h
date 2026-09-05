#pragma once

#include "../Util/Singleton.h"
#include "../DataHeaders/SnakeLadders_Move_Info.h"
#include "../DataHeaders/SnakeLadders_Reward_Info.h"

// DiceGame
// DiceGameManager

class DiceGameManager : public Singleton< DiceGameManager >
{
public:
	struct stDiceGameReward 
	{
		BYTE m_byIndex;
		DWORD m_dwRate;
		int m_iType;
		DWORD m_dwValue1;
		DWORD m_dwValue2;

		stDiceGameReward()
		{
			m_dwRate	= 0;
			m_byIndex	= 0;
			m_iType		= 0;
			m_dwValue1	= 0;
			m_dwValue2	= 0;
		}
	};

	typedef std::map< int, stDiceGameReward > mapOneStepReward;	// 한 단계 별 보상품 map
	typedef std::map< int, mapOneStepReward > mapAllReward;			// 전체 보상품 map
	typedef std::map< int, DWORD > mapInvalidityRate;				// 각 단계 별 개발자K 날아갈 확률 map

protected:

	typedef std::vector<SnakeLadders_Move> vecSNAKELADDER_MOVE;
	typedef boost::unordered_map<SHORT, vecSNAKELADDER_MOVE> mapSNAKELADDER_MOVE_INFO;

	typedef std::vector<SnakeLadders_Reward> vecSNAKELADDER_REWARD;
	typedef boost::unordered_map<int, vecSNAKELADDER_REWARD> mapSNAKELADDER_REWARD_INFO;
	//typedef boost::unordered_map<int, vecSNAKELADDER_REWARD> mapSNAKELADDER_REWARD_INFO;
	



private:

	//////////////////////////////////////////////////////////////////
	bool m_bDiceGameOpen;						// dicegame on/off


	mapSNAKELADDER_MOVE_INFO		m_mapSnakeLadder_Move_Info;	
	mapSNAKELADDER_REWARD_INFO		m_mapSnakeLadder_Reward_Info;

	///
	ioHashString m_szSendID;					// 보상 제공자 : DevK
	int m_iState;								// 보상 상태
	int m_iPeriod;								// 선물함 보관 기간
	DWORD m_dwMent;								// 선물함 멘트 코드
	int	m_iRatio;								// 기존 사용했던 판이 나올 확률

///////////////////////////////////////////////////////////////


	
	int m_iInvalidityMax;						// 구멍 잔여 개수 max
	mapInvalidityRate m_mapInvalidityRate;		// DevK 날아갈 확률 (구멍 잔여 개수에 따른 레벨 단계)

	int m_iRewardStepMax;						// 보상 단계 개수
	mapAllReward m_mapDiceGameReward;			// 보상 구성품
		
private:
	DWORD m_dwRewardRandomMax[OAK_BARREL_HOLE];	// 보상 확률 랜덤 시드


protected:
	SnakeLadders_Move_Info*		m_pSnakeLadderMoveInfoDat;
	SnakeLadders_Reward_Info*	m_pSnakeLadderRewardInfoDat;

public:
	DiceGameManager();
	virtual ~DiceGameManager();

	void Init();
	void Destroy();

public:
	//////////////////////////////////////////////////////////////////
	BOOL LoadINIData( const ioHashString &rkFileName );

	int  GetBoardTotalCount() {return  m_mapSnakeLadder_Move_Info.size(); }
	int  GetRewardTotalCount() {return  m_mapSnakeLadder_Reward_Info.size(); }

	int  GetRNDRewardIndex(int iGroupNum);
	BYTE GetRNDBoradIndex( BYTE BoradIndex);
	bool GetRewardInfoByIndex(int iGroupNum, int iIndex,int iArrNum, ioUserEtcItem::ETCITEMSLOT &rkEtcItem);
	bool GetMoveEndPosition(int iGroupNum, int iStart, int &End);
	bool IsSnakePoint(int iGroupNum,int iPosition);

	ioHashString GetSendID() const { return m_szSendID; }
	DWORD GetMent() const { return m_dwMent; }
	int GetPeriod() const { return m_iPeriod; }
	int GetState() const { return m_iState; }
	int GetRatio() const { return m_iRatio; }	
	//////////////////////////////////////////////////////////////////


	// 오크통 게임 활성화/비활성화 flag
	bool GetDiceGameOpen(){ return m_bDiceGameOpen; }

	// 구멍 최대 개수
	const int GetHoleMax(){ return m_iInvalidityMax; }
	// 보상 단계 최대 개수
	const int GetRewardStepMax(){ return m_iRewardStepMax; }
	
	// 단계별 개발자K 날아갈 확률
	const DWORD GetInvalidityRate( int iStep );
	// 단계별 보상 구성품들. 확률 계산 뒤 해당 구성품 반환
	const void GetOneStepReward( int iStep, BYTE &byIndex );

	// 오크통 보상 구성품 전체 반환
	DiceGameManager::mapAllReward GetMapDiceGameReward() const { return m_mapDiceGameReward; }

public:
	static DiceGameManager& GetSingleton();
};

#define g_DiceGameMgr DiceGameManager::GetSingleton()