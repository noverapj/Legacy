#pragma once
#include "MissionTriggerBase.h"
#include "MissionData.h"
class User;

class ioMission
{
public:
	ioMission();
	virtual ~ioMission();

	void Init();
	void Destroy();
	void Initialize( User* pUser );

	void MissionTableDestroy();

public:
	void DBtoData(CQueryResultData *query_data);

	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

protected:
	void SQLUpdateMission( MissionData *pMissionData );
	void SQLInitMissionType( int iMissionType );

public:
	void TriggerMission(const MissionClasses eMissionClass, DWORDVec& vValues, BOOL bMacro = FALSE);
	void InsertMission(const DWORD dwCode, MissionState eState, MissionTypes eType, const int iValue);
	void SaveTimeMission();

public:
	BOOL IsComplete(MissionData* pMissionData);
	BOOL IsAliveMission(const DWORD dwCode);

public:
	MissionData* GetMission(const DWORD dwCode);
	int	GetCompletedTypeCount(const int iType);

	BOOL IsRenewalData() { return m_bRenew; }
	void SetRenewalFlag(BOOL bFlag) { m_bRenew = bFlag; }

	void AddCompletedTypeCount(const int iType);
	void InitCompletedTypeCount(const int iType);

	BOOL IsMissionTableEmpty() { return m_vMissionTable.empty(); }

	void InitMissionTypes(IntVec& vResetList);
	void InitMissionType(int iMissionType);

	void FillMissionTableInfoIntoPacket(SP2Packet& kPacket);
	void DeleteMissionData(const int iType);

	int RecvCompensation(const DWORD dwCode);

	void TimeMissionCheck(const DWORD dwCode);
	BOOL IsTimeMission(const DWORD dwCode);

	BOOL IsPrevData();

	void MacroSetActiveMissionValue(SP2Packet& kPacket);

protected:
	typedef std::vector<MissionData*> vMissionData;

protected:
	vMissionData m_vMissionTable;

	IntVec m_vCompletedTypeCount;	//완료한 일일,주, 월 미션 수. 연관된 미션 처리 후 0으로 초기화.

	User* m_pUser;
	
	BOOL m_bRenew;
};