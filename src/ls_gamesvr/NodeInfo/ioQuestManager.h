#ifndef _ioQuestManager_h_
#define _ioQuestManager_h_

#include "../Util/Singleton.h"
#include "QuestVariety.h"

/**************************************************************************************/
/* 퀘스트의 발생은 클라이언트에서 체크 함											  */
/* 퀘스트의 완료는 최대한 클라이언트와 서버 모두 체크해야 어뷰즈를 막을 수 있으나	  */
/* 기능에 따라서 서버 체크가 부담스러울 수 있다. 이 경우 판단을 1회만하면 완료되는	  */
/* (낚시하기) 퀘스트는 클라이언트에서 체크해도 될 듯하다.							  */
/**************************************************************************************/

enum QuestPerform       // 퀘스트 수행 방식
{
	QP_NORMAL = 1,      // 기본 퀘스트
	QP_EVENT  = 2,      // 이벤트 퀘스트
};

enum QuestState
{
	QS_PROGRESS = 0,    // 진행중 - 받은 상태
	QS_ATTAIN,			// 달성 - 퀘스트 달성했지만 보상은 받지 않았음
	QS_COMPLETE,        // 완료 - 보상 받았음.
};

enum QuestOccur         // 퀘스트 발생 조건 타입 1
{
	QO_NONE = 0,
	QO_ENTER_LOBBY,	
	QO_ENTER_BATTLE_PVE,
	QO_ENTER_BATTLE_PVP,
	QO_BATTLE_PVP_FINAL_RESULT,
	QO_BATTLE_FINAL_RESULT,
	QO_PLAZA_ENTER,
	QO_FISHING_SUCCESS,
	QO_FISHING_FAILED,
	QO_FISHING_LEVELUP,
	QO_FISHING_SELL,
	QO_SOLDIER_ACQUIRE,
	QO_GRADE_EXP_ACQUIRE,
	QO_SOLDIER_EXP_ACQUIRE,
	QO_LOBBYNPLAZA_ENTER,
	QO_GAME_LOGIN,
	QO_PVE_ROUND_CLEAR,
	QO_ENTER_ROOM_PVE,
	QO_ENTER_ROOM_PVP,
	QO_EXCAVATION_LEVELUP,
	QO_ETCITEM_USE,
	QO_CAMP_SEASON_REWARD,        
	QO_GAME_LOGIN_DORMANT,
	QO_PCROOM_AUTHORITY,
	QO_FRIEND_RECOMMEND,        
	QO_GAME_LOGIN_DORMANT_CUSTOM,
	QO_PRACTICE_SUCCESS,
};

namespace QuestClass
{
	static ioHashString QCN_PRACTICE_SUCCESS = "QuestPracticeSuccess";
};


class User;
class ioQuestManager : public Singleton< ioQuestManager >
{
protected:
	typedef std::vector< QuestParent * > vQuestVariety;
	vQuestVariety m_QuestVariety;

	// 보상 리스트 - 선물로 지급함.
protected:
	struct QuestReward
	{
		int m_iPresentType;
		int m_iPresentState;
		int m_iPresentMent;
		int m_iPresentValue1;
		int m_iPresentValue2;
		int m_iPresentValue3;
		int m_iPresentValue4;
		int m_iPresentPeriod;
		bool m_bDirectReward;
		QuestReward()
		{
			m_iPresentType = m_iPresentState = m_iPresentMent = m_iPresentValue1 = m_iPresentValue2 = m_iPresentValue3 = m_iPresentValue4 = m_iPresentPeriod = 0;
			m_bDirectReward = false;
		}
	};
	typedef std::map< DWORD, QuestReward > QuestRewardMap;
	QuestRewardMap m_QuestRewardMap;

protected:
	DWORD m_dwCurrentTime;

protected:
	DWORD m_dwOneDayQuestHour;
	DWORD m_dwNextOneDayQuestDate;

protected:
	void ClearQuestVariety();

protected:
	QuestParent *CreateQuest( const ioHashString &rClassName );
	void SetQuest(char* csClassName, DWORD dwMainIndex, DWORD dwSubIndex, ioINILoader &rkLoader, bool bCreate = false, DWORD dwChangeSubIndex = 0, DWORD dwChangeNextSubIndex = 0);

public:
	void LoadINIData();

public:
	QuestParent *GetQuest( DWORD dwMainIndex, DWORD dwSubIndex );

public:
	bool SendRewardPresent( User *pSendUser, DWORD dwPresentID );
	void GetRewardPresent(DWORD dwPresentID, int& iType, int& iCode, int& iCount, int& iParam);

public:
	bool IsSameChanneling( User *pSendUser, DWORD dwMainIndex, DWORD dwSubIndex );

public:
	DWORD GetPrevOneDayQuestDate();

public:
	void ProcessQuest();
	void SendAliveEventQuest( User *pSendUser );

protected:
	void ProcessOneDayQuest();

	void CheckActiveQuest();
public:
	static ioQuestManager& GetSingleton();

public:   
	ioQuestManager();
	virtual ~ioQuestManager();
};
#define g_QuestMgr ioQuestManager::GetSingleton()
#endif