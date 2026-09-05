#pragma once

class SP2Packet;
// 나 이외의 팀원
struct TeamUserData
{
	DWORD		 m_dwTableIndex;
	DWORD		 m_dwUserIndex;
	ioHashString m_szNick;

	TeamUserData()
	{
		m_dwTableIndex = m_dwUserIndex = 0;
	}
};
typedef std::vector< TeamUserData > TeamUserVec;

struct TeamData
{
	DWORD m_dwTourIndex;
	DWORD m_dwTeamIndex;
	DWORD m_dwTeamOwnerIndex;
	ioHashString m_szTeamName;
	TeamUserVec  m_TeamUserList;
	
	TeamData()
	{
		m_dwTourIndex = m_dwTeamIndex = m_dwTeamOwnerIndex = 0;
	}
};
typedef std::vector< TeamData > TeamDataVec;

struct CheerTeamInfo
{
	DWORD m_dwTourIndex;
	DWORD m_dwTeamIndex;
	int	  m_iCheerCnt;
	int	  m_iMyCheerTeamPage;

	CheerTeamInfo()
	{
		m_dwTourIndex = 0;
		m_dwTeamIndex = 0;
		m_iCheerCnt   = 0;
		m_iMyCheerTeamPage = -1;
	}
};
typedef std::vector< CheerTeamInfo > CheerTeamInfoVec;

class ioUserTournament
{
protected:
	TeamDataVec m_TeamDataList;
	CheerTeamInfoVec m_CheerList;

protected:
	bool m_bUpdateMainUI;
	bool m_bUpdateSubUI;

public:
	void ApplyTeamData( SP2Packet &rkPacket );
	void ApplyUserData( DWORD dwTeamIndex, SP2Packet &rkPacket );
	void CreateTeamData( SP2Packet &rkPacket );
	void JoinTeamData( SP2Packet &rkPacket );

public:
	TeamData &GetTeamData( DWORD dwTeamIndex );
	DWORD GetTeamIndex( DWORD dwTourIndex );
	TeamUserData GetTeamUserData( DWORD dwTeamIndex, const ioHashString &rkName );
	ioHashString &GetTeamUserName( DWORD dwTeamIndex, DWORD dwUserIndex );
	void  GetTeamList( TeamDataVec &rkTeamList );
	int   GetCurrentTeamUserCount( DWORD dwTourIndex );

public:
	bool IsTourTeam( DWORD dwTourIndex );
	bool IsAlreadyTeam( DWORD dwTeamIndex );
	bool IsAlreadyUser( DWORD dwTeamIndex, DWORD dwUserIndex );

public:
	void SetTeamUserAdd( DWORD dwTeamIndex, SP2Packet &rkPacket ); 

public:
	void DeleteTeam( DWORD dwTeamIndex );
	bool TournamentEndDeleteTeam( DWORD dwTourIndex );
	void LeaveTeamUser( DWORD dwTeamIndex, DWORD dwLeaveIndex );

public:
	bool IsUpdateMainUI();
	bool IsUpdateSubUI();

public:
	bool IsCheerTeam( DWORD dwTourIdx, DWORD dwTeamIdx );
	bool IsCheerTournament( DWORD dwTourIdx );

	void AddCheerTeamList( DWORD dwTourIdx, DWORD dwTeamIdx );
	void UpdateCheerTeamPage( DWORD dwTourIdx, int iUpdatePage );

	const CheerTeamInfo& GetCheerTeam( DWORD dwTourIdx );
	CheerTeamInfo& GetCheerTeamNotConst( DWORD dwTourIdx );

	void DeleteCheerTeam( DWORD dwTourIdx );

public:
	ioUserTournament();
	virtual ~ioUserTournament();
};