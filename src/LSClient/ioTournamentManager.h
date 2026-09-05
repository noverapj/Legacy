#pragma once

#include "TournamentNode.h"

class SP2Packet;
typedef std::vector< TournamentNode * > TournamentVec;
class ioTournamentManager : public Singleton< ioTournamentManager >
{
public:
	enum
	{
		REGULAR_TEAM_CREATE_TIMER = 30000,
	};

protected:
	TournamentVec m_TournamentList;

protected:		// UI Helper
	DWORD m_dwRegularTeamCreateUITimer;               // 팀 생성창 자동 Show
	DWORD m_dwRegularTourRequestTime;

public:
	void Initialize();
	void Process();

public:           // 정규 대회
	DWORD GetRegularIndex();
	int GetRegularState();
	int GetRegularResourceType();
	bool IsRegularDisableTournament();
	const ioHashString& GetRegularDisableTournamentMent();

public:
	void CreateTournamentRegular( DWORD dwIndex, int iRegularResourceType );
	void CreateTournamentCustom( DWORD dwIndex, DWORD dwOwnerIndex, DWORD dwBannerB, DWORD dwBannerS );
	void CreateTournament( DWORD dwIndex );

public:
	void ApplyTournamentRegularInfo( SP2Packet &rkPacket );
	void ApplyTournamentMainInfo( SP2Packet &rkPacket );
	void ApplyTournamentScheduleInfo( SP2Packet &rkPacket );
	void ApplyTournamentRoundTeamData( SP2Packet &rkPacket );

public:
	TournamentNode *GetTournament( DWORD dwIndex, bool bDataSyncCheck = true );

public:
	void SetRegularTeamCreateCampFailed(){ m_dwRegularTeamCreateUITimer = FRAMEGETTIME(); }
	bool IsRegularTeamCreateUIShow();
	void CheckRegularTournaemntRequest();

public:
	static ioTournamentManager& GetSingleton();

public:   
	ioTournamentManager();
	virtual ~ioTournamentManager();
};
#define g_TournamentMgr ioTournamentManager::GetSingleton()
