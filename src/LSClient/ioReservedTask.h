#pragma once

#include <vector>


// 예약된 명령어 종류
enum ReservedTaskTypes
{
	RESERVEDCMD_TYPE_NONE = 0,
	RESERVEDCMD_TYPE_SERVERCHANGE,
	RESERVEDCMD_TYPE_ATTENDANCE,
	RESERVEDCMD_TYPE_PCROOOM,
	RESERVEDCMD_TYPE_TOURNAMENT,
	RESERVEDCMD_TYPE_ROLLBOOK,
	RESERVEDCMD_TYPE_GUILD_RANK_REWARD,
	RESERVEDCMD_TYPE_END,
};

// 예약된 명령어 조건
enum ReservedTaskWhens
{
	RESERVEDCMD_WHEN_NONE = 0,
	RESERVEDCMD_WHEN_ENTERLOBBY,				// 로비에 입장했을 때 실행
	RESERVEDCMD_WHEN_ENTERROOM,					// 방에 입장했을때 실행
	RESERVEDCMD_WHEN_ENTERHEADQUATER,			// 본부에 입장했을때 실행
	RESERVEDCMD_WHEN_RECV_ATTENDANCE,
	RESERVEDCMD_WHEN_RECV_ATTENDANCE_REWARD,
	RESERVEDCMD_WHEN_RECV_PCROOOM,
	RESERVEDCMD_WHEN_RECV_TOURNAEMNT_REWARD,
	RESERVEDCMD_WHEN_ROLLBOOK,					// 누적출석부
	RESERVEDCMD_WHEN_RECV_GUILD_RANK_REWARD,	//길드 랭크 보상

	RESERVEDCMD_WHEN_END,
};

// 명령어 데이타를 담는 클래스
struct ioTask
{
	ioTask()
	{
		eType	= RESERVEDCMD_TYPE_NONE;
		eWhen	= RESERVEDCMD_WHEN_NONE;
		bEraseAfterExecution = false;
	}
	virtual ~ioTask()
	{}

	ReservedTaskTypes eType;
	ReservedTaskWhens eWhen;
	bool bEraseAfterExecution;
};

// 서버이동 예약
struct ioTaskServerMove : ioTask
{
	ioTaskServerMove()
	{
		iServerIndex	= 0;
	}
	int iServerIndex;
};

//출석체크 후 보상 정보 정보 확인
struct ioTaskAttendanceReward : ioTask
{
	ioTaskAttendanceReward()
	{
		iTodayPresentType = iTodayValue1 = iTodayValue2 = iAccruePresentType = iAccrueValue1 = iAccrueValue2 = 0;
		dwDate = 0;
	}
	int iTodayPresentType;
	int iTodayValue1;
	int iTodayValue2;

	int iAccruePresentType;
	int iAccrueValue1;
	int iAccrueValue2;
	DWORD dwDate;
};

//대회 보상
struct ioTaskTournamentReward : ioTask
{
	ioTaskTournamentReward()
	{
		AttainTourPos = 0;
		dwStartDate = dwCheerPeso = 0;
		iMyCampPos = iWinCampPos = iLadderBonusPeso = iLadderRank = iLadderPoint = 0;
		m_eQuestProgress = QP_NONE;
	}

	BYTE AttainTourPos;
	DWORD dwStartDate, dwCheerPeso;
	int iMyCampPos, iWinCampPos, iLadderBonusPeso, iLadderRank, iLadderPoint;
	ioHashString szWinnerTeamName;

	enum QuestProgress
	{
		QP_NONE,
		QP_OCURR,
		QP_COMPLETE,
	};
	QuestProgress m_eQuestProgress;
};

//누적출석체크 타입 및 정보
struct ioTaskRollBook: ioTask
{
	ioTaskRollBook()
	{
		iTablePage = iPosition = iState = dwReceiveTime = 0;
	}
	DWORD	dwReceiveTime;
	int		iState;
	int		iTablePage;
	int		iPosition;
	
};

struct ioTaskGuildRankReward: ioTask
{
	ioTaskGuildRankReward()
	{
		iPresentType = iPresentValue1 = iPresentValue2 = 0;
	}
	int iPresentType;
	int iPresentValue1;
	int iPresentValue2;

};

// 예약 데이타 관리 클래스
class ioReservedTask : public Singleton< ioReservedTask >
{
protected:
	typedef std::vector<ioTask*> vReservedTask;
	vReservedTask m_vReservedTask;

public:
	static ioReservedTask& GetSingleton();

public:
	const ioTask* const GetTask( ReservedTaskTypes eTaskType, ReservedTaskWhens eWhen );

	void AddReservedTask( ioTask* pTask );
	void ExecuteReservedTask( ReservedTaskWhens eWhen );

	void RemoveTask( const ReservedTaskTypes eType );
	
	int GetReservedTaskCount()	{ return m_vReservedTask.size(); }

protected:
	bool OnExecute( ioTask* pTask );
	bool OnExecuteServerChange( ioTask* pTask );
	bool OnExecuteRecvAtttendance( ioTask* pTask );
	bool OnExecuteRecvPCRoom( ioTask* pTask );
	bool OnExecuteRecvTournament( ioTask* pTask );
	bool OnExecuteRecvRollBook( ioTask* pTask );
	bool OnExecuteRecvGuldRankReward( ioTask* pTask );

public:
	void Init();
	void Destroy();

	ioReservedTask(void);
	virtual ~ioReservedTask(void);
};

#define g_RserveTask ioReservedTask::GetSingleton()