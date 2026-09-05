
#ifndef _ioUserTournament_h_
#define _ioUserTournament_h_

#include "ioDBDataController.h"

enum CheerRewardType
{
	CRT_CHAMP	= 1,
	CRT_PREDICT = 2,
};

class ioUserTournament : public ioDBDataController
{
public:
	// - 테이블인덱스, 유저인덱스, 닉네임, 레벨, 진영포인트, 길드인덱스
	struct TeamUserData
	{
		DWORD		 m_dwTableIndex;
		DWORD		 m_dwUserIndex;
		ioHashString m_szNick;

		// 아래 데이터는 동기화하지 않는다 - 실시간 동기화에 포함되어있음.
		int			 m_iGradeLevel;
		int          m_iLadderPoint;
		DWORD        m_dwGuildIndex;

		//
		BYTE         m_CampPos;

		TeamUserData()
		{
			m_dwTableIndex = 0;
			m_dwUserIndex  = 0;
			m_iGradeLevel  = 0;
			m_iLadderPoint = 0;
			m_dwGuildIndex = 0;
			m_CampPos      = 0;
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

		// 대전 매칭을 위해 꼭 필요한 정보 - 메인 서버와 동일해야한다.
		SHORT m_Position;          // 해당 강에서 위치
		BYTE  m_TourPos;           // 0은 탈락 & 예비 엔트리 / 1은 256강 / 2는 128강 ..
		SHORT m_InvitePosition;    // 초대 받은 경우는 해당 포지션이 세팅됨.

		TeamData()
		{
			m_TourPos = 0;
			m_Position = m_InvitePosition = 0;
			m_dwTourIndex = m_dwTeamIndex = m_dwTeamOwnerIndex = 0;
		}
	};
	typedef std::vector< TeamData > TeamDataVec;

public:
	struct CheerTeamInfo
	{
		DWORD m_dwTourIndex;
		DWORD m_dwTeamIndex;

		CheerTeamInfo()
		{
			m_dwTourIndex = 0;
			m_dwTeamIndex = 0;
		}
	};
	typedef std::vector< CheerTeamInfo > CheerTeamInfoVec;

public:
	struct RegularRewardDBData
	{
		DWORD m_dwTableIndex;
		DWORD m_dwStartDate;

		BYTE  m_TourPos;

		int   m_iMyCampPos;
		int   m_iWinCampPos;
		int   m_iLadderBonusPeso;
		int   m_iLadderRank;
		int   m_iLadderPoint;

		RegularRewardDBData()
		{
			Clear();
		}

		void Clear()
		{
			m_dwTableIndex		= 0;
			m_dwStartDate		= 0;

			m_TourPos			= 0;
			m_iMyCampPos		= CAMP_NONE;
			m_iWinCampPos		= CAMP_NONE;
			m_iLadderBonusPeso	= 0;
			m_iLadderRank		= 0;
			m_iLadderPoint		= 0;
		}
	};

protected:
	TeamDataVec m_TeamDataList;
	CheerTeamInfoVec m_CheerList;
	RegularRewardDBData m_RegularRewardDBData;

public:
	bool IsAlreadyTeam( DWORD dwTeamIndex );
	bool IsInviteCheckTeamSend( DWORD dwBlueIndex, DWORD dwRedIndex );
	bool IsInviteCheckTeamSend( DWORD dwTeamIndex );
	bool IsTourTeam( DWORD dwTourIndex );

public:
	ioUserTournament::TeamData &GetTeamData( DWORD dwTeamIndex );
	ioUserTournament::TeamData &GetTournamentTeamData( DWORD dwTourIndex );

public:
	virtual void Initialize( User *pUser );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );
	
	//사용 안함
	virtual bool DBtoNewIndex( DWORD dwIndex ){ return false; }
	virtual void SaveData(){}

public:
	void DBtoUserData( DWORD dwTeamIndex, CQueryResultData *query_data );

public:
	bool CreateTeamData( DWORD dwTourIndex, DWORD dwTeamIndex, DWORD dwOwnerIndex, ioHashString &rkTeamName, SHORT Position, BYTE TourPos );
	bool JoinTeamData( DWORD dwTourIndex, DWORD dwTeamIndex, DWORD dwOwnerIndex, const ioHashString &rkTeamName, SHORT Position, BYTE TourPos );
	void DeleteTeamData( DWORD dwTeamIndex );
	bool TournamentEndDeleteTeam( DWORD dwTourIndex );
	void TournamentTeamPosSync( DWORD dwTeamIndex, SHORT Position, BYTE TourPos, bool bSync );

public:
	void LeaveTeamUserNode( DWORD dwTeamIndex, DWORD dwLeaveUserIndex );
	void LeaveTeamUser( DWORD dwSenderIndex, DWORD dwTeamIndex, DWORD dwLeaveUserIndex );

public:
	bool IsAlreadyUser( ioUserTournament::TeamData &rkTeamData, DWORD dwUserIndex );
	void AddTeamUserData( DWORD dwTeamIndex, ioUserTournament::TeamUserData &rkUserData );
	void AddTeamUserAgreeServerSync( ioUserTournament::TeamData &rkTeamData, ioUserTournament::TeamUserData &rkAddData );

public:
	void DBtoCheerData( CQueryResultData *query_data );
	void TournamentEndDeleteCheerData( CQueryResultData *query_data );
	void TournamentEndDeleteCheerData( DWORD dwTourIndex );

public:
	bool IsCheerTeam( DWORD dwTourIndex );

public:
	void SetRegularRewardData( DWORD dwTableIndex, DWORD dwStartDate, BYTE TourPos, int iMyCampPos, int iWinCampPos, int iLadderBonusPeso, int iLadderRank, int iLadderPoint );
	inline const RegularRewardDBData& GetRegularRewardData(){ return m_RegularRewardDBData; }
	inline void ClearRegularRewardData(){ m_RegularRewardDBData.Clear(); }
public:
	ioUserTournament();
	virtual ~ioUserTournament();
};

#endif