#pragma once

#include "ioUserTournament.h"


class SP2Packet;
class TeamGlobalData
{
protected:
	enum
	{
		DATA_SYNC_TIME_OTHER = 120000,
		DATA_SYNC_TIME_OWNER = 60000,
	};
protected:
	TeamData m_DefaultData;

	// 실시간 동기화 데이터
	SHORT m_Position;
	BYTE  m_MaxPlayer;
	int   m_iCheerPoint;
	BYTE  m_TourPos;			// 0은 탈락 & 예비 엔트리 / 1은 256강 / 2는 128강 ..
	int   m_iLadderPoint;
	BYTE  m_CampPos;
	
	DWORD m_dwSyncTime;
	bool  m_bUpdateMainUI;
	bool  m_bUpdateSubUI;
	bool  m_bUpdateInfoUI;

protected:
	bool IsAlreadyUser( DWORD dwUserIndex );

public:
	inline DWORD GetTourIndex(){ return m_DefaultData.m_dwTourIndex; }
	inline DWORD GetTeamIndex(){ return m_DefaultData.m_dwTeamIndex; }
	inline DWORD GetOwnerIndex(){ return m_DefaultData.m_dwTeamOwnerIndex; }
	inline ioHashString &GetTeamName(){ return m_DefaultData.m_szTeamName; }

	inline SHORT GetPosition(){ return m_Position; }
	inline BYTE  GetMaxPlayer(){ return m_MaxPlayer; }
	inline int   GetCheerPoint(){ return m_iCheerPoint; }
	inline BYTE  GetTourPos(){ return m_TourPos; }
	inline int   GetLadderPoint(){ return m_iLadderPoint; }
	inline BYTE  GetCampPos(){ return m_CampPos; }
	inline int   GetUserSize(){ return m_DefaultData.m_TeamUserList.size(); }

public:
	void SetPosition( SHORT Position ){ m_Position = Position; }
	void SetTourPos( BYTE TourPos ){ m_TourPos = TourPos; }
	void SetCheerPoint( int iCheerPoint ){ m_iCheerPoint = iCheerPoint; }
	void SetCampPos( BYTE CampPos ){ m_CampPos = CampPos; }

public:
	void SetDefaultData( TeamData &rkTeamData );
	void SetSyncData( SP2Packet &rkPacket );

public:
	void ClearUserList();
	void AddUserList( DWORD dwUserIndex, const ioHashString &rkUserNick );
	void GetUserList( TeamUserVec &rkUserList );
	void DeleteTeamUserData( DWORD dwUserIndex );

public:
	bool IsDataSync();
	void SendDataSync();

public:
	bool IsUpdateMainUI();
	bool IsUpdateSubUI();
	bool IsUpdateInfoUI();

public:
	TeamGlobalData();
	virtual ~TeamGlobalData();
};
typedef std::vector< TeamGlobalData * > TeamGlobalDataVec;
//////////////////////////////////////////////////////////////////////////
class ioTournamentTeamManager : public Singleton< ioTournamentTeamManager >
{
protected:
	TeamGlobalDataVec m_TournamentTeamList;           // 클라이언트 종료시까지 삭제 안함.
	
public:
	TeamGlobalData *GetTeamData( DWORD dwIndex, bool bDataSync = true );

public:
	TeamGlobalData *SetDefaultData( TeamData &rkTeamData, bool bNewSync = false  );
	TeamGlobalData *SetSyncData( DWORD dwTeamIndex, SP2Packet &rkPacket );
	
	void NewTeamGlobalData( DWORD dwTourIdx, DWORD dwTeamIdx );
	void SetFullData( SP2Packet &rkPacket );

public:
	void ClearTeamUserData( DWORD dwTeamIndex );
	TeamGlobalData *AddTeamUserData( DWORD dwTeamIndex, DWORD dwUserIndex, const ioHashString &rkUserNick );
	void DeleteTeamUserData( DWORD dwTeamIndex, DWORD dwUserIndex );
	void DeleteTournamentTeam( DWORD dwTourIndex );

public:
	void UpdateCheerPoint( DWORD dwTeamIdx, int iCheerPoint );

public:
	static ioTournamentTeamManager& GetSingleton();

public:   
	ioTournamentTeamManager();
	virtual ~ioTournamentTeamManager();
};
#define g_TournamentTeamMgr ioTournamentTeamManager::GetSingleton()
